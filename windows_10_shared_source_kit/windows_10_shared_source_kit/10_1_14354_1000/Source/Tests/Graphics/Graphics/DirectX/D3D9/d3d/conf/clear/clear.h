//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// clear_test.h
//
// CClearTest class - Tests many possible IDirect3DDeviceX::Clear() parameter combinations
//
// History: 8/21/2000 Bobby Paulose     - Created
//
//////////////////////////////////////////////////////////////////////

#ifndef __CLEAR_TEST_H__
#define __CLEAR_TEST_H__

#include "CD3DTest.h"
#include "CTextures.h"

// Get access to the framework
extern CD3DWindowFramework   *g_pFramework;

#define GLE(x) (m_pD3D->HResultToString(x))

#ifndef D3DRGBA
#define D3DRGBA(r, g, b, a) ((((DWORD) (255.0*a)) << 24) | (((DWORD) (255.0*r)) << 16) | (((DWORD) (255.0*g)) << 8) | (((DWORD) (255.0*b)) << 6)) 
#endif

#define CLEAR_COLOR_FULL	D3DRGBA(1.0, 0.0, 0.0, 1.0) //red 
#define CLEAR_COLOR_FULL_2	D3DRGBA(0.0, 0.0, 1.0, 1.0) //blue 
#define CLEAR_COLOR_FULL_ALPHA0	D3DRGBA(1.0, 0.0, 0.0, 0.0) 
#define CLEAR_COLOR_FULL_ALPHA1	D3DRGBA(1.0, 0.0, 0.0, 1.0) // Alpha 1.0 is ok for even formats with A just 1 bit.

#define CLEAR_COLOR_0	D3DRGBA(0.0, 0.0, 0.0, 1.0) 

#define CLEAR_COLOR__RECTS	D3DRGBA(0.0, 0.0, 1.0, 0.0) //blue

#define RENDERING_COLOR		D3DRGBA(0.0, 1.0, 0.0, 1.0) // Green used to render triangles.

#define COLOR_NOTUSED 0xffffffff


#define LOWER_Z 0.49f
#define HIGHER_Z 0.51f
#define STENCIL_0 0
#define STENCIL_1 1



#define TEST_CASES 14
#define NUM_VERTICES 6
#define IMGHEIGHT	256
#define IMGWIDTH	256
#define ARGB_MAKE(a, r, g, b) RGBA_MAKE(r, g, b, a) 


// Class definition
class CClearTest: public CD3DTest
{
    // Private Data

    CnTexture *  m_pTextureRT;
    CSurface * m_pOffSrcSurf, * m_pOffRefSurf;
    CSurface * m_pSrcZ, * m_pRefZ;
    
    CSurface * m_pOriginalSrcRT, * m_pOriginalRefRT; 
    CSurface * m_pOriginalSrcZ, * m_pOriginalRefZ; 

    int m_nOriginalZFormat, m_nTexFormat, m_nZFormat, m_nTestCase, m_nTestCases;

	bool m_bAlphaPresent, m_bStencilPresent, m_bScissor;

	static const DWORD VertexFVF;

	typedef struct { float x,y,z,rhw; D3DCOLOR d; } Vertex;

	static const DWORD TexVertexFVF;

	typedef struct { float x,y,z,rhw; D3DCOLOR d; float u, v;} TexVertex;

	static const DWORD dwVertexCount;

	static const Vertex VertexList[NUM_VERTICES];

	static const TexVertex Grid[NUM_VERTICES];

	BOOL boolResults[TEST_CASES];

	protected:
    bool m_bOffscreen, m_bTestRangeSpecified;

    // Public fns
	public:
	CClearTest();
	~CClearTest();

	// Framework functions
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool TestTerminate(void);


	//helper fns
	bool CreateOffScrRendTrgts(int nTexFormat);
	bool CreateZStencil(int nZFormat);
	bool CopyToTexture(void);
	void DescribeFormat(TCHAR * pBuf, int nTexFormat, int nZFormat);
	void DescribeTestcase(TCHAR * pBuf, int nTestcase);
	bool CheckDepthStencilMatch8(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat);
	D3DFORMAT GetCurrentAdapterMode8(UINT Adapter);
    bool DisableScissorTest(VOID);
    bool EnableScissorTest(RECT *pRect);
};




// Clear Texture as render target.
class CTextureClearTest: public CClearTest
{
	public:
	CTextureClearTest();
	~CTextureClearTest();
};


// Clear Offscreen as render target.
class COffscreenClearTest: public CClearTest
{
	public:
	COffscreenClearTest();
	~COffscreenClearTest();
};



#endif  //__CLEAR_TEST_H__
