/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    usbhcdi.h

Abstract:

Environment:

    Kernel & user mode

Revision History:

    6-20-99 : created

--*/

#ifndef   __USB_HCDI_H__
#define   __USB_HCDI_H__
#include "acpiioct.h"
/*
    Power management rules for USB host controllers transitioning from
    USB suspend to USB working.  This is what we expect for
    S3->S0
    S1->S0

    1. The controller must not reset the USB bus or cause a disconnect or power
       loss on any of the root USB ports.
    2. The system BIOS must not enable any type of legacy USB BIOS or otherwise
       enable the host controller to a run state.
    3. If a PCI reset occurs in addition to rule 1 the BIOS must restore all
       host registers to their state prior to entering low power.  Root ports
       should NOT indicate connect or enable status changes.
    4. The controller hardware must be in a functional state -- capable of
       driving resume and entering the run state without requiring a global
       hardware reset that otherwise would result in a USB bus reset driven on
       the root ports.

*/


#define USB_BAD_PTR     ((PVOID) (-1))



/* status code returned by core functions */

typedef enum _USB_MINIPORT_STATUS {

    USBMP_STATUS_SUCCESS = 0,
    USBMP_STATUS_BUSY,
    USBMP_STATUS_NO_RESOURCES,
    USBMP_STATUS_NO_BANDWIDTH,
    USBMP_STATUS_INIT_FAILURE,
    USBMP_STATUS_FAILURE,
    USBMP_STATUS_NOT_SUPPORTED,
    USBMP_STATUS_HARDWARE_FAILURE,
    USBMP_STATUS_NTERRCODE_NOT_MAPPFED,
    USBMP_STATUS_FORCE_RESET

} USB_MINIPORT_STATUS;

/*
    common USB 2 definitions
*/
#define FS_BYTES_PER_MICROFRAME 188
#define MICROFRAMES_PER_FRAME   8

// number of byte times allocated to an SOF packet at the beginning of a frame
#define FS_SOF                  6

// scheduling window for budget tracking, periods longer than this are reduced to this.
// Also impacts space required for/ tracking data structures.  Otherwise fairly arbitrary.
#define MAXFRAMES   32

#define MAXMICROFRAMES  (MAXFRAMES * 8)

#define MAX_ULONG          ((ULONG)0xFFFFFFFF)

/*
    32 bit FrameNumber gets incremented every millisecond and rolls over 
    after ~49.71 days. During rollover (FFFFFFFF to 0), checks  involving 
    <, >, <= and >= between frame numbers can fail. GREATER_THAN_EQUAL_TO
    and GREATER_THAN and handle the rollover conditions.
    Frame numbers in the range [CurrentFrame, CurrentFrame + MAX_ULONG/2)
    are assumed to be in future and rest in the past.
*/

// x > y
#define GREATER_THAN(x, y) \
    ((x) - ((y) + 1) < MAX_ULONG/2)

// x >= y
#define GREATER_THAN_EQUAL_TO(x,y) \
    ((x) - (y) < MAX_ULONG/2)


/* define a test guids for the miniport pass-thru interface */
/* {53D3650A-A4E7-4b0f-BC1D-B76DEB40FA1E}*/
DEFINE_GUID(MINIPORT_PASSTHRU_TEST_GUID,
0x53d3650a, 0xa4e7, 0x4b0f, 0xbc, 0x1d, 0xb7, 0x6d, 0xeb, 0x40, 0xfa, 0x1e);

/* {386289AA-02EC-486e-925E-838931877F4B}*/
DEFINE_GUID(MINIPORT_PASSTHRU_TEST_BADGUID,
0x386289aa, 0x2ec, 0x486e, 0x92, 0x5e, 0x83, 0x89, 0x31, 0x87, 0x7f, 0x4b);

#define TEST_FLAG(var, flag)    (((var) & (flag)) ? TRUE : FALSE)
#define CLEAR_FLAG(var, flag)   InterlockedAnd(&(var), ~(flag))
#define SET_FLAG(var, flag)     InterlockedOr(&(var), (flag));

/*
    Definition for the 32 bit physical address
    that controller hardware understands.

    If a miniport HW structure only supports 32 bit
    physical addresses then this type is used to
    indicate it.

*/
typedef ULONG HW_32BIT_PHYSICAL_ADDRESS;
typedef HW_32BIT_PHYSICAL_ADDRESS *PHW_32BIT_PHYSICAL_ADDRESS;

typedef PHYSICAL_ADDRESS HW_64BIT_PHYSICAL_ADDRESS;

#pragma warning(push)
#pragma warning(disable: 4214) // bit field types other than int
#pragma warning(disable: 4201) // nameless struct/union

typedef struct _MP_HW_PHYSICAL_ADDRESS {
    union {
        HW_32BIT_PHYSICAL_ADDRESS Hw32;
        HW_64BIT_PHYSICAL_ADDRESS Hw64;
    };
} MP_HW_PHYSICAL_ADDRESS, *PMP_HW_PHYSICAL_ADDRESS;

C_ASSERT((sizeof(MP_HW_PHYSICAL_ADDRESS) == 8));

/*
   This structure is used for pointers embedded in
   HW structures by the miniport.  They is always sized
   for 64 bit to limit 32/64 bit porting problems.
*/

typedef struct _MP_HW_POINTER {
        PVOID Pointer;
#ifndef _WIN64
        ULONG PadTo8;
#endif
} MP_HW_POINTER, *PMP_HW_POINTER;

C_ASSERT((sizeof(MP_HW_POINTER) == 8));

typedef struct _MP_HW_LIST_ENTRY {
   LIST_ENTRY List;
#ifndef _WIN64
   ULONG PadTo16[2];
#endif
} MP_HW_LIST_ENTRY, *PMP_HW_LIST_ENTRY;

C_ASSERT((sizeof(MP_HW_LIST_ENTRY) == 16));


#define PENDPOINT_DATA PVOID
#define PDEVICE_DATA PVOID
#define PTRANSFER_CONTEXT PVOID
#define PDEVH_CONTEXT   PVOID
/* High speed only */
#define PHSB_HC_DATA PVOID
#define PHSB_TT_DATA PVOID
#define PHSB_EP_DATA PVOID

/*
    we redifine the USBDI HCD AREA for the miniport model
*/
struct _USBPORT_DATA {
    PVOID HcdTransferContext;
    PVOID UrbSig;
    PVOID Reserved7[6];
};

#ifdef _WIN64
#define URB_SIG ((PVOID) 0xDEADF00DDEADF00D)
#else
#define URB_SIG ((PVOID) 0xDEADF00D)
#endif

//C_ASSERT(sizeof(struct _URB_HCD_AREA) ==  sizeof(struct _USBPORT_DATA))

#define IN_TRANSFER(tp)       (((tp)->TransferFlags & \
                                USBD_TRANSFER_DIRECTION_IN) ? TRUE : FALSE)

#define SHORT_TRANSFER_OK(tp) (((tp)->TransferFlags & \
                                 USBD_SHORT_TRANSFER_OK) ? TRUE : FALSE)

/*
Common transfer request parameter definition, all transfer
requests passed to the miniport will be mapped to this
format.  The miniport will/can use this structure to
reference fields that are common to all transfers
as well as fields specific to isochronous and
control transfers.
*/

#define MPTX_SPLIT_TRANSFER             0x00000001
#define MPTX_SPLIT_TRANSFER_TAIL        0x00000020

typedef struct _TRANSFER_PARAMETERS {

    /* identical to URB field */
    ULONG TransferFlags;
    /* identical to URB field */
    ULONG TransferBufferLength;
    /* uniquely identifies a transfer set */
    ULONG SequenceNumber;
    /* miniport special handling requirements */
    ULONG MiniportFlags;
    /* USB frame this transfer completed in */
    ULONG FrameCompleted;
    /* setup packet for control transfers */
    UCHAR SetupPacket[8];

} TRANSFER_PARAMETERS, *PTRANSFER_PARAMETERS;

typedef struct _MINIPORT_ISO_PACKET {
    /*
      length of this packet
    */
    ULONG Length;
    /*
       bytes transferred this packet
     */
    ULONG LengthTransferred;
    /*
       virtual frame to transmit this packet
    */
    ULONG FrameNumber;
    ULONG MicroFrameNumber;
    /*
       completion code for this packet
     */
    USBD_STATUS UsbdStatus;

    ULONG BufferPointerCount;

    /* support up to 2 sg entries per packet */
    /* max packet size for a USB 1.1 frame is ~1024 bytes*/

    ULONG BufferPointer0Length;
    MP_HW_PHYSICAL_ADDRESS BufferPointer0;

    ULONG BufferPointer1Length;
    MP_HW_PHYSICAL_ADDRESS BufferPointer1;

} MINIPORT_ISO_PACKET, *PMINIPORT_ISO_PACKET;


typedef struct _MINIPORT_ISO_TRANSFER {

    ULONG Sig;
    ULONG PacketCount;
    PUCHAR SystemAddress;
    MINIPORT_ISO_PACKET Packets[1];

} MINIPORT_ISO_TRANSFER, *PMINIPORT_ISO_TRANSFER;


/*
    These structures are used to pass IoMapped transfer
    buffers to the miniport
*/

//
// Page size and shift value used by the OHCI, EHCI and UHCI
// controller  usb controllers use a defined
//
// this must always be 4k -- it is defined by the controller HW
#define USB_PAGE_SIZE       0x00001000

#define USB_PAGE_SHIFT      12L

#define TRANSFER_SG_ENTRY32 TRANSFER_SG_ENTRY

typedef struct _TRANSFER_SG_ENTRY {
    MP_HW_PHYSICAL_ADDRESS LogicalAddress;
    PUCHAR SystemAddress;
    ULONG Length;
    ULONG StartOffset;
} TRANSFER_SG_ENTRY, *PTRANSFER_SG_ENTRY;

#define USBMP_SGFLAG_SINGLE_PHYSICAL_PAGE   0x00000001

typedef struct _TRANSFER_SG_LIST {
    ULONG SgFlags;
    PUCHAR MdlVirtualAddress;
    PUCHAR MdlSystemAddress;
    ULONG SgCount;
    TRANSFER_SG_ENTRY SgEntry[1];
} TRANSFER_SG_LIST, *PTRANSFER_SG_LIST;

// Capabilities of the host controller which may vary between different
// instances of host controllers which share the same miniport.
//
typedef struct _MINIPORT_HC_CAPABILITIES {
    USHORT  Size;
    USHORT  Version;
} MINIPORT_HC_CAPABILITIES, *PMINIPORT_HC_CAPABILITIES;

#define MINIPORT_HC_CAPABILITIES_VERSION1   1
#define MINIPORT_HC_CAPABILITIES_VERSION2   2

typedef struct _MINIPORT_HC_CAPABILITIES_V1 {
    USHORT  Size;
    USHORT  Version;

    // 0: Host controller is not capable of using
    // HW_64BIT_PHYSICAL_ADDRESS pointers for data buffers.
    //
    // 1: Host controller is capable of using HW_64BIT_PHYSICAL_ADDRESS
    // pointers for data buffers.
    //
    ULONG   Bit64AddressingCapability;

} MINIPORT_HC_CAPABILITIES_V1, *PMINIPORT_HC_CAPABILITIES_V1;

typedef struct _MINIPORT_HC_CAPABILITIES_V2 {
    USHORT  Size;
    USHORT  Version;

    // 0: Host controller is not capable of using
    // HW_64BIT_PHYSICAL_ADDRESS pointers for data buffers.
    //
    // 1: Host controller is capable of using HW_64BIT_PHYSICAL_ADDRESS
    // pointers for data buffers.
    //
    ULONG   Bit64AddressingCapability;

    BOOLEAN IntegratedTTSupported;

} MINIPORT_HC_CAPABILITIES_V2, *PMINIPORT_HC_CAPABILITIES_V2;

/**************************************************************
 **************************************************************
USBPORT Interface Services

    NOTES:
        - these functions are callable at raised IRQL


***************************************************************
***************************************************************/

#define USBPRTFN __stdcall

/*
VOID
USBPORTSVC_InvalidateEndpoint(
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData
    );

*/

typedef VOID
    (USBPRTFN *PPORTFN_INVALIDATE_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_DATA
    );

/*
PUCHAR
USBPORTSVC_MapHwPhysicalToVirtual(
    HW_32BIT_PHYSICAL_ADDRESS HwPhysicalAddress,
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData
    )

    maps a physical address from the miniport to
    a virtual address
*/

typedef PUCHAR
    (USBPRTFN *PPORTFN_PHYS_TO_VIRTUAL) (
        HW_32BIT_PHYSICAL_ADDRESS,
        PDEVICE_DATA,
        PENDPOINT_DATA
    );

/*
VOID
USBPORTSVC_CompleteTransfer(
    PDEVICE_DATA DeviceData
    PDEVICE_DATA EndpointData,
    PTRANSFER_PARAMETERS TransferParameters,
    USBD_STATUS UsbdStatus,
    ULONG BytesTransferred
    );

    Called by miniport to complete an async transfer request
*/

typedef VOID
    (USBPRTFN *PPORTFN_COMPLETE_TRANSFER) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        PTRANSFER_PARAMETERS,
        USBD_STATUS,
        ULONG
    );

/*
VOID
USBPORTSVC_CompleteIsoTransfer(
    PDEVICE_DATA DeviceData
    PDEVICE_DATA EndpointData,
    PTRANSFER_PARAMETERS TransferParameters,
    PMINIPORT_ISO_TRANSFER IsoTransfer
    );

    Called by miniport to complete an iso transfer request
*/

typedef VOID
    (USBPRTFN *PPORTFN_COMPLETE_ISO_TRANSFER) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        PTRANSFER_PARAMETERS,
        PMINIPORT_ISO_TRANSFER
    );


/* ROOT HUB functions */


/*
VOID
USBPORTSVC_InvalidateRootHub(
    PDEVICE_DATA DeviceData
    );

    Called by the miniport to indicate the root hub
    needs attention

*/

typedef VOID
    (USBPRTFN *PPORTFN_INVALIDATE_ROOTHUB) (
        PDEVICE_DATA
    );


/* Debug functions */

/*
VOID
USBPORTSVC_DbgPrint(
    PDEVICE_DATA DeviceData,
    ULONG Level,
    PCH Format,
    PVOID Arg0,
    PVOID Arg1,
    PVOID Arg2,
    PVOID Arg3,
    PVOID Arg4,
    PVOID Arg5
    );

    Called by miniport to print a message to the debugger
    the message is printed if the var USBPORT_DEBUG_TRACE_LEVEL
    is >= level.
*/

typedef VOID
    (USBPRTFN *PPORTFN_DBGPRINT) (
        PDEVICE_DATA,
        ULONG,
        PCH,
        int,
        int,
        int,
        int,
        int,
        int
    );

/*
VOID
USBPORTSVC_TestDebugBreak(
    PDEVICE_DATA DeviceData
    );

   Triggers a break in the debugger in the registry key
   debugbreakOn is set.  These breakpoints are useful for
   debugging hardware/client software problems

*/

typedef VOID
    (USBPRTFN *PPORTFN_TEST_DEBUG_BREAK) (
        PDEVICE_DATA
    );

/*
VOID
USBPORTSVC_AssertFailure(
    PDEVICE_DATA DeviceData
    PVOID FailedAssertion,
    PVOID FileName,
    ULONG LineNumber,
    PCHAR Message
    );


*/

typedef VOID
    (USBPRTFN *PPORTFN_ASSERT_FAILURE) (
        PDEVICE_DATA,
        PVOID,
        PVOID,
        ULONG,
        PCHAR
    );

/*
VOID
USBPORTSVC_LogEntry(
    PDEVICE_DATA DeviceData,

    );


*/

/* Miniport LOG MASKS */
#define G 0x10000001    /* always log */

typedef VOID
    (USBPRTFN *PPORTFN_LOGENTRY) (
        PDEVICE_DATA,
        ULONG,
        ULONG,
        ULONG_PTR,
        ULONG_PTR,
        ULONG_PTR
    );

/* other functions */

/*
USB_MINIPORT_STATUS
USBPORTSVC_ReadWriteConfigSpace(
    PDEVICE_DATA DeviceData,
    BOOLEAN Read,
    PVOID Buffer,
    ULONG Offset,
    ULONG Length
    )

    reads a registry key value from the branch associated
    with the PDO for the host controller.

    this API reads from either the software or hardware
    branch

    this function cannot be called at raised IRQL
*/

typedef USB_MINIPORT_STATUS
    (USBPRTFN *PPORTFN_READWRITE_CONFIG_SPACE) (
        PDEVICE_DATA,
        BOOLEAN,
        PVOID,
        ULONG,
        ULONG
    );

/*
VOID
USBPORTSVC_Wait(
    PDEVICE_DATA DeviceData,
    ULONG MillisecondsToWait
    )

    Execute a syncronous wait for a specified number of
    milliseconds
*/

typedef VOID
    (USBPRTFN *PPORTFN_WAIT) (
        PDEVICE_DATA,
        ULONG
    );

/*
VOID
USBPORTSVC_BugCheck(
    PDEVICE_DATA DeviceData
    )

*/

typedef VOID
    (USBPRTFN *PPORTFN_BUGCHECK) (
        PDEVICE_DATA
    );


/*
VOID
USBPORTSVC_NotifyDoubleBuffer(
    PDEVICE_DATA DeviceData
    PTRANSFER_PARAMETERS TransferParameters,
    PVOID SystemAddress,
    ULONG Length
    )

*/

typedef VOID
    (USBPRTFN *PPORTFN_NOTIFY_DOUBLE_BUFFER) (
        PDEVICE_DATA,
        PTRANSFER_PARAMETERS,
        PVOID,
        ULONG
    );


/*
USB_MINIPORT_STATUS
USBPORTSVC_GetMiniportRegistryKeyValue(
    PDEVICE_DATA DeviceData,
    BOOLEAN SoftwareBranch,
    PCWSTR KeyNameString,
    ULONG KeyNameStringLength,
    PVOID Data,
    ULONG DataLength
    )

    reads a registry key value from the branch associated
    with the PDO for the host controller.

    this API reads from either the software or hardware
    branch

    this function cannot be called at raised IRQL
*/

typedef USB_MINIPORT_STATUS
    (USBPRTFN *PPORTFN_GET_MINIPORT_REGESTRY_KEY_VALUE) (
        PDEVICE_DATA,
        BOOLEAN,
        PCWSTR,
        ULONG,
        PVOID,
        ULONG
    );


/*++

USBPORTSVC_RequestAsyncCallback, USBPORTSVC_RequestAsyncCallbackEx

--*/

typedef VOID
    (__stdcall *PMINIPORT_CALLBACK) (
        PDEVICE_DATA,
        PVOID
    );

typedef VOID
    (USBPRTFN *PPORTFN_REQUEST_ASYNC_CALLBACK) (
        PDEVICE_DATA,
        ULONG,
        PVOID,
        ULONG,
        PMINIPORT_CALLBACK
    );

typedef NTSTATUS
    (USBPRTFN *PPORTFN_REQUEST_ASYNC_CALLBACK_EX) (
        PDEVICE_DATA,
        ULONG,
        PVOID,
        ULONG,
        PMINIPORT_CALLBACK,
        PVOID *,
        BOOLEAN
    );

typedef BOOLEAN
    (USBPRTFN *PPORTFN_CANCEL_ASYNC_CALLBACK) (
        PDEVICE_DATA,
        PVOID,
        BOOLEAN
    );

/*
VOID
USBPORTSVC_InvalidateController(
    PDEVICE_DATA DeviceData,
    USB_CONTROLLER_STATE ControllerState
    )

*/

typedef enum _USB_CONTROLLER_STATE {

    UsbMpControllerPowerFault,
    UsbMpControllerNeedsHwReset,
    UsbMpControllerRemoved,
    UsbMpSimulateInterrupt,
    UsbMpSignalInterrupt,
    UsbMpControllerNeedsHwResetOnPassive

} USB_CONTROLLER_STATE;


typedef VOID
    (__stdcall *PPORTFN_INVALIDATE_CONTROLLER) (
        PDEVICE_DATA,
        USB_CONTROLLER_STATE
    );


/*
NTSTATUS
USBPORTSVC_EtwWrite(
    __in PDEVICE_DATA DeviceData,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt LPCGUID ActivityId,
    __in ULONG UserDataCount,
    __in_ecount_opt(UserDataCount) PEVENT_DATA_DESCRIPTOR  UserData
    );

*/

typedef NTSTATUS
    (USBPRTFN *PPORTFN_ETW_WRITE) (
    __in PDEVICE_DATA DeviceData,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt LPCGUID ActivityId,
    __in ULONG UserDataCount,
    __in_ecount_opt(UserDataCount) PEVENT_DATA_DESCRIPTOR  UserData
    );

typedef BOOLEAN
    (USBPRTFN *PPORTFN_IS_INTERRUPT_PER_TD_ENABLED) (
    __in PDEVICE_DATA DeviceData
    );

typedef BOOLEAN
    (USBPRTFN *PPORTFN_IGNORE_RESUME_SIGNALING) (
    __in PDEVICE_DATA DeviceData
    );

/*********************************************************************************************************************
 *********************************************************************************************************************
USB MINIPORT interface functions

    prototypes for functions called by the USB port driver (usbport)
**********************************************************************************************************************
**********************************************************************************************************************/

#define USBMPFN __stdcall

/*++
    MINIPORT_SubmitTransfer
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_SUBMIT_TRANSFER) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        PTRANSFER_PARAMETERS,
        PTRANSFER_CONTEXT,
        PTRANSFER_SG_LIST
    );

/*++
    MINIPORT_SubmitTransfer
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_SUBMIT_ISO_TRANSFER) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        PTRANSFER_PARAMETERS,
        PTRANSFER_CONTEXT,
        PMINIPORT_ISO_TRANSFER
    );


/*++
    MINIPORT_AbortTransfer
--*/

typedef VOID
    (USBMPFN *PMPFN_ABORT_TRANSFER) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        PTRANSFER_CONTEXT,
        PULONG
    );

/*++
    MINIPORT_OpenEndpoint
--*/

typedef enum _ENDPOINT_TRANSFER_TYPE {
    Isochronous = 0,
    Control,
    Bulk,
    Interrupt
} ENDPOINT_TRANSFER_TYPE;


typedef enum _ENDPOINT_TRANSFER_DIRECTION {
    In = 0,
    Out
} ENDPOINT_TRANSFER_DIRECTION;

typedef enum _DEVICE_SPEED {
    LowSpeed = 0,
    FullSpeed,
    HighSpeed
} DEVICE_SPEED;


/* these values are input by the port driver */

/*
    Bandwidth Management:

    All bandwidth allocation is managed by the port driver.  The
    bandwidth consumed by an endpoint is passed to the miniport
    but this is purely informational.

    Load balancing for interrupt endpoints is handled by passing
    the miniport the appropriate schedule offset for an interrupt
    endpoint.

    interrupt endpoints may occupy different locations in the
    schedule dpeneding on the period. The consume bandwidth only
    for locations they occupy.  This is the 'ScheduleOffset'.

    USBPORT will choose an appropriate schedule offset and pass this
    to the miniport for the open.

    period      offsets
       1          0
       2          0,1
       4          0,..3
       8          0,..7
       16         0,..15
       32         0,..31

*/

typedef struct _ENDPOINT_PARAMETERS {

    USHORT DeviceAddress;
    USHORT EndpointAddress;
    USHORT MaxPacketSize;
    /* adjusted interrupt period will be one of : 128, 64, 32, 16, 8, 4, 2, 1 */
    UCHAR Period;
    UCHAR MaxPeriod;
    // bandwidth required in bits/ms
    // ie the reserved bw that this endpont will
    // consume
    DEVICE_SPEED DeviceSpeed;
    ULONG Bandwidth;
    ULONG ScheduleOffset;
    ENDPOINT_TRANSFER_TYPE TransferType;
    ENDPOINT_TRANSFER_DIRECTION TransferDirection;
    PUCHAR CommonBufferVa;
    HW_32BIT_PHYSICAL_ADDRESS CommonBufferPhys;
    ULONG CommonBufferBytes;
    /* EP_PARM_FLAG_ parm flags */
    ULONG EndpointFlags;
    ULONG MaxTransferSize;
    /* USB 2.0 parameters device address of the hub (TT) for this ep */
    USHORT TtDeviceAddress;
    /* Port number (really TT number) of the TT for this device. */
    USHORT TtPortNumber;

    UCHAR InterruptScheduleMask;
    UCHAR SplitCompletionMask;
    UCHAR TransactionsPerMicroframe;
    UCHAR Pad;
    USHORT MuxPacketSize;
    ULONG Ordinal;

    /* Addional fields only valid if the USBX flag is indicated */

} ENDPOINT_PARAMETERS, *PENDPOINT_PARAMETERS;

/* these are returned by the miniport */
typedef struct _ENDPOINT_REQUIREMENTS {
    /* size of the common buffer the miniport will need to service this endpoint */
    ULONG MinCommonBufferBytes;

    /*
       The largest single transfer this endpoint
       can handle. If a client driver passes
       down a larger transfer usbport will break
       it in to multiple requests.

       MinCommonBufferBytes/sizeof(TD) should be
       enough TDs to statify at least one request
       of MaximumTransferSize.

       Ideally it should be able to handle two -- this
       will ensure that the bus does not go idle.

    */

    ULONG MaximumTransferSize;

} ENDPOINT_REQUIREMENTS, *PENDPOINT_REQUIREMENTS;


// **
// EP_PARM_FLAG_
// Enpoint Parameter flags, describe required endpoint behaviors including possible optimaizations

//
// EP_PARM_FLAG_NOHALT - the endpoint should not halt on the
//      host side as a result of a bus error
//
#define EP_PARM_FLAG_NOHALT           0x00000004

//
// EP_PARM_FLAG_LOPWR - the endpoint is a Wireless USB Low Power endpoint
//
#define EP_PARM_FLAG_LOPWR            0x00010000

//
// EP_PARM_FLAG_ADJ_MAXPKT - the endpoint supports Wireless USB adjustable
//      Max Packet sizes
//
#define EP_PARM_FLAG_ADJ_MAXPKT       0x00020000





typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_OPEN_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_PARAMETERS,
        PENDPOINT_DATA
    );


/*****************************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------------*/
/* USBX  USB miniport extensions

    These are extended versions of the miniport APIS that give the miniports control of more complex tasks such as
    bandwidth allocation and hardware synchronization.

*/
/*****************************************************************************************************************/

/*++
    MINIPORT_UsbxOpenEndpoint
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_USBX_OPEN_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_PARAMETERS,
        PENDPOINT_DATA,
        PHSB_HC_DATA,
        PHSB_EP_DATA,
        PHSB_TT_DATA
    );



//
// These structures are used for both UsbxOpenEndpoint and UsbxQueryEndpointRequirements
//

/*++
    MINIPORT_UsbxPokeEndpoint
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_USBX_POKE_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_PARAMETERS,
        PENDPOINT_DATA,
        PHSB_HC_DATA,
        PHSB_TT_DATA,
        PHSB_EP_DATA
    );


/*++
    MINIPORT_UsbxQueryEndpointRequirements
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_USBX_QUERY_ENDPOINT_REQUIREMENTS) (
        PDEVICE_DATA,
        PENDPOINT_PARAMETERS,
        PENDPOINT_DATA
    );


/*++
    MINIPORT_UsbxInitHsbController
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_USBX_INIT_HSB_CONTROLLER) (
        PDEVICE_DATA,
        PHSB_HC_DATA
    );

/*++
    MINIPORT_UsbxAllocateBandwidth
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_USBX_ALLOCATE_BANDWIDTH) (
        PDEVICE_DATA,
        PENDPOINT_PARAMETERS,
        PENDPOINT_DATA,
        PHSB_HC_DATA,
        PHSB_TT_DATA,
        PHSB_EP_DATA
    );

/*++
    MINIPORT_UsbxFreeBandwidth
--*/

typedef VOID
    (USBMPFN *PMPFN_USBX_FREE_BANDWIDTH) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        PHSB_HC_DATA,
        PHSB_TT_DATA,
        PHSB_EP_DATA
    );



/*++
    MINIPORT_UsbxInitHsbTransactionTranslator
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_USBX_INIT_HSB_TT) (
        PDEVICE_DATA,
        PHSB_HC_DATA,
        PHSB_TT_DATA
    );

/*++
    MINIPORT_UsbxInitHsbEndpoint
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_USBX_INIT_HSB_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        PHSB_HC_DATA,
        PHSB_EP_DATA,
        PHSB_TT_DATA
    );


/*++
    MINIPORT_UsbxQueryBandwidthData
--*/

typedef ULONG
    (USBMPFN *PMPFN_USBX_QUERY_BW_DATA) (
        PDEVICE_DATA,
        PHSB_HC_DATA,
        ULONG,
        ULONG
    );

/*++
    MINIPORT_UsbxQueryTtBandwidthData
--*/

typedef ULONG
    (USBMPFN *PMPFN_USBX_QUERY_TT_BW_DATA) (
        PDEVICE_DATA,
        PHSB_HC_DATA,
        PHSB_TT_DATA,
        ULONG
    );


/*++
    MINIPORT_UsbxQueryEpBandwidthData
--*/

typedef ULONG
    (USBMPFN *PMPFN_USBX_QUERY_EP_BW_DATA) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        PHSB_HC_DATA,
        PHSB_TT_DATA,
        PHSB_EP_DATA
    );


/*++
    MINIPORT_NotifyTransferQueueState
--*/

typedef VOID
    (USBMPFN *PMPFN_NOTIFY_TRANSFER_QUEUE_STATE) (
        PDEVICE_DATA,
        ULONG,
        ULONG
    );


/*++
    MINIPORT_PollHwSync
--*/

typedef ULONG
    (USBMPFN *PMPFN_POLL_HW_SYNC) (
        PDEVICE_DATA,
        PENDPOINT_DATA
    );


/*---------------------------------------------------------------------------------------------------------------*/
/*****************************************************************************************************************/


/*++
    MINIPORT_PokeEndpoint
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_POKE_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_PARAMETERS,
        PENDPOINT_DATA
    );


/*++
    MINIPORT_RebalanceEndpoint (not supported)
--*/

typedef VOID
    (USBMPFN *PMPFN_REBALANCE_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_PARAMETERS,
        PENDPOINT_DATA
    );

/*++
    MINIPORT_QueryEndpointRequirements
--*/

#define MAX_ISO_PACKETS_PER_TRANSFER    256

typedef VOID
    (USBMPFN *PMPFN_QENDPOINT_REQUIREMENTS) (
        PDEVICE_DATA,
        PENDPOINT_PARAMETERS,
        PENDPOINT_REQUIREMENTS
    );

/*++
    MINIPORT_PollEndpoint
--*/

typedef VOID
    (USBMPFN *PMPFN_POLL_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_DATA
    );

/*++
    MINIPORT_CloseEndpoint
--*/

typedef VOID
    (USBMPFN *PMPFN_CLOSE_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_DATA
    );

/*++
    MINIPORT_SetEndpointState, MINIPORT_GetEndpointState
--*/
typedef enum _MP_ENDPOINT_STATE {
    ENDPOINT_TRANSITION = 0,    // reserved for use by port driver
    ENDPOINT_IDLE,
    ENDPOINT_PAUSE,
    ENDPOINT_ACTIVE,
    ENDPOINT_REMOVE,
    ENDPOINT_CLOSED,
    ENDPOINT_ERROR      = 100    // reserved for cases where the state cannot be determined
} MP_ENDPOINT_STATE;

typedef VOID
    (USBMPFN *PMPFN_SET_ENDPOINT_STATE) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        MP_ENDPOINT_STATE
    );

typedef MP_ENDPOINT_STATE
    (USBMPFN *PMPFN_GET_ENDPOINT_STATE) (
        PDEVICE_DATA,
        PENDPOINT_DATA
    );

/*++
    MINIPORT_SetEndpointDataToggle
--*/

typedef VOID
    (USBMPFN *PMPFN_SET_ENDPOINT_DATA_TOGGLE) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        ULONG
    );

/*++
    MINIPORT_GetEndpointStatus,  MINIPORT_SetEndpointStatus
--*/

typedef enum _MP_ENDPOINT_STATUS {
    ENDPOINT_STATUS_RUN =               0,
    ENDPOINT_STATUS_HALT =              1,
    ENDPOINT_STATUS_ERROR =             100,
    ENDPOINT_STATUS_ERROR_HC_HALTED =   101,
} MP_ENDPOINT_STATUS;


typedef MP_ENDPOINT_STATUS
    (USBMPFN *PMPFN_GET_ENDPOINT_STATUS) (
        PDEVICE_DATA,
        PENDPOINT_DATA
    );

typedef VOID
    (USBMPFN *PMPFN_SET_ENDPOINT_STATUS) (
        PDEVICE_DATA,
        PENDPOINT_DATA,
        MP_ENDPOINT_STATUS
    );

/*++
    MINIPORT_Get32BitFrameNumber
--*/

typedef ULONG
    (USBMPFN *PMPFN_GET_32BIT_FRAME_NUMBER) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_InterruptNextSOF
--*/

typedef VOID
    (USBMPFN *PMPFN_INTERRUPT_NEXT_SOF) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_PollController
--*/

typedef VOID
    (USBMPFN *PMPFN_POLL_CONTROLLER) (
        PDEVICE_DATA
    );

/***************************************************************
PNP/POWER Functions

    The following core pnp functions are serialized as a group

****************************************************************/

/*++
    MINIPORT_CheckController
--*/

typedef VOID
    (USBMPFN *PMPFN_CHECK_CONTROLLER) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_StartController

    The structure is used for both 32 bit and 64 bit EHCI.



--*/

#define USB_MAX_COMMON_BUFFER_BLOCKS        8

typedef struct _HC_RESOURCES {

/* values for Flags field */
#define HCR_UNMAP_IO_REGS     0x0000001
#define HCR_MAP_IO_REGS       0x0000002
#define HCR_IRQ               0x0000004
#define HCR_MEM_REGS          0x0000008

    ULONG Flags;
    USB_CONTROLLER_FLAVOR ControllerFlavor;

    /* interrupt */
    ULONG InterruptVector;
    KIRQL InterruptLevel;
    KAFFINITY Affinity;
    BOOLEAN ShareIRQ;
    KINTERRUPT_MODE InterruptMode;
    PKINTERRUPT InterruptObject;

    /* io ports */
    /* memory mapped */
    PVOID DeviceRegisters;
    ULONG DeviceRegistersLength;

    PUCHAR CommonBufferVa;
    HW_32BIT_PHYSICAL_ADDRESS CommonBufferPhys;

    /* BIOS detected
        filled in by miniport
    */
    BOOLEAN DetectedLegacyBIOS;
    BOOLEAN Restart;

    /* Additional common buffer allocations (rev 3)
    */

    /* indicates the seg register for all common buffer blocks passed to the miniport */
    HW_32BIT_PHYSICAL_ADDRESS ControllerSegmentRegister;

    ULONG CommonBufferBlockCount;
    PVOID CommonBufferBlocksVa[USB_MAX_COMMON_BUFFER_BLOCKS];
    HW_32BIT_PHYSICAL_ADDRESS CommonBufferBlocksPhys[USB_MAX_COMMON_BUFFER_BLOCKS];

    PVOID HostControllerFdo;
    PVOID HostControllerPdo;
    PVOID TargetDeviceObject;

    BOOLEAN DisablePciConfigSpace;

    //
    // If the host controller is enumerated by ACPI, these fields
    // will contain the detailed enumeration info retrieved by USBPORT
    //
    PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER AcpiDeviceInformation;
    ULONG AcpiDeviceInformationSize;

} HC_RESOURCES, *PHC_RESOURCES;


typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_START_CONTROLLER) (
        PDEVICE_DATA,
        PHC_RESOURCES
    );

/*++
    MINIPORT_PreStartController
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_PRESTART_CONTROLLER) (
        PDEVICE_DATA,
        PHC_RESOURCES
    );

/*++
    MINIPORT_StopController
--*/

typedef VOID
    (USBMPFN *PMPFN_STOP_CONTROLLER) (
        PDEVICE_DATA,
        BOOLEAN
    );

/*++
    MINIPORT_StopControllerWithInterrupts
--*/

typedef VOID
    (USBMPFN *PMPFN_STOP_CONTROLLER_WITH_INTERRUPTS) (
        PDEVICE_DATA
    );




/*
    MINIPORT_ResetController

    Reset the host controller hardware

    This function is serialized with the ISR and DPC and holds
    the core function lock in order to provide a safe environment
    or the miniport to reset the host hardware.


VOID
MINIPORT ResetController(
    PDEVICE_DATA DeviceData
    );
*/

typedef VOID
    (USBMPFN *PMPFN_RESET_CONTROLLER) (
        PDEVICE_DATA
    );

/*
    MINIPORT_ResetControllerDontReinit

    Reset the host controller hardware during controller reset recovery,
    however dont reinitalize it.

    This function is serialized with the ISR and DPC and holds
    the core function lock in order to provide a safe environment
    or the miniport to reset the host hardware.


VOID
MINIPORT ResetControllerDontReinit(
    PDEVICE_DATA DeviceData
    );
*/

typedef VOID
    (USBMPFN *PMPFN_RESET_CONTROLLER_DONT_REINIT) (
        PDEVICE_DATA
    );

/*
    MINIPORT_PostResetController

    Post Reset the host controller hardware

    This function called at passive into miniport after a controller reset has 
    happened during controller reset recovery.


VOID
MINIPORT PostResetController(
    PDEVICE_DATA DeviceData
    );
*/

typedef VOID
    (USBMPFN *PMPFN_POST_RESET_CONTROLLER) (
        PDEVICE_DATA
    );

/*
    MINIPORT_ReinitController

    Reinit the host controller hardware after reset

    This function is serialized with the ISR and DPC and holds
    the core function lock in order to provide a safe environment
    or the miniport to reset the host hardware.


VOID
MINIPORT ReinitController(
    PDEVICE_DATA DeviceData
    );
*/

typedef VOID
    (USBMPFN *PMPFN_REINIT_CONTROLLER) (
        PDEVICE_DATA
    );


/*++
    MINIPORT_SudpendController
--*/

typedef VOID
    (USBMPFN *PMPFN_SUSPEND_CONTROLLER) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_ResumeController
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_RESUME_CONTROLLER) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_FlushInterrupts
--*/

typedef VOID
    (USBMPFN *PMPFN_FLUSH_INTERRUPTS) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_TakePortControl
--*/

typedef VOID
    (USBMPFN *PMPFN_TAKE_PORT_CONTROL) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_ReadCfgFlag
--*/

typedef ULONG
    (USBMPFN *PMPFN_READ_CFG_FLAG) (
        PDEVICE_DATA
    );


/*++
    MINIPORT_EnableInterrupts
--*/

typedef VOID
    (USBMPFN *PMPFN_ENABLE_INTERRUPTS) (
        PDEVICE_DATA
    );


/*++
    MINIPORT_CreateDeviceData
--*/

typedef VOID
    (USBMPFN *PMPFN_CREATE_DEVICE_DATA) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_DeleteDeviceData
--*/

typedef VOID
    (USBMPFN *PMPFN_DELETE_DEVICE_DATA) (
        PDEVICE_DATA
    );


/*++
    MINIPORT_OpenDevHandle
--*/

typedef VOID
    (USBMPFN *PMPFN_OPEN_DEVH) (
        PDEVICE_DATA,
        PDEVH_CONTEXT
    );

/*++
    MINIPORT_CloseDevHandle
--*/

typedef VOID
    (USBMPFN *PMPFN_CLOSE_DEVH) (
        PDEVICE_DATA,
        PDEVH_CONTEXT
    );


/*++
    MINIPORT_DisableInterrupts

    Disable Interrupts by the USB the controller HW.

    On return from this function the controller is expected to not generate ANY interrupts. Also the controller should
    ack all interrupts since on return from this routine the ISR & ISRDPC will no longer be called.


    IRQL = This function is synchronized with the ISR
--*/

typedef VOID
    (USBMPFN *PMPFN_DISABLE_INTERRUPTS) (
        PDEVICE_DATA
    );


/*++
    MINIPORT_SetWakeOnConnect
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_SET_WAKE_ON_CONNECT) (
        PDEVICE_DATA,
        ULONG
    );

/*++
    MINIPORT_DbgFreeDeviceData
--*/

typedef VOID
    (USBMPFN *PMPFN_DBG_FREE_DEVICE_DATA) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_DbgFreeEndpoint
--*/

typedef VOID
    (USBMPFN *PMPFN_DBG_FREE_ENDPOINT) (
        PDEVICE_DATA,
        PENDPOINT_DATA
    );

/*++
    MINIPORT_DbgFreeDevHandle
--*/

typedef VOID
    (USBMPFN *PMPFN_DBG_FREE_DEVH) (
        PDEVICE_DATA,
        PDEVH_CONTEXT
    );

/*++
    MINIPORT_HaltController
--*/

typedef VOID
    (USBMPFN *PMPFN_HALT_CONTROLLER) (
        PDEVICE_DATA
    );





/***************************************************************
ROOT HUB Functions

MINIPORT_RH_GetRootHubData
MINIPORT_RH_DisableIrq
MINIPORT_RH_EnableIrq

MINIPORT_RH_GetStatus
MINIPORT_RH_GetPortStatus
MINIPORT_RH_GetHubStatus

Port Functions, all use PMPFN_RH_PORT_FUNCTION
MINIPORT_RH_SetFeaturePortReset
MINIPORT_RH_SetFeaturePortSuspend
MINIPORT_RH_SetFeaturePortPower
MINIPORT_RH_SetFeaturePortEnable

MINIPORT_RH_ClearFeaturePortEnable
MINIPORT_RH_ClearFeaturePortSuspend
MINIPORT_RH_ClearFeaturePortPower

MINIPORT_RH_ClearFeaturePortConnectChange
MINIPORT_RH_ClearFeaturePortResetChange
MINIPORT_RH_ClearFeaturePortEnableChange
MINIPORT_RH_ClearFeaturePortSuspendChange
MINIPORT_RH_ClearFeaturePortOvercurrentChange

MINIPORT_RH_SetWakeOnConnect

****************************************************************/

/*
    root hub port status data as defined in the Hub Class
    section of the CORE (USB 1.1) spec.
*/

typedef struct _RH_PORT_STATUS {

    union {
        struct {

            /* Status bits 0.. 15 */

            ULONG Connected:1;
            ULONG Enabled:1;
            ULONG Suspended:1;
            ULONG OverCurrent:1;
            ULONG Reset:1;
            ULONG Reserved0:3;
            ULONG PowerOn:1;
            ULONG LowSpeed:1;
            ULONG HighSpeed:1;
            ULONG Reserved1:4;
            /* borrowed from reserved bits to indicate
                port disposition */
            ULONG OwnedByCC:1;

            /* Change bits 16..31 */

            ULONG ConnectChange:1;
            ULONG EnableChange:1;
            ULONG SuspendChange:1;
            ULONG OverCurrentChange:1;
            ULONG ResetChange:1;
            ULONG Reserved2:11;

        };
        ULONG ul;
   };
} RH_PORT_STATUS, *PRH_PORT_STATUS;

C_ASSERT(sizeof(RH_PORT_STATUS) == sizeof(ULONG));

typedef struct _RH_HUB_STATUS {

    /* Status bits 0.. 15 */
    union {
        struct {
            ULONG LocalPowerLost:1;
            ULONG OverCurrent:1;
            ULONG Reserved:14;

            /* Change bits 16..31 */

            ULONG LocalPowerChange:1;
            ULONG OverCurrentChange:1;
            ULONG Reserved2:14;
        };
        ULONG ul;
    };
} RH_HUB_STATUS, *PRH_HUB_STATUS;

C_ASSERT(sizeof(RH_HUB_STATUS) == sizeof(ULONG));

/*
   Hub Charateristics as defined
   in 11.11.2 of the USB core spec.
*/
/* PowerSwitchType */
#define USBPORT_RH_POWER_SWITCH_GANG    0
#define USBPORT_RH_POWER_SWITCH_PORT    1

typedef union _RH_HUB_CHARATERISTICS {

    USHORT us;
    struct {
        /*
            00 = Gang switched
            01 = port power switched

            1x = 1.0 hubs with no power
                 switching
        */
        USHORT PowerSwitchType:2;       /* 0-1      */
        USHORT CompoundDevice:1;        /* 2        */
        USHORT OverCurrentProtection:2; /* 3-4      */
        USHORT Reserved:11;             /* 5-15     */
    };

} RH_HUB_CHARATERISTICS, *PRH_HUB_CHARATERISTICS;

C_ASSERT(sizeof(RH_HUB_CHARATERISTICS) == sizeof(USHORT));

#pragma warning(pop)

typedef struct _ROOTHUB_DATA {

    ULONG NumberOfPorts;         // number of ports on this hub
    RH_HUB_CHARATERISTICS HubCharacteristics;
    USHORT pad;
    ULONG PowerOnToPowerGood;    // port power on till power good in 2ms
    ULONG HubControlCurrent;     // max current in mA

} ROOTHUB_DATA, *PROOTHUB_DATA;



/*
    MINIPORT_RH_GetRootHubData

    get information about the root hub, port
    driver uses this information to emulate
    the root hub for the miniport

    IRQL = PASSIVE_LEVEL


VOID
MINIPORT_RH_GetRootHubData(
    PDEVICE_DATA DeviceData,
     PROOTHUB_DATA HubData
    );
*/

typedef VOID
    (USBMPFN *PMPFN_RH_GET_ROOTHUB_DATA) (
        PDEVICE_DATA,
        PROOTHUB_DATA
    );


/*
    MINIPORT_RH_GetStatus

    Used to support the GET_STATUS command sent to the root hub device.

    may return with the following bits set in Status

        USB_GETSTATUS_SELF_POWERED                0x01
        USB_GETSTATUS_REMOTE_WAKEUP_ENABLED       0x02

    are we self powered?
    are we a remote wakeup source?

    see section 9.4.5 USB 1.1 spec


    IRQL = PASSIVE_LEVEL


USB_MINIPORT_STATUS
MINIPORT_RH_GetStatus(
    PDEVICE_DATA DeviceData
     PUSHORT Status
    );
*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_RH_GET_STATUS) (
        PDEVICE_DATA,
         PUSHORT
    );

/*
    MINIPORT_RH_DisableIrq  (OPTIONAL)

    If the comntroller is capable of generating interrupts
    on root hub status changes then it must provide this
    service to disable/enable the feature.

    IRQL = DPC_LEVEL


VOID
MINIPORT_RH_DisableIrq(
    PDEVICE_DATA DeviceData
    );
*/

typedef VOID
    (USBMPFN *PMPFN_RH_DISABLE_IRQ) (
        PDEVICE_DATA
    );

/*
    MINIPORT_RH_EnableIrq   (OPTIONAL)

    IRQL = DPC_LEVEL


VOID
MINIPORT_RH_EnableIrq(
    PDEVICE_DATA DeviceData
    );
*/

typedef VOID
    (USBMPFN *PMPFN_RH_ENABLE_IRQ) (
        PDEVICE_DATA
    );

/*
    MINIPORT_RH_GetPortStatus

    Used to support the GET_STATUS hub class command sent
    to the root hub interface for a port.

    PortNumber = 1,2, etc

    see section 11.16.2.5 of USB 1.1 spec

    IRQL = PASSIVE_LEVEL


USB_MINIPORT_STATUS
MINIPORT_RH_GetPortStatus(
    PDEVICE_DATA DeviceData,
    USHORT PortNumber,
     PRH_PORT_STATUS PortStatus
    );
*/


typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_RH_GET_PORT_STATUS) (
        PDEVICE_DATA,
        USHORT,
        PRH_PORT_STATUS
    );


/*
    MINIPORT_RH_GetHubStatus

    Used to support the GET_STATUS hub class command sent
    to the root hub interface for a hub.

    IRQL = PASSIVE_LEVEL


USB_MINIPORT_STATUS
MINIPORT_RH_GetHubStatus(
    PDEVICE_DATA DeviceData,
     PRH_HUB_STATUS HubStatus
    );
*/


typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_RH_GET_HUB_STATUS) (
        PDEVICE_DATA,
        PRH_HUB_STATUS
    );


/*
    MINIPORT_RH_PORT_FUNCTION

    format for root hub services that act on ports.

    PortNumber = 1,2, etc

    IRQL = PASSIVE_LEVEL


USB_MINIPORT_STATUS
MINIPORT_RH_PORT_FUNCTION(
    PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

    The following services use the MINIPORT_RH_PORT_FUNCTION
    format

    MINIPORT_RH_SetFeaturePortReset

    MINIPORT_RH_SetFeaturePortPower

    MINIPORT_RH_SetFeaturePortEnable

    MINIPORT_RH_SetFeaturePortSuspend

    MINIPORT_RH_ClearFeaturePortEnable

    MINIPORT_RH_ClearFeaturePortPower

    MINIPORT_RH_ClearFeaturePortEnableChange

    MINIPORT_RH_ClearFeaturePortConnectChange

    MINIPORT_RH_ClearFeaturePortResetChange

    MINIPORT_RH_ClearFeaturePortOvercurrentChange

    PortNumber Zero indicates the hub, this applies specifically to:

        MINIPORT_RH_ClearFeaturePortPower
        MINIPORT_RH_ClearFeaturePortOvercurrentChange

*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_RH_PORT_FUNCTION) (
        PDEVICE_DATA,
        USHORT
    );

/*

typedef VOID
    (USBMPFN* PMPFN_RH_ASYNC_COMMAND) (
        PDEVICE_DATA,
        PUSB_DEFAULT_PIPE_SETUP_PACKET,
        PUCHAR,
        ULONG,
        PTRANSFER_CONTEXT
     );


    Used to send RH commands async to the miniport.  THe miniport will call
    the completion service, PPORTFN_RH_COMMAND_COMPLETE, when it is done.

*/

typedef VOID
    (USBMPFN* PMPFN_RH_ASYNC_COMMAND) (
        PDEVICE_DATA,
        PUSB_DEFAULT_PIPE_SETUP_PACKET,
        PUCHAR,
        ULONG,
        PTRANSFER_PARAMETERS
     );

typedef VOID
    (USBMPFN* PPORTFN_RH_COMMAND_COMPLETE) (
        PDEVICE_DATA,
        PTRANSFER_PARAMETERS,
        USB_MINIPORT_STATUS,
        ULONG
    );

typedef USB_MINIPORT_STATUS
    (USBMPFN* PMPFN_RH_GET_CONTAINER_ID_FOR_PORT) (
        PDEVICE_DATA,
        USHORT,
        LPGUID
    );

typedef VOID
    (USBMPFN* PMPFN_RH_SET_CONTAINER_ID_FOR_PORT) (
        PDEVICE_DATA,
        USHORT,
        LPGUID
    );

/***************************************************************
INTERRUPT Functions

these functions are not serialized
****************************************************************/
/*++
    MINIPORT_InterruptService
--*/


typedef BOOLEAN
    (USBMPFN *PMPFN_INTERRUPT_SERVICE) (
        PDEVICE_DATA
    );

/*++
    MINIPORT_InterruptDpc
--*/


typedef VOID
    (USBMPFN *PMPFN_INTERRUPT_DPC) (
        PDEVICE_DATA,
        BOOLEAN
    );


/*++
    MINIPORT_InterruptDpcEx
--*/

// flags returned by miniport that describe the nature of the interrupt

// This flag indicates the controller detected an interrupt while suspended.
#define USBMP_IRQ_SSP_RESUME_DETECT     0x00000001

// This flag indicates the controller detected a change on one of the root hub ports.
#define USBMP_IRQ_INV_ROOTHUB           0x00000002


typedef ULONG
    (USBMPFN *PMPFN_INTERRUPT_DPC_EX) (
        PDEVICE_DATA,
        BOOLEAN
    );

/***************************************************************
 DEBUG/TEST Functions

****************************************************************/

/*
    MINIPORT_SendOnePacket

    IRQL = ANY

    used to support the single-step debug application.  This api is
    OPTIONAL



VOID
MINIPORT_SendOnePacket (
    PDEVICE_DATA DeviceData,
    PPACKET_PARAMETERS PacketParameters,
    PUCHAR PacketData,
    PULONG PacketLength,
    PUCHAR WorkspaceVirtualAddress,
    HW_32BIT_PHYSICAL_ADDRESS WorkspacePhysicalAddress,
    ULONG WorkspaceLength,
    PUSBD_STATUS UsbdStatus
    );

*/

typedef enum _SS_PACKET_TYPE {
    ss_Setup = 0,
    ss_In,
    ss_Out,
    ss_Iso_In,
    ss_Iso_Out
} SS_PACKET_TYPE;


typedef enum _SS_PACKET_SPEED {
    ss_Low = 0,
    ss_Full,
    ss_High
} SS_PACKET_SPEED;


typedef enum _SS_PACKET_DATA_TOGGLE {
    ss_Toggle0 = 0,
    ss_Toggle1
} SS_PACKET_DATA_TOGGLE;


typedef struct _MP_PACKET_PARAMETERS {
    UCHAR DeviceAddress;
    UCHAR EndpointAddress;
    USHORT MaximumPacketSize;
    USHORT ErrorCount;
    USHORT Pad;
    SS_PACKET_TYPE Type;
    SS_PACKET_SPEED Speed;
    SS_PACKET_DATA_TOGGLE Toggle;
    /* 2.0 hubs */
    USHORT HubDeviceAddress;
    USHORT PortTTNumber;
} MP_PACKET_PARAMETERS, *PMP_PACKET_PARAMETERS;

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_SEND_ONE_PACKET) (
        PDEVICE_DATA,
        PMP_PACKET_PARAMETERS,
        PUCHAR,
        PULONG,
        PUCHAR,
        HW_32BIT_PHYSICAL_ADDRESS,
        ULONG,
        USBD_STATUS *
    );


/***************************************************************
 API Functions
    These are called at IRQL passive_level
****************************************************************/

/*
    MINIPORT_PassThru

    IRQL = PASSIVE_LEVEL



USB_MINIPORT_STATUS
MINIPORT_PassThru (
    PDEVICE_DATA DeviceData,
    GUID *FunctionGuid,
    ULONG ParameterLength,
    PVOID Parameters
    );
*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_PASS_THRU) (
        PDEVICE_DATA,
        GUID *,
        ULONG,
        PVOID
    );


/*++
    MINIPORT_GetHcCapabilities

    Called after the HC has been started and the HC hardware is
    accessible.
--*/

typedef USB_MINIPORT_STATUS
    (USBMPFN *PMPFN_GET_HC_CAPABILITIES) (
        PDEVICE_DATA,
        PMINIPORT_HC_CAPABILITIES
    );


/*
    MINIPORT_InitializeDumpInterface

    Called by usbport to initialize the dump interfaces
    in usbehci
--*/

typedef NTSTATUS
    (USBMPFN *PMPFN_INTERFACE_INITIALIZE) (
        PVOID
    );

/*
    MINIPORT_CleanupDumpInterface

    Called by usbport to cleanup the dump interfaces
    in usbehci
*/
typedef NTSTATUS
    (USBMPFN *PMPFN_HC_CLEANUP) (
        PVOID
    );

/*
    MINIPORT_GetDebugPortNumber

    Called by usbport to determine which root port is the debug port, if any
*/
typedef USHORT
    (USBMPFN *PMPFN_GET_DEBUG_PORT_NUMBER) (
        PVOID
    );

/*
    MINIPORT_CheckInvalidateRootHub

    Called by usbport to determine if the root hub should be invalidated immediately after the HC is suspended
*/
typedef BOOLEAN
    (USBMPFN *PMPFN_CHECK_INVALIDATE_ROOT_HUB) (
        PVOID
    );

/*
    MINIPORT_SetPortResumeTime

    Called by usbport to set or clear the minimum resume signaling time for a root port
*/
typedef VOID
    (USBMPFN *PMPFN_SET_PORT_RESUME_TIME) (
        PVOID,
        USHORT,
        BOOLEAN,
        UCHAR
    );


/***************************************************************
 ***************************************************************
 REGISTRATION PACKET
 ***************************************************************
****************************************************************/

/* HCI Hardware Types */

typedef enum _USB_HCI_TYPE {

    USB_UNKNOWN_HCI = 0,
    USB_OHCI,
    USB_UHCI,
    USB_EHCI

} USB_HCI_TYPE;


/* Miniport Option Flags */

/* resources required by miniport */
#define USB_MINIPORT_OPT_NEED_IRQ             0x00000001
#define USB_MINIPORT_OPT_NEED_IOPORT          0x00000002
#define USB_MINIPORT_OPT_NEED_MEMORY          0x00000004
/* USB version */
#define USB_MINIPORT_OPT_USB11                0x00000008
/* indicates support for USB2.0 High Speed Specification */
#define USB_MINIPORT_OPT_USB20                0x00000010

/* support selective suspend */
#define USB_MINIPORT_OPT_NO_SS                0x00000020
/* disables synchronization of the ISRDPC and the
   MP_DisableInterrupts function. usbed by UHCI
   to clear/ set PIRQD in config space. */
#define USB_MINIPORT_OPT_NO_IRQ_SYNC          0x00000040
/* indicates controller poll routine should be called
    this will always be at least ounce evert 500ms
*/
#define USB_MINIPORT_OPT_POLL_CONTROLLER      0x00000080
/* for bridge drivers with no hw resources */
#define USB_MINIPORT_OPT_NO_PNP_RESOURCES     0x00000100
/* poll when HW suspended */
#define USB_MINIPORT_OPT_POLL_IN_SUSPEND      0x00000200
/* miniport supports USBX extended miniport interface */
#define USB_MINIPORT_OPT_USBX                 0x00000400

/* miniport uses polling option for hardware sync (MPf_UsbxCheckHwSync) */
#define USB_MINIPORT_OPT_HW_SYNC              0x00000800

/* Will not zero miniport device data  */
#define USB_MINIPORT_OPT_KEEP_DEVICE_DATA     0x00001000

/* Will not poll the PCI config space for controller/companion info */
#define USB_MINIPORT_OPT_NO_PCI_CONFIG        0x00002000

/* Enabled ASYNC RH Class commands  */
#define USB_MINIPORT_OPT_RH_ASYNC             0x00004000

/* Keep IRQ and IntDpc enabled until after StopController */
#define USB_MINIPORT_OPT_IRQ_DURING_STOP      0x00008000

/* Call Open and Close endpoint at passive */
#define USB_MINIPORT_OPT_EP_OPENCLOSE_PASSIVE 0x00010000

/* UWB-WirelessUSB - Support for TT enabled hubs   */
#define USB_MINIPORT_OPT_TT_HUB_ENABLED       0x00020000

/* Allow Submit Transfer to return BUSY when no transfers are pending */
#define USB_MINIPORT_OPT_BUSY_WHILE_IDLE      0x00040000

/* Call MINIPORT_DisableInterrupts() and flush the ISR DPC before
 calling MINIPORT_SuspendController().  The miniport is responsible for
 reenabling the Root Hub port status change interrupt, if appropriate,
 before returning from MINIPORT_SuspendController().
*/
#define USB_MINIPORT_OPT_SUSPEND_DISABLE_IRQ  0x00080000

/* The miniport will not use double buffering for DMA transfers */
#define USB_MINIPORT_OPT_NO_DOUBLE_BUFFER     0x00100000

#define USB_MINIPORT_HCI_VERSION USB_MINIPORT_HCI_VERSION_5
#define USB_MINIPORT_HCI_VERSION_1            100
#define USB_MINIPORT_HCI_VERSION_2            200
#define USB_MINIPORT_HCI_VERSION_3            300
#define USB_MINIPORT_HCI_VERSION_310          310   // treated as version 4 going forward
#define USB_MINIPORT_HCI_VERSION_5            500

/*
    Bus Bandwidth defined by spec
*/
#define USB_11_BUS_BANDWIDTH                12000
#define USB_20_BUS_BANDWIDTH                400000

#define USB_HCI_MN                          0x10000001

/* version shipped with XP 2600 this must remaon unchanged */

typedef struct _USBPORT_REGISTRATION_PACKET_V1 {

    /* begin version (1) interface definition */

    /* Host Controller HCI Type */
    USB_HCI_TYPE HciType;

    /* registration parameters */
    ULONG OptionFlags;

    /*
       Total bus bandwidth avaibale in MBits
     */
    ULONG BusBandwidth;

    LONG Reserved;

    ULONG DeviceDataSize;
    ULONG EndpointDataSize;
    ULONG TransferContextSize;

    /* 7 char ASCII NULL terminated name */
    UCHAR Name[8];

    /*
        Amount of global common buffer needed
        this memory is passed to the miniport on
        a start and freed on a stop
    */
    ULONG CommonBufferBytes;

    /* miniport Functions */
    PMPFN_OPEN_ENDPOINT                   MINIPORT_OpenEndpoint;
    PMPFN_POKE_ENDPOINT                   MINIPORT_PokeEndpoint;
    PMPFN_QENDPOINT_REQUIREMENTS          MINIPORT_QueryEndpointRequirements;
    PMPFN_CLOSE_ENDPOINT                  MINIPORT_CloseEndpoint;
    PMPFN_START_CONTROLLER                MINIPORT_StartController;
    PMPFN_STOP_CONTROLLER                 MINIPORT_StopController;
    PMPFN_SUSPEND_CONTROLLER              MINIPORT_SuspendController;
    PMPFN_RESUME_CONTROLLER               MINIPORT_ResumeController;
    PMPFN_INTERRUPT_SERVICE               MINIPORT_InterruptService;
    PMPFN_INTERRUPT_DPC                   MINIPORT_InterruptDpc;
    PMPFN_SUBMIT_TRANSFER                 MINIPORT_SubmitTransfer;
    PMPFN_SUBMIT_ISO_TRANSFER             MINIPORT_SubmitIsoTransfer;
    PMPFN_ABORT_TRANSFER                  MINIPORT_AbortTransfer;
    PMPFN_GET_ENDPOINT_STATE              MINIPORT_GetEndpointState;
    PMPFN_SET_ENDPOINT_STATE              MINIPORT_SetEndpointState;
    PMPFN_POLL_ENDPOINT                   MINIPORT_PollEndpoint;
    PMPFN_CHECK_CONTROLLER                MINIPORT_CheckController;
    PMPFN_GET_32BIT_FRAME_NUMBER          MINIPORT_Get32BitFrameNumber;
    PMPFN_INTERRUPT_NEXT_SOF              MINIPORT_InterruptNextSOF;
    PMPFN_ENABLE_INTERRUPTS               MINIPORT_EnableInterrupts;
    PMPFN_DISABLE_INTERRUPTS              MINIPORT_DisableInterrupts;
    PMPFN_POLL_CONTROLLER                 MINIPORT_PollController;
    PMPFN_SET_ENDPOINT_DATA_TOGGLE        MINIPORT_SetEndpointDataToggle;
    PMPFN_GET_ENDPOINT_STATUS             MINIPORT_GetEndpointStatus;
    PMPFN_SET_ENDPOINT_STATUS             MINIPORT_SetEndpointStatus;
    PMPFN_RESET_CONTROLLER                MINIPORT_ResetController;

    /* root hub functions */
    PMPFN_RH_GET_ROOTHUB_DATA             MINIPORT_RH_GetRootHubData;
    PMPFN_RH_GET_STATUS                   MINIPORT_RH_GetStatus;
    PMPFN_RH_GET_PORT_STATUS              MINIPORT_RH_GetPortStatus;
    PMPFN_RH_GET_HUB_STATUS               MINIPORT_RH_GetHubStatus;
    /* root hub port functions */
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortReset;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnableChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortConnectChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortResetChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspendChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortOvercurrentChange;

    /* optional root hub functions */
    PMPFN_RH_DISABLE_IRQ                  MINIPORT_RH_DisableIrq;
    PMPFN_RH_ENABLE_IRQ                   MINIPORT_RH_EnableIrq;

    /* OPTIONAL DEBUG SERVICES

        Miniport should return USBMP_STATUS_NOT_SUPPORTED for the
        services not supported.
     */
    PMPFN_SEND_ONE_PACKET                 MINIPORT_StartSendOnePacket;
    PMPFN_SEND_ONE_PACKET                 MINIPORT_EndSendOnePacket;

    /* PASS-THRU API function

    */
    PMPFN_PASS_THRU                        MINIPORT_PassThru;


    /* interface Services */
    PPORTFN_DBGPRINT                            USBPORTSVC_DbgPrint;
    PPORTFN_TEST_DEBUG_BREAK                    USBPORTSVC_TestDebugBreak;
    PPORTFN_ASSERT_FAILURE                      USBPORTSVC_AssertFailure;
    PPORTFN_GET_MINIPORT_REGESTRY_KEY_VALUE     USBPORTSVC_GetMiniportRegistryKeyValue;
    PPORTFN_INVALIDATE_ROOTHUB                  USBPORTSVC_InvalidateRootHub;
    PPORTFN_INVALIDATE_ENDPOINT                 USBPORTSVC_InvalidateEndpoint;
    PPORTFN_COMPLETE_TRANSFER                   USBPORTSVC_CompleteTransfer;
    PPORTFN_COMPLETE_ISO_TRANSFER               USBPORTSVC_CompleteIsoTransfer;
    PPORTFN_LOGENTRY                            USBPORTSVC_LogEntry;
    PPORTFN_PHYS_TO_VIRTUAL                     USBPORTSVC_MapHwPhysicalToVirtual;
    PPORTFN_REQUEST_ASYNC_CALLBACK              USBPORTSVC_RequestAsyncCallback;
    PPORTFN_READWRITE_CONFIG_SPACE              USBPORTSVC_ReadWriteConfigSpace;
    PPORTFN_WAIT                                USBPORTSVC_Wait;
    PPORTFN_INVALIDATE_CONTROLLER               USBPORTSVC_InvalidateController;
    PPORTFN_BUGCHECK                            USBPORTSVC_BugCheck;
    PPORTFN_NOTIFY_DOUBLE_BUFFER                USBPORTSVC_NotifyDoubleBuffer;

    PMPFN_REBALANCE_ENDPOINT                    MINIPORT_RebalanceEndpoint;
    PMPFN_FLUSH_INTERRUPTS                      MINIPORT_FlushInterrupts;

} USBPORT_REGISTRATION_PACKET_V1, *PUSBPORT_REGISTRATION_PACKET_V1;

/*
    Miniport version 2 api packet
*/
typedef struct _USBPORT_REGISTRATION_PACKET_V2 {

    /* begin version (1) interface definition */

    /* Host Controller HCI Type */
    USB_HCI_TYPE HciType;

    /* registration parameters */
    ULONG OptionFlags;

    /*
       Total bus bandwidth avaibale in MBits
     */
    ULONG BusBandwidth;

    LONG Reserved;

    ULONG DeviceDataSize;
    ULONG EndpointDataSize;
    ULONG TransferContextSize;

    /* 7 char ASCII NULL terminated name */
    UCHAR Name[8];

    /*
        Amount of global common buffer needed
        this memory is passed to the miniport on
        a start and freed on a stop
    */
    ULONG CommonBufferBytes;

    /* miniport Functions */
    PMPFN_OPEN_ENDPOINT                   MINIPORT_OpenEndpoint;
    PMPFN_POKE_ENDPOINT                   MINIPORT_PokeEndpoint;
    PMPFN_QENDPOINT_REQUIREMENTS          MINIPORT_QueryEndpointRequirements;
    PMPFN_CLOSE_ENDPOINT                  MINIPORT_CloseEndpoint;
    PMPFN_START_CONTROLLER                MINIPORT_StartController;
    PMPFN_STOP_CONTROLLER                 MINIPORT_StopController;
    PMPFN_SUSPEND_CONTROLLER              MINIPORT_SuspendController;
    PMPFN_RESUME_CONTROLLER               MINIPORT_ResumeController;
    PMPFN_INTERRUPT_SERVICE               MINIPORT_InterruptService;
    PMPFN_INTERRUPT_DPC                   MINIPORT_InterruptDpc;
    PMPFN_SUBMIT_TRANSFER                 MINIPORT_SubmitTransfer;
    PMPFN_SUBMIT_ISO_TRANSFER             MINIPORT_SubmitIsoTransfer;
    PMPFN_ABORT_TRANSFER                  MINIPORT_AbortTransfer;
    PMPFN_GET_ENDPOINT_STATE              MINIPORT_GetEndpointState;
    PMPFN_SET_ENDPOINT_STATE              MINIPORT_SetEndpointState;
    PMPFN_POLL_ENDPOINT                   MINIPORT_PollEndpoint;
    PMPFN_CHECK_CONTROLLER                MINIPORT_CheckController;
    PMPFN_GET_32BIT_FRAME_NUMBER          MINIPORT_Get32BitFrameNumber;
    PMPFN_INTERRUPT_NEXT_SOF              MINIPORT_InterruptNextSOF;
    PMPFN_ENABLE_INTERRUPTS               MINIPORT_EnableInterrupts;
    PMPFN_DISABLE_INTERRUPTS              MINIPORT_DisableInterrupts;
    PMPFN_POLL_CONTROLLER                 MINIPORT_PollController;
    PMPFN_SET_ENDPOINT_DATA_TOGGLE        MINIPORT_SetEndpointDataToggle;
    PMPFN_GET_ENDPOINT_STATUS             MINIPORT_GetEndpointStatus;
    PMPFN_SET_ENDPOINT_STATUS             MINIPORT_SetEndpointStatus;
    PMPFN_RESET_CONTROLLER                MINIPORT_ResetController;

    /* root hub functions */
    PMPFN_RH_GET_ROOTHUB_DATA             MINIPORT_RH_GetRootHubData;
    PMPFN_RH_GET_STATUS                   MINIPORT_RH_GetStatus;
    PMPFN_RH_GET_PORT_STATUS              MINIPORT_RH_GetPortStatus;
    PMPFN_RH_GET_HUB_STATUS               MINIPORT_RH_GetHubStatus;
    /* root hub port functions */
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortReset;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnableChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortConnectChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortResetChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspendChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortOvercurrentChange;

    /* optional root hub functions */
    PMPFN_RH_DISABLE_IRQ                  MINIPORT_RH_DisableIrq;
    PMPFN_RH_ENABLE_IRQ                   MINIPORT_RH_EnableIrq;

    /* OPTIONAL DEBUG SERVICES

        Miniport should return USBMP_STATUS_NOT_SUPPORTED for the
        services not supported.
     */
    PMPFN_SEND_ONE_PACKET                 MINIPORT_StartSendOnePacket;
    PMPFN_SEND_ONE_PACKET                 MINIPORT_EndSendOnePacket;

    /* PASS-THRU API function

    */
    PMPFN_PASS_THRU                        MINIPORT_PassThru;


    /* interface Services */
    PPORTFN_DBGPRINT                            USBPORTSVC_DbgPrint;
    PPORTFN_TEST_DEBUG_BREAK                    USBPORTSVC_TestDebugBreak;
    PPORTFN_ASSERT_FAILURE                      USBPORTSVC_AssertFailure;
    PPORTFN_GET_MINIPORT_REGESTRY_KEY_VALUE     USBPORTSVC_GetMiniportRegistryKeyValue;
    PPORTFN_INVALIDATE_ROOTHUB                  USBPORTSVC_InvalidateRootHub;
    PPORTFN_INVALIDATE_ENDPOINT                 USBPORTSVC_InvalidateEndpoint;
    PPORTFN_COMPLETE_TRANSFER                   USBPORTSVC_CompleteTransfer;
    PPORTFN_COMPLETE_ISO_TRANSFER               USBPORTSVC_CompleteIsoTransfer;
    PPORTFN_LOGENTRY                            USBPORTSVC_LogEntry;
    PPORTFN_PHYS_TO_VIRTUAL                     USBPORTSVC_MapHwPhysicalToVirtual;
    PPORTFN_REQUEST_ASYNC_CALLBACK              USBPORTSVC_RequestAsyncCallback;
    PPORTFN_READWRITE_CONFIG_SPACE              USBPORTSVC_ReadWriteConfigSpace;
    PPORTFN_WAIT                                USBPORTSVC_Wait;
    PPORTFN_INVALIDATE_CONTROLLER               USBPORTSVC_InvalidateController;
    PPORTFN_BUGCHECK                            USBPORTSVC_BugCheck;
    PPORTFN_NOTIFY_DOUBLE_BUFFER                USBPORTSVC_NotifyDoubleBuffer;

    PMPFN_REBALANCE_ENDPOINT                    MINIPORT_RebalanceEndpoint;
    PMPFN_FLUSH_INTERRUPTS                      MINIPORT_FlushInterrupts;

    /* end version (1) definition */

    /* begin version (2) definition */
    PMPFN_RH_PORT_FUNCTION                      MINIPORT_Chirp_RH_Port;
    PMPFN_TAKE_PORT_CONTROL                     MINIPORT_TakePortControl;
    PVOID pad2;
    PVOID pad3;

    /* end version (2) definition */

} USBPORT_REGISTRATION_PACKET_V2, *PUSBPORT_REGISTRATION_PACKET_V2;

/*
    Miniport version 3  api packet  (LONGHORN Beta 1)
*/
typedef struct _USBPORT_REGISTRATION_PACKET_V3 {
    /* begin version (1) interface definition */

    /* Host Controller HCI Type */
    USB_HCI_TYPE HciType;

    /* registration parameters */
    ULONG OptionFlags;

    /*
       Total bus bandwidth avaibale in MBits
     */
    ULONG BusBandwidth;

    LONG Reserved;

    ULONG DeviceDataSize;
    ULONG EndpointDataSize;
    ULONG TransferContextSize;

    /* 7 char ASCII NULL terminated name */
    UCHAR Name[8];

    /*
        Amount of global common buffer needed this memory is passed to the miniport on a start and freed on a stop.
    */
    ULONG CommonBufferBytes;


    /* miniport Functions */
    PMPFN_OPEN_ENDPOINT                   MINIPORT_OpenEndpoint;
    PMPFN_POKE_ENDPOINT                   MINIPORT_PokeEndpoint;
    PMPFN_QENDPOINT_REQUIREMENTS          MINIPORT_QueryEndpointRequirements;
    PMPFN_CLOSE_ENDPOINT                  MINIPORT_CloseEndpoint;
    PMPFN_START_CONTROLLER                MINIPORT_StartController;
    PMPFN_STOP_CONTROLLER                 MINIPORT_StopController;
    PMPFN_SUSPEND_CONTROLLER              MINIPORT_SuspendController;
    PMPFN_RESUME_CONTROLLER               MINIPORT_ResumeController;
    PMPFN_INTERRUPT_SERVICE               MINIPORT_InterruptService;
    PMPFN_INTERRUPT_DPC                   MINIPORT_InterruptDpc;
    PMPFN_SUBMIT_TRANSFER                 MINIPORT_SubmitTransfer;
    PMPFN_SUBMIT_ISO_TRANSFER             MINIPORT_SubmitIsoTransfer;
    PMPFN_ABORT_TRANSFER                  MINIPORT_AbortTransfer;
    PMPFN_GET_ENDPOINT_STATE              MINIPORT_GetEndpointState;
    PMPFN_SET_ENDPOINT_STATE              MINIPORT_SetEndpointState;
    PMPFN_POLL_ENDPOINT                   MINIPORT_PollEndpoint;
    PMPFN_CHECK_CONTROLLER                MINIPORT_CheckController;
    PMPFN_GET_32BIT_FRAME_NUMBER          MINIPORT_Get32BitFrameNumber;
    PMPFN_INTERRUPT_NEXT_SOF              MINIPORT_InterruptNextSOF;
    PMPFN_ENABLE_INTERRUPTS               MINIPORT_EnableInterrupts;
    PMPFN_DISABLE_INTERRUPTS              MINIPORT_DisableInterrupts;
    PMPFN_POLL_CONTROLLER                 MINIPORT_PollController;
    PMPFN_SET_ENDPOINT_DATA_TOGGLE        MINIPORT_SetEndpointDataToggle;
    PMPFN_GET_ENDPOINT_STATUS             MINIPORT_GetEndpointStatus;
    PMPFN_SET_ENDPOINT_STATUS             MINIPORT_SetEndpointStatus;
    PMPFN_RESET_CONTROLLER                MINIPORT_ResetController;

    /* root hub functions */
    PMPFN_RH_GET_ROOTHUB_DATA             MINIPORT_RH_GetRootHubData;
    PMPFN_RH_GET_STATUS                   MINIPORT_RH_GetStatus;
    PMPFN_RH_GET_PORT_STATUS              MINIPORT_RH_GetPortStatus;
    PMPFN_RH_GET_HUB_STATUS               MINIPORT_RH_GetHubStatus;
    /* root hub port functions */
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortReset;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnableChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortConnectChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortResetChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspendChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortOvercurrentChange;

    /* optional root hub functions */
    PMPFN_RH_DISABLE_IRQ                  MINIPORT_RH_DisableIrq;
    PMPFN_RH_ENABLE_IRQ                   MINIPORT_RH_EnableIrq;

    /* OPTIONAL DEBUG SERVICES

        Miniport should return USBMP_STATUS_NOT_SUPPORTED for the
        services not supported.
     */
    PMPFN_SEND_ONE_PACKET                 MINIPORT_StartSendOnePacket;
    PMPFN_SEND_ONE_PACKET                 MINIPORT_EndSendOnePacket;

    /* PASS-THRU API function

    */
    PMPFN_PASS_THRU                        MINIPORT_PassThru;


    /* interface Services */
    PPORTFN_DBGPRINT                            USBPORTSVC_DbgPrint;
    PPORTFN_TEST_DEBUG_BREAK                    USBPORTSVC_TestDebugBreak;
    PPORTFN_ASSERT_FAILURE                      USBPORTSVC_AssertFailure;
    PPORTFN_GET_MINIPORT_REGESTRY_KEY_VALUE     USBPORTSVC_GetMiniportRegistryKeyValue;
    PPORTFN_INVALIDATE_ROOTHUB                  USBPORTSVC_InvalidateRootHub;
    PPORTFN_INVALIDATE_ENDPOINT                 USBPORTSVC_InvalidateEndpoint;
    PPORTFN_COMPLETE_TRANSFER                   USBPORTSVC_CompleteTransfer;
    PPORTFN_COMPLETE_ISO_TRANSFER               USBPORTSVC_CompleteIsoTransfer;
    PPORTFN_LOGENTRY                            USBPORTSVC_LogEntry;
    PPORTFN_PHYS_TO_VIRTUAL                     USBPORTSVC_MapHwPhysicalToVirtual;
    PPORTFN_REQUEST_ASYNC_CALLBACK              USBPORTSVC_RequestAsyncCallback;
    PPORTFN_READWRITE_CONFIG_SPACE              USBPORTSVC_ReadWriteConfigSpace;
    PPORTFN_WAIT                                USBPORTSVC_Wait;
    PPORTFN_INVALIDATE_CONTROLLER               USBPORTSVC_InvalidateController;
    PPORTFN_BUGCHECK                            USBPORTSVC_BugCheck;
    PPORTFN_NOTIFY_DOUBLE_BUFFER                USBPORTSVC_NotifyDoubleBuffer;

    PMPFN_REBALANCE_ENDPOINT                    MINIPORT_RebalanceEndpoint;
    PMPFN_FLUSH_INTERRUPTS                      MINIPORT_FlushInterrupts;

    /* end version (1) definition */

    /* begin version (2) definition */
    PMPFN_RH_PORT_FUNCTION                      MINIPORT_Chirp_RH_Port;
    PMPFN_TAKE_PORT_CONTROL                     MINIPORT_TakePortControl;

    /* end version (2) definition */

    /* begin version (3) definition */
    PVOID                                       RegistryPath;
    PVOID pad3;

    /*
    Optional extra blocks of common buffer this allows the miniport to break up large schedule  structures if it doesn't
    require all the memory to be physically contiguous.

    If the miniport does not need additional blocks then it should report block count of zero.
    */

    ULONG CommonBufferBlockCount;
    ULONG CommonBufferBlockBytes[8];

    /* Size of 'High Speed Bus' context structures for interface to HS budget code */
    /* if miniport is USB 2.0 high speed controller context */
    ULONG HsbContextSize;
    /* if miniport is USB 2.0 then tt's must be managed as well */
    ULONG HsbTtContextSize;
    /* if miniport is USB 2.0 then we need extra context for budgeter */
    ULONG HsbEpContextSize;

    /* normal (rev3) functions (not USBX) */
    PMPFN_TAKE_PORT_CONTROL                     MINIPORT_ReleasePortControl;
    PMPFN_READ_CFG_FLAG                         MINIPORT_ReadCfgFlag;
    PMPFN_SET_WAKE_ON_CONNECT                   MINIPORT_SetWakeOnConnect;
    PMPFN_INTERRUPT_DPC_EX                      MINIPORT_InterruptDpcEx;
    PMPFN_NOTIFY_TRANSFER_QUEUE_STATE           MINIPORT_NotifyTransferQueueState;
    PMPFN_POLL_HW_SYNC                          MINIPORT_PollHwSync;

    /* USBX functions */
    PMPFN_USBX_INIT_HSB_TT                      MINIPORT_UsbxInitHsbTransactionTranslator;
    PMPFN_USBX_INIT_HSB_CONTROLLER              MINIPORT_UsbxInitHsbController;
    PMPFN_USBX_INIT_HSB_ENDPOINT                MINIPORT_UsbxInitHsbEndpoint;

    PMPFN_USBX_ALLOCATE_BANDWIDTH               MINIPORT_UsbxAllocateBandwidth;
    PMPFN_USBX_FREE_BANDWIDTH                   MINIPORT_UsbxFreeBandwidth;
    PMPFN_USBX_QUERY_ENDPOINT_REQUIREMENTS      MINIPORT_UsbxQueryEndpointRequirements;
    PMPFN_USBX_POKE_ENDPOINT                    MINIPORT_UsbxPokeEndpoint;
    PMPFN_USBX_OPEN_ENDPOINT                    MINIPORT_UsbxOpenEndpoint;

    PMPFN_USBX_QUERY_BW_DATA                    MINIPORT_UsbxQueryBandwidthData;
    PMPFN_USBX_QUERY_TT_BW_DATA                 MINIPORT_UsbxQueryTtBandwidthData;
    PMPFN_USBX_QUERY_EP_BW_DATA                 MINIPORT_UsbxQueryEpBandwidthData;

    PPORTFN_REQUEST_ASYNC_CALLBACK_EX           USBPORTSVC_RequestAsyncCallbackEx;
    PPORTFN_CANCEL_ASYNC_CALLBACK               USBPORTSVC_CancelAsyncCallback;

    /* wireless add-ons */
    /* per USB function context */
    ULONG DevHContextSize;
    // (optional) may be set to NULL by miniport
    PMPFN_OPEN_DEVH                             MINIPORT_OpenDevHandle;
    PMPFN_CLOSE_DEVH                            MINIPORT_CloseDevHandle;
    PMPFN_CREATE_DEVICE_DATA                    MINIPORT_CreateDeviceData;
    PMPFN_DELETE_DEVICE_DATA                    MINIPORT_DeleteDeviceData;
    /* called just before usbport frees the memory */
    PMPFN_DBG_FREE_ENDPOINT                     MINIPORT_DbgFreeEndpoint;
    PMPFN_DBG_FREE_DEVICE_DATA                  MINIPORT_DgbFreeDeviceData;
    PMPFN_DBG_FREE_DEVH                         MINIPORT_DgbFreeDevHandle;

    PMPFN_HALT_CONTROLLER                       MINIPORT_HaltController;
//#ifdef VISTA_RTM
    // the following functions will be added to miniport if ver 3 post Vista Beta 2:

    PMPFN_GET_32BIT_FRAME_NUMBER                MINIPORT_Get32BitMicroFrameNumber;
//#endif  // VISTA_RTM
    /* end version (3) definition */

} USBPORT_REGISTRATION_PACKET_V3, *PUSBPORT_REGISTRATION_PACKET_V3;


/*
    Miniport version 3.10/4  api packet  (Wireless USB support)
*/


typedef struct _USBPORT_REGISTRATION_PACKET_V4 {

    /* begin version (1) interface definition */

    /* Host Controller HCI Type */
    USB_HCI_TYPE HciType;

    /* registration parameters */
    ULONG OptionFlags;

    /*
       Total bus bandwidth avaibale in MBits
     */
    ULONG BusBandwidth;

    LONG Reserved;

    ULONG DeviceDataSize;
    ULONG EndpointDataSize;
    ULONG TransferContextSize;

    /* 7 char ASCII NULL terminated name */
    UCHAR Name[8];

    /*
        Amount of global common buffer needed this memory is passed to the miniport on a start and freed on a stop.
    */
    ULONG CommonBufferBytes;


    /* miniport Functions */
    PMPFN_OPEN_ENDPOINT                   MINIPORT_OpenEndpoint;
    PMPFN_POKE_ENDPOINT                   MINIPORT_PokeEndpoint;
    PMPFN_QENDPOINT_REQUIREMENTS          MINIPORT_QueryEndpointRequirements;
    PMPFN_CLOSE_ENDPOINT                  MINIPORT_CloseEndpoint;
    PMPFN_START_CONTROLLER                MINIPORT_StartController;
    PMPFN_STOP_CONTROLLER                 MINIPORT_StopController;
    PMPFN_SUSPEND_CONTROLLER              MINIPORT_SuspendController;
    PMPFN_RESUME_CONTROLLER               MINIPORT_ResumeController;
    PMPFN_INTERRUPT_SERVICE               MINIPORT_InterruptService;
    PMPFN_INTERRUPT_DPC                   MINIPORT_InterruptDpc;
    PMPFN_SUBMIT_TRANSFER                 MINIPORT_SubmitTransfer;
    PMPFN_SUBMIT_ISO_TRANSFER             MINIPORT_SubmitIsoTransfer;
    PMPFN_ABORT_TRANSFER                  MINIPORT_AbortTransfer;
    PMPFN_GET_ENDPOINT_STATE              MINIPORT_GetEndpointState;
    PMPFN_SET_ENDPOINT_STATE              MINIPORT_SetEndpointState;
    PMPFN_POLL_ENDPOINT                   MINIPORT_PollEndpoint;
    PMPFN_CHECK_CONTROLLER                MINIPORT_CheckController;
    PMPFN_GET_32BIT_FRAME_NUMBER          MINIPORT_Get32BitFrameNumber;
    PMPFN_INTERRUPT_NEXT_SOF              MINIPORT_InterruptNextSOF;
    PMPFN_ENABLE_INTERRUPTS               MINIPORT_EnableInterrupts;
    PMPFN_DISABLE_INTERRUPTS              MINIPORT_DisableInterrupts;
    PMPFN_POLL_CONTROLLER                 MINIPORT_PollController;
    PMPFN_SET_ENDPOINT_DATA_TOGGLE        MINIPORT_SetEndpointDataToggle;
    PMPFN_GET_ENDPOINT_STATUS             MINIPORT_GetEndpointStatus;
    PMPFN_SET_ENDPOINT_STATUS             MINIPORT_SetEndpointStatus;
    PMPFN_RESET_CONTROLLER                MINIPORT_ResetController;

    /* root hub functions */
    PMPFN_RH_GET_ROOTHUB_DATA             MINIPORT_RH_GetRootHubData;
    PMPFN_RH_GET_STATUS                   MINIPORT_RH_GetStatus;
    PMPFN_RH_GET_PORT_STATUS              MINIPORT_RH_GetPortStatus;
    PMPFN_RH_GET_HUB_STATUS               MINIPORT_RH_GetHubStatus;
    /* root hub port functions */
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortReset;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnableChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortConnectChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortResetChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspendChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortOvercurrentChange;

    /* optional root hub functions */
    PMPFN_RH_DISABLE_IRQ                  MINIPORT_RH_DisableIrq;
    PMPFN_RH_ENABLE_IRQ                   MINIPORT_RH_EnableIrq;

    /* OPTIONAL DEBUG SERVICES

        Miniport should return USBMP_STATUS_NOT_SUPPORTED for the
        services not supported.
     */
    PMPFN_SEND_ONE_PACKET                 MINIPORT_StartSendOnePacket;
    PMPFN_SEND_ONE_PACKET                 MINIPORT_EndSendOnePacket;

    /* PASS-THRU API function

    */
    PMPFN_PASS_THRU                        MINIPORT_PassThru;


    /* interface Services */
    PPORTFN_DBGPRINT                            USBPORTSVC_DbgPrint;
    PPORTFN_TEST_DEBUG_BREAK                    USBPORTSVC_TestDebugBreak;
    PPORTFN_ASSERT_FAILURE                      USBPORTSVC_AssertFailure;
    PPORTFN_GET_MINIPORT_REGESTRY_KEY_VALUE     USBPORTSVC_GetMiniportRegistryKeyValue;
    PPORTFN_INVALIDATE_ROOTHUB                  USBPORTSVC_InvalidateRootHub;
    PPORTFN_INVALIDATE_ENDPOINT                 USBPORTSVC_InvalidateEndpoint;
    PPORTFN_COMPLETE_TRANSFER                   USBPORTSVC_CompleteTransfer;
    PPORTFN_COMPLETE_ISO_TRANSFER               USBPORTSVC_CompleteIsoTransfer;
    PPORTFN_LOGENTRY                            USBPORTSVC_LogEntry;
    PPORTFN_PHYS_TO_VIRTUAL                     USBPORTSVC_MapHwPhysicalToVirtual;
    PPORTFN_REQUEST_ASYNC_CALLBACK              USBPORTSVC_RequestAsyncCallback;
    PPORTFN_READWRITE_CONFIG_SPACE              USBPORTSVC_ReadWriteConfigSpace;
    PPORTFN_WAIT                                USBPORTSVC_Wait;
    PPORTFN_INVALIDATE_CONTROLLER               USBPORTSVC_InvalidateController;
    PPORTFN_BUGCHECK                            USBPORTSVC_BugCheck;
    PPORTFN_NOTIFY_DOUBLE_BUFFER                USBPORTSVC_NotifyDoubleBuffer;

    PMPFN_REBALANCE_ENDPOINT                    MINIPORT_RebalanceEndpoint;
    PMPFN_FLUSH_INTERRUPTS                      MINIPORT_FlushInterrupts;

    /* end version (1) definition */

    /* begin version (2) definition */
    PMPFN_RH_PORT_FUNCTION                      MINIPORT_Chirp_RH_Port;
    PMPFN_TAKE_PORT_CONTROL                     MINIPORT_TakePortControl;

    /* end version (2) definition */

    /* begin version (3) definition */
    PVOID                                       RegistryPath;
    PVOID pad3;

    /*
    Optional extra blocks of common buffer this allows the miniport to break up large schedule  structures if it doesn't
    require all the memory to be physically contiguous.

    If the miniport does not need additional blocks then it should report block count of zero.
    */

    ULONG CommonBufferBlockCount;
    ULONG CommonBufferBlockBytes[8];

    /* Size of 'High Speed Bus' context structures for interface to HS budget code */
    /* if miniport is USB 2.0 high speed controller context */
    ULONG HsbContextSize;
    /* if miniport is USB 2.0 then tt's must be managed as well */
    ULONG HsbTtContextSize;
    /* if miniport is USB 2.0 then we need extra context for budgeter */
    ULONG HsbEpContextSize;

    /* normal (rev3) functions (not USBX) */
    PMPFN_TAKE_PORT_CONTROL                     MINIPORT_ReleasePortControl;
    PMPFN_READ_CFG_FLAG                         MINIPORT_ReadCfgFlag;
    PMPFN_SET_WAKE_ON_CONNECT                   MINIPORT_SetWakeOnConnect;
    PMPFN_INTERRUPT_DPC_EX                      MINIPORT_InterruptDpcEx;
    PMPFN_NOTIFY_TRANSFER_QUEUE_STATE           MINIPORT_NotifyTransferQueueState;
    PMPFN_POLL_HW_SYNC                          MINIPORT_PollHwSync;

    /* USBX functions */
    PMPFN_USBX_INIT_HSB_TT                      MINIPORT_UsbxInitHsbTransactionTranslator;
    PMPFN_USBX_INIT_HSB_CONTROLLER              MINIPORT_UsbxInitHsbController;
    PMPFN_USBX_INIT_HSB_ENDPOINT                MINIPORT_UsbxInitHsbEndpoint;

    PMPFN_USBX_ALLOCATE_BANDWIDTH               MINIPORT_UsbxAllocateBandwidth;
    PMPFN_USBX_FREE_BANDWIDTH                   MINIPORT_UsbxFreeBandwidth;
    PMPFN_USBX_QUERY_ENDPOINT_REQUIREMENTS      MINIPORT_UsbxQueryEndpointRequirements;
    PMPFN_USBX_POKE_ENDPOINT                    MINIPORT_UsbxPokeEndpoint;
    PMPFN_USBX_OPEN_ENDPOINT                    MINIPORT_UsbxOpenEndpoint;

    PMPFN_USBX_QUERY_BW_DATA                    MINIPORT_UsbxQueryBandwidthData;
    PMPFN_USBX_QUERY_TT_BW_DATA                 MINIPORT_UsbxQueryTtBandwidthData;
    PMPFN_USBX_QUERY_EP_BW_DATA                 MINIPORT_UsbxQueryEpBandwidthData;

    PPORTFN_REQUEST_ASYNC_CALLBACK_EX           USBPORTSVC_RequestAsyncCallbackEx;
    PPORTFN_CANCEL_ASYNC_CALLBACK               USBPORTSVC_CancelAsyncCallback;

    /* wireless add-ons */
    /* per USB function context */
    ULONG DevHContextSize;
    // (optional) may be set to NULL by miniport
    PMPFN_OPEN_DEVH                             MINIPORT_OpenDevHandle;
    PMPFN_CLOSE_DEVH                            MINIPORT_CloseDevHandle;
    PMPFN_CREATE_DEVICE_DATA                    MINIPORT_CreateDeviceData;
    PMPFN_DELETE_DEVICE_DATA                    MINIPORT_DeleteDeviceData;
    /* called just before usbport frees the memory */
    PMPFN_DBG_FREE_ENDPOINT                     MINIPORT_DbgFreeEndpoint;
    PMPFN_DBG_FREE_DEVICE_DATA                  MINIPORT_DgbFreeDeviceData;
    PMPFN_DBG_FREE_DEVH                         MINIPORT_DgbFreeDevHandle;

    PMPFN_HALT_CONTROLLER                       MINIPORT_HaltController;
//#ifdef VISTA_RTM
    // the following functions will be added to miniport if ver 3 post Vista Beta 2:

    PMPFN_GET_32BIT_FRAME_NUMBER                MINIPORT_Get32BitMicroFrameNumber;
//#endif  // VISTA_RTM
    /* end version (3) definition */

    /* begin version <3.10/> definition */

    PMPFN_RH_ASYNC_COMMAND                      MINIPORT_RH_AsyncCommand;

    PPORTFN_RH_COMMAND_COMPLETE                 USBPORTSVC_Rh_Complete;

    PMPFN_STOP_CONTROLLER_WITH_INTERRUPTS       MINIPORT_StopControllerWithInterrupts;

    PPORTFN_ETW_WRITE                           USBPORTSVC_EtwWrite;

    PMPFN_RH_GET_CONTAINER_ID_FOR_PORT          MINIPORT_RH_GetContainerIdForPort;

    PMPFN_RH_SET_CONTAINER_ID_FOR_PORT          MINIPORT_RH_SetContainerIdForPort;

    PMPFN_GET_HC_CAPABILITIES                   MINIPORT_GetHcCapabilities;

    PMPFN_INTERFACE_INITIALIZE                  MINIPORT_InitializeDumpInterface;

    PMPFN_HC_CLEANUP                            MINIPORT_CleanupDumpInterface;

    PMPFN_GET_DEBUG_PORT_NUMBER                 MINIPORT_GetDebugPortNumber;

    PPORTFN_IS_INTERRUPT_PER_TD_ENABLED         USBPORTSVC_IsInterruptPerTdEnabled;

    PPORTFN_IGNORE_RESUME_SIGNALING             USBPORTSVC_IgnoreResumeSignaling;

    PMPFN_RESET_CONTROLLER_DONT_REINIT          MINIPORT_ResetControllerDontReinit;

    PMPFN_POST_RESET_CONTROLLER                 MINIPORT_PostResetController;

    PMPFN_REINIT_CONTROLLER                     MINIPORT_ReinitController;

    PMPFN_CHECK_INVALIDATE_ROOT_HUB             MINIPORT_CheckInvalidateRootHub;

    PMPFN_SET_PORT_RESUME_TIME                  MINIPORT_SetPortResumeTime;

    /* end version (3.10/4) definition */

} USBPORT_REGISTRATION_PACKET_V4, *PUSBPORT_REGISTRATION_PACKET_V4;

/*
    Miniport version 5 api packet  (add a pre start controller routine)
*/

typedef struct _USBPORT_REGISTRATION_PACKET {

    /* begin version (1) interface definition */

    /* Host Controller HCI Type */
    USB_HCI_TYPE HciType;

    /* registration parameters */
    ULONG OptionFlags;

    /*
       Total bus bandwidth avaibale in MBits
     */
    ULONG BusBandwidth;

    LONG Reserved;

    ULONG DeviceDataSize;
    ULONG EndpointDataSize;
    ULONG TransferContextSize;

    /* 7 char ASCII NULL terminated name */
    UCHAR Name[8];

    /*
        Amount of global common buffer needed this memory is passed to the miniport on a start and freed on a stop.
    */
    ULONG CommonBufferBytes;


    /* miniport Functions */
    PMPFN_OPEN_ENDPOINT                   MINIPORT_OpenEndpoint;
    PMPFN_POKE_ENDPOINT                   MINIPORT_PokeEndpoint;
    PMPFN_QENDPOINT_REQUIREMENTS          MINIPORT_QueryEndpointRequirements;
    PMPFN_CLOSE_ENDPOINT                  MINIPORT_CloseEndpoint;
    PMPFN_START_CONTROLLER                MINIPORT_StartController;
    PMPFN_STOP_CONTROLLER                 MINIPORT_StopController;
    PMPFN_SUSPEND_CONTROLLER              MINIPORT_SuspendController;
    PMPFN_RESUME_CONTROLLER               MINIPORT_ResumeController;
    PMPFN_INTERRUPT_SERVICE               MINIPORT_InterruptService;
    PMPFN_INTERRUPT_DPC                   MINIPORT_InterruptDpc;
    PMPFN_SUBMIT_TRANSFER                 MINIPORT_SubmitTransfer;
    PMPFN_SUBMIT_ISO_TRANSFER             MINIPORT_SubmitIsoTransfer;
    PMPFN_ABORT_TRANSFER                  MINIPORT_AbortTransfer;
    PMPFN_GET_ENDPOINT_STATE              MINIPORT_GetEndpointState;
    PMPFN_SET_ENDPOINT_STATE              MINIPORT_SetEndpointState;
    PMPFN_POLL_ENDPOINT                   MINIPORT_PollEndpoint;
    PMPFN_CHECK_CONTROLLER                MINIPORT_CheckController;
    PMPFN_GET_32BIT_FRAME_NUMBER          MINIPORT_Get32BitFrameNumber;
    PMPFN_INTERRUPT_NEXT_SOF              MINIPORT_InterruptNextSOF;
    PMPFN_ENABLE_INTERRUPTS               MINIPORT_EnableInterrupts;
    PMPFN_DISABLE_INTERRUPTS              MINIPORT_DisableInterrupts;
    PMPFN_POLL_CONTROLLER                 MINIPORT_PollController;
    PMPFN_SET_ENDPOINT_DATA_TOGGLE        MINIPORT_SetEndpointDataToggle;
    PMPFN_GET_ENDPOINT_STATUS             MINIPORT_GetEndpointStatus;
    PMPFN_SET_ENDPOINT_STATUS             MINIPORT_SetEndpointStatus;
    PMPFN_RESET_CONTROLLER                MINIPORT_ResetController;

    /* root hub functions */
    PMPFN_RH_GET_ROOTHUB_DATA             MINIPORT_RH_GetRootHubData;
    PMPFN_RH_GET_STATUS                   MINIPORT_RH_GetStatus;
    PMPFN_RH_GET_PORT_STATUS              MINIPORT_RH_GetPortStatus;
    PMPFN_RH_GET_HUB_STATUS               MINIPORT_RH_GetHubStatus;
    /* root hub port functions */
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortReset;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_SetFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnable;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortPower;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspend;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortEnableChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortConnectChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortResetChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortSuspendChange;
    PMPFN_RH_PORT_FUNCTION      MINIPORT_RH_ClearFeaturePortOvercurrentChange;

    /* optional root hub functions */
    PMPFN_RH_DISABLE_IRQ                  MINIPORT_RH_DisableIrq;
    PMPFN_RH_ENABLE_IRQ                   MINIPORT_RH_EnableIrq;

    /* OPTIONAL DEBUG SERVICES

        Miniport should return USBMP_STATUS_NOT_SUPPORTED for the
        services not supported.
     */
    PMPFN_SEND_ONE_PACKET                 MINIPORT_StartSendOnePacket;
    PMPFN_SEND_ONE_PACKET                 MINIPORT_EndSendOnePacket;

    /* PASS-THRU API function

    */
    PMPFN_PASS_THRU                        MINIPORT_PassThru;


    /* interface Services */
    PPORTFN_DBGPRINT                            USBPORTSVC_DbgPrint;
    PPORTFN_TEST_DEBUG_BREAK                    USBPORTSVC_TestDebugBreak;
    PPORTFN_ASSERT_FAILURE                      USBPORTSVC_AssertFailure;
    PPORTFN_GET_MINIPORT_REGESTRY_KEY_VALUE     USBPORTSVC_GetMiniportRegistryKeyValue;
    PPORTFN_INVALIDATE_ROOTHUB                  USBPORTSVC_InvalidateRootHub;
    PPORTFN_INVALIDATE_ENDPOINT                 USBPORTSVC_InvalidateEndpoint;
    PPORTFN_COMPLETE_TRANSFER                   USBPORTSVC_CompleteTransfer;
    PPORTFN_COMPLETE_ISO_TRANSFER               USBPORTSVC_CompleteIsoTransfer;
    PPORTFN_LOGENTRY                            USBPORTSVC_LogEntry;
    PPORTFN_PHYS_TO_VIRTUAL                     USBPORTSVC_MapHwPhysicalToVirtual;
    PPORTFN_REQUEST_ASYNC_CALLBACK              USBPORTSVC_RequestAsyncCallback;
    PPORTFN_READWRITE_CONFIG_SPACE              USBPORTSVC_ReadWriteConfigSpace;
    PPORTFN_WAIT                                USBPORTSVC_Wait;
    PPORTFN_INVALIDATE_CONTROLLER               USBPORTSVC_InvalidateController;
    PPORTFN_BUGCHECK                            USBPORTSVC_BugCheck;
    PPORTFN_NOTIFY_DOUBLE_BUFFER                USBPORTSVC_NotifyDoubleBuffer;

    PMPFN_REBALANCE_ENDPOINT                    MINIPORT_RebalanceEndpoint;
    PMPFN_FLUSH_INTERRUPTS                      MINIPORT_FlushInterrupts;

    /* end version (1) definition */

    /* begin version (2) definition */
    PMPFN_RH_PORT_FUNCTION                      MINIPORT_Chirp_RH_Port;
    PMPFN_TAKE_PORT_CONTROL                     MINIPORT_TakePortControl;

    /* end version (2) definition */

    /* begin version (3) definition */
    PVOID                                       RegistryPath;
    PVOID pad3;

    /*
    Optional extra blocks of common buffer this allows the miniport to break up large schedule  structures if it doesn't
    require all the memory to be physically contiguous.

    If the miniport does not need additional blocks then it should report block count of zero.
    */

    ULONG CommonBufferBlockCount;
    ULONG CommonBufferBlockBytes[8];

    /* Size of 'High Speed Bus' context structures for interface to HS budget code */
    /* if miniport is USB 2.0 high speed controller context */
    ULONG HsbContextSize;
    /* if miniport is USB 2.0 then tt's must be managed as well */
    ULONG HsbTtContextSize;
    /* if miniport is USB 2.0 then we need extra context for budgeter */
    ULONG HsbEpContextSize;

    /* normal (rev3) functions (not USBX) */
    PMPFN_TAKE_PORT_CONTROL                     MINIPORT_ReleasePortControl;
    PMPFN_READ_CFG_FLAG                         MINIPORT_ReadCfgFlag;
    PMPFN_SET_WAKE_ON_CONNECT                   MINIPORT_SetWakeOnConnect;
    PMPFN_INTERRUPT_DPC_EX                      MINIPORT_InterruptDpcEx;
    PMPFN_NOTIFY_TRANSFER_QUEUE_STATE           MINIPORT_NotifyTransferQueueState;
    PMPFN_POLL_HW_SYNC                          MINIPORT_PollHwSync;

    /* USBX functions */
    PMPFN_USBX_INIT_HSB_TT                      MINIPORT_UsbxInitHsbTransactionTranslator;
    PMPFN_USBX_INIT_HSB_CONTROLLER              MINIPORT_UsbxInitHsbController;
    PMPFN_USBX_INIT_HSB_ENDPOINT                MINIPORT_UsbxInitHsbEndpoint;

    PMPFN_USBX_ALLOCATE_BANDWIDTH               MINIPORT_UsbxAllocateBandwidth;
    PMPFN_USBX_FREE_BANDWIDTH                   MINIPORT_UsbxFreeBandwidth;
    PMPFN_USBX_QUERY_ENDPOINT_REQUIREMENTS      MINIPORT_UsbxQueryEndpointRequirements;
    PMPFN_USBX_POKE_ENDPOINT                    MINIPORT_UsbxPokeEndpoint;
    PMPFN_USBX_OPEN_ENDPOINT                    MINIPORT_UsbxOpenEndpoint;

    PMPFN_USBX_QUERY_BW_DATA                    MINIPORT_UsbxQueryBandwidthData;
    PMPFN_USBX_QUERY_TT_BW_DATA                 MINIPORT_UsbxQueryTtBandwidthData;
    PMPFN_USBX_QUERY_EP_BW_DATA                 MINIPORT_UsbxQueryEpBandwidthData;

    PPORTFN_REQUEST_ASYNC_CALLBACK_EX           USBPORTSVC_RequestAsyncCallbackEx;
    PPORTFN_CANCEL_ASYNC_CALLBACK               USBPORTSVC_CancelAsyncCallback;

    /* wireless add-ons */
    /* per USB function context */
    ULONG DevHContextSize;
    // (optional) may be set to NULL by miniport
    PMPFN_OPEN_DEVH                             MINIPORT_OpenDevHandle;
    PMPFN_CLOSE_DEVH                            MINIPORT_CloseDevHandle;
    PMPFN_CREATE_DEVICE_DATA                    MINIPORT_CreateDeviceData;
    PMPFN_DELETE_DEVICE_DATA                    MINIPORT_DeleteDeviceData;
    /* called just before usbport frees the memory */
    PMPFN_DBG_FREE_ENDPOINT                     MINIPORT_DbgFreeEndpoint;
    PMPFN_DBG_FREE_DEVICE_DATA                  MINIPORT_DgbFreeDeviceData;
    PMPFN_DBG_FREE_DEVH                         MINIPORT_DgbFreeDevHandle;

    PMPFN_HALT_CONTROLLER                       MINIPORT_HaltController;
//#ifdef VISTA_RTM
    // the following functions will be added to miniport if ver 3 post Vista Beta 2:

    PMPFN_GET_32BIT_FRAME_NUMBER                MINIPORT_Get32BitMicroFrameNumber;
//#endif  // VISTA_RTM
    /* end version (3) definition */

    /* begin version <3.10/4> definition */

    PMPFN_RH_ASYNC_COMMAND                      MINIPORT_RH_AsyncCommand;

    PPORTFN_RH_COMMAND_COMPLETE                 USBPORTSVC_Rh_Complete;

    PMPFN_STOP_CONTROLLER_WITH_INTERRUPTS       MINIPORT_StopControllerWithInterrupts;

    PPORTFN_ETW_WRITE                           USBPORTSVC_EtwWrite;

    PMPFN_RH_GET_CONTAINER_ID_FOR_PORT          MINIPORT_RH_GetContainerIdForPort;

    PMPFN_RH_SET_CONTAINER_ID_FOR_PORT          MINIPORT_RH_SetContainerIdForPort;

    PMPFN_GET_HC_CAPABILITIES                   MINIPORT_GetHcCapabilities;

    PMPFN_INTERFACE_INITIALIZE                  MINIPORT_InitializeDumpInterface;

    PMPFN_HC_CLEANUP                            MINIPORT_CleanupDumpInterface;

    PMPFN_GET_DEBUG_PORT_NUMBER                 MINIPORT_GetDebugPortNumber;

    PPORTFN_IS_INTERRUPT_PER_TD_ENABLED         USBPORTSVC_IsInterruptPerTdEnabled;

    PPORTFN_IGNORE_RESUME_SIGNALING             USBPORTSVC_IgnoreResumeSignaling;

    PMPFN_RESET_CONTROLLER_DONT_REINIT          MINIPORT_ResetControllerDontReinit;

    PMPFN_POST_RESET_CONTROLLER                 MINIPORT_PostResetController;

    PMPFN_REINIT_CONTROLLER                     MINIPORT_ReinitController;

    PMPFN_CHECK_INVALIDATE_ROOT_HUB             MINIPORT_CheckInvalidateRootHub;

    PMPFN_SET_PORT_RESUME_TIME                  MINIPORT_SetPortResumeTime;

    /* end version (3.10/4) definition */

    /* begin version (5) definition */

    PMPFN_PRESTART_CONTROLLER                   MINIPORT_PreStartController;

    /* end version (5) definition */

} USBPORT_REGISTRATION_PACKET, *PUSBPORT_REGISTRATION_PACKET;

/**************************************************************
**************************************************************
 USBPORT DLL Services
***************************************************************
***************************************************************/

#ifndef USBPORT

DECLSPEC_IMPORT
NTSTATUS
USBPORT_RegisterUSBPortDriver(
    PDRIVER_OBJECT DriverObject,
    ULONG MiniportHciVersion,
    PUSBPORT_REGISTRATION_PACKET RegistrationPacket,
    PUNICODE_STRING RegistryPath
    );

DECLSPEC_IMPORT
ULONG
USBPORT_GetHciMn(
    );

DECLSPEC_IMPORT
BOOLEAN
USBPORT_IsControllerInHibernatePath(
    __in PDEVICE_OBJECT FdoDeviceObject
    );

#endif

/* miniport must define these */

#undef PDEVICE_DATA
#undef PTRANSFER_CONTEXT
#undef PENDPOINT_DATA
#undef PDEVH_CONTEXT

#undef PHSB_TT_DATA
#undef PHSB_HC_DATA
#undef PHSB_EP_DATA

#endif /* __USB_HCDI_H__ */
