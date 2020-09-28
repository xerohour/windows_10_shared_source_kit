#ifndef DXGI_TEST_SWAP_EFFECTS_INCLUDED
#define DXGI_TEST_SWAP_EFFECTS_INCLUDED

#include <windows.h>
#include <dxgi.h>
#include <WGFTestCore.h>
#include <WGFTestUI.h>


typedef
struct _DXGISwapEffectRecord
{
	DXGI_SWAP_EFFECT dxgiSwapEffect;
	const char * szSwapEffectName;
}
DXGISwapEffectRecord;

extern DXGISwapEffectRecord DXGISwapEffects[];

extern const UINT cuiNoOfSwapEffects;

bool IsFlipModel(DXGI_SWAP_EFFECT e);
bool IsSequential(DXGI_SWAP_EFFECT e);
bool operator == (const DXGISwapEffectRecord& lhs, const DXGISwapEffectRecord& rhs);
bool __stdcall FromString(DXGISwapEffectRecord *pDXGISwapEffectRecord, const tstring &swapEffectRecord);
tstring __stdcall ToString(const DXGISwapEffectRecord &swapEffectRecord);

#endif
