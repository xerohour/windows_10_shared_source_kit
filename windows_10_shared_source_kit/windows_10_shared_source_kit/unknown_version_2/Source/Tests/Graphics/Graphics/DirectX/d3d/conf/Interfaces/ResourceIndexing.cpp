
// Includes
#include "ResourceIndexing.h"
#include "IncludeResource.h" // CIncludeResource

// Defines
#define MEMBER_OFFSET( member, instance ) \
	(int) ( ptrdiff_t( &instance.member ) - ptrdiff_t( &instance ) )

// Names value sets
BEGIN_NAMED_VALUES( D3D_RESOURCE_TYPE )
	  
	  // Textures/Buffers
	  NAMED_VALUE( _T( "Buffer" ), D3D_RESOURCE_TYPE_BUFFER )
	  NAMED_VALUE( _T( "Texture1D" ), D3D_RESOURCE_TYPE_TEXTURE1D )
	  NAMED_VALUE( _T( "Texture1D_Array" ), D3D_RESOURCE_TYPE_TEXTURE1D_ARRAY )
	  NAMED_VALUE( _T( "Texture2D" ), D3D_RESOURCE_TYPE_TEXTURE2D )
	  NAMED_VALUE( _T( "Texture2D_Array" ), D3D_RESOURCE_TYPE_TEXTURE2D_ARRAY )
	  NAMED_VALUE( _T( "Texture2DMS" ), D3D_RESOURCE_TYPE_TEXTURE2D_MS )
	  NAMED_VALUE( _T( "Texture2DMS_Array" ), D3D_RESOURCE_TYPE_TEXTURE2D_MS_ARRAY )
	  NAMED_VALUE( _T( "TextureCube" ), D3D_RESOURCE_TYPE_TEXTURECUBE )
	  NAMED_VALUE( _T( "Texture3D" ), D3D_RESOURCE_TYPE_TEXTURE3D )

	  // Constant buffers
	  NAMED_VALUE( _T( "ConstantBuffer" ), D3D_RESOURCE_TYPE_CONSTANT_BUFFER )
	  
	  // Samplers
	  NAMED_VALUE( _T( "Sampler" ), D3D_RESOURCE_TYPE_SAMPLER )

END_NAMED_VALUES( D3D_RESOURCE_TYPE )



//------------------------------------------------------------------------------------------------
// CResourceIndexing
//------------------------------------------------------------------------------------------------

CResourceIndexing::~CResourceIndexing(void)
{
}

/////

TEST_RESULT CResourceIndexing::Setup()
{ 
	// Local varaibles
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Call the parent's setup
	tr = CInterfacesTest::Setup();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CResourceIndexing::Setup() - CInterfacesTest::Setup() didn't pass.") );
		goto setupDone;
	}

	// Create the shaders
	tr = CreateShaders();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CResourceIndexing::SetupTestCase() - CreateShaders() failed.") );
		goto setupDone;
	}

	// Create and set the constant buffers
	tr = CreateConstantBuffers();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CResourceIndexing::Setup() - CreateConstantBuffers() failed.") );
		goto setupDone;
	}

    bool IsLegacyWarp10Dll;
    if (FAILED(hr = GetFramework()->IsLegacyD3D10WarpDll(GetDXGIAdapter(), GetDevice(), IsLegacyWarp10Dll)))
    {
        LogError(__FILEW__, __LINE__, L"Call to CheckFeatureSupport() failed", hr, ERRORCODE_TYPE_HRESULT, WindowsTest::Graphics::Direct3D::gFCDeviceCheckFeatureSupport);
        tr = RESULT_FAIL;
        goto setupDone;
    }

    if (IsLegacyWarp10Dll)
    {
        // The legacy warp implementation in d3d10warp is very slow.  Increase the timeout
        // for that implementation while keeping the requirement the same for the newer d3d12warp
        // implementation.
        m_statisticsQueryTimeout = max(20000, GetFramework()->GetQueryTimeout());
    }

setupDone:
	return tr;
}

/////

TEST_RESULT CResourceIndexing::SetupTestCase()
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	m_bConstantBufferForInstanceSlotInitialized = false;

	// Setup the interfaces
	tr = SetupInterfaces();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CResourceIndexing::Setup() - SetupInterfaces() failed.") );
		goto setupDone;
	}

	// Setup the resources (call class's virtual function)
	tr = SetupResources();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CResourceIndexing::SetupTestCase() - SetupResources() failed.") );
		goto setupDone;
	}

	// Setup Streamout buffers
	tr = SetupOutputBuffers();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CResourceIndexing::SetupTestCase() - SetupOutputBuffers() failed.") );
		goto setupDone;
	}

	// Setup Streamout queries
	tr = SetupQueries();

	if( RESULT_PASS != tr )
	{
		WriteToLog( _T("CResourceIndexing::SetupTestCase() - SetupQueries() failed.") );
		goto setupDone;
	}

	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );

setupDone:
	return tr;
}

/////

TEST_RESULT CResourceIndexing::ExecuteTestCase()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	UINT* pConstData = NULL;

	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	D3D11_QUERY_DATA_SO_STATISTICS soStats;
	UINT queryDataSize = 0;
	BOOL soOverflow = FALSE;

	const UINT vsInvocationsExpected = D3D_SHADER_STAGE_COMPUTE == m_shaderStage ? 0 : 1;
	const UINT hsInvocationsExpected = D3D_SHADER_STAGE_COMPUTE == m_shaderStage ? 0 : 1;
	const UINT dsInvocationsExpected = D3D_SHADER_STAGE_COMPUTE == m_shaderStage ? 0 : 1;
	const UINT gsInvocationsExpected = D3D_SHADER_STAGE_COMPUTE == m_shaderStage ? 0 : 1;
	const UINT psInvocationsExpected = D3D_SHADER_STAGE_PIXEL   == m_shaderStage ? 1 : 0;
	const UINT csInvocationsExpected = D3D_SHADER_STAGE_COMPUTE == m_shaderStage ? 1 : 0;

	const BOOL verifyQueries = GetFramework()->GetExecutionPriorityLevel() > 1;

	UINT numPrimitivesExpected = 0;
	BOOL overflowExpected = FALSE;

	switch( m_shaderStage )
	{
		// The Dispatch() API does not invoke stream-output
	case D3D_SHADER_STAGE_COMPUTE:
		numPrimitivesExpected = 0;
		overflowExpected = FALSE;
		break;

		// There is no stream-out buffer bound for the PS test
		// but the GS is still created with stream out, so overflow is expected
	case D3D_SHADER_STAGE_PIXEL:
		numPrimitivesExpected = 0;
		overflowExpected = TRUE; // No SO buffer is bound
		break;

	default:
		numPrimitivesExpected = 1;
		overflowExpected = FALSE;  
	}

	//
	// Setup pipeline
	//
	if( m_pUAV )
	{
		const UINT clearColor[4] = {0,0,0,0};
		GetEffectiveContext()->ClearUnorderedAccessViewUint( m_pUAV, clearColor );
	}

	// Set the shaders with instance information
	if( D3D_SHADER_STAGE_VERTEX == m_shaderStage )
	{
		GetEffectiveContext()->VSSetShader( m_pVertexShader[0], m_pInstances, m_numClassInstances );
	}
	else
	{
		GetEffectiveContext()->VSSetShader( m_pVertexShader[1], NULL, 0 );
	}

	if( D3D_SHADER_STAGE_HULL == m_shaderStage )
	{
		GetEffectiveContext()->HSSetShader( m_pHullShader[0], m_pInstances, m_numClassInstances);
	}
	else
	{
		GetEffectiveContext()->HSSetShader( m_pHullShader[1], NULL, 0);
	}

	if( D3D_SHADER_STAGE_DOMAIN == m_shaderStage )
	{
		GetEffectiveContext()->DSSetShader( m_pDomainShader[0], m_pInstances, m_numClassInstances);
	}
	else
	{
		GetEffectiveContext()->DSSetShader( m_pDomainShader[1], NULL, 0);
	}

	if( D3D_SHADER_STAGE_GEOMETRY == m_shaderStage )
	{
		GetEffectiveContext()->GSSetShader( m_pGeometryShader[0], m_pInstances, m_numClassInstances);
	}
	else
	{
		GetEffectiveContext()->GSSetShader( m_pGeometryShader[1], NULL, 0);
	}

	if( D3D_SHADER_STAGE_PIXEL == m_shaderStage )
	{
		GetEffectiveContext()->PSSetShader( m_pPixelShader, m_pInstances, m_numClassInstances);
	}
	else
	{
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0);
	}

	if( D3D_SHADER_STAGE_COMPUTE == m_shaderStage )
	{
		GetEffectiveContext()->CSSetShader( m_pComputeShader, m_pInstances, m_numClassInstances);
	}
	else
	{
		GetEffectiveContext()->CSSetShader( NULL, NULL, 0);
	}

	// Execute the command list thus far (includes setup)
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::ExecuteTestCase() - ExecuteEffectiveContext() failed. HR = %s"), D3DHResultToString(hr).c_str() );
		goto executeDone;
	}

	if(!m_bConstantBufferForInstanceSlotInitialized)
	{
		// Set constant buffer data (which instance slot index to use in the shader)
		// Note: ALWAYS write to constant buffer #0, component offset 0. I.e the first component in the buffer.
		if( FAILED( hr = GetImmediateContext()->Map( m_pConstantBufferForInstanceSlot, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) )
		{
			tr = RESULT_FAIL;
			WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - Map() for m_pConstantBufferForInstanceSlot failed. HR = %s" ), D3DHResultToString( hr ).c_str() );
			goto executeDone;
		}

		pConstData = (UINT*) mappedResource.pData;
		*pConstData = m_successInstanceSlot;

		// Unmap
		GetImmediateContext()->Unmap( m_pConstantBufferForInstanceSlot, 0 );

		m_bConstantBufferForInstanceSlotInitialized = true;
	}

	//
	// Draw
	//

	// Start queries
	GetEffectiveContext()->Begin( m_pSOStatsQuery );
	GetEffectiveContext()->Begin( m_pSOOverflowPredicate );
	GetEffectiveContext()->Begin( m_pPipelineStatsQuery );

	// Draw
	if( D3D_SHADER_STAGE_COMPUTE == m_shaderStage )
	{
		GetEffectiveContext()->Dispatch(1, 1, 1);
	}
	else
	{
		GetEffectiveContext()->Draw( 3, 0 );
	}

	// End queries
	GetEffectiveContext()->End( m_pSOStatsQuery );
	GetEffectiveContext()->End( m_pSOOverflowPredicate );
	GetEffectiveContext()->End( m_pPipelineStatsQuery );

	// Execute the command list
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::ExecuteTestCase() - ExecuteEffectiveContext() failed. HR = %s"), D3DHResultToString(hr).c_str() );
		goto executeDone;
	}

	//
	// Check pipeline statistics
	//

	queryDataSize = m_pPipelineStatsQuery->GetDataSize();
	if( verifyQueries && sizeof( D3D11_QUERY_DATA_PIPELINE_STATISTICS ) != queryDataSize )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - GetDataSize() for m_pPipelineStatsQuery was incorrect." ) );
		WriteToLog( _T( " Actual data size (in bytes) = %d."), queryDataSize );
		WriteToLog( _T( " Expected data size (in bytes) = %d."), sizeof( D3D11_QUERY_DATA_PIPELINE_STATISTICS ));
		goto executeDone;
	}
	
	// Get the pipeline stats data
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pPipelineStatsQuery, &pipelineStats, queryDataSize, 0, m_statisticsQueryTimeout);
	if (hr != S_OK)
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - GetData() for m_pPipelineStatsQuery failed or timed out. HR = %s" ), D3DHResultToString(hr).c_str() );
		goto executeDone;
	}
	
	// Check number of invocations for each stage in the pipeline
	const struct
	{
		const char*  Name;
		UINT         Actual;
		UINT         Expected;
	} PipelineStateTestCase[] = 
	{
		{ "VS", (UINT)pipelineStats.VSInvocations, vsInvocationsExpected },
		{ "HS", (UINT)pipelineStats.HSInvocations, hsInvocationsExpected },
		{ "DS", (UINT)pipelineStats.DSInvocations, dsInvocationsExpected },
		{ "GS", (UINT)pipelineStats.GSInvocations, gsInvocationsExpected },
		{ "PS", (UINT)pipelineStats.PSInvocations, psInvocationsExpected },
		{ "CS", (UINT)pipelineStats.CSInvocations, csInvocationsExpected },
	};

	for( UINT i = 0; i < ARRAY_SIZE(PipelineStateTestCase); i++ )
	{
		// The comparison is < rather than ==
		// to allow helper invocations
		if( verifyQueries && PipelineStateTestCase[i].Actual < PipelineStateTestCase[i].Expected )
		{   
			tr = RESULT_FAIL;
			WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - Number of %s invocations does not match the expected value." ), PipelineStateTestCase[i].Name );
			WriteToLog( _T( " Actual number of invocations = %d" ), PipelineStateTestCase[i].Actual );
			WriteToLog( _T( " Expected number of invocations = %d" ), PipelineStateTestCase[i].Expected );
			goto executeDone;
		}
	}

	//
	// Check overflow status
	//

	queryDataSize = m_pSOOverflowPredicate->GetDataSize();
	if( sizeof( D3D11_QUERY_SO_OVERFLOW_PREDICATE ) != queryDataSize )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - GetDataSize() for m_pSOOverflowPredicate was incorrect." ) );
		WriteToLog( _T( " Actual data size (in bytes) = %d."), queryDataSize );
		WriteToLog( _T( " Expected data size (in bytes) = %d."), sizeof( D3D11_QUERY_SO_OVERFLOW_PREDICATE ));
		goto executeDone;
	}

	// Get the Streamout overflow data
	for( ; ( hr = GetImmediateContext()->GetData( m_pSOOverflowPredicate, &soOverflow, queryDataSize, 0 ) ) == S_FALSE; )
	{
		Sleep( 0 );
	}

	if( verifyQueries && soOverflow != overflowExpected )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - The Streamout overflow predicate was %s, when it was expected to be %s."), soOverflow ? "true" : "false", overflowExpected ? "true" : "false" );
		goto executeDone;
	}

	//
	// Check stream out statistics
	//

	queryDataSize = m_pSOStatsQuery->GetDataSize();
	if( verifyQueries && sizeof( D3D11_QUERY_DATA_SO_STATISTICS ) != queryDataSize )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - GetDataSize() for m_pSOStatsQuery was incorrect." ) );
		WriteToLog( _T( " Actual data size (in bytes) = %d."), queryDataSize );
		WriteToLog( _T( " Expected data size (in bytes) = %d."), sizeof( D3D11_QUERY_DATA_SO_STATISTICS ) );
		goto executeDone;
	}

	// Get the Streamout stats data
	for( ; ( hr = GetImmediateContext()->GetData( m_pSOStatsQuery, &soStats, queryDataSize, 0 ) ) == S_FALSE; )
	{
		Sleep( 0 );
	}

	if( verifyQueries && soStats.NumPrimitivesWritten != numPrimitivesExpected )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - The number of primitives written to streamout was incorrect." ) );
		WriteToLog( _T( " Actual number of primitives = %d." ), soStats.NumPrimitivesWritten );
		WriteToLog( _T( " Expected number of primitives = %d." ), numPrimitivesExpected );
		WriteToLog( _T( " Primitives storage needed (number that would of been written if there was room) = %d" ), soStats.PrimitivesStorageNeeded );
		goto executeDone;
	}

	//
	// Verify the results
	//

	// Copy the data from the output buffer
	GetEffectiveContext()->CopyResource( m_pOutputStagingBuffer, m_pOutputBuffer );

	// Execute the command list
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::ExecuteTestCase() - ExecuteEffectiveContext() failed. HR = %s"), D3DHResultToString(hr).c_str() );
		goto executeDone;
	}

	// Map the data
	if( FAILED( hr = GetImmediateContext()->Map( m_pOutputStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::ExecuteTestCase() - Map() for staging buffer (m_pOutputStagingBuffer) unexpectedly failed. HR = %s" ), D3DHResultToString( hr ).c_str() );
		goto executeDone;
	}

	// Verify result
	tr = VerifyResult( mappedResource );

	if( RESULT_PASS != tr )
		WriteToLog( _T("CResourceIndexing::ExecuteTestCase() - VerifyResult() failed.") );

	// Unmap
	GetImmediateContext()->Unmap( m_pOutputStagingBuffer, 0 );
	
executeDone:
	return tr;
}

/////

void CResourceIndexing::CleanupTestCase()
{
	for( UINT i = 0; i < m_numClassInstances; ++i )
		m_pInstances[i] = NULL;

	SAFE_RELEASE( m_pSuccessInstance );
	SAFE_RELEASE( m_pFailInstance );

	SAFE_RELEASE( m_pOutputBuffer );
	SAFE_RELEASE( m_pOutputStagingBuffer );
	SAFE_RELEASE( m_pUAV );
	SAFE_RELEASE( m_pRTV );

	SAFE_RELEASE( m_pSOStatsQuery );
	SAFE_RELEASE( m_pSOOverflowPredicate );
	SAFE_RELEASE( m_pPipelineStatsQuery );

	CInterfacesTest::CleanupTestCase();
}

/////

void CResourceIndexing::Cleanup()
{
	for(UINT i = 0; i < 2; i++)
	{
		SAFE_RELEASE( m_pVertexShader[i] );
		SAFE_RELEASE( m_pHullShader[i] );
		SAFE_RELEASE( m_pDomainShader[i] );
		SAFE_RELEASE( m_pGeometryShader[i] );
	}
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pComputeShader );

	SAFE_RELEASE( m_pConstantBufferForInstanceSlot );

	CInterfacesTest::Cleanup();
}

/////

TEST_RESULT CResourceIndexing::SetupInterfaces()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	const tstring successClassName = g_successClassNamePrefix + ToString( m_resourceType );
	const tstring failClassName = g_failClassNamePrefix + ToString( m_resourceType );


	//
	// Create class instances, one for success, another for failure
	//

	if( FAILED( hr = m_pClassLinkage->CreateClassInstance(	successClassName.c_str(), 
		GetConstantBufferOffset(true), GetConstantVectorOffset(true), GetTextureOffset(true), GetSamplerOffset(true), &m_pSuccessInstance ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::SetupInterfaces() - CreateClassInstance() for the success instance failed. HR = %s"), D3DHResultToString(hr).c_str());
		goto setupDone;
	}

	if( FAILED( hr = m_pClassLinkage->CreateClassInstance(	failClassName.c_str(), 
		GetConstantBufferOffset(false), GetConstantVectorOffset(false), GetTextureOffset(false), GetSamplerOffset(false), &m_pFailInstance ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::SetupInterfaces() - CreateClassInstance() for the fail instance failed. HR = %s"), D3DHResultToString(hr).c_str());
		goto setupDone;
	}

	//
	// Setup the array of instances to be passed to the shaders
	//

	for( UINT i = 0; i < m_numClassInstances; ++i )
	{
		// The only success instance is 
		if( i == m_successInstanceSlot )
			m_pInstances[i] = m_pSuccessInstance;
		else
			m_pInstances[i] = m_pFailInstance;
	}

setupDone:
	return tr;
}

/////

TEST_RESULT CResourceIndexing::CreateShaders()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	VSIN dummyVar;

	ID3D10Blob *pShaderBlob = NULL;
	ID3D10Blob *pErrorBlob = NULL;

	const struct
	{
		const char*      EntryPoint[2];
		const char*      Profile;
		D3D_SHADER_STAGE Stage;
	} Shaders[6] = 
	{
		{ {"VS_TEST", "VS_PASSTHROUGH"}, "vs_5_0", D3D_SHADER_STAGE_VERTEX },
		{ {"HS_TEST", "HS_PASSTHROUGH"}, "hs_5_0", D3D_SHADER_STAGE_HULL  },
		{ {"DS_TEST", "DS_PASSTHROUGH"}, "ds_5_0", D3D_SHADER_STAGE_DOMAIN  },
		{ {"GS_TEST", "GS_PASSTHROUGH"}, "gs_5_0", D3D_SHADER_STAGE_GEOMETRY },
		{ {"PS_TEST", NULL},             "ps_5_0", D3D_SHADER_STAGE_PIXEL },
		{ {"CS_TEST", NULL},             "cs_5_0", D3D_SHADER_STAGE_COMPUTE },
	};

	const D3D11_SO_DECLARATION_ENTRY SODeclarations[] =
	{
		{ 0, _T("RESOURCE_DATA"), 0, 0, 1, 0 },	// Output RESOURCE_DATA to stream 0, output slot 0. 1x 32-bit value.
	};

	const D3D11_INPUT_ELEMENT_DESC ieDesc[] =
	{
		{ "junkData", 0, DXGI_FORMAT_R32_UINT, 0, MEMBER_OFFSET( data, dummyVar ), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	

	for( UINT i = 0; i < _countof(Shaders); i++ )
	{
		for( UINT j = 0; j < 2; j++)
		{
			if( Shaders[i].EntryPoint[j] )
			{
				//
				// Compile the shader
				// #include directives in the *.fx files are handled by CIncludeResource
				//
				CIncludeResource includeResource;

				hr = D3DX11CompileFromResource(
				  NULL,
				  GetFXFileToCompile().c_str(),
				  NULL,
				  NULL,	// Macros
				  &includeResource,
				  Shaders[i].EntryPoint[j],
				  Shaders[i].Profile,
				  D3D10_SHADER_DEBUG,
				  0,
				  NULL,
				  &pShaderBlob,
				  &pErrorBlob,
				  NULL
				);

				if( FAILED( hr ) || pShaderBlob == NULL )
				{
					WriteToLog(_T("CResourceIndexing::CreateShaders() - Failed to compile shader. HR = %s, Errors = %s"),
						D3DHResultToString(hr).c_str(), pErrorBlob ? pErrorBlob->GetBufferPointer() : "");
					tr = RESULT_FAIL;
					goto setupDone;
				}

				switch( Shaders[i].Stage )
				{
				case D3D_SHADER_STAGE_VERTEX:
					hr = GetDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), (j == 0) ? m_pClassLinkage : NULL, &m_pVertexShader[j] );
					break;

				case D3D_SHADER_STAGE_HULL:
					hr = GetDevice()->CreateHullShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), (j == 0) ? m_pClassLinkage : NULL, &m_pHullShader[j] );
					break;

				case D3D_SHADER_STAGE_DOMAIN:
					hr = GetDevice()->CreateDomainShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), (j == 0) ? m_pClassLinkage : NULL, &m_pDomainShader[j] );
					break;

				case D3D_SHADER_STAGE_GEOMETRY:
					hr = GetDevice()->CreateGeometryShaderWithStreamOutput(
					  pShaderBlob->GetBufferPointer(),
					  pShaderBlob->GetBufferSize(),
					  SODeclarations,
					  1,
					  NULL,
					  0,
					  (j == 0) ? D3D11_SO_NO_RASTERIZED_STREAM : 0, // rasterize stream 0 in the passthrough version
					  (j == 0) ? m_pClassLinkage : NULL,
					  &m_pGeometryShader[j]
					);
					break;

				case D3D_SHADER_STAGE_PIXEL:
					assert( 0 == j );
					hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), m_pClassLinkage, &m_pPixelShader );
					break;

				case D3D_SHADER_STAGE_COMPUTE:
					assert( 0 == j );
					hr = GetDevice()->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), m_pClassLinkage, &m_pComputeShader );
					break;

				default:
					WriteToLog(_T("CResourceIndexing::CreateShaders() - Unknown shader stage"));
					tr = RESULT_FAIL;
					goto setupDone;
				}

				if( FAILED( hr ) )
				{
					WriteToLog(_T("CResourceIndexing::CreateShaders() - Failed to create shader. HR = %s"), D3DHResultToString(hr).c_str());
					tr = RESULT_FAIL;
					goto setupDone;
				}

				SAFE_RELEASE(pShaderBlob);
				SAFE_RELEASE(pErrorBlob);	
			}
		}
	}

setupDone:
	SAFE_RELEASE(pShaderBlob);
	SAFE_RELEASE(pErrorBlob);	

	return tr;
}

/////

TEST_RESULT CResourceIndexing::SetupOutputBuffers()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_BUFFER_DESC soBufferDesc;
	const UINT soOffsets[] = { 0 };
	ID3D11Texture2D* pRenderTarget = NULL;
	D3D11_TEXTURE2D_DESC texDesc;

	// PS and CS output with a UAV
	// all other stages output with stream out
	bool bindStreamOut = true;

	switch( m_shaderStage )
	{
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_COMPUTE:
		bindStreamOut = false;
		break;
	}

	ZeroMemory( &soBufferDesc, sizeof(soBufferDesc) );

	// Create the streamout buffer description
	soBufferDesc.ByteWidth = GetShaderOutputSize() * 3; // for 1 triangle
	soBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	soBufferDesc.BindFlags = bindStreamOut ? D3D11_BIND_STREAM_OUTPUT : D3D11_BIND_UNORDERED_ACCESS;
	soBufferDesc.MiscFlags = bindStreamOut ? 0 : D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	// Create the streamout buffer
	if( FAILED(hr = GetDevice()->CreateBuffer( &soBufferDesc, NULL, &m_pOutputBuffer )) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::SetupOutputBuffers() - Failed to create the streamout buffer. HR = %s"), D3DHResultToString(hr).c_str());
		goto setupDone;
	}

	// Create the staging streamout buffer
	soBufferDesc.Usage = D3D11_USAGE_STAGING;
	soBufferDesc.BindFlags = 0;
	soBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	soBufferDesc.MiscFlags = 0;

	if( FAILED(hr = GetDevice()->CreateBuffer( &soBufferDesc, NULL, &m_pOutputStagingBuffer )) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::SetupOutputBuffers() - Failed to create the streamout staging buffer. HR = %s"), D3DHResultToString(hr).c_str());
		goto setupDone;
	}

	// Create unordered access view (used for CS and PS writes)
	if( !bindStreamOut )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC( D3D11_UAV_DIMENSION_BUFFER, DXGI_FORMAT_R32_TYPELESS, 0, 1, 0, D3D11_BUFFER_UAV_FLAG_RAW );

		if( FAILED(hr = GetDevice()->CreateUnorderedAccessView( m_pOutputBuffer, &uavDesc, &m_pUAV )) )
		{
			tr = RESULT_FAIL;
			WriteToLog( _T("CResourceIndexing::SetupOutputBuffers() - Failed to create unordered access view. HR = %s"), D3DHResultToString(hr).c_str());
			goto setupDone;
		}
	}

	// Create render target and view (needed to ensure the PS executes)
	texDesc = CD3D11_TEXTURE2D_DESC( DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, 1, 1, D3D11_BIND_RENDER_TARGET );
	if( FAILED(hr = GetDevice()->CreateTexture2D( &texDesc, NULL, &pRenderTarget )) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::SetupOutputBuffers() - Failed to create render target. HR = %s"), D3DHResultToString(hr).c_str());
		goto setupDone;
	}

	if( FAILED(hr = GetDevice()->CreateRenderTargetView( pRenderTarget, NULL, &m_pRTV )) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::SetupOutputBuffers() - Failed to create render target view. HR = %s"), D3DHResultToString(hr).c_str());
		goto setupDone;
	}

	// Bind m_pOutputBuffer as a UAV or SO target
	UINT uavInitialCount = 0;

	{
		ID3D11RenderTargetView* pNULLRTV = NULL;
		ID3D11UnorderedAccessView* pNULLUAV = NULL;

		GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 1, &pNULLRTV, NULL, 1, 1, &pNULLUAV, &uavInitialCount );
		GetEffectiveContext()->CSSetUnorderedAccessViews( 1, 1, &pNULLUAV, &uavInitialCount );
		GetEffectiveContext()->SOSetTargets( 0, NULL, soOffsets );
	}

	switch( m_shaderStage )
	{
	case D3D_SHADER_STAGE_PIXEL:
		GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 1, &m_pRTV, NULL, 1, 1, &m_pUAV, &uavInitialCount );
		break;

	case D3D_SHADER_STAGE_COMPUTE:
		GetEffectiveContext()->CSSetUnorderedAccessViews( 1, 1, &m_pUAV, &uavInitialCount );
		break;

	default:
		GetEffectiveContext()->SOSetTargets( 1, &m_pOutputBuffer, soOffsets );
	}	

	// Setup the viewport to ensure that the PS executes
	{
		const D3D11_VIEWPORT vp = { 0, 0, 1, 1, 0, 1 };
		GetEffectiveContext()->RSSetViewports( 1, &vp );
	}

setupDone:
	SAFE_RELEASE( pRenderTarget );
	return tr;
}

/////

TEST_RESULT CResourceIndexing::CreateConstantBuffers()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_BUFFER_DESC constantBufferDesc;

	ZeroMemory( &constantBufferDesc, sizeof(constantBufferDesc) );

	// Create the buffer description
	constantBufferDesc.ByteWidth = 4 * sizeof(UINT);	// Will write the instance slot index to use in the test
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// Create the constant buffer
	if( FAILED( hr = GetDevice()->CreateBuffer( &constantBufferDesc, NULL, &m_pConstantBufferForInstanceSlot ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T("CResourceIndexing::CreateConstantBuffers() - Failed to create the constant buffer (m_pConstantBufferForInstanceSlot). HR = %s"), D3DHResultToString(hr).c_str());
		goto setupDone;
	}

	// Bind the constant buffers to the different pipeline stages
	GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pConstantBufferForInstanceSlot );
	GetEffectiveContext()->HSSetConstantBuffers( 0, 1, &m_pConstantBufferForInstanceSlot );
	GetEffectiveContext()->DSSetConstantBuffers( 0, 1, &m_pConstantBufferForInstanceSlot );
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pConstantBufferForInstanceSlot );
	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pConstantBufferForInstanceSlot );
	GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &m_pConstantBufferForInstanceSlot );

setupDone:
	return tr;
}

/////

TEST_RESULT CResourceIndexing::SetupQueries()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_QUERY_DESC queryDesc;

	//
	// Create the Streamout stats query
	//

	queryDesc.MiscFlags = 0;
	queryDesc.Query = D3D11_QUERY_SO_STATISTICS;

	if( FAILED( hr = GetDevice()->CreateQuery( &queryDesc, &m_pSOStatsQuery ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::SetupQueries() - CreateQuery() for m_pSOStatsQuery failed. HR = %s" ), D3DHResultToString( hr ).c_str() );
		goto setupDone;
	}

	//
	// Setup Streamout overflow query
	//

	queryDesc.Query = D3D11_QUERY_SO_OVERFLOW_PREDICATE;

	if( FAILED( hr = GetDevice()->CreateQuery( &queryDesc, &m_pSOOverflowPredicate ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::SetupQueries() - CreateQuery() for m_pSOOverflowPredicate failed. HR = %s" ), D3DHResultToString( hr ).c_str() );
		goto setupDone;
	}

	//
	// Setup Pipeline query
	//

	queryDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;

	if( FAILED( hr = GetDevice()->CreateQuery( &queryDesc, &m_pPipelineStatsQuery ) ) )
	{
		tr = RESULT_FAIL;
		WriteToLog( _T( "CResourceIndexing::SetupQueries() - CreateQuery() for m_pPipelineStatsQuery failed. HR = %s" ), D3DHResultToString( hr ).c_str() );
		goto setupDone;
	}

setupDone:
	return tr;
}
