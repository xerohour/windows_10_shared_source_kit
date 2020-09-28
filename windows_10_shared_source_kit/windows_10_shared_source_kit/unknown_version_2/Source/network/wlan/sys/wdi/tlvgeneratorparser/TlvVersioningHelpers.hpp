nctions_Reference;
    Interface->InterfaceDereference = PciCrashdumpFunctions_Dereference;

    //
    // Crashdump functions interface private.
    //

    Interface->PowerOn = PciCrashdumpFunctions_PowerOn;
    return STATUS_SUCCESS;
}

VOID
PciCrashdumpFunctions_Reference (
    __inout PVOID Context
    )

{

    LONG Result;

    UNREFERENCED_PARAMETER(Context);

    Result = InterlockedIncrement(&PciCrashdumpFunctionsReferenceCount);

    PCI_ASSERT(Result > 0);

    return;
}

VOID
PciCrashdumpFunctions_Dereference (
    __inout PVOID Context
    )

{

    LONG Result;

    UNREFERENCED_PARAMETER(Context);

    Result = InterlockedDecrement(&PciCrashdumpFunctionsReferenceCount);

    PCI_ASSERT(Result >= 0);

    return;
}

NTSTATUS
PciCrashdumpFunctions_PowerOn (
    _In_opt_ PVOID Context
    )

{

    UNREFERENCED_PARAMETER(Context);

    return PciEarlyRestoreResourcesInternal(PCI_EARLY_RESTORE_CRASHDUMP,
                                            FALSE);
}

NTSTATUS
PciInterface_Noop(
    __in PVOID Context
    )

/*++

Routine Description:

    This routine is a no-op interface reference/dereference routine for
    ReenumerateSelf and DeviceReset interfaces.

Arguments:

    Context - Unused.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Context);

    return STATUS_SUCCESS;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciReenumerateSelfInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )

/*++

Routine Description:

    This routine initializes the ReenumerateSelf interface.

Arguments:

    DeviceExtension - Supplies the device extension pointer.

    InterfaceSpecificData - Not used.

    Version - Supplies the interface version.

    Size - Supplies the size of the provided interface buffer.

    InterfaceReturn - Supplies a pointer to a variable that receives the
        interface.

    Filtered - Not supported for this interface.

Return Value:

    NT Status value.

--*/

{

    PREENUMERATE_SELF_INTERFACE_STANDARD Interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(REENUMERATE_SELF_INTERFACE_STANDARD)) {
        return STATUS_INVALID_PARAMETER;
    }

    Interface = (PREENUMERATE_SELF_INTERFACE_STANDARD)InterfaceReturn;
    Interface->Size = sizeof(REENUMERATE_SELF_INTERFACE_STANDARD);
    Interface->Context = DeviceExtension;
    Interface->InterfaceReference = PciInterface_Noop;
    Interface->InterfaceDereference = PciInterface_Noop;
    Interface->SurpriseRemoveAndReenumerateSelf = PciSurpriseRemoveAndReenumerateSelf;

    return STATUS_SUCCESS;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
PciDeviceResetInterface_DeviceReset(
    _In_ PVOID Context,
    _In_ DEVICE_RESET_TYPE ResetType,
    _In_ ULONG Flags,
    _In_opt_ PVOID Parameters
    )

/*++

Routine Description:

    This routine handles device reset requests and calls appropriate handler
    for specified reset type.

Arguments:

    Context - Supplies pointer to PCI_DEVICE stored in the
                   interface returned by QueryInterface IRP.

    ResetType - Supplies the type of the reset to perform.

    Flags - Not used. Must be zero.

    Parameters - Supplies additional reset type dependent parameters.

Return Value:

    NTSTATUS

--*/

{

    PPCI_DEVICE Device;
    IO_STACK_LOCATION IrpStack;
    DEVICE_RESET_INTERFACE_STANDARD ResetInterface;
    PFUNCTION_LEVEL_DEVICE_RESET_PARAMETERS ResetParameters;
    USHORT Size;
    NTSTATUS Status;

    Device = (PPCI_DEVICE)Context;

    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    if (ResetType == FunctionLevelDeviceReset) {
        Status = PciAttemptDeviceReset(Device);

        NT_ASSERT(Status != STATUS_PENDING);

        ResetParameters = (PFUNCTION_LEVEL_DEVICE_RESET_PARAMETERS)Parameters;
        if ((ResetParameters != NULL) &&
            (ResetParameters->Size >= sizeof(*ResetParameters))) {

            ResetParameters->DeviceResetCompletion(
                Status,
                ResetParameters->CompletionContext
                );
        }

    //
    // For platform-level device reset (PLDR) requests, query for the PLDR
    // interface from the parent and issue PLDR on the parent. PLDR requests
    // should only be forwarded one level up to the parent port/bridge (to
    // avoid PLDR going up to the root).
    //

    } else if ((ResetType == PlatformLevelDeviceReset) &&
               (Device->HeaderType == PCI_DEVICE_TYPE)) {

        Size = sizeof(DEVICE_RESET_INTERFACE_STANDARD);
        RtlZeroMemory(&ResetInterface, Size);
        IrpStack.MajorFunction = IRP_MJ_PNP;
        IrpStack.MinorFunction = IRP_MN_QUERY_INTERFACE;
        IrpStack.Parameters.QueryInterface.InterfaceType =
            (LPGUID)&GUID_DEVICE_RESET_INTERFACE_STANDARD;

        IrpStack.Parameters.QueryInterface.Version =
            DEVICE_RESET_INTERFACE_VERSION;

        IrpStack.Parameters.QueryInterface.Size = Size;
        IrpStack.Parameters.QueryInterface.Interface =
            (PINTERFACE)&ResetInterface;

        IrpStack.Parameters.QueryInterface.InterfaceSpecificData = NULL;
        Status = PciSendPnpIrp(Device->Parent->DeviceObject, &IrpStack, NULL);
        if (NT_SUCCESS(Status) && (ResetInterface.DeviceReset != NULL)) {
            Status = ResetInterface.DeviceReset(
                         ResetInterface.Context,
                         PlatformLevelDeviceReset,
                         Flags,
                         NULL
                         );

            if (ResetInterface.InterfaceDereference != NULL) {
                ResetInterface.InterfaceDereference(ResetInterface.Context);
            }
        }

    } else {

        Status = STATUS_NOT_SUPPORTED;
    }

    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciDeviceResetInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )

/*++

Routine Description:

    This routine initializes the device reset interface.

Arguments:

    DeviceExtension - Supplies the device extension pointer.

    InterfaceSpecificData - Not used.

    Version - Supplies the interface version.

    Size - Supplies the size of the provided interface buffer.

    InterfaceReturn - Supplies a pointer to a variable that receives the
        interface.

    Filtered - Not supported for this interface.

Return Value:

    NT Status value.

--*/

{

    PDEVICE_RESET_INTERFACE_STANDARD Interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(DEVICE_RESET_INTERFACE_STANDARD)) {
        return STATUS_INVALID_PARAMETER;
    }

    Interface = (PDEVICE_RESET_INTERFACE_STANDARD)InterfaceReturn;
    Interface->Size = sizeof(DEVICE_RESET_INTERFACE_STANDARD);
    Interface->Context = DeviceExtension;
    Interface->InterfaceReference = PciInterface_Noop;
    Interface->InterfaceDereference = PciInterface_Noop;
    Interface->DeviceReset = PciDeviceResetInterface_DeviceReset;
    Interface->SupportedResetTypes =
        PciQuerySupportedResetTypes(DeviceExtension);

    return STATUS_SUCCESS;
}

_Function_class_(PCI_SET_ACS)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
PciSecurityInterface_SetAcs (
    _Inout_ PVOID Context,
    _In_ PCI_ACS_BIT EnableSourceValidation,
    _In_ PCI_ACS_BIT EnableTranslationBlocking,
    _In_ PCI_ACS_BIT EnableP2PRequestRedirect,
    _In_ PCI_ACS_BIT EnableCompletionRedirect,
    _In_ PCI_ACS_BIT EnableUpstreamForwarding,
    _In_ PCI_ACS_BIT EnableEgressControl,
    _In_ PCI_ACS_BIT EnableDirectTranslatedP2P
    )

/*++

Routine Description:

    This routine is a wrapper, in Interface Normal Form, for
    PciVirtualizationConfigureAcs.

Arguments:

    Context - Supplies pointer to PCI_DEVICE stored in the
                   interface returned by QueryInterface IRP.

    The rest of the parameters are values of bits described in the ACS part
        of the PCI Express specification.

Return Value:

    NT Status value.

--*/

{
    PPCI_DEVICE Device;
    PCI_EXPRESS_ACS_CONTROL BitsToModify = {0};
    PCI_EXPRESS_ACS_CONTROL BitState = {0};

    PAGED_CODE();

    Device = (PPCI_DEVICE)Context;

    if (EnableSourceValidation != PciAcsBitDontCare) {
        BitsToModify.SourceValidation = 1;
        if (EnableSourceValidation == PciAcsBitEnable) {
            BitState.SourceValidation = 1;
        }
    }

    if (EnableTranslationBlocking != PciAcsBitDontCare) {
        BitsToModify.TranslationBlocking = 1;
        if (EnableTranslationBlocking == PciAcsBitEnable) {
            BitState.TranslationBlocking = 1;
        }
    }

    if (EnableP2PRequestRedirect != PciAcsBitDontCare) {
        BitsToModify.RequestRedirect = 1;
        if (EnableP2PRequestRedirect == PciAcsBitEnable) {
            BitState.RequestRedirect = 1;
        }
    }

    if (EnableCompletionRedirect != PciAcsBitDontCare) {
        BitsToModify.CompletionRedirect = 1;
        if (EnableCompletionRedirect == PciAcsBitEnable) {
            BitState.CompletionRedirect = 1;
        }
    }

    if (EnableUpstreamForwarding != PciAcsBitDontCare) {
        BitsToModify.UpstreamForwarding = 1;
        if (EnableUpstreamForwarding == PciAcsBitEnable) {
            BitState.UpstreamForwarding = 1;
        }
    }

    if (EnableEgressControl != PciAcsBitDontCare) {
        BitsToModify.EgressControl = 1;
        if (EnableEgressControl == PciAcsBitEnable) {
            BitState.EgressControl = 1;
        }
    }

    if (EnableDirectTranslatedP2P != PciAcsBitDontCare) {
        BitsToModify.DirectTranslation = 1;
        if (EnableDirectTranslatedP2P == PciAcsBitEnable) {
            BitState.DirectTranslation = 1;
        }
    }

    PciVirtualizationConfigureAcs(Device, BitsToModify, BitState);

    return STATUS_SUCCESS;
}


_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciSecurityInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )

/*++

Routine Description:

    This routine initializes the security interface.

Arguments:

    DeviceExtension - Supplies the device extension pointer.

    InterfaceSpecificData - Not used.

    Version - Supplies the interface version.

    Size - Supplies the size of the provided interface buffer.

    InterfaceReturn - Supplies a pointer to a variable that receives the
        interface.

    Filtered - Not supported for this interface.

Return Value:

    NT Status value.

--*/

{

    PPCI_SECURITY_INTERFACE Interface;
    NTSTATUS Status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(PCI_SECURITY_INTERFACE)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // As long as there is only an ACS-related function in the interface, check
    // for ACS support before returning the interface.
    //

    Status = PciVirtualizationCheckAcsSupport((PPCI_DEVICE)DeviceExtension);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Interface = (PPCI_SECURITY_INTERFACE)InterfaceReturn;
    Interface->Size = sizeof(PCI_SECURITY_INTERFACE);
    Interface->Context = DeviceExtension;
    Interface->InterfaceReference = PciInterface_Noop;
    Interface->InterfaceDereference = PciInterface_Noop;
    Interface->SetAccessControlServices = PciSecurityInterface_SetAcs;

    return STATUS_SUCCESS;
}
#define D3D_OVERLOADS
#include <d3d.h>
#include "Address.h"

//************************************************************************
// Wrap Test functions

CWrapTest::CWrapTest()
{
	m_szTestName = "Texture Address Wrap";
	m_szCommandKey = "Wrap";

    // Init framework options
	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE | PF_ALPHA;
	m_Options.ModeOptions.dwTexExclude = PF_METN;

	// Tell parent class what caps to look for
	dwAddressCapU = D3DPTADDRESSCAPS_WRAP;
	dwAddressCapV = D3DPTADDRESSCAPS_WRAP;
}

bool CWrapTest::Setup(void)
{
	// Turn on Wrap addressing
	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, (DWORD)D3DTADDRESS_WRAP);

	return CAddressTest::Setup();
}
$(OBJ_PATH)\$(O)\CopyTexCSTiled.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /ECopyTexCSTiled -Fh$(OBJ_PATH)\$(O)\CopyTexCSTiled.hpp TiledResourcesCS.hlsl

$(OBJ_PATH)\$(O)\CopyTexCSNormal.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /ECopyTexCSNormal -Fh$(OBJ_PATH)\$(O)\CopyTexCSNormal.hpp TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessRawBufferTiled.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessRawBufferTiled -Fh$(OBJ_PATH)\$(O)\ProcessRawBufferTiled.hpp TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessStructuredBufferTiled.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessStructuredBufferTiled -Fh$(OBJ_PATH)\$(O)\ProcessStructuredBufferTiled.hpp TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessRawBufferNormal.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessRawBufferNormal -Fh$(OBJ_PATH)\$(O)\ProcessRawBufferNormal.hpp TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessStructuredBufferNormal.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessStructuredBufferNormal -Fh$(OBJ_PATH)\$(O)\ProcessStructuredBufferNormal.hpp TiledResourcesCS.hls