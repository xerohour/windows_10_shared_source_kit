//////////////////////////////////////////////////////////////////////////
//  Rasterizer.cpp
//  created:	2006/07/14
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include <WGFTestCore.h>
#include <stdlib.h>
#include <queue>
#include <pch.h>
#include <DXGIBufferCompare.h>
using namespace std;

#include "rasterizer.h"
#include "fillmode.h"
#include "cullmode.h"
#include "state.h"
#include "interpolator.h"
#include "pullModel.h"
#include "forcedsamplecount.h"

//
// Test Case Data
//
Vertex VB[] = {
	{1.5f, 1.5f},			// pixel 1, 1
	{6.5f, 1.5f},			// pixel 6, 1
	{6.5f, 6.5f},			// pixel 6, 6
	{6.5f, 2000.0f},		// pixel 6, way offscreen
	
	{0.5f, 0.5f,      0.5f,   1,   -7},			// pixel 0, 0 center
	{3.5f, 0.5f,      2.0f,   8,    5},			// pixel 3, 0 center
	{0.5f, 3.5f,     10.0f,  19,   10},			// pixel 0, 3 center
	{3.5f, 3.5f,	377.0f, 377,  377}			// pixel 3, 3 center
};

IndexBuffer IB[] = 
{
	// single triangles
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CW,  3, { 0, 1, 2 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CCW, 3, { 0, 2, 1 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CW,  3, { 0, 1, 3 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CW,  3, { 1, 2, 0 } },

	// quads
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, CW,  4, { 4, 5, 6, 7 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, CCW, 4, { 4, 6, 5, 7 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  CW,  6, { 4, 5, 6, 5, 7, 6 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  CCW,  6, { 4, 6, 5, 5, 6, 7 } },

	// quads - same layout but different leading vertex
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, CW,  4, { 7, 6, 5, 4 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, CCW, 4, { 7, 5, 6, 4 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  CW,  6, { 7, 6, 5, 6, 4, 5} },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  CCW,  6, { 7, 5, 6, 6, 5, 4} },

	// quads - alternate split
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, CW,  4, { 6, 4, 7, 5 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, CCW, 4, { 6, 7, 4, 5 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  CW,  6, { 6, 4, 7, 4, 5, 7 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  CCW, 6, { 6, 7, 4, 4, 7, 5 } },

	// quads - alternate split + different leading vertex
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, CW,  4, { 6, 4, 7, 5 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, CCW, 4, { 6, 7, 4, 5 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  CW,  6, { 6, 4, 7, 4, 5, 7 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  CCW, 6, { 6, 7, 4, 4, 7, 5 } },

	// zero area triangles
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CW,  3, { 0, 1, 1 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CCW, 3, { 1, 1, 0 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CW,  3, { 1, 2, 2 } },
	{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, CW,  3, { 3, 3, 2 } },
};

IndexBuffer *FullSet[] = { &IB[0], &IB[1], &IB[2], &IB[3], 
						   &IB[4], &IB[5], &IB[6], &IB[7],
						   &IB[8], &IB[9], &IB[10], &IB[11],
						   &IB[12], &IB[13], &IB[14], &IB[15],
						   &IB[16], &IB[17], &IB[18], &IB[19] };

IndexBuffer *SingleTriSet[] = { &IB[0], &IB[1], &IB[2], &IB[3] };

IndexBuffer *QuadSet[] = { &IB[4], &IB[5], &IB[6], &IB[7],
						   &IB[8], &IB[9], &IB[10], &IB[11],
						   &IB[12], &IB[13], &IB[14], &IB[15],
						   &IB[16], &IB[17], &IB[18], &IB[19] };

IndexBuffer *DegenTriSet[] = { &IB[20], &IB[21], &IB[22], &IB[23] };

D3D11_FILL_MODE FillModeSet[] = { D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID };
D3D11_CULL_MODE CullModeSet[] = { D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK };

//
// extern
//
CRasterizerTestApp g_TestApp;

//
// forward declarations 
//
tstring __stdcall ToString( IndexBuffer *type );
bool __stdcall FromString( IndexBuffer **pType, const tstring &str );

//////////////////////////////////////////////////////////////////////////
// CRasterizerTestApp
//////////////////////////////////////////////////////////////////////////

void CRasterizerTestApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();
	UnRegisterOptionVariable("srconly");
	m_D3DOptions.SrcOnly = true;

	int n = AddOptionCategory( _T( "Debug" ) );  
	RegisterOptionVariable( _T( "LogShaders" ), _T( "Logs the shaders for failing cases." ), &m_bLogShaders, false, OPTION_VALUE_CONSTANT, n );
	
	return;
}

bool CRasterizerTestApp::InitTestGroups()
{
	if( !ADD_TEST( tstring(_T( "Interpolator" )), CInterpolator ) ) return false;
	if( !ADD_TEST( tstring(_T( "FillMode" )), CFillMode ) ) return false;
	if( !ADD_TEST( tstring(_T( "State" )), CStateLimitsTest ) ) return false;
	if( !ADD_TEST( tstring(_T( "CullMode" )), CCullMode ) ) return false;
	if( !ADD_TEST( tstring(_T( "SampleFrequency" )), CSampleFreq ) ) return false;
	if( !ADD_TEST( tstring(_T( "PullModel" )), CPullModel ) ) return false;
	if( !ADD_TEST( tstring(_T( "ForcedSampleCount" )), CForcedSampleCountTest ) ) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// CRasterizerTest
//////////////////////////////////////////////////////////////////////////
CRasterizerTest::CRasterizerTest( ) 
: m_RTFormat( DXGI_FORMAT_R8G8B8A8_UNORM ),
  m_uRTSampleCount( 1 ),
  m_uForcedSampleCount( 0 ),
  m_uRTWidth( 8 ),
  m_uRTHeight( 8 ),
  m_pRTTexture2D( NULL ),
  m_pTexA( NULL ),
  m_pTexB( NULL ),
  m_pRTView( NULL ),
  m_pConstBuffer( NULL ),
  m_pVB( NULL ),
  m_pIB( NULL ),
  m_pVertexLayout( NULL ),
  m_pRSEnabled( NULL ),
  m_pBSLogicOR( NULL ),
  m_pVS( NULL ),
  m_pPS( NULL ),
  m_pDevice( NULL ),
  m_pBuffer( NULL ),
  m_pQuery( NULL ),
  m_VBData( ARRAY_SIZE( VB ) ),
  m_pDeviceContext( NULL )
{
	g_TestApp.m_RenderTargetOptions.SwapChainDesc.Width = m_uRTWidth;
	g_TestApp.m_RenderTargetOptions.SwapChainDesc.Height = m_uRTHeight;
}

void CRasterizerTest::InitRasterizerParameters()
{
	// 
	//	parameters
	//
	CTestCaseParameter< IndexBuffer* > *pBufferParam = AddParameter( _T( "IndexBuffer" ), &m_pBuffer );
	CTestCaseParameter< D3D11_FILL_MODE > *pFillModeParam = AddParameter( _T( "FillMode" ), &m_FillMode );
	CTestCaseParameter< D3D11_CULL_MODE > *pCullModeParam = AddParameter( _T( "CullMode" ), &m_CullMode );
	CTestCaseParameter< bool > *pFCCParam = AddParameter( _T( "FrontCounterClockwise" ), &m_bFCC );

	//
	// test factors
	//

	// fill mode factors
	m_rfFillMode = AddParameterValueSet< D3D11_FILL_MODE > ( pFillModeParam, 
		new CTableValueSet< D3D11_FILL_MODE >( FillModeSet, sizeof(D3D11_FILL_MODE), ARRAY_SIZE(FillModeSet) ) ); 
	m_rfFillModeWire = AddParameterValue< D3D11_FILL_MODE > ( pFillModeParam, D3D11_FILL_WIREFRAME );
	m_rfFillModeSolid = AddParameterValue< D3D11_FILL_MODE > ( pFillModeParam, D3D11_FILL_SOLID );

	// cull mode factors
	m_rfCullMode = AddParameterValueSet< D3D11_CULL_MODE > ( pCullModeParam, 
		new CTableValueSet< D3D11_CULL_MODE >( CullModeSet, sizeof(D3D11_CULL_MODE), ARRAY_SIZE(CullModeSet) ) ); 
	m_rfCullModeNone = AddParameterValue< D3D11_CULL_MODE > ( pCullModeParam, D3D11_CULL_NONE );
	
	// fcc parameters
	m_rfFCC = AddParameterValueSet< bool > ( pFCCParam, new CBoolValueSet() );
	m_rfFCCTrue = AddParameterValue< bool > ( pFCCParam, true );


	// IndexBuffer factors
	m_rfIBSet = AddParameterValueSet< IndexBuffer* > ( _T( "IndexBuffer" ), 
						new CTableValueSet< IndexBuffer* >( FullSet, sizeof(IndexBuffer*), ARRAY_SIZE(FullSet) ) );
	m_rfSingleTriSet = AddParameterValueSet< IndexBuffer* > ( _T( "IndexBuffer" ),
						new CTableValueSet< IndexBuffer* >( SingleTriSet, sizeof(IndexBuffer*), ARRAY_SIZE(SingleTriSet) ) );
	m_rfQuadSet = AddParameterValueSet< IndexBuffer* > ( _T( "IndexBuffer" ),
						new CTableValueSet< IndexBuffer* >( QuadSet, sizeof(IndexBuffer*), ARRAY_SIZE(QuadSet) ) );
	m_rfDegenTriSet = AddParameterValueSet< IndexBuffer* > ( _T( "IndexBuffer" ),
						new CTableValueSet< IndexBuffer* >( DegenTriSet, sizeof(IndexBuffer*), ARRAY_SIZE(DegenTriSet) ) );


	// filter out dx10.1 tests
	if (GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0)
	{
		// 10.1 specifies back facing for zero area triangles, 10.0 specifies nothing for this
		// IB 20 through 23 specify zero area triangles
		SkipMatching( FilterRange<IndexBuffer*>( _T("IndexBuffer"), &IB[20], &IB[23] ) );
	}
}

TEST_RESULT CRasterizerTest::Setup()
{
	return SetupD3D();
}

void CRasterizerTest::SetupRasterizerTestCase()
{
	HRESULT hr;

	//
	// Set Rasterizer state
	//

	// Win8:293911 - Sample count 1 is only valid on D3D_FEATURE_LEVEL_11_0 or lower when MultisampleEnable is false
	const bool bMultisampleEnable = ((m_uForcedSampleCount == 1) && (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_1)) ? false : true;

	D3D11_RASTERIZER_DESC1 rastDesc;
	rastDesc.CullMode = m_CullMode;
	rastDesc.FrontCounterClockwise = m_bFCC;
	rastDesc.DepthBias = false;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = true;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = bMultisampleEnable;
	rastDesc.AntialiasedLineEnable = false;
	rastDesc.FillMode = m_FillMode;
	rastDesc.ForcedSampleCount = m_uForcedSampleCount;
	if( FAILED( hr = m_pDevice->CreateRasterizerState1( &rastDesc, &m_pRSEnabled ) ) )
		throw TRException("CreateRasterizerState for m_pRSEnabled failed.", hr, RESULT_FAIL);

	m_pDeviceContext->RSSetState( m_pRSEnabled );
	//m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	m_pDeviceContext->IASetPrimitiveTopology( m_pBuffer->top );

	//
	// Fill index buffer
	//
	FillIndexBuffer(m_pBuffer);
}

TEST_RESULT CRasterizerTest::SetupD3D()
{
	HRESULT hr;
	m_pDevice = GetDevice1();
	m_pDeviceContext = GetEffectiveContext1();

	TEST_RESULT tRes = RESULT_PASS;
	ID3D10Blob *pVSBuf = NULL;
	ID3D10Blob *pPSBuf = NULL;

	try 
	{
		//
		// Setup the default viewport
		//
		D3D11_VIEWPORT vp;
		vp.Height = (float) m_uRTHeight;
		vp.Width = (float) m_uRTWidth;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pDeviceContext->RSSetViewports( 1, &vp );


		//
		// Create the constant buffer
		//
		D3D11_BUFFER_DESC bufdesc;
		bufdesc.ByteWidth = 512;
		bufdesc.Usage = D3D11_USAGE_DYNAMIC;
		bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdesc.MiscFlags = 0;
		if( FAILED( hr = m_pDevice->CreateBuffer( &bufdesc, NULL, &m_pConstBuffer ) ) )
			throw TRException("CreateBuffer for m_pConstBuffer failed.", hr, RESULT_FAIL);

		m_pDeviceContext->VSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->GSSetConstantBuffers( 0, 1, &m_pConstBuffer );
		m_pDeviceContext->PSSetConstantBuffers( 0, 1, &m_pConstBuffer );

		const char* szPSProfile = "ps_4_0";
		const char* szVSProfile = "vs_4_0";
		switch(GetDevice()->GetFeatureLevel())
		{
		default:
			break;
		case D3D_FEATURE_LEVEL_10_1:
			szPSProfile = "ps_4_1";
			szVSProfile = "vs_4_1";
			break;
		case D3D_FEATURE_LEVEL_11_0:
		case D3D_FEATURE_LEVEL_11_1:
			szPSProfile = "ps_5_0";
			szVSProfile = "vs_5_0";
			break;
		}

		//
		// Create Shaders
		//
		char szVS[] = 
		"struct VSIn																\n"
		"{																			\n"
		"	float2 pos : pos;														\n"
		"	float fdata : fdata;													\n"
		"	uint udata : udata;													\n"
		"	int idata : idata;													\n"
		"};																			\n"
		"struct VSOut																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	float3 data : data;														\n"
		"};																			\n"
		"cbuffer cb0 : register(b0)													\n"
		"{																			\n"
		"	float4 inverseViewport;													\n"
		"};																			\n"
		"VSOut main(VSIn input)														\n"
		"{																			\n"
		"	VSOut output;															\n"
		"	output.pos = float4( 0, 0, 0, 1.0 );									\n"
		"	output.pos.xy = input.pos * inverseViewport.zw + inverseViewport.xy;	\n"
		"	output.data = float3( 0, 0, 0 );										\n"

		"	return output;															\n"
		"}";

		char szPS[] = 
		"struct PSIn																\n"
		"{																			\n"
		"	float4 pos : SV_Position;												\n"
		"	float3 data : data;														\n"
		"};																			\n"
		"struct PSOut																\n"
		"{																			\n"
		"	float4 Color : SV_Target;												\n"
		"};																			\n"
		"cbuffer cb0 : register(b0)													\n"
		"{																			\n"
		"	float4 inverseViewport;													\n"
		"};																			\n"
		"PSOut main(PSIn input)														\n"
		"{																			\n"
		"	PSOut output;															\n"
		"	output.Color = float4( 1.0, 1.0, 1.0, 1.0 );							\n"
		"	return output;															\n"
		"}";	

			// Compile and create all shader
		if ( FAILED( hr = D3DX10CompileFromMemory( szVS, sizeof(szVS), NULL, NULL, NULL, "main", szVSProfile, 0, 0, NULL, &pVSBuf, NULL, NULL )) )
			throw TRException("CompileShaderFromMemory Failed.", hr, RESULT_FAIL);

		if ( FAILED( hr = m_pDevice->CreateVertexShader( (DWORD*) pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), NULL, &m_pVS ) ) )
			throw TRException("CreateVertexShader Failed.", hr, RESULT_FAIL);

		if ( FAILED( hr = D3DX10CompileFromMemory( szPS, sizeof(szPS), NULL, NULL, NULL, "main", szPSProfile, 0, 0, NULL, &pPSBuf, NULL, NULL )) )
			throw TRException("CompileShaderFromMemory Failed.", hr, RESULT_FAIL);

		if ( FAILED( hr = m_pDevice->CreatePixelShader( (DWORD*) pPSBuf->GetBufferPointer(), pPSBuf->GetBufferSize(), NULL, &m_pPS ) ) )
			throw TRException("CreatePixelShader Failed.", hr, RESULT_FAIL);

		m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
		m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );

		// Define our vertex data layout
		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "pos",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "fdata", 0, DXGI_FORMAT_R32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "udata", 0, DXGI_FORMAT_R32_UINT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "idata", 0, DXGI_FORMAT_R32_SINT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if(	FAILED(	hr = m_pDevice->CreateInputLayout( layout, 4, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &m_pVertexLayout ) ) )
			throw TRException("CreateInputLayout for m_pVertexLayout Failed.", hr, RESULT_FAIL);

		m_pDeviceContext->IASetInputLayout( m_pVertexLayout );

		// copy vertex buffer
		memcpy( &m_VBData[0], VB, ARRAY_SIZE(VB) * sizeof(Vertex) );
		CreateVB();

		//
		// Create index buffer
		//
		D3D11_BUFFER_DESC IBDesc =
		{
			100,
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_INDEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0
		};
		if( FAILED( hr = m_pDevice->CreateBuffer( &IBDesc, NULL, &m_pIB ) ) )
			throw TRException("CreateBuffer for m_pIB Failed.", hr, RESULT_FAIL);
		m_pDeviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0 );

		//
		// setup const data
		//
		D3D11_MAPPED_SUBRESOURCE bufData;
		if( FAILED( hr = GetEffectiveContext( )->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &bufData ) ) )
			throw TRException("Map on m_pConstBuffer Failed", hr, RESULT_FAIL);

		// Inverse viewport scale.
		((float*)bufData.pData)[2] = 2 / (float)m_uRTWidth;
		((float*)bufData.pData)[3] = -2 / (float)m_uRTHeight;
		// Prescaled inverse viewport translation.
		((float*)bufData.pData)[0] = -1.0f;
		((float*)bufData.pData)[1] = 1.0f;

		GetEffectiveContext( )->Unmap( m_pConstBuffer, 0 );

		//
		// setup box for texture copy
		//
		m_TexBox.back = 1;
		m_TexBox.front = 0;
		m_TexBox.left = 0;
		m_TexBox.right = m_uRTWidth;
		m_TexBox.top = 0;		
		m_TexBox.bottom = m_uRTHeight;

	}
	catch (TRException& exc)
	{
		SAFE_RELEASE( pVSBuf );
		SAFE_RELEASE( pPSBuf );	
		exc.Prepend("[CRasterizerTest::SetupD3D()] ");
		throw;
	}

	SAFE_RELEASE( pVSBuf );
	SAFE_RELEASE( pPSBuf );

	if (tRes != RESULT_PASS)
		return tRes; 

	return SetupRenderTarget();
}

void CRasterizerTest::CreateVB()
{
	HRESULT hr;
	// unbind and release
	ID3D11Buffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };
	m_pDeviceContext->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );
	SAFE_RELEASE( m_pVB );

	//
	// Create vertex buffer
	//
	D3D11_BUFFER_DESC VBDesc =
	{
		m_VBData.size() * sizeof( Vertex ),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0
	};
	D3D11_SUBRESOURCE_DATA initialData = { &m_VBData[0], 0, 0 };
	if( FAILED( hr = m_pDevice->CreateBuffer( &VBDesc, &initialData, &m_pVB ) ) )
		throw TRException("CreateBuffer for m_pVB Failed.", hr, RESULT_FAIL);

	UINT strides[] = { sizeof( Vertex ) };
	m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVB, strides, offsets );
}

TEST_RESULT CRasterizerTest::SetupRenderTarget()
{
	try {
		HRESULT hr;
		UINT32 uSupport;
		m_pDevice->CheckFormatSupport(m_RTFormat, &uSupport);
		if (!(uSupport & D3D10_FORMAT_SUPPORT_RENDER_TARGET))
			throw TRException("Format not supported as render target", RESULT_SKIP);

		if(m_uRTSampleCount > 1)
		{
			if(GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_1)
				throw TRException("Sample frequency tests only run on feature-level >= 10_1", RESULT_SKIP);

			UINT NumQualityLevels = 0;
			if(SUCCEEDED(m_pDevice->CheckMultisampleQualityLevels(m_RTFormat, m_uRTSampleCount, &NumQualityLevels)))
			{
				if(0 == NumQualityLevels)
					throw TRException("MSAA format not supported", RESULT_SKIP);
			}
			else
			{
				throw TRException("CheckMultisampleQualityLevels failed", RESULT_SKIP);
			}
		}


		//
		// Setup RenderTarget
		// The D3D10_CENTER_MULTISAMPLE_PATTERN pattern is used because the test data only works at pixel centers
		//
		D3D11_TEXTURE2D_DESC tex2ddesc;
		tex2ddesc.ArraySize = 1;
		tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex2ddesc.Format = m_RTFormat;
		tex2ddesc.Height = m_uRTHeight;
		tex2ddesc.Width = m_uRTWidth;
		tex2ddesc.CPUAccessFlags = 0;
		tex2ddesc.MipLevels = 1;
		tex2ddesc.MiscFlags = 0;
		tex2ddesc.SampleDesc.Count = m_uRTSampleCount;
		tex2ddesc.SampleDesc.Quality = (m_uRTSampleCount > 1) ? D3D10_CENTER_MULTISAMPLE_PATTERN : 0;
		tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

		if ( FAILED( hr = m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pRTTexture2D) ) )
			throw TRException("CreateTexture2D failed for m_pRTTexture2D.", hr, RESULT_FAIL);

		tex2ddesc.BindFlags = 0;
		tex2ddesc.Usage = D3D11_USAGE_STAGING;
		tex2ddesc.SampleDesc.Count = 1;
		tex2ddesc.SampleDesc.Quality = 0;
		tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		
		if ( FAILED( hr = m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pTexA ) ) )
			throw TRException("CreateTexture2D failed for m_pRTTexture2DCopy.", hr, RESULT_FAIL);
		
		if ( FAILED( hr = m_pDevice->CreateTexture2D( &tex2ddesc, NULL, &m_pTexB ) ) )
			throw TRException("CreateTexture2D failed for m_pRTTexture2DCopy.", hr, RESULT_FAIL);

		if ( FAILED( hr = m_pDevice->CreateRenderTargetView( m_pRTTexture2D, NULL, &m_pRTView ) ) )
			throw TRException("CreateRenderTargetView failed for m_pRTTexture2D and m_pRTView.", hr, RESULT_FAIL);

		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRTView, NULL );
	}
	catch (TRException& exc)
	{
		exc.Prepend("[CRasterizerTest::SetupRenderTarget()] ");
		throw;
	}

	return RESULT_PASS;
}


// fill index buffer with test case data
void CRasterizerTest::FillIndexBuffer(IndexBuffer *pBuffer)
{
	HRESULT hr;
	assert(pBuffer && pBuffer->size > 0);

	D3D11_MAPPED_SUBRESOURCE bufData;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pIB, 0, D3D11_MAP_WRITE_DISCARD, NULL, &bufData ) ) )
		throw TRException("Map on m_pIB Failed", hr, RESULT_FAIL);
	assert(pBuffer->size);
	memcpy(bufData.pData, pBuffer->data, pBuffer->size*sizeof(UINT));
	GetEffectiveContext()->Unmap(m_pIB,0);
}

void CRasterizerTest::CleanupRasterizerTestCase()
{
	GetEffectiveContext()->RSSetState( NULL );
	SAFE_RELEASE( m_pRSEnabled );
}

UINT CRasterizerTest::CompareBuffers()
{
	HRESULT hr;
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
		
	D3D11_MAPPED_SUBRESOURCE texA, texB;
	if( FAILED( hr = GetImmediateContext()->Map( m_pTexA, 0, D3D11_MAP_READ, 0, &texA ) ) )
		throw TRException("Failed Map on m_pTexA.", hr, RESULT_FAIL);
	if( FAILED( hr = GetImmediateContext()->Map( m_pTexB, 0, D3D11_MAP_READ, 0, &texB ) ) )
		throw TRException("Failed Map on m_pTexB.", hr, RESULT_FAIL);

	CBufferCompare comp;
	UINT diff = comp.Compare(texA.pData, texB.pData, m_RTFormat, 0, 4, m_uRTWidth, m_uRTHeight, texA.RowPitch, texB.RowPitch );

	GetImmediateContext()->Unmap(m_pTexB,0);
	GetImmediateContext()->Unmap(m_pTexA,0);

	return diff;
}

void CRasterizerTest::BeginQuery()
{
	HRESULT hr;
	// setup query and predicate
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_OCCLUSION;
	desc.MiscFlags = 0;
	if ( FAILED( hr = m_pDevice->CreateQuery( &desc, &m_pQuery ) ) ) 
		throw TRException("CreateQuery failed.", hr, RESULT_FAIL);

	GetEffectiveContext()->Begin(m_pQuery);
}

UINT64 CRasterizerTest::EndQuery()
{
	GetEffectiveContext()->End(m_pQuery);
	UINT64 uQResult;
	HRESULT hr;

	if ( FAILED( hr = ExecuteEffectiveContext() ) )
		throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
	
	// wait for results
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pQuery, (void*)&uQResult, sizeof(UINT64), 0 );

	// bad result
	if (hr != S_OK)
	{
		WriteToLog("Query HRESULT = %s", D3DHResultToString(hr).c_str());
		throw TRException("Query failed or timed out.", RESULT_FAIL);
	}

	// cleanup
	SAFE_RELEASE( m_pQuery );
	
	return uQResult;
}	

// Called once per group
void CRasterizerTest::Cleanup()
{
	CleanupD3D();
}

void CRasterizerTest::CleanupD3D()
{
	ID3D11RenderTargetView* pRTNull[] = { NULL, NULL, NULL, NULL };
	ID3D11Buffer* pBufferNull[] = { NULL };
	ID3D11Buffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };

	if ( m_pDevice ) //otherwise, we failed to create the device in the first place
	{
		m_pDeviceContext->OMSetBlendState( NULL, 0, D3D10_DEFAULT_SAMPLE_MASK );
		m_pDeviceContext->IASetInputLayout( NULL );
		m_pDeviceContext->IASetVertexBuffers( 0, 1, pNullBuffers, offsets, offsets );
		m_pDeviceContext->IASetIndexBuffer( 0, DXGI_FORMAT_R32_UINT, 0 );
		m_pDeviceContext->VSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->PSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->OMSetRenderTargets( 4, pRTNull, NULL );
		m_pDeviceContext->VSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDeviceContext->GSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDeviceContext->PSSetConstantBuffers( 0, 1, pBufferNull );
		m_pDeviceContext->RSSetState( NULL );
		m_pDeviceContext->SOSetTargets( 1, pNullBuffers, offsets );

		SAFE_RELEASE( m_pVS );
		SAFE_RELEASE( m_pPS );

		SAFE_RELEASE( m_pRTView );
		SAFE_RELEASE( m_pRTTexture2D );
		SAFE_RELEASE( m_pTexA );
		SAFE_RELEASE( m_pTexB );
		SAFE_RELEASE( m_pConstBuffer );

		SAFE_RELEASE( m_pRSEnabled );
		SAFE_RELEASE( m_pBSLogicOR );

		SAFE_RELEASE( m_pVertexLayout );
		SAFE_RELEASE( m_pVB );
		SAFE_RELEASE( m_pIB );
	}
}

void CRasterizerTest::Present()
{
	// todo
}

UINT CRasterizerTest::GetIBIndex()
{
	return (UINT) ( &(*m_pBuffer) - &IB[0] );
}

void CRasterizerTest::WriteIBToLog()
{
	if (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST == m_pBuffer->top)
		WriteToLog("TRIANGLELIST");
	else if (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP == m_pBuffer->top)
		WriteToLog("TRIANGLESTRIP");	

	WriteToLog("vertex list:");
	for (UINT i = 0; i < m_pBuffer->size; ++i)
	{
		float x = VB[m_pBuffer->data[i]].pos[0];
		float y = VB[m_pBuffer->data[i]].pos[1];
	
		WriteToLog("[%d] x:%f, y:%f", i, x, y);
	}
}

tstring __stdcall ToString( IndexBuffer *type )
{
	// get array index number
	UINT index = (UINT) ( &(*type) - &IB[0] );

	// put index into string
	char result[3];
	result[1] = '\0';
	_snprintf( result, 2, "%d", index );
	result[2] = '\0';

	return tstring(result);
}

bool __stdcall FromString( IndexBuffer **pType, const tstring &str )
{
	// convert string to index
	int index  = atoi( str.c_str() );
	
	assert(index >= 0);
	assert(index < ARRAY_SIZE(IB));

	*pType = &IB[index];

	return true;
}
