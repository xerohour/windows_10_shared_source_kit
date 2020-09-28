/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	ZCmp.h
 *
 ***************************************************************************/

#ifndef __ZCMP_H__
#define __ZCMP_H__

#include "CD3DTest.h"

// Defines
#ifdef FAST_TEST
const int MAX_TESTS = 10;
#else
const int MAX_TESTS = 16;
#endif

// Base Class definitions
class CZCmpTest: public CD3DTest
{
    // Data
    private:
    D3DTLVERTEX     VertexList[24];
    DWORD           dwVertexCount;
	bool			bCapAbort;
	TCHAR			msgString[80];

    // Data
	public:
	DWORD			dwZCmpCap;

	public:
	CZCmpTest();
	~CZCmpTest();

	// Framework functions
	virtual bool CapsCheck(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);

	// Helper functions
	void ComboTests(float Zval);
	void AlwaysTests(int nTest);
	void LessTests(int nTest);
	bool EqualTests(int nTest);
	void GreaterTests(int nTest);
};

// Never Class definitions
class CZNeverTest: public CZCmpTest
{
	public:
	CZNeverTest();

	bool SetDefaultRenderStates(void);
};

// Less Class definitions
class CZLessTest: public CZCmpTest
{
	public:
	CZLessTest();

	bool SetDefaultRenderStates(void);
};

// Equal Class definitions
class CZEqualTest: public CZCmpTest
{
	public:
	CZEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
};

// LessEqual Class definitions
class CZLessEqualTest: public CZCmpTest
{
	public:
	CZLessEqualTest();

	bool SetDefaultRenderStates(void);
};

// Greater Class definitions
class CZGreaterTest: public CZCmpTest
{
	public:
	CZGreaterTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
};

// NotEqual Class definitions
class CZNotEqualTest: public CZCmpTest
{
	public:
	CZNotEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
};

// GreaterEqual Class definitions
class CZGreaterEqualTest: public CZCmpTest
{
	public:
	CZGreaterEqualTest();

	bool SetDefaultRenderStates(void);
    bool ClearFrame(void);
};

// Always Class definitions
class CZAlwaysTest: public CZCmpTest
{
	public:
	CZAlwaysTest();

	bool SetDefaultRenderStates(void);
};

#endif
