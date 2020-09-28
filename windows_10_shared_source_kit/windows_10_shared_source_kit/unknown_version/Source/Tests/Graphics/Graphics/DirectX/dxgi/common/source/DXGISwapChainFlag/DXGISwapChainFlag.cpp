#include "DXGISwapChainFlag.h"


DXGISwapChainFlagRecord DXGISwapChainFlag[] =
{
    {(DXGI_SWAP_CHAIN_FLAG)NULL , "NULL"},
    {DXGI_SWAP_CHAIN_FLAG_NONPREROTATED , "DXGI_SWAP_CHAIN_FLAG_NONPREROTATED"},
    {DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH , "DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH"}
};

const UINT cuiNoOfSwapChainFlag = sizeof(DXGISwapChainFlag) / sizeof(DXGISwapChainFlag[0]);

bool operator == (const DXGISwapChainFlagRecord& lhs, const DXGISwapChainFlagRecord& rhs)
{
    bool match = (CSTR_EQUAL == CompareString(
                LOCALE_INVARIANT,
                NORM_IGNORECASE,
                lhs.szName, -1,
                rhs.szName, -1));

    if (lhs.dxgiSwapChainFlag == rhs.dxgiSwapChainFlag && match )
    {
        return TRUE;
    }
    
    return FALSE;

};

bool __stdcall FromString(DXGISwapChainFlagRecord *pDXGIPresentFlagRecord, const tstring &DXGIPresentFlagRecordString)
{
    return true;
};

tstring __stdcall ToString(const DXGISwapChainFlagRecord &DXGISwapChainFlagRecord)
{
    return tstring(DXGISwapChainFlagRecord.szName);
};
