/*++

Copyright (c) 2004  Microsoft Corporation

Module Name:

    WmiLog.c

Abstract:

    This module contains Wmi loging support.

Author:

    Jsaon Kace (jkace)

Revision History:

    6/2004:  Initial version

--*/



#include "common.h"
#include <stdio.h>
#include <strsafe.h>

//
//Globals
//

#ifdef USBPORT_WPP
#include "wmilog.tmh"
#endif

#pragma alloc_text(PAGE, USBPORT_FunctionControl)
#define USBPORT_TRACING_GUID 0
#define USBPORT_TRACE_MOF_FILE L"UsbportWMI"
#define USBPORT_DEFAULT_WMI_TRACE_FLAGS 0x03

ULONG       UsbportWmiTraceEnable = 0;
TRACEHANDLE UsbportWmiLoggerHandle = 0;
ULONG       UsbportWmiTraceFlags = 0;
ULONG       UsbportWmiTraceLevel = 0;
ULONG FdoGuidCount = 3;

WMIGUIDREGINFO GuidList[] =
{
    {&GUID_USB_WMI_TRACING,             0, WMIREG_FLAG_TRACED_GUID |
                                           WMIREG_FLAG_TRACE_CONTROL_GUID},
    {&GUID_USB_WMI_NODE_INFO , 1, 0},

    {&GUID_USB_WMI_DEVICE_PERF_INFO , 1, 0}
};

#define USB_WMI_NODE_INFO_INDEX 1
#define USB_WMI_PERFORMANCE_INDEX 2

NTSTATUS
USBPORT_WmiDriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING UniRegistryPath,
    PUSBPORT_MINIPORT_DRIVER MiniportDriver
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
    PUNICODE_STRING registryPath = &MiniportDriver->RegistryPath;
    //
    // Need to ensure that the registry path is null-terminated.
    // Allocate pool to hold a null-terminated copy of the path.
    // Safe in paged pool since all registry routines execute at
    // PASSIVE_LEVEL.
    //
    registryPath->MaximumLength = UniRegistryPath->Length + sizeof(UNICODE_NULL);
    registryPath->Length = UniRegistryPath->Length;

    ALLOC_POOL_Z(registryPath->Buffer,
                 NonPagedPoolNx,
                 registryPath->MaximumLength);

    if (registryPath->Buffer) {

       // make a copy of the reg path
       RtlCopyMemory(registryPath->Buffer,
                     UniRegistryPath->Buffer,
                     UniRegistryPath->Length);
    } else {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //UsbportWmiInit = TRUE;

    return STATUS_SUCCESS;
}


VOID
USBPORT_WmiUnload(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING MiniportRegistryPath
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
    if (MiniportRegistryPath->Buffer) {
        UsbFreePool(MiniportRegistryPath->Buffer);
        MiniportRegistryPath->Buffer = NULL;
    }
}

VOID
USBPORT_WmiRegisterLog(
    IN  PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    This is a initialization function in which we call IoWMIRegistrationControl
    to register for WMI loging.

Arguments:
    FdoDeviceObject.

Return Value:
    None.

--*/
{
    NTSTATUS status;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    //
    // If we are already registered, do nothing
    //
    if(devExt->Fdo.UsbportWmiRegister){
        return;
    }

    status = USBPORT_InitializeWmilibContext(&devExt->Fdo.WmiLibContext);

    if (!NT_SUCCESS(status)){
        goto USBPORT_WmiRegisterLog_Done;
    }

    //
    // Register with WMI.
    //
    status = IoWMIRegistrationControl(FdoDeviceObject,
                                      WMIREG_ACTION_REGISTER);

USBPORT_WmiRegisterLog_Done:

    if (!NT_SUCCESS(status)){
        DbgTrace((UsbpDebugTrace, "USBPORT_WmiRegisterLog: Error status %x returned\n",
                         status));
    }
    else{
        devExt->Fdo.UsbportWmiRegister = TRUE;
    }
    return;
}

VOID
USBPORT_WmiUnRegisterLog(
    IN  PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    This is a unregistration function in which we call IoWMIRegistrationControl
    to unregister for WMI loging.

Arguments:
    FdoDeviceObject.

Return Value:
    None.

--*/
{
    NTSTATUS status;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    //
    // Register with WMI.
    //
    status = IoWMIRegistrationControl(FdoDeviceObject,
                                      WMIREG_ACTION_DEREGISTER);
    if (!NT_SUCCESS(status)){
        DbgTrace((UsbpDebugTrace, "USBPORT_WmiUnRegisterLog: Error status %x returned\n",
                         status));
    }
    else{
        devExt->Fdo.UsbportWmiRegister = FALSE;
    }
    return;
}

NTSTATUS
USBPORT_WmiLogEvent(
    IN UCHAR    EventType,
    IN GUID     LogGUID,
    IN PVOID    Data1,
    IN ULONG    Data2,
    IN PUCHAR   Format,
    IN ...
    )
/*++

Routine Description:

    This is the main wmi logging funcion. This function should be used
    throughtout the USBPORT driver where WMI logging is required.

Arguments:
    LogLevel,
    LogType,
    LogGUID,
    Format,
    ...

Return Value:
    NTSTATUS

--*/
{

    va_list             marker;
    WMI_LOG_DATA        Wmi_log_data ={0,0};
    EVENT_TRACE_HEADER  *Wnode;
    NTSTATUS            status = STATUS_UNSUCCESSFUL;
    USBPORT_WMI_DATA    wmiData;

    va_start(marker, Format);
    StringCchVPrintf(wmiData.Buffer,
                     sizeof(wmiData.Buffer),
                     Format,
                     marker);
    va_end(marker);

    if(UsbportWmiTraceEnable &&
       UsbportWmiLoggerHandle){

        wmiData.DataPtr = Data1;
        wmiData.Data = Data2;
        wmiData.EventType = EventType;
        wmiData.BufferLength = 0;

        Wmi_log_data.Data.DataPtr = (ULONG64)&wmiData;
        Wmi_log_data.Data.Length = sizeof(USBPORT_WMI_DATA);
        Wmi_log_data.Header.Size = sizeof(WMI_LOG_DATA);
        Wmi_log_data.Header.Flags = WNODE_FLAG_TRACED_GUID | WNODE_FLAG_USE_MOF_PTR;
        Wmi_log_data.Header.Class.Type = EventType;
        Wmi_log_data.Header.Guid = LogGUID;
        Wnode = &Wmi_log_data.Header;
        ((PWNODE_HEADER)Wnode)->HistoricalContext = UsbportWmiLoggerHandle;

        //
        // Call TraceLogger to  write this event
        //

        status = IoWMIWriteEvent((PVOID)&(Wmi_log_data.Header));

        if(status != STATUS_SUCCESS){

            DbgTrace((UsbpDebugTrace, "USBPORT_WmiLogEvent: IoWMIWriteEvent failed with status %x\n",
                     status));
        }
    }

    return status;
}


NTSTATUS
USBPORT_WmiLogData(
    IN UCHAR    EventType,
    IN GUID     LogGUID,
    IN PVOID    Data1,
    IN ULONG    Data2,
    IN PVOID    Buffer,
    IN ULONG    BufferLength
    )
/*++

Routine Description:

    This is the main wmi logging funcion. This function should be used
    throughtout the USBPORT driver where WMI logging is required.

Arguments:
    LogLevel,
    LogType,
    LogGUID,
    Format,
    ...

Return Value:
    NTSTATUS

--*/
{

    WMI_LOG_DATA        Wmi_log_data ={0,0};
    EVENT_TRACE_HEADER  *Wnode;
    NTSTATUS            status = STATUS_UNSUCCESSFUL;
    USBPORT_WMI_DATA    wmiData;

    if(BufferLength > sizeof(wmiData.Buffer)){
        return STATUS_BUFFER_TOO_SMALL;
    }

    if(UsbportWmiTraceEnable &&
       UsbportWmiLoggerHandle){

        wmiData.DataPtr = Data1;
        wmiData.Data = Data2;
        wmiData.EventType = EventType;
        wmiData.BufferLength = BufferLength;
        RtlCopyBytes(wmiData.Buffer, Buffer, BufferLength);

        Wmi_log_data.Data.DataPtr = (ULONG64)&wmiData;
        Wmi_log_data.Data.Length = sizeof(USBPORT_WMI_DATA);
        Wmi_log_data.Header.Size = sizeof(WMI_LOG_DATA);
        Wmi_log_data.Header.Flags = WNODE_FLAG_TRACED_GUID | WNODE_FLAG_USE_MOF_PTR;
        Wmi_log_data.Header.Class.Type = EventType;
        Wmi_log_data.Header.Guid = LogGUID;
        Wnode = &Wmi_log_data.Header;
        ((PWNODE_HEADER)Wnode)->HistoricalContext = UsbportWmiLoggerHandle;

        //
        // Call TraceLogger to  write this event
        //

        status = IoWMIWriteEvent((PVOID)&(Wmi_log_data.Header));

        if(status != STATUS_SUCCESS){

            DbgTrace((UsbpDebugTrace, "USBPORT_WmiLogData: IoWMIWriteEvent failed with status %x\n",
                     status));
        }
    }

    return status;
}

NTSTATUS
USBPORT_DispatchSystemControl(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    )
/*++

Routine Description:

    Dispatch routine for System Control IRPs (MajorFunction == IRP_MJ_SYSTEM_CONTROL)

Arguments:

    DeviceObject
    Irp

Return Value:

    NT status code

--*/
{
    PWMILIB_CONTEXT wmilibContext;
    NTSTATUS status;
    SYSCTL_IRP_DISPOSITION disposition;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    wmilibContext = &devExt->Fdo.WmiLibContext;

    //
    // Call Wmilib helper function to crack the irp. If this is a wmi irp
    // that is targetted for this device then WmiSystemControl will callback
    // at the appropriate callback routine.
    //
    status = WmiSystemControl(wmilibContext,
                              FdoDeviceObject,
                              Irp,
                              &disposition);

    switch(disposition) {
    case IrpProcessed:
        //
        // This irp has been processed and may be completed or pending.
        //
        USBPORT_Core_UnlinkAndFreeIoRequest(UsbIoRequest, Irp);

        break;

    case IrpNotCompleted:
        //
        // This irp has not been completed, but has been fully processed.
        // we will complete it now.
        USBPORT_Core_IoCompleteIrp(FdoDeviceObject,
                            Irp,
                            UsbIoRequest,
                            Irp->IoStatus.Status,
                            Irp->IoStatus.Information);
        break;

    case IrpForward:
    case IrpNotWmi:
    default:
        //
        // This irp is either not a WMI irp or is a WMI irp targetted
        // at a device lower in the stack.

        status = USBPORT_PassIrp(FdoDeviceObject, NULL, NULL, TRUE, TRUE, TRUE, Irp, UsbIoRequest);
        break;
    }

    return(status);
}

NTSTATUS
USBPORT_QueryWmiRegInfo(
    IN PDEVICE_OBJECT FdoDeviceObject,
    OUT ULONG *RegFlags,
    OUT PUNICODE_STRING InstanceName,
    OUT PUNICODE_STRING *RegistryPath,
    OUT PUNICODE_STRING MofResourceName,
    OUT PDEVICE_OBJECT *Pdo
    )
/*++

Routine Description:

    This routine is a callback into the driver to retrieve the list of
    guids or data blocks that the driver wants to register with WMI. This
    routine may not pend or block. Driver should NOT call
    WmiCompleteRequest.

Arguments:

    DeviceObject is the device whose registration info is being queried

    *RegFlags returns with a set of flags that describe the guids being
        registered for this device. If the device wants enable and disable
        collection callbacks before receiving queries for the registered
        guids then it should return the WMIREG_FLAG_EXPENSIVE flag. Also the
        returned flags may specify WMIREG_FLAG_INSTANCE_PDO in which case
        the instance name is determined from the PDO associated with the
        device object. Note that the PDO must have an associated devnode. If
        WMIREG_FLAG_INSTANCE_PDO is not set then Name must return a unique
        name for the device.

    InstanceName returns with the instance name for the guids if
        WMIREG_FLAG_INSTANCE_PDO is not set in the returned *RegFlags. The
        caller will call ExFreePool with the buffer returned.

    *RegistryPath returns with the registry path of the driver. The caller
         does NOT free this buffer.

    *MofResourceName returns with the name of the MOF resource attached to
        the binary file. If the driver does not have a mof resource attached
        then this can be returned as NULL. The caller does NOT free this
        buffer.

    *Pdo returns with the device object for the PDO associated with this
        device if the WMIREG_FLAG_INSTANCE_PDO flag is retured in
        *RegFlags.

Return Value:

    status

--*/
{
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    *RegistryPath = &devExt->Fdo.MiniportDriver->RegistryPath;
    *RegFlags = WMIREG_FLAG_INSTANCE_PDO;
    *Pdo = devExt->Fdo.PhysicalDeviceObject;
    //RtlInitUnicodeString(MofResourceName, USBPORT_TRACE_MOF_FILE);

    return(STATUS_SUCCESS);
}

NTSTATUS
USBPORT_InitializeWmilibContext(
    IN PWMILIB_CONTEXT WmilibContext
    )
/*++

Routine Description:

    This routine will initialize the wmilib context structure with the
    guid list and the pointers to the wmilib callback functions. This routine
    should be called before calling IoWmiRegistrationControl to register
    your device object.

Arguments:

    WmilibContext is pointer to the wmilib context.

Return Value:

    status

--*/
{
    RtlZeroMemory(WmilibContext, sizeof(WMILIB_CONTEXT));

    WmilibContext->GuidCount = FdoGuidCount;
    WmilibContext->GuidList = GuidList;
    WmilibContext->QueryWmiRegInfo   = USBPORT_QueryWmiRegInfo;
    WmilibContext->QueryWmiDataBlock = USBPORT_QueryWmiDataBlock;
    WmilibContext->SetWmiDataBlock   = NULL;
    WmilibContext->SetWmiDataItem    = NULL;
    WmilibContext->ExecuteWmiMethod  = NULL;
    WmilibContext->WmiFunctionControl = USBPORT_FunctionControl;

    return(STATUS_SUCCESS);
}

NTSTATUS
USBPORT_FunctionControl(
  IN PDEVICE_OBJECT FdoDeviceObject,
  IN PIRP Irp,
  IN ULONG GuidIndex,
  IN WMIENABLEDISABLECONTROL Function,
  IN BOOLEAN Enable
  )
/*++

  Routine Description:

      This routine is a callback into the driver to enabled or disable event
      generation or data block collection. A device should only expect a
      single enable when the first event or data consumer enables events or
      data collection and a single disable when the last event or data
      consumer disables events or data collection. Data blocks will only
      receive collection enable/disable if they were registered as requiring
      it. If the driver can complete enabling/disabling within the callback it
      should call WmiCompleteRequest to complete the irp before returning to
      the caller. Or the driver can return STATUS_PENDING if the irp cannot be
      completed immediately and must then call WmiCompleteRequest once the
      data is changed.

  Arguments:

      DeviceObject is the device object

      GuidIndex is the index into the list of guids provided when the
          device registered

      Function specifies which functionality is being enabled or disabled
      (currently either WmiEventControl or WmiDataBlockControl)

      Enable is TRUE then the function is being enabled else disabled

  Return Value:

      status

--*/
{

    NTSTATUS            status = STATUS_SUCCESS;
    PIO_STACK_LOCATION  irpStack;
    PWNODE_HEADER       Wnode;

    PAGED_CODE();

    if (Function) {
        //
        // We only support WmiEventControl
        //
      return STATUS_UNSUCCESSFUL;
    }

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    Wnode = (PWNODE_HEADER)irpStack->Parameters.WMI.Buffer;
    NT_ASSERT(irpStack->Parameters.WMI.BufferSize >= sizeof(WNODE_HEADER));


    //
    // Enable/Disable events
    //

    switch (GuidIndex) {
    case USBPORT_TRACING_GUID:

        //
        // NOTE: for the TraceLog event, level is passed in as the HIWORD(Wnode->Version)
        // from tracelog.exe -level
        //
        //

        if (Enable) {

          InterlockedExchange(&UsbportWmiTraceEnable, 1);
          UsbportWmiLoggerHandle = ((PWNODE_HEADER)irpStack->Parameters.WMI.Buffer)->HistoricalContext;
          UsbportWmiTraceLevel = (ULONG) WmiGetLoggerEnableLevel(UsbportWmiLoggerHandle);
          UsbportWmiTraceFlags = (ULONG) WmiGetLoggerEnableFlags(UsbportWmiLoggerHandle);

          if(UsbportWmiTraceLevel == 0){
              UsbportWmiTraceLevel = 1;
          }
          if(UsbportWmiTraceFlags == 0){
              UsbportWmiTraceFlags = USBPORT_DEFAULT_WMI_TRACE_FLAGS;
          }

        } else {

          InterlockedExchange(&UsbportWmiTraceEnable, 0);
          UsbportWmiLoggerHandle = 0;
          UsbportWmiTraceFlags = 0;
          UsbportWmiTraceLevel = 0;

        }
        break;

    default:
        status = STATUS_WMI_GUID_NOT_FOUND;
        break;
    }

    status = WmiCompleteRequest(FdoDeviceObject,
                                Irp,
                                status,
                                0,
                                IO_NO_INCREMENT);
    return status;
}

NTSTATUS
USBPORT_QueryWmiDataBlock(
    PDEVICE_OBJECT FdoDeviceObject,
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

    switch (GuidIndex) {
    case USB_WMI_NODE_INFO_INDEX:

        nts = USBPORT_GetControllerNodeInfo(FdoDeviceObject,
                                            Buffer,
                                            OutBufferSize,
                                            &size);
        break;

    case USB_WMI_PERFORMANCE_INDEX:

        nts = USBPORT_GetControllerPerformanceInfo(FdoDeviceObject,
                                                   Buffer,
                                                   OutBufferSize,
                                                   &size);
        break;

    default:
        nts = STATUS_WMI_GUID_NOT_FOUND;
    }

    nts = WmiCompleteRequest(FdoDeviceObject,
                             Irp,
                             nts,
                             size,
                             IO_NO_INCREMENT);

    return nts;
}

NTSTATUS
USBPORT_GetControllerNodeInfo(PDEVICE_OBJECT FdoDeviceObject,
                              __in_bcount(BufferLength) PVOID Buffer,
                              ULONG BufferLength,
                              PULONG LengthOfDataReturned)
{
    ULONG length, stringSize;
    PUSB_DEVICE_NODE_INFO usbNodeInfo = NULL;
    PDEVICE_EXTENSION devExt = NULL;
    ROOTHUB_DATA hubData;
    RH_HUB_CHARATERISTICS rhChars;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    length = sizeof(USB_DEVICE_NODE_INFO);

    if(length > BufferLength){
        *LengthOfDataReturned = length;
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Fill in the node header information
    //

    RtlZeroBytes(Buffer, BufferLength);
    usbNodeInfo = (PUSB_DEVICE_NODE_INFO)Buffer;

    usbNodeInfo->Sig = USB_NODE_INFO_SIG;
    usbNodeInfo->LengthInBytes = length;

    stringSize = sizeof(usbNodeInfo->DeviceDescription);
    if(stringSize > sizeof(devExt->Fdo.Name)){
        stringSize = sizeof(devExt->Fdo.Name);
    }

    RtlCopyBytes(usbNodeInfo->DeviceDescription,
                 devExt->Fdo.Name,
                 stringSize);

    usbNodeInfo->NodeType = UsbController;

    RtlZeroMemory(&usbNodeInfo->BusAddress,
                  sizeof(usbNodeInfo->BusAddress));

    usbNodeInfo->BusAddress.PciBusNumber = devExt->Fdo.BusNumber;
    usbNodeInfo->BusAddress.PciDeviceNumber = devExt->Fdo.BusDevice;
    usbNodeInfo->BusAddress.PciFunctionNumber = devExt->Fdo.BusFunction;


    //
    // Fill in the controller information
    //

    MPRH_GetRootHubData(devExt, &hubData);

    usbNodeInfo->ControllerDeviceInfo.NumberOfRootPorts =
             hubData.NumberOfPorts;

    rhChars.us = hubData.HubCharacteristics.us;

    if (rhChars.PowerSwitchType == USBPORT_RH_POWER_SWITCH_PORT) {
            usbNodeInfo->ControllerDeviceInfo.HcFeatureFlags |=
                USB_HC_FEATURE_FLAG_PORT_POWER_SWITCHING;
    }

    switch(devExt->Fdo.SSP_Support) {
    case SSPsupport_RegEnabled:
        usbNodeInfo->ControllerDeviceInfo.HcFeatureFlags |= USB_HC_FEATURE_FLAG_SEL_SUSPEND;
        break;
    case SSPsupport_RegDisabled:
    case SSPsupport_GlobalDisabled:
    case SSPsupport_HcDisabled:
    case SSPsupport_ErrorDisabled:
        break;
    default:
        USBPORT_ASSERTMSG("Unexpected devExt->Fdo.SSP_Support",FALSE);
    }

    if (devExt->Fdo.Flags.LegacyBIOS) {
        usbNodeInfo->ControllerDeviceInfo.HcFeatureFlags |=
            USB_HC_FEATURE_LEGACY_BIOS;
    }

    usbNodeInfo->ControllerDeviceInfo.PciVendorId =
        devExt->Fdo.PciVendorId;

    usbNodeInfo->ControllerDeviceInfo.PciDeviceId =
        devExt->Fdo.PciDeviceId;

    usbNodeInfo->ControllerDeviceInfo.PciRevision =
        (UCHAR) devExt->Fdo.PciRevisionId;

    *LengthOfDataReturned = length;

    return STATUS_SUCCESS;

}

NTSTATUS
USBPORT_GetControllerPerformanceInfo(PDEVICE_OBJECT FdoDeviceObject,
                                     __in_bcount(BufferLength) PVOID Buffer,
                                     ULONG BufferLength,
                                     PULONG LengthOfDataReturned)
{
    ULONG length;
    PUSB_DEVICE_PERFORMANCE_INFO perfInfo = NULL;
    PDEVICE_EXTENSION devExt = NULL;
    USB_BANDWIDTH_INFO bandInfo;
    USBUSER_REQUEST_HEADER header;
    ULONG resultLength = 0;
    WCHAR buffer[256];
    NTSTATUS ntStatus = STATUS_SUCCESS;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);

    length = sizeof(USB_DEVICE_PERFORMANCE_INFO);

    if(length > BufferLength){
        *LengthOfDataReturned = length;
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Fill in the node header information
    //

    RtlZeroBytes(Buffer, BufferLength);
    perfInfo = (PUSB_DEVICE_PERFORMANCE_INFO)Buffer;

    perfInfo->BulkBytes  =
        devExt->Fdo.PerformanceData.StatBulkDataBytes;
    perfInfo->IsoBytes =
        devExt->Fdo.PerformanceData.StatIsoDataBytes;
    perfInfo->InterruptBytes =
        devExt->Fdo.PerformanceData.StatInterruptDataBytes;
    perfInfo->ControlDataBytes =
        devExt->Fdo.PerformanceData.StatControlDataBytes;

    perfInfo->PciInterruptCount =  devExt->Fdo.StatPciInterruptCount;

    //
    // Get the bandwidth info
    //

    header.RequestBufferLength = sizeof(USBUSER_REQUEST_HEADER) + sizeof(USB_BANDWIDTH_INFO);
    header.UsbUserRequest = USBUSER_GET_BANDWIDTH_INFORMATION;

    USBPORT_UserGetBandwidthInformation(FdoDeviceObject,
                                        &header,
                                        &bandInfo);

    NT_ASSERT(header.ActualBufferLength == sizeof(USBUSER_REQUEST_HEADER) + sizeof(USB_BANDWIDTH_INFO));


    perfInfo->AllocedInterrupt[0] = bandInfo.AllocedInterrupt_1ms;
    perfInfo->AllocedInterrupt[1] = bandInfo.AllocedInterrupt_2ms;
    perfInfo->AllocedInterrupt[2] = bandInfo.AllocedInterrupt_4ms;
    perfInfo->AllocedInterrupt[3] = bandInfo.AllocedInterrupt_8ms;
    perfInfo->AllocedInterrupt[4] = bandInfo.AllocedInterrupt_16ms;
    perfInfo->AllocedInterrupt[5] = bandInfo.AllocedInterrupt_32ms;

    perfInfo->AllocedIso = bandInfo.AllocedIso;

    //
    // return allocation based on bits/32 msec
    // segment of bus time.  Data is always stored
    // in bits/frame(ms)
    //

    perfInfo->Total32secBandwidth = devExt->Fdo.TotalBusBandwidth * 32;

    perfInfo->TransferErrors = devExt->Fdo.PerformanceData.TransferErrors;
    perfInfo->DroppedIsoPackets = devExt->Fdo.PerformanceData.DroppedIsoPackets;
    perfInfo->TotalIsoLatency = devExt->Fdo.PerformanceData.TotalIsoLatency;

    perfInfo->BulkUrbCount = devExt->Fdo.PerformanceData.StatNumberBulkUrbs;
    perfInfo->IsoUrbCount = devExt->Fdo.PerformanceData.StatNumberIsoUrbs;
    perfInfo->InterruptUrbCount = devExt->Fdo.PerformanceData.StatNumberInterruptUrbs;
    perfInfo->ControlUrbCount = devExt->Fdo.PerformanceData.StatNumberControlUrbs;

    //
    // BUGBUG:  Fill in values here!!
    //

    perfInfo->HcIdleState = 1;
    perfInfo->HcAsyncIdleState = 2;
    perfInfo->HcAsyncCacheFlushCount = 3;
    perfInfo->HcPeriodicIdleState = 4;
    perfInfo->HcPeriodicCacheFlushCount = 5;

    //
    // Get the controller name.  We will only use the first 40 bytes returned.
    //

    ntStatus = IoGetDeviceProperty(devExt->Fdo.PhysicalDeviceObject,
                                   DevicePropertyDeviceDescription,
                                   sizeof(buffer),
                                   buffer,
                                   &resultLength);

    if(NT_SUCCESS(ntStatus)){

        if(resultLength > sizeof(perfInfo->DeviceDescription)){
            resultLength = sizeof(perfInfo->DeviceDescription);
        }

        RtlCopyBytes(perfInfo->DeviceDescription,
                     buffer,
                     resultLength);

        //
        // Make sure the buffer is NULL terminated
        //

        perfInfo->DeviceDescription[ (sizeof(perfInfo->DeviceDescription)/sizeof(perfInfo->DeviceDescription[0])) - 1 ] =
            UNICODE_NULL;
    }

    *LengthOfDataReturned = length;

    return STATUS_SUCCESS;

}
