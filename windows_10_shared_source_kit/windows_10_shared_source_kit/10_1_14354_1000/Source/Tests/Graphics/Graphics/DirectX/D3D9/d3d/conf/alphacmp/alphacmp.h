/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	AlphaCmp.h
 *
 ***************************************************************************/

#ifndef __ALPHACMP_H__
#define __ALPHACMP_H__

#include "CD3DTest.h"

// Defines
#define MAX_TESTS	5

// Base Class definitions
class CAlphaCmpTest: public CD3DTest
{
    // Data
    private:
    D3DTLVERTEX     VertexList[3];
    DWORD           dwVertexCount;
	int				nAlphaValue[5];
	bool			bGouraud;
	TCHAR			msgString[80];

    public:
    DWORD			dwAlphaCmpCap;

	public:
	CAlphaCmpTest();
	~CAlphaCmpTest();

	// Framework functions
	virtual bool CapsCheck(void);
	virtual UINT TestInitialize(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);

	// Helper functions
	void DrawTriangle(int nAlpha);
};


// Never Class definitions
class CAlphaNeverTest: public CAlphaCmpTest
{
	public:
	CAlphaNeverTest();

	bool SetDefaultRenderStates(void);
};

// Less Class definitions
class CAlphaLessTest: public CAlphaCmpTest
{
	public:
	CAlphaLessTest();

	bool SetDefaultRenderStates(void);
};

// Equal Class definitions
class CAlphaEqualTest: public CAlphaCmpTest
{
	public:
	CAlphaEqualTest();

	bool SetDefaultRenderStates(void);
};

// LessEqual Class definitions
class CAlphaLessEqualTest: public CAlphaCmpTest
{
	public:
	CAlphaLessEqualTest();

	bool SetDefaultRenderStates(void);
};

// Greater Class definitions
class CAlphaGreaterTest: public CAlphaCmpTest
{
	public:
	CAlphaGreaterTest();

	bool SetDefaultRenderStates(void);
};

// NotEqual Class definitions
class CAlphaNotEqualTest: public CAlphaCmpTest
{
	public:
	CAlphaNotEqualTest();

	bool SetDefaultRenderStates(void);
};

// GreaterEqual Class definitions
class CAlphaGreaterEqualTest: public CAlphaCmpTest
{
	public:
	CAlphaGreaterEqualTest();

	bool SetDefaultRenderStates(void);
};

// Always Class definitions
class CAlphaAlwaysTest: public CAlphaCmpTest
{
	public:
	CAlphaAlwaysTest();

	bool SetDefaultRenderStates(void);
};

#endif
