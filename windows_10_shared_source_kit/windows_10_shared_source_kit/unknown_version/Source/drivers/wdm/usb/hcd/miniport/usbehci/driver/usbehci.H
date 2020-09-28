/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

    usbehci.h

Abstract:



Environment:

    Kernel & user mode

Revision History:

    1-1-00 : created

--*/

#ifndef   __EHCI_H__
#define   __EHCI_H__

#include <initguid.h>





























// correspond to return value for EHCI_CheckHwSync
// These values only indicate that transition in or out of the hardware queue is in progress
typedef enum _EHCI_HW_SYNC {

    // qh ref'ed by hardware
    Qh_Busy= 0,
    // qh not ref'ed by hardware
    Qh_Not_Busy = 1

} EHCI_HW_SYNC, *PEHCI_HW_SYNC;

#define NO_EXP_DATE

#define MASK_CHANGE_BITS(p)\
    do {\
    (p).OvercurrentChange = 0;\
    (p).PortEnableChange = 0;\
    (p).PortConnectChange = 0;\
    } WHILE (0);

#define ABS(x) ( (0 < (x)) ? (x) : (0 - (x)))

// What normally gets returned by READ_PORT_ULONG when hardware is
// surprise removed.
//
#define EHCI_HARDWARE_GONE 0xffffffff

// Maximum time to wait for port reset to clear in microseconds
#define USBEHCI_MAX_PORT_RESET_CLEAR_TIME       500
#define USBEHCI_MAX_PORT_RESET_CLEAR_RETRIES    1000

/*
    define resource consumption for endpoints types
*/

#define T_256K          0x00040000
#define T_64K           0x00010000
#define T_4K            0x00001000


// Control: ******************************************************************************
// largest possible transfer for control is 64k
// therefore we support up to 2 transfers of this
// size in HW.  Most control transfers are much
// smaller than this.
// NOTE: we MUST support at least one 64k transfer in
// HW since a single control transfer cannot be
// broken up.

#define MAX_CONTROL_TRANSFER_SIZE   T_64K
// worst case 64k control transfer 4 + status and
// setup + dummy  =
#define TDS_PER_CONTROL_ENDPOINT        7

// maxtransfer is what usbport will split
//#define MAX_BULK_TRANSFER_SIZE          0x00040000
#define MAX_BULK_TRANSFER_SIZE          0x00080000

// enough for 8 256k transfers
// each TD can handle up to 20480 (0x5000) bytes (sec 3.5)
//
#define BULK_BYTES_PER_TD               0x5000
// 256k/20k (x00040000/x5000) = 12
// (TD memory required for a transfer is sizeof(qTd)*256) -- 256*256 = 65536 (64k)
//#define TDS_PER_BULK_TRANSFER           12


// 512k/20k (x00040000/x5000) = 24
// (TD memory required for a transfer is sizeof(qTd)*256) -- 256*256 = 65536 (64k)
#define TDS_PER_BULK_TRANSFER           24

// interrupt ep use same values
//
// This value could possibly be optimized down to 48 for two slots of
// 24 TDs per slot, if 24 TDs is sufficient to span a worst case 512KB
// transfer per slot.  See also the MAX_SLOT_TDS and MAX_SLOTS
// definitions.
//
#define TDS_PER_BULK_ENDPOINT           64
#define TDS_PER_INTERRUPT_ENDPOINT      64

// Isochronous:
#define MAX_ISO_TRANSFER_SIZE        T_256K
// 2 256 packet transfers *3k packet size, we can actually
// handle more
#define MAX_HSISO_TRANSFER_SIZE         0x00180000
#define TDS_PER_ISO_ENDPOINT            32


// default size of frame list
#define USBEHCI_MAX_FRAME            1024

// Time in units of 100 nanoseconds
//
#define MICROSECONDS_IN_100NS   10
#define MILLISECONDS_IN_100NS   (MICROSECONDS_IN_100NS * 1000)
#define SECONDS_IN_100NS        (MILLISECONDS_IN_100NS * 1000)
#define MINUTES_IN_100NS        (SECONDS_IN_100NS * 60)

// ACPI _DSM info

#define DSM_METHOD_AS_ULONG                     (ULONG)'MSD_'

#define USB_DSM_RESET_POST_PROC_REVISION_ID     0
#define USB_DSM_RESET_POST_PROC_FUNCTION_INDEX  1

// USB post reset processing _DSM method

// {CE2EE385-00E6-48cb-9F05-2EDB927C4899}
DEFINE_GUID(USB_DSM_RESET_POST_PROC_GUID, 0xce2ee385, 0x00e6, 0x48cb, 0x9f, 0x05, 0x2e, 0xdb, 0x92, 0x7c, 0x48, 0x99);

/*
    Registry Keys
*/

// Hardware Branch PDO Keys
// HKLM\System\CCS\Enum\ DeviceID \ InstanceID \Device Parameters

#define EN_EHCI_INTERNAL_FLAGS                 L"USBEHCI_InternalFlags"
#define EN_ASYNC_IDLE                          L"EnHcAsyncIdle"
#define EN_PERIODIC_IDLE                       L"EnHcPeriodicIdle"
#define DS_LS_HANDOFF                          L"DsLowSpeedHandoff"
#define EN_SOFT_RETRY                          L"EnHcSoftRetry"
#define SOFT_PER_SEC_RETRY_COUNT               L"HcSoftPerSecRetryCount"
#define MISSED_MICROFRAME_PER_SEC_RETRY_COUNT  L"MissedMicroframePerSecRetryCount"
#define EN_ASYNC_QH_IDLE_REMOVAL               L"EnAsyncQhIdleRemoval"
#define EN_64_BIT_ADDRESSING                   L"En64BitAddressing"
#define EN_ASYNC_QH_DUMMY                      L"EnAsyncQhDummy"
#define RESET_RECOVERY_ENABLE                  L"ResetRecoveryEnable"
#define RESET_RECOVERY_BREAK                   L"ResetRecoveryBreak"
#define INTEGRATED_TT_SUPPORT                  L"IntegratedTTType"



#define EHCI_SLOTS_PER_ENDPOINT                L"SlotsPerEndpoint"

#define EHCI_INTERNAL_ENABLE_RETRY_ON_START             0x00000001
#define EHCI_INTERNAL_DISABLE_OPTIMAL_CACHE_FLUSH       0x00000002
#define EHCI_INTERNAL_CAN_DO_CMD_CHANGES_WITH_DB        0x00000004
#define EHCI_INTERNAL_DISABLE_TWO_DOORBELL_WORKAROUND   0x00000008
#define EHCI_INTERNAL_ENABLE_STRICT_TRANSITION_CHECKS   0x00000010

// Debug Signatures
//
#define  SIG_HCD_IQH            SWIZZLE('20iq')
#define  SIG_HCD_AQH            SWIZZLE('20aq')
#define  SIG_HCD_AQH_DUMMY      SWIZZLE('20ad')
#define  SIG_HCD_QH             SWIZZLE('20qh')
#define  SIG_HCD_DQH            SWIZZLE('20dq')
#define  SIG_HCD_TD             SWIZZLE('20td')
#define  SIG_HCD_SITD           SWIZZLE('SItd')
#define  SIG_HCD_ITD            SWIZZLE('HItd')
#define  SIG_EP_DATA            SWIZZLE('20ep')
#define  SIG_EHCI_TRANSFER      SWIZZLE('20tr')
#define  SIG_EHCI_DD            SWIZZLE('ehci')
#define  SIG_DUMMY_QH           SWIZZLE('dmqh')
#define  SIG_ASYNC_IDLE_REF     SWIZZLE('aiRF')

#undef PDEVICE_DATA

typedef struct _TRANSFER_CONTEXT {

    LIST_ENTRY DD_TransferLink;

    ULONG Sig;
    ULONG PendingTds;
    PTRANSFER_PARAMETERS TransferParameters;
    USBD_STATUS UsbdStatus;
    ULONG BytesTransferred;
    // struct _HCD_TRANSFER_DESCRIPTOR *NextTransferTd;
    struct _ENDPOINT_DATA *EndpointData;

    //for ISO
    ULONG FrameComplete;
    LIST_ENTRY TransferLink;
    PMINIPORT_ISO_TRANSFER IsoTransfer;
    ULONG PendingPackets;
    ULONG PostedFrame;
    PVOID EpSlot;
} TRANSFER_CONTEXT, *PTRANSFER_CONTEXT;


// HCD Endpoint Descriptor (contains the HW descriptor)

// values for HCD_QUEUEHEAD_DESCRIPTOR.Flags
#define EHCI_QH_FLAG_IN_SCHEDULE            0x00000001
#define EHCI_QH_FLAG_QH_REMOVED             0x00000002
#define EHCI_QH_FLAG_STATIC                 0x00000004
#define EHCI_QH_FLAG_HIGHSPEED              0x00000008
#define EHCI_QH_FLAG_IN_HW_ASYNC_SCHEDULE   0x00000010

typedef struct _HCD_QUEUEHEAD_DESCRIPTOR {
   HW_QUEUEHEAD_DESCRIPTOR    HwQH;     // 0x44 bytes (17 dwords)

   HW_32BIT_PHYSICAL_ADDRESS  PhysicalAddress;
   ULONG                      Sig;
   ULONG                      QhFlags;
   ULONG                      Ordinal;
   ULONG                      Period;

   MP_HW_POINTER              EndpointData;
   MP_HW_POINTER              NextQh;
   MP_HW_POINTER              PrevQh;
   MP_HW_POINTER              NextLink;

#ifdef _WIN64
   ULONG                      PadToX32[1];
#else
   ULONG                      PadToX32[2];
#endif
} HCD_QUEUEHEAD_DESCRIPTOR, *PHCD_QUEUEHEAD_DESCRIPTOR;

// A HW_QUEUEHEAD_DESCRIPTOR must be aligned on a 32-byte boundary as
// the low 5 bits of a HW_LINK_POINTER are not used for addressing by
// the host controller hardware.
//
// So allowed sizes for a HCD_QUEUEHEAD_DESCRIPTOR must be multiples of
// 32 bytes, i.e. {96, 128, 160, 192, 224, 256, ...}
//
// If the size of a HCD_QUEUEHEAD_DESCRIPTOR is not a power of 2, i.e.
// {128, 256, ...} then EHCI_AddDummyQueueHeads() may need to be
// modified to deal with allocating QHs in a manner such as to
// guarantee that a QH does not span a page boundary.  This would also
// require modifications to EHCI_GetDummyQueueHeadForFrame() and the
// CommonBufferBlockBytes[0] size calculation in DriverEntry().
//
C_ASSERT((sizeof(HCD_QUEUEHEAD_DESCRIPTOR) == 128));

// HCD_QUEUEHEAD_DESCRIPTOR_PAD is the HCD_QUEUEHEAD_DESCRIPTOR size in
// bytes padded to a power of two that is the same size as the size of a
// HCD_TRANSFER_DESCRIPTOR.
//
// This ensures that every HCD_QUEUEHEAD_DESCRIPTOR and
// HCD_TRANSFER_DESCRIPTOR sub-allocation within the common buffer pool
// for an endpoint never spans a 4KB page boundary.
//
// A HCD_QUEUEHEAD_DESCRIPTOR is only padded when allocating a QH
// associated with an endpoint, not for the Dummy QHs.
//
#define HCD_QUEUEHEAD_DESCRIPTOR_PAD    128

C_ASSERT((sizeof(HCD_QUEUEHEAD_DESCRIPTOR) <= HCD_QUEUEHEAD_DESCRIPTOR_PAD));

//
// HCD Transfer Descriptor (contains the HW descriptor)
//

#define ENDPOINT_DATA_PTR(p) ((struct _ENDPOINT_DATA *) (p).Pointer)
#define TRANSFER_CONTEXT_PTR(p) ((struct _TRANSFER_CONTEXT *) (p).Pointer)
#define TRANSFER_DESCRIPTOR_PTR(p) ((struct _HCD_TRANSFER_DESCRIPTOR *) (p).Pointer)
#define QH_DESCRIPTOR_PTR(p) ((struct _HCD_QUEUEHEAD_DESCRIPTOR *) (p).Pointer)
#define HW_PTR(p) ((UCHAR * ) (p).Pointer)
#define ISO_PACKET_PTR(p) ((struct _MINIPORT_ISO_PACKET *) (p).Pointer)
#define ISO_TRANSFER_PTR(p) ((struct _TRANSFER_CONTEXT *) (p).Pointer)

// values for HCD_TRANSFER_DESCRIPTOR.Flags

#define TD_FLAG_BUSY                0x00000001
#define TD_FLAG_XFER                0x00000002
#define TD_FLAG_DONE                0x00000008
#define TD_FLAG_SKIP                0x00000010
#define TD_FLAG_DUMMY               0x00000020
#define TD_FLAG_SLOT_RESET          0x00000040
#define TD_FLAG_CONTROL_SKIP        0x00000080
#define TD_FLAG_ERROR_SKIP          0x00000100

#pragma warning(push)
#pragma warning(disable: 4201) // nameless struct/union

typedef struct _HCD_TRANSFER_DESCRIPTOR {
    HW_QUEUE_ELEMENT_TD         HwTD;       // 52 (0x34) bytes, 13 dwords
    ULONG                       Sig;
    ULONG                       Flags;
    ULONG                       TransferLength;
    HW_32BIT_PHYSICAL_ADDRESS   PhysicalAddress;

    MP_HW_POINTER               EndpointData;
    MP_HW_POINTER               TransferContext;
    MP_HW_POINTER               NextHcdTD;
    MP_HW_POINTER               AltNextHcdTD;

    union {
        // Control transfer: SETUP packet bytes
        //
        UCHAR                   Packet[8];

        // Bulk / Interrupt Slot Mode: Pointer to the next TD in the
        // current slot, or the first TD of the next slot for the last
        // TD of the current slot.  Control endpoints do not use Slot
        // Mode.
        //
        MP_HW_POINTER           SlotNextHcdTD;
    };

    LIST_ENTRY                  DoneLink;
#ifdef _WIN64
    //ULONG                     PadToX32[0];
#else
    ULONG                       PadToX32[3];
#endif
} HCD_TRANSFER_DESCRIPTOR, *PHCD_TRANSFER_DESCRIPTOR;

#pragma warning(pop)

// A HW_QUEUE_ELEMENT_TD must be aligned on a 32-byte boundary as the
// low 5 bits of a HW_LINK_POINTER are not used for addressing by the
// host controller hardware.
//
// So allowed sizes for a HCD_TRANSFER_DESCRIPTOR must be multiples of
// 32 bytes, i.e. {96, 128, 160, 192, 224, 256, ...}

C_ASSERT((sizeof(HCD_TRANSFER_DESCRIPTOR) == 128));

// Currently EHCI_OpenBulkOrControlEndpoint() and
// EHCI_OpenInterruptEndpoint() assume that the size of a
// HCD_TRANSFER_DESCRIPTOR equals the size of a padded
// HCD_QUEUEHEAD_DESCRIPTOR, and that this size is a power of two to
// ensure that every HCD_QUEUEHEAD_DESCRIPTOR and
// HCD_TRANSFER_DESCRIPTOR sub-allocation within the common buffer pool
// for an endpoint never spans a 4KB page boundary.
//
C_ASSERT((sizeof(HCD_TRANSFER_DESCRIPTOR) == HCD_QUEUEHEAD_DESCRIPTOR_PAD));


typedef struct _HCD_TD_LIST {
    HCD_TRANSFER_DESCRIPTOR Td[1];
} HCD_TD_LIST, *PHCD_TD_LIST;

/*
    Structures used for iso see iso.c
*/

typedef struct _HCD_SI_TRANSFER_DESCRIPTOR {
    HW_SPLIT_ISOCHRONOUS_TD    HwTD;    //64 (16dwords)

    ULONG                      Sig;
    HW_32BIT_PHYSICAL_ADDRESS  PhysicalAddress;
    ULONG                      StartOffset;
    ULONG                      Reserved;

    MP_HW_POINTER              Packet;
    MP_HW_POINTER              Transfer;
    MP_HW_POINTER              NextLink;
#ifdef _WIN64
    ULONG                      PadToX[6];
#else
    ULONG                      PadToX[6];
#endif
} HCD_SI_TRANSFER_DESCRIPTOR, *PHCD_SI_TRANSFER_DESCRIPTOR;

C_ASSERT((sizeof(HCD_SI_TRANSFER_DESCRIPTOR) == 128));


typedef struct _HCD_SITD_LIST {
    HCD_SI_TRANSFER_DESCRIPTOR Td[1];
} HCD_SITD_LIST, *PHCD_SITD_LIST;


typedef struct _HCD_HSISO_TRANSFER_DESCRIPTOR {
    HW_ISOCHRONOUS_TD          HwTD;    // 92 (0x5C) bytes, 23 dwords

    ULONG                      Sig;
    HW_32BIT_PHYSICAL_ADDRESS  PhysicalAddress;
    ULONG                      HostFrame;

    MP_HW_POINTER              FirstPacket;
    MP_HW_POINTER              NextLink;
    MP_HW_POINTER              Transfer;

} HCD_HSISO_TRANSFER_DESCRIPTOR, *PHCD_HSISO_TRANSFER_DESCRIPTOR;

// A HW_ISOCHRONOUS_TD must be aligned on a 32-byte boundary as the low
// 5 bits of a HW_LINK_POINTER are not used for addressing by the host
// controller hardware.
//
// So allowed sizes for a HCD_HSISO_TRANSFER_DESCRIPTOR must be
// multiples of 32 bytes, i.e. {96, 128, 160, 192, 224, 256, ...}

C_ASSERT((sizeof(HCD_HSISO_TRANSFER_DESCRIPTOR) == 128));

typedef struct _HCD_HSISOTD_LIST {
    HCD_HSISO_TRANSFER_DESCRIPTOR Td[1];
} HCD_HSISOTD_LIST, *PHCD_HSISOTD_LIST;

/*
    Used for data structure that describes the interrupt
    schedule (see periodic.c)
*/
typedef struct _PERIOD_TABLE {
    UCHAR Period;
    UCHAR qhIdx;
    UCHAR InterruptScheduleMask;
} PERIOD_TABLE, *PPERIOD_TABLE;


typedef enum _SLOT_STATE {

    slot_NotBusy,
    slot_Busy,
    slot_Abort

} SLOT_STATE;

typedef enum _EHCI_TRANSFER_TYPE {

    isoch_periodic = 1,
    int_periodic,

    control_async,
    bulk_async

} EHCI_TRANSFER_TYPE;


/*
    Slot mode settings assume MAX_TRANSFER of 256k

    256k = 16 TDs (~16k per TD)
    512k = 32 TDs (~16k per TD)

*/

//#define MAX_SLOT_TDS    16
#define MAX_SLOT_TDS      32

#define MAX_SLOTS         2

C_ASSERT(MAX_SLOT_TDS * MAX_SLOTS <= TDS_PER_BULK_ENDPOINT);
C_ASSERT(MAX_SLOT_TDS * MAX_SLOTS <= TDS_PER_INTERRUPT_ENDPOINT);

typedef struct _ENDPOINT_SLOT {
    ULONG Idx;
    LONG Sequence;
    SLOT_STATE State;

    PTRANSFER_CONTEXT   TransferContext;

    PHCD_TRANSFER_DESCRIPTOR FirstTd;
    PHCD_TRANSFER_DESCRIPTOR Tds[MAX_SLOT_TDS];
} ENDPOINT_SLOT, *PENDPOINT_SLOT;


#define EHCI_EDFLAG_NOHALT          0x00000001
#define EHCI_EDFLAG_HC_RESET        0x00000002

typedef struct _ENDPOINT_DATA {

    LIST_ENTRY AcfLink;
    LIST_ENTRY AllEpLink;

    ULONG Sig;

    ENDPOINT_PARAMETERS Parameters;
    MP_ENDPOINT_STATUS EpStatus;
    PHCD_QUEUEHEAD_DESCRIPTOR QueueHead;
    ULONG Flags;
    EHCI_HW_SYNC HwSyncState;
    LARGE_INTEGER StateChangeTc;
    ULONG DoorbellCount;
    ULONG SmodeTransfers;
    LIST_ENTRY SmodeTransferListHead;

    PHCD_QUEUEHEAD_DESCRIPTOR StaticQH;
    PPERIOD_TABLE PeriodTableEntry;

    PHCD_TD_LIST TdList;
    PHCD_SITD_LIST SiTdList;
    PHCD_HSISOTD_LIST HsIsoTdList;

    PHCD_QUEUEHEAD_DESCRIPTOR NextFrameStaticQH;

    ULONG TdCount;
    ULONG FreeTds;
    ULONG LastFrame;
    ULONG QhChkPhys;
    PVOID QhChk;

    EHCI_TRANSFER_TYPE TransferTypeFlag;
    ULONG IsochTransfers;
    LIST_ENTRY IsoTransferListHead;
    LIST_ENTRY DoneTdList;
    MP_ENDPOINT_STATE CurEpState;
    struct _ENDPOINT_DATA *PrevEndpoint;
    struct _ENDPOINT_DATA *NextEndpoint;

    // for control transfer only
    PHCD_TRANSFER_DESCRIPTOR SetupTD;
    PHCD_TRANSFER_DESCRIPTOR StatusTD;

    //
    // For Retrying Txs
    //
    USHORT          PendingSoftRetryCount;
    USHORT          PendingMissedMicroframeRetryCount;
    ULONG           PendingSoftRetryCountInitTimeMs;
    ULONG           PendingMissedMicroframeRetryCountInitTimeMs;

    ULONG           EndpointInScheduleCount;

    ULONG           sMode;
    ULONG           SlotTdErrorCount;
    ULONG           TdsPerSlot;
    LONG            SlotSequence;
    ULONG           SlotCount;

    PENDPOINT_SLOT  CurrentSlot;
    PENDPOINT_SLOT  SlotPtr[MAX_SLOTS];
    ENDPOINT_SLOT   Slots[MAX_SLOTS];

} ENDPOINT_DATA, *PENDPOINT_DATA;


/*****************************************************************************************

KeyValueName:
EnHcAsyncIdle, EnHcPeriodicIdle

OS Version:
Windows Vista

Location:
Hardware Branch,
HKLM\System\CCS\Enum\ DeviceID \ InstanceID \Device Parameters

Type:
DWORD

Default:
The default is Enabled.

Description:

Non-zero value Enables, zero diables. This key controls the the toggling
of the cmd.PeriodicScheduleEnable or cmd.AsyncScheduleEnable field based
on the presence or absence of periodic transfers pending on the bus.

This key replaces EnIdleEndpointSupport in Vista.

------------------------------------------------------------------------------------------


KeyValueName:
DsLowSpeedHandoff

OS Version:
Windows Vista

Location:
Hardware Branch,
HKLM\System\CCS\Enum\ DeviceID \ InstanceID \Device Parameters

Type:
DWORD

Default:
The default is Disabled.

Description:
Disable Low Speed handoff. When enabled all USB devices connected to the
high speed ports remain on the high speed bus -- devices are not handed
off to the Companion USB (1.x) controllers. This key is provided for
testing and diagnostic purposes.

------------------------------------------------------------------------------------------

KeyValueName:
EnHcSoftRetry

OS Version:
Windows Vista

Location:
Hardware Branch,
HKLM\System\CCS\Enum\ DeviceID \ InstanceID \Device Parameters

Type:
DWORD

Default:
The default is Disabled.

Description:
Non-zero value Enables, zero diables. This key controls whether or not
the we try to soft-retry an asynchronous transfer if we get an XactError

------------------------------------------------------------------------------------------





























*****************************************************************************************/


typedef enum _EHCI_IDLE_REF_TYPE {

    SstoolAPI = 1,
    RH_PortResumeComplete,
    RH_FinishReset,
    RH_PortResetComplete

} EHCI_IDLE_REF_TYPE;

typedef struct _EHCI_IDLE_REF_CONTEXT {

    LIST_ENTRY IdleRefLink;
    ULONG Sig;
    EHCI_IDLE_REF_TYPE RefType;
    PVOID RefObject;

} EHCI_IDLE_REF_CONTEXT, *PEHCI_IDLE_REF_CONTEXT;

typedef struct _EHCI_PORT_EVENT_CONTEXT {
    USHORT PortNumber;
    PEHCI_IDLE_REF_CONTEXT AsyncIdleRef;
    ULONG ResumeTime;
    BOOLEAN Retry;
} EHCI_PORT_EVENT_CONTEXT, *PEHCI_PORT_EVENT_CONTEXT;

typedef enum _ASYNC_SCHEDULE_STATE {

    AsyncScheduleState_Disabled = 1,
    AsyncScheduleState_EnablePendingDoorbell = 2,
    AsyncScheduleState_EnableRequested = 3,
    AsyncScheduleState_Enabled = 4,
    AsyncScheduleState_DisablePendingDoorbell = 5,
    AsyncScheduleState_DisableRequested = 6

} ASYNC_SCHEDULE_STATE;

typedef enum _PERIODIC_SCHEDULE_STATE {

    PeriodicScheduleState_Disabled = 1,
    PeriodicScheduleState_EnablePendingDoorbell = 2,
    PeriodicScheduleState_EnableRequested = 3,
    PeriodicScheduleState_Enabled = 4,
    PeriodicScheduleState_DisablePendingDoorbell = 5,
    PeriodicScheduleState_DisableRequested = 6

} PERIODIC_SCHEDULE_STATE;

typedef enum _ASYNC_DOORBELL_STATE {

    AsyncDoorbellState_NotWaiting = 1,
    AsyncDoorbellState_PendingScheduleEnable = 2,
    AsyncDoorbellState_PendingScheduleDisable = 3,
    AsyncDoorbellState_Requested = 4,
    AsyncDoorbellState_RequestedWithRetry = 5

} ASYNC_DOORBELL_STATE;


typedef enum _INTEGRATED_TT_FLAVOR {
    IntegratedTTNotSupported = 0,
    IntegratedTTSpeedInPortSc = 1,
    IntegratedTTSpeedInHostPc = 2
} INTEGRATED_TT_FLAVOR;

#define RETRY_COUNT_RESET_TIME_MS 1000
#define MAX_PER_SEC_RETRY_COUNT 100
#define MAX_PER_SEC_RETRY_COUNT_OAK_TRAIL 1000
#define DEFAULT_PER_SEC_RETRY_COUNT 10

typedef struct _DEVICE_DATA {

    ULONG                       Sig;

    PDEVICE_OBJECT              HostControllerFdo;
    PDEVICE_OBJECT              TargetDeviceObject;

    // list of all 'opened' endpoints
    LIST_ENTRY                  AllEpListHead;
    ULONG                       HcSuspendedFlag;
    ULONG                       EhciVersion;

    /* enable disble feature vars - easier to read in debugger */

    // Enables NEC B1 dummy queue head fix
    ULONG                       UseDummyQheadsEnabled;
    ULONG                       AsyncTransferCount;
    ULONG                       AsyncIdleDetectEnabled;
    ULONG                       AsyncIdleDetectError;
    ULONG                       PeriodicTransferCount;
    ULONG                       PeriodicIdleDetectEnabled;
    ULONG                       PeriodicIdleDetectEnabledOverride;
    ULONG                       LowSpeedHandOffEnabled;
    ULONG                       SoftPerSecRetryCount;
    ULONG                       MissedMicroframePerSecRetryCount;
    ULONG                       Bit64AddressingCapability;
    BOOLEAN                     Bit64AddressingCapabilityOverride;
    BOOLEAN                     CompanionControllerHandoffEnabled;









    BOOLEAN                     TerminateQH;

    // If AsyncQhIdleRemoval is TRUE then QHs are dynamically removed
    // from the Async list when they have no active transfers.
    //
    BOOLEAN                     AsyncQhIdleRemoval;
    BOOLEAN                     AsyncQhIdleRemovalOverride;

    BOOLEAN                     AsyncQueueHeadDummyEnable;
    BOOLEAN                     AsyncQueueHeadDummyEnableOverride;






    ULONG                       SlotsPerEndpoint;

    BOOLEAN                     PciConfigSpaceDisabled;

    BOOLEAN                     EvaluateAcpiMethodOnResetSucceeded;

    ULONG                       ImmediateAsyncEnableChange;
    ULONG                       ImmediatePeriodicEnableChange;

    ULONG                       SstoolApiActive;

    // global lists of transfers by schedule type
    // smode is bulk, control and interrupt

    // contol and bulk (AKA async)
    LIST_ENTRY                  PendingSmodeCTB_ListHead;
    // interrupt (periodic)
    LIST_ENTRY                  PendingSmodeINT_ListHead;
    // isoch (periodic)
    LIST_ENTRY                  PendingIsochListHead;

    // refs prevent idle
    LIST_ENTRY                  AsyncIdleRefListHead;
    ULONG                       AsyncIdleRefNoRes;

    PHC_OPERATIONAL_REGISTER    OperationalRegisters;
    PHC_CAPABILITIES_REGISTER   CapabilitiesRegisters;
    PHC_DEBUGPORT_REGISTERS     DebugPortRegisters;

    PKINTERRUPT                 Interrupt;
    USBINTR                     EnabledInterrupts;

    PHCD_QUEUEHEAD_DESCRIPTOR   AsyncQueueHead;
    PHCD_QUEUEHEAD_DESCRIPTOR   AsyncQueueHeadDummy;
    LIST_ENTRY                  AcfListHead;
    KSPIN_LOCK                  AcfListSpin;
    PKTHREAD                    AcfLockThread;
    PENDPOINT_DATA              EpAtTheDoor;

    USB_CONTROLLER_FLAVOR       ControllerFlavor;

    ULONG                       LastFrame;
    ULONG                       FrameNumberHighPart;

    ULONG                       PortResetChange;
    ULONG                       PortSuspendChange;
    ULONG                       PortConnectChange;
    ULONG                       PortPMChirp;
    ULONG                       PortInResume;
    ULONG                       PortSuspended;

    ULONG                       IrqStatus;

    USHORT                      NumberOfPorts;
    USHORT                      PortPowerControl;

    PHCD_QUEUEHEAD_DESCRIPTOR   LockPrevQh;
    PHCD_QUEUEHEAD_DESCRIPTOR   LockNextQh;
    PHCD_QUEUEHEAD_DESCRIPTOR   LockQh;

    PVOID                       CommonBufferVa;
    HW_32BIT_PHYSICAL_ADDRESS   CommonBufferPhys;
    ULONG                       CommonBufferBytes;

    PVOID                       CommonBufferBlocksVa;
    HW_32BIT_PHYSICAL_ADDRESS   CommonBufferBlocksPhys;
    ULONG                       CommonBufferBlockBytes;

    PHCD_QUEUEHEAD_DESCRIPTOR   StaticInterruptQH[65];

    PHW_32BIT_PHYSICAL_ADDRESS  FrameListBaseAddress;
    HW_32BIT_PHYSICAL_ADDRESS   FrameListBasePhys;

    PENDPOINT_DATA              IsoEndpointListHead;
    PHCD_QUEUEHEAD_DESCRIPTOR   DummyQueueHeads;
    HW_32BIT_PHYSICAL_ADDRESS   DummyQueueHeadsPhys;

    // volitile registers saved when controller is put in 'suspend'
    ULONG                       PeriodicListBaseSave;
    ULONG                       AsyncListAddrSave;
    ULONG                       SegmentSelectorSave;
    USBCMD                      CmdSave;
    USBINTR                     IntrSave;

    USHORT                      Vid;
    USHORT                      Dev;

    CONFIGFLAG                  CurrentConfigFlag;
    // value of config flag last time we went into suspend
    CONFIGFLAG                  SuspendConfigFlag;

    ULONG                       SuspendHcNotHalted;

    //
    // This is a count of HostSystemError interrupts we service.  These
    // are typically caused by either a bad TD in the schedule or a PCI
    // error between the HC when accessing memory to execute the
    // schedule.
    //
    ULONG                       HostSystemErrorCount;
    ULONG                       HostSystemErrorResetNeeded;

    USHORT                      DebugPortNumber;
    USHORT                      DebugPortNumberUI;

    // we only need this for older revs of usbport
    // that will call checkController after start fails
    BOOLEAN                     DeviceStarted;
    UCHAR                       SavedFladj;

    PVOID                       PortResetCb[EHCI_MAX_PORT];
    PEHCI_IDLE_REF_CONTEXT      PortResetCbRef[EHCI_MAX_PORT];

    // Stats used for debugging purposes.
    //
    ULONG                       StatAsyncEnableCount;
    ULONG                       StatAsyncDisableCount;
    ULONG                       StatPeriodicEnableCount;
    ULONG                       StatPeriodicDisableCount;
    ULONG                       StatAsyncEnableTimeoutCount;
    ULONG                       StatAsyncDisableTimeoutCount;
    ULONG                       StatPeriodicEnableTimeoutCount;
    ULONG                       StatPeriodicDisableTimeoutCount;
    ULONG                       StatRingDoorbellDisabledCount;
    ULONG                       StatRingDoorbellDisablingCount;
    ULONG                       StatRingDoorbellEnablingCount;
    ULONG                       StatRingDoorbellEnabledCount;
    ULONG                       StatRingDoorbellHwRemovedCount;
    ULONG                       StatAnswerDoorbell;
    ULONG                       StatAsyncCacheFlushIdle;
    ULONG                       StatAsyncCacheFlushNormal;
    ULONG                       StatAsyncCacheFlushRetry;
    ULONG                       StatIntOnAsyncAdvancePendingCount;
    ULONG                       StatIntOnAsyncAdvanceSyncRoutineCount;
    ULONG                       StatRingDoorbellEnabledWithRetryCount;
    ULONG                       StatRingDoorbellEnabledWithRetryIgnoredCount;
    ULONG                       StatAsyncCacheFlushPrevented;

    ULONG                       CurrentRingDoorbellDisablingTimerCount;
    ULONG                       CurrentRingDoorbellEnablingTimerCount;
    ULONG                       CurrentRingDoorbellEnabledTimerCount;
    ULONG                       CurrentIntOnAsyncAdvancePendingCount;

    ULONG                       StatResetNeededCount;
    ULONG                       ResetNeeded;
    ULONG                       ResetRecoveryEnable;
    ULONG                       ResetRecoveryBreak;

    ASYNC_SCHEDULE_STATE        AsyncScheduleState;
    ASYNC_DOORBELL_STATE        AsyncDoorbellState;
    PERIODIC_SCHEDULE_STATE     PeriodicScheduleState;

    ULONG                       EhciInternalFlags;
    BOOLEAN                     PreventCmdChangesWithDoorbellPending;
    BOOLEAN                     OptimizeInactiveQhFlush;
    BOOLEAN                     DisableTwoDoorbellWorkaround;
    BOOLEAN                     StrictTransitionChecksEnabled;
    BOOLEAN                     IntegratedTTSupportRegistryOverride;

    ULONG                       CurrentAsyncEnableCount;
    ULONG                       InitialDoorbellRetryEnableValue;
    ULONG                       CurrentDoorbellRetryEnableValue;
    ULONG                       DoorbellRetryRequired;
    ULONG                       PeriodicEnableChangeWithDoorbellPending;
    ULONG                       PeriodicDisableChangeWithDoorbellPending;
    PORTSC                      SuspendPortStatus[EHCI_MAX_PORT];
    PORTSC                      ResumePortStatus[EHCI_MAX_PORT];

    LONG                        ResumeTimeRefCount[EHCI_MAX_PORT];
    UCHAR                       ResumeSignalTime[EHCI_MAX_PORT];

    INTEGRATED_TT_FLAVOR        IntegratedTTSupport;

#ifdef _ARM_
    BOOLEAN                     Need_OMAP_ULPI_ForceHS_OnResume;
#endif
    BOOLEAN                     StopHcForPortOperations;

    BOOLEAN                     InvalidateRootHub;

} DEVICE_DATA, *PDEVICE_DATA;

//
// Stats pulled from the EHCI miniport DEVICE_DATA and written as the
// DumpData for an error log entry.
//
typedef struct _EHCI_ERROR_LOG_STATS {

    USHORT                      VendorId;
    USHORT                      DeviceId;
    ULONG                       HostSystemErrorCount;
    ULONG                       StatAsyncEnableCount;
    ULONG                       StatAsyncDisableCount;
    ULONG                       StatPeriodicEnableCount;
    ULONG                       StatPeriodicDisableCount;
    ULONG                       StatAsyncEnableTimeoutCount;
    ULONG                       StatAsyncDisableTimeoutCount;
    ULONG                       StatPeriodicEnableTimeoutCount;
    ULONG                       StatPeriodicDisableTimeoutCount;
    ULONG                       StatRingDoorbellDisabledCount;
    ULONG                       StatRingDoorbellDisablingCount;
    ULONG                       StatRingDoorbellEnablingCount;
    ULONG                       StatRingDoorbellEnabledCount;
    ULONG                       StatAnswerDoorbell;
    ULONG                       StatAsyncCacheFlushIdle;
    ULONG                       StatAsyncCacheFlushNormal;
    ULONG                       StatAsyncCacheFlushRetry;
    ULONG                       StatIntOnAsyncAdvancePendingCount;
    ULONG                       StatIntOnAsyncAdvanceSyncRoutineCount;
    ULONG                       StatResetNeededCount;
    ULONG                       InitialDoorbellRetryEnableValue;
    ULONG                       CurrentDoorbellRetryEnableValue;
    ULONG                       DoorbellRetryRequired;
    ULONG                       PeriodicEnableChangeWithDoorbellPending;
    ULONG                       PeriodicDisableChangeWithDoorbellPending;

} EHCI_ERROR_LOG_STATS, *PEHCI_ERROR_LOG_STATS;

//
// The size, in bytes, of the variable-length DumpData member of the
// IO_ERROR_LOG_PACKET structure must be a multiple of sizeof(ULONG).
//
C_ASSERT((sizeof(EHCI_ERROR_LOG_STATS) % sizeof(ULONG)) == 0);

//
// The size, in bytes, of the error log entry cannot exceed
// ERROR_LOG_MAXIMUM_SIZE.
//
C_ASSERT((sizeof(IO_ERROR_LOG_PACKET) - sizeof(ULONG) + sizeof(EHCI_ERROR_LOG_STATS))
         < ERROR_LOG_MAXIMUM_SIZE);


/*
    Callouts to port driver services
*/
extern USBPORT_REGISTRATION_PACKET RegistrationPacket;

#define USBPORT_DBGPRINT(dd, l, f, arg0, arg1, arg2, arg3, arg4, arg5) \
        RegistrationPacket.USBPORTSVC_DbgPrint((dd), (l), (f), (arg0), (arg1), \
            (arg2), (arg3), (arg4), (arg5))

#define USBPORT_GET_REGISTRY_KEY_VALUE(dd, branch, keystring, keylen, data, datalen) \
        RegistrationPacket.USBPORTSVC_GetMiniportRegistryKeyValue((dd), (branch), \
            (keystring), (keylen), (data), (datalen))

#define USBPORT_INVALIDATE_ROOTHUB(dd) \
        RegistrationPacket.USBPORTSVC_InvalidateRootHub((dd));

#define USBPORT_COMPLETE_TRANSFER(dd, ep, tp, status, length) \
        RegistrationPacket.USBPORTSVC_CompleteTransfer((dd), (ep), (tp), \
            (status), (length));

#define USBPORT_INVALIDATE_ENDPOINT(dd, ep) \
        RegistrationPacket.USBPORTSVC_InvalidateEndpoint((dd), (ep));

#define USBPORT_PHYSICAL_TO_VIRTUAL(addr, dd, ep) \
        RegistrationPacket.USBPORTSVC_MapHwPhysicalToVirtual((addr), (dd), (ep));

//#define USBPORT_REQUEST_ASYNC_CALLBACK(dd, t, c, cl, f) \
//        RegistrationPacket.USBPORTSVC_RequestAsyncCallback((dd), (t), \
//            (c), (cl), (f));

#define USBPORT_REQUEST_ASYNC_CALLBACK_EX(dd, t, c, cl, f, cbh, rhf) \
        RegistrationPacket.USBPORTSVC_RequestAsyncCallbackEx((dd), (t), \
            (c), (cl), (f), (cbh), (rhf));

#define USBPORT_CANCEL_ASYNC_CALLBACK(dd, cbh, rhf)  \
         RegistrationPacket.USBPORTSVC_CancelAsyncCallback((dd), (cbh), (rhf));

#define USBPORT_WAIT(dd, t) \
        RegistrationPacket.USBPORTSVC_Wait((dd), (t));

#define USBPORT_BUGCHECK(dd) \
        RegistrationPacket.USBPORTSVC_BugCheck(dd)

#define USBPORT_COMPLETE_ISO_TRANSFER(dd, ep, t, iso) \
        RegistrationPacket.USBPORTSVC_CompleteIsoTransfer((dd), (ep), (t), \
            (iso));

#define USBPORT_INVALIDATE_CONTROLLER(dd, s) \
        RegistrationPacket.USBPORTSVC_InvalidateController((dd), (s))

#define USBPORT_READ_CONFIG_SPACE(dd, b, o, l) \
        RegistrationPacket.USBPORTSVC_ReadWriteConfigSpace((dd), TRUE, \
            (b), (o), (l))

#define USBPORT_WRITE_CONFIG_SPACE(dd, b, o, l) \
        RegistrationPacket.USBPORTSVC_ReadWriteConfigSpace((dd), FALSE, \
            (b), (o), (l))

#define USBPORT_ETW_WRITE(dd, ed, ai, udc, ud) \
        RegistrationPacket.USBPORTSVC_EtwWrite((dd), (ed), (ai), (udc), (ud))

#define USBPORT_IS_INTERRUPT_PER_TD_ENABLED(dd) \
        RegistrationPacket.USBPORTSVC_IsInterruptPerTdEnabled((dd))

#ifdef _WIN64
#define DUMMY_TD_CONTEXT ((PVOID) 0xABADBABEABADBABE)
#else
#define DUMMY_TD_CONTEXT ((PVOID) 0xABADBABE)
#endif

// note: we must initialize the low 12 bits of the
// buffer page ptr to zero to the last three nipples
// are 0

#define INITIALIZE_TD_FOR_TRANSFER(td, tc) \
        { ULONG i;\
        HC_QTD_TOKEN token;\
        TRANSFER_CONTEXT_PTR((td)->TransferContext) = (tc);\
        (td)->Flags |= TD_FLAG_XFER; \
        for (i=0; i<5; i++) {\
            (td)->HwTD.BufferPage[i].ul = 0x0bad0000;\
            (td)->HwTD.BufferPage64[i]  = 0x00000000;\
        }\
        (td)->HwTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT;\
        (td)->HwTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;\
        token.ul = 0;\
        token.ErrorCounter = 3;\
        (td)->HwTD.Token = token;\
        TRANSFER_DESCRIPTOR_PTR((td)->NextHcdTD) = NULL;\
        }

#define SET_NEXT_TD(dd, linkTd, nextTd) \
        EHCI_SetNextTd((dd), (linkTd), (nextTd), FALSE)

#define SET_ALTNEXT_TD(dd, linkTd, nextTd) \
        EHCI_SetAltNextTd((dd), (linkTd), (nextTd))

#define SET_NEXT_AND_ALTNEXT_TD(dd, linkTd, nextTd) \
        EHCI_SetNextTd((dd), (linkTd), (nextTd), TRUE)

#define SET_NEXT_TD_NULL(linkTd) \
    TRANSFER_DESCRIPTOR_PTR((linkTd)->NextHcdTD) = NULL;\
    TRANSFER_DESCRIPTOR_PTR((linkTd)->AltNextHcdTD) = NULL;\
    (linkTd)->HwTD.Next_qTD.HwAddress = EHCI_TERMINATE_BIT; \
    (linkTd)->HwTD.AltNext_qTD.HwAddress = EHCI_TERMINATE_BIT;


#ifdef _WIN64
#define FREE_TD_CONTEXT ((PVOID) 0xDEADFACEDEADFACE)
#else
#define FREE_TD_CONTEXT ((PVOID) 0xDEADFACE)
#endif

#define EHCI_FREE_TD(dd, ep, td) \
    (td)->Flags = 0;\
    (td)->HwTD.Next_qTD.HwAddress = 0;\
    (td)->HwTD.AltNext_qTD.HwAddress = 0;\
    (ep)->FreeTds++;\
    LOGENTRY((dd), G, '_fTD', (td), 0, 0);\
    TRANSFER_CONTEXT_PTR((td)->TransferContext) = FREE_TD_CONTEXT;

#define EHCI_ALLOC_TD(dd, ep) EHCI_AllocTd((dd), (ep));


#define TEST_BIT(value, bitNumber) ((value) & (1<<(bitNumber))) ? TRUE : FALSE

#define SET_BIT(value, bitNumber) ((value) |= (1<<(bitNumber)))

#define CLEAR_BIT(value, bitNumber)  ((value) &= ~(1<<(bitNumber)))


// assuming only one bit is set this macro returns that bit
//
#define GET_BIT_SET(d, bit) \
    {   \
        UCHAR tmp = (d);\
        (bit)=0; \
        while (!(tmp & 0x01) && ((bit)<7)) {\
            (bit)++;\
            tmp >>= 1;\
        };\
    }

FORCEINLINE
ULONG
EHCI_TdBytesTransferred(
    __in ULONG OriginalBytesToTransfer,
    __in ULONG CurrentBytesToTransfer
    )
/*++

Routine Description:

    This routine calculates the number of bytes transferred by a TD,
    which is the number of bytes originally requested to be transferred
    minus the number of bytes still remaining to be transferred.

    The number of bytes remaining to be transfered should always be less
    than or equal to then number originally requested to be transferred.
    If for some unknown reason the host controller updates the remaining
    bytes to be transferred with a value larger than the original value
    this routine will return zero instead of a large unsigned value.

--*/
{
    if (OriginalBytesToTransfer >= CurrentBytesToTransfer) {
        return (OriginalBytesToTransfer - CurrentBytesToTransfer);
    } else {
        return 0;
    }
}


FORCEINLINE
VOID
EHCI_StopHcAndWaitForHalt(
    __in PHC_OPERATIONAL_REGISTER HcOp
    )
/*++

Routine Description:

    This routine clears the Run/Stop bit and waits for the HcHalted by to be set

--*/
{
    USBSTS status;
    USBCMD cmd;
    ULONG i;
        
    cmd.ul = READ_REGISTER_ULONG(&HcOp->UsbCommand.ul);
    cmd.HostControllerRun = 0;
    WRITE_REGISTER_ULONG(&HcOp->UsbCommand.ul, cmd.ul);
        
    // wait for the HC to halt
    for (i=0; i<16; i++) {
        status.ul = READ_REGISTER_ULONG(&HcOp->UsbStatus.ul);
        if (status.HcHalted) {
            break;
        }
        KeStallExecutionProcessor(125);
    }
}


//****************************************************************************************
//
// ASYNC.C Function Prototypes
//
//****************************************************************************************

VOID
EHCI_EnableAsyncList(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_DisableAsyncList(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_LinkControlTransferToQueue(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

PHCD_QUEUEHEAD_DESCRIPTOR
EHCI_InitializeQH(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __out_bcount(HCD_QUEUEHEAD_DESCRIPTOR_PAD)PHCD_QUEUEHEAD_DESCRIPTOR Qh,
    HW_32BIT_PHYSICAL_ADDRESS HwPhysAddress
    );

PHCD_TRANSFER_DESCRIPTOR
EHCI_InitializeTD(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __out PHCD_TRANSFER_DESCRIPTOR Td,
    HW_32BIT_PHYSICAL_ADDRESS HwPhysAddress
    );

USB_MINIPORT_STATUS
EHCI_ControlTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_PARAMETERS TransferUrb,
    __inout PTRANSFER_CONTEXT TransferContext,
    __inout PTRANSFER_SG_LIST TransferSGList
    );

USB_MINIPORT_STATUS
EHCI_BulkTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_PARAMETERS TransferUrb,
    __inout PTRANSFER_CONTEXT TransferContext,
    __inout PTRANSFER_SG_LIST TransferSGList
    );

USB_MINIPORT_STATUS
EHCI_OpenBulkOrControlEndpoint(
    __inout PDEVICE_DATA DeviceData,
    BOOLEAN Control,
    __inout PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_InsertQueueHeadInHWAsyncList(
     __inout PDEVICE_DATA               DeviceData,
     __inout PHCD_QUEUEHEAD_DESCRIPTOR  AsyncQhToInsert
    );

VOID
EHCI_RemoveQueueHeadFromHWAsyncList(
     __inout PDEVICE_DATA               DeviceData,
     __inout PHCD_QUEUEHEAD_DESCRIPTOR  AsyncQhToRemove
    );

VOID
EHCI_InsertQueueHeadInAsyncList(
    __inout PDEVICE_DATA DeviceData,
    __inout PHCD_QUEUEHEAD_DESCRIPTOR Qh
    );

VOID
EHCI_RemoveQueueHeadFromAsyncList(
    __inout PDEVICE_DATA DeviceData,
    __inout PHCD_QUEUEHEAD_DESCRIPTOR Qh
    );

ULONG
EHCI_MapAsyncTransferToTd(
    __inout PDEVICE_DATA DeviceData,
    ULONG MaxPacketSize,
    ULONG LengthMapped,
    __inout_opt PULONG NextToggle,
    __inout PTRANSFER_CONTEXT TransferContext,
    __inout PHCD_TRANSFER_DESCRIPTOR Td,
    __inout PTRANSFER_SG_LIST SgList
    );

VOID
EHCI_SetAsyncEndpointState(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    MP_ENDPOINT_STATE State
    );

MP_ENDPOINT_STATUS
EHCI_GetAsyncEndpointStatus(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_SetAsyncEndpointStatus(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    MP_ENDPOINT_STATUS Status
    );

VOID
EHCI_ProcessDoneAsyncTd(
    __inout PDEVICE_DATA DeviceData,
    __inout PHCD_TRANSFER_DESCRIPTOR Td,
    __inout_opt PTRANSFER_CONTEXT AbortTransferContext
    );

USBD_STATUS
EHCI_GetErrorFromTD(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PHCD_TRANSFER_DESCRIPTOR Td
    );

VOID
EHCI_AbortControlTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_CONTEXT TransferContext
    );

USB_MINIPORT_STATUS
EHCI_PokeAsyncEndpoint(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_PARAMETERS EndpointParameters,
     __inout PENDPOINT_DATA EndpointData
    );

_Requires_lock_not_held_(DeviceData->AcfListSpin) _Acquires_lock_(DeviceData->AcfListSpin)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_raises_(DISPATCH_LEVEL)
VOID
EHCI_AcquireAsyncDoorbellLock(
     _Inout_ PDEVICE_DATA DeviceData,
     _Out_ __deref _IRQL_saves_ PKIRQL Irql
     );

_Requires_lock_held_(DeviceData->AcfListSpin) _Releases_lock_(DeviceData->AcfListSpin)
_IRQL_requires_(DISPATCH_LEVEL)
VOID
EHCI_ReleaseAsyncDoorbellLock(
     _Inout_ PDEVICE_DATA DeviceData,
     _In_ _IRQL_restores_ KIRQL Irql
     );

VOID
EHCI_AnswerAsyncDoorbell(
     __inout PDEVICE_DATA DeviceData
     );

BOOLEAN
EHCI_IsAsyncDoorbellPending(
     __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_ClearCurrentAsyncStats(
     __inout PDEVICE_DATA DeviceData
     );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_SetAsyncDoorbellState(
     __inout PDEVICE_DATA DeviceData,
     __in ASYNC_DOORBELL_STATE AsyncDoorbellNextState
     );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_UpdateScheduleStates(
     __inout PDEVICE_DATA DeviceData,
     __in USBCMD cmd,
     __in USBSTS sts
     );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_UpdateAsyncScheduleState(
     __inout PDEVICE_DATA DeviceData,
     __in USBCMD cmd,
     __in USBSTS sts
     );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_SetAsyncScheduleState(
     __inout PDEVICE_DATA DeviceData,
     __in ASYNC_SCHEDULE_STATE NextState
     );

VOID
EHCI_UpdateEndpointAsFreedFromHwCache(
    __in PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA epData
    );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_ClearAsyncDoorbellList(
     __inout PDEVICE_DATA DeviceData
     );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_RingAsyncDoorbell(
     __inout PDEVICE_DATA DeviceData
     );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_iAsyncCacheFlush(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __in BOOLEAN DoorbellRetry
     );

VOID
EHCI_PollActiveControlEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
     );

VOID
EHCI_PollHaltedControlEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
     );

VOID
EHCI_PollPerAsyEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __in BOOLEAN AsyncEndpoint
     );

VOID
EHCI_PollControlEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_AssertQhChk(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_DATA EndpointData
     );

VOID
EHCI_SetNextTd(
    __in PDEVICE_DATA DeviceData,
    __inout PHCD_TRANSFER_DESCRIPTOR  LinkTd,
    __in PHCD_TRANSFER_DESCRIPTOR  NextTd,
    BOOLEAN SetAltNext
    );

VOID
EHCI_SetNextSlotTd(
    __in PDEVICE_DATA DeviceData,
    __inout PHCD_TRANSFER_DESCRIPTOR  LinkTd,
    __in PHCD_TRANSFER_DESCRIPTOR  NextTd,
    __in PHCD_TRANSFER_DESCRIPTOR  AltNextTd
    );

VOID
EHCI_SetAltNextTd(
    __in PDEVICE_DATA DeviceData,
    __inout PHCD_TRANSFER_DESCRIPTOR  LinkTd,
    __in PHCD_TRANSFER_DESCRIPTOR  NextTd
    );

VOID
EHCI_sMode_ResetSlot(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT Slot
     );

VOID
EHCI_sMode_InitSlot(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     ULONG Idx,
     __inout PENDPOINT_SLOT Slot,
     __inout PHCD_TRANSFER_DESCRIPTOR FirstTd,
     __inout PHCD_TRANSFER_DESCRIPTOR NextSlotFirstTd
     );

VOID
EHCI_sMode_InitEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
     );

VOID
EHCI_sMode_PollEndpoint(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __in BOOLEAN AsyncEndpoint
     );

BOOLEAN
EHCI_sMode_PollEndpointSlot(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT Slot
     );

VOID
EHCI_sMode_ActivateQh(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT Slot
     );

USB_MINIPORT_STATUS
EHCI_sMode_Transfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_PARAMETERS TransferUrb,
     __inout PTRANSFER_CONTEXT TransferContext,
     __inout PTRANSFER_SG_LIST TransferSGList,
     __in BOOLEAN AsyncEndpoint
     );

VOID
EHCI_sMode_AbortSlot(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT AbortSlot,
     __inout PTRANSFER_CONTEXT AbortTransferContext
    );

VOID
EHCI_sMode_AbortAsyncTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_CONTEXT AbortTransferContext
     );

ULONG
EHCI_CheckHwSync_Async(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    );

ULONG
EHCI_CheckHwSync(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_DecPendingTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_CONTEXT TransferContext
    );

VOID
EHCI_IncPendingTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_CONTEXT TransferContext
    );

PENDPOINT_SLOT
EHCI_sMode_IncSlotIdx(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PENDPOINT_SLOT Slot
    );

VOID
EHCI_sMode_ShortTransfer(
     __inout PDEVICE_DATA DeviceData,
     __inout PENDPOINT_DATA EndpointData,
     __inout PTRANSFER_CONTEXT TransferContext
    );

PVOID
EHCI_AllocNpPool(
     __in ULONG NumberOfBytes
     );

VOID
EHCI_iAssertQueueHead(
    __inout PDEVICE_DATA DeviceData,
    __inout PHCD_QUEUEHEAD_DESCRIPTOR Qh
    );

HW_32BIT_PHYSICAL_ADDRESS
EHCI_iGetAsyncListHwAddress(
    PDEVICE_DATA DeviceData
    );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_iEnableAsyncList(
    PDEVICE_DATA DeviceData
    );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_iDisableAsyncList(
    PDEVICE_DATA DeviceData
    );

PEHCI_IDLE_REF_CONTEXT
EHCI_RefAsyncIdle(
    __inout PDEVICE_DATA DeviceData,
    __in EHCI_IDLE_REF_TYPE RefType,
    __in_opt PVOID RefObject
    );

VOID
EHCI_DerefAsyncIdle(
    __inout PDEVICE_DATA DeviceData,
    __in PEHCI_IDLE_REF_CONTEXT Rx
    );

//****************************************************************************************
//
// INT.C Function Prototypes
//
//****************************************************************************************

BOOLEAN
EHCI_InterruptService (
    __inout PDEVICE_DATA DeviceData
    );

KSYNCHRONIZE_ROUTINE EHCI_InterruptDpcSyncRoutine;

ULONG
EHCI_InterruptDpcEx (
     __inout PDEVICE_DATA DeviceData,
     BOOLEAN EnableInterrupts
    );

VOID
EHCI_InterruptDpc (
    __inout PDEVICE_DATA DeviceData,
    BOOLEAN EnableInterrupts
    );

VOID
USBMPFN
EHCI_DisableInterrupts(
    __inout PDEVICE_DATA DeviceData
    );

VOID
USBMPFN
EHCI_FlushInterrupts(
    __inout PDEVICE_DATA DeviceData
    );

VOID
USBMPFN
EHCI_EnableInterrupts(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_RH_DisableIrq(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_RH_EnableIrq(
    __inout PDEVICE_DATA DeviceData
    );

ULONG
EHCI_Get32BitFrameNumber(
    __inout PDEVICE_DATA DeviceData
    );

ULONG
EHCI_Get32BitMicroFrameNumber(
     __inout PDEVICE_DATA DeviceData
    );

BOOLEAN
EHCI_HardwarePresent(
    __inout PDEVICE_DATA DeviceData,
    BOOLEAN Notify
    );

//****************************************************************************************
//
// ISO.C Function Prototypes
//
//****************************************************************************************

VOID
EHCI_RebalanceIsoEndpoint(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_InitializeSiTD(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PENDPOINT_PARAMETERS EndpointParameters,
    __inout PHCD_SI_TRANSFER_DESCRIPTOR SiTd,
    __in PHCD_SI_TRANSFER_DESCRIPTOR PrevSiTd,
    HW_32BIT_PHYSICAL_ADDRESS PhysicalAddress
    );

VOID
EHCI_InsertIsoTdsInSchedule(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout_opt PENDPOINT_DATA PrevEndpointData,
    __inout_opt PENDPOINT_DATA NextEndpointData
    );

VOID
EHCI_RemoveIsoTdsFromSchedule(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

USB_MINIPORT_STATUS
EHCI_OpenIsochronousEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_MapIsoPacketToTd(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PMINIPORT_ISO_PACKET Packet,
    __inout PHCD_SI_TRANSFER_DESCRIPTOR SiTd
    );

VOID
EHCI_CompleteIsoPacket(
    __inout PDEVICE_DATA DeviceData,
    __inout PMINIPORT_ISO_PACKET Packet,
    __inout PHCD_SI_TRANSFER_DESCRIPTOR SiTd
    );

PMINIPORT_ISO_PACKET
EHCI_GetPacketForFrame(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    _When_(return != NULL, _Out_) PTRANSFER_CONTEXT *Transfer,
    ULONG Frame
    );

VOID
EHCI_InternalPollIsoEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    BOOLEAN Complete
    );

VOID
EHCI_PollIsoEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_PollResetIsoEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

USB_MINIPORT_STATUS
EHCI_AbortIsoTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_CONTEXT TransferContext
    );

USB_MINIPORT_STATUS
EHCI_SubmitIsoTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_PARAMETERS TransferParameters,
    __inout PTRANSFER_CONTEXT TransferContext,
    __inout PMINIPORT_ISO_TRANSFER IsoTransfer
    );

VOID
EHCI_SetIsoEndpointState(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    MP_ENDPOINT_STATE State
    );

VOID
EHCI_Initialize_iTD(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PENDPOINT_PARAMETERS EndpointParameters,
    __inout PHCD_HSISO_TRANSFER_DESCRIPTOR IsoTd,
    HW_32BIT_PHYSICAL_ADDRESS PhysicalAddress,
    ULONG Frame
    );

VOID
EHCI_MapHsIsoPacketsToTd(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __in PMINIPORT_ISO_PACKET FirstPacket,
    __inout PHCD_HSISO_TRANSFER_DESCRIPTOR IsoTd,
    BOOLEAN InterruptOnComplete
    );

VOID
EHCI_CompleteHsIsoPackets(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData,
    __inout PMINIPORT_ISO_PACKET FirstPacket,
    __inout PHCD_HSISO_TRANSFER_DESCRIPTOR IsoTd
    );

USB_MINIPORT_STATUS
EHCI_OpenHsIsochronousEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_RemoveHsIsoTdsFromSchedule(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_InsertHsIsoTdsInSchedule(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout_opt PENDPOINT_DATA PrevEndpointData,
    __inout_opt PENDPOINT_DATA NextEndpointData
    );

VOID
EHCI_InternalPollHsIsoEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    BOOLEAN Complete,
    BOOLEAN AlwaysSchedule
    );

USB_MINIPORT_STATUS
EHCI_PokeIsoEndpoint(
     __in PDEVICE_DATA DeviceData,
     __in PENDPOINT_PARAMETERS EndpointParameters,
     __inout PENDPOINT_DATA EndpointData
    );

ULONG
EHCI_AddDummyQueueHeads(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_InterruptNextSOF(
    __inout PDEVICE_DATA DeviceData
    );

//****************************************************************************************
//
// PERIODIC.C Function Prototypes
//
//****************************************************************************************

VOID
EHCI_EnablePeriodicList(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_DisablePeriodicList(
    __inout PDEVICE_DATA DeviceData
    );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_iEnablePeriodicList(
    __inout PDEVICE_DATA DeviceData
    );

_Requires_lock_held_(DeviceData->AcfListSpin)
VOID
EHCI_iDisablePeriodicList(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_SetPeriodicScheduleState(
     __inout PDEVICE_DATA DeviceData,
     __in PERIODIC_SCHEDULE_STATE NextState
     );

VOID
EHCI_UpdatePeriodicScheduleState(
     __inout PDEVICE_DATA DeviceData,
     __in USBCMD cmd,
     __in USBSTS sts
     );

USB_MINIPORT_STATUS
EHCI_OpenInterruptEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __in PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_InsertQueueHeadInPeriodicList(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_RemoveQueueHeadFromPeriodicList(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

USB_MINIPORT_STATUS
EHCI_InterruptTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_PARAMETERS TransferUrb,
    __inout PTRANSFER_CONTEXT TransferContext,
    __inout PTRANSFER_SG_LIST TransferSGList
    );

PHCD_QUEUEHEAD_DESCRIPTOR
EHCI_GetQueueHeadForFrame(
    __in PDEVICE_DATA DeviceData,
    ULONG Frame
    );

VOID
EHCI_InitializeInterruptSchedule(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_WaitFrames(
     __inout PDEVICE_DATA DeviceData,
     ULONG Frames
    );

VOID
EHCI_RebalanceInterruptEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

ULONG
EHCI_CheckHwSync_Periodic(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

//****************************************************************************************
//
// ROOTHUB.C Function Prototypes
//
//****************************************************************************************

VOID
EHCI_RH_GetRootHubData(
    __in PDEVICE_DATA DeviceData,
    __out PROOTHUB_DATA HubData
    );

USB_MINIPORT_STATUS
EHCI_RH_SetWakeConnect(
     __inout PDEVICE_DATA DeviceData,
     ULONG Enable
    );

USB_MINIPORT_STATUS
EHCI_RH_GetStatus(
    __inout PDEVICE_DATA DeviceData,
    __out PUSHORT Status
    );

USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortEnable(
     __inout PDEVICE_DATA DeviceData,
     USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortPower(
     __inout PDEVICE_DATA DeviceData,
     USHORT PortNumber
    );

VOID
EHCI_RH_PortResumeComplete(
    __inout PDEVICE_DATA DeviceData,
    PVOID Context
    );

USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortSuspend (
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortSuspendChange (
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortOvercurrentChange (
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_GetPortStatus(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber,
    __out PRH_PORT_STATUS portStatus
    );

VOID
EHCI_RH_FinishReset(
    __inout PDEVICE_DATA DeviceData,
    __inout PVOID Context
    );

VOID
EHCI_RH_PortResetComplete(
    __inout PDEVICE_DATA DeviceData,
    __inout PVOID Context
    );

USB_MINIPORT_STATUS
EHCI_RH_SetFeaturePortReset(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_SetFeaturePortSuspend(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_SetFeaturePortPower(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_SetFeaturePortEnable(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortConnectChange(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortEnableChange(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_GetHubStatus(
    __inout PDEVICE_DATA DeviceData,
    __out PRH_HUB_STATUS HubStatus
    );

USB_MINIPORT_STATUS
EHCI_RH_ClearFeaturePortResetChange(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USB_MINIPORT_STATUS
EHCI_RH_UsbprivRootPortStatus(
    __inout PDEVICE_DATA DeviceData,
    ULONG ParameterLength,
    __inout PVOID Parameters
    );

USB_MINIPORT_STATUS
EHCI_RH_ChirpRootPort(
    __inout PDEVICE_DATA DeviceData,
    USHORT PortNumber
    );

USHORT
EHCI_GetDebugPortNumber(
    __in PDEVICE_DATA DeviceData
    );

BOOLEAN
EHCI_CheckInvalidateRootHub(
    __in PDEVICE_DATA DeviceData
    );

VOID
EHCI_SetPortResumeTime(
    __in PDEVICE_DATA DeviceData,
    __in USHORT PortNumber,
    __in BOOLEAN SetTime,
    __in UCHAR ResumeTime
    );


//****************************************************************************************
//
// SSTOOL.C Function Prototypes
//
//****************************************************************************************

USB_MINIPORT_STATUS
USBMPFN
EHCI_StartSendOnePacket(
     __inout PDEVICE_DATA DeviceData,
     __in PMP_PACKET_PARAMETERS PacketParameters,
     __inout_bcount(*PacketLength) PUCHAR PacketData,
     __in PULONG PacketLength,
     __inout_xcount(WorkSpaceLength) PUCHAR WorkspaceVirtualAddress,
     HW_32BIT_PHYSICAL_ADDRESS WorkspacePhysicalAddress,
     ULONG WorkSpaceLength,
     OUT USBD_STATUS *UsbdStatus
    );

USB_MINIPORT_STATUS
USBMPFN
EHCI_EndSendOnePacket(
     __inout PDEVICE_DATA DeviceData,
     __in PMP_PACKET_PARAMETERS PacketParameters,
     __out_bcount(*PacketLength) PUCHAR PacketData,
     __inout PULONG PacketLength,
     __inout_xcount(WorkSpaceLength) PUCHAR WorkspaceVirtualAddress,
     HW_32BIT_PHYSICAL_ADDRESS WorkspacePhysicalAddress,
     __in ULONG WorkSpaceLength,
     __out USBD_STATUS *UsbdStatus
    );

//****************************************************************************************
//
// USBEHCI.C Function Prototypes
//
//****************************************************************************************

VOID
EHCI_AssertHwAlignment(
    __inout PDEVICE_DATA DeviceData,
    __in HW_32BIT_PHYSICAL_ADDRESS HwPhysicalAddress,
    __in ULONG Length
    );

USB_MINIPORT_STATUS
EHCI_InitializeHardware(
    __inout PDEVICE_DATA DeviceData
    );

// The "StaticQHs" common buffer allocation is used for the following
// sub-allocations:
//
// (1) Frame list.
// (2) 64 static interrupt queue heads.
// (3) Asynchronous List Persistent Queue Head.
// (4) Asynchronous List Persistent Queue Head Dummy.
//
#define USBEHCI_COMMON_BUFFER_BYTES \
    ((USBEHCI_MAX_FRAME * sizeof(HW_32BIT_PHYSICAL_ADDRESS)) + \
     (64 * sizeof(HCD_QUEUEHEAD_DESCRIPTOR)) + \
     sizeof(HCD_QUEUEHEAD_DESCRIPTOR) + \
     sizeof(HCD_QUEUEHEAD_DESCRIPTOR))

USB_MINIPORT_STATUS
EHCI_InitializeSchedule(
     __inout PDEVICE_DATA DeviceData,
    __inout_bcount(USBEHCI_COMMON_BUFFER_BYTES) PUCHAR StaticQHs,
     HW_32BIT_PHYSICAL_ADDRESS StaticQHsPhys,
     __inout PUCHAR DummyQHs,
     HW_32BIT_PHYSICAL_ADDRESS DummyQHsPhys
    );

VOID
EHCI_ReadUlongRegFlag(
     __inout PDEVICE_DATA DeviceData,
     __in_bcount(FlagNameSize) PCWSTR FlagName,
     ULONG FlagNameSize,
     __in ULONG DefaultFlagValue,
     __out_opt PULONG Flag,
     __out_opt PBOOLEAN FoundInRegistry,
     BOOLEAN SoftwareBranch
    );

VOID
EHCI_ReadUlongRegValue(
     __inout PDEVICE_DATA DeviceData,
     __in_bcount(ValueNameSize) PCWSTR ValueName,
     ULONG ValueNameSize,
     __in ULONG DefaultValue,
     __out_opt PULONG Value,
     __out_opt PBOOLEAN FoundInRegistry,
     BOOLEAN SoftwareBranch
    );

VOID
EHCI_GetRegistryParameters(
     __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_HaltController(
    __inout PDEVICE_DATA DeviceData
    );

VOID
USBMPFN
EHCI_StopController(
    __inout PDEVICE_DATA DeviceData,
    BOOLEAN HwPresent
    );

VOID
USBMPFN
EHCI_TakePortControl(
    __inout PDEVICE_DATA DeviceData
    );

VOID
USBMPFN
EHCI_ReleasePortControl(
     __inout PDEVICE_DATA DeviceData
    );

ULONG
USBMPFN
EHCI_ReadConfigFlag(
     __inout PDEVICE_DATA DeviceData
    );

VOID
USBMPFN
EHCI_CreateDeviceData(
    __inout PDEVICE_DATA DeviceData
    );

VOID
USBMPFN
EHCI_DeleteDeviceData(
    __inout PDEVICE_DATA DeviceData
    );

USB_MINIPORT_STATUS
USBMPFN
EHCI_PreStartController(
    __inout PDEVICE_DATA DeviceData,
    __in PHC_RESOURCES HcResources
    );

USB_MINIPORT_STATUS
USBMPFN
EHCI_StartController(
    __inout PDEVICE_DATA DeviceData,
    __in PHC_RESOURCES HcResources
    );

USB_MINIPORT_STATUS
USBMPFN
EHCI_GetHcCapabilities(
    __inout PDEVICE_DATA                DeviceData,
    __inout PMINIPORT_HC_CAPABILITIES   HcCapabilities
    );

VOID
EHCI_ClearSavedRegisters(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_AssertSavedRegisters(
    __inout PDEVICE_DATA DeviceData
    );

VOID
EHCI_SuspendController(
    __inout PDEVICE_DATA DeviceData
    );

USB_MINIPORT_STATUS
EHCI_ResumeController(
    __inout PDEVICE_DATA DeviceData
    );

USB_MINIPORT_STATUS
EHCI_OpenEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_CloseEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_DbgFreeEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_DbgCheckEndpoints(
      __inout PDEVICE_DATA DeviceData
    );

USB_MINIPORT_STATUS
EHCI_PokeEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_RebalanceEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_PARAMETERS EndpointParameters,
    __inout PENDPOINT_DATA EndpointData
    );

USB_MINIPORT_STATUS
EHCI_QueryEndpointRequirements(
    __inout PDEVICE_DATA DeviceData,
    __in PENDPOINT_PARAMETERS EndpointParameters,
    __out PENDPOINT_REQUIREMENTS EndpointRequirements
    );

VOID
EHCI_PollEndpoint(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

PHCD_TRANSFER_DESCRIPTOR
EHCI_AllocTd(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_SetEndpointStatus(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    MP_ENDPOINT_STATUS Status
    );

MP_ENDPOINT_STATUS
EHCI_GetEndpointStatus(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData
    );

VOID
EHCI_SetEndpointState(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    MP_ENDPOINT_STATE State
    );

MP_ENDPOINT_STATE
EHCI_GetEndpointState(
    __in PDEVICE_DATA DeviceData,
    __in PENDPOINT_DATA EndpointData
    );

VOID
EHCI_PollController(
    __inout PDEVICE_DATA DeviceData
    );

USB_MINIPORT_STATUS
EHCI_SubmitTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_PARAMETERS TransferUrb,
    __inout PTRANSFER_CONTEXT TransferContext,
    __inout PTRANSFER_SG_LIST TransferSGList
    );

VOID
EHCI_AbortTransfer(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    __inout PTRANSFER_CONTEXT TransferContext,
    __inout PULONG BytesTransferred
    );

USB_MINIPORT_STATUS
EHCI_GetIdleStatus(
    PDEVICE_DATA DeviceData,
    ULONG ParameterLength,
    PVOID Parameters
    );

USB_MINIPORT_STATUS
EHCI_PassThru (
     __inout PDEVICE_DATA DeviceData,
     __in GUID *FunctionGuid,
     ULONG ParameterLength,
     __inout_bcount(ParameterLength) PVOID Parameters
    );

VOID
EHCI_SetEndpointDataToggle(
    __inout PDEVICE_DATA DeviceData,
    __inout PENDPOINT_DATA EndpointData,
    ULONG Toggle
    );

#if DBG
#define EHCI_ASSERT_RUN_BIT(dd) EHCI_AssertRunBit((dd))
#else
#define EHCI_ASSERT_RUN_BIT(dd)
#endif

VOID
EHCI_AssertRunBit(
    __inout PDEVICE_DATA DeviceData
    );

UCHAR
EHCI_GetEECP(
    __inout PDEVICE_DATA DeviceData,
    UCHAR CapabilityId
    );

USB_MINIPORT_STATUS
EHCI_StopBIOS(
    __inout PDEVICE_DATA DeviceData
    );

PHC_DEBUGPORT_REGISTERS
EHCI_LocateDebugPortRegisters(
    __inout PDEVICE_DATA    DeviceData,
    __in PHC_RESOURCES   HcResources
    );

NTSTATUS
EHCI_EvaluateAcpiRoutine(
    IN  PDEVICE_OBJECT           TargetDeviceObject,
    IN  ULONG                    MethodName
    );

NTSTATUS
EHCI_EvaluateAcpiRoutineOnHcReset(
    IN  PDEVICE_OBJECT           TargetDeviceObject
    );

VOID
EHCI_NotifyTransferQueueState(
    __inout PDEVICE_DATA DeviceData,
    __in ULONG AsyncTransferCount,
    __in ULONG PeriodicTransferCount
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
EHCI_CheckController(
    __inout PDEVICE_DATA DeviceData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EHCI_WriteErrorLogEntry(
    __in PDEVICE_DATA   DeviceData,
    __in NTSTATUS       ErrorCode,
    __in BOOLEAN        IsLockHeld
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
EHCI_ResetController(
    __inout PDEVICE_DATA DeviceData
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
EHCI_ResetControllerDontReinit(
    __inout PDEVICE_DATA DeviceData
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
EHCI_ReinitController(
    __inout PDEVICE_DATA DeviceData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
EHCI_PostResetController(
    __inout PDEVICE_DATA DeviceData
    );

#endif /* __EHCI_H__ */
