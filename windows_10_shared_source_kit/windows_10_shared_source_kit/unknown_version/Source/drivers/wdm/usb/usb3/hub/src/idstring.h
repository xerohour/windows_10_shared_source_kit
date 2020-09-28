//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: idstring.c */

#define MSCOMPATID_SIZE 8


#ifndef GUID_STR_LENGTH
#define GUID_STR_LENGTH     38  // "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"
#endif  // GUID_STR_LENGTH


#define USB_STORAGE_SUBCLASS_SCSI 0x06
#define USB_STORAGE_PROTOCOL_UAS 0x62

//
// This particualar way of conversion is for compat with Win7
// stack. We know that it does not deal correctly with values 
// higher than 9 i.e. A will translate to ':' instead of 'A'. 
// But this is how the Win7 stack did. 
//
#define CONVERT_BCDDEVICE_FROM_HEX_TO_STRING( _BcdDeviceUshort, _BcdDeviceString )\
    _BcdDeviceString[0] = (_BcdDeviceUshort >> 12) + '0'; \
    _BcdDeviceString[1] = ((_BcdDeviceUshort >> 8) & 0xf) + '0'; \
    _BcdDeviceString[2] = ((_BcdDeviceUshort >> 4) & 0xf) + '0'; \
    _BcdDeviceString[3] = (_BcdDeviceUshort & 0xf) + '0'; \
    _BcdDeviceString[4] = '\0';

VOID
HUBID_FreeID(
    __in
        PUSB_ID_STRING IdString
    );

NTSTATUS
HUBID_BuildDeviceID(
    __in
        PDEVICE_CONTEXT DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    );


NTSTATUS
HUBID_BuildHardwareID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    );


NTSTATUS
HUBID_BuildDeviceCompatibleID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    );


NTSTATUS
HUBID_BuildClassCompatibleID(
    __in
        PDEVICE_CONTEXT DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    );


NTSTATUS
HUBID_BuildCompatibleID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    );

VOID
HUBID_BuildContainerID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PWDFDEVICE_INIT     DeviceInit
    );


VOID
HUBID_BuildUxdPnpId(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PUNICODE_STRING     IdString
    );

NTSTATUS
HUBID_CopyIDString(
    __in
        PUSB_ID_STRING      OutputIdString,
    __inout
        PUSB_ID_STRING      InputIdString
    );
