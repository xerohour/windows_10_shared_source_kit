//////////////////////////////////////////////////////////////////////////
//  D3DSampleLocation.cpp
//  created:	2004/10/13
//
//  purpose: Implementation of SampleFinder
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "D3DSampleLocation.h"
#include "Shell.h"
#include "D3D10Wrapped.h"
#include "D3DMultiWrapped.h"
#include "D3D10WrappedGlobals.h"
#include "D3D10Test.h"

#include <queue>

struct VERTEX 
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 tex;
};

bool SampleFinder::Search( CD3DTestFramework* pFramework, CRenderTargetView* pRTV, CTexture2D* pTex, std::vector<POINT> &vPos )
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		HRESULT hr;
   		// get ID3D10Device interface
		ID3D10Device *pD3D10Device = NULL;
		pFramework->GetDevice()->GetSubDevice( 0 )->GetIUnknown()->QueryInterface( __uuidof(ID3D10Device), (void**) &pD3D10Device );

		// preserve state
		ID3D10StateBlock	*pStateBlock(NULL);
		D3D10_STATE_BLOCK_MASK mask;
		if( FAILED( hr = D3DWrappedStateBlockMaskEnableAll( &mask ) ) )
			throw TRException("D3DStateBlockMaskEnableAll", hr);
		if( FAILED( hr = D3DWrappedCreateStateBlock( pD3D10Device, &mask, &pStateBlock ) ) )
			throw TRException("D3DCreateStateBlock", hr);
		pD3D10Device->Release();
		if( FAILED( hr = pStateBlock->Capture() ) )
			throw TRException("pStateBlock->Capture()", hr);


		TEST_RESULT tRes;
		// Find sample locations
		SampleFinder finder;
		POINT pixpos = {0,0};
		if (FAILED( finder.Setup( pFramework, pRTV, pTex, pixpos ) ) )
			throw TRException("SampleFinder failed Setup.");
		if (FAILED( finder.FindSamples() ) )
			throw TRException("SampleFinder failed FindSamples.");
		if (FAILED( finder.OrderSamples() ) )
			throw TRException("SampleFinder failed OrderSamples.");
		finder.Cleanup();

		vPos.clear();
		UINT uCount = finder.SampleCount();
		for (UINT i = 0; i < uCount; ++i)
			vPos.push_back(finder.GetLoc(i));

		// restore state
		if( FAILED( hr = pStateBlock->Apply() ) )
			throw TRException("pStateBlock->Apply()", hr);
		SAFE_RELEASE( pStateBlock );

	}
	catch (TRException& exc)
	{
		exc.Prepend("[SampleFinder::Search()] ");
		pFramework->WriteToLog( exc.GetError().c_str() );
		tRes = exc.GetResult();
	}

	return tRes == RESULT_PASS;
}

SampleFinder::SampleFinder()
: m_pDevice(NULL),
  m_pFramework(NULL),
  m_pRTTex2D(NULL),
  m_pRTTex2DLoad(NULL),
  m_pTex2DCopy(NULL),
  m_pTex2DResolve(NULL),
  m_pDSTex2D(NULL),
  m_pSRView(NULL),
  m_pRTView(NULL),
  m_pRTViewLoad(NULL),
  m_pDSView(NULL),
  m_pConstBuffer(NULL),
  m_pVertexBuffer(NULL),
  m_pVertexLayout(NULL),
  m_pRS(NULL),
  m_pBS(NULL),
  m_pDSS(NULL),
  m_pVS(NULL),
  m_pPS(NULL),
  m_pPSLoad(NULL),
  m_uSampleCount(0),
  m_bResolveSupport(false),
  m_bMSLoadSupport(false)
{
	memset(&m_pSampleLocation, 0, sizeof(m_pSampleLocation));
}

SampleFinder::~SampleFinder()
{
	//Cleanup();
}

POINT SampleFinder::GetLoc(UINT i)
{
	//assert(i < m_uSampleCount);
	return m_pSampleLocation[i];
}

TEST_RESULT SampleFinder::Setup(CD3DTestFramework *pFramework, CRenderTargetView* pRTV, CTexture2D* pRTTex2D, POINT PixPos)
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		HRESULT hr;
		Cleanup();

		// init
		m_pDevice = pFramework->GetDevice();
		m_pFramework = pFramework;
		m_pRTTex2D = pRTTex2D;
		m_pRTView = pRTV;

		m_PixBox.left = PixPos.x;
		m_PixBox.right = PixPos.x + 1;
		m_PixBox.top = PixPos.y;
		m_PixBox.bottom = PixPos.y + 1;
		m_PixBox.front = 0;
		m_PixBox.back = 1;		

		// render target description
		D3D_TEXTURE2D_DESC rtdesc;
		m_pRTTex2D->GetDesc( &rtdesc );

		//
		// Setup the default viewport
		//
		D3D_VIEWPORT vp;
		vp.Height = rtdesc.Height;
		vp.Width = rtdesc.Width;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pDevice->RSSetViewports( 1, &vp );

		//
		// Set Rasterizer state
		//
		D3D_RASTERIZER_DESC rastDesc;
		rastDesc.CullMode = D3D_CULL_NONE;
		rastDesc.FrontCounterClockwise = false;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.MultisampleEnable = true;
		rastDesc.AntialiasedLineEnable = false;
		rastDesc.FillMode = D3D_FILL_SOLID;
		if( FAILED( hr = m_pDevice->CreateRasterizerState( &rastDesc, &m_pRS ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.", hr);

		//
		// Create the constant buffer
		//
		D3D_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = 512;
		bufdesc.Usage = D3D_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( hr = m_pDevice->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.", hr);
	
		//
		// Create vertex buffer
		//
		D3D_BUFFER_DESC bufferDesc =
		{
			100 * sizeof( VERTEX ),
			D3D_USAGE_DYNAMIC,
			D3D_BIND_VERTEX_BUFFER,
			D3D_CPU_ACCESS_WRITE,
			0
		};
		if( FAILED( hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pVertexBuffer ) ) )
			throw TRException("CreateBuffer for m_pVertexBuffer Failed.", hr);

		UINT strides[] = { sizeof( VERTEX ) };
		UINT offsets[] = { 0 };

		//
		// setup const data
		//
		float *bufData;
		if( FAILED( hr = m_pConstBuffer->Map( D3D_MAP_WRITE_DISCARD, NULL, (void **) &bufData ) ) )
			throw TRException("Map on m_pConstBuffer Failed", hr);
		// Inverse viewport scale.
		bufData[2] = 2.f / rtdesc.Width;
		bufData[3] = -2.f / rtdesc.Height;
		// Prescaled inverse viewport translation.
		bufData[0] = -1.f;
		bufData[1] = 1.f;
		//pix pos
		bufData[4] = (float)m_PixBox.left;
		bufData[5] = (float)m_PixBox.top;
		m_pConstBuffer->Unmap();

		// Check support for multisample load and resolve
		m_bResolveSupport = false;
		m_bMSLoadSupport = false;
		D3D_RENDER_TARGET_VIEW_DESC rtvDesc;
		m_pRTView->GetDesc(&rtvDesc);
		UINT32 uSupport;
		m_pDevice->CheckFormatSupport(rtvDesc.Format, &uSupport);
		if (pFramework->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0)
		{
			if (uSupport & D3D_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET)
				m_bResolveSupport = true;

			if (uSupport & D3D10_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)
				m_bResolveSupport = true;
			else
			{
				tstring errStr = "Format '" + ToString(rtvDesc.Format) + "' does not support resolve.";
				throw TRException(errStr.c_str());
			}
		}
		else
		{
			if (uSupport & D3D10_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)
				m_bResolveSupport = true;
			if (uSupport & D3D10_FORMAT_SUPPORT_MULTISAMPLE_LOAD)
			{
				D3D_TEXTURE2D_DESC tex2dDesc;
				m_pRTTex2D->GetDesc(&tex2dDesc);

				// Added for 10L9 tests running 10.0 feature level
				if(tex2dDesc.BindFlags & D3D_BIND_SHADER_RESOURCE)
				{
					m_bMSLoadSupport = true;
				}
			}
		}
		if (!m_bResolveSupport && !m_bMSLoadSupport)
				throw TRException("Format does not support resolve or multisample load");

		CreateTextures();
		CreateShaders();	
	}
	catch (TRException& exc)
	{
		exc.Prepend("[SampleFinder::Setup()] ");
		m_pFramework->WriteToLog( exc.GetError().c_str() );
		tRes = exc.GetResult();
	}

	return tRes;
}

void SampleFinder::Update( POINT PixPos )
{
	HRESULT hr;
	m_PixBox.left = PixPos.x;
	m_PixBox.right = PixPos.x + 1;
	m_PixBox.top = PixPos.y;
	m_PixBox.bottom = PixPos.y + 1;
	m_PixBox.front = 0;
	m_PixBox.back = 1;	

	// render target description
	D3D_TEXTURE2D_DESC rtdesc;
	m_pRTTex2D->GetDesc( &rtdesc );

	//
	// setup const data
	//
	float *bufData;
	if( FAILED( hr = m_pConstBuffer->Map( D3D_MAP_WRITE_DISCARD, NULL, (void **) &bufData ) ) )
		throw TRException("Map on m_pConstBuffer Failed", hr);
	// Inverse viewport scale.
	bufData[2] = 2.f / rtdesc.Width;
	bufData[3] = -2.f / rtdesc.Height;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.f;
	bufData[1] = 1.f;
	//pix pos
	bufData[4] = (float)m_PixBox.left;
	bufData[5] = (float)m_PixBox.top;
	m_pConstBuffer->Unmap();
}

void SampleFinder::CreateTextures()
{
	HRESULT hr;
	//
	// Setup RenderTarget
	//
	// render target description
	D3D_TEXTURE2D_DESC rtdesc;
	m_pRTTex2D->GetDesc( &rtdesc );

	D3D_TEXTURE2D_DESC tex2ddesc;
	D3D_RENDER_TARGET_VIEW_DESC rtvDesc;
	m_pRTView->GetDesc( &rtvDesc );	

	tex2ddesc.ArraySize = 1;
	tex2ddesc.BindFlags = 0;	
	tex2ddesc.CPUAccessFlags = D3D_CPU_ACCESS_READ;
	tex2ddesc.Format = rtvDesc.Format;
	tex2ddesc.Height = rtdesc.Height;
	tex2ddesc.Width = rtdesc.Width;
	tex2ddesc.MiscFlags = 0;
	tex2ddesc.MipLevels = 1;
	tex2ddesc.SampleDesc.Count = 1;
	tex2ddesc.SampleDesc.Quality = 0;
	tex2ddesc.Usage = D3D_USAGE_STAGING;
	if ( FAILED(hr = m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pTex2DCopy )) )
		throw TRException("CreateTexture2D failed for m_pRTTexture2DCopy.", hr);

	tex2ddesc.CPUAccessFlags = 0;
	tex2ddesc.Usage = D3D_USAGE_DEFAULT;
	// for 10L9, the resource with D3D_BIND_SHADER_RESOURCE can not be copied from
	tex2ddesc.BindFlags = D3D_BIND_RENDER_TARGET;
	if ( FAILED(hr = m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pTex2DResolve )) )
		throw TRException("CreateTexture2D failed for m_pRTTexture2DResolve.", hr);

	tex2ddesc.BindFlags = D3D_BIND_RENDER_TARGET;
	if ( FAILED(hr = m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pRTTex2DLoad )) )
		throw TRException("CreateTexture2D failed for m_pRTTexture2DResult.", hr);

	// render target view
	D3D_RENDER_TARGET_VIEW_DESC rtviewdesc;
	rtviewdesc.ViewDimension = D3D_RTV_DIMENSION_TEXTURE2D;
	rtviewdesc.Texture2D.MipSlice = 0;
	rtviewdesc.Format = rtvDesc.Format;
	if ( FAILED( hr = m_pDevice->CreateRenderTargetView( m_pRTTex2DLoad, &rtviewdesc, &m_pRTViewLoad ) ) )
		throw TRException("CreateRenderTargetView failed for m_pRTTexture2D and m_pRTView.", hr);

	// if the caller is a 10.0 and above test, and wants to use MSLoad, its rendertarget resource has to
	// created with D3D_BIND_SHADER_RESOURCE, in order for this resource to be used as shader resource here
	//
	// if the caller is a 10L9 test, it can not use MSLoad. 
	if (m_bMSLoadSupport)
	{
		D3D_SHADER_RESOURCE_VIEW_DESC svDesc;
		svDesc.Format = rtvDesc.Format;
		svDesc.Texture2D.MipLevels = 1;
		svDesc.Texture2D.MostDetailedMip = 0;
		svDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
		if( FAILED( hr = m_pDevice->CreateShaderResourceView( m_pRTTex2D, &svDesc, &m_pSRView ) ) )
			throw TRException("CreateShaderResourceView failed for m_pSRView.", hr); 
	}
		
}

void SampleFinder::CreateShaders()
{
	HRESULT hr;
	CBlob *pVSBuf = NULL;
	CBlob *pPSBuf = NULL;
	CBlob *pMSPSBuf = NULL; 
	CBlob *pErrorBuf = NULL;

	try
	{
	//
	//	Declare shaders
	//
	char szVS[] = 
		"struct VSIn																\n"
		"{																			\n"
		"	float2 pos : pos;														\n"
		"	float2 tex : tex;														\n"
		"};																			\n"
		"struct VSOut																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	float2 tex : tex;														\n"
		"};																			\n"
		"cbuffer cb0 : register(b0)													\n"
		"{																			\n"
		"	float4 inverseViewport;													\n"
		"	float alpha;															\n"
		"};																			\n"
		"VSOut main(VSIn input)														\n"
		"{																			\n"
		"	VSOut output;															\n"

		"	output.pos = float4( 0, 0, 0, 1.0 );									\n"
		"	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;	\n"
		"	output.tex = input.tex;													\n"

		"	return output;															\n"
		"}";

	char szPS[] = 
		"struct PSIn																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	float2 tex : tex;														\n"
		"};																			\n"
		"struct PSOut																\n"
		"{																			\n"
		"	%s Color : SV_Target;													\n"
		"};																			\n"
		"cbuffer cb0 : register(b0)													\n"
		"{																			\n"
		"	float4 inverseViewport;													\n"
		"	float alpha;															\n"
		"};																			\n"
		"PSOut main(PSIn input)														\n"
		"{																			\n"
		"	PSOut output;															\n"
		"   output.Color = %s( 1, 1, 1, 1 );										\n"
		"	return output;															\n"
		"}";	
 

	char szMSPS[] =
		"Texture2DMS<%s, %d> MyTex;													\n"
		"struct PSIn																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	float2 tex : tex;														\n"
		"};																			\n"
		"struct PSOut																\n"
		"{																			\n"
		"	%s Color : SV_Target;													\n"
		"};																			\n"
		"cbuffer cb0 : register(b0)													\n"
		"{																			\n"
		"	float4 inverseViewport;													\n"
		"	float2 pixel;															\n"
		"};																			\n"
		"PSOut main(PSIn input)														\n"
		"{																			\n"
		"	%s x = %s(0,0,0,0);														\n"
		"   int width, height, count;												\n"
		"	MyTex.GetDimensions(width, height, count);								\n"
		"   for (int i = 0; i < %d; i++)											\n"
		"   {																		\n"
		"		if (MyTex.Load(int3(pixel.x,pixel.y,0), i).x > 0)					\n"
		"			x = %s(1,1,1,1);												\n"
		"   }																		\n"
		"	PSOut output;															\n"	
		"	output.Color = x;														\n"
		"	return output;															\n"
		"}";

	char szFloat4[] = "float4";
	char szUint4[] = "uint4";
	char szInt4[] = "int4";

	// get description for format
	D3D_TEXTURE2D_DESC rtdesc;
	m_pRTTex2D->GetDesc(&rtdesc);
	D3D_RENDER_TARGET_VIEW_DESC rtvDesc;
	m_pRTView->GetDesc( &rtvDesc );	
	DWORD flags = GetFlags( rtvDesc.Format );

	// Compile and create all shader
	if ( FAILED(hr = m_pFramework->CompileShaderFromMemory( szVS, sizeof(szVS), "main", "vs_4_0", 0, &pVSBuf, &pErrorBuf )) )
		throw TRException( tstring("Failed to compile VS. ") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : ""), hr );
	if ( FAILED(hr = m_pDevice->CreateVertexShader( pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVS )) )
		throw TRException("Failed to create VS.", hr);

	// create buffers for constructed shader text
	const UINT MAX_STR = 4000;
	char szShader[ MAX_STR ];
	
	// depending on format, use int or float output
	char *szType = szFloat4;
	if (flags & FF_UINT)
		szType = szUint4;
	else if (flags & FF_SINT)
		szType = szInt4;
	
	// construct PS, requires output type info
	_snprintf( szShader, MAX_STR-1, szPS, szType, szType);
	szShader[MAX_STR-1] = 0;
	if ( FAILED(hr = m_pFramework->CompileShaderFromMemory( szShader, strlen(szShader), "main", "ps_4_0", D3D_SHADER_DEBUG, &pPSBuf, &pErrorBuf )) )
		throw TRException( tstring("Failed to compile PS. ") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : ""), hr );
	if ( FAILED(hr = m_pDevice->CreatePixelShader( pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), &m_pPS )) )
		throw TRException("Failed to create PS.", hr);

	// combine MSPS, requires output type info and sample count
	if (m_bMSLoadSupport)
	{
		UINT uCount = rtdesc.SampleDesc.Count;
		_snprintf( szShader, MAX_STR-1, szMSPS, szType, uCount, szType, szType, szType, uCount, szType );
		szShader[MAX_STR-1] = 0;
		if ( FAILED(hr = m_pFramework->CompileShaderFromMemory( szShader, strlen(szShader), "main", "ps_4_0", 0, &pMSPSBuf, &pErrorBuf )) )
			throw TRException( tstring("Failed to compile MSPS. ") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : ""), hr );
		else if ( FAILED(hr = m_pDevice->CreatePixelShader( pMSPSBuf->GetBufferPointer(), pMSPSBuf->GetBufferSize(), &m_pPSLoad )) )
			throw TRException("Failed to create MSPS.", hr);
	}

	// Define our vertex data layout
	const D3D_INPUT_ELEMENT_DESC layout[] =
	{
		{ "pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D_INPUT_PER_VERTEX_DATA, 0 },
		{ "tex", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D_INPUT_PER_VERTEX_DATA, 0 },
	};
	if(	FAILED(	hr = m_pDevice->CreateInputLayout( layout, 2, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVertexLayout ) ) )
		throw TRException("CD3DTestFramework::UpdateSampleLocations - CreateInputLayout for m_pVertexLayout Failed.", hr);
	
	}
	catch (TRException& exc)
	{
		// cleanup
		SAFE_RELEASE( pVSBuf );
		SAFE_RELEASE( pPSBuf );
		SAFE_RELEASE( pMSPSBuf );
		SAFE_RELEASE( pErrorBuf );
		throw exc;
	}
	// cleanup
	SAFE_RELEASE( pVSBuf );
	SAFE_RELEASE( pPSBuf );
	SAFE_RELEASE( pMSPSBuf );
	SAFE_RELEASE( pErrorBuf );
}

TEST_RESULT SampleFinder::AddDepth(CTexture2D* pDSTex2D, CDepthStencilView *pDSView)
{
	HRESULT hr;
	assert(pDSTex2D);
	assert(pDSView);
	assert(m_pDevice);

	m_pDSTex2D = pDSTex2D;
	m_pDSView = pDSView;

	D3D_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	if( FAILED( hr = m_pDevice->CreateDepthStencilState( &dsDesc, &m_pDSS ) ) )
	{
		m_pFramework->WriteToLogF("[SampleFinder::AddDepth] CreateDepthStencilState() failed. hr = %s", D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void SampleFinder::ResetStates()
{
	// set state
	UINT strides[] = { sizeof( VERTEX ) };
	UINT offsets[] = { 0 };
	m_pDevice->IASetInputLayout( m_pVertexLayout );
	m_pDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );
	m_pDevice->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	m_pDevice->RSSetState( m_pRS );
	m_pDevice->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDevice->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDevice->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
	m_pDevice->OMSetDepthStencilState( m_pDSS, 0 );
	m_pDevice->OMSetRenderTargets(1, &m_pRTView, m_pDSView );

	// set shaders
	m_pDevice->VSSetShader( m_pVS );
	m_pDevice->GSSetShader( NULL );
	m_pDevice->PSSetShader( m_pPS );
}


TEST_RESULT SampleFinder::TestPixelNonZero(BOOL &bResult, bool bResolve, UINT srcSubResource)
{
	HRESULT hr = E_UNEXPECTED;
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		
		D3D_RENDER_TARGET_VIEW_DESC rtvDesc;
		m_pRTView->GetDesc(&rtvDesc);
		D3D_TEXTURE2D_DESC rtDesc;
		m_pRTTex2D->GetDesc( &rtDesc );

		if ( !m_bMSLoadSupport )
		{
			bResolve = true;
			if ( !m_bResolveSupport && rtDesc.SampleDesc.Count > 1)
				throw TRException("The RTV format supports neither MS Load nor MS resolve. ");
		}

		if ( !bResolve )
		{	
			ResetStates();

			// set shaders
			m_pDevice->VSSetShader( m_pVS );
			m_pDevice->GSSetShader( NULL );
			m_pDevice->PSSetShader( m_pPSLoad );	

			// fill vertex buffer
			VERTEX *pDestVerts;
			if( FAILED( hr = m_pVertexBuffer->Map( D3D_MAP_WRITE_DISCARD, NULL, (void **) &pDestVerts ) ) )
				throw TRException("m_pVertexBuffer->Map() failed.", hr);
			
			float x = (float)m_PixBox.left;
			float y = (float)m_PixBox.top;
			pDestVerts[0].pos = D3DXVECTOR2(x, y);
			pDestVerts[1].pos = D3DXVECTOR2(x, y+1);
			pDestVerts[2].pos = D3DXVECTOR2(x+1, y);
			pDestVerts[3].pos = D3DXVECTOR2(x+1, y+1);
			m_pVertexBuffer->Unmap();

			// multisample load requires another draw
			FLOAT ClearColor[4] = { 0.0, 0.0, 0.0, 0.0 };
			m_pDevice->OMSetRenderTargets( 1, &m_pRTViewLoad, NULL );
			m_pDevice->ClearRenderTargetView( m_pRTViewLoad, ClearColor );
			m_pDevice->PSSetShaderResources( 0, 1, &m_pSRView );
			m_pDevice->Draw(4, 0);
			//GetDevice()->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, &box );

			// set shader back
			CShaderResourceView	*pSRVNull[] = { NULL, NULL, NULL, NULL };
			m_pDevice->PSSetShaderResources( 0, 1, pSRVNull );
			m_pDevice->PSSetShader( m_pPS );
			m_pDevice->GSSetShader( NULL );
			m_pDevice->OMSetRenderTargets(1, &m_pRTView, NULL );
			m_pDevice->CopySubresourceRegion( m_pTex2DResolve, 0, 0, 0, 0, m_pRTTex2DLoad, 0, 0 );
			m_pDevice->CopySubresourceRegion( m_pTex2DCopy, 0, 0, 0, 0, m_pRTTex2DLoad, 0, &m_PixBox );
			
		}
		else
		{
			m_pDevice->ResolveSubresource( m_pTex2DResolve, 0, m_pRTTex2D, srcSubResource, rtvDesc.Format );
			m_pDevice->CopySubresourceRegion( m_pTex2DCopy, 0, 0, 0, 0, m_pTex2DResolve, 0, &m_PixBox );
		}

		D3D_MAPPED_TEXTURE2D texmap;
		if( FAILED( hr = m_pTex2DCopy->Map( 0, D3D_MAP_READ, 0, &texmap ) ) )
			throw TRException("m_pRTTexture2DCopy->Map() failed.", hr);

		UINT uRedBits, uMask;

		// pull out the red channel
		uRedBits = GetBitsPerComponent( 0, rtvDesc.Format );
		uMask = 2 * (1 << (uRedBits -1)) - 1;

		bResult = (long)(((UINT*)texmap.pData)[0] & uMask);

		m_pTex2DCopy->Unmap(0);
	}
	catch (TRException& exc)
	{
		exc.Prepend("[SampleFinder::TestPixelNonZero()] ");
		m_pFramework->WriteToLog( exc.GetError().c_str() );
		tRes = exc.GetResult();
	}

	return tRes;
}

TEST_RESULT SampleFinder::FindSamples( bool bAllowOverlap, UINT srcSubResource )
{
	HRESULT hr = E_FAIL;
	TEST_RESULT tRes = RESULT_PASS;
	bool bResolve = !m_bMSLoadSupport;
	try
	{
		ResetStates();

		struct SUBPIXEL
		{
			UINT x, y;  // actual subpixel position is ( x/256.0, y/256.0 )
			UINT size;  // width and height = size/256.0
		};

		m_uSampleCount = 0;
		
		D3D_RENDER_TARGET_VIEW_DESC rtvDesc;
		m_pRTView->GetDesc(&rtvDesc);
		D3D_TEXTURE2D_DESC rtDesc;
		m_pRTTex2D->GetDesc( &rtDesc );

		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		UINT strides[] = { sizeof( VERTEX ) };
		UINT offsets[] = { 0 };
		
		// region of interest
		D3D_BOX PixBox;
		PixBox.left = m_PixBox.left;
		PixBox.right = m_PixBox.left + 1;
		PixBox.top = m_PixBox.top;
		PixBox.bottom = m_PixBox.top + 1;
		PixBox.front = 0;
		PixBox.back = 1;
		
		for (UINT uSample = 0; uSample < rtDesc.SampleDesc.Count; ++uSample)
		{
			// only run outer loop once
			if (!bAllowOverlap)
				uSample = rtDesc.SampleDesc.Count;
		
			//queue of possible sample locations
			std::queue<SUBPIXEL> q;

			UINT uCount = 0;
			UINT uChildren = 0;
			bool bFirstPass = true;

			// push square filling pixel
			SUBPIXEL sp;
			sp.x = 0;
			sp.y = 0;
			sp.size = 1 << (m_pFramework->m_iSubPixelPrecision);
			
			float loc_denominator = (float)(sp.size);

			q.push( sp );
			while (!q.empty())
			{
				if (q.size() > 256)
					throw TRException("Sample Finder Failed.  Finding more than 32 samples.", RESULT_ABORT);

				sp = q.front();
				q.pop();

				// convert subpixel coordinates to screen coordinates
				float x = (sp.x / loc_denominator) + m_PixBox.left;
				float y = (sp.y / loc_denominator) + m_PixBox.top;
				float size = sp.size / loc_denominator;

				// unbind vb
				//CBuffer *pNullBuffers[] = { NULL };
				//m_pDevice->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );

				// fill vertex buffer
				VERTEX *pDestVerts;
				if( FAILED( hr = m_pVertexBuffer->Map( D3D_MAP_WRITE_DISCARD, NULL, (void **) &pDestVerts ) ) )
					throw TRException("m_pVertexBuffer->Map() failed.", hr);
				pDestVerts[0].pos = D3DXVECTOR2(x, y);
				pDestVerts[1].pos = D3DXVECTOR2(x, y+size);
				pDestVerts[2].pos = D3DXVECTOR2(x+size, y);
				pDestVerts[3].pos = D3DXVECTOR2(x+size, y+size);
				m_pVertexBuffer->Unmap();

				// bind vb
				m_pDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );

				// render
				m_pDevice->OMSetRenderTargets(1, &m_pRTView, m_pDSView );
				m_pDevice->ClearRenderTargetView( m_pRTView, ClearColor );
				if( m_pDSView )
					m_pDevice->ClearDepthStencilView( m_pDSView, D3D_CLEAR_DEPTH, 1.0f, 0);
				if (bAllowOverlap)
					m_pDevice->OMSetBlendState( NULL, 0, 1 << uSample );
				m_pDevice->Draw(4, 0);

				// check resulting pixel
				BOOL bCoverage;
				if( RESULT_PASS != TestPixelNonZero( bCoverage, bResolve, srcSubResource ) )
					throw TRException("TexPixelNonZero Failed.");

				if(bFirstPass)
					bFirstPass = false;	// We don't want to count the first subpixel as a "child" it is the root.
				else
				{
					// Increment the number of children that have been processed
					uChildren++;

					// Increment the number of subpixels with coverage
					if( bCoverage )
					{
						uCount++;
					}

					// If we have done the four children, check to see if we have had at least one subpixel with coverage
					if(uChildren == 4)
					{
						// Check to see if we have at least one subpixel with covera`ge
						if(uCount > 0)
						{
							// Good, reset the counts
							uChildren = 0;
							uCount = 0;
						}
						else
						{
							// There should of been at lease one child that reported coverage!
							tstring error = tstring("A parent subpixel reported coverage, however no childern of that subpixel reported coverage. Here is the parent information: Parent size = ") + 
								ToString(sp.size * 2) + 
								tstring(", Parent Position = (") + 
								ToString(sp.x - sp.size) + 
								tstring(", ") + 
								ToString(sp.y - sp.size) + 
								tstring(").");
							// throw TRException(error);
							m_pFramework->WriteToLogF(error.c_str());
						}
					}
				}

				// split square or report it as sample location
				if ( bCoverage ) 
				{
					if (sp.size == 1)
					{
						if (m_uSampleCount < ARRAYSIZE(m_pSampleLocation))
						{
							// sample location found
							m_pSampleLocation[m_uSampleCount].x = sp.x;
							m_pSampleLocation[m_uSampleCount].y = sp.y;
						}

						++m_uSampleCount;
					}
					else
					{
						//Quad splitting
						SUBPIXEL s;
						s.size = sp.size / 2;

						s.x = sp.x;
						s.y = sp.y;
						q.push( s );

						s.x = sp.x + s.size;
						s.y = sp.y;
						q.push( s );

						s.x = sp.x;
						s.y = sp.y + s.size;
						q.push( s );

						s.x = sp.x + s.size;
						s.y = sp.y + s.size;
						q.push( s );
					}
				} // end if
			} // end while
		} // end for

		if (m_uSampleCount != rtDesc.SampleDesc.Count)
		{
			m_pFramework->WriteToLogF("SampleFinder options:");
			if (!bAllowOverlap)
				m_pFramework->WriteToLogF("-do not allow allow overlaping samples");
			if (bResolve)
				m_pFramework->WriteToLogF("-readback used multisample resolve.");
			else 
				m_pFramework->WriteToLogF("-readback used multisample load.");
			m_pFramework->WriteToLogF("Found: %d", m_uSampleCount);
			m_pFramework->WriteToLogF("Expected: %d", rtDesc.SampleDesc.Count);
			for (UINT i = 0; i < m_uSampleCount; ++i)
				m_pFramework->WriteToLogF("[Sample %d] x:%d, y:%d", i, m_pSampleLocation[i].x, m_pSampleLocation[i].y);
			throw TRException("Sample count found does not match the texture description.");
		}
	}
	catch (TRException& exc)
	{
		exc.Prepend("[Sample Finder::FindSamples()] ");
		m_pFramework->WriteToLog( exc.GetError().c_str() );
		tRes = exc.GetResult();
	}

	return tRes;
}


TEST_RESULT SampleFinder::OrderSamples( UINT srcSubResource )
{

	float loc_denominator = (float) (1 << (m_pFramework->m_iSubPixelPrecision));
	
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		HRESULT hr;
		if (m_uSampleCount < 1)
			throw TRException("No samples found.");

		D3D_TEXTURE2D_DESC rtDesc;
		m_pRTTex2D->GetDesc( &rtDesc );

		for (UINT uMaskIndex = 0; uMaskIndex < m_uSampleCount; uMaskIndex++)
		{
			// only let through a single sample
			UINT uMask = 1 << uMaskIndex;
			UINT uSampleIndex = 999;

			for (UINT uSampleID = 0; uSampleID < m_uSampleCount; uSampleID++)
			{
				ResetStates();

				// apply new mask
				m_pDevice->OMSetBlendState( NULL, NULL, uMask );

				{
					if (uSampleID >= m_uSampleCount || uSampleID > _countof( m_pSampleLocation ) - 1)
						throw TRException("Invalid sample index.");

					const float x = m_PixBox.left + m_pSampleLocation[uSampleID].x/loc_denominator;
					const float y = m_PixBox.top  + m_pSampleLocation[uSampleID].y/loc_denominator;
					const float size = 1.0f/loc_denominator;

					VERTEX *pDestVerts;

					//Fill vertex buffer
					if( FAILED( hr = m_pVertexBuffer->Map( D3D_MAP_WRITE_DISCARD, NULL, (void **) &pDestVerts ) ) )
						throw TRException( "Map on m_pVertexBuffer failed.", hr );

					pDestVerts[0].pos = D3DXVECTOR2(x, y);
					pDestVerts[1].pos = D3DXVECTOR2(x+size, y);
					pDestVerts[2].pos = D3DXVECTOR2(x, y+size);

					m_pVertexBuffer->Unmap();
				}

				// clear
				FLOAT ClearColor[4] = { 0, 0, 0, 0 };
				m_pDevice->ClearRenderTargetView( m_pRTView, ClearColor );
				if (m_pDSView)
					m_pDevice->ClearDepthStencilView( m_pDSView, D3D_CLEAR_DEPTH, 1.0f, 0);
				m_pDevice->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
				m_pDevice->Draw(3, 0);

				// check resulting pixel
				BOOL bResult;
				bool bResolve = !m_bMSLoadSupport;
				if( RESULT_PASS != TestPixelNonZero(bResult, bResolve, srcSubResource) )
					throw TRException("TestPixelNonZero failed.");

				// found sample and mask match
				if (bResult)
				{	
					if ((uSampleIndex != 999) || (uSampleID < uMaskIndex))
					{
						tstring errorStr = tstring("Sample mask (") + ToString(uMask) + 
							tstring(") corresponds to more than 1 sample.");
						throw TRException( errorStr );
					}

					uSampleIndex = uSampleID;
				}
			}
			// nothing found
			if (uSampleIndex == 999)
			{
				tstring errorStr = tstring("Sample mask (") + ToString(uMask) + 
					tstring(") does not correspond to any samples.");
				throw TRException( errorStr );
			}

			// swap samples
			if (uSampleIndex != uMaskIndex)
			{
				std::swap(m_pSampleLocation[uSampleIndex], m_pSampleLocation[uMaskIndex]);
			}
		}
	}	
	catch (TRException& exc)
	{
		exc.Prepend("[SampleFinder::OrderSamples()] ");
		m_pFramework->WriteToLog( exc.GetError().c_str() );
		tRes = exc.GetResult();
	}

	return tRes;
}

void SampleFinder::Cleanup()
{
	CBuffer* pBufferNull[] = { NULL };
	CBuffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };

	if (m_pFramework && m_pFramework->GetDevice())
	{
		m_pDevice = m_pFramework->GetDevice();
		m_pDevice->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
		m_pDevice->IASetInputLayout( NULL );
		m_pDevice->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );
		m_pDevice->VSSetShader( NULL );
		m_pDevice->GSSetShader( NULL );
		m_pDevice->PSSetShader( NULL );
		m_pDevice->VSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDevice->PSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDevice->RSSetState( NULL );
		m_pDevice->OMSetDepthStencilState( NULL, 0 );

		m_pDevice = NULL;
	}

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSLoad );

	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pBS );
	SAFE_RELEASE( m_pDSS );

	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexBuffer );

	SAFE_RELEASE( m_pRTViewLoad );
	SAFE_RELEASE( m_pSRView );

	SAFE_RELEASE( m_pRTTex2DLoad );
	SAFE_RELEASE( m_pTex2DResolve );
	SAFE_RELEASE( m_pTex2DCopy );
}