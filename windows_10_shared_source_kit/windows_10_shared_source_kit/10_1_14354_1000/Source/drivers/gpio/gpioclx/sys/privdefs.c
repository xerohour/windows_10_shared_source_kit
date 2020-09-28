/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    privdefs.c

Abstract:

    This header file contains routines that call into kernel exports or use
    definitions that are not exposed to regular drivers via WDM headers.


Environment:

    Kernel mode

--*/

//
// ------------------------------------------------------------------- Includes
//

#define NO_PCH_INCLUDES
#include "pch.h"
#undef NO_PCH_INCLUDES

#include <ntosp.h>
#include <secintr.h>
#include "privdefs.h"

extern HAL_SECONDARY_INTERRUPT_INFORMATION GpioHalSecondaryInformation;


//
// ------------------------------------------------------------------- Routines
//

ULONG
GpiopGetBaseRegistrationBufferSize (
    VOID
    )

/*++

Routine Description:

    This routine returns the size of PO_FX_PRIMARY_DEVICE type.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    return sizeof(PO_FX_PRIMARY_DEVICE);
}

VOID
GpiopInitializeIdleStateBufferAndCount (
    __inout PVOID PowerRegistrationBuffer,
    __in ULONG ComponentId,
    __in PVOID IdleStateBuffer,
    __in ULONG IdleStateCount
    )

/*++

Routine Description:

    This routine initializes the idle state buffer and count in the
    PO_FX_PRIMARY_DEVICE buffer.

Arguments:

    PowerRegistrationBuffer - Supplies a pointer to PO_FX_PRIMARY_DEVICE buffer.

    ComponentId - Supplies the ID of component.

    IdleStateBuffer - Supplies a pointer to the idle state buffer.

    IdleStateCount - Supplies a count of the number of idle states for the
        specified component.

Return Value:

    None.

--*/

{

    PPO_FX_PRIMARY_DEVICE RuntimePower;

    PAGED_CODE();

    //
    // Specify the state information for each idle state.
    //

    RuntimePower = (PPO_FX_PRIMARY_DEVICE)PowerRegistrationBuffer;
    RuntimePower->Components[ComponentId].IdleStateCount = IdleStateCount;
    RuntimePower->Components[ComponentId].IdleStates = IdleStateBuffer;
    return;
}

//
// ---------------------------------- Wrappers for power framework (PoFx) APIs.
//

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
    )

/*++

Routine Description:

    This routine initializes the supplied PO_FX_PRIMARY_DEVICE with all the
    callbacks.

Arguments:

    PowerRegistrationBuffer - Supplies a pointer to PO_FX_PRIMARY_DEVICE buffer.

    ComponentCount - Supplies the number of components on the controller.

    ComponentActiveCallback - Supplies the callback for active transitions.

    ComponentIdleCallback - Supplies the callback for idle transitions.

    ComponentIdleStateCallback - Supplies the callback for f-state transitions.

    DevicePowerRequiredCallback - Supplies a callback for device power required
        cases.

    DevicePowerNotRequiredCallback - Supplies a callback for device power not
        required cases.

    PowerControlCallback  Supplies a callback for PEP IO control requests.

    DeviceContext - Supplies a pointer to the context received with the
        callback invocations.

    DeviceInformationCallback - Supplies a callback that PEP invokes when
        it requires the controller information (pin state).

Return Value:

    None.

--*/

{

    PPO_FX_PRIMARY_DEVICE RuntimePower;

    PAGED_CODE();

    //
    // Initialize the regular power framework callbacks.
    //
    //

    RuntimePower = (PPO_FX_PRIMARY_DEVICE)PowerRegistrationBuffer;
    RuntimePower->Version = PO_FX_VERSION_V1;
    RuntimePower->ComponentCount = ComponentCount;
    RuntimePower->ComponentIdleStateCallback = ComponentIdleStateCallback;
    RuntimePower->ComponentActiveConditionCallback = ComponentActiveCallback;
    RuntimePower->ComponentIdleConditionCallback = ComponentIdleCallback;
    RuntimePower->DevicePowerRequiredCallback = DevicePowerRequiredCallback;
    RuntimePower->DevicePowerNotRequiredCallback =
        DevicePowerNotRequiredCallback;

    RuntimePower->PowerControlCallback = PowerControlCallback;
    RuntimePower->DeviceContext = DeviceContext;

    //
    // Initialize all the special callbacks that primary controllers (like
    // GPIOs) need to supply.
    //

    RuntimePower->ComponentCriticalTransitionCallback =
        CriticalTransitionCallback;

    return;
}

NTSTATUS
GpiopRegisterPrimaryDeviceInternal (
    __in PDEVICE_OBJECT WdmDevice,
    __in PVOID RuntimePower,
    __out POHANDLE *PowerHandle
    )

/*++

Routine Description:

    This routine registers the GPIO controller with the power framework. This
    enables the PEP to perform runtime power management on the GPIO controller.

    Each bank is registered as a separate component. This is a contract imposed
    by the GPIO class extension that the GPIO client driver and the PEP need to
    honor. For each bank, only F0 (fully-on) and F1 (off) are supported today.

Arguments:

    WdmDevice - Supplies a pointer to the GPIO (WDM) device object.

    RuntimePower - Supplies a pointer to the structure describing the power
        characteristics of the GPIO controller.

    Handle - Supplies a pointer to the output buffer to receive the handle to
        the Runtime Power Management Framework device.

Return Value:

    NTSTATUS code.

--*/

{

    PAGED_CODE();

    return PoFxRegisterPrimaryDevice(WdmDevice, RuntimePower, PowerHandle);
}

//
// ----------------------------------------------------- Wrappers for HAL APIs.
//

NTSTATUS
GpioHalEnableInterrupt (
    __in ULONG Gsiv,
    __in KINTERRUPT_MODE InterruptMode,
    __in KINTERRUPT_POLARITY Polarity
    )

/*++

Routine Description:

    This routine is a wrapper on top of the HAL interrupt enable routine. This
    is defined here since the INTERRUPT_CONNECTION_DATA is not exported to
    regular drivers.

Arguments:

    Gsiv - Supplies the GSIV for interrupt line that should be enabled.

    InterruptMode - Supplies the trigger mode (edge or level) associated with
        this interrupt.

    Polarity - Supplies the polarity (active low or active high) associated with
        this interrupt.

Return Value:

    NTSTATUS code.

--*/

{

    INTERRUPT_CONNECTION_DATA ConnectionData;
    NTSTATUS Status;
    PINTERRUPT_VECTOR_DATA VectorData;

    RtlZeroMemory(&ConnectionData, sizeof(INTERRUPT_CONNECTION_DATA));
    ConnectionData.Count = 1;
    VectorData = &ConnectionData.Vectors[0];
    VectorData->Type = InterruptTypeControllerInput;
    VectorData->Irql = DISPATCH_LEVEL + 1;
    VectorData->Mode = InterruptMode;
    VectorData->Polarity = Polarity;
    VectorData->TargetProcessors.Group = 0x0;
    VectorData->TargetProcessors.Mask = 0x1;
    VectorData->ControllerInput.Gsiv = Gsiv;
    Status = HalEnableInterrupt(&ConnectionData);
    return Status;
}

NTSTATUS
GpioHalDisableInterrupt (
    __in ULONG Gsiv,
    __in KINTERRUPT_MODE InterruptMode,
    __in KINTERRUPT_POLARITY Polarity
    )

/*++

Routine Description:

    This routine is a wrapper on top of the HAL interrupt disable routine. This
    is defined here since the INTERRUPT_CONNECTION_DATA is not exported to
    regular drivers.

    This routine is not marked PAGED as it may be called before/after the boot
    device is in D0/D3 if boot device has GPIO dependencies.

Arguments:

    Gsiv - Supplies the GSIV for interrupt line that should be disconnected.

    InterruptMode - Supplies the trigger mode (edge or level) associated with
        this interrupt.

    Polarity - Supplies the polarity (active low or active high) associated with
        this interrupt.

Return Value:

    NTSTATUS code.

--*/

{

    INTERRUPT_CONNECTION_DATA ConnectionData;
    NTSTATUS Status;
    PINTERRUPT_VECTOR_DATA VectorData;

    RtlZeroMemory(&ConnectionData, sizeof(INTERRUPT_CONNECTION_DATA));
    ConnectionData.Count = 1;
    VectorData = &ConnectionData.Vectors[0];
    VectorData->Type = InterruptTypeControllerInput;
    VectorData->Irql = DISPATCH_LEVEL + 1;
    VectorData->Mode = InterruptMode;
    VectorData->Polarity = Polarity;
    VectorData->TargetProcessors.Group = 0x0;
    VectorData->TargetProcessors.Mask = 0x1;
    VectorData->ControllerInput.Gsiv = Gsiv;
    Status = HalDisableInterrupt(&ConnectionData);
    return Status;
}

NTSTATUS
GpioHalMaskInterrupt (
    __in ULONG Gsiv,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine is a wrapper on top of the HAL interrupt mask routine. This
    is defined here for consistency with the other interrupt routines.

    This routine is not marked PAGED so that the storage device does not spin
    up and potentially delay entering connected standby.

Arguments:

    Gsiv - Supplies the GSIV for interrupt line that should be masked.

    Flags - Supplies flags that control mask/unmask behavior.

Return Value:

    NTSTATUS code.

--*/

{

    return HalMaskInterrupt(Gsiv, Flags);
}

NTSTATUS
GpioHalUnmaskInterrupt (
    __in ULONG Gsiv,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine is a wrapper on top of the HAL interrupt unmask routine. This
    is defined here for consistency with the other interrupt routines.

    This routine is not marked PAGED so as not to add delays to exiting
    connected standby.

Arguments:

    Gsiv - Supplies the GSIV for interrupt line that should be unmasked.

    Flags - Supplies flags that control mask/unmask behavior.

Return Value:

    NTSTATUS code.

--*/

{

    return HalUnmaskInterrupt(Gsiv, Flags);
}

VOID
GpiopInvokeIsrForGsiv (
    __in ULONG Gsiv,
    __in ULONG_PTR Context,
    __out PBOOLEAN IsrWasDispatched
    )

/*++

Routine Description:

    This routine is a wrapper on top of the HAL interrupt dispatch routine.

Arguments:

    Gsiv - Supplies the GSIV for which the ISR should be run.

    Context - Supplies a pointer to the secondary interrupt controller that
              is responsible for the managing the supplied GSIV.  This context
              was supplied to the caller when the interrupt was enabled.

    IsrWasDispatched - Supplies a pointer to a boolean indicating that at least
                       one ISR was synchronously dispatched.


Return Value:

    None.

--*/

{

    KINTERRUPT_DISPATCH_STATUS DispatchStatus;

    DispatchStatus = IsrNotDispatched;
    (VOID)GpioHalSecondaryInformation.InvokeIsrForGsiv(Gsiv,
                                                       Context,
                                                       &DispatchStatus);

    switch(DispatchStatus) {

    case IsrNotDispatched:
        *IsrWasDispatched = FALSE;
        break;


    case IsrDispatchDeferred:
        *IsrWasDispatched = TRUE;
        break;

    case IsrDispatched:
        *IsrWasDispatched = TRUE;
        break;

    default:

        NT_ASSERT(FALSE);

        *IsrWasDispatched = FALSE;
        break;
    }
}

BOOLEAN
GpiopExTryQueueWorkItem (
    __inout  PWORK_QUEUE_ITEM WorkItem,
    __in     WORK_QUEUE_TYPE QueueType
    )

/*++

Routine Description:

    This routine is a wrapper on top of ExTryQueueWorkItem routine.

Arguments:

    WorkItem - Supplies a pointer to the work item to add the the queue.

    QueueType - Specifies the type of work queue that the work item
        should be placed in.

Return Value:

    BOOLEAN - return value from ExTryQueueWorkItem().

--*/

{

    return ExTryQueueWorkItem(WorkItem, QueueType);
}

//
// ------------------------------------------ Wrappers for Enhanced timer APIs.
//

PVOID
GpiopExAllocateTimerInternal (
    _In_ PEXT_CALLBACK Callback,
    _In_opt_ PVOID Context
    )

/*++

Routine Description:

    This routine is a wrapper on top of the ExAllocateTimerInternal() API.

Arguments:

    DeferredRoutine - Supplies the callback to run when the timer expires.

    DeferredContext - Supplies a context to pass to the callback.

Return Value:

    A pointer to the timer that was created or NULL.

--*/

{

    ULONG TimerAttributes;

    TimerAttributes = EX_TIMER_IDLE_RESILIENT | EX_TIMER_HIGH_RESOLUTION;
    return ExAllocateTimerInternal(Callback, Context, TimerAttributes);
}

