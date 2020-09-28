/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    aspm.c

Abstract:

    This module contains functions associated with active state link power
    management (ASPM), an express feature.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes
// --------------------------------------------------------------------- Pragmas

NTSTATUS
ExpressBridgeRegisterDeviceForAspmCallback (
    __in PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressPortInitializeLinkPowerManagement (
    _In_ PEXPRESS_PORT ExpressPort,
    _Inout_ PBOOLEAN EnableL0sOverride,
    _Inout_ PBOOLEAN EnableL1Override,
    _Inout_ PULONG AspmOverrideFlags
    );

NTSTATUS
ExpressDownstreamSwitchPortProcessAspmPolicy (
    _In_ PEXPRESS_BRIDGE ExpressBridge,
    _Inout_ PBOOLEAN DisableLinkPowerManagement
    );

BOOLEAN
ExpressOverrideLinkPowerManagement (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressUpdateBaselineLinkPowerManagementSupport (
    _In_ PEXPRESS_BRIDGE ExpressBridge,
    _Inout_ PBOOLEAN EnableL0sOverride,
    _Inout_ PBOOLEAN EnableL1Override
    );

VOID
ExpressUpdateBaselineVersion (
    __in PEXPRESS_BRIDGE ExpressBridge
    );

BOOLEAN
ExpressPortFindOptInOptOutPolicy (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressLinkDisableL0sL1State (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

NTSTATUS
ExpressLinkEnableL0sState (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

NTSTATUS
ExpressLinkEnableL0sL1State (
    __inout PEXPRESS_BRIDGE ExpressBridge
    );

VOID
ExpressBridgeDisableL0sL1OnUpstreamLink (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in PEXPRESS_LINK UpstreamLink
    );

VOID
ExpressPortDisableL0sL1OnDownstreamLink (
    __inout PEXPRESS_PORT ExpressPort
    );

VOID
ExpressUpdateReceiveLaneExitLatency (
    __inout PEXPRESS_LINK ExpressLink
    );

NTSTATUS
ExpressPortUpdateLatencyBound (
    __inout PEXPRESS_PORT ExpressPort
    );

NTSTATUS
ExpressBridgeEnableCommonClockConfiguration (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __inout PEXPRESS_LINK ExpressLink
    );

VOID
ExpressEnableDisableDownstreamLinkStatesForRetrain (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in BOOLEAN EnableLinkStates
    );

VOID
ExpressBridgeStallForLinkRetrain (
    __in PEXPRESS_BRIDGE ExpressBridge
    );

POWER_SETTING_CALLBACK ExpressLinkPowerSchemeCallback;

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, ExpressBridgeInitializeLinkPowerManagement)
    #pragma alloc_text(PAGE, ExpressBridgeProcessLinkForPowerManagement)
    #pragma alloc_text(PAGE, ExpressBridgeRegisterDeviceForAspmCallback)
    #pragma alloc_text(PAGE, ExpressBridgeUnregisterDeviceForAspmCallback)
    #pragma alloc_text(PAGE, ExpressBridgeUnregisterAspmPolicyCallback)
    #pragma alloc_text(PAGE, ExpressPortInitializeLinkPowerManagement)
    #pragma alloc_text(PAGE, ExpressPortPrepareForLinkPowerManagement)
    #pragma alloc_text(PAGE, ExpressDownstreamSwitchPortProcessAspmPolicy)
    #pragma alloc_text(PAGE, ExpressOverrideLinkPowerManagement)
    #pragma alloc_text(PAGE, ExpressUpdateBaselineLinkPowerManagementSupport)
    #pragma alloc_text(PAGE, ExpressUpdateBaselineVersion)
    #pragma alloc_text(PAGE, ExpressPortFindOptInOptOutPolicy)
    #pragma alloc_text(PAGE, ExpressUpdateLinkPowerManagementSupport)
    #pragma alloc_text(PAGE, ExpressSwitchUpdateLinkLatency)
    #pragma alloc_text(PAGE, ExpressUpdateReceiveLaneExitLatency)
    #pragma alloc_text(PAGE, ExpressPortUpdateLatencyBound)
    #pragma alloc_text(PAGE, ExpressBridgeEnableCommonClockConfiguration)
    #pragma alloc_text(PAGE, ExpressLinkPowerSchemeCallback)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions
// ------------------------------------------------- Routines for express bridge

NTSTATUS
ExpressBridgeInitializeLinkPowerManagement (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __inout PEXPRESS_LINK ExpressLink
    )

/*++

Routine Description:

    This is an init routine for link power management of a root port
    or downstream switch port. This routine saves the power management
    support and exit latencies for an express link emanating from the
    bridge. It invokes a routine to find the Aspm policy for downstream
    switch ports.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge for a root port or
        downstream switch port.

    ExpressLink - Supplies a pointer to an express link on a root port or
        downstream switch port.

Return Value:

    STATUS_SUCCESS.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    PCI_ASSERT((ExpressBridge->BridgeType == PciExpressRootPort) ||
               (ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort));

    //
    // Check for system policy on Aspm.
    //

    if ((PciSystemWideHackFlags & PCI_SYS_HACK_DISABLE_EXPRESS_ASPM) != 0) {
        return STATUS_SUCCESS;
    }

    //
    // Process Aspm policy for downstream switch port.
    //

    if (ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort) {
        ExpressDownstreamSwitchPortProcessAspmPolicy(
            ExpressBridge,
            &ExpressBridge->Downstream.u.SwitchPort.UpstreamLinkL0sL1Override);
    }

    //
    // Find out if the express link supports link power management and if not
    // then bail out with success.
    //

    Status = ExpressUpdateLinkPowerManagementSupport(ExpressLink);
    if (!NT_SUCCESS(Status)) {
        return STATUS_SUCCESS;
    }

    //
    // Cache the L0s and L1 exit latencies.
    //

    ExpressUpdateReceiveLaneExitLatency(ExpressLink);
    return STATUS_SUCCESS;
}

NTSTATUS
ExpressBridgeProcessLinkForPowerManagement (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine is invoked after a bus scan to prepare and configure an
    express link for power management. Preparing a link involves placing
    the components at each end on a common clock and retraining the link.
    For configuring an express link, this routine finds the baseline version
    and link power management capabilities for all functions on a device
    at the downstream end of the link. Thereafter it disables the L0s and
    L1 link states on an express link until each function on the device
    is started.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge.

Return Value:

    STATUS_NOT_SUPPORTED for an express bridge that is not a root port or
        downstream switch port.

    STATUS_SUCCESS.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    PEXPRESS_PORT ExpressPort;
    NTSTATUS Status;

    PAGED_CODE();

    if (PciSystemWideHackFlags & PCI_SYS_HACK_DISABLE_EXPRESS_ASPM) {
        return STATUS_SUCCESS;
    }

    //
    // Any express bridge link power state configuration is only for root ports
    // and downstream switch ports.
    //

    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        return STATUS_NOT_SUPPORTED;
    }

    //
    // If there is no device behind the bridge then bail out with success.
    //

    Bus = ExpressBridge->PciBridge->Bus;
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    if (Bus->ChildDevices == NULL) {
        PciReleasePassiveLock(&Bus->ChildDeviceLock);
        return STATUS_SUCCESS;
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // If policy dictates or the bridge contains errata, disable ASPM on the
    // bridge.
    //

    if (ExpressBridge->PciBridge->DisableLinkPowerManagement != 0) {
        ExpressBridge->Downstream.AspmL0sOverride = TRUE;
        ExpressBridge->Downstream.AspmL1Override = TRUE;
        ExpressBridge->Downstream.AspmOverrideFlags |=
            PCI_ASPM_L1_ERRATA | PCI_ASPM_L0s_ERRATA;
        goto Exit;
    }

    //
    // Configure the components at the two ends of an express link to use the
    // same reference clock. A failure from this call is not fatal.
    //

    Status = ExpressBridgeEnableCommonClockConfiguration(
                ExpressBridge,
                &ExpressBridge->Downstream.ExpressLink
                );

    if (NT_SUCCESS(Status)) {

        //
        // The link state exit latencies can vary based on the clock config
        // in use. Update our notion of latencies for L0s and L1 states as
        // an attempt was made above to put the link components on a common
        // reference clock.
        //

        ExpressUpdateReceiveLaneExitLatency(&ExpressBridge->Downstream.ExpressLink);
    }

    //
    // Check if any device on this bridge is specified in the errata manager
    // as incapable of supporting link power management. In that case update
    // the downstream link profile and disable Aspm for this link.
    //

    if (ExpressOverrideLinkPowerManagement(ExpressBridge) != FALSE) {
        ExpressBridge->Downstream.AspmL0sOverride = TRUE;
        ExpressBridge->Downstream.AspmL1Override = TRUE;
        ExpressBridge->Downstream.AspmOverrideFlags |=
            PCI_ASPM_L1_ERRATA | PCI_ASPM_L0s_ERRATA;
        goto Exit;
    }

    //
    // Walk through the child list and prepare the device for link power
    // management. This device may be a single function or a multi-function
    // device.
    //
    // N.B. The initialize link power management routine for an express port
    //      must be called after clock configuration so that the exit latencies
    //      are properly determined.
    //

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (Device = Bus->ChildDevices;
         Device != NULL;
         Device = Device->Sibling) {

        ExpressPort = Device->ExpressPort;
        if (ExpressPort == NULL) {
            continue;
        }

        if (!EXPRESS_INGRESS_LINK_PORT(ExpressPort)) {
            continue;
        }

        ExpressPortInitializeLinkPowerManagement(
            ExpressPort,
            &ExpressBridge->Downstream.AspmL0sOverride,
            &ExpressBridge->Downstream.AspmL1Override,
            &ExpressBridge->Downstream.AspmOverrideFlags);

    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // Find an active common denominator of link power management support
    // across all functions.
    //

    ExpressUpdateBaselineLinkPowerManagementSupport(
        ExpressBridge,
        &ExpressBridge->Downstream.AspmL0sOverride,
        &ExpressBridge->Downstream.AspmL1Override);

    ExpressUpdateBaselineVersion(ExpressBridge);

Exit:
    ExpressLinkDisableL0sL1State(ExpressBridge);
    return STATUS_SUCCESS;
}

NTSTATUS
ExpressBridgeRegisterDeviceForAspmCallback (
    __in PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked to register an Aspm policy callback for link power
    management of an express port. The registration is done when all functions
    are willing to participate in link power management and are started.

Arguments:

    ExpressPort - Supplies a pointer to an express port.

Return Value:

    STATUS_SUCCESS or an error code returned from a call in this routine.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    ULONG DevicesScannedOnBus;
    ULONG RegistrantsForAspmCallback;
    PEXPRESS_BRIDGE ExpressBridge;
    NTSTATUS Status;

    PAGED_CODE();

    PCI_ASSERT(ExpressPort->RegisteredForAspmCallback != 0);


    ExpressBridge = ExpressPort->Parent;
    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        PCI_ASSERT(FALSE);
        return STATUS_SUCCESS;
    }

    //
    // Update the reference count maintained in an express bridge tracking
    // the number of devices that can be enabled for Aspm.
    //

    RegistrantsForAspmCallback = 0;
    RegistrantsForAspmCallback =
        InterlockedIncrement(
            &ExpressBridge->Downstream.RegistrantsForAspmCallback);

    //
    // Find the number of devices scanned behind a bridge.
    //

    Bus = ExpressBridge->PciBridge->Bus;
    DevicesScannedOnBus = 0;
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (Device = Bus->ChildDevices; Device != NULL; Device = Device->Sibling) {
        DevicesScannedOnBus += 1;
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // If all devices are onboard then register a policy callback to update the
    // link states.
    //

    Status = STATUS_SUCCESS;
    if ((RegistrantsForAspmCallback == DevicesScannedOnBus) &&
        (ExpressBridge->Downstream.AspmPolicyHandle == NULL)) {

        Status = PoRegisterPowerSettingCallback(
                    ExpressBridge->PciBridge->Bus->DeviceObject,
                    &GUID_PCIEXPRESS_ASPM_POLICY,
                    ExpressLinkPowerSchemeCallback,
                    ExpressBridge,
                    &ExpressBridge->Downstream.AspmPolicyHandle);

    }

    return Status;
}

NTSTATUS
ExpressBridgeUnregisterDeviceForAspmCallback (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked to unregister an express port so that it does not
    receive any Aspm callback. The callback is unregistered as soon as any
    PDO for an express port (in case of multifunction device) receives a call
    to destroy the device.

Arguments:

    ExpressPort - Supplies a pointer to an express port.

Return Value:

    STATUS_SUCCESS

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;

    PAGED_CODE();

    if (ExpressPort->RegisteredForAspmCallback == 0) {
        return STATUS_SUCCESS;
    }

    ExpressBridge = ExpressPort->Parent;
    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        PCI_ASSERT(FALSE);
        return STATUS_SUCCESS;
    }

    InterlockedDecrement(
        &ExpressBridge->Downstream.RegistrantsForAspmCallback);

    ExpressPort->RegisteredForAspmCallback = 0;
    ExpressBridgeUnregisterAspmPolicyCallback(
        ExpressBridge,
        &ExpressBridge->Downstream.AspmPolicyHandle);

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressBridgeUnregisterAspmPolicyCallback (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __inout PVOID *AspmPolicyHandle
    )

/*++

Routine Description:

    This routine is invoked to de-register Aspm policy callback.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

    AspmPolicyHandle - Supplies a pointer to a placeholder for the Aspm policy
        handle.

Return Value:

    STATUS_SUCCESS or an error code returned from a function call.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(ExpressBridge);

    Status = STATUS_SUCCESS;
    if (*AspmPolicyHandle != NULL) {
        Status = PoUnregisterPowerSettingCallback(*AspmPolicyHandle);

        PCI_ASSERT(NT_SUCCESS(Status));

        *AspmPolicyHandle = NULL;
    }

    return Status;
}

// --------------------------------------------------- Routines for express port

NTSTATUS
ExpressPortInitializeLinkPowerManagement (
    _In_ PEXPRESS_PORT ExpressPort,
    _Inout_ PBOOLEAN EnableL0sOverride,
    _Inout_ PBOOLEAN EnableL1Override,
    _Inout_ PULONG AspmOverrideFlags
    )

/*++

Routine Description:

    This routine is invoked to prepare an ingress (downstream) link for
    power management. For an endpoint port, this routine computes the
    latencies perceived by the device, compares them to the acceptable
    latencies and determines if there is a need for an override.

    This routine is invoked with the child device lock held.

Arguments:

    ExpressPort - Supplies a pointer to an express port for a device.

    EnableL0sOverride - Supplies a placeholder for a boolean to specify an L0s
        link state override.

    EnableL1Override - Supplies a placeholder for a boolean to specify an L1
        link state override.

    AspmOverrideFlags - Supplies a placeholder for flags which provide
        ASPM debugging information

Return Value:

    STATUS_SUCCESS.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;
    PEXPRESS_LINK ExpressLink;
    ULONG L0sReceiveLatencyForEndpoint;
    ULONG L0sTransmitLatencyForEndpoint;
    ULONG L1LatencyForEndpoint;
    NTSTATUS Status;
    PSWITCH_COMPLEX SwitchComplex;
    PEXPRESS_LINK UpstreamLink;

    PAGED_CODE();

    //
    // Examine the link capabilities register to find Aspm support for the
    // downstream end of an express link.
    //

    ExpressLink = ExpressPort->ExpressLink;
    Status = ExpressUpdateLinkPowerManagementSupport(ExpressLink);
    if (!NT_SUCCESS(Status)) {
        return STATUS_SUCCESS;
    }

    //
    // Cache the L0s and L1 exit latencies.
    //

    ExpressUpdateReceiveLaneExitLatency(ExpressLink);

    //
    // If this is an upstream switch port or an Express to PCI/PCI-X bridge
    // then there is no more work to be done for enabling Aspm on these links.
    //

    if ((ExpressPort->DeviceType == PciExpressUpstreamSwitchPort) ||
        (ExpressPort->DeviceType == PciExpressToPciXBridge)) {

        return STATUS_SUCCESS;
    }

    //
    // Find the acceptable latency for an express endpoint.
    //

    ExpressPortUpdateLatencyBound(ExpressPort);

    //
    // Ensure that the latencies perceived by the endpoint is within the
    // acceptable latency limit for the device. Otherwise set the override
    // field to prevent this endpoint from entering into the corresponding
    // link power state.
    //

    //
    // First, compute the L0s latency perceived by the device.
    //

    ExpressBridge = ExpressPort->Parent;
    UpstreamLink = ExpressLink->TargetLink;
    if (ExpressLink->L0sSupported != FALSE) {
        L0sReceiveLatencyForEndpoint = UpstreamLink->L0sExitLatencyHigh;
        L0sTransmitLatencyForEndpoint = ExpressLink->L0sExitLatencyHigh;
        if (ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort) {
            SwitchComplex = ExpressBridge->Downstream.u.SwitchPort.SwitchComplex;
            L0sReceiveLatencyForEndpoint =
                max(SwitchComplex->ComputedL0sReceiveLatency,
                    L0sReceiveLatencyForEndpoint);

            L0sTransmitLatencyForEndpoint =
                max(SwitchComplex->ComputedL0sTransmitLatency,
                    L0sTransmitLatencyForEndpoint);

        }

        ExpressPort->ComputedL0sLatency = L0sReceiveLatencyForEndpoint +
                                          L0sTransmitLatencyForEndpoint;

        if (ExpressPort->ComputedL0sLatency >
            ExpressPort->AcceptableL0sLatency) {

            *EnableL0sOverride = TRUE;
            *AspmOverrideFlags |= PCI_ASPM_L0s_LATENCY;
        }
    }

    //
    // Compute the L1 latency perceived by the device.
    //

    if ((UpstreamLink->L1Supported != FALSE) &&
        (ExpressLink->L1Supported != FALSE)) {

        L1LatencyForEndpoint = max(UpstreamLink->L1ExitLatencyHigh,
                                    ExpressLink->L1ExitLatencyHigh);

        if (ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort) {
            SwitchComplex = ExpressBridge->Downstream.u.SwitchPort.SwitchComplex;
            L1LatencyForEndpoint =
                max((SwitchComplex->ComputedL1Latency + 1000),
                    L1LatencyForEndpoint);

        }

        ExpressPort->ComputedL1Latency = L1LatencyForEndpoint;
        if (L1LatencyForEndpoint > ExpressPort->AcceptableL1Latency) {
            *EnableL1Override = TRUE;
            *AspmOverrideFlags |= PCI_ASPM_L1_LATENCY;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressPortPrepareForLinkPowerManagement (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked to register a callback for configuring an express
    link in one of the link power states based on the policy in play.

Arguments:

    ExpressPort - Supplies a pointer to an express port.

Return Value:

    STATUS_SUCCESS on success or an appropriate error code returned from a
        function call.

--*/

{

    PEXPRESS_BRIDGE ExpressBridge;
    PEXPRESS_LINK ExpressLink;
    BOOLEAN Leave;
    NTSTATUS Status;

    PAGED_CODE();

    if ((PciSystemWideHackFlags & PCI_SYS_HACK_DISABLE_EXPRESS_ASPM) != 0) {
        return STATUS_SUCCESS;
    }

    //
    // Examine the express bridge for link state overrides on the downstream
    // link.
    //

    if (ExpressPort->Parent == NULL) {
        return STATUS_SUCCESS;
    }

    ExpressBridge = ExpressPort->Parent;
    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        return STATUS_SUCCESS;
    }

    //
    // If it has been determined that the downstream link will not be enabled
    // for any Aspm states then bail out with success.
    //

    if ((ExpressBridge->Downstream.AspmL0sOverride != FALSE) &&
        (ExpressBridge->Downstream.AspmL1Override != FALSE)) {
        return STATUS_SUCCESS;
    }

    //
    // Examine the express port for Aspm policy callback.
    //

    if (ExpressPort->ExpressLink == NULL) {
        return STATUS_SUCCESS;
    }

    if (ExpressPort->RegisteredForAspmCallback != 0) {
        return STATUS_SUCCESS;
    }

    ExpressLink = ExpressPort->ExpressLink;
    Leave = FALSE;
    Status = STATUS_SUCCESS;
    switch (ExpressPort->DeviceType) {
    case PciExpressUpstreamSwitchPort:
    case PciExpressToPciXBridge:

        //
        // If an upstream switch port or an express to Pci-X bridge is
        // integrated inside a chipset then do not evaluate device policy
        // source.
        //

        if (ExpressPort->Parent->ExpressCapabilities.SlotImplemented == 0) {
            break;
        }

        //
        // Fall through.
        //

    case PciExpressEndpoint:
    case PciExpressLegacyEndpoint:
        if (ExpressPortFindOptInOptOutPolicy(ExpressPort) != FALSE) {
            ExpressBridge->Downstream.AspmL0sOverride = TRUE;
            ExpressBridge->Downstream.AspmL1Override = TRUE;
            ExpressBridge->Downstream.AspmOverrideFlags |=
                PCI_ASPM_L1_OPTOUT | PCI_ASPM_L0s_OPTOUT;
            Leave = TRUE;
        }

        break;

    case PciExpressRootPort:
    case PciExpressDownstreamSwitchPort:
    case PciXToExpressBridge:
    case PciExpressRootComplexIntegratedEndpoint:

        PCI_ASSERT(FALSE);

        Leave = TRUE;
        break;
    }

    if (Leave != FALSE) {
        return Status;
    }

    if (((ExpressLink->L0sSupported != FALSE) &&
         (ExpressBridge->Downstream.AspmL0sOverride == FALSE)) ||
        ((ExpressLink->L1Supported != FALSE) &&
         (ExpressBridge->Downstream.AspmL1Override == FALSE))) {

        ExpressPort->RegisteredForAspmCallback = 1;
        Status = ExpressBridgeRegisterDeviceForAspmCallback(ExpressPort);
    }

    return Status;
}

// ---------------------------------------------------- Routines for Aspm policy

NTSTATUS
ExpressDownstreamSwitchPortProcessAspmPolicy (
    _In_ PEXPRESS_BRIDGE ExpressBridge,
    _Inout_ PBOOLEAN DisableLinkPowerManagement
    )

/*++

Routine Description:

    This routine is invoked to find the link power management policy for a
    downstream switch port. If the switch port is integrated in a chipset
    then the system policy on Aspm is applicable for it. In other cases,
    evaluate all sources of Aspm policy and update the field that denotes
    the policy for downstream switch port.


Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

    DisableLinkPowerManagement - Supplies a place holder for a boolean that is
        set to TRUE for not enabling link power management.

Return Value:

    STATUS_SUCCESS.

--*/

{

    ULONG BufferLength;
    DEVICE_INSTALL_STATE InstallState;
    PDEVICE_OBJECT PhysicalDeviceObject;
    BOOLEAN Result;
    NTSTATUS Status;
    PSWITCH_COMPLEX SwitchComplex;

    PAGED_CODE();

    PCI_ASSERT(ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort);

    //
    // Check if the downstream switch port is integrated inside a chipset.
    // In that case the system policy on Aspm is applicable to the downstream
    // switch port. So bail out with success.
    //

    SwitchComplex = ExpressBridge->Downstream.u.SwitchPort.SwitchComplex;
    if (SwitchComplex->IntegratedComponent != FALSE) {
        return STATUS_SUCCESS;
    }

    //
    // Check if this bridge was specified in the errata manager or registry as
    // incapable of supporting Aspm.
    //

    if (ExpressBridge->PciBridge->DisableLinkPowerManagement != 0) {
        goto AspmOverrideRequired;
    }

    //
    // The Inf based Opt-in/Opt-out policy is available only after the bridge
    // is truly installed (as opposed to installed via the critical device
    // database). So check for the current install state and examine the
    // Opt-in/Opt-out policy depending on the device version. If the bridge
    // is not in an install state then do not enable Aspm for now.
    //

    BufferLength = sizeof(DEVICE_INSTALL_STATE);
    PhysicalDeviceObject = ExpressBridge->PciBridge->Bus->PhysicalDeviceObject;
    Status = IoGetDeviceProperty(PhysicalDeviceObject,
                                 DevicePropertyInstallState,
                                 BufferLength,
                                 &InstallState,
                                 &BufferLength);

    if (!NT_SUCCESS(Status) || (InstallState != InstallStateInstalled)) {
        goto AspmOverrideRequired;
    }

    //
    // Read the Aspm opt-in or opt-out registry settings for the downstream
    // switch port based on its version.
    //

    if (ExpressBridge->PciBridge->ExpressBaseVersion11OrGreater != 0) {

        //
        // This is a 1.1 device. The default policy for such device is to
        // enable Aspm for its links unless the device opts out thru an INF.
        // So check for the presence of an Opt-out field.
        //

        Result = PciIsDeviceFeatureEnabled(
                    PhysicalDeviceObject->DeviceExtension,
                    PCI_FEATURE_ASPM_OPT_OUT) == PciFeatureStateEnabled;

        if (Result != FALSE) {
            goto AspmOverrideRequired;
        }

    } else {

        //
        // For an express device with version number < 1.1, the default policy
        // is to not enable Aspm unless the device opts in. So check for the
        // Opt-In field.
        //

        Result = PciIsDeviceFeatureEnabled(
                    PhysicalDeviceObject->DeviceExtension,
                    PCI_FEATURE_ASPM_OPT_IN) == PciFeatureStateEnabled;

        if (Result == FALSE) {
            goto AspmOverrideRequired;
        }
    }

    return STATUS_SUCCESS;

AspmOverrideRequired:
    *DisableLinkPowerManagement = TRUE;
    ExpressBridge->Downstream.AspmOverrideFlags |=
        PCI_ASPM_L1_OPTOUT | PCI_ASPM_L0s_OPTOUT;

    return STATUS_SUCCESS;
}

BOOLEAN
ExpressOverrideLinkPowerManagement (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine checks if any device behind the express bridge should not be
    configured for Aspm because it was specified so in the errata manager or
    registry.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    TRUE if the express link should not be configured for power management.

    FALSE otherwise.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    BOOLEAN DisableLinkPowerManagement;

    PAGED_CODE();

    //
    // Check if there is any device behind the bridge with a hack flag for not
    // enabling Aspm on the express link.
    //

    Bus = ExpressBridge->PciBridge->Bus;
    DisableLinkPowerManagement = FALSE;
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (Device = Bus->ChildDevices;
         Device != NULL;
         Device = Device->Sibling) {

        if ((Device->HackFlags & PCI_HACK_NO_ASPM_FOR_EXPRESS_LINK) != 0) {
            DisableLinkPowerManagement = TRUE;
            break;
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);
    return DisableLinkPowerManagement;
}

VOID
ExpressUpdateBaselineLinkPowerManagementSupport (
    _In_ PEXPRESS_BRIDGE ExpressBridge,
    _Inout_ PBOOLEAN EnableL0sOverride,
    _Inout_ PBOOLEAN EnableL1Override
    )

/*++

Routine Description:

    This routine is invoked to determine the baseline Aspm capabilities of a
    multifunction device. If any of the functions in a multifunction device
    cannot enter into L0s or L1 state then none of the functions will be placed
    in that state.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge for a root port or
        downstream switch port.

    EnableL0sOverride - Supplies a placeholder for a boolean for L0s link state
        override.

    EnableL1Override - Supplies a placeholder for a boolean for L1 link state
        override.

Return Value:

    None.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    PEXPRESS_LINK ExpressLink;
    PEXPRESS_PORT ExpressPort;

    PAGED_CODE();

    //
    // Find the common denominator of link state capabilities for all functions
    // in an express device.
    //

    Bus = ExpressBridge->PciBridge->Bus;
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (Device = Bus->ChildDevices;
         Device != NULL;
         Device = Device->Sibling) {

        ExpressPort = Device->ExpressPort;
        if (ExpressPort == NULL) {
            continue;
        }

        ExpressLink = ExpressPort->ExpressLink;
        if (ExpressLink == NULL) {
            continue;
        }

        if (ExpressLink->L0sSupported == FALSE) {
            *EnableL0sOverride = TRUE;
        }

        if (ExpressLink->L1Supported == FALSE) {
            *EnableL1Override = TRUE;
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);
    return;
}

VOID
ExpressUpdateBaselineVersion (
    __in PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine is invoked to find the baseline version of a multifunction
    device. A multifunction device may have some 1.1 and some pre-1.1 devices
    mixed together. In that case, this routine will examine all functions on
    the express bridge, check if there is any device of pre-1.1 era and assume
    a baseline version of pre-1.1 for all functions on this device.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge for a root port or
        a downstream switch port.

Return Value:

    None.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    ULONG DevicesWithVersion11;
    ULONG DevicesBelowVersion11;
    PEXPRESS_PORT ExpressPort;

    PAGED_CODE();

    //
    // In the first pass find the number of fuctions that comply with 1.1
    // and ones that do not.
    //

    Bus = ExpressBridge->PciBridge->Bus;
    DevicesWithVersion11 = 0;
    DevicesBelowVersion11 = 0;
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (Device = Bus->ChildDevices;
         Device != NULL;
         Device = Device->Sibling) {

        ExpressPort = Device->ExpressPort;
        if (ExpressPort == NULL) {
            continue;
        }

        if (ExpressPort->BaseVersion11OrGreater != 0) {
            DevicesWithVersion11 += 1;

        } else {
            DevicesBelowVersion11 += 1;
        }
    }

    //
    // Check if there is a requirement for an override. In none then return
    // immediately.
    //

    if ((DevicesWithVersion11 == 0) || (DevicesBelowVersion11 == 0)) {
        goto Exit;
    }

    //
    // In the second pass, update the information on baseline version for every
    // function in an express device. This governs the link states that each of
    // these functions can enter to match the power policy in play.
    //

    for (Device = Bus->ChildDevices;
         Device != NULL;
         Device = Device->Sibling) {

        ExpressPort = Device->ExpressPort;
        if (ExpressPort == NULL) {
            continue;
        }

        ExpressPort->BaseVersion11OrGreater = 0;
    }

Exit:
    PciReleasePassiveLock(&Bus->ChildDeviceLock);
    return;
}

BOOLEAN
ExpressPortFindOptInOptOutPolicy (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked to find the opt-in/opt-out policy for an express
    port on link power management.

Arguments:

    ExpressPort - Supplies a pointer to an express port.

Return Value:

    TRUE if an express port should not be enabled for Aspm.

    FALSE otherwise.

--*/

{

    ULONG BufferLength;
    PPCI_DEVICE Device;
    BOOLEAN DisableLinkPowerManagement;
    DEVICE_INSTALL_STATE InstallState;
    BOOLEAN Result;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Initialize a default policy of no link power management for express
    // links.
    //

    DisableLinkPowerManagement = TRUE;

    //
    // Check if the device is in some critical device installation state.
    // If so then do not enable Aspm for the links until the device is in
    // a fully installed state.
    //

    BufferLength = sizeof(DEVICE_INSTALL_STATE);
    Device = ExpressPort->Device;
    Status = IoGetDeviceProperty(Device->DeviceObject,
                                 DevicePropertyInstallState,
                                 BufferLength,
                                 &InstallState,
                                 &BufferLength);

    if (!NT_SUCCESS(Status) || (InstallState != InstallStateInstalled)) {

        //
        // The device will not be configured for link power management.
        //

        return DisableLinkPowerManagement;
    }

    //
    // Examine the Opt-in/Opt-out policy based on the version number of the
    // device.
    //

    if (ExpressPort->BaseVersion11OrGreater != 0) {

        //
        // This is a 1.1 device. The default policy for such device is to
        // enable ASPM for its links unless the device opts out thru an INF.
        //

        Result =
            PciIsDeviceFeatureEnabled(Device, PCI_FEATURE_ASPM_OPT_OUT) == PciFeatureStateEnabled;
        if (Result == FALSE) {
            DisableLinkPowerManagement = FALSE;
        }

    } else {

        //
        // For an express device with version number < 1.1, the default policy
        // is to not enable ASPM unless the device opts in.
        //

        Result =
            PciIsDeviceFeatureEnabled(Device, PCI_FEATURE_ASPM_OPT_IN) == PciFeatureStateEnabled;
        if (Result != FALSE) {
            DisableLinkPowerManagement = FALSE;
        }
    }

    return DisableLinkPowerManagement;
}

NTSTATUS
ExpressLinkDisableL0sL1State (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This callback routine is invoked to disable L0s and L1 link states on an
    express link. The state changes based on the current policy and the upstream
    or downstream link state capabilities.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    STATUS_SUCCESS

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    PEXPRESS_PORT ExpressPort;
    NTSTATUS Status;

    NON_PAGED_CODE();

    Bus = ExpressBridge->PciBridge->Bus;
    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        goto Exit;
    }

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    for (Device = Bus->ChildDevices; Device != NULL; Device = Device->Sibling) {
        Status = IoAcquireRemoveLock(&Device->RemoveLock, NULL);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        ExpressPort = Device->ExpressPort;
        if ((ExpressPort == NULL) || (ExpressPort->ExpressLink == NULL)) {
            IoReleaseRemoveLock(&Device->RemoveLock, NULL);
            continue;
        }

        //
        // Make sure to disable downstream link before upstream link.
        //

        ExpressPortDisableL0sL1OnDownstreamLink(ExpressPort);
        IoReleaseRemoveLock(&Device->RemoveLock, NULL);
    }

    ExpressBridgeDisableL0sL1OnUpstreamLink(ExpressBridge,
                                            &ExpressBridge->Downstream.ExpressLink);

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

Exit:
    return STATUS_SUCCESS;
}

NTSTATUS
ExpressLinkEnableL0sState (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This callback routine is invoked to place the links in L0s state. On exit
    from this function:

    - If the upstream and all downstream ports support L0s, it will be enabled.
      Otherwise L0s will be left disabled on all ports.
    - L1 will be disabled on all ports.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    BOOLEAN EnableL0s;
    PEXPRESS_LINK ExpressLink;
    PEXPRESS_PORT ExpressPort;
    PCI_EXPRESS_ASPM_CONTROL LinkPowerState;
    NTSTATUS Status;

    NON_PAGED_CODE();

    Bus = ExpressBridge->PciBridge->Bus;
    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        goto Exit;
    }

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    if ((ExpressBridge->Downstream.ExpressLink.L0sSupported == FALSE) ||
        (ExpressBridge->Downstream.AspmL0sOverride != FALSE) ||
        (Bus->ChildDevices == NULL)) {

        //
        // If L0s isn't supported by the parent, or has been overridden
        // in the device settings, or there are no children, don't enable it.
        //

        EnableL0s = FALSE;

    } else {

        //
        // Otherwise iterate through all children to verify that they
        // all support L0s.
        //

        EnableL0s = TRUE;
        for (Device = Bus->ChildDevices; Device != NULL; Device = Device->Sibling) {

            //
            // Determine if this child supports L0s. If anything goes wrong,
            // assume L0s should be disabled.
            //

            Status = IoAcquireRemoveLock(&Device->RemoveLock, NULL);
            if ((!NT_SUCCESS(Status)) ||
                (Device->ExpressPort == NULL) ||
                (Device->ExpressPort->ExpressLink == NULL) ||
                (Device->ExpressPort->ExpressLink->L0sSupported == FALSE)) {

                EnableL0s = FALSE;
            }

            //
            // Remember to release the lock if we had succeeded in grabbing it.
            //

            if (NT_SUCCESS(Status)) {
                IoReleaseRemoveLock(&Device->RemoveLock, NULL);
            }

            //
            // No need to continue enumerating children if we already failed.
            //

            if (EnableL0s == FALSE) {
                break;
            }
        }
    }

    //
    // Configure the L0s state and disable the L1 state for the end of the link
    // owned by the downstream device.  This is done for each function of the
    // device.
    //

    for (Device = Bus->ChildDevices; Device != NULL; Device = Device->Sibling) {
        Status = IoAcquireRemoveLock(&Device->RemoveLock, NULL);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        ExpressPort = Device->ExpressPort;
        if (ExpressPort == NULL) {
            IoReleaseRemoveLock(&Device->RemoveLock, NULL);
            continue;
        }

        ExpressLink = ExpressPort->ExpressLink;
        if (ExpressLink == NULL) {
            IoReleaseRemoveLock(&Device->RemoveLock, NULL);
            continue;
        }

        LinkPowerState = 0;
        if ((ExpressLink->L0sEnabled == FALSE) ||
            (ExpressLink->L1Enabled != FALSE)) {

            ExpressLink->L0sEnabled = EnableL0s;
            if(EnableL0s != FALSE) {
                LinkPowerState |= L0sEntryEnabled;
            }

            ExpressLink->L1Enabled = FALSE;
            ExpressLink->LinkControl.ActiveStatePMControl = LinkPowerState;
            PciWriteExpressRegister(ExpressPort,
                                    LinkControl,
                                    &ExpressLink->LinkControl);

        }

        IoReleaseRemoveLock(&Device->RemoveLock, NULL);
    }

    //
    // Enable the L0s state and disable the L1 state in the end of the link
    // owned by the downstream switchport.  The L0s state will not be enabled
    // if it has been overridden in the device settings.
    //

    LinkPowerState = 0;
    if ((ExpressBridge->Downstream.ExpressLink.L0sEnabled == FALSE) ||
        (ExpressBridge->Downstream.ExpressLink.L1Enabled != FALSE)) {

        if (EnableL0s != FALSE) {
            LinkPowerState |= L0sEntryEnabled;
            ExpressBridge->Downstream.ExpressLink.L0sEnabled = TRUE;
        }

        ExpressBridge->Downstream.ExpressLink.L1Enabled = FALSE;
        ExpressBridge->Downstream.ExpressLink.LinkControl.ActiveStatePMControl = LinkPowerState;
        PciWriteExpressBridgeRegister(ExpressBridge,
                                      LinkControl,
                                      &ExpressBridge->Downstream.ExpressLink.LinkControl);

    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

Exit:
    return STATUS_SUCCESS;
}

NTSTATUS
ExpressLinkEnableL0sL1State (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This callback routine is invoked to place the links in L0s and L1 state
    depending on the link capability and other factors.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    STATUS_SUCCESS

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    BOOLEAN EnableL0sUpstream;
    BOOLEAN EnableL1Upstream;
    PEXPRESS_LINK ExpressLink;
    PEXPRESS_PORT ExpressPort;
    PCI_EXPRESS_ASPM_CONTROL LinkPowerState;
    NTSTATUS Status;

    NON_PAGED_CODE();

    Bus = ExpressBridge->PciBridge->Bus;
    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        goto Exit;
    }

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    //
    // Enable the L0s/L1 link states on the end of the express link owned
    // by the device.  This is done for each function of the device.  Each
    // state is only enabled if it has not been overridden by settings in the
    // device.
    //

    EnableL0sUpstream = FALSE;
    EnableL1Upstream = FALSE;
    for (Device = Bus->ChildDevices; Device != NULL; Device = Device->Sibling) {
        Status = IoAcquireRemoveLock(&Device->RemoveLock, NULL);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        ExpressPort = Device->ExpressPort;
        if (ExpressPort == NULL) {
            IoReleaseRemoveLock(&Device->RemoveLock, NULL);
            continue;
        }

        ExpressLink = ExpressPort->ExpressLink;
        if (ExpressLink == NULL) {
            IoReleaseRemoveLock(&Device->RemoveLock, NULL);
            continue;
        }

        LinkPowerState = 0;
        if ((ExpressLink->L0sSupported != FALSE) &&
            (ExpressBridge->Downstream.ExpressLink.L0sSupported != FALSE) &&
            (ExpressBridge->Downstream.AspmL0sOverride == FALSE) &&
            (ExpressLink->L0sEnabled == FALSE)) {

            LinkPowerState |= L0sEntryEnabled;
            ExpressLink->L0sEnabled = TRUE;
            EnableL0sUpstream = TRUE;

        }

        if ((ExpressBridge->Downstream.ExpressLink.L1Supported != FALSE) &&
            (ExpressLink->L1Supported != FALSE) &&
            (ExpressBridge->Downstream.AspmL1Override == FALSE) &&
            (ExpressLink->L1Enabled == FALSE)) {

            LinkPowerState |= L1EntryEnabled;
            ExpressLink->L1Enabled = TRUE;
            EnableL1Upstream = TRUE;
        }

        if (LinkPowerState != 0) {
            ExpressLink->LinkControl.ActiveStatePMControl |= LinkPowerState;
            PciWriteExpressRegister(ExpressPort,
                                    LinkControl,
                                    &ExpressLink->LinkControl);
        }

        IoReleaseRemoveLock(&Device->RemoveLock, NULL);
    }

    //
    // Enable the L0s/L1 link states on the end of the express link owned
    // by the downstream switchport.  Each state will only be enabled if it has
    // not been overridden by settings in the device.
    //

    LinkPowerState = 0;
    if (EnableL0sUpstream != FALSE) {
        LinkPowerState |= L0sEntryEnabled;
        ExpressBridge->Downstream.ExpressLink.L0sEnabled = TRUE;
    }

    if (EnableL1Upstream != FALSE) {
        LinkPowerState |= L1EntryEnabled;
        ExpressBridge->Downstream.ExpressLink.L1Enabled = TRUE;
    }

    if (LinkPowerState != 0) {
        ExpressBridge->Downstream.ExpressLink.LinkControl.ActiveStatePMControl |= LinkPowerState;
        PciWriteExpressBridgeRegister(ExpressBridge,
                                      LinkControl,
                                      &ExpressBridge->Downstream.ExpressLink.LinkControl);
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

Exit:
    return STATUS_SUCCESS;
}

// ---------------------------------------------------- Routines for link states

VOID
ExpressBridgeDisableL0sL1OnUpstreamLink (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in PEXPRESS_LINK UpstreamLink
    )

/*++

Routine Description:

    This routine is invoked to disable Aspm on an upstream link (that end of a
    link that is closer to the root bus).

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    None.

--*/

{

    NON_PAGED_CODE();

    UpstreamLink->LinkControl.ActiveStatePMControl = L0sAndL1EntryDisabled;
    PciWriteExpressBridgeRegister(ExpressBridge,
                                  LinkControl,
                                  &UpstreamLink->LinkControl);

    UpstreamLink->L0sEnabled = FALSE;
    UpstreamLink->L1Enabled = FALSE;
    return;
}

VOID
ExpressPortDisableL0sL1OnDownstreamLink (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked to disable Aspm on a downstream link (ingress link)
    i.e. the end of an express link that is further away from the root port.

Arguments:

    ExpressPort - Supplies a pointer to an express port for a device.

Return Value:

    None.

--*/

{

    PEXPRESS_LINK ExpressLink;

    NON_PAGED_CODE();

    ExpressLink = ExpressPort->ExpressLink;
    if (ExpressLink == NULL) {
        return;
    }

    ExpressLink->LinkControl.ActiveStatePMControl = L0sAndL1EntryDisabled;
    PciWriteExpressRegister(ExpressPort,
                            LinkControl,
                            &ExpressLink->LinkControl);

    ExpressLink->L0sEnabled = FALSE;
    ExpressLink->L1Enabled = FALSE;
    return;
}

NTSTATUS
ExpressUpdateLinkPowerManagementSupport (
    __inout PEXPRESS_LINK ExpressLink
    )

/*++

Routine Description:

    This routine is invoked to examine the link capabilities and cache the
    extent of Aspm support that is available on the link.

Arguments:

    ExpressLink - Supplies a pointer to an express link.

Return Value:

    None.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    if (ExpressLink->LinkCapabilities.AspmOptionalityCompliance != 0) {
        switch (ExpressLink->LinkCapabilities.ActiveStatePMSupport) {
        case NoAspmSupport:
            Status = STATUS_NOT_SUPPORTED;
            break;

        case L0sEntrySupport:
            ExpressLink->L0sSupported = TRUE;
            break;

        case L1EntrySupport:
            ExpressLink->L1Supported = TRUE;
            break;

        case L0sAndL1EntrySupport:
            ExpressLink->L0sSupported = TRUE;
            ExpressLink->L1Supported = TRUE;
            break;

        default:
            Status = STATUS_NOT_SUPPORTED;
            break;
        }

    } else {
        switch (ExpressLink->LinkCapabilities.ActiveStatePMSupport) {
        case L0sEntrySupport:
            ExpressLink->L0sSupported = TRUE;
            break;

        case L0sAndL1EntrySupport:
            ExpressLink->L0sSupported = TRUE;
            ExpressLink->L1Supported = TRUE;
            break;

        default:
            Status = STATUS_NOT_SUPPORTED;
            break;
        }
    }

    return Status;
}

// -------------------------------------------- Routines for latency computation

NTSTATUS
ExpressSwitchUpdateLinkLatency (
    __inout PSWITCH_COMPLEX SwitchComplex
    )

/*++

Routine Description:

    This routine is invoked to compute the link latencies for a switch complex.

    L0s link latency:
    For a switch complex linked to a root port bridge, the latency is computed
    by taking a sum of receive and transmit lane latencies for the root port
    and the upstream switch port at the two ends of an express link. For a
    switch complex linked to a switch complex on the upstream side, the latency
    is computed by taking a max of the parent switch complex and the sum of the
    receive and transmit lane exit latencies of the upstream switch port for the
    current switch complex.

    L1 link latency:
    For a switch complex linked to a root port bridge, the latency is computed
    by taking a max of exit latencies for the root port and the upstream switch
    port at the two ends of an express link. For a switch complex linked to
    a switch complex on the upstream side, the latency is computed by taking
    a max of exit latencies for a dowstream switch port of the parent switch
    complex, upstream switch port for the given switch complex and latency of
    the switch complex on the upstream side padded with 1000 us for the delay
    in start of transition (to account for an extra hop).

Arguments:

    SwitchComplex - Supplies a pointer to a switch complex.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PPCI_BUS Bus;
    PEXPRESS_BRIDGE ParentExpressBridge;
    PEXPRESS_LINK ParentExpressLink;
    PSWITCH_COMPLEX ParentSwitchComplex;
    PEXPRESS_BRIDGE UpstreamSwitchPortBridge;

    PAGED_CODE();

    if (PciSystemWideHackFlags & PCI_SYS_HACK_DISABLE_EXPRESS_ASPM) {
        return STATUS_SUCCESS;
    }

    UpstreamSwitchPortBridge = SwitchComplex->UpstreamPort;
    ParentExpressBridge = UpstreamSwitchPortBridge->Parent;
    if (ParentExpressBridge->BridgeType == PciExpressRootPort) {
        Bus = ParentExpressBridge->PciBridge->Bus;
        ParentExpressLink = &ParentExpressBridge->Downstream.ExpressLink;
        PciAcquirePassiveLock(&Bus->ChildDeviceLock);

        PCI_ASSERT(ParentExpressLink->TargetLink != NULL);

        //
        // Compute the L0s latency for this switch.
        //

        if (ParentExpressLink->L0sSupported != FALSE) {
            SwitchComplex->ComputedL0sReceiveLatency =
                ParentExpressLink->L0sExitLatencyHigh;

        }

        if (ParentExpressLink->TargetLink->L0sSupported != FALSE) {
            SwitchComplex->ComputedL0sTransmitLatency =
                ParentExpressLink->TargetLink->L0sExitLatencyHigh;

        }

        //
        // Compute the L1 latency for this switch.
        //

        if ((ParentExpressLink->L1Supported != FALSE) &&
            (ParentExpressLink->TargetLink->L1Supported != FALSE)) {

            SwitchComplex->ComputedL1Latency =
                max(ParentExpressLink->L1ExitLatencyHigh,
                    ParentExpressLink->TargetLink->L1ExitLatencyHigh);
        }

        PciReleasePassiveLock(&Bus->ChildDeviceLock);

    } else {

        PCI_ASSERT(ParentExpressBridge->BridgeType ==
                   PciExpressDownstreamSwitchPort);

        Bus = ParentExpressBridge->PciBridge->Bus;
        ParentExpressLink =
            &ParentExpressBridge->Downstream.ExpressLink;

        ParentSwitchComplex =
            ParentExpressBridge->Downstream.u.SwitchPort.SwitchComplex;

        PciAcquirePassiveLock(&Bus->ChildDeviceLock);

        PCI_ASSERT(ParentExpressLink->TargetLink != NULL);

        //
        // Compute the L0s latency for this switch.
        //

        if (ParentExpressLink->L0sSupported != FALSE) {
            SwitchComplex->ComputedL0sReceiveLatency =
                max(ParentSwitchComplex->ComputedL0sReceiveLatency,
                    ParentExpressLink->L0sExitLatencyHigh);

        }

        if (ParentExpressLink->TargetLink->L0sSupported != FALSE) {
            SwitchComplex->ComputedL0sTransmitLatency =
                max(ParentSwitchComplex->ComputedL0sTransmitLatency,
                    ParentExpressLink->TargetLink->L0sExitLatencyHigh);

        }

        //
        // Compute the L1 latency for this switch.
        //

        if ((ParentExpressLink->L1Supported != FALSE) &&
            (ParentExpressLink->TargetLink->L1Supported != FALSE)) {

            SwitchComplex->ComputedL1Latency =
                max(ParentExpressLink->L1ExitLatencyHigh,
                    ParentExpressLink->TargetLink->L1ExitLatencyHigh);

        }

        if (ParentSwitchComplex->ComputedL1Latency != 0) {
            SwitchComplex->ComputedL1Latency =
                max(ParentSwitchComplex->ComputedL1Latency + 1000,
                    SwitchComplex->ComputedL1Latency);

        }

        PciReleasePassiveLock(&Bus->ChildDeviceLock);
    }

    return STATUS_SUCCESS;
}

VOID
ExpressUpdateReceiveLaneExitLatency (
    __inout PEXPRESS_LINK ExpressLink
    )

/*++

Routine Description:

    This routine is invoked to update the receive lane exit latencies for an
    express link.

Arguments:

    ExpressLink - Supplies a pointer to an express link.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    if (ExpressLink->L0sSupported != FALSE) {
        switch (ExpressLink->LinkCapabilities.L0sExitLatency) {
        case L0s_Below64ns:
            ExpressLink->L0sExitLatencyLow = 0;
            ExpressLink->L0sExitLatencyHigh = 64;
            break;

        case L0s_64ns_128ns:
            ExpressLink->L0sExitLatencyLow = 64;
            ExpressLink->L0sExitLatencyHigh = 128;
            break;

        case L0s_128ns_256ns:
            ExpressLink->L0sExitLatencyLow = 128;
            ExpressLink->L0sExitLatencyHigh = 256;
            break;

        case L0s_256ns_512ns:
            ExpressLink->L0sExitLatencyLow = 256;
            ExpressLink->L0sExitLatencyHigh = 512;
            break;

        case L0s_512ns_1us:
            ExpressLink->L0sExitLatencyLow = 512;
            ExpressLink->L0sExitLatencyHigh = 1000;
            break;

        case L0s_1us_2us:
            ExpressLink->L0sExitLatencyLow = 1000;
            ExpressLink->L0sExitLatencyHigh = 2000;
            break;

        case L0s_2us_4us:
            ExpressLink->L0sExitLatencyLow = 2000;
            ExpressLink->L0sExitLatencyHigh = 4000;
            break;

        case L0s_Above4us:
            ExpressLink->L0sExitLatencyLow = 4000;
            ExpressLink->L0sExitLatencyHigh = 8000;
            break;

        default:

            PCI_ASSERT(FALSE);

            break;
        }
    }

    if (ExpressLink->L1Supported != FALSE) {
        switch (ExpressLink->LinkCapabilities.L1ExitLatency) {
        case L1_Below1us:
            ExpressLink->L1ExitLatencyLow = 0;
            ExpressLink->L1ExitLatencyHigh = 1000;
            break;

        case L1_1us_2us:
            ExpressLink->L1ExitLatencyLow = 1000;
            ExpressLink->L1ExitLatencyHigh = 2000;
            break;

        case L1_2us_4us:
            ExpressLink->L1ExitLatencyLow = 2000;
            ExpressLink->L1ExitLatencyHigh = 4000;
            break;

        case L1_4us_8us:
            ExpressLink->L1ExitLatencyLow = 4000;
            ExpressLink->L1ExitLatencyHigh = 8000;
            break;

        case L1_8us_16us:
            ExpressLink->L1ExitLatencyLow = 8000;
            ExpressLink->L1ExitLatencyHigh = 16000;
            break;

        case L1_16us_32us:
            ExpressLink->L1ExitLatencyLow = 16000;
            ExpressLink->L1ExitLatencyHigh = 32000;
            break;

        case L1_32us_64us:
            ExpressLink->L1ExitLatencyLow = 32000;
            ExpressLink->L1ExitLatencyHigh = 64000;
            break;

        case L1_Above64us:
            ExpressLink->L1ExitLatencyLow = 64000;
            ExpressLink->L1ExitLatencyHigh = 128000;
            break;

        default:

            PCI_ASSERT(FALSE);

            break;
        }
    }

    return;
}

NTSTATUS
ExpressPortUpdateLatencyBound (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine is invoked to update the acceptable L0s and L1 latency bounds
    for an express endpoint.

Arguments:

    ExpressPort - Supplies a pointer to an express port for a device.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PAGED_CODE();

    PCI_ASSERT(ExpressPort->ExpressLink != NULL);

    if (ExpressPort->ExpressLink->L0sSupported != FALSE) {
        switch (ExpressPort->DeviceCapabilities.L0sAcceptableLatency) {
        case L0s_Below64ns:
            ExpressPort->AcceptableL0sLatency = 64;
            break;

        case L0s_64ns_128ns:
            ExpressPort->AcceptableL0sLatency = 128;
            break;

        case L0s_128ns_256ns:
            ExpressPort->AcceptableL0sLatency = 256;
            break;

        case L0s_256ns_512ns:
            ExpressPort->AcceptableL0sLatency = 512;
            break;

        case L0s_512ns_1us:
            ExpressPort->AcceptableL0sLatency = 1000;
            break;

        case L0s_1us_2us:
            ExpressPort->AcceptableL0sLatency = 2000;
            break;

        case L0s_2us_4us:
            ExpressPort->AcceptableL0sLatency = 4000;
            break;

        case L0s_Above4us:
            ExpressPort->AcceptableL0sLatency = MAXULONG;
            break;

        default:

            PCI_ASSERT(FALSE);

            break;
        }
    }

    if (ExpressPort->ExpressLink->L1Supported != FALSE) {
        switch (ExpressPort->DeviceCapabilities.L1AcceptableLatency) {
        case L1_Below1us:
            ExpressPort->AcceptableL1Latency = 1000;
            break;

        case L1_1us_2us:
            ExpressPort->AcceptableL1Latency = 2000;
            break;

        case L1_2us_4us:
            ExpressPort->AcceptableL1Latency = 4000;
            break;

        case L1_4us_8us:
            ExpressPort->AcceptableL1Latency = 8000;
            break;

        case L1_8us_16us:
            ExpressPort->AcceptableL1Latency = 16000;
            break;

        case L1_16us_32us:
            ExpressPort->AcceptableL1Latency = 32000;
            break;

        case L1_32us_64us:
            ExpressPort->AcceptableL1Latency = 64000;
            break;

        case L1_Above64us:
            ExpressPort->AcceptableL1Latency = MAXULONG;
            break;

        default:

            PCI_ASSERT(FALSE);

            break;
        }
    }

    return STATUS_SUCCESS;
}

// ------------------------------------------------------ Miscellaneous routines

NTSTATUS
ExpressBridgeEnableCommonClockConfiguration (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __inout PEXPRESS_LINK UpstreamLink
    )

/*++

Routine Description:

    This routine enables common clock configuration for components at the two
    ends of an express link.

Arguments:

    ExpressBridge - Supplies a pointer to an Express Bridge.

    UpstreamLink - Supplies a pointer to an upstream end of an Express Link.

Return Value:

    STATUS_NOT_SUPPORTED if the components at the two ends of an express link
        do no support common clock configuration.

    STATUS_SUCCESS on success.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    PEXPRESS_LINK DownstreamLink;

    PAGED_CODE();

    PCI_ASSERT((ExpressBridge->BridgeType == PciExpressRootPort) ||
               (ExpressBridge->BridgeType == PciExpressDownstreamSwitchPort));

    Bus = ExpressBridge->PciBridge->Bus;
    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    DownstreamLink = UpstreamLink->TargetLink;
    if (DownstreamLink == NULL) {
        goto Exit;
    }

    //
    // If the components at the two ends of an express link do not use
    // common clock connection, then bail out as there is nothing to do
    // here.
    //

    if ((UpstreamLink->LinkStatus.SlotClockConfig == 0) ||
        (DownstreamLink->LinkStatus.SlotClockConfig == 0)) {

        goto Exit;
    }

    //
    // Set the common clock config bit for an upstream link.
    //

    UpstreamLink->LinkControl.CommonClockConfig = 1;
    PciWriteExpressBridgeRegister(ExpressBridge,
                                  LinkControl,
                                  &UpstreamLink->LinkControl);

    //
    // Do the same for downstream express link. If this is a multi-function
    // device then there will be more than one set of link registers where
    // each set corresponds to one function.
    //

    for (Device = Bus->ChildDevices; Device != NULL; Device = Device->Sibling) {

        PCI_ASSERT(Device->ExpressPort != NULL);

        DownstreamLink = Device->ExpressPort->ExpressLink;
        if (DownstreamLink == NULL) {
            continue;
        }

        PCI_ASSERT(DownstreamLink->LinkStatus.SlotClockConfig != 0);

        DownstreamLink->LinkControl.CommonClockConfig = 1;
        PciWriteExpressRegister(Device->ExpressPort,
                                LinkControl,
                                &DownstreamLink->LinkControl);

    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // Now retrain the link.
    //

    ExpressBridgeRetrainLink(ExpressBridge);

    //
    // The common clock configuration is now complete. This may influence
    // certain link capabilities. So read the link capabilities register and
    // cache them for later processing.
    //

    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkCapabilities,
                                 &UpstreamLink->LinkCapabilities);

    return STATUS_SUCCESS;

Exit:
    PciReleasePassiveLock(&Bus->ChildDeviceLock);
    return STATUS_NOT_SUPPORTED;
}

NTSTATUS
ExpressBridgeRetrainLink (
    __inout PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine is invoked to retrain the express link for an express bridge.
    This is typically invoked after common clock configuraton as part of restore
    during power up.

Arguments:

    ExpressBridge - Supplies a pointer to an express bridge.

Return Value:

    STATUS_SUCCESS.

--*/

{



    PCI_EXPRESS_LINK_CONTROL_REGISTER ExpressLinkControlRegister;
    PEXPRESS_LINK UpstreamLink;

    NON_PAGED_CODE();

    if (!DOWNSTREAM_PORT(ExpressBridge)) {
        return STATUS_SUCCESS;
    }

    UpstreamLink = &ExpressBridge->Downstream.ExpressLink;

    PCI_ASSERT(UpstreamLink->TargetLink != NULL);

    if (UpstreamLink->LinkControl.CommonClockConfig == 0) {
        return STATUS_SUCCESS;
    }

    //
    // Cache the current link control register settings for this bridge.
    //

    RtlCopyMemory(&ExpressLinkControlRegister,
                  &UpstreamLink->LinkControl,
                  sizeof(PCI_EXPRESS_LINK_CONTROL_REGISTER));

    //
    // Before the link retrain, ASPM needs to be disabled.  This is a work
    // around for hardware that does not properly handle retraining with ASPM
    // enabled. A link may enter into L1 state and some endpoints cannot recover
    // from this condition. First disable ASPM on the downstream link.
    //

    ExpressEnableDisableDownstreamLinkStatesForRetrain(ExpressBridge, FALSE);

    //
    // Now, disable ASPM on the upstream link.
    //

    ExpressLinkControlRegister.ActiveStatePMControl = L0sAndL1EntryDisabled;
    PciWriteExpressBridgeRegister(ExpressBridge,
                                  LinkControl,
                                  &ExpressLinkControlRegister);

    //
    // Stall before doing the link retrain.
    //

    ExpressBridgeStallForLinkRetrain(ExpressBridge);

    //
    // Perform the retrain.
    //

    ExpressLinkControlRegister.RetrainLink = 1;
    PciWriteExpressBridgeRegister(ExpressBridge,
                                  LinkControl,
                                  &ExpressLinkControlRegister);

    //
    // Stall one more time before re-enabling ASPM.
    //

    ExpressBridgeStallForLinkRetrain(ExpressBridge);

    //
    // Restore the upstream and downstream link state settings.
    //

    PciWriteExpressBridgeRegister(ExpressBridge,
                                  LinkControl,
                                  &UpstreamLink->LinkControl);

    ExpressEnableDisableDownstreamLinkStatesForRetrain(ExpressBridge, TRUE);
    return STATUS_SUCCESS;
}

VOID
ExpressEnableDisableDownstreamLinkStatesForRetrain (
    __inout PEXPRESS_BRIDGE ExpressBridge,
    __in BOOLEAN EnableLinkStates
    )

/*++

Routine Description:

    This routine is invoked to enable/disable link states for the duration of
    link retraining operation.

Arguments:

    ExpressBridge - Supplies a pointer to the Express Bridge that is the
        target for link retraining.

    EnableLinkStates - Supplies a boolean controlling the action.

Return Value:

    None.

--*/

{

    PPCI_BUS Bus;
    PPCI_DEVICE Device;
    PEXPRESS_LINK ExpressLink;
    PEXPRESS_PORT ExpressPort;
    PCI_EXPRESS_LINK_CONTROL_REGISTER ExpressLinkControlRegister;
    NTSTATUS Status;

    NON_PAGED_CODE();

    Bus = ExpressBridge->PciBridge->Bus;
    for (Device = Bus->ChildDevices; Device != NULL; Device = Device->Sibling) {
        Status = IoAcquireRemoveLock(&Device->RemoveLock, NULL);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        ExpressPort = Device->ExpressPort;
        if (ExpressPort == NULL) {
            IoReleaseRemoveLock(&Device->RemoveLock, NULL);
            continue;
        }

        ExpressLink = ExpressPort->ExpressLink;
        if (ExpressLink == NULL) {
            IoReleaseRemoveLock(&Device->RemoveLock, NULL);
            continue;
        }

        if (EnableLinkStates != FALSE) {
            PciWriteExpressRegister(ExpressPort,
                                    LinkControl,
                                    &ExpressLink->LinkControl);

        } else {
            RtlCopyMemory(&ExpressLinkControlRegister,
                          &ExpressLink->LinkControl,
                          sizeof(PCI_EXPRESS_LINK_CONTROL_REGISTER));

            ExpressLinkControlRegister.ActiveStatePMControl =
                L0sAndL1EntryDisabled;

            PciWriteExpressRegister(ExpressPort,
                                    LinkControl,
                                    &ExpressLinkControlRegister);
        }

        IoReleaseRemoveLock(&Device->RemoveLock, NULL);
    }

    return;
}

VOID
ExpressBridgeStallForLinkRetrain (
    __in PEXPRESS_BRIDGE ExpressBridge
    )

/*++

Routine Description:

    This routine is used to stall during an express link retrain.  This is
    done to ensure an existing retrain isn't already in progress.

Arguments:

    ExpressBridge - Supplies a pointer to the Express Bridge that is the
        target for link retraining.

Return Value:

    None.

--*/

{

    PCI_EXPRESS_LINK_STATUS_REGISTER ExpressBridgeLinkStatus;

    NON_PAGED_CODE();

    RtlZeroMemory(&ExpressBridgeLinkStatus,
                  sizeof(PCI_EXPRESS_LINK_STATUS_REGISTER));

    //
    // Read the link status register.
    //

    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkStatus,
                                 &ExpressBridgeLinkStatus);

    //
    // If already clear then return success.
    //

    if (ExpressBridgeLinkStatus.LinkTraining == 0) {
        return;
    }

    //
    // Stall for 100 microseconds and then reread the register.
    //

    ExpressBridge->PciBridge->StallDispatch(ExpressBridge->PciBridge->InterfaceContext, 100);
    PciReadExpressBridgeRegister(ExpressBridge,
                                 LinkStatus,
                                 &ExpressBridgeLinkStatus);

    return;
}

_Function_class_(POWER_SETTING_CALLBACK)
_IRQL_requires_same_
NTSTATUS
ExpressLinkPowerSchemeCallback (
    _In_ LPCGUID SettingGuid,
    _In_reads_bytes_(ValueLength) PVOID NewValue,
    _In_ ULONG ValueLength,
    _Inout_opt_ PVOID Context
    )

/*++

Routine Description:

    This callback routine is invoked to apply the new Aspm policy whenever the
    policy changes. The Aspm state changes based on the current policy and
    upstream/downstream link state capabilities.

Arguments:

    SettingGuid - Supplies a pointer to a GUID for the power policy.

    NewValue - Supplies a pointer to the value for the power policy.

    ValueLength - Supplies the length of the NewValue buffer in bytes.

    Context - Supplies a pointer to an express bridge.

Return Value:

    STATUS_UNSUCCESSFUL when passed in parameters are invalid.

    STATUS_DELETE_PENDING on failure to acquire a remove lock.

    Or a status code returned from one of the functions in this call.

--*/

{

    PPCI_BUS Bus;
    PEXPRESS_BRIDGE ExpressBridge;
    NTSTATUS Status;
    PULONG Value;

    PAGED_CODE();

    //
    // Sanity check.
    //

    if ((IsEqualGUID(&GUID_PCIEXPRESS_ASPM_POLICY, SettingGuid) == FALSE) ||
        (NewValue == NULL) ||
        (ValueLength != sizeof(ULONG))) {

        return STATUS_UNSUCCESSFUL;
    }

    Value = (PULONG)NewValue;
    ExpressBridge = (PEXPRESS_BRIDGE)Context;
    _Analysis_assume_(ExpressBridge != NULL);
    Bus = ExpressBridge->PciBridge->Bus;

    //
    // As this routine will touch config space on both the
    // bridge and the child, take a power reference so the 
    // child will not be in D3Cold.
    //

    PciBus_PowerReference(Bus, PciBusPowerReasonPowerSchemeCallbackPending);

    //
    // Acquire a remove lock on the bus so that the remove Irp processing is
    // serialized for the bridge.
    //

    Status = IoAcquireRemoveLock(&Bus->RemoveLock, NULL);
    if (!NT_SUCCESS(Status)) {
        PciBus_PowerDereference(Bus, PciBusPowerReasonPowerSchemeCallbackPending);
        return Status;
    }

    switch (*Value) {
    case PCIEXPRESS_ASPM_DISABLE_POLICY:
        ExpressLinkDisableL0sL1State(ExpressBridge);
        break;

    case PCIEXPRESS_ASPM_L0S_POLICY:
        ExpressLinkEnableL0sState(ExpressBridge);
        break;

    case PCIEXPRESS_ASPM_L0SL1_POLICY:
        ExpressLinkEnableL0sL1State(ExpressBridge);
        break;

    default:

        PCI_ASSERT(FALSE);

        break;
    }

    IoReleaseRemoveLock(&Bus->RemoveLock, NULL);
    PciBus_PowerDereference(Bus, PciBusPowerReasonPowerSchemeCallbackPending);
    return STATUS_SUCCESS;
}
