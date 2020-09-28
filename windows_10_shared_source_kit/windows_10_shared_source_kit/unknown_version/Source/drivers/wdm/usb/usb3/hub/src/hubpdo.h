/*++

Copyright (c) Microsoft Corporation

Module Name:

    hubpdo.h

Abstract:

    Contains definitions for the HUB's child PDOs

Author:



Environment:

    Kernel mode only.

--*/
#pragma once

#define USBHUB3_WDF_TAG     0x77334855  //"UH3w"
#define USBHUB3_HUB_TAG     0x68334855  //"UH3h"
#define USBHUB3_DEVICE_TAG  0x64334855  //"UH3d"
#define USBHUB3_PORT_TAG    0x70334855  //"UH3p"

#define LOCATION_INTERFACE_DEVICE_STRING_COUNT (sizeof "USB(XXX)" + 1)  
#define LOCATION_INTERFACE_VERSION 1  

/*
typedef struct _USBD_CONFIG_HANDLE {
    ULONG Sig;
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor;
    LIST_ENTRY InterfaceHandleList;
} USBD_CONFIG_HANDLE, *PUSBD_CONFIG_HANDLE;
*/
typedef union _PDO_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       InD0:1;  
        ULONG       ArmedForWake:1;
        ULONG       RequiresArmingForWake:1;
        ULONG       WaitWakePending:1;
        ULONG       SupportsFunctionSuspend:1;
        ULONG       InBootPath:1;
        ULONG       ResetOnLastResume:1;
        ULONG       ProgrammingLostOnLastReset:1;
        ULONG       PowerLostOnLastReset:1;
        ULONG       DeviceGone:1;
        ULONG       QueryStopped:1;
        ULONG       QueryRemoved:1;
        ULONG       IdleStateMachineInitialized:1;
        ULONG       ReportPortAsDisabled:1;
        ULONG       WakeNotificationSent:1;
    };
} PDO_FLAGS, *PPDO_FLAGS;

C_ASSERT(sizeof(PDO_FLAGS) == sizeof(ULONG));

// {FB093825-BF59-4fe6-8B6F-AFD6714C69E5}
DEFINE_GUID(GUID_POWER_USB_U1_ENABLE_FOR_DEVICES, 
0xfb093825, 0xbf59, 0x4fe6, 0x8b, 0x6f, 0xaf, 0xd6, 0x71, 0x4c, 0x69, 0xe5);

// {903281EA-C859-47a9-8901-7A118C7BF5A7}
DEFINE_GUID(GUID_POWER_USB_U2_ENABLE_FOR_DEVICES, 
0x903281ea, 0xc859, 0x47a9, 0x89, 0x1, 0x7a, 0x11, 0x8c, 0x7b, 0xf5, 0xa7);

// {C5F91E9D-0CED-43f2-905E-8F9D38E93339}
DEFINE_GUID(GUID_POWER_USB_U1_ENABLE_FOR_HUBS, 
0xc5f91e9d, 0xced, 0x43f2, 0x90, 0x5e, 0x8f, 0x9d, 0x38, 0xe9, 0x33, 0x39);

// {34751A37-E607-452a-B9F7-D020FC5CD0DC}
DEFINE_GUID(GUID_POWER_USB_U2_ENABLE_FOR_HUBS, 
0x34751a37, 0xe607, 0x452a, 0xb9, 0xf7, 0xd0, 0x20, 0xfc, 0x5c, 0xd0, 0xdc);

// {A7B5D35E-C12C-4f36-A44F-00628F900388}
DEFINE_GUID(GUID_POWER_USB_U1_TIMEOUT_FOR_DEVICES, 
0xa7b5d35e, 0xc12c, 0x4f36, 0xa4, 0x4f, 0x0, 0x62, 0x8f, 0x90, 0x3, 0x88);

// {300C0440-EE77-4c1b-AF99-3A33F8A9CA5D}
DEFINE_GUID(GUID_POWER_USB_U2_TIMEOUT_FOR_DEVICES, 
0x300c0440, 0xee77, 0x4c1b, 0xaf, 0x99, 0x3a, 0x33, 0xf8, 0xa9, 0xca, 0x5d);

// {329423DD-222F-4b04-815B-08C1A6D89C8E}
DEFINE_GUID(GUID_POWER_USB_U1_TIMEOUT_FOR_HUBS, 
0x329423dd, 0x222f, 0x4b04, 0x81, 0x5b, 0x8, 0xc1, 0xa6, 0xd8, 0x9c, 0x8e);

// {BBFBADAF-7520-491a-910D-4A2CEE72203F}
DEFINE_GUID(GUID_POWER_USB_U2_TIMEOUT_FOR_HUBS, 
0xbbfbadaf, 0x7520, 0x491a, 0x91, 0xd, 0x4a, 0x2c, 0xee, 0x72, 0x20, 0x3f);

// {D4E98F31-5FFE-4ce1-BE31-1B38B384C009}
DEFINE_GUID(GUID_POWER_USB_3_LINK_POWER_MANAGEMENT_POLICY, 
0xd4e98f31, 0x5ffe, 0x4ce1, 0xbe, 0x31, 0x1b, 0x38, 0xb3, 0x84, 0xc0, 0x9);

#define LPM_SETTINGS_COUNT      5

__drv_functionClass(POWER_SETTING_CALLBACK)
__drv_sameIRQL
NTSTATUS
HUBPDO_PowerSettingCallback (  
    __in 
        LPCGUID             SettingGuid,
    __in_bcount(ValueLength) 
        PULONG              NewValue,
    __in 
        ULONG               ValueLength,
    __inout_opt 
        PDEVICE_CONTEXT     DeviceContext
   );


typedef struct _HUB_PDO_CONTEXT {
    BOOLEAN                         FailIo;

    LONG                            CyclePortInProgress;
    
    PDEVICE_OBJECT                  TargetDeviceObject;

    PHUB_FDO_CONTEXT                HubFdoContext;

    PDEVICE_CONTEXT                 AssociatedDSM;

    PDO_FLAGS                       PdoFlags;

    ULONG                           LastPowerRequested;

    WDFMEMORY                       ConfigurationHandleMemory;

    USHORT                          PortNumber;
    
    PUSB_START_FAILDATA             FailData;

    PHUB_FDO_CONTEXT                ChildHubFdoContext;

    ISM_CONTEXT                     IdleIrpContext;
    
    WDFREQUEST                      WakeNotification;

    PVOID                           LpmPowerSettingsHandles[LPM_SETTINGS_COUNT];

//  WDFIOTARGET                     DummyParentTarget;

    D3COLD_SUPPORT_INTERFACE        D3ColdFilter;

    WDF_POWER_DEVICE_STATE          CurrentWdfPowerDeviceState;

    ULONG                           USBDClientContractVersion;

    HANDLE                          SleepStudyHandle;
} HUB_PDO_CONTEXT, *PHUB_PDO_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(HUB_PDO_CONTEXT, GetHubPdoContext)

typedef struct _WAKE_NOTIFICATION_REQUEST_CONTEXT {

    REQUEST_REMOTE_WAKE_NOTIFICATION            UcxRequestContext;
    KEVENT                                      CompletionEvent;
    
} WAKE_NOTIFICATION_REQUEST_CONTEXT, *PWAKE_NOTIFICATION_REQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WAKE_NOTIFICATION_REQUEST_CONTEXT, GetWakeNotificationRequestContext)

typedef struct _IRP_COMPLETE_WORKER_CONTEXT {

    PIRP                            Irp;
    
} IRP_COMPLETE_WORKER_CONTEXT, *PIRP_COMPLETE_WORKER_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(IRP_COMPLETE_WORKER_CONTEXT, GetCompleteIrpWorkItemContext)

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL     HUBPDO_EvtIoInternalDeviceControl;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL              HUBPDO_EvtIoDeviceControl;
EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT             HUBPDO_EvtDeviceSelfManagedIoInit;
EVT_WDF_DEVICE_SELF_MANAGED_IO_SUSPEND          HUBPDO_EvtDeviceSelfManagedIoSuspend;
EVT_WDF_DEVICE_SELF_MANAGED_IO_RESTART          HUBPDO_EvtDeviceSelfManagedIoRestart;
EVT_WDF_DEVICE_D0_ENTRY                         HUBPDO_EvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT                          HUBPDO_EvtDeviceD0Exit;
EVT_WDF_DEVICE_PREPARE_HARDWARE                 HUBPDO_EvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE                 HUBPDO_EvtDeviceReleaseHardware;
EVT_WDF_DEVICE_SURPRISE_REMOVAL                 HUBPDO_EvtDeviceSurpriseRemoval;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS                HUBPDO_EvtDeviceWdmIrpPreprocess;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS                HUBPDO_EvtDeviceWdmIrpQueryInterfacePreprocess;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS                HUBPDO_EvtDeviceWdmIrpQueryDeviceTextPreprocess;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS                HUBPDO_EvtDeviceWdmIrpQueryIdPreprocess;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS                HUBPDO_EvtDeviceWdmIrpQueryPnPPreprocess;
EVT_WDF_DEVICE_ENABLE_WAKE_AT_BUS               HUBPDO_EvtDeviceEnableWakeAtBus;
EVT_WDF_DEVICE_DISABLE_WAKE_AT_BUS              HUBPDO_EvtDeviceDisableWakeAtBus;
EVT_WDF_DEVICE_USAGE_NOTIFICATION_EX            HUBPDO_EvtDeviceUsageNotificationEx;
EVT_WDF_DEVICE_QUERY_STOP                       HUBPDO_EvtDeviceQueryStop;
EVT_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY      HUBPDO_EvtDeviceResourceRequirementsQuery;
EVT_WDF_DEVICE_REPORTED_MISSING                 HUBPDO_EvtDeviceReportedMissing;
EVT_WDF_TIMER                                   HUBPDO_EvtOutOfBandwidthTimer;
EVT_WDF_DEVICE_QUERY_REMOVE                     HUBPDO_EvtDeviceQueryRemove;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS                HUBPDO_EvtDeviceWdmIrpPnPPowerPreprocess;
//EVT_WDF_OBJECT_CONTEXT_DESTROY                HUBPDO_EvtDeviceDestroy;
EVT_WDF_OBJECT_CONTEXT_CLEANUP                  HUBPDO_EvtDeviceCleanup;
EVT_WDF_TIMER                                   HUBPDO_EvtBootResetTimer;
// EVT_WDF_IO_TARGET_REMOVE_COMPLETE            HUBPDO_EvtIoTargetRemoveComplete;
SET_D3COLD_SUPPORT                              HUBPDO_D3ColdSupportInterfaceSetD3ColdSupport;
GET_IDLE_WAKE_INFO                              HUBPDO_D3ColdSupportInterfaceGetIdleWakeInfo;
GET_D3COLD_CAPABILITY                           HUBPDO_D3ColdSupportInterfaceGetD3ColdCapability;
GET_D3COLD_CAPABILITY                           HUBPDO_D3ColdSupportInterfaceGetD3ColdBusDriverSupport;
GET_D3COLD_LAST_TRANSITION_STATUS               HUBPDO_D3ColdSupportInterfaceGetLastTransitionStatus;
IO_COMPLETION_ROUTINE                           HUBPDO_WdmPnpPowerIrpCompletionRoutine;
IO_COMPLETION_ROUTINE                           HUBPDO_WdmPnpPowerIrpCompletionRoutineForAsynchronousCompletion;
EVT_WDF_WORKITEM                                HUBPDO_EvtCompleteIrpWorkItem;

//
// It is important that we overwrite all the methods of the D3 cold
// interface. The following assert will help us catch any additions
// to the D3 cold interface in future
//
C_ASSERT(sizeof(D3COLD_SUPPORT_INTERFACE) == 
         (FIELD_OFFSET(D3COLD_SUPPORT_INTERFACE,GetLastTransitionStatus) + 
          sizeof(PGET_D3COLD_LAST_TRANSITION_STATUS)));


__drv_functionClass(EVT_WDF_DEVICE_PROCESS_QUERY_INTERFACE_REQUEST)
NTSTATUS
HUBPDO_EvtDeviceProcessQueryInterfaceRequest (
    __in
        WDFDEVICE   Device,
    __in     
        LPGUID      InterfaceType,
  __inout  
        PINTERFACE  ExposedInterface,
  __inout  
        PVOID       ExposedInterfaceSpecificData
    );

VOID
HUBPDO_D3ColdSupportInterfaceReference (
    _In_reads_opt_(_Inexpressible_("varies")) 
        PVOID           Context
    );

VOID
HUBPDO_D3ColdSupportInterfaceDereference (
    _In_reads_opt_(_Inexpressible_("varies")) 
        PVOID           Context
    );



__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBPDO_MakePdoName(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PWDFDEVICE_INIT     DeviceInit,
    __in
        ULONG               Index
    );

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBPDO_AssignPDOIds(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PWDFDEVICE_INIT     DeviceInit
    );


__drv_maxIRQL(PASSIVE_LEVEL)
DSM_EVENT
HUBPDO_CreatePdoInternal(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        BOOLEAN             KnownDevice
    );


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBPDO_GetHubSymbolicLinkName(
    __in 
        PHUB_PDO_CONTEXT PdoContext,
    __in 
        PUNICODE_STRING  HubSymbolicLinkName
    );


BOOLEAN
HUBPDO_IsLanguageSupported(
    PDEVICE_CONTEXT     DeviceContext,
    LANGID              LanguageId
    );


__drv_maxIRQL(DISPATCH_LEVEL)
USBD_STATUS
HUBPDO_GetUSBDErrorFromNTStatus(
    __in
        NTSTATUS            Status
    );


__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
HUBPDO_GetNTStatusFromUSBDError(
    __in
        USBD_STATUS         UsbdStatus
    );


VOID
HUBPDO_NoPingResponse(
    __in
        USB_HUB_CONTEXT    HubContext,
    __in
        USB_DEVICE_CONTEXT UsbDeviceContext
    );

VOID
HUBPDO_DispatchWdmPnpPowerIrpSynchronously(
    __in
        WDFDEVICE           Device,
    __in
        PIRP                Irp
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBPDO_PublishBillboardDetails(
    __in
        PHUB_PDO_CONTEXT PdoContext
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBPDO_BillboardCleanup(
    __in
        PDEVICE_CONTEXT DeviceContext
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBPDO_PublishDualRoleFeaturesProperty(
    __in
        PHUB_PDO_CONTEXT PdoContext
    );