/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    uxd.c

     7-10-2004

Abstract:


    Functions to support redirecting USB device to virtual machines


Author:

    jkace

Environment:

    Kernel mode

Revision History:

UXD:

    This file contains functions that aid in the creation of custom hardware IDs
    for UXD devices.  When a device is considered a UXD device, a custom, registy-based
    PNP hardware ID is created in place of the standard USB PNP hardware ID.  This allows
    a VM driver to be loaded in place of the class driver.
    

REGISTRY KEYS:

   The following registry keys are used to determine the UXD settings:
   
   1) Device keys.  These are global keys that specify a VID/PID
   
        HKLM\System\CCS\Services\UsbHub\uxd_control\devices
        
   2) Port Keys.  These specify a single port on a hub instance
   
        HKLM\System\CCS\Enum\USB\VID_XXXX&PID_YYYY\InstanceID\Device Parameters\uxd
        
 
   3) Global policy settings
   
        HKLM\System\CCS\Services\UsbHub\uxd_control\policy
        
   4) PNP IDs generated for UXD devices
   
        HKLM\System\CCS\Services\UsbHub\uxd_control\pnp

        
DELETING REGISTRY KEYS

    Registry keys for devices and ports may be deleted in the following cases.
    The delete will depend on the global and device settings for the keys
    
    1) Device disable
    2) System Shutdown
    3) Device disconnect
    
    Registry settings indicate whether the keys should be deleted on one or all
    of these events.

        
HARDWARE-ID GENERATION:

    One of 2 different hardware IDs can be created for a device:
    
            a) standard USB hardware ID
            b) custom UXD hardware ID stored in the registry
    
    The driver will check the registry keys and update its hardware IDs in the
    following cases:
    
            1) Device connection
            2) Device re-start after disable
            3) IOCTL_USB_CYCLE_PORT
            
   The currently stored hardwareID is always reported to PnP.
    
    
--*/

#include "hubdef.h"
#include "pch.h"
#include <ntstrsafe.h>

#ifdef HUB_WPP
#include "uxd.tmh"
#endif

#define UXD_DEVICE_KEY_PATH             L"\\registry\\machine\\system\\currentcontrolset\\services\\usbhub\\uxd_control\\devices\\"

#define UXD_GLOBAL_SETTINGS_KEY         L"usbhub\\uxd_control\\policy"
#define UXD_DEVICE_SETTINGS_KEY         L"usbhub\\uxd_control\\devices"
#define UXD_CLASS_SETTINGS_KEY          L"usbhub\\uxd_control\\classes"
#define UXD_GUID_SETTINGS_KEY           L"usbhub\\uxd_control\\pnp"

#define UXD_PORT_SETTINGS_KEY           L"uxd"

#define UXD_GLOBAL_DELETE_SHUTDOWN      L"UxdGlobalDeleteOnShutdown"
#define UXD_GLOBAL_DELETE_RELOAD        L"UxdGlobalDeleteOnReload"
#define UXD_GLOBAL_DELETE_DISCONNECT    L"UxdGlobalDeleteOnDisconnect"
#define UXD_GLOBAL_ENABLE               L"UxdGlobalEnable"

#define UXD_PORT_KEY  L"uxd_port_nnn"
#define UXD_DEVICE_KEY L"VVVVPPPPRRRR"

#define MAX_DEVICE_KEY_LENGTH 12
#define MAX_PORT_KEY_LENGTH 12


typedef struct _HUB_REG_QUERY_DATA {
    __field_bcount(BufferLength) PWCHAR Buffer;
    ULONG  BufferLength;
}HUB_REG_QUERY_DATA, *PHUB_REG_QUERY_DATA;


NTSTATUS
UsbhGetGlobalUxdSettings(
    PDEVICE_OBJECT HubFdo,
    PHUB_GLOBAL_UXD_SETTINGS HubUxdSettings
    )
/*++

Routine Description:

    Queries the UXD_POLICY key for global UXD settings

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    RTL_QUERY_REGISTRY_TABLE UxdQueryTable[5];
    DWORD enableDefault = 0;

    //
    // set the defaults
    // 
    RtlZeroMemory(HubUxdSettings,
                  sizeof(HUB_GLOBAL_UXD_SETTINGS));

    UxdQueryTable[0].QueryRoutine = UsbhQueryGlobalHubValue;
    UxdQueryTable[0].Flags = 0;
    UxdQueryTable[0].Name = UXD_GLOBAL_DELETE_SHUTDOWN;
    UxdQueryTable[0].EntryContext = &HubUxdSettings->DeleteOnShutdown;        
    UxdQueryTable[0].DefaultType = REG_DWORD;                             
    UxdQueryTable[0].DefaultData = &HubUxdSettings->DeleteOnShutdown;      
    UxdQueryTable[0].DefaultLength = sizeof(HubUxdSettings->DeleteOnShutdown);

    UxdQueryTable[1].QueryRoutine = UsbhQueryGlobalHubValue;
    UxdQueryTable[1].Flags = 0;
    UxdQueryTable[1].Name = UXD_GLOBAL_DELETE_RELOAD;
    UxdQueryTable[1].EntryContext = &HubUxdSettings->DeleteOnReload;        
    UxdQueryTable[1].DefaultType = REG_DWORD;                             
    UxdQueryTable[1].DefaultData = &HubUxdSettings->DeleteOnReload;      
    UxdQueryTable[1].DefaultLength = sizeof(HubUxdSettings->DeleteOnReload);

    UxdQueryTable[2].QueryRoutine = UsbhQueryGlobalHubValue;
    UxdQueryTable[2].Flags = 0;
    UxdQueryTable[2].Name = UXD_GLOBAL_DELETE_DISCONNECT;
    UxdQueryTable[2].EntryContext = &HubUxdSettings->DeleteOnDisconnect;        
    UxdQueryTable[2].DefaultType = REG_DWORD;                             
    UxdQueryTable[2].DefaultData = &HubUxdSettings->DeleteOnDisconnect;      
    UxdQueryTable[2].DefaultLength = sizeof(HubUxdSettings->DeleteOnDisconnect);

    UxdQueryTable[3].QueryRoutine = UsbhQueryGlobalHubValue;
    UxdQueryTable[3].Flags = 0;
    UxdQueryTable[3].Name = UXD_GLOBAL_ENABLE;
    UxdQueryTable[3].EntryContext = &HubUxdSettings->Enable;        
    UxdQueryTable[3].DefaultType = REG_DWORD;                             
    UxdQueryTable[3].DefaultData = &enableDefault;      
    UxdQueryTable[3].DefaultLength = sizeof(enableDefault);

    UxdQueryTable[4].QueryRoutine = NULL;
    UxdQueryTable[4].Flags = 0;
    UxdQueryTable[4].Name = NULL;
    UxdQueryTable[4].EntryContext = NULL;        
    UxdQueryTable[4].DefaultType = 0;                             
    UxdQueryTable[4].DefaultData = NULL;      
    UxdQueryTable[4].DefaultLength = 0;

    nts = UsbhRegQueryGlobalKey(RTL_REGISTRY_SERVICES,
                                UXD_GLOBAL_SETTINGS_KEY,
                                sizeof(UXD_GLOBAL_SETTINGS_KEY),
                                UxdQueryTable,
                                sizeof(UxdQueryTable));

    return nts;
}

VOID
UsbhBuildUxdDeviceKey(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    __out_bcount(KeyNameLength) PWCHAR KeyName,
    ULONG KeyNameLength
    )
/*++

Routine Description:

    Creates the string name of the UXD device key

Arguments:


Return Value:

   NTSTATUS

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    USHORT idVendor, idProduct, bcdRev;

    UsbhAssert(HubFdo, KeyNameLength >= sizeof(UXD_DEVICE_KEY));
    
    idVendor = pdoExt->DeviceDescriptor.idVendor;
    idProduct = pdoExt->DeviceDescriptor.idProduct;
    bcdRev = pdoExt->DeviceDescriptor.bcdDevice;

    //
    // Create our device key
    //
    RtlStringCbPrintfW(KeyName, 
                       KeyNameLength, 
                       L"%4.4x%4.4x%4.4x",
                       idVendor,
                       idProduct,
                       bcdRev);

    return;
}

VOID
UsbhBuildUxdPortKey(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    __out_bcount(KeyNameLength) PWCHAR KeyName,
    ULONG KeyNameLength
    )
/*++

Routine Description:

    Creates the string name of the UXD port key
    
Arguments:


Return Value:

   NTSTATUS

--*/
{
    UsbhAssert(HubFdo, KeyNameLength >= sizeof(UXD_PORT_KEY));
    
    //
    // Create our port key
    //
    RtlStringCbPrintfW(KeyName, 
                       KeyNameLength, 
                       L"uxd_port_%3.3d",
                       PortNumber);

    return;
}


NTSTATUS 
UsbhGetUxdLoadKeySettings(
    PDEVICE_OBJECT HubFdo,
    __in HANDLE KeyHandle,
    __in_z PWCHAR KeyName,
    __in PUSB_HUB_DEVICE_UXD_SETTINGS DeviceUxdSettings
    )
/*++

Routine Description:

    Queries the registry for the UXD settings for a particular hub port

Arguments:


Return Value:

   NTSTATUS

--*/
{
    PKEY_VALUE_PARTIAL_INFORMATION partialInfo = NULL;
    UNICODE_STRING valueName;
    ULONG length;
    ULONG resultLength;
    NTSTATUS nts = STATUS_SUCCESS;
    

    RtlInitUnicodeString(&valueName, KeyName);

    //
    // Size and allocate a KEY_VALUE_PARTIAL_INFORMATION structure,
    // including room for the returned value data.
    //

    length = FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data) +
            sizeof(USB_HUB_DEVICE_UXD_SETTINGS);

    UsbhAllocatePool_Z(partialInfo, PagedPool, length);

    if (partialInfo) {
        // Query the value data.
        //
        nts = ZwQueryValueKey(KeyHandle,
                              &valueName,
                              KeyValuePartialInformation,
                              partialInfo,
                              length,
                              &resultLength);


        if (NT_SUCCESS(nts)) {
            //
            // Only copy the smaller of the requested data length or
            // the actual data length.
            //

            RtlCopyMemory(DeviceUxdSettings,
                          partialInfo->Data,
                          sizeof(USB_HUB_DEVICE_UXD_SETTINGS) < partialInfo->DataLength ?
                          sizeof(USB_HUB_DEVICE_UXD_SETTINGS) :
                          partialInfo->DataLength);

        }

        UsbhFreePool(partialInfo);
    }

    else {
        nts = STATUS_INSUFFICIENT_RESOURCES;
    }

    return nts;
}

NTSTATUS
UsbhOpenUxdPortHandle(
    PDEVICE_OBJECT HubFdo,
    HANDLE *KeyHandle
    )
/*++

Routine Description:

    Opens a handle to the UXD port key for a hub instance
    
Arguments:


Return Value:

   NTSTATUS

--*/
{
    HANDLE devKeyHandle = NULL, portHandle = NULL;
    ULONG devInstKeyType = PLUGPLAY_REGKEY_DEVICE;
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    NTSTATUS nts = STATUS_SUCCESS;
    UNICODE_STRING unicodeString;

    UsbhAssertPassive();


    //
    // Open our device key
    //

    nts = IoOpenDeviceRegistryKey(hubExt->PhysicalDeviceObject,
                                  devInstKeyType,
                                  STANDARD_RIGHTS_ALL,
                                  &devKeyHandle);

    if (NT_SUCCESS(nts)) {
        OBJECT_ATTRIBUTES oa;

        //
        // Open the UXD key.  Create it if it does not exist
        //
        RtlInitUnicodeString(&unicodeString, UXD_PORT_SETTINGS_KEY);

        InitializeObjectAttributes(
            &oa,
            &unicodeString,
            OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
            devKeyHandle,
            NULL);

        nts = ZwCreateKey(&portHandle, 
                          KEY_ALL_ACCESS, 
                          &oa,
                          0,
                          0,
                          REG_OPTION_NON_VOLATILE,
                          NULL);

        if(NT_SUCCESS(nts)){
            *KeyHandle = portHandle;
        }

        ZwClose(devKeyHandle);
    }

    return nts;
}

NTSTATUS
UsbhGetUxdPortKey(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PUSB_HUB_DEVICE_UXD_SETTINGS DeviceUxdSettings
    )
/*++

Routine Description:

    Retrieves the UXD port key if it exists.
    
Arguments:


Return Value:

   NTSTATUS
   
   SUCCESS if the key exists, an error if it does not

--*/
{

    NTSTATUS nts = STATUS_UNSUCCESSFUL;
    HANDLE portKeyHandle = NULL;
    WCHAR uxdPortKey[] = UXD_PORT_KEY;

    UsbhAssertPassive();

    UsbhBuildUxdPortKey(HubFdo,
                        PortNumber,
                        uxdPortKey,
                        sizeof(uxdPortKey));

    //
    // Open the parent key
    //
    nts = UsbhOpenUxdPortHandle(HubFdo,
                                &portKeyHandle);

    if(NT_SUCCESS(nts)){
 
        DbgTrace((UsbhDebugTrace, "%!FUNC! Searching for UXD regkey: %ws-%ws\n",
                  UXD_PORT_SETTINGS_KEY,
                  uxdPortKey));

        nts = UsbhGetUxdLoadKeySettings(HubFdo,
                                        portKeyHandle,
                                        uxdPortKey,
                                        DeviceUxdSettings);

        ZwClose(portKeyHandle);
    }

    return nts;
}



NTSTATUS
UsbhQueryUxdDevice(
    PCWSTR ValueName,
    __in ULONG ValueType,
    __inout_bcount(ValueLength) PVOID ValueData,
    __in ULONG ValueLength,
    __inout_opt PVOID Context,
    __inout_opt PVOID EntryContext
    )
/*++

Routine Description:

    This routine is a callback routine for RtlQueryRegistryValues

    This is used for global hub values -- currently we only support dword values.

Arguments:

    ValueName - The name of the value (ignored).
    ValueType - The type of the value
    ValueData - The data for the value.
    ValueLength - The length of ValueData, read from the key.
    Context -
    EntryContext - buffer to save the value

Return Value:

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    PHUB_REG_QUERY_DATA queryData;

    DbgTrace((UsbhDebugTrace, "%!FUNC! \n"));
    DbgTrace((UsbhDebugTrace, "ValueName %ws\n", (PWCHAR) ValueName));
    DbgTrace((UsbhDebugTrace, "ValueType %d\n", ValueType));
    DbgTrace((UsbhDebugTrace, "ValueData %p\n", ValueData));
    DbgTrace((UsbhDebugTrace, "ValueLength %d\n", ValueLength));
    DbgTrace((UsbhDebugTrace, "Context %p\n", Context));
    DbgTrace((UsbhDebugTrace, "EntryContext %p\n", EntryContext));

    if(!EntryContext){
        return STATUS_INVALID_PARAMETER;
    }

    switch (ValueType) {
    case REG_BINARY:
        //
        // USB_HUB_DEVICE_UXD_SETTINGS
        //
        if(ValueLength > sizeof(USB_HUB_DEVICE_UXD_SETTINGS)){
            nts = STATUS_BUFFER_OVERFLOW;
        }
        else{
            RtlCopyMemory(EntryContext, ValueData, ValueLength);
        }
        break;

    case REG_SZ:
        //
        // PNP ID string
        //
        if(ValueLength > 512) {
             nts = STATUS_BUFFER_OVERFLOW;
        }
        else{
            queryData = (PHUB_REG_QUERY_DATA)EntryContext;

            //
            // Allocate our buffer
            //
            UsbhAllocatePool_Z(queryData->Buffer, NonPagedPool, ValueLength);

            if(queryData->Buffer){
                queryData->BufferLength = ValueLength;
                RtlCopyMemory(queryData->Buffer, ValueData, ValueLength);
            }
        }
        break;

    default:
        nts = STATUS_INVALID_PARAMETER;
    }
    return nts;
}



NTSTATUS
UsbhGetUxdDeviceKey(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_HUB_DEVICE_UXD_SETTINGS DeviceUxdSettings
    )
/*++

Routine Description:

    Queries for a UXD device key

Arguments:


Return Value:

   NTSTATUS
   
   SUCCESS if the key exists, failure if it does not

--*/
{

    NTSTATUS nts = STATUS_UNSUCCESSFUL;
    WCHAR uxdDeviceKey[] = UXD_DEVICE_KEY;
    RTL_QUERY_REGISTRY_TABLE queryTable[2];

    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));

    UsbhBuildUxdDeviceKey(HubFdo,
                          Pdo,
                          uxdDeviceKey,
                          sizeof(uxdDeviceKey));

    UsbhAssertPassive();

    queryTable[0].QueryRoutine = UsbhQueryUxdDevice;
    queryTable[0].Flags = RTL_QUERY_REGISTRY_REQUIRED;
    queryTable[0].Name = uxdDeviceKey;
    queryTable[0].EntryContext = DeviceUxdSettings;        
    queryTable[0].DefaultType = 0;                             
    queryTable[0].DefaultData = NULL;      
    queryTable[0].DefaultLength = 0;

    queryTable[1].QueryRoutine = NULL;
    queryTable[1].Flags = 0;
    queryTable[1].Name = NULL;
    queryTable[1].EntryContext = NULL;        
    queryTable[1].DefaultType = 0;                             
    queryTable[1].DefaultData = NULL;      
    queryTable[1].DefaultLength = 0;
        

    DbgTrace((UsbhDebugTrace,"%!FUNC! Searching for UXD regkey: %ws-%ws\n",
                  UXD_DEVICE_SETTINGS_KEY,
                  uxdDeviceKey));

    nts = RtlQueryRegistryValues(RTL_REGISTRY_SERVICES,
                                 UXD_DEVICE_SETTINGS_KEY,
                                 queryTable,
                                 NULL,
                                 NULL);
    return nts;
}


NTSTATUS
UsbhUpdateUxdSettings(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    BOOLEAN UpdateHardwareId,
    BOOLEAN *UpdateRequired
    )
/*++

Routine Description:

    Queries the registry for UXD port and device keys and updates the PDO's 
    stored UXD state

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts  = STATUS_UNSUCCESSFUL;
    USB_HUB_DEVICE_UXD_SETTINGS newUxdSettings;
    USB_HUB_DEVICE_UXD_SETTINGS oldUxdSettings;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();
    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));

    RtlZeroMemory(&newUxdSettings, sizeof(USB_HUB_DEVICE_UXD_SETTINGS));
    RtlZeroMemory(&oldUxdSettings, sizeof(USB_HUB_DEVICE_UXD_SETTINGS));

    //
    // We need to acquire our semaphore to prevent anyone else from using
    // our settings while they are being updated.
    //
   
    UsbhAcquirePdoUxdLock(HubFdo,Pdo);
    
    *UpdateRequired = FALSE;
    pdoExt->PdoFlags.DeviceReservedUxd = 0;
    oldUxdSettings = pdoExt->UxdSettings;


    //
    // First we look for a port or device key.  If present, it will overwrite our
    // current key.  If the keys are not found, our current key
    // is erased.
    //

    do{
        //
        // Query our global settings
        //

        UsbhGetGlobalUxdSettings(HubFdo,
                                 &hubExt->GlobalUxdSettings);

        if(!hubExt->GlobalUxdSettings.Enable ||
           HubG.GlobalDisableUxdSupport){
            //
            // Disabled, do not query for more keys
            //
            DbgTrace((UsbhDebugTrace,"UXD: Policy disabled\n"));
            UsbhReleasePdoUxdLock(HubFdo, Pdo);
            return STATUS_SUCCESS;
        }

        //
        // First we will check for port-specific settings
        //
        nts = UsbhGetUxdPortKey(HubFdo,
                                pdoExt->PortNumber,
                                &newUxdSettings);
    
        if(NT_SUCCESS(nts)){
            //
            // Found port settings
            //
            DbgTrace((UsbhDebugTrace,"UXD: Found PORT settings for port %d\n",
                      pdoExt->PortNumber));
            pdoExt->PdoFlags.DeviceReservedUxd = 1;
            break;
        }
    
        //
        // Now check for device-specific settings
        //
    
        nts = UsbhGetUxdDeviceKey(HubFdo,
                                  Pdo,
                                  &newUxdSettings);
    
        if(NT_SUCCESS(nts)){
            //
            // Found device settings
            //
            DbgTrace((UsbhDebugTrace,"UXD: Found DEVICE settings for %x-%x-%x\n",
                      pdoExt->DeviceDescriptor.idVendor,
                      pdoExt->DeviceDescriptor.idProduct,
                      pdoExt->DeviceDescriptor.bcdDevice));
            pdoExt->PdoFlags.DeviceReservedUxd = 1;
            break;
        }

        //
        // No settings found
        //
        DbgTrace((UsbhDebugTrace,"UXD: No UXD settings found for port %d\n",
                      pdoExt->PortNumber));

    } WHILE (0);

    //
    // Copy our new settings.  If we failed to get a key then we will zero our settings.
    //

    RtlCopyMemory(&pdoExt->UxdSettings,
                  &newUxdSettings,
                  sizeof(USB_HUB_DEVICE_UXD_SETTINGS));

    //
    // See if we need to update our state.  Otherwise our existing PNP ids should
    // suffice.
    //
    if(sizeof(USB_HUB_DEVICE_UXD_SETTINGS) != RtlCompareMemory(&newUxdSettings, 
                                                           &oldUxdSettings, 
                                                           sizeof(USB_HUB_DEVICE_UXD_SETTINGS))){
        //
        // If our PDO is for an external hub, all we need to do is update our settings.
        // we will not report UXD PNP ids for hubs but we must store our settings so 
        // the attached hub can query them.  We will also tell the cycle-port handler
        // not to cycle the device
        //
        if(!pdoExt->PdoFlags.DeviceIsHub){
            *UpdateRequired = TRUE;
        }
    }

    //
    // Do not return an error if we did not find a key
    //
    nts = STATUS_SUCCESS;

    do{
  
        if(*UpdateRequired &&
           UpdateHardwareId){
    
            USB_ID_STRING newId;
    
            nts = UsbhBuildHardwareID(HubFdo, Pdo,
                    &newId);

            if(NT_SUCCESS(nts)){
                UsbhAssert(HubFdo, newId.Buffer);
                //
                // Attempt to replace our hardware ID
                //
                UsbhFreeID(&pdoExt->PnpHardwareId);
                pdoExt->PnpHardwareId = newId;
                DbgTrace((UsbhDebugTrace, "UXD Replacing hardware ID with:%ws: id(%p) length %d\n", 
                          newId.Buffer,
                          newId.Buffer, 
                          newId.LengthInBytes));
            }
            else{
                // Failure, return error
                DbgTrace((UsbhDebugTrace, "UXD UsbhBuildHardwareID failed, cannot replace hardware ID\n"));
                break;
            }

            nts = UsbhBuildDeviceID(HubFdo, Pdo,
                    &newId);

            if(NT_SUCCESS(nts)){
                UsbhAssert(HubFdo, newId.Buffer);
                //
                // Attempt to replace our device ID
                //
                UsbhFreeID(&pdoExt->PnpDeviceId);
                pdoExt->PnpDeviceId = newId;
                DbgTrace((UsbhDebugTrace, "UXD Replacing hardware ID with:%ws: id(%p) length %d\n", 
                          newId.Buffer,
                          newId.Buffer, 
                          newId.LengthInBytes));
            }
            else{
                // Failure, return error
                DbgTrace((UsbhDebugTrace, "UXD UsbhBuildDeviceID failed, cannot replace Device ID\n"));
                break;
            }
        }
    } WHILE (0);

    UsbhReleasePdoUxdLock(HubFdo,Pdo);

    if ((pdoExt->SqmProperties.Virtualized == 0) && (pdoExt->PdoFlags.DeviceReservedUxd == 1)) {
        pdoExt->SqmProperties.Virtualized = 1;
        UsbhUpdateSqmFlags(Pdo);
    }

    return nts;
}

NTSTATUS
UsbhDeleteUxdPortSettings(
    PDEVICE_OBJECT HubFdo,
    HANDLE PortKeyHandle,
    ULONG PortNumber
    )
/*++

Routine Description:

    Deletes the UXD port key

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    WCHAR uxdPortKey[] = UXD_PORT_KEY;

    UsbhBuildUxdPortKey(HubFdo,
                        PortNumber,
                        uxdPortKey,
                        sizeof(uxdPortKey));

    DbgTrace((UsbhDebugTrace,"UXD: Deleting PORT key %ws\n",
                      uxdPortKey));

    nts = RtlDeleteRegistryValue(RTL_REGISTRY_HANDLE,
                                 PortKeyHandle,
                                 uxdPortKey);

    return nts;
}

NTSTATUS
UsbhDeleteUxdPortKey(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber
    )
/*++

Routine Description:

    Deletes the UXD port key for a particular port

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    HANDLE portKeyHandle = NULL;

    //
    // Open our handle
    //

    nts = UsbhOpenUxdPortHandle(HubFdo,
                                &portKeyHandle);

    if(!NT_SUCCESS(nts)){
        return nts;
    }

    //
    // Delete our port key if present
    //

    nts = UsbhDeleteUxdPortSettings(HubFdo,
                                    portKeyHandle,
                                    PortNumber);

    ZwClose(portKeyHandle);

    return nts;
}

NTSTATUS
UsbhDeleteUxdDeviceKey(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Deletes the UXD device key for the PDO's device
    
Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    WCHAR deviceKey[] = UXD_DEVICE_KEY;

    UsbhBuildUxdDeviceKey(HubFdo,
                          Pdo,
                          deviceKey,
                          sizeof(deviceKey));

    DbgTrace((UsbhDebugTrace,"UXD: Deleting DEVICE key %ws\n",
                      deviceKey));

    RtlDeleteRegistryValue(RTL_REGISTRY_SERVICES,
                           UXD_DEVICE_SETTINGS_KEY,
                           deviceKey);

    return nts;
}

NTSTATUS
UsbhDeleteUxdSettings(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    ULONG UxdEvent
    )
/*++

Routine Description:

    Deletes a particular device key if it exists.  The pdo's UXD properties
    determine whether it is appropriate to delete the key

Arguments:


Return Value:

   NTSTATUS

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    BOOLEAN deleteKey = FALSE;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    UsbhAssertPassive();

    //
    // Refresh our global settings.
    //
    
    UsbhGetGlobalUxdSettings(HubFdo,
                             &hubExt->GlobalUxdSettings);

    switch(UxdEvent){
    case UXD_EVENT_DISABLE:
        if(pdoExt->UxdSettings.DeleteOnReload ||
           hubExt->GlobalUxdSettings.DeleteOnReload){
            deleteKey = TRUE;
        }
        break;
    case UXD_EVENT_DISCONNECT:
        if(pdoExt->UxdSettings.DeleteOnDisconnect ||
           hubExt->GlobalUxdSettings.DeleteOnDisconnect){
            deleteKey = TRUE;
        }
        break;
    default:
        break;
    }

    if(deleteKey){
     
        //
        // delete our port key
        //
        UsbhDeleteUxdPortKey(HubFdo,
                                   pdoExt->PortNumber);
    
        //
        // delete our device key
        //
        UsbhDeleteUxdDeviceKey(HubFdo,
                                     Pdo);
    }

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhGetPnpKey(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    GUID IdGuid,
    __deref_out_bcount(*BufferLength) PWCHAR *Buffer,
    __out PULONG BufferLength
    )
/*++

Routine Description:

    Retrieves the PNP ID for a PDO based on a GUID

Arguments:


Return Value:

   NTSTATUS

--*/
{

    NTSTATUS nts = STATUS_SUCCESS;
    RTL_QUERY_REGISTRY_TABLE queryTable[2];
    UNICODE_STRING guidString;
    HUB_REG_QUERY_DATA queryData;

    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));
    UsbhAssertPassive();

    nts = RtlStringFromGUID((REFGUID)&IdGuid,
                            &guidString);

    if(!NT_SUCCESS(nts)){
        return nts;
    }

    queryData.Buffer = NULL;
    queryData.BufferLength = 0;
   
    queryTable[0].QueryRoutine = UsbhQueryUxdDevice;
    queryTable[0].Flags = RTL_QUERY_REGISTRY_REQUIRED;
    queryTable[0].Name = guidString.Buffer;
    queryTable[0].EntryContext = &queryData;        
    queryTable[0].DefaultType = 0;                             
    queryTable[0].DefaultData = NULL;      
    queryTable[0].DefaultLength = 0;

    queryTable[1].QueryRoutine = NULL;
    queryTable[1].Flags = 0;
    queryTable[1].Name = NULL;
    queryTable[1].EntryContext = NULL;        
    queryTable[1].DefaultType = 0;                             
    queryTable[1].DefaultData = NULL;      
    queryTable[1].DefaultLength = 0;
        

    nts = RtlQueryRegistryValues(RTL_REGISTRY_SERVICES,
                                 UXD_GUID_SETTINGS_KEY,
                                 queryTable,
                                 NULL,
                                 NULL);

    if(NT_SUCCESS(nts)){
        //
        // We found the reg key
        //
        if(!queryData.Buffer || queryData.BufferLength == 0){
            return STATUS_INVALID_PARAMETER;
        }

       *Buffer = queryData.Buffer;
       *BufferLength = queryData.BufferLength;
    }

    return nts;
}


NTSTATUS
UsbhBuildUxdPnpId(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_ID_STRING IdString
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
    NTSTATUS nts = STATUS_UNSUCCESSFUL;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PWCHAR idBuffer = NULL;
    PWCHAR multiSzBuffer = NULL;
    ULONG idBufferLength = 0;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    
    UsbhAssert(HubFdo, pdoExt->PdoFlags.DeviceReservedUxd == 1);

    IdString->Buffer = NULL;
    IdString->LengthInBytes = 0;

    nts = UsbhGetPnpKey(HubFdo,
                        Pdo,
                        pdoExt->UxdSettings.PnpGuid,
                        &idBuffer,
                        &idBufferLength);

    if(NT_SUCCESS(nts) && idBufferLength != 0){
        UsbhAssert(HubFdo, idBuffer);

        //
        // Convert to a multi-sting by adding a terminating NULL
        //

        UsbhAllocatePool_Z(multiSzBuffer, 
                           NonPagedPool, 
                           idBufferLength+sizeof(UNICODE_NULL));
        if(multiSzBuffer){
            RtlCopyMemory(multiSzBuffer, 
                          idBuffer, 
                          idBufferLength);
            IdString->Buffer = multiSzBuffer;
            IdString->LengthInBytes = idBufferLength+sizeof(UNICODE_NULL);
        }
        else{
            nts = STATUS_INSUFFICIENT_RESOURCES;
        }
        UsbhFreePool(idBuffer);
    }
  
    return nts;
}

NTSTATUS 
UsbhWriteUxdPortKey(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_DEVICE_UXD_SETTINGS ParentUxdSettings,
    HANDLE Handle,
    __in_z PWCHAR PortKeyName
    )
/*++

Routine Description:

    Writes a UXD port key to the registry

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;

    nts = RtlWriteRegistryValue(
                RTL_REGISTRY_HANDLE,
                Handle,
                PortKeyName,
                REG_BINARY,
                ParentUxdSettings,
                sizeof(USB_HUB_DEVICE_UXD_SETTINGS));

    return nts;
}


NTSTATUS
UsbhPropagateUxdState(
    PDEVICE_OBJECT HubFdo,
    PUSB_HUB_DEVICE_UXD_SETTINGS ParentUxdSettings
    )
/*++

Routine Description:

    Propagates a UXD port key to all downstream ports of a hub

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    ULONG i;
    WCHAR uxdPortKey[] = UXD_PORT_KEY;
    HANDLE handle = NULL;

    nts = UsbhOpenUxdPortHandle(HubFdo,
                                &handle);

    if(!NT_SUCCESS(nts)){
        return nts;
    }

    for(i=1; i<= N_PORTS(HubFdo); i++){

        UsbhBuildUxdPortKey(HubFdo,
                            i,
                            uxdPortKey,
                            sizeof(uxdPortKey));

        nts = UsbhWriteUxdPortKey(HubFdo,
                                  ParentUxdSettings,
                                  handle,
                                  uxdPortKey);
        
    }

    ZwClose(handle);

    return nts;
}


VOID
UsbhFreeDeviceConfigInfo(
    __in PHUB_INTERNAL_CONFIG_INFO HubConfigInfo
    )
{

    UsbhFreeID(&HubConfigInfo->CompatibleIds);
    UsbhFreeID(&HubConfigInfo->HardwareIds);
    UsbhFreeID(&HubConfigInfo->DeviceDescription);

    ExFreePool(HubConfigInfo);
    return;
}


NTSTATUS
UsbhQueryParentHubConfig(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Queries the parent hub device for its UXD config info

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    PHUB_INTERNAL_CONFIG_INFO configInfo = NULL;
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    KEVENT event;
    PIO_STACK_LOCATION ioStack = NULL;
    PIRP irp = NULL;
    IO_STATUS_BLOCK ioStatus;

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    UsbhAllocatePool_Z(configInfo, NonPagedPool, sizeof(HUB_INTERNAL_CONFIG_INFO));

    if(!configInfo){
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    configInfo->Version = 1;
    configInfo->Length = sizeof(HUB_INTERNAL_CONFIG_INFO);

    irp = IoBuildDeviceIoControlRequest(IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO,
                                        hubExt->TopOfStackDeviceObject,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        TRUE,
                                        &event,
                                        &ioStatus);
    if(!irp){
        UsbhFreePool(configInfo);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ioStack = IoGetNextIrpStackLocation(irp);
    ioStack->Parameters.Others.Argument1 = configInfo;

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject,
                       irp);

    if(nts == STATUS_PENDING){
        KeWaitForSingleObject(&event, 
                              Executive, 
                              KernelMode, 
                              FALSE, 
                              NULL);
        nts = STATUS_SUCCESS;
    }

    if(NT_SUCCESS(nts) && NT_SUCCESS(ioStatus.Status)){
        UsbhAssert(HubFdo, configInfo->Version == 1);
        if(configInfo->Flags.UxdEnabled){
            //
            // We must propogate our parent's UXD settings to our child ports
            //
            UsbhPropagateUxdState(HubFdo,
                                  &configInfo->UxdSettings);
        }
    }

    UsbhFreeDeviceConfigInfo(configInfo);
    return nts;
}

NTSTATUS
UsbhCopyPnpString(
    __in PUSB_ID_STRING Output,
    __out PUSB_ID_STRING Input
    )
/*++

Routine Description:

    Allocates memory for and copies a USB_ID_STRING

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;

    RtlZeroMemory(Output, sizeof(USB_ID_STRING));

    if(Input->Buffer &&
       Input->LengthInBytes){

        RtlCopyMemory(Output, Input, sizeof(USB_ID_STRING));

        UsbhAllocatePool_Z(Output->Buffer,
                           NonPagedPool,
                           Input->LengthInBytes);

        if(!Output->Buffer){
            return STATUS_INSUFFICIENT_RESOURCES;
        }
        
        RtlCopyMemory(Output->Buffer,
                      Input->Buffer,
                      Input->LengthInBytes);

    }

    return nts;
}

NTSTATUS
UsbhAcquirePdoUxdLock(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Acquires the UXD semaphore

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);

    KeWaitForSingleObject(&pdoExt->UxdLock,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);
    return nts;
}

NTSTATUS
UsbhReleasePdoUxdLock(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Releases the UXD semaphore

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);

    KeReleaseSemaphore(&pdoExt->UxdLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);

    return nts;
}

NTSTATUS
UsbhOpenRegistryKey(
    HANDLE *ReturnHandle,
    HANDLE KeyHandle,
    __in PWSTR  KeyName
    )
/*++

Routine Description:

    Opens a registry key and returns a handle

Arguments:


Return Value:

   NTSTATUS

--*/
{
    UNICODE_STRING unicodeString;
    OBJECT_ATTRIBUTES oa;

    RtlInitUnicodeString(&unicodeString, KeyName);

    InitializeObjectAttributes(&oa,
                               &unicodeString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               KeyHandle,
                               NULL);

    return ZwOpenKey(ReturnHandle,
                     KEY_ALL_ACCESS,
                     &oa);
}

NTSTATUS
UsbhDeleteUxdSubKeys(
    PDEVICE_OBJECT HubFdo,
    HANDLE KeyHandle
    )
/*++

Routine Description:

    Given a handle to a key, deletes all UXD subkeys that have a shutdown policy

Arguments:


Return Value:

   NTSTATUS

--*/
{
    PKEY_VALUE_BASIC_INFORMATION  basicInformation;
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    USB_HUB_DEVICE_UXD_SETTINGS deviceSettings;
    ULONG size, index;
    ULONG resultLength;
    NTSTATUS nts = STATUS_SUCCESS;

    RtlZeroMemory(&deviceSettings,sizeof(USB_HUB_DEVICE_UXD_SETTINGS));
   
    size = sizeof(KEY_VALUE_BASIC_INFORMATION) + 
                   ((MAX_DEVICE_KEY_LENGTH+1) * sizeof(WCHAR));

    UsbhAllocatePool_Z(basicInformation, 
                       NonPagedPool,
                       size);

    if(!basicInformation){
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    index = 0;
   
    do{
        nts = ZwEnumerateValueKey(KeyHandle,
                                  index,
                                  KeyValueBasicInformation,
                                  basicInformation,
                                  size,
                                  &resultLength);

        if(!NT_SUCCESS(nts)){
            break;
        }

        index++;

        if(basicInformation->Type != REG_BINARY){
            // not a key we are looking for
            continue;
        }

        //
        // Read the key
        //
        nts = UsbhGetUxdLoadKeySettings(HubFdo,
                                        KeyHandle,
                                        basicInformation->Name,
                                        &deviceSettings);

        if(!NT_SUCCESS(nts)){
            continue;
        }

        if(deviceSettings.DeleteOnShutdown ||
           hubExt->GlobalUxdSettings.DeleteOnShutdown){

            //
            // Delete the key
            //

            DbgTrace((UsbhDebugTrace,"UXD: Shutdown-Deleting subkey key %ws\n",
                          basicInformation->Name));

            if(NT_SUCCESS(RtlDeleteRegistryValue(RTL_REGISTRY_HANDLE,
                                                 KeyHandle,
                                                 basicInformation->Name))){

                //
                // After we delete the key we need to decement our index since there
                // is now once less value in the registry.
                //
                index--;
            }
            
        }
    } WHILE (1);

    UsbhFreePool(basicInformation);

    return STATUS_SUCCESS;
}

NTSTATUS
UsbhUxdShutdown(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Dispatch routine for IRP_MJ_SHUTDOWN.  Deletes all necessary UXD keys.

Arguments:


Return Value:

   NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    HANDLE keyHandle;

    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));

    UsbhAssertPassive();


    //
    // refresh our global settings
    //
    UsbhGetGlobalUxdSettings(HubFdo,
                             &hubExt->GlobalUxdSettings);

    //
    // Open our devices keys
    //

    nts = UsbhOpenRegistryKey(&keyHandle,
                              NULL,
                              UXD_DEVICE_KEY_PATH);


    if(NT_SUCCESS(nts)){

        UsbhDeleteUxdSubKeys(HubFdo,
                             keyHandle);

        ZwClose(keyHandle);
    }

    //
    // Open our port keys
    //

    keyHandle = NULL;
    nts = UsbhOpenUxdPortHandle(HubFdo,
                                &keyHandle);

    if(NT_SUCCESS(nts)){
        UsbhDeleteUxdSubKeys(HubFdo,
                             keyHandle);
        ZwClose(keyHandle);
    }

    return STATUS_SUCCESS;
}


