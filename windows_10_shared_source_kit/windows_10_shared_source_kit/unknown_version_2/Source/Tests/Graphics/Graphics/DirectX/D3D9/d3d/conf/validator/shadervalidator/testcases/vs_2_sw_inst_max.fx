  "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	breakp p0.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_137_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_3_0 : texldl v# inside rep block with breakp p.z is allowed";
string VS_137 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.z "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_138_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_3_0 : texldl v# inside rep block with breakp !p.z is allowed";
string VS_138 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	breakp !p0.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_139_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_3_0 : texldl v# inside rep block with breakp !p.w is allowed";
string VS_139 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.w "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_140_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_3_0 : texldl v# inside rep block with breakp p.w is allowed";
string VS_140 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	breakp p0.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
    //////////////////////////////////////////////////////////////////////////
//  Topology.cpp
//  created:	2005/04/25
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "Topology.h"

const char szVSSource[]= 
	"dcl_input v[0].xy\n" // Number of emit calls
	"dcl_output o[0].xy\n"
	"mov o[0].xy, v[0].xy\n"
	;
	
const char szGSHeaderComponentCount3[] = 
	"dcl_temps 2\n"
	"dcl_input v[1][0].xy\n"
	"%s\n"	// Declare stream if needed (Can't have dcl_streams after any output decls)
	"dcl_output o[0].xyz\n"
	"dcl_inputPrimitive point\n"
	"dcl_maxOutputVertexCount 341\n" // floor( 1024 / 3 ) == 341
	;

const char szGSHeaderComponentCount4[] = 
	"dcl_temps 2\n"
	"dcl_input v[1][0].xy\n"	// Number of emit calls per GS invocation
	"%s\n"	// Declare stream if needed (Can't have dcl_streams after any output decls)
	"dcl_output o[0].xyzw\n"		// 4 components
	"dcl_inputPrimitive point\n"
	"dcl_maxOutputVertexCount 256\n" // floor( 1024 / 4 ) == 256
	;

const char szGSHeaderComponentCount7[] = 
	"dcl_temps 2\n"
	"dcl_input v[1][0].xy\n"	// Number of emit calls per GS invocation
	"%s\n"	// Declare stream if needed (Can't have dcl_streams after any output decls)
	"dcl_output o[0].xyzw\n"	// 4 +
	"dcl_output o[1].xyz\n"		// 3 components = 7 component output vertex
	"dcl_inputPrimitive point\n"
	"dcl_maxOutputVertexCount 146\n" // floor( 1024 / 7 ) == 146
	;

const char szGSOutputTristrip[] =
	"dcl_outputTopology trianglestrip\n"
	;
	
const char szGSOutputLineStrip[] =
	"dcl_outputTopology linestrip\n"
	;
	
const char szGSOutputPointList[] =
	"dcl_outputTopology pointlist\n"
	;
	
const char szGSSourceComponentCount4[] =
	"mov r0, v[0][0].xy\n"
	"mov r1.x, 0\n"
	"loop\n"
		"iadd r1.y, r1.x, 1\n"
		"iadd r1.z, r1.x, 2\n"
		"iadd r1.w, r1.x, 3\n"
		"mov o[0], r1\n" // write to 4-component output
		"%s\n"	// Emit instruction
		
		// See if we are finished emitting vertices.
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r1.x, r1.x, 1\n"
	"endloop\n"
	;

const char szGSSourceComponentCount7[] =
	"mov r0, v[0][0].xy\n"
	"mov r1.x, 0\n"
	"loop\n"
		"iadd r1.y, r1.x, 1\n"
		"iadd r1.z, r1.x, 2\n"
		"iadd r1.w, r1.x, 3\n"
		"mov o[0], r1\n"		// write to first 4-component output
		"mov o[1].xyz, r1\n"  // write to second 3-component output
		"%s\n"	// Emit instruction
		
		// See if we are finished emitting vertices.
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r1.x, r1.x, 1\n"
	"endloop\n"
	;

const char szGSSourceComponentCount3[] =
	"mov r0, v[0][0].xy\n"
	"mov r1.x, 0\n"
	"loop\n"
		"iadd r1.y, r1.x, 1\n"
		"iadd r1.z, r1.x, 2\n"
		"iadd r1.w, r1.x, 3\n"
		"mov o[0].xyz, r1\n" // write to 3-component output
		"%s\n"	// Emit instruction
		
		// See if we are finished emitting vertices.
		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r1.x, r1.x, 1\n"
	"endloop\n"
	;

const char szGSSourceCut[] =
	"mov r0, v[0][0].xy\n"
	"mov r1.x, 0\n"
	"loop\n"
		"iadd r1.y, r1.x, 1\n"
		"iadd r1.z, r1.x, 2\n"
		"iadd r1.w, r1.x, 3\n"
		"mov o[0], r1\n"
		"%s\n"	// Emit instruction

		// See if we need to insert a cut.
		"iadd r0.y, r0.y, 1\n"
		"if_z r0.y\n"
			"mov r0.y, v[0][0].y\n" // Reset cut counter
			"%s\n"	// Cut instruction
		"endif\n"

		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r1.x, r1.x, 1\n"
	"endloop\n"
	;

const char szGSSourceEmitCut[] =
	"mov r0, v[0][0].xy\n"
	"mov r1.x, 0\n"
	"loop\n"
		"iadd r1.y, r1.x, 1\n"
		"iadd r1.z, r1.x, 2\n"
		"iadd r1.w, r1.x, 3\n"
		"mov o[0], r1\n"

		// See if we need to insert a cut.
		"iadd r0.y, r0.y, 1\n"
		"if_z r0.y\n"
			"mov r0.y, v[0][0].y\n" // Reset cut counter
			"%s\n"	// Emit then cut instruction
		"else\n"
			"%s\n"	// Emit instruction
		"endif\n"

		"iadd r0.x, r0.x, -1\n"
		"breakc_z r0.x\n"
		"iadd r1.x, r1.x, 1\n"
	"endloop\n"
	;

BEGIN_NAMED_VALUES( CShader5xTest_Topology::OUTPUT_TOPOLOGY )
	NAMED_VALUE( _T( "pointlist" ), CShader5xTest_Topology::OT_POINTLIST )
	NAMED_VALUE( _T( "linestrip" ), CShader5xTest_Topology::OT_LINESTRIP )
	NAMED_VALUE( _T( "trianglestrip" ), CShader5xTest_Topology::OT_TRIANGLESTRIP )
END_NAMED_VALUES( CShader5xTest_Topology::OUTPUT_TOPOLOGY )


void CShader5xTest_Topology::Cleanup()
{
	ID3D11Buffer *pBufferNull[] = { NULL, NULL, NULL, NULL };
	UINT strides[] = { 0, 0, 0, 0 };
	UINT offsets[] = { 0, 0, 0, 0 };

	if( GetEffectiveContext() )
	{	
		GetEffectiveContext()->IASetVertexBuffers( 0, 4, pBufferNull, strides, offsets );
		GetEffectiveContext()->IASetInputLayout( NULL );
		GetEffectiveContext()->RSSetState( NULL );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
		}
	}
	
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pRS );
	SAFE_RELEASE( m_pSOStats[0] );
	SAFE_RELEASE( m_pSOStats[1] );
	SAFE_RELEASE( m_pSOStats[2] );
	SAFE_RELEASE( m_pSOStats[3] );

	return CShader5xTest::Cleanup();
}

TEST_RESULT CShader5xTest_Topology::Setup()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	m_soBufferSize = 65536;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	// Generate the shader profile, like vs_5_0
	const tstring shaderProfile = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_VERTEX,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	tstring fullVSSource = "";
	
	tRes = CShader5xTest::Setup();
	if( tRes != RESULT_PASS )
		goto setupDone;

	// Build pass-through VS.
	fullVSSource = shaderProfile + " \n" + szVSSource;

	if( !AssembleShader( fullVSSource.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "AssembleShader() failed assembling VS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		tRes = RESULT_FAIL;
		goto setupDone;
	}
	
	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS ) ) )
	{
		WriteToLog( _T( "CreateVertexShader() failed, hr=%s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}
	
	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	
	if( g_bDumpDissasm )
	MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	// Create a vertex buffer
	D3D11_BUFFER_DESC desc;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = 2 * sizeof( UINT );
	if( FAILED( hr = GetDevice()->CreateBuffer( &desc, NULL, &m_pVertexBuffer ) ) )
	{
		WriteToLog( 1, _T( "CDevice::CreateBuffer() failed to create vertex buffer, hr=%s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}
	
	// Bind the vertex buffer
	ID3D11Buffer *buffers[] = { m_pVertexBuffer };
	UINT strides[] = { 2 * sizeof( UINT ) };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, buffers, strides, offsets );

	// Set input layout.
	D3D11_INPUT_ELEMENT_DESC pIEDesc[] =
	{
		{ "0_", 0, DXGI_FORMAT_R32G32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	if( FAILED( hr = GetDevice()->CreateInputLayout( pIEDesc, 1, pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pInputLayout ) ) )
	{
		WriteToLog( _T( "CreateInputLayout() failed, hr=%s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}
	
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );
	// Disable rasterizer
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.f;
	rd.SlopeScaledDepthBias = 0.f;
	rd.DepthClipEnable = false;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	
	if( FAILED( hr = GetDevice()->CreateRasterizerState( &rd, &m_pRS ) ) )
	{
		WriteToLog( _T( "CreateRasterizerState() failed, hr=%s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}
	GetEffectiveContext()->RSSetState( m_pRS );
	
	// Create query object to keep track of number of output primitives.
	D3D11_QUERY_DESC qd;
	D3D11_QUERY queries[] = 
	{	
		D3D11_QUERY_SO_STATISTICS_STREAM0, 
		D3D11_QUERY_SO_STATISTICS_STREAM1,
		D3D11_QUERY_SO_STATISTICS_STREAM2,
		D3D11_QUERY_SO_STATISTICS_STREAM3
	};

	qd.MiscFlags = 0;
	m_numSOQueries = ( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? 4 : 1;

	for(UINT i = 0; i < m_numSOQueries; i++)
	{
		// There are only multiple streams (and thus queries) for FL11+
		qd.Query = ( GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? queries[i] : D3D11_QUERY_SO_STATISTICS;

		if( FAILED( hr = GetDevice()->CreateQuery( &qd, &m_pSOStats[i] ) ) )
		{
			WriteToLog( _T( "CreateQuery() failed, hr=%s" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setupDone;
		}
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
		goto setupDone;
	}

setupDone:
	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( pErrorBuf );

	return tRes;
}

TEST_RESULT CShader5xTest_Topology::SetupTestCase()
{
	HRESULT hr;
	// Initialize the vertex buffer
	UINT pVBData[2];
	pVBData[0] = m_nEmitCount;
	pVBData[1] = m_nCutEvery;
	if (FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pVertexBuffer, pVBData) ) )
	{
		WriteToLog( _T( "Failed to map m_pVertexBuffer, hr=%s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	
	// Set the stream output buffer
	ID3D11Buffer *buffers[4] = { m_pStreamOutputBuffer, NULL, NULL, NULL };

	UINT offsets[] = { 0, 0, 0, 0 };
	GetEffectiveContext()->SOSetTargets( 1, buffers, offsets );
	
	return BuildGS();
}

TEST_RESULT CShader5xTest_Topology::BuildGS()
{
	HRESULT hr;
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	SAFE_RELEASE( m_pGS );
	
	const D3D11_SO_DECLARATION_ENTRY soDecl3[] =
	{
		{ m_stream, "0_", 0, 0, 3, 0 },
	};
	
	const D3D11_SO_DECLARATION_ENTRY soDecl4[] =
	{
		{ m_stream, "0_", 0, 0, 4, 0 },
	};
	
	const D3D11_SO_DECLARATION_ENTRY soDecl7[] =
	{
		{ m_stream, "0_", 0, 0, 4, 0 },
		{ m_stream, "1_", 0, 0, 3, 0 },
	};
	
	// Compose GS
	char szBuffer[1024];
	char szGSHeader[ MAX_SHADERSTR ];
	char szGSStreamStr[ 50 ];
	const D3D11_SO_DECLARATION_ENTRY *soDecl = NULL;
	UINT numDeclEntries;

	// Form the string used for the stream decl
	if( m_bStreamsDeclared )
		sprintf( szGSStreamStr, "%s m%u", "dcl_stream ", m_stream );
	else
		sprintf( szGSStreamStr, "" );

	switch( m_gsOutputComponentCount )
	{
		case 3:
			sprintf( szGSHeader, szGSHeaderComponentCount3, szGSStreamStr );
			soDecl = soDecl3;
			numDeclEntries = 1;
			break;
			
		case 4:
			sprintf( szGSHeader, szGSHeaderComponentCount4, szGSStreamStr );
			soDecl = soDecl4;
			numDeclEntries = 1;
			break;
			
		case 7:
			sprintf( szGSHeader, szGSHeaderComponentCount7, szGSStreamStr );
			soDecl = soDecl7;
			numDeclEntries = 2;
			break;
	}
	size_t n = _snprintf( szBuffer, sizeof( szBuffer ) - 1, "%s", szGSHeader );
	
	const char *szGSTopology;
	switch( m_OutputTopology )
	{
		default:
		case OT_POINTLIST:
			szGSTopology = szGSOutputPointList;
			break;
			
		case OT_LINESTRIP:
			szGSTopology = szGSOutputLineStrip;
			break;
			
		case OT_TRIANGLESTRIP:
			szGSTopology = szGSOutputTristrip;
			break;
	}
	
	n += _snprintf( szBuffer + n, sizeof( szBuffer ) - n - 1, "%s", szGSTopology );
	n += _snprintf( szBuffer + n, sizeof( szBuffer ) - n - 1, "%s", GetGSSource() );
	
	// Make PreFast happy	
	szBuffer[1023] = 0;

	// Generate the shader profile, like vs_5_0
	const tstring shaderProfile = g_TestApp.GetShaderProfile(
		D3D_SHADER_STAGE_GEOMETRY,
		g_TestApp.m_D3DOptions.FeatureLevel,
		g_TestApp.m_D3DOptions.FeatureLevel);

	const tstring fullGSSource = shaderProfile + " \n" + szBuffer;
		
	// Build GS.
	if( !AssembleShader( fullGSSource.c_str(), &pShaderBuf, &pErrorBuf ) )
	{
		WriteToLog( _T( "AssembleShader() failed assembling GS: %s" ), pErrorBuf->GetBufferPointer() );
		goto fail;
	}
	
	const UINT pBufferStrides[] = 
	{ 
		2 * 4 * sizeof( UINT32 ), 
		0, 
		0, 
		0 
	};
	
	// Create the GS
	hr = GetDevice()->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), 
		soDecl, numDeclEntries, pBufferStrides, 1, 0, NULL, &m_pGS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateGeometryShaderWithStreamOutput() failed, hr=%s" ), D3DHResultToString(hr).c_str() );
		goto fail;
	}
	
	// Assign the GS to the pipeline
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	
	if( g_bDumpDissasm )
	MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		goto fail;
	}

	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( pErrorBuf );

	return RESULT_PASS;

fail:
	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( pErrorBuf );
	
	return RESULT_FAIL;
};

TEST_RESULT CShader5xTest_Topology::ExecuteTestCase()
{
	HRESULT hr;
	int emitCount = 0;

	switch( m_gsOutputComponentCount )
	{
		case 3:
			emitCount = min( m_nEmitCount, 1024 / 3 );
			break;
		case 4:
			emitCount = min( m_nEmitCount, 1024 / 4 );
			break;
		case 7:
			emitCount = min( m_nEmitCount, 1024 / 7 );
			break;
	}
	
	// Calculate the number of expected primitives.
	int nPrimCount;
	typedef DWORD Vertex[2][4];
	
	switch( m_OutputTopology )
	{
		case OT_POINTLIST:
			nPrimCount = emitCount;
			break;
			
		case OT_LINESTRIP:
			nPrimCount = max( 0, emitCount - 1 );
			break;
			
		case OT_TRIANGLESTRIP:
			nPrimCount = max( 0, emitCount - 2 );
			break;
			
		default:
			nPrimCount = 0;
	}
	
	// Query the number of output primitives generated
	
	for(UINT i = 0; i < m_numSOQueries; i++)
	{
		GetEffectiveContext()->Begin(m_pSOStats[i]);
	}
	GetEffectiveContext()->Draw( 1, 0 );
	for(UINT i = 0; i < m_numSOQueries; i++)
	{
		GetEffectiveContext()->End(m_pSOStats[i]);
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	UINT64 NumPrimitivesWritten = 0;
	for(UINT i = 0; i < m_numSOQueries; i++)
	{
		D3D11_QUERY_DATA_SO_STATISTICS sostats;
		hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pSOStats[i], ( void **) &sostats, sizeof( sostats ), 0 );
		if (hr != S_OK)
		{
			WriteToLog( "GetData failed or timed out, hr=%s", D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		NumPrimitivesWritten += sostats.NumPrimitivesWritten;
	}
	
	if( NumPrimitivesWritten != nPrimCount )
	{
		WriteToLog( "%i primitives generated, expected %i", NumPrimitivesWritten, nPrimCount );
		return RESULT_FAIL;
	}
	
	TEST_RESULT r = RESULT_PASS;
	
	UINT nPrimSize = 0;
	switch( m_OutputTopology )
	{
		case OT_POINTLIST:
			nPrimSize = 1;
			break;
			
		case OT_LINESTRIP:
			nPrimSize = 2;
			break;
			
		case OT_TRIANGLESTRIP:
			nPrimSize = 3;
			break;
	}

	// Examine the primitive output
	Vertex *pResult;
	D3D11_BOX box;
	box.left = 0;
	box.right = STREAM_OUTPUT_BUFFER_SIZE;
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;
	GetImmediateContext()->CopySubresourceRegion( m_pStreamOutputBufferCopy, 0, 0, 0, 0, m_pStreamOutputBuffer, 0, &box );
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	if (FAILED( hr = GetImmediateContext()->Map( m_pStreamOutputBufferCopy, 0, D3D11_MAP_READ, 0, &mappedBuffer ) ) )
	{
		WriteToLog( _T( "Failed to map m_pStreamOutputBufferCopy, hr=%s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	pResult = (Vertex*)(mappedBuffer.pData);
	
	UINT nPrimVertsLeft = nPrimSize;
	UINT nStart = 0;
	UINT nCut = m_nCutEvery;
	
	for( UINT i = emitCount - 1; i != (UINT) -1; --i )
	{
		nPrimVertsLeft--;
		
		if( nPrimVertsLeft == 0 )
		{
			bool bEven = !( nStart & 1 );

			// Find the leading vertex
			Vertex *pVertex[3] = {0};
			
			switch( m_OutputTopology )
			{
				case OT_POINTLIST:
					pVertex[0] = &pResult[0];
					if( (*pVertex[0])[0][0] != nStart )
						pVertex[0] = NULL;
					break;
					
				case OT_LINESTRIP:
					pVertex[0] = &pResult[0];
					pVertex[1] = &pResult[1];
					
					if( ( (*pVertex[0])[0][0] != nStart + 0 ) ||
						( (*pVertex[0])[0][1] != nStart + 1 ) )
							pVertex[0] = NULL;
					break;
					
				case OT_TRIANGLESTRIP:
					for( UINT z = 0; z < nPrimSize; ++z )
					{
						// Triangles only
						if( pResult[z][0][0] == ( bEven ? nStart : nStart + 2 ) )
						{
							pVertex[0] = &pResult[z];
							pVertex[1] = &pResult[(z + 1)%3];
							pVertex[2] = &pResult[(z + 2)%3];
							
							if( bEven )
							{
								// Make sure the verts are ordered correctly
								if( ( (*pVertex[1])[0][0] != nStart + 1 ) ||
									( (*pVertex[2])[0][0] != nStart + 2 ) )
										pVertex[0] = NULL;
							}
							else
							{
								// Make sure the verts are ordered correctly
								if( ( (*pVertex[1])[0][0] != nStart + 1 ) ||
									( (*pVertex[2])[0][0] != nStart + 0 ) )
										pVertex[0] = NULL;
							}
							break;
						}
					}
					break;
			}
						
			if( !pVertex[0] )
			{
				r = RESULT_FAIL;
			}
			
			if( r == RESULT_PASS )
			{
				// Make sure each vertex contains expected data
				for( UINT n = 0; n < nPrimSize; ++n )
				{
					switch( m_gsOutputComponentCount )
					{
						case 3:
							if( ( (*pVertex[n])[0][1] != (*pVertex[n])[0][0] + 1 ) ||
								( (*pVertex[n])[0][2] != (*pVertex[n])[0][0] + 2 ) )
									r =RESULT_FAIL;
							break;
						case 4:
							if( ( (*pVertex[n])[0][1] != (*pVertex[n])[0][0] + 1 ) ||
								( (*pVertex[n])[0][2] != (*pVertex[n])[0][0] + 2 ) ||
								( (*pVertex[n])[0][3] != (*pVertex[n])[0][0] + 3 ) )
									r =RESULT_FAIL;
							break;
						case 7:
							if( ( (*pVertex[n])[0][1] != (*pVertex[n])[0][0] + 1 ) ||
								( (*pVertex[n])[0][2] != (*pVertex[n])[0][0] + 2 ) ||
								( (*pVertex[n])[0][3] != (*pVertex[n])[0][0] + 3 ) )
									r =RESULT_FAIL;
							if( ( (*pVertex[n])[1][0] != (*pVertex[n])[0][0] + 0 ) ||
								( (*pVertex[n])[1][1] != (*pVertex[n])[0][0] + 1 ) ||
								( (*pVertex[n])[1][2] != (*pVertex[n])[0][0] + 2 ) )
									r =RESULT_FAIL;
							break;
							
					}
				}
			}
			
			if( r != RESULT_PASS )
			{			
				WriteToLog( _T( "Invalid data in output stream." ) );
			}
			
			pResult += nPrimSize;
			
			nPrimVertsLeft = 1;
			nStart++;
		}
		
		if( nCut )
		{
			if( --nCut )
			{
				nPrimVertsLeft = nPrimSize;
				nCut = m_nCutEvery;
			}
		}
	}

	GetImmediateContext()->Unmap( m_pStreamOutputBufferCopy, 0 );
	
	return r;
}

void CShader5xTest_Topology::CleanupTestCase()
{
}

TEST_RESULT CShader5xTest_TopologyStream::Setup()
{
	// Call the base Setup to determine if should skip
	TEST_RESULT tRes = CShader5xTest_Topology::Setup();
	
	if( RESULT_PASS != tRes )
		return tRes;

	// Stream tests are only valid on FL11+
	if( GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
	{
		WriteToLog( _T("CShader5xTest_TopologyStream::Setup() - emit_stream, cut_stream, and emitThenCut_stream instructions are only FL11+\n") );
		return RESULT_SKIP;
	}

	return tRes;
}


//////////////////////////////////////////////////////////////////////////
// CShader5xTest_emit
//////////////////////////////////////////////////////////////////////////

CShader5xTest_emit::CShader5xTest_emit()
{
}

const char *CShader5xTest_emit::GetGSSource()
{
	switch( m_gsOutputComponentCount )
	{
		case 3:
			sprintf( m_szGSSourceShader, szGSSourceComponentCount3, "emit");
			break;
			
		case 4:
			sprintf( m_szGSSourceShader, szGSSourceComponentCount4, "emit");
			break;
			
		case 7:
			sprintf( m_szGSSourceShader, szGSSourceComponentCount7, "emit");
			break;
		default:
			return NULL;
	}

	return m_szGSSourceShader;
}

void CShader5xTest_emit::InitTestParameters()
{
	CUserValueSet< OUTPUT_TOPOLOGY > *pOutputTopologies = new CUserValueSet< OUTPUT_TOPOLOGY >();
	pOutputTopologies->AddValue( OT_POINTLIST );
	pOutputTopologies->AddValue( OT_LINESTRIP );
	pOutputTopologies->AddValue( OT_TRIANGLESTRIP );
	
	CUserValueSet< UINT > *pNumOutputComponentsValueSet = new CUserValueSet< UINT >();
	pNumOutputComponentsValueSet->AddValue( 4 );
	pNumOutputComponentsValueSet->AddValue( 3 );
	pNumOutputComponentsValueSet->AddValue( 7 );
	
	CUserValueSet< UINT > *pEmitCountValueSet = new CUserValueSet< UINT >();
	pEmitCountValueSet->AddValue( 1 );
	pEmitCountValueSet->AddValue( 3 );
	pEmitCountValueSet->AddValue( 4 );
	pEmitCountValueSet->AddValue( 37 );
	pEmitCountValueSet->AddValue( 146 );
	pEmitCountValueSet->AddValue( 256 );
	pEmitCountValueSet->AddValue( 341 );
	pEmitCountValueSet->AddValue( 1024 );
	
	AddParameter( "OutputTopology", &m_OutputTopology, pOutputTopologies );
	AddParameter( "NumOutputComponents", &m_gsOutputComponentCount, pNumOutputComponentsValueSet );
	AddParameter( "EmitCount", &m_nEmitCount, pEmitCountValueSet );
}

//////////////////////////////////////////////////////////////////////////
// CShader5xTest_cut
//////////////////////////////////////////////////////////////////////////
CShader5xTest_cut::CShader5xTest_cut()
{
}

const char *CShader5xTest_cut::GetGSSource()
{
	sprintf( m_szGSSourceShader, szGSSourceCut, "emit", "cut");
	return m_szGSSourceShader;
}

void CShader5xTest_cut::InitTestParameters()
{
	CUserValueSet< OUTPUT_TOPOLOGY > *pOutputTopologies = new CUserValueSet< OUTPUT_TOPOLOGY >();
	pOutputTopologies->AddValue( OT_POINTLIST );
	pOutputTopologies->AddValue( OT_LINESTRIP );
	pOutputTopologies->AddValue( OT_TRIANGLESTRIP );
	
	AddParameter( "OutputTopology", &m_OutputTopology, pOutputTopologies );
	AddParameter( "EmitCount", &m_nEmitCount, new CRangeValueSet< UINT >( 1, 12, 1 ) );
	AddParameter( "CutEvery", &m_nCutEvery, new CRangeValueSet< UINT >( 2, 5, 1 ) );

	m_gsOutputComponentCount = 4;
}

//////////////////////////////////////////////////////////////////////////
// CShader5xTest_emitThenCut
//////////////////////////////////////////////////////////////////////////
CShader5xTest_emitThenCut::CShader5xTest_emitThenCut()
{
}

const char *CShader5xTest_emitThenCut::GetGSSource()
{
	sprintf( m_szGSSourceShader, szGSSourceEmitCut, "emitthencut", "emit");
	return m_szGSSourceShader;
}

void CShader5xTest_emitThenCut::InitTestParameters()
{
	CUserValueSet< OUTPUT_TOPOLOGY > *pOutputTopologies = new CUserValueSet< OUTPUT_TOPOLOGY >();
	pOutputTopologies->AddValue( OT_POINTLIST );
	pOutputTopologies->AddValue( OT_LINESTRIP );
	pOutputTopologies->AddValue( OT_TRIANGLESTRIP );
	
	AddParameter( "OutputTopology", &m_OutputTopology, pOutputTopologies );
	AddParameter( "EmitCount", &m_nEmitCount, new CRangeValueSet< UINT >( 1, 12, 1 ) );
	AddParameter( "CutEvery", &m_nCutEvery, new CRangeValueSet< UINT >( 2, 5, 1 ) );

	m_gsOutputComponentCount = 4;
}

//////////////////////////////////////////////////////////////////////////
// CShader5xTest_emitStream
//////////////////////////////////////////////////////////////////////////

CShader5xTest_emitStream::CShader5xTest_emitStream()
{
	m_bStreamsDeclared = true;
}

const char *CShader5xTest_emitStream::GetGSSource()
{
	// Local variables
	char szGSStreamStr[ 50 ];

	// Form the string used for the stream decl
	sprintf( szGSStreamStr, "%s m%u", "emit_stream", m_stream );

	switch( m_gsOutputComponentCount )
	{
		case 3:
			sprintf( m_szGSSourceShader, szGSSourceComponentCount3, szGSStreamStr );
			break;
			
		case 4:
			sprintf( m_szGSSourceShader, szGSSourceComponentCount4, szGSStreamStr );
			break;
			
		case 7:
			sprintf( m_szGSSourceShader, szGSSourceComponentCount7, szGSStreamStr );
			break;
		default:
			return NULL;
	}

	return m_szGSSourceShader;
}

void CShader5xTest_emitStream::InitTestParameters()
{
	CUserValueSet< OUTPUT_TOPOLOGY > *pOutputTopologies = new CUserValueSet< OUTPUT_TOPOLOGY >();
	pOutputTopologies->AddValue( OT_POINTLIST );
	pOutputTopologies->AddValue( OT_LINESTRIP );
	pOutputTopologies->AddValue( OT_TRIANGLESTRIP );
	
	CUserValueSet< UINT > *pNumOutputComponentsValueSet = new CUserValueSet< UINT >();
	pNumOutputComponentsValueSet->AddValue( 4 );
	pNumOutputComponentsValueSet->AddValue( 3 );
	pNumOutputComponentsValueSet->AddValue( 7 );
	
	CUserValueSet< UINT > *pEmitCountValueSet = new CUserValueSet< UINT >();
	pEmitCountValueSet->AddValue( 1 );
	pEmitCountValueSet->AddValue( 3 );
	pEmitCountValueSet->AddValue( 4 );
	pEmitCountValueSet->AddValue( 37 );
	pEmitCountValueSet->AddValue( 146 );
	pEmitCountValueSet->AddValue( 256 );
	pEmitCountValueSet->AddValue( 341 );
	pEmitCountValueSet->AddValue( 1024 );

	CUserValueSet< UINT > *pStreamIndexValueSet = new CUserValueSet< UINT >();
	pStreamIndexValueSet->AddValue( 0 );
	pStreamIndexValueSet->AddValue( 1 );
	pStreamIndexValueSet->AddValue( 2 );
	pStreamIndexValueSet->AddValue( 3 );
	
	AddParameter( "OutputTopology", &m_OutputTopology, pOutputTopologies );
	AddParameter( "NumOutputComponents", &m_gsOutputComponentCount, pNumOutputComponentsValueSet );
	AddParameter( "EmitCount", &m_nEmitCount, pEmitCountValueSet );
	AddParameter( "Stream", &m_stream, pStreamIndexValueSet );
}

//////////////////////////////////////////////////////////////////////////
// CShader5xTest_cutStream
//////////////////////////////////////////////////////////////////////////
CShader5xTest_cutStream::CShader5xTest_cutStream()
{
	m_bStreamsDeclared = true;
}

const char *CShader5xTest_cutStream::GetGSSource()
{
	// Local variables
	char szGSEmitStreamStr[ 50 ];
	char szGSCutStreamStr[ 50 ];

	// Form the string used for the stream decl
	sprintf( szGSEmitStreamStr, "%s m%u", "emit_stream", m_stream );
	sprintf( szGSCutStreamStr, "%s m%u", "cut_stream", m_stream );

	sprintf( m_szGSSourceShader, szGSSourceCut, szGSEmitStreamStr, szGSCutStreamStr);
	return m_szGSSourceShader;
}

void CShader5xTest_cutStream::InitTestParameters()
{
	CUserValueSet< OUTPUT_TOPOLOGY > *pOutputTopologies = new CUserValueSet< OUTPUT_TOPOLOGY >();
	pOutputTopologies->AddValue( OT_POINTLIST );
	pOutputTopologies->AddValue( OT_LINESTRIP );
	pOutputTopologies->AddValue( OT_TRIANGLESTRIP );

	CUserValueSet< UINT > *pStreamIndexValueSet = new CUserValueSet< UINT >();
	pStreamIndexValueSet->AddValue( 0 );
	pStreamIndexValueSet->AddValue( 1 );
	pStreamIndexValueSet->AddValue( 2 );
	pStreamIndexValueSet->AddValue( 3 );
	
	AddParameter( "OutputTopology", &m_OutputTopology, pOutputTopologies );
	AddParameter( "EmitCount", &m_nEmitCount, new CRangeValueSet< UINT >( 1, 12, 1 ) );
	AddParameter( "CutEvery", &m_nCutEvery, new CRangeValueSet< UINT >( 2, 5, 1 ) );
	AddParameter( "Stream", &m_stream, pStreamIndexValueSet );

	m_gsOutputComponentCount = 4;
}

//////////////////////////////////////////////////////////////////////////
// CShader5xTest_emitThenCutStream
//////////////////////////////////////////////////////////////////////////
CShader5xTest_emitThenCutStream::CShader5xTest_emitThenCutStream()
{
	m_bStreamsDeclared = true;
}

const char *CShader5xTest_emitThenCutStream::GetGSSource()
{
	// Local variables
	char szGSEmitStreamStr[ 50 ];
	char szGSEmitThenCutStreamStr[ 50 ];

	// Form the string used for the stream decl
	sprintf( szGSEmitStreamStr, "%s m%u", "emit_stream", m_stream );
	sprintf( szGSEmitThenCutStreamStr, "%s m%u", "emitthencut_stream", m_stream );

	sprintf( m_szGSSourceShader, szGSSourceEmitCut, szGSEmitThenCutStreamStr, szGSEmitStreamStr);
	return m_szGSSourceShader;
}

void CShader5xTest_emitThenCutStream::InitTestParameters()
{
	CUserValueSet< OUTPUT_TOPOLOGY > *pOutputTopologies = new CUserValueSet< OUTPUT_TOPOLOGY >();
	pOutputTopologies->AddValue( OT_POINTLIST );
	pOutputTopologies->AddValue( OT_LINESTRIP );
	pOutputTopologies->AddValue( OT_TRIANGLESTRIP );

	CUserValueSet< UINT > *pStreamIndexValueSet = new CUserValueSet< UINT >();
	pStreamIndexValueSet->AddValue( 0 );
	pStreamIndexValueSet->AddValue( 1 );
	pStreamIndexValueSet->AddValue( 2 );
	pStreamIndexValueSet->AddValue( 3 );
	
	AddParameter( "OutputTopology", &m_OutputTopology, pOutputTopologies );
	AddParameter( "EmitCount", &m_nEmitCount, new CRangeValueSet< UINT >( 1, 12, 1 ) );
	AddParameter( "CutEvery", &m_nCutEvery, new CRangeValueSet< UINT >( 2, 5, 1 ) );
	AddParameter( "Stream", &m_stream, pStreamIndexValueSet );

	m_gsOutputComponentCount = 4;
}
﻿<?xml version='1.0' encoding='utf-8' standalone='yes'?>
<Content xmlns="http://microsoft.com/schemas/Windows/Kits/Manifest">
  <ContentGroup
      BuildArchitecture="each"
      Destination="$(_BuildArch)"
      >
    <FeatureReference Name="Device.Graphics"/>
    <BinaryFile
        Destination="nttest\windowstest\graphics\d3d\conf"
        Name="AlphaCmp.exe"
        Source="$(OBJ_PATH)\$(O)"
        />
  </ContentGroup>
</Content>
﻿<?xml version='1.0' encoding='utf-8' standalone='yes'?>
<Content xmlns="http://microsoft.com/schemas/Windows/Kits/Manifest">
  <ContentGroup
      BuildArchitecture="each"
      Destination="$(_BuildArch)"
      >
    <FeatureReference Name="Device.Graphics"/>
    <BinaryFile
        Destination="nttest\windowstest\graphics\d3d\conf"
        Name="NonPowTwo.exe"
        Source="$(OBJ_PATH)\$(O)"
        />
  </ContentGroup>
</Content>
#include "Test_Include.fx"

int Test_Count = 449;

string TestInfo
<
	string TestType = "VS";
>
= "vs_2_sw_inst_max";

VS_CRITERIA VS_001_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_2_sw : max source reg combination v0, v1 is allowed";
string VS_001 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"max r0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_2_sw : max source reg combination v0, r0 is allowed";
string VS_002 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_2_sw : max source reg combination v0, c0 is allowed";
string VS_003 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"max r0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_2_sw : max source reg combination r0, v0 is allowed";
string VS_004 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_2_sw : max source reg combination r0, r1 is allowed";
string VS_005 = 
	"vs_2_sw "
	"mov r0, c0 "
	"mov r1, c0 "
	"max r0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_2_sw : max source reg combination r0, c0 is allowed";
string VS_006 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"max r0, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_2_sw : max source reg combination c0, v0 is allowed";
string VS_007 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"max r0, c0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_008_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_2_sw : max source reg combination c0, r0 is allowed";
string VS_008 = 
	"vs_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"max r0, c0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_009_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_2_sw : max source reg combination c0, c1 is allowed";
string VS_009 = 
	"vs_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"max r0, c0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_010_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_2_sw : max source reg combination -r0, -v0 is allowed";
string VS_010 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, -r0, -v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_011_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_2_sw : max source reg combination -r0, v0 is allowed";
string VS_011 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, -r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_012_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_2_sw : max source reg combination r0, -v0 is allowed";
string VS_012 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, -v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_013_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_2_sw : max source reg combination r0, v0 is allowed";
string VS_013 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_014_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_2_sw : max source reg combination r0.x, v0 is allowed";
string VS_014 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.x, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_015_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_2_sw : max source reg combination r0.y, v0 is allowed";
string VS_015 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.y, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_016_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_2_sw : max source reg combination r0.z, v0 is allowed";
string VS_016 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.z, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_017_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_2_sw : max source reg combination r0.w, v0 is allowed";
string VS_017 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.w, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_018_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_2_sw : max source reg combination r0.yzxw, v0 is allowed";
string VS_018 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.yzxw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_019_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_2_sw : max source reg combination r0.zxyw, v0 is allowed";
string VS_019 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.zxyw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_2_sw : max source reg combination r0.wzyx, v0 is allowed";
string VS_020 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.wzyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_2_sw : max source reg combination r0.wyxz, v0 is allowed";
string VS_021 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.wyxz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_2_sw : max source reg combination r0.xzyw, v0 is allowed";
string VS_022 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.xzyw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_2_sw : max source reg combination r0.xywz, v0 is allowed";
string VS_023 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.xywz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_2_sw : max source reg combination r0.zyx, v0 is allowed";
string VS_024 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.zyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_2_sw : max source reg combination r0.xzy, v0 is allowed";
string VS_025 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.xzy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_2_sw : max source reg combination r0.ywx, v0 is allowed";
string VS_026 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.ywx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_2_sw : max source reg combination r0.yx, v0 is allowed";
string VS_027 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.yx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_2_sw : max source reg combination r0.wz, v0 is allowed";
string VS_028 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.wz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_2_sw : max source reg combination r0.zy, v0 is allowed";
string VS_029 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0.zy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_2_sw : max source reg combination r0, v0.x is allowed";
string VS_030 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_2_sw : max source reg combination r0, v0.y is allowed";
string VS_031 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.y "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_2_sw : max source reg combination r0, v0.z is allowed";
string VS_032 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.z "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_2_sw : max source reg combination r0, v0.w is allowed";
string VS_033 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.w "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_2_sw : max source reg combination r0, v0.yzxw is allowed";
string VS_034 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.yzxw "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_2_sw : max source reg combination r0, v0.zxyw is allowed";
string VS_035 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.zxyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_2_sw : max source reg combination r0, v0.wzyx is allowed";
string VS_036 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.wzyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_2_sw : max source reg combination r0, v0.wyxz is allowed";
string VS_037 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.wyxz "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_2_sw : max source reg combination r0, v0.xzyw is allowed";
string VS_038 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.xzyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_2_sw : max source reg combination r0, v0.xywz is allowed";
string VS_039 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.xywz "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_2_sw : max source reg combination r0, v0.zyx is allowed";
string VS_040 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.zyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_2_sw : max source reg combination r0, v0.xzy is allowed";
string VS_041 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"mov r0, c0 "
	"max r0, r0, v0.xzy "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_2_sw : max source reg combination r0, v0.ywx is allowed";
string VS_042 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"max r0, r0, v0.ywx "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_2_sw : max source reg combination r0, v0.yx is allowed";
string VS_043 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"max r0, r0, v0.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_2_sw : max source reg combination r0, v0.wz is allowed";
string VS_044 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"max r0, r0, v0.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_2_sw : max source reg combination r0, v0.zy is allowed";
string VS_045 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"max r0, r0, v0.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_2_sw : max source reg combination a0, a0 is NOT allowed";
string VS_046 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_2_sw : max source reg combination a0, b0 is NOT allowed";
string VS_047 = 
	"vs_2_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"max r0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_2_sw : max source reg combination a0, i0 is NOT allowed";
string VS_048 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"max r0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_2_sw : max source reg combination a0, aL is NOT allowed";
string VS_049 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"max r0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_2_sw : max source reg combination a0, p0 is NOT allowed";
string VS_050 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_2_sw : max source reg combination a0, oPos is NOT allowed";
string VS_051 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_2_sw : max source reg combination a0, oFog is NOT allowed";
string VS_052 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_2_sw : max source reg combination a0, oPts is NOT allowed";
string VS_053 = 
	"vs_2_sw "
	"dcl ts "
	"mova a0.x, c0.x "
	"max r0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_2_sw : max source reg combination a0, oD0 is NOT allowed";
string VS_054 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_055_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_2_sw : max source reg combination a0, oT0 is NOT allowed";
string VS_055 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_056_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_2_sw : max source reg combination b0, a0 is NOT allowed";
string VS_056 = 
	"vs_2_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"max r0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_057_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_2_sw : max source reg combination b0, b0 is NOT allowed";
string VS_057 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_058_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_2_sw : max source reg combination b0, i0 is NOT allowed";
string VS_058 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"max r0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_059_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_2_sw : max source reg combination b0, aL is NOT allowed";
string VS_059 = 
	"vs_2_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"max r0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_060_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_2_sw : max source reg combination b0, p0 is NOT allowed";
string VS_060 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_061_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_2_sw : max source reg combination b0, oPos is NOT allowed";
string VS_061 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_062_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_2_sw : max source reg combination b0, oFog is NOT allowed";
string VS_062 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_063_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_2_sw : max source reg combination b0, oPts is NOT allowed";
string VS_063 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl ts "
	"max r0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_064_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_2_sw : max source reg combination b0, oD0 is NOT allowed";
string VS_064 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_065_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_2_sw : max source reg combination b0, oT0 is NOT allowed";
string VS_065 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_066_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_2_sw : max source reg combination i0, a0 is NOT allowed";
string VS_066 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"max r0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_067_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_2_sw : max source reg combination i0, b0 is NOT allowed";
string VS_067 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"max r0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_068_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_2_sw : max source reg combination i0, i0 is NOT allowed";
string VS_068 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_069_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_2_sw : max source reg combination i0, aL is NOT allowed";
string VS_069 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"max r0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_070_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_2_sw : max source reg combination i0, p0 is NOT allowed";
string VS_070 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_071_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_2_sw : max source reg combination i0, oPos is NOT allowed";
string VS_071 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_072_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_2_sw : max source reg combination i0, oFog is NOT allowed";
string VS_072 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_073_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_2_sw : max source reg combination i0, oPts is NOT allowed";
string VS_073 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"max r0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_074_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_2_sw : max source reg combination i0, oD0 is NOT allowed";
string VS_074 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_075_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_2_sw : max source reg combination i0, oT0 is NOT allowed";
string VS_075 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_076_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_2_sw : max source reg combination aL, a0 is NOT allowed";
string VS_076 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"max r0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_077_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_2_sw : max source reg combination aL, b0 is NOT allowed";
string VS_077 = 
	"vs_2_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"max r0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_078_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_2_sw : max source reg combination aL, i0 is NOT allowed";
string VS_078 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"max r0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_079_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_2_sw : max source reg combination aL, aL is NOT allowed";
string VS_079 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_080_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_2_sw : max source reg combination aL, p0 is NOT allowed";
string VS_080 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_081_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_2_sw : max source reg combination aL, oPos is NOT allowed";
string VS_081 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_082_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_2_sw : max source reg combination aL, oFog is NOT allowed";
string VS_082 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_083_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_2_sw : max source reg combination aL, oPts is NOT allowed";
string VS_083 = 
	"vs_2_sw "
	"dcl ts "
	"mova aL.x, c0.x "
	"max r0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_084_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_2_sw : max source reg combination aL, oD0 is NOT allowed";
string VS_084 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_085_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_2_sw : max source reg combination aL, oT0 is NOT allowed";
string VS_085 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_086_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_2_sw : max source reg combination p0, a0 is NOT allowed";
string VS_086 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_087_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_2_sw : max source reg combination p0, b0 is NOT allowed";
string VS_087 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_088_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_2_sw : max source reg combination p0, i0 is NOT allowed";
string VS_088 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_089_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_2_sw : max source reg combination p0, aL is NOT allowed";
string VS_089 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_090_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_2_sw : max source reg combination p0, p0 is NOT allowed";
string VS_090 = 
	"vs_2_sw "
	"max r0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_091_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_2_sw : max source reg combination p0, oPos is NOT allowed";
string VS_091 = 
	"vs_2_sw "
	"max r0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_092_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_2_sw : max source reg combination p0, oFog is NOT allowed";
string VS_092 = 
	"vs_2_sw "
	"max r0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_093_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_2_sw : max source reg combination p0, oPts is NOT allowed";
string VS_093 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_094_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_2_sw : max source reg combination p0, oD0 is NOT allowed";
string VS_094 = 
	"vs_2_sw "
	"max r0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_095_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_2_sw : max source reg combination p0, oT0 is NOT allowed";
string VS_095 = 
	"vs_2_sw "
	"max r0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_096_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_2_sw : max source reg combination oPos, a0 is NOT allowed";
string VS_096 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_097_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_2_sw : max source reg combination oPos, b0 is NOT allowed";
string VS_097 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_098_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_2_sw : max source reg combination oPos, i0 is NOT allowed";
string VS_098 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_099_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_2_sw : max source reg combination oPos, aL is NOT allowed";
string VS_099 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_100_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_2_sw : max source reg combination oPos, p0 is NOT allowed";
string VS_100 = 
	"vs_2_sw "
	"max r0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_101_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_2_sw : max source reg combination oPos, oPos is NOT allowed";
string VS_101 = 
	"vs_2_sw "
	"max r0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_102_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_2_sw : max source reg combination oPos, oFog is NOT allowed";
string VS_102 = 
	"vs_2_sw "
	"max r0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_103_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_2_sw : max source reg combination oPos, oPts is NOT allowed";
string VS_103 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_104_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_2_sw : max source reg combination oPos, oD0 is NOT allowed";
string VS_104 = 
	"vs_2_sw "
	"max r0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_105_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_2_sw : max source reg combination oPos, oT0 is NOT allowed";
string VS_105 = 
	"vs_2_sw "
	"max r0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_106_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_2_sw : max source reg combination oFog, a0 is NOT allowed";
string VS_106 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_107_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_2_sw : max source reg combination oFog, b0 is NOT allowed";
string VS_107 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_108_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_2_sw : max source reg combination oFog, i0 is NOT allowed";
string VS_108 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_109_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_2_sw : max source reg combination oFog, aL is NOT allowed";
string VS_109 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_110_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_2_sw : max source reg combination oFog, p0 is NOT allowed";
string VS_110 = 
	"vs_2_sw "
	"max r0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_111_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_2_sw : max source reg combination oFog, oPos is NOT allowed";
string VS_111 = 
	"vs_2_sw "
	"max r0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_112_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_2_sw : max source reg combination oFog, oFog is NOT allowed";
string VS_112 = 
	"vs_2_sw "
	"max r0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_113_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_2_sw : max source reg combination oFog, oPts is NOT allowed";
string VS_113 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_114_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_2_sw : max source reg combination oFog, oD0 is NOT allowed";
string VS_114 = 
	"vs_2_sw "
	"max r0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_115_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_2_sw : max source reg combination oFog, oT0 is NOT allowed";
string VS_115 = 
	"vs_2_sw "
	"max r0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_116_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_2_sw : max source reg combination oPts, a0 is NOT allowed";
string VS_116 = 
	"vs_2_sw "
	"dcl ts "
	"mova a0.x, c0.x "
	"max r0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_117_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_2_sw : max source reg combination oPts, b0 is NOT allowed";
string VS_117 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl ts "
	"max r0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_118_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_2_sw : max source reg combination oPts, i0 is NOT allowed";
string VS_118 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"max r0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_119_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_2_sw : max source reg combination oPts, aL is NOT allowed";
string VS_119 = 
	"vs_2_sw "
	"dcl ts "
	"mova aL.x, c0.x "
	"max r0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_120_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_2_sw : max source reg combination oPts, p0 is NOT allowed";
string VS_120 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_121_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_2_sw : max source reg combination oPts, oPos is NOT allowed";
string VS_121 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_122_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_2_sw : max source reg combination oPts, oFog is NOT allowed";
string VS_122 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_123_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_2_sw : max source reg combination oPts, oPts is NOT allowed";
string VS_123 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_124_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_2_sw : max source reg combination oPts, oD0 is NOT allowed";
string VS_124 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_125_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_2_sw : max source reg combination oPts, oT0 is NOT allowed";
string VS_125 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_126_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_2_sw : max source reg combination oD0, a0 is NOT allowed";
string VS_126 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_127_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_2_sw : max source reg combination oD0, b0 is NOT allowed";
string VS_127 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_128_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_2_sw : max source reg combination oD0, i0 is NOT allowed";
string VS_128 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_129_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_2_sw : max source reg combination oD0, aL is NOT allowed";
string VS_129 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_130_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_2_sw : max source reg combination oD0, p0 is NOT allowed";
string VS_130 = 
	"vs_2_sw "
	"max r0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_131_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_2_sw : max source reg combination oD0, oPos is NOT allowed";
string VS_131 = 
	"vs_2_sw "
	"max r0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_132_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_2_sw : max source reg combination oD0, oFog is NOT allowed";
string VS_132 = 
	"vs_2_sw "
	"max r0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_133_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_2_sw : max source reg combination oD0, oPts is NOT allowed";
string VS_133 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_134_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_2_sw : max source reg combination oD0, oD0 is NOT allowed";
string VS_134 = 
	"vs_2_sw "
	"max r0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_135_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_2_sw : max source reg combination oD0, oT0 is NOT allowed";
string VS_135 = 
	"vs_2_sw "
	"max r0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_136_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_2_sw : max source reg combination oT0, a0 is NOT allowed";
string VS_136 = 
	"vs_2_sw "
	"mova a0.x, c0.x "
	"max r0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_137_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_2_sw : max source reg combination oT0, b0 is NOT allowed";
string VS_137 = 
	"vs_2_sw "
	"defb b0, true "
	"max r0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_138_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_2_sw : max source reg combination oT0, i0 is NOT allowed";
string VS_138 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"max r0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_139_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_2_sw : max source reg combination oT0, aL is NOT allowed";
string VS_139 = 
	"vs_2_sw "
	"mova aL.x, c0.x "
	"max r0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_140_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_2_sw : max source reg combination oT0, p0 is NOT allowed";
string VS_140 = 
	"vs_2_sw "
	"max r0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_141_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_2_sw : max source reg combination oT0, oPos is NOT allowed";
string VS_141 = 
	"vs_2_sw "
	"max r0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_142_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_2_sw : max source reg combination oT0, oFog is NOT allowed";
string VS_142 = 
	"vs_2_sw "
	"max r0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_143_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_2_sw : max source reg combination oT0, oPts is NOT allowed";
string VS_143 = 
	"vs_2_sw "
	"dcl ts "
	"max r0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_144_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_2_sw : max source reg combination oT0, oD0 is NOT allowed";
string VS_144 = 
	"vs_2_sw "
	"max r0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_145_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_2_sw : max source reg combination oT0, oT0 is NOT allowed";
string VS_145 = 
	"vs_2_sw "
	"max r0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_146_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_2_sw : max dest r# is allowed";
string VS_146 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_147_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_2_sw : max dest oPos is allowed";
string VS_147 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max oPos, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_148_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_2_sw : max dest oD# is allowed";
string VS_148 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max oD0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_149_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_2_sw : max dest oT# is allowed";
string VS_149 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max oT0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_150_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_2_sw : max dest v# is NOT allowed";
string VS_150 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max v0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_151_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_2_sw : max dest c# is NOT allowed";
string VS_151 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max c1, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_152_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_2_sw : max dest a0 is NOT allowed";
string VS_152 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max a0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_153_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_2_sw : max dest b# is NOT allowed";
string VS_153 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max b0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_154_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_2_sw : max dest i# is NOT allowed";
string VS_154 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max i0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_155_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_2_sw : max dest aL is NOT allowed";
string VS_155 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max aL, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_156_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_2_sw : max dest p0 is NOT allowed";
string VS_156 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max p0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_157_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_2_sw : max write mask .x is allowed";
string VS_157 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.x, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_158_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_2_sw : max write mask .y is allowed";
string VS_158 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.y, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_159_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_2_sw : max write mask .z is allowed";
string VS_159 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.z, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_160_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_2_sw : max write mask .w is allowed";
string VS_160 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.w, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_161_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_2_sw : max write mask .xy is allowed";
string VS_161 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.xy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_162_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_2_sw : max write mask .yz is allowed";
string VS_162 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.yz, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_163_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_2_sw : max write mask .zw is allowed";
string VS_163 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.zw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_164_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_2_sw : max write mask .xz is allowed";
string VS_164 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.xz, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_165_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_2_sw : max write mask .xw is allowed";
string VS_165 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.xw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_166_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_2_sw : max write mask .yw is allowed";
string VS_166 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.yw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_167_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_2_sw : max write mask .xyz is allowed";
string VS_167 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.xyz, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_168_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_2_sw : max write mask .yzw is allowed";
string VS_168 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.yzw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_169_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_2_sw : max write mask .xzw is allowed";
string VS_169 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.xzw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_170_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_2_sw : max write mask .xyzw is allowed";
string VS_170 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.xyzw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_171_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_2_sw : max write mask .yx is NOT allowed";
string VS_171 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.yx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_172_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_2_sw : max write mask .zx is NOT allowed";
string VS_172 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.zx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_173_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_2_sw : max write mask .zy is NOT allowed";
string VS_173 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.zy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_174_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_2_sw : max write mask .wx is NOT allowed";
string VS_174 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.wx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_175_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_2_sw : max write mask .wz is NOT allowed";
string VS_175 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.wz, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_176_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_2_sw : max write mask .wy is NOT allowed";
string VS_176 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.wy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_177_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_2_sw : max write mask .zyx is NOT allowed";
string VS_177 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.zyx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_178_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_2_sw : max write mask .wzy is NOT allowed";
string VS_178 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.wzy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_179_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_2_sw : max write mask .wzx is NOT allowed";
string VS_179 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.wzx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_180_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_2_sw : max write mask .wyx is NOT allowed";
string VS_180 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.wyx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_181_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_2_sw : max write mask .yxw is NOT allowed";
string VS_181 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.yxw, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_182_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_2_sw : max write mask .wzyx is NOT allowed";
string VS_182 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.wzyx, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_183_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_2_sw : max write mask .zxwy is NOT allowed";
string VS_183 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"max r0.zxwy, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_184_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_2_sw : max is allowed in a 1 level if b0 block";
string VS_184 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 "
	"	max r0, v0, v1 "
	"endif ";

VS_CRITERIA VS_185_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_2_sw : max is allowed in a 8 level if b0 block";
string VS_185 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								max r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_186_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_2_sw : max is allowed in a 16 level if b0 block";
string VS_186 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																max r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_187_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_2_sw : max is allowed in a 24 level if b0 block";
string VS_187 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								max r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_188_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_2_sw : max is NOT allowed in a 25 level if b0 block";
string VS_188 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								if b0 "
	"																									max r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_189_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_189_Desc = "vs_2_sw : max is allowed in a 1 level if b0 nop else block";
string VS_189 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 nop else "
	"	max r0, v0, v1 "
	"endif ";

VS_CRITERIA VS_190_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_190_Desc = "vs_2_sw : max is allowed in a 8 level if b0 nop else block";
string VS_190 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								max r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_191_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_191_Desc = "vs_2_sw : max is allowed in a 16 level if b0 nop else block";
string VS_191 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																max r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_192_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_192_Desc = "vs_2_sw : max is allowed in a 24 level if b0 nop else block";
string VS_192 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								max r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_193_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_193_Desc = "vs_2_sw : max is NOT allowed in a 25 level if b0 nop else block";
string VS_193 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									max r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_194_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_194_Desc = "vs_2_sw : max is allowed in a 1 level loop aL, i0 block";
string VS_194 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"loop aL, i0 "
	"	max r0, v0, v1 "
	"endloop ";

VS_CRITERIA VS_195_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_195_Desc = "vs_2_sw : max is allowed in a 2 level loop aL, i0 block";
string VS_195 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_196_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_196_Desc = "vs_2_sw : max is allowed in a 3 level loop aL, i0 block";
string VS_196 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			max r0, v0, v1 "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_197_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_197_Desc = "vs_2_sw : max is allowed in a 4 level loop aL, i0 block";
string VS_197 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				max r0, v0, v1 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_198_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_198_Desc = "vs_2_sw : max is NOT allowed in a 5 level loop aL, i0 block";
string VS_198 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					max r0, v0, v1 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_199_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_199_Desc = "vs_2_sw : max is allowed in a 1 level rep i0 block";
string VS_199 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"rep i0 "
	"	max r0, v0, v1 "
	"endrep ";

VS_CRITERIA VS_200_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_200_Desc = "vs_2_sw : max is allowed in a 2 level rep i0 block";
string VS_200 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"rep i0 "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_201_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_201_Desc = "vs_2_sw : max is allowed in a 3 level rep i0 block";
string VS_201 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			max r0, v0, v1 "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_202_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_202_Desc = "vs_2_sw : max is allowed in a 4 level rep i0 block";
string VS_202 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				max r0, v0, v1 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_203_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_203_Desc = "vs_2_sw : max is NOT allowed in a 5 level rep i0 block";
string VS_203 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					max r0, v0, v1 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_204_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_204_Desc = "vs_2_sw : max is allowed in a 1 level if_lt c0.x, c0.y block";
string VS_204 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if_lt c0.x, c0.y "
	"	max r0, v0, v1 "
	"endif ";

VS_CRITERIA VS_205_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_205_Desc = "vs_2_sw : max is allowed in a 8 level if_lt c0.x, c0.y block";
string VS_205 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								max r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_206_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_206_Desc = "vs_2_sw : max is allowed in a 16 level if_lt c0.x, c0.y block";
string VS_206 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																max r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_207_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_207_Desc = "vs_2_sw : max is allowed in a 24 level if_lt c0.x, c0.y block";
string VS_207 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																if_lt c0.x, c0.y "
	"																	if_lt c0.x, c0.y "
	"																		if_lt c0.x, c0.y "
	"																			if_lt c0.x, c0.y "
	"																				if_lt c0.x, c0.y "
	"																					if_lt c0.x, c0.y "
	"																						if_lt c0.x, c0.y "
	"																							if_lt c0.x, c0.y "
	"																								max r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_208_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_208_Desc = "vs_2_sw : max is NOT allowed in a 25 level if_lt c0.x, c0.y block";
string VS_208 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																if_lt c0.x, c0.y "
	"																	if_lt c0.x, c0.y "
	"																		if_lt c0.x, c0.y "
	"																			if_lt c0.x, c0.y "
	"																				if_lt c0.x, c0.y "
	"																					if_lt c0.x, c0.y "
	"																						if_lt c0.x, c0.y "
	"																							if_lt c0.x, c0.y "
	"																								if_lt c0.x, c0.y "
	"																									max r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_209_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_209_Desc = "vs_2_sw : max is allowed in a 1 level if_lt c0.x, c0.y nop else block";
string VS_209 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if_lt c0.x, c0.y nop else "
	"	max r0, v0, v1 "
	"endif ";

VS_CRITERIA VS_210_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_210_Desc = "vs_2_sw : max is allowed in a 8 level if_lt c0.x, c0.y nop else block";
string VS_210 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								max r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_211_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_211_Desc = "vs_2_sw : max is allowed in a 16 level if_lt c0.x, c0.y nop else block";
string VS_211 = 
	"vs_2_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																max r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_212_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_212_Desc = "vs_2_sw : max is allowed in a 24 level if_lt c0.x, c0.y nop else block";
string VS_212 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																if_lt c0.x, c0.y nop else "
	"																	if_lt c0.x, c0.y nop else "
	"																		if_lt c0.x, c0.y nop else "
	"																			if_lt c0.x, c0.y nop else "
	"																				if_lt c0.x, c0.y nop else "
	"																					if_lt c0.x, c0.y nop else "
	"																						if_lt c0.x, c0.y nop else "
	"																							if_lt c0.x, c0.y nop else "
	"																								max r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_213_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_213_Desc = "vs_2_sw : max is NOT allowed in a 25 level if_lt c0.x, c0.y nop else block";
string VS_213 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																if_lt c0.x, c0.y nop else "
	"																	if_lt c0.x, c0.y nop else "
	"																		if_lt c0.x, c0.y nop else "
	"																			if_lt c0.x, c0.y nop else "
	"																				if_lt c0.x, c0.y nop else "
	"																					if_lt c0.x, c0.y nop else "
	"																						if_lt c0.x, c0.y nop else "
	"																							if_lt c0.x, c0.y nop else "
	"																								if_lt c0.x, c0.y nop else "
	"																									max r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_214_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_214_Desc = "vs_2_sw : max is allowed in a 1 level if p0.x block";
string VS_214 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	max r0, v0, v1 "
	"endif ";

VS_CRITERIA VS_215_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_215_Desc = "vs_2_sw : max is allowed in a 8 level if p0.x block";
string VS_215 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								max r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_216_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_216_Desc = "vs_2_sw : max is allowed in a 16 level if p0.x block";
string VS_216 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																max r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_217_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_217_Desc = "vs_2_sw : max is allowed in a 24 level if p0.x block";
string VS_217 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								max r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_218_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_218_Desc = "vs_2_sw : max is NOT allowed in a 25 level if p0.x block";
string VS_218 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								if p0.x "
	"																									max r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_219_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_219_Desc = "vs_2_sw : max is allowed in a 1 level if p0.x nop else block";
string VS_219 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	max r0, v0, v1 "
	"endif ";

VS_CRITERIA VS_220_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_220_Desc = "vs_2_sw : max is allowed in a 8 level if p0.x nop else block";
string VS_220 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								max r0, v0, v1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_221_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_221_Desc = "vs_2_sw : max is allowed in a 16 level if p0.x nop else block";
string VS_221 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																max r0, v0, v1 "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_222_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_222_Desc = "vs_2_sw : max is allowed in a 24 level if p0.x nop else block";
string VS_222 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								max r0, v0, v1 "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_223_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_223_Desc = "vs_2_sw : max is NOT allowed in a 25 level if p0.x nop else block";
string VS_223 = 
	"vs_2_sw "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								if p0.x nop else "
	"																									max r0, v0, v1 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_224_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_224_Desc = "vs_2_sw : max is allowed in a 1 level loop aL, i0 break block";
string VS_224 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break "
	"	max r0, v0, v1 "
	"endloop ";

VS_CRITERIA VS_225_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_225_Desc = "vs_2_sw : max is NOT allowed in a 8 level loop aL, i0 break block";
string VS_225 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								max r0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_226_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_226_Desc = "vs_2_sw : max is NOT allowed in a 16 level loop aL, i0 break block";
string VS_226 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																max r0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_227_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_227_Desc = "vs_2_sw : max is NOT allowed in a 24 level loop aL, i0 break block";
string VS_227 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								max r0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_228_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_228_Desc = "vs_2_sw : max is NOT allowed in a 25 level loop aL, i0 break block";
string VS_228 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								loop aL, i0 break "
	"																									max r0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_229_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_229_Desc = "vs_2_sw : max is allowed in a 1 level rep i0 break block";
string VS_229 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break "
	"	max r0, v0, v1 "
	"endrep ";

VS_CRITERIA VS_230_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_230_Desc = "vs_2_sw : max is NOT allowed in a 8 level rep i0 break block";
string VS_230 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								max r0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_231_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_231_Desc = "vs_2_sw : max is NOT allowed in a 16 level rep i0 break block";
string VS_231 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																max r0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_232_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_232_Desc = "vs_2_sw : max is NOT allowed in a 24 level rep i0 break block";
string VS_232 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								max r0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_233_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_233_Desc = "vs_2_sw : max is NOT allowed in a 25 level rep i0 break block";
string VS_233 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								rep i0 break "
	"																									max r0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_234_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_234_Desc = "vs_2_sw : max is allowed in a 1 level loop aL, i0 breakp p0.x block";
string VS_234 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 breakp p0.x "
	"	max r0, v0, v1 "
	"endloop ";

VS_CRITERIA VS_235_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_235_Desc = "vs_2_sw : max is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string VS_235 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								max r0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_236_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_236_Desc = "vs_2_sw : max is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string VS_236 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																max r0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_237_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_237_Desc = "vs_2_sw : max is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string VS_237 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								max r0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_238_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_238_Desc = "vs_2_sw : max is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string VS_238 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								loop aL, i0 breakp p0.x "
	"																									max r0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_239_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_239_Desc = "vs_2_sw : max is allowed in a 1 level rep i0 breakp p0.x block";
string VS_239 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 breakp p0.x "
	"	max r0, v0, v1 "
	"endrep ";

VS_CRITERIA VS_240_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_240_Desc = "vs_2_sw : max is NOT allowed in a 8 level rep i0 breakp p0.x block";
string VS_240 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								max r0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_241_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_241_Desc = "vs_2_sw : max is NOT allowed in a 16 level rep i0 breakp p0.x block";
string VS_241 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																max r0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_242_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_242_Desc = "vs_2_sw : max is NOT allowed in a 24 level rep i0 breakp p0.x block";
string VS_242 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								max r0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_243_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_243_Desc = "vs_2_sw : max is NOT allowed in a 25 level rep i0 breakp p0.x block";
string VS_243 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								rep i0 breakp p0.x "
	"																									max r0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_244_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_244_Desc = "vs_2_sw : max is allowed in a 1 level loop aL, i0 break_le c0.x, c0.y block";
string VS_244 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	max r0, v0, v1 "
	"endloop ";

VS_CRITERIA VS_245_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_245_Desc = "vs_2_sw : max is NOT allowed in a 8 level loop aL, i0 break_le c0.x, c0.y block";
string VS_245 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								max r0, v0, v1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_246_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_246_Desc = "vs_2_sw : max is NOT allowed in a 16 level loop aL, i0 break_le c0.x, c0.y block";
string VS_246 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								loop aL, i0 break_le c0.x, c0.y "
	"									loop aL, i0 break_le c0.x, c0.y "
	"										loop aL, i0 break_le c0.x, c0.y "
	"											loop aL, i0 break_le c0.x, c0.y "
	"												loop aL, i0 break_le c0.x, c0.y "
	"													loop aL, i0 break_le c0.x, c0.y "
	"														loop aL, i0 break_le c0.x, c0.y "
	"															loop aL, i0 break_le c0.x, c0.y "
	"																max r0, v0, v1 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_247_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_247_Desc = "vs_2_sw : max is NOT allowed in a 24 level loop aL, i0 break_le c0.x, c0.y block";
string VS_247 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								loop aL, i0 break_le c0.x, c0.y "
	"									loop aL, i0 break_le c0.x, c0.y "
	"										loop aL, i0 break_le c0.x, c0.y "
	"											loop aL, i0 break_le c0.x, c0.y "
	"												loop aL, i0 break_le c0.x, c0.y "
	"													loop aL, i0 break_le c0.x, c0.y "
	"														loop aL, i0 break_le c0.x, c0.y "
	"															loop aL, i0 break_le c0.x, c0.y "
	"																loop aL, i0 break_le c0.x, c0.y "
	"																	loop aL, i0 break_le c0.x, c0.y "
	"																		loop aL, i0 break_le c0.x, c0.y "
	"																			loop aL, i0 break_le c0.x, c0.y "
	"																				loop aL, i0 break_le c0.x, c0.y "
	"																					loop aL, i0 break_le c0.x, c0.y "
	"																						loop aL, i0 break_le c0.x, c0.y "
	"																							loop aL, i0 break_le c0.x, c0.y "
	"																								max r0, v0, v1 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_248_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_248_Desc = "vs_2_sw : max is NOT allowed in a 25 level loop aL, i0 break_le c0.x, c0.y block";
string VS_248 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								loop aL, i0 break_le c0.x, c0.y "
	"									loop aL, i0 break_le c0.x, c0.y "
	"										loop aL, i0 break_le c0.x, c0.y "
	"											loop aL, i0 break_le c0.x, c0.y "
	"												loop aL, i0 break_le c0.x, c0.y "
	"													loop aL, i0 break_le c0.x, c0.y "
	"														loop aL, i0 break_le c0.x, c0.y "
	"															loop aL, i0 break_le c0.x, c0.y "
	"																loop aL, i0 break_le c0.x, c0.y "
	"																	loop aL, i0 break_le c0.x, c0.y "
	"																		loop aL, i0 break_le c0.x, c0.y "
	"																			loop aL, i0 break_le c0.x, c0.y "
	"																				loop aL, i0 break_le c0.x, c0.y "
	"																					loop aL, i0 break_le c0.x, c0.y "
	"																						loop aL, i0 break_le c0.x, c0.y "
	"																							loop aL, i0 break_le c0.x, c0.y "
	"																								loop aL, i0 break_le c0.x, c0.y "
	"																									max r0, v0, v1 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_249_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_249_Desc = "vs_2_sw : max is allowed in a 1 level rep i0 break_le c0.x, c0.y block";
string VS_249 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break_le c0.x, c0.y "
	"	max r0, v0, v1 "
	"endrep ";

VS_CRITERIA VS_250_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_250_Desc = "vs_2_sw : max is NOT allowed in a 8 level rep i0 break_le c0.x, c0.y block";
string VS_250 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								max r0, v0, v1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_251_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_251_Desc = "vs_2_sw : max is NOT allowed in a 16 level rep i0 break_le c0.x, c0.y block";
string VS_251 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								rep i0 break_le c0.x, c0.y "
	"									rep i0 break_le c0.x, c0.y "
	"										rep i0 break_le c0.x, c0.y "
	"											rep i0 break_le c0.x, c0.y "
	"												rep i0 break_le c0.x, c0.y "
	"													rep i0 break_le c0.x, c0.y "
	"														rep i0 break_le c0.x, c0.y "
	"															rep i0 break_le c0.x, c0.y "
	"																max r0, v0, v1 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_252_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_252_Desc = "vs_2_sw : max is NOT allowed in a 24 level rep i0 break_le c0.x, c0.y block";
string VS_252 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								rep i0 break_le c0.x, c0.y "
	"									rep i0 break_le c0.x, c0.y "
	"										rep i0 break_le c0.x, c0.y "
	"											rep i0 break_le c0.x, c0.y "
	"												rep i0 break_le c0.x, c0.y "
	"													rep i0 break_le c0.x, c0.y "
	"														rep i0 break_le c0.x, c0.y "
	"															rep i0 break_le c0.x, c0.y "
	"																rep i0 break_le c0.x, c0.y "
	"																	rep i0 break_le c0.x, c0.y "
	"																		rep i0 break_le c0.x, c0.y "
	"																			rep i0 break_le c0.x, c0.y "
	"																				rep i0 break_le c0.x, c0.y "
	"																					rep i0 break_le c0.x, c0.y "
	"																						rep i0 break_le c0.x, c0.y "
	"																							rep i0 break_le c0.x, c0.y "
	"																								max r0, v0, v1 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_253_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_253_Desc = "vs_2_sw : max is NOT allowed in a 25 level rep i0 break_le c0.x, c0.y block";
string VS_253 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								rep i0 break_le c0.x, c0.y "
	"									rep i0 break_le c0.x, c0.y "
	"										rep i0 break_le c0.x, c0.y "
	"											rep i0 break_le c0.x, c0.y "
	"												rep i0 break_le c0.x, c0.y "
	"													rep i0 break_le c0.x, c0.y "
	"														rep i0 break_le c0.x, c0.y "
	"															rep i0 break_le c0.x, c0.y "
	"																rep i0 break_le c0.x, c0.y "
	"																	rep i0 break_le c0.x, c0.y "
	"																		rep i0 break_le c0.x, c0.y "
	"																			rep i0 break_le c0.x, c0.y "
	"																				rep i0 break_le c0.x, c0.y "
	"																					rep i0 break_le c0.x, c0.y "
	"																						rep i0 break_le c0.x, c0.y "
	"																							rep i0 break_le c0.x, c0.y "
	"																								rep i0 break_le c0.x, c0.y "
	"																									max r0, v0, v1 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_254_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_254_Desc = "vs_2_sw : max is allowed in a if b0 and if b0 block";
string VS_254 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	if b0 "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_255_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_255_Desc = "vs_2_sw : max is allowed in a if b0 and if b0 nop else block";
string VS_255 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	if b0 nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_256_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_256_Desc = "vs_2_sw : max is allowed in a if b0 and loop aL, i0 block";
string VS_256 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_257_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_257_Desc = "vs_2_sw : max is allowed in a if b0 and rep i0 block";
string VS_257 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_258_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_258_Desc = "vs_2_sw : max is allowed in a if b0 and if_lt c0.x, c0.y block";
string VS_258 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	if_lt c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_259_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_259_Desc = "vs_2_sw : max is allowed in a if b0 and if_lt c0.x, c0.y nop else block";
string VS_259 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	if_lt c0.x, c0.y nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_260_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_260_Desc = "vs_2_sw : max is allowed in a if b0 and if p0.x block";
string VS_260 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	if p0.x "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_261_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_261_Desc = "vs_2_sw : max is allowed in a if b0 and if p0.x nop else block";
string VS_261 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	if p0.x nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_262_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_262_Desc = "vs_2_sw : max is allowed in a if b0 and loop aL, i0 break block";
string VS_262 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	loop aL, i0 break "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_263_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_263_Desc = "vs_2_sw : max is allowed in a if b0 and rep i0 break block";
string VS_263 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_texcoord v1 "
	"if b0 "
	"	rep i0 break "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_264_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_264_Desc = "vs_2_sw : max is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string VS_264 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_265_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_265_Desc = "vs_2_sw : max is allowed in a if b0 and rep i0 breakp p0.x block";
string VS_265 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_266_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_266_Desc = "vs_2_sw : max is allowed in a if b0 and loop aL, i0 break_le c0.x, c0.y block";
string VS_266 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_267_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_267_Desc = "vs_2_sw : max is allowed in a if b0 and rep i0 break_le c0.x, c0.y block";
string VS_267 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 "
	"	rep i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_268_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_268_Desc = "vs_2_sw : max is allowed in a if b0 nop else and if b0 block";
string VS_268 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	if b0 "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_269_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_269_Desc = "vs_2_sw : max is allowed in a if b0 nop else and if b0 nop else block";
string VS_269 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	if b0 nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_270_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_270_Desc = "vs_2_sw : max is allowed in a if b0 nop else and loop aL, i0 block";
string VS_270 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_271_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_271_Desc = "vs_2_sw : max is allowed in a if b0 nop else and rep i0 block";
string VS_271 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_272_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_272_Desc = "vs_2_sw : max is allowed in a if b0 nop else and if_lt c0.x, c0.y block";
string VS_272 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	if_lt c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_273_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_273_Desc = "vs_2_sw : max is allowed in a if b0 nop else and if_lt c0.x, c0.y nop else block";
string VS_273 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	if_lt c0.x, c0.y nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_274_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_274_Desc = "vs_2_sw : max is allowed in a if b0 nop else and if p0.x block";
string VS_274 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	if p0.x "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_275_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_275_Desc = "vs_2_sw : max is allowed in a if b0 nop else and if p0.x nop else block";
string VS_275 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_276_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_276_Desc = "vs_2_sw : max is allowed in a if b0 nop else and loop aL, i0 break block";
string VS_276 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_277_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_277_Desc = "vs_2_sw : max is allowed in a if b0 nop else and rep i0 break block";
string VS_277 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	rep i0 break "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_278_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_278_Desc = "vs_2_sw : max is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string VS_278 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_279_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_279_Desc = "vs_2_sw : max is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string VS_279 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_280_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_280_Desc = "vs_2_sw : max is allowed in a if b0 nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_280 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_281_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_281_Desc = "vs_2_sw : max is allowed in a if b0 nop else and rep i0 break_le c0.x, c0.y block";
string VS_281 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_color v1 "
	"if b0 nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_282_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_282_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and if b0 block";
string VS_282 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	if b0 "
	"		max r0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_283_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_283_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and if b0 nop else block";
string VS_283 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_284_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_284_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and loop aL, i0 block";
string VS_284 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_285_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_285_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and rep i0 block";
string VS_285 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_286_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_286_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and if_lt c0.x, c0.y block";
string VS_286 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_287_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_287_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and if_lt c0.x, c0.y nop else block";
string VS_287 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_288_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_288_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and if p0.x block";
string VS_288 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	if p0.x "
	"		max r0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_289_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_289_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and if p0.x nop else block";
string VS_289 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_290_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_290_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and loop aL, i0 break block";
string VS_290 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		max r0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_291_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_291_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and rep i0 break block";
string VS_291 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	rep i0 break "
	"		max r0, v0, v1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_292_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_292_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string VS_292 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_293_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_293_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string VS_293 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_294_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_294_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and loop aL, i0 break_le c0.x, c0.y block";
string VS_294 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_295_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_295_Desc = "vs_2_sw : max is allowed in a loop aL, i0 and rep i0 break_le c0.x, c0.y block";
string VS_295 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"loop aL, i0 "
	"	rep i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_296_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_296_Desc = "vs_2_sw : max is allowed in a rep i0 and if b0 block";
string VS_296 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"rep i0 "
	"	if b0 "
	"		max r0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_297_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_297_Desc = "vs_2_sw : max is allowed in a rep i0 and if b0 nop else block";
string VS_297 = 
	"vs_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"rep i0 "
	"	if b0 nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_298_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_298_Desc = "vs_2_sw : max is allowed in a rep i0 and loop aL, i0 block";
string VS_298 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"rep i0 "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_299_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_299_Desc = "vs_2_sw : max is allowed in a rep i0 and rep i0 block";
string VS_299 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_300_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_300_Desc = "vs_2_sw : max is allowed in a rep i0 and if_lt c0.x, c0.y block";
string VS_300 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if_lt c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_301_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_301_Desc = "vs_2_sw : max is allowed in a rep i0 and if_lt c0.x, c0.y nop else block";
string VS_301 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if_lt c0.x, c0.y nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_302_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_302_Desc = "vs_2_sw : max is allowed in a rep i0 and if p0.x block";
string VS_302 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if p0.x "
	"		max r0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_303_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_303_Desc = "vs_2_sw : max is allowed in a rep i0 and if p0.x nop else block";
string VS_303 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	if p0.x nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_304_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_304_Desc = "vs_2_sw : max is allowed in a rep i0 and loop aL, i0 break block";
string VS_304 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	loop aL, i0 break "
	"		max r0, v0, v1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_305_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_305_Desc = "vs_2_sw : max is allowed in a rep i0 and rep i0 break block";
string VS_305 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 break "
	"		max r0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_306_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_306_Desc = "vs_2_sw : max is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string VS_306 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_307_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_307_Desc = "vs_2_sw : max is allowed in a rep i0 and rep i0 breakp p0.x block";
string VS_307 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_308_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_308_Desc = "vs_2_sw : max is allowed in a rep i0 and loop aL, i0 break_le c0.x, c0.y block";
string VS_308 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_309_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_309_Desc = "vs_2_sw : max is allowed in a rep i0 and rep i0 break_le c0.x, c0.y block";
string VS_309 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"rep i0 "
	"	rep i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_310_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_310_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and if b0 block";
string VS_310 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	if b0 "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_311_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_311_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and if b0 nop else block";
string VS_311 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	if b0 nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_312_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_312_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and loop aL, i0 block";
string VS_312 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_313_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_313_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and rep i0 block";
string VS_313 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_314_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_314_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y block";
string VS_314 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_315_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_315_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string VS_315 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_316_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_316_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and if p0.x block";
string VS_316 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	if p0.x "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_317_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_317_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and if p0.x nop else block";
string VS_317 = 
	"vs_2_sw "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_318_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_318_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and loop aL, i0 break block";
string VS_318 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_319_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_319_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and rep i0 break block";
string VS_319 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	rep i0 break "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_320_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_320_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_320 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_321_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_321_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and rep i0 breakp p0.x block";
string VS_321 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_322_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_322_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string VS_322 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_323_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_323_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string VS_323 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_324_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_324_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and if b0 block";
string VS_324 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y nop else "
	"	if b0 "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_325_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_325_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and if b0 nop else block";
string VS_325 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y nop else "
	"	if b0 nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_326_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_326_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 block";
string VS_326 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_327_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_327_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and rep i0 block";
string VS_327 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_328_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_328_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and if_lt c0.x, c0.y block";
string VS_328 = 
	"vs_2_sw "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_329_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_329_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and if_lt c0.x, c0.y nop else block";
string VS_329 = 
	"vs_2_sw "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_330_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_330_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and if p0.x block";
string VS_330 = 
	"vs_2_sw "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	if p0.x "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_331_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_331_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and if p0.x nop else block";
string VS_331 = 
	"vs_2_sw "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_332_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_332_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 break block";
string VS_332 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_333_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_333_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and rep i0 break block";
string VS_333 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 break "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_334_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_334_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string VS_334 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_335_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_335_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and rep i0 breakp p0.x block";
string VS_335 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_336_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_336_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_336 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_337_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_337_Desc = "vs_2_sw : max is allowed in a if_lt c0.x, c0.y nop else and rep i0 break_le c0.x, c0.y block";
string VS_337 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_338_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_338_Desc = "vs_2_sw : max is allowed in a if p0.x and if b0 block";
string VS_338 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if b0 "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_339_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_339_Desc = "vs_2_sw : max is allowed in a if p0.x and if b0 nop else block";
string VS_339 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if b0 nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_340_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_340_Desc = "vs_2_sw : max is allowed in a if p0.x and loop aL, i0 block";
string VS_340 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_341_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_341_Desc = "vs_2_sw : max is allowed in a if p0.x and rep i0 block";
string VS_341 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_342_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_342_Desc = "vs_2_sw : max is allowed in a if p0.x and if_lt c0.x, c0.y block";
string VS_342 = 
	"vs_2_sw "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if_lt c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_343_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_343_Desc = "vs_2_sw : max is allowed in a if p0.x and if_lt c0.x, c0.y nop else block";
string VS_343 = 
	"vs_2_sw "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_344_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_344_Desc = "vs_2_sw : max is allowed in a if p0.x and if p0.x block";
string VS_344 = 
	"vs_2_sw "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if p0.x "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_345_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_345_Desc = "vs_2_sw : max is allowed in a if p0.x and if p0.x nop else block";
string VS_345 = 
	"vs_2_sw "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	if p0.x nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_346_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_346_Desc = "vs_2_sw : max is allowed in a if p0.x and loop aL, i0 break block";
string VS_346 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	loop aL, i0 break "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_347_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_347_Desc = "vs_2_sw : max is allowed in a if p0.x and rep i0 break block";
string VS_347 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	rep i0 break "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_348_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_348_Desc = "vs_2_sw : max is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string VS_348 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_349_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_349_Desc = "vs_2_sw : max is allowed in a if p0.x and rep i0 breakp p0.x block";
string VS_349 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_texcoord v1 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_350_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_350_Desc = "vs_2_sw : max is allowed in a if p0.x and loop aL, i0 break_le c0.x, c0.y block";
string VS_350 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_351_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_351_Desc = "vs_2_sw : max is allowed in a if p0.x and rep i0 break_le c0.x, c0.y block";
string VS_351 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_352_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_352_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and if b0 block";
string VS_352 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	if b0 "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_353_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_353_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and if b0 nop else block";
string VS_353 = 
	"vs_2_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_354_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_354_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and loop aL, i0 block";
string VS_354 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_355_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_355_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and rep i0 block";
string VS_355 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	rep i0 "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_356_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_356_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and if_lt c0.x, c0.y block";
string VS_356 = 
	"vs_2_sw "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	if_lt c0.x, c0.y "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_357_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_357_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and if_lt c0.x, c0.y nop else block";
string VS_357 = 
	"vs_2_sw "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	if_lt c0.x, c0.y nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_358_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_358_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and if p0.x block";
string VS_358 = 
	"vs_2_sw "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	if p0.x "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_359_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_359_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and if p0.x nop else block";
string VS_359 = 
	"vs_2_sw "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		max r0, v0, v1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_360_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_360_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and loop aL, i0 break block";
string VS_360 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_361_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_361_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and rep i0 break block";
string VS_361 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	rep i0 break "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_362_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_362_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string VS_362 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_363_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_363_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string VS_363 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		max r0, v0, v1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_364_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_364_Desc = "vs_2_sw : max is allowed in a if p0.x nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_364 = 
	"vs_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "