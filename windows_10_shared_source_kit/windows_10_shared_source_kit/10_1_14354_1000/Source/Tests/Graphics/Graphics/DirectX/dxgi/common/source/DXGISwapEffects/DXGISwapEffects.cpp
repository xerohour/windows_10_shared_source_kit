#include "DXGISwapEffects.h"


DXGISwapEffectRecord DXGISwapEffects[] =
{
	{DXGI_SWAP_EFFECT_DISCARD, "DXGI_SWAP_EFFECT_DISCARD"},
	{DXGI_SWAP_EFFECT_SEQUENTIAL, "DXGI_SWAP_EFFECT_SEQUENTIAL"},
   	{DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL, "DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL"},
    {DXGI_SWAP_EFFECT_FLIP_DISCARD, "DXGI_SWAP_EFFECT_FLIP_DISCARD"}
};

DXGISwapEffectRecord g_DXGISwapEffectUnknown = {(DXGI_SWAP_EFFECT)-1, "Unknown Swap Effect"};

const UINT cuiNoOfSwapEffects = sizeof(DXGISwapEffects) / sizeof(DXGISwapEffects[0]);

bool IsFlipModel(DXGI_SWAP_EFFECT e)
{
    return (e == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL || e == DXGI_SWAP_EFFECT_FLIP_DISCARD);
}

bool IsSequential(DXGI_SWAP_EFFECT e)
{
    return (e == DXGI_SWAP_EFFECT_SEQUENTIAL || e == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL);
}

bool operator == (const DXGISwapEffectRecord& lhs, const DXGISwapEffectRecord& rhs)
{
    bool match = (CSTR_EQUAL == CompareString(
                LOCALE_INVARIANT,
                NORM_IGNORECASE,
                lhs.szSwapEffectName, -1,
                rhs.szSwapEffectName, -1));

    if (lhs.dxgiSwapEffect == rhs.dxgiSwapEffect && match )
	{
		return TRUE;
	}
	
    return FALSE;

};

//****************************************************************************
// FromString 
// Converts string representation of DXGI_SWAP_EFFECT to DXGISwapEffectRecord
//****************************************************************************
bool __stdcall FromString(DXGISwapEffectRecord *pDXGISwapEffectRecord, const tstring &DXGISwapEffectString)
{
    for(unsigned int u = 0; u< cuiNoOfSwapEffects; u++)
    {
        if (tcistring(DXGISwapEffectString) == DXGISwapEffects[u].szSwapEffectName)
        {
            // Found a match!
            *pDXGISwapEffectRecord = DXGISwapEffects[u];
            return true;
        }
    }

    // No match, return false.
    return false;
};

tstring __stdcall ToString(const DXGISwapEffectRecord &DXGISwapEffectRecord)
{
    return tstring(DXGISwapEffectRecord.szSwapEffectName);
};
