/*++

Copyright (c) 1999  Microsoft Corporation

Module Name:

    usbpriv.h

Abstract:

Environment:

    Kernel & user mode

Revision History:

    10-30-01 : created

--*/

#ifndef   __HCDIGUID_H__
#define   __HCDIGUID_H__

/* guid for miniport idle status pass-thru interface */
/* {e1b9c71c-0e27-4b0a-8383-21699c2fd3ed}*/

DEFINE_GUID(GUID_IDLE_STATUS, 
0xe1b9c71c, 0x0e27, 0x4b0a, 0x83, 0x83, 0x21, 0x69, 0x9c, 0x2f, 0xd3, 0xed);


/* guid for logging HC device data to ETW via pass-thru interface */
/* {f5293e35-debb-49b3-a5f6-81298dd62d0a}*/

DEFINE_GUID(GUID_HC_DEVICE_DATA, 
0xf5293e35, 0xdebb, 0x49b3, 0xa5, 0xf6, 0x81, 0x29, 0x8d, 0xd6, 0x2d, 0x0a);

/*
    This api returns a single dword that reflects the current state of the async, periodic and run bits 
    on the host controllers.

    This API is used to validate the asyn-idle periodic-idle and selective suspend features for usb host 
    controllers.
    
*/

#define USBHC_RUN                       0x00000001
#define USBHC_PERIODIC_LIST_ENABLE      0x00000002
#define USBHC_ASYNC_LIST_ENABLE         0x00000004
#define USBHC_HIGHSPEED                 0x00000008


#endif /* __HCDIGUID_H__ */
