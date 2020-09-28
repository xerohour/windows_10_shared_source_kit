/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    buspower.c

Abstract:

    This module manages power policy for PCI's FDOs, which amount to the busses
    themselves.  Earlier versions of this file did simple power policy, where
    busses were only placed in low power states when the system was going to
    sleep or when the driver got a "remove" IRP, which might happen if the
    device node was disabled in Device Manager or if the entire bus was being
    ejected from the machine, as with a docking station.
 
    This version of the file introduces two sets of changes.  First, it attempts
    to more aggressively shut off the PCI bus if all of the devices (functions,
    really) on the bus are already in D3 and they can support D3cold.  Second,
    it integrates with the Power Engine Plugin (PEP) so that PEPs which are
    supplied by SoC vendors can know when a bus has entered a low power state
    and notify a bus that it should re-enter a higher power state.
 
    Old behavior:
 
    - When a machine wakes up, the bus is put in D0.
    - When a machine sleeps, the bus is put in some Dx state, where Dx is
      determined by wake states.
    - When a bus is put in D0, the bridge is reprogrammed.
    - If it's a HyperTransport bridge, those registers are reprogrammed.
    - When the last child on the bus is moved to D0 on a PCIe bus, the
      link is retrained.
 
    New behavior:
 
    - When a machine wakes up, the bus heirarchy is brought into D0 temporarily,
      long enough to allow any pending PME interrupts to be handled.  If no
      devices move themselves into D0, then the bus heirarchy is put back into
      various low power states.
    - When a PME IRP completes to the bridge FDO, the bus goes to D0.
    - When a child device attempts to go to D0, the bridge is first moved to D0.
    - If the last child device moves out of D0, the bridge moves to Dx, where
      Dx is determined by wake state.
    - If the machine sleeps and some child device is still in D0, the bus still
      goes to Dx, where Dx is determined by wake state.
    - HyperTransport is treated as above.
    - When the first (not last) child on the bus is moved to D0 on a PCIe bus,
      the link is retrained, before the D0 IRP completes.
    - This version uses the PEP, so D0 and Dx transitions are asynchronous,
      happening when the PEP says they should.
 










--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "buspower.tmh"

// --------------------------------------------------------------------- Defines

#define PCI_FSTATE_COUNT 2

// ----------------------------------------------------------------------- Types

typedef
__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PCI_POWER_POLICY_STATE_FUNC (
    _In_ PPCI_BUS Bus
    );

typedef PCI_POWER_POLICY_STATE_FUNC *PPCI_POWER_POLICY_STATE_FUNC;

typedef enum _PCI_BUS_POWER_POLICY_STATE_FLAGS {
    PciBusPowerPolicyFlagLocked         = 1,
    PciBusPowerPolicyFlagMarkArmed      = 2,
    PciBusPowerPolicyFlagMarkDisarmed   = 4,
    PciBusPowerPolicyFlagSendWW         = 8,
    PciBusPowerPolicyFlagCancelWW       = 0x10,
    PciBusPowerPolicyFlagWakeReady      = 0x20,
    PciBusPowerPolicyFlagSignalRemoval  = 0x40
} PCI_BUS_POWER_POLICY_STATE_FLAGS, *PPCI_BUS_POWER_POLICY_STATE_FLAGS;

typedef struct _PCI_BUS_POWER_POLICY_STATE {

    PCI_BUS_POWER_STATES                NextState[PciBusEventMaximum];
    PPCI_POWER_POLICY_STATE_FUNC        Func;
    PCI_BUS_POWER_POLICY_STATE_FLAGS    Flags;

} PCI_BUS_POWER_POLICY_STATE, *PPCI_BUS_POWER_POLICY_STATE;

typedef const PCI_BUS_POWER_POLICY_STATE *PCPCI_BUS_POWER_POLICY_STATE;

typedef struct _PCI_BUS_WAKE_STATE {

    PCI_BUS_POWER_STATES                NextState[PciWakeEventMaximum ];
    PCI_BUS_POWER_POLICY_STATE_FLAGS    ActionFlags;
    PCI_BUS_POWER_STATES                FallThroughState;

} PCI_BUS_WAKE_STATE, *PPCI_BUS_WAKE_STATE;

typedef const PCI_BUS_WAKE_STATE *PCPCI_BUS_WAKE_STATE;
 
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciBus_QueryPower (
    _Inout_ PIRP Irp,
    _In_ PIO_STACK_LOCATION IrpSp,
    _In_ PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_SetPower(
    _Inout_ PIRP Irp,
    _In_ PIO_STACK_LOCATION IrpSp,
    _Inout_ PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBus_WaitWake (
    _Inout_ PIRP Irp,
    _In_ PIO_STACK_LOCATION IrpSp,
    _Inout_ PPCI_COMMON_EXTENSION DeviceExtension
    );

NTSTATUS
PciBusHandleSystemPower (
    _In_ PPCI_BUS Bus,
    _Inout_ PIRP Irp
    );

NTSTATUS
PciBusPowerDown (
    _Inout_ PPCI_BUS Bus,
    _In_ PIO_STACK_LOCATION IrpSp
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
PciBus_GenericWakePower (
    _In_ PPCI_BUS Bus,
    _In_ BOOLEAN PowerUp
    );

REQUEST_POWER_COMPLETE PciBus_PowerCompletion;
IO_COMPLETION_ROUTINE PciBusSystemPowerCompletion;
IO_COMPLETION_ROUTINE PciBusPowerUpCompletion;
PO_FX_DEVICE_POWER_REQUIRED_CALLBACK PciBusDevicePowerRequired;
PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK PciBus_PoFxActive;
PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK PciBus_PoFxIdle;
PO_FX_COMPONENT_IDLE_STATE_CALLBACK PciBus_PoFxIdleState;
PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK PciBusDevicePowerNotRequired;
PCI_POWER_POLICY_STATE_FUNC PciBus_LeavingS0DxDisarming;
PCI_POWER_POLICY_STATE_FUNC PciBus_S0D0;
PCI_POWER_POLICY_STATE_FUNC PciBus_S0Disarmed;
PCI_POWER_POLICY_STATE_FUNC PciBus_S0Dozing;
PCI_POWER_POLICY_STATE_FUNC PciBus_S0DozingArmed;
PCI_POWER_POLICY_STATE_FUNC PciBus_S0Off;
PCI_POWER_POLICY_STATE_FUNC PciBus_S0Stirring;
PCI_POWER_POLICY_STATE_FUNC PciBus_StirringD0;
PCI_POWER_POLICY_STATE_FUNC PciBus_StirringDisarmed;
PCI_POWER_POLICY_STATE_FUNC PciBus_SxArmed;
PCI_POWER_POLICY_STATE_FUNC PciBus_Sx;
PCI_POWER_POLICY_STATE_FUNC PciBus_SxOn;

// --------------------------------------------------------------------- Pragmas

#pragma alloc_text(PAGE, BusPowerRegisterPep)
#pragma alloc_text(PAGE, BusPowerUnregisterPep)
#pragma alloc_text(PAGE, PciBus_RunDownWake)

    //
    // Nothing else in the power code is pageable.
    //

// --------------------------------------------------------------------- Globals

const PCI_MN_DISPATCH_ENTRY PciBus_DispatchPowerTable[] = {
    { IRP_DISPATCH, PciBus_WaitWake     , FALSE}, // 0x00 - IRP_MN_WAIT_WAKE
    { IRP_DOWNWARD, PciIrpNotSupported  , FALSE}, // 0x01 - IRP_MN_POWER_SEQUENCE
    { IRP_DISPATCH, PciBus_SetPower     , FALSE}, // 0x02 - IRP_MN_SET_POWER
    { IRP_DISPATCH, PciBus_QueryPower   , FALSE}, // 0x03 - IRP_MN_QUERY_POWER
    { IRP_DOWNWARD, PciIrpNotSupported  , FALSE}  //      - UNHANDLED Power IRP
};


//
// What follows is a state machine for dealing with arming buses for wake.
// 
// The state machine itself is documented in a Visio file which should still be
// in the same directory as this file - "PCI Power.vdx".  This state table
// encoding will make much, much more sense if you open that file before reading
// any further.
// 
// The rules for operation are:
// 
//  A)  The Wait/Wake IRP must be sent at PASSIVE_LEVEL.
// 
//  B)  The W/W IRP must be passed down synchronously, so that the sender can
//      know that the device is armed for wake at the point where
//      PoRequestPowerIrp completes.
// 
//  C)  Completion of W/W IRPs is (like any other IRP) allowed to occur at 
//      DISPATCH_LEVEL.  That's really inconvenient, though, because then you
//      can't turn around and re-send the W/W IRP in the completion routine
//      (see (A) above.)
// 
//  D)  Some drivers in the stack may not comprehend (C), so the PCI driver
//      always completes W/W IRPs at PASSIVE_LEVEL, which will tend to present
//      them at the sender's completion routine at PASSIVE_LEVEL.  This matters
//      mostly for non-PCI FDOs on top of PCI PDOs.  Even so, this PCI-owned
//      FDO is going to do the same think so that filters aren't confused.  (For
//      lots of reasons, there are a lot of filters in the world that filter all
//      PCI devices, bridge or otherwise.)
// 
//  E)  The bus should be armed for wake if one or more children don't support
//      Native Express PME and the PCI PDO associated with the device has 
//      received a W/W IRP.  (This is distinct from the device actually being
//      armed for wake, as ACPI may have swallowed a W/W IRP if the device
//      is described in the ACPI namespace with its own wake-related objects,
//      indicating that wake for that device is directly wired up to an ACPI-
//      owned General Purpose Event (GPE) pin.  See http://acpi.info for more
//      information.)
// 
//  F)  If the system is leaving S0, then the bus should be armed for wake if it
//      has received a W/W IRP for any child, regardless of Native Express PME
//      support, if and only if the power capabilites for this bus include 
//      being able to trigger wake from some D-state that will be available in
//      the target S-state.
// 
//  G)  The bus should not be allowed to leave D0 while the system is in S0 if
//      any child which is armed for wake cannot wake from D3cold.
// 
//  H)  The bus should not be allowed to leave D0 while the system is in S0 if
//      the power capabilites of the bus (which ultimately derive from the ACPI
//      namespace) don't allow this bus to wake from D3.  This is because we
//      have no evidence that putting the bus in D1 or D2 would actually save
//      much power and the behavior for a bus in those states isn't very well
//      defined.  If either of those two things changes, then perhaps this
//      should be reconsidered.
// 
//  I)  If anything fails in the W/W state machine, the right thing to do is to
//      complete all the W/W IRPs at all the children.  They are supposed to be
//      able to deal with spurious wakeup (and they have been doing so in other
//      situations.)  If they choose to rearm themselves, that's good and it
//      will restart this state machine.
// 
//  J)  No power references will be added or removed during S-state transitions,
//      since the power policy owners of the child stacks should have completed
//      their work before completing their Sx IRPs, and an Sx IRP won't be sent
//      to this node by the Power Manager (Po) unless the children have already
//      processed theirs.
// 
//  K)  Wake references must precede power reference (because the WDK says, for
//      good reason, that W/W IRPs must be sent down the stack before Dx IRPs
//      are sent.)
// 
//  L)  Power dereferences cannot follow power references until the device has
//      passed through D0.  This applies to in-line code from a specific child.
//      Obviously, different children will act differently.  But a specific
//      piece of code which requests that the bus come to D0 must actually wait
//      for the bus to come to D0 before it allows the bus to drop to a low-
//      power state.
//

const PCI_BUS_POWER_POLICY_STATE PciBus_State[PciBusPowerMaximum] =
{

    // PciBusPowerLeavingS0ArmedDx
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerLeavingS0Dx,            // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        NULL,
        0
    },

    // PciBusPowerLeavingS0Dx
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerLeavingS0DxDisarming    // PciBusEventWakeReady
        },
        NULL,
        0
    },

    // PciBusPowerLeavingS0DxDisarmed
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerSxOn,                   // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        NULL,
        0
    },

    // PciBusPowerLeavingS0DxDisarming
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        PciBus_LeavingS0DxDisarming,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerS0D0
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        PciBus_S0D0,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerS0Disarmed
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerS0D0,                   // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        PciBus_S0Disarmed,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerS0Dozing
    {
        {
         PciBusPowerS0DozingStirring,       // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerS0DozingArmed           // PciBusEventWakeReady
        },
        PciBus_S0Dozing,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerS0DozingArmed
    {
        {
         PciBusPowerS0DozingArmedStirring,  // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerS0Dx,                   // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        PciBus_S0DozingArmed,
        0
    },

    // PciBusPowerS0DozingArmedStirring
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerLeavingS0ArmedDx,       // PciBusEventLeavingS0
         PciBusPowerS0Stirring,             // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        NULL,
        0
    },

    // PciBusPowerS0DozingStirring
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerSxDozingStirring,       // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerS0Stirring              // PciBusEventWakeReady
        },
        NULL,
        0
    },

    // PciBusPowerS0Dx
    {
        {
         PciBusPowerS0Stirring,             // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        NULL,
        0
    },

    // PciBusPowerS0Off
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerS0Disarmed              // PciBusEventWakeReady
        },
        PciBus_S0Off,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerS0On
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerS0Dozing,               // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerSxOn,                   // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        NULL,
        0
    },

    // PciBusPowerS0Stirring
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerLeavingS0Dx,            // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerStirringDisarmed        // PciBusEventWakeReady
        },
        PciBus_S0Stirring,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerStirringD0
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        PciBus_StirringD0,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerStirringDisarmed
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerLeavingS0DxDisarmed,    // PciBusEventLeavingS0
         PciBusPowerStirringD0,             // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        PciBus_StirringDisarmed,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerSx
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerS0Off,                  // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        PciBus_Sx,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerSxArmed
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerSx,                     // PciBusEventPoRequestComplete
         PciBusPowerMaximum                 // PciBusEventWakeReady
        },
        PciBus_SxArmed,
        PciBusPowerPolicyFlagLocked
    },

    // PciBusPowerSxDozingStirring
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerLeavingS0ArmedDx        // PciBusEventWakeReady
        },
        NULL,
        0
    },

    // PciBusPowerSxOn
    {
        {
         PciBusPowerMaximum,                // PciBusEventFirstPowerRefAdded 
         PciBusPowerMaximum,                // PciBusEventLastPowerRefRemoved
         PciBusPowerMaximum,                // PciBusEventEnteringS0
         PciBusPowerMaximum,                // PciBusEventLeavingS0
         PciBusPowerMaximum,                // PciBusEventPoRequestComplete
         PciBusPowerSxArmed                 // PciBusEventWakeReady
        },
        PciBus_SxOn,
        PciBusPowerPolicyFlagLocked
    }
};

const PCI_BUS_WAKE_STATE PciExpressWakeStates[PciExpressWakeMaximum] =
{
    // PciExpressWakeArmedOff
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeDisarmingOff,        // PciWakeEventLastWakeRefRemoved
         PciExpressWakeTriggeredOff,        // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeArmedStirring,       // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeArmedOn
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeDisarmedOn,          // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeQueuingDozing        // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagMarkArmed,
        PciExpressWakeMaximum
    },

    // PciExpressWakeArmedStirring
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagCancelWW,
        PciExpressWakeArmedStirringWait
    },

    // PciExpressWakeArmedStirringWait
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeDisarmingStirringWait,// PciWakeEventLastWakeRefRemoved
         PciExpressWakeStirringTriggered,   // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeStirringCancelled,   // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeCompleteDereferenced
    {
        {
         PciExpressWakeQueuedTriggered,     // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeTriggeredDereferenced,// PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeDereferencedSent
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciExpressWakeDisarmingOff
    },

    // PciExpressWakeDisarmedDozing
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciExpressWakeDisarmedOff
    },

    // PciExpressWakeDisarmedOff
    {
        {
         PciExpressWakeTriggeredOff,        // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeDisarmedStirring,    // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeDisarmedOn
    {
        {
         PciExpressWakeArmedOn,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeDisarmedDozing       // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagMarkDisarmed | PciBusPowerPolicyFlagSignalRemoval,
        PciExpressWakeMaximum
    },

    // PciExpressWakeDisarmedStirring
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciExpressWakeDisarmedOn
    },

    // PciExpressWakeDisarmingCancelled
    {
        {
         PciExpressWakeStirringCancelled,   // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeDisarmedStirring,    // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeDisarmingComplete
    {
        {
         PciExpressWakeStirringTriggered,   // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeDisarmedStirring,    // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeDisarmingOff
    {
        {
         PciExpressWakeArmedOff,            // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeDisarmingTriggered,  // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeDisarmingStirring,   // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeDisarmingStirring
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagCancelWW,
        PciExpressWakeDisarmingStirringWait
    },

    // PciExpressWakeDisarmingStirringWait
    {
        {
         PciExpressWakeArmedStirringWait,   // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeDisarmingComplete,   // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeDisarmingCancelled,  // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeDisarmingTriggered
    {
        {
         PciExpressWakeTriggeredOff,        // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeDisarmedStirring,    // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeQueuedDereferenced
    {
        {
         PciExpressWakeQueuedDozing,        // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeCompleteDereferenced,// PciWakeEventWaitWakeComplete
         PciExpressWakeDereferencedSent,    // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeQueuedDozing
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeQueuedDereferenced,  // PciWakeEventLastWakeRefRemoved
         PciExpressWakeQueuedTriggered,     // PciWakeEventWaitWakeComplete
         PciExpressWakeQueuedSent,          // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeQueuedSent
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciExpressWakeArmedOff
    },

    // PciExpressWakeQueuedTriggered
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeCompleteDereferenced,// PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeTriggeredComplete,   // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeQueuingDozing
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagSendWW,
        PciExpressWakeQueuedDozing
    },

    // PciExpressWakeStirringCancelled
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeDisarmingCancelled,  // PciWakeEventLastWakeRefRemoved
         PciExpressWakeTriggeredReady,      // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeStirringTriggered
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeDisarmingComplete,   // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeTriggeredReady,      // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeTriggeredComplete
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciExpressWakeTriggeredOff
    },

    // PciExpressWakeTriggeredDereferenced
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciExpressWakeDisarmingTriggered
    },

    // PciExpressWakeTriggeredOff
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeDisarmingTriggered,  // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeTriggeredReady,      // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        0,
        PciExpressWakeMaximum
    },

    // PciExpressWakeTriggeredReady
    {
        {
         PciExpressWakeMaximum,             // PciWakeEventFirstWakeRefAdded
         PciExpressWakeMaximum,             // PciWakeEventLastWakeRefRemoved
         PciExpressWakeMaximum,             // PciWakeEventWaitWakeComplete
         PciExpressWakeMaximum,             // PciWakeEventWorkerSentWaitWake
         PciExpressWakeMaximum,             // PciWakeEventWorkerCanceled
         PciExpressWakeMaximum,             // PciWakeEventPowerUp
         PciExpressWakeMaximum              // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciExpressWakeArmedOn
    }
};

const PCI_BUS_WAKE_STATE PciConventionalWakeStates[PciConvWakeMaximum] =
{
    // PciConvWakeArmed
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeDereferenced,           // PciWakeEventLastWakeRefRemoved
         PciConvWakeReferenced,             // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeArmedDozing             // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagMarkArmed,
        PciConvWakeMaximum
    },

    // PciConvWakeArmedDozing
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciConvWakeArmed
    },

    // PciConvWakeCanceledReferenced
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeReferencedCanceled,     // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeCanceledReferencedIncomplete, // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeCanceledReferencedDozing// PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeCanceledReferencedDozing
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeDozingReferencedComplete, // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeDozingReferencedCanceled, // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeCanceledReferencedIncomplete
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeDereferencedCanceled,   // PciWakeEventLastWakeRefRemoved
         PciConvWakeReferenced,             // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeDozingReferencedCanceled// PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeCanceledWait
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeDisarmedDozing,         // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeDereferenced
    {
        {
         PciConvWakeCanceledReferenced,     // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeDereferencedTriggered,  // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeDereferencedCanceled,   // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeDereferencedDozing      // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagCancelWW,
        PciConvWakeMaximum
    },

    // PciConvWakeDereferencedCanceled
    {
        {
         PciConvWakeCanceledReferencedIncomplete, // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeDisarmed,               // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeCanceledWait            // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeDereferencedDozing
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeDereferencedWaiting,    // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeCanceledWait,           // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeDereferencedTriggered
    {
        {
         PciConvWakeReferencedCanceled,     // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeDisarmed,               // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeDereferencedWaiting     // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeDereferencedWaiting
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeDisarmedDozing,         // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeDisarmed
    {
        {
         PciConvWakeReferenced,             // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeDisarmed,               // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeDisarmedDozing          // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagMarkDisarmed | PciBusPowerPolicyFlagSignalRemoval,
        PciConvWakeMaximum
    },

    // PciConvWakeDisarmedDozing
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciConvWakeDisarmed
    },

    // PciConvWakeDozingReferenced
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagSendWW,
        PciConvWakeQueuedDozing
    },

    // PciConvWakeDozingReferencedCanceled
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeDozingReferenced,       // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeDozingReferencedComplete
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeDozingReferenced,       // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeQueuedDereferenced
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeQueuedTriggered,        // PciWakeEventWaitWakeComplete
         PciConvWakeDereferenced,           // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeQueuedDozing
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeTriggeredQueuedDozing,  // PciWakeEventWaitWakeComplete
         PciConvWakeArmedDozing,            // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeQueuedTriggered
    {
        {
         PciConvWakeTriggeredQueued,        // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeDisarmed,               // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeReferenced
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagSendWW,
        PciConvWakeWWQueued
    },

    // PciConvWakeReferencedCanceled
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeReferenced,             // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeTriggeredDozing
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,               // PciWakeEventWaitWakeComplete
         PciConvWakeMaximum,                // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        PciBusPowerPolicyFlagWakeReady,
        PciConvWakeReferenced
    },

    // PciConvWakeTriggeredQueued
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeReferenced,             // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeTriggeredQueuedDozing
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeMaximum,                // PciWakeEventLastWakeRefRemoved
         PciConvWakeMaximum,                // PciWakeEventWaitWakeComplete
         PciConvWakeTriggeredDozing,        // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeMaximum                 // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },

    // PciConvWakeWWQueued
    {
        {
         PciConvWakeMaximum,                // PciWakeEventFirstWakeRefAdded
         PciConvWakeQueuedDereferenced,     // PciWakeEventLastWakeRefRemoved
         PciConvWakeTriggeredQueued,        // PciWakeEventWaitWakeComplete
         PciConvWakeArmed,                  // PciWakeEventWorkerSentWaitWake
         PciConvWakeMaximum,                // PciWakeEventWorkerCanceled
         PciConvWakeMaximum,                // PciWakeEventPowerUp
         PciConvWakeQueuedDozing            // PciWakeEventPowerDown
        },
        0,
        PciConvWakeMaximum
    },
};


// ----------------------------------------------------------- Utility Functions

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
BusPowerRegisterPep(
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine registers with the runtime power framework, interlocking power
    policy decisions with the PEP.

Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    success unless PO has a problem with that

--*/

{

    ULONG fState;
    static PO_FX_COMPONENT_IDLE_STATE idleState[PCI_FSTATE_COUNT];
    static PO_FX_DEVICE fxTable;
    static const GUID pciComponent =
        {0x4310047d, 0x5916, 0x442c, {0x89, 0x15, 0xe8, 0x67, 0x21, 0xaa, 0xc7, 0xf2}};
		
    NTSTATUS status;

    PAGED_CODE();

    for (fState = 0; fState < PCI_FSTATE_COUNT; fState++) {
        idleState[fState].TransitionLatency     = 1000000 * fState;  // 100ms * fState index
        idleState[fState].ResidencyRequirement  = 0;        // Probably better supplied by the child device
        idleState[fState].NominalPower          = PO_FX_UNKNOWN_POWER; // Can't really know this
    }

    RtlZeroMemory(&fxTable, sizeof(fxTable));
    fxTable.Version                             = PO_FX_VERSION_V1;
    fxTable.ComponentCount                      = 1;
    fxTable.ComponentActiveConditionCallback    = PciBus_PoFxActive;
    fxTable.ComponentIdleConditionCallback      = PciBus_PoFxIdle;
    fxTable.ComponentIdleStateCallback          = PciBus_PoFxIdleState;
    fxTable.DevicePowerRequiredCallback         = PciBusDevicePowerRequired;
    fxTable.DevicePowerNotRequiredCallback      = PciBusDevicePowerNotRequired;
    fxTable.DeviceContext                       = Bus;

    /* 4310047d-5916-442c-8915-e86721aac7f2 */
    fxTable.Components[0].Id                        = pciComponent;
    fxTable.Components[0].IdleStateCount            = PCI_FSTATE_COUNT;
    fxTable.Components[0].IdleStates                = idleState;
    fxTable.Components[0].DeepestWakeableIdleState  = PCI_FSTATE_COUNT - 1;

    status = PoFxRegisterDevice(Bus->PhysicalDeviceObject,
                                &fxTable,
                                &Bus->PepHandle);

    if (NT_SUCCESS(status)) {

        //
        // The number of times we call PoFxActivateComponent below should match
        // the number of power references we're tracking internally.
        //

        {
            LONG powerReferences = 0;
            USHORT reason;

            _No_competing_thread_begin_
            for (reason = 0; reason < PciBusPowerReasonMaximum; reason++) {
                powerReferences += Bus->BusPowerReferenceReason[reason];
            }
            _No_competing_thread_end_

            PCI_ASSERT(powerReferences == 1);
        }

        PoFxSetDeviceIdleTimeout(Bus->PepHandle, (ULONGLONG)-1);
        PoFxActivateComponent(Bus->PepHandle, 0, PO_FX_FLAG_ASYNC_ONLY);
        PoFxStartDevicePowerManagement(Bus->PepHandle);

        //
        // If the BIOS doesn't support idling in S0, then add a power reference
        // so that this bus always remains turned on except when the system
        // leaves S0.
        //

        if ((Bus->IdleDState <= DeviceWakeDepthD0) &&
            !(PciSystemWideHackFlags & PCI_SYS_HACK_EVERYBODY_SUPPORTS_D3COLD)) {
            PciBus_PowerReference(Bus, PciBusPowerAllowF1DisableBridgeD3);
        }
    }
    
    return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
BusPowerUnregisterPep(
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine unregisters with the runtime power framework.

Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    PAGED_CODE();
    HANDLE pepHandle;

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): BusPowerUnregisterPep called",
                Bus->SecondaryBusNumber);

    if (Bus->PepHandle != NULL) {

        //
        // As part of the unregister process, POFX will
        // activate the device, which causes a race condition
        // where PCI may call active or idle on a torn down 
        // fxdevice.  To avoid this situation, NULL out the 
        // PepHandle, and avoid active/idle calls when the 
        // PepHandle is NULL.
        //

        pepHandle = Bus->PepHandle;
        Bus->PepHandle = NULL;
        PoFxUnregisterDevice(pepHandle);
    }
}

_Function_class_(PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_PoFxActive (
    _In_ PVOID Context,
    _In_ ULONG Component
    )

/*++

Routine Description:

    The runtime power framework invokes this callback as a promise that it will
    not invoke PciBusDevicePowerRequired from now until the time that this
    driver again calls PoFxIdleComponent.

Arguments:

    Context - Supplies the bus extension for the root bus/bridge.
 
Return Value:

    none

--*/

{
    PPCI_BUS Bus = (PPCI_BUS)Context;

    NON_PAGED_CODE();
    _Analysis_assume_(Bus != NULL);

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "PoFx says active");

    PCI_ASSERT((Bus->BusPowerState == PciBusPowerS0On) ||
               (Bus->BusPowerState == PciBusPowerStirringD0));

    //
    // Mark power available and let anything waiting on that continue.
    //
    PCI_BUS_SET_FLAG(Bus, BusPowerAvailable);
    KeSetEvent(&Bus->BusPowerEvent, IO_NO_INCREMENT, FALSE);

    //
    // If any child devices have blocked while the bus was idle, unblock them
    // here.
    //

    PciBus_SignalWorkerRetireD0Irps(Bus);

    return;
    UNREFERENCED_PARAMETER(Component);
}

_Function_class_(PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_PoFxIdle (
    _In_ PVOID Context,
    _In_ ULONG Component
    )

/*++

Routine Description:

    The runtime power framework invokes this callback to indicate that it might
    call PciBusDevicePowerRequired between now and when this driver calls
    PoFxActivateComponent.

Arguments:

    Context - Supplies the bus extension for the root bus/bridge.
 
Return Value:

    none

--*/

{
    PPCI_BUS Bus = (PPCI_BUS)Context;

    NON_PAGED_CODE();
    _Analysis_assume_(Bus != NULL);

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "PoFx says idle");

    //
    // Mark this bus as one that may be turned off, and thus any action which
    // depends on the bus remaining powered on should be blocked.
    //

    KeClearEvent(&Bus->BusPowerEvent);
    PCI_BUS_CLEAR_FLAG(Bus, BusPowerAvailable);

    PoFxCompleteIdleCondition(Bus->PepHandle, Component);
    return;
}

_Function_class_(PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_PoFxIdleState (
    _In_ PVOID Context,
    _In_ ULONG Component,
    _In_ ULONG State
    )

{

    PPCI_BUS Bus = (PPCI_BUS)Context;

    NON_PAGED_CODE();
    _Analysis_assume_(Bus != NULL);

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): PoFx says to move to F-State %d",
                Bus->SecondaryBusNumber,
                State);

    PoFxCompleteIdleState(Bus->PepHandle, Component);
    return;
}

__drv_functionClass(PO_FX_DEVICE_POWER_REQUIRED_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBusDevicePowerRequired (
    _In_ PVOID Context
    )

/*++

Routine Description:

    The runtime power framework invokes this callback to add a power reference
    from this bus.

Arguments:

    Context - Supplies the bus extension for the root bus/bridge.
 
Return Value:

    none

--*/

{
    PPCI_BUS Bus = (PPCI_BUS)Context;

    NON_PAGED_CODE();
    _Analysis_assume_(Bus != NULL);

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): PoFx says power required",
                Bus->SecondaryBusNumber);
    PCI_BUS_CLEAR_FLAG(Bus, PepPoweredOff);
    PciBus_PowerStateM(Bus, PciBusEventFirstPowerRefAdded);
}

__drv_functionClass(PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBusDevicePowerNotRequired (
    _In_ PVOID Context
    )

/*++

Routine Description:

    The runtime power framework invokes this callback to remove a power
    reference from this bus.

Arguments:

    Context - Supplies the bus extension for the root bus/bridge.
 
Return Value:

    none

--*/

{
    PPCI_BUS Bus = (PPCI_BUS)Context;

    NON_PAGED_CODE();
    _Analysis_assume_(Bus != NULL);

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): PoFx says power not required",
                Bus->SecondaryBusNumber);
    PCI_BUS_SET_FLAG(Bus, PepPoweredOff);
    PciBus_PowerStateM(Bus, PciBusEventLastPowerRefRemoved);
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_QueueChildD0Irp (
    _Inout_ PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine marks the IRP pending and then references the parent bus's
    power.  If the parent bus is in D0, processing for this IRP will happen
    immediately.  If not, it will happen once the parent reaches D0.

Arguments:

    Device - The extension for the child device.
 
Return Value: 
 
    none 

--*/

{
    NON_PAGED_CODE();
    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "Queuing D0 IRP %p for device (%x:%x)",
                Device->DIrp,
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    PCI_DEVICE_SET_FLAG(Device, WaitingForPowerUp);

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_RetireChildPowerIrps (
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine looks through the list of child devices that have power IRPs
    unretired and calls into the correct module to retire them, now that the
    parent bus is in D0.

Arguments:

    Bus - Pointer to a PCI bus context.
 
Return Value: 
 
    none 

--*/

{
#define TIMES_TO_POLL_LINK  10
    PPCI_DEVICE Child;
    LARGE_INTEGER Delay;
    ULONG Index;
    KIRQL OldIrql;
    BOOLEAN Retire;
    PCI_EXPRESS_LINK_STATUS_REGISTER StatusReg;
    BOOLEAN WaitForLink;

    NON_PAGED_CODE();

    //
    // If this is an Express bridge for a downstream port, then look at the 
    // Link Status Register's "Data Link Layer Link Active Bit," if present,
    // and wait for it to be set.  If it doesn't exist, just wait for a while
    // to allow the link to settle.
    //

    if (!PCI_ROOT_BUS(Bus) &&
        (Bus->PciBridge->ExpressBridge != NULL)) {

        WaitForLink = FALSE;
        if (DOWNSTREAM_PORT(Bus->PciBridge->ExpressBridge)) {
            if (Bus->PciBridge->ExpressBridge->Downstream.ExpressLink.LinkCapabilities.DataLinkLayerActiveReportingCapable == 1) {
                WaitForLink = TRUE;
            }
        }

        if (WaitForLink == FALSE) {

            Bus->PciBridge->DelayPassive(Bus->PciBridge->InterfaceContext, DLUpTime);

        } else {

            for (Index = 0; Index < TIMES_TO_POLL_LINK; Index++) {

                PciReadExpressBridgeRegister(Bus->PciBridge->ExpressBridge,
                                             LinkStatus,
                                             &StatusReg);

                if (StatusReg.DataLinkLayerActive == 1) {
                    break;
                }

                Delay.QuadPart = -1 * 10000 * PORT_POWER_ON_SETTLE_TIME / TIMES_TO_POLL_LINK;
                KeDelayExecutionThread(KernelMode, FALSE, &Delay);
            }
        }
    }

    //
    // Scan children and handle their Wait/Wake IRPs.  Express
    // ports should have already handled waiting PME interrupts at the moment
    // that the device entered D0.  This is just to catch any stragglers and to
    // handle non-"native" PME.  If the still have W/W IRPs pending, just
    // complete the W/W IRP and let them sort it out.
    //
    // If a child is waiting to be powered up then retire its D0 IRP, otherwise
    // notify the runtime power framework that the device needs to be surprise
    // powered on. We don't want to  notify the runtime power framework for a 
    // device that might have triggered a PME because that will result in the
    // device receiving a surprise powered on event.
    //

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    PCI_BUS_CLEAR_FLAG(Bus, RetiringChildD0Irps);

    for (Child = Bus->ChildDevices;
         Child != NULL;
         Child = Child->Sibling) {

        PCI_ASSERT(PCI_DEVICE_EXTENSION(Child));

        if (Child->WaitingForPowerUp != FALSE) {

            //
            // Check again while holding the spinlock.
            //

            Retire = FALSE;

            KeAcquireSpinLock(&Child->D0IrpLock, &OldIrql);
            if (Child->WaitingForPowerUp != FALSE) {
                PCI_DEVICE_CLEAR_FLAG(Child, WaitingForPowerUp);
                Retire = TRUE;
            }
            KeReleaseSpinLock(&Child->D0IrpLock, OldIrql);

            if (Retire != FALSE) {
                PciDevice_RetireD0Irp(Child);
            }

        } else if (Child->WakeIrpState == PciWakeIrpQueued) {
            
            PciDevice_WakeTriggered(Child, 
                                    Bus->WokeSystem, 
                                    Bus->WakeIrpStatus);

        } else {
            PoFxNotifySurprisePowerOn(Child->DeviceObject);
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // Drop the power reference that was added before this was queued to the
    // worker thread.
    //

    PciBus_PowerDereference(Bus, PciBusPowerReasonRetiringChildD0Irps);
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_ConventionalWakeTriggered (
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine is called when a wait/wake IRP completes for a conventional
    PCI bus which is already powered on.  This need not be called for buses
    which have their wait/wake IRPs completed while they are off.  That will
    be taken care of in PciBus_RetireChildPowerIrps.

Arguments:

    Bus - Pointer to a PCI bus context.
 
Return Value: 
 
    none 

--*/

{
    PPCI_DEVICE Child;
    PCI_PMCSR PmCsr;
    KIRQL OldIrql;
    BOOLEAN WakeReferenced;

    PCI_ASSERT(Bus->BusPowerAvailable == 1);

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    for (Child = Bus->ChildDevices;
         Child != NULL;
         Child = Child->Sibling) {

        PCI_ASSERT(PCI_DEVICE_EXTENSION(Child));

        if (Child->WakeIrpState == PciWakeIrpQueued) {


            //
            // If the child device is a P2P bridge with a wait/wake IRP pending,
            // just complete it and let that bus sort out what's going on.
            //

            if ((Child->BaseClass == PCI_CLASS_BRIDGE_DEV) &&
                (Child->SubClass == PCI_SUBCLASS_BR_PCI_TO_PCI)) {
            
                PciDevice_WakeTriggered(Child, 
                                        Bus->WokeSystem, 
                                        Bus->WakeIrpStatus);
                
            //
            // If the device has no PM caps, and it has a wait/wake IRP pending,
            // just complete it.
            //
           
            } else if (Child->HackFlags & PCI_HACK_NO_PM_CAPS) {

                PciDevice_WakeTriggered(Child, 
                                        Bus->WokeSystem, 
                                        Bus->WakeIrpStatus);

            //
            // For non-bridge devices, read the PME status register to see if
            // the device is actually triggering PME, and if so, complete its
            // wait/wake IRP.  If the wake IRP failed at this layer, fail it at
            // all layers, regardless of whether the PME status bit was set.
            //
            
            } else {

                PciReadPowerCapabilityRegister(Child, PMCSR, &PmCsr);
    
                if (PmCsr.PMEStatus == 1) {
    
                    PciDevice_WakeTriggered(Child, 
                                            Bus->WokeSystem, 
                                            STATUS_SUCCESS);
    
                } else if (!NT_SUCCESS(Bus->WakeIrpStatus)) {
    
                    PciDevice_WakeTriggered(Child, 
                                            Bus->WokeSystem, 
                                            Bus->WakeIrpStatus);
                }
            }
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // At this point, the bus power is maintained because references were taken
    // if we completed any wait/wake IRPs above.  So we can drop the reference
    // that we added when completing the wait/wake IRP.
    //

    KeAcquireSpinLock(&Bus->BusPowerLock, &OldIrql);
    WakeReferenced = Bus->WakePowerReferenced == 1;
    if (WakeReferenced != FALSE) {
        PCI_BUS_CLEAR_FLAG(Bus, WakePowerReferenced);
    }
    KeReleaseSpinLock(&Bus->BusPowerLock, OldIrql);

    if (WakeReferenced != FALSE) {
        PciBus_PowerDereference(Bus, PciBusPowerReasonBusWaitWakeComplete);
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
VOID
PciBus_GenericWakePower (
    _In_ PPCI_BUS Bus,
    _In_ BOOLEAN PowerUp
    )

/*++

Routine Description:

    This routine queues an event to the worker thread which will eventually
    cause an event to be queued to the wake state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    if (PowerUp != FALSE) {
        PCI_BUS_SET_FLAG(Bus, WakeStatePowerOn);
    } else {
        PCI_BUS_CLEAR_FLAG(Bus, WakeStatePowerOn);
    }

    ExInterlockedInsertTailList(&Bus->Root->PmeQueueWakeEventList,
                                &Bus->WaitingForWaitStateMachine,
                                &Bus->Root->PmeListLock);
    KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventQueueWakeStateEvent],
               IO_NO_INCREMENT,
               FALSE);

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_PassWakeEvent (
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine inserts an event in the wake state machine, while running on
    the worker thread.
 
    The conventional wake state machine is special-cased here.  A bus-power-on
    event is always synchronously acknowledged without running the state
    machine.

Arguments:

    Bus - Pointer to a PCI bus context.
 
Return Value: 
 
    none 

--*/

{
    NON_PAGED_CODE();

    if (!(PCI_BUS_USES_NATIVE_PME(Bus)) &&
        (Bus->WakeStatePowerOn == 1)) {
        PciBus_PowerStateM(Bus, PciBusEventWakeReady);
        return;
    }

    if (Bus->WakeStatePowerOn == 1) {
        PciBus_WakeStateM(Bus, PciWakeEventPowerUp);
    } else {
        PciBus_WakeStateM(Bus, PciWakeEventPowerDown);
    }

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_RunDownWake (
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine ensures that the wake state machine isn't doing anything any
    more, which is necessary at time of bus removal.

Arguments:

    Bus - Pointer to a PCI bus context.
 
Return Value: 
 
    none 

--*/

{
    PAGED_CODE();

    KeClearEvent(&Bus->BusPowerEvent);
    PCI_BUS_SET_FLAG(Bus, Removing);
    while (Bus->ChildWakeCount != 0) {
        PciBus_DisarmForWake(Bus);
    }

    if (Bus->ArmedForWake == 1) {
        KeWaitForSingleObject(&Bus->BusPowerEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
    }

    return;
}

_Function_class_(REQUEST_POWER_COMPLETE)
_IRQL_requires_same_
VOID
PciBus_PowerCompletion (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ UCHAR MinorFunction,
    _In_ POWER_STATE PowerState,
    _In_opt_ PVOID Context,
    _In_ PIO_STATUS_BLOCK IoStatus
    )

/*++

Routine Description:

    This is invoked upon completion of a D-state IRP.

Arguments:

    DeviceObject - unused
 
    MinorFunction - unused
 
    PowerState - unused
 
    Context - Pointer to a bus device extension.
 
    IoStatus - unused
 
Return Value: 
 
    none 

--*/

{
    PPCI_BUS Bus = (PPCI_BUS)Context;
    KIRQL OldIrql;
    BOOLEAN WakeReferenced;

    NON_PAGED_CODE();
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(MinorFunction);
    _Analysis_assume_(Bus != NULL);

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): %!STATUS!",
                Bus->SecondaryBusNumber,
                IoStatus->Status); 

    if (!PCI_CARDBUS_BRIDGE(Bus)) {
        PciBus_PowerStateM(Bus, PciBusEventPoRequestComplete);
    }

    //
    // If the bus is turning on because of a wait/wake IRP, or because we need
    // to scan for PME events on this bus, drop those references.
    //

    if (PowerState.DeviceState == PowerDeviceD0) {

        if (Bus->PmePowerReferenced == 1) {
            PCI_ASSERT(!PCI_BUS_USES_NATIVE_PME(Bus));
            PCI_BUS_CLEAR_FLAG(Bus, PmePowerReferenced);
            PciBus_PowerDereference(Bus, PciBusPowerReasonPmeTriggered);
        }

        //
        // If the wait/wake IRP completed while the bus was not in D0 (as should
        // always happen with Express and sometimes happen with Conventional)
        // then there will still be a power reference.  We can drop that now.
        //

        KeAcquireSpinLock(&Bus->BusPowerLock, &OldIrql);
        WakeReferenced = Bus->WakePowerReferenced == 1;
        if (WakeReferenced != FALSE) {
            PCI_BUS_CLEAR_FLAG(Bus, WakePowerReferenced);
        }
        KeReleaseSpinLock(&Bus->BusPowerLock, OldIrql);

        if (WakeReferenced != FALSE) {
            PciBus_PowerDereference(Bus, PciBusPowerReasonBusWaitWakeComplete);
        }
    }
}

__drv_functionClass(REQUEST_POWER_COMPLETE)
_IRQL_requires_same_
VOID
PciBus_WaitWakeCompletion (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ UCHAR MinorFunction,
    _In_ POWER_STATE PowerState,
    _In_opt_ PVOID Context,
    _In_ PIO_STATUS_BLOCK IoStatus
    )

/*++

Routine Description:

    This is invoked upon completion of a wait/wake IRP.

Arguments:

    DeviceObject - unused
 
    MinorFunction - unused
 
    PowerState - unused
 
    Context - Pointer to a bus device extension.
 
    IoStatus - unused
 
Return Value: 
 
    none 

--*/

{
    PPCI_BUS Bus = (PPCI_BUS)Context;
    KIRQL OldIrql;

    NON_PAGED_CODE();
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(MinorFunction);
    UNREFERENCED_PARAMETER(PowerState);
    _Analysis_assume_(Bus != NULL);

    TraceEvents(Bus->BlackBoxHandle,
                NT_SUCCESS(IoStatus->Status) ? TRACE_LEVEL_INFORMATION : TRACE_LEVEL_WARNING,
                DBG_PPO,
                "(%x): wait wake completed %!STATUS!",
                Bus->SecondaryBusNumber,
                IoStatus->Status);

    //
    // WakeIrpStatus will be used as the completion status for child wake IRPs
    // if we're not using the Express PME mechanisms directly.  So copy the
    // status from this IRP, unless it's STATUS_CANCELLED, in which case,
    // convert that to success, as that's a normal part of the wake protocol.
    //

    if (IoStatus->Status == STATUS_CANCELLED) {
        Bus->WakeIrpStatus = STATUS_SUCCESS;
    } else {
        Bus->WakeIrpStatus = IoStatus->Status;

        if (NT_SUCCESS(Bus->WakeIrpStatus)) {

            //
            // If this bus contains a hot-plug controller, kick the PnP manager
            // into asking for a rescan of the bus, as the wake may be in
            // response to a hotplug event.
            //

            if (PCI_BUS_IS_EXPRESS_HOTPLUG_CONTROLLER(Bus)) {
                IoInvalidateDeviceRelations(Bus->PhysicalDeviceObject, BusRelations);
            }
        }
    }

    //
    // The cancellation code will null the WakeIrp so that it can't possibly
    // be cancelled twice.  So it may already be null by the time we reach this
    // point.
    //

    KeAcquireSpinLock(&Bus->BusPowerLock, &OldIrql);
    if (Bus->WakeIrp != NULL) {

        if (NT_SUCCESS(IoStatus->Status)) {
            Bus->WokeSystem = PoGetSystemWake(Bus->WakeIrp);
        }

        Bus->WakeIrp = NULL;
    }
    KeReleaseSpinLock(&Bus->BusPowerLock, OldIrql);

    //
    // Now take a power reference which will be dropped when the bus comes
    // to D0.  Do it at DISPATCH_LEVEL so that this thread doesn't block.
    //

    if (NT_SUCCESS(IoStatus->Status)) {

        KeRaiseIrql(DISPATCH_LEVEL, &OldIrql);
        PciBus_PowerReference(Bus, PciBusPowerReasonBusWaitWakeComplete);
        PCI_BUS_SET_FLAG(Bus, WakePowerReferenced);
        KeLowerIrql(OldIrql);
    }

    //
    // If the wait/wake IRP succeeded, kick the worker thread into calling back
    // at PASSIVE_LEVEL on another stack.
    //

    if (NT_SUCCESS(IoStatus->Status) || (IoStatus->Status == STATUS_CANCELLED)) {

        ExInterlockedInsertTailList(&Bus->Root->PmeWaitWakeCompleteList,
                                    &Bus->WaitingForWaitWakeComplete,
                                    &Bus->Root->PmeListLock);

        KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventCompleteWaitWake],
                   IO_NO_INCREMENT,
                   FALSE);
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_SignalWorkerRetireD0Irps (
    _Inout_ PPCI_BUS    Bus
    )

/*++

Routine Description:

    This routine puts this bus on the list of ones that are waiting for D0 IRP
    retirement and kicks off the worker thread.
 
    This routine must be called with the Bus's BusPowerLock held.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value: 
 
    none

--*/

{
    NTSTATUS Status;
    KIRQL OldIrql;

    NON_PAGED_CODE();
    NT_ASSERT(Bus->BusPowerAvailable != 0);

    //
    // The action below is serialized implicitly.  This function is called
    // from the power state machine when resuming from S0 and it is also called
    // from the "active" callback from PoFx.  Since PoFx will move the bus to
    // "active" before leaving S0, these two can't overlap.  Multiple
    // activations can't overlap because this driver is carefully (we hope)
    // written so that no code ever calls PoFxIdleComponent (drops a reference
    // added with PoFxActivateComponent until the bus has passed through D0.
    //

    if (Bus->RetiringChildD0Irps == 0) {

        Status = IoAcquireRemoveLock(&Bus->RemoveLock, PCI_D0RETIRE_LOCK_TAG);
        if (NT_SUCCESS(Status)) {
    
            //
            // Take a power reference that will be dropped when all child D0
            // IRPs are retired, so that the bus can't turn off before the
            // worker thread runs that code.  Raise to DISPACH_LEVEL to be sure
            // that the function doesn't block.
            //

            KeRaiseIrql(DISPATCH_LEVEL, &OldIrql);
            PciBus_PowerReference(Bus, PciBusPowerReasonRetiringChildD0Irps);
            KeLowerIrql(OldIrql);

            PCI_BUS_SET_FLAG(Bus, RetiringChildD0Irps);
            
            ExInterlockedInsertTailList(&Bus->Root->PmeD0RetireList,
                                        &Bus->WaitingForChildD0Retirement,
                                        &Bus->Root->PmeListLock);
    
            KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventRetireD0],
                       IO_NO_INCREMENT,
                       FALSE);
        }
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
PciBus_PowerReference (
    _Inout_ PPCI_BUS                        Bus,
    _In_    PCI_BUS_POWER_REFERENCE_REASONS Reason
    )

/*++

Routine Description:

    This routine increments the power reference count on the bus.  If this
    reference changes from 0 to 1, then the bus will transition from Dx to D0.
 
    The semantics of this function differ slightly between PASSIVE_LEVEL and
    DISPATCH_LEVEL.  If invoked at PASSIVE_LEVEL, this function will block
    until the device is in D0.  If called at DISPATCH_LEVEL, it will never
    block.  If, however, the device is not in D0, it will return FALSE.
 
    If the caller gets back "FALSE" then it is the caller's responsibility to
    decide whether the current operation needs to be pended.  If so, the caller
    should return pending and queue some PASSIVE_LEVEL code.  That code should
    attempt to take another reference, which will block until the device is
    in D0.  The caller is responsible for removing the extra reference.
 
    Note that if the caller gets back false and does not queue something at
    PASSIVE_LEVEL, the device will lose a reference and it will be stuck in D0
    when it finally arrives there.

Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.
 
    Reason - This parameter specifies why the power reference is being taken.

Return Value: 
 
    TRUE if the device is in D0 by the time the function returns 

--*/

{

    BOOLEAN BusPowerAvailable;
    LONG Count;

    NON_PAGED_CODE();

    //
    // Cardbus is the Power Policy Owner, if present.
    //

    if (PCI_CARDBUS_BRIDGE(Bus)) {
        return TRUE;
    }

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "PciBus_PowerReference for reason %d",
                Reason);

    Count = InterlockedIncrement(&(Bus->BusPowerReferenceReason[Reason]));
    PCI_ASSERT(Count != 0);

    //
    // If this bus is in the process of being removed, do not call active or idle.
    //

    if (Bus->PepHandle == NULL) {
        TraceEvents(Bus->BlackBoxHandle,
            TRACE_LEVEL_INFORMATION,
            DBG_PPO,
            "PciBus_PowerReference on a NULL handle");

        return TRUE;
    }

    //
    // All reason codes contribute to the active/idle decision EXCEPT
    // PciBusPowerAllowF1DisableBridgeD3.  That reason code is only used
    // to adjust the device idle timeout.
    //
    
    if (Reason != PciBusPowerAllowF1DisableBridgeD3) {
        PoFxActivateComponent(Bus->PepHandle, 0, PO_FX_FLAG_ASYNC_ONLY);

    } else if (Count == 1) {

        //
        // Set the idle timeout to max when the PciBusPowerAllowF1DisableBridgeD3
        // transitions from 0->1.
        //

        PoFxSetDeviceIdleTimeout(Bus->PepHandle, (ULONGLONG)-1);
    }

    //
    // If the D-IRP is in flight (either because we're about to send it in-line
    // here or because another thread has sent it and it hasn't completed yet,
    // then let the caller know that the device isn't actually powered up yet.
    //

    BusPowerAvailable = (Bus->BusPowerAvailable == 1) ? TRUE : FALSE;

    if ((KeGetCurrentIrql() < DISPATCH_LEVEL) && (BusPowerAvailable == 0)) {
        KeWaitForSingleObject(&Bus->BusPowerEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);
        BusPowerAvailable = TRUE;
    }

    return BusPowerAvailable;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_PowerDereference (
    _Inout_ PPCI_BUS                        Bus,
    _In_    PCI_BUS_POWER_REFERENCE_REASONS Reason
    )

/*++

Routine Description:

    This routine decrements the power reference count on the bus.  If this
    reference changes from 1 to 0, then the bus will transition from D0 to Dx.

    This routine, though non-pageable, must be invoked at PASSIVE_LEVEL.

Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

    Reason - This parameter specifies why the power reference is being released.

Return Value: 
 
    none 

--*/

{

    LONG Count;

    NON_PAGED_CODE();

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "PciBus_PowerDereference for reason %d",
                Reason);

    //
    // Cardbus is the Power Policy Owner, if present.
    //

    if (PCI_CARDBUS_BRIDGE(Bus)) {
        return;
    }


    Count = InterlockedDecrement(&(Bus->BusPowerReferenceReason[Reason]));
    PCI_ASSERT(Count != -1);

    //
    // If this bus is in the process of being removed, do not call active or idle.
    //

    if (Bus->PepHandle == NULL) {
        TraceEvents(Bus->BlackBoxHandle,
            TRACE_LEVEL_INFORMATION,
            DBG_PPO,
            "PciBus_PowerDereference on a NULL handle");

        return;
    }

    //
    // All reason codes contribute to the active/idle decision EXCEPT
    // PciBusPowerAllowF1DisableBridgeD3.  That reason code is only used
    // to adjust the device idle timeout.
    //

    if (Reason != PciBusPowerAllowF1DisableBridgeD3) {
        PoFxIdleComponent(Bus->PepHandle, 0, PO_FX_FLAG_ASYNC_ONLY);

    } else if (Count == 0) {

        //
        // Set the idle timeout to 0 when the PciBusPowerAllowF1DisableBridgeD3
        // transitions from 1->0.
        //

        PoFxSetDeviceIdleTimeout(Bus->PepHandle, (ULONGLONG)0);
    }

    return;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_ArmForWake (
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine changes the wake state of the bus itself.
 
    If any child needs older-style non-native PME, then this will immediately
    result in IRP_MN_WAIT_WAKE sent down this stack, which will either cause the
    parent to arm for wake or it will be intercepted by an ACPI filter in this
    stack, which will arm a GPE pin for wake.
 
    If any child needs newer-style native PME, then nothing happens until the
    bus leaves D0, at which point the W/W IRP will be sent down the stack.
 
    Note that W/W is an asynchronous mechanism, so this function just kicks off
    a state machine that may take a while to complete.

Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none - It might seem like there should be some status here, but the actions
        taken are asynchronous, so the caller can't really know whether the
        process succeeded or failed.  If it fails, the bus will falsely trigger
        wake on the child by completing its W/W IRP.

--*/

{
    KIRQL OldIrql;
    PCI_BUS_WAKE_EVENTS Event = PciWakeEventMaximum;

    NON_PAGED_CODE();

    //
    // Cardbus is the Power Policy Owner, if present.
    //

    if (PCI_CARDBUS_BRIDGE(Bus)) {
        return;
    }

    KeAcquireSpinLock(&Bus->BusPowerLock, &OldIrql);

    Bus->ChildWakeCount++;

    if (Bus->ChildWakeCount ==  1) {
        Event = PciWakeEventFirstWakeRefAdded;
    }

    KeReleaseSpinLock(&Bus->BusPowerLock, OldIrql);

    if (Event != PciWakeEventMaximum) {
        PciBus_WakeStateM(Bus, Event);
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_DisarmForWake (
    _Inout_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine changes the wake state of the bus itself, reversing the effects
    of PciBus_ArmForWake.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    KIRQL OldIrql;
    PCI_BUS_WAKE_EVENTS Event = PciWakeEventMaximum;

    NON_PAGED_CODE();

    //
    // Cardbus is the Power Policy Owner, if present.
    //

    if (PCI_CARDBUS_BRIDGE(Bus)) {
        return;
    }

    KeAcquireSpinLock(&Bus->BusPowerLock, &OldIrql);

    Bus->ChildWakeCount--;

    if (Bus->ChildWakeCount == 0) {
        Event = PciWakeEventLastWakeRefRemoved;
    }

    KeReleaseSpinLock(&Bus->BusPowerLock, OldIrql);

    if (Event != PciWakeEventMaximum) {
        PciBus_WakeStateM(Bus, Event);
    }
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_SendWWPassive (
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine sends the wait/wake IRP.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    POWER_STATE PowerState;
    PIO_STACK_LOCATION IrpSp;

    NON_PAGED_CODE();
    PCI_ASSERT(!PCI_CARDBUS_BRIDGE(Bus));

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): sending wait wake",
                Bus->SecondaryBusNumber);

    Bus->WokeSystem = FALSE;

    //
    // Mark the system power state that we're shooting at within the wait/wake
    // IRP.  This will translate into parameters that the BIOS sees in its
    // _DSW object, if it supplies one.
    // 
    // This works differently for native Express and BIOS-controlled (either
    // native or conventional) wake.  In the conventional wake path, we arm at
    // the moment the child device arms.  So there will probably be no S-IRP
    // present.  In that case, report the lowest S-state that this bus
    // can wake from.  ACPI actually rejects arming the wake pin if the S-state
    // reported here is lighter than the one that we specify.
    //

    if (Bus->SIrp == NULL) {
        if (PCI_BUS_USES_NATIVE_PME(Bus)) {
            PowerState.SystemState = PowerSystemWorking;
        } else {
            PowerState.SystemState = Bus->SystemWake;
        }
    } else {
        IrpSp = IoGetCurrentIrpStackLocation(Bus->SIrp);
        PowerState.SystemState =
            IrpSp->Parameters.Power.State.SystemState;
    }

    //
    // "No competing thread" is okay here because the race for the wake
    // IRP can't occur before the wake IRP is received at the stack.
    //
    _No_competing_thread_begin_
    PoRequestPowerIrp(Bus->DeviceObject,
                      IRP_MN_WAIT_WAKE,
                      PowerState,
                      PciBus_WaitWakeCompletion,
                      Bus,
                      &Bus->WakeIrp);
    _No_competing_thread_end_

    PciBus_WakeStateM(Bus, PciWakeEventWorkerSentWaitWake);

}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_CancelWWPassive (
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine cancels the wait/wake IRP.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    KIRQL OldIrql;
    PIRP WakeIrp = NULL;

    NON_PAGED_CODE();
    PCI_ASSERT(!PCI_CARDBUS_BRIDGE(Bus));

    PciBus_WakeStateM(Bus, PciWakeEventWorkerCanceled);

    KeAcquireSpinLock(&Bus->BusPowerLock, &OldIrql);
    if (Bus->WakeIrp != NULL) {
        WakeIrp = Bus->WakeIrp;
        Bus->WakeIrp = NULL;
    }
    KeReleaseSpinLock(&Bus->BusPowerLock, OldIrql);

    //
    // If there was nothing to cancel, it was either because the wait/wake IRP
    // failed or because its completion got consumed in another state.  If that
    // happened, kick the state machine with a wait/wake completion event to
    // send toward the "disarmed" state.
    //

    if (WakeIrp != NULL) {
        IoCancelIrp(WakeIrp);
    } else {
        PciBus_WakeStateM(Bus, PciWakeEventWaitWakeComplete);
    }
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_SendD0Unlocked (
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine sends the D0 IRP that brings the bus back into a high
    power state.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    POWER_STATE PowerState;
    NTSTATUS Status;

    NON_PAGED_CODE();
    PCI_ASSERT(!PCI_CARDBUS_BRIDGE(Bus));

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): sending D0 IRP",
                Bus->SecondaryBusNumber);
     
    PowerState.DeviceState = PowerDeviceD0;
    Status = PoRequestPowerIrp(Bus->DeviceObject,
                               IRP_MN_SET_POWER,
                               PowerState,
                               &PciBus_PowerCompletion,
                               Bus,
                               NULL);

    if (!NT_SUCCESS(Status)) {
        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_PPO,
                    "(%x): PoRequestPowerIrp %!STATUS!",
                    Bus->SecondaryBusNumber,
                    Status);
    }
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_SendDx (
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine sends the Dx IRP that puts the bus into a low-power state.  The
    target D-state depends on whether the bus is armed for wake.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.
 
Return Value:

    none

--*/

{
    PIO_STACK_LOCATION IrpSp;
    POWER_STATE PowerState;

    NON_PAGED_CODE();

    //
    // If armed for wake, derive the target D-state from the device capabilites.
    // Otherwise, shoot for D3.
    //
    // If the machine is going to hibernate and this bus is part of the
    // hibernation path, override the previous choice with a D0 IRP, so that
    // the device remains powered on for the hibernation.  We send an IRP here
    // because the state machine requires IRP completion to move forward.
    //

    if (Bus->SIrp != NULL) {
        IrpSp = IoGetCurrentIrpStackLocation(Bus->SIrp);
        if ((IrpSp->Parameters.Power.State.SystemState == PowerSystemHibernate) &&
            (Bus->DeviceUsage.Hibernate != 0)) {
            PowerState.DeviceState = PowerDeviceD0;
        }

        if (Bus->ArmedForWake == 1) {

            if (Bus->DeviceWake == PowerDeviceUnspecified) {

                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_ERROR,
                            DBG_DEVPOWER,
                            "(%x): Arming bus for wake even though the BIOS "
                            "didn't supply wake objects.  Probably a faulty BIOS.",
                            Bus->SecondaryBusNumber);
                PowerState.DeviceState = PowerDeviceD3;

            } else {
                PowerState.DeviceState = Bus->DeviceWake;
            }

        } else {
            PowerState.DeviceState = PowerDeviceD3;
        }
    } else {

        if (Bus->ArmedForWake == 1) {

            //
            // If the bus is armed for wake, then we idle in the D-state that
            // the firmware says is "wakeable."  Thus we need to set D3cold 
            // support to the right value and the map the "wake depth" to the
            // target D-state.
            //

            if (Bus->D3coldIntrf.SetD3ColdSupport != NULL) {
                if (Bus->IdleDState == DeviceWakeDepthD3cold) {
                    Bus->D3coldIntrf.SetD3ColdSupport(Bus->D3coldIntrf.Context,
                                                      TRUE);
                } else {
                    Bus->D3coldIntrf.SetD3ColdSupport(Bus->D3coldIntrf.Context,
                                                      FALSE);
                }
            }

            PowerState.DeviceState = MapWakeDepthToDstate(Bus->IdleDState);

        } else {

            //
            // The bus will only idle when all the devices beneath it support
            // D3cold.  So if the bus isn't armed for wake, we always support
            // D3cold.
            //

            if (Bus->D3coldIntrf.SetD3ColdSupport != NULL) {
                Bus->D3coldIntrf.SetD3ColdSupport(Bus->D3coldIntrf.Context,
                                                  TRUE);
            }

            PowerState.DeviceState = PowerDeviceD3;
        }
    }

    PoRequestPowerIrp(Bus->DeviceObject,
                      IRP_MN_SET_POWER,
                      PowerState,
                      &PciBus_PowerCompletion,
                      Bus,
                      NULL);
}

NTSTATUS
PciBus_WaitWake(
    _Inout_ PIRP Irp,
    _In_ PIO_STACK_LOCATION IrpSp,
    _Inout_ PPCI_COMMON_EXTENSION DeviceExtension
    )
/*++

Routine Description:

    Handle IRP_MN_WAIT_WAKE for PCI FDOs.

Arguments:

    DeviceObject - Pointer to the device object for which this IRP applies.

    Irp - Pointer to the IRP.

Return Value:

    NT status.

--*/
{
    PPCI_BUS Bus;

    NON_PAGED_CODE();
    
    UNREFERENCED_PARAMETER(IrpSp);
    
    Bus = (PPCI_BUS)DeviceExtension;

    PCI_ASSERT(PCI_BUS_EXTENSION(Bus));

    //
    // PCMCIA.sys controls the wait/wake IRPs for
    // cardbus stacks.
    //
    if (!PCI_CARDBUS_BRIDGE(Bus)) {
        PCI_ASSERT(Bus->WakeIrp == Irp);
    }

    IoSkipCurrentIrpStackLocation(Irp);
    return PoCallDriver(Bus->AttachedDeviceObject, Irp);
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciBus_CompleteWWPassive (
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine is invoked on a worker thread at PASSIVE_LEVEL after the W/W
    IRP completes.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    BOOLEAN BusPowered;
    KIRQL OldIrql;

    NON_PAGED_CODE();
    PCI_ASSERT(!PCI_CARDBUS_BRIDGE(Bus));

    //
    // Conventional PCI buses (and those PCIe downstream ports running in 
    // compatibility mode) have no way to detect when they should start looking
    // for children that triggered PME, except by looking at the completion
    // of a wait/wake IRP.  So kick off that process here.
    //

    if (!PCI_BUS_USES_NATIVE_PME(Bus)) {

        //
        // First, find out if the bus is turned on.  Since this function runs
        // on the worker thread, it must not block, so raise to DISPATCH_LEVEL
        // to be sure that it won't.
        //

        KeRaiseIrql(DISPATCH_LEVEL, &OldIrql);
        BusPowered = PciBus_PowerReference(Bus, PciBusPowerReasonPmeTriggered);
        KeLowerIrql(OldIrql);

        //
        // If the bus isn't powered on now, that's fine.  The act of powering it
        // on will trigger the evaluation of PME across it and its children.
        //

        if (BusPowered != FALSE) {
            PciBus_ConventionalWakeTriggered(Bus);
            PciBus_PowerDereference(Bus, PciBusPowerReasonPmeTriggered);
        } else {
            PCI_BUS_SET_FLAG(Bus, PmePowerReferenced);
        }
    }

    PciBus_WakeStateM(Bus, PciWakeEventWaitWakeComplete);
}

VOID
PciBus_ProcessDeferredPmeEvents(
    _In_ PPCI_BUS Bus
    )

/*++

Routine Description:

    When a PME interrupt is delivered to a root port, the PME code tries to
    find the device responsible and make it shut up, after which it completes
    the wait/wake IRP for the device.  Lastly, it re-enables PME interrupts at
    the root port.
 
    If the bus on which the device resides happens to be powered off, this bus
    must be powered back on before the device can be made to shut up.
    Consequently the PME interrupt processing must be deferred until that bus
    can be powered back up.
 
    This routine identifies this situation and restarts PME processing if it
    had stalled waiting for this bus to be powered up.
 
    This should only be possible for PCI Express buses that are not root ports,
    since root ports can't handle interrupts when they are not in D0. So it
    should be okay to take the root port bus power lock here.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{

    BOOLEAN EnqueuePmeEvent = FALSE;
    PEXPRESS_BRIDGE ExpressBridge;
    PPCI_BRIDGE PciBridge;
    KIRQL OldIrql;
    NTSTATUS Status;

    NON_PAGED_CODE();
    PCI_ASSERT(Bus->BusPowerAvailable == 1);

    if (Bus->PmeEventDeferred == 1) {

        PCI_ASSERT(!PCI_ROOT_BUS(Bus));

        //
        // Find the root port.  You'd think we could just find a pointer to the
        // root port express bridge here, but we can't do that directly because
        // this bridge may not be Express.
        //

        PciBridge = Bus->PciBridge;
        while (!PCI_ROOT_BUS(PciBridge->Bus->ParentBus)) {
            PciBridge = PciBridge->Bus->ParentBus->PciBridge;

            if ((PciBridge->ExpressBridge != NULL) &&
                (PciBridge->ExpressBridge->BridgeType == PciExpressRootPort)) {
                break;
            }
        }

        ExpressBridge = PciBridge->ExpressBridge;
        PCI_ASSERT(ExpressBridge != NULL);
        PCI_ASSERT(ExpressBridge->BridgeType == PciExpressRootPort);

        KeAcquireSpinLock(&Bus->Root->Bus->BusPowerLock, &OldIrql);
        if (!ExpressBridge->Downstream.u.RootPort.OnPmeList) {
            ExpressBridge->Downstream.u.RootPort.PmeEventType = ExpressPmeRuntimeEvent;
            ExpressBridge->Downstream.u.RootPort.OnPmeList = TRUE;
            EnqueuePmeEvent = TRUE;
        }
        KeReleaseSpinLock(&Bus->Root->Bus->BusPowerLock, OldIrql);

        if (EnqueuePmeEvent) {

            Status = IoAcquireRemoveLock(&ExpressBridge->PciBridge->Bus->RemoveLock, 
                                         PCI_PME_REMOVE_LOCK_TAG);

            if (NT_SUCCESS(Status)) {
                ExInterlockedInsertTailList(&Bus->Root->PmeRootPortList,
                                            &ExpressBridge->Downstream.u.RootPort.PmeListEntry,
                                            &Bus->Root->PmeListLock);
    
                KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventRootPortAttention],
                           IO_NO_INCREMENT, 
                           FALSE);
            }
        }
    }

    return;
}

LARGE_INTEGER
PciBus_WaitTimeForBusSettle (
    _In_ PPCI_BUS Bus,
    _In_ ULONG DeviceSpecificDelay
    )

/*++

Routine Description:

    This function calculates the time necessary between now and when devices
    on this bus can be expected to respond to config access, using a previously
    taken timestamp when the bus itself was turned on.

Arguments:

    Bus - Pointer to the struct describing this bus.
 
    DeviceSpecificDelay - The amount of time we should wait for a particular
        device in milliseconds.

Return Value:

    0 if the devices on the bus have already had time to become coherent.
    non-zero values are in interrupt time (100ns units).

--*/

{
    ULONG Delay;
    LARGE_INTEGER QpcFreq;
    NTSTATUS Status;
    LARGE_INTEGER TimeToWait;

    TimeToWait.QuadPart = 0;

    if (Bus->SettleTimeRequired == 0) {
        return TimeToWait;
    }

    if (Bus->TimeOfPowerUp.QuadPart != 0) {

        //
        // Find time since power up.
        //

        TimeToWait = KeQueryPerformanceCounter(&QpcFreq);
        TimeToWait.QuadPart -= Bus->TimeOfPowerUp.QuadPart;

        //
        // Put it in terms of 100ns units (interrupt time.)
        //

        Status = RtlULongLongMult(TimeToWait.QuadPart, 
                                  10000000, 
                                  &TimeToWait.QuadPart);

        //
        // Overflow means that we've already waited long enough.
        //

        if (!NT_SUCCESS(Status) || (TimeToWait.QuadPart < 0)) {
            TimeToWait.QuadPart = 0;
        } else {

            TimeToWait.QuadPart /= QpcFreq.QuadPart;

            //
            // Convert milliseconds to 100ns units.
            //

            Status = RtlULongLongToULong((ULONGLONG)DeviceSpecificDelay * 10000, 
                                         &Delay);
            if (!NT_SUCCESS(Status)) {

                //
                // Incomprehensibly large delay must be stored in the hack
                // table or the registry.  Cap it at something reasonable.
                //

                Delay = DEVICE_LINK_ON_SETTLE_TIME * 10000;
            }

            //
            // Cap it at 100ms, as the PCI and PCIe specs say, or whatever
            // we got from the errata manager.
            //

            if ((ULONGLONG)TimeToWait.QuadPart > Delay) {
                TimeToWait.QuadPart = 0;
            } else {
                TimeToWait.QuadPart = Delay - TimeToWait.QuadPart;
            }
        }
    }

    return TimeToWait;
}

// ----------------------------------------------------------- IRP_MN_ Functions
NTSTATUS
PciBus_QueryPower (
    _Inout_ PIRP Irp,
    _In_ PIO_STACK_LOCATION IrpSp,
    _In_ PPCI_COMMON_EXTENSION DeviceExtension
    )

/*++

Routine Description:

    This is a dispatch routine for Query power Irps. 
    This routine services both the System and Device Query power Irps.

Arguments:

    Irp - Supplies the System/Device Query power Irp.

    IrpSp - Supplies the current stack location for the power Irp.

    DeviceExtension - Supplies the bus extension for the root bus/bridge.

Return Value:

    STATUS_PENDING if the power Irp is a System power Irp sent to 
    non-cardbus bridges.

    Otherwise the status code returned from IoCallDriver.

--*/

{

    PPCI_BUS Bus;

    NON_PAGED_CODE();

    Bus = (PPCI_BUS)DeviceExtension;

    PCI_ASSERT(PCI_BUS_EXTENSION(Bus));

    //
    // If we aren't started, don't touch the Irp, simply pass it along.
    //

    if (DeviceExtension->DeviceState != PciStarted) {
        goto Exit;
    }

    switch (IrpSp->Parameters.Power.Type) {
    case SystemPowerState:

        //
        // PCMCIA is the power policy owner for the cardbus stack.
        // So for cardbus bridges, just pass the Irp along.
        //

        if (PCI_CARDBUS_BRIDGE(Bus)) {
            Irp->IoStatus.Status = STATUS_SUCCESS;
        } else {
            if ((Bus->WakeIrp != NULL) &&
                (IrpSp->Parameters.Power.State.SystemState < PowerSystemShutdown) &&
                (Bus->SystemWake < IrpSp->Parameters.Power.State.SystemState)) {
                PciCompleteRequest(Irp, STATUS_UNSUCCESSFUL);
                return STATUS_UNSUCCESSFUL;
            } else {
                Irp->IoStatus.Status = STATUS_SUCCESS;
            }
        }
        break;

    case DevicePowerState:
        Irp->IoStatus.Status = STATUS_SUCCESS;
        break;

    default:

        //
        // If the Irp type is not known, simply pass it along.
        //

        break;
    }

Exit:

    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(Bus->AttachedDeviceObject, Irp);
}

NTSTATUS
PciBus_SetPower (
    _Inout_ PIRP Irp,
    _In_ PIO_STACK_LOCATION IrpSp,
    _Inout_ PPCI_COMMON_EXTENSION DeviceExtension
    )

/*++

Routine Description:

    This is a dispatch routine for Set power Irps. 
    This routine services both the System and Device power Irps.

Arguments:

    Irp - Supplies the Set System/Device power Irp.

    IrpSp - Supplies the current stack location for the power Irp.

    DeviceExtension - Supplies the bus extension for the root bus/bridge.

Return Value:

    STATUS_PENDING if the power Irp is a System power Irp sent to 
    non-cardbus bridges.

    Error status code obtained when the bridge failed to power down.

    Otherwise the status code returned from IoCallDriver.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE ChildDevice;
    BOOLEAN ProcessSx;
    NTSTATUS Status;

    NON_PAGED_CODE();

    PCI_ASSERT(PCI_BUS_EXTENSION(DeviceExtension));

    Bus = (PPCI_BUS)DeviceExtension;
    ChildDevice = Bus->ChildDevices;
    ProcessSx = TRUE;
    switch (IrpSp->Parameters.Power.Type) {
    case SystemPowerState:

        //
        // PCMCIA is the power policy owner for the cardbus stack.
        // So for cardbus bridges, mark the Irp with success and pass it along.
        //

        if (PCI_CARDBUS_BRIDGE(Bus)) {
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;
        }

        //
        // The system may send a system power IRP to this stack before the
        // device (bus) has been started, particularly in the case that the
        // device didn't get the resources it needed.  This will result in the
        // PnP tree lock being dropped before a start arrives (because it may
        // never) and then S-state transitions can occur.
        //

        if (DeviceExtension->DeviceState != PciStarted) {
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;
        }

#if defined(_ARM_) || defined(_ARM64_)

        //
        // On ARM platforms, only power down bridges on S4/5 transitions 
        // if the child devices are in D3.
        //

        if ((IrpSp->Parameters.Power.State.SystemState == PowerSystemHibernate) ||
            (IrpSp->Parameters.Power.State.SystemState == PowerSystemShutdown)) {

            while (ChildDevice != NULL) {
                if (ChildDevice->LogicalPowerState != PowerDeviceD3) {
                    ProcessSx = FALSE;
                    break;
                }

                ChildDevice = ChildDevice->Sibling;
            }

            if (ProcessSx == FALSE) {
                Irp->IoStatus.Status = STATUS_SUCCESS;
                break;
            }
        }

#endif

        //
        // The system may send an S0 IRP to cancel a query-power IRP that was
        // sent earlier.  If this happens, just succeed and pass the IRP along.
        //

        if ((Bus->ExpectingS0Irp == 0) &&
            (IrpSp->Parameters.Power.State.SystemState == PowerSystemWorking)) {
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;
        }

        //
        // If this bus has an integrated hot-plug controller and the system is
        // leaving S0, drop the wake reference so that we don't wake the bus
        // on device insertion.
        //

        if ((IrpSp->Parameters.Power.State.SystemState != PowerSystemWorking) &&
            (PCI_BUS_IS_EXPRESS_HOTPLUG_CONTROLLER(Bus))) {
            PciBus_DisarmForWake(Bus);
        }

        return PciBusHandleSystemPower(Bus, Irp);

    case DevicePowerState:

        //
        // If this is a cardbus bridge then PCMCIA is the power policy owner 
        // for the cardbus stack. Hands off.
        //

        if (PCI_CARDBUS_BRIDGE(Bus)) {
            if (IrpSp->Parameters.Power.State.DeviceState > Bus->PowerState) {
                PCI_BUS_CLEAR_FLAG(Bus, BusPowerAvailable);
                KeClearEvent(&Bus->BusPowerEvent);
            }
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;
        }

        //
        // If there is no change in the device power state, then simply pass 
        // the Irp along.
        //

        if (Bus->PowerState == IrpSp->Parameters.Power.State.DeviceState) {
            Irp->IoStatus.Status = STATUS_SUCCESS;
            break;
        }

        //
        // Check if the PCI bus is powering up or down and take an appropriate
        // action.
        //

        if (Bus->PowerState > IrpSp->Parameters.Power.State.DeviceState) {

            //
            // The PCI bus is powering up. The hardware state will be restored
            // in a completion routine - when the Irp is on its way back up.
            //

            IoCopyCurrentIrpStackLocationToNext(Irp);
            IoSetCompletionRoutine(Irp, 
                                   PciBusPowerUpCompletion, 
                                   Bus, 
                                   TRUE, 
                                   TRUE, 
                                   TRUE);

            return IoCallDriver(Bus->AttachedDeviceObject, Irp);
        }

        PCI_ASSERT(Bus->PowerState < IrpSp->Parameters.Power.State.DeviceState);

        Status = PciBusPowerDown(Bus, IrpSp);

        //
        // If the bus failed to power down, then complete the Irp with the 
        // same failure code.
        //

        if (!NT_SUCCESS(Status)) {
            Irp->IoStatus.Status = Status;
            PciCompleteRequest(Irp, Status);
            return Status;
        }

        //
        // On success, pass the low power Irp down the stack.
        //

        Irp->IoStatus.Status = Status;
        break;

    default:

        //
        // If the Irp is of an unknown type, simply pass it along.
        //

        break;
    }

    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(Bus->AttachedDeviceObject, Irp);
}


NTSTATUS
PciBusHandleSystemPower (
    _In_ PPCI_BUS Bus,
    _Inout_ PIRP Irp
    )

/*++

Routine Description:

    This routine handles the System power Irp for PCI bus.

Arguments:

    Bus - Supplies the PCI bus extension.

    Irp - Supplies a Query/Set System power Irp.

Return Value:

    STATUS_PENDING.

--*/

{

    NON_PAGED_CODE();

    IoMarkIrpPending(Irp);
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp, 
                           PciBusSystemPowerCompletion, 
                           Bus, 
                           TRUE, 
                           TRUE, 
                           TRUE);

    IoCallDriver(Bus->AttachedDeviceObject, Irp);
    return STATUS_PENDING;
}

NTSTATUS
PciBusPowerDown (
    _Inout_ PPCI_BUS Bus,
    _In_ PIO_STACK_LOCATION IrpSp
    )

/*++

Routine Description:

    This routine is invoked to power down the PCI bus.

Arguments:

    Bus - Supplies the PCI bus extension.

    IrpSp - Supplies the current stack location for the power Irp.

Return Value:

    STATUS_SUCCESS for a PCI root bus. Also when the bus is moving into a
    deeper sleep state.

    In all other cases, the status code returned from a function call in
    this routine.

--*/

{

    DEVICE_POWER_STATE OldPowerState;

    NON_PAGED_CODE();

    OldPowerState = Bus->PowerState;

    //
    // Update the power state in the bus extension.
    //

    Bus->PowerState = IrpSp->Parameters.Power.State.DeviceState;

    //
    // PCI root buses do not save any hardware state when powering down.
    // So there is no action to take.
    //

    if (PCI_ROOT_BUS(Bus)) {
        return STATUS_SUCCESS;
    }

    //
    // If the PCI bridge is already powered down and is transitioning 
    // to a deeper sleep state, then pass the Irp along.
    //

    if (OldPowerState != PowerDeviceD0) {
        return STATUS_SUCCESS;
    }

    //
    // Disable express bridge interrupts before the rest of the bridge goes
    // down.
    //

    if ((Bus->PciBridge != NULL) &&
        (Bus->PciBridge->ExpressBridge != NULL)) {
        ExpressBridgeDisableInterrupts(Bus->PciBridge->ExpressBridge);
    }

    //
    // Power down the bridge. This needs to happen now - when the Irp is
    // on its way down.
    //

    return PciBridgePowerDown(Bus->PciBridge);
}

// ------------------------------------------------------ IRP Callback Functions

__drv_functionClass(IO_COMPLETION_ROUTINE)
_IRQL_requires_same_
NTSTATUS
PciBusSystemPowerCompletion (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp,
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )

/*++

Routine Description:

    This is a completion routine for System power Irps.
    This completion routine is set for both the Set/Query System power Irps.
    The completion routine queues a corresponding Device power Irp.
    The device power state is obtained from S->D state mappings preserved 
    in the bus extension.

Arguments:

    DeviceObject - Supplies a device object.

    Irp - Supplies the System power Irp.

    Context - Supplies the Bus extension.

Return Value:

    STATUS_SUCCESS on failure to queue a Device power Irp. This will allow
    the System Irp to complete.

--*/

{

    PPCI_BUS Bus;
    PIO_STACK_LOCATION IrpStack;
    SYSTEM_POWER_STATE SystemState;

    NON_PAGED_CODE();
    UNREFERENCED_PARAMETER(DeviceObject);

    Bus = (PPCI_BUS)Context;
    IrpStack = IoGetCurrentIrpStackLocation(Irp);
    SystemState = IrpStack->Parameters.Power.State.SystemState;

    PCI_ASSERT((SystemState > PowerSystemUnspecified) && 
               (SystemState < PowerSystemMaximum));
    _Analysis_assume_(Bus != NULL);

    IoMarkIrpPending(Irp);
    Bus->SIrp = Irp;

    //
    // Cardbus is the Power Policy Owner, if present.
    //

    if (!PCI_CARDBUS_BRIDGE(Bus)) {

        //
        // Waking the entire system and putting it to sleep are handled very
        // differently.
        //

        if (SystemState == PowerSystemWorking) {

            PciBus_PowerStateM(Bus, PciBusEventEnteringS0);

            //
            // If the machine is coming back from hibernation, bring this bus back
            // to D0 so that it can be rescanned.  Raise to DISPATCH_LEVEL for this
            // so that the IRP is dispatched asynchronously and the S0 IRP can 
            // complete.
            //

            if (Bus->NeedToScanAfterHibernate == 1) {
                PCI_BUS_CLEAR_FLAG(Bus, NeedToScanAfterHibernate);
                IoInvalidateDeviceRelations(Bus->PhysicalDeviceObject, BusRelations);
            }

            //
            // If this bus has an integrated hot-plug controller and the system
            // is entering S0, add a wake reference so that a hot-plug bus
            // that's idling will get woken when a device is inserted.
            //

            if (PCI_BUS_IS_EXPRESS_HOTPLUG_CONTROLLER(Bus)) {
                PciBus_ArmForWake(Bus);
            }

        } else {

            PciBus_PowerStateM(Bus, PciBusEventLeavingS0);

            //
            // If the system is hibernating, flag this bus as needing a rescan when
            // the system comes out of hibernation.
            //

            if (SystemState == PowerSystemHibernate) {
                PCI_BUS_SET_FLAG(Bus, NeedToScanAfterHibernate);
            }
        }
    }

    //
    // This will halt the completion of a System power Irp and will be resumed
    // after the Device power Irp is completed, wake signals are processed, etc.
    //

    return STATUS_MORE_PROCESSING_REQUIRED;
}

__drv_functionClass(IO_COMPLETION_ROUTINE)
_IRQL_requires_same_
NTSTATUS
PciBusPowerUpCompletion (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp,
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )

/*++

Routine Description:

    This is a completion routine for a Set Device power Irp and is set
    when power is applied to the device.

Arguments:

    DeviceObject - Supplies the device object.

    Irp - Supplies the Device power Irp.

    Context - Supplies the bus extension.

Return Value:

    STATUS_SUCCESS for root buses.

    STATUS_SUCCESS when there is no change in the hardware state.

    Otherwise an appropriate status as returned from PciBridgePowerUp.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    PIO_STACK_LOCATION IrpStack;
    NTSTATUS Status;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(DeviceObject);

    Bus = (PPCI_BUS)Context;
    _Analysis_assume_(Bus != NULL);
    Bus->TimeOfPowerUp = KeQueryPerformanceCounter(NULL);
    IrpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // If the Irp is marked pending then update the pending flag 
    // in the stack location.
    //

    if (Irp->PendingReturned != FALSE) {
        IoMarkIrpPending(Irp);
    }

    //
    // Update the bus power state.
    //

    Bus->PowerState = IrpStack->Parameters.Power.State.DeviceState;

    //
    // There is no hardware state to restore for PCI root bus, though there
    // may be certain system devices behind the root bus that do not have any
    // drivers loaded for them and therefore do not get D-Irps. The hardware
    // state for such devices is restored here.
    //

    if (PCI_ROOT_BUS(Bus)) {
        for (Device = Bus->ChildDevices;
             Device != NULL;
             Device = Device->Sibling) {

            //
            // Certain devices on PCI root bus do not receive D-Irps but 
            // require re-programmong of hyper-transport based MSI mapping
            // capability so that other PCI devices can use MSI interrupts.
            //

            if (Bus->PowerState == PowerDeviceD0) {
                if ((Device->HtMsiMapCapability != 0) &&
                    ((Device->HackFlags & PCI_HACK_PROGRAM_HT_MSI_MAP) != 0)) {

                    PciProgramHtMsiMapCapability(Device);
                }
            }
        }

        return STATUS_SUCCESS;
    }

    //
    // If the PCI bridge is transitioning to a lighter sleep state but is not 
    // fully powered on, then do not restore any hardware state and complete 
    // the Irp with success.
    //

    if (Bus->PowerState != PowerDeviceD0) {
        return STATUS_SUCCESS;
    }

    //
    // Power up the bridge.
    //

    Status = PciBridgePowerUp(Bus->PciBridge);

    //
    // If this is a cardbus bridge, the power policy ownership is in the 
    // cardbus driver, which means that the state machine won't handle the
    // power up process.  Just do it in line here.
    //

    if (PCI_CARDBUS_BRIDGE(Bus)) {

        //
        // Enable express bridge interrupts.
        //

        if ((Bus->PciBridge != NULL) &&
            (Bus->PciBridge->ExpressBridge != NULL)) {
            ExpressDownstreamSwitchPortEnableInterrupts(Bus->PciBridge->ExpressBridge);
        }

        PCI_BUS_SET_FLAG(Bus, BusPowerAvailable);
        KeSetEvent(&Bus->BusPowerEvent, IO_NO_INCREMENT, FALSE);
    }

    return Status;
}

// ------------------------------------------- Power Policy Owner State Machines

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_LeavingS0DxDisarming (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    ExInterlockedInsertTailList(&Bus->Root->PmeD0SendList,
                                &Bus->WaitingForD0Send,
                                &Bus->Root->PmeListLock);
    KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventSendD0],
               IO_NO_INCREMENT,
               FALSE);

    return PciBusPowerLeavingS0DxDisarmed;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_S0D0 (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    //
    // Enable express bridge interrupts.
    //

    if ((Bus->PciBridge != NULL) &&
        (Bus->PciBridge->ExpressBridge != NULL)) {
        ExpressDownstreamSwitchPortEnableInterrupts(Bus->PciBridge->ExpressBridge);
    }

    //
    // Complete the system power IRP.
    //

    PCI_ASSERT(Bus->SIrp);
    PCI_BUS_CLEAR_FLAG(Bus, ExpectingS0Irp);
    PciCompleteRequest(Bus->SIrp, STATUS_SUCCESS);
    Bus->SIrp = NULL;

    PCI_BUS_SET_FLAG(Bus, BusPowerAvailable);

    //
    // Restart any PME event handling that was deferred because this bus was
    // powered down.
    //

    PciBus_ProcessDeferredPmeEvents(Bus);

    //
    // If anybody is waiting for their power reference, let them go.  This is
    // done here because PoFx doesn't driver S-state transitions and so it won't
    // be done in ComponentActiveConditionCallback.
    //

    PCI_BUS_SET_FLAG(Bus, BusPowerAvailable);
    KeSetEvent(&Bus->BusPowerEvent, IO_NO_INCREMENT, FALSE);

    //
    // Promise PoFx that this device is turned on for good, or at least until
    // PoFx says otherwise.
    //

    PoFxReportDevicePoweredOn(Bus->PepHandle);

    //
    // Signal that any D0 IRPs waiting at the child PDOs should be
    // processed.
    //
    
    PciBus_SignalWorkerRetireD0Irps(Bus);

    return PciBusPowerS0On;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_S0Disarmed (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    ExInterlockedInsertTailList(&Bus->Root->PmeD0SendList,
                                &Bus->WaitingForD0Send,
                                &Bus->Root->PmeListLock);
    KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventSendD0],
               IO_NO_INCREMENT,
               FALSE);

    return PciBusPowerMaximum;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_S0Dozing (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    PciBus_GenericWakePower(Bus, FALSE);

    return PciBusPowerMaximum;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_S0DozingArmed (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    PciBus_SendDx(Bus);

    //
    // Notify PoFx that DevicePowerNotRequiredCallback is completed.
    //

    PoFxCompleteDevicePowerNotRequired(Bus->PepHandle);
    return PciBusPowerMaximum;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_S0Off (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    PciBus_GenericWakePower(Bus, TRUE);

    return PciBusPowerMaximum;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_S0Stirring (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    PciBus_GenericWakePower(Bus, TRUE);

    return PciBusPowerMaximum;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_StirringD0 (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    //
    // Enable express bridge interrupts.
    //

    if ((Bus->PciBridge != NULL) &&
        (Bus->PciBridge->ExpressBridge != NULL)) {
        ExpressDownstreamSwitchPortEnableInterrupts(Bus->PciBridge->ExpressBridge);
    }

    PCI_BUS_SET_FLAG(Bus, BusPowerAvailable);

    //
    // Restart any PME event handling that was deferred because this bus was
    // powered down.
    //

    PciBus_ProcessDeferredPmeEvents(Bus);

    //
    // Promise PoFx that this device will remain in D0 until PoFx says
    // otherwise.
    //

    PoFxReportDevicePoweredOn(Bus->PepHandle);

    return PciBusPowerS0On;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_StirringDisarmed (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    ExInterlockedInsertTailList(&Bus->Root->PmeD0SendList,
                                &Bus->WaitingForD0Send,
                                &Bus->Root->PmeListLock);
    KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventSendD0],
               IO_NO_INCREMENT,
               FALSE);

    return PciBusPowerMaximum;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_SxArmed (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    PCI_BUS_CLEAR_FLAG(Bus, BusPowerAvailable);
    PciBus_SendDx(Bus);

    return PciBusPowerMaximum;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_Sx (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    PCI_ASSERT(Bus->SIrp != NULL);
    PCI_BUS_SET_FLAG(Bus, ExpectingS0Irp);
    PciCompleteRequest(Bus->SIrp, STATUS_SUCCESS);
    Bus->SIrp = NULL;

    return PciBusPowerMaximum;
}

__drv_functionClass(PCI_POWER_POLICY_STATE_FUNC)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_requires_same_
PCI_BUS_POWER_STATES
PciBus_SxOn (
    _In_ PPCI_BUS Bus
    )


/*++

Routine Description:

    This routine implements the things that should happen on entry to the
    state.  See PCI Power.vdx in this directory for a visual description of the
    state machine.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.

Return Value:

    none

--*/

{
    NON_PAGED_CODE();

    //
    // Clear the BusPowerAvailable flag, as PoFx doesn't drive S-state
    // transitions, so it won't get cleared in ComponentIdleConditionCallback.
    //

    PCI_BUS_CLEAR_FLAG(Bus, BusPowerAvailable);

    //
    // Send an event to the wake state machine saying "Wake Power Down."
    //

    PciBus_GenericWakePower(Bus, FALSE);

    return PciBusPowerMaximum;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_PowerStateM (
    _Inout_     PPCI_BUS Bus,
    _In_        PCI_BUS_POWER_EVENTS Event
    )

/*++

Routine Description:

    This routine implements the processing of the state machine itself
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.
 
    Event - The event that is being injected into the state machine.  This is
        described in PCI Power.vdx as the label on the line from one state
        to another.
 
Return Value:

    none

--*/

{
    
    PPCI_POWER_POLICY_STATE_FUNC Func = NULL;
    PCI_BUS_POWER_STATES NextState;
    KIRQL OldIrql;

    NON_PAGED_CODE();

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): => %!BUS_POWER_EVENT!",
                Bus->SecondaryBusNumber,
                Event); 
    PCI_ASSERT(Event >= 0 && Event < PciBusEventMaximum); 

    //
    // This state machine runs with a couple of rules:
    //
    // A) The state function is optional.  If it doesn't exist, the
    //    state machine exits, to be re-entered when another event is presented.
    //
    // B) The state function returns the next state.  If it returns
    //    PciBusPowerMaximum, then the state machine exits.
    //
    // C) The state table encodes whether the state function should be invoked
    //    with the state machine lock held.  If not, the state machine will
    //    exit after invoking the state function, regardless of return code.
    //

    KeAcquireSpinLock(&Bus->BusPowerLock, &OldIrql);

    NextState = PciBus_State[Bus->BusPowerState].NextState[Event];

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): From %!BUS_POWER_STATE! to %!BUS_POWER_STATE!",
                Bus->SecondaryBusNumber,
                Bus->BusPowerState,
                NextState); 

    if (NextState != PciBusPowerMaximum) {
        Bus->BusPowerState = NextState;
    } else {
        
        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_PPO,
                    "(%x): UNEXPECTED EVENT FOR STATE %!BUS_POWER_STATE!",
                    Bus->SecondaryBusNumber,
                    Bus->BusPowerState);
        PCI_ASSERTMSG("Update state machine", FALSE);
        goto Exit;
    }

    do {
        Func = PciBus_State[Bus->BusPowerState].Func;

        if (PciBus_State[Bus->BusPowerState].Flags & PciBusPowerPolicyFlagLocked) {

            PCI_ASSERT(Func != NULL);

            NextState = Func(Bus);
            Func = NULL;

            if (NextState != PciBusPowerMaximum) {
                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_INFORMATION,
                            DBG_PPO,
                            "(%x): From %!BUS_POWER_STATE! to %!BUS_POWER_STATE!",
                            Bus->SecondaryBusNumber,
                            Bus->BusPowerState,
                            NextState); 

                Bus->BusPowerState = NextState;
            } else {
                goto Exit;
            }
        } else {
            goto Exit;
        }
    } while (TRUE);

Exit:

    KeReleaseSpinLock(&Bus->BusPowerLock, OldIrql);

    if (Func != NULL) {

        NextState = Func(Bus);
        PCI_ASSERT(NextState == PciBusPowerMaximum);
        UNREFERENCED_PARAMETER(NextState);
    }

    return;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PciBus_WakeStateM (
    _Inout_     PPCI_BUS Bus,
    _In_        PCI_BUS_WAKE_EVENTS Event
    )

/*++

Routine Description:

    This routine implements the processing of the wake state machine itself.
 
Arguments:

    Bus - Supplies the bus extension for the root bus/bridge.
 
    Event - The event that is being injected into the state machine.  This is
        described in PCI Power.vdx as the label on the line from one state
        to another.
 
Return Value:

    none

--*/

{
    
    PCI_BUS_POWER_POLICY_STATE_FLAGS ActionFlags;
    BOOLEAN Express;
    ULONG MaxState;
    ULONG NextState;
    KIRQL OldIrql;
    PCPCI_BUS_WAKE_STATE States;

    NON_PAGED_CODE();

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_PPO,
                "(%x): => %!BUS_WAKE_EVENT!",
                Bus->SecondaryBusNumber,
                Event);
    PCI_ASSERT(Event >= 0 && Event < PciWakeEventMaximum); 

    //
    // Use the PCI Express wake state machine for any bus in Express mode except
    // a root bus.  (I.e. root ports and below.)  This is because a root PCI bus
    // has no root port above it for receiving PME interrupts.
    //

    if (PCI_BUS_USES_NATIVE_PME(Bus)) {
        Express = TRUE;
        MaxState = PciExpressWakeMaximum;
        States = PciExpressWakeStates;
    } else {
        Express = FALSE;
        MaxState = PciConvWakeMaximum;
        States = PciConventionalWakeStates;

        //
        // The conventional wake state machine is special in that it doesn't
        // need to pay attention to bus power on events.  So the function that
        // invokes the state machine with that event is wired to respond back
        // immediately with "wake ready" in that case.
        //

        PCI_ASSERT(Event != PciWakeEventPowerUp);
    }

    KeAcquireSpinLock(&Bus->BusWakeLock, &OldIrql);

    NextState = States[Bus->BusWakeState].NextState[Event];

    if (Express != FALSE) {
        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_PPO,
                    "(%x): From %!BUS_EXPRESS_WAKE_STATE! to %!BUS_EXPRESS_WAKE_STATE!",
                    Bus->SecondaryBusNumber,
                    Bus->BusWakeState,
                    NextState); 
    } else {
        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_PPO,
                    "(%x): From %!BUS_CONVENTIONAL_WAKE_STATE! to %!BUS_CONVENTIONAL_WAKE_STATE!",
                    Bus->SecondaryBusNumber,
                    Bus->BusWakeState,
                    NextState); 
    }

    if (NextState != MaxState) {
        Bus->BusWakeState = NextState;
    } else {
        
        if (Express != FALSE) {
            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_INFORMATION,
                        DBG_PPO,
                        "(%x): UNEXPECTED EVENT FOR STATE %!BUS_EXPRESS_WAKE_STATE!",
                        Bus->SecondaryBusNumber,
                        Bus->BusWakeState);
        } else {
            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_INFORMATION,
                        DBG_PPO,
                        "(%x): UNEXPECTED EVENT FOR STATE %!BUS_CONVENTIONAL_WAKE_STATE!",
                        Bus->SecondaryBusNumber,
                        Bus->BusWakeState);
        }
        PCI_ASSERTMSG("Update state machine", FALSE);
        goto Exit;
    }

    do {
        ActionFlags = States[Bus->BusWakeState].ActionFlags;
        NextState = States[Bus->BusWakeState].FallThroughState;

        //
        // Take whatever action is implied by the action flags.
        //
        while (ActionFlags != 0) {

            //
            // Isolate lowest bit and switch
            //
            switch (((ActionFlags - 1) & ActionFlags) ^ ActionFlags) {
            case PciBusPowerPolicyFlagMarkArmed:
                PCI_BUS_SET_FLAG(Bus, ArmedForWake);
                ActionFlags &= ~PciBusPowerPolicyFlagMarkArmed;
                break;

            case PciBusPowerPolicyFlagMarkDisarmed:
                PCI_BUS_CLEAR_FLAG(Bus, ArmedForWake);
                ActionFlags &= ~PciBusPowerPolicyFlagMarkDisarmed;
                break;

            case PciBusPowerPolicyFlagSendWW:
                ExInterlockedInsertTailList(&Bus->Root->PmeWaitWakeSendList,
                                            &Bus->WaitingForWaitWakeSend,
                                            &Bus->Root->PmeListLock);
                KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventSendWaitWake],
                           IO_NO_INCREMENT,
                           FALSE);
                ActionFlags &= ~PciBusPowerPolicyFlagSendWW;
                break;

            case PciBusPowerPolicyFlagCancelWW:
                ExInterlockedInsertTailList(&Bus->Root->PmeWaitWakeCancelList,
                                            &Bus->WaitingForWaitWakeCancel,
                                            &Bus->Root->PmeListLock);
                KeSetEvent(&Bus->Root->PmeEvent[PciRootPmeEventCancelWaitWake],
                           IO_NO_INCREMENT,
                           FALSE);
                ActionFlags &= ~PciBusPowerPolicyFlagCancelWW;
                break;

            case PciBusPowerPolicyFlagWakeReady:
                PciBus_PowerStateM(Bus, PciBusEventWakeReady);
                ActionFlags &= ~PciBusPowerPolicyFlagWakeReady;
                break;
            case PciBusPowerPolicyFlagSignalRemoval:
                if (Bus->Removing == 1) {
                    KeSetEvent(&Bus->BusPowerEvent, IO_NO_INCREMENT, FALSE);
                }
                ActionFlags &= ~PciBusPowerPolicyFlagSignalRemoval;
                break;
            default:
                PCI_ASSERTMSG("Unknown action flag", FALSE);
                ActionFlags = 0;
            }
        }

        if (NextState != MaxState) {
            if (Express != FALSE) {
                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_INFORMATION,
                            DBG_PPO,
                            "(%x): From %!BUS_EXPRESS_WAKE_STATE! to %!BUS_EXPRESS_WAKE_STATE!",
                            Bus->SecondaryBusNumber,
                            Bus->BusWakeState,
                            NextState); 
            } else {
                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_INFORMATION,
                            DBG_PPO,
                            "(%x): From %!BUS_CONVENTIONAL_WAKE_STATE! to %!BUS_CONVENTIONAL_WAKE_STATE!",
                            Bus->SecondaryBusNumber,
                            Bus->BusWakeState,
                            NextState); 
            }

            Bus->BusWakeState = NextState;
        } else {
            goto Exit;
        }
    } while (TRUE);

Exit:

    KeReleaseSpinLock(&Bus->BusWakeLock, OldIrql);
    return;
}
