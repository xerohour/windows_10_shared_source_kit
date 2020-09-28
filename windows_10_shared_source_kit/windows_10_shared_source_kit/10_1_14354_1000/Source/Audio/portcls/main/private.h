/*****************************************************************************
 * private.h - WDM Audio class driver
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#ifndef _PORTCLS_PRIVATE_H_
#define _PORTCLS_PRIVATE_H_


#include "portclsp.h"
#include "StreamRes.h"
#include "dmusicks.h"
#include "stdunk.h"
#include "validate.h"

#ifndef PC_KDEXT
#if (DBG)
#define STR_MODULENAME  "PortCls: "
#define DEBUG_VARIABLE PORTCLSDebug
#endif
#endif  // PC_KDEXT

#include <ksdebug.h>
#include <wchar.h>
#include <limits.h>
#include <lpe.h>
#include <ntintsafe.h>

#define PORTCLS_DEVICE_EXTENSION_SIGNATURE  0x000BABEE

#ifndef DEBUGLVL_LIFETIME
#define DEBUGLVL_LIFETIME DEBUGLVL_VERBOSE
#endif

//
// Default Power Management Settings
//  (drivers can override with INF settings to improve power performance)
// 
#define DEFAULT_CONSERVATION_IDLE_TIME      0       // idle timeout disabled in conservation mode
#define DEFAULT_PERFORMANCE_IDLE_TIME       0       // idle timeout disabled in performance mode
#define DEFAULT_IDLE_DEVICE_POWER_STATE     PowerDeviceD3
#define DEFAULT_IDLE_ENABLED_STATE          TRUE    // idle power management is opt-out (enabled by default)
#define DEFAULT_IMMEDIATE_IDLE_STATE        FALSE   // immediate idle is off by default

#define DEFAULT_SINGLE_COMPONENT_MULTI_FX_STATES FALSE   // Single-component multi-Fx states is off by default.

// Used to convert from seconds to hundreds of nanoseconds.
#define HNS_PER_SEC                         10000000

// Due to internal calculations for tolerance on this idle value
// the max is actually ULONG_MAX * 100000.  Note, the max possible
// value will become a special value soon.
#define IDLE_TIMEOUT_MAX_HNS                0x0001869FFFFE7960 

// When the system has gone into low power epoch, we don't want to 
// idle for too long before powering down.
#define IDLE_TIMEOUT_LOW_POWER_EPOCH_HNS    10000000

// Macro to convert portcls idle timeout values which are in seconds with 0 meaning no idle to 
// runtime power framework idle timeout values which are in hundreds of nanoseconds with 0 
// meaning timeout immediately.
#define PORTCLS_IDLE_TO_RUNTIME_IDLE(pcIdle) (((pcIdle == 0) || ((ULONGLONG)pcIdle * HNS_PER_SEC > IDLE_TIMEOUT_MAX_HNS)) ? IDLE_TIMEOUT_MAX_HNS : ((ULONGLONG)pcIdle * HNS_PER_SEC))

//
// Definitions for runtime framework registration
//
#define PORTCLS_POFX_NUM_COMPONENTS 1   // We only support a single component
#define PORTCLS_POFX_NUM_F_STATES   1   // We only support a single idle state (F0)

//-------------------------------------------------------------------------
// Description:
//
// If the condition evaluates to TRUE, perform the given statement
// then jump to the given label.
//
// Parameters:
//
//      condition - [in] Code that fits in if statement
//      action - [in] action to perform in body of if statement
//      label - [in] label to jump if condition is met
//
#define IF_TRUE_ACTION_JUMP(condition, action, label)           \
    if(condition)                                               \
    {                                                           \
        action;                                                 \
        goto label;                                             \
    }
/*****************************************************************************
 * PHYSICALCONNECTION
 *****************************************************************************
 * List entry for list of physical connections.
 */
typedef struct
{
    LIST_ENTRY      ListEntry;      // Must be first.
    PSUBDEVICE      FromSubdevice;
    PUNICODE_STRING FromString;
    ULONG           FromPin;
    PSUBDEVICE      ToSubdevice;
    PUNICODE_STRING ToString;
    ULONG           ToPin;
}
PHYSICALCONNECTION, *PPHYSICALCONNECTION;

/*****************************************************************************
 * DEVICEINTERFACE
 *****************************************************************************
 * List entry for list of physical connections.
 */
typedef struct
{
    LIST_ENTRY      ListEntry;      // Must be first.
    GUID            Interface;
    UNICODE_STRING  SymbolicLinkName;
    PSUBDEVICE      Subdevice;
}
DEVICEINTERFACE, *PDEVICEINTERFACE;

/*****************************************************************************
 * TIMEOUTCALLBACK
 *****************************************************************************
 * List entry for list of IoTimeout clients.
 */
typedef struct
{
    LIST_ENTRY          ListEntry;
    PIO_TIMER_ROUTINE   TimerRoutine;
    PVOID               Context;
} TIMEOUTCALLBACK,*PTIMEOUTCALLBACK;

/*****************************************************************************
 * POWER_REQUEST_CONTEXT
 *****************************************************************************
 * Context for PcRequestNewPowerState completions
 */
typedef struct
{
    KEVENT      SyncEvent;
    NTSTATUS    Status;
} POWER_REQUEST_CONTEXT,*PPOWER_REQUEST_CONTEXT;

/*****************************************************************************
 * POWER_DEVICE_CONTEXT
 *****************************************************************************
 * Context for PoRequestPowerIrp completions from DispatchPower
 */
typedef struct
{
    PDEVICE_CONTEXT pDeviceContext;
    PIRP            pSystemIrp;
} POWER_DEVICE_CONTEXT,*PPOWER_DEVICE_CONTEXT;

/*****************************************************************************
 * IResourceListInit
 *****************************************************************************
 * Initialization interface for list of resources.
 */
DECLARE_INTERFACE_(IResourceListInit,IResourceList)
{
    DEFINE_ABSTRACT_UNKNOWN()   // For IUnknown

    // For IResourceList
    STDMETHOD_(ULONG,NumberOfEntries)
    (   THIS
    )   PURE;

    STDMETHOD_(ULONG,NumberOfEntriesOfType)
    (   THIS_
        IN      CM_RESOURCE_TYPE    Type
    )   PURE;

    STDMETHOD_(PCM_PARTIAL_RESOURCE_DESCRIPTOR,FindTranslatedEntry)
    (   THIS_
        IN      CM_RESOURCE_TYPE    Type,
        IN      ULONG               Index
    )   PURE;

    STDMETHOD_(PCM_PARTIAL_RESOURCE_DESCRIPTOR,FindUntranslatedEntry)
    (   THIS_
        IN      CM_RESOURCE_TYPE    Type,
        IN      ULONG               Index
    )   PURE;

    STDMETHOD_(NTSTATUS,AddEntry)
    (   THIS_
        IN      PCM_PARTIAL_RESOURCE_DESCRIPTOR Translated,
        IN      PCM_PARTIAL_RESOURCE_DESCRIPTOR Untranslated
    )   PURE;

    STDMETHOD_(NTSTATUS,AddEntryFromParent)
    (   THIS_
        IN      struct IResourceList *  Parent,
        IN      CM_RESOURCE_TYPE        Type,
        IN      ULONG                   Index
    )   PURE;

    STDMETHOD_(PCM_RESOURCE_LIST,TranslatedList)
    (   THIS
    )   PURE;

    STDMETHOD_(PCM_RESOURCE_LIST,UntranslatedList)
    (   THIS
    )   PURE;

    // For IResourceListInit
    STDMETHOD_(NTSTATUS,Init)
    (   THIS_
        IN      PCM_RESOURCE_LIST   TranslatedResources,
        IN      PCM_RESOURCE_LIST   UntranslatedResources,
        IN      POOL_TYPE           PoolType
    )   PURE;

    STDMETHOD_(NTSTATUS,InitFromParent)
    (   THIS_
        IN      PRESOURCELIST       ParentList,
        IN      ULONG               MaximumEntries,
        IN      POOL_TYPE           PoolType
    )   PURE;
};

typedef IResourceListInit *PRESOURCELISTINIT;

/*****************************************************************************
 * CResourceList
 *****************************************************************************
 * Resource list implementation.
 */
class CResourceList
:   public IResourceListInit,
    public CUnknown
{
private:
    PCM_RESOURCE_LIST   Untranslated;
    PCM_RESOURCE_LIST   Translated;
    ULONG               EntriesAllocated;
    ULONG               EntriesInUse;

public:
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CResourceList);
    ~CResourceList();

    /*************************************************************************
     * IResourceListInit methods
     */
    STDMETHODIMP_(NTSTATUS) Init
    (
        IN      PCM_RESOURCE_LIST   TranslatedResources,
        IN      PCM_RESOURCE_LIST   UntranslatedResources,
        IN      POOL_TYPE           PoolType
    );
    STDMETHODIMP_(NTSTATUS) InitFromParent
    (
        IN      PRESOURCELIST       ParentList,
        IN      ULONG               MaximumEntries,
        IN      POOL_TYPE           PoolType
    );

    /*************************************************************************
     * IResourceList methods
     */
    STDMETHODIMP_(ULONG) NumberOfEntries
    (   void
    );
    STDMETHODIMP_(ULONG) NumberOfEntriesOfType
    (
        IN      CM_RESOURCE_TYPE    Type
    );
    STDMETHODIMP_(PCM_PARTIAL_RESOURCE_DESCRIPTOR) FindTranslatedEntry
    (
        IN      CM_RESOURCE_TYPE    Type,
        IN      ULONG               Index
    );
    STDMETHODIMP_(PCM_PARTIAL_RESOURCE_DESCRIPTOR) FindUntranslatedEntry
    (
        IN      CM_RESOURCE_TYPE    Type,
        IN      ULONG               Index
    );
    STDMETHODIMP_(NTSTATUS) AddEntry
    (
        IN      PCM_PARTIAL_RESOURCE_DESCRIPTOR Translated,
        IN      PCM_PARTIAL_RESOURCE_DESCRIPTOR Untranslated
    );
    STDMETHODIMP_(NTSTATUS) AddEntryFromParent
    (
        IN      PRESOURCELIST       Parent,
        IN      CM_RESOURCE_TYPE    Type,
        IN      ULONG               Index
    );
    STDMETHODIMP_(PCM_RESOURCE_LIST) TranslatedList
    (   void
    );
    STDMETHODIMP_(PCM_RESOURCE_LIST) UntranslatedList
    (   void
    );
};

/*****************************************************************************
 * IRegistryKeyInit
 *****************************************************************************
 * Interface for registry key with Init.
 */
DECLARE_INTERFACE_(IRegistryKeyInit,IRegistryKey)
{
    DEFINE_ABSTRACT_UNKNOWN()   // For IUnknown

    // For IRegistryKey
    STDMETHOD_(NTSTATUS,QueryKey)
    (   THIS_
        IN      KEY_INFORMATION_CLASS   KeyInformationClass,
        OUT     PVOID                   KeyInformation,
        IN      ULONG                   Length,
        OUT     PULONG                  ResultLength
    )   PURE;

    STDMETHOD_(NTSTATUS,EnumerateKey)
    (   THIS_
        IN      ULONG                   Index,
        IN      KEY_INFORMATION_CLASS   KeyInformationClass,
        OUT     PVOID                   KeyInformation,
        IN      ULONG                   Length,
        OUT     PULONG                  ResultLength
    )   PURE;

    STDMETHOD_(NTSTATUS,QueryValueKey)
    (   THIS_
        IN      PUNICODE_STRING             ValueName,
        IN      KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
        OUT     PVOID                       KeyValueInformation,
        IN      ULONG                       Length,
        OUT     PULONG                      ResultLength
    )   PURE;

    STDMETHOD_(NTSTATUS,EnumerateValueKey)
    (   THIS_
        IN      ULONG                       Index,
        IN      KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
        OUT     PVOID                       KeyValueInformation,
        IN      ULONG                       Length,
        OUT     PULONG                      ResultLength
    )   PURE;

    STDMETHOD_(NTSTATUS,SetValueKey)
    (   THIS_
        IN      PUNICODE_STRING     ValueName OPTIONAL,
        IN      ULONG               Type,
        IN      PVOID               Data,
        IN      ULONG               DataSize
    )   PURE;

    STDMETHOD_(NTSTATUS,QueryRegistryValues)
    (   THIS_
        IN      PRTL_QUERY_REGISTRY_TABLE   QueryTable,
        IN      PVOID                       Context OPTIONAL
    )   PURE;

    STDMETHOD_(NTSTATUS,NewSubKey)
    (   THIS_
        OUT     IRegistryKey **     RegistrySubKey,
        IN      PUNKNOWN            OuterUnknown    OPTIONAL,
        IN      ACCESS_MASK         DesiredAccess,
        IN      PUNICODE_STRING     SubKeyName,
        IN      ULONG               CreateOptions,
        OUT     PULONG              Disposition     OPTIONAL
    )   PURE;

    STDMETHOD_(NTSTATUS,DeleteKey)
    (   THIS
    )   PURE;

    // For IRegistryKeyInit
    STDMETHOD_(NTSTATUS,Init)
    (   THIS_
        IN      ULONG               RegistryKeyType,
        IN      ACCESS_MASK         DesiredAccess,
        IN      PDEVICE_OBJECT      DeviceObject        OPTIONAL,
        IN      PSUBDEVICE          SubDevice           OPTIONAL,
        IN      POBJECT_ATTRIBUTES  ObjectAttributes    OPTIONAL,
        IN      ULONG               CreateOptions       OPTIONAL,
        OUT     PULONG              Disposition         OPTIONAL
    )   PURE;
};

typedef IRegistryKeyInit *PREGISTRYKEYINIT;


/*****************************************************************************
 * CRegistryKey
 *****************************************************************************
 * Registry Key implementation.
 */
class CRegistryKey
:   public IRegistryKeyInit,
    public CUnknown
{
private:
    HANDLE      m_KeyHandle;    // Key Handle
    BOOLEAN     m_KeyDeleted;   // Key Deleted Flag
    BOOLEAN     m_GeneralKey;   // Only general keys may be deleted

public:
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CRegistryKey);
    ~CRegistryKey();

    /*************************************************************************
     * IRegistryKeyInit methods
     */
    STDMETHODIMP_(NTSTATUS) Init
    (
        IN      ULONG               RegistryKeyType,
        IN      ACCESS_MASK         DesiredAccess,
        IN      PDEVICE_OBJECT      DeviceObject        OPTIONAL,
        IN      PSUBDEVICE          SubDevice           OPTIONAL,
        IN      POBJECT_ATTRIBUTES  ObjectAttributes    OPTIONAL,
        IN      ULONG               CreateOptions       OPTIONAL,
        OUT     PULONG              Disposition         OPTIONAL
    );

    /*************************************************************************
     * IRegistryKey methods
     */
    STDMETHODIMP_(NTSTATUS) QueryKey
    (
        IN      KEY_INFORMATION_CLASS   KeyInformationClass,
        OUT     PVOID                   KeyInformation,
        IN      ULONG                   Length,
        OUT     PULONG                  ResultLength
    );

    STDMETHODIMP_(NTSTATUS) EnumerateKey
    (
        IN      ULONG                   Index,
        IN      KEY_INFORMATION_CLASS   KeyInformationClass,
        OUT     PVOID                   KeyInformation,
        IN      ULONG                   Length,
        OUT     PULONG                  ResultLength
    );

    STDMETHODIMP_(NTSTATUS) QueryValueKey
    (
        IN      PUNICODE_STRING             ValueName,
        IN      KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
        OUT     PVOID                       KeyValueInformation,
        IN      ULONG                       Length,
        OUT     PULONG                      ResultLength
    );

    STDMETHODIMP_(NTSTATUS) EnumerateValueKey
    (
        IN      ULONG                       Index,
        IN      KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
        OUT     PVOID                       KeyValueInformation,
        IN      ULONG                       Length,
        OUT     PULONG                      ResultLength
    );

    STDMETHODIMP_(NTSTATUS) SetValueKey
    (
        IN      PUNICODE_STRING         ValueName   OPTIONAL,
        IN      ULONG                   Type,
        IN      PVOID                   Data,
        IN      ULONG                   DataSize
    );

    STDMETHODIMP_(NTSTATUS) QueryRegistryValues
    (
        IN      PRTL_QUERY_REGISTRY_TABLE   QueryTable,
        IN      PVOID                       Context OPTIONAL
    );

    STDMETHODIMP_(NTSTATUS) NewSubKey
    (
        OUT     PREGISTRYKEY *      RegistrySubKey,
        IN      PUNKNOWN            OuterUnknown    OPTIONAL,
        IN      ACCESS_MASK         DesiredAccess,
        IN      PUNICODE_STRING     SubKeyName,
        IN      ULONG               CreateOptions,
        OUT     PULONG              Disposition     OPTIONAL
    );

    STDMETHODIMP_(NTSTATUS) DeleteKey
    (   void
    );
};

/*****************************************************************************
 * Functions
 */

/*****************************************************************************
 * AcquireDevice()
 *****************************************************************************
 * Acquire exclusive access to the device.
 */
NTSTATUS
AcquireDevice
(
    _In_    PDEVICE_CONTEXT pDeviceContext
);

/*****************************************************************************
 * ReleaseDevice()
 *****************************************************************************
 * Release exclusive access to the device.
 */
VOID
ReleaseDevice
(
    _In_    PDEVICE_CONTEXT pDeviceContext
);

/*****************************************************************************
 * AcquirePositionLock()
 *****************************************************************************
 * Acquire exclusive access to the position lock.
 */
NTSTATUS
AcquirePositionLock
(
    _In_    PDEVICE_CONTEXT pDeviceContext
);

/*****************************************************************************
 * ReleasePositionLock()
 *****************************************************************************
 * Release exclusive access to the position lock.
 */
VOID
ReleasePositionLock
(
    _In_    PDEVICE_CONTEXT pDeviceContext
);

/*****************************************************************************
 * SubdeviceIndex()
 *****************************************************************************
 * Returns the index of a subdevice in the create items list or ULONG(-1) if
 * not found.
 */
ULONG
SubdeviceIndex
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PSUBDEVICE      Subdevice
);


/*****************************************************************************
 * GetDeviceACPIInfo()
 *****************************************************************************
 * Called in response to a PnP - IRP_MN_QUERY_CAPABILITIES
 * Call the bus driver to fill out the inital info,
 * Then overwrite with our own...
 *
 */
NTSTATUS
GetDeviceACPIInfo
(
    IN      PIRP            pIrp,
    IN      PDEVICE_OBJECT  pDeviceObject
);

NTSTATUS
GetUlongFromRegistry
(
    _In_  PDEVICE_CONTEXT   DeviceContext,
    _In_  ULONG             DevInstKeyType,
    _In_  PUNICODE_STRING   SubKeyName,
    _In_  PUNICODE_STRING   ValueName,
    _Out_ PULONG            Value
);

NTSTATUS
GetLockInfoFromRegistry
(
    _In_  PDEVICE_CONTEXT   DeviceContext
);

NTSTATUS
GetIdleInfoFromRegistry
(
    IN  PDEVICE_CONTEXT     DeviceContext,
    OUT PULONG              ConservationIdleTime,
    OUT PULONG              PerformanceIdleTime,
    OUT PDEVICE_POWER_STATE IdleDeviceState,
    OUT PBOOLEAN            ImmediateIdle
);

NTSTATUS
GetFxIdleInfoFromRegistry
(
    _In_  PDEVICE_CONTEXT   DeviceContext,
    _Out_ PBOOLEAN          SingleComponentMultiFxStates
);


NTSTATUS
CheckCurrentPowerState
(
    IN  PDEVICE_OBJECT      pDeviceObject
);

__drv_dispatchType(IRP_MJ_CREATE)
DRIVER_DISPATCH DispatchCreate;

__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH DispatchDeviceIoControl;

__drv_dispatchType(IRP_MJ_READ)
DRIVER_DISPATCH DispatchRead;

__drv_dispatchType(IRP_MJ_WRITE)
DRIVER_DISPATCH DispatchWrite;

__drv_dispatchType(IRP_MJ_FLUSH_BUFFERS)
DRIVER_DISPATCH DispatchFlush;

__drv_dispatchType(IRP_MJ_CLOSE)
DRIVER_DISPATCH DispatchClose;

__drv_dispatchType(IRP_MJ_QUERY_SECURITY)
DRIVER_DISPATCH DispatchQuerySecurity;

__drv_dispatchType(IRP_MJ_SET_SECURITY)
DRIVER_DISPATCH DispatchSetSecurity;

__drv_dispatchType(IRP_MJ_POWER)
DRIVER_DISPATCH DispatchPower;

__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH DispatchPnp;

__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
DRIVER_DISPATCH DispatchSystemControl;

FAST_IO_DEVICE_CONTROL DispatchFastDeviceIoControl;
FAST_IO_READ DispatchFastRead;
FAST_IO_WRITE DispatchFastWrite;

/*****************************************************************************
 * CompleteIrp()
 *****************************************************************************
 * Complete an IRP unless status is STATUS_PENDING.
 */
NTSTATUS
CompleteIrp
(
    IN      PDEVICE_CONTEXT pDeviceContext,
    IN      PIRP            pIrp,
    IN      NTSTATUS        ntStatus
);

/*****************************************************************************
 * ForwardIrpSynchronous()
 *****************************************************************************
 * Forward a PnP IRP to the PDO.  The IRP is not completed at this level,
 * this function does not return until the lower driver has completed the IRP,
 * and DecrementPendingIrpCount() is not called.
 */
NTSTATUS
ForwardIrpSynchronous
(
    IN      PDEVICE_CONTEXT pDeviceContext,
    IN      PIRP            pIrp
);


/*****************************************************************************
 * ForwardIrpAsynchronous()
 *****************************************************************************
 * Forward a PnP IRP to the PDO.  The IRP is completed at this level
 * regardless of the outcome, this function returns immediately regardless of
 * whether the IRP is pending in the lower driver, and
 * DecrementPendingIrpCount() is called in all cases.
 */
NTSTATUS
ForwardIrpAsynchronous
(
    IN      PDEVICE_CONTEXT pDeviceContext,
    IN      PIRP            pIrp
);

/*****************************************************************************
 * PcRequestNewPowerState()
 *****************************************************************************
 * This routine is used to request a new power state for the device.  It is
 * normally used internally by portcls but is also exported to adapters so
 * that the adapters can also request power state changes.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcRequestNewPowerState
(
    __in    PDEVICE_OBJECT      pDeviceObject,
    __in    DEVICE_POWER_STATE  RequestedNewState
);

/*****************************************************************************
 * RequestNewPowerState()
 *****************************************************************************
 * Called by the policy manager to
 * request a change in the power state of the
 * device.
 *
 */
NTSTATUS
RequestNewPowerState
(
    IN      PDEVICE_CONTEXT     pDeviceContext,
    IN      DEVICE_POWER_STATE  RequestedNewState
);

/*****************************************************************************
 * PcDispatchProperty()
 *****************************************************************************
 * Dispatch a property via a PCPROPERTY_ITEM entry.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcDispatchProperty
(
    IN          PIRP                pIrp            OPTIONAL,
    IN          PPROPERTY_CONTEXT   pPropertyContext,
    IN const    KSPROPERTY_SET *    pKsPropertySet  OPTIONAL,
    IN          ULONG               ulIdentifierSize,
    IN          PKSIDENTIFIER       pKsIdentifier,
    IN OUT      PULONG              pulDataSize,
    IN OUT      PVOID               pvData          OPTIONAL
);

/*****************************************************************************
 * PcValidateDeviceContext()
 *****************************************************************************
 * Probes DeviceContext for writing.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcValidateDeviceContext
(
    IN      PDEVICE_CONTEXT         pDeviceContext,
    IN      PIRP                    pIrp
);

/*****************************************************************************
 * CompletePendedIrps
 *****************************************************************************
 * This pulls pended irps off the queue and passes them back to the appropriate
 * dispatcher via KsoDispatchIrp.
 */

typedef enum {

    EMPTY_QUEUE_AND_PROCESS = 0,
    EMPTY_QUEUE_AND_FAIL

} COMPLETE_STYLE;

void
CompletePendedIrps
(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PDEVICE_CONTEXT     pDeviceContext,
    IN      COMPLETE_STYLE      CompleteStyle
);

/*****************************************************************************
 * FailPendedIrpsForSubdevice
 *****************************************************************************
 * This pulls pended irps off the queue for a particular subdevice.
 */
void
FailPendedIrpsForSubdevice
(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PDEVICE_CONTEXT     pDeviceContext,
    IN      PSUBDEVICE          pSubdevice
);

typedef enum {

    QUEUED_CALLBACK_FREE = 0,
    QUEUED_CALLBACK_RETAIN
} QUEUED_CALLBACK_RETURN;

typedef QUEUED_CALLBACK_RETURN (*PFNQUEUED_CALLBACK)(
    IN  PDEVICE_OBJECT      DeviceObject,
    IN  PVOID               Context
    );

#define MAX_THREAD_REENTRANCY   20

typedef struct {

    union {

        PIO_WORKITEM        IoWorkItem;
        KDPC                Dpc;
    }; // unnamed union

    PFNQUEUED_CALLBACK  QueuedCallback;
    PDEVICE_OBJECT      DeviceObject;
    PVOID               Context;
    ULONG               Flags;
    KIRQL               Irql;
    LONG                Enqueued;
} QUEUED_CALLBACK_ITEM, *PQUEUED_CALLBACK_ITEM;

#define EQCM_SUPPORT_OR_FAIL_FLAGS      0xFFFF0000
#define EQCM_SUPPORT_OR_IGNORE_FLAGS    0x0000FFFF

#define EQCF_DIFFERENT_THREAD_REQUIRED  0x00020000

#define EQCM_SUPPORTED_FLAGS    \
    ( EQCF_DIFFERENT_THREAD_REQUIRED )

NTSTATUS
CallbackEnqueue(
    IN      PVOID                   *pCallbackHandle,
    IN      PFNQUEUED_CALLBACK      CallbackRoutine,
    IN      PDEVICE_OBJECT          DeviceObject,
    IN      PVOID                   Context,
    IN      KIRQL                   Irql,
    IN      ULONG                   Flags
    );

NTSTATUS
CallbackCancel(
    IN      PVOID   pCallbackHandle
    );

VOID
CallbackFree(
    IN      PVOID   pCallbackHandle
    );

typedef enum {

    IRPDISP_NOTREADY = 1,
    IRPDISP_PROCESS

} IRPDISP;

enum PcDispatcherLock
{
    ePcDispatcherLock_None,
    ePcDispatcherLock_Device,
    ePcDispatcherLock_Position,
};

IRPDISP
GetIrpDisposition(
    _In_    PDEVICE_OBJECT      DeviceObject,
    _In_    UCHAR               MajorFunction,
    _In_    PcDispatcherLock    DispatcherLock,
    _In_    PIRP                Irp
    );

typedef enum {

    STOPSTYLE_PAUSE_FOR_REBALANCE,
    STOPSTYLE_DISABLE

} PNPSTOP_STYLE;

NTSTATUS
PnpStopDevice
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PNPSTOP_STYLE   StopStyle
);

NTSTATUS
AcquireRemoveLock
(
    IN      PDEVICE_CONTEXT pDeviceContext,
    IN      PVOID           pTag
);

void
ReleaseRemoveLock
(
    IN      PDEVICE_CONTEXT pDeviceContext,
    IN      PVOID           pTag
);

NTSTATUS 
CheckPositionProperty
(
    _In_    PIRP        Irp, 
    _Out_   BOOLEAN *   IsPositionProperty
);

/*****************************************************************************
 * Callbacks registered with runtime power framework.
 *****************************************************************************
 */
PO_FX_COMPONENT_IDLE_STATE_CALLBACK         PcPowerFxComponentIdleStateCallback;
PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK   PcPowerFxComponentActiveConditionCallback;
PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK     PcPowerFxComponentIdleConditionCallback;
PO_FX_DEVICE_POWER_REQUIRED_CALLBACK        PcPowerFxDevicePowerRequiredCallback;
PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK    PcPowerFxDevicePowerNotRequiredCallback;
PO_FX_POWER_CONTROL_CALLBACK                PcPowerFxPowerControlCallback;

/*****************************************************************************
 * Helper routines for calling into the runtime power framework.
 *****************************************************************************
 */
NTSTATUS
AcquireRuntimeFramework
(
    __in PDEVICE_CONTEXT pDeviceContext
);

NTSTATUS
AcquireRuntimeFrameworkStarted
(
    __in PDEVICE_CONTEXT pDeviceContext
);

VOID
ReleaseRuntimeFramework
(
    __in PDEVICE_CONTEXT pDeviceContext
);

NTSTATUS RegisterRuntimeManagement
(
    _In_ PDEVICE_OBJECT  pDeviceObject
);

NTSTATUS RegisterRuntimeManagement_ScSs
(
    _In_ PDEVICE_OBJECT  pDeviceObject,
    _In_ PDEVICE_CONTEXT pDeviceContext
);

NTSTATUS RegisterRuntimeManagement_ScMs(
    _In_ PDEVICE_OBJECT  pDeviceObject,
    _In_ PDEVICE_CONTEXT pDeviceContext
);

NTSTATUS UnregisterRuntimeManagement
(
    PDEVICE_CONTEXT      pDeviceContext
);

NTSTATUS
ActivateRuntimePowerComponent
(
    __in PDEVICE_CONTEXT pDeviceContext,
    __in BOOLEAN         bWait
);

NTSTATUS
IdleRuntimePowerComponent
(
    __in PDEVICE_CONTEXT pDeviceContext,
    __in BOOLEAN         bWait
);

NTSTATUS
ReportRuntimePowerComponentPoweredOn
(
    __in PDEVICE_CONTEXT pDeviceContext
);

NTSTATUS
SetRuntimePowerIdleTimeout
(
    __in PDEVICE_CONTEXT pDeviceContext,
    __in ULONGLONG       ullIdleTimeoutHns
);

ULONGLONG
GetCurrentRuntimeIdleTimeout
(
    __in PDEVICE_CONTEXT pDeviceContext
);

POWER_SETTING_CALLBACK PcPowerSettingCallback;
POWER_SETTING_CALLBACK PcLowPowerEpochCallback;

#endif
