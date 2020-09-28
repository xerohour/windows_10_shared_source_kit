/*++
Copyright (c) Microsoft Corporation

Module Name:

    controller.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "controller.tmh"
#include "usbxhcimsg.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Controller_WdfEvtDeviceAdd)
#pragma alloc_text(PAGE, Controller_ConfigureS0IdleSettings)
#pragma alloc_text(PAGE, Controller_UpdateIdleTimeout)
#pragma alloc_text(PAGE, Controller_UpdateIdleTimeoutOnControllerFDOD0Entry)
#pragma alloc_text(PAGE, Controller_ConfigureSxWakeSettings)
#pragma alloc_text(PAGE, Controller_CreateWdfDevice)
#pragma alloc_text(PAGE, Controller_WdfEvtDevicePrepareHardware)
#pragma alloc_text(PAGE, Controller_WdfEvtDeviceReleaseHardware)
#pragma alloc_text(PAGE, Controller_WdfEvtDeviceUsageNotification)
#pragma alloc_text(PAGE, Controller_SetLogIdentifier)
#pragma alloc_text(PAGE, Controller_GetDeviceEnumerator)
#pragma alloc_text(PAGE, Controller_Create)
#pragma alloc_text(PAGE, Controller_RetrievePciData)
#pragma alloc_text(PAGE, Controller_PopulatePciDeviceInformation)
#pragma alloc_text(PAGE, Controller_RetrieveAcpiData)
#pragma alloc_text(PAGE, Controller_PopulateAcpiDeviceInformation)
#pragma alloc_text(PAGE, Controller_RetrieveUrsData)
#pragma alloc_text(PAGE, Controller_DriverInstalledDueToCompatibleIdMatch)
#pragma alloc_text(PAGE, Controller_SetDeviceDescription)
#pragma alloc_text(PAGE, Controller_PopulateDeviceFlags)
#pragma alloc_text(PAGE, Controller_PopulateDeviceFlagsFromRegistry)
#pragma alloc_text(PAGE, Controller_PopulateInternalDeviceFlags)
#pragma alloc_text(PAGE, Controller_PopulateHardwareVerifierFlags)
#pragma alloc_text(PAGE, Controller_PopulateTestConfiguration)
#pragma alloc_text(PAGE, Controller_UcxEvtEnableForwardProgress)
#pragma alloc_text(PAGE, Controller_WdfEvtDeviceFilterRemoveResourceRequirements)
#pragma alloc_text(PAGE, Controller_WdfEvtDeviceFilterAddResourceRequirements)
#pragma alloc_text(PAGE, Controller_WdfEvtDeviceRemoveAddedResources)
#pragma alloc_text(PAGE, Controller_WdfEvtDeviceSelfManagedIoCleanup)
#pragma alloc_text(PAGE, Controller_TelemetryAddControllerData)
#pragma alloc_text(PAGE, Controller_TelemetryOkToGenerateReport)
#pragma alloc_text(PAGE, Controller_TelemetryUpdateSubmitReportSuccess)



#pragma alloc_text(PAGE, Controller_UpdateSqmDatapoints)
#pragma alloc_text(PAGE, Controller_ExecuteDSMToEnableRTD3)
#pragma alloc_text(PAGE, Controller_ExecuteDSM)
#pragma alloc_text(PAGE, Controller_ExecuteDSMForHSICDisconnectInU3)
#endif

#if DBG

//
// Test flag to set in debugger to cause Live dumps creation.
//
volatile BOOLEAN g_TriggerLiveDump = FALSE;
volatile Controller_FatalErrorAction g_TriggerRecoveryAction = Controller_FatalErrorNoAction;

//
// Test flag to set in debugger to simulate controller surprise removal.
//
volatile BOOLEAN g_SimulateSurpriseRemoval = FALSE;

#endif

PCI_VID_TABLE_ENTRY PciVendorIdTable[] = {
    { PCI_VENDOR_ID_ATI_AMD,        L"ATI/AMD"              },
    { PCI_VENDOR_ID_AMD,            L"AMD"                  },
    { PCI_VENDOR_ID_NEC_RENESAS,    L"Renesas"              },
    { PCI_VENDOR_ID_RENESAS,        L"Renesas"              },
    { PCI_VENDOR_ID_TI,             L"Texas Instruments"    },
    { PCI_VENDOR_ID_NVIDIA,         L"NVIDIA"               },
    { PCI_VENDOR_ID_VIA,            L"VIA"                  },
    { PCI_VENDOR_ID_ETRON,          L"Etron"                },
    { PCI_VENDOR_ID_ASMEDIA,        L"ASMedia"              },
    { PCI_VENDOR_ID_FRESCO_LOGIC,   L"Fresco Logic"         },
    { PCI_VENDOR_ID_INTEL,          L"Intel(R)"             }
};

ACPI_VID_TABLE_ENTRY AcpiVendorIdTable[] = {
    { "NVDA", L"NVIDIA"             },
    { "TXNW", L"Texas Instruments"  },
    { "QCOM", L"Qualcomm"           },
    { "INTC", L"Intel(R)"           },
    { "INTL", L"Intel(R)"           }
};

PWSTR XhciCompatibleIdList[] = {
    L"PCI\\CC_0C0330",
    L"ACPI\\PNP0D10"
};

//
// Global Triage Info for dbgeng and 0x9F work
//

USBXHCI_TRIAGE_INFO g_UsbXhciTriageInfo = {
    //Version
    USBXHCI_TRIAGE_INFO_VERSION_1,

    // _DEVICE_DATA
    sizeof(DEVICE_DATA),                            //DeviceDataSize
    FIELD_OFFSET(DEVICE_DATA,ControllerData),       //DeviceDataControllerDataOffset
    sizeof(CONTROLLER_DATA),                        // ControllerDataSize

    // _CONTROLLER_DATA
    FIELD_OFFSET(CONTROLLER_DATA, WdfDevice),          // ControllerData_WdfDeviceOffset
    FIELD_OFFSET(CONTROLLER_DATA, UcxController),      // ControllerData_UcxControllerOffset;
    FIELD_OFFSET(CONTROLLER_DATA, RecorderLog),        // ControllerData_RecorderLogOffset;
    FIELD_OFFSET(CONTROLLER_DATA, IoControlHandle),    // ControllerData_IoControlOffset;
    sizeof(IOCONTROL_DATA),                            // IoControlDataSize;
    FIELD_OFFSET(CONTROLLER_DATA, RegisterHandle),     // ControllerData_RegisterOffset;
    sizeof(REGISTER_DATA),                             // RegisterDataSize;
    FIELD_OFFSET(CONTROLLER_DATA, CommonBufferHandle), // ControllerData_CommonBufferOffset;
    sizeof(COMMON_BUFFER_DATA),                        // CommonBufferDataSize;
    FIELD_OFFSET(CONTROLLER_DATA, InterrupterHandle),  // ControllerData_InterrupterOffset;
    sizeof(PRIMARY_INTERRUPTER_DATA),                  // PrimaryInterrupterSize;
    FIELD_OFFSET(CONTROLLER_DATA, DeviceSlotHandle),   // ControllerData_DeviceSlotOffset;
    sizeof(DEVICESLOT_DATA),                           // DeviceSlotDataSize;
    FIELD_OFFSET(CONTROLLER_DATA, CommandHandle),      // ControllerData_CommandOffset;
    sizeof(COMMAND_DATA),                              // CommandDataSize;
    FIELD_OFFSET(CONTROLLER_DATA, RootHubHandle),      // ControllerData_RootHubOffset;
    sizeof(ROOTHUB_DATA),                              // RootHubDataSize;
    FIELD_OFFSET(CONTROLLER_DATA, WdfTimer),           // ControllerData_WdfTimerOffset;

    // IFR Log fields are initalized at DriverEntry
    0,  // IfrLogHeaderSize
    0,  // IfrLogSizeOffset
    0,  // IfrSizeOfLogSizeField
    0,  // IfrDriverContextOffset
    0,  // IfrDriverContextSize

    // WPP Global control
    (PVOID) &WPP_GLOBAL_Control,        // WppGlobalControlPtr
    sizeof(*WPP_GLOBAL_Control),        // WppGlobalControlSize
    NULL,                               // WdfTriageInfoPtr (assigned later)
    (PVOID) &g_UsbXhciDriverObject      // GlobalDriverObjectPtr
};

NTSTATUS 
Controller_EvtPostPoFxRegisterDevice(  
    __in
        WDFDEVICE  Device,  
    __in
        POHANDLE   PoHandle  
    )  
{  
    GetDeviceData(Device)->ControllerData->PoHandle = PoHandle;
  
    return STATUS_SUCCESS;  
}  
  
VOID 
Controller_EvtPrePoFxUnregisterDevice(  
    __in
        WDFDEVICE  Device,  
    __in
        POHANDLE   PoHandle  
    )  
{  
    UNREFERENCED_PARAMETER(PoHandle);  
  
    GetDeviceData(Device)->ControllerData->PoHandle = NULL;
  
}  

NTSTATUS
Controller_WdfEvtDeviceAdd(
    WDFDRIVER       WdfDriver,
    PWDFDEVICE_INIT WdfDeviceInit
    )
/*++

Routine Description:

    A driver's EvtDriverDeviceAdd event callback function performs device
    initialization operations when the Plug and Play (PnP) manager reports
    the existence of a device.

--*/
{
    NTSTATUS                                status;
    WDFDEVICE                               wdfDevice;
    PCONTROLLER_DATA                        controllerData;
    PDEVICE_DATA                            deviceData;
    RECORDER_LOG_CREATE_PARAMS              recorderLogCreateParams;
    RECORDER_LOG                            recorderLog;
    ULONG                                   instanceNumber;

    UNREFERENCED_PARAMETER(WdfDriver);

    PAGED_CODE();

    TRY {

        //
        // Create the recorder log for the controller. If this fails, then
        // set the recorder log to the default log.
        //
        RECORDER_LOG_CREATE_PARAMS_INIT(&recorderLogCreateParams, NULL);
#if defined (DBG)
        recorderLogCreateParams.TotalBufferSize = 8192;
        recorderLogCreateParams.ErrorPartitionSize = 200;
#else
        recorderLogCreateParams.TotalBufferSize = 1024;
        recorderLogCreateParams.ErrorPartitionSize = 200;
#endif

        WppRecorderLogCreate(&recorderLogCreateParams, &recorderLog);

        //
        // Let UCX initialize the WDFDEVICEINIT
        //
        status = UcxInitializeDeviceInit(WdfDeviceInit);

        if (!NT_SUCCESS(status)) {
            LogError(recorderLog, Controller, "UcxInitializeDeviceInit Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Create the WDF device object.
        //
        status = Controller_CreateWdfDevice(WdfDeviceInit, recorderLog, &wdfDevice, &instanceNumber);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }


        //
        // Objects are created in the following order:
        //
        //  Controller_Create
        //  IoControl_Create
        //  Register_Create
        //  CommonBuffer_Create
        //  Interrupter_Create
        //  Command_Create
        //  RootHub_Create
        //
        status = Controller_Create(wdfDevice, recorderLog, instanceNumber, &controllerData);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "Controller_Create Failed %!STATUS!", status);
            LEAVE;
        }

        status = IoControl_Create(wdfDevice, controllerData, &controllerData->IoControlHandle);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "IoControl_Create Failed %!STATUS!", status);
            LEAVE;
        }

        status = Register_Create(wdfDevice, controllerData, &controllerData->RegisterHandle);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "Register_Create Failed %!STATUS!", status);
            LEAVE;
        }

        status = Interrupter_Create(wdfDevice, controllerData, &controllerData->InterrupterHandle);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "Interrupter_Create Failed %!STATUS!", status);
            LEAVE;
        }

        status = DeviceSlot_Create(wdfDevice, controllerData, &controllerData->DeviceSlotHandle);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "DeviceSlot_Create Failed %!STATUS!", status);
            LEAVE;
        }

        status = Command_Create(wdfDevice, controllerData, &controllerData->CommandHandle);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "Command_Create Failed %!STATUS!", status);
            LEAVE;
        }

        status = CommandFilter_Create(wdfDevice, controllerData, &controllerData->CommandFilterHandle);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "CommandFilter_Create Failed %!STATUS!", status);
            LEAVE;
        }

        status = RootHub_Create(wdfDevice, controllerData, &controllerData->RootHubHandle);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "RootHub_Create Failed %!STATUS!", status);
            LEAVE;
        }

        status = Wmi_Create(wdfDevice, controllerData, &controllerData->WmiHandle);

        if (!NT_SUCCESS(status)) {

            LogError(recorderLog, Controller, "Wmi_Create Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Set the WDF device data to point to the controller data, which
        // is stored in the UCX controller data.
        //
        deviceData = GetDeviceData(wdfDevice);
        deviceData->ControllerData    = controllerData;
        deviceData->UsbXhciTriageInfo = &g_UsbXhciTriageInfo;

        Controller_ConfigureS0IdleSettings(controllerData);
        Controller_ConfigureSxWakeSettings(controllerData);

        //
        // Initialize the list of devices for this controller.
        //
        KeInitializeSpinLock(&controllerData->DeviceListLock);
        InitializeListHead(&controllerData->DeviceListHead);
        controllerData->DeviceListCount = 0;

        //
        // To begin with, controller is logically in a D3Final state
        //
        controllerData->CurrentWdfPowerDeviceState = WdfPowerDeviceD3Final;

        //
        // Insert the controller into the global controller list.
        //
        Controller_AddControllerToGlobalControllerList(controllerData);













        Etw_ControllerCreate(NULL, controllerData);

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ConfigureS0IdleSettings(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    This routine configures the device to be put into a low power mode while the
    system is in S0. It does that after confirming that the device is capable of
    wake from Dx, while the system is in S0.

    N.B.: WdfDeviceAssignS0IdleSettings is supposed to take care of figuring out
    the wake capability of the device and configure S0Idle correctly. But it is
    more aggressive than we would like - if D3Cold interface is not available, it
    will check DEVICE_CAPABILITIES to determine S0Idle. But we do not want to fallback
    to DEVICE_CAPABILITIES since we don't trust them, and prefer to be conservative.

--*/
{
    DEVICE_WAKE_DEPTH               deepestWakeableDstate;
    D3COLD_SUPPORT_INTERFACE        d3ColdInterface;
    WDF_POWER_FRAMEWORK_SETTINGS    poFxSettings; 
    NTSTATUS                        status;

    PAGED_CODE();

    TRY {

        RtlZeroMemory(&d3ColdInterface, sizeof(D3COLD_SUPPORT_INTERFACE));

        if (Controller_GetDeviceFlags(ControllerData).DisableIdlePowerManagement) {

            ControllerData->ControllerS0IdleStatus = S0IdleStatusNotConfiguredDueToHackFlag;
            LogWarning(ControllerData->RecorderLog, Controller, "Setting S0IdleStatusNotConfiguredDueToHackFlag");
            LEAVE;
        }

        Controller_ExecuteDSMToEnableRTD3(ControllerData);

        //
        // Determine support for S0 idle power management through the parent's D3Cold
        // interface, then enable KMDF's idle power policy to power down an idle controller
        // if appropriate.
        //
        status = WdfFdoQueryForInterface(ControllerData->WdfDevice,
                                         &GUID_D3COLD_SUPPORT_INTERFACE,
                                         (PINTERFACE)&d3ColdInterface,
                                         sizeof(D3COLD_SUPPORT_INTERFACE),
                                         D3COLD_SUPPORT_INTERFACE_VERSION,
                                         NULL);

        if (!NT_SUCCESS(status)) {

            LogInfo(ControllerData->RecorderLog, Controller,
                    "WdfFdoQueryForInterface(GUID_D3COLD_SUPPORT_INTERFACE) failed %!STATUS!", status);

            ControllerData->ControllerS0IdleStatus = S0IdleStatusNotConfiguredDueToNoS0WakeCapability;
            LEAVE;
        }

        deepestWakeableDstate = DeviceWakeDepthNotWakeable;

        if (d3ColdInterface.GetIdleWakeInfo != NULL) {

            status = d3ColdInterface.GetIdleWakeInfo(d3ColdInterface.Context,
                                                     PowerSystemWorking,
                                                     &deepestWakeableDstate);
            if (!NT_SUCCESS(status)) {

                LogWarning(ControllerData->RecorderLog, Controller, "GetIdleWakeInfo call failed %!STATUS!", status);
            }
        }

        d3ColdInterface.InterfaceDereference(d3ColdInterface.Context);

        if (deepestWakeableDstate <= DeviceWakeDepthD0) {

            ControllerData->ControllerS0IdleStatus = S0IdleStatusNotConfiguredDueToNoS0WakeCapability;
            LogInfo(ControllerData->RecorderLog, Controller, 
                "Setting S0IdleStatusNotConfiguredDueToNoS0WakeCapability because deepestWakeableDstate(%d) <= DeviceWakeDepthD0GetIdleWakeInfo(%d)", 
                deepestWakeableDstate, DeviceWakeDepthD0);
            LEAVE;
        }

        ControllerData->ControllerS0IdleStatus = S0IdleStatusConfigured;

        Controller_UpdateIdleTimeout(ControllerData, CONTROLLER_IDLE_TIMEOUT_LONG_MS);
        LogInfo(ControllerData->RecorderLog, Controller,
            "Controller IdleTimeout set to %d", CONTROLLER_IDLE_TIMEOUT_LONG_MS);

        //
        // WDF implicitly registers with PEP when S0 Idle is configured.
        // And PoFx notifications can only be registered if WDF registers
        // with PEP.
        //
        WDF_POWER_FRAMEWORK_SETTINGS_INIT(&poFxSettings);  

        poFxSettings.EvtDeviceWdmPostPoFxRegisterDevice = Controller_EvtPostPoFxRegisterDevice;  
        poFxSettings.EvtDeviceWdmPrePoFxUnregisterDevice = Controller_EvtPrePoFxUnregisterDevice;
        
        status = WdfDeviceWdmAssignPowerFrameworkSettings(ControllerData->WdfDevice, &poFxSettings);  
        
        if (!NT_SUCCESS(status)) {  
            LogError(ControllerData->RecorderLog, Controller,
                     "Failed to initialize Power Framework settings - %!STATUS!", status);  
            LEAVE;  
        } 

    } FINALLY {

        LogInfo(ControllerData->RecorderLog, Controller,
                "%!S0IdleStatus!", ControllerData->ControllerS0IdleStatus);
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_UpdateIdleTimeoutOnControllerFDOD0Entry(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    Sets the Controller FDO Idle timeout to a large value to allow RootHub PDO
    sufficient time to receive its D0Entry. Ideally, Framework should guarantee
    that Controller FDO does not get put back to Dx before its static child (the
    RootHub PDO) receives its D0Entry. But that is not the case. Specifically,
    the following problem was observed on SOC systems:

    During boot, controller FDO enters D0, then immediately exits and reenters D0.
    Then the RootHub enters D0. This unnecessary Dx transition causes unnecessary
    controller resets, and may cause some SuperSpeed devices to switch to HighSpeed
    if a warm port reset in progress was interrupted by the second controller reset.

--*/
{
    PAGED_CODE();

    if (SystemPoweringUp(ControllerData->SystemPowerAction) &&
        (ControllerData->ControllerS0IdleStatus == S0IdleStatusConfigured) &&
        (ControllerData->LastSetIdleTimeout != CONTROLLER_IDLE_TIMEOUT_LONG_MS)) {

        Controller_UpdateIdleTimeout(ControllerData, CONTROLLER_IDLE_TIMEOUT_LONG_MS);
        LogError(ControllerData->RecorderLog, Controller,
            "Controller IdleTimeout set to %d", CONTROLLER_IDLE_TIMEOUT_LONG_MS);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_UpdateIdleTimeoutOnRootHubPDOD0Entry(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    Sets the Controller FDO Idle timeout to a small value now that we know that the
    RootHub has entered D0.

--*/
{
    if ((ControllerData->ControllerS0IdleStatus == S0IdleStatusConfigured) &&
        (ControllerData->LastSetIdleTimeout != CONTROLLER_IDLE_TIMEOUT_SHORT_MS)) {

        WdfWorkItemEnqueue(ControllerData->IdleTimeoutUpdateWorkItem);
    }
}

VOID
Controller_IdleTimeoutUpdateWorker(
    WDFWORKITEM WdfWorkItem
    )
{
    PCONTROLLER_DATA controllerData;

    controllerData = GetControllerData(WdfWorkItemGetParentObject(WdfWorkItem));

    Controller_UpdateIdleTimeout(controllerData, CONTROLLER_IDLE_TIMEOUT_SHORT_MS);
    LogError(controllerData->RecorderLog, Controller,
            "Controller IdleTimeout set to %d", CONTROLLER_IDLE_TIMEOUT_SHORT_MS);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_UpdateIdleTimeout(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               Timeout
    )
{
    WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS   idleSettings;
    NTSTATUS                                status;

    PAGED_CODE();

    TRY {

        WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&idleSettings, IdleCanWakeFromS0);

        idleSettings.UserControlOfIdleSettings = IdleAllowUserControl;
        idleSettings.IdleTimeout               = Timeout;
        idleSettings.IdleTimeoutType           = SystemManagedIdleTimeoutWithHint;

        status = WdfDeviceAssignS0IdleSettings(ControllerData->WdfDevice, &idleSettings);

        //
        // This is not a fatal error. Controller is not capable of remote wake while in S0.
        // The only downside is that the controller will not be taken out of D0 while in S0.
        //
        if (!NT_SUCCESS(status)) {

            LogInfo(ControllerData->RecorderLog, Controller,
                     "WdfDeviceAssignS0IdleSettings Failed %!STATUS!, setting to S0IdleStatusNotConfiguredDueToNoS0WakeCapability", status);

            ControllerData->ControllerS0IdleStatus = S0IdleStatusNotConfiguredDueToNoS0WakeCapability;
            LEAVE;
        }

        ControllerData->LastSetIdleTimeout = Timeout;

        LogInfo(ControllerData->RecorderLog, Controller, "WdfDeviceAssignS0IdleSettings succeeded, S0Idle Timeout changed to %u", Timeout);

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ConfigureSxWakeSettings(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    For system sleep, set up the controller so that it propogates wait wake
    if child is armed for wake.

--*/
{
    NTSTATUS                                status;
    WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS   wakeSettings;

    PAGED_CODE();

    TRY {

        WDF_DEVICE_POWER_POLICY_WAKE_SETTINGS_INIT(&wakeSettings);

        wakeSettings.Enabled                             = WdfFalse;
        wakeSettings.ArmForWakeIfChildrenAreArmedForWake = TRUE;
        wakeSettings.IndicateChildWakeOnParentWake       = TRUE;
        wakeSettings.UserControlOfWakeSettings           = WakeDoNotAllowUserControl;

        status = WdfDeviceAssignSxWakeSettings(ControllerData->WdfDevice, &wakeSettings);

        //
        // Failure is not fatal. It simply means that the controller is not capable
        // of remote wake, implying that no USB device connected to this controller
        // will be able to wake the system.
        //
        // TODO: PostWin8 Add hardware verifier break for this case.
        //
        if (!NT_SUCCESS(status)) {

            LogError(ControllerData->RecorderLog, Controller,
                     "WdfDeviceAssignSxWakeSettings Failed %!STATUS! -- attached devices will not be able to wake the system", status);
        }

    } FINALLY {

    }

    return;
}

VOID
Controller_NotifyPortStateToPep(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        SUB_DX_STATE        SubDxState
    )
/*++

Routine Description:

    This routine sends the power control code the PEP notifying it
    about the port states
    
--*/
{

    NTSTATUS                                status;
    
    TRY {
    
        if (ControllerData->PoHandle == NULL) {
            LEAVE;
        }
        
        //
        // This Power control code is used to inform the PEP about the state of 
        // the root ports of the XHCI controller. PEP can use this information
        // to optimize power when the controller subsequently enters a Dx state
        //                              
        status = PoFxPowerControl(ControllerData->PoHandle,
                                  (LPCGUID)&GUID_HC_SUB_DX_STATE,
                                  (PVOID)&SubDxState,
                                  sizeof(SubDxState),
                                  NULL,
                                  0,
                                  NULL); 

        if (!NT_SUCCESS(status)) {
            LogInfo(ControllerData->RecorderLog, Controller, "PoFxPowerControl failed with %!STATUS! ", status);
            //
            // Most PEPs don't understand this Ioctl, failure is not fatal
            //
        } 
    
    } FINALLY {
    
    }
    
    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_CreateWdfDevice(
    __in
        PWDFDEVICE_INIT WdfDeviceInit,
    __in
        RECORDER_LOG    RecorderLog,
    __out
        WDFDEVICE       *WdfDevice,
    __out
        PULONG          InstanceNumber
    )
/*++

Routine Description:

    This function creates the WDF device, along with the device interface
    and symbolic link for the device.

--*/
{
    NTSTATUS                            status;
    WDF_PNPPOWER_EVENT_CALLBACKS        wdfPnpPowerCallbacks;
    WDF_OBJECT_ATTRIBUTES               wdfAttributes;
    WDF_OBJECT_ATTRIBUTES               wdfRequestAttributes;
    PNP_BUS_INFORMATION                 pnpBusInformation;
    ULONG                               instanceNumber;
    UCHAR                               startIrpMinorCode;
    BOOLEAN                             deleteRecorderLog;
    PDEVICE_DATA                        deviceData;
    WDF_FDO_EVENT_CALLBACKS             fdoEventCallbacks;
    WDF_POWER_POLICY_EVENT_CALLBACKS    powerPolicyCallbacks;

    DECLARE_UNICODE_STRING_SIZE(uniDeviceName, DeviceNameSize);
    DECLARE_UNICODE_STRING_SIZE(uniSymlinkName, SymlinkNameSize);

    PAGED_CODE();

    TRY {

        deleteRecorderLog = TRUE;
        instanceNumber = 0;

        //
        // Set PNP callbacks that are called by WDF.
        //
        WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&wdfPnpPowerCallbacks);
        wdfPnpPowerCallbacks.EvtDevicePrepareHardware = Controller_WdfEvtDevicePrepareHardware;
        wdfPnpPowerCallbacks.EvtDeviceReleaseHardware = Controller_WdfEvtDeviceReleaseHardware;
        wdfPnpPowerCallbacks.EvtDeviceUsageNotification = Controller_WdfEvtDeviceUsageNotification;

        wdfPnpPowerCallbacks.EvtDeviceD0Entry = Controller_WdfEvtDeviceD0Entry;

        wdfPnpPowerCallbacks.EvtDeviceD0EntryPostInterruptsEnabled =
            Controller_WdfEvtDeviceD0EntryPostInterruptsEnabled;

        wdfPnpPowerCallbacks.EvtDeviceD0ExitPreInterruptsDisabled =
            Controller_WdfEvtDeviceD0ExitPreInterruptsDisabled;

        wdfPnpPowerCallbacks.EvtDeviceD0Exit = Controller_WdfEvtDeviceD0Exit;

        wdfPnpPowerCallbacks.EvtDeviceSelfManagedIoInit = Controller_WdfEvtDeviceSelfManagedIoInit;
        wdfPnpPowerCallbacks.EvtDeviceSelfManagedIoCleanup  = Controller_WdfEvtDeviceSelfManagedIoCleanup;

        WdfDeviceInitSetPnpPowerEventCallbacks(WdfDeviceInit, &wdfPnpPowerCallbacks);


        WDF_POWER_POLICY_EVENT_CALLBACKS_INIT(&powerPolicyCallbacks);
        powerPolicyCallbacks.EvtDeviceArmWakeFromS0 = Controller_WdfEvtDeviceArmWakeFromS0;
        powerPolicyCallbacks.EvtDeviceDisarmWakeFromS0 = Controller_WdfEvtDeviceDisarmWakeFromS0;
        powerPolicyCallbacks.EvtDeviceWakeFromS0Triggered = Controller_WdfEvtDeviceWakeFromS0Triggered;
        powerPolicyCallbacks.EvtDeviceArmWakeFromSx = Controller_WdfEvtDeviceArmWakeFromSx;
        powerPolicyCallbacks.EvtDeviceDisarmWakeFromSx = Controller_WdfEvtDeviceDisarmWakeFromSx;

        WdfDeviceInitSetPowerPolicyEventCallbacks(WdfDeviceInit,
                                                  &powerPolicyCallbacks);

        //
        // Set a resource remove callback. This is done to remove MSI interrupt resources
        // in order to force line-based interrupts, if needed.
        //
        WDF_FDO_EVENT_CALLBACKS_INIT(&fdoEventCallbacks);
        fdoEventCallbacks.EvtDeviceFilterRemoveResourceRequirements =
            Controller_WdfEvtDeviceFilterRemoveResourceRequirements;
        fdoEventCallbacks.EvtDeviceFilterAddResourceRequirements =
            Controller_WdfEvtDeviceFilterAddResourceRequirements;
        fdoEventCallbacks.EvtDeviceRemoveAddedResources =
            Controller_WdfEvtDeviceRemoveAddedResources;

        WdfFdoInitSetEventCallbacks(WdfDeviceInit, &fdoEventCallbacks);

        //
        // If D0Entry fails, child devnodes should get ReleaseHardware callbacks before
        // this devnode.
        //
        WdfDeviceInitSetReleaseHardwareOrderOnFailure(WdfDeviceInit,
                                                      WdfReleaseHardwareOrderOnFailureAfterDescendants);

        //
        // Set a preprocess callback for the set power irp. This is done to determine the state
        // that the system is resuming from.
        //
        startIrpMinorCode = (UCHAR)IRP_MN_SET_POWER;

        status = WdfDeviceInitAssignWdmIrpPreprocessCallback(WdfDeviceInit,
                                                             Controller_WdfEvtPreprocessSetPowerIrp,
                                                             IRP_MJ_POWER,
                                                             &startIrpMinorCode,
                                                             1);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller,
                     "WdfDeviceInitAssignWdmIrpPreprocessCallback Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Set the preprocess callback for the controller reset test hook.
        //
        if (g_WdfDriverUsbXhciContext->TestModeOn) {

            status = WdfDeviceInitAssignWdmIrpPreprocessCallback(WdfDeviceInit,
                                                                 Controller_EvtIoInternalDeviceControl,
                                                                 IRP_MJ_INTERNAL_DEVICE_CONTROL,
                                                                 NULL,
                                                                 0);

            if (!NT_SUCCESS(status)) {

                LogError(RecorderLog, Controller,
                         "WdfDeviceInitAssignWdmIrpPreprocessCallback Failed %!STATUS!", status);
                LEAVE;
            }
        }

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, DEVICE_DATA);
        wdfAttributes.EvtCleanupCallback = Controller_WdfEvtCleanupCallback;

        //
        // A unique static device name is generated in order to maintain
        // backward compatibility with the prior implementations.
        //
        do {

            status = RtlUnicodeStringPrintf(&uniDeviceName,
                                            L"%ws%d",
                                            BASE_DEVICE_NAME,
                                            instanceNumber);

            if (!NT_SUCCESS(status)) {

                LogError(RecorderLog, Controller,
                         "RtlUnicodeStringPrintf (uniDeviceName) Failed %!STATUS!", status);
                LEAVE;
            }

            status = WdfDeviceInitAssignName(WdfDeviceInit, &uniDeviceName);

            if (!NT_SUCCESS(status)) {

                LogError(RecorderLog, Controller, "WdfDeviceInitAssignName Failed %!STATUS!", status);
                LEAVE;
            }

            //
            // Set the security descriptor for the device.
            //
            status = WdfDeviceInitAssignSDDLString(WdfDeviceInit, &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R);

            if (!NT_SUCCESS(status)) {

                LogError(RecorderLog, Controller,
                         "WdfDeviceInitAssignSDDLString Failed %!STATUS!", status);
                LEAVE;
            }

            WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfRequestAttributes, REQUEST_DATA);

            WdfDeviceInitSetRequestAttributes(WdfDeviceInit, &wdfRequestAttributes);

            status = WdfDeviceCreate(&WdfDeviceInit, &wdfAttributes, WdfDevice);

            //
            // TODO: PostWin8 Potentially unbounded loop.
            //
            if (status == STATUS_OBJECT_NAME_COLLISION) {

                LogWarning(RecorderLog, Controller, "WdfDeviceCreate ObjectNameCollision %d",
                           instanceNumber);
                instanceNumber++;

            } else if (!NT_SUCCESS(status)) {

                LogError(RecorderLog, Controller, "WdfDeviceCreate Failed %!STATUS!", status);
                LEAVE;

            } else {

                deleteRecorderLog = FALSE;
                break;
            }

        } WHILE (TRUE);

        //
        // Save the recorder log in the device data context. This is referened when the
        // cleanup callback is called to delete the recorder log.
        //
        deviceData = GetDeviceData(*WdfDevice);
        deviceData->RecorderLog = RecorderLog;

        //
        // Indicate special file support
        //
        WdfDeviceSetSpecialFileSupport(*WdfDevice,
                                       WdfSpecialFilePaging,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(*WdfDevice,
                                       WdfSpecialFileHibernation,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(*WdfDevice,
                                       WdfSpecialFileDump,
                                       TRUE);
        WdfDeviceSetSpecialFileSupport(*WdfDevice,
                                       WdfSpecialFileBoot,
                                       TRUE);

        //
        // Create the symbolic link.
        //
        status = RtlUnicodeStringPrintf(&uniSymlinkName,
                                        L"%ws%d",
                                        BASE_SYMBOLIC_LINK_NAME,
                                        instanceNumber);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller,
                     "RtlUnicodeStringPrintf (SymlinkName) Failed %!STATUS!", status);
            LEAVE;
        }

        status = WdfDeviceCreateSymbolicLink(*WdfDevice, &uniSymlinkName);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller,
                     "WdfDeviceCreateSymbolicLink Failed %!STATUS!", status);
            LEAVE;
        }
        LogInfo(RecorderLog, Controller, "Created %ws", uniSymlinkName.Buffer);

        //
        // Set bus information for children devices.
        //
        pnpBusInformation.BusTypeGuid = GUID_BUS_TYPE_USB;
        pnpBusInformation.LegacyBusType = PNPBus;
        pnpBusInformation.BusNumber = 0;

        WdfDeviceSetBusInformationForChildren(*WdfDevice, &pnpBusInformation);

        //
        // Create the device interface.
        //
        status = WdfDeviceCreateDeviceInterface(*WdfDevice,
                                                (LPGUID)&GUID_CLASS_USB_HOST_CONTROLLER,
                                                NULL);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller,
                     "WdfDeviceCreateDeviceInterface Failed %!STATUS!", status);
            LEAVE;
        }

    } FINALLY {

        //
        // If the WDF Device is not created, then the cleanup callback is
        // not called. If that happens, the the recorder log must be deleted
        // or else it is leaked.
        //
        if (deleteRecorderLog) {
            WppRecorderLogDelete(RecorderLog);
        }

        *InstanceNumber = instanceNumber;
    }

    return status;
}

NTSTATUS
Controller_WdfEvtDevicePrepareHardware(
    WDFDEVICE       WdfDevice,
    WDFCMRESLIST    WdfResourcesRaw,
    WDFCMRESLIST    WdfResourcesTranslated
    )
/*++

Routine Description:

    A driver's EvtDevicePrepareHardware event callback function performs any
    operations that are needed to make a device accessible to the driver.

Return Value:

    If the EvtDevicePrepareHardware callback function encounters no errors, it
    must return STATUS_SUCCESS or another status value for which
    NT_SUCCESS(status) equals TRUE. Otherwise, it must return a status value for
    which NT_SUCCESS(status) equals FALSE.

    If NT_SUCCESS(status) equals FALSE, the framework calls the driver's
    EvtDeviceReleaseHardware callback function.

--*/
{
    NTSTATUS            status;
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;

    PAGED_CODE();

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        LogInfo(controllerData->RecorderLog, Controller, "Controller_WdfEvtDevicePrepareHardware Start. WdfDevice 0x%p", WdfDevice);

        //
        // Objects are prepared in the following order:
        //
        //  Register_PrepareHardware
        //  Interrupter_PrepareHardware
        //  DeviceSlot_PrepareHardware
        //  Command_PrepareHardware
        //  RootHub_PrepareHardware
        //
        status = Register_PrepareHardware(controllerData->RegisterHandle, WdfResourcesTranslated);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "Register_PrepareHardware Failed %!STATUS!", status);
            LEAVE;
        }


        //
        // Ideally, the WdfDmaEnabler (CommonBuffer) object should be created once
        // in AddDevice. But some information needed for creating it is not available
        // until PrepareHardware. So create the CommonBuffer object in PrepareHardware,
        // but only if we haven't already created it in a previous PrepareHardware
        // for this device. WdfDmaEnabler object is never explicitly deleted, it gets
        // cleaned up with the WdfDevice that it is parented to.
        //
        if (controllerData->CommonBufferHandle == NULL) {

            status = CommonBuffer_Create(WdfDevice,
                                         controllerData,
                                         &controllerData->CommonBufferHandle);

            if (!NT_SUCCESS(status)) {

                LogError(controllerData->RecorderLog, Controller,
                         "CommonBuffer_Create Failed %!STATUS!", status);
                LEAVE;
            }
        }

        status = Interrupter_PrepareHardware(controllerData->InterrupterHandle,
                                             WdfResourcesRaw,
                                             WdfResourcesTranslated);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "Interrupter_PrepareHardware Failed %!STATUS!", status);
            LEAVE;
        }

        status = DeviceSlot_PrepareHardware(controllerData->DeviceSlotHandle);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "DeviceSlot_PrepareHardware Failed %!STATUS!", status);
            LEAVE;
        }

        status = Command_PrepareHardware(controllerData->CommandHandle);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "Command_PrepareHardware Failed %!STATUS!", status);
            LEAVE;
        }

        status = RootHub_PrepareHardware(controllerData->RootHubHandle);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "RootHub_PrepareHardware Failed %!STATUS!", status);
            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            DbgPrint("USBXHCI: Controller enumeration failure in WdfEvtDevicePrepareHardware\n");
            LogFatal(controllerData->RecorderLog, Controller,
                     "Controller enumeration failure in WdfEvtDevicePrepareHardware %!STATUS!", status);

            Controller_ReportFatalError(controllerData,
                                        Controller_FatalErrorReportFailedNoRestart,
                                        USB3_WER_BUGCODE_USBXHCI_D0ENTRY_FAILED,
                                        NULL,
                                        NULL,
                                        NULL);

            if (KdRefreshDebuggerNotPresent() == FALSE) {
                DbgBreakPoint();
            }
        }
    }

    return status;
}

NTSTATUS
Controller_WdfEvtDeviceReleaseHardware(
    WDFDEVICE       WdfDevice,
    WDFCMRESLIST    WdfResourcesTranslated
    )
/*++

Routine Description:

    A driver's EvtDeviceReleaseHardware event callback function performs
    operations that are needed when a device is no longer accessible.

Return Value:

    If the EvtDeviceReleaseHardware callback function encounters no errors,
    it must return STATUS_SUCCESS or another status value for which
    NT_SUCCESS(status) equals TRUE. Otherwise, it must return a status value
    for which NT_SUCCESS(status) equals FALSE.

--*/
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;

    UNREFERENCED_PARAMETER(WdfResourcesTranslated);

    PAGED_CODE();

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        LogInfo(controllerData->RecorderLog, Controller, "WdfDevice 0x%p", WdfDevice);

        //
        // Ideally these should be flushed before D0Exit, but if D0Entry
        // fails, D0Exit will not get called. So flush the work items one
        // more time in ReleaseHardware.
        //
        // By flushing before releasing objects, we allow TelemetryReportWorker
        // to save off more details about the hardware.
        //
        WdfWorkItemFlush(controllerData->TelemetryReportWorkItem);
        WdfWorkItemFlush(controllerData->IdleTimeoutUpdateWorkItem);

        //
        // Objects are released in the following order:
        //
        //  RootHub_ReleaseHardware
        //  Command_ReleaseHardware
        //  DeviceSlot_ReleaseHardware
        //  Interrupter_ReleaseHardware
        //  Register_ReleaseHardware
        //
        // These functions should not queue work items since the hardware could
        // be released by the time work items ran.
        //
        RootHub_ReleaseHardware(controllerData->RootHubHandle);
        Command_ReleaseHardware(controllerData->CommandHandle);
        DeviceSlot_ReleaseHardware(controllerData->DeviceSlotHandle);
        Interrupter_ReleaseHardware(controllerData->InterrupterHandle);
        Register_ReleaseHardware(controllerData->RegisterHandle);

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

VOID
Controller_WdfEvtDeviceUsageNotification(
    WDFDEVICE               WdfDevice,
    WDF_SPECIAL_FILE_TYPE   WdfSpecialFileType,
    BOOLEAN                 IsInNotificationPath
    )
/*++

Routine Description:

    Called whenever our FDO is notified of a change in its use for special file
    support. Saves the status of various special files for later use.

--*/
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;

    PAGED_CODE();

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        LogInfo(controllerData->RecorderLog, Controller,
                "WdfDevice 0x%p, %!WdfSpecialFileType!, Inpath: %u",
                WdfDevice, WdfSpecialFileType, IsInNotificationPath);

        if (IsInNotificationPath) {
            controllerData->SpecialFileStatus |= (1 << WdfSpecialFileType);
            //
            // Lock the driver in memory. To do this,
            // randomly pick one function that belongs to
            // the PAGE section. And then all functions
            // in this section will be locked. We do not
            // need to unlock them ever, so we don't need
            // the return value
            //
            (VOID)MmLockPagableCodeSection(Controller_WdfEvtDeviceAdd);

        } else {
            controllerData->SpecialFileStatus &= ~(1 << WdfSpecialFileType);
        }

    } FINALLY {

    }

    return;
}

NTSTATUS
Controller_WdfEvtDeviceSelfManagedIoInit(
    WDFDEVICE               WdfDevice
    )
/*++

Routine Description:

    The framework calls the EvtDeviceSelfManagedIoInit function
    after driver's EvtDeviceD0Entry & EvtDeviceD0EntryPostInterruptsEnabled
    function when the device is started for the first time. This callback is not
    called when the device is stopped/restarted or the device goes through
    suspend/resume cycle.

    This callback is meant for the driver to do one-time resource initialization
    and device/software configuration.

Return Value:

    NTSTATUS

--*/
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;
    NTSTATUS            status;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        LogVerbose(controllerData->RecorderLog, Controller, "WdfDevice 0x%p ", WdfDevice);

        //
        // Problem : When this controller is being shared with the USB kernel debug
        // module, if it were to be put into S0Idle, wake will not work because
        // the bus driver cannot physically put it into Dx, since it is being
        // used by the debug module.
        //
        // Workaround : Keep a power reference on the controller while the System is
        // in S0. Drop the reference while the System is in LPE.
        //
        if (Controller_InUseByDebugger(controllerData)) {

            LogInfo(controllerData->RecorderLog, Controller,
                    "WdfDevice 0x%p Controller is shared with debug module", WdfDevice);

            status = PoRegisterPowerSettingCallback(NULL,
                                                    &GUID_LOW_POWER_EPOCH,
                                                    Controller_LPEEnterExitCallback,
                                                    controllerData,
                                                    &controllerData->LPEEnterExitCallbackHandle);

            //
            // Non-fatal error. Log and continue.
            //
            if (!NT_SUCCESS(status)) {
                LogWarning(controllerData->RecorderLog, Controller,
                           "PoRegisterPowerSettingCallback(GUID_LOW_POWER_EPOCH) failed %!STATUS!", status);
            }
        }

        //
        // Attempt to provide a more descriptive device description if possible.
        // This is only needed once per device install, but since we cannot ensure
        // that, do it once per lifetime of the devnode.
        //
        Controller_SetDeviceDescription(controllerData);

        //
        // Hardware verifier flags are queried in this function because
        //  - That information is only needed once per device.
        //  - xHCI version information is not available until PrepareHardware, so can't
        //    do this in AddDevice.
        // Note that this means that no verifier breaks will trigger until this function
        // runs.
        //
        Controller_PopulateHardwareVerifierFlags(controllerData);

        //
        // KSE flags are queried once when the device is created using VEN/DEV/REV.
        // But since firmware information is not available at that time, query the
        // flags once again here after firmware information is known.
        //
        Controller_PopulateDeviceFlagsFromKse(controllerData);

        //
        // Preallocate a buffer that will be used to pad split transfers that need
        // to be rounded up to the packet size.
        //
        if (Controller_GetDeviceFlags(controllerData).RoundSplitTransferSizeToPacketSize) {

            controllerData->ScratchTransferBufferData =
                CommonBuffer_AcquireBuffer(controllerData->CommonBufferHandle,
                                           SCRATCH_TRANSFER_BUFFER_SIZE,
                                           controllerData,
                                           CONTROLLER_ALLOC_TAG1);

            if (controllerData->ScratchTransferBufferData == NULL) {

                LogWarning(controllerData->RecorderLog, Controller,
                           "WdfDevice 0x%p : Failed to acquire scratch transfer commonbuffer",
                           WdfDevice);
            }
        }

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_UpdateSqmDatapoints(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    Updates controller related SQM datapoints at known location in the registry
    (hardware key), to be picked up by usbceip.dll.

--*/
{
    NTSTATUS        status;
    WDFKEY          wdfKey;
    UNICODE_STRING  valueName;
    ULONG           value;

    PAGED_CODE();

    TRY {

        status = WdfDeviceOpenRegistryKey(ControllerData->WdfDevice,
                                          PLUGPLAY_REGKEY_DEVICE,
                                          (KEY_READ | KEY_WRITE),
                                          WDF_NO_OBJECT_ATTRIBUTES,
                                          &wdfKey);

        if (!NT_SUCCESS(status)) {

            LogWarning(ControllerData->RecorderLog, Controller,
                       "WdfDeviceOpenRegistryKey Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // HCRestoreStateFailureCount.
        //
        RtlInitUnicodeString(&valueName, DEVICE_REGISTRY_KEY_HC_RESTORE_STATE_FAILURE_COUNT);

        status = WdfRegistryQueryULong(wdfKey, &valueName, &value);

        if (NT_SUCCESS(status)) {
            value = value + ControllerData->Counter.RestoreStateFailureCount;
        } else {
            value = ControllerData->Counter.RestoreStateFailureCount;
        }

        status = WdfRegistryAssignULong(wdfKey, &valueName, value);

        if (!NT_SUCCESS(status)) {
            LogWarning(ControllerData->RecorderLog, Controller,
                       "WdfRegistryAssignULong Failed %!STATUS!", status);
        }

        //
        // HCRecoveryCount.
        //
        RtlInitUnicodeString(&valueName, DEVICE_REGISTRY_KEY_HC_RECOVERY_COUNT);

        status = WdfRegistryQueryULong(wdfKey, &valueName, &value);

        if (NT_SUCCESS(status)) {
            value = value + ControllerData->Counter.ResetCount;
        } else {
            value = ControllerData->Counter.ResetCount;
        }

        status = WdfRegistryAssignULong(wdfKey, &valueName, value);

        if (!NT_SUCCESS(status)) {
            LogWarning(ControllerData->RecorderLog, Controller,
                       "WdfRegistryAssignULong Failed %!STATUS!", status);
        }

    } FINALLY {

        if (wdfKey != NULL) {
            WdfRegistryClose(wdfKey);
        }
    }

    return;
}

VOID
Controller_WdfEvtDeviceSelfManagedIoCleanup(
    WDFDEVICE               WdfDevice
    )
/*++

Routine Description:

    Cleanup the resources allocated by EvtDeviceSelfManagedIoInit.

--*/
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;

    PAGED_CODE();

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        LogVerbose(controllerData->RecorderLog, Controller, "WdfDevice 0x%p ", WdfDevice);

        if (controllerData->LPEEnterExitCallbackHandle) {
            PoUnregisterPowerSettingCallback(controllerData->LPEEnterExitCallbackHandle);
            controllerData->LPEEnterExitCallbackHandle = NULL;
        }

        Controller_ReleaseWdfPowerReferenceForDebugger(controllerData);

        if (controllerData->ScratchTransferBufferData != NULL) {

            CommonBuffer_ReleaseBuffer(controllerData->CommonBufferHandle,
                                       controllerData->ScratchTransferBufferData);

            controllerData->ScratchTransferBufferData = NULL;
        }

        Controller_UpdateSqmDatapoints(controllerData);

    } FINALLY {

    }

    return;
}

NTSTATUS
Controller_WdfEvtDeviceD0Entry(
    WDFDEVICE               WdfDevice,
    WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    The framework calls the EvtDeviceD0Entrycallback function
    immediately after the device enters its working (D0) state and is
    available to the driver, but before the device's interrupts have
    been enabled. The PreviousState parameter identifies the device
    power state that the device was in before it entered the D0 state.

    The callback function must perform any operations that are needed to
    make the device fully operational, such as loading firmware or
    enabling device capabilities that are disabled when the device is in
    a low-power state.

Return Value:

    If the EvtDeviceD0Entrycallback function encounters no errors, it
    must return STATUS_SUCCESS or another status value for which
    NT_SUCCESS(status) equals TRUE. Otherwise, it must return a status
    value for which NT_SUCCESS(status) equals FALSE.

    The framework does not call the driver's EvtDeviceD0Exit callback
    function after the EvtDeviceD0Entry callback function returns a
    status value for which NT_SUCCESS(status) equals FALSE.

--*/
{
    NTSTATUS                status;
    PDEVICE_DATA            deviceData;
    PCONTROLLER_DATA        controllerData;
    PCAPABILITY_REGISTERS   capabilityRegisters;
    HCLENGTHVERSION         lengthVersion;
    ULONG                   pass;
    BOOLEAN                 controllerWasReset = FALSE;
    POWER_ACTION            powerAction;
    BOOLEAN                 controllerGone;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;
        controllerGone = FALSE;

        powerAction = WdfDeviceGetSystemPowerAction(WdfDevice);

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_D0_ENTRY_START(
            NULL,
            controllerData->UcxController,
            WdfPreviousState,
            powerAction,
            (USHORT)controllerData->WaitWakeQueued);

        LogInfo(controllerData->RecorderLog, Controller,
                "WdfDevice 0x%p %!WdfPowerDeviceState! %!SystemPowerAction!",
                WdfDevice, WdfPreviousState, controllerData->SystemPowerAction);

        capabilityRegisters = Register_GetCapabilityRegisters(controllerData->RegisterHandle);

        lengthVersion.AsUlong32 = Register_ReadUlong(&capabilityRegisters->LengthVersion.AsUlong32);

        if (lengthVersion.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {
            LogError(controllerData->RecorderLog, Controller, "Controller Gone");
            controllerGone = TRUE;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // A BIOS handoff is required on a clean boot and on resume from S4.
        // For clean boot, handoff is done in the PrepareHardware callback.
        // For resume from S4, we will do the handoff here, unless the Crashdump
        // stack was used to read the hiberfile, in which case, it must have
        // done the handoff.
        //
        if (controllerData->SystemPowerAction == SystemPowerAction_ResumeFromHibernate) {

            //
            // Diskdump no longer calls crashdump cleanup on resume from hibernate.
            // So the controller would not be stopped when we get here. We need
            // to stop it, so that it can be reset for resume from hibernate
            //
            if ((Crashdump_HiberfileReadEnabled(Controller_GetHibernateContext(controllerData)))) {

                status = Register_ControllerStop(controllerData->RegisterHandle);

                if (!NT_SUCCESS(status)) {
                    LogError(controllerData->RecorderLog, Controller,
                             "Register_ControllerStop Failed %!STATUS!", status);
                }


            } else {

                status = Register_BiosHandoff(controllerData->RegisterHandle);

                if (!NT_SUCCESS(status)) {

                    LogError(controllerData->RecorderLog, Controller,
                             "Register_BiosHandoff Failed %!STATUS!", status);
                    LEAVE;
                }
            }

            DeviceSlot_D0EntryCleanupState(controllerData->DeviceSlotHandle,
                                           WdfPreviousState);

            status = Register_ControllerReset(controllerData->RegisterHandle, FALSE);

            if (!NT_SUCCESS(status)) {

                LogError(controllerData->RecorderLog, Controller,
                         "Register_ControllerReset Failed %!STATUS!", status);

                LEAVE;
            }

            controllerWasReset = TRUE;
        }

        Controller_InitFrameNumber(controllerData);

        //
        // Exectute the _DSM method to simulate HSIC disconnect if required
        //
        Controller_ExecuteDSMForHSICDisconnectInU3(controllerData, TRUE);

        //
        // Objects enter D0 in the following order:
        //
        //  Interrupter_D0Entry
        //  DeviceSlot_D0Entry
        //  Command_D0Entry
        //
        for (pass = 1; pass <= 2; pass++) {

            if ((pass == 2) ||
                (Controller_D0EntryAttemptRestoreState(controllerData, WdfPreviousState) == FALSE)) {

                //
                // On a clean boot , the controller has been reset in
                // Register_PrepareHardware() callback, and for resume from
                // hibernate, we just reset the controller. So there is no
                // controller state to be restored.
                //
                // Or, if this is the second pass then the controller
                // has been reset here due to the failure of the
                // Controller Restore State operation, or the Controller
                // Restore State operation was not even attempted.
                //
                // The interrupter state (ERSTSZ, ERSTBA, ERDP, event
                // rings) should be reinitialized and should not be
                // preserved in this case.
                //
                status = Interrupter_D0Entry(controllerData->InterrupterHandle, WdfPreviousState, FALSE);

            } else {

                Register_D0EntryPreRestoreState(controllerData->RegisterHandle);

                //
                // The interrupter state (ERSTSZ, ERSTBA, ERDP, event
                // rings) should be preserved when resuming from Sleep
                // or controller selective suspend, prior to invoking
                // the Controller Restore State operation.
                //
                // See 4.23.2 xHCI Power Management, Required system
                // software steps for powering up and restoring xHC
                // state.
                //
                status = Interrupter_D0Entry(controllerData->InterrupterHandle, WdfPreviousState, TRUE);
            }

            if (!NT_SUCCESS(status)) {

                LogError(controllerData->RecorderLog, Controller,
                         "Interrupter_D0Entry Failed %!STATUS!", status);
                LEAVE;
            }

            status = DeviceSlot_D0Entry(controllerData->DeviceSlotHandle, WdfPreviousState);

            if (!NT_SUCCESS(status)) {

                LogError(controllerData->RecorderLog, Controller,
                         "DeviceSlot_D0Entry Failed %!STATUS!", status);
                LEAVE;
            }

            status = Command_D0Entry(controllerData->CommandHandle, WdfPreviousState);

            if (!NT_SUCCESS(status)) {

                LogError(controllerData->RecorderLog, Controller,
                         "Command_D0Entry Failed %!STATUS!", status);
                LEAVE;
            }

            if ((pass == 2) ||
                (WdfPreviousState == WdfPowerDeviceD3Final) ||
                (controllerData->SystemPowerAction == SystemPowerAction_ResumeFromHibernate)) {

                break;
            }

            //
            // Attempt to restore the controller to the
            // state that exisited prior to the system
            // suspend / resume.
            //
            if (Controller_D0EntryAttemptRestoreState(controllerData, WdfPreviousState)) {

                status = Controller_D0EntryRestoreState(controllerData);

                if (NT_SUCCESS(status)) {

                    //
                    // If restore state succeeds then a second
                    // pass is not necessary.
                    //
                    break;

                } else {

                    LogInfo(controllerData->RecorderLog, Controller,
                            "Controller_D0EntryRestoreState Failed %!STATUS!", status);

                    Controller_HwVerifierBreakIfEnabled(
                        controllerData,
                        NULL,
                        NULL,
                        ControllerHwVerifierControllerSaveRestoreFailed,
                        "Controller restore state operation failed",
                        NULL,
                        NULL);
                }

            } else {

                LogInfo(controllerData->RecorderLog, Controller,
                        "Skipping restore state");
            }

            //
            // If restore state failed or was not even attempted
            // then dump the device slot information that
            // existed prior to the system suspend / resume.
            //
            DeviceSlot_D0EntryCleanupState(controllerData->DeviceSlotHandle,
                                           WdfPreviousState);

            //
            // Reset the controller after the restore state failed.
            //
            status = Register_ControllerReset(controllerData->RegisterHandle, FALSE);

            if (!NT_SUCCESS(status)) {

                LogError(controllerData->RecorderLog, Controller,
                         "Register_ControllerReset Failed %!STATUS!", status);
                LEAVE;
            }

            controllerWasReset = TRUE;

            //
            // Now continue with a second pass after the
            // controller is reset.
            //
        }

        status = Register_D0Entry(controllerData->RegisterHandle, WdfPreviousState);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "Register_D0Entry Failed %!STATUS!", status);
            LEAVE;
        }

        status = RootHub_D0Entry(controllerData->RootHubHandle, WdfPreviousState);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "RootHub_D0Entry Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Only set to WdfPowerDeviceD0 on EvtDeviceD0Entry success.
        //
        controllerData->CurrentWdfPowerDeviceState = WdfPowerDeviceD0;

        if (controllerWasReset == TRUE) {

            UCX_CONTROLLER_RESET_COMPLETE_INFO  controllerResetCompleteInfo;

            UCX_CONTROLLER_RESET_COMPLETE_INFO_INIT(&controllerResetCompleteInfo,
                                                    UcxControllerStateLost,
                                                    FALSE);

            UcxControllerResetComplete(controllerData->UcxController,
                                       &controllerResetCompleteInfo);
        }

    } FINALLY {

        if (NT_SUCCESS(status)) {

            //
            // On a successfull D0Entry, set the system power action to none if the
            // system was powering up. This means that system has completed a boot
            // or resume operation and is fully powered. This state is used to
            // distinguish a device selective suspend from device going into low
            // power state due to System power down.
            //
            if (SystemPoweringUp(controllerData->SystemPowerAction)) {
                controllerData->SystemPowerAction = SystemPowerAction_None;
            }

            Controller_UpdateIdleTimeoutOnControllerFDOD0Entry(controllerData);

        } else {

            //
            // Mark all device slots as disabled.
            //
            DeviceSlot_DisableAllDeviceSlots(controllerData->DeviceSlotHandle);

            //
            // If EvtDeviceD0Entry() fails then the hardware can still be
            // accessed during the subsequent EvtDeviceReleaseHardware().
            // Go ahead and mark the controller gone now so that
            // nothing else will attempt to access the hardware other
            // than EvtDeviceReleaseHardware().
            //
            if (controllerGone) {
                Controller_SetControllerGone(controllerData, TRUE);
            } else {
                //
                // This is a fatal error
                //

                Controller_ReportFatalError(controllerData,
                                            Controller_FatalErrorNoAction,
                                            USB3_WER_BUGCODE_USBXHCI_D0ENTRY_FAILED,
                                            NULL,
                                            NULL,
                                            NULL);

                Controller_SetControllerGone(controllerData, FALSE);

            }
        }

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_D0_ENTRY_COMPLETE(
            NULL,
            controllerData->UcxController,
            WdfPreviousState,
            powerAction,
            status);
    }

    return status;
}

NTSTATUS
Controller_WdfEvtDeviceD0EntryPostInterruptsEnabled(
    WDFDEVICE               WdfDevice,
    WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    A driver's EvtDeviceD0EntryPostInterruptsEnabled event callback function performs
    device-specific operations that are required after the driver has enabled the device's
    hardware interrupts.

Return Value:

    If the EvtDeviceD0EntryPostInterruptsEnabled callback function encounters no errors,
    it must return STATUS_SUCCESS or another status value for which NT_SUCCESS(status)
    equals TRUE. Otherwise, it must return a status value for which
    NT_SUCCESS(status) equals FALSE.

    If NT_SUCCESS(status) equals FALSE, the framework calls the driver's
    EvtDeviceD0Exit callback function.

--*/
{
    NTSTATUS            status;
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;
    POWER_ACTION        powerAction;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        powerAction = WdfDeviceGetSystemPowerAction(WdfDevice);

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_D0_ENTRY_POST_INTERRUPTS_START(
            NULL,
            controllerData->UcxController,
            WdfPreviousState,
            powerAction,
            (USHORT)controllerData->WaitWakeQueued);

        LogInfo(controllerData->RecorderLog, Controller,
                "WdfDevice 0x%p %!WdfPowerDeviceState! %!SystemPowerAction!",
                WdfDevice, WdfPreviousState, controllerData->SystemPowerAction);

        //
        // Objects enter D0 in the following order:
        //
        //  Controller_D0EntryPostInterruptsEnabled
        //  RootHub_D0EntryPostInterruptsEnabled
        //  Command_D0EntryPostInterruptsEnabled
        //
        status = Controller_D0EntryPostInterruptsEnabled(controllerData, WdfPreviousState);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "Controller_D0EntryPostInterruptsEnabled Failed %!STATUS!", status);
            LEAVE;
        }

        status = RootHub_D0EntryPostInterruptsEnabled(controllerData->RootHubHandle, WdfPreviousState);

        if (!NT_SUCCESS(status)) {
            LogError(controllerData->RecorderLog, Controller,
                     "RootHub_D0EntryPostInterruptsEnabled Faile %!STATUS!", status);
            LEAVE;
        }

        status = Command_D0EntryPostInterruptsEnabled(controllerData->CommandHandle, WdfPreviousState);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "Command_D0EntryPostInterruptsEnabled Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // If ControllerNotSupported is set, it means that we do not want
        // to run on this controllers firmware.
        //
        if (controllerData->DeviceFlags.ControllerNotSupported) {
            status = STATUS_NOT_SUPPORTED;
            LogError(controllerData->RecorderLog, Controller,
                     "Controller Firmware is not supported");
            LEAVE;
        }

        //
        // Update the WMI controller capabilities now that the firmware has been
        // retrieved and the controller is running.
        //
        Wmi_CreateControllerCapabilities(controllerData->WmiHandle);

    } FINALLY {

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_D0_ENTRY_POST_INTERRUPTS_COMPLETE(
            NULL,
            controllerData->UcxController,
            WdfPreviousState,
            powerAction,
            status);
    }

    return status;
}

VOID
ControllerPreInterruptsDisableAcpiCallout(
    WDFDEVICE   WdfDevice
    )
/*++

Routine Description:

    Invokes IOCTL_ACPI_EVAL_METHOD IOCTL for the 'PRMI' method. This method is
    Qualcomm-specific. Currently it is used to - Prime the wake interrupt before
    interrupts are disabled.

--*/
{
    PDEVICE_DATA            deviceData;
    PCONTROLLER_DATA        controllerData;
    WDF_MEMORY_DESCRIPTOR   inputDescriptor;
    WDF_MEMORY_DESCRIPTOR   outputDescriptor;
    ACPI_EVAL_INPUT_BUFFER  inputBuffer;
    ACPI_EVAL_OUTPUT_BUFFER outputBuffer;
    WDFIOTARGET             ioTargetHandle;
    ULONG_PTR               sizeReturned;
    NTSTATUS                status;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        if (Controller_GetDeviceFlags(controllerData).DoPreInterruptsDisableAcpiCallout == 0) {
            LEAVE;
        }

        LogInfo(controllerData->RecorderLog,
                Controller,"WdfDevice 0x%p, Invoking 'PRMI' method", WdfDevice);

        RtlZeroMemory(&inputBuffer, sizeof(ACPI_EVAL_INPUT_BUFFER));

        inputBuffer.Signature         = ACPI_EVAL_INPUT_BUFFER_SIGNATURE;
        inputBuffer.MethodNameAsUlong = (ULONG)'IMRP';

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inputDescriptor,
                                          (PVOID) &inputBuffer,
                                          sizeof(ACPI_EVAL_INPUT_BUFFER));

        RtlZeroMemory(&outputBuffer, sizeof(ACPI_EVAL_OUTPUT_BUFFER));
        outputBuffer.Signature = ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE;

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor,
                                          (PVOID) &outputBuffer,
                                          sizeof(ACPI_EVAL_OUTPUT_BUFFER));

        ioTargetHandle = WdfDeviceGetIoTarget(WdfDevice);

        status = WdfIoTargetSendIoctlSynchronously(ioTargetHandle,
                                                   NULL,
                                                   IOCTL_ACPI_EVAL_METHOD,
                                                   &inputDescriptor,
                                                   &outputDescriptor,
                                                   NULL,
                                                   &sizeReturned);

        if (!NT_SUCCESS(status)) {

            LogInfo(controllerData->RecorderLog, Controller,
                    "WdfDevice 0x%p, IOCTL_ACPI_EVAL_METHOD(PRMI) failed with %!STATUS!", WdfDevice, status);
            LEAVE;
        }
        
        if (outputBuffer.Signature != ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE) {
            LogInfo(controllerData->RecorderLog, Controller,
                    "WdfDevice 0x%p, IOCTL_ACPI_EVAL_METHOD(PRMI) output buffer is malformed", WdfDevice);
            status = STATUS_ACPI_INVALID_DATA;
            LEAVE;
        }

        LogInfo(controllerData->RecorderLog, Controller,
                "WdfDevice 0x%p, IOCTL_ACPI_EVAL_METHOD(PRMI) success", WdfDevice);

    } FINALLY {

    }

    return;
}

NTSTATUS
Controller_WdfEvtDeviceD0ExitPreInterruptsDisabled(
    __in
        WDFDEVICE               WdfDevice,
    __in
        WDF_POWER_DEVICE_STATE  WdfTargetState
    )
{
    NTSTATUS            status;
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;
    POWER_ACTION        powerAction;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        powerAction = WdfDeviceGetSystemPowerAction(WdfDevice);

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_D0_EXIT_PRE_INTERRUPTS_START(
            NULL,
            controllerData->UcxController,
            WdfTargetState,
            powerAction,
            (USHORT)controllerData->WaitWakeQueued);

        LogInfo(controllerData->RecorderLog, Controller,
                "WdfDevice 0x%p %!WdfPowerDeviceState! %!SystemPowerAction!",
                WdfDevice, WdfTargetState, controllerData->SystemPowerAction);

        ControllerPreInterruptsDisableAcpiCallout(WdfDevice);

        Interrupter_D0ExitPreInterruptsDisabled(controllerData->InterrupterHandle, WdfTargetState);

    } FINALLY {

        status = STATUS_SUCCESS;

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_D0_EXIT_PRE_INTERRUPTS_COMPLETE(
            NULL,
            controllerData->UcxController,
            WdfTargetState,
            powerAction,
            status);
    }

    return status;
}

NTSTATUS
Controller_WdfEvtDeviceD0Exit(
    WDFDEVICE               WdfDevice,
    WDF_POWER_DEVICE_STATE  WdfTargetState
    )
/*++

Routine Description:

    A driver's EvtDeviceD0Exit event callback function performs operations
    that are needed when the driver's device leaves the D0 power state.

Return Value:

    If the EvtDeviceD0Exit callback function encounters no errors, it must
    return STATUS_SUCCESS or another status value for which NT_SUCCESS(status)
    equals TRUE. Otherwise, it must return a status value for which
    NT_SUCCESS(status) equals FALSE.

--*/
{
    NTSTATUS            status;
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;
    POWER_ACTION        powerAction;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        powerAction = WdfDeviceGetSystemPowerAction(WdfDevice);

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_D0_EXIT_START(
            NULL,
            controllerData->UcxController,
            WdfTargetState,
            powerAction,
            (USHORT)controllerData->WaitWakeQueued);

        LogInfo(controllerData->RecorderLog, Controller,
                "WdfDevice 0x%p %!WdfPowerDeviceState! %!SystemPowerAction!",
                WdfDevice, WdfTargetState, controllerData->SystemPowerAction);

        controllerData->CurrentWdfPowerDeviceState = WdfTargetState;

        NT_ASSERT(SystemPoweringUp(controllerData->SystemPowerAction) == FALSE);

        //
        // If this controller is used for hibernate, do not stop the controller
        // here. Stop the WdfTimer that checks if the controller is operational.
        // The initialization of the hibernate stack would take care of reinitializing
        // the controller (if required) and also stopping it when done.
        //
        if ((Controller_GetHibernateContext(controllerData) != NULL) &&
            (WdfTargetState == WdfPowerDevicePrepareForHibernation)) {

            WdfTimerStop(controllerData->WdfTimer, TRUE);
            LEAVE;
        }

        //
        // Objects exit D0 in the following order:
        //
        //  CommonBuffer_D0Exit
        //  RootHub_D0Exit
        //  Controller_D0Exit
        //  Interrupter_D0ExitStopped
        //  Controller_D0ExitSaveState
        //
        CommonBuffer_D0Exit(controllerData->CommonBufferHandle, WdfTargetState);

        RootHub_D0Exit(controllerData->RootHubHandle, WdfTargetState, controllerData->SystemPowerAction);

        Controller_D0Exit(controllerData, WdfTargetState);

        Interrupter_D0ExitStopped(controllerData->InterrupterHandle, WdfTargetState);

        if (Controller_D0ExitAttemptSaveState(controllerData, WdfTargetState) == FALSE) {

            LogInfo(controllerData->RecorderLog, Controller,
                    "Skipping save state");
            LEAVE;
        }

        status = Controller_D0ExitSaveState(controllerData);

        if (!NT_SUCCESS(status)) {

            controllerData->ControllerSaveStateFailed = TRUE;

            LogInfo(controllerData->RecorderLog, Controller,
                    "Controller_D0ExitSaveState Failed %!STATUS!", status);

            Controller_HwVerifierBreakIfEnabled(
                controllerData,
                NULL,
                NULL,
                ControllerHwVerifierControllerSaveRestoreFailed,
                "Controller save state operation failed",
                NULL,
                NULL);
        }

        Register_D0ExitPostSaveSate(controllerData->RegisterHandle);

        //
        // Exectute the _DSM method to simulate HSIC disconnect if required
        //
        Controller_ExecuteDSMForHSICDisconnectInU3(controllerData, FALSE);

    } FINALLY {

        if (Controller_GetDeviceFlags(controllerData).ResetControllerOnShutdown &&
            (powerAction == PowerActionShutdownReset)) {

            //
            // Reset the controller on shutdown in case the BIOS
            // restarts the controller without resetting it first in a
            // shutdown and restart scenario.
            //
            status = Register_ControllerReset(controllerData->RegisterHandle, TRUE);

            if (!NT_SUCCESS(status)) {

                LogInfo(controllerData->RecorderLog, Controller,
                        "Register_ControllerReset Failed %!STATUS!", status);
            }
        }

        //
        // Consider moving to EvtDeviceSelfManagedIoSuspend in future.
        //
        WdfWorkItemFlush(controllerData->TelemetryReportWorkItem);
        WdfWorkItemFlush(controllerData->IdleTimeoutUpdateWorkItem);

        status = STATUS_SUCCESS;

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_D0_EXIT_COMPLETE(
            NULL,
            controllerData->UcxController,
            WdfTargetState,
            powerAction,
            status);
    }

    return status;
}

NTSTATUS
Controller_WdfEvtDeviceArmWakeFromS0(
    WDFDEVICE   WdfDevice
    )
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;
    PROOTHUB_DATA       rootHubData;
    PULONG              portSCRegister;
    PORTSC              portSC;
    ULONG               i;
    KIRQL               irql;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;
        rootHubData = controllerData->RootHubHandle;
        LogInfo(controllerData->RecorderLog,
                Controller,"WdfDevice 0x%p ArmWakeFromS0", WdfDevice);

        controllerData->WaitWakeQueued = TRUE;

        for (i = 0; i < rootHubData->NumberOfPorts; i++) {
            KeAcquireSpinLock(&rootHubData->PortData[i].ReadModifyWriteLock, &irql);
            portSCRegister = &rootHubData->PortRegisterSet[i].PortStatusAndControl.AsUlong32;

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

            portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
            portSC.WakeOnConnectEnable     = 1;
            portSC.WakeOnDisconnectEnable  = 1;
            portSC.WakeOnOverCurrentEnable = 1;
            Register_WriteUlong(portSCRegister, portSC.AsUlong32);

            KeReleaseSpinLock(&rootHubData->PortData[i].ReadModifyWriteLock, irql);
        }

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

VOID
Controller_WdfEvtDeviceDisarmWakeFromS0(
    WDFDEVICE   WdfDevice
    )
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;
    PROOTHUB_DATA       rootHubData;
    PULONG              portSCRegister;
    PORTSC              portSC;
    ULONG               i;
    KIRQL               irql;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;
        rootHubData = controllerData->RootHubHandle;
        LogInfo(controllerData->RecorderLog,
                Controller,"WdfDevice 0x%p DisarmWakeFromS0", WdfDevice);

        controllerData->WaitWakeQueued = FALSE;
        //
        // We don't want to disable the wake bits for some controllers that can't
        // reliably detect connect changes without them set.
        //
        if (Controller_GetDeviceFlags(controllerData).ForceWakeEnableBitsInD0) {
            return;
        }

        for (i = 0; i < rootHubData->NumberOfPorts; i++) {
            KeAcquireSpinLock(&rootHubData->PortData[i].ReadModifyWriteLock, &irql);
            portSCRegister = &rootHubData->PortRegisterSet[i].PortStatusAndControl.AsUlong32;

            portSC.AsUlong32 = Register_ReadUlong(portSCRegister);

            portSC.AsUlong32 &= PORTSC_RMW_PRESERVATION_MASK;
            portSC.WakeOnConnectEnable     = 0;
            portSC.WakeOnDisconnectEnable  = 0;
            portSC.WakeOnOverCurrentEnable = 0;
            Register_WriteUlong(portSCRegister, portSC.AsUlong32);

            KeReleaseSpinLock(&rootHubData->PortData[i].ReadModifyWriteLock, irql);
        }

    } FINALLY {

    }

    return;
}

NTSTATUS
Controller_WdfEvtDeviceArmWakeFromSx(
    WDFDEVICE   WdfDevice
    )
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;
        LogInfo(controllerData->RecorderLog,
                Controller,"WdfDevice 0x%p ArmWakeFromSx", WdfDevice);

        controllerData->WaitWakeQueued = TRUE;

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

VOID
Controller_WdfEvtDeviceDisarmWakeFromSx(
    WDFDEVICE   WdfDevice
    )
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;
        LogInfo(controllerData->RecorderLog,
                Controller,"WdfDevice 0x%p DisarmWakeFromSx", WdfDevice);

        controllerData->WaitWakeQueued = FALSE;

    } FINALLY {

    }

    return;
}

VOID
Controller_WdfEvtDeviceWakeFromS0Triggered(
    WDFDEVICE   WdfDevice
    )
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        LogInfo(controllerData->RecorderLog,
                Controller,"WdfDevice 0x%p WakeFromS0Triggered", WdfDevice);

        //
        // Tell the class extension that a root hub port changed.
        //
        UcxRootHubPortChanged(controllerData->RootHubHandle->UcxRootHub);

    } FINALLY {

    }

    return;
}

VOID
Controller_WdfEvtCleanupCallback(
    WDFOBJECT   WdfDevice
    )
/*++

Routine Description:

    The driver's EvtCleanupCallback event callback function removes the driver's
    references on an object so that the object can be deleted.

--*/
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;


        WppRecorderLogDelete(deviceData->RecorderLog);
        deviceData->RecorderLog = NULL;

        //
        // In Controller_WdfEvtDeviceAdd() the call to Controller_CreateWdfDevice()
        // can succeed but then the call to Controller_Create() can fail, in which
        // case deviceData->ControllerData will be NULL.
        //
        if (controllerData != NULL) {

            //
            // Remove the controller from the global controller list.
            //
            Controller_RemoveControllerFromGlobalControllerList(controllerData);

            Etw_ControllerDelete(NULL, controllerData);
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_SetLogIdentifier(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    CHAR logIdentifier[RECORDER_LOG_IDENTIFIER_MAX_CHARS + sizeof(NULL)];

    PAGED_CODE();

    TRY {

        if (ControllerData->RecorderLog == NULL) {
            LEAVE;
        }

        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {

            RtlStringCchPrintfA((PCHAR)&logIdentifier,
                                sizeof(logIdentifier),
                                "%02d %04x %04x",
                                Controller_GetInstanceNumber(ControllerData),
                                Controller_GetPciVendorID(ControllerData),
                                Controller_GetPciDeviceID(ControllerData));

        } else {

            RtlStringCchPrintfA((PCHAR)&logIdentifier,
                                sizeof(logIdentifier),
                                "%02d %s %s",
                                Controller_GetInstanceNumber(ControllerData),
                                Controller_GetAcpiVendorID(ControllerData),
                                Controller_GetAcpiDeviceID(ControllerData));
        }

        WppRecorderLogSetIdentifier(ControllerData->RecorderLog, (PSTR)&logIdentifier);

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
DEVICE_ENUMERATOR
Controller_GetDeviceEnumerator(
    __in
        WDFDEVICE       WdfDevice,
    __in
        RECORDER_LOG    RecorderLog
    )
/*++

Routine Description:

    Determine which bus driver enumerated the PDO of this driver stack.
    This can be used to determine the underlying bus the controller is
    located on.

--*/
{
    DEVICE_ENUMERATOR               enumerator;
    NTSTATUS                        status;
    ULONG                           resultLength;
    WCHAR                           enumeratorName[ENUMERATOR_NAME_SIZE];

    PAGED_CODE();

    TRY {

        //
        // Assume that the enumerator is PCI.
        //
        enumerator = DeviceEnumeratorPci;

        status = IoGetDeviceProperty(WdfDeviceWdmGetPhysicalDevice(WdfDevice),
                                     DevicePropertyEnumeratorName,
                                     sizeof(enumeratorName),
                                     enumeratorName,
                                     &resultLength);

        if (!NT_SUCCESS(status)) {
            LogError(RecorderLog, Controller, "IoGetDeviceProperty for DevicePropertyEnumeratorName failed %!STATUS!", status);
            LEAVE;
        }

        //
        // The device property is guaranteed to be null-terminated.
        //
        __analysis_assume_nullterminated(enumeratorName);

        if (_wcsicmp(enumeratorName, ENUMERATOR_NAME_ACPI) == 0) {

            enumerator = DeviceEnumeratorAcpi;

        } else if (_wcsicmp(enumeratorName, URS_ENUMERATOR_NAME) == 0) {

            enumerator = DeviceEnumeratorUrs;

        } else {

            NT_ASSERT(_wcsicmp(enumeratorName, ENUMERATOR_NAME_PCI) == 0);
        }

    } FINALLY {

        LogInfo(RecorderLog, Controller, "%!DeviceEnumerator!", enumerator);
    }

    return enumerator;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_Create(
    __in
        WDFDEVICE       WdfDevice,
    __in
        RECORDER_LOG    RecorderLog,
    __in
        ULONG           InstanceNumber,
    __out
        PHANDLE         ObjectHandle
    )
/*++

Routine Description:

    This function initializes the controller object and creates any resources
    that are required for the controller.

--*/
{
    NTSTATUS                                status;
    UCX_CONTROLLER_CONFIG                   ucxControllerConfig;
    WDF_OBJECT_ATTRIBUTES                   wdfAttributes;
    UCXCONTROLLER                           ucxController;
    PCONTROLLER_DATA                        controllerData;
    WDF_TIMER_CONFIG                        wdfTimerConfig;
    WDF_WORKITEM_CONFIG                     wdfWorkItemConfig;
    ULONGLONG                               firmwareVersion;
    UCX_CONTROLLER_PARENT_BUS_TYPE          parentBusType;
    DEVICE_ENUMERATOR                       enumerator;
    RECORDER_LOG_CREATE_PARAMS              runDownLogCreateParams;
    ACPI_DEVICE_INFORMATION                 acpiDeviceInformation;
    PCI_DEVICE_INFORMATION                  pciDeviceInformation;
    PXHCI_TELEMETRY_DATA                    telemetryData;

    PAGED_CODE();

    TRY {

        firmwareVersion = ULONGLONG_MAX;

        RtlZeroMemory(&acpiDeviceInformation, sizeof(ACPI_DEVICE_INFORMATION));
        RtlZeroMemory(&pciDeviceInformation, sizeof(PCI_DEVICE_INFORMATION));

        //
        // Create a separate log for run down events. This log needs to be written to sparingly
        // in order to keep its size minimal. The log should contain mostly static rundown events
        // This log will be added to minidumps in case of system crash by IFR bugcheck callback
        // and can be used to obtain data about controller from minidumps.
        //
        RECORDER_LOG_CREATE_PARAMS_INIT(&runDownLogCreateParams, NULL);

        runDownLogCreateParams.TotalBufferSize = 512;
        runDownLogCreateParams.ErrorPartitionSize = 0;

        RtlStringCchPrintfA(runDownLogCreateParams.LogIdentifier,
                            sizeof(runDownLogCreateParams.LogIdentifier),
                            "%02d RUNDOWN",
                            InstanceNumber);

        //
        // Prepare to create the UCX controller object.
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, CONTROLLER_DATA);

        UCX_CONTROLLER_CONFIG_INIT(&ucxControllerConfig, "USBXHCI");

        ucxControllerConfig.EvtControllerUsbDeviceAdd           = UsbDevice_UcxEvtDeviceAdd;
        ucxControllerConfig.EvtControllerGetCurrentFrameNumber  = Controller_UcxEvtGetCurrentFrameNumber;
        ucxControllerConfig.EvtControllerGetDumpData            = Crashdump_UcxEvtGetDumpData;
        ucxControllerConfig.EvtControllerFreeDumpData           = Crashdump_UcxEvtFreeDumpData;
        ucxControllerConfig.EvtControllerReset                  = Controller_UcxEvtReset;
        ucxControllerConfig.EvtControllerEnableForwardProgress  = Controller_UcxEvtEnableForwardProgress;
        ucxControllerConfig.EvtControllerQueryUsbCapability     = Controller_UcxEvtQueryUsbCapability;

        //
        // Determine the parent bus type. If the enumerator is PCI or ACPI, then the
        // parent bus is PCI or ACPI respectively. If the enumerator is URS (USB Role-Switch),
        // then we need to query the actual underlying bus (PCI or ACPI).
        //

        enumerator = Controller_GetDeviceEnumerator(WdfDevice, RecorderLog);

        if (enumerator == DeviceEnumeratorPci) {

            parentBusType = UcxControllerParentBusTypePci;

            //
            // Retrieve the PCI data, which includes the Vendor ID, Device ID, and Revision.
            //
            status = Controller_RetrievePciData(WdfDevice,
                                                RecorderLog,
                                                &pciDeviceInformation,
                                                &firmwareVersion);

            if (!NT_SUCCESS(status)) {

                LogError(RecorderLog, Controller, "Controller_RetrievePciData Failed %!STATUS!", status);
                LEAVE;
            }

        } else if (enumerator == DeviceEnumeratorAcpi) {

            parentBusType = UcxControllerParentBusTypeAcpi;

            status = Controller_RetrieveAcpiData(WdfDevice,
                                                 RecorderLog,
                                                 &acpiDeviceInformation);

            if (!NT_SUCCESS(status)) {

                LogError(RecorderLog, Controller, "Controller_RetrieveAcpiData Failed %!STATUS!", status);
                LEAVE;
            }

        } else {

            NT_ASSERT(enumerator == DeviceEnumeratorUrs);

            //
            // This is a dual-role USB controller currently in host mode.
            // Retrieve properties of the actual underlying bus.
            //

            status = Controller_RetrieveUrsData(WdfDevice,
                                                RecorderLog,
                                                &parentBusType,
                                                &acpiDeviceInformation,
                                                &pciDeviceInformation,
                                                &firmwareVersion);

            if (!NT_SUCCESS(status)) {

                LogError(RecorderLog, Controller, "Controller_RetrieveUrsData Failed %!STATUS!", status);
                LEAVE;
            }
        }

        //
        // Now that we know the actual parent bus type, set the UCX information accordingly.
        //

        switch (parentBusType) {

        case UcxControllerParentBusTypeAcpi:
            UCX_CONTROLLER_CONFIG_SET_ACPI_INFO(&ucxControllerConfig,
                                                acpiDeviceInformation.VendorId,
                                                acpiDeviceInformation.DeviceId,
                                                acpiDeviceInformation.RevisionId);
            break;

        default:
            NT_ASSERT(parentBusType == UcxControllerParentBusTypePci);
            UCX_CONTROLLER_CONFIG_SET_PCI_INFO(&ucxControllerConfig,
                                               pciDeviceInformation.VendorId,
                                               pciDeviceInformation.DeviceId,
                                               pciDeviceInformation.RevisionId,
                                               pciDeviceInformation.BusNumber,
                                               pciDeviceInformation.DeviceNumber,
                                               pciDeviceInformation.FunctionNumber);
            break;
        }

        //
        // Create the UCX controller object.
        //
        status = UcxControllerCreate(WdfDevice,
                                     &ucxControllerConfig,
                                     &wdfAttributes,
                                     &ucxController);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller, "UcxControllerCreate Failed %!STATUS!", status);
            LEAVE;
        }

        controllerData = GetControllerData(ucxController);

        controllerData->WdfDevice           = WdfDevice;
        controllerData->UcxController       = ucxController;
        controllerData->RecorderLog         = RecorderLog;
        controllerData->FirmwareVersion     = firmwareVersion;
        controllerData->InstanceNumber      = InstanceNumber;
        controllerData->ParentBusType       = parentBusType;

        if (controllerData->ParentBusType == UcxControllerParentBusTypePci) {

            RtlCopyMemory(&controllerData->PciDeviceInfo,
                          &pciDeviceInformation,
                          sizeof(PCI_DEVICE_INFORMATION));

            controllerData->AcpiDeviceInfo.VendorId[0]      = ANSI_NULL;
            controllerData->AcpiDeviceInfo.DeviceId[0]      = ANSI_NULL;
            controllerData->AcpiDeviceInfo.RevisionId[0]    = ANSI_NULL;

        } else {

            RtlCopyMemory(&controllerData->AcpiDeviceInfo,
                          &acpiDeviceInformation,
                          sizeof(ACPI_DEVICE_INFORMATION));

            controllerData->PciDeviceInfo.VendorId          = LONG_MAX;
            controllerData->PciDeviceInfo.DeviceId          = LONG_MAX;
            controllerData->PciDeviceInfo.RevisionId        = 0;
            controllerData->PciDeviceInfo.BusNumber         = 0;
            controllerData->PciDeviceInfo.DeviceNumber      = 0;
            controllerData->PciDeviceInfo.FunctionNumber    = 0;
        }

        //
        // Update the recorder log ID.
        //
        Controller_SetLogIdentifier(controllerData);
        *ObjectHandle = (HANDLE)controllerData;

        //
        // Create a WDF timer that is used to track the xHCI controller.
        //
        WDF_TIMER_CONFIG_INIT_PERIODIC(&wdfTimerConfig,
                                       Controller_WdfEvtTimerFunction,
                                       CONTROLLER_WATCHDOG_TIMER_INTERVAL_MS);

        wdfTimerConfig.TolerableDelay = CONTROLLER_WATCHDOG_TIMER_TOLERABLE_DELAY_MS;

        WDF_OBJECT_ATTRIBUTES_INIT(&wdfAttributes);
        wdfAttributes.ParentObject = ucxController;

        status = WdfTimerCreate(&wdfTimerConfig, &wdfAttributes, &controllerData->WdfTimer);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller, "WdfTimerCreate Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Create a WDF work item that is used to update the Controller FDO S0 Idle
        // timeout value at PASSIVE_LEVEL after RootHub PDO D0Entry.
        //
        WDF_OBJECT_ATTRIBUTES_INIT(&wdfAttributes);
        wdfAttributes.ParentObject = ucxController;

        WDF_WORKITEM_CONFIG_INIT(&wdfWorkItemConfig, Controller_IdleTimeoutUpdateWorker);

        status = WdfWorkItemCreate(&wdfWorkItemConfig,
                                   &wdfAttributes,
                                   &controllerData->IdleTimeoutUpdateWorkItem);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller, "WdfWorkItemCreate Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Create a WDF work item that is used to write live kernel dump
        // at PASSIVE_LEVEL.
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, XHCI_TELEMETRY_DATA);
        wdfAttributes.ParentObject = ucxController;

        WDF_WORKITEM_CONFIG_INIT(&wdfWorkItemConfig, Controller_TelemetryReportWorker);

        status = WdfWorkItemCreate(&wdfWorkItemConfig,
                                   &wdfAttributes,
                                   &controllerData->TelemetryReportWorkItem);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller, "WdfWorkItemCreate Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Initialize Telemetry Data
        //
        telemetryData = GetTelemetryData(controllerData->TelemetryReportWorkItem);
        KeInitializeMutex(&telemetryData->XhciTelemetryMutex, 0);
        InitializeListHead(&telemetryData->XhciTelemetryDataListHead);
        KeInitializeSpinLock(&telemetryData->XhciTelemetryDataListLock);

        //
        // Retrieve device flags for the xHCI controller. This must be done
        // after the PCI config is retrieved, since the PCI VEN/DEV maybe
        // used to determine which flags to set. Also, it utilizes the
        // TelemetryReportWorkItem.
        //
        Controller_PopulateDeviceFlags(controllerData);

        if (controllerData->DeviceFlags.ControllerNotSupported) {
            status = STATUS_NOT_SUPPORTED;
            LogError(RecorderLog, Controller, "Controller is not supported");
            LEAVE;
        }

        Controller_PopulateTestConfiguration(controllerData);

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateTestConfiguration(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    This routine queries the USB Test Configuration registry keys and populates
    and test parameters, if TestMode is ON.

--*/
{
    WDFKEY              deviceSubKey;
    NTSTATUS            status;
    WDFKEY              testConfigKey;
    PTEST_CONFIGURATION testConfiguration;

    DECLARE_UNICODE_STRING_SIZE(deviceSubkeyName, VID_PID_REV_BUFFER_SIZE);

    PAGED_CODE();

    TRY {

        deviceSubKey      = NULL;
        testConfigKey     = NULL;
        testConfiguration = &ControllerData->TestConfiguration;

        //
        // Since 0 could be a valid value for a test hook, initialize all test
        // hooks to ULONG_MAX.
        //
        RtlFillMemory(testConfiguration, sizeof(TEST_CONFIGURATION), UCHAR_MAX);

        if (g_WdfDriverUsbXhciContext->TestModeOn == FALSE) {
            LEAVE;
        }

        status = WdfRegistryOpenKey(NULL,
                                    &g_TestConfigKeyName,
                                    KEY_READ,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &testConfigKey);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {

            status = RtlUnicodeStringPrintf(&deviceSubkeyName,
                                            L"%04X%04X%02X",
                                            Controller_GetPciVendorID(ControllerData),
                                            Controller_GetPciDeviceID(ControllerData),
                                            Controller_GetPciRevisionID(ControllerData));

        } else if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypeAcpi) {

            status =  RtlUnicodeStringPrintf(&deviceSubkeyName,
                                             L"%S%S%S",
                                             Controller_GetAcpiVendorID(ControllerData),
                                             Controller_GetAcpiDeviceID(ControllerData),
                                             Controller_GetAcpiRevisionID(ControllerData));

        } else {

            LogError(ControllerData->RecorderLog, Controller, "Unsupported controller parent bus type.");
            LEAVE;
        }

        if (!NT_SUCCESS(status)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Unable to format VendorId/DeviceId/Rev subkey name %!STATUS!", status);
            LEAVE;
        }

        status = WdfRegistryOpenKey(testConfigKey,
                                    &deviceSubkeyName,
                                    KEY_READ,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &deviceSubKey);

        if (!NT_SUCCESS(status)) {

            status = WdfRegistryOpenKey(testConfigKey,
                                        &g_TestConfigGlobalKeyName,
                                        KEY_READ,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &deviceSubKey);
        }

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        (VOID)WdfRegistryQueryValue(deviceSubKey,
                                    &G_TestConfigCommonBufferDisableCachingKeyValue,
                                    sizeof(ULONG),
                                    &testConfiguration->CommonBufferDisableCaching,
                                    NULL,
                                    NULL);

        (VOID)WdfRegistryQueryValue(deviceSubKey,
                                    &G_TestConfigMaxDmaMapRegistersKeyValue,
                                    sizeof(ULONG),
                                    &testConfiguration->MaxDmaMapRegisters,
                                    NULL,
                                    NULL);

        (VOID)WdfRegistryQueryValue(deviceSubKey,
                                    &G_TestConfigEventRingSegmentCount,
                                    sizeof(ULONG),
                                    &testConfiguration->EventRingSegmentCount,
                                    NULL,
                                    NULL);

        (VOID)WdfRegistryQueryValue(deviceSubKey,
                                    &G_TestConfigEventRingSegmentSize,
                                    sizeof(ULONG),
                                    &testConfiguration->EventRingSegmentSize,
                                    NULL,
                                    NULL);

    } FINALLY {

        if (deviceSubKey) {
            WdfRegistryClose(deviceSubKey);
        }

        if (testConfigKey) {
            WdfRegistryClose(testConfigKey);
        }
    }

    return;
}

VOID
Controller_InitializeDeviceNotificationRegister(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    Initialize the Device Notification Control (DNCTRL) register. Specifically,
    enable FUNCTION_WAKE notification.

--*/
{
    DNCTRL                  deviceNotificationControl;
    POPERATIONAL_REGISTERS  operationalRegisters;

    operationalRegisters = Register_GetOperationalRegisters(ControllerData->RegisterHandle);

    //
    // 5.4.4 Device Notification Control Register (DNCTRL)
    //
    // Enable Device Notification Event generation if a Device
    // Notification TP is received with its Notification Type field
    // set to '1' (FUNCTION_WAKE), etc.
    //
    deviceNotificationControl.AsUlong32 = 0;
    deviceNotificationControl.EnableFunctionWakeNotification = 1;
    Register_WriteUlong(&operationalRegisters->DeviceNotificationControl.AsUlong32,
                        deviceNotificationControl.AsUlong32);
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_D0EntryRestoreState(
    __in
        PCONTROLLER_DATA        ControllerData
    )
/*++

Routine Description:

    This function is called when the controller enters D0 while the
    controller is still halted.

--*/
{
    NTSTATUS                status;
    ULONG                   count;
    LARGE_INTEGER           time;
    POPERATIONAL_REGISTERS  operationalRegisters;
    USBCMD                  usbCommand;
    USBSTS                  usbStatus;
    ULONGLONG               beginTime;
    ULONGLONG               currentTime;

    TRY {

        LogInfo(ControllerData->RecorderLog, Controller,
                   "Begin: Controller Restore State operation");

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_RESTORE_STATE_START(
            NULL,
            ControllerData->UcxController);

        operationalRegisters = Register_GetOperationalRegisters(ControllerData->RegisterHandle);

        usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.HcHalted == FALSE) {
            LogError(ControllerData->RecorderLog, Controller, 
                "HW_COMPLIANCE: Cannot restore state because controller is not halted! Returning STATUS_ADAPTER_HARDWARE_ERROR");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        if ((usbStatus.SaveStateStatus == TRUE)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Cannot restore state because Controller Save State operation is currently in progress, returning STATUS_ADAPTER_HARDWARE_ERROR");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        if ((usbStatus.RestoreStateStatus == TRUE)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Cannot restore state because Controller Restore State operation is currently in progress, returning STATUS_ADAPTER_HARDWARE_ERROR");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        //
        // DNCTRL is one of the registers that must be restored before restoring controller state.
        //
        Controller_InitializeDeviceNotificationRegister(ControllerData);

        //
        // Restore the controller state.
        //
        LogInfo(ControllerData->RecorderLog, Controller,
                   "Begin: Controller Restore State register write");
        beginTime = KeQueryUnbiasedInterruptTime();

        usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
        usbCommand.ControllerRestoreState = TRUE;
        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

        count = DEFAULT_STATE_RESTORE_WAIT_COUNT;

        do {

            usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

            if (usbStatus.SaveRestoreError == TRUE) {
                currentTime = KeQueryUnbiasedInterruptTime();
                LogError(ControllerData->RecorderLog, Controller, 
                    "HW_COMPLIANCE: Controller Restore State failed after %I64u milliseconds because usbStatus.SaveRestoreError is 1!",
                    ((currentTime - beginTime)/10000ULL));

                //
                // Clear the Save/Restore Error
                //
                usbStatus.AsUlong32 = 0;
                usbStatus.SaveRestoreError = TRUE;
                Register_WriteUlong(&operationalRegisters->UsbStatus.AsUlong32, usbStatus.AsUlong32);

                status = STATUS_ADAPTER_HARDWARE_ERROR;
                break;
            }

            if (usbStatus.RestoreStateStatus == FALSE) {
                currentTime = KeQueryUnbiasedInterruptTime();
                LogInfo(ControllerData->RecorderLog, Controller, "Controller Restore State successful after %I64u microseconds", 
                    ((currentTime - beginTime)/10ULL));
                status = STATUS_SUCCESS;
                break;
            }

            if (count == 0) {
                currentTime = KeQueryUnbiasedInterruptTime();
                LogError(ControllerData->RecorderLog, Controller,
                         "HW_COMPLIANCE: Controller Restore State failed to complete after %I64u milliseconds. Returning STATUS_ADAPTER_HARDWARE_ERROR.",
                         ((currentTime - beginTime)/10000ULL));
                NT_ASSERTMSG("Controller Restore State failed to complete?", 0);
                status = STATUS_ADAPTER_HARDWARE_ERROR;
                break;
            }

            //
            // Delay for 200 microseconds before re-reading the register.
            //
            time.QuadPart = WDF_REL_TIMEOUT_IN_US(DEFAULT_STATE_RESTORE_WAIT_TIME);

            KeDelayExecutionThread(KernelMode, FALSE, &time);

            count--;

        } WHILE (TRUE);

    } FINALLY {

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_RESTORE_STATE_COMPLETE(
            NULL,
            ControllerData->UcxController,
            status);

        if (!NT_SUCCESS(status)) {
            ControllerData->Counter.RestoreStateFailureCount += 1;
        }
    }

    return status;
}

VOID
Controller_HighResTimerCompletion(
    PEX_TIMER Timer,
    PVOID Context
    )
/*++

Routine Description:

    Completion routine for high resolution timer.

--*/
{
    PKEVENT event;

    UNREFERENCED_PARAMETER(Timer);

    event = (PKEVENT)Context;

    __analysis_assume(Context != NULL);

    KeSetEvent(event, IO_NO_INCREMENT, FALSE);
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_Start(
    __in
        PCONTROLLER_DATA        ControllerData
    )
/*++

Routine Description:

    This routine sets the USB Command Register (USBCMD) Run/Stop (R/S)
    bit to start a halted host controller.  It also sets the Interrupter
    Enable (INTE) bit to enable host controller interrupt generation.

    This is also the routine that is responsible for setting the Device
    Notification Control Register (DNCTRL) bit to enable FUNCTION_WAKE
    notifications.

    This routine is called during D0 entry and also during non-PnP reset
    recovery after the host controller has been reset.

    The host controller should be halted when this routine is called.

--*/
{
    NTSTATUS                status;
    POPERATIONAL_REGISTERS  operationalRegisters;
    USBCMD                  usbCommand;
    USBSTS                  usbStatus;
    ULONG                   i;
    ULONGLONG               beginTime;
    ULONGLONG               currentTime;
    KEVENT                  event;
    PEX_TIMER               timer;
    LONGLONG                dueTime;

    TRY {

        timer = NULL;

        LogInfo(ControllerData->RecorderLog, Controller,
                   "Begin: Controller Start operation");

        if (Controller_IsControllerAccessible(ControllerData) == FALSE) {
            status = STATUS_SUCCESS;
            LEAVE;
        }

        operationalRegisters = Register_GetOperationalRegisters(ControllerData->RegisterHandle);

        //
        // 5.4.2 USB Status Register (USBSTS)
        //
        // Verify that the controller is halted. This is required before setting
        // the run bit.
        //
        usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.HcHalted != TRUE) {

            LogError(ControllerData->RecorderLog, Controller, "HW_COMPLIANCE: The xHC must be halted before software can set the run bit!");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            NT_ASSERTMSG("Controller is not halted", 0);
            LEAVE;
        }

        //
        // Spec does not say when DNCTRL should be written. Do this before starting the controller,
        // to be safe. Note that on resume from Dx, this register will be written once before
        // restoring state and once before starting the controller.
        //
        // N.B: WIN8 behavior was to initialize DNCTRL after starting the controller.
        //
        Controller_InitializeDeviceNotificationRegister(ControllerData);

        //
        // 5.4.1 USB Command Register (USBCMD)
        //
        // Enable interrupts, set the run bit, and enable additional completion codes & errors
        // as supported by the controller
        //
        usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
        usbCommand.InterrupterEnable = TRUE;
        usbCommand.RunStopBit = TRUE;

        usbCommand.CEMEnable = (Register_GetConfigureEndpointCommandMaxExitLatency(ControllerData->RegisterHandle) != 0);
        usbCommand.ExtendedTBCEnable = ((Register_GetExtendedTBC(ControllerData->RegisterHandle) != 0) &&
                                        (Register_GetLargeESITPayload(ControllerData->RegisterHandle) != 0));

        LogInfo(ControllerData->RecorderLog, Controller,
                   "Begin: Controller Start register write");
        beginTime = KeQueryUnbiasedInterruptTime();
        currentTime = 0;

        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

        //
        // 5.4.2 USB Status Register (USBSTS)
        //
        // Wait for the HcHalted bit to clear after setting the RunStop bit.
        //
        for (i = 0; i < CONTROLLER_PRIMARY_POLLING_COUNT_LIMIT; i++) {
            KeStallExecutionProcessor(CONTROLLER_PRIMARY_POLLING_FREQUENCY_IN_uS);
            usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);
            if (usbStatus.HcHalted != TRUE) {
                currentTime = KeQueryUnbiasedInterruptTime();
                break;
            }
        }

        if (usbStatus.HcHalted == TRUE) {

            NT_ASSERTMSG("IRQL is assumed to be PASSIVE_LEVEL", KeGetCurrentIrql() < DISPATCH_LEVEL);

            //
            // Controller failed to clear halt in the first 5 ms of aggressive polling by the driver.
            // Implement a secondary wait period with less frequent polling using high resolution
            // timers and relinquish the CPU while waiting for the timer event.
            //
            timer = ExAllocateTimer(Controller_HighResTimerCompletion,
                                    &event,
                                    EX_TIMER_HIGH_RESOLUTION);

            if (timer == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            KeInitializeEvent(&event, SynchronizationEvent, FALSE);
            dueTime = WDF_REL_TIMEOUT_IN_MS(CONTROLLER_SECONDARY_POLLING_FREQUENCY_IN_MS);
            for (i = 0; i < CONTROLLER_SECONDARY_POLLING_COUNT_LIMIT; i++) {

                ExSetTimer(timer, dueTime, 0, NULL);
                KeWaitForSingleObject(&event,
                                      Executive,
                                      KernelMode,
                                      FALSE,
                                      NULL);

                usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);
                if (usbStatus.HcHalted != TRUE) {
                    currentTime = KeQueryUnbiasedInterruptTime();
                    break;
                }
            }
        }

        if (usbStatus.HcHalted == TRUE) {
            currentTime = KeQueryUnbiasedInterruptTime();
            LogError(ControllerData->RecorderLog, Controller, 
                "HW_COMPLIANCE: %I64u milliseconds after software set the RunStop bit, the xHC has still not cleared the HcHalted bit!",
                ((currentTime - beginTime)/10000ULL));
            NT_ASSERTMSG("HcHalted is still set after setting RunStopBit", 0);
        }
        else {
            LogInfo(ControllerData->RecorderLog, Controller, 
                "HcHalted was cleared %I64u microseconds after the RunStop bit was set.",
                ((currentTime - beginTime)/10ULL));
        }

        //
        // Start the WDF timer.
        //
        WdfTimerStart(ControllerData->WdfTimer,
                      WDF_REL_TIMEOUT_IN_MS(CONTROLLER_WATCHDOG_TIMER_INTERVAL_MS));

        status = STATUS_SUCCESS;

    } FINALLY {

        if (timer != NULL) {
            ExDeleteTimer(timer, FALSE, FALSE, NULL);
        }
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_D0EntryPostInterruptsEnabled(
    __in
        PCONTROLLER_DATA        ControllerData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    This function is called when the controller enters D0.

--*/
{
    NTSTATUS                status;

    TRY {

        LogInfo(ControllerData->RecorderLog, Controller,
                "ControllerData 0x%p WdfPreviousState %d", ControllerData, WdfPreviousState);

        status = Controller_Start(ControllerData);

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_D0Exit(
    __in
        PCONTROLLER_DATA        ControllerData,
    __in
        WDF_POWER_DEVICE_STATE  WdfTargetState
    )
/*++

Routine Description:

    This function is called when the controller exits D0.

--*/
{
    NTSTATUS    status;

    TRY {

        //
        // Stop the WDF timer.
        //
        WdfTimerStop(ControllerData->WdfTimer, TRUE);

        LogVerbose(ControllerData->RecorderLog, Controller,
                   "ControllerData 0x%p %!WdfPowerDeviceState!",
                   ControllerData, WdfTargetState);

        status = Register_ControllerStop(ControllerData->RegisterHandle);

        if (!NT_SUCCESS(status)) {
            LogError(ControllerData->RecorderLog, Controller, "Controller failed to stop, Register_ControllerStop returned %!STATUS!", status);
        }

    } FINALLY {

    }

    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_D0ExitSaveState(
    __in
        PCONTROLLER_DATA        ControllerData
    )
/*++

Routine Description:

    This function is called when the controller exits D0 after the
    controller has been halted.

--*/
{
    NTSTATUS                status;
    ULONG                   count;
    LARGE_INTEGER           time;
    POPERATIONAL_REGISTERS  operationalRegisters;
    USBCMD                  usbCommand;
    USBSTS                  usbStatus;
    ULONGLONG               beginTime;
    ULONGLONG               currentTime;

    TRY {

        LogInfo(ControllerData->RecorderLog, Controller,
                   "Begin: Controller Save State operation");

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_SAVE_STATE_START(
            NULL,
            ControllerData->UcxController);

        operationalRegisters = Register_GetOperationalRegisters(ControllerData->RegisterHandle);

        usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.HcHalted == FALSE) {
            LogError(ControllerData->RecorderLog, Controller, 
                "HW_COMPLIANCE: Cannot save state because controller is not halted! Returning STATUS_ADAPTER_HARDWARE_ERROR");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        if ((usbStatus.SaveStateStatus == TRUE)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Cannot save state because Controller Save State operation is currently in progress, returning STATUS_ADAPTER_HARDWARE_ERROR");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }
        if (usbStatus.RestoreStateStatus == TRUE) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Cannot save state because Controller Restore State operation is currently in progress, returning STATUS_ADAPTER_HARDWARE_ERROR");
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }
        

        //
        // Save the controller state.
        //
        LogInfo(ControllerData->RecorderLog, Controller,
                   "Begin: Controller Save State register write");
        beginTime = KeQueryUnbiasedInterruptTime();

        usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
        usbCommand.ControllerSaveState = TRUE;
        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

        count = DEFAULT_STATE_SAVE_WAIT_COUNT;

        do {

            usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

            if (usbStatus.SaveRestoreError == TRUE) {
                
                currentTime = KeQueryUnbiasedInterruptTime();
                LogError(ControllerData->RecorderLog, Controller,
                    "HW_COMPLIANCE: Controller Save State failed after %I64u milliseconds because usbStatus.SaveRestoreError is 1!",
                    ((currentTime - beginTime)/10000ULL));

                //
                // Clear the Save/Restore Error
                //
                usbStatus.AsUlong32 = 0;
                usbStatus.SaveRestoreError = TRUE;
                Register_WriteUlong(&operationalRegisters->UsbStatus.AsUlong32, usbStatus.AsUlong32);

                status = STATUS_ADAPTER_HARDWARE_ERROR;
                break;
            }

            if (usbStatus.SaveStateStatus == FALSE) {
                currentTime = KeQueryUnbiasedInterruptTime();
                LogInfo(ControllerData->RecorderLog, Controller, "Controller Save State successful after %I64u microseconds", 
                    ((currentTime - beginTime)/10ULL));
                status = STATUS_SUCCESS;
                break;
            }

            if (count == 0) {
                currentTime = KeQueryUnbiasedInterruptTime();
                LogError(ControllerData->RecorderLog, Controller,
                    "HW_COMPLIANCE: Controller Save State failed to complete after %I64u milliseconds. Returning STATUS_ADAPTER_HARDWARE_ERROR.",
                    ((currentTime - beginTime)/10000ULL));
                NT_ASSERTMSG("Controller Save State failed to complete?", 0);
                status = STATUS_ADAPTER_HARDWARE_ERROR;
                break;
            }

            //
            // Delay for 200 microseconds before re-reading the register.
            //
            time.QuadPart = WDF_REL_TIMEOUT_IN_US(DEFAULT_STATE_SAVE_WAIT_TIME);

            KeDelayExecutionThread(KernelMode, FALSE, &time);

            count--;

        } WHILE (TRUE);

    } FINALLY {

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_SAVE_STATE_COMPLETE(
            NULL,
            ControllerData->UcxController,
            status);
    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulatePciDeviceInformation(
    __in
        RECORDER_LOG            RecorderLog,
    __in
        ULONG                   PciBusNumber,
    __in
        USHORT                  PciDeviceNumber,
    __in
        USHORT                  PciFunctionNumber,
    __in
        PBUS_INTERFACE_STANDARD PciBusInterfaceStandard,
    __in
        PPCI_COMMON_CONFIG      PciCommonConfig,
    __out
        PPCI_DEVICE_INFORMATION PciDeviceInformation,
    __inout
        PULONGLONG              FirmwareVersion
    )
{
    PAGED_CODE();

    TRY {

        LogInfo(RecorderLog, Controller,
                "PCI Bus.Device.Function: %d.%d.%d",
                PciBusNumber, PciDeviceNumber, PciFunctionNumber);

        LogInfo(RecorderLog, Controller,
                "PCI: VendorId 0x%04x DeviceId 0x%04x RevisionId 0x%02x",
                PciCommonConfig->VendorID,
                PciCommonConfig->DeviceID,
                PciCommonConfig->RevisionID);










        PciDeviceInformation->BusNumber      = PciBusNumber;
        PciDeviceInformation->DeviceNumber   = PciDeviceNumber;
        PciDeviceInformation->FunctionNumber = PciFunctionNumber;

        PciDeviceInformation->VendorId       = PciCommonConfig->VendorID;
        PciDeviceInformation->DeviceId       = PciCommonConfig->DeviceID;
        PciDeviceInformation->RevisionId     = PciCommonConfig->RevisionID;
        PciDeviceInformation->SubVendorID    = PciCommonConfig->u.type0.SubVendorID;
        PciDeviceInformation->SubSystemID    = PciCommonConfig->u.type0.SubSystemID;

        //
        // Cache the context and set bus data bus interface. This is used
        // to disable the controller after non-pnp error recovery failure,
        // at which point we don't want to query the for the bus interfaces.
        //
        PciDeviceInformation->Context        = PciBusInterfaceStandard->Context;
        PciDeviceInformation->SetBusData     = PciBusInterfaceStandard->SetBusData;

    } FINALLY {
    }
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_RetrievePciData(
    __in
        WDFDEVICE               WdfDevice,
    __in
        RECORDER_LOG            RecorderLog,
    __out
        PPCI_DEVICE_INFORMATION PciDeviceInformation,
    __inout
        PULONGLONG              FirmwareVersion
    )
{
    ULONG                   address;
    ULONG                   resultLength;
    ULONG                   bytesRead;
    NTSTATUS                status;
    PCI_COMMON_CONFIG       pciCommonConfig;
    ULONG                   pciBusNumber;
    USHORT                  pciDeviceNumber;
    USHORT                  pciFunctionNumber;
    BUS_INTERFACE_STANDARD  pciBusInterfaceStandard;

    PAGED_CODE();

    TRY {

        status = WdfDeviceQueryProperty(WdfDevice,
                                        DevicePropertyBusNumber,
                                        sizeof(pciBusNumber),
                                        (PVOID)&pciBusNumber,
                                        &resultLength);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller,
                     "WdfDeviceQueryProperty(DevicePropertyBusNumber) Failed %!STATUS!", status);
        }

        //
        // Query the PCI Address, which is the Device Number and
        // Function Number.
        //
        status = WdfDeviceQueryProperty(WdfDevice,
                                        DevicePropertyAddress,
                                        sizeof(address),
                                        (PVOID)&address,
                                        &resultLength);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller,
                     "WdfDeviceQueryProperty(DevicePropertyAddress) Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // For PCI, the DevicePropertyAddress has the device number in
        // the high word and the function number in the low word.
        //
        pciDeviceNumber = (USHORT)(((address) >> 16) & 0x0000FFFF);
        pciFunctionNumber = (USHORT)((address) & 0x0000FFFF);

        //
        // Retrieve the bus interface used to read & write to PCI config space.
        //
        status = WdfFdoQueryForInterface(WdfDevice,
                                         &GUID_BUS_INTERFACE_STANDARD,
                                         (PINTERFACE)&pciBusInterfaceStandard,
                                         sizeof(BUS_INTERFACE_STANDARD),
                                         1,
                                         NULL);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller,
                     "WdfFdoQueryForInterface Failed %!STATUS!", status);
            LEAVE;
        }

        RtlZeroMemory(&pciCommonConfig, sizeof(pciCommonConfig));

        bytesRead = pciBusInterfaceStandard.GetBusData(pciBusInterfaceStandard.Context,
                                                       PCI_WHICHSPACE_CONFIG,
                                                       &pciCommonConfig,
                                                       0,
                                                       sizeof(pciCommonConfig));

        if (bytesRead != sizeof(pciCommonConfig)) {

             status = STATUS_UNSUCCESSFUL;
             LogError(RecorderLog, Controller,
                      "Failed to read pciCommonConfig: BytesRead %d", bytesRead);
             LEAVE;
        }

        Controller_PopulatePciDeviceInformation(RecorderLog,
                                                pciBusNumber,
                                                pciDeviceNumber,
                                                pciFunctionNumber,
                                                &pciBusInterfaceStandard,
                                                &pciCommonConfig,
                                                PciDeviceInformation,
                                                FirmwareVersion
                                                );

    } FINALLY {
    }

    return status;
}


_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_RetrieveUrsData(
    __in
        WDFDEVICE                       WdfDevice,
    __in
        RECORDER_LOG                    RecorderLog,
    __out
        UCX_CONTROLLER_PARENT_BUS_TYPE* ParentBusType,
    __out
        PACPI_DEVICE_INFORMATION        AcpiDeviceInformation,
    __out
        PPCI_DEVICE_INFORMATION         PciDeviceInformation,
    __inout
        PULONGLONG                      FirmwareVersion
    )
/*++

Routine Description:

    For URS (USB Role Switch) enumerate dual-role controllers, fetch details of the underlying bus.
    Either AcpiDeviceInformation or PciDeviceInformation is populated and the ParentBusType is set
    accordingly.

--*/
{
    NTSTATUS                status;
    WDFIOTARGET             ursIoTarget;
    WDF_MEMORY_DESCRIPTOR   outputDescriptor;
    PURS_DEVICE_INFORMATION ursDeviceInformation;
    ULONG                   bufferSize;

    PAGED_CODE();

    TRY {

        bufferSize = sizeof(*ursDeviceInformation);
        ursDeviceInformation = ExAllocatePoolWithTag(NonPagedPool,
                                                     bufferSize,
                                                     DriverAllocationTag);

        if (ursDeviceInformation == NULL) {
            LogError(RecorderLog, Controller, "Failed to allocate %Iu bytes", bufferSize);
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(ursDeviceInformation, bufferSize);
        ursIoTarget = WdfDeviceGetIoTarget(WdfDevice);
        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor,
                                          ursDeviceInformation,
                                          bufferSize);

        status = WdfIoTargetSendInternalIoctlSynchronously(
                     ursIoTarget,
                     NULL,
                     IOCTL_INTERNAL_URS_GET_DEVICE_INFORMATION,
                     NULL,
                     &outputDescriptor,
                     NULL,
                     NULL);

        if (status == STATUS_BUFFER_OVERFLOW) {

            bufferSize = (ULONG) ursDeviceInformation->Size;
            ExFreePoolWithTag(ursDeviceInformation, DriverAllocationTag);
            ursDeviceInformation = ExAllocatePoolWithTag(NonPagedPool,
                                                         bufferSize,
                                                         DriverAllocationTag);

            if (ursDeviceInformation == NULL) {
                LogError(RecorderLog, Controller, "Failed to allocate %Iu bytes", bufferSize);
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            RtlZeroMemory(ursDeviceInformation, bufferSize);
            WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor,
                                              ursDeviceInformation,
                                              bufferSize);

            status = WdfIoTargetSendInternalIoctlSynchronously(
                         ursIoTarget,
                         NULL,
                         IOCTL_INTERNAL_URS_GET_DEVICE_INFORMATION,
                         NULL,
                         &outputDescriptor,
                         NULL,
                         NULL);
        }

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller, "IOCTL_INTERNAL_URS_GET_DEVICE_INFORMATION failed %!STATUS!", status);
            LEAVE;
        }

        switch (ursDeviceInformation->BusType) {

        case UrsBusTypeAcpi:
            *ParentBusType = UcxControllerParentBusTypeAcpi;

            Controller_PopulateAcpiDeviceInformation(
                &(ursDeviceInformation->AcpiDeviceInformation.AcpiDeviceInformationOutput),
                AcpiDeviceInformation);

            RtlZeroMemory(PciDeviceInformation, sizeof(*PciDeviceInformation));

            break;

        default:
            NT_ASSERT(ursDeviceInformation->BusType == UrsBusTypePci);
            *ParentBusType = UcxControllerParentBusTypePci;

            Controller_PopulatePciDeviceInformation(
                RecorderLog,
                ursDeviceInformation->PciDeviceInformation.BusNumber,
                ursDeviceInformation->PciDeviceInformation.DeviceNumber,
                ursDeviceInformation->PciDeviceInformation.FunctionNumber,
                &(ursDeviceInformation->PciDeviceInformation.BusInterfaceStandard),
                &(ursDeviceInformation->PciDeviceInformation.CommonConfig),
                PciDeviceInformation,
                FirmwareVersion);

            RtlZeroMemory(AcpiDeviceInformation, sizeof(*AcpiDeviceInformation));

            break;
        }

    } FINALLY {

        if (ursDeviceInformation != NULL) {
            ExFreePoolWithTag(ursDeviceInformation, DriverAllocationTag);
        }
    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateAcpiDeviceInformation(
    __in
        PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER AcpiInfo,
    __out
        PACPI_DEVICE_INFORMATION               AcpiDeviceInformation
    )
{
    ULONG vendorIdStringLength;
    ULONG deviceIdStringLength;

    TRY {

        //
        // Extract the device identification strings. VendorId and DeviceId are
        // stuffed in a single string that starts at VendorIdStringOffset and is
        // of length VendorStringLength. DeviceIdStringOffset points to somewhere
        // within this string.
        //

        RtlStringCchPrintfA(AcpiDeviceInformation->RevisionId,
                            MAX_REVISION_ID_STRING_LENGTH,
                            "%04X",
                            AcpiInfo->HardwareRevision);

        if (AcpiInfo->VendorIdStringOffset != 0) {

            if (AcpiInfo->DeviceIdStringOffset != 0) {
                vendorIdStringLength = AcpiInfo->DeviceIdStringOffset - AcpiInfo->VendorIdStringOffset;
            } else {
                vendorIdStringLength = AcpiInfo->VendorStringLength;
            }

            NT_ASSERT(vendorIdStringLength != 0);
            NT_ASSERT(vendorIdStringLength < MAX_VENDOR_ID_STRING_LENGTH);

            RtlCopyMemory(AcpiDeviceInformation->VendorId,
                          (PUCHAR)AcpiInfo + AcpiInfo->VendorIdStringOffset,
                          vendorIdStringLength);

            if (AcpiInfo->DeviceIdStringOffset != 0) {

                deviceIdStringLength = AcpiInfo->VendorStringLength - vendorIdStringLength;

                NT_ASSERT(deviceIdStringLength != 0);
                NT_ASSERT(deviceIdStringLength < MAX_DEVICE_ID_STRING_LENGTH);

                RtlCopyMemory(AcpiDeviceInformation->DeviceId,
                              (PUCHAR)AcpiInfo + AcpiInfo->DeviceIdStringOffset,
                              deviceIdStringLength);

            } else {

                RtlStringCchPrintfA(AcpiDeviceInformation->DeviceId,
                                    MAX_DEVICE_ID_STRING_LENGTH,
                                    "%s",
                                    ACPI_DEVICE_ID_UNKNOWN);
            }

        } else {

            RtlStringCchPrintfA(AcpiDeviceInformation->VendorId,
                                MAX_VENDOR_ID_STRING_LENGTH,
                                "%s",
                                ACPI_VENDOR_ID_UNKNOWN);

            RtlStringCchPrintfA(AcpiDeviceInformation->DeviceId,
                                MAX_DEVICE_ID_STRING_LENGTH,
                                "%s",
                                ACPI_DEVICE_ID_UNKNOWN);
        }

    } FINALLY {
    }
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_RetrieveAcpiData(
    __in
        WDFDEVICE                   WdfDevice,
    __in
        RECORDER_LOG                RecorderLog,
    __inout
        PACPI_DEVICE_INFORMATION    AcpiDeviceInformation
    )
{
    NTSTATUS                                status;
    WDFIOTARGET                             acpiIoTarget;
    PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER  acpiInfo;
    ACPI_DEVICE_INFORMATION_OUTPUT_BUFFER   acpiInfoBuffer;
    WDF_MEMORY_DESCRIPTOR                   outputDescriptor;
    USHORT                                  requiredBufferSize;

    PAGED_CODE();

    TRY {

        RtlZeroMemory(&acpiInfoBuffer, sizeof(ACPI_DEVICE_INFORMATION_OUTPUT_BUFFER));

        acpiIoTarget = WdfDeviceGetIoTarget(WdfDevice);

        //
        // Send the first IOCTL to figure out the required size of the output buffer.
        //
        acpiInfo = &acpiInfoBuffer;

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor,
                                          (PVOID)acpiInfo,
                                          sizeof(ACPI_DEVICE_INFORMATION_OUTPUT_BUFFER));

        status = WdfIoTargetSendIoctlSynchronously(acpiIoTarget,
                                                   NULL,
                                                   IOCTL_ACPI_GET_DEVICE_INFORMATION,
                                                   NULL,
                                                   &outputDescriptor,
                                                   NULL,
                                                   NULL);

        if (status != STATUS_BUFFER_OVERFLOW) {

            LogError(RecorderLog, Controller,
                     "IOCTL_ACPI_GET_DEVICE_INFORMATION failed %!STATUS!", status);
            LEAVE;
        }

        requiredBufferSize = acpiInfoBuffer.Size;

        acpiInfo = ExAllocatePoolWithTag(NonPagedPool,
                                         requiredBufferSize,
                                         DriverAllocationTag);

        if (acpiInfo == NULL) {

            LogError(RecorderLog, Controller,
                     "Failed to allocate %u bytes", requiredBufferSize);
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(acpiInfo, requiredBufferSize);

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor,
                                          (PVOID)acpiInfo,
                                          requiredBufferSize);

        status = WdfIoTargetSendIoctlSynchronously(acpiIoTarget,
                                                   NULL,
                                                   IOCTL_ACPI_GET_DEVICE_INFORMATION,
                                                   NULL,
                                                   &outputDescriptor,
                                                   NULL,
                                                   NULL);

        if (!NT_SUCCESS(status)) {

            LogError(RecorderLog, Controller,
                     "IOCTL_ACPI_GET_DEVICE_INFORMATION failed %!STATUS!", status);
            LEAVE;
        }

        Controller_PopulateAcpiDeviceInformation(acpiInfo, AcpiDeviceInformation);

    } FINALLY {

        if ((acpiInfo != NULL) && (acpiInfo != &acpiInfoBuffer)) {
            ExFreePoolWithTag(acpiInfo, DriverAllocationTag);
        }
    }

    return status;
}

BOOLEAN
__drv_requiresIRQL(PASSIVE_LEVEL)
Controller_DriverInstalledDueToCompatibleIdMatch(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    This function determines if the xHCI driver was installed due to a Compatible
    Id match, as opposed to other more specific device Ids.

--*/
{
    ULONG       i;
    NTSTATUS    status;
    BOOLEAN     compatibleIdMatch;
    DEVPROPTYPE devPropType;
    ULONG       requiredSize;

    DECLARE_UNICODE_STRING_SIZE(matchingDeviceId, MATCHING_ID_SIZE);

    PAGED_CODE();

    TRY {

        compatibleIdMatch = FALSE;

        RtlZeroMemory(matchingDeviceId.Buffer, matchingDeviceId.MaximumLength);

        //
        // The matching ID should never be larger than the matching ID size, at least
        // in the case where a match occurred on the class code (CC_0C0330).
        //
        status = IoGetDevicePropertyData(WdfDeviceWdmGetPhysicalDevice(ControllerData->WdfDevice),
                                         &DEVPKEY_Device_MatchingDeviceId,
                                         LOCALE_NEUTRAL,
                                         0,
                                         matchingDeviceId.MaximumLength - sizeof(UNICODE_NULL),
                                         matchingDeviceId.Buffer,
                                         &requiredSize,
                                         &devPropType);

        if (!NT_SUCCESS(status)) {

            LogWarning(ControllerData->RecorderLog, Controller,
                       "IoGetDevicePropertyData Failed %!STATUS!", status);
            LEAVE;
        }

        NT_ASSERT(devPropType == DEVPROP_TYPE_STRING);

        __analysis_assume_nullterminated(matchingDeviceId.Buffer);

        for (i = 0; i < RTL_NUMBER_OF(XhciCompatibleIdList); i += 1) {

            if (!_wcsicmp(matchingDeviceId.Buffer, XhciCompatibleIdList[i])) {
                compatibleIdMatch = TRUE;
                break;
            }
        }

    } FINALLY {

    }

    return compatibleIdMatch;
}

VOID
__drv_requiresIRQL(PASSIVE_LEVEL)
Controller_SetDeviceDescription(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description:

    This function sets the device description (friendly name) property for the xHCI
    controller to a friendlier name with the following format:

       @System32\\drivers\\usbxhci.sys,#<MsgId>;%1 USB %2 eXtensible Host Controller - %3 (Microsoft);(<vidString>,<UsbMajorVersion.UsbMinorVersion>,<MajorVersion.MinorVersion>)

    If VendorID string is unavailable, then "Standard" is used in the description.

--*/
{
    NTSTATUS                status;
    ULONG                   vidIndex;
    PPCI_VID_TABLE_ENTRY    pciVidEntry;
    PACPI_VID_TABLE_ENTRY   acpiVidEntry;
    PWCHAR                  vidString;
    PMESSAGE_RESOURCE_ENTRY messageString;
    size_t                  messageStringSize;
    PWCHAR                  messageStringW;
    size_t                  messageStringWSize;
    PMESSAGE_RESOURCE_ENTRY stdVendorID;
    size_t                  stdVendorIDSize;
    PWCHAR                  stdVendorIDDisplay;
    size_t                  stdVendorIDDisplaySize;
    PWCHAR                  uniDeviceDesc;
    size_t                  uniDeviceDescSize;
    USB_VERSION             usbVersion;
    WCHAR                   driverPath[] = L"@System32\\drivers\\usbxhci.sys";

    PAGED_CODE();

    TRY {
        vidString = NULL;
        uniDeviceDesc = NULL;
        stdVendorIDDisplay = NULL;
        messageStringW = NULL;

        //
        // Only set the device description if the driver installed due to a Compatible
        // Id match. If the driver installed due to match on a different device Id
        // (third party INF), then we provide a generic device description, and let
        // the INF override it.
        //
        if (Controller_DriverInstalledDueToCompatibleIdMatch(ControllerData) == FALSE) {
            LEAVE;
        }

        usbVersion = RootHub_GetHighestUsbVersionSupported(ControllerData->RootHubHandle);

        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {

            for (vidIndex = 0; vidIndex < ARRAYSIZE(PciVendorIdTable); vidIndex++) {

                pciVidEntry = &PciVendorIdTable[vidIndex];

                if (pciVidEntry->VendorId == Controller_GetPciVendorID(ControllerData)) {
                    vidString = pciVidEntry->VendorString;
                    break;
                }
            }

        } else {

            for (vidIndex = 0; vidIndex < ARRAYSIZE(AcpiVendorIdTable); vidIndex++) {

                acpiVidEntry = &AcpiVendorIdTable[vidIndex];

                if (_stricmp(acpiVidEntry->VendorIdShort, ControllerData->AcpiDeviceInfo.VendorId) == 0) {
                    vidString = acpiVidEntry->VendorIdLong;
                    break;
                }
            }
        }

        //
        // Extract the complete device description string from localized message table
        //
#pragma prefast(disable:28175, "Disable prefast warning about accessing the driver object's DriverStart member")
        status = RtlFindMessage(g_UsbXhciDriverObject->DriverStart,
                                11, //RT_MESSAGETABLE
                                LANG_NEUTRAL,
                                USBXHCIENUM_DEVICE_DESC,
                                &messageString);
        if (!NT_SUCCESS(status)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Unable to extract device description from message table");
            LEAVE;
        }

        messageStringSize = messageString->Length - FIELD_OFFSET(MESSAGE_RESOURCE_ENTRY, Text);
        if (messageStringSize > (NTSTRSAFE_MAX_CCH * sizeof(WCHAR))) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Device description resource string too long");
            LEAVE;
        }

        //
        // Get the length of the extracted message string
        //
        status = RtlUnalignedStringCbLengthW((PWSTR)messageString->Text,
                                             messageStringSize,
                                             &messageStringSize);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // String returned by RtlFindMessage has CR/LF appended to it by default.
        // These needs to be removed from the friendly name provided to
        // IoSetDevicePropertyData - WinSE:427001
        //
        messageStringW = (PWCHAR)messageString->Text;
        if (messageStringSize >= (2 * sizeof(WCHAR)) &&
                messageStringW[(messageStringSize/sizeof(WCHAR)) - 1] == L'\n' &&
                messageStringW[(messageStringSize/sizeof(WCHAR)) - 2] == L'\r') {
            messageStringWSize = messageStringSize - sizeof(WCHAR);
            messageStringW  = ExAllocatePoolWithTag(PagedPool,
                                                    messageStringWSize,
                                                    DriverAllocationTag);
            if (messageStringW == NULL) {
                LogError(ControllerData->RecorderLog, Controller,
                        "Failed to allocate memory for messageStringW");
                LEAVE;
            }
            RtlZeroMemory(messageStringW, messageStringWSize);
            wcsncpy_s(messageStringW,
                      messageStringWSize / sizeof(WCHAR),
                      (PWCHAR)messageString->Text,
                      (messageStringSize / sizeof(WCHAR)) - 2);
        }

        //
        // Compute the size of the indirect string we will build with following format:
        //      @[path\]<modname>,#<MsgId>;<MessageString>;(arg1,arg2)
        // to generate this:
        //      @System32\\drivers\\usbxhci.sys,#<MsgId>;%1 USB %2 eXtensible Host Controller - %3 (Microsoft);(<vidString>,<UsbMajorVersion.UsbMinorVersion>,<MajorVersion.MinorVersion>)
        //
        uniDeviceDescSize = sizeof(driverPath) - sizeof(WCHAR) +    // "@System32\drivers\usbxhci.sys"
                            13 * sizeof(WCHAR) +                    // 3 formatting chars and 10 digit MsgId in ,#<MsgId>;
                            (messageStringSize - (2 * sizeof(WCHAR))) + // <MessageString> size
                            5 * sizeof(WCHAR) +                     // Formatting chars in ;(<arg1>,<arg2>,<arg3>)
                            5 * sizeof(WCHAR) +                     // <arg2> having format "%X.%X"
                            5 * sizeof(WCHAR) +                     // <arg3> having format "%X.%X"
                            sizeof(WCHAR);                          // Terminating NULL

        if (vidString) {
            uniDeviceDescSize += (wcslen(vidString) * sizeof(WCHAR));    // <arg1> in formatted string
            uniDeviceDesc = ExAllocatePoolWithTag(PagedPool,
                                                  uniDeviceDescSize,
                                                  DriverAllocationTag);
            if (uniDeviceDesc == NULL) {
                LogError(ControllerData->RecorderLog, Controller,
                         "Failed to allocate memory for uniDeviceDesc");
                LEAVE;
            }
            status = RtlStringCbPrintfW(uniDeviceDesc,
                                        uniDeviceDescSize,
                                        L"%s,#%d;%s;(%s,%X.%X,%X.%X)",
                                        driverPath,
                                        USBXHCIENUM_DEVICE_DESC,
                                        messageStringW,
                                        vidString,
                                        usbVersion.MajorVersion,
                                        usbVersion.MinorVersion,
                                        ControllerData->MajorVersion,
                                        ControllerData->MinorVersion);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

        } else {

            LogInfo(ControllerData->RecorderLog, Controller, "Unknown Vendor Id encountered");

            //
            // Extract the "Standard" VendorID string from localized message table
            //
            status = RtlFindMessage(g_UsbXhciDriverObject->DriverStart,
                                    11, //RT_MESSAGETABLE
                                    LANG_NEUTRAL,
                                    USBXHCIENUM_DEVICE_DESC_STD,
                                    &stdVendorID);
            if (!NT_SUCCESS(status)) {
                NT_ASSERTMSG("Unable to extract device description from message table", 0);
                LEAVE;
            }

            stdVendorIDSize = stdVendorID->Length - FIELD_OFFSET(MESSAGE_RESOURCE_ENTRY, Text);
            if (stdVendorIDSize > (NTSTRSAFE_MAX_CCH * sizeof(WCHAR))) {
                LogError(ControllerData->RecorderLog, Controller,
                         "Device description resource string too long");
                LEAVE;
            }

            status = RtlUnalignedStringCbLengthW((PWSTR)stdVendorID->Text,
                                                 stdVendorIDSize,
                                                 &stdVendorIDSize);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            //
            // String returned by RtlFindMessage has CR/LF appended to it by default.
            // These needs to be removed from the standard stdVendorID
            //
            uniDeviceDescSize += (stdVendorIDSize - (2 * sizeof(WCHAR)));     // <arg1> in formatted string
            uniDeviceDesc = ExAllocatePoolWithTag(PagedPool,
                                                  uniDeviceDescSize,
                                                  DriverAllocationTag);
            if (uniDeviceDesc == NULL) {
                LogError(ControllerData->RecorderLog, Controller,
                         "Failed to allocate memory for uniDeviceDesc");
                LEAVE;
            }

            stdVendorIDDisplay = (PWCHAR)stdVendorID->Text;
            if (stdVendorIDSize >= (2 * sizeof(WCHAR)) &&
                    stdVendorIDDisplay[(stdVendorIDSize/sizeof(WCHAR)) - 1] == L'\n' &&
                    stdVendorIDDisplay[(stdVendorIDSize/sizeof(WCHAR)) - 2] == L'\r') {
                stdVendorIDDisplaySize = stdVendorIDSize - sizeof(WCHAR);
                stdVendorIDDisplay = ExAllocatePoolWithTag(PagedPool,
                                                           stdVendorIDDisplaySize,
                                                           DriverAllocationTag);
                if (stdVendorIDDisplay == NULL) {
                    LogError(ControllerData->RecorderLog, Controller,
                             "Insufficient Memory");
                    LEAVE;
                }

                RtlZeroMemory(stdVendorIDDisplay, stdVendorIDDisplaySize);

                wcsncpy_s(stdVendorIDDisplay,
                          stdVendorIDDisplaySize / sizeof(WCHAR),
                          (PWCHAR)stdVendorID->Text,
                          (stdVendorIDSize / sizeof(WCHAR)) - 2);
            }

            status = RtlStringCbPrintfW(uniDeviceDesc,
                                        uniDeviceDescSize,
                                        L"%s,#%d;%s;(%s,%X.%X,%X.%X)",
                                        driverPath,
                                        USBXHCIENUM_DEVICE_DESC,
                                        messageStringW,
                                        stdVendorIDDisplay,
                                        usbVersion.MajorVersion,
                                        usbVersion.MinorVersion,
                                        ControllerData->MajorVersion,
                                        ControllerData->MinorVersion);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        //
        // Calculate the actual length of the formatted Device Description string
        // because IoSetDevicePropertyData expects an exact length.
        //
        status = RtlUnalignedStringCbLengthW(uniDeviceDesc,
                                             uniDeviceDescSize,
                                             &uniDeviceDescSize);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = IoSetDevicePropertyData(WdfDeviceWdmGetPhysicalDevice(ControllerData->WdfDevice),
                                         &DEVPKEY_Device_FriendlyName,
                                         LOCALE_NEUTRAL,
                                         PLUGPLAY_PROPERTY_PERSISTENT,
                                         DEVPROP_TYPE_STRING_INDIRECT,
                                         (ULONG)(uniDeviceDescSize + sizeof(L'\0')),
                                         uniDeviceDesc);
        if (!NT_SUCCESS(status)) {
            LogWarning(ControllerData->RecorderLog, Controller,
                       "IoSetDevicePropertyData Failed %!STATUS!", status);
            LEAVE;
        }

    } FINALLY {
        if (uniDeviceDesc) {
            ExFreePoolWithTag(uniDeviceDesc, DriverAllocationTag);
        }
        if (stdVendorIDDisplay) {
            ExFreePoolWithTag(stdVendorIDDisplay, DriverAllocationTag);
        }
        if (messageStringW) {
            ExFreePoolWithTag(messageStringW, DriverAllocationTag);
        }
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateDeviceFlagsFromKse(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description:

    Queries the KSE component for errata flags to be applied to devices with matching
    VEN/DEV/Rev or VEN/DEV.

Returns:

    VOID

--*/
{
    NTSTATUS    status;
    BOOLEAN     deviceFlagsFound;
    ULONG64     deviceFlagsVenDev;
    ULONG64     deviceFlagsVenDevRev;
    ULONG64     deviceFlagsVenDevRevFirmware;
    ULONG64     deviceFlagsVenDevSubsys;
    ULONG64     deviceFlagsVenDevSubsysRev;
    WCHAR       kseHardwareId[KSE_HARDWARE_ID_BUFFER_SIZE];

    deviceFlagsFound = FALSE;

    TRY {

        if (g_KseQueryDeviceFlags == NULL) {
            LEAVE;
        }

        //
        // See if there are any Device Flags set for the VEN/DEV without Revision.
        //
        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {
            RtlStringCchPrintfW(kseHardwareId,
                                KSE_HARDWARE_ID_BUFFER_SIZE,
                                L"USBXHCI:PCI\\VEN_%04X&DEV_%04X",
                                Controller_GetPciVendorID(ControllerData),
                                Controller_GetPciDeviceID(ControllerData));
        } else {
            RtlStringCchPrintfW(kseHardwareId,
                                KSE_HARDWARE_ID_BUFFER_SIZE,
                                L"USBXHCI:ACPI\\VEN_%S&DEV_%S",
                                Controller_GetAcpiVendorID(ControllerData),
                                Controller_GetAcpiDeviceID(ControllerData));
        }

        status = g_KseQueryDeviceFlags(kseHardwareId, L"USBXHCI", &deviceFlagsVenDev);

        if (NT_SUCCESS(status)) {

            LogInfo(ControllerData->RecorderLog, Controller,
                    "Found DeviceFlags 0x%I64x for %ws", deviceFlagsVenDev, kseHardwareId);

            ControllerData->DeviceFlags.AsULong64 |= deviceFlagsVenDev;
            deviceFlagsFound = TRUE;
        }

        //
        // See if there are any Device Flags set for this device's VEN/DEV/Rev.
        //
        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {
            RtlStringCchPrintfW(kseHardwareId,
                                KSE_HARDWARE_ID_BUFFER_SIZE,
                                L"USBXHCI:PCI\\VEN_%04X&DEV_%04X&REV_%02X",
                                Controller_GetPciVendorID(ControllerData),
                                Controller_GetPciDeviceID(ControllerData),
                                Controller_GetPciRevisionID(ControllerData));
        } else {
            RtlStringCchPrintfW(kseHardwareId,
                                KSE_HARDWARE_ID_BUFFER_SIZE,
                                L"USBXHCI:ACPI\\VEN_%S&DEV_%S&REV_%S",
                                Controller_GetAcpiVendorID(ControllerData),
                                Controller_GetAcpiDeviceID(ControllerData),
                                Controller_GetAcpiRevisionID(ControllerData));
        }

        status = g_KseQueryDeviceFlags(kseHardwareId, L"USBXHCI", &deviceFlagsVenDevRev);

        if (NT_SUCCESS(status)) {

            LogInfo(ControllerData->RecorderLog, Controller,
                    "Found DeviceFlags 0x%I64x for %ws", deviceFlagsVenDevRev, kseHardwareId);

            ControllerData->DeviceFlags.AsULong64 |= deviceFlagsVenDevRev;
            deviceFlagsFound = TRUE;
        }

        //
        // See if there are any Device Flags set for this device's VEN/DEV/Rev/FW.
        //
        if (Controller_GetFirmwareVersion(ControllerData) != ULONGLONG_MAX) {

            if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {
                RtlStringCchPrintfW(kseHardwareId,
                                    KSE_HARDWARE_ID_BUFFER_SIZE,
                                    L"USBXHCI:PCI\\VEN_%04X&DEV_%04X&REV_%02X&%I64X",
                                    Controller_GetPciVendorID(ControllerData),
                                    Controller_GetPciDeviceID(ControllerData),
                                    Controller_GetPciRevisionID(ControllerData),
                                    ControllerData->FirmwareVersion);
            } else {
                RtlStringCchPrintfW(kseHardwareId,
                                    KSE_HARDWARE_ID_BUFFER_SIZE,
                                    L"USBXHCI:ACPI\\VEN_%S&DEV_%S&REV_%S&%I64X",
                                    Controller_GetAcpiVendorID(ControllerData),
                                    Controller_GetAcpiDeviceID(ControllerData),
                                    Controller_GetAcpiRevisionID(ControllerData),
                                    ControllerData->FirmwareVersion);
            }

            status = g_KseQueryDeviceFlags(kseHardwareId, L"USBXHCI", &deviceFlagsVenDevRevFirmware);

            if (NT_SUCCESS(status)) {

                LogInfo(ControllerData->RecorderLog, Controller,
                        "Found DeviceFlags 0x%I64x for %ws",
                        deviceFlagsVenDevRevFirmware, kseHardwareId);

                ControllerData->DeviceFlags.AsULong64 |= deviceFlagsVenDevRevFirmware;
                deviceFlagsFound = TRUE;
            }
        }

        //
        // See if there are any Device Flags set for this device's VEN/DEV/SUBSYS.
        //
        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {
            RtlStringCchPrintfW(kseHardwareId,
                                KSE_HARDWARE_ID_BUFFER_SIZE,
                                L"USBXHCI:PCI\\VEN_%04X&DEV_%04X&SUBSYS_%04X%04X",
                                Controller_GetPciVendorID(ControllerData),
                                Controller_GetPciDeviceID(ControllerData),
                                Controller_GetPciSubSystemID(ControllerData),
                                Controller_GetPciSubVendorID(ControllerData));

            status = g_KseQueryDeviceFlags(kseHardwareId, L"USBXHCI", &deviceFlagsVenDevSubsys);

            if (NT_SUCCESS(status)) {

                LogInfo(ControllerData->RecorderLog, Controller,
                        "Found DeviceFlags 0x%I64x for %ws", deviceFlagsVenDevSubsys, kseHardwareId);

                ControllerData->DeviceFlags.AsULong64 |= deviceFlagsVenDevSubsys;
                deviceFlagsFound = TRUE;
            }
        }

        //
        // See if there are any Device Flags set for this device's VEN/DEV/SUBSYS/Rev.
        //
        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {
            RtlStringCchPrintfW(kseHardwareId,
                                KSE_HARDWARE_ID_BUFFER_SIZE,
                                L"USBXHCI:PCI\\VEN_%04X&DEV_%04X&SUBSYS_%04X%04X&REV_%02X",
                                Controller_GetPciVendorID(ControllerData),
                                Controller_GetPciDeviceID(ControllerData),
                                Controller_GetPciSubSystemID(ControllerData),
                                Controller_GetPciSubVendorID(ControllerData),
                                Controller_GetPciRevisionID(ControllerData));

            status = g_KseQueryDeviceFlags(kseHardwareId, L"USBXHCI", &deviceFlagsVenDevSubsysRev);

            if (NT_SUCCESS(status)) {

                LogInfo(ControllerData->RecorderLog, Controller,
                        "Found DeviceFlags 0x%I64x for %ws", deviceFlagsVenDevSubsysRev, kseHardwareId);

                ControllerData->DeviceFlags.AsULong64 |= deviceFlagsVenDevSubsysRev;
                deviceFlagsFound = TRUE;
            }
        }

        //
        // There are two cases for which a live dump should be generated right away:
        //  [1] Firmware is outdated, but we can continue functioning.
        //  [2] Controller has known issues due to which it is not supported.
        //
        // This live dump is supposed to generate an Action Center report.
        //
        if (ControllerData->DeviceFlags.ControllerNotSupported) {

            Controller_ReportFatalError(ControllerData,
                                        Controller_FatalErrorNoRestart,
                                        USB3_WER_BUGCODE_USBXHCI_FIRMWARE_NOT_SUPPORTED,
                                        NULL,
                                        NULL,
                                        NULL);

        } else if (ControllerData->DeviceFlags.ControllerFirmwareOutdated) {

            LogError(ControllerData->RecorderLog, Controller,
                "HW_COMPLIANCE: Controller Firmware has known bugs, and newer firmware is available");

            Controller_HwVerifierBreakIfEnabled(
                ControllerData,
                NULL,
                NULL,
                ControllerHwVerifierFirmwareVersionOutdated,
                "Controller Firmware has known issues, and newer firmware is available",
                NULL,
                NULL);

            Controller_ReportFatalError(ControllerData,
                                        Controller_FatalErrorNoAction,
                                        USB3_WER_BUGCODE_USBXHCI_FIRMWARE_OUTDATED,
                                        NULL,
                                        NULL,
                                        NULL);
        }

    } FINALLY {
        if(deviceFlagsFound == FALSE) {
            LogWarning(ControllerData->RecorderLog, Controller,
                "No Kse Device Flags were found for %ws", kseHardwareId);
        }
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateDeviceFlagsFromRegistry(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               RegistryStoreType
    )
/*++

Routine Description:

    This function retrieves the device flags from the registry.

--*/
{
    NTSTATUS        status;
    WDFKEY          wdfKey;
    UNICODE_STRING  valueName;
    ULONG           value;

    PAGED_CODE();

    TRY {

        if (RegistryStoreType == RegistryStoreTypeDriver) {

            status = WdfDriverOpenParametersRegistryKey(
                        WdfDeviceGetDriver(ControllerData->WdfDevice),
                        KEY_READ,
                        WDF_NO_OBJECT_ATTRIBUTES,
                        &wdfKey);

        } else {

            status = WdfDeviceOpenRegistryKey(ControllerData->WdfDevice,
                                              PLUGPLAY_REGKEY_DEVICE,
                                              KEY_READ,
                                              WDF_NO_OBJECT_ATTRIBUTES,
                                              &wdfKey);
        }

        if (!NT_SUCCESS(status)) {

            LogWarning(ControllerData->RecorderLog, Controller,
                       "WdfDeviceOpenRegistryKey Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // UseStrictBiosHandoff.
        //
        RtlInitUnicodeString(&valueName, DEVICE_REGISTRY_KEY_USE_STRICT_BIOS_HANDOFF);

        status = WdfRegistryQueryULong(wdfKey, &valueName, &value);

        if (NT_SUCCESS(status)) {
            if (value == 0) {
                ControllerData->DeviceFlags.UseStrictBIOSHandoff = 0;
            } else if (value == 1) {
                ControllerData->DeviceFlags.UseStrictBIOSHandoff = 1;
            }
        }

    } FINALLY {

        if (wdfKey != NULL) {
            WdfRegistryClose(wdfKey);
        }

        LogVerbose(ControllerData->RecorderLog, Controller,
                   "DeviceFlags 0x%I64x after reading from registry store %d",
                   ControllerData->DeviceFlags.AsULong64, RegistryStoreType);
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateInternalDeviceFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description:

    This routine will set non-generic device flags hardcoded for specific
    controllers.

--*/
{
    PAGED_CODE();

    TRY {






























    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateDeviceFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description:

    This routine initializes the device flags that are used for the xHCI
    controller. The device flags are set using the following order:

        1. Initially set the default device flags to zero.

        2. Set device flags as retrieved from the Kse database.

        3. Set device flags based on registry key values in the driver store.

        4. Set device flags based on registry key values in the device store.

--*/
{
    PAGED_CODE();

    TRY {

        ControllerData->DeviceFlags.AsULong64 = 0;

        ControllerData->DeviceFlags.IgnoreBIOSHandoffFailure = 1;

        //
        // Retrieve device flags from the errata manager.
        //
        Controller_PopulateDeviceFlagsFromKse(ControllerData);

        //
        // Retrieve device flags within the driver store.
        //
        Controller_PopulateDeviceFlagsFromRegistry(ControllerData, RegistryStoreTypeDriver);

        //
        // Retrieve device flags within the device store.
        //
        Controller_PopulateDeviceFlagsFromRegistry(ControllerData, RegistryStoreTypeDevice);

        LogInfo(ControllerData->RecorderLog, Controller, "DeviceFlags 0x%I64x",
                ControllerData->DeviceFlags.AsULong64);

        //
        // Set the non-generic device flags hardcoded in our code.
        //
        Controller_PopulateInternalDeviceFlags(ControllerData);

    } FINALLY {

    }

    return;
}

VOID
Controller_WdfEvtTimerFunction(
    WDFTIMER    WdfTimer
    )
/*++

Routine Description:

    This function is a periodic timer that is called at a specified interval.

--*/
{

    PCONTROLLER_DATA        controllerData;
    POPERATIONAL_REGISTERS  operationalRegisters;
    USBSTS                  usbStatus;

    TRY {

        controllerData = GetControllerData(WdfTimerGetParentObject(WdfTimer));
        operationalRegisters = Register_GetOperationalRegisters(controllerData->RegisterHandle);

        usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.AsUlong32 == HARDWARE_GONE_ULONG32_REGISTER_VALUE) {

            LogWarning(controllerData->RecorderLog, Controller, "Controller_WdfEvtTimerFunction: controller gone detected");
            Controller_SetControllerGone(controllerData, TRUE);
            LEAVE;
        }

        //
        // A host system error is fatal system error with no expectation that
        // software can recover the controller/system.
        //
        if (usbStatus.HostSystemError) {

            LogFatal(controllerData->RecorderLog, Controller,
                     "HostSystemError detected, non-recoverable system error");

            Controller_HwVerifierBreakIfEnabled(
                controllerData,
                NULL,
                NULL,
                ControllerHwVerifierHostSystemError,
                "Controller reported Host System Error",
                NULL,
                NULL);

            Controller_ReportFatalError(controllerData,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_HOST_SYSTEM_ERROR,
                                        NULL,
                                        NULL,
                                        NULL);
            LEAVE;
        }

        //
        // A host controller error is a recoverable and requires system software
        // to reset and the xHCI controller.
        //
        if (usbStatus.HostControllerError) {

            LogFatal(controllerData->RecorderLog, Controller,
                     "HostControllerError detected, attempting controller recovery");

            Controller_HwVerifierBreakIfEnabled(
                controllerData,
                NULL,
                NULL,
                ControllerHwVerifierHostControllerError,
                "Controller reported Host Controller Error",
                NULL,
                NULL);

            Controller_ReportFatalError(controllerData,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_HOST_CONTROLLER_ERROR,
                                        NULL,
                                        NULL,
                                        NULL);
            LEAVE;
        }

        #if DBG

        //
        // This flag can be set in the debugger to test Live Dump creation and testing
        //
        if (g_TriggerLiveDump) {

            g_TriggerLiveDump = FALSE;

            LogFatal(controllerData->RecorderLog, Controller,
                     "Forcing live dump creation because g_ForceLiveDump is set");

            Controller_ReportFatalError(controllerData,
                                        g_TriggerRecoveryAction,
                                        USB3_WER_BUGCODE_USBXHCI_HOST_SYSTEM_ERROR,
                                        NULL,
                                        NULL,
                                        NULL);
            LEAVE;
        }

        //
        // Simulate controller surprise removal by disabling the controller in PCI.
        //
        if (g_SimulateSurpriseRemoval) {

            g_SimulateSurpriseRemoval = FALSE;

            LogFatal(controllerData->RecorderLog, Controller,
                     "Simulating controller surprise removal by disabling the controller in PCI");

            if (Controller_GetParentBusType(controllerData) == UcxControllerParentBusTypePci) {

                USHORT commandData;

                commandData = 0;

                controllerData->PciDeviceInfo.SetBusData(controllerData->PciDeviceInfo.Context,
                                                         PCI_WHICHSPACE_CONFIG,
                                                         &commandData,
                                                         PCI_COMMAND_REGISTER,
                                                         sizeof(commandData));
            }
        }

        #endif

        //
        // Controller reset count is maintained per interval. Clear the reset count
        // once the interval is finished.
        //
        controllerData->ResetInterval += 1;

        if (controllerData->ResetInterval == CONTROLLER_RESET_RECOVERY_INTERVAL_TIMER) {
            controllerData->ResetInterval = 0;
            controllerData->ResetCount = 0;

            LogInfo(controllerData->RecorderLog, Controller,
                     "Controller ResetCount and ResetInterval reset to 0.");
        }

        //
        // Check to see if common buffer resources need to be rebalanced.
        //
        CommonBuffer_RebalanceResources(controllerData->CommonBufferHandle);

    } FINALLY {

    }

    return;
}

FORCEINLINE
VOID
Controller_SetFrameNumberData(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               FrameNumber,
    __in
        ULONGLONG           FrameNumberTime_ms
    )
/*++

Routine Description:

    This routine stores the information about the FrameNumber and FrameNumberTime_ms
    in an atomic fashion.

    Since we use a 64-bit interlocked operation, the amount of data we can store in
    an atomic fashion is limited.

    Thus we only store the upper 21 bits of the FrameNumber (HighPart), and adjust
    the FrameNumberTime_ms acordingly too.

--*/
{
    FRAME_SNAPSHOT snapShot;
    snapShot.HighPart = FrameNumber >> MFINDEX_FRAME_SHIFT;
    snapShot.TimeWhenLowPartWasZero_ms = FrameNumberTime_ms - (FrameNumber & FRAME_INDEX_MASK);

    InterlockedExchange64(&ControllerData->FrameNumberData.Snapshot.Asll, snapShot.Asll);
}

FORCEINLINE
VOID
Controller_GetFrameNumberData(
    __in
        PCONTROLLER_DATA    ControllerData,
    __out
        ULONG              *FrameNumber,
    __out
        ULONGLONG          *FrameNumberTime_ms
    )
/*++

Routine Description:

    This routine retrieves the information last stored about the FrameNumber
    and FrameNumberTime_ms in an atomic fashion.

--*/
{
    FRAME_SNAPSHOT snapshot;

    //
    // There is not InterlockedRead64, thus have to use an InterlockedOr64 operation here.
    //
    snapshot.Asll = InterlockedOr64(&ControllerData->FrameNumberData.Snapshot.Asll, 0);
    *FrameNumber = ((ULONG)snapshot.HighPart) << MFINDEX_FRAME_SHIFT;
    *FrameNumberTime_ms = snapshot.TimeWhenLowPartWasZero_ms;
}

FORCEINLINE
ULONGLONG
Controller_GetRelativeTime_ms(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description:

    This routine returns the number of miliseconds that have passed since we booted.

    Please note that this would not account for the time we are suspended, however
    that should not cause any issues, since even on the 2.0 stack when we are suspended
    interrupts are turned off and the
    frame number does not increment.

--*/
{
    LARGE_INTEGER tickCount;
    ULONGLONG     time_ms;

    KeQueryTickCount(&tickCount);
    time_ms = (tickCount.QuadPart * ControllerData->FrameNumberData.TimeIncrementPerTick) / 10000;

    return time_ms;
}

FORCEINLINE
VOID
Controller_InitFrameNumber(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description:

    This routine initializes the FRAME_NUMBER_DATA structure in ControllerData that is
    use to generate a 32 bit FrameNumber from a 11 bit hardware counter.

--*/
{
    ULONG         frameNumber;
    ULONGLONG     initTime_ms;

    TRY {

        if (ControllerData->FrameNumberData.Initialized == FALSE) {

            ControllerData->FrameNumberData.TimeIncrementPerTick = KeQueryTimeIncrement();

            frameNumber = Register_Get11BitFrameNumber(ControllerData->RegisterHandle,
                                                       MICROFRAME_INCREMENT_DEFAULT);

            initTime_ms = Controller_GetRelativeTime_ms(ControllerData);

            //
            // Store the Initial TimeStamp when the frame number was 0.
            // The time is relative to FrameNumberData.InitTime_ms
            //
            Controller_SetFrameNumberData(ControllerData, frameNumber, initTime_ms);

            ControllerData->FrameNumberData.Initialized = TRUE;
        }

    } FINALLY {

    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
Controller_GetFrameNumber(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               MicroframeIncrement
    )
/*++

Routine Description:

    This function generates a 32  bit frame number.

    XHCI hardware has a 11 bit hardware counter, that we need to extrapolate to form a
    32 bit counter.

Arguments:

    MicroframeIncrement - The value to be added to the Microframe number before generating a
    frame number from it. This value is just passed into the Register_Get11BitFrameNumber call.

--*/
{
    ULONG   currRegisterBasedFrameNumber11Bits;
    ULONG   prevFrameNumber;
    ULONG   currTimeBasedFrameNumber;
    ULONG   currTimeBasedFrameNumber11Bits;
    ULONG   frameNumber32Bit;
    ULONGLONG prevTime_ms;
    ULONGLONG currTime_ms;
    ULONGLONG deltaTime_ms;

    currTime_ms = Controller_GetRelativeTime_ms(ControllerData);
    currRegisterBasedFrameNumber11Bits = Register_Get11BitFrameNumber(ControllerData->RegisterHandle,
                                                                      MicroframeIncrement);

    //
    // Get the information about the Previously generated Frame number and the time
    // at which it was generated.
    //
    Controller_GetFrameNumberData(ControllerData, &prevFrameNumber, &prevTime_ms);

    deltaTime_ms = currTime_ms - prevTime_ms;

    currTimeBasedFrameNumber = prevFrameNumber + (ULONG)deltaTime_ms;
    currTimeBasedFrameNumber11Bits = currTimeBasedFrameNumber & FRAME_INDEX_MASK;

    //
    // The actual value of the 32 bit frame number we are going to return is going to be based off
    // the Hardware Register.
    // So we are going to use bits 31:11 from the time based one, and bits 10:0 from what was
    // read from the registers.
    // However if the 11 bit hardware values & from the time based one are close to the
    // COUNTER_11BIT_WRAP_VALUE boundary, it is possible that one of them has wrapped around and
    // other other one has not. We need to account for that.
    //
    // If the Controller_GetFrameNumber is being called frequently then the
    // we would be able to correctly account for any drift between Time based and hardware based
    // Frame number.
    // However if there are huge gaps between Controller_GetFrameNumber calls, it is possible that
    // the two frame nubmers may drift far apart that we over-calculate or under-calculate
    // the frame number. However in that case such a deviation should not cause any issues in the
    // client driver.
    //

    frameNumber32Bit = ((currTimeBasedFrameNumber & ~FRAME_INDEX_MASK) |
                        currRegisterBasedFrameNumber11Bits);

    if (currTimeBasedFrameNumber11Bits < currRegisterBasedFrameNumber11Bits) {
        if (currRegisterBasedFrameNumber11Bits - currTimeBasedFrameNumber11Bits  > (COUNTER_11BIT_WRAP_VALUE / 2) ) {
            //
            // Looks like the Time Based counter has wrapped around, however the Register based one
            // has not.
            //
            frameNumber32Bit -= COUNTER_11BIT_WRAP_VALUE;
        }
    } else {
        if (currTimeBasedFrameNumber11Bits - currRegisterBasedFrameNumber11Bits  > (COUNTER_11BIT_WRAP_VALUE / 2) ) {
            //
            // Looks like the Register Based counter has wrapped around, however the Time based one
            // has not.
            //
            frameNumber32Bit += COUNTER_11BIT_WRAP_VALUE;
        }

    }

    //
    // Store the information about the generated 32 bit Frame number, to improve the
    // acuracy of future Controller_GetFrameNumber calls.
    // Since the Controller_SetFrameNumberData operation is atomic, we are fine even if
    // multiple threads are simultaneously call Controller_SetFrameNumberData
    //
    Controller_SetFrameNumberData(ControllerData, frameNumber32Bit, currTime_ms);

    LogVerbose(ControllerData->RecorderLog, Controller, "FrameNumber 0x%08x", frameNumber32Bit);

    return frameNumber32Bit;
}

NTSTATUS
Controller_UcxEvtGetCurrentFrameNumber(
    UCXCONTROLLER   UcxController,
    PULONG          FrameNumber
    )
/*++

Routine Description:

    This function returns the current 32-bit frame number.

--*/
{
    PCONTROLLER_DATA    controllerData;

    controllerData = GetControllerData(UcxController);

    if (controllerData->CurrentWdfPowerDeviceState != WdfPowerDeviceD0) {

        //
        // Do not access the host controller registers if the controller
        // is not in D0.
        //
        *FrameNumber = 0xFFFFFFFF;

    } else {

        //
        // Return a 32-bit frame number. This is done by retrieving the value from the
        // micro frame index, shifting 3 bits to remove the microframe bits, leaving
        // the frame value. And then retrieving the internally tracked upper bits
        // of the frame number.
        //
        *FrameNumber = Controller_GetFrameNumber(controllerData, MICROFRAME_INCREMENT_DEFAULT);
    }

    //EventWriteUSBXHCI_ETW_EVENT_GET_CURRENT_FRAME_NUMBER(NULL, UcxController, *FrameNumber);

    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_DisableController(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description:

    This function attempts to disable the controller at the parent bus. For PCI,
    this is done by clearing the command register in the PCI config space.

--*/
{
    USHORT  commandData;

    TRY {

        //
        // Mark the controller as unavailable.
        //
        Controller_SetControllerGone(ControllerData, FALSE);

        //
        // Disable the PCI device.
        //
        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {

            //
            // To disable the PCI device, the driver clears the PCI command
            // register, located at in the PCI config space at offset 0x4.
            // This has a high likelihood of stopping the controller, at
            // which point it should be safe to complete any pending requests
            // and gracefully fail by invalidating the device state. This
            // will cause the controller to load code 43.
            //
            commandData = 0;

            ControllerData->PciDeviceInfo.SetBusData(
                ControllerData->PciDeviceInfo.Context,
                PCI_WHICHSPACE_CONFIG,
                &commandData,
                PCI_COMMAND_REGISTER,
                sizeof(commandData));

        } else {

            //
            // What now? Could the controller still be performing
            // busmaster DMA to system memory at this point?
            //
            LogError(ControllerData->RecorderLog, Controller, "Non-PCI bus: unable to disable controller");
            NT_ASSERTMSG("Non-PCI bus: unable to disable controller", 0);
        }

        //
        // Make sure all outstanding events on the various event rings are
        // processed and there are no more DPCs running, befrore returning.
        //
        KeFlushQueuedDpcs();

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_InternalReset(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description:

    This routine is internal host controller reset routine which is
    called by the UCX non-PnP host controller reset event callback
    handler.

    This routine is also called internally if the host controller needs
    to be reset outside the context of the UCX non-PnP host controller
    reset event callback handler, for example if entry into D0 has not
    been completed yet.  (Or is this really if START_DEVICE has not been
    completed yet?)

    It attempts to stop the host controller, and then reset,
    re-initialize, and then restart the host controller.

--*/
{
    NTSTATUS    stopStatus, resetStatus, startStatus, finalStatus;

    TRY {

        resetStatus = STATUS_SUCCESS;

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_INTERNAL_RESET_START(
            NULL,
            ControllerData->UcxController);

        LogInfo(ControllerData->RecorderLog, Controller, "Begin: xHC Internal Reset");

        if (InterlockedIncrement(&ControllerData->ResetLock) != 1) {
#if DBG
            NT_ASSERTMSG("Race condition in xHCI controller reset", 0);
#else
            if (KdRefreshDebuggerNotPresent() == FALSE) {

                DbgBreakPoint();
            }
#endif
        }

        ControllerData->ResetCount++;

        ControllerData->Counter.ResetCount += 1;

        //
        // Stop the WDF timer.
        //
        WdfTimerStop(ControllerData->WdfTimer, TRUE);

        //
        // Notify the host controller command handler that a controller
        // reset is in progress.  This will block writing new commands
        // to the command ring buffer and writing to the command ring
        // doorbell register.
        //
        Command_ControllerResetPreReset(ControllerData->CommandHandle);

        DeviceSlot_ControllerResetPreReset(ControllerData->DeviceSlotHandle);

        //
        // Ensure that any outstanding DPCs have completed before attempting
        // to stop the controller. This is a good enough way to ensure that
        // our DPCs are not actively ringing the doorbell while we attempt to
        // stop the controller. It is still possible that after the KeFlushQueueDpcs
        // return, and before we call Register_ControllerStop more interrupts are
        // generated and more DPCs get queued. The guaranteed way would be to
        // synchronize with the ISR to ensure that we stop queuing DPCs. However,
        // for Win8 it is too late to implement such a solution. In addition this
        // is controller reset recovery code, and the more likely case would be
        // that the controller is stuck and not generating any new interrupts.
        //
        KeFlushQueuedDpcs();

        //
        // Stop the controller if it is running and if it is possible to
        // stop it.  If the controller is running and cannot be stopped
        // then the subsequent controller reset will proceed anyway.
        //
        stopStatus = Register_ControllerStop(ControllerData->RegisterHandle);

        //
        // Regardless of whether the controller actually stopped, unconditionally
        // do another flush of outstanding DPCs. This is to account for the edge
        // case where the controller might have generated an interrupt after the
        // previous KeFlushQueuedDpcs call. Once again, this is not fool proof if
        // the controller decides to fail the Stop operation and is generating new
        // interrupts.
        //
        // At this point, we don't want DPCs running for two reasons:
        // 1. Do not ring the doorbell while attempting a controller reset.
        // 2. Ensure DPCs have finished executing before deprogramming devices in
        //    DeviceSlot_ControllerResetPostReset().
        //
        KeFlushQueuedDpcs();

        if (!NT_SUCCESS(stopStatus)) {

            LogError(ControllerData->RecorderLog, Controller, "Call to Register_ControllerStop failed with status %!STATUS!", stopStatus);
            Controller_DisableController(ControllerData);

        } else {

            //
            // Reset the controller.  A failure here is a fatal condition,
            // although cleanup will still continue but the controller will
            // not be restarted.
            //
            resetStatus = Register_ControllerReset(ControllerData->RegisterHandle, TRUE);

            if (!NT_SUCCESS(resetStatus)) {
                LogError(ControllerData->RecorderLog, Controller, "Call to Register_ControllerReset failed with status %!STATUS!", resetStatus);
            }
        }

        //
        // Complete all pending commands and re-initialize the command
        // ring.  Do this before marking all devices as disabled so
        // that any pending transfers blocked on command completion
        // should be cleaned up before disabling devices and their
        // associated endpont transfer rings.
        //
        Command_ControllerResetPostReset(ControllerData->CommandHandle);

        //
        // Ensure any CommonBuffer callbacks have been completed.
        //
        CommonBuffer_ControllerResetPostReset(ControllerData->CommonBufferHandle);

        //
        // Mark all devices as reset disabled and re-initialize the
        // device slots and scratchpad buffers.
        //
        DeviceSlot_ControllerResetPostReset(ControllerData->DeviceSlotHandle);

        //
        // Let the root hub component re-initialize its state
        //
        RootHub_ControllerResetPostReset(ControllerData->RootHubHandle);

        //
        // If the controller stop failed, then quit after cleaning up the
        // command and device slots.
        //
        if (!NT_SUCCESS(stopStatus)) {

            LogError(ControllerData->RecorderLog, Controller, 
                "HW_COMPLIANCE: During controller reset recovery, controller stop timed out!  %!STATUS!",
                stopStatus);   

            Controller_HwVerifierBreakIfEnabled(
                ControllerData,
                NULL,
                NULL,
                ControllerHwVerifierResetRecoveryStopTimeout,
                "During controller reset recovery, controller stop timed out",
                NULL,
                NULL);

            Controller_ReportFatalError(ControllerData,
                                        Controller_FatalErrorNoRestart,
                                        USB3_WER_BUGCODE_USBXHCI_RESET_RECOVERY_STOP_FAILED,
                                        NULL,
                                        NULL,
                                        NULL);
            finalStatus = stopStatus;
            LEAVE;
        }

        //
        // Re-initialize the interrrupt event rings.
        //
        Interrupter_ControllerResetPostReset(ControllerData->InterrupterHandle);

        //
        // If the controller reset failed then quit now after cleaning
        // up and before restarting the controller.
        //
        if (!NT_SUCCESS(resetStatus)) {

            LogError(ControllerData->RecorderLog, Controller, 
                "HW_COMPLIANCE: During controller reset recovery, controller reset timed out!  %!STATUS!",
                resetStatus);

            Controller_HwVerifierBreakIfEnabled(
                ControllerData,
                NULL,
                NULL,
                ControllerHwVerifierResetRecoveryResetTimeout,
                "During controller reset recovery, controller reset timed out",
                NULL,
                NULL);

            Controller_ReportFatalError(ControllerData,
                                        Controller_FatalErrorNoRestart,
                                        USB3_WER_BUGCODE_USBXHCI_RESET_RECOVERY_RESET_FAILED,
                                        NULL,
                                        NULL,
                                        NULL);

            finalStatus = resetStatus;
            LEAVE;
        }

        //
        // Restart the controller.  A failure here is a fatal condition.
        //
        startStatus = Controller_Start(ControllerData);

        if (!NT_SUCCESS(startStatus)) {

            LogError(ControllerData->RecorderLog, Controller, 
                "HW_COMPLIANCE: During controller reset recovery, controller start timed out!  %!STATUS!", startStatus);

            Controller_HwVerifierBreakIfEnabled(
                ControllerData,
                NULL,
                NULL,
                ControllerHwVerifierResetRecoveryStartTimeout,
                "During controller reset recovery, controller start timed out",
                NULL,
                NULL);

            Controller_ReportFatalError(ControllerData,
                                        Controller_FatalErrorNoRestart,
                                        USB3_WER_BUGCODE_USBXHCI_RESET_RECOVERY_RESTART_FAILED,
                                        NULL,
                                        NULL,
                                        NULL);
            finalStatus = startStatus;
            LEAVE;
        }

        finalStatus = STATUS_SUCCESS;

    } FINALLY {

        if (NT_SUCCESS(finalStatus)) {

            //
            // Allow commands to be queued on the controller again.
            //
            Command_ControllerResetPostResetSuccess(ControllerData->CommandHandle);

            LogInfo(ControllerData->RecorderLog, Controller, "xHC Internal Reset succeeded");

        } else {

            //
            // Immediately fail all commands.
            //
            Command_ControllerResetPostResetFailure(ControllerData->CommandHandle);

            UcxControllerSetFailed(ControllerData->UcxController);

            LogError(ControllerData->RecorderLog, Controller, "xHC Internal Reset failed with finalStatus %!STATUS!", finalStatus);
        }

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_INTERNAL_RESET_COMPLETE(
            NULL,
            ControllerData->UcxController,
            finalStatus);

        NT_VERIFY(InterlockedDecrement(&ControllerData->ResetLock) >= 0);
    }

    return;
}

VOID
Controller_UcxEvtReset(
    UCXCONTROLLER   UcxController
    )
/*++

Routine Description:

    This routine is the non-PnP host controller reset event callback
    handler.

    It attempts to stop the host controller, and then reset,
    re-initialize, and then restart the host controller.

    N.B: We should not receive another one of this callbacks if a previous
    reset failed, because in that case, controller will have been disabled,
    and UCX will have been informed.

--*/
{
    UCX_CONTROLLER_RESET_COMPLETE_INFO  ucxControllerResetCompleteInfo;
    PCONTROLLER_DATA                    controllerData;

    TRY {

        controllerData = GetControllerData(UcxController);

        if (Controller_IsControllerAccessible(controllerData)) {

            Controller_InternalReset(controllerData);

        } else {

            LogError(controllerData->RecorderLog, Controller,
                       "Controller is not accessible");
        }

        //
        // Inform UCX that the controller reset is complete.
        //
        UCX_CONTROLLER_RESET_COMPLETE_INFO_INIT(&ucxControllerResetCompleteInfo,
                                                UcxControllerStateLost,
                                                TRUE);

        UcxControllerResetComplete(UcxController, &ucxControllerResetCompleteInfo);

    } FINALLY {

    }

    return;
}

NTSTATUS
Controller_UcxEvtEnableForwardProgress(
    UCXCONTROLLER   UcxController
    )
/*++

Routine Description:

    This routine is the Forward Progress enable callback handler. It is
    called by Ucx for a controller that is hosting a device for which
    the client driver has enabled forward progress.

    It simply saves this information, to be queried by other objects later.

--*/
{
    PCONTROLLER_DATA controllerData;

    PAGED_CODE();

    TRY {

        controllerData = GetControllerData(UcxController);

        controllerData->ForwardProgressEnabled = TRUE;

    } FINALLY {

    }

    return STATUS_SUCCESS;
}

NTSTATUS
Controller_UcxEvtQueryUsbCapability(
    UCXCONTROLLER   UcxController,
    PGUID           CapabilityType,
    ULONG           OutputBufferLength,
    PVOID           OutputBuffer,
    PULONG          ResultLength
    )
/*++

Routine Description:

--*/
{
    NTSTATUS            status;
    PCONTROLLER_DATA    controllerData;
    ULONG               numStreams;

    TRY {

        controllerData = GetControllerData(UcxController);

        numStreams = 0;
        *ResultLength = 0;

        if (RtlCompareMemory(CapabilityType,
                             &GUID_USB_CAPABILITY_CHAINED_MDLS,
                             sizeof(GUID)) == sizeof(GUID)) {

            if (Controller_GetDeviceFlags(controllerData).DisableChainedMDLSupport) {

                LogInfo(controllerData->RecorderLog, Controller,
                        "GUID_USB_CAPABILITY_CHAINED_MDLS Not Supported");
                status = STATUS_NOT_SUPPORTED;

            } else {

                LogInfo(controllerData->RecorderLog, Controller, "GUID_USB_CAPABILITY_CHAINED_MDLS");
                status = STATUS_SUCCESS;
            }

        } else if (RtlCompareMemory(CapabilityType,
                                    &GUID_USB_CAPABILITY_STATIC_STREAMS,
                                    sizeof(GUID)) == sizeof(GUID)) {

            LogInfo(controllerData->RecorderLog, Controller, "GUID_USB_CAPABILITY_STATIC_STREAMS");
            *ResultLength = sizeof(USHORT);

            if (OutputBufferLength >= sizeof(USHORT)) {

                if (OutputBuffer != NULL) {

                    //
                    // Number of static streams always fits within a USHORT, so we allow that buffer
                    // size, even though docs say BufferLength == sizeof(ULONG).
                    //
                    numStreams = Register_GetSupportedNumberOfStreams(controllerData->RegisterHandle);
                    NT_ASSERT(numStreams < (1 << 16));
                    *(PUSHORT)OutputBuffer = (USHORT)numStreams;
                    status = STATUS_SUCCESS;

                } else {

                    status = STATUS_INVALID_PARAMETER;
                }

            } else {

                LogInfo(controllerData->RecorderLog, Controller, "buffer too small %d",
                        OutputBufferLength);
                status = STATUS_BUFFER_TOO_SMALL;
            }

        } else if (RtlCompareMemory(CapabilityType,
                                    &GUID_USB_CAPABILITY_FUNCTION_SUSPEND,
                                    sizeof(GUID)) == sizeof(GUID)) {

            LogInfo(controllerData->RecorderLog, Controller, "GUID_USB_CAPABILITY_FUNCTION_SUSPEND");
            status = STATUS_SUCCESS;

        } else if (RtlCompareMemory(CapabilityType,
                                    &GUID_USB_CAPABILITY_SELECTIVE_SUSPEND,
                                    sizeof(GUID)) == sizeof(GUID)) {

            LogInfo(controllerData->RecorderLog, Controller, "GUID_USB_CAPABILITY_SELECTIVE_SUSPEND");
            status = STATUS_SUCCESS;

        } else if (RtlCompareMemory(CapabilityType,
                                    &GUID_USB_CAPABILITY_CLEAR_TT_BUFFER_ON_ASYNC_TRANSFER_CANCEL,
                                    sizeof(GUID)) == sizeof(GUID)) {

            if (Controller_GetDeviceFlags(controllerData).ClearTtBufferOnAsyncTransferCancel) {

                LogInfo(controllerData->RecorderLog, Controller, "GUID_USB_CAPABILITY_CLEAR_TT_BUFFER_ON_ASYNC_TRANSFER_CANCEL");
                status = STATUS_SUCCESS;

            } else {

                LogInfo(controllerData->RecorderLog, Controller,
                        "GUID_USB_CAPABILITY_CLEAR_TT_BUFFER_ON_ASYNC_TRANSFER_CANCEL Not Supported");
                status = STATUS_NOT_SUPPORTED;
            }

        } else if (RtlCompareMemory(CapabilityType,
                                    &GUID_USB_CAPABILITY_HIGH_BANDWIDTH_ISOCH,
                                    sizeof(GUID)) == sizeof(GUID)) {

            *ResultLength = sizeof(ULONG);

            if (OutputBufferLength >= sizeof(ULONG)) {

                if (OutputBuffer != NULL) {

                    if (Register_GetLargeESITPayload(controllerData->RegisterHandle)) {

                        LogInfo(controllerData->RecorderLog, Controller, "GUID_USB_CAPABILITY_HIGH_BANDWIDTH_ISOCH");
                        *(PULONG)OutputBuffer = (Register_GetExtendedTBC(controllerData->RegisterHandle)) ?
                                                MAX_ISOCH_BURST_COUNT_EXTENDED : MAX_ISOCH_BURST_COUNT;
                        status = STATUS_SUCCESS;

                    } else {

                        LogInfo(controllerData->RecorderLog, Controller,
                                "GUID_USB_CAPABILITY_HIGH_BANDWIDTH_ISOCH Not Supported");
                        status = STATUS_NOT_SUPPORTED;
                    }

                } else {

                    LogError(controllerData->RecorderLog, Controller, "buffer pointer is NULL");
                    status = STATUS_INVALID_PARAMETER;
                }
            } else {

                LogError(controllerData->RecorderLog, Controller, "buffer too small %d",
                         OutputBufferLength);
                status = STATUS_BUFFER_TOO_SMALL;
            }

        } else {

            LogInfo(controllerData->RecorderLog, Controller, "Unhandled UcxEvtQueryUsbCapability");
            status = STATUS_NOT_IMPLEMENTED; // STATUS_NOT_SUPPORTED
        }

        //
        // Log the capability being queried and what we return.
        // Note, some capabilities are always handled at the hub layer and never reach xHCI. These
        // capabilities are logged there instead of here
        //
        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_QUERY_USB_CAPABILITY(
            NULL,
            UcxController,
            CapabilityType,
            status,
            numStreams);

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_SetControllerGone(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        BOOLEAN             NotifyPnP
    )
/*++

Routine Description:

    This routine sets the controller gone flag and notifies the system
    that the controller has been removed if the system has not already
    been notified.

    If NotfiyPnp is TRUE, it means that the controller has been actually removed
    and this routine generates a live dump report for the same.

    If NotifyPnp is FALSE, it means that we are simulating a controller remove due
    to a critical error. A live dump would have already been generated for the
    critical error, thus this routine does not generate one.

--*/
{
    Controller_FatalErrorAction fatalErrorAction;
    ULONG                       reasonCode;

    TRY {

        if (NotifyPnP && (ControllerData->ControllerGone == FALSE)) {

            LogError(ControllerData->RecorderLog, Controller, 
                "HW_COMPLIANCE: A register read returned all FFs. Controller is considered physically removed");

            Controller_HwVerifierBreakIfEnabled(
                ControllerData,
                NULL,
                NULL,
                ControllerHwVerifierRegisterReadReturnedAllFF,
                "A register read returned all FFs. Controller is considered physically removed",
                NULL,
                NULL);

        }

        ControllerData->ControllerGone = TRUE;

        if (NotifyPnP) {
            fatalErrorAction = Controller_FatalErrorControllerGoneNotifyUCXAndPNP;
            reasonCode = USB3_WER_BUGCODE_USBXHCI_NO_LIVE_DUMP;
        } else {
            fatalErrorAction = Controller_FatalErrorControllerGoneNotifyUCX;
            reasonCode = USB3_WER_BUGCODE_USBXHCI_CONTROLLER_GONE;
        }

        Controller_ReportFatalError(ControllerData,
                                    fatalErrorAction,
                                    reasonCode,
                                    NULL,
                                    NULL,
                                    NULL);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
CRASHDUMP_PORT_TYPE
Controller_GetRootPortType(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               PortNumber
    )
/*++

Routine Description:

    This routine gets the port type of a port on the root hub

--*/
{
    return RootHub_GetPortType(ControllerData->RootHubHandle, PortNumber);
}

NTSTATUS
Controller_EvtIoInternalDeviceControl(
    WDFDEVICE   WdfDevice,
    PIRP        Irp
    )
/*++

Routine Description:

    This routine processes internal USB IOCTLs. Currently, it only honors
    IOCTL_INTERNAL_USB_RESET_CONTROLLER_ASYNC. It assumes that TestMode is
    ON when it gets called for this IOCTL.

--*/
{
    PCONTROLLER_DATA    controllerData;
    ULONG               ioControlCode;
    PIO_STACK_LOCATION  ioStack;
    NTSTATUS            status;

    TRY {

        ioStack        = IoGetCurrentIrpStackLocation(Irp);
        ioControlCode  = ioStack->Parameters.DeviceIoControl.IoControlCode;
        controllerData = GetDeviceData(WdfDevice)->ControllerData;

        NT_ASSERT(g_WdfDriverUsbXhciContext->TestModeOn);

        if (ioControlCode != IOCTL_INTERNAL_USB_RESET_CONTROLLER_ASYNC) {
            IoSkipCurrentIrpStackLocation(Irp);
            status = WdfDeviceWdmDispatchPreprocessedIrp(WdfDevice, Irp);
            LEAVE;
        }

        LogInfo(controllerData->RecorderLog, Controller, "TestConfig: triggering controller reset");

        Controller_ReportFatalError(controllerData,
                                    Controller_FatalErrorReset,
                                    USB3_WER_BUGCODE_USBXHCI_TEST_HOOK_IGNORE,
                                    NULL,
                                    NULL,
                                    NULL);

        status = STATUS_SUCCESS;
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    } FINALLY {

    }

    return status;
}

NTSTATUS
Controller_WdfEvtPreprocessSetPowerIrp(
        WDFDEVICE   WdfDevice,
        PIRP        Irp
    )
/*++

Routine Description:

    A driver's EvtDeviceWdmIrpPreprocess event callback function receives
    an IRP before the framework processes the IRP.

Return Value:

    Status returned is from call to WdfDeviceWdmDispatchPreprocessedIrp.

--*/
{
    NTSTATUS                    status;
    PDEVICE_DATA                deviceData;
    PIO_STACK_LOCATION          ioStack;
    PCONTROLLER_DATA            controllerData;
    SYSTEM_POWER_STATE_CONTEXT  systemPowerStateContext;

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        ioStack = IoGetCurrentIrpStackLocation(Irp);

        //
        // Translate the various Power transition information to simpler System
        // Power actions.
        //
        if (ioStack->Parameters.Power.Type == SystemPowerState) {

            systemPowerStateContext = ioStack->Parameters.Power.SystemPowerStateContext;

            switch (systemPowerStateContext.TargetSystemState) {

            case PowerSystemWorking:

                if (systemPowerStateContext.CurrentSystemState == PowerSystemHibernate) {
                    controllerData->SystemPowerAction = SystemPowerAction_ResumeFromHibernate;
                } else {
                    controllerData->SystemPowerAction = SystemPowerAction_ResumeFromSleep;
                }
                break;

            case PowerSystemSleeping1:
            case PowerSystemSleeping2:
            case PowerSystemSleeping3:

                if (systemPowerStateContext.EffectiveSystemState == PowerSystemHibernate) {
                    controllerData->SystemPowerAction = SystemPowerAction_HybridSleep;
                } else {
                    controllerData->SystemPowerAction = SystemPowerAction_Sleep;
                }
                break;

            case PowerSystemHibernate:

                controllerData->SystemPowerAction = SystemPowerAction_Hibernate;
                break;

            case PowerSystemShutdown:

                if (systemPowerStateContext.EffectiveSystemState == PowerSystemHibernate) {
                    controllerData->SystemPowerAction = SystemPowerAction_Hibernate;
                } else {
                    controllerData->SystemPowerAction = SystemPowerAction_Shutdown;
                }
                break;

            default:

                NT_ASSERTMSG("Unidentified target system state", FALSE);
                break;
            }

            LogInfo(deviceData->RecorderLog, Controller,
                    "%!SystemPowerAction!", controllerData->SystemPowerAction);


            if (Controller_GetHibernateContext(controllerData) != NULL) {

                Crashdump_ProcessSystemPowerAction(Controller_GetHibernateContext(controllerData),
                                                   controllerData->SystemPowerAction);
            }
        }

        IoSkipCurrentIrpStackLocation(Irp);

        status = WdfDeviceWdmDispatchPreprocessedIrp(WdfDevice, Irp);

        if (!NT_SUCCESS(status)) {

            LogError(deviceData->RecorderLog, Controller,
                     "WdfDeviceWdmDispatchPreprocessedIrp Failed %!STATUS!", status);
        }

    } FINALLY {

    }

    return status;
}

__drv_functionClass(EVT_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
Controller_WdfEvtDeviceFilterRemoveResourceRequirements(
    __in
        WDFDEVICE       WdfDevice,
    __in
        WDFIORESREQLIST IoResourceRequirementsList
    )
/*++

Routine Description:

    A driver's EvtDeviceFilterRemoveResourceRequirements event callback function
    can remove resources from a set of hardware resource requirements before the
    system assigns resources to a device.

Return Value:

    If EvtDeviceFilterRemoveResourceRequirements encountered no errors it must
    return STATUS_SUCCESS. Otherwise it must return an NTSTATUS value that
    NT_SUCCESS evaluates as FALSE.

--*/
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;
    NTSTATUS            status;

    PAGED_CODE();

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        LogInfo(controllerData->RecorderLog,
                Controller,"WdfDevice 0x%p WdfIoResReqList 0x%p",
                WdfDevice,
                IoResourceRequirementsList);

        status = Interrupter_FilterRemoveResourceRequirements(controllerData->InterrupterHandle,
                                                              IoResourceRequirementsList);

    } FINALLY {

    }

    return status;
}

__drv_functionClass(EVT_WDF_DEVICE_FILTER_RESOURCE_REQUIREMENTS)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
Controller_WdfEvtDeviceFilterAddResourceRequirements(
    __in
        WDFDEVICE       WdfDevice,
    __in
        WDFIORESREQLIST IoResourceRequirementsList
    )
/*++

Routine Description:

    A driver's EvtDeviceFilterAddResourceRequirements event callback function
    can add resources to a set of hardware resource requirements before the
    system assigns resources to a device.

Return Value:

    If EvtDeviceFilterAddResourceRequirements encountered no errors it must
    return STATUS_SUCCESS. Otherwise it must return an NTSTATUS value that
    NT_SUCCESS evaluates as FALSE.

--*/
{
    PDEVICE_DATA        deviceData;
    PCONTROLLER_DATA    controllerData;
    NTSTATUS            status;

    PAGED_CODE();

    TRY {

        deviceData = GetDeviceData(WdfDevice);
        controllerData = deviceData->ControllerData;

        LogInfo(controllerData->RecorderLog,
                Controller,"WdfDevice 0x%p WdfIoResReqList 0x%p",
                WdfDevice,
                IoResourceRequirementsList);

        status = Interrupter_FilterAddResourceRequirements(controllerData->InterrupterHandle,
                                                           IoResourceRequirementsList);

    } FINALLY {

    }

    return status;
}

__drv_functionClass(EVT_WDF_DEVICE_REMOVE_ADDED_RESOURCES)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
Controller_WdfEvtDeviceRemoveAddedResources(
    __in
        WDFDEVICE       WdfDevice,
    __in
        WDFCMRESLIST    ResourcesRaw,
    __in
        WDFCMRESLIST    ResourcesTranslated
    )
/*++

Routine Description:

    A driver's EvtDeviceRemoveAddedResources event callback function removes
    hardware resources that the driver's EvtDeviceFilterAddResourceRequirements
    callback function added.

    N.B: This function does nothing as we do not actually add any resources in
         the EvtDeviceFilterAddResourceRequirements callback. This callback is
         provided due to the below framework requirement:

         If a driver provides an EvtDeviceFilterAddResourceRequirements callback
         function that adds resources to a device's hardware requirements list,
         the driver must also provide an EvtDeviceRemoveAddedResources callback
         function.

Return Value:

    If EvtDeviceRemoveAddedResources encountered no errors it must return
    STATUS_SUCCESS. Otherwise it must return an NTSTATUS value that NT_SUCCESS
    evaluates as FALSE.

--*/
{
    UNREFERENCED_PARAMETER(WdfDevice);
    UNREFERENCED_PARAMETER(ResourcesRaw);
    UNREFERENCED_PARAMETER(ResourcesTranslated);

    PAGED_CODE();

    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_AddControllerToGlobalControllerList(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    KIRQL   irql;

    //
    // Add a reference on the controller when it is inserted into the
    // global controller list.
    //
    WdfObjectReferenceWithTag(ControllerData->UcxController,
                              TAG(Controller_AddControllerToGlobalControllerList));

    //
    // Insert the controller into the global controller list.
    //
    KeAcquireSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, &irql);

    InsertTailList(&g_WdfDriverUsbXhciContext->ControllerListHead,
                   &ControllerData->ControllerListEntry);

    g_WdfDriverUsbXhciContext->ControllerListCount++;

    KeReleaseSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, irql);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_RemoveControllerFromGlobalControllerList(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    KIRQL   irql;

    //
    // Remove the controller from the global controller list.
    //
    KeAcquireSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, &irql);

    RemoveEntryList(&ControllerData->ControllerListEntry);

    g_WdfDriverUsbXhciContext->ControllerListCount--;

    KeReleaseSpinLock(&g_WdfDriverUsbXhciContext->ControllerListLock, irql);

    //
    // Release the reference on the controller when it is removed from
    // the global controller list.
    //
    WdfObjectDereferenceWithTag(ControllerData->UcxController,
                                TAG(Controller_AddControllerToGlobalControllerList));
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_AddDeviceToControllerDeviceList(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        UCXUSBDEVICE        UcxUsbDevice,
    __in
        PLIST_ENTRY         DeviceListEntry
    )
{
    KIRQL   irql;

    //
    // Add a reference on the controller when a device is inserted into
    // the list of all devices that exist for this controller.
    //
    WdfObjectReferenceWithTag(ControllerData->UcxController,
                              TAG(Controller_AddDeviceToControllerDeviceList));

    //
    // Add a reference on the device when it is inserted into the list
    // of all devices that exist for this controller.
    //
    WdfObjectReferenceWithTag(UcxUsbDevice,
                              TAG(Controller_AddDeviceToControllerDeviceList));

    //
    // Insert this device into the list of all devices that exist for
    // this controller.
    //
    KeAcquireSpinLock(&ControllerData->DeviceListLock, &irql);

    InsertTailList(&ControllerData->DeviceListHead, DeviceListEntry);

    ControllerData->DeviceListCount++;

    KeReleaseSpinLock(&ControllerData->DeviceListLock, irql);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_RemoveDeviceFromControllerDeviceList(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        UCXUSBDEVICE        UcxUsbDevice,
    __in
        PLIST_ENTRY         DeviceListEntry
    )
{
    KIRQL   irql;

    //
    // Remove this device from the list of all devices that exist for
    // this controller.
    //
    KeAcquireSpinLock(&ControllerData->DeviceListLock, &irql);

    RemoveEntryList(DeviceListEntry);

    NT_ASSERT(ControllerData->DeviceListCount > 0);
    ControllerData->DeviceListCount--;

    KeReleaseSpinLock(&ControllerData->DeviceListLock, irql);

    //
    // Release the reference on the device when it is removed from the
    // list of all devices that exist for this controller.
    //
    WdfObjectDereferenceWithTag(UcxUsbDevice,
                                TAG(Controller_AddDeviceToControllerDeviceList));

    //
    // Release the reference on the controller when a device is removed
    // from the list of all devices that exist for this controller.
    //
    WdfObjectDereferenceWithTag(ControllerData->UcxController,
                                TAG(Controller_AddDeviceToControllerDeviceList));
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_TelemetryAddControllerData(
    __in
        PCONTROLLER_DATA ControllerData,
    __in
        HANDLE           TelemetryHandle
    )
/*++

Routine Description:

    This routine adds the controller data as well as data for child objects like
    Register, CommonBuffer, Interrupter, DeviceSlot, Command, RootHub, and WMI.

--*/
{
    NTSTATUS status;
    PVOID    ifrLog;
    ULONG    ifrLogSize;
    GUID     wppGuid;

    PAGED_CODE();

    TRY {

        TELEMETRY_INSERT_BLOCK(TelemetryHandle, ControllerData, sizeof(CONTROLLER_DATA));

        //
        // Store the controller's IFR log.
        //
        status = WppRecorderLogDumpLiveData(ControllerData->RecorderLog,
                                            &ifrLog,
                                            &ifrLogSize,
                                            &wppGuid);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        TELEMETRY_INSERT_BLOCK(TelemetryHandle, ifrLog, ifrLogSize);

        TELEMETRY_INSERT_BLOCK(TelemetryHandle, ControllerData->RegisterHandle, sizeof(REGISTER_DATA));

        TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                               ControllerData->CommonBufferHandle,
                               sizeof(COMMON_BUFFER_DATA));

        TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                               ControllerData->InterrupterHandle,
                               sizeof(PRIMARY_INTERRUPTER_DATA));


        TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                               ControllerData->DeviceSlotHandle,
                               sizeof(DEVICESLOT_DATA));

        //
        // Add device slot handle array
        //

        if (ControllerData->DeviceSlotHandle != NULL) {

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->DeviceSlotHandle->UsbDeviceHandleArray,
                                   (ControllerData->DeviceSlotHandle->NumberOfDeviceSlots + 1) * sizeof(HANDLE));
        }

        TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                               ControllerData->CommandHandle,
                               sizeof(COMMAND_DATA));

        TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                               ControllerData->RootHubHandle,
                               sizeof(ROOTHUB_DATA));

        TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                               ControllerData->WmiHandle,
                               sizeof(WMI_DATA));

#if 0


















        if (ControllerData->RegisterHandle != NULL &&
            ControllerData->RegisterHandle->RegistersMapped) {

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RegisterHandle->CapabilityRegisters,
                                   sizeof(CAPABILITY_REGISTERS));

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RegisterHandle->OperationalRegisters,
                                   sizeof(OPERATIONAL_REGISTERS));

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RegisterHandle->RuntimeRegisters,
                                   FIELD_OFFSET(RUNTIME_REGISTERS, InterrupterRegisters) +
                                   (ControllerData->RegisterHandle->SupportedNumberOfInterrupters * sizeof(INTERRUPTER_REGISTER_SET)));

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RegisterHandle->DoorbellRegisters,
                                   sizeof(DOORBELL_REGISTER));

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RegisterHandle->ExtendedCapabilityRegisters,
                                   sizeof(EXTENDED_CAPABILITY));

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RegisterHandle->UsbLegSupCapability,
                                   sizeof(USBLEGSUPCAPABILITY));
        }

        if (ControllerData->RootHubHandle != NULL) {

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RootHubHandle->OperationalRegisters,
                                   sizeof(OPERATIONAL_REGISTERS));

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RootHubHandle->PortRegisterSet,
                                   ControllerData->RootHubHandle->NumberOfPorts * sizeof(PORT_REGISTER_SET));

            TELEMETRY_INSERT_BLOCK(TelemetryHandle,
                                   ControllerData->RootHubHandle->PortData,
                                   sizeof(ROOTHUB_PORT_DATA));
        }
#endif

    } FINALLY {
    }
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
Controller_PopulateHardwareVerifierFlags(
    __in
        PCONTROLLER_DATA    ControllerData
    )
/*++

Routine Description

    This routine opens the hardware verifier registry key HKLM\System\
    CSS\Control\usb\HardwareVerifier\<VVVVDDDDRR> or <VVVVDDDD> (where VVVV
    is the VendorId and DDDD is the deviceId and RR is revisionId of the controller)
    If the above key does not exist global settings are used from the key
    HKLM\System\CSS\Control\usb\HardwareVerifier\Global\xHCI.

--*/
{
    NTSTATUS            status;
    WDFKEY              verifierKey;
    WDFKEY              deviceSubkey;
    WDFKEY              versionSubKey;
    PCUNICODE_STRING    versionSubKeyName;
    UCHAR               xHCIMajorVersion;
    UCHAR               xHCIMinorVersion;
    ULONG64             verifierFlags;

    DECLARE_UNICODE_STRING_SIZE(deviceSubkeyName, VID_PID_REV_BUFFER_SIZE);

    PAGED_CODE();

    TRY {

        verifierFlags = 0;
        verifierKey = NULL;
        deviceSubkey = NULL;
        versionSubKey = NULL;

        //
        // Look up XHCI protocol key based on controller version
        //
        xHCIMajorVersion = Controller_GetXhciMajorVersion(ControllerData);
        xHCIMinorVersion = Controller_GetXhciMinorVersion(ControllerData);

        if (xHCIMajorVersion < XHCI_MAJOR_VERSION_1) {

            versionSubKeyName = &g_HwVerifierxHCI96;

        } else if (xHCIMajorVersion == XHCI_MAJOR_VERSION_1 &&
                   xHCIMinorVersion == 0) {

            versionSubKeyName = &g_HwVerifierxHCI10;

        } else {

            LogWarning(ControllerData->RecorderLog, Controller,
                       "Hardware verifier unsupported XHCI version %d.%d, using xHCI 1.0 verifierFlags",
                       xHCIMajorVersion,
                       xHCIMinorVersion);

            versionSubKeyName = &g_HwVerifierxHCI10;

        }

        //
        // Open the verifier registry root key
        //
        status = WdfRegistryOpenKey(NULL,
                                    &g_HwVerifierKeyName,
                                    KEY_READ,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &verifierKey);


        if (!NT_SUCCESS(status)) {
            verifierKey = NULL;
            LEAVE;
        }

        //
        // Get Controller VID/PID and Device ID
        //
        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {

            status = RtlUnicodeStringPrintf(&deviceSubkeyName,
                                            L"%04X%04X%02X",
                                            Controller_GetPciVendorID(ControllerData),
                                            Controller_GetPciDeviceID(ControllerData),
                                            Controller_GetPciRevisionID(ControllerData));

        } else if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypeAcpi) {

            status =  RtlUnicodeStringPrintf(&deviceSubkeyName,
                                             L"%S%S%S",
                                             Controller_GetAcpiVendorID(ControllerData),
                                             Controller_GetAcpiDeviceID(ControllerData),
                                             Controller_GetAcpiRevisionID(ControllerData));

        } else {
            LogError(ControllerData->RecorderLog, Controller,
                     "Unsupported controller parent bus type.");
            LEAVE;
        }

        //
        // First check if there is something specific for this deivce
        // VID, PID and revision
        //
        if (!NT_SUCCESS(status)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Unable to format VendorId/DeviceId/Rev subkey name %!STATUS!",
                     status);
            LEAVE;
        }

        status = WdfRegistryOpenKey(verifierKey,
                                    &deviceSubkeyName,
                                    KEY_READ,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &deviceSubkey);

        if (!NT_SUCCESS(status)) {
            deviceSubkey = NULL;
        }

        if (NT_SUCCESS(status)) {
            //
            // Try to open the Version specific subkey
            //
            status = WdfRegistryOpenKey(deviceSubkey,
                                        versionSubKeyName,
                                        KEY_READ,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &versionSubKey);
            if (!NT_SUCCESS(status)) {
                versionSubKey = NULL;
            }
        }

        if (NT_SUCCESS(status)) {

            status = WdfRegistryQueryValue(versionSubKey,
                                           &g_HwVerifierControllerName,
                                           sizeof(verifierFlags),
                                           &verifierFlags,
                                           NULL,
                                           NULL);
        }

        if (NT_SUCCESS(status) ||
            (status != STATUS_OBJECT_NAME_NOT_FOUND)) {
            //
            // We either found a value specific to this device
            // and we don't need to look any more OR one of the
            // APIs failed with unexpected status
            //
            LEAVE;
        }

        if (versionSubKey) {
            WdfRegistryClose(versionSubKey);
            versionSubKey = NULL;
        }

        if (deviceSubkey) {
            WdfRegistryClose(deviceSubkey);
            deviceSubkey = NULL;
        }

        //
        // Now let us check if there is something specific for this deivce
        // VendorId and DeviceId
        //

        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {

            status = RtlUnicodeStringPrintf(&deviceSubkeyName,
                                        L"%04X%04X",
                                        Controller_GetPciVendorID(ControllerData),
                                        Controller_GetPciDeviceID(ControllerData));

        } else if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypeAcpi) {

            status =  RtlUnicodeStringPrintf(&deviceSubkeyName,
                                             L"%S%S",
                                             Controller_GetAcpiVendorID(ControllerData),
                                             Controller_GetAcpiDeviceID(ControllerData));

        } else {
            LogError(ControllerData->RecorderLog, Controller,
                     "Unsupported controller parent bus type.");
            LEAVE;
        }


        if (!NT_SUCCESS(status)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "Unable to format VendorId/DeviceId subkey name %!STATUS!",
                     status);
            LEAVE;
        }

        status = WdfRegistryOpenKey(verifierKey,
                                    &deviceSubkeyName,
                                    KEY_READ,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &deviceSubkey);

        if (!NT_SUCCESS(status)) {
            deviceSubkey = NULL;
        }


        if (NT_SUCCESS(status)) {
            //
            // Try to open the Version specific subkey
            //
            status = WdfRegistryOpenKey(deviceSubkey,
                                        versionSubKeyName,
                                        KEY_READ,
                                        WDF_NO_OBJECT_ATTRIBUTES,
                                        &versionSubKey);
            if (!NT_SUCCESS(status)) {
                versionSubKey = NULL;
            }
        }


        if (NT_SUCCESS(status)) {

            status = WdfRegistryQueryValue(versionSubKey,
                                           &g_HwVerifierControllerName,
                                           sizeof(verifierFlags),
                                           &verifierFlags,
                                           NULL,
                                           NULL);
        }

        if (NT_SUCCESS(status) ||
            (status != STATUS_OBJECT_NAME_NOT_FOUND)) {
            //
            // We either found a value specific to this device
            // and we don't need to look any more OR one of the
            // APIs failes with unexpected status
            //
            LEAVE;
        }

        if (versionSubKey) {
            WdfRegistryClose(versionSubKey);
            versionSubKey = NULL;
        }

        if (deviceSubkey) {
            WdfRegistryClose(deviceSubkey);
            deviceSubkey = NULL;
        }

        //
        // If we didn't find anything specific to the device,
        // we will look at the global value
        //
        status = WdfRegistryOpenKey(verifierKey,
                                    &g_HwVerifierGlobalName,
                                    KEY_READ,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &deviceSubkey);


        if (!NT_SUCCESS(status)) {
            deviceSubkey = NULL;
            LEAVE;
        }

        status = WdfRegistryOpenKey(deviceSubkey,
                                    versionSubKeyName,
                                    KEY_READ,
                                    WDF_NO_OBJECT_ATTRIBUTES,
                                    &versionSubKey);

        if (!NT_SUCCESS(status)) {
            versionSubKey = NULL;
            LEAVE;
        }

        status = WdfRegistryQueryValue(versionSubKey,
                                       &g_HwVerifierControllerName,
                                       sizeof(verifierFlags),
                                       &verifierFlags,
                                       NULL,
                                       NULL);

        if (NT_SUCCESS(status) ||
            (status != STATUS_OBJECT_NAME_NOT_FOUND)) {
            //
            // We either found a value specific to this device
            // and we don't need to look any more OR one of the
            // APIs failes with unexpected status
            //
            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            //
            // If flags were not explicitly specified using any of the regkeys,
            // then set all the flags on DBG build, otherwise clear them all.
            // But SaveRestore failure can get too verbose, so keep that one off.
            //
#if DBG
            ControllerData->HardwareVerifierFlags = (ControllerHwVerifierMax - 1);
            ControllerData->HardwareVerifierFlags &= (~ControllerHwVerifierControllerSaveRestoreFailed);
#else
            ControllerData->HardwareVerifierFlags = 0;
#endif

            if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
                status = STATUS_SUCCESS;

            } else {

                LogError(ControllerData->RecorderLog, Controller,
                         "Failed to read verifier registry key %!STATUS!",
                         status);
            }

        } else {

            ControllerData->HardwareVerifierFlags = verifierFlags;
        }

        LogInfo(ControllerData->RecorderLog, Controller,
            "Adjusted HardwareVerifierFlags value: 0x%I64x", ControllerData->HardwareVerifierFlags);

        if (versionSubKey) {
            WdfRegistryClose(versionSubKey);
        }

        if (deviceSubkey) {
            WdfRegistryClose(deviceSubkey);
        }

        if (verifierKey) {
            WdfRegistryClose(verifierKey);
        }
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
Controller_TelemetryOkToGenerateReport(
    __in
        PCONTROLLER_DATA     ControllerData,
    __in
        PXHCI_TELEMETRY_DATA TelemetryData,
    __in
        ULONG                ReasonCode
    )
/*++

Routine Description:

    This routine decides whether a telemetry report should be generated for
    the given reason code.

    This routine is not re-entrant and is protected by the telemetry workitem's mutex

--*/
{
    NTSTATUS        status;
    WDFKEY          deviceKey;
    UNICODE_STRING  valueName;
    ULONG           value;
    ULONG64         firmwareVersion;
    BOOLEAN         generateReport;

    PAGED_CODE();

    TRY {

        deviceKey       = NULL;
        generateReport  = FALSE;

        //
        // Valid reason code should be in the range 0x1001 - 0x103F.
        //
        NT_ASSERTMSG("Unexpected reason code", ((ReasonCode & ~(0x103FUL)) == 0));

        //
        // Telemetry report should be written no more than once per reason code,
        // per controller, per boot.
        //
        if (ControllerData->TelemetryHistory & (1ULL << (ReasonCode & 0x3FUL))) {

            LogInfo(ControllerData->RecorderLog, Controller,
                    "Skipping writing of telemetry report for reason code %!BugCheckReasonCode!",
                    ReasonCode);
            LEAVE;
        }

        switch (ReasonCode) {

        case USB3_WER_BUGCODE_USBXHCI_FIRMWARE_OUTDATED:

            //
            // For a given outdated firmware version, do not write the report more
            // than once per the lifetime of the OS install.
            //
            status = WdfDeviceOpenRegistryKey(ControllerData->WdfDevice,
                                              PLUGPLAY_REGKEY_DEVICE,
                                              KEY_WRITE,
                                              WDF_NO_OBJECT_ATTRIBUTES,
                                              &deviceKey);

            if (!NT_SUCCESS(status)) {

                LogWarning(ControllerData->RecorderLog, Controller,
                           "WdfDeviceOpenRegistryKey Failed %!STATUS!", status);
                LEAVE;
            }

            RtlInitUnicodeString(&valueName, DEVICE_REGISTRY_KEY_OUTDATED_FIRMWARE_VERSION);

            status = WdfRegistryQueryValue(deviceKey,
                                           &valueName,
                                           sizeof(ULONG64),
                                           &firmwareVersion,
                                           NULL,
                                           NULL);

            if (!NT_SUCCESS(status)) {

                if (status != STATUS_OBJECT_NAME_NOT_FOUND) {

                    LogWarning(ControllerData->RecorderLog, Controller,
                               "WdfRegistryQueryValue Failed %!STATUS!", status);
                    LEAVE;
                }

            } else {

                if (ControllerData->FirmwareVersion == firmwareVersion) {
                    LEAVE;
                }
            }

            generateReport = TRUE;
            break;

        case USB3_WER_BUGCODE_USBXHCI_OUT_OF_ORDER_COMMAND_COMPLETION:

            //
            // Do not write this report more than once per the lifetime of the OS install.
            //
            status = WdfDeviceOpenRegistryKey(ControllerData->WdfDevice,
                                              PLUGPLAY_REGKEY_DEVICE,
                                              KEY_WRITE,
                                              WDF_NO_OBJECT_ATTRIBUTES,
                                              &deviceKey);

            if (!NT_SUCCESS(status)) {

                LogWarning(ControllerData->RecorderLog, Controller,
                           "WdfDeviceOpenRegistryKey Failed %!STATUS!", status);
                LEAVE;
            }

            RtlInitUnicodeString(&valueName, DEVICE_REGISTRY_KEY_OUT_OF_ORDER_COMMAND_COMPLETION);

            status = WdfRegistryQueryValue(deviceKey,
                                           &valueName,
                                           sizeof(ULONG),
                                           &value,
                                           NULL,
                                           NULL);

            if (!NT_SUCCESS(status) && (status != STATUS_OBJECT_NAME_NOT_FOUND)) {

                LogWarning(ControllerData->RecorderLog, Controller,
                           "WdfRegistryQueryValue Failed %!STATUS!", status);
                LEAVE;
            }

            generateReport = TRUE;
            break;

        case USB3_WER_BUGCODE_USBXHCI_CONTROLLER_GONE:

            //
            // Do not write this report more than once
            //
            if (TelemetryData->GontrollerGoneDumpGenerated == 0) {
                generateReport = TRUE;
                TelemetryData->GontrollerGoneDumpGenerated = 1;
            } else {
                generateReport = FALSE;
            }
            break;

        default:
            generateReport = TRUE;
            break;
        }

    } FINALLY {

        if (deviceKey != NULL) {
            WdfRegistryClose(deviceKey);
        }

    }

    return generateReport;
}

VOID
Controller_TelemetryReport(
    __in
        PCONTROLLER_DATA           ControllerData,
    __in
        PXHCI_TELEMETRY_DATA       TelemetryData,
    __in
        PXHCI_TELEMETRY_DATA_ENTRY TelemetryDataEntry,
    __in_ecount(NumDevices)
        PDEVICE_OBJECT            *DeviceList,
    __in
        ULONG                      NumDevices
    )
{

    NTSTATUS                   status;
    XHCI_LIVEDUMP_CONTEXT      xhciLiveDumpContext;
    HANDLE                     telemetryHandle;
    PVOID                      ifrLog;
    ULONG                      ifrLogSize;
    PVOID                      wppDriverContext;
    ULONG                      wppDriverContextSize;
    GUID                       wppGuid;
    PDEVICE_DATA               deviceData;
    ULONG                      i;

    TRY {
        telemetryHandle = NULL;

        if (Controller_TelemetryOkToGenerateReport(ControllerData,
                                                   TelemetryData,
                                                   TelemetryDataEntry->ReasonCode)
            == FALSE) {
            LEAVE;
        }

        //
        // Initialize the xhci live dump context which is the parameter # 2 for
        // all xHCI live dumps.
        //
        RtlZeroMemory(&xhciLiveDumpContext, sizeof(XHCI_LIVEDUMP_CONTEXT));

        xhciLiveDumpContext.ControllerHandle    = ControllerData;
        xhciLiveDumpContext.UsbDeviceHandle     = TelemetryDataEntry->UsbDeviceHandle;
        xhciLiveDumpContext.EndpointHandle      = TelemetryDataEntry->EndpointHandle;
        xhciLiveDumpContext.TransferRingHandle  = TelemetryDataEntry->TransferRingHandle;

        if (Controller_GetParentBusType(ControllerData) == UcxControllerParentBusTypePci) {

            RtlStringCchPrintfA(xhciLiveDumpContext.VendorId,
                                sizeof(xhciLiveDumpContext.VendorId),
                                "%04X",
                                Controller_GetPciVendorID(ControllerData));

            RtlStringCchPrintfA(xhciLiveDumpContext.DeviceId,
                                sizeof(xhciLiveDumpContext.DeviceId),
                                "%04X",
                                Controller_GetPciDeviceID(ControllerData));

            RtlStringCchPrintfA(xhciLiveDumpContext.RevisionId,
                                sizeof(xhciLiveDumpContext.RevisionId),
                                "%02X",
                                Controller_GetPciRevisionID(ControllerData));

        } else {

            RtlStringCchPrintfA(xhciLiveDumpContext.VendorId,
                                sizeof(xhciLiveDumpContext.VendorId),
                                "%s",
                                Controller_GetAcpiVendorID(ControllerData));

            RtlStringCchPrintfA(xhciLiveDumpContext.DeviceId,
                                sizeof(xhciLiveDumpContext.DeviceId),
                                "%s",
                                Controller_GetAcpiDeviceID(ControllerData));

            RtlStringCchPrintfA(xhciLiveDumpContext.RevisionId,
                                sizeof(xhciLiveDumpContext.RevisionId),
                                "%s",
                                Controller_GetAcpiRevisionID(ControllerData));
        }

        RtlStringCchPrintfA(xhciLiveDumpContext.FirmwareVersion,
                            sizeof(xhciLiveDumpContext.FirmwareVersion),
                            "%I64X",
                            Controller_GetFirmwareVersion(ControllerData));

        telemetryHandle = TelemetryData_CreateReport((PWCHAR)&USBXHCI_TELEMETRY_REPORT_TYPE,
                                                     BUGCODE_USB3_DRIVER,
                                                     TelemetryDataEntry->ReasonCode,
                                                     (ULONG_PTR)&xhciLiveDumpContext,
                                                     0,
                                                     0);

        if (telemetryHandle == NULL) {

            LogError(ControllerData->RecorderLog, Controller, "TelemetryData_CreateReport Failed");
            LEAVE;

        }

        TELEMETRY_INSERT_BLOCK(telemetryHandle, &xhciLiveDumpContext, sizeof(XHCI_LIVEDUMP_CONTEXT));

        TELEMETRY_INSERT_BLOCK(telemetryHandle, &g_UsbXhciDriverObject, sizeof(PVOID));

        TELEMETRY_INSERT_BLOCK(telemetryHandle, g_UsbXhciDriverObject, sizeof(DRIVER_OBJECT));

        //
        // Add controller data and IFR logs
        //
        Controller_TelemetryAddControllerData(ControllerData, telemetryHandle);

        //
        // Store the global pointer to the WPP control block
        //
        TELEMETRY_INSERT_BLOCK(telemetryHandle, &WPP_GLOBAL_Control, sizeof(PVOID));

        //
        // Store the WPP control block if it exists
        //
        if (WPP_GLOBAL_Control != NULL) {

            TELEMETRY_INSERT_BLOCK(telemetryHandle, WPP_GLOBAL_Control, sizeof(*WPP_GLOBAL_Control));
        }

        //
        // Store the WPP driver context
        //
        status = WppRecorderDumpLiveDriverData(&wppDriverContext,
                                               &wppDriverContextSize,
                                               &wppGuid);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        TELEMETRY_INSERT_BLOCK(telemetryHandle, wppDriverContext, wppDriverContextSize);

        //
        // Add all XHCI device objects
        //
        for (i = 0; i < NumDevices; i++) {

            //
            // Add the device object
            //
            if (!NT_SUCCESS(TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                                DeviceList[i],
                                                                sizeof(DEVICE_OBJECT)))) {
                break;
            }

            deviceData = DeviceList[i]->DeviceExtension;

            if (deviceData->ControllerData != ControllerData) {
                continue;
            }

            if (!NT_SUCCESS(TelemetryData_InsertTriageDataBlock(telemetryHandle,
                                                                deviceData,
                                                                sizeof(DEVICE_DATA)))) {
                break;
            }

            //
            // Store the device data's IFR log
            //
            status = WppRecorderLogDumpLiveData(deviceData->RecorderLog,
                                                &ifrLog,
                                                &ifrLogSize,
                                                &wppGuid);

            if (!NT_SUCCESS(status)) {
                break;
            }

            status = TelemetryData_InsertTriageDataBlock(telemetryHandle, ifrLog, ifrLogSize);

            if (!NT_SUCCESS(status)) {
                break;
            }
        }

        status = TelemetryData_SubmitReport(telemetryHandle);

        if (!NT_SUCCESS(status)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "TelemetryData_SubmitReport Failed %!STATUS!", status);
            LEAVE;
        }

        Controller_TelemetryUpdateSubmitReportSuccess(ControllerData,
                                                      TelemetryDataEntry->ReasonCode);

    } FINALLY {

        if (telemetryHandle != NULL) {
            TelemetryData_CloseHandle(telemetryHandle);
        }

    }
}

VOID
Controller_TelemetryReportWorker(
    WDFWORKITEM WdfWorkItem
    )
/*++

Routine Description:

    This routine is a Wdf work item that executes at PASSIVE_LEVEL and writes
    out a live kernel dump. PASSIVE_LEVEL execution is required for
    TelemetryData_CreateReport to capture the module information.

--*/
{
    PCONTROLLER_DATA           controllerData;
    NTSTATUS                   status;
    PXHCI_TELEMETRY_DATA       telemetryData;
    PDEVICE_OBJECT            *deviceList;
    ULONG                      numDevices;
    ULONG                      i;
    LIST_ENTRY                 telemetryList;
    ULONG                      recoveryActions;
    PXHCI_TELEMETRY_DATA_ENTRY telemetryDataEntry;
    KIRQL                      irql;

    TRY {

        deviceList      = NULL;
        numDevices      = 0;

        controllerData  = GetControllerData(WdfWorkItemGetParentObject(WdfWorkItem));
        telemetryData   = GetTelemetryData(WdfWorkItem);

        LogInfo(controllerData->RecorderLog, Controller,
                 "Controller_TelemetryReportWorker Start");

        KeWaitForMutexObject(&telemetryData->XhciTelemetryMutex,
                             Executive,
                             KernelMode,
                             FALSE,
                             NULL);

        KeAcquireSpinLock(&telemetryData->XhciTelemetryDataListLock, &irql);

        //
        // Move the Telemetry list to a local listhead
        //
        TRANSFER_LIST(&telemetryList, &telemetryData->XhciTelemetryDataListHead);

        //
        // Move the recovery actions to a local variable
        //
        recoveryActions = telemetryData->RecoveryActions;
        telemetryData->RecoveryActions = 0;

        KeReleaseSpinLock(&telemetryData->XhciTelemetryDataListLock, irql);

        //
        // Get list of device object
        //
        status = IoEnumerateDeviceObjectList(g_UsbXhciDriverObject,
                                             NULL,
                                             0,
                                             &numDevices);

        if (status != STATUS_BUFFER_TOO_SMALL) {

            LogError(controllerData->RecorderLog, Controller,
                     "IoEnumerateDeviceObjectList Failed %!STATUS!", status);
            LEAVE;

        }

        deviceList = ExAllocatePoolWithTag(NonPagedPool,
                                           numDevices * sizeof(PDEVICE_OBJECT),
                                           DriverAllocationTag);

        if (deviceList == NULL) {

            LogError(controllerData->RecorderLog, Controller,
                     "Unable to allocate memory for device object list");
            LEAVE;

        }

        status = IoEnumerateDeviceObjectList(g_UsbXhciDriverObject,
                                             deviceList,
                                             numDevices * sizeof(PDEVICE_OBJECT),
                                             &i);

        if (!NT_SUCCESS(status)) {

            LogError(controllerData->RecorderLog, Controller,
                     "IoEnumberateDeviceObjectList Failed %!STATUS!", status);

            ExFreePoolWithTag(deviceList, DriverAllocationTag);
            deviceList = NULL;
            LEAVE;
        }

        NT_ASSERTMSG("Number of device objects changed between calls to IoEnumerateDeviceObjectList", i == numDevices);

        FOR_ALL_IN_LIST(XHCI_TELEMETRY_DATA_ENTRY,
                        &telemetryList,
                        XhciTelemetryDataListEntry,
                        telemetryDataEntry) {

            //
            // Generate a live dump for each entry in the telemetry list
            //
            Controller_TelemetryReport(controllerData,
                                       telemetryData,
                                       telemetryDataEntry,
                                       deviceList,
                                       numDevices);

        }

    } FINALLY {

        if (deviceList != NULL) {

            //
            // Dereference device objects, referenced by IoEnumerateDeviceObjectList()
            //
            for (i = 0; i < numDevices; i++) {
                ObDereferenceObject(deviceList[i]);
            }

            ExFreePoolWithTag(deviceList, DriverAllocationTag);
        }

        while (FALSE == IsListEmpty(&telemetryList)) {

            telemetryDataEntry = CONTAINING_RECORD(RemoveHeadList(&telemetryList),
                                                   XHCI_TELEMETRY_DATA_ENTRY,
                                                   XhciTelemetryDataListEntry);

            ExFreePoolWithTag(telemetryDataEntry, DriverAllocationTag);
        }

        //
        // Initiate recovery actions
        //
        Controller_InitiateRecovery(controllerData, telemetryData, recoveryActions);

        KeReleaseMutex(&telemetryData->XhciTelemetryMutex, FALSE);

    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_ReportFatalError(
    __in
        PCONTROLLER_DATA            ControllerData,
    __in
        Controller_FatalErrorAction RecoveryAction,
    __in
        ULONG                       ReasonCode,
    __in_opt
        HANDLE                      UsbDeviceHandle,
    __in_opt
        HANDLE                      EndpointHandle,
    __in_opt
        HANDLE                      TransferRingHandle
    )
{
    PXHCI_TELEMETRY_DATA telemetryData;
    PXHCI_TELEMETRY_DATA_ENTRY telemetryDataEntry;
    KIRQL  irql;

    LogFatal(ControllerData->RecorderLog, Controller,
             "XHCI reported fatal error: reason: %!BugCheckReasonCode!, %!Controller_FatalErrorAction!",
             ReasonCode,
             RecoveryAction);
    DbgPrint("\nXHCI reported fatal error\n");

    //
    // We will not have live dumps for boot devices for now
    //
    if (Controller_InBootOrPagingPath(ControllerData)) {

        Controller_InitiateBootRecovery(ControllerData, RecoveryAction);
        return;
    }

    telemetryData = GetTelemetryData(ControllerData->TelemetryReportWorkItem);

    telemetryDataEntry = NULL;

#if !defined (DSF_DRIVER)
    //
    // Note: We don't create a minidump for a DSF controller.
    //
    if (ReasonCode != USB3_WER_BUGCODE_USBXHCI_NO_LIVE_DUMP) {
        telemetryDataEntry = ExAllocatePoolWithTag(NonPagedPool,
                                                   sizeof(XHCI_TELEMETRY_DATA_ENTRY),
                                                   DriverAllocationTag);
    }
#endif

#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST, "Based on the conditional compile above, this may be redundant")
    if (NULL != telemetryDataEntry) {

        telemetryDataEntry->ReasonCode           = ReasonCode;
        telemetryDataEntry->UsbDeviceHandle      = UsbDeviceHandle;
        telemetryDataEntry->EndpointHandle       = EndpointHandle;
        telemetryDataEntry->TransferRingHandle   = TransferRingHandle;

        InitializeListEntry(&telemetryDataEntry->XhciTelemetryDataListEntry);

    }

    KeAcquireSpinLock(&telemetryData->XhciTelemetryDataListLock, &irql);

#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST, "Based on the conditional compile above, this may be redundant")
    if (NULL != telemetryDataEntry) {

        InsertTailList(&telemetryData->XhciTelemetryDataListHead,
                       &telemetryDataEntry->XhciTelemetryDataListEntry);

    }

    SET_FLAG(telemetryData->RecoveryActions, RecoveryAction);

    KeReleaseSpinLock(&telemetryData->XhciTelemetryDataListLock, irql);

    //
    // The telemetry work item will perform the recovery action for this controller
    //
    WdfWorkItemEnqueue(ControllerData->TelemetryReportWorkItem);

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Controller_IsControllerAccessible(
    __in
        PCONTROLLER_DATA    ControllerData
    )
{
    if (Register_AreRegistersMapped(ControllerData->RegisterHandle) &&
        (ControllerData->ControllerGone == FALSE)) {
        return TRUE;
    } else {
        LogError(ControllerData->RecorderLog, Controller, "HW_COMPLIANCE: IsControllerAccessible failed!  AreRegistersMapped = %u, ControllerGone = %u",
                 (ULONG)Register_AreRegistersMapped(ControllerData->RegisterHandle),
                 (ULONG)ControllerData->ControllerGone);

        return FALSE;
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_HwVerifierBreakIfEnabled(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in_opt
        UCXUSBDEVICE        Device,
    __in_opt
        UCXENDPOINT         Endpoint,
    __in
        ULONG64             Flag,
    __in
        PCSTR               MessageStr,
    __in_opt
        PTRB                CommandTRB,
    __in_opt
        PTRB                EventTRB
    )
/*++

Routine Description:

    This routine breaks into the kernel debugger if the given hardware verifier
    flag is enabled

--*/
{
    TRB     dummyTRB;
    PUCHAR  localCommandTRB;
    PUCHAR  localEventTRB;

    LogError(ControllerData->RecorderLog, Controller, "xHCI Hardware Verifier condition was hit: %s",
             MessageStr);

    if (Controller_GetHardwareVerifierFlags(ControllerData) & Flag) {

    DbgPrint("xHCI Hardware Verifier Break: %s\n",
                  MessageStr);
#if DBG
        NT_ASSERTMSG("xHCI Hardware verifier Break", 0);
#else
        if (KdRefreshDebuggerNotPresent() == FALSE) {

            DbgBreakPoint();
        }
#endif

    }

    //
    // Since we promised ETW to log 16 bytes each for the CommandTRB and EventTRB,
    // we have to log dummy data if the actual pointers supplied to us are NULL.
    //
    RtlZeroMemory(&dummyTRB, sizeof(TRB));

    if (CommandTRB != NULL) {
        localCommandTRB = (PUCHAR)CommandTRB;
    } else {
        localCommandTRB = (PUCHAR)&dummyTRB;
    }

    if (EventTRB != NULL) {
        localEventTRB = (PUCHAR)EventTRB;
    } else {
        localEventTRB = (PUCHAR)&dummyTRB;
    }

    EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_ERROR(
        NULL,
        ControllerData->UcxController,
        Device,
        Endpoint,
        Flag,
        MessageStr,
        localCommandTRB,
        localEventTRB);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_InitiateRecovery(
    __in
        PCONTROLLER_DATA            ControllerData,
    __in
        PXHCI_TELEMETRY_DATA        TelemetryData,
    __in
        ULONG                       RecoveryActions
    )
/*++

Routine Description:

    This routine initiates recovery action after controller failure

--*/
{
    WDF_DEVICE_STATE deviceState;

    LogInfo(ControllerData->RecorderLog, Controller,
            "Begin: Controller_InitiateRecovery");

    //
    // If reset recovery has been invoked too many times in a short time
    // interval, disable the controller.
    //
    if (TEST_FLAG(RecoveryActions, Controller_FatalErrorReset) &&
        (ControllerData->ResetCount > CONTROLLER_MAX_RESET_RECOVERY_PER_INTERVAL)) {

        SET_FLAG(RecoveryActions, Controller_FatalErrorNoRestart);

        LogFatal(ControllerData->RecorderLog, Controller,
                        "Controller_InitiateRecovery: Setting flag Controller_FatalErrorNoRestart: %d > %d", ControllerData->ResetCount, CONTROLLER_MAX_RESET_RECOVERY_PER_INTERVAL);
    }

    //
    // If HW verifier is enabled for the controller, break into the debugger before intiating
    // recovery, reset or notification
    //
    while (RecoveryActions != 0) {

        if (TEST_FLAG(RecoveryActions, Controller_FatalErrorControllerGoneNotifyUCXAndPNP) ||
            TEST_FLAG(RecoveryActions, Controller_FatalErrorControllerGoneNotifyUCX)) {

            if (TelemetryData->ControllerGoneRecoveryActionProcessed == 0) {

                //
                // Immediately Fail All Commands
                //
                Command_ControllerGone(ControllerData->CommandHandle);

                KeFlushQueuedDpcs();

                //
                // Ensure any CommonBuffer callbacks have been completed.
                //
                CommonBuffer_ControllerGone(ControllerData->CommonBufferHandle);

                //
                // Complete any device managemnt requests
                //
                DeviceSlot_ControllerGone(ControllerData->DeviceSlotHandle);

                //
                // Notify UCX that the controller is gone.
                //
                KeFlushQueuedDpcs();
                UcxControllerSetFailed(ControllerData->UcxController);

                NT_ASSERT(TelemetryData->ControllerGonePnpNotified == 0);

                if (TEST_FLAG(RecoveryActions, Controller_FatalErrorControllerGoneNotifyUCX)) {
                    //
                    // This is not a case of removal, so notify Wdf about the failure.
                    //
                    WdfDeviceSetFailed(ControllerData->WdfDevice, WdfDeviceFailedNoRestart);
                }

                TelemetryData->ControllerGoneRecoveryActionProcessed = 1;
            }

            if ((TelemetryData->ControllerGonePnpNotified == 0) &&
                TEST_FLAG(RecoveryActions, Controller_FatalErrorControllerGoneNotifyUCXAndPNP)) {

                LogInfo(ControllerData->RecorderLog, Controller,
                        "Controller_InitiateRecovery: Notifying PnP that the controller is gone");

                WDF_DEVICE_STATE_INIT(&deviceState);
                deviceState.Removed = WdfTrue;
                WdfDeviceSetDeviceState(ControllerData->WdfDevice, &deviceState);
                TelemetryData->ControllerGonePnpNotified = 1;
            }

            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorControllerGoneNotifyUCXAndPNP);
            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorControllerGoneNotifyUCX);
            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorInternalReset);
            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorReset);
            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorNoRestart);

        } else if (TEST_FLAG(RecoveryActions, Controller_FatalErrorNoRestart)) {

            Controller_DisableController(ControllerData);
            //
            // TODO: PostWin8
            // This call to  the call WdfDeviceSetFailed can be removed since
            // Controller_DisableController would queue a
            // Controller_FatalErrorControllerGoneNotifyUCX recovery action and that would
            // call WdfDeviceSetFailed
            //
            WdfDeviceSetFailed(ControllerData->WdfDevice, WdfDeviceFailedNoRestart);

            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorInternalReset);
            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorReset);
            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorNoRestart);

        } else if (TEST_FLAG(RecoveryActions, Controller_FatalErrorInternalReset)) {

            Controller_InternalReset(ControllerData);

            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorInternalReset);

        } else if (TEST_FLAG(RecoveryActions, Controller_FatalErrorReset)) {

            //
            // Initiate non-PnP error recovery.
            //
            UcxControllerNeedsReset(Controller_GetUcxControllerObject(ControllerData));

            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorReset);

        } else if (TEST_FLAG(RecoveryActions, Controller_FatalErrorReportFailedNoRestart)) {

            //
            // This action is called from the Prepare Hardware to report the device
            // as FailedNoRestart to WDF (PNP) in order to avoid endlessly attempting
            // to restart the device.
            //
            WdfDeviceSetFailed(ControllerData->WdfDevice, WdfDeviceFailedNoRestart);
            CLEAR_FLAG(RecoveryActions, Controller_FatalErrorReportFailedNoRestart);

        } else {

            NT_ASSERTMSG("Invalid RecoveryActions 0x%x", RecoveryActions);
            LogError(ControllerData->RecorderLog, Controller,
                    "ERROR_ASSERT: Invalid RecoveryActions 0x%x",
                    RecoveryActions);

            //
            // Break to loop
            //
            RecoveryActions = 0;
        }
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_TelemetryUpdateSubmitReportSuccess(
    __in
        PCONTROLLER_DATA    ControllerData,
    __in
        ULONG               ReasonCode
    )
/*++

Routine Description:

    This routine marks that a telemetry report should not be generated for the given reason
    code more than once per boot
    USB3_WER_BUGCODE_USBXHCI_FIRMWARE_OUTDATED is a special case and should not
    be generated more than once for the lifetime of the OS for a given firmware version

--*/
{
    NTSTATUS        status;
    WDFKEY          deviceKey;
    UNICODE_STRING  valueName;
    ULONG           value;

    PAGED_CODE();

    TRY {

        deviceKey = NULL;

        //
        // Valid reason code should be in the range 0x1001 - 0x103F.
        //
        if((ReasonCode & ~(0x103FUL)) != 0) {
            LogError(ControllerData->RecorderLog, Controller,
                    "ERROR_ASSERT: Unexpected reason code 0x%x",
                    ReasonCode);
        }
        NT_ASSERTMSG("Unexpected reason code", ((ReasonCode & ~(0x103FUL)) == 0));

        ControllerData->TelemetryHistory |= (1ULL << (ReasonCode & 0x3FUL));

        switch (ReasonCode) {

        case USB3_WER_BUGCODE_USBXHCI_FIRMWARE_OUTDATED:

            //
            // Save the firmware version to the registry which will be used to ensure that
            // this report is not written more than once for a given firmware version for
            // the lifetime of this OS install.
            //
            status = WdfDeviceOpenRegistryKey(ControllerData->WdfDevice,
                                              PLUGPLAY_REGKEY_DEVICE,
                                              KEY_WRITE,
                                              WDF_NO_OBJECT_ATTRIBUTES,
                                              &deviceKey);

            if (!NT_SUCCESS(status)) {

                LogWarning(ControllerData->RecorderLog, Controller,
                           "WdfDeviceOpenRegistryKey Failed %!STATUS!", status);
                LEAVE;
            }

            RtlInitUnicodeString(&valueName, DEVICE_REGISTRY_KEY_OUTDATED_FIRMWARE_VERSION);

            status = WdfRegistryAssignValue(deviceKey,
                                            &valueName,
                                            REG_BINARY,
                                            sizeof(ULONG64),
                                            (PVOID)&ControllerData->FirmwareVersion);

            if (!NT_SUCCESS(status)) {

                LogWarning(ControllerData->RecorderLog, Controller,
                           "WdfRegistryAssignValue Failed %!STATUS!", status);
                LEAVE;
            }
            break;

        case USB3_WER_BUGCODE_USBXHCI_OUT_OF_ORDER_COMMAND_COMPLETION:

            //
            // Write a value to the registry which will be used to ensure that this report
            // is not written more than once for the lifetime of this OS install.
            //
            status = WdfDeviceOpenRegistryKey(ControllerData->WdfDevice,
                                              PLUGPLAY_REGKEY_DEVICE,
                                              KEY_WRITE,
                                              WDF_NO_OBJECT_ATTRIBUTES,
                                              &deviceKey);

            if (!NT_SUCCESS(status)) {

                LogWarning(ControllerData->RecorderLog, Controller,
                           "WdfDeviceOpenRegistryKey Failed %!STATUS!", status);
                LEAVE;
            }

            RtlInitUnicodeString(&valueName, DEVICE_REGISTRY_KEY_OUT_OF_ORDER_COMMAND_COMPLETION);

            value = 1;

            status = WdfRegistryAssignValue(deviceKey,
                                            &valueName,
                                            REG_DWORD,
                                            sizeof(ULONG),
                                            &value);

            if (!NT_SUCCESS(status)) {

                LogWarning(ControllerData->RecorderLog, Controller,
                           "WdfRegistryAssignValue Failed %!STATUS!", status);
                LEAVE;
            }
            break;

        default:
            break;
        }

    } FINALLY {

        if (deviceKey != NULL) {
            WdfRegistryClose(deviceKey);
        }
    }
}

































































































__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_InitiateBootRecovery(
    __in
        PCONTROLLER_DATA            ControllerData,
    __in
        ULONG                       RecoveryAction
    )
/*++

Routine Description:

    This routine bypasses telemetry report generation for boot devices.
    Not all code path are thread safe in this routine. Controller_InternalReset cannot be called by
    2 thread in parallel for the same controller. The current design of the code will not let this
    happen. But future changes should be made with this in mind.
    UcxControllerNeedsReset is thread safe.

--*/
{

    //
    // We can use a static variable here inplace of a member in CONTROLLER_DATA as
    // only one controller in the system can be in the boot path and that this function is
    // currently not called by 2 thread at once. Once these assumptions are broken we will
    // need to move this variable to CONTROLLER_DATA
    //
    static ULONG internalResetCounter = 0;
    ULONG        incrementedInternalResetCounter;
    //
    // We should be here only for the boot path
    //
    NT_ASSERT(Controller_InBootOrPagingPath(ControllerData));

    if (TEST_FLAG(RecoveryAction, Controller_FatalErrorControllerGoneNotifyUCXAndPNP) ||
        TEST_FLAG(RecoveryAction, Controller_FatalErrorControllerGoneNotifyUCX) ||
        TEST_FLAG(RecoveryAction, Controller_FatalErrorNoRestart)) {

        Usb3_KeBugCheckEx(USB3_BUGCODE_BOOT_DEVICE_FAILED,
                          Controller_GetWdmPhysicalDeviceObject(ControllerData),
                          FailedBootDeviceType_UsbController,
                          0);

    } else if (TEST_FLAG(RecoveryAction, Controller_FatalErrorInternalReset)) {

        //
        // Adding a assert for calling out explicitly that this function should not be called by
        // by 2 or more threads at the same time
        //
        incrementedInternalResetCounter = InterlockedIncrementULONG(&internalResetCounter);

        if (incrementedInternalResetCounter != 1) {
            NT_ASSERTMSG("More than one simultaneous call to Controller_FatalErrorInternalReset", 0);
        }

        Controller_InternalReset(ControllerData);

        InterlockedDecrementULONG(&internalResetCounter);

    } else if (TEST_FLAG(RecoveryAction, Controller_FatalErrorReset)) {

        //
        // Initiate non-PnP error recovery.
        //
        UcxControllerNeedsReset(Controller_GetUcxControllerObject(ControllerData));

    } else {

        NT_ASSERTMSG("Invalid RecoveryActions 0x%x", RecoveryAction);

    }

}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ExecuteDSMToEnableRTD3(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    If the ExecuteDSMToEnableRTD3InAddDevice workaround is set, execute a _DSM() to change the _S0W()
    return value, thereby enabling or disabling RTD3.

    The arguments to the _DSM are as follows:
    1.	Arg0 - A Buffer containing GUID: GUID_DSM_ENABLE_RTD3
    2.	Arg1 - An Integer containing the value to be returned by _S0W(). '3' to enable RTD3, '0' to disable RTD3.
    3.	Arg2 - Reserved, pass zero.
    4.	Arg3 - Reserved, pass zero.

--*/
{

    PAGED_CODE();

    TRY {

        if (Controller_GetDeviceFlags(ControllerData).ExecuteDSMToEnableRTD3InAddDevice == 0) {
            LEAVE;
        }

        //
        // Argument 1 to _DSM Method: An Integer containing the value to be returned by _S0W(). '3' to enable RTD3.
        //
        Controller_ExecuteDSM(ControllerData, 3);

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ExecuteDSMForHSICDisconnectInU3(
    __in
        PCONTROLLER_DATA ControllerData,
    __in
        BOOLEAN          IsD0Entry
    )
    /*++

    Routine Description:

    If the ExecuteDSMForHSICDisconnectInU3 workaround is set, execute a _DSM() to support HSIC and SSIC
    devices to force a disconnect in U3.

    The arguments to the _DSM are as follows:
    1.  Arg0 - A Buffer containing GUID: GUID_DSM_ENABLE_RTD3
    2.  Arg1 - 0x6 on D0Exit and 0x5 on D0Entry
    3.  Arg2 - Reserved, pass zero.
    4.  Arg3 - Reserved, pass zero.

    --*/
{

    PAGED_CODE();

    TRY {

        if (Controller_GetDeviceFlags(ControllerData).ExecuteDSMForHSICDisconnectInU3 == 0) {
            LEAVE;
        }

        if (IsD0Entry) {
            Controller_ExecuteDSM(ControllerData, 5);
        } else {
            Controller_ExecuteDSM(ControllerData, 6);
        }

    } FINALLY{

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ExecuteDSM(
    __in
        PCONTROLLER_DATA ControllerData,
    __in 
        ULONG Argument1
    )
    /*++

    Routine Description:

    Execute a _DSM() method

    The arguments to the _DSM are as follows:
    1.  Arg0 - A Buffer containing GUID: GUID_DSM_ENABLE_RTD3
    2.  Arg1 - Passed as parameter to this function
    3.  Arg2 - Reserved, pass zero.
    4.  Arg3 - Reserved, pass zero.

    --*/
{
    PACPI_METHOD_ARGUMENT           argument;
    WDF_MEMORY_DESCRIPTOR           inputDescriptor;
    WDFIOTARGET                     ioTargetHandle;
    ACPI_EVAL_OUTPUT_BUFFER         outputBuffer;
    WDF_MEMORY_DESCRIPTOR           outputDescriptor;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX paramtersBuffer;
    ULONG                           parametersBufferSize;
    ULONG_PTR                       sizeReturned;
    NTSTATUS                        status;
    WDFDEVICE                       wdfDevice;

    PAGED_CODE();

    TRY {

        paramtersBuffer = NULL;
        wdfDevice = ControllerData->WdfDevice;

        parametersBufferSize =
            sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
            (sizeof(GUID) - sizeof(ULONG)) +
            (sizeof(ACPI_METHOD_ARGUMENT) * 3);

        paramtersBuffer =
            (PACPI_EVAL_INPUT_BUFFER_COMPLEX)ExAllocatePoolWithTag(PagedPool,
                parametersBufferSize,
                DriverAllocationTag);
        if (paramtersBuffer == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LogError(ControllerData->RecorderLog, Controller, "Failed to parametersBuffer for _DSM - %!STATUS!", status);
            LEAVE;
        }

        RtlZeroMemory(paramtersBuffer, parametersBufferSize);
        paramtersBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
        paramtersBuffer->MethodNameAsUlong = (ULONG)'MSD_';
        paramtersBuffer->Size = parametersBufferSize;
        paramtersBuffer->ArgumentCount = 4;

        //
        // Argument 0: UUID.
        //
        argument = &paramtersBuffer->Argument[0];
        ACPI_METHOD_SET_ARGUMENT_BUFFER(argument, &GUID_DSM_ENABLE_RTD3, sizeof(GUID));

        //
        // Argument 1: Passed as parameter to this function
        //
        argument = ACPI_METHOD_NEXT_ARGUMENT(argument);
        ACPI_METHOD_SET_ARGUMENT_INTEGER(argument, Argument1);

        //
        // Argument 2: Reserved, pass zero.
        //
        argument = ACPI_METHOD_NEXT_ARGUMENT(argument);
        ACPI_METHOD_SET_ARGUMENT_INTEGER(argument, 0);

        //
        // Argument 3: Reserved, pass zero.
        //
        argument = ACPI_METHOD_NEXT_ARGUMENT(argument);
        ACPI_METHOD_SET_ARGUMENT_INTEGER(argument, 0);

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inputDescriptor,
            (PVOID)paramtersBuffer,
            parametersBufferSize);

        RtlZeroMemory(&outputBuffer, sizeof(ACPI_EVAL_OUTPUT_BUFFER));
        outputBuffer.Signature = ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE;

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&outputDescriptor,
            (PVOID)&outputBuffer,
            sizeof(ACPI_EVAL_OUTPUT_BUFFER));

        ioTargetHandle = WdfDeviceGetIoTarget(wdfDevice);

        status = WdfIoTargetSendIoctlSynchronously(ioTargetHandle,
            NULL,
            IOCTL_ACPI_EVAL_METHOD,
            &inputDescriptor,
            &outputDescriptor,
            NULL,
            &sizeReturned);

        if (!NT_SUCCESS(status)) {

            LogInfo(ControllerData->RecorderLog, Controller,
                "WdfDevice 0x%p, IOCTL_ACPI_EVAL_METHOD(_DSM) failed with %!STATUS!", wdfDevice, status);
            LEAVE;
        }

        if (outputBuffer.Signature != ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE) {
            LogInfo(ControllerData->RecorderLog, Controller,
                "WdfDevice 0x%p, IOCTL_ACPI_EVAL_METHOD(_DSM) output buffer is malformed", wdfDevice);
            status = STATUS_ACPI_INVALID_DATA;
            LEAVE;
        }

        LogInfo(ControllerData->RecorderLog, Controller,
            "WdfDevice 0x%p, IOCTL_ACPI_EVAL_METHOD(_DSM) success", wdfDevice);

    } FINALLY {

        if (paramtersBuffer) {
            ExFreePoolWithTag(paramtersBuffer, DriverAllocationTag);
        }
    }

    return;
}

NTSTATUS
Controller_LPEEnterExitCallback(
    LPCGUID SettingGuid,
    PVOID   Value,
    ULONG   ValueLength,
    PVOID   Context
    )
/*++

Routine Description:

    This is the callback for notification of Low Power Epoch entry and exit.
    Currently, it is registered if the xHCI controller is being shared with
    the USB kernel debug module. It ensures that a power reference is held
    on the controller when System is in S0, but dropped when System enters
    LPE, so as not to block DRIPS entry.

--*/
{
    PCONTROLLER_DATA    controllerData;
    BOOLEAN             notInLPE;
    NTSTATUS            status;

#define NOT_IN_LPE 0

    TRY {

        NT_ASSERT_ASSUME(Context != NULL);

        controllerData = (PCONTROLLER_DATA)Context;

        if ((Value == NULL) || (ValueLength != sizeof(ULONG))) {
            status = STATUS_INVALID_PARAMETER;
            LogError(controllerData->RecorderLog, Controller,
                     "WdfDevice 0x%p Invalid parameter supplied to Power Setting Callback",
                     controllerData->WdfDevice);
            LEAVE;
        }

        if (IsEqualGUID(SettingGuid, &GUID_LOW_POWER_EPOCH) == FALSE) {
            status = STATUS_INVALID_PARAMETER;
            LogError(controllerData->RecorderLog, Controller,
                     "WdfDevice 0x%p Received Power Setting Callback for unexpected GUID",
                     controllerData->WdfDevice);
            LEAVE;
        }

        notInLPE = ((*(PULONG)Value) == NOT_IN_LPE);

        if (notInLPE) {

            LogInfo(controllerData->RecorderLog, Controller,
                    "WdfDevice 0x%p LPE Exit", controllerData->WdfDevice);

            if (Controller_InUseByDebugger(controllerData)) {
                Controller_AcquireWdfPowerReferenceForDebugger(controllerData);
            }

        } else {

            LogInfo(controllerData->RecorderLog, Controller,
                    "WdfDevice 0x%p LPE Entry", controllerData->WdfDevice);

            if (Controller_InUseByDebugger(controllerData)) {
                Controller_ReleaseWdfPowerReferenceForDebugger(controllerData);
            }
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

BOOLEAN
Controller_InUseByDebugger(
    __in
        PCONTROLLER_DATA ControllerData
    )
/*++

Routine Description:

    Returns TRUE if kernel debug is currently enabled on this system,
    and this xHCI controller is being shared with the USB kernel debug
    module. Note: TRUE does not imply that there is currently an active
    USB debug connection. i.e. debug cable could be unplugged.

--*/
{
    PDEVICE_OBJECT pdo;

    pdo = WdfDeviceWdmGetPhysicalDevice(ControllerData->WdfDevice);

    return ((pdo->Flags & DO_DEVICE_USED_BY_DEBUGGER) != 0);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_AcquireWdfPowerReferenceForDebugger(
    __in
        PCONTROLLER_DATA ControllerData
    )
{
    NTSTATUS status;

    TRY {

        if (ControllerData->WdfPowerReferenceForDebuggerAcquired) {
            LEAVE;
        }

        status = WdfDeviceStopIdleWithTag(ControllerData->WdfDevice,
                                          FALSE,
                                          TAG(Controller_AcquireWdfPowerReferenceForDebugger));

        if (!NT_SUCCESS(status)) {
            LogError(ControllerData->RecorderLog, Controller,
                     "WdfDevice 0x%p WdfDeviceStopIdleWithTag Failed %!STATUS!",
                     ControllerData->WdfDevice, status);
            LEAVE;
        }

        ControllerData->WdfPowerReferenceForDebuggerAcquired = TRUE;

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_WDF_POWER_REFERENCE_FOR_DEBUGGER(
            NULL,
            ControllerData,
            ControllerData->WdfPowerReferenceForDebuggerAcquired);

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ReleaseWdfPowerReferenceForDebugger(
    __in
        PCONTROLLER_DATA ControllerData
    )
{
    if (ControllerData->WdfPowerReferenceForDebuggerAcquired) {

        WdfDeviceResumeIdleWithTag(ControllerData->WdfDevice,
                                   TAG(Controller_AcquireWdfPowerReferenceForDebugger));

        ControllerData->WdfPowerReferenceForDebuggerAcquired = FALSE;

        EventWriteUSBXHCI_ETW_EVENT_CONTROLLER_WDF_POWER_REFERENCE_FOR_DEBUGGER(
            NULL,
            ControllerData,
            ControllerData->WdfPowerReferenceForDebuggerAcquired);
    }
}
