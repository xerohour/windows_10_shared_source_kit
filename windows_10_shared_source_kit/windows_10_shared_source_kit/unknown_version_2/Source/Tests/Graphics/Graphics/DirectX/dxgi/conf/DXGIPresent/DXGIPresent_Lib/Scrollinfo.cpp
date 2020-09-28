//****************************************************************************
// ScrollInfo.cpp
// 
// string conversion functions for ScrollInfo test parameter.  ScrollInfo is
// used to house scrolling data for DXGIPresent test.
// 
// History:
// 	7/26/2010 - Olanh - Initial Creation
//
// Copyright Microsoft Corporation
//****************************************************************************

#include <scrollinfo.h>
#include <float.h>
#include <stdio.h>

// left  top  right bottom    offX  offY
SCROLLINGINFO g_ScrollingInfo[] = {{false, 0.0f,  0.0f,  0.0f,  0.0f,     0.0f,  0.0f }, // No scrolling
                                   {true,  0.0f,  0.0f, 0.75f, 0.75f,   -0.25f,-0.25f }, // Scroll left-up
                                   {true,  0.25f,  0.0f, 1.0f,  0.75f,    0.25f,-0.25f }, // Scroll right-up
                                   {true,  0.25f, 0.25f, 1.0f,   1.0f,    0.25f, 0.25f }, // Scroll right-down
                                   {true,  0.0f, 0.25f, 0.75f,  1.0f,   -0.25f, 0.25f }}; // Scroll left-down

const unsigned int g_uNumOfScrollInfoElements = sizeof(g_ScrollingInfo)/sizeof(g_ScrollingInfo[0]);

//-----------------------------------------------------------------------------
// Function: ToString
//-----------------------------------------------------------------------------
tstring __stdcall ToString(const SCROLLINGINFO &si)
{
    char szDesc[128];

    _snprintf(szDesc, 128, "Scroll Rect: [%.2f, %.2f, %.2f, %.2f], Offset: [%.2f, %.2f]", 
        si.fLeft, si.fTop, si.fRight, si.fBottom,
        si.fOffX, si.fOffY);
    // Fix Missing String Termination (OACR-T)
    szDesc[sizeof(szDesc)/sizeof(szDesc[0]) - 1] = 0;
    
    return tstring( szDesc );
};

//-----------------------------------------------------------------------------
// FromString 
// Converts string representation of a SCROLLINGINFO to a SCROLLINGINFO struct. 
// Not sure how practical the use of this really is.   Assuming the format is
//  "[x1, y1, x2, y2], [sx, sy]" 
//-----------------------------------------------------------------------------
bool __stdcall FromString(SCROLLINGINFO *psi, const tstring &tstrDesc)
{
    // Not currently supported.
    return false;
};

//-----------------------------------------------------------------------------
// Function: Comparison Operator
//-----------------------------------------------------------------------------
bool operator ==(const SCROLLINGINFO &x, const SCROLLINGINFO &y)
{
    return (bool)!memcmp(&x,&y, sizeof(SCROLLINGINFO));
}
