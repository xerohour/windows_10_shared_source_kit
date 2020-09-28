/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Fog.h
 *
 ***************************************************************************/

#ifndef __FOG_H__
#define __FOG_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Defines (Fog type)
#define VERTEX			1
#define TABLE_LINEAR	2
#define TABLE_EXP		3
#define TABLE_EXP2		4
#define W_LINEAR		5
#define W_EXP			6
#define W_EXP2			7

// Defines (Number of tests)
#define VERTEX_TESTS		12
#define TABLE_LINEAR_TESTS	22
#define TABLE_EXP_TESTS		11
#define TABLE_EXP2_TESTS	11
#define W_LINEAR_TESTS		40
#define W_EXP_TESTS			11
#define W_EXP2_TESTS		11

#define TEXTURE_VERTEX_TESTS    4   
#define TEXTURE_TABLE_TESTS     3
#define TEXTURE_W_TESTS         3

// Base Class definitions
class CFogTest: public CD3DTest
{
    // Data
	private:
    D3DTLVERTEX     VertexList[24];
    DWORD           dwVertexCount;
	CnTexture *		pTexture;
#if FAST_TEST
    TCHAR			msgString[800];
    TCHAR			szStatus[800];
#else
	TCHAR			msgString[80];
	TCHAR			szStatus[80];
#endif

    // Data
	public:
	DWORD			dwFog;
	bool			bCapAbort;
	D3DCOLOR		FogColor;
	D3DCOLOR		Stripes[8];

	public:
	CFogTest();
	~CFogTest();

	// Framework functions
	virtual	bool CapsCheck(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT uTestNum);
	virtual void SceneRefresh(void);
	virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void VertexTests(int nTest);
    void TextureVertexTests(int nTest);
	void TableTests(void);
	void WTests(void);

#if FAST_TEST
    UINT m_totalNumTestCases;
#endif
};

// Vertex Class definitions
class CVertexTest: public CFogTest
{
	public:
	CVertexTest();

	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// TableLinear Class definitions
class CTableLinearTest: public CFogTest
{
	public:
	CTableLinearTest();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// TableExp Class definitions
class CTableExpTest: public CFogTest
{
	public:
	CTableExpTest();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// TableExp2 Class definitions
class CTableExp2Test: public CFogTest
{
	public:
	CTableExp2Test();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// WLinear Class definitions
class CWLinearTest: public CFogTest
{
	public:
	CWLinearTest();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// WExp Class definitions
class CWExpTest: public CFogTest
{
	public:
	CWExpTest();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// WExp2 Class definitions
class CWExp2Test: public CFogTest
{
	public:
	CWExp2Test();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// Texture Vertex Class definitions
class CTextureVertexTest: public CFogTest
{
	public:
	CTextureVertexTest();

	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// Texture TableLinear Class definitions
class CTextureTableLinearTest: public CFogTest
{
	public:
	CTextureTableLinearTest();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// Texture TableExp Class definitions
class CTextureTableExpTest: public CFogTest
{
	public:
	CTextureTableExpTest();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// Texture TableExp2 Class definitions
class CTextureTableExp2Test: public CFogTest
{
	public:
	CTextureTableExp2Test();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// Texture WLinear Class definitions
class CTextureWLinearTest: public CFogTest
{
	public:
	CTextureWLinearTest();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// Texture WExp Class definitions
class CTextureWExpTest: public CFogTest
{
	public:
	CTextureWExpTest();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

// Texture WExp2 Class definitions
class CTextureWExp2Test: public CFogTest
{
	public:
	CTextureWExp2Test();

	bool SetDefaultMatrices(void);
	bool SetDefaultRenderStates(void);
	UINT TestInitialize(void);
};

#endif
