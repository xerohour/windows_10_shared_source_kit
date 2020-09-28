//*************************************************************************************************
// File: Filter.h
// 
// Copyright (C) 1994-2004 Microsoft Corporation.  All Rights Reserved.
//
// Author:      Cliff Owen (cliffo)
//*************************************************************************************************

#ifndef __FILTER_H__
#define __FILTER_H__

#include "CD3DTest.h"
#include "CTextures.h"

typedef struct
{
    UINT                uMeshWidth;
    UINT                uMeshHeight;
    UINT                uTextureSize;
    DWORD               dwMinState;
    DWORD               dwMagState;
} FILTERLIST, *PFILTERLIST;

typedef struct
{
    float   x;
    float   y;
    float   z;
    float   w;
    DWORD   dwDiffuse;
    float   u;
    float   v;
} TLVERTEX, *PTLVERTEX;

// Base Class definitions

class CFilterTest: public CD3DTest
{
    protected:
    
	    CnTexture *		m_pTexture;
	    PFILTERLIST     m_pFilterList;
	    UINT            m_uFilterCount;
        TCHAR           m_szStatusString[80];
    
	public:
	
	    CFilterTest();
	    ~CFilterTest();

	    // Framework functions
	    virtual UINT TestInitialize(void);
	    virtual bool ExecuteTest(UINT);
	    virtual bool TestTerminate(void);
	    virtual void SetFilterList(PFILTERLIST, UINT);
	    virtual void UpdateStatus();
	    virtual void BuildStatusString(UINT uIndex);
	    virtual bool CapsCheck(void);
	    virtual void TestSpecificSetup(UINT) {}
	    virtual void TestSpecificCleanup() {}

#if FAST_TEST
        virtual bool AddFormatModes(DISPLAYMODE *pMode, bool bWindowed);
#endif
};

// NOTE: See the individual test files for descriptions of the test
// classes. LinearFilter.cpp, PointFilter.cpp, PyramidalFilter.cpp, and GaussianFilter.cpp

#endif
