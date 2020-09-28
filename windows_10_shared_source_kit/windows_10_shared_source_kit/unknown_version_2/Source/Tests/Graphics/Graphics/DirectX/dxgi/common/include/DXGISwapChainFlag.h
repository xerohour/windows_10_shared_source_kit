#pragma once

#include <windows.h>
#include <dxgi.h>
#include <WGFTestCore.h>
#include <WGFTestUI.h>


typedef
struct _DXGISwapChainFlagRecord
{
	DXGI_SWAP_CHAIN_FLAG dxgiSwapChainFlag;
	const char * szName;
}
DXGISwapChainFlagRecord;

extern DXGISwapChainFlagRecord DXGISwapChainFlag[];

extern const UINT cuiNoOfSwapChainFlag;

bool operator == (const DXGISwapChainFlagRecord& lhs, const DXGISwapChainFlagRecord& rhs);
bool __stdcall FromString(DXGISwapChainFlagRecord *pDXGIPresentFlagRecord, const tstring &swapEffectRecord);
tstring __stdcall ToString(const DXGISwapChainFlagRecord &swapEffectRecord);


