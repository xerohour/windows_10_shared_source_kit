/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	WCmp.h
 *
 ***************************************************************************/

#ifndef __WCMP_H__
#define __WCMP_H__

#include "CD3DTest.h"

// Defines
#define MAX_TESTS	16

// Base Class definitions
class CWCmpTest: public CD3DTest
{
    // Data
    private:
	TCHAR			msgString[80];

    // Data
    public:
    D3DTLVERTEX     VertexList[24];
    DWORD           dwVertexCount;
	DWORD			dwWCmpCap;

	public:
	CWCmpTest();
	~CWCmpTest();

	// Framework functions
	virtual bool SetDefaultMatrices(void);
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
class CWNeverTest: public CWCmpTest
{
	public:
	CWNeverTest();

	bool SetDefaultRenderStates(void);
};

// Less Class definitions
class CWLessTest: public CWCmpTest
{
	public:
	CWLessTest();

	bool SetDefaultRenderStates(void);
};

// Equal Class definitions
class CWEqualTest: public CWCmpTest
{
    // Data
    D3DTLVERTEX     BackgroundList[4];

	public:
	CWEqualTest();

	bool SetDefaultRenderStates(void);
	void SceneRefresh(void);

	// Helper functions
	void SetupBackground(void);
};

// LessEqual Class definitions
class CWLessEqualTest: public CWCmpTest
{
	public:
	CWLessEqualTest();

	bool SetDefaultRenderStates(void);
};

// Greater Class definitions
class CWGreaterTest: public CWCmpTest
{
    // Data
    D3DTLVERTEX     BackgroundList[4];

	public:
	CWGreaterTest();

	bool SetDefaultRenderStates(void);
	void SceneRefresh(void);

	// Helper functions
	void SetupBackground(void);
};

// NotEqual Class definitions
class CWNotEqualTest: public CWCmpTest
{
    // Data
    D3DTLVERTEX     BackgroundList[4];

	public:
	CWNotEqualTest();

	bool SetDefaultRenderStates(void);
	void SceneRefresh(void);

	// Helper functions
	void SetupBackground(void);
};

// GreaterEqual Class definitions
class CWGreaterEqualTest: public CWCmpTest
{
    // Data
    D3DTLVERTEX     BackgroundList[4];

	public:
	CWGreaterEqualTest();

	bool SetDefaultRenderStates(void);
	void SceneRefresh(void);

	// Helper functions
	void SetupBackground(void);
};

// Always Class definitions
class CWAlwaysTest: public CWCmpTest
{
	public:
	CWAlwaysTest();

	bool SetDefaultRenderStates(void);
};

#endif
