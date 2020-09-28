// StretchRectDepthStencil.h: interface for the CStretchRectDepthStencil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRETCHRECTDEPTHSTENCIL_H__1D92B2F6_9DA6_4081_ACB6_9217C3B6BB66__INCLUDED_)
#define AFX_STRETCHRECTDEPTHSTENCIL_H__1D92B2F6_9DA6_4081_ACB6_9217C3B6BB66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d9.h>
#include <d3dx9.h>
#include "CD3DTest.h"
#include "CShapes.h"
#include "CBuffers.h"
#include "CTextures.h"


typedef struct _D3DVERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
} D3DVERTEX;

#define D3DVERTEXHANDLE	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

#define		WIDTH	4
#define		HEIGHT	3
#define		DEPTH	8

/************************************************************
 * Test specification										*
 ************************************************************/

class CStretchRectDepthStencil : public CD3DTest
{
public:
	CStretchRectDepthStencil();
	virtual ~CStretchRectDepthStencil();

	// framework virtual methods
	virtual void CommandLineHelp();					// command line help for the test
	virtual UINT TestInitialize();					// Init the testcase
	virtual bool TestTerminate();					// cleanup
	virtual bool ExecuteTest(UINT nTest);			// Execute the testcase
	virtual void OnDeviceLost();					// Called after DEVICELOST.  Test should release all D3D objects created in POOL_DEFAULT
    virtual void OnReset();							// Called after Reset() succeeds.  Test should re-create all D3D objects released in OnDeviceLost and refresh all data within existing objects.
	
protected:
	FORMAT			*m_pCurrentDepthStencilFormat;			// current depth stencil format
	int				m_nFrames;								// number of frames to run the test for each ZBuffer
	CShapes			m_shapes[WIDTH*HEIGHT*DEPTH+1];			// all the shapes
	LPVERTEXBUFFER	m_pVertexBuffer[WIDTH*HEIGHT*DEPTH+1];	// vertex buffer
	LPINDEXBUFFER	m_pIndexBuffer[WIDTH*HEIGHT*DEPTH+1];	// index buffer
	LPTEXTURE		m_pTexture;								// texture to be applied to all boxes
	LPSURFACE		m_pOriginalDepthStencil, m_pCopyDepthStencil;
	LPSURFACE		m_pBackBuffer;					// backbuffer surface
	SURFACEDESC		m_sdBackBuffer;					// backbuffer description
	LPSURFACE		m_pSurfaceBeforeStretchRect, m_pSurfaceAfterStretchRect;	// hold the backbuffer contents before and after StretchRect
	bool			m_bSave;

	// helper methods
	void			Check_HRFail( HRESULT hResult, const char* sMethod );	// if hResult is not D3D_OK, throws an exception
	HRESULT			DuplicateDepthStencil();			// duplicate the depth stencil format (the contents are NOT copied though)
	HRESULT			RenderScene(bool bStencilRendering);// render the main scene
	HRESULT			CompareSurfaces();					// compare the two surfaces, before and after StretchRect

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


#endif // !defined(AFX_STRETCHRECTDEPTHSTENCIL_H__1D92B2F6_9DA6_4081_ACB6_9217C3B6BB66__INCLUDED_)
