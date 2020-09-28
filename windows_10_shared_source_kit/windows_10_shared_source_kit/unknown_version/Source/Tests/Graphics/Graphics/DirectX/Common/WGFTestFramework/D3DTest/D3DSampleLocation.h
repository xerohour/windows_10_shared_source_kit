//////////////////////////////////////////////////////////////////////////
//  D3DSampleLocation.h
//  created:	2004/10/07
//
//  purpose: Helper class to determine sample locations
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "D3DTest.h"
#include "WGFTestCore.h"
#include "D3DMultiWrapped.h"
#include "D3DValueSets.h"
#include "D3DEnumsInfo.h"
#include "DXGIValueSets.h"

//////////////////////////////////////////////////////////////////////////
// SampleFinder
//
// Finds sample locations
// PREREQS: Requires a multisample texture.  
//		    The format needs to support multisample load or multisample resolve.
//			If the texture only supports multisample load, it must be bindable
//			as a shader resource.
//////////////////////////////////////////////////////////////////////////
class SampleFinder
{
public:
	SampleFinder();
	~SampleFinder();

	static bool Search( CD3DTestFramework*, CRenderTargetView*, CTexture2D*, std::vector<POINT>& );

	// create and deletes resources
	TEST_RESULT Setup( CD3DTestFramework *pFramework, CRenderTargetView* pRTV, CTexture2D* pRTTex2D, POINT PixPos );
	TEST_RESULT AddDepth( CTexture2D* pDSTex2D, CDepthStencilView *pDSView );
	void Cleanup();

	// utility 
	TEST_RESULT FindSamples( bool AllowOverlap = true, UINT srcSubResource = 0 );
	TEST_RESULT OrderSamples( UINT srcSubResource = 0 );
	TEST_RESULT TestPixelNonZero( BOOL &bResult, bool bResolve = false, UINT srcSubResource = 0 );
	void Update( POINT PixPos );
	
	// accessors
	POINT GetLoc( UINT i );
	CTexture2D *ResolveTex() { return m_pTex2DResolve; }
	UINT SampleCount()		 { return m_uSampleCount; }

private:
	void ResetStates();
	void CreateTextures();
	void CreateShaders();

	CDevice				*m_pDevice;
	CD3DTestFramework   *m_pFramework;

	CTexture2D			*m_pRTTex2D;
	CTexture2D			*m_pRTTex2DLoad;
	CTexture2D			*m_pTex2DCopy;
	CTexture2D			*m_pTex2DResolve;
	CTexture2D   		*m_pDSTex2D;
	
	CShaderResourceView *m_pSRView;
	CRenderTargetView	*m_pRTView;
	CRenderTargetView	*m_pRTViewLoad;
	CDepthStencilView	*m_pDSView;
	
	CBuffer				*m_pConstBuffer;
	CBuffer				*m_pVertexBuffer;
	CInputLayout		*m_pVertexLayout;

	CRasterizerState	*m_pRS;
	CBlendState			*m_pBS;
	CDepthStencilState	*m_pDSS;

	CVertexShader		*m_pVS;
	CPixelShader		*m_pPS;
	CPixelShader		*m_pPSLoad;

	D3D_BOX				m_PixBox;

	POINT				m_pSampleLocation[32];
	UINT				m_uSampleCount;
	
	// must support either multisample resolve or multisample load
	bool m_bResolveSupport;
	bool m_bMSLoadSupport;
};


//////////////////////////////////////////////////////////////////////////
// TRException
//
// Wraps exceptions and test results 
//////////////////////////////////////////////////////////////////////////
struct TRException
{
	TRException(tstring error, TEST_RESULT tr = RESULT_FAIL)
		: sError( error ),
		  tRes( tr )
	{}

	TRException(tstring error, HRESULT hr, TEST_RESULT tr = RESULT_FAIL )
		: sError( error ),
	 	  tRes( tr )
	{
		sError += tstring(" (") + D3DHResultToString(hr) + tstring(") ");
	}

	void Prepend( tstring str ) { sError = str + sError; }
	tstring GetError() { return sError + tstring( "\n" ); }
	TEST_RESULT GetResult() { return tRes; }

private:
	tstring sError;
	TEST_RESULT tRes;
};