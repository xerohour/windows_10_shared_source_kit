/*
Module: ETW.C


name:USBHUB ETW Logging

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    4-9-2008

Abstract:

    Code for handling ETW registration, event logging and rundown events

Module Entry Points:

<pre>
         PNP event                       Function
    -----------------------------------------------------------
    Entry  -->linked to DriverEntry      UsbhEtwDriverEntry
    Unload -->linked to Unload           UsbhEtwUnload
    Start  -->linked to PNP START        NULL
    Stop   -->linked to PNP STOP         NULL
    Add    -->linked to AddDevice        NULL
    Remove -->linked to PNP REMOVE       NULL

</pre>

*/

#include "hubdef.h"
#include "pch.h"


#ifdef HUB_WPP
#include "etw.tmh"
#endif

CCHAR usbfile_etw_c[] = "etw.c";

#define USBFILE usbfile_etw_c


typedef
NTSTATUS  
(*PFN_IO_GET_ACTIVITY_ID_IRP) (
    __in
        PIRP    Irp,
    __out
        LPGUID  Guid
    );  

PFN_IO_GET_ACTIVITY_ID_IRP g_IoGetActivityIdIrp;


NTSTATUS
UsbhEtwDriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING UniRegistryPath
    )
/*++

Routine Description:

    Registers the USBHUB ETW provider

    called in context of DriverEntry

Arguments:

    DriverObject - USBHUB driver object

    UniRegistryPath - Registry path for USBHUB

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    REGHANDLE regHandle;
    UNICODE_STRING functionName;

    UsbhKdPrint((1, "USBHUB.SYS\n"));
    
    //
    // Register with ETW
    //
    status = EtwRegister( &USBHUB_ETW_PROVIDER,
                          UsbhEtwEnableCallback,
                          NULL,
                          &regHandle );

    if (NT_SUCCESS(status)) {
        HubG.EtwRegHandle = regHandle;

        //
        // Telemetry opt-in for Manifested ETW Providers.
        //
        EnableManifestedProviderForMicrosoftTelemetry(regHandle);
    }

    RtlInitUnicodeString(&functionName, L"IoGetActivityIdIrp");
    g_IoGetActivityIdIrp = (PFN_IO_GET_ACTIVITY_ID_IRP)MmGetSystemRoutineAddress(&functionName);

    return status;
}


VOID
UsbhEtwUnload(
    PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Unregisters the USBHUB ETW provider

    called in context of DriverUnload

Arguments:

    DriverObject - USBHUB driver object

Irql: PASSIVE

Return Value:

    none

--*/
{
    DbgTrace((UsbhDebugTrace, "%!FUNC!\n"));

    //
    //  Unregister the driver as an ETW provider
    //
    if (HubG.EtwRegHandle != (REGHANDLE)NULL) {
        EtwUnregister(HubG.EtwRegHandle);
        HubG.EtwRegHandle = (REGHANDLE)NULL;
    }

}

VOID
UsbhEtwEnableCallback (
    __in LPCGUID SourceId,
    __in ULONG IsEnabled,
    __in UCHAR Level,
    __in ULONGLONG MatchAnyKeyword,
    __in ULONGLONG MatchAllKeyword,
    __in_opt PEVENT_FILTER_DESCRIPTOR FilterData,
    __in_opt PVOID CallbackContext
    )
/*++

Routine Description:

    Enable/Disable callback for ETW.  Logs rundown events when ETW is enabled.

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    if (IsEnabled) {
        HubG.EtwLoggingEnabled = 1;
        // Log rundown events
        UsbhEtwRundown();
    } else {
        HubG.EtwLoggingEnabled = 0;
    }
}

VOID
UsbhEtwRundown (
    VOID
    )
/*++

Routine Description:

    Logs rundown events for USBHUB.SYS.  Will log each hub, and all PDOs attached to each hub.

Arguments:

Irql: PASSIVE

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    PLIST_ENTRY hub_le, pdo_le;
    HUB_FDO_PNPSTATE pnpState;
    PDEVICE_OBJECT HubFdo;

    WAIT_EVENT(&HubG.HubFdoOmniListLock);
    
    // Loop through all hubs
    for (hub_le = HubG.HubFdoOmniListHead.Flink;
         hub_le != &HubG.HubFdoOmniListHead;
         hub_le = hub_le->Flink) {

        hubExt = (PDEVICE_EXTENSION_HUB) CONTAINING_RECORD(
                   hub_le,
                   struct _DEVICE_EXTENSION_HUB,
                   FdoOmniLink);    
        
        ASSERT_FDOEXT(hubExt);
        
        HubFdo = hubExt->FunctionalDeviceObject;

        // Prevent hub PnP state change while we are logging the rundown events for the hub
        pnpState = UsbhAcquireFdoPnpLock(HubFdo, PNP_CONTEXT(HubFdo), Ev_ETW, 0, 1);

        // Only log the hub if it has successfully started
        if (pnpState == FDO_WaitPnpStop) {
            // Log the hub's information to ETW
            UsbhEtwLogHubInformation(hubExt, &USBHUB_ETW_EVENT_HUB_INFORMATION, TRUE);

            //
            // Dump the exceptions that we might have recevied in the past
            // for this hub
            //
            UsbhEtwLogHubPastExceptions( hubExt );

            // Log the individual port attributes
            UsbhEtwLogPortInformation(HubFdo);

            // Block all PDO PnP state changes
            UsbhAcquirePdoStateLock(HubFdo, PNP_CONTEXT(HubFdo), PdoEv_ETW);

            // Run down all PDOs for the hub that are on the present list
            for(pdo_le = hubExt->PdoPresentListHead.Flink;
                pdo_le != &hubExt->PdoPresentListHead;
                pdo_le = pdo_le->Flink) {

                pdoExt = (PDEVICE_EXTENSION_PDO) CONTAINING_RECORD(
                        pdo_le,
                        struct _DEVICE_EXTENSION_PDO,
                        PdoListLink);

                ASSERT_PDOEXT(pdoExt);

                // Log the PDO's information to ETW
                UsbhEtwLogDeviceInformation(pdoExt, &USBHUB_ETW_EVENT_DEVICE_INFORMATION, TRUE);

                UsbhEtwLogDeviceDescription(pdoExt, &USBHUB_ETW_EVENT_DEVICE_DESCRIPTION);
            }
    
            UsbhReleasePdoStateLock(HubFdo, PNP_CONTEXT(HubFdo));
        }
        UsbhReleaseFdoPnpLock(HubFdo, PNP_CONTEXT(HubFdo));
    }
    SET_EVENT(&HubG.HubFdoOmniListLock);
}
        

VOID
UsbhEtwLogHubInformation(
    __in PDEVICE_EXTENSION_HUB hubExt,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in BOOLEAN LogHubDescriptor
    )
/*++

Routine Description:

    Logs information about a hub to ETW.

Arguments:

    hubExt - Device extension for the hub we are logging about

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    LogHubDescriptor - TRUE if the hub descriptor should be logged with this event

Return Value:

    none
--*/
{
    ETW_USB_HC EtwUsbHc;
    ETW_USB_HUB EtwUsbHub;
    ETW_USB_HUB_STATE EtwUsbHubState;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    // Get information about the USB host controller the hub is attached to
    UsbhEtwGetHcInfo(hubExt, &EtwUsbHc);

    // Get information about the hub 
    UsbhEtwGetHubInfo(hubExt, &EtwUsbHub);

    EtwUsbHubState.DevicePowerState = hubExt->Fdo_Dx;
    EtwUsbHubState.HubSuspendState = hubExt->SSH_State;
    
    // Write the info to ETW
    UsbhEtwWrite(
        EventDescriptor,
        NULL,
        &EtwUsbHc,
        sizeof(EtwUsbHc),
        &EtwUsbHub,
        sizeof(EtwUsbHub),
        &EtwUsbHubState,
        sizeof(EtwUsbHubState),
        &hubExt->HubDescriptor,
        sizeof(USB_HUB_DESCRIPTOR),
        NULL);
    
}


VOID
UsbhEtwLogHubPastExceptions(
    __in PDEVICE_EXTENSION_HUB  hubExt
)
/*++

Routine Description:

    Logs Hub exceptions that have been stored till now

Arguments:

    hubExt - Device extension for the hub we are logging about

Return Value:

    none
--*/
{
    PHUB_EXCEPTION_RECORD exr = NULL;
    PLIST_ENTRY le = NULL;
    KLOCK_QUEUE_HANDLE lh;
    ULONG portNumber;
    LARGE_INTEGER  currentTime;
    ULONGLONG  relativeTime;
    PCHUB_EXCEPTION_DISPATCH hubExDispatch;
    ETW_USB_HC etwUsbHc;
    ETW_USB_HUB etwUsbHub;


    // Get information about the USB host controller the hub is attached to
    UsbhEtwGetHcInfo(hubExt, &etwUsbHc);

    // Get information about the hub 
    UsbhEtwGetHubInfo(hubExt, &etwUsbHub);


    KeQuerySystemTime( &currentTime );


    KeAcquireInStackQueuedSpinLock(&hubExt->ExceptionListSpin,
                                   &lh);    

    for(le = hubExt->ExceptionList.Flink;
        le != &hubExt->ExceptionList;
        le = le->Flink){       
                                  
        exr = CONTAINING_RECORD(le, HUB_EXCEPTION_RECORD, ExrLink);

        //
        // This is the time difference between the point when event 
        // occured and when Etw logging was enabled 
        //
        relativeTime = currentTime.QuadPart - exr->EventTime.QuadPart;

        hubExDispatch = UsbhGetExceptionDispatch(hubExt->FunctionalDeviceObject, exr->Class);

        portNumber = exr->PortNumber;

        // Write the info to ETW
        UsbhEtwWrite(
            &USBHUB_ETW_EVENT_HUB_PAST_EXCEPTION,
            NULL,
            &relativeTime,
            sizeof(relativeTime),
            &etwUsbHc,
            sizeof(ETW_USB_HC),
            &etwUsbHub,
            sizeof(ETW_USB_HUB),
            &portNumber,
            sizeof(portNumber),
            &exr->Class,
            sizeof(exr->Class),
            &exr->NtStatus,
            sizeof(exr->NtStatus),
            &exr->UsbdStatus,
            sizeof(exr->UsbdStatus),
            hubExDispatch->DebugText,
            (strlen(hubExDispatch->DebugText) + 1) * sizeof(CHAR),
            NULL);

    }

    KeReleaseInStackQueuedSpinLock(&lh);



}


VOID
UsbhEtwLogDeviceDescription(
    __in PDEVICE_EXTENSION_PDO PdoExt,
    __in PCEVENT_DESCRIPTOR EventDescriptor
    )
/*++

Routine Description:

    Logs information about a USB device to ETW.

Arguments:

    pdoExt - Device extension for the usb device PDO we are logging about

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ETW_USB_HC etwUsbHc;
    ETW_USB_DEVICE EtwUsbDevice;

    if ((HubG.EtwLoggingEnabled == 0) || 
        (PdoExt->PnPDeviceDescription == NULL) ||
        (PdoExt->PnPDeviceDescriptionLength == 0)) {
        return;
    }

    hubExt = FdoExt(PdoExt->HubFdo);

    // Get information about the USB host controller the device is attached to
    UsbhEtwGetHcInfo(hubExt, &etwUsbHc);

    // Get information about the USB device we are logging about
    UsbhEtwGetDeviceInfo(PdoExt, &EtwUsbDevice);

    UsbhEtwWrite(EventDescriptor,
                 NULL,
                 &etwUsbHc,
                 sizeof(etwUsbHc),
                 &EtwUsbDevice,
                 sizeof(EtwUsbDevice),
                 PdoExt->PnPDeviceDescription,
                 PdoExt->PnPDeviceDescriptionLength,
                 NULL);
}


VOID
UsbhEtwLogDeviceInformation(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in BOOLEAN LogDeviceDescriptor
    )
/*++

Routine Description:

    Logs information about a USB device to ETW.

Arguments:

    pdoExt - Device extension for the usb device PDO we are logging about

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    LogDeviceDescriptor - TRUE if the device descriptor should be logged with this event

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ETW_USB_HC EtwUsbHc;
    ETW_USB_DEVICE EtwUsbDevice;
    ETW_USB_DEVICE_STATE EtwUsbDeviceState;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    hubExt = FdoExt(pdoExt->HubFdo);

    // Get information about the USB host controller the device is attached to
    UsbhEtwGetHcInfo(hubExt, &EtwUsbHc);

    // Get information about the USB device we are logging about
    UsbhEtwGetDeviceInfo(pdoExt, &EtwUsbDevice);
     
    EtwUsbDeviceState.DevicePowerState = pdoExt->Pdo_Dx;

    if (LogDeviceDescriptor) {
        // Log HC info, Device info, and Device Descriptor
        UsbhEtwWrite(
            EventDescriptor,
            NULL,
            &EtwUsbHc,
            sizeof(EtwUsbHc),
            &EtwUsbDevice,
            sizeof(EtwUsbDevice),
            &EtwUsbDeviceState,
            sizeof(EtwUsbDeviceState),
            &pdoExt->DeviceDescriptor,
            sizeof(USB_DEVICE_DESCRIPTOR),
            NULL);
    } else {
        UsbhEtwWrite(
            EventDescriptor,
            NULL,
            &EtwUsbHc,
            sizeof(EtwUsbHc),
            &EtwUsbDevice,
            sizeof(EtwUsbDevice),
            &EtwUsbDeviceState,
            sizeof(EtwUsbDeviceState),
            NULL);
    }
}


VOID
UsbhEtwLogHubEvent(
    __in PDEVICE_OBJECT HubFdo,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in ULONG PortNumber,
    __in NTSTATUS NtStatus
    )
/*++

Routine Description:

    Logs information about a hub (FDO) event to ETW.

Arguments:

    HubFdo - FDO for the hub we are logging about

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    PortNumber - 1-based port number that the event occurred on.  0 indicates an event for the whole hub.

    NtStatus - NT status value associated with the event.

Return Value:

    none
--*/
{
    UsbhEtwLogHubIrpEvent(HubFdo,
                          NULL,
                          NULL,
                          EventDescriptor,
                          PortNumber,
                          NtStatus);
}


VOID
UsbhEtwLogHubIrpEvent(
    __in PDEVICE_OBJECT HubFdo,
    __in_opt PIRP Irp,
    __in_opt LPGUID ActivityId,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in ULONG PortNumber,
    __in NTSTATUS NtStatus
    )
/*++

Routine Description:

    Logs information about a hub (FDO) event to ETW.

Arguments:

    HubFdo - FDO for the hub we are logging about

    Irp - IRP that can be used to retrieve an activity ID
    
    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    PortNumber - 1-based port number that the event occurred on.  0 indicates an event for the whole hub.

    NtStatus - NT status value associated with the event.

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ETW_USB_HC EtwUsbHc;
    ETW_USB_HUB EtwUsbHub;
    NTSTATUS status;
    GUID activityId;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    hubExt = FdoExt(HubFdo);

    // Get information about the USB host controller the hub is attached to
    UsbhEtwGetHcInfo(hubExt, &EtwUsbHc);

    // Get information about the hub 
    UsbhEtwGetHubInfo(hubExt, &EtwUsbHub);

    if (ActivityId) {
        activityId = *ActivityId;
        status = STATUS_SUCCESS;
    } else {
        status = UsbhEtwGetActivityId(Irp, &activityId);
    }

    // Write the info to ETW
    UsbhEtwWrite(
        EventDescriptor,
        NT_SUCCESS(status) ? &activityId : NULL,
        &EtwUsbHc,
        sizeof(EtwUsbHc),
        &EtwUsbHub,
        sizeof(EtwUsbHub),
        &PortNumber,
        sizeof(PortNumber),
        &NtStatus,
        sizeof(NtStatus),
        NULL);
}

VOID
UsbhEtwLogHubException(
    __in PDEVICE_OBJECT HubFdo,
    __in ULONG PortNumber,
    __in HUB_EXCEPTION_CLASS Class,
    __in NTSTATUS NtStatus,
    __in USBD_STATUS UsbdStatus,
    __in PCSTR DebugText,
    __in ULONG DebugTextSize
    )
/*++

Routine Description:

    Logs information about a hub exception event to ETW

Arguments:

    HubFdo - FDO for the hub we are logging about

    PortNumber - 1-based port number that the event occurred on.  0 indicates an event for the whole hub.

    Class - Hub exception type.

    NtStatus - NT status code for the exception.

    UsbdStatus - URB status code for the exception.

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ETW_USB_HC EtwUsbHc;
    ETW_USB_HUB EtwUsbHub;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    hubExt = FdoExt(HubFdo);

    // Get information about the USB host controller the hub is attached to
    UsbhEtwGetHcInfo(hubExt, &EtwUsbHc);

    // Get information about the hub 
    UsbhEtwGetHubInfo(hubExt, &EtwUsbHub);

    // Write the info to ETW
    UsbhEtwWrite(
        &USBHUB_ETW_EVENT_HUB_EXCEPTION,
        NULL,
        &EtwUsbHc,
        sizeof(EtwUsbHc),
        &EtwUsbHub,
        sizeof(EtwUsbHub),
        &PortNumber,
        sizeof(PortNumber),
        &Class,
        sizeof(Class),
        &NtStatus,
        sizeof(NtStatus),
        &UsbdStatus,
        sizeof(UsbdStatus),
        DebugText,
        DebugTextSize,
        NULL);
}

#pragma pack(1)
typedef struct _ETW_ACPI_UPC_PACKAGE {
    UCHAR   PortIsConnectable;
    UCHAR   PortConnectorType;
    ULONG   Reserved0;
    ULONG   Reserved1;
} ETW_ACPI_UPC_PACKAGE, *PETW_ACPI_UPC_PACKAGE;
#pragma pack()

VOID
UsbhEtwLogPortInformation(
    __in PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Logs information about the ports of a hub to ETW.

Arguments:

    hubFdo - Device object for the hub we are logging about

Return Value:

    none
--*/
{
    USHORT port;
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_EXTENDED_PORT_ATTRIBUTES AttrList;
    PHUB_PORT_DATA PortData;
    ETW_ACPI_UPC_PACKAGE EtwAcpiUpcPackage;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    hubExt = FdoExt(HubFdo);

    AttrList = hubExt->ExtendedPortAttributes;

    for(port=1; port <= N_PORTS(HubFdo); port++) {
        
        PortData = UsbhGetPortData(HubFdo, port);

        // Log the port's extended attributes
        UsbhEtwLogHubEventWithExtraData(HubFdo,
                                &USBHUB_ETW_EVENT_HUB_PORT_ATTRIBUTES,
                                port,
                                &AttrList[port],
                                sizeof(USB_EXTENDED_PORT_ATTRIBUTES));

        if (hubExt->AcpiAttributes) {
            // Log the ACPI _UPC package for the port if it exists
            if (AttrList[port].ul & USB_PORTATTR_ACPI_UPC_VALID_BIT) {
                // ETW aligns structure elements on byte-boundaries, so we need to remap the UPC values to
                // a byte-aligned structure
                EtwAcpiUpcPackage.PortIsConnectable = hubExt->AcpiAttributes[port].UpcPackage.PortIsConnectable;
                EtwAcpiUpcPackage.PortConnectorType = hubExt->AcpiAttributes[port].UpcPackage.PortConnectorType;
                EtwAcpiUpcPackage.Reserved0 = hubExt->AcpiAttributes[port].UpcPackage.Reserved0;
                EtwAcpiUpcPackage.Reserved1 = hubExt->AcpiAttributes[port].UpcPackage.Reserved1;
               
                UsbhEtwLogHubEventWithExtraData(HubFdo,
                                        &USBHUB_ETW_EVENT_HUB_PORT_ACPI_UPC_VALUE,
                                        port,
                                        &EtwAcpiUpcPackage,
                                        sizeof(ETW_ACPI_UPC_PACKAGE));
            }

            // Log the ACPI _PLD structure for the port if it exists
            if (AttrList[port].ul & USB_PORTATTR_ACPI_PLD_VALID_BIT) {
                UsbhEtwLogHubEventWithExtraData(HubFdo,
                                        &USBHUB_ETW_EVENT_HUB_PORT_ACPI_PLD_VALUE,
                                        port,
                                        &hubExt->AcpiAttributes[port].PldBuffer,
                                        sizeof(ACPI_PLD_BUFFER));
            }
        }
    }
}


VOID
UsbhEtwLogHubEventWithExtraData(
    __in PDEVICE_OBJECT HubFdo,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in ULONG PortNumber,
    __in_bcount(DataSize) PVOID Data,
    __in ULONG DataSize
    )
/*++

Routine Description:

    Logs information about a hub (FDO) event to ETW, including a data structure

Arguments:

    HubFdo - FDO for the hub we are logging about

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    PortNumber - 1-based port number that the event occurred on.  0 indicates an event for the whole hub.

    Data - Extra data to be logged about the event.

    DataSize - Size of the extra data to be logged.

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ETW_USB_HC EtwUsbHc;
    ETW_USB_HUB EtwUsbHub;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    hubExt = FdoExt(HubFdo);

    // Get information about the USB host controller the hub is attached to
    UsbhEtwGetHcInfo(hubExt, &EtwUsbHc);

    // Get information about the hub 
    UsbhEtwGetHubInfo(hubExt, &EtwUsbHub);

    // Write the info to ETW
    UsbhEtwWrite(
        EventDescriptor,
        NULL,
        &EtwUsbHc,
        sizeof(EtwUsbHc),
        &EtwUsbHub,
        sizeof(EtwUsbHub),
        &PortNumber,
        sizeof(PortNumber),
        Data,
        DataSize,
        NULL);
}

NTSTATUS
UsbhEtwGetActivityId(
    __in PIRP Irp,
    __out LPGUID ActivityId
    )
/*++

Routine Description:

    Retrieves the activity ID for an IRP

Arguments:

    Irp - IRP that can be used to retrieve an activity ID

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status;

    if (Irp && g_IoGetActivityIdIrp) {
        status = g_IoGetActivityIdIrp(Irp, ActivityId);
    } else {
        status = STATUS_UNSUCCESSFUL;
    }

    return status;
}


VOID
UsbhEtwLogHubPowerEvent(
    __in PDEVICE_OBJECT HubFdo,
    __in_opt PIRP Irp,
    __in_opt LPGUID ActivityId,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in POWER_STATE PowerState,
    __in NTSTATUS NtStatus
    )
/*++

Routine Description:

    Logs information about an set power request for a USB hub.

Arguments:

    HubFdo - FDO for the hub we are logging about

    Irp - IRP that can be used to retrieve an activity ID.  Can be NULL.

    ActivityId - Activity ID to log.  Can be NULL.

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    PowerState - Target power state for the device.

    NtStatus - NT status value associated with the event.

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ETW_USB_HC EtwUsbHc;
    ETW_USB_HUB EtwUsbHub;
    GUID activityId;
    NTSTATUS status;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    hubExt = FdoExt(HubFdo);

    // Get information about the USB host controller the hub is attached to
    UsbhEtwGetHcInfo(hubExt, &EtwUsbHc);

    // Get information about the hub 
    UsbhEtwGetHubInfo(hubExt, &EtwUsbHub);

    if (ActivityId) {
        activityId = *ActivityId;
        status = STATUS_SUCCESS;
    } else {
        status = UsbhEtwGetActivityId(Irp, &activityId);
    }

    // Write the info to ETW
    UsbhEtwWrite(
            EventDescriptor,
            NT_SUCCESS(status) ? &activityId : NULL,
            &EtwUsbHc,
            sizeof(EtwUsbHc),
            &EtwUsbHub,
            sizeof(EtwUsbHub),
            &PowerState,
            sizeof(PowerState),
            &NtStatus,
            sizeof(NtStatus),
            NULL);
}


VOID
UsbhEtwLogDeviceEvent(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in NTSTATUS NtStatus
    )
/*++

Routine Description:

    Logs information about an event for a USB device (PDO).

Arguments:

    pdoExt - Device extension for the USB device we are logging about

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    NtStatus - NT status value associated with the event.

Return Value:

    none
--*/
{
    UsbhEtwLogDeviceIrpEvent(pdoExt,
                             NULL,
                             EventDescriptor,
                             NtStatus);
}


VOID
UsbhEtwLogDeviceIrpEvent(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in_opt PIRP Irp,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in NTSTATUS NtStatus
    )
/*++

Routine Description:

    Logs information about an event for a USB device (PDO).

Arguments:

    pdoExt - Device extension for the USB device we are logging about

    Irp - IRP that can be used to retrieve an activity ID

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    NtStatus - NT status value associated with the event.

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ETW_USB_HC EtwUsbHc;
    ETW_USB_DEVICE EtwUsbDevice;
    NTSTATUS status;
    GUID activityId;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    hubExt = FdoExt(pdoExt->HubFdo);

    // Get information about the USB host controller the device is attached to
    UsbhEtwGetHcInfo(hubExt, &EtwUsbHc);

    // Get information about the USB device we are logging about
    UsbhEtwGetDeviceInfo(pdoExt, &EtwUsbDevice);

    status = UsbhEtwGetActivityId(Irp, &activityId);

    // Write the info to ETW
    UsbhEtwWrite(
        EventDescriptor,
        NT_SUCCESS(status) ? &activityId : NULL,
        &EtwUsbHc,
        sizeof(EtwUsbHc),
        &EtwUsbDevice,
        sizeof(EtwUsbDevice),
        &NtStatus,
        sizeof(NtStatus),
        NULL);
}

VOID
UsbhEtwLogDevicePowerEvent(
    __in PDEVICE_EXTENSION_PDO pdoExt,
    __in_opt PIRP Irp,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in POWER_STATE PowerState,
    __in NTSTATUS NtStatus
    )
/*++

Routine Description:

    Logs information about an event for a USB device (PDO).

Arguments:

    pdoExt - Device extension for the USB device we are logging about

    Irp - IRP that can be used to retrieve an activity ID

    EventDescriptor - Descriptor for this event as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    DevicePowerState - Target power state for the device.

    NtStatus - NT status value associated with the event.

Return Value:

    none
--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    ETW_USB_HC EtwUsbHc;
    ETW_USB_DEVICE EtwUsbDevice;
    NTSTATUS status;
    GUID activityId;

    if (HubG.EtwLoggingEnabled == 0) {
        return;
    }

    hubExt = FdoExt(pdoExt->HubFdo);

    // Get information about the USB host controller the device is attached to
    UsbhEtwGetHcInfo(hubExt, &EtwUsbHc);

    // Get information about the USB device we are logging about
    UsbhEtwGetDeviceInfo(pdoExt, &EtwUsbDevice);

    status = UsbhEtwGetActivityId(Irp, &activityId);

    // Write the info to ETW
    UsbhEtwWrite(
        EventDescriptor,
        NT_SUCCESS(status) ? &activityId : NULL,
        &EtwUsbHc,
        sizeof(EtwUsbHc),
        &EtwUsbDevice,
        sizeof(EtwUsbDevice),
        &PowerState,
        sizeof(PowerState),
        &NtStatus,
        sizeof(NtStatus),
        NULL);
}

NTSTATUS
UsbhEtwWrite(
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt LPCGUID ActivityId,
    ...
    )
/*++

Routine Description:

    Formats and writes an event to ETW.

Arguments:

    EventDescriptor - Event descriptor as defined in the ETW manifest (USBHUB.EVENTS.MAN)

    ActivityId - ETW Activity ID used for end-to-end tracing.

    ... - Null terminated list of paired data pointers (VOID*) and data sizes (ULONG) 
          to be formatted and logged to ETW.

Return Value:

    NT status
--*/
{
    EVENT_DATA_DESCRIPTOR   eventDataDescriptor[MAX_ETW_DESCRIPTORS] = {0};
    NTSTATUS                ntStatus;
    va_list                 DataList;
    VOID*                   DataPtr;
    ULONG                   DataSize = 0;
    ULONG                   DescCount=0;

    va_start(DataList, ActivityId);
    
    // Get the first data structure to be logged (can be NULL if no data to be logged with this event)
    DataPtr = va_arg(DataList, VOID*);

    // Loop through each set of data pointers and sizes and create an ETW data descriptor for each one

    if (DataPtr) {
        DataSize = va_arg(DataList, ULONG);
    }

    while ( (DataPtr) && (DataSize) && (DescCount < MAX_ETW_DESCRIPTORS) ) {

        EventDataDescCreate(&eventDataDescriptor[DescCount++],
                            DataPtr,
                            DataSize);
    
        DataPtr = va_arg(DataList, VOID*);

        if (DataPtr) {
            DataSize = va_arg(DataList, ULONG);
        }
    }

    va_end(DataList);

    // Now write the event to ETW
    ntStatus = EtwWrite(HubG.EtwRegHandle,
                        EventDescriptor,
                        ActivityId,
                        DescCount,
                        eventDataDescriptor);

    return ntStatus;
}
