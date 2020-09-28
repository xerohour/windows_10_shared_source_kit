/*++

Copyright (c) 2004  Microsoft Corporation

Module Name:

    usbport.h

Abstract:

    private header for usb port driver

Environment:

    Kernel & user mode

Revision History:

    10-27-95 : created
    01-12-04 : fix bugs

--*/

#ifndef   __USBPORT_H__
#define   __USBPORT_H__

#include "triage-usbport.h"
#include "smstructs.h"

/* This is the goatcode */
#define USBPORT_TRACKING_ID              3

/*
It is not quite clear what the 'disconnect' status code for NT is.
-- We use a macro in case we need to change it.
*/
#define NT_DEVICE_DISCONNECTED_STATUS STATUS_NO_SUCH_DEVICE

typedef enum _T_WORK_STATE {

    t_work_off = 0,
    t_work_run = 1,
    t_work_run_again = 2

} T_WORK_STATE;



//third parm to KeInitializeEvent
#define USBD_EVENT_STATE_SIGNALED        TRUE
#define USBD_EVENT_STATE_NOT_SIGNALED    FALSE

#define USBD_STATUS_NOT_SET     0xFFFFFFFF

// The USBPORT_ADDRESS_AND_SIZE_TO_SPAN_PAGES_4K macro takes a virtual address
// and size and returns the number of host controller 4KB pages spanned by
// the size.
//
#define USBPORT_ADDRESS_AND_SIZE_TO_SPAN_PAGES_4K(Va,Size) \
   (((((Size) - 1) >> USB_PAGE_SHIFT) + \
   (((((ULONG)(Size-1)&(USB_PAGE_SIZE-1)) + (PtrToUlong(Va) & (USB_PAGE_SIZE -1)))) >> USB_PAGE_SHIFT)) + 1L)

#define IS_ADDR_32_BYTE_ALIGNED(Va) ( (((ULONG_PTR)Va) % 32) == 0 )
#define IS_ADDR_8_BYTE_ALIGNED(Va) ( (((ULONG_PTR)Va) % 8) == 0 )
#define IS_ADDR_4_BYTE_ALIGNED(Va) ( (((ULONG_PTR)Va) % 4) == 0 )

#define STATUS_BOGUS            0xFFFFFFFF

// deadman timer interval in milliseconds
#define USBPORT_DM_TIMER_INTERVAL   1000
#define USBPORT_DM_TIMER_TOLERABLE_DELAY 1000

// DPC Watchdog Timeout Threshold Percentage
#define USBPORT_REMAINING_DPC_TIMEOUT_THRESHOLD  50

/*
    Registry Keys
*/

// Software Branch PDO Keys
// HKLM\SYSTEM\CCS\Control\Class\{36FC9E60-C465-11CF-8056-444553540000}\xxxx
//
#define USBPORT_SW_BRANCH   TRUE

#define FLAVOR_KEY                      L"HcFlavor"
#define BW_KEY                          L"TotalBusBandwidth"
#define DISABLE_SS_KEY                  L"HcDisableAllSelectiveSuspend"

// Limit common buffer allocations for the miniport to the physical
// address range below 2GB.  Only bits 0 through 30 of the physical
// address can be set.  Bit 31 of the physical address cannot be set.
//
#define PDOKEY_COMMON_BUFFER_2GB_LIMIT  L"CommonBuffer2GBLimit"

#define PDOKEY_FORCE_HC_RESET_ON_RESUME L"ForceHCResetOnResume"

#define PDOKEY_FAST_RESUME_ENABLE       L"FastResumeEnable"

// Hardware Branch PDO Keys
// HKLM\CCS\ENUM\PCI\DeviceParameters
#define USBPORT_HW_BRANCH   FALSE

#define DS_PCI_CONFIG_SPACE             L"DsPciConfigSpace"

#define SYM_LINK_KEY                    L"SymbolicName"
#define SYM_LEGSUP_KEY                  L"DetectedLegacyBIOS"

// Global Reg Keys HKLM\CCS\Services\USB
#define DEBUG_LEVEL_KEY                 L"debuglevel"
#define DEBUG_LOG_MASK                  L"debuglogmask"
#define DEBUG_CATC_ENABLE               L"debugcatc"
#define DEBUG_LOG_ENABLE                L"debuglogenable"

#define G_DISABLE_SS_KEY                L"DisableSelectiveSuspend"
#define G_DISABLE_CC_DETECT_KEY         L"DisableCcDetect"
#define G_FORCE_HC_D3_NO_WAKE_ARM       L"ForceHcD3NoWakeArm"


// debug keys
// for debugging power management
#define G_ENABLE_PM_DEBUG_KEY            L"EnPMDebug"


//direct controller access (used for WHQL diagnostic applications)
#define ENABLE_DCA                      L"EnableDCA"
// Disables CC handoff, this means when EHCI root hub is disabled the ports still remian under
// EHCI control.  This behavior is identical to XP and is used for the HCT tests that rely
// on this behavior.
// ** This flag takes effect only if a USBUSER_ mode IOCTL is issued to the stack and is cleared on the next
// ** re-start of the root hub.   It is should used only for compatibility with older versions of the HCT and
// ** similar diagnostics.
#define FORCE_PORTS_HS                  L"ForcePortsHighSpeed"

/*
    HC types
    define known HC types

*/

// Opti Hydra derivative
#define HC_VID_OPTI             0x1045
#define HC_PID_OPTI_HYDRA       0xC861

/*

Intel UHCI controller

PIIX4
ICH     2412
ICH0    2422
ICH2    2444

ICH3M   2482
ICH3M   2484
ICH3M   2487

ICH4    24C2 (uhci)
ICH4    24C4 (uhci)
ICH4    24C7 (uhci)

25a9 uhci
25aa uhci


*/
#define HC_VID_INTEL            0x8086

#define PIIXn_MASK              0x7000

#define HC_PID_PIIX3            0x7020
#define HC_PID_PIIX4            0x7112
#define HC_PID_PIIX5            0x719a

#define ICHn_MASK               0x2000

#define HC_PID_INTEL_ICH1_1     0x2412
#define HC_PID_INTEL_ICH1_2     0x2422

#define HC_PID_INTEL_ICH2_1     0x2442
#define HC_PID_INTEL_ICH2_2     0x2444

#define HC_PID_INTEL_ICH3M_1    0x2482
#define HC_PID_INTEL_ICH3M_2    0x2484
#define HC_PID_INTEL_ICH3M_3    0x2487

#define HC_PID_INTEL_ICH4_1     0x24C2
#define HC_PID_INTEL_ICH4_2     0x24C4
#define HC_PID_INTEL_ICH4_3     0x24C7

// these will map to UHCI_Intel, IDs are recorded here for reference
#define HC_PID_INTEL_ICH5_1     0x24D7
#define HC_PID_INTEL_ICH5_2     0x24D2
#define HC_PID_INTEL_ICH5_3     0x24D4
#define HC_PID_INTEL_ICH5_4     0x24DE

#define HC_PID_INTEL_ICH6_1     0x2658
#define HC_PID_INTEL_ICH6_2     0x2659
#define HC_PID_INTEL_ICH6_3     0x265a
#define HC_PID_INTEL_ICH6_4     0x265b

// VIA USB controller
#define HC_VID_VIA              0x1106
#define HC_PID_VIA              0x3038

// NEC USB companion controller
#define HC_VID_NEC_CC           0x1033
#define HC_PID_NEC_CC           0x0035
#define HC_REV_NEC_CC           0x41

// VIA USB companion controller
#define HC_VID_VIA_CC           0x1106
#define HC_PID_VIA_CC           0x3038
#define HC_REV_VIA_CC           0x50


// Intel USB companion controller (ICH4)
#define HC_VID_INTEL_CC         0x8086
#define HC_PID_INTEL_CC1        0x24C2
#define HC_PID_INTEL_CC2        0x24C4
#define HC_PID_INTEL_CC3        0x24C7

#define HC_HWID_NVIDIA_TEGRA2_TEMP "NVD_USBEHCI"
#define HC_HWID_NVIDIA_TEGRA2    "NVDA0003"
#define HC_HWID_NVIDIA_TEGRA3    "NVDA0103"

#define HC_HWID_INTEL_MEDFIELD   "PNP0FFC"

#define PENDPOINT_DATA PVOID
#define PDEVICE_DATA PVOID
#define PTRANSFER_CONTEXT PVOID

/* high speed miniport context */
#define PHSB_HC_DATA PVOID
#define PHSB_TT_DATA PVOID
#define PHSB_EP_DATA PVOID


// the maximum interval we support for an interrupt
// endpoint in the schedule, larger intervals are
// rounded down
#define USBPORT_MAX_INTEP_POLLING_INTERVAL    32

//
// Maximum transfer sizes per URB
//
// These values are returned to function drivers in the _USBD_PIPE_INFORMATION structure.  They indicate the
// largest transfer the port driver will allow in a single IRP/URB request.
//
// ** These values are enforced.  If a function drivers may submits transfers with larger sizes they will be
// failed with USBD_STATUS_INVALID_PARAMETER.
//
#define USBPORT_MAX_CONTROL_TRANSFER_SIZE       1024*64
#define USBPORT_MAX_INTERRUPT_TRANSFER_SIZE     1024*1024*4
#define USBPORT_MAX_BULK_TRANSFER_SIZE          1024*1024*4

/*
    Power structures
*/

#define USBPORT_MAPPED_SLEEP_STATES     5

//
// ETW related event tags
//
#define ETW_PUT_SG_LIST_TAG             L"PUT_SGLIST"
#define ETW_BUILD_SG_LIST_TAG           L"BUILD_SGLIST"

typedef enum _HC_POWER_ATTRIBUTES {
    HcPower_Y_Wakeup_Y = 0,
    HcPower_N_Wakeup_N,
    HcPower_Y_Wakeup_N,
    HcPower_N_Wakeup_Y
} HC_POWER_ATTRIBUTES;

typedef struct _HC_POWER_STATE {
    SYSTEM_POWER_STATE  SystemState;
    DEVICE_POWER_STATE  DeviceState;
    HC_POWER_ATTRIBUTES Attributes;
} HC_POWER_STATE, *PHC_POWER_STATE;

typedef struct _HC_POWER_STATE_TABLE {
    HC_POWER_STATE PowerState[USBPORT_MAPPED_SLEEP_STATES];
} HC_POWER_STATE_TABLE, *PHC_POWER_STATE_TABLE;

/*
used for dpc and transfer timeouts
*/
typedef enum _USB_TIMEOUT_STATE {
    Tmo_Disabled = 0,
    Tmo_Enabled,
    Tmo_DisabledExpired
} USB_TIMEOUT_STATE;

typedef struct _USB_TIMEOUT {

    ULONG Sig;
    PDEVICE_OBJECT HcFdo;
    // state, allows us to disable after the timeout expires
    USB_TIMEOUT_STATE TmoState;
    // original ms timeout
    ULONG MillisecondTimeout;
    // ms timeout converted to ticks
    LARGE_INTEGER TmoTimeoutTickCount;
    // init time tick count
    LARGE_INTEGER TmoBeginTickCount;
    // tick count when this timer expires
    LARGE_INTEGER TmoExpireTickCount;
    // backpointer for debugging
    PVOID OwnerObject;

} USB_TIMEOUT, *PUSB_TIMEOUT;


/*
    common structure used to represent transfer
    requests
*/

typedef struct _TRANSFER_URB {

    struct _URB_HEADER Hdr;

    PVOID UsbdPipeHandle;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;
    union {
        ULONG Timeout;              // no Linked Urbs
        PVOID ReservedMBNull;       // fields
    };
    struct _USBPORT_DATA pd;        // fields for USBPORT use

    union {
        struct {
            ULONG StartFrame;
            ULONG NumberOfPackets;
            ULONG ErrorCount;
            USBD_ISO_PACKET_DESCRIPTOR IsoPacket[0];
        } Isoch;
        UCHAR SetupPacket[8];
    } u;

} TRANSFER_URB, *PTRANSFER_URB;

/* Internal IoRequest tracking structure */
//USBPORT_StartDM_Timer, USBPORT_DM_TimerDpc
#define UsbIoTag_DmTimer        'MTmD'  // DmTM
//USBPORT_InvalidateController
#define UsbIoTag_SimIrq         'Imis'  // simI
//UsbIoTag_HcWakeIrp
#define UsbIoTag_HcWakeIrp      'KWch'  // hcWK
//USBPORT_Ev_Rh_IntrEp_Invalidate
#define UsbIoTag_RhDpc          'pdHR'  // RHdp
//UsbIoTag_SSPworker
#define UsbIoTag_SSPworker      'Wpss'  // sspW
//USBPORT_InvalidateController
#define UsbIoTag_HardwareReset  '!Rwh'  // hwR!
//USBPORT_InvalidateController
#define UsbIoTag_SurpriseRemove 'pruS'  // Surp
//USBPORT_QueuePowerWorker
#define UsbIoTag_PowerWorker    'Wwop'  // powW
//USBPORTSVC_RequestAsyncCallbackEx
#define UsbIoTag_MpAsyncTimer   'mtsA'  // Astm
//USBPORT_Dispatch (irps that come thru main dispatch)
#define UsbIoTag_IoIrp          'PRIi'  // iIRP
//USBPORT_FdoPnP
#define UsbIoTag_FdoPnpIrp      'pnpF'  // Fpnp
//USBPORT_PdoPnP
#define UsbIoTag_PdoPnpIrp      'pnpP'  // Ppnp
//USBPORT_InitStateVars
#define UsbIoTag_PnpAdd         'DddA'  // AddD
//
#define UsbIoTag_PnpStart       'trtS'  // Strt
//USBPORT_CreateRootHubPdo
#define UsbIoTag_RhCreate       'rcHR'  // RHcr
//USBPORT_Xdpc_Enable
#define UsbIoTag_Xdpc           'cpdX'  // Xdpc
//USBPORT_Core_iMapTransfer
#define UsbIoTag_IoMap          'pamI'  // Imap

#define UsbIoTag_IsrDpc         'dRSI'  // ISRd

#define UsbIoTag_TESTworker     'tset'  // test

typedef struct _USB_IOREQUEST_CONTEXT {

    LIST_ENTRY IoRequestLink;

    PDEVICE_OBJECT DevObj;

    ULONG Sig;
    ULONG Tag;
    PVOID Object;
    PVOID OriginalObject;

    BOOLEAN FreeContext;
    BOOLEAN ObjUnlinked;

} USB_IOREQUEST_CONTEXT, *PUSB_IOREQUEST_CONTEXT;


typedef struct _HCD_STATE_LOG_ENTRY {

    ULONG  Event;
    ULONG  CurrentState;
    ULONG  NextState;

    union {
        MP_ENDPOINT_STATE   HwEpState;
        ULONG               Tag;
        ULONG               Info;
    } i;

} HCD_STATE_LOG_ENTRY, *PHCD_STATE_LOG_ENTRY;

// Internal work item structure used by USBPORT_QueuePowerWorker() and
// USBPORT_UsbPowerWorker()

typedef enum _USB_POWER_WORK_PHASE {

    PowerWorkPhase1,
    PowerWorkPhase2

} USB_POWER_WORK_PHASE;

typedef struct _USB_POWER_WORK {
     PIO_WORKITEM           QueueItem;
     PDEVICE_OBJECT         FdoDeviceObject;
     PIRP                   SystemPowerIrp;
     USB_IOREQUEST_CONTEXT  UsbIoRequest;
     USB_POWER_WORK_PHASE   PowerWorkPhase;
} USB_POWER_WORK, *PUSB_POWER_WORK;

/* tracking information for OCA online crash analysis */

// save 16 chars of driver name
#define USB_DRIVER_NAME_LEN 16

/*
    BUS_CONTEXT Object

    This object is returned for both types of interface queries,
    it should not be freed until all references to it are gone.

    A separate instance of the bus context is created for each device
    on the bus. The bus context is a static structure that a function
    driver can use to refer to its device, its safer that the device
    handle because it won't go away until the caller frees it.
*/

typedef enum _USBPORT_BUS_CONTEXT_WAKE {
    BusCtx_No_WakeUsb = 0,
    BusCtx_WakeUsb,
} USBPORT_BUS_CONTEXT_WAKE;

typedef enum _USBPORT_BUS_CONTEXT_STATE {
    Bc_Invalid = 0,
    Bc_Valid,
    Bc_Removed,
    Bc_Released      // all external references have been removed
} USBPORT_BUS_CONTEXT_STATE;

typedef enum _USBPORT_BUS_CONTEXT_EVENT {
    EvBc_Create = 1,
    EvBc_Usbd_Ref,
    EvBc_Usbd_Deref,
    EvBc_Usbh_Ref,
    EvBc_Usbh_Deref,
    EvBc_Remove,
    EvBc_UsbMinid_Ref,
    EvBc_UsbMinid_Deref,
    EvBc_UsbHubSS_Ref,
    EvBc_UsbHubSS_Deref,
    EvBc_UsbFp_Ref,
    EvBc_UsbFp_Deref,
} USBPORT_BUS_CONTEXT_EVENT;

typedef struct _BUS_CONTEXT {
    ULONG BusCxtSig;

    USBPORT_BUS_CONTEXT_STATE BcState;

    LONG InterfaceRefCount;

    // these are used for diagnostic purposes in the event of a ref leak
    // if usbdref is non zero then the leak is probably in the function
    // driver.
    LONG UsbdRefCount;
    LONG UsbhRefCount;
    LONG UsbMinidRefCount;
    LONG UsbHubSSRefCount;

    USBPORT_BUS_CONTEXT_WAKE WakeState;

    LIST_ENTRY NextEntry;

    PDEVICE_OBJECT RhPdo;
    PDEVICE_OBJECT HcFdo;

    // DEVH_BUS_CONTEXT_TAG
    struct _USBD_DEVICE_HANDLE *DeviceHandle;
} BUS_CONTEXT, *PBUS_CONTEXT;


typedef struct _USB_ABORT_PIPE_CONTEXT {

    // Link to DEVICE_EXTENSION AxCsqIrpListHead list.  Protected by
    // USBPORT_TxAcquireLock() / USBPORT_TxReleaseLock.
    //
    LIST_ENTRY  AxCsqIrpLink;

    // Link to DEVICE_EXTENSION Fdo.AxPendingListHead list.  Protected
    // by USBPORT_TxAcquireLock() / USBPORT_TxReleaseLock.
    //
    LIST_ENTRY  AxPendingLink;

    // Link to DEVICE_EXTENSION Fdo.AxDoneListHead list.  Protected by
    // USBPORT_TxAcquireLock() / USBPORT_TxReleaseLock.
    //
    LIST_ENTRY  AxDoneLink;

    // Link to DEVICE_EXTENSION Fdo.AxCanceledListHead list.  Protected
    // by USBPORT_TxAcquireLock() / USBPORT_TxReleaseLock.
    //
    LIST_ENTRY  AxCanceledLink;

    // SIG_ABORT_PIPE_CONTEXT
    //
    ULONG       Sig;

    PURB        Urb;

    PIRP        Irp;

    // USBD_DEVICE_HANDLE referenced by the IRP and URB.
    //
    struct _USBD_DEVICE_HANDLE *DeviceHandle;

    // Optional event signaled by USBPORT_Core_CompleteAbortRequest().
    //
    PKEVENT     Event;

    PKTHREAD    Thread;

    struct _HCD_ENDPOINT    *Endpoint;

    struct _USBOBJ_REF      *EpRefHandle;

    // Context used for IoCsqInsertIrpEx() and IoCsqRemoveIrp().
    //
    IO_CSQ_IRP_CONTEXT CsqContext;

    // List of transfers associated with this USB_ABORT_PIPE_CONTEXT.
    // Initialized by USBPORT_Core_iAbortEndpoint().  Updated by
    // USBPORT_Core_iCheckAbortList() and USBPORT_AxCsqRemoveIrp().
    // Protected by USBPORT_TxAcquireLock() / USBPORT_TxReleaseLock.
    //
    LIST_ENTRY  TxListHead;

    // Reference on this USB_ABORT_PIPE_CONTEXT taken by
    // USBPORT_AxCsqInsertIrpEx() and released by
    // USBPORT_AxCsqRemoveIrp().  This reference prevents
    // USBPORT_Core_FlushCompletedAborts() from freeing this
    // USB_ABORT_PIPE_CONTEXT.  Protected by USBPORT_TxAcquireLock() /
    // USBPORT_TxReleaseLock.
    //
    ULONG       USBPORT_AxCsqRemoveIrpRef;

    // Reference on this USB_ABORT_PIPE_CONTEXT taken by and released by
    // USBPORT_Core_FlushCompletedAborts().  This reference prevents
    // USBPORT_AxCsqRemoveIrp() from freeing this
    // USB_ABORT_PIPE_CONTEXT.  Protected by USBPORT_TxAcquireLock() /
    // USBPORT_TxReleaseLock.

    ULONG       USBPORT_Core_FlushCompletedAbortsRef;

    // Fields for minidump debugging.
    //
    USHORT      PciVendorId;
    USHORT      PciDeviceId;
    USHORT      DeviceVID;
    USHORT      DevicePID;
    WCHAR       DriverName[USB_DRIVER_NAME_LEN];

} USB_ABORT_PIPE_CONTEXT, *PUSB_ABORT_PIPE_CONTEXT;

// Check against the current structure size limit hard coded into
// DBGENG.DLL
//
C_ASSERT(sizeof(USB_ABORT_PIPE_CONTEXT) < USBPORT_MAXIMUM_USB_ABORT_PIPE_CONTEXT_SIZE);

/*
    This is the structure we use to track common buffer blocks we allocate.
*/

typedef struct _USBPORT_COMMON_BUFFER {

    ULONG Sig;

    // Entry on the free list
    //
    LIST_ENTRY FreeListEntry;

    // The common buffer allocation length passed to
    // AllocateCommonBuffer
    //
    ULONG TotalLength;

    // The virtual address returned by AllocateCommonBuffer
    //
    __field_bcount(TotalLength) PVOID VirtualAddress;

    // The logical (physical) address returned by AllocateCommonBuffer
    //
    PHYSICAL_ADDRESS LogicalAddress;

    // The page-aligned virtual address passed to the miniport
    //
    __field_bcount(TotalLength) PVOID MiniportVa;

    // The page-aligned logical (32-bit physical) address passed to the miniport
    //
    HW_32BIT_PHYSICAL_ADDRESS MiniportPhys;

} USBPORT_COMMON_BUFFER, *PUSBPORT_COMMON_BUFFER;

/*
    structure we use to track bound drivers
*/

typedef struct _USBPORT_MINIPORT_DRIVER {

    // driver object assocaited with this particular
    // miniport
    PDRIVER_OBJECT DriverObject;

    LIST_ENTRY ListEntry;

    PDRIVER_UNLOAD MiniportUnload;

    ULONG HciVersion;

    UNICODE_STRING RegistryPath;

    // copy of the registration packet passed in
    USBPORT_REGISTRATION_PACKET RegistrationPacket;

} USBPORT_MINIPORT_DRIVER, *PUSBPORT_MINIPORT_DRIVER;


/*
    A separate context structure used for IRP tracking.
    we do this because clients frequently free the IRP
    while it is pending corrupting any lists linked with
    the irp itself.
*/
typedef struct _USB_IRP_CONTEXT {
    ULONG Sig;
    LIST_ENTRY ListEntry;
    struct _USBD_DEVICE_HANDLE *DeviceHandle;
    PIRP Irp;
} USB_IRP_CONTEXT, *PUSB_IRP_CONTEXT;

// indicates that the transfer was in the miniport when canceled and may contain some data
#define USBPORT_TXFLAG_MPCANCELED           0x00000001
#define USBPORT_TXFLAG_MAPPED               0x00000002
#define USBPORT_TXFLAG_HIGHSPEED            0x00000004
// Shadow copy of USBPORT_REQUEST_MDL_ALLOCATED flag from URB UsbdFlags
#define USBPORT_TXFLAG_MDL_ALLOCATED        0x00000008
#define USBPORT_TXFLAG_ISO                  0x00000020
// indicates USBPORT has timed out this transfer
#define USBPORT_TXFLAG_TIMEOUT              0x00000040
#define USBPORT_TXFLAG_DEVICE_GONE          0x00000080
//indicates that the transfer was explicitky completed by miniport
#define USBPORT_TXFLAG_MPCOMPLETED          0x00000200
#define USBPORT_TXFLAG_RH_CONTROL           0x00000800
#define USBPORT_TXFLAG_RH_INTERRUPT         0x00001000
// indicates the error code has been set for a split transfer, all split child status codes
// are ignored once this flag is set. (The split that generated the error is the one we want to return.)
#define USBPORT_TXFLAG_SPLIT_ERROR          0x00002000
// transfer sent by USBPORT_SendCommandEx
#define USBPORT_TXFLAG_URB_CALLBACK         0x00004000
// indicates csq 'complete-canceled-irp' has been called for this transfer
#define USBPORT_TXFLAG_IO_CANCEL            0x10000000



typedef enum _USBPORT_TRANSFER_DIRECTION {
    NotSet = 0,
    ReadData,       // ie in
    WriteData,      // ie out
} USBPORT_TRANSFER_DIRECTION;

/*
define types for split transfers
*/

typedef enum _TX_TYPE {
    Tx_NOT_SPLIT = 0,
    Tx_SPLIT_PARENT,
    Tx_SPLIT_CHILD,
} TX_TYPE;


/*
    USB Transfer States
*/

typedef enum _TX_STATE {

    //0x00
    TX_Undefined = 0,
    //0x01
    // transfer is in the priority queue
    // transfer is in the (cancel safe) queue
    TX_InQueue,
    //0x02
    // transfer is in the priority queue
    // transfer is in the (cancel safe) queue
    TX_MapPending,
    //0x03
    TX_MapPending_Canceled,
    //0x04
    // transfer buffers are mapped
    TX_Mapped,
    //0x05
    TX_Canceled,
    //0x06
    TX_Mapped_Canceled,
    //0x07
    // transfer is in miniport
    TX_Mapped_inMp,
    //0x08
    TX_Mapped_inMp_Canceled,
    //0x09
    TX_HwDone,
    //0x0a
    // error occured before the transfer was mapped or on a transfer that does not require mapping
    TX_Error,
    //0x0b
    // error occured after the transfer was mapped
    TX_Mapped_Error,
    //0x0c
    TX_Mapped_TerminateSplit,
    //0x0d
    TX_Mapped_IoComplete,
    //0x0e
    TX_Mapped_Split,
    //0x0f
    TX_Free,
    //0x10
    TX_Split_c_IoComplete,
    //0x11
    TX_IoComplete


} TX_STATE;

/*
    USB Transfer Events
*/

typedef enum _TX_EVENT {
    //0x00
    // isnserted in cancel safe queue
    Ev_TX_Reserved = 0,
    //0x01
    // error during io_map transfer
    Ev_TX_IoMapError,
     //0x02
    // isoch error during io_map
    Ev_TX_Iomap_Isoch_Error,
     //0x03
    Ev_TX_Iomap_ZeroByte,
    //0x04
    // transfer successfully mapped
    //map+
    Ev_TX_MapTransferSuccess,
    //0x05
    // invoke map_transfer
    //mpSG
    Ev_TX_MapTransfer,
    //0x06
    // miniport completed transfer
    //mpCP
    Ev_TX_MiniportCompleted,
    //0x07
    // submit to miniport success
    Ev_TX_MpSubmitSuccess,
    //0x08
    // submit to miniport failed
    Ev_TX_MpSubmitError,
    //0x09
    // miniport busy
    Ev_TX_MpSubmitBusy,
    //0x0a
    Ev_TX_Done,
    //0x0b
    // transfer completed to function driver (IoCompleteRequest)
    Ev_TX_Free,
    //0x0c
    Ev_TX_CompleteTerminateSplit,
    //0x0d
    Ev_TX_IomapSplitError,
    //0x0e
    Ev_TX_TerminateSplit,
    //0x0f
    Ev_TX_CompleteSplitParent,
    //0x10
    Ev_TX_FreeSplit,
    //0x11
    Ev_TX_CancelPriorityTransfer_Paused,
    //0x12
    Ev_TX_CancelPriorityTransfer,
    //0x13
    Ev_TX_MiniportComplete,
    //0x14
    Ev_TX_MapTransferCanceled,
    //0x15
    Ev_TX_SplitTransfer,
    //0x16
    Ev_TX_MapMpSubmitBusy,
    //0x17
    Ev_TX_Icsq,
    //0x18
    //0x19 submit to miniport failed on split transfer after mapping
    Ev_TX_MapMpSubmitErrorComplete,
    //0x1a
    Ev_TX_MapMpSubmitError,
    //0x1b submit path that skips the complete
    Ev_TX_MpSubmitErrorNC,


} TX_EVENT;


typedef struct _SCATTER_GATHER_ENTRY *PSCATTER_GATHER_ENTRY; // Forward reference


/*

The Transfer Irp States track which of the following lists a Transfer/Irp is on:

List              | List Head                         | List Entry
------------------+-----------------------------------+--------------------------
Priority Queue    | devExt->Fdo.TxPriorityListHead    | transfer->TxPriorityLink
------------------+-----------------------------------+--------------------------
Done Queue        | devExt->Fdo.TxDoneListHead        | transfer->TxDoneLink
------------------+-----------------------------------+--------------------------
Cancel Safe Queue | devExt->TxCsqIrpListHead          | transfer->TxCsqIrpLink
------------------+-----------------------------------+--------------------------
Cancel List       | devExt->Fdo.TxCancelIrpListHead   | transfer->TxCancelIrpLink
------------------+-----------------------------------+--------------------------

The transfer->IrpState is protected by the global TX Lock, i.e.
USBPORT_TxAcquireLock(), AKA USBPORT_AcquireEpListLock(), AKA the global
devExt->Fdo.EndpointListSpin.

The Priority Queue and the Done Queue are mutually exclusive.
A Transfer/Irp will never be on both at the same time.

The Cancel Safe Queue and the Cancel List are mutually exclusive.
A Transfer/Irp will never be on both at the same time.

The Irp State names have no obvious meaning on their own other than
indicating which single list or pair of lists a Transfer/Irp is on.

"TxQueued"  -> Priority Queue
"IoDone"    -> Done Queue
"Pending"   -> Cancel Safe Queue
"Canceled"  -> Cancel List
"Complete"  -> none

NOTE: There is an exception to the meaning of the "IoDone" state.  In
addition to meaning that the Transfer/Irp is actually on the Done Queue,
it can also mean that the Transfer/Irp was removed from the Done Queue
by USBPORT_Core_UsbIocDpc_Worker() and there can still be a thread of
execution in USBPORT_Core_iIrpCsqCompleteDoneTransfer() with a
reference to the Transfer/Irp.


Cancel    | Priority |  Done  | Cancel Safe
List      | Queue    |  Queue | Queue
----------+----------+--------+---------------+----------------------
 no       | no        | no    | no            | Irp_Complete
----------+----------+--------+---------------+----------------------
 no       | no        | no    | YES           | Irp_Pending
----------+----------+--------+---------------+----------------------
 no       | no        | YES   | no            | Irp_IoDone
----------+----------+--------+---------------+----------------------
 no       | no        | YES   | YES           | Irp_IoDone_Pending
----------+----------+--------+---------------+----------------------
 no       | YES       | no    | no            | Irp_TxQueued
----------+----------+--------+---------------+----------------------
 no       | YES       | no    | YES           | Irp_TxQueued_Pending
----------+----------+--------+---------------+----------------------
 no       | YES       | YES   | no            | invalid
----------+----------+--------+---------------+----------------------
 no       | YES       | YES   | YES           | invalid
----------+----------+--------+---------------+----------------------
 YES      | no        | no    | no            | Irp_Canceled
----------+----------+--------+---------------+----------------------
 YES      | no        | no    | YES           | invalid
----------+----------+--------+---------------+----------------------
 YES      | no        | YES   | no            | Irp_IoDone_Canceled
----------+----------+--------+---------------+----------------------
 YES      | no        | YES   | YES           | invalid
----------+----------+--------+---------------+----------------------
 YES      | YES       | no    | no            | Irp_TxQueued_Canceled
----------+----------+--------+---------------+----------------------
 YES      | YES       | no    | YES           | invalid
----------+----------+--------+---------------+----------------------
 YES      | YES       | YES   | no            | invalid
----------+----------+--------+---------------+----------------------
 YES      | YES       | YES   | YES           | invalid
----------+----------+--------+---------------+----------------------

*/

typedef enum _TX_IRP_STATE {
    Irp_Complete = 0,               // (none)
    Irp_Pending = 1,                // (Cancel Safe Queue)
    Irp_IoDone = 2,                 // (Done Queue)
    Irp_IoDone_Pending = 3,         // (Done Queue, Cancel Safe Queue)
    Irp_TxQueued = 4,               // (Priority Queue)
    Irp_TxQueued_Pending = 5,       // (Priority Queue, Cancel Safe Queue)

    Irp_Canceled = 8,               // (Cancel List)

    Irp_IoDone_Canceled = 10,       // (Done Queue, Cancel List)
    Irp_TxQueued_Canceled = 12,     // (Priority Queue, Cance List)

    Irp_Canceled_Complete = 1000,
    Irp_Reserved = 1001
} TX_IRP_STATE;


//typedef struct _TX_STATE_LOG_ENTRY {
//    TX_EVENT Event;
//    TX_STATE CurrentState;
//    TX_STATE NextState;
//    ULONG  Info;
//} TX_STATE_LOG_ENTRY, *PTX_STATE_LOG_ENTRY;

#define TX_STATE_LOG_ENTRY HCD_STATE_LOG_ENTRY
#define NUM_TX_STATE_LOG_ENTRIES 16


//
// general purpose callback context
//
typedef struct _USBPORT_URB_CB_CONTEXT {

    ULONG Sig;
    USB_DEFAULT_PIPE_SETUP_PACKET SetupPacket;

    struct _USBD_DEVICE_HANDLE *DeviceHandle;
    struct _HCD_ENDPOINT *Endpoint;
    struct _USBD_DEVICE_HANDLE *HsHubDeviceHandle;
    struct _TRANSACTION_TRANSLATOR *Tt;

    PKEVENT CommandCompleteEvent;

} USBPORT_URB_CB_CONTEXT, *PUSBPORT_URB_CB_CONTEXT;

typedef
VOID
(*PURB_CALLBACK_ROUTINE) (
    PDEVICE_OBJECT HcFdo,
    PTRANSFER_URB Urb,
    PVOID Context,
    NTSTATUS NtStatus
    );

typedef struct _HCD_TRANSFER_CONTEXT {

    LIST_ENTRY TxCsqIrpLink;
    LIST_ENTRY TxPriorityLink;
    LIST_ENTRY TxCancelIrpLink;

    ULONG Sig;
    TX_TYPE TxType;

    // these values are set before we record the transfer in a minidump
    //
    ULONG OcaSig1;
    USB_CONTROLLER_FLAVOR HcFlavor;
    // contrller info
    USHORT PciVendorId;
    USHORT PciDeviceId;
    UCHAR PciRevisionId;

    //
    // Due to a Bug in NVIDIA Harmony board, certain transfers cause memory corruption when
    // Scatter Gather capable DMA Adapter is used. Refer to Windows 8 Bug: 79552 for details.
    // Thus such transfers need to be double buffered.
    //
    // Please note that DoubleBuffer, DoubleBufferMdl is not related to the DoubleBufferListHead
    //
    BOOLEAN DoubleBufferDueToTegraDmaBug;
    PVOID DoubleBuffer;
    PMDL DoubleBufferMdl;
    PMDL OrigBufferMdl;
    PVOID OrigBuffer;
    ULONG OcaSig2;

    ULONG Flags;

    TX_STATE TxState;
    // Total length of this structure
    ULONG TotalLength;
    // length up to miniport context
    ULONG PrivateLength;

    ENDPOINT_TRANSFER_TYPE EpTransferType;
    USBPORT_TRANSFER_DIRECTION Direction;
    // timeout, 0 = no timeout
    ULONG MillisecondTimeout;
    USB_TIMEOUT Timeout;

    // for perf work
    ULONG MiniportFrameCompleted;
    // track bytes transferred this transfer
    ULONG MiniportBytesTransferred;
    USBD_STATUS UsbdStatus;

    // irp to signal on completion
    PIRP Irp;
    TX_IRP_STATE IrpState;
    // event to signal on completion
    PKEVENT CompleteEvent;

    // point back to the original URB
    PTRANSFER_URB Urb;

    PURB_CALLBACK_ROUTINE UrbCallbackFunc;
    PVOID UrbCallbackContext;

    // The device handle is pinned(reffed) by the URB request at the time it is submitted to USBPORT.
    // The DeviceHandle is embedded in the URB structure and will not be deleted until the URB reference
    // is dropped (when the transfer is completed. Since the URB is not allocated by USBPORT it may not
    // always be valid so we keep a duplicate copy of the devicehandle here.  This copy can be used  to
    // access the device handle and to validate that the URB has not been corrupted.
    PVOID UrbDeviceHandle;

    // for linkage on endpoint lists
    LIST_ENTRY TxEndpointLink;

    // List entry for HCD_ENDPOINT TxMappedNotInMiniportListHead.
    //
    LIST_ENTRY  TxMappedNotInMiniportListEntry;

    // lookaside list for completed transfers
    //USBPORT_Core_UsbIocDpc_Worker
    LIST_ENTRY TxDoneLink;

    // List entry for HCD_ENDPOINT
    // TxCanceledPendingClearTTCompletionListHead or
    // TxDonePendingClearTTCompletionListHead.
    //
    LIST_ENTRY TxClearTTCompletionListEntry;

    //USBPORT_Core_iAbortEndpoint
    // link lookaside list of aborted transfers
    LIST_ENTRY TxAbortLink;
    //owner of the abort
    PUSB_ABORT_PIPE_CONTEXT AbortContext;

    TRANSFER_PARAMETERS Tp;
    PMDL TransferBufferMdl;
    // used for perf
    ULONG IoMapStartFrame;

    // for Double buffering
    LIST_ENTRY DoubleBufferListHead;
    LIST_ENTRY IsoTegraDoubleBufferListHead;

    // parent transfer
    struct _HCD_TRANSFER_CONTEXT *ParentTransfer;
    //struct _HCD_ENDPOINT *Endpoint;
    // reference to endpoint in transfer
    struct _USBOBJ_REF *EpRefHandle;

    PUCHAR MiniportContext;

    // index number of split child
    ULONG ParentIdx;        // this is the Tp.SequenceNumber
    ULONG SplitIdx;
    LIST_ENTRY SplitTransferListHead;
    LIST_ENTRY SplitLink;
    LIST_ENTRY TmpLink;

    PMINIPORT_ISO_TRANSFER IsoTransfer;
    ULONG IsoPeriod;

    // OCA info from device
    USHORT DeviceVID;
    USHORT DevicePID;
    WCHAR DriverName[USB_DRIVER_NAME_LEN];

    ULONG NumScatterGatherLists;

    ULONG NumScatterGatherListsRemaining;

    // These are the scatter-gather entries used by PutDmaAdapter and GetDmaAdapter
    PSCATTER_GATHER_ENTRY   ScatterGatherEntries;

    IO_CSQ_IRP_CONTEXT CsqContext;

    LARGE_INTEGER Queued_TimeStamp;
    LARGE_INTEGER Completed_TimeStamp;

    ULONG LogIdx;
    TX_STATE_LOG_ENTRY Log[NUM_TX_STATE_LOG_ENTRIES];

    PVOID SGBuffer;
    ULONG SGBufferLength;
    TRANSFER_SG_LIST SgList;

} HCD_TRANSFER_CONTEXT, *PHCD_TRANSFER_CONTEXT;

// Check against the current structure size limit hard coded into
// DBGENG.DLL
//
C_ASSERT(sizeof(HCD_TRANSFER_CONTEXT) < USBPORT_MAXIMUM_HCD_TRANSFER_CONTEXT_SIZE);

// DMA scatter / gather data structures allocated as part of a
// HCD_TRANSFER_CONTEXT by USBPORT_AllocTransfer() and used by
// USBPORT_FlushMapTransferList(), USBPORT_ProcessScatterGatherList(),
// and USBPORT_CompleteTransfer().
//
typedef struct _SCATTER_GATHER_ENTRY
{
    // SIG_HAL_SG_LIST
    ULONG Sig;
    // Back pointer to containing HCD_TRANSFER_CONTEXT structure.
    //
    PHCD_TRANSFER_CONTEXT   Transfer;

    // DMA Transfer Context
    //
    UCHAR                   DmaTransferContext[DMA_TRANSFER_CONTEXT_SIZE_V1];

    // Allocated by system call to GetScatterGatherList().
    //
    PSCATTER_GATHER_LIST    ScatterGatherList;

} SCATTER_GATHER_ENTRY, *PSCATTER_GATHER_ENTRY;


/*
    The pipe handle structure us our primary means of
    tracking USB endpoints.  Contained within the handle
    is our endpoint data structure as well as the
    miniport endpoint data structure.
*/

typedef VOID
    (__stdcall *PENDPOINT_WORKER_FUNCTION) (
        struct _HCD_ENDPOINT *
    );

typedef union _USB_ENDPOINT_FLAGS {
    ULONG                  ul;
    struct {

        // The MapTransfers flag when set indicates that the miniport
        // needs a DMA mapped SG list to process each transfer.  The
        // MapTransfers flag is mutally exclusive with the VirtualBus
        // flag.
        //
        ULONG   MapTransfers:1;

        // The RootHub flag is set for the root hub Control and
        // Interrupt endpoints.  RootHub endpoint transfers are not
        // submitted to the miniport.
        //
        ULONG   RootHub:1;

        // The Pure flag when set causes the starting frame number for
        // the next transfer to be reinitialized when the
        // USBD_START_ISO_TRANSFER_ASAP flag is used. (Isochronous
        // endpoints only).
        //
        ULONG   Pure:1;

        // The DeviceGone flag is set for all endpoints for a device
        // when the device handle for the device is removed.  The only
        // code which sets DeviceGone is USBPORT_AbortAllTransfers(),
        // which is only called by USBPORT_RemoveDeviceHandle().
        //
        ULONG   DeviceGone:1;

        // The VirtualBus flag when set indicates that the miniport does
        // not need a DMA mapped SG list to process each transfer.  The
        // VirtualBus flag is mutally exclusive with the MapTransfers
        // flag.
        //
        ULONG   VirtualBus:1;

        // The HwSyncOnSOF flag when set indicates that the endpoint
        // state change synchronization with the miniport is via
        // miniport bus frame number advance.  The HwSyncOnSOF flag is
        // mutually exclusive with the PollForHwSync flag.
        //
        ULONG   HwSyncOnSOF:1;

        // The PollForHwSync flag when set indicates that the endpoint
        // state change synchronization with the miniport is via calling
        // MINIPORT_PollHwSync().  The PollForHwSync flag is mutually
        // exclusive with the HwSyncOnSOF flag.
        //
        ULONG   PollForHwSync:1;

        // The FatIso flag is set for isochronous endpoints with a
        // wMaxPacketSize above a threshold which affects the budget
        // scheduler.
        //
        ULONG   FatIso:1;

        // The USB11_Bandwidth flag is set for USB_MINIPORT_OPT_USB11
        // miniport endpoints for which bandwidth is successfully
        // allocated which must later be freed.
        //
        ULONG   USB11_Bandwidth:1;

        // The USBX_Bandwidth flag is set for USB_MINIPORT_OPT_USBX
        // miniport endpoints for which bandwidth is successfully
        // allocated via calling MINIPORT_UsbxAllocateBandwidth() which
        // must later be freed via calling
        // MINIPORT_UsbxFreeBandwidth().
        //
        // NOTE: No miniports currently set the USB_MINIPORT_OPT_USBX
        // flag so there is code which is not currently exercised here.
        //
        ULONG   USBX_Bandwidth:1;

        // The PipeClosed flag is set by USBPORT_ClosePipe().
        //
        ULONG   PipeClosed:1;

        // The HcReset flag is set by USBPORT_HcResetDpc().
        //
        ULONG   HcReset:1;

        // A non-zero value for the ClearTtBufferOnCancel flag indicates
        // that this endpoint is a Control or Bulk endpoint on a
        // full-speed or low-speed device which is downstream of TT hub.
        // A CLEAR_TT_BUFFER request should be sent to the TT hub when a
        // transfer on this endpoint is canceled or aborted while the
        // request was actively queued in the miniport hardware.
        //
        ULONG   ClearTtBufferOnCancel:1;

        ULONG   Unused:19;
    };

} USB_ENDPOINT_FLAGS, *PUSB_ENDPOINT_FLAGS;

C_ASSERT(sizeof(USB_ENDPOINT_FLAGS) == sizeof(ULONG));

// these states are protected by the RootHubDpcSpin
typedef enum _HC_ROOTHUB_INTEP_STATE {

    RhIntrEp_Init = 0,
    //RIcl
    RhIntrEp_Closed,
    //RIop
    RhIntrEp_Open,
    //RIwt
    RhIntrEp_WaitDpc,

} HC_ROOTHUB_INTEP_STATE;


typedef enum _HC_ROOTHUB_INTEP_EVENT  {

    //riOP
    Ev_Rh_IntrEp_Open = 1,
    //riCL
    Ev_Rh_IntrEp_Close,
    //rhIV
    Ev_Rh_IntrEp_Invalidate,

} HC_ROOTHUB_INTEP_EVENT ;


// these states are protected by the EndpointListSpin
typedef enum _HC_GLOBAL_EP_STATE {

    //GEIN 0
    GEp_Init = 0,
    //GEXX 1
    GEp_NotUsed,
    //GERP 2
    GEp_RemovePending,
    //GECL 3
    GEp_Closed,
    //GEAC 4
    GEp_Active,
    //GEPP 5
    GEp_PausePending,
    //GEPS 6
    GEp_Paused,
    //GERM 7
    GEp_Removed,
    //GEPC 8
    GEp_PausePendingCancel,
    //GEFR 9
    GEp_Free,
    //GEUK 10
    GEp_Unknown,
    //GENU 11
    // replaces the 'zapped' guard (flag) with.  The zapped state indicates that the miniport
    // does not refernce the endpoint. It is used in cases where the endpoint needs to be
    // recovered from hardware failure.
    GEp_Zapped,

    // 12
    GEp_PausedClearTTNeeded,
    // 13
    GEp_PausedClearTTPending,
    // 14
    GEp_ActiveClearTTNeeded,
    // 15
    GEp_ActiveClearTTPending

} HC_GLOBAL_EP_STATE;


typedef enum _HC_GLOBAL_EP_EVENT  {

    //geOP 1
    Ev_gEp_Open = 1,
    //geCL 2
    Ev_gEp_ReqRemove,
    //geRM 3
    Ev_gEp_Close,
    // 4
    Ev_gEp_ReqActive,
    // 5
    Ev_gEp_ReqPause,
    // 6
    Ev_gEp_HwSync,
    // 7
    Ev_gEp_Reset,
    // 8
    // special states for handling iocancelirp
    // endpoint needs to be paused for a canceled transfer, it is resumed after canceled transfers are flushed
    // off the hardware
    Ev_gEp_ReqPauseCancel,
    // 9
    Ev_gEp_ReqActiveCancel,
    // 10
    // indicates a forced hardware sync on power management
    Ev_gEp_PM_HwSync,
    // 11
    // request a resync, activate on enpoint following suspend
    Ev_gEp_PM_ReqResume,
    // 12
    Ev_gEp_PM_ReqSuspend,
    // 13
    // replaces the 'Zapped' guard (flag)
    Ev_gEp_Zap,
    // 14
    Ev_gEp_PollEndpoint,
    // 15
    Ev_gEp_AbortTransfer,
    // 16
    Ev_gEp_SubmitTransfer,
    // 17
    Ev_gEp_Clone,
    // 18
    Ev_gEp_Poke,
    // 19
    Ev_gEp_Rebalance,
    // 20
    Ev_gEp_Reopen,

    // 21
    Ev_gEp_ClearTTNeeded,
    // 22
    Ev_gEp_ClearTTIssued,
    // 23
    Ev_gEp_ClearTTComplete

} HC_GLOBAL_EP_EVENT  ;

typedef struct _EP_LOCK_HANDLE {
    ULONG Tag;
    PVOID EpUnreferenced;
    KIRQL LockIrql;
} EP_LOCK_HANDLE, *PEP_LOCK_HANDLE;

//typedef struct _GEP_STATE_LOG_ENTRY {
//    HC_GLOBAL_EP_EVENT Event;
//    HC_GLOBAL_EP_STATE CurrentState;
//    HC_GLOBAL_EP_STATE NextState;
//    MP_ENDPOINT_STATE  HwEpState;
//} GEP_STATE_LOG_ENTRY, *PGEP_STATE_LOG_ENTRY;

#define GEP_STATE_LOG_ENTRY HCD_STATE_LOG_ENTRY
#if DBG
#define NUM_GEP_STATE_LOG_ENTRIES 16
#else
#define NUM_GEP_STATE_LOG_ENTRIES 8
#endif

#include <PSHPACK1.H>

typedef struct _ETW_USB_ENDPOINT {

    PVOID   Endpoint;
    PVOID   PipeHandle;
    PVOID   DeviceHandle;

} ETW_USB_ENDPOINT, *PETW_USB_ENDPOINT;

#include <POPPACK.H>

// The reason why 512KB is choosen for the max forward transfer size is
// that in doing so we can avoid dealing with transfer splitting
#define USB_MAX_FP_XFER_SIZE   512*1024

#define NOT_IN_USE 0
#define IN_USE 1

#define FP_CTX(x)      ((x)->FpContext)
#define FP_ENABLED(x)  (FP_CTX((x)))

typedef struct _HCD_ENDPOINT_FORWARD_PROGRESS_CONTEXT {
    ULONG MaxForwardProgressTransferSize;

    ULONG MdlState;
    PMDL  Mdl;
    PKTHREAD MdlOwner;

    ULONG TransferContextState;
    ULONG TransferContextSize;
    PVOID TransferContext;
    PKTHREAD TransferContextOwner;

    ULONG SGBufferState;
    PVOID SGBuffer;
    ULONG SGBufferLength;
    PKTHREAD SGBufferOwner;

} HCD_ENDPOINT_FORWARD_PROGRESS_CONTEXT, *PHCD_ENDPOINT_FORWARD_PROGRESS_CONTEXT;

// Structure used for endpoint reference list.
//
// N.B. This is an opague structure. Direct access to the internal
// internal fields is disallowed. Use USBPORT_EndpointFromHandle
// to obtain the endpoint object instead.
typedef struct _USBOBJ_REF
{
     LIST_ENTRY UsbRefLink;
     ULONG Sig;
     struct _HCD_ENDPOINT *Endpoint;
     PVOID Object;
     ULONG Tag;
     PKTHREAD Thread;

} USBOBJ_REF, *PUSBOBJ_REF;

typedef struct _HCD_ENDPOINT_EXTENSION_FOR_TEGRA {

    KSPIN_LOCK IsoDbHandleListLock;
    LIST_ENTRY PhysicallyContiguousIsoDbHandlesForTegra;
    LIST_ENTRY IsoDbHandlesForTegra;

} HCD_ENDPOINT_EXTENSION_FOR_TEGRA, *PHCD_ENDPOINT_EXTENSION_FOR_TEGRA;

typedef struct _HCD_ENDPOINT {

    // for linkage to global endpoint list
    // GlobalEndpointList
    LIST_ENTRY GlobalLink;

    ULONG Sig;

    PHCD_ENDPOINT_EXTENSION_FOR_TEGRA  TegraExtension;

    USB_ENDPOINT_FLAGS EpFlags;

    PDEVICE_OBJECT FdoDeviceObject;
    // for synchronous operations at passive level the current thread is recorded here prior to
    // waiting.
    PVOID EpCurrentThread;

    // we only allow one at a time
    PUSB_ABORT_PIPE_CONTEXT PendingAbortContext;

    struct _TRANSACTION_TRANSLATOR *Tt;

    MP_ENDPOINT_STATUS CurrentStatus;
    MP_ENDPOINT_STATE CurrentEpState;
    MP_ENDPOINT_STATE PendingEpState;
    ULONG StateChangeFrame;

    LONG UsbHcIntDpcGenerationCount;

    BOOLEAN MpOpenRef;
    HC_GLOBAL_EP_STATE GepState;
    KEVENT GepEventPause;
    KEVENT GepEventRemove;
    ULONG GepLogIdx;
    GEP_STATE_LOG_ENTRY GepLog[NUM_GEP_STATE_LOG_ENTRIES];

    // list used to track all transfers queued to the endpoint, not all
    // transfer have Irps associated with them.
    LIST_ENTRY HwPendingListHead;

    // The TxMappedNotInMiniportListHead list tracks all endpoint
    // transfers which have been mapped but not yet successfully
    // submitted to the miniport.  This is essentially transfers which
    // are in the TX_Mapped state and not in the TX_Mapped_inMp state.
    //
    // A transfer for an endpoint should not not be submitted to the
    // miniport unless this list is empty or the transfer is the first
    // transfer on this list to prevent out of order transfer submission
    // to the miniport.
    //
    LIST_ENTRY  TxMappedNotInMiniportListHead;

    // reported max_transfer
    ULONG UsbMaxControllerTransfer;

    // used by USBPORT_ProcessNeoStateChangeList for linkage on the hw state change list
    LIST_ENTRY StateLink;

    // used by USBPORT_Core_FlushController tmp list
    LIST_ENTRY FlushHcLink;

    // used by USB2 lib code
    LIST_ENTRY RebalanceLink;

    // List of transfers on this endpoint which have been canceled or
    // aborted while actively queued in the miniport and then completed
    // from a hardware point of view, but which are waiting for the
    // completion of a CLEAR_TT_BUFFER request before the transfers are
    // finally completed from a software point of view.  Transfers
    // found on this list by USBPORT_Core_UsbIocDpc_Worker() will be
    // removed from this list and the TxDoneListHead and inserted into
    // the TxDonePendingClearTTCompletionListHead.
    //
    LIST_ENTRY TxCanceledPendingClearTTCompletionListHead;

    // List of transfers on this endpoint which are waiting for a
    // CLEAR_TT_BUFFER request to complete.  When the CLEAR_TT_BUFFER
    // request completes all transfers on this list will be removed and
    // inserted back into the TxDoneListHead, where USBPORT_Core_UsbIocDpc_Worker()
    // will pick them up and process their final completion.
    //
    LIST_ENTRY TxDonePendingClearTTCompletionListHead;

    // iso stuff
    ULONG NextTransferStartFrame;

    PUSBPORT_COMMON_BUFFER CommonBuffer;
    ENDPOINT_PARAMETERS Parameters;

    // pointer to High speed budget context
    PVOID HsbEpContextPtr;
    // used to stall close endpoint when we may still need access

    HC_ROOTHUB_INTEP_STATE RhEpState;
    LIST_ENTRY EpRefListHead;
    KEVENT EpClosedEvent;
    // resource-less ref count
    ULONG EpRefCountNoRes;

    //USBPORT_OpenEndpointRefList
    struct _USBOBJ_REF *OpenEpRefHandle;

    struct _USBOBJ_REF *EpUsb2Handle;
    //USBPORT_iEndpointInsertStateList
    struct _USBOBJ_REF *EpStateHandle;

    //USBPORT_Core_FlushController
    struct _USBOBJ_REF *EpFlushHcHandle;

    struct _HCD_ENDPOINT *BudgetNextEndpoint;

    ULONG LastTransferSequenceNumberAccepted;

    ETW_USB_ENDPOINT EtwUsbEndpoint;
    USB_ENDPOINT_DESCRIPTOR EndpointDescriptor;

    //Backup reference for forward progress support
    USBOBJ_REF  EpRefFP;

    // forward progress context
    PHCD_ENDPOINT_FORWARD_PROGRESS_CONTEXT FpContext;

    ULONG MiniportEndpointDataSize;
    PVOID MiniportEndpointData[0];  // PVOID for IA64 alignment

} HCD_ENDPOINT, *PHCD_ENDPOINT;

// Check against the current structure size limit hard coded into
// DBGENG.DLL
//
C_ASSERT(sizeof(HCD_ENDPOINT) < USBPORT_MAXIMUM_HCD_ENDPONT_SIZE);

__inline
PMDL
FPAllocMdlEP(
    __in PHCD_ENDPOINT Endpoint,
    __in ULONG TransferLength
    )
{
    if (FP_ENABLED(Endpoint) &&
        TransferLength<=FP_CTX(Endpoint)->MaxForwardProgressTransferSize&&
        !InterlockedCompareExchange(&FP_CTX(Endpoint)->MdlState,IN_USE,NOT_IN_USE)) {
        FP_CTX(Endpoint)->MdlOwner=KeGetCurrentThread();
        return FP_CTX(Endpoint)->Mdl;
    } else {
        return NULL;
    }
}

__inline
BOOLEAN
FPFreeMdlEP(
    __in PHCD_ENDPOINT Endpoint,
    __in PMDL Mdl
    )
{
    if (FP_ENABLED(Endpoint) && FP_CTX(Endpoint)->Mdl==Mdl) {
        NT_ASSERT(FP_CTX(Endpoint)->MdlState);
        FP_CTX(Endpoint)->MdlState = NOT_IN_USE;
        return TRUE;
    } else {
        return FALSE;
    }
}

__inline
PVOID
FPAllocTransferContextEP(
    __in PHCD_ENDPOINT Endpoint,
    __in ULONG TransferContextSize
    )
{
    if (FP_ENABLED(Endpoint)&&
        TransferContextSize<=FP_CTX(Endpoint)->TransferContextSize&&
        !InterlockedCompareExchange(&FP_CTX(Endpoint)->TransferContextState,IN_USE,NOT_IN_USE)) {
        FP_CTX(Endpoint)->TransferContextOwner=KeGetCurrentThread();
        RtlZeroMemory(FP_CTX(Endpoint)->TransferContext,FP_CTX(Endpoint)->TransferContextSize);
        return FP_CTX(Endpoint)->TransferContext;
    } else {
        return NULL;
    }
}

__inline
BOOLEAN
FPFreeTransferContextEP(
    __in PHCD_ENDPOINT Endpoint,
    __in PVOID TransferContext
    )
{
    if (FP_ENABLED(Endpoint)&&
        FP_CTX(Endpoint)->TransferContext==TransferContext) {
        NT_ASSERT(FP_CTX(Endpoint)->TransferContextState);
        FP_CTX(Endpoint)->TransferContextState=NOT_IN_USE;
        return TRUE;
    } else {
        return FALSE;
    }
}

__inline
PVOID
FPAllocSgBufEP(
    __in PHCD_ENDPOINT Endpoint,
    __in ULONG TransferLength,
    __in PULONG AllocationLength
    )
{
    if (FP_ENABLED(Endpoint)&&
        TransferLength<=FP_CTX(Endpoint)->MaxForwardProgressTransferSize&&
        !InterlockedCompareExchange(&FP_CTX(Endpoint)->SGBufferState,IN_USE,NOT_IN_USE)) {
        FP_CTX(Endpoint)->SGBufferOwner=KeGetCurrentThread();
        (*(AllocationLength))=FP_CTX(Endpoint)->SGBufferLength;
        return FP_CTX(Endpoint)->SGBuffer;
    } else {
        return NULL;
    }
}

__inline
BOOLEAN
FPFreeSgBufEP(
    __in PHCD_ENDPOINT Endpoint,
    __in PVOID SGBuffer
    )
{
    if (FP_ENABLED(Endpoint)&&
        (ULONG_PTR)FP_CTX(Endpoint)->SGBuffer==(ULONG_PTR)SGBuffer) {
        NT_ASSERT(FP_CTX(Endpoint)->SGBufferState);
        FP_CTX(Endpoint)->SGBufferState=NOT_IN_USE;
        return TRUE;
    } else {
        return FALSE;
    }
}


// Check against the current structure size limit hard coded into
// DBGENG.DLL
//
C_ASSERT(sizeof(USBOBJ_REF) < USBPORT_MAXIMUM_USBOBJ_REF_SIZE);

// Same as above, but with an additional LIST_ENTRY to allow references
// to be linked together on a user supplied list in addition to being
// linked onto the reference list of the object being referenced.
//
// See USBPORT_ReferenceAllDeviceEndpoints().
//
typedef struct _USBOBJ_REF_EX
{
    USBOBJ_REF;     // Unnamed.  MUST BE FIRST.

    LIST_ENTRY      ExRefLink;

} USBOBJ_REF_EX, *PUSBOBJ_REF_EX;


#ifndef KDEXTS

typedef union _USB_HUB_CLEAR_TT_BUFFER_VALUE {

    USHORT                  us;
    struct {
        USHORT   EndpointNumber:4;     // 0 - 3
        USHORT   DeviceAddress:7;      // 4 - 10
        USHORT   EndpointType:2;       // 11 - 12
        USHORT   Reserved_MBZ:2;       // 13-14

        USHORT   Direction_1forIN:1;   // 15
    };

} USB_HUB_CLEAR_TT_BUFFER_VALUE, *PUSB_HUB_CLEAR_TT_BUFFER_VALUE;

#endif

typedef enum _TT_STATE {
    TT_Init = 0,
    TT_Removed,
    TT_Deleted,
} TT_STATE;

#define TTFLAGS_RESET_TT_ON_CANCEL              1
#define TTFLAGS_NO_CLEAR_TT_BUFFER_ON_CANCEL    2

typedef struct _TRANSACTION_TRANSLATOR {

    ULONG Sig;

    TT_STATE TtState;
    USHORT DeviceAddress;
    //Not to be confused with the actual port number.  This is the value that is used in hub commands like CLEAR_TT_BUFFER.
    //For a single TT hub this must be 1, for multi TT it is the physical port associated with the TT.
    //NOTE: This is not the same as TtPortNumber in the USBD_DEVICE_HANDLE or ENDPOINT_PARAMETERS which is always the
    //value of the physical port associated with the TT.
    USHORT TT_Port;


    LIST_ENTRY TtLink;
    LIST_ENTRY TtGlobalLink;
    LIST_ENTRY RefListHead;

    PVOID NotUsed;

    // each translator is a virtual 1.1 bus, this value represents
    // the total bandwith available on the virtual bus, ~10 Mbits
    ULONG TotalBusBandwidth;
    // this table represents bandwidth allocations on the virtual bus.
    //jd
    //ULONG BandwidthTable[USBPORT_MAX_INTEP_POLLING_INTERVAL];

    ULONG MaxAllocedBw;
    ULONG MinAllocedBw;

    ULONG TtFlags;

    PVOID HsbTtContextPtr;
    //PVOID Usb2LibTtContext[0];  // PVOID for IA64 alignment

} TRANSACTION_TRANSLATOR, *PTRANSACTION_TRANSLATOR;


typedef struct _TT_REF_OBJ {

    ULONG Sig;
    ULONG Tag;
    PVOID Tt;
    PVOID Object;
    LIST_ENTRY RefLink;

} TT_REF_OBJ, *PTT_REF_OBJ;


#define EP_MAX_TRANSFER(ep) ((ep)->Parameters.MaxTransferSize)
#define EP_MAX_PACKET(ep) ((ep)->Parameters.MaxPacketSize)


#define USBPORT_PIPE_STATE_CLOSED  0x00000001
#define USBPORT_PIPE_ZERO_BW       0x00000002

#ifdef WIN64
#define USB_ZERO_BW_ENDP_HANDLE     ((PVOID) (0xFFFFFFFF0E0E0E0E))
#else
#define USB_ZERO_BW_ENDP_HANDLE     ((PVOID) ((0x0E0E0E0E)))
#endif

typedef struct _USBD_PIPE_HANDLE_I {

    ULONG Sig;
    USB_ENDPOINT_DESCRIPTOR EndpointDescriptor;

    ULONG PipeStateFlags;

    ULONG UsbdPipeFlags;

    PUSBOBJ_REF EpHandle;

    // for pipe handle list attached to device
    LIST_ENTRY ListEntry;

} USBD_PIPE_HANDLE_I, *PUSBD_PIPE_HANDLE_I;

#define INITIALIZE_DEFAULT_PIPE(dp, mp) \
    do {\
    (dp).UsbdPipeFlags = 0;\
    (dp).EndpointDescriptor.bLength =\
            sizeof(USB_ENDPOINT_DESCRIPTOR);\
    (dp).EndpointDescriptor.bDescriptorType =\
            USB_ENDPOINT_DESCRIPTOR_TYPE;\
    (dp).EndpointDescriptor.bEndpointAddress =\
            USB_DEFAULT_ENDPOINT_ADDRESS;\
    (dp).EndpointDescriptor.bmAttributes =\
            USB_ENDPOINT_TYPE_CONTROL;\
    (dp).EndpointDescriptor.wMaxPacketSize =\
            mp;\
    (dp).EndpointDescriptor.bInterval = 0;\
    (dp).Sig = SIG_PIPE_HANDLE;\
    (dp).PipeStateFlags = USBPORT_PIPE_STATE_CLOSED;\
    } WHILE (0)


typedef struct _USBD_INTERFACE_HANDLE_I {
    ULONG Sig;
    LIST_ENTRY InterfaceLink;
    BOOLEAN HasAlternateSettings;
    // number associated with this interface defined
    // in the interface descriptor
    UCHAR Pad[3];
    // copy of interface descriptor (header) ie no endpoints
    // the endpoint descriptors are in the PipeHandles
    USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    // array of pipe handle structures
    USBD_PIPE_HANDLE_I PipeHandle[0];
} USBD_INTERFACE_HANDLE_I, *PUSBD_INTERFACE_HANDLE_I;


typedef struct _USBD_CONFIG_HANDLE {
    ULONG Sig;
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor;
    LIST_ENTRY InterfaceHandleList;
} USBD_CONFIG_HANDLE, *PUSBD_CONFIG_HANDLE;

typedef struct _USBD_PERFORMANCE_DATA {

    // Performance Counters

    ULONG StatBulkDataBytes;
    ULONG StatNumberBulkUrbs;
    ULONG StatIsoDataBytes;
    ULONG StatNumberIsoUrbs;
    ULONG StatInterruptDataBytes;
    ULONG StatNumberInterruptUrbs;
    ULONG StatControlDataBytes;
    ULONG StatNumberControlUrbs;
    ULONG TransferErrors;
    ULONG DroppedIsoPackets;
    ULONG TotalIsoLatency;

} USBD_PERFORMANCE_DATA, *PUSBD_PERFORMANCE_DATA;

#define TEST_DEVICE_FLAG(dh, flag) ((dh)->DeviceFlags & (flag)) ? TRUE : FALSE
#define SET_DEVICE_FLAG(dh, flag) ((dh)->DeviceFlags |= (flag))
#define CLEAR_DEVICE_FLAG(dh, flag) ((dh)->DeviceFlags &= ~(flag))

// values for DeviceFlags
#define USBPORT_DEVICEFLAG_ROOTHUB                              0x00000002
#define USBPORT_DEVICEFLAG_HSHUB                                0x00000010
#define USBPORT_DEVICEFLAG_HSHUB_RESET_TT_ON_CANCEL             0x00000020
#define USBPORT_DEVICEFLAG_HSHUB_NO_CLEAR_TT_BUFFER_ON_CANCEL   0x00000040
#define USBPORT_DEVICEFLAG_SUPPORTS_MSOS20_DESCRIPTOR           0x00000080
#define USBPORT_DEVICEFLAG_ORPHANED                             0x00000100

#define MSOS20_DESCRIPTOR_INDEX 0x07

#define IS_ROOT_HUB(dh) (BOOLEAN)((dh)->DeviceFlags & USBPORT_DEVICEFLAG_ROOTHUB)

#ifndef KDEXTS
#define InvalidDeviceHandle ((PVOID)(-1))
#endif

/*
USBD Device Handles

States:
-------
Created - initial state
Enabled_Addr0 - device is enabled at address 0
Enabled - device is addressed, may not be configured.
Disabled - handle is disabled, structures remain but the device cannot be
            accessed by a function driver.  This is used by restore_device.
Removed - device hasn been removed (no longer physically present)
Deleted - device handle has been deleted, last ref removed
...
Idle

Events:
-------
CreateDevice_Fail
CreateDevice_Success

InitializeDevice_Fail
InitializeDevice_Success

RestoreDevice_Fail
RemoveDevice_Success


*/
typedef enum _USBD_DEVH_STATE {
    //bit 1
    Devh_Created            = 1,
    // bit2
    Devh_Enabled_Addr0      = 2,
    // bit 3
    Devh_Valid              = 4,
    // bit 4
    Devh_Removed            = 8,
    // bit 5
    Devh_DeletePending      = 16,
    // bit 6
    Devh_Deleted            = 32
} USBD_DEVH_STATE;


typedef enum _USBD_DEVH_EVENT {
    // USBPORT_NeoCreateDevice
    Ev_CreateDevice_Fail           =1,
    Ev_CreateDevice_Success,
    // USBPORT_InitializeDevice
    Ev_InitializeDevice_Fail,
    Ev_InitializeDevice_Success,
    // USBPORTBUSIF_RestoreUsbDevice
    Ev_RestoreDevice_Fail,
    // USBPORT_CloneDevice
    Ev_Clone,
    Ev_RestoreDevice_Success,
    // USBPORT_NeoRemoveDevice
    Ev_Remove,
    Ev_RemoveDone,
    // USBPORT_DereferenceDeviceHandle
    Ev_Delete,
    // USBPORT_RootHub_RemoveDevice
    Ev_RhRemoved,
    // USBPORT_CreateRootHubPdo
    Ev_CreateRoothub_Fail,
    Ev_CreateRoothub_Success,

    // Marking device handles as Devh_Removed due to stopping the host
    // controller.
    //
    Ev_RemoveControllerStopping

} USBD_DEVH_EVENT;


/*
STATE                   EVENT                               NEXT_STATE
.............................................................................
Devh_Created            (insert devh list)
                        Ev_CreateDevice_Fail            Devh_DeletePending ; rem,insert deleted list
                        Ev_CreateDevice_Success         Devh_Enabled_Addr0
                        Ev_InitializeDevice_Fail        -
                        Ev_InitializeDevice_Success     -
                        Ev_RestoreDevice_Fail           Devh_DeletePending ; rem,insert deleted list
                        Ev_Clone                        -
                        Ev_RestoreDevice_Success        -
                        Ev_Remove                       -
                        Ev_RemoveDone                   -
                        Ev_Delete                       -
                        Ev_RhRemoved                    -
                        Ev_CreateRoothub_Fail           Devh_DeletePending ; rem, insert deleted list
                        Ev_CreateRoothub_Success        Devh_Valid
.............................................................................
Devh_Enabled_Addr0
                        Ev_CreateDevice_Fail            -
                        Ev_CreateDevice_Success         -
                        Ev_InitializeDevice_Fail        Devh_DeletePending  ; rem, insert deleted list
                        Ev_InitializeDevice_Success     Devh_Valid
                        Ev_RestoreDevice_Fail           -
                        Ev_Clone                        -
                        Ev_RestoreDevice_Success        -
                        Ev_Remove                       Devh_Removed
                        Ev_RemoveDone                   -
                        Ev_Delete                       -
                        Ev_RhRemoved                    -
                        Ev_CreateRoothub_Fail           -
                        Ev_CreateRoothub_Success        -
.............................................................................
Devh_Valid
                        Ev_CreateDevice_Fail            -
                        Ev_CreateDevice_Success         -
                        Ev_InitializeDevice_Fail        -
                        Ev_InitializeDevice_Success     -
                        Ev_RestoreDevice_Fail           -
                        Ev_Clone                        Devh_Removed
                        Ev_RestoreDevice_Success        -
                        Ev_Remove                       Devh_Removed
                        Ev_RemoveDone                   -
                        Ev_Delete                       -
                        Ev_RhRemoved                    -
                        Ev_CreateRoothub_Fail           -
                        Ev_CreateRoothub_Success        -
.............................................................................
Devh_Removed
                        Ev_CreateDevice_Fail
                        Ev_CreateDevice_Success
                        Ev_InitializeDevice_Fail
                        Ev_InitializeDevice_Success
                        Ev_RestoreDevice_Fail
                        Ev_Clone
                        Ev_RestoreDevice_Success        Devh_DeletePending
                        Ev_Remove
                        Ev_RemoveDone                   Devh_DeletePending  ; rem, insert deleted list
                        Ev_Delete
                        Ev_RhRemoved
                        Ev_CreateRoothub_Fail
                        Ev_CreateRoothub_Success
.............................................................................
Devh_DeletePending
                        Ev_CreateDevice_Fail            -
                        Ev_CreateDevice_Success         -
                        Ev_InitializeDevice_Fail        -
                        Ev_InitializeDevice_Success     -
                        Ev_RestoreDevice_Fail           -
                        Ev_Clone                        -
                        Ev_RestoreDevice_Success        -
                        Ev_Remove                       -
                        Ev_RemoveDone                   -
                        Ev_Delete                       Devh_Deleted        ; remove deleted list
                        Ev_RhRemoved                    -
                        Ev_CreateRoothub_Fail           -
                        Ev_CreateRoothub_Success        -




*/

// DEVH Tags

#define DEVH_INIT_TAG                '+hvd'  //dvh+
#define DEVH_HSHUB_TAG               'bhHS'  //HShb
#define DEVH_URB_TAG                 'TbrU'  //UrbT
#define DEVH_SEND_CMD_TAG            'dmcS'  //Scmd
#define DEVH_SEND_CMDEX_TAG          'emcS'  //Scme
#define DEVH_ABORT_PIPE_TAG          'PrbA'  //AbrP
#define DEVH_CREATE_DEV_TAG          'Derc'  //creD
// used with IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX
#define DEVH_RETURN_DH_TAG           'HDtr'  //rtDH
// when we link a bus context to a handle
#define DEVH_BUS_CONTEXT_TAG         'TCsb'  //bsCT
#define DEVH_RH_IDLE_TAG             'DIhr'  //rhID
#define DEVH_RH_CA_IDLE_TAG          'DIca'  //caID
#define DEVH_CLEAR_TT_BUFFER_TAG     'BCtt'  //ttCB
#define DEVH_HSHUB_CLEAR_TT_BUFFER_TAG    '2Ctt'  //ttC2
#define DEVH_BUSIF_GDPI_TAG          'IPDG'  //GDPI
#define DEVH_BUSIF_BQDI_TAG          'IDQB'  //BQDI
#define DEVH_GBIU_TAG                'UIBG'  //GBIU

// TT Tags
#define TT_INIT_TAG                'TTni'  //inTT
#define TT_DEVH_TAG                'TTvd'  //dvTT
#define TT_ENDPOINT_TAG            'TTpe'  //epTT
#define TT_BUSCONTEXT_TAG          'TTsb'  //bsTT
#define TT_CLEAR_TT_BUFFER_TAG     'TTlc'  //clTT

// EPREF Tags
//USBPORT_Core_AllocTransfer
#define EPREF_TRANSFER_TAG         'rtPE'  //EPtr
//USBPORT_SyncResetPipeAndClearStall
#define EPREF_RESET_PIPE_TAG       'prPE'  //EPrp
//USBPORT_OpenEndpointRefList
#define EPREF_OPEN_TAG             'poPE'  //EPop
#define EPREF_RH_INTERRUPT_TAG     'riPE'  //EPir
//USBPORT_OpenEndpoint
#define EPREF_PIPE_TAG             'ipPE'  //EPpi
#define EPREF_RH_DPC_TAG           'hrPE'  //EPrh
//USBPORT_AllocateBandwidthUSB20
#define EPREF_USB2TAG              '2uPE'  //EPu2
//USBPORT_AbortPipe
#define EPREF_ABORT_PIPE_TAG       'paPE'  //EPap
// the endpoint is on the 'state list'
#define EPREF_STATE_TAG            'tsPE'  //EPst
//USBPORT_ProcessScatterGatherList
#define EPREF_MAP_TAG              'pmPE'  //EPmp
//USBPORT_FlushController
#define EPREF_FLUSH_HC_TAG         'cfPE'  //EPfc
//USBPORT_Core_SyncAbortEndpoint
#define EPREF_SYNC_ABORT_PIPE_TAG  'PAPE'  //EPAP
//USBPORT_Core_AllocateAbortRequest
#define EPREF_SYNC_ABORT_CTX_TAG   'xRBA'  //ABRx
//USBPORT_AllocateForwardProgressContext
#define EPREF_FORWARD_PROGRESS_TAG 'PgwF'  //FwPg

//typedef struct _DEVH_STATE_LOG_ENTRY {
//    USBD_DEVH_EVENT Event;
//    USBD_DEVH_STATE CurrentState;
//    USBD_DEVH_STATE NextState;
//    ULONG Tag;
//} DEVH_STATE_LOG_ENTRY, *PDEVH_STATE_LOG_ENTRY;

#define DEVH_STATE_LOG_ENTRY HCD_STATE_LOG_ENTRY
#define NUM_DEVH_STATE_LOG_ENTRIES 8


typedef struct _DEVH_REF_OBJ {

    ULONG Sig;
    ULONG Tag;
    PVOID DeviceHandle;
    PVOID Object;
    PIRP Irp;
    LIST_ENTRY RefLink;
    BOOLEAN PendingFlag;

} DEVH_REF_OBJ, *PDEVH_REF_OBJ;

// Same as above, but with an additional LIST_ENTRY to allow references
// to be linked together on a user supplied list in addition to being
// linked onto the reference list of the object being referenced.
//
// See USBPORT_ReferenceAllDeviceHandles().
//
typedef struct _DEVH_REF_OBJ_EX {

    DEVH_REF_OBJ;   // Unnamed.  MUST BE FIRST.

    LIST_ENTRY      ExRefLink;

} DEVH_REF_OBJ_EX, *PDEVH_REF_OBJ_EX;


#include <PSHPACK1.H>

typedef struct _ETW_USB_DEVICE {

    PVOID               DeviceHandle;
    USHORT              idVendor;
    USHORT              idProduct;
    USB_PORT_PATH       PortPath;
    USB_DEVICE_SPEED    DeviceSpeed;
    ULONG               DeviceAddress;

} ETW_USB_DEVICE, *PETW_USB_DEVICE;

#include <POPPACK.H>

typedef struct _USBD_DEVICE_HANDLE {
    ULONG Sig;
     // USB address assigned to the device
    USHORT DeviceAddress;
    USHORT TtPortNumber;

    PDEVICE_OBJECT FdoDevObj;

    ULONG IdleReadyState;

    USBD_DEVH_STATE DhState;
    // if present this event will be signaled when the handle pending count goes
    // to -1 indicating all outstanding requests on the handle are complete.
    KEVENT RemoveDevhEvent;

    // This is a count of refs tha will block USBPORT_RemoveDevice, basically anything
    // that access the endoints or otherwise releies on the initialized handle to
    // function.
    LONG PendingCount;

    // pointer to the TT itself this TT is refernced by this handle so the parent cannot be
    // reomved before this device
    // TT_DEVH_TAG
    struct _TRANSACTION_TRANSLATOR *TtHandle;
    // device handle for the high speed hub that has the TT for this device
    // this will be NULL if no TT is involved.
    // NOTE: this is not necessarily the direct parent.
    // DEVH_HSHUB_TAG
    struct _USBD_DEVICE_HANDLE *HsHubDeviceHandle;

    PUSBD_CONFIG_HANDLE ConfigurationHandle;

    USBD_PIPE_HANDLE_I DefaultPipe;
    USB_DEVICE_SPEED DeviceSpeed;

    // a copy of the USB device descriptor
    USB_DEVICE_DESCRIPTOR DeviceDescriptor;

    ULONG DeviceFlags;

    // used to created a list of valid device handles
    LIST_ENTRY DevhLink;

    // keep a list of valid open pipes
    LIST_ENTRY PipeHandleList;

    ULONG TtCount;
    // keep a list of tt structures for high speed hubs
    LIST_ENTRY TtListHead;
    LIST_ENTRY RefListHead;
    ULONG      RefCountNoRes;

    PVOID  DevicePdo_Unreferenced;
    WCHAR DriverName[USB_DRIVER_NAME_LEN];

    USBD_PERFORMANCE_DATA PerformanceData;

    ULONG LogIdx;
    DEVH_STATE_LOG_ENTRY Log[NUM_DEVH_STATE_LOG_ENTRIES];

    PVOID MiniportContext;

    UNICODE_STRING  PhysicalDeviceObjectName;

    ETW_USB_DEVICE  EtwUsbDevice;

    UCHAR MsOs20VendorCode;

    USB_DEVICE_DESCRIPTOR OriginalDeviceDescriptor;

} USBD_DEVICE_HANDLE, *PUSBD_DEVICE_HANDLE;

// Check against the current structure size limit hard coded into
// DBGENG.DLL
//
C_ASSERT(sizeof(USBD_DEVICE_HANDLE) < USBPORT_MAXIMUM_USBD_DEVICE_HANDLE_SIZE);

// structure containing pointers to be recorded by the next bugcheck
typedef struct _USBPORT_MINIDUMP_DATA {

    // data depends on the bugcheck, these may be NULL if this is a random
    // bugcheck ie one we did not throw ourselves
    PVOID DataBuffer;
    ULONG DataLength;

} USBPORT_MINIDUMP_DATA, *PUSBPORT_MINIDUMP_DATA;


// we serialize access to the device configuration thru a semaphore. This gives us exclusive access to the bus
// during a configuration operation.
// This is a global semaphore for the bus and will effectively serialize configuration operations from all
// client drivers.
//
// jd
// Note: This lock was apparently being used to protect the device handle list but is unclear exactly why.  I
// have removed this part.  The semaphore is only used to protect the configurations now -- ie pipe handles.
//
// It is used in these cases:
// 1. while the config is being changed
// 2. while an IOCTL API is accessing the pipe handles to return info about a config




#define LOCK_DEVICE_CONFIGS(dh, fdo) \
        { \
            PDEVICE_EXTENSION devExt;\
            GET_DEVICE_EXT(devExt, (fdo)); \
            LOGENTRY((fdo), LOG_PNP, 'LKdv', (dh), 0, 0);\
            KeWaitForSingleObject(&(devExt)->Fdo.DeviceCfgLock, \
                                  Executive,\
                                  KernelMode, \
                                  FALSE, \
                                  NULL); \
         }

#define UNLOCK_DEVICE_CONFIGS(dh, fdo) \
        { \
            PDEVICE_EXTENSION devExt;\
            GET_DEVICE_EXT(devExt, (fdo)); \
            LOGENTRY((fdo), LOG_PNP, 'UKdv', (dh), 0, 0);\
            KeReleaseSemaphore(&(devExt)->Fdo.DeviceCfgLock,\
                               LOW_REALTIME_PRIORITY,\
                               1,\
                               FALSE);\
         }



#define USBPORT_BAD_HANDLE ((PVOID)(-1))

// PnPStateFlags
//#define USBPORT_PNP_NOT_STARTED         0x00000001
//#define USBPORT_PNP_STARTED             0x00000002
//#define USBPORT_PNP_STOPPED             0x00000004
//#define USBPORT_PNP_REMOVED             0x00000008
//#define USBPORT_PNP_START_FAILED        0x00000010
//#define USBPORT_PNP_SURPRISE_REMOVED    0x00000020
//#define USBPORT_PNP_FAKE_REMOVED        0x00000040
//#define USBPORT_PNP_DELETED             0x00000080
//#define USBPORT_PNP_PRESENT             0x00000100
//#define USBPORT_PNP_REPORTED_MISSING    0x00000200

// Flags:both FDO and PDO
//#define USBPORT_FLAG_IFACE_REGISTER     0x00000001
//#define USBPORT_FLAG_IFACE_ENABLE       0x00000002

// Valid states for the root hub PDO, these replace the PnPStateFlags flags

#define USBPORT_GetPnpState(ext) ((ext)->PnpState)


typedef enum _USBPORT_PNP_STATE {

    PnpNotStarted,
    PnpStarted,
    PnpStartFailed,
    PnpStopped,
    PnpReportedMissing,
    PnpSurpriseRemoved,
    PnpSoftRemoved,
    PnpRemoved,
    PnpDeleted,

} USBPORT_PNP_STATE;


// PnpNotStarted - initial state when created.
// PnpStarted -
// PnpReportedMissing - pnp knows the PDO is gone -- either implicitly or explicitly thru QBR

typedef enum _USBPORT_PNP_EVENT {
    // PDO events
    EvPDO_IRP_MN_SURPRISE_REMOVAL,
    EvPDO_IRP_MN_REMOVE_DEVICE,
    EvPDO_IRP_MN_STOP_DEVICE,
    EvPDO_IRP_MN_START_DEVICE,

    // FDO events
    EvFDO_IRP_MN_SURPRISE_REMOVAL = 100,
    EvFDO_IRP_MN_REMOVE_DEVICE,
    EvFDO_IRP_MN_STOP_DEVICE,
    EvFDO_IRP_MN_START_DEVICE,
    EvFDO_IRP_MN_QBR_RH_ERROR,
    EvFDO_IRP_MN_QBR_RH,
    EvFDO_IRP_MN_RE_START_DEVICE
} USBPORT_PNP_EVENT;

//IoRegisterDeviceInterface(SymbolicLinkName)
//IoSetDeviceInterfaceState(SymbolicLinkName)
//RtlFreeUnicodeString(SymbolicLinkName)

/*
state               event                                           Next State
-----------------------------------------------------------------------------------
IfNotRegistered     (REGISTER)IoRegisterDeviceInterface             IfRegistered

IfRegistered        (ENABLE)IoSetDeviceInterfaceState(TRUE)         IfEnabled
IfRegistered        (DEREGISTER)RtlFreeUnicodeString                IfNotRegistered

IfEnabled           (DISABLE)IoSetDeviceInterfaceState(FALSE)       IfRegistered
*/

typedef enum _USBPORT_INTERFACE_STATE {
    IfNotRegistered,
    IfRegistered,
    IfEnabled
} USBPORT_INTERFACE_STATE;

typedef enum _DM_TIMER_STATE {
    DmTimer_Uninit = 0,
    DmTimer_Init,
    DmTimer_Enabled,
    DmTimer_DisablePending,
    DmTimer_Disabled,
    DmTimer_Suspended
} DM_TIMER_STATE;


// these are protected by the dmtimer spinlock
// Policy : set new values under a spin lock
//        : get multiple values under a spin lock
//        : get single value without a spin lock
// the dmtimer spin lock is used to synchronize start and stop firing of DPCs
typedef union _DM_TIMER_FLAGS {
    ULONG                  ul;
    struct {

// set to disable the DM timers work
// while controller is in low power
        ULONG   SkipTimerWork:1;
// set when HC is not in D0 or the miniport is
// not initialized
        ULONG   DecodesOff:1;
// causes us to fail Urbs sent to us when the controller
// is in low power or otherwise unavailable
        ULONG   FailUrbs:1;

        ULONG   WakeOnInterruptUnused:1;

// used to keep from scheduling >1 Idle CB worker at a time
        ULONG   IdleCbPending:1;
        ULONG   Reserved:27;
    };
} DM_TIMER_FLAGS, *PDM_TIMER_FLAGS;

// power FDO flags
typedef union _POWER_FLAGS {
    ULONG                  ul;
    struct {
        ULONG   BusArmedForWake:1;

        // **NOTE: The following two flags (Off and Suspended) are
        // Mutually Exclusive
        //
        ULONG   Off:1;

        // Since the true power state of the HW must remain independent
        // of OS power management we have our own flags for this.  This
        // flag is set to indicate the controller is 'suspended'.
        //
        ULONG   Suspended:1;

        ULONG   HcIsWakeupSource:1;
        ULONG   HcIsSystemWakeSource:1;

        ULONG   HcResumeFailure:1;

        ULONG   MpRestartFailed:1;

        // 1 = rh S4 state was mapped to s4, the hub driver and devices
        // will think they can wakeup from S4. This is ONLY set if we
        // change the the S4 state from its true reported value.
        //
        ULONG   RhS3mappedToS4:1;

        ULONG   ForceControllerOff:1;

        // This flag indicates registry-based policy.
        // If enabled and if the bus is not armed for wake,
        // then the controller will be placed in D3, regardless
        // of the capabilities of the parent bus.
        //
        // On some systems, this reduces the power consumption in sleep,
        // as the Hc is disabled (in D3).
        //
        ULONG   ForceHcD3NoWakeArm:1;

        ULONG   Reserved:22;
    };

} POWER_FLAGS, *PPOWER_FLAGS;

C_ASSERT(sizeof(POWER_FLAGS) == sizeof(ULONG));


//  FDO flags
typedef union _FDO_FLAGS {

    ULONG                  ul;
    struct {

        ULONG   IrqConnected:1;

        ULONG   PollController:1;

        ULONG   IrqEnabled:1;

        ULONG   LegacySymbolicLink:1;

        ULONG   ControllerGone:1;

        ULONG   LegacyBIOS:1;

        ULONG   CatcTrap:1;

        ULONG   HctDiagMode:1;

        ULONG   IsCompanionController:1;

        ULONG   FdoRegistered:1;

        ULONG   RhIrqEnabled:1;

        ULONG   PnpIoStartRef:1;

        ULONG   PMDebug:1;

        ULONG   OnPnpThread:1;

        ULONG   UsbAddressInit:1;

        ULONG   HctForcePortsHighSpeed:1;

        ULONG   HctDCAEnabled:1;

        ULONG   IgnoreResumeSignaling:1;

        ULONG   InterruptPerTd:1;

        ULONG   Reserved:13;
    };

} FDO_FLAGS, *PFDO_FLAGS;


C_ASSERT(sizeof(FDO_FLAGS) == sizeof(ULONG));

// default setting for the hc instanced SSP key used by
// USBPORT_SelectiveSuspendRegistry
// 0= ON by default; 1= OFF by default

#if defined(_IA64_)
// selective suspend OFF by default
#define HC_DISABLESSP_DEFAULT        1
#else
#define HC_DISABLESSP_DEFAULT        0
//#define HC_DISABLESSP_DEFAULT        1
#endif

typedef enum _USBHC_SSP_SUPPORT {
    // default is to check registry
    SSPsupport_RegDefined             =0,
    SSPsupport_RegEnabled,
    SSPsupport_RegDisabled,
    SSPsupport_GlobalDisabled,
    SSPsupport_HcDisabled,
    SSPsupport_ErrorDisabled
} USBHC_SSP_SUPPORT;

// MiniportStateFlags
//  miniport is either started (set) OR not started (clear)
#define MP_STATE_STARTED                0x00000001
#define MP_STATE_SUSPENDED              0x00000002
#define MP_STATE_FORCE_RESET            0x00000004
#define MP_STATE_IN_RESET_RECOVERY      0x00000008

// USB HC wake states

typedef enum _USBHC_WAKE_STATE {
    HCWAKESTATE_DISARMED             =1,
    HCWAKESTATE_WAITING              =2,
    HCWAKESTATE_WAITING_CANCELLED    =3,
    HCWAKESTATE_ARMED                =4,
    HCWAKESTATE_ARMING_CANCELLED     =5,
    HCWAKESTATE_COMPLETING           =7
} USBHC_WAKE_STATE;

// states for USB2 and CC start synchronization
typedef enum _USB2_CFG_STATE {
    Unitialized = 0,
    Usb2RootInit = 1,
    Usb2Start = 2,
    UsbAllStart = 3,
    Usb2WaitforCfgFlag = 4,
    NotUsb2
} USB2_CFG_STATE;

/*
    Hc power state transitions

Power up                        S-state         D-state     RootHubReady
                                                            (sig == 1)
-------------------------------------------------------------------------------
FdoPwr_WaitS0                   Sx              Dx          0
 FdoPwr_WaitS0_IoComplete       S0              Dx          0
  FdoPwr_WaitD0_PoComplete      S0              D0          0
   FdoPwr_D0_WaitWorker1        S0              D0          0
   FdoPwr_D0_WaitWorker2        S0              D0          0
    FdoPwr_D0_WaitSyncUsb2      S0              D0          0
      FdoPwr_WaitSx (in S0)     S0              D0          1
...............................................................................
Power down                      S-state         D-state     RootHubReady
                                                            (sig == 1)
--------------------------------------------------------------------------------
FdoPwr_WaitSx                   S0              D0          1
 FdoPwr_WaitSx_IoComplete       Sx              D0          0
  FdoPwr_WaitDx_PoComplete      Sx              D0          0
    FdoPwr_WaitS0 (in Sx)       Sx              D0          0


*/

typedef enum _HC_FDO_POWERSTATE {

    // waiting to enter working after
    // system suspend
    FdoPwr_WaitS0 = 1,          //USBPORT_FdoSystemPowerState
    FdoPwr_WaitS0_IoComplete,   //USBPORT_SystemPowerS0IrpCompletion
    FdoPwr_WaitD0_PoComplete,   //USBPORT_PoRequestCompletion
    FdoPwr_D0_WaitWorker1,      //USBPORT_PowerUpPhase1
    FdoPwr_D0_WaitWorker2,      //USBPORT_PowerUpPhase2
    FdoPwr_D0_WaitSyncUsb2,     //USBPORT_SynchronizeControllersResume

    // in S0 waiting for Sx
    FdoPwr_WaitSx,
    FdoPwr_WaitSx_IoComplete,   //USBPORT_SystemPowerSxIrpCompletion
    FdoPwr_WaitDx_PoComplete,   //USBPORT_PoRequestCompletion
    FdoPwr_WaitSx_Chirping,
    FdoPwr_PowerError,
    FdoPwr_HardwareFailure


} HC_FDO_POWERSTATE, *PHC_FDO_POWERSTATE ;


typedef enum _HC_FDO_EVENT {

    FdoPwrEv_SetS0 = 1,                 //seS0
    FdoPwrEv_S0_IoComplete,             //cpS0
    FdoPwrEv_D0_PoComplete_Success,     //D0aa
    FdoPwrEv_D0_PoComplete_Error,       //D0er
    FdoPwrEv_D0_DoSetD0_1,  // phase 1  //D0p1
    FdoPwrEv_D0_DoSetD0_2,  // phase 2  //D0p2

    FdoPwrEv_SyncUsb2_Complete,
    FdoPwrEv_Rh_SetD0,
    //0x09
    FdoPwrEv_Rh_Wake,
    //0x0a
    FdoPwrEv_Rh_NoWake,
    //0x0b
    FdoPwrEv_Sx_IoComplete,
    //0x0c
    FdoPwrEv_Dx_PoComplete_Success,
    //0x0d
    FdoPwrEv_Dx_PoComplete_Error,
    //0x0e
    FdoPwrEv_SetS1,
    //0x0f
    FdoPwrEv_SetS2,
    //0x10
    FdoPwrEv_SetS3,
    //0x11
    FdoPwrEv_SetS4,
    //0x12
    FdoPwrEv_SetS5,
    //0x13
    FdoPwrEv_NoD0Worker,
    //0x14
    FdoPwrEv_Rh_SetPowerSys,
    //0x15
    FdoPwrEv_SyncUsb2_NotUsb2,
    //0x16
    FdoPwrEv_SyncUsb2_Usb2Disabled,
    //0x17
    FdoPwrEv_SyncUsb2_DoChirp,
    //0x18
    FdoPwrEv_SyncUsb2_WaitChirp,
    //0x19
    FdoPwrEv_SyncUsb2_Chirping,
    //0x1a
    FdoPwrEv_SyncUsb2_WaitTimeout,
    //0x1b
    FdoPwrEv_D0_HardwareFailure,
    //0x1c
    FdoPwrEv_Rh_SetPowerSys_S0_Resume,
    //0x1d
    FdoPwrEv_SetSxHybridSleepDetected

} HC_FDO_EVENT, *PHC_FDO_EVENT ;


typedef struct _HC_STATE_LOG_ENTRY {

    HC_FDO_EVENT Event;
    HC_FDO_POWERSTATE CurrentState;
    HC_FDO_POWERSTATE NextState;
    NTSTATUS NtStatus;
    PVOID ContextData;
    LARGE_INTEGER SysTime;

} HC_STATE_LOG_ENTRY, *PHC_STATE_LOG_ENTRY;

#define NUM_STATE_LOG_ENTRIES 8

/*
    XDPC transfer DPCs
*/

typedef enum _XDPC_STATE {
    XDPC_Undef = 0,
    XDPC_Enabled,
    XDPC_DpcQueued,
    XDPC_Running,
    XDPC_Signaled,
    XDPC_RunningDisabled,
    XDPC_DpcQueuedDisabled,
    XDPC_Disabled,
} XDPC_STATE;

/*
*/

typedef enum _XDPC_EVENT {
    Ev_Xdpc_InitDpc = 0,
    Ev_Xdpc_Enable,
    Ev_Xdpc_Disable,
    Ev_Xdpc_TimeCheck,
    Ev_Xdpc_TimeExpired,
    Ev_Xdpc_Begin,
    Ev_Xdpc_End,
    Ev_Xdpc_Signal,
    Ev_Xdpc_Worker,
    Ev_Xdpc_Error
} XDPC_EVENT;

#pragma warning(push)
#pragma warning(disable: 4115) // Needed because C doesn't allow forward declaration

typedef
__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
(*PXDPC_WORKER_ROUTINE) (
    __inout PDEVICE_OBJECT,
    __inout struct _XDPC_CONTEXT *,
    __drv_restoresIRQL __in KIRQL
    );

#pragma warning(pop)

typedef struct _XDPC_STATE_LOG_ENTRY {

    XDPC_EVENT Event;
    XDPC_STATE CurrentState;
    XDPC_STATE NextState;
    ULONG  Info;

} XDPC_STATE_LOG_ENTRY, *PXDPC_STATE_LOG_ENTRY;

#define NUM_XDPC_STATE_LOG_ENTRIES 8

typedef struct _XDPC_CONTEXT  {

    // list of all USB transfer DPCs
    LIST_ENTRY XdpcLink;
    ULONG Sig;
    KDPC Dpc;
    KDPC ThreadedDpc;

    // run time for this dpc
    USB_TIMEOUT Timeout;

    PKTHREAD Thread;
    PDEVICE_OBJECT HcFdo;

    // transfer being processed by this dpc
    struct _HCD_TRANSFER_CONTEXT *CurrentTransfer;

    // Next transfer that will be processed by this dpc
    struct _HCD_TRANSFER_CONTEXT *NextTransfer;

    // ref for FDO
    USB_IOREQUEST_CONTEXT UsbIoRequest;

    ULONG TxLockState;
    PXDPC_WORKER_ROUTINE DpcWorker;

    // max time in milliseconds this dpc should be allowed to run
    ULONG DpcLimit_ms;

    XDPC_STATE State;

    ULONG LogIdx;
    XDPC_STATE_LOG_ENTRY Log[NUM_XDPC_STATE_LOG_ENTRIES];

} XDPC_CONTEXT, *PXDPC_CONTEXT;


typedef struct _USB_ADDRESS_LIST {
    RTL_BITMAP Bitmap;
    ULONG Bits[4];
} USB_ADDRESS_LIST, *PUSB_ADDRESS_LIST;

extern REGHANDLE    EtwRegHandle;
extern volatile ULONG        EtwLoggingEnabled;
extern volatile ULONG        EtwPerfLoggingEnabled;

#include <PSHPACK1.H>

typedef struct _ETW_HC_DEVICE {

    PDEVICE_OBJECT  DeviceObject;
    ULONG           PciBus;
    USHORT          PciDevice;
    USHORT          PciFunction;
    USHORT          PciVendorId;
    USHORT          PciDeviceId;

} ETW_HC_DEVICE, *PETW_HC_DEVICE;

#include <POPPACK.H>

#define MAX_ROOT_HUB_PORTS  16

#define EHCI_COMP_IDX   0

typedef enum _RUNTIME_PM_STATE {
    RTPM_Uninitialized,
    RTPM_OutstandingIdle,
    RTPM_Idle,
    RTPM_OutstandingActive,
    RTPM_Active
} RUNTIME_PM_STATE;

typedef enum _RUNTIME_PM_EVENT {
    RunTimePmEventPowerRequiredCallbackInvoked,
    RunTimePmEventPowerNotRequiredCallbackInvoked,
    RunTimePmEventActiveCallbackInvoked,
    RunTimePmEventIdleCallbackInvoked,
    RunTimePmEventEnteredD0,
    RunTimePmEventEnteredD2,
    RunTimePmEventEnteredD3
} RUNTIME_PM_EVENT;

typedef enum _RUNTIME_PM_SETUP_STATUS {
    RunTimePmSetupStatusSuccessful,
    RunTimePmSetupStatusFailedDueToCompanionController,
    RunTimePmSetupStatusFailedDueToNoS0WakeSupport,
    RunTimePmSetupStatusFailedDueToFailedPoRegistration
} RUNTIME_PM_SETUP_STATUS;


typedef struct _USBPORT_RTPM_CONTEXT {

    // Related to registering / unregistering
    POHANDLE Handle;

    // Current runtime PM state
    RUNTIME_PM_STATE RuntimePMState;

    // Event to signal for ComponentState
    KEVENT ComponentCallbackActiveEvent;

    // Tracks whether RTPM is suspended or not
    BOOLEAN RuntimePMSuspended;

    // The context used by the state machine engine
    SM_CONTEXT  RtpmSmContext;

    DEVICE_WAKE_DEPTH   S0WakeableDState;

    //DevicePowerState and SetupStatus are used in
    //run down ETW events to help debug power issues
    DEVICE_POWER_STATE  CurrentDevicePowerState;

    RUNTIME_PM_SETUP_STATUS  SetupStatus;

} USBPORT_RTPM_CONTEXT, *PUSBPORT_RTPM_CONTEXT;

NTSTATUS
USBPORT_RuntimePMContextInit(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RuntimePMContextFree(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_RuntimePMRegister(
    PDEVICE_OBJECT FdoDeviceObject
    );

BOOLEAN
USBPORT_IsRuntimePMEnabled(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RuntimePMStateActiveAssert(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RequestRuntimePMSuspend(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RequestRuntimePMResume(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RequestRuntimePMTransitionToActive(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RequestRuntimePMTransitionToIdle(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_RequestRuntimePMExit(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORTRTPM_EtwEvent(
    PDEVICE_OBJECT      FdoDeviceObject,
    RUNTIME_PM_EVENT    Event
    );

typedef struct _FDO_EXTENSION {

    PDEVICE_OBJECT PhysicalDeviceObject;
    PDEVICE_OBJECT TopOfStackDeviceObject;

    PDEVICE_OBJECT RhPdoPtr;

    KSPIN_LOCK RhPdoPtrSpin;

    // to serialize access to root hub data structures
    KSPIN_LOCK RootHubDpcSpin;

    // enable power management debug tracing
    ULONG PMDebug;
    // 7 char NULL terminated ansi driver name UHCI, EHCI, OHCI
    UCHAR Name[8];

    // addresses assigned to devices
    PUSB_ADDRESS_LIST AddressList;

    // pointer to miniport Data
    PDEVICE_DATA MiniportDeviceData;
    PUSBPORT_MINIPORT_DRIVER MiniportDriver;

    PKTHREAD MpSuspendThread;
    PKTHREAD MpResumeThread;

    // B/W Mgmt
    // track alloactions in bits/secs
    ULONG MinAllocedBw;
    ULONG MaxAllocedBw;
    ULONG AllocedIsoBW;
    ULONG AllocedLowSpeedBW;
    // for periods 1, 2, 4, 8, 16, 32
    ULONG AllocedInterruptBW[6];
    // USB 1.1 :  12000 ( 12 MBits/sec)
    // USB 2.0 : 400000 (400 MBits/sec)
    ULONG TotalBusBandwidth;
    ULONG BusBandwidthTable[USBPORT_MAX_INTEP_POLLING_INTERVAL];

    // BusContext
    LIST_ENTRY BusContextHead;
    KSPIN_LOCK BusContextSpin;

    // flags - few more at other places too
    ULONG HcOptionFlags;
    ULONG MpStateFlags;
    FDO_FLAGS Flags;
    DM_TIMER_FLAGS DmTimerFlags;
    DM_TIMER_STATE DmTimerState;
    // multi-state flags
    USBHC_SSP_SUPPORT SSP_Support;

    ULONG BusNumber; // slot
    ULONG BusDevice;
    ULONG BusFunction;
    ULONG Usb2BusFunction;
    ULONG DisablePciConfigSpace;
    // PCI and its bus interface
    USHORT PciVendorId;
    USHORT PciDeviceId; // USB productId
    UCHAR PciRevisionId; // USB bcdDevice
    UCHAR PciClass;
    UCHAR PciSubClass;
    UCHAR PciProgIf;
    USB_CONTROLLER_FLAVOR HcFlavor;
    BUS_INTERFACE_STANDARD PciBusInterface;
    D3COLD_SUPPORT_INTERFACE D3ColdInterface;
    PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER AcpiDeviceInformation;
    ULONG AcpiDeviceInformationSize;

    // USB2 and CC and their issues
    LONG DependentControllers;
    KSPIN_LOCK HcSyncSpin;

    // chirping process
    LONG InPortChirp;

    // held during device enum
    KSEMAPHORE BusLock;
    KSEMAPHORE DeviceCfgLock;
    // serilaizes access to the USB2 budget code
    KSEMAPHORE Usb2bwLock;
    // used to serialize calls into the miniports for MP_SuspendController and MP_ResumeController
    KSEMAPHORE MpSuspendResumeLock;

    // power related management
    ULONG PwrLogIdx;
    POWER_FLAGS PowerFlags;
    HC_FDO_POWERSTATE CurrentFdoPowerState;
    // log of last system power transition requested
    SYSTEM_POWER_STATE LastSystemSleepState;
    // this may be different if this is fast s4
    SYSTEM_POWER_STATE LastSystemSleepEntered;
    SYSTEM_POWER_STATE CurrentSystemPowerState;
    ULONG LastSystemSleepBusArmedForWake;
    ULONG NumSleepsEntered;

    HC_POWER_STATE_TABLE HcPowerStateTbl;
    ULONG FastResumeEnable;

    // wait wake
    KDPC HcWakeDpc;
    PIRP HcPendingWakeIrp;
    KEVENT HcPendingWakeIrpEvent;
    USBHC_WAKE_STATE HcWakeState;
    KSPIN_LOCK WakeIrpSpin;

    // pnp resources

    // -1 = Disconnected
    // 0  = Connected
    // 1  = IsrDpc Queued
    LONG IoConnectInterruptState;
    LONG InIsrDpc;

    PKINTERRUPT InterruptObject;
    HC_RESOURCES HcResources;
    ULONG UseSingleDmaAdapter;
    ULONG AdapterObjectDeviceDescriptionVersion;
    PDMA_ADAPTER CommonBufferAdapterObject;
    ULONG CommonBufferNumberOfMapRegisters;
    PDMA_ADAPTER AdapterObject;

    ULONG NumberOfMapRegisters;
    PUSBPORT_COMMON_BUFFER ScratchCommonBuffer;
    PUSBPORT_COMMON_BUFFER ControllerCommonBuffer;
    PUSBPORT_COMMON_BUFFER MpCommonBufferBlock[USB_MAX_COMMON_BUFFER_BLOCKS];

    LIST_ENTRY FreeSmallCommonBufferList;
    LIST_ENTRY FreeLargeCommonBufferList;
    ULONG FreeSmallCommonBufferCount;
    ULONG FreeSmallCommonBufferMaxCount;
    ULONG FreeLargeCommonBufferCount;
    ULONG FreeLargeCommonBufferMaxCount;
    ULONG LargeCommonBufferThreshold;
    KSPIN_LOCK CommonBufferListSpin;
    ULONG CommonBuffer2GBLimit;
    ULONG Bit64AddressingCapability;

    ULONG DeviceNameIdx;
    UNICODE_STRING LegacyLinkUnicodeString;

    // DM Timer
    KSPIN_LOCK DM_TimerSpin;
    KDPC DM_TimerDpc;
    KDPC DM_SimulateTimerDpc;
    KTIMER DM_Timer;

    // priority transfers
    LONG TxMapBusy;

    XDPC_CONTEXT UsbHcIntDpc;
    XDPC_CONTEXT UsbDoneDpc;
    XDPC_CONTEXT UsbMapDpc;
    XDPC_CONTEXT UsbIocDpc;

    LONG TxNeedsMapDpcCount;

    LONG UsbHcIntDpcGenerationCount;

    LIST_ENTRY XdpcListHead;

    LIST_ENTRY TxDoneListHead;
    KEVENT TxDoneListEmpty_Event;
    // this is the priority queue
    LIST_ENTRY TxPriorityListHead;      // protected by USBPORT_TxAcquireLock()
    KEVENT TxPriorityListEmpty_Event;
    ULONG TxPriorityListAsyncCount;     // protected by USBPORT_TxAcquireLock()
    ULONG TxPriorityListPeriodicCount;  // protected by USBPORT_TxAcquireLock()

    // holds transfers that we think the Csq is going to complete
    LIST_ENTRY TxCancelIrpListHead;
    KEVENT TxCancelIrpListEmpty_Event;

    // List of pending and completed abort-pipe requests
    LIST_ENTRY AxPendingListHead;
    LIST_ENTRY AxDoneListHead;
    LIST_ENTRY AxCanceledListHead;

    LIST_ENTRY EpNeoStateChangeList;

    ULONG HcdEndpointSize;

    // list of all endpoints in the system (see global endpoint state)
    LIST_ENTRY GlobalEndpointList;
    //protects GlobalEndpointList
    KSPIN_LOCK EndpointListSpin;
    ULONG EpListLockState;

    // device handles and tts
    LIST_ENTRY GlobalTtListHead;
    LIST_ENTRY DeviceHandleList;
    // handles sit on this list until we free them
    LIST_ENTRY DeviceHandleDeletedList;
    KSPIN_LOCK DevHandleListSpin;
    ULONG OrphanedDeviceCount;
    KSPIN_LOCK GlobalTtSpin;

    // protects all core functions called into mp and power events
    KSPIN_LOCK CoreFunctionSpin;
    KSPIN_LOCK MpRootHubFunctionSpin;
    // MpAsyncTimerListSpin protects MpAsyncTimerListHead
    KSPIN_LOCK MpAsyncTimerListSpin;

    // DPC queued from ISRs
    KDPC IsrDpc;
    KSPIN_LOCK IsrDpcSpin;

    //NEOROOT root hub vars
    // TxCsq is a queue for all Irps submitted to the root hub endpoints
    KDPC RootHubDpc;
    LIST_ENTRY RH_iTxqHead;
    IO_CSQ RH_iTxCsq;
    // MpAsyncTimerListHead is protected by MpAsyncTimerListSpin
    LIST_ENTRY MpAsyncTimerListHead;


    // variants of invalidate controller
    KDPC HcResetDpc;
    LONG HcResetWIQueued;
    USB_WORKITEM_HANDLE HcResetWI;
    KDPC SurpriseRemoveDpc;
    KDPC SimulateInterruptDpc;
    KTIMER SimulateInterruptTimer;
    PVOID SimulateInterruptHighResTimer;
    PUSB_IOREQUEST_CONTEXT SimulateInterruptIoRequest;

    // cache registry entries for speedup and raised irql access
    LIST_ENTRY RegistryCache;

    // seq num assigned to each transfer
    LONG NextTransferSequenceNumber;

    // context for USB2 budgeter
    PVOID HsbControllerContextPtr;

    // link controllers on global list
    LIST_ENTRY ControllerLink;
    LIST_ENTRY ControllerRundownLink;

    // stat counters
    KSPIN_LOCK StatCounterSpin;
    USBD_PERFORMANCE_DATA PerformanceData;
    ULONG StatPciInterruptCount;
    ULONG StatHardResetCount;
    ULONG StatWorkSignalCount;
    ULONG StatWorkIdleTime;
    ULONG StatCommonBufferBytes;
    ULONG ForceHCResetOnResume;

    //wmi
    WMILIB_CONTEXT WmiLibContext;

    //boot sync
    USB2_CFG_STATE CfgState;

    //power
    KEVENT ChirpCompleteEv;
    KEVENT RootHubReadyEv;
    NTSTATUS PowerError_Nts;
    KSPIN_LOCK PowerSpin;
    HC_STATE_LOG_ENTRY PwrLog[NUM_STATE_LOG_ENTRIES];




    BOOLEAN RootHubTTCapable;
    BOOLEAN UsbportWmiRegister;

    // static IoRequest Contexts
    // DmTimer Dpc
    USB_IOREQUEST_CONTEXT UsbIoDmTimer;
    // Dpc queued by the ISR (USBPORT_InterruptService)
    USB_IOREQUEST_CONTEXT UsbIoIsrDpc;
    // ScatterGather (Dma) operations
    USB_IOREQUEST_CONTEXT UsbIoDmaCallback;

    // data we record in a minidump
    USBPORT_MINIDUMP_DATA MiniDumpData;

    UNICODE_STRING  PhysicalDeviceObjectName;

    ETW_HC_DEVICE   EtwHcDevice;

    BOOLEAN         HcDisconnectRootHubPorts;
    RH_PORT_STATUS  HcDisconnectRootHubPortStatus[MAX_ROOT_HUB_PORTS];

    // Total count of paging/hiber/dump files in path
    ULONG InPathCount;
    // Count of hibernate files in path
    ULONG HibernateCount;

    // Count of how many times a threaded DPC has been used
    ULONG ThreadedDpcCount;

    USBPORT_RTPM_CONTEXT    RuntimePMContext;

    BOOLEAN IsUrsEnumerated;

    // Cache align the miniport device extension.  This must be the last
    // field of the USBPORT FDO_EXTENSION.
    //
    DECLSPEC_CACHEALIGN PVOID MiniportExtension[0];

} FDO_EXTENSION, *PFDO_EXTENSION;

C_ASSERT((sizeof(FDO_EXTENSION) % SYSTEM_CACHE_ALIGNMENT_SIZE) == 0);

// ref index for root hub PDO

#define REF_RhPDO_CreateRootHubPdo              0
#define REF_RhPDO_GetRootHubSymbolicName        1
#define REF_RhPDO_UsbFdoUserIoctl               2
#define REF_RhPDO_UserGetRootHubName            3
#define REF_RhPDO_UserPowerInformation          4
#define REF_RhPDO_UserGetBusStatistics0         5
#define REF_RhPDO_Stop                          6
#define REF_RhPDO_SSP                           7
#define REF_RhPDO_ReadUsb2CfgFlag               8
#define REF_RhPDO_InvokeRhCallback              9
#define REF_RhPDO_FdoSurpriseRemoval            10
#define REF_RhPDO_Remove                        11
#define REF_RhPDO_CompleteHubPdoWaitWake        12
#define REF_RhPDO_RootHubEnabledForWake         13
#define REF_RhPDO_CompleteCanceledIdleIrp       14
#define REF_RhPDO_Power                         15
#define REF_RhPDO_PowerUsb2Port_CC              16
#define REF_RhPDO_PowerCallback                 17
#define REF_RhPDO_TEST                          18
#define REF_RhPDO_UsbFdoIoctlDiagnosticModeOn   19

#define REF_RhPDO_MaxIndex                      20

typedef struct _PDO_EXTENSION {

    PUSBD_DEVICE_HANDLE RootHubDeviceHandle;
    // latches the interrupt endpoint if the root hub points to it
    PUSBOBJ_REF RH_InterruptEp;

    UCHAR ConfigurationValue;

    // pointers to our root hub descriptors
    // NOTE: these ptrs point in to the 'Descriptors'
    // buffer so dont try to free them
    PUSB_DEVICE_DESCRIPTOR DeviceDescriptor;
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor;
    PUSB_HUB_DESCRIPTOR HubDescriptor;

    // irp associated with remote wakeup, ie irp posted by the HUB driver
    // NOTE (neohub):
    // The presence or absence of the wake irp only indicates that the hub
    // is in a 'wakeup path'. The presence of the wake irp implies that the
    // BUS should be armed. We do this to keep the wake tree for WDM consistent.
    PIRP PendingWaitWakeIrp;

    // pointer to buffer contining descriptors
    PUCHAR Descriptors;

    PRH_INIT_CALLBACK HubInitCallback;
    PVOID HubInitContext;

    // data recorded in the event the hub driver fails to start.
    PUSB_START_FAILDATA FailData;

    // set when the hub driber posts an int transfer to the root
    LONG RhCfgFlag;

    // TRUE when root hub has been selectively suspended
    BOOLEAN SelectiveSuspend;

    KSEMAPHORE SSLock;

    // Count of paging/hiber/dump files in path
    // We use a single count for all three types of special files, since there
    // is no need (at this time) to distinguish between them.
    ULONG InPathCount;

    LONG PdoRefTable[REF_RhPDO_MaxIndex];

} PDO_EXTENSION, *PPDO_EXTENSION;


/* USB spec defined port flags */
#define PORT_STATUS_CONNECT         0x001
#define PORT_STATUS_ENABLE          0x002
#define PORT_STATUS_SUSPEND         0x004
#define PORT_STATUS_OVER_CURRENT    0x008
#define PORT_STATUS_RESET           0x010
#define PORT_STATUS_POWER           0x100
#define PORT_STATUS_LOW_SPEED       0x200
#define PORT_STATUS_HIGH_SPEED      0x400

/*
    root hub status codes
*/
typedef enum _RHSTATUS {

     RH_SUCCESS = 0,
     RH_NAK,
     RH_STALL,
     RH_PENDING

} RHSTATUS;

/* port operations */

typedef enum _PORT_OPERATION {

  SetFeaturePortReset = 0,
  SetFeaturePortPower,
  SetFeaturePortEnable,
  SetFeaturePortSuspend,
  ClearFeaturePortEnable,
  ClearFeaturePortPower,
  ClearFeaturePortSuspend,
  ClearFeaturePortEnableChange,
  ClearFeaturePortConnectChange,
  ClearFeaturePortResetChange,
  ClearFeaturePortSuspendChange,
  ClearFeaturePortOvercurrentChange

} PORT_OPERATION;

// UsbDbgBreakFlags
#define HC_BREAK_ON_ISR_DPC         0x0000001


#define NUMBER_OF_PORTS(de) ((de)->Pdo.HubDescriptor->bNumberOfPorts)
#define HUB_DESRIPTOR_LENGTH(de) ((de)->Pdo.HubDescriptor->bDescriptorLength)

#define USB_PP_HISTORY_LEN 16


typedef struct _DEVICE_EXTENSION {

    ULONG Sig;
    ULONG PadToOffset8;
    PUSBPORT_TRIAGE_INFO UsbportTriageInfo;
    PDEVICE_OBJECT HcFdoDeviceObject;
    DEBUG_LOG Log;

    // these are used to trigger breaks on specific instances of a host controller or root hub, these rae used to trigger bereaks on the
    // ISR or ISR_DPC for a pariticular controller instance
    // see USB_DBBFLAG_nnn
    ULONG UsbDbgBreakFlags;
    USBPORT_INTERFACE_STATE InterfaceState;
    USBPORT_PNP_STATE PnpState;

    ULONG CurrentPnpFunc;
    ULONG PnpFuncHistoryIdx;
    UCHAR PnpFuncHistory[USB_PP_HISTORY_LEN];

    ULONG CurrentPowerFunc;
    ULONG PowerFuncHistoryIdx;
    UCHAR PowerFuncHistory[USB_PP_HISTORY_LEN];
    ULONG PowerFuncHistoryData[USB_PP_HISTORY_LEN];

    ULONG PnpLogIdx;
    HC_STATE_LOG_ENTRY PnpLog[NUM_STATE_LOG_ENTRIES];

    DEVICE_POWER_STATE Dx_State;

    DEVICE_CAPABILITIES DeviceCapabilities;

    // RemoveLock type effect
    KSPIN_LOCK IoRequestSpin;
    LONG IoRequestCount;
    LONG IoRequestAsyncCallbackCount;
    LIST_ENTRY IoRequestListHead;
    LONG IoRequestCountNoRes;
    USB_IOREQUEST_CONTEXT IoRequestContext;

    // coresponds to _MN_STOP on FDO
    USB_IOREQUEST_CONTEXT UsbIoPnpStop;
    KEVENT IoRequestStopEvent;

    // coresponds to _MN_REMOVE on FDO
    USB_IOREQUEST_CONTEXT UsbIoPnpRemove;
    KEVENT IoRequestRemoveEvent;
    ULONG IoRequestAllow;

    // Set when IoRequestAsyncCallbackCount is -1.  Waited on before
    // calling MINIPORT_StopController() or MINIPORT_SuspendController().
    //
    KEVENT IoRequestAsyncCallbackEvent;

    // io cancel safe queue
    // only valid for FDO
    IO_CSQ TxCsq;
    LIST_ENTRY TxCsqIrpListHead;

    IO_CSQ AxCsq;
    LIST_ENTRY AxCsqIrpListHead;

    UNICODE_STRING SymbolicLinkName;

    PHCD_TRANSFER_CONTEXT CurrentAbortTransfer;

    T_WORK_STATE TestWorkState;
    ULONG TestWorkWait;

    PEHCI_CRASHDUMP_CONTEXT DumpContext[DUMP_MAX_CONTEXT_COUNT];

    union {
        PDO_EXTENSION Pdo;
        FDO_EXTENSION Fdo;
    };

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

C_ASSERT(FIELD_OFFSET(DEVICE_EXTENSION, Sig) == 0);
C_ASSERT(FIELD_OFFSET(DEVICE_EXTENSION, UsbportTriageInfo) == USBPORT_TRIAGE_INFO_OFFSET);

__inline
PMDL
FPAllocMdl(
    __in PDEVICE_EXTENSION DevExt,
    __in PUSBD_DEVICE_HANDLE DeviceHandle,
    __in PHCD_ENDPOINT Endpoint,
    __in ULONG TransferFlags,
    __in ULONG TransferLength
    )
{
    if (TEST_FLAG(TransferFlags, USBD_FORWARD_PROGRESS_TRANSFER)) {
        return FPAllocMdlEP(Endpoint, TransferLength);
    } else {
        return NULL;
    }
    UNREFERENCED_PARAMETER(DevExt);
    UNREFERENCED_PARAMETER(DeviceHandle);
}

__inline
BOOLEAN
FPFreeMdl(
    __in PDEVICE_EXTENSION DevExt,
    __in PUSBD_DEVICE_HANDLE DeviceHandle,
    __in PHCD_ENDPOINT Endpoint,
    __in PMDL Mdl
    )
{
    return FPFreeMdlEP(Endpoint, Mdl);
    UNREFERENCED_PARAMETER(DevExt);
    UNREFERENCED_PARAMETER(DeviceHandle);
}

__inline
PVOID
FPAllocTransferContext(
    __in PDEVICE_EXTENSION DevExt,
    __in PUSBD_DEVICE_HANDLE DeviceHandle,
    __in PHCD_ENDPOINT Endpoint,
    __in ULONG TransferFlags,
    __in ULONG TransferContextSize
    )
{
    if (TEST_FLAG(TransferFlags, USBD_FORWARD_PROGRESS_TRANSFER)) {
        return FPAllocTransferContextEP(Endpoint, TransferContextSize);
    } else {
        return NULL;
    }
    UNREFERENCED_PARAMETER(DevExt);
    UNREFERENCED_PARAMETER(DeviceHandle);
}

__inline
BOOLEAN
FPFreeTransferContext(
    __in PDEVICE_EXTENSION DevExt,
    __in PUSBD_DEVICE_HANDLE DeviceHandle,
    __in PHCD_ENDPOINT Endpoint,
    __in PVOID TransferContext
    )
{
    return FPFreeTransferContextEP(Endpoint,TransferContext);
    UNREFERENCED_PARAMETER(DevExt);
    UNREFERENCED_PARAMETER(DeviceHandle);
}

__inline
PVOID
FPAllocSgBuf(
    __in PDEVICE_EXTENSION DevExt,
    __in PUSBD_DEVICE_HANDLE DeviceHandle,
    __in PHCD_ENDPOINT Endpoint,
    __in ULONG TransferFlags,
    __in ULONG TransferLength,
    __in PULONG AllocationLength
    )
{
    if (TEST_FLAG(TransferFlags, USBD_FORWARD_PROGRESS_TRANSFER)) {
        return FPAllocSgBufEP(Endpoint, TransferLength, AllocationLength);
    } else {
        return NULL;
    }
    UNREFERENCED_PARAMETER(DevExt);
    UNREFERENCED_PARAMETER(DeviceHandle);
}

__inline
BOOLEAN
FPFreeSgBuf(
    __in PDEVICE_EXTENSION DevExt,
    __in PUSBD_DEVICE_HANDLE DeviceHandle,
    __in PHCD_ENDPOINT Endpoint,
    __in PVOID SGBuffer
    )
{
    return FPFreeSgBufEP(Endpoint, SGBuffer);
    UNREFERENCED_PARAMETER(DevExt);
    UNREFERENCED_PARAMETER(DeviceHandle);
}

/*
    used to track cached registry keys for miniports
*/

typedef struct _USBPORT_REG_CACHE_ENTRY {
    ULONG Sig;
    LIST_ENTRY RegLink;
    BOOLEAN SoftwareBranch;
    // length in bytes
    ULONG DataLength;
    PUCHAR Data;
    ULONG KeyNameStringLength;
    WCHAR KeyNameString[0];
} USBPORT_REG_CACHE_ENTRY, *PUSBPORT_REG_CACHE_ENTRY;

#include <PSHPACK1.H>

typedef struct _ETW_USBUSER_OP_SEND_ONE_PACKET_DATA {

    UCHAR       DeviceAddress;
    UCHAR       EndpointAddress;
    USHORT      MaximumPacketSize;
    ULONG       Timeout;
    ULONG       UsbPacketFlags;
    ULONG       DataLength;
    USHORT      HubDeviceAddress;
    USHORT      PortTTNumber;
    USBD_STATUS UsbdStatus;

} ETW_USBUSER_OP_SEND_ONE_PACKET_DATA, *PETW_USBUSER_OP_SEND_ONE_PACKET_DATA;

#include <POPPACK.H>

// define an interlocked version of RemoveEntryList
#define USBPORT_InterlockedRemoveEntryList(ListEntry, Spinlock) \
    {\
        KIRQL irql;\
        KeAcquireSpinLock((Spinlock), &irql);\
        RemoveEntryList((ListEntry));\
        KeReleaseSpinLock((Spinlock), irql);\
    }

#define GET_HEAD_LIST(head, le) \
    if (IsListEmpty(&(head))) {\
        le = NULL;\
    } else {\
        le = (head).Flink;\
    }

//
// allocates a zeroed buffer that the OS is expedected to free
//
#define ALLOC_POOL_OSOWNED(p, pt, nb) UsbAllocPool_Z((p), (pt), (nb))

#ifndef KDEXTS

#define UsbFreePool(p) ExFreePoolWithTag((p), USBPORT_TAG)

//
// Wrapper for alloc pool.
// On success the buffer is zeroed out and STATUS_SUCCESS is returned
// On Failure STATUS_INSUFFICIENT_RESOURCES and Ptr is set to NULL.
//

NTSTATUS
__forceinline
UsbAllocPool_Z(
    __deref_out_bcount(NumberOfBytes) PVOID *Ptr,
    __in POOL_TYPE PoolType,
    __in ULONG NumberOfBytes
    )
{
    PVOID p;

    if (Ptr == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    *Ptr = NULL;
    USBPORT_ASSERT(NumberOfBytes);
    if (NumberOfBytes == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    p = ExAllocatePoolWithTag(PoolType, NumberOfBytes, USBPORT_TAG);

    if (p) {
        RtlZeroMemory(p, NumberOfBytes);
        *Ptr = p;
        return STATUS_SUCCESS;
    } else {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

}
#endif

//
// allocates a zeroed buffer that we are expected to free
//
#define ALLOC_POOL_Z(p, pt, nb) UsbAllocPool_Z(&(p), pt, nb)

#define GET_DEVICE_EXT(e, d) (e) = (d)->DeviceExtension
#define GET_DEVICE_HANDLE(dh, urb) \
    (dh) = ((PURB)(urb))->UrbHeader.UsbdDeviceHandle;

#define DEVEXT_FROM_DEVDATA(de, dd) \
    (de) = (PDEVICE_EXTENSION) \
            CONTAINING_RECORD((dd),\
                              struct _DEVICE_EXTENSION, \
                              Fdo.MiniportExtension)

#define ENDPOINT_FROM_EPDATA(ep, epd) \
    (ep) = (PHCD_ENDPOINT) \
            CONTAINING_RECORD((epd),\
                              struct _HCD_ENDPOINT, \
                              MiniportEndpointData)

#define TRANSFER_FROM_TPARAMETERS(t, tp) \
    (t) = (PHCD_TRANSFER_CONTEXT) \
            CONTAINING_RECORD((tp),\
                              struct _HCD_TRANSFER_CONTEXT, \
                              Tp)

#define SET_USBD_ERROR(u, s) USBPORT_SetUSBDError((PURB)(u),(s))


#define ACQUIRE_WAKEIRP_LOCK(fdo, i) \
    do {\
    PDEVICE_EXTENSION ext;\
    GET_DEVICE_EXT(ext, (fdo));\
    ASSERT_FDOEXT(ext);\
    LOGENTRY(fdo, LOG_POWER, 'wwL+', 0, (fdo), 0);\
    KeAcquireSpinLock(&ext->Fdo.WakeIrpSpin, &(i));\
    } WHILE (0)

#define RELEASE_WAKEIRP_LOCK(fdo, i) \
    do {\
    PDEVICE_EXTENSION ext;\
    GET_DEVICE_EXT(ext, (fdo));\
    ASSERT_FDOEXT(ext);\
    LOGENTRY(fdo, LOG_POWER, 'wwL-', 0, (fdo), (i));\
    KeReleaseSpinLock(&ext->Fdo.WakeIrpSpin, (i));\
    } WHILE (0)

#define USBPORT_AcquireSpinLock(HcFdo, SpinLock, Irql) KeAcquireSpinLock(SpinLock, Irql)
#define USBPORT_ReleaseSpinLock(HcFdo, SpinLock, Irql) KeReleaseSpinLock(SpinLock, Irql)

// sl = spinlock
// c = context
// this macro currently maps to a conventional spinlock but can
// be changed to an queued spinlock or other call
#define ACQUIRE_ROOTHUB_LOCK(de, c) KeAcquireSpinLock(&((de)->Fdo.RootHubDpcSpin), (c))
#define RELEASE_ROOTHUB_LOCK(de, c) KeReleaseSpinLock(&((de)->Fdo.RootHubDpcSpin), (c))


#define USBPORT_IS_USB20(de)\
    ((de)->Fdo.HcOptionFlags & USB_MINIPORT_OPT_USB20)

#define USBPORT_IS_USBX(de)\
    ((de)->Fdo.HcOptionFlags & USB_MINIPORT_OPT_USBX)

#define USBPORT_HW_SYNC(de)\
    ((de)->Fdo.HcOptionFlags & USB_MINIPORT_OPT_HW_SYNC)

#define USBPORT_IS_TT_ENABLED(de)\
    (((de)->Fdo.HcOptionFlags & USB_MINIPORT_OPT_USB20) || \
    ((de)->Fdo.HcOptionFlags & USB_MINIPORT_OPT_TT_HUB_ENABLED))

#define USBPORT_ACQUIRE_DM_LOCK(de, i) \
    KeAcquireSpinLock(&(de)->Fdo.DM_TimerSpin, &(i))

#define USBPORT_RELEASE_DM_LOCK(de, i) \
    KeReleaseSpinLock(&(de)->Fdo.DM_TimerSpin, (i))

#define USBPORT_ACQUIRE_POWER_LOCK(de, i) \
    KeAcquireSpinLock(&(de)->Fdo.PowerSpin, &(i))

#define USBPORT_RELEASE_POWER_LOCK(de, i) \
    KeReleaseSpinLock(&(de)->Fdo.PowerSpin, (i))

//
// Macros to set transfer direction flag
//

#define USBPORT_SET_TRANSFER_DIRECTION_IN(tf)  \
        ((tf) |= USBD_TRANSFER_DIRECTION_IN)

#define USBPORT_SET_TRANSFER_DIRECTION_OUT(tf) \
        ((tf) &= ~USBD_TRANSFER_DIRECTION_IN)


//
// Flags for the URB header flags field used by port
//

#define USBPORT_REQUEST_IS_TRANSFER        0x00000001
#define USBPORT_REQUEST_MDL_ALLOCATED      0x00000002
#define USBPORT_REQUEST_USES_DEFAULT_PIPE  0x00000004
#define USBPORT_REQUEST_NO_DATA_PHASE      0x00000008
#define USBPORT_RESET_DATA_TOGGLE          0x00000010
#define USBPORT_TRANSFER_ALLOCATED         0x00000020
#define USBPORT_REQUEST_IS_PIPE_REQUEST    0x00000040


#define USBPORT_INIT_SETUP_PACKET(s, brequest, \
    direction, recipient, typ, wvalue, windex, wlength) \
    {\
    (s).bRequest = (brequest);\
    (s).bmRequestType.Dir = (direction);\
    (s).bmRequestType.Type = (typ);\
    (s).bmRequestType.Recipient = (recipient);\
    (s).bmRequestType.Reserved = 0;\
    (s).wValue.W = (wvalue);\
    (s).wIndex.W = (windex);\
    (s).wLength = (wlength);\
    }

#define USBPORT_INIT_SETUP_PACKET_EX(s, brequest, \
    bmrequest, wvalue, windex, wlength) \
    {\
    (s).bRequest = (brequest);\
    (s).bmRequestType.B = (bmrequest);\
    (s).wValue.W = (wvalue);\
    (s).wIndex.W = (windex);\
    (s).wLength = (wlength);\
    }

/*
 * miniport interface macros
 */

#define MPF_RESTART         0x00000001
#define MPF_ON_PNP_THREAD   0x00000002

// These two macro have the same definition.  The 'MPF' macro is used
// when accessing the host controller RegistrationPacket to access a
// Miniport Function.  The 'REGISTRATION_PACKET' macro is used for all
// other purposes of accessing the host controller RegistrationPacket.
//
#define REGISTRATION_PACKET(de) ((de)->Fdo.MiniportDriver->RegistrationPacket)

#define MPF(de) ((de)->Fdo.MiniportDriver->RegistrationPacket)


#define MP_DisableInterrupts(fdo) USBPORT_MpInterrupts((fdo), FALSE);

#define MP_EnableInterrupts(fdo) USBPORT_MpInterrupts((fdo), TRUE);


#define MP_StartSendOnePacket(de, p, mpd, mpl, vaddr, phys, len, u, mpStatus) \
    {\
    KIRQL irql;\
    USBPORT_ASSERT(MPF(de).MINIPORT_StartSendOnePacket != NULL); \
    USBPORT_AcquireSpinLock((de)->HcFdoDeviceObject, &(de)->Fdo.CoreFunctionSpin, &irql);\
    mpStatus = MPF(de).MINIPORT_StartSendOnePacket(\
        (de)->Fdo.MiniportDeviceData,\
        (p),\
        (mpd),\
        (mpl),\
        (vaddr),\
        (phys),\
        (len),\
        (u));\
    USBPORT_ReleaseSpinLock((de)->HcFdoDeviceObject, &(de)->Fdo.CoreFunctionSpin, irql);\
    }


#define MP_EndSendOnePacket(de,p, mpd, mpl, vaddr, phys, len, u, mpStatus) \
    {\
    KIRQL irql;\
    USBPORT_ASSERT(MPF(de).MINIPORT_EndSendOnePacket != NULL); \
    USBPORT_AcquireSpinLock((de)->HcFdoDeviceObject, &(de)->Fdo.CoreFunctionSpin, &irql);\
    mpStatus = MPF(de).MINIPORT_EndSendOnePacket(\
        (de)->Fdo.MiniportDeviceData,\
        (p),\
        (mpd),\
        (mpl),\
        (vaddr),\
        (phys),\
        (len),\
        (u));\
    USBPORT_ReleaseSpinLock((de)->HcFdoDeviceObject, &(de)->Fdo.CoreFunctionSpin, irql);\
    }

#define MPRH_GetStatus(de, s, mpStatus) \
    (mpStatus) = MPF(de).MINIPORT_RH_GetStatus(\
        (de)->Fdo.MiniportDeviceData,\
        (s))


#if 0
#define MPRH_GetPortStatus(de, port, status, mpStatus) \
    (mpStatus) = MPF(de).MINIPORT_RH_GetPortStatus(\
        (de)->Fdo.MiniportDeviceData,\
        (port),\
        (status))
#else
#define MPRH_GetPortStatus(de, port, status, mpStatus) \
    (mpStatus) = USBPORT_GetRootHubPortStatus((de),\
                                              (port),\
                                              (status))
#endif


#define MPRH_GetHubStatus(de, status, mpStatus) \
    (mpStatus) = MPF(de).MINIPORT_RH_GetHubStatus(\
        (de)->Fdo.MiniportDeviceData,\
        (status))

#define MPRH_GetRootHubData(de, data) \
    MPF(de).MINIPORT_RH_GetRootHubData((de)->Fdo.MiniportDeviceData,\
                                       (data))


// see USBPORT_RH_NeoQueueAsyncTransfer

#define ROOTHUB_LOCK_ACQUIRED       1
#define ROOTHUB_LOCK_NOT_ACQUIRED   0

VOID
__inline
MPRH_DisableIrq(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in ULONG RootHubLockStatus
    );

VOID
__inline
MPRH_EnableIrq(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in ULONG RootHubLockStatus
    );

#define MP_PassThru(de, guid, l, data, mpStatus) \
                                                 \
        if (MPF(de).MINIPORT_PassThru) { \
            (mpStatus) = MPF(de).MINIPORT_PassThru(\
                (de)->Fdo.MiniportDeviceData,\
                (guid),\
                (l),\
                (data)); \
        } \
        else { \
            (mpStatus) = USBMP_STATUS_NOT_SUPPORTED; \
        }

#define MILLISECONDS_TO_100_NS_UNITS(ms) (((LONG)(ms)) * 10000)


#define USBPORT_GET_BIT_SET(d, bit) \
    do {   \
        UCHAR tmp = (UCHAR)(d);\
        (bit)=0; \
        while (!(tmp & 0x01)) {\
            (bit)++;\
            tmp >>= 1;\
        };\
    } WHILE (0)

#include <PSHPACK1.H>
typedef struct _ETW_EXECUTION_METRICS {

    ULARGE_INTEGER  StartTime;  // In MicroSeconds
    ULARGE_INTEGER  EndTime;    // In MicroSeconds
    ULARGE_INTEGER  Delta;      // In MicroSeconds
} ETW_EXECUTION_METRICS, *PETW_EXECUTION_METRICS;
#include <POPPACK.H>

//
// Macros used to log Code execution times. These are enabled only when ETW tracing
// is enabled
//

#define INIT_EXEC_METRICS(X)    \
    do  {   \
        X.StartTime.QuadPart = 0;\
        X.EndTime.QuadPart = 0;\
        X.Delta.QuadPart = 0;\
    } WHILE(0)

#define ETW_MEASURE_TIME_START(X)  \
    do  {   \
        INIT_EXEC_METRICS(X);\
        if(EtwPerfLoggingEnabled && EtwEventEnabled(EtwRegHandle, &USBPORT_ETW_EVENT_CODE_EXECUTION_TIME))   { \
                X.StartTime = USBPort_GetTimeStampInMicroSeconds();\
                X.EndTime.QuadPart = 0;\
                X.Delta.QuadPart = 0;\
        }\
    } WHILE(0)

#define ETW_MEASURE_TIME_STOP(X)    \
    do  {   \
        if(EtwPerfLoggingEnabled && EtwEventEnabled(EtwRegHandle, &USBPORT_ETW_EVENT_CODE_EXECUTION_TIME))   { \
            if (X.StartTime.QuadPart > 0) {\
                X.EndTime = USBPort_GetTimeStampInMicroSeconds();\
                NT_ASSERTMSG("End time is less than start time!", X.EndTime.QuadPart >= X.StartTime.QuadPart);\
                X.Delta.QuadPart = X.EndTime.QuadPart - X.StartTime.QuadPart;\
            }\
         }\
    } WHILE(0)

#define ETW_STOP_AND_LOG_TIME(FDO, X, TAG) \
    do  {   \
        if(EtwPerfLoggingEnabled && EtwEventEnabled(EtwRegHandle, &USBPORT_ETW_EVENT_CODE_EXECUTION_TIME))   { \
            if (X.StartTime.QuadPart > 0) {\
                UNICODE_STRING tag;\
                X.EndTime = USBPort_GetTimeStampInMicroSeconds();\
                NT_ASSERTMSG("End time is less than start time!", X.EndTime.QuadPart >= X.StartTime.QuadPart);\
                X.Delta.QuadPart = X.EndTime.QuadPart - X.StartTime.QuadPart;\
                if(NT_SUCCESS(RtlUnicodeStringInit(&tag, L#TAG)))    {\
                    (VOID) USBPORT_EtwWriteCodeExecutionTime(FDO, &X, NULL, tag.Buffer, tag.Length);\
                }\
            }\
         }\
    } WHILE(0)

extern PKBUGCHECK_REASON_CALLBACK_RECORD USBPORT_BugCheckLog;
extern PKBUGCHECK_REASON_CALLBACK_RECORD USBPORT_BugCheckData;
extern PKBUGCHECK_REASON_CALLBACK_RECORD USBPORT_BugCheckExtData;

KBUGCHECK_REASON_CALLBACK_ROUTINE USBPORT_BugCheckSaveLog;
KBUGCHECK_REASON_CALLBACK_ROUTINE USBPORT_BugCheckSaveData;
KBUGCHECK_REASON_CALLBACK_ROUTINE USBPORT_BugCheckSaveExtData;

extern PDEVICE_OBJECT USBPORT_CurrentFdo;

#define USBPORT_InitAllBugChecks()\
    USBPORT_ASSERT(USBPORT_BugCheckLog == NULL);\
    USBPORT_BugCheckLog = USBPORT_InitBugCheck(USBPORT_BugCheckSaveLog);\
    USBPORT_ASSERT(USBPORT_BugCheckData == NULL);\
    USBPORT_BugCheckData = USBPORT_InitBugCheck(USBPORT_BugCheckSaveData);\
    USBPORT_ASSERT(USBPORT_BugCheckExtData == NULL);\
    USBPORT_BugCheckExtData = USBPORT_InitBugCheck(USBPORT_BugCheckSaveExtData);\
    USBPORT_CurrentFdo = NULL;

#define USBPORT_FreeAllBugChecks()\
    USBPORT_FreeBugCheck(USBPORT_BugCheckLog);\
    USBPORT_FreeBugCheck(USBPORT_BugCheckData);\
    USBPORT_FreeBugCheck(USBPORT_BugCheckExtData);\

typedef
NTSTATUS
(*PFN_IO_GET_ACTIVITY_ID_IRP) (
    __in
        PIRP    Irp,
    __out
        LPGUID  Guid
    );

PFN_IO_GET_ACTIVITY_ID_IRP      g_IoGetActivityIdIrp;

FORCEINLINE
NTSTATUS
USBPORT_GetActivityIdIrp (
    __in
        PIRP    Irp,
    __out
        LPGUID  Guid
    )
/*++

Routine Description:

    Wrapper for IoGetActivityIdIrp

Returns:

    NTSTATUS

--*/
{
    NTSTATUS    status;

    if (g_IoGetActivityIdIrp == NULL) {
        status = STATUS_NOT_FOUND;
    } else {
        status = g_IoGetActivityIdIrp(Irp, Guid);
    }

    return status;
}


LARGE_INTEGER
USBPort_GetPerformanceCounter(
        __out_opt PLARGE_INTEGER PerformanceFrequency
        );

ULARGE_INTEGER
USBPort_GetTimeStampInMicroSeconds();

IO_WORKITEM_ROUTINE_EX USBPORT_HcResetWI;

/*

USBPORT Locks

-----------------------------------------------------------------------------------------------------
KSPIN_LOCK
-----------------------------------------------------------------------------------------------------

<GLOBAL (all controllers)>
USBPORT_GlobalsSpinLock;

<FDO>
RhPdoPtrSpin;
RootHubDpcSpin;
BusContextSpin;
HcSyncSpin;
WakeIrpSpin;
SspIdleIrpSpin;
SspStateSpin;
CommonBufferListSpin;
DM_TimerSpin;
EndpointListSpin;
DevHandleListSpin;
GlobalTtSpin;
CoreFunctionSpin;
MpRootHubFunctionSpin;
IsrDpcSpin;
StatCounterSpin;
PowerSpin;

<DEVOBJ> FDO,PDO
IoRequestSpin;

----------------------------------------------------------------------------------------------------
KSEMAPHORE
----------------------------------------------------------------------------------------------------

.BusLock;
.DeviceCfgLock;
.Usb2bwLock
.MpSuspendResumeLock


-----------------------------------------------------------------------------------------------------
KSPIN_LOCK PRIORITY
-----------------------------------------------------------------------------------------------------


FDO.BusContextSpin              32
FDO.MpRootHubFunctionSpin       32

FDO.DevHandleListSpin           16
FDO.GlobalTtSpin                16
FDO.RootHubDpcSpin              16

FDO.PowerSpin                   8       (USBPORT_ACQUIRE_POWER_LOCK)
FDO.EndpointListSpin            8
FDO.DM_TimerSpin                8       (USBPORT_ACQUIRE_DM_LOCK)

FDO.CoreFunctionSpin            4
FDO,PDO.IoRequestSpin           2
FDO.IsrDpcSpin                  1


todo (rank these)
----
FDO.RhPdoPtrSpin;
FDO.RootHubDpcSpin;
FDO.HcSyncSpin;
FDO.WakeIrpSpin;
FDO.SspIdleIrpSpin;
FDO.SspStateSpin;
FDO.CommonBufferListSpin;
FDO.StatCounterSpin;
FDO.PowerSpin;
USBPORT_GlobalsSpinLock;


-----------------------------------------------------------------------------------------------------
LOCK USAGE
-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------
DevHandleListSpin
(FDO)
protects state changes on device handles share the same lock.

-----------------------------------------------------------------------------------------------------
GlobalTtSpin
(FDO)
protects the tt reference lists stored in each tt. All TTs share the same lock.

-----------------------------------------------------------------------------------------------------
DeviceCfgLock

-----------------------------------------------------------------------------------------------------
Usb2bwLock (sem)
(FDO)
serialzes access thru the USB2 budgeter (bandwidth allocations for USB2).

-----------------------------------------------------------------------------------------------------
BusContextSpin
(FDO)
protects access to bus context structure. all buscontext share the same lock

-----------------------------------------------------------------------------------------------------
EndpointListSpin
(FDO)
protects the global list of endpoints - GlobalEndpointList
protects endpoint reference list
-----------------------------------------------------------------------------------------------------
RootHubDpcSpin
(FDO)
serializes access to root hub data structures and Dpc for the root hub interrupt pipe.
-----------------------------------------------------------------------------------------------------
IsrDpcSpin
Synchronizes the ISR DPC with other DPC routines.

-----------------------------------------------------------------------------------------------------
MpRootHubFunctionSpin
(FDO)
serializes async timer callbacks

-----------------------------------------------------------------------------------------------------
PowerSpin
(FDO)
protects changes thru USBPORT_SetHcFdoPowerState

-----------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------
DM_TimerSpin
(FDO)
protects changes to flags in the DM_timer Dpc

also protects:
MpStateFlags
DmTimerFlags

-----------------------------------------------------------------------------------------------------
*****************************************************************************************************

*/

#ifdef USB_FORCE_MEMORY_STRESS
extern ULONG NumberOfResourceAllocations;
// fail one allocation out of 512
#define FP_FAIL_RESOURCE_ALLOCATION(devext, always) \
    (devext->Fdo.InPathCount && \
     ((NumberOfResourceAllocations++, (always)) ||!(NumberOfResourceAllocations&0x1ff)))
#endif


// {498C044A-201B-4631-A522-5C744ED4E678}
// 0 - set IOC for the last TD of a transfer only;
//     more efficient on power/slightly lower performance
// 1 - set IOC for all TDs;
//     better performance/less efficient on power
DEFINE_GUID(GUID_POWER_INTERRUPT_PER_TD_ENABLE,
0x498c044a, 0x201b, 0x4631, 0xa5, 0x22, 0x5c, 0x74, 0x4e, 0xd4, 0xe6, 0x78);

#endif /*  __USBPORT_H__ */
