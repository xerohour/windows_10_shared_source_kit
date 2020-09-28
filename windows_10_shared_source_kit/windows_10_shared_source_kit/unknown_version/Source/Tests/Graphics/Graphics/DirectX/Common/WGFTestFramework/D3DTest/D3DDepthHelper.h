//////////////////////////////////////////////////////////////////////////
//  D3DDepthHelper.h
//  created:	2006/10/03
//
//  purpose: Helper class to fill depth buffers
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "D3DTest.h"
#include "WGFTestCore.h"
#include "D3DMultiWrapped.h"
#include "D3DValueSets.h"
#include "D3DEnumsInfo.h"
#include "DXGIValueSets.h"

//////////////////////////////////////////////////////////////////////////
// DepthHelper
//
// Fill depth buffer
// PREREQS: Requires a texture bindable as a depth/stencil target.
//			CD3DTestFramework.
//			pData is size ArraySize*Width*Height*SampleCount.
//			
// Example use: 
//		//Texture 2d
//		DepthHelper helper;
//		helper.Setup(GetFramework(), pDSView);
//		helper.Fill(pData, uWidth, uHeight);
//
//		//Texture 2dms
//		DepthHelper helper;
//		helper.Setup(GetFramework(), pDSView);
//		helper.Fill(pData, uWidth, uHeight, uSampleCount);
//
//		//Texture 1d
//		DepthHelper helper;
//		helper.Setup(GetFramework(), pDSView);
//		helper.Fill(pData, uWidth);
//
//		//Multiple textures
//		DepthHelper helper;
//		helper.Setup(GetFramework());
//		for (UINT i = 0; i < uDSCount; i++)
//			helper.Fill(pData, uWidth, uHeight, 1, 0, 0, pDSView[i]);
//
//	
//////////////////////////////////////////////////////////////////////////
class DepthHelper
{
public:
	//
	DepthHelper();
	~DepthHelper();

	// create and deletes resources
	TEST_RESULT Setup( CD3DTestFramework *pFramework, CDepthStencilView* pDSV = NULL );
	void Cleanup();

	// utility
	// Note: pData layout should be pData[slices][height][width][samples]
	TEST_RESULT Fill( FLOAT *pData, 
					  UINT uWidth, 
					  UINT uHeight = 1, 
					  UINT uSampleCount = 1,
					  UINT uRowPitch = 0, 
					  UINT uSlicePitch = 0, 
					  CDepthStencilView* pDSV = NULL );

private:
	void CreateShaders();
	void Refresh();
	void FillCB( UINT uSlice );
	void FillVB( FLOAT *pData, 
			     UINT uSample, 
			     UINT uSlice, 
			     UINT uSampleCount,
			     UINT uRowPitch, 
			     UINT uSlicePitch );

	// 
	void ResolveDimension(UINT&);

	// sets only the state used
	void SetState();
	void ResetState();
	

	CDevice				*m_pDevice;
	CD3DTestFramework   *m_pFramework;
	CDepthStencilView   *m_pDSV;
	
	CBuffer				*m_pConstBuffer;
	CBuffer				*m_pVertexBuffer;
	CInputLayout		*m_pVertexLayout;

	CRasterizerState	*m_pRS;
	CDepthStencilState	*m_pDSS;

	CVertexShader		*m_pVS;
	CGeometryShader		*m_pGS;

	D3D_VIEWPORT		m_VP;

	UINT m_uWidth, m_uHeight;
};


