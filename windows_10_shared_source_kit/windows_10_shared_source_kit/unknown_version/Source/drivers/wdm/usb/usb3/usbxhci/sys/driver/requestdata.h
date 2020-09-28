/*++
Copyright (c) Microsoft Corporation

Module Name:

    requestdata.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

typedef union _REQUEST_DATA {

    ENDPOINT_UPDATE_DATA                EndpointUpdateData;
    ENDPOINT_STATIC_STREAMS_CONFIGURE   EndpointStaticStreamsConfigure;
    ENDPOINT_RESET_DATA                 EndpointResetData;
    USBDEVICE_ENDPOINTS_CONFIGURE_DATA  UsbDeviceEndpointsConfigureData;

    CONTROL_TRANSFER_DATA               ControlTransferData;
    ISOCH_TRANSFER_DATA                 IsochTransferData;
    BULK_TRANSFER_DATA                  BulkTransferData;

} REQUEST_DATA, *PREQUEST_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_DATA, GetRequestData)
