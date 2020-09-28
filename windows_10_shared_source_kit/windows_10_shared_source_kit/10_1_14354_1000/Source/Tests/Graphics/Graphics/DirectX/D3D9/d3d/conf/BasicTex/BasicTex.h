//-----------------------------------------------------------------------------
// File: BasicTex.h
//
// Desc: Class dif for all BasicTex tests
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef __BASICTEX_H__
#define __BASICTEX_H__

#define D3D_OVERLOADS
#include <d3d.h>
#include "CD3DTest.h"
#include "CTextures.h"

#define VERTEXES_MAX 528
#define MSGSIZE_MAX 80
#define GRIDSIZE_MAX 11

// UV structure
typedef struct _UV{
	float u;
	float v;

public:
	_UV() {u=0.0f; v=0.0f;};
	_UV(float u1, float v1) {u=u1; v=v1;};

} UV;

//----------------------------------------------------------------------------
// Name: CBasicText
// Desc: Base class for BasicText test groups
//-----------------------------------------------------------------------------
class CBasicTex: public CD3DTest
{
    // Private Data
	private:
    D3DTLVERTEX     m_VertexList[VERTEXES_MAX];     // Our vertex list
    DWORD           m_dwVertexCount;                // Number of vertex currently in list
	TCHAR			m_szTSTName[MSGSIZE_MAX];       // Test name string
	TCHAR			m_szTexLoadStatus[MSGSIZE_MAX]; // Texture load type string

    // Public Data
	public:
    CnTexture*		m_pTexture;         // The one and only texture
    UINT            m_nTextureSize;     // Size of the texture m_nTextureSize X m_nTextureSize to load
    DWORD           m_dwTexturePool;    // Pool to load the texture with
    UV				m_uvTopLeft;        // UV for top left of grid
	UV				m_uvTopRight;       // UV for top right of grid
	UV				m_uvBottomLeft;     // UV for bottom left of grid
	UV				m_uvBottomRight;    // UV for bottom right of grid
    int             m_nGridXDiv;        // The number of squares in the X of the grid
    int             m_nGridYDiv;        // The number of squares in the Y of the grid
    bool            m_bUseStripeImage;  // To load a striped image or a gradent
    UINT            m_uNumTests;        // The number of tests to run (chilled must set)
    
    // Private Test Functions
	private:
    bool  UpdateTexLoadStatus(void);                // Updates m_szTexLoadStatus with currect tex load string
    bool  CreateTex(void);                          // Creates m_pTexture
    bool  LoadTexImage(void);                       // Loads am image on to m_pTexture depending on m_bUseStripeImage
    bool  PoolToString(DWORD dwPool, LPTSTR szOut); // Converts memory pool to string
#if FAST_TEST
    bool  RenderTest(UINT testNum);                         // Renders one frame
#else
    bool  RenderTest(void);                         // Renders one frame
#endif
    //bool  AddColor(D3DCOLOR color);                 // Adds a color to m_Colors and adds 1 to m_dwColorCount
    void  DrawGrid();                               // Creats a grid using triangle strips depending on m_nGrid*Div
    void  CalcUV(int nX, int nY, float* uOut, float* vOut); // Returns the UV coords for a row col mix based on m_uv*
    float CalcCoord(float fFirst, float fSecond, int nStepping, int nGridDivide); // Helper divide for CalcUV

	// D3D Framework Derived Functions
	public:
	virtual bool CapsCheck(void);       // Check caps requred by test. 
	virtual UINT TestInitialize(void);  // Setup non-device specific settings, variables, ect.
	virtual bool Setup(void);           // Setup device specific settings, variables, ect.
    virtual void Cleanup(void);         // Cleanup device specific settings, variables, ect. Tied with Setup()
	virtual bool ExecuteTest(UINT);     // Called once for each test defined by SetTestRange() call
    virtual bool ClearFrame(void);      // Clear our buffers
    virtual void UpdateStatus(void);    // Called once each frame to update status window text     

#if FAST_TEST
    virtual bool AddFormatModes(DISPLAYMODE *pMode, bool bWindowed);
#endif
    
	// Public Test Functions
	CBasicTex();
	~CBasicTex() {;}
};

//----------------------------------------------------------------------------
// Name: CMapping
// Desc: Mapping test group.
// Cycles texture mappings
//-----------------------------------------------------------------------------
#define MAPPINGTESTS 256
#define MAPPINGTESTS_TILE 60
class CMapping: public CBasicTex
{
    // Private Data
	private:
    UV    m_uvlstTopLeft[4];
    UV    m_uvlstTopRight[4];
    UV    m_uvlstBottomLeft[4];
    UV    m_uvlstBottomRight[4];

	// D3D Framework Derived Functions
	public:
	virtual bool CapsCheck(void);       // Check caps requred by test. 
	virtual bool ExecuteTest(UINT);     // Called once for each test defined by SetTestRange() call
    virtual void CommandLineHelp(void); // Command line help for the test case
    virtual bool Setup(void);

	// Public Test Functions
	CMapping();
    ~CMapping() {;}

    bool ParseGridDiv(TCHAR *pcsGrid, int *pX, int *pY);
};

//----------------------------------------------------------------------------
// Name: AGP
// Desc: Base class for all BasicTex - AGP tests. 
// Handles cap check/texture pool setup, and verification of texture on ProcessFrame
//-----------------------------------------------------------------------------
class CAGP: public CBasicTex
{    
	// D3D Framework Derived Functions
	public:
	virtual bool CapsCheck(void);       // Check caps requred by test. 
    virtual bool ProcessFrame(void);    // Make sure texture is in AGP
    virtual void CommandLineHelp(void); // Command line help for the test case

	// Public Test Functions
	CAGP();
    ~CAGP(){;}
};

//----------------------------------------------------------------------------
// Name: CAGPnxn
// Desc: AGP Texture size test groups.
// Handles test group identification and setting texture size for CBasicTex
//-----------------------------------------------------------------------------
class CAGP8x8: public CAGP
{
    // Public Test Functions
	public:
	CAGP8x8();
    ~CAGP8x8(){;}
};
class CAGP16x16: public CAGP
{
	// Public Test Functions
	public:	
	CAGP16x16();
    ~CAGP16x16(){;}
};
class CAGP64x64: public CAGP
{
	// Public Test Functions
	public:	
	CAGP64x64();
    ~CAGP64x64(){;}
};
class CAGP256x256: public CAGP
{
	// Public Test Functions
	public:	
	CAGP256x256();
    ~CAGP256x256(){;}
};
class CAGP1024x1024: public CAGP
{
	// Public Test Functions
	public:	
	CAGP1024x1024();
    ~CAGP1024x1024(){;}
};


#endif
