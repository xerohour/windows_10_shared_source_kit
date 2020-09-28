/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    msos.c

     1-24-2004

Abstract:


    Functions to support the Microsoft Operating System (MSOS) descriptor. The MSOS
    descriptor is an attempt to extend the USB specification to address specific needs
    or features of the opearting system.
    At the time this code was added the feature was completely unused but it was thought
    that this may change in the near future.




Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "msos.tmh"
#endif

// include some code shared with usbccgp for os decsriptors.

#include "internal.c"
#include "msosdesc.c"

CCHAR usbfile_msos_c[] = "msos.c";

#define USBFILE usbfile_msos_c


NTSTATUS
UsbhGetMSOS_Descriptor(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Description:

    Get the MSOS descriptor from a device if the device has one.  This function will first
    check the Pdo extension flags (set by reading the registry) first.

    Note that if the request wedges the device somehow it will fail susbsequent requests and
    force an automatic retry.  When enumeration is retried we will have already set the flag
    to indicate that this feature is not supported.

Arguments:

Return Value:

    STATUS_NOT_SUPPORTED if the device does not support the MSOS descriptor.


--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    USHORT nBytes;
    NTSTATUS nts, reg_nts;
    POS_STRING osString = NULL;
    USBD_STATUS usbdStatus;
    UCHAR osvcContext[2];
    extern PDRIVER_OBJECT UsbhDriverObject;

    pdoExt = PdoExt(Pdo);

    // check for global override
    if (HubG.GlobalMsOsDescriptorMode == HUBG_MSOS_DISABLE) {
        return STATUS_NOT_SUPPORTED;
    }

    // creaye the key we will need
    reg_nts = UsbhRegCreateUsbflagsKey(UsbhDriverObject);
    if (NT_ERROR(reg_nts)) {
        // if we cannot create this key then disble the MSOS descriptor support
        HubG.GlobalMsOsDescriptorMode = HUBG_MSOS_DISABLE;
        return STATUS_NOT_SUPPORTED;
    }    

    // flags should already be set up
    UsbhAssert(HubFdo, pdoExt->DeviceFlags.DevFlagsInitialized);

    do {
        UsbhAllocatePool_Z(osString, NonPagedPool, sizeof(struct _OS_STRING));

        if (osString == NULL) {
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        // reset query flag if e are force MSOS
        if (HubG.GlobalMsOsDescriptorMode == HUBG_MSOS_FORCE_ENUM) {
            // print at level zero -- this key should only be set in debug circumstances so
            // we want complaints about it.
            UsbhKdPrint((0, "PDO Port[%d] %p - Forcing MSOS descriptor\n", pdoExt->PortNumber, Pdo));

            pdoExt->DeviceFlags.QueryMsOs = 1;
            pdoExt->DeviceFlags.MsOsSupported = 0;
        }

        if (pdoExt->DeviceFlags.QueryMsOs == 0) {
            // flags indicate do not query
            nts = STATUS_NOT_SUPPORTED;
            break;
        }

        if (pdoExt->DeviceFlags.MsOsSupported) {
            // we read supported vc fron regsitry
            nts = STATUS_SUCCESS;
            break;
        }

        // have not queried this device before -- do so now.

        nBytes = sizeof(struct _OS_STRING);
        nts = UsbhGetStringFromDevice(HubFdo,
                                        Pdo,
                                        &usbdStatus,
                                        (PUCHAR)osString,
                                        &nBytes,
                                        0x00, // the XP driver used lang ID 0, our spec requries it
                                        OS_STRING_DESCRIPTOR_INDEX);
        if(usbdStatus==USBD_STATUS_TIMEOUT)
        {
              ULONG MSOSHang=1;
              RtlWriteRegistryValue(RTL_REGISTRY_CONTROL,
                                  pdoExt->DeviceUsbRegPath,
                                  L"MSOSDescriptorHang",
                                  REG_DWORD,
                                  &MSOSHang,
                                  sizeof(MSOSHang));

        }

        // only try this once
        pdoExt->DeviceFlags.QueryMsOs = 0;

        if (NT_SUCCESS(nts) &&
            (nBytes == sizeof(OS_STRING)) &&
            (RtlCompareMemory(&osString->MicrosoftString,
                              MS_OS_STRING_SIGNATURE,
                              sizeof(osString->MicrosoftString)) ==
             sizeof(osString->MicrosoftString))) {
            // This device has a valid MS OS String Descriptor.
            // Let's pluck out the corresponding Vendor Code and
            // save that away in the device extension.
            //
            pdoExt->DeviceFlags.MsOsDescriptorVc = osString->bVendorCode;
            osvcContext[1] = (UCHAR) pdoExt->DeviceFlags.MsOsDescriptorVc;
            osvcContext[0] = 1;
            pdoExt->DeviceFlags.MsOsSupported = 1;
        } else {
            // returning an error here other than STATUS_NOT_SUPPORTED will force a faulre
            // in SetupDevice and trigger a reset_retry.
            // nts = STATUS_DEVICE_DATA_ERROR;
            nts = STATUS_NOT_SUPPORTED;

            pdoExt->DeviceFlags.MsOsSupported = 0;

            osvcContext[0] = 0;
            osvcContext[1] = 0;

            // failure to return an MSOS descriptor is not an error but we do track
            // failure of the query for our own records with an exception.

            if (NT_SUCCESS(nts)) {
                // something was returned but it did not match
                HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, PnpErr_InvalidMsOsDescriptor,
                          nts, usbdStatus, osString, nBytes);
            } else {
                // the command failed
                HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, PnpErr_MsOsDescriptor_NotSupported,
                          nts, usbdStatus, osString, nBytes);
            }
        }

        // Write the MsOsVendorCode value to the registry.  It indicates whether
        // or not the device supports the MS OS Descriptor request, and if
        // does, what the Vendor Code is.
        //

        if (pdoExt->DeviceUsbRegPath) {
            ULONG SkipContainerId;

            RtlWriteRegistryValue(RTL_REGISTRY_CONTROL,
                                  pdoExt->DeviceUsbRegPath,
                                  L"osvc",
                                  REG_BINARY,
                                  &osvcContext[0],
                                  sizeof(osvcContext));
        
            // Check if the device supports the Container ID descriptor
            if (osString->bFlags & MS_OS_FLAGS_CONTAINERID) {
                pdoExt->DeviceFlags.SkipContainerIdQuery = 0;
                SkipContainerId = 0;
            } else {
                pdoExt->DeviceFlags.SkipContainerIdQuery = 1;
                SkipContainerId = 1;
            }

           RtlWriteRegistryValue(RTL_REGISTRY_CONTROL,
                                  pdoExt->DeviceUsbRegPath,
                                  L"SkipContainerIdQuery",
                                  REG_BINARY,
                                  &SkipContainerId,
                                  sizeof(SkipContainerId));

        }

    } WHILE (0);

    UsbhFreePool(osString);

    return nts;

}

NTSTATUS
UsbhGetMsOsFeatureDescriptor(
    PDEVICE_OBJECT  Pdo,
    UCHAR           Recipient,
    UCHAR           InterfaceNumber,
    USHORT          Index,
    PVOID           DataBuffer,
    ULONG           DataBufferLength,
    PULONG          BytesReturned
    )
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USBD_STATUS usbdStatus;
    USHORT dataReturned;


    if (DataBuffer == NULL ||
        DataBufferLength == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    if(BytesReturned){
        *BytesReturned = 0;
    }

    // setup packet for Get_Descriptor, string
    setupPacket.bmRequestType.Recipient = Recipient;
    setupPacket.bmRequestType.Type = BMREQUEST_VENDOR;
    setupPacket.bmRequestType.Reserved = 0;
    setupPacket.bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;
    setupPacket.bRequest = (UCHAR)pdoExt->DeviceFlags.MsOsDescriptorVc;
    setupPacket.wValue.HiByte = InterfaceNumber;
    setupPacket.wValue.LowByte = 0;  // page index
    setupPacket.wIndex.W = Index;
    setupPacket.wLength = (USHORT)DataBufferLength;
    dataReturned = (USHORT)DataBufferLength;

    nts = UsbhSyncSendCommandToDevice(pdoExt->HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      DataBuffer,
                                      &dataReturned,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      &usbdStatus);

    if(NT_SUCCESS(nts)){
        if(BytesReturned){
            *BytesReturned = dataReturned;
        }
    }

    return nts;
}

BOOLEAN
UsbhExtPropDescSemaphorePresent(
    PDEVICE_OBJECT Pdo
    )
{

    NTSTATUS nts;
    WCHAR DidExtPropDescKey[] = L"ExtPropDescSemaphore";
    DWORD regData = 0;

    /*
    * Check if the semaphore value is already set in the registry.  We only
    * care whether or not it already exists, not what data it has.
    */


    nts = UsbhGetPdoRegistryParameter(Pdo,
                                      DidExtPropDescKey,
                                      &regData,
                                      sizeof(regData),
                                      NULL,
                                      NULL,
                                      TRUE);
    //
    // We only care that the key exists, not what the data is
    //

    if (NT_SUCCESS(nts))
    {
        // Already did this once for this device instance.  Don't do it again.
        //
        return TRUE;
    }

    // Set the semaphore key in the registry so that we only run the following
    // code once per device.

    regData = 1;


    nts = UsbhSetPdoRegistryParameter(Pdo,
                                      DidExtPropDescKey,
                                      REG_DWORD,
                                      &regData,
                                      sizeof(regData),
                                      TRUE);
    if(!NT_SUCCESS(nts)){
        //
        // Error occured, do not poll for ext properties
        //
        return TRUE;
    }

    return FALSE;
}

PMS_EXT_CONFIG_DESC
UsbhGetMsOsExtendedConfigDesc(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )

{
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PMS_EXT_CONFIG_DESC extConfigDesc = NULL;
    MS_EXT_CONFIG_DESC_HEADER  extConfigDescHeader;
    ULONG bytesReturned;
    NTSTATUS nts = STATUS_SUCCESS;

    if (pdoExt->DeviceFlags.MsOsSupported == 0) {
        // MSOS not supported
        nts = STATUS_NOT_SUPPORTED;
        goto UsbhGetMsOsExtendedConfigDesc_Done;
    }

    // Request the descriptor header

    RtlZeroMemory(&extConfigDescHeader,
                  sizeof(MS_EXT_CONFIG_DESC_HEADER));

    nts = UsbhGetMsOsFeatureDescriptor(Pdo,
                                       0, // recipient device
                                       0, // interface
                                       MS_EXT_CONFIG_DESCRIPTOR_INDEX,
                                       &extConfigDescHeader,
                                       sizeof(MS_EXT_CONFIG_DESC_HEADER),
                                       &bytesReturned);

    if(!NT_SUCCESS(nts)){
        DbgTrace((UsbhDebugTrace, "UsbhGetMsOsExtendedConfigDesc: Get Header Failed! Status = %x\n",
                  nts));
        goto UsbhGetMsOsExtendedConfigDesc_Done;
    }

    if(bytesReturned != sizeof(MS_EXT_CONFIG_DESC_HEADER) ||
        extConfigDescHeader.bcdVersion != MS_EXT_CONFIG_DESC_VER ||
        extConfigDescHeader.wIndex != MS_EXT_CONFIG_DESCRIPTOR_INDEX ||
        extConfigDescHeader.bCount == 0 ||
        extConfigDescHeader.dwLength != sizeof(MS_EXT_CONFIG_DESC_HEADER) +
        extConfigDescHeader.bCount * sizeof(MS_EXT_CONFIG_DESC_FUNCTION)){

        nts = STATUS_DEVICE_DATA_ERROR;
        DbgTrace((UsbhDebugTrace, "UsbhGetMsOsExtendedConfigDesc: Header Invalid!\n"));
        goto UsbhGetMsOsExtendedConfigDesc_Done;
    }

    // Allocate a buffer large enough for the entire descriptor

    UsbhAllocatePool_Z(extConfigDesc,
                       NonPagedPool,
                       extConfigDescHeader.dwLength);

    if(!extConfigDesc){
        nts = STATUS_INSUFFICIENT_RESOURCES;
        goto UsbhGetMsOsExtendedConfigDesc_Done;
    }

    nts = UsbhGetMsOsFeatureDescriptor(Pdo,
                                       0, // recipient
                                       0, // interface
                                       MS_EXT_CONFIG_DESCRIPTOR_INDEX,
                                       extConfigDesc,
                                       extConfigDescHeader.dwLength,
                                       &bytesReturned);

    if(!NT_SUCCESS(nts)){
        DbgTrace((UsbhDebugTrace, "UsbhGetMsOsExtendedConfigDesc: Get Descriptor Failed! Status = %x\n",
                  nts));
        goto UsbhGetMsOsExtendedConfigDesc_Done;
    }

    if(bytesReturned != extConfigDescHeader.dwLength ||
       RtlCompareMemory(&extConfigDescHeader,
                         extConfigDesc,
                         sizeof(MS_EXT_CONFIG_DESC_HEADER)) !=
                    sizeof(MS_EXT_CONFIG_DESC_HEADER) ) {
        nts = STATUS_DEVICE_DATA_ERROR;
        goto UsbhGetMsOsExtendedConfigDesc_Done;
    }

    nts = USBD_ValidateExtendedConfigurationDescriptor(extConfigDesc,
                                                       bytesReturned,
                                                       pdoExt->CachedConfigDescriptor,
                                                       pdoExt->CachedConfigDescriptor->wTotalLength,
                                                       USBHUB_HEAP_TAG);
    if(!NT_SUCCESS(nts)){
        DbgTrace((UsbhDebugTrace, "UsbhGetMsOsExtendedConfigDesc: Validation Failed! Status = %x\n",
                  nts));
        goto UsbhGetMsOsExtendedConfigDesc_Done;
    }

UsbhGetMsOsExtendedConfigDesc_Done:

    if(NT_SUCCESS(nts)){
        UsbhAssert(pdoExt->HubFdo, extConfigDesc);
        return extConfigDesc;
    }

    else{
        if(extConfigDesc){
            UsbhFreePool(extConfigDesc);
        }
        return NULL;
    }
}

VOID
UsbhInstallMsOsExtendedProperties(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:


This routine is called to instal MsOs defined extended properties into the registry.
This routine perfonms the appropriate locking necessary to do this safely such that
the hub and pdo do not 'disappear' while the operation is in progress.

If the device does not suppor the OS descriptor no action is taken.

If extended properties are already installed in th eregistry then no action is taken.



Arguments:

Pdo - This is the PDO after it in initialized by PNP such that the appropriate registry
        keys have been created and are accessible.

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    MS_EXT_PROP_DESC_HEADER msExtPropDescHeader;
    PMS_EXT_PROP_DESC pMsExtPropDesc = NULL;
    NTSTATUS nts;
    ULONG bytesReturned = 0;

    pdoExt = PdoExt(Pdo);

    if (pdoExt->DeviceFlags.MsOsSupported == 0) {
        // MSOS not supported
        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    if (pdoExt->PdoFlags.ExtPropertyInstalled) {
        // been there, done that
        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    // Only try this once
    pdoExt->PdoFlags.ExtPropertyInstalled = 1;

    UsbhAssert(Sc->HubFdo, pdoExt->DeviceFlags.MsOsSupported);

    // use the pnp lock to protect the operation -- this will stall
    // pnp operations in this hub until we can complete.
    UsbhAcquireFdoPnpLock(Sc->HubFdo, Sc, Ev_API, msosExtProp_Tag, 1);

    UsbhAssertPassive();

    // Check if we have already done this for this instance

    if(UsbhExtPropDescSemaphorePresent(Pdo)){
        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    RtlZeroMemory(&msExtPropDescHeader,
                  sizeof(MS_EXT_PROP_DESC_HEADER));

    // Request just the header of the MS Extended Property Descriptor
    //

        nts =  UsbhGetMsOsFeatureDescriptor(Pdo,
                                        1, // Recipient Interface
                                        0, // interface number (BUGBUG)
                                        MS_EXT_PROP_DESCRIPTOR_INDEX,
                                        &msExtPropDescHeader,
                                        sizeof(MS_EXT_PROP_DESC_HEADER),
                                        &bytesReturned);

    if (!NT_SUCCESS(nts)){
        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    //
    // Check the header before requesting the entire descriptor
    //

    if(bytesReturned != sizeof(MS_EXT_PROP_DESC_HEADER) ||
        msExtPropDescHeader.dwLength < sizeof(MS_EXT_PROP_DESC_HEADER) ||
        msExtPropDescHeader.bcdVersion != MS_EXT_PROP_DESC_VER ||
        msExtPropDescHeader.wIndex != MS_EXT_PROP_DESCRIPTOR_INDEX ||
        msExtPropDescHeader.wCount == 0){

        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    //
    // Allocate a buffer large enough for the entire descriptor
    //

    UsbhAllocatePool_Z(pMsExtPropDesc,
                       NonPagedPool,
                       msExtPropDescHeader.dwLength);

    if (!pMsExtPropDesc){
        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    //
    // Request the entire MS Extended Property Descriptor
    //

    nts =  UsbhGetMsOsFeatureDescriptor(Pdo,
                                        1, // Recipient Interface
                                        0, // interface number (BUGBUG)
                                        MS_EXT_PROP_DESCRIPTOR_INDEX,
                                        pMsExtPropDesc,
                                        msExtPropDescHeader.dwLength,
                                        &bytesReturned);

    if(!NT_SUCCESS(nts)){
        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    //
    // Check to see if we have the same header
    //

    if(bytesReturned != msExtPropDescHeader.dwLength ||
       RtlCompareMemory(&msExtPropDescHeader,
                        pMsExtPropDesc,
                        sizeof(MS_EXT_PROP_DESC_HEADER)) !=
       sizeof(MS_EXT_PROP_DESC_HEADER)){

        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    //
    // Validate the descriptor
    //

    nts= USBD_ValidateExtendedPropertyDescriptor(
                    pMsExtPropDesc,
                    bytesReturned);

    if(!NT_SUCCESS(nts)){
        goto UsbhInstallMsOsExtendedProperties_Done;
    }

    //
    // MS Extended Property Descriptor retrieved ok, parse and
    // install each Custom Property Section it contains.
    //

    nts = USBD_InstallExtPropDescSections (Pdo,
                                           pMsExtPropDesc,
                                           pMsExtPropDesc->Header.dwLength);

    if(!NT_SUCCESS(nts)){
        goto UsbhInstallMsOsExtendedProperties_Done;
    }

UsbhInstallMsOsExtendedProperties_Done:

    if(pMsExtPropDesc){
        UsbhFreePool(pMsExtPropDesc);
    }

    UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);

    return;
}

NTSTATUS
UsbhGetMsOsContainerID(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Reads and validates the MS OS Container ID descriptor.


Arguments:
    HubFdo - 

    Pdo - 

Return Value:

    NT Status value indicating an error in querying the device for the container ID

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    MS_CONTAINER_ID_DESC msContainerIdDesc;
    NTSTATUS nts;
    ULONG bytesReturned = 0;

    pdoExt = PdoExt(Pdo);

    if ((pdoExt->DeviceFlags.MsOsSupported == 0) || (pdoExt->DeviceFlags.SkipContainerIdQuery) ){
        // MSOS or Container ID not supported
        return STATUS_SUCCESS;
    }

    RtlZeroMemory(&msContainerIdDesc, sizeof(MS_CONTAINER_ID_DESC));

    // Request the descriptor header
    nts = UsbhGetMsOsFeatureDescriptor(Pdo,
                                       0, // recipient device
                                       0, // interface
                                       MS_CONTAINER_ID_DESCRIPTOR_INDEX,
                                       &msContainerIdDesc.Header,
                                       sizeof(MS_CONTAINER_ID_DESC_HEADER),
                                       &bytesReturned);

    if NT_ERROR(nts) {
        if (pdoExt->DeviceUsbRegPath) {
            ULONG SkipContainerId = 1;

            RtlWriteRegistryValue(RTL_REGISTRY_CONTROL,
                                  pdoExt->DeviceUsbRegPath,
                                  L"SkipContainerIdQuery",
                                  REG_BINARY,
                                  &SkipContainerId,
                                  sizeof(SkipContainerId));

            pdoExt->DeviceFlags.SkipContainerIdQuery = 1;
        }
        return nts;
    }
    
    if (bytesReturned == 0) {
        return STATUS_SUCCESS;
    }
    
    if (bytesReturned != sizeof(MS_CONTAINER_ID_DESC_HEADER) ||
        msContainerIdDesc.Header.bcdVersion != MS_CONTAINER_ID_DESC_VER ||
        msContainerIdDesc.Header.wIndex != MS_CONTAINER_ID_DESCRIPTOR_INDEX ||
        msContainerIdDesc.Header.dwLength != sizeof(MS_CONTAINER_ID_DESC) ) {

        HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, PnpErr_InvalidContainerID,
                      0, 0, &msContainerIdDesc, sizeof(MS_CONTAINER_ID_DESC));
        return STATUS_NOT_SUPPORTED;
    }

    // Get the entire descriptor
    nts = UsbhGetMsOsFeatureDescriptor(Pdo,
                                       0, // recipient device
                                       0, // interface
                                       MS_CONTAINER_ID_DESCRIPTOR_INDEX,
                                       &msContainerIdDesc,
                                       sizeof(MS_CONTAINER_ID_DESC),
                                       &bytesReturned);

    if NT_ERROR(nts) {
        if (pdoExt->DeviceUsbRegPath) {
            ULONG SkipContainerId = 1;

            RtlWriteRegistryValue(RTL_REGISTRY_CONTROL,
                                  pdoExt->DeviceUsbRegPath,
                                  L"SkipContainerIdQuery",
                                  REG_BINARY,
                                  &SkipContainerId,
                                  sizeof(SkipContainerId));

            pdoExt->DeviceFlags.SkipContainerIdQuery = 1;

            HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, PnpErr_InvalidContainerID,
                          nts, 0, &msContainerIdDesc, sizeof(MS_CONTAINER_ID_DESC));
        }
        return nts;
    }

    if(bytesReturned != sizeof(MS_CONTAINER_ID_DESC)){
        HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, PnpErr_InvalidContainerID,
                      0, 0, &msContainerIdDesc, sizeof(MS_CONTAINER_ID_DESC));
        return STATUS_NOT_SUPPORTED;
    }

    // Check for Container ID of all zeros

    if (IsEqualGUID((LPGUID)&msContainerIdDesc.Data.bContainerID, (LPGUID) &GUID_NULL)) {
        // Container ID is all zeros.  Throw an exception.  
        HUB_EXCEPTION(USBFILE,  HubFdo, pdoExt->PortNumber, PnpErr_InvalidContainerID,
                      0, 0, &msContainerIdDesc, sizeof(MS_CONTAINER_ID_DESC));
        return STATUS_NOT_SUPPORTED;
    }

    // Save Container ID into our PDO extension
    RtlCopyMemory(&pdoExt->ContainerID, &msContainerIdDesc.Data.bContainerID, sizeof(GUID));
    pdoExt->ContainerIDValid = TRUE;

    return STATUS_SUCCESS;
}

BOOLEAN
UsbhGetNextMsOs20Descriptor(
    __in PUCHAR EndOfDescriptorSet,
    __inout PMSOS20_COMMON_DESCRIPTOR *CurrentDescriptor
    )
/*++

Routine Description:

    Returns the next MS OS 2.0 Descriptor

Returns:

    MSOS20_COMMON_DESCRIPTOR or NULL

--*/
{
    PUCHAR                      nextDescriptor;
    PMSOS20_COMMON_DESCRIPTOR   msOs20CommonDescriptor;
    
    if ((*CurrentDescriptor)->wDescriptorType == MSOS20_SUBSET_HEADER_FUNCTION) {
        PMSOS20_FUNCTION_SUBSET_HEADER functionSubset;

        functionSubset = (PMSOS20_FUNCTION_SUBSET_HEADER)*CurrentDescriptor;

        if (functionSubset->wLength < sizeof(MSOS20_FUNCTION_SUBSET_HEADER)) {
            *CurrentDescriptor = NULL;
            return FALSE;
        }

        if (functionSubset->wTotalLength < 
            (sizeof(MSOS20_COMMON_DESCRIPTOR) + sizeof(MSOS20_FUNCTION_SUBSET_HEADER))) {

            *CurrentDescriptor = NULL;
            return FALSE;
        }
        
        nextDescriptor = (PUCHAR)functionSubset + functionSubset->wTotalLength;

    } else if ((*CurrentDescriptor)->wDescriptorType == MSOS20_SUBSET_HEADER_CONFIGURATION) {
        PMSOS20_CONFIGURATION_SUBSET_HEADER configurationSubset;

        configurationSubset = (PMSOS20_CONFIGURATION_SUBSET_HEADER)*CurrentDescriptor;

        if (configurationSubset->wLength < sizeof(MSOS20_CONFIGURATION_SUBSET_HEADER)) {
            *CurrentDescriptor = NULL;
            return FALSE;
        }

        if (configurationSubset->wTotalLength < 
            (sizeof(MSOS20_COMMON_DESCRIPTOR) + sizeof(MSOS20_CONFIGURATION_SUBSET_HEADER))) {

            *CurrentDescriptor = NULL;
            return FALSE;
        }

        nextDescriptor = (PUCHAR)configurationSubset + configurationSubset->wTotalLength;
    
    } else {

        if ((*CurrentDescriptor)->wLength < sizeof(MSOS20_COMMON_DESCRIPTOR)) {
            *CurrentDescriptor = NULL;
            return FALSE;
        }
        
        nextDescriptor = (PUCHAR)*CurrentDescriptor + (*CurrentDescriptor)->wLength;
    }

    if (nextDescriptor == EndOfDescriptorSet) {
        *CurrentDescriptor = NULL;
        return TRUE;
    }

    if (nextDescriptor > EndOfDescriptorSet) {
        *CurrentDescriptor = NULL;
        return FALSE;
    }

    if ((nextDescriptor + sizeof(MSOS20_COMMON_DESCRIPTOR)) > EndOfDescriptorSet) {
        *CurrentDescriptor = NULL;
        return FALSE;
    }

    msOs20CommonDescriptor = (PMSOS20_COMMON_DESCRIPTOR) nextDescriptor;

    if ((nextDescriptor + msOs20CommonDescriptor->wLength) > EndOfDescriptorSet) {
        *CurrentDescriptor = NULL;
        return FALSE;
    }
    
    *CurrentDescriptor = msOs20CommonDescriptor;

    return TRUE;
}


NTSTATUS
UsbhGetMsOs20DescriptorSet(
    PDEVICE_OBJECT  HubFdo,
    PDEVICE_OBJECT  Pdo,
    PMSOS20_DESCRIPTOR_SET_INFO DescriptorInfo
    )
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USBD_STATUS usbdStatus;
    USHORT dataReturned;

    UsbhAllocatePool_Z(pdoExt->MsOs20Info.DescriptorSet, NonPagedPool, DescriptorInfo->wLength);

    if (pdoExt->MsOs20Info.DescriptorSet == NULL) {
        LOG(HubFdo, LOG_BUS, 'MS20', 0, 0);
        pdoExt->MsOs20Info.BytesReturned = 0;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // setup packet for Get_Descriptor, string
    setupPacket.bmRequestType.Recipient = 0;
    setupPacket.bmRequestType.Type = BMREQUEST_VENDOR;
    setupPacket.bmRequestType.Reserved = 0;
    setupPacket.bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;
    setupPacket.bRequest = DescriptorInfo->bMS_VendorCode;
    setupPacket.wValue.W = 0;
    setupPacket.wIndex.W = MSOS20_DESCRIPTOR_INDEX;
    setupPacket.wLength = DescriptorInfo->wLength;
    dataReturned = DescriptorInfo->wLength;

    nts = UsbhSyncSendCommandToDevice(pdoExt->HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      (PUCHAR)pdoExt->MsOs20Info.DescriptorSet,
                                      &dataReturned,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      &usbdStatus);

    if (!NT_SUCCESS(nts)) {
        LOG(HubFdo, LOG_BUS, 'MS21', 0, 0);
        pdoExt->MsOs20Info.BytesReturned = 0;
        UsbhFreePool(pdoExt->MsOs20Info.DescriptorSet);
    } else {    
        pdoExt->MsOs20Info.BytesReturned = dataReturned;
    }

    return nts;
}



BOOLEAN
UsbhParseAndInstallRegistryValueDescriptors(
    HANDLE RegHandle,
    PMSOS20_INFO MsOs20Info,
    PMSOS20_COMMON_DESCRIPTOR MsOs20DescriptorSet,
    PUCHAR EndOfSet
    )
{
    BOOLEAN result;
    PMSOS20_COMMON_DESCRIPTOR msOs20CommonDescriptor;
    NTSTATUS nts;

    msOs20CommonDescriptor = MsOs20DescriptorSet;

    do {
      
        result = UsbhGetNextMsOs20Descriptor(EndOfSet,
                                             &msOs20CommonDescriptor);

        if (result == FALSE) {
            return FALSE;
        }

        if (msOs20CommonDescriptor != NULL) {
            if (msOs20CommonDescriptor->wDescriptorType == MSOS20_FEATURE_REGISTRY_PROPERTY) {

                PMSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR msOs20RegVal;
                UNICODE_STRING unicodeNameString;
                PMSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA propertyData;

                msOs20RegVal = (PMSOS20_FEATURE_REGISTRY_VALUE_DESCRIPTOR) msOs20CommonDescriptor;
            
                propertyData = (PMSOS20_FEATURE_REGISTRY_VALUE_PROPERTY_DATA) 
                                 ((PUCHAR)(msOs20RegVal->PropertyName) + 
                                   msOs20RegVal->wPropertyNameLength);

                RtlInitUnicodeString(&unicodeNameString,
                                     msOs20RegVal->PropertyName);

                nts = ZwSetValueKey(RegHandle,
                                    &unicodeNameString,
                                    0,
                                    msOs20RegVal->wPropertyDataType,
                                    propertyData->PropertyData,
                                    propertyData->wPropertyDataLength);

                if (!NT_SUCCESS(nts)) {
                    return FALSE;
                }
            } 
        }
    } while (msOs20CommonDescriptor != NULL);

    return TRUE;
}

VOID
UsbhInstallMsOs20RegistryProperties(
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:


Arguments:

Pdo - This is the PDO after it in initialized by PNP such that the appropriate registry
      keys have been created and are accessible.

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts;
    PMSOS20_COMMON_DESCRIPTOR msOs20CommonDescriptor;
    PUCHAR endOfDescriptorSet;
    HANDLE regHandle;
    BOOLEAN result;

    pdoExt = PdoExt(Pdo);

    // use the pnp lock to protect the operation -- this will stall
    // pnp operations in this hub until we can complete.
    UsbhAcquireFdoPnpLock(Sc->HubFdo, Sc, Ev_API, msosExtProp_Tag, 1);

    UsbhAssertPassive();

    // Check if we have already done this for this instance
    if(UsbhExtPropDescSemaphorePresent(Pdo)){
        UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);
        return;
    }

    nts= IoOpenDeviceRegistryKey(Pdo,
                                 PLUGPLAY_REGKEY_DEVICE,
                                 STANDARD_RIGHTS_ALL,
                                 &regHandle);

    if (!NT_SUCCESS(nts)) {
        UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);
        return;
    }

    // Walk the MS OS 2.0 Descriptor set and install registry properties
    msOs20CommonDescriptor = (PMSOS20_COMMON_DESCRIPTOR)pdoExt->MsOs20Info.DescriptorSet;
    endOfDescriptorSet = (PUCHAR)pdoExt->MsOs20Info.DescriptorSet +
                         pdoExt->MsOs20Info.DescriptorSet->wTotalLength;

    result = UsbhParseAndInstallRegistryValueDescriptors(regHandle,
                                                         &pdoExt->MsOs20Info,
                                                         msOs20CommonDescriptor,
                                                         endOfDescriptorSet);

    ZwClose(regHandle);
    UsbhReleaseFdoPnpLock(Sc->HubFdo, Sc);

    if (result == TRUE) {
        pdoExt->PdoFlags.ExtPropertyInstalled = 1;
    }
}


NTSTATUS
UsbhSendMsOs20AltEnumCommand(
    PDEVICE_OBJECT  HubFdo,
    PDEVICE_OBJECT  Pdo,
    PMSOS20_DESCRIPTOR_SET_INFO DescriptorInfo
    )
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    USBD_STATUS usbdStatus;
    USHORT dataLength;

    // setup packet for Alternate Enumeration Command
    setupPacket.bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
    setupPacket.bmRequestType.Type = BMREQUEST_VENDOR;
    setupPacket.bmRequestType.Reserved = 0;
    setupPacket.bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;
    setupPacket.bRequest = DescriptorInfo->bMS_VendorCode;
    setupPacket.wValue.HiByte = DescriptorInfo->bAltEnumCmd;
    setupPacket.wValue.LowByte = 0;
    setupPacket.wIndex.W = MSOS20_SET_ALT_ENUMERATION;
    setupPacket.wLength = 0;
    dataLength = 0;

    nts = UsbhSyncSendCommandToDevice(pdoExt->HubFdo,
                                      Pdo,
                                      &setupPacket,
                                      NULL,
                                      &dataLength,
                                      USBH_CONTROL_PIPE_TIMEOUT,
                                      &usbdStatus);

    if (!NT_SUCCESS(nts)) {
        LOG(HubFdo, LOG_BUS, 'AE21', 0, 0);
    } else {    
        pdoExt->PdoFlags.MsOs20AltEnumCmdSent = TRUE;
    }

    return nts;
}
