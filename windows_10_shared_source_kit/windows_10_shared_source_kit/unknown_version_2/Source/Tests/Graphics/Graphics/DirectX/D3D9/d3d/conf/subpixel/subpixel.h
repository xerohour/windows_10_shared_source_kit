/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	subpixel.h
 *
 ***************************************************************************/

#ifndef __SUBPIXEL_H__
#define __SUBPIXEL_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#ifdef FAST_TEST
#define MAX_TESTS	20
#else
#define MAX_TESTS	400
#endif

// Class definition
class CSubPixelTest: public CD3DTest
{
    // Data
    private:
    D3DTLVERTEX     VertexList[528];
    DWORD           dwVertexCount;
	CnTexture *		pTexture;
	TCHAR			msgString[80];
	TCHAR			szStatus[80];

	public:
	CSubPixelTest();
	~CSubPixelTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int nTest, int nWDiv, int nHDiv, int nTexFormats);
	float CalcU(float xVal, float yVal, float fPixels, int nMapping);
	float CalcV(float xVal, float yVal, float fPixels, int nMapping);
};

#endif


