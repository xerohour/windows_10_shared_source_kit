/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    usbboot.h

Abstract:

    This header contains the information shared by the USB bus stack 
    and USB storage stack in order to support boot-over-usb
    
Environment:

    Kernel mode

History:

--*/

#ifndef   __USB_BOOT_H__
#define   __USB_BOOT_H__

#include <ntdef.h>
#include <usbiodef_priv.h>

#define DUMP_BUFFER_HEADER_SIZE         PAGE_SIZE
#define DUMP_BUFFER_SCRATCH_SIZE        PAGE_SIZE
#define DUMP_DOUBLE_BUFFER_SIZE         (DUMP_MAX_TRANSFER_PAGES * PAGE_SIZE)

#define DUMP_POOL_TAG                   'pmd'

//
// The spec is not clear as to how much time we should wait for the schedule bit 
// in the usbsts register to go to an expected state. 
// Making this timeout a 1 second wait (25 microsec * 40000 = 1 second).
//
#define DUMP_SCHEDULE_RETRY_COUNT       40000

#define DUMP_MAX_TD_SUPPORTED           4
#define DUMP_MAX_PAGES_PER_TD           5

#define DUMP_MAX_TRANSFER_PAGES         (DUMP_MAX_TD_SUPPORTED * DUMP_MAX_PAGES_PER_TD)

#define DUMP_MAX_CONTEXT_COUNT          2

#define DUMP_MAX_ENDPOINT_COUNT         5

#define FILE_DEVICE_USB                 FILE_DEVICE_UNKNOWN

#define IOCTL_INTERNAL_USB_GET_DUMP_DATA  \
                                            CTL_CODE(FILE_DEVICE_USBEX,  \
                                            USB_GET_DUMP_DATA,  \
                                            METHOD_BUFFERED,  \
                                            FILE_ANY_ACCESS)

#define IOCTL_INTERNAL_USB_FREE_DUMP_DATA  \
                                            CTL_CODE(FILE_DEVICE_USBEX,  \
                                            USB_FREE_DUMP_DATA,  \
                                            METHOD_BUFFERED,  \
                                            FILE_ANY_ACCESS)

#define IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS  \
                                            CTL_CODE(FILE_DEVICE_USBEX,  \
                                            USB_NOTIFY_FORWARD_PROGRESS,  \
                                            METHOD_NEITHER,  \
                                            FILE_ANY_ACCESS)

#define DUMP_MAX_CONTEXT_COUNT          2

typedef enum _DUMP_POWER_DOWN_TYPE {
    DumpPowerDownTypeCrashdump = 0,
    DumpPowerDownTypeHibernateWriteStart,
    DumpPowerDownTypeHibernateWriteEnd,
    DumpPowerDownTypeHibernateReadStart,
    DumpPowerDownTypeHibernateReadEnd,
    DumpPowerDownTypeHybridSleep,
    DumpPowerDownTypeMax
} DUMP_POWER_DOWN_TYPE;

/*
    PortPathDepth -
    Depth of hubs through which a device is attached to a host
    controller.
    0: Root hub device embedded in host controller.
    1: Devices attached directly to a root hub port.
    2 - 6: Devices attached through a chain of a hubs at most 5 deep,
    not including root hub.  (See section 4.1.1 Bus Topology in the USB
    Specification Revision 2.0)

    PortPath -
    List of 1-based port numbers on the chain of hubs through which a
    device is attached.  PortPath list has PortPathDepth valid entries.
*/
typedef struct _USB_DUMP_PORT_PATH {

    ULONG   PortPathDepth;
    ULONG   PortPath[6];

} USB_DUMP_PORT_PATH, *PUSB_DUMP_PORT_PATH;

typedef struct _USB_DUMP_DEVICE_ENDPOINT_INFO {

    UCHAR Address;
    ULONG MaximumTransferSize;  // # of Bytes.

} USB_DUMP_DEVICE_ENDPOINT_INFO, *PUSB_DUMP_DEVICE_ENDPOINT_INFO;

//
// This is input to the IOCTL_INTERNAL_USB_GET_DUMP_DATA IOCTL
//
typedef struct _USB_DUMP_DEVICE_INFO {

    ULONG EndpointCount;
    __field_ecount(EndpointCount) PUSB_DUMP_DEVICE_ENDPOINT_INFO EndpointInfo;

    UCHAR ConfigurationId;
    UCHAR InterfaceId;
    UCHAR AlternateSettingId;

    // To be set to NULL by storage client driver.
    
    PVOID DeviceHandle;

} USB_DUMP_DEVICE_INFO, *PUSB_DUMP_DEVICE_INFO;

typedef NTSTATUS
    (__stdcall *PUSB_HC_INITIALIZE_FUNCTION) (
        PVOID ControllerContext
    );

typedef NTSTATUS
    (__stdcall *PUSB_HC_SEND_COMMAND_FUNCTION) (
        PVOID ControllerContext,
        PURB,
        PPHYSICAL_ADDRESS
    );

typedef NTSTATUS
    (__stdcall *PUSB_HC_RESET_DEVICE_FUNCTION) (
        PVOID ControllerContext
    );

typedef NTSTATUS
    (__stdcall *PUSB_HC_CLEANUP_FUNCTION) (
        PVOID ControllerContext
    );

typedef NTSTATUS
    (__stdcall *PUSB_HC_SEND_ASYNC_COMMAND_FUNCTION) (
        PVOID ControllerContext,
        PURB,
        PPHYSICAL_ADDRESS
    );

typedef NTSTATUS
    (__stdcall *PUSB_HC_POLL_FOR_ASYNC_COMMAND_COMPLETION_FUNCTION) (
        PVOID ControllerContext,
        PURB *
    );

typedef struct _EHCI_CRASHDUMP_CONTEXT {

    //
    // EHCI context that contains info to be used at crashdump time
    //

    //
    // USBPORT.SYS's DMA adapter to be used for getting a scatterscatter-gather list for SRB data buffer 
    //
    PDMA_ADAPTER DmaAdapter;

    //
    // Device address
    //
    ULONG DeviceAddress;

    //
    // Device's parent hub address
    //
    ULONG ParentHubDeviceAddress;

    //
    // Pointer to common buffer pool to be used for queue heads, td's, etc.
    //
    PVOID CommonBufferPool;

    //
    // Physical Address for the above buffer
    //
    PHYSICAL_ADDRESS CommonBufferPA;

    //
    // Size of the buffer allocated above
    //
    ULONG CommonBufferSize;

    //
    // Base address for USB registers
    //
    PVOID BaseRegisterPointer;

    //
    // Path to the USB device
    //
    USB_DUMP_PORT_PATH PortPath;

    //
    // USBPORT FDO 
    //
    PDEVICE_OBJECT USBPortFDO;

    //
    // Flag to indicate if hibernate is occurring
    //
    BOOLEAN    Hibernate;

    //
    // Flag to indicate if controller in a transitory state
    //
    BOOLEAN    ControllerTransition;

    //
    // Flag to indicate if the controller can access 64 bit address
    //
    BOOLEAN    Bit64AddressingCapability;

    //
    // Keeping this as 4 for UASP compatibility
    // BOT will only use 2 of these
    //
    USB_DUMP_DEVICE_ENDPOINT_INFO EndpointInfo[DUMP_MAX_ENDPOINT_COUNT];

    //
    // Following 3 flags required to configure the device properly
    //
    UCHAR      InterfaceId;
    UCHAR      AlternateSettingId;
    UCHAR      ConfigurationId;

} EHCI_CRASHDUMP_CONTEXT, *PEHCI_CRASHDUMP_CONTEXT;

//
// This is input to the IOCTL_INTERNAL_USB_GET_DUMP_DATA  and
// IOCTL_INTERNAL_USB_FREE_DUMP_DATA IOCTLs.
//
typedef struct _USB_DUMP_CONTROLLER_INFO {

    // Major version of the structure 
    ULONG MajorVersion;

    // Minor version of the structure 
    ULONG MinorVersion;

    // Device specific data allocated by USB core driver to access the device at crashdump time
    // Note the data structure this field points to is specific to the controller driver and will contain
    // fields for common buffer, etc. to be used at crashdump time.
    PVOID ControllerContext;

    // Pointer to the USB hardware initialize routine
    PUSB_HC_INITIALIZE_FUNCTION ControllerInitialize;

    // Pointer to the routine to send command to the device
    PUSB_HC_SEND_COMMAND_FUNCTION ControllerSendURB;

    // Pointer to the routine to send command to the device asynchronously
    PUSB_HC_SEND_ASYNC_COMMAND_FUNCTION ControllerSendURBAsync;

    //Pointer to the routine to check for completion of Asynchronous command
    PUSB_HC_POLL_FOR_ASYNC_COMMAND_COMPLETION_FUNCTION ControllerPollForCompletion;
    
    // Pointer to the routine to reset the device
    PUSB_HC_RESET_DEVICE_FUNCTION ControllerResetDevice;

    // Pointer to the USB hardware cleanup routine
    PUSB_HC_CLEANUP_FUNCTION ControllerCleanUp;

    //
    // Pointer to common buffer to be used for device specific DMA
    //
    PVOID ControllerCommonBufferVA;

    //
    // Physical Address for the above buffer
    //
    PHYSICAL_ADDRESS ControllerCommonBufferPA;

    //
    // Size of the buffer allocated above
    //
    ULONG ControllerCommonBufferSize;

    //
    // Does this controller support multi-stage resume
    //
    BOOLEAN ControllerSupportsMultistageResume;

} USB_DUMP_CONTROLLER_INFO, *PUSB_DUMP_CONTROLLER_INFO;

typedef struct _USBSTOR_DUMP_DATA
{
    //
    // Specifies the length in bytes
    //
    USHORT Length;

    //
    // Device specific data structure 
    //
    USBD_PIPE_INFORMATION ControlEndpoint;
    USBD_PIPE_INFORMATION BulkInEndpoint;
    USBD_PIPE_INFORMATION BulkOutEndpoint;  

    //
    // URB to handle synchronous transfer
    //
    union
    {
        struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST ControlUrb;
        struct _URB_BULK_OR_INTERRUPT_TRANSFER      BulkIntrUrb;
    } Urb;

   //
   // USB controller interface 
   //
   USB_DUMP_CONTROLLER_INFO ControllerInfo;

} USBSTOR_DUMP_DATA, *PUSBSTOR_DUMP_DATA;

typedef NTSTATUS
    (__stdcall *PUSB_CONTROL_TRANSFER_FUNCTION) (
     PVOID ControllerContext,
     PUCHAR SetupPacket,
     PHYSICAL_ADDRESS Buffer,
     ULONG BufferLength
    );


NTSTATUS
__stdcall
UsbDumpResetDevice(
    __inout PEHCI_CRASHDUMP_CONTEXT ControllerContext,
    __inout PUSB_CONTROL_TRANSFER_FUNCTION ControlTransfer,
    __inout PULONG DeviceAddress
    );

#define USBD_FORWARD_PROGRESS_TRANSFER        0x00000010 

#define USB_FORWARD_PROGRESS_INFO_VERSION_INVALID 0
#define USB_FORWARD_PROGRESS_INFO_VERSION_1 1
#define USB_FORWARD_PROGRESS_INFO_VERSION_MAX 1

typedef struct _USB_FORWARD_PROGRESS_INFO_HEADER {
    ULONG Version;
    ULONG Size;
}USB_FORWARD_PROGRESS_INFO_HEADER, *PUSB_FORWARD_PROGRESS_INFO_HEADER;

typedef struct _USB_PIPE_FORWARD_PROGRESS_INFO {
    USBD_PIPE_HANDLE PipeHandle; // client driver fills this up
    ULONG MaxForwardProgressTransferSize; // e.g., PAGE_SIZE(UASP)/max (BOT). 
                                          // Client can supply a value smaller than 4K 
}USB_PIPE_FORWARD_PROGRESS_INFO, *PUSB_PIPE_FORWARD_PROGRESS_INFO;

#ifndef ANY_SIZE
#define ANY_SIZE 1
#endif

typedef struct _USB_FORWARD_PROGRESS_INFO_V1 {
    USB_FORWARD_PROGRESS_INFO_HEADER Header;   
    PVOID DeviceHandle; // opaque field used by the USB bus stack only
    ULONG ControlPipeMaxForwardProgressTransferSize; // 0 if control pipe doesn't requires FP support
    ULONG NumberOfPipes; // number of pipes on the device that require FP support
    USB_PIPE_FORWARD_PROGRESS_INFO Pipes[ANY_SIZE]; // pipe FP information
}USB_FORWARD_PROGRESS_INFO_V1, *PUSB_FORWARD_PROGRESS_INFO_V1;

#define SIZE_OF_USB_FORWARD_PROGRESS_INFO_V1(numOfPipes) \
    ((sizeof(USB_FORWARD_PROGRESS_INFO_V1) - sizeof(USB_PIPE_FORWARD_PROGRESS_INFO)) + \
    (numOfPipes*sizeof(USB_PIPE_FORWARD_PROGRESS_INFO)))

typedef USB_FORWARD_PROGRESS_INFO_V1 USB_FORWARD_PROGRESS_INFO;
typedef USB_FORWARD_PROGRESS_INFO_V1 *PUSB_FORWARD_PROGRESS_INFO;

#define SIZE_OF_USB_FORWARD_PROGRESS_INFO \
    SIZE_OF_USB_FORWARD_PROGRESS_INFO_V1

#define WINPE_KEY                    L"MiniNT"

#define PW_GP_SYSTEM_FULL_PATH    \
    L"\\Registry\\Machine\\System\\CurrentControlSet\\Policies\\Microsoft\\PortableOperatingSystem"

#define PW_GP_HIBERNATE      L"Hibernate"
#define PW_GP_SLEEP          L"Sleep"

#define PW_GP_HIBERNATE_DEFAULT 0
#define PW_GP_SLEEP_DEFAULT     1
    
#define USB_CEIP_KEY_FULL_PATH    \
    L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Usb\\Ceip"
#define USB_CEIP_KEY_SURPRISE_REMOVAL_COUNT_VALUE   \
    L"BootPathSurpriseRemovalCount"

#ifdef USB_FORWARD_PROGRESS_BUS_INTERFACE

DECLARE_HANDLE(USB_WORKITEM_HANDLE);

typedef VOID (*PUSB_IRP_WORKER_ROUTINE) (
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    );

__drv_functionClass(USB_BUSIFFN_QUEUE_WORKITEM)
typedef
USB_WORKITEM_HANDLE
USB_BUSIFFN
USB_BUSIFFN_ALLOCATE_WORKITEM (    
    __in PDEVICE_OBJECT DeviceObject,
    __in BOOLEAN ForwardProgress
    );

typedef USB_BUSIFFN_ALLOCATE_WORKITEM *PUSB_BUSIFFN_ALLOCATE_WORKITEM;

__drv_functionClass(USB_BUSIFFN_FREE_WORKITEM)
typedef
VOID
USB_BUSIFFN
USB_BUSIFFN_FREE_WORKITEM (    
    __in USB_WORKITEM_HANDLE IoWorkItem
    );

typedef USB_BUSIFFN_FREE_WORKITEM *PUSB_BUSIFFN_FREE_WORKITEM;

__drv_functionClass(USB_BUSIFFN_QUEUE_WORKITEM)
typedef
VOID
USB_BUSIFFN
USB_BUSIFFN_QUEUE_WORKITEM (    
    __in PDEVICE_OBJECT DeviceObject,
    __in USB_WORKITEM_HANDLE IoWorkItem,
    __in PIO_WORKITEM_ROUTINE_EX WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in_opt PVOID Context,
    __in BOOLEAN ForwardProgress
    );

typedef USB_BUSIFFN_QUEUE_WORKITEM *PUSB_BUSIFFN_QUEUE_WORKITEM;

__drv_functionClass(USB_BUSIFFN_DEFER_IRP_PROCESSING)
typedef
VOID
USB_BUSIFFN
USB_BUSIFFN_DEFER_IRP_PROCESSING (
    __in PDEVICE_OBJECT DeviceObject,
    __in PUSB_IRP_WORKER_ROUTINE IrpWorkerRoutine,
    __in PIRP Irp
    );

typedef USB_BUSIFFN_DEFER_IRP_PROCESSING *PUSB_BUSIFFN_DEFER_IRP_PROCESSING;

__drv_functionClass(USB_BUSIFFN_RESET_DEVICE_ADDRESS)
typedef
NTSTATUS
USB_BUSIFFN
USB_BUSIFFN_RESET_DEVICE_ADDRESS (
    __in PVOID BusContext,
    __in PUSB_DEVICE_HANDLE DeviceHandle
    );

typedef USB_BUSIFFN_RESET_DEVICE_ADDRESS *PUSB_BUSIFFN_RESET_DEVICE_ADDRESS;

__drv_functionClass(USB_BUSIFFN_RESTORE_DEVICE2)
typedef
NTSTATUS
USB_BUSIFFN
USB_BUSIFFN_RESTORE_DEVICE_HANDLE (
    __in PVOID BusContext,
    __in PUSB_DEVICE_HANDLE DeviceHandle
    );

typedef USB_BUSIFFN_RESTORE_DEVICE_HANDLE *PUSB_BUSIFFN_RESTORE_DEVICE_HANDLE;

#define USB_BUSIF_HUB_FORWARD_PROGRESS_VERSION_0         0x0000

// {D74BD695-5916-4a8a-9AD6-5B207F453065}
DEFINE_GUID(USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS_GUID, 
0xd74bd695, 0x5916, 0x4a8a, 0x9a, 0xd6, 0x5b, 0x20, 0x7f, 0x45, 0x30, 0x65);

typedef struct _USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS {

    USHORT Size;
    USHORT Version;
    // returns
    PVOID BusContext;
    PINTERFACE_REFERENCE InterfaceReference;
    PINTERFACE_DEREFERENCE InterfaceDereference;

    //
    // functions for the hub driver
    //

    PUSB_BUSIFFN_ALLOCATE_WORKITEM AllocateWorkItem;
    PUSB_BUSIFFN_FREE_WORKITEM FreeWorkItem;
    PUSB_BUSIFFN_QUEUE_WORKITEM QueueWorkItem;
    PUSB_BUSIFFN_DEFER_IRP_PROCESSING DeferIrpProcessing;

} USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS, *PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS;

#endif // USB_FORWARD_PROGRESS_BUS_INTERFACE
#endif // _USB_BOOT_H
