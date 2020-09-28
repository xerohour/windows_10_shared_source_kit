//////////////////////////////////////////////////////////////////////////
//  depth.h
//  created:	2006/11/10
//
//  purpose:    base class for all depth tests
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "d3d10_1.h"
#include "new_off.h"
#include "d3dx10.h"
#include "D3D11Test.h"
#include "new_on.h"
#include <WGFTestCore.h>
#include <D3DSampleLocation.h> // for TRException
#include "testapp.h"

#include "MinMaxRange.h"

extern CDepthStencilTestApp g_TestApp;

// default pixel shader outputs white.
enum PSType { DEFAULT, ODEPTH, NONE };
struct VERTEX { float x,y,z; };

class CDepthTest : public CD3D11Test
{
public:
	CDepthTest();
	virtual ~CDepthTest();

protected:
	void InitDepthTestParameters();

	// setup and cleanup for resources common to all depth tests
	void SetupCommon();
	void CleanupCommon();
	void SetupTestCaseCommon();
	void CleanupTestCaseCommon();

	virtual TEST_RESULT Setup() = 0;
	virtual TEST_RESULT SetupTestCase() = 0;
	virtual TEST_RESULT ExecuteTestCase() = 0;
	virtual void CleanupTestCase() = 0;
	virtual void Cleanup() = 0;

protected:
	//
	// test case parameters
	//
	
	// depth state desc - only some members are used as parameters
	D3D11_DEPTH_STENCIL_DESC		m_DSDesc;

	// depth texture desc - also used to fill out 1D desc
	D3D11_TEXTURE2D_DESC			m_DSTex2DDesc;

	// depth view desc 
	D3D11_DEPTH_STENCIL_VIEW_DESC	m_DSVDesc;
	// current mip slice - added to dsv union in setuptestcase
	UINT						m_uMipSlice;
	UINT						m_uArraySlice;

	bool						m_bODepth;	
	bool						m_bCube;
	PSType						m_PSType;

    D3D_RENDER_TARGET_OPTIONS m_origRenderTargetOptions;

	//
	// test case factors
	// 

	testfactor::RFactor m_rfFormat;
	testfactor::RFactor m_rfTex1D, m_rfTex1DArray;
	testfactor::RFactor m_rfTex2D, m_rfTex2DArray;
	testfactor::RFactor m_rfCube;
	testfactor::RFactor m_rfAll;
	testfactor::RFactor m_rfReduced;
	testfactor::RFactor m_rfReducedWithStencil; // same as m_rfReduced, but only contains formats that allow stencils


	// 
	//	d3dm setup
	//

	void SetupTextures();
	void SetupVB();
	void SetupShaders();

	void GetDepth(std::vector<float>&);
	void CopyDepth(void *pSrc, std::vector<float>& vDest, UINT uRowPitch = 0);

	void FillVB(const std::vector<float>&);
	void FillConstantBuffer(float depth = 0.5f);
	
	void CompileShader(HMODULE hModule, LPCSTR pResource, LPCSTR pFunctionName, LPCSTR pProfile, UINT uFlags, ID3D10Blob **ppShader);


	// depth textures
	ID3D11Resource			*m_pDSTex;
	ID3D11Resource			*m_pDSTexStaging;
	ID3D11Resource			*m_pRTTex;
	ID3D11DepthStencilView	*m_pDSV;
	ID3D11DepthStencilView	*m_pDSVWriteable;
	ID3D11RenderTargetView	*m_pRTV;
	
	// depth stencil state
	ID3D11DepthStencilState	*m_pDSS;

	// vertex buffer
	ID3D11Buffer			 *m_pVB;
	ID3D11InputLayout	 *m_pIL;

	// constant buffer
	ID3D11Buffer			 *m_pCB;

	// rasterizer 
	ID3D11RasterizerState *m_pRS;

	// shaders
	ID3D11VertexShader	 *m_pVS, *m_pVSODepth;
	ID3D11GeometryShader  *m_pGSTri, *m_pGSPoint;  
	ID3D11PixelShader	 *m_pPS, *m_pPSODepth;

	// subresource width and height
	UINT	m_uSubResWidth;
	UINT	m_uSubResHeight;
};

