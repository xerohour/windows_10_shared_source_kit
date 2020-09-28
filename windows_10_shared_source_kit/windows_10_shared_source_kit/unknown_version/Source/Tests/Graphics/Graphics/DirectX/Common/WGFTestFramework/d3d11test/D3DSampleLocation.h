//////////////////////////////////////////////////////////////////////////
//  D3DSampleLocation.h
//  created:	2004/10/07
//
//  purpose: Helper class to determine sample locations
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11Test.h"
#include "WGFTestCore.h"
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

	// State of the immediate context will not be affected by this call
	static bool Search( CD3D11TestFramework*, ID3D11RenderTargetView*, ID3D11Texture2D*, std::vector<POINT>& );

	// create and deletes resources
	TEST_RESULT Setup( CD3D11TestFramework *pFramework, ID3D11RenderTargetView* pRTV, ID3D11Texture2D* pRTTex2D, POINT PixPos );
	TEST_RESULT Setup( CD3D11TestFramework *pFramework, ID3D11RenderTargetView* pRTV, ID3D11Texture2D* pRTTex2D, POINT PixPos, UINT forcedSampleCount );
	TEST_RESULT AddDepth( ID3D11Texture2D* pDSTex2D, ID3D11DepthStencilView *pDSView );
	void Cleanup();

	// utility 
	TEST_RESULT FindSamples( bool AllowOverlap = true, UINT srcSubResource = 0 );
	TEST_RESULT OrderSamples( UINT srcSubResource = 0 );
	TEST_RESULT TestPixelNonZero( BOOL &bResult, bool bResolve = false, UINT srcSubResource = 0 );
	void Update( POINT PixPos );
	
	// accessors
	POINT GetLoc( UINT i );
	ID3D11Texture2D *ResolveTex() { return m_pTex2DResolve; }
	UINT SampleCount()		 { return m_uSampleCount; }
	UINT ForcedSampleCount() { return m_uForcedSampleCount; }
	UINT EffectiveSampleCount() { return m_uEffectiveSampleCount; }

private:
	void ResetStates();
	void CreateTextures();
	void CreateShaders();
	void ExecuteDeferredContext();

	ID3D11Device1				*m_pDevice;
	ID3D11DeviceContext1		*m_pImmediateContext;
	ID3D11DeviceContext1		*m_pDeferredContext;
	CD3D11TestFramework         *m_pFramework;

	ID3D11Texture2D			    *m_pRTTex2D;
	ID3D11Texture2D			    *m_pRTTex2DLoad;
	ID3D11Texture2D			    *m_pTex2DCopy;
	ID3D11Texture2D			    *m_pTex2DResolve;
	ID3D11Texture2D   		    *m_pDSTex2D;
	
	ID3D11ShaderResourceView    *m_pSRView;
	ID3D11RenderTargetView	    *m_pRTView;
	ID3D11RenderTargetView	    *m_pRTViewLoad;
	ID3D11DepthStencilView	    *m_pDSView;
	
	ID3D11Buffer				*m_pConstBuffer;
	ID3D11Buffer				*m_pVertexBuffer;
	ID3D11InputLayout		    *m_pVertexLayout;

	ID3D11RasterizerState1	    *m_pRS;
	ID3D11BlendState			*m_pBS;
	ID3D11DepthStencilState	    *m_pDSS;
	ID3D11DepthStencilState		*m_pDSSDisabled;

	ID3D11VertexShader		    *m_pVS;
	ID3D11PixelShader		    *m_pPS;
	ID3D11PixelShader		    *m_pPSLoad;

	D3D11_BOX				    m_PixBox;

	POINT				        m_pSampleLocation[32];
	UINT				        m_uSampleCount;
	UINT						m_uForcedSampleCount;
	UINT						m_uEffectiveSampleCount;
	bool						m_bResolveSupport;
	bool						m_bMSLoadSupport;
};


//////////////////////////////////////////////////////////////////////////
// TRException
//
// Wraps exceptions and test results 
//////////////////////////////////////////////////////////////////////////
struct TRException
{
	TRException(tstring error, RESULT_CODE rc = RESULT_FAIL)
		: sError( error ),
		  tRes( rc )
	{}

	TRException(tstring error, HRESULT hr, RESULT_CODE rc = RESULT_FAIL)
		: sError( error ),
		  tRes( rc )
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