#include "DXGIPresentFlagRecord.h"


DXGIPresentFlagRecord DXGIPresentFlags[] =
{
	{0, "0"},
    {DXGI_PRESENT_TEST, "DXGI_PRESENT_TEST"},
	{DXGI_PRESENT_DO_NOT_SEQUENCE, "DXGI_PRESENT_DO_NOT_SEQUENCE"}
   // {DXGI_PRESENT_RESTART, "DXGI_PRESENT_RESTART"}
};

const UINT cuiNoOfPresentFlags = sizeof(DXGIPresentFlags) / sizeof(DXGIPresentFlags[0]);


bool operator == (const DXGIPresentFlagRecord& lhs, const DXGIPresentFlagRecord& rhs)
{
    bool match = (CSTR_EQUAL == CompareString(
                LOCALE_INVARIANT,
                NORM_IGNORECASE,
                lhs.szPresentFlagName, -1,
                rhs.szPresentFlagName, -1));

    if (lhs.dxgiPresentFlag == rhs.dxgiPresentFlag && match )
	{
		return TRUE;
	}
	
    return FALSE;

};

bool __stdcall FromString(DXGIPresentFlagRecord *pDXGIPresentFlagRecord, const tstring &DXGIPresentFlagRecordString)
{
    return true;
};

tstring __stdcall ToString(const DXGIPresentFlagRecord &dxgiPresentFlagRecord)
{
    return tstring(dxgiPresentFlagRecord.szPresentFlagName);
};
