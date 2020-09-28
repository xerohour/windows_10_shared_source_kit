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
// Multisample helper
//
// Fill depth buffer
// PREREQS: Requires a texture bindable as a render target and resource view.
//			CD3DTestFramework.
//			pData is a float array with size ArraySize*Width*Height*SampleCount*sizeof(float4)
//			
// Notes: 
//	-view format is required if the texture is typeless
//	-data needs to be of the form float pData[ArraySize][SampleCount][Height][Width]
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


class MSHelper
{
public:
	MSHelper() {};

	// writes to CTexture2D from float data
	static bool Write( CD3DTestFramework *pFramework, CTexture2D* pTex2DMS, FLOAT* pData, DXGI_FORMAT ViewFormat = DXGI_FORMAT_UNKNOWN );
	// reads from CTexture2D into float data
	static bool Read( CD3DTestFramework *pFramework, CTexture2D* pTex2DMS, FLOAT* pData, DXGI_FORMAT ViewFormat = DXGI_FORMAT_UNKNOWN );
};
