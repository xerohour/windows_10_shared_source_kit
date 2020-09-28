//****************************************************************************
// ScrollInfo.h
// 
// typedef and string conversion functions for ScrollInfo test parameter.  ScrollInfo is
// used to house scrolling data for DXGIPresent test.
// 
// History:
// 	7/26/2010 - Olanh - Initial Creation
//
// Copyright Microsoft Corporation
//****************************************************************************

#include <windows.h>
#include <WGFTestCore.h>

//-----------------------------------------------------------------------------
// SCROLLINFO - Test Parameter data
//-----------------------------------------------------------------------------
typedef struct tagScrollingInfo
{
    bool bPerformScroll;
    float fLeft, fTop, fRight, fBottom;   // RECT defining destination for scroll.  Expressed as a fraction of the swap chain dimensions.
    float fOffX, fOffY;   // offset from source position for dest rect above. Expressed as a fraction of the swap chain dimensions.
//    RECT ScrollRect;
//    POINT ScrollOffset;
} SCROLLINGINFO;


extern SCROLLINGINFO g_ScrollingInfo[];
extern const unsigned int g_uNumOfScrollInfoElements; 

tstring __stdcall ToString(const SCROLLINGINFO &si);
bool __stdcall FromString(SCROLLINGINFO *psi, const tstring &tstrDesc);
bool operator ==(const SCROLLINGINFO &x, const SCROLLINGINFO &y);

bool IsNonZeroScrollInfo(SCROLLINGINFO *psi);