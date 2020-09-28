//-----------------------------------------------------------------------------
// 
//  ToString
//
//-----------------------------------------------------------------------------

#pragma once

#include "DXGILatency.h"

bool __stdcall FromString(TestType *pType, const tstring &str);
tstring __stdcall ToString(TestType resource);

bool __stdcall FromString(DXGI_SWAP_EFFECT *pType, const tstring &str);
tstring __stdcall ToString(DXGI_SWAP_EFFECT resource);

