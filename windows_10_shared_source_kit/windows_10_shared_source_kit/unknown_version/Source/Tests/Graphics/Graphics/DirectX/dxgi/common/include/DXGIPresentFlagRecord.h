#pragma once

#include <windows.h>
#include <dxgi.h>
#include <WGFTestCore.h>
#include <WGFTestUI.h>


struct DXGIPresentFlagRecord
{
	UINT dxgiPresentFlag;
	const char * szPresentFlagName;
};

extern DXGIPresentFlagRecord DXGIPresentFlags[];

extern const UINT cuiNoOfPresentFlags;

bool operator == (const DXGIPresentFlagRecord& lhs, const DXGIPresentFlagRecord& rhs);
bool __stdcall FromString(DXGIPresentFlagRecord *pDXGIPresentFlagRecord, const tstring &presentFlagRecord);
tstring __stdcall ToString(const DXGIPresentFlagRecord &presentFlagRecord);



