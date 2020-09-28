// StretchRectKeepStates.h: interface for the CStretchRectKeepStates class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRETCHRECTKEEPSTATES_H__F5654E0A_3560_40D4_8C8C_ABEC44E196C4__INCLUDED_)
#define AFX_STRETCHRECTKEEPSTATES_H__F5654E0A_3560_40D4_8C8C_ABEC44E196C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d9.h>
#include <d3dx9.h>
#include "CD3DTest.h"
#include "CShapes.h"
#include "CBuffers.h"
#include "CTextures.h"
#include <vector>
using namespace std;

#ifndef D3DVERTEXHANDLE
typedef struct _D3DVERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
} D3DVERTEX;

#define D3DVERTEXHANDLE	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

#define		WIDTH	4
#define		HEIGHT	3
#define		DEPTH	2
#endif

typedef struct
{
	FORMAT formatSrc;
	FORMAT formatDest;
} SRCDEST_FORMATS;

/************************************************************
 * Test specification										*
 ************************************************************/

class CStretchRectKeepStates : public CD3DTest
{
public:
	CStretchRectKeepStates();
	virtual ~CStretchRectKeepStates();

	// framework virtual methods
	virtual UINT TestInitialize();					// Init the testcase
	virtual bool TestTerminate();					// cleanup
	virtual bool ExecuteTest(UINT nTest);			// Execute the testcase
	virtual void OnDeviceLost();					// Called after DEVICELOST.  Test should release all D3D objects created in POOL_DEFAULT
    virtual void OnReset();							// Called after Reset() succeeds.  Test should re-create all D3D objects released in OnDeviceLost and refresh all data within existing objects.
	virtual bool CapsCheck();						// Check caps
	
protected:
	// testcase parameters (we'll assume surfaces to be 256x256 in POOL_DEFAULT, it also doesn't matter how the surfaces are created)
	vector<SRCDEST_FORMATS>	m_vecFormats;
	vector<RECT>	m_vecSrcRects;
	vector<RECT>	m_vecDestRects;
	vector<D3DTEXTUREFILTERTYPE>	m_vecFilters;

	D3DRENDERSTATETYPE	m_d3dRenderState;			// render state to be monitored
	vector<DWORD>	m_vecd3dRenderStateValues;		// various values to be set for this render state

	UINT m_nIndexFormat;							// index in the SrcDestFormats vector
	UINT m_nIndexSrcRect;							// index in the SrcRects vector
	UINT m_nIndexDestRect;							// index in the DestRects vector
	UINT m_nIndexFilter;							// index in the Filters vector
	UINT m_nIndexd3dRenderStateValue;				// index in the RenderStateValues vector

	CShapes			m_shapes[WIDTH*HEIGHT*DEPTH];			// all the shapes
	LPVERTEXBUFFER	m_pVertexBuffer[WIDTH*HEIGHT*DEPTH];	// vertex buffer
	LPINDEXBUFFER	m_pIndexBuffer[WIDTH*HEIGHT*DEPTH];		// index buffer
	LPTEXTURE		m_pTexture;								// texture to be applied to all boxes
	SURFACEDESC		m_sdBackBuffer;					// backbuffer description
	LPSURFACE		m_pBackBuffer;					// backbuffer surface
	LPSURFACE		m_pBackBufferSurfaceWithoutSR, m_pBackBufferSurfaceWithSR;	// back buffer contents with and without StretchRect call
	LPTEXTURE		m_pSrcTexture;					// Src and Dest textures for StretchRect; surface pointers will be obtained from them
	LPSURFACE		m_pSrcSurface, m_pDestSurfaceWithoutRS, m_pDestSurfaceWithRS;	// Src and Dest (withouth and with RenderState set) surface to be used in Stretch Rect

	bool			m_bSave;
	CAPS			m_DeviceCaps;
	DISPLAYMODE		m_DisplayMode;

	virtual void	InitializeRenderState()=0;		// virtual pure method to force derived classes from here, for each tested render state

	// helper methods
	void			Check_HRFail( HRESULT hResult, const char* sMethod );	// if hResult is not D3D_OK, throws an exception
	HRESULT			RenderScene(bool bRender, bool bStretchRect);			// render the main scene
	HRESULT			CompareSurfaces( LPSURFACE pSrcSurface, LPSURFACE pDestSurface, RECT *pRect = NULL );	// compare the two surfaces, before and after StretchRect
	void			DecodeTestNumber( UINT nTest );		// decode test number
	void			Dump();								// dump the input parameters

	// SDK Framework methods
	virtual HRESULT ConfirmDevice(CAPS*,DWORD,FORMAT);	// Called during device initialization, this code checks the device for some minimum set of capabilities
    virtual HRESULT OneTimeSceneInit();					// Called during initial app startup, this function performs all the permanent initialization
    virtual HRESULT InitDeviceObjects();				// Initialize device-dependent objects. This is the place to create mesh and texture objects.
    virtual HRESULT RestoreDeviceObjects();				// Restore device-memory objects and state after a device is created or resized.
    virtual HRESULT FrameMove();						// Called once per frame, the call is the entry point for animating the scene.
    virtual HRESULT Render();							// Called once per frame, the call is the entry point for 3d rendering. This function sets up render states, clears the viewport, and renders the scene.
    virtual HRESULT InvalidateDeviceObjects();			// Called when the device-dependent objects are about to be lost.
    virtual HRESULT DeleteDeviceObjects();				// Called when the app is exiting, or the device is being changed, this function deletes any device dependent objects.
    virtual HRESULT FinalCleanup();						// Called before the app exits, this function gives the app the chance to cleanup after itself.
};


class CStretchRectColorWriteEnable : public CStretchRectKeepStates
{
public:
	CStretchRectColorWriteEnable();
protected:
	virtual void	InitializeRenderState();
	virtual HRESULT ConfirmDevice(CAPS*,DWORD,FORMAT);	// Called during device initialization, this code checks the device for some minimum set of capabilities
};


class CStretchRectFillMode : public CStretchRectKeepStates
{
public:
	CStretchRectFillMode();
protected:
	virtual void	InitializeRenderState();
};


class CStretchRectShadeMode : public CStretchRectKeepStates
{
public:
	CStretchRectShadeMode();
protected:
	virtual void	InitializeRenderState();
};


class CStretchRectLastPixel : public CStretchRectKeepStates
{
public:
	CStretchRectLastPixel();
protected:
	virtual void	InitializeRenderState();
};


class CStretchRectSrcBlend : public CStretchRectKeepStates
{
public:
	CStretchRectSrcBlend();
protected:
	virtual void	InitializeRenderState();
};


class CStretchRectDitherEnable : public CStretchRectKeepStates
{
public:
	CStretchRectDitherEnable();
protected:
	virtual void	InitializeRenderState();
};


class CStretchRectSpecularEnable : public CStretchRectKeepStates
{
public:
	CStretchRectSpecularEnable();
protected:
	virtual void	InitializeRenderState();
};


class CStretchRectPixelFog : public CStretchRectKeepStates
{
public:
	CStretchRectPixelFog();
protected:
	virtual void	InitializeRenderState();
};


class CStretchRectVertexFog : public CStretchRectKeepStates
{
public:
	CStretchRectVertexFog();
protected:
	virtual void	InitializeRenderState();
};


class CStretchRectAmbient : public CStretchRectKeepStates
{
public:
	CStretchRectAmbient();
protected:
	virtual void	InitializeRenderState();
};

#endif // !defined(AFX_STRETCHRECTKEEPSTATES_H__F5654E0A_3560_40D4_8C8C_ABEC44E196C4__INCLUDED_)
