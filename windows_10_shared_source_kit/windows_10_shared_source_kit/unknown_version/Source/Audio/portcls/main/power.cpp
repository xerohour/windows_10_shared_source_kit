/*****************************************************************************
 * power.cpp - WDM Streaming port class driver
 *****************************************************************************
 *
 * Copyright (c) Microsoft.  All rights reserved.
 *
 */

#include "private.h"

#include "pcwpptr.h"
#include "power.tmh"

#ifndef DEBUGLVL_POWER
#define DEBUGLVL_POWER DEBUGLVL_VERBOSE
#endif

REQUEST_POWER_COMPLETE  DevicePowerIrpCompletionRoutine;
REQUEST_POWER_COMPLETE  SystemPowerUpDeviceIrpCompletionRoutine;
IO_COMPLETION_ROUTINE   PowerIrpCompletionRoutine;
REQUEST_POWER_COMPLETE  RequestPowerIrpCompletionRoutine;

NTSTATUS
ProcessPowerIrp
(
    IN      PIRP                pIrp,
    IN      PIO_STACK_LOCATION  pIrpStack,
    IN      PDEVICE_OBJECT      pDeviceObject
);

void
PowerNotifySubdevices
(
    IN  PDEVICE_CONTEXT     pDeviceContext,
    IN  POWER_STATE         PowerState
);

#pragma code_seg()
/*****************************************************************************
 * iSetIdlePowerManagement()
 *****************************************************************************
 * Helper method for the ports to handle 
 * IPortClsPower::SetIdlePowerManagement().  This method sets the idle
 * power managmenent enable flag in the device context.  Also, this method
 * will register or unregister idle power management from the power manager
 * if necessary.  We need to do the registration because the miniport may call
 * this method at any time.
 *
 */
NTSTATUS
iSetIdlePowerManagement
(
    IN PDEVICE_OBJECT _pDeviceObject,
    IN BOOLEAN        _bEnabled
)
{
    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_POWER,
                         "Device: %p, enable idle-power-management: %d",
                         _pDeviceObject,
                         (int)_bEnabled);
    
    // Validate input
    if ( (NULL == _pDeviceObject) || (NULL == _pDeviceObject->DeviceExtension) )
    {
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT deviceContext =
        PDEVICE_CONTEXT(_pDeviceObject->DeviceExtension);

    deviceContext->EnableIdlePowerManagement = _bEnabled;

    //
    // If the runtime power framework is already managing our device
    // let's go ahead and call the framework to set the new idle.
    //
    if (deviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted)
    {
        NTSTATUS ntStatus               = STATUS_SUCCESS;
        BOOL     toggleComponentPower   = FALSE;
        
        //
        // Check if the miniport has registered an IAdapterPowerManagement3 (WIN8+ only) interface.
        //
        if (NULL != deviceContext->pAdapterPower)
        {
            PVOID pPower = NULL;

            //
            // QI for IAdapterPowerManagement3
            //
            ntStatus = deviceContext->pAdapterPower->QueryInterface
            (
                IID_IAdapterPowerManagement3,
                &pPower
            );

            if (NT_SUCCESS(ntStatus) && pPower != NULL)
            {
                //
                // We got an IAdapterPowerManagement3 interface, go ahead
                // and toggle active idle so the new timeout will take effect.
                //
                toggleComponentPower = TRUE;

                //
                // Release the interface.
                //
                PUNKNOWN( pPower )->Release();
                pPower = NULL;
            }
        }

        //
        // If the miniport has registered an IAdapterPowerManagement3 (WIN8+ only) interface,
        // toggle active idle so that the new timeout will take effect.
        // Failures below are non-fatal as the new timeout will take effect the next time the
        // device reports idle.
        //
        if (toggleComponentPower)
        {
            ntStatus = ActivateRuntimePowerComponent( deviceContext, TRUE );
            if (!NT_SUCCESS(ntStatus))
            {
                DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                     PCTRACE_POWER,
                                     "Device %p, ActivateRuntimePowerComponent failed, "
                                     "%!STATUS!", 
                                     _pDeviceObject,
                                     ntStatus);

                // 
                // Do not call IdleRuntimePowerComponent.
                //
                toggleComponentPower = FALSE;
            }
        }
        
        SetRuntimePowerIdleTimeout( deviceContext, GetCurrentRuntimeIdleTimeout( deviceContext ) );

        //
        // If the miniport has registered an IAdapterPowerManagement3 (WIN8+ only) interface,
        // idle the component.
        // Failures below are non-fatal as the new timeout will take effect the next time the
        // device reports idle.
        //
        if (toggleComponentPower)
        {
            ntStatus = IdleRuntimePowerComponent( deviceContext, TRUE );
            if (!NT_SUCCESS(ntStatus))
            {
                DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                     PCTRACE_POWER,
                                     "Device %p, IdleRuntimePowerComponent failed, "
                                     "%!STATUS!",
                                     _pDeviceObject,
                                     ntStatus);
            }
        }
    }

    return STATUS_SUCCESS;
}

#pragma code_seg()
PC_EXIT_LATENCY
GetCurrentD3ExitLatency
(
    IN      PDEVICE_CONTEXT     pDeviceContext,
    IN      DEVICE_POWER_STATE  DeviceState,
    IN      SYSTEM_POWER_STATE  SystemState
)
{
    //
    // If the device is powered up, then the device must be able
    // to respond immediately.
    //
    if (PowerDeviceD0 == DeviceState)
    {
        return PcExitLatencyInstant;
    }

    //
    // If the system is entering a sleep state, then the longest
    // exit latency can be tolerated.
    //
    if (PowerSystemWorking != SystemState)
    {
        return PcExitLatencyResponsive;
    }

    //
    // If the system is in Connected Standby, then the longest
    // exit latency can be tolerated.
    //
    if ((NULL != pDeviceContext->RuntimePowerInfo) &&
        (TRUE == pDeviceContext->RuntimePowerInfo->LowPowerEpoch))
    {
        return PcExitLatencyResponsive;
    }

    //
    // If the system is in S0 and not in Connected Standby, then
    // it must be fast when powering up.
    //
    return PcExitLatencyFast;
}


#pragma code_seg()
VOID
iPowerChangeState
(
    IN      PDEVICE_CONTEXT     pDeviceContext,
    IN      DEVICE_POWER_STATE  DeviceState,
    IN      SYSTEM_POWER_STATE  SystemState
)
{
    PVOID pPower = NULL;
    NTSTATUS ntStatus = STATUS_SUCCESS;

    // Validate input
    if ( NULL == pDeviceContext || NULL == pDeviceContext->pAdapterPower )
    {
        goto exit;
    }

    // Query for IAdapterPowerManagement3
    ntStatus = pDeviceContext->pAdapterPower->QueryInterface
    (
        IID_IAdapterPowerManagement3,
        &pPower
    );
    if ( NT_SUCCESS(ntStatus) )
    {
        // We got an IAdapterPowerManagment3 interface.
        PADAPTERPOWERMANAGEMENT3( pPower )->PowerChangeState3(DeviceState, SystemState, GetCurrentD3ExitLatency(pDeviceContext, DeviceState, SystemState));
        
        goto exit;
    }
    
    // Query for IAdapterPowerManagement2
    ntStatus = pDeviceContext->pAdapterPower->QueryInterface
    (
        IID_IAdapterPowerManagement2,
        &pPower
    );
    if ( NT_SUCCESS(ntStatus) )
    {
        // We got an IAdapterPowerManagement2 interface.
        PADAPTERPOWERMANAGEMENT2( pPower )->PowerChangeState2(DeviceState, SystemState);

        goto exit;
    }

    // Query for IAdapterPowerManagement
    ntStatus = pDeviceContext->pAdapterPower->QueryInterface
    (
        IID_IAdapterPowerManagement,
        &pPower
    );
    if ( NT_SUCCESS(ntStatus) )
    {
        // We got an IAdapterPowerManagement interface.
        POWER_STATE NewPowerState;
        NewPowerState.DeviceState = DeviceState;
        PADAPTERPOWERMANAGEMENT( pPower )->PowerChangeState( NewPowerState );
    }

exit:
    if (NULL != pPower)
    {
        PUNKNOWN( pPower )->Release();
        pPower = NULL;
    }

    return;
}

#pragma code_seg("PAGE")
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
)
{
    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[GetDeviceACPIInfo]" );

    ASSERT( pDeviceObject );

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT( pDeviceContext );

    if(pDeviceContext != NULL)
    {
        // Need to call down to the PDO (bus driver)
        // and let it fill out the default for this bus
        ntStatus = ForwardIrpSynchronous( pDeviceContext, pIrp );
        if( NT_SUCCESS(ntStatus) )
        {
            PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(pIrp);
            PDEVICE_CAPABILITIES pDeviceCaps = irpSp->Parameters.DeviceCapabilities.Capabilities;

            ASSERT( pDeviceCaps );
            ASSERT( pDeviceCaps->Size >= sizeof( DEVICE_CAPABILITIES ) );

            if( pDeviceCaps && ( pDeviceCaps->Size >= sizeof( DEVICE_CAPABILITIES ) ) )
            {
                // pass the structure on down to the adapter
                if( pDeviceContext->pAdapterPower )
                {
                    PVOID pPower = NULL;
                    NTSTATUS ntTempStatus = STATUS_SUCCESS;

                    // Query for IAdapterPowerManagement3.
                    ntTempStatus = pDeviceContext->pAdapterPower->QueryInterface
                    (
                        IID_IAdapterPowerManagement3,
                        &pPower
                    );
                    if ( NT_SUCCESS(ntTempStatus) )
                    {
                        // We got an IAdapterPowerManagement3 interface.
                        ntStatus = PADAPTERPOWERMANAGEMENT3( pPower )->QueryDeviceCapabilities( pDeviceCaps );
                    }

                    if ( !NT_SUCCESS(ntTempStatus) )
                    {
                        // Query for IAdapterPowerManagement2.
                        ntTempStatus = pDeviceContext->pAdapterPower->QueryInterface
                        (
                            IID_IAdapterPowerManagement2,
                            &pPower
                        );
                        if ( NT_SUCCESS(ntTempStatus) )
                        {
                            // We got an IAdapterPowerManagement2 interface.
                            ntStatus = PADAPTERPOWERMANAGEMENT2( pPower )->QueryDeviceCapabilities( pDeviceCaps );                        
                        }
                    }

                    if ( !NT_SUCCESS(ntTempStatus) )
                    {
                        // Query for IAdapterPowerManagement.
                        ntTempStatus = pDeviceContext->pAdapterPower->QueryInterface
                        (
                            IID_IAdapterPowerManagement,
                            &pPower
                        );
                        if ( NT_SUCCESS(ntTempStatus) )
                        {
                            // We got an IAdapterPowerManagement interface.
                            ntStatus = PADAPTERPOWERMANAGEMENT( pPower )->QueryDeviceCapabilities( pDeviceCaps );
                        }
                    }

                    ASSERT(ntStatus != STATUS_PENDING);

                    if (NULL != pPower)
                    {
                        PUNKNOWN( pPower )->Release();
                        pPower = NULL;
                    }
                }

                // make sure that we have sensible settings for the system sleep states
                pDeviceCaps->DeviceState[PowerSystemWorking] = PowerDeviceD0;
                ULONG i;
                for(i=ULONG(PowerSystemSleeping1); i <= ULONG(PowerSystemShutdown); i++ )
                {
                    // and we want some sleeping in the sleep modes.
                    //



                    // We go ahead and include this code, even though it is possible that
                    // there are devices that exist that can maintain state in the device
                    // while sleeping.
                    //
                    if(pDeviceCaps->DeviceState[i] == PowerDeviceD0)
                    {
                        pDeviceCaps->DeviceState[i] = PowerDeviceD3;
                    }
                }

                // save in our device extension the stuff we're interested in
                for( i=ULONG(PowerSystemUnspecified); i < ULONG(PowerSystemMaximum); i++)
                {
                    pDeviceContext->DeviceStateMap[ i ] = pDeviceCaps->DeviceState[ i ];
                }
            }
        }

        // complete the irp
        CompleteIrp( pDeviceContext, pIrp, ntStatus );

        // attempt to get the idle info from the registry
        if( NT_SUCCESS(ntStatus) )
        {
            ULONG ConservationIdleTime;
            ULONG PerformanceIdleTime;
            DEVICE_POWER_STATE IdleDeviceState;
            BOOLEAN ImmediateIdle;

            NTSTATUS ntStatus2 = GetIdleInfoFromRegistry( pDeviceContext,
                                                          &ConservationIdleTime,
                                                          &PerformanceIdleTime,
                                                          &IdleDeviceState,
                                                          &ImmediateIdle);
            if(NT_SUCCESS(ntStatus2))
            {
                pDeviceContext->ConservationIdleTime = ConservationIdleTime;
                pDeviceContext->PerformanceIdleTime = PerformanceIdleTime;
                pDeviceContext->IdleDeviceState = IdleDeviceState;
                //
                // If the device has been removed already, then the RuntimePowerInfo
                // structure will have been freed already.
                //
                if (NULL != pDeviceContext->RuntimePowerInfo)
                {
                    pDeviceContext->RuntimePowerInfo->ImmediateIdle = ImmediateIdle;
                }
            }

            //
            // If the runtime power framework is already managing our device
            // let's go ahead and call the framework to set the new idle.
            //
            if ((NULL != pDeviceContext->RuntimePowerInfo) && 
                (pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted))
            {
                SetRuntimePowerIdleTimeout( pDeviceContext, GetCurrentRuntimeIdleTimeout( pDeviceContext ) );
            }

            DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                 PCTRACE_POWER,
                                 "[GetDeviceACPIInfo] Idle Detection Enabled %d %d %d %s",
                                 pDeviceContext->ConservationIdleTime,
                                 pDeviceContext->PerformanceIdleTime,
                                 ULONG(pDeviceContext->IdleDeviceState),
                                 pDeviceContext->IdleTimer ? "" : "FAILED!" );
        }
    }

    return ntStatus;
}

#pragma code_seg()
/*****************************************************************************
 * DevicePowerIrpCompletionRoutine
 *****************************************************************************
 * Completion routine for device power irps resulting from query system power
 * IRPs and system sleep power IRPs.
 *
 */
VOID
DevicePowerIrpCompletionRoutine
(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      UCHAR               MinorFunction,
    IN      POWER_STATE         PowerState,
    IN      PVOID               pContext,
    IN      PIO_STATUS_BLOCK    pIoStatus
)
{
    ASSERT(pDeviceObject);
    ASSERT(pIoStatus);
    ASSERT(pContext);

    PPOWER_DEVICE_CONTEXT pCompletionContext = (PPOWER_DEVICE_CONTEXT)pContext;

    ASSERT(pCompletionContext->pDeviceContext);
    ASSERT(pCompletionContext->pSystemIrp);

    if((pCompletionContext != NULL) && (pCompletionContext->pSystemIrp != NULL))
    {
        // set the return code from the device Irp into the system Irp
        pCompletionContext->pSystemIrp->IoStatus.Status = pIoStatus->Status;

        // start the next power Irp
        PoStartNextPowerIrp( pCompletionContext->pSystemIrp );

        // release the remove lock for the system Irp
        ReleaseRemoveLock( pCompletionContext->pDeviceContext, pCompletionContext->pSystemIrp );

        DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                             PCTRACE_POWER,
                             "Device %p, device power Irp completed, completing System Irp %p, %!STATUS!",
                             pDeviceObject,
                             pCompletionContext->pSystemIrp,
                             pIoStatus->Status);

        // complete the system Irp
        IoCompleteRequest( pCompletionContext->pSystemIrp, IO_NO_INCREMENT );

        // free the context
        ExFreePool( pCompletionContext );
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * SystemPowerUpDeviceIrpCompletionWorker()
 *****************************************************************************
 * Called in the SystemPowerUpIrpCompletionRoutine in order to ensure that
 * runtime power management is resumed if SystemPowerUpIrpCompletionRoutine
 * is called at DISPATCH_LEVEL. This is done in a work item so that we can 
 * block waiting for the runtime power framework to complete resuming. The 
 * system power IRP is not completed here because it was already completed 
 * for fast resume support.
 */
QUEUED_CALLBACK_RETURN
SystemPowerUpDeviceIrpCompletionWorker
(
    IN  PDEVICE_OBJECT      pDeviceObject,
    IN  PVOID               pContext
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(pDeviceObject);

    PPOWER_DEVICE_CONTEXT pCompletionContext = (PPOWER_DEVICE_CONTEXT)pContext;

    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_POWER,
                         "Device %p",
                         pDeviceObject);

    ASSERT(pCompletionContext->pDeviceContext);

    if((pCompletionContext != NULL) && (pCompletionContext->pDeviceContext != NULL))
    {
        //
        // Let the runtime power management framework know the device is now 
        // powered on.
        //
        NTSTATUS ntStatus = ReportRuntimePowerComponentPoweredOn(pCompletionContext->pDeviceContext);
        if (!NT_SUCCESS(ntStatus))
        {
            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                 PCTRACE_POWER,
                                 "Device %p, failed to report device powered on, %!STATUS!",
                                 pDeviceObject,
                                 ntStatus );
        }

        if(pCompletionContext->pDeviceContext->RuntimePowerInfo != NULL)
        {
            //
            // Flag that the runtime framework will now be managing our state.
            //
            pCompletionContext->pDeviceContext->RuntimePowerInfo->RuntimeSystemWorking = TRUE;
        }

        //
        // Free the context
        //
        ExFreePool( pCompletionContext );
    }

    return QUEUED_CALLBACK_FREE;
}

#pragma code_seg()
/*****************************************************************************
 * SystemPowerUpDeviceIrpCompletionRoutine
 *****************************************************************************
 * Completion routine for device power irps resulting from a system resume
 * power IRP. The system power IRP is not completed here because it was
 * already completed for fast resume support.
 *
 */
VOID
SystemPowerUpDeviceIrpCompletionRoutine
(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      UCHAR               MinorFunction,
    IN      POWER_STATE         PowerState,
    IN      PVOID               pContext,
    IN      PIO_STATUS_BLOCK    pIoStatus
)
{
    ASSERT(pDeviceObject);
    ASSERT(pIoStatus);
    ASSERT(pContext);

    PPOWER_DEVICE_CONTEXT pCompletionContext = (PPOWER_DEVICE_CONTEXT)pContext;

    ASSERT(pCompletionContext->pDeviceContext);

    if((pCompletionContext != NULL))
    {
        if( DISPATCH_LEVEL == KeGetCurrentIrql() )
        {
            //
            // At DISPATCH_LEVEL, so we need to queue a work item to resume runtime
            // power management.
            //
            (void) CallbackEnqueue(
                    &pCompletionContext->pDeviceContext->RuntimePowerInfo->pWorkQueueItemSystemPower,
                    SystemPowerUpDeviceIrpCompletionWorker,
                    pDeviceObject,
                    pContext,
                    PASSIVE_LEVEL,
                    EQCF_DIFFERENT_THREAD_REQUIRED
                    );
        }
        else
        {
            //
            // Since we're already at the right level, just call the work item
            // routine directly.
            //
            (void) SystemPowerUpDeviceIrpCompletionWorker(pDeviceObject, pContext);
        }
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * DevicePowerWorker()
 *****************************************************************************
 * Called in the PowerIrpCompletionRoutine in order to notify the device of
 * the state change if the PowerIrpCompletionRoutine is called at DISPATCH_LEVEL.
 * This is done in a work item so that paged code can be called in the process
 * of changing the power state.
 */
QUEUED_CALLBACK_RETURN
DevicePowerWorker
(
    IN  PDEVICE_OBJECT      pDeviceObject,
    IN  PVOID               pContext
)
{
    PAGED_CODE();

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    POWER_STATE NewPowerState;
    PIRP pIrp = (PIRP)pContext;
    PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation( pIrp );
    BOOLEAN relPositionLock = FALSE;

    NewPowerState = pIrpStack->Parameters.Power.State;

    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_POWER,
                         "Device %p, NewPowerState=D%d",
                         pDeviceObject,
                         NewPowerState.DeviceState - PowerDeviceD0);

    // acquire the device so we're sync'ed with creates
    AcquireDevice(pDeviceContext);

    // acquire dispatcher locks to drain I/O requests.
    if (pDeviceContext->UsePositionLock)
    {
        AcquirePositionLock(pDeviceContext);
        relPositionLock = TRUE;
    }

    // change the driver state if it has a registered POWER interface
    if( pDeviceContext->pAdapterPower )
    {
        iPowerChangeState(pDeviceContext, NewPowerState.DeviceState, pDeviceContext->CurrentSystemState);
    }

    // keep track of new state
    pDeviceContext->CurrentDeviceState = NewPowerState.DeviceState;

    // notify everyone we're now in our lighter D-state
    PoSetPowerState( pDeviceObject,
                     DevicePowerState,
                     NewPowerState );

    PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                   PC_ETYPE_PWR_D0 + (UCHAR)(NewPowerState.DeviceState - PowerDeviceD0),
                   PortCls_PowerState_GUID,
                   (ULONGLONG)pDeviceObject,
                   NULL,
                   1,       // 0 = Request, 1 = Completion
                   GetCurrentD3ExitLatency(pDeviceContext, NewPowerState.DeviceState, pDeviceContext->CurrentSystemState),
                   (ULONGLONG)NULL );

    PowerNotifySubdevices( pDeviceContext, NewPowerState );

    // set PendCreates appropriately
    if( pDeviceContext->DeviceStopState == DeviceStarted )
    {
        // start allowing creates
        pDeviceContext->PendCreates = FALSE;
    



        //  We now use the mutex here to prevent
        //  context switches or multiprocessor multithreading from causing
        //  a deadlock of all audio (pending and never completing synchronous IRP)
        
        CompletePendedIrps( pDeviceObject,
                            pDeviceContext,
                            EMPTY_QUEUE_AND_PROCESS );
    }

    // release locks.
    if (relPositionLock)
    {
        ReleasePositionLock(pDeviceContext);
        relPositionLock = FALSE;
    }
    
    ReleaseDevice(pDeviceContext);

    //
    // start the next power Irp, complete the current one, and release the remove lock.
    //
    PoStartNextPowerIrp( pIrp );
    IoCompleteRequest( pIrp, IO_NO_INCREMENT );
    ReleaseRemoveLock( pDeviceContext, pIrp );

    return QUEUED_CALLBACK_FREE;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * SystemPowerDownWorkerInternal()
 *****************************************************************************
 * Handles the case where system is going to sleep.
 */
NTSTATUS SystemPowerDownWorkerInternal(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PIRP                pIrp
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);
    ASSERT(pIrp);

    NTSTATUS        ntStatus = STATUS_SUCCESS;
    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT( pDeviceObject->DeviceExtension );
    POWER_STATE     newDevicePowerState;
    
    //
    // Get the necessary device power state given the new system power state.
    //
    newDevicePowerState.DeviceState = pDeviceContext->DeviceStateMap[ pDeviceContext->CurrentSystemState ];

    //
    // Do a sanity check on the device state. For Bluetooth audio devices enumerated by our Bluetooth stack,
    // it is possible for this map to be all 0 (PowerDeviceUnspecified) except for the location associated with 
    // PowerSystemWorking which is set to D0. 
    //
    if ((newDevicePowerState.DeviceState < PowerDeviceD0) ||
        (newDevicePowerState.DeviceState > PowerDeviceD3) )
    {
        if (pDeviceContext->CurrentSystemState == PowerSystemWorking)
        {
            newDevicePowerState.DeviceState = PowerDeviceD0;
        } else
        {
            newDevicePowerState.DeviceState = PowerDeviceD3;
        }
    }

    if(pDeviceContext->RuntimePowerInfo != NULL)
    {
        //
        // Since we're leaving PowerSystemWorking, the runtime framework will
        // no longer manage our active state, set the flag that will force
        // us to pend IRPs ourselves.
        //
        pDeviceContext->RuntimePowerInfo->RuntimeSystemWorking = FALSE;
    }

    //
    // See if we need to send a Dx Power IRP.  If so, it better be for a system sleep
    // request because we wouldn't have called this worker if we were doing fast resume.
    //
    if (newDevicePowerState.DeviceState != pDeviceContext->CurrentDeviceState)
    {
        PPOWER_DEVICE_CONTEXT pCompletionContext = (PPOWER_DEVICE_CONTEXT)ExAllocatePoolWithTag( NonPagedPoolNx, sizeof(POWER_DEVICE_CONTEXT), 'wPcP' );
        if( NULL == pCompletionContext )
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        } else
        {
            pCompletionContext->pDeviceContext = pDeviceContext;
            pCompletionContext->pSystemIrp = pIrp;

            //
            // request a device set power Irp
            //
            ntStatus = PoRequestPowerIrp(pDeviceContext->PhysicalDeviceObject,
                                            IRP_MN_SET_POWER,
                                            newDevicePowerState,
                                            DevicePowerIrpCompletionRoutine,
                                            pCompletionContext,
                                            NULL );
            if( STATUS_PENDING == ntStatus )
            {
                ntStatus = STATUS_SUCCESS;
            }
            else
            {
                ExFreePool( pCompletionContext );
            }
        }

        if(NT_SUCCESS(ntStatus))
        {
            ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
        } else
        {
            pIrp->IoStatus.Status = ntStatus;
            ReleaseRemoveLock( pDeviceContext, pIrp );
        }
    } else
    {
        pIrp->IoStatus.Status = ntStatus;
        ReleaseRemoveLock( pDeviceContext, pIrp );
    }

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * SystemPowerDownWorker()
 *****************************************************************************
 * 
 */
QUEUED_CALLBACK_RETURN SystemPowerDownWorker(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PVOID               pContext
)
{
    PAGED_CODE();

    NTSTATUS        ntStatus = STATUS_SUCCESS;
    PIRP            pIrp = (PIRP)pContext;
    
    //
    // Call the internal system power work routine to do the real work.
    //
    ntStatus = SystemPowerDownWorkerInternal(pDeviceObject, pIrp);
     
    //
    // Start the next power Irp and complete the current one if
    // we don't need to do anything else.
    //
    if (STATUS_MORE_PROCESSING_REQUIRED != ntStatus)
    {
        PoStartNextPowerIrp( pIrp );
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
    }

    return QUEUED_CALLBACK_FREE;
}

#pragma code_seg()
/*****************************************************************************
 * PowerIrpCompletionRoutine
 *****************************************************************************
 * Completion routine for power irps from PoCallDriver
 *
 */
NTSTATUS
PowerIrpCompletionRoutine
(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      PIRP                pIrp,
    IN      PVOID               pContext
)
{
    ASSERT(pDeviceObject);
    ASSERT(pIrp);
    ASSERT(pContext);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation( pIrp );

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT( pContext );

    switch( pIrpStack->MinorFunction )
    {
        case IRP_MN_QUERY_POWER:
            if( DevicePowerState == pIrpStack->Parameters.Power.Type )
            {
                // Device Query Power
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_POWER,
                                     "Device %p, Irp=%p Device QueryPower",
                                     pDeviceObject,
                                     pIrp );

                // start the next power Irp
                PoStartNextPowerIrp( pIrp );

                // release the remove lock for the device irp
                ReleaseRemoveLock( pDeviceContext, pIrp );

                ntStatus = pIrp->IoStatus.Status;
                
                // Log info.
                DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                     PCTRACE_POWER,
                                     "Device %p, query device power state: D%d -> D%d, %!STATUS!",
                                     pDeviceObject,
                                     pDeviceContext->CurrentDeviceState - PowerDeviceD0,
                                     pIrpStack->Parameters.Power.State.DeviceState - PowerDeviceD0,
                                     ntStatus);

            } else
            {
                // System Query Power
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_POWER,
                                     "Device %p, Irp=%p System QueryPower",
                                     pDeviceObject,
                                     pIrp );

                // Check if lower drivers succeeded the Irp
                ntStatus = pIrp->IoStatus.Status;
                
                // Log info.
                DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                     PCTRACE_POWER,
                                     "Device %p, query system power state: S%d -> S%d, %!STATUS!",
                                     pDeviceObject,
                                     pDeviceContext->CurrentSystemState - PowerSystemWorking,
                                     pIrpStack->Parameters.Power.State.SystemState - PowerSystemWorking,
                                     ntStatus);

                if(NT_SUCCESS(ntStatus))
                {
                    POWER_STATE         newPowerState;

                    // determine appropriate device state
                    newPowerState.DeviceState = pDeviceContext->DeviceStateMap[ pIrpStack->Parameters.Power.State.SystemState ];

                    // do a sanity check on the device state
                    if ((newPowerState.DeviceState < PowerDeviceD0) ||
                        (newPowerState.DeviceState > PowerDeviceD3) )
                    {
                        if (pIrpStack->Parameters.Power.State.SystemState == PowerSystemWorking)
                        {
                            newPowerState.DeviceState = PowerDeviceD0;
                        } else
                        {
                            newPowerState.DeviceState = PowerDeviceD3;
                        }
                    }

                    PPOWER_DEVICE_CONTEXT pCompletionContext = (PPOWER_DEVICE_CONTEXT)ExAllocatePoolWithTag( NonPagedPoolNx, sizeof(POWER_DEVICE_CONTEXT), 'wPcP' );
                    if( NULL == pCompletionContext )
                    {
                        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                    } else
                    {
                        pCompletionContext->pDeviceContext = pDeviceContext;
                        pCompletionContext->pSystemIrp = pIrp;

                        // request a device query power Irp
                        ntStatus = PoRequestPowerIrp(pDeviceContext->PhysicalDeviceObject,
                                                     IRP_MN_QUERY_POWER,
                                                     newPowerState,
                                                     DevicePowerIrpCompletionRoutine,
                                                     pCompletionContext,
                                                     NULL );
                        if( STATUS_PENDING == ntStatus )
                        {
                            ntStatus = STATUS_SUCCESS;
                        }
                        else
                        {
                            ExFreePool( pCompletionContext );
                        }
                    }

                    if(NT_SUCCESS(ntStatus))
                    {
                        ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
                    } else
                    {
                        // System Query Power
                        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                             PCTRACE_POWER,
                                             "Device %p, Irp=%p System QueryPower failed, %!STATUS!",
                                             pDeviceObject,
                                             pIrp,
                                             ntStatus);
                        
                        pIrp->IoStatus.Status = ntStatus;
                        ReleaseRemoveLock( pDeviceContext, pIrp );
                    }
                } else
                {
                    ReleaseRemoveLock( pDeviceContext, pIrp );
                }
            }
            break;

        case IRP_MN_SET_POWER:
            if( DevicePowerState == pIrpStack->Parameters.Power.Type )
            {
                // Device Set Power
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_POWER,
                                     "Device %p, Irp=%p Device SetPower",
                                     pDeviceObject,
                                     pIrp );
                
                if( FALSE == pDeviceContext->PoweringDown )
                {
                    // Log info.
                    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                         PCTRACE_POWER,
                                         "Device %p, set device power state (up): D%d -> D%d, %!STATUS!",
                                         pDeviceObject,
                                         pDeviceContext->CurrentDeviceState - PowerDeviceD0,
                                         pIrpStack->Parameters.Power.State.DeviceState - PowerDeviceD0,
                                         ntStatus);
                    
                    // get the new power state
                    POWER_STATE NewPowerState = pIrpStack->Parameters.Power.State;

                    if( DISPATCH_LEVEL == KeGetCurrentIrql() )
                    {
                        // At DISPATCH_LEVEL, so we need to queue a work item to change the device state
                        ntStatus = CallbackEnqueue(
                                        &pDeviceContext->pWorkQueueItemStart,
                                        DevicePowerWorker,
                                        pDeviceObject,
                                        (PVOID)pIrp,
                                        PASSIVE_LEVEL,
                                        EQCF_DIFFERENT_THREAD_REQUIRED
                                        );

                        if (NT_SUCCESS(ntStatus))
                        {
                            ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
                        } else
                        {
                            pIrp->IoStatus.Status = ntStatus;
                            ReleaseRemoveLock( pDeviceContext, pIrp );
                        }
                    } else
                    {
                        // acquire the device so we're sync'ed with creates
                        ntStatus = AcquireDevice(pDeviceContext);

                        if( NT_SUCCESS(ntStatus) )
                        {
                            BOOLEAN relPositionLock = FALSE;
                            
                            // acquire dispatcher locks to drain I/O requests.
                            if (pDeviceContext->UsePositionLock)
                            {
                                AcquirePositionLock(pDeviceContext);
                                relPositionLock = TRUE;
                            }
                            
                            // keep track of new state
                            pDeviceContext->CurrentDeviceState = NewPowerState.DeviceState;

                            // change the driver state if it has a registered POWER interface
                            if( pDeviceContext->pAdapterPower )
                            {
                                iPowerChangeState(pDeviceContext, NewPowerState.DeviceState, pDeviceContext->CurrentSystemState);
                            }

                            // notify the subdevices
                            PowerNotifySubdevices( pDeviceContext, NewPowerState );

                            // notify everyone we're now in the powered up D state
                            PoSetPowerState( pDeviceObject,
                                             DevicePowerState,
                                             NewPowerState );

                            PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                                           PC_ETYPE_PWR_D0 + (UCHAR)(NewPowerState.DeviceState - PowerDeviceD0),
                                           PortCls_PowerState_GUID,
                                           (ULONGLONG)pDeviceObject,
                                           NULL,
                                           1,       // 0 = Request, 1 = Completion
                                           GetCurrentD3ExitLatency(pDeviceContext, NewPowerState.DeviceState, pDeviceContext->CurrentSystemState),
                                           (ULONGLONG)NULL );

                            // set PendCreates appropriately
                            if( pDeviceContext->DeviceStopState == DeviceStarted )
                            {
                                // start allowing creates
                                pDeviceContext->PendCreates = FALSE;




                                //  We now use the mutex here to prevent
                                //  context switches or multiprocessor multithreading from causing
                                //  a deadlock of all audio (pending and never completing synchronous IRP)

                                CompletePendedIrps( pDeviceObject,
                                                    pDeviceContext,
                                                    EMPTY_QUEUE_AND_PROCESS );
                            }

                            // release locks.
                            if (relPositionLock)
                            {
                                ReleasePositionLock(pDeviceContext);
                                relPositionLock = FALSE;
                            }
                            
                            // release the device
                            ReleaseDevice(pDeviceContext);
                        }

                        // start the next power Irp
                        PoStartNextPowerIrp( pIrp );

                        // release the remove lock for the device irp
                        ReleaseRemoveLock( pDeviceContext, pIrp );
                    }
                }
                else
                {
                    // Log info.
                    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                         PCTRACE_POWER,
                                         "Device %p, set device power state (down): D%d, %!STATUS!",
                                         pDeviceObject,
                                         pIrpStack->Parameters.Power.State.DeviceState - PowerDeviceD0,
                                         ntStatus);
                    
                    // start the next power Irp
                    PoStartNextPowerIrp( pIrp );

                    // release the remove lock for the device irp
                    ReleaseRemoveLock( pDeviceContext, pIrp );
                }

                if(NT_SUCCESS(ntStatus))
                {
                    ntStatus = pIrp->IoStatus.Status;
                }
            } else
            {
                // System Set Power
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_POWER,
                                     "Device %p, Irp=%p System SetPower",
                                     pDeviceObject,
                                     pIrp );
                
                // Log info.
                DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                     PCTRACE_POWER,
                                     "Device %p, set system power state: S%d -> S%d, %!STATUS!",
                                     pDeviceObject,
                                     pDeviceContext->CurrentSystemState - PowerSystemWorking,
                                     pIrpStack->Parameters.Power.State.SystemState - PowerSystemWorking,
                                     ntStatus);

                // Check if lower drivers succeeded the Irp
                ntStatus = pIrp->IoStatus.Status;

                if(NT_SUCCESS(ntStatus))
                {
                    POWER_STATE         newPowerState;
                    BOOLEAN             bPoweringUp = pDeviceContext->CurrentSystemState > pIrpStack->Parameters.Power.State.SystemState;

                    // keep track of the current system state
                    pDeviceContext->CurrentSystemState = pIrpStack->Parameters.Power.State.SystemState;

                    // determine appropriate device state
                    newPowerState.DeviceState = pDeviceContext->DeviceStateMap[ pDeviceContext->CurrentSystemState ];

                    // do a sanity check on the device state
                    if ((newPowerState.DeviceState < PowerDeviceD0) ||
                        (newPowerState.DeviceState > PowerDeviceD3) )
                    {
                        if (pIrpStack->Parameters.Power.State.SystemState == PowerSystemWorking)
                        {
                            newPowerState.DeviceState = PowerDeviceD0;
                        } else
                        {
                            newPowerState.DeviceState = PowerDeviceD3;
                        }
                    }

                    // We need to support fast resume if we're coming back up
                    if( bPoweringUp )
                    {
                        if( newPowerState.DeviceState != pDeviceContext->CurrentDeviceState )
                        {
                            // Go ahead and request the Dx Power IRP
                            PPOWER_DEVICE_CONTEXT pCompletionContext = (PPOWER_DEVICE_CONTEXT)ExAllocatePoolWithTag( NonPagedPoolNx, sizeof(POWER_DEVICE_CONTEXT), 'wPcP' );
                            if( NULL == pCompletionContext )
                            {
                                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                                pIrp->IoStatus.Status = ntStatus;
                                ReleaseRemoveLock( pDeviceContext, pIrp );

                                // Log info.
                                DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                                     PCTRACE_POWER,
                                                     "Device %p, set-system-power error, %!STATUS!",
                                                     pDeviceObject,
                                                     ntStatus);
                            } else
                            {
                                pCompletionContext->pDeviceContext = pDeviceContext;
                                pCompletionContext->pSystemIrp = pIrp;

                                // request a device set power Irp
                                PoRequestPowerIrp(pDeviceContext->PhysicalDeviceObject,
                                                    IRP_MN_SET_POWER,
                                                    newPowerState,
                                                    SystemPowerUpDeviceIrpCompletionRoutine,
                                                    pCompletionContext,
                                                    NULL );

                                // start the next power irp
                                PoStartNextPowerIrp( pIrp );

                                // release the remove lock
                                ReleaseRemoveLock( pDeviceContext, pIrp );
                                ntStatus = STATUS_SUCCESS;
                            }
                        } else
                        {
                            // start the next power irp
                            PoStartNextPowerIrp( pIrp );

                            // release the remove lock
                            ReleaseRemoveLock( pDeviceContext, pIrp );
                        }
                    } else
                    {
                        if( DISPATCH_LEVEL == KeGetCurrentIrql() )
                        {
                            //
                            // At DISPATCH_LEVEL, so we need to queue a work item to change the system state
                            //
                            ntStatus = CallbackEnqueue(
                                            &pDeviceContext->RuntimePowerInfo->pWorkQueueItemSystemPower,
                                            SystemPowerDownWorker,
                                            pDeviceObject,
                                            (PVOID)pIrp,
                                            PASSIVE_LEVEL,
                                            EQCF_DIFFERENT_THREAD_REQUIRED
                                            );
                            if (NT_SUCCESS(ntStatus))
                            {
                                ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
                            } else
                            {
                                //
                                // Since we failed to queue the work item, just update
                                // the Irp's status and release the remove lock.
                                //
                                pIrp->IoStatus.Status = ntStatus;
                                ReleaseRemoveLock( pDeviceContext, pIrp );
                            }
                        } else
                        {
                            //
                            // Just call SystemPowerWorker directly since we're not at DISPATCH_LEVEL.
                            // This call will update the Irp's status and release the remove lock
                            // if necessary.
                            //
                            ntStatus = SystemPowerDownWorkerInternal(pDeviceObject, pIrp);
                        }
                    }
                } else
                {
                    ReleaseRemoveLock( pDeviceContext, pIrp );
                }
            }
            break;

        default:
            break;
    }
    
    // Log power outcome.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p, Irp=%p Power request, %!STATUS!",
                         pDeviceObject,
                         pIrp,
                         ntStatus);

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * DispatchPower()
 *****************************************************************************
 * Handles all the power/ACPI messages from the OS
 *
 */
NTSTATUS
DispatchPower
(
    IN      PDEVICE_OBJECT  pDeviceObject,
    IN      PIRP            pIrp
)
{
    PAGED_CODE();

    ASSERT( pDeviceObject );
    ASSERT( pIrp );

    NTSTATUS ntStatus = STATUS_SUCCESS;

    PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation( pIrp );

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT( pDeviceObject->DeviceExtension );

    ntStatus = PcValidateDeviceContext( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        PoStartNextPowerIrp( pIrp );
        ntStatus = pIrp->IoStatus.Status;
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    ntStatus = AcquireRemoveLock( pDeviceContext, pIrp );
    if(!NT_SUCCESS(ntStatus))
    {
        pIrp->IoStatus.Status = ntStatus;
        PoStartNextPowerIrp( pIrp );
        IoCompleteRequest( pIrp, IO_NO_INCREMENT );
        return ntStatus;
    }

    switch( pIrpStack->MinorFunction )
    {
        case IRP_MN_QUERY_POWER:
            if( DevicePowerState == pIrpStack->Parameters.Power.Type )
            {
                // Device Query Power
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_POWER,
                                     "Device %p, Irp=%p Device QueryPower",
                                     pDeviceObject,
                                     pIrp );

                // Get the new power state
                POWER_STATE NewPowerState = pIrpStack->Parameters.Power.State;

                // Log info.
                DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                     PCTRACE_POWER,
                                     "Device %p, query device power state: D%d -> D%d",
                                     pDeviceObject,
                                     pDeviceContext->CurrentDeviceState - PowerDeviceD0,
                                     NewPowerState.DeviceState - PowerDeviceD0 );
                
                // Simply query the driver if it has registered an interface
                if( pDeviceContext->pAdapterPower )
                {
                    PVOID pPower = NULL;
                    NTSTATUS ntTempStatus = STATUS_SUCCESS;

                    // Query for IAdapterPowerManagement3.
                    ntTempStatus = pDeviceContext->pAdapterPower->QueryInterface
                    (
                        IID_IAdapterPowerManagement3,
                        &pPower
                    );
                    if ( NT_SUCCESS(ntTempStatus) )
                    {
                        // We got an IAdapterPowerManagement3 interface.
                        ntStatus = PADAPTERPOWERMANAGEMENT3( pPower )->QueryPowerChangeState( NewPowerState );
                    }

                    if ( !NT_SUCCESS(ntTempStatus) )
                    {
                        // Query for IAdapterPowerManagement2.
                        ntTempStatus = pDeviceContext->pAdapterPower->QueryInterface
                        (
                            IID_IAdapterPowerManagement2,
                            &pPower
                        );
                        if ( NT_SUCCESS(ntTempStatus) )
                        {
                            // We got an IAdapterPowerManagement2 interface.
                            ntStatus = PADAPTERPOWERMANAGEMENT2( pPower )->QueryPowerChangeState( NewPowerState );
                        }
                    }

                    if ( !NT_SUCCESS(ntTempStatus) )
                    {
                        ntTempStatus = pDeviceContext->pAdapterPower->QueryInterface
                        (
                            IID_IAdapterPowerManagement,
                            &pPower
                        );
                        if ( NT_SUCCESS(ntTempStatus) )
                        {
                            // We got an IAdapterPowerManagement interface.
                            ntStatus = PADAPTERPOWERMANAGEMENT( pPower )->QueryPowerChangeState( NewPowerState );
                        }
                    }

                    if (NULL != pPower)
                    {
                        PUNKNOWN( pPower )->Release();
                        pPower = NULL;
                    }
                }

                if(!NT_SUCCESS(ntStatus))
                {
                    // Log info.
                    DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                         PCTRACE_POWER,
                                         "Device %p, query device power state failure: D%d -> D%d, "
                                         "%!STATUS!",
                                         pDeviceObject,
                                         pDeviceContext->CurrentDeviceState - PowerDeviceD0,
                                         NewPowerState.DeviceState - PowerDeviceD0,
                                         ntStatus );
                    
                    PoStartNextPowerIrp( pIrp );
                    CompleteIrp(pDeviceContext, pIrp, ntStatus);
                } else
                {
                    // mart the Irp as pending
                    IoMarkIrpPending( pIrp );

                    // set up the Irp stack
                    IoCopyCurrentIrpStackLocationToNext( pIrp );

                    // set the completion routine
                    IoSetCompletionRoutine( pIrp,
                                            PowerIrpCompletionRoutine,
                                            pDeviceContext,
                                            TRUE,
                                            TRUE,
                                            TRUE );

                    // pass the Irp down the stack
                    PoCallDriver( pDeviceContext->NextDeviceInStack, pIrp );

                    // return STATUS_PENDING becuase we pended the Irp
                    ntStatus = STATUS_PENDING;
                }
            } else
            {
                // System Query Power
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_POWER,
                                     "Device %p, Irp=%p System QueryPower",
                                     pDeviceObject,
                                     pIrp );
                
                // Log info.
                DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                     PCTRACE_POWER,
                                     "Device %p, query system power state: S%d -> S%d",
                                     pDeviceObject,
                                     pDeviceContext->CurrentSystemState - PowerSystemWorking,
                                     pIrpStack->Parameters.Power.State.SystemState - PowerSystemWorking );

                // mark the Irp as pending
                IoMarkIrpPending( pIrp );

                // set up the Irp stack
                IoCopyCurrentIrpStackLocationToNext( pIrp );

                // set the completion routine
                IoSetCompletionRoutine( pIrp,
                                        PowerIrpCompletionRoutine,
                                        pDeviceContext,
                                        TRUE,
                                        TRUE,
                                        TRUE );

                // pass the Irp down the stack
                PoCallDriver( pDeviceContext->NextDeviceInStack, pIrp );

                // return STATUS_PENDING becuase we pended the Irp
                ntStatus = STATUS_PENDING;
            }
            break;

        case IRP_MN_SET_POWER:
            if( DevicePowerState == pIrpStack->Parameters.Power.Type )
            {
                BOOLEAN relPositionLock = FALSE;
                
                // Device Set Power
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_POWER,
                                     "Device %p, Irp=%p Device SetPower",
                                     pDeviceObject,
                                     pIrp );

                // Acquire the device to sync with creates
                AcquireDevice( pDeviceContext );
                
                // acquire dispatcher locks to drain I/O requests.
                if (pDeviceContext->UsePositionLock)
                {
                    AcquirePositionLock(pDeviceContext);
                    relPositionLock = TRUE;
                }
                

                // Get the new power state
                POWER_STATE NewPowerState = pIrpStack->Parameters.Power.State;
                
                // Log info.
                DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                     PCTRACE_POWER,
                                     "Device %p, device power state: D%d -> D%d",
                                     pDeviceObject,
                                     pDeviceContext->CurrentDeviceState - PowerDeviceD0,
                                     NewPowerState.DeviceState - PowerDeviceD0 );

                PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                               PC_ETYPE_PWR_D0 + (UCHAR)(NewPowerState.DeviceState - PowerDeviceD0),
                               PortCls_PowerState_GUID,
                               (ULONGLONG)pDeviceObject,
                               NULL,
                               0,       // 0 = Request, 1 = Completion
                               GetCurrentD3ExitLatency(pDeviceContext, NewPowerState.DeviceState, pDeviceContext->CurrentSystemState),
                               (ULONGLONG)NULL );

                // Determine whether we are powering up or down
                pDeviceContext->PoweringDown = (pDeviceContext->CurrentDeviceState < NewPowerState.DeviceState)?TRUE:FALSE;

                if( TRUE == pDeviceContext->PoweringDown )
                {
                    // notify the subdevices
                    PowerNotifySubdevices( pDeviceContext, NewPowerState );

                    // change the driver state if it has a registered POWER interface
                    if( pDeviceContext->pAdapterPower )
                    {
                        iPowerChangeState(pDeviceContext, NewPowerState.DeviceState, pDeviceContext->CurrentSystemState);
                    }

                    // notify the power manager
                    PoSetPowerState( pDeviceObject,
                                     DevicePowerState,
                                     NewPowerState );

                    PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                                   PC_ETYPE_PWR_D0 + (UCHAR)(NewPowerState.DeviceState - PowerDeviceD0),
                                   PortCls_PowerState_GUID,
                                   (ULONGLONG)pDeviceObject,
                                   NULL,
                                   1,       // 0 = Request, 1 = Completion
                                   GetCurrentD3ExitLatency(pDeviceContext, NewPowerState.DeviceState, pDeviceContext->CurrentSystemState),
                                   (ULONGLONG)NULL );

                    // keep track of new state
                    pDeviceContext->CurrentDeviceState = NewPowerState.DeviceState;
                }

                // mark the Irp as pending
                IoMarkIrpPending( pIrp );

                // set up the Irp stack
                IoCopyCurrentIrpStackLocationToNext( pIrp );

                // set the completion routine
                IoSetCompletionRoutine( pIrp,
                                        PowerIrpCompletionRoutine,
                                        pDeviceContext,
                                        TRUE,
                                        TRUE,
                                        TRUE );

                // pass the Irp down the stack
                PoCallDriver( pDeviceContext->NextDeviceInStack, pIrp );

                // return STATUS_PENDING because we pended the Irp
                ntStatus = STATUS_PENDING;
                
                // release locks.
                if (relPositionLock)
                {
                    ReleasePositionLock(pDeviceContext);
                    relPositionLock = FALSE;
                }

                // release the device
                ReleaseDevice( pDeviceContext );
            } else
            {
                // System Set Power
                DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                     PCTRACE_POWER,
                                     "Device %p, Irp=%p System SetPower",
                                     pDeviceObject,
                                     pIrp );
                
                // Log info.
                DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                     PCTRACE_POWER,
                                     "Device %p, system power state: S%d -> S%d",
                                     pDeviceObject,
                                     pDeviceContext->CurrentSystemState - PowerSystemWorking,
                                     pIrpStack->Parameters.Power.State.SystemState - PowerSystemWorking );

                // mark the Irp as pending
                IoMarkIrpPending( pIrp );

                // set up the Irp stack
                IoCopyCurrentIrpStackLocationToNext( pIrp );

                // set the completion routine
                IoSetCompletionRoutine( pIrp,
                                        PowerIrpCompletionRoutine,
                                        pDeviceContext,
                                        TRUE,
                                        TRUE,
                                        TRUE );

                // pass the Irp down the stack
                ntStatus = PoCallDriver( pDeviceContext->NextDeviceInStack, pIrp );

                // return STATUS_PENDING because we pended the Irp
                ntStatus = STATUS_PENDING;
            }
            break;

        default:
            // Other power Irp... just pass it down
            ntStatus = ForwardIrpAsynchronous( pDeviceContext, pIrp );
            break;
    }
    
    // Device Query Power
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p, Irp=%p Power request, %!STATUS!",
                         pDeviceObject,
                         pIrp,
                         ntStatus);

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * PcRegisterAdapterPowerManagement()
 *****************************************************************************
 * Register the adapter's power management interface
 * with portcls.  This routine also does a QI for a shutdown notification
 * interface.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcRegisterAdapterPowerManagement
(
    __in    PUNKNOWN    Unknown,
    __in    PVOID       pvContext1
)
{
    PAGED_CODE();

    ASSERT(pvContext1);
    ASSERT(Unknown);

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[PcRegisterAdapterPowerManagement]" );

    //
    // Validate Parameters.
    //
    if (NULL == pvContext1 ||
        NULL == Unknown)
    {
        return STATUS_INVALID_PARAMETER;
    }

    NTSTATUS        ntStatus        = STATUS_UNSUCCESSFUL;
    PDEVICE_OBJECT  pDeviceObject   = PDEVICE_OBJECT(pvContext1);
    PDEVICE_CONTEXT pDeviceContext  = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT( pDeviceContext );

    //
    // Validate DeviceContext.
    //
    if (NULL == pDeviceContext)
    {
        return STATUS_INVALID_PARAMETER;
    }

    // Make sure this is really a supported
    // interface (Note: We have to release
    // it when the device is closed/stopped )
    PVOID pResult = NULL;

    // Check for IAdapterPowerManagement3
    ntStatus = Unknown->QueryInterface
    (
        IID_IAdapterPowerManagement3,
        &pResult
    );

    if( NT_SUCCESS(ntStatus) )
    {
        // Store the interface for later use.
        pDeviceContext->pAdapterPower = PUNKNOWN( pResult );

        return ntStatus;
    }

    // Check for IAdapterPowerManagement2
    pResult = NULL;
    ntStatus = Unknown->QueryInterface
    (
        IID_IAdapterPowerManagement2,
        &pResult
    );

    if( NT_SUCCESS(ntStatus) )
    {
        // Store the interface for later use.
        pDeviceContext->pAdapterPower = PUNKNOWN( pResult );

        return ntStatus;
    }

    // Check for IAdapterPowerManagement
    pResult = NULL;
    ntStatus = Unknown->QueryInterface
    (
        IID_IAdapterPowerManagement,
        &pResult
    );

    if( NT_SUCCESS(ntStatus) )
    {
        // Store the interface for later use.
        pDeviceContext->pAdapterPower = PUNKNOWN( pResult );

        return ntStatus;
    } 
    
    pDeviceContext->pAdapterPower = 0;

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * PcUnregisterAdapterPowerManagement()
 *****************************************************************************
 * Unregister the adapter's power management interface
 * with portcls.  
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcUnregisterAdapterPowerManagement
(
    __in    PDEVICE_OBJECT  pDeviceObject
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[PcUnregisterAdapterPowerManagement]" );

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject)
    {
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT pDeviceContext  = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    ASSERT( pDeviceContext );

    //
    // Validate DeviceContext.
    //
    if (NULL == pDeviceContext)
    {
        return STATUS_INVALID_PARAMETER;
    }

    // Ensure that there's actually an 
    // IAdapterPowerManagement interface 
    // to release.
    if ((NULL == pDeviceContext->pAdapterPower))
    {
        return STATUS_INVALID_DEVICE_STATE;
    }

    pDeviceContext->pAdapterPower->Release();
    pDeviceContext->pAdapterPower = NULL;

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * PowerNotifySubdevices()
 *****************************************************************************
 * Called by ProcessPowerIrp to notify the device's subdevices of a power
 * state change.
 *
 */
void
PowerNotifySubdevices
(
    IN  PDEVICE_CONTEXT     pDeviceContext,
    IN  POWER_STATE         PowerState
)
{
    PAGED_CODE();

    ASSERT(pDeviceContext);

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[PowerNotifySubdevices]" );

    // only notify the subdevices if we're started and if there are subdevices
    if (pDeviceContext->DeviceStopState == DeviceStarted)
    {
        PKSOBJECT_CREATE_ITEM createItem = pDeviceContext->CreateItems;

        // iterate through the subdevices
        for( ULONG index=0; index < pDeviceContext->MaxObjects; index++,createItem++)
        {
            if( createItem && (createItem->Create) )
            {
                PSUBDEVICE subDevice = PSUBDEVICE( createItem->Context );

                if( subDevice )
                {
                    // notify the subdevice
                    subDevice->PowerChangeNotify( PowerState );
                }
            }
        }
    }
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * UpdateActivePinCount()
 *****************************************************************************
 *
 */
NTSTATUS
UpdateActivePinCount
(
    IN  PDEVICE_CONTEXT     DeviceContext,
    IN  BOOL                Increment
)
{
    PAGED_CODE();

    ASSERT(DeviceContext);

    ULONG       ActivePinCount;
    NTSTATUS    ntStatus = STATUS_SUCCESS;

    // adjust the active pin count
    if( Increment )
    {
        ActivePinCount = InterlockedIncrement( PLONG(&DeviceContext->ActivePinCount) );

        if ( 1 == ActivePinCount )
        {
            // register the system state as busy
            if( DeviceContext->SystemStateHandle )
            {
                PoSetPowerRequest( DeviceContext->SystemStateHandle, PowerRequestSystemRequired );
            }

            // Let runtime power management know we're active
            ActivateRuntimePowerComponent( DeviceContext, TRUE );
        }
    } else
    {
        ActivePinCount = InterlockedDecrement( PLONG(&DeviceContext->ActivePinCount) );

        if( 0 == ActivePinCount )
        {
            // register the system state as not busy
            if( DeviceContext->SystemStateHandle )
            {
                PoClearPowerRequest( DeviceContext->SystemStateHandle, PowerRequestSystemRequired );
            }

            // Let runtime power management know we're idle
            IdleRuntimePowerComponent( DeviceContext, TRUE );
        }
    }

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[UpdateActivePinCount] %d",
                         ActivePinCount );

    return ntStatus;
}

/*****************************************************************************
 * GetUlongFromRegistry()
 *****************************************************************************
 *
 */
#pragma code_seg("PAGE")
NTSTATUS
GetUlongFromRegistry
(
    _In_  PDEVICE_CONTEXT   DeviceContext,
    _In_  ULONG             DevInstKeyType,
    _In_  PUNICODE_STRING   SubKeyName,
    _In_  PUNICODE_STRING   ValueName,
    _Out_ PULONG            Value
)
{
    PAGED_CODE();

    ASSERT(DeviceContext);
    ASSERT(Value);

    NTSTATUS            status;
    OBJECT_ATTRIBUTES   attributes = {0};
    HANDLE              rootKey = NULL;
    HANDLE              subKey = NULL;
    BOOLEAN             closeRootKey = FALSE;
    BOOLEAN             closeSubKey = FALSE;
    ULONG               bytesReturned = 0;
    PVOID               keyData = NULL;

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "PDO %p, DevInstKeyType %d, Key Name %wZ, Value Name %wZ",
                         DeviceContext->PhysicalDeviceObject, 
                         DevInstKeyType,
                         SubKeyName,
                         ValueName);

    // store default values in return parms
    *Value = 0;

    // open the driver registry key
    status = IoOpenDeviceRegistryKey( DeviceContext->PhysicalDeviceObject,
                                      DevInstKeyType,
                                      KEY_READ,
                                      &rootKey );
    if(!NT_SUCCESS(status))
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "Failed to open the DevInstKeyType %d key for PDO %p, %!STATUS!", 
                              DevInstKeyType, DeviceContext->PhysicalDeviceObject, status);
        goto Done;
    }

    closeRootKey = TRUE;

    // init the power settings key object attributes
    InitializeObjectAttributes( &attributes,
                                SubKeyName,
                                OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                rootKey,
                                NULL );

    // open the power settings key
    status = ZwOpenKey(&subKey, KEY_READ, &attributes);
    if(!NT_SUCCESS(status))
    {
        //
        // Note: this is not an error b/c these values are optional...
        //
        DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                             PCTRACE_POWER,
                             "DevInstKeyType %d, %wZ key is not present for PDO %p, %!STATUS!", 
                              DevInstKeyType, SubKeyName, DeviceContext->PhysicalDeviceObject, status);
        goto Done;
    }


    closeSubKey = TRUE;

    
    // allocate a buffer to hold the query
    keyData = ExAllocatePoolWithTag(PagedPool,
                                    sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                                    'dKcP' ); //  'PcKd'
    if(NULL == keyData)
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "Failed to alloc buffer, STATUS_INSUFFICIENT_RESOURCES");
        goto Done;
    }

    
    // get the immediate idle setting
    status = ZwQueryValueKey( subKey,
                              ValueName,
                              KeyValuePartialInformation,
                              keyData,
                              sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                              &bytesReturned );
    if(!NT_SUCCESS(status))
    {
        //
        // Note: this is not an error b/c these values are optional...
        //
        DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                             PCTRACE_POWER,
                             "DevInstKeyType %d, %wZ\\%wZ value is not present for PDO %p, %!STATUS!", 
                              DevInstKeyType, SubKeyName, ValueName, DeviceContext->PhysicalDeviceObject, status);
        goto Done;
    }

    PKEY_VALUE_PARTIAL_INFORMATION partialInfo = PKEY_VALUE_PARTIAL_INFORMATION(keyData);

    if(partialInfo->DataLength == sizeof(DWORD))
    {
        // set the return value
        *Value = *(PDWORD(partialInfo->Data));
    }

Done:

    if (keyData)
    {
        ExFreePool(keyData);
        keyData = NULL;
    }
    
    if(closeSubKey)
    {
        ZwClose(subKey);
        subKey = NULL;
        closeSubKey = FALSE;
    }

    if (closeRootKey)
    {
        ZwClose(rootKey);
        rootKey = NULL;
        closeRootKey = FALSE;
    }

    
    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_POWER,
                         "PDO %p, DevInstKeyType %d, %wZ\\%wZ value is %d, %!STATUS!", 
                          DeviceContext->PhysicalDeviceObject, DevInstKeyType, SubKeyName, ValueName, *Value, status);
    
    return status;
}

/*****************************************************************************
 * GetLockInfoFromRegistry()
 *****************************************************************************
 *
 */
#pragma code_seg("PAGE")
NTSTATUS
GetLockInfoFromRegistry
(
    _In_  PDEVICE_CONTEXT   DeviceContext
)
{
    PAGED_CODE();

    ASSERT(DeviceContext);

    NTSTATUS        status;
    UNICODE_STRING  pnpSettingsKeyName;
    UNICODE_STRING  valueName;
    ULONG           value;

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_INIT,
                         "PDO %p",
                         DeviceContext->PhysicalDeviceObject);

    RtlInitUnicodeString(&pnpSettingsKeyName, L"DispatchSettings");

    // 
    // Get the DispatchSettings\UsePositionLock value.
    //
    RtlInitUnicodeString(&valueName, L"UsePositionLock");
    value = 0;
    status = GetUlongFromRegistry(DeviceContext,
                                  PLUGPLAY_REGKEY_DRIVER,
                                  &pnpSettingsKeyName,
                                  &valueName,
                                  &value);
    if (NT_SUCCESS(status))
    {
        DeviceContext->UsePositionLock = value > 0 ? 1 : 0;
    }

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * GetIdleInfoFromRegistry()
 *****************************************************************************
 *
 */
NTSTATUS
GetIdleInfoFromRegistry
(
    IN  PDEVICE_CONTEXT     DeviceContext,
    OUT PULONG              ConservationIdleTime,
    OUT PULONG              PerformanceIdleTime,
    OUT PDEVICE_POWER_STATE IdleDeviceState,
    OUT PBOOLEAN            ImmediateIdle
)
{
    PAGED_CODE();

    ASSERT(DeviceContext);
    ASSERT(ConservationIdleTime);
    ASSERT(PerformanceIdleTime);
    ASSERT(IdleDeviceState);

    NTSTATUS ntStatus;
    HANDLE DriverRegistryKey;
    HANDLE PowerSettingsKey;

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[GetIdleInfoFromRegistry]" );

    // store default values in return parms
    *ConservationIdleTime = DEFAULT_CONSERVATION_IDLE_TIME;
    *PerformanceIdleTime = DEFAULT_PERFORMANCE_IDLE_TIME;
    *IdleDeviceState = DEFAULT_IDLE_DEVICE_POWER_STATE;
    *ImmediateIdle = DEFAULT_IMMEDIATE_IDLE_STATE;

    // open the driver registry key
    ntStatus = IoOpenDeviceRegistryKey( DeviceContext->PhysicalDeviceObject,
                                        PLUGPLAY_REGKEY_DRIVER,
                                        KEY_READ,
                                        &DriverRegistryKey );
    if(NT_SUCCESS(ntStatus))
    {
        OBJECT_ATTRIBUTES PowerSettingsAttributes;
        UNICODE_STRING PowerSettingsKeyName;

        // init the power settings key name
        RtlInitUnicodeString( &PowerSettingsKeyName, L"PowerSettings" );

        // init the power settings key object attributes
        InitializeObjectAttributes( &PowerSettingsAttributes,
                                    &PowerSettingsKeyName,
                                    OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                    DriverRegistryKey,
                                    NULL );

        // open the power settings key
        ntStatus = ZwOpenKey( &PowerSettingsKey,
                              KEY_READ,
                              &PowerSettingsAttributes );
        if(NT_SUCCESS(ntStatus))
        {
            UNICODE_STRING ConservationKey,PerformanceKey,IdleStateKey,ImmediateIdleKey;
            ULONG BytesReturned;

            // init the key names
            RtlInitUnicodeString( &ConservationKey, L"ConservationIdleTime" );
            RtlInitUnicodeString( &PerformanceKey, L"PerformanceIdleTime" );
            RtlInitUnicodeString( &IdleStateKey, L"IdlePowerState" );
            RtlInitUnicodeString( &ImmediateIdleKey, L"ImmediateIdle" );

            // allocate a buffer to hold the query
            PVOID KeyData = ExAllocatePoolWithTag(PagedPool,
                                                  sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                                                  'dKcP' ); //  'PcKd'
            if( NULL != KeyData )
            {
                // get the conservation idle time
                ntStatus = ZwQueryValueKey( PowerSettingsKey,
                                            &ConservationKey,
                                            KeyValuePartialInformation,
                                            KeyData,
                                            sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                                            &BytesReturned );
                if(NT_SUCCESS(ntStatus))
                {
                    PKEY_VALUE_PARTIAL_INFORMATION PartialInfo = PKEY_VALUE_PARTIAL_INFORMATION(KeyData);

                    if(PartialInfo->DataLength == sizeof(DWORD))
                    {
                        // set the return value
                        *ConservationIdleTime = *(PDWORD(PartialInfo->Data));
                    }
                }

                // get the performance idle time
                ntStatus = ZwQueryValueKey( PowerSettingsKey,
                                            &PerformanceKey,
                                            KeyValuePartialInformation,
                                            KeyData,
                                            sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                                            &BytesReturned );
                if(NT_SUCCESS(ntStatus))
                {
                    PKEY_VALUE_PARTIAL_INFORMATION PartialInfo = PKEY_VALUE_PARTIAL_INFORMATION(KeyData);

                    if(PartialInfo->DataLength == sizeof(DWORD))
                    {
                        // set the return value
                        *PerformanceIdleTime = *(PDWORD(PartialInfo->Data));
                    }
                }

                // get the device idle state
                ntStatus = ZwQueryValueKey( PowerSettingsKey,
                                            &IdleStateKey,
                                            KeyValuePartialInformation,
                                            KeyData,
                                            sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                                            &BytesReturned );
                if(NT_SUCCESS(ntStatus))
                {
                    PKEY_VALUE_PARTIAL_INFORMATION PartialInfo = PKEY_VALUE_PARTIAL_INFORMATION(KeyData);

                    if(PartialInfo->DataLength == sizeof(DWORD))
                    {
                        // determine the return value
                        switch( *(PDWORD(PartialInfo->Data)) )
                        {
                            case 3:
                                *IdleDeviceState = PowerDeviceD3;
                                break;

                            case 2:
                                *IdleDeviceState = PowerDeviceD2;
                                break;

                            case 1:
                                *IdleDeviceState = PowerDeviceD1;
                                break;

                            default:
                                *IdleDeviceState = PowerDeviceD3;
                                break;
                        }
                    }
                }

                // get the immediate idle setting
                ntStatus = ZwQueryValueKey( PowerSettingsKey,
                                            &ImmediateIdleKey,
                                            KeyValuePartialInformation,
                                            KeyData,
                                            sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                                            &BytesReturned );
                if(NT_SUCCESS(ntStatus))
                {
                    PKEY_VALUE_PARTIAL_INFORMATION PartialInfo = PKEY_VALUE_PARTIAL_INFORMATION(KeyData);

                    if(PartialInfo->DataLength == sizeof(DWORD))
                    {
                        // set the return value
                        *ImmediateIdle = *(PDWORD(PartialInfo->Data)) != 0;
                    }
                }

                // free the key info buffer
                ExFreePool( KeyData );
            }

            // close the power settings key
            ZwClose( PowerSettingsKey );
        }

        // close the driver registry key
        ZwClose( DriverRegistryKey );
    }

    // always succeed since we return either the registry value(s) or the defaults
    return STATUS_SUCCESS;
}

/*****************************************************************************
 * GetFxIdleInfoFromRegistry()
 *****************************************************************************
 *
 */
#pragma code_seg("PAGE")
NTSTATUS
GetFxIdleInfoFromRegistry
(
    _In_  PDEVICE_CONTEXT   DeviceContext,
    _Out_ PBOOLEAN          SingleComponentMultiFxStates
)
{
    PAGED_CODE();

    ASSERT(DeviceContext);
    ASSERT(SingleComponentMultiFxStates);

    NTSTATUS    ntStatus;
    HANDLE      driverRegistryKey;
    HANDLE      powerSettingsKey;

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[GetFxIdleInfoFromRegistry] PDO %p",
                         DeviceContext->PhysicalDeviceObject);

    // store default values in return parms
    *SingleComponentMultiFxStates = DEFAULT_SINGLE_COMPONENT_MULTI_FX_STATES;

    // open the driver registry key
    ntStatus = IoOpenDeviceRegistryKey( DeviceContext->PhysicalDeviceObject,
                                        PLUGPLAY_REGKEY_DRIVER,
                                        KEY_READ,
                                        &driverRegistryKey );
    if(NT_SUCCESS(ntStatus))
    {
        OBJECT_ATTRIBUTES   powerSettingsAttributes;
        UNICODE_STRING      powerSettingsKeyName;

        // init the power settings key name
        RtlInitUnicodeString( &powerSettingsKeyName, L"PowerSettings" );

        // init the power settings key object attributes
        InitializeObjectAttributes( &powerSettingsAttributes,
                                    &powerSettingsKeyName,
                                    OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                    driverRegistryKey,
                                    NULL );

        // open the power settings key
        ntStatus = ZwOpenKey( &powerSettingsKey,
                              KEY_READ,
                              &powerSettingsAttributes );
        
        if(NT_SUCCESS(ntStatus))
        {
            UNICODE_STRING  singleComponentMultiFxStatesName;
            ULONG           bytesReturned;

            // init the key names
            RtlInitUnicodeString( &singleComponentMultiFxStatesName, L"SingleComponentMultiFxStates" );

            // allocate a buffer to hold the query
            PVOID keyData = ExAllocatePoolWithTag(PagedPool,
                                                  sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                                                  'dKcP' ); //  'PcKd'
            if( NULL != keyData )
            {
                // get the immediate idle setting
                ntStatus = ZwQueryValueKey( powerSettingsKey,
                                            &singleComponentMultiFxStatesName,
                                            KeyValuePartialInformation,
                                            keyData,
                                            sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD),
                                            &bytesReturned );
                if(NT_SUCCESS(ntStatus))
                {
                    PKEY_VALUE_PARTIAL_INFORMATION partialInfo = PKEY_VALUE_PARTIAL_INFORMATION(keyData);

                    if(partialInfo->DataLength == sizeof(DWORD))
                    {
                        // set the return value
                        *SingleComponentMultiFxStates = *(PDWORD(partialInfo->Data)) != 0;
                    }
                }
                else
                {
                    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                         PCTRACE_POWER,
                                         "PDO %p, SingleComponentMultiFxStates value is not present, %!STATUS!", 
                                         DeviceContext->PhysicalDeviceObject, ntStatus);
                }

                // free the key info buffer
                ExFreePool(keyData);
            }
            else 
            {
                DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                     PCTRACE_POWER,
                                     "Failed to alloc buffer for SingleComponentMultiFxStates value, STATUS_INSUFFICIENT_RESOURCES");
            }

            // close the power settings key
            ZwClose(powerSettingsKey);
        }
        else 
        {
            DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                                 PCTRACE_POWER,
                                 "PLUGPLAY_REGKEY_DRIVER\\PowerSettings key is not present for PDO %p, %!STATUS!", 
                                  DeviceContext->PhysicalDeviceObject, ntStatus);
        }

        // close the driver registry key
        ZwClose(driverRegistryKey);
    }
    else
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "Failed to open the PLUGPLAY_REGKEY_DRIVER key for PDO %p, %!STATUS!", 
                              DeviceContext->PhysicalDeviceObject, ntStatus);
    }
    
    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_POWER,
                         "PDO %p, SingleComponentMultiFxStates is %s",
                         DeviceContext->PhysicalDeviceObject, (*SingleComponentMultiFxStates) ? "On" : "Off");

    
    // always succeed since we return either the registry value(s) or the default(s)
    return STATUS_SUCCESS;
}

#pragma code_seg()
/*****************************************************************************
 * RequestPowerIrpCompletionRoutine
 *****************************************************************************
 * Completion routine for device power irps from PoRequestPowerIrp
 *
 */
VOID
RequestPowerIrpCompletionRoutine
(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      UCHAR               MinorFunction,
    IN      POWER_STATE         PowerState,
    IN      PVOID               pContext,
    IN      PIO_STATUS_BLOCK    pIoStatus
)
{
    ASSERT(pDeviceObject);
    ASSERT(pIoStatus);
    ASSERT(pContext);

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[RequestPowerIrpCompletionRoutine]" );

    POWER_REQUEST_CONTEXT* pPowerRequestContext = (POWER_REQUEST_CONTEXT*)pContext;

    if(pPowerRequestContext != NULL)
    {
        pPowerRequestContext->Status = pIoStatus->Status;

        KeSetEvent( &pPowerRequestContext->SyncEvent,
                    0,
                    FALSE );
    }
}

#pragma code_seg()
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
)
{
    ASSERT(pDeviceObject);

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "[PcRequestNewPowerState] %d",
                         RequestedNewState );

    //
    // Validate Parameters.
    //
    if (NULL == pDeviceObject)
    {
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Validate DeviceContext.
    //
    if (NULL == pDeviceContext)
    {
        return STATUS_INVALID_PARAMETER;
    }

    // check the new power state
    if ((RequestedNewState < PowerDeviceD0) || (RequestedNewState > PowerDeviceD3))
    {
        return STATUS_INVALID_PARAMETER;
    }

    // check if this is actually a state change
    if( RequestedNewState != pDeviceContext->CurrentDeviceState )
    {
        POWER_STATE NewPowerState;
        POWER_REQUEST_CONTEXT PowerRequestContext;

        //
        // If the caller wants to exit D0 but the device is currently active,
        // fail the request.
        //
        if (RequestedNewState > PowerDeviceD0 && 
            pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted &&
            pDeviceContext->RuntimePowerInfo->DeviceActive)
        {
            return STATUS_INVALID_DEVICE_STATE;
        }

        NewPowerState.DeviceState = RequestedNewState;

        KeInitializeEvent( &PowerRequestContext.SyncEvent,
                           SynchronizationEvent,
                           FALSE );

        //
        // Acquire the device power request spin lock
        //
        KIRQL oldIrql;
        KeAcquireSpinLock( &pDeviceContext->RuntimePowerInfo->PowerStateLock, &oldIrql );

        ntStatus = PoRequestPowerIrp( pDeviceObject,
                                      IRP_MN_SET_POWER,
                                      NewPowerState,
                                      RequestPowerIrpCompletionRoutine,
                                      &PowerRequestContext,
                                      NULL );

        //
        // Release the device power request spinlock
        //
        KeReleaseSpinLock( &pDeviceContext->RuntimePowerInfo->PowerStateLock, oldIrql );

        // Did this get allocated and sent??
        //
        if( NT_SUCCESS(ntStatus) )
        {
            // Wait for the completion event
            KeWaitForSingleObject( &PowerRequestContext.SyncEvent,
                                   Suspended,
                                   KernelMode,
                                   FALSE,
                                   NULL );

            ntStatus = PowerRequestContext.Status;
        }
    }

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * CheckCurrentPowerState()
 *****************************************************************************
 * This routine resets the idle timer and checks to see if the device is
 * current in the D0 (full power) state.  If it isn't, it requests that the
 * device power up to D0.
 */
NTSTATUS
CheckCurrentPowerState
(
    IN  PDEVICE_OBJECT      pDeviceObject
)
{
    PAGED_CODE();

    ASSERT(pDeviceObject);

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    // reset the idle timer
    PULONG pulIdleTimer = pDeviceContext->IdleTimer;
    if( pulIdleTimer )
    {
        PoSetDeviceBusy( pulIdleTimer );
    }

    // check if we're in not in PowerDeviceD0...
    if ( pDeviceContext->CurrentDeviceState != PowerDeviceD0 )
    {
        // ...and the system is in a working state
        if ( pDeviceContext->CurrentSystemState == PowerSystemWorking )
        {
            //
            // Get PO to send a power request to this device stack to put it
            // back into the D0 state.
            //
            POWER_STATE newPowerState;

            newPowerState.DeviceState = PowerDeviceD0;

            PoRequestPowerIrp( pDeviceContext->PhysicalDeviceObject,
                               IRP_MN_SET_POWER,
                               newPowerState,
                               NULL,
                               NULL,
                               NULL );
        }

        //
        // The Irp requested above might not have been
        // completed yet.  Return an error code so that GetIrpDisposition puts
        // the irp on the queue.
        //
        ntStatus = STATUS_DEVICE_NOT_READY;
    }

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * PcLowPowerEpochCallback()
 *****************************************************************************
 * Callback invoked whenever the low power epoch status changes.  We store the 
 * new value and inform the miniport of this change if it supports 
 * IAdapterPowerManagement3.
 */
__drv_functionClass(POWER_SETTING_CALLBACK)
_IRQL_requires_same_
NTSTATUS 
PcLowPowerEpochCallback(
    _In_                            LPCGUID SettingGuid,
    _In_reads_bytes_(ValueLength)   PVOID   Value,
    _In_                            ULONG   ValueLength,
    _Inout_opt_                     PVOID   Context
    )
{
    PAGED_CODE();

    //
    // The context is our device object.
    //
    PDEVICE_OBJECT pDeviceObject = (PDEVICE_OBJECT)Context;
    ASSERT(pDeviceObject);

    //
    // If the device object isn't valid, just return.
    //
    if ( ( NULL == pDeviceObject ) || ( NULL == pDeviceObject->DeviceExtension ) )
    {
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);

    //
    // If the runtime power info isn't valid, just return.
    //
    if ( NULL == pDeviceContext->RuntimePowerInfo )
    {
        return STATUS_SUCCESS;
    }

    //
    // See if the Low Power Epoch setting has changed.
    //
    if ( InlineIsEqualGUID(*SettingGuid, GUID_LOW_POWER_EPOCH) && (sizeof(BOOL) == ValueLength) )
    {
        PADAPTERPOWERMANAGEMENT3    pAdapterPower = NULL;
        NTSTATUS                    ntStatus = STATUS_SUCCESS;

        //
        // Generate an ETW event indicating that we just received a 
        // low power epoch notification.
        //
        PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                       PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
                       PortCls_PowerPolicy_GUID,
                       (ULONGLONG) pDeviceObject,
                       (ULONGLONG) (*((BOOL*)Value)),
                       0,       // 0 = Beginning, 1 = Completed
                       0,
                       0);

        //
        // Low Power Epoch setting has changed.  Store the new value.
        //
        pDeviceContext->RuntimePowerInfo->LowPowerEpoch = *((BOOL*)Value);

        //
        // If the miniport has registered a power management interface,
        // see if it's IAdapaterPowerManagement3, and call 
        // D3ExitLatencyChanged only if the device is in D3.
        //
        if ( pDeviceContext->pAdapterPower )
        {
            ntStatus = pDeviceContext->pAdapterPower->QueryInterface
            (
                IID_IAdapterPowerManagement3,
                (PVOID*)(&pAdapterPower)
            );
            if ( NT_SUCCESS(ntStatus) && PowerDeviceD3 == pDeviceContext->CurrentDeviceState )
            {
                //
                // Compute the new D3 exit latency
                //
                PC_EXIT_LATENCY NewD3ExitLatency = GetCurrentD3ExitLatency
                (
                    pDeviceContext, 
                    pDeviceContext->CurrentDeviceState, 
                    pDeviceContext->CurrentSystemState
                );

                //
                // Generate an ETW event indicating that we're about to 
                // tell the miniport our new D3 exit latency.
                //
                PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                               PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
                               PortCls_PowerPolicy_GUID,
                               (ULONGLONG) pDeviceObject,
                               NewD3ExitLatency,
                               0,       // 0 = Beginning, 1 = Completed
                               0,
                               0);

                //
                // Call the miniport.  If the call fails, we don't do anything for now.
                //
                (void) pAdapterPower->D3ExitLatencyChanged( NewD3ExitLatency );

                //
                // Generate an ETW event indicating that we've completed
                // telling the miniport about our new D3 exit latency.
                //
                PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                               PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE,
                               PortCls_PowerPolicy_GUID,
                               (ULONGLONG) pDeviceObject,
                               NewD3ExitLatency,
                               1,       // 0 = Beginning, 1 = Completed
                               0,
                               0);
            }

            if ( NULL != pAdapterPower )
            {
                pAdapterPower->Release();
            }
        }

        //
        // If the runtime power framework is currently managing our device
        // let's go ahead and call the framework to update our idle timeout.
        //
        if (pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted)
        {
            SetRuntimePowerIdleTimeout( pDeviceContext, GetCurrentRuntimeIdleTimeout( pDeviceContext ) );
        }

        //
        // Generate an ETW event indicating that we just completed
        // handling a low power epoch notification.
        //
        PcWmiLogEvent( TRACE_LEVEL_INFORMATION,
                       PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION,
                       PortCls_PowerPolicy_GUID,
                       (ULONGLONG) pDeviceObject,
                       (ULONGLONG) (*((BOOL*)Value)),
                       1,       // 0 = Beginning, 1 = Completed
                       0,
                       0);
    }

    return STATUS_SUCCESS;
}
