/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    pcip.h

Abstract:

    This module contains local definitions for PCI.SYS.









--*/

//
// Disable warnings of features used by the standard headers
//
// Disable warning C4214: nonstandard extension used : bit field types other than int
// Disable warning C4201: nonstandard extension used : nameless struct/union
// Disable warning C4115: named type definition in parentheses
// Disable warning C4127: conditional expression is constant
//
#pragma warning(disable:4214 4201 4115 4127)

#define InitSafeBootMode TempInitSafeBootMode
#ifndef _NTDDK_
#include "ntosp.h"
#endif
#undef InitSafeBootMode

#include "ntstrsafe.h"
#include "ntintsafe.h"
#include "wdmguid.h"
#include "wdmsec.h"
#include "zwapi.h"
#include "arbiter.h"
#include "acpiioct.h"
#include "pcidevctrl.h"
#include "pciintrf.h"
#include "pcicodes.h"
#include "acpitabl.h"
#include "vffailure.h"
#include "halpnpp.h"
#include <winmeta.h>
#include <TraceLoggingProvider.h>
#include <telemetry\microsofttelemetry.h>
#include <WppRecorder.h>

#include "buspower.h"
#include "root.h"
#include "resizablebar.h"

//
// PREfast for Drivers Analysis Mode
//
__kernel_driver;

//
// Disable warnings that prevent PCI from compiling /W4
//
// Disable warning C4057; X differs in indirection to slightly different base types from Y
// Disable warning C4244; '+='/'-=' : conversion from 'int' to 'X', possible loss of data
//
#pragma warning(disable:4057 4244)

VOID
KdAcquireDebuggerLock (
    __in PKIRQL OldIrql
    );

VOID
KdReleaseDebuggerLock (
    KIRQL OldIrql
    );

///////////////////////////////////////////////////////////////////////////////
// Hardware Definitions                                                      //
///////////////////////////////////////////////////////////////////////////////

//
// This is the config space headers defined by the 2.2 PCI spec.
// Note for type0 and type1 headers the length is 0x40, for type2 it is
// 0x48 bytes.
//
// We do not use PCI_COMMON_CONFIG from ntos\inc\pci.h because it includes the
// 192 bytes of space allocated for device specific usage and does not include
// the extra 8 bytes of cardbus controller info.
//

//
// PCI Device
//
#define PCI_TYPE0_BAR_COUNT     6
#define PCI_TYPE0_HEADER_LENGTH 0x40

typedef struct _PCI_TYPE0 {
    ULONG   BaseAddresses[PCI_TYPE0_BAR_COUNT];
    ULONG   CIS;
    USHORT  SubVendorID;
    USHORT  SubSystemID;
    ULONG   ROMBaseAddress;
    UCHAR   CapabilitiesPtr;
    UCHAR   Reserved1[3];
    ULONG   Reserved2;
    UCHAR   InterruptLine;
    UCHAR   InterruptPin;
    UCHAR   MinimumGrant;
    UCHAR   MaximumLatency;
} PCI_TYPE0, *PPCI_TYPE0;

//
// PCI to PCI Bridge
//
#define PCI_TYPE1_BAR_COUNT     2
#define PCI_TYPE1_HEADER_LENGTH 0x40

#define PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT    0x100000
#define PCI_TYPE1_MEMORY_WINDOW_ADDRESS_MASK ~(PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT-1)
#define PCI_TYPE1_IO_WINDOW_ALIGNMENT        0x1000
#define PCI_TYPE1_IO_WINDOW_ADDRESS_MASK     ~(PCI_TYPE1_IO_WINDOW_ALIGNMENT-1)

#define PCI_TYPE1_WINDOW_DECODE_TYPE_MASK    0xf
#define PCI_TYPE1_IO_WINDOW_DECODE_32BIT     0x1
#define PCI_TYPE1_MEMORY_WINDOW_DECODE_64BIT 0x1

typedef struct _PCI_TYPE1 {
    ULONG   BaseAddresses[PCI_TYPE1_BAR_COUNT];
    UCHAR   PrimaryBus;
    UCHAR   SecondaryBus;
    UCHAR   SubordinateBus;
    UCHAR   SecondaryLatency;
    UCHAR   IOBase;
    UCHAR   IOLimit;
    USHORT  SecondaryStatus;
    USHORT  MemoryBase;
    USHORT  MemoryLimit;
    USHORT  PrefetchBase;
    USHORT  PrefetchLimit;
    ULONG   PrefetchBaseUpper32;
    ULONG   PrefetchLimitUpper32;
    USHORT  IOBaseUpper16;
    USHORT  IOLimitUpper16;
    UCHAR   CapabilitiesPtr;
    UCHAR   Reserved1[3];
    ULONG   ROMBaseAddress;
    UCHAR   InterruptLine;
    UCHAR   InterruptPin;
    USHORT  BridgeControl;
} PCI_TYPE1, *PPCI_TYPE1;

//
// PCI to CARDBUS Bridge
//

#define PCI_TYPE2_HEADER_LENGTH 0x48

#define PCI_TYPE2_MEMORY_WINDOW_ALIGNMENT     0x1000
#define PCI_TYPE2_MEMORY_WINDOW_ADDRESS_MASK  ~(PCI_TYPE2_MEMORY_WINDOW_ALIGNMENT-1)
#define PCI_TYPE2_IO_WINDOW_ALIGNMENT     0x4
#define PCI_TYPE2_IO_WINDOW_ADDRESS_MASK  ~(PCI_TYPE2_IO_WINDOW_ALIGNMENT-1)

typedef struct _PCI_TYPE2 {
    ULONG   SocketRegistersBaseAddress;
    UCHAR   CapabilitiesPtr;
    UCHAR   Reserved;
    USHORT  SecondaryStatus;
    UCHAR   PrimaryBus;
    UCHAR   SecondaryBus;
    UCHAR   SubordinateBus;
    UCHAR   SecondaryLatency;
    ULONG   MemoryBase0;
    ULONG   MemoryLimit0;
    ULONG   MemoryBase1;
    ULONG   MemoryLimit1;
    ULONG   IoBase0;
    ULONG   IoLimit0;
    ULONG   IoBase1;
    ULONG   IoLimit1;
    UCHAR   InterruptLine;
    UCHAR   InterruptPin;
    USHORT  BridgeControl;
    //
    // These extend past the normal 0x40 byte header
    //
    USHORT  SubVendorID;
    USHORT  SubSystemID;
    ULONG   LegacyModeBaseAddress;
} PCI_TYPE2, *PPCI_TYPE2;

//
// Common structures/defines
//

#define PCI_MAX_BAR_COUNT             PCI_TYPE0_BAR_COUNT
#define PCI_MAX_CONFIG_TYPE           PCI_CARDBUS_BRIDGE_TYPE
#define PCI_TYPE0_OR_1_HEADER_LENGTH  0x40

#define PCI_DECODE_ENABLES (PCI_ENABLE_MEMORY_SPACE    \
                            | PCI_ENABLE_IO_SPACE      \
                            | PCI_ENABLE_BUS_MASTER)

#define PCI_CONTROLLED_COMMAND_BITS (PCI_DECODE_ENABLES | PCI_DISABLE_LEVEL_INTERRUPT)

//
// These are valid for all header types
//
typedef struct _PCI_TYPE_COMMON {
    UCHAR   Reserved1[FIELD_OFFSET(PCI_TYPE0, InterruptLine)];
    UCHAR   InterruptLine;
    UCHAR   InterruptPin;
    UCHAR   Reserved2[2];
} PCI_TYPE_COMMON, *PPCI_TYPE_COMMON;

//
// These are valid for PCI-PCI and PCI-Cardbus bridges
//
typedef struct _PCI_TYPE_BRIDGE_COMMON {
    UCHAR   Reserved1[FIELD_OFFSET(PCI_TYPE1, PrimaryBus)];
    UCHAR   PrimaryBus;
    UCHAR   SecondaryBus;
    UCHAR   SubordinateBus;
    UCHAR   SecondaryLatency;
    UCHAR   Reserved2[FIELD_OFFSET(PCI_TYPE1, InterruptLine) - FIELD_OFFSET(PCI_TYPE1, IOBase)];
    UCHAR   InterruptLine;
    UCHAR   InterruptPin;
    USHORT  BridgeControl;
} PCI_TYPE_BRIDGE_COMMON, *PPCI_TYPE_BRIDGE_COMMON;

//
// Make sure these structures stay the same
//
C_ASSERT(sizeof(PCI_TYPE_COMMON) == sizeof(PCI_TYPE0));
C_ASSERT(sizeof(PCI_TYPE_COMMON) == sizeof(PCI_TYPE1));
C_ASSERT(sizeof(PCI_TYPE_COMMON) == FIELD_OFFSET(PCI_TYPE2, SubVendorID));
C_ASSERT(sizeof(PCI_TYPE_COMMON) == sizeof(PCI_TYPE_BRIDGE_COMMON));
C_ASSERT(FIELD_OFFSET(PCI_TYPE_COMMON,InterruptLine) ==
         FIELD_OFFSET(PCI_TYPE_BRIDGE_COMMON,InterruptLine));
C_ASSERT(FIELD_OFFSET(PCI_TYPE1,InterruptLine) ==
         FIELD_OFFSET(PCI_TYPE2,InterruptLine));
C_ASSERT(FIELD_OFFSET(PCI_TYPE1,InterruptLine) ==
         FIELD_OFFSET(PCI_TYPE_BRIDGE_COMMON,InterruptLine));

typedef struct _PCI_CONFIG_HEADER {
    USHORT  VendorID;                   // (ro)
    USHORT  DeviceID;                   // (ro)
    USHORT  Command;                    // Device control
    USHORT  Status;
    UCHAR   RevisionID;                 // (ro)
    UCHAR   ProgIf;                     // (ro)
    UCHAR   SubClass;                   // (ro)
    UCHAR   BaseClass;                  // (ro)
    UCHAR   CacheLineSize;              // (ro+)
    UCHAR   LatencyTimer;               // (ro+)
    UCHAR   HeaderType;                 // (ro)
    UCHAR   BIST;                       // Built in self test

    union {
        PCI_TYPE0 type0;
        PCI_TYPE1 type1;
        PCI_TYPE2 type2;
        PCI_TYPE_COMMON common;
        PCI_TYPE_BRIDGE_COMMON bridgeCommon;
    };
} PCI_CONFIG_HEADER, *PPCI_CONFIG_HEADER;

//
// Macros for manipulating config space buffers.
//
#ifndef FIELD_SIZE
    #define FIELD_SIZE(type, field) (sizeof(((type *)0)->field))
#endif

//
// This macro computes if a range of bytes with configuration
// space from offset for length bytes will intersect with the
// any of the fields between field1 and field2 as defined in
// PCI_CONFIG_HEADER
//
#define INTERSECT_CONFIG_FIELD_RANGE(offset, length, field1, field2)    \
    INTERSECT((offset),                                                 \
              (offset) + (length) - 1,                                  \
              FIELD_OFFSET(PCI_CONFIG_HEADER, field1),                  \
              FIELD_OFFSET(PCI_CONFIG_HEADER, field2)                   \
                + FIELD_SIZE(PCI_CONFIG_HEADER, field2) - 1             \
              )
//
// This macro computes if a range of bytes with configuration
// space from offset for length bytes will intersect with
// field as defined in PCI_CONFIG_HEADER
//
#define INTERSECT_CONFIG_FIELD(offset, length, field)                   \
    INTERSECT_CONFIG_FIELD_RANGE(offset, length, field, field)

//
// These macros give the offset/length of a field inside the
// configuration space header.
//
#define CONFIG_FIELD_OFFSET(field)                                      \
    FIELD_OFFSET(PCI_CONFIG_HEADER, field)

#define CONFIG_FIELD_LENGTH(field)                                      \
    FIELD_SIZE(PCI_CONFIG_HEADER, field)

//
// These macros give the offset/length of a field inside the
// SR-IOV capability relative to the start of config space.
//
#define SRIOV_FIELD_OFFSET(device, field)                               \
    FIELD_OFFSET(PCI_EXPRESS_SRIOV_CAPABILITY, field)                   \
    + (device)->ExpressPort->SriovCapability

#define SRIOV_FIELD_LENGTH(device, field)                               \
    FIELD_SIZE(PCI_EXPRESS_SRIOV_CAPABILITY, field)

//
// Waiting period after applying power to a bus before completing D0 IRPs
// for child devices, in milliseconds.  The Conventional spec says 100ms at
// 33Mhz.  The Express spec says 100ms after the link comes up, which is not
// deterministically detectable.  100ms is, empirically, too small for a lot
// of devices.
//
#define PORT_POWER_ON_SETTLE_TIME      150
#define DEVICE_LINK_ON_SETTLE_TIME     100
#define DEVICE_DSTATE_CHANGE_LIMIT     100

///////////////////////////////////////////////////////////////////////////////
// Device Extensions                                                         //
///////////////////////////////////////////////////////////////////////////////

//
// ---------------------------------------------------------- Common Structures
//

typedef struct _PCI_SEGMENT PCI_SEGMENT, *PPCI_SEGMENT;
typedef struct _PCI_ROOT PCI_ROOT, *PPCI_ROOT;
typedef struct _PCI_BUS PCI_BUS, *PPCI_BUS;
typedef struct _PCI_DEVICE PCI_DEVICE, *PPCI_DEVICE;
typedef struct _ROOT_COMPLEX ROOT_COMPLEX, *PROOT_COMPLEX;
typedef struct _SWITCH_COMPLEX SWITCH_COMPLEX, *PSWITCH_COMPLEX;
typedef struct _EXPRESS_LINK EXPRESS_LINK, *PEXPRESS_LINK;
typedef struct _EXPRESS_BRIDGE EXPRESS_BRIDGE, *PEXPRESS_BRIDGE;
typedef struct _EXPRESS_COMPATIBILITY_PORT EXPRESS_COMPATIBILITY_PORT, *PEXPRESS_COMPATIBILITY_PORT;
typedef struct _EXPRESS_PORT EXPRESS_PORT, *PEXPRESS_PORT;
typedef struct _PCI_HOTPLUG_SLOT PCI_HOTPLUG_SLOT, *PPCI_HOTPLUG_SLOT;
typedef struct _PCI_HOTPLUG_CONTROLLER PCI_HOTPLUG_CONTROLLER, *PPCI_HOTPLUG_CONTROLLER;

#define PCI_DEVICE_OBJECT(_DeviceObject) \
    ((_DeviceObject)->DriverObject == PciDriverObject)

#define PCI_BUS_EXTENSION(_Extension)     \
    ((_Extension != NULL) &&              \
     (((PPCI_COMMON_EXTENSION)(_Extension))->Signature == PCI_BUS_SIGNATURE))

#define PCI_DEVICE_EXTENSION(_Extension)     \
    ((_Extension != NULL) &&                 \
     (((PPCI_COMMON_EXTENSION)(_Extension))->Signature == PCI_DEVICE_SIGNATURE))

#define PCI_ROOT_BUS(_Bus)      \
    (!((_Bus)->PciBridge))

#define PCI_PCI_BRIDGE(_Bus)        \
    ((_Bus)->PciBridge && ((_Bus)->PciBridge->HeaderType == PCI_BRIDGE_TYPE))

#define PCI_CARDBUS_BRIDGE(_Bus)    \
    ((_Bus)->PciBridge && ((_Bus)->PciBridge->HeaderType == PCI_CARDBUS_BRIDGE_TYPE))

#define PCI_HOST_BRIDGE(_Device)    \
    (((_Device)->BaseClass == PCI_CLASS_BRIDGE_DEV) && ((_Device)->SubClass == PCI_SUBCLASS_BR_HOST))

#define PCI_SURFACEBOOK_GPU(_Device) \
            ((_Device->VendorID == 0x10de) && \
             (_Device->DeviceID == 0x134b) && \
             (_Device->RevisionID == 0xa2) && \
             (_Device->SubVendorID == 0x1414))

#define PCI_DEVICE_SIGNATURE 'DicP'
#define PCI_BUS_SIGNATURE    'BicP'
#define PCI_CONTROLDEVICE_SIGNATURE 'CicP'

//
// Values for WakeIrpState
//
typedef enum _PCI_WAKE_IRP_STATE {
    PciWakeIrpNone = 0,
    PciWakeIrpProcessing,
    PciWakeIrpQueued,
    PciWakeIrpCancelled,
    PciWakeIrpCompleted,
    PciWakeIrpMaximum
} PCI_WAKE_IRP_STATE, *PPCI_WAKE_IRP_STATE;

//
// Values for S0IrpState
//

#define PciS0IrpNormalCompletion    0
#define PciS0IrpDelayCompletion     1
#define PciS0IrpDelayAckAndRelease  2

//
// Device Usage
//
typedef struct _PCI_DEVICE_USAGE {
    LONG Paging;
    LONG Hibernate;
    LONG CrashDump;
} PCI_DEVICE_USAGE, *PPCI_DEVICE_USAGE;

typedef struct _PCI_COMMON_EXTENSION {

    ULONG Signature;
    PCI_OBJECT_STATE DeviceState;
    PCI_OBJECT_STATE TentativeNextState;
    PCI_DEVICE_USAGE DeviceUsage;

} PCI_COMMON_EXTENSION, *PPCI_COMMON_EXTENSION;

//
// ---------------------------------------------------------------- Delay Info
//

//
// Enumeration that defines the type of delay a bridge may request
//

typedef enum _PCI_DELAY_TYPE {

    //
    // Number of microseconds PCI should wait after a Conventional Reset,
    // before issuing the first Configuration Access.
    //

    ResetTime = 0,

    //
    // Number of microseconds that PCI should wait after the Downstream
    // Port above this device reports Data Link Layer Link Active,
    // before issuing the first Configuration Access.

    DLUpTime = 1,

    //
    // Number of microseconds PCI should wait after an FLR
    //

    FLRResetTime = 2,

    //
    // Number of microseconds PCI should wait on D3hot-to-D0 transitions,
    // before issuing the first request
    //

    D3hotToD0Time = 3,

    //
    // Number of microseconds PCI should wait after setting the VF Enable
    // bit, before issuing the first request
    //

    VFEnableTime = 4,

    //
    // Number of microseconds PCI should wait for pending PCI Express transactions
    // to complete.
    //

    ExpressTransactionTime = 5,

    //
    // Number of microseconds PCI should wait for a device to settle during
    // a hard reset after applying a secondary bus reset to it.
    //

    SecondaryBusResetTime = 6,

    //
    // Max enum value
    //

    DelayMax

} PCI_DELAY_INFO;

// 50 ms
#define EXPRESS_TRANSACTION_TIMEOUT (50*1000)

//
// Macro to initialize the delay structure.  These
// values were taken from the PCI SIG's ECN titled
// "ACPI additions for FW latency optimizations" and
// are reprsented in microseconds.
//

#define PCI_DEVICE_INITIALIZE_DELAYS(_DELAY) \
{                                            \
    (_DELAY)[ResetTime] = (100 * 1000);       \
    (_DELAY)[DLUpTime] = (150 * 1000);        \
    (_DELAY)[FLRResetTime] = (100 * 1000);    \
    (_DELAY)[D3hotToD0Time] = (100 * 1000);    \
    (_DELAY)[VFEnableTime] = (100 * 1000);    \
    (_DELAY)[ExpressTransactionTime] = EXPRESS_TRANSACTION_TIMEOUT; \
    (_DELAY)[SecondaryBusResetTime] = (500 * 1000); \
}

typedef
_Function_class_(BRIDGE_DELAY)
_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
BRIDGE_DELAY (
    _In_ PVOID Context,
    _In_ PCI_DELAY_INFO Delay
    );

typedef BRIDGE_DELAY *PBRIDGE_DELAY;

typedef
_Function_class_(BRIDGE_STALL)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
BRIDGE_STALL (
    _In_ PVOID Context,
    _In_ ULONG MicroSeconds
    );

typedef BRIDGE_STALL *PBRIDGE_STALL;

//
// ---------------------------------------------------------------- Pci Segment
//

#define LOCAL_BUS_NUMBER(_FullBusNumber_)                       \
    ((_FullBusNumber_) & 0xFF)

#define FULL_BUS_NUMBER(_SegmentNumber_, _LocalBusNumber_)      \
    (((_SegmentNumber_) << 8) | (_LocalBusNumber_))

#define SEGMENT_NUMBER_FOR_BUS(_FullBusNumber_)         \
    (((_FullBusNumber_) >> 8) & 0xFFFF)

typedef struct _PCI_BRIDGE_REQUIREMENTS PCI_BRIDGE_REQUIREMENTS, *PPCI_BRIDGE_REQUIREMENTS;

//
// A PCI segment is a logical collection of 256 PCI buses.
// Within each segment, the bus numbers for the PCI buses must be unique.
// PCI buses in different segments are permitted to have overlapping bus number
// range. Thus, use of segment allows reuse of PCI bus numbers.
//
//
typedef struct _PCI_SEGMENT {

    PPCI_SEGMENT Next;

    //
    // SegmentNumber is the logical grouping identifier. It starts with 0.
    //

    USHORT SegmentNumber;

    //
    // Linked list of root buses in this segment
    //

    PPCI_BUS PciRootBusList;
    PPCI_BUS PciRootBusTail;

    BOOLEAN GuessedRootBusNumber;

    _At_(BusNumberMap.SizeOfBitMap, _Field_range_(==, 0x100))
    RTL_BITMAP BusNumberMap;
    ULONG      BusNumberMapBuffer[8];  // enough for 256 bits

    //
    // Array of pointers to bridge allocation hints for each bar.
    // May be used to override default bridge allocations as bridges
    // are enumerated.
    //

    PPCI_BRIDGE_REQUIREMENTS BridgeRequirementOverride[256];

} PCI_SEGMENT, *PPCI_SEGMENT;

//
// ------------------------------------------------------------------- Pci Root
//

typedef struct _PCI_ROOT_FLAGS {
    union {
        struct {
            ULONG DsmForDelayOnSxResumeEvaluated:1;
            ULONG DelayOnSxResume:1;
            ULONG Reserved:31;
        };
        ULONG FlagsAsULONG;
    };
} PCI_ROOT_FLAGS, *PPCI_ROOT_FLAGS;

#define PCI_ROOT_SET_FLAG(_Root, _Flag)  \
{                                        \
    PCI_ROOT_FLAGS _tempflags;           \
                                         \
    _tempflags.FlagsAsULONG = 0;         \
    _tempflags._Flag = 1;                \
                                         \
    InterlockedOr((volatile LONG*)&_Root->FlagsAsULONG, _tempflags.FlagsAsULONG);\
}

#define PCI_ROOT_CLEAR_FLAG(_Root, _Flag) \
{                                         \
    PCI_ROOT_FLAGS _tempflags;            \
                                          \
    _tempflags.FlagsAsULONG = (ULONG)-1;  \
    _tempflags._Flag = 0;                 \
                                          \
    InterlockedAnd((volatile LONG*)&_Root->FlagsAsULONG, _tempflags.FlagsAsULONG);\
}

typedef struct _PCI_ROOT {

    //
    // Next in a linked list of roots in this segment
    //
    PPCI_ROOT Next;

    //
    // Pointer back to our segment
    //
    PPCI_SEGMENT Segment;

    //
    // Pointer to our bus
    //
    PPCI_BUS Bus;

    //
    // Pointer to a root complex.
    // This pointer is non-NULL on systems with express hierarchies.
    //
    PROOT_COMPLEX RootComplex;

    //
    // The interface provided by our enumerator that allows us to access our
    // config space
    //
    PPCI_BUS_INTERFACE_STANDARD PciBusInterface;

    //
    // The hardware characteristics and operating mode of a PCI root bus.
    //

    PCI_ROOT_BUS_HARDWARE_CAPABILITY HardwareCapability;

    //
    // Bitmap indicating where bridges are configured under this root.
    // A bit set indicates a bridge has its secondary bus number there.
    //
    RTL_BITMAP  BridgeMap;
    ULONG       BridgeMapBuffer[9];  // enough for 256 bits, plus one buffer ULONG

    //
    // This structure contains a copy of the information assembled by the
    // PSHED which describes the platform's PCI Express error reporting
    //

    WHEA_ERROR_SOURCE_DESCRIPTOR ErrorSourceDescriptor;

    //
    // These variables are defined for PME event handling and thread management.
    //

    LIST_ENTRY PmeRootPortList;
    LIST_ENTRY PmeWaitWakeSendList;
    LIST_ENTRY PmeWaitWakeCompleteList;
    LIST_ENTRY PmeWaitWakeCancelList;
    LIST_ENTRY PmeD0SendList;
    LIST_ENTRY PmeD0RetireList;
    LIST_ENTRY PmeQueueWakeEventList;
    PKTHREAD PmeDispatcherThread;
    KSPIN_LOCK PmeListLock;
    KEVENT PmeEvent[PciRootPmeEventMaximum];

    //
    // Flags
    //
    PCI_ROOT_FLAGS;

} PCI_ROOT, *PPCI_ROOT;

//
// ----------------------------------------------------------------- Pci Bridge
//

//
// The PCI_BRIDGE structure contains fields that apply to any bridge that
// has both a primary and secondary PCI bus, that is, PCI-PCI bridges
// and cardbus bridges.
//

//
// Resources.
//
#define PCI_BRIDGE_VGA_RESOURCE_COUNT  3
#define PCI_BRIDGE_RESOURCE_COUNT      (PCI_BRIDGE_VGA_RESOURCE_COUNT+3)

typedef union _PCI_BRIDGE_RESOURCES {

    struct {
        CM_PARTIAL_RESOURCE_DESCRIPTOR MemoryWindow;
        CM_PARTIAL_RESOURCE_DESCRIPTOR PrefetchWindow;
        CM_PARTIAL_RESOURCE_DESCRIPTOR IoWindow;
        CM_PARTIAL_RESOURCE_DESCRIPTOR VgaRange[PCI_BRIDGE_VGA_RESOURCE_COUNT];
    };

    CM_PARTIAL_RESOURCE_DESCRIPTOR ByIndex[PCI_BRIDGE_RESOURCE_COUNT];

} PCI_BRIDGE_RESOURCES, *PPCI_BRIDGE_RESOURCES;

//
// Requirements.
//
typedef struct _PCI_BRIDGE_WINDOW_REQUIREMENT {
    BOOLEAN Disabled;
    ULONGLONG Size;
    ULONGLONG Alignment;
    ULONGLONG LengthBelow4GB;
} PCI_BRIDGE_WINDOW_REQUIREMENT, *PPCI_BRIDGE_WINDOW_REQUIREMENT;

typedef struct _PCI_BRIDGE_REQUIREMENTS {
    PCI_BRIDGE_WINDOW_REQUIREMENT MemoryWindow;
    PCI_BRIDGE_WINDOW_REQUIREMENT PrefetchWindow;
    PCI_BRIDGE_WINDOW_REQUIREMENT IoWindow;
} PCI_BRIDGE_REQUIREMENTS, *PPCI_BRIDGE_REQUIREMENTS;

typedef struct _PCI_BRIDGE {

    PPCI_BUS Bus;

    PCI_SLOT_NUMBER Slot;
    UCHAR HeaderType;

    PCI_BRIDGE_REQUIREMENTS MinimumRequirements;
    PCI_BRIDGE_REQUIREMENTS PreferredRequirements;
    PCI_BRIDGE_REQUIREMENTS FallbackRequirements;
    PCI_BRIDGE_REQUIREMENTS SubtractiveRequirements;
    PCI_BRIDGE_RESOURCES BiosConfiguration;
    PCI_BRIDGE_RESOURCES Resources;
    CM_PARTIAL_RESOURCE_DESCRIPTOR InterruptResource;

    union {
        struct {
            ULONG LegacyVgaBitSet:1;
            ULONG IsaBitSet:1;
            ULONG Subtractive:1;
        } Attributes;
        ULONG AttributesAsULONG;
    };

    struct {
        ULONG Reserved1:1;
        ULONG GotRequirements:1;
        ULONG IoWindowOnBootConfig:1;
        ULONG InterruptConnected:1;
        ULONG HyperTransportPath:1;
        ULONG MsiHtConversionPath:1;
        ULONG D3Causes66MHzB2:1;
        ULONG AvoidChildD1D2ForSld:1;
        ULONG DisableLinkPowerManagement:1;
        ULONG ExpressBaseVersion11OrGreater:1;
        ULONG SecondaryBusResetOnLinkStateChange:1;
        ULONG LinkDisableOnSlotPowerDown:1;
        ULONG DisableChildPowerManagement:1;
        ULONG DownstreamDeviceNotRemovable:1;
        ULONG EnableAllBridgeInterrupts:1;
        ULONG PathIsExpress:1;
        ULONG RootPortDomain:1;
        ULONG E2ETLPPrefixSupported:1;
        ULONG AtomicsRoutingSupported:1;
        ULONG Atomics32BitCompleterSupported:1;
        ULONG Atomics64BitCompleterSupported:1;
        ULONG Atomics128BitCompleterSupported:1;
        ULONG AcsCapIncorrectFormat:1;
        ULONG Reserved2:9;
    };

    UCHAR PciXCapability;
    UCHAR ExpressCapability;

    PEXPRESS_BRIDGE ExpressBridge;

    PBRIDGE_DELAY DelayPassive;
    PBRIDGE_STALL StallDispatch;
    PVOID InterfaceContext;

} PCI_BRIDGE, *PPCI_BRIDGE;

// ------------------------------------------------------------------ PCI Buses

//
// Bus 0 acts like a conventional bus, even if it's an Express root complex
// because there are conventional functions on it.  (It's actually usually
// moot, as a typical ACPI BIOS won't do anything related to wake at the root
// PCI bus device level, putting _PSW (or equivalent) methods on all the
// functions on bus 0 that are bridges and/or devices that might generate
// wake.)
//

#define PCI_BUS_USES_NATIVE_PME(_Bus_) \
    ((_Bus_->SecondaryInterface == PciExpress) && \
     (_Bus_->Root->HardwareCapability.OscControlGranted.u.ExpressNativePME != 0) && \
     !(PCI_ROOT_BUS(_Bus_)))

#define PCI_BUS_IS_EXPRESS_HOTPLUG_CONTROLLER(_Bus_) \
    (!PCI_ROOT_BUS(_Bus_) && \
     (_Bus_->PciBridge->ExpressBridge != NULL) && \
     (DOWNSTREAM_PORT(_Bus_->PciBridge->ExpressBridge) != FALSE) && \
     (_Bus_->PciBridge->ExpressBridge->Downstream.HotPlugController != NULL))

#define PCI_BUS_IS_THUNDERBOLT_BUS(_Bus_) \
    ((PCI_PCI_BRIDGE(_Bus_)) && \
     (_Bus_->PciBridge->ExpressBridge != NULL) && \
     (_Bus_->PciBridge->ExpressBridge->BridgeType == PciExpressUpstreamSwitchPort) && \
     (_Bus_->PciBridge->ExpressBridge->ThunderboltCapability != 0))

typedef struct _PCI_BUS_FLAGS {
    union {
        struct {
            ULONG AcquiredHPP:1;
            ULONG AcquiredHPX:1;
            ULONG ArbitersInitialized:1;
            ULONG IoArbiterVideoHackApplied:1;
            ULONG ExtendedConfigAvailable:1;
            ULONG IgnoreBootConfig:1;
            ULONG StartedAlready:1;
            ULONG NeedToScanAfterHibernate:1;
            ULONG BusPowerAvailable:1;
            ULONG FirstBusScanDone:1;
            ULONG RetiringChildD0Irps:1;
            ULONG ExpectingS0Irp:1;
            ULONG PepPoweredOff:1;
            ULONG WakeStatePowerOn:1;
            ULONG ArmedForWake:1;
            ULONG Removing:1;
            ULONG PmeEventDeferred:1;
            ULONG WakePowerReferenced:1;
            ULONG PmePowerReferenced:1;
            ULONG OnceContainedNoDevices:1;
            ULONG RequirementsChanged:1;
            ULONG StartedWithoutInf:1;
            ULONG BusmasterDisabledOnBoot:1;
            ULONG SettleTimeRequired:1;
            ULONG Reserved:8;
        };
        ULONG FlagsAsULONG;
    };
} PCI_BUS_FLAGS, *PPCI_BUS_FLAGS;

#define PCI_BUS_SET_FLAG(_Bus, _Flag)   \
{                                       \
    PCI_BUS_FLAGS _tempflags;           \
                                        \
    _tempflags.FlagsAsULONG = 0;        \
    _tempflags._Flag = 1;               \
                                        \
    InterlockedOr((volatile LONG*)&_Bus->FlagsAsULONG, _tempflags.FlagsAsULONG);\
}

#define PCI_BUS_CLEAR_FLAG(_Bus, _Flag) \
{                                       \
    PCI_BUS_FLAGS _tempflags;           \
                                        \
    _tempflags.FlagsAsULONG = (ULONG)-1;\
    _tempflags._Flag = 0;               \
                                        \
    InterlockedAnd((volatile LONG*)&_Bus->FlagsAsULONG, _tempflags.FlagsAsULONG);\
}

//
// _HPX method information
//

typedef struct _PCI_HPX_INFO {

#define HPX_TYPE_0_VALID 1
#define HPX_TYPE_1_VALID 2
#define HPX_TYPE_2_VALID 4

    UCHAR ValidTypes;

    //
    // Type 0: PCI Setting Record
    //

    struct _HPX_TYPE_0 {
        UCHAR CacheLineSize;
        UCHAR LatencyTimer;
        BOOLEAN EnableSERR;
        BOOLEAN EnablePERR;
    } type0;

/*
    //
    // Type 1: PCI-x Setting Record
    //

    struct _HPX_TYPE_1 {
        UCHAR MaxMemoryReadByteCount;
        UCHAR AverageMaxOutstandingSplitTransactions;
        UCHAR TotalMaxOutstandingSplitTransactions;
    } type1;
*/ 

    //
    // Type 2: PCI Express Setting Record
    //

#define PCI_HPX_AND_MASK 0
#define PCI_HPX_OR_MASK 1

    struct _HPX_TYPE_2 {
        ULONG UncorrectableErrorMask[2];
        ULONG UncorrectableErrorSeverity[2];
        ULONG CorrectableErrorMask[2];
        ULONG CapabilitiesAndControl[2];
        ULONG DeviceControl[2];
        ULONG LinkControl[2];
        ULONG SecUncorrectableErrorSeverity[2];
        ULONG SecUncorrectableErrorMask[2];
    } type2;
} PCI_HPX_INFO, *PPCI_HPX_INFO;

typedef struct _PCI_CHILDREN_BUSNUM_HINT {
    LIST_ENTRY ListEntry;
    PCI_SLOT_NUMBER Slot;
    UCHAR BusNumber;
    UCHAR Secondary;
    UCHAR Subordinate;
} PCI_CHILDREN_BUSNUM_HINT, *PPCI_CHILDREN_BUSNUM_HINT;

typedef struct _PCI_BUS {

    PCI_COMMON_EXTENSION;

    IO_REMOVE_LOCK RemoveLock;

    //
    // Bus Tree pointers - protected by PciGlobalLock
    //
    PPCI_BUS ParentBus;
    PPCI_BUS SiblingBus;
    PPCI_BUS ChildBuses;
    PPCI_BUS ChildBusTail;

    PPCI_ROOT Root;
    PPCI_BRIDGE PciBridge;  // Non-NULL if the bus is created by a PCI-PCI bridge

    //
    // Children enumerated on this bus
    //
    PPCI_DEVICE ChildDevices;
    PPCI_DEVICE ChildDeviceTail;
    KEVENT ChildDeviceLock;
    KEVENT ExpressLinkQuiescentEvent;
    RTL_BITMAP EvaluatedRidMap;
    ULONG EvaluatedRidMapBuffer[8]; // enough for 256 Routing IDs

    //
    // Device Object pointers
    //
    PDEVICE_OBJECT DeviceObject;
    PDEVICE_OBJECT AttachedDeviceObject;
    PDEVICE_OBJECT PhysicalDeviceObject;

    //
    // Hardware characteristics of secondary interface.
    //
    PCI_HARDWARE_INTERFACE SecondaryInterface;

    //
    // Bus Numbers (contain segment number)
    //
    ULONG       SecondaryBusNumber;
    ULONG       SubordinateBusNumber;
    _At_(BusNumberMap.SizeOfBitMap, _Field_range_(==, 0x100))
    RTL_BITMAP  BusNumberMap;
    ULONG       BusNumberMapBuffer[8];  // enough for 256 bits
    UCHAR       CapturedBusNumberCount; // captured for child SR-IOV devices

    //
    // Power
    //
    DEVICE_POWER_STATE      PowerState;
    DEVICE_POWER_STATE      SystemStateMapping[PowerSystemMaximum];
    SYSTEM_POWER_STATE      SystemWake;
    DEVICE_POWER_STATE      DeviceWake;     // D-state for wake from Sx
    DEVICE_WAKE_DEPTH       IdleDState;     // D-state for wake from S0
    __volatile LONG         DevicesPoweredDown;
    PIRP                    SIrp;
    _Guarded_by_(BusPowerLock)
    PIRP                    WakeIrp;
    BOOLEAN                 WokeSystem;
    LIST_ENTRY              WaitingForWaitStateMachine;
    LIST_ENTRY              WaitingForD0Send;
    LIST_ENTRY              WaitingForWaitWakeSend;
    LIST_ENTRY              WaitingForWaitWakeComplete;
    LIST_ENTRY              WaitingForWaitWakeCancel;
    LIST_ENTRY              WaitingForChildD0Retirement;
    KSPIN_LOCK              BusPowerLock;
    KSPIN_LOCK              BusWakeLock;
    LARGE_INTEGER           TimeOfPowerUp; // KeQPC units
    KEVENT                  BusPowerEvent;
    _Guarded_by_(BusPowerLock)
    PCI_BUS_POWER_STATES    BusPowerState;
    _Write_guarded_by_(BusPowerLock)
    USHORT                  ChildWakeCount;
    _Guarded_by_(BusWakeLock)
    union {
        ULONG                           BusWakeState;
        PCI_CONVENTIONAL_WAKE_STATES    BusWakeAsConventional;
        PCI_EXPRESS_WAKE_STATES         BusWakeAsExpress;
    };
    NTSTATUS                WakeIrpStatus;

    LONG                    BusPowerReferenceReason[PciBusPowerReasonMaximum];

    //
    // Arbiters
    //
    PARBITER_INSTANCE MemoryArbiter;
    PARBITER_INSTANCE IoArbiter;
    PARBITER_INSTANCE BusNumberArbiter;

    //
    // Flags
    //
    PCI_BUS_FLAGS;

    ULONG HackFlags;

    //
    // Parameters from _HPP method
    //
    struct {
        UCHAR CacheLineSize;
        UCHAR LatencyTimer;
        BOOLEAN EnableSERR;
        BOOLEAN EnablePERR;
    } _HPP;

    PCI_HPX_INFO _HPX;

    volatile LONG PciRoutingInterfaceCount;
    volatile LONG PciLegacyDeviceInterfaceCount;
    volatile LONG PciPmeInterfaceCount;
    volatile LONG PciLocationInterfaceCount;

    //
    // Power policy tracking
    //

    POHANDLE PepHandle;

    //
    // Tracing
    //

    RECORDER_LOG BlackBoxHandle;

    D3COLD_SUPPORT_INTERFACE D3coldIntrf;

    //
    // A list that hints the bus numbers of the child buses.
    //

    LIST_ENTRY  ChildrenBusNumHints;

} PCI_BUS, *PPCI_BUS;

//
// ----------------------------------------------------------------- Pci Device
//

// Conventional BARs plus VF BARs plus a ROM bar
#define PCI_DEVICE_RESOURCE_COUNT ((PCI_MAX_BAR_COUNT * 2) + 1)

//
// I/O and Memory Resources
//
typedef union _PCI_DEVICE_REQUIREMENTS {
    struct {
        //
        // BARs - ones that are not implemented or not available
        // on a particular device type will have CmResourceTypeNull;
        //
        IO_RESOURCE_DESCRIPTOR Bars[PCI_MAX_BAR_COUNT];

        union {
            struct {
                IO_RESOURCE_DESCRIPTOR RomBar;
                IO_RESOURCE_DESCRIPTOR VfBars[PCI_MAX_BAR_COUNT];
            } type0;

            struct {
                IO_RESOURCE_DESCRIPTOR RomBar;
                IO_RESOURCE_DESCRIPTOR MemoryWindow;
                IO_RESOURCE_DESCRIPTOR PrefetchWindow;
                IO_RESOURCE_DESCRIPTOR IoWindow;
            } type1;

            struct {
                IO_RESOURCE_DESCRIPTOR SocketBar;
                IO_RESOURCE_DESCRIPTOR LegacyBar;
                IO_RESOURCE_DESCRIPTOR MemoryWindows[2];
                IO_RESOURCE_DESCRIPTOR IoWindows[2];
            } type2;

        };  // unnamed union
    }; //unnamed struct

    IO_RESOURCE_DESCRIPTOR ByIndex[PCI_DEVICE_RESOURCE_COUNT];

} PCI_DEVICE_REQUIREMENTS, *PPCI_DEVICE_REQUIREMENTS;

typedef union _PCI_DEVICE_RESOURCES {
    struct {
        //
        // BARs - ones that are not implemented or not available
        // on a particular device type will have CmResourceTypeNull;
        //
        CM_PARTIAL_RESOURCE_DESCRIPTOR Bars[PCI_MAX_BAR_COUNT];

        union {
            struct {
                CM_PARTIAL_RESOURCE_DESCRIPTOR RomBar;
                CM_PARTIAL_RESOURCE_DESCRIPTOR VfBars[PCI_MAX_BAR_COUNT];
            } type0;

            struct {
                CM_PARTIAL_RESOURCE_DESCRIPTOR RomBar;
                CM_PARTIAL_RESOURCE_DESCRIPTOR MemoryWindow;
                CM_PARTIAL_RESOURCE_DESCRIPTOR PrefetchWindow;
                CM_PARTIAL_RESOURCE_DESCRIPTOR IoWindow;
            } type1;

            struct {
                CM_PARTIAL_RESOURCE_DESCRIPTOR SocketBar;
                CM_PARTIAL_RESOURCE_DESCRIPTOR LegacyBar;
                CM_PARTIAL_RESOURCE_DESCRIPTOR MemoryWindows[2];
                CM_PARTIAL_RESOURCE_DESCRIPTOR IoWindows[2];
            } type2;

        };  // unnamed union
    }; //unnamed struct

    //
    // The descriptors array can be used to loop through all the
    // I/O and memory descriptors.
    //
    CM_PARTIAL_RESOURCE_DESCRIPTOR ByIndex[PCI_DEVICE_RESOURCE_COUNT];

} PCI_DEVICE_RESOURCES, *PPCI_DEVICE_RESOURCES;

//
// Interrupts
//
typedef enum {
    PciNone = 0,
    PciLineBased,
    PciMsi,
    PciMsiX
} PCI_INTERRUPT_TYPE;

typedef struct _PCI_DEVICE_INTERRUPT_REQUIREMENT {

    IO_RESOURCE_DESCRIPTOR LineBased;

    struct {
        PCI_INTERRUPT_TYPE Type;
        ULONG MessagesRequested;
        IO_RESOURCE_DESCRIPTOR SingleMessage;

    } Message;

} PCI_DEVICE_INTERRUPT_REQUIREMENT, *PPCI_DEVICE_INTERRUPT_REQUIREMENT;

#define PCI_MSIX_ENTRY_MASKED 0x80000000

typedef struct _PCI_DEVICE_INTERRUPT_RESOURCE {

    PCI_INTERRUPT_TYPE Type;
    UCHAR InterruptLine;
    BOOLEAN PolicySpecified;

    union {
        struct {
            UCHAR GrantedBits;
            INTERRUPT_VECTOR_DATA ConnectionData;
        } Msi;

        struct {
            UCHAR BARIndex;
            ULONG TableOffset;
            PPCI_MSIX_TABLE_ENTRY Table;
            ULONG MessagesGranted;
            PULONG EntryMap;
            PINTERRUPT_CONNECTION_DATA ConnectionData;
        } MsiX;
    };

} PCI_DEVICE_INTERRUPT_RESOURCE, *PPCI_DEVICE_INTERRUPT_RESOURCE;

//
// Header Operations
//
typedef
NTSTATUS
(*PPCI_GET_REQUIREMENTS)(
    IN PPCI_DEVICE Device,
    OUT PPCI_DEVICE_REQUIREMENTS Requirements
    );

typedef
NTSTATUS
(*PPCI_GET_RESOURCES)(
    IN PPCI_DEVICE Device,
    OUT PPCI_DEVICE_RESOURCES Resources
    );

typedef
NTSTATUS
(*PPCI_SET_RESOURCES)(
    IN PPCI_DEVICE Device,
    IN PPCI_DEVICE_RESOURCES Resources
    );

typedef struct _PCI_HEADER_OPERATIONS {
    PPCI_GET_REQUIREMENTS GetRequirements;
    PPCI_GET_RESOURCES GetResources;
    PPCI_SET_RESOURCES SetResources;
} PCI_HEADER_OPERATIONS, *PPCI_HEADER_OPERATIONS;

//
// Debug flags
//
#if DBG
#define PCI_GOT_REQUIREMENTS 0x00000001
#define PCI_BRIDGE_DISABLED  0x00000002
#endif

#define EXPRESS_INVALID_CAPABILITY_VERSION          0x00000001
#define EXPRESS_INVALID_HIERARCHY                   0x00000002
#define EXPRESS_INVALID_EXTENDED_CAPABILITY         0x00000004
#define EXPRESS_INCOMPATIBLE_MAX_PAYLOAD_SIZE       0x00000008
#define EXPRESS_ALLOCATION_FAILED                   0x00000010

#define PCI_INVALID_DEVICE_ID_DATA                  0x00010000
#define PCI_DEVICE_POWER_FAILURE                    0x00020000
#define PCI_FAILED_START_RESOURCE_PROCESSING        0x00040000
#define PCI_FAILED_HARDWARE_UPDATE                  0x00080000

#define PciSetEnumFailureCode(_Device, _Flag)       \
    (_Device)->EnumFailureCode |= (_Flag)

//
// The test for whether a device is in D3cold depends on whether it is on a root
// PCI bus.  For root PCI buses, the assumption is that the bus itself is always
// on while the machine is on.  Devices on root PCI buses are assumed to be in
// D3cold if the device is in D3 *and* the firmware says that the device
// supports D3cold *and* the device policy says D3cold is okay.  For bridged PCI
// buses, the test for D3cold is about whether the bus itself has left D0 while
// all those other device conditions are true.
//

#define PCI_DEVICE_IN_D3COLD(_Device)                           \
    ((_Device->D3ColdSupport == D3ColdSupportEnabled) &&        \
     (_Device->LogicalPowerState == PowerDeviceD3) &&           \
     ((PCI_ROOT_BUS(_Device->Parent) ?                          \
       (_Device->FirmwareSaysDeviceDoesD3cold == 1) :           \
       (_Device->Parent->BusPowerAvailable == 0))))

//
// Registers to save/restore (along with the resources)
//
typedef struct _PCI_SAVE_RESTORE_DATA {

    USHORT Command;
    UCHAR  CacheLineSize;
    UCHAR  LatencyTimer;
    UCHAR  InterruptLine;
    struct {
        USHORT BridgeControl;
        UCHAR  SecondaryBusNumber;
        UCHAR  SubordinateBusNumber;
    } bridge;

} PCI_SAVE_RESTORE_DATA, *PPCI_SAVE_RESTORE_DATA;

typedef enum _PCI_D3COLD_SUPPORT {
    D3ColdSupportUninitialized = 0,
    D3ColdSupportDisabledInRegistry,
    D3ColdSupportDisabled,
    D3ColdSupportEnabled
} PCI_D3COLD_SUPPORT, *PPCI_D3COLD_SUPPORT;

typedef struct _PCI_DEVICE_FLAGS {
    union {
        struct {
            ULONGLONG ConfigureMwi:1;
            ULONGLONG BusNumbersConfigured:1;
            ULONGLONG OnDebugPath:1;
            ULONGLONG PciOwned:1;
            ULONGLONG LegacyDriver:1;
            ULONGLONG NativeIdeSwitchAllowed:1;
            ULONGLONG IdeInNativeMode:1;
            ULONGLONG IdeInterfaceActive:1;
            ULONGLONG Present:1;
            ULONGLONG ReportedMissing:1;
            ULONGLONG Connected:1;
            ULONGLONG PmeOwned:1;
            ULONGLONG PmeEnabled:1;
            ULONGLONG TriggeredPme:1;
            ULONGLONG HyperTransport:1;
            ULONGLONG MsiHtConverter:1;
            ULONGLONG ExtendedConfigAvailable:1;
            ULONGLONG B2On66MHzSegmentIllegal:1;
            ULONGLONG Device66MHzCapable:1;
            ULONGLONG DsmToIgnoreBootConfigEvaluated:1;
            ULONGLONG IgnoreBootConfig:1;
            ULONGLONG Removed:1;
            ULONGLONG AriIsEnabled:1;
            ULONGLONG MultiFunction:1;
            ULONGLONG AlreadyBeenStarted:1;
            ULONGLONG WaitingForPowerUp:1;
            ULONGLONG OnPostPath:1;
            ULONGLONG SriovDeviceDidntGetVfBarSpace:1;
            ULONGLONG ExtraBusPowerReferenceForWake:1;
            ULONGLONG FirmwareSaysDeviceDoesD3cold:1;
            ULONGLONG TransactionTimerSetPower:1;
            ULONGLONG SurpriseRemoveAndReenumerateSelf:1;
            ULONGLONG BusmasterDisabledOnBoot:1;
            ULONGLONG DsmForDelayOptimizationEvaluated:1;
            ULONGLONG NoInitialConfig:1;
            ULONGLONG DisableWheaDuringPLDR:1;
            ULONGLONG WheaReportingDisabled:1;
            ULONGLONG Reserved:27;
        };
        ULONGLONG FlagsAsULONGLONG;
    };
} PCI_DEVICE_FLAGS, *PPCI_DEVICE_FLAGS;

#define PCI_DEVICE_SET_FLAG(_Device, _Flag)   \
{                                       \
    PCI_DEVICE_FLAGS _tempflags;        \
                                        \
    _tempflags.FlagsAsULONGLONG = 0;    \
    _tempflags._Flag = 1;               \
                                        \
    InterlockedOr64((volatile LONGLONG *)&_Device->FlagsAsULONGLONG, _tempflags.FlagsAsULONGLONG);\
}

#define PCI_DEVICE_CLEAR_FLAG(_Device, _Flag) \
{                                       \
    PCI_DEVICE_FLAGS _tempflags;        \
                                        \
    _tempflags.FlagsAsULONGLONG = (ULONGLONG)-1;\
    _tempflags._Flag = 0;               \
                                        \
    InterlockedAnd64((volatile LONGLONG *)&_Device->FlagsAsULONGLONG, _tempflags.FlagsAsULONGLONG);\
}

typedef struct _PCI_DEVICE {

    PCI_COMMON_EXTENSION;

    //
    // The location of the device.
    //
    ULONG BusNumber;
    PCI_SLOT_NUMBER Slot;

    //
    // Device Identification data
    // [Read in at creation and then read only]
    //
    USHORT VendorID;
    USHORT DeviceID;
    UCHAR  RevisionID;
    UCHAR  BaseClass;
    UCHAR  SubClass;
    UCHAR  ProgIf;
    USHORT SubVendorID;
    USHORT SubSystemID;
    UCHAR  HeaderType;
    UCHAR  InterruptPin;

    //
    // Offsets of extended capabilities we support, 0 indicates the capability
    // is absent.
    //
    UCHAR FirstCapability;
    UCHAR PowerCapability;
    UCHAR SubIDsCapability;
    UCHAR AgpCapability;
    UCHAR PciXCapability;
    UCHAR AdvancedFeatures;
    UCHAR MsiCapability;
    UCHAR MsiXCapability;
    UCHAR FirstHtCapability;
    UCHAR HtMsiMapCapability;
    UCHAR ExpressCapability;

    USHORT PossibleDecodes;
    USHORT ResourceDecodes;
    USHORT PreviousCommand;

    //
    // Connections to device tree
    //
    PDEVICE_OBJECT DeviceObject;
    PPCI_ROOT      Root;
    PPCI_BUS       Parent;
    PPCI_DEVICE    Sibling;

    IO_REMOVE_LOCK RemoveLock;

    //
    // Pertains to the express portion of the device.
    //
    PEXPRESS_COMPATIBILITY_PORT ExpressCompatibilityPort;
    PEXPRESS_PORT ExpressPort;
    PPCI_HOTPLUG_SLOT HotPlugSlot;

    //
    // PCIe devices can have a serial number to uniquely
    // identify the device.  This is used to create the
    // device instance ID for the device.
    //
    // NOTE: This is read directly from the hardware the
    // first time the device is ever enumerated. This value
    // is then persisted in the registry for all subsequent
    // enumerations so long as the device identification
    // (Vendor ID, Device ID etc) all remain the same. This
    // allows a matching device replacement to occur without
    // losing all device specific settings.
    //
    ULONGLONG InstanceIdSerialNumber;

    //
    // Resources
    //
    PCI_DEVICE_RESOURCES Resources;
    PCI_DEVICE_REQUIREMENTS Requirements;
    PCI_DEVICE_INTERRUPT_RESOURCE InterruptResource;
    PCI_DEVICE_INTERRUPT_REQUIREMENT InterruptRequirement;
    PCI_SAVE_RESTORE_DATA SavedRegisters;
    ULONG CapturedProcessorCount;

    //
    // Power
    //
    DEVICE_POWER_STATE          LogicalPowerState;
    DEVICE_POWER_STATE          PreviousPowerState;
    DEVICE_POWER_STATE          WakeLevel;
    LONG                        PowerStateChangeDelay;
    struct _PM_SUPPORT          PowerSupport;
    KTIMER                      PowerTimer;
    KDPC                        PowerTimerDpc;
    PIRP                        DIrp;
    PIRP                        WakeIrp;
    PCI_WAKE_IRP_STATE          WakeIrpState;
    PCI_D3COLD_SUPPORT          D3ColdSupport;
    KTIMER                      TransactionTimer;
    KDPC                        TransactionTimerDpc;
    USHORT                      D0DelayInMilliseconds;
    UCHAR                       MpsSizeOverride:3;
    UCHAR                       MpsSizeOverrideSet:1;
    UCHAR                       TransactionTimeoutPasses;
    UCHAR                       DstateChangeRetryCount;
    D3COLD_SUPPORT_INTERFACE    D3ColdFilter;

    //
    // This lock is defined to enable completing a device Wait Wake Irp that
    // is cancelled before the dispatch routine sets a cancellation routine
    // for the Irp. This lock allows the cancel routine to busy wait until
    // the Irp is safely placed in the extension. It also safe guards the
    // dispatch routine from touching the Irp after the Irp has completed.
    //

    KSPIN_LOCK         WakeIrpLock;

    //
    // This lock is defined to allow only one thread to complete a D0 IRP
    //

    KSPIN_LOCK         D0IrpLock;

    //
    // Flags
    //

    PCI_DEVICE_FLAGS;

    //
    // Bridge Only Flags
    //
    struct {
        ULONG D3Illegal:1;
        ULONG D3Causes66MHzB2:1;
    } BridgeFlags;

    ULONGLONG HackFlags;

    //
    // Pointer to a global table of header operations appropriate for this type
    //
    PPCI_HEADER_OPERATIONS Operations;

    volatile LONG PciBusInterfaceCount;
    volatile LONG AgpTargetInterfaceRefCount;
    volatile LONG PciCardBusInterfaceCount;
    volatile LONG PciDevicePresentInterfaceCount;
    volatile LONG PciNativeIdeInterfaceCount;
    volatile LONG PciLocationInterfaceCount;
    volatile LONG PciD3ColdSupportInterfaceCount;

    ULONG EnumFailureCode;

#if DBG
    ULONG DebugFlags;
#endif

    //
    // Handle to debug device power callback registration.
    //

    HANDLE DebugDevicePowerCallbackHandle;

    //
    // Delay optimizations provided by FW.  This array
    // is indexed by the PCI_DELAY_INFO enum
    //

    LONG DelayInfo[DelayMax];

} PCI_DEVICE, *PPCI_DEVICE;

///////////////////////////////////////////////////////////////////////////////
// PCI Express extensions                                                    //
///////////////////////////////////////////////////////////////////////////////

typedef enum _PCI_HOTPLUG_COMMANDTYPE PCI_HOTPLUG_COMMAND_TYPE;

typedef struct _HOTPLUG_WORKER_PARAMETERS {

    PPCI_HOTPLUG_SLOT Slot;
    PCI_HOTPLUG_COMMAND_TYPE Type;
    ULONG_PTR CommandContext;

} HOTPLUG_WORKER_PARAMETERS, *PHOTPLUG_WORKER_PARAMETERS;

typedef enum {

    ExpressPmeRuntimeEvent = 1,
    ExpressPmePowerUpEvent

} EXPRESS_PME_EVENT_TYPE;

//
// A Root Complex as defined in the PCI Express Base specification denotes
// the root of an I/O hierarchy that connects the CPU/memory subsystem to
// the I/O. A PCI Express Root Complex may support one or more PCI Express
// Ports, commonly known as Root Ports, Root Complex Integrated Endpoints
// and optional Root Complex Event Collectors. Root Ports originate PCI
// Express hierarchy domain from a PCI Express Root Complex. A Root Complex
// as defined in the Express parlance can be found on every PCI Root Bus
// that originates Express hierarchies (except in the case where an Express
// hierarchy originates thru a PCI/PCI-X  to Express bridge. The term
// "Root Complex" must not be confused with its use to mean a logical
// assembly of multiple root buses.
//

typedef struct _ROOT_COMPLEX {

    PPCI_BUS PciRootBus;
    PEXPRESS_PORT IntegratedEndpointList;
    PEXPRESS_PORT EventCollectorList;
    PEXPRESS_BRIDGE RootPortList;

    //
    // This mutex lock protects various lists (defined above) in the root
    // complex. These lists correspond to different elements found in a root
    // complex. No other lock can be acquired after acquiring the root complex
    // list lock.
    //

    KEVENT ListLock;

} ROOT_COMPLEX, *PROOT_COMPLEX;

//
// A Switch is defined as a logical assembly of multiple Virtual PCI-PCI
// Bridge devices. A single virtual bridge represents an upstream switch
// port connection to the element above the switch in the hierarchy while
// one or more virtual bridges represent the downstream connections.
//

typedef struct _SWITCH_COMPLEX {

    PEXPRESS_BRIDGE UpstreamPort;
    PEXPRESS_BRIDGE DownstreamPortList;

    ULONG ComputedL0sReceiveLatency;
    ULONG ComputedL0sTransmitLatency;
    ULONG ComputedL1Latency;

    BOOLEAN IntegratedComponent;

} SWITCH_COMPLEX, *PSWITCH_COMPLEX;

//
// An express link represents a point-to-point connection between
// the Root Ports and Switch Downstream Ports as egress links and
// the non-integrated Endpoints, Upstream Switch Port and the
// Express to PCI/X bridges as ingress end of the links.
//

typedef struct _EXPRESS_LINK {

    PEXPRESS_LINK TargetLink;
    PCI_EXPRESS_LINK_CONTROL_REGISTER LinkControl;
    PCI_EXPRESS_LINK_CAPABILITIES_REGISTER LinkCapabilities;
    PCI_EXPRESS_LINK_STATUS_REGISTER LinkStatus;

    //
    // Fields to support ASPM.
    //

    BOOLEAN L0sSupported;
    BOOLEAN L1Supported;

    BOOLEAN L0sEnabled;
    BOOLEAN L1Enabled;

    ULONG L0sExitLatencyLow;
    ULONG L0sExitLatencyHigh;

    ULONG L1ExitLatencyLow;
    ULONG L1ExitLatencyHigh;

} EXPRESS_LINK, *PEXPRESS_LINK;

//
// ASPM debug flags
//

#define PCI_ASPM_L1_LATENCY  0x00000001
#define PCI_ASPM_L1_ERRATA   0x00000002
#define PCI_ASPM_L1_OPTOUT   0x00000004

#define PCI_ASPM_L0s_LATENCY 0x00010000
#define PCI_ASPM_L0s_ERRATA  0x00020000
#define PCI_ASPM_L0s_OPTOUT  0x00040000

#define DOWNSTREAM_PORT(__ExpressBridge) \
((__ExpressBridge->BridgeType == PciExpressRootPort) || \
 (__ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort))

//
// These bits are defined by the PCIe spec, as part of the DeviceCapabilities2
// and DeviceControl2 registers.
//

typedef enum _EXPRESS_PORT_OBFF_SUPPORT {
    ObffNotSupported = 0,
    ObffSupportedViaMessages,
    ObffSupportedViaWake,
    ObffSupportedViaBoth
} EXPRESS_PORT_OBFF_SUPPORT, *PEXPRESS_PORT_OBFF_SUPPORT;

typedef enum _EXPRESS_PORT_OBFF_CONTROL {
    ObffDisabled = 0,
    ObffEnabledUsingMessagesVariationA,
    ObffEnabledUsingMessagesVariationB,
    ObffEnabledUsingWake
} EXPRESS_PORT_OBFF_CONTROL, *PEXPRESS_PORT_OBFF_CONTROL;

typedef
_Function_class_(BRIDGE_ARI_FORWARDING_ENABLE)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
BRIDGE_ARI_FORWARDING_ENABLE (
    _In_ PVOID Context,
    _In_ BOOLEAN AriForwardingEnable
    );

typedef BRIDGE_ARI_FORWARDING_ENABLE *PBRIDGE_ARI_FORWARDING_ENABLE;

typedef
_Function_class_(BRIDGE_OBFF_CONTROL)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
BRIDGE_OBFF_CONTROL (
    _In_ PVOID Context,
    _In_ EXPRESS_PORT_OBFF_CONTROL ObffControl
    );

typedef BRIDGE_OBFF_CONTROL *PBRIDGE_OBFF_CONTROL;

typedef
_Function_class_(BRIDGE_GET_POWER_PARAMETERS)
_IRQL_requires_(PASSIVE_LEVEL)
VOID
BRIDGE_GET_POWER_PARAMETERS (
    _In_ PVOID Context,
    _Out_ PPCI_EXPRESS_LTR_MAX_LATENCY_REGISTER UpstreamLatency,
    _Out_ PEXPRESS_PORT_OBFF_CONTROL ObffConfiguration
    );

typedef BRIDGE_GET_POWER_PARAMETERS *PBRIDGE_GET_POWER_PARAMETERS;

//
// The PCI Express Base specification defines a Virtual PCI-PCI Bridge
// devices to represent Root Ports as well as an Upstream and Downstream
// Switch ports. These virtual bridges are represented as Express bridges
// in the driver.
//

#define PCI_TRACKED_ACS_BITS  7
typedef struct _EXPRESS_BRIDGE {

    PCI_EXPRESS_DEVICE_TYPE BridgeType;
    PPCI_BRIDGE PciBridge;
    PROOT_COMPLEX RootComplex;
    PEXPRESS_BRIDGE Parent;

    //
    // Express bridge control functions for things in the DeviceControl2
    // register, which is controlled at the PDO.
    //

    PINTERFACE_DEREFERENCE InterfaceDereference;
    PBRIDGE_GET_POWER_PARAMETERS GetPowerParameters;
    PBRIDGE_ARI_FORWARDING_ENABLE AriForwardingEnable;
    PBRIDGE_OBFF_CONTROL ObffControl;

    //
    // This field is used to link a root port bridge to its root complex
    // and a downstream switch port bridge to its switch complex.
    //

    PEXPRESS_BRIDGE Sibling;

    //
    // Save express capabilities register for all bridge types.
    //

    PCI_EXPRESS_CAPABILITIES_REGISTER ExpressCapabilities;

    //
    // AER Capability is an optional extended capability implemented by any
    // PCI Express device.
    //

    USHORT AerCapability;
    USHORT Reserved;

    //
    // ARI fields
    //

    BOOLEAN AriForwardingSupported;
    BOOLEAN AriForwardingEnabled;

    //
    // ACS (Access Control Services)
    //

    USHORT AcsCapability;
    BOOLEAN AcsNotNeeded;
    UCHAR Reserved1;
    USHORT AcsEnableCount[PCI_TRACKED_ACS_BITS];

    //
    // ATS (Address Translation Services)
    //
    USHORT AtsCapability;

    //
    // The error source descriptor for this bridge.
    //

    ULONG WheaInitialized;
    WHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource;

    //
    // Suggested latency (starting point) values for downstream devices.
    //

    PCI_EXPRESS_LTR_MAX_LATENCY_REGISTER UpstreamLatency;

    //
    // Holds the intended OBFF configuration of the express bridge, discerned
    // either by probing the hardware or evaluating the _DSM defined in the PCI
    // BIOS specification.
    //

    EXPRESS_PORT_OBFF_CONTROL ObffConfiguration;

    //
    // Indicates whether the bridge is a Thunderbolt device
    //

    USHORT ThunderboltCapability;

    //
    // Physical slot number
    //

    USHORT PhysicalSlotNumber;

    //
    // Timer and DPC used to release locks after hotplug commands are
    // executed on this bridge.
    //

    KTIMER HotPlugCommandTimer;
    KDPC HotPlugCommandDpc;
    PIO_WORKITEM HotPlugWorkItem;
    HOTPLUG_WORKER_PARAMETERS HotPlugWorkerParameters;

    union {

        struct {

            EXPRESS_LINK ExpressLink;

            //
            // Fields defined for managing a hot plug capable slot.
            //

            BOOLEAN HotPlugCapable;
            PPCI_HOTPLUG_CONTROLLER HotPlugController;

            //
            // This field describes an interrupt state for a switch port as
            // governed by external events like PnP and Power. It does not
            // always relay the current state of the hardware. An ISR must
            // never examine this field to base its decision about asserting
            // an interrupt. This field is always updated with a spin lock
            // held. Any changes to the interrupt related bits on the switch
            // port by an ISR are not reflected here. This field describes the
            // final state for the switch port to be in.
            //

            LONG InterruptsEnabled;
            KSPIN_LOCK InterruptStateLock;

            //
            // This field is defined to keep track of work items delegated for
            // the DPC and/or worker routine. This field must always be updated
            // using Interlocked operations.
            //

            __volatile LONG DelegatedWorkItems;

            //
            // These are defined to support Aspm.
            //

            __volatile ULONG RegistrantsForAspmCallback;
            PVOID AspmPolicyHandle;
            BOOLEAN AspmL0sOverride;
            BOOLEAN AspmL1Override;
            ULONG AspmOverrideFlags;

            //
            // Variables defined to facilitate interrupt connection.
            //

            BOOLEAN InterruptConnected;
            ULONG InterruptConnectVersion;

            union {
                struct {

                    USHORT MaxPayloadSize;
                    USHORT ReadCompletionBoundary;
                    PCI_EXPRESS_ROOT_CONTROL_REGISTER RootControl;

                    PVOID ConnectionContext;
                    KSPIN_LOCK MessageRoutineSpinLock;

                    //
                    // This field is defined to prevent a callback for S0 from
                    // enabling interrupts if the root port has taken ownership
                    // for enabling them in the process of power Irp handling
                    // related to changes in the system state.
                    //

                    BOOLEAN EnableInterruptsOnS0Callback;

                    //
                    // These are defined so that a root port can be placed on a
                    // PME processing list and to record PME event requestor.
                    //

                    BOOLEAN OnPmeList;
                    LIST_ENTRY PmeListEntry;
                    EXPRESS_PME_EVENT_TYPE PmeEventType;
                    PCI_EXPRESS_PME_REQUESTOR_ID PmeRequestorId;

                    //
                    // AER Registers.
                    //

                    PCI_EXPRESS_ROOT_ERROR_COMMAND RootErrorCommand;
                    PCI_EXPRESS_ROOT_ERROR_STATUS RootErrorStatus;
                    PCI_EXPRESS_ERROR_SOURCE_ID ErrorSourceId;

                } RootPort;

                struct _DOWNSTREAM_SWITCHPORT {

                    PEXPRESS_BRIDGE RootPortBridge;
                    PSWITCH_COMPLEX SwitchComplex;

                    PKINTERRUPT InterruptObject;

                    //
                    // ASPM
                    //

                    BOOLEAN UpstreamLinkL0sL1Override;

                } SwitchPort;
            } u;

        } Downstream;

        struct _UPSTREAM_SWITCHPORT {

            PEXPRESS_BRIDGE RootPortBridge;
            PSWITCH_COMPLEX SwitchComplex;

        } UpstreamSwitchPort;

    };

} EXPRESS_BRIDGE, *PEXPRESS_BRIDGE;

#include <pshpack1.h>

typedef struct _EXPRESS_ERROR_PACKET {
    WHEA_ERROR_PACKET Header;
    WHEA_PCIEXPRESS_ERROR_SECTION PciExpressError;
} EXPRESS_ERROR_PACKET, *PEXPRESS_ERROR_PACKET;

#include <poppack.h>

//
// An express port links to the physical device object extension and represents
// any of the following - Root complex integrated endpoints, event collectors,
// Root Ports, Switch ports, Legacy and non-Legacy endpoint devices and Express
// to PCI/PCI-X or PCI/PCI-X to Express bridges.
//

typedef struct _EXPRESS_PORT {

    PCI_EXPRESS_DEVICE_TYPE DeviceType;
    PPCI_DEVICE Device;

    PROOT_COMPLEX RootComplex;
    PEXPRESS_BRIDGE RootPortBridge;
    PEXPRESS_BRIDGE Parent;

    //
    // The ExpressCapabilitiesRegister provides the version
    // number for the PCI Express Capability.
    //

    PCI_EXPRESS_CAPABILITIES_REGISTER ExpressCapabilities;

    //
    // DeviceControl register is available on all Express devices.
    // The values of this register is saved and restored by the PDO
    // for all device types.
    //

    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControl;

    //
    // DeviceControl2 register is available on all Express devices.
    // The values of this register is saved and restored by the PDO
    // for all device types.
    //
    // N.B. A copy of the DeviceControl2 register is also stored in the
    //  Express_Bridge structure to enable control of the ARI Enable bit and
    //  OBFFEnable bits in this register.  What is stored here is what should
    //  be written to the device on a D0 transition.
    //

    PCI_EXPRESS_DEVICE_CONTROL_2_REGISTER DeviceControl2;

    //
    // The device capabilities register is also available on all express
    // devices.
    //

    PCI_EXPRESS_DEVICE_CAPABILITIES_REGISTER DeviceCapabilities;
    PCI_EXPRESS_DEVICE_CAPABILITIES_2_REGISTER DeviceCapabilities2;

    //
    // The Slot Capabilities register is only present on Express devices that
    // implement a slot as indicated by a bit in the Express Capabilities
    // register.
    //

    PCI_EXPRESS_SLOT_CAPABILITIES_REGISTER SlotCapabilities;

    //
    // This field is only used for linking RC Integrated Endpoints
    // and RC Event Collectors to the Root Complex.
    //

    PEXPRESS_PORT Sibling;

    //
    // ExpressLink is only valid for devices with Links.
    // These include Endpoint devices, PCI Express to PCI/PCI-X bridges, and
    // Upstream Ports of Switches. An express link is established or severed
    // while in possession of a ChildDeviceLock.
    //

    PEXPRESS_LINK ExpressLink;

    //
    // These fields are relevant for an express endpoint device that can
    // participate in the link power management.
    //

    ULONG AcceptableL0sLatency;
    ULONG AcceptableL1Latency;

    ULONG ComputedL0sLatency;
    ULONG ComputedL1Latency;

    struct {
        ULONG BaseVersion11OrGreater:1;
        ULONG RegisteredForAspmCallback:1;
    };

    //
    // Serial Number Capability is an optional extended capability
    // implemented by any PCI Express device. The device serial
    // number is unique across all devices with the same VendorID/DeviceID.
    //
    // NOTE: The value stored here is guaranteed to always match the value
    // stored in this device's serial number capability.  The first time
    // it is read it will be cached in the registry and saved in the
    // PCI_DEVICE extention for later PCI Instance ID generation.  It is
    // possible for the value read from the current capability to not match
    // the cached registry value if this device was replaced by a device
    // that differs *only* by serial number.
    //

    USHORT SerialNumberCapability;
    ULONGLONG SerialNumber;

    //
    // AER Capability is an optional extended capability implemented by any
    // PCI Express device.
    //

    USHORT AerCapability;

    //
    // A copy of the device's AER capabilities block.
    //

    PCI_EXPRESS_AER_CAPABILITY AerCapabilityBlock;

    //
    // ARI Capability fields
    //

    ULONG NextAriFunctionNumber;
    USHORT AriCapability;

    //
    // ACS (Access Control Services) Capability
    //

    USHORT AcsCapability;
    PCI_EXPRESS_ACS_CONTROL AcsControl;

    //
    // ATS (Address Translation Services) Support
    //
    USHORT AtsCapability;

    //
    // PASID capability
    //

    USHORT PasidCapability;

    //
    // PRI capability
    //

    USHORT PriCapability;

    //
    // HAL power callback information for SVM capable devices
    //

    PHAL_DEVICE_POWER_TRANSITION_CALLBACK HalDevicePowerTransCallback;
    PVOID HalDevicePowerTransCallbackContext;

    //
    // VC (Virtual Channel) Capability
    //
    USHORT VcCapability;

    //
    // Virtualization (SR-IOV) fields
    //
    USHORT SriovCapability;

    USHORT FirstVFOffset;
    USHORT VFStride;
    USHORT InitialVFs;
    USHORT TotalVFs;
    USHORT NumVFs;
    BOOLEAN EnableSriovDecodes;
    BOOLEAN SriovEnabled;
    ULONG ProbedVfBars[PCI_TYPE0_BAR_COUNT];

    //
    // WHEA error packet is used for reporting PCI Express errors to WHEA.
    //

    EXPRESS_ERROR_PACKET WheaErrorPacket;

    WHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource;

    //
    // Transaction Processing Hints (TPH)
    //

    USHORT TphRequesterCapability;
    BOOLEAN TphRequesterCapabilitySaved;
    PCI_EXPRESS_TPH_REQUESTER_CAPABILITY TphRequesterCapabilityBlock;

    //
    // Optional Table of TPH Steering Tags saved from either the TPH capability
    // block or the MSI-X capability block.
    //

    PPCI_EXPRESS_TPH_ST_TABLE_ENTRY TphSteeringTags;

    //
    // Latency Tolerance Reporting (LTR) and OBFF, which are two parts of
    // runtime power management.
    //

    EXPRESS_PORT_OBFF_CONTROL Obff;

    union {
        struct {
            USHORT LtrCapability;
            BOOLEAN LtrCapabilitySaved;
            PCI_EXPRESS_LTR_CAPABILITY LtrCapabilityBlock;
        };
        struct {
            PCI_EXPRESS_LTR_MAX_LATENCY_REGISTER LatencyForDownstreamEndpoints;
        };
    };

    //
    // Thunderbolt capability.
    //

    USHORT ThunderboltCapability;

    USHORT L1PmSsCapability;
    BOOLEAN L1PmSsCapabilitySaved;
    PCI_EXPRESS_L1_PM_SS_CAPABILITY L1PmSsCapabilityBlock;

    PCI_RESIZABLE_BAR_INFO ResizableBar;

    //
    // Save/restore area for Virtual Channel capability
    //

    ULONG VcSaveSize;
    _Field_size_bytes_(VcSaveSize)
    PUCHAR VcSaveArea;
} EXPRESS_PORT, *PEXPRESS_PORT;

//
// An express compatibility port contains read-only data for an express device
// which is under control of the system BIOS and not the OS.
//

typedef struct _EXPRESS_COMPATIBILITY_PORT {

    PCI_EXPRESS_DEVICE_TYPE DeviceType;
    PPCI_DEVICE Device;

    //
    // These fields are PCI Express device registers.
    //

    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControl;
    PCI_EXPRESS_DEVICE_CAPABILITIES_REGISTER DeviceCapabilities;

    //
    // This field indicates whether PCI Express link registers are valid
    // for this device (i.e. that there is an ingress express link).
    //

    BOOLEAN ExpressLinkValid;

    //
    // These fields are PCI Express link registers.
    //

    PCI_EXPRESS_LINK_CAPABILITIES_REGISTER LinkCapabilities;
    PCI_EXPRESS_LINK_CONTROL_REGISTER LinkControl;
    PCI_EXPRESS_LINK_STATUS_REGISTER LinkStatus;

    struct {

        //
        // This field represents teh base version of the device.
        //

        ULONG BaseVersion11OrGreater:1;
    };

    //
    // AER Capability is an optional extended capability implemented by any
    // PCI Express device.
    //

    USHORT AerCapability;

    //
    // Thunderbolt capability.
    //

    USHORT ThunderboltCapability;

    PCI_RESIZABLE_BAR_INFO ResizableBar;

} EXPRESS_COMPATIBILITY_PORT, *PEXPRESS_COMPATIBILITY_PORT;

typedef struct _EXPRESS_LINK_QUIESCENT_CONTEXT {

    ULONG Signature;
    BOOLEAN InProgress;
    volatile LONG ReferenceCount;
    PPCI_DEVICE Device;

} EXPRESS_LINK_QUIESCENT_CONTEXT, *PEXPRESS_LINK_QUIESCENT_CONTEXT;

//
// ------------------------------------------------------------- Express Macros
//

//
// An Express link is a point-to-point link that interconnects a set of
// components. An ingress express link refers to that end of the link
// which is further away from the root port than its terminal link.
//
//

#define EXPRESS_INGRESS_LINK_PORT(_Port)                                    \
    (((_Port)->DeviceType == PciExpressEndpoint)  ||                        \
     ((_Port)->DeviceType == PciExpressLegacyEndpoint)  ||                  \
     ((_Port)->DeviceType == PciExpressUpstreamSwitchPort)  ||              \
     ((_Port)->DeviceType == PciExpressToPciXBridge))

#define EXPRESS_COMPATIBILITY_INGRESS_LINK_PORT(_Port)                      \
    (((_Port)->DeviceType == PciExpressEndpoint)  ||                        \
     ((_Port)->DeviceType == PciExpressLegacyEndpoint)  ||                  \
     ((_Port)->DeviceType == PciExpressUpstreamSwitchPort)  ||              \
     ((_Port)->DeviceType == PciExpressToPciXBridge))

#define EXPRESS_RC_INTEGRATED_DEVICE(_Port)                                 \
    (((_Port)->DeviceType == PciExpressRootComplexIntegratedEndpoint) ||    \
     ((_Port)->DeviceType == PciExpressRootComplexEventCollector))

///////////////////////////////////////////////////////////////////////////////
// PCI Express and SHPC Hot Plug Definitions and Extensions                  //
///////////////////////////////////////////////////////////////////////////////

#define PCI_SLOT_EVENT_MAP_COUNT        (2 * SlotInterruptMaximum)

#define PCI_HOTPLUG_SLOT_LOCATE_REQUESTED       0x1
#define PCI_HOTPLUG_SLOT_PROBLEM_DETECTED       0x2

#define PCI_HOTPLUG_SLOT_INVALIDATE_DEVICE      0x1
#define PCI_HOTPLUG_SLOT_UNPRIMED               0x2

#define PCI_DEVICE_REMOVABLE(_Device)                               \
    (((_Device)->HotPlugSlot != NULL) &&                            \
     ((_Device)->HotPlugSlot->Removable != FALSE))

#define PCI_DEVICE_EJECTABLE(_Device)                               \
    (((_Device)->HotPlugSlot != NULL) &&                            \
     ((_Device)->HotPlugSlot->PowerControllerPresent != FALSE))

#define PCI_DEVICE_SURPRISE_REMOVABLE(_Device)                      \
    (((_Device)->HotPlugSlot != NULL) &&                            \
     ((_Device)->HotPlugSlot->HotPlugSurprise != FALSE))

typedef enum {
    SlotIndicatorOff,
    SlotIndicatorBlinking,
    SlotIndicatorOn
} PCI_HOTPLUG_INDICATOR_STATE;

typedef enum {
    SlotPowerOff,
    SlotPowerOn
} PCI_HOTPLUG_POWER_STATE;

typedef enum {
    LinkStateDisable,
    LinkStateEnable
} PCI_HOTPLUG_LINK_STATE;

typedef enum {
    LocateRequested,
    LocateRequestCleared,
    ProblemDetected,
    ProblemCleared,
    LeavingDisabled
} PCI_ATTENTION_INDICATOR_EVENT_TYPE;

typedef enum {
    SlotStateNotReady,
    SlotStateDisabled,
    SlotStateWaitingWhileDisabled,
    SlotStatePowered,
    SlotStateBusSpeedSet,
    SlotStateLinkActiveTimerQueued,
    SlotStateSettleTimerQueued,
    SlotStateFullyFunctional,
    SlotStateWaitingWhileEnabled,
    SlotStatePowerOffTimerQueued,
    SlotStateEjectRequested
} PCI_HOTPLUG_SLOT_STATE;

typedef enum {
    SlotEventNone,
    SlotEventPrimed,
    SlotEventUnprimed,
    SlotEventEnableRequest,
    SlotEventDisableRequest,
    SlotEventSurpriseDisableRequest,
    SlotEventAttentionRequest,
    SlotEventLinkStateActive,
    SlotEventLinkStateNotActive,
    SlotEventPowerFault,
    SlotEventStackTornDown,
    SlotEventStackTearDownFailed,
    SlotEventTimerExpired,
    SlotEventActionCompleted,
    SlotEventForceEnable,
    SlotEventForceDisable
} PCI_HOTPLUG_EVENT_TYPE;

typedef enum {
    SlotCommandNoop,
    SlotCommandSetSlotPower,
    SlotCommandSetPowerIndicator,
    SlotCommandSetAttentionIndicator,
    SlotCommandEnableDisableInterrupts,
    SlotCommandSetLinkState,
    SlotCommandQueueAbortTimer,
    SlotCommandQueueSettleTimer,
    SlotCommandQueueLinkActiveTimer,
    SlotCommandQueuePowerOffTimer,
    SlotCommandSetEvent,
    SlotCommandInvalidateBusRelations,
    SlotCommandSetBusSpeed,
    SlotCommandRequestDeviceEject,
    SlotCommandQueueHotPlugEvent
} PCI_HOTPLUG_COMMAND_TYPE;

typedef enum _PCI_HOTPLUG_SLOT_INTERRUPT {
    SlotInterruptPresenceDetect = 0,
    SlotInterruptLatchChange = 1,
    SlotInterruptAttentionButton = 2,
    SlotInterruptLinkStateChange = 3,
    SlotInterruptPowerFault = 4,
    SlotInterruptMaximum = 7
} PCI_HOTPLUG_SLOT_INTERRUPT, *PPCI_HOTPLUG_SLOT_INTERRUPT;

typedef struct _PCI_HOTPLUG_INTERRUPTS {
    BOOLEAN CommandCompleteInterrupt;
    ULONG SlotMask;
    ULONG SlotInterrupts[ANYSIZE_ARRAY];
} PCI_HOTPLUG_INTERRUPTS, *PPCI_HOTPLUG_INTERRUPTS;

typedef
BOOLEAN
(*GET_DEVICE_PRESENCE_STATE) (
    __in PPCI_HOTPLUG_SLOT Slot
    );

typedef
BOOLEAN
(*GET_LATCH_STATE) (
    __in PPCI_HOTPLUG_SLOT Slot
    );

typedef
BOOLEAN
(*GET_LINK_STATE) (
    __in PPCI_HOTPLUG_SLOT Slot
    );

typedef
BOOLEAN
(*EXECUTE_HOTPLUG_COMMANDS) (
    __in PPCI_HOTPLUG_SLOT Slot,
    __in PCI_HOTPLUG_COMMAND_TYPE Type,
    __in ULONG_PTR Context
    );

typedef
BOOLEAN
(*ENABLE_DISABLE_CONTROLLER_INTERRUPTS) (
    __in PPCI_HOTPLUG_CONTROLLER Controller,
    __in BOOLEAN Enable
    );

typedef struct _PCI_HOTPLUG_SLOT {

    ULONG PhysicalSlotNumber;
    BOOLEAN HotPlugSurprise;
    BOOLEAN Removable;
    PCI_HOTPLUG_SLOT_STATE CurrentState;

    PPCI_HOTPLUG_CONTROLLER Controller;

    PPCI_DEVICE Device;
    KSPIN_LOCK DeviceLock;

    LIST_ENTRY EventQueue;
    KSPIN_LOCK QueueLock;
    BOOLEAN EventQueueBlocked;

    KTIMER Timer;
    KDPC TimerDpc;
    ULONG TimerFlags;

    //
    // Each bit in the UCHAR is one of the elements of the PCI_HOTPLUG_SLOT_
    // INTERRUPTS enum.
    //

    LONG SlotInterrupts;

    //
    // Hot Plug elements.
    //

    BOOLEAN AttentionButtonPresent;
    BOOLEAN PowerControllerPresent;
    BOOLEAN MRLSensorPresent;
    BOOLEAN AttentionIndicatorPresent;
    BOOLEAN PowerIndicatorPresent;
    BOOLEAN ElectromechanicalLockPresent;

    UCHAR AttentionIndicatorFlags;

    PCI_HOTPLUG_EVENT_TYPE EventMap[PCI_SLOT_EVENT_MAP_COUNT];

} PCI_HOTPLUG_SLOT, *PPCI_HOTPLUG_SLOT;

typedef struct _PCI_HOTPLUG_CONTROLLER {

    PPCI_BRIDGE PciBridge;

    LIST_ENTRY ActionQueue;
    KSPIN_LOCK QueueLock;
    BOOLEAN ActionQueueBlocked;

    KTIMER CommandTimer;
    KDPC CommandTimerDpc;
    KSPIN_LOCK TimerLock;
    BOOLEAN TimersDisabled;

    BOOLEAN LinkActiveReportingCapable;
    BOOLEAN InstantaneousCommandCompleteSupport;
    BOOLEAN CommandCompleteInterruptPending;
    ULONG UnprocessedCommandCompleteInterrupts;

    GET_DEVICE_PRESENCE_STATE GetDevicePresenceState;
    GET_LATCH_STATE GetLatchState;
    GET_LINK_STATE GetLinkState;
    EXECUTE_HOTPLUG_COMMANDS ExecuteHotPlugCommands;
    ENABLE_DISABLE_CONTROLLER_INTERRUPTS EnableDisableControllerInterrupts;

    //
    // Slot structures.
    //

    ULONG SlotCount;
    PCI_HOTPLUG_SLOT Slots[ANYSIZE_ARRAY];

} PCI_HOTPLUG_CONTROLLER, *PPCI_HOTPLUG_CONTROLLER;

//
// ---------------------------------------------------------- Custom Properties
//

//
// A table is defined that assembles the custom properties of a pci root bus
// and a pci device. The entries for this table are in a format given below.
//

typedef struct _PCI_PROPERTY_LIST_ENTRY {

    //
    // A property key identifier.
    //

    const DEVPROPKEY *DevPropKey;

    //
    // Defines an attribute for a property - volatile vs persistent, read-only
    // vs read-write.
    //

    ULONG Flags;

    //
    // Describes a property type.
    //

    DEVPROPTYPE DevPropType;

    //
    // Size of property data.
    //

    ULONG DataSize;

    //
    // A property value itself.
    //

    ULONG Data;

    //
    // This field is initialized to STATUS_NOT_SUPPORTED for every property in
    // a table. If a root bus or a device supports and updates the data field
    // for a table entry, then the corresponding status field must be changed
    // to success. if this field is not sucess, then the property will not be
    // set.
    //

    NTSTATUS Status;

} PCI_PROPERTY_LIST_ENTRY, *PPCI_PROPERTY_LIST_ENTRY;

#define PCI_MAX_MINOR_POWER_IRP  IRP_MN_QUERY_POWER
#define PCI_MAX_MINOR_PNP_IRP    IRP_MN_DEVICE_ENUMERATED

///////////////////////////////////////////////////////////////////////////////
// Exports                                                                   //
///////////////////////////////////////////////////////////////////////////////

//
// ------------------------------------------------------------------ arbiters.c
//

//
// Arbiter state values.
//
#define PCI_ARBITER_STATE_UNINITIALIZED 0
#define PCI_ARBITER_STATE_CONFIGURED    1
#define PCI_ARBITER_STATE_REPROGRAMMED  2

NTSTATUS
PciInitializeArbiters(
    __inout PPCI_BUS Bus
    );

VOID
PciDestroyArbiters(
    __inout PPCI_BUS Bus
    );

NTSTATUS
PciRangeListFromResourceList(
    __in PPCI_BUS Bus,
    __in PARBITER_INSTANCE Arbiter,
    __in_ecount(Count) PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptors,
    __in ULONG Count,
    __out PRTL_RANGE_LIST RangeList
    );

BOOLEAN
PciArbitrateRequirementInsideBridgeWindow(
    __in PPCI_BUS Bus,
    __in PDEVICE_OBJECT DeviceObject,
    __in_ecount(Count) PIO_RESOURCE_DESCRIPTOR Alternatives,
    __in ULONG Count
    );

NTSTATUS
PciArbiter_UnpackRequirement(
    __in PIO_RESOURCE_DESCRIPTOR Descriptor,
    __out PULONGLONG Minimum,
    __out PULONGLONG Maximum,
    __out PULONGLONG Length,
    __out PULONGLONG Alignment
    );

NTSTATUS
PciArbiter_PackResource(
    __in PIO_RESOURCE_DESCRIPTOR Requirement,
    __in ULONGLONG Start,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor
    );

NTSTATUS
PciArbiter_UnpackResource(
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor,
    __out PULONGLONG Start,
    __out PULONGLONG Length
    );

LONG
PciArbiter_ScoreRequirement(
    __in PIO_RESOURCE_DESCRIPTOR Descriptor
    );

//
// ----------------------------------------------------------------- arbmemory.c
//

typedef struct _PCI_ARBITER_PREFETCHABLE_STATE {

    //
    // The number of prefetchable ranges
    //
    USHORT PrefetchableCount;

    //
    // Indicates that this arbiter will arbitrate prefetchable memory
    //
    BOOLEAN PrefetchablePresent;

    //
    // The allocation ordering list to be used for prefetchable memory
    //
    ARBITER_ORDERING_LIST PrefetchableOrdering;

    //
    // The allocation ordering list to be used for standard memory
    //
    ARBITER_ORDERING_LIST NonprefetchableOrdering;

} PCI_ARBITER_PREFETCHABLE_STATE, *PPCI_ARBITER_PREFETCHABLE_STATE;

typedef struct _PCI_ARBITER_MEMORY_EXTENSION {

    //
    // Indicates the state of this arbiter.  Permissible values are PCI_ARBITER_STATE_*.
    //
    ULONG ArbiterState;

    //
    // The original memory allocation ordering (from the registry)
    //
    ARBITER_ORDERING_LIST OriginalOrdering;

    BOOLEAN PrefetchableStateSaved;

    PCI_ARBITER_PREFETCHABLE_STATE Current;
    PCI_ARBITER_PREFETCHABLE_STATE Saved;

} PCI_ARBITER_MEMORY_EXTENSION, *PPCI_ARBITER_MEMORY_EXTENSION;

NTSTATUS
PciInitializeMemoryArbiter(
    __in PPCI_BUS Bus,
    __out PARBITER_INSTANCE *MemoryArbiter
    );

NTSTATUS
PciProgramMemoryArbiter(
    __in PPCI_BUS Bus,
    __inout PARBITER_INSTANCE Arbiter,
    __in_ecount(ResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    __in ULONG ResourceCount,
    __out PRTL_RANGE_LIST RangeList
    );

//
// ------------------------------------------------------------------- arbport.c
//

extern RTL_RANGE_LIST PciIsaBitExclusionList;
extern RTL_RANGE_LIST PciVgaAndIsaBitExclusionList;

NTSTATUS
PciInitializeIoArbiter(
    __in PPCI_BUS Bus,
    __out PARBITER_INSTANCE *IoArbiter
    );

NTSTATUS
PciProgramIoArbiter(
    __in PPCI_BUS Bus,
    __in PARBITER_INSTANCE Arbiter,
    __in_ecount(ResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR Resources,
    __in ULONG ResourceCount,
    __inout PRTL_RANGE_LIST RangeList
    );

NTSTATUS
PciBuildDefaultIoExclusionLists(
    VOID
    );

//
// Errata Mnager Pci Hack Rule Guids.
//

//
// IgnorePciRootBusNumberRange.
//

extern const GUID GUID_EM_RULE_IGNORE_PCI_ROOTBUS_NUMBER_RANGE;

//
// IgnoreOverlappingPciRootBusNumberRanges.
//

extern const GUID GUID_EM_RULE_IGNORE_OVERLAPPING_PCI_ROOTBUS_NUMBER_RANGES;

//
// IgnoreCBMemLimits.
//

extern const GUID GUID_EM_RULE_IGNORE_CB_MEM_LIMITS;

//
// AvoidAssertOnBadIdDataForHostBridge.
//

extern const GUID GUID_EM_RULE_AVOID_ASSERT_ON_BAD_ID_DATA_FOR_HOST_BRIDGE;

//
// IgnoreIsaVgaBitConflict.
//

extern const GUID GUID_EM_RULE_IGNORE_ISA_VGA_BIT_CONFLICT;

//
// DisablePciExpressASPM.
//

extern const GUID GUID_EM_RULE_DISABLE_PCI_EXPRESS_ASPM;

//
// Work around unreported memory at F8.
//

extern const GUID GUID_EM_PCI_ARBITER_UNREPORTED_MEMORY_AT_F8_RULE;

//
// Match for AMD cpu type.
//

extern const GUID GUID_EM_CPU_TYPE_AMD;

//
// -------------------------------------------------------------------- aspm.c
//

//
// Possible values for PCI Express ASPM power policy.
//

#define PCIEXPRESS_ASPM_DISABLE_POLICY      0x0
#define PCIEXPRESS_ASPM_L0S_POLICY          0x1
#define PCIEXPRESS_ASPM_L0SL1_POLICY        0x2

NTSTATUS
ExpressBridgeInitializeLinkPowerManagement (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __inout PEXPRESS_LINK ExpressLink
    );

NTSTATUS
ExpressBridgeProcessLinkForPowerManagement (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

NTSTATUS
ExpressBridgeUnregisterDeviceForAspmCallback (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressBridgeUnregisterAspmPolicyCallback (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __inout PVOID *AspmPolicyHandle
    );

NTSTATUS
ExpressPortPrepareForLinkPowerManagement (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressUpdateLinkPowerManagementSupport (
    __inout PEXPRESS_LINK ExpressLink
    );

NTSTATUS
ExpressSwitchUpdateLinkLatency (
    __inout PSWITCH_COMPLEX SwitchComplex
    );

NTSTATUS
ExpressBridgeRetrainLink (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

//
// -------------------------------------------------------------------- bridge.c
//

extern const GUID GUID_PCI_BRIDGE_INTERFACE;

typedef struct _PCI_BRIDGE_INTERFACE {

    //
    // generic interface header
    //
    USHORT Size;
    USHORT Version;
    PVOID Context;
    PINTERFACE_REFERENCE InterfaceReference;
    PINTERFACE_DEREFERENCE InterfaceDereference;

    //
    // Bridge interface specifics
    //
    PCI_BRIDGE_RESOURCES BiosConfiguration;
    PCI_BRIDGE_REQUIREMENTS MinimumRequirements;

    UCHAR PciXCapability;
    UCHAR ExpressCapability;
    USHORT AerCapability;
    USHORT AcsCapability;
    USHORT AtsCapability;
    USHORT ThunderboltCapability;
    BOOLEAN AcsNotNeeded;

    UCHAR SecondaryBusNumber;
    UCHAR SubordinateBusNumber;

    USHORT PhysicalSlotNumber;

    struct {
        ULONG Subtractive:1;
        ULONG IsaBitSet:1;
        ULONG LegacyVgaBitSet:1;
        ULONG BusNumbersConfigured:1;
        ULONG HyperTransport:1;
        ULONG MsiHtConverter:1;
        ULONG ExtendedConfigAvailable:1;
        ULONG D3Causes66MHzB2:1;
        ULONG AvoidChildD1D2ForSld:1;
        ULONG DisableLinkPowerManagement:1;
        ULONG ExpressBaseVersion11OrGreater:1;
        ULONG IgnoreBootConfig:1;
        ULONG SecondaryBusResetOnLinkStateChange:1;
        ULONG LinkDisableOnSlotPowerDown:1;
        ULONG DisableChildPowerManagement:1;
        ULONG DownstreamDeviceNotRemovable:1;
        ULONG EnableAllBridgeInterrupts:1;
        ULONG BusmasterDisabledOnBoot:1;
        ULONG E2ETLPPrefixSupported:1;
        ULONG AtomicsRoutingSupported:1;
        ULONG Atomics32BitCompleterSupported:1;
        ULONG Atomics64BitCompleterSupported:1;
        ULONG Atomics128BitCompleterSupported:1;
        ULONG AcsCapIncorrectFormat:1;
        ULONG Reserved:8;
    };

    WHEA_ERROR_SOURCE_DESCRIPTOR ErrorSource;

    PBRIDGE_GET_POWER_PARAMETERS GetPowerParameters;
    PBRIDGE_ARI_FORWARDING_ENABLE AriForwardingEnable;
    PBRIDGE_OBFF_CONTROL ObffControl;
    PBRIDGE_DELAY DelayPassive;
    PBRIDGE_STALL StallDispatch;

} PCI_BRIDGE_INTERFACE, *PPCI_BRIDGE_INTERFACE;

#define PCI_MEMORY_WINDOW_REQUIREMENT(_IoReq)                  \
    (((_IoReq)->Type == CmResourceTypeMemory) &&               \
     ((_IoReq)->Flags & CM_RESOURCE_MEMORY_WINDOW_DECODE) &&   \
     (!((_IoReq)->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE)))

#define PCI_PREFETCH_WINDOW_REQUIREMENT(_IoReq)                \
    ((((_IoReq)->Type == CmResourceTypeMemory) ||              \
      ((_IoReq)->Type == CmResourceTypeMemoryLarge)) &&        \
     ((_IoReq)->Flags & CM_RESOURCE_MEMORY_WINDOW_DECODE) &&   \
     ((_IoReq)->Flags & CM_RESOURCE_MEMORY_PREFETCHABLE))

#define PCI_IO_WINDOW_REQUIREMENT(_IoReq)                    \
    (((_IoReq)->Type == CmResourceTypePort) &&               \
     ((_IoReq)->Flags & CM_RESOURCE_PORT_WINDOW_DECODE))

#define PCI_WINDOW_REQUIREMENT(_IoReq)                    \
    (PCI_MEMORY_WINDOW_REQUIREMENT(_IoReq) ||             \
     PCI_PREFETCH_WINDOW_REQUIREMENT(_IoReq) ||           \
     PCI_IO_WINDOW_REQUIREMENT(_IoReq))

#define ADDRESS_BELOW_4GB(_Address_)     \
    (((_Address_) & 0xFFFFFFFF00000000) ? FALSE : TRUE)

NTSTATUS
PciProcessNewBridgedBus(
    __inout PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot
    );

NTSTATUS
PciBridgeStart(
    __inout PPCI_BRIDGE Bridge,
    __in PCM_RESOURCE_LIST RawResources,
    __in PCM_RESOURCE_LIST TranslatedResources
    );

NTSTATUS
PciBridgePowerUp(
    __in PPCI_BRIDGE Bridge
    );

NTSTATUS
PciBridgePowerDown(
    __in PPCI_BRIDGE Bridge
    );

NTSTATUS
PciBridgeProcessStartResources(
    __in PPCI_BRIDGE Bridge,
    __in_ecount(StartResourceCount) PCM_PARTIAL_RESOURCE_DESCRIPTOR StartResources,
    __in ULONG StartResourceCount,
    __out PPCI_BRIDGE_RESOURCES BridgeResources
    );

BOOLEAN
PciBridgeCheckLocationCompatibility (
    __in PPCI_BRIDGE Bridge,
    __in PPCI_BRIDGE_RESOURCES Resources,
    __in PPCI_BRIDGE_REQUIREMENTS Requirements
    );

NTSTATUS
PciBridgeFilterRequirementsList(
    __in PPCI_BRIDGE Bridge,
    __in PIO_RESOURCE_REQUIREMENTS_LIST InputRequirements,
    __out PIO_RESOURCE_REQUIREMENTS_LIST *FilteredRequirements
    );

NTSTATUS
PciBridgeComputeRequirements(
    __inout PPCI_BRIDGE Bridge,
    __inout PPCI_BRIDGE_REQUIREMENTS PreferredRequirements,
    __inout PPCI_BRIDGE_REQUIREMENTS FallbackRequirements,
    __inout PPCI_BRIDGE_REQUIREMENTS SubtractiveRequirements
    );

NTSTATUS
PciDeviceRequirementsToBridgeRequirements(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_REQUIREMENTS DeviceRequirements,
    __out PPCI_BRIDGE_REQUIREMENTS BridgeRequirements
    );

VOID
PciBridgeUpdateD3Availability (
    __in PPCI_BRIDGE Bridge
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBridgeReevaluateWindows (
    _In_ PPCI_BRIDGE Bridge
    );

VOID
PciBridgeAssertSecondaryReset (
    __inout PPCI_BRIDGE Bridge
    );

VOID
PciBridgeDeassertSecondaryReset (
    __inout PPCI_BRIDGE Bridge
    );

//
// -------------------------------------------------------------------- busapi.c
//

extern PPCI_SEGMENT  PciSegmentList;
extern KEVENT        PciLegacyDescriptionLock;
extern volatile LONG PciCreateControlDeviceCount;

DRIVER_ADD_DEVICE PciAddDevice;

VOID
PciRemoveBusFromTree(
    __in PPCI_BUS Bus
    );

PPCI_BUS
PciNextBusInSegment(
    __in PPCI_BUS Bus
    );

ULONG
PciRootCountInTree();

NTSTATUS
PciQueryDeviceUsageInTree(
    __in ULONG BusNumber,
    __in ULONG DeviceNumber,
    __in ULONG FunctionNumber,
    __out PPCI_QUERY_DEVICE_USAGE_OUTPUT DeviceUsageOutput
    );

NTSTATUS
PciEarlyRestoreResources (
    _In_ SYSTEM_POWER_STATE SleepState
    );

NTSTATUS
PciEarlyRestoreResourcesInternal (
    _In_ ULONG RestoreType,
    _In_ BOOLEAN RestorePoweredDevices
    );

NTSTATUS
PciDebugDevicePowerCallbackRegister (
    __in PPCI_DEVICE Device
    );

NTSTATUS
PciDebugDevicePowerCallbackDeregister (
    __in PPCI_DEVICE Device
    );

//
// ---------------------------------------------------------------- busnumbers.c
//

#define PciSetBitRangeInclusive(_BitMap, _Start, _End)      \
    RtlSetBits((_BitMap), (_Start), (_End) - (_Start) + 1)

#define PciClearBitRangeInclusive(_BitMap, _Start, _End)      \
    RtlClearBits((_BitMap), (_Start), (_End) - (_Start) + 1)

#define PciSetBitsToEnd(_BitMap, _Start)                \
    PciSetBitRangeInclusive((_BitMap), (_Start), (_BitMap)->SizeOfBitMap - 1)

VOID
PciConfigureBusNumbers(
    __inout PPCI_BUS Bus
    );

BOOLEAN
PciAreBusNumbersConfigured(
    __in PPCI_BUS ParentBus,
    __in UCHAR Primary,
    __in UCHAR Secondary,
    __in UCHAR Subordinate
    );

VOID
PciExtendBusNumberRange(
    __in PPCI_BRIDGE PciBridge,
    __in UCHAR Subordinate
    );

//
// -------------------------------------------------------------- busnumspread.c
//

VOID
PciSpreadBridgesHotplug(
    __in PPCI_BUS Bus
    );

VOID
PciOverrideBridgeRequirements(
    _In_ PPCI_BRIDGE Bridge,
    _Inout_ PPCI_BRIDGE_REQUIREMENTS Preferred,
    _Inout_ PPCI_BRIDGE_REQUIREMENTS Fallback
    );

//
// -------------------------------------------------------------------- buspnp.c
//

//
// ------------------------------------------------------------------ buspower.c
//

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
BusPowerRegisterPep(
    _Inout_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
BusPowerUnregisterPep(
    _Inout_ PPCI_BUS Bus
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
PciBus_PowerReference (
    _Inout_ PPCI_BUS                        Bus,
    _In_    PCI_BUS_POWER_REFERENCE_REASONS Reason
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_PowerDereference (
    _Inout_ PPCI_BUS                        Bus,
    _In_    PCI_BUS_POWER_REFERENCE_REASONS Reason
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_QueueChildD0Irp (
    _Inout_ PPCI_DEVICE Device
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_ArmForWake (
    _Inout_ PPCI_BUS Bus
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_DisarmForWake (
    _Inout_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_SendWWPassive (
    _In_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_CancelWWPassive (
    _In_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_CompleteWWPassive (
    _In_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_SendD0Unlocked (
    _In_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_RetireChildPowerIrps (
    _Inout_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_ConventionalWakeTriggered (
    _In_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_PassWakeEvent (
    _Inout_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_RunDownWake (
    _Inout_ PPCI_BUS Bus
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_PowerStateM (
    _Inout_     PPCI_BUS Bus,
    _In_        PCI_BUS_POWER_EVENTS Event
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_WakeStateM (
    _Inout_     PPCI_BUS Bus,
    _In_        PCI_BUS_WAKE_EVENTS Event
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_SignalWorkerRetireD0Irps (
    _Inout_ PPCI_BUS    Bus
    );

LARGE_INTEGER
PciBus_WaitTimeForBusSettle (
    _In_ PPCI_BUS Bus,
    _In_ ULONG DeviceSpecificDelay
    );

//
// -------------------------------------------------------------------- config.c
//

extern UCHAR PciConfigDoubleBuffer[0x1000];
extern KEVENT PciConfigDoubleBufferLock;

VOID
PciGetConfigSlotAndBus(
    __in  PPCI_DEVICE      Device,
    __in  USHORT           FunctionOffset,
    __out PPCI_SLOT_NUMBER Slot,
    __out PULONG           BusNumber
    );

VOID
PciReadDeviceConfig(
    __in PPCI_DEVICE Device,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

VOID
PciWriteDeviceConfig(
    __in PPCI_DEVICE Device,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

VOID
PciProbeDeviceConfig(
    __in PPCI_DEVICE Device,
    __out_bcount(Length) PVOID ReadBuffer,
    __out_bcount(Length) PVOID ProbeBuffer,
    __in ULONG Offset,
    __in ULONG Length
    );

VOID
PciReadSlotConfig(
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

VOID
PciWriteSlotConfig(
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

NTSTATUS
PciExternalReadDeviceConfig(
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

NTSTATUS
PciExternalWriteDeviceConfig(
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

NTSTATUS
PciReadDeviceSpace(
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __in ULONG WhichSpace,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length,
    __out PULONG LengthRead
    );

NTSTATUS
PciWriteDeviceSpace(
    __in PPCI_DEVICE Device,
    __in USHORT FunctionOffset,
    __in ULONG WhichSpace,
    __in_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length,
    __out PULONG LengthWritten
    );

//
// VOID
// PciReadBridgeConfig(
//     IN PPCI_BRIDGE Bridge,
//     OUT PVOID Buffer,
//     IN ULONG Offset,
//     IN ULONG Length
//     );
//
#define PciReadBridgeConfig(Bridge, Buffer, Offset, Length)                    \
{                                                                              \
    PPCI_BRIDGE _Bridge = (Bridge);                                            \
    PciReadSlotConfig(                                                         \
        _Bridge->Bus->ParentBus,                                               \
        _Bridge->Slot,                                                         \
        Buffer,                                                                \
        Offset,                                                                \
        Length                                                                 \
        );                                                                     \
}

//
// VOID
// PciWriteBridgeConfig(
//     IN PPCI_BRIDGE Bridge,
//     OUT PVOID Buffer,
//     IN ULONG Offset,
//     IN ULONG Length
//     );
//
#define PciWriteBridgeConfig(Bridge, Buffer, Offset, Length)                   \
{                                                                              \
    PPCI_BRIDGE _Bridge = (Bridge);                                            \
    PciWriteSlotConfig(                                                        \
        _Bridge->Bus->ParentBus,                                               \
        _Bridge->Slot,                                                         \
        Buffer,                                                                \
        Offset,                                                                \
        Length                                                                 \
        );                                                                     \
}

//
// VOID
// PciReadConfigHeader(
//     IN PPCI_DEVICE Device,
//     OUT PPCI_CONFIG_HEADER Config
//     );
//
#define PciReadConfigHeader(Device, Config)                                    \
{                                                                              \
    PPCI_DEVICE _Device = (Device);                                            \
    PciReadDeviceConfig(                                                       \
        _Device,                                                               \
        (Config),                                                              \
        0,                                                                     \
        _Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE ?                       \
            PCI_TYPE2_HEADER_LENGTH : PCI_TYPE0_OR_1_HEADER_LENGTH             \
        );                                                                     \
}

//
// VOID
// PciWriteConfigHeader(
//     IN PPCI_DEVICE Device,
//     IN PPCI_CONFIG_HEADER Config
//     );
//
#define PciWriteConfigHeader(Device, Config)                                   \
{                                                                              \
    PPCI_DEVICE _Device = (Device);                                            \
    PciWriteDeviceConfig(                                                      \
        _Device,                                                               \
        (Config),                                                              \
        0,                                                                     \
        _Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE ?                       \
            PCI_TYPE2_HEADER_LENGTH : PCI_TYPE0_OR_1_HEADER_LENGTH             \
        );                                                                     \
}

#define PciReadConfigRegister(Device, RegisterName, Buffer)                    \
    PciReadDeviceConfig(Device,                                                \
                        (PUCHAR)Buffer,                                        \
                        CONFIG_FIELD_OFFSET(RegisterName),                     \
                        CONFIG_FIELD_LENGTH(RegisterName)                      \
                        )

#define PciWriteConfigRegister(Device, RegisterName, Buffer)                   \
    PciWriteDeviceConfig(Device,                                               \
                         (PUCHAR)Buffer,                                       \
                         CONFIG_FIELD_OFFSET(RegisterName),                    \
                         CONFIG_FIELD_LENGTH(RegisterName)                     \
                         )

#define PciProbeConfigRegister(Device, RegisterName, Buffer, ProbedBuffer)     \
    PciProbeDeviceConfig(Device,                                               \
                         (PUCHAR)Buffer,                                       \
                         (PUCHAR)ProbedBuffer,                                 \
                         CONFIG_FIELD_OFFSET(RegisterName),                    \
                         CONFIG_FIELD_LENGTH(RegisterName)                     \
                         )

#define PciReadDeviceCapability(Device, CapabilityName, Capability)           \
{                                                                             \
    PPCI_DEVICE _Device = (Device);                                           \
    PciReadDeviceConfig(_Device,                                              \
                        (PUCHAR)Capability,                                   \
                        _Device->CapabilityName,                              \
                        sizeof(*Capability)                                   \
                        );                                                    \
}

#define PciReadPowerCapabilityRegister(Device, RegisterName, Buffer)          \
{                                                                             \
    PPCI_DEVICE _Device = (Device);                                           \
    PCI_ASSERT(_Device->PowerCapability);                                     \
    PciReadDeviceConfig(_Device,                                              \
                        (PUCHAR)Buffer,                                       \
                        _Device->PowerCapability                              \
                            + FIELD_OFFSET(PCI_PM_CAPABILITY, RegisterName),  \
                        RTL_FIELD_SIZE(PCI_PM_CAPABILITY, RegisterName)       \
                        );                                                    \
}

#define PciWritePowerCapabilityRegister(Device, RegisterName, Buffer)         \
{                                                                             \
    PPCI_DEVICE _Device = (Device);                                           \
    PCI_ASSERT(_Device->PowerCapability);                                     \
    PciWriteDeviceConfig(_Device,                                             \
                         (PUCHAR)Buffer,                                      \
                         _Device->PowerCapability                             \
                             + FIELD_OFFSET(PCI_PM_CAPABILITY, RegisterName), \
                         RTL_FIELD_SIZE(PCI_PM_CAPABILITY, RegisterName)      \
                         );                                                   \
}

#define PciReadSubIDsCapabilityRegister(Device, RegisterName, Buffer)                   \
{                                                                                       \
    PPCI_DEVICE _Device = (Device);                                                     \
    PCI_ASSERT(_Device->SubIDsCapability);                                              \
    PciReadDeviceConfig(_Device,                                                        \
                        (PUCHAR)Buffer,                                                 \
                        _Device->SubIDsCapability                                       \
                            + FIELD_OFFSET(PCI_SUBSYSTEM_IDS_CAPABILITY, RegisterName), \
                        RTL_FIELD_SIZE(PCI_SUBSYSTEM_IDS_CAPABILITY, RegisterName)      \
                        );                                                              \
}

#define PciReadMsiCapabilityRegister(Device, RegisterName, Buffer)            \
{                                                                             \
    PPCI_DEVICE _Device = (Device);                                           \
    PCI_ASSERT(_Device->MsiCapability);                                       \
    PciReadDeviceConfig(_Device,                                              \
                        (PUCHAR)Buffer,                                       \
                        _Device->MsiCapability                                \
                            + FIELD_OFFSET(PCI_MSI_CAPABILITY, RegisterName), \
                        RTL_FIELD_SIZE(PCI_MSI_CAPABILITY, RegisterName)      \
                        );                                                    \
}

#define PciWriteMsiCapabilityRegister(Device, RegisterName, Buffer)            \
{                                                                              \
    PPCI_DEVICE _Device = (Device);                                            \
    PCI_ASSERT(_Device->MsiCapability);                                        \
    PciWriteDeviceConfig(_Device,                                              \
                         (PUCHAR)Buffer,                                       \
                         _Device->MsiCapability                                \
                             + FIELD_OFFSET(PCI_MSI_CAPABILITY, RegisterName), \
                         RTL_FIELD_SIZE(PCI_MSI_CAPABILITY, RegisterName)      \
                         );                                                    \
}

#define PciReadMsiXCapabilityRegister(Device, RegisterName, Buffer)            \
{                                                                              \
    PPCI_DEVICE _Device = (Device);                                            \
    PCI_ASSERT(_Device->MsiXCapability);                                       \
    PciReadDeviceConfig(_Device,                                               \
                        (PUCHAR)Buffer,                                        \
                        _Device->MsiXCapability                                \
                            + FIELD_OFFSET(PCI_MSIX_CAPABILITY, RegisterName), \
                        RTL_FIELD_SIZE(PCI_MSIX_CAPABILITY, RegisterName)      \
                        );                                                     \
}

#define PciWriteMsiXCapabilityRegister(Device, RegisterName, Buffer)            \
{                                                                               \
    PPCI_DEVICE _Device = (Device);                                             \
    PCI_ASSERT(_Device->MsiXCapability);                                        \
    PciWriteDeviceConfig(_Device,                                               \
                         (PUCHAR)Buffer,                                        \
                         _Device->MsiXCapability                                \
                             + FIELD_OFFSET(PCI_MSIX_CAPABILITY, RegisterName), \
                         RTL_FIELD_SIZE(PCI_MSIX_CAPABILITY, RegisterName)      \
                         );                                                     \
}

#define PciReadHtMsiCapabilityRegister(Device, RegisterName, Buffer)           \
{                                                                              \
    PPCI_DEVICE _Device = (Device);                                            \
    PCI_ASSERT(_Device->HtMsiMapCapability);                                   \
    PciReadDeviceConfig(_Device,                                               \
                        (PUCHAR)Buffer,                                        \
                        _Device->HtMsiMapCapability                            \
                            + FIELD_OFFSET(PCI_HT_CAPABILITY, RegisterName),   \
                        RTL_FIELD_SIZE(PCI_HT_CAPABILITY, RegisterName)        \
                        );                                                     \
}

#define PciWriteHtMsiCapabilityRegister(Device, RegisterName, Buffer)          \
{                                                                              \
    PPCI_DEVICE _Device = (Device);                                            \
    PCI_ASSERT(_Device->HtMsiMapCapability);                                   \
    PciWriteDeviceConfig(_Device,                                              \
                         (PUCHAR)Buffer,                                       \
                         _Device->HtMsiMapCapability                           \
                             + FIELD_OFFSET(PCI_HT_CAPABILITY, RegisterName),  \
                         RTL_FIELD_SIZE(PCI_HT_CAPABILITY, RegisterName)       \
                         );                                                    \
}

#define PciReadSlotConfigRegister(Bus, Slot, RegisterName, Buffer)           \
    PciReadSlotConfig(Bus,                                                   \
                      Slot,                                                  \
                      (PUCHAR)Buffer,                                        \
                      FIELD_OFFSET(PCI_CONFIG_HEADER, RegisterName),         \
                      CONFIG_FIELD_LENGTH(RegisterName)                      \
                      )

#define PciWriteSlotConfigRegister(Bus, Slot, RegisterName, Buffer)          \
    PciWriteSlotConfig(Bus,                                                  \
                      Slot,                                                  \
                      (PUCHAR)Buffer,                                        \
                      FIELD_OFFSET(PCI_CONFIG_HEADER, RegisterName),         \
                      CONFIG_FIELD_LENGTH(RegisterName)                      \
                      )

#define PciReadBridgeConfigRegister(Bridge, RegisterName, Buffer)            \
    PciReadBridgeConfig(Bridge,                                              \
                        (PUCHAR)Buffer,                                      \
                        FIELD_OFFSET(PCI_CONFIG_HEADER, RegisterName),       \
                        CONFIG_FIELD_LENGTH(RegisterName)                    \
                        )

#define PciWriteBridgeConfigRegister(Bridge, RegisterName, Buffer)           \
    PciWriteBridgeConfig(Bridge,                                             \
                         (PUCHAR)Buffer,                                     \
                         FIELD_OFFSET(PCI_CONFIG_HEADER, RegisterName),      \
                         CONFIG_FIELD_LENGTH(RegisterName)                   \
                         )

#define PciReadExpressRegister(Port, RegisterName, Buffer)                     \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT(_Device->ExpressCapability);                                    \
    PciReadDeviceConfig(_Device,                                               \
                        (PUCHAR)Buffer,                                        \
                        _Device->ExpressCapability +                           \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

#define PciReadExpressRegisterEx(Device, RegisterName, Buffer)                 \
{                                                                              \
    PCI_ASSERT(Device->ExpressCapability);                                     \
    PciReadDeviceConfig(Device,                                                \
                        (PUCHAR)Buffer,                                        \
                        Device->ExpressCapability +                            \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

#define PciWriteExpressRegister(Port, RegisterName, Buffer)                    \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT(_Device->ExpressCapability);                                    \
    PciWriteDeviceConfig(_Device,                                              \
                        (PUCHAR)Buffer,                                        \
                        _Device->ExpressCapability +                           \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

#define PciWriteExpressRegisterEx(Device, RegisterName, Buffer)                \
{                                                                              \
    PCI_ASSERT(Device->ExpressCapability);                                     \
    PciWriteDeviceConfig(Device,                                               \
                        (PUCHAR)Buffer,                                        \
                        Device->ExpressCapability +                            \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

#define PciReadSriovRegister(Port, RegisterName, Buffer)                       \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT((Port)->SriovCapability);                                       \
    PciReadDeviceConfig(_Device,                                               \
                        (PUCHAR)Buffer,                                        \
                         (Port)->SriovCapability +                             \
                            FIELD_OFFSET(PCI_EXPRESS_SRIOV_CAPABILITY,         \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_SRIOV_CAPABILITY,           \
                                       RegisterName)                           \
                        );                                                     \
}

#define PciWriteSriovRegister(Port, RegisterName, Buffer)                      \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT((Port)->SriovCapability);                                       \
    PciWriteDeviceConfig(_Device,                                              \
                        (PUCHAR)Buffer,                                        \
                         (Port)->SriovCapability +                             \
                            FIELD_OFFSET(PCI_EXPRESS_SRIOV_CAPABILITY,         \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_SRIOV_CAPABILITY,           \
                                       RegisterName)   \
                        );                                                     \
}

//
// On specific Intel root ports, the ACS capability is malformed.  The control
// register, which should reside at offset 6 is actually at offset 8.
// PCI_EXPRESS_ACS_CAPABILITY_ERRATA represents this PCI sig non-compliant 
// structure.  The hackflag is set on a per device basis based on rules in
// the errata manager.
//

typedef struct _PCI_EXPRESS_ACS_CAPABILITY_INCORRECT {
    PCI_EXPRESS_ENHANCED_CAPABILITY_HEADER  Header;
    PCI_EXPRESS_ACS_CAPABILITY_REGISTER     Capability;
    USHORT                                  Reserved;
    PCI_EXPRESS_ACS_CONTROL                 Control;
    ULONG                                   EgressControl[1];
} PCI_EXPRESS_ACS_CAPABILITY_INCORRECT;

#define PciReadAcsRegister(Port, RegisterName, Buffer)                     \
{                                                                          \
    PPCI_DEVICE _Device = (Port)->Device;                                  \
    PCI_ASSERT((Port)->AcsCapability);                                     \
    if (_Device->HackFlags & PCI_HACK_ACS_CAPABILITY_INCORRECT_FORMAT) {   \
        PciReadDeviceConfig(                                               \
            _Device,                                                       \
            (PUCHAR)Buffer,                                                \
            (Port)->AcsCapability +                                        \
                FIELD_OFFSET(PCI_EXPRESS_ACS_CAPABILITY_INCORRECT, RegisterName), \
            RTL_FIELD_SIZE(PCI_EXPRESS_ACS_CAPABILITY_INCORRECT, RegisterName) \
            );                                                             \
    } else {                                                               \
        PciReadDeviceConfig(                                               \
            _Device,                                                       \
            (PUCHAR)Buffer,                                                \
            (Port)->AcsCapability +                                        \
                FIELD_OFFSET(PCI_EXPRESS_ACS_CAPABILITY, RegisterName),    \
            RTL_FIELD_SIZE(PCI_EXPRESS_ACS_CAPABILITY, RegisterName)       \
            );                                                             \
    }                                                                      \
}

#define PciWriteAcsRegister(Port, RegisterName, Buffer)                     \
{                                                                          \
    PPCI_DEVICE _Device = (Port)->Device;                                  \
    PCI_ASSERT((Port)->AcsCapability);                                     \
    if (_Device->HackFlags & PCI_HACK_ACS_CAPABILITY_INCORRECT_FORMAT) {   \
        PciWriteDeviceConfig(                                              \
            _Device,                                                       \
            (PUCHAR)Buffer,                                                \
            (Port)->AcsCapability +                                        \
                FIELD_OFFSET(PCI_EXPRESS_ACS_CAPABILITY_INCORRECT, RegisterName), \
            RTL_FIELD_SIZE(PCI_EXPRESS_ACS_CAPABILITY_INCORRECT, RegisterName) \
            );                                                             \
    } else {                                                               \
        PciWriteDeviceConfig(                                              \
            _Device,                                                       \
            (PUCHAR)Buffer,                                                \
            (Port)->AcsCapability +                                        \
                FIELD_OFFSET(PCI_EXPRESS_ACS_CAPABILITY, RegisterName),    \
            RTL_FIELD_SIZE(PCI_EXPRESS_ACS_CAPABILITY, RegisterName)       \
            );                                                             \
    }                                                                      \
}

#define PciReadAriRegister(Port, RegisterName, Buffer)                         \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT((Port)->AriCapability);                                         \
    PciReadDeviceConfig(_Device,                                               \
                        (PUCHAR)Buffer,                                        \
                        (Port)->AriCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_ARI_CAPABILITY,           \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_ARI_CAPABILITY,             \
                            RegisterName)                                      \
                        );                                                     \
}

#define PciReadTphRequesterRegister(Port, RegisterName, Buffer)                \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT((Port)->TphRequesterCapability);                                \
    PciReadDeviceConfig(_Device,                                               \
                        (PUCHAR)Buffer,                                        \
                         (Port)->TphRequesterCapability +                      \
                            FIELD_OFFSET(PCI_EXPRESS_TPH_REQUESTER_CAPABILITY, \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_TPH_REQUESTER_CAPABILITY,   \
                                       RegisterName)                           \
                        );                                                     \
}

#define PciWriteTphRequesterRegister(Port, RegisterName, Buffer)               \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT((Port)->TphRequesterCapability);                                \
    PciWriteDeviceConfig(_Device,                                              \
                        (PUCHAR)Buffer,                                        \
                         (Port)->TphRequesterCapability +                      \
                            FIELD_OFFSET(PCI_EXPRESS_TPH_REQUESTER_CAPABILITY, \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_TPH_REQUESTER_CAPABILITY,   \
                                       RegisterName)                           \
                        );                                                     \
}

#define PciReadLtrRegister(Port, RegisterName, Buffer)                         \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT((Port)->LtrCapability);                                         \
    PciReadDeviceConfig(_Device,                                               \
                        (PUCHAR)Buffer,                                        \
                         (Port)->LtrCapability +                               \
                            FIELD_OFFSET(PCI_EXPRESS_LTR_CAPABILITY,           \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_LTR_CAPABILITY,             \
                                       RegisterName)                           \
                        );                                                     \
}

#define PciWriteLtrRegister(Port, RegisterName, Buffer)                        \
{                                                                              \
    PPCI_DEVICE _Device = (Port)->Device;                                      \
    PCI_ASSERT((Port)->LtrCapability);                                         \
    PciWriteDeviceConfig(_Device,                                              \
                        (PUCHAR)Buffer,                                        \
                         (Port)->LtrCapability +                               \
                            FIELD_OFFSET(PCI_EXPRESS_LTR_CAPABILITY,           \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_LTR_CAPABILITY,             \
                                       RegisterName)                           \
                        );                                                     \
}

#define PciReadExpressBridgeRegister(Bridge, RegisterName, Buffer)             \
{                                                                              \
    PPCI_BRIDGE _Bridge_ = (Bridge)->PciBridge;                                \
    PCI_ASSERT(_Bridge_->ExpressCapability);                                   \
    PciReadBridgeConfig(_Bridge_,                                              \
                        (PUCHAR)Buffer,                                        \
                        _Bridge_->ExpressCapability +                          \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

#define PciReadExpressBridgeRegisterEx(Bridge, RegisterName, Buffer)           \
{                                                                              \
    PCI_ASSERT(Bridge->ExpressCapability);                                     \
    PciReadBridgeConfig(Bridge,                                                \
                        (PUCHAR)Buffer,                                        \
                        Bridge->ExpressCapability +                            \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

#define PciWriteExpressBridgeRegister(Bridge, RegisterName, Buffer)            \
{                                                                              \
    PPCI_BRIDGE _Bridge_ = (Bridge)->PciBridge;                                \
    PCI_ASSERT(_Bridge_->ExpressCapability);                                   \
    PciWriteBridgeConfig(_Bridge_,                                             \
                        (PUCHAR)Buffer,                                        \
                        _Bridge_->ExpressCapability +                          \
                            FIELD_OFFSET(PCI_EXPRESS_CAPABILITY, RegisterName),\
                        RTL_FIELD_SIZE(PCI_EXPRESS_CAPABILITY, RegisterName)   \
                        );                                                     \
}

//
// The following macros read and write AER information from/to extended
// config space for an EXPRESS_PORT and an EXPRESS_BRIDGE.
//

#define PciReadExpressAerCapabilities(Port, Buffer)                            \
    PCI_ASSERT((Port)->AerCapability);                                         \
    PciReadDeviceConfig((Port)->Device,                                        \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AerCapability,                                 \
                        FIELD_OFFSET(PCI_EXPRESS_AER_CAPABILITY,               \
                                     SecUncorrectableErrorStatus)              \
                        );                                                     \

#define PciWriteExpressAerCapabilities(Port, Buffer)                           \
    PCI_ASSERT((Port)->AerCapability);                                         \
    PciWriteDeviceConfig((Port)->Device,                                       \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AerCapability,                                 \
                        FIELD_OFFSET(PCI_EXPRESS_AER_CAPABILITY,               \
                                     SecUncorrectableErrorStatus)              \
                        );

#define PciReadExpressAerRegister(Port, RegisterName, Buffer)                  \
    PCI_ASSERT((Port)->AerCapability);                                         \
    PciReadDeviceConfig((Port)->Device,                                        \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AerCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_AER_CAPABILITY,           \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_AER_CAPABILITY,             \
                                       RegisterName)                           \
                        );

#define PciWriteExpressAerRegister(Port, RegisterName, Buffer)                 \
    PCI_ASSERT((Port)->AerCapability);                                         \
    PciWriteDeviceConfig((Port)->Device,                                       \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AerCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_AER_CAPABILITY,           \
                                         RegisterName),                        \
                         RTL_FIELD_SIZE(PCI_EXPRESS_AER_CAPABILITY,            \
                                        RegisterName)                          \
                        );

#define PciReadExpressBridgeAerCapabilities(Port, Buffer)                      \
    PCI_ASSERT((Port)->AerCapability);                                         \
    PciReadDeviceConfig((Port)->Device,                                        \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AerCapability,                                 \
                        sizeof(PCI_EXPRESS_AER_CAPABILITY)                     \
                        );                                                     \

#define PciWriteExpressBridgeAerCapabilities(Port, Buffer)                     \
    PCI_ASSERT((Port)->AerCapability);                                         \
    PciWriteDeviceConfig((Port)->Device,                                       \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AerCapability,                                 \
                         sizeof(PCI_EXPRESS_AER_CAPABILITY)                    \
                        );

#define PciReadExpressBridgeAerRegister(Port, RegisterName, Buffer)            \
    PCI_ASSERT((Port)->AerCapability);                                         \
    PciReadDeviceConfig((Port)->Device,                                        \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AerCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_AER_CAPABILITY,           \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_AER_CAPABILITY,             \
                                       RegisterName)                           \
                        );

#define PciWriteExpressBridgeAerRegister(Port, RegisterName, Buffer)           \
    PCI_ASSERT((Port)->AerCapability);                                         \
    PciWriteDeviceConfig((Port)->Device,                                       \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AerCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_AER_CAPABILITY,           \
                                         RegisterName),                        \
                         RTL_FIELD_SIZE(PCI_EXPRESS_AER_CAPABILITY,            \
                                        RegisterName)                          \
                        );

#define PciReadExpressRootPortAerCapabilities(Bridge, Buffer)                  \
    PCI_ASSERT((Bridge)->AerCapability);                                       \
    PciReadBridgeConfig((Bridge)->PciBridge,                                   \
                        (PUCHAR)(Buffer),                                      \
                        (Bridge)->AerCapability,                               \
                        sizeof(PCI_EXPRESS_ROOTPORT_AER_CAPABILITY)            \
                        );

#define PciWriteExpressRootPortAerCapabilities(Bridge, Buffer)                 \
    PCI_ASSERT((Bridge)->AerCapability);                                       \
    PciWriteBridgeConfig((Bridge)->PciBridge,                                  \
                        (PUCHAR)(Buffer),                                      \
                        (Bridge)->AerCapability,                               \
                         sizeof(PCI_EXPRESS_ROOTPORT_AER_CAPABILITY)           \
                        );

#define PciReadExpressRootPortAerRegister(Bridge, RegisterName, Buffer)        \
    PCI_ASSERT((Bridge)->AerCapability);                                       \
    PciReadBridgeConfig((Bridge)->PciBridge,                                   \
                        (PUCHAR)(Buffer),                                      \
                        (Bridge)->AerCapability +                              \
                            FIELD_OFFSET(PCI_EXPRESS_ROOTPORT_AER_CAPABILITY,  \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_ROOTPORT_AER_CAPABILITY,    \
                                       RegisterName)                           \
                        );

#define PciWriteExpressRootPortAerRegister(Bridge, RegisterName, Buffer)       \
    PCI_ASSERT((Bridge)->AerCapability);                                       \
    PciWriteBridgeConfig((Bridge)->PciBridge,                                  \
                        (PUCHAR)(Buffer),                                      \
                        (Bridge)->AerCapability +                              \
                            FIELD_OFFSET(PCI_EXPRESS_ROOTPORT_AER_CAPABILITY,  \
                                         RegisterName),                        \
                         RTL_FIELD_SIZE(PCI_EXPRESS_ROOTPORT_AER_CAPABILITY,   \
                                        RegisterName)                          \
                        );

#define PciIsAerGranted(Root)                                                  \
     (((Root)->HardwareCapability.OscControlGranted.u.ExpressAdvancedErrorReporting != 0) || \
     ((PciSystemWideHackFlags & PCI_SYS_HACK_CONTROL_EXPRESS_FEATURES) != 0))

#define PciIsExpressGranted(Root)                                             \
     (((Root)->HardwareCapability.OscControlGranted.u.ExpressCapabilityStructure != 0) || \
     ((PciSystemWideHackFlags & PCI_SYS_HACK_CONTROL_EXPRESS_FEATURES) != 0))

#define PCI_DEVICE_CONFIGURED_FOR_AER(_Device)                           \
    ((PciIsAerGranted((_Device)->Root)) &&                               \
     ((_Device)->ExpressPort != NULL) &&                                 \
     (((_Device)->ExpressPort->ErrorSource.Flags & WHEA_ERROR_SOURCE_FLAG_FIRMWAREFIRST) == 0))

//
// The following macros read and write VC information from/to extended
// config space for an EXPRESS_PORT.
//

#define PciReadExpressVcRegister(Port, RegisterName, Buffer)                   \
    PCI_ASSERT((Port)->VcCapability);                                          \
    PciReadDeviceConfig((Port)->Device,                                        \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->VcCapability +                                 \
                           FIELD_OFFSET(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY,\
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY, \
                                       RegisterName)                           \
                        );

#define PciWriteExpressVcRegister(Port, RegisterName, Buffer)                  \
    PCI_ASSERT((Port)->VcCapability);                                          \
    PciWriteDeviceConfig((Port)->Device,                                       \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->VcCapability +                                 \
                           FIELD_OFFSET(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY,\
                                         RegisterName),                        \
                         RTL_FIELD_SIZE(PCI_EXPRESS_VIRTUAL_CHANNEL_CAPABILITY,\
                                        RegisterName)                          \
                        );

//
// Macros to read and write ACS information from/to extended
// config space for an EXPRESS_BRIDGE
//

#define PciReadExpressBridgeAcsRegister(Bridge, RegisterName, Buffer)               \
{                                                                                   \
    PCI_ASSERT((Bridge)->AcsCapability);                                            \
    if ((Bridge)->PciBridge->AcsCapIncorrectFormat) {                               \
        PciReadBridgeConfig((Bridge)->PciBridge,                                    \
                            (PUCHAR)(Buffer),                                       \
                            (Bridge)->AcsCapability +                               \
                                FIELD_OFFSET(PCI_EXPRESS_ACS_CAPABILITY_INCORRECT,  \
                                             RegisterName),                         \
                            RTL_FIELD_SIZE(PCI_EXPRESS_ACS_CAPABILITY_INCORRECT,    \
                                           RegisterName)                            \
                            );                                                      \
    } else {                                                                        \
        PciReadBridgeConfig((Bridge)->PciBridge,                                    \
                            (PUCHAR)(Buffer),                                       \
                            (Bridge)->AcsCapability +                               \
                                FIELD_OFFSET(PCI_EXPRESS_ACS_CAPABILITY,            \
                                             RegisterName),                         \
                            RTL_FIELD_SIZE(PCI_EXPRESS_ACS_CAPABILITY,              \
                                           RegisterName)                            \
                            );                                                      \
    }                                                                               \
}

#define PciWriteExpressBridgeAcsRegister(Bridge, RegisterName, Buffer)             \
{                                                                                  \
    PCI_ASSERT((Bridge)->AcsCapability);                                           \
    if ((Bridge)->PciBridge->AcsCapIncorrectFormat) {                              \
        PciWriteBridgeConfig((Bridge)->PciBridge,                                  \
                            (PUCHAR)(Buffer),                                      \
                            (Bridge)->AcsCapability +                              \
                                FIELD_OFFSET(PCI_EXPRESS_ACS_CAPABILITY_INCORRECT, \
                                             RegisterName),                        \
                             RTL_FIELD_SIZE(PCI_EXPRESS_ACS_CAPABILITY_INCORRECT,  \
                                            RegisterName)                          \
                            );                                                     \
    } else {                                                                       \
        PciWriteBridgeConfig((Bridge)->PciBridge,                                  \
                            (PUCHAR)(Buffer),                                      \
                            (Bridge)->AcsCapability +                              \
                                FIELD_OFFSET(PCI_EXPRESS_ACS_CAPABILITY,           \
                                             RegisterName),                        \
                             RTL_FIELD_SIZE(PCI_EXPRESS_ACS_CAPABILITY,            \
                                            RegisterName)                          \
                            );                                                     \
    }                                                                              \
}

//
// Marcos to read/write from/to PASID registers.
//

#define PciReadExpressPasidRegister(Port, RegisterName, Buffer)                \
    PCI_ASSERT((Port)->PasidCapability);                                       \
    PciReadDeviceConfig((Port)->Device,                                        \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->PasidCapability +                              \
                            FIELD_OFFSET(PCI_EXPRESS_PASID_CAPABILITY,         \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_PASID_CAPABILITY,           \
                                       RegisterName)                           \
                        );

#define PciWriteExpressPasidRegister(Port, RegisterName, Buffer)               \
    PCI_ASSERT((Port)->PasidCapability);                                       \
    PciWriteDeviceConfig((Port)->Device,                                       \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->PasidCapability +                              \
                            FIELD_OFFSET(PCI_EXPRESS_PASID_CAPABILITY,         \
                                         RegisterName),                        \
                         RTL_FIELD_SIZE(PCI_EXPRESS_PASID_CAPABILITY,          \
                                        RegisterName)                          \
                        );

//
// Marcos to read/write from/to ATS registers.
//

#define PciReadExpressAtsRegister(Port, RegisterName, Buffer)                  \
    PCI_ASSERT((Port)->AtsCapability);                                         \
    PciReadDeviceConfig((Port)->Device,                                        \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AtsCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_ATS_CAPABILITY,           \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_ATS_CAPABILITY,             \
                                       RegisterName)                           \
                        );

#define PciWriteExpressAtsRegister(Port, RegisterName, Buffer)                 \
    PCI_ASSERT((Port)->AtsCapability);                                         \
    PciWriteDeviceConfig((Port)->Device,                                       \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->AtsCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_ATS_CAPABILITY,           \
                                         RegisterName),                        \
                         RTL_FIELD_SIZE(PCI_EXPRESS_ATS_CAPABILITY,            \
                                        RegisterName)                          \
                        );

//
// Marcos to read/write from/to PRI registers.
//

#define PciReadExpressPriRegister(Port, RegisterName, Buffer)                  \
    PCI_ASSERT((Port)->PriCapability);                                         \
    PciReadDeviceConfig((Port)->Device,                                        \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->PriCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_PRI_CAPABILITY,           \
                                         RegisterName),                        \
                        RTL_FIELD_SIZE(PCI_EXPRESS_PRI_CAPABILITY,             \
                                       RegisterName)                           \
                        );

#define PciWriteExpressPriRegister(Port, RegisterName, Buffer)                 \
    PCI_ASSERT((Port)->PriCapability);                                         \
    PciWriteDeviceConfig((Port)->Device,                                       \
                        (PUCHAR)(Buffer),                                      \
                        (Port)->PriCapability +                                \
                            FIELD_OFFSET(PCI_EXPRESS_PRI_CAPABILITY,           \
                                         RegisterName),                        \
                         RTL_FIELD_SIZE(PCI_EXPRESS_PRI_CAPABILITY,            \
                                        RegisterName)                          \
                        );

//
// --------------------------------------------------------------------- debug.c
//

//
// Debug Levels
//
#define ERROR_LEVEL     ((1 << DPFLTR_ERROR_LEVEL) | DPFLTR_MASK)
#define WARNING_LEVEL   ((1 << DPFLTR_WARNING_LEVEL) | DPFLTR_MASK)
#define TRACE_LEVEL     ((1 << DPFLTR_TRACE_LEVEL) | DPFLTR_MASK)
#define INFO_LEVEL      ((1 << DPFLTR_INFO_LEVEL) | DPFLTR_MASK)

//
// The following can be passed into the TRACE macro to get
// tracing of different areas.
//
#define PNP_TRACE_LEVEL         (0x00000100 | DPFLTR_MASK)
#define PO_TRACE_LEVEL          (0x00000200 | DPFLTR_MASK)
#define ADD_DEVICE_TRACE_LEVEL  (0x00000400 | DPFLTR_MASK)
#define ENUM_TRACE_LEVEL        (0x00000800 | DPFLTR_MASK)
#define RESOURCE_TRACE_LEVEL    (0x00001000 | DPFLTR_MASK)
#define ARBITER_TRACE_LEVEL     (0x00002000 | DPFLTR_MASK)
#define DISPATCH_TRACE_LEVEL    (0x00004000 | DPFLTR_MASK)
#define WAKE_TRACE_LEVEL        (0x00008000 | DPFLTR_MASK)
#define INIT_TRACE_LEVEL        (0x00010000 | DPFLTR_MASK)
#define CONFIG_TRACE_LEVEL      (0x00020000 | DPFLTR_MASK)
#define INTERFACE_TRACE_LEVEL   (0x00040000 | DPFLTR_MASK)
#define ROM_TRACE_LEVEL         (0x00080000 | DPFLTR_MASK)
#define BUS_NUMBER_TRACE_LEVEL  (0x00100000 | DPFLTR_MASK)
#define HOTPLUG_TRACE_LEVEL     (0x00200000 | DPFLTR_MASK)
#define AER_TRACE_LEVEL         (0x00400000 | DPFLTR_MASK)
#define DELAY_TRACE_LEVEL       (0x00800000 | DPFLTR_MASK)
#define LATERESTORE_TRACE_LEVEL (0x01000000 | DPFLTR_MASK)

#if DBG

    //
    // Debugging will deadlock the machine at IRQL IPI_LEVEL or above
    //
    #define PciDebuggingAllowed()   \
        (KeGetCurrentIrql() < IPI_LEVEL)

    //
    // Debug macros that should be used across the board.
    //
    #define ERROR          if (PciDebuggingAllowed()) PciDebugError
    #define WARN           if (PciDebuggingAllowed()) PciDebugWarn
    #define TRACE          if (PciDebuggingAllowed()) PciDebugTrace
    #define TRACE_CONTINUE if (PciDebuggingAllowed()) PciDebugMsg
    #define INFO           if (PciDebuggingAllowed()) PciDebugInfo
    #define VERIFIER WARN
    #define DebugDumpConfigHeader          if (PciDebuggingAllowed()) PciDebugDumpConfigHeader
    #define DebugDumpDeviceCapabilities    if (PciDebuggingAllowed()) PciDebugDumpDeviceCapabilities
    #define DebugDumpInvalidObject       if (PciDebuggingAllowed()) PciDebugDumpInvalidObject
    #define DebugDumpCapabilitiesList(_Level, _CapList)          \
            if (PciDebuggingAllowed())                           \
                PciDebugDumpCapabilitiesList(_Level, _CapList)
    #define DebugDumpResourceList(_Level, _ResList)         \
            if (PciDebuggingAllowed())                      \
                PciDebugDumpResourceList(_Level, _ResList)
    #define DebugDumpResourceArray(_Level, _ResArray, _Count)         \
            if (PciDebuggingAllowed())                                \
                PciDebugDumpResourceArray(_Level, _ResArray, _Count)
    #define DebugDumpBridgeRequirements(_Level, _Reqs)                \
            if (PciDebuggingAllowed())                                \
                PciDebugDumpBridgeRequirements(_Level, _Reqs)
    #define DebugDumpArbiterAllocationState(_Level, _State)           \
            if (PciDebuggingAllowed())                                \
                PciDebugDumpArbiterAllocationState(_Level, _State)
    #define DebugCheckRegisterSettings(_Device)          \
            if (PciDebuggingAllowed())                   \
                PciDebugCheckRegisterSettings(_Device)
    #define DebugDumpRequirementsList(_Level, _ReqList)         \
            if (PciDebuggingAllowed())                          \
                PciDebugDumpRequirementsList(_Level, _ReqList)
    #define DebugDumpCmResource(_Level, _Resource)          \
            if (PciDebuggingAllowed())                      \
                PciDebugDumpCmResource(_Level, _Resource)
    #define DebugDumpIoResource(_Level, _Resource)          \
            if (PciDebuggingAllowed())                      \
                PciDebugDumpIoResource(_Level, _Resource)
    #define DebugSetDebugFlag(_Device, _Flag)    (_Device)->DebugFlags |= (_Flag)
    #define DebugClearDebugFlag(_Device, _Flag)    (_Device)->DebugFlags &= ~(_Flag)
    #define DebugDumpIrpStack(_IrpSp, _Extension, _TableMax)           \
            if (PciDebuggingAllowed())                                 \
                PciDebugDumpIrpStack(_IrpSp, _Extension, _TableMax)
    #define DebugDumpIrpStatus(_MajorFunction, _Status)                 \
            if (PciDebuggingAllowed())                                  \
                PciDebugDumpIrpStatus(_MajorFunction, _Status)
    #define DebugDumpBitMap(_Level, _Bitmap)                \
            if (PciDebuggingAllowed())                      \
                PciDebugDumpBitMap(_Level, _Bitmap)
    #define DebugBreakOnIrp(_DeviceObject, _IrpSp)          \
            PciDebugBreakOnIrp(_DeviceObject, _IrpSp)
    #define DebugStringFromGuid(_Guid, _UnicodeString)      \
            RtlStringFromGUID(_Guid, _UnicodeString)

    //
    // More debug macros - these functions are defined in hotplug.c since they
    // are hotplug only.
    //
    #define DebugDumpHotplugEvent(_Slot, _Event)            \
            if (PciDebuggingAllowed())                      \
                PciDebugDumpHotPlugEvent(_Slot, _Event)
    #define DebugDumpHotplugAction(_Action)                 \
            if (PciDebuggingAllowed())                      \
                PciDebugDumpHotPlugAction(_Action)
    #define DebugDumpHotplugSlotState(_Slot)                \
            if (PciDebuggingAllowed())                      \
                PciDebugDumpHotPlugSlotState(_Slot)

    //
    // ASSERT macros
    //
    #define PCI_ASSERT  \
        if (PciDebuggingAllowed()) NT_ASSERT

    #define PCI_ASSERTMSG  \
        if (PciDebuggingAllowed()) NT_ASSERTMSG

    VOID
    PciDebugError(
        __in __format_string PSTR Format,
        ...
        );

    VOID
    PciDebugWarn(
        __in __format_string PSTR Format,
        ...
        );

    VOID
    PciDebugTrace(
        __in ULONG Level,
        __in __format_string PSTR Format,
        ...
        );

    VOID
    PciDebugMsg(
        __in ULONG Level,
        __in __format_string PSTR Format,
        ...
        );

    VOID
    PciDebugInfo(
        __in __format_string PSTR Format,
        ...
        );

    VOID
    PciDebugDumpConfigHeader(
        __in ULONG Level,
        __in PPCI_CONFIG_HEADER ConfigHeader
        );

    VOID
    PciDebugDumpDeviceCapabilities(
        __in ULONG Level,
        __in PDEVICE_CAPABILITIES Capabilities
        );

    VOID
    PciDebugDumpCmResource(
        __in ULONG Level,
        __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor
        );

    VOID
    PciDebugDumpResourceList(
        __in ULONG Level,
        __in PCM_RESOURCE_LIST ResourceList
        );

    VOID
    PciDebugDumpResourceArray(
        __in ULONG Level,
        __in PCM_PARTIAL_RESOURCE_DESCRIPTOR ResourceArray,
        __in ULONG Count
        );

    VOID
    PciDebugDumpBridgeRequirements(
        __in ULONG Level,
        __in PPCI_BRIDGE_REQUIREMENTS Requirements
        );

    VOID
    PciDebugDumpArbiterAllocationState(
        __in ULONG Level,
        __in PARBITER_ALLOCATION_STATE State
        );

    VOID
    PciDebugCheckRegisterSettings(
        __inout PPCI_DEVICE Device
        );

    VOID
    PciDebugDumpIoResource(
        __in ULONG Level,
        __in PIO_RESOURCE_DESCRIPTOR Descriptor
        );

    VOID
    PciDebugDumpRequirementsList(
        __in ULONG Level,
        __in PIO_RESOURCE_REQUIREMENTS_LIST List
        );

    VOID
    PciDebugDumpCapabilitiesList(
        __in ULONG Level,
        __in PPCI_DEVICE Device
        );

    VOID
    PciDebugDumpIrpStack(
        __in PIO_STACK_LOCATION IrpSp,
        __in PPCI_COMMON_EXTENSION DeviceExtension,
        __in ULONG MinorTableMax
        );

    VOID
    PciDebugDumpIrpStatus(
        __in ULONG MajorFunction,
        __in NTSTATUS Status
        );

    VOID
    PciDebugDumpInvalidObject(
        __in PDEVICE_OBJECT DeviceObject,
        __in PIRP Irp,
        __in PUCHAR MajorString
        );

    VOID
    PciDebugDumpBitMap(
        __in ULONG Level,
        __in PRTL_BITMAP BitMap
        );

    PUCHAR
    PciDebugPnpIrpTypeToText(
        __in ULONG IrpMinorCode
        );

    PUCHAR
    PciDebugPoIrpTypeToText(
        __in ULONG IrpMinorCode
        );

    BOOLEAN
    PciDebugBreakOnIrp(
        __in PDEVICE_OBJECT DeviceObject,
        __in PIO_STACK_LOCATION IrpStack
        );

#else

    #define ERROR          if(0)
    #define WARN           if(0)
    #define TRACE          if(0)
    #define TRACE_CONTINUE if(0)
    #define INFO           if(0)
    #define VERIFIER       if(0)
    #define DebugDumpConfigHeader        if (0)
    #define DebugDumpDeviceCapabilities  if (0)
    #define DebugDumpInvalidObject       if (0)
    #define DebugDumpCapabilitiesList(_Level, _CapList)
    #define DebugDumpResourceList(_Level, _ResList)
    #define DebugDumpResourceArray(_Level, _ResArray, _Count)
    #define DebugDumpBridgeRequirements(_Level, _Reqs)
    #define DebugDumpArbiterAllocationState(_Level, _State)
    #define DebugCheckRegisterSettings(_Device)
    #define DebugDumpRequirementsList(_Level, _ReqList)
    #define DebugDumpCmResource(_Level, _Resource)
    #define DebugDumpIoResource(_Level, _Resource)
    #define DebugSetDebugFlag(_Device, _Flag)
    #define DebugClearDebugFlag(_Device, _Flag)
    #define DebugDumpIrpStack(_IrpSp, _Extension, _TableMax)
    #define DebugDumpIrpStatus(_MajorFunction, _Status)
    #define DebugBreakOnIrp(_DeviceObject, _IrpSp) FALSE
    #define DebugDumpBitMap(_Level, _Bitmap)
    #define DebugStringFromGuid(_Guid, _UnicodeString) STATUS_UNSUCCESSFUL

    #define DebugDumpHotplugEvent(_Slot, _Event)
    #define DebugDumpHotplugAction(_Action)
    #define DebugDumpHotplugSlotState(_Slot)

    #define PCI_ASSERT(_exp)
    #define PCI_ASSERTMSG(_msg,_exp)

#endif

//
// -------------------------------------------------------------------- delay.c
//

NTSTATUS
PciDevice_Delay(
    __in PPCI_DEVICE Device,
    __in PCI_DELAY_INFO DelayType
    );

NTSTATUS
PciDevice_Stall(
    __in PPCI_DEVICE Device,
    __in ULONG MicroSeconds
    );

//
// -------------------------------------------------------------------- devapi.c
//

//
// Name of the volatile key under the DeviceParameters key where data that needs
// to be persistent accross removes, but NOT reboots is stored
//
#define BIOS_CONFIG_KEY_NAME L"BiosConfig"

//
// Name of the the non-volatile key under the DeviceParameters key where data that
// needs to be persistent accross reboots is stored.
//
#define SLOT_INFO_KEY_NAME L"SlotPersistentInfo"

typedef struct _PCI_DEVICE_ID_DATA {

    USHORT VendorID;
    USHORT DeviceID;
    UCHAR  RevisionID;
    UCHAR  ProgIf;
    UCHAR  SubClass;
    UCHAR  BaseClass;
    USHORT SubVendorID;
    USHORT SubSystemID;
    UCHAR  HeaderType;

} PCI_DEVICE_ID_DATA, *PPCI_DEVICE_ID_DATA;

typedef struct _PCI_SLOT_PERSISTENT_INFO {

    USHORT Version;
    USHORT Size;
    PCI_DEVICE_ID_DATA IdData;

    //
    // Version 1 of this struct only contains
    // the PCI Express Serial Number info
    //
    ULONGLONG ExpressSerialNumber;

} PCI_SLOT_PERSISTENT_INFO, *PPCI_SLOT_PERSISTENT_INFO;

#define PCI_INIT_SLOT_PERSISTENT_INFO(_Info)                    \
        (_Info)->Version = 1;                                   \
        (_Info)->Size = FIELD_OFFSET(PCI_SLOT_PERSISTENT_INFO,  \
                                     ExpressSerialNumber);      \
        (_Info)->ExpressSerialNumber = 0;

typedef struct _PCI_SAVED_CONFIGURATION {

    PCI_DEVICE_ID_DATA IdData;
    PCI_SAVE_RESTORE_DATA SaveRestore;
    PCI_DEVICE_RESOURCES Resources;

    struct {
        ULONG IoWindowDisabled:1;
        ULONG MemoryWindowDisabled:1;
        ULONG PrefetchWindowDisabled:1;
    } Bridge;

} PCI_SAVED_CONFIGURATION, *PPCI_SAVED_CONFIGURATION;

#define PCI_UNCONFIGURED_DEVICE(_Device, _SaveRestoreData)        \
    (!((_SaveRestoreData)->Command & (PCI_ENABLE_IO_SPACE         \
                                     | PCI_ENABLE_MEMORY_SPACE    \
                                     | PCI_ENABLE_BUS_MASTER))    \
        && ((_SaveRestoreData)->CacheLineSize == 0)               \
        && (((_SaveRestoreData)->LatencyTimer == 0) ||            \
             ((_Device)->PciXCapability &&                        \
              (_SaveRestoreData)->LatencyTimer == 0x40)))

typedef enum {
    PowerDownDevice,
    DisconnectDevice,
    KeepDeviceActive
} PCI_DISCONNECTION_POLICY;

typedef NTSTATUS (*PPCI_WALK_DEVICE_PATH_CALLBACK)(
    _In_ PPCI_BUS Bus,
    _Inout_ PVOID Context
    );

NTSTATUS
PciWalkDevicePath(
    _In_ PPCI_DEVICE Device,
    _In_ PPCI_WALK_DEVICE_PATH_CALLBACK Callback,
    _Inout_ PVOID Context,
    _Out_opt_ PPCI_BUS *Root
    );

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciUpdateHardware(
    __inout PPCI_DEVICE Device,
    __in_opt PVOID ReconnectDevice
    );

NTSTATUS
PciProbeForRequirements(
    __inout PPCI_DEVICE Device,
    __in_opt PVOID UnusedParameter
    );

BOOLEAN
PciDeviceConnected(
    __in PPCI_DEVICE Device
    );

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciConnectDevice(
    __inout PPCI_DEVICE Device
    );

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciDisconnectDevice(
    __inout PPCI_DEVICE Device
    );

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciReconnectDevice(
    __inout PPCI_DEVICE Device
    );

NTSTATUS
PciRestoreDeviceRegisters(
    __inout PPCI_DEVICE Device,
    __inout PPCI_SAVE_RESTORE_DATA SavedRegisters
    );

PCI_DISCONNECTION_POLICY
PciCanDisableDevice(
    __in PPCI_DEVICE Device
    );

PCI_DISCONNECTION_POLICY
PciCanDisableDeviceForPowerDown(
    __in PPCI_DEVICE Device,
    __in DEVICE_POWER_STATE PowerState,
    __in POWER_ACTION PowerAction
    );

NTSTATUS
PciReadSlotIdData(
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __out PPCI_DEVICE_ID_DATA IdData
    );

//
// NTSTATUS
// PciReadDeviceIdentification(
//     IN PPCI_DEVICE Device,
//     OUT PPCI_DEVICE_IDENTIFICATION Identification
//     )
//
#define PciReadDeviceIdData(_Device, _Id)     \
    PciReadSlotIdData((_Device)->Parent, (_Device)->Slot, _Id)

_IRQL_requires_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
PciGetSavedSlotInformation(
    _In_ PPCI_BUS Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ PWSTR InfoKeyName,
    _In_ ULONG SlotInfoSize,
    _Out_ PVOID SlotInfo
    );

//
// NTSTATUS
// PciGetSavedDeviceConfiguration(
//     IN PPCI_DEVICE Device,
//     OUT PPCI_SAVED_CONFIGURATION Config
//     )
//
#define PciGetSavedDeviceConfiguration(_Device, _Config)        \
    PciGetSavedSlotInformation((_Device)->Parent,               \
                               (_Device)->Slot,                 \
                               BIOS_CONFIG_KEY_NAME,            \
                               sizeof(PCI_SAVED_CONFIGURATION), \
                               (PVOID)_Config)

//
// NTSTATUS
// PciGetSavedBridgeConfig(
//     IN PPCI_BRIDGE Bridge,
//     OUT PPCI_SAVED_CONFIGURATION Config
//     )
//
#define PciGetSavedBridgeConfiguration(_Bridge, _Config)         \
    PciGetSavedSlotInformation((_Bridge)->Bus->ParentBus,        \
                               (_Bridge)->Slot,                  \
                               BIOS_CONFIG_KEY_NAME,             \
                               sizeof(PCI_SAVED_CONFIGURATION),  \
                               (PVOID)_Config)

#define PciGetSlotPersistentInformation(_Device, _Info)         \
    PciGetSavedSlotInformation((_Device)->Parent,               \
                               (_Device)->Slot,                 \
                               SLOT_INFO_KEY_NAME,              \
                               sizeof(PCI_SLOT_PERSISTENT_INFO),\
                               (PVOID)_Info)

_IRQL_requires_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
PciSaveSlotInformation(
    _In_ PPCI_DEVICE Device,
    _In_ PWSTR InfoKeyName,
    _In_ USHORT InfoKeySize,
    _In_ BOOLEAN IsVolatile,
    _In_ ULONG SlotInfoSize,
    _In_ PVOID SlotInfo
    );

#define PciSaveDeviceConfiguration(_Device, _Config)        \
    PciSaveSlotInformation(_Device,                         \
                           BIOS_CONFIG_KEY_NAME,            \
                           sizeof(BIOS_CONFIG_KEY_NAME),    \
                           TRUE,                            \
                           sizeof(PCI_SAVED_CONFIGURATION), \
                           (PVOID)_Config)

#define PciSaveSlotPersistentInfo(_Device, _Info)           \
    PciSaveSlotInformation(_Device,                         \
                           SLOT_INFO_KEY_NAME,              \
                           sizeof(SLOT_INFO_KEY_NAME),      \
                           FALSE,                           \
                           sizeof(PCI_SLOT_PERSISTENT_INFO),\
                           (PVOID)_Info)

NTSTATUS
PciUpdateSavedDeviceConfiguration(
    __in PPCI_DEVICE Device
    );

NTSTATUS
PciApplyHPX(
    __inout PPCI_DEVICE Device
    );

VOID
PciApplyHPP(
    __inout PPCI_DEVICE Device
    );

BOOLEAN
PciBridgeOnVGAPath(
    __in PPCI_DEVICE Device
    );

BOOLEAN
PciOnPostPath (
    __in PPCI_DEVICE Device
    );

BOOLEAN
PciSubtractiveDecodeBridge(
    __in PPCI_DEVICE Device
    );

BOOLEAN
PciSlotPresentInParentMethod(
    __in PPCI_DEVICE Device,
    __in ULONG Method
    );

BOOLEAN
PciDeviceOnDebugPath(
    __in PPCI_DEVICE Device,
    __in PPCI_SAVED_CONFIGURATION ConfigData
    );

BOOLEAN
PciIsSameDevice(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_ID_DATA IdData
    );

BOOLEAN
PciIsSameResource(
    __in PPCI_DEVICE_RESOURCES Resources,
    __in PPCI_DEVICE_RESOURCES SavedResources
    );

VOID
PciGetSaveRestoreData(
    __in PPCI_DEVICE Device,
    __out PPCI_SAVE_RESTORE_DATA HardwareRegisters
    );

VOID
PciUpdateSaveRestoreData(
    __in PPCI_DEVICE Device,
    __out PPCI_SAVE_RESTORE_DATA HardwareRegisters
    );

//
// -------------------------------------------------------------------- devpnp.c
//

//
// ---------------------------------------------------------------------- diag.c
//

NTSTATUS
PciDiagInitialize (
    VOID
    );

NTSTATUS
PciDiagUninitialize (
    VOID
    );

//
// ------------------------------------------------------------------ devpower.c
//

extern ULONG ExpressDevicesEnabledForWake;
extern KSPIN_LOCK ExpressWakeControlLock;

#define PCI_TO_NT_POWER_STATE(PciPowerState) \
    (PciPowerState + PowerDeviceD0)

#define NT_TO_PCI_POWER_STATE(NtPowerState) \
    (NtPowerState - PowerDeviceD0)

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciPowerUpDevice(
    _Inout_ PPCI_DEVICE Device
    );

DEVICE_POWER_STATE
PciGetHardwarePowerState (
    __in PPCI_DEVICE Device
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciSetHardwarePowerStateReset (
    _In_    PPCI_DEVICE         Device
    );

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciSetPowerChangeDelay (
    _In_ PPCI_DEVICE        Device,
    _In_ DEVICE_POWER_STATE OldState,
    _In_ DEVICE_POWER_STATE NewState
    );

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciStallForPowerChange(
    _In_ PPCI_DEVICE Device,
    _In_ DEVICE_POWER_STATE NewPowerState
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciSetHardwarePowerStatePassive (
    _In_ PPCI_DEVICE Device,
    _In_ DEVICE_POWER_STATE DeviceState
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PciSetHardwarePowerStateDispatch (
    _In_ PPCI_DEVICE Device,
    _In_ DEVICE_POWER_STATE DeviceState
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciDevice_RetireD0Irp (
    _Inout_ PPCI_DEVICE Device
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciDevice_WakeTriggered (
    _Inout_ PPCI_DEVICE Device,
    _In_ BOOLEAN SystemWake,
    _In_ NTSTATUS CompletionStatus
    );

//
// ------------------------------------------------------------------ dispatch.c
//

//
// Dispatch table entries
//
typedef enum _PCI_DISPATCH_STYLE {
    IRP_COMPLETE, // Complete IRP, adjust status as neccessary
    IRP_DOWNWARD, // Dispatch on the way down, adjust status as neccessary
    IRP_UPWARD,   // Dispatch on the way up, adjust status as neccessary
    IRP_DISPATCH  // Dispatch downward, don't touch afterwards
} PCI_DISPATCH_STYLE;

typedef NTSTATUS (*PCI_MN_DISPATCH_FUNCTION) (
    IN PIRP Irp,
    IN PIO_STACK_LOCATION IrpSp,
    IN PVOID DeviceExtension
    );

typedef struct _PCI_MN_DISPATCH_ENTRY {
    PCI_DISPATCH_STYLE        DispatchStyle;
    PCI_MN_DISPATCH_FUNCTION  DispatchFunction;

    //
    // This field indicates that the IRP needs to access the config space.
    // Hence, the power on the parent bus must be available. Force that by
    // taking the PciBusPowerReasonChildNeedsD0 reference.
    //

    BOOLEAN                   RequireConfigSpaceAccess;
} PCI_MN_DISPATCH_ENTRY, *PPCI_MN_DISPATCH_ENTRY;

typedef const PCI_MN_DISPATCH_ENTRY *PCPCI_MN_DISPATCH_ENTRY;

//
// VOID
// PciCompleteRequest(
//     IN OUT PIRP Irp,
//     IN NTSTATUS Status
//     );
//
#define PciCompleteRequest(_Irp_,_Status_)                      \
{                                                           \
    (_Irp_)->IoStatus.Status = (_Status_);                  \
    IoCompleteRequest((_Irp_), IO_NO_INCREMENT);            \
}

__drv_dispatchType(IRP_MJ_POWER)
__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH PciDispatchPnpPower;

__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
DRIVER_DISPATCH PciDispatchSystemControl;

__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH PciDispatchDeviceControl;

__drv_dispatchType(IRP_MJ_CREATE)
DRIVER_DISPATCH PciDispatchCreate;

NTSTATUS
PciPassIrp(
    __in PPCI_COMMON_EXTENSION DeviceExtension,
    __inout PIRP Irp
    );

NTSTATUS
PciCallDownIrpStack(
    __in PPCI_COMMON_EXTENSION DeviceExtension,
    __inout PIRP Irp
    );

NTSTATUS
PciIrpNotSupported(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    );

__drv_functionClass(KDEFERRED_ROUTINE)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
KDEFERRED_ROUTINE PciPowerDownDeviceTimerDpc;

__drv_functionClass(KDEFERRED_ROUTINE)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
KDEFERRED_ROUTINE PciPowerUpDeviceTimerDpc;

//
// ---------------------------------------------------------------------- enum.c
//

//
// When a device has this registry key under its plug and play key, it will
// indicate the extra features it supports/requires that can not be gathered
// from the hardware itself. The features are listed as REG_DWORD value.
//

#define PCI_DEVICE_FEATURE_BITS L"e5b3b5ac-9725-4f78-963f-03dfb1d828c7"

//
// The following are the features a device can support. The way they are
// specified by the driver writers is by including the corresponding INF
// directives in machine.inf or pci.inf
//

#define PCI_FEATURE_IO_NOT_REQUIRED          L"IoNotRequired"
#define PCI_FEATURE_ASPM_OPT_IN              L"ASPMOptIn"
#define PCI_FEATURE_ASPM_OPT_OUT             L"ASPMOptOut"
#define PCI_FEATURE_ACS_NOT_REQUIRED         L"AcsNotRequired"
#define PCI_FEATURE_ACS_DO_NOT_USE           L"DoNotUseAcs"
#define PCI_FEATURE_D3_COLD_SUPPORTED        L"D3ColdSupported"
#define PCI_FEATURE_ENABLE_BRIDGE_INTERRUPTS L"EnableAllBridgeInterrupts"
#define PCI_FEATURE_ATOMICS_OPT_IN           L"AtomicsOptIn"
#define PCI_FEATURE_SETTLE_TIME_REQUIRED     L"SettleTimeRequired"
#define PCI_FEATURE_IGNORE_ERRORS_PLDR       L"IgnoreErrorsDuringPLDR"

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciScanBus(
    __inout PPCI_BUS Bus,
    __out PULONG DeviceCount
    );

NTSTATUS
PciScanBusCompletion (
    __inout PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciDestroyDevice(
    __inout PPCI_DEVICE Device
    );

VOID
PciSurpriseRemoveAndReenumerateSelf(
    __in PPCI_DEVICE Device
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciAttemptDeviceReset(
    _In_ PPCI_DEVICE Device
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciIsFunctionLevelResetSupported(
    _In_ PPCI_DEVICE Device
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
ULONG
PciQuerySupportedResetTypes(
    _In_ PPCI_DEVICE Device
    );

//
// ------------------------------------------------------------- expressbridge.c
//

NTSTATUS
ExpressProcessNewVirtualBridge (
    _In_ PPCI_BRIDGE Bridge,
    _In_ PPCI_BRIDGE_INTERFACE Interface,
    _Out_ PEXPRESS_BRIDGE *NewExpressBridge
    );

NTSTATUS
ExpressBridgeStart (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in PCM_RESOURCE_LIST RawList,
    __in PCM_RESOURCE_LIST TranslatedList
    );

NTSTATUS
ExpressBridgeStop (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

NTSTATUS
ExpressBridgePowerUp (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

NTSTATUS
ExpressBridgePowerDown (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressBridgeProcessMaxPayloadSize (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
ExpressBridgeDisableInterrupts (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressBridgeDisconnectInterrupt (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressDestroyVirtualBridge (
    __in __drv_in(__drv_freesMem(Pool)) PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressRootPortSingletonPmeEventWorker (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressRootPortAckPmeEvent (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

BOOLEAN
ExpressBridgeSetAriEnable (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in BOOLEAN OnOrOff
    );

//
// --------------------------------------------------------------- expressport.c
//

#define PCI_EXPRESS_INVALID_ENHANCED_CAP_ID             0xFFFF

//
// Vendor specific IDs.
//

#define PCI_VENDOR_INTEL 0x8086
#define PCI_EXPRESS_THUNDERBOLT_VSEC_ID 0x1234

typedef struct _EXPRESS_ROOT_PORT_CONTEXT {
    ULONG Signature;
    ULONG ReferenceCount;
    ULONG BusNumber;
    PCI_SLOT_NUMBER SlotNumber;
    PVOID PrivateInterface;
} EXPRESS_ROOT_PORT_CONTEXT, *PEXPRESS_ROOT_PORT_CONTEXT;

NTSTATUS
ExpressInitializeNewCompatibilityPort (
    __in PPCI_DEVICE Device
    );

NTSTATUS
ExpressProcessNewPort (
    __in PPCI_DEVICE Device
    );

VOID
ExpressPortInitializeAERSupport (
    __in PEXPRESS_PORT ExpressPort
    );

ULONG
ExpressCheckAcsHierarchyCompatibility(
   _In_ PEXPRESS_PORT ExpressPort
   );

VOID
ExpressRemoveIntegratedPortFromRootComplex (
    __inout PEXPRESS_PORT ExpressPort
    );

_IRQL_requires_max_(APC_LEVEL)
VOID
ExpressWaitForPendingTransactions (
    _In_ PEXPRESS_PORT ExpressPort
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
ExpressCheckForPendingTransactions (
    _In_ PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressSavePortRegisters (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressCompatibilitySavePortRegisters (
    __inout PEXPRESS_COMPATIBILITY_PORT CompatibilityPort
    );

NTSTATUS
ExpressRestorePortRegisters (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressCompatibilityRestorePortRegisters (
    __in PEXPRESS_COMPATIBILITY_PORT CompatibilityPort
    );

VOID
ExpressDestroyCompatibilityPort (
    __in __drv_in(__drv_freesMem(Pool)) PEXPRESS_COMPATIBILITY_PORT ExpressPort
    );

VOID
ExpressDestroyPort (
    __in __drv_in(__drv_freesMem(Pool)) PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressDetermineSlotNumber (
    __in PEXPRESS_PORT ExpressPort,
    __out PULONG SlotNumber
    );

#define ExpressNextAriFunctionNumber(Port)  ((Port)->NextAriFunctionNumber)

NTSTATUS
ExpressApplyHPXSettings(
    __in PEXPRESS_PORT ExpressPort,
    __in PPCI_HPX_INFO HPX
    );

//
// ---------------------------------------------------------------------- hack.c
//

extern const GUID GUID_EM_PCI_DEVICE_TYPE;
extern const GUID GUID_EM_PCI_DEVICE_HACK_RULE;
extern const GUID GUID_EM_PCI_D0_DELAY_RULE;
extern const GUID GUID_EM_PCI_DEVICE_SET_MPSSIZE_RULE;
extern const GUID GUID_EM_PCI_DEVICE_MATCH_CALLBACK;
extern const GUID GUID_EM_PCI_DEVICE_HACK_BIOS_MATCH_RULE;
extern const GUID GUID_EM_PCI_DEVICE_HACK_CPU_MATCH_RULE;
extern const GUID GUID_EM_PCI_DEVICE_DMA_REMAPPING_ERROR_HANDLING_RULE;
extern const GUID GUID_EM_PCI_DEVICE_INTERRUPT_REMAPPING_NOT_ALL_REMAPPED_RULE;
extern const GUID GUID_EM_PCI_DEVICE_SET_HACKFLAGS_CALLBACK;
extern const GUID GUID_EM_PCI_DEVICE_SET_D0DELAY_CALLBACK;
extern const GUID GUID_EM_PCI_DEVICE_SET_MPSSIZE_CALLBACK;
extern const GUID GUID_EM_PCI_DEVICE_QUERY_RULE_CALLBACK;
extern const GUID GUID_EM_PCI_DEVICE_DISABLE_PCI_EXPRESS_ASPM_RULE;

//
// Hack flags
//

#define PCI_HACK_FAIL_QUERY_STOP            0x0000000000000001L
#define PCI_HACK_NO_DISABLE_FOR_STOP_REMOVE 0x0000000000000002L
#define PCI_HACK_LOCK_RESOURCES             0x0000000000000004L
#define PCI_HACK_NO_ENUM_AT_ALL             0x0000000000000008L
#define PCI_HACK_ENUM_NO_RESOURCE           0x0000000000000010L
#define PCI_HACK_AVOID_D1D2_FOR_SLD         0x0000000000000020L
#define PCI_HACK_NEVER_DISCONNECT           0x0000000000000040L
#define PCI_HACK_DONT_DISABLE               0x0000000000000080L
#define PCI_HACK_MULTIFUNCTION              0x0000000000000100L
#define PCI_HACK_16BIT_CB_IO_WINDOW         0x0000000000000200L
#define PCI_HACK_IGNORE_NON_STICKY_ISA      0x0000000000000400L
#define PCI_HACK_IGNORE_BOOT_CONFIG         0x0000000000000800L
#define PCI_HACK_DOUBLE_DECKER              0x0000000000001000L
#define PCI_HACK_ONE_CHILD                  0x0000000000002000L   // bus hack flag
#define PCI_HACK_PRESERVE_COMMAND           0x0000000000004000L
#define PCI_HACK_DEFAULT_CARDBUS_WINDOWS    0x0000000000008000L
#define PCI_HACK_CB_SHARE_CMD_BITS          0x0000000000010000L
#define PCI_HACK_IGNORE_ROOT_TOPOLOGY       0x0000000000020000L
#define PCI_HACK_SUBTRACTIVE_DECODE         0x0000000000040000L
#define PCI_HACK_NO_EXPRESS_CAP             0x0000000000080000L
#define PCI_HACK_NO_ASPM_FOR_EXPRESS_LINK   0x0000000000100000L
#define PCI_HACK_CLEAR_INT_DISABLE_FOR_MSI  0x0000000000200000L
#define PCI_HACK_NO_SUBSYSTEM               0x0000000000400000L
#define PCI_HACK_COMMAND_REWRITE            0x0000000000800000L
#define PCI_HACK_AVOID_HARDWARE_ISA_BIT     0x0000000001000000L
#define PCI_HACK_BRIDGE_WINDOW_ALIGNMENT    0x0000000002000000L
#define PCI_HACK_NOT_MSI_HT_CONVERTER       0x0000000004000000L
#define PCI_HACK_SBR_ON_LINK_STATE_CHANGE   0x0000000008000000L
#define PCI_HACK_LINK_DISABLE_ON_SLOT_PWRDN 0x0000000010000000L
#define PCI_HACK_NO_PM_CAPS                 0x0000000020000000L
#define PCI_HACK_DISABLE_POWER_DOWN         0x0000000040000000L
#define PCI_HACK_NO_SUBSYSTEM_AFTER_D3      0x0000000080000000L
#define PCI_HACK_VIDEO_LEGACY_DECODE        0x0000000100000000L
#define PCI_HACK_FAKE_CLASS_CODE            0x0000000200000000L
#define PCI_HACK_RESET_BRIDGE_ON_POWERUP    0x0000000400000000L
#define PCI_HACK_BAD_NATIVE_IDE             0x0000000800000000L
#define PCI_HACK_FAIL_QUERY_REMOVE          0x0000001000000000L
#define PCI_HACK_CRITICAL_DEVICE            0x0000002000000000L
#define PCI_HACK_OVERRIDE_CRITICAL_DEVICE   0x0000004000000000L
#define PCI_HACK_EXPRESS_VERSION_IS_2       0x0000008000000000L

//
// Formerly - PCI_HACK_PRESERVE_BRIDGE_WINDOWS.
// This is no longer relevant with multi-level rebalance support.
// This flag should not be resused.
//

#define PCI_HACK_DEPRECATED_39                            0x0000008000000000L
#define PCI_HACK_NO_REVISION_AFTER_D3                     0x0000010000000000L
#define PCI_HACK_PROGRAM_HT_MSI_MAP                       0x0000020000000000L
#define PCI_HACK_DISABLE_CHILD_PM                         0x0000040000000000L
#define PCI_HACK_DOWNSTREAM_DEVICE_NOT_REMOVABLE          0x0000080000000000L
#define PCI_HACK_IGNORE_AER_CAPABILITY                    0x0000100000000000L
#define PCI_HACK_FORCE_RESOURCE_VF_BARS                   0x0000200000000000L
#define PCI_HACK_NOT_WAKEABLE                             0x0000400000000000L
#define PCI_HACK_MSIX_TABLE_OFF_BY_ONE                    0x0000800000000000L
#define PCI_HACK_ACS_CAPABILITY_INCORRECT_FORMAT          0x0001000000000000L

//
// System wide hack flags - These are stored in PciSystemWideHackFlags
//

#define PCI_SYS_HACK_DISABLE_REBALANCE                              0x00000001

//
// A system wide hack to ignore the bios configued root bus range is
// put in only when the driver can expand the pci root bus number range
// to accomodate all the underlying BIOS configued pci bridges without
// any overlap with other pci root buses. This is only for root buses in
// segment 0 for legacy systems.
//
#define PCI_SYS_HACK_IGNORE_ROOTBUS_RANGE                           0x00000002
#define PCI_SYS_HACK_DISABLE_HOTPLUG                                0x00000004

//
// There are certain BIOSes that configure PCI root buses with overlapping bus
// number ranges. This is an undesired behavior. The root arbiters treat bus
// numbers as a shared resource and hence allow bus number conflicts. This
// might have caused such BIOS bugs to go unnoticed. Now, with introduction of
// segments, we have a simple check that fails AddDevice call in such cases.
// This hack is to ignore the overlapping root bus number ranges and not return
// failure from AddDevice due to such failure. In the long term, root arbiters
// should treat all resources as exclusive.
//
#define PCI_SYS_HACK_IGNORE_OVERLAPPING_ROOTBUS_RANGES              0x00000008

//
// This is defined to provide a workaround for a BIOS bug whereby a memory
// window on a cardbus bridge is programmed with limits less than the base,
// prompting us to completely ignore this requirement. With this flag, we
// revert to the XP/SRV03 behavior on select machines.
//
#define PCI_SYS_HACK_IGNORE_CB_MEMORY_WINDOW_LIMITS                 0x00000010

//
// This is defined for a single test system to not assert when bad
// Id data fields are read for a PCI host bridge during Start.
//
#define PCI_SYS_HACK_AVOID_ASSERT_ON_BAD_IDDATA_FOR_HOST_BRIDGE     0x00000020

//
// This is defined to allow disabling ASPM on PCI Express links for those
// systems that are known to cause NMI.
//
#define PCI_SYS_HACK_DISABLE_EXPRESS_ASPM                           0x00000040

//
// This is defined to force PCI driver to assume control for all Express
// features irrespective of the results of an _OSC method evaluation.
//
#define PCI_SYS_HACK_CONTROL_EXPRESS_FEATURES                       0x00000080

//
// This is defined to allow bridges to be configured without setting the ISA
// Enable bit, even when they are peers of bridges with the VGA Enable bit set.
// This can be set on systems where the VGA path does not claim 10 bit VGA
// aliases with a higher priority than the bridge window ranges of other
// bridges.
//
#define PCI_SYS_HACK_IGNORE_ISA_VGA_CONFLICT                        0x00000100

//
// Ignore any bus number configuration for express bridges so that the PCI
// driver can provide a bigger aperture of bus numbers. Do not do this for
// a video bridge since we don't expect deep hierarchies below them.
//
#define PCI_SYS_HACK_IGNORE_BUS_NUMBERS_FOR_EXPRESS_BRIDGES         0x00000200

//
// Ignore any disabled bridge windows for express hierarchies.
//
#define PCI_SYS_HACK_IGNORE_DISABLED_EXPRESS_BRIDGE_WINDOW          0x00000400

//
// Force early resource restore on resume from S3/S4.
//
#define PCI_SYS_HACK_FORCE_EARLY_PCI_RESTORE                        0x00000800

//
// AVAILABLE                                                        0x00001000
//

//
// Force all devices and buses to support D3cold.
//
#define PCI_SYS_HACK_EVERYBODY_SUPPORTS_D3COLD                      0x00002000

//
// Force all devices and buses to use D3hot.
//
#define PCI_SYS_HACK_NOBODY_SUPPORTS_D3COLD                         0x00004000

//
// This OS is running in a mode where SR-IOV can't occur, either because the
// machine doesn't support it or because this isn't a Hyper-V parent partition.
//
#define PCI_SYS_HACK_NO_SRIOV                                       0x00008000

//
// Force the use of legacy device ID generation without the use of PCIe serial
// number support
//
#define PCI_SYS_HACK_DISABLE_EXPRESS_SERIAL_NUMBER_SUPPORT          0x00010000

//
// Disable serial number caching support on Express enabled machines that
// are using serial numbers.
//
#define PCI_SYS_HACK_DISABLE_EXPRESS_SERIAL_NUMBER_CACHING          0x00020000

//
// Bus hack flags
// NB: These are not currently applied to cardbus bridges
//
// PCI_BUS_HACK_LOCK_RESOURCES - prevent devices on *this* bus from
// being moved.  If a BAR is unconfigured it will still be assigned
// resources from what is available on the bus.  If the BAR is
// configured only those resources if available will be assigned, if
// not available the the device will fail CM_PROBLEM_RESOURCE_CONFLICT.
//
// Putting /PCILOCK in boot.ini applies this to all devices in the system.
//
#define PCI_BUS_HACK_LOCK_RESOURCES                       0x00000001

//
// Macros to identify individual devices that need to be hacked aorund.
//
#define PCI_S3_868(_Device)                                                 \
    (((_Device)->VendorID == 0x5333) &&                                     \
     (((_Device)->DeviceID == 0x88f0) || ((_Device)->DeviceID == 0x8880)))

#define PCI_CIRRUS_54XX(_Device)           \
    (((_Device)->VendorID == 0x1013) &&    \
     ((_Device)->DeviceID == 0x00a0))

#define PCI_VGA_CONTROLLER(_Device)                             \
    ((((_Device)->BaseClass == PCI_CLASS_PRE_20) &&             \
      ((_Device)->SubClass  == PCI_SUBCLASS_PRE_20_VGA)) ||     \
     (((_Device)->BaseClass == PCI_CLASS_DISPLAY_CTLR) &&       \
      ((_Device)->SubClass  == PCI_SUBCLASS_VID_VGA_CTLR)))

#define PCI_HACK_FLAG_SUBSYSTEM 0x01
#define PCI_HACK_FLAG_REVISION  0x02

typedef struct _PCI_HACK_TABLE_ENTRY {
    USHORT VendorID;
    USHORT DeviceID;
    USHORT SubVendorID;
    USHORT SubSystemID;
    ULONGLONG HackFlags;
    UCHAR   RevisionID;
    UCHAR   Flags;
} PCI_HACK_TABLE_ENTRY, *PPCI_HACK_TABLE_ENTRY;

DWORD
PciApplyArbiterInitializationHack (
    VOID
    );

NTSTATUS
PciBuildHackTable(
    __in HANDLE ServiceHandle,
    __out PPCI_HACK_TABLE_ENTRY *HackTable
    );

NTSTATUS
PciRegisterWithEM (
    __inout PDRIVER_OBJECT DriverObject
    );

VOID
PciDeregisterWithEM (
    VOID
    );

NTSTATUS
PciGetHackFlagsFromEM (
    __inout PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
PciGetHackFlagsFromRegistry (
    __in PPCI_DEVICE Device,
    __out PULONGLONG HackFlags
    );

NTSTATUS
PciGetBusHackFlags(
    __in PPCI_BUS Bus,
    __out PULONG HackFlags
    );

VOID
PciMungeClassCodes(
    __in PPCI_DEVICE Device
    );

VOID
PciMungeHackFlags(
    __inout PPCI_DEVICE Device
    );

VOID
PciApplyInitialHacks(
    __inout PPCI_DEVICE Device
    );

//
// ----------------------------------------------------------------- hotplug.c
//

NTSTATUS
PciCreateHotPlugController (
    __in PPCI_BRIDGE PciBridge,
    __in ULONG SlotCount,
    __out PPCI_HOTPLUG_CONTROLLER *HotPlugController
    );

NTSTATUS
PciStartHotPlugController (
    __inout PPCI_HOTPLUG_CONTROLLER Controller
    );

VOID
PciInitializeHotPlugEventMap (
    __inout PPCI_HOTPLUG_SLOT Slot
    );

NTSTATUS
PciEnableDisableHotPlugInterrupts (
    __inout PPCI_HOTPLUG_CONTROLLER Controller,
    __in BOOLEAN EnableInterrupts
    );

VOID
PciProcessHotPlugInterrupts (
    __inout PPCI_HOTPLUG_CONTROLLER Controller,
    __in PPCI_HOTPLUG_INTERRUPTS HotPlugInterrupts
    );

NTSTATUS
PciHandleHotPlugDriverRemoval (
    __inout PPCI_HOTPLUG_SLOT Slot
    );

VOID
PciDestroyHotPlugController (
    __inout PPCI_HOTPLUG_CONTROLLER *HotPlugController
    );

//
// ------------------------------------------------------------------------ id.c
//

NTSTATUS
PciQueryDeviceID(
    __in PPCI_DEVICE Device,
    __out_bcount(*BufferLength) PWCHAR Buffer,
    __inout PULONG BufferLength
    );

NTSTATUS
PciQueryInstanceID(
    __in PPCI_DEVICE Device,
    __out_bcount(*BufferLength) PWCHAR Buffer,
    __inout PULONG BufferLength
    );

NTSTATUS
PciQueryHardwareIDs(
    __in PPCI_DEVICE Device,
    __out_bcount(*BufferLength) PWCHAR Buffer,
    __inout PULONG BufferLength
    );

NTSTATUS
PciQueryCompatibleIDs(
    __in PPCI_DEVICE Device,
    __out_bcount(*BufferLength) PWCHAR Buffer,
    __inout PULONG BufferLength
    );

NTSTATUS
PciQueryDeviceText(
    __in PPCI_DEVICE Device,
    __in DEVICE_TEXT_TYPE TextType,
    __in LCID LocaleId,
    __deref_out PWSTR *DeviceText
    );

//
// ----------------------------------------------------------------------- ide.c
//

//
// Programming Interface encodings for PCI IDE Controllers
// BaseClass = 1, SubClass = 1
//
#define PCI_IDE_PRIMARY_NATIVE_MODE         0x01
#define PCI_IDE_PRIMARY_MODE_CHANGEABLE     0x02
#define PCI_IDE_SECONDARY_NATIVE_MODE       0x04
#define PCI_IDE_SECONDARY_MODE_CHANGEABLE   0x08

#define PCI_LEGACY_IDE_CONTROLLER(_Device)                                  \
        ((_Device)->BaseClass == PCI_CLASS_MASS_STORAGE_CTLR                \
        && (_Device)->SubClass == PCI_SUBCLASS_MSC_IDE_CTLR                 \
        && !BITS_SET((_Device)->ProgIf, (PCI_IDE_PRIMARY_NATIVE_MODE        \
                                        | PCI_IDE_SECONDARY_NATIVE_MODE)))

#define PCI_NATIVE_IDE_CONTROLLER(_Device)                                  \
        ((_Device)->BaseClass == PCI_CLASS_MASS_STORAGE_CTLR                \
        && (_Device)->SubClass == PCI_SUBCLASS_MSC_IDE_CTLR                 \
        && BITS_SET((_Device)->ProgIf, (PCI_IDE_PRIMARY_NATIVE_MODE         \
                                        | PCI_IDE_SECONDARY_NATIVE_MODE)))

#define PCI_NATIVE_CAPABLE_IDE_CONTROLLER(_Device)                          \
        ((_Device)->BaseClass == PCI_CLASS_MASS_STORAGE_CTLR                \
        && (_Device)->SubClass == PCI_SUBCLASS_MSC_IDE_CTLR                 \
        && BITS_SET((_Device)->ProgIf, (PCI_IDE_PRIMARY_MODE_CHANGEABLE     \
                                        | PCI_IDE_SECONDARY_MODE_CHANGEABLE)))

BOOLEAN
PciConfigureIdeController(
    __inout PPCI_DEVICE Device,
    __in BOOLEAN DisableIoDecode
    );

//
// ---------------------------------------------------------------------- init.c
//

//
// Support for kernel debugger and headless ports that can't be turned off
// This is retrieved from the registry in DriverEntry and thus the bus numbers
// are how the firmware configured the machine and not necessarily the current
// settings.  Luckily we saved away the BIOS config in the registry.
//
typedef struct _PCI_DEBUG_PORT {
    USHORT Segment;
    ULONG Bus;

    PCI_SLOT_NUMBER Slot;
} PCI_DEBUG_PORT, *PPCI_DEBUG_PORT;

//
// Support for early restoration of devices to accomodate debugging and
// hibernation.
//

#define PCI_EARLY_RESTORE_INVALID   0
#define PCI_EARLY_RESTORE_DEBUG     1
#define PCI_EARLY_RESTORE_HIBER     2
#define PCI_EARLY_RESTORE_CRASHDUMP 4

typedef struct _PCI_EARLY_RESTORE_LIST {
    LIST_ENTRY ListEntry;
    ULONG Bus;
    PCI_SLOT_NUMBER Slot;
    ULONG DeviceType;
} PCI_EARLY_RESTORE_LIST, *PPCI_EARLY_RESTORE_LIST;

extern LIST_ENTRY PciEarlyRestoreList;

NTSTATUS
PciEarlyRestoreAddListItem (
    ULONG Bus,
    PPCI_SLOT_NUMBER Slot,
    ULONG DeviceType
    );

//
// Support for device restoration to accomodate KSR.  These
// devices will be restored to the state they were before PCI
// configured them.
//

extern BOOLEAN PciKSREnabled;

typedef struct _PCI_LATE_RESTORE_NODE {
    LIST_ENTRY ListEntry;
    PPCI_BUS_INTERFACE_STANDARD PciBusInterface;
    ULONG BusNumber;
    PCI_SLOT_NUMBER Slot;
    ULONG Skip;
    ULONG PowerCap;
    PCI_PMCSR PMCSR;
    PCI_COMMON_HEADER Cfg;
} PCI_LATE_RESTORE_NODE, *PPCI_LATE_RESTORE_NODE;

VOID
PciLateRestoreInitialize (
    VOID
    );

NTSTATUS
PciLateRestoreAddNode (
    PPCI_BUS Bus,
    PCI_SLOT_NUMBER Slot
    );

VOID
PciLateRestoreResources(
    VOID
    );

VOID
PciLateRestoreTraceList (
    VOID
    );

extern PDRIVER_OBJECT PciDriverObject;
extern UNICODE_STRING PciRegistryPath;

extern BOOLEAN PciLockDeviceResources;
extern BOOLEAN PciSystemMsiEnabled;
extern ULONG PciSystemWideHackFlags;
extern ULONG PciWHEARecordPreallocationCount;
extern ULONG PciEnableNativeModeATA;
extern ULONG PciEnableLegacyPcieRuntimePme;

extern KEVENT PciGlobalLock;
extern KEVENT PciBusLock;

extern PDEVICE_OBJECT PciControlDevice;
extern BOOLEAN PciSystemPowerStateTransitionActive;

_Acquires_lock_(_Global_critical_region_)
_Acquires_nonreentrant_lock_((PVOID)Lock)
__forceinline
void
PciAcquirePassiveLock(
    __in PKEVENT Lock
    )
{
    KeEnterCriticalRegion();
    KeWaitForSingleObject(Lock,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);
}

_Releases_nonreentrant_lock_((PVOID)Lock)
_Releases_lock_(_Global_critical_region_)
__forceinline
void
PciReleasePassiveLock(
    __in PKEVENT Lock
    )
{
    KeSetEvent(Lock, IO_NO_INCREMENT, FALSE);
    KeLeaveCriticalRegion();
}

_Acquires_lock_((PVOID)Lock)
__drv_mustHoldCriticalRegion
__forceinline
void
PciAcquirePassiveLockInCriticalSection(
    __in PKEVENT Lock
    )
{
    KeWaitForSingleObject(Lock,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);
}

_Releases_lock_((PVOID)Lock)
__drv_mustHoldCriticalRegion
__forceinline
void
PciReleasePassiveLockInCriticalSection(
    __in PKEVENT Lock
    )
{
    KeSetEvent(Lock, IO_NO_INCREMENT, FALSE);
}

//
// System processor information.
//

ULONG PciNumaNodeCount;
ULONG PciGroupZeroNumaNodeCount;;
KEVENT PciNumaInformationLock;
ULONG PciProcessorCount;

//
// This array holds the group affinity mask for all NUMA node. It assumes
// that each NUMA node is fully contained in a group, thus a group affinity
// mask is sufficient to describe the affinities of all processors in the
// NUMA node. All nodes in group 0 must appear before any nodes in higher
// groups in the array.
//

PGROUP_AFFINITY PciNumaNodesProcessorMask;

//
// This array holds group relative number of the next processor in each
// node to be affinitized for a new message. Note that only one processor
// is affinitized for a single message.
//

PUCHAR PciRoundRobinCurrentProcessor;

NTSTATUS
PciGetNumaInformation (
    VOID
    );

VOID
PciFreeNumaInformation (
    VOID
    );

extern BOOLEAN PciInterruptSteeringEnabled;

//
// ----------------------------------------------------------------- interface.c
//

//
// Definition of PCI_INTERFACE structure
//

// Bit definitions for Flags field.
#define PCIIF_PDO  0x01  // Interface can be used by a PDO
#define PCIIF_FDO  0x02  // Interface can be used by an FDO
#define PCIIF_ROOT 0x04  // Interface can be used only at by the root.

typedef enum {

    PciInterface_Arbiter,
    PciInterface_BusHandler,
    PciInterface_AgpTarget,
    PciInterface_IntRouteHandler,
    PciInterface_PciCb,
    PciInterface_LegacyDeviceDetection,
    PciInterface_PmeHandler,
    PciInterface_DevicePresent,
    PciInterface_NativeIde,
    PciInterface_Location,
    PciInterface_Translator,
    PciInterface_Bridge,
    PciInterface_ExpressLinkQuiescent,
    PciInterface_ExpressRootPortAccess,
    PciInterface_MsiXTableConfig,
    PciInterface_Virtualization,
    PciInterface_D3ColdSupport,
    PciInterface_CrashdumpFunctions,
    PciInterface_ReenumerateSelf,
    PciInterface_IommuBusInterface,
    PciInterface_DeviceResetStandard,
    PciInterface_SecurityInterface
} PCI_INTERFACE_SIGNATURE;

typedef
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PCI_INTERFACE_CONSTRUCTOR (
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    );

typedef PCI_INTERFACE_CONSTRUCTOR *PPCI_INTERFACE_CONSTRUCTOR;

typedef struct _PCI_INTERFACE {
    LPCGUID InterfaceType;
    USHORT MinSize;
    USHORT MinVersion;
    USHORT MaxVersion;
    USHORT Flags;
    PCI_INTERFACE_SIGNATURE Signature;
    PPCI_INTERFACE_CONSTRUCTOR Constructor;
} PCI_INTERFACE, *PPCI_INTERFACE;

typedef const PCI_INTERFACE *PCPCI_INTERFACE;

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciQueryInterface(
    __in PVOID DeviceExtension,
    __in LPCGUID InterfaceType,
    __in USHORT Size,
    __in USHORT Version,
    __in PVOID InterfaceSpecificData,
    __out_bcount(Size) PINTERFACE InterfaceReturn,
    __in BOOLEAN LastChance,
    __in NTSTATUS CurrentIrpStatus
    );

VOID
PciRefDereferenceNoop(
    __in PVOID Context
    );

//
// ----------------------------------------------------------------- interrupt.c
//

typedef struct _INTERRUPT_CONNECTION_DATA *PINTERRUPT_CONNECTION_DATA;

typedef struct _LEGACY_DEVICE {
    SINGLE_LIST_ENTRY List;
    PDEVICE_OBJECT LegacyDeviceObject;
    ULONG          Bus;
    ULONG          PciSlot;
    UCHAR          InterruptLine;
    UCHAR          InterruptPin;
    UCHAR          ClassCode;
    UCHAR          SubClassCode;
    PDEVICE_OBJECT ParentPdo;
    ROUTING_TOKEN  RoutingToken;
    PPCI_DEVICE    DeviceExtension;
} LEGACY_DEVICE, *PLEGACY_DEVICE;

NTSTATUS
PciGetInterruptRequirement(
    _In_ PPCI_DEVICE Device,
    _Inout_ PPCI_DEVICE_INTERRUPT_REQUIREMENT Requirement
    );

NTSTATUS
PciProcessInterruptDescriptor(
    __inout PPCI_DEVICE Device,
    __in PINTERRUPT_CONNECTION_DATA ConnectionData,
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor,
    __in ULONG DescriptorNumber,
    __inout PPCI_DEVICE_INTERRUPT_RESOURCE InterruptResource
    );

NTSTATUS
PciProgramInterruptResource(
    __in PPCI_DEVICE Device
    );

NTSTATUS
PciProgramHtMsiMapCapability (
    __in PPCI_DEVICE Device
    );

VOID
PciDisableInterrupts(
    __in PPCI_DEVICE Device
    );

UCHAR
PciGetHalReportedInterruptLine(
    __in PPCI_DEVICE Device
    );

VOID
PciMaskMsiXInterrupt(
    __in PPCI_DEVICE Device
    );

VOID
PciMaskMsiInterrupt(
    __in PPCI_DEVICE Device
    );

NTSTATUS
PciGetMessageCount(
    __in PPCI_DEVICE Device,
    __out ULONG *MessageCount
    );

NTSTATUS
PciGetMessagePolicy (
    __in PPCI_DEVICE Device,
    __in ULONG MessageCount,
    __in ULONG MessageNumber,
    __inout PUCHAR Iterator,
    __out PBOOLEAN DevicePolicyExists,
    __out PIRQ_DEVICE_POLICY AffinityPolicy,
    __out PIRQ_PRIORITY PriorityPolicy,
    __out PUSHORT Group,
    __out PKAFFINITY TargetProcessors
    );

NTSTATUS
PciCacheLegacyDeviceRouting(
    __in PDEVICE_OBJECT LegacyDO,
    __in ULONG Bus,
    __in ULONG PciSlot,
    __in UCHAR InterruptLine,
    __in UCHAR InterruptPin,
    __in UCHAR ClassCode,
    __in UCHAR SubClassCode,
    __in PDEVICE_OBJECT ParentPdo,
    __in PPCI_DEVICE DeviceExtension,
    __out_opt PDEVICE_OBJECT *OldLegacyDO
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciDeleteVolatileInterruptRoutingData (
    _In_ PPCI_DEVICE Device
    );

//
// -------------------------------------------------------------------- isrdpc.c
//

//
// Represents the interrupts.
//

#define INTERRUPT_SOURCE_EXPRESS_PME             0x00000001
#define INTERRUPT_SOURCE_EXPRESS_HOTPLUG         0x00000002
#define INTERRUPT_SOURCE_EXPRESS_AER             0x00000004

//
// Represents work items delegated by an ISR for a DPC and worker routine.
//

#define EXPRESS_PME_WAKE_EVENT_HANDLING          0x00000001
#define EXPRESS_HOT_PLUG_EVENT_HANDLING          0x00000002
#define EXPRESS_AER_EVENT_HANDLING               0x00000004

NTSTATUS
ExpressRootPortConnectInterrupt (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in PCM_RESOURCE_LIST RawList,
    __in PCM_RESOURCE_LIST TranslatedList
    );

__drv_functionClass(IO_DPC_ROUTINE)
_IRQL_always_function_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
IO_DPC_ROUTINE ExpressRootPortInterruptDpc;

_IRQL_requires_same_
__drv_functionClass(KSTART_ROUTINE)
KSTART_ROUTINE RootPmeEventDispatcher;

BOOLEAN
ExpressRootPortCheckForSpuriousPmeEvent (
    __in PEXPRESS_BRIDGE ExpressBridge
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
ExpressRootPortAckPmeEventAtDeviceSource (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressRootPortEnableInterrupts (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressRootPortDisableInterrupts (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressRootPortDisconnectInterrupt (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

NTSTATUS
ExpressDownstreamSwitchPortConnectInterrupt (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in PCM_RESOURCE_LIST RawList,
    __in PCM_RESOURCE_LIST TranslatedList
    );

__drv_functionClass(IO_DPC_ROUTINE)
_IRQL_always_function_min_(DISPATCH_LEVEL)
_IRQL_requires_(DISPATCH_LEVEL)
_IRQL_requires_same_
IO_DPC_ROUTINE ExpressDownstreamSwitchPortInterruptDpc;

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
ExpressDownstreamSwitchPortEnableInterrupts (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressDownstreamSwitchPortDisableInterrupts (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressDownstreamSwitchPortDisconnectInterrupt (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressBridgeHotPlugInterruptRoutine (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __inout PPCI_HOTPLUG_CONTROLLER HotPlugController,
    __deref_volatile __inout PLONG DelegatedWorkItems,
    __out PBOOLEAN AssertInterrupt,
    __out PBOOLEAN QueueDpc
    );

//
// -------------------------------------------------------------------- legacy.c
//

VOID
PciHookHal(
    VOID
    );

VOID
PciUnhookHal(
    VOID
    );

PPCI_DEVICE
PciFindDeviceByLocation(
    __in ULONG BusNumber,
    __in PCI_SLOT_NUMBER Slot,
    __in BOOLEAN NoLocking
    );

//
// ------------------------------------------------------------------- pciprop.c
//

NTSTATUS
PciRootSetCustomProperties(
    __in PPCI_ROOT PciRoot
    );

NTSTATUS
PciDeviceSetCustomProperties (
    __in PPCI_DEVICE Device
    );

//
// ------------------------------------------------------------------ resource.c
//

#define PCI_CM_RESOURCE_VERSION     1
#define PCI_CM_RESOURCE_REVISION    1

#define ALIGN_RANGE_DOWN(range, alignment) \
    ((range) & ~((alignment) - 1))

#define ALIGN_RANGE_UP(range, alignment) \
    (ALIGN_RANGE_DOWN((range) + ((alignment) -1), alignment))

#define FOR_ALL_IN_PARALLEL_ARRAYS(_Array1, _Array2, _Count, _Descriptor1, _Descriptor2) \
            for ( (_Descriptor1) = (_Array1), (_Descriptor2) = (_Array2);                \
                  (_Descriptor1) < (_Array1) + (_Count);                                 \
                  (_Descriptor1)++, (_Descriptor2)++ )

//
// FOR_ALL_IN_RESOURCE_LIST(
//      IN PCM_RESOURCE_LIST _ResList,
//      OUT PCM_PARTIAL_RESOURCE_DESCRIPTOR _Descriptor
//      )
//
//  Iterates over the resource descriptors in a CM_RESOURCE_LIST of Count 1
//
#define FOR_ALL_IN_RESOURCE_LIST(_ResList, _Descriptor)             \
    PCI_ASSERT((_ResList)->Count == 1);                             \
    FOR_ALL_IN_ARRAY(                                               \
        (_ResList)->List[0].PartialResourceList.PartialDescriptors, \
        (_ResList)->List[0].PartialResourceList.Count,              \
        (_Descriptor)                                               \
        )

//
// FOR_ALL_IN_PARALLEL_RESOURCE_LISTS(
//      IN PCM_RESOURCE_LIST _ResList1,
//      IN PCM_RESOURCE_LIST _ResList2
//      OUT PCM_PARTIAL_RESOURCE_DESCRIPTOR _Descriptor1
//      OUT PCM_PARTIAL_RESOURCE_DESCRIPTOR _Descriptor2
//      )
//
//  Iterates over the resource descriptors in two CM_RESOURCE_LISTs of Count 1
//
#define FOR_ALL_IN_PARALLEL_RESOURCE_LISTS(_ResList1, _ResList2, _Descriptor1, _Descriptor2)    \
    PCI_ASSERT(((_ResList1)->Count == 1) && ((_ResList2)->Count == 1));                         \
    PCI_ASSERT((_ResList1)->List[0].PartialResourceList.Count                                   \
               <= (_ResList2)->List[0].PartialResourceList.Count);                              \
    FOR_ALL_IN_PARALLEL_ARRAYS(                                                                 \
        (_ResList1)->List[0].PartialResourceList.PartialDescriptors,                            \
        (_ResList2)->List[0].PartialResourceList.PartialDescriptors,                            \
        (_ResList1)->List[0].PartialResourceList.Count,                                         \
        (_Descriptor1),                                                                         \
        (_Descriptor2)                                                                          \
        )

extern const GUID PCI_DSM_UUID;

NTSTATUS
PciBuildRequirementsList(
    __in PPCI_DEVICE Device,
    __out PIO_RESOURCE_REQUIREMENTS_LIST *RequirementsList
    );

NTSTATUS
PciBuildCurrentResourceList(
    __in PPCI_DEVICE Device,
    __out PCM_RESOURCE_LIST *ResourceList
    );

VOID
PciMarkHiberPhase (
    VOID
    );

BOOLEAN
PciMultiStageResumeCapable(
    VOID
    );

NTSTATUS
PciProcessStartResources(
    __in PPCI_DEVICE Device,
    __in PCM_RESOURCE_LIST StartResourceList,
    __in PCM_RESOURCE_LIST TranslatedResourceList,
    __out PPCI_DEVICE_RESOURCES Resources,
    __out PPCI_DEVICE_INTERRUPT_RESOURCE InterruptResource
    );

BOOLEAN
PciIsVgaResource(
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Resource
    );

NTSTATUS
PciResourceFromBar(
    __in ULONG Bar,
    __in ULONG UpperBar,
    __in PIO_RESOURCE_DESCRIPTOR Requirement,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR Resource
    );

NTSTATUS
PciResourceFromRomBar(
    __in ULONG RomBar,
    __in PIO_RESOURCE_DESCRIPTOR Requirement,
    __out PCM_PARTIAL_RESOURCE_DESCRIPTOR Resource
    );

NTSTATUS
PciRequirementFromBar(
    _In_ ULONG ProbedBar,
    _In_ ULONG UpperBar,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Requirement,
    _Out_opt_ PBOOLEAN Bar64Bit
    );

NTSTATUS
PciRequirementFromRomBar(
    _In_ ULONG ProbedRomBar,
    _In_ ULONG CurrentRomBar,
    _Inout_ PIO_RESOURCE_DESCRIPTOR Requirement
    );

PIO_RESOURCE_REQUIREMENTS_LIST
PciAllocateIoRequirementsList(
    __in ULONG BusNumber,
    __in ULONG SlotNumber,
    __in ULONG ListCount,
    __in ULONG ResourceCount
    );

PCM_RESOURCE_LIST
PciAllocateCmResourceList(
    __in PPCI_DEVICE Device,
    __in ULONG ResourceCount
    );

BOOLEAN
PciIsFixedRequirement(
    __in PIO_RESOURCE_DESCRIPTOR Requirement
    );

//
// ----------------------------------------------------------------------- rom.c
//

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciReadRomImage(
    _In_ PPCI_DEVICE Device,
    _In_ ULONG WhichSpace,
    _Inout_updates_(*Length) PVOID Buffer,
    _In_ ULONG Offset,
    _Inout_ PULONG Length
    );

//
// ---------------------------------------------------------------------- root.c
//

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciProcessNewRootBus(
    _Inout_ PPCI_BUS Bus
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciDestroyRootBus(
    _Inout_ PPCI_BUS Bus
    );

NTSTATUS
PciGetRootBusNumbersFromBootConfig(
    __in PPCI_ROOT Root,
    __out PULONG SecondaryBusNumber,
    __out PULONG SubordinateBusNumber
    );

NTSTATUS
PciGetSegmentForRootBus (
    __in PPCI_ROOT Root
    );

NTSTATUS
PciRootFilterRequirementsList (
    __in PPCI_BUS Bus,
    __inout PIO_RESOURCE_REQUIREMENTS_LIST Requirements
    );

NTSTATUS
PciRootFilterStartResources (
    __in PPCI_BUS Bus,
    __inout PCM_RESOURCE_LIST RawResources,
    __inout PCM_RESOURCE_LIST TranslatedResources
    );

NTSTATUS
ExpressProcessNewRootComplex (
    __inout PPCI_BUS PciBus
    );

VOID
ExpressDestroyRootComplex (
    __in __drv_in(__drv_freesMem(Pool)) PROOT_COMPLEX RootComplex
    );

//
// --------------------------------------------------------------------- state.c
//

#define PCI_ACQUIRE_STATE_LOCK(Extension)                       \
   PciBeginStateTransition((PPCI_COMMON_EXTENSION)(Extension),  \
                           PciSynchronizedOperation)

#define PCI_RELEASE_STATE_LOCK(Extension)                       \
   PciCancelStateTransition((PPCI_COMMON_EXTENSION)(Extension), \
                            PciSynchronizedOperation)

VOID
PciInitializeState(
    __inout PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBeginStateTransition(
    __inout PPCI_COMMON_EXTENSION DeviceExtension,
    __in PCI_OBJECT_STATE      NewState
    );

VOID
PciCommitStateTransition(
    __inout PPCI_COMMON_EXTENSION DeviceExtension,
    __in PCI_OBJECT_STATE      NewState
    );

NTSTATUS
PciCancelStateTransition(
    __inout PPCI_COMMON_EXTENSION DeviceExtension,
    __in PCI_OBJECT_STATE      StateNotEntered
    );

BOOLEAN
PciIsInTransitionToState(
    __in PPCI_COMMON_EXTENSION DeviceExtension,
    __in PCI_OBJECT_STATE NextState
    );

//
// --------------------------------------------------------------------- tracelog.c
//

TRACELOGGING_DECLARE_PROVIDER(PciTraceLoggingProvider);
extern const GUID GUID_PCI_TRACELOG_PROVIDER;

NTSTATUS
PciTraceLogInitialize (
    VOID
    );

NTSTATUS
PciTraceLogUninitialize (
    VOID
    );

//
// --------------------------------------------------------------------- type0.c
//

extern PCI_HEADER_OPERATIONS PciType0HeaderOperations;

//
// --------------------------------------------------------------------- type1.c
//

extern PCI_HEADER_OPERATIONS PciType1HeaderOperations;

//
// --------------------------------------------------------------------- type2.c
//

extern PCI_HEADER_OPERATIONS PciType2HeaderOperations;

//
// --------------------------------------------------------------------- utils.c
//

typedef
NTSTATUS
(*PCRITICALROUTINE)(
    IN PVOID Extension,
    IN PVOID Context
    );

typedef struct _PCI_CRITICAL_ROUTINE_CONTEXT {

    volatile LONG Gate;
    volatile LONG Barrier;

    PCRITICALROUTINE Routine;
    PVOID Extension;
    PVOID Context;
    NTSTATUS Status;

} PCI_CRITICAL_ROUTINE_CONTEXT, *PPCI_CRITICAL_ROUTINE_CONTEXT;

#define DSM_METHOD_INPUT_ARGUMENTS_COUNT                4

typedef enum _PCI_FIRMWARE_DSM_FUNCTIONS {
    PCI_DSM_METHOD_QUERY_FUNCTION                   = 0,
    PCI_DSM_METHOD_BUS_CAPABILITIES                 = 4,
    PCI_DSM_METHOD_IGNORE_BOOT_CONFIG               = 5,
    PCI_DSM_METHOD_LATENCY_TOLERANCE                = 6,
    PCI_DSM_METHOD_DELAY_ON_SX_RESUME               = 8,
    PCI_DSM_METHOD_DELAY_OPTIMIZATION               = 9,
    PCI_DSM_METHOD_MAX
} PCI_FIRMWARE_DSM_FUNCTIONS, *PPCI_FIRMWARE_DSM_FUNCTIONS;

typedef enum _PCI_FIRMWARE_FUNCTION_REVISIONS {
    PCI_DSM_METHOD_REVISION_DEFAULT                 = 1,
    PCI_DSM_METHOD_BUS_CAPABILITIES_REV             = 2,
    PCI_DSM_METHOD_IGNORE_BOOT_CONFIG_REV           = 1,
    PCI_DSM_METHOD_LATENCY_TOLERANCE_REV            = 2,
    PCI_DSM_METHOD_DELAY_ON_SX_RESUME_REV           = 3,
    PCI_DSM_METHOD_DELAY_OPTIMIZATION_REV           = 3
} PCI_FIRMWARE_FUNCTION_REVISIONS;

NTSTATUS
PciSendPnpIrp(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIO_STACK_LOCATION Location,
    __out_opt PULONG_PTR Information
    );

NTSTATUS
PciSendIoctl(
    __in PDEVICE_OBJECT Device,
    __in ULONG IoctlCode,
    __in_bcount_opt(InputBufferLength) PVOID InputBuffer,
    __in ULONG InputBufferLength,
    __out_bcount_opt(OutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferLength
    );

VOID
NullResourceRequirement (
    _Inout_ PIO_RESOURCE_DESCRIPTOR Descriptor
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciGetDeviceProperty(
    _In_ PDEVICE_OBJECT PhysicalDeviceObject,
    _In_ DEVICE_REGISTRY_PROPERTY DeviceProperty,
    _Out_
    _At_(*PropertyBuffer, __drv_allocatesMem(Mem))
         PVOID *PropertyBuffer
    );

_Success_(return != FALSE)
BOOLEAN
PciOpenKey(
    _In_ PWSTR KeyName,
    _In_opt_ HANDLE ParentHandle,
    _In_ ACCESS_MASK Access,
    _Out_ PHANDLE Handle,
    _Out_opt_ PNTSTATUS Status
    );

NTSTATUS
PciGetRegistryValue(
    __in PWSTR ValueName,
    __in_opt PWSTR KeyName,
    __in_opt HANDLE ParentHandle,
    __in ULONG Type,
    __deref_out_bcount(*Length) PVOID *Buffer,
    __out PULONG Length
    );

NTSTATUS
PciDeviceCallCritical(
    __in PPCI_DEVICE Device,
    __in PCRITICALROUTINE CallbackRoutine,
    __inout_opt PVOID CallbackContext,
    __in BOOLEAN OnDebugPath
    );

_IRQL_requires_same_
__drv_functionClass(KIPI_BROADCAST_WORKER)
_IRQL_requires_(IPI_LEVEL)
KIPI_BROADCAST_WORKER PciExecuteCriticalSystemRoutine;

BOOLEAN
PciUnicodeStringStrStr(
    __in PUNICODE_STRING SearchString,
    __in PUNICODE_STRING SubString,
    __in BOOLEAN CaseInsensitive
    );

NTSTATUS
PciLocalDeviceUsage(
    __inout PPCI_DEVICE_USAGE DeviceUsage,
    __in DEVICE_USAGE_NOTIFICATION_TYPE NotificationType,
    __in BOOLEAN InPath
    );

typedef enum _PCI_FEATURE_STATE {
    PciFeatureStateInvalid,
    PciFeatureStateUndefined,
    PciFeatureStateEnabled,
    PciFeatureStateDisabled
} PCI_FEATURE_STATE, *PPCI_FEATURE_STATE;

PCI_FEATURE_STATE
PciIsDeviceFeatureEnabled (
    __in PPCI_DEVICE Device,
    __in PWSTR FeatureName
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciIsDsmFunctionSupported (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ ULONG FunctionIndex,
    _In_ ULONG FunctionRevision,
    _Out_ PBOOLEAN Supported
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciPrepareInputParametersForDsmMethod (
    _In_ PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer,
    _In_ ULONG ParametersBufferSize,
    _In_ ULONG FunctionIndex,
    _In_ ULONG FunctionRevision,
    _In_ LPCGUID Guid
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciSendIoctlForDsmEvaluation (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_reads_bytes_(InputBufferSize) PVOID InputBuffer,
    _In_ ULONG InputBufferSize,
    _Out_writes_bytes_(OutputBufferSize) PVOID OutputBuffer,
    _In_ ULONG OutputBufferSize
    );

NTSTATUS
PciLocationInterface_GetLocationStrings(
    __inout PVOID Context,
    __out PWCHAR *LocationStrings
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodToIgnoreBootConfig (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PBOOLEAN IgnoreBootConfig
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodForLtr (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PPCI_EXPRESS_LTR_MAX_LATENCY_REGISTER LtrValues
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodForObff (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PPCI_FIRMWARE_BUS_CAPS BusCaps
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodForDelayOnSxResume (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PBOOLEAN DelayOnSxResume
    );

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodForDelayOptimization (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PULONG Delays
    );

BOOLEAN
PciIsNonHotplugBridgeWithNoChildren(
    __in PPCI_DEVICE Device
    );

//
// ------------------------------------------------------------------- ctrldev.c
//

//
// Control device Extension
//
typedef struct _CONTROL_DEVICE_EXTENSION {

    ULONG Signature;
    KEVENT ProxyDeviceLock;
    ULONG ByteSize;
    _Field_z_
    _Field_size_bytes_(ByteSize)
    PWCHAR MultiSzProxyDevicePath;

} CONTROL_DEVICE_EXTENSION, *PCONTROL_DEVICE_EXTENSION;

__forceinline
PCONTROL_DEVICE_EXTENSION
PciGetControlDeviceExtension(
    VOID
    )
{
    PCI_ASSERT(PciControlDevice != NULL);
    return (PCONTROL_DEVICE_EXTENSION)PciControlDevice->DeviceExtension;
}

VOID
PciCleanupCtrlDevExt(
   _Inout_ PCONTROL_DEVICE_EXTENSION Extension
   );

NTSTATUS
PciCreateControlDevice(
   VOID
   );

NTSTATUS
PciControl_DeviceControl(
   _In_ PDEVICE_OBJECT  DeviceObject,
   _In_ PIRP            Irp
);

//
// ------------------------------------------------------------------- vmproxy.c
//

EXTERN_C DECLSPEC_SELECTANY const WCHAR s_VmProxyRegPath[] =
L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\PnP\\Pci";
EXTERN_C DECLSPEC_SELECTANY const WCHAR s_VmProxyValue[] = L"VmProxy";
EXTERN_C DECLSPEC_SELECTANY const WCHAR s_PciId[] = L"PCI";
EXTERN_C DECLSPEC_SELECTANY const WCHAR s_VmProxyId[] = L"PCIP";

NTSTATUS
VmProxyIsProxyDevice(
    _In_ const PPCI_DEVICE Device,
    _In_opt_ LPCWSTR ProxyDevicePaths,
    _Out_ BOOLEAN *ProxyDevice
    );

NTSTATUS
VmProxyCheckTearDown(
   _In_ PPCI_DEVICE Device,
   _In_opt_ LPCWSTR ProxyDevicePaths,
   _Out_ BOOLEAN *TearDownOldCreateNew
   );

NTSTATUS
VmProxyReadDevicePathsFromRegistry(
   _In_ PCONTROL_DEVICE_EXTENSION Extension
   );

NTSTATUS
VmProxyGetDeviceLocationString (
    _In_ const PPCI_DEVICE Device,
    _Outptr_result_z_ PWCHAR *DeviceLocationString
    );

NTSTATUS
VmProxyAddRemovePath(
   _In_ PCONTROL_DEVICE_EXTENSION Extension,
   _In_ const PIRP Irp,
   _In_ BOOLEAN AddPath
   );

NTSTATUS
VmProxyClearPath(
   _In_ PCONTROL_DEVICE_EXTENSION Extension
   );

//
// ------------------------------------------------------------------ verifier.c
//

//
// This is the list of PCI verifier failures.
//
typedef enum {

    PCI_VERIFIER_BRIDGE_REPROGRAMMED = 1,
    PCI_VERIFIER_PMCSR_TIMEOUT,
    PCI_VERIFIER_PROTECTED_CONFIGSPACE_ACCESS,
    PCI_VERIFIER_INVALID_WHICHSPACE

} PCI_VERIFIER_FAILURE, *PPCI_VERIFIER_FAILURE;

//
// This structure specifies table elements used when failing hardware, bioses,
// or drivers.
//
typedef struct {

    PCI_VERIFIER_FAILURE  VerifierFailure;
    VF_FAILURE_CLASS      FailureClass;
    ULONG                 Flags;
    PSTR                  FailureText;

} PCI_VERIFIER_DATA, *PPCI_VERIFIER_DATA;

VOID
PciVerifierInit(
    __in PDRIVER_OBJECT DriverObject
    );

VOID
PciVerifierUnload(
    VOID
    );

PPCI_VERIFIER_DATA
PciVerifierRetrieveFailureData(
    __in PCI_VERIFIER_FAILURE VerifierFailure
    );

//
// ---------------------------------------------------------------------- whea.c
//

VOID
PciWheaInit (
    VOID
    );

//
// --------------------------------------------------------------------- sriov.c
//

extern const PCI_EXPRESS_ACS_CONTROL PciAcsBitsForIov;

NTSTATUS
PciGetRequirementsForVfs(
    __inout PPCI_DEVICE Device,
    __inout PPCI_DEVICE_REQUIREMENTS Requirements
    );

NTSTATUS
PciGetResourcesForVfs(
    __in PPCI_DEVICE Device,
    __inout PPCI_DEVICE_RESOURCES Resources
    );

NTSTATUS
PciSetResourcesForVfs(
    __in PPCI_DEVICE Device,
    __in PPCI_DEVICE_RESOURCES Resources
    );

LOGICAL
PciIsSriovSupported(
    __in PPCI_DEVICE Device
    );

LOGICAL
PciIsSriovDriverAvailable(
    __in PPCI_DEVICE Device
    );

UCHAR
PciMaximumBusNumbersToCapture(
    __in PPCI_DEVICE Device
    );

VOID
PciEnableSriovDecodes(
    __in PPCI_DEVICE Device
    );

VOID
PciDisableSriovDecodes(
    __in PPCI_DEVICE Device
    );

NTSTATUS
PciVirtualizationCheckAcsSupport(
    __in PPCI_DEVICE Device
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciVirtualizationConfigureAcs(
    _In_ PPCI_DEVICE Device,
    _In_ PCI_EXPRESS_ACS_CONTROL BitsToModify,
    _In_ PCI_EXPRESS_ACS_CONTROL BitState
    );

//
// ----------------------------------------------------------------------- tph.c
//

NTSTATUS
ExpressSaveTphRequesterCapability (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressRestoreTphRequesterCapability (
    __inout PEXPRESS_PORT ExpressPort
    );

//
// ----------------------------------------------------------------------- ltr.c
//

NTSTATUS
ExpressSaveLtrCapability (
    _Inout_ PEXPRESS_PORT ExpressPort,
    _Inout_ PPCI_EXPRESS_LTR_CAPABILITY CapabilityTemp,
    _In_ BOOLEAN CommitTemp
    );

NTSTATUS
ExpressRestoreLtrCapability (
    _Inout_ PEXPRESS_PORT ExpressPort
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressInitializeLtrValue (
    _Inout_ PEXPRESS_PORT ExpressPort
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressInitializeObffValue (
    _Inout_ PEXPRESS_PORT ExpressPort
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressPortGetLtrValueForExpressBridge (
    _In_ PEXPRESS_PORT ExpressPort,
    _Out_ PPCI_EXPRESS_LTR_MAX_LATENCY_REGISTER UpstreamLatency
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressPortGetObffValueForExpressBridge (
    _In_ PEXPRESS_PORT ExpressPort,
    _Out_ PEXPRESS_PORT_OBFF_CONTROL Obff
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressBridgeCalculateLtrValues (
    _Inout_ PEXPRESS_BRIDGE ExpressBridge,
    _In_ ULONG UpstreamLatency
    );

_IRQL_requires_max_(HIGH_LEVEL)
VOID
ExpressPortObffEnable (
    _In_ PEXPRESS_PORT ExpressPort
    );

_IRQL_requires_max_(HIGH_LEVEL)
VOID
ExpressPortObffDisable (
    _In_ PEXPRESS_PORT ExpressPort
    );

//
// --------------------------------------------------------------- consolelock.c
//

typedef enum _PCI_CONSOLE_STATE {
    PciConsoleStatePreLogon = 0,
    PciConsoleStateUnlocked,
    PciConsoleStateLocked
} PCI_CONSOLE_STATE, *PPCI_CONSOLE_STATE;

extern BOOLEAN PciEnableConsoleLockHandling;

NTSTATUS
PciInitConsoleLock (
    );

NTSTATUS
PciDeinitConsoleLock (
    );

BOOLEAN
PciBusAffectedByConsoleLock(
    _In_ PPCI_BUS Bus
    );

BOOLEAN
PciBusShouldBeDisabledByConsoleLock(
    _In_ PPCI_BUS Bus
    );

VOID
PciSetBusDisabledByConsoleLock(
    _In_ PPCI_BUS Bus,
    _In_ BOOLEAN Disable
    );

//
// ------------------------------------------------------------- resizablebars.c
//

NTSTATUS
PciResizableBarInitialize (
    __inout PPCI_DEVICE Device,
    __in USHORT CapabilityOffset
    );

ULONG
PciGetBarRequirementCount (
    __in PPCI_DEVICE Device
    );

ULONG
PciPopulateBarRequirements (
    __in PPCI_DEVICE Device,
    __in ULONG Index,
    __in PPCI_DEVICE_RESOURCES PreferredResources,
    __inout PIO_RESOURCE_DESCRIPTOR Requirement
    );

NTSTATUS
PciResizableBarSetSize (
    __in PPCI_DEVICE Device,
    __in ULONG BarIndex,
    __in PIO_RESOURCE_DESCRIPTOR Requirement
    );

VOID
PciRequirementFromPreferredResource(
    __in PCM_PARTIAL_RESOURCE_DESCRIPTOR Resource,
    __out PIO_RESOURCE_DESCRIPTOR Requirement
    );

BOOLEAN
PciGenericRequirementsAllowed(
    __in PPCI_DEVICE Device,
    __in ULONG Index
    );

//
// ------------------------------------------------------------- iommu.c
//

#define SVM_CAP_ATS_MASK 0x1
#define SVM_CAP_PRI_MASK 0x2
#define SVM_CAP_PASID_MASK 0x4
#define SVM_CAP_MASK (SVM_CAP_ATS_MASK | SVM_CAP_PRI_MASK | SVM_CAP_PASID_MASK)

NTSTATUS
PciDeviceQueryPasid (
    __in PEXPRESS_PORT ExpressPort,
    __out PUCHAR MaxPasidWidth,
    __out PBOOLEAN ExecutePermissionSupported,
    __out PBOOLEAN PrivilegedModeSupported
    );

NTSTATUS
PciDeviceSetPasidControl (
    __in PEXPRESS_PORT ExpressPort,
    __in BOOLEAN PasidEnabled,
    __in BOOLEAN ExecutePermissionEnabled,
    __in BOOLEAN PrivilegedModeEnabled
    );

NTSTATUS
PciDeviceEnablePasid (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
PciDeviceDisablePasid (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
PciDeviceQueryAts (
    __in PEXPRESS_PORT ExpressPort,
    __out PUSHORT InvalidQueueDepth,
    __out PBOOLEAN PageAlignedRequest,
    __out PBOOLEAN GlobalInvalidateSupported
    );

NTSTATUS
PciDeviceSetAtsControl (
    __in PEXPRESS_PORT ExpressPort,
    __in USHORT SmallestTranslationUnit,
    __in BOOLEAN Enable
    );

NTSTATUS
PciDeviceEnableAts (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
PciDeviceDisableAts (
    __in PEXPRESS_PORT ExpressPort
    );

VOID
PciDeviceQueryPri (
    __in PEXPRESS_PORT ExpressPort,
    __out PBOOLEAN Enabled,
    __out PBOOLEAN ResponseFailure,
    __out PBOOLEAN UnexpectedPageRequestGroupIndex,
    __out PBOOLEAN Stopped,
    __out PBOOLEAN PrgResponsePasidRequired
    );

NTSTATUS
PciDeviceSetPriControl (
    __in PEXPRESS_PORT ExpressPort,
    __in BOOLEAN Enable,
    __in BOOLEAN Reset
    );

NTSTATUS
PciDeviceEnablePri (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
PciDeviceDisablePri (
    __in PEXPRESS_PORT ExpressPort
    );

VOID
PciSetOutstandingPageRequests (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
PciResetOutstandingPageRequests (
    __in PEXPRESS_PORT ExpressPort
    );

// ---------------------------------------------------------------------- atomicop.c

NTSTATUS
PciDeviceQueryAtomics (
    __in PEXPRESS_PORT ExpressPort,
    __out PBOOLEAN Supported
    );

NTSTATUS
PciDeviceEnableAtomics (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
PciDeviceDisableAtomics (
    __in PEXPRESS_PORT ExpressPort
    );

//
// ------------------------------------------------------------- generic exports
//

#define NON_PAGED_CODE()
#define KD_PAGED_CODE()

#define PciAllocatePool(_Type, _Size)       \
    ExAllocatePoolWithTag(_Type, _Size, 'BicP')

#define PciFreePool(_Pointer)   \
    ExFreePoolWithTag(_Pointer, 'BicP')

#define PciFreeExternalPool(_Pointer)   \
    ExFreePool(_Pointer)

//  BOOLEAN
//  BITS_SET(
//      IN  USHORT C
//      IN  USHORT F
//      )
//
#define BITS_SET(C,F) ((BOOLEAN)(((C) & (F)) == (F)))

//  BOOLEAN
//  ANY_BITS_SET(
//      IN  USHORT C
//      IN  USHORT F
//      )
//
#define ANY_BITS_SET(C,F) ((BOOLEAN)(((C) & (F)) != 0))

//
//
// VOID
// PciConstStringToUnicodeString(
//     OUT PUNICODE_STRING u,
//     IN  PCWSTR p
//     )
//
//
#define PciConstStringToUnicodeString(u, p)                                     \
    (u)->Length = ((u)->MaximumLength = sizeof((p))) - sizeof(WCHAR);   \
    (u)->Buffer = (p)

//
// Define an internal macro to suppress innocuous integer overflow warnings.
//

#define PCI_MARK_SAFE_ADDITION(_Base, _Addend) \
    __analysis_assume((_Base) < ((_Base) + (_Addend)))

//
// Define an internal macro to add an offset to a pointer value.
//

#define Add2Ptr(P,I) ((PVOID)(((PUCHAR)(P))+(I)))

//
// Define an internal macro to set an ARI function number into a PCI_SLOT_NUMBER
//

#define SET_ARI_SLOT(s, f)                              \
    do {                                                \
        (s)->u.bits.FunctionNumber = (f) & 0x7;         \
        (s)->u.bits.DeviceNumber = ((f) >> 3) & 0x1f;   \
        (s)->u.bits.Reserved = 0;                       \
    } while (0)

#define GET_ARI_FUNCTION_NUMBER(s, f)                           \
        do {                                                    \
            (f) = (s)->u.bits.FunctionNumber & 0x7;             \
            (f) = ((s)->u.bits.DeviceNumber & 0x1f) << 3;       \
        } while (0)

#define WPP_CHECK_FOR_NULL_STRING  //to prevent exceptions due to NULL strings

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) \
           WPP_LEVEL_LOGGER(flags)

#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) \
           (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

// 47711976-08c7-44ef-8fa2-082da6a30a30

#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(PciDriverTraceGuid,(47711976,08c7,44ef,8fa2,082da6a30a30), \
        WPP_DEFINE_BIT(DBG_INIT)             /* 0000.0001 */ \
        WPP_DEFINE_BIT(DBG_PNP)              /* 0000.0002 */ \
        WPP_DEFINE_BIT(DBG_IOCTL)            /* 0000.0004 */ \
        WPP_DEFINE_BIT(DBG_ENUM)             /* 0000.0008 */ \
        WPP_DEFINE_BIT(DBG_PPO)              /* 0000.0010 */ \
        WPP_DEFINE_BIT(DBG_DEVPOWER)         /* 0000.0020 */ \
        WPP_DEFINE_BIT(DBG_CONFIG)           /* 0000.0040 */ \
        )

#define PCI_PORT_REMOVE_LOCK_TAG        ((PVOID)(ULONG_PTR)'troP')
#define PCI_PME_REMOVE_LOCK_TAG         ((PVOID)(ULONG_PTR)' emP')
#define PCI_D0RETIRE_LOCK_TAG           ((PVOID)(ULONG_PTR)'tR0D')
