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
