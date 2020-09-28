/*++

Copyright (c) 2004  Microsoft Corporation

Module Name:

    roothub.c

Abstract:

    root hub emultation code for usbport driver

Environment:

    kernel mode only

Notes:

Revision History:

    06-21-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "roothub.tmh"
#endif

#define RH_STANDARD_REQ    0
#define RH_CLASS_REQ       1

#define MIN(x, y)  (((x)<(y)) ? (x) : (y))

//
// HUB feature selectors
//
#define C_HUB_LOCAL_POWER           0
#define C_HUB_OVER_CURRENT          1
#define PORT_CONNECTION             0
#define PORT_ENABLE                 1
#define PORT_SUSPEND                2
#define PORT_OVER_CURRENT           3
#define PORT_RESET                  4
#define PORT_POWER                  8
#define PORT_LOW_SPEED              9
#define C_PORT_CONNECTION           16
#define C_PORT_ENABLE               17
#define C_PORT_SUSPEND              18
#define C_PORT_OVER_CURRENT         19
#define C_PORT_RESET                20


#define HUB_REQUEST_GET_STATUS      0
#define HUB_REQUEST_CLEAR_FEATURE   1
#define HUB_REQUEST_GET_STATE       2
#define HUB_REQUEST_SET_FEATURE     3
#define HUB_REQUEST_GET_DESCRIPTOR  6
#define HUB_REQUEST_SET_DESCRIPTOR  7

// recipient codes in the bRequestType field
#define RECIPIENT_DEVICE      0
#define RECIPIENT_INTRFACE    1
#define RECIPIENT_ENDPOINT    2
#define RECIPIENT_PORT        3

// Descriptor Templates

// the following structures are emulated the same
// way for all port drivers

// default device descriptor
UCHAR RH_DeviceDescriptor[] = {0x12, //bLength
                               0x01, //bDescrpitorType
                               0x00, 0x01, //bcdUSB
                               0x09, //bDeviceClass
                               0x01, //bDeviceSubClass
                               0x00, //bDeviceProtocol
                               0x08, //bMaxPacketSize0
                               0x00, 0x00, //idVendor
                               0x00, 0x00, //idProduct
                               0x00, 0x00, //bcdDevice
                               0x00, //iManufacturer
                               0x00, //iProduct
                               0x00, //iSerialNumber
                               0x01};//bNumConfigurations

typedef struct _RH_CONFIGURATION_DESCRIPTOR {
    USB_CONFIGURATION_DESCRIPTOR    ConfigurationDescriptor;
    USB_INTERFACE_DESCRIPTOR        InterfaceDescriptor;
    USB_ENDPOINT_DESCRIPTOR         EndpointDescriptor;
} RH_CONFIGURATION_DESCRIPTOR, *PRH_CONFIGURATION_DESCRIPTOR;

const RH_CONFIGURATION_DESCRIPTOR RH_ConfigurationDescriptor = {
                         /* Config Descriptor   */
                        {0x09,   //bLength
                         0x02,   //bDescriptorType
                         0x0019, //wTotalLength
                         0x01,   //bNumInterfaces
                         0x23,   //iConfigurationValue
                         0x00,   //iConfiguration
                         0x40,   //bmAttributes
                         0x00},  //MaxPower

                         /* Interface Descriptor */
                        {0x09,   //bLength
                         0x04,   //bDescriptorType
                         0x00,   //bInterfaceNumber
                         0x00,   //bAlternateSetting
                         0x01,   //bNumEndpoints
                         0x09,   //bInterfaceClass
                         0x01,   //bInterfaceSubClass
                         0x00,   //bInterfaceProtocol
                         0x00},  //iInterface

                         /* Endpoint Descriptor  */
                        {0x07,   //bLength
                         0x05,   //bDescriptorType
                         0x81,   //bEndpointAddress
                         0x03,   //bmAttributes
                         0x0008, //wMaxPacketSize
                         0x0a}}; //bInterval

const UCHAR RH_HubDescriptor[] =
                      {0x09,  //bLength
                       0x29,  //bDescriptorType
                       0x00,  //bNbrPorts
                       0x00, 0x00, //wHubCharacteristics
                       0x00,  // bPwrOn2PwrGood
                       0x00};  // bHubContrCurrent


#define RH_DEV_TO_HOST      1
#define RH_HOST_TO_DEV      0


VOID
MPRH_DisableIrq(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in ULONG RootHubLockStatus
    )
{
    KIRQL irql = PASSIVE_LEVEL, isrIrql;

    ASSERT_FDOEXT(HcFdoExt);

    if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
        return;
    }

    if (RootHubLockStatus == ROOTHUB_LOCK_NOT_ACQUIRED) {
        ACQUIRE_ROOTHUB_LOCK(HcFdoExt, &irql);
    }

    DbgTrace((UsbpRootHub,"MPRH_DisableIrq \n"));

    // this needs to synchronize with the isr Dpc
    KeAcquireSpinLock(&HcFdoExt->Fdo.IsrDpcSpin, &isrIrql);
    MPF(HcFdoExt).MINIPORT_RH_DisableIrq(HcFdoExt->Fdo.MiniportDeviceData);
    HcFdoExt->Fdo.Flags.RhIrqEnabled = 0;
    KeReleaseSpinLock(&HcFdoExt->Fdo.IsrDpcSpin, isrIrql);

    if (RootHubLockStatus == ROOTHUB_LOCK_NOT_ACQUIRED) {
        RELEASE_ROOTHUB_LOCK(HcFdoExt, irql);
    }
}


VOID
MPRH_EnableIrq(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in ULONG RootHubLockStatus
    )
{
    KIRQL irql = PASSIVE_LEVEL, isrIrql;

    ASSERT_FDOEXT(HcFdoExt);

    if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
        return;
    }

    if (RootHubLockStatus == ROOTHUB_LOCK_NOT_ACQUIRED) {
        ACQUIRE_ROOTHUB_LOCK(HcFdoExt, &irql);
    }

    DbgTrace((UsbpRootHub,"MPRH_EnableIrq \n"));

    KeAcquireSpinLock(&HcFdoExt->Fdo.IsrDpcSpin, &isrIrql);
    MPF(HcFdoExt).MINIPORT_RH_EnableIrq(HcFdoExt->Fdo.MiniportDeviceData);
    HcFdoExt->Fdo.Flags.RhIrqEnabled = 1;
    KeReleaseSpinLock(&HcFdoExt->Fdo.IsrDpcSpin, isrIrql);

    if (RootHubLockStatus == ROOTHUB_LOCK_NOT_ACQUIRED) {
        RELEASE_ROOTHUB_LOCK(HcFdoExt, irql);
    }
}


RHSTATUS
USBPORT_RootHub_PortRequest(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PORT_OPERATION PortOperation
    )
/*++

Routine Description:

    Process a standard command sent on the control endpoint
    of the root hub.

Arguments:

    SetupPacket - pointer to a SetupPacket packet

Return Value:

    Root Hub status code.

--*/
{
    USHORT  PortNumber;
    RHSTATUS rhStatus = RH_STALL;
    PDEVICE_EXTENSION devExt, rhDevExt;
    PUSBPORT_REGISTRATION_PACKET RegPacket;
    KIRQL irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in>\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    rhDevExt = USBPORT_GetRootHubPdoExt(FdoDeviceObject);
    USBPORT_ASSERT(rhDevExt != NULL);

    PortNumber = SetupPacket->wIndex.W;

    RegPacket = &REGISTRATION_PACKET(devExt);

    USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
    LOGENTRY(FdoDeviceObject, LOG_RH, 'rSCM', SetupPacket, 0, 0);

    if (PortNumber > 0 &&
        PortNumber <= NUMBER_OF_PORTS(rhDevExt)) {

        USB_MINIPORT_STATUS mpStatus;

        if (devExt->Fdo.HcDisconnectRootHubPorts) {
            mpStatus = USBMP_STATUS_SUCCESS;

            if (PortNumber <= MAX_ROOT_HUB_PORTS) {

                switch (PortOperation) {
                    case SetFeaturePortReset:
                        break;
                    case SetFeaturePortPower:
                        break;
                    case SetFeaturePortEnable:
                        break;
                    case SetFeaturePortSuspend:
                        break;
                    case ClearFeaturePortEnable:
                        break;
                    case ClearFeaturePortPower:
                        break;
                    case ClearFeaturePortConnectChange:
                        // Clear the connect change simulated by
                        // USBPORT_HcDisconnectRootHubPorts() or by
                        // USBPORTSVC_InvalidateRootHub()
                        devExt->Fdo.HcDisconnectRootHubPortStatus[PortNumber - 1].ConnectChange = 0;
                        // Also clear a real hardware connect change in
                        // case one exists.
                        mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortConnectChange(
                            devExt->Fdo.MiniportDeviceData,
                            PortNumber);
                        break;
                    case ClearFeaturePortResetChange:
                        break;
                    case ClearFeaturePortEnableChange:
                        break;
                    case ClearFeaturePortSuspend:
                        break;
                    case ClearFeaturePortSuspendChange:
                        break;
                    case ClearFeaturePortOvercurrentChange:
                        break;
                    default:
                        mpStatus = USBMP_STATUS_FAILURE;
                        break;
                }
            }

        } else {

            switch(PortOperation) {
                case SetFeaturePortReset:
                    mpStatus = RegPacket->MINIPORT_RH_SetFeaturePortReset(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case SetFeaturePortPower:
                    if (USBPORT_IS_USB20(devExt)) {
                        USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
                        mpStatus = USBPORT_RootHub_PowerUsb2Port(FdoDeviceObject,
                            PortNumber);
                        USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
                    } else {
                        mpStatus = RegPacket->MINIPORT_RH_SetFeaturePortPower(
                            devExt->Fdo.MiniportDeviceData,
                            PortNumber);
                    }
                    break;
                case SetFeaturePortEnable:
                    mpStatus = RegPacket->MINIPORT_RH_SetFeaturePortEnable(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case SetFeaturePortSuspend:
                    mpStatus = RegPacket->MINIPORT_RH_SetFeaturePortSuspend(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case ClearFeaturePortEnable:
                    mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortEnable(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case ClearFeaturePortPower:
                    mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortPower(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case ClearFeaturePortConnectChange:
                    if (PortNumber <= MAX_ROOT_HUB_PORTS) {
                        // Clear the connect change simulated by
                        // USBPORT_HcReconnectRootHubPorts().
                        devExt->Fdo.HcDisconnectRootHubPortStatus[PortNumber - 1].ConnectChange = 0;
                    }
                    mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortConnectChange(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case ClearFeaturePortResetChange:
                    mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortResetChange(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case ClearFeaturePortEnableChange:
                    mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortEnableChange(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case ClearFeaturePortSuspend:
                    mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortSuspend(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case ClearFeaturePortSuspendChange:
                    mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortSuspendChange(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                case ClearFeaturePortOvercurrentChange:
                    mpStatus = RegPacket->MINIPORT_RH_ClearFeaturePortOvercurrentChange(
                        devExt->Fdo.MiniportDeviceData,
                        PortNumber);
                    break;
                default:
                    mpStatus = USBMP_STATUS_FAILURE;
                    TEST_TRAP();
            }
        }

        rhStatus = MPSTATUS_TO_RHSTATUS(mpStatus);

    } else {
        rhStatus = RH_STALL;
        TEST_TRAP();
    }

    USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
    DbgTrace((UsbpRootHub,"#%!FUNC!<out> %08.8x\n", rhStatus));
    return rhStatus;
}


RHSTATUS
USBPORT_RootHub_HubRequest(
    PDEVICE_OBJECT FdoDeviceObject,
    PORT_OPERATION PortOperation
    )
/*++

Routine Description:

    Process a standard command sent on the control endpoint
    of the root hub.

Arguments:

    SetupPacket - pointer to a SetupPacket packet

Return Value:

    Root Hub status code.

--*/
{
    RHSTATUS rhStatus = RH_STALL;
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    KIRQL irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in>\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_RH, 'hSCM', 0, 0, 0);

    switch(PortOperation) {
    case ClearFeaturePortOvercurrentChange:
        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        mpStatus = MPF(devExt).MINIPORT_RH_ClearFeaturePortOvercurrentChange(
            devExt->Fdo.MiniportDeviceData,
            0);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
        rhStatus = MPSTATUS_TO_RHSTATUS(mpStatus);
        break;
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> %08.8x\n", rhStatus));
    return rhStatus;

}


RHSTATUS
USBPORT_RootHub_StandardCommand(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUCHAR Buffer,
    PULONG BufferLength
    )
/*++

Routine Description:

    Process a standard command sent on the control endpoint
    of the root hub.

Arguments:

    SetupPacket - pointer to a SetupPacket packet

Return Value:

    Root Hub status code.

--*/
{
    PVOID descriptor = NULL;
    ULONG length;
    RHSTATUS rhStatus = RH_STALL;
    PDEVICE_EXTENSION devExt, rhDevExt;
    KIRQL irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in>\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    rhDevExt = USBPORT_GetRootHubPdoExt(FdoDeviceObject);
    USBPORT_ASSERT(rhDevExt != NULL);

    LOGENTRY(FdoDeviceObject, LOG_RH, 'rSCM', SetupPacket, 0, 0);

    //
    // switch on the command
    //
    switch (SetupPacket->bRequest) {
    case USB_REQUEST_SET_ADDRESS:
        //
        //
        //
        if (SetupPacket->wIndex.W == 0 &&
            SetupPacket->wLength == 0 &&
            SetupPacket->bmRequestType.Dir == RH_HOST_TO_DEV) {
            rhDevExt->Pdo.RootHubDeviceHandle->DeviceAddress =
                    (UCHAR)SetupPacket->wValue.W;

            rhStatus = RH_SUCCESS;
            LOGENTRY(FdoDeviceObject, LOG_RH, 'rSAD', 0, 0, 0);
        }
        break;

    case USB_REQUEST_GET_DESCRIPTOR:
        {
            ULONG siz = 0;
            UCHAR descriptorIndex, descriptorType;

            descriptorType = (UCHAR) SetupPacket->wValue.HiByte;
            descriptorIndex = (UCHAR) SetupPacket->wValue.LowByte;

            switch (descriptorType) {

            case USB_DEVICE_DESCRIPTOR_TYPE:
                if (descriptorIndex == 0 &&
                    SetupPacket->bmRequestType.Dir == RH_DEV_TO_HOST) {

                    siz = sizeof(RH_DeviceDescriptor);
                    // use PDO specific copy
                    descriptor = rhDevExt->Pdo.DeviceDescriptor;
                    LOGENTRY(FdoDeviceObject, LOG_RH, 'rGDS', descriptor, siz, 0);
                }
                break;

            case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                if (descriptorIndex == 0 &&
                    SetupPacket->bmRequestType.Dir == RH_DEV_TO_HOST) {
                    siz = sizeof(RH_ConfigurationDescriptor);
                    // use pdo specific copy
                    descriptor = rhDevExt->Pdo.ConfigurationDescriptor;
                    LOGENTRY(FdoDeviceObject, LOG_RH, 'rGCS', descriptor, siz, 0);
                }
                break;

            //
            // BUGBUG these descriptor types not handled
            //
            case USB_STRING_DESCRIPTOR_TYPE:
                // we will stall
                TEST_TRAP();
            default:
                // we will stall
                TEST_TRAP();
            } /* descriptorType */

            if (descriptor) {

                length = MIN(*BufferLength, siz);

                RtlCopyMemory(Buffer, descriptor, length);
                *BufferLength = length;
                rhStatus = RH_SUCCESS;
            }
        }
        break;

    case USB_REQUEST_GET_STATUS:
        //
        // get_device_status
        //
        // report that we are self powered
        //
        // BUGBUG
        // are we self powered?
        // are we a remote wakeup source?
        //
        // see section 9.4.5 USB 1.0 spec
        //
        {
        PUSHORT status = (PUSHORT) Buffer;

        if (*BufferLength < sizeof(USHORT)) {
            USBPORT_ASSERTMSG("BufferLength expected to be atleast 2", FALSE);
            break;
        }

        if (SetupPacket->wValue.W == 0 &&   //mbz
            SetupPacket->wLength == 2 &&
            SetupPacket->wIndex.W == 0 &&   //device
            SetupPacket->bmRequestType.Dir == RH_DEV_TO_HOST) {

            USB_MINIPORT_STATUS mpStatus;

            MPRH_GetStatus(devExt, status, mpStatus);

            *BufferLength = sizeof(*status);

            rhStatus = MPSTATUS_TO_RHSTATUS(mpStatus);
        }
        }
        break;

    case USB_REQUEST_GET_CONFIGURATION:
        //
        // get_device_configuration
        //
        if (SetupPacket->wValue.W == 0 &&   //mbz
            SetupPacket->wIndex.W == 0 &&   //mbz
            SetupPacket->wLength == 1 &&
            SetupPacket->bmRequestType.Dir == RH_DEV_TO_HOST) {

            length = MIN(*BufferLength, sizeof(rhDevExt->Pdo.ConfigurationValue));
            RtlCopyMemory(Buffer, &rhDevExt->Pdo.ConfigurationValue, length);
            *BufferLength = length;
            rhStatus = RH_SUCCESS;
        }
        break;

    case USB_REQUEST_CLEAR_FEATURE:

        if (SetupPacket->wIndex.W == 0 &&   // index 0
            SetupPacket->wLength == 0 &&    // mbz
            SetupPacket->bmRequestType.Dir == RH_HOST_TO_DEV &&
            SetupPacket->wValue.W == USB_FEATURE_REMOTE_WAKEUP) {

            USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
            rhStatus = MPf_SetWakeOnConnect(devExt, 0);
            USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
        }   // else stall
        break;

    case USB_REQUEST_SET_CONFIGURATION:
        {
        PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor =
            (PUSB_CONFIGURATION_DESCRIPTOR)&RH_ConfigurationDescriptor.ConfigurationDescriptor;

        if (SetupPacket->wIndex.W == 0 &&   // mbz
            SetupPacket->wLength == 0 &&  // mbz
            SetupPacket->bmRequestType.Dir == RH_HOST_TO_DEV &&
            (SetupPacket->wValue.W ==
                configurationDescriptor->bConfigurationValue ||
             SetupPacket->wValue.W == 0)) {

            rhDevExt->Pdo.ConfigurationValue =
                (UCHAR) SetupPacket->wValue.W;
            rhStatus = RH_SUCCESS;
            LOGENTRY(FdoDeviceObject, LOG_RH, 'rSEC',
                rhDevExt->Pdo.ConfigurationValue, 0, 0);
        }
        }
        break;

    case USB_REQUEST_SET_FEATURE:

        if (SetupPacket->wIndex.W == 0 &&   // index 0
            SetupPacket->wLength == 0 &&    // mbz
            SetupPacket->bmRequestType.Dir == RH_HOST_TO_DEV &&
            SetupPacket->wValue.W == USB_FEATURE_REMOTE_WAKEUP) {

            USBPORT_AcquireSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
            rhStatus = MPf_SetWakeOnConnect(devExt, 1);
            USBPORT_ReleaseSpinLock(devExt->HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
        }   // else stall

        break;
    //
    // these commands are optional for the hub
    //
    case USB_REQUEST_SET_DESCRIPTOR:
    case USB_REQUEST_SET_INTERFACE:
    case USB_REQUEST_GET_INTERFACE:
    case USB_REQUEST_SYNC_FRAME:
    default:
        // bad command, probably a bug in the
        // hub driver
        TEST_TRAP();
        break;
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> rhStatus = %08.8x\n", rhStatus));

    return rhStatus;
}


RHSTATUS
USBPORT_RootHub_ClassCommand(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUCHAR Buffer,
    PULONG BufferLength
    )
/*++

Routine Description:

    Process a hub class command to the root hub control endpoint.

Arguments:

Return Value:

    Root Hub status code.

--*/
{
    ULONG length;
    RHSTATUS rhStatus = RH_STALL;
    PDEVICE_EXTENSION devExt, rhDevExt;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    rhDevExt = USBPORT_GetRootHubPdoExt(FdoDeviceObject);
    USBPORT_ASSERT(rhDevExt != NULL);

    LOGENTRY(FdoDeviceObject, LOG_RH, 'rCCM', SetupPacket, 0, 0);

    //
    // switch on the command
    //

    switch (SetupPacket->bRequest) {
    case HUB_REQUEST_GET_STATUS:
        //
        //
        //
        if (SetupPacket->bmRequestType.Recipient == RECIPIENT_PORT) {
            //
            // get port status
            //
            PRH_PORT_STATUS portStatus;
            //
            // see if we have a valid request
            //

            if (Buffer != NULL &&
                SetupPacket->wIndex.W > 0 &&
                SetupPacket->wIndex.W <= NUMBER_OF_PORTS(rhDevExt) &&
                SetupPacket->wLength >= sizeof(*portStatus)) {

                USB_MINIPORT_STATUS mpStatus;

                USBPORT_ASSERT(sizeof(*portStatus) == 4);
                USBPORT_ASSERT(*BufferLength >= sizeof(*portStatus));
                portStatus = (PRH_PORT_STATUS) Buffer;
                RtlZeroMemory(Buffer, sizeof(*portStatus));

                MPRH_GetPortStatus(devExt,
                                   SetupPacket->wIndex.W,
                                   portStatus,
                                   mpStatus);

                rhStatus = MPSTATUS_TO_RHSTATUS(mpStatus);
            }
        } else {

            //
            // get hub status
            //
            USB_MINIPORT_STATUS mpStatus;
            PRH_HUB_STATUS hubStatus;

            if (Buffer != NULL) {
                USBPORT_ASSERT(sizeof(*hubStatus) == 4);
                USBPORT_ASSERT(*BufferLength >= sizeof(*hubStatus));
                hubStatus = (PRH_HUB_STATUS) Buffer;
                RtlZeroMemory(Buffer, sizeof(*hubStatus));

                MPRH_GetHubStatus(devExt, hubStatus, mpStatus);

                LOGENTRY(FdoDeviceObject, LOG_RH, 'rGHS',
                    *((PULONG) hubStatus), 0, 0);

                rhStatus = MPSTATUS_TO_RHSTATUS(mpStatus);
            }

        }
        break;

    case HUB_REQUEST_CLEAR_FEATURE:
        //
        // Hub/Port Clear Feature
        //
        LOGENTRY(FdoDeviceObject, LOG_RH, 'rCFR',
            SetupPacket->bmRequestType.Recipient, 0, 0);
        if (SetupPacket->bmRequestType.Recipient == RECIPIENT_PORT) {
            //
            // clear port feature
            //
            LOGENTRY(FdoDeviceObject, LOG_RH, 'rCPR',
                SetupPacket->wValue.W, 0, 0);
            switch(SetupPacket->wValue.W) {
                //
                //
                //
            case PORT_ENABLE:

                // disable the port
                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                ClearFeaturePortEnable);

                LOGENTRY(FdoDeviceObject, LOG_RH, 'rDsP',
                    SetupPacket->wIndex.W, 0, rhStatus);
                break;

            case PORT_POWER:

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                ClearFeaturePortPower);

                LOGENTRY(FdoDeviceObject, LOG_RH, 'rDpP',
                    SetupPacket->wIndex.W, 0, rhStatus);
                break;
            //
            // the following are not valid commands,
            // return a stall since that is most likely
            // what a real hub would do
            //
            case PORT_CONNECTION:
            case PORT_OVER_CURRENT:
            case PORT_LOW_SPEED:
            case PORT_RESET:
                TEST_TRAP();
                break;

            case C_PORT_CONNECTION:

                 rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                ClearFeaturePortConnectChange);
                LOGENTRY(FdoDeviceObject, LOG_RH, 'cfCC',
                        SetupPacket->wIndex.W, 0, rhStatus);
                break;

            case C_PORT_ENABLE:

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                ClearFeaturePortEnableChange);
                LOGENTRY(FdoDeviceObject, LOG_RH, 'cfEC',
                        SetupPacket->wIndex.W, 0, rhStatus);

                break;

            case C_PORT_RESET:

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                ClearFeaturePortResetChange);
                LOGENTRY(FdoDeviceObject, LOG_RH, 'cfRC',
                        SetupPacket->wIndex.W, 0, rhStatus);

                break;

            case PORT_SUSPEND:

                // clearing port suspend generates resume signalling
                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                ClearFeaturePortSuspend);
                LOGENTRY(FdoDeviceObject, LOG_RH, 'cfPS',
                        SetupPacket->wIndex.W, 0, rhStatus);

                break;

            case C_PORT_SUSPEND:

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                ClearFeaturePortSuspendChange);
                LOGENTRY(FdoDeviceObject, LOG_RH, 'cfPS',
                        SetupPacket->wIndex.W, 0, rhStatus);

                break;

            case C_PORT_OVER_CURRENT:

                // overcuurent generated on NEC machines for ports with
                // no device attached.  We want to find out:
                // 1. Does the port still function - Yes
                // 2. Does the UI popup No
                // the overcurrent occurs on the port with no device connected
                // and the system in question has only one USB port.

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                ClearFeaturePortOvercurrentChange);
                LOGENTRY(FdoDeviceObject, LOG_RH, 'cfOC',
                        SetupPacket->wIndex.W, 0, rhStatus);

                break;
            default:
                TEST_TRAP();
            }
        } else {
            //
            // clear hub feature
            //
            LOGENTRY(FdoDeviceObject, LOG_RH, 'rCHR', SetupPacket->wValue.W, 0, 0);
            switch(SetupPacket->wValue.W) {
            case C_HUB_LOCAL_POWER:
                rhStatus = RH_SUCCESS;
                break;
            case C_HUB_OVER_CURRENT:
                rhStatus =
                    USBPORT_RootHub_HubRequest(FdoDeviceObject,
                                                ClearFeaturePortOvercurrentChange);
                break;
            default:
                TEST_TRAP();
            }
        }
        break;

    case HUB_REQUEST_GET_STATE:
        //
        //
        //
        TEST_TRAP();
        break;

    case HUB_REQUEST_SET_FEATURE:
        //
        //  Hub/Port feature request
        //
        if (SetupPacket->bmRequestType.Recipient == RECIPIENT_PORT) {
            //
            // set port feature
            //
            switch(SetupPacket->wValue.W) {
            case PORT_RESET:

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                SetFeaturePortReset);
                LOGENTRY(FdoDeviceObject, LOG_RH, 'sfPR',
                        SetupPacket->wIndex.W, 0, rhStatus);
                break;

            case PORT_SUSPEND:

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                SetFeaturePortSuspend);
                LOGENTRY(FdoDeviceObject, LOG_RH, 'sfPS',
                        SetupPacket->wIndex.W, 0, rhStatus);
                break;

            case PORT_ENABLE:

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                SetFeaturePortEnable);

                LOGENTRY(FdoDeviceObject, LOG_RH, 'sfPE',
                        SetupPacket->wIndex.W, 0, rhStatus);
                break;

            case PORT_POWER:

                rhStatus =
                    USBPORT_RootHub_PortRequest(FdoDeviceObject,
                                                SetupPacket,
                                                SetFeaturePortPower);

                LOGENTRY(FdoDeviceObject, LOG_RH, 'sfPP',
                        SetupPacket->wIndex.W, 0, rhStatus);
                break;
            case PORT_CONNECTION:
            case PORT_OVER_CURRENT:
            case PORT_LOW_SPEED:

            case C_PORT_CONNECTION:
            case C_PORT_ENABLE:
            case C_PORT_SUSPEND:
            case C_PORT_OVER_CURRENT:
            case C_PORT_RESET:
            default:
                TEST_TRAP();
            }
        } else {
            //
            // set hub feature
            //
            switch(SetupPacket->wValue.W) {
            case C_HUB_LOCAL_POWER:
            case C_HUB_OVER_CURRENT:
            default:
                TEST_TRAP();
            }

        }
        break;

    case HUB_REQUEST_GET_DESCRIPTOR:
        //
        // return the hub descriptor
        //
        if (Buffer != NULL &&
            rhDevExt != NULL &&
            (SetupPacket->wValue.HiByte == 0 ||
             SetupPacket->wValue.HiByte ==
                rhDevExt->Pdo.HubDescriptor->bDescriptorType) &&
            // we already know it is a class command
            SetupPacket->bmRequestType.Dir == RH_DEV_TO_HOST) {
            LOGENTRY(FdoDeviceObject, LOG_RH, 'rGHD', SetupPacket,
                SetupPacket->wLength, 0);
            length = MIN(*BufferLength, HUB_DESRIPTOR_LENGTH(rhDevExt));

            RtlCopyMemory(Buffer, rhDevExt->Pdo.HubDescriptor, length);
            *BufferLength = length;
            rhStatus = RH_SUCCESS;
        }
        break;

    case HUB_REQUEST_SET_DESCRIPTOR:
        //
        //
        //
        TEST_TRAP();
        break;

    default:
        // bad command
        TEST_TRAP();
        break;
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> rhStatus = %08.8x\n", rhStatus));
    return rhStatus;
}


VOID
USBPORT_SetBit(
    PVOID Bitmap,
    ULONG BitNumber
    )
/* ++

   Description:

   Set a bit in a given a string of bytes.

   Arguments:

   Return:

-- */
{
    ULONG dwordOffset;
    ULONG bitOffset;
    PULONG l = (PULONG) Bitmap;


    dwordOffset = BitNumber / 32;
    bitOffset = BitNumber % 32;

    l[dwordOffset] |= (1 << bitOffset);
}


RHSTATUS
USBPORT_RootHub_Endpoint1(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in PVOID IntPipeDataBuffer,
    __in ULONG IntPipeDataBufferLength
    )
/*++

Routine Description:

    Simulates an interrupt transfer.  This routine will copy the data from an abuffer to the an interrupt transfer urb.

Arguments:

    HcFdo - controller fdo

    Transfer - transfer to fill in

    EpRefHandle - handle for the root hub interrupt pipe.

    IntPipeDataBuffer - buffer filled in with int change data.

    IntPipeDataBufferLength - input: size of IntPipeDataBuffer data

Return Value:

    root hub transfer status code

--*/
{
    PDEVICE_EXTENSION devExt;
    RHSTATUS rhStatus;
    PTRANSFER_URB urb = NULL;
    PVOID buffer = NULL;

    ASSERT_TRANSFER(Transfer);

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));
    // we get here because we peeked a data change so default to success unless we see an error
    rhStatus = RH_SUCCESS;

    LOGENTRY(HcFdo, LOG_RH, 'Ep10', IntPipeDataBuffer, IntPipeDataBufferLength, 0);

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    do {
        if (devExt->Fdo.Flags.ControllerGone ||
            devExt->Fdo.PowerFlags.MpRestartFailed) {

            rhStatus = RH_NAK;
            break;
        }

        // validate the buffer length
        urb = Transfer->Urb;
        if (urb) {
            buffer = Transfer->Tp.TransferBufferLength ? urb->TransferBufferMDL->MappedSystemVa :  NULL;
        }

        if (buffer == NULL ||
            Transfer->Tp.TransferBufferLength < IntPipeDataBufferLength) {

            TEST_TRAP();
            LOGENTRY(HcFdo, LOG_RH, 'Ep11', IntPipeDataBufferLength, Transfer->Tp.TransferBufferLength, 0);

            rhStatus = RH_STALL;
            break;
        }

        // zero buffer in case hub driver did not
        RtlZeroMemory(buffer, Transfer->Tp.TransferBufferLength);

    }WHILE (0);

    switch (rhStatus) {
    case RH_NAK:
        if (urb) {
            urb->TransferBufferLength = 0;
        }
        LOGENTRY(HcFdo, LOG_RH, 'Ep12', urb, buffer, 0);

        DbgTrace((UsbpRootHub,"#%!FUNC! NAK \n"));
        break;

    case RH_SUCCESS:

        // set bytes transferred for this interrupt endpoint
        USBPORT_ASSERT(urb);

        urb->TransferBufferLength =
            Transfer->MiniportBytesTransferred = IntPipeDataBufferLength;

        RtlCopyMemory(buffer, IntPipeDataBuffer, IntPipeDataBufferLength);

        LOGENTRY(HcFdo, LOG_RH, 'Ep13', IntPipeDataBuffer, buffer, IntPipeDataBufferLength);

        DbgTrace((UsbpRootHub,"#%!FUNC! ACK bytes = %d\n", urb->TransferBufferLength));
        break;

    case RH_STALL:

        if (urb) {
            urb->TransferBufferLength = 0;
        }
        LOGENTRY(HcFdo, LOG_RH, 'Ep14', 0, 0, 0);

        DbgTrace((UsbpRootHub,"#%!FUNC! STALL \n"));
        TEST_TRAP();
        break;
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> %08.8x\n", rhStatus));
    return rhStatus;
}


RHSTATUS
USBPORT_RootHub_Endpoint1_Peek(
    __in PDEVICE_OBJECT HcFdo,
    __in PHCD_ENDPOINT Endpoint,
    __inout PVOID IntPipeDataBuffer,
    __inout PULONG IntPipeDataBufferLength
    )
/*++

Routine Description:

    simulates an interrupt transfer for the root hub.  This function takes a buffer and length and checks
    the root hub ports if a change is detected the appropriate bits are set in the buffer.  This function
    allows us to 'lookahead' without dequeing the irp.

Arguments:

    HcFdo - controller fdo

    Endpoint - (ref'ed) root hub interrupt endpoint.

    IntPipeDataBuffer - buffer filled in with int change data.

    IntPipeDataBufferLength - input: size of IntPipeDataBuffer.  output: nuber of data bytes needed to report the
    change data.

Return Value:

    root hub transfer status code:

    RH_NAK - no data to report

--*/
{
    PDEVICE_EXTENSION rhDevExt, devExt;
    RHSTATUS rhStatus;
    ULONG need;
    RH_HUB_STATUS hubStatus;
    USB_MINIPORT_STATUS mpStatus;
    ULONG i;
    KIRQL dm_irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));
    // assume no change
    rhStatus = RH_NAK;

    ASSERT_ENDPOINT(Endpoint);

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_ACQUIRE_DM_LOCK(devExt, dm_irql);

    if (devExt->Fdo.DmTimerFlags.DecodesOff) {
        // returning NAK if decodes are off
        USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);
        return rhStatus;

    }

    if (devExt->Fdo.Flags.ControllerGone ||
        devExt->Fdo.PowerFlags.MpRestartFailed) {

        USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);
        return rhStatus;
    }

    USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

    rhDevExt = USBPORT_GetRootHubPdoExt(HcFdo);
    if (rhDevExt== NULL) {
        return rhStatus;
    }

    // compute how many bytes do we need to report a status
    // change for any port
    // 0,1-7  = 1
    // 8-15 = 2

    need = (NUMBER_OF_PORTS(rhDevExt)/8)+1;

    if (IntPipeDataBuffer == NULL ||
        *IntPipeDataBufferLength < need) {

        TEST_TRAP();
        rhStatus = RH_STALL;
        goto USBPORT_RootHub_Endpoint1_Peek_Done;
    }

    // zero buffer in case hub driver did not
    RtlZeroMemory(IntPipeDataBuffer, *IntPipeDataBufferLength);

    // get the current port status and
    // construct a bitmask of changed ports

    // GTS USBPORT_ACQUIRE_DM_LOCK(devExt, dm_irql);

    if (!devExt->Fdo.DmTimerFlags.DecodesOff) {
        for (i=0; i< NUMBER_OF_PORTS(rhDevExt); i++) {

            RH_PORT_STATUS portStatus;

            // usb spec does not allow more than 255 ports
            USBPORT_ASSERT(i<256);
            MPRH_GetPortStatus(devExt, (USHORT)(i+1), &portStatus, mpStatus);

            LOGENTRY(HcFdo, LOG_RH, 'epk1', portStatus.ul,   mpStatus, i+1);

            if (mpStatus != USBMP_STATUS_SUCCESS ) {
                TEST_TRAP();
                rhStatus = RH_STALL;
                // GTS USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

                goto USBPORT_RootHub_Endpoint1_Peek_Done;
            }

            if (portStatus.ConnectChange ||
                portStatus.EnableChange ||
                portStatus.SuspendChange ||
                portStatus.OverCurrentChange ||
                portStatus.ResetChange) {

                USBPORT_SetBit(IntPipeDataBuffer, i+1);

                rhStatus = RH_SUCCESS;
            }
        }

        //
        // We created a bit map (base of 1 not 0) listing whether or not
        // change has occurred on any of the down stream ports of the
        // root hub.

        // Bit 0 is reserved for the status change of the hub itself.
        //

        MPRH_GetHubStatus(devExt, &hubStatus, mpStatus);

        if (mpStatus != USBMP_STATUS_SUCCESS ) {
            TEST_TRAP();
            rhStatus = RH_STALL;
            // GTS USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

            goto USBPORT_RootHub_Endpoint1_Peek_Done;
        }

        if (hubStatus.LocalPowerChange ||
            hubStatus.OverCurrentChange) {

            USBPORT_SetBit(IntPipeDataBuffer, 0);
            rhStatus = RH_SUCCESS;
        }
    }

    // GTS USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

    switch (rhStatus) {
    case RH_NAK:
        DbgTrace((UsbpRootHub,"#%!FUNC! NAK \n"));
        break;

    case RH_SUCCESS:

        // set bytes transferred for this interrupt endpoint
        *IntPipeDataBufferLength= need;
//        urb->TransferBufferLength =
//            Transfer->MiniportBytesTransferred =
//                Transfer->Tp.TransferBufferLength;

        DbgTrace((UsbpRootHub,"#%!FUNC! ACK bytes = %d\n", *IntPipeDataBufferLength));
        break;

    case RH_STALL:
        DbgTrace((UsbpRootHub,"#%!FUNC! STALL \n"));
        TEST_TRAP();
        break;
    }

    LOGENTRY(HcFdo, LOG_RH, 'epk2', IntPipeDataBuffer, *IntPipeDataBufferLength, rhStatus);

USBPORT_RootHub_Endpoint1_Peek_Done:

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> %08.8x\n", rhStatus));

    return rhStatus;
}



RHSTATUS
USBPORT_RootHub_Endpoint0(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

Arguments:

Return Value:

    root hub transfer status code

--*/
{
    RHSTATUS rhStatus;
    PTRANSFER_URB urb;
    PUSB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    PUCHAR buffer;
    ULONG bufferLength, mpOptionFlags = 0;
    PDEVICE_EXTENSION devExt;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_TRANSFER(Transfer);
    urb = Transfer->Urb;

    //
    // convert transfer buffer from MDL
    //
    buffer =
        Transfer->Tp.TransferBufferLength ?
            urb->TransferBufferMDL->MappedSystemVa :
            NULL;
    bufferLength = Transfer->Tp.TransferBufferLength;

    setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)
        &urb->u.SetupPacket[0];

    if (setupPacket->bmRequestType.Type == RH_STANDARD_REQ) {

        rhStatus =
            USBPORT_RootHub_StandardCommand(FdoDeviceObject,
                                            setupPacket,
                                            buffer,
                                            &bufferLength);
        if (rhStatus == RH_SUCCESS) {
            // set the return length
            Transfer->MiniportBytesTransferred = bufferLength;
        }

    } else if (setupPacket->bmRequestType.Type == RH_CLASS_REQ) {

        mpOptionFlags = devExt->Fdo.HcOptionFlags;

        //
        // For Wireless USB wire adapters, it is more efficient to
        // send the actual transfer to the miniport and let them complete
        // asynchronously.
        //
        if(TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_RH_ASYNC) &&
           setupPacket->bRequest != HUB_REQUEST_GET_DESCRIPTOR){

            rhStatus =
                USBPORT_RootHub_AsyncCommand(FdoDeviceObject,
                                             setupPacket,
                                             buffer,
                                             bufferLength,
                                             Transfer);

        } else {

            rhStatus =
                USBPORT_RootHub_ClassCommand(FdoDeviceObject,
                                             setupPacket,
                                             buffer,
                                             &bufferLength);

        }

        if (rhStatus == RH_SUCCESS) {
            // set the return length
            Transfer->MiniportBytesTransferred = bufferLength;
        }

    } else {
        rhStatus = RH_STALL;
        // probably a bug in the hub driver
        TEST_TRAP();
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> %08.8x\n", rhStatus));
    return rhStatus;
}


NTSTATUS
USBPORT_InitRootHubDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEVICE_DESCRIPTOR  RootHubDeviceDescriptor,
    PUSBD_DEVICE_HANDLE RootHubDeviceHandle
    )
/*++

Routine Description:

    Initialize a the device handle for the root hub. This is used on restart as
    well as start.

Arguments:

    RootHubDeviceHandle - allocated root hub device handle

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION devExt = NULL;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(RootHubDeviceHandle->Sig == SIG_DEVICE_HANDLE);
    USBPORT_ASSERT(RootHubDeviceDescriptor != NULL);

    RootHubDeviceHandle->DeviceAddress = 0;
    RootHubDeviceHandle->ConfigurationHandle = NULL;
    // define root hub speed based on the type of conroller
    if (USBPORT_IS_USB20(devExt)) {
        RootHubDeviceHandle->DeviceSpeed = UsbHighSpeed;
    } else {
        RootHubDeviceHandle->DeviceSpeed = UsbFullSpeed;
    }

    RootHubDeviceHandle->DeviceFlags = USBPORT_DEVICEFLAG_ROOTHUB;

    InitializeListHead(&RootHubDeviceHandle->PipeHandleList);
    InitializeListHead(&RootHubDeviceHandle->TtListHead);

    // copy device descriptor to device handle
    RtlCopyMemory(&RootHubDeviceHandle->DeviceDescriptor,
                  RootHubDeviceDescriptor,
                  sizeof(USB_DEVICE_DESCRIPTOR));

    // Store a self-reference DeviceHandle in the ETW device info.
    //
    RootHubDeviceHandle->EtwUsbDevice.DeviceHandle = RootHubDeviceHandle;

    // Store the Device Descriptor idVendor and idProduct in the ETW
    // device info.
    //
    RootHubDeviceHandle->EtwUsbDevice.idVendor  =
        RootHubDeviceHandle->DeviceDescriptor.idVendor;

    RootHubDeviceHandle->EtwUsbDevice.idProduct =
        RootHubDeviceHandle->DeviceDescriptor.idProduct;

    RootHubDeviceHandle->EtwUsbDevice.DeviceSpeed =
        RootHubDeviceHandle->DeviceSpeed;


    INITIALIZE_DEFAULT_PIPE(RootHubDeviceHandle->DefaultPipe,
                            USB_DEFAULT_MAX_PACKET);

    // init the default pipe
    nts = USBPORT_OpenEndpoint(RootHubDeviceHandle,
                               FdoDeviceObject,
                               &RootHubDeviceHandle->DefaultPipe,
                               NULL,
                               TRUE);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> %08.8x\n", nts));

    return nts;

}


NTSTATUS
USBPORT_CreateRootHubPdo(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT *RootHubPdo
    )
/*++

Routine Description:

    Attempt to create the root hub

Arguments:

    *RootHubPdo set to NULL if unsuccessful

Return Value:

    NTSTATUS

--*/
{
    ULONG i = 0;
    ULONG len = 0;
    ULONG index = 0;
    ULONG portbytes = 0;
    ULONG hubDescLen = 0;
    PUCHAR desc = NULL;
    PUCHAR descToFree = NULL;
    NTSTATUS ntStatus = STATUS_BOGUS;
    ROOTHUB_DATA hubData;
    UNICODE_STRING rootHubName;
    PDEVICE_OBJECT devObj = NULL;
    PDEVICE_EXTENSION devExt = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL;

    ASSERT_PASSIVE();

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    do {
        devObj = NULL;
        ntStatus = USBPORT_MakeRootHubPdoName(FdoDeviceObject,
                                              &rootHubName,
                                              index);
        if (!NT_SUCCESS(ntStatus)) {
            break;
        }

        ntStatus = IoCreateDevice(devExt->Fdo.MiniportDriver->DriverObject,
                                  sizeof(DEVICE_EXTENSION),
                                  &rootHubName,
                                  FILE_DEVICE_BUS_EXTENDER,
                                  FILE_DEVICE_SECURE_OPEN,
                                  FALSE,
                                  &devObj);
        index++;
        RtlFreeUnicodeString(&rootHubName);

    } while (ntStatus == STATUS_OBJECT_NAME_COLLISION);


    if (!NT_SUCCESS(ntStatus)) {
        goto Exit;
    }

    USBPORT_ASSERT(devObj != NULL);
    *RootHubPdo = devObj;
    rhDevExt = devObj->DeviceExtension;
    LOGENTRY(FdoDeviceObject, LOG_PNP, 'rPDO', devObj, rhDevExt, 0);

    rhDevExt->Sig = ROOTHUB_DEVICE_EXT_SIG;
    rhDevExt->HcFdoDeviceObject = FdoDeviceObject;

    USBPORT_Core_InitIoRequestCount(devObj);
    USBPORT_Core_IncIoRequestCount(devObj, &rhDevExt->UsbIoPnpRemove, rhDevExt, UsbIoTag_RhCreate, FALSE);

    // initialize root hub extension
    USBPORT_ComputeRootHubDeviceCaps(FdoDeviceObject, devObj);

    //SET_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_NOT_STARTED);
    //SET_FLAG(rhDevExt->PnpStateFlags, USBPORT_PNP_PRESENT);
    rhDevExt->PnpState = PnpNotStarted;

    rhDevExt->Pdo.RootHubDeviceHandle = USBPORT_AllocDeviceHandle(FdoDeviceObject);

    if (rhDevExt->Pdo.RootHubDeviceHandle == NULL) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean;
    }

    // root hub device is not configured yet
    rhDevExt->Pdo.ConfigurationValue = 0;

    // get root hub information from the miniport
    RtlZeroMemory(&hubData, sizeof(ROOTHUB_DATA));
    MPRH_GetRootHubData(devExt, &hubData);

    // use hub data to fabricate a hub descriptor
    // compute number of bytes to represent the ports
    // 0 = reserved
    // 1-7 ports  = 1 bytes
    // 8-15 ports = 2 bytes
    // 16-23 ports = 3 bytes
    // 24-31 ports = 4 bytes
    //...
    USBPORT_ASSERT(hubData.NumberOfPorts != 0);
    portbytes = ((((hubData.NumberOfPorts) / 8) + 1));

    // craft root hub descriptors
    hubDescLen = 7 + (portbytes * 2);
    len = sizeof(RH_DeviceDescriptor) +
          sizeof(RH_ConfigurationDescriptor) +
          hubDescLen;

    // allocate space for our descriptors
    ALLOC_POOL_Z(desc, NonPagedPoolNx, len);
    descToFree = desc;
    if (NULL == desc) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean;
    }
    LOGENTRY(FdoDeviceObject, LOG_RH, 'rhDS', desc, len, portbytes);
    rhDevExt->Pdo.Descriptors = desc;
    // set up device descriptor
    rhDevExt->Pdo.DeviceDescriptor = (PUSB_DEVICE_DESCRIPTOR)desc;
    RtlCopyMemory(rhDevExt->Pdo.DeviceDescriptor,
                  &RH_DeviceDescriptor[0],
                  sizeof(RH_DeviceDescriptor));
    // fix it up for USB2
    if (USBPORT_IS_USB20(devExt)) {
        rhDevExt->Pdo.DeviceDescriptor->bcdUSB  = 0x200;
        rhDevExt->Pdo.DeviceDescriptor->bMaxPacketSize0 = 0x40;
    }
    // use HC vendor and device for root hub
    rhDevExt->Pdo.DeviceDescriptor->idVendor = devExt->Fdo.PciVendorId;
    rhDevExt->Pdo.DeviceDescriptor->idProduct = devExt->Fdo.PciDeviceId;
    rhDevExt->Pdo.DeviceDescriptor->bcdDevice = devExt->Fdo.PciRevisionId;

    desc += sizeof(RH_DeviceDescriptor);

    // set up config descriptor
    rhDevExt->Pdo.ConfigurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)desc;
    LOGENTRY(FdoDeviceObject, LOG_RH, 'rhCS', desc,0, 0);
    RtlCopyMemory(rhDevExt->Pdo.ConfigurationDescriptor,
                  &RH_ConfigurationDescriptor,
                  sizeof(RH_ConfigurationDescriptor));

    // Adjust config descriptor interrupt endpoint wMaxPacketSize to
    // reflect the number of root hub ports.
    //
    ((PRH_CONFIGURATION_DESCRIPTOR)(rhDevExt->Pdo.ConfigurationDescriptor))
        ->EndpointDescriptor.wMaxPacketSize = (USHORT)(hubData.NumberOfPorts / 8) + 1;

    if (devExt->Fdo.RootHubTTCapable) {
        ((PRH_CONFIGURATION_DESCRIPTOR)(rhDevExt->Pdo.ConfigurationDescriptor))
            ->InterfaceDescriptor.bInterfaceProtocol = 0x01;  // We only support single TT
    }


    desc += sizeof(RH_ConfigurationDescriptor);

    // set up the hub descriptor
    rhDevExt->Pdo.HubDescriptor = (PUSB_HUB_DESCRIPTOR)desc;
    LOGENTRY(FdoDeviceObject, LOG_RH, 'rhHS', desc, 0, 0);
    RtlCopyMemory(rhDevExt->Pdo.HubDescriptor,
                  &RH_HubDescriptor[0],
                  sizeof(RH_HubDescriptor));
    rhDevExt->Pdo.HubDescriptor->bDescriptorLength = (UCHAR)hubDescLen;
    rhDevExt->Pdo.HubDescriptor->bNumberOfPorts = (UCHAR)hubData.NumberOfPorts;
    rhDevExt->Pdo.HubDescriptor->wHubCharacteristics = hubData.HubCharacteristics.us;
    rhDevExt->Pdo.HubDescriptor->bPowerOnToPowerGood = (UCHAR)hubData.PowerOnToPowerGood;
    rhDevExt->Pdo.HubDescriptor->bHubControlCurrent = (UCHAR)hubData.HubControlCurrent;

    // fill in the var part
    for (i = 0; i < portbytes; i++) {
        rhDevExt->Pdo.HubDescriptor->bRemoveAndPowerMask[i] = 0;
        rhDevExt->Pdo.HubDescriptor->bRemoveAndPowerMask[i+portbytes] = 0xff;
    }

    ntStatus = USBPORT_InitRootHubDeviceHandle(FdoDeviceObject,
                                               rhDevExt->Pdo.DeviceDescriptor,
                                               rhDevExt->Pdo.RootHubDeviceHandle);

    if (!NT_SUCCESS(ntStatus)) {
        goto Clean;
    }

    USBPORT_SetDeviceHandleState(FdoDeviceObject, rhDevExt->Pdo.RootHubDeviceHandle,
               Devh_Valid, Ev_CreateRoothub_Success);

    devObj->StackSize = FdoDeviceObject->StackSize;

    rhDevExt->Pdo.SelectiveSuspend = FALSE;
    KeInitializeSemaphore(&rhDevExt->Pdo.SSLock, 1, 1);

    //
    // Initialize Power and PnP logs
    //
    RtlFillMemory(&rhDevExt->PowerFuncHistory, USB_PP_HISTORY_LEN, 0xff);
    RtlFillMemory(&rhDevExt->PnpFuncHistory, USB_PP_HISTORY_LEN, 0xff);

    // initialize object
    devObj->Flags |= DO_POWER_PAGABLE;
    devObj->Flags &= ~DO_DEVICE_INITIALIZING;

    ntStatus = STATUS_SUCCESS;
    goto Exit;

Clean:
    DbgTrace((UsbpRootHub,"#%!FUNC!<Clean>\n"));

    if (NULL != rhDevExt->Pdo.RootHubDeviceHandle) {
        USBPORT_DeleteDeviceHandle(FdoDeviceObject,
                                   rhDevExt->Pdo.RootHubDeviceHandle,
                                   Ev_CreateRoothub_Fail);
        rhDevExt->Pdo.RootHubDeviceHandle = NULL;
    }

    if (NULL != descToFree) {
        UsbFreePool(descToFree);
        descToFree = NULL;
    }
    if (NULL != devObj) {
        IoDeleteDevice(devObj);
    }
    *RootHubPdo = devObj = NULL;

Exit:

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> %08.8x\n", ntStatus));

    return ntStatus;
}


NTSTATUS
USBPORT_CreateRootHubDeviceHandle(
    PDEVICE_OBJECT PdoDeviceObject
    )
/*++

Routine Description:

    Attempt to create the root device handle.

    PdoDeviceObject DeviceExtension->Pdo.RootHubDeviceHandle set to
    NULL if unsuccessful.

Arguments:

    PdoDeviceObject - Root Hub PDO Device Object

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_EXTENSION   rhDevExt = NULL;
    PUSBD_DEVICE_HANDLE rhDevHandle = NULL;
    NTSTATUS            ntStatus = STATUS_SUCCESS;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    USBPORT_ASSERT(rhDevExt->Pdo.RootHubDeviceHandle == NULL);

    rhDevHandle = rhDevExt->Pdo.RootHubDeviceHandle =
        USBPORT_AllocDeviceHandle(rhDevExt->HcFdoDeviceObject);

    if (rhDevExt->Pdo.RootHubDeviceHandle == NULL) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    if (NT_SUCCESS(ntStatus)) {

        ntStatus = USBPORT_InitRootHubDeviceHandle(
            rhDevExt->HcFdoDeviceObject,
            rhDevExt->Pdo.DeviceDescriptor,
            rhDevHandle);

        if (NT_SUCCESS(ntStatus)) {

            USBPORT_SetDeviceHandleState(rhDevExt->HcFdoDeviceObject,
                                         rhDevHandle,
                                         Devh_Valid,
                                         Ev_CreateRoothub_Success);

        } else {

            USBPORT_SetDeviceHandleState(rhDevExt->HcFdoDeviceObject,
                                         rhDevHandle,
                                         Devh_Valid,
                                         Ev_CreateRoothub_Fail);

            USBPORT_DeleteDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                       rhDevHandle,
                                       Ev_CreateRoothub_Fail);

            rhDevExt->Pdo.RootHubDeviceHandle = NULL;
        }
    }

    return ntStatus;
}


VOID
USBPORT_RootHub_RemoveDevice(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject
    )
/*++

Routine Description:

    Remove the 'ROOT HUB' USB Device and free the device handle

    Called by:

    USBPORT_FdoPnP() for HC FDO IRP_MN_STOP_DEVICE
    USBPORT_FdoPnP() for HC FDO IRP_MN_REMOVE_DEVICE
    USBPORT_PdoPnP() for RH PDO IRP_MN_REMOVE_DEVICE
    USBPORT_StopDevice()

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION rhDevExt = NULL;
    PDEVICE_EXTENSION devExt = NULL;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    LOGENTRY(FdoDeviceObject, LOG_RH, 'rhrm', rhDevExt->Pdo.RootHubDeviceHandle, 0, 0);

    if (rhDevExt->Pdo.RootHubDeviceHandle == NULL) {
        return;
    }

    // close the internal ref to the root hub interrupt pipe
    USBPORT_Ev_Rh_IntrEp_Close(FdoDeviceObject);

    // close the current config
    USBPORT_InternalCloseConfiguration(rhDevExt->Pdo.RootHubDeviceHandle,
                                       FdoDeviceObject,
                                       0);

    // close the default pipe
    USBPORT_ClosePipe(rhDevExt->Pdo.RootHubDeviceHandle,
                      FdoDeviceObject,
                      &rhDevExt->Pdo.RootHubDeviceHandle->DefaultPipe);

    // Release all previously allocated TT resources for the root hub if
    // it implements embedded TT support.
    //
    USBPORT_RemoveTTs(FdoDeviceObject,
                      rhDevExt->Pdo.RootHubDeviceHandle);

    USBPORT_RemoveDeviceHandle(FdoDeviceObject,
                               rhDevExt->Pdo.RootHubDeviceHandle,
                               Ev_RhRemoved,
                               FALSE,
                               TRUE);

    USBPORT_DeleteDeviceHandle(FdoDeviceObject,
                               rhDevExt->Pdo.RootHubDeviceHandle,
                               Ev_RhRemoved);

    rhDevExt->Pdo.RootHubDeviceHandle = NULL;

    DbgTrace((UsbpRootHub,"#%!FUNC!<out>\n"));
    return;
}


VOID
USBPORTSVC_InvalidateRootHub(
    PDEVICE_DATA DeviceData
    )
/*++

Routine Description:

    Service exported to miniports.

    This routine is called by the miniports to indicate a possible status change one the root hub ports.

    This routine is callbale from the following miniport functions:

    MINIPORT_InterruptDpc
    MINIPORT_InterruptDpcEx
    callbacks for USBPORT_REQUEST_ASYNC_CALLBACK

    Assumes that Fdo.MpRootHubFunctionSpin is held

Arguments:

    DeviceData - context data for the miniport.

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT fdoDeviceObject;
    ROOTHUB_DATA hubData;
    ULONG i, nPorts;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);
    fdoDeviceObject = devExt->HcFdoDeviceObject;

    LOGENTRY(fdoDeviceObject, LOG_RH, 'rhNO', 0, 0, 0);

    MPRH_GetRootHubData(devExt, &hubData);
    nPorts = hubData.NumberOfPorts;

    if (nPorts > MAX_ROOT_HUB_PORTS) {
        nPorts = MAX_ROOT_HUB_PORTS;
    }

    // Simulate connect changes on all root hub ports if a root hub
    // port disconnect operation is in progress.  This will cause
    // USBPORT_RootHub_PortRequest(ClearFeaturePortConnectChange) to
    // clear the real hardware root port connect change.
    //
    if (devExt->Fdo.HcDisconnectRootHubPorts) {

        for (i = 0; i < nPorts; i++) {

            devExt->Fdo.HcDisconnectRootHubPortStatus[i].ConnectChange = 1;
        }
    }

    USBPORT_Ev_Rh_IntrEp_Invalidate(fdoDeviceObject);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out>\n"));
}


VOID
USBPORT_UserSetRootPortFeature(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_ROOTPORT_FEATURE Parameters
    )
/*++

Routine Description:

    Cycle a specific Root Port

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    ULONG currentFrame, nextFrame;
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    USB_USER_ERROR_CODE usbUserStatus;
    RH_PORT_STATUS portStatus;
    KIRQL irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    usbUserStatus = UsbUserSuccess;

    LOGENTRY(FdoDeviceObject, LOG_RH, 'SRpF', 0, Parameters->PortNumber, Parameters->PortFeature);

    if (!USBPORT_ValidateRootPortApi(FdoDeviceObject, Parameters->PortNumber)) {
        Header->UsbUserStatusCode =
            usbUserStatus = UsbUserInvalidParameter;
        return;
    }

    if (!USBPORT_Check_DCA_Enabled(FdoDeviceObject)) {
        Header->UsbUserStatusCode = UsbUserFeatureDisabled;
        return;
    }

    switch(Parameters->PortFeature) {

    case PORT_RESET:
    {
        ULONG loopCount = 0;

        // attempt a reset
        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_SetFeaturePortReset(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
        // wait for reset change, this process is drive byn the
        // HC root hub hardware or miniport

        do {
/*
            USBPORT_Wait(FdoDeviceObject, 1);
*/
            currentFrame = MPf_Get32BitFrameNumber(devExt);
            do {
                nextFrame = MPf_Get32BitFrameNumber(devExt);
                if(nextFrame < currentFrame) {
                    // roll-over
                    //
                    currentFrame = nextFrame;
                    nextFrame = MPf_Get32BitFrameNumber(devExt);
                }
            } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

            MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);

            loopCount++;

        } while ((portStatus.ResetChange == 0) &&
                 (loopCount < 5));

        // clear the change bit
        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortResetChange(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;
    }

    case PORT_POWER:

        // power the port
        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_SetFeaturePortPower(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);
        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case PORT_ENABLE:

        // enable the port
        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_SetFeaturePortEnable(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case PORT_SUSPEND:

        // suspend the port
        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_SetFeaturePortSuspend(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);
        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    default:

        usbUserStatus = UsbUserNotSupported;
        break;
    }

    LOGENTRY(FdoDeviceObject, LOG_RH, 'SRp>', 0, 0, usbUserStatus);

    Header->UsbUserStatusCode = usbUserStatus;

    DbgTrace((UsbpRootHub,"#%!FUNC!<out>\n"));
}

VOID
USBPORT_UserClearRootPortFeature(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_ROOTPORT_FEATURE Parameters
    )
/*++

Routine Description:

    Cycle a specific Root Port

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none.

--*/
{
    ULONG currentFrame, nextFrame;
    PDEVICE_EXTENSION devExt;
    USB_MINIPORT_STATUS mpStatus;
    USB_USER_ERROR_CODE usbUserStatus;
    RH_PORT_STATUS portStatus;
    KIRQL irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    usbUserStatus = UsbUserSuccess;

    LOGENTRY(FdoDeviceObject, LOG_RH, 'CFRp', 0, Parameters->PortNumber, Parameters->PortFeature);

    if (!USBPORT_ValidateRootPortApi(FdoDeviceObject, Parameters->PortNumber)) {
        Header->UsbUserStatusCode =
            usbUserStatus = UsbUserInvalidParameter;
        return;
    }

    if (!USBPORT_Check_DCA_Enabled(FdoDeviceObject)) {
        Header->UsbUserStatusCode = UsbUserFeatureDisabled;
        return;
    }

    switch(Parameters->PortFeature) {

    case PORT_ENABLE:

        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortEnable(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case PORT_POWER:

        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortPower(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case PORT_SUSPEND:

        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortSuspend(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case C_PORT_ENABLE:

        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortEnableChange(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case C_PORT_CONNECTION:

        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortConnectChange(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);


        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case C_PORT_RESET:

        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortResetChange(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);


        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case C_PORT_SUSPEND:

        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortSuspendChange(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/

        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);


        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    case C_PORT_OVER_CURRENT:

        USBPORT_AcquireSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);
        MPF(devExt).MINIPORT_RH_ClearFeaturePortOvercurrentChange(
            devExt->Fdo.MiniportDeviceData,
            Parameters->PortNumber);
        USBPORT_ReleaseSpinLock(HcFdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
/*
        USBPORT_Wait(FdoDeviceObject, 1);
*/
        currentFrame = MPf_Get32BitFrameNumber(devExt);

        do {
            nextFrame = MPf_Get32BitFrameNumber(devExt);
            if(nextFrame < currentFrame) {
                // roll-over
                //
                currentFrame = nextFrame;
                nextFrame = MPf_Get32BitFrameNumber(devExt);
            }
        } while ((nextFrame - currentFrame) < FRAME_COUNT_WAIT);

        MPRH_GetPortStatus(devExt, (USHORT)(Parameters->PortNumber),
                &portStatus, mpStatus);
        // status is low 16 bits
        Parameters->PortStatus = (USHORT) portStatus.ul;
        break;

    default:

        usbUserStatus = UsbUserNotSupported;
        break;
    }

    LOGENTRY(FdoDeviceObject, LOG_RH, 'CFR>', 0, 0, usbUserStatus);

    Header->UsbUserStatusCode = usbUserStatus;

    DbgTrace((UsbpRootHub,"#%!FUNC!<out>\n"));

}


USB_MINIPORT_STATUS
USBPORT_RootHub_PowerUsb2Port(
    PDEVICE_OBJECT FdoDeviceObject,
    USHORT Port
    )
/*++

Routine Description:

    Power up the USB 2 port and the associated CC port

    NOTE: This routine is only called by USBPORT_RootHub_PortRequest()
    and the USBPORT_RootHub_PortRequest() routine explicitly releases
    the Fdo.MpRootHubFunctionSpin spinlock before calling this routine.

    It is unclear if there is a specific reason why the USB2 and CC
    Fdo.MpRootHubFunctionSpin spinlocks should not be acquired around the
    MINIPORT_RH_SetFeaturePortPower() calls here.  The
    Fdo.MpRootHubFunctionSpin spinlock is acquired in the similar routine
    USBPORT_SyncPowerAndChirpUsb2Ports().

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    miniport status

--*/

{
    PDEVICE_EXTENSION devExt, rhDevExt;
    PDEVICE_RELATIONS devRelations;
    USHORT p;
    ULONG i;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    rhDevExt= USBPORT_GetRootHubPdoExt(FdoDeviceObject);
    USBPORT_ASSERT(rhDevExt != NULL);

    LOGENTRY(FdoDeviceObject, LOG_RH, '20pw', 0, 0, 0);

    // power the associated CC controllers ports
    // for now power all ports for any CC

    devRelations =
        USBPORT_FindCompanionControllers(FdoDeviceObject,
                                         FALSE,
                                         TRUE);

    // may get NULL here if no CCs found or are registered

    for (i=0; devRelations && i< devRelations->Count; i++) {
        PDEVICE_OBJECT ccFdo = devRelations->Objects[i];
        PDEVICE_EXTENSION ccDevExt, ccRhDevExt;
        PDEVICE_OBJECT ccRhPdo;

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'pwCC', ccFdo, 0, 0);

        GET_DEVICE_EXT(ccDevExt, ccFdo);
        ASSERT_FDOEXT(ccDevExt);

        ccRhPdo = USBPORT_RefRootHubPdo(ccFdo, REF_RhPDO_PowerUsb2Port_CC);

        if (ccRhPdo) {
            GET_DEVICE_EXT(ccRhDevExt, ccRhPdo);
            ASSERT_PDOEXT(ccRhDevExt);

            // power the port
            for (p=0;
                 (USBPORT_GetPnpState(ccRhDevExt) == PnpStarted &&
                    p< NUMBER_OF_PORTS(ccRhDevExt));
                 p++) {
                // CC Fdo.MpRootHubFunctionSpin spinlock not held here
                MPF(ccDevExt).MINIPORT_RH_SetFeaturePortPower(
                    ccDevExt->Fdo.MiniportDeviceData,
                    p+1);
            }

            USBPORT_DerefRootHubPdo(ccFdo, ccRhPdo, REF_RhPDO_PowerUsb2Port_CC);
        }
    }

    // USB2 Fdo.MpRootHubFunctionSpin spinlock not held here
    MPF(devExt).MINIPORT_RH_SetFeaturePortPower(devExt->Fdo.MiniportDeviceData,
                                                Port);

    // thou shall not leak memory
    if (devRelations != NULL) {
        UsbFreePool(devRelations);
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out>\n"));

    return USBMP_STATUS_SUCCESS;
}

/*

Root hub interrupt endpoint state machine.

STATE                   NEXT_STATE
EVENT

............................................
Rh_WaitIdle              NextState
--------------------------------------------


*/

VOID
USBPORT_Ev_Rh_IntrEp_Close(
    __in PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    Ev_Rh_IntrEp_Close - close event, closes the root hub interrupt pipe.

Arguments:

    HcFdo - Fdo for the USB host controller

Return Value:

    None.

--*/
{

    KIRQL rhIrql;
    PDEVICE_EXTENSION rhDevExt;
    PDEVICE_EXTENSION devExt;
    PHCD_ENDPOINT intrEp;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    // remove internal references to the root hub

    ACQUIRE_ROOTHUB_LOCK(devExt, &rhIrql);

    // we should have a root hub pdo since we are closing an endpoint associated with it.

    rhDevExt = USBPORT_GetRootHubPdoExt(HcFdo);
    USBPORT_ASSERT(rhDevExt != NULL);

    if (rhDevExt && rhDevExt->Pdo.RH_InterruptEp){
        intrEp = USBPORT_EndpointFromHandle(rhDevExt->Pdo.RH_InterruptEp);
        ASSERT_ENDPOINT(intrEp);

#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST __WARNING_REDUNDANT_POINTER_TEST_FAR_EVIDENCE, "intrEp cannot be NULL; see ASSERT_ENDPOINT")
        if (intrEp) {
            switch(intrEp->RhEpState) {
            //case RhIntrEp_Closed:
            case RhIntrEp_Open:
            case RhIntrEp_WaitDpc:
                intrEp->RhEpState = RhIntrEp_Closed;
                break;
            default:
                USBPORT_ASSERTMSG("Unexpected intrEp->RhEpState",FALSE);
            }

            // rlease the global 'open' reference on the interrupt endpoint
            USBPORT_DereferenceEndpoint(HcFdo, rhDevExt->Pdo.RH_InterruptEp);
            rhDevExt->Pdo.RH_InterruptEp = NULL;
        }
    }

    RELEASE_ROOTHUB_LOCK(devExt, rhIrql);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out>\n"));
}


NTSTATUS
USBPORT_Ev_Rh_IntrEp_Open(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT IntrEp
    )
/*++

Routine Description:

    Ev_Rh_IntrEp_Open - open event, opens and initailizes the root hub interrupt pipe.

    This is an 'internal' open that is used to latch the device handle for the miniport.  When
    open the miniport can call InvalidateRootHub to indiacte possible changes on the root ports.

Arguments:

    HcFdo - Fdo for the USB host controller

Return Value:

    None.

--*/
{
    KIRQL rhIrql;
    PDEVICE_EXTENSION rhDevExt;
    PDEVICE_EXTENSION devExt;
    NTSTATUS nts = STATUS_NO_SUCH_DEVICE;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    ACQUIRE_ROOTHUB_LOCK(devExt, &rhIrql);
    ASSERT_ENDPOINT(IntrEp);

    rhDevExt = USBPORT_GetRootHubPdoExt(HcFdo);
    USBPORT_ASSERT(rhDevExt != NULL);

    if (rhDevExt) {
        // initial state is init
        IntrEp->RhEpState = RhIntrEp_Init;


        nts = USBPORT_ReferenceEndpoint(HcFdo,
                                        IntrEp,
                                        &rhDevExt->Pdo.RH_InterruptEp,
                                        rhDevExt,
                                        EPREF_RH_INTERRUPT_TAG);
        if (NT_SUCCESS(nts)) {
            IntrEp->RhEpState = RhIntrEp_Open;
        }
    }
    RELEASE_ROOTHUB_LOCK(devExt, rhIrql);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> %08.8x\n", nts));

    return nts;

}


VOID
USBPORT_Ev_Rh_IntrEp_Invalidate(
    __in PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    Ev_Rh_IntrEp_Invalidate - handles the invalidate event for the root hub.

    Invalidates the state of the root hub, this will trigger USBPORT to poll the the host controller to detect any
    changes on the root hub ports and report them on the interrut pipe.

    This routine should be called by the miniport if it detects an change internally either from being polled or from
    a hardware interrupt.

    **

Arguments:

    HcFdo - Fdo for the USB host controller

Return Value:

    None.

--*/
{
    PHCD_ENDPOINT intrEp = NULL;
    PDEVICE_EXTENSION devExt, rhDevExt;
    KIRQL irql;
    PUSBOBJ_REF eph;
    NTSTATUS nts;
    BOOLEAN queued;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(HcFdo, LOG_RH, 'rhIV', 0, 0, 0);

    // make sure we have an endpoint
    ACQUIRE_ROOTHUB_LOCK(devExt, &irql);

    rhDevExt= USBPORT_GetRootHubPdoExt(HcFdo);

    if (rhDevExt != NULL) {

        if (rhDevExt->Pdo.RH_InterruptEp) {
            PUSB_IOREQUEST_CONTEXT usbIoRequest;

            //endpoint = rhDevExt->Pdo.RootHubInterruptEndpoint;
            intrEp = USBPORT_EndpointFromHandle(rhDevExt->Pdo.RH_InterruptEp);
            ASSERT_ENDPOINT(intrEp);

            // take action based on the current state
            switch(intrEp->RhEpState) {
            case RhIntrEp_Open:

                usbIoRequest = USBPORT_Core_AllocIoRequest(HcFdo, NULL, UsbIoTag_RhDpc);

                if (usbIoRequest) {

                    // reference the endpoint then queue a DPC to handle the transfer the possible transfer
                    nts = USBPORT_ReferenceEndpoint(HcFdo,
                                                     intrEp,
                                                     &eph,
                                                     rhDevExt,
                                                     EPREF_RH_DPC_TAG);

                    if (NT_SUCCESS(nts)) {
                        queued = KeInsertQueueDpc(&devExt->Fdo.RootHubDpc, eph, usbIoRequest);
                        USBPORT_ASSERT(queued);
                        if (queued) {
                            intrEp->RhEpState = RhIntrEp_WaitDpc;
                        } else {
                            // already queued
                            USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);
                        }
                    } else {
                        // error cold not ref
                         USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);
                    }
                }
                break;

            case RhIntrEp_WaitDpc:
                // already waiting
                break;

            default:
                break;
           }
        }
    }

    RELEASE_ROOTHUB_LOCK(devExt, irql);

    if ((rhDevExt != NULL) && (rhDevExt->Pdo.SelectiveSuspend)) {
        // Complete WW IRP only if root hub is selectively suspended
        USBPORT_CompleteHubPdoWaitWake(HcFdo, STATUS_SUCCESS);
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));
}


VOID
USBPORT_Ev_Rh_IntrEp_Dpc(
    __in PKDPC Dpc,
    __in_opt PVOID DeferredContext,
    __in_opt PVOID SystemArgument1,
    __in_opt PVOID SystemArgument2
    )
/*++

Routine Description:

    This routine runs at DISPATCH_LEVEL IRQL.

    The root hub DPC checks the root hub endpoint for status changes and reports the chnages on the interrupt pipe.

    All control tramsfers now complete synchronously as soon as they are dispatched to the miniport.

Arguments:

    Dpc - Pointer to the DPC object.

    DeferredContext - supplies FdoDeviceObject.

    SystemArgument1 - endpoint (referenced).

    SystemArgument2 - USB_IOREQUEST

Return Value:

    None.

--*/
{
    PDEVICE_OBJECT hcFdo = DeferredContext;
    PDEVICE_EXTENSION devExt, rhDevExt;
    PUSBOBJ_REF intrEpHandle = SystemArgument1;
    PUSB_IOREQUEST_CONTEXT usbIoRequest = SystemArgument2;
    PHCD_ENDPOINT intrEp;
    KIRQL irql;
    BOOLEAN rhInit = FALSE;
    LONG oldRhCfgFlag;
    ETW_EXECUTION_METRICS   execMetrics;

    ICE_START_TRACE();
    ETW_MEASURE_TIME_START(execMetrics);

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));


    GET_DEVICE_EXT(devExt, hcFdo);
    ASSERT_FDOEXT(devExt);

    rhDevExt= USBPORT_GetRootHubPdoExt(hcFdo);
    USBPORT_ASSERT(rhDevExt != NULL);

    LOGENTRY(hcFdo, LOG_RH, 'rDPC', 0, 0, rhDevExt);

    if (rhDevExt == NULL) {
        USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);
        return;
    }

    intrEp = USBPORT_EndpointFromHandle(intrEpHandle);
    ASSERT_ENDPOINT(intrEp);

    // indicate Dpc is now running, this will allow another DPC to be queued
    ACQUIRE_ROOTHUB_LOCK(devExt, &irql);

    // take action based on the current state
    switch(intrEp->RhEpState) {
    case RhIntrEp_WaitDpc:
        intrEp->RhEpState = RhIntrEp_Open;
        break;

    default:
        break;
    }

    if (!IsListEmpty(&devExt->Fdo.RH_iTxqHead)) {
        // transfer queued means the hub driver is enabled so we can allow the companions to
        // enumerate.
        rhInit = TRUE;
    }
    RELEASE_ROOTHUB_LOCK(devExt, irql);

    if (rhInit) {
        // first transfer posted to USB20 flushes the callbacks

        USBPORT_ASSERT(rhDevExt->Pdo.HubInitCallback == NULL);

        if (USBPORT_IS_USB20(devExt)) {
            //oldRhCfgFlag = rhDevExt->Pdo.RhCfgFlag
            //IF (rhDevExt->Pdo.RhCfgFlag == 0) THEN
            //    rhDevExt->Pdo.RhCfgFlag = 1;
            //    FlushRhCallbacks();
            //}
            // if we transition 0->1 then flush the callbacks

            oldRhCfgFlag = InterlockedCompareExchange(&rhDevExt->Pdo.RhCfgFlag, 1, 0);
            if (oldRhCfgFlag == 0) {
                USBPORT_FlushRhCallbacks(hcFdo);
            }
        }
    }

    // GTS MPf_CheckController(devExt);

    // this routine will check the decodes flag and if decodes are off the request is treated like a NAK.
    // The decodes may be off temporarily while the miniport resets the controller.
    USBPORT_Rh_IntrEp_Worker(hcFdo, intrEp);

    ACQUIRE_ROOTHUB_LOCK(devExt, &irql);
    if (!IsListEmpty(&devExt->Fdo.RH_iTxqHead)) {
        // if the interrupt transfer queue is not empty the the controller  reported a status change with
        // no change detectable so we re-enable the root hub status hw irq here.
        MPRH_EnableIrq(devExt, ROOTHUB_LOCK_ACQUIRED);
    }
    RELEASE_ROOTHUB_LOCK(devExt, irql);

    // deref the endpoint for the DPC
    USBPORT_DereferenceEndpoint(hcFdo, intrEpHandle);

    // match the inc when the DPC was queued
    USBPORT_Core_UnlinkAndFreeIoRequest(usbIoRequest, usbIoRequest);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));

    ETW_STOP_AND_LOG_TIME(hcFdo, execMetrics, RHINTREP);
    ICE_STOP_TRACE();

    return;
}


PHCD_TRANSFER_CONTEXT
USBPORT_Rh_TransferFromIrp(
    __in PDEVICE_OBJECT HcFdo,
    __inout PIRP Irp
    )
/*++

Routine Description:

    Used by the root hub code to extract a transfer from an Irp.

Arguments:

    DeviceObject - Fdo for USB HC

    Irp - irp containing a transfer Urb

Return Value:

    transfer context

--*/
{
    PIO_STACK_LOCATION  ioStk;
    PTRANSFER_URB urb;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    // extract the urb;
    ioStk = IoGetCurrentIrpStackLocation(Irp);
    urb = ioStk->Parameters.Others.Argument1;

    ASSERT_TRANSFER_URB(urb);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> urb,context %p, %p\n", urb, urb->pd.HcdTransferContext));

    return urb->pd.HcdTransferContext;
}


VOID
USBPORT_Rh_Initialize(
    __in PDEVICE_OBJECT HcFdo
    )
/*++

Routine Description:

    Initailzes a root hub state machine and queues for the root hub interrupt pipe.
    This is called when the FDO initializes.

Arguments:

    DeviceObject - Fdo for USB HC

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION devExt;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);


    // root hub DPC
    KeInitializeDpc(&devExt->Fdo.RootHubDpc, USBPORT_Ev_Rh_IntrEp_Dpc, HcFdo);

    //interrupt ep

    // iTxq uses roothub spinlock
    InitializeListHead(&devExt->Fdo.RH_iTxqHead);
    IoCsqInitialize(&devExt->Fdo.RH_iTxCsq,
                USBPORT_RhIntrEp_Insert_TxqIrp,            //CsqInsertIrp,
                USBPORT_RhIntrEp_Remove_TxqIrp,            //CsqRemoveIrp,
                USBPORT_RhIntrEp_PeekNext_TxqIrp,          //CsqPeekNextIrp,
                USBPORT_RhIntrEp_Acquire_TxqLock,          //CsqAcquireLock,
                USBPORT_RhIntrEp_Release_TxqLock,          //CsqReleaseLock,
                USBPORT_RhIntrEp_CompleteCanceled_TxqIrp   //CsqCompleteCanceledIrp
                );


    //control endpoint


}


VOID
USBPORT_RH_NeoQueueAsyncTransfer(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PIRP Irp,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Transfer Queueing function for root hub endpoints (EpQueueAsyncTransfer);

    This routine handles URB requests for the root hub.  It replaces the endpointworker path used in
    earlier versions of USBPORT which routed thru the core transfer path.  The requests are still passed thru
    the primary URB dispatch handler in urb.c where the device handle and pipe handles are referenced.

    The root hub is an emulated device so the transfers do not need to be mapped and passed to the
    hardware via IoMaptransfer.

    This function utilizes the newer cancel safe queues to handle the requests queued to the root hub. It
    queues the transfer to the endpoints internal queue then 'runs' the endpoint.


    MPRH_DisableIrq
    MPRH_EnableIrq

    These routines are used to disable/enable to port status change interrupt on the host controller.  The status
    change interrrupt is disabled interrupt ep queue is detected empty and enabled whenver an interrupt transfer
    is queued.  The reason for this is that some host controllers will continue to assert the hardware interrupt
    until is acked con the control pipe (just like an external hub).  The status chnage interrupt is also enabled
    whenever the controller is put into 'selective suspend'.

Arguments:

    DeviceObject - Fdo for USB HC

    Endpoint - one of the endpoints for the root hub

    Irp - Irp associated with the request.  May be NULL for an internal control request.

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION devExt;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(HcFdo, LOG_RH, 'neo1', Endpoint, Irp, Transfer);

    if (Endpoint->Parameters.TransferType == Control) {

        if (Irp) {
            Irp->IoStatus.Status = STATUS_PENDING;
            IoMarkIrpPending(Irp);
        }
        SET_FLAG(Transfer->Flags, USBPORT_TXFLAG_RH_CONTROL);
        // this will complete the request
        USBPORT_Rh_CtrlEp_Worker(HcFdo, Endpoint, Irp, Transfer);

    } else {

        SET_FLAG(Transfer->Flags, USBPORT_TXFLAG_RH_INTERRUPT);
        USBPORT_ASSERT(Irp);
        if (Irp) {
            // queue the irp an sched a Dpc to check the root ports for a status change
            IoCsqInsertIrp(&devExt->Fdo.RH_iTxCsq, Irp, NULL);
        }

        USBPORT_Ev_Rh_IntrEp_Invalidate(HcFdo);
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));
}


VOID
USBPORT_Rh_CtrlEp_Worker(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PIRP Irp,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Processes transfer queued for root hub control endpoints. Replaces old root hub worker function.

    This routine processes the control transfer and completes the irp.

Arguments:

Return Value:

    None.

--*/
{
    USBD_STATUS usbdStatus;
    RHSTATUS rhStatus;
    PDEVICE_EXTENSION devExt;
    KIRQL irql, dm_irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    ASSERT_ENDPOINT(Endpoint);
    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    // dequeue transfer from this enpoint that matches this irp

    LOGENTRY(HcFdo, LOG_RH, 'ctw1', Transfer, 0, Irp);
    ASSERT_TRANSFER(Transfer);

    // call root hub emulation code
    USBPORT_ASSERT(Endpoint->Parameters.TransferType == Control);

    USBPORT_ACQUIRE_DM_LOCK(devExt, dm_irql);

    if (devExt->Fdo.Flags.ControllerGone ||
        devExt->Fdo.PowerFlags.MpRestartFailed) {

        // controller was surprise removed (hardware gone)
        // return an error to reflect this
        usbdStatus = USBPORT_NtStatus_TO_UsbdStatus(NT_DEVICE_DISCONNECTED_STATUS);
        rhStatus = RH_STALL;

        LOGENTRY(HcFdo, LOG_RH, 'ctw4', Transfer, usbdStatus, 0);
        USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);
    } else {

        USBPORT_RELEASE_DM_LOCK(devExt, dm_irql);

        rhStatus = USBPORT_RootHub_Endpoint0(HcFdo, Transfer);

        // root hub control endpoint should not NAK.
        USBPORT_ASSERT(rhStatus != RH_NAK);
        LOGENTRY(HcFdo, LOG_RH, 'ctw2', Transfer, rhStatus, 0);

        usbdStatus = RHSTATUS_TO_USBDSTATUS(rhStatus);
    }

    //
    // With asynchronous root hub commands added for Wireless USB, we need
    // to handle the case where the miniport returns RH_PENDING
    //
    if(rhStatus != RH_PENDING){

        // this will complete the irp with the appropriate status and handle any other book-keeping for the
        // request.
        LOGENTRY(HcFdo, LOG_RH, 'ctw3', Transfer, usbdStatus, 0);

        irql = USBPORT_TxAcquireLock(HcFdo, NULL);
        Transfer->UsbdStatus = usbdStatus;
        //<this routine drops the lock>
        USBPORT_Core_iCompleteDoneTransfer(HcFdo, Transfer->Irp, Transfer, NULL, irql);
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));
}


VOID
USBPORT_Rh_IntrEp_Worker(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Processes transfer queued for root hub interrupt endpoints. Replaces old root hub worker function.

    This routine polls the root hub for status changes and reports them on the interrupt pipe.

Arguments:

Return Value:

    None.

--*/
{
    USBD_STATUS usbdStatus;
    RHSTATUS rhStatus;
    PDEVICE_EXTENSION devExt;
    BYTE buffer[32] = {0}; // usb spec only allows up to 255 ports
    ULONG cbBuffer;
    KIRQL irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    ASSERT_ENDPOINT(Endpoint);
    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_ASSERT(Endpoint->Parameters.TransferType == Interrupt);

    // call root hub emulation code
    cbBuffer = sizeof(buffer);
    rhStatus = USBPORT_RootHub_Endpoint1_Peek(HcFdo, Endpoint, buffer, &cbBuffer);
    LOGENTRY(HcFdo, LOG_RH, 'itw1', rhStatus, buffer, cbBuffer);

    if (rhStatus != RH_NAK) {

        PIRP irp;

        LOGENTRY(HcFdo, LOG_RH, 'itw2', 0, rhStatus, 0);

        // process one interrupt transfer from the queue
        irp = IoCsqRemoveNextIrp(&devExt->Fdo.RH_iTxCsq, NULL);
        LOGENTRY(HcFdo, LOG_TRANSFER, 'itw3', irp, 0, 0);

        if (irp) {
            PHCD_TRANSFER_CONTEXT transfer;

            // get the transfer context from the irp
            transfer = USBPORT_Rh_TransferFromIrp(HcFdo, irp);

            LOGENTRY(HcFdo, LOG_RH, 'itw4', transfer, 0, irp);
            ASSERT_TRANSFER(transfer);

            rhStatus = USBPORT_RootHub_Endpoint1(HcFdo, transfer, buffer, cbBuffer);

            usbdStatus = RHSTATUS_TO_USBDSTATUS(rhStatus);

            // this will complete the irp with the appropriate status and handle any other book-keeping for the
            // request.
            irql = USBPORT_TxAcquireLock(HcFdo, NULL);
            LOGENTRY(HcFdo, LOG_RH, 'itw5', transfer, usbdStatus, 0);
            transfer->UsbdStatus = usbdStatus;
            //<this routine drops the lock>
            USBPORT_Core_iCompleteDoneTransfer(HcFdo, transfer->Irp, transfer, NULL, irql);
        }

    }
    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));

}


VOID
USBPORT_RhIntrEp_Insert_TxqIrp(
    __in PIO_CSQ Csq,
    __inout PIRP Irp
    )
/*++

Routine Description:

    PIO_CSQ_INSERT_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               Fdo.RH_iTxCsq);
    ASSERT_FDOEXT(devExt);

    InsertTailList(&devExt->Fdo.RH_iTxqHead,
                   &Irp->Tail.Overlay.ListEntry);
    // we have a transfer pending but no changes yet enable the controller to generate an interrupt
    // if a root hub change occurs.
    MPRH_EnableIrq(devExt, ROOTHUB_LOCK_ACQUIRED);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));
}


VOID
USBPORT_RhIntrEp_Remove_TxqIrp(
    __in PIO_CSQ Csq,
    __inout PIRP Irp
    )
/*++

Routine Description:

    PIO_CSQ_REMOVE_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               Fdo.RH_iTxCsq);
    ASSERT_FDOEXT(devExt);

    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);

    if (IsListEmpty(&devExt->Fdo.RH_iTxqHead)) {
        // if the transfer list is empty turn off the change notification interrupt
        MPRH_DisableIrq(devExt, ROOTHUB_LOCK_ACQUIRED);
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));
}


PIRP
USBPORT_RhIntrEp_PeekNext_TxqIrp(
    __in PIO_CSQ Csq,
    __inout PIRP Irp,
    __inout_opt PVOID PeekContext
    )
/*++

Routine Description:

    PIO_CSQ_PEEK_NEXT_IRP

    Rules for peeNext the way I understand them -- these are not documented in
    the DDK text.

    if Irp == NULL start from list head
    else start peeking from the Irp following the Irp passed in.

    When we encounter an irp matching the context we return the irp

    if peekcontext == NULL then we return the next irp

    The DDK sample checks the FileObject in the irpstack even though a
    PeekContext is never specified.

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PLIST_ENTRY le;
    PIRP nextIrp = NULL;
    PIO_STACK_LOCATION     irpStack;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    USBPORT_ASSERT(PeekContext == NULL);
    devExt = CONTAINING_RECORD(Csq,
                              DEVICE_EXTENSION,
                              Fdo.RH_iTxCsq);
    ASSERT_FDOEXT(devExt);
    LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'pkRH', Irp, PeekContext, 0);

    if (Irp == NULL) {
        // start at the head
        le = devExt->Fdo.RH_iTxqHead.Flink;
    } else {
        // start at the next irp in the queue
        le = Irp->Tail.Overlay.ListEntry.Flink;
    }

    while (le != &devExt->Fdo.RH_iTxqHead) {

        nextIrp = (PIRP) CONTAINING_RECORD(le,
                                struct _IRP,
                                Tail.Overlay.ListEntry);

        irpStack = IoGetCurrentIrpStackLocation(nextIrp);

// This check is unecessary since we don't use a PeekContext
        if (irpStack->FileObject == (PFILE_OBJECT) PeekContext) {
            break;
        }

        le = le->Flink;
    }

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> nextIrp=%p\n", nextIrp));

    return nextIrp;

}

__drv_raisesIRQL(DISPATCH_LEVEL)
VOID
USBPORT_RhIntrEp_Acquire_TxqLock(
    __in PIO_CSQ Csq,
    __out __deref __drv_savesIRQL PKIRQL Irql
    )
/*++

Routine Description:

    PIO_CSQ_ACQUIRE_LOCK

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT fdo;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               Fdo.RH_iTxCsq);
    ASSERT_FDOEXT(devExt);
    fdo = devExt->HcFdoDeviceObject;

    LOGENTRY(fdo, LOG_RH, 'rhL+', 0, fdo, 0);
    ACQUIRE_ROOTHUB_LOCK(devExt, Irql);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));
}


VOID
USBPORT_RhIntrEp_Release_TxqLock(
    __in PIO_CSQ Csq,
    __drv_restoresIRQL __in KIRQL Irql
    )
/*++

Routine Description:

    PIO_CSQ_RELEASE_LOCK

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT fdo;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               Fdo.RH_iTxCsq);
    ASSERT_FDOEXT(devExt);
    fdo = devExt->HcFdoDeviceObject;

    LOGENTRY(fdo, LOG_RH, 'rhL-', 0, fdo, 0);
    RELEASE_ROOTHUB_LOCK(devExt, Irql);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));

}


VOID
USBPORT_RhIntrEp_CompleteCanceled_TxqIrp(
    __in PIO_CSQ Csq,
    __inout PIRP CanceledIrp
    )
/*++

Routine Description:

    PIO_CSQ_COMPLETE_CANCELED_IRP

    see DDK

Arguments:

    see DDK

Return Value:

--*/
{

    PDEVICE_EXTENSION devExt;
    PTRANSFER_URB urb;
    PIO_STACK_LOCATION ioStk;
    PHCD_TRANSFER_CONTEXT transfer;
    KIRQL irql;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> CanceledIrp=%p\n", CanceledIrp));

    devExt = CONTAINING_RECORD(Csq,
                               DEVICE_EXTENSION,
                               Fdo.RH_iTxCsq);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'caBA', CanceledIrp, 0, 0);

    ioStk = IoGetCurrentIrpStackLocation(CanceledIrp);
    urb = ioStk->Parameters.Others.Argument1;
    transfer = urb->pd.HcdTransferContext;
    ASSERT_TRANSFER(transfer);
    transfer->UsbdStatus = USBD_STATUS_CANCELED;

    irql = USBPORT_TxAcquireLock(devExt->HcFdoDeviceObject, NULL);
    // complete thru standard path to free resources
    USBPORT_Core_iCompleteDoneTransfer(devExt->HcFdoDeviceObject, transfer->Irp, transfer, NULL, irql);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));

}


NTSTATUS
USBPORT_HardResetRootPdo(
    __in PDEVICE_OBJECT HcFdo
    )
/*
     Requests a reset of the root hub PDO.  the hub driver will request this if detects errors on one of the root hub ports.
*/
{

    PDEVICE_EXTENSION devExt;
    RH_PORT_STATUS portStatus;
    USB_MINIPORT_STATUS mpStatus;
    USHORT p;
    ULONG nPorts;
    ROOTHUB_DATA hubData;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    MPRH_GetRootHubData(devExt, &hubData);
    nPorts = hubData.NumberOfPorts;

    if (devExt->Fdo.PowerFlags.MpRestartFailed) {

        return STATUS_NO_SUCH_DEVICE;
    }

    for (p=0;
         p<nPorts;
         p++) {

        KIRQL rh_irql;

        MPRH_GetPortStatus(devExt,
               p+1,
               &portStatus,
               mpStatus);

        LOGENTRY(HcFdo, LOG_PNP, 'rrRS', HcFdo, p, portStatus.ul);

        if (mpStatus == USBMP_STATUS_SUCCESS &&
            portStatus.PowerOn == 1 &&
            portStatus.Connected == 1 &&
            portStatus.Enabled == 1) {

            // clear the enable bit since the hub is re-initializing.
            USBPORT_AcquireSpinLock(HcFdo, &devExt->Fdo.MpRootHubFunctionSpin, &rh_irql);
            MPF(devExt).MINIPORT_RH_ClearFeaturePortEnable(devExt->Fdo.MiniportDeviceData, p+1);
            USBPORT_ReleaseSpinLock(HcFdo, &devExt->Fdo.MpRootHubFunctionSpin, rh_irql);
        }
    }

    return STATUS_SUCCESS;
}



PDEVICE_OBJECT
USBPORT_iRefRootHubPdo(
    __in PDEVICE_OBJECT HcFdo,
    __in LONG RefOwnerTag
    )
/*
     Refernces and returns the root hub PDO currently associated with an HcFDO.  The ref must be released with a call to USBPORT_DerefRootHubPdo

     Will return NULL if no root hub PDO or Pdo cannot be referenced
*/
{

    PDEVICE_EXTENSION devExt, rhDevExt;
    PDEVICE_OBJECT rhPdo;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    rhPdo = devExt->Fdo.RhPdoPtr;
    if (rhPdo != NULL) {

        GET_DEVICE_EXT(rhDevExt, rhPdo);
        ASSERT_PDOEXT(rhDevExt);

        USBPORT_ASSERT(RefOwnerTag >= 0);
        USBPORT_ASSERT(RefOwnerTag < REF_RhPDO_MaxIndex);

        if (RefOwnerTag >= 0 && RefOwnerTag < REF_RhPDO_MaxIndex) {
            USBPORT_ASSERT(rhDevExt->Pdo.PdoRefTable[RefOwnerTag]>=0);
            rhDevExt->Pdo.PdoRefTable[RefOwnerTag]++;
            //SANITY check
        }

        ObReferenceObject(rhPdo);
    }

    return rhPdo;
}



PDEVICE_OBJECT
USBPORT_RefRootHubPdo(
    __in PDEVICE_OBJECT HcFdo,
    __in LONG RefOwnerTag
    )
/*
     References and returns the root hub PDO currently associated with an HcFDO.  The ref must be released with a call to USBPORT_DerefRootHubPdo.

     This routine will return NULL if no root hub PDO or Pdo cannot be referenced.

     USBPORT_RefRootHubPdo
     USBPORT_DerefRootHubPdo
     USBPORT_SetRootHubPdo
     USBPORT_GetRootHubPdo

     The routines are used to access the root hub pdo device extension thru the host controller FDO. They reference the device object so that it
     is not deleted while the pdoext is in use.

*/
{

    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT rhPdo;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_AcquireSpinLock(HcFdo, &devExt->Fdo.RhPdoPtrSpin, &irql);

    rhPdo = USBPORT_iRefRootHubPdo(HcFdo, RefOwnerTag);

    USBPORT_ReleaseSpinLock(HcFdo, &devExt->Fdo.RhPdoPtrSpin, irql);

    return rhPdo;
}


VOID
USBPORT_DerefRootHubPdo(
    __in PDEVICE_OBJECT HcFdo,
    __in PDEVICE_OBJECT RhPdo,
    __in LONG RefOwnerTag
    )
/*
     Releases reference on the root hub PDO
*/
{

    PDEVICE_EXTENSION devExt, rhDevExt;
    KIRQL irql;


    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    GET_DEVICE_EXT(rhDevExt, RhPdo);
    ASSERT_PDOEXT(rhDevExt);

    USBPORT_AcquireSpinLock(HcFdo, &devExt->Fdo.RhPdoPtrSpin, &irql);

    USBPORT_ASSERT(RefOwnerTag >= 0);
    USBPORT_ASSERT(RefOwnerTag < REF_RhPDO_MaxIndex);

    if (RefOwnerTag >= 0 && RefOwnerTag < REF_RhPDO_MaxIndex) {
        rhDevExt->Pdo.PdoRefTable[RefOwnerTag]--;
        //SANITY check
        USBPORT_ASSERT(rhDevExt->Pdo.PdoRefTable[RefOwnerTag]>=0);
    }
    ObDereferenceObject(RhPdo);

    USBPORT_ReleaseSpinLock(HcFdo, &devExt->Fdo.RhPdoPtrSpin, irql);

    return;
}


PDEVICE_OBJECT
USBPORT_SetRootHubPdo(
    __in PDEVICE_OBJECT HcFdo,
    __in_opt PDEVICE_OBJECT NewRhPdoValue
    )
/*
    Sets the Hc RootHubPdo pointer to a new value and returns the previous value (unreferenced).

*/
{

    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT oldRhPdo;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    USBPORT_AcquireSpinLock(HcFdo, &devExt->Fdo.RhPdoPtrSpin, &irql);

    oldRhPdo = devExt->Fdo.RhPdoPtr;
    devExt->Fdo.RhPdoPtr = NewRhPdoValue;
    // ref for Hc Fdo Extension
    if (NewRhPdoValue) {
        USBPORT_iRefRootHubPdo(HcFdo, REF_RhPDO_CreateRootHubPdo);
    }

    USBPORT_ReleaseSpinLock(HcFdo, &devExt->Fdo.RhPdoPtrSpin, irql);

    if (oldRhPdo) {
        USBPORT_DerefRootHubPdo(HcFdo, oldRhPdo, REF_RhPDO_CreateRootHubPdo);
    }

    return oldRhPdo;
}


PDEVICE_EXTENSION
USBPORT_GetRootHubPdoExt(
    __in PDEVICE_OBJECT HcFdo
    )
/*
    Returns the current value of the root hub PDO pointer device extension (unreferenced). This routine is used in codepaths where the root hub
    PDO is already implicitly referenced such as the IO path or root hub emulation path.

*/
{

    PDEVICE_EXTENSION devExt, rhDevExt;
    PDEVICE_OBJECT rhPdo;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    rhDevExt = NULL;

    USBPORT_AcquireSpinLock(HcFdo, &devExt->Fdo.RhPdoPtrSpin, &irql);

    rhPdo = devExt->Fdo.RhPdoPtr;

    if (rhPdo) {
        GET_DEVICE_EXT(rhDevExt, rhPdo);
        ASSERT_PDOEXT(rhDevExt);
    }

    USBPORT_ReleaseSpinLock(HcFdo, &devExt->Fdo.RhPdoPtrSpin, irql);

    return rhDevExt;
}

VOID
USBPORTSVC_Rh_Complete(
    __in PDEVICE_DATA DeviceData,
    __in PTRANSFER_PARAMETERS TransferParameters,
    __in USB_MINIPORT_STATUS  MpStatus,
    __in ULONG BytesReturned
    )
/*++

Routine Description:

    Completes async root hub commands.

Arguments:

Return Value:

    None.

--*/
{
    USBD_STATUS usbdStatus;
    PDEVICE_OBJECT hcFdo = NULL;
    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    PHCD_TRANSFER_CONTEXT transfer = NULL;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    DEVEXT_FROM_DEVDATA(devExt, DeviceData);
    ASSERT_FDOEXT(devExt);

    hcFdo = devExt->HcFdoDeviceObject;

    TRANSFER_FROM_TPARAMETERS(transfer, TransferParameters);
    ASSERT_TRANSFER(transfer);

    LOGENTRY(hcFdo, LOG_RH, 'ctrh', transfer, MpStatus, BytesReturned);

    usbdStatus = MPSTATUS_TO_USBSTATUS(MpStatus);

    // this will complete the irp with the appropriate status and handle any other book-keeping for the
    // request.

    irql = USBPORT_TxAcquireLock(hcFdo, NULL);

    transfer->UsbdStatus = usbdStatus;
    transfer->MiniportBytesTransferred = BytesReturned;

    //<this routine drops the lock>
    USBPORT_Core_iCompleteDoneTransfer(hcFdo, transfer->Irp, transfer, NULL, irql);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));
}



RHSTATUS
USBPORT_RootHub_AsyncCommand(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    __in PUCHAR Buffer,
    __in ULONG BufferLength,
    __in PHCD_TRANSFER_CONTEXT Transfer
    )
/*++

Routine Description:

    Process a hub class command to the miniport asynchronously.  THe miniport will call
    USBPORTSVC_Rh_Complete when the request is complete.

Arguments:

Return Value:

    Root Hub status code.

--*/
{
    PDEVICE_EXTENSION devExt;

    DbgTrace((UsbpRootHub,"#%!FUNC!<in> \n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOGENTRY(FdoDeviceObject, LOG_RH, 'rASM', SetupPacket, 0, 0);

    MPF(devExt).MINIPORT_RH_AsyncCommand(devExt->Fdo.MiniportDeviceData,
                                         SetupPacket,
                                         Buffer,
                                         BufferLength,
                                         &Transfer->Tp);

    DbgTrace((UsbpRootHub,"#%!FUNC!<out> \n"));
    return RH_PENDING;
}

USB_MINIPORT_STATUS
USBPORT_GetRootHubPortStatus(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in USHORT PortNumber,
    __out PRH_PORT_STATUS PortStatus
    )
/*++

Routine Description:

    This routine returns in the PortStatus parameter the status of the
    root hub port specified by the PortNumber.

    If a root hub port disconnect operation is in progress the
    simulated root hub port status is returned instead of the real
    hardware root hub port status.

--*/
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_SUCCESS;
    KIRQL irql;

    USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.MpRootHubFunctionSpin, &irql);

    if (HcFdoExt->Fdo.HcDisconnectRootHubPorts) {

        // Return the simulated root hub port status.

        if ((PortNumber > 0) && (PortNumber <= MAX_ROOT_HUB_PORTS)) {

            PortStatus->ul = HcFdoExt->Fdo.HcDisconnectRootHubPortStatus[PortNumber - 1].ul;

        } else {

            PortStatus->ul = 0;
            PortStatus->PowerOn = 1;
        }

    } else {

        // Return the real hardware root hub port status, but include
        // any left over connect changes from the simulated root hub
        // port status.

        mps = MPF(HcFdoExt).MINIPORT_RH_GetPortStatus(
            HcFdoExt->Fdo.MiniportDeviceData,
            PortNumber,
            PortStatus);

        if ((PortNumber > 0) && (PortNumber <= MAX_ROOT_HUB_PORTS)) {

            PortStatus->ul |= HcFdoExt->Fdo.HcDisconnectRootHubPortStatus[PortNumber - 1].ul;
        }
    }

    USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.MpRootHubFunctionSpin, irql);

    return mps;
}


VOID
USBPORT_HcDisconnectRootHubPorts(
    __in PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    This routine simulates disconnect changes on all root hub ports.

    The disconnect changes will be returned by
    USBPORT_GetRootHubPortStatus() until acknowledged by
    USBPORT_RootHub_PortRequest(ClearFeaturePortConnectChange).

    The hub driver should be informed of the disconnect changes by the
    call to USBPORT_Ev_Rh_IntrEp_Invalidate() done here.

--*/
{
    PDEVICE_EXTENSION devExt;
    ROOTHUB_DATA hubData;
    PUSBPORT_REGISTRATION_PACKET RegPacket;
    USHORT PortNumber;
    ULONG nPorts;
    RH_PORT_STATUS portStatus;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    RegPacket = &REGISTRATION_PACKET(devExt);

    MPRH_GetRootHubData(devExt, &hubData);
    nPorts = hubData.NumberOfPorts;

    portStatus.ul = 0;
    portStatus.PowerOn = 1;
    portStatus.ConnectChange = 1;

    USBPORT_AcquireSpinLock(FdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);

    if (!devExt->Fdo.HcDisconnectRootHubPorts) {

        devExt->Fdo.HcDisconnectRootHubPorts = TRUE;

        for (PortNumber = 0; PortNumber < nPorts; PortNumber++) {

            // Make sure all ports are disabled.
            //
            RegPacket->MINIPORT_RH_ClearFeaturePortEnable(
                devExt->Fdo.MiniportDeviceData,
                PortNumber + 1);

            if (PortNumber < MAX_ROOT_HUB_PORTS) {

                // Simulate port connect status changes.
                //
                devExt->Fdo.HcDisconnectRootHubPortStatus[PortNumber].ul = portStatus.ul;
            }
        }

        USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);

        USBPORT_CancelAllAsyncTimerCallbacks(FdoDeviceObject);

        USBPORT_Ev_Rh_IntrEp_Invalidate(FdoDeviceObject);

    } else {

        USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
    }
}

VOID
USBPORT_HcReconnectRootHubPorts(
    __in PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    This routine simulates reconnect changes on all root hub ports and
    allows USBPORT_GetRootHubPortStatus() to return the real hardware
    root hub port status again, but the real hardware port status will
    include the reconnect changes until acknowledged by
    USBPORT_RootHub_PortRequest(ClearFeaturePortConnectChange).

    The hub driver should be informed of the disconnect changes by the
    call to USBPORT_Ev_Rh_IntrEp_Invalidate() done here.

--*/
{
    PDEVICE_EXTENSION devExt;
    ROOTHUB_DATA hubData;
    USHORT PortNumber;
    ULONG nPorts;
    RH_PORT_STATUS portStatus;
    KIRQL irql;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    MPRH_GetRootHubData(devExt, &hubData);
    nPorts = hubData.NumberOfPorts;

    if (nPorts > MAX_ROOT_HUB_PORTS) {
        nPorts = MAX_ROOT_HUB_PORTS;
    }

    portStatus.ul = 0;
    portStatus.ConnectChange = 1;

    USBPORT_AcquireSpinLock(FdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, &irql);

    if (devExt->Fdo.HcDisconnectRootHubPorts) {

        devExt->Fdo.HcDisconnectRootHubPorts = FALSE;

        for (PortNumber = 0; PortNumber < nPorts; PortNumber++) {

            if (PortNumber < MAX_ROOT_HUB_PORTS) {

                devExt->Fdo.HcDisconnectRootHubPortStatus[PortNumber].ul = portStatus.ul;
            }
        }

        USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);

        USBPORT_Ev_Rh_IntrEp_Invalidate(FdoDeviceObject);

    } else {

        USBPORT_ReleaseSpinLock(FdoDeviceObject, &devExt->Fdo.MpRootHubFunctionSpin, irql);
    }
}
