/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubfdo.c

Abstract:

    USB Hub FDO

--*/

#include "pch.h"
#include "hubfdo.tmh"


#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, HUBFDO_EvtDeviceAdd)
#pragma alloc_text (PAGE, HUBFDO_EvtDevicePrepareHardware)
#pragma alloc_text (PAGE, HUBFDO_EvtDeviceReleaseHardware)
#pragma alloc_text (PAGE, HUBFDO_EvtDeviceSurpriseRemoval)
#pragma alloc_text (PAGE, HUBFDO_LogHubResetRecoveryWasInvoked)
#pragma alloc_text (PAGE, HUBFDO_IoctlValidateParameters)
#pragma alloc_text (PAGE, HUBFDO_EvtIoDeviceControl)
#pragma alloc_text (PAGE, HUBFDO_IoctlResetHub)
#pragma alloc_text (PAGE, HUBFDO_IoctlCyclePort)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetNodeConnectionInformationExV2)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetPortConnectorProperties)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetHubInformationEx)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetDescriptorFromNodeConnection)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetNodeConnectionName)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetHubCapsEx)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetHubCaps)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetNodeConnectionAttributes)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetNodeConnectionDriverKeyName)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetNodeInformation)
#pragma alloc_text (PAGE, HUBFDO_IoctlGetPortStatus)
#pragma alloc_text (PAGE, HUBFDO_SetFriendlyNameForBlockedHub)
#pragma alloc_text (PAGE, HUBFDO_QueryHubErrataFlags)
#endif

const CHAR UserModeFdoRequest[]            = "User Mode FDO Request";

extern WPP_TRIAGE_INFO g_Usbhub3_WppTriage_Info;
extern PDRIVER_OBJECT g_Usbhub3DriverObject;

// Initialize the USBHUB3_TRIAGE_INFO structure used by
// DBGENG.DLL

USBHUB3_TRIAGE_INFO g_Usbhub3_Triage_Info = {
    USBHUB3_TRIAGE_INFO_VERSION_1,                           // Version

    // Hub FDO Context
    FIELD_OFFSET(HUB_FDO_CONTEXT, ConfigurationDescriptor),      // ConfigurationDescriptorOffset
    FIELD_OFFSET(HUB_FDO_CONTEXT, HubControlRequest.WdfRequest), // HubControlRequestWdfRequestOffset
    FIELD_OFFSET(CONTROL_REQUEST, Irp),                          // ControlRequestIrpOffset

    FIELD_OFFSET(HUB_FDO_CONTEXT, HubFdoDevice),                             // HubFdoDeviceOffset
    FIELD_OFFSET(HUB_FDO_CONTEXT, HubControlRequest.WdfRequest),             // ControlWdfRequestOffset
    FIELD_OFFSET(HUB_FDO_CONTEXT, StatusChangeInterruptRequest.WdfRequest),  // StatusChangeInterruptWdfRequestOffset
    sizeof(HUB_FDO_CONTEXT),                                 // HubContextSize
    sizeof(USB_CONFIGURATION_DESCRIPTOR),                    // ConfigurationDescriptorSize
    sizeof(CONTROL_REQUEST),                                 // HubControlRequestSize


    //  Port Info
    FIELD_OFFSET(HUB_FDO_CONTEXT, HubInformation.RootHub30PortInfoPointerArray),// RootHub30PortInfoArrayOffset
    FIELD_OFFSET(HUB_FDO_CONTEXT, HubInformation.NumberOf30Ports),              // RootHub30PortInfoCountOffset
    sizeof(((HUB_INFORMATION *) 0)->NumberOf30Ports),                           // RootHub30PortInfoCountSize
    sizeof(ROOTHUB_30PORT_INFO_EX),                                             // RootHub30PortInfoSize
    FIELD_OFFSET(HUB_FDO_CONTEXT, HubInformation.RootHub20PortInfoPointerArray),// RootHub20PortInfoArrayOffset
    FIELD_OFFSET(HUB_FDO_CONTEXT, HubInformation.NumberOf20Ports),              // RootHub20PortInfoCountOffset
    sizeof(((HUB_INFORMATION *) 0)->NumberOf20Ports),                           // RootHub20PortInfoCountSize
    sizeof(ROOTHUB_20PORT_INFO), // RootHub20PortInfoSize

    // Port Context
    FIELD_OFFSET(HUB_FDO_CONTEXT, MuxContext.PSMListHead), // PSMListHeadOffset
    sizeof(PORT_CONTEXT),                                  // PortContextSize
    FIELD_OFFSET(PORT_CONTEXT, MuxContext),                // PortContextOffset
    FIELD_OFFSET(PORT_CONTEXT, IfrLog),                    // PortContextIfrLogOffset


    // Device Context
    FIELD_OFFSET(HUB_FDO_CONTEXT, MuxContext.DSMListHead),     // DSMListHeadOffset
    FIELD_OFFSET(DEVICE_CONTEXT, MuxContext),                  // DeviceContextOffset
    FIELD_OFFSET(DEVICE_CONTEXT, PdoContext),                  // DevicePdoContextOffset
    FIELD_OFFSET(DEVICE_CONTEXT, CurrentDeviceConfiguration),  // DeviceCurrentConfigurationOffset
    FIELD_OFFSET(DEVICE_CONTEXT, OldDeviceConfiguration),      // DeviceOldDeviceConfigurationOffset
    FIELD_OFFSET(DEVICE_CONTEXT, NextInterfaceToSet),          // DeviceNextInterfaceToSet

    FIELD_OFFSET(DEVICE_CONTEXT, OldInterface),                // DeviceOldInterfaceOffset
    FIELD_OFFSET(DEVICE_CONTEXT, NewInterface),                // DeviceNewInterfaceOffset
    FIELD_OFFSET(DEVICE_CONTEXT, EndpointsToBeEnabled),        // DeviceEndPointsToBeEnabledOffset
    FIELD_OFFSET(DEVICE_CONTEXT, EndpointsToBeDisabled),       // DeviceEndPointsToBeDisabledOffset
    FIELD_OFFSET(DEVICE_CONTEXT, EndpointsToRemainUnchanged),  // DeviceEndPointsToRemainUnchangedOffset
    FIELD_OFFSET(DEVICE_CONTEXT, UcxEndpointArrayCount),       // DeviceUcxEndPointArrayCountOffset

    FIELD_OFFSET(DEVICE_CONTEXT, ControlRequest),              // DeviceControlRequestOffset
    FIELD_OFFSET(DEVICE_CONTEXT, UcxRequest),                  // DeviceWdfUcxRequestOffset
    FIELD_OFFSET(DEVICE_CONTEXT, UcxRequestIrp),               // DeviceUcxRequestIrpOffset
    FIELD_OFFSET(DEVICE_CONTEXT, UcxIoctlRequestMemory),       // DeviceWdfUcxIoctlRequestMemoryOffset
    FIELD_OFFSET(DEVICE_CONTEXT, CurrentClientRequest),        // DeviceWdfCurrentClientRequestOffset
    FIELD_OFFSET(DEVICE_CONTEXT, PdoDeviceInit),               // DeviceWdfDeviceInitOffset

    FIELD_OFFSET(DEVICE_CONTEXT, CurrentTargetPipe),           // DeviceCurrentTargetPipeOffset
    FIELD_OFFSET(DEVICE_CONTEXT, DeviceDescriptor),            // DeviceDescriptorOffset
    FIELD_OFFSET(DEVICE_CONTEXT, CompatIdInterfaceDescriptor), // DeviceCompatIdInterfaceDescriptorOffset
    FIELD_OFFSET(DEVICE_CONTEXT, ConfigurationDescriptor),     // DeviceConfigurationDescriptorOffset
    FIELD_OFFSET(DEVICE_CONTEXT, LanguageIdDescriptor),        // DeviceLanguageIdDescriptorOffset
    FIELD_OFFSET(DEVICE_CONTEXT, ProductIdStringDescriptor),   // DeviceProductIdStringDescriptorOffset

    FIELD_OFFSET(DEVICE_CONTEXT, DeviceQualifierDescriptor),   // DeviceDeviceQualifierDescriptorOffset
    FIELD_OFFSET(DEVICE_CONTEXT, BOSDescriptor),               // DeviceBOSDescriptorOffset
    FIELD_OFFSET(DEVICE_CONTEXT, MsExtConfigDesc),             // DeviceMsExtConfigDescOffset
    FIELD_OFFSET(DEVICE_CONTEXT, MsExtPropertyDesc),           // DeviceMsExtPropertyDescOffset

    FIELD_OFFSET(HUB_PDO_CONTEXT, AssociatedDSM),              // DevicePdoAssociatedContextOffset
    FIELD_OFFSET(HUB_PDO_CONTEXT, IdleIrpContext.IdleIrp),     // DevicePdoIdleIrpOffset
    FIELD_OFFSET(HUB_PDO_CONTEXT, WakeNotification),           // DevicePdoWakeNotificationOffset

    // Common Types
    sizeof(DEVICE_CONTEXT),                 // DeviceContextSize
    sizeof(HUB_PDO_CONTEXT),                // DevicePdoContextSize
    sizeof(((DEVICE_CONTEXT *)0)->UcxEndpointArrayCount), // DeviceEndPointSizeOfCountFieldOffset
    sizeof(CONTROL_REQUEST),                // DeviceControlRequestSize
    sizeof(IRP),                            // IrpSize
    sizeof(PIPE_HANDLE),                    // PipeHandleSize

    sizeof(USB_INTERFACE_DESCRIPTOR),       // UsbInterfaceDescriptorSize
    sizeof(USB_STRING_DESCRIPTOR),          // UsbStringDescriptorSize
    sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR),// UsbDeviceQualifierDescriptorSize
    sizeof(USB_BOS_DESCRIPTOR),             // BOSDescriptorSize
    sizeof(MS_EXT_CONFIG_DESC),             // MsExtConfigDescSize
    sizeof(MS_EXT_PROP_DESC),               // MsExtPropDescSize
    sizeof(DEVICE_CONFIGURATION),           // DeviceConfigurationSize
    sizeof(USB_ENDPOINT_DESCRIPTOR),        // UsbEndpointDescriptorSize
    sizeof(USB_CONFIGURATION_DESCRIPTOR),   // DeviceUsbConfigurationDescriptorSize

    // Device Configuration
    FIELD_OFFSET(DEVICE_CONFIGURATION, InterfaceList),           // ConInterfaceListOffset
    FIELD_OFFSET(DEVICE_CONFIGURATION, ConfigurationDescriptor), // ConDeviceConfigurationDescriptorOffset
    sizeof(INTERFACE_HANDLE),                                    // InterfaceHandleSize
    FIELD_OFFSET(DEVICE_CONFIGURATION, InterfaceList),           // DeviceConfigurationInterfaceListOffset

    // Inteface Handle
    FIELD_OFFSET(INTERFACE_HANDLE, NumberOfEndpoints),   // IfHNumOfEndpointsOffset
    FIELD_OFFSET(INTERFACE_HANDLE, PipeHandle),          // IfHPipeHandleOffset
    sizeof(((INTERFACE_HANDLE *) 0)->NumberOfEndpoints), // IfHNumOfEndpointsSize
    FIELD_OFFSET(INTERFACE_HANDLE, InterfaceLink),       // IfHInterfaceLinkOffset
    FIELD_OFFSET(INTERFACE_HANDLE, InterfaceDescriptor), // IfHInterfaceDescriptorOffset

    // Pipe Handle
    FIELD_OFFSET(PIPE_HANDLE, EndpointDescriptor),       // PiHEndpointDescriptorOffset
    FIELD_OFFSET(PIPE_HANDLE, UcxEndpoint),              // PipeHandleUcxEndpointOffset

    // Some of Ifr Log fields are initialized in DriverEntry
    FIELD_OFFSET(HUB_FDO_CONTEXT, IfrLog),  // IfrLogHeaderOffset
    0,  // IfrLogHeaderSize
    0,  // IfrLogSizeOffset
    0,  // IfrSizeOfLogSizeField
    0,  // IfrDriverContextOffset
    0,  // IfrDriverContextSize

    // Driver and Device objects
    (PVOID) &g_Usbhub3DriverObject,     // GlobalDriverObjectPtr
    (PVOID) &WPP_GLOBAL_Control,        // WppGlobalControlPtr
    sizeof(*WPP_GLOBAL_Control) // WppGlobalControlSize
};


FORCEINLINE
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBFDO_AddEventAndWait(
    __in
        PHUB_FDO_CONTEXT   HubFdoContext,
    __in
        HSM_EVENT          Event
    )
/*++

Routine Description:

    Adds a PnP/Power event to HSM and waits for the PnP/Power Event to
    be signaled indicating the event has completed.

Returns:

    NTSTATUS

--*/
{
    NTSTATUS                status;

    TRY {

        NT_ASSERT(HubFdoContext->HubInformation.HubFlags.Initialized == 1);


        KeResetEvent(&HubFdoContext->PnPPowerEvent);

        HUBSM_AddHsmEvent(HubFdoContext,
                          Event);

        HUBMISC_WaitForSignal(&HubFdoContext->PnPPowerEvent,
                              "Hub FDO PnpCallback",
                              WdfObjectContextGetObject(HubFdoContext));

        status = HubFdoContext->PnPPowerStatus;

    } FINALLY {

    }

    return status;
}


NTSTATUS
HUBFDO_GetDeviceCapabilities(
    __in
        WDFDEVICE               Device,
    __out
        PDEVICE_CAPABILITIES    Capabilities,
    __in
        RECORDER_LOG            IfrLog
    )
/*++

Routine Description:

    Function retrieves the DEVICE_CAPABILITIES descriptor from the device

Arguments:

    Device - handle to device

    Capabilities - device capabilities

Return Value:

    NT status code

--*/
{
    WDFREQUEST                  request;
    WDFIOTARGET                 target;
    WDF_REQUEST_SEND_OPTIONS    options;
    WDF_REQUEST_REUSE_PARAMS    reuse;
    IO_STACK_LOCATION           stack;
    NTSTATUS                    status;

    TRY {

        request = NULL;
        target = WdfDeviceGetIoTarget(Device);

        status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES,
                                  target,
                                  &request);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // All pnp IRPs must be initialized with STATUS_NOT_SUPPORTED
        //
        WDF_REQUEST_REUSE_PARAMS_INIT(&reuse,
                                      WDF_REQUEST_REUSE_NO_FLAGS,
                                      STATUS_NOT_SUPPORTED);
        WdfRequestReuse(request, &reuse);

        //
        // Initialize device capabilities according to the DDK docs
        //
        RtlZeroMemory(Capabilities, sizeof(DEVICE_CAPABILITIES));
        Capabilities->Size = sizeof(DEVICE_CAPABILITIES);
        Capabilities->Version  =  1;
        Capabilities->Address  = (ULONG) -1;
        Capabilities->UINumber = (ULONG) -1;

        RtlZeroMemory(&stack, sizeof(stack));
        stack.MajorFunction = IRP_MJ_PNP;
        stack.MinorFunction = IRP_MN_QUERY_CAPABILITIES;
        stack.Parameters.DeviceCapabilities.Capabilities = Capabilities;

        WdfRequestWdmFormatUsingStackLocation(request,
                                              &stack);

        WDF_REQUEST_SEND_OPTIONS_INIT(&options,
                                      WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);

        if (WdfRequestSend(request, target, &options) == FALSE) {
            DbgTraceTo(IfrLog, TL_ERROR, Device, "Failed to send IRP_MN_QUERY_CAPABILITIES");
        }

        status = WdfRequestGetStatus(request);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

        if (request != NULL) {
            WdfObjectDelete(request);
        }

    }

    return status;

}

#define SUPERSPEED_HUB_NON_FUNCTIONAL L"SuperSpeed USB Hub (Non Functional)"

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
HUBFDO_SetFriendlyNameForBlockedHub (
    __in
        PHUB_FDO_CONTEXT                    HubFdoContext
    )
/*++

Routine Description:

    This function sets the friendly name for a super speed hub
    that has been blocked because it is known to have problems.
    All ports of such a hub are disabled so that the downstream
    devices will enuemrate on the companion 2.0 ports

--*/
{
    NTSTATUS    status;

    PAGED_CODE();

    TRY {

        status = IoSetDevicePropertyData(WdfDeviceWdmGetPhysicalDevice(WdfObjectContextGetObject(HubFdoContext)),
                                         &DEVPKEY_Device_DeviceDesc,
                                         LOCALE_NEUTRAL,
                                         PLUGPLAY_PROPERTY_PERSISTENT,
                                         DEVPROP_TYPE_STRING,
                                         sizeof(SUPERSPEED_HUB_NON_FUNCTIONAL),
                                         SUPERSPEED_HUB_NON_FUNCTIONAL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_WARNING,
                       Hub,
                       "IoSetDevicePropertyData Failed %!STATUS!",
                       status);
        }

    } FINALLY {

    }

    return;
}


NTSTATUS
HUBFDO_EvtDeviceShutdownPreprocess (
    WDFDEVICE   Device,
    PIRP        Irp
    )
/*++

Routine Description:

    This routine handles IRP_MJ_SHUTDOWN for the hub FDO.

Return Value:

    NTSTATUS

--*/
{
    //
    // Cleanup UxD registry settings for the hub if needed
    //
    HUBREG_UxdShutdown(GetHubFdoContext(Device));

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp,IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}


NTSTATUS
HUBFDO_EvtDeviceAdd(
    WDFDRIVER                   Driver,
    PWDFDEVICE_INIT             DeviceInit
    )
/*++
Routine Description:

    Hub_EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES                   fdoAttributes;
    NTSTATUS                                status;
    NTSTATUS                                statusIO;
    WDFDEVICE                               device;
    PNP_BUS_INFORMATION                     busInfo;
    WDF_PNPPOWER_EVENT_CALLBACKS            pnpPowerCallbacks;
    PHUB_FDO_CONTEXT                        hubFdoContext;
    WDF_IO_QUEUE_CONFIG                     queueConfig;
    WDF_OBJECT_ATTRIBUTES                   queueAttributes;
    WDF_DEVICE_PNP_CAPABILITIES             pnpCapabilities;
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS   idleSettings;
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS   wakeSettings;
    WDF_POWER_POLICY_EVENT_CALLBACKS        powerPolicyCallbacks;
    RECORDER_LOG_CREATE_PARAMS              recorderLogCreateParams;
    USBD_HANDLE                             usbdHandle;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE ();

    TRY {

        EventWriteUSBHUB3_ETW_EVENT_HUB_ADD_DEVICE_START(0);

        hubFdoContext = NULL;

        //
        // Initialize attributes structure to specify size and accessor function
        // for storing device context.
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&fdoAttributes, HUB_FDO_CONTEXT);
        fdoAttributes.EvtCleanupCallback = HUBFDO_EvtDeviceCleanupCallback;
        fdoAttributes.EvtDestroyCallback = HUBFDO_EvtDeviceDestroyCallback;

        //
        // Set our power/pnp event handlers
        //
        WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);

        pnpPowerCallbacks.EvtDeviceD0Entry = HUBFDO_EvtDeviceD0Entry;
        pnpPowerCallbacks.EvtDeviceD0Exit = HUBFDO_EvtDeviceD0Exit;
        pnpPowerCallbacks.EvtDevicePrepareHardware = HUBFDO_EvtDevicePrepareHardware;
        pnpPowerCallbacks.EvtDeviceReleaseHardware = HUBFDO_EvtDeviceReleaseHardware;
        pnpPowerCallbacks.EvtDeviceSurpriseRemoval = HUBFDO_EvtDeviceSurpriseRemoval;
        pnpPowerCallbacks.EvtDeviceUsageNotification = HUBFDO_EvtDeviceUsageNotification;
        WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

        //
        // Assign a preprocess callback for shutdown notification
        //
        status = WdfDeviceInitAssignWdmIrpPreprocessCallback(DeviceInit,
                                                             HUBFDO_EvtDeviceShutdownPreprocess,
                                                             IRP_MJ_SHUTDOWN,
                                                             NULL,
                                                             0);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfDeviceInitAssignWdmIrpPreprocessCallback Failed %!STATUS!",
                       status);
            LEAVE;
        }

        WDF_POWER_POLICY_EVENT_CALLBACKS_INIT(&powerPolicyCallbacks);
        powerPolicyCallbacks.EvtDeviceArmWakeFromS0 = HUBFDO_EvtDeviceArmWakeFromS0;
        powerPolicyCallbacks.EvtDeviceDisarmWakeFromS0 = HUBFDO_EvtDeviceDisarmWakeFromS0;
        powerPolicyCallbacks.EvtDeviceArmWakeFromSx = HUBFDO_EvtDeviceArmWakeFromSx;
        powerPolicyCallbacks.EvtDeviceDisarmWakeFromSx = HUBFDO_EvtDeviceDisarmWakeFromSx;

        WdfDeviceInitSetPowerPolicyEventCallbacks(DeviceInit, &powerPolicyCallbacks);

        //
        // If D0Entry fails, child devnodes should get ReleaseHardware callbacks before
        // this devnode.
        //
        WdfDeviceInitSetReleaseHardwareOrderOnFailure(DeviceInit,
                                                      WdfReleaseHardwareOrderOnFailureAfterDescendants);

        //
        // Create a framework device object. In response to this call, framework
        // creates a WDM deviceobject and attach to the PDO.
        //
        status = WdfDeviceCreate(&DeviceInit,
                                 &fdoAttributes,
                                 &device);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                       TL_ERROR,
                       Driver,
                       "WdfDeviceCreate Failed %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // Register for shutdown notification
        //
        statusIO = IoRegisterShutdownNotification(WdfDeviceWdmGetDeviceObject(device));
        if (!NT_SUCCESS(statusIO)) {
            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                       TL_ERROR,
                       Driver,
                       "HUBFDO_EvtDeviceAdd: IoRegisterShutdownNotification Failed %!STATUS!",
                       statusIO);
        }

        hubFdoContext = GetHubFdoContext(device);
        hubFdoContext->HubFdoDevice = device;
        hubFdoContext->HubPdo = WdfDeviceWdmGetPhysicalDevice(device);

        //
        // Link to global triage structure
        //

        hubFdoContext->Usbhub3TriageInfo = &g_Usbhub3_Triage_Info;

        hubFdoContext->HubInformation.HubNumber = USBD_AllocateHubNumber();

        RECORDER_LOG_CREATE_PARAMS_INIT_APPEND_POINTER(&recorderLogCreateParams,
                                                       NULL,
                                                       hubFdoContext);

        recorderLogCreateParams.TotalBufferSize = 1024;
        recorderLogCreateParams.ErrorPartitionSize = 512;

        RtlStringCchPrintfA(recorderLogCreateParams.LogIdentifier,
                            sizeof(recorderLogCreateParams.LogIdentifier),
                            "hub%d",
                            hubFdoContext->HubInformation.HubNumber);

        status = WppRecorderLogCreate(&recorderLogCreateParams, &hubFdoContext->IfrLog);

        if (!NT_SUCCESS(status)) {
            hubFdoContext->IfrLog = GetHubGlobals(WdfGetDriver())->IfrLog;
        }

        //
        // Indicate special file support
        //
        WdfDeviceSetSpecialFileSupport(device,
                                       WdfSpecialFilePaging,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(device,
                                       WdfSpecialFileHibernation,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(device,
                                       WdfSpecialFileDump,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(device,
                                       WdfSpecialFileBoot,
                                       TRUE);


        //
        // Retrieve the power and pnp capabilities from the parent
        //
        status = HUBFDO_GetDeviceCapabilities(device,
                                              &hubFdoContext->DeviceCapabilities,
                                              hubFdoContext->IfrLog);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                       TL_ERROR,
                       Driver,
                       "HUBFDO_EvtDeviceAdd: HUBFDO_GetDeviceCapabilities Failed %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // Set up the PNP capabilities to flag that the hub can be safely surprise
        // removed.  Note that by not modifying any of the other fields, WDF will
        // use the values returned from the lower drivers in the stack.
        //
        WDF_DEVICE_PNP_CAPABILITIES_INIT(&pnpCapabilities);
        pnpCapabilities.SurpriseRemovalOK = WdfTrue;
        WdfDeviceSetPnpCapabilities(device,
                                    &pnpCapabilities);


        //
        // Retrieve the Hub/Parent bus interface
        //
        //
        // Fill in the FDO information, so that our KD extension can get to
        // the FDO from the PDO
        //
        hubFdoContext->HubParentInterface.HubFdoContext = hubFdoContext;

        status = WdfFdoQueryForInterface(device,
                                         &GUID_HUB_PARENT_INTERFACE,
                                         (PINTERFACE)&hubFdoContext->HubParentInterface,
                                         sizeof(HUB_PARENT_INTERFACE),
                                         HUB_PARENT_INTERFACE_VERSION_LATEST,
                                         NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Query for Hub Parent Interface Failed %!STATUS!", status);
            LEAVE;
        }

        if (hubFdoContext->HubParentInterface.HubDepth == 0) {

            //
            // For the root hub, local target is the final IO Target
            //
            hubFdoContext->HubInfoFromParent.IoTarget =
                WdfIoTargetWdmGetTargetDeviceObject(WdfDeviceGetIoTarget(device));

            hubFdoContext->RootHubIoTarget = WdfDeviceGetIoTarget(device);

            //
            // Max port power for the root hub is high power
            //
            hubFdoContext->HubInformation.MaxPortPower = MaxPortPower500Milliamps;
        }


        status = USBD_CreateHandle(WdfDeviceWdmGetDeviceObject(device),
                                   WdfIoTargetWdmGetTargetDeviceObject(WdfDeviceGetIoTarget(device)),
                                   USBD_CLIENT_CONTRACT_VERSION_602,
                                   USBHUB3_HUB_TAG,
                                   &usbdHandle);
        if(NT_SUCCESS(status)) {

            status = USBD_QueryUsbCapability(usbdHandle,
                                             (GUID*)&GUID_USB_CAPABILITY_HIGH_BANDWIDTH_ISOCH,
                                             sizeof(hubFdoContext->HubInformation.SuperSpeedPlusIsochBurstCount),
                                             (PUCHAR)&hubFdoContext->HubInformation.SuperSpeedPlusIsochBurstCount,
                                             NULL);

            if (!NT_SUCCESS(status)) {
                hubFdoContext->HubInformation.SuperSpeedPlusIsochBurstCount = 0;
            }

            status = USBD_QueryUsbCapability(usbdHandle,
                                             (GUID*)&GUID_USB_CAPABILITY_SELECTIVE_SUSPEND,
                                             0,
                                             NULL,
                                             NULL);
            USBD_CloseHandle(usbdHandle);
        }


        if (!NT_SUCCESS(status)) {
            INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, SelectiveSuspendNotSupportedByParentStack);

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Selective Suspend Not Suported by parent stack: %!STATUS!", status);
            status = STATUS_SUCCESS;
        }


        if ((hubFdoContext->HubInformation.HubFlags.SelectiveSuspendNotSupportedByParentStack == 0) &&
            hubFdoContext->HubParentInterface.IsHubWakeCapable){

            //
            // Set up the hub for selective suspend
            //
            WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&idleSettings,
                                                       IdleCanWakeFromS0);

            //
            // Timeout is a power policy setting but the default value
            // is set to 50 ms
            //
            idleSettings.IdleTimeout = DEFAULT_HUB_IDLE_TIMEOUT;

            hubFdoContext->CurrentIdleTimeout = DEFAULT_HUB_IDLE_TIMEOUT;

            //
            // Enable by default, but let the user override it.
            //
            idleSettings.UserControlOfIdleSettings = IdleAllowUserControl;
            idleSettings.Enabled = WdfUseDefault;
            idleSettings.DxState = PowerDeviceD2;

            if (hubFdoContext->HubParentInterface.HubDepth == 0) {

                //
                // UCX currently assumes that the root hub is in D0 before a reset
                // recovery is attempted when requested by the UCX client. In some
                // cases such as when there is no wait wake IRP pending and when
                // the controller/root hub devnodes are being surprise removed, this
                // is not possible and results in a three-way deadlock among the hub
                // driver, UCX and the UCX client (USBXHCI).
                //
                // In order to address this deadlock, root hub FDO will be marked as
                // a PoFx device (system managed) as the power framework guarantees
                // that the root hub is in D0 when its surprise remove is processed.
                //
                // As a result of this change, there might be an additional delay in
                // idle entry that is subject to the current system clock resolution
                // as KMDF uses two timers (100ns timer + idle timeout hint timer)
                // instead of one. This might result in the idle entry being delayed
                // by upto 15.6ms, but given that there might be a compat implication
                // to lowering the idle timeout value below 50ms in order to compensate
                // for this delay, this additional delay is assumed to be tolerable.
                //
                idleSettings.IdleTimeoutType = SystemManagedIdleTimeoutWithHint;
            }

            status = WdfDeviceAssignS0IdleSettings(device,
                                                   &idleSettings);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(hubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "WdfDeviceAssignS0IdleSettings Failed %!STATUS!",
                           status);

                //
                // For now, allow super speed hubs to enumerate even if they
                // don't support remote wake and as a result don't support
                // selective suspend
                //
                if ((status == STATUS_POWER_STATE_INVALID) &&
                    (hubFdoContext->HubParentInterface.HubSpeed == UsbSuperSpeed)) {

                    DbgTraceTo(hubFdoContext->IfrLog,
                               TL_WARNING,
                               Hub,
                               "HW_COMPLIANCE: SuperSpeed hub will not support selective suspend");

                    USB_HW_VERIFIER_HUB_BREAK(hubFdoContext, HubHwVerifierNoSelectiveSuspendSupport);

                    status = STATUS_SUCCESS;

                    EventWriteUSBHUB3_ETW_EVENT_HUB_SELECTIVE_SUSPEND_NOT_SUPPORTED(
                        &hubFdoContext->CurrentActivityId,
                        hubFdoContext->HubParentInterface.Hub,
                        (UINT32)STATUS_POWER_STATE_INVALID);

                }

            } else {
                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, S0IdleConfigured);

                status = PoRegisterPowerSettingCallback(WdfDeviceWdmGetDeviceObject(device),
                                                        &GUID_POWER_HUB_SELECTIVE_SUSPEND_TIMEOUT,
                                                        HUBFDO_PowerSettingCallback,
                                                        hubFdoContext,
                                                        &hubFdoContext->SelectiveSuspendTimeoutPowerSetting);
                if (!NT_SUCCESS(status)) {

                    DbgTraceTo(hubFdoContext->IfrLog,
                               TL_ERROR,
                               Hub,
                               "PoRegisterPowerSettingCallback Failed %!STATUS!",
                               status);

                    //
                    // Ignore the error
                    //
                    status = STATUS_SUCCESS;
                    hubFdoContext->SelectiveSuspendTimeoutPowerSetting = NULL;

                }


            }
        }

        //
        // To begin with, fdo is logically in a D3Final state
        //
        hubFdoContext->CurrentWdfPowerDeviceState = WdfPowerDeviceD3Final;


        //
        // For system sleep, set up hub so that they propogate wait
        // wake if child is armed for wake but does not allow user
        // to arm the hub itself for wake.
        //
        WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(&wakeSettings);
        wakeSettings.ArmForWakeIfChildrenAreArmedForWake = TRUE;
        wakeSettings.Enabled = WdfFalse;
        wakeSettings.IndicateChildWakeOnParentWake = TRUE;
        wakeSettings.UserControlOfWakeSettings = WakeDoNotAllowUserControl;
        status = WdfDeviceAssignSxWakeSettings(device,
                                               &wakeSettings);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "WdfDeviceAssignSxWakeSettings Failed %!STATUS!",
                       status);

            //
            // This is not a fatal error
            //
            status = STATUS_SUCCESS;
        }

        hubFdoContext->HubControllerStackInterface.Hub = hubFdoContext->HubParentInterface.Hub;
        hubFdoContext->HubControllerStackInterface.HubContext = (USB_HUB_CONTEXT)hubFdoContext;
        hubFdoContext->HubControllerStackInterface.ClearTTBuffer = HUBHTX_ClearTTBuffer;
        hubFdoContext->HubControllerStackInterface.NoPingResponse = HUBPDO_NoPingResponse;

        status = WdfFdoQueryForInterface(device,
                                         &GUID_HUB_CONTROLLERSTACK_INTERFACE,
                                         (PINTERFACE)&hubFdoContext->HubControllerStackInterface,
                                         sizeof(HUB_CONTROLLERSTACK_INTERFACE),
                                         HUB_CONTROLLERSTACK_INTERFACE_VERSION_1_0,
                                         NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Query for Hub Controllerstack Interface Failed %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // We fill in the handle only for the root hub. For all other hubs, the parent
        // will fill it in.
        //
        if (hubFdoContext->HubParentInterface.HubDepth == 0) {

            status = WdfFdoQueryForInterface(device,
                                             &USB_BUS_INTERFACE_USBDI_GUID,
                                             (PINTERFACE)&hubFdoContext->UsbdBusInterface,
                                             sizeof(struct _USB_BUS_INTERFACE_USBDI_V3),
                                             USB_BUSIF_USBDI_VERSION_3,
                                             hubFdoContext->HubParentInterface.Hub);
        } else{

            status = WdfFdoQueryForInterface(device,
                                             &USB_BUS_INTERFACE_USBDI_GUID,
                                             (PINTERFACE)&hubFdoContext->UsbdBusInterface,
                                             sizeof(struct _USB_BUS_INTERFACE_USBDI_V3),
                                             USB_BUSIF_USBDI_VERSION_3,
                                             NULL);
        }

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Query for Usbd Bus Interface Failed %!STATUS!", status);
            LEAVE;
        }


        //
        // Create the default queue to handle user-mode IOCTLs as well as
        // Get Port Status request forwarded from the PDO
        //
        WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig,
                                               WdfIoQueueDispatchSequential);
        queueConfig.EvtIoDeviceControl = HUBFDO_EvtIoDeviceControl;
        queueConfig.EvtIoInternalDeviceControl = HUBFDO_EvtIoInternalDeviceControlFromPDO;

        //
        // The user-mode I/O queue is not power managed because most user-mode IOCTLs return
        // cached information that doesn't require the hub to be in D0.  Requests that require
        // the hub to be in D0 (such as get descriptor or get port status requests) are handled by
        // the hub or device state machine, which will fail the request if the hub or device is not
        // in D0.
        //
        queueConfig.PowerManaged = WdfFalse;

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes,
                                                USER_MODE_IOCTL_QUEUE_CONTEXT);

        queueAttributes.ExecutionLevel = WdfExecutionLevelPassive;

        status = WdfIoQueueCreate(device,
                                  &queueConfig,
                                  &queueAttributes,
                                  &hubFdoContext->DeviceControlQueue);

        if (!NT_SUCCESS(status)) {
            NT_ASSERT(FALSE);

            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Creation of the Device Control Queue failed %!STATUS!",
                       status);

            LEAVE;
        }

        //
        // This value is used in responding to the IRP_MN_QUERY_BUS_INFORMATION
        // for the child devices. This is an optional information provided to
        // uniquely idenitfy the bus the device is connected.
        //
        busInfo.BusTypeGuid = GUID_BUS_TYPE_USB;
        busInfo.LegacyBusType = PNPBus;
        busInfo.BusNumber = 0;

        WdfDeviceSetBusInformationForChildren(device, &busInfo);

        HSMMUX_InitializeHSMMuxContext(hubFdoContext);

        status = HUBMISC_InitializeHsm(hubFdoContext);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, Initialized);

        status = HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDODeviceAdd);

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_HUB_ADD_DEVICE_COMPLETE(
            0,
            (hubFdoContext != NULL) ?
              hubFdoContext->HubParentInterface.Hub :
              NULL,
            status);

        if (!NT_SUCCESS(status)) {

            if ((hubFdoContext != NULL) &&
                (hubFdoContext->HubInformation.HubNumber != 0)) {

                    USBD_ReleaseHubNumber(hubFdoContext->HubInformation.HubNumber);
                    hubFdoContext->HubInformation.HubNumber = 0;
            }

            DbgTraceTo(GetHubGlobals(WdfGetDriver())->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Add Device Failed! %!STATUS!",
                       status);
        }
    }

    return status;
}


NTSTATUS
HUBFDO_EvtDevicePrepareHardware(
    WDFDEVICE               Device,
    WDFCMRESLIST            ResourcesRaw,
    WDFCMRESLIST            ResourcesTranslated
    )
/*++
Routine Description:

    HUBFDO_EvtDevicePrepareHardware is where we can first talk to the device.
    This is where we will retrieve the descriptors, configure the hub, etc.

Arguments:

    Device - the WDFDEVICE representing our FDO

    ResourcesRaw -

    ResourcesTranslated -

Return Value:

    NTSTATUS

--*/
{

    NTSTATUS                status;
    PHUB_FDO_CONTEXT        hubFdoContext;
    PPORT_CONTEXT           portContext;
    UNICODE_STRING          symbolicLinkNameUnicode;

    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    PAGED_CODE ();

    TRY {

        hubFdoContext = GetHubFdoContext(Device);

        EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_START(
            0,
            hubFdoContext->HubParentInterface.Hub);

        HUBREG_QueryValuesInHubHardwareKey(hubFdoContext);

        status = HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDOPrepareHardware);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (hubFdoContext->HubInformation.HubFlags.DisableSuperSpeed == 1) {
            HUBFDO_SetFriendlyNameForBlockedHub(hubFdoContext);
        }

        RtlInitUnicodeString(&symbolicLinkNameUnicode, NULL);

        HUBPARENT_GetHubSymbolicLinkName(hubFdoContext,
                                         &symbolicLinkNameUnicode);

        if (symbolicLinkNameUnicode.Length == 0) {
            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "NULL Hub Symbolic Link Name!");
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        hubFdoContext->HubSymbolicLinkNameUnicode.Buffer =
            (PWCH)ExAllocatePoolWithTag(NonPagedPool,
                                        symbolicLinkNameUnicode.Length * sizeof(WCHAR),
                                        USBHUB3_HUB_TAG);

        if (hubFdoContext->HubSymbolicLinkNameUnicode.Buffer == NULL) {
            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to allocate memory for symbolic link name");
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        hubFdoContext->HubSymbolicLinkNameUnicode.Length = 0;
        hubFdoContext->HubSymbolicLinkNameUnicode.MaximumLength = symbolicLinkNameUnicode.Length;

        RtlCopyUnicodeString(&hubFdoContext->HubSymbolicLinkNameUnicode,
                             &symbolicLinkNameUnicode);

        HUBACPI_GetAcpiPortAttributes(hubFdoContext);

        HUBCONNECTOR_MapHubPorts(hubFdoContext);

        WMI_RegisterHub(Device);

        //
        // Lets see if the parent controller is incapable of supporting streams on superspeed
        // devices
        //

        if (hubFdoContext->HubControllerStackInterface.AreSuperspeedStreamsSupported(
                                                hubFdoContext->HubParentInterface.Hub)) {
            INTERLOCKED_SET_BIT_FIELD(CONTROLLER_FLAGS, &hubFdoContext->ControllerFlags, SuperSpeedStreamsSupported);
        }

    } FINALLY {
        PHUB_GLOBALS            hubGlobals;
        PUCX_CONTROLLER_INFO    controllerInfo;
        hubGlobals = GetHubGlobals(WdfGetDriver());
        //
        // Lock our global list of hub FDOs
        //
        WdfWaitLockAcquire(hubGlobals->HubFdoListLock, NULL);

        if ((hubGlobals->HubGlobalFlags.EtwEnabled == 1) &&
            (NT_SUCCESS(status))) {

            //
            // Log info about the new hub to ETW
            //
            switch (hubFdoContext->HsmContext.SubSmFlags) {
            case HsmSubSmFlagHub30:
                EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_USB30_HUB_INFORMATION(
                    0,
                    hubFdoContext->HubParentInterface.Hub,
                    sizeof(USB_30_HUB_DESCRIPTOR),
                    &hubFdoContext->HubDescriptor.Hub30Descriptor,
                    hubFdoContext->CurrentWdfPowerDeviceState);
                break;

            case HsmSubSmFlagHub20:
                EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_USB20_HUB_INFORMATION(
                    0,
                    hubFdoContext->HubParentInterface.Hub,
                    sizeof(USB_HUB_DESCRIPTOR),
                    &hubFdoContext->HubDescriptor.Hub20Descriptor,
                    hubFdoContext->CurrentWdfPowerDeviceState);
                break;

            case HsmSubSmFlagHubRoot:
                EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_ROOT_HUB_INFORMATION(
                    0,
                    hubFdoContext->HubParentInterface.Hub,
                    hubFdoContext->CurrentWdfPowerDeviceState);
                break;

            default:
                EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_UNKNOWN_HUB_TYPE(
                    0,
                    hubFdoContext->HubParentInterface.Hub);
                break;
            }

            controllerInfo = &hubFdoContext->ControllerInfo;

            //
            // Enumerate all ports on the hub and log to ETW
            //
            FOR_ALL_IN_LIST(PORT_CONTEXT,
                            &hubFdoContext->MuxContext.PSMListHead,
                            MuxContext.ListEntry,
                            portContext) {

                switch (portContext->PsmContext.StartIndex) {
                case StartIndexPsm20:
                    EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_20_PORT_V1(
                        0,
                        hubFdoContext->HubParentInterface.Hub,
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
                    EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_30_PORT_V1(
                        0,
                        hubFdoContext->HubParentInterface.Hub,
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

                //
                // Log ACPI _UPC object if present
                //
                if (portContext->PortInformation.PortFlags.PortFlagAcpiUpcValid == 1) {

                    EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_ACPI_UPC(
                        0,
                        hubFdoContext->HubParentInterface.Hub,
                        portContext->PortInformation.PortNumber,
                        sizeof(ACPI_UPC_PACKAGE),
                        &portContext->AcpiUpcPackage);
                }

                //
                // Log ACPI _PLD object if present
                //
                if (portContext->PortInformation.PortFlags.PortFlagAcpiPldValid == 1) {

                    EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_ACPI_PLD(
                        0,
                        hubFdoContext->HubParentInterface.Hub,
                        portContext->PortInformation.PortNumber,
                        sizeof(ACPI_PLD_BUFFER),
                        &portContext->AcpiPldPackage);
                }
            }
        }

        //
        // Add the new hub to the global list
        //
        InsertTailList(&hubGlobals->HubFdoListHead, &hubFdoContext->ListEntry);

        WdfWaitLockRelease(hubGlobals->HubFdoListLock);

        EventWriteUSBHUB3_ETW_EVENT_HUB_PREPARE_HARDWARE_COMPLETE(
            0,
            hubFdoContext->HubParentInterface.Hub,
            status);

        DbgTraceTo(hubFdoContext->IfrLog,
                   TL_ERROR,
                   Hub,
                   "HubFlags:0x%X",
                   hubFdoContext->HubInformation.HubFlags.AsUlong32);
    }

    return status;
}


NTSTATUS
HUBFDO_EvtDeviceReleaseHardware(
    WDFDEVICE               Device,
    WDFCMRESLIST            ResourcesTranslated
    )
/*++
Routine Description:

    HUBFDO_EvtDeviceReleaseHardware is called when we are stopping or getting removed.
    Here we undo whatever we did in PrepareHardware and prepare to receive another
    PrepareHardware

Arguments:

    Device - The WDFDEVICE representing our FDO

    ResourcesTranslated -

Return Value:

    NTSTATUS

--*/
{

    NTSTATUS                        status;
    PHUB_FDO_CONTEXT                hubFdoContext;
    PHUB_GLOBALS                    hubGlobals;
    CONTROLLER_STOP_IDLE_CONTEXT    stopIdleContext;

    UNREFERENCED_PARAMETER(ResourcesTranslated);

    PAGED_CODE ();

    TRY {

        hubFdoContext = GetHubFdoContext(Device);

        INTERLOCKED_CLEAR_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, CanAcquirePowerReference);

        EventWriteUSBHUB3_ETW_EVENT_HUB_RELEASE_HARDWARE_START(
            0,
            hubFdoContext->HubParentInterface.Hub);

        HUBCONNECTOR_UnMapHubPorts(hubFdoContext);
        //
        // When a USB tree attached to the root hub gets
        // detached while in suspended state, it is possible
        // that the controller goes into S0Idle by the time
        // PnP processes the missing event and sends Suprise
        // Remove. When the surprise remove finally arrives,
        // hub will cleanup endpoints and devices from the
        // controller. Therefore we need to keep the controller
        // in D0, while the ReleaseHardware is being processed
        // by the HSM (and in turn by all DSMs)
        //
        CONTROLLER_STOP_IDLE_CONTEXT_INIT(&stopIdleContext);

        HUBUCX_AcquirePowerReferenceOnController(hubFdoContext,
                                                 &stopIdleContext);

        status = HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDOReleaseHardware);

        HUBUCX_ReleasePowerReferenceOnController(hubFdoContext,
                                                 &stopIdleContext);

        hubGlobals = GetHubGlobals(WdfGetDriver());

        WdfWaitLockAcquire(hubGlobals->HubFdoListLock, NULL);
        RemoveEntryList(&hubFdoContext->ListEntry);
        WdfWaitLockRelease(hubGlobals->HubFdoListLock);

        HUBMISC_FreeBufferIfNotNull(hubFdoContext->HubSymbolicLinkNameUnicode.Buffer, USBHUB3_HUB_TAG);
        RtlInitUnicodeString(&hubFdoContext->HubSymbolicLinkNameUnicode, NULL);

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_HUB_RELEASE_HARDWARE_COMPLETE(
            0,
            hubFdoContext->HubParentInterface.Hub,
            status);
    }

    return status;
}


NTSTATUS
HUBFDO_EvtDeviceD0Entry(
    WDFDEVICE               Device,
    WDF_POWER_DEVICE_STATE  PreviousState
    )
/*++
Routine Description:

    HUBFDO_EvtDeviceD0Entry is called whenever our FDO is powered UP either
    explicitly or implicitly.

Arguments:

    Device - the WDFDEVICE representing our FDO

    PreviousState - the previous power state we are coming from

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                status;
    PHUB_FDO_CONTEXT        hubFdoContext;
    BOOLEAN                 activityIdSet;
    POWER_ACTION            powerAction;

    UNREFERENCED_PARAMETER(PreviousState);

    TRY {

       hubFdoContext = GetHubFdoContext(Device);

       //
       // Store it for logging it in ETW rundown
       //
       hubFdoContext->CurrentWdfPowerDeviceState = WdfPowerDeviceD0;


       INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, CanAcquirePowerReference);

       if ((hubFdoContext->SelectiveSuspendSetting == NULL) &&
           (hubFdoContext->HubInformation.HubFlags.S0IdleConfigured == 1)) {

           //
           // We register for this power setting in D0Entry as opposed to AddDevice
           // is because the callback of this setting reqquires us to take power
           // reference, which we can only do once the hub has started
           //
           status = PoRegisterPowerSettingCallback(WdfDeviceWdmGetDeviceObject(Device),
                                                   &GUID_USB_SETTING_SELECTIVE_SUSPEND,
                                                   HUBFDO_PowerSettingCallback,
                                                   hubFdoContext,
                                                   &hubFdoContext->SelectiveSuspendSetting);
           if (!NT_SUCCESS(status)) {

               DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "PoRegisterPowerSettingCallback Failed %!STATUS!", status);

               //
               // Ignore the error
               //
               status = STATUS_SUCCESS;
               hubFdoContext->SelectiveSuspendSetting = NULL;

           }
       }


       status = STATUS_SUCCESS;

        activityIdSet = FALSE;

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_D0_ENTRY_START() == TRUE) {

            status = EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID,
                                          &hubFdoContext->HsmContext.CurrentActivityId);

            if (NT_SUCCESS(status)) {

                HUBMISC_SetActivityIdIrp(hubFdoContext->HubControlRequest.Irp,
                                         &hubFdoContext->HsmContext.CurrentActivityId);

                activityIdSet = TRUE;
            }
        }

        powerAction = WdfDeviceGetSystemPowerAction(Device);

        EventWriteUSBHUB3_ETW_EVENT_HUB_D0_ENTRY_START(
            &hubFdoContext->HsmContext.CurrentActivityId,
            hubFdoContext->HubParentInterface.Hub,
            WdfPowerDeviceD0,
            powerAction,
            (USHORT)hubFdoContext->HubInformation.HubFlags.WaitWakeQueued);

        switch (powerAction) {

        case PowerActionSleep:
        case PowerActionHibernate:
        case PowerActionShutdown:
        case PowerActionShutdownReset:
        case PowerActionShutdownOff:
            status = HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDOD0EntryFromSx);
            break;

        case PowerActionNone:
            status = HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDOD0Entry);

            break;

        default:
            NT_ASSERTMSG("Unexpected Power Action for Hub FDO", FALSE);
        }


    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_HUB_D0_ENTRY_COMPLETE(
            &hubFdoContext->HsmContext.CurrentActivityId,
            hubFdoContext->HubParentInterface.Hub,
            status,
            powerAction);

        if (activityIdSet == TRUE) {
            RtlZeroMemory(&hubFdoContext->HsmContext.CurrentActivityId, sizeof(GUID));
        }
    }

    return status;
}


NTSTATUS
HUBFDO_EvtDeviceD0Exit(
    WDFDEVICE               Device,
    WDF_POWER_DEVICE_STATE  TargetState
    )
/*++
Routine Description:

    HUBFDO_EvtDeviceD0Exit is called whenever our FDO is powering
    DOWN, either explicitly or implicitly

Arguments:

    Device - the WDFDEVICE representing our FDO

    TargetState - the power state we are transitioning to

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                status;
    PHUB_FDO_CONTEXT        hubFdoContext;
    POWER_ACTION            powerAction;
    BOOLEAN                 activityIdSet;

    TRY {

        status = STATUS_SUCCESS;
        activityIdSet = FALSE;

        hubFdoContext = GetHubFdoContext(Device);

        //
        // Store it for logging it in ETW rundown
        //
        hubFdoContext->CurrentWdfPowerDeviceState = TargetState;

        INTERLOCKED_CLEAR_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, CanAcquirePowerReference);

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_D0_EXIT_START()) {

            status = EtwActivityIdControl(EVENT_ACTIVITY_CTRL_CREATE_ID,
                                          &hubFdoContext->HsmContext.CurrentActivityId);

            if (NT_SUCCESS(status)) {

                HUBMISC_SetActivityIdIrp(hubFdoContext->HubControlRequest.Irp,
                                         &hubFdoContext->HsmContext.CurrentActivityId);

                activityIdSet = TRUE;
            }
        }

        powerAction = WdfDeviceGetSystemPowerAction(Device);

        EventWriteUSBHUB3_ETW_EVENT_HUB_D0_EXIT_START(
            &hubFdoContext->HsmContext.CurrentActivityId,
            hubFdoContext->HubParentInterface.Hub,
            TargetState,
            powerAction,
            (USHORT)hubFdoContext->HubInformation.HubFlags.WaitWakeQueued);

        switch (powerAction) {

        case PowerActionSleep:
        case PowerActionHibernate:
        case PowerActionShutdown:
        case PowerActionShutdownReset:
        case PowerActionShutdownOff:

#if DBG
            //
            // If hang on D0 is set, stall execution here to cause 0x9F (POWER_STATE_FAILURE) bugcheck
            //

            if (g_TriggerPowerStateFailureBugcheck) {
                HUBMISC_InducePowerStateFailureBugcheck();
            }

#endif
            status = HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDOD0Exit);
            break;

        case PowerActionNone:
            if (TargetState == WdfPowerDeviceD3Final) {

                status = HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDOD0ExitFinal);
            } else {
                status = HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDOD0Exit);
            }

            break;

        default:
            NT_ASSERTMSG("Unexpected Power Action for Hub FDO", FALSE);
        }

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_HUB_D0_EXIT_COMPLETE(
            &hubFdoContext->HsmContext.CurrentActivityId,
            hubFdoContext->HubParentInterface.Hub,
            status,
            powerAction);

        if (activityIdSet == TRUE) {
            RtlZeroMemory(&hubFdoContext->HsmContext.CurrentActivityId, sizeof(GUID));
        }
    }

    return status;
}


VOID
HUBFDO_EvtDeviceSurpriseRemoval(
    WDFDEVICE               Device
    )
/*++
Routine Description:

    HubFdo_EvtDeviceSurpriseRemoval is called whenever our FDO is surprise
    removed

Arguments:

    Device - the WDFDEVICE representing our FDO

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;

    PAGED_CODE ();

    TRY {

        hubFdoContext = GetHubFdoContext(Device);

        EventWriteUSBHUB3_ETW_EVENT_HUB_SURPRISE_REMOVAL_START(
            0,
            hubFdoContext->HubParentInterface.Hub);

        HUBMUX_QueueSurpriseRemoveToAllSMs(hubFdoContext);

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_HUB_SURPRISE_REMOVAL_COMPLETE(
            0,
            hubFdoContext->HubParentInterface.Hub);
    }
}

VOID
HUBFDO_EvtDeviceUsageNotification(
    WDFDEVICE               Device,
    WDF_SPECIAL_FILE_TYPE   NotificationType,
    BOOLEAN                 IsInNotificationPath
    )
/*++
Routine Description:

    Called whenever our FDO is notified of a change in its use
    for special file support

Arguments:

    Device - the WDFDEVICE representing our FDO

    NotificationType - specifies the type of special file this call pertains to

    IsInNotificationPath - Indicates whether or not the system is starting to use the special file
            or if the system has finished using the special file

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT                        hubFdoContext;
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS   wakeSettings;
    NTSTATUS                                status;



    TRY {
        hubFdoContext = GetHubFdoContext(Device);

        switch(NotificationType) {
        case WdfSpecialFilePaging:
        case WdfSpecialFileBoot:
            if (IsInNotificationPath) {
                INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, IsInBootOrPagingPath);

                //
                // Arm the hub for wake from system sleep if
                // a boot device is connected downstream
                //
                WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(&wakeSettings);
                wakeSettings.ArmForWakeIfChildrenAreArmedForWake = TRUE;
                wakeSettings.Enabled = WdfTrue;
                wakeSettings.IndicateChildWakeOnParentWake = TRUE;
                wakeSettings.UserControlOfWakeSettings = WakeDoNotAllowUserControl;
                status = WdfDeviceAssignSxWakeSettings(Device,
                                                     &wakeSettings);

                if (!NT_SUCCESS(status)) {

                  DbgTraceTo(hubFdoContext->IfrLog,
                             TL_ERROR,
                             Hub,
                             "WdfDeviceAssignSxWakeSettings Failed for hub in the boot path %!STATUS!",
                             status);

                }

            }
            //
            // Fall through the next case statement
            //
        case WdfSpecialFileHibernation:
            //
            // Once special file is enabled, we never disable it
            // again because system requires a reboot to change
            // any special file location. For huibernatation, we
            // will get a disable without reboot but it not worth
            // disabling forward progress for it
            //
            if (IsInNotificationPath) {
                if (hubFdoContext->HsmContext.NeedsForwardProgress) {
                    LEAVE;
                }

                hubFdoContext->HsmContext.NeedsForwardProgress = TRUE;
                HUBUCX_NotifyForwardProgress(hubFdoContext);
                if (hubFdoContext->HubParentInterface.HubDepth == 0) {
                    //
                    // Lock the driver in memory. Only need to
                    // do it once, for the root hub. To do this,
                    // randomly pick one function that belongs to
                    // the PAGE section. And then all functions
                    // in this section will be locked. We do not
                    // need to unlock them ever, so we don't need
                    // the return value
                    //
                    (VOID)MmLockPagableCodeSection(HUBFDO_EvtDeviceAdd);
                }
            }
            break;

        default:
            break;
        }

    } FINALLY {

    }

}

VOID
HUBFDO_EvtDeviceCleanupCallback(
    WDFOBJECT               Device
    )
/*++
Routine Description:


Arguments:

    Device - The WDFDEVICE representing our FDO

Return Value:

    NTSTATUS

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;
    NTSTATUS                status;

    TRY {

        hubFdoContext = GetHubFdoContext(Device);

        EventWriteUSBHUB3_ETW_EVENT_HUB_CLEANUP_START(
            0,
            hubFdoContext->HubParentInterface.Hub);

        //
        //
        // Unregister for the Selective Suspend Timeout power setting
        //
        if (hubFdoContext->SelectiveSuspendTimeoutPowerSetting) {
            status = PoUnregisterPowerSettingCallback(hubFdoContext->SelectiveSuspendTimeoutPowerSetting);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub,
                           "PoUnregisterPowerSettingCallback Failed %!STATUS!", status);
            }

            hubFdoContext->SelectiveSuspendTimeoutPowerSetting = NULL;
        }

        //
        //
        // Unregister for the Selective Suspend setting
        //
        if (hubFdoContext->SelectiveSuspendSetting) {
            status = PoUnregisterPowerSettingCallback(hubFdoContext->SelectiveSuspendSetting);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub,
                           "PoUnregisterPowerSettingCallback Failed %!STATUS!", status);
            }

            hubFdoContext->SelectiveSuspendSetting = NULL;
        }


        // Verify that the HSM was successfully initialized
        //
        if (hubFdoContext->HubInformation.HubFlags.Initialized == 1) {

            //
            // HSM is not initialized, so we can't send it any events
            //
            HUBFDO_AddEventAndWait(hubFdoContext, HsmEventFDODeviceCleanup);

        }

        //
        // Clean up any non-WDF resources here
        //

        if (hubFdoContext->HsmContext.SmWorker) {

            HUBMISC_FlushWorkerItem(hubFdoContext,
                                    hubFdoContext->HsmContext.SmWorker);

            HUBMISC_DeleteWorkerItem(hubFdoContext,
                                     hubFdoContext->HsmContext.SmWorker);

            hubFdoContext->HsmContext.SmWorker = NULL;
        }


        HUBMISC_FreeBufferIfNotNull(hubFdoContext->ConfigurationDescriptor, USBHUB3_HUB_TAG);

        WppRecorderLogDelete(hubFdoContext->IfrLog);

        if (hubFdoContext->HubInformation.HubNumber != 0) {
            USBD_ReleaseHubNumber(hubFdoContext->HubInformation.HubNumber);
            hubFdoContext->HubInformation.HubNumber = 0;
        }

    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_HUB_CLEANUP_COMPLETE(
            0,
            hubFdoContext->HubParentInterface.Hub);
    }
}

VOID
HUBFDO_EvtDeviceDestroyCallback(
    WDFOBJECT               Device
    )
/*++
Routine Description:


Arguments:

    Device - The WDFDEVICE representing our FDO

Return Value:

    NTSTATUS

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;

    TRY {

        hubFdoContext = GetHubFdoContext(Device);

        if (hubFdoContext->HsmContext.Timer) {
            ExDeleteTimer(hubFdoContext->HsmContext.Timer, TRUE, FALSE, NULL);
            hubFdoContext->HsmContext.Timer = NULL;
        }


    } FINALLY {

    }
}

NTSTATUS
HUBFDO_EvtDeviceArmWakeFromS0(
    WDFDEVICE               Device
    )
/*++
Routine Description:

    HUBFDO_EvtDeviceArmWakeFromS0 is called whenever our FDO sends
    wait wake in preparation of Selective Suspend
Arguments:

    Device - the WDFDEVICE representing our FDO

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;

    TRY {

        hubFdoContext = GetHubFdoContext(Device);
        //
        // ArmedForWake keeps track of whether hub needs to be
        // armed for connect/disconnect/overcurrent. This information
        // is used by PSM30 to decide whether to arm itself.
        //
        INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, ArmedForWake);
        //
        // WaitWakeQueued keeps track of whether there is a wait
        // wake IRP queued by the FDO. It is only used for
        // diagnostive purposes
        //
        INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, WaitWakeQueued);


    } FINALLY {

    }

    return STATUS_SUCCESS;
}

VOID
HUBFDO_EvtDeviceDisarmWakeFromS0(
    WDFDEVICE               Device
    )
/*++
Routine Description:

    HUBFDO_EvtDeviceArmWakeFromS0 is called whenever our FDO sends
    wait wake in preparation of Selective Suspend
Arguments:

    Device - the WDFDEVICE representing our FDO

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;

    TRY {

        hubFdoContext = GetHubFdoContext(Device);
        INTERLOCKED_CLEAR_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, ArmedForWake);
        INTERLOCKED_CLEAR_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, WaitWakeQueued);


    } FINALLY {

    }

}

NTSTATUS
HUBFDO_EvtDeviceArmWakeFromSx(
    WDFDEVICE               Device
    )
/*++
Routine Description:

    HUBFDO_EvtDeviceArmWakeFromSx is called whenever our FDO sends
    wait wake in preparation of System Suspend
Arguments:

    Device - the WDFDEVICE representing our FDO

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;

    TRY {

        hubFdoContext = GetHubFdoContext(Device);
        //
        // WaitWakeQueued keeps track of whether there is a wait
        // wake IRP queued by the FDO. It is only used for
        // diagnostive purposes
        //
        INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, WaitWakeQueued);


    } FINALLY {

    }

    return STATUS_SUCCESS;
}

VOID
HUBFDO_EvtDeviceDisarmWakeFromSx(
    WDFDEVICE               Device
    )
/*++
Routine Description:

    HUBFDO_EvtDeviceArmWakeFromSx is called whenever wait wake
    for System Suspend is cancelled or completed

Arguments:

    Device - the WDFDEVICE representing our FDO

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT        hubFdoContext;

    TRY {

        hubFdoContext = GetHubFdoContext(Device);
        INTERLOCKED_CLEAR_BIT_FIELD(HUB_FLAGS, &hubFdoContext->HubInformation.HubFlags, WaitWakeQueued);


    } FINALLY {

    }

}


VOID
HUBFDO_ReleaseWdfPowerReference (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Notifies WDF framwork that the hub is idle.

Return Value:

    VOID

--*/
{
    KIRQL               currentIrql;

    TRY {
        //DbgTraceTo(HubFdoContext->IfrLog, TL_INFO, Hub, "HSM releasing a Power reference on the hub 0x%p", HubFdoContext);
        if(HubFdoContext->HubInformation.HubFlags.WdfPowerReferencePending == 1){
            // If we call this API at passive level, KMDF can choose to turn back
            // and call in it one of our PDO event callbacks that block. Those
            // callbacks will not unblock until the state machine runs, which will
            // not happen until we return from this call. So we could end up in
            // a deadlock. To avoid this issue, we temperorily raise the IRQL
            // to dispatch
            //
            KeRaiseIrql(DISPATCH_LEVEL,
                        &currentIrql);

            WdfDeviceResumeIdle(HubFdoContext->HubFdoDevice);

            KeLowerIrql(currentIrql);

            INTERLOCKED_CLEAR_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, WdfPowerReferencePending);
        }

    } FINALLY {

    }
}


VOID
HUBFDO_AcquireWdfPowerReference (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Notifies WDF framwork that the hub is not idle.

Return Value:

    HsmEventOperationPending
    HsmEventOperationSuccess
    HsmEventOperationError

--*/
{
    NTSTATUS            status;
    KIRQL               currentIrql;


    TRY {

        //DbgTraceTo(HubFdoContext->IfrLog, TL_INFO, Hub, "HSM acquiring a Power reference on the hub 0x%p", HubFdoContext);

        NT_ASSERT(HubFdoContext->HubInformation.HubFlags.WdfPowerReferencePending == 0);

        // If we call this API at passive level, KMDF can choose to turn back
        // and call in it one of our PDO event callbacks that block. Those
        // callbacks will not unblock until the state machine runs, which will
        // not happen until we return from this call. So we could end up in
        // a deadlock. To avoid this issue, we temperorily raise the IRQL
        // to dispatch
        //
        KeRaiseIrql(DISPATCH_LEVEL,
                    &currentIrql);

        status = WdfDeviceStopIdle(HubFdoContext->HubFdoDevice, FALSE);

        KeLowerIrql(currentIrql);


        if (NT_SUCCESS(status)) {
            INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, WdfPowerReferencePending);
        }

    } FINALLY {

    }

}


VOID
HUBFDO_ReportDeviceFailureToPnp (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Reports hub failure to Pnp.

Return Value:

    VOID

--*/
{
    TRY {

        if (HubFdoContext->HubFailureMsgId != 0) {
            HUBMISC_ReportPnPFailureProblem( HubFdoContext->HubFdoDevice,
                                             HubFdoContext->HubFailureMsgId );
        }

        WdfDeviceSetFailed( HubFdoContext->HubFdoDevice, WdfDeviceFailedNoRestart );

    } FINALLY {

    }
}


VOID
HUBFDO_SignalPnpPowerEvent (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    This function sets the return status to Success and signals the event on
    which all the WDF callbacks are waiting on.

Return Value:

    VOID

--*/
{
    TRY {

        HubFdoContext->PnPPowerStatus = STATUS_SUCCESS;

        KeSetEvent(&HubFdoContext->PnPPowerEvent, IO_NO_INCREMENT, FALSE);

    } FINALLY {

    }
}


VOID
HUBFDO_SignalPnpPowerFailureEvent (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    This function sets the return status to Failure and signals the event
    on which all the WDF callbacks are waiting on.

Return Value:

    VOID

--*/
{
    TRY {

        HubFdoContext->PnPPowerStatus = STATUS_UNSUCCESSFUL;

        KeSetEvent(&HubFdoContext->PnPPowerEvent, IO_NO_INCREMENT, FALSE);

    } FINALLY {

    }
}


VOID
HUBFDO_EvtIoInternalDeviceControlFromPDO (
    WDFQUEUE        Queue,
    WDFREQUEST      Request,
    size_t          OutputBufferLength,
    size_t          InputBufferLength,
    ULONG           IoControlCode
    )
/*++

Routine Description:

    This function dispatches requests that were forwarded from the PDO to
    the FDO queue.

Return Value:

    VOID

--*/
{
    PHUB_FDO_CONTEXT                hubFdoContext;
    NTSTATUS                        status;
    PHUB_PDO_CONTEXT                pdoContext;
    WDF_REQUEST_PARAMETERS          params;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    TRY {

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        switch (IoControlCode) {
        case IOCTL_INTERNAL_USB_GET_PORT_STATUS:

            WDF_REQUEST_PARAMETERS_INIT(&params);
            WdfRequestGetParameters(Request,
                                   &params);

            pdoContext = (PHUB_PDO_CONTEXT)params.Parameters.Others.Arg2;

            //
            // This request needs to go through HSM because
            // it involves sending a transfer to the hub and
            // requires coordination with the current state
            // of the hub
            //
            hubFdoContext->GetPortStatusTargetPortContext = pdoContext->AssociatedDSM->PortContext;
            hubFdoContext->PdoGetPortStatusRequest = Request;

            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventPDORequestGetPortStatus);

            status = STATUS_PENDING;
            break;

        default:
            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, 
                "ERROR_ASSERT: FDO got an IOCTL from the PDO that it shouldn't need to handle: 0x%x", IoControlCode);
            NT_ASSERTMSG("FDO got an IOCTL from the PDO that it shouldn't need to handle", FALSE);
            status = STATUS_UNSUCCESSFUL;
            break;
        }


    } FINALLY {

        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(Request,
                               status);
        }

    }

}


NTSTATUS
HUBFDO_IoctlValidateParameters(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        size_t              MinInputLength,
    __in
        size_t              InputBufferLength,
    __in_opt
        PVOID               InputBuffer,
    __in
        size_t              MinOutputLength,
    __in
        size_t              OutputBufferLength
    )
/*++
Routine Description:

    Validates input/output buffer lengths and the connection index (port number)
    for user mode IOCTLs

    If MinInputLength is non-zero it is assumed to point to a connectionIndex value
    in the first ULONG

Arguments:

    IoControlCode - IOCTL code we are validating

    MinInputLength - minimum size of input data, usually a connection index

    InputBufferLength - Size of the input buffer for the current IOCTL

    InputBuffer - Input buffer for the current IOCTL

    MinOutputLength - min we need to return useful information.

    OutputBufferLength - Size of the output buffer for the current IOCTL

--*/
{
    PULONG                          connectionIndex;
    NTSTATUS                        status;

    PAGED_CODE();

    TRY {

        connectionIndex = NULL;

        status = STATUS_SUCCESS;

        //
        // The hub must be initialized and running to return useful information
        //
        if (HubFdoContext->HubInformation.HubFlags.Configured == 0) {

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // validate input buffer length
        //
        if (InputBufferLength < MinInputLength) {

            DbgTraceTo(HubFdoContext->IfrLog, TL_ERROR, Hub, "Input buffer length too small");

            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        //
        // make sure out buffer length is large enough to hold the minimum response
        //
        if (OutputBufferLength < MinOutputLength) {

            DbgTraceTo(HubFdoContext->IfrLog, TL_ERROR, Hub, "Output buffer length too small");

            status = STATUS_BUFFER_TOO_SMALL;
            LEAVE;
        }

        //
        // Assumption here is that if there is an input buffer
        // the first ULONG is the connection index (port number).
        //
        if (MinInputLength >= sizeof(*connectionIndex)) {

            connectionIndex = (PULONG)InputBuffer;

            //
            // Make sure port number is valid
            //
            if ((*connectionIndex == 0) ||
                (*connectionIndex > HubFdoContext->HubInformation.NumberOfPorts)) {

                DbgTraceTo(HubFdoContext->IfrLog, TL_ERROR, Hub, "Connection index not a valid port number");

                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }
        }

    } FINALLY {

    }

    return status;
}


FORCEINLINE
WDFDEVICE
HUBFDO_FindDeviceForPort(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        ULONG               PortNumber
    )
/*++
Routine Description:

    Finds a child device for specified port

    This function requires the static child list be locked for iteration.

Returns:

    Device context for the device if found, NULL otherwise

--*/
{
    PHUB_PDO_CONTEXT                pdoContext;
    WDFDEVICE                       childDevice;

    TRY {

        childDevice = WdfFdoRetrieveNextStaticChild(HubFdoContext->HubFdoDevice,
                                                    NULL,
                                                    WdfRetrievePresentChildren);

        while (childDevice != NULL) {

            pdoContext = GetHubPdoContext(childDevice);

            if (pdoContext->PortNumber == PortNumber) {

                LEAVE;
            }

            childDevice = WdfFdoRetrieveNextStaticChild(HubFdoContext->HubFdoDevice,
                                                        childDevice,
                                                        WdfRetrievePresentChildren);
        }

    } FINALLY {

    }

    return childDevice;
}

FORCEINLINE
PPORT_CONTEXT
HUBFDO_FindPortContextFromNumber(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        ULONG               PortNumber
    )
/*++
Routine Description:

    Finds the port context got a given port number


Returns:

    Port context for the port is found, NULL otherwise

--*/
{
    PPORT_CONTEXT                       currentPort;
    PPORT_CONTEXT                       targetPort;

    TRY {

        targetPort = NULL;

        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &HubFdoContext->MuxContext.PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            if (currentPort->PortInformation.PortNumber == PortNumber) {

                targetPort = currentPort;
                break;
            }

        }

    } FINALLY {

    }

    return targetPort;

}


EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBFDO_GetPortStatusForDebuggingComplete;
VOID
HUBFDO_GetPortStatusForDebuggingComplete (
    __in
        WDFREQUEST                      WdfRequest,
    __in
        WDFIOTARGET                     Target,
    __in
        PWDF_REQUEST_COMPLETION_PARAMS  CompletionParams,
    __in
        WDFCONTEXT                      Context
    )
/*++

Routine Description:

    Stub completion routine for GetPortStatus debugging request

Returns:

    VOID

--*/
{
    UNREFERENCED_PARAMETER(WdfRequest);
    UNREFERENCED_PARAMETER(Target);
    UNREFERENCED_PARAMETER(CompletionParams);
    UNREFERENCED_PARAMETER(Context);
}


VOID
HUBFDO_GetPortStatusForDebugging (
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        ULONG               PortNumber
    )
/*++
Routine Description:

    This function is only for the purposes of debugging
    This code allows us to force a query port status when
    no change is being received from the hardware.

Returns:

    Device context for the device if found, NULL otherwise

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET      setupPacket;
    PPORT_CONTEXT                       currentPort;
    NTSTATUS                            status;
    USHORT                              length;

    TRY {

        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &HubFdoContext->MuxContext.PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            if (currentPort->PortInformation.PortNumber != PortNumber) {
                continue;
            }
            setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET)currentPort->PortControlRequest.Urb.SetupPacket;

            //
            // Format the setup packet for Get Port Status
            // USB 3.0 Spec, 10.14.2.6
            // USB 2.0 Spec, 11.24.2.7
            //
            setupPacket->bmRequestType.Recipient = BMREQUEST_TO_OTHER;
            setupPacket->bmRequestType.Type =  BMREQUEST_CLASS;
            setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;

            setupPacket->bRequest = USB_REQUEST_GET_STATUS;
            setupPacket->wIndex.W = (USHORT)PortNumber;
            if (currentPort->PortInformation.PortFlags.PortFlagEnhancedSuperSpeed) {
                setupPacket->wValue.W = USB_STATUS_EXT_PORT_STATUS;
                length = sizeof(currentPort->CurrentExtPortStatusChange);
            } else {
                setupPacket->wValue.W = USB_STATUS_PORT_STATUS;
                length = sizeof(currentPort->CurrentExtPortStatusChange.PortStatusChange);
            }
            setupPacket->wLength = length;

            status = HUBMISC_ControlTransfer(HubFdoContext,
                                             HubFdoContext->HubParentInterface.Hub,
                                             currentPort,
                                             &currentPort->PortControlRequest,
                                             HUBFDO_GetPortStatusForDebuggingComplete,
                                             &currentPort->CurrentExtPortStatusChange,
                                             length,
                                             FALSE,
                                             FALSE);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "Get Port Status Control Transfer Failed %!STATUS!",
                           status);
            }

            NT_ASSERT(NT_SUCCESS(status));

            break;

        }

    } FINALLY {

    }

}


VOID
HUBFDO_BuildUsb20HubDescriptor(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __out
        PUSB_HUB_DESCRIPTOR HubDescriptor
    )
/*++
Routine Description:

    Returns or builds a USB 2.0 Hub Descriptor.  This is used for compatibility with
    legacy applications that only understand USB2 hub descriptors.

Parameters:

--*/
{
    size_t                  numPortBytes;
    ULONG                   i;

    TRY {

        RtlZeroMemory(HubDescriptor, sizeof(USB_HUB_DESCRIPTOR));

        //
        // Build the hub descriptor for the root hub
        //
        if (HubFdoContext->HubParentInterface.HubDepth == 0) {

            HubDescriptor->bDescriptorLength = 0x09;
            HubDescriptor->bDescriptorType = USB_20_HUB_DESCRIPTOR_TYPE;
            HubDescriptor->bNumberOfPorts = (UCHAR)HubFdoContext->HubInformation.NumberOfPorts;
            HubDescriptor->wHubCharacteristics = 0;
            HubDescriptor->bPowerOnToPowerGood = 2;
            HubDescriptor->bHubControlCurrent = 0;

            numPortBytes = (((HubDescriptor->bNumberOfPorts) / 8) + 1);

            //
            // fill in the variable part of the hub descriptor
            //
            for (i = 0; i < numPortBytes; i++) {
                HubDescriptor->bRemoveAndPowerMask[i] = 0;
                HubDescriptor->bRemoveAndPowerMask[i+numPortBytes] = 0xff;
            }

        } else {

            switch(HubFdoContext->HubParentInterface.HubSpeed) {
            case UsbHighSpeed:
            case UsbFullSpeed:
                //
                // USB 2.0/1.1 Hubs can simply copy the cached hub descriptor
                //
                RtlCopyMemory(HubDescriptor,
                              &HubFdoContext->HubDescriptor.Hub20Descriptor,
                              sizeof(USB_HUB_DESCRIPTOR));
                break;

            case UsbSuperSpeed:
                //
                // USB 3.0 hubs use a different format for the hub descriptor.
                // We build a USB 2.0 hub descriptor to maintain compatibility
                //
                HubDescriptor->bDescriptorLength = 0x09;
                HubDescriptor->bDescriptorType = USB_20_HUB_DESCRIPTOR_TYPE;
                HubDescriptor->bNumberOfPorts = (UCHAR)HubFdoContext->HubInformation.NumberOfPorts;
                HubDescriptor->wHubCharacteristics =
                    (HubFdoContext->HubDescriptor.Hub30Descriptor.wHubCharacteristics & 0x1F);
                HubDescriptor->bPowerOnToPowerGood =
                    HubFdoContext->HubDescriptor.Hub30Descriptor.bPowerOnToPowerGood;
                HubDescriptor->bHubControlCurrent =
                    HubFdoContext->HubDescriptor.Hub30Descriptor.bHubControlCurrent;

                numPortBytes = (((HubDescriptor->bNumberOfPorts) / 8) + 1);

                //
                // fill in the variable part of the hub descriptor
                //
                for (i = 0; i < numPortBytes; i++) {
                    HubDescriptor->bRemoveAndPowerMask[i] = 0;
                    HubDescriptor->bRemoveAndPowerMask[i+numPortBytes] = 0xff;
                }

                break;

            default:
                NT_ASSERTMSG("Invalid Hub Type", 0);
                break;
            }
        }
    } FINALLY {
    }

}

VOID
HUBFDO_IoctlGetPortStatus(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Handles the request to get the current port status

Parameters:

--*/
{
    NTSTATUS                        status;
    ULONG                           portNumber;
    PUSB_PORT_STATUS_INFORMATION    portStatus;
    PHUB_FDO_CONTEXT                hubFdoContext;
    PPORT_CONTEXT                   portContext;

    PAGED_CODE();

    TRY {

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &portStatus,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(ULONG),
                                                InputBufferLength,
                                                portStatus,
                                                sizeof(USB_PORT_STATUS_INFORMATION),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        portNumber = (USHORT) portStatus->ConnectionIndex;
        RtlZeroMemory(portStatus, OutputBufferLength);
        portStatus->ConnectionIndex = portNumber;

        portContext = HUBFDO_FindPortContextFromNumber(hubFdoContext,
                                                       portNumber);

        if (portContext != NULL) {

            //
            // This request needs to go through HSM because
            // it involves sending a transfer to the hub and
            // requires coordination with the current state
            // of the hub
            //
            hubFdoContext->GetPortStatusTargetPortContext = portContext;
            hubFdoContext->PdoGetPortStatusRequest = Request;

            HUBSM_AddHsmEvent(hubFdoContext,
                              HsmEventPDORequestGetPortStatus);

        } else {
            status = STATUS_INVALID_PARAMETER;

            WdfRequestSetInformation(Request, sizeof(USB_PORT_STATUS_INFORMATION));
            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(Request, status);
        }
    }
}

VOID
HUBFDO_IoctlGetNodeInformation(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns information about the USB hub.

Parameters:

    Input:
        USB_NODE_INFORMATION structure

    Output:
        USB_NODE_INFORMATION structure filled in as appropriate

--*/
{
    NTSTATUS                status;
    PUSB_NODE_INFORMATION   nodeInformation;
    PHUB_FDO_CONTEXT        hubFdoContext;
    USB_HUB_DESCRIPTOR      *hubDescriptor;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                0,
                                                InputBufferLength,
                                                NULL,
                                                sizeof(USB_NODE_INFORMATION),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &nodeInformation,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        RtlZeroMemory(nodeInformation, OutputBufferLength);

        nodeInformation->NodeType = UsbHub;

        //
        // 100 milliamps/port means bus powered
        //
        if (hubFdoContext->HubInformation.MaxPortPower == MaxPortPower100Milliamps) {

            nodeInformation->u.HubInformation.HubIsBusPowered = TRUE;
        }

        hubDescriptor = &nodeInformation->u.HubInformation.HubDescriptor;

        HUBFDO_BuildUsb20HubDescriptor(hubFdoContext, hubDescriptor);

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_INFORMATION_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_INFORMATION_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                0,
                status);
        }

        WdfRequestCompleteWithInformation(Request,
                                          status,
                                          sizeof(struct _USB_NODE_INFORMATION));
    }
}



VOID
HUBFDO_IoctlGetNodeConnectionDriverKeyName(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns the DriverKeyName associated with a PDO on a particular port.

Parameters:

    Input:
        USB_NODE_CONNECTION_DRIVERKEY_NAME structure

    Output:
        USB_NODE_CONNECTION_DRIVERKEY_NAME filled in as appropriate

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_NODE_CONNECTION_DRIVERKEY_NAME driverKeyName;
    ULONG                               driverKeyNameLength;
    ULONG                               portNumber;
    WDFDEVICE                           childDevice;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;
        portNumber = 0;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        driverKeyName = NULL;

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &driverKeyName,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(ULONG),
                                                InputBufferLength,
                                                driverKeyName,
                                                sizeof(USB_NODE_CONNECTION_DRIVERKEY_NAME),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        portNumber = (USHORT) driverKeyName->ConnectionIndex;

        WdfFdoLockStaticChildListForIteration(hubFdoContext->HubFdoDevice);

        childDevice = HUBFDO_FindDeviceForPort(hubFdoContext, portNumber);

        if (childDevice == NULL) {

            WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        RtlZeroMemory(driverKeyName, OutputBufferLength);

        //
        // restore the original connection index
        //
        driverKeyName->ConnectionIndex = portNumber;

        //
        // Determine the space available to copy the drive key name
        //
        driverKeyNameLength =
            (ULONG)(OutputBufferLength -
                    (sizeof(USB_NODE_CONNECTION_DRIVERKEY_NAME) - sizeof(WCHAR)));

        status = WdfDeviceQueryProperty(childDevice,
                                        DevicePropertyDriverKeyName,
                                        driverKeyNameLength,
                                        &driverKeyName->DriverKeyName[0],
                                        &driverKeyNameLength);

        WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

        //
        // eat the buffer too small error and return the actual length
        //
        if (status == STATUS_BUFFER_TOO_SMALL) {
            status = STATUS_SUCCESS;
        }

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        driverKeyName->ActualLength = driverKeyNameLength +
                                      sizeof(USB_NODE_CONNECTION_DRIVERKEY_NAME);

        if (OutputBufferLength >= driverKeyName->ActualLength) {

            //
            // user buffer is bigger, just indicate how much we copied
            //
            WdfRequestSetInformation(Request, driverKeyName->ActualLength);

        } else {

            //
            // COMPAT: Windows 7 hub driver will return a NULL string
            //         if the actual length of the driver key name is
            //         larger than the space in the supplied buffer
            //
            driverKeyName->DriverKeyName[0] = UNICODE_NULL;

            WdfRequestSetInformation(Request, sizeof(USB_NODE_CONNECTION_DRIVERKEY_NAME));
        }

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                portNumber,
                status);
        }

        //
        // COMPAT: In case of an error set the ActualLength to zero.
        //         Some 3rd-party drivers check this length without
        //         checking the returned status for an error.
        //
        if (!NT_SUCCESS(status) &&
            (driverKeyName != NULL) &&
            (OutputBufferLength >= sizeof(USB_NODE_CONNECTION_DRIVERKEY_NAME))) {

                driverKeyName->DriverKeyName[0] = UNICODE_NULL;
                driverKeyName->ActualLength = sizeof(USB_NODE_CONNECTION_DRIVERKEY_NAME);
        }

        WdfRequestComplete(Request, status);
    }
}


VOID
HUBFDO_IoctlGetNodeConnectionInfo(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength,
    __in
        BOOLEAN         extendedApi
    )
/*++
Routine Description:

    Returns information about a specific USB hub port.  If there is a device
    connected to the port, information about the device is also
    returned.

Parameters:

    extendedApi - Indicates extended information is to be returned.

    Input:

        Pointer to USB_NODE_CONNECTION_INFORMATION if extendedApi is FALSE
        OR
        Pointer to USB_NODE_CONNECTION_INFORMATION_EX structure if extendedApi is TRUE:

            ConnectionIndex - this is the 1 based port number.

    Output:

        Pointer to USB_NODE_CONNECTION_INFORMATION or USB_NODE_CONNECTION_INFORMATION_EX structure:

            DeviceDescriptor - USB device descriptor.

            CurrentConfigurationValue - Currently selected configuration value.

            if extendedApi is FALSE:
                LowSpeed - TRUE if device operating at low-speed.

            if extendedApi is TRUE:
                Speed - indicates the 'current' operating speed (UsbLowSpeed=0, UsbFullSpeed=1
                        and UsbHighSpeed=2).

            DeviceIsHub - TRUE if the attached device is a USB hub.  True is only returned if
                          the hub's PDO is in the started state.

            DeviceAddress - USB assigned device address.

            NumberOfOpenPipes - Number of open USB pipes in the current configuration

            ConnectionStatus - The current USB connection status (see USB_CONNECTION_STATUS).

            USB_PIPE_INFO PipeList[] - list of open pipes including schedule offset and
                                       endpoint descriptor.

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_NODE_CONNECTION_INFORMATION    nodeConnectionInformation;
    PDEVICE_CONTEXT                     deviceContext;
    PHUB_PDO_CONTEXT                    pdoContext;
    PPORT_CONTEXT                       portContext;
    ULONG                               portNumber;
    WDFDEVICE                           childDevice;
    PINTERFACE_HANDLE                   interfaceHandle;
    ULONG                               i;
    KIRQL                               irql;
    ULONG                               outPipeIndex;

    TRY {

        status = STATUS_SUCCESS;
        portNumber = 0;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &nodeConnectionInformation,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(ULONG),
                                                InputBufferLength,
                                                nodeConnectionInformation,
                                                sizeof(USB_NODE_CONNECTION_INFORMATION),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        portNumber = (USHORT) nodeConnectionInformation->ConnectionIndex;

        RtlZeroMemory(nodeConnectionInformation, OutputBufferLength);

        portContext = HUBFDO_FindPortContextFromNumber(hubFdoContext,
                                                       portNumber);

        if (portContext != NULL) {
            nodeConnectionInformation->ConnectionStatus = portContext->ConnectionStatus;
        } else {
            nodeConnectionInformation->ConnectionStatus = DeviceGeneralFailure;

        }

        //
        // QueryPortStatusForDebugging will always be FALSE.
        // It is intended to be set to TRUE only through the
        // debugger for the purpose of forcing a port status
        // query
        //
        if (hubFdoContext->QueryPortStatusForDebugging) {

            HUBFDO_GetPortStatusForDebugging(hubFdoContext,
                                             portNumber);
        }

        WdfFdoLockStaticChildListForIteration(hubFdoContext->HubFdoDevice);

        childDevice = HUBFDO_FindDeviceForPort(hubFdoContext, portNumber);

        if (childDevice == NULL) {

            WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

            WdfRequestSetInformation(Request, sizeof(USB_NODE_CONNECTION_INFORMATION));
            status = STATUS_SUCCESS;
            LEAVE;
        }

        //
        // Reference the child device

        pdoContext = GetHubPdoContext(childDevice);
        deviceContext = pdoContext->AssociatedDSM;

        nodeConnectionInformation->ConnectionIndex = portNumber;

        nodeConnectionInformation->DeviceDescriptor = deviceContext->DeviceDescriptor;

        nodeConnectionInformation->NumberOfOpenPipes = 0;

        if (!extendedApi) {
            if (deviceContext->UsbDeviceInfo.DeviceSpeed == UsbLowSpeed) {

                nodeConnectionInformation->LowSpeed = TRUE;
            } else {

                nodeConnectionInformation->LowSpeed = FALSE;
            }

        } else {
            PUSB_NODE_CONNECTION_INFORMATION_EX nodeConnectionInformationEx;

                NT_ASSERT(sizeof(USB_NODE_CONNECTION_INFORMATION) ==
                          sizeof(USB_NODE_CONNECTION_INFORMATION_EX));

                nodeConnectionInformationEx =
                    (PUSB_NODE_CONNECTION_INFORMATION_EX) nodeConnectionInformation;

                //
                // COMPAT: Return high-speed for superspeed devices, to avoid confusing
                //         clients that are not USB3 aware.
                //
                if (deviceContext->UsbDeviceInfo.DeviceSpeed == UsbSuperSpeed) {
                    nodeConnectionInformationEx->Speed = UsbHighSpeed;
                } else {
                    nodeConnectionInformationEx->Speed = deviceContext->UsbDeviceInfo.DeviceSpeed;
                }
        }

        nodeConnectionInformation->DeviceAddress = (USHORT)deviceContext->DeviceAddress;

        if (deviceContext->DeviceFlags.DeviceIsHub) {

            nodeConnectionInformation->DeviceIsHub = TRUE;
        }

        //
        // Synchronize access to the device configuration interface/endpoint
        // structures with DSM.
        //
        KeAcquireSpinLock(&deviceContext->ConfigurationLock, &irql);

        if ((deviceContext->DeviceStateFlags.ConfigurationIsValid == 1) &&
            (deviceContext->CurrentDeviceConfiguration != NULL)) {
            size_t                          bytesRemaining;
            PUSB_CONFIGURATION_DESCRIPTOR   configurationDescriptor;

            configurationDescriptor =
                (PUSB_CONFIGURATION_DESCRIPTOR)
                deviceContext->CurrentDeviceConfiguration->ConfigurationDescriptor;

            nodeConnectionInformation->CurrentConfigurationValue =
                configurationDescriptor->bConfigurationValue;

            nodeConnectionInformation->NumberOfOpenPipes =
                deviceContext->CurrentDeviceConfiguration->NumberOfEndpoints;

            bytesRemaining = OutputBufferLength - sizeof(USB_NODE_CONNECTION_INFORMATION);

            outPipeIndex = 0;

            FOR_ALL_IN_LIST(INTERFACE_HANDLE,
                            &deviceContext->CurrentDeviceConfiguration->InterfaceList,
                            InterfaceLink,
                            interfaceHandle) {

                if (bytesRemaining < sizeof(USB_PIPE_INFO)) {
                    break;
                }

                //
                // Walk the endpoint handles for this interface
                //
                for (i = 0; i < interfaceHandle->NumberOfEndpoints; i++) {
                    PPIPE_HANDLE    pipeHandle;

                    if (bytesRemaining < sizeof(USB_PIPE_INFO)) {
                        break;
                    }

                    pipeHandle = &interfaceHandle->PipeHandle[i];

                    nodeConnectionInformation->PipeList[outPipeIndex].EndpointDescriptor =
                        *(pipeHandle->EndpointDescriptor);

                    nodeConnectionInformation->PipeList[outPipeIndex].ScheduleOffset = 0;

                    outPipeIndex++;

                    bytesRemaining -= sizeof(struct _USB_PIPE_INFO);
                }
            }

            KeReleaseSpinLock(&deviceContext->ConfigurationLock, irql);

            WdfRequestSetInformation(Request, OutputBufferLength - bytesRemaining);

        } else {

            KeReleaseSpinLock(&deviceContext->ConfigurationLock, irql);

            WdfRequestSetInformation(Request, sizeof(USB_NODE_CONNECTION_INFORMATION));
        }

        WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

    } FINALLY {

        if (extendedApi == TRUE) {
            if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_COMPLETE()) {
                NTSTATUS    activityIdStatus;
                GUID        activityId;

                activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

                EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_COMPLETE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                    hubFdoContext->HubParentInterface.Hub,
                    portNumber,
                    status);
            }
        } else {
            if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_COMPLETE()) {
                NTSTATUS    activityIdStatus;
                GUID        activityId;

                activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

                EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_COMPLETE(
                    NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                    hubFdoContext->HubParentInterface.Hub,
                    portNumber,
                    status);
            }
        }

        WdfRequestComplete(Request, status);
    }
}


VOID
HUBFDO_IoctlGetNodeConnectionAttributes(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns the Microsoft extended port attributes for a specific port. The
    caller inputs the port number as the 'ConnectionIndex'.

    COMPAT: Extended port attributes is always set to zero on Vista and
            later versions of Windows.

    This API also returns the current connection status of the port.

Parameters:

    Input:

        Pointer to USB_NODE_CONNECTION_INFORMATION structure:

            ConnectionIndex - this is the 1 based port number.

    Output:

        Pointer to USB_NODE_CONNECTION_INFORMATION structure:

            ConnectionStatus - The current connection status, if a device is connected.

            PortAttributes - set to 0.

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_NODE_CONNECTION_ATTRIBUTES     nodeConnectionAttributes;
    ULONG                               portNumber;
    PPORT_CONTEXT                       portContext;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;
        portNumber = 0;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &nodeConnectionAttributes,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(ULONG),
                                                InputBufferLength,
                                                nodeConnectionAttributes,
                                                sizeof(USB_NODE_CONNECTION_ATTRIBUTES),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        portNumber = (USHORT) nodeConnectionAttributes->ConnectionIndex;

        RtlZeroMemory(nodeConnectionAttributes, OutputBufferLength);

        nodeConnectionAttributes->ConnectionIndex = portNumber;
        nodeConnectionAttributes->PortAttributes = 0;

        portContext = HUBFDO_FindPortContextFromNumber(hubFdoContext,
                                                       portNumber);

        if (portContext != NULL) {
            nodeConnectionAttributes->ConnectionStatus = portContext->ConnectionStatus;
        } else {
            nodeConnectionAttributes->ConnectionStatus = DeviceGeneralFailure;

        }

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                portNumber,
                status);
        }

        WdfRequestCompleteWithInformation(Request,
                                          status,
                                          sizeof(USB_NODE_CONNECTION_ATTRIBUTES));
    }
}


VOID
HUBFDO_IoctlGetHubCaps(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns the capabilities for a particular USB hub.  The capabilities are a
    bit field of flags.

Parameters:

    Input:

        Pointer to USB_HUB_CAPABILITIES structure.

    Output:

        Pointer to USB_HUB_CAPABILITIES structure:

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_HUB_CAPABILITIES               hubCapabilities;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &hubCapabilities,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                0,
                                                InputBufferLength,
                                                NULL,
                                                sizeof(USB_HUB_CAPABILITIES),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        RtlZeroMemory(hubCapabilities, OutputBufferLength);

        if (hubFdoContext->HubInfoFromParent.HubFlags.HubIsHighSpeedCapable == 1) {
            hubCapabilities->HubIs2xCapable = 1;
        } else {
            hubCapabilities->HubIs2xCapable = 0;
        }

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_HUB_CAPABILITIES_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_HUB_CAPABILITIES_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                0,
                status);
        }

        WdfRequestCompleteWithInformation(Request,
                                          status,
                                          sizeof(USB_HUB_CAPABILITIES));
    }
}


VOID
HUBFDO_IoctlGetHubCapsEx(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns the capabilities for a particular USB hub.  The capabilities are a
    bit field of flags.

Parameters:

    Input:

        Pointer to USB_HUB_CAPABILITIES_EX structure.

    Output:

        Pointer to USB_HUB_CAPABILITIES_EX structure:

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_HUB_CAPABILITIES_EX            hubCapabilitiesEx;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &hubCapabilitiesEx,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                0,
                                                InputBufferLength,
                                                NULL,
                                                sizeof(USB_HUB_CAPABILITIES_EX),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        RtlZeroMemory(hubCapabilitiesEx, OutputBufferLength);


        if (hubFdoContext->HubInfoFromParent.HubFlags.HubIsHighSpeedCapable == 1) {
            hubCapabilitiesEx->CapabilityFlags.HubIsHighSpeedCapable = 1;
        } else {
            hubCapabilitiesEx->CapabilityFlags.HubIsHighSpeedCapable = 0;
        }

        if (hubFdoContext->HubInformation.HubFlags.IsMultiTtHub == 1) {

            hubCapabilitiesEx->CapabilityFlags.HubIsMultiTt = 1;
            hubCapabilitiesEx->CapabilityFlags.HubIsMultiTtCapable = 1;
        }

        if ((hubFdoContext->HubParentInterface.HubDepth == 0) ||
            (hubFdoContext->HubParentInterface.HubSpeed == UsbHighSpeed)) {

            hubCapabilitiesEx->CapabilityFlags.HubIsHighSpeed = 1;
        }

        if (hubFdoContext->HubParentInterface.HubDepth == 0) {

            hubCapabilitiesEx->CapabilityFlags.HubIsRoot = 1;
        }

        if (hubFdoContext->HubInformation.HubFlags.WakeOnConnect == 1) {

            hubCapabilitiesEx->CapabilityFlags.HubIsArmedWakeOnConnect = 1;
        }

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_HUB_CAPABILITIES_EX_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_HUB_CAPABILITIES_EX_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                0,
                status);
        }

        WdfRequestCompleteWithInformation(Request,
                                          status,
                                          sizeof(USB_HUB_CAPABILITIES_EX));
    }
}


VOID
HUBFDO_IoctlGetNodeConnectionName(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns the symbolic link name associated with the connection in the
    buffer passed in.  This API only returns a name if the connection is
    for a USB hub. If the connection if for a device or the connection is
    empty a NULL NodeName is returned.

    This API always succeeds as long as the inputbuffer is >=
    sizeof(USB_NODE_CONNECTION_NAME), even if the entire name will not fit.

Parameters:


Input:

    Pointer to a USB_NODE_CONNECTION_NAME structure:

        ConnectionIndex - this is the 1 based port number in the case of a hub.

Output:

    Pointer to a USB_NODE_CONNECTION_NAME structure:

        ActualLength - actual length of this entire structure including name.

        NodeName - unicode symbolic name for this node, including NULL.
--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_NODE_CONNECTION_NAME           nodeConnectionName;
    PHUB_PDO_CONTEXT                    pdoContext;
    ULONG                               portNumber;
    WDFDEVICE                           childDevice;
    WDFSTRING                           symbolicLinkName;
    size_t                              bytesRemaining;
    UNICODE_STRING                      symbolicLinkNameUnicode;
    size_t                              symbolicLinkNameLength;
    PWCHAR                              currentCharacter;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;
        portNumber = 0;

        pdoContext = NULL;

        symbolicLinkName = NULL;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &nodeConnectionName,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(ULONG),
                                                InputBufferLength,
                                                nodeConnectionName,
                                                sizeof(USB_NODE_CONNECTION_NAME),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        portNumber = (USHORT) nodeConnectionName->ConnectionIndex;

        WdfFdoLockStaticChildListForIteration(hubFdoContext->HubFdoDevice);

        childDevice = HUBFDO_FindDeviceForPort(hubFdoContext, portNumber);

        if (childDevice != NULL) {
            pdoContext = GetHubPdoContext(childDevice);
        }

        if ((childDevice == NULL) ||
            (pdoContext->AssociatedDSM->DeviceFlags.DeviceIsHub == 0)) {

            WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

            nodeConnectionName->ActualLength = sizeof(USB_NODE_CONNECTION_NAME);
            nodeConnectionName->NodeName[0] = UNICODE_NULL;

            WdfRequestSetInformation(Request, sizeof(USB_NODE_CONNECTION_NAME));

            status = STATUS_SUCCESS;
            LEAVE;
        }

        RtlZeroMemory(nodeConnectionName, OutputBufferLength);

        //
        // restore the original connection index
        //
        nodeConnectionName->ConnectionIndex = portNumber;

        symbolicLinkName = NULL;
        status = HUBMISC_GetDeviceInterfacePath((LPGUID)&GUID_DEVINTERFACE_USB_HUB,
                                                childDevice,
                                                &symbolicLinkName,
                                                hubFdoContext->IfrLog);

        WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        WdfStringGetUnicodeString(symbolicLinkName, &symbolicLinkNameUnicode);

        //
        // Determine the space available to copy the symbolic name
        //
        bytesRemaining = OutputBufferLength - (size_t)FIELD_OFFSET(USB_NODE_CONNECTION_NAME, NodeName);

        currentCharacter = HUBMISC_StripSymbolicNamePrefix(&symbolicLinkNameUnicode,
                                                           &symbolicLinkNameLength);

        nodeConnectionName->ActualLength = (ULONG)(symbolicLinkNameLength + sizeof(USB_NODE_CONNECTION_NAME));

        if (symbolicLinkNameLength <= bytesRemaining) {

            RtlCopyMemory(nodeConnectionName->NodeName,
                          currentCharacter,
                          symbolicLinkNameLength);

            WdfRequestSetInformation(Request, nodeConnectionName->ActualLength);

            status = STATUS_SUCCESS;
            LEAVE;

        } else {

            nodeConnectionName->NodeName[0] = UNICODE_NULL;

            WdfRequestSetInformation(Request, sizeof(USB_NODE_CONNECTION_NAME));

            status = STATUS_SUCCESS;
            LEAVE;

        }

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_NAME_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_NAME_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                portNumber,
                status);
        }

        if (symbolicLinkName != NULL) {

            WdfObjectDelete(symbolicLinkName);
        }

        WdfRequestComplete(Request, status);
    }
}

VOID
HUBFDO_IoctlGetDescriptorFromNodeConnection(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:


Parameters:


Input:


Output:

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_DESCRIPTOR_REQUEST             descriptorRequest;
    PHUB_PDO_CONTEXT                    pdoContext;
    ULONG                               portNumber;
    WDFDEVICE                           childDevice;
    struct _URB_CONTROL_TRANSFER_EX     *urb;
    IO_STACK_LOCATION                   ioStackLocation;
    PUSER_MODE_IOCTL_QUEUE_CONTEXT      queueContext;
    PUSB_DEFAULT_PIPE_SETUP_PACKET      setupPacket;
    PDEVICE_CONTEXT                     deviceContext;
    ULONG                               descriptorSize;
    PVOID                               sourceDescriptor;
    BOOLEAN                             descriptorIsCached;
    BOOLEAN                             activityIdValid;
    GUID                                activityId;
    BOOLEAN                             eventQueued;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;
        activityIdValid = FALSE;
        portNumber = 0;
        pdoContext = NULL;
        deviceContext = NULL;
        descriptorSize = 0;
        eventQueued = FALSE;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &descriptorRequest,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(ULONG),
                                                InputBufferLength,
                                                descriptorRequest,
                                                sizeof(USB_DESCRIPTOR_REQUEST) + 1,
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        portNumber = (USHORT) descriptorRequest->ConnectionIndex;

        //
        // Zero out the descriptor buffer if it exists
        //
        if (OutputBufferLength - FIELD_OFFSET(USB_DESCRIPTOR_REQUEST, Data) > 0) {
            RtlZeroMemory(&descriptorRequest->Data, OutputBufferLength - FIELD_OFFSET(USB_DESCRIPTOR_REQUEST, Data));
        }

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION_START()) {

            if NT_SUCCESS(HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId)) {
                activityIdValid = TRUE;
            }

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION_START(
                (activityIdValid) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                portNumber);
        }

        WdfFdoLockStaticChildListForIteration(hubFdoContext->HubFdoDevice);
        childDevice = HUBFDO_FindDeviceForPort(hubFdoContext, portNumber);

        if (childDevice != NULL) {
            pdoContext = GetHubPdoContext(childDevice);
            deviceContext = pdoContext->AssociatedDSM;
        }

        if ((childDevice == NULL) || (deviceContext == NULL) ||
            (deviceContext->DeviceStateFlags.DeviceIsKnown == 0)) {

            status = STATUS_NO_SUCH_DEVICE;

            WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);
            LEAVE;
        }

        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET) &descriptorRequest->SetupPacket;

        //
        // COMPAT: Explicity set USB_REQUEST_GET_DESCRIPTOR in the setup packet.
        //         Also hard-code for standard requests only, to prevents the API
        //         from ever being used for accessing vendor or class requests.
        //
        setupPacket->bRequest = USB_REQUEST_GET_DESCRIPTOR;
        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_DEVICE;
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_DEVICE_TO_HOST;
        setupPacket->bmRequestType.Reserved = 0;

        descriptorIsCached = FALSE;
        sourceDescriptor = NULL;

        //
        // See if we can return a cached descriptor
        //
        switch (setupPacket->wValue.HiByte) {
        case USB_DEVICE_DESCRIPTOR_TYPE:
            descriptorIsCached = TRUE;
            sourceDescriptor = &deviceContext->DeviceDescriptor;
            descriptorSize = min(sizeof(USB_DEVICE_DESCRIPTOR),
                                 (ULONG)OutputBufferLength - sizeof(USB_DESCRIPTOR_REQUEST));
            break;

        case USB_CONFIGURATION_DESCRIPTOR_TYPE:
            //
            // LowByte is the descriptor index. We only cache the default
            // configuration which is at index 0. Anything other than the
            // default, needs to go the device
            //
            if ((deviceContext->ConfigurationDescriptor != NULL) &&
                (setupPacket->wValue.LowByte == 0)) {
                descriptorIsCached = TRUE;
                sourceDescriptor = deviceContext->ConfigurationDescriptor;
                descriptorSize = min(deviceContext->ConfigurationDescriptor->wTotalLength,
                                     (ULONG)OutputBufferLength - sizeof(USB_DESCRIPTOR_REQUEST));
            }
            break;

        case USB_BOS_DESCRIPTOR_TYPE:
            if (deviceContext->BOSDescriptor != NULL) {
                descriptorIsCached = TRUE;
                sourceDescriptor = deviceContext->BOSDescriptor;
                descriptorSize = min(deviceContext->BOSDescriptor->wTotalLength,
                                     (ULONG)OutputBufferLength - sizeof(USB_DESCRIPTOR_REQUEST));
            }
            break;

        default:
            break;
        }

        if (descriptorIsCached == TRUE) {

            RtlCopyMemory(descriptorRequest->Data,
                          sourceDescriptor,
                          descriptorSize);

            WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);
            LEAVE;
        }

        if ((setupPacket->wValue.HiByte == USB_STRING_DESCRIPTOR_TYPE) &&
            (setupPacket->wValue.LowByte != 0) &&
            (setupPacket->wValue.LowByte == deviceContext->DeviceDescriptor.iSerialNumber) &&
            (setupPacket->wIndex.W == 0x409)) {

            if (deviceContext->DeviceFlags.SerialNumber == 1) {
                //
                // Asking for serial number string descriptor.  We will need to re-construct it from the 
                // USB_ID_STRING
                //
                USHORT stringDescriptorLength;
                USHORT decorationSize;

                decorationSize = 0;

                if (deviceContext->DeviceStateFlags.DecorateSerialNumber == 1) {
                    decorationSize = USB_SERIAL_NUMBER_DECORATION_SIZE;
                }

                stringDescriptorLength =
                    (USHORT)(FIELD_OFFSET(USB_STRING_DESCRIPTOR, bString) +  // beginning offset of the string field
                    deviceContext->SerialNumberId.LengthInBytes -            // plus length of the NULL-terminated string
                    decorationSize -                                         // minus decoration
                    sizeof(WCHAR));                                          // minus the terminating NULL

                descriptorSize = min(stringDescriptorLength,
                    (ULONG)OutputBufferLength - sizeof(USB_DESCRIPTOR_REQUEST));

                if (descriptorSize <= setupPacket->wLength) {
                    PUSB_STRING_DESCRIPTOR serialNumberDesc;

                    serialNumberDesc = (PUSB_STRING_DESCRIPTOR)descriptorRequest->Data;
                    serialNumberDesc->bLength = ((UCHAR)((descriptorSize) & 0xff));
                    serialNumberDesc->bDescriptorType = USB_STRING_DESCRIPTOR_TYPE;
                    RtlCopyMemory(serialNumberDesc->bString,
                        deviceContext->SerialNumberId.Buffer + (decorationSize / sizeof(WCHAR)),
                        deviceContext->SerialNumberId.LengthInBytes - decorationSize - sizeof(WCHAR));

                    WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);
                    LEAVE;
                }
            } else {
                status = STATUS_UNSUCCESSFUL;
                WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);
                LEAVE;
            }
        }

        //
        // We need to send the descriptor request to the device
        //
        queueContext = GetUserModeIoctlContext(Queue);

        RtlCopyMemory(&queueContext->ControlRequest.Urb.SetupPacket,
                      setupPacket,
                      sizeof(USB_DEFAULT_PIPE_SETUP_PACKET));

        urb = &queueContext->ControlRequest.Urb;
        urb->Hdr.UsbdDeviceHandle = pdoContext->AssociatedDSM->UsbDevice;

        //
        // Take a reference on the target DSM so that it does not go away
        // after we release the child lock. This reference will be released
        // when the user mode request is completed
        //
        WdfObjectReferenceWithTag(WdfObjectContextGetObject(deviceContext),
                                  (PVOID) UserModeFdoRequest);

        WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

        //
        // Set up the control transfer
        //
        urb->Hdr.Length = sizeof(struct _URB_CONTROL_TRANSFER_EX);
        urb->Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER_EX;

        urb->TransferFlags = USBD_DEFAULT_PIPE_TRANSFER |
                             USBD_SHORT_TRANSFER_OK |
                             USBD_TRANSFER_DIRECTION_IN;

        urb->TransferBufferLength = (ULONG) OutputBufferLength - sizeof(USB_DESCRIPTOR_REQUEST);
        urb->TransferBuffer = descriptorRequest->Data;
        urb->TransferBufferMDL = NULL;

        urb->Timeout = CONTROL_TRANSFER_TIMEOUT;

        RtlZeroMemory(&ioStackLocation,  sizeof(ioStackLocation));
        ioStackLocation.MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        ioStackLocation.Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
        ioStackLocation.Parameters.Others.Argument1 = urb;

        WdfRequestWdmFormatUsingStackLocation(Request, &ioStackLocation);

        deviceContext->PreformattedGetDescriptorRequest = Request;

        //
        // This request needs to be sent by DSM so that it
        // can serialize it with other device operations.
        // DSM will either send the request to the controller
        // stack or will set the event directly
        //
        HUBSM_AddDsmEvent(deviceContext,
                          DsmEventFDORequestGetDescriptor);
        eventQueued = TRUE;

    } FINALLY {
        if (!eventQueued) {
            //
            // if we did not end up queueing an event in
            // DSM, then we need to complete it here. Otherwise
            // request will be processed by DSM
            //

            HUBFDO_CompleteGetDescriptorRequest(hubFdoContext,
                                                portNumber,
                                                Request,
                                                status,
                                                descriptorSize);
        }

    }
}

VOID
HUBFDO_CompleteGetDescriptorRequest(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        ULONG               PortNumber,
    __in
        WDFREQUEST          Request,
    __in
        NTSTATUS            Status,
    __in
        ULONG               DescriptorSize
    )
/*++
Routine Description:

    Completes the user mode Get Descriptor request
    with the given status.

Parameters:


--*/
{
    BOOLEAN                             activityIdValid;
    GUID                                activityId;

    TRY {

        activityIdValid = FALSE;

        DbgTraceTo(HubFdoContext->IfrLog,
                   TL_INFO,
                   Hub,
                   "Get Device descriptor IOCTL completed with %!STATUS!",
                   Status);

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION_COMPLETE()) {

            if NT_SUCCESS(HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId)) {
                activityIdValid = TRUE;
            }

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION_COMPLETE(
                (activityIdValid) ? &activityId : 0,
                HubFdoContext->HubParentInterface.Hub,
                PortNumber,
                Status);
        }

        if (!NT_SUCCESS(Status)) {

            WdfRequestComplete(Request, Status);

        } else {

            WdfRequestCompleteWithInformation(Request,
                                              Status,
                                              DescriptorSize + sizeof(USB_DESCRIPTOR_REQUEST));
        }


    } FINALLY {

    }
}

VOID
HUBFDO_IoctlGetHubInformationEx(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns the information about the hub.

Parameters:

    Input:

        Pointer to USB_HUB_INFORMATION_EX structure.

    Output:

        Pointer to USB_HUB_INFORMATION_EX structure:

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_HUB_INFORMATION_EX             hubInformationEx;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &hubInformationEx,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                0,
                                                InputBufferLength,
                                                NULL,
                                                sizeof(USB_HUB_INFORMATION_EX),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        RtlZeroMemory(hubInformationEx, OutputBufferLength);

        hubInformationEx->HighestPortNumber = hubFdoContext->HubInformation.NumberOfPorts;

        switch (hubFdoContext->HsmContext.SubSmFlags) {
        case HsmSubSmFlagHubRoot:
            hubInformationEx->HubType = UsbRootHub;
            break;

        case HsmSubSmFlagHub20:
            hubInformationEx->HubType = Usb20Hub;

            RtlCopyMemory(&hubInformationEx->u.UsbHubDescriptor,
                          &hubFdoContext->HubDescriptor.Hub20Descriptor,
                          sizeof(USB_HUB_DESCRIPTOR));
            break;

        case HsmSubSmFlagHub30:
            hubInformationEx->HubType = Usb30Hub;

            RtlCopyMemory(&hubInformationEx->u.Usb30HubDescriptor,
                          &hubFdoContext->HubDescriptor.Hub30Descriptor,
                          sizeof(USB_30_HUB_DESCRIPTOR));
            break;

        default:
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Invalid Hub Type");
            break;
        }

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_HUB_INFORMATION_EX_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_HUB_INFORMATION_EX_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                0,
                status);
        }

        WdfRequestCompleteWithInformation(Request,
                                          status,
                                          sizeof(USB_HUB_INFORMATION_EX));
    }
}


VOID
HUBFDO_IoctlGetPortConnectorProperties(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns information about a port and associated connector.

Parameters:

    Input:

        Pointer to USB_PORT_CONNECTOR_PROPERTIES structure.

    Output:

        Pointer to USB_PORT_CONNECTOR_PROPERTIES structure:

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_PORT_CONNECTOR_PROPERTIES      portConnectorProperties;
    ULONG                               portNumber;
    USHORT                              companionIndex;
    PPORT_CONTEXT                       portContext;
    PPORT_CONTEXT                       companionPortContext;
    size_t                              bytesLeft;
    size_t                              symbolicLinkNameLength;
    PWCHAR                              currentCharacter;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;

        portNumber = 0;
        companionIndex = 0;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &portConnectorProperties,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(USB_PORT_CONNECTOR_PROPERTIES),
                                                InputBufferLength,
                                                portConnectorProperties,
                                                sizeof(USB_PORT_CONNECTOR_PROPERTIES),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        portNumber = (USHORT) portConnectorProperties->ConnectionIndex;
        companionIndex = portConnectorProperties->CompanionIndex;

        RtlZeroMemory(portConnectorProperties, OutputBufferLength);

        //
        // restore the original connection and companion indexes
        //
        portConnectorProperties->ConnectionIndex = portNumber;
        portConnectorProperties->CompanionIndex = companionIndex;

        //
        // Set default for actual length
        //
        portConnectorProperties->ActualLength = sizeof(USB_PORT_CONNECTOR_PROPERTIES);

        portContext = HUBFDO_FindPortContextFromNumber(hubFdoContext, LOWORD(portNumber));

        if (portContext == NULL) {

            status = STATUS_INVALID_PARAMETER;

            WdfRequestSetInformation(Request, sizeof(USB_PORT_CONNECTOR_PROPERTIES));
            LEAVE;
        }

        if (portContext->PortInformation.PortFlags.PortFlagDebugCapable == 1) {
            portConnectorProperties->UsbPortProperties.PortIsDebugCapable = 1;
        }

        if (portContext->PortInformation.PortFlags.PortFlagRemovable == 1) {
            portConnectorProperties->UsbPortProperties.PortIsUserConnectable = 1;
        }

        if (portContext->PortInformation.PortFlags.PortFlagTypeCWithoutSwitch == 1) {
            portConnectorProperties->UsbPortProperties.PortHasMultipleCompanions = 1;
        }

        if (portContext->AcpiUpcPackage.PortConnectorType == ACPI_UPC_TYPE_C_USB_20_ONLY ||
            portContext->AcpiUpcPackage.PortConnectorType == ACPI_UPC_TYPE_C_USB_20_AND_SS_WITH_SWITCH ||
            portContext->AcpiUpcPackage.PortConnectorType == ACPI_UPC_TYPE_C_USB_20_AND_SS_WITHOUT_SWITCH) {

            portConnectorProperties->UsbPortProperties.PortConnectorIsTypeC = 1;
        }


        if (portConnectorProperties->CompanionIndex > 1) {
            WdfRequestSetInformation(Request, sizeof(USB_PORT_CONNECTOR_PROPERTIES));
            LEAVE;
        }

        bytesLeft = (OutputBufferLength - sizeof(USB_PORT_CONNECTOR_PROPERTIES));

        //
        // The connector map should be locked for as long as the companion port context
        // is being referenced.  Once the connector map lock is released the companion
        // port context may be freed at any time.
        //
        HUBCONNECTOR_LockConnectorMap();

        companionPortContext = HUBCONNECTOR_GetCompanionPort(portContext, 
                                                             portConnectorProperties->CompanionIndex);

        if (companionPortContext != NULL) {
            PUNICODE_STRING    symbolicLinkName;

            symbolicLinkName = &companionPortContext->HubFdoContext->HubSymbolicLinkNameUnicode;

            portConnectorProperties->CompanionPortNumber =
                companionPortContext->PortInformation.PortNumber;

            currentCharacter = HUBMISC_StripSymbolicNamePrefix(symbolicLinkName,
                                                               &symbolicLinkNameLength);

            if (currentCharacter != NULL) {

                portConnectorProperties->ActualLength = (ULONG)(symbolicLinkNameLength +
                                                        sizeof(USB_PORT_CONNECTOR_PROPERTIES));

                if (bytesLeft >= symbolicLinkNameLength) {
                    RtlCopyMemory(&portConnectorProperties->CompanionHubSymbolicLinkName,
                                  currentCharacter,
                                  symbolicLinkNameLength);

                    WdfRequestSetInformation(Request, OutputBufferLength);

                } else {
                    portConnectorProperties->CompanionHubSymbolicLinkName[0] = UNICODE_NULL;
                    WdfRequestSetInformation(Request, sizeof(USB_PORT_CONNECTOR_PROPERTIES));
                }

            } else {
                portConnectorProperties->CompanionHubSymbolicLinkName[0] = UNICODE_NULL;
                WdfRequestSetInformation(Request, sizeof(USB_PORT_CONNECTOR_PROPERTIES));
            }
        } else {
            WdfRequestSetInformation(Request, sizeof(USB_PORT_CONNECTOR_PROPERTIES));
        }

        HUBCONNECTOR_UnlockConnectorMap();

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                portNumber,
                status);
        }

        WdfRequestComplete(Request, status);
    }
}


VOID
HUBFDO_IoctlGetNodeConnectionInformationExV2
(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:

    Returns information about a port.

Parameters:

    Input:

        Pointer to USB_NODE_CONNECTION_INFORMATION_EX_V2 structure.

    Output:

        Pointer to USB_NODE_CONNECTION_INFORMATION_EX_V2 structure:

--*/
{
    NTSTATUS                                status;
    PHUB_FDO_CONTEXT                        hubFdoContext;
    PUSB_NODE_CONNECTION_INFORMATION_EX_V2  nodeConnectionInfoExV2;
    ULONG                                   portNumber;
    PPORT_CONTEXT                           portContext;
    USB_PROTOCOLS                           protocolsSupportedByClient;
    WDFDEVICE                               childDevice;
    PHUB_PDO_CONTEXT                        pdoContext;
    PDEVICE_CONTEXT                         deviceContext;

    PAGED_CODE();

    TRY {

        status = STATUS_SUCCESS;

        portNumber = 0;

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &nodeConnectionInfoExV2,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2),
                                                InputBufferLength,
                                                nodeConnectionInfoExV2,
                                                sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2),
                                                OutputBufferLength);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (nodeConnectionInfoExV2->Length < sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2)) {
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        portNumber = (USHORT) nodeConnectionInfoExV2->ConnectionIndex;
        protocolsSupportedByClient = nodeConnectionInfoExV2->SupportedUsbProtocols;

        RtlZeroMemory(nodeConnectionInfoExV2, OutputBufferLength);

        //
        // restore the original connection index
        //
        nodeConnectionInfoExV2->ConnectionIndex = portNumber;

        //
        // For initial version of this API the client must support the USB 3.0 protocol
        //
        if (protocolsSupportedByClient.Usb300 != 1) {
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        //
        // restore the original connection index
        //
        nodeConnectionInfoExV2->ConnectionIndex = portNumber;

        //
        // Set the length supported by this version of the hub driver
        //
        nodeConnectionInfoExV2->Length = sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2);

        WdfRequestSetInformation(Request, sizeof(USB_NODE_CONNECTION_INFORMATION_EX_V2));

        portContext = HUBFDO_FindPortContextFromNumber(hubFdoContext, LOWORD(portNumber));

        if (portContext == NULL) {
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        switch (portContext->PortInformation.PortProtocol) {
        case PortProtocolUsb20:
            nodeConnectionInfoExV2->SupportedUsbProtocols.Usb110 = 1;

            if ((hubFdoContext->HubParentInterface.HubSpeed == UsbHighSpeed) ||
                (hubFdoContext->HsmContext.SubSmFlags == HsmSubSmFlagHubRoot)) {

                nodeConnectionInfoExV2->SupportedUsbProtocols.Usb200 = 1;
            }
            break;

        case PortProtocolUsb30:
            nodeConnectionInfoExV2->SupportedUsbProtocols.Usb300 = 1;
            break;

        //
        // NOTE: For future versions of USB, we will need to check here for the highest client
        // supported protocol and conditionally mark them as supported if the client understands them
        //

        default:
            DbgTraceTo(hubFdoContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Invalid Port Protocol (%x) for port %d",
                       portContext->PortInformation.PortProtocol,
                       portNumber);
            break;
        }

        WdfFdoLockStaticChildListForIteration(hubFdoContext->HubFdoDevice);

        childDevice = HUBFDO_FindDeviceForPort(hubFdoContext, portNumber);

        if (childDevice == NULL) {
            WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);
            LEAVE;
        }

        pdoContext = GetHubPdoContext(childDevice);
        deviceContext = pdoContext->AssociatedDSM;

        if (deviceContext != NULL) {
            if (deviceContext->DeviceFlags.DeviceIsSuperSpeedCapable == 1) {
                nodeConnectionInfoExV2->Flags.DeviceIsSuperSpeedCapableOrHigher = 1;
            }

            nodeConnectionInfoExV2->Flags.DeviceIsSuperSpeedPlusCapableOrHigher = HUBMISC_IsDeviceSuperSpeedPlusCapableOrHigher(deviceContext) ? 1 : 0;

            if (portContext->PortInformation.PortProtocol == PortProtocolUsb30) {
                nodeConnectionInfoExV2->Flags.DeviceIsOperatingAtSuperSpeedOrHigher = 1;

                nodeConnectionInfoExV2->Flags.DeviceIsOperatingAtSuperSpeedPlusOrHigher = HUBMISC_IsDeviceOperatingAtSuperSpeedPlusOrHigher(deviceContext) ? 1 : 0;
            }
        }

        WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_V2_COMPLETE()) {
            NTSTATUS    activityIdStatus;
            GUID        activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_V2_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                portNumber,
                status);
        }

        WdfRequestComplete(Request, status);
    }
}


VOID
HUBFDO_IoctlCyclePort(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength
    )
/*++
Routine Description:


Parameters:

    Input:
        USB_CYCLE_PORT_PARAMS structure

    Output:
        USB_CYCLE_PORT_PARAMS filled in as appropriate

--*/
{
    NTSTATUS                            status;
    PHUB_FDO_CONTEXT                    hubFdoContext;
    PUSB_CYCLE_PORT_PARAMS              cyclePortParams;
    ULONG                               portNumber;
    WDFDEVICE                           childDevice;
    PDEVICE_CONTEXT                     deviceContext;
    PHUB_PDO_CONTEXT                    pdoContext;
    BOOLEAN                             updateRequired;
    BOOLEAN                             activityIdValid;
    GUID                                activityId;

    PAGED_CODE();

    TRY {

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        portNumber = 0;
        activityIdValid = FALSE;
        cyclePortParams = NULL;
        updateRequired = FALSE;

        status = WdfRequestRetrieveOutputBuffer(Request,
                                                OutputBufferLength,
                                                &cyclePortParams,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to retrieve Output Buffer %!STATUS!", status);
            LEAVE;
        }

        status = HUBFDO_IoctlValidateParameters(hubFdoContext,
                                                sizeof(ULONG),
                                                InputBufferLength,
                                                cyclePortParams,
                                                sizeof(USB_CYCLE_PORT_PARAMS),
                                                OutputBufferLength);

        cyclePortParams->StatusReturned = USBD_STATUS_SUCCESS;

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        portNumber = (USHORT) cyclePortParams->ConnectionIndex;

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_HUB_CYCLE_PORT_START()) {

            if (NT_SUCCESS(HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId))) {
                activityIdValid = TRUE;
            }

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_HUB_CYCLE_PORT_START(
                (activityIdValid) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                portNumber);
        }

        WdfFdoLockStaticChildListForIteration(hubFdoContext->HubFdoDevice);

        childDevice = HUBFDO_FindDeviceForPort(hubFdoContext, portNumber);

        if (childDevice == NULL) {

            WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);
            status = STATUS_NO_SUCH_DEVICE;
            cyclePortParams->StatusReturned = (ULONG)USBD_STATUS_DEVICE_GONE;
            LEAVE;
        }

        pdoContext = GetHubPdoContext(childDevice);
        deviceContext = pdoContext->AssociatedDSM;

        if (deviceContext == NULL) {

            WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);
            status = STATUS_NO_SUCH_DEVICE;
            cyclePortParams->StatusReturned = (ULONG)USBD_STATUS_DEVICE_GONE;
            LEAVE;
        }

        HUBREG_UpdateUxdSettings(hubFdoContext,
                                 deviceContext,
                                 &updateRequired);


        //
        // Take a reference on the target DSM so that it does not go away
        // after we release the child lock
        //
        WdfObjectReferenceWithTag(WdfObjectContextGetObject(deviceContext),
                                  (PVOID) UserModeFdoRequest);



        WdfFdoUnlockStaticChildListFromIteration(hubFdoContext->HubFdoDevice);

        //
        // See if the calling process is at admin level or higher
        //
        status = HUBMISC_VerifyCallerIsAdmin(hubFdoContext->IfrLog);

        if (NT_SUCCESS(status) || (updateRequired == TRUE)) {

            status = WdfDeviceStopIdle(hubFdoContext->HubFdoDevice, TRUE);

            if (!NT_SUCCESS(status)) {
                DbgTraceTo(hubFdoContext->IfrLog, TL_ERROR, Hub, "Unable to stop hub idle %!STATUS!", status);
                LEAVE;
            }

            //
            // Cycle the port. But we need to ensure that there is only
            // cycle port queued to prevent PSM queue from getting blown
            // Once a cycle port is queued, all subsequent cycle ports
            // are no-op anyways
            //
            if (InterlockedCompareExchange(&pdoContext->CyclePortInProgress,
                                           1,
                                           0) == 0) {


                deviceContext->PortContext->PsmContext.AddEvent(deviceContext->PortContext,
                                                                PsmEventDeviceRequestCycle);
            }

            WdfDeviceResumeIdle(hubFdoContext->HubFdoDevice);
        }


        WdfObjectDereferenceWithTag(WdfObjectContextGetObject(deviceContext),
                                    (PVOID) UserModeFdoRequest);


    } FINALLY {

        EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_HUB_CYCLE_PORT_COMPLETE(
           (activityIdValid) ? &activityId : 0,
            hubFdoContext->HubParentInterface.Hub,
            portNumber,
            status);

        WdfRequestCompleteWithInformation(Request,
                                          status,
                                          sizeof(USB_CYCLE_PORT_PARAMS));
    }
}

VOID
HUBFDO_IoctlResetHub(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request
    )
/*++
Routine Description:

    This function resets the hub in response to IOCTL_USB_RESET_HUB
    If the process calling this IOCTL has admin privilege only then the hub
    will be reset. The hub will not be reset if it is not in a working state.

Parameters:


--*/
{
    NTSTATUS            status;
    PHUB_FDO_CONTEXT    hubFdoContext;

    PAGED_CODE();

    TRY {

        hubFdoContext = GetHubFdoContext(WdfIoQueueGetDevice(Queue));

        status = HUBMISC_VerifyCallerIsAdmin(hubFdoContext->IfrLog);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (!HUBMUX_CheckAndQueueIfResetHubIsEnabled(GetHubFdoContext(WdfIoQueueGetDevice(Queue)))){
            status = STATUS_UNSUCCESSFUL;
        }

    } FINALLY {

        if (EventEnabledUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_RESET_HUB_COMPLETE()) {
            NTSTATUS            activityIdStatus;
            GUID                activityId;

            activityIdStatus = HUBMISC_GetActivityIdIrp(WdfRequestWdmGetIrp(Request), &activityId);

            EventWriteUSBHUB3_ETW_EVENT_HUB_IOCTL_USB_RESET_HUB_COMPLETE(
                NT_SUCCESS(activityIdStatus) ? &activityId : 0,
                hubFdoContext->HubParentInterface.Hub,
                0,
                status);
        }

        WdfRequestComplete(Request, STATUS_SUCCESS);
    }
}

VOID
HUBFDO_EvtIoDeviceControl(
    __in
        WDFQUEUE        Queue,
    __in
        WDFREQUEST      Request,
    __in
        size_t          OutputBufferLength,
    __in
        size_t          InputBufferLength,
    __in
        ULONG           IoControlCode
    )
/*++

Routine Description:

    Handles user-mode IOCTLs for the hub FDO

Return Value:

    VOID

--*/
{
    WDF_REQUEST_SEND_OPTIONS options;
    BOOLEAN                  requestSent;

    PAGED_CODE();

    TRY {

        switch(IoControlCode) {
        case IOCTL_USB_GET_PORT_STATUS:
            HUBFDO_IoctlGetPortStatus(Queue,
                                      Request,
                                      OutputBufferLength,
                                      InputBufferLength);
            break;

        case IOCTL_USB_GET_NODE_INFORMATION:
            HUBFDO_IoctlGetNodeInformation(Queue,
                                           Request,
                                           OutputBufferLength,
                                           InputBufferLength);
            break;

        case IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME:
            HUBFDO_IoctlGetNodeConnectionDriverKeyName(Queue,
                                                       Request,
                                                       OutputBufferLength,
                                                       InputBufferLength);
            break;

        case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION:
            HUBFDO_IoctlGetNodeConnectionInfo(Queue,
                                              Request,
                                              OutputBufferLength,
                                              InputBufferLength,
                                              FALSE);
            break;

        case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX:
            HUBFDO_IoctlGetNodeConnectionInfo(Queue,
                                              Request,
                                              OutputBufferLength,
                                              InputBufferLength,
                                              TRUE);
            break;

        case IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES:
            HUBFDO_IoctlGetNodeConnectionAttributes(Queue,
                                                    Request,
                                                    OutputBufferLength,
                                                    InputBufferLength);
            break;

        case IOCTL_USB_GET_NODE_CONNECTION_NAME:
            HUBFDO_IoctlGetNodeConnectionName(Queue,
                                              Request,
                                              OutputBufferLength,
                                              InputBufferLength);
            break;

        case IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION:
            HUBFDO_IoctlGetDescriptorFromNodeConnection(Queue,
                                                        Request,
                                                        OutputBufferLength,
                                                        InputBufferLength);
            break;

        case IOCTL_USB_GET_HUB_CAPABILITIES:
            HUBFDO_IoctlGetHubCaps(Queue,
                                   Request,
                                   OutputBufferLength,
                                   InputBufferLength);
            break;

        case IOCTL_USB_GET_HUB_CAPABILITIES_EX:
            HUBFDO_IoctlGetHubCapsEx(Queue,
                                     Request,
                                     OutputBufferLength,
                                     InputBufferLength);
            break;

        case IOCTL_USB_HUB_CYCLE_PORT:
            HUBFDO_IoctlCyclePort(Queue,
                                  Request,
                                  OutputBufferLength,
                                  InputBufferLength);
            break;

        case IOCTL_USB_RESET_HUB:
            HUBFDO_IoctlResetHub(Queue,
                                 Request);
            break;

        case IOCTL_USB_GET_HUB_INFORMATION_EX:
            HUBFDO_IoctlGetHubInformationEx(Queue,
                                            Request,
                                            OutputBufferLength,
                                            InputBufferLength);
            break;

        case IOCTL_USB_GET_PORT_CONNECTOR_PROPERTIES:
            HUBFDO_IoctlGetPortConnectorProperties(Queue,
                                                   Request,
                                                   OutputBufferLength,
                                                   InputBufferLength);
            break;

        case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX_V2:
            HUBFDO_IoctlGetNodeConnectionInformationExV2(Queue,
                                                         Request,
                                                         OutputBufferLength,
                                                         InputBufferLength);
            break;

        default:
            WDF_REQUEST_SEND_OPTIONS_INIT(&options,
                                          WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

            requestSent = WdfRequestSend(Request,
                                         WdfDeviceGetIoTarget(WdfIoQueueGetDevice(Queue)),
                                         &options);

            if (requestSent == FALSE) {
                WdfRequestComplete(Request, WdfRequestGetStatus(Request));
            }
        }
    } FINALLY {

    }
}

VOID
HUBFDO_LogHubWasResetOnResume (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Fires an ETW event indicating that the hub got
    reset on resume. This is to be used by the logo
    tests to determine if it is ok for the downstream
    device to disconnect and reconnect.

Return Value:

    VOID

--*/
{
    EventWriteUSBHUB3_ETW_EVENT_HUB_WAS_RESET_ON_RESUME(
        0,
        HubFdoContext->HubParentInterface.Hub);
}


VOID
HUBFDO_LogHubResetRecoveryWasInvoked (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Writes into registry information about how many
    times this hub has been reset for the lifetime of
    the devnode. It also writes the invalid port status
    if that is the reason for hub reset. This infromation
    will be read by the SQM engine.

Return Value:

    VOID

--*/
{

    PAGED_CODE();

    HubFdoContext->HubInformation.HubResetRecoveryCount++;

    //
    // We don't care about the returned status. If it
    // failed, then we cannot do anything. Inner function
    // will log the specific failure
    //
    HUBREG_WriteValueToHubHardwareKey(HubFdoContext,
                                      &g_HubInstHardResetCount,
                                      HubFdoContext->HubInformation.HubResetRecoveryCount);

    if (HubFdoContext->HubInformation.LastHubResetPortStatus.AsUlong32 != 0) {

        HUBREG_WriteValueToHubHardwareKey(HubFdoContext,
                                          &g_HubInstLastHubResetPortStatus,
                                          HubFdoContext->HubInformation.LastHubResetPortStatus.AsUlong32);

        HubFdoContext->HubInformation.LastHubResetPortStatus.AsUlong32 = 0;
    }
}

__drv_functionClass(POWER_SETTING_CALLBACK)
__drv_sameIRQL
NTSTATUS
HUBFDO_PowerSettingCallback (  
    _In_ 
        LPCGUID             SettingGuid,
    _In_reads_bytes_(ValueLength) 
        PULONG              NewValue,
    _In_ 
        ULONG               ValueLength,
    _Inout_opt_ 
        PHUB_FDO_CONTEXT    HubFdoContext
   )
/*++

Routine Description:

    Called by the power manager whenever one of the
    registered power setting changes.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                                status;
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS   idleSettings;
    _Analysis_assume_(HubFdoContext != NULL);

    TRY {

        status = STATUS_SUCCESS;

        //
        // We should not be receiving a callback if the hub is not
        // setup for selective suspend
        //
        if (HubFdoContext->HubInformation.HubFlags.S0IdleConfigured == 0) {
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_USB_SETTING_SELECTIVE_SUSPEND,
                             sizeof(GUID)) == sizeof(GUID)) {

            if (ValueLength < sizeof(ULONG)) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            if (*NewValue > 1) {
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            if (*NewValue == 1) {

                if (HubFdoContext->HubInformation.HubFlags.PowerReferenceAcquiredDueToGlobalSetting == 1) {

                    WdfDeviceResumeIdle(HubFdoContext->HubFdoDevice);
                    INTERLOCKED_CLEAR_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, PowerReferenceAcquiredDueToGlobalSetting);
                    DbgTraceTo(HubFdoContext->IfrLog, TL_INFO, Hub, "Releasing the power reference because the global idle setting is on");
                }

                LEAVE;
            }

            if (HubFdoContext->HubInformation.HubFlags.PowerReferenceAcquiredDueToGlobalSetting == 0 ) {

                DbgTraceTo(HubFdoContext->IfrLog, TL_INFO, Hub, "Acquiring a power reference because the global idle setting is off");

                status = WdfDeviceStopIdle(HubFdoContext->HubFdoDevice, FALSE);

                if (NT_SUCCESS(status)) {
                    INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, PowerReferenceAcquiredDueToGlobalSetting);
                } else {

                    status = STATUS_SUCCESS;
                    DbgTraceTo(HubFdoContext->IfrLog, TL_WARNING, Hub, "WdfDeviceStopIdle failed with status %!STATUS!", status);

                }

                LEAVE;
            }

            LEAVE;
        }


        if (RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_POWER_HUB_SELECTIVE_SUSPEND_TIMEOUT,
                             sizeof(GUID)) == sizeof(GUID)) {

            if (*NewValue == HubFdoContext->CurrentIdleTimeout) {
                LEAVE;
            }

            DbgTraceTo(HubFdoContext->IfrLog, TL_INFO, Hub, "Changing hub timeout to %d milliseconds because of the the global setting", *NewValue);

            HubFdoContext->CurrentIdleTimeout = *NewValue;

            WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&idleSettings,
                                                       IdleCanWakeFromS0);

            //
            // Change the timeout to the new value in miliseconds
            //
            idleSettings.IdleTimeout = *NewValue;

            //
            // Keep the rest of the setting same as original
            //
            idleSettings.UserControlOfIdleSettings = IdleAllowUserControl;
            idleSettings.Enabled = WdfUseDefault;
            idleSettings.DxState = PowerDeviceD2;

            status = WdfDeviceAssignS0IdleSettings(WdfObjectContextGetObject(HubFdoContext),
                                                   &idleSettings);

            //
            // Failures can happen as hub might not be in the right
            // power state
            //
            if (!NT_SUCCESS(status)) {
                DbgTraceTo(HubFdoContext->IfrLog, TL_WARNING, Hub, "WdfDeviceAssignS0IdleSettings failed with status %!STATUS!", status);
                status = STATUS_SUCCESS;
            }

            LEAVE;
        }
        //
        // Reaching at this point indicates that the setting GUID did not match
        // any of the known GUIDS
        //
        status = STATUS_INVALID_PARAMETER;

    } FINALLY {

    }

    return status;
}

_Use_decl_annotations_
HSM_EVENT
HUBFDO_QueryHubErrataFlags (
        PHUB_FDO_CONTEXT   HubFdoContext
    )
/*++

Routine Description:

    Queries the usbflags device registry key and the kernel shimming engine for hub errata flags

Returns:

    VOID

--*/
{
    NTSTATUS            status;
    HSM_EVENT           hsmEvent;
    CHAR                vendorId[MAX_VENDOR_ID_STRING_LENGTH];
    CHAR                deviceId[MAX_DEVICE_ID_STRING_LENGTH];
    CHAR                revisionId[MAX_REVISION_ID_STRING_LENGTH];


    PAGED_CODE();

    //
    // Get the Hw Verifier flags. Ignore the return value
    //
    if (HubFdoContext->HubParentInterface.HubDepth == 0) {

        switch (HubFdoContext->ControllerInfo.Type) {

        case UcxControllerParentBusTypePci:

            RtlStringCchPrintfA(vendorId,
                MAX_VENDOR_ID_STRING_LENGTH,
                "%04X",
                HubFdoContext->ControllerInfo.Pci.VendorId);

            RtlStringCchPrintfA(deviceId,
                MAX_DEVICE_ID_STRING_LENGTH,
                "%04X",
                HubFdoContext->ControllerInfo.Pci.DeviceId);

            RtlStringCchPrintfA(revisionId,
                MAX_REVISION_ID_STRING_LENGTH,
                "%04X",
                HubFdoContext->ControllerInfo.Pci.RevisionId);
            break;

        case UcxControllerParentBusTypeAcpi:
                
            RtlCopyMemory(vendorId,
                HubFdoContext->ControllerInfo.Acpi.VendorId,
                MAX_VENDOR_ID_STRING_LENGTH);

            RtlCopyMemory(deviceId,
                HubFdoContext->ControllerInfo.Acpi.DeviceId,
                MAX_DEVICE_ID_STRING_LENGTH);

            RtlCopyMemory(revisionId,
                HubFdoContext->ControllerInfo.Acpi.RevisionId,
                MAX_REVISION_ID_STRING_LENGTH);
            break;

        default:

            RtlStringCchPrintfA(vendorId,
                MAX_VENDOR_ID_STRING_LENGTH,
                "%04X",
                LONG_MAX);

            RtlStringCchPrintfA(deviceId,
                MAX_DEVICE_ID_STRING_LENGTH,
                "%04X",
                LONG_MAX);

            RtlStringCchPrintfA(revisionId,
                MAX_REVISION_ID_STRING_LENGTH,
                "%04X",
                0);
            break;
        }

        HUBREG_QueryUsbHardwareVerifierValue(&HubFdoContext->HubInfoFromParent.DeviceDescriptor,
                                             vendorId,
                                             deviceId,
                                             revisionId,
                                             &g_HwVerifierHubName,
                                             HubFdoContext->IfrLog,
                                             &HubFdoContext->HardwareVerifierFlags);

        status = HUBREG_QueryHubErrataFlags(HubFdoContext,
                                            vendorId,
                                            deviceId,
                                            revisionId);
    } else {

        HUBMISC_ConvertUsbDeviceIdsToString(&HubFdoContext->HubInfoFromParent.DeviceDescriptor,
                                            vendorId,
                                            deviceId,
                                            revisionId);

        HUBREG_QueryUsbHardwareVerifierValue(&HubFdoContext->HubInfoFromParent.DeviceDescriptor,
                                             vendorId,
                                             deviceId,
                                             revisionId,
                                             &g_HwVerifierHubName,
                                             HubFdoContext->IfrLog,
                                             &HubFdoContext->HardwareVerifierFlags);

        status = HUBREG_QueryHubErrataFlags(HubFdoContext,
                                            vendorId,
                                            deviceId,
                                            revisionId);
    }


    hsmEvent = NT_SUCCESS(status) ? HsmEventOperationSuccess: HsmEventOperationFailure;

    return hsmEvent;

}

_Use_decl_annotations_
VOID
HUBFDO_BugcheckSystemOnBootHubFailure (
        PHUB_FDO_CONTEXT    HubFdoContext
    )
/*++

Routine Description:

    Something really bad happened with the hub in the boot path.
    The system is going to bug-check soon anyways. We bug-check in
    USB for diagnosability

Returns:


--*/
{

    TRY {

        Usb3_KeBugCheckEx(USB3_BUGCODE_BOOT_DEVICE_FAILED,
                          WdfDeviceWdmGetDeviceObject((WdfObjectContextGetObject(HubFdoContext))),
                          (ULONG_PTR) FailedBootDeviceType_UsbHub,
                          (ULONG_PTR) NULL);
    } FINALLY {

    }

}

_Use_decl_annotations_
HSM_EVENT
HUBFDO_CheckIfHubIsInBootPath (
        PHUB_FDO_CONTEXT    HubFdoContext
    )
/*++

Routine Description:


Returns:


--*/
{
    return ((HubFdoContext->HubInformation.HubFlags.IsInBootOrPagingPath == 1) ? HsmEventYes:HsmEventNo);
}

