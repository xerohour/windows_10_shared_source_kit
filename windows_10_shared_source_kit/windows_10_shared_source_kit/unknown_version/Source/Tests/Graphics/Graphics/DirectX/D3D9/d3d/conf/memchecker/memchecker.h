// MemChecker.h: interface for the CMemChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMCHECKER_H__5A664A69_55D7_4934_8B73_1A33134D98EA__INCLUDED_)
#define AFX_MEMCHECKER_H__5A664A69_55D7_4934_8B73_1A33134D98EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#include <vector>
#include <d3d8.h>
#include <CD3DX.h>
#include <d3dx8.h>
#include "CD3DTest.h"
#include "CShapes.h"
#include "CBuffers.h"
#include "CSurface.h"
#include "CTextures.h"
#include "CSwapChain.h"

using namespace std;

#define OBJECTS		50

struct FVFVERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float tu, tv;
};

#define FVFVERTEXHANDLE	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)


//
// CMemChecker - base class
//

class CMemChecker : public CD3DTest
{
public:
	CMemChecker();
	virtual ~CMemChecker();

	UINT TestInitialize(); // Init the testcase
	bool TestTerminate(); // cleanup
	bool ExecuteTest(UINT nTest); // Execute the testcase

protected:
	bool DisplayFrame(); // override present call
	bool ProcessFrame(); // process the frame

	virtual void onActivity() = 0; // the activity each test performs
	virtual void onDestroy() {}; // release the pointers

	// helper methods
	void Check_HRFail( HRESULT hResult, const char* sMethod ); // if hResult is not D3D_OK, throws an exception
	void DecodeTestNumber(UINT nTest); // decode test number

	TCHAR					m_szMemLeak[256]; // memleak message
	int						m_iCalls; // number of times to perform the activity
	DEVICEDESC*				m_pdeviceCaps; // device caps
	UINT					m_Retry;	// number of times to retry the test if a leak is detected

	// vectors used to decompose the test number
	vector<POINT>			m_vecSize;
	vector<DWORD>			m_vecPool;
	vector<DWORD>			m_vecUsage;
	vector<FORMAT>			m_vecFormat;
	UINT					m_indexSize;
	UINT					m_indexPool;
	UINT					m_indexUsage;
	UINT					m_indexFormat;
};


//
// CCreateTexture - calls CreateTexture
//

class CCreateTexture : public CMemChecker
{
public:
	CCreateTexture();

protected:
	virtual void onActivity();

	LPTEXTURE m_pTexture;
};


//
// CCreateCubeTexture - calls CreateCubeTexture
//

class CCreateCubeTexture : public CMemChecker
{
public:
	CCreateCubeTexture();

protected:
	virtual void onActivity();

	LPCUBETEXTURE m_pCubeTexture;
};


//
// CCreateVolumeTexture - calls CreateVolumeTexture
//

class CCreateVolumeTexture : public CMemChecker
{
public:
	CCreateVolumeTexture();

protected:
	virtual void onActivity();

	LPVOLUMETEXTURE m_pVolumeTexture;
};


//
// CCreateVertexBuffer - calls CreateVertexBuffer
//

class CCreateVertexBuffer : public CMemChecker
{
public:
	CCreateVertexBuffer();

protected:
	virtual void onActivity();

	LPVERTEXBUFFER m_pVertexBuffer;
};


//
// CCreateIndexBuffer - calls CreateIndexBuffer
//

class CCreateIndexBuffer : public CMemChecker
{
public:
	CCreateIndexBuffer();

protected:
	virtual void onActivity();

	LPINDEXBUFFER m_pIndexBuffer;
};


//
// CCreateAdditionalSwapChain - calls CreateAdditionalSwapChain
//

class CCreateAdditionalSwapChain : public CMemChecker
{
public:
	CCreateAdditionalSwapChain();

protected:
	virtual void onActivity();

	LPSWAPCHAIN m_pSwapChain;
};


//
// CCreateOffscreenPlainSurface - calls CreateOffscreenPlainSurface
//

class CCreateOffscreenPlainSurface : public CMemChecker
{
public:
	CCreateOffscreenPlainSurface();

protected:
	virtual void onActivity();

	LPSURFACE m_pOffscreenPlainSurface;
};


//
// CCreateRenderTarget - calls CreateRenderTarget
//

class CCreateRenderTarget : public CMemChecker
{
public:
	CCreateRenderTarget();

protected:
	virtual void onActivity();

	LPSURFACE m_pRenderTarget;
};


//
// CCreateVertexShader - calls CreateVertexShader
//

class CCreateVertexShader : public CMemChecker
{
public:
	CCreateVertexShader();
	UINT TestInitialize(); // Init the testcase

protected:
	virtual void onActivity();

	TCHAR m_strVertexShader1[1024];
	CVertexShader *m_pVertexShader;
};


//
// CCreatePixelShader - calls CreatePixelShader
//

class CCreatePixelShader : public CMemChecker
{
public:
	CCreatePixelShader();
	UINT TestInitialize(); // Init the testcase

protected:
	virtual void onActivity();

	TCHAR m_strPixelShader1[1024];
	CPShader *m_pPixelShader;
	DWORD m_PixelShader;
};


//
// CRenderTest - renders a scene from buffers
//

class CRenderTest : public CMemChecker
{
public:
	CRenderTest();
	UINT TestInitialize(); // Init the testcase
	bool TestTerminate(); // cleanup

	bool SetDefaultMatrices();
	bool SetDefaultMaterials();
	bool SetDefaultRenderStates();
	bool SetDefaultLights();

protected:
	virtual void onDestroy(); // release the pointers
	virtual void onActivity();
	void onDraw(); // draw the scene

	// methods to intialize/draw/use the scene
	void CreateBuffers(); // create vertex and index buffers
	void FillBuffers(); // fill the buffers
	void GetBackBuffer(); // get the back buffer
	
	// surface variables
	LPSURFACE				m_psBackBuffer;			// the backbuffer surface
	SURFACEDESC				m_sdBackBuffer;			// surface description for backbuffer

	MATERIAL				m_sMaterial;
	LIGHT					m_sLight;
	LPVERTEXBUFFER			m_pVBs[OBJECTS];		// all vertex buffers
	LPINDEXBUFFER			m_pIBs[OBJECTS];		// all vertex buffers
	LPTEXTURE				m_pTextures[OBJECTS];	// all textures
	CShapes					*m_pShapes[OBJECTS];	// all the shapes
	float					m_vSpeed[OBJECTS];		// the speed (and direction) for all the objects
};

#endif // !defined(AFX_MEMCHECKER_H__5A664A69_55D7_4934_8B73_1A33134D98EA__INCLUDED_)
