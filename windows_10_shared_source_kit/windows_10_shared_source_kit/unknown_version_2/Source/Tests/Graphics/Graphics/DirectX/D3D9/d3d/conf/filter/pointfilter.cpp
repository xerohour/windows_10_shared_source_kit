//*************************************************************************************************
// File: PointFilter.cpp
// 
// Copyright (C) 1994-2004 Microsoft Corporation.  All Rights Reserved.
//
// Author:      Cliff Owen (cliffo)
//*************************************************************************************************
#include "filter.h"
#include "d3d9.h"

//*************************************************************************************************
// Point Filtering, Minification
//*************************************************************************************************

class CPointMin: public CFilterTest
{
    public:
    
    CPointMin()
    {
	    m_szTestName = "Texture Filter Point Minify";
	    m_szCommandKey = "PointMin";
	    m_Options.D3DOptions.fMinDXVersion = 8.0f;
	    
        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter

            { 28,       28,         32,             TEXF_POINT,     TEXF_NONE},
            { 28,       28,         64,             TEXF_POINT,     TEXF_NONE},
            { 28,       28,         128,            TEXF_POINT,     TEXF_NONE},
            { 36,       36,         64,             TEXF_POINT,     TEXF_NONE},
            { 44,       44,         64,             TEXF_POINT,     TEXF_NONE},
            { 44,       44,         128,            TEXF_POINT,     TEXF_NONE},
            { 44,       44,         256,            TEXF_POINT,     TEXF_NONE},
            { 52,       52,         64,             TEXF_POINT,     TEXF_NONE},
            { 52,       52,         128,            TEXF_POINT,     TEXF_NONE},
            { 52,       52,         256,            TEXF_POINT,     TEXF_NONE},
            { 72,       72,         128,            TEXF_POINT,     TEXF_NONE},
            { 76,       76,         128,            TEXF_POINT,     TEXF_NONE},
            { 84,       84,         128,            TEXF_POINT,     TEXF_NONE},
            { 92,       92,         128,            TEXF_POINT,     TEXF_NONE},
            { 92,       92,         256,            TEXF_POINT,     TEXF_NONE},
            { 98,       98,         128,            TEXF_POINT,     TEXF_NONE},
            { 98,       98,         256,            TEXF_POINT,     TEXF_NONE},
            { 104,      104,        128,            TEXF_POINT,     TEXF_NONE},
            { 104,      104,        256,            TEXF_POINT,     TEXF_NONE},
            { 120,      120,        128,            TEXF_POINT,     TEXF_NONE},
            { 120,      120,        256,            TEXF_POINT,     TEXF_NONE},
            { 156,      156,        256,            TEXF_POINT,     TEXF_NONE},
            { 188,      188,        256,            TEXF_POINT,     TEXF_NONE},
            { 216,      216,        256,            TEXF_POINT,     TEXF_NONE},
        };	

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

CPointMin   PointMin;

//*************************************************************************************************
// Point Filtering, Magnification
//*************************************************************************************************

class CPointMag: public CFilterTest
{
    public:
    
    CPointMag()
    {
	    m_szTestName = "Texture Filter Point Magnify";
	    m_szCommandKey = "PointMag";
	    m_Options.D3DOptions.fMinDXVersion = 8.0f;
	    
        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter

            { 52,       52,         32,             TEXF_NONE,     TEXF_POINT},
            { 68,       68,         32,             TEXF_NONE,     TEXF_POINT},
            { 76,       76,         32,             TEXF_NONE,     TEXF_POINT},
            { 76,       76,         64,             TEXF_NONE,     TEXF_POINT},
            { 88,       88,         32,             TEXF_NONE,     TEXF_POINT},
            { 88,       88,         64,             TEXF_NONE,     TEXF_POINT},
            { 102,      102,        32,             TEXF_NONE,     TEXF_POINT},
            { 102,      102,        64,             TEXF_NONE,     TEXF_POINT},
            { 120,      120,        32,             TEXF_NONE,     TEXF_POINT},
            { 120,      120,        64,             TEXF_NONE,     TEXF_POINT},
            { 156,      156,        32,             TEXF_NONE,     TEXF_POINT},
            { 156,      156,        64,             TEXF_NONE,     TEXF_POINT},
            { 156,      156,        128,            TEXF_NONE,     TEXF_POINT},
            { 188,      188,        32,             TEXF_NONE,     TEXF_POINT},
            { 188,      188,        64,             TEXF_NONE,     TEXF_POINT},
            { 188,      188,        128,            TEXF_NONE,     TEXF_POINT},
            { 216,      216,        32,             TEXF_NONE,     TEXF_POINT},
            { 216,      216,        64,             TEXF_NONE,     TEXF_POINT},
            { 216,      216,        128,            TEXF_NONE,     TEXF_POINT},
        };	

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

CPointMag   PointMag;
