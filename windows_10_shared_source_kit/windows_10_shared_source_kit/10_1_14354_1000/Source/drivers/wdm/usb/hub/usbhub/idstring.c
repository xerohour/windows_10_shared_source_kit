/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    ex.c

     3-10-2002

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

--*/

#include "hubdef.h"
#include "pch.h"

#include <bcrypt.h>

#ifdef HUB_WPP
#include "idstring.tmh"
#endif

CCHAR usbfile_idstring_c[] = "idstring.c";

#define USBFILE usbfile_idstring_c

// {4B06FD46-C84E-4664-9C65-0C86D9047A0C}
static const GUID GUID_USB_CONTAINER_ID_NAME_SPACE = 
{ 0x4b06fd46, 0xc84e, 0x4664, { 0x9c, 0x65, 0xc, 0x86, 0xd9, 0x4, 0x7a, 0xc } };


//
// used to convert id strings from descriptors
//

USHORT
Short2Bcd(
    USHORT Value
    )
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


const UCHAR Nibble[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                       'A','B', 'C', 'D', 'E', 'F'};

#define MAKEID_HEX  0
#define MAKEID_BCD  1
#define MAKEID_MSDESC 2

PWCHAR
UsbhMakeId(
    __in ULONG IdType,
    const WCHAR *IdFormatString,
    __in_bcount_opt(*AppendToStringLength) PWCHAR AppendToString,
    __in PULONG AppendToStringLength,
    __in USHORT NullCount,
    __in USHORT Digits,
    __in USHORT HexId,
    PUCHAR MsCompatId
    )
/*++

Routine Description:

    Given a wide Id format string like "FOOnnnn\0".  Adds the HexId value to
    nnnn as hex, this string is appended to the buffer (AppendToString) passed
    and a new buffer with both strings is returned.

    example:

    in  IdFormatString = L"FOOnnnn\0"
        AppendToString = NULL
        AppendToStringLength = 0;
        NullCount = 0
        Digits = 4
        HexId = 0x123A

    out : FOO123A\0

Irql: PASSIVE

Arguments:
    IdFormatString - template for the id we need to generate

    AppendToString - input string

    AppendToStringLength - PTR, in = length of AppendToString in bytes
                                out = total length of return buffer in bytes

    NullCount - number of additional NULLs to append to the output buffer.

    Digits - 2 or 4

    HextId - hexId to use in place of nnnn or nn

Return Value:

    New buffer with the converted IdString appended to the end of the
    contents of the original buffer (AppendToString).



--*/
/*
    given a wide Id string like "FOOnnnn\0"
    add the HexId value to nnnn as hex
    this string is appended to the buffer passed in

    eg
    in  : FOOnnnn\0 , 0x123A
    out : FOO123A\0
*/
{

    PWCHAR tmp, id;
    PUCHAR p;
    SIZE_T siz, idLen;
    ULONG i;

    idLen = wcslen(IdFormatString)*sizeof(WCHAR);
    siz = idLen+(USHORT)(*AppendToStringLength)+(NullCount*sizeof(WCHAR));

    if(IdType == MAKEID_MSDESC && MsCompatId){
        siz += ((strlen((PCHAR)MsCompatId)-1) * sizeof(WCHAR) );
    }

    UsbhAllocatePool_Z(tmp, NonPagedPool, siz);

    if (tmp == NULL) {
        *AppendToStringLength = 0;
    } else {
        // this takes care of the nulls
        if (AppendToStringLength) {
            RtlCopyMemory(tmp, AppendToString, *AppendToStringLength);
        }
        p = (PUCHAR) tmp;
        p += *AppendToStringLength;
        RtlCopyMemory(p, IdFormatString, idLen);
        id = (PWCHAR) p;
        *AppendToStringLength = (ULONG)siz;

        // now convert the vaules
        while (*id != (WCHAR)'n' && Digits) {
            id++;
        }

        switch (IdType) {
        case MAKEID_HEX:

            switch(Digits) {
            case 2:
                *(id) = NIBBLE_TO_HEX((HexId >> 4) & 0x000f);
                *(id+1) =  NIBBLE_TO_HEX(HexId & 0x000f);
                break;
            case 4:
                *(id) = NIBBLE_TO_HEX(HexId >> 12);
                *(id+1) = NIBBLE_TO_HEX((HexId >> 8) & 0x000f);
                *(id+2) = NIBBLE_TO_HEX((HexId >> 4) & 0x000f);
                *(id+3) =  NIBBLE_TO_HEX(HexId & 0x000f);
                break;
            }
            break;

        case MAKEID_BCD:
            switch(Digits) {
            case 2:
                *(id) = BcdNibbleToAscii((HexId >> 4) & 0x000f);
                *(id+1) =  BcdNibbleToAscii(HexId & 0x000f);
                break;
            case 4:
                *(id) = BcdNibbleToAscii(HexId >> 12);
                *(id+1) = BcdNibbleToAscii((HexId >> 8) & 0x000f);
                *(id+2) = BcdNibbleToAscii((HexId >> 4) & 0x000f);
                *(id+3) =  BcdNibbleToAscii(HexId & 0x000f);
                break;
            }
            break;

        case MAKEID_MSDESC:
            if(MsCompatId){
                for(i=0; i<8; i++){
                    if(MsCompatId[i] == '\0'){
                        break;
                    }
                    *(id+i) = (WCHAR)MsCompatId[i];
                }
            }
            break;
        }
    }

    if (AppendToString != NULL) {
        UsbhFreePool(AppendToString);
    }

    return tmp;
}


VOID
UsbhFreeID(
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Frees an id string

--*/
{
    if (IdString->Buffer) {
        UsbhAssert(NULL, IdString->LengthInBytes);
        UsbhFreePool(IdString->Buffer);
        IdString->Buffer = NULL;
        IdString->LengthInBytes = 0;
    }
}


NTSTATUS
UsbhBuildDeviceID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Generates the device id string:

        USB\VID_nnnn&PID_nnnn\0\0

    If PDO  is NULL we generate the default:

        USB\VID_0000&PID_0000\0\0

--*/
{
    PWCHAR id;
    ULONG length;
    USHORT idVendor, idProduct;
    NTSTATUS nts = STATUS_SUCCESS;

    id = NULL;
    length = 0;

    if (Pdo) {
        PDEVICE_EXTENSION_PDO pdoExt;

        pdoExt = PdoExt(Pdo);

        if(pdoExt->PdoFlags.DeviceReservedUxd &&
           !pdoExt->PdoFlags.DeviceIsHub){

            //
            // This PDO is slated for assignment to a VM, use a custom DeviceID.
            // If we are being called in the context of a switch between the host
            // and a VM, we are protected by the Uxd semaphore.  Otherwise we
            // are being called from SetupDevice and no one else can touch our
            // UXD settings.
            //

            nts = UsbhBuildUxdPnpId(HubFdo,
                                    Pdo,
                                    IdString);

            if(NT_SUCCESS(nts)){
                //
                // ID created successfully
                //

                LOG(HubFdo, LOG_IDSTR, 'diUX', IdString->Buffer, IdString->LengthInBytes);
                DbgTrace((UsbhDebugTrace, "UXD DeviceID :%ws: id(%p) length %d\n", 
                          IdString->Buffer,
                          IdString->Buffer, 
                          IdString->LengthInBytes));
                return STATUS_SUCCESS;
            }
            else{
                //
                // error
                //
                LOG(HubFdo, LOG_IDSTR, 'diXE', 0, nts);
                DbgTrace((UsbhDebugTrace, "UXD DeviceID Not Found\n"));
                nts = STATUS_SUCCESS;
            }
        }

        idVendor = pdoExt->DeviceDescriptor.idVendor;
        idProduct = pdoExt->DeviceDescriptor.idProduct;

    } else {
        idVendor = 0;
        idProduct = 0;
    }

    LOG(HubFdo, LOG_IDSTR, 'dvID', idVendor, idProduct);
    DbgTrace((UsbhDebugTrace, "DeviceID vid = 0x%04.4x pid = 0x%04.4x\n",
                idVendor, idProduct));

    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"USB\\VID_nnnn\0",
                   id,
                   &length,
                   0,  // no NULL
                   4,  // 4 digits
                   idVendor,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'di1!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"&PID_nnnn\0",
                   id,
                   &length,
                   2,  // add double null
                   4,  // 4 digits
                   idProduct,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'di12', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    IdString->Buffer = id;
    IdString->LengthInBytes = length;
    DbgTrace((UsbhDebugTrace, "DeviceID :%ws: %d\n", id, length));

    return STATUS_SUCCESS;

}


NTSTATUS
UsbhBuildHardwareID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Generates the device hardware id string:

        USB\VID_nnnn&PID_nnnn&REV_nnnn\0
        USB\VID_nnnn&PID_nnnn\0\0

    If PDO is NULL the we generate the default:
        USB\UNKNOWN

--*/
{
    PWCHAR id;
    ULONG length;
    USHORT idVendor, idProduct, bcdRev;
    NTSTATUS nts = STATUS_SUCCESS;

    id = NULL;
    length = 0;

    if (Pdo) {
        PDEVICE_EXTENSION_PDO pdoExt;

        pdoExt = PdoExt(Pdo);

        if(pdoExt->PdoFlags.DeviceReservedUxd &&
           !pdoExt->PdoFlags.DeviceIsHub){

            //
            // This PDO is slated for assignment to a VM, use a custom HwID.
            // If we are being called in the context of a switch between the host
            // and a VM, we are protected by the Uxd semaphore.  Otherwise we
            // are being called from SetupDevice and no one else can touch our
            // UXD settings.
            //

            nts = UsbhBuildUxdPnpId(HubFdo,
                                    Pdo,
                                    IdString);

            if(NT_SUCCESS(nts)){
                //
                // ID created successfully
                //

                LOG(HubFdo, LOG_IDSTR, 'hwUX', IdString->Buffer, IdString->LengthInBytes);
                DbgTrace((UsbhDebugTrace, "UXD HardwareID :%ws: id(%p) length %d\n", 
                          IdString->Buffer,
                          IdString->Buffer, 
                          IdString->LengthInBytes));
                return STATUS_SUCCESS;
            }
            else{
                //
                // error
                //
                LOG(HubFdo, LOG_IDSTR, 'hwXE', 0, nts);
                DbgTrace((UsbhDebugTrace, "UXD HardwareID Not Found\n"));
                nts = STATUS_SUCCESS;
            }
        }

        idVendor = pdoExt->DeviceDescriptor.idVendor;
        idProduct = pdoExt->DeviceDescriptor.idProduct;
        bcdRev = pdoExt->DeviceDescriptor.bcdDevice;
        DbgTrace((UsbhDebugTrace, "HardwareID vid = 0x%04.4x pid = 0x%04.4x rev = (bcd)0x%04.4x\n",
            idVendor, idProduct, bcdRev));
    } else {
        idVendor = 0;
        idProduct = 0;

        id = UsbhMakeId(
                   MAKEID_HEX,
                   L"USB\\UNKNOWN\0",
                   id,
                   &length,
                   2,  // double NULL
                   0,  // 0 digits
                   0,
                   NULL);
        if (id) {
            IdString->Buffer = id;
            IdString->LengthInBytes = length;

            LOG(HubFdo, LOG_IDSTR, 'hiU!', id, length);
            DbgTrace((UsbhDebugTrace, "<Unknown> HardwareID :%ws: id(%p) length %d\n", id,
                id, length));
            return STATUS_SUCCESS;
        } else {
            LOG(HubFdo, LOG_IDSTR, 'hi3!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    LOG(HubFdo, LOG_IDSTR, 'hwID', idVendor, idProduct);

    // make USB\VID_nnnn&PID_nnnn&REV_nnnn\0

    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"USB\\VID_nnnn\0",
                   id,
                   &length,
                   0,
                   4,  // 4 digits
                   idVendor,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'hi2!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"&PID_nnnn\0",
                   id,
                   &length,
                   0,  // no NULL
                   4,  // 4 digits
                   idProduct,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'hi1!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    id = UsbhMakeId(
                   MAKEID_BCD,
                   L"&REV_nnnn\0",
                   id,
                   &length,
                   1,  // add a null
                   4,  // 4 digits
                   bcdRev,
                   NULL);

    // add USB\VID_nnnn&PID_nnnn\0\0

     id = UsbhMakeId(
                   MAKEID_HEX,
                   L"USB\\VID_nnnn\0",
                   id,
                   &length,
                   0,
                   4,  // 4 digits
                   idVendor,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'hi5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"&PID_nnnn\0",
                   id,
                   &length,
                   2,  // double NULL
                   4,  // 4 digits
                   idProduct,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'hi4!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    IdString->Buffer = id;
    IdString->LengthInBytes = length;

    DbgTrace((UsbhDebugTrace, "HardwareID :%ws: %d\n", id, length));

    return STATUS_SUCCESS;
}

static const WCHAR DeviceCompatibleIDs[] =
{
    // L"USB\\DevClass_00&SubClass_00&Prot_00\0"

    'U','S','B','\\','D','e','v','C','l','a','s','s','_','0','0',
    '&','S','u','b','C','l','a','s','s','_','0','0',
    '&','P','r','o','t','_','0','0',
    0,

    // L"USB\\DevClass_00&SubClass_00\0"

    'U','S','B','\\','D','e','v','C','l','a','s','s','_','0','0',
    '&','S','u','b','C','l','a','s','s','_','0','0',
    0,

    // L"USB\\DevClass_00\0"
    'U','S','B','\\','D','e','v','C','l','a','s','s','_','0','0',
    0,

    // L"USB\\COMPOSITE\0"
    'U','S','B','\\','C','O','M','P','O','S','I','T','E',
    0,
    0
};

NTSTATUS
UsbhBuildDeviceCompatibleID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Generates the device compatible id string:

    USB\DevClass_00&SubClass_00&Prot_00\0
    USB\DevClass_00&SubClass_00\0
    USB\DevClass_00\0
    USB\COMPOSITE\0\0

--*/
{
    PWCHAR id;

    UsbhAssert(HubFdo, Pdo);

    UsbhAllocatePool_Z(id, NonPagedPool, sizeof(DeviceCompatibleIDs));

    if (id) {
        RtlCopyMemory(id, &DeviceCompatibleIDs[0], sizeof(DeviceCompatibleIDs));
        IdString->Buffer = id;
        IdString->LengthInBytes = sizeof(DeviceCompatibleIDs);
        return STATUS_SUCCESS;
    }

    return STATUS_INSUFFICIENT_RESOURCES;
}


NTSTATUS
UsbhBuildClassCompatibleID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Generates the device compatible id string:

        USB\MS_COMP_s&MS_SUBCOMP_s } -- optional from MS OS descriptor
        USB\MS_COMP_s              }
    USB\Class_nn&SubClass_nn&Prot_nn
    USB\Class_nn&SubClass_nn
    USB\Class_nn

--*/
{
    USHORT usbClass, usbSubClass, usbProt;
    PWCHAR id;
    ULONG length;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    id = NULL;
    length = 0;

    usbClass = pdoExt->CompatIdInterfaceDescriptor.bInterfaceClass;
    usbSubClass = pdoExt->CompatIdInterfaceDescriptor.bInterfaceSubClass;
    usbProt = pdoExt->CompatIdInterfaceDescriptor.bInterfaceProtocol;

    LOG(HubFdo, LOG_IDSTR, 'dcID', usbClass, usbSubClass);

    if (pdoExt->MsOs20Info.Flags.CompatibleId) {

        //USB\MS_COMP_s&MS_SUBCOMP_s
        //USB\MS_COMP_s

        if ((pdoExt->MsOs20Info.CompatibleIdDescriptor->SubCompatibleId[0] != '\0') && 
            (pdoExt->MsOs20Info.CompatibleIdDescriptor->CompatibleId[0] != '\0')) {
            
            id = UsbhMakeId(MAKEID_MSDESC,
                            L"USB\\MS_COMP_n\0",
                            id,
                            &length,
                            0,  // no NULL
                            1,
                            0,
                            (PUCHAR)pdoExt->MsOs20Info.CompatibleIdDescriptor->CompatibleId);
    
            if (!id) {
                LOG(HubFdo, LOG_BUS, 'ci5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
                return STATUS_INSUFFICIENT_RESOURCES;
            }
    
            id = UsbhMakeId(MAKEID_MSDESC,
                            L"&MS_SUBCOMP_n\0",
                            id,
                            &length,
                            1,  // 1 NULL
                            1,
                            0,
                            (PUCHAR)pdoExt->MsOs20Info.CompatibleIdDescriptor->SubCompatibleId);
    
            if (!id) {
                LOG(HubFdo, LOG_BUS, 'ci5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
                return STATUS_INSUFFICIENT_RESOURCES;
            }
        }

        if (pdoExt->MsOs20Info.CompatibleIdDescriptor->CompatibleId[0] != '\0') {
            id = UsbhMakeId(MAKEID_MSDESC,
                            L"USB\\MS_COMP_n\0",
                            id,
                            &length,
                            1,  // 1 NULL
                            1,
                            0,
                            (PUCHAR)pdoExt->MsOs20Info.CompatibleIdDescriptor->CompatibleId);
    
            if (!id) {
                LOG(HubFdo, LOG_BUS, 'ci5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
                return STATUS_INSUFFICIENT_RESOURCES;
            }
        }

    } else if(pdoExt->MsExtConfigDesc){

        //USB\MS_COMP_s&MS_SUBCOMP_s
        //USB\MS_COMP_s

        if(pdoExt->MsExtConfigDesc->Function[0].SubCompatibleID[0] != '\0'){
            id = UsbhMakeId(MAKEID_MSDESC,
                            L"USB\\MS_COMP_n\0",
                            id,
                            &length,
                            0,  // no NULL
                            1,
                            0,
                            pdoExt->MsExtConfigDesc->Function[0].CompatibleID);
    
            if (!id) {
                LOG(HubFdo, LOG_BUS, 'ci5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
                return STATUS_INSUFFICIENT_RESOURCES;
            }
    
            id = UsbhMakeId(MAKEID_MSDESC,
                            L"&MS_SUBCOMP_n\0",
                            id,
                            &length,
                            1,  // 1 NULL
                            1,
                            0,
                            pdoExt->MsExtConfigDesc->Function[0].SubCompatibleID);
    
            if (!id) {
                LOG(HubFdo, LOG_BUS, 'ci5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
                return STATUS_INSUFFICIENT_RESOURCES;
            }
        }

        if(pdoExt->MsExtConfigDesc->Function[0].CompatibleID[0] != '\0'){
            id = UsbhMakeId(MAKEID_MSDESC,
                            L"USB\\MS_COMP_n\0",
                            id,
                            &length,
                            1,  // 1 NULL
                            1,
                            0,
                            pdoExt->MsExtConfigDesc->Function[0].CompatibleID);
    
            if (!id) {
                LOG(HubFdo, LOG_BUS, 'ci5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
                return STATUS_INSUFFICIENT_RESOURCES;
            }
        }

    }

    //USB\Class_nn&SubClass_nn&Prot_nn\0
    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"USB\\Class_nn\0",
                   id,
                   &length,
                   0,  // no NULL
                   2,  // 2 digits
                   usbClass,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'ci5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"&SubClass_nn\0",
                   id,
                   &length,
                   0,  // no NULL
                   2,  // 4 digits
                   usbSubClass,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'ci4!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"&Prot_nn\0",
                   id,
                   &length,
                   1,  // 1 NULL
                   2,  // 2 digits
                   usbProt,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'ci3!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //USB\Class_nn&SubClass_nn\0
    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"USB\\Class_nn\0",
                   id,
                   &length,
                   0,  // no NULL
                   2,  // 2 digits
                   usbClass,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'ci2!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"&SubClass_nn\0",
                   id,
                   &length,
                   1,  // 1 NULL
                   2,  // 2 digits
                   usbSubClass,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'ci1!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //USB\Class_nn\0\0
    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"USB\\Class_nn\0",
                   id,
                   &length,
                   2,  // double NULL
                   2,  // 2 digits
                   usbClass,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'ci0!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    IdString->Buffer = id;
    IdString->LengthInBytes = length;
    LOG(HubFdo, LOG_IDSTR, 'ccid', Pdo, id);
    DbgTrace((UsbhDebugTrace, "UsbhBuildClassCompatibleID :%ws: %d\n", id, length));

    return STATUS_SUCCESS;
}


static const WCHAR BillboardCompatibleIDs[] =
{
    // L"USB\\DevClass_11&SubClass_00&Prot_00\0"

    'U','S','B','\\','C','l','a','s','s','_','1','1',
    '&','S','u','b','C','l','a','s','s','_','0','0',
    '&','P','r','o','t','_','0','0',
    0,
    0
};


NTSTATUS
UsbhBuildCompatibleID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Generates the device compatible id string:

    If PDO is NULL the we generate the default:
        USB\UNKNOWN

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PWCHAR id = NULL;
    ULONG length = 0;

    LOG(HubFdo, LOG_IDSTR, 'ccID', Pdo, 0);

    if (Pdo == NULL) {
        id = UsbhMakeId(
                   MAKEID_HEX,
                   L"USB\\UNKNOWN\0",
                   id,
                   &length,
                   2,  // double NULL
                   0,  // 0 digits
                   0,
                   NULL);
        if (id) {
            IdString->Buffer = id;
            IdString->LengthInBytes = length;
            return STATUS_SUCCESS;
        } else {
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }

    pdoExt = PdoExt(Pdo);
    
    if (pdoExt->PdoFlags.DeviceIsBillboard) {
        // Billboard device has no interfaces, we we need to
        // create the comapt id using the Ids from the
        // device descriptor
        UsbhAllocatePool_Z(id, NonPagedPool, sizeof(BillboardCompatibleIDs));

        if (id) {
            RtlCopyMemory(id, &BillboardCompatibleIDs[0], sizeof(BillboardCompatibleIDs));
            IdString->Buffer = id;
            IdString->LengthInBytes = sizeof(BillboardCompatibleIDs);
            return STATUS_SUCCESS;
        } else {
            return STATUS_INSUFFICIENT_RESOURCES;
        }
    }     

    if (pdoExt->PdoFlags.DeviceIsComposite) {
        return UsbhBuildDeviceCompatibleID(HubFdo, Pdo, IdString);
    } else {
        return UsbhBuildClassCompatibleID(HubFdo, Pdo, IdString);
    }
}



NTSTATUS
UsbhBuildContainerID(
    __in PDEVICE_OBJECT HubFdo,
    __in PDEVICE_OBJECT Pdo
    )
/*++

    Description:

    Generates a Container ID based on the device's VID, PID, revision, and serial number

--*/
{
    NTSTATUS              status = STATUS_SUCCESS;
    BCRYPT_HASH_HANDLE    hHash = NULL;
    DWORD                 dwcb = 0, 
                          dwcbResult = 0;
    PUCHAR                pbHashObject = NULL,
                          pbHash = NULL;    
    PDEVICE_EXTENSION_PDO pdoExt;
    BCRYPT_ALG_HANDLE     HContainerIDHashAlg = NULL;
    PWCHAR                id, UsbUniqueString = NULL;
    ULONG                 length;
    size_t                UsbUniqueStringLength;

    LOG(HubFdo, LOG_IDSTR, 'BCID', Pdo, 0);

    pdoExt = PdoExt(Pdo);

    id = NULL;
    length = 0;

    //
    // Build a globally unique string from VID, PID, Revision, and serial number
    //

    // start with the device's VID
    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"nnnn",
                   id,
                   &length,
                   0,
                   4,  // 4 digits
                   pdoExt->DeviceDescriptor.idVendor,
                   NULL);
        
    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'BC1!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // add the device's PID
    id = UsbhMakeId(
                   MAKEID_HEX,
                   L"nnnn\0",
                   id,
                   &length,
                   0,  // no NULL
                   4,  // 4 digits
                   pdoExt->DeviceDescriptor.idProduct,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'BC2!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // add the device's revision
    id = UsbhMakeId(
                   MAKEID_BCD,
                   L"nnnn\0",
                   id,
                   &length,
                   1,  // add a null
                   4,  // 4 digits
                   pdoExt->DeviceDescriptor.bcdDevice,
                   NULL);

    if (!id) {
        LOG(HubFdo, LOG_IDSTR, 'BC3!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // allocate the final string
    UsbUniqueStringLength = length + pdoExt->SerialNumberId.LengthInBytes;

    if (UsbUniqueStringLength < sizeof(WCHAR)) {
        LOG(HubFdo, LOG_IDSTR, 'BC5!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto UsbhBuildContainerID_finish;
    }

    UsbhAllocatePool_Z(UsbUniqueString, NonPagedPool, UsbUniqueStringLength);

    if (!UsbUniqueString) {
        LOG(HubFdo, LOG_IDSTR, 'BC4!', Pdo, STATUS_INSUFFICIENT_RESOURCES);
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto UsbhBuildContainerID_finish;
    }

    // copy the VID, PID, revision string 
    status = RtlStringCbCopyW(UsbUniqueString, UsbUniqueStringLength, id);
    if (!NT_SUCCESS(status)) {
        goto UsbhBuildContainerID_finish;
    }
    
    // add the serial number to the string if the buffer is valid
    if(pdoExt->SerialNumberId.Buffer)
    {
        status = RtlStringCbCatW(UsbUniqueString, UsbUniqueStringLength, pdoExt->SerialNumberId.Buffer);
        if (!NT_SUCCESS(status)) {
            goto UsbhBuildContainerID_finish;
        }
    }

    // get the length of the resulting string, without terminating NULL 
    
    status = RtlStringCbLengthW(UsbUniqueString, UsbUniqueStringLength, &UsbUniqueStringLength);
    if (!NT_SUCCESS(status)) {
        goto UsbhBuildContainerID_finish;
    }

    // Open the the SHA1 algorithm provider
    status = BCryptOpenAlgorithmProvider(&HContainerIDHashAlg, 
                                         BCRYPT_SHA1_ALGORITHM, 
                                         MS_PRIMITIVE_PROVIDER, 
                                         BCRYPT_PROV_DISPATCH);

    if (!NT_SUCCESS(status)) {
        HContainerIDHashAlg = NULL;
        goto UsbhBuildContainerID_finish;
    }

    // Create the initial hash object
    status = BCryptGetProperty(HContainerIDHashAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&dwcb, sizeof(dwcb), &dwcbResult, 0);
    if(!NT_SUCCESS(status)){
        goto UsbhBuildContainerID_finish;
    }
    if(sizeof(DWORD) != dwcbResult){
        status = STATUS_INVALID_BUFFER_SIZE;
        goto UsbhBuildContainerID_finish;
    }

    UsbhAllocatePool_Z(pbHashObject , NonPagedPool, dwcb);
    if(NULL == pbHashObject){
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto UsbhBuildContainerID_finish;
    }

    status = BCryptCreateHash(HContainerIDHashAlg, &hHash, pbHashObject, dwcb, NULL, 0, 0);
    if(!NT_SUCCESS(status)){
        goto UsbhBuildContainerID_finish;
    }

    // Hash in the USB Container ID name space
    status = BCryptHashData(hHash, (PUCHAR)&GUID_USB_CONTAINER_ID_NAME_SPACE, sizeof(GUID_USB_CONTAINER_ID_NAME_SPACE), 0);
    if(!NT_SUCCESS(status)){
        goto UsbhBuildContainerID_finish;
    }

    // Hash in the unique USB string
    status = BCryptHashData(hHash, (PUCHAR)UsbUniqueString, (ULONG)UsbUniqueStringLength, 0);
    if(!NT_SUCCESS(status)){
        goto UsbhBuildContainerID_finish;
    }

    // Get the resulting hash
    status = BCryptGetProperty(HContainerIDHashAlg, BCRYPT_HASH_LENGTH, (PUCHAR)&dwcb, sizeof(dwcb), &dwcbResult, 0);
    if(!NT_SUCCESS(status)){
        goto UsbhBuildContainerID_finish;
    }

    if(sizeof(DWORD) != dwcbResult){
        status = STATUS_INVALID_PARAMETER;
        goto UsbhBuildContainerID_finish;
    }

    if(sizeof(GUID) > dwcb){
        status = STATUS_INVALID_PARAMETER;
        goto UsbhBuildContainerID_finish;
    }

    UsbhAllocatePool_Z(pbHash , NonPagedPool, dwcb);
    if(NULL == pbHash){
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto UsbhBuildContainerID_finish;
    }

    status = BCryptFinishHash(hHash, (PUCHAR)pbHash, dwcb, 0);
    if(!NT_SUCCESS(status)){
        goto UsbhBuildContainerID_finish;
    }

    // Let's now copy the resulting hash into the container ID
    // and set the version number and the variant fields.

    RtlCopyMemory(&pdoExt->ContainerID, pbHash, sizeof(GUID));

    pdoExt->ContainerID.Data3 &= 0x0FFF;     // Clear the version number
    pdoExt->ContainerID.Data3 |= (5 << 12);  // Set version = (name-based SHA1) = 5

    pdoExt->ContainerID.Data4[0] &= 0x3F;    // clear the variant bits
    pdoExt->ContainerID.Data4[0] |= 0x80;

    pdoExt->ContainerIDValid = TRUE;

UsbhBuildContainerID_finish:

    if(hHash){
        BCryptDestroyHash(hHash);
    }

    if(pbHash){
        UsbhFreePool(pbHash);
    }

    if(pbHashObject){
        UsbhFreePool(pbHashObject);
    }

    if (UsbUniqueString) {
        UsbhFreePool(UsbUniqueString);
    }

    if (id) {
        UsbhFreePool(id);
    }

    if (HContainerIDHashAlg) {
        BCryptCloseAlgorithmProvider(HContainerIDHashAlg, 0);
    }

    return status;
}


NTSTATUS
UsbhGetSerialNumber(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Get the device specific unique serial number.

    Since a serial number is optional we don't return failure if cannot fetch
    the serial number or if the workaround flag indicates we should ignore it (see below).

    Any error returned here will result in failure of the device to enumerate.

    *** compatibilty note ***
    the 9x/2k hub driver always ignored errors in this case.  In the event
    of an error we revert to no serial number and log an error.

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    USHORT nBytes;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    PUSB_STRING_DESCRIPTOR sd;

    pdoExt = PdoExt(Pdo);

    UsbhAssert(HubFdo, IdString->Buffer == NULL);
    UsbhAssert(HubFdo, IdString->LengthInBytes == 0);

    if (pdoExt->DeviceFlags.DisableSerialNumber) {
        // hacks indicate we need to ignore the serial number on this device
        // even if present
        return STATUS_SUCCESS;
    }

    // see if the device descriptor indicates a serial number
    if (pdoExt->DeviceDescriptor.iSerialNumber == 0) {
        // no SN
        return STATUS_SUCCESS;
    }

    // allocate a buffer to hold it
    UsbhAllocatePool_Z(sd, NonPagedPool, MAXIMUM_USB_STRING_LENGTH);

    // default to success in most cases -- we only return an error if
    // we want to abort enumeration and retry.
    nts = STATUS_SUCCESS;

    do {
        ULONG wCharCount;
        BOOLEAN failEnumeration;

        failEnumeration = FALSE;

        if (!sd) {
            // in this case we should probably give it a retry
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }


        nBytes = MAXIMUM_USB_STRING_LENGTH;

        // Traditionaly we always look for the SN in English.
        // I don't know if there is any reason not to

        nts = UsbhGetStringFromDevice(HubFdo,
                                      Pdo,
                                      &usbdStatus,
                                      (PUCHAR)sd,
                                      &nBytes,
                                      0x409, // good'ol american english
                                      pdoExt->DeviceDescriptor.iSerialNumber);

        if (NT_ERROR(nts)) {
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, UsbErr_GetSNstringDescrptorFailed,
                          nts, usbdStatus, sd, nBytes);

            UsbhFreePool(sd);
            nts = STATUS_SUCCESS;
            break;
        }


        if (!UsbhValidateStringDescriptor(HubFdo,
                                          sd,
                                          nBytes,
                                          &usbdStatus)) {
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, BadUsbString,
                      nts, usbdStatus, sd, nBytes);
            UsbhFreePool(sd);
            // no error here, just ignore the SN
            nts = STATUS_SUCCESS;
            break;
        }

        // we validate this above
        UsbhAssert(HubFdo, sd->bLength>2);

        // count of chars is the descriptor length - the two byte header
        // divided by two. The validation function makes sure this is an even
        // number.
        wCharCount = (sd->bLength/sizeof(WCHAR)) - 1;

        // shift everything left 2 bytes and add a unicode null
        // the first two bytes are length and descriptor type

        if (wCharCount) {

            PWCHAR pwch;
            ULONG i;

            pwch = (PWCHAR) sd;
            for (i=0; i< wCharCount; i++) {
                *pwch = *(pwch+1);
                pwch++;
            }
            // the NULL
            *pwch = 0;

            IdString->LengthInBytes = nBytes;
            IdString->Buffer = (PWCHAR) sd;

            if (UsbhValidateSerialNumberString(HubFdo,
                                               Pdo,
                                               IdString,
                                               &usbdStatus,
                                               &failEnumeration)) {

                // looks good, use it
                nts = STATUS_SUCCESS;
                break;

            } else {
                // if we were unable to get the string or it is invalid,
                // just act as if none exists ie return success


                // throw an exception to note that the string is invalid
                HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber,
                                  BadSerialNumberString,
                                  nts, usbdStatus, sd, nBytes);

                UsbhFreeID(IdString);

                if (failEnumeration == FALSE) {
                    nts = STATUS_SUCCESS;
                } else {
                    pdoExt->EnumFailureMsgId = USBENUM_INVALID_SERIAL_NUMBER;
                    nts = STATUS_INVALID_PARAMETER;
                }
                break;
            }
        }

        // treat a NULL string as invalid

        // if we were unable to get the string or it is invalid,
        // just act as if none exists ie return success

        // throw an exception to note that the string is invalid
        HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, NullSerialNumberString,
                          nts, usbdStatus, sd, nBytes);

        UsbhFreePool(sd);
        
        nts = STATUS_SUCCESS;

    } WHILE (0);

    return nts;

}


BOOLEAN
UsbhLanguageSupported(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    LANGID LanguageId
    )
/*++

    Description:

    returns true if the given languageId exists in the languageid string

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    LANGID *langId;

    pdoExt = PdoExt(Pdo);


    langId = pdoExt->SupportedLanguageIds.Buffer;
    if (langId &&
        pdoExt->SupportedLanguageIds.LengthInBytes) {

        while (*langId) {
            if (*langId == LanguageId) {
                DbgTrace((UsbhDebugTrace, "LanguageId %04.4x IS supported\n",
                        LanguageId));
                return TRUE;
            }
            langId++;
        }
    }

    if (langId == NULL) {
        DbgTrace((UsbhDebugTrace, "no language ID array reported\n"));
    }

    DbgTrace((UsbhDebugTrace, "LanguageId %04.4x NOT supported\n", LanguageId));

    return FALSE;
}


NTSTATUS
UsbhGetProductIdString(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    LANGID LanguageId,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Get the ProductId string from the device in the appropriate laguage.

    This routine will return the cached productId if it exists otherwise it
    will free the old id and allocate a new one.

Returns:

    returns an allocated id string stored in the PDO extension

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    USHORT nBytes;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    PUSB_STRING_DESCRIPTOR sd;

    pdoExt = PdoExt(Pdo);

    UsbhAssert(HubFdo, !pdoExt->DeviceFlags.DisableSerialNumber);

    if (IdString->Buffer) {
        // see if the id we have is the correct laguage
        if (IdString->LanguageId == LanguageId) {
            // yes, return success caller will use this string
            return STATUS_SUCCESS;
        }
    }

    // see if the new language is supported
    if (!UsbhLanguageSupported(HubFdo, Pdo, LanguageId)) {
        return STATUS_NOT_SUPPORTED;
    } else {
        // supported, free the old string if we have one
        UsbhFreeID(IdString);
    }

    // no string or we need to get a different language

    UsbhAssert(HubFdo, IdString->Buffer == NULL);
    UsbhAssert(HubFdo, IdString->LengthInBytes == 0);

    // allocate a buffer to hold it
    UsbhAllocatePool_Z(sd, NonPagedPool, MAXIMUM_USB_STRING_LENGTH);

    if (sd) {
        ULONG wCharCount;

        nBytes = MAXIMUM_USB_STRING_LENGTH;

        // get the string

        nts = UsbhGetStringFromDevice(HubFdo,
                                      Pdo,
                                      &usbdStatus,
                                      (PUCHAR)sd,
                                      &nBytes,
                                      LanguageId,
                                      pdoExt->DeviceDescriptor.iProduct);

        if (NT_ERROR(nts) ||
            !UsbhValidateStringDescriptor(HubFdo,
                                          sd,
                                          nBytes,
                                          &usbdStatus)) {
            // string is either invalid or the device failed the request, in
            // either case this is a hardware error.
            // Log an exception and return STATUS_NOT_SUPPORTED
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, BadUsbString,
                          nts, usbdStatus, sd, nBytes);
            UsbhFreePool(sd);
            return STATUS_NOT_SUPPORTED;

        }

        // we validate this above
        UsbhAssert(HubFdo, sd->bLength>2);

        // count of chars is the descriptor length - the two byte header
        // divided by two. The validation function makes sure this is an even
        // number.
        wCharCount = (sd->bLength/sizeof(WCHAR)) - 1;

        // shift everything left 2 bytes and add a unicode null
        // the first two bytes are length and descriptor type

        if (wCharCount) {

            PWCHAR pwch;
            ULONG i;

            pwch = (PWCHAR) sd;
            for (i=0; i< wCharCount; i++) {
                *pwch = *(pwch+1);
                pwch++;
            }
            // the NULL
            *pwch = 0;

            IdString->LengthInBytes = nBytes;
            IdString->Buffer = (PWCHAR) sd;
            IdString->LanguageId = LanguageId;

            return STATUS_SUCCESS;
        } else {

            // treat a NULL string as not supported
            UsbhFreePool(sd);
            return STATUS_NOT_SUPPORTED;
        }



    }

    // return an error for the out-of-memory case

    return STATUS_INSUFFICIENT_RESOURCES;

}


NTSTATUS
UsbhGetLocationIdString(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    LANGID LanguageId,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Initializes the location string -- this is the string we report for
    DeviceTextLocationInformation

Paraneters

    LanguageId -- not used

Returns:

    returns an allocated id string stored in the PDO extension

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    ULONG length;
    PWCHAR id = NULL;
    USHORT portBcd;
    USHORT hubBcd;

    pdoExt = PdoExt(Pdo);

    UsbhAssert(HubFdo, IdString->Buffer == NULL);
    UsbhAssert(HubFdo, IdString->LengthInBytes == 0);

    hubBcd = Short2Bcd((USHORT) FdoExt(HubFdo)->HubNumber);
    portBcd = Short2Bcd(pdoExt->PortNumber);

    for (;;) {
        length= 0;

        id = UsbhMakeId(MAKEID_BCD,
                           L"Port_#nnnn\0",
                           id,
                           &length,
                           0,  // no NULL
                           4,  // 4 digits
                           portBcd,
                           NULL);

        if (id == NULL) {
           nts = STATUS_INSUFFICIENT_RESOURCES;
           break;
        }

        id = UsbhMakeId(MAKEID_BCD,
                               L".Hub_#nnnn\0",
                               id,
                               &length,
                               1,  // 1 NULL
                               4,  // 4 digits
                               hubBcd,
                               NULL);

        if (id) {
            IdString->LengthInBytes = length;
            IdString->Buffer = id;
            IdString->LanguageId = 0x409;

            nts = STATUS_SUCCESS;
            break;

        } else {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

    }

    return nts;

}


NTSTATUS
UsbhGetLanguageIdString(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
    )
/*++

    Description:

    Fetches the array of supported Language Ids

    see sec 9.6.7 of the USB 2.0 spec for details on this transaction
Returns:

    returns an allocated id string

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    USHORT nBytes;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    PUSB_STRING_DESCRIPTOR sd;

    pdoExt = PdoExt(Pdo);

    UsbhAssert(HubFdo, IdString->Buffer == NULL);
    UsbhAssert(HubFdo, IdString->LengthInBytes == 0);


    if (pdoExt->DeviceDescriptor.iProduct == 0) {
        DbgTrace((UsbhDebugTrace, "UsbhGetLanguageIdString - no product string index\n"));
        return STATUS_NOT_SUPPORTED;
    }

    // allocate a buffer to hold it
    UsbhAllocatePool_Z(sd, NonPagedPool, MAXIMUM_USB_STRING_LENGTH);

    if (sd) {
        ULONG wCharCount;

        nBytes = MAXIMUM_USB_STRING_LENGTH;

        // get the string

        nts = UsbhGetStringFromDevice(HubFdo,
                                      Pdo,
                                      &usbdStatus,
                                      (PUCHAR)sd,
                                      &nBytes,
                                      0,
                                      0); // index 0 is language string

        if (NT_ERROR(nts) ||
            !UsbhValidateStringDescriptor(HubFdo,
                                          sd,
                                          nBytes,
                                          &usbdStatus)) {
            // string is either invalid or the device failed the request, in
            // either case this is a hardware error.
            // Log an exception and return STATUS_NOT_SUPPORTED
            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber,
                          BadUsbString_LangId,
                          nts, usbdStatus, sd, nBytes);
            UsbhFreePool(sd);
            DbgTrace((UsbhDebugTrace, "UsbhGetLanguageIdString - bad langId string\n"));
            return STATUS_NOT_SUPPORTED;

        }

        // we validate this above
        UsbhAssert(HubFdo, sd->bLength>2);

        // count of chars is the descriptor length - the two byte header
        // divided by two. The validation function makes sure this is an even
        // number.
        wCharCount = (sd->bLength/sizeof(WCHAR)) - 1;

        // shift everything left 2 bytes and add a unicode null
        // the first two bytes are length and descriptor type

        if (wCharCount) {

            PWCHAR pwch;
            ULONG i;

            pwch = (PWCHAR) sd;
            for (i=0; i< wCharCount; i++) {
                *pwch = *(pwch+1);
                pwch++;
            }
            // the NULL
            *pwch = 0;

            IdString->LengthInBytes = nBytes;
            IdString->Buffer = (PWCHAR) sd;


            DbgTrace((UsbhDebugTrace, "UsbhGetLanguageIdString - (%d) supported LangIds\n",
                wCharCount));

            pwch = (PWCHAR) sd;
            i = 0;
            while (*pwch) {
                DbgTrace((UsbhDebugTrace, "%d: 0x%x \n", i++, *pwch));
                pwch++;
            }

            return STATUS_SUCCESS;
        } else {

            // treat a NULL string as not supported
            UsbhFreePool(sd);
            return STATUS_NOT_SUPPORTED;
        }

    }

    // return an error for the out-of-memory case

    return STATUS_INSUFFICIENT_RESOURCES;

}

NTSTATUS
UsbhUpdatePdoPnpId(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING Destination,
    PUSB_ID_STRING Source
    )
{
    NTSTATUS nts = STATUS_SUCCESS;

    //
    // Make sure we are not currently processing a PNP irp for this PDO
    //


    return nts;
}


VOID
UsbhBuildUnknownIds(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

    Description:

    Generates the unknown USB device, hardware and compat IDs that include a failure description

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PWCHAR deviceId;
    PWCHAR hardwareId;
    PWCHAR compatId;
    PWCHAR unknownIdString;
    ULONG deviceIdLength;
    ULONG hardwareIdLength;
    ULONG compatIdLength;
    ENUM_FAILURE_PID enumFailurePid;

    LOG(HubFdo, LOG_IDSTR, 'unID', Pdo, 0);

    pdoExt = PdoExt(Pdo);


    switch (pdoExt->EnumFailureMsgId) {
    case USBENUM_PORT_RESET_FAILURE:
        unknownIdString = L"USB\\RESET_FAILURE\0";
        enumFailurePid = EnumFailurePid_ResetFailure;
        break;

    case USBENUM_DEVICE_DESCRIPTOR_FAILURE:
        unknownIdString = L"USB\\DEVICE_DESCRIPTOR_FAILURE\0";
        enumFailurePid = EnumFailurePid_DeviceDescriptorFailure;
        break;

    case USBENUM_CONFIG_DESCRIPTOR_FAILURE:
        unknownIdString = L"USB\\CONFIG_DESCRIPTOR_FAILURE\0";
        enumFailurePid = EnumFailurePid_ConfigDescriptorFailure;
        break;

    case USBENUM_SET_ADDRESS_FAILURE:
        unknownIdString = L"USB\\SET_ADDRESS_FAILURE\0";
        enumFailurePid = EnumFailurePid_SetAddressFailure;
        break;

    case USBENUM_BAD_DEVICE_DESCRIPTOR:
        unknownIdString = L"USB\\DEVICE_DESCRIPTOR_VALIDATION_FAILURE\0";
        enumFailurePid = EnumFailurePid_BadDeviceDesc;
        break;

    case USBENUM_BAD_CONFIG_DESCRIPTOR:
        unknownIdString = L"USB\\CONFIGURATION_DESCRIPTOR_VALIDATION_FAILURE\0";
        enumFailurePid = EnumFailurePid_BadConfigDesc;
        break;

    default:
        return;
    }

    // 
    // Create the device, hardware, and compat IDs together before freeing the default IDs.
    // If creation of any of these IDs fails we will fall back to the default unknown device ID
    //
    deviceIdLength = 0;
    hardwareIdLength = 0;
    compatIdLength = 0;

    hardwareId = UsbhMakeId(MAKEID_HEX,
                            unknownIdString,
                            NULL,
                            &hardwareIdLength,
                            2,  // double NULL
                            0,  // 0 digits
                            0,
                            NULL);

    if (!hardwareId) {
        return;
    }

    compatId = UsbhMakeId(MAKEID_HEX,
                          unknownIdString,
                          NULL,
                          &compatIdLength,
                          2,  // double NULL
                          0,  // 0 digits
                          0,
                          NULL);
      
    if (!compatId) {
        UsbhFreePool(hardwareId);
        return;
    }

    deviceId = UsbhMakeId(MAKEID_HEX,
                          L"USB\\VID_nnnn\0",
                          NULL,
                          &deviceIdLength,
                          0,  // no NULL
                          4,  // 4 digits
                          0x0000,
                          NULL);

    if (!deviceId) {
        UsbhFreePool(hardwareId);
        UsbhFreePool(compatId);
        return;
    }

    deviceId = UsbhMakeId(MAKEID_HEX,
                          L"&PID_nnnn\0",
                          deviceId,
                          &deviceIdLength,
                          2,  // add double null
                          4,  // 4 digits
                          enumFailurePid,
                          NULL);

    if (!deviceId) {
        UsbhFreePool(hardwareId);
        UsbhFreePool(compatId);
        return;
    }

    //
    // Successfully created all IDs.  Now free the defaults IDs and replace them with
    // the new ones.
    //
    UsbhFreeID(&pdoExt->PnpCompatibleId);
    pdoExt->PnpCompatibleId.Buffer = compatId;
    pdoExt->PnpCompatibleId.LengthInBytes = compatIdLength;

    UsbhFreeID(&pdoExt->PnpHardwareId);
    pdoExt->PnpHardwareId.Buffer = hardwareId;
    pdoExt->PnpHardwareId.LengthInBytes = hardwareIdLength;

    UsbhFreeID(&pdoExt->PnpDeviceId);
    pdoExt->PnpDeviceId.Buffer = deviceId;
    pdoExt->PnpDeviceId.LengthInBytes = deviceIdLength;

}