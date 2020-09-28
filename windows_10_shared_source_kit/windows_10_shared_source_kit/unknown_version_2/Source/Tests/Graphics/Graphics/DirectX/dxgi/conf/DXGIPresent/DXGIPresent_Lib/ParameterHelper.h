//-----------------------------------------------------------------------------
// 
//  ToString
//
//-----------------------------------------------------------------------------

#pragma once
// Framework headers
#include "WGFTestCore.h"
#include "WGFTestUI.h"

#include <RegionHelper.h>

bool __stdcall FromString(RECT *pType, const tstring &str);
tstring __stdcall ToString(RECT resource);

bool __stdcall FromString(RegionHelper *pType, const tstring &str);
tstring __stdcall ToString(RegionHelper resource);
bool __stdcall operator== (const RegionHelper &rhs, const RegionHelper &lhs);
