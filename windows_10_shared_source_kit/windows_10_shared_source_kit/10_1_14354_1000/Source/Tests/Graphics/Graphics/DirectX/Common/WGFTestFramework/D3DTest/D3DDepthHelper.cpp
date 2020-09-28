//////////////////////////////////////////////////////////////////////////
//  D3DDepthHelper.h
//  created:	2006/10/03
//
//  purpose: Helper class to fill depth buffers
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "D3DDepthHelper.h"
#include "D3D10Wrapped.h"
#include "D3DMultiWrapped.h"
#include "D3D10Test.h"

//
// structs
//
struct VERTEX 
{
	D3DXVECTOR3 pos;
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

	void Prepend( tstring str ) { sError = str + sError; }
	tstring GetError() { return sError + tstring( "\n" ); }
	TEST_RESULT GetResult() { return tRes; }

private:
	tstring sError;
	TEST_RESULT tRes;
};


//////////////////////////////////////////////////////////////////////////
// DepthHelper
//
// Helper class to fill depth buffers
//////////////////////////////////////////////////////////////////////////
DepthHelper::DepthHelper()
: m_pDevice( NULL ),
  m_pFramework( NULL ),
  m_pDSV( NULL ),
  m_pConstBuffer( NULL ),
  m_pVertexBuffer( NULL ),
  m_pVertexLayout( NULL ),
  m_pRS( NULL ),
  m_pDSS( NULL ),
  m_pVS( NULL ),
  m_pGS( NULL ),
  m_uWidth( 0 ),
  m_uHeight( 0 )
{

}

DepthHelper::~DepthHelper()
{
	Cleanup();
}

TEST_RESULT DepthHelper::Setup(CD3DTestFramework *pFramework, CDepthStencilView* pDSV /* = NULL  */)
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		Cleanup();

		// set depth/stencil view
		if (pDSV) m_pDSV = pDSV;

		// validate framework
		if (!pFramework)
			throw TRException("No CD3DTestFramework provided.", RESULT_ABORT);
		m_pFramework = pFramework;

		// get device
		m_pDevice = pFramework->GetDevice();
		if (!m_pDevice)
			throw TRException("TestFramework did not provide a valid device.", RESULT_ABORT);

		//
		// Set Rasterizer state
		//
		D3D_RASTERIZER_DESC rastDesc;
		rastDesc.CullMode = D3D_CULL_NONE;
		rastDesc.FrontCounterClockwise = true;
		rastDesc.DepthBias = false;
		rastDesc.DepthBiasClamp = 0;
		rastDesc.SlopeScaledDepthBias = 0;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.MultisampleEnable = true;
		rastDesc.AntialiasedLineEnable = false;
		rastDesc.FillMode = D3D_FILL_SOLID;
		if( FAILED( m_pDevice->CreateRasterizerState( &rastDesc, &m_pRS ) ) )
			throw TRException("CreateRasterizerState for m_pRSEnabled failed.", RESULT_ABORT);

		//
		// Create DepthStencil state
		//
		D3D_DEPTH_STENCIL_DESC dsDesc;
		memset(&dsDesc, 0, sizeof(dsDesc));
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D_COMPARISON_ALWAYS;
		dsDesc.StencilEnable = FALSE;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;
		if( FAILED( m_pDevice->CreateDepthStencilState( &dsDesc, &m_pDSS ) ) )
			throw TRException("CreateDepthStencilState() failed.", RESULT_ABORT);

		//
		// Create the constant buffer
		//
		D3D_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = 128;
		bufdesc.Usage = D3D_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( m_pDevice->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.", RESULT_ABORT);

		//
		// Create shaders
		//
		CreateShaders();
	}
	catch (TRException& exc)
	{
		exc.Prepend("[DepthHelper::Setup()] ");
		m_pFramework->WriteToLog( exc.GetError().c_str() );
		tRes = exc.GetResult();
	}

	return tRes;
}

void DepthHelper::CreateShaders()
{
	//
	// Vertex Shader
	//
	char szVS[] = 
		"struct VSIn																\n"
		"{																			\n"
		"	float3 pos : pos;														\n"
		"};																			\n"
		"struct VSOut																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"};																			\n"
		"cbuffer cb0 : register(b0)													\n"
		"{																			\n"
		"	float4 inverseViewport;													\n"
		"	uint rtindex;																\n"
		"};																			\n"
		"VSOut main(VSIn input)														\n"
		"{																			\n"
		"	VSOut output;															\n"
		"	output.pos = float4( 0, 0, input.pos.z, 1.0 );							\n"
		"	output.pos.xy = input.pos.xy * inverseViewport.zw + inverseViewport.xy;	\n"
		"	return output;															\n"
		"}";

	CBlob *pBuf = NULL;
	
	// Compile and create shader
	if ( FAILED(m_pFramework->CompileShaderFromMemory( szVS, sizeof(szVS), "main", "vs_4_0", 0, &pBuf, NULL )) )
		throw TRException("Failed to compile VS.");
	if ( FAILED(m_pDevice->CreateVertexShader( pBuf->GetBufferPointer(), pBuf->GetBufferSize(), &m_pVS )) )
		throw TRException("Failed to create VS.");

	//
	//  Vertex Layout
	//
	const D3D_INPUT_ELEMENT_DESC layout[] =
	{
		{ "pos", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D_INPUT_PER_VERTEX_DATA, 0 },
	};
	if(	FAILED(	m_pDevice->CreateInputLayout( layout, 1, pBuf->GetBufferPointer(), pBuf->GetBufferSize(), &m_pVertexLayout ) ) )
		throw TRException("CD3DTestFramework::UpdateSampleLocations - CreateInputLayout for m_pVertexLayout Failed.");
	
	SAFE_RELEASE( pBuf );

	//
	// Geometry Shader
	//

	char szGS[] = 
		"struct GSIn																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"};																			\n"

		"struct GSOut																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	uint rtindex: SV_RenderTargetArrayIndex;								\n"
		"};																			\n"

		"cbuffer cb0 : register(b0)													\n"
		"{																			\n"
		"	float4 inverseViewport;													\n"
		"	uint rtindex;															\n"
		"};																			\n"

		"[maxvertexcount (24)]														\n"
		"void main(point GSIn input[1], inout PointStream<GSOut> stream)			\n"
		"{																			\n"
		"	GSOut output;															\n"
		"	output.rtindex = rtindex;												\n"
		"	output.pos = input[0].pos;												\n"
		"	stream.Append(output);													\n"
		"}";

	// Compile and create shader
	if ( FAILED(m_pFramework->CompileShaderFromMemory( szGS, sizeof(szGS), "main", "gs_4_0", 0, &pBuf, NULL )) )
		throw TRException("Failed to compile GS.", RESULT_ABORT);
	if ( FAILED(m_pDevice->CreateGeometryShader( pBuf->GetBufferPointer(), pBuf->GetBufferSize(), &m_pGS )) )
		throw TRException("Failed to create GS.", RESULT_ABORT);
}

//
// Refresh - recreate objects that could change between fill calls
//		   - STATES SET - RSSetViewports
//
void DepthHelper::Refresh()
{
	SAFE_RELEASE( m_pVertexBuffer );

	//
	// Create vertex buffer
	//
	D3D_BUFFER_DESC bufferDesc =
	{
		m_uWidth * m_uHeight * sizeof( VERTEX ),
		D3D_USAGE_DYNAMIC,
		D3D_BIND_VERTEX_BUFFER,
		D3D_CPU_ACCESS_WRITE,
		0
	};
	if( FAILED( m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pVertexBuffer ) ) )
		throw TRException("CreateBuffer for m_pVertexBuffer Failed.", RESULT_ABORT);

	//
	// Setup viewport
	//
	m_VP.Height = m_uHeight;
	m_VP.Width = m_uWidth;
	m_VP.MinDepth = 0;
	m_VP.MaxDepth = 1;
	m_VP.TopLeftX = 0;
	m_VP.TopLeftY = 0;
	m_pDevice->RSSetViewports( 1, &m_VP );
}

//
// FillCB - called every time array index changes
//		  - STATES SET: VSSetConstantBuffers 
//
void DepthHelper::FillCB( UINT uSlice )
{
	// unset CB
	CBuffer* pBufferNull[] = { NULL };
	m_pDevice->VSSetConstantBuffers( 0, 1, pBufferNull );

	//
	// setup const data
	//
	float *bufData;
	if( FAILED( m_pConstBuffer->Map( D3D_MAP_WRITE_DISCARD, NULL, (void **) &bufData ) ) )
		throw TRException("Map on m_pConstBuffer Failed", RESULT_ABORT);
	// Inverse viewport scale.
	bufData[2] = 2.f / m_uWidth;
	bufData[3] = -2.f / m_uHeight;
	// Prescaled inverse viewport translation.
	bufData[0] = -1.f;
	bufData[1] = 1.f;
	//array index
	UINT *pIndex = (UINT*)&bufData[4];
	*pIndex = uSlice;
	m_pConstBuffer->Unmap();

	// set CB
	m_pDevice->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
}


//
// ResolveDimension - resolves array size from depth/stencil view desc
//
void DepthHelper::ResolveDimension(UINT &uArraySize)
{
	// get array info
	D3D_DEPTH_STENCIL_VIEW_DESC dsvdesc;
	m_pDSV->GetDesc(&dsvdesc);

	switch (dsvdesc.ViewDimension)
	{
	case D3D_DSV_DIMENSION_TEXTURE1DARRAY:
		uArraySize = dsvdesc.Texture1DArray.ArraySize;
		break;
	case D3D_DSV_DIMENSION_TEXTURE2DARRAY:
		uArraySize = dsvdesc.Texture2DArray.ArraySize;
		break;
	case D3D_DSV_DIMENSION_TEXTURE2DMSARRAY:
		uArraySize = dsvdesc.Texture2DMSArray.ArraySize;
		break;
	default:
		uArraySize = 1;
		break;
	}
}

//
// FillVB - called for every slice for every sample ( slice count * sample count )
//		  - STATES SET: IASetVertexBuffers 
// Note: pData layout should be pData[slices][height][width][samples]
void DepthHelper::FillVB( FLOAT *pData, 
						  UINT uSample, 
						  UINT uSlice, 
						  UINT uSampleCount,
						  UINT uRowPitch, 
						  UINT uSlicePitch )
{
	// unset vb
	CBuffer *pNullBuffers[] = { NULL };
	UINT strides[] = { sizeof( VERTEX ) };
	UINT offsets[] = { 0 };
	m_pDevice->IASetVertexBuffers( 0, 1, pNullBuffers, strides, offsets );


	// fill vertex buffer
	VERTEX *pVerts;
	if( FAILED( m_pVertexBuffer->Map( D3D_MAP_WRITE_DISCARD, NULL, (void **) &pVerts ) ) )
		throw TRException("m_pVertexBuffer->Map() failed.", RESULT_ABORT);

	// slice address
	BYTE *pSlice = (BYTE*)pData + uSlicePitch * uSlice;
	for (UINT y = 0; y < m_uHeight; ++y)
	{
		// row address
		FLOAT *pRow = (FLOAT*)(pSlice + uRowPitch * y);
		for (UINT x = 0; x < m_uWidth; ++x)
		{
			FLOAT depth = pRow[uSample + uSampleCount*x];
			pVerts->pos = D3DXVECTOR3( x + 0.5f, y + 0.5f, depth);
			++pVerts;
		}
	}
	m_pVertexBuffer->Unmap();

	// set vb
	m_pDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, strides, offsets );
}
//
// Fill - entry point for filling depth buffer
//
// Note: pData layout should be pData[slices][height][width][samples]
TEST_RESULT DepthHelper::Fill(FLOAT *pData, 
					   UINT uWidth, 
					   UINT uHeight /* = 1 */, 
					   UINT uSampleCount /* = 1 */, 
					   UINT uRowPitch /* = 0 */, 
					   UINT uSlicePitch /* = 0 */, 
					   CDepthStencilView* pDSV /* = NULL  */)
{
	TEST_RESULT tRes = RESULT_PASS;
	try
	{
		// check depth/stencil view
		if (pDSV)
			m_pDSV = pDSV;
		if (!m_pDSV)
			throw TRException("No depth/stencil view provided.", RESULT_ABORT);

		// refresh 
		if ((m_uWidth != uWidth) || (m_uHeight != uHeight))
		{
			m_uWidth = uWidth;
			m_uHeight = uHeight;
			Refresh();
		}

		// set state
		SetState();

		// if pitch not provided, assume packed
		if (!uRowPitch)
			uRowPitch = m_uWidth * uSampleCount * sizeof(FLOAT);
		if (!uSlicePitch)
			uSlicePitch = m_uHeight * uRowPitch;

		// calculate first slice and array size
		UINT uArraySize;
		ResolveDimension(uArraySize);


		///////////////////////////////////////////////////////////////////////////
		////		START BUGBUG 
		///////////////////////////////////////////////////////////////////////////
		//CTexture2D *pTex2D(NULL);
		//CTexture1D *pTex1D(NULL);
		//CRenderTargetView *pRTV(NULL);
		//D3D_RENDER_TARGET_VIEW_DESC rtvdesc;

		//D3D_DEPTH_STENCIL_VIEW_DESC dsvdesc;
		//m_pDSV->GetDesc(&dsvdesc);

		//if (( dsvdesc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE1D ) ||
		//	( dsvdesc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE1DARRAY ))
		//{
		//	D3D_TEXTURE1D_DESC tex1ddesc;
		//	tex1ddesc.ArraySize = uArraySize;
		//	tex1ddesc.BindFlags = D3D_BIND_RENDER_TARGET;
		//	tex1ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	tex1ddesc.Width = m_uWidth;
		//	tex1ddesc.CPUAccessFlags = 0;
		//	tex1ddesc.MipLevels = 1;
		//	tex1ddesc.MiscFlags = 0;
		//	tex1ddesc.Usage = D3D_USAGE_DEFAULT;
		//	if ( FAILED( m_pDevice->CreateTexture1D( &tex1ddesc, NULL, &pTex1D) ) )
		//		throw TRException("CreateTexture2D failed for pTex1D.");
		//
		//	// render target view
		//	D3D_RENDER_TARGET_VIEW_DESC rtviewdesc;
		//	switch (dsvdesc.ViewDimension)
		//	{
		//	case D3D_DSV_DIMENSION_TEXTURE1D:
		//		rtviewdesc.Texture1D.MipSlice = 0;
		//		rtviewdesc.ViewDimension = D3D_RTV_DIMENSION_TEXTURE1D;
		//		break;
		//	case D3D_DSV_DIMENSION_TEXTURE1DARRAY:
		//		rtviewdesc.Texture1DArray.ArraySize = uArraySize;
		//		rtviewdesc.Texture1DArray.FirstArraySlice = 0;
		//		rtviewdesc.Texture1DArray.MipSlice = 0;
		//		rtviewdesc.ViewDimension = D3D_RTV_DIMENSION_TEXTURE1DARRAY;
		//		break;
		//	}
		//	rtviewdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	if ( FAILED( m_pDevice->CreateRenderTargetView( pTex1D, &rtviewdesc, &pRTV ) ) )
		//		throw TRException("CreateRenderTargetView failed for m_pTex1D and m_pRTV.");
		//}
		//else 
		//{
		//	D3D_TEXTURE2D_DESC tex2ddesc;
		//	tex2ddesc.ArraySize = uArraySize;
		//	tex2ddesc.BindFlags = D3D_BIND_RENDER_TARGET;
		//	tex2ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // optional MS format support...
		//	tex2ddesc.Height = m_uHeight;
		//	tex2ddesc.Width = m_uWidth;
		//	tex2ddesc.CPUAccessFlags = 0;
		//	tex2ddesc.MipLevels = 1;
		//	tex2ddesc.MiscFlags = 0;
		//	tex2ddesc.SampleDesc.Count = uSampleCount;
		//	tex2ddesc.SampleDesc.Quality = 0;
		//	tex2ddesc.Usage = D3D_USAGE_DEFAULT;
		//	if ( FAILED( m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &pTex2D) ) )
		//		throw TRException("CreateTexture2D failed for pTex2D.");

		//	// render target view
		//	D3D_RENDER_TARGET_VIEW_DESC rtviewdesc;
		//	switch (dsvdesc.ViewDimension)
		//	{
		//	case D3D_DSV_DIMENSION_TEXTURE2DARRAY:
		//		rtviewdesc.Texture2DArray.ArraySize = uArraySize;
		//		rtviewdesc.Texture2DArray.FirstArraySlice = 0;
		//		rtviewdesc.Texture2DArray.MipSlice = 0;
		//		rtviewdesc.ViewDimension = D3D_RTV_DIMENSION_TEXTURE2DARRAY;
		//		break;
		//	case D3D_DSV_DIMENSION_TEXTURE2DMSARRAY:
		//		rtviewdesc.Texture2DMSArray.ArraySize = uArraySize;
		//		rtviewdesc.Texture2DMSArray.FirstArraySlice = 0;
		//		rtviewdesc.ViewDimension = D3D_RTV_DIMENSION_TEXTURE2DMSARRAY;
		//		break;
		//	case D3D_DSV_DIMENSION_TEXTURE2DMS:
		//		rtviewdesc.ViewDimension = D3D_RTV_DIMENSION_TEXTURE2DMS;
		//		break;
		//	case D3D_DSV_DIMENSION_TEXTURE2D:
		//		rtviewdesc.Texture2D.MipSlice = 0;
		//		rtviewdesc.ViewDimension = D3D_RTV_DIMENSION_TEXTURE2D;
		//		break;
		//	}
		//	rtviewdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	if ( FAILED( m_pDevice->CreateRenderTargetView( pTex2D, &rtviewdesc, &pRTV ) ) )
		//		throw TRException("CreateRenderTargetView failed for m_pTex1D and m_pRTV.");
		//}
		//m_pDevice->OMSetRenderTargets(1, &pRTV, m_pDSV );
		///////////////////////////////////////////////////////////////////////////
		////		END BUGBUG
		///////////////////////////////////////////////////////////////////////////

		m_pDevice->ClearDepthStencilView(m_pDSV, D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);

		// let the filling commence!
		// # draw calls = array size * sample count
		for ( UINT uSlice = 0; uSlice < uArraySize; ++uSlice )
		{
			FillCB( uSlice );
			for ( UINT uSample = 0; uSample < uSampleCount; ++uSample )
			{
				FillVB(pData, uSample, uSlice, uSampleCount, uRowPitch, uSlicePitch);
				m_pDevice->OMSetBlendState( NULL, NULL, 1 << uSample );
				m_pDevice->Draw(m_uWidth * m_uHeight, 0);
			}
		}

		//// BUGBUG - remove next 4 lines
		//m_pDevice->OMSetRenderTargets(0, NULL, NULL );
		//SAFE_RELEASE(pTex2D);
		//SAFE_RELEASE(pTex1D);
		//SAFE_RELEASE(pRTV);
		
	}
	catch (TRException& exc)
	{
		exc.Prepend("[DepthHelper::Fill()] ");
		m_pFramework->WriteToLog( exc.GetError().c_str() );
		tRes = exc.GetResult();
	}

	// recover old state
	ResetState();

	return tRes;
}

void DepthHelper::SetState()
{
	// set state
	UINT strides[] = { sizeof( VERTEX ) };
	UINT offsets[] = { 0 };
	m_pDevice->IASetInputLayout( m_pVertexLayout );
	m_pDevice->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_POINTLIST );
	m_pDevice->RSSetState( m_pRS );
	m_pDevice->RSSetViewports( 1, &m_VP );
	m_pDevice->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
	m_pDevice->OMSetDepthStencilState(m_pDSS, 0);
	m_pDevice->OMSetRenderTargets(0, NULL, m_pDSV );

	// set shaders
	m_pDevice->VSSetShader( m_pVS );
	m_pDevice->GSSetShader( m_pGS );
	m_pDevice->PSSetShader( NULL );

	m_pDevice->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	m_pDevice->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
}

void DepthHelper::ResetState()
{
	CBuffer* pBufferNull[] = { NULL };
	CBuffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };

	if (m_pDevice)
	{
		m_pDevice->IASetInputLayout( NULL );
		m_pDevice->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );
		m_pDevice->RSSetState( NULL );
		m_pDevice->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
		m_pDevice->OMSetDepthStencilState( NULL, 0 );
		m_pDevice->OMSetRenderTargets(0, NULL, NULL );
		
		m_pDevice->VSSetShader( NULL );
		m_pDevice->GSSetShader( NULL );
		m_pDevice->PSSetShader( NULL );
		m_pDevice->VSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDevice->GSSetConstantBuffers( 0, 1, pBufferNull );
	}
}

void DepthHelper::Cleanup()
{
	SAFE_RELEASE( m_pConstBuffer );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pVertexLayout );

	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pDSS );
	
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
}