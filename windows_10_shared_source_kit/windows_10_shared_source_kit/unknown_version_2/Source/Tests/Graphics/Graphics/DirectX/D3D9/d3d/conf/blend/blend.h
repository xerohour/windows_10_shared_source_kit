/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Blend.h
 *
 ***************************************************************************/

#ifndef __BLEND_H__
#define __BLEND_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines
#define MAX_TESTS		20

// Class definition
class CBlendTest: public CD3DTest
{
    // Data
	private:
	CnTexture *		pTexture;
	D3DCOLOR		Stripes[8];
	D3DCOLOR		AlphaStripes[8];
	DWORD			dwSrcAlphaCap;
	DWORD			dwInvSrcAlphaCap;
	DWORD			dwAlphaGouraudCap;
	TCHAR			szStatus[80];

    // Data
	public:
    D3DTLVERTEX     VertexList[6];
    DWORD           dwVertexCount;
	DWORD			dwBlendCap;
    DWORD           dwTextureOpCap;
    DWORD           dwTextureOpCap2;
	LPCTSTR			szTextureOp;
	LPCTSTR			szTextureOp2;
	DWORD			dwBlendState;
    TCHAR			msgString[80];

    public:
	CBlendTest();
	~CBlendTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	virtual HRESULT DrawBasicGrid(int nTest, int nTexFormats);
	virtual void DrawBlendingGrid(int nAlpha);
};

// Default Class definitions
class CDefaultTest: public CBlendTest
{
	public:
	CDefaultTest();
	~CDefaultTest();

	bool SetDefaultRenderStates(void);
};

// Decal Class definitions
class CDecalTest: public CBlendTest
{
	public:
	CDecalTest();
	~CDecalTest();

	bool SetDefaultRenderStates(void);
};

// Modulate Class definitions
class CModulateTest: public CBlendTest
{
	public:
	CModulateTest();
	~CModulateTest();

	bool SetDefaultRenderStates(void);
};

// Decal Alpha Class definitions
class CDecalAlphaTest: public CBlendTest
{
	public:
	CDecalAlphaTest();
	~CDecalAlphaTest();

	bool SetDefaultRenderStates(void);
};

// Modulate Alpha Class definitions
class CModulateAlphaTest: public CBlendTest
{
	public:
	CModulateAlphaTest();
	~CModulateAlphaTest();

	bool SetDefaultRenderStates(void);
};

// Copy Class definitions
class CCopyTest: public CBlendTest
{
	public:
	CCopyTest();
	~CCopyTest();

	bool SetDefaultRenderStates(void);
};

// Add Class definitions
class CAddTest: public CBlendTest
{
	public:
	CAddTest();
	~CAddTest();

	bool SetDefaultRenderStates(void);
	HRESULT DrawBasicGrid(int nTest, int nTexFormats);
	void DrawBlendingGrid(int nAlpha);
};

#endif
