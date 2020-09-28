/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	ZBuffer.h
 *
 ***************************************************************************/

#ifndef __ZBUFFER_H__
#define __ZBUFFER_H__

#include "CD3DTest.h"

// Defines
const int MAX_TESTS = 1;
const int MAX_DISCARDABLE_TESTS = 8;

// Base Class definitions
class CZBufferTest: public CD3DTest
{
    // Data
	public:
	bool			bEnable;
    D3DTLVERTEX     VertexList[3];
    DWORD           dwVertexCount;
	TCHAR			msgString[80];

	public:
	CZBufferTest();
	~CZBufferTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual void SceneRefresh(void);
};

// Verify Class definitions
class CZVerifyTest: public CZBufferTest
{
	// Private Data
    private:
	bool	 bValidFormat;
	TCHAR	 errString[1024];

	public:
	CZVerifyTest();

	// Framework functions
	bool ExecuteTest(UINT);
	void SceneRefresh(void);
	bool ProcessFrame(void);

	// Helper functions
	bool VerifyFormat(void);
};

// Print Class definitions
class CZPrintTest: public CZBufferTest
{
	public:
	CZPrintTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
};

// Enable Class definitions
class CZEnableTest: public CZBufferTest
{
	public:
	CZEnableTest();

	// Framework functions
	bool ExecuteTest(UINT);

	// Helper functions
	void DrawTriangle(void);
};

// Never Class definitions
class CZNeverTest: public CZBufferTest
{
	// Framework functions
	bool ExecuteTest(UINT);

	// Helper functions
	void DrawTriangle(void);
};

// NeverOn Class definitions
class CZNeverOnTest: public CZNeverTest
{
	public:
	CZNeverOnTest();
};

// NeverOff Class definitions
class CZNeverOffTest: public CZNeverTest
{
	public:
	CZNeverOffTest();
};

// Discard Class definitions
class CZDiscardableTest: public CZBufferTest
{
    // Data
    private:
    CnSurface *pOriginalZ, *pNewZ; 
    CnSurface *pOriginalRT, *pNewRT; 

	public:
	bool bDiscard;

	// Framework functions
	bool ClearFrame(void);
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
	void SceneRefresh(void);
	bool ProcessFrame(void);
	bool TestTerminate(void);

	// Helper functions
	void DrawTriangle(int);
};

// No Discard Class definitions
class CZNoDiscardTest: public CZDiscardableTest
{
	public:
	CZNoDiscardTest();
};

// Discard Class definitions
class CZDiscardTest: public CZDiscardableTest
{
	public:
	CZDiscardTest();
};

#endif
