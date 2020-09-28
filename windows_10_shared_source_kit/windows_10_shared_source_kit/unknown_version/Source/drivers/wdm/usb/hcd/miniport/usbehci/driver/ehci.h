/*++

Copyright (c) 1999, 2000  Microsoft Corporation

Module Name:

    ehci.h

Abstract:

   Definitions from Enhanced USB 2.0
   controller specification

Environment:

    Kernel & user mode

Revision History:

    1-1-00 : created jdunn

--*/


#ifndef EHCI_H
#define EHCI_H

#include <PSHPACK4.H>
//
// Don't use <PSHPACK1.H> on shared memory data structures that should only
// be accessed using 4-byte load/store instructions (e.g use ld4 instructions
// instead of ld1 instructions on ia64 machines).
//

#define EHCI_PAGE_SIZE      0x1000

//
// Host Controler Hardware Registers as accessed in memory
//

//
// HCLENGTHVERION - Capability Register Length / Interface Versoin Number
//

typedef union _HCLENGTHVERSION {
   ULONG                   ul;
   struct {
      ULONG                 HcCapLength:8;                      // 0-7
      ULONG                 Reserved:8;                         // 8-15
      ULONG                 HcVersion:16;                       // 16-31
   };
} HCLENGTHVERSION, *PHCLENGTHVERSION;

C_ASSERT(sizeof(HCLENGTHVERSION) == sizeof(ULONG));

//
// 2.2.3 HCSPARAMS - Structural Parameters
//

// N_PORTS. This field specifies the number of physical downstream ports
// implemented on this host controller.  The value of this field
// determines how many port registers are addressable in the Operational
// Register Space (see Table 2-8).  Valid values are in the range of 1H
// to FH.  A zero in this field is undefined.
//
#define EHCI_MAX_PORT   15

typedef union _HCSPARAMS {
   ULONG                   ul;
   struct {
      ULONG                 NumberOfPorts:4;                    // 0-3
      ULONG                 PortPowerControl:1;                 // 4
      ULONG                 Reserved1:3;                        // 5-7
      ULONG                 NumberOfPortsPerCompConroller:4;    // 8-11
      ULONG                 NumberOfCompControllers:4;          // 12-15
      ULONG                 PortLEDs:1;                         // 16
      ULONG                 Reserved2:3;                        // 17-19
      ULONG                 DebugPortNumber:4;                  // 20-23
      ULONG                 Reserved3:8;                        // 24-31
   };
} HCSPARAMS, *PHCSPARMS;

C_ASSERT(sizeof(HCSPARAMS) == sizeof(ULONG));

//
// HCCPARAMS - Capability Parameters
//

typedef union _HCCPARAMS {
   ULONG                   ul;
   struct {
      ULONG                 Bit64Addressing:1;      // 0
      ULONG                 ProgramableFrameList:1; // 1
      ULONG                 AsynSchedulePark:1;     // 2
      ULONG                 Reserved1:1;            // 3
      ULONG                 IsochronousThreshold:4; // 4-7
      ULONG                 EECP:8; // 8-15
//INTEL_PSU
      ULONG                 PeriodicScheduleUpdate:1; //16
      ULONG                 Reserved2:15;           // 17-31
//INTEL_PSU
   };
} HCCPARAMS, *PHCCPARMS;

C_ASSERT(sizeof(HCCPARAMS) == sizeof(ULONG));

//
//  EECP Register
//

#define EECP_CAP_RESERVED           0x00
#define EECP_PRE_OS_HANDOFF         0x01

typedef union _HC_EECP {
   ULONG                   ul;
   struct {
      ULONG                 CapId:8;          // 0-7
      ULONG                 NextCap:8;        // 8-15
      ULONG                 CapData:16;       // 16-31
   };
} HC_EECP, *PHC_EECP;

C_ASSERT(sizeof(HC_EECP) == sizeof(ULONG));

//
// EECP BIOS Registers
//

typedef union _USBLEGSUP {
   ULONG                   ul;
   struct {
      ULONG                 CapId:8;          // 0-7
      ULONG                 NextCap:8;        // 8-15
      ULONG                 HcBIOSowned:1;    // 16
      ULONG                 Reserved1:7;      // 17-23
      ULONG                 HcOSowned:1;      // 24
      ULONG                 Reserved2:7;      // 25-31
   };
} USBLEGSUP, *PUSBLEGSUP;

C_ASSERT(sizeof(USBLEGSUP) == sizeof(ULONG));

typedef union _USBLEGCTLSTS {
   ULONG                   ul;
   struct {
      ULONG                 UsbSMIEnable:1;             // 0
      ULONG                 SMIonUSBErrorEn:1;          // 1
      ULONG                 SMIonPortChangeEn:1;        // 2
      ULONG                 SMIonFrameListRolloverEn:1; // 3

      ULONG                 SMIonHostSystemErrorEn:1;   // 4
      ULONG                 SMIonAsyncAdvanceEn:1;      // 5
      ULONG                 Reserved1:7;                // 6-12
      ULONG                 SMIonOSownershipEn:1;       // 13
      ULONG                 SMIonPCICommandEn:1;        // 14
      ULONG                 SMIonBAREn:1;               // 15

      ULONG                 SMIonUSBComplete:1;         // 16
      ULONG                 SMIonUSBError:1;            // 17
      ULONG                 SMIonPortChange:1;          // 18
      ULONG                 SMIonFrameListRollover:1;   // 19

      ULONG                 SMIonHostSystemError:1;     // 20
      ULONG                 SMIonAsyncAdvance:1;        // 21
      ULONG                 Reserved2:7;                // 22-28
      ULONG                 SMIonOSownership:1;         // 29
      ULONG                 SMIonPCIcommand:1;          // 30
      ULONG                 SMIonBAR:1;                 // 31
   };
} USBLEGCTLSTS, *PUSBLEGCTLSTS;

C_ASSERT(sizeof(USBLEGCTLSTS) == sizeof(ULONG));

typedef struct _USB_LEGACYBIOS_REGISTERS {

   USBLEGSUP                Caps;
   USBLEGCTLSTS             CtlSts;

} USB_LEGACYBIOS_REGISTERS, *PUSB_LEGACYBIOS_REGISTERS;

C_ASSERT(sizeof(USB_LEGACYBIOS_REGISTERS) == (2 * sizeof(ULONG)));


//
// CAPABILITIES REGISTER
//

typedef struct _HC_CAPABILITIES_REGISTER {

   HCLENGTHVERSION          HcLengthVersion;
   HCSPARAMS                HcStructuralParameters;
   HCCPARAMS                HcCapabilityParameters;

} HC_CAPABILITIES_REGISTER, *PHC_CAPABILITIES_REGISTER;

C_ASSERT(sizeof(HC_CAPABILITIES_REGISTER) == (3 * sizeof(ULONG)));

//
// USBCMD - USB Command Register
//

//
// Definitions for HC_QTD_TOKEN.Pid
//

#define HcCmd_FrameListSizeIs1024         0
#define HcCmd_FrameListSizeIs512          1
#define HcCmd_FrameListSizeIs256          2


typedef union _USBCMD {

    ULONG                   ul;
    struct {
        ULONG               HostControllerRun:1;        // 0
        ULONG               HostControllerReset:1;      // 1
        ULONG               FrameListSize:2;            // 2-3
        ULONG               PeriodicScheduleEnable:1;   // 4
        ULONG               AsyncScheduleEnable:1;      // 5
        ULONG               IntOnAsyncAdvanceDoorbell:1;// 6
        ULONG               HostControllerLightReset:1; // 7

        ULONG               ParkModeCount:2;            // 8-9
        ULONG               ReservedMBZ:1;              // 10
        ULONG               ParkModeEnable:1;           // 11

//INTEL_PSU
        ULONG               PrefetchEnable:1;           // 12
        ULONG               Reserved1:3;                // 13-15
//INTEL_PSU
        ULONG               InterruptThreshold:8;       // 16-23
        ULONG               Reserved2:8;                // 24-31
    };

} USBCMD, *PUSBCMD;

C_ASSERT(sizeof(USBCMD) == sizeof(ULONG));

//
// USBSTS - USB Status Register
//

#define HcInterruptStatusMask                   0x0000003F

#define HcInt_IntOnasyncAdvance                 0x00000020
#define HcInt_HostSystemError                   0x00000010
#define HcInt_FrameListRollover                 0x00000008
#define HcInt_PortChangeDetect                  0x00000004
#define HcInt_UsbError                          0x00000002
#define HcInt_UsbInterrupt                      0x00000001

typedef union _USBSTS {

    ULONG                   ul;
    struct {
        // controller interrupt status bits
        ULONG               UsbInterrupt:1;                 // 0
        ULONG               UsbError:1;                     // 1
        ULONG               PortChangeDetect:1;             // 2
        ULONG               FrameListRollover:1;            // 3
        ULONG               HostSystemError:1;              // 4
        ULONG               IntOnAsyncAdvance:1;            // 5
        ULONG               ReservedStatus:6;               // 6-11

        // controller status
        ULONG               HcHalted:1;                     // 12
        ULONG               Reclimation:1;                  // 13
        ULONG               PeriodicScheduleStatus:1;       // 14
        ULONG               AsyncScheduleStatus:1;          // 15
        ULONG               Reserved:16;                    // 16-31
    };

} USBSTS, *PUSBSTS;

C_ASSERT(sizeof(USBSTS) == sizeof(ULONG));


//
// USBINTR - USB Interrupt Enable Register
//

typedef union _USBINTR {

    ULONG                   ul;
    struct {
        ULONG               UsbInterrupt:1;                 // 0
        ULONG               UsbError:1;                     // 1
        ULONG               PortChangeDetect:1;             // 2
        ULONG               FrameListRollover:1;            // 3
        ULONG               HostSystemError:1;              // 4
        ULONG               IntOnAsyncAdvance:1;            // 5
        //HostSystemError
        //HostControllerProcessError
        ULONG               Reserved:26;                   // 6-31
    };

} USBINTR, *PUSBINTR;

C_ASSERT(sizeof(USBINTR) == sizeof(ULONG));

//
// FRNUM - Frame Number Register
//

typedef union _FRINDEX {

    ULONG                   ul;
    struct {
        ULONG               FrameListCurrentIndex:14;
        ULONG               Reserved:18;
    };

} FRINDEX, *PFRINDEX;

C_ASSERT(sizeof(FRINDEX) == sizeof(ULONG));

//
// CONFIGFLAG -
//

typedef union _CONFIGFLAG {

    ULONG                   ul;
    struct {
        ULONG               RoutePortsToEHCI:1;
        ULONG               Reserved:31;
    };

} CONFIGFLAG, *PCONFIGFLAG;

C_ASSERT(sizeof(CONFIGFLAG) == sizeof(ULONG));


//
// PORTSC - Port Status and Control Register
//

typedef union _PORTSC {

    ULONG                   ul;
    struct {
        ULONG               PortConnect:1;          // 0
        ULONG               PortConnectChange:1;    // 1
        ULONG               PortEnable:1;           // 2
        ULONG               PortEnableChange:1;     // 3

        ULONG               OvercurrentActive:1;    // 4
        ULONG               OvercurrentChange:1;    // 5
        ULONG               ForcePortResume:1;      // 6
        ULONG               PortSuspend:1;          // 7

        ULONG               PortReset:1;            // 8
        ULONG               HighSpeedDevice:1;      // 9
        ULONG               LineStatus:2;           // 10-11

        ULONG               PortPower:1;            // 12
        ULONG               PortOwnedByCC:1;        // 13
        ULONG               PortIndicator:2;        // 14-15

        ULONG               PortTestControl:4;      // 16-19

        ULONG               WakeOnConnect:1;        // 20
        ULONG               WakeOnDisconnect:1;     // 21
        ULONG               WakeOnOvercurrent:1;    // 22
        ULONG               Reserved:3;             // 23-25
        ULONG               LowSpeed:1;             // 26
        ULONG               HiSpeed:1;              // 27
        ULONG               Reserved2:4;            // 28-31
    };

} PORTSC, *PPORTSC;

C_ASSERT(sizeof(PORTSC) == sizeof(ULONG));














































#ifdef _ARM_




























#endif








































typedef struct _HC_OPERATIONAL_REGISTER {

    // 00h
    USBCMD                          UsbCommand;
    USBSTS                          UsbStatus;
    USBINTR                         UsbInterruptEnable;
    FRINDEX                         UsbFrameIndex;

    // 10h
    HW_32BIT_PHYSICAL_ADDRESS       SegmentSelector;
    HW_32BIT_PHYSICAL_ADDRESS       PeriodicListBase;
    HW_32BIT_PHYSICAL_ADDRESS       AsyncListAddr;
    TTCTRL                          TTControl;

    // 20h
    ULONG                           ReservedB0[4];

    // 30h
    ULONG                           ReservedB1[4];

    // 40h
    CONFIGFLAG                      ConfigFlag;
    PORTSC                          PortRegister[1];

} HC_OPERATIONAL_REGISTER, *PHC_OPERATIONAL_REGISTER;


//#define HcDTYPE_iTD                 0    // iterative TD
//#define HcDTYPE_QH                  1    // queue head
//#define HcDTYPE_siTD                2    // isochronousTD

// note that bits 0,1,2 are used for QH type
// bits 4, and 5 are used for the nak cnt in the
// transfer overlay
#define HW_LINK_FLAGS_MASK          0x0000001f

typedef union _HW_LINK_POINTER {

   HW_32BIT_PHYSICAL_ADDRESS        HwAddress;
// this screws up the 64-bit compiler
#if 0
   struct {
        ULONG Terminate:1;                   // 0
        ULONG DType:2;                       // 1-2
        ULONG ReservedMBZ:2;                 // 3-4
        ULONG PhysicalAddressBits:27;        // 5-31
   };
#endif

} HW_LINK_POINTER, *PHW_LINK_POINTER;

#define EHCI_TERMINATE_BIT      0x00000001 // 00001
#define EHCI_DTYPE_QH           0x00000002 // 00010
#define EHCI_DTYPE_SITD         0x00000004 // 00100
#define EHCI_DTYPE_FSTN         0x00000006 // 00110
#define EHCI_RsvdMBZ            0x00000018 // 11000
#define EHCI_DTYPE_Mask         0x0000001E // 11110

#define SET_T_BIT(addr) ((addr) |= EHCI_TERMINATE_BIT)
#define CLEAR_T_BIT(addr) ((addr) &= ~EHCI_TERMINATE_BIT)
#define SET_SITD(addr) do {\
                        ((addr) &= ~EHCI_DTYPE_Mask);\
                        ((addr) |= EHCI_DTYPE_SITD);\
                       } WHILE (0)

#define SET_QH(addr)  do {\
                        ((addr) &= ~EHCI_DTYPE_Mask);\
                        ((addr) |= EHCI_DTYPE_QH);\
                      } WHILE (0)

#define SET_FSTN(addr) do {\
                        ((addr) &= ~EHCI_DTYPE_Mask);\
                        ((addr) |= EHCI_DTYPE_FSTN);\
                       } WHILE (0)

C_ASSERT(sizeof(HW_LINK_POINTER) == sizeof(ULONG));

//
// Isochronous Transfer Descriptor
//

typedef union _HC_ITD_BUFFER_POINTER0 {
    ULONG ul;
    struct {
        ULONG DeviceAddress:7;          // 0-6
        ULONG Reserved:1;               // 7
        ULONG EndpointNumber:4;         // 8-11
        ULONG BufferPointer:20;         // 12-31
    };
} HC_ITD_BUFFER_POINTER0, *PHC_ITD_BUFFER_POINTER0;

C_ASSERT(sizeof(HC_ITD_BUFFER_POINTER0) == sizeof(ULONG));


typedef union _HC_ITD_BUFFER_POINTER1 {
    ULONG ul;
    struct {
        ULONG MaxPacketSize:11;         // 0-10
        ULONG Direction:1;              // 11
        ULONG BufferPointer:20;         // 12-31
    };
} HC_ITD_BUFFER_POINTER1, *PHC_ITD_BUFFER_POINTER1;

C_ASSERT(sizeof(HC_ITD_BUFFER_POINTER1) == sizeof(ULONG));


typedef union _HC_ITD_BUFFER_POINTER2 {
    ULONG ul;
    struct {
        ULONG Multi:2;                  // 0-1
        ULONG Reserved:10;              // 2-11
        ULONG BufferPointer:20;         // 12-31
    };
} HC_ITD_BUFFER_POINTER2, *PHC_ITD_BUFFER_POINTER2;

C_ASSERT(sizeof(HC_ITD_BUFFER_POINTER2) == sizeof(ULONG));

typedef union _HC_ITD_BUFFER_POINTER {
    ULONG ul;
    struct {
        ULONG Reserved:12;              // 0-11
        ULONG BufferPointer:20;         // 12-31
    };
} HC_ITD_BUFFER_POINTER, *PHC_ITD_BUFFER_POINTER;

C_ASSERT(sizeof(HC_ITD_BUFFER_POINTER) == sizeof(ULONG));


typedef union _HC_ITD_TRANSACTION {
    ULONG ul;
    struct {
        ULONG Offset:12;                // 0-11
        ULONG PageSelect:3;             // 12-14
        ULONG InterruptOnComplete:1;    // 15
        ULONG Length:12;                // 16-27
        ULONG XactError:1;              // 28
        ULONG BabbleDetect:1;           // 29
        ULONG DataBufferError:1;        // 30
        ULONG Active:1;                 // 31
    };
} HC_ITD_TRANSACTION, *PHC_ITD_TRANSACTION;

C_ASSERT(sizeof(HC_ITD_TRANSACTION) == sizeof(ULONG));

// See Figure B-1. 64-bit Isochronous Transaction Descriptor (iTD-64) in
// the Enhanced Host Controller Interface Specification.
//
typedef struct _HW_ISOCHRONOUS_TD {
    HW_LINK_POINTER                     NextLink;
    HC_ITD_TRANSACTION      volatile    Transaction[8];
    HC_ITD_BUFFER_POINTER0              BufferPointer0;
    HC_ITD_BUFFER_POINTER1              BufferPointer1;
    HC_ITD_BUFFER_POINTER2              BufferPointer2;
    HC_ITD_BUFFER_POINTER               BufferPointer3;
    HC_ITD_BUFFER_POINTER               BufferPointer4;
    HC_ITD_BUFFER_POINTER               BufferPointer5;
    HC_ITD_BUFFER_POINTER               BufferPointer6;
    ULONG                               BufferPointer64[7];
} HW_ISOCHRONOUS_TD, *PHW_ISOCHRONOUS_TD;

// A HW_ISOCHRONOUS_TD must be aligned on a 32-byte boundary as the low
// 5 bits of a HW_LINK_POINTER are not used for addressing by the host
// controller hardware.
//
// A HW_ISOCHRONOUS_TD is not padded to a multiple of 32 bytes, but it
// must embedded in a structure (i.e. a HCD_HSISO_TRANSFER_DESCRIPTOR)
// which is a multiple of 32 bytes in size, i.e. {96, 128, 160, 192,
// 224, 256, ...}
//
C_ASSERT(sizeof(HW_ISOCHRONOUS_TD) == 92);

//
// Split Transaction Isochronous Transfer Descriptor
//

typedef union _HC_SITD_CAPS {
    ULONG   ul;
    struct {
        ULONG DeviceAddress:7;          // 0-6
        ULONG Reserved0:1;              // 7
        ULONG EndpointNumber:4;         // 8-11
        ULONG Reserved1:4;              // 12-15
        ULONG HubAddress:7;             // 16-22
        ULONG Reserved2:1;              // 23
        ULONG PortNumber:7;             // 24-30
        ULONG Direction:1;              // 31
    };
} HC_SITD_CAPS, *PHC_SITD_CAPS;

C_ASSERT(sizeof(HC_SITD_CAPS) == sizeof(ULONG));

typedef union _HC_SITD_CONTROL {
    ULONG   ul;
    struct {
        ULONG sMask:8;                  // 0-7
        ULONG cMask:8;                  // 8-15
        ULONG Reserved:16;              // 16-31
    };
} HC_SITD_CONTROL, *PHC_SITD_CONTROL;

C_ASSERT(sizeof(HC_SITD_CONTROL) == sizeof(ULONG));

typedef union _HC_SITD_STATE {
    ULONG   ul;
    struct {
        ULONG Reserved0:1;              // 0
        ULONG SplitXState:1;            // 1
        ULONG MissedMicroframe:1;       // 2
        ULONG XactErr:1;                // 3

        ULONG BabbleDetected:1;         // 4
        ULONG DataBufferError:1;        // 5
        ULONG ERR:1;                    // 6
        ULONG Active:1;                 // 7

        ULONG cProgMask:8;              // 8-15
        ULONG BytesToTransfer:10;       // 16-25
        ULONG Reserved1:4;              // 26-29
        ULONG PageSelect:1;             // 30
        ULONG InterruptOnComplete:1;    // 31
    };
} HC_SITD_STATE, *PHC_SITD_STATE;

C_ASSERT(sizeof(HC_SITD_STATE) == sizeof(ULONG));

// Tposition
#define TPOS_ALL        0
#define TPOS_BEGIN      1

typedef union _HC_SITD_BUFFER_POINTER1 {
    ULONG ul;
    struct {
        ULONG Tcount:3;                 // 0-2
        ULONG Tposition:2;              // 3-4
        ULONG Reseved:7;                // 5-11
        ULONG BufferPointer:20;         // 12-31
    };
} HC_SITD_BUFFER_POINTER1, *PHC_SITD_BUFFER_POINTER1;

C_ASSERT(sizeof(HC_SITD_BUFFER_POINTER1) == sizeof(ULONG));

typedef union _HC_SITD_BUFFER_POINTER0 {
    ULONG ul;
    struct {
        ULONG CurrentOffset:12;         // 0-11
        ULONG BufferPointer:20;         // 12-31
    };
} HC_SITD_BUFFER_POINTER0, *PHC_SITD_BUFFER_POINTER0;

C_ASSERT(sizeof(HC_SITD_BUFFER_POINTER0) == sizeof(ULONG));

typedef struct _HW_SPLIT_ISOCHRONOUS_TD {
    HW_LINK_POINTER                     NextLink;
    HC_SITD_CAPS                        Caps;
    HC_SITD_CONTROL                     Control;
    HC_SITD_STATE           volatile    State;
    HC_SITD_BUFFER_POINTER0             BufferPointer0;
    HC_SITD_BUFFER_POINTER1             BufferPointer1;
    HW_LINK_POINTER                     BackPointer;
    ULONG                               BufferPointer64_0;
    ULONG                               BufferPointer64_1;
    ULONG                               Pad[7];
} HW_SPLIT_ISOCHRONOUS_TD, *PHW_SPLIT_ISOCHRONOUS_TD;


C_ASSERT(sizeof(HW_SPLIT_ISOCHRONOUS_TD) == 64);

//
// Queue Element Transfer Descriptor
//

//
// Definitions for HC_QTD_TOKEN.Pid
//

#define HcTOK_Out           0
#define HcTOK_In            1
#define HcTOK_Setup         2
#define HcTOK_Reserved      3

#define HcTOK_PingDoOut     0
#define HcTOK_PingDoPing    1

#define HcTOK_Toggle0       0
#define HcTOK_Toggle1       1

typedef union _HC_BUFFER_POINTER {
    ULONG ul;
    struct {
        ULONG CurrentOffset:12;     // 0-11
        ULONG BufferPointer:20;     // 12-31
    };
} HC_BUFFER_POINTER, *PHC_BUFFER_POINTER;

C_ASSERT(sizeof(HC_BUFFER_POINTER) == sizeof(ULONG));


typedef union _HC_QTD_TOKEN {
    ULONG   ul;
    struct {
        // status bits
        ULONG PingState:1;        // 0
        ULONG SplitXstate:1;      // 1
        ULONG MissedMicroFrame:1; // 2
        ULONG XactErr:1;          // 3
        ULONG BabbleDetected:1;   // 4
        ULONG DataBufferError:1;  // 5
        ULONG Halted:1;           // 6
        ULONG Active:1;           // 7

        ULONG Pid:2;                    // 8-9
        ULONG ErrorCounter:2;           // 10-11
        ULONG C_Page:3;                 // 12-14
        ULONG InterruptOnComplete:1;    // 15

        ULONG BytesToTransfer:15;       // 16-30
        ULONG DataToggle:1;             // 31
    };
} HC_QTD_TOKEN, *PHC_QTD_TOKEN;

C_ASSERT(sizeof(HC_QTD_TOKEN) == sizeof(ULONG));


// See Figure B-3. 64-bit Queue Element Transaction Descriptor (qTD-64)
// in the Enhanced Host Controller Interface Specification.
//
typedef struct _HW_QUEUE_ELEMENT_TD {
    HW_LINK_POINTER                 Next_qTD;           // Host Controller Read Only
    HW_LINK_POINTER                 AltNext_qTD;        // Host Controller Read Only
    HC_QTD_TOKEN        volatile    Token;              // Host Controller Read/Write
    HC_BUFFER_POINTER               BufferPage[5];
    ULONG                           BufferPage64[5];    // Host Controller Read Only
} HW_QUEUE_ELEMENT_TD, *PHW_QUEUE_ELEMENT_TD;

// A HW_QUEUE_ELEMENT_TD must be aligned on a 32-byte boundary as the
// low 5 bits of a HW_LINK_POINTER are not used for addressing by the
// host controller hardware.
//
// A HW_QUEUE_ELEMENT_TD is not padded to a multiple of 32 bytes, but it
// must embedded in a structure which is a multiple of 32 bytes in size,
// i.e. {64, 96, 128, 160, 192, 224, 256, ...}
//
C_ASSERT(sizeof(HW_QUEUE_ELEMENT_TD) == 52);


typedef union HC_OVLAY_8 {
    ULONG   ul;
    struct {
        // status bits
        ULONG CprogMask:8;        // 0-7
        ULONG Buffer:24;
    };
} HC_OVLAY_8, *PHC_OVLAY_8;

C_ASSERT(sizeof(HC_OVLAY_8) == sizeof(ULONG));

typedef union HC_OVLAY_9 {
    ULONG   ul;
    struct {
        // status bits
        ULONG fTag:5;        // 0-4
        ULONG Sbytes:7;      // 5-11
        ULONG Buffer:20;
    };
} HC_OVLAY_9, *PHC_OVLAY_9;

C_ASSERT(sizeof(HC_OVLAY_9) == sizeof(ULONG));


// See Figure B-4. 64-bit Queue Head Descriptor (QH-64) in the Enhanced
// Host Controller Interface Specification.  The Transfer Overlay is a
// subset of the Queue Head Descriptor.
//
typedef struct _HW_OVERLAY_AREA {
    HW_LINK_POINTER             Next_qTD;       // dw4
    HW_LINK_POINTER             AltNext_qTD;    // dw5
    HC_QTD_TOKEN                Token;          // dw6
    HC_BUFFER_POINTER           BufferPage0;    // dw7
    HC_OVLAY_8                  OverlayDw8;
    HC_OVLAY_9                  OverlayDw9;
    HC_BUFFER_POINTER           BufferPage3;
    HC_BUFFER_POINTER           BufferPage4;
    ULONG                       BufferPage64[5];
} HW_OVERLAY_AREA, *PHW_OVERLAY_AREA;

C_ASSERT(sizeof(HW_OVERLAY_AREA) == 52);
C_ASSERT(sizeof(HW_OVERLAY_AREA) == sizeof(HW_QUEUE_ELEMENT_TD));
C_ASSERT(FIELD_OFFSET(HW_OVERLAY_AREA, Token) == FIELD_OFFSET(HW_QUEUE_ELEMENT_TD, Token));


//
// General Transfer Descriptor
//

typedef union _HW_TRANSFER_DESCRIPTOR {
    HW_QUEUE_ELEMENT_TD         qTD;
    HW_OVERLAY_AREA             Ov;
} HW_TRANSFER_DESCRIPTOR, *PHW_TRANSFER_DESCRIPTOR;

// A HW_TRANSFER_DESCRIPTOR must be aligned on a 32-byte boundary as the
// low 5 bits of a HW_LINK_POINTER are not used for addressing by the
// host controller hardware.
//
// A HW_TRANSFER_DESCRIPTOR is not padded to a multiple of 32 bytes, but
// it must embedded in a structure (i.e. a HCD_TRANSFER_DESCRIPTOR)
// which is a multiple of 32 bytes in size, i.e. {64, 96, 128, 160, 192,
// 224, 256, ...}
//
C_ASSERT(sizeof(HW_TRANSFER_DESCRIPTOR) == 52);

//
// Definitions for HC_ENDPOINT_CHARACTERSITICS.DataToggleControl
//
#define HcEPCHAR_Ignore_Toggle         0    // ignore DT bit from incomming QTD
#define HcEPCHAR_Toggle_From_qTD       1    // DT from incomming QTD

//
// Definitions for HC_ENDPOINT_CHARACTERSITICS.EndpointSpeed
//

#define HcEPCHAR_FullSpeed      0    // 12Mbs
#define HcEPCHAR_LowSpeed       1    // 1.5Mbs
#define HcEPCHAR_HighSpeed      2    // 480Mbs
#define HcEPCHAR_Reserved       3


typedef union _HC_ENDPOINT_CHARACTERSITICS {
    ULONG   ul;
    struct {
        ULONG DeviceAddress:7;          // 0-6
        ULONG IBit:1;                   // 7
        ULONG EndpointNumber:4;         // 8-11
        ULONG EndpointSpeed:2;          // 12-13
        ULONG DataToggleControl:1;      // 14
        ULONG HeadOfReclimationList:1;  // 15
        ULONG MaximumPacketLength:11;   // 16-26
        ULONG ControlEndpointFlag:1;    // 27
        ULONG NakReloadCount:4;         // 28-31
    } ;
} HC_ENDPOINT_CHARACTERSITICS, *PHC_ENDPOINT_CHARACTERSITICS;

C_ASSERT(sizeof(HC_ENDPOINT_CHARACTERSITICS) == sizeof(ULONG));


typedef union _HC_ENDPOINT_CAPABILITIES {
    ULONG   ul;
    struct {
        ULONG InterruptScheduleMask:8;  // 0-7
        ULONG SplitCompletionMask:8;    // 8-15
        ULONG HubAddress:7;             // 16-22
        ULONG PortNumber:7;             // 23-29
        ULONG HighBWPipeMultiplier:2;   // 30-31
    };
} HC_ENDPOINT_CAPABILITIES, *PHC_ENDPOINT_CAPABILITIES;

C_ASSERT(sizeof(HC_ENDPOINT_CAPABILITIES) == sizeof(ULONG));

// Queue Head Descriptor
//
// See Figure B-4. 64-bit Queue Head Descriptor (QH-64) in the Enhanced
// Host Controller Interface Specification.
//
typedef struct _HW_QUEUEHEAD_DESCRIPTOR {

   HW_LINK_POINTER                          HLink;      // horizontal link ptr dw:0
   HC_ENDPOINT_CHARACTERSITICS              EpChars;    // dw:1
   HC_ENDPOINT_CAPABILITIES                 EpCaps;     // dw:2
   HW_LINK_POINTER              volatile    CurrentTD;  // dw:3
   HW_TRANSFER_DESCRIPTOR       volatile    Overlay;    // dw:4-16

} HW_QUEUEHEAD_DESCRIPTOR, *PHW_QUEUEHEAD_DESCRIPTOR;

// A HW_QUEUEHEAD_DESCRIPTOR is not padded to a multiple of 32 bytes,
// but it must embedded in a structure (i.e. a HCD_QUEUEHEAD_DESCRIPTOR)
// which is a multiple of 32 bytes in size, i.e. {64, 96, 128, 160, 192,
// 224, 256, ...}
//
C_ASSERT(sizeof(HW_QUEUEHEAD_DESCRIPTOR) == 68);

//
// Periodic Frame Span Traversal Node (FSTN)
//

typedef struct _HW_PERIODIC_FSTN {

    // contains a link pointer to the next object in the periodic list
    HW_LINK_POINTER                 NormalPath;    // dw:0

    // contains a link pointer to a queue head in the periodic list
    // if T-bit is a 0, then this FSTN is a Save-Place indicator
    // when T-bit is a 1, then this FSTN is the Restore indicator
    HW_LINK_POINTER                 BackPath;      // dw:1

} HW_PERIODIC_FSTN, *PHW_PERIODIC_FSTN;

C_ASSERT(sizeof(HW_PERIODIC_FSTN) == 8);

//
// EHCI Debug Port Registers
//

#ifndef PCI_CAPABILITY_ID_DEBUG_PORT

#define PCI_CAPABILITY_ID_DEBUG_PORT        0x0A

#endif

//
// See section C.1 in the EHCI Specifcation Revision 1.0
//
typedef struct _DEBUGPORTCAPABILITY
{
    ULONG   CapabilityID:8;
    //
    // The value of 0Ah in this field indicates that the function
    // supports a Debug Port.


    ULONG   Next:8;
    //
    // Pointer to the next item in the capabilites list.  Must be NULL
    // for the final item in the list.


    ULONG   Offset:12;
    //
    // This 12 bit field indicatates the byte offset (up to 4K) within
    // the BAR indicated by BAR#.  This offset is required to be DWORD
    // aligned and therefore bits 16 and 17 are always zero.


    ULONG   Reserved:1;


    ULONG   BarNumber:3;
    //
    // A 3-bit field, which indiates which one of the possible 6 Base
    // Address Register offsets contains the Debug Port registers.  For
    // example, a value of 1h indicates the first BAR (offset 10h) while
    // a value of 5 indicates that the BAR is at 20h.  This offset is
    // independent as to whether the BAR is 32 or 64 bit.  For example,
    // if the offset were 3 indicating that the BAR at offset 18h
    // contains the Debug Port, BARs at offset 10h and 14h may or may
    // not be implemented.  This field is read only and only values 1-6h
    // are valid.  (A 64-bit BAR is allowed.)  Only a memory BAR is
    // allowed.

} DEBUGPORTCAPABILITY, *PDEBUGPORTCAPABILITY;

C_ASSERT(sizeof(DEBUGPORTCAPABILITY) == sizeof(ULONG));


//
// See section C.3.1 in the EHCI Specifcation Revision 1.0
//
// Debug Port Control Regsiter
//
// The Debug Port Control register (offset 0h) allows software to
// interact with the USB2 Debug Port.  The hardware associated with this
// register provides no checks to ensure that software programs the
// interface correctly.  How the hardware behaves when programmed
// illegally is undefined.  The definition of each bit or field in the
// control register is described in the table below.  Note: all bits in
// the control register can be written or read simultaneously.  Software
// is required to preserve reserved fields/bits by performing a
// read-modify-write operation.  Registers which have reset values will
// reset based on a HCReset, a D3-D0 transition, or a PCI reset.
//
typedef union _DEBUGPORTCONTROL
{
    ULONG       ul;

    struct
    {
        ULONG   DataLength:4;   // 3:0   R/W
        //
        // For write operations, this field is set by software to
        // indicate to the hardware how many bytes of data in Data
        // Buffer are to be transferred to the console when Write/Read#
        // is set when software sets Go.  A value of 0h indicates that a
        // zero-length packet should be sent.  A value of 1-8 indicates
        // 1-8 bytes are to be transferred.  Values 9-Fh are illegal and
        // how hardware behaves if used is undefined.
        //
        // For read operations, this field is set by hardware to
        // indicate to software how many bytes in Data Buffer are valid
        // in response to software setting Go when Write/Read# is
        // cleared.  A value of 0h indicates that a zero length packet
        // was returned.  (The state of Data Buffer is not defined.)  A
        // value of 1-8 indicates 1-8 bytes were received.  Hardware is
        // not allowed to return values in the range 9-Fh when the
        // Error/Good# field is a zero (0b) after an IN transaction
        // completes.  The value in this field is not valid when the
        // Error/Good# field is a one (1b) after an IN transaction
        // completes.
        //
        // The transferring of data always starts with byte 0 in the
        // data area and moves toward byte 7 until the transfer size is
        // reached. Reset default = 0h.


        ULONG   WriteRead:1;    // 4     R/W
        //
        // Software sets this bit to indicate that the current request
        // is a write and clears it to indicate a read.  Reset default = 0.


        ULONG   Go:1;           // 5     R/W
        //
        // Software sets this bit to cause the hardware to perform a
        // request.  Writing this bit to a 1 when the bit is already set
        // may result in undefined behavior.  Writing a 0 to this bit has
        // no effect.  When set, the hardware clears this bit when the
        // hardware sets the Done bit.  (Completion of a request is
        // indicated by the Done bit.)  Reset default = 0.


        ULONG   ErrorGood:1;    // 6     RO
        //
        // Updated by hardware at the same time it sets the Done bit.
        // When set it indicates that an error occurred.  Details of the
        // error are provided in the Exception field.  When cleared, it
        // indicates that the request terminated successfully.  Reset
        // default = 0.


        ULONG   Exception:3;    // 9:7   RO
        //
        // Exception This field indicates the exception when Error/Good#
        // is set.  This field cannot be cleared by software.  Reset
        // default = 000b.
        //
        // Value        Meaning
        //
        // 000b         None
        //
        // 001b         Transaction error or babble: indicates the USB2
        //              transaction had an error (CRC, bad PID, timeout,
        //              packet babble, etc.)
        //
        // 010b         HW error.  Request was attempted (or in progress)
        //              when port was suspended or reset.
        //
        // 011b-111b    Reserved


        ULONG   InUse:1;        // 10    R/W
        //
        // Set by software to indicate that the port is in use.  Cleared
        // by software to indicate that the port is free and may be used
        // by other software.  Reset default = 0.  (This bit has no affect
        // on hardware.)


        ULONG   Reserved1:5;    // 15:11 RO


        ULONG   Done:1;         // 16    R/WC
        //
        // This bit is set by HW to indicate that the request is
        // complete.  Writing a 1 to this bit will clear it.  Writing a 0
        // to this bit has no effect.  Reset default = 0.


        ULONG   Reserved2:11;   // 27:17 RO


        ULONG   Enabled:1;      // 28    R/W
        //
        // This bit is a one if the debug port is enabled for operation.
        // Software can clear this by writing a zero to it.  The
        // controller clears the bit for the same conditions where
        // hardware sets the Port Enable/Disable Change bit (in the
        // PORTSC register).  (Note: this bit is not cleared when System
        // Software clears the Port Enabled/Disabled bit (in the PORTSC
        // register).  Software can directly set this bit, if the port is
        // already enabled in the associated Port Status and Control
        // register (this is HW enforced).  Reset default = 0.


        ULONG   Reserved3:1;    // 29    RO


        ULONG   Owner:1;        // 30    R/W
        //
        // When debug software writes a one to this bit, the ownership
        // of the debug port is forced to the EHCI controller (ie.
        // immediately taken away from the companion controller).  If
        // the port was already owned by the EHCI controller, then
        // setting this bit is has no effect.  This bit overrides all of
        // the ownership related bits in the standard EHCI registers.
        // Reset default = 0.  Note that the value in this bit may not
        // effect the value reported in the Port Owner bit in the
        // associated PORTSC register.


        ULONG   Reserved4:1;    // 31    RO

    };
} DEBUGPORTCONTROL, *PDEBUGPORTCONTROL;

C_ASSERT(sizeof(DEBUGPORTCONTROL) == sizeof(ULONG));


//
// See section C.3.2 in the EHCI Specifcation Revision 1.0
//
// USB PIDs Register
//
// This Dword register is used to communicate PID information between
// the USB debug driver and the USB2 debug port.  The debug port uses
// some of these fields to generate USB packets, and uses other fields
// to return PID information to the USB debug driver.
//
typedef union _DEBUGPORTUSBPIDS
{
    ULONG       ul;

    struct
    {
        ULONG   TokenPID:8;     // 7:0   R/W
        //
        // The debug port controller sends this PID as the Token PID for
        // each USB transaction.  Software will typically set this field
        // to either IN, OUT or SETUP PID values.  Reset default =
        // undefined.

        ULONG   SendPID:8;      // 15:8  R/W
        //
        // Send PID The debug port controller sends this PID to begin
        // the data packet when sending data to USB (ie. Write/Read# is
        // asserted).  Software will typically set this field to either
        // DATA0 or DATA1 PID values.  Reset default = undefined.

        ULONG   ReceivedPID:8;  // 23:16 RO
        //
        // The debug port controller updates this field with the
        // received PID for transactions in either direction.  When the
        // controller is sending data (Write/Read# is asserted), this
        // field is updated with the handshake PID that is received from
        // the device.  When the host controller is receiving data
        // (Write/Read# is not asserted), this field is updated with the
        // data packet PID (if the device sent data), or the handshake
        // PID (if the device NAKs the request).  This field is valid
        // when the controller sets the Done bit to a one (1b) and the
        // Error/Good# field in the Control/Status register is a zero.
        // If a transaction completes (Done bit transitions to a one)
        // with Error/Good# field set to a one, then the contents of
        // this field are undefined.  Reset default = undefined.

        ULONG   Reserved:8;     // 31:24 RO

    };
} DEBUGPORTUSBPIDS, *PDEBUGPORTUSBPIDS;

C_ASSERT(sizeof(DEBUGPORTUSBPIDS) == sizeof(ULONG));


//
// See section C.3.3 in the EHCI Specifcation Revision 1.0
//
// Data Buffer
//
// The data buffer consists of 8 bytes arranged as two consecutive
// Dwords located at offset 08h and 0Ch in the debug port's register
// space.
//
typedef union _DEBUGPORTDATABUFFER
{
    ULONG       ul;

    struct
    {
        ULONG   Byte0:8;  // 63:32, 31:0  R/W
        ULONG   Byte1:8;  // 63:32, 31:0  R/W
        ULONG   Byte2:8;  // 63:32, 31:0  R/W
        ULONG   Byte3:8;  // 63:32, 31:0  R/W
        //
        // The least significant byte is accessed at offset 08h and the
        // most significant byte is accessed at offset 0Fh.  Each byte in
        // Data Buffer can be individually accessed.
        //
        // Data Buffer must be written with data before software
        // initiates a write request.  For a read request, Data Buffer
        // contains valid data when Done is set, Error/Good# is a zero
        // (0b), and Data Length specifies the number of bytes that are
        // valid. If an IN transaction completes (Done bit transitions
        // to a one) with Error/Good# field set to a one, then the
        // contents of this field are undefined.  Reset default =
        // undefined.

    };
} DEBUGPORTDATABUFFER, *PDEBUGPORTDATABUFFER;

C_ASSERT(sizeof(DEBUGPORTDATABUFFER) == sizeof(ULONG));


typedef union _PACKETBUFFER
{
    struct
    {
        DEBUGPORTDATABUFFER DataBuffer0;
        DEBUGPORTDATABUFFER DataBuffer1;
    };
    UCHAR   Packet[8];
} PACKETBUFFER, *PPACKETBUFFER;

C_ASSERT(sizeof(PACKETBUFFER) == 8);


//
// See section C.3.4 in the EHCI Specifcation Revision 1.0
//
// Device Address Regsiter
//
// The Device Address register holds the information necessary to
// properly address the Debug Device when generating transactions.  It
// specifies the USB device address of the debug device, and the
// endpoint address for USB tokens generated by the debug port.
//
typedef union _DEBUGPORTDEVICEADDRESS
{
    ULONG       ul;

    struct
    {
        ULONG   USBEndpoint:4;  // 3:0   R/W
        //
        // 4-bit field that identifies the endpoint used by the
        // controller for all Token PID generation.  Reset default = 01h.


        ULONG   Reserved1:4;    // 7:3   RO


        ULONG   USBAddress:7;   // 14:8  R/W
        //
        // 7-bit field that identifies the USB device address used by
        // the controller for all Token PID generation. This is a R/W
        // field that is set to 7Fh after power-on reset.  Reset
        // default = 7Fh.

        ULONG   Reserved2:17;   // 31:15 RO

    };
} DEBUGPORTDEVICEADDRESS, *PDEBUGPORTDEVICEADDRESS;

C_ASSERT(sizeof(DEBUGPORTDEVICEADDRESS) == sizeof(ULONG));



typedef struct _HC_DEBUGPORT_REGISTERS
{
    DEBUGPORTCONTROL        Control;        // Offset 0x00

    DEBUGPORTUSBPIDS        USBPIDs;        // Offset 0x04

    DEBUGPORTDATABUFFER     DataBuffer0;    // Offset 0x08

    DEBUGPORTDATABUFFER     DataBuffer1;    // Offset 0x0C

    DEBUGPORTDEVICEADDRESS  DeviceAddress;  // Offset 0x10

} HC_DEBUGPORT_REGISTERS, *PHC_DEBUGPORT_REGISTERS;


//
// Values used in the USB PIDs Register
//
                            // PID    Host Register       BUS bit order
#define PID_OUT     0xE1    // OUT    1110 0001 (0xE1) -> 1000 0111 (0x87)
#define PID_IN      0x69    // IN     0110 1001 (0x69) -> 1001 0110 (0x96)
#define PID_SETUP   0x2D    // SETUP  0010 1101 (0x2D) -> 1011 0100 (0xB4)

#define PID_DATA0   0xC3    // DATA0  1100 0011 (0xC3) -> 1100 0011 (0xC3)
#define PID_DATA1   0x4B    // DATA1  0100 1011 (0x4B) -> 1101 0010 (0xD2)

#define PID_ACK     0xD2    // ACK    1101 0010 (0xD2) -> 0100 1011 (0x4B)
#define PID_NAK     0x5A    // NAK    0101 1010 (0x5A) -> 0101 1010 (0x5A)
#define PID_STALL   0x1E    // STALL  0001 1110 (0x1E) -> 0111 1000 (0x78)
#define PID_NYET    0x96    // NYET   1001 0110 (0x96) -> 0110 1001 (0x69)

#include <POPPACK.H>



#endif /* EHCI_H */
