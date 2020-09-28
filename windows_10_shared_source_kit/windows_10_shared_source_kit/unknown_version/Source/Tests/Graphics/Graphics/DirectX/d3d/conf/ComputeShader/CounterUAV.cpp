#include "CounterUAV.h"

///////////////////////////////////////////////////////////////////////////////
// Counter Structured buffer UAV test
///////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------

BEGIN_NAMED_VALUES(UAV_READ_TYPE)
	NAMED_VALUE("ReadUAVAppend", READ_ALL_UAVS_APPEND )
	NAMED_VALUE("ReadUAVCounter", READ_ALL_UAVS_COUNTER )
	NAMED_VALUE("ReadUAVMatching", MATCHING_READ_WRITE_UAVS )
END_NAMED_VALUES(UAV_READ_TYPE)

//------------------------------------------------------------------------------------------------
testfactor::RFactor CCounterUAVTest::InitCommonTestParameters()
{
	// number of UAVs on the same buffer to write to
	testfactor::RFactor rfOneUAVWrite = AddParameter<UINT>(_T("NumUAVsForWrite"), &m_numUAVsWrite, 1);
	CUserValueSet<UINT> * pNumUAVs = new CUserValueSet<UINT>;
	pNumUAVs->AddValue(2);
	//pNumUAVs->Adde(3);
	testfactor::RFactor rfMultiUAVsWrite = AddParameterValueSet<UINT>(_T("NumUAVsForWrite"), pNumUAVs);
	// number of UAVs on the same buffer to read from
	testfactor::RFactor rfOneUAVRead = AddParameter<UINT>(_T("NumUAVsForRead"), &m_numUAVsRead, 1);
	testfactor::RFactor rfMultiUAVsRead = AddParameterValueSet<UINT>(_T("NumUAVsForRead"), pNumUAVs);
	
	// Type of UAV for read
	// BUGBUG: Do not verify the results read from append UAV for now
	// testfactor::RFactor rfUAVReadAppend = AddParameter<UAV_READ_TYPE>(_T("TypeOfUAVForRead"), &m_readUAVType,
	//	READ_ALL_UAVS_APPEND);
	testfactor::RFactor rfUAVReadCounter = AddParameter<UAV_READ_TYPE>(_T("TypeOfUAVForRead"), &m_readUAVType,
		READ_ALL_UAVS_COUNTER);
	testfactor::RFactor rfUAVReadMatching = AddParameterValue<UAV_READ_TYPE>(_T("TypeOfUAVForRead"), 
		MATCHING_READ_WRITE_UAVS);

	// if append buffer is mixed with counter buffer
	CTestCaseParameter<bool> *pMixedAppendPara = AddParameter<bool>(_T("MixedAppend"), &m_bMixedAppend);
	pMixedAppendPara->SetDefaultValue(FALSE);
	testfactor::RFactor rfIsMixedAppend = AddParameterValueSet<bool>( pMixedAppendPara, new CBoolValueSet() );

	// if outbound read/write happens
	CTestCaseParameter<bool> *pOutBoundReadPara = AddParameter<bool>(_T("OutBoundRead"), &m_bOutBoundRead);
	pOutBoundReadPara->SetDefaultValue(FALSE);
	testfactor::RFactor rfIsOutBoundRead = AddParameterValueSet<bool>( pOutBoundReadPara, new CBoolValueSet());
	CTestCaseParameter<bool> *pOutBoundWritePara = AddParameter<bool>(_T("OutBoundWrite"), &m_bOutBoundWrite);
	pOutBoundWritePara->SetDefaultValue(FALSE);
	testfactor::RFactor rfIsOutBoundWrite = AddParameterValueSet<bool>( pOutBoundWritePara, new CBoolValueSet() );

	// if the UAV is full after write
	CTestCaseParameter<bool> *pFullUAVPara = AddParameter<bool>(_T("FullUAVWrite"), &m_bFullUAV);
	pFullUAVPara->SetDefaultValue(TRUE);
	testfactor::RFactor rfIsFullUAV = AddParameterValueSet<bool>( pFullUAVPara, new CBoolValueSet());
	// if two UAVs are continuous in memory	
	CTestCaseParameter<bool> *pContiUAVPara = AddParameter<bool>(_T("ContinuousUAVs"), &m_bContinuousUAV);
	pContiUAVPara->SetDefaultValue(TRUE);
	testfactor::RFactor rfIsContinuousUAV = AddParameterValueSet<bool>( pContiUAVPara, new CBoolValueSet());

	testfactor::RFactor rfCommon =  ( 
		// Single UAV to write to
			rfOneUAVWrite * rfOneUAVRead * rfUAVReadCounter // * rfIsOutBoundRead * rfIsOutBoundWrite
		// Multi UAVs to write to
		  + rfMultiUAVsWrite * ( rfOneUAVRead * rfUAVReadCounter + rfMultiUAVsRead * rfUAVReadMatching ) 
		  * rfIsMixedAppend * rfIsFullUAV * rfIsContinuousUAV
		) ;

	return rfCommon;
}

//------------------------------------------------------------------------------------------------
void CCounterUAVTest::InitTestParameters()
{
	// dispatch groups
	CUserValueSet<UINT> * pDimensionValues = new CUserValueSet<UINT>;
	pDimensionValues->AddValue(1);
	pDimensionValues->AddValue(2);
	pDimensionValues->AddValue(8);
	testfactor::RFactor rfDispatchX = AddParameter<UINT>( _T("DispatchDimensionX"), &m_dispatchX, pDimensionValues);
	testfactor::RFactor rfDispatchY = AddParameter<UINT>( _T("DispatchDimensionY"), &m_dispatchY, 1);
	testfactor::RFactor rfDispatchZ = AddParameter<UINT>( _T("DispatchDimensionZ"), &m_dispatchZ, 1);
	
	// threads per group
	testfactor::RFactor rfThreadNumX = AddParameter<UINT>( _T("NumThreadPerGroupX"), &m_numThreadsX, pDimensionValues);
	testfactor::RFactor rfThreadNumY = AddParameter<UINT>( _T("NumThreadPerGroupY"), &m_numThreadsY, pDimensionValues);
	testfactor::RFactor rfThreadNumZ = AddParameter<UINT>( _T("NumThreadPerGroupZ"), &m_numThreadsZ, pDimensionValues);
	
	testfactor::RFactor rfRoot =  
		rfDispatchX * rfDispatchY  * rfDispatchZ * ( rfThreadNumX % rfThreadNumY % rfThreadNumZ) 
		* InitCommonTestParameters() ;

	SetRootTestFactor(rfRoot);	

	// Adding priority filters
	CFilterHandle filterNumThreads = FilterEqual<UINT>(_T("DispatchDimensionX"), 2) &&
		FilterEqual<UINT>(_T("NumThreadPerGroupX"), 8) && 
		FilterEqual<UINT>(_T("NumThreadPerGroupY"), 8) &&
		FilterEqual<UINT>(_T("NumThreadPerGroupZ"), 8);

	AddPriorityPatternFilter(filterNumThreads, WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );

}


TEST_RESULT CCounterUAVTest::Setup()
{
	if ( !CComputeTest::Setup() )
	{
		WriteToLog(_T("CComputeTest::Setup() failed"));
		return RESULT_FAIL;
	}
	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		WriteToLog(_T("Skip this group on feature levels less than 11_0."));
		return RESULT_SKIP;
	}

	return RESULT_PASS;
}

//------------------------------------------------------------------------------------------------
TEST_RESULT CCounterUAVTest::SetupTestCase()
{
	if ( SkipSlowOnRefWithMessage( m_dispatchX * m_numThreadsX * m_numThreadsY * m_numThreadsZ > 512  ) )
		return RESULT_SKIP;
	
	if ( SetupUAVs() != RESULT_PASS )
	{
		WriteToLog(_T("CCounterUAVTest::SetupTestCase: SetupUAVs() failed "));
		return RESULT_FAIL;
	}

	if ( SetupShader() != RESULT_PASS )
	{
		WriteToLog(_T("CCounterUAVTest::SetupTestCase: SetupShader() failed "));
		return RESULT_FAIL;
	}

	return RESULT_PASS;

}

//------------------------------------------------------------------------------------------------
#define CREATE_BUFFER(descStruct, pResource) \
	hr = GetFramework()->CreateBufferTiledOptional(&descStruct, NULL, &(pResource)); \
	if (FAILED( hr )) \
	{ \
	WriteToLog(_T("CCounterUAVTest::SetupUAVs(): CreateBuffer() failed for "#pResource". hr = %s"), \
				D3DHResultToString(hr).c_str() ); \
		tr = RESULT_FAIL; \
		goto error_end; \
	}

#define CREATE_UAV(pResource, descStruct, pUAView) \
	hr = m_pDevice->CreateUnorderedAccessView(pResource, &descStruct, &(pUAView) ); \
	if (FAILED( hr )) \
	{ \
	WriteToLog(_T("CMemorySyncTest::CreateSourceUAVs(): CreateUnorderedAccessView failed for "#pUAView". hr = %s"), \
				D3DHResultToString(hr).c_str() ); \
		tr = RESULT_FAIL; \
		goto error_end; \
	}

TEST_RESULT CCounterUAVTest::SetupUAVs()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	// add extra bytes between UAVs if they are not continuous, 
	// use sizeof(RECORD) to make sure that buffer size is multiples of structure size
	m_UAVGap = 0;
	if ( !m_bContinuousUAV )
		m_UAVGap = sizeof(RECORD);
	
	// make it half full by doubling the size if the UAV should not be full
	m_sizeScale = 1;
	if ( !m_bFullUAV )
		m_sizeScale = 2;

	UINT threadsTotal = m_dispatchX * m_dispatchY * m_dispatchZ * m_numThreadsX * m_numThreadsY * m_numThreadsZ;
	m_UAVSize = sizeof(RECORD) * threadsTotal * m_sizeScale;
	m_bufferSize = ( m_UAVSize + m_UAVGap ) * m_numUAVsWrite;

	// initialize UAV pointers
	m_ppWriteUAVs = new ID3D11UnorderedAccessView*[m_numUAVsWrite];
	m_ppReadUAVs = new ID3D11UnorderedAccessView*[m_numUAVsRead];
	for (UINT i = 0; i < m_numUAVsWrite; i++)
		m_ppWriteUAVs[i] = NULL;
	for (UINT i = 0; i < m_numUAVsRead; i++)
		m_ppReadUAVs[i] = NULL;

	UINT *pInitData = new UINT[m_bufferSize/sizeof(UINT)];

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth = m_bufferSize;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = sizeof(RECORD);
	// Create output buffer
	CREATE_BUFFER( bufDesc, m_pOutputBuf );
	// Create source buffer
	CREATE_BUFFER( bufDesc, m_pSourceBuf );
	// Create staging buffer for output
	bufDesc.Usage = D3D11_USAGE_STAGING;
	bufDesc.BindFlags = 0;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	CREATE_BUFFER( bufDesc, m_pCopyOutBuffer );

	// initialize the data in the Source buffer
	memset(pInitData, 0xAD, m_bufferSize);
	GetImmediateContext()->UpdateSubresource(m_pSourceBuf, 0, NULL, pInitData, 0, 0);
	
	if ( m_bMixedAppend )
	{   // Create a buffer to keep a global counter for Append buffer
		bufDesc.ByteWidth = sizeof(UINT);
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.StructureByteStride = 0;
		CREATE_BUFFER( bufDesc, m_pGlobalCounterBuf );
	}

	bufDesc.ByteWidth = sizeof(UINT);
	bufDesc.Usage = D3D11_USAGE_STAGING;
	bufDesc.BindFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufDesc.StructureByteStride = 0;
	CREATE_BUFFER( bufDesc, m_pCopySctCountBuf );


	// UAVs for writes
	D3D11_UNORDERED_ACCESS_VIEW_DESC  outUAVDesc;
	outUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	outUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	for ( UINT i = 0; i < m_numUAVsWrite; i++ )
	{	
		if ( i == 0)
			outUAVDesc.Buffer.FirstElement = 0;
		else
			outUAVDesc.Buffer.FirstElement = ( i * m_UAVSize + m_UAVGap ) / sizeof(RECORD);
		outUAVDesc.Buffer.NumElements = m_UAVSize / sizeof(RECORD);
		outUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		if ( m_bMixedAppend && i == 0 )
			outUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		CREATE_UAV( m_pSourceBuf, outUAVDesc, (m_ppWriteUAVs[i]) );
	}

	// UAVs for reads
	if ( m_numUAVsRead == 1)
	{
		outUAVDesc.Buffer.FirstElement = 0;
		outUAVDesc.Buffer.NumElements = m_bufferSize / sizeof(RECORD);
		if ( m_readUAVType == READ_ALL_UAVS_APPEND )
			outUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		else if ( m_readUAVType == READ_ALL_UAVS_COUNTER )
			outUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		CREATE_UAV( m_pSourceBuf, outUAVDesc, (m_ppReadUAVs[0]) );
	}
	else
	{
		for ( UINT i = 0; i < m_numUAVsRead; i++ )
		{	
			if ( i == 0)
				outUAVDesc.Buffer.FirstElement = 0;
			else
				outUAVDesc.Buffer.FirstElement = ( i * m_UAVSize + m_UAVGap ) / sizeof(RECORD);
			outUAVDesc.Buffer.NumElements = m_UAVSize / sizeof(RECORD);
			outUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
			if ( m_bMixedAppend && i == 0 )
				outUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
			CREATE_UAV( m_pSourceBuf, outUAVDesc, (m_ppReadUAVs[i]) );
		}
	}

	// create output UAV
	outUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	outUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	outUAVDesc.Buffer.FirstElement = 0;
	outUAVDesc.Buffer.NumElements = m_bufferSize / sizeof(RECORD);
	outUAVDesc.Buffer.Flags = 0;
	CREATE_UAV( m_pOutputBuf, outUAVDesc, m_pOutputUAV );

	if ( m_bMixedAppend )
	{   // Create the UAV to keep a global counter for Append buffer
		outUAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		outUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		outUAVDesc.Buffer.FirstElement = 0;
		outUAVDesc.Buffer.NumElements = 1;
		outUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		CREATE_UAV( m_pGlobalCounterBuf, outUAVDesc, m_pGlobalCounterUAV );
	}

	SAFE_DELETE_ARRAY(pInitData);
	return tr;
	

error_end:

	SAFE_DELETE_ARRAY(pInitData);
	return tr;
	
}

//------------------------------------------------------------------------------------------------
#define CREATE_CSSHADER( pShader, macros) \
	hr = CComputeTest::CompileAndCreateComputeShaderFromResource( \
		_T("RC_COUNTERUAV_FX"),  _T("CSMain"), D3D_FEATURE_LEVEL_11_0, \
		&pShader, macros); \
	if ( FAILED(hr) ) \
	{ \
		tr = RESULT_FAIL; \
		WriteToLog( _T( "CComputeTest::CompileAndCreateComputeShaderFromResource failed for "#pShader". hr = %s."), \
			D3DHResultToString(hr).c_str() ); \
		goto shader_err; \
	}
TEST_RESULT CCounterUAVTest::SetupShader()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	tstring numThreadsXStr = FormatString( "%i", m_numThreadsX );
	tstring numThreadsYStr = FormatString( "%i", m_numThreadsY );
	tstring numThreadsZStr = FormatString( "%i", m_numThreadsZ );
		
	D3D10_SHADER_MACRO macroWriteAppend[] =
	{
		{ "numThreadX", numThreadsXStr.c_str()},
		{ "numThreadY", numThreadsYStr.c_str()},
		{ "numThreadZ", numThreadsZStr.c_str()},
		{ "WRITE_TO_APPEND", "true" },
		NULL
	};
	CREATE_CSSHADER(m_pCSWriteToAppend, macroWriteAppend);
	
	D3D10_SHADER_MACRO macroWriteCounter[] =
	{
		{ "numThreadX", numThreadsXStr.c_str()},
		{ "numThreadY", numThreadsYStr.c_str()},
		{ "numThreadZ", numThreadsZStr.c_str()},
		{ "WRITE_TO_COUNTER", "true" },
		NULL
	};
	CREATE_CSSHADER(m_pCSWriteToCounter, macroWriteCounter);

	D3D10_SHADER_MACRO macroReadAppend[] =
	{
		{ "numThreadX", numThreadsXStr.c_str()},
		{ "numThreadY", numThreadsYStr.c_str()},
		{ "numThreadZ", numThreadsZStr.c_str()},
		{ "READ_FROM_APPEND", "true" },
		NULL
	};
	CREATE_CSSHADER(m_pCSReadFromAppend, macroReadAppend);
	
	D3D10_SHADER_MACRO macroReadCounter[] =
	{
		{ "numThreadX", numThreadsXStr.c_str()},
		{ "numThreadY", numThreadsYStr.c_str()},
		{ "numThreadZ", numThreadsZStr.c_str()},
		{ "READ_FROM_COUNTER", "true" },
		NULL
	};
	CREATE_CSSHADER(m_pCSReadFromCounter, macroReadCounter);
	

	D3D10_SHADER_MACRO macroReadCounterSingle[] =
	{
		{ "numThreadX", "1"},
		{ "numThreadY", "1"},
		{ "numThreadZ", "1"},
		{ "READ_FROM_COUNTER", "true" },
		NULL
	};
	CREATE_CSSHADER(m_pCSReadFromCounterSingle, macroReadCounterSingle);
	
	return tr;

shader_err:

	return tr;
}




//------------------------------------------------------------------------------------------------
TEST_RESULT CCounterUAVTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	const UINT clearVals[4] = {0, 0, 0, 0};
	m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pOutputUAV, clearVals );

	
	ID3D11UnorderedAccessView * ppUAViews[8] = {NULL};
	ppUAViews[0] = m_pOutputUAV;
	UINT offsets[8] = {0};
	UINT numUAVs = 0;

	// writing passes
	for (UINT i = 0; i < m_numUAVsWrite; i++)
	{
		ppUAViews[1] = m_ppWriteUAVs[i];
		
		// the first one of mixed multiple UAVs is append UAV
		if ( m_bMixedAppend && i == 0)
		{
			ppUAViews[2] = m_pGlobalCounterUAV;
			numUAVs = 3;
			m_pEffectiveContext->CSSetShader(m_pCSWriteToAppend, NULL, NULL);
		}
		else // the rest are counter UAVs
		{
			numUAVs = 2;
			m_pEffectiveContext->CSSetShader(m_pCSWriteToCounter, NULL, NULL);
		}
		m_pEffectiveContext->CSSetUnorderedAccessViews(0, numUAVs, ppUAViews, offsets);
		m_pEffectiveContext->Dispatch(m_dispatchX, m_dispatchY, m_dispatchZ);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): ExecuteEffectiveContext() failed for UAV write.") );
			return RESULT_FAIL;
		}
		
		// verify copyStructureCount
		UINT expCountVal = m_dispatchX * m_dispatchY * m_dispatchZ * m_numThreadsX * m_numThreadsY * m_numThreadsZ;
		tr = VerifyStructureCount(m_ppWriteUAVs[i], expCountVal);
		if ( tr == RESULT_FAIL )
		{
			WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): VerifyStructureCount() failed for UAV write.") );
			return RESULT_FAIL;
		}
		
	}

	// The first read pass: Use the same UAV as the one used for write
	m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pOutputUAV, clearVals );
	for (UINT i = 0; i < m_numUAVsWrite; i++)
	{
		ppUAViews[1] = m_ppWriteUAVs[i];
		
		// the first one of mixed multiple UAVs is append UAV
		if ( m_bMixedAppend && i == 0)
		{
			ppUAViews[2] = m_pGlobalCounterUAV;
			numUAVs = 3;
			offsets[0] = offsets[1] = offsets[2] = -1;
			m_pEffectiveContext->CSSetShader(m_pCSReadFromAppend, NULL, NULL);
		}
		else // the rest are counter UAVs
		{
			numUAVs = 2;
			offsets[0] = offsets[1] = -1;
			m_pEffectiveContext->CSSetShader(m_pCSReadFromCounter, NULL, NULL);
		}
		m_pEffectiveContext->CSSetUnorderedAccessViews(0, numUAVs, ppUAViews, offsets);
		m_pEffectiveContext->Dispatch(m_dispatchX, m_dispatchY, m_dispatchZ);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): ExecuteEffectiveContext() failed for UAV read,\
						   first pass.") );
			return RESULT_FAIL;
		}

		// verify copyStructureCount
		tr = VerifyStructureCount(m_ppWriteUAVs[i], 0);
		if ( tr == RESULT_FAIL )
		{
			WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): VerifyStructureCount() failed for UAV read, \
						   first pass.") );
			return RESULT_FAIL;
		}

		// verify data in result UAV
		// Data written by Append UAV is not verified for now
		if ( m_bMixedAppend && i == 0 )
			continue;
		tr = VerifyResultData(m_ppWriteUAVs[0]); // the window of data on the result UAV is the same as m_ppWriteUAVs[0]
		if ( tr == RESULT_FAIL )
		{
			WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): VerifyResultData() failed for UAV %d."), i );
			return RESULT_FAIL;
		}
	}

	// The second read pass: Use a different UAV from the one used for write
	m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pOutputUAV, clearVals );
	if ( m_readUAVType != MATCHING_READ_WRITE_UAVS )  // Using one big UAV covering the whole buffer to read out data
	{
		UINT dispatchNum[3];
		// use shader m_pCSReadFromCounterSingle to read from the UAV, which just has 1 thread per group
		dispatchNum[0] = m_bufferSize / sizeof(RECORD); 
		dispatchNum[1] = 1;
		dispatchNum[2] = 1;
		ppUAViews[1] = m_ppReadUAVs[0];
		if ( m_readUAVType == READ_ALL_UAVS_APPEND )
		{
			ppUAViews[2] = m_pGlobalCounterUAV;
			numUAVs = 3;
			offsets[0] = offsets[2] = -1;
			offsets[1] = m_bufferSize / sizeof(RECORD);
			m_pEffectiveContext->CSSetShader(m_pCSReadFromAppend, NULL, NULL);
		}
		else if ( m_readUAVType == READ_ALL_UAVS_COUNTER )
		{
			numUAVs = 2;
			offsets[0] = -1;
			offsets[1] = m_bufferSize / sizeof(RECORD);
			m_pEffectiveContext->CSSetShader(m_pCSReadFromCounterSingle, NULL, NULL);
		}
		m_pEffectiveContext->CSSetUnorderedAccessViews(0, numUAVs, ppUAViews, offsets);
		m_pEffectiveContext->Dispatch(dispatchNum[0], dispatchNum[1], dispatchNum[2]);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): ExecuteEffectiveContext() failed.") );
			return RESULT_FAIL;
		}

		// verify copyStructureCount
		tr = VerifyStructureCount(m_ppReadUAVs[0], 0);
		if ( tr == RESULT_FAIL )
		{
			WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): VerifyStructureCount() failed for UAV read, \
						   second pass.") );
			return RESULT_FAIL;
		}

		// verify data in result UAV
		for(UINT i = 0; i < m_numUAVsWrite; i++)
		{
			// Data written by Append UAV is not verified for now
			if ( m_bMixedAppend && i == 0 )
				continue;
			tr = VerifyResultData(m_ppWriteUAVs[i]);
			if ( tr == RESULT_FAIL )
			{
				WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): VerifyResultData() failed for UAV %d."), i );
				return RESULT_FAIL;
			}
		}
	}
	else // using multiple UAVs to read data, with matching type of the UAVs used for write
	{
		for (UINT i = 0; i < m_numUAVsRead; i++)
		{ 
			ppUAViews[1] = m_ppReadUAVs[i];
			// the first one of mixed multiple UAVs is append UAV
			if ( m_bMixedAppend && i == 0)
			{
				ppUAViews[2] = m_pGlobalCounterUAV;
				numUAVs = 3;
				offsets[0] = offsets[2] = -1;
				offsets[1] = ( m_UAVSize / m_sizeScale ) / sizeof(RECORD);
				m_pEffectiveContext->CSSetShader(m_pCSReadFromAppend, NULL, NULL);
			}
			else // the rest are counter UAVs
			{
				numUAVs = 2;
				offsets[0] = -1;
				offsets[1] = ( m_UAVSize / m_sizeScale ) / sizeof(RECORD);
				m_pEffectiveContext->CSSetShader(m_pCSReadFromCounter, NULL, NULL);
			}

			m_pEffectiveContext->CSSetUnorderedAccessViews(0, numUAVs, ppUAViews, offsets);
			m_pEffectiveContext->Dispatch(m_dispatchX, m_dispatchY, m_dispatchZ);
			if( FAILED( hr = ExecuteEffectiveContext() ) )
			{
				WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): ExecuteEffectiveContext() failed for UAV read,\
							   second pass.") );
				return RESULT_FAIL;
			}

			// verify copyStructureCount
			tr = VerifyStructureCount(m_ppReadUAVs[i], 0);
			if ( tr == RESULT_FAIL )
			{
				WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): VerifyStructureCount() failed for UAV read, \
							   second pass.") );
				return RESULT_FAIL;
			}
			// verify data in result UAV
			// Data written by Append UAV is not verified for now
			if ( m_bMixedAppend && i == 0 )
				continue;
			tr = VerifyResultData(m_ppReadUAVs[0]); // the window of data on the result UAV is the same as m_ppReadUAVs[0]
			if ( tr == RESULT_FAIL )
			{
				WriteToLog( _T("CCounterUAVTest::ExecuteTestCase(): VerifyResultData() failed for UAV %d."), i );
				return RESULT_FAIL;
			}
		}
	} //else
	return tr;

}

//------------------------------------------------------------------------------------------------
TEST_RESULT CCounterUAVTest::VerifyStructureCount(ID3D11UnorderedAccessView* pUAVWithCount, UINT expectVal)
{
	TEST_RESULT tr = RESULT_PASS;

	GetImmediateContext()->CopyStructureCount(m_pCopySctCountBuf, 0, pUAVWithCount);
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	HRESULT hr = GetImmediateContext()->Map( m_pCopySctCountBuf, 0,  D3D11_MAP_READ, 0, &mappedRes );
	if (FAILED (hr))
	{
		WriteToLog( _T("CCounterUAVTest::VerifyStructureCount: Failed to map m_pCopySctCountBuf.") );
		tr = RESULT_FAIL;
		goto Done;
	}

	if ( *((UINT*)mappedRes.pData) != expectVal )
	{
		WriteToLog( _T("CCounterUAVTest::VerifyStructureCount: result of CopyStructureCount does not match expected value.\
					   found value: %d, expected value: %d"), *((UINT*)mappedRes.pData), expectVal );
		tr = RESULT_FAIL;
		goto Done;
	}
Done:
	GetImmediateContext()->Unmap( m_pCopySctCountBuf, 0);
	return tr;
}

//------------------------------------------------------------------------------------------------
TEST_RESULT CCounterUAVTest::VerifyResultData(ID3D11UnorderedAccessView* pUAVDataWindow)
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAViewDesc;
	pUAVDataWindow->GetDesc(&UAViewDesc);

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	GetImmediateContext()->CopyResource( m_pCopyOutBuffer, m_pOutputBuf);
	hr = GetImmediateContext()->Map( m_pCopyOutBuffer, 0,  D3D11_MAP_READ, 0, &mappedRes );
	if (FAILED (hr))
	{
		WriteToLog( _T("CCounterUAVTest::VerifyResultData: Failed to map m_pCopyOutBuffer.") );
		tr = RESULT_FAIL;
		goto Done;
	}
	UINT firstElement = UAViewDesc.Buffer.FirstElement;
	UINT numElements = UAViewDesc.Buffer.NumElements;
	if ( !m_bFullUAV )
		numElements /= m_sizeScale;
	UINT *pResData = NULL;

	for (UINT i = 0; i <  numElements; i++)
	{
		pResData = (UINT *)mappedRes.pData + ( firstElement + i ) * sizeof(RECORD)/sizeof(UINT);
		if ( ( firstElement == 0 && ( pResData[0] != i || pResData[0] != pResData[1] || pResData[0] != pResData[2]) ) ||
			 ( firstElement != 0 && ( pResData[0] != i || pResData[1] != i + firstElement || pResData[0] != pResData[2] ) ) )
		{
			UINT expecedDecrement = firstElement + i; 
			WriteToLog( _T("CCounterUAVTest::VerifyResultData: Data in the result UAV do not match.\
						   incrementID = %d, decrementID = %d, data = %d, expectedValue = (%d, %d, %d )"), 
						   pResData[0], pResData[1], pResData[2], i, expecedDecrement, i );
			tr = RESULT_FAIL;
			goto Done;
		}
	}

Done:
	GetImmediateContext()->Unmap( m_pCopyOutBuffer, 0);
	return tr;
}

//------------------------------------------------------------------------------------------------
void CCounterUAVTest::CleanupTestCase()
{
	if (m_ppWriteUAVs)
	{
		for (UINT i = 0; i < m_numUAVsWrite; i++)
			SAFE_RELEASE(m_ppWriteUAVs[i]);
	}
	if (m_ppReadUAVs)
	{
		for (UINT i = 0; i < m_numUAVsRead; i++)
			SAFE_RELEASE(m_ppReadUAVs[i]);
	}

	SAFE_RELEASE(m_pOutputBuf);
	SAFE_RELEASE(m_pSourceBuf);
	SAFE_RELEASE(m_pCopyOutBuffer);
	SAFE_RELEASE(m_pGlobalCounterBuf);
	SAFE_RELEASE(m_pCopySctCountBuf);

	SAFE_RELEASE(m_pOutputUAV);
	SAFE_RELEASE(m_pGlobalCounterUAV);

	SAFE_RELEASE(m_pCSWriteToCounter);
	SAFE_RELEASE(m_pCSWriteToAppend);
	SAFE_RELEASE(m_pCSReadFromCounter);
	SAFE_RELEASE(m_pCSReadFromAppend);
	SAFE_RELEASE(m_pCSReadFromCounterSingle);

	SAFE_DELETE_ARRAY(m_ppWriteUAVs);
	SAFE_DELETE_ARRAY(m_ppReadUAVs);
}

///////////////////////////////////////////////////////////////////////////////
// Counter Structured buffer UAV test for Pixel Shader
///////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------

void CCounterUAVPSTest::InitTestParameters()
{
	// viewport width and height, working as the X and Y dimensions of a thead group in Compute
	CUserValueSet<UINT> * pViewportVals = new CUserValueSet<UINT>;
	pViewportVals->AddValue(1);
	pViewportVals->AddValue(63);
	pViewportVals->AddValue(2048);
	testfactor::RFactor rfViewWidth = AddParameter<UINT>(_T("ViewWidth"), &m_ViewWidth, pViewportVals);
	// testfactor::RFactor rfViewHeight = AddParameter<UINT>(_T("ViewHeight"), &m_ViewHeight, pViewportVals);


	// testing both "UAV with RTV" and "UAV only" cases for pixel shader
	testfactor::RFactor rfIsUAVOnly = AddParameter<bool>( _T( "IsUAVOnly" ), &m_bUAVOnly, new CBoolValueSet() );
 
	// get the common test factors from the base class
	testfactor::RFactor rfCommon = CCounterUAVTest::InitCommonTestParameters();
	testfactor::RFactor rfRoot = rfViewWidth *  rfCommon % rfIsUAVOnly;

	SetRootTestFactor(rfRoot);	

	// Adding priority filters
	AddPriorityPatternFilter( FilterEqual<UINT>(_T("ViewWidth"), 63), WeightFactorLarge );
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CCounterUAVPSTest::Setup()
{
	TEST_RESULT tr = CComputeTest::Setup();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CComputeTest::Setup() failed"));
		return tr;
	}

	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		WriteToLog(_T("Skip this group on feature levels less than 11_0."));
		return RESULT_SKIP;
	}
	
	tr = SetupVSStage();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CCounterUAVPSTest::SetupVSStage() failed"));
		return tr;
	}

	return RESULT_PASS;
}

//------------------------------------------------------------------------------------------------
TEST_RESULT CCounterUAVPSTest::SetupTestCase()
{
	if ( SkipSlowOnRefWithMessage( m_ViewWidth > 2048  ) )
		return RESULT_SKIP;
	
	if ( SetupUAVs() != RESULT_PASS )
	{
		WriteToLog(_T("CCounterUAVTest::SetupTestCase: SetupUAVs() failed "));
		return RESULT_FAIL;
	}

	if ( SetupShader() != RESULT_PASS )
	{
		WriteToLog(_T("CCounterUAVTest::SetupTestCase: SetupShader() failed "));
		return RESULT_FAIL;
	}

	// create a dummy rendertarget
	if ( CComputeTest::SetupRTV( m_bufferSize/sizeof(RECORD), 1 ) != RESULT_PASS )
	{
		WriteToLog(_T("CCounterUAVTest::SetupTestCase: CComputeTest::SetupRTV() failed "));
		return RESULT_FAIL;
	}

	// Set vertex buffer
	UINT stride = sizeof( float ) * 3;
	UINT offset = 0;
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );

	// Set shaders
	GetEffectiveContext()->VSSetShader( m_pVertexShader, NULL, 0 );

	return RESULT_PASS;

}

//------------------------------------------------------------------------------------------------
TEST_RESULT CCounterUAVPSTest::SetupUAVs()
{
	HRESULT hr;

	m_dispatchX = 1;
	m_dispatchY = 1;
	m_dispatchZ = 1; 
	// using a buffer RTV as the dummy RTV, so the viewport height is 1
	m_numThreadsX = m_ViewWidth;
	m_numThreadsY = 1;
	m_numThreadsZ = 1;

	// create UAVS
	TEST_RESULT tr = RESULT_PASS;
	tr = CCounterUAVTest::SetupUAVs();
	if ( tr != RESULT_PASS )
	{
		WriteToLog(_T("CCounterUAVPSTest::SetupUAVs(): CCounterUAVTest::SetupUAVs() failed "));
		return tr;
	}
	
	return tr;

}

//------------------------------------------------------------------------------------------------
#define CREATE_PSSHADER( pShader, macros) \
	hr = CComputeTest::CompileAndCreatePixelShaderFromResource( \
		_T("RC_COUNTERUAV_FX"),  _T("PSMain"), D3D_FEATURE_LEVEL_11_0, \
		&pShader, macros); \
	if ( FAILED(hr) ) \
	{ \
		tr = RESULT_FAIL; \
		WriteToLog( _T( "CComputeTest::CompileAndCreatePixelShaderFromResource failed for "#pShader". hr = %s."), \
			D3DHResultToString(hr).c_str() ); \
		goto shader_err; \
	}
TEST_RESULT CCounterUAVPSTest::SetupShader()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	tstring isUAVOnly;
	if (m_bUAVOnly)
		isUAVOnly = _T("1");
	else
		isUAVOnly = _T("0");
		
	D3D10_SHADER_MACRO macroWriteAppend[] =
	{
		{ _T( "PS_WRITE_TO_APPEND" ), _T( "true" ) },
		{ _T( "PIXEL_SHADER_UAV_ONLY" ), isUAVOnly.c_str() },
		NULL
	};
	CREATE_PSSHADER(m_pPSWriteToAppend, macroWriteAppend);
	
	D3D10_SHADER_MACRO macroWriteCounter[] =
	{
		{ _T( "PS_WRITE_TO_COUNTER" ), _T( "true" ) },
		{ _T( "PIXEL_SHADER_UAV_ONLY" ), isUAVOnly.c_str() },
		NULL
	};
	CREATE_PSSHADER(m_pPSWriteToCounter, macroWriteCounter);

	D3D10_SHADER_MACRO macroReadAppend[] =
	{
		{ _T( "PS_READ_FROM_APPEND" ), _T( "true" ) },
		{ _T( "PIXEL_SHADER_UAV_ONLY" ), isUAVOnly.c_str() },
		NULL
	};
	CREATE_PSSHADER(m_pPSReadFromAppend, macroReadAppend);
	
	D3D10_SHADER_MACRO macroReadCounter[] =
	{
		{ _T( "PS_READ_FROM_COUNTER" ), _T( "true" ) },
		{ _T( "PIXEL_SHADER_UAV_ONLY" ), isUAVOnly.c_str() },
		NULL
	};
	CREATE_PSSHADER(m_pPSReadFromCounter, macroReadCounter);
		
	return tr;

shader_err:

	return tr;
}

//------------------------------------------------------------------------------------------------
TEST_RESULT CCounterUAVPSTest::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;

	const UINT clearVals[4] = {0, 0, 0, 0};
	m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pOutputUAV, clearVals );
	
	ID3D11UnorderedAccessView * ppUAViews[8] = {NULL};
	ppUAViews[0] = m_pOutputUAV;
	UINT offsets[8] = {0};
	UINT numUAVs = 0;
	
	// setup viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)m_ViewWidth;
	vp.Height = 1.0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// writing passes
	for (UINT i = 0; i < m_numUAVsWrite; i++)
	{
		ppUAViews[1] = m_ppWriteUAVs[i];
		
		// the first one of mixed multiple UAVs is append UAV
		if ( m_bMixedAppend && i == 0)
		{
			ppUAViews[2] = m_pGlobalCounterUAV;
			numUAVs = 3;
			m_pEffectiveContext->PSSetShader(m_pPSWriteToAppend, NULL, NULL);
		}
		else // the rest are counter UAVs
		{
			numUAVs = 2;
			m_pEffectiveContext->PSSetShader(m_pPSWriteToCounter, NULL, NULL);
		}
		vp.Width = (float)m_ViewWidth;
		m_pEffectiveContext->RSSetViewports( 1, &vp );
		if (m_bUAVOnly)
		{
			m_pEffectiveContext->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL,
			0, numUAVs, ppUAViews, offsets);
		}
		else
		{
			m_pEffectiveContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &m_pRTView, NULL,
			1, numUAVs, ppUAViews, offsets);
		}
		m_pEffectiveContext->Draw(6, 0);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): ExecuteEffectiveContext() failed for UAV write.") );
			return RESULT_FAIL;
		}
		
		// verify copyStructureCount
		UINT expCountVal = m_ViewWidth;
		tr = VerifyStructureCount(m_ppWriteUAVs[i], expCountVal);
		if ( tr == RESULT_FAIL )
		{
			WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): VerifyStructureCount() failed for UAV write.") );
			return RESULT_FAIL;
		}
		
	}

	// The first read pass: Use the same UAV as the one used for write
	m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pOutputUAV, clearVals );
	for (UINT i = 0; i < m_numUAVsWrite; i++)
	{
		ppUAViews[1] = m_ppWriteUAVs[i];
		
		// the first one of mixed multiple UAVs is append UAV
		if ( m_bMixedAppend && i == 0)
		{
			ppUAViews[2] = m_pGlobalCounterUAV;
			numUAVs = 3;
			offsets[0] = offsets[1] = offsets[2] = -1;
			m_pEffectiveContext->PSSetShader(m_pPSReadFromAppend, NULL, NULL);
		}
		else // the rest are counter UAVs
		{
			numUAVs = 2;
			offsets[0] = offsets[1] = -1;
			m_pEffectiveContext->PSSetShader(m_pPSReadFromCounter, NULL, NULL);
		}
		vp.Width = (float)m_ViewWidth;
		m_pEffectiveContext->RSSetViewports( 1, &vp );
		if (m_bUAVOnly)
		{
			m_pEffectiveContext->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL,
				0, numUAVs, ppUAViews, offsets);
		}
		else
		{
			m_pEffectiveContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &m_pRTView, NULL,
				1, numUAVs, ppUAViews, offsets);
		}
		m_pEffectiveContext->Draw(6, 0);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): ExecuteEffectiveContext() failed for UAV read,\
						   first pass.") );
			return RESULT_FAIL;
		}

		// verify copyStructureCount
		tr = VerifyStructureCount(m_ppWriteUAVs[i], 0);
		if ( tr == RESULT_FAIL )
		{
			WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): VerifyStructureCount() failed for UAV read, \
						   first pass.") );
			return RESULT_FAIL;
		}

		// verify data in result UAV
		// Data written by Append UAV is not verified for now
		if ( m_bMixedAppend && i == 0 )
			continue;
		tr = VerifyResultData(m_ppWriteUAVs[0]); // the window of data on the result UAV is the same as m_ppWriteUAVs[0]
		if ( tr == RESULT_FAIL )
		{
			WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): VerifyResultData() failed for UAV %d."), i );
			return RESULT_FAIL;
		}
	}

	// The second read pass: Use a different UAV from the one used for write
	m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pOutputUAV, clearVals );
	if ( m_readUAVType != MATCHING_READ_WRITE_UAVS )  // Using one big UAV covering the whole buffer to read out data
	{
		
		ppUAViews[1] = m_ppReadUAVs[0];
		if ( m_readUAVType == READ_ALL_UAVS_APPEND )
		{
			ppUAViews[2] = m_pGlobalCounterUAV;
			numUAVs = 3;
			offsets[0] = offsets[2] = -1;
			offsets[1] = m_bufferSize / sizeof(RECORD);
			m_pEffectiveContext->PSSetShader(m_pPSReadFromAppend, NULL, NULL);
		}
		else if ( m_readUAVType == READ_ALL_UAVS_COUNTER )
		{
			numUAVs = 2;
			offsets[0] = -1;
			offsets[1] = m_bufferSize / sizeof(RECORD);
			m_pEffectiveContext->PSSetShader(m_pPSReadFromCounter, NULL, NULL);
		}
		vp.Width = (float)(m_bufferSize / sizeof(RECORD));
		m_pEffectiveContext->RSSetViewports( 1, &vp );
		if (m_bUAVOnly)
		{
			m_pEffectiveContext->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL,
				0, numUAVs, ppUAViews, offsets);
		}
		else
		{
			m_pEffectiveContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &m_pRTView, NULL,
				1, numUAVs, ppUAViews, offsets);
		}
		m_pEffectiveContext->Draw(6, 0);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): ExecuteEffectiveContext() failed.") );
			return RESULT_FAIL;
		}

		// verify copyStructureCount
		tr = VerifyStructureCount(m_ppReadUAVs[0], 0);
		if ( tr == RESULT_FAIL )
		{
			WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): VerifyStructureCount() failed for UAV read, \
						   second pass.") );
			return RESULT_FAIL;
		}

		// verify data in result UAV
		for(UINT i = 0; i < m_numUAVsWrite; i++)
		{
			// Data written by Append UAV is not verified for now
			if ( m_bMixedAppend && i == 0 )
				continue;
			tr = VerifyResultData(m_ppWriteUAVs[i]);
			if ( tr == RESULT_FAIL )
			{
				WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): VerifyResultData() failed for UAV %d."), i );
				return RESULT_FAIL;
			}
		}
	}
	else // using multiple UAVs to read data, with matching type of the UAVs used for write
	{
		for (UINT i = 0; i < m_numUAVsRead; i++)
		{ 
			ppUAViews[1] = m_ppReadUAVs[i];
			// the first one of mixed multiple UAVs is append UAV
			if ( m_bMixedAppend && i == 0)
			{
				ppUAViews[2] = m_pGlobalCounterUAV;
				numUAVs = 3;
				offsets[0] = offsets[2] = -1;
				offsets[1] = ( m_UAVSize / m_sizeScale ) / sizeof(RECORD);
				m_pEffectiveContext->PSSetShader(m_pPSReadFromAppend, NULL, NULL);
			}
			else // the rest are counter UAVs
			{
				numUAVs = 2;
				offsets[0] = -1;
				offsets[1] = ( m_UAVSize / m_sizeScale ) / sizeof(RECORD);
				m_pEffectiveContext->PSSetShader(m_pPSReadFromCounter, NULL, NULL);
			}

			vp.Width = (float)m_ViewWidth;
			m_pEffectiveContext->RSSetViewports( 1, &vp );
			if (m_bUAVOnly)
			{
				m_pEffectiveContext->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL,
					0, numUAVs, ppUAViews, offsets);
			}
			else
			{
				m_pEffectiveContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &m_pRTView, NULL,
					1, numUAVs, ppUAViews, offsets);
			}
			m_pEffectiveContext->Draw(6, 0);
			if( FAILED( hr = ExecuteEffectiveContext() ) )
			{
				WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): ExecuteEffectiveContext() failed for UAV read,\
							   second pass.") );
				return RESULT_FAIL;
			}

			// verify copyStructureCount
			tr = VerifyStructureCount(m_ppReadUAVs[i], 0);
			if ( tr == RESULT_FAIL )
			{
				WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): VerifyStructureCount() failed for UAV read, \
							   second pass.") );
				return RESULT_FAIL;
			}
			// verify data in result UAV
			// Data written by Append UAV is not verified for now
			if ( m_bMixedAppend && i == 0 )
				continue;
			tr = VerifyResultData(m_ppReadUAVs[0]); // the window of data on the result UAV is the same as m_ppReadUAVs[0]
			if ( tr == RESULT_FAIL )
			{
				WriteToLog( _T("CCounterUAVPSTest::ExecuteTestCase(): VerifyResultData() failed for UAV %d."), i );
				return RESULT_FAIL;
			}
		}
	} //else
	return tr;

}

//------------------------------------------------------------------------------------------------
void CCounterUAVPSTest::CleanupTestCase()
{
	CCounterUAVTest::CleanupTestCase();

	
	SAFE_RELEASE(m_pPSWriteToCounter);
	SAFE_RELEASE(m_pPSWriteToAppend);
	SAFE_RELEASE(m_pPSReadFromCounter);
	SAFE_RELEASE(m_pPSReadFromAppend);

	SAFE_RELEASE(m_pRTTex2D);
	SAFE_RELEASE(m_pRTView);
}

//------------------------------------------------------------------------------------------------
void CCounterUAVPSTest::Cleanup()
{
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pVertexBuffer);
}