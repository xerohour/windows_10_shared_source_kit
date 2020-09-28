/*++
Copyright (c) Microsoft Corporation

Module Name:

    driver.c

Abstract:

    USB Hub Driver

--*/
#include "pch.h"
#include "driver.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, HUBDRIVER_EtwRundownHub)
#pragma alloc_text (PAGE, HUBDRIVER_EtwRundownUsbDevice)
#pragma alloc_text (PAGE, HUBDRIVER_EtwEnableCallback)
#endif

//
// This object helps kd extension
//
PDRIVER_OBJECT g_Usbhub3DriverObject = NULL;

//
// Global WPP Triage info structure
//
WPP_TRIAGE_INFO g_Usbhub3_WppTriage_Info;

//
// USB Triage info structure
//
extern USBHUB3_TRIAGE_INFO g_Usbhub3_Triage_Info;


NTSTATUS
DriverEntry(
    __in
        PDRIVER_OBJECT         DriverObject,
    __in
        PUNICODE_STRING        RegistryPath
    )
/*++
Routine Description:

    Initialize the call backs structure of Driver Framework.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path,
                   to driver-specific key in the registry.

Return Value:

  NT Status Code

--*/
{
    WDF_DRIVER_CONFIG           config;
    NTSTATUS                    status;
    WDF_OBJECT_ATTRIBUTES       attributes;
    WDFDRIVER                   hubDriver;
    PHUB_GLOBALS                hubGlobals;
    UNICODE_STRING              functionName;
    RECORDER_CONFIGURE_PARAMS   recorderConfigureParams;
    RECORDER_LOG_CREATE_PARAMS  recorderLogCreateParams;
    NTSTATUS                    sleepStudyStatus;

    TRY {

        g_Usbhub3DriverObject = DriverObject;
        hubGlobals = NULL;

        //
        // Initialize global to make NonPagedPool be treated as NxPool on Win8 
        // and NonPagedPool on down-level
        //
        ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

        WPP_INIT_TRACING(DriverObject, RegistryPath);
        RECORDER_CONFIGURE_PARAMS_INIT(&recorderConfigureParams);
        recorderConfigureParams.CreateDefaultLog = FALSE;
        WppRecorderConfigure(&recorderConfigureParams);

        //
        // Initiialize driver config to control the attributes that
        // are global to the driver.
        //
        WDF_DRIVER_CONFIG_INIT(&config,
                               HUBFDO_EvtDeviceAdd);

        config.DriverPoolTag = USBHUB3_WDF_TAG;

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, HUB_GLOBALS);
        attributes.EvtCleanupCallback = DriverCleanup;

        //
        // Create a framework driver object to represent our driver.
        //
        status = WdfDriverCreate(DriverObject,
                                 RegistryPath,
                                 &attributes,
                                 &config,
                                 &hubDriver);

        if (!NT_SUCCESS(status)) {
            WPP_CLEANUP(DriverObject);
            LEAVE;
        }

        hubGlobals = GetHubGlobals(hubDriver);
        
        //
        // Create a recorder log for the hub driver.  
        //
        RECORDER_LOG_CREATE_PARAMS_INIT(&recorderLogCreateParams, NULL);  
        recorderLogCreateParams.TotalBufferSize = 1024;
        recorderLogCreateParams.ErrorPartitionSize = 512;

        RtlStringCchPrintfA(recorderLogCreateParams.LogIdentifier,
                            sizeof(recorderLogCreateParams.LogIdentifier),
                            "hub driver");

        status = WppRecorderLogCreate(&recorderLogCreateParams, &hubGlobals->IfrLog);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = hubDriver;

        status = WdfWaitLockCreate(&attributes,
                                   &hubGlobals->HubFdoListLock);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        InitializeListHead(&hubGlobals->HubFdoListHead);        

        status = WdfWaitLockCreate(&attributes,
                                   &hubGlobals->ConnectorMapLock);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        InitializeListHead(&hubGlobals->ConnectorMapHead);    

        //
        // Register with ETW
        //
        EventRegisterMicrosoft_Windows_USB_USBHUB3();
        sleepStudyStatus = UsbSleepStudy_Initialize();

        if (NT_SUCCESS(sleepStudyStatus)) {
            hubGlobals->SleepStudyEnabled = TRUE;
        } else {
            hubGlobals->SleepStudyEnabled = FALSE;
            DbgTraceTo(hubGlobals->IfrLog,
                TL_INFO,
                Driver,
                "UsbSleepStudy_Initialize Failed: %!STATUS!",
                sleepStudyStatus);
        }

        //
        // Telemetry opt-in for Manifested ETW Providers.
        //
        EnableManifestedProviderForMicrosoftTelemetry(Microsoft_Windows_USB_USBHUB3Handle);

        HUBREG_QueryGlobalHubValues(hubGlobals);
        HUBREG_QueryGlobalUsb20HardwareLpmSettings(hubGlobals);          
        HUBREG_QueryGlobalUsbLtmSettings(hubGlobals);

        //
        // Import kernel functions that are only supported in Windows 8 and later.
        //
        RtlInitUnicodeString(&functionName, L"KseQueryDeviceFlags");
        g_KseQueryDeviceFlags = (PFN_KSE_QUERY_DEVICE_FLAGS)MmGetSystemRoutineAddress(&functionName);
  
        RtlInitUnicodeString(&functionName, L"IoGetActivityIdIrp");
        g_IoGetActivityIdIrp = (PFN_IO_GET_ACTIVITY_ID_IRP)MmGetSystemRoutineAddress(&functionName);

        RtlInitUnicodeString(&functionName, L"IoSetActivityIdIrp");
        g_IoSetActivityIdIrp = (PFN_IO_SET_ACTIVITY_ID_IRP)MmGetSystemRoutineAddress(&functionName);

        //
        // Query the global KSE flags
        //
        HUBMISC_QueryKseGlobalFlags(hubGlobals);

        //
        // Update the global WPP Triage info structure
        //
        WppRecorderGetTriageInfo(&g_Usbhub3_WppTriage_Info);

        //
        // Update the global WDF Triage info structure
        //
        g_Usbhub3_Triage_Info.WdfTriageInfoPtr = (PWDF_TRIAGE_INFO) WdfGetTriageInfo();

        //
        // Update the USB Triage info with WPP fields
        //
        g_Usbhub3_Triage_Info.IfrLogHeaderSize = g_Usbhub3_WppTriage_Info.WppAutoLogHeaderSize;
        g_Usbhub3_Triage_Info.IfrLogSizeOffset = g_Usbhub3_WppTriage_Info.WppAutoLogHeaderSizeOffset;
        g_Usbhub3_Triage_Info.IfrSizeOfLogSizeField = g_Usbhub3_WppTriage_Info.WppSizeOfAutoLogHeaderSizeField;
        g_Usbhub3_Triage_Info.IfrDriverContextOffset = g_Usbhub3_WppTriage_Info.WppDriverContextOffset;
        g_Usbhub3_Triage_Info.IfrDriverContextSize = g_Usbhub3_WppTriage_Info.WppDriverContextSize;

    } FINALLY {

        if (NT_SUCCESS(status) && (hubGlobals != NULL)) {
            DbgTraceTo(hubGlobals->IfrLog,
                       TL_INFO,
                       Driver,
                       "HubGlobalFlags:0x%X",
                       hubGlobals->HubGlobalFlags.AsUlong32);
        }
    }

    return status;
}

VOID
DriverCleanup(
    __in
        WDFOBJECT              WdfDriver
    )
/*++

Routine Description:

    The driver's EvtCleanupCallback event callback function removes the driver's
    references on an object so that the object can be deleted.

Arguments:

    WdfDriver - A handle to a framework object.

Return Value:

    None

--*/
{
    PHUB_GLOBALS            hubGlobals;

    TRY {

#pragma prefast(suppress:28930, "Unused assignment of pointer warning in free builds due to pointer being referenced in NT_ASSERT macros")  
        hubGlobals = GetHubGlobals(WdfDriver);

        NT_ASSERT(IsListEmpty(&hubGlobals->HubFdoListHead));
        NT_ASSERT(IsListEmpty(&hubGlobals->ConnectorMapHead));
        
        WppRecorderLogDelete(hubGlobals->IfrLog);

        WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)WdfDriver));

        if (hubGlobals->SleepStudyEnabled) {
            UsbSleepStudy_Uninitialize();
        }

        //
        //  Unregister the driver as an ETW provider
        //
        EventUnregisterMicrosoft_Windows_USB_USBHUB3();

        g_Usbhub3DriverObject = NULL;

    } FINALLY {

    }

    return;
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBDRIVER_EtwRundownHub (
    __in
        LPCGUID                      ActivityId,
    __in
        PHUB_FDO_CONTEXT             HubFdoContext
    )
/*++

Routine Description:

    Logs information about a hub to ETW

Arguments:

Return Value:

    VOID

--*/
{
    PPORT_CONTEXT           portContext;
    PUCX_CONTROLLER_INFO    controllerInfo;

    PAGED_CODE ();

    TRY {
        switch (HubFdoContext->HsmContext.SubSmFlags) {
        case HsmSubSmFlagHub30:
            EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_USB30_HUB_INFORMATION(
                ActivityId,
                HubFdoContext->HubParentInterface.Hub,
                sizeof(USB_30_HUB_DESCRIPTOR),
                &HubFdoContext->HubDescriptor.Hub30Descriptor,
                HubFdoContext->CurrentWdfPowerDeviceState);
            break;

        case HsmSubSmFlagHub20:
            EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_USB20_HUB_INFORMATION(
                ActivityId,
                HubFdoContext->HubParentInterface.Hub,
                sizeof(USB_HUB_DESCRIPTOR),
                &HubFdoContext->HubDescriptor.Hub20Descriptor,
                HubFdoContext->CurrentWdfPowerDeviceState);
            break;
    
        case HsmSubSmFlagHubRoot:
            EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_ROOT_HUB_INFORMATION(
                ActivityId,
                HubFdoContext->HubParentInterface.Hub,
                HubFdoContext->CurrentWdfPowerDeviceState);
            break;
        }

        controllerInfo = &HubFdoContext->ControllerInfo;

        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &HubFdoContext->MuxContext.PSMListHead,
                        MuxContext.ListEntry,
                        portContext) {

            switch (portContext->PsmContext.StartIndex) {
            case StartIndexPsm20:
                EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_20_PORT_V1(
                    ActivityId,
                    HubFdoContext->HubParentInterface.Hub,
                    portContext->PortInformation.PortNumber,
                    portContext->PortInformation.PortFlags.PortFlagAcpiUpcValid ? 1 : 0,
                    (portContext->PortInformation.PortFlags.PortFlagAcpiUpcValid == 1) ?
                        portContext->AcpiUpcPackage.PortConnectorType :
                        0,
                    portContext->ConnectorId.UcmConnectorId,
                    controllerInfo->Type,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.BusNumber : 0,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.DeviceNumber : 0,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.FunctionNumber : 0,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.VendorId : LONG_MAX,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.DeviceId : LONG_MAX,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.RevisionId : 0,
                    (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.VendorId : NULL,
                    (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.DeviceId : NULL,
                    (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.RevisionId : NULL);

                break;

            case StartIndexPsm30:
                EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_30_PORT_V1(
                    ActivityId,
                    HubFdoContext->HubParentInterface.Hub,
                    portContext->PortInformation.PortNumber,
                    portContext->PortInformation.PortFlags.PortFlagAcpiUpcValid ? 1 : 0,
                    (portContext->PortInformation.PortFlags.PortFlagAcpiUpcValid == 1) ?
                        portContext->AcpiUpcPackage.PortConnectorType :
                        0,
                    portContext->ConnectorId.UcmConnectorId,
                    controllerInfo->Type,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.BusNumber : 0,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.DeviceNumber : 0,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.FunctionNumber : 0,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.VendorId : LONG_MAX,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.DeviceId : LONG_MAX,
                    (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.RevisionId : 0,
                    (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.VendorId : NULL,
                    (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.DeviceId : NULL,
                    (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.RevisionId : NULL);

                break;
            }

            if (portContext->PortInformation.PortFlags.PortFlagAcpiUpcValid == 1) {

                EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_ACPI_UPC(
                    ActivityId,
                    HubFdoContext->HubParentInterface.Hub,
                    portContext->PortInformation.PortNumber,
                    sizeof(ACPI_UPC_PACKAGE),
                    &portContext->AcpiUpcPackage);
            }

            if (portContext->PortInformation.PortFlags.PortFlagAcpiPldValid == 1) {

                EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_ACPI_PLD(
                    ActivityId,
                    HubFdoContext->HubParentInterface.Hub,
                    portContext->PortInformation.PortNumber,
                    sizeof(ACPI_PLD_BUFFER),
                    &portContext->AcpiPldPackage);
            }
        }
    } FINALLY {
    
    }
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBDRIVER_EtwRundownUsbDevice (
    __in
        LPCGUID                     ActivityId,
    __in
        WDFDEVICE                   UsbDevice
    )
/*++

Routine Description:

    Logs information about a USB device to ETW

Arguments:

Return Value:

    VOID

--*/
{
    PHUB_PDO_CONTEXT        pdoContext;
    WDF_OBJECT_ATTRIBUTES   objectAttributes;
    WDFMEMORY               deviceDescriptionMemory;
    PWSTR                   deviceDescription;
    NTSTATUS                status;
    WDFSTRING               symbolicLinkName;
    UNICODE_STRING          symbolicLinkNameUnicode;
    LPWSTR                  symbolicLinkNameString;
    WDFSTRING               pdoName;
    UNICODE_STRING          pdoNameUnicode;
    LPWSTR                  pdoNameString;
    PUSB_TOPOLOGY_ADDRESS   usbTopologyAddress;
    ULONG                   portPath[6];
    ULONG                   i;
    PUCX_CONTROLLER_INFO    controllerInfo;

    PAGED_CODE ();

    TRY {

        pdoContext = GetHubPdoContext(UsbDevice);

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = UsbDevice;

        deviceDescriptionMemory = NULL;
        status = WdfDeviceAllocAndQueryProperty(UsbDevice,
                                                DevicePropertyDeviceDescription,
                                                NonPagedPool,
                                                &objectAttributes,
                                                &deviceDescriptionMemory);

        if (NT_SUCCESS(status)) {
            deviceDescription = WdfMemoryGetBuffer(deviceDescriptionMemory, NULL);
        } else {
            deviceDescription = NULL;
        }

        RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);
    
        status = HUBMISC_GetDeviceInterfacePath(
                                pdoContext->AssociatedDSM->DeviceFlags.DeviceIsHub == 1 ?
                                 (LPGUID)&GUID_DEVINTERFACE_USB_HUB :
                                 (LPGUID)&GUID_DEVINTERFACE_USB_DEVICE,
                                UsbDevice,
                                &symbolicLinkName,
                                pdoContext->AssociatedDSM->PortContext->IfrLog);

        if (NT_SUCCESS(status)) {
        
            WdfStringGetUnicodeString(symbolicLinkName, &symbolicLinkNameUnicode);

            symbolicLinkNameString = ExAllocatePoolWithTag(NonPagedPool,
                                                           symbolicLinkNameUnicode.Length + sizeof(WCHAR),
                                                           USBHUB3_DEVICE_TAG);

            //
            // String needs to be NULL-terminated
            //
            if (symbolicLinkNameString != NULL) {
                RtlZeroMemory(symbolicLinkNameString,
                              symbolicLinkNameUnicode.Length + sizeof(WCHAR));

                RtlCopyMemory(symbolicLinkNameString, 
                              symbolicLinkNameUnicode.Buffer, 
                              symbolicLinkNameUnicode.Length);
            }
        } else {
            symbolicLinkNameString = NULL;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = UsbDevice;
        pdoName = NULL;
        pdoNameString = NULL;

        status = WdfStringCreate(NULL, &objectAttributes, &pdoName);
    
        if (NT_SUCCESS(status)) {
            status = WdfDeviceRetrieveDeviceName(UsbDevice, pdoName);

            if (NT_SUCCESS(status)) {
        
                WdfStringGetUnicodeString(pdoName, &pdoNameUnicode);

                pdoNameString = ExAllocatePoolWithTag(NonPagedPool,
                                                      pdoNameUnicode.Length + sizeof(WCHAR),
                                                      USBHUB3_DEVICE_TAG);

                //
                // String needs to be NULL-terminated
                //
                if (pdoNameString != NULL) {
                    RtlZeroMemory(pdoNameString,
                                  pdoNameUnicode.Length + sizeof(WCHAR));

                    RtlCopyMemory(pdoNameString, 
                                  pdoNameUnicode.Buffer, 
                                  pdoNameUnicode.Length);
                }
            }
        }

        usbTopologyAddress = &pdoContext->HubFdoContext->HubParentInterface.HubTopologyAddress;

        portPath[0] = usbTopologyAddress->RootHubPortNumber;
        for (i = 0; i < 5; i++) {
            portPath[i+1] = usbTopologyAddress->HubPortNumber[i];
        }

        if (pdoContext->HubFdoContext->HubParentInterface.HubDepth < 6) {
            portPath[pdoContext->HubFdoContext->HubParentInterface.HubDepth] = pdoContext->PortNumber;
        }

        controllerInfo = &pdoContext->HubFdoContext->ControllerInfo;

        EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_DEVICE_INFORMATION_V1(
                ActivityId,
                pdoContext->HubFdoContext->HubParentInterface.Hub,
                pdoContext->AssociatedDSM->UsbDevice,
                pdoContext->PortNumber,
                deviceDescription,
                symbolicLinkNameString,
                sizeof(USB_DEVICE_DESCRIPTOR),
                &pdoContext->AssociatedDSM->DeviceDescriptor,
                (pdoContext->AssociatedDSM->ConfigurationDescriptor != NULL) ?
                  pdoContext->AssociatedDSM->ConfigurationDescriptor->wTotalLength :
                  0,
                (PUCHAR)pdoContext->AssociatedDSM->ConfigurationDescriptor,
                pdoNameString,
                pdoContext->PdoFlags.InD0 ? 0 : 1,
                pdoContext->HubFdoContext->HubParentInterface.HubDepth+1,
                (unsigned int *)portPath,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.BusNumber : 0,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.DeviceNumber : 0,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.FunctionNumber : 0,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.VendorId : LONG_MAX,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.DeviceId : LONG_MAX,
                (controllerInfo->Type == UcxControllerParentBusTypePci) ? controllerInfo->Pci.RevisionId : 0,
                pdoContext->CurrentWdfPowerDeviceState,
                pdoContext->AssociatedDSM->Usb20LpmStatus,
                controllerInfo->Type,
                (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.VendorId : NULL,
                (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.DeviceId : NULL,
                (controllerInfo->Type == UcxControllerParentBusTypeAcpi) ? controllerInfo->Acpi.RevisionId : NULL,
                pdoContext->AssociatedDSM->PortContext->PortInformation.PortFlags.PortFlagAcpiUpcValid ? 1 : 0,
                (pdoContext->AssociatedDSM->PortContext->PortInformation.PortFlags.PortFlagAcpiUpcValid == 1) ?
                    pdoContext->AssociatedDSM->PortContext->AcpiUpcPackage.PortConnectorType : 
                    0,
                pdoContext->AssociatedDSM->PortContext->ConnectorId.UcmConnectorId);

        if (symbolicLinkNameString != NULL) {
            ExFreePoolWithTag(symbolicLinkNameString, USBHUB3_DEVICE_TAG);
        }

        if (symbolicLinkName != NULL) {
            WdfObjectDelete(symbolicLinkName);
        }

        if (pdoNameString != NULL) {
            ExFreePoolWithTag(pdoNameString, USBHUB3_DEVICE_TAG);
        }

        if (pdoName != NULL) {
            WdfObjectDelete(pdoName);
        }

        if (deviceDescriptionMemory != NULL) {
            WdfObjectDelete(deviceDescriptionMemory);
        }
        
    } FINALLY {

    }
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBDRIVER_EtwEnableCallback (
    __in
        LPCGUID                     SourceId,
    __in
        ULONG                       IsEnabled,
    __in
        UCHAR                       Level,
    __in
        ULONGLONG                   MatchAnyKeyword,
    __in
        ULONGLONG                   MatchAllKeyword,
    __in_opt
        PEVENT_FILTER_DESCRIPTOR    FilterData,
    __in_opt
        PVOID                       CallbackContext
    )
/*++

Routine Description:

    Callback routine that executes when ETW is enabled or disabled.

Arguments:

    SourceId - Supplies the GUID from the call to EnableTraceEx.

    IsEnabled - Supplies whether tracing is enabled or disabled.

    Level - Supplies the level enabled for the tracing.

    MatchAnyMask - Supplies composite data for match any mask.

    MatchAllMask - Supplies composite data for match all mask.

    FilterData - Supplies the pointer to the arbitrary data to EnableTraceEx.

    CallbackContext - Supplies the context provided to EtwRegister.

Return Value:

    VOID

--*/
{
    PHUB_GLOBALS            hubGlobals;
    PHUB_FDO_CONTEXT        hubFdoContext;
    WDFDEVICE               childDevice;
    GUID                    activityId;

    UNREFERENCED_PARAMETER(SourceId);
    UNREFERENCED_PARAMETER(Level);
    UNREFERENCED_PARAMETER(MatchAllKeyword);
    UNREFERENCED_PARAMETER(MatchAnyKeyword);
    UNREFERENCED_PARAMETER(FilterData);
    UNREFERENCED_PARAMETER(CallbackContext);

    PAGED_CODE ();

    TRY {
    
        hubGlobals = GetHubGlobals(WdfGetDriver());
    
        WdfWaitLockAcquire(hubGlobals->HubFdoListLock, NULL);
        
        if (IsEnabled == EVENT_CONTROL_CODE_ENABLE_PROVIDER) {

            INTERLOCKED_SET_BIT_FIELD(HUB_GLOBAL_FLAGS, &hubGlobals->HubGlobalFlags, EtwEnabled);
            
            RtlZeroMemory(&activityId, sizeof(GUID));
            
            //
            // Allocate Activity ID for rundown
            //
            EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID,
                                 &activityId);

            //
            // Start ETW rundown
            //
            EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_START(
                &activityId);
            
            //
            // Walk list of hub FDOs
            //
            FOR_ALL_IN_LIST(HUB_FDO_CONTEXT,
                            &hubGlobals->HubFdoListHead,
                            ListEntry,
                            hubFdoContext) {

                //
                // Log Hub and Port info to ETW
                //
                HUBDRIVER_EtwRundownHub(&activityId, hubFdoContext);

                //
                // Iterate all child devices of the hub
                //
                WdfFdoLockStaticChildListForIteration(hubFdoContext->HubFdoDevice);

                childDevice = WdfFdoRetrieveNextStaticChild(hubFdoContext->HubFdoDevice,
                                                            NULL, 
                                                            WdfRetrievePresentChildren);
        
                while (childDevice != NULL) {
            
                    //
                    // Log information about the device to ETW
                    //
                    HUBDRIVER_EtwRundownUsbDevice(&activityId, childDevice);

                    childDevice = WdfFdoRetrieveNextStaticChild(hubFdoContext->HubFdoDevice,
                                                                childDevice, 
                                                                WdfRetrievePresentChildren);
                }

                WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

            }
        
            EventWriteUSBHUB3_ETW_EVENT_RUNDOWN_COMPLETE(&activityId);
            
        } else if (IsEnabled == EVENT_CONTROL_CODE_DISABLE_PROVIDER) {
            
            //
            // ETW is now disabled
            //
            INTERLOCKED_CLEAR_BIT_FIELD(HUB_GLOBAL_FLAGS, &hubGlobals->HubGlobalFlags, EtwEnabled);
        }

        WdfWaitLockRelease(hubGlobals->HubFdoListLock);
   
    } FINALLY {
    }
}
