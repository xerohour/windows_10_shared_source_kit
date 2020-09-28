/*****************************************************************************
 * runtime.cpp - WDM Streaming port class driver
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 *
 * This file contains code related to runtime power management
 * for the audio adpaters/miniports
 */

#include "private.h"

#include "pcwpptr.h"
#include "runtime.tmh"

REQUEST_POWER_COMPLETE  RuntimeDevicePowerUpIrpCompletionRoutine;

/*****************************************************************************
 * IsPortClsRuntimePowerInterfaceEnabled()
 *****************************************************************************
 * Helper function to determine if the default single component/single state 
 * power interface is enabled. Default is yes.
 */
#pragma code_seg("PAGE")
BOOLEAN 
IsPortClsRuntimePowerInterfaceEnabled(
    _In_ PDEVICE_OBJECT DeviceObject
)
{
    BOOLEAN         multiFxState    = FALSE;
    PDEVICE_CONTEXT deviceCtx       = NULL;
    
    PAGED_CODE();
    
    if (NULL == DeviceObject)
    {
        goto Done;
    }

    deviceCtx = PDEVICE_CONTEXT(DeviceObject->DeviceExtension);
    if (NULL == deviceCtx)
    {
        goto Done;
    }

    if (deviceCtx->RuntimePowerInfo == NULL ||
        deviceCtx->RuntimePowerInfo->SingleComponentMultiFxStates == FALSE)
    {
        goto Done;
    }

    multiFxState = TRUE;
    
Done:
    //
    // Turn on the legacy interface if single-component/multi-state is not supported.
    //
    return multiFxState != TRUE;
}

/*****************************************************************************
 * PcAssignPowerFrameworkSettings()
 *****************************************************************************
 * Init client device's PoFx settings.
 * This API is only allowed if there is an override in the registry for this
 * device (SingleComponentMultiFxStates setting). That value is set via INF during
 * installation.
 */
#pragma code_seg("PAGE")
PORTCLASSAPI
NTSTATUS
NTAPI
PcAssignPowerFrameworkSettings(
  _In_ PDEVICE_OBJECT               DeviceObject,
  _In_ PPC_POWER_FRAMEWORK_SETTINGS PowerFrameworkSettings
  )
{
    NTSTATUS                    status = STATUS_SUCCESS;
    PDEVICE_CONTEXT             deviceCtx = NULL;
    PPO_FX_COMPONENT_IDLE_STATE idleStates = NULL;
    ULONG                       idleStatesSize = 0;
    PPO_FX_COMPONENT            component = NULL;
    ULONG                       componentSize = 0;
    BYTE *                      buffer = NULL;
        
    PAGED_CODE();

    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p, PowerFrameworkSettings %p",
                         DeviceObject, PowerFrameworkSettings);        
    //
    // Validate parameters.
    //
    if (NULL == DeviceObject || NULL == PowerFrameworkSettings)
    {
        status = STATUS_INVALID_PARAMETER;
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "A null argument was detected, Device %p, PowerFrameworkSettings %p, %!STATUS!",
                             DeviceObject, PowerFrameworkSettings, status);
        goto Done;
    }

    deviceCtx = PDEVICE_CONTEXT(DeviceObject->DeviceExtension);
    if (NULL == deviceCtx)
    {
        status = STATUS_INVALID_PARAMETER;
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "The device's context is null, Device %p, %!STATUS!",
                             DeviceObject, status);
        goto Done;
    }

    //
    // Make sure the driver is allowed to make this call.
    //
    if (deviceCtx->RuntimePowerInfo == NULL ||
        deviceCtx->RuntimePowerInfo->SingleComponentMultiFxStates == FALSE)
    {
        status = STATUS_DEVICE_CONFIGURATION_ERROR;
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "Device %p, the SingleComponentMultiFxStates (INF) flag is not set, %!STATUS!",
                             DeviceObject, status);
        goto Done;
    }
    
    //
    // Make sure the driver is not calling this API multiple times.
    //
    if (deviceCtx->RuntimePowerInfo->ScMs.EvtPcPostPoFxRegisterDevice != NULL ||
        deviceCtx->RuntimePowerInfo->ScMs.EvtPcPrePoFxUnregisterDevice != NULL || 
        deviceCtx->RuntimePowerInfo->ScMs.ComponentActiveConditionCallback != NULL || 
        deviceCtx->RuntimePowerInfo->ScMs.ComponentIdleConditionCallback != NULL || 
        deviceCtx->RuntimePowerInfo->ScMs.ComponentIdleStateCallback != NULL || 
        deviceCtx->RuntimePowerInfo->ScMs.PowerControlCallback != NULL ||
        deviceCtx->RuntimePowerInfo->pRuntimePowerFrameworkHandle != NULL)
    {
        status = STATUS_DEVICE_CONFIGURATION_ERROR;
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "Device %p, PcAssignPowerFrameworkSettings cannot be called multiple times, %!STATUS!",
                             DeviceObject, status);
        goto Done;
    }
    
    //
    // Validate the power struct size.
    //
    if (PowerFrameworkSettings->Size != sizeof(PC_POWER_FRAMEWORK_SETTINGS))
    {
        status = STATUS_INFO_LENGTH_MISMATCH;
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "Device %p expected PowerFrameworkSettings size %d, actual %d, %!STATUS!",
                             DeviceObject,
                             sizeof(PC_POWER_FRAMEWORK_SETTINGS), 
                             PowerFrameworkSettings->Size,
                             status);
        goto Done;
    }

    //
    // If settings for component 0 are specified, make sure it contains at least
    // one F-state.
    //
    if (NULL != PowerFrameworkSettings->Component) 
    {
        if (0 == PowerFrameworkSettings->Component->IdleStateCount) 
        {
            status = STATUS_INVALID_PARAMETER;
            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                 PCTRACE_POWER,
                                 "Device %p Component settings are specified but "
                                  "IdleStateCount is 0. %!STATUS!", 
                                  DeviceObject, status);
            goto Done;
        }

        if (NULL == PowerFrameworkSettings->Component->IdleStates) 
        {
            status = STATUS_INVALID_PARAMETER;
            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                 PCTRACE_POWER,
                                 "Device %p Component settings are specified but IdleStates is NULL. %!STATUS!", 
                                 DeviceObject, status);
            goto Done;
        }
    }

    //
    // Compute buffer size needed for storing component information
    // (including F-states)
    //
    if (NULL != PowerFrameworkSettings->Component) 
    {
        //
        // Compute buffer size needed for storing F-states
        //
        status = RtlULongMult(
                    PowerFrameworkSettings->Component->IdleStateCount,
                    sizeof(*(PowerFrameworkSettings->Component->IdleStates)), 
                    &idleStatesSize
                    );
        
        if (!NT_SUCCESS(status)) 
        {
            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                 PCTRACE_POWER,
                                 "Device %p unable to compute length of buffer "
                                 "required to store F-states. RtlULongMult failed with %!STATUS!",
                                 DeviceObject, status);
            goto Done;
        }
        
        //
        // Compute buffer size needed for storing component information
        // (including F-states)
        //
        status = RtlULongAdd(idleStatesSize, 
                             sizeof(*component), 
                             &componentSize);
        if (!NT_SUCCESS(status)) 
        {
            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                 PCTRACE_POWER,
                                 "Device %p Unable to compute length of buffer "
                                 "required to store driver's component information. RtlULongAdd "
                                 "failed with %!STATUS!", 
                                 DeviceObject, status);
            goto Done;
        }

        ASSERT(componentSize != 0);
        buffer = (BYTE*) ExAllocatePoolWithTag( NonPagedPoolNx, componentSize, 'MRcP' );
        if (buffer == NULL)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                 PCTRACE_POWER,
                                 "Device %p Unable to allocate buffer required to "
                                 "store F-states. %!STATUS!",
                                 DeviceObject, status);
            goto Done;
        }

        RtlZeroMemory(buffer, componentSize);
    
        //
        // Set our pointers to point to appropriate locations in the buffer.
        //
        // NOTES:
        //   - The array of F-states comes first because it has ULONGLONG members 
        //     because of which it has the biggest alignment requirement.
        //   - The logic below works even if the client driver did not specify any
        //     component information. In that case idleStatesSize and componentSize
        //     are both 0 and 'poxSettings' points to the beginning of the allocated
        //     buffer
        //
        idleStates = (PPO_FX_COMPONENT_IDLE_STATE) buffer;
        component = (PPO_FX_COMPONENT) (buffer + idleStatesSize);

        //
        // Copy the component information
        //
        deviceCtx->RuntimePowerInfo->ScMs.PoFxComponent = component;
        RtlCopyMemory(deviceCtx->RuntimePowerInfo->ScMs.PoFxComponent,
                      PowerFrameworkSettings->Component,
                      sizeof(*component));
                
        //
        // Copy the F-states
        //
        deviceCtx->RuntimePowerInfo->ScMs.PoFxComponent->IdleStates = idleStates;
        RtlCopyMemory(deviceCtx->RuntimePowerInfo->ScMs.PoFxComponent->IdleStates,
                      PowerFrameworkSettings->Component->IdleStates,
                      idleStatesSize);
    }
    
    //
    // Copy the relevant parts of the settings buffer
    //
    deviceCtx->RuntimePowerInfo->ScMs.EvtPcPostPoFxRegisterDevice = PowerFrameworkSettings->EvtPcPostPoFxRegisterDevice;
    deviceCtx->RuntimePowerInfo->ScMs.EvtPcPrePoFxUnregisterDevice = PowerFrameworkSettings->EvtPcPrePoFxUnregisterDevice;
    deviceCtx->RuntimePowerInfo->ScMs.ComponentActiveConditionCallback = PowerFrameworkSettings->ComponentActiveConditionCallback;
    deviceCtx->RuntimePowerInfo->ScMs.ComponentIdleConditionCallback = PowerFrameworkSettings->ComponentIdleConditionCallback;
    deviceCtx->RuntimePowerInfo->ScMs.ComponentIdleStateCallback = PowerFrameworkSettings->ComponentIdleStateCallback;
    deviceCtx->RuntimePowerInfo->ScMs.PowerControlCallback = PowerFrameworkSettings->PowerControlCallback;
    deviceCtx->RuntimePowerInfo->ScMs.PoFxDeviceContext = PowerFrameworkSettings->PoFxDeviceContext;

    status = STATUS_SUCCESS;
        
Done:
    if (!NT_SUCCESS(status)) 
    {
        if (NULL != buffer) 
        {
            ExFreePoolWithTag(buffer, 'MRcP');
        }
    }
    
    return status;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * AcquireRuntimeFramework()
 *****************************************************************************
 * Acquire exclusive access to the runtime power framework registration using
 * the runtime framework mutex in the device context.
 */
NTSTATUS
AcquireRuntimeFramework
(
    __in PDEVICE_CONTEXT pDeviceContext
)
{
    PAGED_CODE();

    return KeWaitForSingleObject( &pDeviceContext->RuntimePowerInfo->RuntimeFrameworkMutex,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * AcquireRuntimeFrameworkStarted()
 *****************************************************************************
 * Acquire exclusive access to the runtime power framework registration using
 * the runtime framework mutex in the device context.  If the runtime management
 * isn't started after acquiring access to the framework, release and wait until 
 * runtime management is started again.
 */
NTSTATUS
AcquireRuntimeFrameworkStarted
(
    __in PDEVICE_CONTEXT pDeviceContext
)
{
    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_SUCCESS;

    PVOID WaitHandles[2] = {&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkStartedEvent, &pDeviceContext->RuntimePowerInfo->RuntimeFrameworkUnregisteredEvent};
    ULONG HandlesCount = 2;

    while( TRUE )
    {
        //
        // If the wait is satisfied because the framework was unregistered, then fail, otherwise, acquire the framework 
        // and ensure the framework is started.
        //
        if (KeWaitForMultipleObjects(HandlesCount, WaitHandles, WaitAny, Executive, KernelMode, FALSE, NULL, NULL) != STATUS_WAIT_0)
        {
            ntStatus = STATUS_INVALID_DEVICE_STATE;
            break;
        }

        ntStatus = AcquireRuntimeFramework( pDeviceContext );

        if( pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted )
        {
            break;
        }

        //
        // Someone suspended runtime management before we could acquire the
        // framework, release and try again.
        //
        ReleaseRuntimeFramework( pDeviceContext );
    }

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * ReleaseRuntimeFramework()
 *****************************************************************************
 * Release exclusive access to the runtime power framework registration using
 * the runtime framework mutex in the device context.
 */
VOID
ReleaseRuntimeFramework
(
    __in PDEVICE_CONTEXT pDeviceContext
)
{
    PAGED_CODE();

    KeReleaseMutex(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkMutex,FALSE);
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * RegisterRuntimeManagement
 *****************************************************************************
 * Registers our device with the power runtime framework.
 */
NTSTATUS RegisterRuntimeManagement
(
    _In_ PDEVICE_OBJECT pDeviceObject
)
{
    PAGED_CODE();
    
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    PDEVICE_CONTEXT         pDeviceContext = NULL;

    ASSERT(pDeviceObject);
    if (NULL == pDeviceObject)
    {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto Done;
    }

    pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if (NULL == pDeviceContext)
    {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto Done;
    }
    
    ASSERT(pDeviceContext->RuntimePowerInfo != NULL);
    if (NULL == pDeviceContext->RuntimePowerInfo)
    {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto Done;
    }

    if (pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates)
    {
        // Single-component/multi-state power management.
        ntStatus = RegisterRuntimeManagement_ScMs(pDeviceObject, pDeviceContext);
    }
    else
    {
        // Single-component/single-state power management.
        ntStatus = RegisterRuntimeManagement_ScSs(pDeviceObject, pDeviceContext);
    }

Done:
    return ntStatus;
}

/*****************************************************************************
 * RegisterRuntimeManagement: Single component / multiple state
 *****************************************************************************
 * Registers our device with the power runtime framework.
 */
NTSTATUS RegisterRuntimeManagement_ScSs
(
    _In_ PDEVICE_OBJECT     pDeviceObject,
    _In_ PDEVICE_CONTEXT    pDeviceContext
)
{
    PAGED_CODE();
    NTSTATUS                    ntStatus = STATUS_SUCCESS;
    UINT                        cbPoFxDevice = 0;
    PO_FX_DEVICE*               pFxDevice = NULL;
    PO_FX_COMPONENT*            pFxComponent = NULL;
    PO_FX_COMPONENT_IDLE_STATE* pFxIdleState = NULL;

    ASSERT(pDeviceObject);
    ASSERT(pDeviceContext);
    ASSERT(pDeviceContext->RuntimePowerInfo != NULL);

    //
    // Reserve enough space for a single power component with support for only
    // a single idle state (F0)
    //
    cbPoFxDevice = sizeof(PO_FX_DEVICE) +
                   PORTCLS_POFX_NUM_COMPONENTS * (sizeof(PO_FX_COMPONENT) +
                   PORTCLS_POFX_NUM_F_STATES * sizeof(PO_FX_COMPONENT_IDLE_STATE));

    pFxDevice = (PO_FX_DEVICE*) ExAllocatePoolWithTag( NonPagedPoolNx, cbPoFxDevice, 'MRcP' );
    if (pFxDevice == NULL)
    {
        ntStatus = STATUS_NO_MEMORY;
        goto error;
    }

    RtlZeroMemory(pFxDevice, cbPoFxDevice);

    //
    // Fill in the device information
    //
    pFxDevice->Version = PO_FX_VERSION_V1;
    pFxDevice->ComponentCount = PORTCLS_POFX_NUM_COMPONENTS;
    pFxDevice->ComponentActiveConditionCallback = PcPowerFxComponentActiveConditionCallback;
    pFxDevice->ComponentIdleConditionCallback   = PcPowerFxComponentIdleConditionCallback;
    pFxDevice->DevicePowerRequiredCallback      = PcPowerFxDevicePowerRequiredCallback;
    pFxDevice->DevicePowerNotRequiredCallback   = PcPowerFxDevicePowerNotRequiredCallback;
    pFxDevice->PowerControlCallback             = PcPowerFxPowerControlCallback;
    pFxDevice->DeviceContext = pDeviceObject;

    //
    // Fill in the F0 idle state info
    // Idle states start after all power components
    //
    pFxIdleState = (PO_FX_COMPONENT_IDLE_STATE*)&pFxDevice->Components[PORTCLS_POFX_NUM_COMPONENTS];
    pFxIdleState->TransitionLatency      = PO_FX_UNKNOWN_TIME;
    pFxIdleState->ResidencyRequirement   = PO_FX_UNKNOWN_TIME;
    pFxIdleState->NominalPower           = PO_FX_UNKNOWN_POWER;

    //
    // Fill in the component info
    //
    pFxComponent = &pFxDevice->Components[0];
    pFxComponent->IdleStateCount  = PORTCLS_POFX_NUM_F_STATES;
    pFxComponent->IdleStates      = pFxIdleState;

    //
    // Device is initially active
    //
    pDeviceContext->RuntimePowerInfo->DeviceActive = TRUE;

    //
    // Register with the runtime framework
    //
    ntStatus = PoFxRegisterDevice(pDeviceObject, pFxDevice, &(pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle));
    if (!NT_SUCCESS(ntStatus))
    {
        goto error;
    }

    //
    // Set the initial idle timeout delay on the framework
    //
    PoFxSetDeviceIdleTimeout( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle,
                              GetCurrentRuntimeIdleTimeout( pDeviceContext ) );

    //
    // Finally, start runtime management of our device
    //
    PoFxStartDevicePowerManagement(pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle);
    KeSetEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkStartedEvent, 0, FALSE);
    KeClearEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkUnregisteredEvent);
    pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted = TRUE;

exit:
    //
    // Free the device info as it is no longer needed
    //
    if (NULL != pFxDevice)
    {
        ExFreePoolWithTag(pFxDevice, 'MRcP');
    }

    return ntStatus;

error:

    //
    // For error during initialization, unregister
    //
    if (pDeviceContext != NULL && pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle != NULL)
    {
        PoFxUnregisterDevice(pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle);
        pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle = NULL;
        KeClearEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkStartedEvent);
        KeSetEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkUnregisteredEvent, 0, FALSE);
        pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted = FALSE;
    }

    goto exit;
}

/*****************************************************************************
 * RegisterRuntimeManagement: Single component / multiple state
 *****************************************************************************
 * Registers our device with the power runtime framework.
 */
NTSTATUS RegisterRuntimeManagement_ScMs
(
    _In_ PDEVICE_OBJECT     pDeviceObject,
    _In_ PDEVICE_CONTEXT    pDeviceContext
)
{
    PAGED_CODE();
    NTSTATUS                    ntStatus = STATUS_SUCCESS;
    PO_FX_DEVICE                pofxDevice;
    PO_FX_COMPONENT_IDLE_STATE  pofxIdleState;

    ASSERT(pDeviceObject);
    ASSERT(pDeviceContext);
    ASSERT(pDeviceContext->RuntimePowerInfo != NULL);

    RtlZeroMemory(&pofxDevice, sizeof(pofxDevice));
    RtlZeroMemory(&pofxIdleState, sizeof(pofxIdleState));

    //
    // Fill in the device information
    //
    pofxDevice.Version                          = PO_FX_VERSION_V1;
    pofxDevice.ComponentCount                   = 1;
    pofxDevice.ComponentIdleStateCallback       = PcPowerFxComponentIdleStateCallback;
    pofxDevice.ComponentActiveConditionCallback = PcPowerFxComponentActiveConditionCallback;
    pofxDevice.ComponentIdleConditionCallback   = PcPowerFxComponentIdleConditionCallback;
    pofxDevice.DevicePowerRequiredCallback      = PcPowerFxDevicePowerRequiredCallback;
    pofxDevice.DevicePowerNotRequiredCallback   = PcPowerFxDevicePowerNotRequiredCallback;
    pofxDevice.PowerControlCallback             = PcPowerFxPowerControlCallback;
    pofxDevice.DeviceContext                    = pDeviceObject;

    //
    // Fill in the component info
    //
    if (pDeviceContext->RuntimePowerInfo->ScMs.PoFxComponent)
    {
        RtlCopyMemory(&pofxDevice.Components[0], pDeviceContext->RuntimePowerInfo->ScMs.PoFxComponent, sizeof(pofxDevice.Components[0]));
    }
    else
    {
        //
        // Default to single component/single state if miniport didn't call PcAssignPowerFrameworkSettings.
        //
        pofxIdleState.TransitionLatency         = PO_FX_UNKNOWN_TIME;
        pofxIdleState.ResidencyRequirement      = PO_FX_UNKNOWN_TIME;
        pofxIdleState.NominalPower              = PO_FX_UNKNOWN_POWER;
        
        //
        // Fill in the component info
        //
        pofxDevice.Components[0].IdleStateCount = 1;
        pofxDevice.Components[0].IdleStates     = &pofxIdleState;
    }

    //
    // Device/component is initially active
    //
    pDeviceContext->RuntimePowerInfo->DeviceActive = TRUE;
    pDeviceContext->RuntimePowerInfo->ScMs.PoFxComponentState = 0; // F0.

    //
    // Register with the runtime framework
    //
    ntStatus = PoFxRegisterDevice(pDeviceObject, &pofxDevice, &(pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle));
    if (!NT_SUCCESS(ntStatus))
    {
        DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                             PCTRACE_POWER,
                             "Device %p, PoFxRegisterDevice failed, %!STATUS!",
                             pDeviceObject, ntStatus);        
        goto error;
    }

    //
    // Set the initial idle timeout delay on the framework
    //
    PoFxSetDeviceIdleTimeout( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle,
                              GetCurrentRuntimeIdleTimeout( pDeviceContext ) );

    //
    // Finally, start runtime management of our device
    //
    PoFxStartDevicePowerManagement(pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle);
    KeSetEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkStartedEvent, 0, FALSE);
    KeClearEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkUnregisteredEvent);
    pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted = TRUE;

    if (pDeviceContext->RuntimePowerInfo->ScMs.EvtPcPostPoFxRegisterDevice != NULL)
    {
        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_POWER,
                             "Device %p, invoking client driver's EvtPcPostPoFxRegisterDevice.",
                             pDeviceObject);
        
        ntStatus = pDeviceContext->RuntimePowerInfo->ScMs.EvtPcPostPoFxRegisterDevice(
                                        pDeviceContext->RuntimePowerInfo->ScMs.PoFxDeviceContext, 
                                        pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle);
        if (!NT_SUCCESS(ntStatus))
        {
            DoTraceLevelMessage( TRACE_LEVEL_ERROR,
                                 PCTRACE_POWER,
                                 "Device %p, EvtPcPostPoFxRegisterDevice failed, %!STATUS!",
                                 pDeviceObject, ntStatus);        
            goto error;
        }
    }

exit:

    return ntStatus;

error:
    // 
    // Handle (if valid) will be released by the UnregisterRuntimeManagement.
    //
    KeClearEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkStartedEvent);
    KeSetEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkUnregisteredEvent, 0, FALSE);
    pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted = FALSE;

    goto exit;
}

/*****************************************************************************
 * UnregisterRuntimeManagement
 *****************************************************************************
 * Unregisters our device from the power runtime framework.
 */
NTSTATUS UnregisterRuntimeManagement
(
    PDEVICE_CONTEXT pDeviceContext
)
{
    PAGED_CODE();

    //
    // Only need to unregister if our runtime framework handle is valid meaning
    // that previous registration succeeded.
    //
    if (pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle != NULL)
    {
        if (pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates &&
            pDeviceContext->RuntimePowerInfo->ScMs.EvtPcPrePoFxUnregisterDevice != NULL)
        {
            DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                 PCTRACE_POWER,
                                 "Device %p, invoking client driver's EvtPcPrePoFxUnregisterDevice.",
                                 pDeviceContext->RuntimePowerInfo->pDeviceObject);
            
            pDeviceContext->RuntimePowerInfo->ScMs.EvtPcPrePoFxUnregisterDevice(
                                         pDeviceContext->RuntimePowerInfo->ScMs.PoFxDeviceContext, 
                                         pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle);
        }
        
        AcquireRuntimeFramework(pDeviceContext);

        //
        // Call the framework to unregister.
        //
        PoFxUnregisterDevice(pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle);

        //
        // Clean up the device context.
        //
        pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle = NULL;
        KeClearEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkStartedEvent);
        KeSetEvent(&pDeviceContext->RuntimePowerInfo->RuntimeFrameworkUnregisteredEvent, 0, FALSE);
        pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted = FALSE;

        ReleaseRuntimeFramework(pDeviceContext);
    }

    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * ActivateRuntimePowerComponent()
 *****************************************************************************
 * Helper routine that makes an active call to the runtime power framework.
 * If bWait is set, then the blocking flag is specified, otherwise, the 
 * call is forced to complete asynchronously.
 */
NTSTATUS
ActivateRuntimePowerComponent
(
    __in PDEVICE_CONTEXT pDeviceContext,
    __in BOOLEAN         bWait
)
{
    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Wait for the runtime management to resume if it hasn't already.
    //
    if (!NT_SUCCESS(ntStatus = AcquireRuntimeFrameworkStarted( pDeviceContext )))
    {
        return ntStatus;
    }

    ASSERT( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle != NULL );
    if( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle == NULL )
    {
        goto Exit;
    }

    ASSERT( pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted );
    if( !pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted )
    {
        goto Exit;
    }

    //
    // Make the call to the runtime power framework.
    //
    PoFxActivateComponent( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle,
                           0,
                           bWait ? PO_FX_FLAG_BLOCKING : PO_FX_FLAG_ASYNC_ONLY );

Exit:
    ReleaseRuntimeFramework(pDeviceContext);
    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * IdleRuntimePowerComponent()
 *****************************************************************************
 * Helper routine that makes an idle call to the runtime power framework.
 * If bWait is set, then the blocking flag is specified, otherwise, the 
 * call is forced to complete asynchronously.
 */
NTSTATUS
IdleRuntimePowerComponent
(
    __in PDEVICE_CONTEXT pDeviceContext,
    __in BOOLEAN         bWait
)
{
    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Wait for the runtime management to resume if it hasn't already.
    //
    if (!NT_SUCCESS(ntStatus = AcquireRuntimeFrameworkStarted( pDeviceContext )))
    {
        return ntStatus;
    }

    ASSERT( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle != NULL );
    if( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle == NULL )
    {
        goto Exit;
    }

    ASSERT( pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted );
    if( !pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted )
    {
        goto Exit;
    }

    //
    // Make the call to the runtime power framework.
    //
    PoFxIdleComponent( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle,
                       0,
                       bWait ? PO_FX_FLAG_BLOCKING : PO_FX_FLAG_ASYNC_ONLY );

Exit:
    ReleaseRuntimeFramework(pDeviceContext);
    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * ReportRuntimePowerComponentPoweredOn()
 *****************************************************************************
 * Helper routine that makes an powered up call to the runtime power framework.
  */
NTSTATUS
ReportRuntimePowerComponentPoweredOn
(
    __in PDEVICE_CONTEXT pDeviceContext
)
{
    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Acquire exclusive access to our runtime framework registration.
    //
    if (!NT_SUCCESS(ntStatus = AcquireRuntimeFramework( pDeviceContext )))
    {
        return ntStatus;
    }

    //
    // If we've unregistered, simply exit.
    //
    if( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle == NULL )
    {
        goto Exit;
    }

    //
    // Our registration should be started at this point.
    //
    ASSERT( pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted );
    if( !pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted )
    {
        goto Exit;
    }

    //
    // Make the call to the runtime power framework.
    //
    PoFxReportDevicePoweredOn( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle );

Exit:
    ReleaseRuntimeFramework(pDeviceContext);
    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * SetRuntimePowerIdleTimeout()
 *****************************************************************************
 * Helper routine that sets the idle timeout that the runtime power framework 
 * should use in order to delay before calling our power not required 
 * callback.
 */
NTSTATUS
SetRuntimePowerIdleTimeout
(
    __in PDEVICE_CONTEXT    pDeviceContext,
    __in ULONGLONG          ullIdleTimeoutHns
)
{
    PAGED_CODE();

    NTSTATUS ntStatus = STATUS_SUCCESS;

    //
    // Wait for the runtime management to resume if it hasn't already.
    //
    if (!NT_SUCCESS(ntStatus = AcquireRuntimeFrameworkStarted( pDeviceContext )))
    {
        return ntStatus;
    }

    ASSERT( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle != NULL );
    if( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle == NULL )
    {
        goto Exit;
    }

    ASSERT( pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted );
    if( !pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted )
    {
        goto Exit;
    }

    //
    // Make the call to the runtime power framework to set the timeout.
    //
    PoFxSetDeviceIdleTimeout( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle,
                              ullIdleTimeoutHns );

Exit:
    ReleaseRuntimeFramework(pDeviceContext);

    return ntStatus;
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * GetCurrentRuntimeIdleTimeout()
 *****************************************************************************
 * Helper routine that evaluates the different inputs needed to determine
 * what the current runtime idle timeout should be.  Also converts registry
 * timeout values from seconds to hundreds of nanoseconds.
 */
ULONGLONG
GetCurrentRuntimeIdleTimeout
(
    __in PDEVICE_CONTEXT pDeviceContext
)
{
    PAGED_CODE();

    ASSERT( pDeviceContext != NULL );

    //
    // If idle power management is disabled, set the idle timeout to the 
    // maximum value.
    //
    if ( !pDeviceContext->EnableIdlePowerManagement )
    {
        return IDLE_TIMEOUT_MAX_HNS;
    }

    //
    // If the runtime power info has not been allocated yet, just use
    // the performance value (more power).
    //
    if ( NULL == pDeviceContext->RuntimePowerInfo )
    {
        return PORTCLS_IDLE_TO_RUNTIME_IDLE( pDeviceContext->PerformanceIdleTime );
    }

    //
    // If immediate idle is enabled, set the idle timeout to 0.
    //
    if ( pDeviceContext->RuntimePowerInfo->ImmediateIdle )
    {
        return 0;
    }

    //
    // If we're in low power epoch, use the hard-coded value to avoid taking
    // too long to idle power down.
    //
    if ( pDeviceContext->RuntimePowerInfo->LowPowerEpoch )
    {
        return IDLE_TIMEOUT_LOW_POWER_EPOCH_HNS;
    }
    
    //
    // If the current system power personality is set to save the least power, use the
    // performance value.
    //
    if ( PersonalityMinPowerSavings == pDeviceContext->RuntimePowerInfo->PowerPersonality )
    {
        return PORTCLS_IDLE_TO_RUNTIME_IDLE( pDeviceContext->PerformanceIdleTime );
    }
    
    //
    // If the current system power personality is set to save the most power, use the
    // conservation value.
    //
    if ( PersonalityMaxPowerSavings == pDeviceContext->RuntimePowerInfo->PowerPersonality )
    {
        return PORTCLS_IDLE_TO_RUNTIME_IDLE( pDeviceContext->ConservationIdleTime );
    }

    // 
    // Otherwise, the personality is set to typical so we need to look at the
    // current power source.  If the system is running off AC, then power savings
    // are less important, so use the performance value.
    //
    if ( PoAc == pDeviceContext->RuntimePowerInfo->PowerCondition )
    {
        return PORTCLS_IDLE_TO_RUNTIME_IDLE( pDeviceContext->PerformanceIdleTime );
    }
    
    //
    // If we got here, then the system is either running on DC (battery) or a
    // temporary AC source, so use the conservation value.
    //
    return PORTCLS_IDLE_TO_RUNTIME_IDLE( pDeviceContext->ConservationIdleTime );
}

#pragma code_seg("PAGE")
/*****************************************************************************
 * PcPowerSettingCallback()
 *****************************************************************************
 * Callback invoked whenever the systems power profile or AC/DC source status
 * changes.  We store the new values and update the idle timeout registered
 * with the runtime framework.
 */
__drv_functionClass(POWER_SETTING_CALLBACK)
_IRQL_requires_same_
NTSTATUS 
PcPowerSettingCallback(
    _In_                            LPCGUID SettingGuid,
    _In_reads_bytes_(ValueLength)   PVOID   Value,
    _In_                            ULONG   ValueLength,
    _Inout_opt_                     PVOID   Context
    )
{
    PAGED_CODE();

    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Setting Guid %!guid!, context %p",
                         SettingGuid,
                         Context);
    //
    // The context is our device context.
    //
    PDEVICE_CONTEXT pDeviceContext = (PDEVICE_CONTEXT)Context;
    ASSERT(Context);

    //
    // If the runtime power info isn't valid, just return.
    //
    if ( NULL == pDeviceContext->RuntimePowerInfo )
    {
        return STATUS_SUCCESS;
    }

    if ( InlineIsEqualGUID(*SettingGuid, GUID_ACDC_POWER_SOURCE) && (sizeof(SYSTEM_POWER_CONDITION) == ValueLength) )
    {
        //
        // The power source has changed, store the new source.
        //
        pDeviceContext->RuntimePowerInfo->PowerCondition = *((SYSTEM_POWER_CONDITION*)Value);
    }
    else if ( InlineIsEqualGUID(*SettingGuid, GUID_POWERSCHEME_PERSONALITY) && (sizeof(GUID) == ValueLength) )
    {
        //
        // The power personality has changed, store the new value.
        //
        pDeviceContext->RuntimePowerInfo->PowerPersonality =
            InlineIsEqualGUID(*(LPGUID)Value, GUID_MAX_POWER_SAVINGS)   ?   PersonalityMaxPowerSavings :
            InlineIsEqualGUID(*(LPGUID)Value, GUID_MIN_POWER_SAVINGS)   ?   PersonalityMinPowerSavings :
                                                                            PersonalityTypicalPowerSavings;
    }

    //
    // Re-evaluate the desired runtime idle timeout and set it on the framework if the framework
    // is already started.
    //
    if ( pDeviceContext->RuntimePowerInfo->RuntimePowerFrameworkStarted )
    {
        SetRuntimePowerIdleTimeout( pDeviceContext, GetCurrentRuntimeIdleTimeout( pDeviceContext ) );
    }

    return STATUS_SUCCESS;
}

#pragma code_seg()
/*****************************************************************************
 * PcPowerFxComponentIdleStateCallback()
 *****************************************************************************
 * Callback invoked whenever component's idle state changes. 
 */
__drv_functionClass(PO_FX_COMPONENT_IDLE_STATE_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PcPowerFxComponentIdleStateCallback(
    _In_ PVOID      Context,
    _In_ ULONG      Component,
    _In_ ULONG      State
    )
{
    PDEVICE_OBJECT  pDeviceObject = NULL;
    PDEVICE_CONTEXT pDeviceContext = NULL;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p, Component %d, State %d",
                         Context, Component, State);
    
    ASSERT(Component == 0);

    pDeviceObject = (PDEVICE_OBJECT)Context;
    ASSERT(pDeviceObject);
    if (NULL == pDeviceObject)
    {
        return;
    }

    pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if (NULL == pDeviceContext)
    {
        return;
    }

    ASSERT(pDeviceContext->RuntimePowerInfo != NULL);
    
    if (pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates)
    {
        pDeviceContext->RuntimePowerInfo->ScMs.PoFxComponentState = State;
    }
    
    //
    // Invoke the miniport callback if present.
    //
    if (pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates &&
        pDeviceContext->RuntimePowerInfo->ScMs.ComponentIdleStateCallback != NULL)
    {
        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_POWER,
                             "Device %p, invoking client driver's PO_FX_COMPONENT_IDLE_STATE_CALLBACK (%d).",
                             Context, State);
        
        pDeviceContext->RuntimePowerInfo->ScMs.ComponentIdleStateCallback(
                                     pDeviceContext->RuntimePowerInfo->ScMs.PoFxDeviceContext, 
                                     Component,
                                     State);
    }
    else
    {
        PoFxCompleteIdleState(pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle, Component);
    }
}

#pragma code_seg()
/*****************************************************************************
 * PcPowerFxComponentActiveConditionCallback()
 *****************************************************************************
 * Callback invoked whenever component's active state changes.  Updates the
 * active state in the device context.
 */
__drv_functionClass(PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PcPowerFxComponentActiveConditionCallback(
    _In_ PVOID      Context,
    _In_ ULONG      Component
    )
{
    PDEVICE_OBJECT  pDeviceObject = NULL;
    PDEVICE_CONTEXT pDeviceContext = NULL;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p, Component %d",
                         Context,
                         Component );

    ASSERT(Component == 0);
    
    pDeviceObject = (PDEVICE_OBJECT)Context;
    ASSERT(pDeviceObject);
    if (NULL == pDeviceObject)
    {
        return;
    }

    pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if (NULL == pDeviceContext)
    {
        return;
    }
    
    ASSERT(pDeviceContext->RuntimePowerInfo != NULL);
    
    //
    // Store the new active state of the device.
    //
    pDeviceContext->RuntimePowerInfo->DeviceActive = TRUE;

    //
    // Invoke the miniport callback if present.
    //
    if (pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates &&
        pDeviceContext->RuntimePowerInfo->ScMs.ComponentActiveConditionCallback != NULL)
    {
        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_POWER,
                             "Device %p, invoking client driver's PO_FX_COMPONENT_ACTIVE_CONDITION_CALLBACK.",
                             Context);
        
        pDeviceContext->RuntimePowerInfo->ScMs.ComponentActiveConditionCallback(
                                     pDeviceContext->RuntimePowerInfo->ScMs.PoFxDeviceContext, 
                                     Component);
    }
}

#pragma code_seg()
/*****************************************************************************
 * PcPowerFxComponentIdleConditionCallback()
 *****************************************************************************
 * Callback invoked whenever component's active state changes.  Updates the
 * active state in the device context.
 */
__drv_functionClass(PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PcPowerFxComponentIdleConditionCallback(
    _In_ PVOID      Context,
    _In_ ULONG      Component
    )
{
    PDEVICE_OBJECT  pDeviceObject = NULL;
    PDEVICE_CONTEXT pDeviceContext = NULL;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p, Component %d",
                         Context,
                         Component );

    ASSERT(Component == 0);
    
    pDeviceObject = (PDEVICE_OBJECT)Context;
    ASSERT(pDeviceObject);
    if (NULL == pDeviceObject)
    {
        return;
    }

    pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if (NULL == pDeviceContext)
    {
        return;
    }
    
    ASSERT(pDeviceContext->RuntimePowerInfo != NULL);
    
    //
    // Store the new active state of the device.
    //
    pDeviceContext->RuntimePowerInfo->DeviceActive = FALSE;

    //
    // Invoke the miniport callback if present.
    //
    if (pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates &&
        pDeviceContext->RuntimePowerInfo->ScMs.ComponentIdleConditionCallback != NULL)
    {
        DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                             PCTRACE_POWER,
                             "Device %p, invoking client driver's PO_FX_COMPONENT_IDLE_CONDITION_CALLBACK.",
                             Context);
        
        pDeviceContext->RuntimePowerInfo->ScMs.ComponentIdleConditionCallback(
                                     pDeviceContext->RuntimePowerInfo->ScMs.PoFxDeviceContext, 
                                     Component);
    }
    else
    {
        PoFxCompleteIdleCondition(pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle, Component);
    }
}

#pragma code_seg()
/*****************************************************************************
 * PcPowerFxDevicePowerRequiredCallback()
 *****************************************************************************
 * Callback invoked whenever the framework determines when we need
 * to be powered.  If power is required, idle power management is 
 * disabled and a D0 device power IRP is requested if necessary.
 */
__drv_functionClass(PO_FX_DEVICE_POWER_REQUIRED_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PcPowerFxDevicePowerRequiredCallback(
    _In_ PVOID      Context
    )
{
    PDEVICE_OBJECT  pDeviceObject = NULL;
    PDEVICE_CONTEXT pDeviceContext = NULL;
    POWER_STATE     newPowerState;
    KIRQL           oldIrql;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_POWER,
                         "Device %p",
                         Context);
    
    pDeviceObject = (PDEVICE_OBJECT)Context;
    ASSERT(pDeviceObject);
    if (NULL == pDeviceObject)
    {
        return;
    }

    pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if (NULL == pDeviceContext)
    {
        return;
    }

    //
    // Acquire the device power request spin lock
    //
    KeAcquireSpinLock( &pDeviceContext->RuntimePowerInfo->PowerStateLock, &oldIrql );

    //
    // Request a D0 device power IRP independently of the CurrentDeviceState.
    // It may be possible that the CurrentDeviceState is D0 even though a D3
    // device power IRP was requested by PcPowerFxDevicePowerNotRequiredCallback 
    // and about to be dispatched to portcls/driver.
    //
    newPowerState.DeviceState = PowerDeviceD0;

    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_POWER,
                         "Device %p, requesting device power state D%d -> D0",
                         Context,
                         pDeviceContext->CurrentDeviceState - PowerDeviceD0 );
    
    (void) PoRequestPowerIrp( pDeviceObject,
                              IRP_MN_SET_POWER,
                              newPowerState,
                              RuntimeDevicePowerUpIrpCompletionRoutine, 
                              NULL,
                              NULL );

    //
    // Release the device power request spinlock
    //
    KeReleaseSpinLock( &pDeviceContext->RuntimePowerInfo->PowerStateLock, oldIrql );
}

#pragma code_seg()
/*****************************************************************************
 * PcPowerFxDevicePowerNotRequiredCallback()
 *****************************************************************************
 * Callback invoked whenever the framework determines that we don't
 * need to be powered.  When power is not required, then idle power 
 * management is enabled.
 */
__drv_functionClass(PO_FX_DEVICE_POWER_NOT_REQUIRED_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
PcPowerFxDevicePowerNotRequiredCallback(
    _In_ PVOID      Context
    )
{
    PDEVICE_OBJECT  pDeviceObject = NULL;
    PDEVICE_CONTEXT pDeviceContext = NULL;
    POWER_STATE     newPowerState;
    KIRQL           oldIrql;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                         PCTRACE_POWER,
                         "Device %p",
                         Context);
    
    pDeviceObject = (PDEVICE_OBJECT)Context;
    ASSERT(pDeviceObject);
    if (NULL == pDeviceObject)
    {
        return;
    }

    pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if (NULL == pDeviceContext)
    {
        return;
    }

    //
    // Acquire the device power request spin lock
    //
    KeAcquireSpinLock( &pDeviceContext->RuntimePowerInfo->PowerStateLock, &oldIrql );

    //
    // Request an IdleDeviceState power IRP if the device isn't already in that state
    //
    newPowerState.DeviceState = pDeviceContext->IdleDeviceState;

    if ( pDeviceContext->CurrentDeviceState != pDeviceContext->IdleDeviceState )
    {
        // Log info.
        DoTraceLevelMessage( TRACE_LEVEL_INFORMATION,
                             PCTRACE_POWER,
                             "Device %p, requesting device power state D%d -> D%d",
                             Context,
                             pDeviceContext->CurrentDeviceState - PowerDeviceD0,
                             newPowerState.DeviceState - PowerDeviceD0 );
        
        (void) PoRequestPowerIrp( pDeviceObject,
                                    IRP_MN_SET_POWER,
                                    newPowerState,
                                    NULL, 
                                    NULL,
                                    NULL );
    }

    //
    // Release the device power request spinlock
    //
    KeReleaseSpinLock( &pDeviceContext->RuntimePowerInfo->PowerStateLock, oldIrql );

    PoFxCompleteDevicePowerNotRequired( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle );
}

#pragma code_seg()
/*****************************************************************************
 * PcPowerFxPowerControlCallback()
 *****************************************************************************
 * Callback invoked in response to a context sharing request with the 
 * framework.  Since we do not use or expose this functionality, this 
 * callback should never be invoked.
 */
__drv_functionClass(PO_FX_POWER_CONTROL_CALLBACK)
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
PcPowerFxPowerControlCallback(
    _In_                                    PVOID   DeviceContext,
    _In_                                    LPCGUID PowerControlCode,
    _In_reads_bytes_opt_(InBufferSize)      PVOID   InBuffer,
    _In_                                    SIZE_T  InBufferSize,
    _Out_writes_bytes_opt_(OutBufferSize)   PVOID   OutBuffer,
    _In_                                    SIZE_T  OutBufferSize,
    _Out_opt_                               PSIZE_T BytesReturned
)
{
    PDEVICE_OBJECT  pDeviceObject = NULL;
    PDEVICE_CONTEXT pDeviceContext = NULL;
    NTSTATUS        status;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p, PowerControlCode %!guid!",
                         DeviceContext,
                         PowerControlCode);
    
    pDeviceObject = (PDEVICE_OBJECT)DeviceContext;
    ASSERT(pDeviceObject);
    if (NULL == pDeviceObject)
    {
        return STATUS_INVALID_PARAMETER;
    }

    pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT(pDeviceContext);
    if (NULL == pDeviceContext)
    {
        return STATUS_INVALID_PARAMETER;
    }

    ASSERT(pDeviceContext->RuntimePowerInfo != NULL);
    
    //
    // If a callback has not been registered, we can't support this callback.
    //
    status = STATUS_NOT_IMPLEMENTED;
    
    if (pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates)
    {
        if (pDeviceContext->RuntimePowerInfo->ScMs.PowerControlCallback != NULL)
        {
            DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                 PCTRACE_POWER,
                                 "Device %p, invoking client driver's PO_FX_POWER_CONTROL_CALLBACK.",
                                 DeviceContext);
            
            status = pDeviceContext->RuntimePowerInfo->ScMs.PowerControlCallback(
                                         pDeviceContext->RuntimePowerInfo->ScMs.PoFxDeviceContext, 
                                         PowerControlCode,
                                         InBuffer,
                                         InBufferSize,
                                         OutBuffer,
                                         OutBufferSize,
                                         BytesReturned);
        }
    }
    else
    {
        if (pDeviceContext->RuntimePowerInfo->ScSs.PowerControlCallback != NULL)
        {
            DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                                 PCTRACE_POWER,
                                 "Device %p, invoking client driver's PO_FX_POWER_CONTROL_CALLBACK.",
                                 DeviceContext);
        
            status =  pDeviceContext->RuntimePowerInfo->ScSs.PowerControlCallback(
                                        PowerControlCode,
                                        InBuffer,
                                        InBufferSize,
                                        OutBuffer,
                                        OutBufferSize,
                                        BytesReturned,
                                        pDeviceContext->RuntimePowerInfo->ScSs.PowerControlCallbackContext);
        }
    }

    return status;
}

#pragma code_seg()
/*****************************************************************************
 * iSendRuntimePowerControl()
 *****************************************************************************
 * Helper method for the ports to be able to call the runtime power
 * framework to share private context with the PEP.
 */
NTSTATUS iSendRuntimePowerControl(
    _In_      PDEVICE_OBJECT    _DeviceObject,
    _In_      LPCGUID           _PowerControlCode,
    _In_opt_  PVOID             _InBuffer,
    _In_      SIZE_T            _InBufferSize,
    _Out_opt_ PVOID             _OutBuffer,
    _In_      SIZE_T            _OutBufferSize,
    _Out_opt_ PSIZE_T           _BytesReturned
    )  
{ 
    NTSTATUS ntStatus = STATUS_SUCCESS;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p",
                         _DeviceObject );
    //
    // Validate input
    //
    if ( (NULL == _DeviceObject) || (NULL == _DeviceObject->DeviceExtension) )
    {
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(_DeviceObject->DeviceExtension);

    //
    // This API is only supported if Single Component/Multiple Fx State is not enabled.
    //
    if (pDeviceContext->RuntimePowerInfo == NULL ||
        pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates == TRUE)
    {
        return STATUS_DEVICE_CONFIGURATION_ERROR;
    }

    //
    // Acquire access to the runtime framework
    //
    if (!NT_SUCCESS(ntStatus = AcquireRuntimeFramework( pDeviceContext )))
    {
        return ntStatus;
    }

    if( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle == NULL )
    {
        ntStatus = STATUS_INVALID_DEVICE_STATE;
        goto Exit;
    }

    //
    // Make the call to the runtime power framework.
    //
    ntStatus = PoFxPowerControl(
                                pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle,
                                _PowerControlCode,
                                _InBuffer,
                                _InBufferSize,
                                _OutBuffer,
                                _OutBufferSize,
                                _BytesReturned); 

Exit:
    ReleaseRuntimeFramework(pDeviceContext);
    return ntStatus;   
}

/*****************************************************************************
 * iRegisterRuntimePowerControlCallback
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
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p, Callback %p, Context %p",
                         _DeviceObject,
                         _Callback,
                         _Context );
    //
    // Validate input
    //
    if ( (NULL == _DeviceObject) || (NULL == _DeviceObject->DeviceExtension) || (NULL == _Callback) )
    {
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(_DeviceObject->DeviceExtension);

    //
    // This API is only supported if Single Component/Multiple Fx State is not enabled.
    //
    if (pDeviceContext->RuntimePowerInfo == NULL ||
        pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates == TRUE)
    {
        return STATUS_DEVICE_CONFIGURATION_ERROR;
    }

    //
    // Acquire access to the runtime framework
    //
    if (!NT_SUCCESS(ntStatus = AcquireRuntimeFramework( pDeviceContext )))
    {
        return ntStatus;
    }

    //
    // Stash away the callback and context to be invoked on power control callbacks from the framework
    //
    pDeviceContext->RuntimePowerInfo->ScSs.PowerControlCallback = _Callback;
    pDeviceContext->RuntimePowerInfo->ScSs.PowerControlCallbackContext = _Context;

    ReleaseRuntimeFramework(pDeviceContext);
    return ntStatus;   
}

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
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    
    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p",
                         _DeviceObject );
    //
    // Validate input
    //
    if ( (NULL == _DeviceObject) || (NULL == _DeviceObject->DeviceExtension) )
    {
        return STATUS_INVALID_PARAMETER;
    }

    PDEVICE_CONTEXT pDeviceContext =
        PDEVICE_CONTEXT(_DeviceObject->DeviceExtension);

    //
    // This API is only supported if Single Component/Multiple Fx State is not enabled.
    //
    if (pDeviceContext->RuntimePowerInfo == NULL ||
        pDeviceContext->RuntimePowerInfo->SingleComponentMultiFxStates == TRUE)
    {
        return STATUS_DEVICE_CONFIGURATION_ERROR;
    }

    //
    // Acquire access to the runtime framework
    //
    if (!NT_SUCCESS(ntStatus = AcquireRuntimeFramework( pDeviceContext )))
    {
        return ntStatus;
    }

    //
    // Clear the callback that was previously registered and the context
    //
    pDeviceContext->RuntimePowerInfo->ScSs.PowerControlCallback = NULL;
    pDeviceContext->RuntimePowerInfo->ScSs.PowerControlCallbackContext = NULL;
    
    ReleaseRuntimeFramework(pDeviceContext);
    return ntStatus;   
}

#pragma code_seg()
/*****************************************************************************
 * RuntimeDevicePowerUpIrpCompletionRoutine
 *****************************************************************************
 * Completion routine for device power irps resulting from a power required
 * callback made by the runtime power framework.
 *
 */
VOID
RuntimeDevicePowerUpIrpCompletionRoutine
(
    IN      PDEVICE_OBJECT      pDeviceObject,
    IN      UCHAR               MinorFunction,
    IN      POWER_STATE         PowerState,
    IN      PVOID               pContext,
    IN      PIO_STATUS_BLOCK    pIoStatus
)
{
    ASSERT(pDeviceObject);

    // Log info.
    DoTraceLevelMessage( TRACE_LEVEL_VERBOSE,
                         PCTRACE_POWER,
                         "Device %p",
                         pDeviceObject );

    PDEVICE_CONTEXT pDeviceContext = PDEVICE_CONTEXT(pDeviceObject->DeviceExtension);
    ASSERT( pDeviceContext );

    if( pDeviceContext != NULL)
    {
        PoFxReportDevicePoweredOn( pDeviceContext->RuntimePowerInfo->pRuntimePowerFrameworkHandle );
    }
}

