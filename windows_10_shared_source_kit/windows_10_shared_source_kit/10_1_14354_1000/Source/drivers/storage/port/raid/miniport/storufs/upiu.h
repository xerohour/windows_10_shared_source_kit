/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    upiu.h

Abstract:

    Contains the UFS Protocol Information Unit data structures and field definitions.

Author:

    Erik Schmidt (eriksch) - October 2012

Environment:



Revision History:

--*/

#pragma once

#pragma warning(disable:4214)   // bit field types other than int
#pragma warning(disable:4201)   // nameless struct/union

//
// UPIU Transaction Opcodes
//

#define UPIU_NOP_OUT                     0x00
#define UPIU_COMMAND                     0x01
#define UPIU_DATA_OUT                    0x02
#define UPIU_TASK_MAN_REQ                0x04
#define UPIU_QUERY_REQ                   0x16
#define UPIU_NOP_IN                      0x20
#define UPIU_RESPONSE                    0x21
#define UPIU_DATA_IN                     0x22
#define UPIU_TASK_MAN_RESP               0x24
#define UPIU_RTT                         0x31
#define UPIU_QUERY_RESP                  0x36

//
// UPIU Transaction Bit Masks
//

#define UPIU_HD_MASK                     0x80
#define UPIU_DD_MASK                     0x40
#define UPIU_OPCODE_MASK                 0x3F

//
// UPIU Command Set Type Codes
//

#define UPIU_CMD_SET_SCSI                0x0
#define UPIU_CMD_SET_UFS                 0x1

//
// UPIU Header Flags
//

#define UPIU_HEADER_FLAGS_F_MASK         0x80
#define UPIU_HEADER_FLAGS_R_MASK         0x40
#define UPIU_HEADER_FLAGS_W_MASK         0x20
#define UPIU_HEADER_FLAGS_ATTR_MASK      0x03

#define UPIU_HEADER_FLAGS_ATTR_SIMPLE    0x0
#define UPIU_HEADER_FLAGS_ATTR_ORDERED   0x1
#define UPIU_HEADER_FLAGS_ATTR_HEAD      0x2

typedef struct _UFS_UPIU_HEADER {
    UCHAR TransactionType;
    UCHAR Flags;
    UCHAR LUN;
    UCHAR TaskTag;

    UCHAR Reserved : 4;
    UCHAR CmdSetType : 4;
    UCHAR QueryFunction;
    UCHAR Response;
    UCHAR Status;

    UCHAR EhsLength;
    UCHAR DevInfo;
    USHORT DataSegmentLength;
} UFS_UPIU_HEADER, *PUFS_UPIU_HEADER;

//
// Task Management Function and Response Codes
//

#define UPIU_TM_FUNCTION_ABORT           0x01
#define UPIU_TM_FUNCTION_ABORT_SET       0x02
#define UPIU_TM_FUNCTION_CLEAR_SET       0x04
#define UPIU_TM_FUNCTION_LU_RESET        0x08
#define UPIU_TM_FUNCTION_QUERY           0x80
#define UPIU_TM_FUNCTION_QUERY_SET       0x81

#define UPIU_TM_RESPONSE_COMPLETE        0x00
#define UPIU_TM_RESPONSE_NOT_SUPPORTED   0x04
#define UPIU_TM_RESPONSE_FAILED          0x05
#define UPIU_TM_RESPONSE_SUCCEEDED       0x08
#define UPIU_TM_RESPONSE_INCORRECT_LUN   0x09

//
// Task Management UPIUs - relevant for both the Request and Response
//

typedef struct _UFS_UPIU_TASK_MAN {
    UFS_UPIU_HEADER Header;
    ULONG Parameters[3];
    ULONG Reserved[2];
} UFS_UPIU_TASK_MAN, *PUFS_UPIU_TASK_MAN;

//
// Command UPIU Data Structure
//

typedef struct _UFS_UPIU_COMMAND {
    UFS_UPIU_HEADER Header;
    ULONG ExpectedDataLength;
    UCHAR Cdb[16];
} UFS_UPIU_COMMAND, *PUFS_UPIU_COMMAND;

#define SIZEOF_UFS_UPIU_COMMAND      DWORD_ALIGNED(sizeof(UFS_UPIU_COMMAND))

//
// Response UPIU Data Structure
//

#define UFS_UPIU_TARGET_SUCCESS                     0x0
#define UFS_UPIU_TARGET_FAILURE                     0x1

#define SCSISTAT_TASK_ABORTED                       0x40

typedef struct _UFS_UPIU_RESPONSE {
    UFS_UPIU_HEADER Header;
    ULONG ResidualTransferCount;
    ULONG Reserved[4];
    USHORT SenseDataLength;
    UCHAR SenseData[18];
} UFS_UPIU_RESPONSE, *PUFS_UPIU_RESPONSE;

#define SIZEOF_UFS_UPIU_RESPONSE     DWORD_ALIGNED(sizeof(UFS_UPIU_RESPONSE))

#define UFS_UPIU_QUERY_FUNCTION_READ         0x01
#define UFS_UPIU_QUERY_FUNCTION_WRITE        0x81

#define UFS_UPIU_QUERY_OPCODE_NOP            0x00
#define UFS_UPIU_QUERY_OPCODE_READ_DESC      0x01
#define UFS_UPIU_QUERY_OPCODE_WRITE_DESC     0x02
#define UFS_UPIU_QUERY_OPCODE_READ_ATTR      0x03
#define UFS_UPIU_QUERY_OPCODE_WRITE_ATTR     0x04
#define UFS_UPIU_QUERY_OPCODE_READ_FLAG      0x05
#define UFS_UPIU_QUERY_OPCODE_SET_FLAG       0x06
#define UFS_UPIU_QUERY_OPCODE_CLEAR_FLAG     0x07
#define UFS_UPIU_QUERY_OPCODE_TOGGLE_FLAG    0x08

//
// The Opcode for the Query Request
//

typedef union _UFS_QUERY_OPCODE {
    struct {
        UCHAR Opcode;
        UCHAR DescriptorIdn;
        UCHAR Index;
        UCHAR Selector;
        UCHAR Reserved1[2];
        USHORT Length;
        UCHAR Reserved2[8];
    } DescriptorOpcode;

    struct {
        UCHAR Opcode;
        UCHAR AttributeIdn;
        UCHAR Index;
        UCHAR Selector;
        UCHAR Reserved1[4];
        UCHAR Value[4];
        UCHAR Reserved2[4];
    } AttributeOpcode;

    struct {
        UCHAR Opcode;
        UCHAR FlagIdn;
        UCHAR Index;
        UCHAR Selector;
        UCHAR Reserved1[7];
        UCHAR Value;
        UCHAR Reserved2[4];
    } FlagOpcode;

    struct {
        UCHAR Opcode;
        UCHAR Idn;
        UCHAR Index;
        UCHAR Selector;
        UCHAR Data[12];
    } GenericOpcode;

} UFS_QUERY_OPCODE, *PUFS_QUERY_OPCODE;

typedef struct _UFS_UPIU_QUERY {
    UFS_UPIU_HEADER Header;

    UFS_QUERY_OPCODE Opcode;

    ULONG Reserved;
} UFS_UPIU_QUERY, *PUFS_UPIU_QUERY;

//
// Physcial Region Description Table
//

#define PRDT_BASE_ADDR_MASK              0x00000003

typedef struct _UFS_UPIU_PRDT {
    ULONG BaseAddr;
    ULONG BaseAddrUpper;
    ULONG Reserved2;
    ULONG DataByteCount : 18;
    ULONG Reserved3 : 14;
} UFS_UPIU_PRDT, *PUFS_UPIU_PRDT;

#define SIZEOF_UFS_UPIU_PRDT             sizeof(UFS_UPIU_PRDT)

#define MAX_SIZEOF_UFS_UPIU_UCD          (SIZEOF_UFS_UPIU_COMMAND + \
                                          SIZEOF_UFS_UPIU_RESPONSE + \
                                          (SIZEOF_UFS_UPIU_PRDT * \
                                           MAX_NUM_SCATTER_GATHER))

//
// UTP Transfer Request Descriptor
//

#define TRD_CT_UFS_STORAGE                           0x1

#define TRD_DD_NODATA                                0x0
#define TRD_DD_SYS2TARGET                            0x1
#define TRD_DD_TARGET2SYS                            0x2

#define TRD_OCS_SUCCESS                              0x0
#define TRD_OCS_INVALID_COMMAND_TABLE_ATTRIBUTES     0x1
#define TRD_OCS_INVALID_PRDT_ATTRIBUTES              0x2
#define TRD_OCS_MISMATCH_DATA_BUFFER_SIZE            0x3
#define TRD_OCS_MISMATCH_RESPONSE_UPIU_SIZE          0x4
#define TRD_OCS_COMMUNICATION_FAILURE                0x5
#define TRD_OCS_ABORTED                              0x6
#define TRD_OCS_FATAL_ERROR                          0x7
#define TRD_OCS_INVALID_OCS_VALUE                    0xF

#define CMD_BASE_ADDR_MASK                           0x000000FF
#define CMD_BASE_ADDR_ALIGNED(X)                     ((X + CMD_BASE_ADDR_MASK) & ~CMD_BASE_ADDR_MASK)

typedef struct _UFS_HCI_TRD {
   ULONG Reserved : 24;
   ULONG Interrupt : 1;
   ULONG DataDirection : 2;
   ULONG Reserved2 : 1;
   ULONG CmdType : 4;

   ULONG Reserved3;

   ULONG OverallCmdStatus : 8;
   ULONG Reserved4 : 24;

   ULONG Reserved5;

   ULONG CmdDescriptorBaseAddr;

   ULONG CmdDescriptorBaseAddrUpper32;

   ULONG RespUPIULen : 16;
   ULONG RespUPIUOff : 16;

   ULONG PRDTLen : 16;
   ULONG PRDTOff : 16;
} UFS_HCI_TRD, *PUFS_HCI_TRD;

//
// Task Management Request List Data Structures
//

#define TMRD_OCS_SUCCESS                                        0x0
#define TMRD_OCS_INVALID_TASK_MANAGEMENT_FUNCTION_ATTRIBUTES    0x1
#define TMRD_OCS_MISMATCH_TASK_MANAGEMENT_REQUEST_SIZE          0x2
#define TMRD_OCS_MISMATCH_TASK_MANAGEMENT_RESPONSE_SIZE         0x3
#define TMRD_OCS_PEER_COMMUNICATION_FAILURE                     0x4
#define TMRD_OCS_ABORTED                                        0x5
#define TMRD_OCS_FATAL_ERROR                                    0x6
#define TMRD_OCS_INVALID_OCS_VALUE                              0xF

typedef struct _UFS_HCI_TMRD {
   ULONG Reserved : 24;
   ULONG Interrupt : 1;
   ULONG Reserved2 : 7;

   ULONG Reserved3;

   ULONG OverallCmdStatus : 8;
   ULONG Reserved4 : 24;

   ULONG Reserved5;

   UFS_UPIU_TASK_MAN Request;

   UFS_UPIU_TASK_MAN Response;
} UFS_HCI_TMRD, *PUFS_HCI_TMRD;

//
// Start Stop Unit CDB for Power Management
//

#define POWER_CONDITION_ACTIVE         1
#define POWER_CONDITION_SLEEP          2
#define POWER_CONDITION_POWERDOWN      3

typedef union _SCSI_START_STOP_UNIT_CDB {
    struct {
        UCHAR Opcode;

        UCHAR Immed : 1;           // 0 to be notified upon completion, 1 to be notified upon receipt
        UCHAR Reserved : 7;

        UCHAR Reserved2[2];

        UCHAR Start : 1;           // 0 to transition to stop, 1 to transition to active
        UCHAR Reserved3 : 1;
        UCHAR NoFlush : 1;         // Dynamic data should be flushed to non-volatile storage
        UCHAR Reserved4 : 1;
        UCHAR PowerCondition : 4;  // Target Power State Value

        UCHAR Control;
    } DUMMYSTRUCTNAME;

    ULONG AsUlong;
} SCSI_START_STOP_UNIT_CDB, *PSCSI_START_STOP_UNIT_CDB;

typedef struct _SCSI_LUN_RECORD {
    UCHAR Reserved1;
    UCHAR Lun;
    UCHAR Reserved2[6];
} SCSI_LUN_RECORD, *PSCSI_LUN_RECORD;

typedef struct _SCSI_REPORT_LUNS_DATA {
    ULONG ListLength;
    ULONG Reserved;

    SCSI_LUN_RECORD Record[MAX_NUM_LU];
} SCSI_REPORT_LUNS_DATA, *PSCSI_REPORT_LUNS_DATA;

//
// Function Prototypes
//

HW_BUILDIO UfsBuildIo;
HW_STARTIO UfsStartIo;

BOOLEAN
UfsPreProcessSrb(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PSCSI_REQUEST_BLOCK Srb
    );

VOID
UfsPostProcessSrb(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PSCSI_REQUEST_BLOCK Srb,
    _In_ ULONG Index
    );

UCHAR
UfsSendTaskManRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG TaskTag,
    _In_ ULONG Lun,
    _In_ ULONG Function,
    _In_ PSCSI_REQUEST_BLOCK Srb
    );

UCHAR
UfsSendTransferRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PSCSI_REQUEST_BLOCK Srb
    );

NTSTATUS
UfsSendStartStopUnitTransferRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG TargetState
    );

ULONG
UfsSendQueryUpiu(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ UCHAR Function,
    _In_ PUFS_QUERY_OPCODE Opcode,
    _In_ ULONG DataLen,
    _Inout_updates_opt_(DataLen) PVOID Data
    );

VOID
UfsSendNopOutUpiu(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

NTSTATUS
UfsSendUicCommand(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Opcode,
    _In_reads_(3) PULONG Parameters,
    _In_ BOOLEAN Synchronous
    );

ULONG
UfsSendUicDmeSet (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG MIBAttribute,
    _In_ ULONG GenSelectorIndex,
    _In_ ULONG Value
    );

ULONG
UfsSendUicDmeGet (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG MIBAttribute,
    _In_ ULONG GenSelectorIndex,
    _Out_ PULONG Value
    );

ULONG
UfsSendUicDmePeerSet (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG MIBAttribute,
    _In_ ULONG GenSelectorIndex,
    _In_ ULONG Value
    );

NTSTATUS
UfsSendUicDmeHibernate (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG HibernateOpcode,
    _Out_ PULONG ErrorCode
    );

NTSTATUS
UfsExecuteUicCommandSynchronous (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG UicOpcode,
    _In_reads_(3) PULONG Args,
    _Out_ PULONG ErrorCode
    );

UFS_HCI_IS
UfsPollForCompletion(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _Inout_opt_ PULONG TrlResources
    );

VOID
UfsCreatePrdt(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PUFS_UPIU_PRDT Prdt,
    _In_ PSTOR_SCATTER_GATHER_LIST ScatterGather
    );