/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubinterface.h

Abstract:

    An .h file that the hub driver will need to include to interract with the 
    controller stack.
    
--*/

#ifndef __HUBINTERFACE_H__
#define __HUBINTERFACE_H__

#include "ucxclass.h"

//
// Contexts passed in by to Ucx. 
// Ucx uses these to call back into the hub.
//
DECLARE_HANDLE(USB_HUB_CONTEXT);
DECLARE_HANDLE(USB_DEVICE_CONTEXT);

DEFINE_GUID(GUID_HUB_CONTROLLERSTACK_INTERFACE,             
            0x4fb2b0e1L, 0x5fdd, 0x4dab, 0xa6, 0x00, 0x5e, 0x15, 0x16, 0xbb, 0xd8, 0xa7);

DEFINE_GUID(GUID_HUB_PARENT_INTERFACE,             
            0xa9264c93L, 0xeccd, 0x4b40, 0x92, 0x9b, 0xb3, 0x54, 0xf1, 0xe9, 0xa2, 0xdc);


typedef struct _USBDEVICE_INFO {
    ULONG               Size;
    USB_DEVICE_SPEED    DeviceSpeed;
    ULONG               PortNumber;
    USB_DEVICE_CONTEXT  Context;
} USBDEVICE_INFO, *PUSBDEVICE_INFO;

typedef
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
(*PFN_USBDEVICE_CREATE) (
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        PUSBDEVICE_INFO    UsbDeviceCreateInfo,
    __out 
        UCXUSBDEVICE      *UsbDevice
    );

typedef
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
(*PFN_USBDEVICE_DELETE) (
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice
    );

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(*PFN_USBDEVICE_DISCONNECT) (
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        ULONG              PortNumber
    );

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(*PFN_USBDEVICE_SET_PDO_INFORMATION) (
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice,
    __in
        PDEVICE_OBJECT     Pdo
    );

typedef
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
(*PFN_DEFAULT_ENDPOINT_CREATE) (
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice,
    __in 
        ULONG              MaxPacketSize,
    __out 
        UCXENDPOINT       *DefaultEndpoint
    );

typedef
VOID
(*PFN_ADDRESS0_OWNERSHIP_RELEASE) (
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice
    );

typedef
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
(*PFN_ENDPOINT_CREATE) (
    __in 
        UCXUSBDEVICE                                  Hub,
    __in 
        UCXUSBDEVICE                                  UsbDevice,
    __in 
        PUSB_ENDPOINT_DESCRIPTOR                      EndpointDescriptor,
    __in
        ULONG                                         EndpointDescriptorBufferLength,
    __in_opt
        PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR EndpointCompanionDescriptor,
    __out 
        UCXENDPOINT                                   *Endpoint
    );

typedef
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
(*PFN_ENDPOINT_DELETE) (
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice,
    __in 
        UCXENDPOINT        Endpoint
    );

typedef
VOID
__drv_maxIRQL(DISPATCH_LEVEL)
(*PFN_ENDPOINT_EXPOSED_TO_CLIENT) (
    __in 
        UCXENDPOINT        Endpoint
    );

typedef
_Must_inspect_result_
USBD_PIPE_HANDLE
(*PFN_ENDPOINT_GET_USBD_PIPE_HANDLE) (
    __in 
        UCXENDPOINT        Endpoint
    );

typedef
ULONG
(*PFN_ENDPOINT_GET_MAXIMUM_TRANSFER_SIZE) (
    __in
        UCXENDPOINT        Endpoint
    );
            

typedef
BOOLEAN
(*PFN_IS_DEVICE_DISCONNECTED) (
    __in 
        UCXUSBDEVICE       Device
    );

typedef struct _UCX_FORWARD_PROGRESS_WORKITEM *PUCX_FORWARD_PROGRESS_WORKITEM; 

typedef
__drv_functionClass(UCX_FORWARD_PROGRESS_WORKITEM_CALLBACK)
VOID
UCX_FORWARD_PROGRESS_WORKITEM_CALLBACK (
    __in
        PDEVICE_OBJECT                    DeviceObject,
    __in 
        PVOID                             CallbackContext,
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM    UcxForwardProgressWorkItem
    );

typedef UCX_FORWARD_PROGRESS_WORKITEM_CALLBACK *PUCX_FORWARD_PROGRESS_WORKITEM_CALLBACK;

#define UCX_FORWARD_PROGRESS_WORKITEM_NO_FLAGS 0
#define UCX_FORWARD_PROGRESS_WORKITEM_FLAG_NEED_FLUSH_FUNCTIONALITY 1

typedef struct _UCXCONTROLLER_PRIVCONTEXT *PUCXCONTROLLER_PRIVCONTEXT;

typedef struct _UCX_FORWARD_PROGRESS_WORKITEM_FLAGS {
    ULONG                                   NeedsFlushFunctionality:1;
    ULONG                                   Queued:1;
    ULONG                                   :30;
} UCX_FORWARD_PROGRESS_WORKITEM_FLAGS;

//
// For IOCTL_INTERNAL_USB_ROOTHUB_GET_INFO, HUBUCX_ROOTHUB_INFO is the
// structure shared by HUB and UCX, while only ROOTHUB_INFO is exposed
// to the HCD.
//
typedef struct _HUBUCX_ROOTHUB_INFO {
    //
    // These are used for root-hubs only.
    //
    ROOTHUB_INFO;
    USBD_PIPE_HANDLE                        UsbdInterruptPipeHandle;
} HUBUCX_ROOTHUB_INFO, *PHUBUCX_ROOTHUB_INFO;

C_ASSERT(sizeof(UCX_FORWARD_PROGRESS_WORKITEM_FLAGS) == sizeof(ULONG));

typedef
PUCX_FORWARD_PROGRESS_WORKITEM
(*PFN_FORWARD_PROGRESS_WORKITEM_ALLOCATE) (
    __in
        UCXUSBDEVICE                            Hub,
    __in 
        PDEVICE_OBJECT                          WdmDeviceObject,
    __in
        ULONG                                   Flags
    );

typedef
VOID
(*PFN_FORWARD_PROGRESS_WORKITEM_DELETE) (
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM   UcxForwardProgressWorkItem
    );

typedef enum {
    UCX_FORWARD_PROGRESS_DEFAULT_OPTIONS = 0xcad00000,
    UCX_FORWARD_PROGRESS_NOT_REQUIRED
} UCX_FORWARD_PROGRESS_ENQUEUE_OPTIONS;

typedef
VOID
(*PFN_FORWARD_PROGRESS_WORKITEM_ENQUEUE) (
    __in 
        PUCX_FORWARD_PROGRESS_WORKITEM          UcxForwardProgressWorkitem,
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM_CALLBACK Callback,
    __in_opt
        PVOID                                   CallbackContext,
    __in
        UCX_FORWARD_PROGRESS_ENQUEUE_OPTIONS    ForwardProgressEnqueueOptions
    );

typedef
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
(*PFN_FORWARD_PROGRESS_WORKITEM_FLUSH) (
    __in 
        PUCX_FORWARD_PROGRESS_WORKITEM          UcxForwardProgressWorkitem
); 

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(*PFN_CLEAR_TT_BUFFER) (
    __in 
        USB_HUB_CONTEXT    HubContext,
    __in
        USB_DEVICE_CONTEXT UsbDeviceContext,
    __in 
        UCXENDPOINT        Endpoint,
    __in
        ULONG              EndpointNumber,
    __in
        ULONG              TTportNumber
);

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(*PFN_CLEAR_TT_BUFFER_COMPLETE) (
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXENDPOINT        Endpoint
);

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(*PFN_NO_PING_RESPONSE) (
    __in 
        USB_HUB_CONTEXT    HubContext,
    __in
        USB_DEVICE_CONTEXT UsbDeviceContext
);

typedef 
__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
(*PFN_ARE_SUPERSPEED_STREAMS_SUPPORTED) (
    __in
        UCXUSBDEVICE        Hub
    );
        
typedef struct _CONTROLLER_STOP_IDLE_CONTEXT {
    BOOLEAN powerReferenceAcquired;
} CONTROLLER_STOP_IDLE_CONTEXT, *PCONTROLLER_STOP_IDLE_CONTEXT;

FORCEINLINE 
VOID
CONTROLLER_STOP_IDLE_CONTEXT_INIT(
    PCONTROLLER_STOP_IDLE_CONTEXT StopIdleContext
    )
{
    RtlZeroMemory(StopIdleContext, sizeof(*StopIdleContext));
}

typedef 
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
(*PFN_CONTROLLER_STOP_IDLE_SYNCHRONOUS) (
    __in
        UCXUSBDEVICE                  Hub,
    __in
        PCONTROLLER_STOP_IDLE_CONTEXT StopIdleContext
    );

typedef 
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(*PFN_CONTROLLER_RESUME_IDLE) (
    __in
        UCXUSBDEVICE                  Hub,
    __in
        PCONTROLLER_STOP_IDLE_CONTEXT StopIdleContext
    );

typedef PVOID PFN_USB_DEVICE_GET_PERFORMANCE_INFO;

typedef 
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(*PFN_CONTROLLER_GET_INFO) (
    __in
        UCXUSBDEVICE            Hub,
    __out
        PUCX_CONTROLLER_INFO    ControllerInfo
    );

typedef struct _HUB_CONTROLLERSTACK_INTERFACE {
    
    //
    // Standard unnamed interface structure
    //
    INTERFACE;

    //
    // Parameters to be filled by Hub Driver.
    //
    UCXUSBDEVICE                                Hub;
    USB_HUB_CONTEXT                             HubContext;
    PFN_CLEAR_TT_BUFFER                         ClearTTBuffer;

    //
    // Parameters to be filled by Ucx
    //
    PFN_USBDEVICE_CREATE                        UsbDeviceCreate;
    PFN_USBDEVICE_DELETE                        UsbDeviceDelete;
    PFN_USBDEVICE_DISCONNECT                    UsbDeviceDisconnect;
    PFN_USBDEVICE_SET_PDO_INFORMATION           UsbDeviceSetPdoInformation;
    PFN_DEFAULT_ENDPOINT_CREATE                 DefaultEndpointCreate;
    PFN_ADDRESS0_OWNERSHIP_RELEASE              Address0OwnershipRelease;
    PFN_ENDPOINT_CREATE                         EndpointCreate;
    PFN_ENDPOINT_DELETE                         EndpointDelete;
    PFN_ENDPOINT_EXPOSED_TO_CLIENT              EndpointExposedToClient;
    PFN_ENDPOINT_GET_USBD_PIPE_HANDLE           EndpointGetUsbdPipeHandle;
    PFN_ENDPOINT_GET_MAXIMUM_TRANSFER_SIZE      EndpointGetMaximumTransferSize;
    PFN_USB_DEVICE_GET_PERFORMANCE_INFO         UsbDeviceGetPerformanceInfo;
    PFN_FORWARD_PROGRESS_WORKITEM_ALLOCATE      ForwardProgressWorkItemAllocate;
    PFN_FORWARD_PROGRESS_WORKITEM_DELETE        ForwardProgressWorkItemDelete;
    PFN_FORWARD_PROGRESS_WORKITEM_ENQUEUE       ForwardProgressWorkItemEnqueue;
    PFN_FORWARD_PROGRESS_WORKITEM_FLUSH         ForwardProgressWorkItemFlush;
    PFN_CLEAR_TT_BUFFER_COMPLETE                ClearTTBufferComplete;
    PFN_NO_PING_RESPONSE                        NoPingResponse;
    PFN_IS_DEVICE_DISCONNECTED                  IsDeviceDisconnected;
    PFN_ARE_SUPERSPEED_STREAMS_SUPPORTED        AreSuperspeedStreamsSupported;
    PFN_CONTROLLER_STOP_IDLE_SYNCHRONOUS        ControllerStopIdleSynchronous;
    PFN_CONTROLLER_RESUME_IDLE                  ControllerResumeIdle;
    PFN_CONTROLLER_GET_INFO                     ControllerGetInfo;
} HUB_CONTROLLERSTACK_INTERFACE, *PHUB_CONTROLLERSTACK_INTERFACE;


typedef
__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
(*PFN_WAS_HUB_RESET_ON_RESUME) (
    __in 
        PVOID             Context
    );

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
(*PFN_WAS_DEVICE_PROGRAMMING_LOST_ON_RESUME) (
    __in 
        PVOID             Context
    );

typedef
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
(*PFN_GET_HUB_SYMBOLIC_LINK_NAME) (
    __in 
        PVOID             Context,
    __in 
        PUNICODE_STRING  HubSymbolicLinkName
    );

#define HUB_CONTROLLERSTACK_INTERFACE_VERSION_1_0 1000

typedef struct _HUB_PARENT_INTERFACE {
    
    //
    // Fields to be filled by the Parent
    //
    
    //
    // Standard unnamed interface structure
    //

    INTERFACE;
    
    UCHAR                                      HubDepth;
    UCXUSBDEVICE                               Hub;
    USB_DEVICE_SPEED                           HubSpeed;
    BOOLEAN                                    IsHubWakeCapable;
    BOOLEAN                                    IsEnhancedSuperSpeed;
    PFN_WAS_HUB_RESET_ON_RESUME                WasHubResetOnResume;
    PFN_WAS_DEVICE_PROGRAMMING_LOST_ON_RESUME  WasDeviceProgrammingLostOnResume;
    PVOID                                      ConnectorId;
    USHORT                                     First30PortToMap;
    USHORT                                     Last30PortToMap;
    PFN_GET_HUB_SYMBOLIC_LINK_NAME             GetHubSymbolicLinkName;
    USB_TOPOLOGY_ADDRESS                       HubTopologyAddress;

    
    //
    // The following information is passed
    // from the child FDO to its parent. This
    // is only used for debugging purposes.
    //
    PVOID                                      HubFdoContext;    

} HUB_PARENT_INTERFACE, *PHUB_PARENT_INTERFACE;

#define HUB_PARENT_INTERFACE_VERSION_LATEST 1013


//
// Flags for the URB header flags (UrbHeader->UsbdFlags) field
//

//Legacy (defined in 2.0 stack)
#define USBDFLAGS_UCX_MASK                 0x00FFFFFF
#define USBPORT_REQUEST_IS_TRANSFER        0x00000001
#define USBPORT_REQUEST_MDL_ALLOCATED      0x00000002
#define USBPORT_REQUEST_USES_DEFAULT_PIPE  0x00000004
#define USBPORT_REQUEST_NO_DATA_PHASE      0x00000008
#define USBPORT_RESET_DATA_TOGGLE          0x00000010
#define USBPORT_TRANSFER_ALLOCATED         0x00000020
#define USBPORT_REQUEST_IS_PIPE_REQUEST    0x00000040

//New Flags to be used between Hub and Ucx
#define USBDFLAGS_HUB_MASK                 0xFF000000
#define USBDFLAG_ROOTHUB_PEND_INTERRUPT_TX 0x80000000
#define USBDFLAG_ALLOW_LARGE_TRANSFER      0x40000000


#endif
