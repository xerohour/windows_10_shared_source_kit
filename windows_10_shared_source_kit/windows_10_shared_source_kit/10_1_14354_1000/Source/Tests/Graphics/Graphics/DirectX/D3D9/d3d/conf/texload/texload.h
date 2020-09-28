/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	TexLoad.h
 *
 ***************************************************************************/

#ifndef __TEXLOAD_H__
#define __TEXLOAD_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#define MAX_TESTS		5
#define MAX_MIP_TESTS	18
#define MAX_CUBE_TESTS	18

// Class definition
class CTexLoadTest: public CD3DTest
{
    // Private Data
	private:
    D3DTLVERTEX     VertexList[12];
    DWORD           dwVertexCount;
    CSurface *		pSrcTarget;
	CTexture *		pSrcTexture;
	CTexture *		pSrc2Texture;
	CTexture *		pDestTexture;
	CImage *	    pMipImage[9];
	D3DCOLOR		Red[1];
	D3DCOLOR		Green[1];
	D3DCOLOR		Blue[1];
	float			SizeArray[18];
	int				nSize;
	DWORD			dwDevCaps;
	TCHAR			msgString[80];
	TCHAR			szStatus[80];

    // Public Data
	public:
	bool			bMipMap;
	bool			bAGP, bManage, bSystem, bVideo;

	public:
	CTexLoadTest();
	~CTexLoadTest();

	// Framework functions
	virtual bool SetDefaultRenderStates(void);
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual bool ProcessFrame(void);
	virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(float fMesh, bool bSingle);
};

// System Class definitions
class CSystemTest: public CTexLoadTest
{
	public:
	CSystemTest();
	~CSystemTest();
};

// Video Class definitions
class CVideoTest: public CTexLoadTest
{
	public:
	CVideoTest();
	~CVideoTest();
};

// AGP Class definitions
class CAGPTest: public CTexLoadTest
{
	public:
	CAGPTest();
	~CAGPTest();
};

// Manage Class definitions
class CManageTest: public CTexLoadTest
{
	public:
	CManageTest();
	~CManageTest();
};

// MipMap System Class definitions
class CMipMapSystemTest: public CTexLoadTest
{
	public:
	CMipMapSystemTest();
	~CMipMapSystemTest();
};

// MipMap Video Class definitions
class CMipMapVideoTest: public CTexLoadTest
{
	public:
	CMipMapVideoTest();
	~CMipMapVideoTest();
};

// MipMap AGP Class definitions
class CMipMapAGPTest: public CTexLoadTest
{
	public:
	CMipMapAGPTest();
	~CMipMapAGPTest();
};

// MipMap Manage Class definitions
class CMipMapManageTest: public CTexLoadTest
{
	public:
	CMipMapManageTest();
	~CMipMapManageTest();
};

#endif
