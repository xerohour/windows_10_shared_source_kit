/*
Module: WMI.C


name:USB WMI notification events

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    3-10-2002

Abstract:

    This is the code that sends and receives notifications to or from  USBUI. In other
    words this is where the popup messages come from.  The messages themselves are stored
    in USBUI.

Notification Class structures *(WinNT5.0(2000) WinNT5.1(XP) WinNT6)*:

    Data is sent from the hub driver thru WMI to USBUI for connection notification
    events (enumeration failures etc..). Notifications are sent via WmiFireEvent using
    GUID_USB_WMI_STD_NOTIFICATION.  Notifications may also be passed into the hub driver
    where information is filled in and returned to USBUI.

        + Connection Notification *USB_CONNECTION_NOTIFICATION*
        + USB_BUS_NOTIFICATION
        + USB_HUB_NOTIFICATION

Module Entry Points:

<pre>
         PNP event                       Function
    -----------------------------------------------------------
    Entry  -->linked to DriverEntry      UsbhWmiDriverEntry
    Unload -->linked to Unload           UsbhWmiUnload
    Start  -->linked to PNP START        NULL
    Stop   -->linked to PNP STOP         NULL
    Add    -->linked to AddDevice        UsbhWmiPnpStart
    Remove -->linked to PNP REMOVE       UsbhWmiPnpStop

</pre>

*/

#include "hubdef.h"
#include "pch.h"

#include <wmistr.h>

#ifdef HUB_WPP
#include "wmi.tmh"
#endif

CCHAR usbfile_wmi_c[] = "wmi.c";

#define USBFILE usbfile_wmi_c


/*-----------------------------------------------------------------------------------

name: WMI Connection Notification

    Parameters are dependent on the USB_NOTIFICATION_TYPE.

Connection Notification Structure:

<pre>

    typedef struct _USB_CONNECTION_NOTIFICATION {
        USB_NOTIFICATION_TYPE NotificationType;
        ULONG ConnectionNumber;
        ULONG RequestedBandwidth;
        ULONG EnumerationFailReason;
        ULONG PowerRequested;
        ULONG HubNameLength;
    }

</pre>


Connection Event Types (of type USB_NOTIFICATION_TYPE):

    These events are sent via WmiFireEvent when they are detected by the hub driver.

    + EnumerationFailure
    + InsufficentBandwidth
    + InsufficentPower
    + OverCurrent


Connection Query Types (of type USB_NOTIFICATION_TYPE):

    These APIs take action and/or return information.

    + ResetOvercurrent
    + EnumerationFailure

------------------------------------------------------------------------------------*/



#define MAX_NUM_WMI_SUPPORTED_GUIDS         6
#define MAX_NUM_PORT_WMI_SUPPORTED_GUIDS    3

UNICODE_STRING UsbhRegistryPath;
BOOLEAN UsbhWmiInit = FALSE;

WMIGUIDREGINFO USB_WmiGuidList[MAX_NUM_WMI_SUPPORTED_GUIDS];
WMIGUIDREGINFO USB_PortWmiGuidList[MAX_NUM_PORT_WMI_SUPPORTED_GUIDS];

#define WMI_USB_POWER_DEVICE_WAKE_ENABLE     3

typedef struct _USB_DEVICE_UI_FIRMWARE_REVISION
{
    USHORT Length;
    WCHAR FirmwareRevisionString[1];
} USB_DEVICE_UI_FIRMWARE_REVISION, *PUSB_DEVICE_UI_FIRMWARE_REVISION;



NTSTATUS
UsbhWmiDriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING UniRegistryPath
    )
/*++

Routine Description:

    inits global stuff we need for WMI support

    called in context of DriverEntry

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PUNICODE_STRING registryPath = &UsbhRegistryPath;
    ULONG gIdx = 0;

    UsbhKdPrint((1, "USBHUB.SYS\n"));
    
    //
    // Need to ensure that the registry path is null-terminated.
    // Allocate pool to hold a null-terminated copy of the path.
    // Safe in paged pool since all registry routines execute at
    // PASSIVE_LEVEL.
    //
    registryPath->MaximumLength = UniRegistryPath->Length + sizeof(UNICODE_NULL);
    registryPath->Length = UniRegistryPath->Length;

    UsbhAllocatePool_Z(registryPath->Buffer,
                       PagedPool,
                       registryPath->MaximumLength);

    if (registryPath->Buffer) {

       // make a copy of the reg path
       RtlCopyMemory(registryPath->Buffer,
                     UniRegistryPath->Buffer,
                     UniRegistryPath->Length);
    } else {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // These are the GUIDs that we support for the HUB.
    gIdx = 0;
    USB_WmiGuidList[gIdx].Guid = (LPCGUID)&GUID_USB_WMI_STD_DATA;
    USB_WmiGuidList[gIdx].InstanceCount = 1;
    USB_WmiGuidList[gIdx].Flags = 0;
    gIdx++;
    UsbhAssert(NULL, gIdx < MAX_NUM_WMI_SUPPORTED_GUIDS);

    USB_WmiGuidList[gIdx].Guid = (LPCGUID)&GUID_USB_WMI_STD_NOTIFICATION;
    USB_WmiGuidList[gIdx].InstanceCount = 1;
    USB_WmiGuidList[gIdx].Flags = 0;
    gIdx++;
    UsbhAssert(NULL, gIdx < MAX_NUM_WMI_SUPPORTED_GUIDS);


    // "Allow this device to save power" AKA Selective Suspend
    // must be index 2
    UsbhAssert(NULL, WMI_USB_POWER_DEVICE_ENABLE == gIdx);
    USB_WmiGuidList[gIdx].Guid = (LPCGUID)&GUID_POWER_DEVICE_ENABLE;
    USB_WmiGuidList[gIdx].InstanceCount = 1;
    USB_WmiGuidList[gIdx].Flags = 0;
    gIdx++;
    UsbhAssert(NULL, gIdx < MAX_NUM_WMI_SUPPORTED_GUIDS);

    // "Allow this device wake system" AKA Wake on connect
    UsbhAssert(NULL, WMI_USB_POWER_DEVICE_WAKE_ENABLE == gIdx);
    USB_WmiGuidList[gIdx].Guid = (LPCGUID)&GUID_POWER_DEVICE_WAKE_ENABLE;
    USB_WmiGuidList[gIdx].InstanceCount = 1;
    USB_WmiGuidList[gIdx].Flags = 0;
    gIdx++;

    UsbhAssert(NULL, WMI_USB_HUB_NODE_INFORMATION == gIdx);
    USB_WmiGuidList[gIdx].Guid = (LPCGUID)&GUID_USB_WMI_NODE_INFO;
    USB_WmiGuidList[gIdx].InstanceCount = 1;
    USB_WmiGuidList[gIdx].Flags = 0;
    gIdx++;

    UsbhAssert(NULL, gIdx < MAX_NUM_WMI_SUPPORTED_GUIDS);
    USB_WmiGuidList[gIdx].Guid =
        (LPCGUID)&GUID_USB_WMI_SURPRISE_REMOVAL_NOTIFICATION;
    USB_WmiGuidList[gIdx].InstanceCount = 1;
    USB_WmiGuidList[gIdx].Flags = WMIREG_FLAG_EVENT_ONLY_GUID;
    gIdx++;


    // These are the GUIDs that we support for the PORT PDOs.

    USB_PortWmiGuidList[0].Guid = (LPCGUID)&MSDeviceUI_FirmwareRevision_GUID;
    USB_PortWmiGuidList[0].InstanceCount = 1;
    USB_PortWmiGuidList[0].Flags = 0;

    USB_PortWmiGuidList[1].Guid = (LPCGUID)&GUID_USB_WMI_DEVICE_PERF_INFO;
    USB_PortWmiGuidList[1].InstanceCount = 1;
    USB_PortWmiGuidList[1].Flags = 0;

    USB_PortWmiGuidList[2].Guid = (LPCGUID)&GUID_USB_WMI_NODE_INFO;
    USB_PortWmiGuidList[2].InstanceCount = 1;
    USB_PortWmiGuidList[2].Flags = 0;

    UsbhWmiInit = TRUE;

    return STATUS_SUCCESS;
}


VOID
UsbhWmiUnload(
    PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    frees global stuff we needed for WMI support

    called in context of DriverUnload

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));

    if (!UsbhWmiInit) {
        return;
    }

    if (UsbhRegistryPath.Buffer) {
        UsbhFreePool(UsbhRegistryPath.Buffer);
        UsbhRegistryPath.Buffer = NULL;
    }

    UsbhWmiInit = FALSE;
}



NTSTATUS
UsbhWmiPnpStart(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Allocs what we need for WMI support

    called in context of AddDevice

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PWMILIB_CONTEXT wmiLibInfo;

    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));
    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    wmiLibInfo = &hubExt->WmiLibInfo;

    wmiLibInfo->GuidCount = sizeof (USB_WmiGuidList) /
                            sizeof (WMIGUIDREGINFO);
    UsbhAssert(HubFdo, MAX_NUM_WMI_SUPPORTED_GUIDS == wmiLibInfo->GuidCount);

    wmiLibInfo->GuidList = USB_WmiGuidList;
    wmiLibInfo->QueryWmiRegInfo = UsbhQueryWmiRegInfo;
    wmiLibInfo->QueryWmiDataBlock = UsbhQueryWmiDataBlock;
    wmiLibInfo->SetWmiDataBlock = UsbhSetWmiDataBlock;
    wmiLibInfo->SetWmiDataItem = NULL;
    wmiLibInfo->ExecuteWmiMethod = UsbhExecuteWmiMethod;
    wmiLibInfo->WmiFunctionControl = NULL;

    IoWMIRegistrationControl(HubFdo,
                             WMIREG_ACTION_REGISTER);

    hubExt->FdoFlags.WmiRegistered=1;

    LOG(HubFdo, LOG_WMI, 'Awmi', 0, 0);

    return STATUS_SUCCESS;
}


VOID
UsbhWmiPnpStop(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Frees WMI resources

    called in context of MN_REMOVE, undoes alloc

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    if (hubExt->FdoFlags.WmiRegistered) {
        IoWMIRegistrationControl(HubFdo,
                                 WMIREG_ACTION_DEREGISTER);

        hubExt->FdoFlags.WmiRegistered = 0;
    }

}


NTSTATUS
UsbhSetWmiDataBlock(
    IN PDEVICE_OBJECT HubFdo,
    IN PIRP Irp,
    IN ULONG GuidIndex,
    IN ULONG InstanceIndex,
    IN ULONG BufferSize,
    IN PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to set the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    BufferSize has the size of the data block passed

    Buffer has the new values for the data block


Return Value:

    status

--*/
{
    PDEVICE_EXTENSION_HUB   hubExt;
    NTSTATUS nts;
    ULONG size = 0;

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_WMI, 'sWMI', 0, 0);
    // must be passive since we stop SS synchronously
    UsbhAssertPassive();

    switch(GuidIndex) {
    case WMI_USB_DRIVER_INFORMATION:

        nts = /*STATUS_WMI_READ_ONLY*/STATUS_INVALID_DEVICE_REQUEST;
        break;

    case WMI_USB_POWER_DEVICE_WAKE_ENABLE:

        //
        //  This code controls the UI check box 'Allow this device to wake
        //  the system'.  Essentially this is control for the wake on connect
        //  feature.
        //
        if (HubG.GlobalWakeOnConnectUI != HUB_WAKE_C_UserControl) {
            // hide UI
            nts = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        LOG(HubFdo, LOG_WMI, 'wmi3', Buffer, BufferSize);

        size = sizeof(BOOLEAN);
        if (BufferSize < size) {
            nts = STATUS_BUFFER_TOO_SMALL;
            break;
        }

         // set the default
        if (*(PBOOLEAN)Buffer) {
            hubExt->FdoFlags.AllowWakeOnConnect = 1;
        } else {
            hubExt->FdoFlags.AllowWakeOnConnect = 0;
        }

        UsbhSetRegistryWakeOnConnect(HubFdo, *(PBOOLEAN)Buffer);

        nts = STATUS_SUCCESS;
        break;

    case WMI_USB_POWER_DEVICE_ENABLE:

        // *This code is used to set the selective suspend option from the UI,
        // it sets the appropriate registry key and dynamically starts or stops
        // the feature for the bus in the port driver.

        // we do not check the UI flag here because that attribute is controlled
        // at load time.  If we are getting this call then the UI must be present
        // and we simply respond to the request.

        // NOTE: This will override the global selective suspend policy set via
        // the power callback.

        size = sizeof(BOOLEAN);

        if (BufferSize < size) {
            nts = STATUS_BUFFER_TOO_SMALL;
        } else if (0 != InstanceIndex) {
            nts = STATUS_INVALID_DEVICE_REQUEST;
        } else if (hubExt->SSH_EnabledStatus == SSH_DISABLED_VIA_USBPORT) {
            nts = STATUS_INVALID_DEVICE_REQUEST; 
        } else {
            if (*(PBOOLEAN)Buffer) {
                UsbhSshEnableDisable(HubFdo, SSH_ENABLED_VIA_WMI);
                UsbhSetHubSelectiveSuspend(HubFdo, TRUE);
            } else {
                UsbhSshEnableDisable(HubFdo, SSH_DISABLED_VIA_WMI);
                UsbhSetHubSelectiveSuspend(HubFdo, FALSE);
            }
            nts = STATUS_SUCCESS;
        }
        break;

    default:

        nts = STATUS_WMI_GUID_NOT_FOUND;
    }

    nts = WmiCompleteRequest(HubFdo,
                             Irp,
                             nts,
                             0,
                             IO_NO_INCREMENT);

    return(nts);
}


NTSTATUS
UsbhGetHubNodeInfo(
                    PDEVICE_OBJECT HubFdo,
                    __in_bcount(BufferLength) PUCHAR Buffer,
                    ULONG BufferLength,
                    PULONG LengthOfDataReturned
                    )
/*++

Routine Description:




Arguments:

    Pdo - Pdo associated with the query

    Buffer - Buffer for returned node info.

    BufferLength - input, sizeof of data buffer, output length of data copied.
    
    LengthOfDataReturned - if STATUS_BUFFER_TOO_SMALL, returns the required buffer length
    Otherwise returns the number of bytes written.

Return Value:

    status

--*/
{

    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    PUSB_DEVICE_NODE_INFO usbNodeInfo = NULL;
    PUSB_HUB_DEVICE_INFO usbHubDeviceInfo = NULL;
    NTSTATUS nts = STATUS_SUCCESS;
    ULONG size;
    ULONG nPorts, i;
    PUSB_DEVICE_HANDLE dh;

    nPorts = N_PORTS(HubFdo);

    size = sizeof(USB_DEVICE_NODE_INFO) + 
        (nPorts-1)*sizeof(USB_HUB_PORT_INFORMATION);

    if(size > BufferLength){
        *LengthOfDataReturned = size;
        return STATUS_BUFFER_TOO_SMALL;
    }

    RtlZeroBytes(Buffer, BufferLength);
    usbNodeInfo = (PUSB_DEVICE_NODE_INFO)Buffer;

    //
    // Fill in our node header info
    //

    usbNodeInfo->Sig = USB_NODE_INFO_SIG;
    usbNodeInfo->LengthInBytes = size;

    usbNodeInfo->NodeType = HubDevice;
    usbNodeInfo->BusAddress = hubExt->TopologyAddress;
    
    //
    // USB_HUB_DEVICE_INFO
    //

    usbHubDeviceInfo = &usbNodeInfo->HubDeviceInfo;

    RtlCopyBytes(&usbHubDeviceInfo->HubDescriptor,
                 &hubExt->HubDescriptor,
                 sizeof(usbHubDeviceInfo->HubDescriptor));

    usbHubDeviceInfo->HubNumber = hubExt->HubNumber;
    usbHubDeviceInfo->NumberOfHubPorts = N_PORTS(HubFdo);

    //
    // Get our device address from our device handle
    //

    //usbHubDeviceInfo->DeviceAddress;

    if(hubExt->FdoFlags.HubIsRoot){
        usbHubDeviceInfo->HubIsRootHub = TRUE;
    }
    if(!USB_BUS_POWERED_HUB(hubExt)){
        usbHubDeviceInfo->HubIsSelfPowered = TRUE;
    }

    //
    // Fill in port specific information
    //

    for(i=0; i<nPorts; i++){
        PUSB_HUB_PORT_INFORMATION usbHubPortInfo = NULL;
        PHUB_PORT_DATA pd = NULL;
        PDEVICE_OBJECT pdo = NULL;

        usbHubPortInfo = &usbHubDeviceInfo->PortInfo[i];
        pd = UsbhGetPortData(HubFdo, (USHORT)i+1);

        if(!pd){
            continue;
        }

        usbHubPortInfo->PortNumber = pd->PortNumber;
        usbHubPortInfo->ConnectionIndex = pd->PortNumber;
        usbHubPortInfo->ConnectionStatus = pd->ConnectionStatus;
        
        //
        // Try to get the PDO for this port
        //
        pdo = UsbhLatchPdo(HubFdo,
                           (USHORT)i+1,
                           NULL,
                           'WMIH');

        if(pdo){
            PDEVICE_EXTENSION_PDO pdoExt = PdoExt(pdo);

            //
            // Now ref the device handle
            //

            dh = UsbhRefPdoDeviceHandle(pdoExt->HubFdo, pdo, NULL, Devh_PdoApi_HubNode_Tag);

            if (dh) {
                PUSB_DEVICE_INFORMATION_0 level_0 = NULL;

                level_0 = UsbhGetDeviceInformationEx(HubFdo,
                                                     &nts,
                                                     dh);
                if(NT_SUCCESS(nts)){
                    usbHubPortInfo->DeviceAddress = level_0->DeviceAddress;
                    UsbhFreePool(level_0);
                }

                UsbhDerefPdoDeviceHandle(HubFdo,
                                         dh,
                                         NULL,
                                         Devh_PdoApi_HubNode_Tag);
            }

            UsbhUnlatchPdo(HubFdo,
                           pdo,
                           NULL,
                           'WMIH');

        }
    }

    *LengthOfDataReturned = size;

    return STATUS_SUCCESS;
}

NTSTATUS
UsbhQueryWmiDataBlock(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    ULONG GuidIndex,
    ULONG InstanceIndex,
    ULONG InstanceCount,
    PULONG InstanceLengthArray,
    ULONG OutBufferSize,
    PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to query for the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    InstanceCount is the number of instances expected to be returned for
        the data block.

    InstanceLengthArray is a pointer to an array of ULONG that returns the
        lengths of each instance of the data block. If this is NULL then
        there was not enough space in the output buffer to fulfill the request
        so the irp should be completed with the buffer needed.

    BufferAvail on has the maximum size available to write the data
        block.

    Buffer on return is filled with the returned data block


Return Value:

    status

--*/
{
    NTSTATUS nts;
    ULONG size = 0;
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    // because we access the registry
    UsbhAssertPassive();

    switch (GuidIndex) {
    case WMI_USB_DRIVER_INFORMATION:

        LOG(HubFdo, LOG_WMI, 'wmi1', InstanceLengthArray, 0);
        if (InstanceLengthArray != NULL) {
            *InstanceLengthArray = 0;
            nts = STATUS_SUCCESS;
        } else {
            nts = STATUS_BUFFER_TOO_SMALL;
        }
        break;

    case WMI_USB_POWER_DEVICE_WAKE_ENABLE:

        //
        //  This code controls the UI check box 'Allow this device to wake
        //  the system'.  Essentially this is control for the wake on connect
        //  feature.
        //
       if (HubG.GlobalWakeOnConnectUI != HUB_WAKE_C_UserControl) {
            // hide UI
            nts = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        LOG(HubFdo, LOG_WMI, 'wmi3', Buffer, OutBufferSize);

        size = sizeof(BOOLEAN);
        if (OutBufferSize < size) {
            nts = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        if (hubExt->FdoFlags.AllowWakeOnConnect) {
            *(PBOOLEAN)Buffer = TRUE;
        } else {
            *(PBOOLEAN)Buffer = FALSE;
        }
        *InstanceLengthArray = size;
        nts = STATUS_SUCCESS;
        break;

    case WMI_USB_POWER_DEVICE_ENABLE:

        //
        //  This code controls the UI check box 'Allow this device to save
        //  power'
        //

        // This code controls the UI check box "Allow the computer to turn..."
        LOG(HubFdo, LOG_WMI, 'wmi2', Buffer, OutBufferSize);

        // see if the UI is disabled

        if ((HubG.GlobalDisableSelectiveSuspendUI) || (hubExt->SSH_EnabledStatus == SSH_DISABLED_VIA_USBPORT)) {
            // disable UI
            nts = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

        //
        //
        if ((0 != InstanceIndex) || (1 != InstanceCount)) {
            TEST_TRAP();
            nts = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
        size = sizeof(BOOLEAN);
        if (OutBufferSize < size) {
            nts = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        switch (hubExt->SSH_EnabledStatus) {
        case SSH_ENABLED_BY_DEFAULT:
        case SSH_ENABLED_VIA_POWER_POLICY:
        case SSH_ENABLED_VIA_WMI:
            *(PBOOLEAN)Buffer = 1;
            break;
        default:
            *(PBOOLEAN)Buffer = 0;
            break;
        }
        
        *InstanceLengthArray = size;
        nts = STATUS_SUCCESS;
        break;

    case WMI_USB_HUB_NODE_INFORMATION:

        //
        // report hub information
        //

        nts = UsbhGetHubNodeInfo(HubFdo,
                                 Buffer,
                                 OutBufferSize,
                                 &size);

        if(NT_SUCCESS(nts) &&
           InstanceCount == 1 &&
            InstanceLengthArray){
            *InstanceLengthArray = size;
        }
        break;

    default:

        nts = STATUS_WMI_GUID_NOT_FOUND;
    }

    LOG(HubFdo, LOG_WMI, 'wmiC', Irp, size);

    nts = WmiCompleteRequest(HubFdo,
                             Irp,
                             nts,
                             size,
                             IO_NO_INCREMENT);

    LOG(HubFdo, LOG_WMI, 'wmiF', 0, nts);
    return nts;
}



NTSTATUS
UsbhGetPerformanceInfo(PDEVICE_OBJECT Pdo,
                       PUCHAR Buffer,
                       ULONG BufferLength,
                       PULONG LengthOfDataReturned,
                       PUSB_DEVICE_HANDLE DeviceHandle
                       )
/*++

Routine Description:

   This routine queries performance info for a USB device handle thu USBPORT.
   This routine is called in the context of a WMI query on the PDO.

   In the case where this called in the context of WMI the PDO is ref'ed by WMI
   this means it won't be removed and also means we won't release the hub FDO
   so it isn't necessary to latch the PDO here.  However, the PDO may be in any
   PnP state following remove (refAbsent, or Absent) so the data may not be valid.



Arguments:

    Pdo - Pdo associated with the query

    Buffer - Buffer for returned perf info.

    BufferLength - input, sizeof of data buffer, output length of data copied.

    DeviceHandle - device handle we are querying.

Return Value:

    status

--*/
{
    PUSB_HUB_BUS_INTERFACE bus;
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PDEVICE_OBJECT hubFdo = pdoExt->HubFdo;
    NTSTATUS nts;
    USB_DEVICE_PERFORMANCE_INFO_0 level_0;
    PUSB_DEVICE_PERFORMANCE_INFO perfInfo;
    ULONG dataReturned = 0, i, size;

    level_0.InformationLevel = 0;

    bus = &FdoExt(hubFdo)->HubBusInterface;
    if (!bus->GetDevicePerformanceInfo) {
        nts =  STATUS_NOT_IMPLEMENTED;
        *LengthOfDataReturned = 0;
        UsbhAssert(hubFdo, bus->GetDevicePerformanceInfo!=NULL);
        return nts;
    } else {
        nts = bus->GetDevicePerformanceInfo(bus->BusContext,
                                          DeviceHandle,
                                          &level_0,
                                          sizeof(level_0),
                                          &dataReturned);
    }

    if(!NT_SUCCESS(nts) ||
       dataReturned != sizeof(level_0)){
        *LengthOfDataReturned = 0;
        return nts;
    }

    perfInfo = (PUSB_DEVICE_PERFORMANCE_INFO)Buffer;

    //
    // Zero our perf data
    //

    RtlZeroBytes(perfInfo, sizeof(USB_DEVICE_PERFORMANCE_INFO));

    perfInfo->BulkBytes = level_0.BulkBytes;
    perfInfo->BulkUrbCount = level_0.BulkUrbCount;
    perfInfo->ControlDataBytes = level_0.ControlDataBytes;
    perfInfo->ControlUrbCount = level_0.ControlUrbCount;
    perfInfo->InterruptBytes = level_0.InterruptBytes;
    perfInfo->InterruptUrbCount = level_0.InterruptUrbCount;
    perfInfo->IsoBytes = level_0.IsoBytes;
    perfInfo->IsoUrbCount = level_0.IsoUrbCount;
    perfInfo->AllocedIso = level_0.AllocedIso;
    perfInfo->Total32secBandwidth = level_0.Total32secBandwidth;
    perfInfo->TotalTtBandwidth = level_0.TotalTtBandwidth;
    perfInfo->TransferErrors = level_0.TransferErrors;
    perfInfo->DroppedIsoPackets = level_0.DroppedIsoPackets;
    perfInfo->TotalIsoLatency = level_0.TotalIsoLatency;

    for(i=0; i<6; i++){
        perfInfo->AllocedInterrupt[i] = level_0.AllocedInterrupt[i];
    }

    size = sizeof(perfInfo->DeviceDescription);
    if(size > sizeof(pdoExt->DeviceDescription)){
        size = sizeof(pdoExt->DeviceDescription);
    }

    RtlCopyBytes(perfInfo->DeviceDescription,
                 pdoExt->DeviceDescription,
                 size);

    perfInfo->DeviceSpeed = pdoExt->Speed;

    *LengthOfDataReturned = sizeof(USB_DEVICE_PERFORMANCE_INFO);

    return nts;
}


NTSTATUS
UsbhGetDeviceNodeInfo(PDEVICE_OBJECT Pdo,
                      __in_bcount(BufferLength) PUCHAR Buffer,
                      ULONG BufferLength,
                      PULONG LengthOfDataReturned,
                      PUSB_DEVICE_HANDLE DeviceHandle
                      )
/*++

Routine Description:




Arguments:

    Pdo - Pdo associated with the query

    Buffer - Buffer for returned node info.

    BufferLength - input, sizeof of data buffer, output length of data copied.
    
    LengthOfDataReturned - if STATUS_BUFFER_TOO_SMALL, returns the required buffer length
    Otherwise returns the number of bytes written.

    DeviceHandle - device handle we are querying.

Return Value:

    status

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt = PdoExt(Pdo);
    PDEVICE_OBJECT hubFdo = pdoExt->HubFdo;
    PUSB_DEVICE_NODE_INFO usbNodeInfo = NULL;
    PUSB_DEVICE_INFO usbDeviceInfo = NULL;
    NTSTATUS nts = STATUS_SUCCESS;
    ULONG size, stringSize, i;
    PUSB_DEVICE_INFORMATION_0 level_0 = NULL;
    PHUB_PORT_DATA pd = NULL;

    size = sizeof(USB_DEVICE_NODE_INFO);

    if(size > BufferLength){
        *LengthOfDataReturned = size;
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Figure out how many bytes we need for the variable pipe info
    //

    level_0 = UsbhGetDeviceInformationEx(hubFdo,
                                         &nts,
                                         DeviceHandle);

    if(!NT_SUCCESS(nts)){
        *LengthOfDataReturned = 0;
        return STATUS_DEVICE_NOT_CONNECTED;
    }

    size += level_0->NumberOfOpenPipes * sizeof(USB_PIPE_INFO);
    size -= sizeof(USB_PIPE_INFO);

    if(size > BufferLength){
        *LengthOfDataReturned = size;
        UsbhFreePool(level_0);
        return STATUS_BUFFER_TOO_SMALL;
    }

    RtlZeroBytes(Buffer, BufferLength);
    usbNodeInfo = (PUSB_DEVICE_NODE_INFO)Buffer;

    usbNodeInfo->Sig = USB_NODE_INFO_SIG;
    usbNodeInfo->LengthInBytes = size;

    stringSize = sizeof(usbNodeInfo->DeviceDescription);
    if(stringSize > sizeof(pdoExt->DeviceDescription)){
        stringSize = sizeof(pdoExt->DeviceDescription);
    }

    RtlCopyBytes(usbNodeInfo->DeviceDescription,
                 pdoExt->DeviceDescription,
                 stringSize);

    usbNodeInfo->NodeType = UsbDevice;

    UsbhBuildTopologyAddress(hubFdo,
                             &usbNodeInfo->BusAddress,
                             pdoExt->PortNumber);
    
    //
    // USB_DEVICE_INFO
    //

    usbDeviceInfo = &usbNodeInfo->UsbDeviceInfo;

    RtlCopyBytes(&usbDeviceInfo->DeviceDescriptor,
                 &pdoExt->DeviceDescriptor,
                 sizeof(usbDeviceInfo->DeviceDescriptor));

    usbDeviceInfo->Speed = pdoExt->Speed;
    usbDeviceInfo->PortNumber = pdoExt->PortNumber;
    usbDeviceInfo->CurrentConfigurationValue = level_0->CurrentConfigurationValue;
    usbDeviceInfo->DeviceAddress = level_0->DeviceAddress;

    pd = UsbhGetPortData(hubFdo, (USHORT)pdoExt->PortNumber);

    if (pd) {

        usbDeviceInfo->ConnectionIndex = pd->PortNumber;
        usbDeviceInfo->ConnectionStatus = pd->ConnectionStatus;
    }

    //
    // Copy pipe info
    //

    usbDeviceInfo->NumberOfOpenPipes = level_0->NumberOfOpenPipes;
    for(i=0; i < level_0->NumberOfOpenPipes; i++){
        usbDeviceInfo->PipeList[i].EndpointDescriptor = 
            level_0->PipeList[i].EndpointDescriptor;
        usbDeviceInfo->PipeList[i].ScheduleOffset =
            level_0->PipeList[i].ScheduleOffset;
    }

    UsbhFreePool(level_0);

    //
    // Copy PNP strings
    //

    stringSize = pdoExt->PnpHardwareId.LengthInBytes;
    if(stringSize > sizeof(usbDeviceInfo->PnpHardwareId)){
        stringSize = sizeof(usbDeviceInfo->PnpHardwareId);
    }
    RtlCopyBytes(usbDeviceInfo->PnpHardwareId,
                 pdoExt->PnpHardwareId.Buffer,
                 stringSize);

    stringSize = pdoExt->PnpCompatibleId.LengthInBytes;
    if(stringSize > sizeof(usbDeviceInfo->PnpCompatibleId)){
        stringSize = sizeof(usbDeviceInfo->PnpCompatibleId);
    }
    RtlCopyBytes(usbDeviceInfo->PnpCompatibleId,
                 pdoExt->PnpCompatibleId.Buffer,
                 stringSize);

    stringSize = pdoExt->SerialNumberId.LengthInBytes;
    if(stringSize > sizeof(usbDeviceInfo->SerialNumberId)){
        stringSize = sizeof(usbDeviceInfo->SerialNumberId);
    }
    RtlCopyBytes(usbDeviceInfo->SerialNumberId,
                 pdoExt->SerialNumberId.Buffer,
                 stringSize);

    stringSize = pdoExt->FriendlyNameIdString.LengthInBytes;
    if(stringSize > sizeof(usbDeviceInfo->PnpDeviceDescription)){
        stringSize = sizeof(usbDeviceInfo->PnpDeviceDescription);
    }
    RtlCopyBytes(usbDeviceInfo->PnpDeviceDescription,
                 pdoExt->FriendlyNameIdString.Buffer,
                 stringSize);

    *LengthOfDataReturned = size;

    return STATUS_SUCCESS;
}

NTSTATUS
UsbhPdoQueryWmiDataBlock(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    ULONG GuidIndex,
    ULONG InstanceIndex,
    ULONG InstanceCount,
    PULONG InstanceLengthArray,
    ULONG OutBufferSize,
    PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to query for the contents of
    a data block. When the driver has finished filling the data block it
    must call ClassWmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being queried.

    InstanceCount is the number of instances expected to be returned for
        the data block.

    InstanceLengthArray is a pointer to an array of ULONG that returns the
        lengths of each instance of the data block. If this is NULL then
        there was not enough space in the output buffer to fulfill the request
        so the irp should be completed with the buffer needed.

    BufferAvail on has the maximum size available to write the data
        block.

    Buffer on return is filled with the returned data block


Return Value:

    status

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PUSB_DEVICE_UI_FIRMWARE_REVISION fwRevBuf;
    NTSTATUS nts;
    ULONG size = 0;
    PWCHAR revstr;
    USHORT bcdDevice;
    USHORT stringsize;
    PUSB_DEVICE_HANDLE dh;

    pdoExt = PdoExt(DeviceObject);

    switch (GuidIndex) {
    case 0:

        // Return USB device FW revision # in the following format "xx.xx".
        // Need buffer large enough for this string plus NULL terminator.

        LOG(pdoExt->HubFdo, LOG_HUB, 'wmi3', Buffer, OutBufferSize);

        stringsize = 6 * sizeof(WCHAR);

        size = sizeof(struct _USB_DEVICE_UI_FIRMWARE_REVISION) + (ULONG)stringsize;

        if (OutBufferSize < size) {
            nts = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        bcdDevice = pdoExt->DeviceDescriptor.bcdDevice;

        fwRevBuf = (PUSB_DEVICE_UI_FIRMWARE_REVISION)Buffer;

        revstr = &fwRevBuf->FirmwareRevisionString[0];

        *revstr = BcdNibbleToAscii(bcdDevice >> 12);
        *(revstr+1) = BcdNibbleToAscii((bcdDevice >> 8) & 0x000f);
        *(revstr+2) = '.';
        *(revstr+3) = BcdNibbleToAscii((bcdDevice >> 4) & 0x000f);
        *(revstr+4) = BcdNibbleToAscii(bcdDevice & 0x000f);
        *(revstr+5) = 0;

        fwRevBuf->Length = stringsize;

        *InstanceLengthArray = size;
        nts = STATUS_SUCCESS;
        break;

    case WMI_USB_PERFORMANCE_INFORMATION:

        if(OutBufferSize < sizeof(USB_DEVICE_PERFORMANCE_INFO)){
            nts = STATUS_BUFFER_TOO_SMALL;
            size = sizeof(USB_DEVICE_PERFORMANCE_INFO);
            break;
        }

        dh = UsbhRefPdoDeviceHandle(pdoExt->HubFdo, DeviceObject, Irp, Devh_PdoApi_perf_Tag);

        if (dh) {
            
            nts = UsbhGetPerformanceInfo(DeviceObject,
                                         Buffer,
                                         OutBufferSize,
                                         &size,
                                         dh);

            if(NT_SUCCESS(nts) &&
               InstanceCount == 1 &&
               InstanceLengthArray){
                *InstanceLengthArray = size;
            }

            UsbhDerefPdoDeviceHandle(pdoExt->HubFdo,
                                     dh,
                                     Irp,
                                     Devh_PdoApi_perf_Tag);

        } else {
            nts = NT_DEVICE_DISCONNECTED_STATUS;
        }

        break;

    case WMI_USB_DEVICE_NODE_INFORMATION:

        //
        // Only report info for leaf PDOs
        //
        if(pdoExt->PdoFlags.DeviceIsHub || 
           pdoExt->PdoFlags.DeviceIsComposite){
            size = 0;
            nts = STATUS_WMI_GUID_NOT_FOUND;
            break;
        }

        dh = UsbhRefPdoDeviceHandle(pdoExt->HubFdo, DeviceObject, Irp, Devh_PdoApi_DeviceNode_Tag);

        if (dh) {

            nts = UsbhGetDeviceNodeInfo(DeviceObject,
                                        Buffer,
                                        OutBufferSize,
                                        &size,
                                        dh);

            if(NT_SUCCESS(nts) &&
               InstanceCount == 1 &&
               InstanceLengthArray){
                *InstanceLengthArray = size;
            }

            UsbhDerefPdoDeviceHandle(pdoExt->HubFdo,
                                     dh,
                                     Irp,
                                     Devh_PdoApi_DeviceNode_Tag);
        } else {
            size = 0;
            nts = NT_DEVICE_DISCONNECTED_STATUS;
        }

        break;

    default:
        LOG(pdoExt->HubFdo, LOG_HUB, 'wmi4', 0, 0);


        nts = STATUS_WMI_GUID_NOT_FOUND;
    }

    LOG(pdoExt->HubFdo, LOG_HUB, 'wmiC', Irp, size);
    nts = WmiCompleteRequest(DeviceObject,
                                Irp,
                                nts,
                                size,
                                IO_NO_INCREMENT);

    LOG(pdoExt->HubFdo, LOG_HUB, 'wmiP', 0, nts);
    return nts;
}


/*
    Dispatch table for WMI methods we support
*/

typedef NTSTATUS HUBWMI_METHOD(PDEVICE_OBJECT HubFdo,
                            ULONG InBufferSize,
                            ULONG OutBufferSize,
                            PULONG ReturnLength,
                            PUCHAR Buffer);


typedef struct _HUB_WMI_METHOD_DISPATCH {

    USB_NOTIFICATION_TYPE   Notification;
    HUBWMI_METHOD           *MethodHandler;

} HUB_WMI_METHOD_DISPATCH, *PHUB_WMI_METHOD_DISPATCH;


const HUB_WMI_METHOD_DISPATCH UsbhWmiMethod[ModernDeviceInLegacyHub+1] = {

 EnumerationFailure,                UsbhWmiEnumerationFailure,
 InsufficentBandwidth,              UsbhWmiInsufficentBandwidth,
 OverCurrent,                       UsbhWmiDoNothing,
 InsufficentPower,                  UsbhWmiInsufficentPower,
 ResetOvercurrent,                  UsbhWmiResetOvercurrent,
 AcquireBusInfo,                    UsbhAcquireBusInfo,
 AcquireHubName,                    UsbhAcquireHubName,
 AcquireControllerName,             UsbhAcquireControllerName,
 HubOvercurrent,                    UsbhWmiDoNothing,
 HubPowerChange,                    UsbhWmiDoNothing,
 HubNestedTooDeeply,                UsbhWmiDoNothing,
 ModernDeviceInLegacyHub,           UsbhWmiDoNothing

};


NTSTATUS
UsbhExecuteWmiMethod(
    PDEVICE_OBJECT HubFdo,
    PIRP Irp,
    ULONG GuidIndex,
    ULONG InstanceIndex,
    ULONG MethodId,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PUCHAR Buffer
    )
/*++

Routine Description:

    This routine is a callback into the driver to execute a method. When the
    driver has finished filling the data block it must call
    WmiCompleteRequest to complete the irp. The driver can
    return STATUS_PENDING if the irp cannot be completed immediately.

Arguments:

    DeviceObject is the device whose data block is being queried

    Irp is the Irp that makes this request

    GuidIndex is the index into the list of guids provided when the
        device registered

    InstanceIndex is the index that denotes which instance of the data block
        is being called.

    MethodId has the id of the method being called

    InBufferSize has the size of the data block passed in as the input to
        the method.

    OutBufferSize on entry has the maximum size available to write the
        returned data block.

    Buffer on entry has the input data block and on return has the output
        output data block.


Return Value:

    status

--*/
{
    PUSB_NOTIFICATION notification;
    NTSTATUS nts = STATUS_WMI_GUID_NOT_FOUND;
    ULONG size = 0;
    PDEVICE_EXTENSION_HUB hubExt;

    LOG(HubFdo, LOG_WMI, 'WmiM', Irp, Buffer);
    LOG(HubFdo, LOG_WMI, 'Wmi1', OutBufferSize, InBufferSize);
    LOG(HubFdo, LOG_WMI, 'Wmi2', GuidIndex, 0);
    hubExt = FdoExt(HubFdo);

    do {
        HUB_FDO_PNPSTATE pnpState;

        pnpState = hubExt->CurrentFdoPnpState;
        if (pnpState != FDO_WaitPnpStop) {
            nts = NT_DEVICE_DISCONNECTED_STATUS;
            TEST_TRAP();
            break;
        }

        notification = (PUSB_NOTIFICATION) Buffer;

        switch (GuidIndex) {
        case WMI_USB_DRIVER_INFORMATION:
            size = sizeof(struct _USB_NOTIFICATION);
            if (OutBufferSize < size) {
                LOG(HubFdo, LOG_WMI, 'wmi!', Irp, OutBufferSize);
                nts = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            switch(notification->NotificationType) {
            case EnumerationFailure:
            case InsufficentBandwidth:
            case InsufficentPower:
            case OverCurrent:
            case ResetOvercurrent:
            case AcquireBusInfo:
            case AcquireHubName:
            case AcquireControllerName:
            case HubOvercurrent:
            case HubPowerChange:
            case HubNestedTooDeeply:
            case ModernDeviceInLegacyHub:
        
                LOG(HubFdo, LOG_WMI, 'wmM1', Buffer, OutBufferSize);
                LOG(HubFdo, LOG_WMI, 'wmM2', Buffer, notification->NotificationType);
                nts = UsbhWmiMethod[notification->NotificationType].MethodHandler(
                            HubFdo,
                            InBufferSize,
                            OutBufferSize,
                            &size,
                            Buffer);
                break;
                
            default:
                nts = STATUS_WMI_GUID_NOT_FOUND;
                break;
            }                
        }

    } WHILE (0);

    LOG(HubFdo, LOG_WMI, 'WMIC', Irp, nts);
    nts = WmiCompleteRequest(HubFdo,
                             Irp,
                             nts,
                             size,
                             IO_NO_INCREMENT);

    return nts;
}


NTSTATUS
UsbhQueryWmiRegInfo(
    PDEVICE_OBJECT HubFdo,
    PULONG RegFlags,
    PUNICODE_STRING InstanceName,
    PUNICODE_STRING *RegistryPath,
    PUNICODE_STRING MofResourceName,
    PDEVICE_OBJECT *Pdo
    )
/*++

Routine Description:

    This routine is a callback into the driver to retrieve information about
    the guids being registered.

    Implementations of this routine may be in paged memory

Arguments:

    DeviceObject is the device whose registration information is needed

    *RegFlags returns with a set of flags that describe all of the guids being
        registered for this device. If the device wants enable and disable
        collection callbacks before receiving queries for the registered
        guids then it should return the WMIREG_FLAG_EXPENSIVE flag. Also the
        returned flags may specify WMIREG_FLAG_INSTANCE_PDO in which case
        the instance name is determined from the PDO associated with the
        device object. Note that the PDO must have an associated devnode. If
        WMIREG_FLAG_INSTANCE_PDO is not set then Name must return a unique
        name for the device. These flags are ORed into the flags specified
        by the GUIDREGINFO for each guid.

    InstanceName returns with the instance name for the guids if
        WMIREG_FLAG_INSTANCE_PDO is not set in the returned *RegFlags. The
        caller will call ExFreePool with the buffer returned.

    *RegistryPath returns with the registry path of the driver. This is
        required

    MofResourceName returns with the name of the MOF resource attached to
        the binary file. If the driver does not have a mof resource attached
        then this can be returned unmodified. If a value is returned then
        it is NOT freed.

    *Pdo returns with the device object for the PDO associated with this
        device if the WMIREG_FLAG_INSTANCE_PDO flag is returned in
        *RegFlags.

Return Value:

    status

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    *RegFlags = WMIREG_FLAG_INSTANCE_PDO;
    *RegistryPath = &UsbhRegistryPath;
    *Pdo = hubExt->PhysicalDeviceObject;

    LOG(HubFdo, LOG_WMI, 'wmiR', 0, 0);

    return STATUS_SUCCESS;
}


NTSTATUS
UsbhPdoQueryWmiRegInfo(
    PDEVICE_OBJECT DeviceObject,
    PULONG RegFlags,
    PUNICODE_STRING InstanceName,
    PUNICODE_STRING *RegistryPath,
    PUNICODE_STRING MofResourceName,
    PDEVICE_OBJECT *Pdo
    )
/*++

Routine Description:

    This routine is a callback into the driver to retrieve information about
    the guids being registered.

    Implementations of this routine may be in paged memory

Arguments:

    DeviceObject is the device whose registration information is needed

    *RegFlags returns with a set of flags that describe all of the guids being
        registered for this device. If the device wants enable and disable
        collection callbacks before receiving queries for the registered
        guids then it should return the WMIREG_FLAG_EXPENSIVE flag. Also the
        returned flags may specify WMIREG_FLAG_INSTANCE_PDO in which case
        the instance name is determined from the PDO associated with the
        device object. Note that the PDO must have an associated devnode. If
        WMIREG_FLAG_INSTANCE_PDO is not set then Name must return a unique
        name for the device. These flags are ORed into the flags specified
        by the GUIDREGINFO for each guid.

    InstanceName returns with the instance name for the guids if
        WMIREG_FLAG_INSTANCE_PDO is not set in the returned *RegFlags. The
        caller will call ExFreePool with the buffer returned.

    *RegistryPath returns with the registry path of the driver. This is
        required

    MofResourceName returns with the name of the MOF resource attached to
        the binary file. If the driver does not have a mof resource attached
        then this can be returned unmodified. If a value is returned then
        it is NOT freed.

    *Pdo returns with the device object for the PDO associated with this
        device if the WMIREG_FLAG_INSTANCE_PDO flag is returned in
        *RegFlags.

Return Value:

    status

--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts = STATUS_SUCCESS;
    ULONG requiredLength = 0, size;
    PWCHAR description = NULL;

    pdoExt = PdoExt(DeviceObject);

    *RegFlags = WMIREG_FLAG_INSTANCE_PDO;
    *RegistryPath = &UsbhRegistryPath;
    *Pdo = DeviceObject;

    // pdo must be valid before calling IoGetDeviceProperty
    if (!pdoExt->PdoFlags.PnpValid) {
        // PnP not ready yet
        nts = STATUS_INVALID_DEVICE_STATE;

        HUB_EXCEPTION(USBFILE,  pdoExt->HubFdo, 0, GenErr_WmiApiValidationFailure,
                         nts, 0, NULL, 0);
        return nts;
    }

    nts = IoGetDeviceProperty(DeviceObject,
                              DevicePropertyDeviceDescription,
                              sizeof(pdoExt->DeviceDescription),
                              pdoExt->DeviceDescription,
                              &requiredLength);

    if(nts == STATUS_BUFFER_TOO_SMALL){
        UsbhAllocatePool_Z(description, NonPagedPool, requiredLength);

        if(!description){
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        nts = IoGetDeviceProperty(DeviceObject,
                                  DevicePropertyDeviceDescription,
                                  requiredLength,
                                  description,
                                  &requiredLength);

        if(NT_SUCCESS(nts)){

            size = sizeof(pdoExt->DeviceDescription);

            if(sizeof(pdoExt->DeviceDescription) > requiredLength){
                size = requiredLength;
            }

            RtlCopyBytes(pdoExt->DeviceDescription,
                         description,
                         size);
            pdoExt->DeviceDescription[(sizeof(pdoExt->DeviceDescription)/sizeof(WCHAR))-1] =
                UNICODE_NULL;
        }
    }

    if(description){
        UsbhFreePool(description);
    }


    return STATUS_SUCCESS;
}

/*-----------------------------------------------------------------------------------

name: EnumerationFailure_Event

    This notification is sent ->to USER MODE when a USB device can not be enumerated.


parameters:



------------------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------------

name: EnumerationFailure_Query

    This notification(query) is sent from <- USER MODE in response to an
    EnumerationFailure Event.  This query only returns the 'last error' on
    a given port regardless of the presence or absence of a device.


parameters (input) :

    Buffer - pointer to _USB_CONNECTION_NOTIFICATION.
    NotificationType - USB_NOTIFICATION_TYPE.
    ConnectionNumber - (1) based port number.

parameters (output) :

   EnumerationFailReason - exception class code, why it failed.

Returns:

    Error if the port number is out of range.

------------------------------------------------------------------------------------*/
NTSTATUS
UsbhWmiEnumerationFailure(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    )
/*++

Routine Description:

    Handler for the WMI call back for enumeration failure.  The hub returns the
    enumeration FailReasonId ie 'why it failed'.

    *just fills in a failreason id

    Since there is no context for the WMI event we have to do the equivalent of
    GetLastError here -- that is return the last enumeration error that occurred
    on this port.

    The Enumeration FailReasonId maps directly to a Hub Exception, see exdef.h.


Arguments:

    Buffer - pointer to USB_CONNECTION_NOTIFICATION

        >ConnectionNumber - the port number which generated the Wmi Event

Return Value:

    status

--*/
{
    PUSB_CONNECTION_NOTIFICATION connectionNotification =
        (PUSB_CONNECTION_NOTIFICATION)Buffer;
    PHUB_PORT_DATA pd;
    ULONG size;

    LOG(HubFdo, LOG_WMI, 'Mthd', EnumerationFailure, Buffer);
    UsbhAssert(HubFdo, connectionNotification->NotificationType == EnumerationFailure);

    *ReturnLength = 0;
    size = sizeof(struct _USB_CONNECTION_NOTIFICATION);

    if (OutBufferSize < size) {
        return STATUS_BUFFER_TOO_SMALL;
    } else {

        pd = UsbhGetPortData(HubFdo,
                             (USHORT) connectionNotification->ConnectionNumber);
        if (pd) {
            // set the fail reason to the last exception that occurred on this
            // port
            // I don't believe USBUI actually uses this information
            connectionNotification->EnumerationFailReason =
                        pd->LastExceptionClass;
            *ReturnLength = size;
            return STATUS_SUCCESS;
        }

    }

    return STATUS_INVALID_PARAMETER;

}


NTSTATUS
UsbhWmiInsufficentBandwidth(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    )
/*++

Routine Description:

    Handler for the WMI call back for out of bandwidth.

    The last requested bandwidth is stored in the port data structure.

    *just fills in the bandwidth

Arguments:

    Buffer - pointer to USB_CONNECTION_NOTIFICATION

        >ConnectionNumber - the port number which generated the Wmi Event

Return Value:

    status

--*/
{
    PUSB_CONNECTION_NOTIFICATION connectionNotification =
        (PUSB_CONNECTION_NOTIFICATION)Buffer;
    PHUB_PORT_DATA pd;
    ULONG size;

    LOG(HubFdo, LOG_WMI, 'Mthd', InsufficentBandwidth, Buffer);
    UsbhAssert(HubFdo, connectionNotification->NotificationType == InsufficentBandwidth);
    *ReturnLength = 0;

    size = sizeof(struct _USB_CONNECTION_NOTIFICATION);

    if (OutBufferSize < size) {
        return STATUS_BUFFER_TOO_SMALL;
    } else {

        pd = UsbhGetPortData(HubFdo,
                             (USHORT) connectionNotification->ConnectionNumber);
        if (pd) {
            // bandwidth requested in bits/ms
            connectionNotification->RequestedBandwidth =
                        pd->LastBandwidthRequested;
            *ReturnLength = size;
            return STATUS_SUCCESS;
        }

    }

    return STATUS_INVALID_PARAMETER;

}


NTSTATUS
UsbhWmiDoNothing(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    )
/*++

Routine Description:

    The nop method handler, we just return success if parameters are OK
Arguments:

    Buffer - pointer to USB_CONNECTION_NOTIFICATION

        >ConnectionNumber - the port number which generated the Wmi Event

Return Value:

    status

--*/
{
    PUSB_CONNECTION_NOTIFICATION connectionNotification =
        (PUSB_CONNECTION_NOTIFICATION)Buffer;
    PHUB_PORT_DATA pd;
    ULONG size;

    LOG(HubFdo, LOG_WMI, 'Mthd', 0xFFFFFFFF, Buffer);
    size = sizeof(struct _USB_CONNECTION_NOTIFICATION);
    *ReturnLength = 0;

    if (OutBufferSize < size) {
        return STATUS_BUFFER_TOO_SMALL;
    } else {

        pd = UsbhGetPortData(HubFdo,
                             (USHORT) connectionNotification->ConnectionNumber);
        if (pd) {
            return STATUS_SUCCESS;
        }

    }

    return STATUS_INVALID_PARAMETER;

}


NTSTATUS
UsbhAcquireHubName(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    )
/*++

Routine Description:

    returns the symbolic name of the hub PDO. This is used by USBUI to query
    info about the hub through device ioctls.

    we utilize the fact that these structures have some
    elements in common when passing the buffer to other APIs

         USB_HUB_NAME                USB_ACQUIRE_INFO
         ------------                ----------------
    00                               USB_NOTIFICATION_TYPE NotificationType;
    04   ULONG ActualLength;         ULONG TotalSize;
    08   WCHAR HubName[1];           WCHAR Buffer[1];


Arguments:

    Buffer - pointer to USB_ACQUIRE_INFO

Return Value:

    status

--*/
{
    ULONG size;
    PUSB_HUB_NAME hubName;
    PUSB_ACQUIRE_INFO acquireInfo = (PUSB_ACQUIRE_INFO) Buffer;
    NTSTATUS nts;

    DbgTrace((UsbhDebugTrace,">%!FUNC! in-size %d out-size %d\n", InBufferSize, OutBufferSize));
    LOG(HubFdo, LOG_WMI, 'AHN1', Buffer, InBufferSize);
    LOG(HubFdo, LOG_WMI, 'AHN2', Buffer, OutBufferSize);

    *ReturnLength = 0;
    size = sizeof(struct _USB_ACQUIRE_INFO);

    if (OutBufferSize < size ||
        acquireInfo->TotalSize < size) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    // return the smaller of the two as the output length so we don't
    // copy more than necessary
    size = acquireInfo->TotalSize > OutBufferSize ?
                OutBufferSize : acquireInfo->TotalSize;

    hubName = (PUSB_HUB_NAME) &acquireInfo->TotalSize;
    LOG(HubFdo, LOG_WMI, 'AHN3', hubName, size);

    // TotalSize contains the size of the notification type as well
    // consume notification type field
    hubName->ActualLength -= sizeof(USB_NOTIFICATION_TYPE);
    OutBufferSize -= sizeof(USB_NOTIFICATION_TYPE);

    // As long as ActualLength is less than the true output buffer
    // length we are safe
    if (hubName->ActualLength > OutBufferSize) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    // this always succeeds, even if there is not enough room
    nts = UsbhSyncSendDeviceIoctl(HubFdo,
                                   IOCTL_INTERNAL_USB_GET_HUB_NAME,
                                   hubName,
                                   OutBufferSize,
                                   TRUE);

    LOG(HubFdo, LOG_WMI, 'AHN4', nts, 0);

    // readjust to previous value
    if (NT_SUCCESS(nts)) {
        hubName->ActualLength += sizeof(USB_NOTIFICATION_TYPE);
        OutBufferSize += sizeof(USB_NOTIFICATION_TYPE);

        *ReturnLength = size;
    }

    return nts;

}


NTSTATUS
UsbhAcquireControllerName(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    )
/*++

Routine Description:

    we utilize the fact that these structures have some
    elements in common when passing the buffer to other APIs

         USB_HUB_NAME                USB_ACQUIRE_INFO
         ------------                ----------------
    00                               USB_NOTIFICATION_TYPE NotificationType;
    04   ULONG ActualLength;         ULONG TotalSize;
    08   WCHAR HubName[1];           WCHAR Buffer[1];


Arguments:

    Buffer - pointer to USB_ACQUIRE_INFO

Return Value:

    status

--*/
{
    ULONG size;
    PUSB_HUB_NAME controllerName;
    PUSB_ACQUIRE_INFO acquireInfo;
    NTSTATUS nts;

    DbgTrace((UsbhDebugTrace,">%!FUNC! in-size %d out-size %d\n", InBufferSize, OutBufferSize));
    LOG(HubFdo, LOG_WMI, 'ACN1', Buffer, InBufferSize);
    LOG(HubFdo, LOG_WMI, 'ACN2', Buffer, OutBufferSize);

    *ReturnLength = 0;

    size = sizeof(struct _USB_ACQUIRE_INFO);
    acquireInfo = (PUSB_ACQUIRE_INFO) Buffer;

    if (OutBufferSize < size ||
        acquireInfo->TotalSize < size) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    // use the smaller of the two specified values for the copy
    // back to user mode
    size = acquireInfo->TotalSize > OutBufferSize ?
            OutBufferSize : acquireInfo->TotalSize;

    controllerName = (PUSB_HUB_NAME) &acquireInfo->TotalSize;

    LOG(HubFdo, LOG_WMI, 'ACN3', controllerName, size);

    // TotalSize contains the size of the notification type as well
    // consume USB_NOTIFICATION_TYPE
    controllerName->ActualLength -= sizeof(USB_NOTIFICATION_TYPE);
    OutBufferSize -= sizeof(USB_NOTIFICATION_TYPE);

    if (controllerName->ActualLength > OutBufferSize) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    nts = UsbhGetControllerName(HubFdo,
                                controllerName,
                                controllerName->ActualLength);

    LOG(HubFdo, LOG_WMI, 'ACN4', controllerName, nts);

    // readjust to previous value
    if (NT_SUCCESS(nts)) {
        controllerName->ActualLength += sizeof(USB_NOTIFICATION_TYPE);
        OutBufferSize += sizeof(USB_NOTIFICATION_TYPE);
        *ReturnLength = size;
    }

    return nts;
}


NTSTATUS
UsbhWmiInsufficentPower(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    )
/*++

Routine Description:

Arguments:

    Buffer - Handler for not-enough-power event

Return Value:

    status

--*/
{
    PUSB_CONNECTION_NOTIFICATION connectionNotification =
        (PUSB_CONNECTION_NOTIFICATION)Buffer;
    PHUB_PORT_DATA pd;
    ULONG size;
    NTSTATUS nts = STATUS_INVALID_PARAMETER;

    DbgTrace((UsbhDebugTrace,">%!FUNC! in-size %d out-size %d\n", InBufferSize, OutBufferSize));
    LOG(HubFdo, LOG_WMI, 'met1', connectionNotification, OutBufferSize);
    UsbhAssert(HubFdo, connectionNotification->NotificationType == InsufficentPower);
    *ReturnLength = 0;

    do {
        size = sizeof(struct _USB_CONNECTION_NOTIFICATION);

        if (OutBufferSize < size) {
            nts =  STATUS_BUFFER_TOO_SMALL;
            break;
        } else {

            pd = UsbhGetPortData(HubFdo,
                                 (USHORT) connectionNotification->ConnectionNumber);
            if (pd) {
                // power requested in milliamps
                connectionNotification->PowerRequested =
                            pd->LastPowerRequested;
                *ReturnLength = size;

                nts = STATUS_SUCCESS;
                break;
            }

        }
    } WHILE (0);

    DbgTrace((UsbhDebugTrace,"<%!FUNC! %!STATUS!\n", nts));

    return nts;
}


NTSTATUS
UsbhAcquireBusInfo(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    )
/*++

Routine Description:

    Fetches the device relative bus information.  In this case it is relative
    to the hub drivers connection to the USBD bus interface.

Arguments:

    Buffer - pointer to notification structure

Return Value:

    status

--*/
{
    PUSB_BUS_NOTIFICATION busNotification;
    NTSTATUS nts;

    DbgTrace((UsbhDebugTrace,">%!FUNC! in-size %d out-size %d\n", InBufferSize, OutBufferSize));
    busNotification = (PUSB_BUS_NOTIFICATION) Buffer;
    *ReturnLength = 0;

    if (OutBufferSize < sizeof(struct _USB_BUS_NOTIFICATION)) {
        DbgTrace((UsbhDebugTrace,"<%!FUNC!  STATUS_BUFFER_TOO_SMALL\n"));
        return STATUS_BUFFER_TOO_SMALL;
    }

    *ReturnLength = sizeof(struct _USB_BUS_NOTIFICATION);

    nts = UsbhGetDeviceBusInfo(HubFdo, NULL, busNotification, OutBufferSize);

    DbgTrace((UsbhDebugTrace,"<%!FUNC! nts %!STATUS! return length %d\n", nts, *ReturnLength));
    return nts;

}

/*-----------------------------------------------------------------------------------

name: ResetOvercurrent_Query

    This notification(request) is sent from <- USSER MODE in response to an
       Overcurrent Event.  This function is used to 'reset' the overcurrent
       condition on the port.

parameters (input) :

    Buffer - pointer to _USB_CONNECTION_NOTIFICATION.
    NotificationType - USB_NOTIFICATION_TYPE.
    ConnectionNumber - (1) based port number.

parameters (output) :


 ------------------------------------------------------------------------------------*/
NTSTATUS
UsbhWmiResetOvercurrent(
    PDEVICE_OBJECT HubFdo,
    ULONG InBufferSize,
    ULONG OutBufferSize,
    PULONG ReturnLength,
    PUCHAR Buffer
    )
/*++

Routine Description:

Arguments:

    Buffer -

Return Value:

    status

--*/
{
    PUSB_CONNECTION_NOTIFICATION notification;
    ULONG pn;

    DbgTrace((UsbhDebugTrace,">%!FUNC! \n"));

    // buffer size already checked

    notification = (PUSB_CONNECTION_NOTIFICATION) Buffer;
    pn = notification->ConnectionNumber;

    if (pn == 0 && pn > N_PORTS(HubFdo)) {
        return STATUS_INVALID_PARAMETER;
    }

    UsbhOvercurrentResetWorker(HubFdo, pn, PNP_CONTEXT(HubFdo), FALSE);

    return STATUS_SUCCESS;
}


PUSB_CONNECTION_NOTIFICATION
UsbhBuildWmiConnectionNotification(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber
    )
/*++

Routine Description:

   Allocates a connection notification structure for use with WMI.

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    NTSTATUS nts;
    USB_HUB_NAME hubName;
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_CONNECTION_NOTIFICATION notification;

    DbgTrace((UsbhDebugTrace,">%!FUNC! %d\n", PortNumber));
    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    UsbhAllocatePool_Z(notification, NonPagedPool, sizeof(struct _USB_CONNECTION_NOTIFICATION));

    if (notification) {
        notification->ConnectionNumber = PortNumber;


        // the hub name is actually the symbolic name of our PDO so
        // we have to send a request to our own parent to get it.

        nts = UsbhSyncSendDeviceIoctl(HubFdo,
                                      IOCTL_INTERNAL_USB_GET_HUB_NAME,
                                      &hubName,
                                      sizeof(struct _USB_HUB_NAME),
                                      TRUE);

        LOG(HubFdo, LOG_WMI, 'hnl1', notification, nts);

        if (NT_SUCCESS(nts)) {
            notification->HubNameLength = hubName.ActualLength;
        } else {
            notification->HubNameLength = 0;
        }

        LOG(HubFdo, LOG_WMI, 'hnl2', notification, notification->HubNameLength);

    }

    DbgTrace((UsbhDebugTrace,"<%!FUNC! %p\n", notification));

    return notification;
}


VOID
UsbhPdoRegisterWmi(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Registers a PDO with WMI

Arguments:

Irql: PASSIVE

Return Value:

    none, not failable

--*/
{
    PWMILIB_CONTEXT wmiLibInfo;
    PDEVICE_EXTENSION_PDO pdoExt;

    pdoExt = PdoExt(Pdo);

    UsbhAssert(NULL, !pdoExt->PdoFlags.WmiRegistered);
    // register the WMI handler stuff for the PDO
    wmiLibInfo = &pdoExt->WmiLibInfo;

    wmiLibInfo->GuidCount = sizeof (USB_PortWmiGuidList) /
                             sizeof (WMIGUIDREGINFO);
    UsbhAssert(NULL, MAX_NUM_PORT_WMI_SUPPORTED_GUIDS == wmiLibInfo->GuidCount);

    // do not register GUID_USB_WMI_NODE_INFO if device is hub or composite.
    if (pdoExt->PdoFlags.DeviceIsHub || 
        pdoExt->PdoFlags.DeviceIsComposite) {
        wmiLibInfo->GuidCount -= 1;
    }

    wmiLibInfo->GuidList = USB_PortWmiGuidList;
    wmiLibInfo->QueryWmiRegInfo = UsbhPdoQueryWmiRegInfo;
    wmiLibInfo->QueryWmiDataBlock = UsbhPdoQueryWmiDataBlock;
    wmiLibInfo->SetWmiDataBlock = NULL;
    wmiLibInfo->SetWmiDataItem = NULL;
    wmiLibInfo->ExecuteWmiMethod = NULL;
    wmiLibInfo->WmiFunctionControl = NULL;

    IoWMIRegistrationControl(Pdo,
                             WMIREG_ACTION_REGISTER);
    pdoExt->PdoFlags.WmiRegistered = 1;

    return;
}

NTSTATUS
UsbhBuildTopologyAddress(
    PDEVICE_OBJECT HubFdo,
    PUSB_TOPOLOGY_ADDRESS Address,
    ULONG PortNumber
    )
{
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION_HUB hubExt = FdoExt(HubFdo);
    ULONG count = 0;

    //
    // Copy our address
    //
    RtlCopyMemory(Address, 
                  &hubExt->TopologyAddress, 
                  sizeof(hubExt->TopologyAddress));

    if(Address->RootHubPortNumber == 0){
        NT_ASSERT(hubExt->FdoFlags.HubIsRoot);
        Address->RootHubPortNumber = (USHORT)PortNumber;
    }
    else{
        NT_ASSERT(!hubExt->FdoFlags.HubIsRoot);
        for(count=0; count < 5; count++){
            if(Address->HubPortNumber[count] == 0){
                Address->HubPortNumber[count] = (USHORT)PortNumber;
                break;
            }
        }
    }

    return nts;
}


