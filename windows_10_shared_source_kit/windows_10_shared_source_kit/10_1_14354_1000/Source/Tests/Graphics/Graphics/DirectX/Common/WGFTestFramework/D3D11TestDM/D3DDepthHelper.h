//////////////////////////////////////////////////////////////////////////
//  D3DDepthHelper.h
//  created:	2006/10/03
//
//  purpose: Helper class to fill depth buffers
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11Test.h"
#include "WGFTestCore.h"
#include "DXGIValueSets.h"

//////////////////////////////////////////////////////////////////////////
// DepthHelper
//
// Fill depth buffer
// PREREQS: Requires a texture bindable as a depth/stencil target.
//			CD3D11TestFramework.
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
	TEST_RESULT Setup( CD3D11TestFramework *pFramework, ID3D11DepthStencilView* pDSV = NULL );
	void Cleanup();

	// utility
	// Note: pData layout should be pData[slices][height][width][samples]
	TEST_RESULT Fill( FLOAT *pData, 
					  UINT uWidth, 
					  UINT uHeight = 1, 
					  UINT uSampleCount = 1,
					  UINT uRowPitch = 0, 
					  UINT uSlicePitch = 0, 
					  ID3D11DepthStencilView* pDSV = NULL );

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
	

	ID3D11Device				*m_pDevice;
	ID3D11DeviceContext			*m_pDeviceContext;
	CD3D11TestFramework   *m_pFramework;
	ID3D11DepthStencilView   *m_pDSV;
	
	ID3D11Buffer				*m_pConstBuffer;
	ID3D11Buffer				*m_pVertexBuffer;
	ID3D11InputLayout		*m_pVertexLayout;

	ID3D11RasterizerState	*m_pRS;
	ID3D11DepthStencilState	*m_pDSS;

	ID3D11VertexShader		*m_pVS;
	ID3D11GeometryShader		*m_pGS;

	D3D11_VIEWPORT		m_VP;

	UINT m_uWidth, m_uHeight;
};


