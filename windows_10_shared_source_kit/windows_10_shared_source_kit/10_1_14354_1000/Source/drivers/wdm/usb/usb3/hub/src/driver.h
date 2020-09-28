/*++
Copyright (c) Microsoft Corporation

Module Name:

    driver.h

Abstract:

    USB HUB Driver

--*/

#pragma once

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_OBJECT_CONTEXT_CLEANUP DriverCleanup;


typedef union _HUB_GLOBAL_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG       HubGlobalKeyName:1;
        ULONG       DisableSelectiveSuspendUI:1;
        ULONG       MsOsDescriptorMode:1;
        ULONG       EnableDiagnosticMode:1;
        ULONG       DisableUxdSupport:1;
        ULONG       EnableExtendedValidation:1;
        ULONG       WakeOnConnectUI:1;
        ULONG       DisableOnSoftRemove:1;
        ULONG       UxdGlobalDeleteShutdown:1;
        ULONG       UxdGlobalDeleteOnReload:1;
        ULONG       UxdGlobalDeleteDisconnect:1;
        ULONG       UxdGlobalEnable:1;
        ULONG       EtwEnabled:1;
        ULONG       EnableReservedValidation:1;
        ULONG       EnableStrictestValidation:1;
        ULONG       EnableUsb20HardwareLpm:1;
        ULONG       PreventDebounceTimeForSuperSpeedDevices:1;
        ULONG       DisableFastEnumeration:1;
        ULONG       EnableUsbLtm:1;
    };
} HUB_GLOBAL_FLAGS, *PHUB_GLOBAL_FLAGS;

typedef enum _MS_OS_MODE {
    MsOsModeOneTimeQuery = 0,
    MsOsModeForceQuery = 1,
    MsOsModeDisableQuery = 2
} MS_OS_MODE, *PMS_OS_MODE;

typedef struct _HUB_GLOBALS {
    ULONG                           Reserved;
    HUB_GLOBAL_FLAGS                HubGlobalFlags;
    MS_OS_MODE                      MsOsMode;
    LIST_ENTRY                      HubFdoListHead;
    WDFWAITLOCK                     HubFdoListLock;
    LIST_ENTRY                      ConnectorMapHead;
    WDFWAITLOCK                     ConnectorMapLock;
    RECORDER_LOG                    IfrLog;
    UCHAR                           Usb20HardwareLpmTimeout;
    WDFWMIINSTANCE                  SurpriseRemovalWmiNotificationHandle;
    LONG                            AllowDebounceTimeForSuperSpeedDevices;
    BOOLEAN                         SleepStudyEnabled;
} HUB_GLOBALS, *PHUB_GLOBALS;


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(HUB_GLOBALS, GetHubGlobals)

__declspec(dllexport)
__declspec(selectany)

ULONG Microsoft_USBD_Compat_Version = 0x0110;


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBDRIVER_EtwRundownHub (
    __in
        LPCGUID                      ActivityId,
    __in
        PHUB_FDO_CONTEXT             HubFdoContext
    );


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBDRIVER_EtwRundownUsbDevice (
    __in
        LPCGUID                     ActivityId,
    __in
        WDFDEVICE                   UsbDevice
    );


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
    );

#define MCGEN_PRIVATE_ENABLE_CALLBACK_V2 HUBDRIVER_EtwEnableCallback


typedef  
NTSTATUS  
(*PFN_KSE_QUERY_DEVICE_FLAGS)(  
    __in
        LPCWSTR DeviceKey,  
    __in
        LPCWSTR DeviceClass,  
    __out
        PULONG64 DeviceFlags  
    );  
  
PFN_KSE_QUERY_DEVICE_FLAGS      g_KseQueryDeviceFlags;  

typedef
NTSTATUS  
(*PFN_IO_GET_ACTIVITY_ID_IRP) (
    __in
        PIRP    Irp,
    __out
        LPGUID  Guid
    );  

PFN_IO_GET_ACTIVITY_ID_IRP      g_IoGetActivityIdIrp;

typedef
NTSTATUS  
(*PFN_IO_SET_ACTIVITY_ID_IRP) (
    __inout
        PIRP    Irp,
    __in_opt
        LPGUID  Guid
    );  

PFN_IO_SET_ACTIVITY_ID_IRP      g_IoSetActivityIdIrp;

typedef
VOID
(*PFN_IO_FREE_ACTIVITY_ID_IRP) (
    __inout
        PIRP    Irp
    );  

PFN_IO_FREE_ACTIVITY_ID_IRP      g_IoFreeActivityIdIrp;
