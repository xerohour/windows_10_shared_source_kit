/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	AGP.h
 *
 ***************************************************************************/

#ifndef __AGP_H__
#define __AGP_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#define MAX_TESTS	4

// Base Class definitions
class CAGPTest: public CD3DTest
{
    // Data
	private:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	D3DCOLOR		Stripes[8];
	CnTexture *		pTexture;
	bool			bUseAGP;
	TCHAR			szStatus[80];
	TCHAR			msgString[80];

    // Data
	public:
	int				nTextureSize;

	public:
	CAGPTest();
	~CAGPTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
    virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int nTest);
};

// Texture 8x8 Class definitions
class CTexture8x8Test: public CAGPTest
{
	public:
	CTexture8x8Test();
	~CTexture8x8Test();
};

// Texture 16x16 Class definitions
class CTexture16x16Test: public CAGPTest
{
	public:
	CTexture16x16Test();
	~CTexture16x16Test();
};

// Texture 64x64 Class definitions
class CTexture64x64Test: public CAGPTest
{
	public:
	CTexture64x64Test();
	~CTexture64x64Test();
};

// Texture 256x256 Class definitions
class CTexture256x256Test: public CAGPTest
{
	public:
	CTexture256x256Test();
	~CTexture256x256Test();
};

// Texture 1024x1024 Class definitions
class CTexture1024x1024Test: public CAGPTest
{
	public:
	CTexture1024x1024Test();
	~CTexture1024x1024Test();
};

#endif
