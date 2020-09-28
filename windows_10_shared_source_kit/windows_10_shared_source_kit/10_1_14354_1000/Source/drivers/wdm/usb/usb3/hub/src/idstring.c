/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    idstring.c


Abstract:

    Code to generate PnP id strings.

    Some notes on PDO Id generation:

    DeviceId strings:

        USB\VID_nnnn&PID_nnnn

        VID_nnnn = is the hex value from the 'idVendor' field of the
                   device descriptor
        PID_nnnn = is the hex value from the 'idProduct' field of the device
                   descriptor

        USB\VID_0000&PID_0000  is generated for an unknown device

...........................................................................................

    HardwareId strings:

        USB\VID_nnnn&PID_nnnn&REV_nnnn
        USB\VID_nnnn&PID_nnnn

        VID_nnnn = is the hex string value from the 'idVendor' field of the
                   device descriptor
        PID_nnnn = is the hex string value from the 'idProduct' field from the
                   device descriptor
        REV_nnnn = string value from the BcdDevice Field of the device
                   descriptor


        USB\UNKNOWN is generated for an unknown device

...........................................................................................

    CompatibleId strings:

        one of two sets of compatible ids can generated DevClass and Class

------->Device Class Compatible Ids:

        *DevClass Ids are generated for composite devices
            devices with 1 configuration and
            >1 interfaces and
            devclass code in devicedescriptor == 0

            the string generated is a constant

        USB\DevClass_00&SubClass_00&Prot_00
        USB\DevClass_00&SubClass_00
        USB\DevClass_00
        USB\COMPOSITE

        DevClass_00
        SubClass_00
        Prot_00

------->Class Compatible Ids:

        *Class Ids are generated for non-composite devices.  The interface
        descriptor used for the ids is the first one found.


        USB\MS_COMP_s&MS_SUBCOMP_s } -- optional from MS OS descriptor
        USB\MS_COMP_s              }
        USB\Class_nn&SubClass_nn&Prot_nn
        USB\Class_nn&SubClass_nn
        USB\Class_nn

        Class_nn    = hex string value from interface descriptor,
                      bInterfaceClass
        SubClass_nn = hex string value from interface descriptor,
                      bInterfaceSubclass
        Prot_nn     = hex string value from interface descriptor,
                      bInterfaceProtocol


        USB\UNKNOWN is generated for an unknown device

Author:

    jdunn

Environment:

    Kernel mode

Revision History:
     3-10-2002
     4-6-2010 - ported from 2.0 stack


--*/


#include "pch.h"
#include <bcrypt.h>
#include <ntstrsafe.h>
#include "idstring.tmh"

#define MAX_ID_LEN 128
// {4B06FD46-C84E-4664-9C65-0C86D9047A0C}
static const GUID GUID_USB_CONTAINER_ID_NAME_SPACE =
{ 0x4b06fd46, 0xc84e, 0x4664, { 0x9c, 0x65, 0xc, 0x86, 0xd9, 0x4, 0x7a, 0xc } };

//
// used to convert id strings from descriptors
//

USHORT
HUBID_Short2Bcd(
    __in
        USHORT Value
    )
/*++

Routine Description:

    converts a Short to its BCD equivalent

Arguments:

    Value - a USHORT value

Return Value:

    The BCD equivalent of Value

--*/
{
    short tmp, x = 1000;
    short bcd = 0;

    if (Value>9999) {
        Value = 0;
    }

    do {
        tmp = (Value-(Value%x))/x;
        Value = Value-tmp*x;
        bcd = ((bcd <<4) | tmp);
        x = x/10;
    } while (x);

    return bcd;
}



NTSTATUS
HUBID_AppendStringToIDString(
    __in
        PCUNICODE_STRING    UnicodeString,
    __inout
        PUSB_ID_STRING      IdString
    )
/*++

Routine Description:

    Appends the specified UnicodeString into the specified
    IdString.  If the buffer for IdString is not big enough to hold the 
    string, the buffer will be freed, and a new one will be allocated.  

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS        status;
    ULONG           length;
    ULONG           prevLength;
    PUCHAR          tmp;
    PWCHAR          oldBuffer;


    TRY {

        //
        // compute the length. The length in the IdString is the actual buffer 
        // length (including all nulls, which we assume there are two).  
        // The UnicodeStringLength does NOT include ending NULLs.        
        //
        if (IdString->Buffer == NULL) {
            prevLength = 0;
        } else {
            //
            // remove the extra terminating NULL.
            prevLength = IdString->LengthInBytes - sizeof(WCHAR);
        }

        length = prevLength + UnicodeString->Length + 2*sizeof(WCHAR);

        tmp = NULL;
        tmp = ExAllocatePoolWithTag(NonPagedPool,
                                    length,
                                    USBHUB3_DEVICE_TAG);

        if (tmp == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(tmp, length);


        if (prevLength > 0) {
            RtlCopyMemory(tmp, 
                          IdString->Buffer,
                          prevLength);
        }

        RtlCopyMemory(&tmp[prevLength],
                      UnicodeString->Buffer,
                      UnicodeString->Length);


        oldBuffer = IdString->Buffer;
        IdString->Buffer = (PWCHAR)tmp;
        IdString->LengthInBytes = length;
        if (oldBuffer) {
            ExFreePoolWithTag(oldBuffer, USBHUB3_DEVICE_TAG);
        }
        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}




VOID
HUBID_FreeID(
    __in
        PUSB_ID_STRING IdString
    )
/*++

Routine Description:

    Frees and ID string

Return Value:

    VOID

--*/
{

    if (IdString->Buffer) {
        NT_ASSERT(IdString->LengthInBytes);
        ExFreePoolWithTag(IdString->Buffer, USBHUB3_DEVICE_TAG);
        IdString->Buffer = NULL;
        IdString->LengthInBytes = 0;
    }
}

typedef enum {
    DeviceID,
    HardwareID,
    CompatID,
    ContainerID
} HUBID_PNP_STRING_TYPE, *PHUBID_PNP_STRING_TYPE;

NTSTATUS
HUBID_AssignIDString(
    __in
        PCUNICODE_STRING        UnicodeString,
    __in
        HUBID_PNP_STRING_TYPE   StringType,
    __in_opt
        PWDFDEVICE_INIT         DeviceInit,
    __inout_opt
        PUSB_ID_STRING          IDString
    )
/*++

Routine Description:

    Assigns the specified string in the DeviceInit and/or creates an IDString
    for the specified unicode string

Return Value:

    VOID

--*/
{
    NTSTATUS        status;

    TRY {

        status = STATUS_SUCCESS;

        if (DeviceInit != NULL) {

            switch(StringType) {
            case DeviceID:

                status = WdfPdoInitAssignDeviceID(DeviceInit,
                                                  UnicodeString);
                break;
            case HardwareID:
                status = WdfPdoInitAddHardwareID(DeviceInit,
                                                 UnicodeString);

                break;

            case CompatID:
                status = WdfPdoInitAddCompatibleID(DeviceInit,
                                                   UnicodeString);

                break;

            case ContainerID:
                status = WdfPdoInitAssignContainerID(DeviceInit,
                                                     UnicodeString);

                break;

            default:
                NT_ASSERT(DeviceInit == NULL);
                break;

            }
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        if (IDString != NULL) {

            status = HUBID_AppendStringToIDString(UnicodeString,
                                                  IDString);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

    } FINALLY {


    } 

    return status;

}


VOID
HUBID_BuildUxdPnpId(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PUNICODE_STRING     IdString
    )
/*++

Routine Description:

    Retrieves the PNP ID for a PDO based on a GUID

Arguments:

Return Value:

   NTSTATUS
   
   SUCCESS if the key exists, failure otherwise

--*/
{
    NTSTATUS                status;
    PWCHAR                  multiSzBuffer;
    WDFSTRING               uxdIdString;

    TRY {

        NT_ASSERT(DeviceContext->DeviceFlags.DeviceReservedUXD == 1);

        uxdIdString = NULL;

        status = WdfStringCreate(NULL,
                                 WDF_NO_OBJECT_ATTRIBUTES,
                                 &uxdIdString);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_WARNING,
                       Device,
                       "Failed to create string for UxD PnP ID %!STATUS!",
                       status);
            LEAVE;
        }

        status = HUBREG_GetUxdPnpValue(DeviceContext,
                                       DeviceContext->UxdSettings.PnpGuid,
                                       uxdIdString);

        if (NT_SUCCESS(status) && (uxdIdString != NULL)) {

            WdfStringGetUnicodeString(uxdIdString, IdString);

            //
            // Convert to a multi-string by adding a terminating NULL
            //
            multiSzBuffer = ExAllocatePoolWithTag(NonPagedPool,
                                                  IdString->Length + sizeof(UNICODE_NULL),
                                                  USBHUB3_DEVICE_TAG);

            if (multiSzBuffer == NULL) {
                DbgTraceTo(DeviceContext->PortContext->IfrLog,
                           TL_WARNING,
                           Device,
                           "Failed to allocate multi-string for UxD PnP ID");
                LEAVE;
            }

            RtlZeroMemory(multiSzBuffer,
                          IdString->Length + sizeof(UNICODE_NULL));

            RtlCopyMemory(multiSzBuffer, 
                          IdString->Buffer, 
                          IdString->Length);
            
            IdString->Buffer = multiSzBuffer;
            IdString->Length += sizeof(UNICODE_NULL);
        }
    
    } FINALLY {
        
        if (uxdIdString != NULL) {
            WdfObjectDelete(uxdIdString);
        }
    }
}


NTSTATUS
HUBID_BuildDeviceID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString
    )
/*++

Routine Description:

    Generates the device id string:

        USB\VID_nnnn&PID_nnnn\0\0

    If device is unknown we generate the default:

        USB\VID_0000&PID_0000\0\0

Return Value:

    NTSTATUS

--*/
{
    USHORT      idVendor;
    USHORT      idProduct;
    NTSTATUS    status;
    DECLARE_UNICODE_STRING_SIZE(deviceID, sizeof(L"USB\\VID_nnnn&PID_nnnn\0"));
    DECLARE_UNICODE_STRING_SIZE(nonFunctionalID, sizeof(L"USB\\VID_nnnn&PID_nnnn_NON_FUNCTIONAL\0"));

    TRY {

        if (IDString != NULL) {
            RtlZeroMemory(IDString,
                          sizeof(USB_ID_STRING));
        }

        if (DeviceContext->DeviceStateFlags.DeviceIsKnown) {

            if (DeviceContext->DeviceFlags.DeviceReservedUXD &&
                (DeviceContext->DeviceFlags.DeviceIsHub == 0)) {

                NT_ASSERT(DeviceContext->UxdId.Buffer != NULL);

                status = HUBID_AssignIDString(&DeviceContext->UxdId,
                                              DeviceID,
                                              DeviceInit,
                                              IDString);
                
                LEAVE;
            }

            idVendor = DeviceContext->DeviceDescriptor.idVendor;
            idProduct = DeviceContext->DeviceDescriptor.idProduct;

        } else if (DeviceContext->DeviceStateFlags.ConfigDescIsValid == 1) {
            idVendor = DeviceContext->DeviceDescriptor.idVendor;
            idProduct = DeviceContext->DeviceDescriptor.idProduct;
        } else {

            idVendor = 0;

            switch (DeviceContext->EnumMsgId) {
            case USBENUM_PORT_RESET_FAILURE:
                idProduct = EnumFailurePid_ResetFailure;
                break;

            case USBENUM_DEVICE_DESCRIPTOR_FAILURE:
                idProduct = EnumFailurePid_DeviceDescriptorFailure;
                break;

            case USBENUM_CONFIG_DESCRIPTOR_FAILURE:
                idProduct = EnumFailurePid_ConfigDescriptorFailure;
                break;

            case USBENUM_SET_ADDRESS_FAILURE:
                idProduct = EnumFailurePid_SetAddressFailure;
                break;

            case USBENUM_BAD_DEVICE_DESCRIPTOR:
                idProduct = EnumFailurePid_BadDeviceDesc;
                break;

            case USBENUM_BAD_CONFIG_DESCRIPTOR:
                idProduct = EnumFailurePid_BadConfigDesc;
                break;

            case USBENUM_PORT_LINK_SSINACTIVE:
                idProduct = EnumFailurePid_PortInSSInactive;
                break;

            case USBENUM_PORT_LINK_COMPLIANCE_MODE:
                idProduct = EnumFailurePid_PortInComplianceMode;
                break;

            default:
                idProduct = EnumFailurePid_UnknownDevice;
                break;
            }
        }

        if (DeviceContext->DeviceHackFlags.NonFunctional == 1) {

            status = RtlUnicodeStringPrintf(&nonFunctionalID,
                                            L"USB\\VID_%04X&PID_%04X_NON_FUNCTIONAL\0", 
                                            idVendor,
                                            idProduct);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            status = HUBID_AssignIDString(&nonFunctionalID,
                                          DeviceID,
                                          DeviceInit,
                                          IDString);
        
            if (!NT_SUCCESS(status)) {
                LEAVE;

            }
        } else {
        
            status = RtlUnicodeStringPrintf(&deviceID, 
                                            L"USB\\VID_%04X&PID_%04X\0", 
                                            idVendor,
                                            idProduct);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            status = HUBID_AssignIDString(&deviceID,
                                          DeviceID,
                                          DeviceInit,
                                          IDString);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

    } FINALLY {

    }

    return status;

}


DECLARE_CONST_UNICODE_STRING(UnknownDeviceId,                L"USB\\UNKNOWN_DEVICE\0");
DECLARE_CONST_UNICODE_STRING(ResetFailureId,                 L"USB\\RESET_FAILURE\0");
DECLARE_CONST_UNICODE_STRING(DeviceDescriptorFailureId,      L"USB\\DEVICE_DESCRIPTOR_FAILURE\0");
DECLARE_CONST_UNICODE_STRING(ConfigDescriptorFailureId,      L"USB\\CONFIG_DESCRIPTOR_FAILURE\0");
DECLARE_CONST_UNICODE_STRING(SetAddressFailureId,            L"USB\\SET_ADDRESS_FAILURE\0");
DECLARE_CONST_UNICODE_STRING(BadDeviceDescriptorId,          L"USB\\DEVICE_DESCRIPTOR_VALIDATION_FAILURE\0");
DECLARE_CONST_UNICODE_STRING(BadConfigDescriptorId,          L"USB\\CONFIGURATION_DESCRIPTOR_VALIDATION_FAILURE\0");
DECLARE_CONST_UNICODE_STRING(PortLinkSSInactiveId,           L"USB\\PORT_LINK_SSINACTIVE\0");
DECLARE_CONST_UNICODE_STRING(PortLinkComplianceModeId,       L"USB\\PORT_LINK_COMPLIANCE_MODE\0");

PCUNICODE_STRING
HUBID_GetUnknownId(
    __in
        PDEVICE_CONTEXT     DeviceContext
    )
/*++

Routine Description:

    Determines which "unknown device" hardware/compat ID to use

Return Value:

    Pointer to the unknown device hardware/compat ID to use

--*/
{
    switch (DeviceContext->EnumMsgId) {
    case USBENUM_PORT_RESET_FAILURE:
        return &ResetFailureId;

    case USBENUM_DEVICE_DESCRIPTOR_FAILURE:
        return &DeviceDescriptorFailureId;

    case USBENUM_CONFIG_DESCRIPTOR_FAILURE:
        return &ConfigDescriptorFailureId;

    case USBENUM_SET_ADDRESS_FAILURE:
        return &SetAddressFailureId;

    case USBENUM_BAD_DEVICE_DESCRIPTOR:
        return &BadDeviceDescriptorId;

    case USBENUM_BAD_CONFIG_DESCRIPTOR:
        return &BadConfigDescriptorId;

    case USBENUM_PORT_LINK_SSINACTIVE:
        return &PortLinkSSInactiveId;

    case USBENUM_PORT_LINK_COMPLIANCE_MODE:
        return &PortLinkComplianceModeId;

    default:
        return &UnknownDeviceId;
    }
}

NTSTATUS
HUBID_BuildHardwareID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    )
/*++

Routine Description:

    Generates the device hardware id string:

        USB\VID_nnnn&PID_nnnn&REV_nnnn\0
        USB\VID_nnnn&PID_nnnn\0\0

    If device is unknown we generate the default:

        USB\UNKNOWN

Return Value:

    NTSTATUS

--*/
{
        
    USHORT          idVendor;
    USHORT          idProduct;
    UCHAR           bcdRev[5];
    NTSTATUS        status;
    DECLARE_UNICODE_STRING_SIZE(hwID, sizeof(L"USB\\VID_nnnn&PID_nnnn&REV_nnnn\0"));
    PCUNICODE_STRING unknownDeviceId; 


    TRY {

        status = STATUS_SUCCESS;

        if (IDString != NULL) {
            RtlZeroMemory(IDString,
                          sizeof(USB_ID_STRING));
        }

        
        if ((DeviceContext->DeviceStateFlags.DeviceIsKnown == 0) && 
            (DeviceContext->DeviceStateFlags.ConfigDescIsValid == 0)) {

            unknownDeviceId = HUBID_GetUnknownId(DeviceContext);

            status = HUBID_AssignIDString(unknownDeviceId,
                                          HardwareID,
                                          DeviceInit,
                                          IDString);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
            LEAVE;
        }

        if (DeviceContext->DeviceFlags.DeviceReservedUXD &&
            (DeviceContext->DeviceFlags.DeviceIsHub == 0) && 
            (DeviceInit != NULL)) {

            NT_ASSERT(DeviceContext->UxdId.Buffer != NULL);

            status = HUBID_AssignIDString(&DeviceContext->UxdId,
                                          HardwareID,
                                          DeviceInit,
                                          IDString);
            
            LEAVE;
        }

        idVendor = DeviceContext->DeviceDescriptor.idVendor;
        idProduct = DeviceContext->DeviceDescriptor.idProduct;
        
        if (DeviceContext->DeviceDescriptor.bcdUSB <= 0x200) {
            //
            // This particualar way of conversion is for compat with Win7
            // stack. We know that it does not deal correctly with values 
            // higher than 9 i.e. A will translate to ':' instead of 'A'. 
            // But this is how the Win7 stack did and so we keep doing
            // it for existing devices
            //
            CONVERT_BCDDEVICE_FROM_HEX_TO_STRING(DeviceContext->DeviceDescriptor.bcdDevice,
                                                 bcdRev);
            //
            // make USB\VID_nnnn&PID_nnnn&REV_nnnn\0
            //
            status = RtlUnicodeStringPrintf(&hwID, 
                                            L"USB\\VID_%04X&PID_%04X&REV_%S\0",
                                            idVendor,
                                            idProduct,
                                            bcdRev);
        } else {

            status = RtlUnicodeStringPrintf(&hwID, 
                                            L"USB\\VID_%04X&PID_%04X&REV_%04X\0",
                                            idVendor,
                                            idProduct,
                                            DeviceContext->DeviceDescriptor.bcdDevice);

        }

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }


        status = HUBID_AssignIDString(&hwID,
                                      HardwareID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // add USB\VID_nnnn&PID_nnnn\0\0
        //
        status = RtlUnicodeStringPrintf(&hwID, 
                                        L"USB\\VID_%04X&PID_%04X\0",
                                        idVendor,
                                        idProduct);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }


        status = HUBID_AssignIDString(&hwID,
                                      HardwareID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

    }

    return status;
}



NTSTATUS
HUBID_BuildDeviceCompatibleID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    )

/*++

Routine Description:

    Generates the device compatible id string:

        USB\DevClass_00&SubClass_00&Prot_00\0
        USB\DevClass_00&SubClass_00\0
        USB\DevClass_00\0
        USB\COMPOSITE\0\0

Return Value:

    NTSTATUS

--*/
{
    
    NTSTATUS    status;
    DECLARE_CONST_UNICODE_STRING(compatID0, L"USB\\DevClass_00&SubClass_00&Prot_00");
    DECLARE_CONST_UNICODE_STRING(compatID1, L"USB\\DevClass_00&SubClass_00");
    DECLARE_CONST_UNICODE_STRING(compatID2, L"USB\\DevClass_00");
    DECLARE_CONST_UNICODE_STRING(compatID3, L"USB\\COMPOSITE");

    UNREFERENCED_PARAMETER(DeviceContext);

    TRY {

        if (IDString != NULL) {
            RtlZeroMemory(IDString,
                          sizeof(USB_ID_STRING));
        }

        status = HUBID_AssignIDString(&compatID0,
                                      CompatID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = HUBID_AssignIDString(&compatID1,
                                      CompatID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }
        
        status = HUBID_AssignIDString(&compatID2,
                                      CompatID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }
        
        status = HUBID_AssignIDString(&compatID3,
                                      CompatID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }
    

    } FINALLY {

    }

    return status;
}




NTSTATUS
HUBID_BuildHubCompatibleID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString
    )
/*++

Routine Description:

    Generates the hub compatible id string:

        USB\\USB30_HUB\0\0
               or
        USB\\USB20_HUB\0\0

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS    status;
    DECLARE_CONST_UNICODE_STRING(hub20compatID, L"USB\\USB20_HUB");
    DECLARE_CONST_UNICODE_STRING(hub30compatID, L"USB\\USB30_HUB");
    

    TRY {

        status = STATUS_SUCCESS;

        if (IDString != NULL) {
            RtlZeroMemory(IDString,
                          sizeof(USB_ID_STRING));
        }


        switch (DeviceContext->UsbDeviceInfo.DeviceSpeed) {
            case UsbHighSpeed:
            case UsbFullSpeed:
                NT_ASSERT(DeviceContext->DeviceDescriptor.bcdUSB < 0x300);
                status = HUBID_AssignIDString(&hub20compatID,
                                              CompatID,
                                              DeviceInit,
                                              IDString);
                break;

            case UsbSuperSpeed:
                NT_ASSERT(DeviceContext->DeviceDescriptor.bcdUSB >= 0x300);
                status = HUBID_AssignIDString(&hub30compatID,
                                              CompatID,
                                              DeviceInit,
                                              IDString);

                break;
                
            default:
                NT_ASSERTMSG("Invalid Device Speed for Hub", FALSE);
                LEAVE;
        }

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

               

    } FINALLY {

    }

    return status;
}


NTSTATUS
HUBID_BuildClassCompatibleID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    )
/*++

Routine Description:

    Generates the device compatible id string:

        USB\MS_COMP_s&MS_SUBCOMP_s } -- optional from MS OS descriptor
        USB\MS_COMP_s              }
        USB\Class_nn&SubClass_nn&Prot_nn
        USB\Class_nn&SubClass_nn
        USB\Class_nn

Return Value:

    NTSTATUS

--*/
{
    
    USHORT      usbClass;
    USHORT      usbSubClass;
    USHORT      usbProt;
    NTSTATUS    status;
    DECLARE_UNICODE_STRING_SIZE(compatID, MAX_ID_LEN);

    TRY {

        if (IDString != NULL) {
            RtlZeroMemory(IDString,
                          sizeof(USB_ID_STRING));
        }

        if (DeviceContext->CompatIdInterfaceDescriptor != NULL) {

            usbClass = DeviceContext->CompatIdInterfaceDescriptor->bInterfaceClass;
            usbSubClass = DeviceContext->CompatIdInterfaceDescriptor->bInterfaceSubClass;
            usbProt = DeviceContext->CompatIdInterfaceDescriptor->bInterfaceProtocol;

        } else {

            usbClass = usbSubClass = usbProt = 0;

        }

        //
        //USB\MS_COMP_s&MS_SUBCOMP_s
        //USB\MS_COMP_s
        //
        if (DeviceContext->MsOs20Info.Flags.CompatibleId == 1) {

            if ((DeviceContext->MsOs20Info.CompatibleIdDescriptor->SubCompatibleId[0] != '\0') && 
                (DeviceContext->MsOs20Info.CompatibleIdDescriptor->CompatibleId[0] != '\0')) {
                
                status = RtlUnicodeStringPrintf(&compatID, 
                                                L"USB\\MS_COMP_%.8S&MS_SUBCOMP_%.8S\0",
                                                DeviceContext->MsOs20Info.CompatibleIdDescriptor->CompatibleId,
                                                DeviceContext->MsOs20Info.CompatibleIdDescriptor->SubCompatibleId);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

                status = HUBID_AssignIDString(&compatID,
                                              CompatID,
                                              DeviceInit,
                                              IDString);
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

            }

            if (DeviceContext->MsOs20Info.CompatibleIdDescriptor->CompatibleId[0] != '\0') {


                status = RtlUnicodeStringPrintf(&compatID, 
                                                L"USB\\MS_COMP_%.8S\0",
                                                DeviceContext->MsOs20Info.CompatibleIdDescriptor->CompatibleId);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

                status = HUBID_AssignIDString(&compatID,
                                              CompatID,
                                              DeviceInit,
                                              IDString);
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

            }
            
        } else if (DeviceContext->MsExtConfigDesc) {

            if (DeviceContext->MsExtConfigDesc->Function[0].SubCompatibleID[0] != '\0') {
                
                status = RtlUnicodeStringPrintf(&compatID, 
                                                L"USB\\MS_COMP_%.8S&MS_SUBCOMP_%.8S\0",
                                                DeviceContext->MsExtConfigDesc->Function[0].CompatibleID,
                                                DeviceContext->MsExtConfigDesc->Function[0].SubCompatibleID);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

                status = HUBID_AssignIDString(&compatID,
                                              CompatID,
                                              DeviceInit,
                                              IDString);
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

            }

            if (DeviceContext->MsExtConfigDesc->Function[0].CompatibleID[0] != '\0') {


                status = RtlUnicodeStringPrintf(&compatID, 
                                                L"USB\\MS_COMP_%.8S\0",
                                                DeviceContext->MsExtConfigDesc->Function[0].CompatibleID);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

                status = HUBID_AssignIDString(&compatID,
                                              CompatID,
                                              DeviceInit,
                                              IDString);
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

            }

        }

        //
        //USB\Class_nn&SubClass_nn&Prot_nn\0
        //
        status = RtlUnicodeStringPrintf(&compatID, 
                                        L"USB\\Class_%02X&SubClass_%02X&Prot_%02X\0",
                                        usbClass,
                                        usbSubClass,
                                        usbProt);
                                                

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = HUBID_AssignIDString(&compatID,
                                      CompatID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }


        //
        //USB\Class_nn&SubClass_nn\0
        //
        status = RtlUnicodeStringPrintf(&compatID, 
                                        L"USB\\Class_%02X&SubClass_%02X\0",
                                        usbClass,
                                        usbSubClass);
                                                

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = HUBID_AssignIDString(&compatID,
                                      CompatID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        //USB\Class_nn\0\0
        //
        status = RtlUnicodeStringPrintf(&compatID, 
                                        L"USB\\Class_%02X\0",
                                        usbClass);
                                                

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = HUBID_AssignIDString(&compatID,
                                      CompatID,
                                      DeviceInit,
                                      IDString);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

    }
    return status;
}


NTSTATUS
HUBID_BuildCompatibleID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in_opt
        PWDFDEVICE_INIT     DeviceInit,
    __out_opt
        PUSB_ID_STRING      IDString    
    )
/*++

Routine Description:

    Generates the device compatible id string:

    If the device is unknown we generate the default:
        USB\UNKNOWN

Return Value:

    NTSTATUS

--*/
{    
    NTSTATUS            status;
    PCUNICODE_STRING    unknownDeviceId;
    DECLARE_CONST_UNICODE_STRING(billboardCompatID, L"USB\\Class_11&SubClass_00&Prot_00\0");

    TRY {

        if (IDString != NULL) {
            RtlZeroMemory(IDString,
                          sizeof(USB_ID_STRING));
        }

        if ((DeviceContext->DeviceStateFlags.DeviceIsKnown == 0) && 
            (DeviceContext->DeviceStateFlags.ConfigDescIsValid == 0)) {

            unknownDeviceId = HUBID_GetUnknownId(DeviceContext);

            status = HUBID_AssignIDString(unknownDeviceId,
                                          CompatID,
                                          DeviceInit,
                                          IDString);
            LEAVE;
            
        }

        if (DeviceContext->DeviceFlags.DeviceIsComposite) {

            status = HUBID_BuildDeviceCompatibleID(DeviceContext,
                                                   DeviceInit,
                                                   IDString);
        
        } else if (DeviceContext->DeviceFlags.DeviceIsHub) {

            status = HUBID_BuildHubCompatibleID(DeviceContext,
                                                DeviceInit,
                                                IDString);

        } else if (DeviceContext->DeviceFlags.DeviceIsBillboard) {

            //
            // Billboard device has no interfaces, we we need to
            // create the comapt id using the Ids from the
            // device descriptor
            //
            status = HUBID_AssignIDString(&billboardCompatID,
                                          CompatID,
                                          DeviceInit,
                                          IDString);

        } else {

            status = HUBID_BuildClassCompatibleID(DeviceContext,
                                                  DeviceInit,
                                                  IDString);
        }

    } FINALLY {

    }

    return status;
}

VOID
HUBID_BuildContainerID(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PWDFDEVICE_INIT     DeviceInit
    )
/*++

Routine Description:

    Determines if the hub driver should specify a container
    ID, and if that container ID was one retrieved from the device
    or if it should generates a Container ID based on the device's VID, PID,
    revision, and serial number

Return Value:

    VOID

--*/
{

    NTSTATUS                status;
    BCRYPT_HASH_HANDLE      hHash;
    DWORD                   dwcb;
    DWORD                   dwcbResult;
    PUCHAR                  pbHashObject;
    PUCHAR                  pbHash;
    BCRYPT_ALG_HANDLE       hContainerIDHashAlg;
    ULONG                   length;
    UNICODE_STRING          uniqueString;
    UNICODE_STRING          containerIDString;
    UCHAR                   bcdRev[5];

    TRY {

        status = STATUS_SUCCESS;
        hHash = NULL;
        dwcb = 0;
        dwcbResult = 0;
        pbHashObject = NULL;
        pbHash = NULL;
        hContainerIDHashAlg = NULL;    

        RtlZeroMemory(&uniqueString,
                      sizeof(uniqueString));

        RtlZeroMemory(&containerIDString,
                      sizeof(containerIDString));

        if (DeviceContext->DeviceFlags.DeviceNotRemovable == 1) {
            //
            // Don't report container IDs for non-removable devices
            //
            LEAVE;
        }

        //
        // See if we need to generate a ContainerId from the VID, PID
        //
        if ((DeviceContext->DeviceFlags.BOSContainerIdValid == 0) &&
            (DeviceContext->DeviceFlags.ContainerIdValid == 0) &&
            (DeviceContext->DeviceFlags.SerialNumber == 1) && 
            (DeviceContext->DeviceFlags.DeviceNotRemovable == 0)) {
        
            length = sizeof(L"vvvvpppprrrr\0") + DeviceContext->SerialNumberId.LengthInBytes;;

            uniqueString.Buffer = ExAllocatePoolWithTag(NonPagedPool,
                                                        length,
                                                        USBHUB3_DEVICE_TAG);

            if (uniqueString.Buffer == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            uniqueString.MaximumLength = (USHORT)length;
            uniqueString.Length = 0;

            
            if (DeviceContext->DeviceDescriptor.bcdUSB <= 0x200) {
                //
                // This particualar way of conversion is for compat with Win7
                // stack. We know that it does not deal correctly with values 
                // higher than 9 i.e. A will translate to ':' instead of 'A'. 
                // But this is how the Win7 stack did and so we keep doing
                // it for existing devices
                //
                CONVERT_BCDDEVICE_FROM_HEX_TO_STRING(DeviceContext->DeviceDescriptor.bcdDevice,
                                                     bcdRev);
                status = RtlUnicodeStringPrintf(&uniqueString,
                                                L"%.4X%.4X%S",
                                                DeviceContext->DeviceDescriptor.idVendor,
                                                DeviceContext->DeviceDescriptor.idProduct,
                                                bcdRev);
            } else {
                
                status = RtlUnicodeStringPrintf(&uniqueString,
                                                L"%.4X%.4X.%4X",
                                                DeviceContext->DeviceDescriptor.idVendor,
                                                DeviceContext->DeviceDescriptor.idProduct,
                                                DeviceContext->DeviceDescriptor.bcdUSB);

            }


            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            //
            // add the serial number to the string if the buffer is valid
            //
            if (DeviceContext->SerialNumberId.Buffer) {

                if (DeviceContext->DeviceStateFlags.DecorateSerialNumber == 1) {

                    status = RtlUnicodeStringCbCatStringN(&uniqueString,
                                                          (PWCHAR)((PUCHAR)DeviceContext->SerialNumberId.Buffer + USB_SERIAL_NUMBER_DECORATION_SIZE),
                                                          DeviceContext->SerialNumberId.LengthInBytes - (USB_SERIAL_NUMBER_DECORATION_SIZE + sizeof(WCHAR)));
                } else {
                    status = RtlUnicodeStringCbCatStringN(&uniqueString,
                                                          DeviceContext->SerialNumberId.Buffer,
                                                          DeviceContext->SerialNumberId.LengthInBytes);
                }

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

            }


            //
            // Open the the SHA1 algorithm provider
            //
            status = BCryptOpenAlgorithmProvider(&hContainerIDHashAlg,
                                                 BCRYPT_SHA1_ALGORITHM,
                                                 MS_PRIMITIVE_PROVIDER,
                                                 BCRYPT_PROV_DISPATCH);

            if (!NT_SUCCESS(status)) {
                hContainerIDHashAlg = NULL;
                LEAVE;
            }

            //
            // Create the initial hash object
            //
            status = BCryptGetProperty(hContainerIDHashAlg,
                                       BCRYPT_OBJECT_LENGTH,
                                       (PUCHAR)&dwcb,
                                       sizeof(dwcb),
                                       &dwcbResult,
                                       0);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            if (sizeof(DWORD) != dwcbResult) {
                status = STATUS_INVALID_BUFFER_SIZE;
                LEAVE;
            }


            pbHashObject = ExAllocatePoolWithTag(NonPagedPool,
                                                 dwcb,
                                                 USBHUB3_DEVICE_TAG);
            if (NULL == pbHashObject) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            status = BCryptCreateHash(hContainerIDHashAlg,
                                      &hHash,
                                      pbHashObject,
                                      dwcb,
                                      NULL,
                                      0,
                                      0);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            //
            // Hash in the USB Container ID name space
            //
            status = BCryptHashData(hHash,
                                    (PUCHAR)&GUID_USB_CONTAINER_ID_NAME_SPACE,
                                    sizeof(GUID_USB_CONTAINER_ID_NAME_SPACE),
                                    0);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            //
            // Hash in the unique USB string
            //
            status = BCryptHashData(hHash,
                                    (PUCHAR)uniqueString.Buffer,
                                    (ULONG) uniqueString.Length,
                                    0);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            //
            // Get the resulting hash
            //
            status = BCryptGetProperty(hContainerIDHashAlg,
                                       BCRYPT_HASH_LENGTH,
                                       (PUCHAR)&dwcb,
                                       sizeof(dwcb),
                                       &dwcbResult,
                                       0);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            if (sizeof(DWORD) != dwcbResult) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            if (sizeof(GUID) > dwcb) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            pbHash = ExAllocatePoolWithTag(NonPagedPool,
                                           dwcb,
                                           USBHUB3_DEVICE_TAG);
            if (NULL == pbHash) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            status = BCryptFinishHash(hHash,
                                      (PUCHAR)pbHash,
                                      dwcb,
                                      0);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            //
            // Let's now copy the resulting hash into the container ID
            // and set the version number and the variant fields.
            //

            
            RtlCopyMemory(&DeviceContext->ContainerId,
                          pbHash,
                          sizeof(GUID));

            DeviceContext->ContainerId.Data3 &= 0x0FFF;     // Clear the version number
            DeviceContext->ContainerId.Data3 |= (5 << 12);  // Set version = (name-based SHA1) = 5

            DeviceContext->ContainerId.Data4[0] &= 0x3F;    // clear the variant bits
            DeviceContext->ContainerId.Data4[0] |= 0x80;            
            
            INTERLOCKED_SET_BIT_FIELD(USB_DEVICE_PROPERTIES, &DeviceContext->DeviceFlags, ContainerIdValid);
        }

        //
        // If either of these bits are set, then the container ID 
        // field is valid.
        // 
        if ((DeviceContext->DeviceFlags.ContainerIdValid == 0) &&
            (DeviceContext->DeviceFlags.BOSContainerIdValid == 0)) {

            //
            // Lets let PnP generate a container ID
            //
            LEAVE;

        }
        status = RtlStringFromGUID(&DeviceContext->ContainerId,
                                   &containerIDString);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }
        

        status = WdfPdoInitAssignContainerID(DeviceInit,
                                             &containerIDString);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }


    } FINALLY {

        if (hHash) {
            BCryptDestroyHash(hHash);
        }

        if (pbHash) {
            ExFreePoolWithTag(pbHash, USBHUB3_DEVICE_TAG);
        }

        if(pbHashObject){
            ExFreePoolWithTag(pbHashObject, USBHUB3_DEVICE_TAG);
        }

        if (uniqueString.Buffer) {
            ExFreePoolWithTag(uniqueString.Buffer, USBHUB3_DEVICE_TAG);
        }

        if (containerIDString.Buffer) {
            RtlFreeUnicodeString(&containerIDString);
        }

        if (hContainerIDHashAlg) {
            BCryptCloseAlgorithmProvider(hContainerIDHashAlg,
                                         0);
        }

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(DeviceContext->PortContext->IfrLog,
                       TL_ERROR,
                       Device,
                       "HUBID_BuildContainerID failing with status %!STATUS!",
                       status);
        }

    }
    return;
}


NTSTATUS
HUBID_CopyIDString(
    __in
        PUSB_ID_STRING      OutputIdString,
    __inout
        PUSB_ID_STRING      InputIdString
    )
/*++

Routine Description:

    Allocates memory for and copies a USB_ID_STRING

Arguments:

Return Value:

   NTSTATUS

--*/
{
    NTSTATUS status;

    TRY {

        status = STATUS_SUCCESS;

        RtlZeroMemory(OutputIdString, sizeof(USB_ID_STRING));

        if ((InputIdString->Buffer == NULL) || (InputIdString->LengthInBytes == 0)) {
            LEAVE;
        }

        RtlCopyMemory(OutputIdString,
                      InputIdString,
                      sizeof(USB_ID_STRING));

        OutputIdString->Buffer = ExAllocatePoolWithTag(NonPagedPool,
                                                       InputIdString->LengthInBytes,
                                                       USBHUB3_DEVICE_TAG);

        if (OutputIdString->Buffer == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }
        
        RtlCopyMemory(OutputIdString->Buffer,
                      InputIdString->Buffer,
                      InputIdString->LengthInBytes);

    } FINALLY {
    
    }

    return status;
}
