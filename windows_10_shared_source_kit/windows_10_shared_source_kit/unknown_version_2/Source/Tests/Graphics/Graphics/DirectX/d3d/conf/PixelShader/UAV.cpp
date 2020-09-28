#include "PixelShader.h" 
#include "D3DDepthHelper.h"
#include "D3DMSHelper.h"

BEGIN_NAMED_VALUES( UAV_TYPE )
	NAMED_VALUE( _T( "RAW" ),        UAV_RAW )
	NAMED_VALUE( _T( "TYPED" ),      UAV_TYPED )
	NAMED_VALUE( _T( "STRUCTURED" ), UAV_STRUCTURED )
	NAMED_VALUE( _T( "APPEND" ),     UAV_APPEND )
	NAMED_VALUE( _T( "COUNTER" ),    UAV_COUNTER )
END_NAMED_VALUES( UAV_TYPE )

BEGIN_NAMED_VALUES( UAV_ACCESS_METHOD )
	NAMED_VALUE( _T( "UAV_ACCESS_STORE" ),                  UAV_ACCESS_STORE )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_AND" ),             UAV_ACCESS_ATOMIC_AND )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_XOR" ),             UAV_ACCESS_ATOMIC_XOR )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_OR" ),              UAV_ACCESS_ATOMIC_OR )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_CMP_STORE" ),       UAV_ACCESS_ATOMIC_CMP_STORE )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_EXCHANGE" ),        UAV_ACCESS_ATOMIC_EXCHANGE )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_CMP_EXCHANGE" ),    UAV_ACCESS_ATOMIC_CMP_EXCHANGE )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_ADD" ),             UAV_ACCESS_ATOMIC_ADD )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_IMAX" ),            UAV_ACCESS_ATOMIC_IMAX )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_IMIN" ),            UAV_ACCESS_ATOMIC_IMIN )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_UMAX" ),            UAV_ACCESS_ATOMIC_UMAX )
	NAMED_VALUE( _T( "UAV_ACCESS_ATOMIC_UMIN" ),            UAV_ACCESS_ATOMIC_UMIN )
	NAMED_VALUE( _T( "UAV_ACCESS_IMM_ATOMIC_AND" ),         UAV_ACCESS_IMM_ATOMIC_AND )
	NAMED_VALUE( _T( "UAV_ACCESS_IMM_ATOMIC_XOR" ),         UAV_ACCESS_IMM_ATOMIC_XOR )
	NAMED_VALUE( _T( "UAV_ACCESS_IMM_ATOMIC_OR" ),          UAV_ACCESS_IMM_ATOMIC_OR )
	NAMED_VALUE( _T( "UAV_ACCESS_IMM_ATOMIC_ADD" ),         UAV_ACCESS_IMM_ATOMIC_ADD )
	NAMED_VALUE( _T( "UAV_ACCESS_IMM_ATOMIC_IMAX" ),        UAV_ACCESS_IMM_ATOMIC_IMAX )
	NAMED_VALUE( _T( "UAV_ACCESS_IMM_ATOMIC_IMIN" ),        UAV_ACCESS_IMM_ATOMIC_IMIN )
	NAMED_VALUE( _T( "UAV_ACCESS_IMM_ATOMIC_UMAX" ),        UAV_ACCESS_IMM_ATOMIC_UMAX )
	NAMED_VALUE( _T( "UAV_ACCESS_IMM_ATOMIC_UMIN" ),        UAV_ACCESS_IMM_ATOMIC_UMIN )
END_NAMED_VALUES( UAV_ACCESS_METHOD )                                      

static const DXGI_FORMAT RT_FORMAT      = DXGI_FORMAT_R8G8B8A8_UNORM;
static const DXGI_FORMAT Z_FORMAT       = DXGI_FORMAT_R32_TYPELESS;
static const DXGI_FORMAT UAV_FORMAT     = DXGI_FORMAT_R32G32_UINT;
static const UINT UAV_STRUCTURE_STRIDE  = sizeof(UINT) * 2;
static const UINT RT_WIDTH              = 15; // odd to ensure helper pixels are used (UAV writes should not occur for helper pixels)
static const UINT RT_HEIGHT             = 15;
static const UINT UAV_WIDTH             = RT_WIDTH;
static const UINT VERTEX_COUNT          = 4;

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 color;
};

static const D3D11_COMPARISON_FUNC DEPTH_FUNC_TABLE[] = 
{
	D3D11_COMPARISON_LESS,
	D3D11_COMPARISON_LESS_EQUAL,
	D3D11_COMPARISON_GREATER,
	D3D11_COMPARISON_GREATER_EQUAL
}; 

// Sample counts 1 & 4 are tested because they are both required by all hardware
static const UINT SAMPLE_COUNT_TABLE[] = 
{
	1,
	4,
};

// Returns an approriate value for the initial data in the UAV so that the test will 
// work properly
static UINT GetInitialUAVData(UAV_ACCESS_METHOD AccessMethod)
{
	switch(AccessMethod)
	{
	case UAV_ACCESS_STORE:
	case UAV_ACCESS_ATOMIC_XOR:
	case UAV_ACCESS_ATOMIC_OR:
	case UAV_ACCESS_ATOMIC_CMP_STORE:
	case UAV_ACCESS_ATOMIC_EXCHANGE:
	case UAV_ACCESS_ATOMIC_CMP_EXCHANGE:
	case UAV_ACCESS_ATOMIC_ADD:
	case UAV_ACCESS_ATOMIC_IMAX:
	case UAV_ACCESS_ATOMIC_UMAX:

	case UAV_ACCESS_IMM_ATOMIC_XOR:
	case UAV_ACCESS_IMM_ATOMIC_OR:
	case UAV_ACCESS_IMM_ATOMIC_ADD:
	case UAV_ACCESS_IMM_ATOMIC_IMAX:
	case UAV_ACCESS_IMM_ATOMIC_UMAX:
		return 0;

	case UAV_ACCESS_ATOMIC_IMIN:
	case UAV_ACCESS_IMM_ATOMIC_IMIN:
		return INT_MAX;

	case UAV_ACCESS_ATOMIC_AND:
	case UAV_ACCESS_ATOMIC_UMIN:
	case UAV_ACCESS_IMM_ATOMIC_AND:
	case UAV_ACCESS_IMM_ATOMIC_UMIN:
		return UINT_MAX;

	default:
		assert( false );
		return 0;
	}
}

CPixelShaderUAVWrite::CPixelShaderUAVWrite()
{
	m_pPixelShader = NULL;
	m_pVertexShader = NULL;
	m_pUAVBuffer = NULL;
	m_pUAVTexture1D = NULL;
	m_pUAVTexture2D = NULL;
	m_pUAVTexture3D = NULL;
	m_pUAV = NULL;
	m_pVB = NULL;
	m_pInputLayout = NULL;
	m_pQuery = NULL;
}

void CPixelShaderUAVWrite::InitTestParameters()
{
	AddParameter<bool>                  ( "ShaderEarlyDepth",       &m_ShaderEarlyDepth );
	AddParameter<bool>                  ( "WriteODepth",            &m_WriteODepth );
	AddParameter<D3D11_COMPARISON_FUNC> ( "DepthFunc",              &m_DepthFunc );
	AddParameter<UINT>                  ( "DepthTestResult",        &m_DepthTestResult );
	AddParameter<UINT>                  ( "SampleCount",            &m_SampleCount );
	AddParameter<bool>                  ( "DiscardBeforeWrite",     &m_DiscardBeforeWrite );
	AddParameter<bool>                  ( "DiscardAfterWrite",      &m_DiscardAfterWrite );
	AddParameter<UAV_TYPE>              ( "UAVType",                &m_UAVType );
	AddParameter<UAV_ACCESS_METHOD>     ( "UAVAccessMethod",        &m_UAVAccessMethod );
	AddParameter<D3D11_UAV_DIMENSION>   ( "UAVDimension",           &m_UAVDimension );

	SetParameterDefaultValue<bool>                  ( "ShaderEarlyDepth",       false );
	SetParameterDefaultValue<bool>                  ( "WriteODepth",            false );
	SetParameterDefaultValue<D3D11_COMPARISON_FUNC> ( "DepthFunc",              D3D11_COMPARISON_ALWAYS );
	SetParameterDefaultValue<UINT>                  ( "DepthTestResult",        UINT_MAX );
	SetParameterDefaultValue<UINT>                  ( "SampleCount",            1 );
	SetParameterDefaultValue<bool>                  ( "DiscardBeforeWrite",     false );
	SetParameterDefaultValue<bool>                  ( "DiscardAfterWrite",      false );
	SetParameterDefaultValue<UAV_TYPE>              ( "UAVType",                UAV_RAW );
	SetParameterDefaultValue<UAV_ACCESS_METHOD>     ( "UAVAccessMethod",        UAV_ACCESS_STORE );
	SetParameterDefaultValue<D3D11_UAV_DIMENSION>   ( "UAVDimension",           D3D11_UAV_DIMENSION_BUFFER );

	testfactor::RFactor rShaderEarlyDepthTF = AddParameterValueSet<bool>( "ShaderEarlyDepth", new CBoolValueSet() );
	testfactor::RFactor rWriteODepthTF      = AddParameterValueSet<bool>( "WriteODepth", new CBoolValueSet() );

	testfactor::RFactor rDepthFunc = AddParameterValueSet( "DepthFunc", new CTableValueSet<D3D11_COMPARISON_FUNC>( DEPTH_FUNC_TABLE, sizeof( DEPTH_FUNC_TABLE[0] ), ARRAY_SIZE( DEPTH_FUNC_TABLE ) ) );
	
	CUserValueSet<UINT>* pDepthTestResultValues = new CUserValueSet<UINT>();
	pDepthTestResultValues->AddValue( 0 );
	pDepthTestResultValues->AddValue( 0xAAAAAAAA );
	pDepthTestResultValues->AddValue( UINT_MAX );
	testfactor::RFactor rDepthTestResult = AddParameterValueSet( "DepthTestResult", pDepthTestResultValues ); 
	
	testfactor::RFactor rSampleCount = AddParameterValueSet<UINT>( "SampleCount", new CTableValueSet<UINT>( SAMPLE_COUNT_TABLE, sizeof( SAMPLE_COUNT_TABLE[0] ), ARRAY_SIZE( SAMPLE_COUNT_TABLE ) ) );

	testfactor::RFactor rDiscardBeforeWriteTF = AddParameterValueSet<bool>( "DiscardBeforeWrite", new CBoolValueSet() );
	testfactor::RFactor rDiscardAfterWriteTF  = AddParameterValueSet<bool>( "DiscardAfterWrite", new CBoolValueSet() );

	testfactor::RFactor rDimensionBuffer = AddParameterValue<D3D11_UAV_DIMENSION>( "UAVDimension", D3D11_UAV_DIMENSION_BUFFER );
	testfactor::RFactor rDimensionTex1D  = AddParameterValue<D3D11_UAV_DIMENSION>( "UAVDimension", D3D11_UAV_DIMENSION_TEXTURE1D );
	testfactor::RFactor rDimensionTex2D  = AddParameterValue<D3D11_UAV_DIMENSION>( "UAVDimension", D3D11_UAV_DIMENSION_TEXTURE2D );
	testfactor::RFactor rDimensionTex3D  = AddParameterValue<D3D11_UAV_DIMENSION>( "UAVDimension", D3D11_UAV_DIMENSION_TEXTURE3D );

	testfactor::RFactor rTypeRaw        = AddParameterValue<UAV_TYPE>( "UAVType", UAV_RAW );
	testfactor::RFactor rTypeTyped      = AddParameterValue<UAV_TYPE>( "UAVType", UAV_TYPED );
	testfactor::RFactor rTypeStructured = AddParameterValue<UAV_TYPE>( "UAVType", UAV_STRUCTURED );
	testfactor::RFactor rTypeCounter    = AddParameterValue<UAV_TYPE>( "UAVType", UAV_COUNTER );
	testfactor::RFactor rTypeAppend     = AddParameterValue<UAV_TYPE>( "UAVType", UAV_APPEND );

	//pAccessMethodValues->AddValue( UAV_ACCESS_STORE );
	CUserValueSet<UAV_ACCESS_METHOD>* pAccessMethodValues = new CUserValueSet<UAV_ACCESS_METHOD>();
	pAccessMethodValues->AddValue( UAV_ACCESS_STORE );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_AND );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_XOR );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_OR );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_CMP_STORE );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_EXCHANGE );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_CMP_EXCHANGE );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_ADD );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_IMAX );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_IMIN );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_UMAX );
	pAccessMethodValues->AddValue( UAV_ACCESS_ATOMIC_UMIN );
	pAccessMethodValues->AddValue( UAV_ACCESS_IMM_ATOMIC_AND );
	pAccessMethodValues->AddValue( UAV_ACCESS_IMM_ATOMIC_XOR );
	pAccessMethodValues->AddValue( UAV_ACCESS_IMM_ATOMIC_OR );
	pAccessMethodValues->AddValue( UAV_ACCESS_IMM_ATOMIC_ADD );
	pAccessMethodValues->AddValue( UAV_ACCESS_IMM_ATOMIC_IMAX );
	pAccessMethodValues->AddValue( UAV_ACCESS_IMM_ATOMIC_IMIN );
	pAccessMethodValues->AddValue( UAV_ACCESS_IMM_ATOMIC_UMAX );
	pAccessMethodValues->AddValue( UAV_ACCESS_IMM_ATOMIC_UMIN );
	testfactor::RFactor rfAccessMethods = AddParameterValueSet( "UAVAccessMethod", pAccessMethodValues );
	testfactor::RFactor rfAccessMethod_Store = AddParameterValue<UAV_ACCESS_METHOD>( "UAVAccessMethod", UAV_ACCESS_STORE );

	// Append buffers only use the store access method
	testfactor::RFactor rBufferFactor = rDimensionBuffer * (((rTypeRaw + rTypeTyped + rTypeStructured + rTypeCounter) * rfAccessMethods) + (rTypeAppend * rfAccessMethod_Store));

	// Only typed UAVs can be created from non-buffer resources
	testfactor::RFactor rTex1DFactor  = rDimensionTex1D * rTypeTyped * rfAccessMethods;
	testfactor::RFactor rTex2DFactor  = rDimensionTex2D * rTypeTyped * rfAccessMethods;
	testfactor::RFactor rTex3DFactor  = rDimensionTex3D * rTypeTyped * rfAccessMethods;

	testfactor::RFactor rTypeAndAccess = rBufferFactor + rTex1DFactor + rTex2DFactor + rTex3DFactor;

	SetRootTestFactor
		( 
			(rTypeAndAccess * (rDiscardAfterWriteTF + rDiscardBeforeWriteTF) * rWriteODepthTF * rShaderEarlyDepthTF * rDepthTestResult) % (rSampleCount * rDepthFunc)
		);

	// Prioritizing test cases
	// Making non_immediate versions lower priority
	AddPriorityPatternFilter( 
		FilterEqual<UAV_ACCESS_METHOD>(_T("UAVAccessMethod"), UAV_ACCESS_ATOMIC_AND) ||
		FilterEqual<UAV_ACCESS_METHOD>(_T("UAVAccessMethod"), UAV_ACCESS_ATOMIC_XOR) ||
		FilterEqual<UAV_ACCESS_METHOD>(_T("UAVAccessMethod"), UAV_ACCESS_ATOMIC_OR) ||
		FilterEqual<UAV_ACCESS_METHOD>(_T("UAVAccessMethod"), UAV_ACCESS_ATOMIC_ADD) ||
		FilterEqual<UAV_ACCESS_METHOD>(_T("UAVAccessMethod"), UAV_ACCESS_ATOMIC_IMAX) ||
		FilterEqual<UAV_ACCESS_METHOD>(_T("UAVAccessMethod"), UAV_ACCESS_ATOMIC_IMIN) ||
		FilterEqual<UAV_ACCESS_METHOD>(_T("UAVAccessMethod"), UAV_ACCESS_ATOMIC_UMAX) ||
		FilterEqual<UAV_ACCESS_METHOD>(_T("UAVAccessMethod"), UAV_ACCESS_ATOMIC_UMIN) 
		, WeightFactorSmall);
	// Making Tex1D and Tex3D lower priority
	AddPriorityPatternFilter( 
		FilterEqual<D3D11_UAV_DIMENSION>(_T("UAVDimension"), D3D11_UAV_DIMENSION_TEXTURE1D) ||
		FilterEqual<D3D11_UAV_DIMENSION>(_T("UAVDimension"), D3D11_UAV_DIMENSION_TEXTURE3D)  
		, WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );

}

static D3D10_SHADER_MACRO Macro( const char* name, const char* def )
{
	D3D10_SHADER_MACRO ret = { name, def };
	return ret;
}

static const char* VS = 
"struct VSIn"
"{"
"    float3 pos   : POS;"
"    float4 color : CLR;"
"};"
"struct VSOut"
"{"
"    float4 pos   : SV_POSITION;"
"    float4 color : CLR;"
"};"
"VSOut VSEntry(VSIn input)"
"{"
"    VSOut output;"
"    output.pos = float4(input.pos, 1.0f);"
"    output.color = input.color;"
"    return output;"
"}";

TEST_RESULT CPixelShaderUAVWrite::Setup()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	// Vertex shader
	hr = g_App.CompileShaderFromMemory( VS, 
										strlen(VS), 
										"VSEntry", 
										D3D_SHADER_STAGE_VERTEX, 
										D3D_FEATURE_LEVEL_11_0, 
										D3D_FEATURE_LEVEL_11_0, 
										0, 
										&pShader, 
										&pError,
										NULL, 
										NULL );

	if( pError )
	{
		WriteToLog( (const char*)pError->GetBufferPointer() );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( "CompileShaderFromMemory failed for VS - result code '%s'", D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	hr = GetDevice()->CreateVertexShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pVertexShader );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreateVertexShader() unexpectedly failed - result code '%s'", D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	// depth stencil state
	for( UINT i = 0; i < ARRAY_SIZE( DEPTH_FUNC_TABLE ); i++ )
	{
		ID3D11DepthStencilState* pDSS = NULL;

		D3D11_DEPTH_STENCIL_DESC depthDesc = CD3D11_DEPTH_STENCIL_DESC( D3D11_DEFAULT );
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDesc.DepthFunc = DEPTH_FUNC_TABLE[i];
		hr = GetDevice()->CreateDepthStencilState( &depthDesc, &pDSS );
		if( FAILED( hr ) )
		{
			WriteToLog( "CreateDepthStencilState() failed - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}

		m_DepthStencilStateMap[ DEPTH_FUNC_TABLE[i] ] = pDSS;
	}

	// Input layout
	{
		D3D11_INPUT_ELEMENT_DESC inputLayout[] = 
		{
			{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "CLR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = GetDevice()->CreateInputLayout( inputLayout,
											 ARRAY_SIZE( inputLayout ),
											 pShader->GetBufferPointer(),
											 pShader->GetBufferSize(),
											 &m_pInputLayout );
		if ( FAILED( hr ) )
		{
			WriteToLog( "CreateInputLayout() failed - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}
	}

	// vertex buffer
	{
		D3D11_BUFFER_DESC vbDesc = CD3D11_BUFFER_DESC( sizeof(Vertex) * VERTEX_COUNT,
													   D3D11_BIND_VERTEX_BUFFER,
													   D3D11_USAGE_DEFAULT );
		hr = GetDevice()->CreateBuffer( &vbDesc, NULL, &m_pVB );
		if ( FAILED( hr ) )
		{
			WriteToLog( "CreateBuffer() failed for VB - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}
	}

	// render target & RTV
	for( UINT i = 0; i < ARRAY_SIZE( SAMPLE_COUNT_TABLE ); i++ )
	{
		UINT sampleCount = SAMPLE_COUNT_TABLE[i];

		ID3D11Texture2D* pRT = NULL;

		D3D11_TEXTURE2D_DESC rtDesc = CD3D11_TEXTURE2D_DESC( RT_FORMAT,
															 RT_WIDTH,
															 RT_HEIGHT,
															 1,
															 1,
															 D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
															 D3D11_USAGE_DEFAULT,
															 0,
															 sampleCount,
															 D3D11_STANDARD_MULTISAMPLE_PATTERN,
															 0 );

		hr = GetFramework()->CreateTexture2DMapDefaultOptional( &rtDesc, NULL, &pRT );
		if( FAILED( hr ) )
		{
			WriteToLog( "CreateTexture2D() failed for render target - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}

		m_RenderTargetMap[ sampleCount ] = pRT;

		ID3D11RenderTargetView* pRTV = NULL;

		hr = GetDevice()->CreateRenderTargetView( pRT, NULL, &pRTV );
		if( FAILED( hr ) )
		{
			WriteToLog( "CreateRenderTargetView() failed - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}

		m_RenderTargetViewMap[ sampleCount ] = pRTV;
	}

	// depth buffer & DSV
	for( UINT i = 0; i < ARRAY_SIZE( SAMPLE_COUNT_TABLE ); i++ )
	{
		UINT sampleCount = SAMPLE_COUNT_TABLE[i];

		ID3D11Texture2D* pDepthBuffer = NULL;

		D3D11_TEXTURE2D_DESC zDesc = CD3D11_TEXTURE2D_DESC( Z_FORMAT,
															RT_WIDTH,
															RT_HEIGHT,
															1,
															1,
															D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
															D3D11_USAGE_DEFAULT,
															0,
															sampleCount,
															D3D11_STANDARD_MULTISAMPLE_PATTERN,
															0 );

		hr = GetFramework()->CreateTexture2DMapDefaultOptional( &zDesc, NULL, &pDepthBuffer );
		if( FAILED( hr ) )
		{
			WriteToLog( "CreateTexture2D() failed for z buffer - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}

		m_DepthStencilMap[ sampleCount ] = pDepthBuffer;

		ID3D11DepthStencilView* pDSV = NULL;

		assert( DXGI_FORMAT_R32_TYPELESS == Z_FORMAT );
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC( sampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D,
																				DXGI_FORMAT_D32_FLOAT,
																				0,
																				0,
																				1 );

		hr = GetDevice()->CreateDepthStencilView( pDepthBuffer, &dsvDesc, &pDSV );
		if( FAILED( hr ) )
		{
			WriteToLog( "CreateDepthStencilView() failed - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}

		m_DepthStencilViewMap[ sampleCount ] = pDSV;
	}

	// Pipeline stats query
	{
		D3D11_QUERY_DESC queryDesc = { D3D11_QUERY_PIPELINE_STATISTICS, 0 };
		hr = GetDevice()->CreateQuery( &queryDesc, &m_pQuery );
		if( FAILED( hr ) )
		{
			WriteToLog( "CreateQuery() failed - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}
	}

cleanup:
	SAFE_RELEASE( pShader );
	SAFE_RELEASE( pError );

	return tr;
}

// Releases all interfaces in a map, and clears all entries in that map
template< typename MAPTYPE >
void ReleaseMap( MAPTYPE& map )
{
	for( MAPTYPE::iterator it = map.begin(); it != map.end(); ++it )
	{
		(*it).second->Release();
	}
	map.clear();
}

void CPixelShaderUAVWrite::Cleanup()
{
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pQuery );

	ReleaseMap( m_DepthStencilStateMap );
	ReleaseMap( m_RenderTargetMap );
	ReleaseMap( m_RenderTargetViewMap );
	ReleaseMap( m_DepthStencilMap );
	ReleaseMap( m_DepthStencilViewMap );
}

TEST_RESULT CPixelShaderUAVWrite::SetupTestCase()
{
	// pixel shader
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	ID3D11Resource* pUAVResource = NULL;

	// Determine macro definitions used by this test case
	std::vector<D3D10_SHADER_MACRO> Defines;

	if( m_ShaderEarlyDepth ) { Defines.push_back( Macro( "FORCE_EARLY_DEPTH", "1" ) ); }
	if( m_WriteODepth )      { Defines.push_back( Macro( "PS_OUTPUT_DEPTH", "1" ) ); }

	switch( m_UAVType )
	{
	case UAV_RAW:        Defines.push_back( Macro( "USE_RAW_BUFFER", "1" ) ); break;
	case UAV_STRUCTURED: Defines.push_back( Macro( "USE_STRUCTURED_BUFFER", "1" ) ); break;
	case UAV_APPEND:     Defines.push_back( Macro( "USE_APPEND_BUFFER", "1" ) ); break;
	case UAV_COUNTER:    Defines.push_back( Macro( "USE_COUNTER_BUFFER", "1" ) ); break;

	case UAV_TYPED:      
		switch( m_UAVDimension )
		{
		case D3D11_UAV_DIMENSION_BUFFER:    Defines.push_back( Macro( "USE_TYPED_BUFFER", "1" ) ); break;
		case D3D11_UAV_DIMENSION_TEXTURE1D: Defines.push_back( Macro( "USE_TYPED_TEXTURE1D", "1" ) ); break;
		case D3D11_UAV_DIMENSION_TEXTURE2D: Defines.push_back( Macro( "USE_TYPED_TEXTURE2D", "1" ) ); break;
		case D3D11_UAV_DIMENSION_TEXTURE3D: Defines.push_back( Macro( "USE_TYPED_TEXTURE3D", "1" ) ); break;
		default: assert( false );
		}
		break;
	default: assert( false );
	}

	switch( m_UAVAccessMethod )
	{
	case UAV_ACCESS_STORE:                  Defines.push_back( Macro( "OP_STORE", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_AND:             Defines.push_back( Macro( "OP_ATOMIC_AND", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_XOR:             Defines.push_back( Macro( "OP_ATOMIC_XOR", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_OR:              Defines.push_back( Macro( "OP_ATOMIC_OR", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_CMP_STORE:       Defines.push_back( Macro( "OP_ATOMIC_CMP_STORE", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_EXCHANGE:        Defines.push_back( Macro( "OP_ATOMIC_EXCHANGE", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_CMP_EXCHANGE:    Defines.push_back( Macro( "OP_ATOMIC_CMP_EXCHANGE", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_ADD:             Defines.push_back( Macro( "OP_ATOMIC_ADD", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_IMAX:            Defines.push_back( Macro( "OP_ATOMIC_MAX", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_IMIN:            Defines.push_back( Macro( "OP_ATOMIC_MIN", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_UMAX:            Defines.push_back( Macro( "OP_ATOMIC_MAX", "1" ) ); break;
	case UAV_ACCESS_ATOMIC_UMIN:            Defines.push_back( Macro( "OP_ATOMIC_MIN", "1" ) ); break;

	case UAV_ACCESS_IMM_ATOMIC_AND:         Defines.push_back( Macro( "OP_IMM_ATOMIC_AND", "1" ) ); break;
	case UAV_ACCESS_IMM_ATOMIC_XOR:         Defines.push_back( Macro( "OP_IMM_ATOMIC_XOR", "1" ) ); break;
	case UAV_ACCESS_IMM_ATOMIC_OR:          Defines.push_back( Macro( "OP_IMM_ATOMIC_OR", "1" ) ); break;
	case UAV_ACCESS_IMM_ATOMIC_ADD:         Defines.push_back( Macro( "OP_IMM_ATOMIC_ADD", "1" ) ); break;
	case UAV_ACCESS_IMM_ATOMIC_IMAX:        Defines.push_back( Macro( "OP_IMM_ATOMIC_MAX", "1" ) ); break;
	case UAV_ACCESS_IMM_ATOMIC_IMIN:        Defines.push_back( Macro( "OP_IMM_ATOMIC_MIN", "1" ) ); break;
	case UAV_ACCESS_IMM_ATOMIC_UMAX:        Defines.push_back( Macro( "OP_IMM_ATOMIC_MAX", "1" ) ); break;
	case UAV_ACCESS_IMM_ATOMIC_UMIN:        Defines.push_back( Macro( "OP_IMM_ATOMIC_MIN", "1" ) ); break;
	default: assert( false );
	}

	bool bUnsignedUAV = false;

	switch( m_UAVAccessMethod )
	{
	case UAV_ACCESS_STORE:
	case UAV_ACCESS_ATOMIC_AND:
	case UAV_ACCESS_ATOMIC_XOR:
	case UAV_ACCESS_ATOMIC_OR:
	case UAV_ACCESS_ATOMIC_CMP_STORE:
	case UAV_ACCESS_ATOMIC_EXCHANGE:
	case UAV_ACCESS_ATOMIC_CMP_EXCHANGE:
	case UAV_ACCESS_ATOMIC_ADD:
	case UAV_ACCESS_ATOMIC_IMAX:
	case UAV_ACCESS_ATOMIC_IMIN:
	case UAV_ACCESS_IMM_ATOMIC_AND:
	case UAV_ACCESS_IMM_ATOMIC_XOR:
	case UAV_ACCESS_IMM_ATOMIC_OR:
	case UAV_ACCESS_IMM_ATOMIC_ADD:
	case UAV_ACCESS_IMM_ATOMIC_IMAX:
	case UAV_ACCESS_IMM_ATOMIC_IMIN:
		Defines.push_back( Macro( "DATATYPE", "int" ) );
		break;

	case UAV_ACCESS_IMM_ATOMIC_UMAX:
	case UAV_ACCESS_IMM_ATOMIC_UMIN:
	case UAV_ACCESS_ATOMIC_UMAX:        
	case UAV_ACCESS_ATOMIC_UMIN:   
		Defines.push_back( Macro( "DATATYPE", "uint" ) );
		bUnsignedUAV = true;
		break;

	default: assert( false );
	}

	if( m_DiscardBeforeWrite ) { Defines.push_back( Macro( "DISCARD_BEFORE_WRITE", "1" ) ); }
	if( m_DiscardAfterWrite )  { Defines.push_back( Macro( "DISCARD_AFTER_WRITE", "1" ) ); }
	

	// defines are always NULL terminated
	Defines.push_back( Macro( NULL, NULL ) );

	//
	// Compile shader source
	//
	hr = D3DX10CompileFromResource( NULL, 
									"psuav.hlsl", 
									NULL, 
									&(Defines[0]), 
									NULL, 
									"PSEntry", 
									"ps_5_0", 
									0, 
									0, 
									NULL, 
									&pShader, 
									&pError, 
									NULL );

	if( pError )
	{
		WriteToLog( (const char*)pError->GetBufferPointer() );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( "CompileShaderFromResource() unexpectedly failed for PS - result code '%s'", D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	hr = GetDevice()->CreatePixelShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pPixelShader );
	if( FAILED( hr ) )
	{
		WriteToLog( "CreatePixelShader() unexpectedly failed - result code '%s'", D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	// resource & UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC* pUAVDesc = NULL;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory( &uavDesc, sizeof(uavDesc) );

		uavDesc.ViewDimension = m_UAVDimension;

		if( D3D11_UAV_DIMENSION_BUFFER == m_UAVDimension )
		{
			UINT structureByteStride = 0;
			UINT bufferFlags = 0;
			uavDesc.Buffer.NumElements = UAV_WIDTH * m_SampleCount;

			switch( m_UAVType )
			{
			case UAV_RAW:    
				bufferFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS; 
				uavDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_RAW;
				uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				uavDesc.Buffer.NumElements *= 2; // each element of the UAV is 64-bits, but RAW UAV format is 32-bits per element
				break;

			case UAV_TYPED:     
				if( bUnsignedUAV )
				{
					uavDesc.Format = DXGI_FORMAT_R32_UINT;
				}
				else
				{
					uavDesc.Format = DXGI_FORMAT_R32_SINT;
				}
				uavDesc.Buffer.NumElements *= 2; // each element of the UAV is 64-bits, but typed UAV format is 32-bits per element
				break;

			case UAV_STRUCTURED:
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				bufferFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; 
				structureByteStride = UAV_STRUCTURE_STRIDE; 
				break;

			case UAV_APPEND:     
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				bufferFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; 
				uavDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_APPEND;
				structureByteStride = UAV_STRUCTURE_STRIDE; 
				break;

			case UAV_COUNTER:     
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				bufferFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; 
				uavDesc.Buffer.Flags |= D3D11_BUFFER_UAV_FLAG_COUNTER;
				structureByteStride = UAV_STRUCTURE_STRIDE; 
				break;

			default: 
				assert( false );
			}

			D3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC( UAV_WIDTH * sizeof(UINT) * 2 * m_SampleCount,
															   D3D11_BIND_UNORDERED_ACCESS,
															   D3D11_USAGE_DEFAULT,
															   0,
															   bufferFlags,
															   structureByteStride );
			hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pUAVBuffer );
			if ( FAILED( hr ) )
			{   
				WriteToLog( "CreateBuffer() failed for UAV - result code '%s'", D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto cleanup;
			}

			pUAVResource = m_pUAVBuffer;

			pUAVDesc = &uavDesc;
		}
		else if( D3D11_UAV_DIMENSION_TEXTURE1D == m_UAVDimension )
		{
			D3D11_TEXTURE1D_DESC texDesc = CD3D11_TEXTURE1D_DESC( bUnsignedUAV ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R32_SINT,
																  UAV_WIDTH * 2 * m_SampleCount, // each element of the UAV is 64-bits, but typed UAV format is 32-bits per element
																  1,
																  1,
																  D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS );

			hr = GetDevice()->CreateTexture1D( &texDesc, NULL, &m_pUAVTexture1D );
			if ( FAILED( hr ) )
			{   
				WriteToLog( "CreateTexture1D() failed for UAV - result code '%s'", D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto cleanup;
			}

			pUAVResource = m_pUAVTexture1D;
		}
		else if( D3D11_UAV_DIMENSION_TEXTURE2D == m_UAVDimension )
		{
			D3D11_TEXTURE2D_DESC texDesc = CD3D11_TEXTURE2D_DESC( bUnsignedUAV ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R32_SINT,
																  UAV_WIDTH * 2 * m_SampleCount, // each element of the UAV is 64-bits, but typed UAV format is 32-bits per element
																  1,
																  1,
																  1,
																  D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS );

			hr = GetFramework()->CreateTexture2DMapDefaultOptional( &texDesc, NULL, &m_pUAVTexture2D );
			if ( FAILED( hr ) )
			{   
				WriteToLog( "CreateTexture2D() failed for UAV - result code '%s'", D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto cleanup;
			}

			pUAVResource = m_pUAVTexture2D;
		}
		else if( D3D11_UAV_DIMENSION_TEXTURE3D == m_UAVDimension )
		{
			D3D11_TEXTURE3D_DESC texDesc = CD3D11_TEXTURE3D_DESC( bUnsignedUAV ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R32_SINT,
																  UAV_WIDTH * 2 * m_SampleCount, // each element of the UAV is 64-bits, but typed UAV format is 32-bits per element
																  1,
																  1,
																  1,
																  D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS );

			hr = GetFramework()->CreateTexture3DMapDefaultOptional( &texDesc, NULL, &m_pUAVTexture3D );
			if ( FAILED( hr ) )
			{   
				WriteToLog( "CreateTexture3D() failed for UAV - result code '%s'", D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto cleanup;
			}

			pUAVResource = m_pUAVTexture3D;
		}
		
		hr = GetDevice()->CreateUnorderedAccessView( pUAVResource, pUAVDesc, &m_pUAV );
		if ( FAILED( hr ) )
		{   
			WriteToLog( "CreateUnorderedAccessView() failed - result code '%s'", D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto cleanup;
		}
	}

cleanup:
	SAFE_RELEASE( pShader );
	SAFE_RELEASE( pError );

	return tr;
}

void CPixelShaderUAVWrite::CleanupTestCase()
{
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pUAVBuffer);
	SAFE_RELEASE(m_pUAVTexture1D);
	SAFE_RELEASE(m_pUAVTexture2D);
	SAFE_RELEASE(m_pUAVTexture3D);
	SAFE_RELEASE(m_pUAV);
}

TEST_RESULT CPixelShaderUAVWrite::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;
	D3D11_VIEWPORT viewport = { 0, 0, RT_WIDTH, RT_HEIGHT, 0, 1 };

	ID3D11DepthStencilState* pDSS = m_DepthStencilStateMap[ m_DepthFunc ];  assert( pDSS );
	ID3D11RenderTargetView* pRTV  = m_RenderTargetViewMap[ m_SampleCount ]; assert( pRTV );
	ID3D11DepthStencilView* pDSV  = m_DepthStencilViewMap[ m_SampleCount ]; assert( pDSV );

	// Setup Z buffer values to make the depth test pass according to m_DepthTestResult
	assert( DXGI_FORMAT_R32_TYPELESS == Z_FORMAT );
	GetImmediateContext()->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	std::vector< float > depthData;
	depthData.resize( RT_WIDTH * RT_HEIGHT * m_SampleCount );

	UINT idx = 0;

	for( UINT y = 0; y < RT_HEIGHT; y++ )
	{
		for( UINT x = 0; x < RT_WIDTH; x++ )
		{
			for( UINT s = 0; s < m_SampleCount; s++ )
			{
				float depthValue = GetDepthBufferValue( s );

				depthData[idx] = depthValue;
				idx++;
			}
		}
	}

	DepthHelper helper;
	tr = helper.Setup( GetFramework(), pDSV );
	if( tr != RESULT_PASS )
	{
		WriteToLog( "DepthHelper::Setup failed" );
		goto cleanup;
	}

	tr = helper.Fill( &(depthData[0]), RT_WIDTH, RT_HEIGHT, m_SampleCount );
	if( tr != RESULT_PASS )
	{
		WriteToLog( "DepthHelper::Fill failed" );
		goto cleanup;
	}

	// setup pipeline state
	UINT vbStride = sizeof( Vertex );
	UINT vbOffset = 0;
	UINT appendCount = 0;

	GetEffectiveContext()->RSSetViewports( 1, &viewport );
	GetEffectiveContext()->PSSetShader( m_pPixelShader, NULL, 0 );
	GetEffectiveContext()->VSSetShader( m_pVertexShader, NULL, 0 );
	GetEffectiveContext()->OMSetDepthStencilState( pDSS, 0 );
	GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 1, &pRTV, pDSV, 1, 1, &m_pUAV, &appendCount );
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVB, &vbStride, &vbOffset );
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// fill resources
	float black[4] = {0,0,0,0};
	GetEffectiveContext()->ClearRenderTargetView( pRTV, black );
	
	UINT uavInitialData = GetInitialUAVData( m_UAVAccessMethod );
	UINT uavInitial[4] = {uavInitialData, uavInitialData, uavInitialData, uavInitialData};
	GetEffectiveContext()->ClearUnorderedAccessViewUint( m_pUAV, uavInitial );

	Vertex vertices[4] = 
	{
		{ D3DXVECTOR3( -1.0f,  1.0f, 0.75f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
		{ D3DXVECTOR3(  1.0f,  1.0f, 0.75f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
		{ D3DXVECTOR3( -1.0f, -1.0f, 0.75f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
		{ D3DXVECTOR3(  1.0f, -1.0f, 0.75f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
	};
	GetEffectiveContext()->UpdateSubresource( m_pVB, 0, NULL, vertices, 0, 0 );
	
	// draw (bracketed by a pipeline stats query)
	GetEffectiveContext()->Begin( m_pQuery );
	GetEffectiveContext()->Draw( 4, 0 );
	GetEffectiveContext()->End( m_pQuery );

	// execute deferred commands
	hr = ExecuteEffectiveContext();
	if ( FAILED( hr ) )
	{
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	// check results
	tr = CheckResults();

cleanup:
	return tr;
}

TEST_RESULT CPixelShaderUAVWrite::CheckResults()
{
	TEST_RESULT tr = RESULT_PASS;

	// Render target
	tr = CheckResults_RenderTarget();
	if ( RESULT_PASS != tr )
	{
		goto cleanup;
	}

	// Z buffer
	tr = CheckResults_ZBuffer();
	if ( RESULT_PASS != tr )
	{
		goto cleanup;
	}

	// UAV
	tr = CheckResults_UAV();
	if ( RESULT_PASS != tr )
	{
		goto cleanup;
	}

	// Query
	tr = CheckResults_Query();
	if ( RESULT_PASS != tr )
	{
		goto cleanup;
	}

cleanup:
	return tr;
}

TEST_RESULT CPixelShaderUAVWrite::CheckResults_RenderTarget()
{
	TEST_RESULT tr = RESULT_PASS;

	assert( DXGI_FORMAT_R8G8B8A8_UNORM == RT_FORMAT );

	std::vector< DWORD > RTContents;
	RTContents.resize( RT_WIDTH * RT_HEIGHT * m_SampleCount );

	ID3D11Texture2D* pRT = m_RenderTargetMap[ m_SampleCount ]; assert( pRT );

	if( !MSHelper::ReadWithoutConversion( GetFramework(), pRT, (void*) &(RTContents[0]), RT_FORMAT ) )
	{
		WriteToLog( "MSHelper::Read failed for render target" );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	UINT pixelIndex = 0;

	for( UINT s = 0; s < m_SampleCount; s++ )
	{
		for( UINT y = 0; y < RT_HEIGHT; y++ )
		{
			for( UINT x = 0; x < RT_WIDTH; x++ )
			{
				DWORD srcPixel = RTContents[pixelIndex];
				pixelIndex++;

				// expect white unless the pixel was discarded or the depth test failed
				DWORD expectedPixel = 0xFFFFFFFF;

				if( (m_DiscardBeforeWrite || m_DiscardAfterWrite) &&
					(x > 0) )
				{   
					expectedPixel = 0;
				}
				if( 0 == ( m_DepthTestResult & (1 << s) ) )
				{
					expectedPixel = 0;
				}

				if( expectedPixel != srcPixel )
				{
					WriteToLog( "Render target contained unexpected value.  Pixel Position = (%u, %u).  Sample index = %u.  Found pixel = 0x%x.  Expected value = 0x%x",
						x, y, s, srcPixel, expectedPixel );
					tr = RESULT_FAIL;
					goto cleanup;
				}
			}
		}
	}

cleanup:
	return tr;
}

TEST_RESULT CPixelShaderUAVWrite::CheckResults_ZBuffer()
{
	ID3D11Texture2D* pDepthBuffer = m_DepthStencilMap[ m_SampleCount ];

	TEST_RESULT tr = RESULT_PASS;

	assert( DXGI_FORMAT_R32_TYPELESS == Z_FORMAT );

	std::vector< float > RTContents;
	RTContents.resize( RT_WIDTH * RT_HEIGHT * m_SampleCount);

	if( !MSHelper::ReadWithoutConversion( GetFramework(), pDepthBuffer, (void*) &(RTContents[0]), DXGI_FORMAT_R32_FLOAT ) )
	{
		WriteToLog( "MSHelper::Read failed for depth buffer" );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	UINT pixelIndex = 0;

	for( UINT s = 0; s < m_SampleCount; s++ )
	{
		float depthBufferValue = GetDepthBufferValue( s );

		for( UINT y = 0; y < RT_HEIGHT; y++ )
		{
			for( UINT x = 0; x < RT_WIDTH; x++ )
			{
				float srcPixel = RTContents[pixelIndex];
				pixelIndex++;

				float expectedPixel;

				if( 0 != ( m_DepthTestResult & (1 << s) ) ) 
				{
					if( m_ShaderEarlyDepth )
					{
						// Interpolated vertex value should be used
						// Even if the pixel shader is discarded, the depth test and write still occurs
						expectedPixel = 0.75f;
					}
					else if( (m_DiscardBeforeWrite || m_DiscardAfterWrite) &&
						(x > 0) )
					{ 
						// Pixel was discarded, depth write should be ignored
						expectedPixel = depthBufferValue;
					}
					else if( m_WriteODepth )
					{
						// Shader writes to ODepth, and does not force early depth test
						// so the shader written value of 0.5 should be used
						expectedPixel = 0.5f;
					}
					else
					{
						// Interpolated vertex value should be used
						expectedPixel = 0.75f;
					}
				}
				else
				{
					expectedPixel = depthBufferValue;
				}

				if( !CompareFloatEpsilon(expectedPixel, srcPixel, 0.01f ) )
				{
					WriteToLog( "Depth buffer contained unexpected value.  Pixel Position = (%u, %u).  Sample index = %u.  Found pixel = %f.  Expected value = %f",
						x, y, s, srcPixel, expectedPixel );
					tr = RESULT_FAIL;
					goto cleanup;
				}
			}
		}
	}

cleanup:
	return tr;
}

struct UAVPixel
{
	UINT R;
	UINT G;
};

TEST_RESULT CPixelShaderUAVWrite::CheckResults_UAV()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;
	ID3D11Resource* pStagingResource = NULL;
	ID3D11Resource* pDefaultResource = NULL;
	ID3D11Buffer* pStagingBuffer = NULL;
	ID3D11Texture1D* pStagingTexture1D = NULL;
	ID3D11Texture2D* pStagingTexture2D = NULL;
	ID3D11Texture3D* pStagingTexture3D = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedSR;

	// create staging resource
	switch( m_UAVDimension )
	{
	case D3D11_UAV_DIMENSION_BUFFER:
		{
			D3D11_BUFFER_DESC stagingDesc;
			m_pUAVBuffer->GetDesc( &stagingDesc );

			stagingDesc.Usage = D3D11_USAGE_STAGING;
			stagingDesc.BindFlags = 0;
			stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			stagingDesc.MiscFlags = 0;
			stagingDesc.StructureByteStride = 0;

			hr = GetDevice()->CreateBuffer( &stagingDesc, NULL, &pStagingBuffer );
			if ( FAILED( hr ) )
			{
				WriteToLog( "CreateBuffer failed for staging buffer.  hr = %s", D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto cleanup;
			}

			pStagingResource = pStagingBuffer;
			pDefaultResource = m_pUAVBuffer;
		}
		break;

	case D3D11_UAV_DIMENSION_TEXTURE1D:
		{
			D3D11_TEXTURE1D_DESC stagingDesc;
			m_pUAVTexture1D->GetDesc( &stagingDesc );

			stagingDesc.Usage = D3D11_USAGE_STAGING;
			stagingDesc.BindFlags = 0;
			stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			stagingDesc.MiscFlags = 0;

			hr = GetDevice()->CreateTexture1D( &stagingDesc, NULL, &pStagingTexture1D );
			if ( FAILED( hr ) )
			{
				WriteToLog( "CreateTexture1D failed for staging buffer.  hr = %s", D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto cleanup;
			}

			pStagingResource = pStagingTexture1D;
			pDefaultResource = m_pUAVTexture1D;
		}
		break;

	case D3D11_UAV_DIMENSION_TEXTURE2D:
		{
			D3D11_TEXTURE2D_DESC stagingDesc;
			m_pUAVTexture2D->GetDesc( &stagingDesc );

			stagingDesc.Usage = D3D11_USAGE_STAGING;
			stagingDesc.BindFlags = 0;
			stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			stagingDesc.MiscFlags = 0;

			hr = GetFramework()->CreateTexture2DMapDefaultOptional( &stagingDesc, NULL, &pStagingTexture2D );
			if ( FAILED( hr ) )
			{
				WriteToLog( "CreateTexture2D failed for staging buffer.  hr = %s", D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto cleanup;
			}

			pStagingResource = pStagingTexture2D;
			pDefaultResource = m_pUAVTexture2D;
		}
		break;

	case D3D11_UAV_DIMENSION_TEXTURE3D:
		{
			D3D11_TEXTURE3D_DESC stagingDesc;
			m_pUAVTexture3D->GetDesc( &stagingDesc );

			stagingDesc.Usage = D3D11_USAGE_STAGING;
			stagingDesc.BindFlags = 0;
			stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			stagingDesc.MiscFlags = 0;

			hr = GetFramework()->CreateTexture3DMapDefaultOptional( &stagingDesc, NULL, &pStagingTexture3D );
			if ( FAILED( hr ) )
			{
				WriteToLog( "CreateTexture3D failed for staging buffer.  hr = %s", D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto cleanup;
			}

			pStagingResource = pStagingTexture3D;
			pDefaultResource = m_pUAVTexture3D;
		}
		break;

	default:
		assert( false );
	}
	

	// Copy to staging buffer
	GetImmediateContext()->CopyResource( pStagingResource, pDefaultResource );

	// Map staging buffer
	hr = GetImmediateContext()->Map( pStagingResource, 0, D3D11_MAP_READ, 0, &mappedSR );
	if ( FAILED( hr ) )
	{
		WriteToLog( "Failed to map staging buffer.  hr = %s", D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	UINT uavInitialData = GetInitialUAVData( m_UAVAccessMethod );

	const UAVPixel* pixels = (const UAVPixel*)mappedSR.pData;

	UINT numExpectedElements = 0;

	for ( UINT x = 0; x < UAV_WIDTH; x++ )
	{
		for( UINT s = 0; s < m_SampleCount; s++ )
		{
			UAVPixel pixel = pixels[(x * m_SampleCount) + s];

			bool depthTestResult = 0 != ( m_DepthTestResult & (1 << s) );

			bool discardedBeforeWrite = m_DiscardBeforeWrite && (x > 0);

			bool expectWrite;

			if( m_ShaderEarlyDepth )
			{
				// [earlydepthstencil] is enabled
				// The UAV write should occur if the depth test passed, 
				// and the PS did not discard before the UAV write
				expectWrite = depthTestResult && !discardedBeforeWrite;
			}
			else
			{
				// [earlydepthstencil] is disabled
				// The UAV write should occur if the PS did not discard before the UAV write
				// the results of the depth test do not matter
				expectWrite = !discardedBeforeWrite;
			}

			// For Append & Counter UAVs
			// the src pixel position does not determine the write location
			// The only thing that is tested is the number of elements written
			if( (UAV_APPEND == m_UAVType) || (UAV_COUNTER == m_UAVType) )
			{
				if( expectWrite )
				{
					numExpectedElements++;
				}
			}
			else
			{
				UINT byteOffset = ((x * m_SampleCount) + s) * sizeof( UAVPixel );

				if( expectWrite )
				{
					if( (pixel.R == uavInitialData) || (pixel.G == uavInitialData) )
					{
						WriteToLog( "UAV contained initial data at position where the UAV write should have ocurred.  Byte offset in UAV = 0x%x.  UAV initial data = 0x%x.  Sample Index = %u.", byteOffset, uavInitialData, s );
						tr = RESULT_FAIL;
						goto cleanup;
					}
				}
				else
				{
					if( (pixel.R != uavInitialData) || (pixel.G != uavInitialData) )
					{
						WriteToLog( "UAV contained non-initial data data at position where the UAV write should not have ocurred.  Byte offset in UAV = 0x%x.  UAV initial data = 0x%x.  Sample Index = %u.", byteOffset, uavInitialData, s );
						tr = RESULT_FAIL;
						goto cleanup;
					}
				}
			}
		}
	}

	if( (UAV_APPEND == m_UAVType) || (UAV_COUNTER == m_UAVType) )
	{
		for( UINT i = 0; i < numExpectedElements; i++ )
		{
			UAVPixel pixel = pixels[i];

			if( (pixel.R == uavInitialData) || (pixel.G == uavInitialData) )
			{
				WriteToLog( "Append UAV did not have enough elements appended.  Number of expected elements = %u.  Number of elements found = %u", numExpectedElements, i );
				tr = RESULT_FAIL;
				goto cleanup;
			}
		}

		UINT numRemaining = (UAV_WIDTH * m_SampleCount) - numExpectedElements;

		for( UINT i = 0; i < numRemaining; i++ )
		{
			UAVPixel pixel = pixels[numExpectedElements + i];

			if( (pixel.R != uavInitialData) || (pixel.G != uavInitialData) )
			{
				WriteToLog( "Append UAV had too many elements appended.  Number of expected elements = %u.  Number of elements found = %u", numExpectedElements, numExpectedElements + i );
				tr = RESULT_FAIL;
				goto cleanup;
			}
		}
	}

	GetImmediateContext()->Unmap( pStagingResource, 0 );

cleanup:
	SAFE_RELEASE( pStagingBuffer );
	SAFE_RELEASE( pStagingTexture1D );
	SAFE_RELEASE( pStagingTexture2D );
	SAFE_RELEASE( pStagingTexture3D );
	return tr;
}

TEST_RESULT CPixelShaderUAVWrite::CheckResults_Query()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_FALSE;

	// get the query data
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	ZeroMemory( &pipelineStats, sizeof(pipelineStats) );

	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pQuery, &pipelineStats, sizeof(pipelineStats), 0 );
	if( hr != S_OK )
	{
		WriteToLog( "GetData failed or timed out for pipeline stats query.  hr = %s", D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto cleanup;
	}

	if( m_ShaderEarlyDepth && (0 == m_DepthTestResult) )
	{
		if( pipelineStats.PSInvocations > 0 )
		{
			WriteToLog( "PSInvocations was %u when it was expected to be 0.  The depth test failed for all active pixels, and the shader has the [earlydepthstencil] attribute.", (UINT)pipelineStats.PSInvocations );
			tr = RESULT_FAIL;
			goto cleanup;
		}
	}

	UINT pixelCount = RT_WIDTH * RT_HEIGHT;

	if( !m_ShaderEarlyDepth && (0 == m_DepthTestResult) )
	{
		if( pipelineStats.PSInvocations < pixelCount )
		{
			WriteToLog( "PSInvocations was %u when it was expected to be at least %u.  The depth test failed for all active pixels, but the shader writes to a UAV and does not have the [earlydepthstencil] attribute.", (UINT)pipelineStats.PSInvocations, pixelCount );
			tr = RESULT_FAIL;
			goto cleanup;
		}
	}

	if( m_DepthTestResult & 0x1 )
	{
		if( pipelineStats.PSInvocations < pixelCount )
		{
			WriteToLog( "PSInvocations was %u when it was expected to be at least %u.  The depth test passed for at least 1 sample in each pixel.", (UINT)pipelineStats.PSInvocations, pixelCount );
			tr = RESULT_FAIL;
			goto cleanup;
		}
	}

cleanup:
	return tr;
}


float CPixelShaderUAVWrite::GetDepthBufferValue(UINT Sample)
{
	float depthValue = 0.0f;
	bool shouldPassDepthTest = 0 != (m_DepthTestResult & ( 1 << Sample ));

	switch( m_DepthFunc )
	{
	case D3D11_COMPARISON_LESS:
	case D3D11_COMPARISON_LESS_EQUAL:
		depthValue = shouldPassDepthTest ? 1.0f : 0.0f;
		break;

	case D3D11_COMPARISON_GREATER:
	case D3D11_COMPARISON_GREATER_EQUAL:
		depthValue = shouldPassDepthTest ? 0.0f : 1.0f;
		break;
		
	default:
		assert( false );
	}

	return depthValue;
}

