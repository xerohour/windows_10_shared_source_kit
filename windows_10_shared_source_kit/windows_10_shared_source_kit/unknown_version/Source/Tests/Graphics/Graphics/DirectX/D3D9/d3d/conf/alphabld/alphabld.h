/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	AlphaBld.h
 *
 ***************************************************************************/

#ifndef __ALPHABLD_H__
#define __ALPHABLD_H__

#include "CD3DTest.h"

// Defines
#ifdef FAST_TEST
// Defines
#define COLOR_VARIATIONS 3
#define NUM_BLEND_OPS 5

static const DWORD g_cPermutableBlendStates[] =
{
	1,  // D3DBLEND_ZERO,
	2,  // D3DBLEND_ONE,
	3,  // D3DBLEND_SRCCOLOR,
	4,  // D3DBLEND_INVSRCCOLOR,
	5,  // D3DBLEND_SRCALPHA,
	6,  // D3DBLEND_INVSRCALPHA,
	7,  // D3DBLEND_DESTALPHA,
	8,  // D3DBLEND_INVDESTALPHA,
	9,  // D3DBLEND_DESTCOLOR,
	10, // D3DBLEND_INVDESTCOLOR,
	11, // D3DBLEND_SRCALPHASAT,
	14, // D3DBLEND_BLENDFACTOR,
	15, // D3DBLEND_INVBLENDFACTOR,
};

#define NUM_PERMUTABLE_BLEND_TYPES ARRAYSIZE(g_cPermutableBlendStates)
enum
{
	NUM_PERMUTABLE_TESTS = NUM_PERMUTABLE_BLEND_TYPES * NUM_PERMUTABLE_BLEND_TYPES,
	D3DBLEND_BOTHINVSRCALPHA_TEST,
	D3DBLEND_BOTHSRCALPHA_TEST,
	NUM_ALPHA_BLEND_TESTS
};
#else
#define MAX_TESTS		30
#endif

// Class definition
class CAlphaBldTest: public CD3DTest
{
    // Data
	private:
    D3DTLVERTEX     VertexList[40];
    D3DTLVERTEX     BackgroundList[400];
    DWORD           dwVertexCount;
	int				nAlphaValue[5];
    DWORD           dwBlendOp[5];
	DWORD			dwBlendFactor[5];
	DWORD			dwBlendFactorAlpha[5];
    bool            bBlendOps, bSeparateAlphaCap;
	FORMAT			BackBufferFormat;
	TCHAR			msgString[80];

    // Data
    public:
	DWORD			dwSrcCap, dwSrcBlend;
	DWORD			dwDestCap, dwDestBlend;
    LPCTSTR			szSrcName;
	LPCTSTR			szDestName;
	int				nAlphaEnable;
	bool			bChecker;
	bool			bSwapEffectCap;
	bool			bBlendFactor;
	bool			bSeparateAlpha;

#ifdef FAST_TEST
	int             numBlendOps;
#endif

    public:
	CAlphaBldTest();

	// Framework functions
    virtual bool PreModeSetup(void);
	virtual bool CapsCheck(void);
	virtual UINT TestInitialize(void);
    virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
    virtual void UpdateStatus(void);

	// Helper functions
	void DrawBackground(void);
#ifdef FAST_TEST
	void DrawGrid(int nTest, int nMesh, int xOffset, int yOffset, int gridWidth, int gridHeight);
#else
	void DrawGrid(int nTest, int nMesh);
#endif
	void DrawCircle(LPD3DVECTOR pCenter, float Radius, int nDivisions, D3DCOLOR Color);
};

#ifndef FAST_TEST
// False Class definitions
class CFalseTest: public CAlphaBldTest
{
	public:
	CFalseTest();
};

// Zero/Zero Class definitions
class CZeroZeroTest: public CAlphaBldTest
{
	public:
	CZeroZeroTest();
};

// Zero/One Class definitions
class CZeroOneTest: public CAlphaBldTest
{
	public:
	CZeroOneTest();
};

// Zero/SrcColor Class definitions
class CZeroSrcColorTest: public CAlphaBldTest
{
	public:
	CZeroSrcColorTest();
};

// Zero/InvSrcColor Class definitions
class CZeroInvSrcColorTest: public CAlphaBldTest
{
	public:
	CZeroInvSrcColorTest();
};

// Zero/SrcAlpha Class definitions
class CZeroSrcAlphaTest: public CAlphaBldTest
{
	public:
	CZeroSrcAlphaTest();
};

// Zero/InvSrcAlpha Class definitions
class CZeroInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CZeroInvSrcAlphaTest();
};

// Zero/DestAlpha Class definitions
class CZeroDestAlphaTest: public CAlphaBldTest
{
	public:
	CZeroDestAlphaTest();
};

// Zero/InvDestAlpha Class definitions
class CZeroInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CZeroInvDestAlphaTest();
};

// Zero/DestColor Class definitions
class CZeroDestColorTest: public CAlphaBldTest
{
	public:
	CZeroDestColorTest();
};

// Zero/InvDestColor Class definitions
class CZeroInvDestColorTest: public CAlphaBldTest
{
	public:
	CZeroInvDestColorTest();
};

// Zero/SrcAlphaSat Class definitions
class CZeroSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CZeroSrcAlphaSatTest();
};

// Zero/BlendFactor Class definitions
class CZeroBlendFactorTest: public CAlphaBldTest
{
	public:
	CZeroBlendFactorTest();
};

// Zero/InvBlendFactor Class definitions
class CZeroInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CZeroInvBlendFactorTest();
};

// One/Zero Class definitions
class COneZeroTest: public CAlphaBldTest
{
	public:
	COneZeroTest();
};

// One/One Class definitions
class COneOneTest: public CAlphaBldTest
{
	public:
	COneOneTest();
};

// One/SrcColor Class definitions
class COneSrcColorTest: public CAlphaBldTest
{
	public:
	COneSrcColorTest();
};

// One/InvSrcColor Class definitions
class COneInvSrcColorTest: public CAlphaBldTest
{
	public:
	COneInvSrcColorTest();
};

// One/SrcAlpha Class definitions
class COneSrcAlphaTest: public CAlphaBldTest
{
	public:
	COneSrcAlphaTest();
};

// One/InvSrcAlpha Class definitions
class COneInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	COneInvSrcAlphaTest();
};

// One/DestAlpha Class definitions
class COneDestAlphaTest: public CAlphaBldTest
{
	public:
	COneDestAlphaTest();
};

// One/InvDestAlpha Class definitions
class COneInvDestAlphaTest: public CAlphaBldTest
{
	public:
	COneInvDestAlphaTest();
};

// One/DestColor Class definitions
class COneDestColorTest: public CAlphaBldTest
{
	public:
	COneDestColorTest();
};

// One/InvDestColor Class definitions
class COneInvDestColorTest: public CAlphaBldTest
{
	public:
	COneInvDestColorTest();
};

// One/SrcAlphaSat Class definitions
class COneSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	COneSrcAlphaSatTest();
};

// One/BlendFactor Class definitions
class COneBlendFactorTest: public CAlphaBldTest
{
	public:
	COneBlendFactorTest();
};

// One/InvBlendFactor Class definitions
class COneInvBlendFactorTest: public CAlphaBldTest
{
	public:
	COneInvBlendFactorTest();
};

// SrcColor/Zero Class definitions
class CSrcColorZeroTest: public CAlphaBldTest
{
	public:
	CSrcColorZeroTest();
};

// SrcColor/One Class definitions
class CSrcColorOneTest: public CAlphaBldTest
{
	public:
	CSrcColorOneTest();
};

// SrcColor/SrcColor Class definitions
class CSrcColorSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcColorSrcColorTest();
};

// SrcColor/InvSrcColor Class definitions
class CSrcColorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcColorInvSrcColorTest();
};

// SrcColor/SrcAlpha Class definitions
class CSrcColorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcColorSrcAlphaTest();
};

// SrcColor/InvSrcAlpha Class definitions
class CSrcColorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcColorInvSrcAlphaTest();
};

// SrcColor/DestAlpha Class definitions
class CSrcColorDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcColorDestAlphaTest();
};

// SrcColor/InvDestAlpha Class definitions
class CSrcColorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcColorInvDestAlphaTest();
};

// SrcColor/DestColor Class definitions
class CSrcColorDestColorTest: public CAlphaBldTest
{
	public:
	CSrcColorDestColorTest();
};

// SrcColor/InvDestColor Class definitions
class CSrcColorInvDestColorTest: public CAlphaBldTest
{
	public:
	CSrcColorInvDestColorTest();
};

// SrcColor/SrcAlphaSat Class definitions
class CSrcColorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CSrcColorSrcAlphaSatTest();
};

// SrcColor/BlendFactor Class definitions
class CSrcColorBlendFactorTest: public CAlphaBldTest
{
	public:
	CSrcColorBlendFactorTest();
};

// SrcColor/InvBlendFactor Class definitions
class CSrcColorInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CSrcColorInvBlendFactorTest();
};

// InvSrcColor/Zero Class definitions
class CInvSrcColorZeroTest: public CAlphaBldTest
{
	public:
	CInvSrcColorZeroTest();
};

// InvSrcColor/One Class definitions
class CInvSrcColorOneTest: public CAlphaBldTest
{
	public:
	CInvSrcColorOneTest();
};

// InvSrcColor/SrcColor Class definitions
class CInvSrcColorSrcColorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorSrcColorTest();
};

// InvSrcColor/InvSrcColor Class definitions
class CInvSrcColorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvSrcColorTest();
};

// InvSrcColor/SrcAlpha Class definitions
class CInvSrcColorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcColorSrcAlphaTest();
};

// InvSrcColor/InvSrcAlpha Class definitions
class CInvSrcColorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvSrcAlphaTest();
};

// InvSrcColor/DestAlpha Class definitions
class CInvSrcColorDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcColorDestAlphaTest();
};

// InvSrcColor/InvDestAlpha Class definitions
class CInvSrcColorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvDestAlphaTest();
};

// InvSrcColor/DestColor Class definitions
class CInvSrcColorDestColorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorDestColorTest();
};

// InvSrcColor/InvDestColor Class definitions
class CInvSrcColorInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvDestColorTest();
};

// InvSrcColor/SrcAlphaSat Class definitions
class CInvSrcColorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvSrcColorSrcAlphaSatTest();
};

// InvSrcColor/BlendFactor Class definitions
class CInvSrcColorBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorBlendFactorTest();
};

// InvSrcColor/InvBlendFactor Class definitions
class CInvSrcColorInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvBlendFactorTest();
};

// SrcAlpha/Zero Class definitions
class CSrcAlphaZeroTest: public CAlphaBldTest
{
	public:
	CSrcAlphaZeroTest();
};

// SrcAlpha/One Class definitions
class CSrcAlphaOneTest: public CAlphaBldTest
{
	public:
	CSrcAlphaOneTest();
};

// SrcAlpha/SrcColor Class definitions
class CSrcAlphaSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSrcColorTest();
};

// SrcAlpha/InvSrcColor Class definitions
class CSrcAlphaInvSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvSrcColorTest();
};

// SrcAlpha/SrcAlpha Class definitions
class CSrcAlphaSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSrcAlphaTest();
};

// SrcAlpha/InvSrcAlpha Class definitions
class CSrcAlphaInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvSrcAlphaTest();
};

// SrcAlpha/DestAlpha Class definitions
class CSrcAlphaDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaDestAlphaTest();
};

// SrcAlpha/InvDestAlpha Class definitions
class CSrcAlphaInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvDestAlphaTest();
};

// SrcAlpha/DestColor Class definitions
class CSrcAlphaDestColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaDestColorTest();
};

// SrcAlpha/InvDestColor Class definitions
class CSrcAlphaInvDestColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvDestColorTest();
};

// SrcAlpha/SrcAlphaSat Class definitions
class CSrcAlphaSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSrcAlphaSatTest();
};

// SrcAlpha/BlendFactor Class definitions
class CSrcAlphaBlendFactorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaBlendFactorTest();
};

// SrcAlpha/InvBlendFactor Class definitions
class CSrcAlphaInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvBlendFactorTest();
};

// InvSrcAlpha/Zero Class definitions
class CInvSrcAlphaZeroTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaZeroTest();
};

// InvSrcAlpha/One Class definitions
class CInvSrcAlphaOneTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaOneTest();
};

// InvSrcAlpha/SrcColor Class definitions
class CInvSrcAlphaSrcColorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaSrcColorTest();
};

// InvSrcAlpha/InvSrcColor Class definitions
class CInvSrcAlphaInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvSrcColorTest();
};

// InvSrcAlpha/SrcAlpha Class definitions
class CInvSrcAlphaSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaSrcAlphaTest();
};

// InvSrcAlpha/InvSrcAlpha Class definitions
class CInvSrcAlphaInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvSrcAlphaTest();
};

// InvSrcAlpha/DestAlpha Class definitions
class CInvSrcAlphaDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaDestAlphaTest();
};

// InvSrcAlpha/InvDestAlpha Class definitions
class CInvSrcAlphaInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvDestAlphaTest();
};

// InvSrcAlpha/DestColor Class definitions
class CInvSrcAlphaDestColorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaDestColorTest();
};

// InvSrcAlpha/InvDestColor Class definitions
class CInvSrcAlphaInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvDestColorTest();
};

// InvSrcAlpha/SrcAlphaSat Class definitions
class CInvSrcAlphaSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaSrcAlphaSatTest();
};

// InvSrcAlpha/BlendFactor Class definitions
class CInvSrcAlphaBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaBlendFactorTest();
};

// InvSrcAlpha/InvBlendFactor Class definitions
class CInvSrcAlphaInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvBlendFactorTest();
};

// DestAlpha/Zero Class definitions
class CDestAlphaZeroTest: public CAlphaBldTest
{
	public:
	CDestAlphaZeroTest();
};

// DestAlpha/One Class definitions
class CDestAlphaOneTest: public CAlphaBldTest
{
	public:
	CDestAlphaOneTest();
};

// DestAlpha/SrcColor Class definitions
class CDestAlphaSrcColorTest: public CAlphaBldTest
{
	public:
	CDestAlphaSrcColorTest();
};

// DestAlpha/InvSrcColor Class definitions
class CDestAlphaInvSrcColorTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvSrcColorTest();
};

// DestAlpha/SrcAlpha Class definitions
class CDestAlphaSrcAlphaTest: public CAlphaBldTest
{
	public:
	CDestAlphaSrcAlphaTest();
};

// DestAlpha/InvSrcAlpha Class definitions
class CDestAlphaInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvSrcAlphaTest();
};

// DestAlpha/DestAlpha Class definitions
class CDestAlphaDestAlphaTest: public CAlphaBldTest
{
	public:
	CDestAlphaDestAlphaTest();
};

// DestAlpha/InvDestAlpha Class definitions
class CDestAlphaInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvDestAlphaTest();
};

// DestAlpha/DestColor Class definitions
class CDestAlphaDestColorTest: public CAlphaBldTest
{
	public:
	CDestAlphaDestColorTest();
};

// DestAlpha/InvDestColor Class definitions
class CDestAlphaInvDestColorTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvDestColorTest();
};

// DestAlpha/SrcAlphaSat Class definitions
class CDestAlphaSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CDestAlphaSrcAlphaSatTest();
};

// DestAlpha/BlendFactor Class definitions
class CDestAlphaBlendFactorTest: public CAlphaBldTest
{
	public:
	CDestAlphaBlendFactorTest();
};

// DestAlpha/InvBlendFactor Class definitions
class CDestAlphaInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvBlendFactorTest();
};

// InvDestAlpha/Zero Class definitions
class CInvDestAlphaZeroTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaZeroTest();
};

// InvDestAlpha/One Class definitions
class CInvDestAlphaOneTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaOneTest();
};

// InvDestAlpha/SrcColor Class definitions
class CInvDestAlphaSrcColorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaSrcColorTest();
};

// InvDestAlpha/InvSrcColor Class definitions
class CInvDestAlphaInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvSrcColorTest();
};

// InvDestAlpha/SrcAlpha Class definitions
class CInvDestAlphaSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaSrcAlphaTest();
};

// InvDestAlpha/InvSrcAlpha Class definitions
class CInvDestAlphaInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvSrcAlphaTest();
};

// InvDestAlpha/DestAlpha Class definitions
class CInvDestAlphaDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaDestAlphaTest();
};

// InvDestAlpha/InvDestAlpha Class definitions
class CInvDestAlphaInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvDestAlphaTest();
};

// InvDestAlpha/DestColor Class definitions
class CInvDestAlphaDestColorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaDestColorTest();
};

// InvDestAlpha/InvDestColor Class definitions
class CInvDestAlphaInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvDestColorTest();
};

// InvDestAlpha/SrcAlphaSat Class definitions
class CInvDestAlphaSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaSrcAlphaSatTest();
};

// InvDestAlpha/BlendFactor Class definitions
class CInvDestAlphaBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaBlendFactorTest();
};

// InvDestAlpha/InvBlendFactor Class definitions
class CInvDestAlphaInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvBlendFactorTest();
};

// DestColor/Zero Class definitions
class CDestColorZeroTest: public CAlphaBldTest
{
	public:
	CDestColorZeroTest();
};

// DestColor/One Class definitions
class CDestColorOneTest: public CAlphaBldTest
{
	public:
	CDestColorOneTest();
};

// DestColor/SrcColor Class definitions
class CDestColorSrcColorTest: public CAlphaBldTest
{
	public:
	CDestColorSrcColorTest();
};

// DestColor/InvSrcColor Class definitions
class CDestColorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CDestColorInvSrcColorTest();
};

// DestColor/SrcAlpha Class definitions
class CDestColorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CDestColorSrcAlphaTest();
};

// DestColor/InvSrcAlpha Class definitions
class CDestColorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CDestColorInvSrcAlphaTest();
};

// DestColor/DestAlpha Class definitions
class CDestColorDestAlphaTest: public CAlphaBldTest
{
	public:
	CDestColorDestAlphaTest();
};

// DestColor/InvDestAlpha Class definitions
class CDestColorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CDestColorInvDestAlphaTest();
};

// DestColor/DestColor Class definitions
class CDestColorDestColorTest: public CAlphaBldTest
{
	public:
	CDestColorDestColorTest();
};

// DestColor/InvDestColor Class definitions
class CDestColorInvDestColorTest: public CAlphaBldTest
{
	public:
	CDestColorInvDestColorTest();
};

// DestColor/SrcAlphaSat Class definitions
class CDestColorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CDestColorSrcAlphaSatTest();
};

// DestColor/BlendFactor Class definitions
class CDestColorBlendFactorTest: public CAlphaBldTest
{
	public:
	CDestColorBlendFactorTest();
};

// DestColor/InvBlendFactor Class definitions
class CDestColorInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CDestColorInvBlendFactorTest();
};

// InvDestColor/Zero Class definitions
class CInvDestColorZeroTest: public CAlphaBldTest
{
	public:
	CInvDestColorZeroTest();
};

// InvDestColor/One Class definitions
class CInvDestColorOneTest: public CAlphaBldTest
{
	public:
	CInvDestColorOneTest();
};

// InvDestColor/SrcColor Class definitions
class CInvDestColorSrcColorTest: public CAlphaBldTest
{
	public:
	CInvDestColorSrcColorTest();
};

// InvDestColor/InvSrcColor Class definitions
class CInvDestColorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvSrcColorTest();
};

// InvDestColor/SrcAlpha Class definitions
class CInvDestColorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestColorSrcAlphaTest();
};

// InvDestColor/InvSrcAlpha Class definitions
class CInvDestColorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvSrcAlphaTest();
};

// InvDestColor/DestAlpha Class definitions
class CInvDestColorDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestColorDestAlphaTest();
};

// InvDestColor/InvDestAlpha Class definitions
class CInvDestColorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvDestAlphaTest();
};

// InvDestColor/DestColor Class definitions
class CInvDestColorDestColorTest: public CAlphaBldTest
{
	public:
	CInvDestColorDestColorTest();
};

// InvDestColor/InvDestColor Class definitions
class CInvDestColorInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvDestColorTest();
};

// InvDestColor/SrcAlphaSat Class definitions
class CInvDestColorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvDestColorSrcAlphaSatTest();
};

// InvDestColor/BlendFactor Class definitions
class CInvDestColorBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvDestColorBlendFactorTest();
};

// InvDestColor/InvBlendFactor Class definitions
class CInvDestColorInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvBlendFactorTest();
};

// SrcAlphaSat/Zero Class definitions
class CSrcAlphaSatZeroTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatZeroTest();
};

// SrcAlphaSat/One Class definitions
class CSrcAlphaSatOneTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatOneTest();
};

// SrcAlphaSat/SrcColor Class definitions
class CSrcAlphaSatSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatSrcColorTest();
};

// SrcAlphaSat/InvSrcColor Class definitions
class CSrcAlphaSatInvSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvSrcColorTest();
};

// SrcAlphaSat/SrcAlpha Class definitions
class CSrcAlphaSatSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatSrcAlphaTest();
};

// SrcAlphaSat/InvSrcAlpha Class definitions
class CSrcAlphaSatInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvSrcAlphaTest();
};

// SrcAlphaSat/DestAlpha Class definitions
class CSrcAlphaSatDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatDestAlphaTest();
};

// SrcAlphaSat/InvDestAlpha Class definitions
class CSrcAlphaSatInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvDestAlphaTest();
};

// SrcAlphaSat/DestColor Class definitions
class CSrcAlphaSatDestColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatDestColorTest();
};

// SrcAlphaSat/InvDestColor Class definitions
class CSrcAlphaSatInvDestColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvDestColorTest();
};

// SrcAlphaSat/SrcAlphaSat Class definitions
class CSrcAlphaSatSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatSrcAlphaSatTest();
};

// SrcAlphaSat/BlendFactor Class definitions
class CSrcAlphaSatBlendFactorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatBlendFactorTest();
};

// SrcAlphaSat/InvBlendFactor Class definitions
class CSrcAlphaSatInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvBlendFactorTest();
};

// BothSrcAlpha Class definitions
class CBothSrcAlphaTest: public CAlphaBldTest
{
	public:
	CBothSrcAlphaTest();
};

// BothInvSrcAlpha Class definitions
class CBothInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CBothInvSrcAlphaTest();
};

// BlendFactor/Zero Class definitions
class CBlendFactorZeroTest: public CAlphaBldTest
{
	public:
	CBlendFactorZeroTest();
};

// BlendFactor/One Class definitions
class CBlendFactorOneTest: public CAlphaBldTest
{
	public:
	CBlendFactorOneTest();
};

// BlendFactor/SrcColor Class definitions
class CBlendFactorSrcColorTest: public CAlphaBldTest
{
	public:
	CBlendFactorSrcColorTest();
};

// BlendFactor/InvSrcColor Class definitions
class CBlendFactorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CBlendFactorInvSrcColorTest();
};

// BlendFactor/SrcAlpha Class definitions
class CBlendFactorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CBlendFactorSrcAlphaTest();
};

// BlendFactor/InvSrcAlpha Class definitions
class CBlendFactorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CBlendFactorInvSrcAlphaTest();
};

// BlendFactor/DestAlpha Class definitions
class CBlendFactorDestAlphaTest: public CAlphaBldTest
{
	public:
	CBlendFactorDestAlphaTest();
};

// BlendFactor/InvDestAlpha Class definitions
class CBlendFactorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CBlendFactorInvDestAlphaTest();
};

// BlendFactor/DestColor Class definitions
class CBlendFactorDestColorTest: public CAlphaBldTest
{
	public:
	CBlendFactorDestColorTest();
};

// BlendFactor/InvDestColor Class definitions
class CBlendFactorInvDestColorTest: public CAlphaBldTest
{
	public:
	CBlendFactorInvDestColorTest();
};

// BlendFactor/SrcAlphaSat Class definitions
class CBlendFactorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CBlendFactorSrcAlphaSatTest();
};

// BlendFactor/BlendFactor Class definitions
class CBlendFactorBlendFactorTest: public CAlphaBldTest
{
	public:
	CBlendFactorBlendFactorTest();
};

// BlendFactor/InvBlendFactor Class definitions
class CBlendFactorInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CBlendFactorInvBlendFactorTest();
};

// InvBlendFactor/Zero Class definitions
class CInvBlendFactorZeroTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorZeroTest();
};

// InvBlendFactor/One Class definitions
class CInvBlendFactorOneTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorOneTest();
};

// InvBlendFactor/SrcColor Class definitions
class CInvBlendFactorSrcColorTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorSrcColorTest();
};

// InvBlendFactor/InvSrcColor Class definitions
class CInvBlendFactorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorInvSrcColorTest();
};

// InvBlendFactor/SrcAlpha Class definitions
class CInvBlendFactorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorSrcAlphaTest();
};

// InvBlendFactor/InvSrcAlpha Class definitions
class CInvBlendFactorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorInvSrcAlphaTest();
};

// InvBlendFactor/DestAlpha Class definitions
class CInvBlendFactorDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorDestAlphaTest();
};

// InvBlendFactor/InvDestAlpha Class definitions
class CInvBlendFactorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorInvDestAlphaTest();
};

// InvBlendFactor/DestColor Class definitions
class CInvBlendFactorDestColorTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorDestColorTest();
};

// InvBlendFactor/InvDestColor Class definitions
class CInvBlendFactorInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorInvDestColorTest();
};

// InvBlendFactor/SrcAlphaSat Class definitions
class CInvBlendFactorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorSrcAlphaSatTest();
};

// InvBlendFactor/BlendFactor Class definitions
class CInvBlendFactorBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorBlendFactorTest();
};

// InvBlendFactor/InvBlendFactor Class definitions
class CInvBlendFactorInvBlendFactorTest: public CAlphaBldTest
{
	public:
	CInvBlendFactorInvBlendFactorTest();
};

// AlphaTargetFalse Class definitions
class CAlphaTargetFalseTest: public CAlphaBldTest
{
	public:
	CAlphaTargetFalseTest();

	bool PreModeSetup(void);
};

// SwapEffect Class definitions
class CSwapEffectTest: public CAlphaBldTest
{
	public:
	CSwapEffectTest();

	bool FilterResolution(PDISPLAYMODE pMode);
};

// SwapEffectCopy Class definitions
class CSwapEffectCopyTest: public CSwapEffectTest
{
	public:
	CSwapEffectCopyTest();
};

// SwapEffectFlip Class definitions
class CSwapEffectFlipTest: public CSwapEffectTest
{
	public:
	CSwapEffectFlipTest();
};

// SwapEffectDiscard Class definitions
class CSwapEffectDiscardTest: public CSwapEffectTest
{
	public:
	CSwapEffectDiscardTest();
};

#endif // FAST_TEST
#endif
