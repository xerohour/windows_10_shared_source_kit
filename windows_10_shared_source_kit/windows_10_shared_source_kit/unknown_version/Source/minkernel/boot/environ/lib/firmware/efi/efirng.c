/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efirng.c

Abstract:

    Wrappers for EFI RNG routines.

    All efi call must be made in physical mode.  When a boot environment
    application runs in virtual mode, pointers need to be modified and fixed
    before every EFI call.  Additionally firmware pointers/addresses need to be
    virtually mapped for the caller to read contents.  The wrappers intend to
    hide as much of that work as possible from the caller.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootlibp.h"

//
// -------------------------------------------------------------------- Pragmas
//

//#pragma warning(disable:4152)      // Function pointer to data pointer.

//
// --------------------------------------------------------------- RNG Services
//

NTSTATUS
EfiRngGetRng (
    __in EFI_RNG_PROTOCOL *Protocol,
    __in UINTN RNGValueLength,
    __out_bcount(RNGValueLenth) UINT8 *RNGValue
    )

/*++

Routine Description:

    Wrapper function around the EFI RNG protocol GetRng function
    to take care of the virtual/physical address mappings.

Arguments:

    Protocol - Supplies a pointer to the RNG protocol structure

    RNGValueLength - Supplies the size of the RNGValue buffer. 

    RNGValue - Supplies a pointer to the buffer that receives the 
        random bytes

Return Value:

    STATUS_SUCCESS or error value.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS Pa = {0};
    EFI_RNG_PROTOCOL *ProtocolPa;
    UINT8 *RNGValuePa;

    ProtocolPa = Protocol;
    RNGValuePa = RNGValue;
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Protocol, &Pa);
        ProtocolPa = (PVOID)(ULONG_PTR)Pa.QuadPart;
        BlMmTranslateVirtualAddress(RNGValue, &Pa);
        RNGValuePa = (PVOID)(ULONG_PTR)Pa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested service call.
    //

    EfiStatus = ProtocolPa->GetRNG( ProtocolPa,
                                    NULL,
                                    RNGValueLength,
                                    RNGValuePa );

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}


