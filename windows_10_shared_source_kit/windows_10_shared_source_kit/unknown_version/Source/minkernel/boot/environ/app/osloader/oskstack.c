/*++

Copyright (c) Microsoft Corporation

Module Name:

    oskstack.c

Abstract:

    NTOS Kernel stack support.

Environment:

    Boot Environment.

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"
#include <hvhdk_mini.h>

// ------------------------------------------------------------------ Functions

#if defined(_X86_) || defined (_AMD64_)

#define ROUNDUP64(x) (((x) + 63) & ~63)


ULONG
OslpArchDetectXSaveAreaSize (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    Determines the footprint of the FXSAVE (or XSAVE, if available) buffer,
    including all available features.

    N.B. Determining footprint of the FXSAVE (or XSAVE) buffer is done
    during kernel initialization also.

Arguments:

    None.

Return Value:

    Size of the FXSAVE/XSAVE buffer.

--*/

{

    ULONG64 ConfigAllowedFeatures;
    ULONG ConfigExtendedStateFlags;
    ULONG64 CpuEnabledUserFeatures;
    ULONG64 CpuEnabledSupervisorFeatures;
    ULONG64 CpuEnabledAllFeatures;
    CPUID_INFO CpuInfo;
    ULONG Index;
    ULONG XSaveSize;

    //
    // Check if XSAVE is even supported.
    //

    XSaveSize = FIELD_OFFSET(XSAVE_AREA, Header);
    CpuEnabledUserFeatures = 0;
    CpuEnabledSupervisorFeatures = 0;
    BlArchCpuId(0x1, 0, &CpuInfo);
    if ((CpuInfo.Ecx & (1UL << 26)) == 0) {
        goto calc_complete;
    }

    //
    // XSAVE is supported. Determine the user and supervisor features enabled.
    //

    XSaveSize += sizeof(XSAVE_AREA_HEADER);
    ConfigAllowedFeatures = LoaderBlock->Extension->XsaveAllowedFeatures;
    ConfigExtendedStateFlags = LoaderBlock->Extension->XsaveFlags;

    BlArchCpuId(0xd, 0, &CpuInfo);
    CpuEnabledUserFeatures = ((ULONG64)(CpuInfo.Eax)) |
                             (((ULONG64)(CpuInfo.Edx)) << 32);

    CpuEnabledUserFeatures = CpuEnabledUserFeatures & (ConfigAllowedFeatures | XSTATE_MASK_LEGACY);

    //
    // If XSAVEC/XSAVES are supported and has been enabled, retrieve the
    // supervisor features available, and calculate size of XSAVE buffer in
    // compacted format.
    //

    BlArchCpuId(0xd, 1, &CpuInfo);
    if (((CpuInfo.Eax & (1UL << 1)) && (CpuInfo.Eax & (1UL << 3))) &&
        ((ConfigExtendedStateFlags & XSAVE_FLAG_ENABLE_XSAVEC) != 0)) {

        CpuEnabledSupervisorFeatures = ((ULONG64)(CpuInfo.Ecx)) |
                                       (((ULONG64)(CpuInfo.Edx)) << 32);

        CpuEnabledSupervisorFeatures = CpuEnabledSupervisorFeatures & ConfigAllowedFeatures;
        CpuEnabledAllFeatures = CpuEnabledUserFeatures | CpuEnabledSupervisorFeatures;
        for (Index = XSTATE_GSSE; Index < MAXIMUM_XSTATE_FEATURES; Index += 1) {
            if ((CpuEnabledAllFeatures & (1ui64 << Index)) != 0) {
                BlArchCpuId(0xd, Index, &CpuInfo);

                //
                // Checking if ECX[1] is set to 1. This indicates if the start offset of the state
                // in the XSAVE buffer should be aligned to a 64 byte boundary.
                //

                if ((CpuInfo.Ecx & XSTATE_ALIGN_MASK) != 0) {
                    XSaveSize = ROUNDUP64(XSaveSize);
                }

                XSaveSize += CpuInfo.Eax;
            }
        }

        goto calc_complete;
    }

    //
    // If XSAVE is supported, but not XSAVEC/XSAVES, calculate size of XSAVE
    // buffer in legacy format.
    //

    for (Index = XSTATE_GSSE; Index < MAXIMUM_XSTATE_FEATURES; Index += 1) {
        if (CpuEnabledUserFeatures & (1ui64 << Index)) {
            BlArchCpuId(0xd, Index, &CpuInfo);
            if (XSaveSize < CpuInfo.Ebx + CpuInfo.Eax) {
                XSaveSize = CpuInfo.Ebx + CpuInfo.Eax;
            }
        }
    }

calc_complete:

    LoaderBlock->Extension->XsaveAllowedFeatures = CpuEnabledUserFeatures |
                                                   CpuEnabledSupervisorFeatures;

    return XSaveSize;
}

VOID
OslReadXsaveRegistryValues(
    __in ULONG SystemHiveHandle,
    __out PLOADER_PARAMETER_BLOCK LoaderBlock
    )
{

    ULONG64 AllowedFeatures;
    ULONG ExtendedStateFlags;

    //
    // N.B. These values are filled in for Xbox System OS in
    // %BaseDir%\xbox\lnm\bldtools\makevbi\paramblk.cpp:
    //                                            VbiBuildLoaderParameterBlock.
    //

    if (!OslGetExtendedStateAllowedFeatures(SystemHiveHandle,
                                            &AllowedFeatures)) {

        //
        // If AllowedFeatures registry value is not defined, Allow x87, SSE, AVX
        // MPX and AVX512.
        //

        AllowedFeatures = XSTATE_MASK_LEGACY |
                          XSTATE_MASK_AVX |
                          XSTATE_MASK_MPX |
                          XSTATE_MASK_AVX512;

    } else {
        AllowedFeatures &= XSTATE_MASK_ALLOWED;
        AllowedFeatures |= XSTATE_MASK_LEGACY;
    }

    if (!OslGetExtendedStateFlags(SystemHiveHandle, &ExtendedStateFlags)) {

        //
        // If Flags registry value is not defined, assume xsaveopt and xsavec
        // are allowed.
        //

        ExtendedStateFlags = XSAVE_FLAG_ENABLE_XSAVEOPT |
                             XSAVE_FLAG_ENABLE_XSAVEC;

    }

    //
    // C requires OPT
    //

    if ((ExtendedStateFlags & XSAVE_FLAG_ENABLE_XSAVEOPT) == 0) {
        ExtendedStateFlags &= ~XSAVE_FLAG_ENABLE_XSAVEC;
    }

#if defined(_AMD64_)

    if (OslArchHvlInitialized() != FALSE) {
        AllowedFeatures &= HV_X64_XSAVE_SUPPORTED_FEATURES;
    }

#endif

    LoaderBlock->Extension->XsaveFlags = ExtendedStateFlags;
    LoaderBlock->Extension->XsaveAllowedFeatures = AllowedFeatures;

    return;
}

#endif

ULONG
OslDetermineKernelStackSize (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    Determines the footprint of the FXSAVE (or XSAVE, if available) buffer,
    including all available features.

Arguments:

    None.

Return Value:

    Size of the FXSAVE/XSAVE buffer.

--*/

{

#if defined(_X86_) || defined (_AMD64_)

    ULONG StackSize;
    ULONG XSaveSize;

    XSaveSize = OslpArchDetectXSaveAreaSize(LoaderBlock);
    if (XSaveSize <= XSTATE_STACK_NEW_PAGE_THRESHOLD) {
        return KERNEL_STACK_SIZE;
    }

    StackSize = KERNEL_STACK_SIZE;
    StackSize += ALIGN_RANGE_UP(XSaveSize - XSTATE_STACK_NEW_PAGE_THRESHOLD,
                                PAGE_SIZE);

    ASSERT(StackSize > KERNEL_STACK_SIZE);

    if (StackSize > (KERNEL_LARGE_STACK_SIZE - PAGE_SIZE)) {
        return (KERNEL_LARGE_STACK_SIZE - PAGE_SIZE);
    }

    return StackSize;

#else

    UNREFERENCED_PARAMETER(LoaderBlock);

    return KERNEL_STACK_SIZE;

#endif

}

