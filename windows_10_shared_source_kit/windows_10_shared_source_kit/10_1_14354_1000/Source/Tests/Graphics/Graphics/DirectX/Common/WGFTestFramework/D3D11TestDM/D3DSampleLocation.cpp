//////////////////////////////////////////////////////////////////////////
//  D3DSampleLocation.cpp
//  created:	2004/10/13
//
//  purpose: Implementation of SampleFinder
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "D3DSampleLocation.h"
#include "Shell.h"
#include "D3D11Test.h"

#include <queue>

struct SLVERTEX 
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 tex;
};

bool SampleFinder::Search( CD3D11TestFramework* pFramework, ID3D11RenderTargetView* pRTV, ID3D11Texture2D* pTex, std::vector<POINT> &vPos )
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		// The DX10 version of this code uses a state block to save/restore state here
		// state blocks are not a part of the D3D11 core, so they are not used here
		// Rather the sample finder uses a deferred context to achieve the same effect

		TEST_RESULT tRes;
		// Find sample locations
		SampleFinder finder;
		POINT pixpos = {0,0};

		if (RESULT_PASS != (tRes = finder.Setup( pFramework, pRTV, pTex, pixpos ) ) )
			throw TRException("SampleFinder failed Setup.", tRes);

		if (RESULT_PASS != (tRes = finder.FindSamples() ) )
			throw TRException("SampleFinder failed FindSamples.", tRes);

		if (RESULT_PASS != (tRes = finder.OrderSamples() ) )
			throw TRException("SampleFinder failed OrderSamples.", tRes);

		finder.Cleanup();

		vPos.clear();
		UINT uCount = finder.SampleCount();
		for (UINT i = 0; i < uCount; ++i)
			vPos.push_back(finder.GetLoc(i));

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
  m_pImmediateContext(NULL),
  m_pDeferredContext(NULL),
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
  m_pDSSDisabled(NULL),
  m_pVS(NULL),
  m_pPS(NULL),
  m_pPSLoad(NULL),
  m_uSampleCount(0),
  m_uForcedSampleCount(0),
  m_uEffectiveSampleCount(0),
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

TEST_RESULT SampleFinder::Setup(CD3D11TestFramework *pFramework, ID3D11RenderTargetView* pRTV, ID3D11Texture2D* pRTTex2D, POINT PixPos)
{
	const UINT forcedSampleCountOff = 0;
	return Setup( pFramework, pRTV, pRTTex2D, PixPos, forcedSampleCountOff );
}

TEST_RESULT SampleFinder::Setup(CD3D11TestFramework *pFramework, ID3D11RenderTargetView* pRTV, ID3D11Texture2D* pRTTex2D, POINT PixPos, UINT forcedSampleCount)
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		Cleanup();

		// init
		m_pDevice = pFramework->GetDevice1();
		m_pImmediateContext = pFramework->GetImmediateContext1();
		m_pFramework = pFramework;
		m_pRTTex2D = pRTTex2D;
		m_pRTView = pRTV;
		m_uForcedSampleCount = forcedSampleCount;
		m_bResolveSupport = false;
		m_bMSLoadSupport = false;

		// As of 12/15/2010 - CreateDeferredContext1 is not implemented. To workaround, we will QI
		ID3D11DeviceContext *pDeferredContext = NULL;
		if ( FAILED( m_pDevice->CreateDeferredContext( 0, &pDeferredContext ) ) )
		{
			throw TRException( "CreateDeferredContext failed" );
		}

		// Query for 11.1 device context interface 
		if( FAILED( pDeferredContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void **) &m_pDeferredContext) ) )
		{
			SAFE_RELEASE( pDeferredContext );
			throw TRException( "QueryInterface failed" );
		}

		SAFE_RELEASE( pDeferredContext );

		m_PixBox.left = PixPos.x;
		m_PixBox.right = PixPos.x + 1;
		m_PixBox.top = PixPos.y;
		m_PixBox.bottom = PixPos.y + 1;
		m_PixBox.front = 0;
		m_PixBox.back = 1;		

		// render target description
		D3D11_TEXTURE2D_DESC rtdesc;
		m_pRTTex2D->GetDesc( &rtdesc );

		// Check support for multisample load and resolve
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		m_pRTView->GetDesc(&rtvDesc);
		UINT32 uSupport;
		m_pDevice->CheckFormatSupport(rtvDesc.Format, &uSupport);
		if (uSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE)
			m_bResolveSupport = true;
		if (uSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_LOAD)
			m_bMSLoadSupport = true;
		
		//
		// Determine the effective sample count of the RT
		//
		m_uEffectiveSampleCount = m_uForcedSampleCount > 0 ? m_uForcedSampleCount : rtdesc.SampleDesc.Count;

		//
		// Setup the default viewport
		//
		D3D11_VIEWPORT vp;
		vp.Height = (FLOAT) rtdesc.Height;
		vp.Width = (FLOAT) rtdesc.Width;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pDeferredContext->RSSetViewports( 1, &vp );

		//
		// Set Rasterizer state
		//

		// Win8:307101 - Sample count 1 is only valid on D3D_FEATURE_LEVEL_11_0 or lower when MultisampleEnable is false
		const bool bMultisampleEnable = ((m_uForcedSampleCount == 1) && (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1)) ? false : true;

		D3D11_RASTERIZER_DESC1 rastDesc;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.MultisampleEnable = bMultisampleEnable;
		rastDesc.AntialiasedLineEnable = false;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.ForcedSampleCount = m_uForcedSampleCount;
		if( FAILED( m_pDevice->CreateRasterizerState1( &rastDesc, &m_pRS ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.");

		//
		// Set the Depth Stencil state
		//

		D3D11_DEPTH_STENCILOP_DESC depthOpDesc;
		depthOpDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthOpDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthOpDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthOpDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		D3D11_DEPTH_STENCIL_DESC depthDesc;
		depthDesc.BackFace = depthOpDesc;
		depthDesc.DepthEnable = false;
		depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDesc.FrontFace = depthOpDesc;
		depthDesc.StencilEnable = false;
		depthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		if( FAILED( m_pDevice->CreateDepthStencilState(&depthDesc, &m_pDSSDisabled) ) )
		{
			throw TRException("CreateDepthStencilState for m_pDSSDisabled failed.");
		}

		//
		// Create the constant buffer
		//
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = 512;
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( m_pDevice->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.");
	
		//
		// Create vertex buffer
		//
		D3D11_BUFFER_DESC bufferDesc =
		{
			100 * sizeof( SLVERTEX ),
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0
		};
		if( FAILED( m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pVertexBuffer ) ) )
			throw TRException("CreateBuffer for m_pVertexBuffer Failed.");

		UINT strides[] = { sizeof( SLVERTEX ) };
		UINT offsets[] = { 0 };

		//
		// setup const data
		//
		float *bufData;
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		if( FAILED( m_pDeferredContext->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
			throw TRException("Map on m_pConstBuffer Failed");
		bufData = (float*)mappedRes.pData;
		// Inverse viewport scale.
		bufData[2] = 2.f / rtdesc.Width;
		bufData[3] = -2.f / rtdesc.Height;
		// Prescaled inverse viewport translation.
		bufData[0] = -1.f;
		bufData[1] = 1.f;
		//pix pos
		bufData[4] = (float)m_PixBox.left;
		bufData[5] = (float)m_PixBox.top;
		m_pDeferredContext->Unmap(m_pConstBuffer,0);

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
	m_PixBox.left = PixPos.x;
	m_PixBox.right = PixPos.x + 1;
	m_PixBox.top = PixPos.y;
	m_PixBox.bottom = PixPos.y + 1;
	m_PixBox.front = 0;
	m_PixBox.back = 1;	

	// render target description
	D3D11_TEXTURE2D_DESC rtdesc;
	m_pRTTex2D->GetDesc( &rtdesc );

	//
	// setup const data
	//
	float *bufData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if( FAILED( m_pDeferredContext->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
		throw TRException("Map on m_pConstBuffer Failed");
	bufData = (float*) mappedRes.pData;

	// Inverse viewport scale.
	bufData[2] = 2.f / rtdesc.Width;
	bufData[3] = -2.f / rtdesc.Height;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.f;
	bufData[1] = 1.f;
	//pix pos
	bufData[4] = (float)m_PixBox.left;
	bufData[5] = (float)m_PixBox.top;
	m_pDeferredContext->Unmap(m_pConstBuffer,0);
}

void SampleFinder::CreateTextures()
{
	//
	// Setup RenderTarget
	//
	// render target description
	D3D11_TEXTURE2D_DESC rtdesc;
	m_pRTTex2D->GetDesc( &rtdesc );

	D3D11_TEXTURE2D_DESC tex2ddesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	m_pRTView->GetDesc( &rtvDesc );	

	tex2ddesc.ArraySize = 1;
	tex2ddesc.BindFlags = 0;	
	tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tex2ddesc.Format = rtvDesc.Format;
	tex2ddesc.Height = rtdesc.Height;
	tex2ddesc.Width = rtdesc.Width;
	tex2ddesc.MiscFlags = 0;
	tex2ddesc.MipLevels = 1;
	tex2ddesc.SampleDesc.Count = 1;
	tex2ddesc.SampleDesc.Quality = 0;
	tex2ddesc.Usage = D3D11_USAGE_STAGING;
	if ( FAILED(m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pTex2DCopy )) )
		throw TRException("CreateTexture2D failed for m_pRTTexture2DCopy.");

	tex2ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex2ddesc.CPUAccessFlags = 0;
	tex2ddesc.Usage = D3D11_USAGE_DEFAULT;
	if ( FAILED(m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pTex2DResolve )) )
		throw TRException("CreateTexture2D failed for m_pRTTexture2DResolve.");

	tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	if ( FAILED(m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pRTTex2DLoad )) )
		throw TRException("CreateTexture2D failed for m_pRTTexture2DResult.");

	// render target view
	D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
	rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtviewdesc.Texture2D.MipSlice = 0;
	rtviewdesc.Format = rtvDesc.Format;
	if ( FAILED( m_pDevice->CreateRenderTargetView( m_pRTTex2DLoad, &rtviewdesc, &m_pRTViewLoad ) ) )
		throw TRException("CreateRenderTargetView failed for m_pRTTexture2D and m_pRTView.");

	// resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC svDesc;
	svDesc.Format = rtvDesc.Format;
	svDesc.Texture2D.MipLevels = 1;
	svDesc.Texture2D.MostDetailedMip = 0;
	svDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	if( FAILED( m_pDevice->CreateShaderResourceView( m_pRTTex2D, &svDesc, &m_pSRView ) ) )
		throw TRException("CreateShaderResourceView failed for m_pSRView.");
}

void SampleFinder::CreateShaders()
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
	D3D11_TEXTURE2D_DESC rtdesc;
	m_pRTTex2D->GetDesc(&rtdesc);
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	m_pRTView->GetDesc( &rtvDesc );	
	DWORD flags = GetFlags( rtvDesc.Format );

	ID3D10Blob *pVSBuf = NULL;
	ID3D10Blob *pPSBuf = NULL;
	ID3D10Blob *pMSPSBuf = NULL; 
	ID3D10Blob *pErrorBuf = NULL; 

	// Compile and create all shader
	if ( FAILED(D3DX10CompileFromMemory( szVS, sizeof(szVS), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, NULL, &pVSBuf, &pErrorBuf, NULL )) )
		throw TRException( tstring("Failed to compile VS.") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "") );
	if ( FAILED(m_pDevice->CreateVertexShader( pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVS )) )
		throw TRException("Failed to create VS.");

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
	if ( FAILED(D3DX10CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "main", "ps_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pPSBuf, &pErrorBuf, NULL )) )
		throw TRException( tstring("Failed to compile PS.") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "") );
	if ( FAILED(m_pDevice->CreatePixelShader( pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPS )) )
		throw TRException("Failed to create PS.");

	// combine MSPS, requires output type info and sample count
	UINT uCount = rtdesc.SampleDesc.Count;
	_snprintf( szShader, MAX_STR-1, szMSPS, szType, uCount, szType, szType, szType, uCount, szType );
	szShader[MAX_STR-1] = 0;
	if ( FAILED(D3DX10CompileFromMemory( szShader, strlen(szShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pMSPSBuf, &pErrorBuf, NULL )) )
		throw TRException( tstring("Failed to compile MSPS.") + (pErrorBuf ? (char*)pErrorBuf->GetBufferPointer() : "") );
	else if ( FAILED(m_pDevice->CreatePixelShader( pMSPSBuf->GetBufferPointer(), pMSPSBuf->GetBufferSize(), NULL, &m_pPSLoad )) )
		throw TRException("Failed to create MSPS.");

	// Define our vertex data layout
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "tex", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	if(	FAILED(	m_pDevice->CreateInputLayout( layout, 2, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVertexLayout ) ) )
		throw TRException("CD3D11TestFramework::UpdateSampleLocations - CreateInputLayout for m_pVertexLayout Failed.");
	
	// cleanup
	SAFE_RELEASE( pVSBuf );
	SAFE_RELEASE( pPSBuf );
	SAFE_RELEASE( pMSPSBuf );
	SAFE_RELEASE( pErrorBuf );
}

//
// Executes all deferred commands on the immediate context
// This does not change the state of the immediate or deferred contexts
//
void SampleFinder::ExecuteDeferredContext()
{
	ID3D11CommandList* pCommandList = NULL;

	if( FAILED( m_pDeferredContext->FinishCommandList( TRUE, &pCommandList ) ) )
		throw TRException("FinishCommandList failed");

	m_pImmediateContext->ExecuteCommandList( pCommandList, TRUE );

	SAFE_RELEASE( pCommandList );
}

TEST_RESULT SampleFinder::AddDepth(ID3D11Texture2D* pDSTex2D, ID3D11DepthStencilView *pDSView)
{
	assert(pDSTex2D);
	assert(pDSView);
	assert(m_pDevice);

	// We can only add depth if TIR is not being used (forced sample count is 0 - OFF)
	if( m_uForcedSampleCount > 0 )
	{
		m_pFramework->WriteToLog("[SampleFinder::AddDepth] Depth/Stencil Views must not be bound, depth testing must be disabled, and the shader must not output depth while rendering with ForcedSampleCount 1 or greater, otherwise rendering behavior is undefined.");
		return RESULT_FAIL;
	}

	m_pDSTex2D = pDSTex2D;
	m_pDSView = pDSView;

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	if( FAILED( m_pDevice->CreateDepthStencilState( &dsDesc, &m_pDSS ) ) )
	{
		m_pFramework->WriteToLog("[SampleFinder::AddDepth] CreateDepthStencilState() failed.");
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void SampleFinder::ResetStates()
{
	// set state
	UINT strides[] = { sizeof( SLVERTEX ) };
	UINT offsets[] = { 0 };

	assert( m_pDeferredContext );
	m_pDeferredContext->IASetInputLayout( m_pVertexLayout );
	m_pDeferredContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );
	m_pDeferredContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	m_pDeferredContext->RSSetState( m_pRS );
	m_pDeferredContext->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDeferredContext->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDeferredContext->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );

	if( m_uForcedSampleCount > 0 )
	{
		m_pDeferredContext->OMSetDepthStencilState( m_pDSSDisabled, 0 );
	}
	else
	{
		m_pDeferredContext->OMSetDepthStencilState( m_pDSS, 0 );
	}

	m_pDeferredContext->OMSetRenderTargets(1, &m_pRTView, m_pDSView );

	// set shaders
	m_pDeferredContext->VSSetShader( m_pVS, NULL, 0 );
	m_pDeferredContext->GSSetShader( NULL, NULL, 0 );
	m_pDeferredContext->PSSetShader( m_pPS, NULL, 0 );
}


TEST_RESULT SampleFinder::TestPixelNonZero(BOOL &bResult, bool bResolve, UINT srcSubResource)
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		m_pRTView->GetDesc(&rtvDesc);
		D3D11_TEXTURE2D_DESC rtDesc;
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
			m_pDeferredContext->VSSetShader( m_pVS, NULL, 0 );
			m_pDeferredContext->GSSetShader( NULL, NULL, 0 );
			m_pDeferredContext->PSSetShader( m_pPSLoad, NULL, 0 );	

			// fill vertex buffer
			SLVERTEX *pDestVerts;
			D3D11_MAPPED_SUBRESOURCE mappedRes;
			if( FAILED( m_pDeferredContext->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
				throw TRException("m_pDeferredContext->Map() failed.");
			pDestVerts = (SLVERTEX*) mappedRes.pData;
			
			float x = (float)m_PixBox.left;
			float y = (float)m_PixBox.top;
			pDestVerts[0].pos = D3DXVECTOR2(x, y);
			pDestVerts[1].pos = D3DXVECTOR2(x, y+1);
			pDestVerts[2].pos = D3DXVECTOR2(x+1, y);
			pDestVerts[3].pos = D3DXVECTOR2(x+1, y+1);
			m_pDeferredContext->Unmap(m_pVertexBuffer,0);

			// multisample load requires another draw
			FLOAT ClearColor[4] = { 0.0, 0.0, 0.0, 0.0 };
			m_pDeferredContext->OMSetRenderTargets( 1, &m_pRTViewLoad, NULL );
			m_pDeferredContext->ClearRenderTargetView( m_pRTViewLoad, ClearColor );
			m_pDeferredContext->PSSetShaderResources( 0, 1, &m_pSRView );
			m_pDeferredContext->Draw(4, 0);
			//GetDevice()->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, &box );

			// set shader back
			ID3D11ShaderResourceView	*pSRVNull[] = { NULL, NULL, NULL, NULL };
			m_pDeferredContext->PSSetShaderResources( 0, 1, pSRVNull );
			m_pDeferredContext->PSSetShader( m_pPS, NULL, 0 );
			m_pDeferredContext->GSSetShader( NULL, NULL, 0 );
			m_pDeferredContext->OMSetRenderTargets(1, &m_pRTView, NULL );
			m_pDeferredContext->CopySubresourceRegion( m_pTex2DResolve, 0, 0, 0, 0, m_pRTTex2DLoad, 0, 0 );
			m_pDeferredContext->CopySubresourceRegion( m_pTex2DCopy, 0, 0, 0, 0, m_pRTTex2DLoad, 0, &m_PixBox );
			
		}
		else
		{
			m_pDeferredContext->ResolveSubresource( m_pTex2DResolve, 0, m_pRTTex2D, srcSubResource, rtvDesc.Format );
			m_pDeferredContext->CopySubresourceRegion( m_pTex2DCopy, 0, 0, 0, 0, m_pTex2DResolve, 0, &m_PixBox );
		}

		// Execute all pending commands in m_pDeferredContext on the immediate context
		// But do not affect immediate context state
		ExecuteDeferredContext();

		D3D11_MAPPED_SUBRESOURCE texmap;
		if( FAILED( m_pImmediateContext->Map( m_pTex2DCopy, 0, D3D11_MAP_READ, 0, &texmap ) ) )
			throw TRException("m_pDeferredContext->Map() failed.");

		UINT uRedBits, uMask;

		// pull out the red channel
		uRedBits = GetBitsPerComponent( 0, rtvDesc.Format );
		uMask = 2 * (1 << (uRedBits -1)) - 1;

		bResult = (long)(((UINT*)texmap.pData)[0] & uMask);

		m_pImmediateContext->Unmap(m_pTex2DCopy, 0);
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

		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		UINT strides[] = { sizeof( SLVERTEX ) };
		UINT offsets[] = { 0 };
		
		// region of interest
		D3D11_BOX PixBox;
		PixBox.left = m_PixBox.left;
		PixBox.right = m_PixBox.left + 1;
		PixBox.top = m_PixBox.top;
		PixBox.bottom = m_PixBox.top + 1;
		PixBox.front = 0;
		PixBox.back = 1;
		
		for (UINT uSample = 0; uSample < EffectiveSampleCount(); ++uSample)
		{
			// only run outer loop once
			if (!bAllowOverlap)
				uSample = EffectiveSampleCount();
		
			//queue of possible sample locations
			std::queue<SUBPIXEL> q;

			// push square filling pixel
			SUBPIXEL sp;
			sp.x = 0;
			sp.y = 0;
			sp.size = 256;

			q.push( sp );
			while (!q.empty())
			{
				if (q.size() > 256)
					throw TRException("Sample Finder Failed.  Finding more than 32 samples.", RESULT_ABORT);

				sp = q.front();
				q.pop();

				// convert subpixel coordinates to screen coordinates
				float x = (sp.x / 256.0f) + m_PixBox.left;
				float y = (sp.y / 256.0f) + m_PixBox.top;
				float size = sp.size / 256.0f;

				// unbind vb
				//ID3D11Buffer *pNullBuffers[] = { NULL };
				//m_pDevice->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );

				// fill vertex buffer
				SLVERTEX *pDestVerts;
				D3D11_MAPPED_SUBRESOURCE mappedRes;
				if( FAILED( m_pDeferredContext->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
					throw TRException("m_pDeferredContext->Map() failed.");
				pDestVerts = (SLVERTEX*) mappedRes.pData;

				pDestVerts[0].pos = D3DXVECTOR2(x, y);
				pDestVerts[1].pos = D3DXVECTOR2(x, y+size);
				pDestVerts[2].pos = D3DXVECTOR2(x+size, y);
				pDestVerts[3].pos = D3DXVECTOR2(x+size, y+size);
				m_pDeferredContext->Unmap(m_pVertexBuffer,0);

				// bind vb
				m_pDeferredContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );

				// render
				m_pDeferredContext->OMSetRenderTargets(1, &m_pRTView, m_pDSView );
				m_pDeferredContext->ClearRenderTargetView( m_pRTView, ClearColor );
				if( m_pDSView )
					m_pDeferredContext->ClearDepthStencilView( m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
				if (bAllowOverlap)
					m_pDeferredContext->OMSetBlendState( NULL, 0, 1 << uSample );
				m_pDeferredContext->Draw(4, 0);

				// check resulting pixel
				BOOL bCoverage;
				if( RESULT_PASS != TestPixelNonZero( bCoverage, bResolve, srcSubResource ) )
					throw TRException("TexPixelNonZero Failed.");

				// split square or report it as sample location
				if ( bCoverage ) {
					if (sp.size == 1)
					{
						// sample location found
						m_pSampleLocation[m_uSampleCount].x = sp.x;
						m_pSampleLocation[m_uSampleCount++].y = sp.y;	
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

		if (m_uSampleCount != EffectiveSampleCount())
		{
			D3D11_TEXTURE2D_DESC rtDesc;
			m_pRTTex2D->GetDesc( &rtDesc );

			m_pFramework->WriteToLogF("SampleFinder options:");
			if (!bAllowOverlap)
				m_pFramework->WriteToLogF("-do not allow allow overlaping samples");
			if (bResolve)
				m_pFramework->WriteToLogF("-readback used multisample resolve.");
			else 
				m_pFramework->WriteToLogF("-readback used multisample load.");
			m_pFramework->WriteToLogF("Found: %u", m_uSampleCount);
			m_pFramework->WriteToLogF("Expected: %u (Forced Sample Count = %u, RT Sample count = %u)", EffectiveSampleCount(), ForcedSampleCount(), rtDesc.SampleDesc.Count);
			for (UINT i = 0; i < m_uSampleCount; ++i)
				m_pFramework->WriteToLogF("[Sample %d] x:%d, y:%d", i, m_pSampleLocation[i].x, m_pSampleLocation[i].y);
			throw TRException("Sample count found does not match the texture description (or Forced Sample Count, if TIR is enabled).");
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

static bool operator!=(const POINT& lhs, const POINT& rhs)
{
	return (lhs.x != rhs.x) || (lhs.y != rhs.y);
}


TEST_RESULT SampleFinder::OrderSamples( UINT srcSubResource )
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		if (m_uSampleCount < 1)
			throw TRException("No samples found.");

		D3D11_TEXTURE2D_DESC rtDesc;
		m_pRTTex2D->GetDesc( &rtDesc );

		for (UINT uMaskIndex = 0; uMaskIndex < m_uSampleCount; uMaskIndex++)
		{
			// only let through a single sample
			UINT uMask = 1 << uMaskIndex;
			UINT uSampleIndex = 999;

			for (UINT uSampleID = uMaskIndex; uSampleID < m_uSampleCount; uSampleID++)
			{
				ResetStates();

				// apply new mask
				m_pDeferredContext->OMSetBlendState( NULL, NULL, uMask );

				{
					if (uSampleID >= m_uSampleCount)
						throw TRException("Invalid sample index.");

					const float x = m_PixBox.left + m_pSampleLocation[uSampleID].x/256.0f;
					const float y = m_PixBox.top  + m_pSampleLocation[uSampleID].y/256.0f;
					const float size = 1.0/256;

					SLVERTEX *pDestVerts;
					D3D11_MAPPED_SUBRESOURCE mappedRes;
					//Fill vertex buffer
					if( FAILED( m_pDeferredContext->Map( m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mappedRes ) ) )
						throw TRException( "Map on m_pVertexBuffer failed." );
					pDestVerts = (SLVERTEX*) mappedRes.pData;

					pDestVerts[0].pos = D3DXVECTOR2(x, y);
					pDestVerts[1].pos = D3DXVECTOR2(x+size, y);
					pDestVerts[2].pos = D3DXVECTOR2(x, y+size);

					m_pDeferredContext->Unmap(m_pVertexBuffer,0);
				}

				// clear
				FLOAT ClearColor[4] = { 0, 0, 0, 0 };
				m_pDeferredContext->ClearRenderTargetView( m_pRTView, ClearColor );
				if (m_pDSView)
					m_pDeferredContext->ClearDepthStencilView( m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
				m_pDeferredContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
				m_pDeferredContext->Draw(3, 0);

				// check resulting pixel
				BOOL bResult = true;
				bool bResolve = !m_bMSLoadSupport;
				if( RESULT_PASS != TestPixelNonZero(bResult, bResolve, srcSubResource) )
					throw TRException("TestPixelNonZero failed.");

				// found sample and mask match
				if (bResult)
				{	
					// Fail if more than 1 sample is found coressponding to the same mask bit
					// and the samples are at different locations
					if ((uSampleIndex != 999) && (m_pSampleLocation[uSampleIndex] != m_pSampleLocation[uSampleID]))
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
	m_pImmediateContext = NULL;

	// Workaround for Win7: 729753
	// Unrecycled emulated command lists leak some of their memory. 
	// But, recycled command lists do clean up nicely. This problem is only in emulation...
	if( m_pDeferredContext )
	{
		ID3D11CommandList *pDelCmdList = NULL;
		m_pDeferredContext->FinishCommandList( TRUE, &pDelCmdList );
		SAFE_RELEASE( pDelCmdList );
		SAFE_RELEASE( m_pDeferredContext );
	}
	
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSLoad );

	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pBS );
	SAFE_RELEASE( m_pDSS );
	SAFE_RELEASE( m_pDSSDisabled );

	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexBuffer );

	SAFE_RELEASE( m_pRTViewLoad );
	SAFE_RELEASE( m_pSRView );

	SAFE_RELEASE( m_pRTTex2DLoad );
	SAFE_RELEASE( m_pTex2DResolve );
	SAFE_RELEASE( m_pTex2DCopy );
}