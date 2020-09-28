#include "Tessellation.h"

// The Test App
CTessellationConfApp  g_TestApp;


///////////////////////////////////////////////////////////////////////////////
//	Tessellation Conformance testing
///////////////////////////////////////////////////////////////////////////////

BEGIN_NAMED_VALUES( HullOutputType )
	NAMED_VALUE( _T( "ControlPoints" ), HullOutput_ControlPoints )
	NAMED_VALUE( _T( "PatchConstants" ), HullOutput_PatchConstants )
END_NAMED_VALUES( HullOutputType )

// Test data for tri patches
float     g_triangleListData[][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.5f, 0.0f, 1.0f},
	{0.5f, 0.1f, 0.0f, 1.0f},

	{0.35f, 0.55f, 0.0f, 1.0f},
	{0.85f, 0.55f, 0.0f, 1.0f},
	{0.7f,  0.3f, 0.0f, 1.0f},

	{0.55f, 0.1f, 0.0f, 1.0f},
	{0.9f,  0.5f, 0.0f, 1.0f},
	{0.7f,  0.1f, 0.0f, 1.0f}
};
extern const UINT  g_numTriangles = sizeof(g_triangleListData)/sizeof(float[12]);

float
f_getNaN()
{
	UINT32 dwNaNVal = 0x7f800001; // QNaN
	return *(float*)(&dwNaNVal);
}

bool
isNaN(float in)
{
	UINT32 dwInVal = *(UINT32*)(&in);
	if ((dwInVal & 0x7f800000) != 0x7f800000)
		return false;
	if ((dwInVal & 0x007fffff) == 0)
		return false; // +infinity
	return true;
}

bool
isINF(float in)
{
	UINT32 dwInVal = *(UINT32*)(&in);
	if ((dwInVal & 0x7fffffff) != 0x7f800000)
		return false;
	return true;
}

//---------------------------------------------------------------

TEST_RESULT
CTessellationConfTest::SetupBuffers( )
{
	TEST_RESULT         tRes = RESULT_PASS;
	HRESULT             hr = E_FAIL;

	// constant buffers
	D3D11_BUFFER_DESC   bufDesc;

	ZeroStructMem(bufDesc);
	bufDesc.ByteWidth = 512;
	bufDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufDesc.MiscFlags = 0;

	for (int devIdx = 0; devIdx < ARRAYSIZE(m_pConstBuffers); ++devIdx)
	{
		for (int k = 0; k < ARRAYSIZE(m_pConstBuffers[devIdx]); ++k)
		{
			if (FAILED( hr = m_pDeviceArray[devIdx]->CreateBuffer( &bufDesc, NULL, &m_pConstBuffers[devIdx][k]) ))
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T( "%s CreateBuffer(CB[%d][%d]) failed, hr=%s" ), LOG_HEADER, 
					devIdx, k, D3DHResultToString(hr).c_str() );
				goto Exit;
			}
		}
	}

	ON_EACH_CONTEXT->HSSetConstantBuffers( 0, 1, &m_pConstBuffers[CONTEXT_ID][0] );
	ON_EACH_CONTEXT->HSSetConstantBuffers( 1, 1, &m_pConstBuffers[CONTEXT_ID][1] );
	ON_EACH_CONTEXT->DSSetConstantBuffers( 0, 1, &m_pConstBuffers[CONTEXT_ID][0] );

	for ( int devIdx = 0; devIdx < ARRAYSIZE(m_pConstBuffers); ++devIdx )
	{
		D3D11_MAPPED_SUBRESOURCE    mappedRes = {0};

		int k = 0;  // #0 buffer

		if ( FAILED( hr = m_pContextArray[devIdx]->Map( m_pConstBuffers[devIdx][k], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes ) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s Map(SO[%d]) failed, hr=%s" ), LOG_HEADER, 
				devIdx, D3DHResultToString(hr).c_str() );
			goto Exit;
		}

		float      *tessVals = (float*)mappedRes.pData;
		int         n = 0;
		tstring     domainName = GetTestCaseParameter( _T("Domain") )->GetValueAsString();

		if ( domainName == "quad" )
		{
			for (int n = 0; n < 4; ++n)
			{
				tessVals[n * 4] = m_TessVals[n];
			}
		}
		else
		if ( domainName == "tri" )
		{
			for (int n = 0; n < 3; ++n)
			{
				tessVals[n * 4] = g_triangleListData[n][0];
				tessVals[n * 4 + 1] = g_triangleListData[n][1];
				tessVals[n * 4 + 2] = g_triangleListData[n][2];
				tessVals[n * 4 + 3] = g_triangleListData[n][3];
			}
		}
		else
		if ( domainName == "isoline" )
		{
			for (int n = 0; n < 4; ++n)
			{
				tessVals[n * 4] = m_TessVals[n];
			}
		}
  
		m_pContextArray[devIdx]->Unmap(m_pConstBuffers[devIdx][k], 0);


		k = 1;  // #1 buffer
		memset( &mappedRes, 0, sizeof(mappedRes) );

		if ( FAILED( hr = m_pContextArray[devIdx]->Map( m_pConstBuffers[devIdx][k], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes ) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s Map(SO[%d]) failed, hr=%s" ), LOG_HEADER, 
				devIdx, D3DHResultToString(hr).c_str() );
			goto Exit;
		}

		float      *tessFactors = (float*)mappedRes.pData;
  
		if ( domainName == "tri" )
		{
			for (int n = 0; n < 4; ++n)
			{
				if ( n != 3 )
				{
					tessFactors[n * 4] = float(m_uintTessFactors[n]);
				}
				else
				{
					tessFactors[n * 4] = float(m_uintTessFactors[n]) / 10.0f;   // Inside factor needs a fractional value 
				}
			}
		}

		m_pContextArray[devIdx]->Unmap(m_pConstBuffers[devIdx][k], 0);
	}

	for (int devIdx = 0; devIdx < ARRAYSIZE(m_pSOBuffer); ++devIdx)
	{
		// stream-out buffer
		ZeroStructMem(bufDesc);
		bufDesc.ByteWidth = 1024 * 1024 * 32;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.MiscFlags = 0;

		if (FAILED( hr = m_pDeviceArray[devIdx]->CreateBuffer( &bufDesc, NULL, &m_pSOBuffer[devIdx]) ))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s CreateBuffer(SO[%d]) failed, hr=%s" ), LOG_HEADER, 
				devIdx, D3DHResultToString(hr).c_str() );
			goto Exit;
		}
	}

	// create queries
	D3D11_QUERY_DESC    soQuery;

	soQuery.Query = D3D11_QUERY_SO_STATISTICS;
	soQuery.MiscFlags = 0;

	for (int devIdx = 0; devIdx < ARRAYSIZE(m_pSOQuery); ++devIdx)
	{
		if (FAILED( hr = m_pDeviceArray[devIdx]->CreateQuery(&soQuery, &m_pSOQuery[devIdx]) ))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s CreateQuery(SO[%d]) failed, hr=%s" ), LOG_HEADER, 
				devIdx, D3DHResultToString(hr).c_str() );
			goto Exit;
		}
	}

	soQuery.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	soQuery.MiscFlags = 0;

	for (int devIdx = 0; devIdx < ARRAYSIZE(m_pPipeStatsQuery); ++devIdx)
	{
		if (FAILED( hr = m_pDeviceArray[devIdx]->CreateQuery( &soQuery, &m_pPipeStatsQuery[devIdx]) ))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s CreateQuery(PipeStats[%d]) failed, hr=%s" ), LOG_HEADER, 
				devIdx, D3DHResultToString(hr).c_str() );
			goto Exit;
		}
	}

Exit:
	return tRes;
}


//---------------------------------------------------------------

static HRESULT  CompileShaderFromResourceHLSL( const SHADER_RECORD* record, const char* entryPoint, D3D_SHADER_STAGE shaderStage, ID3D10Blob** ppShaderBuf, ID3D10Blob** ppErrorBuf )
{
	return g_TestApp.CompileShaderFromResource( NULL, record->shaderCode.c_str(), entryPoint, shaderStage, D3D_FEATURE_LEVEL_11_0, g_TestApp.m_D3DOptions.FeatureLevel, 0, ppShaderBuf, ppErrorBuf );
}

//
static HRESULT  CompileShaderFromMemoryHLSL( const SHADER_RECORD* record, const char* entryPoint, D3D_SHADER_STAGE shaderStage, ID3D10Blob** ppShaderBuf, ID3D10Blob** ppErrorBuf )
{
	return g_TestApp.CompileShaderFromMemory( record->shaderCode.c_str(), record->shaderCode.size(), entryPoint, 
											  shaderStage, D3D_FEATURE_LEVEL_11_0, g_TestApp.m_D3DOptions.FeatureLevel, 0, 
											  ppShaderBuf, ppErrorBuf );
}

#include <testasm_api.h>

static HRESULT CompileShaderFromMemoryAsm( const SHADER_RECORD* record, D3D_SHADER_STAGE shaderStage, ID3D10Blob** ppShaderBuf, ID3D10Blob** ppErrorBuf )
{
	HRESULT hr;

	D3DTEST_ASSEMBLER_ARGUMENTS args;

	ZeroMemory(&args, sizeof(args));

	args.pSrcString = record->shaderCode.c_str();
	args.uSrcStringSize = record->shaderCode.length();
	args.uAssemblerFlags = D3DX11_ASSEMBLER_ADD_FEATURE_INFO_BLOB;
	args.qwFeatureInfoBlobContents = 
		g_TestApp.m_D3DOptions.Debug ? 0 : D3DX11_ASSEMBLER_RETAIL_SIGNED_ASSEMBLED_SHADER;

	hr = D3DTestAssembleShader( &args );

	*ppShaderBuf = args.pShaderBlob;
	*ppErrorBuf = args.pErrorBlob;

	return hr;
}

static HRESULT CompileShader( const SHADER_RECORD* record, const char* entryPoint, D3D_SHADER_STAGE shaderStage, ID3D10Blob **ppShaderBuf )
{
	HRESULT hr = S_OK;

	*ppShaderBuf = NULL;

	ID3D10Blob* pErrorBuf = NULL;

	switch( record->location )
	{
	case SHADER_SOURCE_RESOURCE_HLSL:
		hr = CompileShaderFromResourceHLSL( record, entryPoint, shaderStage, ppShaderBuf, &pErrorBuf );
		break;

	case SHADER_SOURCE_MEMORY_HLSL:
		hr = CompileShaderFromMemoryHLSL( record, entryPoint, shaderStage, ppShaderBuf, &pErrorBuf );
		break;

	case SHADER_SOURCE_MEMORY_ASM:
		hr = CompileShaderFromMemoryAsm( record, shaderStage, ppShaderBuf, &pErrorBuf );
		break;

	default:
		hr = E_INVALIDARG;
	}

	if( FAILED( hr ) )
	{
		g_TestApp.WriteToLogF( _T( "%s CompileShader() failed, hr=%s" ), LOG_HEADER, D3DHResultToString(hr).c_str() );
		g_TestApp.WriteToLogF( _T( "Shader code: \n%s" ), record->shaderCode.c_str() );

		if (pErrorBuf)
		{
			LPVOID  pBuf = pErrorBuf->GetBufferPointer();
			g_TestApp.WriteToLogF( _T( "Compiler errors: %s" ), (const char*)pBuf );
		}
	}

	SAFE_RELEASE( pErrorBuf );

	return hr;
}

//
TEST_RESULT
CTessellationConfTest::SetupShaders( D3D11_SO_DECLARATION_ENTRY* soDecls, UINT numSoDecls )
{
	TEST_RESULT     tRes = RESULT_PASS;
	HRESULT         hr = E_FAIL;
	ID3D10Blob     *pShaderBuf = NULL;


	SAFE_RELEASE_ARRAY( m_pVS );
	SAFE_RELEASE_ARRAY( m_pHS );
	SAFE_RELEASE_ARRAY( m_pDS );
	SAFE_RELEASE_ARRAY( m_pGS );

	if ( m_VertexShader.shaderCode.size() > 0 )
	{
		if ( FAILED( hr = CompileShader( &m_VertexShader, "vs_main", D3D_SHADER_STAGE_VERTEX, &pShaderBuf ) ) )
		{
			tRes = RESULT_FAIL;
			goto FailureCleanup;
		}

		for (int devIdx = 0; devIdx < ARRAYSIZE(m_pVS); ++devIdx)
		{
			if ( FAILED( hr = m_pDeviceArray[devIdx]->CreateVertexShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pVS[devIdx] ) ) )
			{
				WriteToLog( _T( "%s CreateVertexShader()[%d] failed. hr=%s" ), LOG_HEADER, 
					devIdx, D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto FailureCleanup;
			}
		}

		ON_EACH_CONTEXT->VSSetShader( m_pVS[CONTEXT_ID], NULL, 0 );
		ON_EACH_CONTEXT->IASetInputLayout( NULL );

		SAFE_RELEASE( pShaderBuf );
	}
	else if ( GetFramework()->GetCurrentTestCase() > 0 )
	{
		WriteToLog( _T( "%s No vertex shader named" ), LOG_HEADER );
		tRes = RESULT_FAIL;
		goto FailureCleanup;
	}

	if ( m_HullShader.shaderCode.size() > 0 )
	{
		if ( FAILED( hr = CompileShader( &m_HullShader, "hs_main", D3D_SHADER_STAGE_HULL, &pShaderBuf ) ) )
		{
			tRes = RESULT_FAIL;
			goto FailureCleanup;
		}

		for (int devIdx = 0; devIdx < ARRAYSIZE(m_pHS); ++devIdx)
		{
			if ( FAILED( hr = m_pDeviceArray[devIdx]->CreateHullShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pHS[devIdx] ) ) )
			{
				WriteToLog( _T( "%s CreateHullShader()[%d] failed. hr=%s" ), LOG_HEADER, 
					devIdx, D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto FailureCleanup;
			}
		}

		ON_EACH_CONTEXT->HSSetShader( m_pHS[CONTEXT_ID], NULL, 0 );

		SAFE_RELEASE( pShaderBuf );
	}
	else if ( GetFramework()->GetCurrentTestCase() > 0 )
	{
		WriteToLog( _T( "%s No hull shader named" ), LOG_HEADER );
		tRes = RESULT_FAIL;
		goto FailureCleanup;
	}

	if ( m_DomainShader.shaderCode.size() > 0 )
	{
		if ( FAILED( hr = CompileShader( &m_DomainShader, "ds_main", D3D_SHADER_STAGE_DOMAIN, &pShaderBuf ) ) )
		{
			tRes = RESULT_FAIL;
			goto FailureCleanup;
		}

		for (int devIdx = 0; devIdx < ARRAYSIZE(m_pDS); ++devIdx)
		{
			if ( FAILED( hr = m_pDeviceArray[devIdx]->CreateDomainShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pDS[devIdx] ) ) )
			{
				WriteToLog( _T( "%s CreateDomainShader()[%d] failed. hr=%s" ), LOG_HEADER, 
					devIdx, D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto FailureCleanup;
			}
		}

		ON_EACH_CONTEXT->DSSetShader( m_pDS[CONTEXT_ID], NULL, 0 );

		for (int devIdx = 0; devIdx < ARRAYSIZE(m_pGS); ++devIdx)
		{
			if ( FAILED( hr = m_pDeviceArray[devIdx]->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), 
																							soDecls, numSoDecls, 
																							NULL, 0, 
																							D3D11_SO_NO_RASTERIZED_STREAM, NULL,
																							&m_pGS[devIdx]
																						  ) ) )
			{
				WriteToLog( _T( "%s CreateGeometryShaderWithStreamOutput()[%d] failed. hr=%s" ), LOG_HEADER, 
					devIdx, D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto FailureCleanup;
			}
		}

		ON_EACH_CONTEXT->GSSetShader( m_pGS[CONTEXT_ID], NULL, 0 );

		SAFE_RELEASE( pShaderBuf );
	}
	else if ( GetFramework()->GetCurrentTestCase() > 0 )
	{
		WriteToLog( _T( "%s No domain shader named" ), LOG_HEADER );
		tRes = RESULT_FAIL;
		goto FailureCleanup;
	}

Exit:
	SAFE_RELEASE( pShaderBuf );

	return tRes;

FailureCleanup:
	SAFE_RELEASE_ARRAY( m_pVS );
	SAFE_RELEASE_ARRAY( m_pHS );
	SAFE_RELEASE_ARRAY( m_pDS );
	SAFE_RELEASE_ARRAY( m_pGS );
	SAFE_RELEASE( pShaderBuf );

	goto Exit;
}

typedef void (__cdecl *DebugSetMuteProc)(BOOL bMute);

//
TEST_RESULT
CTessellationConfTest::Setup()
{
	HRESULT         hr = S_OK;
	TEST_RESULT     tRes = RESULT_PASS;

	m_pDevice = g_TestApp.GetDevice();
	m_pDeviceContext = GetImmediateContext();

	D3D_DRIVER_TYPE deviceType = D3D_DRIVER_TYPE_REFERENCE;

	if ( GetFramework()->IsMobile() )
	{
		deviceType = D3D_DRIVER_TYPE_WARP;
	}

	if ( FAILED( hr = g_TestApp.CreateDevice(NULL, deviceType, D3D11_CREATE_DEVICE_DEBUG, g_TestApp.m_D3DOptions.FeatureLevel, &m_pRefDevice, &m_pRefDeviceContext ) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("%s CreateDevice(ref) failed. hr=%s"), LOG_HEADER, D3DHResultToString(hr).c_str() );
	}

	// Set environment variable to disable loading PSAPI.DLL
	SetEnvironmentVariable("COMPILER_DISABLE_REPORT", "1");

	// Disable debug spew from shader compiler
	m_hcompiler = LoadLibrary("D3DCompiler_Test.dll");
	if( m_hcompiler != NULL)
	{
		DebugSetMuteProc DebugSetMute = (DebugSetMuteProc) GetProcAddress(m_hcompiler,"DebugSetMute");

		if(DebugSetMute != NULL)
		{
			DebugSetMute(TRUE);
		}
	}

	return tRes;
}

//
void
CTessellationConfTest::Cleanup( )
{
	SAFE_RELEASE(m_pRefDeviceContext);
	SAFE_RELEASE(m_pRefDevice);

	if(m_hcompiler)
	{
		FreeLibrary(m_hcompiler);
		m_hcompiler = NULL;
	}

	return;
}

//
TEST_RESULT
CTessellationConfTest::SetupResources( D3D11_SO_DECLARATION_ENTRY* soDecls, UINT numSoDecls )
{
	TEST_RESULT     tRes = RESULT_PASS;

	tRes = SetupShaders(soDecls, numSoDecls);
	if( tRes != RESULT_PASS )
		goto Exit;

	tRes = SetupBuffers();
	if( tRes != RESULT_PASS )
		goto Exit;

	// disable rasterization
	ON_EACH_CONTEXT->OMSetRenderTargets( 0, NULL, NULL );

Exit:
	return tRes;
}

//
void
CTessellationConfTest::CleanupResources( )
{

	if (m_pDeviceContext != nullptr && m_pRefDeviceContext != nullptr)
	{
		ID3D11Buffer* pNullConstBuffers[2] = {0};

		ON_EACH_CONTEXT->VSSetConstantBuffers( 0, 2, pNullConstBuffers );
		ON_EACH_CONTEXT->HSSetConstantBuffers( 0, 2, pNullConstBuffers );
		ON_EACH_CONTEXT->DSSetConstantBuffers( 0, 2, pNullConstBuffers );
		ON_EACH_CONTEXT->GSSetConstantBuffers( 0, 2, pNullConstBuffers );
		ON_EACH_CONTEXT->VSSetShader(NULL, NULL, 0);
		ON_EACH_CONTEXT->HSSetShader(NULL, NULL, 0);
		ON_EACH_CONTEXT->DSSetShader(NULL, NULL, 0);
		ON_EACH_CONTEXT->GSSetShader(NULL, NULL, 0);
		ON_EACH_CONTEXT->SOSetTargets(0, NULL, NULL);
	}
  
	SAFE_RELEASE_ARRAY( m_pConstBuffers[0] );
	SAFE_RELEASE_ARRAY( m_pConstBuffers[1] );
	SAFE_RELEASE_ARRAY( m_pVS );
	SAFE_RELEASE_ARRAY( m_pHS );
	SAFE_RELEASE_ARRAY( m_pDS );
	SAFE_RELEASE_ARRAY( m_pGS );
	SAFE_RELEASE_ARRAY( m_pSOBuffer );
	SAFE_RELEASE_ARRAY( m_pSOQuery );
	SAFE_RELEASE_ARRAY( m_pPipeStatsQuery );

	if (m_pDeviceContext != nullptr && m_pRefDeviceContext != nullptr)
	{
		ON_EACH_CONTEXT->Flush();
	}

	return;
}

//
TEST_RESULT 
CTessellationConfTest::CheckSOQueries( D3D11_QUERY_DATA_SO_STATISTICS soStats[2], int size, bool validateResults )
{
	TEST_RESULT     tRes = RESULT_PASS;

	for (int devIdx = 0; devIdx < size; ++devIdx)
	{
		HRESULT     hr = S_FALSE;

		hr = GetFramework()->GetDataWithTimeout(m_pContextArray[devIdx], m_pSOQuery[devIdx], &soStats[devIdx], sizeof(soStats[devIdx]), 0);

		if ( hr != S_OK )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s GetData(pQuery[%d]) failed or timed out, hr=%s" ), LOG_HEADER, 
				devIdx, D3DHResultToString(hr).c_str() );
		}

		// verify number of outputs was not capped
		if (   soStats[devIdx].PrimitivesStorageNeeded != soStats[devIdx].NumPrimitivesWritten
			|| (validateResults && (0 == soStats[devIdx].NumPrimitivesWritten)) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s Not enough space in SO Buffer, Device[%d], Written=%I64d, Needed=%I64d" ), 
				LOG_HEADER, devIdx, soStats[devIdx].NumPrimitivesWritten, soStats[devIdx].PrimitivesStorageNeeded );
		}
	}

	return tRes;
}

//
TEST_RESULT 
CTessellationConfTest::CheckPipeStats( D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeStats[2], int size )
{
	TEST_RESULT     tRes = RESULT_PASS;

	for ( int devIdx = 0; devIdx < size; ++devIdx )
	{
		HRESULT     hr = S_FALSE;

		hr = GetFramework()->GetDataWithTimeout(m_pContextArray[devIdx], m_pPipeStatsQuery[devIdx], &pipeStats[devIdx], sizeof(pipeStats[devIdx]), 0);

		if ( hr != S_OK )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T( "%s GetData(pQuery[%d]) failed or timed out, hr=%s" ), LOG_HEADER, 
				devIdx, D3DHResultToString(hr).c_str() );
		}
	}

	return tRes;
}

//
void
CTessellationConfApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();
	UnRegisterOptionVariable("srconly");
	m_D3DOptions.SrcOnly = true;

	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	return;
}

//
bool
CTessellationConfApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "Tessellator" ), CTessellatorTest ) ) return false;
	if( !ADD_TEST( _T( "Hull" ), CHullShaderTest ) ) return false;
	if( !ADD_TEST( _T( "Domain" ), CDomainShaderTest ) ) return false;
	if( !ADD_TEST( _T( "HullPhases" ), CHullShaderPhasesTest ) ) return false;

	return true;
}
