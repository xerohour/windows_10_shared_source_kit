//*************************************************************************************************
// File: LinearFilter.cpp
// 
// Copyright (C) 1994-2004 Microsoft Corporation.  All Rights Reserved.
//
// Author:      Cliff Owen (cliffo)
//*************************************************************************************************
#include "filter.h"

//*************************************************************************************************
// Linear Filtering, Minification
//*************************************************************************************************

class CLinearMin: public CFilterTest
{
    public:
    
    CLinearMin()
    {
	    m_szTestName = "Texture Filter Linear Minify";
	    m_szCommandKey = "LinearMin";
	    m_Options.D3DOptions.fMinDXVersion = 8.0f;
	    
        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter

            { 32,       32,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 32,       32,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 32,       32,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 48,       48,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         64,             TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 56,       56,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 96,       96,         128,            TEXF_LINEAR,    TEXF_NONE},
            { 96,       96,         256,            TEXF_LINEAR,    TEXF_NONE},
            { 104,      104,        128,            TEXF_LINEAR,    TEXF_NONE},
            { 104,      104,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 120,      120,        128,            TEXF_LINEAR,    TEXF_NONE},
            { 120,      120,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 192,      192,        256,            TEXF_LINEAR,    TEXF_NONE},
            { 216,      216,        256,            TEXF_LINEAR,    TEXF_NONE},
        };	

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

CLinearMin   LinearMin;

//*************************************************************************************************
// Linear Filtering, Magnification
//*************************************************************************************************

class CLinearMag: public CFilterTest
{
    public:
    
    CLinearMag()
    {
	    m_szTestName = "Texture Filter Linear Magnify";
	    m_szCommandKey = "LinearMag";
	    m_Options.D3DOptions.fMinDXVersion = 8.0f;
	    
        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter

            { 48,       48,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 56,       56,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 64,       64,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 76,       76,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 76,       76,         64,             TEXF_NONE,      TEXF_LINEAR},
            { 96,       96,         32,             TEXF_NONE,      TEXF_LINEAR},
            { 96,       96,         64,             TEXF_NONE,      TEXF_LINEAR},
            { 102,      102,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 102,      102,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 120,      120,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 120,      120,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 156,      156,        128,            TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 192,      192,        128,            TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        32,             TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        64,             TEXF_NONE,      TEXF_LINEAR},
            { 216,      216,        128,            TEXF_NONE,      TEXF_LINEAR},
        };	

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

CLinearMag   LinearMag;
