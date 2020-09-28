/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    gpioclxi.h

Abstract:

    This header contains all the definitions and prototypes common across the
    class extension.


Environment:

    Kernel mode

--*/

#pragma once

//
// The GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART macro declares the specified
// struct type. If building the public PDB, it hides private fields.
//
// To use this macro to declare a struct called "MYTYPE":
//
//   //
//   // Include all the fields that should be hidden from the public PDB.
//   //
//
//   typedef struct {...} PRIVATE_MYTYPE_PART;
//
//   //
//   // Include all the fields that should be exposed in the public PDB.
//   //
//
//   typedef struct { int FirstPublicField; ... } PUBLIC_MYTYPE_PART;
//
//   //
//   // This declares a struct called "MYTYPE". For the:
//   //   * Public PDB: This is the struct from the Private PDB but with the
//   //         private part replaced by padding.
//   //   * Private PDB: This consists of the private + public parts.
//   //
//
//   GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(MYTYPE, FirstPublicField);
//
// The public fields are put at the end of the declared struct in order to
// support (public) ANYSIZE_ARRAYs (which must appear at the end). Luckily, no
// struct currently requires a private ANYSIZE_ARRAY.
//

#if defined(GPIO_STRUCTS_SYM_INFO)

//
// Define the macro for the public PDB.
//

#define GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(StructName,FirstPublicField)    \
    typedef struct _PRIVATE_##StructName {                                      \
        PRIVATE_##StructName##_PART;                                            \
        PUBLIC_##StructName##_PART;                                             \
    } PRIVATE_##StructName;                                                     \
                                                                                \
    typedef struct _##StructName {                                              \
        union {                                                                 \
            struct {                                                            \
                UCHAR PaddingForOffset[sizeof(PRIVATE_##StructName##_PART)];    \
                PUBLIC_##StructName##_PART;                                     \
            };                                                                  \
                                                                                \
            UCHAR PaddingForSize[sizeof(PRIVATE_##StructName)];                 \
        };                                                                      \
    } StructName, *P##StructName;                                               \
                                                                                \
    C_ASSERT(FIELD_OFFSET(PUBLIC_##StructName##_PART, FirstPublicField) == 0);  \
    C_ASSERT(FIELD_OFFSET(StructName, FirstPublicField) ==                      \
             FIELD_OFFSET(PRIVATE_##StructName, FirstPublicField));             \
                                                                                \
    C_ASSERT(sizeof(StructName) == sizeof(PRIVATE_##StructName))

#else

//
// Define the macro for the private PDB.
//

#define GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(StructName,FirstPublicField)  \
    typedef struct _##StructName {                                            \
        PRIVATE_##StructName##_PART;                                          \
        PUBLIC_##StructName##_PART;                                           \
    } StructName, *P##StructName

#endif  // defined(GPIO_STRUCTS_SYM_INFO)

#if defined(GPIO_STRUCTS_SYM_INFO)


#undef WDF_DECLARE_CONTEXT_TYPE_WITH_NAME
#define WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(a,b)

#endif  // defined(GPIO_STRUCTS_SYM_INFO)

//
// The signature for the GPIO class extension's device extension.
//

#define GPIO_DEVICE_EXTENSION_SIGNATURE 'OIPG'

//
// Format for the recorder log name.
//

#define GPIO_RECORDER_LOG_NAME_FORMAT "GPIO_%d"

//
// Define the minimum client driver version that GPIO class extension supports.
//

#define GPIO_CLIENT_SUPPORTED_VERSION_MIN  (0x1)

//
// Define the minimum client device information buffer version that GPIO class
// extension supports.
//

#define GPIO_CONTROLLER_BASIC_INFORMATION_VERSION_MIN (0x1)

//
// Define the minimum version for the power information buffer.
//

#define GPIO_BANK_POWER_INFORMATION_OUTPUT_VERSION_MIN (0x1)

//
// Define the minimum version for the interrupt <-> bank mapping information.
//

#define GPIO_INTERRUPT_BINDING_INFORMATION_OUTPUT_VERSION_MIN (0x1)

//
// Define the minimum version for the IOCTL <-> bank mapping information.
//

#define GPIO_IOCTL_BANK_MAPPING_INFORMATION_VERSION_MIN (0x1)

//
// Define the interrupt state history size. On CHK builds builds maintain a
// larger history to aid diagnosis.
//

#if defined(DBG)

#define GPIO_INTERRUPT_HISTORY_SIZE (32)

#else

#define GPIO_INTERRUPT_HISTORY_SIZE (8)

#endif

//
// Define the default noise filter interval (in micro-seconds). It is 500us for
// memory-mapped GPIOs and 1msec for passive-level. The longer interval for
// passive-level GPIOs is to account for the additional scheduling delays in
// that path (need to drop to passive-level, acquire lock etc.)
//

#define GPIO_DEFAULT_NOISE_FILTER_INTERVAL_MEMORY_MAPPED (500)
#define GPIO_DEFAULT_NOISE_FILTER_INTERVAL_PASSIVE (1000)

//
// Define the maximum value that can be specified for noise filter interval
// override via the registry. The maximum interval is chosen to be 1 second
// specified in 100s of micro-second intervals.
//
// Note the minimum interval is bound by the granularity of the high-resolution
// timers, which is currently defined to be 500us.
//

#define GPIO_NOISE_FILTER_INTERVAL_OVERRIDE_MAXIMUM (1 * 1000 * 10) // 1 sec

//
// Macros to allocate and free memory within the GPIO class extension and the
// associated pool tag.
//

#define GPIO_CLX_POOL_TAG 'xlCG'
#define GPIO_CLX_INTERRUPT_POOL_TAG 'IxCG'
#define GPIO_CLX_IO_POOL_TAG 'OxCG'
#define GPIO_CLX_ACPI_EVENT_POOL_TAG 'ExCG'

#define GPIO_ALLOCATE_POOL(Type, Size) \
    ExAllocatePoolWithTag((Type), (Size), GPIO_CLX_POOL_TAG)

#define GPIO_FREE_POOL(Buffer) \
    ExFreePoolWithTag((Buffer), GPIO_CLX_POOL_TAG)

#define GPIO_ALLOCATE_POOL_EX(Type, Size, Tag) \
    ExAllocatePoolWithTag((Type), (Size), Tag)

#define GPIO_FREE_POOL_EX(Buffer, Tag) \
    ExFreePoolWithTag((Buffer), Tag)

//
// Routines to acquire and release the global client driver lock.
//

#define GPIO_ACQUIRE_CLIENT_LOCK() \
    ExAcquireFastMutex(&GpioClientGlobalListLock)

#define GPIO_RELEASE_CLIENT_LOCK() \
    ExReleaseFastMutex(&GpioClientGlobalListLock)

#define GPIO_ASSERT NT_ASSERT

#define GPIO_CLX_VALIDATE_SIGNATURE(Extension) \
    GPIO_ASSERT((Extension)->Signature == GPIO_DEVICE_EXTENSION_SIGNATURE)

//
// Internal flags for GPIO I/O operations
//
// GPIO_IO_FLAG_NONE - No special handling.
//
// GPIO_IO_FLAG_FAST_IO_MODE - Associated I/O operation is handled through 
// the fast I/O interface.
//

#define GPIO_IO_FLAG_NONE 0x0
#define GPIO_IO_FLAG_FAST_IO_MODE 0x1

//
// Define the number of pins above which the long-form APIs will be used.
//

#define GPIO_LONG_PIN_THRESHOLD (64)

//
// Define an unspecified interrupt.
//

#define GPIO_UNSPECIFIED_INTERRUPT (MAX_ULONG)

//
// Define states for work items.
//

#define GPIO_WORKER_NOT_RUNNING 0x0
#define GPIO_WORKER_RUNNING 0x1

//
// Threshold for the maximum number of mask or status clear failures tolerated
// on any pin.
//

#define MASK_UNMASK_FAILURE_COUNT_THRESHOLD (5)
#define STATUS_CLEAR_FAILURE_COUNT_THRESHOLD (5)
#define STATUS_DISCONNECT_FAILURE_COUNT_THRESHOLD (5)
#define RECONFIGURE_FAILURE_COUNT_THRESHOLD (5)

//
// A reconfigure failure is only allowed while the pin state is in
// PossiblyEnhanced mode.
//

#define GPIO_RECONFIGURE_FAILURE_ALLOWED(DebounceContext)                   \
    ((DebounceContext)->Flags.DebounceModel == DebounceModelPossiblyEnhanced)

//
// Define a GPIO registry flag to force all work onto the emergency worker
// thread.
//

#define GPIO_REGISTRY_FORCE_EMERGENCY_WORKER_KEY L"ForceEmergencyWorker"

//
// Define a GPIO registry flag to treat "interrupt + output" as valid.
//

#define GPIO_REGISTRY_ALLOW_INTERRUPT_OUTPUT_KEY L"AllowInterruptOutput"

//
// Define a GPIO registry flag to periodically check that hardware and
// software are in sync concerning which interrupts are enabled.
//

#define GPIO_REGISTRY_CHECK_ENABLED_INTERRUPTS_KEY L"CheckEnabledInterrupts"

//
// Define a GPIO registry flag to force a specific debounce model. This allows
// various debouncing models to be forced easily for testing and diagnosis.
//

#define GPIO_REGISTRY_DEBOUNCE_MODEL_OVERRIDE_KEY L"DebounceModelOverride"

//
// Define a GPIO registry override for the noise filtering interval. This
// may help with testing and diagnosis of debouncing issues. This value is in
// 100s of micro-second intervals (i.e. 500us will be specified as 0x5).
//
// Note the override applies to both on-SoC and off-SoC GPIO controllers.
//

#define GPIO_REGISTRY_DEBOUNCE_NOISE_INTERVAL_KEY L"DebounceNoiseIntervalOverride"

#if !defined(NON_PAGED_PASSIVE_LEVEL_CODE)
#define NON_PAGED_PASSIVE_LEVEL_CODE() \
    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL)
#endif

//
// Definitions for GPIO controller's PNP state
//

typedef enum _GPIO_DEVICE_STATE {
    DEVICE_STATE_UNINITIALIZING = 0,
    DEVICE_STATE_UNINITIALIZE_FAILED,
    DEVICE_STATE_UNINITIALIZED,
    DEVICE_STATE_INITIALIZING,
    DEVICE_STATE_INITIALIZE_FAILED,
    DEVICE_STATE_INITIALIZED,
    DEVICE_STATE_STARTING,
    DEVICE_STATE_START_FAILED,
    DEVICE_STATE_STARTED,
    DEVICE_STATE_STOPPING,
    DEVICE_STATE_STOP_FAILED,
    DEVICE_STATE_STOPPED,
    DEVICE_STATE_SUPRISE_REMOVING,
    DEVICE_STATE_REMOVING,
    DEVICE_STATE_REMOVED
} GPIO_DEVICE_STATE;

//
// Defines the various types of formatting the GPIO class extension will do on
// IO requests prior to sending it to the client driver.
//

typedef enum _GPIO_IO_FORMAT_TYPE {
    IoFormatTypeDefault = 0,
    IoFormatTypeSetClearBitmasks,
    IoFormatTypeMaximum
} GPIO_IO_FORMAT_TYPE, *PGPIO_IO_FORMAT_TYPE;


//
// Define the various states in the interrupt servicing state machine.
//

typedef enum _GPIO_INTERRUPT_STATE {
    InterruptStateInvalid = 0,
    InterruptStateOnInitialInterrupt,
    InterruptStateCheckValidInterrupt,
    InterruptStateDebounceInterrupt,
    InterruptStateProcessActiveBoth,
    InterruptStateProcessAcpiEvent,
    InterruptStateMaskInterruptActiveBoth,
    InterruptStateReconfigureActiveBothInterrupt,
    InterruptStateProcessLevelEdgeInterrupt,
    InterruptStateInvokeTargetIsr,
    InterruptStateClearStatusRegister,
    InterruptStateClearStatusOnInvalidInterrupt,
    InterruptStateProcessDebounceNotification,
    InterruptStateMaximum
} GPIO_INTERRUPT_STATE, *PGPIO_INTERRUPT_STATE;


//
// Define input events for interrupt servicing state machine.
//

typedef enum _GPIO_INTERRUPT_EVENT_TYPE {

    //
    // Secondary event types begin here.
    //
    // InterruptEventReportValidInterrupt, InterruptEventReportNoiseInterrupt,
    // and InterruptEventReportDebounceCompletion are valid when the
    // primary event is InterruptEventOnDebounceEngineNotification.
    //

    InterruptEventNone = 0x0,
    InterruptEventRequestCompletion,
    InterruptEventReportValidInterrupt,
    InterruptEventReportSpuriousInterrupt,
    InterruptEventReportDebounceCompletion,

    //
    // Primary event types begin here
    //

    InterruptEventOnInterrupt,
    InterruptEventOnReplayInterrupt,
    InterruptEventOnDebounceEngineNotification,
    InterruptEventMaximum
} GPIO_INTERRUPT_EVENT_TYPE, *PGPIO_INTERRUPT_EVENT_TYPE;

//
// Define output actions for interrupt servicing state machine.
//

typedef enum _GPIO_INTERRUPT_ACTION_TYPE {
    InterruptActionNone = 0x0,
    InterruptActionCheckValidInterrupt,
    InterruptActionClearStatusRegister,
    InterruptActionDebounceInterrupt,
    InterruptActionMaskInterrupt,
    InterruptActionReconfigureActiveBothInterrupt,
    InterruptActionInvokeTargetIsr,
    InterruptActionMaximum
} GPIO_INTERRUPT_ACTION_TYPE, *PGPIO_INTERRUPT_ACTION_TYPE;

#pragma warning(push)
#pragma warning(disable:4214) /* nonstandard extension used: bit field types other then int */
#pragma warning(disable:4201) /* nonstandard extension used: nameless struct/union */

//
// Data structure to store information about each registered GPIO instance.
//

typedef struct _PRIVATE_GPIO_HUB_LIST_ENTRY_PART {

    //
    // Reference to the name of the target device.
    //
    // N.B. This should be treated as a read-only string and should not be
    //      freed.
    //

    PCUNICODE_STRING TargetName;

    //
    // Reference to the BIOS name of the target device
    //
    // N.B. This should be treated as a read-only string and should not be
    //      freed.
    //

    PCUNICODE_STRING BiosName;

    //
    // A count of the active references to an entry (i.e. number of
    // callers that have acquired a shared reference to an entry).
    //

    volatile LONG BusyCount;

    //
    // A count of number of exclusive waiters. (i.e. number of callers that
    // are waiting to acquire an exclusive reference to the provider).
    //

    volatile LONG ExclusiveWaiterCount;

    //
    // An event that gets signalled when the entry is *potentially* not in use
    // and thus available for exclusive (potentially). The entry might become
    // busy again before an exclusive waiter gets around to check the busy
    // count.
    //

    KEVENT Event;

    //
    // The list used to track entries that need to have their notification
    // event signalled. Note this list is manipulated under the the global
    // signal and interrupt replay list lock.
    //

    LIST_ENTRY SignalListEntry;

    //
    // The list used to track entries that need to have their interrupt
    // service routine called due to a replay request for a controller that
    // was not connected to a primary interrupt. Note this list is manipulated
    // under the the global signal and interrupt replay list lock.
    //
    // This is used for synthetic test controllers only, since real controllers
    // that handle secondary interrupts must be connected to a primary
    // interrupt.
    //

    LIST_ENTRY ReplayListEntry;

} PRIVATE_GPIO_HUB_LIST_ENTRY_PART;

typedef struct _PUBLIC_GPIO_HUB_LIST_ENTRY_PART {

    //
    // The list pointers on the master list.
    //

    LIST_ENTRY ListEntry;

    //
    // The context to be supplied when calling GPIO client driver handlers.
    //
    // This is actually a PDEVICE_EXTENSION.
    //

    PVOID Context;

} PUBLIC_GPIO_HUB_LIST_ENTRY_PART;

GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(GPIO_HUB_LIST_ENTRY, ListEntry);

typedef union _GPIO_PIN_MODE {
    struct {
        ULONG Input: 1;     // Set if the pin is enabled for input.
        ULONG Output: 1;    // Set if the pin is enabled for output.
        ULONG Interrupt: 1;  // Set if the pin is enabled for interrupt.
        ULONG FunctionConfig:1;  // Set if the pin is enabled for func config.
        ULONG AcpiEvent: 1; // Set if the pin is meant for ACPI events.
        ULONG Wake: 1;
        ULONG RunEVTMethod: 1; // Set if _EVT method needs to be run
                                // valid only if AcpiEvent == 1

        ULONG EmulateDebounce: 1;   // valid only if interrupt
        ULONG EmulateActiveBoth: 1; // valid only if interrupt
        ULONG ExclusiveMode: 1;
        ULONG PreserveConfiguration: 1;        // valid only for IO pins.
        ULONG Reserved: 21;
    };

    ULONG AsULONG;
} GPIO_PIN_MODE, *PGPIO_PIN_MODE;

//
// Define the sub-state of a pin connection
//

typedef enum _FUNCTION_CONFIG_PIN_STATE {
    PinUnreserved,
    PinReserved,
    PinCommitted
} FUNCTION_CONFIG_PIN_STATE, *PFUNCTION_CONFIG_PIN_STATE;

C_ASSERT(PinReserved == PinUnreserved + 1);
C_ASSERT(PinCommitted == PinReserved + 1);

typedef struct _PRIVATE_GPIO_PIN_INFORMATION_ENTRY_PART {

    //
    // Tracks the current state within the interrupt state machine for the
    // pin. The state is protected using the bank interrupt lock for both
    // on-SoC and off-SoC GPIO controllers.
    //

    GPIO_INTERRUPT_STATE State;

    //
    // Whether the intial polarity of pin should be ActiveHigh (according to the
    // ACPI _DSM method) or ActiveLow (default).
    //
    // This field is only read if the Polarity is ActiveHigh and the controller
    // emulates ActiveBoth.
    //

    BOOLEAN EmulatedActiveBothInitialPolarityIsHigh;

    //
    // The programmed mode and polarity for the interrupt. This could
    // be different from the actual mode and polarity values only in case of
    // emulated ActiveBoth interrupts (Polarity will be ActiveBoth
    // but CurrentInterruptPolarity will be either ActiveLow or ActiveHigh).
    //

    KINTERRUPT_MODE CurrentInterruptMode;
    KINTERRUPT_POLARITY CurrentInterruptPolarity;

    //
    // The allocated contexts for debounce and noise filter interval timers.
    //

    PVOID DebounceTimerContext;
    PVOID NoiseFilterTimerContext;

    //
    // The worker item for processing debounce and noise timer expirations at
    // passive-level.
    //
    // N.B. The worker is only created for off-SoC GPIO controllers. It is
    //      NULL otherwise.
    //

    WDFWORKITEM DebounceNoiseTimerWorker;

    //
    // Define the per-pin context for interrupt debouncing state machine.
    //

    GPIO_DEBOUNCE_PIN_CONTEXT DebounceContext;

    //
    // Tracks how many times an operation has failed consecutively in a way
    // that affected functionality, after the respective pin was last
    // connected.  Failures that did not affect functionality (e.g. mask
    // failures for pins that are to be immediately unmasked afterwards) are
    // not tracked.
    //
    // If an operation succeeds, its count is reset to 0, except for the
    // disconnect operation because once that succeeds, the operation cannot
    // be re-issued until the pin is re-connected.
    //
    // Since these operations never fail on on-SoC controllers, these variables
    // are only used by off-SoC controllers. They are protected using the bank
    // lock.
    //

    LONG MaskUnmaskFailureCount;
    LONG StatusClearFailureCount;
    LONG DisconnectFailureCount;

    //
    // Tracks, for debugging purposes, whether a particular operation has
    // _ever_ failed in a way that affected functionality. Failures that did
    // not affect functionality (e.g. mask failures for pins that are to be
    // immediately unmasked afterwards) are not tracked.
    //
    // Once a bit is set, it is never reset even if a pin is disconnected and
    // re-connected.
    //
    // Since these operations never fail on on-SoC controllers, there variables
    // are only used by off-SoC controllers. They are protected using the bank
    // lock.
    //

    struct {
        ULONG MaskUnmaskEverFailed:1;
        ULONG StatusClearEverFailed:1;
        ULONG DisconnectEverFailed:1;
    };

    //
    // Tracks the interrupt state history state for the pin. This is protected
    // using the same lock as the state.
    //

    GPIO_INTERRUPT_STATE PreviousState[GPIO_INTERRUPT_HISTORY_SIZE];

    UCHAR StateIndex;

    //
    // Tracks if the last interrupt was valid or not. Refer to
    // GpiopDebounceLogSpuriousInterrupt() for description on how this flag is
    // used. It is only relied upon for debounce interrupts using enhanced
    // debouncing model.
    //
    // This flag is protected by bank-interrupt lock for memory-mapped GPIOs
    // and passive-level bank lock for off-SoC GPIOs.
    //

    BOOLEAN LastInterruptWasValid;

} PRIVATE_GPIO_PIN_INFORMATION_ENTRY_PART;

C_ASSERT(GPIO_INTERRUPT_HISTORY_SIZE <= 0xFF);

typedef struct _PUBLIC_GPIO_PIN_INFORMATION_ENTRY_PART {

    //
    // The mode in which the pin is currently configured in.
    //

    GPIO_PIN_MODE Mode;

    //
    // The current interrupt mask count for the pin.
    // This includes pending mask and unmask requests.
    //
    //   <0 = should not happen
    //    0 = unmasked (default after enabling the interrupt)
    //   >0 = masked (if you make N mask requests, you need to make N unmask
    //                requests to actually unmask the interrupt)
    //
    // This field is only valid after enabling the interrupt (Virq != 0).
    //
    // This field is not protected by any locks.
    //

    volatile LONG InterruptMaskCount;

    //
    // The number of times a particular pin is opened for input and output.
    //

    ULONG InputEnableCount;
    ULONG OutputEnableCount;

    //
    // This field holds the connection state of the pin in question.
    // Currently this enum can be PinUnreserved and PinReserved and is
    // primarily used for the two-phased function config pin connections.
    //

    FUNCTION_CONFIG_PIN_STATE FunctionConfigPinState;

    //
    // This field holds whether the pin has been actually connected for output
    // by the controller or not.
    //

    BOOLEAN OutputConnected;

    //
    // The debounce timeout specified in the GPIO interrupt descriptor. This
    // value is specified in the 100s of milli-second intervals.
    //

    ULONG DebounceTimeout;

    //
    // The noise filter interval. This value is specified in in micro-seconds
    // (minimum and default set to 500us).
    //

    ULONG64 NoiseFilterTimeout;

    //
    // The pull configuration for the line.
    //

    UCHAR PullConfiguration;

    //
    // The following fields are valid only if the pin is configured for
    // interrupt.
    //

    KINTERRUPT_MODE InterruptMode;
    KINTERRUPT_POLARITY Polarity;

    //
    // The callback context when invoking the ISR.
    //

    ULONG_PTR InterruptCallbackContext;
    volatile ULONG Virq;

    //
    // The function number (if pin enabled for function config).
    //

    ULONG FunctionNumber;

    //
    // The following fields are valid only if the pin is configured for IO.
    //
    // N.B. Since a pin can be configured as Interrupt+Output, both sets of
    //      fields (for Interrupt and for IO) are required for each pin.
    //

    USHORT DriveStrength;

} PUBLIC_GPIO_PIN_INFORMATION_ENTRY_PART;

GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(GPIO_PIN_INFORMATION_ENTRY, Mode);

//
// Define a structure to store all the IO and interrupt related information.
//

typedef struct _PRIVATE_GPIO_INTERRUPT_DATA_PART {

    //
    // The GPIO controller's affinity.
    //

    GROUP_AFFINITY Affinity;

    //
    // Worker item and associated flags for interrupt processing.
    //
    // N.B. WDF automatically creates interrupt DPC based on
    //      WDF_INTERRUPT_CONFIG_INIT.
    //
    // N.B. These fields are not used for interrupt processing per-se but
    //      leaving them in as they may be required for mask processing (when
    //      request arrives at DPC or higher).
    //

    WDFWORKITEM InterruptWorker;
    LONG InterruptWorkerRunning;

    //
    // Set if there is any deferred interrupt activity to be performed.
    //

    volatile LONG PendingInterruptActions;

    //
    // Set if there is a mask request pending on a given pin on this bank.
    // The bit is marked if the request has not yet been completed at the
    // controller or if a previous request failed and needs to be retried.
    //
    // N.B. InterruptMaskCount in the pin information table accurately
    //      reflects the pin state and should be used to determined whether a
    //      pin is supposed to be masked or unmasked at any given point.
    //

    volatile ULONG64 PendingInterruptMask;

    //
    // Set if there is an unmask request pending on a given pin on this bank.
    // The bit is marked if the request has not yet been completed at the
    // controller or if a previous request failed and needs to be retried.
    //
    // N.B. InterruptMaskCount in the pin information table accurately
    //      reflects the pin state and should be used to determined whether a
    //      pin is supposed to be masked or unmasked at any given point.
    //

    volatile ULONG64 PendingInterruptUnmask;

    //
    // Status register contains the set of active interrupts.
    //

    ULONG64 StatusRegister;

    //
    // Status bits for interrupts which need to be replayed.
    //
    // N.B. This data is accessed at any IRQL and therefore can be accessed at
    //      HIGH_LEVEL.  It is not protected by the bank interrupt lock.
    //

    volatile LONG64 ReplayRegister;

    //
    // A bit is set if the pin was masked with the
    // GPIO_REQUEST_FLAG_SERVICING_DEFERRED flag.  This delays status clears (as
    // registered in PendingStatusClearMask) until an unmask happens with the
    // GPIO_REQUEST_FLAG_SERVICING_COMPLETE flag, which is after the pin's
    // interrupt service routine has run.
    //
    // If you try to clear the status bit for a level-triggered interrupt
    // before the device has been quiesced (i.e. before the pin's interrupt
    // service routine has completed), GPIO controllers could re-assert the
    // line, resulting in an interrupt storm or a spurious second interrupt.
    //

    ULONG64 DelayPendingStatusClearMask;

    //
    // Set if there is a status clear request pending on a given pin on this
    // bank.
    //
    // A status clear is known as an End of Interrupt (EOI) signal, which
    // prevents the interrupt from being re-asserted.
    //
    // N.B. See DelayPendingStatusClearMask.
    //

    ULONG64 PendingStatusClearMask;

    //
    // Set if there is a disable request pending on a given pin on this bank.
    // This includes if a failed disable needs to be retried.
    //
    // This is always written to under the bank lock.
    // This can be read under the bank lock or interrupt lock.
    //

    ULONG64 PendingDisconnect;

    //
    // A bitmask of which pins are actively being debounced at this point.
    //

    ULONG64 ActiveDebounceMask;

    //
    // A bit mask of pins for which debounce timers need to be started.
    //

    ULONG64 DebounceTimerMask;

    //
    // A bit mask of pins for which noise filter timers need to be started.
    //

    ULONG64 NoiseTimerMask;

    //
    // A bit mask of pins for which interrupt mode/polarity needs to be flipped.
    // Bits in this field are set for emulated debouncing and emulated
    // ActiveBoth cases.
    //

    ULONG64 PendingReconfigure;

    //
    // A bit mask of pins for which active both servicing is still pending
    // (which a reconfiguration request). Bits in this field are only set for
    // emulated ActiveBoth cases.
    //

    ULONG64 PendingActiveBoth;

#ifdef DEBOUNCE_ELAPSED_TIME_ACCOUNTING

    //
    // The (approximate) time at which this interrupt fired.
    //

    volatile LONG64 LastInterruptTime;

#endif

} PRIVATE_GPIO_INTERRUPT_DATA_PART;

typedef struct _PUBLIC_GPIO_INTERRUPT_DATA_PART {

    //
    // The GPIO controller's GSIV, IRQL, and interrupt mode.
    //

    ULONG Gsiv;
    KIRQL Irql;
    KIRQL SynchronizeIrql;
    KINTERRUPT_MODE InterruptMode;
    KINTERRUPT_POLARITY Polarity;

    //
    // Enable register holds the set of pins that are currently enabled for
    // interrupt.
    //
    // This is always written to under the bank lock.
    // This can be read under the bank lock or interrupt lock.
    //

    ULONG64 EnableRegister;

    //
    // Holds the set of pins that are currently masked.
    // This includes pending mask and unmask requests.
    //
    // For on-SoC controllers, this is protected by the interrupt lock.
    //
    // For off-SoC controllers, this is written to under the interrupt lock.
    //

    ULONG64 InterruptMask;

} PUBLIC_GPIO_INTERRUPT_DATA_PART;

GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(GPIO_INTERRUPT_DATA, Gsiv);

//
// Define a structure to hold all the ACPI eventing related information.
//

typedef struct _GPIO_ACPI_EVENT_DATA {
    WDFDPC EventDpc;
    WORK_QUEUE_ITEM EventWdmWorkItem;

    //
    // Indicates if GpiopProcessAcpiEventsWorker() was scheduled but has not
    // finished invoking pending ACPI eventing methods.
    //
    // This is set to either GPIO_WORKER_NOT_RUNNING (default) or
    // GPIO_WORKER_RUNNING.
    //
    // This is manipulated lock-free using interlocked operations.
    //

    LONG EventWorkerRunning;

    //
    // Used to track the number of outstanding calls to
    // GpiopProcessAcpiEventsWorker() and to allow GpiopFlushPendingAcpiEvents()
    // to wait until all outstanding calls have completed.
    //
    // The "run-down" aspect of it is not actually used except to detect the
    // impossible situation where event interrupts somehow fire after they
    // have been deep-masked (after GpiopFlushPendingAcpiEvents() and before
    // GpiopPrepareForAcpiEvents()).
    //

    EX_RUNDOWN_REF EventWorkerReferences;

    //
    // Set to the name of the ACPI eventing method that is about to be run.
    // This is only read by GpiopEnableDisableAcpiEventsWatchdog().
    //

    volatile LONG LastAcpiMethod;

    //
    // The DescriptorList, DescriptorListSize, PreviousStateFailed, and
    // ResourcesAllocated flags are protected by the phase mutex event.
    //

    KEVENT MutexEvent;
    PIO_RESOURCE_LIST DescriptorList;
    ULONG DescriptorListSize;

    //
    // Passive-level lock for synchronizing access ACPI event data. This lock
    // is only used for serially-accessed GPIO controllers (i.e. controllers
    // that can be accessed at passive-level only).
    //
    // Acquire/Release ordering: acquire GPIO bank lock and then event lock.
    //

    FAST_MUTEX EventLock;

    //
    // Interrupt-level lock for synchronizing access ACPI event data. This lock
    // is only used for memory-mapped GPIO controllers (i.e. controllers that
    // can be accessed at DIRQL level).
    //
    // Acquire/Release ordering: acquire GPIO bank interrupt lock and then
    // event interrupt lock.
    //
    // N.B. This is defined as KSPIN_LOCK rather than a WDF interrupt lock as it
    //      needs to be acquired at > DPC level (from ISR), which WDF APIs do
    //      not expose.
    //

    KSPIN_LOCK EventInterruptLock;

    //
    // The ID for the thread owning the passive.
    //

    PRKTHREAD LockOwner;

    //
    // Flag to indicate whether _EVT is defined under this controller or not.
    //

    BOOLEAN EVTMethodDefined;


    BOOLEAN ELEObjectDefined;

    //
    // Edge/Level Pin list buffer. This buffer contains a list of pins for
    // which there is either _Exx or _Lxx or _EVT defined.
    //
    //

    PULONG EdgeLevelPins;


    ULONG ELEEdgePinCount;
    ULONG ELELevelPinCount;

    //
    // Count of pins that are edge/level (i.e. _Exx/_Lxx) in the ACPI namespace.
    //

    ULONG NamespaceEdgePinCount;
    ULONG NamespaceLevelPinCount;

    //
    // The PendingEventsBitmap is protected using the interrupt lock for
    // memory-mapped GPIOs and passive event lock for serially accessible GPIOs.
    //

    RTL_BITMAP PendingEventsBitmap;

    //
    // Callback handle for GUID_ACPI_EVENTING_DISABLED notification.
    //

    PVOID PowerSettingCallbackHandle;

    //
    // A flag that indicates non-wake ACPI eventing interrupts are disabled.
    //

    ULONG NonWakeEventingDisabled;

    //
    // Flags to keep track of various failure states.
    //

    struct {
        BOOLEAN PreviousStateFailed: 1;
        BOOLEAN ResourcesAllocated: 1;
        BOOLEAN Reserved:6;
    };

    //
    // Where GpiopEnableDisableAcpiEvents() is up to.
    // This is only read by GpiopEnableDisableAcpiEventsWatchdog().
    //

    volatile LONG EnableDisableStage;

    //
    // Set to GSIV of an eventing pin that is about to be enabled/disabled.
    // This is only read by GpiopEnableDisableAcpiEventsWatchdog().
    //

    volatile LONG EnableDisableVector;

} GPIO_ACPI_EVENT_DATA, *PGPIO_ACPI_EVENT_DATA;

typedef enum _FSTATE {
    FStateF0 = 0x0,
    FStateF1 = 0x1
} FSTATE, *PFSTATE;

//
// Define the f-states supported by the GPIO class extension and the total
// number of f-states supported per component. Currently only two states are
// supported per component.
//
// F0, which will map to a fully-powered/ON state;
// F1, which will map to a fully-off state where:
//     - All hardware context is lost
//     - The GPIO component/bank cannot generate any interrupts to the CPU
//

#define GPIO_TOTAL_F_STATES (2)

//
// Latency in 100-ns units. We specify a low value of 1 us.
//
// N.B. This is a temporary value. The actual value must be specified by the
//      GPIO client driver.
//

#define GPIO_TRANSITION_LATENCY_F1 (10)

//
// Residency requirement in 100-ns units. We specify a low value of 1 us.
//
// N.B. This is a temporary value. The actual value must be specified by the
//      GPIO client driver.
//

#define GPIO_RESIDENCY_REQUIREMENT_F1 (10)

//
// Define a structure to hold all the power related information for a bank.
//
// The Active, FState, TargetFState and work item related fields are modified
// under the protection of the bank power lock (which is currently defined to
// be the same as the controller-wide power lock).
//

typedef struct _PRIVATE_GPIO_BANK_POWER_DATA_PART {

    //
    // This flag tracks whether we have saved the GPIO bank's hardware context
    // for a non-critical F-state transition, but have not restored it yet.
    //

    BOOLEAN RegularContextSaved;

    //
    // This flag tracks whether we have saved the GPIO bank's hardware context
    // critical F-state transition, but have not restored it yet.
    //

    BOOLEAN CriticalContextSaved;

} PRIVATE_GPIO_BANK_POWER_DATA_PART;

typedef struct _PUBLIC_GPIO_BANK_POWER_DATA_PART {

    //
    // This flag tracks whether the GPIO bank is active or not. The bank
    // registers are only accessible when it is marked as active.
    //

    BOOLEAN IsActive;

    //
    // Holds the current F-state for the GPIO bank.
    //

    FSTATE FState;

    //
    // Holds the maximum F-state supported by the GPIO bank.
    //

    FSTATE MaximumFState;

    //
    // The residency and latency values for F1 state specified by the client.
    //

    PO_FX_COMPONENT_IDLE_STATE F1StateParameters;

} PUBLIC_GPIO_BANK_POWER_DATA_PART;

GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(GPIO_BANK_POWER_DATA, IsActive);

struct _DEVICE_EXTENSION;
typedef struct _DEVICE_EXTENSION DEVICE_EXTENSION, *PDEVICE_EXTENSION;

typedef struct _PRIVATE_GPIO_BANK_ENTRY_PART {

    //
    // The WDF interrupt object representing the GPIO bank's interrupt.
    //
    // Refer to comment above describing ControllerLock for more information.
    //

    WDFINTERRUPT BankInterrupt;

    //
    // The WDF interrupt object representing the GPIO bank's passive-level
    // interrupt. This is only used for off-SoC GPIOs.
    //

    WDFINTERRUPT BankPassiveInterrupt;

    //
    // Index of the interrupt resource for this bank in the PnP raw &
    // translated resource lists.
    //

    ULONG ResourceIndex;

    //
    // The spin lock used to synchronize ISR invocation on this bank. Note
    // this is per-bank synchronization only (i.e., interrupts for other
    // bank still happen).
    //

    WDFSPINLOCK BankInterruptLock;

    //
    // The ID for the thread owning the interrupt lock. This is normally the
    // thread servicing the interrupt but could be some other thread if the
    // lock is explicitly acquired.
    //

    volatile PRKTHREAD InterruptLockOwner;

    //
    // A flag indicating whether the interrupt lock is owned by the client
    // driver or not.
    //

    volatile BOOLEAN InterruptLockClientOwned;

    //
    // Passive-level lock for synchronizing access to a bank.
    //

    WDFWAITLOCK PassiveLock;

    //
    // The ID for the thread owning the passive-level bank lock.
    //

    volatile PRKTHREAD PassiveLockOwner;

    //
    // A flag indicating whether the passive-level bank lock is owned by the
    // client driver or not.
    //

    volatile BOOLEAN PassiveLockClientOwned;

    //
    // If the interrupt for this bank is being actively serviced, then this
    // field supplies the ID for the thread that is currently servicing the
    // interrupt.
    //

    volatile PKTHREAD InterruptThread;

    //
    // This is a count of number of emulated-debounced interrupts actively
    // connected.
    //

    ULONG DebounceInterruptCount;

    //
    // The per-bank secondary I/O queue.
    //

    WDFQUEUE IoQueue;

    //
    // Chain of file-context that have issue connected IO pins on this bank.
    // This chain is used to cleanup connects when the device is going away
    // (e.g. surprise-removed, shutdown etc.).
    //

    LIST_ENTRY ConnectedPinsQueue;

    //
    // The per-bank Activity ID included in ETW events.
    //

    GUID ActivityId;

} PRIVATE_GPIO_BANK_ENTRY_PART;

typedef struct _PUBLIC_GPIO_BANK_ENTRY_PART {

    //
    // ID for this bank.
    //

    BANK_ID BankId;

    //
    // (Back) Pointer to the GPIO device extension.
    //

    PDEVICE_EXTENSION GpioExtension;

    //
    // Holds all the interrupt related information.
    //

    GPIO_INTERRUPT_DATA InterruptData;

    //
    // Holds all the bank power-related information.
    //

    GPIO_BANK_POWER_DATA PowerData;

    //
    // The GPIO pin table which tracks the state for each GPIO pin (per bank).
    // This is an array of pointers to pin entries.
    //
    // The array and the entries themselves are allocated on-demand -- when
    // connecting a pin for I/O or interrupts and, during bank creation, when
    // marking a pin's EmulatedActiveBothInitialPolarityIsHigh field.  All
    // such allocation must be synchronized in the following way:
    //
    //     1. By acquiring an allocation lock.  Currently, this is the
    //        PASSIVE_LEVEL bank lock.
    //
    //     2. If the pin is to be connected for interrupts, it must be
    //        allocated before:
    //
    //        a) GPIO_HUB_VIRQ_LIST_ENTRY::Cache is set to the pin number:
    //           Code assumes that once the cache is set, the allocation for
    //           that pin that has occurred.
    //
    //        b) the corresponding bit in the owning bank's.
    //           GPIO_INTERRUPT_DATA::EnableRegister is set: The ISR expects
    //           that the allocation has occurred.
    //
    // In all cases (allocation and non-allocation cases), all accesses to this
    // data must be performed using the GPIO_*_DYNAMIC_PIN_TABLE_POINTER() and
    // GPIO_*_PIN_ENTRY_POINTER() macros below.
    //
    // There are DEVICE_EXTENSION->PinsPerBank number of pointers except for
    // the last bank, which may have a lesser number of pointers.
    //
    // For simplicitly, the data is only de-allocated when the device extension
    // is destroyed.
    //

    PIN_NUMBER PinCount;
    __field_ecount(PinCount) PGPIO_PIN_INFORMATION_ENTRY *DynamicPinTable;

} PUBLIC_GPIO_BANK_ENTRY_PART;

#define GPIO_READ_DYNAMIC_PIN_TABLE_POINTER(GpioBank)          \
    InterlockedCompareExchangePointer(                         \
        (PVOID volatile *)&(GpioBank)->DynamicPinTable, 0, 0)

#define GPIO_WRITE_DYNAMIC_PIN_TABLE_POINTER(GpioBank,PinTable)  \
    do                                                           \
    {                                                            \
        if (InterlockedExchangePointer(                          \
                (PVOID volatile *)&(GpioBank)->DynamicPinTable,  \
                (PinTable)) != NULL) {                           \
                                                                 \
          GPIO_ASSERT(FALSE);                                    \
                                                                 \
        }                                                        \
    } WHILE(FALSE)

#define GPIO_READ_PIN_ENTRY_POINTER(DynamicPinTable,RelativePin)     \
    InterlockedCompareExchangePointer(                               \
        (PVOID volatile *)&((DynamicPinTable)[RelativePin]), 0, 0)

#define GPIO_WRITE_PIN_ENTRY_POINTER(DynamicPinTable,RelativePin,PinEntry)  \
    do                                                                      \
    {                                                                       \
        if (InterlockedExchangePointer(                                     \
                (PVOID volatile *)&((DynamicPinTable)[RelativePin]),        \
                (PinEntry)) != NULL) {                                      \
                                                                            \
            GPIO_ASSERT(FALSE);                                             \
                                                                            \
        }                                                                   \
    } WHILE(FALSE)

GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(GPIO_BANK_ENTRY, BankId);

//
// Define error flags that are tracked on a controller-wide basis.
//

typedef union _CONTROLLER_ERROR_FLAGS {
    struct {
        ULONG InterruptMaskFailure: 1;
        ULONG InterruptUnmaskFailure: 1;
        ULONG PowerReferenceLeaked: 1;
        ULONG ParentInterruptMaskedDueToMaskFailure: 1;
        ULONG ParentInterruptMaskedDueToStatusClearFailure: 1;
        ULONG ParentInterruptMaskedDueToDisconnectFailure: 1;
        ULONG Reserved: 26;
    };

    ULONG AsULONG;
} CONTROLLER_ERROR_FLAGS, *PCONTROLLER_ERROR_FLAGS;

//
// The GPIO class extension's device extension.
//

typedef struct _PRIVATE_DEVICE_EXTENSION_PART {

    //
    // Tracks the GPIO controller's power state (WDF_POWER_DEVICE_STATE).
    //

    volatile LONG DevicePowerState;

    struct {

        //
        // A flag that indicates whether this device represents the hub device
        // or not.
        //

        USHORT IsHubDevice: 1;

        //
        // A flag that indicates whether the client supplied an internal
        // registration packet.
        //

        USHORT IsInternalClient: 1;
        USHORT Reserved: 14;
    };

    //
    // Holds the synchronization IRQL for the controller (i.e. maximum IRQL
    // across all controller interrupts).
    //

    KIRQL SynchronizationIrql;

    //
    // This fields holds the GSIV for the controller interrupt that corresponds
    // to the synchronization IRQL. This field is only used to determine the
    // IRQL to be assigned to devices behind this GPIO controller.
    //

    ULONG BaseInterruptGsiv;

    //
    // Holds all the ACPI event related data.
    //

    GPIO_ACPI_EVENT_DATA EventData;

    //
    // An I/O target to ourself. This self-target allows interrupt-related
    // requests can be funneled through WDF I/O queues. This allows interrupt
    // related requests to be serailzed with I/O requests and also
    // synchronize them with WDF power engine.
    //

    WDFIOTARGET SelfIoTarget;

    //
    // An I/O target to the resource hub.
    //

    WDFIOTARGET ResourceHubTarget;

    //
    // Handle to power manager for runtime controller power management.
    //

    POHANDLE PowerHandle;

    //
    // The spin lock used to protect access to device powering down flag.
    //

    WDFSPINLOCK PowerDownLock;

    //
    // A flag that tracks whether device is being powered down.
    //

    BOOLEAN DevicePoweringDown;

    //
    // A boolean flag that tracks whether we should handle future PoFx
    // callbacks as NOPs, during certain cases where the device is being torn
    // down before the device has unregistered with PoFx. This flag will become
    // unnecessary after WDF supports integration with PoFx for multiple
    // components & multiple f-states, and the GPIO class extension opts in to
    // that support.
    //
    // The cases of interest are:
    //
    //     1. D0->D3: WDF EvtDeviceD0Exit callback fails
    //
    //     2. D0->D3->D0:
    //        a) WDF EvtDeviceD0Entry callback fails
    //        b) WDF bank interrupt re-connect, for when
    //           WDF_INTERRUPT_CONFIG::ReportInactiveOnPowerDown defaults to
    //           WdfFalse
    //
    //
    //        c) Other cases that the code sadly doesn't handle right now
    //
    // After such a failure, WDF:
    //
    //     1. Initiates a teardown of the WDF device:
    //        - The banks' WDF interrupt object(s) will be disconnected
    //        - Calls to WdfStopIdle()/WdfResumeIdle() will fail
    //        - Eventually, EvtDeviceReleaseHardware() will be called, after
    //          which the interrupt object(s) will be freed
    //
    //        So it is not safe to manipulate interrupt object(s), or call
    //        WdfStopIdle() after WdfResumeIdle() (or vice-versa) without
    //        checking that the first call succeeded (which in this teardown
    //        case will always fail).
    //
    //     2. Calls IoInvalidateDeviceState(), which causes PnP to activate the
    //        device via PoFx callbacks (even though the device should be torn
    //        down) to send a Surprise Remove IRP to WDF. The PoFx callback
    //        races with 1.
    //
    // Unrelated system events can also cause PnP to activate the device via
    // PoFx, and race with 1. Therefore, the current solution of ignoring PoFx
    // callbacks is more general than using the following, which only fixes the
    // race between 2. and 1.
    //
    //    WdfDeviceInitSetReleaseHardwareOrderOnFailure(
    //       ...,
    //       WdfReleaseHardwareOrderOnFailureAfterDescendants)
    //
    //    [this tells WDF to hold off on calling EvtDeviceReleaseHardware()
    //        until it receives the Surprise Remove IRP]
    //
    // This flag is set in WDF callbacks and read from PoFx callbacks, which
    // may be on different threads.
    //

    LONG IgnorePoFxCallbacks;

    //
    // A flag that tracks whether the device is entering D0 for the first
    // time or not. For transitions #2 or higher, the per-bank secondary I/O
    // queues need to be restarted.
    //

    BOOLEAN InitialD0Entry;

    //
    // Worker item and associated flags for D-state transitioning. This is
    // only used for D3 -> D0 transition (since power framework requires to
    // be notified when D0 transition is complete).
    //

    WDFWORKITEM DevicePowerWorkItem;

    //
    // This is true if a PostInterruptEnabled callback is being invoked
    // post-S0 transition. In such cases D0 transition completion needs to be
    // reported to power framework and ACPI eventing pins need to be unmasked.
    //

    BOOLEAN WokeFromSx;

    //
    // This is flag is set if the bank interrupt state (active/inactive) should
    // be managed on every emulated-debounce interrupt connect/disconnect.
    //

    BOOLEAN ActivelyManageBankInterrupt;

    //
    // The last error code returned by the client driver.
    //

    NTSTATUS ClientErrorStatus;

    //
    // Errors that are tracked on a controller-wide basis.
    //

    CONTROLLER_ERROR_FLAGS Errors;

    //
    // Copy of the registration packet supplied by the GPIO client driver.
    // For internal clients, the internal registration packet fields will be
    // initialized with the client driver supplied callbacks. Otherwise those
    // fields will be initialized to NULL.
    //

    GPIO_CLIENT_REGISTRATION_PACKET_COMBINED CombinedPacket;

    //
    // In-flight recorder log handle.
    // This is always set to NULL if !defined(EVENT_TRACING).
    //

    RECORDER_LOG LogHandle;

    //
    // This is set by the IRP_MN_START_DEVICE handling to indicate that ACPI
    // events should be enabled as soon as the next IRP sent by PnP manager
    // (which is known to be IRP_MN_QUERY_DEVICE_RELATIONS) is received. This
    // ensures that ACPI events are only enabled after PnP manager has seen
    // IRP_MN_START_DEVICE fully complete.
    //
    // Reads to this field are not allowed when the IRP_MN_START_DEVICE is
    // in-flight.
    //

    BOOLEAN NeedToEnableAcpiEvents;

} PRIVATE_DEVICE_EXTENSION_PART;

typedef struct _PUBLIC_DEVICE_EXTENSION_PART {

    //
    // N.B. The public part of _DEVICE_EXTENSION comes after the private part,
    //      so this is not the first field in _DEVICE_EXTENSION.
    //

    ULONG Signature;

    //
    // A WDFDEVICE handle to the device (self).
    //

    WDFDEVICE Device;

    //
    // Tracks the GPIO controller's working state (GPIO_DEVICE_STATE).
    //

    volatile LONG DeviceState;

    //
    // Total pins on the GPIO controller (across all banks) and pins per bank.
    //
    // N.B. The last bank may have few than PinsPerBank pins.
    //

    USHORT TotalPins;
    USHORT PinsPerBank;

    //
    // A list of banks, each containing the interrupt and IO information for
    // pins within that bank.
    //

    USHORT TotalBanks;
    __field_ecount(TotalBanks) PGPIO_BANK_ENTRY Banks;

    //
    // Pointer to the target name for the device.
    //
    // N.B. This points to a WDF-maintained string and thus should only be
    //      used as read-only and should not be freed explicitly.
    //

    UNICODE_STRING TargetName;

    //
    // The BIOS name of the target device
    //

    UNICODE_STRING BiosName;

    //
    // Attributes about the GPIO controller.
    //

    CLIENT_CONTROLLER_BASIC_INFORMATION ClientInformation;

    //
    // Copy of the client driver's registration packet. This copy is included
    // here only for debugging purposes. The private device extension includes
    // the combined packet which should be used everywhere in the GPIO class
    // extension.
    //

    GPIO_CLIENT_REGISTRATION_PACKET RegistrationPacketDebug;

    //
    // Allocate storage for client driver's internal data. Cache align the
    // client driver device extension.  This must be the last field of the
    // GPIO's _DEVICE_EXTENSION (as client driver's data follows immediately).
    //

    DECLSPEC_CACHEALIGN BYTE ClientExtension[ANYSIZE_ARRAY];

} PUBLIC_DEVICE_EXTENSION_PART;

GPIO_STRUCT_COMBINE_PRIVATE_PUBLIC_PART(DEVICE_EXTENSION, Signature);

C_ASSERT((sizeof(DEVICE_EXTENSION) % SYSTEM_CACHE_ALIGNMENT_SIZE) == 0);

//
// Define the type for an entry within the global client driver list.
//

typedef struct _GPIO_CLIENT_DRIVER {
    LIST_ENTRY ListEntry;

    //
    // The driver object supplied by the client driver.
    //

    WDFDRIVER DriverObject;

    //
    // Buffer allocated for holding the default device name.
    //

    PWCHAR TargetName;

    //
    // Copy of the registration packet supplied by the client driver at
    // initialization time. For internal test clients, it also includes the
    // internal registration packet.
    //

    GPIO_CLIENT_REGISTRATION_PACKET_COMBINED CombinedPacket;

} GPIO_CLIENT_DRIVER, *PGPIO_CLIENT_DRIVER;

#pragma warning(pop)

//
// This will generate the function named GpioClxGetDeviceExtension to be used for
// retreiving the DEVICE_EXTENSION pointer.
//

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_EXTENSION, GpioClxGetDeviceExtension)

//
// Define the context that will be associated with every interrupt object.
//

typedef struct _GPIO_INTERRUPT_OBJECT_CONTEXT {

    //
    // Pointer to the bank associated with this interrupt.
    //

    PGPIO_BANK_ENTRY Bank;
} GPIO_INTERRUPT_OBJECT_CONTEXT, *PGPIO_INTERRUPT_OBJECT_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(GPIO_INTERRUPT_OBJECT_CONTEXT, GpioClxGetInterruptObjectContext)

//
// Define error flags that are tracked on a controller-wide basis.
//

typedef union _FILE_OBJECT_CONTEXT_FLAGS {
    struct {
        ULONG ReadWriteSupported: 1;
        ULONG DelayedConnect: 1;
        ULONG WriteIssued: 1;
        ULONG FunctionConfig:1;
        ULONG Reserved: 28;
    };

    ULONG AsULONG;
} FILE_OBJECT_CONTEXT_FLAGS, *PFILE_OBJECT_CONTEXT_FLAGS;

//
// Define the context that will be associated with every IO request (handle).
//

typedef struct _GPIO_FILE_OBJECT_CONTEXT {

    //
    // Entry on the per-bank IO/function config operations list. 
    // The entry must be linked/unlinked under the guard of the GPIO bank lock.
    //

    LIST_ENTRY ListEntry;
    PDEVICE_EXTENSION GpioExtension;
    BANK_ID BankId;

    //
    // The current connect mode of the pins associated with this file context.
    // Updates to this field need to be done under the guard of the GPIO
    // bank lock.
    //

    GPIO_CONNECT_IO_PINS_MODE ConnectMode;
    PPIN_NUMBER PinNumberTable;
    ULONG PinCount;
    PGPIO_PIN_INFORMATION_ENTRY *PinInformationTable;
    PPIN_NUMBER ConnectPinTable;
    ULONG ConnectCount;
    PVOID ClientIoContext;
    LARGE_INTEGER ConnectionId;
    PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER ResourceBuffer;
    LONG FastIoInterfaceReferenceCount;

    //
    // The flags associated with this file context. Updates to this field
    // must be done under the guard of the GPIO bank lock.
    //

    FILE_OBJECT_CONTEXT_FLAGS Flags;

} GPIO_FILE_OBJECT_CONTEXT, *PGPIO_FILE_OBJECT_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(GPIO_FILE_OBJECT_CONTEXT, GpioClxGetFileObjectContext)

//
// Define the interrupt-related worker item context.
//

typedef struct _GPIO_INTERRUPT_WORK_ITEM_CONTEXT {

    //
    // Pointer to the bank associated with this interrupt.
    //

    PGPIO_BANK_ENTRY Bank;
} GPIO_INTERRUPT_WORK_ITEM_CONTEXT, *PGPIO_INTERRUPT_WORK_ITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(GPIO_INTERRUPT_WORK_ITEM_CONTEXT, GpiopGetInterruptWorkItemContext)

typedef struct _GPIO_BANK_QUEUE_CONTEXT {
    PGPIO_BANK_ENTRY Bank;
} GPIO_BANK_QUEUE_CONTEXT, *PGPIO_BANK_QUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(GPIO_BANK_QUEUE_CONTEXT, GpiopBankQueueGetContext)

//
// Define the device power-related worker item context.
//

typedef struct _GPIO_DEVICE_POWER_WORK_ITEM_CONTEXT {

    //
    // Pointer to the bank associated with this power request.
    //

    PDEVICE_EXTENSION GpioExtension;

} GPIO_DEVICE_POWER_WORK_ITEM_CONTEXT, *PGPIO_DEVICE_POWER_WORK_ITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(GPIO_DEVICE_POWER_WORK_ITEM_CONTEXT, GpiopGetDevicePowerWorkItemContext)

//
// Define the context that will be associated with the debounce timer object.
//

typedef struct _GPIO_DEBOUNCE_TIMER_CONTEXT {

    //
    // Pointer to the bank associated with this interrupt.
    //

    PGPIO_BANK_ENTRY Bank;

    //
    // Pin number associated with this timer.
    //

    PIN_NUMBER PinNumber;

    //
    // The pin information entry for the corresponding pin.
    //

    PGPIO_PIN_INFORMATION_ENTRY PinInformation;

    //
    // EX_TIMER for the debounce interval.
    //

    PEX_TIMER DebounceTimer;

    //
    // The worker item for processing debounce and noise timer expirations at
    // passive-level.
    //
    // N.B. This field is only initialized for off-SoC GPIO controllers.
    //

    WDFWORKITEM DebounceNoiseTimerWorker;

} GPIO_DEBOUNCE_TIMER_CONTEXT, *PGPIO_DEBOUNCE_TIMER_CONTEXT;

//
// Define the context that will be associated with the debounce timer object.
//

typedef struct _GPIO_NOISE_FILTER_TIMER_CONTEXT {

    //
    // Pointer to the bank associated with this interrupt.
    //

    PGPIO_BANK_ENTRY Bank;

    //
    // Pin number associated with this timer.
    //

    PIN_NUMBER PinNumber;

    //
    // The pin information entry for the corresponding pin.
    //

    PGPIO_PIN_INFORMATION_ENTRY PinInformation;

    //
    // The interrupt epoch number associated with this timer instance.
    //

    ULONG InterruptEpoch;

    //
    // EX_TIMER for the noise filter interval.
    //

    PEX_TIMER NoiseFilterTimer;

    //
    // The worker item for processing debounce and noise timer expirations at
    // passive-level.
    //
    // N.B. This field is only initialized for off-SoC GPIO controllers.
    //

    WDFWORKITEM DebounceNoiseTimerWorker;

    //
    // A copy of the debounce output action that was returned by the debounce
    // engine when the noise filter timer was scheduled. This needs to be
    // presented back to the debounce engine on noise timer expiration.
    //

    GPIO_DEBOUNCE_OUTPUT_ACTION DebounceOutputAction;

} GPIO_NOISE_FILTER_TIMER_CONTEXT, *PGPIO_NOISE_FILTER_TIMER_CONTEXT;

//
// Define the debounce and noise filter worker item context. Since the actions
// are similar in either case (dispatch to the handler at passive-level), a
// single workitem is used.
//

typedef struct _GPIO_DEBOUNCE_NOISE_TIMER_WORK_ITEM_CONTEXT {

    //
    // Pointer to the bank associated with this interrupt.
    //

    PGPIO_BANK_ENTRY Bank;

    //
    // A flag indicating whether debounce timer is to be serviced or noise
    // timer is to be serviced for the pin.
    //
    // N.B. Access to this field is protected using the DPC-level bank interrupt
    //      for off-SoC controller. This structure is not used for on-SoC
    //      GPIO controllers.
    //

    BOOLEAN ServiceDebounceTimer;

    //
    // Supplies a pointer to the corresponding timer context (i.e., it points to
    // debounce timer context if ServiceDebounceTimer == TRUE, otherwise it
    // points to the noise filter timer context).
    //
    // N.B. Access to this field is protected using the DPC-level bank interrupt
    //      for off-SoC controller. This structure is not used for on-SoC
    //      GPIO controllers.
    //

    PVOID TimerContext;

    //
    // Holds whether the timer is still queued or not.
    //
    // N.B. Access to this field is protected using the DPC-level bank interrupt
    //      for off-SoC controller. This structure is not used for on-SoC
    //      GPIO controllers.
    //

    BOOLEAN WorkerQueued;

} GPIO_DEBOUNCE_NOISE_TIMER_WORK_ITEM_CONTEXT, *PGPIO_DEBOUNCE_NOISE_TIMER_WORK_ITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(GPIO_DEBOUNCE_NOISE_TIMER_WORK_ITEM_CONTEXT, GpiopGetDebounceNoiseTimerWorkItemContext)

//
// The interrupt subsystem passes this to the debounce subsystem as opaque
// data.  The debounce subsystem calls back into the interrupt subsystem.
//

typedef struct _GPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT {
    PGPIO_BANK_ENTRY GpioBank;
    PIN_NUMBER PinNumber;
    PGPIO_PIN_INFORMATION_ENTRY PinInformation;
    ULONG InterruptEpoch;
    BOOLEAN DeferActions;
    BOOLEAN ScheduleDebounceTimers;
    BOOLEAN ScheduleNoiseFilterTimers;
    BOOLEAN IsrInvocationDeferred;

    //
    // Context private to the interrupt state machine.
    //

    BOOLEAN ServiceActiveBothInterrupts;
    BOOLEAN ClearStatus;
} GPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT,
    *PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT;

//
// Types and definitions for various interrupt related request types.
//

//
// Define IOCTL for sending interrupt-related requests.
//
// N.B. This is issued as an internal IOCTL (and handled by a separate routine)
//      and thus the IOCTL definition must not conflict with the definition of
//      IOCTL_GPIO_READ_PINS.
//

#define IOCTL_GPIO_INTERRUPT_REQUEST                                   \
    CTL_CODE(FILE_DEVICE_GPIO, 0x0, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef enum _GPIO_INTERRUPT_REQUEST_TYPE {
    EnableInterruptRequest,
    DisableInterruptRequest,
    MaskInterruptRequest,
    UnmaskInterruptRequest
} GPIO_INTERRUPT_REQUEST_TYPE, *PGPIO_INTERRUPT_REQUEST_TYPE;

//
// Internal flags for GpiopMaskInterrupt and GpiopUnmaskInterrupt:
//
// GPIO_REQUEST_FLAG_NONE - No special handling.
//
//   This is the translated version of HAL_MASK_UNMASK_FLAGS_NONE.
//
// GPIO_REQUEST_FLAG_ACPI_EVENT - Currently unused.
//
// GPIO_REQUEST_FLAG_SERVICING_DEFERRED (for masking only) - Specifies that any
//   pending status clear must be delayed until after an unmask.
//
//   Sometimes interrupt service routine execution must be deferred e.g.
//   you cannot execute a passive-level ISR at DIRQL. In that case and if it
//   is a level-triggered interrupt, you must mask it until the ISR completes
//   to avoid an interrupt storm and you must delay the status clear until
//   after the ISR completes (see the comment for
//   "DelayPendingStatusClearMask").
//
//   This is the translated version of HAL_MASK_UNMASK_FLAGS_SERVICING_DEFERRED.
//
// GPIO_REQUEST_FLAG_SERVICING_COMPLETE (for unmasking only) - Specifies that
//   the delayed pending status clear can now be committed.
//
//   This is the translated version of HAL_MASK_UNMASK_FLAGS_SERVICING_COMPLETE.
//

#define GPIO_REQUEST_FLAG_NONE (0x0)
#define GPIO_REQUEST_FLAG_ACPI_EVENT (0x1)
#define GPIO_REQUEST_FLAG_SERVICING_DEFERRED (0x2)
#define GPIO_REQUEST_FLAG_SERVICING_COMPLETE (0x4)

#pragma warning(push)
#pragma warning(disable:4214) /* nonstandard extension used: bit field types other then int */
#pragma warning(disable:4201) /* nonstandard extension used: nameless struct/union */

typedef struct _GPIO_INTERRUPT_REQUEST_PARAMETERS {
    GPIO_INTERRUPT_REQUEST_TYPE Type;
    ULONG Flags;
    ULONG Virq;

    union {
        struct {
            KINTERRUPT_MODE InterruptMode;
            KINTERRUPT_POLARITY Polarity;
            ULONG_PTR CallbackContext;

            //
            // This is used to return the controller-relative pin number.
            //

            volatile ULONG_PTR *HubVirqEntryCache;
        } Enable;

        struct {
            UCHAR PlaceHolder;
        } Disable;

        struct {
            UCHAR PlaceHolder;
        } Unmask;

        struct {
            UCHAR PlaceHolder;
        } Mask;
    };

} GPIO_INTERRUPT_REQUEST_PARAMETERS, *PGPIO_INTERRUPT_REQUEST_PARAMETERS;

#pragma warning(pop)

//
// Define a global list of all clients.
//

extern LIST_ENTRY GpioClientDriverList;

//
// Define a lock that is used to guard access to the global client driver list.
//

extern FAST_MUTEX GpioClientGlobalListLock;

//
// Macro to get the client driver's device extension from GPIO class extension's device
// extension.
//

#define GPIO_GET_CLIENT_DEVICE_EXTENSION(Extension)                         \
    (&(Extension)->ClientExtension)

//
// Macro to get the client driver's device extension from device object.
//

#define GPIO_GET_CLIENT_EXTENSION_FROM_DEVICE(Device)                       \
    (&(GpioClxGetDeviceExtension(Device))->ClientExtension)

//
// Macro to get the client driver's registration packet from GPIO device
// extension.
//
// N.B. This only returns the public part of the registration packet.
//

#define GPIO_GET_CLIENT_REGISTRATION_PACKET(Extension)                      \
    ((PGPIO_CLIENT_REGISTRATION_PACKET)&(Extension)->CombinedPacket)

//
// Macro to get the combined client driver's registration packet from GPIO
// device extension.
//

#define GPIO_GET_CLIENT_COMBINED_REGISTRATION_PACKET(Extension)             \
    (&(Extension)->CombinedPacket)

//
// Macro to check if the registration packet is from an internal client.
//

#define GPIO_IS_CLIENT_INTERNAL(Type)                                       \
    CHECK_FLAG((Type)->CombinedPacket.Flags,                                \
               GPIO_CLIENT_REGISTRATION_FLAGS_INTERNAL)

//
// Macros to check whether the controller interrupt is level or edge triggered.
//

#define GPIO_IS_INTERRUPT_LEVEL_TRIGGERED(Bank)                             \
    ((Bank)->InterruptData.InterruptMode == LevelSensitive)

#define GPIO_IS_INTERRUPT_EDGE_TRIGGERED(Bank)                              \
    ((Bank)->InterruptData.InterruptMode == Latched)

//
// Macros to check whether the controller has an interrupt or not. The value
// will be unspecified (MAX_ULONG) if the controller does not consume any
// interrupt resources (possible in test-scenarios).
//

#define GPIO_IS_INTERRUPT_SPECIFIED(Bank)                                   \
    ((Bank)->InterruptData.Gsiv != GPIO_UNSPECIFIED_INTERRUPT)

//
// Macro to check if the controller callbacks should only be invoked at
// passive IRQL.
//
// For on-SoC controllers, this returns FALSE.
// For off-SoC controllers, this returns TRUE.
//

#define GPIO_IS_PASSIVE_IRQL_ONLY_DEVICE(Extension)                         \
    ((Extension)->ClientInformation.Flags.MemoryMappedController == FALSE)

//
// Determine the bank ID given the absolute pin number.
//

#define GPIO_BANK_ID_FROM_PIN_NUMBER(Extension, Pin)                        \
    ((Pin) / (Extension)->PinsPerBank)

#define GPIO_ABSOLUTE_TO_RELATIVE_PIN(Extension, Pin)                       \
    ((Pin) % (Extension)->PinsPerBank)

#define GPIO_RELATIVE_TO_ABSOLUTE_PIN(Bank, Pin)                            \
    (((Bank)->BankId * (Bank)->GpioExtension->PinsPerBank) + Pin)

#define GPIO_PIN_NUMBER_TO_PIN_MASK(PinNumber)                              \
    ((ULONG64)1 << PinNumber)

#define GPIO_BIOS_DESCRIPTOR_PULL_CONFIGURATION(Descriptor)                 \
    (Descriptor)->PinConfiguration

#define GPIO_BIOS_DESCRIPTOR_IO_RESTRICTION(Descriptor)                     \
    ((Descriptor)->InterruptIoFlags & GPIO_IORESTRICTION_MASK)

#define FUNCTION_CONFIG_BIOS_DESCRIPTOR_FUNCTION(Descriptor)                \
    (Descriptor)->FunctionNumber

#define FUNCTION_CONFIG_BIOS_DESCRIPTOR_PULL_CONFIGURATION(Descriptor)      \
    (Descriptor)->PinConfiguration

#if !defined(GPIO_STRUCTS_SYM_INFO)

//
// Define the various callbacks registered with WDF.
//

EVT_WDF_DRIVER_UNLOAD GpioClxEvtDriverUnload;
EVT_WDF_DEVICE_QUERY_STOP GpioClxEvtDeviceQueryStop;
EVT_WDF_DEVICE_PREPARE_HARDWARE GpioClxEvtDevicePrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE GpioClxEvtDeviceReleaseHardware;
EVT_WDF_DEVICE_D0_ENTRY GpioClxEvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT GpioClxEvtDeviceD0Exit;
EVT_WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED GpioClxEvtDevicePreInterruptsDisabled;
EVT_WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED GpioClxEvtDevicePostInterruptsEnabled;
EVT_WDF_INTERRUPT_ISR GpioClxEvtInterruptIsr;
EVT_WDF_INTERRUPT_ISR GpioClxEvtInterruptPassiveIsr;
EVT_WDF_INTERRUPT_DPC GpioClxEvtInterruptDpc;
EVT_WDF_IO_QUEUE_IO_READ GpioClxEvtProcessReadRequest;
EVT_WDF_IO_QUEUE_IO_WRITE GpioClxEvtProcessWriteRequest;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL GpioClxEvtProcessDeviceIoControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL GpioClxEvtProcessInternalDeviceIoControl;
EVT_WDF_WORKITEM GpioClxEvtHandleInterruptWorker;
EVT_WDF_WORKITEM GpioClxEvtDebounceNoiseFilterExpirationWorker;
EVT_WDF_DEVICE_FILE_CREATE  GpioClxEvtDeviceFileCreate;
EVT_WDF_FILE_CLOSE GpioClxEvtFileClose;
EVT_WDF_DEVICE_CONTEXT_CLEANUP GpioClxEvtDeviceContextCleanup;
EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT GpioClxEvtDeviceSelfManagedIoInit;
EVT_WDF_DEVICE_SELF_MANAGED_IO_FLUSH GpioClxEvtDeviceSelfManagedIoFlush;
EVT_WDF_WORKITEM GpiopDevicePowerD0TransitionWorker;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS GpioClxWdmPreprocessPowerIrp;
EVT_WDFDEVICE_WDM_IRP_PREPROCESS GpioClxWdmPreprocessQueryDeviceRelationsIrp;
EXT_CALLBACK GpioClxEvtDebounceTimerHandler;
EXT_CALLBACK GpioClxEvtNoiseFilterTimerHandler;

//
// Bank related callbacks.
//

EVT_WDF_IO_QUEUE_IO_DEFAULT GpioClxBankEvtProcessIoDefault;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL GpioClxBankEvtProcessDeviceIoControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL GpioClxBankEvtProcessInternalIoControl;
EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE GpioClxBankEvtProcessIoCanceledOnQueue;

#endif  // !defined(GPIO_STRUCTS_SYM_INFO)

//
// Define macros to acquire per-bank passive-level lock.
//
// Off-SoC:
//   This lock is used to synchronize with the ISR and other code that acquires
//   this lock.
//
// On-SoC:
//   At PASSIVE_LEVEL, this lock is sometimes used to synchronize with other
//   code that acquires this lock.
//

#define GPIO_ACQUIRE_BANK_LOCK(Bank)                                        \
{                                                                           \
    GPIO_ASSERT((KeGetCurrentIrql() < DISPATCH_LEVEL) &&                    \
                ((Bank)->PassiveLockOwner != KeGetCurrentThread()));        \
                                                                            \
    WdfWaitLockAcquire((Bank)->PassiveLock, NULL);                          \
    (Bank)->PassiveLockOwner = KeGetCurrentThread();                        \
}

//
// Define macro release per-bank passive-level lock.
//
// Ensure that the current thread owns the passive-level bank lock. Technically,
// the lock could be acquired from one thread and released in another. But the
// current GPIO CLX doesn't use such a model. It makes synchronization harder
// to track and diagnose and thus discouraged.
//

#define GPIO_RELEASE_BANK_LOCK(Bank)                                        \
{                                                                           \
                                                                            \
    GPIO_ASSERT((Bank)->PassiveLockOwner == KeGetCurrentThread());          \
                                                                            \
    (Bank)->PassiveLockOwner = NULL;                                        \
    WdfWaitLockRelease((Bank)->PassiveLock);                                \
}

//
// Define macros to acquire and release the GPIO bank interrupt lock.
//
// Off-SoC:
//   This lock is used to synchronize with other code that acquires this lock.
//
// On-SoC:
//   This lock is used to synchronize with the ISR and other code that acquires
//   this lock.
//

#define GPIO_ACQUIRE_BANK_INTERRUPT_LOCK(Bank)                              \
{                                                                           \
    GPIO_ASSERT((Bank)->InterruptLockOwner != KeGetCurrentThread());        \
                                                                            \
    if ((Bank)->BankInterrupt != NULL) {                                    \
        GPIO_ASSERT(GPIO_IS_INTERRUPT_SPECIFIED(Bank) != FALSE);            \
                                                                            \
        WdfInterruptAcquireLock((Bank)->BankInterrupt);                     \
                                                                            \
    } else {                                                                \
        WdfSpinLockAcquire((Bank)->BankInterruptLock);                      \
    }                                                                       \
                                                                            \
    (Bank)->InterruptLockOwner = KeGetCurrentThread();                      \
}

#define GPIO_RELEASE_BANK_INTERRUPT_LOCK(Bank)                              \
{                                                                           \
    GPIO_ASSERT(((Bank)->InterruptLockOwner != NULL));                      \
                                                                            \
    (Bank)->InterruptLockOwner = NULL;                                      \
    if ((Bank)->BankInterrupt != NULL) {                                    \
        WdfInterruptReleaseLock((Bank)->BankInterrupt);                     \
                                                                            \
    } else {                                                                \
        WdfSpinLockRelease((Bank)->BankInterruptLock);                      \
    }                                                                       \
}

//
// Define macros to acquire and release the GPIO interrupt lock for
// memory-mapped GPIO controllers.
//

#define GPIO_ACQUIRE_BANK_INTERRUPT_LOCK_IF_MEMORY_MAPPED(Bank)             \
{                                                                           \
    if (GPIO_IS_PASSIVE_IRQL_ONLY_DEVICE((Bank)->GpioExtension) == FALSE) { \
        GPIO_ACQUIRE_BANK_INTERRUPT_LOCK(Bank);                             \
    }                                                                       \
}

#define GPIO_RELEASE_BANK_INTERRUPT_LOCK_IF_MEMORY_MAPPED(Bank)             \
{                                                                           \
    if (GPIO_IS_PASSIVE_IRQL_ONLY_DEVICE((Bank)->GpioExtension) == FALSE) { \
        GPIO_RELEASE_BANK_INTERRUPT_LOCK(Bank);                             \
    }                                                                       \
}

//
// Macros to acquire and release the ACPI event processing lock.
//

#define GPIO_ACQUIRE_ACPI_EVENT_LOCK(Extension, Irql)                       \
    GpiopAcquireAcpiEventLock(Extension, Irql)

#define GPIO_RELEASE_ACPI_EVENT_LOCK(Extension, Irql)                       \
    GpiopReleaseAcpiEventLock(Extension, Irql)

//
// Macros to acquire and release the global client driver lock.
//

#define GPIO_ACQUIRE_CLIENT_LOCK()                                          \
    ExAcquireFastMutex(&GpioClientGlobalListLock)

#define GPIO_RELEASE_CLIENT_LOCK()                                          \
    ExReleaseFastMutex(&GpioClientGlobalListLock)

//
// Define macros to acquire the per-bank power lock.
//
// N.B. For simplicity, the per-bank power lock is defined to be the same as
//      the controller-wide power lock. The lock needs to be acquired only
//      during f-state transitions and device PnP state transitions, so there
//      shouldn't be much contention on the lock.
//

#define GPIO_ACQUIRE_BANK_POWER_LOCK(Bank, Irql)                            \
    GPIO_ACQUIRE_CONTROLLER_POWER_LOCK((Bank)->GpioExtension, Irql)

#define GPIO_RELEASE_BANK_POWER_LOCK(Bank, Irql)                            \
    GPIO_RELEASE_CONTROLLER_POWER_LOCK((Bank)->GpioExtension, Irql)

//
// Define macros to acquire the controller-wide power lock.
//

#define GPIO_ACQUIRE_CONTROLLER_POWER_LOCK(Extension, Irql)                 \
    WdfSpinLockAcquire((Extension)->PowerDownLock);

#define GPIO_RELEASE_CONTROLLER_POWER_LOCK(Extension, Irql)                 \
    WdfSpinLockRelease((Extension)->PowerDownLock);

//
// Macro to determine if the caller is on the thread that is currently
// servicing an interrupt for this bank.
//
// In most cases, this returns whether the call originated from the interrupt
// context or not. The only exception is that it will return TRUE if the caller
// is in a DPC that preempted the servicing of a passive bank interrupt. As an
// example, all calls to this macro originating from the debounce timer DPC
// check for this exception.
//
//

#define GPIO_IS_REQUEST_IN_INTERRUPT_CONTEXT(Bank)                          \
    ((Bank)->InterruptThread == KeGetCurrentThread())


#define GPIO_BANK_PASSIVE_LOCK_OWNER(Bank)                                  \
    ((Bank)->PassiveLockOwner == KeGetCurrentThread())

#define GPIO_BANK_INTERRUPT_LOCK_OWNER(Bank)                                \
    ((Bank)->InterruptLockOwner == KeGetCurrentThread())

//
// Macro to check if debouncing needs to be emulated.
//

#define GPIO_IS_DEBOUNCING_EMULATED(Extension)                              \
    ((Extension)->ClientInformation.Flags.EmulateDebouncing == TRUE)

//
// Macro to check if ActiveBoth needs to be emulated.
//

#define GPIO_IS_ACTIVE_BOTH_EMULATED(Extension)                             \
    ((Extension)->ClientInformation.Flags.EmulateActiveBoth == TRUE)

//
// Macro to check if independent I/O HW is supported.
//

#define GPIO_IS_INDEPENDENT_IO_HW_SUPPORTED(Extension)                      \
    ((Extension)->ClientInformation.Flags.IndependentIoHwSupported == TRUE)

//
// Macros to set, clear and query various interrupt-related registers.
//

#define GPIO_SET_PIN_REGISTER(Bank, Register, Pin)                             \
    ((Bank)->PinCount <= 32) ?                                                 \
        (ULONG)InterlockedOr((volatile LONG *)Register, ((ULONG)1 << (Pin)))   \
            :                                                                  \
        (ULONG64)InterlockedOr64((volatile LONGLONG *)Register, ((ULONG64)1 << (Pin)))  \

#define GPIO_CLEAR_PIN_REGISTER(Bank, Register, Pin)                           \
    ((Bank)->PinCount <= 32) ?                                                 \
        (ULONG)InterlockedAnd((volatile LONG *)Register, ~((ULONG)1 << (Pin))) \
            :                                                                  \
        (ULONG64)InterlockedAnd64((volatile LONGLONG *)Register, ~((ULONG64)1 << (Pin)))\

#define GPIO_SET_REGISTER_PINMASK(Bank, Register, PinMask)                     \
    ((Bank)->PinCount <= 32) ?                                                 \
        (ULONG)InterlockedOr((volatile LONG *)Register, (ULONG)(PinMask))      \
            :                                                                  \
        (ULONG64)InterlockedOr64((volatile LONGLONG *)Register, (PinMask))

#define GPIO_CLEAR_REGISTER_PINMASK(Bank, Register, PinMask)                   \
    ((Bank)->PinCount <= 32) ?                                                 \
        (ULONG)InterlockedAnd((volatile LONG *)Register, ~(ULONG)(PinMask))    \
            :                                                                  \
        (ULONG64)InterlockedAnd64((volatile LONGLONG *)Register, ~(PinMask))

#define GPIO_SET_REGISTER_VALUE(Bank, Register, Value)                         \
    ((Bank)->PinCount <= 32) ?                                                 \
        (ULONG)InterlockedExchange((volatile LONG *)Register, (ULONG)(Value))  \
            :                                                                  \
        (ULONG64)InterlockedExchange64((volatile LONGLONG *)Register, (Value))

#define GPIO_READ_REGISTER_VALUE(Bank, Register)                               \
    ((Bank)->PinCount <= 32) ?                                                 \
        (ULONG)InterlockedCompareExchange((volatile LONG *)Register, 0x0, 0x0) \
            :                                                                  \
        (ULONG64)InterlockedCompareExchange64((volatile LONGLONG *)Register, 0x0, 0x0)

#define GPIO_SET_PIN_INTERRUPT_ENABLED(Bank, RelativePin)                   \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.EnableRegister,            \
                          (RelativePin))

#define GPIO_CLEAR_PIN_INTERRUPT_ENABLED(Bank, RelativePin)                 \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.EnableRegister,          \
                            (RelativePin))

#define GPIO_SET_PIN_INTERRUPT_MASK(Bank, RelativePin)                      \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.InterruptMask,             \
                            (RelativePin))

#define GPIO_CLEAR_PIN_INTERRUPT_MASK(Bank, RelativePin)                    \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.InterruptMask,           \
                            (RelativePin))

#define GPIO_SET_PIN_PENDING_INTERRUPT_MASK(Bank, RelativePin)              \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.PendingInterruptMask,      \
                          (RelativePin))

#define GPIO_CLEAR_PIN_PENDING_INTERRUPT_MASK(Bank, RelativePin)            \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.PendingInterruptMask,    \
                            (RelativePin))

#define GPIO_SET_PIN_PENDING_INTERRUPT_UNMASK(Bank, RelativePin)            \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.PendingInterruptUnmask,    \
                          (RelativePin))

#define GPIO_CLEAR_PIN_PENDING_INTERRUPT_UNMASK(Bank, RelativePin)          \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.PendingInterruptUnmask,  \
                            (RelativePin))

#define GPIO_SET_PIN_INTERRUPT_CLEAR_MASK(Bank, RelativePin)                \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.StatusClearMask,           \
                          (RelativePin))

#define GPIO_CLEAR_PIN_INTERRUPT_CLEAR_MASK(Bank, RelativePin)              \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.StatusClearMask,         \
                            (RelativePin))

#define GPIO_SET_PIN_DELAY_PENDING_STATUS_CLEAR(Bank, RelativePin)          \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.DelayPendingStatusClearMask,  \
                            (RelativePin))

#define GPIO_CLEAR_PIN_DELAY_PENDING_STATUS_CLEAR(Bank, RelativePin)        \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.DelayPendingStatusClearMask,  \
                            (RelativePin))

#define GPIO_SET_PIN_PENDING_STATUS_CLEAR(Bank, RelativePin)                \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.PendingStatusClearMask,    \
                            (RelativePin))

#define GPIO_CLEAR_PIN_PENDING_STATUS_CLEAR(Bank, RelativePin)              \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.PendingStatusClearMask,  \
                            (RelativePin))

#define GPIO_SET_INTERRUPT_ENABLE_PINMASK(Bank, PinMask)                    \
    GPIO_SET_REGISTER_PINMASK((Bank),                                       \
                              &(Bank)->InterruptData.EnableRegister,        \
                              (PinMask))

#define GPIO_CLEAR_INTERRUPT_ENABLE_PINMASK(Bank, PinMask)                  \
    GPIO_CLEAR_REGISTER_PINMASK((Bank),                                     \
                                &(Bank)->InterruptData.EnableRegister,      \
                                (PinMask))

#define GPIO_SET_INTERRUPT_MASK_PINMASK(Bank, PinMask)                      \
    GPIO_SET_REGISTER_PINMASK((Bank),                                       \
                              &(Bank)->InterruptData.InterruptMask,         \
                              (PinMask))

#define GPIO_CLEAR_INTERRUPT_MASK_PINMASK(Bank, PinMask)                    \
    GPIO_CLEAR_REGISTER_PINMASK((Bank),                                     \
                                &(Bank)->InterruptData.InterruptMask,       \
                                (PinMask))

#define GPIO_SET_PENDING_INTERRUPT_MASK_PINMASK(Bank, PinMask)              \
    GPIO_SET_REGISTER_PINMASK((Bank),                                       \
                              &(Bank)->InterruptData.PendingInterruptMask,  \
                              (PinMask))

#define GPIO_CLEAR_PENDING_INTERRUPT_MASK_PINMASK(Bank, PinMask)            \
    GPIO_CLEAR_REGISTER_PINMASK((Bank),                                     \
                                &(Bank)->InterruptData.PendingInterruptMask,\
                                (PinMask))

#define GPIO_SET_PENDING_INTERRUPT_UNMASK_PINMASK(Bank, PinMask)            \
    GPIO_SET_REGISTER_PINMASK((Bank),                                       \
                              &(Bank)->InterruptData.PendingInterruptUnmask,\
                              (PinMask))

#define GPIO_CLEAR_PENDING_INTERRUPT_UNMASK_PINMASK(Bank, PinMask)            \
    GPIO_CLEAR_REGISTER_PINMASK((Bank),                                       \
                                &(Bank)->InterruptData.PendingInterruptUnmask,\
                                (PinMask))

#define GPIO_SET_PENDING_STATUS_CLEAR_PINMASK(Bank, PinMask)                \
    GPIO_SET_REGISTER_PINMASK((Bank),                                       \
                              &(Bank)->InterruptData.PendingStatusClearMask,\
                              (PinMask))

#define GPIO_CLEAR_PENDING_STATUS_CLEAR_PINMASK(Bank, PinMask)                \
    GPIO_CLEAR_REGISTER_PINMASK((Bank),                                       \
                                &(Bank)->InterruptData.PendingStatusClearMask,\
                                (PinMask))

#define GPIO_SET_CLEAR_MASK_REGISTER_PINMASK(Bank, PinMask)                 \
    GPIO_SET_REGISTER_PINMASK((Bank),                                       \
                              &(Bank)->InterruptData.StatusClearMask,       \
                              (PinMask))

#define GPIO_CLEAR_CLEAR_MASK_PINMASK(Bank, PinMask)                        \
    GPIO_CLEAR_REGISTER_PINMASK((Bank),                                     \
                                &(Bank)->InterruptData.StatusClearMask,     \
                                (PinMask))

#define GPIO_SET_CLEAR_MASK_REGISTER_VALUE(Bank, Value)                     \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.StatusClearMask,         \
                            (Value))

#define GPIO_READ_ENABLE_REGISTER_VALUE(Bank)                               \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.EnableRegister)         \

#define GPIO_SET_PENDING_MASK_REGISTER_VALUE(Bank, Value)                   \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.PendingInterruptMask,    \
                            (Value))

#define GPIO_READ_PENDING_MASK_REGISTER_VALUE(Bank)                         \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.PendingInterruptMask)   \

#define GPIO_SET_PENDING_UNMASK_REGISTER_VALUE(Bank, Value)                 \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.PendingInterruptUnmask,  \
                            (Value))

#define GPIO_READ_PENDING_UNMASK_REGISTER_VALUE(Bank)                       \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.PendingInterruptUnmask) \

#define GPIO_SET_PENDING_STATUS_CLEAR_VALUE(Bank, Value)                    \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.PendingStatusClearMask,  \
                            (Value))

#define GPIO_READ_PENDING_STATUS_CLEAR_VALUE(Bank)                          \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.PendingStatusClearMask) \

#define GPIO_SET_PIN_PENDING_DISCONNECT(Bank, RelativePin)                  \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.PendingDisconnect,         \
                          (RelativePin))

#define GPIO_CLEAR_PIN_PENDING_DISCONNECT(Bank, RelativePin)                \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.PendingDisconnect,       \
                            (RelativePin))

#define GPIO_SET_PENDING_DISCONNECT_VALUE(Bank, Value)                      \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.PendingDisconnect,       \
                            (Value))

#define GPIO_READ_PENDING_DISCONNECT_VALUE(Bank)                            \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.PendingDisconnect)      \

#define GPIO_SET_PIN_PENDING_ACTIVE_BOTH(Bank, RelativePin)                 \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.PendingActiveBoth,         \
                          (RelativePin))

#define GPIO_CLEAR_PIN_PENDING_ACTIVE_BOTH(Bank, RelativePin)               \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.PendingActiveBoth,      \
                            (RelativePin))

#define GPIO_SET_PENDING_ACTIVE_BOTH_VALUE(Bank, Value)                     \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.PendingActiveBoth,       \
                            (Value))

#define GPIO_READ_PENDING_ACTIVE_BOTH_VALUE(Bank)                           \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.PendingActiveBoth)      \

#define GPIO_SET_PIN_ACTIVE_DEBOUNCE_MASK(Bank, RelativePin)                \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.ActiveDebounceMask,        \
                          (RelativePin))

#define GPIO_CLEAR_PIN_ACTIVE_DEBOUNCE_MASK(Bank, RelativePin)              \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.ActiveDebounceMask,      \
                            (RelativePin))

#define GPIO_READ_ACTIVE_DEBOUNCE_MASK_VALUE(Bank)                          \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.ActiveDebounceMask)     \

#define GPIO_SET_PIN_DEBOUNCE_TIMER_MASK(Bank, RelativePin)                 \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.DebounceTimerMask,         \
                          (RelativePin))

#define GPIO_CLEAR_PIN_DEBOUNCE_TIMER_MASK(Bank, RelativePin)               \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.DebounceTimerMask,       \
                            (RelativePin))

#define GPIO_SET_DEBOUNCE_TIMER_MASK_VALUE(Bank, Value)                     \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.DebounceTimerMask,       \
                            (Value))

#define GPIO_READ_DEBOUNCE_TIMER_MASK_VALUE(Bank)                           \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.DebounceTimerMask)      \

#define GPIO_SET_PIN_PENDING_RECONFIGURE(Bank, RelativePin)                 \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.PendingReconfigure,        \
                          (RelativePin))

#define GPIO_CLEAR_PIN_PENDING_RECONFIGURE(Bank, RelativePin)               \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.PendingReconfigure,      \
                            (RelativePin))

#define GPIO_SET_PENDING_RECONFIGURE_VALUE(Bank, Value)                     \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.PendingReconfigure,      \
                            (Value))

#define GPIO_READ_PENDING_RECONFIGURE_VALUE(Bank)                           \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.PendingReconfigure)     \

#define GPIO_SET_PIN_NOISE_FILTER_TIMER_MASK(Bank, RelativePin)             \
    GPIO_SET_PIN_REGISTER((Bank),                                           \
                          &(Bank)->InterruptData.NoiseTimerMask,            \
                          (RelativePin))

#define GPIO_CLEAR_PIN_NOISE_FILTER_TIMER_MASK(Bank, RelativePin)           \
    GPIO_CLEAR_PIN_REGISTER((Bank),                                         \
                            &(Bank)->InterruptData.NoiseTimerMask,          \
                            (RelativePin))

#define GPIO_SET_NOISE_FILTER_TIMER_MASK_VALUE(Bank, Value)                 \
    GPIO_SET_REGISTER_VALUE((Bank),                                         \
                            &(Bank)->InterruptData.NoiseTimerMask,          \
                            (Value))

#define GPIO_READ_NOISE_FILTER_TIMER_MASK_VALUE(Bank)                       \
    GPIO_READ_REGISTER_VALUE((Bank),                                        \
                             &(Bank)->InterruptData.NoiseTimerMask)         \

//
// Macros to set and get the GPIO controller's PNP state.
//

#define GPIO_SET_DEVICE_STATE(Extension, State) \
    InterlockedExchange(&(Extension)->DeviceState, (State))

#define GPIO_GET_DEVICE_STATE(Extension) \
    InterlockedCompareExchange(&(Extension)->DeviceState, 0, 0)

#define GPIO_SET_DEVICE_POWER_STATE(Extension, State) \
    InterlockedExchange(&(Extension)->DevicePowerState, (State))

#define GPIO_GET_DEVICE_POWER_STATE(Extension) \
    InterlockedCompareExchange(&(Extension)->DevicePowerState, 0, 0)

#define GPIO_SET_IGNORE_PO_FX_CALLBACKS(Extension) \
    InterlockedExchange(&(Extension)->IgnorePoFxCallbacks, (LONG)TRUE)

#define GPIO_GET_IGNORE_PO_FX_CALLBACKS(Extension)                          \
    (BOOLEAN)InterlockedCompareExchange(&(Extension)->IgnorePoFxCallbacks,  \
                                        0,                                  \
                                        0)

//
// The #endif is at the end of the file.
//

#if !defined(GPIO_STRUCTS_SYM_INFO)

//
// Globals exported to other modules.
//

extern BOOLEAN GpioForceEmergencyWorker;

extern RECORDER_LOG GpioLogHandle;

extern volatile LONG GpioLogHandleInstance;
extern ULONG GpioLogBufferSize;

//
// Routines to get the pin entry for a given VIRQ or VIO.
//

_Must_inspect_result_
PGPIO_BANK_ENTRY
GpiopGetBankEntry (
    __in PDEVICE_EXTENSION GpioExtension,
    __in BANK_ID BankId
    );

_Must_inspect_result_
PGPIO_PIN_INFORMATION_ENTRY
GpiopGetPinEntry (
    _In_ PGPIO_BANK_ENTRY GpioBank,
    _In_ PIN_NUMBER RelativePin
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
PGPIO_PIN_INFORMATION_ENTRY
GpiopCreatePinEntry (
    _In_ PGPIO_BANK_ENTRY GpioBank,
    _In_ PIN_NUMBER RelativePin
    );

_Must_inspect_result_
PGPIO_BANK_ENTRY
GpiopGetBankEntryFromVirq (
    __in PDEVICE_EXTENSION GpioExtension,
    __in ULONG Virq
    );

_Must_inspect_result_
PGPIO_PIN_INFORMATION_ENTRY
GpiopGetPinEntryFromVirq (
    __in PDEVICE_EXTENSION GpioExtension,
    __in ULONG Virq,
    __out_opt PGPIO_BANK_ENTRY *BankEntry,
    __out_opt PBANK_ID BankNumber,
    __out_opt PPIN_NUMBER PinNumber
    );

_Must_inspect_result_
PGPIO_PIN_INFORMATION_ENTRY
GpiopGetPinEntryFromPinNumber (
    _In_ PDEVICE_EXTENSION GpioExtension,
    _In_ PIN_NUMBER PinNumber
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
PGPIO_PIN_INFORMATION_ENTRY
GpiopCreatePinEntryFromPinNumber (
    _In_ PDEVICE_EXTENSION GpioExtension,
    _In_ PIN_NUMBER PinNumber
    );

//
// Macros to increment/decrement reference to pin entry.
//

#define REFERENCE_PIN_ENTRY(PinEntry)                                   \
    InterlockedIncrement(&PinEntry->PointerCount)

#define DEREFERENCE_PIN_ENTRY(PinEntry)                                 \
    InterlockedDecrement(&PinEntry->PointerCount)

//
// Prototypes for routines exported to other modules.
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopBuildDeviceExtension (
    __in WDFDRIVER Driver,
    __in WDFDEVICE Device,
    __in PGPIO_CLIENT_DRIVER Client,
    __in RECORDER_LOG RecorderLogHandle
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopCreateClient (
    __in WDFDRIVER DriverObject,
    __in PGPIO_CLIENT_REGISTRATION_PACKET RegistrationPacket
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpiopDeleteDeviceExtension (
    __in PDEVICE_EXTENSION GpioExtension
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopDeleteClient (
    __in WDFDRIVER DriverObject
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
PGPIO_CLIENT_DRIVER
GpiopFindClient (
    __in WDFDRIVER DriverObject
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopInitialize (
    __in ULONG Phase,
    __in PDRIVER_OBJECT WdmDriverObject,
    __in WDFDRIVER WdfDriver,
    __in PUNICODE_STRING RegistryPath
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopQueryControllerInformation (
    __in PDEVICE_EXTENSION GpioExtension
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopInitializeBanksAndInterrupts (
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFCMRESLIST ResourcesTranslated,
    __in WDFCMRESLIST ResourcesRaw
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopQueryResourceHubBiosDescriptor (
    __in WDFDEVICE Device,
    __inout __deref WDFIOTARGET *ResourceHubTarget,
    __in BOOLEAN QueryOnGsiv,
    __in ULONG64 GsivOrConnectionId,
    __out __deref __drv_allocatesMem(Mem)
        PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER *BiosDescriptorOut
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopUpdateEmulatedActiveBothInitialPolarity (
    __in WDFDEVICE Device,
    __inout __deref WDFIOTARGET *ResourceHubTarget,
    __in ULONG Virq,
    __in UCHAR InitialPolarity
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpiopUninitialize (
    __in PDRIVER_OBJECT  DriverObject
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopOpenResourceHubTarget (
    __in WDFDEVICE Device,
    __out WDFIOTARGET *HubTarget
    );

//
// Interrupt related routines and definitions.
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopDisableInterrupt (
    __in PDEVICE_EXTENSION GpioExtension,
    __in ULONG Virq,
    __in BOOLEAN InSurpriseRemovalContext
    );

_Must_inspect_result_
__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopEnableInterrupt (
    __in PDEVICE_EXTENSION GpioExtension,
    __in ULONG Virq,
    __in KINTERRUPT_MODE InterruptMode,
    __in KINTERRUPT_POLARITY Polarity,
    __in ULONG_PTR CallbackContext,
    __inout_opt volatile ULONG_PTR *HubVirqEntryCache
    );

__drv_sameIRQL
VOID
GpiopCheckEnabledInterrupts (
    __in PGPIO_BANK_ENTRY GpioBank
    );

__drv_sameIRQL
NTSTATUS
GpiopMaskInterrupt (
    __in PDEVICE_EXTENSION GpioExtension,
    __in ULONG Flags,
    __in ULONG Virq
    );

__drv_sameIRQL
NTSTATUS
GpiopDeferredMaskRequestHandler (
    __in PGPIO_BANK_ENTRY GpioBank
    );

__drv_sameIRQL
NTSTATUS
GpiopDeferredUnmaskRequestHandler (
    __in PGPIO_BANK_ENTRY GpioBank
    );

__drv_sameIRQL
NTSTATUS
GpiopFlipPinPolarity (
    _In_ PGPIO_BANK_ENTRY GpioBank,
    _In_ PIN_NUMBER PinNumber
    );

__drv_sameIRQL
NTSTATUS
GpiopDeferredReconfigureRequestHandler (
    __in PGPIO_BANK_ENTRY GpioBank
    );

__drv_sameIRQL
NTSTATUS
GpiopDeferredStatusClearRequestHandler (
    __in PGPIO_BANK_ENTRY GpioBank
    );

NTSTATUS
GpiopDeferredDisableInterruptHandler (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in BOOLEAN SynchronousRequest
    );

__drv_sameIRQL
NTSTATUS
GpiopScheduleDeferredInterruptActivities (
    __in PGPIO_BANK_ENTRY GpioBank
    );

LONG
GpiopIncrementMaskCount (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PGPIO_PIN_INFORMATION_ENTRY PinInformation,
    __in PIN_NUMBER PinNumber
    );

__drv_sameIRQL
NTSTATUS
GpiopUnmaskInterrupt (
    __in PDEVICE_EXTENSION GpioExtension,
    __in ULONG Flags,
    __in ULONG Virq
    );

__drv_sameIRQL
NTSTATUS
GpiopRequestInterrupt (
    __in PVOID Context,
    __in ULONG Virq,
    __out PULONG NextGsiv
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopProcessInterruptRequestIoctl (
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFREQUEST Request,
    __in PGPIO_INTERRUPT_REQUEST_PARAMETERS RequestParameters,
    __out PULONG BytesWritten
    );

VOID
GpiopHandleInterruptMaskRequestCompletion (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in ULONG64 PinsToMask,
    __in ULONG64 FailedMask
    );

VOID
GpiopHandleInterruptUnmaskRequestCompletion (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in ULONG64 PinsToUnmask,
    __in ULONG64 FailedUnmask
    );

VOID
GpiopHandleStatusClearRequestCompletion (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in ULONG64 PinsToClear,
    __in ULONG64 FailedStatusClear
    );

VOID
GpiopHandleDisableInterruptRequestFailure (
    __in PGPIO_BANK_ENTRY GpioBank
    );

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopQueryBankIdFromGsivOrConnectionId(
    __in PDEVICE_EXTENSION GpioExtension,
    __in BOOLEAN QueryOnGsiv,
    __in ULONG64 GsivOrConnectionId,
    __out PGPIO_BANK_ENTRY *GpioBankOut,
    __out_opt PUCHAR IoRestriction,
    __out_opt PBOOLEAN FunctionConfig
    );

VOID
GpiopScheduleDebounceTimers (
    __in PGPIO_BANK_ENTRY GpioBank
    );

VOID
GpiopScheduleNoiseFilterTimers (
    __in PGPIO_BANK_ENTRY GpioBank
    );

VOID
GpiopServiceActiveBothInterrupts (
    __in PGPIO_BANK_ENTRY GpioBank
    );

NTSTATUS
GpiopReconfigureInterrupts (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_opt PIN_NUMBER PinNumber
    );

VOID
GpiopInvokeTargetIsr (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in ULONG64 PinMask
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpiopInitializeInterruptState (
    __in PGPIO_PIN_INFORMATION_ENTRY PinInformation
    );

NTSTATUS
GpiopDebounceMaskInterruptCallback (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

NTSTATUS
GpiopDebouceScheduleDebounceTimerCallback (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

VOID
GpiopDebouceScheduleNoiseFilterTimerCallback (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext,
    __in PGPIO_DEBOUNCE_OUTPUT_ACTION AssociatedOutputAction
    );

VOID
GpiopDebounceInvokeTargetIsrCallback (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

VOID
GpiopDebouceCancelNoiseFilterTimerCallback (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

NTSTATUS
GpiopDebounceUnmaskInterruptCallback (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

NTSTATUS
GpiopDebounceReconfigureInterruptCallback (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

ULONG
GpiopDebounceQueryNoiseFilterEpoch (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

VOID
GpiopDebounceLogSpuriousInterrupt (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

VOID
GpiopDebounceAcquireLock (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

VOID
GpiopDebounceReleaseLock (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext
    );

VOID
GpiopDebounceTracePin (
    __in PGPIO_DEBOUNCE_PIN_CONTEXT DebouncePinContext,
    __in PGPIO_INTERRUPT_DEBOUNCE_CONTROLLER_CONTEXT ControllerContext,
    __in PCSTR Message,
    __in ULONG DataCount,
    __in ULONG Data
    );

//
// Function config related routines and definitions.
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopConnectFunctionConfigPinsReserve (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in PLARGE_INTEGER ConnectionId
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopConnectFunctionConfigPinsCommit(
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopConnectFunctionConfigPinsLocked (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in FUNCTION_CONFIG_PIN_STATE FunctionConfigPinState
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopDisconnectFunctionConfigPins (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in BOOLEAN InSurpriseRemovalContext
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpiopDisconnectFunctionConfigPinsLocked (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in BOOLEAN InSurpriseRemovalContext
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopProcessFunctionConfigPinsCommitRequest(
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFQUEUE Queue,
    __in WDFREQUEST Request,
    __out PULONG BytesRead
);

//
// IO related routines and definitions.
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopConnectGpioPins (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in GPIO_CONNECT_IO_PINS_MODE ConnectMode,
    __in PLARGE_INTEGER ConnectionId
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopConnectGpioPinsLocked (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in GPIO_CONNECT_IO_PINS_MODE ConnectMode,
    __in PLARGE_INTEGER ConnectionId
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopDisconnectGpioPins (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in BOOLEAN InSurpriseRemovalContext
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpiopDisconnectGpioPinsLocked (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in BOOLEAN InSurpriseRemovalContext,
    __in BOOLEAN ForcePreserveConfiguration
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopSwitchDirectionPins (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in GPIO_CONNECT_IO_PINS_MODE NewDirection,
    __in BOOLEAN FastIoMode
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopReadPins (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in ULONG Flags,
    __out PVOID Buffer,
    __in ULONG BufferSizeInBits,
    __out PULONG BitsRead
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopWritePins (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PGPIO_FILE_OBJECT_CONTEXT FileContext,
    __in WDFREQUEST Request,
    __in ULONG Flags,
    __in PVOID Buffer,
    __in ULONG BufferSizeInBits,
    __out PULONG BitsWritten
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopProcessControllerSpecificFunctionIoctl (
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFQUEUE Queue,
    __in WDFREQUEST Request,
    __out PULONG BytesReadWritten
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopProcessReadPinsRequest (
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFQUEUE Queue,
    __in WDFREQUEST Request,
    __out PULONG BytesRead
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopProcessWritePinsRequest (
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFQUEUE Queue,
    __in WDFREQUEST Request,
    __out PULONG BytesWritten
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopProcessFastIoInterfaceRequest (
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFQUEUE Queue,
    __in WDFREQUEST Request,
    __out PULONG BytesRead
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpiopCleanupAllBanksInterruptIo (
    __in PDEVICE_EXTENSION GpioExtension,
    __in BOOLEAN InSurpriseRemovalContext
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpiopWaitForStateMachineCompletionOfDebouncedMaskedPin (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PGPIO_PIN_INFORMATION_ENTRY PinInformation,
    __in PIN_NUMBER PinNumber,
    __in BOOLEAN WaitForTimerDpcCompletion
    );

//
// Misc routines (gpioclx.c)
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
GpiopValidateControllerFunctionBankMappings (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PCLIENT_CONTROLLER_QUERY_SET_INFORMATION_OUTPUT Information
    );

//
// Types for the interfaces exported by class extension to the hub driver.
//

typedef
__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
(GPIO_CLX_INITIALIZE_HUB_VIRQ_ENTRY_CACHE) (
    __inout volatile ULONG_PTR *HubVirqEntryCache
);

typedef
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
(GPIO_CLX_CONNECT_INTERRUPT) (
    __in PVOID Context,
    __in ULONG Virq,
    __in __drv_strictTypeMatch(__drv_typeConst) KINTERRUPT_MODE InterruptMode,
    __in __drv_strictTypeMatch(__drv_typeConst) KINTERRUPT_POLARITY Polarity,
    __in ULONG_PTR CallbackContext,
    __inout_opt volatile ULONG_PTR *HubVirqEntryCache
);

typedef
__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
(GPIO_CLX_DISCONNECT_INTERRUPT) (
    __in PVOID Context,
    __in ULONG Virq
);

typedef
__drv_sameIRQL
NTSTATUS
(GPIO_CLX_MASK_INTERRUPT) (
    __in PVOID Context,
    __in ULONG Flags,
    __in ULONG Virq
);

typedef
__drv_sameIRQL
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
(GPIO_CLX_UNMASK_INTERRUPT) (
    __in PVOID Context,
    __in ULONG Flags,
    __in ULONG Virq
);

typedef
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
(GPIO_CLX_QUERY_INTERRUPT_INFORMATION) (
    __in PVOID Context,
    __in ULONG SecondaryGsiv,
    __out PULONG Gsiv,
    __out KIRQL *Irql,
    __out PGROUP_AFFINITY Affinity,
    __out KINTERRUPT_MODE *InterruptMode,
    __out PKINTERRUPT_POLARITY Polarity
);

typedef
__drv_sameIRQL
NTSTATUS
(GPIO_CLX_REQUEST_INTERRUPT) (
    __in PVOID Context,
    __in ULONG Virq,
    __out PULONG NextGsiv
);

typedef
_Must_inspect_result_
__drv_requiresIRQL(HIGH_LEVEL)
NTSTATUS
(GPIO_CLX_QUERY_PIN_INFORMATION) (
    __in PVOID Context,
    __in ULONG Gsiv,
    __in volatile ULONG_PTR *HubVirqEntryCache,
    __out PUSHORT PinIndex,
    __out PVOID *ControllerId
);

typedef GPIO_CLX_INITIALIZE_HUB_VIRQ_ENTRY_CACHE
    *PGPIO_CLX_INITIALIZE_HUB_VIRQ_ENTRY_CACHE;

typedef GPIO_CLX_CONNECT_INTERRUPT *PGPIO_CLX_CONNECT_INTERRUPT;
typedef GPIO_CLX_DISCONNECT_INTERRUPT *PGPIO_CLX_DISCONNECT_INTERRUPT;
typedef GPIO_CLX_MASK_INTERRUPT *PGPIO_CLX_MASK_INTERRUPT;
typedef GPIO_CLX_UNMASK_INTERRUPT *PGPIO_CLX_UNMASK_INTERRUPT;
typedef GPIO_CLX_QUERY_INTERRUPT_INFORMATION *PGPIO_CLX_QUERY_INTERRUPT_INFORMATION;
typedef GPIO_CLX_REQUEST_INTERRUPT *PGPIO_CLX_REQUEST_INTERRUPT;
typedef GPIO_CLX_QUERY_PIN_INFORMATION *PGPIO_CLX_QUERY_PIN_INFORMATION;

//
// Class extension related routines.
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopClassLibraryCreate (
    __in PDRIVER_OBJECT  DriverObject,
    __in PUNICODE_STRING RegistryPath
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpiopClassLibraryDestroy (
    __in PDRIVER_OBJECT  DriverObject
    );

//
// Interrupt-processing related routines and definitions (interrupt.c)
//

extern HAL_SECONDARY_INTERRUPT_INFORMATION GpioHalSecondaryInformation;

GPIO_CLX_INITIALIZE_HUB_VIRQ_ENTRY_CACHE GpioClxInitializeHubVirqEntryCache;
GPIO_CLX_CONNECT_INTERRUPT GpioClxEnableInterrupt;
GPIO_CLX_DISCONNECT_INTERRUPT GpioClxDisableInterrupt;
GPIO_CLX_MASK_INTERRUPT GpioClxMaskInterrupt;
GPIO_CLX_UNMASK_INTERRUPT GpioClxUnmaskInterrupt;
GPIO_CLX_QUERY_INTERRUPT_INFORMATION GpioClxQueryInterruptInformation;
GPIO_CLX_REQUEST_INTERRUPT GpioClxRequestInterrupt;
GPIO_CLX_QUERY_PIN_INFORMATION GpioClxQueryPinInformation;

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopAllocateInterruptProcessingResources (
    __in PDEVICE_EXTENSION GpioExtension
    );

BOOLEAN
GpioClxEvtVirtualRootIsr (
    __in PVOID Context
    );

//
// ACPI eventing related routines and definitions (acpievt.c).
//

typedef enum _ACPI_EVENTING_STATE {
    AcpiEventStateInitialize = 0x0,
    AcpiEventStateAllocateResources,
    AcpiEventStateEnable,
    AcpiEventStateDisable,
    AcpiEventStateReleaseResources,
    AcpiEventStateDestroy,
    AcpiEventStateEnableForConnectedStandby,
    AcpiEventStateDisableForConnectedStandby,

    //
    // Sx-based sleep (S1, S2, S3), hibernate (S4), shutdown/reboot (S5)
    //

    AcpiEventStateDeepUnmaskForSx,
    AcpiEventStateDeepMaskForSx,
    StateMaxAcpiEvents
} ACPI_EVENTING_STATE, *PACPI_EVENTING_STATE;

typedef struct _EVENT_WORK_ITEM_CONTEXT {
    WDFOBJECT Device;
    ACPI_EVENTING_STATE State;
    BOOLEAN Synchronous;
} EVENT_WORK_ITEM_CONTEXT, *PEVENT_WORK_ITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(EVENT_WORK_ITEM_CONTEXT, GpiopGetEventWorkItemContext)

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopManageAcpiEventing (
    __in PDEVICE_EXTENSION GpioExtension,
    __in ACPI_EVENTING_STATE State,
    __in BOOLEAN Synchronous
    );

VOID
GpiopProcessAcpiEvents (
    __in PDEVICE_EXTENSION GpioExtension
    );

BOOLEAN
GpiopIsAcpiEventInterrupt (
    __in PDEVICE_EXTENSION GpioExtension,
    __in ULONG Gsiv
    );

VOID
GpiopAcquireAcpiEventLock (
    __in PDEVICE_EXTENSION GpioExtension,
    __out __drv_out_deref(__drv_savesIRQL) PKIRQL OldIrql
    );

__drv_setsIRQL(OldIrql)
FORCEINLINE
VOID
GpiopReleaseAcpiEventLock (
    __in PDEVICE_EXTENSION GpioExtension,
    __in __drv_in(__drv_restoresIRQL) KIRQL OldIrql
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopValidateEventMethodForPin (
    __in PDEVICE_EXTENSION GpioExtension,
    __in KINTERRUPT_MODE InterruptMode,
    __in PIN_NUMBER PinNumber,
    __out PBOOLEAN RunEVTMethod
    );

VOID
GpiopSetInterruptState (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in WDFINTERRUPT Interrupt,
    __in BOOLEAN ActiveState
    );

//
// Power management related routines (power.c)
//

NTSTATUS
GpiopHandlePowerPostInterruptsEnabled (
    __in WDFDEVICE Device,
    __in WDF_POWER_DEVICE_STATE PreviousState
    );

NTSTATUS
GpiopHandlePowerPreInterruptsDisabled (
    __in WDFDEVICE Device
    );

NTSTATUS
GpiopInitializeDevicePowerWorker (
    __in PDEVICE_EXTENSION GpioExtension
    );

NTSTATUS
GpiopInitializeDxIdlePowerPolicy (
    __in PDEVICE_EXTENSION GpioExtension
    );

NTSTATUS
GpiopRegisterWithPowerFramework (
    __in PDEVICE_EXTENSION GpioExtension
    );

NTSTATUS
GpiopRegisterWithPowerFrameworkInternal (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PDEVICE_OBJECT WdmDevice
    );

VOID
GpiopStartRuntimePowerManagement (
    __in PDEVICE_EXTENSION GpioExtension
    );

VOID
GpiopUnregisterWithPowerFramework (
    __in PDEVICE_EXTENSION GpioExtension
    );

//
// Utility routines (utils.c)
//

VOID
GpioUtilUpdateBitmap (
    __in PRTL_BITMAP Source,
    __inout PRTL_BITMAP Destination,
    __in BOOLEAN AndOperation
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilEnumerateNamespaceChildren (
    __in PDEVICE_EXTENSION GpioExtension,
    __deref_out_bcount(*ReturnBufferSize)
        __drv_at(*ReturnBuffer, __drv_allocatesMem(Mem))
        PACPI_ENUM_CHILDREN_OUTPUT_BUFFER *ReturnBuffer,

    __out_opt PULONG ReturnBufferSize
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilEvaluateAcpiMethod (
    __in WDFDEVICE Device,
    __in ULONG MethodName,
    __in_opt PVOID EvalInputBuffer,
    __deref_out_bcount(*ReturnBufferSize) PVOID *ReturnBuffer,
    __out_opt PULONG ReturnBufferSize
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilGetResourceName (
    __in_bcount(FIELD_OFFSET(RH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER,
                             ConnectionProperties) +
                Descriptor->PropertiesLength)
        PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER Descriptor,
    __out __drv_at(BiosName->Buffer, __drv_allocatesMem(Mem))
        PUNICODE_STRING BiosName
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
GPIO_CONNECT_IO_PINS_MODE
GpioUtilGetOpenModeFromRequest (
    __in WDF_REQUEST_PARAMETERS *RequestParameters,
    __in UCHAR IoRestriction,
    __out PBOOLEAN ReadWriteMode
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilGetIdFromFileName (
    __in PCUNICODE_STRING FileName,
    __out PLARGE_INTEGER ConnectionIdOut
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilGetIdFromFileName (
    __in PCUNICODE_STRING FileName,
    __out PLARGE_INTEGER ConnectionIdOut
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
GpioUtilIsBiosGpioDescriptorValid (
    __in_bcount(DescriptorLength)
        PPNP_GPIO_INTERRUPT_IO_DESCRIPTOR Descriptor,
    __in ULONG DescriptorLength
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
GpioUtilIsBiosFunctionConfigDescriptorValid (
    __in_bcount(DescriptorLength)
        PPNP_FUNCTION_CONFIG_DESCRIPTOR Descriptor,
    __in ULONG DescriptorLength
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilIsChildObjectDefined (
    __in PACPI_ENUM_CHILDREN_OUTPUT_BUFFER EnumChildrenBuffer,
    __in ULONG EnumChildrenBufferSize,
    __in ULONG ChildObjectName
    );


_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilParseNamespaceEventPins (
    __in PACPI_ENUM_CHILDREN_OUTPUT_BUFFER EnumChildrenBuffer,
    __in ULONG EnumChildrenBufferSize,
    __inout_ecount_opt(EdgePinListInputSize) PULONG EdgePinList,
    __in ULONG EdgePinListInputSize,
    __out PULONG EdgePinListOutputCount,
    __inout_ecount_opt(LevelPinListInputSize) PULONG LevelPinList,
    __in ULONG LevelPinListInputSize,
    __out PULONG LevelPinListOutputCount
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilQuerySecondaryGsivRange (
    __out PULONG GsivBase,
    __out PULONG GsivSize
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilSetSecondaryInterruptInterface (
    __in ULONG GsivBase,
    __in ULONG GsivSize,
    __in PDRIVER_OBJECT DriverObject
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioUtilTracePrintPinNumbers (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount
    );

#define ALL_GPIO_PINS (INVALID_PIN_NUMBER)

typedef enum _OPERATION_REGISTER_TYPE {
    AcpiRegisterTypePendingEvents,
    IntRegisterTypeEnableMask,
    IntRegisterTypeInterruptMask,
    IntRegisterTypePendingStatusClearMask,
    IntRegisterTypeDelayPendingStatusClearMask
} OPERATION_REGISTER_TYPE, *POPERATION_REGISTER_TYPE;

VOID
GpioUtilUpdatePinValue (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PIN_NUMBER PinNumber,
    __in OPERATION_REGISTER_TYPE Type,
    __in BOOLEAN SetOperation
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilQueryRegistryFlags (
    __inout_opt WDFKEY *ParametersKey,
    __in_opt WDFDRIVER WdfDriver,
    __in PUNICODE_STRING RegistryPath,
    __in PCUNICODE_STRING KeyValueName,
    __out PULONG OptionValue
    );

//
// _DSM Method Support
//

extern const GUID GPIO_CONTROLLER_DSM_GUID;

#define GPIO_CONTROLLER_DSM_METHOD_ARGUMENTS_COUNT 4
#define GPIO_CONTROLLER_DSM_REVISION 0

#define GPIO_CONTROLLER_DSM_QUERY_FUNCTION_INDEX 0
#define GPIO_CONTROLLER_DSM_ACTIVE_BOTH_POLARITY_FUNCTION_INDEX 1

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilIsDsmFunctionSupported (
    __in WDFDEVICE Device,
    __in ULONG FunctionIndex,
    __in ULONG FunctionRevision,
    __out PBOOLEAN Supported
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilPrepareInputParametersForDsmMethod (
    __in ULONG FunctionIndex,
    __in ULONG FunctionRevision,
    __deref_out_bcount(*ReturnBufferSize) PACPI_EVAL_INPUT_BUFFER_COMPLEX *ReturnBuffer,
    __out_opt PULONG ReturnBufferSize
    );

//
// GPIO_CONTROLLER_DRIVER_ERROR bugcheck sub-codes.
//

#define CLIENT_CONTROLLER_NOT_REGISTERED (0x1)
#define INTERRUPT_ACQUIRE_LOCK_UNLOCK_FAILURE (0x2)
#define INVALID_SAVE_BANK_HARDWARE_CONTEXT (0x3)
#define INVALID_RESTORE_BANK_HARDWARE_CONTEXT (0x4)
#define INTERRUPT_OPERATION_FAILURE  (0x5)
#define IO_OPERATION_FAILURE (0x6)
#define DSM_OUTPUT_MALFORMED (0x7)
#define ENABLED_MASK_MISMATCH (0x8)
#define ENABLE_DISABLE_TIMEOUT (0x9)
#define EMULATED_ACTIVE_BOTH_WITHOUT_LEVELS (0xa)
#define FAST_IO_INTERFACE_REFERENCE_COUNT_NOT_ZERO (0xb)

#define STATUS_SECONDARY_INTERRUPT_ALREADY_PENDING \
        STATUS_INTERRUPT_STILL_CONNECTED

#endif  // !defined(GPIO_STRUCTS_SYM_INFO)

