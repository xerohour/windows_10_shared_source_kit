/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    clientinvk.h

Abstract:

    This file contains prototypes for routines used by the class extension to
    invoke client driver-supplied interfaces.


Environment:

    Kernel mode

--*/

#pragma once

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokePrepareController (
    __in WDFDEVICE Device,
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFCMRESLIST ResourcesRaw,
    __in WDFCMRESLIST ResourcesTranslated
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeReleaseController (
    __in WDFDEVICE Device,
    __in PDEVICE_EXTENSION GpioExtension
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeStartController (
    __in PDEVICE_EXTENSION GpioExtension,
    __in BOOLEAN RestoreContext,
    __in WDF_POWER_DEVICE_STATE PreviousPowerState
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeStopController (
    __in PDEVICE_EXTENSION GpioExtension,
    __in BOOLEAN SaveContext,
    __in WDF_POWER_DEVICE_STATE TargetState
    );

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeQueryBankPowerInformation (
    __in PGPIO_BANK_ENTRY GpioBank,
    __out PCLIENT_CONTROLLER_QUERY_SET_INFORMATION_OUTPUT PowerInformation
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeQueryControllerBasicInformation (
    __in PDEVICE_EXTENSION GpioExtension,
    __out PCLIENT_CONTROLLER_BASIC_INFORMATION ControllerInformation
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeQueryInterruptBinding (
    __in PDEVICE_EXTENSION GpioExtension,
    __in WDFCMRESLIST ResourcesTranslated,
    __in WDFCMRESLIST ResourcesRaw,
    __in ULONG InterruptCount,
    __out_xcount(BindingOutput->Size)
        PCLIENT_CONTROLLER_QUERY_SET_INFORMATION_OUTPUT BindingOutput
   );

//
// ------------------------------------------------------- Interrupt interfaces
//

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeEnableInterrupt (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PIN_NUMBER RelativePin,
    __in __drv_strictTypeMatch(__drv_typeConst) KINTERRUPT_MODE InterruptMode,
    __in __drv_strictTypeMatch(__drv_typeConst) KINTERRUPT_POLARITY Polarity,
    __in_opt PPNP_GPIO_INTERRUPT_IO_DESCRIPTOR Descriptor
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeDisableInterrupt (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PIN_NUMBER RelativePin,
    __in BOOLEAN RetryAttempt
    );

_Must_inspect_result_
NTSTATUS
GpioClnInvokeQueryEnabledInterrupts (
    __in PGPIO_BANK_ENTRY GpioBank,
    __out PULONG64 EnabledMask
    );

_Must_inspect_result_
NTSTATUS
GpioClnInvokeMaskInterrupts (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in ULONG64 InterruptMask,
    __out PULONG64 FailedMask
    );

NTSTATUS
GpioClnInvokeUnmaskInterrupt (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PIN_NUMBER RelativePin,
    __in __drv_strictTypeMatch(__drv_typeConst) KINTERRUPT_MODE InterruptMode,
    __in __drv_strictTypeMatch(__drv_typeConst) KINTERRUPT_POLARITY Polarity,
    __in_opt UCHAR PullConfiguration,
    __in_opt USHORT DebounceTimeout
    );

_Must_inspect_result_
NTSTATUS
GpioClnInvokeQueryActiveInterrupts (
    __in PGPIO_BANK_ENTRY GpioBank
    );

_Must_inspect_result_
NTSTATUS
GpioClnInvokeClearActiveInterrupts (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in ULONG64 ClearMask,
    __out PULONG64 FailedClearMask
    );

VOID
GpioClnInvokePreProcessControllerInterrupt (
    __in PGPIO_BANK_ENTRY GpioBank
    );

NTSTATUS
GpioClnInvokeReconfigureInterrupt (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PIN_NUMBER PinNumber,
    __in KINTERRUPT_MODE InterruptMode,
    __in KINTERRUPT_POLARITY Polarity,
    __in BOOLEAN TolerateFailure
    );

//
// ------------------------------------------------Function config interfaces
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeConnectFunctionConfigPins (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount,
    __in_opt PPNP_FUNCTION_CONFIG_DESCRIPTOR Descriptor
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeDisconnectFunctionConfigPins (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount,
    __in ULONG DisconnectFlags
    );

//
// -------------------------------------------------------------- IO interfaces
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeConnectPins (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount,
    __in GPIO_CONNECT_IO_PINS_MODE ConnectMode,
    __in_opt PPNP_GPIO_INTERRUPT_IO_DESCRIPTOR Descriptor
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeCreateIoContext (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount,
    __in ULONG ConnectMode,
    __deref_out_opt PVOID *ClientIoContext
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeDeleteIoContext (
    __in PDEVICE_EXTENSION GpioExtension,
    __in_opt PVOID ClientIoContext
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeDisconnectPins (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount,
    __in ULONG DisconnectMode,
    __in ULONG DisconnectFlags
    );

_Must_inspect_result_
NTSTATUS
GpioClnInvokeReadPins (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount,
    __out_bcount((PinCount + 7) / 8) PVOID Buffer,
    __out PULONG64 PinValues,
    __in_opt PVOID ClientIoContext,
    __in BOOLEAN PinsWriteConfigured
    );

_Must_inspect_result_
NTSTATUS
GpioClnInvokeWritePins (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount,
    __in_bcount((PinCount + 7) / 8) PVOID Buffer,
    __in ULONG64 SetMask,
    __in ULONG64 ClearMask,
    __in_opt PVOID ClientIoContext
    );

//
// --------------------------------------------------- Bank Pwr mgmt interfaces
//

VOID
GpioClnInvokeSaveBankHardwareContext (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in BOOLEAN CriticalTransition
    );

VOID
GpioClnInvokeRestoreBankHardwareContext (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in BOOLEAN CriticalTransition
    );

//
// ------------------------------------------------------------ Misc interfaces
//

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokePreControllerInterruptDisabledCallback (
    __in PDEVICE_EXTENSION GpioExtension
    );

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeControllerSpecificFunctionHandler (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PVOID InputBuffer,
    __in SIZE_T InputBufferSize,
    __inout PVOID OutputBuffer,
    __in SIZE_T OutputBufferSize,
    __out PULONG BytesReadWritten
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokePostControllerInterruptEnabledCallback (
    __in PDEVICE_EXTENSION GpioExtension
    );

VOID
GpioClnInvokePreBankFStateTransitionCallback (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in FSTATE FState
    );

VOID
GpioClnInvokePostBankFStateTransitionCallback (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in FSTATE FState
    );

__checkReturn
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioClnInvokeQueryControllerFunctionBankMapping (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PVOID InputBuffer,
    __in SIZE_T InputBufferSize,
    __in SIZE_T OutputBufferSize,
    __out_xcount(MappingOutput->Size)
        PCLIENT_CONTROLLER_QUERY_SET_INFORMATION_OUTPUT MappingOutput
   );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioClnInvokeQueryDebounceNoiseIntervals (
    __in PDEVICE_EXTENSION GpioExtension,
    __in PIN_NUMBER AbsolutePinNumber,
    __inout PULONG DebounceTimeout,
    __inout PULONG64 NoiseFilterTimeout,
    __out PGPIO_DEBOUNCE_MODEL DebounceModelOverride
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioClnInvokeControllerCleanupCallback (
    _In_ PDEVICE_EXTENSION GpioExtension
    );


