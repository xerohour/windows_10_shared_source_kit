/*****************************************************************************
 * portclsp.h - WDM Streaming port class driver definitions for port drivers
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#ifndef _PORTCLSP_H_
#define _PORTCLSP_H_

#define WIN9X_COMPAT_SPINLOCK

#ifdef UNDER_NT
#ifdef __cplusplus
extern "C" {
#endif
    #include <ntddk.h>
#ifdef __cplusplus
} // extern "C"
#endif
#endif

#include <wdm\wmilib.h>

#include <ntstrsafe.h>
#include <initguid.h>
#include <ntpoapi.h>
#include "kso.h"
#include "perf.h"
#include "portclsmof.h"
#define PC_NEW_NAMES
#define PC_IMPLEMENTATION
#define PC_NO_IMPORTS
#include "portcls.h"
#include "ksshellp.h"
#include "tracelogging.h"









extern ULONG gBufferDuration;
extern KAFFINITY gInterruptAffinity;

#define WAVECYC_NOTIFICATION_FREQUENCY gBufferDuration


#ifndef UNDER_NT

#define KeGetRecommendedSharedDataAlignment() 0x40
#define KeQueryActiveProcessorCountEx(x) 1
#define KeQueryGroupAffinity(x) 1

#endif


PKSPIN_LOCK
GetServiceGroupSpinLock (
    PSERVICEGROUP pServiceGroup
    );









#if     kEnableDebugLogging

#define kNumDebugLogEntries 256
#define kNumULONG_PTRsPerEntry  4
#define DebugLog PcDebugLog

extern ULONG_PTR *gPcDebugLog;
extern DWORD      gPcDebugLogIndex;

void PcDebugLog(ULONG_PTR param1,ULONG_PTR param2,ULONG_PTR param3,ULONG_PTR param4);

#else   //  !kEnableDebugLogging
#define DebugLog (void)
#endif  //  !kEnableDebugLogging


/*****************************************************************************
 * TIMER_STATE
 *****************************************************************************
 * This represents the state of the IoTimeout timer.
 * Uninitialized means we do nothing with it, initialization failed.
 * Stopped means initialization succeeded, but the timer is stopped, adding/removing callbacks do not restart
 * Paused means the device is running, but the timer is not because the callback list is empty
 * Running means the device is running, and the callback list is not empty, so the timer is running
 */
typedef enum
{
    TimerUninitialized = 0,
    TimerStopped,
    TimerPaused,
    TimerRunning
} TIMER_STATE, *PTIMER_STATE;


void ResetTimerState(PDEVICE_OBJECT pDeviceObject);


/*****************************************************************************
 * Structures.
 */

//
// The following structure & functions are used in power.cpp & pin.cpp (rt)
// and are moved from private.h in main to this header file.
//

typedef enum
{
    DeviceRemoved,
    DeviceSurpriseRemoved,
    DeviceRemovePending,
    DeviceAdded
} DEVICE_REMOVE_STATE,*PDEVICE_REMOVE_STATE;

typedef enum
{
    DeviceStopped,
    DeviceStopPending,
    DevicePausedForRebalance,
    DeviceStarted,
    DeviceStartPending          //  StartDevice has not yet finished
} DEVICE_STOP_STATE,*PDEVICE_STOP_STATE;

typedef enum
{
    PersonalityMaxPowerSavings,
    PersonalityMinPowerSavings,
    PersonalityTypicalPowerSavings,
    PersonalityUnknown
} PORTCLS_POWER_PERSONALITY, *PPORTCLS_POWER_PERSONALITY;

/*****************************************************************************

  SynchronizedDMAQueue

  This provides FIFO serialization between contending streams on the device's
  DMA adapter.  The implementation model is a circular queue with removal, but
  no insertion (only add to the end).  Each stream appears only once in the 
  queue.

  */

#if defined(__cplusplus)	//	LH code now has some "C" code

class CIrpStream;

class SynchronizedDMAQueue
{
    KSPIN_LOCK      Synchronizer;

    CIrpStream*     AdapterOwner;   //  Stream currently "owning" the adapter
    CIrpStream*     LastWaiter;     //  End of the stream queue
public:

    SynchronizedDMAQueue();
    ~SynchronizedDMAQueue();

    bool    IsAvailable(CIrpStream* Requester);
    void    RequestComplete(CIrpStream* Requester);
};

class IrpSuspender
{
    KEVENT  SafeToProceed;  //  Power on and device started
    KEVENT  EnableRemoval;  //  Device Stopped
public:
    IrpSuspender();
    ~IrpSuspender();

    void    SuspendIrps();
    bool    CanIrpBeProcessed(__in PIRP Irp);
    void    ReleaseIrps(bool FailThem);
};

typedef struct DCExtension
{
    SynchronizedDMAQueue    Queue;
    IrpSuspender            Suspender;
    //
    // If miniport opted-in to use this feature, this lock is acquired 
    // for stream position requests.
    //
    KMUTEX                  PositionLock; 

    void* operator new(size_t t);
    void operator delete(void* Goner) { ExFreePool(Goner); }
} *PDC_EXTENSION;

#else
typedef void	SynchronizedDMAQueue;
typedef void    IrpSuspender;
typedef void    DCExtension;
typedef DCExtension *PDC_EXTENSION;
#endif

typedef struct DEVICE_CONTEXT *PDEVICE_CONTEXT;


/*****************************************************************************
 * POWER_IRP_CONTEXT
 *****************************************************************************
 * This is the context structure for processing power irps.
 */
typedef struct
{
    PKEVENT         PowerSyncEvent;
    NTSTATUS        Status;
    PIRP            PendingSystemPowerIrp;
    PDEVICE_CONTEXT DeviceContext;
}
POWER_IRP_CONTEXT,*PPOWER_IRP_CONTEXT;

/*****************************************************************************
 * RUNTIME_POWER_INFO
 *****************************************************************************
 * This is the context structure for keeping track of runtime power 
 * management.
 */
typedef struct
{
    PDEVICE_OBJECT                      pDeviceObject;                      // FDO.
    POHANDLE                            pRuntimePowerFrameworkHandle;       // Handle for runtime power framework registration
    BOOLEAN                             RuntimePowerFrameworkStarted;       // Indicates whether framework management of the device has started
    BOOLEAN                             DeviceActive;                       // Indicates whether device is active and can be accessed
    BOOLEAN                             ImmediateIdle;                      // Indicates whether device should be powered down immediately when 
                                                                            // power is no longer required
    BOOLEAN                             SingleComponentMultiFxStates;       // TRUE for single component/multiple Fx states (default is FALSE).
    union 
    {
        struct
        {
            PCPFNRUNTIME_POWER_CONTROL_CALLBACK         PowerControlCallback; // Callback to invoke on miniport upon power control callbacks
            PVOID                                       PowerControlCallbackContext; // Context to be supplied to miniport in PowerControlCallback
        } ScSs; // Single-Component/Single-State
        
        struct  
        {
            PPO_FX_COMPONENT                            PoFxComponent;
            PFN_PC_POST_PO_FX_REGISTER_DEVICE           EvtPcPostPoFxRegisterDevice;
            PFN_PC_PRE_PO_FX_UNREGISTER_DEVICE          EvtPcPrePoFxUnregisterDevice;
            PPO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK  ComponentActiveConditionCallback;
            PPO_FX_COMPONENT_IDLE_CONDITION_CALLBACK    ComponentIdleConditionCallback;
            PPO_FX_COMPONENT_IDLE_STATE_CALLBACK        ComponentIdleStateCallback;
            PPO_FX_POWER_CONTROL_CALLBACK               PowerControlCallback;
            PVOID                                       PoFxDeviceContext;
            ULONG                                       PoFxComponentState;
        } ScMs;  // Single-Component/Multi-State
    };     
        
    PVOID                               pWorkQueueItemSystemPower;          // Work queue item for system power up completion worker
    KMUTEX                              RuntimeFrameworkMutex;              // Mutex to synchronize calls to the runtime framework
    KEVENT                              RuntimeFrameworkStartedEvent;       // Event signalling that the runtime framework is started
    KEVENT                              RuntimeFrameworkUnregisteredEvent;  // Event signalling that the runtime framework has been unregistered
    KSPIN_LOCK                          PowerStateLock;                     // Spin Lock used for synchronizing power requests
    SYSTEM_POWER_CONDITION              PowerCondition;                     // Indicates if systems is running on AC or DC power
    PVOID                               PowerConditionCallbackHandle;       // Handle for registration of power condition callback
    PORTCLS_POWER_PERSONALITY           PowerPersonality;                   // Indicates personality of current system power scheme
    PVOID                               PowerPersonalityCallbackHandle;     // Handle for registration of power personality callback
    BOOL                                LowPowerEpoch;                      // Indicates whether the platform is in Connected Standby
    PVOID                               LowPowerEpochCallbackHandle;        // Handle for registration of low power epoch callback
    BOOL                                RuntimeSystemWorking;               // Indicates if the runtime framework thinks we're in PowerSystemWorking

    //TODO : Move this to another structure for PnpInfo
    PUNKNOWN                            pAdapterPnp;   
}
RUNTIME_POWER_INFO, *PRUNTIME_POWER_INFO;

/*****************************************************************************
 * DEVICE_CONTEXT
 *****************************************************************************
 * This is the context structure for the device object that represents an
 * entire adapter.  It consists primarily of the create dispatch table (in
 * device header) used by KS to create new filters.  Each item in the table
 * represents a port, i.e. a pairing of a port driver and a miniport driver.
 * The table's item structure contains a user-defined pointer, which is used
 * in this case to point to the subdevice context (SUBDEVICE_CONTEXT).  The
 * subdevice context is extended as required for the port driver and miniport
 * in question.
 */
struct DEVICE_CONTEXT                                           // 32 64  struct packing for 32-bit and 64-bit architectures
{
    PVOID                   pDeviceHeader;                      // 4  8   KS mystery device header.
    PDC_EXTENSION           pExtension;                         // 4  8   Not used.
    PDEVICE_OBJECT          PhysicalDeviceObject;               // 4  8   Physical Device Object
    PCPFNSTARTDEVICE        StartDevice;                        // 4  8   Adapter's StartDevice fn, initialized at
                                                                //        DriverEntry & called at PnP Start_Device time.
    PVOID                   MinidriverReserved[4];              // 16 32  Reserved for multiple binding.

    PDEVICE_OBJECT          NextDeviceInStack;                  // 4  8   Member of the stack below us.
    __field_ecount(MaxObjects) 
    PKSOBJECT_CREATE_ITEM   CreateItems;                        // 4  8   Subdevice create table entries;
    ULONG                   Signature;                          // 4  4   DeviceExtension Signature
    ULONG                   MaxObjects;                         // 4  4   Maximum number of subdevices.
    __field_ecount(MaxObjects) 
    PUNICODE_STRING         SymbolicLinkNames;                  // 4  8   Link names of subdevices.
    LIST_ENTRY              DeviceInterfaceList;                // 8  16  List of device interfaces.
    LIST_ENTRY              PhysicalConnectionList;             // 8  16  List of physical connections.
    KMUTEX                  ListProtect;                        // 32 56  Protect device interface and physical connection lists.
    KMUTEX                  DeviceInUse;                        // 32 56  Device Synchronization
    PVOID                   pWorkQueueItemStart;                // 4  8   Work queue item for pnp start.
    PIRP                    IrpStart;                           // 4  8   Start IRP.

    DEVICE_REMOVE_STATE     DeviceRemoveState;                  // 4  4   Device remove state.
    DEVICE_STOP_STATE       DeviceStopState;                    // 4  4   Device stop state.

    BOOLEAN                 PauseForRebalance;                  // 1  1   Whether to pause or turn card off during rebalance.
    BOOLEAN                 PendCreates;                        // 1  1   Whether to pend creates.
    BOOLEAN                 AllowRegisterDeviceInterface;       // 1  1   Whether to allow registering device interfaces.
    BOOLEAN                 EnableIdlePowerManagement;          // 1  1   Whether or not register for idle power management
    ULONG                   ExistingObjectCount;                // 4  4   Number of existing objects.
    ULONG                   ActivePinCount;                     // 4  4   Number of active pins.

    PUNKNOWN                pAdapterPower;                      // 4  8   Pointer to the adapter's
                                                                //        power-management interface.
    PVOID                   SystemStateHandle;                  // 4  8   Used with PoRegisterSystemState.
    PULONG                  IdleTimer;                          // 4  8   A pointer to the idle timer.
    DEVICE_POWER_STATE      CurrentDeviceState;                 // 4  4   The current state of the device.
    SYSTEM_POWER_STATE      CurrentSystemState;                 // 4  4   The current system power state.
    DEVICE_POWER_STATE      DeviceStateMap[PowerSystemMaximum]; // 28 28  System to device power state map.
    DEVICE_POWER_STATE      IdleDeviceState;                    // 4  4   The device state to transition to when idle.
    ULONG                   ConservationIdleTime;               // 4  4   Idle timeout period for conservation mode.
    ULONG                   PerformanceIdleTime;                // 4  4   Idle timeout period for performance mode.

    LIST_ENTRY              TimeoutList;                        // 8  16  List of IoTimeout callback clients
    KSPIN_LOCK              TimeoutLock;                        // 4  8   IoTimeout list and state spinlock

    PIO_REMOVE_LOCK         RemoveLock;                         // 4  8   Pointer to IO_REMOVE_LOCK (too big to directly include)
    BOOLEAN                 PoweringDown;                       // 1  1   FALSE if powering up, TRUE if powering down
    
    union
    {
        BYTE                LockFlags;                          // 1  1   Config (read-only) flags (0 default).
        struct 
        {
            BYTE            UsePositionLock:    1;              //        0 (default) position requests use global lock.
        };
    };  
    
    BOOLEAN                 FreeToUse1;                         // 1  1   Not used.
    BOOLEAN                 FreeToUse2;                         // 1  1   Not used.
    PPC_WMI_PWMILIB_CONTEXT WmiLibContext;                      // 4  8   Pointer to WMILIB_CONTEXT structure
    PRUNTIME_POWER_INFO     RuntimePowerInfo;                   // 4  8   Pointer to runtime power framework context
    TIMER_STATE             TimeoutState;                       // 4  4   Whether or not the IoInitializeTimeout failed, and the timer is running or paused
    PVOID                   VerifierContext;                    // 4  8   Pointer to the address of driver verifier extension context
}
;                                                               // 256 424
                                                                //        NOTE! For legacy reasons, can never be more than 256/512.
                                                                //        If we need to add more members, change an existing member
                                                                //        to a pointer to an additional expansion piece of memory.

/*****************************************************************************
 * AudioSignalProcessingPinProperties
 *****************************************************************************
 * Property item(s) for KSPROPSETID_AudioSignalProcessing. Each port adds
 * this during Init() if the miniport supports IMiniportAudioSignalProcessing.
 */
extern PCPROPERTY_ITEM AudioSignalProcessingPinProperties[1];


/*****************************************************************************
 * PropertyHandler_BasicSupport
 *****************************************************************************
 * Default basic support handler. Basic processing depends on the size of data.
 * For ULONG it only returns Flags. For KSPROPERTY_DESCRIPTION, the structure   
 * is filled.
 */
NTSTATUS                        
PropertyHandler_BasicSupport
(
    IN PPCPROPERTY_REQUEST         PropertyRequest,
    IN ULONG                       Flags,
    IN DWORD                       PropTypeSetId
);

NTSTATUS
UpdateActivePinCount
(
    IN      PDEVICE_CONTEXT DeviceContext,
    IN      BOOL            Increment
);

/*****************************************************************************
 * PORT_DRIVER
 *****************************************************************************
 * This structure describes a port driver.  This is just a hack until we get
 * real object servers running.
 * TODO:  Create real object servers and put port drivers in 'em.
 */
typedef struct
{
    const GUID *            ClassId;
    PFNCREATEINSTANCE       Create;
}
PORT_DRIVER, *PPORT_DRIVER;





/*****************************************************************************
 * Interface identifiers.
 */

DEFINE_GUID(IID_ISubdevice,
0xb4c90a61, 0x5791, 0x11d0, 0x86, 0xf9, 0x0, 0xa0, 0xc9, 0x11, 0xb5, 0x44);
DEFINE_GUID(IID_IRebalanceSubdevice,
0xed6e316e, 0xd30d, 0x4b45, 0xb9, 0x27, 0x4b, 0xd9, 0xc2, 0x49, 0x32, 0xdb);
DEFINE_GUID(IID_IIrpStream,
0xb4c90a70, 0x5791, 0x11d0, 0x86, 0xf9, 0x0, 0xa0, 0xc9, 0x11, 0xb5, 0x44);
DEFINE_GUID(IID_IIrpStreamSubmit,
0xb4c90a71, 0x5791, 0x11d0, 0x86, 0xf9, 0x0, 0xa0, 0xc9, 0x11, 0xb5, 0x44);
DEFINE_GUID(IID_IIrpStreamVirtual,
0xb4c90a72, 0x5791, 0x11d0, 0x86, 0xf9, 0x0, 0xa0, 0xc9, 0x11, 0xb5, 0x44);
DEFINE_GUID(IID_IIrpStreamPhysical,
0xb4c90a73, 0x5791, 0x11d0, 0x86, 0xf9, 0x0, 0xa0, 0xc9, 0x11, 0xb5, 0x44);
DEFINE_GUID(IID_IIrpStreamNotify,
0xb4c90a74, 0x5791, 0x11d0, 0x86, 0xf9, 0x0, 0xa0, 0xc9, 0x11, 0xb5, 0x44);
DEFINE_GUID(IID_IIrpStreamNotifyPhysical,
0xb4c90a75, 0x5791, 0x11d0, 0x86, 0xf9, 0x0, 0xa0, 0xc9, 0x11, 0xb5, 0x44);





/*****************************************************************************
 * Types
 */

/*****************************************************************************
 * PROPERTY_TABLE
 *****************************************************************************
 * Table of properties for KS consumption.
 */
typedef struct
{
    ULONG           PropertySetCount;
    PKSPROPERTY_SET PropertySets;
    BOOLEAN         StaticSets;
    PBOOLEAN        StaticItems;    // NULL means all item tables are static.
}
PROPERTY_TABLE, *PPROPERTY_TABLE;

/*****************************************************************************
 * EVENT_TABLE
 *****************************************************************************
 * Table of events for KS consumption.
 */
typedef struct
{
    ULONG           EventSetCount;
    PKSEVENT_SET    EventSets;
    BOOLEAN         StaticSets;
    PBOOLEAN        StaticItems;    // NULL means all item tables are static.
}
EVENT_TABLE, *PEVENT_TABLE;

/*****************************************************************************
 * PIN_CINSTANCES
 *****************************************************************************
 * This structure stores instance information for a pin.
 */
typedef struct
{
    ULONG   FilterPossible;
    ULONG   FilterNecessary;
    ULONG   GlobalPossible;
    ULONG   GlobalCurrent;
}
PIN_CINSTANCES, *PPIN_CINSTANCES;

/*****************************************************************************
 * SUBDEVICE_DESCRIPTOR
 *****************************************************************************
 * This structure describes a filter.
 */
typedef struct
{
    ULONG                   PinCount;
    ULONG                   NodeCount;
    PKSTOPOLOGY             Topology;
    PKSPIN_DESCRIPTOR       PinDescriptors;
    PPIN_CINSTANCES         PinInstances;
    PPROPERTY_TABLE         PinPropertyTables;
    PEVENT_TABLE            PinEventTables;
    PROPERTY_TABLE          FilterPropertyTable;
    EVENT_TABLE             FilterEventTable;
    PPROPERTY_TABLE         NodePropertyTables;
    PEVENT_TABLE            NodeEventTables;
}
SUBDEVICE_DESCRIPTOR, *PSUBDEVICE_DESCRIPTOR;

/*****************************************************************************
 * PROPERTY_CONTEXT
 *****************************************************************************
 * Context for property handling.
 */
typedef struct
{
    struct ISubdevice *     pSubdevice;
    PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor;
    PPCFILTER_DESCRIPTOR    pPcFilterDescriptor;
    PUNKNOWN                pUnknownMajorTarget;
    PUNKNOWN                pUnknownMinorTarget;
    ULONG                   ulNodeId;
    PULONG                  pulPinInstanceCounts;
}
PROPERTY_CONTEXT, *PPROPERTY_CONTEXT;

/*****************************************************************************
 * INTERLOCKED_LIST
 *****************************************************************************
 * A LIST_ENTRY with a SPIN_LOCK.
 */
typedef struct
{
    LIST_ENTRY  List;
    KSPIN_LOCK  ListLock;
} INTERLOCKED_LIST, *PINTERLOCKED_LIST;

/*****************************************************************************
 * EVENT_CONTEXT
 *****************************************************************************
 * Context for event handling.
 */
typedef struct
{
    PPROPERTY_CONTEXT   pPropertyContext;
    PINTERLOCKED_LIST   pEventList;
    ULONG               ulPinId;
    ULONG               ulEventSetCount;
    const KSEVENT_SET*  pEventSets;
} EVENT_CONTEXT, *PEVENT_CONTEXT;

/*****************************************************************************
 * PCEVENT_ENTRY
 *****************************************************************************
 * An event entry with attached node and pin ids.
 */
typedef struct
{
    KSEVENT_ENTRY           EventEntry;
    const PCEVENT_ITEM *    EventItem;
    PUNKNOWN                pUnknownMajorTarget;
    PUNKNOWN                pUnknownMinorTarget;
    ULONG                   PinId;
    ULONG                   NodeId;
} PCEVENT_ENTRY, *PPCEVENT_ENTRY;

typedef struct
{
    GUID*           Set;
    ULONG           EventId;
    BOOL            PinEvent;
    ULONG           PinId;
    BOOL            NodeEvent;
    ULONG           NodeId;
    BOOL            ContextInUse;
} EVENT_DPC_CONTEXT,*PEVENT_DPC_CONTEXT;

/*****************************************************************************
 * IRPSTREAM_POSITION
 *****************************************************************************
 * Position descriptor for IrpStream.
 */
typedef struct
{
    ULONGLONG   ullCurrentExtent;           // Current Extent - Maximum stream position

    ULONGLONG   ullMappingPosition;         // Mapping Position - The current mapping position
    ULONGLONG   ullUnmappingPosition;       // Unmapping Position - The current unmapping position

    ULONGLONG   ullStreamPosition;          // Stream Position - The current stream position
    ULONGLONG   ullStreamOffset;            // Stream Base Position - The offset between stream and unmapping position

    ULONGLONG   ullPhysicalOffset;          // Physical Offset - Used by clock to adjust for starvation

    ULONG       ulMappingPacketSize;        // Size of current mapping packet
    ULONG       ulMappingOffset;            // Offset into mapping packet that is currently mapped
    ULONG       ulUnmappingPacketSize;      // Size of current unmapping packet
    ULONG       ulUnmappingOffset;          // Offset into unmapping packet that is currently unmapped

    BOOLEAN     bLoopedInterface;           // Pin interface is KSINTERFACE_STANDARD_LOOPED_STREAMING ?
    BOOLEAN     bMappingPacketLooped;       // Mapping packet is KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA ?
    BOOLEAN     bUnmappingPacketLooped;     // Unmapping packet is KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA ?
}
IRPSTREAM_POSITION, *PIRPSTREAM_POSITION;





/*****************************************************************************
 * Interfaces.
 */

/*****************************************************************************
 * ISubdevice
 *****************************************************************************
 * Interface for subdevices.
 */
#if !defined(DEFINE_ABSTRACT_SUBDEVICE)

#define DEFINE_ABSTRACT_SUBDEVICE()                         \
    STDMETHOD_(PKEVENT,GetRemovalEvent)                     \
    (   THIS                                                \
    )   PURE;                                               \
    STDMETHOD_(void,ReleaseChildren)                        \
    (   THIS                                                \
    )   PURE;                                               \
    STDMETHOD_(NTSTATUS,GetDescriptor)                      \
    (   THIS_                                               \
        OUT     const SUBDEVICE_DESCRIPTOR **   Descriptor  \
    )   PURE;                                               \
    STDMETHOD_(NTSTATUS,DataRangeIntersection)              \
    (   THIS_                                               \
        IN      ULONG           PinId,                      \
        IN      PKSDATARANGE    DataRange,                  \
        IN      PKSDATARANGE    MatchingDataRange,          \
        IN      ULONG           OutputBufferLength,         \
        OUT     PVOID           ResultantFormat   OPTIONAL, \
        OUT     PULONG          ResultantFormatLength       \
    )   PURE;                                               \
    STDMETHOD_(void,PowerChangeNotify)                      \
    (   THIS_                                               \
        IN      POWER_STATE     PowerState                  \
    )   PURE;                                               \
    STDMETHOD_(void,PinCount)                               \
    (   THIS_                                               \
        IN      ULONG           PinId,                      \
        IN  OUT PULONG          FilterNecessary,            \
        IN  OUT PULONG          FilterCurrent,              \
        IN  OUT PULONG          FilterPossible,             \
        IN  OUT PULONG          GlobalCurrent,              \
        IN  OUT PULONG          GlobalPossible              \
    )   PURE;\
    STDMETHOD_(NTSTATUS,GetPinName)                         \
    (   THIS_                                               \
        IN      PIRP            Irp,                        \
        IN      PKSP_PIN        Pin,                        \
        OUT     PVOID           Data                        \
    )   PURE;\
    STDMETHOD_(BOOL,IsPinNameSupported)                     \
    (   THIS                                                \
    )   PURE;

#endif //!defined(DEFINE_ABSTRACT_SUBDEVICE)

DECLARE_INTERFACE_(ISubdevice,IIrpTargetFactory)
{
    DEFINE_ABSTRACT_UNKNOWN()           //  For IUnknown

    DEFINE_ABSTRACT_IRPTARGETFACTORY()  //  For IIrpTargetFactory

    DEFINE_ABSTRACT_SUBDEVICE()         //  For ISubdevice

};

typedef ISubdevice *PSUBDEVICE;

#define IMP_ISubdevice_\
    STDMETHODIMP_(PKEVENT)\
    GetRemovalEvent\
    (   void\
    );\
    STDMETHODIMP_(void)\
    ReleaseChildren\
    (   void\
    );\
    STDMETHODIMP_(NTSTATUS)\
    GetDescriptor\
    (   OUT     const SUBDEVICE_DESCRIPTOR **   Descriptor\
    );\
    STDMETHODIMP_(NTSTATUS)\
    DataRangeIntersection\
    (   IN      ULONG           PinId,\
        IN      PKSDATARANGE    DataRange,\
        IN      PKSDATARANGE    MatchingDataRange,\
        IN      ULONG           OutputBufferLength,\
        OUT     PVOID           ResultantFormat     OPTIONAL,\
        OUT     PULONG          ResultantFormatLength\
    );\
    STDMETHODIMP_(void)\
    PowerChangeNotify\
    (   IN      POWER_STATE     PowerState\
    );\
    STDMETHODIMP_(void)\
    PinCount\
    (   IN      ULONG           PinId,\
        IN  OUT PULONG          FilterNecessary,\
        IN  OUT PULONG          FilterCurrent,\
        IN  OUT PULONG          FilterPossible,\
        IN  OUT PULONG          GlobalCurrent,\
        IN  OUT PULONG          GlobalPossible\
    );\
    STDMETHODIMP_(NTSTATUS)\
    GetPinName\
    (   IN      PIRP            Irp,\
        IN      PKSP_PIN        Pin,\
        OUT     PVOID           Data\
    );\
    STDMETHODIMP_(BOOL)\
    IsPinNameSupported();

#define IMP_ISubdevice\
    IMP_IIrpTargetFactory;\
    IMP_ISubdevice_

/*****************************************************************************
* IRebalanceSubdevice
*****************************************************************************
* Interface for rebalancing subdevices.
*/
#if !defined(DEFINE_ABSTRACT_REBALANCE_SUBDEVICE)

#define DEFINE_ABSTRACT_REBALANCE_SUBDEVICE()               \
    STDMETHOD_(NTSTATUS, PnpQueryStop)                      \
    (   THIS                                                \
    )   PURE;                                               \
    STDMETHOD_(void, PnpCancelStop)                         \
    (   THIS                                                \
    )   PURE;                                               \
    STDMETHOD_(void, PnpStop)                               \
    (   THIS                                                \
    )   PURE;                                               \

#endif //!defined(DEFINE_ABSTRACT_REBALANCE_SUBDEVICE)

DECLARE_INTERFACE_(IRebalanceSubdevice, IUnknown)
{
    DEFINE_ABSTRACT_UNKNOWN()               //  For IUnknown
    DEFINE_ABSTRACT_REBALANCE_SUBDEVICE()   //  For ISubdevice
};

typedef IRebalanceSubdevice *PREBALANCESUBDEVICE;

#define IMP_IRebalanceSubdevice_                            \
    STDMETHODIMP_(NTSTATUS) PnpQueryStop                    \
    (                                                       \
    );                                                      \
    STDMETHODIMP_(void) PnpCancelStop                       \
    (                                                       \
    );                                                      \
    STDMETHODIMP_(void) PnpStop                             \
    (                                                       \
    )

#define IMP_IRebalanceSubdevice                             \
    IMP_IRebalanceSubdevice_

/*****************************************************************************
 * IIrpStreamNotify
 *****************************************************************************
 * Irp stream notification interface (IrpStream sources this).
 */
DECLARE_INTERFACE_(IIrpStreamNotify,IUnknown)
{
    DEFINE_ABSTRACT_UNKNOWN()   //  For IUnknown

    STDMETHOD_(void,IrpSubmitted)
    (   THIS_
        IN      PIRP        Irp,
        IN      BOOLEAN     WasExhausted
    )   PURE;

    STDMETHOD_(NTSTATUS,GetPosition)
    (   THIS_
        IN OUT  PIRPSTREAM_POSITION pIrpStreamPosition
    ) PURE;
};

typedef IIrpStreamNotify *PIRPSTREAMNOTIFY;

#define IMP_IIrpStreamNotify\
    STDMETHODIMP_(void)\
    IrpSubmitted\
    (   IN      PIRP        pIrp\
    ,   IN      BOOLEAN     bWasExhausted\
    );\
    STDMETHODIMP_(NTSTATUS)\
    GetPosition\
    (   IN OUT  PIRPSTREAM_POSITION pIrpStreamPosition\
    )

/*****************************************************************************
 * IIrpStreamNotifyPhysical
 *****************************************************************************
 * Irp stream notification interface (IrpStream sources this).
 */
DECLARE_INTERFACE_(IIrpStreamNotifyPhysical,IIrpStreamNotify)
{
    DEFINE_ABSTRACT_UNKNOWN()   //  For IUnknown

    //  For IIrpStreamNotify
    STDMETHOD_(void,IrpSubmitted)
    (   THIS_
        IN      PIRP        Irp,
        IN      BOOLEAN     WasExhausted
    )   PURE;

    STDMETHOD_(NTSTATUS,GetPosition)
    (   THIS_
        IN OUT  PIRPSTREAM_POSITION pIrpStreamPosition
    ) PURE;

    //  For IIrpStreamNotifyPhysical
    STDMETHOD_(void,MappingsCancelled)
    (   THIS_
        IN      PVOID           FirstTag,
        IN      PVOID           LastTag,
        OUT     PULONG          MappingsCancelled
    )   PURE;
};

typedef IIrpStreamNotifyPhysical *PIRPSTREAMNOTIFYPHYSICAL;

#define IMP_IIrpStreamNotifyPhysical_\
    STDMETHODIMP_(void)\
    MappingsCancelled\
    (   IN      PVOID           FirstTag\
    ,   IN      PVOID           LastTag\
    ,   OUT     PULONG          MappingsCancelled\
    )

#define IMP_IIrpStreamNotifyPhysical\
    IMP_IIrpStreamNotify;\
    IMP_IIrpStreamNotifyPhysical_

/*****************************************************************************
 * IIrpStreamSubmit
 *****************************************************************************
 * Irp stream submission interface.
 */
DECLARE_INTERFACE_(IIrpStreamSubmit,IKsShellTransport)
{
    DEFINE_ABSTRACT_UNKNOWN()   //  For IUnknown

    // for IKsShellTransport
    STDMETHOD_(NTSTATUS,TransferKsIrp)(THIS_
        IN PIRP Irp,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;

    STDMETHOD_(void,Connect)(THIS_
        IN PIKSSHELLTRANSPORT NewTransport OPTIONAL,
        OUT PIKSSHELLTRANSPORT *OldTransport OPTIONAL,
        IN KSPIN_DATAFLOW DataFlow
        ) PURE;

    STDMETHOD_(NTSTATUS,SetDeviceState)(THIS_
        IN KSSTATE NewState,
        IN KSSTATE OldState,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;

    STDMETHOD_(void,SetResetState)(THIS_
        IN KSRESET ksReset,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;
#if DBG
    STDMETHOD_(void,DbgRollCall)(THIS_
        IN  ULONG MaxNameSize,
        OUT __out_ecount(MaxNameSize) PCHAR Name,
        OUT PIKSSHELLTRANSPORT* NextTransport,
        OUT PIKSSHELLTRANSPORT* PrevTransport
        ) PURE;
#endif

    // for IIrpStreamSubmit
    STDMETHOD_(NTSTATUS,GetPosition)
    (   THIS_
        IN OUT  PIRPSTREAM_POSITION pIrpStreamPosition
    )   PURE;
};

typedef IIrpStreamSubmit *PIRPSTREAMSUBMIT;

#define IMP_IIrpStreamSubmit\
    IMP_IKsShellTransport;\
    STDMETHODIMP_(NTSTATUS)\
    GetPosition\
    (   IN OUT  PIRPSTREAM_POSITION pIrpStreamPosition\
    )

/*****************************************************************************
 * IRPSTREAMPACKETINFO
 *****************************************************************************
 * Structure for information regarding an IrpStream packet.
 */
typedef struct
{
    KSSTREAM_HEADER Header;
    ULONGLONG       InputPosition;
    ULONGLONG       OutputPosition;
    ULONG           CurrentOffset;
} IRPSTREAMPACKETINFO, *PIRPSTREAMPACKETINFO;

/*****************************************************************************
 * IIrpStream
 *****************************************************************************
 * Irp stream primary interface.
 */
DECLARE_INTERFACE_(IIrpStream,IIrpStreamSubmit)
{
    DEFINE_ABSTRACT_UNKNOWN()   //  For IUnknown

    // for IKsShellTransport
    STDMETHOD_(NTSTATUS,TransferKsIrp)(THIS_
        IN PIRP Irp,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;

    STDMETHOD_(void,Connect)(THIS_
        IN PIKSSHELLTRANSPORT NewTransport OPTIONAL,
        OUT PIKSSHELLTRANSPORT *OldTransport OPTIONAL,
        IN KSPIN_DATAFLOW DataFlow
        ) PURE;

    STDMETHOD_(NTSTATUS,SetDeviceState)(THIS_
        IN KSSTATE NewState,
        IN KSSTATE OldState,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;

    STDMETHOD_(void,SetResetState)(THIS_
        IN KSRESET ksReset,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;
#if DBG
    STDMETHOD_(void,DbgRollCall)(THIS_
        IN  ULONG MaxNameSize,
        OUT __out_ecount(MaxNameSize) PCHAR Name,
        OUT PIKSSHELLTRANSPORT* NextTransport,
        OUT PIKSSHELLTRANSPORT* PrevTransport
        ) PURE;
#endif

    // for IIrpStreamSubmit
    STDMETHOD_(NTSTATUS,GetPosition)
    (   THIS_
        IN OUT  PIRPSTREAM_POSITION pIrpStreamPosition
    )   PURE;

    // for IIrpStream
    STDMETHOD_(NTSTATUS,Init)
    (   THIS_
        IN      BOOLEAN             WriteOperation,
        IN      PKSPIN_CONNECT      PinConnect,
        IN      PDEVICE_OBJECT      DeviceObject    OPTIONAL,
        IN      PADAPTER_OBJECT     AdapterObject   OPTIONAL,
        IN      ULONG               MaxMappingRegisters
    )   PURE;

    STDMETHOD_(void,CancelAllIrps)
    (   THIS_
        IN BOOL ClearPositionCounters
    )   PURE;

    STDMETHOD_(void,TerminatePacket)
    (   THIS
    )   PURE;

    STDMETHOD_(NTSTATUS,ChangeOptionsFlags)
    (   THIS_
        IN      ULONG    MappingOrMask,
        IN      ULONG    MappingAndMask,
        IN      ULONG    UnmappingOrMask,
        IN      ULONG    UnmappingAndMask
    )   PURE;

    STDMETHOD_(NTSTATUS,GetPacketInfo)
    (   THIS_
        OUT     PIRPSTREAMPACKETINFO    Mapping     OPTIONAL,
        OUT     PIRPSTREAMPACKETINFO    Unmapping   OPTIONAL
    )   PURE;

    STDMETHOD_(NTSTATUS,SetPacketOffsets)
    (   THIS_
        IN      ULONG   MappingOffset,
        IN      ULONG   UnmappingOffset
    )   PURE;
};

typedef IIrpStream *PIRPSTREAM;

#define IMP_IIrpStream_\
    STDMETHODIMP_(NTSTATUS)\
    Init\
    (   IN      BOOLEAN             WriteOperation,\
        IN      PKSPIN_CONNECT      PinConnect,\
        IN      PDEVICE_OBJECT      DeviceObject    OPTIONAL,\
        IN      PADAPTER_OBJECT     AdapterObject   OPTIONAL,\
        IN      ULONG               MaxMappingRegisters\
    );\
    STDMETHODIMP_(void)\
    CancelAllIrps\
    (   IN BOOL ClearPositionCounters\
    );\
    STDMETHODIMP_(void)\
    TerminatePacket\
    (   void\
    );\
    STDMETHODIMP_(NTSTATUS)\
    ChangeOptionsFlags\
    (   IN      ULONG    MappingOrMask,\
        IN      ULONG    MappingAndMask,\
        IN      ULONG    UnmappingOrMask,\
        IN      ULONG    UnmappingAndMask\
    );\
    STDMETHODIMP_(NTSTATUS)\
    GetPacketInfo\
    (   OUT     PIRPSTREAMPACKETINFO    Mapping     OPTIONAL,\
        OUT     PIRPSTREAMPACKETINFO    Unmapping   OPTIONAL\
    );\
    STDMETHODIMP_(NTSTATUS)\
    SetPacketOffsets\
    (   IN      ULONG   MappingOffset,\
        IN      ULONG   UnmappingOffset\
    )

#define IMP_IIrpStream\
    IMP_IIrpStreamSubmit;\
    IMP_IIrpStream_

/*****************************************************************************
 * IIrpStreamVirtual
 *****************************************************************************
 * Irp stream virtual mapping interface.
 */
DECLARE_INTERFACE_(IIrpStreamVirtual,IIrpStream)
{
    DEFINE_ABSTRACT_UNKNOWN()   //  For IUnknown

    // for IKsShellTransport
    STDMETHOD_(NTSTATUS,TransferKsIrp)(THIS_
        IN PIRP Irp,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;

    STDMETHOD_(void,Connect)(THIS_
        IN PIKSSHELLTRANSPORT NewTransport OPTIONAL,
        OUT PIKSSHELLTRANSPORT *OldTransport OPTIONAL,
        IN KSPIN_DATAFLOW DataFlow
        ) PURE;

    STDMETHOD_(NTSTATUS,SetDeviceState)(THIS_
        IN KSSTATE NewState,
        IN KSSTATE OldState,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;

    STDMETHOD_(void,SetResetState)(THIS_
        IN KSRESET ksReset,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;
#if DBG
    STDMETHOD_(void,DbgRollCall)(THIS_
        IN  ULONG MaxNameSize,
        OUT __out_ecount(MaxNameSize) PCHAR Name,
        OUT PIKSSHELLTRANSPORT* NextTransport,
        OUT PIKSSHELLTRANSPORT* PrevTransport
        ) PURE;
#endif

    // for IIrpStreamSubmit
    STDMETHOD_(NTSTATUS,GetPosition)
    (   THIS_
        IN OUT  PIRPSTREAM_POSITION pIrpStreamPosition
    )   PURE;

    // for IIrpStream
    STDMETHOD_(NTSTATUS,Init)
    (   THIS_
        IN      BOOLEAN             WriteOperation,
        IN      PKSPIN_CONNECT      PinConnect,
        IN      PDEVICE_OBJECT      DeviceObject    OPTIONAL,
        IN      PADAPTER_OBJECT     AdapterObject   OPTIONAL,
        IN      ULONG               MaxMappingRegisters
    )   PURE;

    STDMETHOD_(void,CancelAllIrps)
    (   THIS_
        IN BOOL ClearPositionCounters
    )   PURE;

    STDMETHOD_(void,TerminatePacket)
    (   THIS
    )   PURE;

    STDMETHOD_(NTSTATUS,ChangeOptionsFlags)
    (   THIS_
        IN      ULONG    MappingOrMask,
        IN      ULONG    MappingAndMask,
        IN      ULONG    UnmappingOrMask,
        IN      ULONG    UnmappingAndMask
    )   PURE;

    STDMETHOD_(NTSTATUS,GetPacketInfo)
    (   THIS_
        OUT     PIRPSTREAMPACKETINFO    Mapping     OPTIONAL,
        OUT     PIRPSTREAMPACKETINFO    Unmapping   OPTIONAL
    )   PURE;

    STDMETHOD_(NTSTATUS,SetPacketOffsets)
    (   THIS_
        IN      ULONG   MappingOffset,
        IN      ULONG   UnmappingOffset
    )   PURE;

    //  For IIrpStreamVirtual
    STDMETHOD_(void,RegisterNotifySink)
    (   THIS_
        IN      PIRPSTREAMNOTIFY    NotificationSink    OPTIONAL
    )   PURE;

    STDMETHOD_(void,GetLockedRegion)
    (   THIS_
        OUT     PULONG      ByteCount,
        OUT     PVOID *     SystemAddress
    )   PURE;

    STDMETHOD_(void,ReleaseLockedRegion)
    (   THIS_
        IN      ULONG       ByteCount
    )   PURE;

    STDMETHOD_(void,Copy)
    (   THIS_
        IN      BOOLEAN     WriteOperation,
        IN      ULONG       RequestedSize,
        OUT     PULONG      ActualSize,
        IN OUT  PVOID       Buffer
    )   PURE;

    STDMETHOD_(void,Complete)
    (   THIS_
        IN      ULONG       RequestedSize,
        OUT     PULONG      ActualSize
    )   PURE;

    STDMETHOD_(PKSPIN_LOCK,GetIrpStreamPositionLock)
    (   THIS
    )   PURE;

};

typedef IIrpStreamVirtual *PIRPSTREAMVIRTUAL;

#define IMP_IIrpStreamVirtual_\
    STDMETHODIMP_(void)\
    RegisterNotifySink\
    (   IN      PIRPSTREAMNOTIFY    NotificationSink    OPTIONAL\
    );\
    STDMETHODIMP_(void)\
    GetLockedRegion\
    (   OUT     PULONG      ByteCount,\
        OUT     PVOID *     SystemAddress\
    );\
    STDMETHODIMP_(void)\
    ReleaseLockedRegion\
    (   IN      ULONG       ByteCount\
    );\
    STDMETHODIMP_(void)\
    Copy\
    (   IN      BOOLEAN     WriteOperation,\
        IN      ULONG       RequestedSize,\
        OUT     PULONG      ActualSize,\
        IN OUT  PVOID       Buffer\
    );\
    STDMETHODIMP_(void)\
    Complete\
    (   IN      ULONG       RequestedSize,\
        OUT     PULONG      ActualSize\
    );\
    STDMETHODIMP_(PKSPIN_LOCK)\
    GetIrpStreamPositionLock\
    (\
    )

#define IMP_IIrpStreamVirtual\
    IMP_IIrpStream;\
    IMP_IIrpStreamVirtual_

/*****************************************************************************
 * IIrpStreamPhysical
 *****************************************************************************
 * Irp stream physical mapping interface.
 */
DECLARE_INTERFACE_(IIrpStreamPhysical,IIrpStream)
{
    DEFINE_ABSTRACT_UNKNOWN()   //  For IUnknown

    // for IKsShellTransport
    STDMETHOD_(NTSTATUS,TransferKsIrp)(THIS_
        IN PIRP Irp,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;

    STDMETHOD_(void,Connect)(THIS_
        IN PIKSSHELLTRANSPORT NewTransport OPTIONAL,
        OUT PIKSSHELLTRANSPORT *OldTransport OPTIONAL,
        IN KSPIN_DATAFLOW DataFlow
        ) PURE;

    STDMETHOD_(NTSTATUS,SetDeviceState)(THIS_
        IN KSSTATE NewState,
        IN KSSTATE OldState,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;

    STDMETHOD_(void,SetResetState)(THIS_
        IN KSRESET ksReset,
        OUT PIKSSHELLTRANSPORT* NextTransport
        ) PURE;
#if DBG
    STDMETHOD_(void,DbgRollCall)(THIS_
        IN  ULONG MaxNameSize,
        OUT __out_ecount(MaxNameSize) PCHAR Name,
        OUT PIKSSHELLTRANSPORT* NextTransport,
        OUT PIKSSHELLTRANSPORT* PrevTransport
        ) PURE;
#endif

    // for IIrpStreamSubmit
    STDMETHOD_(NTSTATUS,GetPosition)
    (   THIS_
        IN OUT  PIRPSTREAM_POSITION pIrpStreamPosition
    )   PURE;

    // for IIrpStream
    STDMETHOD_(NTSTATUS,Init)
    (   THIS_
        IN      BOOLEAN             WriteOperation,
        IN      PKSPIN_CONNECT      PinConnect,
        IN      PDEVICE_OBJECT      DeviceObject    OPTIONAL,
        IN      PADAPTER_OBJECT     AdapterObject   OPTIONAL,
        IN      ULONG               MaxMappingRegisters
    )   PURE;

    STDMETHOD_(void,CancelAllIrps)
    (   THIS_
        IN BOOL ClearPositionCounters
    )   PURE;

    STDMETHOD_(void,TerminatePacket)
    (   THIS
    )   PURE;

    STDMETHOD_(NTSTATUS,ChangeOptionsFlags)
    (   THIS_
        IN      ULONG    MappingOrMask,
        IN      ULONG    MappingAndMask,
        IN      ULONG    UnmappingOrMask,
        IN      ULONG    UnmappingAndMask
    )   PURE;

    STDMETHOD_(NTSTATUS,GetPacketInfo)
    (   THIS_
        OUT     PIRPSTREAMPACKETINFO    Mapping     OPTIONAL,
        OUT     PIRPSTREAMPACKETINFO    Unmapping   OPTIONAL
    )   PURE;

    STDMETHOD_(NTSTATUS,SetPacketOffsets)
    (   THIS_
        IN      ULONG   MappingOffset,
        IN      ULONG   UnmappingOffset
    )   PURE;

    //  For IIrpStreamPhysical
    STDMETHOD_(void,RegisterPhysicalNotifySink)
    (   THIS_
        IN      PIRPSTREAMNOTIFYPHYSICAL    NotificationSink    OPTIONAL
    )   PURE;

    STDMETHOD_(void,GetMapping)
    (   THIS_
        IN      PVOID               Tag,
        OUT     PPHYSICAL_ADDRESS   PhysicalAddress,
        OUT     PVOID *             VirtualAddress,
        OUT     PULONG              ByteCount,
        OUT     PULONG              Flags
#define MAPPING_FLAG_END_OF_PACKET 0x00000001
    )   PURE;

    STDMETHOD_(void,ReleaseMapping)
    (   THIS_
        IN      PVOID               Tag
    )   PURE;
};

typedef IIrpStreamPhysical *PIRPSTREAMPHYSICAL;

#define IMP_IIrpStreamPhysical_\
    STDMETHODIMP_(void)\
    RegisterPhysicalNotifySink\
    (   IN      PIRPSTREAMNOTIFYPHYSICAL    NotificationSink    OPTIONAL\
    );\
    STDMETHODIMP_(void)\
    GetMapping\
    (   IN      PVOID               Tag,\
        OUT     PPHYSICAL_ADDRESS   PhysicalAddress,\
        OUT     PVOID *             VirtualAddress,\
        OUT     PULONG              ByteCount,\
        OUT     PULONG              Flags\
    );\
    STDMETHODIMP_(void)\
    ReleaseMapping\
    (   IN      PVOID               Tag\
    )

#define IMP_IIrpStreamPhysical\
    IMP_IIrpStream;\
    IMP_IIrpStreamPhysical_

#define IMP_IPortClsVersion \
        STDMETHODIMP_(DWORD) GetVersion() { return kVersionWindowsLonghorn; };

#define IMP_IPortWMIRegistration                    \
    STDMETHODIMP_(NTSTATUS) RegisterWMIProvider     \
    (   IN PDEVICE_OBJECT _pDeviceObject,           \
        IN PVOID _pMiniPortWmiLibContext  \
    ) { return RegisterMiniportWMIProvider(_pDeviceObject,_pMiniPortWmiLibContext);};\
    STDMETHODIMP_(NTSTATUS) UnregisterWMIProvider   \
    (    IN PDEVICE_OBJECT _pDeviceObject           \
    ) {return UnregisterMiniportWMIProvider(_pDeviceObject);};

                      
#define IMP_IPortClsSubdeviceEx                 \
    STDMETHODIMP_(NTSTATUS) UpdatePinDescriptor     \
    (   IN ULONG              _ulPinId,              \
        IN ULONG              _ulFlags,              \
        IN PPCPIN_DESCRIPTOR  _pPinDescriptor        \
    )

#define IMP_IPortClsPower                                                       \
    STDMETHODIMP_(NTSTATUS) RegisterAdapterPowerManagement                      \
    (                                                                           \
        IN PUNKNOWN       _pUnknown,                                            \
        IN PDEVICE_OBJECT _pDeviceObject                                        \
    ) { return PcRegisterAdapterPowerManagement(_pUnknown, _pDeviceObject); };  \
    STDMETHODIMP_(NTSTATUS) UnregisterAdapterPowerManagement                    \
    (                                                                           \
        IN PDEVICE_OBJECT _pDeviceObject                                        \
    ) { return PcUnregisterAdapterPowerManagement(_pDeviceObject); };           \
    STDMETHODIMP_(NTSTATUS) SetIdlePowerManagement                      \
    (                                                                           \
        IN PDEVICE_OBJECT _pDeviceObject,                                       \
        IN BOOLEAN        _bEnabled                                             \
    ) { return iSetIdlePowerManagement(_pDeviceObject, _bEnabled); };

#define IMP_IPortClsPnp                                                         \
    STDMETHODIMP_(NTSTATUS) RegisterAdapterPnpManagement                        \
    (                                                                           \
        IN PUNKNOWN       _pUnknown,                                            \
        IN PDEVICE_OBJECT _pDeviceObject                                        \
    ) { return PcRegisterAdapterPnpManagement(_pUnknown, _pDeviceObject); };    \
    STDMETHODIMP_(NTSTATUS) UnregisterAdapterPnpManagement                      \
    (                                                                           \
        IN PDEVICE_OBJECT _pDeviceObject                                        \
    ) { return PcUnregisterAdapterPnpManagement(_pDeviceObject); };

#define IMP_IPortClsEtwHelper                      \
    STDMETHODIMP_(NTSTATUS) MiniportWriteEtwEvent \
    (                                              \
        IN EPcMiniportEngineEvent	miniportEventType,\
    	IN ULONGLONG 	pvData1,\
	    IN ULONGLONG   pvData2,\
	    IN ULONGLONG   ulData3,\
	    IN ULONGLONG   ulData4 \
    ) ;

#define IMP_IPortClsRuntimePower                                                \
    STDMETHODIMP_(NTSTATUS) SendPowerControl                                    \
    (                                                                           \
        _In_      PDEVICE_OBJECT    _DeviceObject,                              \
        _In_      LPCGUID           _PowerControlCode,                          \
        _In_opt_  PVOID             _InBuffer,                                  \
        _In_      SIZE_T            _InBufferSize,                              \
        _Out_opt_ PVOID             _OutBuffer,                                 \
        _In_      SIZE_T            _OutBufferSize,                             \
        _Out_opt_ PSIZE_T           _BytesReturned                              \
    ) { return iSendRuntimePowerControl(                                        \
               _DeviceObject,                                                   \
               _PowerControlCode,                                               \
               _InBuffer,                                                       \
               _InBufferSize,                                                   \
               _OutBuffer,                                                      \
               _OutBufferSize,                                                  \
               _BytesReturned); };                                              \
    STDMETHODIMP_(NTSTATUS) RegisterPowerControlCallback                        \
    (                                                                           \
        _In_        PDEVICE_OBJECT                      _DeviceObject,          \
        _In_        PCPFNRUNTIME_POWER_CONTROL_CALLBACK _Callback,              \
        _In_opt_    PVOID                               _Context                \
    ) { return iRegisterRuntimePowerControlCallback                             \
        (_DeviceObject,_Callback,_Context); };                                  \
    STDMETHODIMP_(NTSTATUS) UnregisterPowerControlCallback                      \
    (                                                                           \
        _In_    PDEVICE_OBJECT      _DeviceObject                               \
    ) { return iUnregisterRuntimePowerControlCallback(_DeviceObject); };

/*****************************************************************************
 * iUpdatePinDescriptor
 *****************************************************************************
 * Internal helper that updates the descriptor for the specified pin on the 
 * specified subdevice.
 */
NTSTATUS
iUpdatePinDescriptor
(
    _In_    PDEVICE_OBJECT          _pDeviceObject,
    _In_    PSUBDEVICE_DESCRIPTOR   _pSubdeviceDescriptor,
    _In_    ULONG                   _ulPinId,
    _In_    ULONG                   _ulFlags,
    _In_    PPCPIN_DESCRIPTOR       _pPinDescriptor
);

/*****************************************************************************
 * iSetIdlePowerManagement
 *****************************************************************************
 * Internal helper that enables or disabled idle power managment for
 * the adapter.
 */
NTSTATUS
iSetIdlePowerManagement
(
    IN PDEVICE_OBJECT _pDeviceObject,
    IN BOOLEAN        _bEnabled
);

/*****************************************************************************
 * iSendRuntimePowerControl
 *****************************************************************************
 * Internal helper that sends a private context to the runtime power 
 * framework.
 */
NTSTATUS
iSendRuntimePowerControl
(
    _In_      PDEVICE_OBJECT    _DeviceObject,
    _In_      LPCGUID           _PowerControlCode,
    _In_opt_  PVOID             _InBuffer,
    _In_      SIZE_T            _InBufferSize,
    _Out_opt_ PVOID             _OutBuffer,
    _In_      SIZE_T            _OutBufferSize,
    _Out_opt_ PSIZE_T           _BytesReturned
);

/*****************************************************************************
 * iSendRuntimePowerControl
 *****************************************************************************
 * Internal helper that handles the miniport registering a callback for
 * runtime power control callbacks.
 */
NTSTATUS
iRegisterRuntimePowerControlCallback
(
    _In_        PDEVICE_OBJECT                      _DeviceObject,
    _In_        PCPFNRUNTIME_POWER_CONTROL_CALLBACK _Callback,
    _In_opt_    PVOID                               _Context
);

/*****************************************************************************
 * iUnregisterRuntimePowerControlCallback
 *****************************************************************************
 * Internal helper that handles the miniport unregistering a callback for
 * runtime power control callbacks.
 */
NTSTATUS
iUnregisterRuntimePowerControlCallback
(
    _In_      PDEVICE_OBJECT    _DeviceObject
);

/*****************************************************************************
 * PcCreateSubdeviceDescriptor()
 *****************************************************************************
 * Creates a subdevice descriptor.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcCreateSubdeviceDescriptor
(
    IN      PPCFILTER_DESCRIPTOR    FilterDescriptor,
    IN      ULONG                   CategoriesCount,
    IN      GUID *                  Categories,
    IN      ULONG                   StreamInterfacesCount,
    IN      PKSPIN_INTERFACE        StreamInterfaces,
    IN      ULONG                   FilterPropertySetCount,
    IN      PKSPROPERTY_SET         FilterPropertySets,
    IN      ULONG                   FilterEventSetCount,
    IN      PKSEVENT_SET            FilterEventSets,
    IN      ULONG                   PinPropertySetCount,
    IN      PKSPROPERTY_SET         PinPropertySets,
    IN      ULONG                   PinEventSetCount,
    IN      PKSEVENT_SET            PinEventSets,
    OUT     PSUBDEVICE_DESCRIPTOR * OutDescriptor
);

/*****************************************************************************
 * PcDeleteSubdeviceDescriptor()
 *****************************************************************************
 * Deletes a subdevice descriptor.
 */
PORTCLASSAPI
void
NTAPI
PcDeleteSubdeviceDescriptor
(
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor
);

/*****************************************************************************
 * PcValidateConnectRequest()
 *****************************************************************************
 * Validate attempt to create a pin.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcValidateConnectRequest
(
    IN      PIRP                    pIrp,
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor,
    OUT     PKSPIN_CONNECT *        ppKsPinConnect
);

/*****************************************************************************
 * PcValidatePinCount()
 *****************************************************************************
 * Validate pin count.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcValidatePinCount
(   IN      ULONG                   ulPinId,
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor,
    IN      PULONG                  pulPinInstanceCounts
);

/*****************************************************************************
 * PcTerminateConnection()
 *****************************************************************************
 * Decrement instance counts associated with a pin.
 */
PORTCLASSAPI
void
NTAPI
PcTerminateConnection
(
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor,
    IN      PULONG                  pulPinInstanceCounts,
    IN      ULONG                   ulPinId
);

/*****************************************************************************
 * PcVerifyFilterIsReady()
 *****************************************************************************
 * Verify necessary pins are connected.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcVerifyFilterIsReady
(
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor,
    IN      PULONG                  pulPinInstanceCounts
);

/*****************************************************************************
 * PcIsPropertyInPropertyTable()
 *****************************************************************************
 * Checks if a PROPERTY_ITEM is present in the specified property table.
 */
BOOLEAN
PcIsPropertyInPropertyTable
(
    _In_    PPROPERTY_TABLE         PropertyTable,
    _In_    const PCPROPERTY_ITEM * PropertyItem
);

/*****************************************************************************
 * PcAddToPropertyTable()
 *****************************************************************************
 * Adds a PROPERTYITEM property table to a PROPERTY_TABLE structure.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcAddToPropertyTable
(
    IN OUT  PPROPERTY_TABLE         PropertyTable,
    IN      ULONG                   PropertyItemCount,
    IN      const PCPROPERTY_ITEM * PropertyItems,
    IN      ULONG                   PropertyItemSize
);

/*****************************************************************************
 * PcFreePropertyTable()
 *****************************************************************************
 * Frees allocated memory in a PROPERTY_TABLE structure.
 */
PORTCLASSAPI
void
NTAPI
PcFreePropertyTable
(
    IN      PPROPERTY_TABLE         PropertyTable
);

/*****************************************************************************
 * PcHandlePropertyWithTable()
 *****************************************************************************
 * Uses a property table (PropertyTable) to handle a property request IOCTL
 * on the filter/pin. Uses NodePropertyTable if the IOCTL is directed at a
 * node.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcHandlePropertyWithTable
(
    IN      PIRP                    pIrp,
    IN      PROPERTY_TABLE          PropertyTable,
    IN      ULONG                   NodeCount,
    IN      PPROPERTY_TABLE         NodePropertyTables,
    IN      PPROPERTY_CONTEXT       pPropertyContext
);

/*****************************************************************************
 * PcPinPropertyHandler()
 *****************************************************************************
 * Property handler for pin properties on the filter.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcPinPropertyHandler
(
    IN      PIRP        pIrp,
    IN      PKSP_PIN    pKsPPin,
    IN OUT  PVOID       pvData
);

/*****************************************************************************
 * PcAddToEventTable()
 *****************************************************************************
 * Adds a EVENTITEM event table to an EVENT_TABLE structure.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcAddToEventTable
(
    IN OUT  PEVENT_TABLE            EventTable,
    IN      ULONG                   EventItemCount,
    IN      const PCEVENT_ITEM *    EventItems,
    IN      ULONG                   EventItemSize
);

/*****************************************************************************
 * PcFreeEventTable()
 *****************************************************************************
 * Frees allocated memory in an EVENT_TABLE structure.
 */
PORTCLASSAPI
void
NTAPI
PcFreeEventTable
(
    IN      PEVENT_TABLE            EventTable
);

/*****************************************************************************
 * PcHandleEnableEventWithTable()
 *****************************************************************************
 * Uses an event table in the event context (pContext) to handle a KS enable
 * event IOCTL on the filter/pin. Uses the NodeEventTables if the IOCTL is
 * directed at a node.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcHandleEnableEventWithTable
(
    IN      PIRP                    pIrp,
    IN      PEVENT_CONTEXT          pContext,
    IN      ULONG                   NodeCount,
    IN      PEVENT_TABLE            NodeEventTables
);

/*****************************************************************************
 * PcHandleDisableEventWithTable()
 *****************************************************************************
 * Uses an event table to handle a KS disable event IOCTL.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcHandleDisableEventWithTable
(
    IN      PIRP                    pIrp,
    IN      PEVENT_CONTEXT          pContext
);

/*****************************************************************************
 * PcGenerateEventList()
 *****************************************************************************
 * Walks an event list and generates desired events.
 */
PORTCLASSAPI
void
NTAPI
PcGenerateEventList
(
    IN      PINTERLOCKED_LIST   EventList,
    IN      GUID*               Set     OPTIONAL,
    IN      ULONG               EventId,
    IN      BOOL                PinEvent,
    IN      ULONG               PinId,
    IN      BOOL                NodeEvent,
    IN      ULONG               NodeId
);

/*****************************************************************************
 * PcGenerateEventDeferredRoutine()
 *****************************************************************************
 * This DPC routine is used when GenerateEventList is called at greater
 * that DISPATCH_LEVEL.
 */
KDEFERRED_ROUTINE PcGenerateEventDeferredRoutine;

/*****************************************************************************
 * PcNewIrpStreamVirtual()
 *****************************************************************************
 * Creates and initializes an IrpStream object with a virtual access
 * interface.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewIrpStreamVirtual
(
    OUT     PIRPSTREAMVIRTUAL * OutIrpStreamVirtual,
    IN      PUNKNOWN            OuterUnknown            OPTIONAL,
    IN      BOOLEAN             WriteOperation,
    IN      PKSPIN_CONNECT      PinConnect,
    IN      PDEVICE_OBJECT      DeviceObject
);

/*****************************************************************************
 * PcNewIrpStreamPhysical()
 *****************************************************************************
 * Creates and initializes an IrpStream object with a physical access
 * interface.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewIrpStreamPhysical
(
    OUT     PIRPSTREAMPHYSICAL *    OutIrpStreamPhysical,
    IN      PUNKNOWN                OuterUnknown            OPTIONAL,
    IN      BOOLEAN                 WriteOperation,
    IN      PKSPIN_CONNECT          PinConnect,
    IN      PDEVICE_OBJECT          DeviceObject,
    IN      PADAPTER_OBJECT         AdapterObject,
    IN      ULONG                   MaxMappingRegisters
);

/*****************************************************************************
 * PcDmaSlaveDescription()
 *****************************************************************************
 * Fills in a DMA device description for a slave device based on a resource.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcDmaSlaveDescription
(
    IN      PRESOURCELIST       ResourceList,
    IN      ULONG               ResourceIndex,
    IN      BOOLEAN             DemandMode,
    IN      BOOLEAN             AutoInitialize,
    IN      DMA_SPEED           DmaSpeed,
    IN      ULONG               MaximumLength,
    IN      ULONG               DmaPort,
    OUT     PDEVICE_DESCRIPTION DeviceDescription
);

/*****************************************************************************
 * PcDmaMasterDescription()
 *****************************************************************************
 * Fills in a DMA device description for a master device based on a resource
 * list.
 */
PORTCLASSAPI
void
NTAPI
PcDmaMasterDescription
(
    IN      PRESOURCELIST       ResourceList        OPTIONAL,
    IN      BOOLEAN             ScatterGather,
    IN      BOOLEAN             Dma32BitAddresses,
    IN      BOOLEAN             IgnoreCount,
    IN      BOOLEAN             Dma64BitAddresses,
    IN      DMA_WIDTH           DmaWidth,
    IN      DMA_SPEED           DmaSpeed,
    IN      ULONG               MaximumLength,
    IN      ULONG               DmaPort,
    OUT     PDEVICE_DESCRIPTION DeviceDescription
);

/*****************************************************************************
 * PcCaptureFormat()
 *****************************************************************************
 * Capture a data format in an allocated buffer, possibly changing offensive
 * formats.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcCaptureFormat
(
    OUT     PKSDATAFORMAT *         ppKsDataFormatOut,
    IN      PKSDATAFORMAT           pKsDataFormatIn,
    IN      ULONG                   ulKsDataFormatIn,
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor,
    IN      ULONG                   ulPinId
);

/*****************************************************************************
 * PcAcquireFormatResources()
 *****************************************************************************
 * Acquire resources specified in a format.
 */
PORTCLASSAPI
void
NTAPI
PcAcquireFormatResources
(
    IN      PKSDATAFORMAT           pKsDataFormatIn,
    IN      PSUBDEVICE_DESCRIPTOR   pSubdeviceDescriptor,
    IN      ULONG                   ulPinId,
    IN      PPROPERTY_CONTEXT       pPropertyContext
);

/*****************************************************************************
 * PcUnregisterSubdevice()
 *****************************************************************************
 * Unregisters a subdevice.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterSubdevice
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PUNKNOWN        Unknown
);

/*****************************************************************************
 * PcUnregisterPhysicalConnection()
 *****************************************************************************
 * Unregisters a physical connection between subdevices.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterPhysicalConnection
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PUNKNOWN        FromUnknown,
    IN      ULONG           FromPin,
    IN      PUNKNOWN        ToUnknown,
    IN      ULONG           ToPin
);

/*****************************************************************************
 * PcUnregisterPhysicalConnectionToExternal()
 *****************************************************************************
 * Unregisters a physical connection from a subdevice to an external device.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterPhysicalConnectionToExternal
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PUNKNOWN        FromUnknown,
    IN      ULONG           FromPin,
    IN      PUNICODE_STRING ToString,
    IN      ULONG           ToPin
);

/*****************************************************************************
 * PcUnregisterPhysicalConnectionFromExternal()
 *****************************************************************************
 * Unregisters a physical connection to a subdevice from an external device.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterPhysicalConnectionFromExternal
(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN      PUNICODE_STRING FromString,
    IN      ULONG           FromPin,
    IN      PUNKNOWN        ToUnknown,
    IN      ULONG           ToPin
);

/*****************************************************************************
 * PcGetPhysicalDeviceObject()
 *****************************************************************************
 * Return the physical device object passed in to the portcls in the PcAddAdapterDevice call
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcGetPhysicalDeviceObject
(
    IN  PDEVICE_OBJECT  pDeviceObject,
    OUT PDEVICE_OBJECT  *ppPhysicalObject
);

/*****************************************************************************
 * GetMaxMapRegisters
 *****************************************************************************
 */
ULONG
GetMaxMapRegisters
(
    IN      PVOID           pvDmaChannel
);

/*****************************************************************************
 * IsPortClsRuntimePowerInterfaceEnabled()
 *****************************************************************************
 */
BOOLEAN 
IsPortClsRuntimePowerInterfaceEnabled(
    _In_ PDEVICE_OBJECT DeviceObject
);

/*****************************************************************************
 * WMI Registration routines
 *****************************************************************************
 */
NTSTATUS
RegisterMiniportWMIProvider
(
    IN PDEVICE_OBJECT _pDeviceObject,
    IN PVOID _pMiniPortWmiLibContext
);
NTSTATUS 
UnregisterMiniportWMIProvider
(
    IN PDEVICE_OBJECT _pDeviceObject
);
#endif
