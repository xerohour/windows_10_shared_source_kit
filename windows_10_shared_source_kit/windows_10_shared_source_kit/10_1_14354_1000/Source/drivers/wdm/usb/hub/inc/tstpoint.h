//
//    Copyright (C) Microsoft.  All rights reserved.
//
/*

    Test Point Definitions for USBHUB,USBROOT and test version of BULKUSB


*/



typedef enum _HUB_TESTPOINT_OPCODE {
/***
    Service: TstPt_DriverReset
    Description: Signals when a driver initiated reset is detected.
    BusContext: Bus context for the hub where the driver reset ocurred
    DeviceHandle: NULL - not used
    Parameter: PULONG, Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_DriverReset = 0,
/***
    Service: TstPt_DriverResetComplete
    Description: Signals when a driver initiated reset has been completed.
    BusContext: Bus context for the hub where the driver reset ocurred
    DeviceHandle: NULL - not used
    Parameter: PULONG, Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_DriverResetComplete,
/***
    Service: TstPt_DevicePdoStart
    Description: Signals when a the start irp for a device Pdo will be completed.
    BusContext: Bus context for the hub the device is attached to
    DeviceHandle: NULL - not used
    Parameter: PULONG, Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_DevicePdoStart,
/***
    Service: TstPt_SelctiveSuspend
    Description: Signals when a selctive port suspend is requested.
    BusContext: Bus context for the hub where the driver reset ocurred
    DeviceHandle: NULL - not used
    Parameter: PULONG, Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_SelctiveSuspend,
/***
    Service: TstPt_SelctiveSuspend
    Description: Signals when a a selctive port suspend has completed.
    BusContext: Bus context for the hub where the driver reset ocurred
    DeviceHandle: NULL - not used
    Parameter: PULONG, Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_SelctiveSuspendComplete,

/***
    Service: TstPt_PnpRemoveDevice
    Description: Signals when the hub driver receives a remove irp
    BusContext: Bus context for the hub being removed
    DeviceHandle: NULL - not used
    Parameter: not used
    Returns: STATUS_SUCCESS
***/
    TstPt_PnpRemoveDevice,

/***
    Service: TstPt_SetupDeviceFailureRetry
    Description: Forces failure of SetupDevice function forcing
        a retry thru in the enum2 codepath.
    BusContext: Bus context for the hub being removed
    DeviceHandle: NULL - not used
    Parameter: PULONG Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_SetupDeviceFailureRetry,

/***
    Service: TstPt_ResetTimeoutDpc
    Description: Signals when the reset timeout DPC fires, used to create
        conditions where th ereset timeout is queued after the reset completes.
    BusContext: Bus context for the hub being removed
    DeviceHandle: NULL - not used
    Parameter: PULONG Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_ResetTimeoutDpc,

/***
    Service: TstPt_ResetTimeoutDpc
    Description: Indicates the overcurrent reset worker has fired used for any overcurrent worker routine
    
    BusContext: NULL - not used
    DeviceHandle: NULL - not used
    Parameter: PULONG Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_OvercurrentResetWorker,

/***
    Service: TstPt_AsyncStartTimeout
    Description: Indicates the hubs async start time limit has been exceeded
    
    BusContext: NULL - not used
    DeviceHandle: NULL - not used
    Parameter: PULONG Port Number
    Returns: STATUS_SUCCESS
***/
    TstPt_AsyncStartTimeout



} HUB_TESTPOINT_OPCODE, *PHUB_TESTPOINT_OPCODE;



