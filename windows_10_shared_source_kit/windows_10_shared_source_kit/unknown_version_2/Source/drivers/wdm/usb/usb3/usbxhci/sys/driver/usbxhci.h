/*++
Copyright (c) Microsoft Corporation

Module Name:

    register.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

//
// Unless otherwise noted, the section number references contained in
// this header file all refer to the following specification:
//
//                  eXtensible Host Controller Interface
//                                  for
//                          Universal Serial Bus
//                                 (xHCI)
//                             Revision 0.96
//                                 5/8/09
//

#include <PSHPACK4.H>

#define MAX_NUMBER_OF_INTERRUPTERS                  0x400

#define HARDWARE_GONE_ULONG32_REGISTER_VALUE        0xFFFFFFFF
#define HARDWARE_GONE_ULONG64_REGISTER_VALUE        0xFFFFFFFFFFFFFFFF

#define MAX_ISOCH_BURST_COUNT                       4
#define MAX_ISOCH_BURST_COUNT_EXTENDED              32

//
// 5.3.1 Capability Registers Length @ Base + 0x00
// 5.3.2 Host Controller Interface Version Number @ Base + 0x02
//
typedef union _HCLENGTHVERSION {

    ULONG   AsUlong32;

    struct {
        ULONG   CapabilityRegisterLength:8;
        ULONG   Reserved0:8;
        ULONG   MinorRevision:8;
        ULONG   MajorRevision:8;
    };

} HCLENGTHVERSION, *PHCLENGTHVERSION;

C_ASSERT(sizeof(HCLENGTHVERSION) == sizeof(ULONG));

//
// 5.3.3 Structural Parameters 1 @ Base + 0x04
//
typedef union _HCSPARAMS1 {

    ULONG   AsUlong32;

    struct {
        ULONG   NumberOfDeviceSlots:8;
        ULONG   NumberOfInterrupters:11;
        ULONG   Reserved0:5;
        ULONG   NumberOfPorts:8;
    };

} HCSPARAMS1, *PHCSPARAMS1;

C_ASSERT(sizeof(HCSPARAMS1) == sizeof(ULONG));

//
// 5.3.4 Structural Parameters 2 @ Base + 0x08
//
typedef union _HCSPARAMS2 {

    ULONG   AsUlong32;

    struct {
        ULONG   IsochSchedulingThreshold:4;
        ULONG   EventRingSegmentTableMax:4;
        ULONG   Reserved0:13;
        ULONG   MaxScratchpadBuffersHi:5;
        ULONG   ScratchpadRestore:1;
        ULONG   MaxScratchpadBuffers:5;
    };

} HCSPARAMS2, *PHCSPARAMS2;

C_ASSERT(sizeof(HCSPARAMS2) == sizeof(ULONG));

//
// 5.3.5 Structural Parameters 3 @ Base + 0x0C
//
typedef union _HCSPARAMS3 {

    ULONG   AsUlong32;

    struct {
        ULONG   U1DeviceExitLatency:8;
        ULONG   Reserved0:8;
        ULONG   U2DeviceExitLatency:16;
    };

} HCSPARAMS3, *PHCSPARAMS3;

C_ASSERT(sizeof(HCSPARAMS3) == sizeof(ULONG));

#define HCSPARAMS3_U1_DEVICE_EXIT_LATENCY_MAX   0x0A
#define HCSPARAMS3_U2_DEVICE_EXIT_LATENCY_MAX   0x07FF

//
// 5.3.6 Capability Parameters @ Base + 0x10
//
typedef union _HCCPARAMS {

    ULONG   AsUlong32;

    struct {
        ULONG   AddressingCapability:1;
        ULONG   BwNegotiationCapability:1;
        ULONG   ContextSize:1;
        ULONG   PortPowerControl:1;
        ULONG   PortIndicators:1;
        ULONG   LightHCResetCapability:1;
        ULONG   LatencyToleranceMessagingCapability:1;
        ULONG   NoSecondarySidSupport:1;
        ULONG   ParseAllEventData:1;
        ULONG   StoppedShortPacketCapability:1;
        ULONG   StoppedEDTLACapability:1;
        ULONG   ContiguousFrameIDCapability:1;
        ULONG   MaximumPrimaryStreamArraySize:4;
        ULONG   XhciExtendedCapabilitiesPointer:16;
    };

} HCCPARAMS, *PHCCPARAMS;

C_ASSERT(sizeof(HCCPARAMS) == sizeof(ULONG));

//
// 5.3.9 Capability Parameters 2 @ Base + 0x1C
//
typedef union _HCCPARAMS2 {

    ULONG   AsUlong32;

    struct {
        ULONG   U3EntryCapability:1;
        ULONG   ConfigureEndpointCommandMaxExitLatencyCapability:1;
        ULONG   ForceSaveContextCapability:1;
        ULONG   ComplianceTransitionCapability:1;
        ULONG   LargeESITPayloadCapability:1;
        ULONG   ConfigurationInformationCapability:1;
        ULONG   ExtendedTBCCapability:1;
        ULONG   Reserved:25;
    };

} HCCPARAMS2, *PHCCPARAMS2;

C_ASSERT(sizeof(HCCPARAMS2) == sizeof(ULONG));

typedef struct _CAPABILITY_REGISTERS {

    HCLENGTHVERSION         LengthVersion;                  // Base + 0x00

    HCSPARAMS1              StructuralParameters1;          // Base + 0x04

    HCSPARAMS2              StructuralParameters2;          // Base + 0x08

    HCSPARAMS3              StructuralParameters3;          // Base + 0x0C

    HCCPARAMS               CapabilityParameters;           // Base + 0x10

    ULONG                   DoorbellOffset;                 // Base + 0x14

    ULONG                   RuntimeRegisterSpaceOffset;     // Base + 0x18

    HCCPARAMS2              CapabilityParameters2;          // Base + 0x1C
} CAPABILITY_REGISTERS, *PCAPABILITY_REGISTERS;

//
// 5.4.1 USB Command Register @ Operational Base + 0x00
//
typedef union _USBCMD {

    ULONG   AsUlong32;

    struct {
        ULONG   RunStopBit:1;
        ULONG   HostControllerReset:1;
        ULONG   InterrupterEnable:1;
        ULONG   HostSystemErrorEnable:1;
        ULONG   Reserved0:3;
        ULONG   LightHostControllerReset:1;
        ULONG   ControllerSaveState:1;
        ULONG   ControllerRestoreState:1;
        ULONG   EnableWrapEvent:1;
        ULONG   EnableU3MfIndexStop:1;
        ULONG   Reserved1:1;
        ULONG   CEMEnable:1;
        ULONG   ExtendedTBCEnable:1;
        ULONG   Reserved2:17;
    };

} USBCMD, *PUSBCMD;

C_ASSERT(sizeof(USBCMD) == sizeof(ULONG));

//
// 5.4.2 USB Status Register @ Operational Base + 0x04
//
typedef union _USBSTS {

    ULONG   AsUlong32;

    struct {
        ULONG   HcHalted:1;
        ULONG   Reserved0:1;
        ULONG   HostSystemError:1;
        ULONG   EventInterrupt:1;
        ULONG   PortChangeDetect:1;
        ULONG   Reserved1:3;
        ULONG   SaveStateStatus:1;
        ULONG   RestoreStateStatus:1;
        ULONG   SaveRestoreError:1;
        ULONG   ControllerNotReady:1;
        ULONG   HostControllerError:1;
        ULONG   Reserved2:19;
    };

} USBSTS, *PUSBSTS;

C_ASSERT(sizeof(USBSTS) == sizeof(ULONG));

//
// 5.4.3 Page Size Register @ Operational Base + 0x08
//
typedef union _PAGESIZE_REGISTER {

    ULONG   AsUlong32;

    struct {
        ULONG   Size:16;
        ULONG   Reserved0:16;
    };

} PAGESIZE_REGISTER, *PPAGESIZE_REGISTER;

C_ASSERT(sizeof(PAGESIZE_REGISTER) == sizeof(ULONG));

//
// 5.4.4. Device Notification Control Register @ Operational Base + 0x14
//
typedef union _DNCTRL {

    ULONG   AsUlong32;

    struct {
        ULONG   Enable0:1;
        ULONG   EnableFunctionWakeNotification:1;
        ULONG   Enable2:1;
        ULONG   Enable3:1;
        ULONG   Enable4:1;
        ULONG   Enable5:1;
        ULONG   Enable6:1;
        ULONG   Enable7:1;
        ULONG   Enable8:1;
        ULONG   Enable9:1;
        ULONG   Enable10:1;
        ULONG   Enable11:1;
        ULONG   Enable12:1;
        ULONG   Enable13:1;
        ULONG   Enable14:1;
        ULONG   Enable15:1;
        ULONG   Reserved0:16;
    };

} DNCTRL, *PDNCTRL;

C_ASSERT(sizeof(DNCTRL) == sizeof(ULONG));

//
// 5.4.5 Command Ring Control Register @ Operational Base + 0x18
//
typedef union _CRCR {

    ULONG64 AsUlong64;

    struct {
        ULONG64 RingCycleState:1;
        ULONG64 CommandStop:1;
        ULONG64 CommandAbort:1;
        ULONG64 CommandRingRunning:1;
        ULONG64 Reserved0:2;
        ULONG64 CommandRingPointer:58;
    };

} CRCR, *PCRCR;

C_ASSERT(sizeof(CRCR) == sizeof(ULONG64));

//
// 5.4.7 Configure Register @ Operational Base + 0x38
//
typedef union _CONFIG_REGISTER {

    ULONG   AsUlong32;

    struct {
        ULONG   NumberOfDeviceSlotsEnabled:8;
        ULONG   U3EntryEnable:1;
        ULONG   ConfigurationInformationEnable:1;
        ULONG   Reserved0:22;
    };

} CONFIG_REGISTER, *PCONFIG_REGISTER;

C_ASSERT(sizeof(CONFIG_REGISTER) == sizeof(ULONG));

typedef struct _OPERATIONAL_REGISTERS {

    USBCMD                  UsbCommand;

    USBSTS                  UsbStatus;

    PAGESIZE_REGISTER       PageSizeRegister;

    ULONG                   Reserved0[2];

    DNCTRL                  DeviceNotificationControl;

    CRCR                    CommandRingControl;

    ULONG                   Reserved1[4];

    ULARGE_INTEGER          DeviceContextBaseAddressArrayPointer;

    CONFIG_REGISTER         ConfigRegister;

} OPERATIONAL_REGISTERS, *POPERATIONAL_REGISTERS;

//
// 5.4.8 Port Status and Control Register @ Port Register Set + 0x00
//
// See section 5.1.1 Attributes for defintions of the {RO, RW, RW1S,
// ROS, RWS, RW1CS, RsvdZ} annotations.  Note in particular that
// writing a '0' to RW1S and RW1CS bits has no effect, which is the
// desired result unless it is desired to explicitly Set or Clear the
// respective bit.
//
typedef union _PORTSC {

    ULONG   AsUlong32;

    struct {
        ULONG   CurrentConnectStatus:1;         // 0        ROS
        ULONG   PortEnabled:1;                  // 1        RW1CS
        ULONG   Reserved0:1;                    // 2        RsvdZ
        ULONG   OverCurrentActive:1;            // 3        RO
        ULONG   PortReset:1;                    // 4        RW1S
        ULONG   PortLinkState:4;                // 5-8      RWS
        ULONG   PortPower:1;                    // 9      * RWS
        ULONG   PortSpeed:4;                    // 10-13    ROS
        ULONG   PortIndicatorControl:2;         // 14-15  * RWS
        ULONG   PortLinkStateWriteStrobe:1;     // 16       RW
        ULONG   ConnectStatusChange:1;          // 17       RW1CS
        ULONG   PortEnabledOrDisabledChange:1;  // 18       RW1CS
        ULONG   WarmPortResetChange:1;          // 19       RW1CS/RsvdZ
        ULONG   OverCurrentChange:1;            // 20       RW1CS
        ULONG   PortResetChange:1;              // 21       RW1CS
        ULONG   PortLinkStateChange:1;          // 22       RW1CS
        ULONG   PortConfigErrorChange:1;        // 23       RW1CS/RsvdZ
        ULONG   ColdAttachStatus:1;             // 24       RO
        ULONG   WakeOnConnectEnable:1;          // 25     * RWS
        ULONG   WakeOnDisconnectEnable:1;       // 26     * RWS
        ULONG   WakeOnOverCurrentEnable:1;      // 27     * RWS
        ULONG   Reserved2:2;                    // 28-29    RsvdZ
        ULONG   DeviceRemovable:1;              // 30       RO
        ULONG   WarmPortReset:1;                // 31       RW1S/RsvdZ
    };

} PORTSC, *PPORTSC;

C_ASSERT(sizeof(PORTSC) == sizeof(ULONG));

//
// When performing a Read/Modify/Write operation only the bits annotated
// with '*' must be preserved and all other bits should be cleared
// before explicitly setting or clearing the bits to be modified.
//
#define PORTSC_RMW_PRESERVATION_MASK    ((1<<27)|(1<<26)|(1<<25)|(1<<15)|(1<<14)|(1<<9))

//
// Port Status and Control Register - PortLinkState (bits 5-8) value
// definitions.
//
#if !defined (PORT_LINK_STATE_U0)
//
// The following Port Link State values match the values defined in the
// Universal Serial Bus 3.0 Specification, Revision 1.0 and these
// values are defined in the public header file USBSPEC.H:
//
#define PORT_LINK_STATE_U0                  0
#define PORT_LINK_STATE_U1                  1
#define PORT_LINK_STATE_U2                  2
#define PORT_LINK_STATE_U3                  3
#define PORT_LINK_STATE_DISABLED            4
#define PORT_LINK_STATE_RX_DETECT           5
#define PORT_LINK_STATE_INACTIVE            6
#define PORT_LINK_STATE_POLLING             7
#define PORT_LINK_STATE_RECOVERY            8
#define PORT_LINK_STATE_HOT_RESET           9
#define PORT_LINK_STATE_COMPLIANCE_MODE     10
#define PORT_LINK_STATE_TEST_MODE           11
#endif
//
// The following Port Link State values are only defined in the xHCI
// specification:
//
#define PORT_LINK_STATE_RESUME              15

//
// Port Status and Control Register - PortSpeed (bits 10-13) value
// definitions.
//
#define PORT_SPEED_UNDEFINED_DEVICE_ATTACHED             1
#define PORT_SPEED_FULL_SPEED_DEVICE_ATTACHED            1
#define PORT_SPEED_LOW_SPEED_DEVICE_ATTACHED             2
#define PORT_SPEED_HIGH_SPEED_DEVICE_ATTACHED            3
#define PORT_SPEED_SUPER_SPEED_DEVICE_ATTACHED           4
#define PORT_SPEED_SUPER_SPEED_PLUS_DEVICE_ATTACHED      5

//
// Slot Context - Speed Flavor (bits 20-23) value
// definitions.
//
#define SLOT_SPEED_FLAVOR_FULL_SPEED_DEVICE              1
#define SLOT_SPEED_FLAVOR_LOW_SPEED_DEVICE               2
#define SLOT_SPEED_FLAVOR_HIGH_SPEED_DEVICE              3
#define SLOT_SPEED_FLAVOR_ENHANCED_SUPER_SPEED_DEVICE    4

//
// 5.4.9.1 USB3 Port PM and Status Control Register
//
typedef union _PORTPMSC_USB3 {

    ULONG   AsUlong32;

    struct {
        ULONG   U1Timeout:8;
        ULONG   U2Timeout:8;
        ULONG   ForceLinkPMAccept:1;
        ULONG   Reserved:15;
    };

} PORTPMSC_USB3, *PPORTPMSC_USB3;

C_ASSERT(sizeof(PORTPMSC_USB3) == sizeof(ULONG));

//
// 5.4.9.2 USB2 Port PM and Status Control Register
//
typedef union _PORTPMSC_USB2 {

    ULONG   AsUlong32;

    struct {
        ULONG   L1Status:3;
        ULONG   RemoteWakeEnable:1;
        ULONG   BestEfforServiceLatency:4;
        ULONG   L1DeviceSlot:8;
        ULONG   HardwareLPMEnable:1;
        ULONG   Reserved0:11;
        ULONG   TestMode:4;
    };

} PORTPMSC_USB2, *PPORTPMSC_USB2;

C_ASSERT(sizeof(PORTPMSC_USB2) == sizeof(ULONG));

//
// 5.4.9 Port PM Status and Control Register @ Port Register Set + 0x04
//
typedef union _PORTPMSC {
    ULONG           AsUlong32;
    PORTPMSC_USB3   Usb3;
    PORTPMSC_USB2   Usb2;
} PORTPMSC, *PPORTPMSC;

C_ASSERT(sizeof(PORTPMSC) == sizeof(ULONG));

//
// 5.4.10 Port Link Info Register @ Port Register Set + 0x08
//
typedef union _PORTLI {

    ULONG   AsUlong32;

    struct {
        ULONG   LinkErrorCount:16;
        ULONG   RxLaneCount:4;
        ULONG   TxLaneCount:4;
        ULONG   Reserved0:8;
    };

} PORTLI, *PPORTLI;

C_ASSERT(sizeof(PORTLI) == sizeof(ULONG));

//
// 5.4.11 Port Hardware LPM Control Register @ Port Register Set + 0x0C
//
typedef union _PORTHLPMC {
    ULONG   AsUlong32;

    struct {
        ULONG   HostInitiatedResumeDurationMode:2;
        ULONG   L1Timeout:8;
        ULONG   BestEffortServiceLatencyDeep:4;
        ULONG   Reserved0:18;
    };
} PORTHLPMC, *PPORTHLPMC;

C_ASSERT(sizeof(PORTHLPMC) == sizeof(ULONG));

//
// 5.4 Port Register Set @ Operational Base + 0x400 + (0x10 * PortNumber)
//
typedef struct _PORT_REGISTER_SET {

    PORTSC                  PortStatusAndControl;

    PORTPMSC                PortPowerManagementStatusAndControl;

    PORTLI                  PortLinkInfo;

    PORTHLPMC               PortHardwareLPMControl;

} PORT_REGISTER_SET, *PPORT_REGISTER_SET;

//
// 5.5.1 Microframe Index Register @ Runtime Registers + 0x00
//
typedef union _MFINDEX {

    ULONG   AsUlong32;

    struct {
        ULONG   MicroFrameIndex:14;
        ULONG   Reserved0:18;
    };

} MFINDEX, *PMFINDEX;

C_ASSERT(sizeof(MFINDEX) == sizeof(ULONG));

//
// 5.5.2.1 Interrupter Management Register @ Interrupt Register Set + 0x00
//
typedef union _IMAN {

    ULONG   AsUlong32;

    struct {
        ULONG   Pending:1;
        ULONG   Enable:1;
        ULONG   Reserved0:30;
    };

} IMAN, *PIMAN;

C_ASSERT(sizeof(IMAN) == sizeof(ULONG));

//
// 5.5.2.2 Interrupter Moderation Register @ Interrupt Register Set + 0x04
//
typedef union _IMOD {

    ULONG   AsUlong32;

    struct {
        ULONG   Inverval:16;
        ULONG   Counter:16;
    };

} IMOD, *PIMOD;

C_ASSERT(sizeof(IMOD) == sizeof(ULONG));

//
// 5.5.2.3.1 Event Ring Segment Table Size Register @ Interrupt Register Set + 0x08
//
typedef union _ERSTSZ {

    ULONG   AsUlong32;

    struct {
        ULONG   Size:16;
        ULONG   Reserved0:16;
    };

} ERSTSZ, *PERSTSZ;

C_ASSERT(sizeof(ERSTSZ) == sizeof(ULONG));

//
// 5.5.2.3.3 Event Ring Dequeue Pointer Register @ Interrupt Register Set + 0x18
//
typedef union _ERDP {

    ULONG64 AsUlong64;

    struct {
        ULONG64 DequeueErstSegmentIndex:3;
        ULONG64 EventHandlerBusy:1;
        ULONG64 DequeuePointer:60;
    };

} ERDP, *PERDP;

C_ASSERT(sizeof(ERDP) == sizeof(ULONG64));

//
// 5.5.2 Interrupter Register Set @ Base + Runtime Register Space Offset + 0x20 + (32 * Interrupter)
//
typedef struct _INTERRUPTER_REGISTER_SET {

    IMAN                    InterrupterManagement;

    IMOD                    InterrupterModeration;

    ERSTSZ                  EventRingSegmentTableSizeRegister;

    ULONG                   Reserved0;

    ULARGE_INTEGER          EventRingSegmentTableBaseAddress;

    ERDP                    EventRingDequeuePointerRegister;

} INTERRUPTER_REGISTER_SET, *PINTERRUPTER_REGISTER_SET;

//
// 5.5 Host Controller Runtime Registers
//
typedef struct _RUNTIME_REGISTERS {

    MFINDEX                         MicroFrameIndex;

    ULONG                           Reserved0[7];

    INTERRUPTER_REGISTER_SET        InterrupterRegisters[MAX_NUMBER_OF_INTERRUPTERS];

} RUNTIME_REGISTERS, *PRUNTIME_REGISTERS;

//
// 5.6 Doorbell Registers @ Base + Doorbell Array Offset + (4 * DoorbellNumber)
//
typedef union _DOORBELL_REGISTER {

    ULONG   AsUlong32;

    struct {
        ULONG   Target:8;
        ULONG   Reserved0:8;
        ULONG   StreamId:16;
    };

} DOORBELL_REGISTER, *PDOORBELL_REGISTER;

C_ASSERT(sizeof(DOORBELL_REGISTER) == sizeof(ULONG));

//
// 6.2.2 Slot Context
//
typedef struct _SLOT_CONTEXT32 {

    ULONG       RouteString:20;
    ULONG       Speed:4;
    ULONG       Reserved0:1;
    ULONG       MultiTT:1;
    ULONG       Hub:1;
    ULONG       ContextEntries:5;

    ULONG       MaxExitLatency:16;
    ULONG       RootHubPortNumber:8;
    ULONG       NumberOfPorts:8;

    ULONG       TTHubSlotId:8;
    ULONG       TTPortNumber:8;
    ULONG       TTThinkTime:2;
    ULONG       Reserved1:4;
    ULONG       InterrupterTarget:10;

    ULONG       UsbDeviceAddress:8;
    ULONG       Reserved2:19;
    ULONG       SlotState:5;

    ULONG       Reserved3[4];

} SLOT_CONTEXT32, *PSLOT_CONTEXT32;

C_ASSERT(sizeof(SLOT_CONTEXT32) == 32);

typedef struct _SLOT_CONTEXT64 {

    SLOT_CONTEXT32;

    ULONG       Reserved[8];

} SLOT_CONTEXT64, *PSLOT_CONTEXT64;

C_ASSERT(sizeof(SLOT_CONTEXT64) == 64);

#define SLOT_STATE_DISABLED_OR_ENABLED  0
#define SLOT_STATE_DEFAULT              1
#define SLOT_STATE_ADDRESSED            2
#define SLOT_STATE_CONFIGURED           3

//
// 6.2.3 Endpoint Context
//
typedef struct _ENDPOINT_CONTEXT32 {

    ULONG       EndpointState:3;
    ULONG       Reserved0:5;
    ULONG       Mult:2;
    ULONG       MaxPrimaryStreams:5;
    ULONG       LinearStreamArray:1;
    ULONG       Interval:8;
    ULONG       MaxEndpointServiceTimeIntervalPayloadHi:8;

    ULONG       ForceEvent:1;
    ULONG       ErrorCount:2;
    ULONG       EndpointType:3;
    ULONG       Reserved2:1;
    ULONG       HostInitiateDisable:1;
    ULONG       MaxBurstSize:8;
    ULONG       MaxPacketSize:16;

    union {
        ULONG64     DequeuePointer;
        struct {
            ULONG64     DequeueCycleState:1;
            ULONG64     Reserved3:3;
            ULONG64     DequeuePointerLo:28;
            ULONG64     DequeuePointerHi:32;
        };
    };

    ULONG       AverageTrbLength:16;
    ULONG       MaxEndpointServiceTimeIntervalPayload:16;

    ULONG       Reserved4[3];

} ENDPOINT_CONTEXT32, *PENDPOINT_CONTEXT32;

C_ASSERT(sizeof(ENDPOINT_CONTEXT32) == 32);

typedef struct _ENDPOINT_CONTEXT64 {

    ENDPOINT_CONTEXT32;

    ULONG       Reserved[8];

} ENDPOINT_CONTEXT64, *PENDPOINT_CONTEXT64;

C_ASSERT(sizeof(ENDPOINT_CONTEXT64) == 64);

#define ENDPOINT_STATE_DISABLED  0
#define ENDPOINT_STATE_RUNNING   1
#define ENDPOINT_STATE_HALTED    2
#define ENDPOINT_STATE_STOPPED   3
#define ENDPOINT_STATE_ERROR     4

//
// 6.2.4.1 Stream Context
//

typedef struct _STREAM_CONTEXT {

    union {

        ULONG64     DequeuePointer;

        struct {
            ULONG64     DequeueCycleState:1;
            ULONG64     StreamContextType:3;
            ULONG64     DequeuePointerLo:28;
            ULONG64     DequeuePointerHi:32;
        };
    };

    union {
        ULONG       Reserved1;
        struct {
            ULONG       StreamEDTLA:24;
            ULONG       Reserved2:8;
        };
        struct {
            ULONG       Reserved3:7;
            ULONG       EdtlaValid_Alt1:1;
            ULONG       StreamEDTLA_Alt1:24;
        };
    };
    union {
        ULONG       Reserved4;
        struct {
            ULONG       PartialTRBLength:17;
            ULONG       Reserved5:15;
        };
        struct {
            ULONG       StreamEDTLA_Alt2:24;
            ULONG       Reserved6:8;
        };
    };

} STREAM_CONTEXT, *PSTREAM_CONTEXT;



//
// Note: The Context Size (CSZ) field in the HCCPARAMS register does not
// apply to Stream Context data structures, they are always 16 bytes in
// size.
//
C_ASSERT(sizeof(STREAM_CONTEXT) == 16);

#define STREAM_CONTEXT_TYPE_SECONDARY_TR        0
#define STREAM_CONTEXT_TYPE_PRIMARY_TR          1
#define STREAM_CONTEXT_TYPE_PRIMARY_SSA_8       2
#define STREAM_CONTEXT_TYPE_PRIMARY_SSA_16      3
#define STREAM_CONTEXT_TYPE_PRIMARY_SSA_32      4
#define STREAM_CONTEXT_TYPE_PRIMARY_SSA_64      5
#define STREAM_CONTEXT_TYPE_PRIMARY_SSA_128     6
#define STREAM_CONTEXT_TYPE_PRIMARY_SSA_256     7

//
// 6.2.1 Device Context
//
typedef struct _DEVICE_CONTEXT32 {

    SLOT_CONTEXT32      SlotContext;

    ENDPOINT_CONTEXT32  EndpointContext[31];

} DEVICE_CONTEXT32, *PDEVICE_CONTEXT32;

typedef struct _DEVICE_CONTEXT64 {

    SLOT_CONTEXT64      SlotContext;

    ENDPOINT_CONTEXT64  EndpointContext[31];

} DEVICE_CONTEXT64, *PDEVICE_CONTEXT64;

//
// 6.1 Device Context Base Address Array
//
typedef struct _DEVICE_CONTEXT_BASE_ARRAY {

    ULONG64             DeviceContextBaseAddress[256];

} DEVICE_CONTEXT_BASE_ARRAY, *PDEVICE_CONTEXT_BASE_ARRAY;

//
// 6.2.5.1 Input Control Context
//
typedef union _DROP_CONTEXT_FLAGS {

    ULONG   AsUlong32;

    struct {
        ULONG       Reserved0:2;
        ULONG       D2:1;
        ULONG       D3:1;
        ULONG       D4:1;
        ULONG       D5:1;
        ULONG       D6:1;
        ULONG       D7:1;
        ULONG       D8:1;
        ULONG       D9:1;
        ULONG       D10:1;
        ULONG       D11:1;
        ULONG       D12:1;
        ULONG       D13:1;
        ULONG       D14:1;
        ULONG       D15:1;
        ULONG       D16:1;
        ULONG       D17:1;
        ULONG       D18:1;
        ULONG       D19:1;
        ULONG       D20:1;
        ULONG       D21:1;
        ULONG       D22:1;
        ULONG       D23:1;
        ULONG       D24:1;
        ULONG       D25:1;
        ULONG       D26:1;
        ULONG       D27:1;
        ULONG       D28:1;
        ULONG       D29:1;
        ULONG       D30:1;
        ULONG       D31:1;
    };

} DROP_CONTEXT_FLAGS;

C_ASSERT(sizeof(DROP_CONTEXT_FLAGS) == sizeof(ULONG));

typedef union _ADD_CONTEXT_FLAGS {

    ULONG   AsUlong32;

    struct {
        ULONG       A0:1;
        ULONG       A1:1;
        ULONG       A2:1;
        ULONG       A3:1;
        ULONG       A4:1;
        ULONG       A5:1;
        ULONG       A6:1;
        ULONG       A7:1;
        ULONG       A8:1;
        ULONG       A9:1;
        ULONG       A10:1;
        ULONG       A11:1;
        ULONG       A12:1;
        ULONG       A13:1;
        ULONG       A14:1;
        ULONG       A15:1;
        ULONG       A16:1;
        ULONG       A17:1;
        ULONG       A18:1;
        ULONG       A19:1;
        ULONG       A20:1;
        ULONG       A21:1;
        ULONG       A22:1;
        ULONG       A23:1;
        ULONG       A24:1;
        ULONG       A25:1;
        ULONG       A26:1;
        ULONG       A27:1;
        ULONG       A28:1;
        ULONG       A29:1;
        ULONG       A30:1;
        ULONG       A31:1;
    };

} ADD_CONTEXT_FLAGS;

C_ASSERT(sizeof(ADD_CONTEXT_FLAGS) == sizeof(ULONG));

typedef struct _INPUT_CONTROL_CONTEXT32 {

    DROP_CONTEXT_FLAGS  DropContextFlags;

    ADD_CONTEXT_FLAGS   AddContextFlags;

    ULONG               Reserved1[5];

    ULONG               ConfigurationValue:8;
    ULONG               InterfaceNumber:8;
    ULONG               AlternateSetting:8;
    ULONG               Reserved2:8;

} INPUT_CONTROL_CONTEXT32, *PINPUT_CONTROL_CONTEXT32;

C_ASSERT(sizeof(INPUT_CONTROL_CONTEXT32) == 32);

typedef struct _INPUT_CONTROL_CONTEXT64 {

    INPUT_CONTROL_CONTEXT32;

    ULONG       Reserved[8];

} INPUT_CONTROL_CONTEXT64, *PINPUT_CONTROL_CONTEXT64;

C_ASSERT(sizeof(INPUT_CONTROL_CONTEXT64) == 64);

//
// 6.2.5 Input Context
//
typedef struct _INPUT_CONTEXT32 {

    INPUT_CONTROL_CONTEXT32     InputControlContext;

    DEVICE_CONTEXT32;

} INPUT_CONTEXT32, *PINPUT_CONTEXT32;

typedef struct _INPUT_CONTEXT64 {

    INPUT_CONTROL_CONTEXT64     InputControlContext;

    DEVICE_CONTEXT64;

} INPUT_CONTEXT64, *PINPUT_CONTEXT64;

//
// 6.4.5 TRB Completion Codes
//
#define TRB_COMPLETION_CODE_INVALID                             0
#define TRB_COMPLETION_CODE_SUCCESS                             1
#define TRB_COMPLETION_CODE_DATA_BUFFER_ERROR                   2
#define TRB_COMPLETION_CODE_BABBLE_DETECTED_ERROR               3
#define TRB_COMPLETION_CODE_USB_TRANSACTION_ERROR               4
#define TRB_COMPLETION_CODE_TRB_ERROR                           5
#define TRB_COMPLETION_CODE_STALL_ERROR                         6
#define TRB_COMPLETION_CODE_RESOURCE_ERROR                      7
#define TRB_COMPLETION_CODE_BANDWIDTH_ERROR                     8
#define TRB_COMPLETION_CODE_NO_SLOTS_AVAILABLE_ERROR            9
#define TRB_COMPLETION_CODE_INVALID_STREAM_TYPE_ERROR           10
#define TRB_COMPLETION_CODE_SLOT_NOT_ENABLED_ERROR              11
#define TRB_COMPLETION_CODE_ENDPOINT_NOT_ENABLED_ERROR          12
#define TRB_COMPLETION_CODE_SHORT_PACKET                        13
#define TRB_COMPLETION_CODE_RING_UNDERRUN                       14
#define TRB_COMPLETION_CODE_RING_OVERRUN                        15
#define TRB_COMPLETION_CODE_VF_EVENT_RING_FULL_ERROR            16
#define TRB_COMPLETION_CODE_PARAMETER_ERROR                     17
#define TRB_COMPLETION_CODE_BANDWIDTH_OVERRUN_ERROR             18
#define TRB_COMPLETION_CODE_CONTEXT_STATE_ERROR                 19
#define TRB_COMPLETION_CODE_NO_PING_RESPONSE_ERROR              20
#define TRB_COMPLETION_CODE_EVENT_RING_FULL_ERROR               21
#define TRB_COMPLETION_CODE_INCOMPATIBLE_DEVICE_ERROR           22
#define TRB_COMPLETION_CODE_MISSED_SERVICE_ERROR                23
#define TRB_COMPLETION_CODE_COMMAND_RING_STOPPED                24
#define TRB_COMPLETION_CODE_COMMAND_ABORTED                     25
#define TRB_COMPLETION_CODE_STOPPED                             26
#define TRB_COMPLETION_CODE_STOPPED_LENGTH_INVALID              27
#define TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET                28
#define TRB_COMPLETION_CODE_MAX_EXIT_LATENCY_TOO_LARGE_ERROR    29
#define TRB_COMPLETION_CODE_ISOCH_BUFFER_OVERRUN                31
#define TRB_COMPLETION_CODE_EVENT_LOST_ERROR                    32
#define TRB_COMPLETION_CODE_UNDEFINED_ERROR                     33
#define TRB_COMPLETION_CODE_INVALID_STREAM_ID_ERROR             34
#define TRB_COMPLETION_CODE_SECONDARY_BANDWIDTH_ERROR           35
#define TRB_COMPLETION_CODE_SPLIT_TRANSACTION_ERROR             36
#define TRB_COMPLETION_CODE_VENDOR_ERROR_MIN                    192
#define TRB_COMPLETION_CODE_VENDOR_ERROR_MAX                    223

//
// 6.4.6 TRB Types
//
#define TRB_TYPE_RESERVED                               0
#define TRB_TYPE_NORMAL                                 1
#define TRB_TYPE_SETUP_STAGE                            2
#define TRB_TYPE_DATA_STAGE                             3
#define TRB_TYPE_STATUS_STAGE                           4
#define TRB_TYPE_ISOCH                                  5
#define TRB_TYPE_LINK                                   6
#define TRB_TYPE_EVENT_DATA                             7
#define TRB_TYPE_NO_OP                                  8
#define TRB_TYPE_ENABLE_SLOT_COMMAND                    9
#define TRB_TYPE_DISABLE_SLOT_COMMAND                   10
#define TRB_TYPE_ADDRESS_DEVICE_COMMAND                 11
#define TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND             12
#define TRB_TYPE_EVALUATE_CONTEXT_COMMAND               13
#define TRB_TYPE_RESET_ENDPOINT_COMMAND                 14
#define TRB_TYPE_STOP_ENDPOINT_COMMAND                  15
#define TRB_TYPE_SET_DEQUEUE_POINTER_COMMAND            16
#define TRB_TYPE_RESET_DEVICE_COMMAND                   17
#define TRB_TYPE_FORCE_EVENT_COMMAND                    18
#define TRB_TYPE_NEGOTIATE_BANDWIDTH_COMMAND            19
#define TRB_TYPE_SET_LATENCY_TOLERANCE_VALUE_COMMAND    20
#define TRB_TYPE_GET_PORT_BANDWIDTH_COMMAND             21
#define TRB_TYPE_FORCE_HEADER_COMMAND                   22
#define TRB_TYPE_NO_OP_COMMAND                          23
#define TRB_TYPE_TRANSFER_EVENT                         32
#define TRB_TYPE_COMMAND_COMPLETION_EVENT               33
#define TRB_TYPE_PORT_STATUS_CHANGE_EVENT               34
#define TRB_TYPE_BANDWIDTH_REQUEST_EVENT                35
#define TRB_TYPE_DOORBELL_EVENT                         36
#define TRB_TYPE_HOST_CONTROLLER_EVENT                  37
#define TRB_TYPE_DEVICE_NOTIFICATION_EVENT              38
#define TRB_TYPE_MFINDEX_WRAP_EVENT                     39

#define TRB_TYPE_VENDOR_CODE_199                        199

//
// 6.4.1.1 Normal TRB
//
typedef struct _NORMAL_TRB {

    ULONG64     DataBufferPointer;

    ULONG       TransferLength:17;
    ULONG       TDSize:5;
    ULONG       InterrupterTarget:10;

    ULONG       CycleBit:1;
    ULONG       EvaluateNextTrb:1;
    ULONG       InterruptOnShortPacket:1;
    ULONG       NoSnoop:1;
    ULONG       ChainBit:1;
    ULONG       InterruptOnCompletion:1;
    ULONG       ImmediateData:1;
    ULONG       Reserved0:2;
    ULONG       BlockEventInterrupt:1;
    ULONG       Type:6;
    ULONG       Reserved1:16;

} NORMAL_TRB, *PNORMAL_TRB;

C_ASSERT(sizeof(NORMAL_TRB) == (sizeof(ULONG)*4));

//
// 6.4.1.2.1 Setup Stage TRB
//
typedef struct _SETUP_STAGE_TRB {

    ULONG       bmRequestType:8;
    ULONG       bRequest:8;
    ULONG       wValue:16;

    ULONG       wIndex:16;
    ULONG       wLength:16;

    ULONG       TransferLength:17;
    ULONG       Reserved0:5;
    ULONG       InterrupterTarget:10;

    ULONG       CycleBit:1;
    ULONG       Reserved1:4;
    ULONG       InterruptOnCompletion:1;
    ULONG       ImmediateData:1;
    ULONG       Reserved2:3;
    ULONG       Type:6;
    ULONG       TransferType:2;
    ULONG       Reserved3:14;

} SETUP_STAGE_TRB, *PSETUP_STAGE_TRB;

C_ASSERT(sizeof(SETUP_STAGE_TRB) == (sizeof(ULONG)*4));

#define SETUP_STAGE_TRANSFER_TYPE_NO_DATA_STAGE     0
#define SETUP_STAGE_TRANSFER_TYPE_OUT_DATA_STAGE    2
#define SETUP_STAGE_TRANSFER_TYPE_IN_DATA_STAGE     3

//
// 6.4.1.2.2 Data Stage TRB
//
typedef struct _DATA_STAGE_TRB {

    ULONG64     DataBufferPointer;

    ULONG       TransferLength:17;
    ULONG       TDSize:5;
    ULONG       InterrupterTarget:10;

    ULONG       CycleBit:1;
    ULONG       EvaluateNextTrb:1;
    ULONG       InterruptOnShortPacket:1;
    ULONG       NoSnoop:1;
    ULONG       ChainBit:1;
    ULONG       InterruptOnCompletion:1;
    ULONG       ImmediateData:1;
    ULONG       Reserved0:3;
    ULONG       Type:6;
    ULONG       Direction:1;
    ULONG       Reserved1:15;

} DATA_STAGE_TRB, *PDATA_STAGE_TRB;

C_ASSERT(sizeof(DATA_STAGE_TRB) == (sizeof(ULONG)*4));

//
// 6.4.1.2.3 Status Stage TRB
//
typedef struct _STATUS_STAGE_TRB {

    ULONG64     Reserved0;

    ULONG       Reserved1:22;
    ULONG       InterrupterTarget:10;

    ULONG       CycleBit:1;
    ULONG       EvaluateNextTrb:1;
    ULONG       Reserved2:2;
    ULONG       ChainBit:1;
    ULONG       InterruptOnCompletion:1;
    ULONG       Reserved3:4;
    ULONG       Type:6;
    ULONG       Direction:1;
    ULONG       Reserved:15;

} STATUS_STAGE_TRB, *PSTATUS_STAGE_TRB;

C_ASSERT(sizeof(STATUS_STAGE_TRB) == (sizeof(ULONG)*4));

//
// 6.4.1.3 Isoch TRB
//
typedef struct _ISOCH_TRB {

    ULONG64     DataBufferPointer;

    ULONG       TransferLength:17;
    ULONG       TDSize:5;
    ULONG       InterrupterTarget:10;

    ULONG       CycleBit:1;
    ULONG       EvaluateNextTrb:1;
    ULONG       InterruptOnShortPacket:1;
    ULONG       NoSnoop:1;
    ULONG       ChainBit:1;
    ULONG       InterruptOnCompletion:1;
    ULONG       ImmediateData:1;
    ULONG       TransferBurstCount:2;
    ULONG       BlockEventInterrupt:1;
    ULONG       Type:6;
    ULONG       TransferLastBurstPacketCount:4;
    ULONG       FrameId:11;
    ULONG       StartIsochASAP:1;

} ISOCH_TRB, *PISOCH_TRB;

C_ASSERT(sizeof(ISOCH_TRB) == (sizeof(ULONG)*4));

//
// 6.4.2.1 Transfer Event TRB
//
typedef struct _TRANSFER_EVENT_TRB {

    PHYSICAL_ADDRESS    Pointer;

    ULONG               TransferLength:24;
    ULONG               CompletionCode:8;

    ULONG               CycleBit:1;
    ULONG               Reserved0:1;
    ULONG               EventData:1;
    ULONG               Reserved1:7;
    ULONG               Type:6;
    ULONG               EndpointId:5;
    ULONG               Reserved2:3;
    ULONG               SlotId:8;

} TRANSFER_EVENT_TRB, *PTRANSFER_EVENT_TRB;

C_ASSERT(sizeof(TRANSFER_EVENT_TRB) == (sizeof(ULONG)*4));

//
// 6.4.2.2 Command Completion Event TRB
//
typedef struct _COMMAND_COMPLETION_EVENT_TRB {

    ULONG64     CommandPointer;

    ULONG       CompletionParameter:24;
    ULONG       CompletionCode:8;

    ULONG       CycleBit:1;
    ULONG       Reserved1:9;
    ULONG       Type:6;
    ULONG       VirtualFunctionId:8;
    ULONG       SlotId:8;

} COMMAND_COMPLETION_EVENT_TRB, *PCOMMAND_COMPLETION_EVENT_TRB;

C_ASSERT(sizeof(COMMAND_COMPLETION_EVENT_TRB) == (sizeof(ULONG)*4));

//
// 6.4.2.3 Port Status Change Event TRB
//
typedef struct _PORT_STATUS_CHANGE_EVENT_TRB {

    ULONG       Reserved0:24;
    ULONG       PortId:8;

    ULONG       Reserved1;

    ULONG       Reserved2:24;
    ULONG       CompletionCode:8;

    ULONG       CycleBit:1;
    ULONG       Reserved3:9;
    ULONG       Type:6;
    ULONG       Reserved4:16;

} PORT_STATUS_CHANGE_EVENT_TRB, *PPORT_STATUS_CHANGE_EVENT_TRB;

C_ASSERT(sizeof(PORT_STATUS_CHANGE_EVENT_TRB) == (sizeof(ULONG)*4));

//
// 6.4.2.6 Host Controller Event TRB
//
typedef struct _HOST_CONTROLLER_EVENT_TRB {

    ULONG64     Reserved0;

    ULONG       Reserved1:24;
    ULONG       CompletionCode:8;

    ULONG       CycleBit:1;
    ULONG       Reserved2:9;
    ULONG       Type:6;
    ULONG       Reserved3:16;

} HOST_CONTROLLER_EVENT_TRB, *PHOST_CONTROLLER_EVENT_TRB;

C_ASSERT(sizeof(HOST_CONTROLLER_EVENT_TRB) == (sizeof(ULONG)*4));

//
// 6.4.2.7 Device Notification Event TRB
//
typedef struct _DEVICE_NOTIFICATION_EVENT_TRB {

    ULONG       Reserved0:4;
    ULONG       NotificationType:4;
    ULONG       DeviceNotificationDataLo:24;

    ULONG       DeviceNotificationDataHi;

    ULONG       Reserved1:24;
    ULONG       CompletionCode:8;

    ULONG       CycleBit:1;
    ULONG       Reserved2:9;
    ULONG       Type:6;
    ULONG       Reserved3:8;
    ULONG       SlotId:8;

} DEVICE_NOTIFICATION_EVENT_TRB, *PDEVICE_NOTIFICATION_EVENT_TRB;

C_ASSERT(sizeof(DEVICE_NOTIFICATION_EVENT_TRB) == (sizeof(ULONG)*4));

#define DEVICE_NOTIFICATION_TYPE_FUNCTION_WAKE                      1
#define DEVICE_NOTIFICATION_TYPE_LATENCY_TOLERANCE_MESSAGE          2
#define DEVICE_NOTIFICATION_TYPE_BUS_INTERVAL_ADJUSTMENT_MESSAGE    3

//
// See USB 3.0 specification:
// 8.5.6.1 Function Wake Device Noification
//
typedef struct _DEVICE_NOTIFICATION_EVENT_FUNCTION_WAKE_TRB {

    ULONG       Reserved0:4;
    ULONG       NotificationType:4;
    ULONG       Interface:8;        // Function Wake Device Noification: Byte 5
    ULONG       Reserved1:16;

    ULONG       Reserved2;

    ULONG       Reserved3:24;
    ULONG       CompletionCode:8;

    ULONG       CycleBit:1;
    ULONG       Reserved4:9;
    ULONG       Type:6;
    ULONG       Reserved5:8;
    ULONG       SlotId:8;

} DEVICE_NOTIFICATION_EVENT_FUNCTION_WAKE_TRB, *PDEVICE_NOTIFICATION_EVENT_FUNCTION_WAKE_TRB;

C_ASSERT(sizeof(DEVICE_NOTIFICATION_EVENT_FUNCTION_WAKE_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.1 No Op Command TRB
//
typedef struct _NOOP_COMMAND_TRB {

    ULONG       Reserved0[3];

    ULONG       CycleBit:1;
    ULONG       Reserved1:9;
    ULONG       Type:6;
    ULONG       Reserved2:16;

} NOOP_COMMAND_TRB, *PNOOP_COMMAND_TRB;

C_ASSERT(sizeof(NOOP_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.2 Enable Slot Command TRB
//
typedef struct _ENABLE_SLOT_COMMAND_TRB {

    ULONG       Reserved0[3];

    ULONG       CycleBit:1;
    ULONG       Reserved1:9;
    ULONG       Type:6;
    ULONG       Reserved2:16;

} ENABLE_SLOT_COMMAND_TRB, *PENABLE_SLOT_COMMAND_TRB;

C_ASSERT(sizeof(ENABLE_SLOT_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.3 Disable Slot Command TRB
//
typedef struct _DISABLE_SLOT_COMMAND_TRB {

    ULONG       Reserved0[3];

    ULONG       CycleBit:1;
    ULONG       Reserved1:9;
    ULONG       Type:6;
    ULONG       Reserved2:8;
    ULONG       SlotId:8;

} DISABLE_SLOT_COMMAND_TRB, *PDISABLE_SLOT_COMMAND_TRB;

C_ASSERT(sizeof(DISABLE_SLOT_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.4 Address Device Command TRB
//
typedef struct _ADDRESS_DEVICE_COMMAND_TRB {

    ULONG64     InputContextPointer;

    ULONG       Reserved0;

    ULONG       CycleBit:1;
    ULONG       Reserved1:8;
    ULONG       BlockSetAddressRequest:1;
    ULONG       Type:6;
    ULONG       Reserved2:8;
    ULONG       SlotId:8;

} ADDRESS_DEVICE_COMMAND_TRB, *PADDRESS_DEVICE_COMMAND_TRB;

C_ASSERT(sizeof(ADDRESS_DEVICE_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.5 Configure Endpoint Command TRB
//
typedef struct _CONFIGURE_ENDPOINT_COMMAND_TRB {

    ULONG64     InputContextPointer;

    ULONG       Reserved0;

    ULONG       CycleBit:1;                         // 0
    ULONG       Reserved1:8;                        // 1-8
    ULONG       Deconfigure:1;                      // 9
    ULONG       Type:6;                             // 10-15
    ULONG       Reserved2:8;                        // 16-23
    ULONG       SlotId:8;                           // 24-31

} CONFIGURE_ENDPOINT_COMMAND_TRB, *PCONFIGURE_ENDPOINT_COMMAND_TRB;

C_ASSERT(sizeof(CONFIGURE_ENDPOINT_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.6 Evaluate Context Command TRB
//
typedef struct _EVALUATE_CONTEXT_COMMAND_TRB {

    ULONG64     InputContextPointer;

    ULONG       Reserved0;

    ULONG       CycleBit:1;
    ULONG       Reserved1:9;
    ULONG       Type:6;
    ULONG       Reserved2:8;
    ULONG       SlotId:8;

} EVALUATE_CONTEXT_COMMAND_TRB, *PEVALUATE_CONTEXT_COMMAND_TRB;

C_ASSERT(sizeof(EVALUATE_CONTEXT_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.7 Reset Endpoint Command TRB
//
typedef struct _RESET_ENDPOINT_COMMAND_TRB {

    ULONG       Reserved0[3];

    ULONG       CycleBit:1;
    ULONG       Reserved1:8;
    ULONG       TransferStatePreserve:1;
    ULONG       Type:6;
    ULONG       EndpointId:5;
    ULONG       Reserved2:3;
    ULONG       SlotId:8;

} RESET_ENDPOINT_COMMAND_TRB, *PRESET_ENDPOINT_COMMAND_TRB;

C_ASSERT(sizeof(RESET_ENDPOINT_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.8 Stop Endpoint Command TRB
//
typedef struct _STOP_ENDPOINT_COMMAND_TRB {

    ULONG       Reserved0[3];

    ULONG       CycleBit:1;
    ULONG       Reserved1:9;
    ULONG       Type:6;
    ULONG       EndpointId:5;
    ULONG       Reserved2:2;
    ULONG       Suspend:1;
    ULONG       SlotId:8;

} STOP_ENDPOINT_COMMAND_TRB, *PSTOP_ENDPOINT_COMMAND_TRB;

C_ASSERT(sizeof(STOP_ENDPOINT_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.9 Set TR Dequeue Pointer Command TRB
//
typedef struct _SET_DEQUEUE_POINTER_COMMAND_TRB {

    union {
        ULONG64     DequeuePointer;
        struct {
            ULONG64     DequeueCycleState:1;
            ULONG64     StreamContextType:3;
            ULONG64     DequeuePointerLo:28;
            ULONG64     DequeuePointerHi:32;
        };
    };

    ULONG       Reserved0:16;
    ULONG       StreamId:16;

    ULONG       CycleBit:1;
    ULONG       Reserved1:9;
    ULONG       Type:6;
    ULONG       EndpointId:5;
    ULONG       Reserved2:3;
    ULONG       SlotId:8;

} SET_DEQUEUE_POINTER_COMMAND_TRB, *PSET_DEQUEUE_POINTER_COMMAND_TRB;

C_ASSERT(sizeof(SET_DEQUEUE_POINTER_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.3.10 Reset Device Command TRB
//
typedef struct _RESET_DEVICE_COMMAND_TRB {

    ULONG       Reserved0[3];

    ULONG       CycleBit:1;                         // 0
    ULONG       Reserved1:9;                        // 1-9
    ULONG       Type:6;                             // 10-15
    ULONG       Reserved2:8;                        // 16-23
    ULONG       SlotId:8;                           // 24-31

} RESET_DEVICE_COMMAND_TRB, *PRESET_DEVICE_COMMAND_TRB;

C_ASSERT(sizeof(RESET_DEVICE_COMMAND_TRB) == (sizeof(ULONG)*4));

//
// 6.4.4.1 Link TRB
//
typedef struct _LINK_TRB {

    ULONG64     RingSegmentPointer;

    ULONG       Reserved0:22;
    ULONG       InterrupterTarget:10;

    ULONG       CycleBit:1;
    ULONG       ToggleCycle:1;
    ULONG       Reserved1:2;
    ULONG       ChainBit:1;
    ULONG       InterruptOnCompletion:1;
    ULONG       Reserved2:4;
    ULONG       Type:6;
    ULONG       Reserved3:16;

} LINK_TRB, *PLINK_TRB;

C_ASSERT(sizeof(LINK_TRB) == (sizeof(ULONG)*4));

//
// 6.4.4.2 Event Data TRB
//
typedef struct _EVENT_DATA_TRB {

    ULONG64     Data;

    ULONG       Reserved0:22;
    ULONG       InterrupterTarget:10;

    ULONG       CycleBit:1;
    ULONG       EvaluateNextTrb:1;
    ULONG       Reserved1:2;
    ULONG       ChainBit:1;
    ULONG       InterruptOnCompletion:1;
    ULONG       Reserved2:3;
    ULONG       BlockEventInterrupt:1;
    ULONG       Type:6;
    ULONG       Reserved3:16;

} EVENT_DATA_TRB, *PEVENT_DATA_TRB;

C_ASSERT(sizeof(EVENT_DATA_TRB) == (sizeof(ULONG)*4));

typedef struct _TRB {
    union {
        NORMAL_TRB                          Normal;                     // 6.4.1.1
        SETUP_STAGE_TRB                     SetupStage;                 // 6.4.1.2.1
        DATA_STAGE_TRB                      DataStage;                  // 6.4.1.2.2
        STATUS_STAGE_TRB                    StatusStage;                // 6.4.1.2.3
        ISOCH_TRB                           Isoch;                      // 6.4.1.3
        TRANSFER_EVENT_TRB                  TransferEvent;              // 6.4.2.1
        COMMAND_COMPLETION_EVENT_TRB        CommandCompletionEvent;     // 6.4.2.2
        PORT_STATUS_CHANGE_EVENT_TRB        PortStatusChangeEvent;      // 6.4.2.3
        HOST_CONTROLLER_EVENT_TRB           HostControllerEvent;        // 6.4.2.6
        DEVICE_NOTIFICATION_EVENT_TRB       DeviceNotificationEvent;    // 6.4.2.7
        DEVICE_NOTIFICATION_EVENT_FUNCTION_WAKE_TRB
                                            DeviceNotificationEventFunctionWake;
        NOOP_COMMAND_TRB                    NoOpCommand;                // 6.4.3.1
        ENABLE_SLOT_COMMAND_TRB             EnableSlotCommand;          // 6.4.3.2
        DISABLE_SLOT_COMMAND_TRB            DisableSlotCommand;         // 6.4.3.3
        ADDRESS_DEVICE_COMMAND_TRB          AddressDeviceCommand;       // 6.4.3.4
        CONFIGURE_ENDPOINT_COMMAND_TRB      ConfigureEndpointCommand;   // 6.4.3.5
        EVALUATE_CONTEXT_COMMAND_TRB        EvaluateContextCommand;     // 6.4.3.6
        RESET_ENDPOINT_COMMAND_TRB          ResetEndpointCommand;       // 6.4.3.7
        STOP_ENDPOINT_COMMAND_TRB           StopEndpointCommand;        // 6.4.3.8
        SET_DEQUEUE_POINTER_COMMAND_TRB     SetDequeuePointerCommand;   // 6.4.3.9
        RESET_DEVICE_COMMAND_TRB            ResetDeviceCommand;         // 6.4.3.10
        LINK_TRB                            Link;                       // 6.4.4.1
        EVENT_DATA_TRB                      EventData;                  // 6.4.4.2
    };
} TRB, *PTRB;

C_ASSERT(sizeof(TRB) == (sizeof(ULONG)*4));

//
// 6.5 Event Ring Segment Table Data Structure
//
typedef struct _EVENT_RING_SEGMENT_TABLE {

    ULONG64     BaseAddress;

    ULONG       SegmentSize:16;
    ULONG       Reserved0:16;

    ULONG       Reserved1;

} EVENT_RING_SEGMENT_TABLE, *PEVENT_RING_SEGMENT_TABLE;

//
// 6.6 Scratchpad Buffer Array - This is a variable length array, based on
//     the MaxScratchpadBuffers value in the host controller params.
//
typedef struct _SCRATCHPAD_BUFFER_ARRAY {

    ULONG64             DeviceContextBaseAddress[1];

} SCRATCHPAD_BUFFER_ARRAY, *PSCRATCHPAD_BUFFER_ARRAY;

//
// 7 xHCI Extended Capabilities
//
typedef union _EXTENDED_CAPABILITY {

    ULONG   AsUlong32;

    struct {

        //
        // Capability ID - RO. This field identifies the xHCI Extended
        // capability. Refer to Table 138 for a list of the valid xHCI
        // extended capabilities.
        //
        ULONG   CapabilityID:8;                     // 0-7

        //
        // Next xHCI Extended Capability Pointer - RO. This field points
        // to the xHC MMIO space offset of the next xHCI extended
        // capability pointer. A value of 00h indicates the end of the
        // extended capability list. A non-zero value in this register
        // indicates a relative offset, in Dwords, from this Dword to
        // the beginning of the next extended capability.
        // For example, assuming an affective address of this data
        // structure is 350h and assuming a pointer value of 068h, we
        // can calculate the following effective address:
        // 350h + (068h << 2) -> 350h + 1A0h -> 4F0h
        //
        ULONG   NextCapabilityPointer:8;            // 8-15

        //
        // Capability Specific. The definition and attributes of these
        // bits depends on the specific capability.
        //
        ULONG   CapabilitySpecific:16;              // 16-31
    };

} EXTENDED_CAPABILITY, *PEXTENDED_CAPABILITY;

C_ASSERT(sizeof(EXTENDED_CAPABILITY) == sizeof(ULONG));

#define EXTENDED_CAPABILITY_ID_USB_LEGACY_SUPPORT           1
#define EXTENDED_CAPABILITY_ID_SUPPORTED_PROTOCOL           2
#define EXTENDED_CAPABILITY_ID_EXTENDED_POWER_MANAGEMENT    3
#define EXTENDED_CAPABILITY_ID_IO_VIRTUALIZATION            4
#define EXTENDED_CAPABILITY_ID_MESSAGE_INTERRUPT            5
#define EXTENDED_CAPABILITY_ID_LOCAL_MEMORY                 6
#define EXTENDED_CAPABILITY_ID_USB_DEBUG_CAPABILITY         10
#define EXTENDED_CAPABILITY_ID_EXTENDED_MESSAGE_INTERRUPT   17

//
// 7.1 USB Legacy Support Capability
//

//
// 7.1.1 USB Legacy Support Capability (USBLEGSUP)
//

typedef union _HC_BIOS_OWNED_SEMAPHORE {

    UCHAR   AsUchar8;

    struct {

        UCHAR   HcBiosOwnedSemaphore:1;

        UCHAR   Reserved:7;
    };

} HC_BIOS_OWNED_SEMAPHORE, *PHC_BIOS_OWNED_SEMAPHORE;

C_ASSERT(sizeof(HC_BIOS_OWNED_SEMAPHORE) == sizeof(UCHAR));

typedef union _HC_OS_OWNED_SEMAPHORE {

    UCHAR   AsUchar8;

    struct {

        UCHAR   HcOsOwnedSemaphore:1;

        UCHAR   Reserved:7;
    };

} HC_OS_OWNED_SEMAPHORE, *PHC_OS_OWNED_SEMAPHORE;

C_ASSERT(sizeof(HC_OS_OWNED_SEMAPHORE) == sizeof(UCHAR));

typedef union _USBLEGSUP {

    ULONG   AsUlong32;

    struct {

        //
        // Capability ID - RO. This field identifies the extended
        // capability. Refer to Table 138 for the value that identifies
        // the capability as Legacy Support.
        // This extended capability requires one additional 32-bit
        // register for control/status information (USBLEGCTLSTS), and
        // this register is located at offset xECP+04h.
        //
        UCHAR   CapabilityID;                           // 0-7

        //
        // Next Capability Pointer - RO. This field indicates the
        // location of the next capability with respect to the effective
        // address of this capability. Refer to Table 137 for more
        // information on this field.
        //
        UCHAR   NextCapabilityPointer;                  // 8-15

        //
        // HC BIOS Owned Semaphore - RW. Default = '0'. The BIOS sets
        // this bit to establish ownership of the xHC. System BIOS will
        // set this bit to a '0' in response to a request for ownership
        // of the xHC by system software.
        //
        HC_BIOS_OWNED_SEMAPHORE HcBiosOwnedSemaphore;   // 16-23

        //
        // HC OS Owned Semaphore - RW. Default = '0'. System software
        // sets this bit to request ownership of the xHC. Ownership is
        // obtained when this bit reads as '1' and the HC BIOS Owned
        // Semaphore bit reads as '0'.
        //
        HC_OS_OWNED_SEMAPHORE   HcOsOwnedSemaphore;     // 24-31
    };

} USBLEGSUP, *PUSBLEGSUP;

C_ASSERT(sizeof(USBLEGSUP) == sizeof(ULONG));

//
// 7.1.2 USB Legacy Support Control/Status (USBLEGCTLSTS)
//
typedef union _USBLEGCTLSTS {

    ULONG   AsUlong32;

    struct {

        //
        // USB SMI Enable - RW. Default = '0'. When this bit is a '1',
        // and the SMI on SMI on Event Interrupt bit (below) in this
        // register is a '1', the host controller will issue an SMI
        // immediately.
        //
        ULONG   UsbSmiEnable:1;                     // 0

        ULONG   Reserved0:3;                        // 1-3

        //
        // SMI on Host System Error Enable - RW. Default = '0'. When
        // this bit is a '1', and the SMI on Host System Error bit
        // (below) in this register is a '1', the host controller will
        // issue an SMI immediately.
        //
        ULONG   SmiOnHostSystemErrorEnable:1;       // 4

        ULONG   Reserved1:8;                        // 5-12

        //
        // SMI on OS Ownership Enable - RW. Default = '0'. When this bit
        // is a '1' AND the OS Ownership Change bit is '1', the host
        // controller will issue an SMI.
        //
        ULONG   SmiOnOsOwnershipEnable:1;           // 13

        //
        // SMI on PCI Command Enable - RW. Default = '0'. When this bit
        // is '1' and SMI on PCI Command is '1', then the host
        // controller will issue an SMI.
        //
        ULONG   SmiOnPciCommandEnable:1;            // 14

        //
        // SMI on BAR Enable - RW. Default = '0'. When this bit is '1'
        // and SMI on BAR is '1', then the host controller will issue an
        // SMI.
        //
        ULONG   SmiOnBarEnable:1;                   // 15

        //
        // SMI on Event Interrupt - RO. Default = '0'. Shadow bit of
        // Event Interrupt (EINT) bit in the USBSTS register. Refer to
        // Section 5.4.2 for definition.
        // This bit follows the state the Event Interrupt (EINT) bit in
        // the USBSTS register, e.g. it automatically clears when EINT
        // clears or set when EINT is set.
        //
        ULONG   SmiOnEventInterrupt:1;              // 16

        ULONG   Reserved2:3;                        // 17-19

        //
        // SMI on Host System Error - RO. Default = '0'. Shadow bit of
        // Host System Error (HSE) bit in the USBSTS register refer to
        // Section 5.4.2 for definition and effects of the events
        // associated with this bit being set to '1'.
        // To clear this bit to a '0', system software shall write a '1'
        // to the Host System Error (HSE) bit in the USBSTS register.
        //
        ULONG   SmiOnHostSystemError:1;             // 20

        ULONG   Reserved3:8;                        // 21-28

        //
        // SMI on OS Ownership Change - RW1C. Default = '0'. This bit is
        // set to '1' whenever the HC OS Owned Semaphore bit in the
        // USBLEGSUP register transitions from '1' to a '0' or '0' to a
        // '1'.
        //
        ULONG   SmiOnOsOnwershipChange:1;           // 29

        //
        // SMI on PCI Command - RW1C. Default = '0'. This bit is set to
        // '1' whenever the PCI Command Register is written.
        //
        ULONG   SmiOnPciCommand:1;                  // 30

        //
        // SMI on BAR - RW1C. Default = '0'. This bit is set to '1'
        // whenever the Base Address Register (BAR) is written.
        //
        ULONG   SmiOnBar:1;                         // 31
    };

} USBLEGCTLSTS, *PUSBLEGCTLSTS;

C_ASSERT(sizeof(USBLEGCTLSTS) == sizeof(ULONG));

typedef union _USBLEGSUPCAPABILITY {

    ULONG   AsUlong32[2];

    struct {
        USBLEGSUP       UsbLegacySupportCapability;
        USBLEGCTLSTS    UsbLegacySupportControlStatus;
    };

} USBLEGSUPCAPABILITY, *PUSBLEGSUPCAPABILITY;

C_ASSERT(sizeof(USBLEGSUPCAPABILITY) == (2 * sizeof(ULONG)));

//
// 7.6 Debug Capability
//

//
// 7.6.8.1 Debug Capability ID (DCID)
//

typedef union _DC_ID {

    ULONG AsUlong32;

    struct {

        //
        // Capability ID - RO. This field identifies the extended
        // capability. Refer to Table 138 for the value that identifies
        // the capability as Legacy Support.
        // This extended capability requires one additional 32-bit
        // register for control/status information (USBLEGCTLSTS), and
        // this register is located at offset xECP+04h.
        //

        ULONG CapabilityID:8;                                               // 0-7

        //
        // Next Capability Pointer - RO. This field indicates the
        // location of the next capability with respect to the effective
        // address of this capability. Refer to Table 137 for more
        // information on this field.
        //

        ULONG NextCapabilityPointer:8;                                      // 8-15

        //
        // DC Event Ring Segment Table Max (DCERST Max) - R0. 
        // Default = Implementation dependent.  Valid values are 0-15.
        // This field determines the maximum value supporting the Debug 
        // Capability Event Ring Segment Table Base Size registers
        // (7.6.8.3.1) where:
        //  The maximum number of Event Ring Segment Table entries = 2^(DCERST MAX)
        //  e.g. if DCERST Max = 7, then the DC Event Ring Segment Table(s) 
        //  support up to 128 entries, 15 then 32K entries, etc.
        //

        ULONG EventRingSizeExponent:5;                                      // 16-20

        ULONG Reserved:11;                                                  // 21-31
    };

} DC_ID, *PDC_ID;

C_ASSERT(sizeof(DC_ID) == sizeof(ULONG));

//
// 7.6.8.2 Debug Capability Doorbell Register (DCDB) @ Base + 0x04
//

typedef union _DC_DOORBELL {

    ULONG AsUlong32;

    struct {

        //
        // Doorbell Target - RW. Default = Returns 0 when read and the value
        // should be treated as undefined. This field defines the target of
        // the doorbell references.
        //

        ULONG Reserved0:8;            // 0-7
        ULONG DoorbellTarget:8;       // 8-15
        ULONG Reserved1:16;           // 16-31
    };

} DC_DOORBELL, *PDC_DOORBELL;

C_ASSERT(sizeof(DC_DOORBELL) == sizeof(ULONG));

//
// 7.6.8.3 Debug Capability Event Ring
//

//
// 7.6.8.3.1 Debug Capability Event Ring Segment Table Size Reg (DCERSTSZ)
//

typedef union _DC_ERSTSZ {

    ULONG AsUlong32;

    struct {
        ULONG SegmentSize:16;
        ULONG Reserved0:16;
    };

} DC_ERSTSZ, *PDC_ERSTSZ;

C_ASSERT(sizeof(DC_ERSTSZ) == sizeof(ULONG));

//
// 7.6.8.3.2 Debug Capability Event Ring Segment Table Base Address Register (DCERSTBA)
//

typedef union _DC_ERSTBA {

    ULONG64 AsUlong64;

    struct {
        ULONG64 Reserved:4;
        ULONG64 EventRingSegmentTableBaseAddress:60;
    };

} DC_ERSTBA, *PDC_ERSTBA;

C_ASSERT(sizeof(DC_ERSTBA) == sizeof(ULONG64));

//
// 7.6.8.3.3 Debug Capability Event Ring Dequeue Pointer Register (DCERDP)
//

typedef union _DC_ERDP {

    ULONG64 AsUlong64;

    struct {
        ULONG64 SegmentIndex:3;
        ULONG64 Reserved:1;
        ULONG64 DequeuePointer:60;
    };

} DC_ERDP, *PDC_ERDP;

C_ASSERT(sizeof(DC_ERDP) == sizeof(ULONG64));

//
// 7.6.8.4 Debug Capability Control Register (DCCTRL)
//

typedef union _DC_CONTROL {

    ULONG AsUlong32;

    struct {

        //
        // Running (DCR) - RO. Default = 0
        // When 0, Debug Device is not in the Configured state.  
        // When 1, Debug Device is in the Configure state and bulk data pipe
        // transactions are accepted by the Debug Capability and routed to 
        // the IN and OUT Transfer Rings.  A 0 to 1 transition of the Port 
        // Reset (DCPORTSC:PR) bit will cear this bit to 0.
        //

        ULONG Running:1;                                     // 0
        ULONG LinkStatusEventEnable:1;                       // 1
        ULONG HaltOutTR:1;                                   // 2
        ULONG HaltInTR:1;                                    // 3
        ULONG RunChange:1;                                   // 4
        ULONG Reserved:11;                                   // 5-15
        ULONG MaxBurstSize:8;                                // 16-23
        ULONG DeviceAddress:7;                               // 24-30
        ULONG Enable:1;                                      // 31
    };

} DC_CONTROL, *PDC_CONTROL;

C_ASSERT(sizeof(DC_CONTROL) == sizeof(ULONG));

//
// 7.6.8.5 Debug Capability Status Register (DCST)
//

typedef union _DC_STATUS {

    ULONG AsUlong32;

    struct {

        //
        // Event Ring Not Empty (ER) - RO. Default = 0
        // When 1, this field indicates that the Debug Capability Event Ring
        // has a Transfer Event on it. It is automatically cleared by the 
        // xHC when the Debug Capability Event Ring is empty, i.e., the Debug
        // Capability Enqueue Pointer is equal to the Debug Capability Event
        // Ring Dequeue Pointer register.
        //

        ULONG EventRingNotEmpty:1;                                      // 0

        //
        // DbC System Bus Reset (SBR) - RO. When '1', this field indicates that
        // the assertion of Chip Hardware Reset, a System Bus (e.g. the assertion
        // of PCI RST#), or a transition from the PCI PM D3hot state to the D0
        // state shall reset the DbC. When '0', this field indicates that a Chip
        // Hardware Reset or the assertion of Host Controller Reset (HCRST = '1')
        // or Light Host Controller Reset (LHCRST = '1') shall reset the DbC.
        // Resetting the DbC shall clear DCE to '0'.
        //

        ULONG SystemBusReset:1;                                         // 1

        ULONG Reserved0:22;                                             // 2-23

        //
        // Debug Port Number - RO. Default = 0
        // This field provides the ID of the Root Hub port that
        // the Debug Capability has been automatically attached to.
        // The value is 0 when the Debug Capability is not attached
        // to a Root Hub port.
        //

        ULONG DebugPortNumber:8;                                        // 24-31
    };

} DC_STATUS, *PDC_STATUS;

C_ASSERT(sizeof(DC_STATUS) == sizeof(ULONG));

//
// 7.6.8.6 Debug Capability Port Status and Control Register (DCPORTSC)
//

typedef union _DC_PORTSC {

    ULONG AsUlong32;

    struct {

        //
        // Current Connect Status (CCS) - RO. Default = 0
        // When 1, a Root Hub port is connected to a Debug Host and
        // assigned to the Debug Capability. When 0, no Debug Host is 
        // present.  This value reflects the current state of the port,
        // and may not correspond to the value reported by the Connect
        // Status Change (CSC) field in the Port Status Change Event 
        // that was generated by a 0 to transition of this bit.
        // This flag is 0 if Debug Capability Enable is 0.
        //

        ULONG CurrentConnectStatus:1;               // 0
        ULONG PortEnabled:1;                        // 1
        ULONG Reserved0:2;                          // 2-3
        ULONG PortReset:1;                          // 4
        ULONG PortLinkState:4;                      // 5-8
        ULONG Reserved1:1;                          // 9
        ULONG PortSpeed:4;                          // 10-13
        ULONG Reserved2:3;                          // 14-16
        ULONG ConnectStatusChange:1;                // 17
        ULONG Reserved3:3;                          // 18-20
        ULONG PortResetChange:1;                    // 21
        ULONG PortLinkStateChange:1;                // 22
        ULONG PortConfigErrorChange:1;              // 23
        ULONG Reserved4:8;                          // 24-31
    };

} DC_PORTSC, *PDC_PORTSC;

C_ASSERT(sizeof(DC_PORTSC) == sizeof(ULONG));

//
// 7.6.8.7 Debug Capability Context Pointer Register (DCCP)
//

typedef union _DC_CP {

    ULONG64 AsUlong64;

    struct {
        ULONG64 Reserved:4;
        ULONG64 ContextPointer:60;
    };

} DC_CP, *PDC_CP;

C_ASSERT(sizeof(DC_CP) == sizeof(ULONG64));

//
// 7.6.8.8 Debug Capability Device Descriptor Info Register 1 (DCDDI1)
//

typedef union _DC_DDI1 {

    ULONG AsUlong32;

    struct {
        ULONG Protocol:8;                 // 0-7
        ULONG Reserved:8;                 // 8-15
        ULONG VendorID:16;                // 16-31
    };

} DC_DDI1, *PDC_DDI1;

C_ASSERT(sizeof(DC_DDI1) == sizeof(ULONG));

//
// 7.6.8.9 Debug Capability Device Descriptor Info Register 1 (DCDDI2)
//

typedef union _DC_DDI2 {

    ULONG AsUlong32;

    struct {
        ULONG ProductID:16;                 // 0-15
        ULONG DeviceRevision:16;            // 16-31
    };

} DC_DDI2, *PDC_DDI2;

C_ASSERT(sizeof(DC_DDI2) == sizeof(ULONG));

//
// 7.6.9.1 Debug Capability Info Context
//

typedef struct _DC_INFO_CONTEXT {
    ULONG64 Language;
    ULONG64 Manufacturer;
    ULONG64 Product;
    ULONG64 SerialNumber;
    ULONG   LanguageLength:8;               // 0-7
    ULONG   ManufacturerLength:8;           // 8-15
    ULONG   ProductLength:8;                // 16-23
    ULONG   SerialNumberLength:8;           // 24-31
    ULONG   Reserved[7];
} DC_INFO_CONTEXT, *PDC_INFO_CONTEXT;

C_ASSERT(sizeof(DC_INFO_CONTEXT) == 64);

typedef struct _DC_CONTEXT {
    DC_INFO_CONTEXT     InfoContext;
    ENDPOINT_CONTEXT64  OutputEndpointContext;
    ENDPOINT_CONTEXT64  InputEndpointContext;
} DC_CONTEXT, *PDC_CONTEXT;

//
// 7.6.8 USB Debug Capability Structure
//

typedef struct _DEBUG_CAPABILITY {
    DC_ID       ID;                                 // Base + 0x00
    DC_DOORBELL Doorbell;                           // Base + 0x04
    DC_ERSTSZ   EventRingSegmentTableSize;          // Base + 0x08
    ULONG       Reserved0;                          // Base + 0x0C
    DC_ERSTBA   EventRingSegmentTableBaseAddress;   // Base + 0x10
    DC_ERDP     EventRingDequeuePointer;            // Base + 0x18
    DC_CONTROL  Control;                            // Base + 0x20
    DC_STATUS   Status;                             // Base + 0x24
    DC_PORTSC   PortStatusControl;                  // Base + 0x28
    ULONG       Reserved1;                          // Base + 0x2C
    DC_CP       ContextPointer;                     // Base + 0x30
    DC_DDI1     Descriptor1;                        // Base + 0x38
    DC_DDI2     Descriptor2;                        // Base + 0x3C
} DEBUG_CAPABILITY, *PDEBUG_CAPABILITY;

C_ASSERT(sizeof(DEBUG_CAPABILITY) == 64);

//
// 7.2 xHCI Supported Protocol Capability
//
typedef union _SUPPPORTED_PROTOCOL_CAPABILITY_REVISION {

    ULONG   AsUlong32;

    struct {

        //
        // Capability ID - RO. Refer to Table 138 for the value that
        // identifies the capability as Supported Protocol.
        //
        ULONG   CapabilityID:8;                     // 0-7

        //
        // Next Capability Pointer - RO. This field indicates the
        // location of the next capability with respect to the effective
        // address of this capability. Refer to Table 137 for more
        // information on this field.
        //
        ULONG   NextCapabilityPointer:8;            // 8-15

        //
        // Minor Revision - RO. Minor Specification Release Number in
        // Binary-Coded Decimal (i.e., version x.10 is 10h). This field
        // identifies the minor release number component of the
        // specification with which the xHC is compliant.
        //
        ULONG   MinorRevision:8;                    // 16-23

        //
        // Major Revision - RO. Major Specification Release Number in
        // Binary-Coded Decimal (i.e., version 3.x is 03h). This field
        // identifies the major release number component of the
        // specification with which the xHC is compliant.
        //
        ULONG   MajorRevision:8;                    // 24-31
    };

} SUPPPORTED_PROTOCOL_CAPABILITY_REVISION, *PSUPPPORTED_PROTOCOL_CAPABILITY_REVISION;

C_ASSERT(sizeof(SUPPPORTED_PROTOCOL_CAPABILITY_REVISION) == sizeof(ULONG));

typedef union _SUPPPORTED_PROTOCOL_CAPABILITY_NAME_STRING {

    ULONG   AsUlong32;

    struct {

        //
        // Name String - RO. This field is a mnemonic name string that
        // references the specification with which the xHC is compliant.
        // Four ASCII characters may be defined. Allowed characters are:
        // alphanumeric, space, and underscore. Alpha characters are
        // case sensitive. Refer to Table 145 for defined values.
        //
        UCHAR   NameString[4];                      // 0-31
    };

} SUPPPORTED_PROTOCOL_CAPABILITY_NAME_STRING, *PSUPPPORTED_PROTOCOL_CAPABILITY_NAME_STRING;

C_ASSERT(sizeof(SUPPPORTED_PROTOCOL_CAPABILITY_NAME_STRING) == sizeof(ULONG));

typedef union _SUPPPORTED_PROTOCOL_CAPABILITY_PORT_OFFSET_COUNT {

    ULONG   AsUlong32;

    struct {

        //
        // Compatible Port Offset - RO. This field specifies the
        // starting Port Number of Root Hub Ports that support this
        // protocol. Valid values are '1' to MaxPorts.
        //
        ULONG   CompatiblePortOffset:8;             // 0-7

        //
        // Compatible Port Count - RO. This field identifies the number
        // of consecutive Root Hub Ports (starting at the Compatible
        // Port Offset) that support this protocol. Valid values are 1
        // to MaxPorts.
        //
        ULONG   CompatiblePortCount:8;              // 8-15

        ULONG   Reserved0:1;                        // 16

        //
        // High-speed Only (HSO) - RO. Default = Implementation
        // dependent. This field only applies to the USB2 protocol. If
        // this bit is set to '0', the USB2 ports described by this
        // capability are Low-, Full-, and High-speed capable. If this
        // bit is set to '1', the USB2 ports described by this
        // capability are High-speed only, e.g. the ports don't support
        // Low- or Full-speed operation. High-speed only implementations
        // may introduce a "Tier mismatch", refer to section 4.24.2 for
        // more information.
        //
        ULONG   HighSpeedOnly:1;                    // 17

        //
        // Integrated Hub Implemented (IHI) - RO. Default =
        // Implementation dependent. This field only applies to the USB2
        // protocol. If this bit is cleared to '0', the Root Hub to
        // External xHC port mapping adheres to the default mapping
        // described in section 4.24.2.1. If this bit is set to '1', the
        // Root Hub to External xHC port mapping does not adhere to the
        // default mapping described in section 4.24.2.1, and an ACPI or
        // other mechanism is required to define the mapping.
        //
        ULONG   IntegratedHubImplemented:1;         // 18

        //
        // Hardware LPM Capability (HLC) - RO. Default = Implementation
        // dependent. If this bit is set to '1', the ports described by
        // this xHCI Supported Protocol Capability support hardware
        // controlled USB2 Link Power Management. Refer to section
        // 4.23.5.1.1.1.
        //
        ULONG   HardwareLpmCapability:1;            // 19

        //
        // BESL LPM Capability (BLC) - RO. Default = Implementation
        // dependent. If this bit is set to '1', the ports described
        // by this xHCI Supported Protocol Capability shall apply BESL
        // timing to BESL and BESLD fields of the PORTPMSC and PORTHLPMC
        // registers, as defined in Table 13. If this bit is cleared to '0',
        // the ports described by this xHCI Supported Protocol Capability
        // shall apply HIRD timing to BESL and BESLD fields of the PORTPMSC
        // and PORTHLPMC registers, as defined in Table 13. Refer to section
        // 4.23.5.1.1.1 for more information.
        //
        ULONG   BeslLpmCapability:1;                // 20

        ULONG   Reserved1:4;                        // 21-24

        //
        // Hub Depth (MHD) - RO. Default = Implementation dependent. If this
        // field is '0', then the standard USB2 hub depth constraints apply,
        // if this field is > '0', then it indicates the maximum hub depth
        // supported by the USB2 ports.
        //
        ULONG   HubDepth:3;                         // 25-27

        //
        // Protocol Speed ID Count (PSIC). This field indicates the number of Protocol Speed ID (PSI)
        // Dwords that the xHCI Supported Protocol Capability data structure contains.
        // If this field is non-zero, then all speeds supported by the protocol shall be defined using PSI
        // Dwords, i.e. no implied Speed ID mappings apply.
        // Refer to section 7.2.2 and its subsections for protocol specific requirements related to this field.
        //
        ULONG   SpeedIDCount:4;                     // 28-31
    };

} SUPPPORTED_PROTOCOL_CAPABILITY_PORT_OFFSET_COUNT, *PSUPPPORTED_PROTOCOL_CAPABILITY_PORT_OFFSET_COUNT;

C_ASSERT(sizeof(SUPPPORTED_PROTOCOL_CAPABILITY_PORT_OFFSET_COUNT) == sizeof(ULONG));

typedef union _SUPPPORTED_PROTOCOL_CAPABILITY_SLOT {

    ULONG   AsUlong32;

    struct {

        ULONG   SlotType:5;                        // 0-4

        ULONG   Reserved:27;                       // 5-31
    };

} SUPPPORTED_PROTOCOL_CAPABILITY_SLOT, *PSUPPPORTED_PROTOCOL_CAPABILITY_SLOT;

C_ASSERT(sizeof(SUPPPORTED_PROTOCOL_CAPABILITY_SLOT) == sizeof(ULONG));

#define SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_TYPE_SYMMETRIC      0
#define SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_TYPE_ASYMMETRIC_RX  2
#define SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_TYPE_ASYMMETRIC_TX  3

#define SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_BPS                 0
#define SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_KBPS                1
#define SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_MBPS                2
#define SUPPPORTED_PROTOCOL_CAPABILITY_SPEED_GBPS                3

typedef union _SUPPPORTED_PROTOCOL_CAPABILITY_SPEED {

    ULONG   AsUlong32;

    struct {

        ULONG   SpeedID:4;

        ULONG   SpeedExponent:2;

        ULONG   PSIType:2;

        //
        // Not defined in the corresponding USB 3.1 SuperSpeedPlus Capability Descriptor
        //
        ULONG   PSIFullDuplex:1;

        ULONG   Reserved:7;

        ULONG   SpeedMantissa:16;
    };

} SUPPPORTED_PROTOCOL_CAPABILITY_SPEED, *PSUPPPORTED_PROTOCOL_CAPABILITY_SPEED;

C_ASSERT(sizeof(SUPPPORTED_PROTOCOL_CAPABILITY_SPEED) == sizeof(ULONG));

typedef union _SUPPPORTED_PROTOCOL_CAPABILITY {

    ULONG   AsUlong32[4];

    struct {
        SUPPPORTED_PROTOCOL_CAPABILITY_REVISION             Revision;

        SUPPPORTED_PROTOCOL_CAPABILITY_NAME_STRING          NameString;

        SUPPPORTED_PROTOCOL_CAPABILITY_PORT_OFFSET_COUNT    PortOffsetCount;

        SUPPPORTED_PROTOCOL_CAPABILITY_SLOT                 Slot;
    };

} SUPPPORTED_PROTOCOL_CAPABILITY, *PSUPPPORTED_PROTOCOL_CAPABILITY;

C_ASSERT(sizeof(SUPPPORTED_PROTOCOL_CAPABILITY) == (4 * sizeof(ULONG)));

#define SUPPORTED_PROTOCOL_SPEED_POINTER(spc)   ((PSUPPPORTED_PROTOCOL_CAPABILITY_SPEED)((spc) + 1))
#define SUPPORTED_PROTOCOL_NAME_STRING_USB      ' BSU'

#include <POPPACK.H>

