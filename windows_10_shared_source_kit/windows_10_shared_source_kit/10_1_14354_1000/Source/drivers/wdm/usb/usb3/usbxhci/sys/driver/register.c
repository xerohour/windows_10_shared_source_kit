/*++
Copyright (c) Microsoft Corporation

Module Name:

    register.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "register.tmh"
#define WPP_BIT_MYFLAG WPP_BIT_Register

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Register_Create)
#pragma alloc_text(PAGE, Register_PrepareHardware)
#pragma alloc_text(PAGE, Register_ReleaseHardware)
#pragma alloc_text(PAGE, Register_ParseCapabilityRegister)
#endif

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Register_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    )
/*++

Routine Description:

    This function creates an generic wdf object, along with a data
    for xhci register information.

--*/
{
    NTSTATUS                status;
    WDF_OBJECT_ATTRIBUTES   wdfAttributes;
    WDFOBJECT               wdfRegister;
    PREGISTER_DATA          registerData;

    UNREFERENCED_PARAMETER(WdfDevice);

    PAGED_CODE();

    TRY {

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, REGISTER_DATA);
        wdfAttributes.ParentObject = WdfDevice;

        status = WdfObjectCreate(&wdfAttributes, &wdfRegister);

        if (NT_SUCCESS(status)) {

            registerData = GetRegisterData(wdfRegister);

            registerData->WdfRegister = wdfRegister;

            LogInfo(Controller_GetRecorderLog(ControllerHandle), Register,
                    "wdfRegister 0x%p", wdfRegister);

            registerData->ControllerHandle = ControllerHandle;

            //
            // Return register data as the handle to the register object.
            //
            *ObjectHandle = (HANDLE)registerData;

        } else {

            LogError(Controller_GetRecorderLog(ControllerHandle), Register,
                     "WdfObjectCreate Failed %!STATUS!", status);
            LEAVE;
        }

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Register_PrepareHardware(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        WDFCMRESLIST    WdfResourcesTranslated
    )
/*++

Routine Description:

    This function configures the registers for the xHCI controller.

--*/
{
    NTSTATUS                            status;
    ULONG                               i;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR     descriptor;

    PAGED_CODE();

    TRY {

        //
        // Find the resource for the xHCI's registers.
        //
        for (i = 0; i < WdfCmResourceListGetCount(WdfResourcesTranslated); i++) {

            descriptor = WdfCmResourceListGetDescriptor(WdfResourcesTranslated, i);

            switch (descriptor->Type) {

            case CmResourceTypeMemory:

                LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                        "Processing Resource %d: CmResourceTypeMemory", i);
                LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                        "Memory Resource @ 0x%x%x Length %d",
                        descriptor->u.Memory.Start.HighPart, descriptor->u.Memory.Start.LowPart,
                        descriptor->u.Memory.Length);

                if (RegisterData->CapabilityRegisters == NULL) {

                    RegisterData->CapabilityRegisters =
                        (PCAPABILITY_REGISTERS)MmMapIoSpaceEx(descriptor->u.Memory.Start,
                                                              descriptor->u.Memory.Length,
                                                              PAGE_READWRITE | PAGE_NOCACHE);

                    if (RegisterData->CapabilityRegisters == NULL) {

                        LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                                 "MmMapIoSpaceEx Failed");
                        status = STATUS_INSUFFICIENT_RESOURCES;
                        LEAVE;
                    }

                    RegisterData->RegisterMapLength = descriptor->u.Memory.Length;
                    RegisterData->RegistersMapped = TRUE;

                    LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                            "CapabilityRegisters 0x%p RegisterMapLength %d",
                            RegisterData->CapabilityRegisters,
                            RegisterData->RegisterMapLength);
                }
                break;

            default:
                break;
            }
        }

        if (RegisterData->CapabilityRegisters == NULL) {

            status = STATUS_INSUFFICIENT_RESOURCES;
            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "No memory resources provided");
            LEAVE;
        }

        //
        // Parse the capability registers to verify they are conformant with
        // this driver's requirements.
        //
        status = Register_ParseCapabilityRegister(RegisterData);

        if (!NT_SUCCESS(status)) {

            LogWarning(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                       "Register_ParseCapabilityRegister Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Perform the BIOS handoff.
        //
        status = Register_BiosHandoff(RegisterData);

        if (!NT_SUCCESS(status)) {

            LogWarning(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                       "Register_BiosHandoff Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Reset the xHCI controller.
        //
        status = Register_ControllerReset(RegisterData, FALSE);

        if (!NT_SUCCESS(status)) {

            LogWarning(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                       "Register_ControllerReset Failed %!STATUS!", status);
            LEAVE;
        }

    } FINALLY {

        if (RegisterData->CapabilityRegisters == NULL) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "Failed to find CapabilityRegisters");
            status = STATUS_UNSUCCESSFUL;
        }
    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Register_ReleaseHardware(
    __in
        PREGISTER_DATA  RegisterData
    )
/*++

Routine Description:

    This function is called on a release hardware event and frees any
    resources acquired for managing the xHCI registers.

--*/
{
    PAGED_CODE();

    TRY {

        if (RegisterData->CapabilityRegisters != NULL) {

            MmUnmapIoSpace(RegisterData->CapabilityRegisters, RegisterData->RegisterMapLength);

            RegisterData->RegistersMapped = FALSE;
            RegisterData->CapabilityRegisters = NULL;
            RegisterData->OperationalRegisters = NULL;
            RegisterData->RuntimeRegisters = NULL;
            RegisterData->DoorbellRegisters = NULL;
            RegisterData->ExtendedCapabilityRegisters = NULL;
            RegisterData->UsbLegSupCapability = NULL;
            RegisterData->DebugCapability = NULL;
        }

    } FINALLY {

    }

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_D0EntryPreRestoreState(
    __in
        PREGISTER_DATA RegisterData
    )
{
    Register_SetClearSSICPortUnused(RegisterData, FALSE);
    Register_SaveRestoreCHTNonArchitecturalRegisters(RegisterData, FALSE);

    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Register_D0Entry(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    )
/*++

Routine Description:

    This routine is invoked when the controller enters D0 power state. It performs
    actions that need to be done on every D0Entry, like - apply controller-specific
    workarounds that need writing to memory mapped registers.

--*/
{
    TRY {

        UNREFERENCED_PARAMETER(WdfPreviousState);






















































































    } FINALLY {

    }

    return STATUS_SUCCESS;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_D0ExitPostSaveSate(
    __in
        PREGISTER_DATA RegisterData
    )
{
    Register_SetClearSSICPortUnused(RegisterData, TRUE);
    Register_SaveRestoreCHTNonArchitecturalRegisters(RegisterData, TRUE);

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_SetClearSSICPortUnused(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        BOOLEAN                 OnPowerDown
    )
{
    ULONG   ssicPortRegister;
    PULONG  ssicPortRegisterAddress;

    TRY {

        if (Controller_GetDeviceFlags(RegisterData->ControllerHandle).EnableCHTSSICPortUnusedWorkaround == 0) {
            LEAVE;
        }

        ssicPortRegisterAddress = (PULONG)(((PUCHAR)RegisterData->CapabilityRegisters) + 0x883C);

        //
        // PROG_DONE=0
        //
        ssicPortRegister = Register_ReadUlong(ssicPortRegisterAddress);
        ssicPortRegister &= ~(1U << 30);
        Register_WriteUlong(ssicPortRegisterAddress, ssicPortRegister);

        //
        // PROG_UNUSED=Set on Power Down, otherwise clear.
        //
        ssicPortRegister = Register_ReadUlong(ssicPortRegisterAddress);
        if (OnPowerDown) {
            ssicPortRegister |=  (1U << 31);
        } else {
            ssicPortRegister &= ~(1U << 31);
        }
        Register_WriteUlong(ssicPortRegisterAddress, ssicPortRegister);

        //
        // PROG_DONE=1
        //
        ssicPortRegister = Register_ReadUlong(ssicPortRegisterAddress);
        ssicPortRegister |= (1U << 30);
        Register_WriteUlong(ssicPortRegisterAddress, ssicPortRegister);

    } FINALLY {

    }

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_SaveRestoreCHTNonArchitecturalRegisters(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        BOOLEAN         Save
    )
/*++

Routine Description:

    This routine implements a controller workaround.

    Controller bug: Some non-architectural xHCI registers that are supposed to be
    Saved/Restored on D0Exit/D0Entry are not being Saved/Restored.
    
    Woarkaround: In D0Exit, after Controller State Save operation, cache current value of
    Bit [25] in DWORDs beginning at MMIO offsets 0x8904, 0x8A14, and 0x8B24. In D0Entry,
    before Controller State Restore operation, write the saved values back to their
    respective registers.

--*/
{
    ULONG   bitValue;
    ULONG   index;
    PULONG  registerAddress;
    ULONG   registerOffsets[] = { 0x8904, 0x8A14, 0x8B24 };
    ULONG   registerValue;

#define BIT_POSITION_IN_REGISTER 25

    TRY {

        if (Controller_GetDeviceFlags(RegisterData->ControllerHandle).EnableCHTNonArchitecturalRegisterSaveRestore == 0) {
            LEAVE;
        }

        for (index = 0; index < RTL_NUMBER_OF(registerOffsets); index += 1) {

            registerAddress = (PULONG)(((PUCHAR)RegisterData->CapabilityRegisters) + registerOffsets[index]);
            registerValue   = Register_ReadUlong(registerAddress);

            if (Save) {

                bitValue = ((registerValue & (1U << BIT_POSITION_IN_REGISTER)) == 0) ? 0 : 1;

                if (bitValue) {
                    RegisterData->CHTSavedRegisterBits |=  (1U << index);
                } else {
                    RegisterData->CHTSavedRegisterBits &= ~(1U << index);
                }

            } else {

                bitValue = ((RegisterData->CHTSavedRegisterBits & (1U << index)) == 0) ? 0 : 1;

                if (bitValue) {
                    registerValue |=  (1U << BIT_POSITION_IN_REGISTER);
                } else {
                    registerValue &= ~(1U << BIT_POSITION_IN_REGISTER);
                }

                Register_WriteUlong(registerAddress, registerValue);
            }
        }

    } FINALLY {

    }

    return;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Register_ParseCapabilityRegister(
    __in
        PREGISTER_DATA  RegisterData
    )
/*++

Routine Description:

    This routine displays and verfies contents of the xHCI controller's registers.

--*/
{
    NTSTATUS                    status;
    HCLENGTHVERSION             lengthVersion;
    HCSPARAMS1                  parameters1;
    HCSPARAMS2                  parameters2;
    HCSPARAMS3                  parameters3;
    HCCPARAMS                   capabilities;
    ULONG                       doorbellOffset;
    ULONG                       runtimeRegisterSpaceOffset;
    ULONG                       xhciPageSize;
    KSE_XHCI_CONTROLLER_FLAGS   deviceFlags;
    ULONGLONG                   firmwareVersion;

    PAGED_CODE();

    TRY {













































        lengthVersion.AsUlong32 =
            Register_ReadUlong(&RegisterData->CapabilityRegisters->LengthVersion.AsUlong32);

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "XHCI Version 0x%x.0x%x  Capability Registers Length %d", lengthVersion.MajorRevision,
                lengthVersion.MinorRevision, lengthVersion.CapabilityRegisterLength);

        Controller_SetXhciVersion(RegisterData->ControllerHandle,
                                  (UCHAR)lengthVersion.MajorRevision,
                                  (UCHAR)lengthVersion.MinorRevision);

        //
        // Force single interrupter for all 0.96 version controllers. For all known 0.96
        // controllers, this flag should already be set via Kse. For others, we set the flag
        // here. This check is done here because the registers with version information are
        // not available before PrepareHardware.
        //
        if (Controller_GetXhciMajorVersion(RegisterData->ControllerHandle) < XHCI_MAJOR_VERSION_1) {

            deviceFlags.AsULong64 = 0;
            deviceFlags.UseSingleInterrupter = 1;

            LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                    "Forcing single interupter because controller version is 0.96");

            Controller_SetDeviceFlags(RegisterData->ControllerHandle, deviceFlags);
        } else if (Controller_GetXhciMinorVersion(RegisterData->ControllerHandle) >= XHCI_MINOR_VERSION_1 ||
                   Controller_GetXhciMajorVersion(RegisterData->ControllerHandle) > XHCI_MAJOR_VERSION_1) {

            //
            // MSE TRB Transfer Length are available for XHCI 1.1+
            //
            Controller_SetMSELengthUsable(RegisterData->ControllerHandle, TRUE);
        }

        //
        // Set the pointer to the operational registers for the XHCI controller, which is
        // located from the base (capability) registers + capability register length.
        //
        RegisterData->OperationalRegisters =
            (POPERATIONAL_REGISTERS)(((PUCHAR)RegisterData->CapabilityRegisters) +
                                     lengthVersion.CapabilityRegisterLength);

        //
        // 5.3.3 Structural Parameters 1 (HCSPARAMS1)
        //
        parameters1.AsUlong32 =
            Register_ReadUlong(&RegisterData->CapabilityRegisters->StructuralParameters1.AsUlong32);

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "NumberOfDeviceSlots %d NumberOfInterrupters %d NumberOfPorts %d",
                parameters1.NumberOfDeviceSlots, parameters1.NumberOfInterrupters,
                parameters1.NumberOfPorts);

        if (parameters1.NumberOfDeviceSlots == 0) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Invalid number of device slots %d", parameters1.NumberOfDeviceSlots);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        RegisterData->SupportedNumberOfDeviceSlots = parameters1.NumberOfDeviceSlots;

        if ((parameters1.NumberOfInterrupters == 0) ||
            (parameters1.NumberOfInterrupters > MAX_NUMBER_OF_INTERRUPTERS)) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Invalid number of interrupters %d", parameters1.NumberOfInterrupters);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        RegisterData->SupportedNumberOfInterrupters = parameters1.NumberOfInterrupters;

        if (parameters1.NumberOfPorts == 0) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Invalid number of ports %d", parameters1.NumberOfPorts);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        RegisterData->SupportedNumberOfPorts = parameters1.NumberOfPorts;

        //
        // 5.3.4 Structural Parameters 2 (HCSPARAMS2)
        //
        parameters2.AsUlong32 =
            Register_ReadUlong(&RegisterData->CapabilityRegisters->StructuralParameters2.AsUlong32);

        RegisterData->MaxEventRingSegments = (1 << parameters2.EventRingSegmentTableMax);

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "IsochSchedulingThreshold %d EventRingSegmentTableMax %d",
                parameters2.IsochSchedulingThreshold, RegisterData->MaxEventRingSegments);

        RegisterData->MaxScratchpadBuffers = ((parameters2.MaxScratchpadBuffersHi << 5) |
                                              parameters2.MaxScratchpadBuffers);

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "ScratchpadRestore %d MaxScratchpadBuffers %d",
                parameters2.ScratchpadRestore, RegisterData->MaxScratchpadBuffers);

        //
        // 5.3.5 Structural Parameters 3 (HCSPARAMS3)
        //
        parameters3.AsUlong32 =
            Register_ReadUlong(&RegisterData->CapabilityRegisters->StructuralParameters3.AsUlong32);

        RegisterData->U1DeviceExitLatency = (USHORT)parameters3.U1DeviceExitLatency;
        RegisterData->U2DeviceExitLatency = (USHORT)parameters3.U2DeviceExitLatency;

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "U1DeviceExitLatency 0x%x U2DeviceExitLatency 0x%x",
                parameters3.U1DeviceExitLatency, parameters3.U2DeviceExitLatency);

        if (parameters3.U1DeviceExitLatency > HCSPARAMS3_U1_DEVICE_EXIT_LATENCY_MAX) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Invalid U1DeviceExitLatency");
            status = STATUS_INVALID_PARAMETER;
            NT_ASSERT(0);
            LEAVE;
        }

        if (parameters3.U2DeviceExitLatency > HCSPARAMS3_U2_DEVICE_EXIT_LATENCY_MAX) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Invalid U2DeviceExitLatency");
            status = STATUS_INVALID_PARAMETER;
            NT_ASSERT(0);
            LEAVE;
        }

        //
        // 5.3.6 Capability Parameters (HCCPARAMS1)
        //
        capabilities.AsUlong32 =
            Register_ReadUlong(&RegisterData->CapabilityRegisters->CapabilityParameters.AsUlong32);

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "AddressingCapability %d BwNegotiationCapability %d ContextSize %d PortPowerControl %d",
                capabilities.AddressingCapability, capabilities.BwNegotiationCapability,
                capabilities.ContextSize, capabilities.PortPowerControl);

        RegisterData->CachedHccParams.AsUlong32 = capabilities.AsUlong32;

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "PortIndicators %d LightHCResetCapability %d LatencyToleranceMessagingCapability %d NoSecondarySidSupport %d",
                capabilities.PortIndicators, capabilities.LightHCResetCapability,
                capabilities.LatencyToleranceMessagingCapability,
                capabilities.NoSecondarySidSupport);

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "MaximumPrimaryStreamArraySize %d XhciExtendedCapabilitiesPointer 0x%x",
                capabilities.MaximumPrimaryStreamArraySize,
                capabilities.XhciExtendedCapabilitiesPointer);

        //
        // 5.3.9 Capability Parameters (HCCPARAMS2). Zero out if not available
        //
        RegisterData->CachedHccParams2.AsUlong32 = 0;
        if (lengthVersion.CapabilityRegisterLength >= HCCPARAMS2_CAPABILITY_LENGTH) {

            RegisterData->CachedHccParams2.AsUlong32 =
                Register_ReadUlong(&RegisterData->CapabilityRegisters->CapabilityParameters2.AsUlong32);
        }

        //
        // eXtensible Host Controller Interface Revision 1.0 with errata to 6/13/11
        // Table 26: Host Controller Capability Parameters (HCCPARAMS) (Continued)
        //
        // Maximum Primary Stream Array Size (MaxPSASize). This fields
        // identifies the maximum size Primary Stream Array that the xHC
        // supports. The Primary Stream Array size = 2^(MaxPSASize+1).
        // Valid MaxPSASize values are 0 to 15, where '0' indicates that
        // Streams are not supported.
        //
        // Since the register field is only 4 bits wide (15:12) all
        // possible register values are valid.
        //

        // The maximum PSA size supported is currently 7 (4KB Primary
        // Stream Context Array). This is because the largest common
        // buffer size is 4KB.  This supports up to 255 streams.
        //
        // The calculation: 2^(7+1) = 256 * 16bytes per context = 4096 buffer.
        //



        //
        if (capabilities.MaximumPrimaryStreamArraySize > 7) {

            LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "Setting MaxPSASize to 7 from %d", capabilities.MaximumPrimaryStreamArraySize);
            capabilities.MaximumPrimaryStreamArraySize = 7;
        }

        //
        // Translate the raw HC Maximum Primary Stream Array Size
        // (MaxPSASize) value to the HCD Supported Number Of Streams
        // value.
        //
        // Subtract one since Stream ID 0 is reserved.
        //
        // NOTE: If MaxPSASize is 15 then subtract a total of 3 (65533
        // streams supported) as 0xFFFE is reserved as the Prime Stream
        // ID and 0xFFFF is reserved as the NoStream Stream ID.
        //
        if ((capabilities.MaximumPrimaryStreamArraySize == 0) ||
            Controller_GetDeviceFlags(RegisterData->ControllerHandle).DisableStreams) {

            RegisterData->SupportedNumberOfStreams = 0;

        } else {

            RegisterData->SupportedNumberOfStreams = (2 << capabilities.MaximumPrimaryStreamArraySize) - 1;
        }

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "Supported Number Of Streams %d", RegisterData->SupportedNumberOfStreams);

        if (capabilities.XhciExtendedCapabilitiesPointer == 0 ||
            capabilities.XhciExtendedCapabilitiesPointer * sizeof(ULONG) >= RegisterData->RegisterMapLength) {

            //
            // The xHC must implement extended capabilites as section
            // 7.2 xHCI Supported Protocol Capability states: "At least
            // one of these capability structures is required for all
            // xHCI implementations."
            //
            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Invalid Extended Capabilites Pointer %d", capabilities.XhciExtendedCapabilitiesPointer);
            status = STATUS_INVALID_PARAMETER;
            NT_ASSERT(0);
            LEAVE;
        }

        RegisterData->ExtendedCapabilityRegisters =
            (PEXTENDED_CAPABILITY)(((PUCHAR)RegisterData->CapabilityRegisters) +
                                   capabilities.XhciExtendedCapabilitiesPointer * sizeof(ULONG));

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "ExtendedCapabilityRegisters 0x%p", RegisterData->ExtendedCapabilityRegisters);

        //
        // Cache the address of USBLEGSUPCAPABILITY to avoid register reads
        // every time it is needed.
        //
        RegisterData->UsbLegSupCapability =
            (PUSBLEGSUPCAPABILITY)Register_FindFirstExtendedCapability(
                RegisterData,
                EXTENDED_CAPABILITY_ID_USB_LEGACY_SUPPORT);

        RegisterData->DebugCapability =
            (PDEBUG_CAPABILITY)Register_FindFirstExtendedCapability(
                RegisterData,
                EXTENDED_CAPABILITY_ID_USB_DEBUG_CAPABILITY);

        //
        // 5.3.7 Doorbell Offset (DBOFF)
        //
        doorbellOffset = Register_ReadUlong(&RegisterData->CapabilityRegisters->DoorbellOffset);

        if (doorbellOffset == 0) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Invalid DoorbellOffset 0x%x", doorbellOffset);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        RegisterData->DoorbellRegisters =
            (PDOORBELL_REGISTER)(((PUCHAR)RegisterData->CapabilityRegisters) + doorbellOffset);

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "DoorbellRegisters 0x%p DoorbellOffset 0x%x", RegisterData->DoorbellRegisters,
                doorbellOffset);

        //
        // 5.3.8 Runtime Register Space Offset (RTSOFF)
        //
        runtimeRegisterSpaceOffset =
            Register_ReadUlong(&RegisterData->CapabilityRegisters->RuntimeRegisterSpaceOffset);

        if (runtimeRegisterSpaceOffset == 0) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Invalid RuntimeRegisterSpaceOffset 0x%x", runtimeRegisterSpaceOffset);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        RegisterData->RuntimeRegisters =
            (PRUNTIME_REGISTERS)(((PUCHAR)RegisterData->CapabilityRegisters) + runtimeRegisterSpaceOffset);

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                "RuntimeRegisters 0x%p RuntimeRegisterSpaceOffset 0x%x",
                RegisterData->RuntimeRegisters, runtimeRegisterSpaceOffset);

        //
        // 5.4.3 Page Size Register (PAGESIZE)
        //
        xhciPageSize =
            Register_ReadUlong(&RegisterData->OperationalRegisters->PageSizeRegister.AsUlong32);

        if (xhciPageSize != 1) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                     "HW_COMPLIANCE: Unsupported xhciPageSize 0x%x", xhciPageSize);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Register_BiosHandoff(
    __in
        PREGISTER_DATA  RegisterData
    )
/*++

Routine Description:

    This routine attempts to perform the BIOS handoff by unconditionally setting
    the OS_OWN semaphore, followed by ensuring that the BIOS_OWN semaphore gets
    cleared.

--*/
{
    USBSTS                  usbStatus;
    PUSBLEGSUPCAPABILITY    usbLegSupCapability;
    PUSBLEGSUP              usbLegacySupport;
    PUSBLEGCTLSTS           usbLegacySupportControlStatus;
    USBLEGCTLSTS            usbLegacySupportControlStatusValue;
    HC_BIOS_OWNED_SEMAPHORE hcBiosOwnedSemaphore;
    HC_OS_OWNED_SEMAPHORE   hcOsOwnedSemaphore;
    ULONG                   count;
    ULONG                   clearTime;
    LARGE_INTEGER           time;
    NTSTATUS                status;

    TRY {

        usbLegSupCapability = RegisterData->UsbLegSupCapability;

        if (usbLegSupCapability == NULL) {

            LogWarning(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                       "Legacy support capability not implemented, BIOS handoff will not be attempted.");

        } else {

            LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                       "Starting BIOS Handoff");

            usbLegacySupport = &usbLegSupCapability->UsbLegacySupportCapability;

            //
            // Unconditionally set the OS_OWN semaphore.
            //
            hcOsOwnedSemaphore.AsUchar8 =
                Register_ReadUchar(&usbLegacySupport->HcOsOwnedSemaphore.AsUchar8);

            hcOsOwnedSemaphore.HcOsOwnedSemaphore = 1;

            Register_WriteUchar(&usbLegacySupport->HcOsOwnedSemaphore.AsUchar8,
                                hcOsOwnedSemaphore.AsUchar8);

            //
            // Poll for the BIOS_OWN semaphore to clear.
            //
            count = DEFAULT_HC_BIOS_WAIT_COUNT;
            clearTime = 0;

            do {

                hcBiosOwnedSemaphore.AsUchar8 =
                    Register_ReadUchar(&usbLegacySupport->HcBiosOwnedSemaphore.AsUchar8);

                if (hcBiosOwnedSemaphore.HcBiosOwnedSemaphore == 0) {

                    LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                            "HC BIOS Owned Semaphore bit cleared after %d ms", clearTime);
                    status = STATUS_SUCCESS;
                    break;
                }

                if (count == 0) {

                    if (Controller_GetDeviceFlags(RegisterData->ControllerHandle).IgnoreBIOSHandoffFailure) {

                        LogWarning(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                                   "HC BIOS Owned Semaphore bit not cleared after %d ms. Ignoring for this controller.",
                                   DEFAULT_HC_BIOS_WAIT_COUNT * DEFAULT_HC_BIOS_WAIT_TIME_MS);

                        status = STATUS_SUCCESS;
                        break;

                    } else {

                        LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                                 "HW_COMPLIANCE: BIOS Handoff failure: BIOS-Owned Semaphore bit not cleared after %d ms",
                                 DEFAULT_HC_BIOS_WAIT_COUNT * DEFAULT_HC_BIOS_WAIT_TIME_MS);

                        NT_ASSERT(0);

                        Etw_StartDeviceFail(RegisterData->ControllerHandle,
                                            NULL,
                                            StartDeviceFailReasonBiosHandoffFail);

                        status = STATUS_UNSUCCESSFUL;
                        LEAVE;
                    }
                }

                count--;
                clearTime += DEFAULT_HC_BIOS_WAIT_TIME_MS;

                //
                // Delay for 100ms before re-reading the register.
                //
                time.QuadPart = WDF_REL_TIMEOUT_IN_MS(DEFAULT_HC_BIOS_WAIT_TIME_MS);
                KeDelayExecutionThread(KernelMode, FALSE, &time);

            } WHILE (TRUE);
        }

        //
        // Controller should be halted after BIOS handoff. If it is not then halt it if
        // the device flag is set.
        //
        usbStatus.AsUlong32 =
            Register_ReadUlong(&RegisterData->OperationalRegisters->UsbStatus.AsUlong32);

        if (usbStatus.HcHalted == FALSE) {

            if (Controller_GetDeviceFlags(RegisterData->ControllerHandle).UseStrictBIOSHandoff) {

                LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                         "HW_COMPLIANCE: Controller not halted after BIOS handoff.");

                Etw_StartDeviceFail(RegisterData->ControllerHandle,
                                    NULL,
                                    StartDeviceFailReasonControllerNotHaltedAfterHandoff);

                status = STATUS_UNSUCCESSFUL;
                LEAVE;

            } else {

                LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                           "HW_COMPLIANCE: Controller not halted after BIOS handoff, attempting to stop.");

                status = Register_ControllerStop(RegisterData);
                if (!NT_SUCCESS(status)) {
                    LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                               "HW_COMPLIANCE: Failed to stop controller %!STATUS!", status);
                    LEAVE;
                }
            }
        }

        //
        // Turn off SMI on OS ownership change. This is to workaround a problem on some
        // systems that on resume from S3, OS ownership change bit gets set, resulting in an
        // SMI storm and system hang. So always turn off this SMI after a BIOS handoff.
        //
        if (usbLegSupCapability != NULL) {

            usbLegacySupportControlStatus = &usbLegSupCapability->UsbLegacySupportControlStatus;

            usbLegacySupportControlStatusValue.AsUlong32 =
                Register_ReadUlong(&usbLegacySupportControlStatus->AsUlong32);

            usbLegacySupportControlStatusValue.SmiOnOsOwnershipEnable = 0;

            //
            // Clear the RW1C bits so as not to overwrite their values.
            //
            usbLegacySupportControlStatusValue.SmiOnOsOnwershipChange = 0;
            usbLegacySupportControlStatusValue.SmiOnPciCommand = 0;
            usbLegacySupportControlStatusValue.SmiOnBar = 0;

            Register_WriteUlong(&usbLegacySupportControlStatus->AsUlong32,
                                usbLegacySupportControlStatusValue.AsUlong32);
        }

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Register_ControllerStop(
    __in
        PREGISTER_DATA  RegisterData
    )
/*++

Routine Description:

    This routine stops the xHCI controller.

--*/
{
    NTSTATUS                status;
    ULONG                   count;
    LARGE_INTEGER           time;
    POPERATIONAL_REGISTERS  operationalRegisters;
    USBCMD                  usbCommand;
    USBSTS                  usbStatus;

    TRY {

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                 "Register_ControllerStop Begin");

        if (Controller_IsControllerAccessible(RegisterData->ControllerHandle) == FALSE) {
            status = STATUS_SUCCESS;
            LEAVE;
        }

        operationalRegisters = Register_GetOperationalRegisters(RegisterData);

        //
        // Stop the xHCI controller.
        // 5.4.1.1 : The xHC should halt within 16 ms. of software clearing the R/S bit to 0.
        //
        usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
        usbCommand.RunStopBit = FALSE;
        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

        count = DEFAULT_STOP_WAIT_COUNT;

        do {

            usbStatus.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

            if (usbStatus.HcHalted == TRUE) {
                LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                        "Controller successfully halted after %d calls to KeDelayExecutionThread (%d)", (DEFAULT_STOP_WAIT_COUNT - count), DEFAULT_STOP_WAIT_TIME_MS);
                status = STATUS_SUCCESS;
                break;
            }

            if (count == 0) {

                LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                         "HW_COMPLIANCE: Controller failed to stop after %d calls to KeDelayExecutionProcessor", DEFAULT_STOP_WAIT_COUNT);
                status = STATUS_UNSUCCESSFUL;
                break;
            }

            //
            // Delay for 200 microseconds before re-reading the register.
            //
            time.QuadPart = WDF_REL_TIMEOUT_IN_MS(DEFAULT_STOP_WAIT_TIME_MS);

            KeDelayExecutionThread(KernelMode, FALSE, &time);

            count--;

        } WHILE (TRUE);


    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Register_ControllerReset(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        BOOLEAN         ResetWhenRunning
    )
/*++

Routine Description:

    This routine resets the xHCI controller.

--*/
{
    NTSTATUS                status;
    ULONG                   count;
    LARGE_INTEGER           time;
    ULONG                   loopWaitTimeMS;
    ULONG                   totalWaitTimeMS;
    USBSTS                  usbStatus;
    USBCMD                  usbCommand;
    POPERATIONAL_REGISTERS  operationalRegisters;

    TRY {

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                   "Starting Controller Reset");

        if (Controller_IsControllerAccessible(RegisterData->ControllerHandle) == FALSE) {
            status = STATUS_SUCCESS;
            LEAVE;
        }

        operationalRegisters = Register_GetOperationalRegisters(RegisterData);

        //
        // Verify that the controller's controller not ready bit is cleared. This bit must
        // be cleared before the xHCI's operational and runtime registers are accessed.
        //
        status = Register_WaitForControllerReady(RegisterData);
        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                       "HW_COMPLIANCE: CNR bit was not cleared! This bit must be cleared before xHC reset. %!STATUS!", status);
            LEAVE;
        }

        if (ResetWhenRunning == FALSE) {
            //
            // Reset the xHCI controller. This can only be done if the controller is
            // currently halted.
            //
            usbStatus.AsUlong32 =
                Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

            if (usbStatus.HcHalted == FALSE) {

                LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                         "HW_COMPLIANCE: Cannot reset XHCI controller because HcHalted bit is 0!");

                NT_ASSERT(0);

                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }
        }

        Register_SetClearSSICPortUnused(RegisterData, FALSE);

        usbCommand.AsUlong32 = 0;
        usbCommand.HostControllerReset = TRUE;
        Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32,
                            usbCommand.AsUlong32);

        count = 0;
        loopWaitTimeMS = 1;
        totalWaitTimeMS = 0;

        status = STATUS_SUCCESS;

        do {

            usbCommand.AsUlong32 =
                Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);

            if (usbCommand.HostControllerReset == FALSE) {
                LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                        "Controller successfully reset after %u ms", totalWaitTimeMS);
                if(totalWaitTimeMS > HW_COMPLIANCE_XHC_RESET_WARN_MS) {
                    LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                               "HW_COMPLIANCE: xHC took a long time to reset! (%u milliseconds)", totalWaitTimeMS);
                }
                break;
            }

            if (count == DEFAULT_RESET_LOOP_WAIT_COUNT) {

                LogFatal(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                         "HW_COMPLIANCE: Reset timed out: After wait time of %u milliseconds, the controller did not reset!", totalWaitTimeMS);

                Etw_StartDeviceFail(RegisterData->ControllerHandle,
                                    NULL,
                                    StartDeviceFailReasonControllerResetTimedOut);

                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            //
            // Delay for loopWaitTimeMS before re-reading the register. loopWaitTimeMS is
            // doubled on subsequent iterations of the loop, upto a maximum value.
            //
            time.QuadPart = WDF_REL_TIMEOUT_IN_MS(loopWaitTimeMS);
            KeDelayExecutionThread(KernelMode, FALSE, &time);

            LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                    "Controller reset retry (%u) loop wait time %u", count, loopWaitTimeMS);

            count++;
            totalWaitTimeMS += loopWaitTimeMS;

            if (loopWaitTimeMS != DEFAULT_RESET_LOOP_WAIT_TIME_MAX) {
                loopWaitTimeMS <<= 1;
            }

        } WHILE (TRUE);

        //
        // 4.2 After Chip Hardware Reset, wait until the Controller Not Ready (CNR) flag
        // in the USBSTS is '0' before writing any xHC Operational or Runtime registers.
        //
        status = Register_WaitForControllerReady(RegisterData);
        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                       "HW_COMPLIANCE: CNR bit is 1 after xHC reset! %!STATUS!", status);
            LEAVE;
        }













        //
        // The CommandFilter object needs to be informed everytime the controller is reset.
        //
        CommandFilter_ControllerResetPostReset(
            Controller_GetCommandFilterHandle(RegisterData->ControllerHandle));

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Register_WaitForControllerReady(
    __in
        PREGISTER_DATA  RegisterData
    )
/*++

Routine Description:

    Ensure that the xHC is ready for use by polling on the Controller Not Ready (CNR)
    flag in the USBSTS register to get to '0'.

--*/
{
    NTSTATUS                status;
    ULONG                   count;
    ULONG                   clearTime;
    LARGE_INTEGER           time;
    USBSTS                  usbStatus;
    POPERATIONAL_REGISTERS  operationalRegisters;

    TRY {

        LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                   "Starting WaitForControllerReady");

        if (Controller_IsControllerAccessible(RegisterData->ControllerHandle) == FALSE) {
            status = STATUS_SUCCESS;
            LEAVE;
        }

        operationalRegisters = Register_GetOperationalRegisters(RegisterData);

        count = DEFAULT_CNR_WAIT_COUNT;
        clearTime = 0;

        do {

            usbStatus.AsUlong32 =
                Register_ReadUlong(&operationalRegisters->UsbStatus.AsUlong32);

            if (usbStatus.ControllerNotReady == FALSE) {

                LogInfo(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                        "ControllerNotReady bit cleared after %d ms", clearTime);
                break;
            }

            if (count == 0) {

                NT_ASSERT(0);
                LogError(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                         "HW_COMPLIANCE: ControllerNotReady bit not cleared after waiting %d ms",
                         DEFAULT_CNR_WAIT_COUNT * DEFAULT_CNR_WAIT_TIME);
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            //
            // Delay for 100ms before re-reading the register.
            //
            time.QuadPart = WDF_REL_TIMEOUT_IN_MS(DEFAULT_CNR_WAIT_TIME);
            KeDelayExecutionThread(KernelMode, FALSE, &time);

            count--;
            clearTime += DEFAULT_CNR_WAIT_TIME;

        } WHILE (TRUE);

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
PEXTENDED_CAPABILITY
Register_FindFirstExtendedCapability(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        UCHAR                   CapabilityID
    )
/*++

Routine Description:

    This routine finds the first extended capability matching the
    CapabilityID parameter.

Return Value:

    A pointer to the first matching extended capability, if one is
    found, otherwise NULL.

--*/
{
    PEXTENDED_CAPABILITY    extendedCapabilityPointer;
    EXTENDED_CAPABILITY     extendedCapability;

    TRY {

        extendedCapabilityPointer = RegisterData->ExtendedCapabilityRegisters;

        if (extendedCapabilityPointer == NULL) {
            LEAVE;
        }

        do {

            extendedCapability.AsUlong32 =
                Register_ReadUlong(&extendedCapabilityPointer->AsUlong32);

            if (extendedCapability.CapabilityID == CapabilityID) {

                //
                // Found a matching extended capability.
                //
                break;
            }

            if (extendedCapability.NextCapabilityPointer != 0) {

                //
                // Advance to the next extended capability in the list.
                //
                extendedCapabilityPointer =
                    (PEXTENDED_CAPABILITY)((PUCHAR)extendedCapabilityPointer +
                                           extendedCapability.NextCapabilityPointer *
                                           sizeof(ULONG));

                if ((PUCHAR)extendedCapabilityPointer >=
                    (PUCHAR)RegisterData->CapabilityRegisters + RegisterData->RegisterMapLength) {

                    //
                    // Invalid extended capability pointer beyond the
                    // end of mapped register space.
                    //
                    extendedCapabilityPointer = NULL;
                    break;
                }

            } else {

                //
                // Did not find a matching extended capability before
                // reaching the end of the list.
                //
                extendedCapabilityPointer = NULL;
                break;
            }

        } WHILE (TRUE);

    } FINALLY {

    }

    return extendedCapabilityPointer;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
PEXTENDED_CAPABILITY
Register_FindNextExtendedCapability(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        PEXTENDED_CAPABILITY    ExtendedCapabilityPointer,
    __in
        UCHAR                   CapabilityID
    )
/*++

Routine Description:

    This routine finds the next extended capability matching the
    CapabilityID parameter after the ExtendedCapabilityPointer
    parameter.

Return Value:

    A pointer to the next matching extended capability, if one is
    found, otherwise NULL.

--*/
{
    EXTENDED_CAPABILITY     extendedCapability;

    TRY {

        if (ExtendedCapabilityPointer == NULL) {
            LEAVE;
        }

        //
        // Read the starting extended capability header to get the next
        // extended capability header pointer.  The starting extended
        // capability will be skipped over before looking for the next
        // mataching extended capability to return.
        //
        extendedCapability.AsUlong32 =
            Register_ReadUlong(&ExtendedCapabilityPointer->AsUlong32);

        do {

            if (extendedCapability.NextCapabilityPointer != 0) {

                //
                // Advance to the next extended capability in the list.
                //
                ExtendedCapabilityPointer =
                    (PEXTENDED_CAPABILITY)((PUCHAR)ExtendedCapabilityPointer +
                                           extendedCapability.NextCapabilityPointer *
                                           sizeof(ULONG));

                if ((PUCHAR)ExtendedCapabilityPointer >=
                    (PUCHAR)RegisterData->CapabilityRegisters + RegisterData->RegisterMapLength) {

                    //
                    // Invalid extended capability pointer beyond the
                    // end of mapped register space.
                    //
                    ExtendedCapabilityPointer = NULL;
                    break;
                }

                //
                // Read the next extended capability header and then
                // check for a matching extended capability to return.
                //
                extendedCapability.AsUlong32 =
                    Register_ReadUlong(&ExtendedCapabilityPointer->AsUlong32);

                if (extendedCapability.CapabilityID == CapabilityID) {

                    //
                    // Found a matching extended capability.
                    //
                    break;
                }

            } else {

                //
                // Did not find a matching extended capability before
                // reaching the end of the list.
                //
                ExtendedCapabilityPointer = NULL;
                break;
            }

        } WHILE (TRUE);

    } FINALLY {

    }

    return ExtendedCapabilityPointer;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
ULONG
Register_GetExtendedCapabilityTotalSize(
    __in
        PREGISTER_DATA          RegisterData
    )
/*++

Routine Description:

    This routine finds the total size of extended capability in the xHCI controller.

--*/
{
    PSUPPPORTED_PROTOCOL_CAPABILITY                   supportedProtocolPointer;
    SUPPPORTED_PROTOCOL_CAPABILITY_PORT_OFFSET_COUNT  portOffsetCount;
    PEXTENDED_CAPABILITY                              extendedCapabilityPointer;
    EXTENDED_CAPABILITY                               extendedCapability;
    ULONG                                             extendedCapabilitySize;

    TRY {

        extendedCapabilitySize = 0;

        extendedCapabilityPointer = RegisterData->ExtendedCapabilityRegisters;

        if (extendedCapabilityPointer == NULL) {
            LEAVE;
        }

        do {

            extendedCapability.AsUlong32 =
                Register_ReadUlong(&extendedCapabilityPointer->AsUlong32);

            if (extendedCapability.NextCapabilityPointer != 0) {

                //
                // Advance to the next extended capability in the list.
                //
                extendedCapabilityPointer =
                    (PEXTENDED_CAPABILITY)((PUCHAR)extendedCapabilityPointer +
                                           extendedCapability.NextCapabilityPointer *
                                           sizeof(ULONG));

                if ((PUCHAR)extendedCapabilityPointer >=
                    (PUCHAR)RegisterData->CapabilityRegisters + RegisterData->RegisterMapLength) {

                    //
                    // Invalid extended capability pointer beyond the
                    // end of mapped register space.
                    //
                    extendedCapabilitySize = 0;
                    break;
                }

            } else {

                //
                // This is the last extended capability. Add the extended capability size
                // up to this point plus the final extended capability, if known.
                //
                extendedCapabilitySize = (ULONG)((PUCHAR)extendedCapabilityPointer -
                                                 (PUCHAR)RegisterData->ExtendedCapabilityRegisters);

                switch (extendedCapability.CapabilityID) {

                    //
                    // This is the final extended capability. Only the USB legacy
                    // support and supported protocol are fully returned, so
                    // the size is adjusted. For the other capability types, only
                    // the extended capability pointer is returned.
                    //
                    case EXTENDED_CAPABILITY_ID_USB_LEGACY_SUPPORT:
                        extendedCapabilitySize += sizeof(USBLEGSUPCAPABILITY);
                        break;

                    case EXTENDED_CAPABILITY_ID_SUPPORTED_PROTOCOL:
                        supportedProtocolPointer = (PSUPPPORTED_PROTOCOL_CAPABILITY)&extendedCapabilityPointer->AsUlong32;
                        portOffsetCount.AsUlong32 =
                            Register_ReadUlong((PULONG)&supportedProtocolPointer->PortOffsetCount.AsUlong32);
                        extendedCapabilitySize += sizeof(SUPPPORTED_PROTOCOL_CAPABILITY);
                        extendedCapabilitySize += portOffsetCount.SpeedIDCount * sizeof(SUPPPORTED_PROTOCOL_CAPABILITY_SPEED);
                        break;

                    case EXTENDED_CAPABILITY_ID_USB_DEBUG_CAPABILITY:
                    case EXTENDED_CAPABILITY_ID_EXTENDED_POWER_MANAGEMENT:
                    case EXTENDED_CAPABILITY_ID_IO_VIRTUALIZATION:
                    case EXTENDED_CAPABILITY_ID_MESSAGE_INTERRUPT:
                    case EXTENDED_CAPABILITY_ID_EXTENDED_MESSAGE_INTERRUPT:
                    default:
                        extendedCapabilitySize += sizeof(EXTENDED_CAPABILITY);
                        break;
                }

                break;
            }

        } WHILE (TRUE);

    } FINALLY {

    }

    return extendedCapabilitySize;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Register_GetAllExtendedCapability(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        PEXTENDED_CAPABILITY    ExtendedCapabilityBuffer
    )
/*++

Routine Description:

    This routine reads in all of the extended capabilities up to the
    final extended capability or end of the specified buffer.

    This function assumes that the buffer passed in is at least the size
    returned from Register_GetExtendedCapabilityTotalSize.

--*/
{
    PSUPPPORTED_PROTOCOL_CAPABILITY  supportedProtocol;
    PEXTENDED_CAPABILITY             extendedCapabilityPointer;

    TRY {

        extendedCapabilityPointer = RegisterData->ExtendedCapabilityRegisters;

        if (extendedCapabilityPointer == NULL) {
            LEAVE;
        }

        do {

            ExtendedCapabilityBuffer->AsUlong32 =
                Register_ReadUlong(&extendedCapabilityPointer->AsUlong32);

            LogVerbose(Controller_GetRecorderLog(RegisterData->ControllerHandle), Register,
                       "CapabilityID %d NextCapabilityPointer %d",
                       ExtendedCapabilityBuffer->CapabilityID,
                       ExtendedCapabilityBuffer->NextCapabilityPointer);

            switch (ExtendedCapabilityBuffer->CapabilityID) {

                //
                // Only USB legacy support and supported protocol are
                // fully returned from the extended capability list.
                // For the other capability types, only the extended
                // capability pointer is returned.
                //
                case EXTENDED_CAPABILITY_ID_USB_LEGACY_SUPPORT:

                    Register_ReadBufferUlong(&extendedCapabilityPointer->AsUlong32,
                                             (PULONG)ExtendedCapabilityBuffer,
                                             sizeof(USBLEGSUPCAPABILITY)/sizeof(ULONG));
                    break;

                case EXTENDED_CAPABILITY_ID_SUPPORTED_PROTOCOL:

                    Register_ReadBufferUlong(&extendedCapabilityPointer->AsUlong32,
                                             (PULONG)ExtendedCapabilityBuffer,
                                             sizeof(SUPPPORTED_PROTOCOL_CAPABILITY)/sizeof(ULONG));

                    supportedProtocol = (PSUPPPORTED_PROTOCOL_CAPABILITY)ExtendedCapabilityBuffer;
                    if (supportedProtocol->PortOffsetCount.SpeedIDCount != 0) {
                        Register_ReadBufferUlong(&extendedCapabilityPointer->AsUlong32,
                                                 (PULONG)SUPPORTED_PROTOCOL_SPEED_POINTER(supportedProtocol),
                                                 supportedProtocol->PortOffsetCount.SpeedIDCount *
                                                  (sizeof(SUPPPORTED_PROTOCOL_CAPABILITY_SPEED)/sizeof(ULONG)));
                    }
                    break;

                default:
                    break;
            }

            if (ExtendedCapabilityBuffer->NextCapabilityPointer != 0) {

                //
                // Advance to the next extended capability in the list.
                //
                extendedCapabilityPointer =
                    (PEXTENDED_CAPABILITY)((PUCHAR)extendedCapabilityPointer +
                                           ExtendedCapabilityBuffer->NextCapabilityPointer *
                                           sizeof(ULONG));

                if ((PUCHAR)extendedCapabilityPointer >=
                    (PUCHAR)RegisterData->CapabilityRegisters + RegisterData->RegisterMapLength) {

                    //
                    // Invalid extended capability pointer beyond the
                    // end of mapped register space.
                    //
                    break;
                }

                //
                // Advance buffer to the next extended capability in the list.
                //
                ExtendedCapabilityBuffer =
                    (PEXTENDED_CAPABILITY)((PUCHAR)ExtendedCapabilityBuffer +
                                           ExtendedCapabilityBuffer->NextCapabilityPointer *
                                           sizeof(ULONG));

            } else {

                //
                // No more extended capabilities.
                //
                break;
            }

        } WHILE (TRUE);

    } FINALLY {

    }

    return;
}

