/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Perspective.h
 *
 ***************************************************************************/

#ifndef __PERSPECTIVE_H__
#define __PERSPECTIVE_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#define MAX_GRID_TESTS	32
#define MAX_TRI_TESTS	11

// Class definition
class CPerspectiveTest: public CD3DTest
{
    // Data
    private:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
#if FAST_TEST
    TCHAR			szStatus[800];
    TCHAR			msgString[800];
#else
	TCHAR			szStatus[80];
	TCHAR			msgString[80];
#endif

    // Data
    public:
	CnTexture *		pTexture;

	public:
	CPerspectiveTest();
	~CPerspectiveTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawTexGrid(int nTest);
	void DrawColorGrid(int nTest);
	void DrawTriangle(int nTest);

#if FAST_TEST
    UINT m_totalNumTestCases;
#endif
};

// Texture Class definitions
class CTextureTest: public CPerspectiveTest
{
	public:
	CTextureTest();
	~CTextureTest();

    // Framework functions
	UINT TestInitialize(void);

};

// Color Class definitions
class CColorTest: public CPerspectiveTest
{
	public:
	CColorTest();
	~CColorTest();

	// Framework functions
	UINT TestInitialize(void);
};

#endif
