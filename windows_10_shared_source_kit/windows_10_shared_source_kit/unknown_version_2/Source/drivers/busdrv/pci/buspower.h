/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    buspower.h

Abstract:

    This file contains definitions for buspower.c which must be scanned as part
    of WPP tracing, and thus can't be in the same file as the other WPP
    definitions (pcip.h).









--*/

#pragma once

typedef enum _PCI_BUS_POWER_STATES {

    PciBusPowerLeavingS0ArmedDx     = 0,
    PciBusPowerLeavingS0Dx,
    PciBusPowerLeavingS0DxDisarmed,
    PciBusPowerLeavingS0DxDisarming,
    PciBusPowerS0D0,
    PciBusPowerS0Disarmed,
    PciBusPowerS0Dozing,
    PciBusPowerS0DozingArmed,
    PciBusPowerS0DozingArmedStirring,
    PciBusPowerS0DozingStirring,
    PciBusPowerS0Dx,
    PciBusPowerS0Off,
    PciBusPowerS0On,
    PciBusPowerS0Stirring,
    PciBusPowerStirringD0,
    PciBusPowerStirringDisarmed,
    PciBusPowerSx,
    PciBusPowerSxArmed,
    PciBusPowerSxDozingStirring,
    PciBusPowerSxOn,
    PciBusPowerMaximum

} PCI_BUS_POWER_STATES, *PPCI_BUS_POWER_STATES;

// begin_wpp config
// CUSTOM_TYPE(BUS_POWER_STATE, ItemEnum(_PCI_BUS_POWER_STATES));
// end_wpp

typedef enum _PCI_BUS_POWER_EVENTS {

    PciBusEventFirstPowerRefAdded   = 0,
    PciBusEventLastPowerRefRemoved,
    PciBusEventEnteringS0,
    PciBusEventLeavingS0,
    PciBusEventPoRequestComplete,
    PciBusEventWakeReady,
    PciBusEventMaximum

} PCI_BUS_POWER_EVENTS, *PPCI_BUS_POWER_EVENTS;

// begin_wpp config
// CUSTOM_TYPE(BUS_POWER_EVENT, ItemEnum(_PCI_BUS_POWER_EVENTS));
// end_wpp

typedef enum _PCI_CONVENTIONAL_WAKE_STATES {

    PciConvWakeArmed                = 0,
    PciConvWakeArmedDozing,
    PciConvWakeCanceledReferenced,
    PciConvWakeCanceledReferencedDozing,
    PciConvWakeCanceledReferencedIncomplete,
    PciConvWakeCanceledWait,
    PciConvWakeDereferenced,
    PciConvWakeDereferencedCanceled,
    PciConvWakeDereferencedDozing,
    PciConvWakeDereferencedTriggered,
    PciConvWakeDereferencedWaiting,
    PciConvWakeDisarmed,
    PciConvWakeDisarmedDozing,
    PciConvWakeDozingReferenced,
    PciConvWakeDozingReferencedCanceled,
    PciConvWakeDozingReferencedComplete,
    PciConvWakeQueuedDereferenced,
    PciConvWakeQueuedDozing,
    PciConvWakeQueuedTriggered,
    PciConvWakeReferenced,
    PciConvWakeReferencedCanceled,
    PciConvWakeTriggeredDozing,
    PciConvWakeTriggeredQueued,
    PciConvWakeTriggeredQueuedDozing,
    PciConvWakeWWQueued,
    PciConvWakeMaximum

} PCI_CONVENTIONAL_WAKE_STATES, *PPCI_CONVENTIONAL_WAKE_STATES;

// begin_wpp config
// CUSTOM_TYPE(BUS_CONVENTIONAL_WAKE_STATE, ItemEnum(_PCI_CONVENTIONAL_WAKE_STATES));
// end_wpp

typedef enum _PCI_EXPRESS_WAKE_STATES {

    PciExpressWakeArmedOff          = 0,
    PciExpressWakeArmedOn,
    PciExpressWakeArmedStirring,
    PciExpressWakeArmedStirringWait,
    PciExpressWakeCompleteDereferenced,
    PciExpressWakeDereferencedSent,
    PciExpressWakeDisarmedDozing,
    PciExpressWakeDisarmedOff,
    PciExpressWakeDisarmedOn,
    PciExpressWakeDisarmedStirring,
    PciExpressWakeDisarmingCancelled,
    PciExpressWakeDisarmingComplete,
    PciExpressWakeDisarmingOff,
    PciExpressWakeDisarmingStirring,
    PciExpressWakeDisarmingStirringWait,
    PciExpressWakeDisarmingTriggered,
    PciExpressWakeQueuedDereferenced,
    PciExpressWakeQueuedDozing,
    PciExpressWakeQueuedSent,
    PciExpressWakeQueuedTriggered,
    PciExpressWakeQueuingDozing,
    PciExpressWakeStirringCancelled,
    PciExpressWakeStirringTriggered,
    PciExpressWakeTriggeredComplete,
    PciExpressWakeTriggeredDereferenced,
    PciExpressWakeTriggeredOff,
    PciExpressWakeTriggeredReady,
    PciExpressWakeMaximum

} PCI_EXPRESS_WAKE_STATES, *PPCI_EXPRESS_WAKE_STATES;

// begin_wpp config
// CUSTOM_TYPE(BUS_EXPRESS_WAKE_STATE, ItemEnum(_PCI_EXPRESS_WAKE_STATES));
// end_wpp

typedef enum _PCI_BUS_WAKE_EVENTS {

    PciWakeEventFirstWakeRefAdded = 0,
    PciWakeEventLastWakeRefRemoved,
    PciWakeEventWaitWakeComplete,
    PciWakeEventWorkerSentWaitWake,
    PciWakeEventWorkerCanceled,
    PciWakeEventPowerUp,
    PciWakeEventPowerDown,
    PciWakeEventMaximum

} PCI_BUS_WAKE_EVENTS, *PPCI_BUS_WAKE_EVENTS;

// begin_wpp config
// CUSTOM_TYPE(BUS_WAKE_EVENT, ItemEnum(_PCI_BUS_WAKE_EVENTS));
// end_wpp

typedef enum _PCI_BUS_POWER_REFERENCE_REASONS {

    PciBusPowerAllowF1DisableBridgeD3       = 0,
    PciBusPowerReasonChildNeedsD0,
    PciBusPowerReasonChildNeedsD1or2,
    PciBusPowerReasonPmeTriggered,
    PciBusPowerReasonBusPnpOperationPending,
    PciBusPowerReasonPowerSchemeCallbackPending,
    PciBusPowerReasonBusWaitWakeComplete,
    PciBusPowerReasonChildWaitWakeComplete,
    PciBusPowerReasonRetiringChildD0Irps,
    PciBusPowerReasonChildNeedsConfigAccess,
    PciBusPowerReasonMaximum,

} PCI_BUS_POWER_REFERENCE_REASONS, *PPCI_BUS_POWER_REFERENCE_REASONS;

// begin_wpp config
// CUSTOM_TYPE(BUS_POWER_REF, ItemEnum(_PCI_BUS_POWER_REFERENCE_REASONS));
// end_wpp

//
// Note - State.c depends on the order of these.
//
typedef enum _PCI_OBJECT_STATE {
    PciNotStarted = 0,
    PciStarted,
    PciDeleted,
    PciStopped,
    PciSurpriseRemoved,
    PciSynchronizedOperation,
    PciMaxObjectState
} PCI_OBJECT_STATE;

// begin_wpp config
// CUSTOM_TYPE(PNP_STATE, ItemEnum(_PCI_OBJECT_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(WDM_DEVICE_POWER_STATE, ItemEnum(_DEVICE_POWER_STATE));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(DSM_FUNCTION, ItemEnum(_PCI_FIRMWARE_DSM_FUNCTIONS));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(OBFF_CONTROL, ItemEnum(_EXPRESS_PORT_OBFF_CONTROL));
// end_wpp

// begin_wpp config
// CUSTOM_TYPE(OBFF_SUPPORT, ItemEnum(_EXPRESS_PORT_OBFF_SUPPORT));
// end_wpp


