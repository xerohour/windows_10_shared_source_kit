/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ufshci.h

Abstract:

    Register information to interface with the UFS Host Controller.

Author:

    Erik Schmidt (eriksch) - October 2012

Environment:



Revision History:

--*/

#pragma once

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/unions

#define MAX_REQUEST_LIST_SIZE           0x20
#define MAX_TASK_MAN_LIST_SIZE          0x08

#define MAX_NUM_SCATTER_GATHER          0x40 //0x3FFF
#define MAX_NUM_LU                      0x8

//
// Host Controller Capabilities Registers
//

typedef union _UFS_HCI_CAP {
    struct {
        ULONG NumberOfUtpTransferRequestSlots : 5;
        ULONG Reserved1 : 3;
        ULONG NumberOfOutstandingRTTSupported : 8;
        ULONG NumberofUtpTaskManagementRequestSlots : 3;
        ULONG Reserved2 : 4;
        ULONG AutoH8Supported : 1;
        ULONG Addressing64Supported : 1;
        ULONG OutOfOrderDeliverySupported : 1;
        ULONG UICTestModeSupported : 1;
        ULONG Reserved3 : 5;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_CAP, *PUFS_HCI_CAP;

typedef union _UFS_HCI_VER {
    struct {
        ULONG VersionSuffix : 4;
        ULONG MinorVersionNumber : 4;
        ULONG MajorVersionNumber : 8;
        ULONG Reserved : 16;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_VER, *PUFS_HCI_VER;

typedef union _UFS_HCI_HCPID {
    struct {
        ULONG ProductID;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_HCPID, *PUFS_HCI_HCPID;

typedef union _UFS_HCI_HCMID {
    struct {
        ULONG ManufacturerIdCode : 8;
        ULONG BankIndex : 8;
        ULONG Reserved : 16;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_HCMID, *PUFS_HCI_HCMID;

typedef union _UFS_HCI_AHIT {
    struct {
        ULONG TimerValue : 10;
        ULONG TimerScale : 3;
        ULONG Reserved : 19;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_AHIT, *PUFS_HCI_AHIT;

//
// Operation and Runtime Registers
//

typedef union _UFS_HCI_IS {
    struct {
        ULONG TransferReqStatus : 1;
        ULONG EndpointResetStatus : 1;
        ULONG UicError : 1;
        ULONG UicTestModeStatus : 1;
        ULONG UicPowerModeStatus : 1;
        ULONG UicHibernateExitStatus : 1;
        ULONG UicHibernateEnterStatus : 1;
        ULONG UicLinkLostStatus : 1;
        ULONG UicLinkStartupStatus : 1;
        ULONG TaskMgmtReqCompStatus : 1;
        ULONG UicCmdCompStatus : 1;
        ULONG DeviceFatalErrorStatus : 1;
        ULONG ErrorStatus : 1;
        ULONG Reserved : 3;
        ULONG HostContFatalErrorStatus : 1;
        ULONG SysBusFatalErrorStatus : 1;
        ULONG Reserved2 : 14;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_IS, *PUFS_HCI_IS;

typedef union _UFS_HCI_IE {
    struct {
        ULONG TransferRequestCompEnable : 1;
        ULONG DmeEndpointReset : 1;
        ULONG UicErrorEnable : 1;
        ULONG UicTestModeStatusEnable : 1;
        ULONG UicPowerModeStatusEnable : 1;
        ULONG UicHibernateExitStatusEnable : 1;
        ULONG UicHibernateEnterStatusEnable : 1;
        ULONG UicLinkLostStatusEnable : 1;
        ULONG UicLinkStartupStatusEnable : 1;
        ULONG TaskMgmtReqCompEnable : 1;
        ULONG UicCmdCompEnable : 1;
        ULONG DeviceFatalErrorEnable : 1;
        ULONG ErrorEnable : 1;
        ULONG Reserved : 3;
        ULONG HCFatalErrorEnable : 1;
        ULONG SysBusFatalErrorEnable : 1;
        ULONG Reserved2 : 14;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_IE, *PUFS_HCI_IE;

#define UTP_ERROR_INVALID_UPIU_TYPE     0x01

#define UIC_PWR_OK                      0x00
#define UIC_PWR_LOCAL                   0x01
#define UIC_PWR_REMOTE                  0x02
#define UIC_PWR_BUSY                    0x03
#define UIC_PWR_ERROR_CAP               0x04
#define UIC_PWR_FATAL_ERROR             0x05

typedef union _UFS_HCI_HCS {
    struct {
        ULONG DevicePresent : 1;
        ULONG TransferReqListReady : 1;
        ULONG TaskMgmtReqListReady : 1;
        ULONG UicCmdReady : 1;
        ULONG Reserved : 4;
        ULONG UicPowerModeChangeReqStatus : 3;
        ULONG Reserved2 : 1;
        ULONG UtpErrorCode : 4;
        ULONG UtpErrorTaskTag : 8;
        ULONG UtpErrorLun : 8;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_HCS, *PUFS_HCI_HCS;

typedef union _UFS_HCI_HCE {
    struct {
        ULONG HceEnable : 1;
        ULONG Reserved : 31;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_HCE, *PUFS_HCI_HCE;

#define UFS_PHYSICAL_ERROR                       0x1
#define UFS_DATA_LINK_ERROR                      0x2
#define UFS_NETWORK_ERROR                        0x3
#define UFS_TRANSPORT_ERROR                      0x4
#define UFS_DME_ERROR                            0x5
#define UFS_HCFES_ERROR                          0x6
#define UFS_DFES_ERROR                           0x7

typedef union _UFS_HCI_UECPA {
    struct {
        ULONG ErrorCode : 5;
        ULONG Reserved : 26;
        ULONG Error : 1;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UECPA, *PUFS_HCI_UECPA;

#define DATA_ERR_NAC_RECEIVED                    0x0001
#define DATA_ERR_TCx_REPLAY_TIMER_EXPIRED        0x0002
#define DATA_ERR_AFCx_REQUEST_TIMER_EXPIRED      0x0004
#define DATA_ERR_FCx_PROTECTION_TIMER_EXPIRED    0x0008
#define DATA_ERR_CRC_ERROR                       0x0010
#define DATA_ERR_RX_BUFFER_OVERFLOW              0x0020
#define DATA_ERR_MAX_FRAME_LENGTH_EXCEEDED       0x0040
#define DATA_ERR_WRONG_SEQUENCE_NUMBER           0x0080
#define DATA_ERR_AFC_FRAME_SYNTAX_ERROR          0x0100
#define DATA_ERR_NAC_FRAME_SYNTAX_ERROR          0x0200
#define DATA_ERR_EOF_SYNTAX_ERROR                0x0400
#define DATA_ERR_FRAME_SYNTAX_ERROR              0x0800
#define DATA_ERR_BAD_CTRL_SYMBOL_TYPE            0x1000
#define DATA_ERR_PA_INIT_ERROR                   0x2000
#define DATA_ERR_PA_ERROR_IND_RECEIVED           0x4000

typedef union _UFS_HCI_UECDL {
    struct {
        ULONG ErrorCode : 15;
        ULONG Reserved : 16;
        ULONG Error : 1;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UECDL, *PUFS_HCI_UECDL;

#define NET_ERR_UNSUPPORTED_HEADER_TYPE          0x1
#define NET_ERR_BAD_DEVICEID_ENC                 0x2
#define NET_ERR_LHDR_TRAP_PACKET_DROPPING        0x4

typedef union _UFS_HCI_UECN {
    struct {
        ULONG ErrorCode : 3;
        ULONG Reserved : 28;
        ULONG Error : 1;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UECN, *PUFS_HCI_UECN;

#define TRANS_ERR_UNSUPPORTED_HEADER_TYPE        0x01
#define TRANS_ERR_UNKNOWN_CPORTID                0x02
#define TRANS_ERR_NO_CONNECTION_RX               0x04
#define TRANS_ERR_CONTROLLED_SEGMENT_DROPPING    0x08
#define TRANS_ERR_BAD_TC                         0x10
#define TRANS_ERR_E2E_CREDIT_OVERFLOW            0x20
#define TRANS_ERR_SAFETY_VALVE_DROPPING          0x40

typedef union _UFS_HCI_UECT {
    struct {
        ULONG ErrorCode : 7;
        ULONG Reserved : 24;
        ULONG Error : 1;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UECT, *PUFS_HCI_UECT;

typedef union _UFS_HCI_UECDME {
    struct {
        ULONG ErrorCode : 1;
        ULONG Reserved : 30;
        ULONG Error : 1;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UECDME, *PUFS_HCI_UECDME;

typedef union _UFS_HCI_UTRIACR {
    struct {
        ULONG TimeoutVal : 8;
        ULONG CtrThresh : 5;
        ULONG Reserved : 3;
        ULONG CtrTimerReset : 1;
        ULONG Reserved2 : 3;
        ULONG Status : 1;
        ULONG Reserved3 : 3;
        ULONG ParamWriteEnable : 1;
        ULONG Reserved4 : 6;
        ULONG Enable : 1;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTRIACR, *PUFS_HCI_UTRIACR;

//
// UTP Transfer Request Registers
//

#define BASE_ADDR_MASK           0x00000FFF
#define BASE_ADDR_ALIGNED(X)     ((X + BASE_ADDR_MASK) & ~BASE_ADDR_MASK)


typedef union _UFS_HCI_UTRLBA {
    struct {
        ULONG TransferReqListBaseAddress;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTRLBA, *PUFS_HCI_UTRLBA;

typedef union _UFS_HCI_UTRLBAU {
    struct {
        ULONG TransferReqListUpperBaseAddress;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTRLBAU, *PUFS_HCI_UTRLBAU;

typedef union _UFS_HCI_UTRLDBR {
    struct {
        ULONG TransferReqListDoorbell;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTRLDBR, *PUFS_HCI_UTRLDBR;

typedef union _UFS_HCI_UTRLCLR {
    struct {
        ULONG TransferReqListClear;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTRLCLR, *PUFS_HCI_UTRLCLR;

typedef union _UFS_HCI_UTRLRSR {
    struct {
        ULONG TransferReqListRunStop : 1;
        ULONG Reserved : 31;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTRLRSR, *PUFS_HCI_UTRLRSR;

//
// UTP Task Management Request Registers
//

typedef union _UFS_HCI_UTMRLBA {
    struct {
        ULONG TaskMgmtReqListBaseAddress;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTMRLBA, *PUFS_HCI_UTMRLBA;

typedef union _UFS_HCI_UTMRLBAU {
    struct {
        ULONG TaskMgmtListUpperBaseAddress;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTMRLBAU, *PUFS_HCI_UTMRLBAU;

typedef union _UFS_HCI_UTMRLDBR {
    struct {
        ULONG TaskMgmtReqListDoorbell : 8;
        ULONG Reserved : 24;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTMRLDBR, *PUFS_HCI_UTMRLDBR;

typedef union _UFS_HCI_UTMRLCLR {
    struct {
        ULONG TaskMgmtReqListClear : 8;
        ULONG Reserved : 24;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTMRLCLR, *PUFS_HCI_UTMRLCLR;

typedef union _UFS_HCI_UTMRLRSR {
    struct {
        ULONG TaskMgmtReqListRunStop : 1;
        ULONG Reserved : 31;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UTMRLRSR, *PUFS_HCI_UTMRLRSR;

//
// UIC Command Opcodes
//

#define UIC_DME_GET                 0x01
#define UIC_DME_SET                 0x02
#define UIC_DME_PEER_GET            0x03
#define UIC_DME_PEER_SET            0x04
#define UIC_DME_POWERON             0x10
#define UIC_DME_POWEROFF            0x11
#define UIC_DME_ENABLE              0x12
#define UIC_DME_RESET               0x14
#define UIC_DME_ENDPOINTRESET       0x15
#define UIC_DME_LINKSTARTUP         0x16
#define UIC_DME_HIBERNATE_ENTER     0x17
#define UIC_DME_HIBERNATE_EXIT      0x18
#define UIC_DME_TEST_MODE           0x1A

#define UIC_GENERIC_SUCCESS         0x0
#define UIC_GENERIC_FAILURE         0x1

//
// UIC Command Registers
//

typedef union _UFS_HCI_UICCMD {
    struct {
        ULONG Opcode : 8;
        ULONG Reserved : 24;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UICCMD, *PUFS_HCI_UICCMD;

#define UFS_UIC_ERROR_CODE_BITMASK     0x000000FF

typedef union _UFS_HCI_UICCMDARG {
    struct {
        ULONG Argument : 32;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} UFS_HCI_UICCMDARG, *PUFS_HCI_UICCMDARG;

typedef struct _UFS_HCI_CAPABILITIES {
    UFS_HCI_CAP ContCap;
    ULONG Reserved;
    UFS_HCI_VER Ver;
    ULONG Reserved2;
    UFS_HCI_HCPID ProdId;
    UFS_HCI_HCMID ManId;
    UFS_HCI_AHIT AutoH8;
    ULONG Reserved3;
} UFS_HCI_CAPABILITIES, *PUFS_HCI_CAPABILITIES;

typedef struct _UFS_HCI_RUNTIME {
    UFS_HCI_IS Is;
    UFS_HCI_IE Ie;
    ULONG Reserved[2];
    UFS_HCI_HCS Hcs;
    UFS_HCI_HCE Hce;
    UFS_HCI_UECPA PhysErr;
    UFS_HCI_UECDL DataErr;
    UFS_HCI_UECN NetErr;
    UFS_HCI_UECT TransErr;
    UFS_HCI_UECDME DmeErr;
    UFS_HCI_UTRIACR IntAgg;
} UFS_HCI_RUNTIME, *PUFS_HCI_RUNTIME;

typedef struct _UFS_HCI_TRANSFER_REQUEST {
    UFS_HCI_UTRLBA BaseAddr;
    UFS_HCI_UTRLBAU UpperBaseAddr;
    UFS_HCI_UTRLDBR Doorbell;
    UFS_HCI_UTRLCLR Clear;
    UFS_HCI_UTRLRSR RunStop;
    ULONG Reserved[3];
} UFS_HCI_TRANSFER_REQUEST, *PUFS_HCI_TRANSFER_REQUEST;

typedef struct _UFS_HCI_TASK_MGMT {
    UFS_HCI_UTMRLBA BaseAddr;
    UFS_HCI_UTMRLBAU UpperBaseAddr;
    UFS_HCI_UTMRLDBR Doorbell;
    UFS_HCI_UTMRLCLR Clear;
    UFS_HCI_UTMRLRSR RunStop;
    ULONG Reserved[3];
} UFS_HCI_TASK_MGMT, *PUFS_HCI_TASK_MGMT;

typedef struct _UFS_HCI_UIC_COMMAND {
    UFS_HCI_UICCMD Cmd;
    UFS_HCI_UICCMDARG Args[3];
    ULONG Reserved[4];
} UFS_HCI_UIC_COMMAND, *PUFS_HCI_UIC_COMMAND;

typedef struct _UFS_HCI_REG {
    UFS_HCI_CAPABILITIES Cap;
    UFS_HCI_RUNTIME Runtime;
    UFS_HCI_TRANSFER_REQUEST TransReq;
    UFS_HCI_TASK_MGMT TaskMgmt;
    UFS_HCI_UIC_COMMAND UicCmd;
    ULONG Reserved[4];
    ULONG VendorSpecific[16];
} UFS_HCI_REG, *PUFS_HCI_REG;

//
// Query Descriptors
//

#define UFS_DESC_DEVICE_IDN                0x00
#define UFS_DESC_CONFIGURATION_IDN         0x01
#define UFS_DESC_UNIT_IDN                  0x02
#define UFS_DESC_INTERCONNECT_IDN          0x04
#define UFS_DESC_STRING_IDN                0x05
#define UFS_DESC_GEOMETRY_IDN              0x07
#define UFS_DESC_POWER_IDN                 0x08

//
// The Device Descriptor
//

typedef struct _UFS_DEVICE_DESCRIPTOR {
    UCHAR bLength;  // 0x13
    UCHAR bDescriptorType; // 0x00
    UCHAR bDevice;  // 0x00 = Device
    UCHAR bDeviceClass;  // 0x00 = Mass Storage
    UCHAR bDeviceSubClass;
    UCHAR bProtocol;
    UCHAR bNumberLU;
    UCHAR bNumberWLU;
    UCHAR bBootEnable;
    UCHAR bDescrAccessEn;
    UCHAR bInitPowerMode;
    UCHAR bHighPriorityLUN;
    UCHAR bSecureRemovalType;
    UCHAR bSecurityLU;
    UCHAR bBackgroundOpsTermLat;
    UCHAR bInitActiveICCLevel;
    UCHAR wSpecVersion[2];
    UCHAR wManufactureDate[2];
    UCHAR iManufacturerName;
    UCHAR iProductName;
    UCHAR iSerialNumberID;
    UCHAR iOemID;
    UCHAR wManufacturerID[2];
    UCHAR bUD0BaseOffset;
    UCHAR bUDConfigPLength;
    UCHAR bDeviceRTTCap;
    UCHAR wPeriodicRTCUpdate[2];
    UCHAR Reserved[33];
} UFS_DEVICE_DESCRIPTOR, *PUFS_DEVICE_DESCRIPTOR;

//
// The Configuration Descriptor
//

typedef struct _UFS_UNIT_CONFIG_DESCRIPTOR {
    UCHAR bLUEnable;
    UCHAR bBootLunID;
    UCHAR bLUWriteProtect;
    UCHAR bMemoryType;
    UCHAR dNumAllocUnits[4];
    UCHAR bDataReliability;
    UCHAR bLogicalBlockSize;
    ULONG bProvisioningType;
    UCHAR wContextCapabilities[2];
    UCHAR Reserved[3];
} UFS_UNIT_CONFIG_DESCRIPTOR, *PUFS_UNIT_CONFIG_DESCRIPTOR;

typedef struct _UFS_CONFIG_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType; // 0x01
    UCHAR Reserved1;
    UCHAR bBootEnable;
    UCHAR bDescrAccessEn;
    UCHAR bInitPowerMode;
    UCHAR bHighPriorityLUN;
    UCHAR bSecureRemovalType;
    UCHAR bInitActiveICCLevel;
    UCHAR wPeriodicRTCUpdate[2];
    UCHAR Reserved2[5];
    UFS_UNIT_CONFIG_DESCRIPTOR UnitConfig[MAX_NUM_LU];
} UFS_CONFIG_DESCRIPTOR, *PUFS_CONFIG_DESCRIPTOR;

//
// The Geometry Descriptor
//

typedef struct _UFS_GEOMETRY_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;  // 0x07
    UCHAR bMediaTechnology;
    UCHAR Reserved1;
    UCHAR qTotalRawDeviceCapacity[8];
    UCHAR Reserved2;
    UCHAR dSegmentSize[4];
    UCHAR bAllocationUnitSize;
    UCHAR bMinAddrBlockSize;
    UCHAR bOptimalReadBlockSize;
    UCHAR bOptimalWriteBlockSize;
    UCHAR bMaxInBufferSize;
    UCHAR bMaxOutBufferSize;
    UCHAR bRPMB_ReadWriteSize;
    UCHAR Reserved;
    UCHAR bDataOrdering;
    UCHAR bMaxContexIDNumber;
    UCHAR bSysDataTagUnitSize;
    UCHAR bSysDataTagResSize;
    UCHAR bSupportedSecRTypes;
    UCHAR wSupportedMemoryTypes[2];
    UCHAR dSystemCodeMaxNAllocU[4];
    UCHAR wSystemCodeCapAdjFac[2];
    UCHAR dNonPersistMaxNAllocU[4];
    UCHAR wNonPersistCapAdjFac[2];
    UCHAR dEnhanced1MaxNAllocU[4];
    UCHAR wEnhanced1CapAdjFac[2];
    UCHAR dEnhanced2MaxNAllocU[4];
    UCHAR wEnhanced2CapAdjFac[2];
    UCHAR dEnhanced3MaxNAllocU[4];
    UCHAR wEnhanced3CapAdjFac[2];
    UCHAR dEnhanced4MaxNAllocU[4];
    UCHAR wEnhanced4CapAdjFac[2];
} UFS_GEOMETRY_DESCRIPTOR, *PUFS_GEOMETRY_DESCRIPTOR;

//
// The Unit Descriptor
//

typedef struct _UFS_UNIT_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bUnitIndex;
    UCHAR bLUEnable;
    UCHAR bBootLunID;
    UCHAR bLUWriteProtect;
    UCHAR bLUQueueDepth;
    UCHAR Reserved;
    UCHAR bMemoryType;
    UCHAR bDataReliability;
    UCHAR bLogicalBlockSize;
    UCHAR qLogicalBlockCount[8];
    UCHAR dEraseBlockSize[4];
    UCHAR bProvisioningType;
    UCHAR qPhyMemResourceCount[8];
    UCHAR wContextCapabilities[2];
    UCHAR bLargeUnitGranularity_M1;
} UFS_UNIT_DESCRIPTOR, *PUFS_UNIT_DESCRIPTOR;

//
// The RPMB Unit Descriptor
//

typedef struct _UFS_RPMB_UNIT_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType; //0x02
    UCHAR bUnitIndex; //0xC4
    UCHAR bLUEnable;
    UCHAR bBootLunID;
    UCHAR bLUWriteProtect;
    UCHAR bLUQueueDepth;
    UCHAR Reserved1;
    UCHAR bMemoryType;
    UCHAR Reserved2;
    UCHAR bLogicalBlockSize;
    UCHAR qLogicalBlockCount[8];
    UCHAR dEraseBlockSize[4];
    UCHAR bProvisioningType;
    UCHAR qPhyMemResourceCount[8];
    UCHAR Reserved3[3];
} UFS_RPMB_UNIT_DESCRIPTOR, *PUFS_RPMB_UNIT_DESCRIPTOR;

//
// The Power Parameters Descriptor
//

typedef struct _UFS_POWER_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType; // 0x00
    UCHAR wActiveICCLevelsVCC[32];
    UCHAR wActiveICCLevelsVCCQ[32];
    UCHAR wActiveICCLevelsVCCQ2[32];
} UFS_POWER_DESCRIPTOR, *PUFS_POWER_DESCRIPTOR;

//
// The Interconnect Descriptor
//

typedef struct _UFS_INTERCONNECT_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;  // 0x04
    UCHAR bcdUniproVersion[2];
    UCHAR bcdMphyVersion[2];
} UFS_INTERCONNECT_DESCRIPTOR, *PUFS_INTERCONNECT_DESCRIPTOR;

//
// The String Descriptor
//
// This includes the Manufacturer Name, Product Name, OEM ID, and Serial Number
//

#define MAX_UNICODE_STRING_LEN                 127

typedef struct _UFS_STRING_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType; //0x05
    WCHAR String[MAX_UNICODE_STRING_LEN];
} UFS_STRING_DESCRIPTOR, *PUFS_STRING_DESCRIPTOR;

//
// The Flags Descriptor
//

typedef enum {
    Reserved1 = 0,
    fDeviceInit,
    fPermanentWPEn,
    fPowerOnWPEn,
    fBackgroundOpsEn,
    Reserved2,
    fPurgeEnable,
    Reserved3,
    fPhyResourceRemoval,
    fBusyRTC,
    Reserved4,
    fPermanentlyDisableFwUpdate
} UFS_FLAGS_DESCRIPTOR;

//
// The Attributes Descriptor
//

typedef enum {
    bBootLunEn = 0,
    Reserved,
    bCurrentPowerMode,
    bActiveICCLevel,
    bOutOfOrderDataEn,
    bBackgroundOpStatus,
    bPurgeStatus,
    bMaxDataInSize,
    bMaxDataOutSize,
    dDynCapNeeded, // ULONG
    bRefClkFreq,
    bConfigDescrLock,
    bMaxNumOfRTT,
    wExceptionEventControl, // USHORT
    wExceptionEventStatus, // USHORT
    dSecondsPassed, // ULONG
    wContextConf, // USHORT
    dCorrPrgBlkNum, // ULONG
} UFS_ATTRIBUTES_DESCRIPTOR;