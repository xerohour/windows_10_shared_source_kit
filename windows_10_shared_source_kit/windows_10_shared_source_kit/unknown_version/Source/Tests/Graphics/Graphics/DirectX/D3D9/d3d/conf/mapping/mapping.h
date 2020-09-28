/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Mapping.h
 *
 ***************************************************************************/

#ifndef __MAPPING_H__
#define __MAPPING_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#ifdef FAST_TEST
#define MAX_TESTS	25
#else
#define MAX_TESTS	316
#endif

// UV structure
typedef struct _UV{
	float u;
	float v;

public:
	_UV() {u=0.0f; v=0.0f;};
	_UV(float u1, float v1) {u=u1; v=v1;};

} UV;

// Base Class definitions
class CMappingTest: public CD3DTest
{
    // Data
	private:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	CnTexture *		pTexture;
	UV				TopLeft[4];
	UV				TopRight[4];
	UV				BottomLeft[4];
	UV				BottomRight[4];
	TCHAR			szStatus[80];
	TCHAR			msgString[80];

    // Data
	public:
	DWORD			dwMinFilterCap;
	DWORD			dwMagFilterCap;

	public:
	CMappingTest();
	~CMappingTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int nTest, int nTexFormats);
};

// Point Class definitions
class CMapPointTest: public CMappingTest
{
	public:
	CMapPointTest();

	bool SetDefaultRenderStates(void);
};

// Linear Class definitions
class CMapLinearTest: public CMappingTest
{
	public:
	CMapLinearTest();

	bool SetDefaultRenderStates(void);
};

#endif
