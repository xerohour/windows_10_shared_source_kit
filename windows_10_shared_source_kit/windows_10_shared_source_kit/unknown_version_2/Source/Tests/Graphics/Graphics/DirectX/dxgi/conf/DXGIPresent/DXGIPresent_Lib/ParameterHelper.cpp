//-----------------------------------------------------------------------------
// 
//  ParameterHelper
//
//
//-----------------------------------------------------------------------------

#include "parameterhelper.h"
#include <strsafe.h>

//-----------------------------------------------------------------------------
// Function: ToString
//-----------------------------------------------------------------------------
tstring __stdcall ToString(RECT resource)
{
    TCHAR out[ MAX_PATH];
    StringCchPrintf(
        out,
        ARRAYSIZE( out),
        _T( "(%d,%d)(%d,%d)"),
        resource.left,
        resource.top,
        resource.right,
        resource.bottom);

    return tstring(out);
};

//-----------------------------------------------------------------------------
// Function: FromString
//-----------------------------------------------------------------------------
bool __stdcall FromString(RECT *pType, const tstring &str)
{
    HRESULT hr = S_OK;

    UINT sides = 0;
    LONG rectValues[4] = {0};
    TCHAR temp[MAX_PATH] = { 0 };
    str._Copy_s(temp, MAX_PATH, MAX_PATH);

    PTSTR token = _tcstok(temp, _T(", "));
    while (sides < 4 && token)
    {
        rectValues[sides] = _ttol(token);

        token = _tcstok(NULL, _T(", "));
        sides++;
    }

    if (sides != 4)
    {
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        pType->left = rectValues[0];
        pType->top = rectValues[1];
        pType->right = rectValues[2];
        pType->bottom = rectValues[3];
    }

    return SUCCEEDED(hr) ? true : false;
};

//-----------------------------------------------------------------------------
// Function: equality operator
//-----------------------------------------------------------------------------
bool __stdcall operator == (const RegionHelper &rhs, const RegionHelper &lhs)
{
    return false;
};

//-----------------------------------------------------------------------------
// Function: ToString
//-----------------------------------------------------------------------------
tstring __stdcall ToString(RegionHelper resource)
{
    return resource.Description;
};

//-----------------------------------------------------------------------------
// Function: FromString
//-----------------------------------------------------------------------------
bool __stdcall FromString(RegionHelper *pType, const tstring &str)
{
    return false;
}





