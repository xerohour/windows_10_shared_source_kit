/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ufs.h

Abstract:

    Structure definitions needed for UFS I/O routines.

Author:

    Erik Schmidt (eriksch) - October 2012

Environment:



Revision History:

--*/

#pragma once

//
// UFS Versions
//

#define UFS_VERSION_1_0                         0x00010000

//
// Other UFS Macros
//

#define DWORD_ALIGNED(X)                        ((X + 0x7) & 0xFFFFFFF8)

#define REPORT_LUNS_RESTRICTED                  0x00
#define REPORT_LUNS_WELL_KNOWN                  0x01
#define REPORT_LUNS_ALL                         0x02

#define PCI_CONFIG_LEN                          0x30
#define NUM_ACCESS_RANGES                       6

#define INVALID_LUN                             0xFFFFFFFF
#define INVALID_INDEX                           0xFFFFFFFF

#define UFS_COMMAND_TIMEOUT                     2000
#define MAX_NUM_STARTUP_ATTEMPTS                5
#define MAX_NUM_QUERY_ATTEMPTS                  100

//
// Well Defined Lun IDs
//

#define UFS_DATA_LUN_ID                         0x00
#define UFS_POWER_LUN_ID                        0xD0
#define UFS_REPORT_LUNS_ID                      0x81
#define UFS_RPMB_LUN_ID                         0xC4
#define UFS_BOOT_LUN_ID                         0xB0

//
// RPMB Macros
//

#define UFS_RPMB_SECURITY_PROTOCOL              0xEC
#define UFS_RPMB_SECURITY_PROTOCOL_SPECIFIER    0x0001

//
// Handy Macros
//

#define MIN(x,y) ((ULONG)(x) > (ULONG)(y) ? (ULONG)(y) : (ULONG)(x))        // return minimum among x & y
#define MAX(x,y) ((ULONG)(x) > (ULONG)(y) ? (ULONG)(x) : (ULONG)(y))        // return maximum among x & y

#define MS_TO_100NS(ms) ((ULONGLONG)10000*(ms)) // Converts milliseconds to 100ns.
#define MS_TO_US(ms) ((ULONGLONG)1000*(ms))     // Converts milliseconds to microseconds.
#define US_TO_100NS(us) ((ULONGLONG)10*(us))    // Converts microseconds to 100ns.

//
// Feature/Hack Flag Values
//

#define UFS_FEATURE_DISABLE_H8                  0x00000001
#define UFS_FEATURE_ENABLE_SW_H8                0x00000002







//
// Registry Key Strings
//

#define STORUFS_REG_FEATURE_FLAGS               "FeatureFlags"
#define STORUFS_REG_FEATURE_FLAGS_DEFAULT       0

#define STORUFS_REG_HIGH_SPEED_GEAR             "HighSpeedGearMode"
#define STORUFS_REG_HIGH_SPEED_GEAR_DEFAULT     0

#define STORUFS_REG_NUM_LANES                   "NumLanes"
#define STORUFS_REG_NUM_LANES_DEFAULT           0

#define STORUFS_REG_HSSERIES                    "HSSeries"
#define STORUFS_REG_HSSERIES_DEFAULT            0

//
// UFS Structures
//

typedef struct _UFS_REG_VALUES {
    ULONG HighSpeedGearMode;
    ULONG NumLanes;
    UFS_HSRATE_SERIES HSSeries;
} UFS_REG_VALUES, *PUFS_REG_VALUES;

typedef enum _UFS_DEBUG_LEVEL {
    UfsDebugError = 0,
    UfsDebugWarning,
    UfsDebugTrace,
    UfsDebugInfo
} UFS_DEBUG_LEVEL;

typedef struct _UFS_TASKMAN_REQUEST_LIST {
    PUFS_HCI_TMRD List;
    STOR_PHYSICAL_ADDRESS PhysicalAddr;

    //
    // Bit Vectors that are associated with the status of pending commands
    //

    ULONG BuildingRequests;
    ULONG ExecutingRequests;
    ULONG ProcessingRequests;
    ULONG ListSize;

    KSPIN_LOCK RequestsLock;
    STOR_DPC CompletionDpc[MAX_TASK_MAN_LIST_SIZE];
    PSCSI_REQUEST_BLOCK ActiveSrb[MAX_TASK_MAN_LIST_SIZE];

} UFS_TASKMAN_REQUEST_LIST, *PUFS_TASKMAN_REQUEST_LIST;

typedef struct _UFS_UCD_ADDRESS {
    STOR_PHYSICAL_ADDRESS PhysicalAddress;
    PVOID VirtualAddress;
} UFS_UCD_ADDRESS, *PUFS_UCD_ADDRESS;

typedef struct _UFS_TRANSFER_REQUEST_LIST {
    PUFS_HCI_TRD List;
    STOR_PHYSICAL_ADDRESS PhysicalAddr;
    UFS_UCD_ADDRESS UcdList[MAX_REQUEST_LIST_SIZE];

    //
    // Bit Vectors that are associated with the status of pending commands
    //

    ULONG BuildingRequests;
    ULONG ExecutingRequests;
    ULONG ExecutingSyncRequests;
    ULONG ProcessingRequests;

    KSPIN_LOCK RequestsLock;
    STOR_DPC CompletionDpc[MAX_REQUEST_LIST_SIZE];
    ULONG ListSize;
    PSCSI_REQUEST_BLOCK ActiveSrb[MAX_REQUEST_LIST_SIZE];

} UFS_TRANSFER_REQUEST_LIST, *PUFS_TRANSFER_REQUEST_LIST;

typedef enum {
    UfsStandardMode = 0,
    UfsBusResetMode
} UFS_RESET_STATE;

typedef struct _UFS_ADAPTER_EXTENSION {
    PUFS_HCI_REG UfsRegisters;

    //
    // Allocated List Structures that the Host Controller References. These
    // data structures must be freed.
    //

    UFS_TASKMAN_REQUEST_LIST Tmrl;
    UFS_TRANSFER_REQUEST_LIST Trl;

    PUCHAR UncachedExtension;

    //
    // Keeps track of whether we are in dump mode or not
    //

    UCHAR DumpMode;

    //
    // This lock protected variable keeps track of whether there is an active
    // UIC Command, and whether you can edit the data in the UIC registers.
    //

    volatile LONG ActiveUicCmd;
    STOR_DPC UicCompletionDpc;
    BOOLEAN UicSync;

    //
    // Variables needed for the UTP Interrupt Aggregation
    //

    ULONG TimeoutVal;
    ULONG CtrThresh;

    //
    // Bus reset tracking variable and DPC pointer
    //

    volatile LONG BusReset;
    STOR_DPC ResetDpc;

    //
    // Track the current power state
    //

    UFS_POWER Power;

    //
    // Cache device/ adapter properties
    //

    PSTOR_ADDR_BTL8 DeviceAddress;

    ULONGLONG UfsBar;
    USHORT PciVendorID;
    USHORT PciDeviceID;
    UCHAR PciRevisionID;

    UFS_HCI_CAP Capabilities;
    UFS_HCI_VER Version;
    UFS_HCI_HCPID ProductID;
    UFS_HCI_HCMID ManufacturerID;
    UFS_HCI_AHIT AutoH8Settings;
    ULONG FeatureFlags;
    UFS_REG_VALUES RegValues;
    BOOLEAN AutoH8Enabled;

    //
    // PHY Capabilities and state
    //

    UFS_MPHY_CONFIG MPHYCapabilities;
    UFS_MPHY_CONFIG MPHYState;

    //
    // RPMB variables
    //

    BOOLEAN RpmbSupported;
    ULONG MaxRpmbWriteSize;
    ULONG RpmbSize;











#if USE_DBGLOG
    UCHAR DbgLogEntry[UFS_DBGLOG_SIZE];
#endif
    UFS_DBGLOG DbgLog;

    struct {
        ULONG numErrors;
    } Dbg;

} UFS_ADAPTER_EXTENSION, *PUFS_ADAPTER_EXTENSION;

typedef struct _UFS_DUMP_CONTEXT {
    USHORT VendorID;
    USHORT DeviceID;
    UCHAR RevisionID;
    UCHAR Reserved[3];

    ULONGLONG UfsBar;

    PUFS_HCI_TRD Trd;
    PUFS_HCI_TMRD Tmrd;
    UFS_UCD_ADDRESS UcdList[MAX_REQUEST_LIST_SIZE];

    STOR_PHYSICAL_ADDRESS TmrlAddr;
    STOR_PHYSICAL_ADDRESS TrlAddr;

    ULONG FeatureFlags;
    UFS_REG_VALUES RegValues;
} UFS_DUMP_CONTEXT, *PUFS_DUMP_CONTEXT;

typedef struct _UFS_SRB_EXTENSION {
    ULONG Tag;
} UFS_SRB_EXTENSION, *PUFS_SRB_EXTENSION;

typedef struct _UFS_LUN_EXTENSION {
    ULONG Tag;
} UFS_LUN_EXTENSION, *PUFS_LUN_EXTENSION;

//
// Function Prototypes
//

sp_DRIVER_INITIALIZE DriverEntry;
HW_FIND_ADAPTER UfsFindAdapter;
HW_INITIALIZE UfsInitialize;
HW_PASSIVE_INITIALIZE_ROUTINE UfsPassiveInitialize;
HW_RESET_BUS UfsResetBus;
HW_INTERRUPT UfsInterrupt;
HW_ADAPTER_CONTROL UfsAdapterControl;
HW_UNIT_CONTROL UfsUnitControl;

VOID
UfsRegistryReadControllerSettings(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsGetControllerCapabilities(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsGetRPMBCapabilities(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

NTSTATUS
UfsAllocateRequestLists(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PPORT_CONFIGURATION_INFORMATION ConfigInfo
    );

VOID
UfsInitializeRequestLists(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsInitializeDpcs(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

HW_DPC_ROUTINE UfsResetBusDpc;
HW_DPC_ROUTINE UfsCompleteTaskManDpc;
HW_DPC_ROUTINE UfsCompleteTransferDpc;
HW_DPC_ROUTINE UfsCompleteUicCommandDpc;

BOOLEAN
UfsHandleBusError(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PUFS_HCI_IS Is
    );

VOID
UfsClearOutstandingRequests(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Lun
    );

BOOLEAN
UfsInternalResetBus (
    PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsCompleteTaskManRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Index
    );

VOID
UfsCompleteTransferRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Index,
    _In_ UFS_HCI_IS Is
    );

VOID
UfsHostControllerEnable(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

NTSTATUS
UfsStartAdapter(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsStopAdapter(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );

VOID
UfsFreeResources (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    );
