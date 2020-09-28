/*++

Copyright (c) 2008      Microsoft Corporation

Module Name:

        USBINTRL.H

Abstract:

   IOCTLs used only by the USB driver stack

Environment:

    Kernel 

Revision History:

    10-04-08 : created

--*/

#ifndef   __USINTRL_H__
#define   __USINTRL_H__

#include <usbdlib.h>

/*
    kernel mode IOCTL index values

    The following codes are valid only if passed as in
    the icControlCode parameter for
    IRP_MJ_INTERNAL_DEVICE_CONTROL

*/

#ifndef USB_PROPAGATE_RESUME
#define USB_PROPAGATE_RESUME            1000
#endif

#ifndef USB_NOTIFY_HUB_PWR_LOSS
#define USB_NOTIFY_HUB_PWR_LOSS         1001
#endif

#ifndef USB_NOTIFY_HUB_RESET
#define USB_NOTIFY_HUB_RESET            1002
#endif

#ifndef USB_NOTIFY_RESET_PORT_ASYNC
#define USB_NOTIFY_RESET_PORT_ASYNC     1004
#endif

#ifndef USB_RESET_CONTROLLER_ASYNC
#define USB_RESET_CONTROLLER_ASYNC      1005
#endif


#ifndef IOCTL_INTERNAL_USB_PROPAGATE_RESUME
#define IOCTL_INTERNAL_USB_PROPAGATE_RESUME     CTL_CODE(FILE_DEVICE_USB,  \
                                                    USB_PROPAGATE_RESUME, \
                                                    METHOD_NEITHER,  \
                                                    FILE_ANY_ACCESS)
#endif

#ifndef IOCTL_INTERNAL_USB_NOTIFY_HUB_PWR_LOSS  
#define IOCTL_INTERNAL_USB_NOTIFY_HUB_PWR_LOSS  CTL_CODE(FILE_DEVICE_USB,  \
                                                    USB_NOTIFY_HUB_PWR_LOSS, \
                                                    METHOD_NEITHER,  \
                                                    FILE_ANY_ACCESS)
#endif

#ifndef IOCTL_INTERNAL_USB_RESET_PORT_ASYNC
#define IOCTL_INTERNAL_USB_RESET_PORT_ASYNC     CTL_CODE(FILE_DEVICE_USB,  \
                                                    USB_NOTIFY_RESET_PORT_ASYNC, \
                                                    METHOD_NEITHER,  \
                                                    FILE_ANY_ACCESS)
#endif

#ifndef IOCTL_INTERNAL_USB_RESET_NOTIFICATION_REQUEST
#define IOCTL_INTERNAL_USB_RESET_NOTIFICATION_REQUEST     CTL_CODE(FILE_DEVICE_USB,  \
                                                    USB_NOTIFY_HUB_RESET, \
                                                    METHOD_NEITHER,  \
                                                    FILE_ANY_ACCESS)
#endif

#ifndef IOCTL_INTERNAL_USB_RESET_CONTROLLER_ASYNC
#define IOCTL_INTERNAL_USB_RESET_CONTROLLER_ASYNC CTL_CODE(FILE_DEVICE_USB,  \
                                                    USB_RESET_CONTROLLER_ASYNC, \
                                                    METHOD_NEITHER,  \
                                                    FILE_ANY_ACCESS)
#endif

#pragma warning(push)
#pragma warning(disable: 4214) // bit field types other than int
#pragma warning(disable: 4201) // nameless struct/union

typedef struct _USB_RESET_FLAGS {
    ULONG       ControllerProgrammingLost:1;
    ULONG       DevicePowerCycled:1;
} USB_RESET_FLAGS, *PUSB_RESET_FLAGS;

#define IOCTL_INTERNAL_USB_SET_FUNCTION_HANDLE_DATA CTL_CODE(FILE_DEVICE_USBEX,\
                                                     USB_SET_FUNCTION_HANDLE_DATA, \
                                                     METHOD_NEITHER, \
                                                     FILE_ANY_ACCESS)

typedef struct _USB_FUNCTION_DATA {
    USHORT                  Version;
    USHORT                  Size;
    USBD_FUNCTION_HANDLE    UsbdFunctionHandle;
    PDEVICE_OBJECT          PhysicalDeviceObject;
} USB_FUNCTION_DATA, *PUSB_FUNCTION_DATA;

#pragma warning(pop)


#define IOCTL_USB_GET_PORT_STATUS  \
                                            CTL_CODE(FILE_DEVICE_USB,  \
                                            USB_GET_PORT_STATUS, \
                                            METHOD_BUFFERED,  \
                                            FILE_ANY_ACCESS)
                                            
typedef struct _USB_PORT_STATUS_INFORMATION {
    ULONG                   ConnectionIndex;  
    USB_PORT_STATUS         PortStatus;          
} USB_PORT_STATUS_INFORMATION, *PUSB_PORT_STATUS_INFORMATION;


//
// This Power control code is used to inform the PEP about the state of 
// the root ports of the host controller. PEP can use this information
// to optimize power when the controller subsequently enters a Dx state
//                                        
// {C0C3A6AF-0A87-4f41-A51D-21BDE2EBBFB3}
//
DEFINE_GUID(GUID_HC_SUB_DX_STATE, 
0xc0c3a6af, 0xa87, 0x4f41, 0xa5, 0x1d, 0x21, 0xbd, 0xe2, 0xeb, 0xbf, 0xb3);

typedef enum _SUB_DX_STATE {
    SubDxStateInvalid = 0,
    //
    // The connected state indicates that there is at least one port
    // on the controller that has a device connected to it
    //
    SubDxStateConnected = 1,
    //
    // The disconnected state indicates that none of the ports 
    // on the controller have device connected to them
    //
    SubDxStateDisconnected = 2
} SUB_DX_STATE;

#endif
