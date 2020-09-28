/*++
Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    urb.c

Abstract:

    This file has been copied from usbport directory.
    The file has been copied to ensure compatibility in handling urb requests.

Environment:

    kernel mode only

Notes:

Revision History:

--*/

#include "pch.h"

#include "urb.tmh"

//
// Macros to set transfer direction flag
//


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
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
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

URB_HANDLER UrbHandler_NotSupported;
URB_HANDLER UrbHandler_InvalidFunction;
URB_HANDLER UrbHandler_GetCurrentFrame;
URB_HANDLER UrbHandler_Legacy_ControlTransfer;
URB_HANDLER UrbHandler_Legacy_ControlTransferEx;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_SCT_GetSetDescriptor;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_SCT_GetStatus;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_SCT_GetConfiguration;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_SCT_GetInterface;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_GetMSFeatureDescriptor;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_BulkOrInterruptTransfer;
URB_HANDLER UrbHandler_USBPORTStyle_Legacy_IsochTransfer;
URB_HANDLER UrbHandler_AbortPipe;

// last supported function
#define URB_FUNCTION_LAST   URB_FUNCTION_RESERVE_0X0034

// last valid function
URB_DISPATCH_ENTRY UrbDispatchTable[URB_FUNCTION_LAST+1] =
{
    //URB_FUNCTION_SELECT_CONFIGURATION
    UrbHandler_NotSupported,
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
    UrbHandler_NotSupported, // Function
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
    UrbHandler_AbortPipe, // Function
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
    UrbHandler_NotSupported,  // Function
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
    UrbHandler_NotSupported, // Function
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
    UrbHandler_NotSupported, // Function
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
    UrbHandler_NotSupported, // Function
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
    UrbHandler_GetCurrentFrame, // Function
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
    UrbHandler_Legacy_ControlTransfer,  // Function
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
    UrbHandler_USBPORTStyle_Legacy_BulkOrInterruptTransfer, // Function
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
    UrbHandler_USBPORTStyle_Legacy_IsochTransfer, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetSetDescriptor, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetSetDescriptor, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetStatus, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetStatus, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetStatus, // Function
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
    UrbHandler_InvalidFunction, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand,  // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand, // Function
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
    UrbHandler_InvalidFunction, // Function
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
    UrbHandler_NotSupported, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetStatus, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetSetDescriptor, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetSetDescriptor, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetConfiguration, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetInterface, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetSetDescriptor, // Function
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
    UrbHandler_USBPORTStyle_Legacy_SCT_GetSetDescriptor, // Function
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
    UrbHandler_USBPORTStyle_Legacy_GetMSFeatureDescriptor, // Function
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
    UrbHandler_InvalidFunction, // Function
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
    UrbHandler_InvalidFunction, // Function
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
    UrbHandler_InvalidFunction, // Function
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
    UrbHandler_InvalidFunction, // Function
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
    UrbHandler_InvalidFunction, // Function
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
    UrbHandler_NotSupported, // Function
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
    UrbHandler_NotSupported, // Function
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
    UrbHandler_Legacy_ControlTransferEx, // Function
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
    UrbHandler_InvalidFunction, // Function
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
    UrbHandler_InvalidFunction, // Function
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

FORCEINLINE
BOOLEAN
Urb_ValidatePipeHandleWithLockAndList(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        PUCXPIPE_INFO                 UcxPipeInfo
    )
{
    PUCXENDPOINT_PRIVCONTEXT currUcxEndpointContext;
    KIRQL                    irql;
    BOOLEAN                  found = FALSE;

    TRY {

        KeAcquireSpinLock(&UcxUsbDeviceContext->InvalidHandleTrakingLock, &irql);

        FOR_ALL_IN_LIST(UCXENDPOINT_PRIVCONTEXT,
                        &UcxUsbDeviceContext->EndpointListHeadForInvalidHandleTracking,
                        EndpointListEntryForInvalidHandleTraking,
                        currUcxEndpointContext) {

            if (UcxPipeInfo == &currUcxEndpointContext->UcxPipeInfo) {
                found = TRUE;
                break;
            }

        }

        KeReleaseSpinLock(&UcxUsbDeviceContext->InvalidHandleTrakingLock, irql);

    } FINALLY {

    }
    return found;
}

BOOLEAN
Urb_ValidatePipeHandle(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        PUCXPIPE_INFO                 UcxPipeInfo
    )
/*++

Routine Description:

    Validates if the input Pipe Handle is valid.

Arguments:

    UcxPipeInfo - Input Pipe Handle

Return Value:

    BOOLEAN : returns true if the pipe handle is valid (and is not a stream )

Comments:

    We reliaze that we are accessing teh memory pointed to by a handle to validate it.

    However we dont want to track a list of all handles, as it is very expensive
    (unless the AllowIoOnInvalidPipeHandles is set)

    Either we will not validate the handle at all, OR, we do this quick check.

--*/
{
    BOOLEAN            valid;
    BOOLEAN            verifierEnabled;
    PUSBDI_HANDLE_DATA currUsbdiHandleData;

    TRY {

        if (UcxPipeInfo == NULL) {
            //
            // Some client driver historically send IO on NULL handle.
            //
            valid = FALSE;
            LEAVE;
        }

        if (UcxPipeInfo->IsStreamPipe) {
            //
            // This routine is only for regular pipes, not for stream pipes
            //
            valid = FALSE;
            LEAVE;
        }

        if (UcxUsbDeviceContext->AllowIoOnInvalidPipeHandles) {
            valid = Urb_ValidatePipeHandleWithLockAndList(UcxUsbDeviceContext, UcxPipeInfo);
        } else {
            valid = (UcxPipeInfo->Self == UcxPipeInfo);
        }

    } FINALLY {

        if (valid == FALSE) {

            //
            // We are out of the hot path. If verifier is enabled, we need to break into the
            // debugger since this is a client driver error.
            //
            verifierEnabled = FALSE;

            FOR_ALL_IN_LIST(USBDI_HANDLE_DATA,
                            &UcxUsbDeviceContext->USBDInterfaceListHead,
                            USBDInterfaceListEntry,
                            currUsbdiHandleData) {

                if (currUsbdiHandleData->UsbVerifierEnabled) {

                    verifierEnabled = TRUE;
                }
            }

            if (verifierEnabled) {

                //
                // Client sent I/O on a NULL or invalid handle with verifier enabled.
                //
                DbgPrint("Client attempted I/O on stale or invalid pipe handle 0x%p",
                          UcxPipeInfo);
#if DBG
                NT_ASSERTMSG("Client attempted I/O on stale or invalid pipe handle", 0);
#else
                if (KdRefreshDebuggerNotPresent() == FALSE) {

                    DbgBreakPoint();
                }
#endif
            }
        }
    }

    return valid;
}

FORCEINLINE
BOOLEAN
Ucx_IsUnalignedChainedMdl(
    __in
        PMDL  Mdl,
    __in
        ULONG TransferBufferLength
    )
/*++

Routine Description:

    Though the USB 2.0 stack does not support chaned MDLs, and would cause memory corruption in
    case a client driver sends one, unless the chained MDL matches the following property:

        Considering only the chain of MDLs needed to satisfy the TransferBufferLength of the
        Urb, the first MDL ends on a page boundary, the last MDL starts on a page boundary,
        and all the intermediate MDLs start and end on a page boundary.

Return Value:

    This routine returns True, if the MDL does not match the above property

--*/
{

    BOOLEAN isUnalignedMdl;
    ULONG   lengthRemaining;
    PMDL    currMdl;

    TRY {

        if (Mdl->Next == NULL) {
            //
            // Due to a bug in NTFS, for a non-chained MDL it is possible to get a request
            // where TransferBufferLength is greater than MmGetMdlByteCount(TransferBufferMdl).
            // So we need to scope this check to only Chained MDLs.
            // More Details in Windows 8 Bugs : 504192
            isUnalignedMdl = FALSE;
            LEAVE;
        }

        lengthRemaining = TransferBufferLength;
        currMdl = Mdl;

        if (lengthRemaining <= MmGetMdlByteCount(currMdl)){
            //
            // Most of the transfers our there would hit this path and thus this function will
            // not cause a performance impact on the most cases.
            //
            isUnalignedMdl = FALSE;
            LEAVE;
        }

        //
        // Check if the first Mdl ends on a page boundary.
        //
        if ((MmGetMdlByteCount(currMdl) + MmGetMdlByteOffset(currMdl)) % PAGE_SIZE != 0){
            isUnalignedMdl = TRUE;
            LEAVE;
        }

        //
        // Move to the next MDL
        //
        lengthRemaining -= MmGetMdlByteCount(currMdl);
        currMdl = currMdl->Next;

        if (currMdl == NULL) {
            isUnalignedMdl = TRUE;
            LEAVE;
        }

        //
        // Loop till you find the last MDL.
        //
        while (lengthRemaining > MmGetMdlByteCount(currMdl)){

            //
            // All intermediate MDLs should start and end at tha page boundary.
            //
            if (MmGetMdlByteOffset(currMdl) != 0 ||
                (MmGetMdlByteCount(currMdl) % PAGE_SIZE) != 0) {
                isUnalignedMdl = TRUE;
                LEAVE;
            }

            //
            // Move to the Next Mdl
            //
            lengthRemaining -= MmGetMdlByteCount(currMdl);
            currMdl = currMdl->Next;

            if (currMdl == NULL) {
                isUnalignedMdl = TRUE;
                LEAVE;
            }

        }

        //
        // The last MDL must start on a page boundary.
        //
        if (MmGetMdlByteOffset(currMdl) != 0){
            isUnalignedMdl = TRUE;
            LEAVE;
        }

        isUnalignedMdl = FALSE;

    } FINALLY {


    }
    return isUnalignedMdl;

}

NTSTATUS
Urb_USBPORTStyle_ProcessURB(
    __in
        WDFDEVICE               RHPdo,
    __in
        PIRP                    Irp,
    __in
        PURB                    Urb
    )
/*++

Routine Description:

    This routine has been adapted from USBPORT_ProcessUrb.

    Processes a URB from a function-driver IRP.

    Essentially what we do here is look at the URB and validate some
    of the the parameters for the function-driver.

Arguments:

    RhPdo - Handle to the WDFDEVICE for the RootHub Pdo

    Irp -  The IOCTL_INTENAL_USB_SUBMIT_URB ioctl

    Urb -  ptr to URB (USB request block)

Return Value:

--*/
{
    USHORT                        function;
    PTRANSFER_URB                 transferUrb;
    UCXUSBDEVICE                  ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT     ucxUsbDeviceContext;
    PUCXPIPE_INFO                 ucxPipeInfo;
    NTSTATUS                      status;
    USBD_STATUS                   usbdStatus;
    WDFQUEUE                      targetWdfQueue;
    BOOLEAN                       fakeSuccess;

    TRY {

        //
        // Assume success for the completion of the Irp and URB.
        //
        status = STATUS_SUCCESS;
        usbdStatus = USBD_STATUS_SUCCESS;
        fakeSuccess = FALSE;

        //
        // Validate that the URB Length is at least the size of a
        // _URB_HEADER structure before accessing any other fields of the
        // URB.
        //
        if (Urb->UrbHeader.Length < sizeof(struct _URB_HEADER)) {

            //
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
                                    NULL,
                                    &Irp,
                                    sizeof(PIRP),
                                    &Urb,
                                    sizeof(PURB),
                                    &Urb->UrbHeader,
                                    sizeof(struct _URB_HEADER));


            // Cannot do this here: LEAVE;

            // headerValid = TRUE;

        } else {

            // headerValid = TRUE;
        }

        //
        // Initialize the URB status to USBD_STATUS_SUCCESS.
        // Some drivers do not initialize the URB status.
        //
        Urb->UrbHeader.Status = USBD_STATUS_SUCCESS;

        //
        // Retrieve the URB_FUNCTION_* from the URB.
        //
        function = Urb->UrbHeader.Function;

        //
        // Initialize the UsbdFlags field of the URB.
        //
        Urb->UrbHeader.UsbdFlags &= ~USBDFLAGS_UCX_MASK;

        //
        // Validate the URB Function.
        //
        if (function > URB_FUNCTION_LAST) {

            usbdStatus = USBD_STATUS_INVALID_URB_FUNCTION;
            status = USBD_STATUS_To_NTSTATUS(usbdStatus);

            //
            // Log the invalid URB function error.
            // template: tid_USBPORT_Dispatch_Urb_Invalid_Header
            //

            USBPORT_EtwWriteHcData3(FdoDeviceObject,
                                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_FUNCTION,
                                    NULL,
                                    &Irp,
                                    sizeof(PIRP),
                                    &Urb,
                                    sizeof(PURB),
                                    &Urb->UrbHeader,
                                    sizeof(struct _URB_HEADER));

            LEAVE;
        }

        //
        // Validate the URB Length based on the URB Function.  We must do
        // this before we touch any of the URB fields outside of the header.
        //
        if (UrbDispatchTable[function].UrbRequestLength &&
            UrbDispatchTable[function].UrbRequestLength != Urb->UrbHeader.Length) {

            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
            status = USBD_STATUS_To_NTSTATUS(usbdStatus);

            //
            // Log the invalid URB length error.
            // template: tid_USBPORT_Dispatch_Urb_Invalid_Header
            //
            USBPORT_EtwWriteHcData3(FdoDeviceObject,
                                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_HEADER_LENGTH,
                                    NULL,
                                    &Irp,
                                    sizeof(PIRP),
                                    &Urb,
                                    sizeof(PURB),
                                    &Urb->UrbHeader,
                                    sizeof(struct _URB_HEADER));

            LEAVE;
        }


        ucxUsbDevice = (UCXUSBDEVICE) Urb->UrbHeader.UsbdDeviceHandle;

        //
        // We found that there are software bus analyzers that load as lower
        // class filters and and then send GetDescriptor down the stack.
        // If it is loaded below the root hub, then no driver would have filled in
        // the device handle.  USBPORT would treat NULL as the handle for the root
        // hub.  In our case, we don't emulate the root hub in the same way, so instead
        // we will just fail this request to prevent a bugcheck.
        //
        if (ucxUsbDevice == NULL) {
            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
            status = USBD_STATUS_To_NTSTATUS(usbdStatus);
            LEAVE;
        }

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);

        //
        // If the device has been disconnected we need to fail this request.
        //

        if (ucxUsbDeviceContext->Disconnected) {

            LOGENTRY(FdoDeviceObject, LOG_TRANSFER, '!URB', Urb, Irp, function);

            usbdStatus = USBD_STATUS_DEVICE_GONE;
            status = USBD_STATUS_To_NTSTATUS(usbdStatus);

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

            LEAVE;
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
        /*
        [USB3 Edit: Commenting the section for now.. need to see if we need this check here
        if (devExt->Fdo.DmTimerFlags.FailUrbs ||
            devExt->Fdo.PowerFlags.MpRestartFailed) {

            USBPORT_KdPrint((1, "'Error: Bad Request to root hub %p\n", FdoDeviceObject));

            LOGENTRY(FdoDeviceObject, LOG_TRANSFER, '!URr', Urb, Irp, function);

            SET_USBD_ERROR(Urb,
                           USBD_STATUS_DEVICE_GONE);

            status = STATUS_NO_SUCH_DEVICE;

            // Log the URB failure due to HC suspend.
            // template: tid_USBPORT_Dispatch_Urb_Device_Invalid_Header
            //
            USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                          deviceHandle,
                                          &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_DUE_TO_HC_SUSPEND,
                                          NULL,
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
        */

        targetWdfQueue = NULL;

        if (UrbDispatchTable[function].Flags & USBPORT_REQUEST_IS_TRANSFER) {

            //
            // This URB function is a transfer request.  Reference the URB
            // as a TRANSFER_URB to access the TransferBufferLength,
            // TransferBuffer, TransferBufferMDL, etc. fields.
            //
            transferUrb = (PTRANSFER_URB) Urb;

            transferUrb->UrbData.ProcessorNumber = KeGetCurrentProcessorNumberEx(NULL);

            //
            // If this URB function always uses the default pipe set the
            // Pipe Handle in the URB to the default pipe.
            //
            if (UrbDispatchTable[function].Flags & USBPORT_REQUEST_USES_DEFAULT_PIPE) {

                transferUrb->UsbdPipeHandle = ucxUsbDeviceContext->DefaultPipeInfo;

                SET_FLAG(transferUrb->TransferFlags, USBD_DEFAULT_PIPE_TRANSFER);
            }

            //
            // If this URB function is a control transfer and the function
            // driver has set the USBD_DEFAULT_PIPE_TRANSFER flag in the URB
            // then set the Pipe Handle in the URB to the default pipe.
            //
            if (TEST_FLAG(transferUrb->TransferFlags, USBD_DEFAULT_PIPE_TRANSFER) &&
                (function == URB_FUNCTION_CONTROL_TRANSFER ||
                 function == URB_FUNCTION_CONTROL_TRANSFER_EX)) {

                transferUrb->UsbdPipeHandle = ucxUsbDeviceContext->DefaultPipeInfo;
            }

            //
            // Linked URBs are not supported and the UrbLink field must be
            // NULL.  Special case for URB_FUNCTION_CONTROL_TRANSFER_EX
            // which redefined the UrbLink field as the Timeout field.
            //
            if ((transferUrb->ReservedMBNull != NULL) &&
                (function != URB_FUNCTION_CONTROL_TRANSFER_EX)) {

                usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                //
                // Log the non-null UrbLink error.
                // template: tid_USBPORT_Dispatch_Urb_Device_Invalid_Header
                //

                USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                              deviceHandle,
                                              &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_URB_LINK,
                                              NULL,
                                              &Irp,
                                              sizeof(PIRP),
                                              &Urb,
                                              sizeof(PURB),
                                              &Urb->UrbHeader,
                                              sizeof(struct _URB_HEADER));

                //
                // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
                //
                LEAVE;
            }

            //
            // A control transfer with no data phase has no transfer buffer.
            //
            if (UrbDispatchTable[function].Flags & USBPORT_REQUEST_NO_DATA_PHASE) {
                transferUrb->TransferBuffer = NULL;
                transferUrb->TransferBufferMDL = NULL;
                transferUrb->TransferBufferLength = 0;
            }

            //
            // Retrieve the pipe handle from the URB.
            //
            ucxPipeInfo = (PUCXPIPE_INFO) transferUrb->UsbdPipeHandle;

            //
            // Validate the pipe handle from the URB.
            //

            if (FALSE == Urb_ValidatePipeHandle(ucxUsbDeviceContext, ucxPipeInfo)) {

                usbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
                status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                //
                // Log the invalid pipe handle error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
                //
                USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                              deviceHandle,
                                              &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_PIPE_HANDLE,
                                              NULL,
                                              &Irp,
                                              sizeof(PIRP),
                                              &Urb,
                                              sizeof(PURB),
                                              Urb,
                                              sizeof(struct _URB_HEADER) +
                                              sizeof(USBD_PIPE_HANDLE));

                //
                // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
                //
                LEAVE;
            }

            //
            // Retrieve the Target Queue
            //
            targetWdfQueue = ucxPipeInfo->Queue;

            //
            // USBPORT_REQUEST_IS_TRANSFER not allowed for USBPORT_PIPE_ZERO_BW
            //

            if (ucxPipeInfo->EndpointType == UcxEndpointTypeZeroBw) {

                usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                //
                // Log the zero bandwidth pipe handle transfer request error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
                //
                USBPORT_EtwWriteHcDeviceData3(
                    FdoDeviceObject,
                    deviceHandle,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ZERO_BW_PIPE_HANDLE,
                    NULL,
                    &Irp,
                    sizeof(PIRP),
                    &Urb,
                    sizeof(PURB),
                    Urb,
                    sizeof(struct _URB_HEADER) +
                    sizeof(USBD_PIPE_HANDLE));

                //
                // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
                //
                LEAVE;
            }

            switch (function) {
            case URB_FUNCTION_CONTROL_TRANSFER:
            case URB_FUNCTION_CONTROL_TRANSFER_EX:
                if (ucxPipeInfo->EndpointTransferType != EndpointTransferTypeControl) {

                    usbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
                    status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                    //
                    // Log the invalid control transfer endpoint error.
                    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
                    //
                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        FdoDeviceObject,
                        deviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_CONTROL_TRANSFER_ENDPOINT,
                        NULL,
                        &Irp,
                        sizeof(PIRP),
                        &Urb,
                        sizeof(PURB),
                        Urb,
                        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

                }

                if (((transferUrb->TransferBufferLength > ucxPipeInfo->MaximumTransferSize) &&
                     (!TEST_FLAG(transferUrb->Hdr.UsbdFlags, USBDFLAG_ALLOW_LARGE_TRANSFER))) ||
                    (transferUrb->TransferBufferLength > MAXIMUM_CONTROL_TRANSFER_SIZE)) {

                    usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                    status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                    //
                    // Log the invalid control transfer buffer length
                    // error.
                    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
                    //

                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        FdoDeviceObject,
                        deviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_CONTROL_TRANSFER_BUFFER_LENGTH,
                        NULL,
                        &Irp,
                        sizeof(PIRP),
                        &Urb,
                        sizeof(PURB),
                        Urb,
                        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

                }
                break;

            case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
                if (ucxPipeInfo->EndpointTransferType != EndpointTransferTypeBulk &&
                    ucxPipeInfo->EndpointTransferType != EndpointTransferTypeInterrupt) {

                    usbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
                    status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                    //
                    // Log the invalid bulk or interrupt transfer endpoint
                    // error.
                    // template: tid_USBPORT_Dispatch_Urb_Function_Bulk_Or_Interrupt_Transfer
                    //

                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        FdoDeviceObject,
                        deviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_BULK_OR_INTERRUPT_TRANSFER_ENDPOINT,
                        NULL,
                        &Irp,
                        sizeof(PIRP),
                        &Urb,
                        sizeof(PURB),
                        Urb,
                        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));

                }

                if (transferUrb->TransferBufferLength > ucxPipeInfo->MaximumTransferSize) {

                    usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                    status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                    //
                    // Log the invalid bulk or interrupt transfer buffer
                    // length error.
                    // template: tid_USBPORT_Dispatch_Urb_Function_Bulk_Or_Interrupt_Transfer
                    //

                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        FdoDeviceObject,
                        deviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_BULK_OR_INTERRUPT_TRANSFER_BUFFER_LENGTH,
                        NULL,
                        &Irp,
                        sizeof(PIRP),
                        &Urb,
                        sizeof(PURB),
                        Urb,
                        sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER));

                }
                break;

            case URB_FUNCTION_ISOCH_TRANSFER:
                if (ucxPipeInfo->EndpointTransferType != EndpointTransferTypeIsochronous) {

                    usbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
                    status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                    //
                    // Log the invalid isoch transfer endpoint error.
                    // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Isoch_Transfer
                    //

                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        FdoDeviceObject,
                        deviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ISOCHRONOUS_TRANSFER_ENDPOINT,
                        NULL,
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

            if (!NT_SUCCESS(status)) {

                //
                // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
                //
                LEAVE;
            }

            if (transferUrb->TransferBufferLength != 0) {

                //
                // If the TransferBufferLength is non-zero then either a
                // TransferBuffer or a TransferBufferMDL is required.
                //
                if (transferUrb->TransferBuffer    == NULL &&
                    transferUrb->TransferBufferMDL == NULL) {

                    usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                    status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                    //
                    // Log the invalid null transfer buffer and mdl error.
                    // template: tid_USBPORT_Dispatch_Urb_Endpoint_Header
                    //
                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        FdoDeviceObject,
                        deviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_NULL_TRANSFER_BUFFER_AND_MDL,
                        NULL,
                        &Irp,
                        sizeof(PIRP),
                        &Urb,
                        sizeof(PURB),
                        &Urb->UrbHeader,
                        sizeof(struct _URB_HEADER));

                    LEAVE;
                }

            } else {

                //
                // If the TransferBufferLength is zero then a
                // TransferBufferMDL is not allowed.  (Is a non-NULL
                // TransferBuffer allowed in this case for some legacy
                // reason?)
                //
                if (transferUrb->TransferBufferMDL != NULL) {

                    usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                    status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                    //
                    // Log the invalid non-null transfer buffer mdl error.
                    // template: tid_USBPORT_Dispatch_Urb_Endpoint_Header
                    //
                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        FdoDeviceObject,
                        deviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_NON_NULL_TRANSFER_BUFFER_MDL,
                        NULL,
                        &Irp,
                        sizeof(PIRP),
                        &Urb,
                        sizeof(PURB),
                        &Urb->UrbHeader,
                        sizeof(struct _URB_HEADER));

                    LEAVE;
                }
            }

            if (ucxPipeInfo->EndpointTransferType == EndpointTransferTypeControl) {

                if (((transferUrb->TransferBufferLength > ucxPipeInfo->MaximumTransferSize) &&
                     (!TEST_FLAG(transferUrb->Hdr.UsbdFlags, USBDFLAG_ALLOW_LARGE_TRANSFER))) ||
                    (transferUrb->TransferBufferLength > MAXIMUM_CONTROL_TRANSFER_SIZE)) {

                    usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                    status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                    // Log the invalid control transfer buffer length
                    // error.
                    // template: tid_USBPORT_Dispatch_Urb_Function_Control_Transfer
                    //
                    USBPORT_EtwWriteHcDeviceEndpointData3(
                        FdoDeviceObject,
                        deviceHandle,
                        endpoint,
                        &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_CONTROL_TRANSFER_BUFFER_LENGTH,
                        NULL,

                        &Irp,
                        sizeof(PIRP),

                        &Urb,
                        sizeof(PURB),

                        Urb,
                        sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

                    LEAVE;
                }
            }

            if (transferUrb->TransferBufferMDL != NULL) {
                if (Ucx_IsUnalignedChainedMdl(transferUrb->TransferBufferMDL,
                                              transferUrb->TransferBufferLength)) {

                    if (ucxUsbDeviceContext->UcxControllerContext->DriverVerifierEnabled) {

                        Usb3_KeBugCheckEx(USB3_VERIFIER_BUGCODE_CLIENT_USED_CHAINED_MDL,
                                          transferUrb->TransferBufferMDL,
                                          transferUrb,
                                          NULL);
                    }

                    //
                    // Due to compat we cannot fail here.
                    //
                }

            }

        } else if (UrbDispatchTable[function].Flags & USBPORT_REQUEST_IS_PIPE_REQUEST) {

            //
            // Validate UrbPipeRequest PipeHandle for:
            //
            //     URB_FUNCTION_ABORT_PIPE
            //     URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL
            //     URB_FUNCTION_SYNC_RESET_PIPE
            //     URB_FUNCTION_SYNC_CLEAR_STALL

            //
            // Retrieve the pipe handle from the URB.
            //
            ucxPipeInfo = (PUCXPIPE_INFO) Urb->UrbPipeRequest.PipeHandle;

            //
            // Validate the pipe handle from the URB.
            //
            if (FALSE == Urb_ValidatePipeHandle(ucxUsbDeviceContext, ucxPipeInfo)) {

                usbdStatus = USBD_STATUS_INVALID_PIPE_HANDLE;
                status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                //
                // Log the invalid pipe handle pipe request error.
                // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
                //
                USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                              deviceHandle,
                                              &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_PIPE_HANDLE,
                                              NULL,
                                              &Irp,
                                              sizeof(PIRP),
                                              &Urb,
                                              sizeof(PURB),
                                              Urb,
                                              sizeof(struct _URB_HEADER) +
                                              sizeof(USBD_PIPE_HANDLE));

                //
                // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
                //
                LEAVE;
            }

            //
            // Retrieve the Target Queue
            //
            targetWdfQueue = ucxPipeInfo->Queue;

            if (ucxPipeInfo->EndpointType == UcxEndpointTypeZeroBw) {

                //
                // USBPORT_REQUEST_IS_PIPE_REQUEST requests are NOPs for
                // USBPORT_PIPE_ZERO_BW endpoint as they do not exist in the
                // host controller schedule and they cannot accept transfer
                // requests.  Just complete these with success.
                //
                fakeSuccess = TRUE;
                usbdStatus = USBD_STATUS_SUCCESS;
                status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                //
                // Log the zero bandwidth pipe handle nop pipe request warning.
                // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Pipe_Request
                //
                USBPORT_EtwWriteHcDeviceData3(
                    FdoDeviceObject,
                    deviceHandle,
                    &USBPORT_ETW_EVENT_DISPATCH_URB_NOP_ZERO_BW_PIPE_HANDLE_REQUEST,
                    NULL,
                    &Irp,
                    sizeof(PIRP),
                    &Urb,
                    sizeof(PURB),
                    Urb,
                    sizeof(struct _URB_HEADER) +
                    sizeof(USBD_PIPE_HANDLE));

                //
                // Device Handle will be dereferenced at USBPORT_ProcessURB_Done.
                //
                LEAVE;
            }
        }

        // Make sure that neither the Irp nor the Urb are already pending.
        // If this happens it almost always indicates that a client driver
        // has reused an Irp or Urb while it was still pending.
        //
        //[USB3 Edit]: Will reinstate this once we have a plan for it.
        //USBPORT_Core_DetectActiveUrb(FdoDeviceObject, Irp, Urb);

        //
        // Mark the urb status to pending. HCD must set this to appropiate status before
        // completing this request.
        //
        Urb->UrbHeader.Status = USBD_STATUS_PENDING;

        //
        // Call our URB dispatch handler for this specific USBDI function
        //
        status = (UrbDispatchTable[function].UrbHandler)(RHPdo,
                                                         Irp,
                                                         Urb,
                                                         targetWdfQueue);

        //
        // At this point we dont own the irp. It has been dispatched.
        //
        Irp = NULL;

        //
        // NOTE: The URB and Irp may be gone at this point if STATUS_PENDING
        // was returned.
        //

    } FINALLY {

        //
        // Request is being completed from the dispatch routine itself
        //

        if ((!NT_SUCCESS(status)) || (fakeSuccess == TRUE)) {
            if (Irp != NULL) {
                //
                // Looks like we took an error path where the Irp was not dispatched.
                // Complete it here, manually.
                //

                Xrb_IoCompleteRequest(Irp, Urb, status, usbdStatus);
            }
        }
    }

    return status;
}

FORCEINLINE
VOID
Urb_FixControlUrbForCompat(
    __in
        PURB                Urb
    )
/*++

Routine Description:

    This routine contains tweaks we need to do in the Urb for compat purposes.

Arguments:

    Urb - Pointer to the URB

Return Value:

    VOID
--*/
{
    TRY {

        //
        // Following code is being ported from: USBPORT_Core_QueueTransferUrb for
        // compatibility reasons.
        // As per it's implmentation, if Urb->UrbControlTransfer.TransferFlags has
        // USBD_DEFAULT_PIPE_TRANSFER flag set, the 2.0 stack changes the urb function code
        // to URB_FUNCTION_CONTROL_TRANSFER
        //
        if (Urb->UrbControlTransfer.TransferFlags & USBD_DEFAULT_PIPE_TRANSFER) {
            Urb->UrbControlTransfer.Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER;
        }

    } FINALLY {

    }
}

NTSTATUS
Urb_LegacyTransferUrbCompletion(
    PDEVICE_OBJECT  DeviceObject,
    PIRP            Irp,
    PVOID           Context
    )
/*++

Routine Description:

    Completion routine while pre-processing an Transfer Urb (bulk, isoch, intr, control) by a
    legacy driver.

Arguments:

    DeviceObject - The Roothub Pdo

    Irp - The pointer to the irp

    Context - TransferUrb.

Return Value:

    STATUS_CONTINUE_COMPLETION

--*/
{
    NTSTATUS                    status;
    USBD_STATUS                 usbdStatus;
    PTRANSFER_URB               transferUrb = (PTRANSFER_URB) Context;
    UCXUSBDEVICE                ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT   ucxUsbDeviceContext;
    PIO_STACK_LOCATION          irpStack;
    PXRB                        xrb;
    PUCXPIPE_INFO               ucxPipeInfo;
    PUCXENDPOINT_PRIVCONTEXT    ucxEndpointContext;
    ULONG                       index;


    UNREFERENCED_PARAMETER(DeviceObject);

    TRY {

        status = Irp->IoStatus.Status;
        usbdStatus = transferUrb->Hdr.Status;

        //
        // For compat set the information to 0
        //
        Irp->IoStatus.Information = 0;


        if (usbdStatus == USBD_STATUS_PENDING) {

            //
            // HCD did not get a chance to even look at the URB.
            //
            NT_ASSERTMSG("Irp status cannot be success", !NT_SUCCESS(status));

            //
            // COMPAT: For compat use error code that is commonly returned by USBPORT in this
            // situation
            //
            transferUrb->Hdr.Status = USBD_STATUS_CANCELED;
            Irp->IoStatus.Status = USBD_STATUS_To_NTSTATUS(USBD_STATUS_CANCELED);
            transferUrb->TransferBufferLength = 0;

            //
            // COMPAT: for an isoch transfer, the USBD status must be set to all 0xFF's if
            // the request was never handled by the HCD. The error count must be set to zero.
            //
            if (transferUrb->Hdr.Function == URB_FUNCTION_ISOCH_TRANSFER) {

                for (index = 0; index < transferUrb->u.Isoch.NumberOfPackets; index++) {
                    transferUrb->u.Isoch.IsoPacket[index].Status = 0xFFFFFFFF;
                }

                transferUrb->u.Isoch.ErrorCount = 0;
            }

            ucxUsbDevice = (UCXUSBDEVICE) transferUrb->Hdr.UsbdDeviceHandle;
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
            ucxUsbDeviceContext->TransferFailureCount++;

        } else {

            if (!USBD_SUCCESS(usbdStatus)) {
                //
                // HCD is responsible for setting the correct USBD_STATUS. For compat, UCX will
                // set the Irp Status as done by the 2.0 stack.
                //

                ucxUsbDevice = (UCXUSBDEVICE) transferUrb->Hdr.UsbdDeviceHandle;
                ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
                ucxUsbDeviceContext->TransferFailureCount++;

                if (1 == InterlockedExchange(&ucxUsbDeviceContext->PendingNoPingResponseError, 0)) {
                    UsbDevice_NoPingResponse(ucxUsbDeviceContext);
                }

                Irp->IoStatus.Status = USBD_STATUS_To_NTSTATUS(usbdStatus);

                //
                // Determine if HCD is prematurely completing a transfer
                //
                ucxPipeInfo = (PUCXPIPE_INFO) transferUrb->UsbdPipeHandle;
                NT_ASSERT(ucxPipeInfo->IsStreamPipe == FALSE);
                ucxEndpointContext = CONTAINING_RECORD(ucxPipeInfo,
                                                       UCXENDPOINT_PRIVCONTEXT,
                                                       UcxPipeInfo);
                NT_ASSERT(ucxEndpointContext->OkToCancelTransfersSyncEvent == 0);

            } else {

                if (!NT_SUCCESS(status)) {
                    //
                    // Even though this case should never happen, handle it gracefully.
                    //
                    NT_ASSERTMSG("USBD_STATUS indicates success, NTSTATUS indicates failure", 0);
                    transferUrb->Hdr.Status = NTSTATUS_To_USBD_STATUS(status);
                }
            }
        }

        if (Irp->PendingReturned) {
            IoMarkIrpPending(Irp);
        }

        irpStack = IoGetCurrentIrpStackLocation(Irp);
        if (IsUrbXrb((PURB)transferUrb, irpStack)) {
            xrb = UrbToXrb((PURB)transferUrb);
            Xrb_OnComplete(xrb);
        }

        Etw_UrbComplete(Irp, transferUrb);

    } FINALLY {

    }

    return STATUS_CONTINUE_COMPLETION;
}

NTSTATUS
UrbHandler_Legacy_ControlTransfer(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for Control transfer URB function:

    URB_FUNCTION_CONTROL_TRANSFER

    This routine handles control transfer to a device sent by a legacy client driver.

    This is pretty mucy a wrapper around UcxForwardIrpToQueueWithCompletion. We need a wrapper since
    UcxForwardIrpToQueueWithCompletion is an inline function and cannot be used as a function
    pointer.

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    Value returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    NTSTATUS                  status;

    TRY {

        Etw_ControlTransferUrbDispatch(Irp, Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);
    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_Legacy_ControlTransferEx(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for Control transfer URB function:

    URB_FUNCTION_CONTROL_TRANSFER_EX

    This routine handles control transfer to a device sent by a legacy client driver.

    This is pretty mucy a wrapper around UcxForwardIrpToQueueWithCompletion. We need a wrapper since
    UcxForwardIrpToQueueWithCompletion is an inline function and cannot be used as a function
    pointer.

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    Value returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    NTSTATUS                  status;

    TRY {

        Etw_ControlTransferExUrbDispatch(Irp, Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);
    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_AbortPipe(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for AbortPipe URB (URB_FUNCTION_ABORT_PIPE)

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    Value returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    NTSTATUS                          status;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;

    UNREFERENCED_PARAMETER(RHPdo);
    UNREFERENCED_PARAMETER(WdfQueue);

    TRY {

        ucxEndpointContext =
            UcxEndpointGetContextFromEndpointPipeInfo(Urb->UrbPipeRequest.PipeHandle);

        ucxControllerContext = ucxEndpointContext->UcxControllerContext;

        if (FALSE == Endpoint_EventAdd(ucxEndpointContext,
                                       UcxEndpointEventAbortUrbFromClient)) {

            //
            // COMPAT:
            // We need to fail the Abort Urb on this thread itself,
            // with STATUS_NO_SUCH_DEVICE
            // This is required to maintain Compatibility with the 2.0 stack
            //

            status = STATUS_NO_SUCH_DEVICE;

            Xrb_IoCompleteRequest(Irp,
                                  Urb,
                                  status,
                                  NTSTATUS_To_USBD_STATUS(status));

            LEAVE;
        }

        IoCsqInsertIrp(&ucxControllerContext->AbortPipeIrpCsq,
                       Irp,
                       &ucxEndpointContext->AbortPipeIrpCsqContext);

        status = STATUS_PENDING;

        Endpoint_EventAdd(ucxEndpointContext, UcxEndpointEventAbortUrbQueued);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_NotSupported(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for obsolete URB functions.  No longer supported.

    URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL
    URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL
    URB_FUNCTION_GET_FRAME_LENGTH
    URB_FUNCTION_SET_FRAME_LENGTH

    In addition, the following URB functions are supported by the new
    stack but they are processed elsewhere prior to reaching
    Urb_USBPORTStyle_ProcessURB().  These URB functions should never
    actually reach this dispatch handler.

    URB_FUNCTION_SELECT_CONFIGURATION
    URB_FUNCTION_SELECT_INTERFACE
    URB_FUNCTION_RESET_PIPE
    URB_FUNCTION_SYNC_RESET_PIPE
    URB_FUNCTION_SYNC_CLEAR_STALL

    All this routine does is fail the incomming Irp.

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    STATUS_NOT_SUPPORTED

--*/
{
    NTSTATUS    status;
    USBD_STATUS usbdStatus;

    UNREFERENCED_PARAMETER(RHPdo);
    UNREFERENCED_PARAMETER(WdfQueue);

    TRY {

        NT_ASSERTMSG("Unsupported Urb", FALSE);
        LogError(FlagUrb,"Unsupported Urb %d, 0x%p", Urb->UrbHeader.Function, Urb);

        // Log the not supported Urb error.
        // template: tid_USBPORT_Dispatch_Urb_Device_Invalid_Header
        //
        USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                      DeviceHandle,
                                      &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_FUNCTION_NOT_SUPPORTED,
                                      NULL,
                                      &Irp,
                                      sizeof(PIRP),
                                      &Urb,
                                      sizeof(PURB),
                                      &Urb->UrbHeader,
                                      sizeof(struct _URB_HEADER));

        usbdStatus = USBD_STATUS_NOT_SUPPORTED;
        status = USBD_STATUS_To_NTSTATUS(usbdStatus);

        Xrb_IoCompleteRequest(Irp, Urb, status, usbdStatus);

    } FINALLY {

    }
    return status;
}

NTSTATUS
UrbHandler_GetCurrentFrame(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for the following Urb:
        URB_FUNCTION_GET_CURRENT_FRAME_NUMBER

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    NT_STATUS
--*/

{
    NTSTATUS                          status;
    ULONG                             cf;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    UCXUSBDEVICE                      ucxUsbDevice;
    USBD_STATUS                       usbdStatus;

    UNREFERENCED_PARAMETER(RHPdo);
    UNREFERENCED_PARAMETER(WdfQueue);

    TRY {

        ucxUsbDevice = Urb->UrbHeader.UsbdDeviceHandle;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;

        status = Controller_GetCurrentFrameNumber(ucxControllerContext, &cf);

        //
        // COMPAT:
        // This should never fail in case of XHCI. For other controllers this might be
        // somewhat a compat issue if this request fails.
        //
        NT_ASSERT(NT_SUCCESS(status));
        if (!NT_SUCCESS(status)) {
            cf = 0;
        }

        Urb->UrbGetCurrentFrameNumber.FrameNumber = cf;

        usbdStatus = NTSTATUS_To_USBD_STATUS(status);

        Xrb_IoCompleteRequest(Irp, Urb, status, usbdStatus);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_InvalidFunction(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
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

    All this routine does is fails the incomming Irp.

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    STATUS_INVALID_PARAMETER

--*/
{
    NTSTATUS    status;
    USBD_STATUS usbdStatus;

    UNREFERENCED_PARAMETER(RHPdo);
    UNREFERENCED_PARAMETER(WdfQueue);

    TRY {

        // Log the invalid function Urb error.
        // template: tid_USBPORT_Dispatch_Urb_Device_Invalid_Header
        //
        USBPORT_EtwWriteHcDeviceData3(FdoDeviceObject,
                                      DeviceHandle,
                                      &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_FUNCTION_RESERVED,
                                      NULL,
                                      &Irp,
                                      sizeof(PIRP),
                                      &Urb,
                                      sizeof(PURB),
                                      &Urb->UrbHeader,
                                      sizeof(struct _URB_HEADER));

        usbdStatus = USBD_STATUS_INVALID_URB_FUNCTION;
        status = USBD_STATUS_To_NTSTATUS(usbdStatus);

        Xrb_IoCompleteRequest(Irp, Urb, status, usbdStatus);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_BulkOrInterruptTransfer(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
   )
/*++

Routine Description:

    Dispatch handler for Bulk or Interrupt transfer URB function:

    URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    status returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    PTRANSFER_URB             transferUrb;
    PUCXPIPE_INFO             pipeInfo;
    NTSTATUS                  status;

    TRY {

        transferUrb = (PTRANSFER_URB) Urb;
        pipeInfo = transferUrb->UsbdPipeHandle;

        // Set the proper transfer direction based on the direction bit
        // stored with the endpoint address.
        //
        if (pipeInfo->IsEndpointDirectionIn) {
            SET_TRANSFER_DIRECTION_IN(&transferUrb->TransferFlags);
        } else {
            SET_TRANSFER_DIRECTION_OUT(&transferUrb->TransferFlags);
        }

        Etw_BulkTransferUrbDispatch(Irp, Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_IsochTransfer(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for Isochronous URB function:

    URB_FUNCTION_ISOCH_TRANSFER

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    status returned by UcxForwardIrpToQueueWithCompletion if the transfer was queued

--*/
{
    PTRANSFER_URB             transferUrb;
    PUCXPIPE_INFO             pipeInfo;
    NTSTATUS                  status;
    USBD_STATUS               usbdStatus;

    TRY {

        transferUrb = (PTRANSFER_URB) Urb;
        pipeInfo = transferUrb->UsbdPipeHandle;

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'sISO', Urb, 0, 0);

        if (transferUrb->TransferBufferLength == 0 &&
            transferUrb->TransferBufferMDL == NULL &&
            transferUrb->TransferBuffer == NULL) {

            // this is invalid
            NT_ASSERTMSG("Isoch, no buffer", FALSE);

            LOGENTRY(FdoDeviceObject, LOG_ISO, 'badP', transferUrb, 0, 0);

            // Log the invalid zero length isoch transfer error.
            // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Isoch_Transfer
            //
            USBPORT_EtwWriteHcDeviceEndpointData3(
                FdoDeviceObject,
                DeviceHandle,
                Endpoint,
                &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ISOCHRONOUS_ZERO_LENGTH,
                NULL,
                &Irp,
                sizeof(PIRP),
                &Urb,
                sizeof(PURB),
                Urb,
                sizeof(struct _URB_ISOCH_TRANSFER) -
                sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR));

            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
            status = USBD_STATUS_To_NTSTATUS(usbdStatus);

            Xrb_IoCompleteRequest(Irp, Urb, status, usbdStatus);

            LEAVE;
        }

        //
        // TODO: PostWin8
        // This check can be built into the endpoint state machine. 
        // If it is an isoch endpoint with a period greater than 8, we keep it
        // in a purged stage. Then this will not affect the hot path. 
        //
        if (pipeInfo->IsochPeriodInMicroFrames > 8) {

            //
            // We only support Period's of 8 or less. 
            //
            transferUrb->u.Isoch.StartFrame = 0;
            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
            status = USBD_STATUS_To_NTSTATUS(usbdStatus);

            Xrb_IoCompleteRequest(Irp, Urb, status, usbdStatus);

            LEAVE;
        }

        if ((transferUrb->u.Isoch.NumberOfPackets == 0) ||
            (((transferUrb->u.Isoch.NumberOfPackets * pipeInfo->IsochPeriodInMicroFrames) % 8) != 0) ||
            (transferUrb->u.Isoch.NumberOfPackets > pipeInfo->MaximumIsochPacketCount)) {

            // this is invalid
            NT_ASSERTMSG("Isoch, numberOfPackets is not valid", FALSE);

            LOGENTRY(FdoDeviceObject, LOG_ISO, 'badF', transferUrb, 0, 0);

            // Log the invalid NumberOfPackets isoch transfer error.
            // template: tid_USBPORT_Dispatch_Urb_Function_Invalid_Isoch_Transfer
            //
            USBPORT_EtwWriteHcDeviceEndpointData3(
                FdoDeviceObject,
                DeviceHandle,
                Endpoint,
                &USBPORT_ETW_EVENT_DISPATCH_URB_INVALID_ISOCHRONOUS_NUM_PACKETS,
                NULL,
                &Irp,
                sizeof(PIRP),
                &Urb,
                sizeof(PURB),
                Urb,
                sizeof(struct _URB_ISOCH_TRANSFER) -
                sizeof(struct _USBD_ISO_PACKET_DESCRIPTOR));
            
            if (((transferUrb->u.Isoch.NumberOfPackets * pipeInfo->IsochPeriodInMicroFrames) % 8) != 0) {
                //
                // Set StartFrame = 0 for compat
                //
                transferUrb->u.Isoch.StartFrame = 0;
            }

            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
            status = USBD_STATUS_To_NTSTATUS(usbdStatus);

            Xrb_IoCompleteRequest(Irp, Urb, status, usbdStatus);

            LEAVE;
        }

        if (pipeInfo->IsEndpointDirectionIn) {
            SET_TRANSFER_DIRECTION_IN(&transferUrb->TransferFlags);
        } else {
            SET_TRANSFER_DIRECTION_OUT(&transferUrb->TransferFlags);
        }

        Etw_IsochTransferUrbDispatch(Irp, Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);

    } FINALLY  {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_SCT_GetSetDescriptor(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
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

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    status returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    NTSTATUS                        status;

    TRY {

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'gsDE', 0, 0, Urb);

        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)&Urb->UrbControlTransfer.SetupPacket[0];

        // setup common fields
        setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

        setupPacket->bRequest = UrbDispatchTable[Urb->UrbHeader.Function].bRequest;
        setupPacket->bmRequestType.Type = UrbDispatchTable[Urb->UrbHeader.Function].Type;
        setupPacket->bmRequestType.Dir = UrbDispatchTable[Urb->UrbHeader.Function].Direction;
        setupPacket->bmRequestType.Recipient = UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
        setupPacket->bmRequestType.Reserved = 0;

        Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

        if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
            SET_TRANSFER_DIRECTION_IN(&Urb->UrbControlTransfer.TransferFlags);
        } else {
            SET_TRANSFER_DIRECTION_OUT(&Urb->UrbControlTransfer.TransferFlags);
        }

        Etw_GetSetDescriptorUrbDispatch(Irp, Urb);

        Urb_FixControlUrbForCompat(Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_SCT_SetClearFeature(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
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

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    status returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    NTSTATUS                        status;

    TRY {

        LOGENTRY(FdoDeviceObject, LOG_TRANSFER, 'scFE', 0, 0, 0);

        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)&Urb->UrbControlTransfer.SetupPacket[0];

        // setup common fields
        setupPacket->wLength = 0;

        setupPacket->bmRequestType.Type = UrbDispatchTable[Urb->UrbHeader.Function].Type;
        setupPacket->bmRequestType.Dir = UrbDispatchTable[Urb->UrbHeader.Function].Direction;
        setupPacket->bmRequestType.Recipient = UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
        setupPacket->bmRequestType.Reserved = 0;

        setupPacket->bRequest = UrbDispatchTable[Urb->UrbHeader.Function].bRequest;

        Urb->UrbControlTransfer.TransferBufferLength = 0;
        Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

        if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
            SET_TRANSFER_DIRECTION_IN(&Urb->UrbControlTransfer.TransferFlags);
        } else {
            SET_TRANSFER_DIRECTION_OUT(&Urb->UrbControlTransfer.TransferFlags);
        }

        Etw_SetClearFeatureUrbDispatch(Irp, Urb);

        Urb_FixControlUrbForCompat(Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);
    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_SCT_GetStatus(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for Set Status Control transfer URB functions:

    URB_FUNCTION_GET_STATUS_FROM_DEVICE
    URB_FUNCTION_GET_STATUS_FROM_INTERFACE
    URB_FUNCTION_GET_STATUS_FROM_ENDPOINT
    URB_FUNCTION_GET_STATUS_FROM_OTHER

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    error status (if failed parameter validation)
    or
    status returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    NTSTATUS                        status;
    USBD_STATUS                     usbdStatus;

    TRY {

        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)&Urb->UrbControlTransfer.SetupPacket[0];

        //
        // setup common fields
        //
        setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;
        setupPacket->wValue.W = 0;
        setupPacket->bmRequestType.Type = UrbDispatchTable[Urb->UrbHeader.Function].Type;
        setupPacket->bmRequestType.Dir = UrbDispatchTable[Urb->UrbHeader.Function].Direction;
        setupPacket->bmRequestType.Recipient = UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
        setupPacket->bmRequestType.Reserved = 0;
        setupPacket->bRequest = UrbDispatchTable[Urb->UrbHeader.Function].bRequest;

        // some parameter validation
        if (setupPacket->wLength != 2) {

            LogError(FlagUrb, "Bad wLength for GetStatus");

            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
            status = USBD_STATUS_To_NTSTATUS(usbdStatus);

            Xrb_IoCompleteRequest(Irp, Urb, status, usbdStatus);
            LEAVE;
        }

        Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

        if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
            SET_TRANSFER_DIRECTION_IN(&Urb->UrbControlTransfer.TransferFlags);
        } else {
            SET_TRANSFER_DIRECTION_OUT(&Urb->UrbControlTransfer.TransferFlags);
        }

        Etw_GetStatusUrbDispatch(Irp, Urb);

        Urb_FixControlUrbForCompat(Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_SCT_VendorClassCommand(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
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

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    status returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    UCHAR                           direction;
    NTSTATUS                        status;

    TRY {

        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)&Urb->UrbControlTransfer.SetupPacket[0];

        // setup common fields
        setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

        // if a direction was specified in the URB then
        // set direction based on URB transfer flags
        direction = (UCHAR)((Urb->UrbControlTransfer.TransferFlags &
                            USBD_TRANSFER_DIRECTION_IN) ?
                            BMREQUEST_DEVICE_TO_HOST :
                            BMREQUEST_HOST_TO_DEVICE);

        // note that we override only the Recipient,Dir and Type fields

        setupPacket->bmRequestType.Dir = direction;
        setupPacket->bmRequestType.Type = UrbDispatchTable[Urb->UrbHeader.Function].Type;
        setupPacket->bmRequestType.Recipient = UrbDispatchTable[Urb->UrbHeader.Function].Recipient;

        Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

        Etw_ClassVendorUrbDispatch(Irp, Urb);

        Urb_FixControlUrbForCompat(Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_GetMSFeatureDescriptor(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for Get MS Feature Descriptor Control transfer URB
    function:

    URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    status returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET setupPacket;
    NTSTATUS                       status;

    TRY {

        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET) &Urb->UrbControlTransfer.SetupPacket[0];

        //
        // setup common fields
        // Recipient, wIndex and wValue have been set by caller
        // bRequest was set by usbhub
        //

        setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

        setupPacket->bmRequestType.Type = UrbDispatchTable[Urb->UrbHeader.Function].Type;
        setupPacket->bmRequestType.Dir = UrbDispatchTable[Urb->UrbHeader.Function].Direction;

        Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

        if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
            SET_TRANSFER_DIRECTION_IN(&Urb->UrbControlTransfer.TransferFlags);
        } else {
            SET_TRANSFER_DIRECTION_OUT(&Urb->UrbControlTransfer.TransferFlags);
        }

        Etw_GetMsFeatureUrbDispatch(Irp, Urb);

        Urb_FixControlUrbForCompat(Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_SCT_GetInterface(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for Get Interface Control transfer URB function:

    URB_FUNCTION_GET_INTERFACE

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    status returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET        setupPacket;
    NTSTATUS                              status;

    TRY {

        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET) &Urb->UrbControlTransfer.SetupPacket[0];

        setupPacket->bRequest = UrbDispatchTable[Urb->UrbHeader.Function].bRequest;
        setupPacket->bmRequestType.Type = UrbDispatchTable[Urb->UrbHeader.Function].Type;
        setupPacket->bmRequestType.Dir = UrbDispatchTable[Urb->UrbHeader.Function].Direction;
        setupPacket->bmRequestType.Recipient = UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
        setupPacket->bmRequestType.Reserved = 0;
        setupPacket->wValue.W = 0;
        setupPacket->wIndex.W = Urb->UrbControlGetInterfaceRequest.Interface;
        setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

        if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
            SET_TRANSFER_DIRECTION_IN(&Urb->UrbControlTransfer.TransferFlags);
        } else {
            SET_TRANSFER_DIRECTION_OUT(&Urb->UrbControlTransfer.TransferFlags);
        }
        Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

        Etw_GetInterfaceUrbDispatch(Irp, Urb);

        Urb_FixControlUrbForCompat(Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);

    } FINALLY {

    }

    return status;
}

NTSTATUS
UrbHandler_USBPORTStyle_Legacy_SCT_GetConfiguration(
    WDFDEVICE           RHPdo,
    PIRP                Irp,
    PURB                Urb,
    WDFQUEUE            WdfQueue
    )
/*++

Routine Description:

    Dispatch handler for Get Interface Control transfer URB function:

    URB_FUNCTION_GET_CONFIGURATION

Arguments:

    RHPdo - The Roothub Pdo

    Irp - The pointer to the IOCTL_INTERNAL_USB_SUBMIT_URB Irp

    Urb - Pointer to the URB

    WdfQueue - The WdfQueue to which this Urb must be forwarded to.

Return Value:

    status returned by UcxForwardIrpToQueueWithCompletion

--*/
{
    PUSB_DEFAULT_PIPE_SETUP_PACKET    setupPacket;
    NTSTATUS                          status;

    TRY {
        setupPacket = (PUSB_DEFAULT_PIPE_SETUP_PACKET)&Urb->UrbControlTransfer.SetupPacket[0];

        setupPacket->bRequest = UrbDispatchTable[Urb->UrbHeader.Function].bRequest;
        setupPacket->bmRequestType.Type = UrbDispatchTable[Urb->UrbHeader.Function].Type;
        setupPacket->bmRequestType.Dir = UrbDispatchTable[Urb->UrbHeader.Function].Direction;
        setupPacket->bmRequestType.Recipient = UrbDispatchTable[Urb->UrbHeader.Function].Recipient;
        setupPacket->bmRequestType.Reserved = 0;
        setupPacket->wValue.W = 0;
        setupPacket->wIndex.W = 0;
        setupPacket->wLength = (USHORT) Urb->UrbControlTransfer.TransferBufferLength;

        if (setupPacket->bmRequestType.Dir == BMREQUEST_DEVICE_TO_HOST) {
            SET_TRANSFER_DIRECTION_IN(&Urb->UrbControlTransfer.TransferFlags);
        } else {
            SET_TRANSFER_DIRECTION_OUT(&Urb->UrbControlTransfer.TransferFlags);
        }

        Urb->UrbControlTransfer.TransferFlags |= USBD_SHORT_TRANSFER_OK;

        Etw_GetConfigurationUrbDispatch(Irp, Urb);

        Urb_FixControlUrbForCompat(Urb);

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Urb_LegacyTransferUrbCompletion,
                                                    Urb,
                                                    WdfQueue);

        NT_ASSERTMSG("Compatibility Alert. Function Expected to return STATUS_PENDING",
                     status == STATUS_PENDING);

    } FINALLY {

    }

    return status;
}
