#include "AppendConsume.h"

void CAppendConsume::InitTestParameters()
{
	CTestCaseParameter<unsigned int>* pStructSizeParam = AddParameter<unsigned int>( _T("StructSize"), &m_StructureSize ); // done
	CTestCaseParameter<unsigned int>* pAppendOffsetParam = AddParameter<unsigned int>( _T("AppendCountOffset"), &m_AppendCountOffset ); // done
	CTestCaseParameter<unsigned int>* pConsumeOffsetParam = AddParameter<unsigned int>( _T("ConsumeCountOffset"), &m_ConsumeCountOffset ); // done
	CTestCaseParameter<unsigned int>* pTotalThreadCountParam = AddParameter<unsigned int>( _T("TotalThreadDispatchCount"), &m_TotalThreadDispatchCount ); // done
	CTestCaseParameter<unsigned int>* pTotalAppendsParam = AddParameter<unsigned int>( _T("TotalAppendCount"), &m_TotalAppendCount ); // done
	CTestCaseParameter<unsigned int>* pUAVSizeOffsetParam = AddParameter<unsigned int>( _T("UAVSizeOffset"), &m_UAVSizeOffset ); // done
	CTestCaseParameter<unsigned int>* pAppendGroupXParam = AddParameter<unsigned int>( _T("AppendGroupX"), m_NumAppendGroups ); // done
	CTestCaseParameter<unsigned int>* pAppendGroupYParam = AddParameter<unsigned int>( _T("AppendGroupY"), m_NumAppendGroups + 1 ); // default
	CTestCaseParameter<unsigned int>* pAppendGroupZParam = AddParameter<unsigned int>( _T("AppendGroupZ"), m_NumAppendGroups + 2 ); // default
	CTestCaseParameter<unsigned int>* pConsumeGroupXParam = AddParameter<unsigned int>( _T("ConsumeGroupX"), m_NumConsumeGroups ); // done
	CTestCaseParameter<unsigned int>* pConsumeGroupYParam = AddParameter<unsigned int>( _T("ConsumeGroupY"), m_NumConsumeGroups + 1 ); // default
	CTestCaseParameter<unsigned int>* pConsumeGroupZParam = AddParameter<unsigned int>( _T("ConsumeGroupZ"), m_NumConsumeGroups + 2 ); // default
	CTestCaseParameter<bool>* pUseHardwareAppendCounter = AddParameter<bool>( _T("UseHardwareAppendCounter"), &m_UseHardwareAppendCounter ); // done

	CTestCaseParameter<unsigned int>* pNumAppendDrawsParam = AddParameter<unsigned int>( _T("NumAppendDraws"), &m_NumAppendDispatches );

	SetParameterDefaultValue<unsigned int>( pAppendOffsetParam, 0u );
	SetParameterDefaultValue<unsigned int>( pConsumeOffsetParam, 0u );
	SetParameterDefaultValue<unsigned int>( pStructSizeParam, 1u );
	SetParameterDefaultValue<unsigned int>( pUAVSizeOffsetParam, 0u );
	SetParameterDefaultValue<unsigned int>( pNumAppendDrawsParam, 1u );
	
	testfactor::RFactor root;

	// group size 1 for non-divisable dispatch counts.
	{
	  testfactor::RFactor groupXIs1 = AddParameterValue<unsigned int>( pAppendGroupXParam, 1u ) * AddParameterValue( pConsumeGroupXParam, 1u );
	  
	  testfactor::RFactor oddDispatchTotal = AddParameterValueSet<unsigned int>( pTotalThreadCountParam, new CTableValueSet<unsigned int>( &(s_NumAppendsTable[3].first), sizeof(std::pair<unsigned int, unsigned int>), 2 ) );
	  testfactor::RFactor oddAppendTotal = AddParameterValueSet<unsigned int>( pTotalAppendsParam, new CTableValueSet<unsigned int>( &(s_NumAppendsTable[3].second), sizeof(std::pair<unsigned int, unsigned int>), 2 ) );

	  root = groupXIs1 * ( oddDispatchTotal % oddAppendTotal );

	}

	{
	  testfactor::RFactor groups = AddParameterValueSet<unsigned int>( pAppendGroupXParam, new CUserValueSet<unsigned int>( 1u, 2u, 16u ) ) 
		% AddParameterValueSet<unsigned int>( pConsumeGroupXParam, new CUserValueSet<unsigned int>( 1u, 2u, 16u ) );

	  testfactor::RFactor dispatchTotal = AddParameterValueSet<unsigned int>( pTotalThreadCountParam, new CTableValueSet<unsigned int>( &(s_NumAppendsTable[0].first), sizeof(std::pair<unsigned int, unsigned int>), 3 ) );
	  testfactor::RFactor appendTotal = AddParameterValueSet<unsigned int>( pTotalAppendsParam, new CTableValueSet<unsigned int>( &(s_NumAppendsTable[0].second), sizeof(std::pair<unsigned int, unsigned int>), 3 ) );

	  root = root + groups * ( dispatchTotal % appendTotal ) * AddParameterValueSet<unsigned int>( pNumAppendDrawsParam, new CUserValueSet<unsigned int>( 1u, 2u ) );
	}

	{
	  testfactor::RFactor appendOffset = AddParameterValueSet<unsigned int>( pAppendOffsetParam, new CUserValueSet<unsigned int>( 0, /*-1,*/ 7, 50, 33 ) ); // TODO Pri-2 use -1 for two appends.
	  testfactor::RFactor consumeOffset = AddParameterValueSet<unsigned int>( pConsumeOffsetParam, new CUserValueSet<unsigned int>( 0, 7, 50, 113 ) );

	  testfactor::RFactor useHardwareCounter = AddParameterValueSet<bool>( pUseHardwareAppendCounter, new CBoolValueSet() );

	  root = root * (appendOffset * useHardwareCounter * consumeOffset); 
	}

	{
	  testfactor::RFactor structSize = AddParameterValueSet<unsigned int>( pStructSizeParam, new CUserValueSet<unsigned int>( 1u, 4u, 37u ) );

	  testfactor::RFactor uavSizeOffset = AddParameterValueSet<unsigned int>( pUAVSizeOffsetParam, new CUserValueSet<unsigned int>( 0u, 5u, 50u, 17u ) );

	  testfactor::RFactor base = AddParameterValue<unsigned int>( pAppendGroupZParam, 1u )
		* AddParameterValue<unsigned int>( pAppendGroupYParam, 1u )
		* AddParameterValue<unsigned int>( pConsumeGroupYParam, 1u )
		* AddParameterValue<unsigned int>( pConsumeGroupZParam, 1u );


	  root = ( root * base * uavSizeOffset ) % structSize ;
	}

	SetRootTestFactor(root);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("TotalThreadDispatchCount"), 11), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("TotalThreadDispatchCount"), 256), WeightFactorVeryLarge);
	AddPriorityPatternFilter( ( FilterEqual<UINT>(_T("UAVSizeOffset"), 0) || FilterEqual<UINT>(_T("UAVSizeOffset"), 5) ), 
								WeightFactorLarge);

	AddPriorityPatternFilter( FilterEqual<UINT>(_T("AppendCountOffset"), 7), WeightFactorSmall);
	AddPriorityPatternFilter( FilterEqual<UINT>(_T("ConsumeCountOffset"), 113), WeightFactorSmall);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CAppendConsume::Setup()
{
	{
		TEST_RESULT tr = CComputeTest::Setup();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}
	
	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		WriteToLog(_T("Skip this group on feature levels less than 11_0."));
		return RESULT_SKIP;
	}

	return RESULT_PASS;
}

TEST_RESULT CAppendConsume::SetupTestCase()
{
	HRESULT hr;

	const unsigned int clearValue[] = { 0x00000000, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD };

	// each of the shaders has its own thread count.
	// There is a number indicating the total number of threads that should be put to work.
	// Make sure that test cases never hit an instances where the execution thread count
	// isn't exactly what we want it to be.  ie. 71, and 11 won't work.
	m_AppendNumGroupThreads[0] = (m_TotalThreadDispatchCount / m_NumAppendGroups[0]) / m_NumAppendDispatches; // parameters ensure that this is not a fractional value.
	m_AppendNumGroupThreads[1] = 1;
	m_AppendNumGroupThreads[2] = 1;

	m_ConsumeNumGroupThreads[0] = m_TotalThreadDispatchCount / m_NumConsumeGroups[0]; // parameters ensure that this is not a fractional value.
	m_ConsumeNumGroupThreads[1] = 1;
	m_ConsumeNumGroupThreads[2] = 1;

	m_RealConsumeGroupCountX = m_NumConsumeGroups[0];
	if( m_AppendCountOffset + m_ConsumeCountOffset )
	{
		m_RealConsumeGroupCountX += (m_AppendCountOffset + m_ConsumeCountOffset + m_ConsumeNumGroupThreads[0] - 1) / m_ConsumeNumGroupThreads[0];
	}
	{
		D3D11_BUFFER_DESC desc =
		{
			m_TotalThreadDispatchCount * sizeof(UINT),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			0
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			s_pAppendCountData,
			0, 
			0
		};

		hr = GetFramework()->CreateBufferTiledOptional( &desc, &data, m_rDynamicAppendCounts );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create buffer(Dynamic Append Counts): %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	{ // create the constant buffers that will be used to tell the algorithm
	  // which dispatch call is being made.
		const D3D11_BUFFER_DESC desc =
		{
			16,
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_CONSTANT_BUFFER,
			0,
			0,
			0
		};

		m_rDispatchIndexCB.clear();
		unsigned int i[4] = { 0u, 0u, 0u, 0u };
		D3D11_SUBRESOURCE_DATA data = 
		{
			i,
			0,
			0
		};

		for( ; i[0] < m_NumAppendDispatches; ++i[0] )
		{
			m_rDispatchIndexCB.push_back( TCOMPtr<ID3D11Buffer*>() );
			hr = GetFramework()->CreateBufferTiledOptional( &desc, &data, m_rDispatchIndexCB[i[0]] );
			if( FAILED(hr) )
			{
				WriteToLog( _T("Failed to create constant buffer %i, hr=%s"), i[0], D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		}
	}
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC desc( m_rDynamicAppendCounts, DXGI_FORMAT_R32_UINT, 0, m_TotalThreadDispatchCount );

		hr = GetDevice()->CreateShaderResourceView( m_rDynamicAppendCounts, &desc, m_rDynamicAppendSRV );

		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create SRV(Dynamic Append Counts): %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	{
		const D3D11_BUFFER_DESC desc =
		{
			m_TotalThreadDispatchCount * sizeof(UINT),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			0
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			s_pConsumeCountData,
			0, 
			0
		};

		hr = GetFramework()->CreateBufferTiledOptional( &desc, &data, m_rDynamicConsumeCounts );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create buffer(Dynamic Consume Counts): %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	{
		// limit the consume view to be just the window of data we're expecting.
		CD3D11_SHADER_RESOURCE_VIEW_DESC desc( m_rDynamicConsumeCounts, DXGI_FORMAT_R32_UINT, 0, m_TotalThreadDispatchCount );

		hr = GetDevice()->CreateShaderResourceView( m_rDynamicConsumeCounts, &desc, m_rDynamicConsumeSRV );

		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create SRV(Dynamic Consume Counts): %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	{ // Create an append/consume buffer
		const D3D11_BUFFER_DESC desc =
		{
			// + 50 gives a margin for consuming more.
			(m_TotalAppendCount + m_UAVSizeOffset) * (m_StructureSize + 3) * sizeof(UINT),
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_UNORDERED_ACCESS,
			0,
			D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
			(m_StructureSize + 3) * sizeof(UINT)
		};

		hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, m_rAppendConsumeBuffer );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create buffer(AppendConsume): %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	{ // create append/consume UAV
		CD3D11_UNORDERED_ACCESS_VIEW_DESC const uavDesc(
				m_rAppendConsumeBuffer,
				DXGI_FORMAT_UNKNOWN,
				0,
				m_TotalAppendCount + m_UAVSizeOffset,
				D3D11_BUFFER_UAV_FLAG_APPEND
				);

		hr = GetDevice()->CreateUnorderedAccessView( m_rAppendConsumeBuffer, &uavDesc, m_rAppConUAV );

		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create UAV(for append and consume): %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

		// Reads from this will just be out of bounds.
		GetEffectiveContext()->ClearUnorderedAccessViewUint( m_rAppConUAV, clearValue );
	
	{ //create a debugging resource
		const D3D11_BUFFER_DESC desc =
		{
			// + 50 gives a margin for consuming more.
			(m_TotalAppendCount + m_UAVSizeOffset) * (m_StructureSize + 3) * sizeof(UINT),
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_READ,
			0,
			0
		};

		hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, m_rDebugBuffer );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create buffer(debug): %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	
	
	// compile the shaders.
	{
		TCOMPtr<ID3D10Blob*> rShader;
		TCOMPtr<ID3D10Blob*> rError;
		TCOMPtr<ID3D10Blob*> rShaderText;
		const tstring struct_size( FormatString( _T("%d"), m_StructureSize ) );
		{ // compile append shader
			const tstring group_count_X( FormatString( _T("%d"), m_NumAppendGroups[0] ) );
			const tstring group_count_Y( FormatString( _T("%d"), m_NumAppendGroups[1] ) );
			const tstring group_count_Z( FormatString( _T("%d"), m_NumAppendGroups[2] ) );
			const tstring thread_count_X( FormatString( _T("%d"), m_AppendNumGroupThreads[0] ) );
			const tstring thread_count_Y( FormatString( _T("%d"), m_AppendNumGroupThreads[1] ) );
			const tstring thread_count_Z( FormatString( _T("%d"), m_AppendNumGroupThreads[2] ) );

			D3D10_SHADER_MACRO const macros[] = 
			{
				{ _T("DEFINES_SET"), _T("1") },
				{ _T("STRUCT_SIZE"), struct_size.c_str() },
				{ _T("THREAD_COUNT_X"), thread_count_X.c_str() },
				{ _T("THREAD_COUNT_Y"), thread_count_Y.c_str() },
				{ _T("THREAD_COUNT_Z"), thread_count_Z.c_str() },
				{ _T("GROUP_COUNT_X"), group_count_X.c_str() },
				{ _T("GROUP_COUNT_Y"), group_count_Y.c_str() },
				{ _T("GROUP_COUNT_Z"), group_count_Z.c_str() }
				//, { _T("USE_DYNAMIC_COUNTS"), (m_TestAppendConsumeCounts == TEST_COUNTS_DYNAMIC ? _T("1") : _T("0")) }
				,NULL
			};

			HRESULT hr = GetFramework()->CompileShaderFromResource( 
					NULL,
					_T("AppendStructured.fx"),
					_T("main"),
					D3D_SHADER_STAGE_COMPUTE,
					D3D_FEATURE_LEVEL_11_0,
					D3D_FEATURE_LEVEL_11_0,
					0,
					rShader,
					rError,
					macros,
					rShaderText );
			if( FAILED(hr) )
			{
				WriteToLog( _T("AppendStructured.fx") );
				if( *rError )
				{
					WriteToLog( _T("Compile Error: %s"), rError->GetBufferPointer() );
				}
				else
				{
					WriteToLog( _T("Failed to compile shader: hr=%s"), D3DHResultToString(hr).c_str() );
				}
				if( *rShaderText )
				{
					WriteMessage( _T("%s"), rShaderText->GetBufferPointer() );
				}
				return RESULT_FAIL;
			}

			hr = GetDevice()->CreateComputeShader( 
				rShader->GetBufferPointer(),
				rShader->GetBufferSize(),
				NULL,
				&m_pCS );
			if( FAILED(hr) )
			{
				WriteToLog( _T("Failed to create Append shader, hr=%s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		}
		{ // compile consume shader
			const tstring group_count_X( FormatString( _T("%d"), m_RealConsumeGroupCountX ) );
			const tstring group_count_Y( FormatString( _T("%d"), m_NumConsumeGroups[1] ) );
			const tstring group_count_Z( FormatString( _T("%d"), m_NumConsumeGroups[2] ) );
			const tstring thread_count_X( FormatString( _T("%d"), m_ConsumeNumGroupThreads[0] ) );
			const tstring thread_count_Y( FormatString( _T("%d"), m_ConsumeNumGroupThreads[1] ) );
			const tstring thread_count_Z( FormatString( _T("%d"), m_ConsumeNumGroupThreads[2] ) );

			D3D10_SHADER_MACRO const macros[] = 
			{
				{ _T("DEFINES_SET"), _T("1") },
				{ _T("STRUCT_SIZE"), struct_size.c_str() },
				{ _T("THREAD_COUNT_X"), thread_count_X.c_str() },
				{ _T("THREAD_COUNT_Y"), thread_count_Y.c_str() },
				{ _T("THREAD_COUNT_Z"), thread_count_Z.c_str() },
				{ _T("GROUP_COUNT_X"), group_count_X.c_str() },
				{ _T("GROUP_COUNT_Y"), group_count_Y.c_str() },
				{ _T("GROUP_COUNT_Z"), group_count_Z.c_str() }
				//, { _T("USE_DYNAMIC_COUNTS"), (m_TestAppendConsumeCounts == TEST_COUNTS_DYNAMIC ? _T("1") : _T("0")) }
				,NULL
			};

			HRESULT hr = GetFramework()->CompileShaderFromResource( 
					NULL,
					_T("ConsumeStructured.fx"),
					_T("main"),
					D3D_SHADER_STAGE_COMPUTE,
					D3D_FEATURE_LEVEL_11_0,
					D3D_FEATURE_LEVEL_11_0,
					0,
					rShader,
					rError,
					macros,
					rShaderText );
			if( FAILED(hr) )
			{
				WriteToLog( _T("ConsumeStructured.fx") );
				if( *rError )
				{
					WriteToLog( _T("Compile Error: %s"), rError->GetBufferPointer() );
				}
				else
				{
					WriteToLog( _T("Failed to compile shader: hr=%s"), D3DHResultToString(hr).c_str() );
				}
				if( *rShaderText )
				{
					WriteMessage( _T("%s"), rShaderText->GetBufferPointer() );
				}
				return RESULT_FAIL;
			}
			hr = GetDevice()->CreateComputeShader( 
				rShader->GetBufferPointer(),
				rShader->GetBufferSize(),
				NULL,
				m_rConsumeShader );
			if( FAILED(hr) )
			{
				WriteToLog( _T("Failed to create Append shader, hr=%s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		}
	}

	{
		// the CopyStructuredCount will use this as the direct destination.
		const D3D11_BUFFER_DESC desc = 
		{
			4,
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_READ,
			0,
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, m_rFilledSizeStaging );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create buffer(filled size staging), hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	{
		const D3D11_BUFFER_DESC desc = 
		{
			16,
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_CONSTANT_BUFFER,
			0,
			0,
			0
		};

		const unsigned int cb[] = 
		{
			m_AppendCountOffset + m_TotalThreadDispatchCount + m_ConsumeCountOffset,
			m_TotalThreadDispatchCount
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			cb,
			0,
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional( &desc, &data, m_rConsumeLimits );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to create buffer(consume limits), hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	// With respect to the append code:
	// - I need to keep track of the actuall number of appends done by the code.
	// - Keep track of the start offset in the append buffer.
	// - Keep track of the number of appends done beyond the end of the view.
	//
	// - The biggest issue is that I can't control which appends will succeed vs which will fail when
	// the view runs out of space.  So, in test where the view will run out of space, we will be 
	// doing verification on the #of requested appends returned by CopyStructuredCount(), and then validate 
	// the number of appends that resulted in no data being written. (provided the consume thread gathers everything).
	// This is done by checking the zero counter.  The rest of the buffer can be compared on general terms
	// with lessequal counts - or by ensuring that the missing appends are accounted for.
	//
	// A valid point is that there is no reason to mix and match consumption permutations with appending permutations.  The 
	// tests can be done in isolation.
	//
	// Having a starting offset in the append code will just reduce the number of appends possible.  This test is not 
	// meant to check that view boundaries are respected, so the consume pass will not attempted to consume from a larger
	// view than was available to the append view. The counter for appending will be analyzed for correctness.  The
	// consume view and the append view can be the same size.
	//
	// Consuming with an offset larger than the view should result in no error, the out of bounds reads should be just that.
	// - verification requires that we check that we greaterthatequal to the number of expected appends in that range.
	// Consuming more items than found in the view should result in zero return data.
	// Only data at the begining of a view with an offset will be preserved.  Data behind the end of an Append will
	// not be preserved.  
	//
	// There are two ways to overflow the view during append.  One is to set and offset that forces the appends
	// to be larger than the space available, the other is to use more executions to push past the limit.  I think
	// that it's not an interesting case because the behavior is the same either way.
	//
	// I do need a test case where the number of appends does not fill the buffer, but the number of conumes does
	// cover the whole space.
	//
	// TODO Pri-2  , Appending to an already appended to UAV, using the -1 offset, should work as expected.

	// make the target a buffer
	m_TargetUAVSize[0] = m_TotalThreadDispatchCount; // this is the most threads ever dispatched.
	m_TargetUAVSize[1] = 1;
	m_TargetUAVSize[2] = 1;
	m_TargetUAVFormat = DXGI_FORMAT_R32_UINT;
	m_TargetUAVDimension = D3D11_UAV_DIMENSION_BUFFER;
	m_TargetUAVMiscFlags = (D3D11_RESOURCE_MISC_FLAG)0;
	m_TargetUAVBufferFlag = (D3D11_BUFFER_UAV_FLAG)0;
	m_TargetUAVArraySize = 1;
	m_TargetUAVElementByteStride = sizeof(UINT);

	{
		TEST_RESULT tr = CComputeTest::SetupTestCase();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}

	{
		const unsigned int zero[] = { 0, 0, 0, 0 };
		GetEffectiveContext()->ClearUnorderedAccessViewUint( m_pTargetUAV, zero );
	}

	return RESULT_PASS;
}

TEST_RESULT CAppendConsume::ExecuteTestCase()
{
	GetEffectiveContext()->CSSetShaderResources( 0, 1, &*m_rDynamicAppendSRV );
	GetEffectiveContext()->CSSetShader( m_pCS, NULL, NULL );
	for( unsigned int i(0); i < m_NumAppendDispatches; ++i )
	{
		if( i == 0 )
		{
			GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &*m_rAppConUAV, &m_AppendCountOffset );
		}
		else
		{
			const UINT32 offset = -1;
			ID3D11UnorderedAccessView* pEmpty(NULL);
			GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &pEmpty, &offset );
			GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &*m_rAppConUAV, &offset );
		}
		GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &*m_rDispatchIndexCB[i] );
		GetEffectiveContext()->Dispatch( m_NumAppendGroups[0], m_NumAppendGroups[1], m_NumAppendGroups[2] );
	}

	GetEffectiveContext()->CopyStructureCount( m_rFilledSizeStaging, 0, m_rAppConUAV );
	GetEffectiveContext()->CopyResource( m_rDebugBuffer, m_rAppendConsumeBuffer );

	GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &m_pTargetUAV, NULL );
	{
		// Must consume all appends or more for results to be valid.
		const unsigned int offset = m_UseHardwareAppendCounter ? -1 : m_TotalAppendCount + max( m_ConsumeCountOffset, m_AppendCountOffset < m_UAVSizeOffset ? m_AppendCountOffset : m_UAVSizeOffset);
		GetEffectiveContext()->CSSetUnorderedAccessViews( 1, 1, &*m_rAppConUAV, &offset );
	}
	GetEffectiveContext()->CSSetShaderResources( 0, 1, &*m_rDynamicConsumeSRV );
	GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &*m_rConsumeLimits );
	GetEffectiveContext()->CSSetShader( m_rConsumeShader, NULL, NULL );
	GetEffectiveContext()->Dispatch( m_RealConsumeGroupCountX, m_NumConsumeGroups[1], m_NumConsumeGroups[2] );

	GetEffectiveContext()->CopyResource( m_pTargetResourceCopy, m_pTargetResource );
	ExecuteEffectiveContext();

	D3D11_MAPPED_SUBRESOURCE mDebug;
	{
		HRESULT hr = GetImmediateContext()->Map( m_rDebugBuffer, 0, D3D11_MAP_READ, 0, &mDebug );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to map debug buffer for reading, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	{ // Verify the correct number of appended elements.
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT hr = GetImmediateContext()->Map( m_rFilledSizeStaging, 0, D3D11_MAP_READ, 0, &mapped );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to map buffer for reading, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
		if( static_cast<unsigned int const*>( mapped.pData )[0] != m_AppendCountOffset + m_TotalAppendCount )
		{
			WriteToLog( _T("The total number of appends was incorrect. Expected: %u, Found: %u"), m_AppendCountOffset + m_TotalAppendCount, static_cast<unsigned int const*>( mapped.pData )[0] );
			GetImmediateContext()->Unmap( m_rFilledSizeStaging, 0 );
			return RESULT_FAIL;
		}
		GetImmediateContext()->Unmap( m_rFilledSizeStaging, 0 );
	}

	// The following things affect how many consumes there were.
	// total # of threads released.
	// Dynamic or 1 append only
	// AppendInitOffset

	{ // Map and read m_pTargetResourceCopy for results.
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT hr = GetImmediateContext()->Map( m_pTargetResourceCopy, 0, D3D11_MAP_READ, 0, &mapped );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Map(target copy) failed, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		const unsigned int consumeGoal = m_AppendCountOffset + m_TotalAppendCount + m_ConsumeCountOffset;
		const unsigned int consumesPossible = m_TotalAppendCount + m_UAVSizeOffset; 

		const unsigned int numGarbageLocations = m_UAVSizeOffset > m_AppendCountOffset ? m_UAVSizeOffset - m_AppendCountOffset : 0;
		// Reading garbage means that greater-than-equal-to must be used for verification.
		// Since we'll have at least data for the appends, but the garbage will be random.
		const unsigned int consumedGarbageCount = m_ConsumeCountOffset < numGarbageLocations ? m_ConsumeCountOffset : numGarbageLocations;

		const unsigned int validDataConsumesPossible = consumesPossible - numGarbageLocations;

		const unsigned int overConsumptionCount = consumeGoal > consumesPossible ? consumeGoal - consumesPossible : 0;

		const bool appendedOutOfBounds = m_AppendCountOffset > m_UAVSizeOffset;

		const unsigned int appendedOutOfBoundsCount = m_AppendCountOffset < consumesPossible ? m_AppendCountOffset > m_UAVSizeOffset ? m_AppendCountOffset - m_UAVSizeOffset : 0 : m_TotalAppendCount;

		const unsigned int numExpectedZeros( min( m_AppendCountOffset, consumesPossible ) + overConsumptionCount );
	
		// size differences between append/consume, as well as errors will be 
		// written to the zero location of the result UAV. account for this during the check.
		// Count the number of out of bounds reads.
		// Count the number of empty append offset reads.
		if( consumedGarbageCount )
		{
			if( *static_cast<unsigned int const*>(mapped.pData) < numExpectedZeros )
			{
				if( m_AppendCountOffset && overConsumptionCount )
				{
					WriteToLog( _T("Expected more zero consumes due to a starting append offset of %u and an over consumption of %u. Actual Total: %u"), 
							m_AppendCountOffset, overConsumptionCount, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else if( m_AppendCountOffset )
				{
					WriteToLog( _T("Expected more zero consumes due to a starting append offset of %u. Actual Total: %u"), 
							m_AppendCountOffset, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else if( overConsumptionCount )
				{

					WriteToLog( _T("Expected more zero consumes due to an over consumption of %u.  Actual Total: %u"), 
							overConsumptionCount, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else
				{
					assert( false );
				}

				GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0u );
				return RESULT_FAIL;
			}
		}
		else
		{
			// we always consume enough data to cover the m_AppendCountOffset.
			if( *static_cast<unsigned int const*>(mapped.pData) != numExpectedZeros )
			{
				if( m_AppendCountOffset && overConsumptionCount )
				{
					WriteToLog( _T("Expected zero consumes due to a starting append offset of %u and an over consumption of %u. Actual Total: %u"), 
							m_AppendCountOffset, overConsumptionCount, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else if( m_AppendCountOffset )
				{
					WriteToLog( _T("Expected zero consumes due to a starting append offset of %u. Actual Total: %u"), 
							m_AppendCountOffset, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else if( overConsumptionCount )
				{

					WriteToLog( _T("Expected zero consumes due to an over consumption of %u.  Actual Total: %u"), 
							overConsumptionCount, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else
				{
					assert( false );
				}

				GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
				return RESULT_FAIL;
			}
		}

		unsigned int missingAppendCount(0);  // in the end this should be equal to the number of out of bounds appends.

		// now scan the consumed data.
		for( unsigned int i(1); i < m_TotalThreadDispatchCount; ++i )
		{
			if( consumedGarbageCount )
			{
				assert( appendedOutOfBounds == false );
				// we consumed all appends, and then some, so compare >= to expected.
				if( s_pAppendCountData[i] > *(static_cast<unsigned int const*>(mapped.pData) + i) )
				{
					WriteToLog( _T("Expected at least %u Appends by thread index %u, Found: %u"),
							s_pAppendCountData[i], i, *(static_cast<unsigned int const*>(mapped.pData) + i) );
					GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
					return RESULT_FAIL;
				}

			}
			else if( appendedOutOfBounds )
			{
				// we won't be able to find all the data we wanted.  compare <=
				if( s_pAppendCountData[i] < *(static_cast<unsigned int const*>(mapped.pData) + i) )
				{
					WriteToLog( _T("Expected no more than %u Appends by thread index %u, Found: %u"),
							s_pAppendCountData[i], i, *(static_cast<unsigned int const*>(mapped.pData) + i) );
					GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
					return RESULT_FAIL;
				}
				else if( s_pAppendCountData[i] != *(static_cast<unsigned int const*>(mapped.pData) + i) )
				{
					missingAppendCount += s_pAppendCountData[i] - *(static_cast<unsigned int const*>(mapped.pData) + i);
				}
			}
			else
			{
				if( s_pAppendCountData[i] != *(static_cast<unsigned int const*>(mapped.pData) + i) )
				{
					WriteToLog( _T("Expected %u Appends by thread index %u, Found: %u"),
							s_pAppendCountData[i], i, *(static_cast<unsigned int const*>(mapped.pData) + i) );
					GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
					return RESULT_FAIL;
				}
			}
		}

		GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );

		if( appendedOutOfBounds )
		{
			if( missingAppendCount != appendedOutOfBoundsCount )
			{
				WriteToLog( _T("Expected %u missing Appends, Found: %u"), appendedOutOfBoundsCount, missingAppendCount );
				return RESULT_FAIL;
			}
		}
	}

	GetImmediateContext()->Unmap( m_rDebugBuffer, 0 );

	return RESULT_PASS;
}

void CAppendConsume::CleanupTestCase()
{
	SAFE_RELEASE(m_pCS);
	m_rDynamicAppendCounts.Release();
	m_rDynamicConsumeCounts.Release();
	m_rAppendConsumeBuffer.Release();
	m_rDynamicAppendSRV.Release();
	m_rDynamicConsumeSRV.Release();
	m_rAppConUAV.Release();
	m_rConsumeShader.Release();
	m_rFilledSizeStaging.Release();
	m_rConsumeLimits.Release();
	m_rDebugBuffer.Release();
	m_rDispatchIndexCB.clear();
	CComputeTest::CleanupTestCase();
}

void CAppendConsume::Cleanup()
{
	CComputeTest::Cleanup();
}

const std::pair<unsigned int, unsigned int> CAppendConsumeDynamicData::s_NumAppendsTable[] = 
{
	std::make_pair( 1024, 4673 ),
	std::make_pair( 512, 2370 ),
	std::make_pair( 256, 1158 ),
	std::make_pair( 11, 47 ),
	std::make_pair( 71, 339 )
};

// There should be no appends with a combined GroupThreadIndex of 0.  The 
// Zero index will use used to detect consumes outside of the RWBuffer size.
const unsigned int CAppendConsumeDynamicData::s_pAppendCountData[] =
{
	0, 9, 0, 1, 3, 7, 3, 7, 6, 9, 2, 8, 6, 7, 7, 6, 4, 2, 3, 1, 8, 9, 0, 3, 3, 8, 5, 8, 7, 5, 5, 9,
	9, 8, 7, 3, 8, 4, 1, 3, 1, 4, 4, 3, 4, 8, 0, 4, 6, 2, 0, 9, 4, 9, 9, 0, 2, 6, 5, 2, 3, 0, 5, 0,
	4, 9, 7, 9, 9, 5, 2, 7, 3, 0, 2, 2, 7, 1, 9, 0, 2, 6, 5, 8, 0, 0, 6, 4, 8, 0, 2, 2, 7, 5, 2, 5,
	1, 6, 0, 0, 4, 2, 8, 6, 2, 5, 9, 9, 3, 7, 7, 8, 0, 6, 3, 2, 4, 8, 0, 0, 1, 3, 3, 9, 3, 0, 3, 2,
	9, 2, 5, 9, 6, 6, 0, 6, 0, 9, 3, 1, 8, 8, 2, 4, 1, 5, 9, 5, 7, 5, 0, 7, 9, 7, 7, 6, 9, 7, 8, 7,
	0, 4, 9, 0, 8, 0, 0, 4, 2, 8, 1, 1, 7, 8, 2, 8, 3, 2, 8, 5, 9, 9, 5, 3, 9, 0, 0, 9, 6, 1, 4, 2,
	1, 7, 3, 2, 0, 5, 5, 5, 1, 6, 8, 6, 0, 9, 7, 6, 4, 7, 0, 9, 7, 3, 8, 1, 8, 8, 8, 7, 0, 8, 3, 1,
	1, 0, 4, 8, 1, 9, 6, 3, 9, 3, 3, 6, 8, 4, 1, 0, 4, 0, 8, 1, 3, 5, 9, 9, 6, 3, 0, 4, 3, 6, 8, 0,
	1, 6, 7, 0, 1, 8, 8, 5, 9, 6, 7, 7, 6, 6, 9, 2, 7, 4, 1, 5, 4, 7, 7, 4, 0, 3, 6, 3, 9, 5, 7, 9,
	8, 8, 9, 4, 2, 1, 0, 6, 5, 0, 7, 5, 2, 7, 0, 2, 4, 5, 2, 2, 7, 8, 3, 7, 8, 8, 9, 3, 2, 8, 8, 8,
	0, 9, 2, 1, 8, 0, 8, 4, 5, 9, 2, 8, 6, 5, 8, 4, 6, 9, 8, 1, 4, 4, 2, 4, 8, 8, 4, 2, 0, 6, 9, 3,
	0, 0, 9, 4, 3, 1, 3, 7, 7, 7, 0, 6, 2, 2, 8, 4, 7, 5, 6, 6, 3, 4, 3, 0, 2, 5, 2, 7, 7, 1, 7, 3,
	2, 5, 7, 0, 5, 5, 3, 8, 1, 3, 4, 9, 0, 3, 0, 8, 8, 7, 3, 5, 9, 1, 1, 4, 4, 7, 2, 0, 5, 7, 6, 7,
	6, 2, 1, 2, 5, 9, 9, 8, 7, 9, 0, 7, 1, 1, 2, 7, 1, 3, 9, 5, 4, 6, 9, 2, 1, 6, 3, 4, 4, 4, 2, 0,
	2, 8, 6, 2, 7, 9, 3, 3, 9, 3, 4, 6, 3, 6, 3, 0, 8, 0, 3, 2, 5, 5, 4, 6, 5, 4, 7, 6, 7, 5, 0, 4,
	6, 1, 1, 8, 8, 0, 6, 4, 4, 3, 7, 9, 4, 8, 6, 7, 9, 5, 6, 6, 0, 9, 9, 8, 5, 2, 9, 7, 6, 6, 2, 3,
	6, 9, 3, 1, 9, 5, 1, 4, 4, 2, 4, 9, 3, 9, 7, 9, 8, 0, 3, 9, 6, 1, 4, 5, 2, 3, 5, 7, 9, 7, 9, 4,
	4, 9, 2, 4, 2, 5, 3, 6, 9, 9, 3, 7, 2, 8, 9, 5, 3, 1, 5, 8, 5, 5, 2, 3, 6, 6, 2, 2, 2, 3, 0, 8,
	4, 1, 3, 9, 7, 2, 3, 0, 7, 2, 4, 3, 7, 7, 8, 9, 9, 6, 3, 2, 7, 7, 4, 6, 9, 7, 6, 0, 3, 1, 1, 6,
	2, 2, 2, 9, 0, 5, 7, 2, 0, 9, 0, 3, 4, 0, 2, 8, 6, 5, 8, 1, 8, 8, 5, 9, 5, 0, 0, 1, 2, 1, 6, 0,
	3, 4, 7, 1, 3, 2, 2, 2, 8, 8, 7, 2, 8, 6, 4, 0, 1, 7, 3, 9, 3, 8, 0, 0, 0, 2, 7, 1, 3, 1, 6, 4,
	9, 5, 7, 9, 3, 9, 3, 0, 7, 6, 4, 3, 1, 0, 5, 6, 1, 1, 9, 8, 6, 5, 0, 6, 3, 3, 7, 5, 7, 4, 5, 6,
	8, 4, 9, 3, 4, 1, 5, 8, 4, 0, 3, 7, 5, 4, 2, 5, 4, 7, 2, 9, 8, 0, 6, 2, 9, 3, 5, 2, 0, 8, 4, 8,
	0, 3, 0, 2, 9, 0, 1, 9, 2, 5, 6, 8, 7, 0, 6, 9, 0, 7, 1, 0, 6, 6, 2, 2, 0, 9, 7, 3, 8, 5, 2, 8,
	1, 2, 9, 4, 6, 4, 4, 6, 3, 4, 5, 6, 3, 2, 3, 3, 2, 0, 9, 5, 4, 9, 1, 9, 9, 1, 6, 4, 9, 5, 0, 8,
	5, 8, 0, 9, 2, 0, 5, 4, 4, 0, 2, 5, 5, 6, 7, 6, 9, 8, 3, 5, 6, 7, 0, 8, 8, 8, 8, 3, 3, 1, 7, 4,
	6, 8, 3, 3, 3, 6, 5, 9, 1, 0, 2, 5, 1, 0, 9, 6, 9, 1, 4, 5, 2, 4, 0, 0, 3, 4, 0, 1, 0, 2, 0, 6,
	7, 1, 2, 3, 9, 4, 5, 9, 2, 9, 6, 4, 3, 6, 9, 4, 7, 2, 9, 5, 0, 0, 2, 5, 0, 3, 1, 9, 4, 4, 4, 9,
	2, 6, 6, 1, 7, 3, 3, 3, 5, 2, 5, 1, 4, 2, 4, 6, 3, 1, 5, 8, 9, 0, 9, 7, 7, 2, 2, 4, 7, 5, 6, 6,
	5, 0, 7, 8, 5, 1, 6, 6, 7, 5, 9, 3, 9, 0, 5, 3, 1, 2, 0, 6, 8, 1, 0, 9, 4, 7, 2, 6, 7, 1, 2, 6,
	6, 2, 9, 1, 0, 9, 3, 5, 4, 4, 7, 9, 6, 6, 3, 8, 7, 6, 5, 8, 5, 3, 5, 6, 2, 3, 5, 7, 3, 1, 3, 1,
	3, 3, 6, 9, 9, 9, 7, 0, 7, 4, 8, 1, 2, 5, 9, 1, 1, 8, 7, 1, 7, 2, 9, 3, 3, 8, 8, 9, 9, 3, 3, 3
};

// this data is a shuffling of the s_pAppendCountData data, but the shuffle is done in sets based on
// the size of the first value in the s_NumAppendsTable
const unsigned int CAppendConsumeDynamicData::s_pConsumeCountData[] =
{
	3, 7, 3, 1, 9, 2, 7, 9, 6, 0, 0, 5, 4, 3, 7, 3, 0, 9, 4, 9, 4, 8, 7, 6, 0, 8, 5, 9, 3, 4, 6, 3,
	0, 1, 5, 5, 9, 8, 9, 8, 3, 2, 1, 3, 5, 9, 2, 7, 6, 9, 8, 7, 8, 7, 1, 2, 2, 0, 9, 5, 0, 9, 4, 4,
	6, 8, 4, 0, 2, 4, 3, 0, 4, 7, 0, 5, 9, 6, 3, 0, 5, 1, 1, 9, 9, 7, 3, 0, 8, 4, 8, 0, 3, 9, 8, 1,
	2, 0, 0, 0, 4, 7, 7, 4, 0, 1, 5, 0, 6, 2, 9, 9, 6, 8, 9, 5, 0, 0, 5, 8, 4, 9, 6, 7, 9, 0, 3, 7,
	6, 0, 4, 2, 0, 8, 4, 1, 2, 6, 3, 7, 5, 5, 2, 8, 3, 8, 4, 1, 1, 6, 8, 0, 3, 8, 6, 9, 8, 6, 0, 0,
	8, 6, 1, 3, 0, 9, 0, 6, 6, 0, 0, 2, 3, 3, 1, 5, 5, 8, 3, 0, 5, 8, 3, 8, 8, 6, 8, 7, 0, 2, 2, 0,
	2, 8, 9, 1, 3, 8, 7, 6, 7, 0, 1, 7, 0, 9, 9, 4, 2, 5, 7, 9, 2, 3, 0, 9, 7, 0, 5, 1, 2, 1, 3, 8,
	6, 4, 7, 1, 2, 2, 6, 2, 7, 3, 3, 9, 6, 1, 7, 9, 9, 8, 4, 7, 4, 5, 3, 1, 9, 9, 5, 2, 8, 3, 2, 8,
	1, 2, 8, 7, 6, 0, 9, 7, 6, 8, 3, 9, 0, 3, 9, 4, 8, 4, 2, 0, 6, 7, 8, 3, 0, 1, 3, 7, 5, 8, 8, 5,
	9, 3, 7, 9, 5, 5, 6, 3, 1, 2, 6, 3, 5, 4, 7, 5, 4, 2, 5, 2, 5, 6, 1, 7, 4, 2, 8, 7, 0, 2, 7, 8,
	7, 6, 5, 2, 4, 1, 2, 4, 8, 8, 2, 3, 3, 0, 8, 8, 1, 5, 6, 9, 7, 6, 9, 7, 3, 2, 6, 5, 4, 0, 7, 6,
	9, 2, 8, 5, 8, 8, 1, 7, 7, 7, 7, 4, 2, 4, 8, 3, 9, 3, 6, 3, 3, 3, 0, 6, 1, 8, 9, 0, 0, 7, 6, 9,
	8, 1, 9, 0, 8, 5, 2, 0, 5, 9, 2, 6, 4, 5, 3, 7, 4, 0, 4, 4, 9, 4, 9, 7, 8, 4, 7, 9, 3, 1, 0, 7,
	7, 8, 9, 4, 1, 0, 6, 2, 6, 4, 6, 0, 2, 3, 0, 4, 1, 6, 8, 5, 2, 9, 6, 0, 2, 3, 3, 6, 9, 8, 4, 2,
	3, 9, 1, 4, 5, 0, 4, 7, 7, 1, 6, 5, 6, 7, 6, 3, 8, 2, 2, 4, 0, 8, 9, 9, 8, 7, 4, 5, 7, 5, 6, 1,
	1, 4, 1, 2, 7, 5, 2, 2, 6, 8, 5, 4, 6, 1, 0, 8, 9, 3, 6, 7, 0, 7, 9, 4, 2, 3, 9, 0, 2, 4, 5, 7,
	5, 3, 1, 0, 7, 9, 7, 9, 3, 8, 5, 3, 8, 8, 6, 5, 0, 5, 7, 3, 8, 0, 9, 0, 9, 0, 9, 5, 9, 5, 6, 6,
	1, 2, 3, 3, 0, 1, 0, 7, 0, 6, 1, 5, 4, 9, 2, 3, 3, 5, 9, 6, 5, 8, 2, 7, 0, 1, 9, 4, 9, 8, 8, 7,
	4, 9, 1, 2, 1, 3, 9, 9, 4, 0, 9, 2, 7, 6, 3, 0, 6, 7, 0, 2, 0, 4, 2, 9, 1, 8, 3, 4, 5, 9, 7, 7,
	3, 1, 1, 7, 7, 4, 3, 9, 0, 5, 6, 2, 3, 3, 3, 1, 2, 1, 3, 7, 7, 5, 9, 9, 4, 6, 4, 9, 7, 2, 2, 8,
	9, 1, 0, 3, 1, 9, 7, 9, 5, 2, 6, 1, 5, 9, 4, 7, 4, 3, 6, 5, 6, 9, 4, 6, 3, 0, 1, 4, 3, 3, 5, 5,
	3, 3, 9, 4, 0, 3, 4, 8, 4, 8, 6, 5, 7, 6, 2, 3, 0, 4, 3, 6, 5, 1, 6, 8, 3, 1, 2, 0, 9, 6, 8, 5,
	9, 7, 6, 5, 7, 9, 9, 5, 8, 0, 1, 0, 7, 3, 6, 4, 9, 4, 1, 7, 3, 2, 9, 4, 2, 6, 7, 5, 1, 8, 2, 6,
	4, 2, 0, 3, 1, 6, 4, 2, 9, 7, 4, 9, 9, 4, 6, 4, 8, 2, 4, 6, 7, 2, 3, 8, 1, 8, 9, 9, 6, 1, 1, 9,
	3, 0, 9, 9, 5, 9, 3, 6, 3, 4, 2, 7, 1, 3, 1, 4, 9, 6, 0, 0, 3, 7, 4, 2, 0, 5, 3, 6, 5, 5, 3, 4,
	2, 0, 5, 6, 3, 4, 0, 1, 7, 6, 3, 5, 2, 5, 5, 4, 0, 0, 7, 5, 2, 9, 1, 0, 1, 5, 2, 4, 2, 6, 2, 6,
	1, 0, 8, 3, 8, 6, 8, 9, 8, 8, 3, 0, 4, 4, 9, 5, 1, 5, 8, 4, 9, 5, 0, 7, 3, 1, 5, 9, 7, 3, 4, 0,
	5, 1, 2, 3, 3, 1, 5, 6, 6, 3, 2, 7, 7, 6, 0, 3, 8, 2, 2, 3, 0, 5, 7, 6, 0, 6, 4, 0, 4, 8, 6, 8,
	0, 2, 2, 7, 2, 8, 2, 9, 2, 2, 6, 1, 2, 0, 9, 9, 6, 0, 2, 5, 9, 5, 8, 9, 2, 7, 4, 0, 9, 2, 3, 8,
	3, 3, 2, 9, 5, 8, 2, 9, 5, 3, 2, 6, 9, 0, 9, 2, 4, 8, 7, 8, 1, 9, 1, 4, 1, 3, 7, 2, 7, 4, 7, 2,
	0, 7, 7, 7, 5, 1, 9, 5, 1, 8, 5, 4, 3, 1, 3, 6, 2, 2, 9, 6, 9, 6, 4, 2, 3, 5, 8, 1, 6, 3, 3, 8,
	6, 8, 9, 7, 0, 8, 2, 9, 3, 0, 5, 9, 0, 2, 6, 5, 0, 4, 8, 1, 5, 2, 0, 0, 6, 4, 3, 2, 6, 6, 3, 7
};

//----------------------------------------------------------------------------------------

const CAppendConsumePS::Vertex CAppendConsumePS::s_pVertexData[] = 
{
	{ -1.0f, -1.0f, 0.5f },
	{ -1.0f, 1.0f, 0.5f },
	{ 1.0f, -1.0f, 0.5f },
	{ 1.0f, 1.0f, 0.5f }
};

const unsigned int CAppendConsumePS::s_pIndexBufferData[] = 
{
	0, 1, 2, 3
};

#define IFR(x) if( FAILED(x) ) { WriteToLog( _T("%s:%d , %s"), __FILE__, __LINE__, D3DHResultToString(x).c_str() ); return RESULT_FAIL; }
#define IFRM(x,m) if( FAILED(x) ) { WriteToLog( _T("%s:%d , %s, %s"), __FILE__, __LINE__, m, D3DHResultToString(x).c_str() ); return RESULT_FAIL; }

void CAppendConsumePS::InitTestParameters()
{
	CTestCaseParameter<unsigned int>* pStructSizeParam = AddParameter<unsigned int>( _T("StructSize"), &m_StructureSize ); // done
	CTestCaseParameter<unsigned int>* pAppendOffsetParam = AddParameter<unsigned int>( _T("AppendCountOffset"), &m_AppendCountOffset ); // done
	CTestCaseParameter<unsigned int>* pConsumeOffsetParam = AddParameter<unsigned int>( _T("ConsumeCountOffset"), &m_ConsumeCountOffset ); // done
	CTestCaseParameter<unsigned int>* pTotalPixelCountParam = AddParameter<unsigned int>( _T("TotalPixelCount"), &m_TotalPixelDispatchCount ); // done
	CTestCaseParameter<unsigned int>* pTotalAppendsParam = AddParameter<unsigned int>( _T("TotalAppendCount"), &m_TotalAppendCount ); // done
	CTestCaseParameter<unsigned int>* pUAVSizeOffsetParam = AddParameter<unsigned int>( _T("UAVSizeOffset"), &m_UAVSizeOffset ); // done
	CTestCaseParameter<bool>* pUseHardwareAppendCounter = AddParameter<bool>( _T("UseHardwareAppendCounter"), &m_UseHardwareAppendCounter ); // done
	CTestCaseParameter<float>* pViewportParam = AddParameter<float>( _T("AppendViewportWidth"), &m_AppendViewportWidth );

	SetParameterDefaultValue<unsigned int>( pAppendOffsetParam, 0u );
	SetParameterDefaultValue<unsigned int>( pConsumeOffsetParam, 0u );
	SetParameterDefaultValue<unsigned int>( pStructSizeParam, 1u );
	SetParameterDefaultValue<unsigned int>( pUAVSizeOffsetParam, 0u );
	SetParameterDefaultValue<float>( pViewportParam, -1.0f );

	testfactor::RFactor root;
	
	// group size 1 for non-divisable dispatch counts.
	{
		testfactor::RFactor vpWidth = AddParameterValue<float>( pViewportParam, -1.0f );
		testfactor::RFactor oddDispatchTotal = AddParameterValueSet<unsigned int>( pTotalPixelCountParam, new CTableValueSet<unsigned int>( &(s_NumAppendsTable[3].first), sizeof(std::pair<unsigned int, unsigned int>), 2 ) );
		testfactor::RFactor oddAppendTotal = AddParameterValueSet<unsigned int>( pTotalAppendsParam, new CTableValueSet<unsigned int>( &(s_NumAppendsTable[3].second), sizeof(std::pair<unsigned int, unsigned int>), 2 ) );

		root = (oddDispatchTotal % oddAppendTotal) * vpWidth;
	}

	{
		testfactor::RFactor vpWidth = AddParameterValueSet<float>( pViewportParam, new CUserValueSet<float>( 16.0f, 128.0f ) );
		testfactor::RFactor dispatchTotal = AddParameterValueSet<unsigned int>( pTotalPixelCountParam, new CTableValueSet<unsigned int>( &(s_NumAppendsTable[0].first), sizeof(std::pair<unsigned int, unsigned int>), 3 ) );
		testfactor::RFactor appendTotal = AddParameterValueSet<unsigned int>( pTotalAppendsParam, new CTableValueSet<unsigned int>( &(s_NumAppendsTable[0].second), sizeof(std::pair<unsigned int, unsigned int>), 3 ) );

		root = root + ( dispatchTotal % appendTotal ) * vpWidth;
	}

	root = root * AddParameter( _T("ConsumeViewportWidth"), &m_ConsumeViewportWidth, new CUserValueSet<float>( 16u, 121u ) );

	{
		testfactor::RFactor appendOffset = AddParameterValueSet<unsigned int>( pAppendOffsetParam, new CUserValueSet<unsigned int>( 0, /*-1,*/ 7, 50, 33 ) ); // TODO Pri-2 use -1 for two appends.
		testfactor::RFactor consumeOffset = AddParameterValueSet<unsigned int>( pConsumeOffsetParam, new CUserValueSet<unsigned int>( 0, 7, 50, 113 ) );

		testfactor::RFactor useHardwareCounter = AddParameterValueSet<bool>( pUseHardwareAppendCounter, new CBoolValueSet() );

		root = root * (appendOffset * useHardwareCounter * consumeOffset); 
	}

	{
		testfactor::RFactor structSize = AddParameterValueSet<unsigned int>( pStructSizeParam, new CUserValueSet<unsigned int>( 1u, 4u, 37u ) );

		testfactor::RFactor uavSizeOffset = AddParameterValueSet<unsigned int>( pUAVSizeOffsetParam, new CUserValueSet<unsigned int>( 0u, 5u, 50u, 17u ) );

		root = ( root * uavSizeOffset ) % structSize ;
	}

	SetRootTestFactor(root);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("TotalPixelCount"), 11), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("TotalPixelCount"), 256), WeightFactorVeryLarge);
	AddPriorityPatternFilter( ( FilterEqual<UINT>(_T("UAVSizeOffset"), 0) || FilterEqual<UINT>(_T("UAVSizeOffset"), 5) ), 
								WeightFactorLarge);

	AddPriorityPatternFilter( FilterEqual<UINT>(_T("AppendCountOffset"), 7), WeightFactorSmall);
	AddPriorityPatternFilter( FilterEqual<UINT>(_T("ConsumeCountOffset"), 113), WeightFactorSmall);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

TEST_RESULT CAppendConsumePS::Setup()
{
	{
		TEST_RESULT tr = CComputeTest::Setup();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}

	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		WriteToLog(_T("Skip this group on feature levels less than 11_0."));
		return RESULT_SKIP;
	}

	return RESULT_PASS;
}

TEST_RESULT CAppendConsumePS::SetupTestCase()
{
	// decide how big the viewport height must be.
	if( m_AppendViewportWidth == -1.0f )
	{
		m_AppendViewportWidth = static_cast<float>(m_TotalPixelDispatchCount);
	}
	m_AppendViewportHeight = static_cast<float>( m_TotalPixelDispatchCount / static_cast<unsigned int>(m_AppendViewportWidth));

	m_ConsumeViewportHeight = static_cast<float>(m_TotalPixelDispatchCount) / m_ConsumeViewportWidth;
	m_ConsumeViewportHeight = ceil(m_ConsumeViewportHeight);

	if( m_AppendCountOffset + m_ConsumeCountOffset )
	{
		m_ConsumeViewportHeight += (m_AppendCountOffset + m_ConsumeCountOffset) / static_cast<unsigned int>(m_ConsumeViewportWidth);
		if( (m_AppendCountOffset + m_ConsumeCountOffset) % static_cast<unsigned int>(m_ConsumeViewportWidth) )
		{
			m_ConsumeViewportHeight += 1.0f; 
		}
	}

	{ // compile vertex shader
		TCOMPtr<ID3D10Blob*> rError;
		HRESULT hr = GetFramework()->CompileShaderFromResource(
			NULL,
			_T("ACVertexShaderPassThrough.fx"),
			_T("main"),
			D3D_SHADER_STAGE_VERTEX,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_0,
			0,
			m_rVertexByteCode,
			rError,
			NULL,
			NULL );
		if( FAILED(hr) )
		{
			if( *rError )
			{
				WriteToLog( _T("%s, hr=%s"), rError->GetBufferPointer(), D3DHResultToString(hr).c_str() );
			}
			else
			{
				WriteToLog( _T("hr=%s"), D3DHResultToString(hr).c_str() );
			}
			return RESULT_FAIL;
		}

		hr = GetDevice()->CreateVertexShader(
			m_rVertexByteCode->GetBufferPointer(),
			m_rVertexByteCode->GetBufferSize(),
			NULL,
			m_rVertexShader );
		IFRM(hr,_T("ACVertexShaderPassThrough.fx"));
	}
	{ // input layout
		const D3D11_INPUT_ELEMENT_DESC desc = 
		{
			"SV_Position",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		};

		HRESULT hr = GetDevice()->CreateInputLayout( 
			&desc,
			1,
			m_rVertexByteCode->GetBufferPointer(),
			m_rVertexByteCode->GetBufferSize(),
			m_rInputLayout );
		IFR(hr);
	}
	{ // vertex buffer
		const D3D11_BUFFER_DESC desc = 
		{
			4 * sizeof(Vertex),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_VERTEX_BUFFER,
			0,
			0,
			0
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			s_pVertexData,
			0,
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional(
			&desc,
			&data,
			m_rVertexBuffer );
		IFR(hr);
	}
	{ // index buffer
		const D3D11_BUFFER_DESC desc = 
		{
			4 * sizeof(unsigned int),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_INDEX_BUFFER,
			0,
			0,
			0
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			s_pIndexBufferData,
			0,
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional(
			&desc,
			&data,
			m_rIndexBuffer );
		IFR(hr);
	}
	{ // buffer full of the dynamic append count data
		const D3D11_BUFFER_DESC desc =
		{
			m_TotalPixelDispatchCount * sizeof(UINT),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			0
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			s_pAppendCountData,
			0, 
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional( &desc, &data, m_rDynamicAppendCounts );
		IFR(hr);
	}
	{ // create the SRV for the dynamic append data
		const CD3D11_SHADER_RESOURCE_VIEW_DESC desc( m_rDynamicAppendCounts, DXGI_FORMAT_R32_UINT, 0, m_TotalPixelDispatchCount );

		HRESULT hr = GetDevice()->CreateShaderResourceView( m_rDynamicAppendCounts, &desc, m_rDynamicAppendSRV );
		IFR(hr);
	}
	{ // buffer for the dynamic consume counts
		const D3D11_BUFFER_DESC desc =
		{
			m_TotalPixelDispatchCount * sizeof(UINT),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			0
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			s_pConsumeCountData,
			0, 
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional( &desc, &data, m_rDynamicConsumeCounts );
		IFR(hr);
	}
	{ // SRV for the dynamic append data
		// limit the consume view to be just the window of data we're expecting.
		const CD3D11_SHADER_RESOURCE_VIEW_DESC desc( m_rDynamicConsumeCounts, DXGI_FORMAT_R32_UINT, 0, m_TotalPixelDispatchCount );

		HRESULT hr = GetDevice()->CreateShaderResourceView( m_rDynamicConsumeCounts, &desc, m_rDynamicConsumeSRV );
		IFR(hr);
	}
	{ // Create an append/consume buffer
		const D3D11_BUFFER_DESC desc =
		{
			// + 50 gives a margin for consuming more.
			(m_TotalAppendCount + m_UAVSizeOffset) * (m_StructureSize + 3) * sizeof(UINT),
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_UNORDERED_ACCESS,
			0,
			D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
			(m_StructureSize + 3) * sizeof(UINT)
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, m_rAppendConsumeBuffer );
		IFR(hr);
	}
	{ // create append/consume UAV
		const CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc(
				m_rAppendConsumeBuffer,
				DXGI_FORMAT_UNKNOWN,
				0,
				m_TotalAppendCount + m_UAVSizeOffset,
				D3D11_BUFFER_UAV_FLAG_APPEND
				);

		HRESULT hr = GetDevice()->CreateUnorderedAccessView( m_rAppendConsumeBuffer, &uavDesc, m_rAppConUAV );
		IFR(hr);
	}
	{ //create a debugging resource
		const D3D11_BUFFER_DESC desc =
		{
			// + 50 gives a margin for consuming more.
			(m_TotalAppendCount + m_UAVSizeOffset) * (m_StructureSize + 3) * sizeof(UINT),
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_READ,
			0,
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, m_rDebugBuffer );
		IFR(hr);
	}
	{ // clear the append/consume UAV
		const unsigned int clearValue[] = { 0x00000000, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD };
		// Reads from this will just be out of bounds.
		GetEffectiveContext()->ClearUnorderedAccessViewUint( m_rAppConUAV, clearValue );
	}
	{ // compile the shaders.
		TCOMPtr<ID3D10Blob*> rShader;
		TCOMPtr<ID3D10Blob*> rError;
		TCOMPtr<ID3D10Blob*> rShaderText;
		const tstring struct_size( FormatString( _T("%d"), m_StructureSize ) );
		{ // compile append shader
			const tstring pixel_count_X( FormatString( _T("%f"), m_AppendViewportWidth ) );
			const tstring pixel_count_Y( FormatString( _T("%f"), m_AppendViewportHeight ) );

			const D3D10_SHADER_MACRO macros[] = 
			{
				{ _T("DEFINES_SET"), _T("1") },
				{ _T("STRUCT_SIZE"), struct_size.c_str() },
				{ _T("PIXEL_COUNT_X"), pixel_count_X.c_str() },
				{ _T("PIXEL_COUNT_Y"), pixel_count_Y.c_str() },
				NULL
			};

			HRESULT hr = GetFramework()->CompileShaderFromResource( 
					NULL,
					_T("AppendStructuredPS.fx"),
					_T("main"),
					D3D_SHADER_STAGE_PIXEL,
					D3D_FEATURE_LEVEL_11_0,
					D3D_FEATURE_LEVEL_11_0,
					0,
					rShader,
					rError,
					macros,
					rShaderText );
			if( FAILED(hr) )
			{
				WriteToLog( _T("AppendStructuredPS.fx") );
				if( *rError )
				{
					WriteToLog( _T("Compile Error: %s"), rError->GetBufferPointer() );
				}
				else
				{
					WriteToLog( _T("Failed to compile shader: hr=%s"), D3DHResultToString(hr).c_str() );
				}
				if( *rShaderText )
				{
					WriteMessage( _T("%s"), rShaderText->GetBufferPointer() );
				}
				return RESULT_FAIL;
			}

			hr = GetDevice()->CreatePixelShader( 
				rShader->GetBufferPointer(),
				rShader->GetBufferSize(),
				NULL,
				m_rAppendShader );
			IFRM(hr,_T("AppendStructuredPS.fx"));
		}
		{ // compile consume shader
			const tstring pixel_count_X( FormatString( _T("%f"), m_ConsumeViewportWidth ) );
			const tstring pixel_count_Y( FormatString( _T("%f"), m_ConsumeViewportHeight ) );

			const D3D10_SHADER_MACRO macros[] = 
			{
				{ _T("DEFINES_SET"), _T("1") },
				{ _T("STRUCT_SIZE"), struct_size.c_str() },
				{ _T("PIXEL_COUNT_X"), pixel_count_X.c_str() },
				{ _T("PIXEL_COUNT_Y"), pixel_count_Y.c_str() },
				NULL
			};

			HRESULT hr = GetFramework()->CompileShaderFromResource( 
					NULL,
					_T("ConsumeStructuredPS.fx"),
					_T("main"),
					D3D_SHADER_STAGE_PIXEL,
					D3D_FEATURE_LEVEL_11_0,
					D3D_FEATURE_LEVEL_11_0,
					0,
					rShader,
					rError,
					macros,
					rShaderText );
			if( FAILED(hr) )
			{
				WriteToLog( _T("ConsumeStructured.fx") );
				if( *rError )
				{
					WriteToLog( _T("Compile Error: %s"), rError->GetBufferPointer() );
				}
				else
				{
					WriteToLog( _T("Failed to compile shader: hr=%s"), D3DHResultToString(hr).c_str() );
				}
				if( *rShaderText )
				{
					WriteMessage( _T("%s"), rShaderText->GetBufferPointer() );
				}
				return RESULT_FAIL;
			}
			hr = GetDevice()->CreatePixelShader( 
				rShader->GetBufferPointer(),
				rShader->GetBufferSize(),
				NULL,
				m_rConsumeShader );
			IFRM(hr,_T("ConsumeStructuredPS.fx"));
		}
	}
	{ // the CopyStructuredCount will use this as the direct destination.
		const D3D11_BUFFER_DESC desc = 
		{
			4,
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_READ,
			0,
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, m_rFilledSizeStaging );
		IFR(hr);
	}
	{ // Buffer for limiting writes by the pixel shader
		const D3D11_BUFFER_DESC desc = 
		{
			16,
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_CONSTANT_BUFFER,
			0,
			0,
			0
		};

		const unsigned int cb[] = 
		{
			m_AppendCountOffset + m_TotalPixelDispatchCount + m_ConsumeCountOffset,
			m_TotalPixelDispatchCount
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			cb,
			0,
			0
		};

		HRESULT hr = GetFramework()->CreateBufferTiledOptional( &desc, &data, m_rConsumeLimits );
		IFR(hr);
	}
	{ // create the rasterizer state
		const D3D11_RASTERIZER_DESC desc =
		{
			D3D11_FILL_SOLID,
			D3D11_CULL_NONE,
			TRUE,
			0,
			0.0f,
			0.0f,
			FALSE,
			FALSE,
			FALSE,
			FALSE
		};
		HRESULT hr = GetDevice()->CreateRasterizerState( &desc, m_rRastState );
		IFR(hr);
	}
	{ // create the depth stencil state
		const D3D11_DEPTH_STENCIL_DESC desc = 
		{
			FALSE,
			D3D11_DEPTH_WRITE_MASK_ZERO,
			D3D11_COMPARISON_LESS,
			FALSE,
			D3D11_DEFAULT_STENCIL_READ_MASK,
			D3D11_DEFAULT_STENCIL_WRITE_MASK,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_COMPARISON_ALWAYS
		};
		HRESULT hr = GetDevice()->CreateDepthStencilState( &desc, m_rDepthStencilState );
		IFR(hr);
	}
	{
		const D3D11_QUERY_DESC desc = 
		{
			D3D11_QUERY_PIPELINE_STATISTICS,
			0
		};
		HRESULT hr = GetDevice()->CreateQuery( &desc, m_rQuery );
		IFR(hr);
	}

	m_TargetUAVSize[0] = m_TotalPixelDispatchCount; // this is the most threads ever dispatched.
	m_TargetUAVSize[1] = 1;
	m_TargetUAVSize[2] = 1;
	m_TargetUAVFormat = DXGI_FORMAT_R32_UINT;
	m_TargetUAVDimension = D3D11_UAV_DIMENSION_BUFFER;
	m_TargetUAVMiscFlags = (D3D11_RESOURCE_MISC_FLAG)0;
	m_TargetUAVBufferFlag = (D3D11_BUFFER_UAV_FLAG)0;
	m_TargetUAVArraySize = 1;
	m_TargetUAVElementByteStride = sizeof(UINT);

	{
		TEST_RESULT tr = CComputeTest::SetupTestCase();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}
	return RESULT_PASS;
}

TEST_RESULT CAppendConsumePS::ExecuteTestCase()
{
	{
		const unsigned int zero[] = { 0, 0, 0, 0 };
		GetEffectiveContext()->ClearUnorderedAccessViewUint( m_pTargetUAV, zero );
	}
	{ // set the viewport
		const D3D11_VIEWPORT vp = 
		{
			0.0f, 
			0.0f,
			m_AppendViewportWidth,
			m_AppendViewportHeight,
			0.0f,
			1.0f
		};

		GetEffectiveContext()->RSSetViewports( 1, &vp );
	}
	
	// create a dummy rendertarget
	if ( CComputeTest::SetupRTV((UINT)m_AppendViewportWidth, (UINT)m_AppendViewportHeight) != RESULT_PASS )
	{
		WriteToLog(_T("CAppendConsumePS::ExecuteTestCase: CComputeTest::SetupRTV() failed "));
		return RESULT_FAIL;
	}

	GetEffectiveContext()->IASetIndexBuffer( m_rIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );
	GetEffectiveContext()->IASetInputLayout( m_rInputLayout );
	{
		const unsigned int stride = sizeof( Vertex );
		const unsigned int offset = 0u;
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &*m_rVertexBuffer, &stride, &offset );
	}
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	GetEffectiveContext()->VSSetShader( m_rVertexShader, NULL, NULL );

	GetEffectiveContext()->PSSetShaderResources( 0, 1, &*m_rDynamicAppendSRV );
	GetEffectiveContext()->PSSetShader( m_rAppendShader, NULL, NULL );

	GetEffectiveContext()->OMSetDepthStencilState( m_rDepthStencilState, 1u );
	GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 
		1,
		&m_pRTView,
		NULL,
		1,
		1,
		&*m_rAppConUAV,
		&m_AppendCountOffset );
	
	GetEffectiveContext()->RSSetState( m_rRastState );

	GetEffectiveContext()->Begin( m_rQuery );
	GetEffectiveContext()->Draw( 4, 0 );
	GetEffectiveContext()->End( m_rQuery );

	GetEffectiveContext()->CopyStructureCount( m_rFilledSizeStaging, 0, m_rAppConUAV );
	GetEffectiveContext()->CopyResource( m_rDebugBuffer, m_rAppendConsumeBuffer );

	{ // set the viewport
		const D3D11_VIEWPORT vp = 
		{
			0.0f, 
			0.0f,
			m_ConsumeViewportWidth,
			m_ConsumeViewportHeight,
			0.0f,
			1.0f
		};

		GetEffectiveContext()->RSSetViewports( 1, &vp );
	}
	// create a dummy rendertarget
	SAFE_RELEASE(m_pRTTex2D);
	SAFE_RELEASE(m_pRTView);
	if ( CComputeTest::SetupRTV((UINT)m_ConsumeViewportWidth, (UINT)m_ConsumeViewportHeight) != RESULT_PASS )
	{
		WriteToLog(_T("CAppendConsumePS::ExecuteTestCase: CComputeTest::SetupRTV() failed "));
		return RESULT_FAIL;
	}

	{
		ID3D11UnorderedAccessView *const pUAVs[] = 
		{
			m_pTargetUAV,
			m_rAppConUAV
		};

		const unsigned int offsets[] = 
		{
			0,
			m_UseHardwareAppendCounter ? -1 : m_TotalAppendCount + max( m_ConsumeCountOffset, m_AppendCountOffset < m_UAVSizeOffset ? m_AppendCountOffset : m_UAVSizeOffset)
		};

		GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 
			1,
			&m_pRTView,
			NULL,
			1,
			2,
			pUAVs,
			offsets );
	}

	GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &*m_rConsumeLimits );
	GetEffectiveContext()->PSSetShader( m_rConsumeShader, NULL, NULL );

	GetEffectiveContext()->Draw( 4, 0 );

	GetEffectiveContext()->CopyResource( m_pTargetResourceCopy, m_pTargetResource );
	ExecuteEffectiveContext();

	D3D11_MAPPED_SUBRESOURCE mDebug;
	{ // Open the debug append/consume data for reference
		HRESULT hr = GetImmediateContext()->Map( m_rDebugBuffer, 0, D3D11_MAP_READ, 0, &mDebug );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to map debug buffer for reading, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}
	D3D11_QUERY_DATA_PIPELINE_STATISTICS sDebug;
	{
		HRESULT hr = GetImmediateContext()->GetData( m_rQuery, &sDebug, sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS), 0 );
		if (hr == S_FALSE)
		{
			WriteToLog( _T("Query returned S_FALSE even though pipeline was drained.") );
			return RESULT_FAIL;
		}
		else IFR(hr);
	}

	{ // Verify the correct number of appended elements.
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT hr = GetImmediateContext()->Map( m_rFilledSizeStaging, 0, D3D11_MAP_READ, 0, &mapped );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Failed to map buffer for reading, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
		if( static_cast<unsigned int const*>( mapped.pData )[0] != m_AppendCountOffset + m_TotalAppendCount )
		{
			WriteToLog( _T("The total number of appends was incorrect. Expected: %u, Found: %u"), m_AppendCountOffset + m_TotalAppendCount, static_cast<unsigned int const*>( mapped.pData )[0] );
			GetImmediateContext()->Unmap( m_rFilledSizeStaging, 0 );
			return RESULT_FAIL;
		}
		GetImmediateContext()->Unmap( m_rFilledSizeStaging, 0 );
	}

	{ // Map and read m_pTargetResourceCopy for results.
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT hr = GetImmediateContext()->Map( m_pTargetResourceCopy, 0, D3D11_MAP_READ, 0, &mapped );
		if( FAILED(hr) )
		{
			WriteToLog( _T("Map(target copy) failed, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		const unsigned int consumeGoal = m_AppendCountOffset + m_TotalAppendCount + m_ConsumeCountOffset;
		const unsigned int consumesPossible = m_TotalAppendCount + m_UAVSizeOffset; 

		const unsigned int numGarbageLocations = m_UAVSizeOffset > m_AppendCountOffset ? m_UAVSizeOffset - m_AppendCountOffset : 0;
		// Reading garbage means that greater-than-equal-to must be used for verification.
		// Since we'll have at least data for the appends, but the garbage will be random.
		const unsigned int consumedGarbageCount = m_ConsumeCountOffset < numGarbageLocations ? m_ConsumeCountOffset : numGarbageLocations;

		const unsigned int validDataConsumesPossible = consumesPossible - numGarbageLocations;

		const unsigned int overConsumptionCount = consumeGoal > consumesPossible ? consumeGoal - consumesPossible : 0;

		const bool appendedOutOfBounds = m_AppendCountOffset > m_UAVSizeOffset;

		const unsigned int appendedOutOfBoundsCount = m_AppendCountOffset < consumesPossible ? m_AppendCountOffset > m_UAVSizeOffset ? m_AppendCountOffset - m_UAVSizeOffset : 0 : m_TotalAppendCount;

		const unsigned int numExpectedZeros( min( m_AppendCountOffset, consumesPossible ) + overConsumptionCount );
	
		// size differences between append/consume, as well as errors will be 
		// written to the zero location of the result UAV. account for this during the check.
		// Count the number of out of bounds reads.
		// Count the number of empty append offset reads.
		if( consumedGarbageCount )
		{
			if( *static_cast<unsigned int const*>(mapped.pData) < numExpectedZeros )
			{
				if( m_AppendCountOffset && overConsumptionCount )
				{
					WriteToLog( _T("Expected more zero consumes due to a starting append offset of %u and an over consumption of %u. Actual Total: %u"), 
							m_AppendCountOffset, overConsumptionCount, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else if( m_AppendCountOffset )
				{
					WriteToLog( _T("Expected more zero consumes due to a starting append offset of %u. Actual Total: %u"), 
							m_AppendCountOffset, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else if( overConsumptionCount )
				{

					WriteToLog( _T("Expected more zero consumes due to an over consumption of %u.  Actual Total: %u"), 
							overConsumptionCount, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else
				{
					WriteToLog( _T("Expected no zero data to be consumed.  Actual Total: %u"),
						*static_cast<unsigned int const*>(mapped.pData) );
				}

				GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0u );
				return RESULT_FAIL;
			}
		}
		else
		{
			// we always consume enough data to cover the m_AppendCountOffset.
			if( *static_cast<unsigned int const*>(mapped.pData) != numExpectedZeros )
			{
				if( m_AppendCountOffset && overConsumptionCount )
				{
					WriteToLog( _T("Expected zero consumes due to a starting append offset of %u and an over consumption of %u. Actual Total: %u"), 
							m_AppendCountOffset, overConsumptionCount, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else if( m_AppendCountOffset )
				{
					WriteToLog( _T("Expected zero consumes due to a starting append offset of %u. Actual Total: %u"), 
							m_AppendCountOffset, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else if( overConsumptionCount )
				{

					WriteToLog( _T("Expected zero consumes due to an over consumption of %u.  Actual Total: %u"), 
							overConsumptionCount, *static_cast<unsigned int const*>(mapped.pData) );
				}
				else
				{
					WriteToLog( _T("Error: All consumed data was zero while none was expected. Actual Total: %u"),
						*static_cast<unsigned int const*>(mapped.pData) );
				}

				GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
				return RESULT_FAIL;
			}
		}

		unsigned int missingAppendCount(0);  // in the end this should be equal to the number of out of bounds appends.

		// now scan the consumed data.
		for( unsigned int i(1); i < m_TotalPixelDispatchCount; ++i )
		{
			if( consumedGarbageCount )
			{
				assert( appendedOutOfBounds == false );
				// we consumed all appends, and then some, so compare >= to expected.
				if( s_pAppendCountData[i] > *(static_cast<unsigned int const*>(mapped.pData) + i) )
				{
					WriteToLog( _T("Expected at least %u Appends by thread index %u, Found: %u"),
							s_pAppendCountData[i], i, *(static_cast<unsigned int const*>(mapped.pData) + i) );
					GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
					return RESULT_FAIL;
				}

			}
			else if( appendedOutOfBounds )
			{
				// we won't be able to find all the data we wanted.  compare <=
				if( s_pAppendCountData[i] < *(static_cast<unsigned int const*>(mapped.pData) + i) )
				{
					WriteToLog( _T("Expected no more than %u Appends by thread index %u, Found: %u"),
							s_pAppendCountData[i], i, *(static_cast<unsigned int const*>(mapped.pData) + i) );
					GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
					return RESULT_FAIL;
				}
				else if( s_pAppendCountData[i] != *(static_cast<unsigned int const*>(mapped.pData) + i) )
				{
					missingAppendCount += s_pAppendCountData[i] - *(static_cast<unsigned int const*>(mapped.pData) + i);
				}
			}
			else
			{
				if( s_pAppendCountData[i] != *(static_cast<unsigned int const*>(mapped.pData) + i) )
				{
					WriteToLog( _T("Expected %u Appends by thread index %u, Found: %u"),
							s_pAppendCountData[i], i, *(static_cast<unsigned int const*>(mapped.pData) + i) );
					GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
					return RESULT_FAIL;
				}
			}
		}

		GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );

		if( appendedOutOfBounds )
		{
			if( missingAppendCount != appendedOutOfBoundsCount )
			{
				WriteToLog( _T("Expected %u missing Appends, Found: %u"), appendedOutOfBoundsCount, missingAppendCount );
				return RESULT_FAIL;
			}
		}
	}

	GetImmediateContext()->Unmap( m_rDebugBuffer, 0 );

	return RESULT_PASS;
}

void CAppendConsumePS::CleanupTestCase()
{
	m_rDynamicAppendCounts.Release();
	m_rDynamicConsumeCounts.Release();
	m_rAppendConsumeBuffer.Release();
	m_rDynamicAppendSRV.Release();
	m_rDynamicConsumeSRV.Release();
	m_rAppConUAV.Release();
	m_rFilledSizeStaging.Release();
	m_rConsumeLimits.Release();
	m_rVertexShader.Release();
	m_rConsumeShader.Release();
	m_rAppendShader.Release();
	m_rRastState.Release();
	m_rDepthStencilState.Release();
	m_rInputLayout.Release();
	m_rVertexBuffer.Release();
	m_rIndexBuffer.Release();
	m_rVertexByteCode.Release();

	SAFE_RELEASE(m_pRTTex2D);
	SAFE_RELEASE(m_pRTView);

	CComputeTest::CleanupTestCase();
}

void CAppendConsumePS::Cleanup()
{

}

