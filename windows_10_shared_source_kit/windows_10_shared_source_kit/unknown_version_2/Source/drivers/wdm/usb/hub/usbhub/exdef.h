/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    exdef.h

Abstract:

    Hub execption codes and structures.

    Hub exceptions occur as a result of hardware error conditions on the hub or
    one of its ports.  Some of these conditions have defined behaviors such as
    over-current and some do not.  The purpose of this code in the hub driver is
    to have a single standard hardware error handling mechanism.

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#ifndef   __EXDEF_H__
#define   __EXDEF_H__

typedef enum _HUB_EXCEPTION_TYPE {
    PnpErr_ = 0,
    PowerErr,
    HubErr_,
    UsbErr_,
    SsErr_,
    GenErr_    
} HUB_EXCEPTION_TYPE;

typedef enum _HUB_EXCEPTION_CLASS {
   HubExceptionInit = 0,

   HubErr_TransactionFailure_Reset = 1,
   CreatePdoFailure_Popup = 2,
   CreateDeviceFailure_Popup = 3,
   CreateDeviceFailure = 4,
   CreateDeviceFailure_NoHubHandle = 5,
   DeviceOvercurrent_Popup = 6,
   InterruptPipeFailure_Reset = 7,
   HubErr_GetPortStatusFailed = 8,
   HubErr_BadControlData = 9,
   HubErr_NoChangeBit = 10,
   HubErr_PortResetFailed = 11,
   HubErr_SetPortPowerFailed = 12,
   HubErr_GetConfigDescriptorFailed = 13,
   HubErr_GetHubDescriptorFailed = 14,
   HubErr_GetHubDescriptorFailed_NoPorts = 15,
   HubErr_GetDescriptorFailed = 16,
   HubErr_GetStatusFailed = 17,
   HubErr_HubInterfaceNotFound = 18,
   HubErr_ResetPortFailed = 19,
   HubErr_PortDisableFailed_Reset = 20,
   HubErr_SetPortFeatureFailed = 21,
   HubErr_PortResetTtFailed = 22,
   HubErr_GetPortStatusInitFailed = 23,
   HubErr_QueueChangeInitFailed = 24,
   HubErr_BadBusIf = 25,
   HubErr_NoHubBusIf = 26,
   HubErr_NoUsbdiBusIf = 27,
   Reset1Failure_Popup = 28,
   Reset2Failure_Popup = 29,
   InitializeDeviceFailure_Popup = 30,
   InitializeDeviceFailure = 31,
   SetupDeviceFailure_Popup = 32,
   InvalidDeviceConfigDescriptor = 33,
   HubErr_InvalidHubConfigDescriptor = 34,
   NotEnoughPower_Popup = 35,
   BadConfigDescriptor = 36,
   BadSerialNumberString = 37,
   BadUsbString = 38,
   BadSerialNumberChars = 39,
   DuplicateSerialNumber = 40,
   UsbErr_InsufficientBandwidth_Popup = 41,
   LegacyDevice_Popup = 42,
   PowerErr_WarmStartFailed_Reset = 43,
   PowerErr_ColdStartFailed_Reset = 44,
   PowerErr_HubFailAfterSuspend = 45,
   PowerErr_RootHubFailAfterSuspend = 46,
   PowerErr_SetPortResumeFailed = 47,
   PowerErr_GetPortStatusFailed = 48,
   PowerErr_FailedWakeDisable = 49,
   PowerErr_FailedWakeEnable = 50,
   HubErr_SuspendPortFailed = 51,
   PowerErr_FlushPortChangeFailed = 52,
   PowerErr_PowerPortsFailed = 53,
   PowerErr_PostResumeResetFailed = 54,
   DebounceFailure = 55,
   DropDevice = 56,
   HubErr_StartDevFailedLower = 57,
   FailedProductIdString = 58,
   NullSerialNumberString = 59,
   HubErr_PowerBit_Dropped = 60,
   HubErr_Reset1BadEnable = 61,
   HubErr_HardError_Reset = 62,
   BadUsbString_LangId = 63,
   HubErr_InvalidHubDeviceDescriptor = 64,
   HubErr_FinishStartFailed = 65,
   UsbdRestoreDevice_Failed = 66,
   SyncResetDevice_Failed = 67,
   ResetPdoPort_CallerNotPassiveLevel = 68,
   ResetPdoPort_CallerNotInD0 = 69,
   ReplicantDetected = 70,
   HubErr_HardResetFailed = 71,
   HubErr_HubOverCurrentChangeDetected = 72,
   HubErr_HubLocalPowerChangeDetected = 73,
   HubErr_HubChangeDetected = 74,
   HubErr_GlobalOvercurrent_Reset = 75,
   ExcessiveDriverResetsDetected = 76,
   DriverPortResetFailed = 77,
   PowerErr_FailedGetStatusOnSuspend = 78,
   PowerErr_SelectiveSuspendFailed = 79,
   HubErr_Reset2BadEnable = 80,
   PnpErr_Reset1Failure = 81,
   PnpErr_Reset2Failure = 82,
   HubErr_DescriptorParseError = 83,
   PnpErr_InvalidMsOsDescriptor = 84,
   PowerErr_FdoPowerFailure = 85,
   PnpErr_FailedResetOnStart = 86,
   HubErr_PnpDeviceStateError = 87,
   PowerErr_PostHiberRestoreFailed = 88,
   UsbErr_GetSNstringDescrptorFailed = 89,
   GenErr_ApiValidationFailure = 90,
   GenErr_UserIoctlFailed = 91,
   GenErr_UserIoctlFailed_Unsupported = 92,
   GenErr_UserIoctlFailed_InvalidIrql = 93,
   GenErr_IoctlCycleFailed = 94,
   GenErr_IoctlCycleFailed_InvalidIrql = 95,
   GenErr_IoctlCycleDetected = 96,
   PowerErr_MultiplePortsSignalledResume = 97,
   HubErr_PortResetTtFailed_NoAddress = 98,
   GenErr_WmiApiValidationFailure = 99,
   PnpErr_LegacyDevice = 100,
   PnpErr_PdoSymLinkFailed = 101,
   PnpErr_MsOsDescriptor_NotSupported = 102,
   PnpErr_PdoStartFailed = 103,
   PnpErr_CreateDeviceFailure_GetTtFailed = 104,
   HubErr_OvercurrentFalse = 105,
   HubErr_OvercurrentIgnored = 106,
   HubErr_OvercurrentClearFailed = 107,
   HubErr_Overcurrent_AutoClear = 108,
   PowerErr_ResumeFailedHubClassDs = 109,
   PowerErr_ErrorOnUpstreamStatus = 110,
   PowerErr_UpstreamPortDisabled = 111,
   PowerErr_DeviceCycledPostResume = 112,
   PowerErr_HardwareFailureOnWarmStart = 113,
   PowerErr_DeviceUnexpectedlyLostPower = 114,
   HubErr_DriverOvercurrent_Popup = 115,
   HubErr_InterruptZeroBytes = 116,
   HubErr_InvalidChangeBitReported = 117,
   PowerErr_DeviceDisconnectOnSuspend = 118,
   HubErr_SetPortIndicatorFailed = 119,
   GenErr_AppliedDeviceFix = 120,
   PnpErr_FailedResetOnQI = 121,
   PnpErr_BadIdleIrpParameters = 122,
   HubErr_SetUsbWakeFailed = 123,
   PowerErr_PortResumeTimeout = 124,
   PowerErr_ResumeTimeoutRestoreFailed = 125,
   HubErr_HubParked = 126,
   PowerErr_DeviceDisconnectOnResume = 127,
   SsErr_FailedSshResumeOnPort = 128,
   SsErr_FailedResumeOnPort = 129,
   PdoFailureData = 130,
   NestedHubs_Popup = 131,
   HubErr_User_Reset = 132,
   GenErr_AppliedHubFix = 133,
   PowerErr_SyncResumePortFailed = 134,
   PnpErr_InvalidContainerID = 135,
   SsErr_FailedSshSuspendHub = 136,
   SsErr_FailedSshResumeHub = 137,
   PowerErr_ResumeOnResetFailed = 138,
   HubErr_PortChangeStorm = 139,
   UsbErr_GetBillboardStringDescrptorFailed = 140,
   UsbErr_GetAlternateModeStringDescrptorFailed = 141,
   //Terminator
   ExceptionClassLast

} HUB_EXCEPTION_CLASS;

/*
    Format for hub exception block.  This structure is a contiguous block of
    memory to facilitate debugging. The Data and SzFile pointers always point
    into the same allocated memory block as the exception record.
*/

#define USBHUB_LOG_CATPURE      16

typedef struct _HUB_EXCEPTION_RECORD {

    ULONG Sig;

    ULONG TotalLength;

    // hub fdo useful if debugging live remote
    PDEVICE_OBJECT  HubFdo;
    HUB_EXCEPTION_CLASS Class;

    // port number assiciated with exception, 0 = hub
    USHORT PortNumber;
    USHORT Pad;

    USBD_STATUS UsbdStatus;
    NTSTATUS NtStatus;

    // this is limitted to 4k
    USHORT DataLength;
    PUCHAR Data;

    LIST_ENTRY ExrLink;

    // system time when this exception occurred
    LARGE_INTEGER EventTime;

    LOG_ENTRY DebugLog[USBHUB_LOG_CATPURE];

    ULONG Line;
    PUCHAR SzFile;

} HUB_EXCEPTION_RECORD, *PHUB_EXCEPTION_RECORD;

// Check against the current structure size limit hard coded into
// DBGENG.DLL
//
C_ASSERT(sizeof(HUB_EXCEPTION_RECORD) < USBHUB_MAXIMUM_HUB_EXCEPTION_RECORD_SIZE);

/*
    Exception data structures -- these are recorded for specific types of exceptions
    and provide extra info on what happened.
*/

/*
    This is recoreded on enumeration failure
*/
typedef struct _ENUM_FAIL_DATA {

    // elapsed time in frames for reset timeout
    ULONG DeltaTm;
    ULONG RetryCount;

    USHORT PortNumber;

} ENUM_FAIL_DATA, *PENUM_FAIL_DATA;


typedef struct _API_FAIL_DATA {

    ULONG ConnectionIdx;
    ULONG UsbIoCode;
    ULONG OutBuffLength;
    ULONG InBuffLength;
    ULONG MinInputSize;
    ULONG MinOutputSize;
    ULONG CurBusState;
    ULONG FdoFlags;
    ULONG ErrorLine;

} API_FAIL_DATA, *PAPI_FAIL_DATA;


typedef struct _IOCTL_FAIL_DATA {

    ULONG UsbIoctl;

} IOCTL_FAIL_DATA, *PIOCTL_FAIL_DATA;



typedef VOID HUBEXC_HANDLER(PDEVICE_OBJECT HubFdo,
                            PHUB_EXCEPTION_RECORD HubExr);


typedef struct _HUB_EXCEPTION_DISPATCH {

    // hub exception class code from exdef.h
    HUB_EXCEPTION_CLASS     Class;
    // test string describing execption, used in DEBUG BUILDS ONLY
    PCSTR                  DebugText;
    // optional handler function if special proceesing is required
    // such as user notification
    HUBEXC_HANDLER          *Handler;

} HUB_EXCEPTION_DISPATCH, *PHUB_EXCEPTION_DISPATCH;

typedef const HUB_EXCEPTION_DISPATCH *PCHUB_EXCEPTION_DISPATCH;

extern CCHAR usbfile_bus_c[];
extern CCHAR usbfile_busfunc_c[];
extern CCHAR usbfile_busif_c[];
extern CCHAR usbfile_busm_c[];
extern CCHAR usbfile_connect_c[];
extern CCHAR usbfile_dioctl_c[];
extern CCHAR usbfile_dq_c[];
extern CCHAR usbfile_enum1_c[];
extern CCHAR usbfile_enum2_c[];
//extern CCHAR usbfile_ex_C[];
extern CCHAR usbfile_fdopwr_c[];
extern CCHAR usbfile_hub_c[];
extern CCHAR usbfile_idstring_c[];
extern CCHAR usbfile_ioctl_c[];
extern CCHAR usbfile_lock_c[];
extern CCHAR usbfile_log_c[];
extern CCHAR usbfile_msos_c[];
extern CCHAR usbfile_overc_c[];
extern CCHAR usbfile_pchange_c[];
extern CCHAR usbfile_pdo_c[];
extern CCHAR usbfile_pdopwr_c[];
extern CCHAR usbfile_pind_c[];
extern CCHAR usbfile_pnp_c[];
extern CCHAR usbfile_refobj_c[];
extern CCHAR usbfile_reg_c[];
extern CCHAR usbfile_reset_c[];
extern CCHAR usbfile_sshub_c[];
extern CCHAR usbfile_ssdev_c[];
extern CCHAR usbfile_timer_c[];
extern CCHAR usbfile_usbd_c[];
extern CCHAR usbfile_wmi_c[];
extern CCHAR usbfile_etw_c[];

// Fdo, Portnumber, class_code, NTSTATUS, USBD_STATSUS, exceptiondata, length
//#define HUB_EXCEPTION(fdo, pn, c, nts, usbs, ed, edl) \
//    UsbhException((fdo), (pn), (c), (ed), (edl), (nts), (usbs),\
//    __FILE__, __LINE__, FALSE)

// same as hubexception but the hard reset flag is set to true so that the
// hub driver will bypass submitting transfers to the hub until reset has completed.
#define HUB_RESET_EXCEPTION(f_str, fdo, pn, c, nts, usbs, ed, edl) \
    UsbhException((fdo), (pn), (c), (ed), (edl), (nts), (usbs),\
    (f_str), __LINE__, TRUE)


// Fdo, Portnumber, class_code, NTSTATUS, USBD_STATSUS, exceptiondata, length
#define HUB_EXCEPTION(f_str, fdo, pn, c, nts, usbs, ed, edl) \
    UsbhException((fdo), (pn), (c), (ed), (edl), (nts), (usbs),\
    (f_str), __LINE__, FALSE)


#endif /* __EXDEF_H__ */
