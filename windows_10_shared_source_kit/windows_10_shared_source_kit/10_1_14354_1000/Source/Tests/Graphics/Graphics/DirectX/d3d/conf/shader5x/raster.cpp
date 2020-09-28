//////////////////////////////////////////////////////////////////////////
//  Raster.cpp
//  created:	2005/06/29
//
//  purpose: Tests Raster related shader instructions such as deriv_rtx and deriv_rty
//////////////////////////////////////////////////////////////////////////

//#include <D3D10ValueSets.h>
//#include <DXGIValueSets.h>
#include "Raster.h"

#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"

char CShader5xTest_deriv::m_szVSCodeBegin[] =
	"dcl_temps 4"						"\n" 
	"dcl_constantBuffer c0[20], dynamicIndexed"			"\n"
	"dcl_input v[0].xy"					"\n"	// in Position
	"dcl_input v[1].xyzw"				"\n"	// in Texture coordinates
	"dcl_output_sv o[0].xyzw, position"	"\n"	// out Position
	"dcl_output_typed o[1].xyzw, float"	"\n"	// out Texture coordinates or Result
	"dcl_output o[2].xyzw"				"\n"	// out TL Position (used for ld instruction in GS)
	;

char CShader5xTest_deriv::m_szVSCodeEnd[] =
	"mad r2.xy, v[0].xy, c0[0].zwww, c0[0].xyyy""\n"
	"mov r2.zw, vec4( 0.0, 0.0, 0.0, 1.0 )"		"\n"
	"mov o[ 0 ], r2"							"\n"
	"mov o[ 1 ], r1"							"\n"
	"mov o[ 2 ], v[ 0 ].xy"					"\n"
	;

char CShader5xTest_deriv::m_szPSCodeBegin[] =
	"dcl_temps 4"								"\n"
	"dcl_constantBuffer c0[20], dynamicIndexed"			"\n"
	"dcl_input_ps_sv v[0].xyzw, linear_noperspective, position""\n"	// in Position
	"dcl_input_ps v[1].xyzw, linear"			"\n"	// in Texture coordinates or PassThrough Result
	"dcl_output_typed o[0].xyzw,float"			"\n"
	"dcl_sampler s0, mode_default "				"\n"
;

char CShader5xTest_deriv::m_szPSCodeEnd[] =
	"mov o[0], r1"		"\n"
	;


CShader5xTest_deriv::CShader5xTest_deriv( ) :
	m_pTexture( NULL ),
	m_pTextureRT( NULL ),
	m_pTextureRTCopy( NULL ),
	m_pBufferTL( NULL ),
	m_pLayoutTL( NULL ),
	m_pSampler( NULL ),
	m_pSRV( NULL ),
	m_pRTV( NULL ),
	m_pVShader( NULL ),
	m_pGShader( NULL ),
	m_pPShader( NULL ),
	m_pExpectedResult( NULL ),
	m_pResourceData( NULL ),
	m_Format( TEXTURE_FORMAT ),
	m_nPrimitiveSize( PRIMITIVE_SIZE )
{
}

void CShader5xTest_deriv::InitTestParameters( )
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >( );
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );

	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >( );
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );

	DWORD dwInfP = 0x7f800000;	// +INF
	DWORD dwInfN = 0xff800000;  // -INF
	DWORD dwNaN = 0x7f800001;	// NaN

	CUserValueSet< float > *pDerivValues = new CUserValueSet< float >( );
	pDerivValues->AddValue( 1.f );
	pDerivValues->AddValue( -1.f );
	pDerivValues->AddValue( 0.f );
	pDerivValues->AddValue( (float)( rand( ) % RANDOM_MAX ) );
	pDerivValues->AddValue( -(float)( rand( ) % RANDOM_MAX ) );
	pDerivValues->AddValue( *( (float*) &dwInfP ));  
	pDerivValues->AddValue( *( (float*) &dwInfN ));  
	pDerivValues->AddValue( *( (float*) &dwNaN ));  

	testfactor::RFactor rfSrcMod = AddParameter( "SourceMod", &m_SrcMod, pSrcModValues );
	testfactor::RFactor rfInstMod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor rfDeriv = AddParameter( "Derivative", &m_fDerivative, pDerivValues );

	SetRootTestFactor( rfSrcMod * rfInstMod * rfDeriv );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("SourceMod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("SourceMod"), SMOD_ABSNEG), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

TEST_RESULT CShader5xTest_deriv::Setup( )
{
	HRESULT hr;
	TEST_RESULT tr = CShader5xTest::Setup();
	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
		return tr;

	if (!SupportsFeatureLevel( g_TestApp.m_D3DOptions.FeatureLevel ))
		return RESULT_SKIP;

	//create default vertex shader
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	const char* szVSProfile = g_TestApp.GetShaderProfile(D3D_SHADER_STAGE_VERTEX, 
		g_TestApp.m_D3DOptions.FeatureLevel, g_TestApp.m_D3DOptions.FeatureLevel);

	char szCode [ MAX_STR ];
	_snprintf( szCode, MAX_STR  - 1, "%s\n%smov r1, v[ 1 ]\n%s", szVSProfile, m_szVSCodeBegin, m_szVSCodeEnd);
	szCode[MAX_STR - 1] = 0; // Make Prefast Happy

	if( !AssembleShader( szCode, &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "CShader5xTest_deriv::Setup - AssembleShader() failed assembling default VS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		tr = RESULT_FAIL;
		goto setupDone;
	}
	
	SAFE_RELEASE( m_pPassThroughVS );
	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPassThroughVS ) ) )
	{
		WriteToLog( _T( "CShader5xTest_deriv::Setup - CreateVertexShader() failed creating m_pVS. hr = %s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	if( g_bDumpDissasm )
	MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	//declare buffer used for input data and setup input assembler
	D3D11_INPUT_ELEMENT_DESC DeclTL[] =
	{
		// really float, but assembly expects uint
		{"0_", 0, DXGI_FORMAT_R32G32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
		{"1_", 0, DXGI_FORMAT_R32G32_UINT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},  //texture coordinates
	};
	if(	FAILED(	hr = GetDevice()->CreateInputLayout( DeclTL, sizeof(DeclTL) / sizeof(DeclTL[ 0 ]), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayoutTL ) ) )
	{
		WriteToLog(_T("CShader5xTest_deriv::Setup - CreateInputLayout() failed. hr=%s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}
	GetEffectiveContext()->IASetInputLayout( m_pLayoutTL );

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	m_pExpectedResult = new float[32 * 32 * 4];
	FillMemory( (BYTE*)m_pExpectedResult, 32 * 32 * 4 * sizeof(float), 0);//0x81);

	//setup view port
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width    = 32;
    vp.Height   = 32;
    vp.MinDepth     = 0;
    vp.MaxDepth     = 1;
    GetEffectiveContext()->RSSetViewports(1, &vp);

	//set const buffer to use with TL vertices
	
	// The coordinates are given in screen-space.
	// We must compensate for the viewport transform in the shader.
	// This step can be reduced to a single shader mad instruction
	// by prescaling the inverse viewport translation by the
	// inverse viewport scale.
	
	D3DXMatrixIdentity( &m_MatWorld );
	float transx = vp.Width / 2.f;
	float transy = vp.Height / 2.f;

	// Inverse viewport scale.
	m_MatWorld._13 = 1 / transx;
	m_MatWorld._14 = -1 / transy;

	// Prescaled inverse viewport translation.
	m_MatWorld._11 = -transx * m_MatWorld._13;
	m_MatWorld._12 = -transy * m_MatWorld._14;

	if (FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, m_MatWorld)))
	{
		WriteToLog(_T("CShader5xTest_deriv::Setup - Map() for m_pConstBuffer failed. hr=%s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	//setup sampler
	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.BorderColor[0] = 0.5f;
	SamplerDesc.BorderColor[1] = 0.5f;
	SamplerDesc.BorderColor[2] = 0.5f;
	SamplerDesc.BorderColor[3] = 1.0f;
	SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = 5;

	if( FAILED( hr = GetDevice()->CreateSamplerState(&SamplerDesc, &m_pSampler ) ))
	{
		WriteToLog( _T("CShader5xTest_deriv::Setup - CreateSamplerState() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	if( !CreateGeometry( ) )
	{
		tr = RESULT_FAIL;
		goto setupDone;
	}

	tr = SetupTextures( );
	if( tr != RESULT_PASS )
		goto setupDone;

	GetEffectiveContext()->VSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->GSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->PSSetSamplers(0, 1, &m_pSampler);

	GetEffectiveContext()->VSSetShader( m_pPassThroughVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_deriv::Setup - ExecuteEffectiveContext() failed");
		tr = RESULT_FAIL;
		goto setupDone;
	}

setupDone:
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );
	return tr;
}

void CShader5xTest_deriv::Cleanup( )
{
	CShader5xTest::Cleanup();

	ID3D11Buffer *pBufferNull[] = { NULL, NULL, NULL, NULL };
	ID3D11SamplerState *pSamplerNull[] = { NULL, NULL, NULL, NULL };
	UINT strides[] = { 0, 0, 0, 0 };
	UINT offsets[] = { 0, 0, 0, 0 };

	if( GetEffectiveContext() )
	{
		GetEffectiveContext()->VSSetSamplers(0, 1, pSamplerNull);
		GetEffectiveContext()->GSSetSamplers(0, 1, pSamplerNull);
		GetEffectiveContext()->PSSetSamplers(0, 1, pSamplerNull);
		GetEffectiveContext()->IASetVertexBuffers( 0, 4, pBufferNull, strides, offsets );
		GetEffectiveContext()->IASetInputLayout( NULL );

		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
	}
	
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pVShader );

	SAFE_RELEASE( m_pLayoutTL );
	SAFE_RELEASE( m_pBufferTL );
	SAFE_RELEASE( m_pSampler );
	SAFE_RELEASE( m_pTexture );
	SAFE_RELEASE( m_pTextureRT );
	SAFE_RELEASE( m_pTextureRTCopy );
	SAFE_RELEASE( m_pRTV );
	
	delete[] m_pExpectedResult;

	delete [ ] m_pResourceData;
}

TEST_RESULT CShader5xTest_deriv::SetupTestCase( )
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedConstBuffer;
	if( FAILED( hr = GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedConstBuffer ) ) )
	{
		WriteToLog( _T( "CShader5xTest_deriv::SetupTestCase - Map() failed. hr=%s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	float *pConstData = (float*)mappedConstBuffer.pData;

	D3DXVECTOR4 vecDeriv( m_fDerivative, m_fDerivative, m_fDerivative, m_fDerivative );
	memcpy( pConstData, &m_MatWorld, sizeof( D3DXMATRIX ) );
	memcpy( ((BYTE*)pConstData) + sizeof(D3DXMATRIX), &vecDeriv, sizeof(D3DXVECTOR4) );

	GetEffectiveContext()->Unmap( m_pConstBuffer, 0 );

	tstring szInstMod, szNeg, szAbs;
	switch( m_InstMod )
	{
	case IMOD_SAT:
		szInstMod = "_sat";
		break;
	default:
		szInstMod = "";
		break;
	}

	switch( m_SrcMod )
	{
	case SMOD_NEG:
		szNeg = "-";
		szAbs = "r0.x";
		break;
	case SMOD_ABS:
		szNeg = "";
		szAbs = "abs( r0.x )";
		break;
	case SMOD_ABSNEG:
		szNeg = "-";
		szAbs = "abs( r0.x )";
		break;
	default:
		szNeg = "";
		szAbs = "r0.x";
		break;
	}

	tstring szInstName = GetInstName( g_TestApp.m_D3DOptions.FeatureLevel );
	const bool bDerivX = szInstName.substr(0, 9) == _T( "deriv_rtx" );
	const char* szPSProfile = g_TestApp.GetShaderProfile(D3D_SHADER_STAGE_PIXEL, 
		g_TestApp.m_D3DOptions.FeatureLevel, g_TestApp.m_D3DOptions.FeatureLevel);

	tstring shaderCode = szPSProfile + tstring("\n") + m_szPSCodeBegin;
	shaderCode += FormatString("mov r0.x, v[1].%s \n", bDerivX ? "x" : "y"); // texCoord.x or texCoord.y
	shaderCode += "mul r0.x, r0.x, r0.x \n"; // (texCoord)^2
	shaderCode += "mul r0.x, r0.x, c0[4].x \n"; // multiply by constant derivative factor (1, INF, NaN, etc)
	shaderCode += szInstName + szInstMod;
	shaderCode += FormatString(" r1, %s%s\n", szNeg.c_str( ), szAbs.c_str( ) );
	shaderCode += m_szPSCodeEnd;
	
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	if( !AssembleShader( shaderCode.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "CShader5xTest_deriv::SetupTestCase - AssembleShader() failed assembling PS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		return RESULT_FAIL;
	}
	
	SAFE_RELEASE( m_pPShader );
	if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPShader ) ) )
	{
		WriteToLog( _T( "CShader5xTest_deriv::Setup - CreateVertexShader() failed creating m_pPShader. hr = %s"), D3DHResultToString(hr).c_str() );
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		return RESULT_FAIL;
	}

	if( g_bDumpDissasm )
	MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	GetEffectiveContext()->PSSetShader( m_pPShader, NULL, 0 );

	FLOAT colors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GetEffectiveContext()->ClearRenderTargetView( m_pRTV, colors );

	if( !CalculateResult( ) )
		return RESULT_FAIL;
	
	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_deriv::Setup - ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void CShader5xTest_deriv::CleanupTestCase( )
{
	ID3D11ShaderResourceView *pSRVNull[] = { NULL, NULL, NULL, NULL };

	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
	GetEffectiveContext()->PSSetShaderResources( 0, 1, pSRVNull );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
	}

	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pGShader );
	SAFE_RELEASE( m_pPShader );

	FillMemory( (BYTE*)m_pExpectedResult, 32 * 32 * 4 * sizeof(float), 0);//0x81);
}

TEST_RESULT CShader5xTest_deriv::ExecuteTestCase( )
{
	GetEffectiveContext( )->Draw( 4, 0 );
	
	GetEffectiveContext()->CopySubresourceRegion( m_pTextureRTCopy, 0, 0, 0, 0, m_pTextureRT, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_deriv::ExecuteTestCase - ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE texMap;
	
	GetImmediateContext()->Map( m_pTextureRTCopy, 0, D3D11_MAP_READ, 0, &texMap );

	bool bResult = CompareResults( texMap.pData, texMap.RowPitch );

	GetImmediateContext()->Unmap( m_pTextureRTCopy, 0 );

	return bResult ? RESULT_PASS : RESULT_FAIL;
}

bool CShader5xTest_deriv::CreateGeometry( )
{
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.ByteWidth      = 4 * sizeof( TLVERTEX );
	buffer_desc.Usage          = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags      = 0;

	HRESULT hr;
	if( FAILED( hr = GetDevice()->CreateBuffer( &buffer_desc, NULL, &m_pBufferTL) ) )
	{
		WriteToLog( _T("CShader5xTest_deriv::Setup - CreateBuffer() for Stream Input Buffer failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	UINT StridesTL = sizeof( TLVERTEX );
	UINT Offset = 0;
	
	TLVERTEX *vTL = NULL;

	vTL = new TLVERTEX[ 4 ];
	
	TLVERTEX vTL_Temp[] = {
		{{0.f,						(float)m_nPrimitiveSize},	{0.f, 2.f} },
		{{0.f,						0.f},						{0.f, 0.f} },
		{{(float)m_nPrimitiveSize,	(float)m_nPrimitiveSize},	{1.f, 2.f} },
		{{(float)m_nPrimitiveSize,	0.f},						{1.f, 0.f} }
	};
	memcpy( vTL, vTL_Temp, sizeof(vTL_Temp) );

	GetEffectiveContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE mappedDataTL;
	if( FAILED(hr = GetEffectiveContext()->Map( m_pBufferTL, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataTL)) ) 
	{
		WriteToLog( _T("CShader5xTest_deriv::CreateGeometry - Map() for Stream Input Buffer failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return false;
	}
	memcpy(mappedDataTL.pData, vTL, 4 * sizeof(TLVERTEX));
	GetEffectiveContext()->Unmap(m_pBufferTL, 0);

	delete [ ] vTL;

	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pBufferTL, &StridesTL, &Offset);

	return true;
}

TEST_RESULT CShader5xTest_deriv::SetupTextures( )
{
	SAFE_RELEASE( m_pTexture );
	
	int nTextureSize = (int) m_nPrimitiveSize;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width              = nTextureSize;
	texDesc.Height             = nTextureSize;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.Format             = m_Format;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	UINT32 uFormatCaps;
	GetDevice()->CheckFormatSupport( m_Format, &uFormatCaps);
	if ( !(uFormatCaps & D3D11_FORMAT_SUPPORT_TEXTURE2D) || !(uFormatCaps & D3D11_FORMAT_SUPPORT_RENDER_TARGET) )
	{
		WriteToLog( _T( "CShader5xTest_deriv::SetupTextures - device does not support test texture format" ) );	
		return RESULT_SKIP;
	}

	m_pResourceData = new BYTE[ sizeof(float) * 4 * PRIMITIVE_SIZE * PRIMITIVE_SIZE ];
	D3D11_SUBRESOURCE_DATA data;
	data.SysMemPitch = sizeof(float) * 4 * PRIMITIVE_SIZE;
	data.SysMemSlicePitch = sizeof(float) * 4 * PRIMITIVE_SIZE;
	data.pSysMem = m_pResourceData;

	for( UINT j = 0; j < (UINT)nTextureSize; ++j )
	{
		for( UINT i = 0; i < (UINT)nTextureSize * 4; ++i )
		{
			float val = (float)(rand( ) % RANDOM_MAX);
			memcpy( (m_pResourceData) + (j * data.SysMemPitch) + (i * 4), &val, sizeof( float ) );
		}
	}
	
	HRESULT hr;
	if( FAILED( hr = GetDevice( )->CreateTexture2D( &texDesc, &data, &m_pTexture ) ) )
	{
		WriteToLog( _T( "CShader5xTest_deriv::SetupTextures - failed to create source texture. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// create render target
	texDesc.BindFlags			= D3D11_BIND_RENDER_TARGET;
	if( FAILED( hr = GetDevice( )->CreateTexture2D( &texDesc, NULL, &m_pTextureRT ) ) )
	{
		WriteToLog( _T( "CShader5xTest_deriv::SetupTextures - failed to create render target. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// create copy of render target for reading
	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	texDesc.MiscFlags = 0;
	if( FAILED( hr = GetDevice( )->CreateTexture2D( &texDesc, NULL, &m_pTextureRTCopy ) ) )
	{
		WriteToLog( _T( "CShader5xTest_deriv::SetupTextures - failed to create copy of render target. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format						= m_Format;
	srv.ViewDimension			= D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels			= 1;
	srv.Texture2D.MostDetailedMip	= 0;

	hr = GetDevice()->CreateShaderResourceView(m_pTexture, &srv, &m_pSRV );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_deriv::SetupTextures - CreateShaderResourceView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->PSSetShaderResources( 0, 1, &m_pSRV );
	
	D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
//	rtviewdesc.Texture2D.ArraySize = 1;
//	rtviewdesc.Texture2D.FirstArraySlice = 0;
	rtviewdesc.Texture2D.MipSlice = 0;
	rtviewdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	GetDevice()->CreateRenderTargetView( m_pTextureRT, &rtviewdesc, &m_pRTV );
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, NULL );

	return RESULT_PASS;
}

bool CShader5xTest_deriv::CalculateResult( )
{
	float *pExpectedResult = m_pExpectedResult;
	
	tstring szInstName = GetInstName( g_TestApp.m_D3DOptions.FeatureLevel );
	const bool bDerivX = szInstName.substr(0, 9) == _T( "deriv_rtx" );

	const bool bSat = (m_InstMod == IMOD_SAT);

	switch( m_SrcMod )
	{
	case SMOD_NEG:
		m_fDerivative = -m_fDerivative;
		break;
	case SMOD_ABS:
		m_fDerivative = fabsf( m_fDerivative );
		break;
	case SMOD_ABSNEG:
		m_fDerivative = -fabsf( m_fDerivative );
		break;
	}

	float fEven, fOdd;
	for( UINT i = 0; i < (UINT) m_nPrimitiveSize; ++i )
	{
		for( UINT j = 0; j < (UINT) m_nPrimitiveSize; ++j )
		{
			if( bDerivX )
			{
				if( j & 1 ) // odd
				{
					fOdd = (float) (j + 0.5f) / (float) m_nPrimitiveSize;
					fEven = (float) (j + 0.5f - 1) / (float) m_nPrimitiveSize;
				}
				else		// even
				{
					fEven = (float) (j + 0.5f) / (float) m_nPrimitiveSize;
					fOdd = (float) (j + 0.5f + 1) / (float) m_nPrimitiveSize;
				}
			}
			else
			{
				if( i & 1 )	// odd
				{
					fOdd = 2 * (float) (i + 0.5f) / (float) m_nPrimitiveSize;
					fEven = 2 * (float) (i + 0.5f - 1) / (float) m_nPrimitiveSize;
				}
				else		// even
				{
					fEven = 2 * (float) (i + 0.5f) / (float) m_nPrimitiveSize;
					fOdd = 2 * (float) (i + 0.5f + 1) / (float) m_nPrimitiveSize;
				}
			}

			// square the inputs
			fOdd = fOdd * fOdd;
			fEven = fEven * fEven;
			
			float deriv = fOdd - fEven;
			float result = (m_fDerivative * fOdd) - (m_fDerivative * fEven);
			if( bSat )
			{
				if( IsNaN( result ) || // NaN
					*( (UINT32*) &result ) == 0xffc00000 ) // -INF
				{
					result = 0.f;
				}
				else
				{
					result = min( 1.f, max( 0.f, result ) );
				}
			}
			float pOutput [ ] = { result, result, result, result };

			size_t idx = (i * m_nPrimitiveSize * 4) + (j * 4);
			for (int n = 0; n < 4; ++n)
				pExpectedResult[ idx + n ] = pOutput[ n ];
		}
	}

	return true;
}

bool CShader5xTest_deriv::IsNaN( float fValue )
{
	UINT32 uVal = *( (UINT32 *) &fValue ) & 0x7fffffff;
	return uVal >= 0x7f800001 && uVal <= 0x7fffffff;
}

bool CShader5xTest_deriv::CompareResults( void *pOutput, const SIZE_T &uPitch )
{
	
	float *pExpected = m_pExpectedResult;
	float *pResult = (float*) pOutput;

	bool bPass = true;
	bool bLogAll = ((CShader5xTestApp*)GetFramework())->LogVerbose();

	UINT nSizeX = (m_nPrimitiveSize) * 4;
	UINT nSizeY = (m_nPrimitiveSize);

	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(DXGI_FORMAT_R32_FLOAT);
	for( UINT i = 0; i < nSizeY; ++i )
	{
		for( UINT j = 0; j < nSizeX; ++j )
		{
			size_t idx = i * ( 4 * PRIMITIVE_SIZE ) + j;
			float fE = pExpected[ idx ];
			float fRes = *( (float*)(((BYTE*)pResult) + (i * uPitch) + (j * 4) ));
			//if( *( (UINT32*) &pExpected[ idx ] ) != *( (UINT32*) &fRes ) )
			float ulpDiffAllowed = 3.0f;
			float ulpDiffObserved = 0.0f;
			if(    !(IsNaN(fE) && IsNaN(fRes)) 
				&& !MinMaxRange::TestFloatsSame(fE, fRes, pTexFormatInfo, 0, ulpDiffAllowed, &ulpDiffObserved)
				)
			{
				if( bPass || bLogAll )
				{
					bPass = false;
					WriteToLog( _T( "CShader5xTest_deriv::CompareResults - Incorrect result at pixel (%d,%d)\n\tExpected: %#x (%g)\n\tOutput:   %#x (%g)\n\tULP Diff: %g" ), 
						j, i, *( (UINT32*) &pExpected[ idx ] ), pExpected[ idx ], *( (UINT32*) &fRes ), fRes, ulpDiffObserved );
				}
			}
		}
	}
	

	return bPass;
}
