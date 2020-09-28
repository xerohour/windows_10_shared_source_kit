/*++
Copyright (c) Microsoft Corporation

Module Name:

    devicexfer.h

Abstract:

    Control transfers for attached USB device and support routines.

Author:

Environment:

    Kernel mode only.

--*/

#pragma once

EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBDTX_ControlTransferComplete;
EVT_WDF_REQUEST_COMPLETION_ROUTINE HUBDTX_GetDeviceDescriptorComplete;


PPIPE_HANDLE
HUBDTX_GetMatchingInternalPipeHandle (
    __in 
        PDEVICE_CONFIGURATION   DeviceConfiguration,
    __in 
        USBD_PIPE_HANDLE        UsbdPipeHandle
    );
