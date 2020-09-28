//////////////////////////////////////////////////////////////////////////
//	MapDefault.cpp
//
//	Test class and the application class for WGF11MapDefault test
//
//	Purpose:
//	a) Validate drivers don't do a behind the scenes copy when mapping a default buffer
//	b) Validate performance of reading and writing memory to mapped default buffers
//	
//////////////////////////////////////////////////////////////////////////

#include "MapDefault.h"

CMapDefaultApp  g_TestApp;

const UINT SIZE_512KB = 512 * 1024;
const UINT SIZE_1MB = 1024 * 1024;
const UINT SIZE_2MB = 2 * 1024 * 1024;
const UINT SIZE_4MB = 4 * 1024 * 1024;
const UINT SIZE_16MB = 16 * 1024 * 1024;

const UINT WARMUP_LOOPS = 6;
const UINT EFFECTIVE_LOOPS = 16;
const UINT NUM_REPS = 20;
const UINT MAX_RETRIES = 4;

//------------------------------------------------------------------------------------------------
// CMapDefaultApp
//------------------------------------------------------------------------------------------------

bool CMapDefaultApp::InitTestGroups()
{
    // Register test classes with the group manager.
    if( !ADD_TEST( _T( "Zero_Copy" ), CMapDefaultZeroCopyTest ) ) return false;
	if( !ADD_TEST( _T( "Map_Performance" ), CMapDefaultPerfTest ) ) return false;
	
	return true;
    
}

void CMapDefaultApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
}


//------------------------------------------------------------------------------------------------
// CMapDefaultZeroCopyTest
//------------------------------------------------------------------------------------------------

void CMapDefaultZeroCopyTest::InitTestParameters( )
{
	CTestCaseParameter<UINT> *pSizeInBytesParam = AddParameter( _T( "ResourceSizeInBytes" ), &m_SizeInBytes );
    CUserValueSet<UINT> *pSizeInBytesValues = new CUserValueSet<UINT>;
    pSizeInBytesValues->AddValue( SIZE_512KB );
    pSizeInBytesValues->AddValue( SIZE_1MB );
    pSizeInBytesValues->AddValue( SIZE_2MB );
	pSizeInBytesValues->AddValue( SIZE_4MB );
    testfactor::RFactor rfSizeInBytes = AddParameterValueSet( pSizeInBytesParam, pSizeInBytesValues );

	CTestCaseParameter<bool> *pMapReadWriteParam = AddParameter( _T( "MAP_READ_WRITE" ), &m_MapReadWrite );
    CUserValueSet<bool> *pMapReadWriteValues = new CUserValueSet<bool>;
    pMapReadWriteValues->AddValue( false );
    pMapReadWriteValues->AddValue( true );
    testfactor::RFactor rfMapReadWrite = AddParameterValueSet( pMapReadWriteParam, pMapReadWriteValues );
    
	SetRootTestFactor( rfMapReadWrite * rfSizeInBytes );
}
	
TEST_RESULT CMapDefaultZeroCopyTest::Setup()
{
	HRESULT hr;
	m_pDevice = g_TestApp.GetDevice();
    m_pEffectiveContext = g_TestApp.GetEffectiveContext();
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIAdapter1* pAdapter1 = nullptr;

	TEST_RESULT tr = RESULT_PASS;

	if ( !m_pDevice || !m_pEffectiveContext )
	{
		LogError( __FILEW__, __LINE__, L"Device or DeviceContext is NULL.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
	}

	// QI for IDXGIDevice2
	hr = m_pDevice->QueryInterface( __uuidof( IDXGIDevice2 ), reinterpret_cast< void** >( &m_pDXGIDevice2 ) );
	if ( FAILED(hr) )
	{
		LogError( __FILEW__, __LINE__, L"QI for IDXGIDevice2 failed.", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		return RESULT_FAIL;
	}

	D3D11_FEATURE_DATA_D3D11_OPTIONS1 Caps;
	ZeroMemory(&Caps, sizeof(Caps));
	if ( FAILED ( hr = m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, (void*)&Caps, sizeof(Caps)) ) )
	{
		LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
	}

	if ( Caps.MapOnDefaultBuffers == FALSE )
	{
		WriteToLog( "MapOnDefaultBuffers feature not supported, skipping test group." );
		tr = RESULT_SKIP;
		goto Exit;
	}

	// Create the resource data which is large enough for the largest sized resource in the test
	UINT dataSize = SIZE_4MB;

	m_pResourceData = new BYTE[dataSize];
	for ( UINT i = 0; i < dataSize; i++ )
	{
		m_pResourceData[i] = i % 256;
	}

	// raise the priority of the current thread to reduce perf noise caused by other processes
	if( !SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL) )
	{
		LogError( __FILEW__, __LINE__, L"SetThreadPriority() failed", GetLastError(), ERRORCODE_TYPE_WIN32, gFCRequiredCapabilityNotSupported );
	}

	// Set thread affinity so that this thread will only run on one CPU on multi-core systems
	// Bind this thread to the first processor.
	SetThreadAffinityMask(GetCurrentThread(), 0x1);

Exit:
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pAdapter1);

	return tr;
}

TEST_RESULT CMapDefaultZeroCopyTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	LARGE_INTEGER QPCArray_Staging[EFFECTIVE_LOOPS], QPCScore_Staging, QPCTmp_Staging;
	LARGE_INTEGER QPCArray_Default[EFFECTIVE_LOOPS], QPCScore_Default, QPCTmp_Default;
	LARGE_INTEGER QPCTimerFrequency; 
	bool passed;

	// Get Timer Frequency
	if ( !QueryPerformanceFrequency( &QPCTimerFrequency ) || QPCTimerFrequency.QuadPart == 0)
	{
		LogError( __FILEW__, __LINE__, L"QueryPerformanceFrequency() failed", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
	}

	// these operations are very fast, and very susceptible to noise
	// therefore, we will retry up to 3 times
	for ( UINT j = 0; j < MAX_RETRIES; j++ )
	{
		tr = CreateBuffers();
		if ( tr != RESULT_PASS )
		{
			WriteToLog( "%s in %s at line %d.  CreateBuffers() failed.", __FUNCTION__, __FILE__, __LINE__ );
			goto Exit;
		}

		for ( UINT i = 0; i < WARMUP_LOOPS + EFFECTIVE_LOOPS; i++ )
		{
			if ( i % 2 == 0 )
			{
				tr = Scenario_Staging(QPCTmp_Staging);
				if ( tr != RESULT_PASS )
				{
					WriteToLog( "%s in %s at line %d.  Scenario_Staging() failed.", __FUNCTION__, __FILE__, __LINE__ );
					goto Exit;
				}

				tr = Scenario_Default(QPCTmp_Default);
				if ( tr != RESULT_PASS )
				{
					WriteToLog( "%s in %s at line %d.  Scenario_Default() failed.", __FUNCTION__, __FILE__, __LINE__ );
					goto Exit;
				}
			}
			else
			{
				tr = Scenario_Default(QPCTmp_Default);
				if ( tr != RESULT_PASS )
				{
					WriteToLog( "%s in %s at line %d.  Scenario_Default() failed.", __FUNCTION__, __FILE__, __LINE__ );
					goto Exit;
				}

				tr = Scenario_Staging(QPCTmp_Staging);
				if ( tr != RESULT_PASS )
				{
					WriteToLog( "%s in %s at line %d.  Scenario_Staging() failed.", __FUNCTION__, __FILE__, __LINE__ );
					goto Exit;
				}
			}

			// Discard the results from the warm-up passes
			if ( i < WARMUP_LOOPS )
			{
				continue;
			}

			QPCArray_Staging[ i - WARMUP_LOOPS] = QPCTmp_Staging;
			QPCArray_Default[ i - WARMUP_LOOPS] = QPCTmp_Default;
		}

		QPCScore_Staging = CostScore( QPCArray_Staging, EFFECTIVE_LOOPS );
		QPCScore_Default = CostScore( QPCArray_Default, EFFECTIVE_LOOPS );

		passed = QPCScore_Default.QuadPart <= (QPCScore_Staging.QuadPart * 1.1);

		// Log results
		WriteToLog("Read and write to staging buffer scenario: %lld (%lld ms)\n", QPCScore_Staging.QuadPart, QPCScore_Staging.QuadPart * 1000 / QPCTimerFrequency.QuadPart);
		WriteToLog("Read and write to default buffer scenario: %lld (%lld ms)\n", QPCScore_Default.QuadPart, QPCScore_Default.QuadPart * 1000 / QPCTimerFrequency.QuadPart);

		if (passed == false && j < (MAX_RETRIES - 1))
		{
			WriteToLog("Failed attempt number %u, retrying", j);
			SAFE_RELEASE( m_pStagingBuffer );
			SAFE_RELEASE( m_pDefaultBuffer );
			continue;
		}

		if ( passed == false )
		{
			WriteToLog("Default score exceeded staging score by: %i percent\n", (int)(((double)(QPCScore_Default.QuadPart - QPCScore_Staging.QuadPart) / (double)QPCScore_Staging.QuadPart) * 100.0));
			LogError( __FILEW__, __LINE__, L"The default buffer scenario was greater than 10 percent longer than the staging scenario", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
			goto Exit;
		}
		
		break;
	}
	
Exit:
	SAFE_RELEASE( m_pStagingBuffer );
	SAFE_RELEASE( m_pDefaultBuffer );

	return tr;
}


TEST_RESULT CMapDefaultZeroCopyTest::CreateBuffers()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	D3D11_SUBRESOURCE_DATA subResData;
	ZeroMemory( &subResData, sizeof ( D3D11_SUBRESOURCE_DATA ) );
	subResData.pSysMem = m_pResourceData;

	D3D11_BUFFER_DESC bufferDesc; 
	bufferDesc.BindFlags		= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.ByteWidth		= m_SizeInBytes;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags		= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    bufferDesc.Usage			= D3D11_USAGE_DEFAULT;

	hr = GetDevice()->CreateBuffer( &bufferDesc, &subResData, reinterpret_cast<ID3D11Buffer**>(&m_pDefaultBuffer) );
    if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"CreateBuffer() for default buffer failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	bufferDesc.BindFlags		= 0;
	bufferDesc.Usage			= D3D11_USAGE_STAGING;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags		= 0;

	hr = GetDevice()->CreateBuffer( &bufferDesc, &subResData, reinterpret_cast<ID3D11Buffer**>(&m_pStagingBuffer) );
    if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"CreateBuffer() for staging buffer failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

Exit:
	return tr;
}

TEST_RESULT CMapDefaultZeroCopyTest::Scenario_Staging( LARGE_INTEGER &Cost )
{
	LARGE_INTEGER QPC1_Start, QPC1_End;
	LARGE_INTEGER QPC2_Start, QPC2_End;

	HANDLE hEvent = nullptr; 
	TEST_RESULT tr = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	HRESULT hr;
	tstring formattedShaderCode;
	ID3D11UnorderedAccessView* pUAV = nullptr;
	ID3D11UnorderedAccessView* pUAVnull = nullptr;
	ID3D11ComputeShader* pCS = nullptr;
	ID3D10Blob* pShaderBlob = nullptr;
    ID3D10Blob* pErrorBlob = nullptr;
	
	// Create an event for EnqueueSetEvent
	hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( !hEvent )
	{
		LogError( __FILEW__, __LINE__, L"CreateEvent() failed", GetLastError(), ERRORCODE_TYPE_WIN32, gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
		goto Exit;
	}

	// Create our compute shader
	formattedShaderCode = FormatString( szDispatchShader, 4, 4, 1 );

	hr = GetFramework()->CompileShaderFromMemory(formattedShaderCode.c_str(), formattedShaderCode.size(), "cs_main", 
        D3D_SHADER_STAGE_COMPUTE, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_0, 0,
        &pShaderBlob, &pErrorBlob,
        nullptr, nullptr);

	if ( FAILED(hr) || pShaderBlob == nullptr )
	{
		LogError( __FILEW__, __LINE__, L"CompileShaderFromMemory() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		if (pErrorBlob)
        {
            WriteToLog( "Compiler errors: %s", pErrorBlob->GetBufferPointer() );
        }
        tr = RESULT_FAIL;
		goto Exit;
	}

	hr = m_pDevice->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &pCS );

	if ( FAILED(hr) || pCS == nullptr )
	{
		LogError( __FILEW__, __LINE__, L"CreateComputeShader() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
	}
	
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS; 
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW; 
	uavDesc.Buffer.FirstElement = 0; 
	uavDesc.Buffer.NumElements = m_SizeInBytes / sizeof(UINT);

	hr = m_pDevice->CreateUnorderedAccessView( m_pDefaultBuffer, &uavDesc, &pUAV );

	if ( FAILED(hr) )
	{
		LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
	}
	
	if ( m_MapReadWrite )
	{
		hr = m_pEffectiveContext->Map( m_pStagingBuffer, 0, D3D11_MAP_READ_WRITE, 0, &mappedRes );
	}
	else
	{
		hr = m_pEffectiveContext->Map( m_pStagingBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes );
	}
	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	memcpy( mappedRes.pData, m_pResourceData, m_SizeInBytes );

	// Insert ETW event for timing start
	CEPRINTF( "Test Staging Start" );

	if ( !QueryPerformanceCounter( &QPC1_Start ) )
	{
		LogError( __FILEW__, __LINE__, L"QueryPeformanceCounter() failed ", GetLastError(), ERRORCODE_TYPE_WIN32, gFCConformanceFailure );
		tr = RESULT_FAIL;
		m_pEffectiveContext->Unmap( m_pStagingBuffer, 0 );
		goto Exit;
	}

	m_pEffectiveContext->Unmap( m_pStagingBuffer, 0 );

    m_pEffectiveContext->CSSetUnorderedAccessViews( 0, 1, &pUAV, nullptr );

	m_pEffectiveContext->CSSetShader( pCS, nullptr, 0 );

	m_pEffectiveContext->Dispatch( 4, 4, 1 );

	m_pEffectiveContext->CopyResource( m_pStagingBuffer, m_pDefaultBuffer );

	// unbind from pipeline
	m_pEffectiveContext->CSSetShader( nullptr, nullptr, 0 );
	m_pEffectiveContext->CSSetUnorderedAccessViews( 0, 1, &pUAVnull, nullptr );

	hr = m_pDXGIDevice2->EnqueueSetEvent( hEvent );
    if (FAILED(hr))
    {
        LogError( __FILEW__, __LINE__, L"EnqueueSetEvent() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
    }

	QPC_WITH_ERROR_CHECK( QPC1_End );

	DWORD dwResult = WaitForSingleObject( hEvent, 5000 );
	if (dwResult != WAIT_OBJECT_0)
	{
		LogError( __FILEW__, __LINE__, L"Event(s) not signaled within expected time.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		tr = RESULT_FAIL;	
        goto Exit;  
	}

	QPC_WITH_ERROR_CHECK( QPC2_Start );

	if (m_MapReadWrite)
	{
		hr = m_pEffectiveContext->Map( m_pStagingBuffer, 0, D3D11_MAP_READ_WRITE, 0, &mappedRes );
	}
	else
	{
		hr = m_pEffectiveContext->Map( m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedRes );
	}
	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	if ( !QueryPerformanceCounter( &QPC2_End ) )
	{
		LogError( __FILEW__, __LINE__, L"QueryPeformanceCounter() failed ", GetLastError(), ERRORCODE_TYPE_WIN32, gFCConformanceFailure );
		tr = RESULT_FAIL;
		m_pEffectiveContext->Unmap( m_pStagingBuffer, 0 );
		goto Exit;
	}

	m_pEffectiveContext->Unmap( m_pStagingBuffer, 0 );

Exit:
	// Insert ETW event for test end
	CEPRINTF("Test Staging End");

	if ( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = nullptr;
	}

	SAFE_RELEASE( pUAV );
	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );
	SAFE_RELEASE( pCS );

	Cost.QuadPart = (QPC1_End.QuadPart - QPC1_Start.QuadPart) + (QPC2_End.QuadPart - QPC2_Start.QuadPart);

	return tr;
}

TEST_RESULT CMapDefaultZeroCopyTest::Scenario_Default( LARGE_INTEGER &Cost )
{
	LARGE_INTEGER QPC1_Start, QPC1_End;
	LARGE_INTEGER QPC2_Start, QPC2_End;

	HANDLE hEvent = nullptr; 
	TEST_RESULT tr = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	HRESULT hr;
	tstring formattedShaderCode;
	ID3D11UnorderedAccessView* pUAV = nullptr;
	ID3D11UnorderedAccessView* pUAVnull = nullptr;
	ID3D11ComputeShader* pCS = nullptr;
	ID3D10Blob* pShaderBlob = nullptr;
    ID3D10Blob* pErrorBlob = nullptr;

	// Create an event for EnqueueSetEvent
	hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( !hEvent )
	{
		LogError( __FILEW__, __LINE__, L"CreateEvent() failed", GetLastError(), ERRORCODE_TYPE_WIN32, gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
		goto Exit;
	}

	// Create our compute shader
	formattedShaderCode = FormatString( szDispatchShader, 4, 4, 1 );

	hr = GetFramework()->CompileShaderFromMemory(formattedShaderCode.c_str(), formattedShaderCode.size(), "cs_main", 
        D3D_SHADER_STAGE_COMPUTE, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_0, 0,
        &pShaderBlob, &pErrorBlob,
        nullptr, nullptr);

	if ( FAILED(hr) || pShaderBlob == nullptr )
	{
		LogError( __FILEW__, __LINE__, L"CompileShaderFromMemory() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		if (pErrorBlob)
        {
            WriteToLog( "Compiler errors: %s", pErrorBlob->GetBufferPointer() );
        }
        tr = RESULT_FAIL;
		goto Exit;
	}

	hr = m_pDevice->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &pCS );

	if ( FAILED(hr) || pCS == nullptr )
	{
		LogError( __FILEW__, __LINE__, L"CreateComputeShader() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS; 
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW; 
	uavDesc.Buffer.FirstElement = 0; 
	uavDesc.Buffer.NumElements = m_SizeInBytes / sizeof(UINT);

	hr = m_pDevice->CreateUnorderedAccessView( m_pDefaultBuffer, &uavDesc, &pUAV );

	if ( FAILED(hr) )
	{
		LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
	}
	
	if ( m_MapReadWrite )
	{
		hr = m_pEffectiveContext->Map( m_pDefaultBuffer, 0, D3D11_MAP_READ_WRITE, 0, &mappedRes );
	}
	else
	{
		hr = m_pEffectiveContext->Map( m_pDefaultBuffer, 0, D3D11_MAP_WRITE, 0, &mappedRes );
	}

	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	memcpy( mappedRes.pData, m_pResourceData, m_SizeInBytes );

	// Insert ETW event for timing start
	CEPRINTF( "Test Default Start" );

	if ( !QueryPerformanceCounter( &QPC1_Start ) )
	{
		LogError( __FILEW__, __LINE__, L"QueryPeformanceCounter() failed ", GetLastError(), ERRORCODE_TYPE_WIN32, gFCConformanceFailure );
		tr = RESULT_FAIL;
		m_pEffectiveContext->Unmap( m_pDefaultBuffer, 0 );
		goto Exit;
	}

	m_pEffectiveContext->Unmap( m_pDefaultBuffer, 0 );

    // UAVs must be bound after the Unmap call, because the runtime unbinds bound UAVs when they are mapped
    m_pEffectiveContext->CSSetUnorderedAccessViews( 0, 1, &pUAV, nullptr );

	m_pEffectiveContext->CSSetShader( pCS, nullptr, 0 );

	m_pEffectiveContext->Dispatch( 4, 4, 1 );

	m_pEffectiveContext->CopyResource( m_pStagingBuffer, m_pDefaultBuffer );

	// unbind from pipeline
	m_pEffectiveContext->CSSetShader( nullptr, nullptr, 0 );
	m_pEffectiveContext->CSSetUnorderedAccessViews( 0, 1, &pUAVnull, nullptr );

	hr = m_pDXGIDevice2->EnqueueSetEvent( hEvent );
    if (FAILED(hr))
    {
        LogError( __FILEW__, __LINE__, L"EnqueueSetEvent() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
		tr = RESULT_FAIL;
		goto Exit;
    }

	QPC_WITH_ERROR_CHECK( QPC1_End );

	DWORD dwResult = WaitForSingleObject( hEvent, 5000 );
	if (dwResult != WAIT_OBJECT_0)
	{
		LogError( __FILEW__, __LINE__, L"Event(s) not signaled within expected time.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
		tr = RESULT_FAIL;	
        goto Exit;  
	}

	QPC_WITH_ERROR_CHECK( QPC2_Start );

	if (m_MapReadWrite)
	{
		hr = m_pEffectiveContext->Map( m_pDefaultBuffer, 0, D3D11_MAP_READ_WRITE, 0, &mappedRes );
	}
	else
	{
		hr = m_pEffectiveContext->Map( m_pDefaultBuffer, 0, D3D11_MAP_READ, 0, &mappedRes );
	}

	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	if ( !QueryPerformanceCounter( &QPC2_End ) )
	{
		LogError( __FILEW__, __LINE__, L"QueryPeformanceCounter() failed ", GetLastError(), ERRORCODE_TYPE_WIN32, gFCConformanceFailure );
		tr = RESULT_FAIL;
		m_pEffectiveContext->Unmap( m_pDefaultBuffer, 0 );
		goto Exit;
	}

	m_pEffectiveContext->Unmap( m_pDefaultBuffer, 0 );

Exit:
	// Insert ETW event for test end
	CEPRINTF("Test Default End");

	if ( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = nullptr;
	}

	SAFE_RELEASE( pUAV );
	SAFE_RELEASE( pShaderBlob );
	SAFE_RELEASE( pErrorBlob );
	SAFE_RELEASE( pCS );

	Cost.QuadPart = (QPC1_End.QuadPart - QPC1_Start.QuadPart) + (QPC2_End.QuadPart - QPC2_Start.QuadPart);

	return tr;
}

LARGE_INTEGER CMapDefaultZeroCopyTest::CostScore( const LARGE_INTEGER * pCostPerTest, UINT numTests)
{
	LARGE_INTEGER CostAverage;
	CostAverage.QuadPart = 0;
	
	for ( UINT i = 0; i < numTests; i++ )
	{
		CostAverage.QuadPart += pCostPerTest[i].QuadPart;
	}

	CostAverage.QuadPart /= numTests;

	return CostAverage;
}

void CMapDefaultZeroCopyTest::Cleanup()
{
	if ( m_pResourceData )
	{
		delete [] m_pResourceData;
		m_pResourceData = nullptr;
	}

	SAFE_RELEASE( m_pDXGIDevice2 );
}

//------------------------------------------------------------------------------------------------
// CMapDefaultPerfTest (base class is CMapDefaultZeroCopyTest)
//------------------------------------------------------------------------------------------------

void CMapDefaultPerfTest::InitTestParameters( )
{
	CTestCaseParameter<UINT> *pSizeInBytesParam = AddParameter( _T( "ResourceSizeInBytes" ), &m_SizeInBytes );
    CUserValueSet<UINT> *pSizeInBytesValues = new CUserValueSet<UINT>;
    pSizeInBytesValues->AddValue( SIZE_512KB );
    pSizeInBytesValues->AddValue( SIZE_1MB );
    pSizeInBytesValues->AddValue( SIZE_2MB );
	pSizeInBytesValues->AddValue( SIZE_4MB );
    testfactor::RFactor rfSizeInBytes = AddParameterValueSet( pSizeInBytesParam, pSizeInBytesValues );

	CTestCaseParameter<UINT> *pCPUAccessParam = AddParameter( _T( "CPUAccessFlags" ), &m_CPUAccessFlags );
    CUserValueSet<UINT> *pCPUAccessValues = new CUserValueSet<UINT>;
    pCPUAccessValues->AddValue( D3D11_CPU_ACCESS_READ );
    pCPUAccessValues->AddValue( D3D11_CPU_ACCESS_WRITE );
	pCPUAccessValues->AddValue( D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE );
    testfactor::RFactor rfCPUAccessFlag = AddParameterValueSet( pCPUAccessParam, pCPUAccessValues );
    
	SetRootTestFactor( rfCPUAccessFlag * rfSizeInBytes );
}

TEST_RESULT CMapDefaultPerfTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;

	ULONG64 QTCTArray1_Staging[EFFECTIVE_LOOPS], QTCTScore1_Staging, QTCTTmp1_Staging;
	ULONG64 QTCTArray2_Staging[EFFECTIVE_LOOPS], QTCTScore2_Staging, QTCTTmp2_Staging;
	ULONG64 QTCTArray3_Staging[EFFECTIVE_LOOPS], QTCTScore3_Staging, QTCTTmp3_Staging;

	ULONG64 QTCTArray1_Default[EFFECTIVE_LOOPS], QTCTScore1_Default, QTCTTmp1_Default;
	ULONG64 QTCTArray2_Default[EFFECTIVE_LOOPS], QTCTScore2_Default, QTCTTmp2_Default;
	ULONG64 QTCTArray3_Default[EFFECTIVE_LOOPS], QTCTScore3_Default, QTCTTmp3_Default;

	bool passed;

	// these operations are very fast, and very susceptible to noise
	// therefore, we will retry up to 3 times
	for ( UINT j = 0; j < MAX_RETRIES; j++ )
	{
		tr = CreateBuffers();
		if ( tr != RESULT_PASS )
		{
			WriteToLog( "%s in %s at line %d.  CreateBuffers() failed.", __FUNCTION__, __FILE__, __LINE__ );
			goto Exit;
		}

		for ( UINT i = 0; i < WARMUP_LOOPS + EFFECTIVE_LOOPS; i++ )
		{
			// check CPU access flags to see what to run
			if ( m_CPUAccessFlags & D3D11_CPU_ACCESS_WRITE )
			{
				tr = Scenario_Write(QTCTTmp1_Staging, QTCTTmp1_Default, j);
				if ( tr != RESULT_PASS )
				{
					WriteToLog( "%s in %s at line %d.  Scenario_Write() failed.", __FUNCTION__, __FILE__, __LINE__ );
					goto Exit;
				}
			}
			else
			{
				QTCTTmp1_Staging = 0;
				QTCTTmp1_Default = 0;
			}

			if ( m_CPUAccessFlags & D3D11_CPU_ACCESS_READ )
			{
				tr = Scenario_Read(QTCTTmp2_Staging, QTCTTmp2_Default, j);
				if ( tr != RESULT_PASS )
				{
					WriteToLog( "%s in %s at line %d.  Scenario_Read() failed.", __FUNCTION__, __FILE__, __LINE__ );
					goto Exit;
				}
			}
			else
			{
				QTCTTmp2_Staging = 0;
				QTCTTmp2_Default = 0;
			}

			if ( m_CPUAccessFlags == (D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE) )
			{
				tr = Scenario_ReadWrite(QTCTTmp3_Staging, QTCTTmp3_Default, j);
				if ( tr != RESULT_PASS )
				{
					WriteToLog( "%s in %s at line %d.  Scenario_ReadWrite() failed.", __FUNCTION__, __FILE__, __LINE__ );
					goto Exit;
				}
			}
			else
			{
				QTCTTmp3_Staging = 0;
				QTCTTmp3_Default = 0;
			}

			Sleep(100);

			// Discard the results from the warm-up passes
			if ( i < WARMUP_LOOPS )
			{
				continue;
			}

			QTCTArray1_Staging[ i - WARMUP_LOOPS] = QTCTTmp1_Staging;
			QTCTArray1_Default[ i - WARMUP_LOOPS] = QTCTTmp1_Default;

			QTCTArray2_Staging[ i - WARMUP_LOOPS] = QTCTTmp2_Staging;
			QTCTArray2_Default[ i - WARMUP_LOOPS] = QTCTTmp2_Default;

			QTCTArray3_Staging[ i - WARMUP_LOOPS] = QTCTTmp3_Staging;
			QTCTArray3_Default[ i - WARMUP_LOOPS] = QTCTTmp3_Default;

			// useful output for debugging
			// WriteToLog("%lld,%lld\n", QPCTmp1_Staging.QuadPart, QPCTmp1_Default.QuadPart);

		}

		QTCTScore1_Staging = CostScore( QTCTArray1_Staging, EFFECTIVE_LOOPS );
		QTCTScore1_Default = CostScore( QTCTArray1_Default, EFFECTIVE_LOOPS );

		QTCTScore2_Staging = CostScore( QTCTArray2_Staging, EFFECTIVE_LOOPS );
		QTCTScore2_Default = CostScore( QTCTArray2_Default, EFFECTIVE_LOOPS );

		QTCTScore3_Staging = CostScore( QTCTArray3_Staging, EFFECTIVE_LOOPS );
		QTCTScore3_Default = CostScore( QTCTArray3_Default, EFFECTIVE_LOOPS );

		passed = QTCTScore1_Default <= (QTCTScore1_Staging * 1.1) && QTCTScore2_Default <= (QTCTScore2_Staging * 1.1) && QTCTScore3_Default <= (QTCTScore3_Staging * 1.1);

		if (passed == false && j < (MAX_RETRIES - 1))
		{
			WriteToLog("Failed attempt number %u, retrying", j);
			SAFE_RELEASE( m_pDefaultBuffer );
			SAFE_RELEASE( m_pStagingBuffer );
			continue;
		}

		// Log results
		WriteToLog("Memory access cost for WRITE using staging buffer: %lld\n", QTCTScore1_Staging);
		WriteToLog("Memory access cost for WRITE using default buffer: %lld\n", QTCTScore1_Default);

		if ( QTCTScore1_Default > (QTCTScore1_Staging * 1.1) )
		{
			WriteToLog("Default score exceeded staging score by: %i percent\n", (int)(((double)(QTCTScore1_Default - QTCTScore1_Staging) / (double)QTCTScore1_Staging) * 100.0));
			LogError( __FILEW__, __LINE__, L"The default buffer memory access cost for WRITE was greater than 10 percent more than the staging memory access cost", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}

		WriteToLog("Memory access cost for READ using staging buffer: %lld\n", QTCTScore2_Staging);
		WriteToLog("Memory access cost for READ using default buffer: %lld\n", QTCTScore2_Default);

		if ( QTCTScore2_Default > (QTCTScore2_Staging * 1.1) )
		{
			WriteToLog("Default score exceeded staging score by: %i percent\n", (int)(((double)(QTCTScore2_Default - QTCTScore2_Staging) / (double)QTCTScore2_Staging) * 100.0));
			LogError( __FILEW__, __LINE__, L"The default buffer memory access cost for READ was greater than 10 percent more than the staging memory access cost", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}

		WriteToLog("Memory access cost for READ and WRITE using staging buffer: %lld\n", QTCTScore3_Staging);
		WriteToLog("Memory access cost for READ and WRITE using default buffer: %lld\n", QTCTScore3_Default);

		if ( QTCTScore3_Default > (QTCTScore3_Staging * 1.1) )
		{
			WriteToLog("Default score exceeded staging score by: %i percent\n", (int)(((double)(QTCTScore3_Default - QTCTScore3_Staging) / (double)QTCTScore3_Staging) * 100.0));
			LogError( __FILEW__, __LINE__, L"The default buffer memory access cost for READ and WRITE was greater than 10 percent more than the staging memory access cost", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;
		}

		break;
	}
	
Exit:
	SAFE_RELEASE( m_pDefaultBuffer );
	SAFE_RELEASE( m_pStagingBuffer );

	return tr;
}

TEST_RESULT CMapDefaultPerfTest::CreateBuffers()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	D3D11_SUBRESOURCE_DATA subResData;
	ZeroMemory( &subResData, sizeof ( D3D11_SUBRESOURCE_DATA ) );
	subResData.pSysMem = m_pResourceData;

	// Create our DEFAULT and STAGING buffers
	D3D11_BUFFER_DESC bufferDesc; 
	bufferDesc.BindFlags		= D3D11_BIND_UNORDERED_ACCESS; //D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.ByteWidth		= m_SizeInBytes;
	bufferDesc.CPUAccessFlags	= m_CPUAccessFlags;
    bufferDesc.MiscFlags		= 0;
    bufferDesc.Usage			= D3D11_USAGE_DEFAULT;

	hr = GetDevice()->CreateBuffer( &bufferDesc, &subResData, reinterpret_cast<ID3D11Buffer**>(&m_pDefaultBuffer) );
    if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"CreateBuffer() for default buffer failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	bufferDesc.BindFlags		= 0;
	bufferDesc.Usage			= D3D11_USAGE_STAGING;

	hr = GetDevice()->CreateBuffer( &bufferDesc, &subResData, reinterpret_cast<ID3D11Buffer**>(&m_pStagingBuffer) );
    if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"CreateBuffer() for staging buffer failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

Exit:
	return tr;
}

// Non-inlined function used to produce identical behavior for staging and default
_Use_decl_annotations_
void CMapDefaultPerfTest::WriteLoop(BYTE* pDstData, const BYTE* pSrcData, UINT Size)
{
	// write the data to the buffer, byte by byte
	for ( UINT b=0; b<Size; b++ )
	{
		pDstData[b] = pSrcData[b];
	}
}

TEST_RESULT CMapDefaultPerfTest::Scenario_Write( ULONG64 &CostStaging, ULONG64 &CostDefault, UINT retry )
{
	ULONG64 qtct1_start, qtct1_end;
	ULONG64 qtct2_start, qtct2_end;
	ULONG64 qtct1_sum = 0;
	ULONG64 qtct2_sum = 0;
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE mappedResStaging = {0};
	D3D11_MAPPED_SUBRESOURCE mappedResDefault = {0};
	BYTE *pDstMem = nullptr;
	UINT numReps = NUM_REPS * (retry + 1);
	HANDLE hEvent = nullptr; 
	
	// Create an event for EnqueueSetEvent
	hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( !hEvent )
	{
		LogError( __FILEW__, __LINE__, L"CreateEvent() failed", GetLastError(), ERRORCODE_TYPE_WIN32, gFCRequiredCapabilityNotSupported );
		tr = RESULT_FAIL;
		goto Exit;
	}

	m_pEffectiveContext->Flush();
	Sleep(100);

	hr = m_pEffectiveContext->Map( m_pStagingBuffer, 0, D3D11_MAP_WRITE, 0, &mappedResStaging );
	
	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	hr = m_pEffectiveContext->Map( m_pDefaultBuffer, 0, D3D11_MAP_WRITE, 0, &mappedResDefault );
	
	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	// Insert ETW event for timing start
	CEPRINTF( "Test Write Start" );

	for ( UINT i=0; i<numReps; i++ )
	{
		pDstMem = ( BYTE* )( mappedResStaging.pData );

		hr = m_pDXGIDevice2->EnqueueSetEvent( hEvent );
		DWORD dwResult = WaitForSingleObject( hEvent, 5000 );
		if (dwResult != WAIT_OBJECT_0)
		{
			LogError( __FILEW__, __LINE__, L"Event(s) not signaled within expected time.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;	
			goto Exit;  
		}

		QTCT_WITH_ERROR_CHECK( qtct1_start );
		// write the data to the staging buffer, byte by byte
		WriteLoop(pDstMem, m_pResourceData, m_SizeInBytes);
		QTCT_WITH_ERROR_CHECK( qtct1_end );

		qtct1_sum += qtct1_end - qtct1_start;

		pDstMem = ( BYTE* )( mappedResDefault.pData );

		hr = m_pDXGIDevice2->EnqueueSetEvent( hEvent );
		dwResult = WaitForSingleObject( hEvent, 5000 );
		if (dwResult != WAIT_OBJECT_0)
		{
			LogError( __FILEW__, __LINE__, L"Event(s) not signaled within expected time.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			tr = RESULT_FAIL;	
			goto Exit;  
		}

		QTCT_WITH_ERROR_CHECK( qtct2_start );
		// write the data to the default buffer, byte by byte
		WriteLoop(pDstMem, m_pResourceData, m_SizeInBytes);
		QTCT_WITH_ERROR_CHECK( qtct2_end );

		qtct2_sum += qtct2_end - qtct2_start;
	}

Exit:
	if ( hEvent )
	{
		CloseHandle( hEvent );
		hEvent = nullptr;
	}
	if ( mappedResStaging.pData )
	{
		m_pEffectiveContext->Unmap( m_pStagingBuffer, 0 );
	}
	if ( mappedResDefault.pData )
	{
		m_pEffectiveContext->Unmap( m_pDefaultBuffer, 0 );
	}

	// Insert ETW event for test end
	CEPRINTF("Test Write End");

	CostStaging = qtct1_sum;
	CostDefault = qtct2_sum;

	return tr;
}

// Non-inlined function used to produce identical behavior for staging and default
_Use_decl_annotations_
UINT CMapDefaultPerfTest::ReadLoop(const BYTE* pData, UINT Size, UINT numReps)
{
	UINT sum = 0;
	for ( UINT i=0; i<numReps; i++ )
	{
		// read the data from the buffer, byte by byte
		for ( UINT b=0; b<Size; b++ )
		{
			sum += pData[b];
		}
	}
	return sum;
}

TEST_RESULT CMapDefaultPerfTest::Scenario_Read( ULONG64 &CostStaging, ULONG64 &CostDefault, UINT retry )
{
	ULONG64 qtct1_start, qtct1_end;
	ULONG64 qtct2_start, qtct2_end;
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE mappedResStaging = {0};
	D3D11_MAPPED_SUBRESOURCE mappedResDefault = {0};
	BYTE *pSrcMem = nullptr;
	UINT sum = 0;
	UINT numReps = NUM_REPS * (retry + 1);

	hr = m_pEffectiveContext->Map( m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResStaging );
	
	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	hr = m_pEffectiveContext->Map( m_pDefaultBuffer, 0, D3D11_MAP_READ, 0, &mappedResDefault );
	
	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	pSrcMem = ( BYTE* )( mappedResStaging.pData );

	// Insert ETW event for timing start
	CEPRINTF( "Test Write Start" );

	QTCT_WITH_ERROR_CHECK( qtct1_start );
	sum = ReadLoop(pSrcMem, m_SizeInBytes, numReps);
	QTCT_WITH_ERROR_CHECK( qtct1_end );

	pSrcMem = ( BYTE* )( mappedResDefault.pData );
	sum = 0;

	QTCT_WITH_ERROR_CHECK( qtct2_start );
	sum = ReadLoop(pSrcMem, m_SizeInBytes, numReps);
	QTCT_WITH_ERROR_CHECK( qtct2_end );

Exit:
	if ( mappedResStaging.pData )
	{
		m_pEffectiveContext->Unmap( m_pStagingBuffer, 0 );
	}
	if ( mappedResDefault.pData )
	{
		m_pEffectiveContext->Unmap( m_pDefaultBuffer, 0 );
	}

	// Insert ETW event for test end
	CEPRINTF("Test Write End");

	CostStaging = qtct1_end - qtct1_start;
	CostDefault = qtct2_end - qtct2_start;

	return tr;
}

// Non-inlined function used to produce identical behavior for staging and default
_Use_decl_annotations_
void CMapDefaultPerfTest::ReadWriteLoop(BYTE* pData, UINT Size, UINT numReps)
{
	for ( UINT i=0; i<numReps; i++ )
	{
		// read and write data to the buffer, byte by byte
		for ( UINT b=0; b<Size; b++ )
		{
			BYTE value = pData[b];
			pData[b] = value * value;
		}
	}
}

TEST_RESULT CMapDefaultPerfTest::Scenario_ReadWrite( ULONG64 &CostStaging, ULONG64 &CostDefault, UINT retry )
{
	ULONG64 qtct1_start, qtct1_end;
	ULONG64 qtct2_start, qtct2_end;
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_MAPPED_SUBRESOURCE mappedResStaging = {0};
	D3D11_MAPPED_SUBRESOURCE mappedResDefault = {0};
	BYTE *pSrcMem = nullptr;
	UINT value;
	UINT numReps = NUM_REPS * (retry + 1);

	hr = m_pEffectiveContext->Map( m_pStagingBuffer, 0, D3D11_MAP_READ_WRITE, 0, &mappedResStaging );
	
	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	hr = m_pEffectiveContext->Map( m_pDefaultBuffer, 0, D3D11_MAP_READ_WRITE, 0, &mappedResDefault );
	
	if ( FAILED(hr) )
    {
		LogError( __FILEW__, __LINE__, L"Map() failed", hr, ERRORCODE_TYPE_HRESULT, gFCConformanceFailure );
        tr = RESULT_FAIL;
		goto Exit;
    }

	pSrcMem = ( BYTE* )( mappedResStaging.pData );

	// Insert ETW event for timing start
	CEPRINTF( "Test Write Start" );

	QTCT_WITH_ERROR_CHECK( qtct1_start );
	ReadWriteLoop(pSrcMem, m_SizeInBytes, numReps);
	QTCT_WITH_ERROR_CHECK( qtct1_end );

	pSrcMem = ( BYTE* )( mappedResDefault.pData );

	QTCT_WITH_ERROR_CHECK( qtct2_start );
	ReadWriteLoop(pSrcMem, m_SizeInBytes, numReps);
	QTCT_WITH_ERROR_CHECK( qtct2_end );

Exit:
	if ( mappedResStaging.pData )
	{
		m_pEffectiveContext->Unmap( m_pStagingBuffer, 0 );
	}
	if ( mappedResDefault.pData )
	{
		m_pEffectiveContext->Unmap( m_pDefaultBuffer, 0 );
	}

	// Insert ETW event for test end
	CEPRINTF("Test Write End");

	CostStaging = qtct1_end - qtct1_start;
	CostDefault = qtct2_end - qtct2_start;

	return tr;
}

ULONG64 CMapDefaultPerfTest::CostScore( const ULONG64 * pCostPerTest, UINT numTests)
{
	ULONG64 CostAverage = 0;
	std::vector<ULONG64> vScores;
	
	for ( UINT i = 0; i < numTests; i++ )
	{
		vScores.push_back(pCostPerTest[i]);
	}

	std::sort(vScores.begin(), vScores.end());

	for ( UINT i = (numTests/4); i < numTests - (numTests/4); i++ )
	{
		CostAverage += vScores[i];
	}

	CostAverage /= (numTests/2);

	return CostAverage;
}