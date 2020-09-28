#pragma once

extern "C"
{
#include <ndis.h>

//
// To allow linking with user-mode code, inline any functions rather than
// linking to a kernel-mode LIB
//
#undef NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <Ntintsafe.h>

#include "dot11wdi.h"
}

// Publicly sharable WDI types
#include "wditypes.hpp"

#include "tlvlibwpp.hpp"

#include "TlvGeneratorParser.hpp"

#define WDI_TLV_HEADER_SIZE        4      // sizeof(UINT16) + sizeof(UINT16)
#define WDI_POOL_TAG (ULONG) 'GidW'

#ifndef celems
#define celems(x) (sizeof(x) / sizeof((x)[0]))
#endif
