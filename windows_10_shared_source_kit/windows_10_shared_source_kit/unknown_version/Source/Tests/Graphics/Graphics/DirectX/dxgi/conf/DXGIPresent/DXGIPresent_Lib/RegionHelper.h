//-----------------------------------------------------------------------------
//
//  RegionHelper.h
//
//-----------------------------------------------------------------------------
#pragma once

#include <windows.h>
#include <string>
typedef std::basic_string<TCHAR> tstring;

class RegionHelper
{
public:
    RegionHelper();
    virtual ~RegionHelper();
    const RegionHelper& operator=(
        const RegionHelper& rhs);
    
    RegionHelper(
        const RegionHelper& element);

    VOID AddRects(
        RECT *pRects,
        UINT numberOfRects);

public:
    tstring Description;
    char *pDirtyRegion;


private:
    VOID Copy(const RegionHelper* pCopy);



private:
    UINT m_NumberOfRects;
};
