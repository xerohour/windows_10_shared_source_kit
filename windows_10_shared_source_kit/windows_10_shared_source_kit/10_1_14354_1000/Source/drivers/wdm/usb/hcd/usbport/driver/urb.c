/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    urb.c

Abstract:

    main urb "handler"

Environment:

    kernel mode only

Notes:

Revision History:

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "urb.tmh"
#endif

/*
// Urb function summary:

0x0000: URB_FUNCTION_SELECT_CONFIGURATION
0x0001: URB_FUNCTION_SELECT_INTERFACE
0x0002: URB_FUNCTION_ABORT_PIPE
0x0007: URB_FUNCTION_GET_CURRENT_FRAME_NUMBER
0x0008: URB_FUNCTION_CONTROL_TRANSFER
0x0009: URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER
0x000A: URB_FUNCTION_ISOCH_TRANSFER
0x000B: URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE
0x000C: URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE
0x000D: URB_FUNCTION_SET_FEATURE_TO_DEVICE
0x000E: URB_FUNCTION_SET_FEATURE_TO_INTERFACE
0x000F: URB_FUNCTION_SET_FEATURE_TO_ENDPOINT
0x0010: URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE
0x0011: URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE
0x0012: URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT
0x0013: URB_FUNCTION_GET_STATUS_FROM_DEVICE
0x0014: URB_FUNCTION_GET_STATUS_FROM_INTERFACE
0x0015: URB_FUNCTION_GET_STATUS_FROM_ENDPOINT
0x0017: URB_FUNCTION_VENDOR_DEVICE
0x0018: URB_FUNCTION_VENDOR_INTERFACE
0x0019: URB_FUNCTION_VENDOR_ENDPOINT
0x001A: URB_FUNCTION_CLASS_DEVICE
0x001B: URB_FUNCTION_CLASS_INTERFACE
0x001C: URB_FUNCTION_CLASS_ENDPOINT
0x001E: URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL
        (AKA URB_FUNCTION_RESET_PIPE)
0x001F: URB_FUNCTION_CLASS_OTHER
0x0020: URB_FUNCTION_VENDOR_OTHER
0x0021: URB_FUNCTION_GET_STATUS_FROM_OTHER
0x0022: URB_FUNCTION_CLEAR_FEATURE_TO_OTHER
0x0023: URB_FUNCTION_SET_FEATURE_TO_OTHER
0x0024: URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT
0x0025: URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT
0x0026: URB_FUNCTION_GET_CONFIGURATION
0x0027: URB_FUNCTION_GET_INTERFACE
0x0028: URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE
0x0029: URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE
0x002A: URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR
0x0030: URB_FUNCTION_SYNC_RESET_PIPE
0x0031: URB_FUNCTION_SYNC_CLEAR_STALL
0x0032: URB_FUNCTION_CONTROL_TRANSFER_EX

// Obsolete URB functions.  No longer supported.
//
0x0003: URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL
0x0004: URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL
0x0005: URB_FUNCTION_GET_FRAME_LENGTH
0x0006: URB_FUNCTION_SET_FRAME_LENGTH

// Reserved function codes.
//
0x0016: URB_FUNCTION_RESERVED_0X0016
0x001D: URB_FUNCTION_RESERVE_0X001D
0x002B: URB_FUNCTION_RESERVE_0X002B
0x002C: URB_FUNCTION_RESERVE_0X002C
0x002D: URB_FUNCTION_RESERVE_0X002D
0x002E: URB_FUNCTION_RESERVE_0X002E
0x002F: URB_FUNCTION_RESERVE_0X002F
0x0033: URB_FUNCTION_RESERVE_0X0033
0x0034: URB_FUNCTION_RESERVE_0X0034

** URB handler routines

    Handler -- This function handles the specific USBDI request.  If the
    request is queued by the handler then STATUS_PENDING is returned.
*/

typedef
NTSTATUS
URB_HANDLER(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    );

typedef URB_HANDLER *PURB_HANDLER;

typedef struct _URB_DISPATCH_ENTRY {
    // USB API handler
    PURB_HANDLER   UrbHandler;
    // length of the URB expected for this request
    USHORT         UrbRequestLength;
    USHORT         Pad2;
    // request code for setup packet if standard command
    UCHAR          Direction;
    UCHAR          Type;
    UCHAR          Recipient;
    UCHAR          bRequest;

    // tell the generic urb dispatch routine what to do
    ULONG          Flags;
#if DBG
    ULONG ExpectedFunctionCode;
#endif
} URB_DISPATCH_ENTRY;

URB_HANDLER USBPORT_NotSupported;
URB_HANDLER USBPORT_InvalidFunction;
URB_HANDLER USBPORT_ControlTransfer;
URB_HANDLER USBPORT_BulkOrInterruptTransfer;
URB_HANDLER USBPORT_IsochTransfer;
URB_HANDLER USBPORT_AbortPipe;
URB_HANDLER USBPORT_SyncResetPipeAndClearStall;
URB_HANDLER USBPORT_SyncResetPipe;
URB_HANDLER USBPORT_SyncClearStall;
URB_HANDLER USBPORT_SCT_GetSetDescriptor;
URB_HANDLER USBPORT_SCT_SetClearFeature;
URB_HANDLER USBPORT_SCT_GetStatus;
URB_HANDLER USBPORT_SCT_VendorClassCommand;
URB_HANDLER USBPORT_SCT_GetInterface;
URB_HANDLER USBPORT_SCT_GetConfiguration;
URB_HANDLER USBPORT_GetCurrentFrame;
URB_HANDLER USBPORT_GetMSFeatureDescriptor;

// last supported function
#define URB_FUNCTION_LAST   URB_FUNCTION_RESERVE_0X0034

// last valid function
const URB_DISPATCH_ENTRY UrbDispatchTable[URB_FUNCTION_LAST+1] =
{
    //URB_FUNCTION_SELECT_CONFIGURATION
    USBPORT_SelectConfiguration,
    0,  // Length, handler will validate length
    0,  // Pad2
    0,  // bmRequestType.Dir
    0,  // bmRequestType.Type
    0,  // bmRequestType.Recipient
    0,  // bRequest
    0,  // Flags
#if DBG
    URB_FUNCTION_SELECT_CONFIGURATION,
#endif

    //URB_FUNCTION_SELECT_INTERFACE
    USBPORT_SelectInterface, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_SELECT_INTERFACE,
#endif

    //URB_FUNCTION_ABORT_PIPE
    USBPORT_AbortPipe, // Function
    sizeof(struct _URB_PIPE_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    USBPORT_REQUEST_IS_PIPE_REQUEST, // Flags
#if DBG
    URB_FUNCTION_ABORT_PIPE,
#endif

    //URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL
    USBPORT_NotSupported,  // Function
    sizeof(struct _URB_FRAME_LENGTH_CONTROL), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL,
#endif

    //URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL
    USBPORT_NotSupported, // Function
    sizeof(struct _URB_FRAME_LENGTH_CONTROL), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL,
#endif

    //URB_FUNCTION_GET_FRAME_LENGTH
    USBPORT_NotSupported, // Function
    sizeof(struct _URB_GET_FRAME_LENGTH), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_GET_FRAME_LENGTH,
#endif

    //URB_FUNCTION_SET_FRAME_LENGTH
    USBPORT_NotSupported, // Function
    sizeof(struct _URB_SET_FRAME_LENGTH), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_SET_FRAME_LENGTH,
#endif

    //URB_FUNCTION_GET_CURRENT_FRAME_NUMBER
    USBPORT_GetCurrentFrame, // Function
    sizeof(struct _URB_GET_CURRENT_FRAME_NUMBER), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_GET_CURRENT_FRAME_NUMBER,
#endif

    //URB_FUNCTION_CONTROL_TRANSFER
    USBPORT_ControlTransfer,  // Function
    sizeof(struct _URB_CONTROL_TRANSFER), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    USBPORT_REQUEST_IS_TRANSFER,    // Flags
#if DBG
    URB_FUNCTION_CONTROL_TRANSFER,
#endif

    //URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER
    USBPORT_BulkOrInterruptTransfer, // Function
    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    USBPORT_REQUEST_IS_TRANSFER,    // Flags
#if DBG
    URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER,
#endif

    //URB_FUNCTION_ISOCH_TRANSFER
    USBPORT_IsochTransfer, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    USBPORT_REQUEST_IS_TRANSFER, // Flags
#if DBG
    URB_FUNCTION_ISOCH_TRANSFER,
#endif

    //URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE
    USBPORT_SCT_GetSetDescriptor, // Function
    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_DEVICE, // bmRequestType.Recipient
    USB_REQUEST_GET_DESCRIPTOR, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE,
#endif

    //URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE
    USBPORT_SCT_GetSetDescriptor, // Function
    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_DEVICE, // bmRequestType.Recipient
    USB_REQUEST_SET_DESCRIPTOR, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE,
#endif

    //URB_FUNCTION_SET_FEATURE_TO_DEVICE
    USBPORT_SCT_SetClearFeature, // Function
    sizeof(struct _URB_CONTROL_FEATURE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_DEVICE, // bmRequestType.Recipient
    USB_REQUEST_SET_FEATURE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | \
        USBPORT_REQUEST_USES_DEFAULT_PIPE | \
        USBPORT_REQUEST_NO_DATA_PHASE, // Flags
#if DBG
    URB_FUNCTION_SET_FEATURE_TO_DEVICE,
#endif

    //URB_FUNCTION_SET_FEATURE_TO_INTERFACE
    USBPORT_SCT_SetClearFeature, // Function
    sizeof(struct _URB_CONTROL_FEATURE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_INTERFACE, // bmRequestType.Recipient
    USB_REQUEST_SET_FEATURE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | \
        USBPORT_REQUEST_USES_DEFAULT_PIPE | \
        USBPORT_REQUEST_NO_DATA_PHASE, // Flags
#if DBG
    URB_FUNCTION_SET_FEATURE_TO_INTERFACE,
#endif

    //URB_FUNCTION_SET_FEATURE_TO_ENDPOINT
    USBPORT_SCT_SetClearFeature, // Function
    sizeof(struct _URB_CONTROL_FEATURE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_ENDPOINT, // bmRequestType.Recipient
    USB_REQUEST_SET_FEATURE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | \
        USBPORT_REQUEST_USES_DEFAULT_PIPE | \
        USBPORT_REQUEST_NO_DATA_PHASE, // Length
#if DBG
    URB_FUNCTION_SET_FEATURE_TO_ENDPOINT,
#endif

    //URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE
    USBPORT_SCT_SetClearFeature, // Function
    sizeof(struct _URB_CONTROL_FEATURE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_DEVICE, // bmRequestType.Recipient
    USB_REQUEST_CLEAR_FEATURE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | \
        USBPORT_REQUEST_USES_DEFAULT_PIPE | \
        USBPORT_REQUEST_NO_DATA_PHASE, // Flags
#if DBG
    URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE,
#endif

    //URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE
    USBPORT_SCT_SetClearFeature, // Function
    sizeof(struct _URB_CONTROL_FEATURE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_INTERFACE, // bmRequestType.Recipient
    USB_REQUEST_CLEAR_FEATURE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | \
        USBPORT_REQUEST_USES_DEFAULT_PIPE | \
        USBPORT_REQUEST_NO_DATA_PHASE, // Flags
#if DBG
    URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE,
#endif

    //URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT
    USBPORT_SCT_SetClearFeature, // Function
    sizeof(struct _URB_CONTROL_FEATURE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_ENDPOINT, // bmRequestType.Recipient
    USB_REQUEST_CLEAR_FEATURE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | \
        USBPORT_REQUEST_USES_DEFAULT_PIPE | \
        USBPORT_REQUEST_NO_DATA_PHASE, // Flags
#if DBG
    URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT,
#endif

    //URB_FUNCTION_GET_STATUS_FROM_DEVICE
    USBPORT_SCT_GetStatus, // Function
    sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_DEVICE, // bmRequestType.Recipient
    USB_REQUEST_GET_STATUS, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_STATUS_FROM_DEVICE,
#endif

    //URB_FUNCTION_GET_STATUS_FROM_INTERFACE
    USBPORT_SCT_GetStatus, // Function
    sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_INTERFACE, // bmRequestType.Recipient
    USB_REQUEST_GET_STATUS, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_STATUS_FROM_INTERFACE,
#endif

    //URB_FUNCTION_GET_STATUS_FROM_ENDPOINT
    USBPORT_SCT_GetStatus, // Function
    sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_ENDPOINT, // bmRequestType.Recipient
    USB_REQUEST_GET_STATUS, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_STATUS_FROM_ENDPOINT,
#endif

    //URB_FUNCTION_RESERVED_0X0016
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVED_0X0016,
#endif

    //URB_FUNCTION_VENDOR_DEVICE
    USBPORT_SCT_VendorClassCommand,  // Function
    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir, user defined
    BMREQUEST_VENDOR, // bmRequestType.Type
    BMREQUEST_TO_DEVICE, // bmRequestType.Recipient
    0, // bRequest, user defined
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_VENDOR_DEVICE,
#endif

    //URB_FUNCTION_VENDOR_INTERFACE
    USBPORT_SCT_VendorClassCommand, // Function
    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir, user defined
    BMREQUEST_VENDOR, // bmRequestType.Type
    BMREQUEST_TO_INTERFACE, // bmRequestType.Recipient
    0, // bRequest, user defined
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Length
#if DBG
    URB_FUNCTION_VENDOR_INTERFACE,
#endif

    //URB_FUNCTION_VENDOR_ENDPOINT
    USBPORT_SCT_VendorClassCommand, // Function
    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir, user defined
    BMREQUEST_VENDOR, // bmRequestType.Type
    BMREQUEST_TO_ENDPOINT, // bmRequestType.Recipient
    0, // bRequest, user defined
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_VENDOR_ENDPOINT,
#endif

    //URB_FUNCTION_CLASS_DEVICE
    USBPORT_SCT_VendorClassCommand, // Function
    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir, user defined
    BMREQUEST_CLASS, // bmRequestType.Type
    BMREQUEST_TO_DEVICE, // bmRequestType.Recipient
    0, // bRequest, user defined
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_CLASS_DEVICE,
#endif

    //URB_FUNCTION_CLASS_INTERFACE
    USBPORT_SCT_VendorClassCommand, // Function
    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir, user defined
    BMREQUEST_CLASS, // bmRequestType.Type
    BMREQUEST_TO_INTERFACE, // bmRequestType.Recipient
    0, // bRequest, user defined
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_CLASS_INTERFACE,
#endif

    //URB_FUNCTION_CLASS_ENDPOINT
    USBPORT_SCT_VendorClassCommand, // Function
    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir, user defined
    BMREQUEST_CLASS, // bmRequestType.Type
    BMREQUEST_TO_ENDPOINT, // bmRequestType.Recipient
    0, // bRequest, user defined
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_CLASS_ENDPOINT,
#endif

    //URB_FUNCTION_RESERVE_0X001D
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVE_0X001D,
#endif

    //URB_FUNCTION_RESET_PIPE
    USBPORT_SyncResetPipeAndClearStall, // Function
    sizeof(struct _URB_PIPE_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    USBPORT_REQUEST_IS_PIPE_REQUEST, // Flags
#if DBG
    URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL,
#endif

    //URB_FUNCTION_CLASS_OTHER
    USBPORT_SCT_VendorClassCommand, // Function
    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir, user defined
    BMREQUEST_CLASS, // bmRequestType.Type
    BMREQUEST_TO_OTHER, // bmRequestType.Recipient
    0, // bRequest, user defined
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Length
#if DBG
    URB_FUNCTION_CLASS_OTHER,
#endif

    //URB_FUNCTION_VENDOR_OTHER
    USBPORT_SCT_VendorClassCommand, // Function
    sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir, user defined
    BMREQUEST_VENDOR, // bmRequestType.Type
    BMREQUEST_TO_OTHER, // bmRequestType.Recipient
    0, // bRequest, user defined
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_VENDOR_OTHER,
#endif

    //URB_FUNCTION_GET_STATUS_FROM_OTHER
    USBPORT_SCT_GetStatus, // Function
    sizeof(struct _URB_CONTROL_GET_STATUS_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_OTHER, // bmRequestType.Recipient
    USB_REQUEST_GET_STATUS, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_STATUS_FROM_OTHER,
#endif

    //URB_FUNCTION_CLEAR_FEATURE_TO_OTHER
    USBPORT_SCT_SetClearFeature, // Function
    sizeof(struct _URB_CONTROL_FEATURE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_OTHER, // bmRequestType.Recipient
    USB_REQUEST_CLEAR_FEATURE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER |
        USBPORT_REQUEST_USES_DEFAULT_PIPE |
        USBPORT_REQUEST_NO_DATA_PHASE,
#if DBG
    URB_FUNCTION_CLEAR_FEATURE_TO_OTHER,
#endif

    //URB_FUNCTION_SET_FEATURE_TO_OTHER
    USBPORT_SCT_SetClearFeature, // Function
    sizeof(struct _URB_CONTROL_FEATURE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_OTHER, // bmRequestType.Recipient
    USB_REQUEST_SET_FEATURE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER |
        USBPORT_REQUEST_USES_DEFAULT_PIPE |
        USBPORT_REQUEST_NO_DATA_PHASE, // Flags
#if DBG
    URB_FUNCTION_SET_FEATURE_TO_OTHER,
#endif

     //URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT
    USBPORT_SCT_GetSetDescriptor, // Function
    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_ENDPOINT, // bmRequestType.Recipient
    USB_REQUEST_GET_DESCRIPTOR, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT,
#endif

     //URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT
    USBPORT_SCT_GetSetDescriptor, // Function
    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_ENDPOINT, // bmRequestType.Recipient
    USB_REQUEST_SET_DESCRIPTOR, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT,
#endif

    //URB_FUNCTION_GET_CONFIGURATION
    USBPORT_SCT_GetConfiguration, // Function
    sizeof(struct _URB_CONTROL_GET_CONFIGURATION_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_DEVICE, // bmRequestType.Recipient
    USB_REQUEST_GET_CONFIGURATION, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_CONFIGURATION,
#endif

    //URB_FUNCTION_GET_INTERFACE
    USBPORT_SCT_GetInterface, // Function
    sizeof(struct _URB_CONTROL_GET_INTERFACE_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_INTERFACE, // bmRequestType.Recipient
    USB_REQUEST_GET_INTERFACE, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_INTERFACE,
#endif

    //URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE
    USBPORT_SCT_GetSetDescriptor, // Function
    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_INTERFACE, // bmRequestType.Recipient
    USB_REQUEST_GET_DESCRIPTOR, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE,
#endif

    //URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE
    USBPORT_SCT_GetSetDescriptor, // Function
    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST), // Length
    0, // Pad2
    BMREQUEST_HOST_TO_DEVICE, // bmRequestType.Dir
    BMREQUEST_STANDARD, // bmRequestType.Type
    BMREQUEST_TO_INTERFACE, // bmRequestType.Recipient
    USB_REQUEST_SET_DESCRIPTOR, // bRequest
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE,
#endif

    //URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR
    USBPORT_GetMSFeatureDescriptor, // Function
    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST), // Length
    0, // Pad2
    BMREQUEST_DEVICE_TO_HOST, // bmRequestType.Dir
    BMREQUEST_VENDOR, // bmRequestType.Type
    0, // bmRequestType.Recipient defined by caller
    0, // bRequest set by usbhub
    USBPORT_REQUEST_IS_TRANSFER | USBPORT_REQUEST_USES_DEFAULT_PIPE, // Flags
#if DBG
    URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR,
#endif

     //URB_FUNCTION_RESERVE_0X002B
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVE_0X002B,
#endif

    //URB_FUNCTION_RESERVE_0X002C
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVE_0X002C,
#endif

    //URB_FUNCTION_RESERVE_0X002D
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVE_0X002D,
#endif

    //URB_FUNCTION_RESERVE_0X002E
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVE_0X002E,
#endif

    //URB_FUNCTION_RESERVE_0X002F
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVE_0X002F,
#endif

    //URB_FUNCTION_SYNC_RESET_PIPE
    USBPORT_SyncResetPipe, // Function
    sizeof(struct _URB_PIPE_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    USBPORT_REQUEST_IS_PIPE_REQUEST, // Flags
#if DBG
    URB_FUNCTION_SYNC_RESET_PIPE,
#endif

     //URB_FUNCTION_SYNC_CLEAR_STALL
    USBPORT_SyncClearStall, // Function
    sizeof(struct _URB_PIPE_REQUEST), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    USBPORT_REQUEST_IS_PIPE_REQUEST, // Flags
#if DBG
    URB_FUNCTION_SYNC_CLEAR_STALL,
#endif

    //URB_FUNCTION_CONTROL_TRANSFER_EX
    USBPORT_ControlTransfer, // Function
    sizeof(struct _URB_CONTROL_TRANSFER_EX), // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    USBPORT_REQUEST_IS_TRANSFER, // Flags
#if DBG
    URB_FUNCTION_CONTROL_TRANSFER_EX,
#endif

    //URB_FUNCTION_RESERVE_0X0033
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVE_0X0033,
#endif

    //URB_FUNCTION_RESERVE_0X0034
    USBPORT_InvalidFunction, // Function
    0, // Length
    0, // Pad2
    0, // bmRequestType.Dir
    0, // bmRequestType.Type
    0, // bmRequestType.Recipient
    0, // bRequest
    0, // Flags
#if DBG
    URB_FUNCTION_RESERVE_0X0034,
#endif
};

NTSTATUS
USBPORT_ProcessURB(
    PDEVICE_OBJECT          PdoDeviceObject,
    PDEVICE_OBJECT          FdoDeviceObject,
    PIRP                    Irp,
    PURB                    Urb,
    PUSB_IOREQUEST_CONTEXT  UsbIoRequest
    )
/*++

Routine Description:

    Processes a URB from a function-driver IRP.

    Essentially what we do here is look at the URB and validate some
    of the the parameters for the function-driver.

    In some cases we translate the urb in to multiple bus transactions.

Arguments:

    FdoDeviceObject - Device object associated with this IRP request

    Irp -  IO request block

    Urb -  ptr to USB request block

Return Value:

--*/
{
    PDEVICE_EXTENSION     devExt;
    BOOLEAN               headerValid = FALSE;
    USHORT                function;
    PUSBD_DEVICE_HANDLE   deviceHandle = NULL;
    PTRANSFER_URB         transferUrb = NULL;
    PUSBD_PIPE_HANDLE_I   pipeHandle = NULL;
    PHCD_ENDPOINT         endpoint = NULL;
    GUID                  activityIdGuid;
    LPGUID                activityId;
    NTSTATUS              ntStatus;
    ETW_EXECUTION_METRICS execMetrics;
    BOOLEAN               relaxControlMaxTransferSize = FALSE;

    ETW_MEASURE_TIME_START(execMetrics);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // Assume success for the completion of the Irp and URB.
    //
    ntStatus = STATUS_SUCCESS;

    if (NT_SUCCESS(USBPORT_GetActivityIdIrp(Irp, &activityIdGuid))) {
        activityId = &activityIdGuid;
    } else {
        activityId = NULL;
    }

    // Validate that the URB Length is at least the size of a
    // _URB_HEADER structure before accessing any other fields of the
    // URB.
    //
    if (Urb->UrbHeader.Length < sizeof(struct _URB_HEADER)) {

        // Unfortunately for compatibility with shipping drivers
        // variable length URBs (e.g. URB_FUNCTION_SELECT_INTERFACE)
        // cannot be blanket failed here simply because the
        // UrbHeader.Length does not appear to be valid as this has
        // never been validated in the past.
        //
        // Fixed length URBs will still be subsequently validated based
        // on the UrbDispatchTable[function].UrbRequestLength as that
        // has always been done.

        // Log the invalid URB header length warning.
        // template: tid_USBPORT_Dispatch_Urb_Invalid_Header
        //
        USBPORT_EtwWriteHcData3(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_HEADER_LENGTH_WARNING,
                                activityId,

                                &Irp,
                                sizeof(PIRP),

                                &Urb,
                                sizeof(PURB),

                                &Urb->UrbHeader,
                                sizeof(struct _URB_HEADER));

        // Cannot do this here: goto USBPORT_ProcessURB_Done;

        headerValid = TRUE;

    } else {

        headerValid = TRUE;
    }

    // Initialize the URB status to USBD_STATUS_SUCCESS.
    // Some drivers do not initialize the URB status.
    //
    Urb->UrbHeader.Status = USBD_STATUS_SUCCESS;

    // Retrieve the URB_FUNCTION_* from the URB.
    //
    function = Urb->UrbHeader.Function;

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'pURB', Urb, Irp, function);

    // Initialize the UsbdFlags field of the URB.
    //
    Urb->UrbHeader.UsbdFlags = 0;

    // Validate the URB Function.
    //
    if (function > URB_FUNCTION_LAST) {

        ntStatus = SET_USBD_ERROR(Urb,
                                  USBD_STATUS_INVALID_URB_FUNCTION);

        // Log the invalid URB function error.
        // template: tid_USBPORT_Dispatch_Urb_Invalid_Header
        //
        USBPORT_EtwWriteHcData3(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_FUNCTION,
                                activityId,

                                &Irp,
                                sizeof(PIRP),

                                &Urb,
                                sizeof(PURB),

                                &Urb->UrbHeader,
                                sizeof(struct _URB_HEADER));

        goto USBPORT_ProcessURB_Done;
    }
#if DBG
      else {
        USBPORT_ASSERT(UrbDispatchTable[function].ExpectedFunctionCode == function);
    }
#endif

    // Validate the URB Length based on the URB Function.  We must do
    // this before we touch any of the URB fields outside of the header.
    //
    //
    if (UrbDispatchTable[function].UrbRequestLength &&
        UrbDispatchTable[function].UrbRequestLength != Urb->UrbHeader.Length) {

        ntStatus = SET_USBD_ERROR(Urb,
                                  USBD_STATUS_INVALID_PARAMETER);

        // Log the invalid URB length error.
        // template: tid_USBPORT_Dispatch_Urb_Invalid_Header
        //
        USBPORT_EtwWriteHcData3(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_HEADER_LENGTH,
                                activityId,

                                &Irp,
                                sizeof(PIRP),

                                &Urb,
                                sizeof(PURB),

                                &Urb->UrbHeader,
                                sizeof(struct _URB_HEADER));

        goto USBPORT_ProcessURB_Done;
    }

    // Retrieve the Device Handle from the URB.
    //
    GET_DEVICE_HANDLE(deviceHandle, Urb);

    // Handle the special case of the NULL Device Handle, which
    // indicates a URB for the Root Hub.
    //
    if (deviceHandle == NULL) {
        PDEVICE_EXTENSION rhDevExt;

        GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
        ASSERT_PDOEXT(rhDevExt);

        // Set the Device Handle to the Root Hub Device Handle.  This
        // also sets the Device Handle in the client URB.
        //
        deviceHandle = Urb->UrbHeader.UsbdDeviceHandle =
            rhDevExt->Pdo.RootHubDeviceHandle;
    }

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'devH', deviceHandle, Urb, 0);

    // Validate and reference the Device Handle.
    //
    // The USBPORT driver has been modified so that URB requests from
    // function drivers are now pinned to the device handle.  This in
    // turn pins the device driver and PDO in memory.  If no device
    // handle is present then it becomes dangerous to queue the request
    // in usbport. This resulted in cases where the driver that posted
    // the request was able to unload while the irp was still pending in
    // USBPORT.
    //
    // However, it is still possible for requests to reach the port
    // driver after the device handle becomes invalid.
    //
    //     example: if the device handle is marked removed after the
    //     request has passed thru the hub.
    //
    // The neohub driver will now ensure that the PDO remains until
    // USBPORT completes the request (a change from previous versions of
    // WinXP).  USBPORT will also now immediately complete invalid
    // requests instead of queueing them (another change from XP). If
    // the driver continues to send requests while the device is gone
    // this condition is dealt with by the neohub driver which is better
    // equipped to determine the pnp and physical state of the device.
    //

    // NOTE: USBPORT_UrbValidDeviceHandle() will reference the Device
    // Handle if the Device Handle is valid.
    //
    if (!USBPORT_UrbValidDeviceHandle(FdoDeviceObject,
                                      deviceHandle,
                                      Urb,
                                      Irp)) {

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, '!URB', Urb, Irp, function);

        SET_USBD_ERROR(Urb,
                       USBD_STATUS_DEVICE_GONE);

        ntStatus = STATUS_NO_SUCH_DEVICE;

        // Log the invalid device handle error.
        // template: tid_USBPORT_Dispatch_Urb_Invalid_Header
        //
        USBPORT_EtwWriteHcData3(FdoDeviceObject,
                                &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_DEVICE_HANDLE,
                                activityId,

                                &Irp,
                                sizeof(PIRP),

                                &Urb,
                                sizeof(PURB),

                                &Urb->UrbHeader,
                                sizeof(struct _URB_HEADER));

        // Set the Device Handle to NULL as it is invalid and cannot be
        // dereferenced.
        //
        deviceHandle = NULL;

        goto USBPORT_ProcessURB_Done;
    }

    // Fail requests while suspended.
    //
    // Fdo.DmTimerFlags.FailUrbs is set and cleared by
    // USBPORT_SuspendController() and USBPORT_ResumeUsbController().
    //
    // Also fail requests if the miniport fails after a power state
    // transition.
    //
    // Fdo.PowerFlags.MpRestartFailed can be set by
    // USBPORT_ResumeUsbController() and USBPORT_TurnUsbControllerOn().
    //
    if (devExt->Fdo.DmTimerFlags.FailUrbs ||
        devExt->Fdo.PowerFlags.MpRestartFailed ) {

        USBPORT_KdPrint((1, "'Error: Bad Request to root hub %p\n", FdoDeviceObject));

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, '!URr', Urb, Irp, function);

        SET_USBD_ERROR(Urb,
                       USBD_STATUS_DEVICE_GONE);

        ntStatus = STATUS_NO_SUCH_DEVICE;

        // Log the URB failure due to HC suspend.
        // template: tid_USBPORT_Dispatch_Urb_Device_Invalid_Header
        //
        USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                      deviceHandle,
                                      &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_DUE_TO_HC_SUSPEND,
                                      activityId,

                                      &Irp,
                                      sizeof(PIRP),

                                      &Urb,
                                      sizeof(PURB),

                                      &Urb->UrbHeader,
                                      sizeof(struct _URB_HEADER));

        // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
        //
        goto USBPORT_ProcessURB_Done;
    }

    // The Device Handle is valid and has now been referenced.
    //
    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'dURB', Urb, Irp, function);

    USBPORT_ASSERT(NT_SUCCESS(ntStatus));

    if (UrbDispatchTable[function].Flags & USBPORT_REQUEST_IS_TRANSFER) {

        // This URB function is a transfer request.  Reference the URB
        // as a TRANSFER_URB to access the TransferBufferLength,
        // TransferBuffer, TransferBufferMDL, etc. fields.
        //
        transferUrb = (PTRANSFER_URB) Urb;

        // If this URB function always uses the default pipe set the
        // Pipe Handle in the URB to the default pipe.
        //
        if (UrbDispatchTable[function].Flags & USBPORT_REQUEST_USES_DEFAULT_PIPE) {

            transferUrb->UsbdPipeHandle = &deviceHandle->DefaultPipe;

            SET_FLAG(transferUrb->TransferFlags, USBD_DEFAULT_PIPE_TRANSFER);
        }

        // If this URB function is a control transfer and the function
        // driver has set the USBD_DEFAULT_PIPE_TRANSFER flag in the URB
        // then set the Pipe Handle in the URB to the default pipe.
        //
        if (TEST_FLAG(transferUrb->TransferFlags, USBD_DEFAULT_PIPE_TRANSFER) &&
            (function == URB_FUNCTION_CONTROL_TRANSFER ||
             function == URB_FUNCTION_CONTROL_TRANSFER_EX)) {

            transferUrb->UsbdPipeHandle = &deviceHandle->DefaultPipe;
        }

        // Linked URBs are not supported and the UrbLink field must be
        // NULL.  Special case for URB_FUNCTION_CONTROL_TRANSFER_EX
        // which redefined the UrbLink field as the Timeout field.
        //
        if (transferUrb->ReservedMBNull != NULL &&
            function != URB_FUNCTION_CONTROL_TRANSFER_EX) {

            ntStatus = SET_USBD_ERROR(transferUrb,
                                      USBD_STATUS_INVALID_PARAMETER);

            // Log the non-null UrbLink error.
            // template: tid_USBPORT_Dispatch_Urb_Device_Invalid_Header
            //
            USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                          deviceHandle,
                                          &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_URB_LINK,
                                          activityId,

                                          &Irp,
                                          sizeof(PIRP),

                                          &Urb,
                                          sizeof(PURB),

                                          &Urb->UrbHeader,
                                          sizeof(struct _URB_HEADER));

            // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
            //
            goto USBPORT_ProcessURB_Done;
        }

        // Zero out the transfer context field now in case the
        // function driver is recycling the URB.
        //
        transferUrb->pd.HcdTransferContext = NULL;

        // A control transfer with no data phase has no transfer buffer.
        //
        if (UrbDispatchTable[function].Flags & USBPORT_REQUEST_NO_DATA_PHASE) {
            transferUrb->TransferBuffer = NULL;
            transferUrb->TransferBufferMDL = NULL;
            transferUrb->TransferBufferLength = 0;
        }

        // Retrieve the pipe handle from the URB.
        //
        pipeHandle = transferUrb->UsbdPipeHandle;

        // Validate the pipe handle from the URB.
        //
        if (!USBPORT_ValidatePipeHandle(deviceHandle, pipeHandle)) {

            ntStatus = SET_USBD_ERROR(transferUrb,
                                      USBD_STATUS_INVALID_PIPE_HANDLE);

            // Log the invalid pipe handle error.
            // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
            //
            USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                          deviceHandle,
                                          &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_PIPE_HANDLE,
                                          activityId,

                                          &Irp,
                                          sizeof(PIRP),

                                          &Urb,
                                          sizeof(PURB),

                                          Urb,
                                          sizeof(struct _URB_HEADER) +
                                          sizeof(USBD_PIPE_HANDLE));

            // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
            //
            goto USBPORT_ProcessURB_Done;
        }

        // USBPORT_REQUEST_IS_TRANSFER not allowed for USBPORT_PIPE_ZERO_BW
        //
        if (TEST_FLAG(pipeHandle->PipeStateFlags, USBPORT_PIPE_ZERO_BW)) {

            ntStatus = SET_USBD_ERROR(transferUrb,
                                      USBD_STATUS_INVALID_PARAMETER);

            // Log the zero bandwidth pipe handle transfer request error.
            // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
            //
            USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                          deviceHandle,
                                          &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ZERO_BW_PIPE_HANDLE,
                                          activityId,

                                          &Irp,
                                          sizeof(PIRP),

                                          &Urb,
                                          sizeof(PURB),

                                          Urb,
                                          sizeof(struct _URB_HEADER) +
                                          sizeof(USBD_PIPE_HANDLE));

            // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
            //
            goto USBPORT_ProcessURB_Done;
        }

        // Retrieve the endpoint from the pipe handle.
        //
        endpoint = USBPORT_EndpointFromHandle(pipeHandle->EpHandle);
        ASSERT_ENDPOINT(endpoint);

        //
        // Ensure that this URB is valid for this pipe and that the transfer
        // size is valid.
        //

        USBPORT_ASSERT(NT_SUCCESS(ntStatus));

        switch (function) {
        case URB_FUNCTION_CONTROL_TRANSFER:
        case URB_FUNCTION_CONTROL_TRANSFER_EX:
            if (endpoint->Parameters.TransferType != Control) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INVALID_PIPE_HANDLE);

                // Log the invalid control transfer endpoint error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_CONTROL_TRANSFER_ENDPOINT,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
            }

            if (TEST_FLAG(deviceHandle->DeviceFlags, USBPORT_DEVICEFLAG_SUPPORTS_MSOS20_DESCRIPTOR)) {
                PUSB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

                setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)transferUrb->u.SetupPacket;

                if ((setupPacket->bmRequestType.Recipient == 0) &&
                    (setupPacket->bmRequestType.Type == BMREQUEST_VENDOR) &&
                    (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) &&
                    (setupPacket->bRequest == deviceHandle->MsOs20VendorCode) &&
                    (setupPacket->wValue.W == 0) &&
                    (setupPacket->wIndex.W == MSOS20_DESCRIPTOR_INDEX)) {

                    relaxControlMaxTransferSize = TRUE;
                }
            }

            if (transferUrb->TransferBufferLength > USBPORT_MAX_CONTROL_TRANSFER_SIZE) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INVALID_PARAMETER);

                // Log the invalid control transfer buffer length
                // error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_CONTROL_TRANSFER_BUFFER_LENGTH,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
            }
            break;

        case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
            if (endpoint->Parameters.TransferType != Bulk &&
                endpoint->Parameters.TransferType != Interrupt) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INVALID_PIPE_HANDLE);

                // Log the invalid bulk or interrupt transfer endpoint
                // error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Bulk_Or_Interrupt_Transfer
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_BULK_OR_INTERRUPT_TRANSFER_ENDPOINT,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    Urb,
                    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));
            }

            if ((endpoint->Parameters.TransferType == Bulk &&
                 transferUrb->TransferBufferLength > USBPORT_MAX_BULK_TRANSFER_SIZE) ||
                (endpoint->Parameters.TransferType == Interrupt &&
                 transferUrb->TransferBufferLength > USBPORT_MAX_INTERRUPT_TRANSFER_SIZE)) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INVALID_PARAMETER);

                // Log the invalid bulk or interrupt transfer buffer
                // length error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Bulk_Or_Interrupt_Transfer
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_BULK_OR_INTERRUPT_TRANSFER_BUFFER_LENGTH,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    Urb,
                    sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));
            }
            break;

        case URB_FUNCTION_ISOCH_TRANSFER:
            if (endpoint->Parameters.TransferType != Isochronous) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INVALID_PIPE_HANDLE);

                // Log the invalid isoch transfer endpoint error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Isoch_Transfer
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ISOCHRONOUS_TRANSFER_ENDPOINT,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    Urb,
                    sizeof(struct _URB_ISOCH_TRANSFER) -
                    sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR));
            }
            break;
        }

        if (!NT_SUCCESS(ntStatus)) {

            // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
            //
            goto USBPORT_ProcessURB_Done;
        }

        USBPORT_ASSERT(NT_SUCCESS(ntStatus));

        if (transferUrb->TransferBufferLength != 0) {

            // If the TransferBufferLength is non-zero then either a
            // TransferBuffer or a TransferBufferMDL is required.
            //
            if (transferUrb->TransferBuffer    == NULL &&
                transferUrb->TransferBufferMDL == NULL) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INVALID_PARAMETER);


                // Log the invalid null transfer buffer and mdl error.
                // template: tid_USBPORT_Dispatch_Urb_Endpoint_Header
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_NULL_TRANSFER_BUFFER_AND_MDL,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    &Urb->UrbHeader,
                    sizeof(struct _URB_HEADER));

                goto USBPORT_ProcessURB_Done;
            }

        } else {

            // If the TransferBufferLength is zero then a
            // TransferBufferMDL is not allowed.  (Is a non-NULL
            // TransferBuffer allowed in this case for some legacy
            // reason?)
            //
            if (transferUrb->TransferBufferMDL != NULL) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INVALID_PARAMETER);

                // Log the invalid non-null transfer buffer mdl error.
                // template: tid_USBPORT_Dispatch_Urb_Endpoint_Header
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_NON_NULL_TRANSFER_BUFFER_MDL,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    &Urb->UrbHeader,
                    sizeof(struct _URB_HEADER));

                goto USBPORT_ProcessURB_Done;
            }
        }

        if (Control == endpoint->Parameters.TransferType) {

            if (((relaxControlMaxTransferSize == TRUE) &&
                 (transferUrb->TransferBufferLength > USBPORT_MAX_CONTROL_TRANSFER_SIZE)) ||
                (transferUrb->TransferBufferLength > EP_MAX_TRANSFER(endpoint))) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INVALID_PARAMETER);

                // Log the invalid control transfer buffer length
                // error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_CONTROL_TRANSFER_BUFFER_LENGTH,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    Urb,
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

                goto USBPORT_ProcessURB_Done;
            }
        }

        // If a TransferBufferMDL is not specified and only a non-paged
        // pool TransferBuffer is specified then allocate and build an
        // MDL for the non-paged pool TransferBuffer.
        //
        if (transferUrb->TransferBufferMDL    == NULL &&
            transferUrb->TransferBufferLength != 0) {

#ifdef USB_FORCE_MEMORY_STRESS
        // Fail the allocation when low-res simulation is enabled.
        //
        if (!FP_FAIL_RESOURCE_ALLOCATION(devExt, FALSE)) {
#endif
            transferUrb->TransferBufferMDL = IoAllocateMdl(
                transferUrb->TransferBuffer,
                transferUrb->TransferBufferLength,
                FALSE,
                FALSE,
                NULL);

#ifdef USB_FORCE_MEMORY_STRESS
        }
#endif

            // Fall back to the reserved resource
            if (transferUrb->TransferBufferMDL == NULL) {
                transferUrb->TransferBufferMDL =
                    FPAllocMdl(devExt,
                               deviceHandle,
                               endpoint,
                               transferUrb->TransferFlags,
                               transferUrb->TransferBufferLength);

                if (transferUrb->TransferBufferMDL) {
                    MmInitializeMdl(transferUrb->TransferBufferMDL,
                                    transferUrb->TransferBuffer,
                                    transferUrb->TransferBufferLength);
                }
            }

            if (transferUrb->TransferBufferMDL == NULL) {

                ntStatus = SET_USBD_ERROR(transferUrb,
                                          USBD_STATUS_INSUFFICIENT_RESOURCES);

                LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'URB?', Urb, Irp, function);

                // Log the mdl allocation failure.
                // template: tid_USBPORT_Dispatch_Urb_Endpoint_Header
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_ALLOCATE_MDL_FAILURE,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    &Urb->UrbHeader,
                    sizeof(struct _URB_HEADER));

                goto USBPORT_ProcessURB_Done;

            } else {

                // Set USBPORT_REQUEST_MDL_ALLOCATED to indicate that
                // the MDL was not allocated by the function driver and
                // the MDL must be freed when the Irp and URB are
                // completed.
                //
                // USBPORT_REQUEST_MDL_ALLOCATED is propagated to
                // USBPORT_TXFLAG_MDL_ALLOCATED when
                // USBPORT_Core_QueueTransferUrb() is called, which is
                // then checked for when
                // USBPORT_Core_iCompleteDoneTransfer() is called.
                //
                SET_FLAG(transferUrb->Hdr.UsbdFlags,
                         USBPORT_REQUEST_MDL_ALLOCATED);

                MmBuildMdlForNonPagedPool(transferUrb->TransferBufferMDL);
            }
        }

        // The URB request looks valid so far.  Allocate and initialize
        // a HCD_TRANSFER_CONTEXT for the URB.
        //
        {
            USBD_STATUS usbdStatus;

            usbdStatus = USBPORT_Core_AllocTransfer(FdoDeviceObject,
                                                    transferUrb,
                                                    deviceHandle,
                                                    Irp,
                                                    NULL,
                                                    0);

            if (!USBD_SUCCESS(usbdStatus)) {

                ntStatus = SET_USBD_ERROR(transferUrb, usbdStatus);

                LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'URB!',
                    Urb, usbdStatus, ntStatus);

                // Log the transfer context allocation failure.
                // template: tid_USBPORT_Dispatch_Urb_Endpoint_Header
                //
                USBPORT_EtwWriteHcDeviceEndpointData3(
                    FdoDeviceObject,
                    deviceHandle,
                    endpoint,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_ALLOCATE_TRANSFER_CONTEXT_FAILURE,
                    activityId,

                    &Irp,
                    sizeof(PIRP),

                    &Urb,
                    sizeof(PURB),

                    &Urb->UrbHeader,
                    sizeof(struct _URB_HEADER));

                goto USBPORT_ProcessURB_Done;
            }
        }

    } else if (UrbDispatchTable[function].Flags & USBPORT_REQUEST_IS_PIPE_REQUEST) {

        // Validate UrbPipeRequest PipeHandle for:
        //
        //     URB_FUNCTION_ABORT_PIPE
        //     URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL
        //     URB_FUNCTION_SYNC_RESET_PIPE
        //     URB_FUNCTION_SYNC_CLEAR_STALL

        // Retrieve the pipe handle from the URB.
        //
        pipeHandle = (PUSBD_PIPE_HANDLE_I)Urb->UrbPipeRequest.PipeHandle;

        // Validate the pipe handle from the URB.
        //
        if (!USBPORT_ValidatePipeHandle(deviceHandle, pipeHandle)) {

            ntStatus = SET_USBD_ERROR(Urb,
                                      USBD_STATUS_INVALID_PIPE_HANDLE);

            // Log the invalid pipe handle pipe request error.
            // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
            //
            USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                          deviceHandle,
                                          &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_PIPE_HANDLE,
                                          activityId,

                                          &Irp,
                                          sizeof(PIRP),

                                          &Urb,
                                          sizeof(PURB),

                                          Urb,
                                          sizeof(struct _URB_HEADER) +
                                          sizeof(USBD_PIPE_HANDLE));

            // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
            //
            goto USBPORT_ProcessURB_Done;
        }

        if (TEST_FLAG(pipeHandle->PipeStateFlags, USBPORT_PIPE_ZERO_BW)) {

            // USBPORT_REQUEST_IS_PIPE_REQUEST requests are NOPs for
            // USBPORT_PIPE_ZERO_BW endpoint as they do not exist in the
            // host controller schedule and they cannot accept transfer
            // requests.  Just complete these with success.
            //
            ntStatus = SET_USBD_ERROR(Urb,
                                      USBD_STATUS_SUCCESS);

            // Log the zero bandwidth pipe handle nop pipe request warning.
            // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
            //
            USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                          deviceHandle,
                                          &USBPORT_ETW_EVENT_DISPATCH_URB_NOP_ZERO_BW_PIPE_HANDLE_REQUEST,
                                          activityId,

                                          &Irp,
                                          sizeof(PIRP),

                                          &Urb,
                                          sizeof(PURB),

                                          Urb,
                                          sizeof(struct _URB_HEADER) +
                                          sizeof(USBD_PIPE_HANDLE));

            // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
            //
            goto USBPORT_ProcessURB_Done;
        }

        // Retrieve the endpoint from the pipe handle.
        //
        endpoint = USBPORT_EndpointFromHandle(pipeHandle->EpHandle);
        ASSERT_ENDPOINT(endpoint);

        if (endpoint->EpFlags.RootHub) {

            // UrbPipeRequest are not expected for RootHub endpoints.
            // Treat these as NOP requests.  That might not be
            // completely correct behavior for URB_FUNCTION_ABORT_PIPE
            // requests, but USBHUB will never issue a
            // URB_FUNCTION_ABORT_PIPE request for a RootHub endpoint.
            //
            ntStatus = SET_USBD_ERROR(Urb,
                                      USBD_STATUS_SUCCESS);

            // Log the roothub pipe handle nop pipe request warning.
            // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
            //
            USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                          deviceHandle,
                                          &USBPORT_ETW_EVENT_DISPATCH_URB_NOP_ROOTHUB_PIPE_HANDLE_REQUEST,
                                          activityId,

                                          &Irp,
                                          sizeof(PIRP),

                                          &Urb,
                                          sizeof(PURB),

                                          Urb,
                                          sizeof(struct _URB_HEADER) +
                                          sizeof(USBD_PIPE_HANDLE));

            // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
            //
            goto USBPORT_ProcessURB_Done;
        }
    }

    USBPORT_ASSERT(NT_SUCCESS(ntStatus));

    // Make sure that neither the Irp nor the Urb are already pending.
    // If this happens it almost always indicates that a client driver
    // has reused an Irp or Urb while it was still pending.
    //
    USBPORT_Core_DetectActiveUrb(FdoDeviceObject, Irp, Urb);

    //
    // Call our URB dispatch handler for this specific USBDI function
    //
    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'Urb>', 0, function, Irp);

    ntStatus = (UrbDispatchTable[function].UrbHandler)(
                    FdoDeviceObject,
                    Irp,
                    Urb,
                    deviceHandle,
                    pipeHandle,
                    endpoint,
                    activityId);

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'Urb<', ntStatus, function, 0);

    // NOTE: The URB and Irp may be gone at this point if STATUS_PENDING
    // was returned.

USBPORT_ProcessURB_Done:

    if (ntStatus == STATUS_PENDING) {

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'PURB', Urb, Irp, function);

    } else {

        USBPORT_ASSERT(ntStatus != STATUS_INSUFFICIENT_RESOURCES ||
                       !transferUrb ||
                       !TEST_FLAG(transferUrb->TransferFlags, USBD_FORWARD_PROGRESS_TRANSFER));

        // The request was not queued.  Release any allocated resources
        // and complete the irp now.
        //
        if (headerValid) {
#if DBG
            // If there is a USBD error status in the URB then we should
            // return an ntStatus error for the Irp as well.
            //
            if (Urb->UrbHeader.Status != USBD_STATUS_SUCCESS &&
                NT_SUCCESS(ntStatus)) {

                // this is a bug
                USBPORT_ASSERTMSG("USBDSTATUS not aligned with NTSTATUS",FALSE);
            }
#endif
            // If we allocated a transfer structure for the URB then
            // free the transfer structure now.
            //
            if (TEST_FLAG(Urb->UrbHeader.UsbdFlags, USBPORT_TRANSFER_ALLOCATED)) {
                PHCD_TRANSFER_CONTEXT t, tmpTx;
                PUSBOBJ_REF epRefHandle;

                UNREFERENCED_PARAMETER(tmpTx);

                t = USBPORT_Core_UnlinkTransfer(FdoDeviceObject,
                                                (PTRANSFER_URB)Urb,
                                                ((PTRANSFER_URB)Urb)->pd.HcdTransferContext);
#if DBG
                // Scan internal lists for this transfer
                tmpTx = USBPORT_Core_CheckTransferQueues(FdoDeviceObject, t);
                USBPORT_ASSERT(tmpTx == NULL);
#endif

                epRefHandle = t->EpRefHandle;

                // Free pre-allocated SG buffer
                //
                if (t->SGBuffer && !FPFreeSgBuf(devExt,
                                                deviceHandle,
                                                USBPORT_EndpointFromHandle(epRefHandle),
                                                t->SGBuffer)) {
                    UsbFreeToLookaside(t->SGBuffer);
                    t->SGBuffer = NULL;
                }











                // Release transfer context reserved for forward progress
                //
                if (FPFreeTransferContext(devExt,
                                          deviceHandle,
                                          USBPORT_EndpointFromHandle(epRefHandle),
                                          t)) {
                    t = NULL;
                }

                // Release the reference on the endpoint.
                //
                USBPORT_DereferenceEndpoint(FdoDeviceObject, epRefHandle);

                // Free the transfer structure.
                //
                if (t) {
                    UsbFreeToLookaside(t);
                }

            }

            // If an error occurs without calling
            // USBPORT_Core_QueueTransferUrb() then the MDL must be
            // freed.
            //
            if (transferUrb != NULL &&
                TEST_FLAG(transferUrb->Hdr.UsbdFlags,
                          USBPORT_REQUEST_MDL_ALLOCATED)) {

                if (!FPFreeMdl(devExt,
                               deviceHandle,
                               endpoint,
                               transferUrb->TransferBufferMDL)) {
                    IoFreeMdl(transferUrb->TransferBufferMDL);
                }
                transferUrb->TransferBufferMDL = NULL;
            }

            // If the Device Handle was successfully referenced then
            // dereference the Device Handle now.
            //
            if (deviceHandle != NULL) {
                ASSERT_DEVICE_HANDLE(deviceHandle);
                USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                                deviceHandle,
                                                Urb,
                                                DEVH_URB_TAG,
                                                TRUE);
            }
        }

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'Uerr', ntStatus, function, Irp);

        // Complete the Irp with the status code returned by the URB
        // function handler.
        //
        // NOTE: The Irp is completed on behalf of the PDO because that
        // is the Device Object to which the function driver passed the
        // Irp and URB.
        //
        // USBPORT_Core_IoCompleteIrp() sets Irp->IoStatus.Status = NtStatus.
        //
        USBPORT_Core_IoCompleteIrp(PdoDeviceObject, Irp, UsbIoRequest, ntStatus, 0);
    }

    ETW_STOP_AND_LOG_TIME(FdoDeviceObject, execMetrics, PROCESS_URB);
    return ntStatus;
}


NTSTATUS
USBPORT_NotSupported(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandleNotUsed,
    PHCD_ENDPOINT       EndpointNotUsed,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for obsolete URB functions.  No longer supported.

    URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL
    URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL
    URB_FUNCTION_GET_FRAME_LENGTH
    URB_FUNCTION_SET_FRAME_LENGTH

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Request Irp

    Urb - Request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - NULL

    Endpoint - NULL

Return Value:

    STATUS_NOT_SUPPORTED

--*/
{
    SET_USBD_ERROR(Urb, USBD_STATUS_NOT_SUPPORTED);

    // Log the not supported Urb error.
    // template: tid_USBPORT_Dispatch_Urb_Device_Invalid_Header
    //
    USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                  DeviceHandle,
                                  &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_FUNCTION_NOT_SUPPORTED,
                                  ActivityId,

                                  &Irp,
                                  sizeof(PIRP),

                                  &Urb,
                                  sizeof(PURB),

                                  &Urb->UrbHeader,
                                  sizeof(struct _URB_HEADER));

    return STATUS_NOT_SUPPORTED;
}


NTSTATUS
USBPORT_InvalidFunction(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandleNotUsed,
    PHCD_ENDPOINT       EndpointNotUsed,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for reserved URB functions:

    URB_FUNCTION_RESERVED_0X0016
    URB_FUNCTION_RESERVE_0X001D
    URB_FUNCTION_RESERVE_0X002B
    URB_FUNCTION_RESERVE_0X002C
    URB_FUNCTION_RESERVE_0X002D
    URB_FUNCTION_RESERVE_0X002E
    URB_FUNCTION_RESERVE_0X002F
    URB_FUNCTION_RESERVE_0X0033
    URB_FUNCTION_RESERVE_0X0034

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Request Irp

    Urb - Request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - NULL

    Endpoint - NULL

Return Value:

    STATUS_INVALID_PARAMETER

--*/
{
    SET_USBD_ERROR(Urb, USBD_STATUS_INVALID_URB_FUNCTION);

    // Log the invalid function Urb error.
    // template: tid_USBPORT_Dispatch_Urb_Device_Invalid_Header
    //
    USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                  DeviceHandle,
                                  &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_FUNCTION_RESERVED,
                                  ActivityId,

                                  &Irp,
                                  sizeof(PIRP),

                                  &Urb,
                                  sizeof(PURB),

                                  &Urb->UrbHeader,
                                  sizeof(struct _URB_HEADER));

    return STATUS_INVALID_PARAMETER;
}


NTSTATUS
USBPORT_ControlTransfer(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Control transfer URB functions:

    URB_FUNCTION_CONTROL_TRANSFER
    URB_FUNCTION_CONTROL_TRANSFER_EX

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint->Parameters.TransferType validated by USBPORT_ProcessURB()
    as being Control.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PTRANSFER_URB       transferUrb = (PTRANSFER_URB)Urb;
    PCEVENT_DESCRIPTOR  eventDescriptor;

    ASSERT_PIPE_HANDLE(PipeHandle);
    ASSERT_ENDPOINT(Endpoint);

    switch (Urb->UrbHeader.Function) {
        default:
        case URB_FUNCTION_CONTROL_TRANSFER:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER;
            break;
        case URB_FUNCTION_CONTROL_TRANSFER_EX:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CONTROL_TRANSFER_EX;
            break;
    }

    // Log the Control transfer Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
    //
    if (EtwLoggingEnabled) {
        USBPORT_EtwWriteHcDeviceEndpointData3(
            FdoDeviceObject,
            DeviceHandle,
            Endpoint,
            eventDescriptor,
            ActivityId,

            &Irp,
            sizeof(PIRP),

            &Urb,
            sizeof(PURB),

            Urb,
            sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
    }

    USBPORT_Core_QueueTransferUrb(transferUrb);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_BulkOrInterruptTransfer(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Bulk or Interrupt transfer URB function:

    URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint->Parameters.TransferType validated by USBPORT_ProcessURB()
    as being either Bulk or Interrupt.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PTRANSFER_URB transferUrb = (PTRANSFER_URB) Urb;

    ASSERT_PIPE_HANDLE(PipeHandle);
    ASSERT_ENDPOINT(Endpoint);

    // Set the proper transfer direction based on the direction bit
    // stored with the endpoint address.
    //
    if (Endpoint->Parameters.TransferDirection == In) {
        USBPORT_SET_TRANSFER_DIRECTION_IN(transferUrb->TransferFlags);
    } else {
        USBPORT_SET_TRANSFER_DIRECTION_OUT(transferUrb->TransferFlags);
    }

    // Log the Bulk or Interrupt transfer Urb dispatch.
    // template:
    // tid_USBPORT_Dispatch_Urb_Function_Bulk_Or_Interrupt_Transfer
    //

    if (EtwLoggingEnabled) {
        USBPORT_EtwWriteHcDeviceEndpointData3(
            FdoDeviceObject,
            DeviceHandle,
            Endpoint,
            &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER,
            ActivityId,

            &Irp,
            sizeof(PIRP),

            &Urb,
            sizeof(PURB),

            Urb,
            sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));
    }

    USBPORT_Core_QueueTransferUrb(transferUrb);

    return STATUS_PENDING;
}


#define UHCD_ASAP_LATENCY   5

NTSTATUS
USBPORT_IsochTransfer(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Isochronous URB function:

    URB_FUNCTION_ISOCH_TRANSFER

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    NTSTATUS ntStatus;
    PTRANSFER_URB transferUrb = (PTRANSFER_URB) Urb;
    ULONG startFrame, frameCount, p , i, cf, packetCount, maxPacketCount;
    PDEVICE_EXTENSION devExt;
    KIRQL oldIrql;
    BOOLEAN highSpeed = FALSE;
    ULONG period = 1;

#define ABS(x) ( (0 < (x)) ? (x) : (0 - (x)))

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

    DbgTrace((UsbpDebugTrace, "'IsochTransfer\n"));

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'sISO', Urb, 0, 0);

    ASSERT_PIPE_HANDLE(PipeHandle);
    ASSERT_ENDPOINT(Endpoint);

    if (Endpoint->Parameters.DeviceSpeed == HighSpeed) {
        highSpeed = TRUE;
        period = Endpoint->Parameters.Period;
    }

    cf = MPf_Get32BitFrameNumber(devExt);
    LOGENTRY(FdoDeviceObject, LOG_ISO, '>ISO', Urb, period, cf);

    // process an iso transfer request

    // validate the number of packets per urb, USBD validated
    // the the count was less than 256 and some tests rely on this.
    // NOTE that usbport is capable of handling
    // larger requests so we allow larger requests thru an
    // enhanced URB or if the device is high speed.
    maxPacketCount = 255;
    if (highSpeed) {
        // size of schedule
        maxPacketCount = 1024;
    }

    // more validation or 'security'
    // we will just fail this case with error since it doesn't make sense
    //
    if (transferUrb->TransferBufferLength == 0 &&
        transferUrb->TransferBufferMDL == NULL &&
        transferUrb->TransferBuffer == NULL) {

        // this is invalid
        USBPORT_KdPrint((1, "Isoch, no buffer\n"));
        TEST_TRAP();

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'badP', transferUrb, 0, 0);

        ntStatus =
             SET_USBD_ERROR(Urb, USBD_STATUS_INVALID_PARAMETER);

        // Log the invalid zero length isoch transfer error.
        // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Isoch_Transfer
        //
        USBPORT_EtwWriteHcDeviceEndpointData3(
            FdoDeviceObject,
            DeviceHandle,
            Endpoint,
            &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ISOCHRONOUS_ZERO_LENGTH,
            ActivityId,

            &Irp,
            sizeof(PIRP),

            &Urb,
            sizeof(PURB),

            Urb,
            sizeof(struct _URB_ISOCH_TRANSFER) -
            sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR));

        goto USBPORT_IsochTransfer_Done;
    }


    if (transferUrb->u.Isoch.NumberOfPackets == 0 ||
        transferUrb->u.Isoch.NumberOfPackets > maxPacketCount) {

        // this is invalid
        USBPORT_KdPrint((1, "Isoch, numberOfPackets = 0\n"));

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'badF', transferUrb, 0, 0);

        ntStatus =
             SET_USBD_ERROR(Urb, USBD_STATUS_INVALID_PARAMETER);

        // Log the invalid NumberOfPackets isoch transfer error.
        // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Isoch_Transfer
        //
        USBPORT_EtwWriteHcDeviceEndpointData3(
            FdoDeviceObject,
            DeviceHandle,
            Endpoint,
            &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ISOCHRONOUS_NUM_PACKETS,
            ActivityId,

            &Irp,
            sizeof(PIRP),

            &Urb,
            sizeof(PURB),

            Urb,
            sizeof(struct _URB_ISOCH_TRANSFER) -
            sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR));

        goto USBPORT_IsochTransfer_Done;
    }

    // first get the current USB frame number
    cf = MPf_Get32BitFrameNumber(devExt);

    packetCount = transferUrb->u.Isoch.NumberOfPackets;
    if (highSpeed) {
        frameCount = (packetCount * period) / 8;
    } else {
        frameCount = transferUrb->u.Isoch.NumberOfPackets;
    }

    // initialize all packet status codes to 'not_set'
    for (p = 0;
         p < packetCount;
         p++) {

        transferUrb->u.Isoch.IsoPacket[p].Status = USBD_STATUS_NOT_SET;
    }

    // see if ASAP flag is set
    if (TEST_FLAG(transferUrb->TransferFlags,
            USBD_START_ISO_TRANSFER_ASAP)) {
        // Yes,
        // if this is the first transfer on the endpoint
        // AKA pure then set the current frame
        if (Endpoint->EpFlags.Pure) {
            LOGENTRY(FdoDeviceObject, LOG_ISO, 'aspV', Urb, 0, cf);

            // use the same asap latency as the UHCD driver for
            // compatibility
            startFrame =
                Endpoint->NextTransferStartFrame = cf+UHCD_ASAP_LATENCY;
        } else {
            startFrame = Endpoint->NextTransferStartFrame;
            LOGENTRY(FdoDeviceObject, LOG_ISO, 'aspN', Urb, startFrame, cf);

            if (GREATER_THAN(cf, (startFrame + maxPacketCount))) {
                // next asap request out of range, treat this like
                // the pure case instead of erroring out
                LOGENTRY(FdoDeviceObject, LOG_ISO, 'resV', Urb, 0, cf);

                startFrame =
                    Endpoint->NextTransferStartFrame = cf+UHCD_ASAP_LATENCY;
            }
        }

    } else {
        // No,
        // absolute frame number set
        startFrame =
            Endpoint->NextTransferStartFrame =
                transferUrb->u.Isoch.StartFrame;

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'absF', Urb, startFrame, cf);


    }

    LOGENTRY(FdoDeviceObject, LOG_ISO, 'ISsf', Urb, startFrame, cf);

    transferUrb->u.Isoch.StartFrame = startFrame;

#if DBG
    if (!highSpeed) {
        USBPORT_ASSERT(frameCount == packetCount);
    }
#endif
    Endpoint->NextTransferStartFrame += frameCount;

    // now that we have computed a start frame validate it

    if (ABS((LONG)(startFrame - cf)) > USBD_ISO_START_FRAME_RANGE)  {

        // set all iso packet status codes to not_accessed

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'iLAT', Urb, 0, 0);

        for (p = 0;
             p < packetCount;
             p++) {

            USBPORT_ASSERT(transferUrb->u.Isoch.IsoPacket[p].Status  ==
                USBD_STATUS_NOT_SET);

            transferUrb->u.Isoch.IsoPacket[p].Status =
                    USBD_STATUS_ISO_NOT_ACCESSED_LATE;
        }

        ntStatus =
             SET_USBD_ERROR(Urb, USBD_STATUS_BAD_START_FRAME);


        // Log the invalid StartFrame isoch transfer error.
        // template: tid_USBPORT_Dispatch_Urb_Function_Isoch_Transfer_Frame
        //
        USBPORT_EtwWriteHcDeviceEndpointData6(
            FdoDeviceObject,
            DeviceHandle,
            Endpoint,
            &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ISOCHRONOUS_START_FRAME,
            ActivityId,

            &Irp,
            sizeof(PIRP),

            &Urb,
            sizeof(PURB),

            &packetCount,
            sizeof(USHORT),

            Urb,
            sizeof(struct _URB_ISOCH_TRANSFER) -
            sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR),

            &transferUrb->u.Isoch.IsoPacket[0],
            sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR) * packetCount,

            &cf,
            sizeof(ULONG));

    } else {

        // we can transmit at least some of the data

        // set the errors for any packets that got to us too late
        // from the function-driver

        for (i = 0;
             i < frameCount;
             i++) {

            if (GREATER_THAN_EQUAL_TO(cf, (i + startFrame))) {

                if (highSpeed) {
                    ULONG j;
                    p = i*(8/period);

                    for (j=0; j< (8/period); j++) {
                        USBPORT_ASSERT(
                            transferUrb->u.Isoch.IsoPacket[p+j].Status ==
                            USBD_STATUS_NOT_SET);

                        transferUrb->u.Isoch.IsoPacket[p+j].Status =
                            USBD_STATUS_ISO_NOT_ACCESSED_LATE;
                    }
                } else {
                    USBPORT_ASSERT(transferUrb->u.Isoch.IsoPacket[i].Status ==
                        USBD_STATUS_NOT_SET);

                    transferUrb->u.Isoch.IsoPacket[i].Status =
                        USBD_STATUS_ISO_NOT_ACCESSED_LATE;
                }
            }
        }

        if (Endpoint->Parameters.TransferDirection == In) {
            USBPORT_SET_TRANSFER_DIRECTION_IN(transferUrb->TransferFlags);
        } else {
            USBPORT_SET_TRANSFER_DIRECTION_OUT(transferUrb->TransferFlags);
        }

        // Log the Isoch transfer Urb dispatch.
        // template: tid_USBPORT_Dispatch_Urb_Function_Isoch_Transfer
        //
        if (EtwLoggingEnabled) {
            USBPORT_EtwWriteHcDeviceEndpointData5(
                FdoDeviceObject,
                DeviceHandle,
                Endpoint,
                &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_ISOCH_TRANSFER,
                ActivityId,

                &Irp,
                sizeof(PIRP),

                &Urb,
                sizeof(PURB),

                &packetCount,
                sizeof(USHORT),

                Urb,
                sizeof(struct _URB_ISOCH_TRANSFER) -
                sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR),

                &transferUrb->u.Isoch.IsoPacket[0],
                sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR) * packetCount);
        }

        // now queue the urb for processing by HW
        USBPORT_Core_QueueTransferUrb(transferUrb);

        LOGENTRY(FdoDeviceObject, LOG_ISO, 'ISO<',0, 0, 0);

        ntStatus = STATUS_PENDING;
    }

USBPORT_IsochTransfer_Done:

    KeLowerIrql(oldIrql);

    return ntStatus;
}


NTSTATUS
USBPORT_SCT_GetSetDescriptor(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Get and Set Descriptor Control transfer URB
    functions:

    URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE
    URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE
    URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT
    URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT
    URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE
    URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PCEVENT_DESCRIPTOR              eventDescriptor;

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'gsDE', 0, 0, Urb);

    setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)
                  &Urb->UrbControlTransfer.SetupPacket[0];

    // setup common fields
    setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

    setupPacket->bRequest =
        UrbDispatchTable[Urb->UrbHeader.Function].bRequest;
    setupPacket->bmRequestType.Type =
        UrbDispatchTable[Urb->UrbHeader.Function].Type;
    setupPacket->bmRequestType.Dir =
        UrbDispatchTable[Urb->UrbHeader.Function].Direction;
    setupPacket->bmRequestType.Recipient =
        UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
    setupPacket->bmRequestType.Reserved = 0;

    Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;
    if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
        USBPORT_SET_TRANSFER_DIRECTION_IN(Urb->UrbControlTransfer.TransferFlags);
    } else {
        USBPORT_SET_TRANSFER_DIRECTION_OUT(Urb->UrbControlTransfer.TransferFlags);
    }

    switch (Urb->UrbHeader.Function) {
        default:
        case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE;
            break;
        case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE;
            break;
        case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT;
            break;
        case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT;
            break;
        case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE;
            break;
        case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE;
            break;
    }

    // Log the Control transfer Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        eventDescriptor,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    USBPORT_Core_QueueTransferUrb((PTRANSFER_URB)Urb);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_SCT_SetClearFeature(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Set and Clear Feature Control transfer URB
    functions:

    URB_FUNCTION_SET_FEATURE_TO_DEVICE
    URB_FUNCTION_SET_FEATURE_TO_INTERFACE
    URB_FUNCTION_SET_FEATURE_TO_ENDPOINT
    URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE
    URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE
    URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT
    URB_FUNCTION_CLEAR_FEATURE_TO_OTHER
    URB_FUNCTION_SET_FEATURE_TO_OTHER

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PCEVENT_DESCRIPTOR              eventDescriptor;

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'scFE', 0, 0, 0);

    setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)
                  &Urb->UrbControlTransfer.SetupPacket[0];

    // setup common fields
    setupPacket->wLength = 0;

    setupPacket->bmRequestType.Type =
        UrbDispatchTable[Urb->UrbHeader.Function].Type;
    setupPacket->bmRequestType.Dir =
        UrbDispatchTable[Urb->UrbHeader.Function].Direction;
    setupPacket->bmRequestType.Recipient =
        UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
    setupPacket->bmRequestType.Reserved = 0;
    //setupPacket->wValue = Urb->UrbControlFeatureRequest.FeatureSelector;
    //setupPacket->wIndex = Urb->UrbControlFeatureRequest.Index;

    setupPacket->bRequest =
        UrbDispatchTable[Urb->UrbHeader.Function].bRequest;

    Urb->UrbControlTransfer.TransferBufferLength = 0;

    Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;
    if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
        USBPORT_SET_TRANSFER_DIRECTION_IN(Urb->UrbControlTransfer.TransferFlags);
    } else {
        USBPORT_SET_TRANSFER_DIRECTION_OUT( Urb->UrbControlTransfer.TransferFlags);
    }

    switch (Urb->UrbHeader.Function) {
        default:
        case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_FEATURE_TO_DEVICE;
            break;
        case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_FEATURE_TO_INTERFACE;
            break;
        case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_FEATURE_TO_ENDPOINT;
            break;
        case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE;
            break;
        case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE;
            break;
        case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT;
            break;
        case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CLEAR_FEATURE_TO_OTHER;
            break;
        case URB_FUNCTION_SET_FEATURE_TO_OTHER:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SET_FEATURE_TO_OTHER;
            break;
    }

    // Log the Control transfer Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        eventDescriptor,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    USBPORT_Core_QueueTransferUrb((PTRANSFER_URB)Urb);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_SCT_GetStatus(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Set Status Control transfer URB functions:

    URB_FUNCTION_GET_STATUS_FROM_DEVICE
    URB_FUNCTION_GET_STATUS_FROM_INTERFACE
    URB_FUNCTION_GET_STATUS_FROM_ENDPOINT
    URB_FUNCTION_GET_STATUS_FROM_OTHER

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PCEVENT_DESCRIPTOR              eventDescriptor;
    NTSTATUS                        ntStatus;

    setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)
                  &Urb->UrbControlTransfer.SetupPacket[0];

    //
    // setup common fields
    //

    setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;
    setupPacket->wValue.W = 0;
    setupPacket->bmRequestType.Type =
        UrbDispatchTable[Urb->UrbHeader.Function].Type;
    setupPacket->bmRequestType.Dir =
        UrbDispatchTable[Urb->UrbHeader.Function].Direction;
    setupPacket->bmRequestType.Recipient =
        UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
    setupPacket->bmRequestType.Reserved = 0;
    setupPacket->bRequest =
        UrbDispatchTable[Urb->UrbHeader.Function].bRequest;


    // some parameter validation
    if (setupPacket->wLength != 2) {

        ntStatus = SET_USBD_ERROR(Urb,
                                  USBD_STATUS_INVALID_PARAMETER);

        USBPORT_KdPrint((1, "Bad wLength for GetStatus\n"));
        goto USBD_SCT_GetStatus_Done;
    }

    ntStatus = STATUS_PENDING;

    Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;
    if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
        USBPORT_SET_TRANSFER_DIRECTION_IN(Urb->UrbControlTransfer.TransferFlags);
    } else {
        USBPORT_SET_TRANSFER_DIRECTION_OUT(Urb->UrbControlTransfer.TransferFlags);
    }

    switch (Urb->UrbHeader.Function) {
        default:
        case URB_FUNCTION_GET_STATUS_FROM_DEVICE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_STATUS_FROM_DEVICE;
            break;
        case URB_FUNCTION_GET_STATUS_FROM_INTERFACE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_STATUS_FROM_INTERFACE;
            break;
        case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_STATUS_FROM_ENDPOINT;
            break;
        case URB_FUNCTION_GET_STATUS_FROM_OTHER:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_STATUS_FROM_OTHER;
            break;
    }

    // Log the Control transfer Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        eventDescriptor,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    USBPORT_Core_QueueTransferUrb((PTRANSFER_URB)Urb);

USBD_SCT_GetStatus_Done:

    return ntStatus;
}


NTSTATUS
USBPORT_SCT_VendorClassCommand(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Vendor or Class Control transfer URB functions:

    URB_FUNCTION_VENDOR_DEVICE
    URB_FUNCTION_VENDOR_INTERFACE
    URB_FUNCTION_VENDOR_ENDPOINT
    URB_FUNCTION_CLASS_DEVICE
    URB_FUNCTION_CLASS_INTERFACE
    URB_FUNCTION_CLASS_ENDPOINT
    URB_FUNCTION_CLASS_OTHER
    URB_FUNCTION_VENDOR_OTHER

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PCEVENT_DESCRIPTOR              eventDescriptor;
    UCHAR                           direction;

    setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)
                  &Urb->UrbControlTransfer.SetupPacket[0];

    // setup common fields
    setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

    // if a direction was specified in the URB then
    // set direction based on URB transfer flags
    direction = (UCHAR)( (Urb->UrbControlTransfer.TransferFlags &
                          USBD_TRANSFER_DIRECTION_IN) ?
                         BMREQUEST_DEVICE_TO_HOST :
                         BMREQUEST_HOST_TO_DEVICE );

    // note that we override only the Recipient,Dir and Type fields

    setupPacket->bmRequestType.Dir = direction;
    setupPacket->bmRequestType.Type =
        UrbDispatchTable[Urb->UrbHeader.Function].Type;
    setupPacket->bmRequestType.Recipient =
        UrbDispatchTable[Urb->UrbHeader.Function].Recipient;

    Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

    switch (Urb->UrbHeader.Function) {
        default:
        case URB_FUNCTION_VENDOR_DEVICE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_VENDOR_DEVICE;
            break;
        case URB_FUNCTION_VENDOR_INTERFACE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_VENDOR_INTERFACE;
            break;
        case URB_FUNCTION_VENDOR_ENDPOINT:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_VENDOR_ENDPOINT;
            break;
        case URB_FUNCTION_CLASS_DEVICE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CLASS_DEVICE;
            break;
        case URB_FUNCTION_CLASS_INTERFACE:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CLASS_INTERFACE;
            break;
        case URB_FUNCTION_CLASS_ENDPOINT:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CLASS_ENDPOINT;
            break;
        case URB_FUNCTION_CLASS_OTHER:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_CLASS_OTHER;
            break;
        case URB_FUNCTION_VENDOR_OTHER:
            eventDescriptor = &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_VENDOR_OTHER;
            break;
    }

    // Log the Control transfer Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        eventDescriptor,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    USBPORT_Core_QueueTransferUrb((PTRANSFER_URB)Urb);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_GetMSFeatureDescriptor(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Get MS Feature Descriptor Control transfer URB
    function:

    URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'gmFD', 0, 0, Urb);

    setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)
                  &Urb->UrbControlTransfer.SetupPacket[0];

    //
    // setup common fields
    // Recipient, wIndex and wValue have been set by caller
    // bRequest was set by usbhub
    //

    setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

    setupPacket->bmRequestType.Type =
        UrbDispatchTable[Urb->UrbHeader.Function].Type;
    setupPacket->bmRequestType.Dir =
        UrbDispatchTable[Urb->UrbHeader.Function].Direction;


    Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

    if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
        USBPORT_SET_TRANSFER_DIRECTION_IN(Urb->UrbControlTransfer.TransferFlags);
    } else {
        USBPORT_SET_TRANSFER_DIRECTION_OUT(Urb->UrbControlTransfer.TransferFlags);
    }

    // Log the Control transfer Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    USBPORT_Core_QueueTransferUrb((PTRANSFER_URB)Urb);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_SyncResetPipe(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for the Sync Reset Pipe URB function:

    URB_FUNCTION_SYNC_RESET_PIPE

    This resets the host side endpoint status in response to a STALL pid
    or other error condition which resulted in the endpoint being
    halted.  This clears the host side endpoint halt state and readies
    the endpoint to process transfer requests again.  Transfer requests
    are not processed by the host controller hardware while the endpoint
    is halted.

    For Isochronous endpoints this also sets the endpoint "Pure" flag,
    which will cause the starting frame number for Isochronous transfers
    to be reinitialized when the USBD_START_ISO_TRANSFER_ASAP flag is
    used the next time.

    Note: In the original Windows XP and Windows Vista versions of
    this function the endpoint data toggle could also be reset, but only
    if the USBPORT_RESET_DATA_TOGGLE flag is set in the UrbHeader
    UsbdFlags field.  This flag can only ever be set when
    USBPORT_SyncResetPipeAndClearStall() handles a
    URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL request.  A
    standalone handler for URB_FUNCTION_SYNC_RESET_PIPE will never
    reset the endpoint data toggle.

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_PIPE_REQUEST flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_PIPE_REQUEST flag.

Return Value:

    STATUS_SUCCESS

--*/
{
    PDEVICE_EXTENSION   devExt;

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'syrP', Urb, Endpoint, 0);

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_PIPE_HANDLE(PipeHandle);
    ASSERT_ENDPOINT(Endpoint);

    // Log the pipe request Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Pipe_Request
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SYNC_RESET_PIPE,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_PIPE_REQUEST));

    // Set the endpoint "Pure" flag, which will cause the starting frame
    // number for Isochronous transfers to be reinitialized when the
    // USBD_START_ISO_TRANSFER_ASAP flag is used the next time.
    //
    LOGENTRY(FdoDeviceObject, LOG_ISO, 'virg', Urb, Endpoint, 0);
    Endpoint->EpFlags.Pure = 1;

    // Set the endpoint status to ENDPOINT_STATUS_RUN.  This clears the
    // endpoint halted state at the host controller level in the
    // miniport.
    //
    MPf_SetEndpointStatus(devExt, Endpoint, ENDPOINT_STATUS_RUN);

    // MPf_SetEndpointStatus() cannot return any error conditions, so
    // assume everything was successful.
    //
    SET_USBD_ERROR(Urb, USBD_STATUS_SUCCESS);

    return STATUS_SUCCESS;
}


NTSTATUS
USBPORT_SyncClearStall(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for the Sync Clear Stall URB function:

    URB_FUNCTION_SYNC_CLEAR_STALL

    This URB request synchronously sends a Clear Feature Endpoint Stall
    request to the device endpoint.  The host side endpoint state,
    including the data toggle for the endpoint, is NOT modified.

    This will block via KeWaitForSingleObject() in USBPORT_SendCommand()
    and must not be called at DISPATCH level.

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_PIPE_REQUEST flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_PIPE_REQUEST flag.

Return Value:

    Synchronous NTSTATUS result of sending the Clear Feature Endpoint
    Stall request to the device.

--*/
{
    USB_DEFAULT_PIPE_SETUP_PACKET   setupPacket;
    USBD_STATUS                     usbdStatus;
    NTSTATUS                        ntStatus;

    ASSERT_PIPE_HANDLE(PipeHandle);
    ASSERT_ENDPOINT(Endpoint);

    // Log the pipe request Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Pipe_Request
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SYNC_CLEAR_STALL,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_PIPE_REQUEST));

    // Initialize the setup packet for the Clear Feature Endpoint Stall
    // request.
    //
    USBPORT_INIT_SETUP_PACKET(setupPacket,
                              USB_REQUEST_CLEAR_FEATURE,    // bRequest
                              BMREQUEST_HOST_TO_DEVICE,     // Dir
                              BMREQUEST_TO_ENDPOINT,        // Recipient
                              BMREQUEST_STANDARD,           // Type
                              USB_FEATURE_ENDPOINT_STALL,   // wValue
                              Endpoint->Parameters.EndpointAddress, // wIndex
                              0);                           // wLength

    // Send the Clear Feature Endpoint Stall request to the device.
    //
    USBPORT_SendCommand(DeviceHandle,
                        FdoDeviceObject,
                        &setupPacket,
                        NULL,
                        0,
                        NULL,
                        &usbdStatus);

    ntStatus = SET_USBD_ERROR(Urb, usbdStatus);

    return ntStatus;
}


NTSTATUS
USBPORT_SyncResetPipeAndClearStall(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for the Reset Pipe and Clear Stall URB function:

    URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL
    (AKA URB_FUNCTION_RESET_PIPE)

    This URB request peforms the following actions:

    (1) Synchronoulsy sends a Clear Feature Endpoint Stall request to
    the device for a non-Isochronous endpoint.

    (2) Resets the host side endpoint data toggle to DATA0 for
    a non-Isochronous endpoint.

    (3) Sets the endpoint "Pure" flag, which will cause the starting
    frame number for Isochronous endpoint transfers to be reinitialized
    when the USBD_START_ISO_TRANSFER_ASAP flag is used the next time.

    (4) Clears the host side endpoint halted state.

    For a non-Isochronous endpoint this will block via
    KeWaitForSingleObject() in USBPORT_SendCommand() and must not be
    called at DISPATCH level.

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_PIPE_REQUEST flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_PIPE_REQUEST flag.

Return Value:

    An NTSTATUS error if the endpoint cannot be referenced, or
    STATUS_SUCCESS for a Isochronous endpoint, or the synchronous result
    of sending the Clear Feature Endpoint Stall request to the device
    for a non-Isochronous endpoint.

--*/
{
    PDEVICE_EXTENSION   devExt;
    PUSBOBJ_REF         epRefHandle;
    NTSTATUS            ntStatus;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    ASSERT_PIPE_HANDLE(PipeHandle);
    ASSERT_ENDPOINT(Endpoint);

    // Log the pipe request Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Pipe_Request
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_PIPE_REQUEST));

    // Reference the endpoint until this process is complete.
    //
    ntStatus = USBPORT_ReferenceEndpoint(FdoDeviceObject,
                                         Endpoint,
                                         &epRefHandle,
                                         Urb,
                                         EPREF_RESET_PIPE_TAG);

    if (NT_SUCCESS(ntStatus)) {

        // First synchronously send a Clear Feature Endpoint Stall
        // request to the device endpoint.
        //
        if (Endpoint->Parameters.TransferType == Isochronous) {

            // This is a NOP for Isochronous endpoints.
            //
            // The orginal Windows 9x and Windows 2000 stack did not
            // send this request for Isochronous endpoints so we don't
            // either.  Some devices are confused by this.  A client
            // driver may separately send a
            // URB_FUNCTION_SYNC_CLEAR_STALL request to an Isochronous
            // endpoint if this is necessary.
            //
            LOGENTRY(FdoDeviceObject, LOG_ISO, 'iRES', Endpoint, 0, 0);

            ntStatus = SET_USBD_ERROR(Urb, USBD_STATUS_SUCCESS);

        } else {

            USB_DEFAULT_PIPE_SETUP_PACKET   setupPacket;
            USBD_STATUS                     usbdStatus;

            // Indicate that the host side endpoint data toggle should
            // later be reset to DATA0 for a non-Isochronous endpoint.
            //
            SET_FLAG(Urb->UrbHeader.UsbdFlags, USBPORT_RESET_DATA_TOGGLE);

            MPf_SetEndpointDataToggle(devExt, Endpoint, 0);

            // Initialize the setup packet for the Clear Feature Endpoint Stall
            // request.
            //
            USBPORT_INIT_SETUP_PACKET(setupPacket,
                                      USB_REQUEST_CLEAR_FEATURE,    // bRequest
                                      BMREQUEST_HOST_TO_DEVICE,     // Dir
                                      BMREQUEST_TO_ENDPOINT,        // Recipient
                                      BMREQUEST_STANDARD,           // Type
                                      USB_FEATURE_ENDPOINT_STALL,   // wValue
                                      Endpoint->Parameters.EndpointAddress, // wIndex
                                      0);                           // wLength

            // (1) Send the Clear Feature Endpoint Stall request to the
            // device.
            //
            USBPORT_SendCommand(DeviceHandle,
                                FdoDeviceObject,
                                &setupPacket,
                                NULL,
                                0,
                                NULL,
                                &usbdStatus);

            ntStatus = SET_USBD_ERROR(Urb, usbdStatus);
        }

        if (NT_SUCCESS(ntStatus)) {

            // (2) Reset the host side endpoint data toggle to DATA0 at
            // the host controller level in the miniport for a
            // non-Isochronous endpoint.
            //
            if (TEST_FLAG(Urb->UrbHeader.UsbdFlags, USBPORT_RESET_DATA_TOGGLE)) {
                MPf_SetEndpointDataToggle(devExt, Endpoint, 0);
            }

            // (3) Set the endpoint "Pure" flag, which will cause the
            // starting frame number for Isochronous transfers to be
            // reinitialized when the USBD_START_ISO_TRANSFER_ASAP flag
            // is used the next time.
            //
            LOGENTRY(FdoDeviceObject, LOG_ISO, 'virg', Urb, Endpoint, 0);
            Endpoint->EpFlags.Pure = 1;

            // (4) Set the endpoint status to ENDPOINT_STATUS_RUN.  This
            // clears the endpoint halted state at the host controller
            // level in the miniport.
            //
            MPf_SetEndpointStatus(devExt, Endpoint, ENDPOINT_STATUS_RUN);

            // MPf_SetEndpointStatus() cannot return any error
            // conditions, so assume everything was successful.
            //
            ntStatus = SET_USBD_ERROR(Urb, USBD_STATUS_SUCCESS);
        }

        USBPORT_DereferenceEndpoint(FdoDeviceObject, epRefHandle);
    }

    return ntStatus;
}


NTSTATUS
USBPORT_AbortPipe(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Abort Pipe URB function:

    URB_FUNCTION_ABORT_PIPE

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_PIPE_REQUEST flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_PIPE_REQUEST flag.

Return Value:

    STATUS_PENDING - If the URB_FUNCTION_ABORT_PIPE request is
    successfully queued.

--*/
{
    PUSBOBJ_REF epRefHandle;
    NTSTATUS    ntStatus;

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'ARP>', Endpoint, Irp, Urb);

    ASSERT_PIPE_HANDLE(PipeHandle);
    ASSERT_ENDPOINT(Endpoint);

    // Log the pipe request Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Pipe_Request
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_ABORT_PIPE,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_PIPE_REQUEST));

    // Reference the endpoint.  If the device is removed this will fail
    // but all transfers should have already been flushed.
    //
    ntStatus = USBPORT_ReferenceEndpoint(FdoDeviceObject,
                                         Endpoint,
                                         &epRefHandle,
                                         Urb,
                                         EPREF_ABORT_PIPE_TAG);

    if (NT_SUCCESS(ntStatus)) {
        BOOLEAN requestQueued;

        // Initiate the endpoint abort process.  If the request is
        // queued it will complete asynchronously.
        //
        requestQueued = USBPORT_AxQueueAbortRequest(
            FdoDeviceObject,
            Endpoint,
            DeviceHandle,
            Urb,
            Irp,
            NULL);

        if (requestQueued) {

            // USBPORT_ProcessURB() will release the reference on the
            // DeviceHandle and complete the Irp unless STATUS_PENDING
            // is returned here.
            //
            ntStatus = STATUS_PENDING;
        }

        // Release the local endpoint reference.  The endpoint would
        // have been referenced along the USBPORT_AxQueueAbortRequest()
        // path, as necessary.
        //
        USBPORT_DereferenceEndpoint(FdoDeviceObject, epRefHandle);
    }

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'ARP<', Endpoint, Irp, ntStatus);

    return ntStatus;
}


NTSTATUS
USBPORT_SCT_GetInterface(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Get Interface Control transfer URB function:

    URB_FUNCTION_GET_INTERFACE

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

    setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)
                  &Urb->UrbControlTransfer.SetupPacket[0];

    setupPacket->bRequest =
        UrbDispatchTable[Urb->UrbHeader.Function].bRequest;
    setupPacket->bmRequestType.Type =
        UrbDispatchTable[Urb->UrbHeader.Function].Type;
    setupPacket->bmRequestType.Dir =
        UrbDispatchTable[Urb->UrbHeader.Function].Direction;
    setupPacket->bmRequestType.Recipient =
        UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
    setupPacket->bmRequestType.Reserved = 0;
    setupPacket->wValue.W = 0;
    setupPacket->wIndex.W = Urb->UrbControlGetInterfaceRequest.Interface;
    setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

    if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
        USBPORT_SET_TRANSFER_DIRECTION_IN(Urb->UrbControlTransfer.TransferFlags);
    } else {
        USBPORT_SET_TRANSFER_DIRECTION_OUT(Urb->UrbControlTransfer.TransferFlags);
    }
    Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

    // Log the Control transfer Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_INTERFACE,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    USBPORT_Core_QueueTransferUrb((PTRANSFER_URB)Urb);

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_SCT_GetConfiguration(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    PHCD_ENDPOINT       Endpoint,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Get Interface Control transfer URB function:

    URB_FUNCTION_GET_CONFIGURATION

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Transfer request Irp

    Urb - Transfer request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

    Endpoint - Validated by USBPORT_ProcessURB() due to
    USBPORT_REQUEST_IS_TRANSFER flag.

Return Value:

    STATUS_PENDING - If transfer request is successfully queued.

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

    setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)
                  &Urb->UrbControlTransfer.SetupPacket[0];

    setupPacket->bRequest =
        UrbDispatchTable[Urb->UrbHeader.Function].bRequest;
    setupPacket->bmRequestType.Type =
        UrbDispatchTable[Urb->UrbHeader.Function].Type;
    setupPacket->bmRequestType.Dir =
        UrbDispatchTable[Urb->UrbHeader.Function].Direction;
    setupPacket->bmRequestType.Recipient =
        UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
    setupPacket->bmRequestType.Reserved = 0;
    setupPacket->wValue.W = 0;
    setupPacket->wIndex.W = 0;
    setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

    if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
        USBPORT_SET_TRANSFER_DIRECTION_IN(Urb->UrbControlTransfer.TransferFlags);
    } else {
        USBPORT_SET_TRANSFER_DIRECTION_OUT(Urb->UrbControlTransfer.TransferFlags);
    }
    Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

    USBPORT_Core_QueueTransferUrb((PTRANSFER_URB)Urb);

    // Log the Control transfer Urb dispatch.
    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
    //
    USBPORT_EtwWriteHcDeviceEndpointData3(
        FdoDeviceObject,
        DeviceHandle,
        Endpoint,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_CONFIGURATION,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        Urb,
        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    return STATUS_PENDING;
}


NTSTATUS
USBPORT_GetCurrentFrame(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandleNotUsed,
    PHCD_ENDPOINT       EndpointNotUsed,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Dispatch handler for Get Current Frame URB function.

    URB_FUNCTION_GET_CURRENT_FRAME_NUMBER

Arguments:

    FdoDeviceObject - Host Controller Device Object

    Irp - Request Irp

    Urb - Request URB

    DeviceHandle - Always validated and referenced by USBPORT_ProcessURB().

    PipeHandle - NULL

    Endpoint - NULL

Return Value:

    STATUS_SUCCESS

--*/
{
    PDEVICE_EXTENSION   devExt;
    ULONG               cf;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    cf = MPf_Get32BitFrameNumber(devExt);

    LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'Ugcf', Urb, cf, 0);

    Urb->UrbGetCurrentFrameNumber.FrameNumber = cf;

    SET_USBD_ERROR(Urb, USBD_STATUS_SUCCESS);

    // Log the get current frame Urb dispatch.
    // template:
    // tid_USBPORT_Dispatch_Urb_Function_Get_Current_Frame_Number
    //
    USBPORT_EtwWriteHcDeviceData3(
        FdoDeviceObject,
        DeviceHandle,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_GET_CURRENT_FRAME_NUMBER,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        &cf,
        sizeof(ULONG));

    return STATUS_SUCCESS;
}
