/*++

Copyright (c) 2002 Microsoft Corporation

Module Name:

    wake.c

Abstract:

    This module contains code to handle
    IRP_MJ_POWER dispatches for SD controllers

Authors:

    Neil Sandlin (neilsa) Jan 1, 2002

Environment:

    Kernel mode only

Notes:

Revision History:


--*/

#include "pch.h"
#include "wake.tmh"

//
// Internal References
//


IO_COMPLETION_ROUTINE SdbusFdoWaitWakeIoCompletion;
/*
NTSTATUS
SdbusFdoWaitWakeIoCompletion(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP           Irp,
    IN PVOID          Context
    );
*/

DRIVER_CANCEL SdbusPdoWaitWakeCancelRoutine;
/*
VOID
SdbusPdoWaitWakeCancelRoutine(
    IN PDEVICE_OBJECT Pdo,
    IN OUT PIRP Irp
    );
*/

REQUEST_POWER_COMPLETE SdbusFdoWaitWakePoCompletion;
/*
VOID
SdbusFdoWaitWakePoCompletion(
    IN PDEVICE_OBJECT Fdo,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    );
*/


/**************************************************************************

   FDO ROUTINES

 **************************************************************************/


NTSTATUS
SdbusFdoWaitWake(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP           Irp
    )
/*++


Routine Description

   Handles WAIT_WAKE for the given sd controller

Arguments

   Pdo - Pointer to the functional device object for the sd controller
   Irp - The IRP_MN_WAIT_WAKE Irp

Return Value

   STATUS_PENDING    - Wait wake is pending
   STATUS_SUCCESS    - Wake is already asserted, wait wake IRP is completed
                       in this case
   Any other status  - Error
--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    WAKESTATE oldWakeState;

    //
    // Record the wait wake Irp..
    //
    fdoExtension->WaitWakeIrp = Irp;

    oldWakeState = InterlockedCompareExchange((LONG *) &fdoExtension->WaitWakeState,
                                              WAKESTATE_ARMED, WAKESTATE_WAITING);

    DebugPrint((SDBUS_DEBUG_POWER, "fdo %p irp %p WaitWake: prevState %s\n",
                                     Fdo, Irp, WAKESTATE_STRING(oldWakeState)));

    if (oldWakeState == WAKESTATE_WAITING_CANCELLED) {
       fdoExtension->WaitWakeState = WAKESTATE_COMPLETING;

       Irp->IoStatus.Status = STATUS_CANCELLED;
       IoCompleteRequest(Irp, IO_NO_INCREMENT);
       return STATUS_CANCELLED;
    }

    SdbusAddFxDbgLog(fdoExtension,
                     'WwAr',
                     fdoExtension->Active,
                     fdoExtension->FxState,
                     fdoExtension->DevicePowerState);

    IoMarkIrpPending(Irp);

    IoCopyCurrentIrpStackLocationToNext (Irp);
    //
    // Set our completion routine in the Irp..
    //
    IoSetCompletionRoutine(Irp,
                           SdbusFdoWaitWakeIoCompletion,
                           Fdo,
                           TRUE,
                           TRUE,
                           TRUE);

    SetDeviceFlag(fdoExtension, SDBUS_FDO_WAKE_BY_CD);

    //
    // now pass this down to the lower driver..
    //
    PoCallDriver(fdoExtension->LowerDevice, Irp);
    
    return STATUS_PENDING;
}


NTSTATUS
SdbusFdoWaitWakeIoCompletion(
    IN PDEVICE_OBJECT Fdo,
    IN PIRP           Irp,
    IN PVOID          Context
    )
/*++

Routine Description:

   Completion routine for the IRP_MN_WAIT_WAKE request for this
   sd controller. This is called when the WAIT_WAKE IRP is
   completed by the lower driver (PCI/ACPI) indicating either that
   1. SD bus controller asserted wake
   2. WAIT_WAKE was cancelled
   3. Lower driver returned an error for some reason

Arguments:
   Fdo            -    Pointer to Functional device object for the sd controller
   Irp            -    Pointer to the IRP for the  power request (IRP_MN_WAIT_WAKE)
   Context        -    Not used

Return Value:

   STATUS_SUCCESS   - WAIT_WAKE was completed with success
   Any other status - Wake could be not be accomplished.

--*/
{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    WAKESTATE oldWakeState;

    UNREFERENCED_PARAMETER(Context);

#if defined(CAPTURE_ACTIVE_IDLE)
    InterlockedIncrement( &fdoExtension->Dbg.WAITWAKECount );
#endif;

    oldWakeState = InterlockedExchange((LONG *) &fdoExtension->WaitWakeState, WAKESTATE_COMPLETING);

    DebugPrint((SDBUS_DEBUG_POWER, "fdo %p irp %p WW IoComp: prev=%s\n",
                                     Fdo, Irp, WAKESTATE_STRING(oldWakeState)));

    if (oldWakeState != WAKESTATE_ARMED) {
       NT_ASSERT(oldWakeState == WAKESTATE_ARMING_CANCELLED);
       return STATUS_MORE_PROCESSING_REQUIRED;
    }

    if (IsDeviceFlagSet(fdoExtension, SDBUS_FDO_WAKE_BY_CD)) {

        SdbusAddFxDbgLog(fdoExtension,
                         'WwCo',
                         fdoExtension->Active,
                         fdoExtension->FxState,
                         fdoExtension->DevicePowerState);

        ResetDeviceFlag(fdoExtension, SDBUS_FDO_WAKE_BY_CD);

        //
        // We should only initiate a card detect if the W/W irp completed successfully,
        // meaning there was a card insertion / removal.
        //

        if (NT_SUCCESS(Irp->IoStatus.Status)) {

#if defined(CAPTURE_CARD_DETECT)
            InterlockedIncrement( &fdoExtension->Dbg.CardDetectViaWaitWake );
#endif

            KeInsertQueueDpc(&fdoExtension->CardDetectDpc, NULL, NULL);
        }
    }

    return Irp->IoStatus.Status;
}



VOID
SdbusFdoWaitWakePoCompletion(
    IN PDEVICE_OBJECT Fdo,
    IN UCHAR MinorFunction,
    IN POWER_STATE PowerState,
    IN PVOID Context,
    IN PIO_STATUS_BLOCK IoStatus
    )
/*++

Routine Description

   This routine is called on completion of a D irp generated by an S irp.

Parameters

   DeviceObject   -  Pointer to the Fdo for the SDBUS controller
   MinorFunction  -  Minor function of the IRP_MJ_POWER request
   PowerState     -  Power state requested
   Context        -  Context passed in to the completion routine
   IoStatus       -  Pointer to the status block which will contain
                     the returned status
Return Value

   Status

--*/
{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;

    UNREFERENCED_PARAMETER(MinorFunction);
    UNREFERENCED_PARAMETER(PowerState);
    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(IoStatus);

    DebugPrint((SDBUS_DEBUG_POWER, "fdo %p irp %p WaitWakePoCompletion: prevState %s\n",
                                     Fdo, fdoExtension->WaitWakeIrp,
                                     WAKESTATE_STRING(fdoExtension->WaitWakeState)));

    NT_ASSERT (fdoExtension->WaitWakeIrp);
    fdoExtension->WaitWakeIrp = NULL;
    NT_ASSERT (fdoExtension->WaitWakeState == WAKESTATE_COMPLETING);
    fdoExtension->WaitWakeState = WAKESTATE_DISARMED;
}



NTSTATUS
SdbusFdoArmForWake(
    PFDO_EXTENSION FdoExtension
    )
/*++

Routine Description:

   This routine is called to enable the controller for wake. It is called by the Pdo
   wake routines when a wake-enabled controller gets a wait-wake irp, and also by
   the idle routine to arm for wake from D3 by card insertion.

Arguments:

   FdoExtension - device extension of the controller

Return Value:

   status

--*/
{
    NTSTATUS status = STATUS_PENDING;
    LONG oldWakeState;
    POWER_STATE powerState;

    oldWakeState = InterlockedCompareExchange((LONG *) &FdoExtension->WaitWakeState,
                                              WAKESTATE_WAITING, WAKESTATE_DISARMED);

    DebugPrint((SDBUS_DEBUG_POWER, "fdo %p ArmForWake: prevState %s\n",
                                     FdoExtension->DeviceObject, WAKESTATE_STRING(oldWakeState)));

    if ((oldWakeState == WAKESTATE_ARMED) || (oldWakeState == WAKESTATE_WAITING)) {
       return STATUS_SUCCESS;
    }
    if (oldWakeState != WAKESTATE_DISARMED) {
       return STATUS_UNSUCCESSFUL;
    }

    powerState.SystemState = FdoExtension->DeviceCapabilities.SystemWake;

    status = PoRequestPowerIrp(FdoExtension->DeviceObject,
                               IRP_MN_WAIT_WAKE,
                               powerState,
                               SdbusFdoWaitWakePoCompletion,
                               NULL,
                               NULL);

    if (!NT_SUCCESS(status)) {
        FdoExtension->WaitWakeState = WAKESTATE_DISARMED;
        DebugPrint((SDBUS_DEBUG_POWER, "WaitWake to FDO, expecting STATUS_PENDING, got %08X\n", status));      
    }

    return status;
}



NTSTATUS
SdbusFdoDisarmWake(
    PFDO_EXTENSION FdoExtension
    )
/*++

Routine Description:

   This routine is called to disable the controller for wake.

Arguments:

   FdoExtension - device extension of the controller

Return Value:

   status

--*/
{
    WAKESTATE oldWakeState;

    oldWakeState = InterlockedCompareExchange((LONG *) &FdoExtension->WaitWakeState,
                                              WAKESTATE_WAITING_CANCELLED, WAKESTATE_WAITING);

    DebugPrint((SDBUS_DEBUG_POWER, "fdo %p DisarmWake: prevState %s\n",
                                     FdoExtension->DeviceObject, WAKESTATE_STRING(oldWakeState)));

    SdbusAddFxDbgLog(FdoExtension,
                     'WwDa',
                     FdoExtension->Active,
                     FdoExtension->FxState,
                     FdoExtension->DevicePowerState);

    if (oldWakeState != WAKESTATE_WAITING) {

       oldWakeState = InterlockedCompareExchange((LONG *) &FdoExtension->WaitWakeState,
                                                 WAKESTATE_ARMING_CANCELLED, WAKESTATE_ARMED);

       if (oldWakeState != WAKESTATE_ARMED) {
          return STATUS_UNSUCCESSFUL;
       }
    }

    if (oldWakeState == WAKESTATE_ARMED) {
       IoCancelIrp(FdoExtension->WaitWakeIrp);

       //
       // Now that we've cancelled the IRP, try to give back ownership
       // to the completion routine by restoring the WAKESTATE_ARMED state
       //
       oldWakeState = InterlockedCompareExchange((LONG *) &FdoExtension->WaitWakeState,
                                                 WAKESTATE_ARMED, WAKESTATE_ARMING_CANCELLED);

       if (oldWakeState == WAKESTATE_COMPLETING) {
          //
          // We didn't give control back of the IRP in time, we we own it now
          //
          IoCompleteRequest(FdoExtension->WaitWakeIrp, IO_NO_INCREMENT);
       }

    }

    ResetDeviceFlag(FdoExtension, SDBUS_FDO_WAKE_BY_CD);

    return STATUS_SUCCESS;
}

/**************************************************************************

   PDO ROUTINES

 **************************************************************************/


NTSTATUS
SdbusPdoWaitWake(
   IN  PDEVICE_OBJECT Pdo,
   IN  PIRP           Irp,
   OUT BOOLEAN       *CompleteIrp
   )
/*++


Routine Description

   Handles WAIT_WAKE for the given pc-card.

Arguments

   Pdo - Pointer to the device object for the pc-card
   Irp - The IRP_MN_WAIT_WAKE Irp
   CompleteIrp - This routine will set this to TRUE if the IRP should be
                 completed after this is called and FALSE if it should not be
                 touched

Return Value

   STATUS_PENDING    - Wait wake is pending
   STATUS_SUCCESS    - Wake is already asserted, wait wake IRP is completed
                       in this case
   Any other status  - Error
--*/
{

   PPDO_EXTENSION pdoExtension  = Pdo->DeviceExtension;
   PFDO_EXTENSION fdoExtension  = pdoExtension->Socket->FdoExtension;   
   PDRIVER_CANCEL cancelRoutine = NULL;
   NTSTATUS       status;

   *CompleteIrp = FALSE;

   if ((pdoExtension->DeviceCapabilities.DeviceWake == PowerDeviceUnspecified) ||
       (pdoExtension->DeviceCapabilities.DeviceWake < pdoExtension->DevicePowerState)) {
      //
      // Either we don't support wake at all OR the current device power state
      // of the PC-Card doesn't support wake
      //
      return STATUS_INVALID_DEVICE_STATE;
   }

   if (pdoExtension->Flags & SDBUS_PDO_WAKE_PENDING) {
      //
      // A WAKE is already pending
      //
      return STATUS_DEVICE_BUSY;
   }

   status = SdbusFdoArmForWake(fdoExtension);

   if (!NT_SUCCESS(status)) {
      return status;
   }

   //for the time being, expect STATUS_PENDING from FdoArmForWake
   NT_ASSERT(status == STATUS_PENDING);

   //
   // Parent has one (more) waiter..
   //
   InterlockedIncrement((LONG volatile *) &fdoExtension->ChildWaitWakeCount);
   //for testing, make sure there is only one waiter
   NT_ASSERT (fdoExtension->ChildWaitWakeCount == 1);


   pdoExtension->WaitWakeIrp = Irp;
   pdoExtension->Flags |= SDBUS_PDO_WAKE_PENDING;

   //
   // Set Ring enable/cstschg for the card here..
   //
//   (*socket->SocketFnPtr->PCBEnableDisableWakeupEvent)(socket, pdoExtension, TRUE);

   //
   // PCI currently does not do anything with a WW irp for a cardbus PDO. So we hack around
   // this here by not passing the irp down. Instead it is held pending here, so we can
   // set a cancel routine just like the read PDO driver would. If PCI were to do something
   // with the irp, we could code something like the following:
   //
   // if (IsCardBusCard(pdoExtension)) {
   //    IoSetCompletionRoutine(Irp, SdbusPdoWaitWakeCompletion, pdoExtension,TRUE,TRUE,TRUE);
   //    IoCopyCurrentIrpStackLocationToNext(Irp);
   //    status = IoCallDriver (pdoExtension->LowerDevice, Irp);
   //    NT_ASSERT (status == STATUS_PENDING);
   //    return status;
   // }

   //
   // Allow IRP to be cancelled. We could use a dedicated spin lock here for
   // performance instead of taking the system cancel spin lock.
   //

   IoAcquireCancelSpinLock(&Irp->CancelIrql);

   cancelRoutine = IoSetCancelRoutine(Irp, SdbusPdoWaitWakeCancelRoutine);
   NT_ASSERT(cancelRoutine == NULL);

   if (Irp->Cancel) {
      cancelRoutine = IoSetCancelRoutine(Irp, NULL);

      if (cancelRoutine == NULL) {

         //
         // This means that the cancel routine was already invoked, so there's
         // nothing more to do.
         //

         IoReleaseCancelSpinLock(Irp->CancelIrql);   
         status = STATUS_SUCCESS;
         
      } else {

         //
         // Cancel and complete the IRP because the cancel routine hasn't been 
         // invoked yet.
         //

         IoReleaseCancelSpinLock(Irp->CancelIrql);
         status = STATUS_CANCELLED;
         Irp->IoStatus.Information = 0;
         Irp->IoStatus.Status = status;
         IoCompleteRequest(Irp, IO_NO_INCREMENT);
      }
   } else {

      IoReleaseCancelSpinLock(Irp->CancelIrql);
      
      status = STATUS_PENDING;

      IoMarkIrpPending(Irp);

      IoSetCompletionRoutine(Irp,
                          SdbusPdoWaitWakeCompletion,
                          pdoExtension,
                          TRUE,
                          TRUE,
                          TRUE);
   }

   return status;
}



NTSTATUS
SdbusPdoWaitWakeCompletion(
   IN PDEVICE_OBJECT Pdo,
   IN PIRP           Irp,
   IN PPDO_EXTENSION PdoExtension
   )
/*++

Routine Description

   Completion routine called when a pending IRP_MN_WAIT_WAKE Irp completes

Arguments

   Pdo  -   Pointer to the physical device object for the pc-card
   Irp  -   Pointer to the wait wake IRP
   PdoExtension - Pointer to the device extension for the Pdo

Return Value

   Status from the IRP

--*/
{
   PFDO_EXTENSION fdoExtension = PdoExtension->Socket->FdoExtension;

   DebugPrint((SDBUS_DEBUG_POWER, "pdo %p irp %p --> WaitWakeCompletion\n", Pdo, Irp));

   NT_ASSERT (PdoExtension->Flags & SDBUS_PDO_WAKE_PENDING);

   PdoExtension->Flags &= ~SDBUS_PDO_WAKE_PENDING;
   PdoExtension->WaitWakeIrp = NULL;
   //
   // Reset ring enable/cstschg
   //

//   (*socket->SocketFnPtr->PCBEnableDisableWakeupEvent)(socket, PdoExtension, FALSE);

   NT_ASSERT (fdoExtension->ChildWaitWakeCount > 0);
   InterlockedDecrement((LONG volatile *) &fdoExtension->ChildWaitWakeCount);
   //
   // Wake completed
   //

   return Irp->IoStatus.Status;
}



VOID
SdbusPdoWaitWakeCancelRoutine(
   IN PDEVICE_OBJECT Pdo,
   IN OUT PIRP Irp
   )
/*++

Routine Description:

    Cancel an outstanding (pending) WAIT_WAKE Irp.
    Note: The CancelSpinLock is held on entry

Arguments:

    Pdo     -  Pointer to the physical device object for the pc-card
               on which the WAKE is pending
    Irp     -  Pointer to the WAIT_WAKE Irp to be cancelled

Return Value

    None

--*/
{
   PPDO_EXTENSION pdoExtension = Pdo->DeviceExtension;
   PFDO_EXTENSION fdoExtension = pdoExtension->Socket->FdoExtension;

   DebugPrint((SDBUS_DEBUG_POWER, "pdo %p irp %p --> WaitWakeCancelRoutine\n", Pdo, Irp));
   
   IoSetCancelRoutine(Irp, NULL);

   IoReleaseCancelSpinLock(Irp->CancelIrql);

   if (pdoExtension->WaitWakeIrp == NULL) {
      //
      //  Wait wake already completed/cancelled
      //
      return;
   }

   pdoExtension->Flags &= ~SDBUS_PDO_WAKE_PENDING;
   pdoExtension->WaitWakeIrp = NULL;

   //
   // Reset ring enable, disabling wake..
   //
//   (*socket->SocketFnPtr->PCBEnableDisableWakeupEvent)(socket, pdoExtension, FALSE);

   //
   // Since this is cancelled, see if parent's wait wake
   // needs to be cancelled too.
   // First, decrement the number of child waiters..
   //

   NT_ASSERT (fdoExtension->ChildWaitWakeCount > 0);
   if (InterlockedDecrement((LONG volatile *) &fdoExtension->ChildWaitWakeCount) == 0) {
      //
      // No more waiters.. cancel the parent's wake IRP
      //
      NT_ASSERT(fdoExtension->WaitWakeIrp);

      if (fdoExtension->WaitWakeIrp) {
         IoCancelIrp(fdoExtension->WaitWakeIrp);
      }
   }

   //
   // Complete the IRP
   //
   Irp->IoStatus.Information = 0;

   //
   // Is this necessary?
   //
   PoStartNextPowerIrp(Irp);

   Irp->IoStatus.Status = STATUS_CANCELLED;
   IoCompleteRequest(Irp, IO_NO_INCREMENT);
}
