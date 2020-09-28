/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    privdefs.h

Abstract:

    This header file contains prototypes for routines that call into kernel
    exports or use definitions that are not exposed to regular drivers via WDM
    headers.


Environment:

    Kernel mode

--*/


//
// Prototype declarations for various NT wrappers.
//

ULONG
GpiopGetBaseRegistrationBufferSize (
    VOID
    );

VOID
GpiopInitializeIdleStateBufferAndCount (
    __inout PVOID PowerRegistrationBuffer,
    __in ULONG ComponentId,
    __in PVOID IdleStateBuffer,
    __in ULONG IdleStateCount
    );

VOID
GpiopInitializePrimaryDevice (
    __inout PVOID PowerRegistrationBuffer,
    __in ULONG ComponentCount,
    __in PPO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK ComponentActiveCallback,
    __in PPO_FX_COMPONENT_IDLE_CONDITION_CALLBACK ComponentIdleCallback,
    __in PPO_FX_COMPONENT_IDLE_STATE_CALLBACK ComponentIdleStateCallback,
    __in PPO_FX_DEVICE_POWER_REQUIRED_CALLBACK DevicePowerRequiredCallback,
    __in PPO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK DevicePowerNotRequiredCallback,
    __in PPO_FX_POWER_CONTROL_CALLBACK PowerControlCallback,
    __in PVOID DeviceContext,
    __in PPO_FX_COMPONENT_CRITICAL_TRANSITION_CALLBACK CriticalTransitionCallback
    );

NTSTATUS
GpiopRegisterPrimaryDeviceInternal (
    __in PDEVICE_OBJECT WdmDevice,
    __in PVOID RuntimePower,
    __out POHANDLE *PowerHandle
    );

NTSTATUS
GpioHalEnableInterrupt (
    __in ULONG Gsiv,
    __in KINTERRUPT_MODE InterruptMode,
    __in KINTERRUPT_POLARITY Polarity
    );

NTSTATUS
GpioHalDisableInterrupt (
    __in ULONG Gsiv,
    __in KINTERRUPT_MODE InterruptMode,
    __in KINTERRUPT_POLARITY Polarity
    );

NTSTATUS
GpioHalMaskInterrupt (
    __in ULONG Gsiv,
    __in ULONG Flags
    );

NTSTATUS
GpioHalUnmaskInterrupt (
    __in ULONG Gsiv,
    __in ULONG Flags
    );

VOID
GpiopInvokeIsrForGsiv (
    __in ULONG Gsiv,
    __in ULONG_PTR Context,
    __out PBOOLEAN IsrWasDispatched
    );

BOOLEAN
GpiopExTryQueueWorkItem (
    __inout  PWORK_QUEUE_ITEM WorkItem,
    __in     WORK_QUEUE_TYPE QueueType
    );

PVOID
GpiopExAllocateTimerInternal (
    _In_ PEXT_CALLBACK Callback,
    _In_opt_ PVOID Context
    );

