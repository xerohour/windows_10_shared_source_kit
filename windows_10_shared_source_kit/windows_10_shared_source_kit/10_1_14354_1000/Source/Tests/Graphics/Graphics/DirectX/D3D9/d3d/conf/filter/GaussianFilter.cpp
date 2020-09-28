//*************************************************************************************************
// File: GaussianFilter.cpp
// 
// Copyright (C) 1994-2004 Microsoft Corporation.  All Rights Reserved.
//
// Author:      Cliff Owen (cliffo)
//*************************************************************************************************
#include "filter.h"

//*************************************************************************************************
// Gaussian Filtering, Minification
//*************************************************************************************************

class CGaussianMin: public CFilterTest
{
    public:
    
    CGaussianMin()
    {
	    m_szTestName = "Texture Filter Gaussian Minify";
	    m_szCommandKey = "GaussianMin";
	    m_Options.D3DOptions.fMinDXVersion = 9.0f;

        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter           MagFilter

            { 32,       32,         64,             TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 32,       32,         128,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 32,       32,         256,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 48,       48,         64,             TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 48,       48,         128,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 48,       48,         256,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 56,       56,         64,             TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 56,       56,         128,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 56,       56,         256,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 96,       96,         128,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 96,       96,         256,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 104,      104,        128,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 104,      104,        256,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 120,      120,        128,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 120,      120,        256,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 192,      192,        256,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
            { 216,      216,        256,            TEXF_GAUSSIANQUAD,  TEXF_NONE},
        };	

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

CGaussianMin   GaussianMin;

//*************************************************************************************************
// Gaussian Filtering, Magnification
//*************************************************************************************************

class CGaussianMag: public CFilterTest
{
    public:
    
    CGaussianMag()
    {
	    m_szTestName = "Texture Filter Gaussian Magnify";
	    m_szCommandKey = "GaussianMag";
	    m_Options.D3DOptions.fMinDXVersion = 9.0f;
	    
        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter

            { 48,       48,         32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 56,       56,         32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 64,       64,         32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 76,       76,         32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 76,       76,         64,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 96,       96,         32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 96,       96,         64,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 102,      102,        32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 102,      102,        64,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 120,      120,        32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 120,      120,        64,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 156,      156,        32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 156,      156,        64,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 156,      156,        128,            TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 192,      192,        32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 192,      192,        64,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 192,      192,        128,            TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 216,      216,        32,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 216,      216,        64,             TEXF_NONE,      TEXF_GAUSSIANQUAD},
            { 216,      216,        128,            TEXF_NONE,      TEXF_GAUSSIANQUAD},
        };	

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

CGaussianMag   GaussianMag;

//*************************************************************************************************
// Gaussian Filtering, Mixed Min/Mag with Point
//
// NOTE: These cases are purely artificial and would only truly occur if the poly should be doing
// anisotropic filtering. As such, minification is the relevant state.
//*************************************************************************************************

class CGaussianMixedPoint: public CFilterTest
{
    public:
    
    CGaussianMixedPoint()
    {
	    m_szTestName = "Texture Filter Gaussian Mixed with Point sampling";
	    m_szCommandKey = "GuassianMixPoint";
	    m_Options.D3DOptions.fMinDXVersion = 9.0f;
	    
        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter

            // First Magnify Gaussian, Minify Point

            { 30,       48,         32,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 48,       30,         32,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            
            { 30,       56,         32,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 56,       30,         32,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            
            { 30,       64,         32,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 64,       30,         32,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            
            { 48,       76,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 76,       48,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            
            { 48,       96,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 96,       48,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            
            { 48,       102,        64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 102,      48,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 48,       120,        64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 120,      48,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},

            { 48,       156,        64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 156,      48,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 64,       156,        128,            TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 156,      64,         128,            TEXF_POINT,     TEXF_GAUSSIANQUAD},
            
            { 48,       192,        64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 192,      48,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 64,       192,        128,            TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 192,      64,         128,            TEXF_POINT,     TEXF_GAUSSIANQUAD},
            
            { 48,       216,        64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 216,      48,         64,             TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 64,       216,        128,            TEXF_POINT,     TEXF_GAUSSIANQUAD},
            { 216,      64,         128,            TEXF_POINT,     TEXF_GAUSSIANQUAD},
            
            // Second, Magnify Point, Minify Gaussian

            { 30,       48,         32,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 48,       30,         32,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            
            { 30,       56,         32,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 56,       30,         32,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            
            { 30,       64,         32,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 64,       30,         32,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            
            { 48,       76,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 76,       48,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            
            { 48,       96,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 96,       48,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            
            { 48,       102,        64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 102,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 48,       120,        64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 120,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},

            { 48,       156,        64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 156,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 64,       156,        128,            TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 156,      64,         128,            TEXF_GAUSSIANQUAD,    TEXF_POINT},
            
            { 48,       192,        64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 192,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 64,       192,        128,            TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 192,      64,         128,            TEXF_GAUSSIANQUAD,    TEXF_POINT},
            
            { 48,       216,        64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 216,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 64,       216,        128,            TEXF_GAUSSIANQUAD,    TEXF_POINT},
            { 216,      64,         128,            TEXF_GAUSSIANQUAD,    TEXF_POINT},
        };

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

//CGaussianMixedPoint   GaussianMixedPoint;

//*************************************************************************************************
// Gaussian Filtering, Mixed Min/Mag with Linear
//
// NOTE: These cases are purely artificial and would only truly occur if the poly should be doing
// anisotropic filtering. As such, minification is the relevant state.
//*************************************************************************************************

class CGaussianMixedLinear: public CFilterTest
{
    public:
    
    CGaussianMixedLinear()
    {
	    m_szTestName = "Texture Filter Gaussian Mixed with Linear sampling";
	    m_szCommandKey = "GuassianMixLinear";
	    m_Options.D3DOptions.fMinDXVersion = 9.0f;
	    
        FILTERLIST  FilterList[] = {
            // Width    Height      Texture Size    MinFilter       MagFilter

            // First Magnify Gaussian, Minify Linear

            { 30,       48,         32,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 48,       30,         32,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            
            { 30,       56,         32,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 56,       30,         32,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            
            { 30,       64,         32,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 64,       30,         32,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            
            { 48,       76,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 76,       48,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            
            { 48,       96,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 96,       48,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            
            { 48,       102,        64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 102,      48,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 48,       120,        64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 120,      48,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},

            { 48,       156,        64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 156,      48,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 64,       156,        128,            TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 156,      64,         128,            TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            
            { 48,       192,        64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 192,      48,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 64,       192,        128,            TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 192,      64,         128,            TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            
            { 48,       216,        64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 216,      48,         64,             TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 64,       216,        128,            TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            { 216,      64,         128,            TEXF_LINEAR,     TEXF_GAUSSIANQUAD},
            
            // Second, Magnify Linear, Minify Gaussian

            { 30,       48,         32,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 48,       30,         32,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            
            { 30,       56,         32,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 56,       30,         32,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            
            { 30,       64,         32,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 64,       30,         32,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            
            { 48,       76,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 76,       48,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            
            { 48,       96,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 96,       48,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            
            { 48,       102,        64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 102,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 48,       120,        64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 120,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},

            { 48,       156,        64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 156,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 64,       156,        128,            TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 156,      64,         128,            TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            
            { 48,       192,        64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 192,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 64,       192,        128,            TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 192,      64,         128,            TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            
            { 48,       216,        64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 216,      48,         64,             TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 64,       216,        128,            TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
            { 216,      64,         128,            TEXF_GAUSSIANQUAD,    TEXF_LINEAR},
        };

        // Set this data as the permanent data associated with the test.

        SetFilterList(FilterList,sizeof(FilterList)/sizeof(FILTERLIST));	
	}
};

//CGaussianMixedLinear   GaussianMixedLinear;

