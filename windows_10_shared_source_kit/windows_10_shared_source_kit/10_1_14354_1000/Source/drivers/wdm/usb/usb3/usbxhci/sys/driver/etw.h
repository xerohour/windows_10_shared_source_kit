//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

VOID
Etw_ControllerRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    );

VOID
Etw_DeviceListRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    );

VOID
Etw_DeviceRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    );

VOID
Etw_EndpointListRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    );

VOID
Etw_EndpointRundown(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PENDPOINT_DATA              EndpointData
    );

VOID
Etw_ControllerCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    );

VOID
Etw_ControllerDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    );

VOID
Etw_ControllerFirmareVersionUpdate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCONTROLLER_DATA            ControllerData
    );

VOID
Etw_DeviceCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    );

VOID
Etw_DeviceUpdate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    );

VOID
Etw_DeviceDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUSBDEVICE_DATA             UsbDeviceData
    );

VOID
Etw_EndpointCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PENDPOINT_DATA              EndpointData
    );

VOID
Etw_EndpointUpdate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PENDPOINT_DATA              EndpointData
    );

VOID
Etw_EndpointDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PENDPOINT_DATA              EndpointData
    );

VOID
Etw_StartDeviceFail(
    __in
        HANDLE                      ControllerHandle,
    __in_opt
        LPCGUID                     ActivityId,
    __in
        StartDeviceFailReason       ReasonCode
    );

VOID
Etw_CommandSend(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCOMMAND_DATA               CommandData,
    __in
        PCRB                        Crb
    );

VOID
Etw_CommandWaitlisted(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCOMMAND_DATA               CommandData,
    __in
        PCRB                        Crb,
    __in
        COMMAND_WAITLIST_REASON     WaitlistReason
    );

VOID
Etw_CommandComplete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCOMMAND_DATA               CommandData,
    __in
        PCRB                        Crb,
    __in
        CRB_COMPLETION_STATUS       CompletionStatus
    );

VOID
Etw_CommandCompleteError(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PCOMMAND_DATA               CommandData,
    __in
        PCRB                        Crb,
    __in
        CRB_COMPLETION_STATUS       CompletionStatus
    );
