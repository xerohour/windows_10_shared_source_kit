/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ltr.c

Abstract:

    This module contains functions associated with configuring the
    Latency Tolerance Reporting and OBFF PCI Capabilies.  These two things,
    taken together, allow a PCIe endpoint to coordinate its DMA access with the
    chipset power management engine which controls CPU states.  When everything
    is working as planned, the devices report their latency tolerances upstream
    and the chipset sends signals (either messages or wake# switching)
    downstream, with the end result that the endpoint only does DMA when the CPU
    caches are powered up and working for other reasons as well.
 
    Changes in latency tolerance usually imply that internal buffers are filling
    up or emptying.  The LTR cap struct is a way for the BIOS to indicate to
    the device (often via the OS) that certain values of latency aren't really
    interesting and the device can skip some updates.  It's incumbent on the
    chipset to watch the latency updates that are sent up the fabric and to
    send OBFF messages downstream before the device is about to overrun its
    buffers.
 
    Opportunistic Buffer Flush/Fill (OBFF) tells the downstream device when
    good times for DMA arise.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <initguid.h>
#include "ntpoapi.h"
#include "ltr.tmh"

// ------------------------------------------------------------------ Prototypes

// ------------------------------------------------------------------- Functions

#pragma alloc_text(PAGE, ExpressInitializeLtrValue)
#pragma alloc_text(PAGE, ExpressInitializeObffValue)
#pragma alloc_text(PAGE, ExpressPortGetLtrValueForExpressBridge)
#pragma alloc_text(PAGE, ExpressPortGetObffValueForExpressBridge)
#pragma alloc_text(PAGE, ExpressBridgeCalculateLtrValues)

NTSTATUS
ExpressSaveLtrCapability (
    _Inout_ PEXPRESS_PORT ExpressPort,
    _Inout_ PPCI_EXPRESS_LTR_CAPABILITY CapabilityTemp,
    _In_ BOOLEAN CommitTemp
    )

/*++

Routine Description:

    This routine saves/caches the PCI Express device's LTR capability.

Arguments:

    ExpressPort - Supplies the Express device whose LTR capability will
        be saved.

    CapabilityTemp - Supplies a pointer to an LTR capability structure to hold
        a temporary copy of the current LTR capabilities, to be saved upon
        invocation when CommitTemp is set.

    CommitTemp - Supplies a flag indicating whether to commit the preservation
        of the supplied state.  If set, the supplied temporary state will be
        committed. If clear, the current state will be written into the
        supplied temporary location.

Return Value:

    STATUS_SUCCESS

--*/

{

    PCI_ASSERT(EXPRESS_INGRESS_LINK_PORT(ExpressPort) ||
               EXPRESS_RC_INTEGRATED_DEVICE(ExpressPort));  
    PCI_ASSERT(ExpressPort->LtrCapability != 0);
    PCI_ASSERT(ExpressPort->ExpressCapabilities.CapabilityVersion >= 2);

    if (CommitTemp == FALSE) {

        //
        // Save the LTR Capability block.
        //

        PciReadDeviceConfig(ExpressPort->Device,
                            CapabilityTemp,
                            ExpressPort->LtrCapability,
                            sizeof(PCI_EXPRESS_LTR_CAPABILITY));

    } else {
        ExpressPort->LtrCapabilitySaved = TRUE;
        RtlCopyMemory(&ExpressPort->LtrCapabilityBlock,
                      CapabilityTemp,
                      sizeof(PCI_EXPRESS_LTR_CAPABILITY));
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressRestoreLtrCapability (
    _Inout_ PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine restores the PCI Express device's LTR capability.

Arguments:

    ExpressPort - Supplies the Express device whose LTR capability will
        be restored.

Return Value:

    STATUS_SUCCESS

--*/

{

    if (ExpressPort->LtrCapabilitySaved == FALSE) {
        return STATUS_SUCCESS;
    }

    PCI_ASSERT(EXPRESS_INGRESS_LINK_PORT(ExpressPort) ||
               EXPRESS_RC_INTEGRATED_DEVICE(ExpressPort));  
    PCI_ASSERT(ExpressPort->LtrCapability != 0);

    //
    // Restore the Latency register. There are no other writeable
    // registers in the LTR Capability block.
    //

    PciWriteLtrRegister(
        ExpressPort,
        Latency,
        &ExpressPort->LtrCapabilityBlock.Latency.AsULONG);

    return STATUS_SUCCESS;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressInitializeLtrValue (
    _Inout_ PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine gets the starting LTR values for an express port, either from
    the hardware or from the firmware.

Arguments:

    ExpressPort - Supplies the Express port to be filled in with an initial LTR
        value.

Return Value:

    none

--*/

{

    PCI_EXPRESS_LTR_CAPABILITY CapabilityTemp;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // If this is an ingress port, calculate default LTR values, possibly based
    // on what the BIOS had to say about it.
    //

    switch (ExpressPort->DeviceType) {
    case PciExpressRootPort:
        __fallthrough;
    case PciExpressDownstreamSwitchPort:

        //
        // Look for a _DSM at the root port, or at downstream ports that are
        // already candidates for enabling LTR.  Looking for a _DSM at a
        // downstream port allows the BIOS to override the calculated values
        // we might come up with.  We don't, however, want to allow a BIOS to
        // trick us into enabling LTR on a sub-tree of buses under a root port
        // which does not have LTR enabled.  This would violate the PCIe spec,
        // which says:
        // 
        //   Software must not enable LTR in an Endpoint unless the Root Complex
        //   and all intermediate Switches indicate support for LTR.
        //

        if ((ExpressPort->DeviceType == PciExpressRootPort) ||
            (ExpressPort->Device->Parent->PciBridge->ExpressBridge->UpstreamLatency.AsULONG != 0)) {

            Status = PciEvaluateDsmMethodForLtr(ExpressPort->Device->DeviceObject,
                                                &ExpressPort->LatencyForDownstreamEndpoints);
        } else {
            Status = STATUS_NOT_SUPPORTED;
        }

        //
        // If the BIOS didn't supply it, calculate LTR values by looking 
        // upstream.
        //

        if (!NT_SUCCESS(Status)) {

            //
            // Root ports either get their values from firmware or they get 
            // zeros.  This is because the downstream endpoint will either have
            // values programmed into it by the BIOS at boot or it needs to get
            // them from the root port's firmware.
            //

            if (ExpressPort->DeviceType == PciExpressRootPort) {

                ExpressPort->LatencyForDownstreamEndpoints.AsULONG = 0;

            //
            // Downstream switch ports may have values supplied in firmware or
            // they may get them from their upstream switch ports, either
            // because the BIOS plugged values into the upstream switch port's
            // hardware or because there was firmware further up the tree.
            //

            } else {

                if (ExpressPort->DeviceCapabilities2.LTRMechanismSupported == 1) {
                    ExpressPort->LatencyForDownstreamEndpoints.AsULONG =
                        ExpressPort->Device->Parent->PciBridge->ExpressBridge->UpstreamLatency.AsULONG;
                } else {
                    ExpressPort->LatencyForDownstreamEndpoints.AsULONG = 0;
                }
            }

        } else {

            //
            // If the BIOS successfully supplied LTR values and the downstream
            // port doesn't claim to support LTR, that's odd and probably
            // a BIOS bug, but run with it and see what happens.  This will 
            // allow a BIOS to work around slightly busted silicon.
            //

            if (ExpressPort->DeviceCapabilities2.LTRMechanismSupported == 0) {
                TraceEvents(ExpressPort->Device->Parent->BlackBoxHandle,
                            TRACE_LEVEL_WARNING,
                            DBG_ENUM,
                            "BIOS supplied LTR values for downstream port "
                            "(%x:%x) but the port doesn't claim support",
                            ExpressPort->Device->Slot.u.bits.DeviceNumber,
                            ExpressPort->Device->Slot.u.bits.FunctionNumber);
            }
        }

        //
        // If, at this point, we've calculated an LTR value for downstream
        // endpoints associated with this upstream port, enable LTR in this
        // downstream port.
        // 
        // Note that if the BIOS programmed the root port to enable LTR and did
        // not supply a _DSM(LTR), then this bit may already be set.  Don't
        // clear it.
        //

        if (ExpressPort->LatencyForDownstreamEndpoints.AsULONG != 0) {
            PCI_ASSERT(ExpressPort->ExpressCapabilities.CapabilityVersion >= 2);
            ExpressPort->DeviceControl2.LTRMechanismEnable = 1;
        }

        break;

    case PciExpressUpstreamSwitchPort:
        __fallthrough;
    case PciExpressEndpoint:
        __fallthrough;
    case PciExpressToPciXBridge:
        __fallthrough;
    case PciExpressLegacyEndpoint:

        //
        // First, look to see if the hardware has anything programmed in it
        // already.  If there's nothing there, take the value from the upstream
        // port.
        //

        if (ExpressPort->LtrCapability != 0) {

            ExpressSaveLtrCapability(ExpressPort, &CapabilityTemp, FALSE);
            ExpressSaveLtrCapability(ExpressPort, &CapabilityTemp, TRUE);

            if (ExpressPort->LtrCapabilityBlock.Latency.AsULONG == 0) {

                if (ExpressPort->Device->Parent->PciBridge->ExpressBridge->UpstreamLatency.AsULONG != 0) {

                    ExpressPort->LtrCapabilityBlock.Latency.AsULONG = 
                        ExpressPort->Device->Parent->PciBridge->ExpressBridge->UpstreamLatency.AsULONG;
                    ExpressPort->DeviceControl2.LTRMechanismEnable = 1;
                }
            }
        }

        break;

    case PciExpressRootComplexIntegratedEndpoint:
        __fallthrough;
    case PciExpressRootComplexEventCollector:

        if (ExpressPort->LtrCapability != 0) {
            ExpressSaveLtrCapability(ExpressPort, &CapabilityTemp, FALSE);
            ExpressSaveLtrCapability(ExpressPort, &CapabilityTemp, TRUE);
        }

        break;
    }

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressInitializeObffValue (
    _Inout_ PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine gets the starting OBFF values for an express port, either from
    the hardware or from the firmware.

Arguments:

    ExpressPort - Supplies the Express port whose OBFF values will be
        determined.

Return Value:

    none

--*/

{

    PCI_FIRMWARE_BUS_CAPS BusCaps;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // If this is an ingress port see whether the BIOS wants to force use of the
    // OBFF "wake#" mechanism.
    //

    if ((ExpressPort->ExpressCapabilities.CapabilityVersion >= 2) &&
        (ExpressPort->DeviceCapabilities2.OBFFSupported != ObffNotSupported)) {
        ExpressPort->Obff = (EXPRESS_PORT_OBFF_CONTROL)ExpressPort->DeviceControl2.OBFFEnable;
    } else {
        ExpressPort->Obff = ObffDisabled;
    }

    TraceEvents(ExpressPort->Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_ENUM,
                "(%x:%x) BIOS left OBFF hardware set to: %!OBFF_CONTROL!",
                ExpressPort->Device->Slot.u.bits.DeviceNumber,
                ExpressPort->Device->Slot.u.bits.FunctionNumber,
                ExpressPort->Obff);

    switch (ExpressPort->DeviceType) {
    case PciExpressRootPort:
        __fallthrough;
    case PciExpressDownstreamSwitchPort:

        //
        // Sanitize this, looking for busted hardware.
        //

        if (((ExpressPort->DeviceCapabilities2.OBFFSupported == ObffSupportedViaWake) &&
             (ExpressPort->Obff != ObffEnabledUsingWake)) ||
            ((ExpressPort->DeviceCapabilities2.OBFFSupported == ObffSupportedViaMessages) &&
             (ExpressPort->Obff == ObffEnabledUsingWake))) {
            ExpressPort->Obff = ObffDisabled;
        }

        //
        // Ask the BIOS.
        //

        Status = PciEvaluateDsmMethodForObff(ExpressPort->Device->DeviceObject,
                                             &BusCaps);

        //
        // If the BIOS supplied it through a _DSM, overwrite what the hardware
        // said.
        //

        if (NT_SUCCESS(Status)) {

            if (BusCaps.ObffWakeSignalCapable == 1) {

                TraceEvents(ExpressPort->Device->Parent->BlackBoxHandle,
                            TRACE_LEVEL_VERBOSE,
                            DBG_ENUM,
                            "BIOS said use wake# for OBFF");

                ExpressPort->Obff = ObffEnabledUsingWake;
            }
        }
        
        //
        // Now that we've calculated what OBFF should be for this port,
        // "disable" it in the DeviceControl2 register.  This will cause 
        // OBFF to be disabled every time the port returns to the D0 state
        // after a power management transition.  The real OBFF state for this
        // port will be driven by power management transitions of the child
        // device (the upstream port) at Function 0.  The current OBFF state
        // will thus be tracked in the Device->ExpressPort->Parent, which
        // is an EXPRESS_BRIDGE struct.
        //

        ExpressPort->DeviceControl2.OBFFEnable = ObffDisabled;
        break;

    case PciExpressUpstreamSwitchPort:
        __fallthrough;
    case PciExpressEndpoint:

        //
        // Figure out which OBFF mechanism is shared by the endpoint and the
        // upstream port, if any.  Prefer wake#, as that is the most power-
        // efficient form.  Only Function 0 actually controls OBFF.
        //

        if (ExpressPort->Device->Slot.u.AsULONG == 0) {

            switch (ExpressPort->DeviceCapabilities2.OBFFSupported) {

            //
            // If OBFF is not supported in the endpoint, then disable it in the
            // parent downstream port.
            //

            case ObffNotSupported:
                ExpressPort->Obff = ObffDisabled;
                break;

            //
            // If only message signaling is supported, then set the parent to
            // match.
            //

            case ObffSupportedViaMessages:

                switch (ExpressPort->Parent->ObffConfiguration) {
                case ObffEnabledUsingMessagesVariationA:
                    ExpressPort->Obff = ObffEnabledUsingMessagesVariationA;
                    break;
                case ObffEnabledUsingMessagesVariationB:
                    ExpressPort->Obff = ObffEnabledUsingMessagesVariationB;
                    break;
                default:
                    ExpressPort->Obff = ObffDisabled;
                    break;
                }
                break;

            //
            // If wake# is supported, use it.  Note that the parent may not
            // be showing that it supports OBFF at all.  Wake# can "reach
            // around" a bridge that doesn't support OBFF.  The
            // ExpressPort->Obff field will be initialized based on either
            // the hardware or the _DSM.
            //

            case ObffSupportedViaWake:

                if (ExpressPort->Parent->ObffConfiguration == ObffEnabledUsingWake) {
                    ExpressPort->Obff = ObffEnabledUsingWake;
                } else {
                    ExpressPort->Obff = ObffDisabled;
                }
                break;

            //
            // If the device supports "both" then defer to what the BIOS 
            // put in firmware or hardware.
            //

            case ObffSupportedViaBoth:

                if (ExpressPort->Obff != ObffDisabled) {
                    ExpressPort->Obff = ExpressPort->Parent->ObffConfiguration;
                }
                break;
            }
        }
        break;
    }

    TraceEvents(ExpressPort->Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_ENUM,
                "(%x:%x) %!OBFF_SUPPORT! hw:%!OBFF_CONTROL! sw:%!OBFF_CONTROL!",
                ExpressPort->Device->Slot.u.bits.DeviceNumber,
                ExpressPort->Device->Slot.u.bits.FunctionNumber,
                ExpressPort->DeviceCapabilities2.OBFFSupported,
                ExpressPort->DeviceControl2.OBFFEnable,
                ExpressPort->Obff
                );

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressPortGetLtrValueForExpressBridge (
    _In_ PEXPRESS_PORT ExpressPort,
    _Out_ PPCI_EXPRESS_LTR_MAX_LATENCY_REGISTER UpstreamLatency
    )

/*++

Routine Description:

    This routine gets value to report to the FDO that's building an express
    bridge struct for the "UpstreamLatency" which is the value that will be
    reported down to child endpoints.

Arguments:

    ExpressPort - Supplies the Express port associated with an express bridge.
 
    UpstreamLatency - Pointer to a value to be filled in with LTR values which
        should be used for any future upstream port which might be plugged
        in below this express bridge.

Return Value:

    none

--*/

{

    PAGED_CODE();

    switch (ExpressPort->DeviceType) {
    case PciExpressRootPort:
        __fallthrough;
    case PciExpressDownstreamSwitchPort:

        *UpstreamLatency = ExpressPort->LatencyForDownstreamEndpoints;
        break;

    case PciExpressUpstreamSwitchPort:

        *UpstreamLatency = ExpressPort->LtrCapabilityBlock.Latency;
        break;

    default:
        UpstreamLatency->AsULONG = 0;
        break;
    }
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressPortGetObffValueForExpressBridge (
    _In_ PEXPRESS_PORT ExpressPort,
    _Out_ PEXPRESS_PORT_OBFF_CONTROL Obff
    )

/*++

Routine Description:

    This routine gets value to report to the FDO that's building an express
    bridge struct for the "ObffConfiguration" which will cause downstream
    endpoints to use a particular kind of OBFF configuration.

Arguments:

    ExpressPort - Supplies the Express port associated with an express bridge
 
    Obff - The OBFF setting for this port.

Return Value:

    none

--*/

{

    PAGED_CODE();

    switch (ExpressPort->DeviceType) {
    case PciExpressRootPort:
        __fallthrough;
    case PciExpressDownstreamSwitchPort:

        *Obff = ExpressPort->Obff;
        break;

    default:
        *Obff = ObffDisabled;
        break;
    }
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
ExpressBridgeCalculateLtrValues (
    _Inout_ PEXPRESS_BRIDGE ExpressBridge,
    _In_ ULONG UpstreamLatency
    )

/*++

Routine Description:

    This routine takes the value from the PDO for "UpstreamLatency" which is the
    LTR value that was programmed by BIOS, through hardware or firmware, and
    calculates the LTR values for downstream ports.

Arguments:

    ExpressBridge - Supplies the pointer to the struct that tracks an expres
        bridge
 
    UpstreamLatency - Supplies latency values that were caluclated for this
        express bridge when we initialized the downstream port above this
        bridge.

Return Value:

    none

--*/

{

    LONG Msb;
    LARGE_INTEGER Latency;
    PCI_EXPRESS_LTR_MAX_LATENCY_REGISTER LatencyRegister;

    PAGED_CODE();

    switch (ExpressBridge->BridgeType) {

        //
        // For downstream ports, take the value straight from the PDO.
        //

    case PciExpressRootPort:
        __fallthrough;
    case PciExpressDownstreamSwitchPort:

        ExpressBridge->UpstreamLatency.AsULONG = UpstreamLatency;

        //
        // For upstream switch ports, add 1us to the value from the PDO, if
        // there was one, as that will give a good starting point for all
        // downstream switch ports within the switch.  If there was no value
        // from the PDO, that indicates that this link shouldn't support
        // LTR at all.
        //
        break;

    case PciExpressUpstreamSwitchPort:

        if (UpstreamLatency != 0) {

            LatencyRegister.AsULONG = UpstreamLatency;

            Latency.QuadPart = 
                (ULONGLONG)LatencyRegister.MaxSnoopLatencyValue <<
                (LatencyRegister.MaxSnoopLatencyScale * 5);

            Latency.QuadPart += 0x400;  // Add about 1us.

            //
            // Preserve only the ten most significant bits.
            //

            if (_BitScanReverse(&Msb, Latency.HighPart) == 0) {
                _BitScanReverse(&Msb, Latency.LowPart);
            } else {
                Msb += 32;
            }

            LatencyRegister.MaxSnoopLatencyScale = 0;

            while (Msb >= 10) {

                LatencyRegister.MaxSnoopLatencyScale++;
                Latency.QuadPart >>= 5;
                Msb -= 5; 
            }

            LatencyRegister.MaxSnoopLatencyValue = Latency.LowPart;

            //
            // Do it again for the "no-snoop" case.
            //

            Latency.QuadPart = 
                (ULONGLONG)LatencyRegister.MaxNoSnoopLatencyValue <<
                (LatencyRegister.MaxNoSnoopLatencyScale * 5);

            Latency.QuadPart += 0x400;  // Add about 1us.

            //
            // Preserve only the ten most significant bits.
            //

            if (_BitScanReverse(&Msb, Latency.HighPart) == 0) {
                _BitScanReverse(&Msb, Latency.LowPart);
            } else {
                Msb += 32;
            }

            LatencyRegister.MaxNoSnoopLatencyScale = 0;

            while (Msb >= 10) {

                LatencyRegister.MaxNoSnoopLatencyScale++;
                Latency.QuadPart >>= 5;
                Msb -= 5; 
            }

            LatencyRegister.MaxNoSnoopLatencyValue = Latency.LowPart;
            ExpressBridge->UpstreamLatency = LatencyRegister;

        } else {

            ExpressBridge->UpstreamLatency.AsULONG = 0;
        }

        break;
    }
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
ExpressPortObffEnable(
    _In_ PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine enables OBFF in a PCIe function and then, if appropriate,
    enables it in the parent bridge's downstream port.  OBFF must be enabled in
    an endpoint before it is enabled in the parent, when using message-based
    OBFF, because an OBFF message should never be delivered to an endpoint which
    isn't expecting it.  This could cause a UR on the PCIe fabric, which could
    result in an NMI, a machine check, or a WHEA report, or nothing at all.

Arguments:

    ExpressPort - Supplies the pointer to the struct that tracks an express
        port.

Return Value:

    none

--*/

{
    PPCI_DEVICE Device;
    BOOLEAN UpdateParent;

    UpdateParent = FALSE;
    Device = ExpressPort->Device;

    //
    // If this device isn't starting or started, then OBFF stuff won't be
    // completely inititalized yet.  Bail.
    //

    if ((Device->TentativeNextState != PciStarted) &&
        (Device->DeviceState != PciStarted)) {
        return;
    }

    if ((ExpressPort->ExpressCapabilities.CapabilityVersion >= 2) &&
        (ExpressPort->Obff != ObffDisabled)) {

        //
        // If this is an upstream port turn that on.  This has to happen after
        // the endpoint is configured.
        //

        switch (ExpressPort->DeviceType) {
        case PciExpressEndpoint:
            __fallthrough;
        case PciExpressLegacyEndpoint:
            __fallthrough;
        case PciExpressUpstreamSwitchPort:

            if (Device->Slot.u.AsULONG == 0) {
                ExpressPort->DeviceControl2.OBFFEnable = ExpressPort->Obff;

                if (ExpressPort->Obff != ObffEnabledUsingWake) {
                    UpdateParent = TRUE;
                }
            }
            break;

        //
        // For downstream ports, there are two algorithms.  If the device is
        // using message-based OBFF, leave OBFF off until Function 0 of the
        // device below enables OBFF.
        //

        case PciExpressRootPort:
            __fallthrough;
        case PciExpressDownstreamSwitchPort:

            if (ExpressPort->Obff != ObffEnabledUsingWake) {
                ExpressPort->DeviceControl2.OBFFEnable = ObffDisabled;
            } else {
                ExpressPort->DeviceControl2.OBFFEnable = ObffEnabledUsingWake;
            }

            break;

        default:
            PCI_ASSERTMSG("OBFF shouldn't exist for any other types of PCIe functions", FALSE);
        }

        PciWriteExpressRegister(ExpressPort,
                                DeviceControl2,
                                &ExpressPort->DeviceControl2);

        //
        // Turn it on in the parent bridge, matching the child mode.  This
        // has to happen after it is turned on in the child.  The parent may
        // not actually have a DeviceControl2 register, if the machine is using
        // wake# signaling for OBFF.
        //

        if (UpdateParent != FALSE) {
            if (ExpressPort->Parent->ObffControl == NULL) {
                PCI_ASSERT(ExpressPort->Obff == ObffEnabledUsingWake);
            } else {
                ExpressPort->Parent->ObffControl(ExpressPort->Parent->PciBridge->InterfaceContext,
                                                 ExpressPort->Obff);
            }
        }
    } else {

        //
        // The BIOS may have erroneously left OBFF turned on for an upstream
        // port when we believe it can't work, or possibly it could still be on
        // after a hot-unplug of a previous OBFF-enabled device.  Turn it off
        // if we're in the process of starting this device.  (The useful
        // values aren't initialized until start.)
        //

        switch (ExpressPort->DeviceType) {
        case PciExpressEndpoint:
            __fallthrough;
        case PciExpressLegacyEndpoint:
            __fallthrough;
        case PciExpressUpstreamSwitchPort:

            if ((Device->Slot.u.AsULONG == 0) &&
                (ExpressPort->Parent->ObffControl != NULL)) {

                TraceEvents(ExpressPort->Device->Parent->BlackBoxHandle,
                            TRACE_LEVEL_VERBOSE,
                            DBG_ENUM,
                            "(%x:%x) Turning off OBFF when it was left on for some reason",
                            ExpressPort->Device->Slot.u.bits.DeviceNumber,
                            ExpressPort->Device->Slot.u.bits.FunctionNumber);

                ExpressPort->Parent->ObffControl(ExpressPort->Parent->PciBridge->InterfaceContext,
                                                 ObffDisabled);

                if ((ExpressPort->ExpressCapabilities.CapabilityVersion >= 2) &&
                    (ExpressPort->DeviceControl2.OBFFEnable != ObffDisabled)) {

                    ExpressPort->DeviceControl2.OBFFEnable = ObffDisabled;
                    PciWriteExpressRegister(ExpressPort,
                                            DeviceControl2,
                                            &ExpressPort->DeviceControl2);
                }
            }
            break;
        }
    }
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
ExpressPortObffDisable (
    _In_ PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine disables OBFF.  If this is an endpoint and Function 0 and it's
    using OBFF via messaging, it first disables it in the parent.  OBFF via
    messaging must be enabled in an endpoint before it is enabled in the parent
    because an OBFF message should never be delivered to an endpoint which isn't
    expecting it.

Arguments:

    ExpressPort - Supplies the pointer to the struct that tracks an express
        port.

Return Value:

    none

--*/

{

    if (ExpressPort->Obff != ObffDisabled) {

        //
        // Presumably we should never have been able to enable OBFF if the
        // DeviceControl2 register doesn't exist.
        //

        PCI_ASSERT(ExpressPort->ExpressCapabilities.CapabilityVersion >= 2);

        //
        // If this is an upstream port and the parent doesn't have OBFF
        // configured, turn that on.  This has to happen before the endpoint
        // is powered down.
        //

        switch (ExpressPort->DeviceType) {
        case PciExpressEndpoint:
            __fallthrough;
        case PciExpressLegacyEndpoint:
            __fallthrough;
        case PciExpressUpstreamSwitchPort:

            if ((ExpressPort->Obff != ObffEnabledUsingWake) &&
                (ExpressPort->Parent->ObffControl != NULL) &&
                (ExpressPort->Device->Slot.u.AsULONG == 0)) {

                ExpressPort->Parent->ObffControl(ExpressPort->Parent->PciBridge->InterfaceContext,
                                                 ObffDisabled);
            }
        }

        TraceEvents(ExpressPort->Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_ENUM,
                    "(%x:%x) Turning off OBFF",
                    ExpressPort->Device->Slot.u.bits.DeviceNumber,
                    ExpressPort->Device->Slot.u.bits.FunctionNumber);

        ExpressPort->DeviceControl2.OBFFEnable = ObffDisabled;

        PciWriteExpressRegister(ExpressPort,
                                DeviceControl2,
                                &ExpressPort->DeviceControl2.AsUSHORT);
    }
}
