#include "AsyncConf.h" 

#include <mmsystem.h>
#pragma comment(lib, "winmm")

/*
[from hw spec]
Here's a comprehensive list of operations that honor predication:

Draw 
DrawIndexed 
DrawInstanced 
DrawIndexedInstanced 
DrawOpaque 
ClearRenderTarget 
ClearDepthStencil 
CopyRegions 
CopyResource 
UpdateSubresourceUP

gathered from ref code:
draw*
	draw
	drawindexed
	drawinstanced
	drawindexedinstanced
	drawauto
copy*
	copyresource
	copysubresourceregion
updatesubresource
resolve
convert (blt)
clear*
	clearrendertarget
	cleardepthstencil
*/

void CPredicateConfTest::InitTestParameters()
{
	m_PredDesc.MiscFlags = 0;

	testfactor::RFactor rF1, rF2, rF3, rF4, rF5;

	CUserValueSet<D3D11_QUERY> *pPredNameValues = new CUserValueSet< D3D11_QUERY >();
	pPredNameValues->AddValue( D3D11_QUERY_SO_OVERFLOW_PREDICATE );
	pPredNameValues->AddValue( D3D11_QUERY_OCCLUSION_PREDICATE );
	pPredNameValues->AddValue( D3D11_QUERY_OCCLUSION_PREDICATE );

	CUserValueSet<bool> *pHintValues = new CUserValueSet< bool >();
	pHintValues->AddValue( false );
	pHintValues->AddValue( false );
	pHintValues->AddValue( true );

	rF1 = AddParameter<D3D11_QUERY>( _T( "Predicate_Name" ), &m_PredDesc.Query, pPredNameValues );
	rF2 = AddParameter<bool>( _T( "Hint" ), &m_bHint, pHintValues );
	rF3 = AddParameter<bool>( _T( "Predicate_Value" ), &m_bPredValue, BoolValueSet() );
//	rF4 = AddParameter<bool>( _T( "Overlap_Queries" ), &m_bOverlapQueries, BoolValueSet() );
	rF5 = AddParameter<bool>( _T( "Do_GpuJob" ), &m_bDoGpuJob, BoolValueSet() );

	SetRootTestFactor( (rF1 % rF2) * rF3 * /*rF4 **/ rF5 );
}

TEST_RESULT CPredicateConfTest::ExecuteTestCase()
{
	HRESULT hr = S_OK, hr2 = S_OK;
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Predicate *pPredicate = NULL;
	TEST_COLOR expectedColor;
	TEST_COLOR clearColor;

	if (g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0)
	{
		// skip all Predicate cases on FL9
		return RESULT_SKIP;
	}

	bool expectedResult;
	if (m_PredDesc.Query == D3D11_QUERY_OCCLUSION_PREDICATE)
		expectedResult = m_bDoGpuJob?FALSE:TRUE;
	else if (m_PredDesc.Query == D3D11_QUERY_SO_OVERFLOW_PREDICATE)
		expectedResult = m_bDoGpuJob?TRUE:FALSE; // will overflow when no buffer is present
	else
	{
		assert(!"Invalid Test Case");
	}

	// clear out any previous state
	GetEffectiveContext()->SetPredication(NULL, TRUE);
	clearColor.r = 0;
	clearColor.g = 0;
	clearColor.b = 0;
	clearColor.a = 0;
	{
		FLOAT d3dClearColor[4] = {(FLOAT)clearColor.r, (FLOAT)clearColor.g, (FLOAT)clearColor.b, (FLOAT)clearColor.a};
		GetEffectiveContext()->ClearRenderTargetView(m_pRTView, d3dClearColor);
	}
	GetEffectiveContext()->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

	// set up our values
	m_PredDesc.MiscFlags = 0;
	if (m_bHint)
		m_PredDesc.MiscFlags |= D3D11_QUERY_MISC_PREDICATEHINT;
	if ( FAILED(hr = GetDevice()->CreatePredicate(&m_PredDesc, &pPredicate)) )
	{
		WriteToLog( _T("Failed to create Predicate. hr=S_FALSE\n") );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	SetupGpuJob(); // modifies the effective context.

	// maybe render something that will "occlude" the 2nd render
	if (m_bDoGpuJob)
		hr = FAILED(hr)?hr : GpuBoundJob(false);

	// compute the predicate result
	GetEffectiveContext()->Begin(pPredicate);
	hr = FAILED(hr)?hr : GpuBoundJob(false); // modifies the effective context.
	GetEffectiveContext()->End(pPredicate);

	if (FAILED(hr))
	{
		WriteToLog( _T("Failed to execute GPU job. hr=0x%x\n"), (long)hr );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	// Call derived class to initialize for GPU work
	// this must not be done earlier, becuase this can change state, and clear the render target
	tRes = InitializeForGpuJob(); // modifies the effective context
	if (tRes != RESULT_PASS)
	{
		goto testDone;
	}

	// use the predication with a GPU operation
	GetEffectiveContext()->SetPredication(pPredicate, m_bPredValue?TRUE:FALSE);
	DoPredicatedGpuJob(0);

	// use the inverse predicate with a GPU operation
	GetEffectiveContext()->SetPredication(pPredicate, m_bPredValue?FALSE:TRUE);
	DoPredicatedGpuJob(1);

	GetEffectiveContext()->SetPredication(NULL, TRUE);

	if( FAILED(hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	GetImmediateContext()->Flush();

	// ensure predicate is processed
	if (!m_bHint)
	{
		// Verify that the correct Gpu operation executed
		UINT ExpectedOperationIdx = (m_bPredValue ^ expectedResult) ? 0 : 1;

		tRes = VerifyResults(ExpectedOperationIdx);
		if (tRes != RESULT_PASS)
		{
			goto testDone;
		}

		BOOL value;
		hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pPredicate, &value, sizeof(value), D3D11_ASYNC_GETDATA_DONOTFLUSH, 1000 );
		if (hr == S_FALSE)
		{
			WriteToLog( _T("Predicate.GetData took longer than expected. hr=S_FALSE\n") );
			// try again with flush - if it works
			hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pPredicate, &value, sizeof(value), 0 );
			if (hr == S_FALSE)
			{
				WriteToLog( _T("Predicate.GetData appears to be non-terminating (%d seconds elapsed). hr=S_FALSE\n"), GetFramework()->GetQueryTimeout() / 1000 );
				tRes = RESULT_FAIL;
				goto testDone;
			}
		}

		if (hr != S_OK)
		{
			WriteToLog( _T("GetData did not return S_OK. hr=0x%x\n"), (long)hr );
			tRes = RESULT_FAIL;
			goto testDone;
		}

		if (value != (BOOL)expectedResult)
		{
			WriteToLog( _T("GetData value was %d, expected %d.\n"), (long)value, (long)expectedResult );
			tRes = RESULT_FAIL;
			goto testDone;
		}
	}

testDone:
	// cleanup
	CleanupFromGpuJob();

	SAFE_RELEASE( pPredicate );

	return tRes;
}

TEST_RESULT CPredicateConfTest::ReadTexture2DColor(ID3D11Texture2D* pTex, UINT uSubresource, void* pOut)
{
	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Texture2D* pStaging = NULL;

	// Create staging buffer
	D3D11_TEXTURE2D_DESC texDesc;
	pTex->GetDesc(&texDesc);

	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.BindFlags = 0;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	texDesc.MiscFlags = 0;

	if ( FAILED(hr = GetDevice()->CreateTexture2D(&texDesc, NULL, &pStaging)) )
	{
		WriteToLog( _T("Failed to create staging buffer. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

	// Copy to staging buffer
	GetEffectiveContext()->CopyResource(pStaging, pTex);

	if( FAILED(hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		tRes = RESULT_FAIL;
		goto done;
	}

	// Map the staging buffer and copy 1st pixel of uSubresource to pOut
	D3D11_MAPPED_SUBRESOURCE texMap;
	if ( FAILED(hr = GetImmediateContext()->Map(pStaging, uSubresource, D3D11_MAP_READ, 0, &texMap)) )
	{
		WriteToLog( _T("Failed to map staging buffer. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

	const UINT bytesPerElement = GetBitsPerElement( texDesc.Format ) / 8;
	memcpy(pOut, texMap.pData, bytesPerElement);

	GetImmediateContext()->Unmap(pStaging, uSubresource);

done:
	SAFE_RELEASE( pStaging );

	return tRes;
}


///////////////////////////////////////////////////////////////////////////////
CClearRTVPredicateConfTest::CClearRTVPredicateConfTest()
{
	m_pRT = NULL;
	m_pRTV = NULL;
}

CClearRTVPredicateConfTest::~CClearRTVPredicateConfTest()
{
	assert(NULL == m_pRT);
	assert(NULL == m_pRTV);
}

TEST_RESULT CClearRTVPredicateConfTest::InitializeForGpuJob()
{
	assert(NULL == m_pRT);
	assert(NULL == m_pRTV);

	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;

	// Create render target and view
	D3D11_TEXTURE2D_DESC texDesc; 
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width  = 1;
	texDesc.Height = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

	if ( FAILED(hr = GetDevice()->CreateTexture2D(&texDesc, NULL, &m_pRT)) )
	{
		WriteToLog( _T("Failed to create render target. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

	if ( FAILED(hr = GetDevice()->CreateRenderTargetView(m_pRT, NULL, &m_pRTV)) )
	{
		WriteToLog( _T("Failed to create render target view. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

done:
	return tRes;
}

void CClearRTVPredicateConfTest::CleanupFromGpuJob()
{
	SAFE_RELEASE( m_pRT );
	SAFE_RELEASE( m_pRTV );
}

void CClearRTVPredicateConfTest::DoPredicatedGpuJob(UINT uIdx)
{
	assert(m_pRTV);

	FLOAT_COLOR clearColor = GetClearColor(uIdx);

	// The caller must execute the effective context!!!!
	GetEffectiveContext()->ClearRenderTargetView(m_pRTV, clearColor.comp);
}

TEST_RESULT CClearRTVPredicateConfTest::VerifyResults(UINT uIdx)
{
	TEST_RESULT tRes = RESULT_PASS;

	FLOAT_COLOR expectedColor = GetClearColor(uIdx);
	FLOAT_COLOR actualColor;

	tRes = ReadTexture2DColor(m_pRT, 0, &actualColor);
	if (tRes != RESULT_PASS)
	{
		goto done;
	}

	if (0 != memcmp(&expectedColor, &actualColor, sizeof(FLOAT_COLOR)))
	{
		WriteToLog( _T(" Render target color did not match\n") );
		WriteToLog( _T(" Expected color {%f, %f, %f, %f}.\n"), expectedColor.r, expectedColor.g, expectedColor.b, expectedColor.a );
		WriteToLog( _T(" Actual color {%f, %f, %f, %f}.\n"), actualColor.r, actualColor.g, actualColor.b, actualColor.a );
		tRes = RESULT_FAIL;
	}

done:
	return tRes;
}

FLOAT_COLOR CClearRTVPredicateConfTest::GetClearColor(UINT uIdx)
{
	FLOAT_COLOR colors[2] = 
	{
		{0.0f, 0.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
	};

	assert(uIdx < _countof(colors));

	return colors[uIdx];
}


///////////////////////////////////////////////////////////////////////////////
CClearDSVPredicateConfTest::CClearDSVPredicateConfTest()
{
	m_pDepthBuffer = NULL;
	m_pDSV = NULL;
}

CClearDSVPredicateConfTest::~CClearDSVPredicateConfTest()
{
	assert(NULL == m_pDepthBuffer);
	assert(NULL == m_pDSV);
}

TEST_RESULT CClearDSVPredicateConfTest::InitializeForGpuJob()
{
	assert(NULL == m_pDepthBuffer);
	assert(NULL == m_pDSV);

	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;

	// Create depth buffer and view
	D3D11_TEXTURE2D_DESC texDesc; 
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width  = 1;
	texDesc.Height = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if ( FAILED(hr = GetDevice()->CreateTexture2D(&texDesc, NULL, &m_pDepthBuffer)) )
	{
		WriteToLog( _T("Failed to create depth/stencil buffer. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

	if ( FAILED(hr = GetDevice()->CreateDepthStencilView(m_pDepthBuffer, NULL, &m_pDSV)) )
	{
		WriteToLog( _T("Failed to create depth/stencil view. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

done:
	return tRes;
}

void CClearDSVPredicateConfTest::CleanupFromGpuJob()
{
	SAFE_RELEASE( m_pDepthBuffer );
	SAFE_RELEASE( m_pDSV );
}

void CClearDSVPredicateConfTest::DoPredicatedGpuJob(UINT uIdx)
{
	assert(m_pDSV);

	const float clearDepth = GetClearDepth(uIdx);

	// The caller must execute the effective context!!!
	GetEffectiveContext()->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH, clearDepth, 0);
}

TEST_RESULT CClearDSVPredicateConfTest::VerifyResults(UINT uIdx)
{
	TEST_RESULT tRes = RESULT_PASS;

	const float expectedDepth = GetClearDepth(uIdx);
	float actualDepth;

	tRes = ReadTexture2DColor(m_pDepthBuffer, 0, &actualDepth);
	if (tRes != RESULT_PASS)
	{
		goto done;
	}

	if (expectedDepth != actualDepth)
	{
		WriteToLog( _T(" Depth value did not match\n") );
		WriteToLog( _T(" Expected value %f.\n"), expectedDepth );
		WriteToLog( _T(" Actual value %f.\n"), actualDepth );
		tRes = RESULT_FAIL;
	}

done:
	return tRes;
}

float CClearDSVPredicateConfTest::GetClearDepth(UINT uIdx)
{
	assert(uIdx < 2);
	return (float)uIdx;
}

///////////////////////////////////////////////////////////////////////////////
CCopyResourcePredicateConfTest::CCopyResourcePredicateConfTest()
{
	m_pSrc0 = NULL;
	m_pSrc1 = NULL;
	m_pDst = NULL;
}

CCopyResourcePredicateConfTest::~CCopyResourcePredicateConfTest()
{
	assert(NULL == m_pSrc0);
	assert(NULL == m_pSrc1);
	assert(NULL == m_pDst);
}

TEST_RESULT CCopyResourcePredicateConfTest::InitializeForGpuJob()
{
	assert(NULL == m_pSrc0);
	assert(NULL == m_pSrc1);
	assert(NULL == m_pDst);

	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;

	// Create 3 texture 2Ds
	// 1. Red
	// 2. Blue
	// 3. Black

	const DWORD initialColors[3]  = { GetColor(0), GetColor(1), 0 };
	ID3D11Texture2D** ppTex2D[3]  = { &m_pSrc0, &m_pSrc1, &m_pDst };

	for (int k = 0; k < 3; ++k)
	{           
		D3D11_TEXTURE2D_DESC texDesc; 
		ZeroMemory(&texDesc, sizeof(texDesc));

		texDesc.Width  = 1;
		texDesc.Height = 1;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;

		// USAGE_DEFAULT get initial data
		D3D11_SUBRESOURCE_DATA srd = { &initialColors[k], sizeof(DWORD), 0 };

		ID3D11Texture2D** ppDst = ppTex2D[k];
		assert(ppDst);
		assert(!(*ppDst));

		if ( FAILED(hr = GetDevice()->CreateTexture2D(&texDesc, &srd, ppDst)) )
		{
			WriteToLog( _T("Failed to create texture 2d. hr=%s\n"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto done;
		}
	}

done:
	return tRes;
}

void CCopyResourcePredicateConfTest::CleanupFromGpuJob()
{
	SAFE_RELEASE( m_pSrc0 );
	SAFE_RELEASE( m_pSrc1 );
	SAFE_RELEASE( m_pDst );
}

void CCopyResourcePredicateConfTest::DoPredicatedGpuJob(UINT uIdx)
{
	assert(m_pSrc0);
	assert(m_pSrc1);
	assert(m_pDst);

	ID3D11Resource* pDst = m_pDst;
	ID3D11Resource* pSrc = uIdx ? m_pSrc1 : m_pSrc0;

	// The caller must execute the effective context!!!
	GetEffectiveContext()->CopyResource(pDst, pSrc);
}

TEST_RESULT CCopyResourcePredicateConfTest::VerifyResults(UINT uIdx)
{
	TEST_RESULT tRes = RESULT_PASS;

	const DWORD expectedColor = GetColor(uIdx);
	DWORD actualColor = 0;

	tRes = ReadTexture2DColor(m_pDst, 0, &actualColor);
	if (tRes != RESULT_PASS)
	{
		goto done;
	}

	if (expectedColor != actualColor)
	{
		WriteToLog( _T(" Color value did not match\n") );
		WriteToLog( _T(" Expected value 0x%x.\n"), expectedColor );
		WriteToLog( _T(" Actual value 0x%x.\n"), actualColor );
		tRes = RESULT_FAIL;
	}

done:
	return tRes;
}

DWORD CCopyResourcePredicateConfTest::GetColor(UINT uIdx)
{
	const DWORD colors[2] = { 0x000000FF, 0x00FF0000 };

	assert(uIdx < _countof(colors));

	return colors[uIdx];
}

///////////////////////////////////////////////////////////////////////////////
void CCopySubresourceRegionPredicateConfTest::DoPredicatedGpuJob(UINT uIdx)
{
	assert(m_pSrc0);
	assert(m_pSrc1);
	assert(m_pDst);

	ID3D11Resource* pDst = m_pDst;
	ID3D11Resource* pSrc = uIdx ? m_pSrc1 : m_pSrc0;

	// The caller must execute the effective context!!!
	GetEffectiveContext()->CopySubresourceRegion(pDst, 0, 0, 0, 0, pSrc, 0, NULL);
}

///////////////////////////////////////////////////////////////////////////////
CUpdateSubresourcePredicateConfTest::CUpdateSubresourcePredicateConfTest()
{
	m_pDst = NULL;
}

CUpdateSubresourcePredicateConfTest::~CUpdateSubresourcePredicateConfTest()
{
	assert(NULL == m_pDst);
}

TEST_RESULT CUpdateSubresourcePredicateConfTest::InitializeForGpuJob()
{
	assert(NULL == m_pDst);

	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;

	// Create a texture 2D
	D3D11_TEXTURE2D_DESC texDesc; 
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width  = 1;
	texDesc.Height = 1;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;

	if ( FAILED(hr = GetDevice()->CreateTexture2D(&texDesc, NULL, &m_pDst)) )
	{
		WriteToLog( _T("Failed to create texture 2d. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

done:
	return tRes;
}

void CUpdateSubresourcePredicateConfTest::CleanupFromGpuJob()
{
	SAFE_RELEASE( m_pDst );
}

void CUpdateSubresourcePredicateConfTest::DoPredicatedGpuJob(UINT uIdx)
{
	assert(m_pDst);

	const DWORD color = GetColor(uIdx);
	
	// The caller must execute the effective context.
	GetEffectiveContext()->UpdateSubresource(m_pDst, 0, NULL, &color, sizeof(DWORD), sizeof(DWORD));
}

TEST_RESULT CUpdateSubresourcePredicateConfTest::VerifyResults(UINT uIdx)
{
	TEST_RESULT tRes = RESULT_PASS;

	const DWORD expectedColor = GetColor(uIdx);
	DWORD actualColor = 0;

	tRes = ReadTexture2DColor(m_pDst, 0, &actualColor);
	if (tRes != RESULT_PASS)
	{
		goto done;
	}

	if (expectedColor != actualColor)
	{
		WriteToLog( _T(" Color value did not match\n") );
		WriteToLog( _T(" Expected value 0x%x.\n"), expectedColor );
		WriteToLog( _T(" Actual value 0x%x.\n"), actualColor );
		tRes = RESULT_FAIL;
	}

done:
	return tRes;
}

DWORD CUpdateSubresourcePredicateConfTest::GetColor(UINT uIdx)
{
	const DWORD colors[2] = { 0x000000FF, 0x00FF0000 };

	assert(uIdx < _countof(colors));

	return colors[uIdx];
}


///////////////////////////////////////////////////////////////////////////////
CResolveSubresourcePredicateConfTest::CResolveSubresourcePredicateConfTest()
{
	m_pSrc0 = NULL;
	m_pSrc1 = NULL;
	m_pDst = NULL;
}

CResolveSubresourcePredicateConfTest::~CResolveSubresourcePredicateConfTest()
{
	assert(NULL == m_pSrc0);
	assert(NULL == m_pSrc1);
	assert(NULL == m_pDst);
}

TEST_RESULT CResolveSubresourcePredicateConfTest::InitializeForGpuJob()
{
	assert(NULL == m_pSrc0);
	assert(NULL == m_pSrc1);
	assert(NULL == m_pDst);

	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;

	// Create 3 texture 2Ds and clear them to initial colors
	// 1. Multisampled,   red
	// 2. Multisampled,   blue
	// 3. Single sampled, black

	const UINT  sampleCount[3]    = { 4, 4, 1 };
	const DWORD initialColors[3]  = { GetColor(0), GetColor(1), 0 };
	ID3D11Texture2D** ppTex2D[3]  = { &m_pSrc0, &m_pSrc1, &m_pDst };

	for (int k = 0; k < 3; ++k)
	{           
		D3D11_TEXTURE2D_DESC texDesc; 
		ZeroMemory(&texDesc, sizeof(texDesc));

		texDesc.Width  = 1;
		texDesc.Height = 1;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = sampleCount[k];
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		texDesc.CPUAccessFlags = 0;

		ID3D11Texture2D** ppDst = ppTex2D[k];
		assert(ppDst);
		assert(!(*ppDst));

		if ( FAILED(hr = GetDevice()->CreateTexture2D(&texDesc, NULL, ppDst)) )
		{
			WriteToLog( _T("Failed to create texture 2d. hr=%s\n"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_SKIP;  // MSAA is optional
			goto done;
		}

		ID3D11RenderTargetView* pRTV = NULL;
		if ( FAILED(hr = GetDevice()->CreateRenderTargetView(*ppDst, NULL, &pRTV)) )
		{
			WriteToLog( _T("Failed to create texture 2d. hr=%s\n"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto done;
		}

		FLOAT_COLOR clearColor;
		if ( !CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, DXGI_FORMAT_R8G8B8A8_UNORM, &clearColor, 1, 1, (void*)&(initialColors[k]) ) )
		{
			WriteToLog( _T("CFormatHelper::ConvertPixels() failed") );
			tRes = RESULT_FAIL;
			goto done;
		}
		
		// The caller must execute the effective context.
		GetEffectiveContext()->ClearRenderTargetView(pRTV, clearColor.comp);

		SAFE_RELEASE( pRTV );
	}

done:
	return tRes;
}

void CResolveSubresourcePredicateConfTest::CleanupFromGpuJob()
{
	SAFE_RELEASE( m_pSrc0 );
	SAFE_RELEASE( m_pSrc1 );
	SAFE_RELEASE( m_pDst );
}

void CResolveSubresourcePredicateConfTest::DoPredicatedGpuJob(UINT uIdx)
{
	assert(m_pSrc0);
	assert(m_pSrc1);
	assert(m_pDst);

	ID3D11Resource* pDst = m_pDst;
	ID3D11Resource* pSrc = uIdx ? m_pSrc1 : m_pSrc0;

	// The caller must execute the effective context.
	GetEffectiveContext()->ResolveSubresource(pDst, 0, pSrc, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
}

TEST_RESULT CResolveSubresourcePredicateConfTest::VerifyResults(UINT uIdx)
{
	TEST_RESULT tRes = RESULT_PASS;

	const DWORD expectedColor = GetColor(uIdx);
	DWORD actualColor = 0;

	tRes = ReadTexture2DColor(m_pDst, 0, &actualColor);
	if (tRes != RESULT_PASS)
	{
		goto done;
	}

	if (expectedColor != actualColor)
	{
		WriteToLog( _T(" Color value did not match\n") );
		WriteToLog( _T(" Expected value 0x%x.\n"), expectedColor );
		WriteToLog( _T(" Actual value 0x%x.\n"), actualColor );
		tRes = RESULT_FAIL;
	}

done:
	return tRes;
}

DWORD CResolveSubresourcePredicateConfTest::GetColor(UINT uIdx)
{
	const DWORD colors[2] = { 0x000000FF, 0x00FF0000 };

	assert(uIdx < _countof(colors));

	return colors[uIdx];
}

///////////////////////////////////////////////////////////////////////////////
CGenerateMipsPredicateConfTest::CGenerateMipsPredicateConfTest()
{
	m_pTex = NULL;
	m_pSRV = NULL;
}

CGenerateMipsPredicateConfTest::~CGenerateMipsPredicateConfTest()
{
	assert(m_pTex == NULL);
	assert(m_pSRV == NULL);
}

TEST_RESULT CGenerateMipsPredicateConfTest::InitializeForGpuJob()
{
	assert(m_pTex == NULL);
	assert(m_pSRV == NULL);

	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;

	// Create a texture with 2 mip levels
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width = 2;
	texDesc.Height = 2;
	texDesc.MipLevels = 2;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if ( FAILED(hr = GetDevice()->CreateTexture2D(&texDesc, NULL, &m_pTex)) )
	{
		WriteToLog( _T("Failed to create texture 2d. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

	if (FAILED(hr = GetDevice()->CreateShaderResourceView(m_pTex, NULL, &m_pSRV)) )
	{
		WriteToLog( _T("Failed to create shader resource view. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto done;
	}

done:
	return tRes;
}

void CGenerateMipsPredicateConfTest::CleanupFromGpuJob()
{
	SAFE_RELEASE( m_pTex );
	SAFE_RELEASE( m_pSRV );
}

void CGenerateMipsPredicateConfTest::DoPredicatedGpuJob(UINT uIdx)
{
	// Fill top mip level with unpredicated UpdateSubresource
	const DWORD color = GetColor(uIdx);

	// Top level of mip has 4 pixels
	const DWORD color4[4] = { color, color, color, color };

	ID3D11Predicate* pCurrentPredicate = NULL;
	BOOL predicateValue = FALSE;

	// The caller must execute the effective context.
	GetEffectiveContext()->GetPredication(&pCurrentPredicate, &predicateValue);
	GetEffectiveContext()->SetPredication(NULL, TRUE);

	GetEffectiveContext()->UpdateSubresource(m_pTex, 0, NULL, color4, sizeof(DWORD) * 2, 0);

	GetEffectiveContext()->SetPredication(pCurrentPredicate, predicateValue);
	SAFE_RELEASE(pCurrentPredicate);

	// Predicated generate mips
	GetEffectiveContext()->GenerateMips(m_pSRV);
}

TEST_RESULT CGenerateMipsPredicateConfTest::VerifyResults(UINT uIdx)
{
	TEST_RESULT tRes = RESULT_PASS;

	const DWORD expectedColor = GetColor(uIdx);

	DWORD actualColor;

	tRes = ReadTexture2DColor(m_pTex, 1, &actualColor);
	if (tRes != RESULT_PASS)
	{
		goto done;
	}

	if (expectedColor != actualColor)
	{
		WriteToLog( _T(" Color value did not match\n") );
		WriteToLog( _T(" Expected value 0x%x.\n"), expectedColor );
		WriteToLog( _T(" Actual value 0x%x.\n"), actualColor );
		tRes = RESULT_FAIL;
	}

done:
	return tRes;
}

DWORD CGenerateMipsPredicateConfTest::GetColor(UINT uIdx)
{
	const DWORD colors[2] = { 0x000000FF, 0x00FF0000 };

	assert(uIdx < _countof(colors));

	return colors[uIdx];
}

///////////////////////////////////////////////////////////////////////////////
CDrawBasePredicateConfTest::CDrawBasePredicateConfTest()
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer  = NULL;
}

CDrawBasePredicateConfTest::~CDrawBasePredicateConfTest()
{
	assert(NULL == m_pVertexBuffer);
	assert(NULL == m_pIndexBuffer);
}

TEST_RESULT CDrawBasePredicateConfTest::InitializeForGpuJob()
{
	// Clear render target & depth buffer
	FLOAT d3dClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	// The caller must execute the effective context!!!
	GetEffectiveContext()->ClearRenderTargetView(m_pRTView, d3dClearColor);
	GetEffectiveContext()->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

	assert(NULL == m_pVertexBuffer);
	assert(NULL == m_pIndexBuffer);

	HRESULT hr = S_OK;
	TEST_RESULT tRes = RESULT_PASS;

	// Create vertex and index buffers
	{
		D3D11_BUFFER_DESC VBDesc;
		ZeroMemory(&VBDesc, sizeof(VBDesc));

		VBDesc.ByteWidth = sizeof(TVERTEX) * VERTEX_COUNT;
		VBDesc.Usage     = D3D11_USAGE_DEFAULT;
		VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	   
		if( FAILED(hr = GetDevice()->CreateBuffer(&VBDesc, NULL, &m_pVertexBuffer)) )
		{
			WriteToLog( _T("Failed to vertex buffer. hr=%s\n"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto done;
		}

		// Fill the vertex buffer
		// the 1st 4 vertices are color0
		// the next 4 vertices are color1
		const DWORD color0 = GetColor(0);
		const DWORD color1 = GetColor(1);

		assert(8 == VERTEX_COUNT);
		TVERTEX vertices[VERTEX_COUNT] = 
		{
			{ {0,							0,							0}, color0 },
			{ {(float)m_nPrimitiveSize,	    0,							0}, color0 },
			{ {(float)m_nPrimitiveSize,	    (float)m_nPrimitiveSize,	0}, color0 },
			{ {0,							(float)m_nPrimitiveSize,	0}, color0 },

			{ {0,							0,							0}, color1 },
			{ {(float)m_nPrimitiveSize,	    0,							0}, color1 },
			{ {(float)m_nPrimitiveSize,	    (float)m_nPrimitiveSize,	0}, color1 },
			{ {0,							(float)m_nPrimitiveSize,	0}, color1 },
		};

		FillVertexBuffer(m_pVertexBuffer, vertices);
	}

	{
		D3D11_BUFFER_DESC IBDesc;
		ZeroMemory(&IBDesc, sizeof(IBDesc));

		IBDesc.ByteWidth = sizeof(WORD) * INDEX_COUNT;
		IBDesc.Usage     = D3D11_USAGE_DEFAULT;
		IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		assert(INDEX_COUNT == 4);
		const WORD indices[INDEX_COUNT] = { 0, 1, 2, 3 }; // IB just contains identity

		D3D11_SUBRESOURCE_DATA srd = { indices, 0, 0 };
	   
		if( FAILED(hr = GetDevice()->CreateBuffer(&IBDesc, &srd, &m_pIndexBuffer)) )
		{
			WriteToLog( _T("Failed to index buffer. hr=%s\n"), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto done;
		}
	}
	
done:
	return tRes;
}

void CDrawBasePredicateConfTest::CleanupFromGpuJob()
{
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pIndexBuffer );
}

void CDrawBasePredicateConfTest::DoPredicatedGpuJob(UINT uIdx)
{
	assert(m_pVertexBuffer);
	assert(m_pIndexBuffer);

	// Set state
	UINT stride = sizeof(TVERTEX);
	UINT offset = uIdx * stride * INDEX_COUNT;
	
	// The caller must execute the effective context!!!
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	GetEffectiveContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Draw
	Draw();
}

TEST_RESULT CDrawBasePredicateConfTest::VerifyResults(UINT uIdx)
{
	TEST_RESULT tRes = RESULT_PASS;

	const DWORD expectedColor = GetColor(uIdx);
	
	FLOAT_COLOR actualColorF32;

	tRes = ReadTexture2DColor(m_pTextureRT, 0, &actualColorF32);
	if (tRes != RESULT_PASS)
	{
		goto done;
	}

	DWORD actualColor;

	if ( !CFormatHelper::ConvertPixels(CONV_TO_FORMAT, DXGI_FORMAT_R8G8B8A8_UNORM, &actualColorF32, 1, 1, &actualColor) )
	{
		WriteToLog( _T("CFormatHelper::ConvertPixels() failed") );
		tRes = RESULT_FAIL;
	}

	if (expectedColor != actualColor)
	{
		WriteToLog( _T(" Color value did not match\n") );
		WriteToLog( _T(" Expected value 0x%x.\n"), expectedColor );
		WriteToLog( _T(" Actual value 0x%x.\n"), actualColor );
		tRes = RESULT_FAIL;
	}

done:
	return tRes;
}

DWORD CDrawBasePredicateConfTest::GetColor(UINT uIdx)
{
	const DWORD colors[2] = { 0x000000FF, 0x00FF0000 };

	assert(uIdx < _countof(colors));

	return colors[uIdx];
}


void CDrawBasePredicateConfTest::FillVertexBuffer(ID3D11Buffer* pBuffer, const TVERTEX* pVertices)
{
	// The caller must execute the effective context!!!
	GetEffectiveContext()->UpdateSubresource(pBuffer, 0, NULL, pVertices, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
void CDrawPredicateConfTest::Draw()
{
	// The caller must execute the effective context!!!
	GetEffectiveContext()->Draw(INDEX_COUNT, 0);
}

///////////////////////////////////////////////////////////////////////////////
void CDrawInstancedPredicateConfTest::Draw()
{
	// The caller must execute the effective context!!!
	GetEffectiveContext()->DrawInstanced(INDEX_COUNT, 1, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
void CDrawIndexedPredicateConfTest::Draw()
{
	// The caller must execute the effective context!!!
	GetEffectiveContext()->DrawIndexed(INDEX_COUNT, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
void CDrawIndexedInstancedPredicateConfTest::Draw()
{
	// The caller must execute the effective context!!!
	GetEffectiveContext()->DrawIndexedInstanced(INDEX_COUNT, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
void CDrawAutoPredicateConfTest::FillVertexBuffer(ID3D11Buffer* pBuffer, const TVERTEX* pVertices)
{
	//
	// Fill the buffer with vertex data
	//
	CDrawBasePredicateConfTest::FillVertexBuffer(pBuffer, pVertices);

	//
	// Bind m_pVertexBuffer as an SO target
	// to get the proper buffer filled size into the buffer
	//
	UINT Offset = VERTEX_COUNT * sizeof(TVERTEX);
	// The caller must execute the effective context!!!
	GetEffectiveContext()->SOSetTargets(1, &m_pVertexBuffer, &Offset);
	GetEffectiveContext()->SOSetTargets(0, NULL, NULL);
}

void CDrawAutoPredicateConfTest::Draw()
{
	// The caller must execute the effective context!!!
	GetEffectiveContext()->DrawAuto();
}
