/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	TexFormats.h
 *
 ***************************************************************************/

#ifndef __TEXFORMATS_H__
#define __TEXFORMATS_H__


#include "CD3DTest.h"
#include "CTextures.h"

//#include <ddraw.h>
//#include <d3d8.h>
//#include "CD3D.h"

// Defines
#define MAX_TESTS	4

//Error macro
#define GLE(x) (m_pD3D->HResultToString(x))

// Class definition
class CTexFormatsTest: public CD3DTest
{
    // Private Data
	protected:
    D3DTLVERTEX     VertexList[6];
	CnTexture *		pTexture;

    // Public Data
	public:
    DWORD           dwVertexCount;
    DWORD           dwLoadOption;
	D3DCOLOR		Stripes[8];
    bool            bSysMem;
    bool            bFVF;
#if FAST_TEST
    TCHAR			szStatus[8000];
    TCHAR			msgString[8000];
#else
    TCHAR			szStatus[80];
    TCHAR			msgString[80];
#endif

	public:
	CTexFormatsTest();
	~CTexFormatsTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
    virtual void UpdateStatus(void);
	virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int nTest);
    virtual void DrawFVFGrid(void);

#if FAST_TEST
    UINT m_totalNumTestCases;
    virtual void TransformAndDraw(float scaleX, float scaleY, float transX, float transY);
    virtual bool AddFormatModes(DISPLAYMODE *pMode, bool bWindowed);
#endif
};

// UpdateTex Class definitions
class CUpdateTexTest: public CTexFormatsTest
{
	public:
	CUpdateTexTest();
};

// CopyRects Class definitions
class CCopyRectsTest: public CTexFormatsTest
{
	public:
	CCopyRectsTest();
};

// CopyRects Class definitions
class CLockCopyTest: public CTexFormatsTest
{
	public:
	CLockCopyTest();
};

// UpdateSurf Class definitions
class CUpdateSurfTest: public CTexFormatsTest
{
	public:
	CUpdateSurfTest();
};

// Verify Class definitions
class CVerifyTest: public CTexFormatsTest
{
    // Private Data
	private:
	bool	 bValidFormat;
	TCHAR	 errString[1024];
    FORMAT * pFormat;

	public:
	CVerifyTest();

	// Framework functions
	UINT TestInitialize(void);
	bool ExecuteTest(UINT);
    void SceneRefresh(void);
	bool ProcessFrame(void);

	// Helper functions
	bool VerifyFormat(const PIXELFORMAT *pDDPF);
};

// SysMem Class definitions
class CSysMemTest: public CTexFormatsTest
{
	public:
	CSysMemTest();

	// Framework functions
    HRESULT CreateDevices(DEVICEMODE *pMode);
};

// FVF3 Class definitions
class CFVF3Test: public CTexFormatsTest
{
    // Private Data
	private:
    struct D3D_FVF_VERTEX {
        D3DVALUE     x;             // Homogeneous coordinates 
        D3DVALUE     y;
        D3DVALUE     z;
        D3DVALUE     rhw;           // Reciprocal of homogeneous w 
        D3DCOLOR     color;         // Vertex color 
        D3DVALUE     tu;            // 3D Texture coordinates 
        D3DVALUE     tv;
        D3DVALUE     tw;
    };
    
    D3D_FVF_VERTEX FVF_VertexList[6];

    public:
	CFVF3Test();

	// Framework functions
    UINT TestInitialize(void);
    void SceneRefresh(void);

	// Helper functions
    void DrawFVFGrid(void);

#if FAST_TEST
    virtual void TransformAndDraw(float scaleX, float scaleY, float transX, float transY);
#endif
};

// FVF4 Class definitions
class CFVF4Test: public CTexFormatsTest
{
    // Private Data
	private:
    struct D3D_FVF_VERTEX {
        D3DVALUE     x;             // Homogeneous coordinates 
        D3DVALUE     y;
        D3DVALUE     z;
        D3DVALUE     rhw;           // Reciprocal of homogeneous w 
        D3DCOLOR     color;         // Vertex color 
        D3DVALUE     tu;            // 4D Texture coordinates 
        D3DVALUE     tv;
        D3DVALUE     tw;
        D3DVALUE     tq;
    };
    
    D3D_FVF_VERTEX FVF_VertexList[6];

    public:
	CFVF4Test();

	// Framework functions
    UINT TestInitialize(void);
    void SceneRefresh(void);

	// Helper functions
    void DrawFVFGrid(void);

#if FAST_TEST
    virtual void TransformAndDraw(float scaleX, float scaleY, float transX, float transY);
#endif
};

// Print Class definitions
class CPrintTest: public CTexFormatsTest
{
	public:
	CPrintTest();

	// Framework functions
	UINT TestInitialize(void);
};

//-----------------------------------------------------------------------------
// Floating point format tests
//-----------------------------------------------------------------------------

//handy error macro
#define FAIL_AND_EXIT \
	Fail(); \
	bReturn = false; \
	goto Exit;

//-----------------------------------------------------------------------------
//16-Bit format:
//1 sign bit, 5 exponent bits, 10 fraction bits
//thus e varies from 0 to 31
//and f varies from 0 to 1024
//calculated:  2^(e-15)*(1.f)
//so the max float should be  2^(31-15)*(1.999) = 1.31006e5  or maybe 2^(30-15)*(1.999) = 6.5503e4
//the smallest float should be 2^(0-15)*(1.000) = 3.0518e-5
//-----------------------------------------------------------------------------
#if FAST_TEST
const float F16_PRECISION_FACTORS[] = {1.0E0f,  1.0E5f, 3.0E5f, 
                                        1.0E-1f, 1.0E-4f, 3.0E-5f,
                                        -1.0E0f, -1.0E5f, -1.0E-1f, -1.0E-4f};

const float F32_PRECISION_FACTORS[] = {1.0E0f, 1.0E35f, 1.0E38f, 
                                        1.0E-1f, 1.0E-35f,
                                        -1.0E0f, -1.0E2f, -1.0E15f, -1.0E35f, -1.0E-1f, -1.0E-20f, -1.0E-35f};

const UINT NF16_PRECISION_TESTS = sizeof (F16_PRECISION_FACTORS) / sizeof (float);
const UINT NF32_PRECISION_TESTS = sizeof (F32_PRECISION_FACTORS) / sizeof (float);

const UINT NFLOAT16_TESTS = NF16_PRECISION_TESTS;
const UINT NFLOAT32_TESTS = NF32_PRECISION_TESTS;

const float MULTITEX_FACTORS[] = {1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 2.0f,
								  2.0f, 0.5f, 2.0f, 1.0f, 2.0f, 2.0f,
								  4.0f, 0.5f, 4.0f, 1.0f, 4.0f, 2.0f};
const UINT NMULTITEX_FACTORS = 3;
const UINT NMULTITEX_TESTS = 2 * NMULTITEX_FACTORS;
#else //FAST_TEST
const float F16_PRECISION_FACTORS[] = {1.0E0f, 1.0E1f, 1.0E2f, 1.0E3f, 1.0E4f, 1.0E5f, 3.0E5f, 
									   1.0E-1f, 1.0E-2f, 1.0E-3f, 1.0E-4f, 3.0E-5f,
									-1.0E0f, -1.0E2f, -1.0E4f, -1.0E5f, -1.0E-1f, -1.0E-3f, -1.0E-4f};

const float F32_PRECISION_FACTORS[] = {1.0E0f, 1.0E1f, 1.0E5f, 1.0E10f, 1.0E15f, 1.0E20f, 1.0E25f, 1.0E30f, 1.0E35f, 1.0E38f, 
									1.0E-1f, 1.0E-5f, 1.0E-10f, 1.0E-15f, 1.0E-20f, 1.0E-25f, 1.0E-30f, 1.0E-35f,
									-1.0E0f, -1.0E2f, -1.0E15f, -1.0E35f, -1.0E-1f, -1.0E-20f, -1.0E-35f};


const UINT NF16_PRECISION_TESTS = sizeof (F16_PRECISION_FACTORS) / sizeof (float);
const UINT NF32_PRECISION_TESTS = sizeof (F32_PRECISION_FACTORS) / sizeof (float);

const UINT NFLOAT16_TESTS = NF16_PRECISION_TESTS;
const UINT NFLOAT32_TESTS = NF32_PRECISION_TESTS;

const float MULTITEX_FACTORS[] = {1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 2.0f,
								  2.0f, 0.5f, 2.0f, 1.0f, 2.0f, 2.0f,
								  4.0f, 0.5f, 4.0f, 1.0f, 4.0f, 2.0f};
const UINT NMULTITEX_FACTORS = 9;
const UINT NMULTITEX_TESTS = 2 * NMULTITEX_FACTORS;
#endif //FAST_TEST

//-----------------------------------------------------------------------------

typedef union _FloatDWord
{
    FLOAT f;
    DWORD dw;
} FLOATDWORD;


struct TLVERT
{
	D3DVALUE x;
	D3DVALUE y;
	D3DVALUE z;
	D3DVALUE rhw;
	D3DCOLOR color;
	D3DVALUE s;
	D3DVALUE t;

	inline TLVERT () {}
	inline TLVERT (D3DVALUE _x, D3DVALUE _y, D3DVALUE _z, D3DVALUE _rhw, D3DCOLOR _color, D3DVALUE _s, D3DVALUE _t) : x(_x), y(_y), z(_z), rhw(_rhw), color(_color), s(_s), t(_t) {}
};

const DWORD FMT_TLVERT = {D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1};

//-----------------------------------------------------------------------------
// Float texture class definition
//-----------------------------------------------------------------------------

class CFloatTexTest: public CTexFormatsTest
{
private:
	TLVERT		m_pVertList[6];
	TLVERT		m_pFullSizeRect[6];
	CnEffect*	m_pNEffects;
	FORMAT*		m_pTexFormat;

public:
	bool		m_b16BitTest;	

	CFloatTexTest();

	// Framework functions
	bool CapsCheck (void);
	bool Setup (void);
	UINT TestInitialize (void);
	bool TestTerminate (void);
	void SceneRefresh (void);

	bool ExecuteMultiTextureTest (UINT numTexPairs, float brightness, char* backTexCreateTech);
};

class CFloatMultipleTexTest: public CFloatTexTest
{
public:
	CFloatMultipleTexTest();
	UINT TestInitialize (void);
	bool ExecuteTest (UINT uTest);
};

//---------------------------------------------------------------------------------
// Packed RGB texture format tests (D3DFMT_G8R8_G8B8 and D3DFMT_R8G8_B8G8)
//---------------------------------------------------------------------------------

class CPackedRGBTest : public CTexFormatsTest
{
private:
	CnEffect*	m_pEffects;
	CnTexture**	m_ppEncodedTextures;
	CnTexture** m_ppTextures;
	
	static DWORD		dwGradientColors [ ];
	static DWORD		dwStripes [ ];
	
	int		m_nTexWidth;
	int		m_nTexHeight;
	int		m_nLevels;

	bool	CreateTextures( );
	void	DrawScaledQuad( float nWidth, float nHeight );
	bool	SetShaderConstants( int nMipLevel, bool bGreenHiByte );

public:
	CPackedRGBTest( );

	bool	CapsCheck( );
	bool	Setup( );
	UINT	TestInitialize( );
	bool	ExecuteTest( UINT uTestNum );
	void	SceneRefresh( );
	void	UpdateStatus( );
	bool	TestTerminate( );
	void	Cleanup( );
	
};

#endif
