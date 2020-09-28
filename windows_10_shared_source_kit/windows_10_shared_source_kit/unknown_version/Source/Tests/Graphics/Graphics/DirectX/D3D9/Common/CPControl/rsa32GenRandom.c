//
// Shim function for RSA32
//
// The RSA32.lib library somtimes needs random numbers
// In the past it called NewGenRandom in Randlib.lib,
// but randlib has been depricated.
// It now calls Rsa32GenRandom, and the caller has to 
// supply the function.
// This file provides a compatibility shim that we use
// for those areas which still use Randlib.
//

#include <windows.h>
#include <ntsecapi.h>

VOID
Rsa32GenRandom( PBYTE pbBuffer, DWORD cbBuffer )
{
    if( !RtlGenRandom( pbBuffer, cbBuffer ) )
    {
        RaiseException( 'prng', EXCEPTION_NONCONTINUABLE, 0, NULL );
    }
}

