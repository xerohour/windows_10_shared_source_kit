//
//    Copyright (C) Microsoft.  All rights reserved.
//
#define ROOTHUB_ENDPOINT_NUMBER_OF_FORWARD_PROGRESS_REQUESTS 1

#define ROOTHUB_LOCATION (L"USBROOT(0)")
#define LOCATION_INTERFACE_VERSION 1

#define ROOTHUB_INSTANCE_ID_LENGTH 4

#define ROOTHUB_DEVICE_ID L"USB\\ROOT_HUB30\0"

#define ROOTHUB_PCI_HARDWARE_ID_1 L"USB\\ROOT_HUB30&VIDnnnn&PIDnnnn&REVnnnn"
#define ROOTHUB_PCI_HARDWARE_ID_2 L"USB\\ROOT_HUB30&VIDnnnn&PIDnnnn"
#define ROOTHUB_PCI_HARDWARE_ID_3 L"USB\\ROOT_HUB30"

#define ROOTHUB_PCI_HARDWARE_ID_1_FORMAT_STRING L"USB\\ROOT_HUB30&VID%04X&PID%04X&REV%04X"
#define ROOTHUB_PCI_HARDWARE_ID_2_FORMAT_STRING L"USB\\ROOT_HUB30&VID%04X&PID%04X"
#define ROOTHUB_PCI_HARDWARE_ID_3_FORMAT_STRING L"USB\\ROOT_HUB30"

//
// For now instead of \0, using a # so that Rtl String APIs do not get confused. 
// Once the string is formed, we would replace # with \0
//
#define ROOTHUB_PCI_HARDWARE_ID_FORMAT_STRING \
    ROOTHUB_PCI_HARDWARE_ID_1_FORMAT_STRING \
    L"#" \
    ROOTHUB_PCI_HARDWARE_ID_2_FORMAT_STRING \
    L"#" \
    ROOTHUB_PCI_HARDWARE_ID_3_FORMAT_STRING \
    L"#"

#define ROOTHUB_PCI_HARDWARE_ID \
    ROOTHUB_PCI_HARDWARE_ID_1 \
    L"#" \
    ROOTHUB_PCI_HARDWARE_ID_2 \
    L"#" \
    ROOTHUB_PCI_HARDWARE_ID_3 \
    L"#"

#define ROOTHUB_PCI_HARDWARE_ID_NULL_LOCATION_1 (ARRAY_SIZE(ROOTHUB_PCI_HARDWARE_ID_1) - 1)
#define ROOTHUB_PCI_HARDWARE_ID_NULL_LOCATION_2 (ROOTHUB_PCI_HARDWARE_ID_NULL_LOCATION_1 + ARRAY_SIZE(ROOTHUB_PCI_HARDWARE_ID_2))
#define ROOTHUB_PCI_HARDWARE_ID_NULL_LOCATION_3 (ROOTHUB_PCI_HARDWARE_ID_NULL_LOCATION_2 + ARRAY_SIZE(ROOTHUB_PCI_HARDWARE_ID_3))

#define ROOTHUB_ACPI_HARDWARE_ID_1 L"USB\\ROOT_HUB30&VIDssss&PIDssss&REVssss"
#define ROOTHUB_ACPI_HARDWARE_ID_2 L"USB\\ROOT_HUB30&VIDssss&PIDssss"
#define ROOTHUB_ACPI_HARDWARE_ID_3 L"USB\\ROOT_HUB30"

#define ROOTHUB_ACPI_HARDWARE_ID_1_FORMAT_STRING L"USB\\ROOT_HUB30&VID%S&PID%S&REV%S"
#define ROOTHUB_ACPI_HARDWARE_ID_2_FORMAT_STRING L"USB\\ROOT_HUB30&VID%S&PID%S"
#define ROOTHUB_ACPI_HARDWARE_ID_3_FORMAT_STRING L"USB\\ROOT_HUB30"

#define ROOTHUB_ACPI_HARDWARE_ID_FORMAT_STRING \
    ROOTHUB_ACPI_HARDWARE_ID_1_FORMAT_STRING \
    L"#" \
    ROOTHUB_ACPI_HARDWARE_ID_2_FORMAT_STRING \
    L"#" \
    ROOTHUB_ACPI_HARDWARE_ID_3_FORMAT_STRING \
    L"#"

#define ROOTHUB_ACPI_HARDWARE_ID \
    ROOTHUB_ACPI_HARDWARE_ID_1 \
    L"#" \
    ROOTHUB_ACPI_HARDWARE_ID_2 \
    L"#" \
    ROOTHUB_ACPI_HARDWARE_ID_3 \
    L"#"

#define ROOTHUB_ACPI_HARDWARE_ID_NULL_LOCATION_1 (ARRAY_SIZE(ROOTHUB_ACPI_HARDWARE_ID_1) - 1)
#define ROOTHUB_ACPI_HARDWARE_ID_NULL_LOCATION_2 (ROOTHUB_ACPI_HARDWARE_ID_NULL_LOCATION_1 + ARRAY_SIZE(ROOTHUB_ACPI_HARDWARE_ID_2))
#define ROOTHUB_ACPI_HARDWARE_ID_NULL_LOCATION_3 (ROOTHUB_ACPI_HARDWARE_ID_NULL_LOCATION_2 + ARRAY_SIZE(ROOTHUB_ACPI_HARDWARE_ID_3))

typedef struct _ROOTHUB_INTERRUPT_QUEUE_CONTEXT *PROOTHUB_INTERRUPT_QUEUE_CONTEXT;

EVT_WDF_OBJECT_CONTEXT_CLEANUP RootHub_EvtCleanupCallback;

typedef struct _UCXROOTHUB_PRIVCONTEXT {
    UCX_SIG                           Sig;
    UCXROOTHUB                        UcxRootHub;
    UCX_ROOTHUB_CONFIG                Config;
    PUCXCONTROLLER_PRIVCONTEXT        UcxControllerContext;
    PUCXUSBDEVICE_PRIVCONTEXT         UcxUsbDeviceContextOfUcxRootHub;
    UCXENDPOINT                       UcxInterruptEndpoint;
    PROOTHUB_INTERRUPT_QUEUE_CONTEXT  RootHubInterruptQueueContext;
    ULONG                             NumberOf20Ports;
    ULONG                             NumberOf30Ports;
    WDF_DEVICE_PNP_CAPABILITIES       PnpCaps;
    WDF_DEVICE_POWER_CAPABILITIES     PowerCaps;
    KSPIN_LOCK                        RHPdoInfoLock;
    WDFDEVICE                         RootHubPdo;
    PIRP                              AsyncResetIrpFromHub;
    //
    // Protects queue state changes in control (DefaultQueue) and interrupt queues.
    //
    WDFQUEUE                          DefaultQueue;
    //
    // Following information is needed for Compatibilty with 2.0 stack.
    //
    BOOLEAN                           RHPdoStarted;
    WDFSTRING                         RHPdoSymbolicName;

    //
    // Track the last system sleep state for Compat
    //
    SYSTEM_POWER_STATE                LastSystemSleepState;
    //
    // We need to track the system power action ourselves
    // rather than using the WdfDeviceGetSystemPowerAction
    // API is because the information returned by this 
    // API is not reliable in scenarios where the reason
    // for the power transition of our device is different
    // from the on-going system action in progress. e.g. if 
    // while the system resume is going on, and our stack 
    // after having resumed due to system resume is now going 
    // into selective suspend, calling WdfDeviceGetSystemPowerAction 
    // still reflects that system is resuming from suspended 
    // state. Please refer to Windowd 8 bug 984884
    //
    POWER_ACTION                    SystemPowerAction;

    HANDLE                          SleepStudyHandle;
} UCXROOTHUB_PRIVCONTEXT, *PUCXROOTHUB_PRIVCONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(UCXROOTHUB_PRIVCONTEXT, 
                                   UcxRootHubGetContext);
 
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL  RootHub_EvtInterruptTxHandler;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL  RootHub_EvtControlTxHandler;

typedef struct _ROOTHUB_CONTROL_QUEUE_CONTEXT {
    UCX_SIG                 Sig;
    UCXROOTHUB              UcxRootHub;
} ROOTHUB_CONTROL_QUEUE_CONTEXT, *PROOTHUB_CONTROL_QUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(ROOTHUB_CONTROL_QUEUE_CONTEXT, 
                                   RootHubControlQueueGetContext);

typedef struct _ROOTHUB_INTERRUPT_QUEUE_CONTEXT {
    UCX_SIG                 Sig;
    WDFQUEUE                Queue;
    UCXROOTHUB              UcxRootHub;    
    KSPIN_LOCK              PortChangeLock;
    USHORT                  PortChangeGenCount;
    USHORT                  PortChangeGenCountLastProcessed;
    KEVENT                  NoPendingIndicateWakeStatus;
    BOOLEAN                 WasLastTransferCancelled;
    BOOLEAN                 IndicateWakeStatusEnabled;
    WDFREQUEST              PendingRHIntrTx;
} ROOTHUB_INTERRUPT_QUEUE_CONTEXT, *PROOTHUB_INTERRUPT_QUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(ROOTHUB_INTERRUPT_QUEUE_CONTEXT, 
                                   RootHubInterruptQueueGetContext);

#define UCXROOTHUB_MAX_NUMBER_OF_PORTS 255

typedef struct _RHPDO_CONTEXT {
    UCX_SIG                      Sig;
    PUCXROOTHUB_PRIVCONTEXT      UcxRootHubContext;
    PUCXCONTROLLER_PRIVCONTEXT   UcxControllerContext;
} RHPDO_CONTEXT, *PRHPDO_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(RHPDO_CONTEXT, RHPdoGetContext);

//
// Functionality to manage UCXROOTHUB as UCXUSBDEVICE
//
_Must_inspect_result_
NTSTATUS
RootHub_AddUsbDeviceContext(
    __in 
        UCXCONTROLLER      UcxController,
    __in 
        UCXROOTHUB         UcxRootHub
    );

FORCEINLINE
UCXUSBDEVICE
UCXROOTHUB_TO_UCXUSBDEVICE(
    __in
        UCXROOTHUB UcxRootHub
    )
{
    return (UCXUSBDEVICE)(WDFOBJECT)UcxRootHub;
}

FORCEINLINE
UCXROOTHUB
UCXUSBDEVICE_TO_UCXROOTHUB(
    __in
        UCXUSBDEVICE UcxUsbDevice
    )
{
    return (UCXROOTHUB)(WDFOBJECT)UcxUsbDevice;
}

EVT_WDFDEVICE_WDM_IRP_PREPROCESS RootHub_Pdo_EvtInternalDeviceControlIrpPreprocessCallback;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS RootHub_Pdo_EvtQueryInterfaceIrpPreprocessCallback;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS RootHub_Pdo_EvtSetPowerIrpPreprocessCallback;

#ifndef WDFEXT_SUPPORT
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL RootHub_Pdo_EvtIoInternalDeviceControl;
#endif

EVT_WDF_DEVICE_D0_ENTRY            RootHub_Pdo_EvtD0Entry;
EVT_WDF_DEVICE_D0_EXIT             RootHub_Pdo_EvtD0Exit;
EVT_WDF_DEVICE_PREPARE_HARDWARE    RootHub_Pdo_EvtPrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE    RootHub_Pdo_EvtReleaseHardware;
EVT_WDF_DEVICE_ENABLE_WAKE_AT_BUS  RootHub_Pdo_EvtDeviceEnableWakeAtBus;
EVT_WDF_DEVICE_DISABLE_WAKE_AT_BUS RootHub_Pdo_EvtDeviceDisableWakeAtBus;


EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST  RootHub_EvtUSBDInterfaceV1Request;
EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST  RootHub_Evt20USBDIQueryInterfaceRequest;
EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST  RootHub_EvtHubControllerStackQueryInterfaceRequest;
EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST  RootHub_EvtHubParentQueryInterfaceRequest;

_Must_inspect_result_
NTSTATUS
RootHub_Pdo_QueryInterfaceAdd(
    __in 
        WDFDEVICE          RHPdo,
    __in 
        UCXROOTHUB         UcxRootHub
);

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
RootHub_WasResetOnResume(
    __in 
        PVOID             Context
    );

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
RootHub_WasDeviceProgrammingLostOnResume(
    __in 
        PVOID             Context
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
RootHub_GetHubSymbolicLinkName(
    __in 
        PVOID             Context,
    __in 
        PUNICODE_STRING  HubSymbolicLinkName
    );

VOID
RootHub_StartFailingRootHubIO(
    PUCXROOTHUB_PRIVCONTEXT           UcxRootHubContext
    );

VOID
RootHub_ResumeRootHubIO(
    PUCXROOTHUB_PRIVCONTEXT           UcxRootHubContext
    );

VOID
RootHub_CompleteAsyncReset(
    __in
        PUCXROOTHUB_PRIVCONTEXT    UcxRootHubContext,
    __in
        NTSTATUS                   Status
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
RootHub_USBDInterfaceV1Unregister(
    __in
        USBDI_HANDLE         USBDInterfaceHandle
);

FORCEINLINE
NTSTATUS
RootHub_EnableForwardProgress(
    __in 
        UCXROOTHUB                                UcxRootHub,
    __in
        PUSB_FORWARD_PROGRESS_INFO                ForwardProgressInfo
    )
{
    UNREFERENCED_PARAMETER(UcxRootHub);
    UNREFERENCED_PARAMETER(ForwardProgressInfo);

    //
    // RootHub already pre-allocates a WdfRequest each for its Control / Interrupt pipe.
    //
    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
RootHub_PortChanged(
    __in 
        PUCXROOTHUB_PRIVCONTEXT    UcxRootHubContext
   );

