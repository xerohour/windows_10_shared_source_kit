#include "InputAssembler.h"
#include "d3d11on12.h"

//DrawAuto:
//    Draw more than size of SO buffer
//    Draw with SO Buffer with different InputLayout than used to fill buffer
//    Draw many times without resetting the SO Buffer
//    
//    
//    
//    
//    
const DRAW_PARAMS g_DrawParams[] = 
{
	{    1,             0},
	{    10,            0},
	{    10,            33},
	{    65540,         0},
	{    65540,         342},
	{    1,             65540},
	{    1,             -5},
};
const UINT g_DrawParamsSize = sizeof( g_DrawParams ) / sizeof( g_DrawParams[0] );

const DRAWINSTANCED_PARAMS g_DrawInstancedParams[] = 
{    //    VertexCount InstCount    StartVert    StartInst
	{    1,             10,           0,            0},
	{    10,            1,            0,            0},
	{    10,            10,           33,           0},
	{    10,            10,           0,            37},
	{    10,            10,           1,            1},
	{    10,            10,           45,           56},
	{    10,            10,           0,            -5},
	{    10,            10,           -5,           0},
	{    1,             65540,        0,            0},
	{    10,            65540,        0,            65539},
	{    65540,         1,            0,            0},
	{    65540,         10,           65539,        0},
};
const UINT g_DrawInstancedParamsSize = sizeof( g_DrawInstancedParams ) / sizeof( g_DrawInstancedParams[0] );

const DRAWINDEXED_PARAMS g_DrawIndexedParams[] = 
{    //    IndexCount    StartIndex            BaseVertex
	{    1,            0,                    0},
	{    1,            0,                    27},
	{    1,            0,                    MaxBufferSize - 1},
	{    1,            0,                    MaxBufferSize},
	{    10,            0,                    -5},
	{    10,            33,                    0},
	{    10,            0,                    1},
	{    10,            1,                    -15},
	{    10,            45,                    0},
	{    10,            300,                -300},
	{    10,            0,                    65539},
	{    10,            MaxBufferSize - 10,    0},
	{    10,            MaxBufferSize,        0},
	{    10,            -5,                    0},
	{    10,            0,                    INT_MAX - 5},
	{    65540,        0,                    0},
	{    65540,        65539,                0},
};
const UINT g_DrawIndexedParamsSize = sizeof( g_DrawIndexedParams ) / sizeof( g_DrawIndexedParams[0] );

const DRAWINDEXEDINSTANCED_PARAMS g_DrawIndexedInstancedParams[] = 
{    //    IndexCount    InstanceCount        StartIndex            BaseVertex            StartInstance
	{    1,            1,                    0,                    0,                    0},
	{    1,            10,                    1,                    0,                    0},
	{    10,           10,                    0,                    27,                    0},
	{    9,            11,                    0,                    -7,                    0},
	{    10,           1,                     0,                    1,                    31},
	{    300,        2,                    45,                    -301,                167},
	{    1,            1,                    0,                    0,                    MaxBufferSize - 1},
	{    1,            1,                    0,                    0,                    MaxBufferSize},
	{    1,            10,                    0,                    0,                    -5},
	{    10,            1,                    -5,                    0,                    0},
	{    10,            1,                    0,                    INT_MAX - 5,        0},
	{    10,            33,                    MaxBufferSize - 100,0,                    0},
	{    33,            10,                    0,                    MaxBufferSize - 300,0},
	{    65540,        1,                    0,                    0,                    0},
	{    16,            65540,                0,                    0,                    0},
};
const UINT g_DrawIndexedInstancedParamsSize = sizeof( g_DrawIndexedInstancedParams ) / sizeof( g_DrawIndexedInstancedParams[0] );


//The offsets must be aligned with the format stride, so in this case it's 4.
const UINT ZeroOffsetsValues[MaxInputElements] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const UINT SmallOffsetsValues[MaxInputElements] =
{
	4, 12, 4, 8, 16, 12, 4, 8, 12, 0, 16, 4, 4, 12, 0, 8, 12, 4, 16, 8, 8, 0, 4, 0, 4, 16, 12, 20, 12, 0, 4, 16
};

const UINT BigOffsetsValues[MaxInputElements] =
{
	30000, 36000, MaxBufferSize - 4, MaxBufferSize - 12, 252, MaxBufferSize, MaxBufferSize + 4, 65540, USHRT_MAX - 3, USHRT_MAX - 7, MaxBufferSize * 2, USHRT_MAX - 103, USHRT_MAX - 251, USHRT_MAX - 503, USHRT_MAX - 23, MaxBufferSize * 2 - 4,
	MaxBufferSize - 4, USHRT_MAX - 103, MaxBufferSize - 12, MaxBufferSize, MaxBufferSize + 4, 65540, USHRT_MAX - 3, 30000, 36000, USHRT_MAX - 7, MaxBufferSize * 2, USHRT_MAX - 251, USHRT_MAX - 503, USHRT_MAX - 23, MaxBufferSize * 2 - 4, 252
};

// Read from a buffer when not set (VB, IB) should return default value
// Also cover buffers too small. (Views and Constant buffers are covered in another test)

void CDrawTest::InitCommonTestParameters()
{
	CTestCaseParameter< InputClassCase > *pInputClassCaseParam = AddParameter( _T( "InputClassCase" ), &m_InputClassCase );
	CTestCaseParameter< BufferSizeCase > *pVBSizesCaseParam = AddParameter( _T( "VBSizesCase" ), &m_VBSizesCase );
	CTestCaseParameter< VBOffsetsCase > *pVBOffsetsCaseParam = AddParameter( _T( "VBOffsetsCase" ), &m_VBOffsetsCase );
	CTestCaseParameter< VBOffsetsCase > *pIBOffsetCaseParam = AddParameter( _T( "IBOffsetCase" ), &m_IBOffsetCase );
	CTestCaseParameter< BufferSizeCase > *pIBSizeCaseParam = AddParameter( _T( "IBSizeCase" ), &m_IBSizeCase );
	CTestCaseParameter< UINT > *pNumElements = AddParameter< UINT >( _T("NumElements"), &m_NumElements);

	CUserValueSet< BufferSizeCase > *pBufferSizeCaseValues = new CUserValueSet< BufferSizeCase >();
	pBufferSizeCaseValues->AddValue( MaxBufferSizeCase );
	pBufferSizeCaseValues->AddValue( SmallSizeCase );
	pBufferSizeCaseValues->AddValue( SizeRequiredByDrawCase );
	m_rVBSizesCase = AddParameterValueSet< BufferSizeCase >( pVBSizesCaseParam, pBufferSizeCaseValues );
	m_rIBSizeCase = AddParameterValueSet< BufferSizeCase >( pIBSizeCaseParam, pBufferSizeCaseValues );

	CUserValueSet< VBOffsetsCase > *pBufferOffsetCaseValues = new CUserValueSet< VBOffsetsCase >();
	pBufferOffsetCaseValues->AddValue( ZeroOffsets );
	pBufferOffsetCaseValues->AddValue( SmallOffsets );
	pBufferOffsetCaseValues->AddValue( BigOffsets );
	m_rVBOffsetsCase = AddParameterValueSet< VBOffsetsCase >( pVBOffsetsCaseParam, pBufferOffsetCaseValues );
	m_rIBOffsetCase = AddParameterValueSet< VBOffsetsCase >( pIBOffsetCaseParam, pBufferOffsetCaseValues );
	
	/////////////////////////////
	// Input Layouts

	// Only 3 different input layouts : All vertex, All instance, mixed
	// Only try a random instance data step rate to make sure it doesn't mess things.

	CUserValueSet< InputClassCase > *pNonInstancedInputClassCaseValues = new CUserValueSet< InputClassCase >();
	pNonInstancedInputClassCaseValues->AddValue( ICC_AllVertex );
	testfactor::RFactor rNonInstancedInputClassCase = AddParameterValueSet< InputClassCase >( pInputClassCaseParam, pNonInstancedInputClassCaseValues );

	CUserValueSet< InputClassCase > *pInstancedNoAlternateInputClassCaseValues = new CUserValueSet< InputClassCase >();
	pInstancedNoAlternateInputClassCaseValues->AddValue( ICC_AllInstance );
	pInstancedNoAlternateInputClassCaseValues->AddValue( ICC_AllVertex );
	testfactor::RFactor rInstancedNoAlternateInputClassCase = AddParameterValueSet< InputClassCase >( pInputClassCaseParam, pInstancedNoAlternateInputClassCaseValues );

	CUserValueSet< InputClassCase > *pInstancedInputClassCaseValues = new CUserValueSet< InputClassCase >();
	pInstancedInputClassCaseValues->AddValue( ICC_AllInstance );
	pInstancedInputClassCaseValues->AddValue( ICC_AllVertex );
	pInstancedInputClassCaseValues->AddValue( ICC_AlternateByVertex );
	testfactor::RFactor rInstancedInputClassCase = AddParameterValueSet< InputClassCase >( pInputClassCaseParam, pInstancedInputClassCaseValues );

	testfactor::RFactor rNumElements1 = AddParameterValue< UINT >( pNumElements, 1 );

	CUserValueSet< UINT > *pNumElementsValues = new CUserValueSet< UINT >();
	pNumElementsValues->AddValue( 3 );
	if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		pNumElementsValues->AddValue( 32 );
	else
		pNumElementsValues->AddValue( 16 );
  
	testfactor::RFactor rNumElementsMulti = AddParameterValueSet< UINT >( pNumElements, pNumElementsValues );

	//should not cycle alternate for 1 element
	m_rInstancedInputLayout = (rNumElements1 * rInstancedNoAlternateInputClassCase) + (rNumElementsMulti * rInstancedInputClassCase);
	m_rNonInstancedInputLayout = (rNumElements1 + rNumElementsMulti) * rNonInstancedInputClassCase;
}

void CDrawTest::InitTestParameters()
{
	// Initialize root factors
	CDrawTest::InitCommonTestParameters();

	// Create the parameters

	CTestCaseParameter< DrawMethod > *pDrawMethod = AddParameter( _T("DrawMethod"), &m_DrawMethod);

	CTestCaseParameter< UINT > *pVertexCount = AddParameter< UINT >( _T("VertexCount"), &m_VertexCount);
	CTestCaseParameter< UINT > *pStartVertexLocation = AddParameter< UINT >( _T("StartVertexLocation"), &m_StartVertexLocation);
	CTestCaseParameter< UINT > *pIndexCount = AddParameter< UINT >( _T("IndexCount"), &m_IndexCount);
	CTestCaseParameter< UINT > *pStartIndexLocation = AddParameter< UINT >( _T("StartIndexLocation"), &m_StartIndexLocation);
	CTestCaseParameter< INT > *pBaseVertexLocation = AddParameter< INT >( _T("BaseVertexLocation"), &m_BaseVertexLocation);
	CTestCaseParameter< UINT > *pInstanceCount = AddParameter< UINT >( _T("InstanceCount"), &m_InstanceCount);
	CTestCaseParameter< UINT > *pStartInstanceLocation = AddParameter< UINT >( _T("StartInstanceLocation"), &m_StartInstanceLocation);

	// Create the factors
	testfactor::RFactor rDrawMethod = AddParameterValue< DrawMethod >( pDrawMethod, Draw );
	testfactor::RFactor rDrawInstancedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawInstanced );
	testfactor::RFactor rDrawIndexedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawIndexed );
	testfactor::RFactor rDrawIndexedInstancedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawIndexedInstanced );
	testfactor::RFactor rDrawAutoMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawAuto );


	/////////////////////////////
	// Draw
	testfactor::RFactor rDrawVertexCount = AddParameterValueSet< UINT >( pVertexCount, new CTableValueSet<UINT>( &g_DrawParams[0].VertexCount, sizeof( DRAW_PARAMS ), g_DrawParamsSize ) );
	testfactor::RFactor rDrawStartVertexLocation = AddParameterValueSet< UINT >( pStartVertexLocation, new CTableValueSet<UINT>( &g_DrawParams[0].StartVertexLocation, sizeof( DRAW_PARAMS ), g_DrawParamsSize ) );
	testfactor::RFactor rDraw = rDrawMethod * m_rNonInstancedInputLayout * (rDrawVertexCount % rDrawStartVertexLocation);

	/////////////////////////////
	// DrawInstanced
	testfactor::RFactor rDrawInstancedVertexCount = AddParameterValueSet< UINT >( pVertexCount, new CTableValueSet<UINT>( &g_DrawInstancedParams[0].VertexCountPerInstance, sizeof( DRAWINSTANCED_PARAMS ), g_DrawInstancedParamsSize ) );
	testfactor::RFactor rDrawInstancedInstanceCount = AddParameterValueSet< UINT >( pInstanceCount, new CTableValueSet<UINT>( &g_DrawInstancedParams[0].InstanceCount, sizeof( DRAWINSTANCED_PARAMS ), g_DrawInstancedParamsSize ) );
	testfactor::RFactor rDrawInstancedStartVertexLocation = AddParameterValueSet< UINT >( pStartVertexLocation, new CTableValueSet<UINT>( &g_DrawInstancedParams[0].StartVertexLocation, sizeof( DRAWINSTANCED_PARAMS ), g_DrawInstancedParamsSize ) );
	testfactor::RFactor rDrawInstancedStartInstanceLocation = AddParameterValueSet< UINT >( pStartInstanceLocation, new CTableValueSet<UINT>( &g_DrawInstancedParams[0].StartInstanceLocation, sizeof( DRAWINSTANCED_PARAMS ), g_DrawInstancedParamsSize ) );
	testfactor::RFactor rDrawInstanced = rDrawInstancedMethod * m_rInstancedInputLayout * (rDrawInstancedVertexCount % rDrawInstancedInstanceCount % rDrawInstancedStartVertexLocation % rDrawInstancedStartInstanceLocation);

	/////////////////////////////
	// DrawIndexed
	testfactor::RFactor rDrawIndexedIndexCount = AddParameterValueSet< UINT >( pIndexCount, new CTableValueSet<UINT>( &g_DrawIndexedParams[0].IndexCount, sizeof( DRAWINDEXED_PARAMS ), g_DrawIndexedParamsSize ) );
	testfactor::RFactor rDrawIndexedStartIndexLocation = AddParameterValueSet< UINT >( pStartIndexLocation, new CTableValueSet<UINT>( &g_DrawIndexedParams[0].StartIndexLocation, sizeof( DRAWINDEXED_PARAMS ), g_DrawIndexedParamsSize ) );
	testfactor::RFactor rDrawIndexedBaseVertexLocation = AddParameterValueSet< INT >( pBaseVertexLocation, new CTableValueSet<INT>( &g_DrawIndexedParams[0].BaseVertexLocation, sizeof( DRAWINDEXED_PARAMS ), g_DrawIndexedParamsSize ) );
	testfactor::RFactor rDrawIndexed = rDrawIndexedMethod * m_rNonInstancedInputLayout * m_rIBSizeCase * m_rIBOffsetCase * (rDrawIndexedIndexCount % rDrawIndexedStartIndexLocation % rDrawIndexedBaseVertexLocation);
	//TODO: try a few 32bit format cases.

	/////////////////////////////
	// DrawIndexedInstanced
	testfactor::RFactor rDrawIndexedInstancedIndexCount = AddParameterValueSet< UINT >( pIndexCount, new CTableValueSet<UINT>( &g_DrawIndexedInstancedParams[0].IndexCountPerInstance, sizeof( DRAWINDEXEDINSTANCED_PARAMS ), g_DrawIndexedInstancedParamsSize ) );
	testfactor::RFactor rDrawIndexedInstancedInstanceCount = AddParameterValueSet< UINT >( pInstanceCount, new CTableValueSet<UINT>( &g_DrawIndexedInstancedParams[0].InstanceCount, sizeof( DRAWINDEXEDINSTANCED_PARAMS ), g_DrawIndexedInstancedParamsSize ) );
	testfactor::RFactor rDrawIndexedInstancedStartIndexLocation = AddParameterValueSet< UINT >( pStartIndexLocation, new CTableValueSet<UINT>( &g_DrawIndexedInstancedParams[0].StartIndexLocation, sizeof( DRAWINDEXEDINSTANCED_PARAMS ), g_DrawIndexedInstancedParamsSize ) );
	testfactor::RFactor rDrawIndexedInstancedBaseVertexLocation = AddParameterValueSet< INT >( pBaseVertexLocation, new CTableValueSet<INT>( &g_DrawIndexedInstancedParams[0].BaseVertexLocation, sizeof( DRAWINDEXEDINSTANCED_PARAMS ), g_DrawIndexedInstancedParamsSize ) );
	testfactor::RFactor rDrawIndexedInstancedStartInstanceLocation = AddParameterValueSet< UINT >( pStartInstanceLocation, new CTableValueSet<UINT>( &g_DrawIndexedInstancedParams[0].StartInstanceLocation, sizeof( DRAWINDEXEDINSTANCED_PARAMS ), g_DrawIndexedInstancedParamsSize ) );
	testfactor::RFactor rDrawIndexedInstanced = rDrawIndexedInstancedMethod * m_rInstancedInputLayout * m_rIBSizeCase * m_rIBOffsetCase * (rDrawIndexedInstancedIndexCount % rDrawIndexedInstancedInstanceCount % rDrawIndexedInstancedStartIndexLocation % rDrawIndexedInstancedBaseVertexLocation % rDrawIndexedInstancedStartInstanceLocation);

	SetRootTestFactor( ( rDraw + rDrawInstanced + rDrawIndexed + rDrawIndexedInstanced ) * m_rVBSizesCase * m_rVBOffsetsCase );	

	// Use priority patterns to "chip away" non priority 1 test cases
	AddPriorityPatternFilter(FilterNotEqual<UINT>(pVertexCount, 10), 0.5f); // Drawing 1 vertex is not very interesting, but drawing too many vertices is not needed
	AddPriorityPatternFilter(FilterGreater<UINT>(pStartVertexLocation, 1), 0.5f);

	AddPriorityPatternFilter(FilterNotEqual<UINT>(pIndexCount, 10), 0.5f);
	AddPriorityPatternFilter(FilterGreater<UINT>(pStartIndexLocation, 1), 0.5f);

	AddPriorityPatternFilter(FilterEqual<UINT>(pInstanceCount, 1), 0.5f);	// One instance is not very interesting
	AddPriorityPatternFilter(FilterEqual<UINT>(pInstanceCount, 10), 0.5f);	// Drawing too many instances is not needed, 2 should be sufficent for pri 1
	AddPriorityPatternFilter(FilterEqual<UINT>(pInstanceCount, 11), 0.5f);
	AddPriorityPatternFilter(FilterEqual<UINT>(pInstanceCount, 33), 0.5f);
	AddPriorityPatternFilter(FilterEqual<UINT>(pInstanceCount, 65540), 0.5f);
    AddPriorityPatternFilter(FilterGreater<UINT>(pStartInstanceLocation, 1), 0.5f);

	AddPriorityPatternFilter(FilterEqual<BufferSizeCase>(_T("VBSizesCase"), SmallSizeCase), 0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

//
TEST_RESULT CDrawTest::Setup()
{
	TEST_RESULT tr = CInputAssemblerTest::Setup();
	if (tr != RESULT_PASS)
		return tr;

	m_bVertexID = false;
	m_bInstanceID = false;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	for (UINT i=0; i<MaxInputElements; i++)
	{
		bufferDesc.ByteWidth = i + 2;

		m_pSmallVertexBuffers[i] = NULL;

		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = (void *)m_BufferData[i];
		subData.SysMemPitch = 0;
		subData.SysMemSlicePitch = 0;

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &m_pSmallVertexBuffers[i] ) ) )
		{
			WriteToLog( "CreateBuffer() failed" );
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

void CDrawTest::CleanupTestCase()
{
	m_pDeviceContext->VSSetShader( NULL, NULL, 0 );
	m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
	m_pDeviceContext->IASetInputLayout( NULL );
	m_pDeviceContext->SOSetTargets(0, NULL, NULL);
	
	HRESULT hr = S_OK;
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("CDrawTest::CleanupTestCase: Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
	}

	for (UINT i=0; i<MaxInputElements; i++)
	{
		SAFE_RELEASE( m_pDrawSizeVertexBuffers[i] );
	}

	for ( UINT i = 0; i < nMaxUAVs; i++ )
	{
		SAFE_RELEASE( m_pUnorderedAccessView[i] );
	}

	SAFE_RELEASE( m_pDrawIndirectArgumentBuffer );
	SAFE_RELEASE( m_pSOArgBuffer );
	SAFE_RELEASE( m_pUAVSrcBuf );
	SAFE_RELEASE( m_pDrawSizeIndexBuffer );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pStaging );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pVS );

	if(m_pDevice->GetDeviceRemovedReason() != S_OK)
	{
		CDrawTest::Cleanup();
	}
}

void CDrawTest::Cleanup()
{
	CInputAssemblerTest::Cleanup();    

	for (UINT i=0; i<MaxInputElements; i++)
	{
		SAFE_RELEASE( m_pSmallVertexBuffers[i] );
	}
}

//
HRESULT  CDrawTest::SetupArgumentBufferForDrawIndirect()
{
	HRESULT     hr = S_OK;
	UINT        pData[5] = {0};
	UINT        nElements = 0;

	
	// Create DrawIndirect Argument buffer
	D3D11_BUFFER_DESC       bufferDesc = {0};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = 0;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	bufferDesc.StructureByteStride = 0;
	
	switch (m_DrawMethod)
	{
		case DrawInstancedIndirect:
			bufferDesc.ByteWidth = 4 * sizeof(UINT) + m_nAlignedByteOffsetForArgs;
			break;
		case DrawIndexedInstancedIndirect:
			bufferDesc.ByteWidth = 5 * sizeof(UINT) + m_nAlignedByteOffsetForArgs;
			break;
	}

	D3D11_SUBRESOURCE_DATA  subresData = {0};
	UINT *pInitData = new UINT[ bufferDesc.ByteWidth/sizeof(UINT) ];
	ZeroMemory(pInitData, bufferDesc.ByteWidth);
	subresData.pSysMem = pInitData;

	// DrawIndirect arguments
	if ( m_DrawMethod == DrawInstancedIndirect )
	{
		pData[0] = m_VertexCount;
		pData[1] = m_InstanceCount;
		pData[2] = m_StartVertexLocation;
		pData[3] = m_StartInstanceLocation;
		nElements = 4;
	}
	else
	if ( m_DrawMethod == DrawIndexedInstancedIndirect )
	{
		pData[0] = m_IndexCount;
		pData[1] = m_InstanceCount;
		pData[2] = m_StartIndexLocation;
		pData[3] = m_BaseVertexLocation;
		pData[4] = m_StartInstanceLocation;
		nElements = 5;
	}
	else
	{
		WriteToLog( "CDrawTest::SetupArgumentBufferForDrawIndirect(): Wrong Draw method." );
		goto Exit;
	}


	// Ideally another draw call should be used to fill the argument data for the scenario of using
	// Steamout buffer or Rendertarget buffer as the drawindirect argument buffer
	if ( m_DrawIndirectType == StreamOutputBuffer || m_DrawIndirectType == RenderTargetBuffer )
	{
		switch ( m_DrawIndirectType )
		{
			case StreamOutputBuffer:
				bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
				break;

			case RenderTargetBuffer:
				bufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
				break;
		}
		UINT UintOffsetForArgs = m_nAlignedByteOffsetForArgs / sizeof(UINT);
		for (UINT i = 0; i < nElements; i++)
			pInitData[UintOffsetForArgs + i] = pData[i];
	}

	if (FAILED(GetFramework()->CreateBufferTiledOptional(&bufferDesc, &subresData, &m_pDrawIndirectArgumentBuffer)))
	{
		WriteToLog( "CreateBuffer(m_pDrawIndirectArgumentBuffer) failed" );
		goto Exit;
	}

	// creating staging buffer for the argument buffer
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.BindFlags = 0;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.MiscFlags = 0;
	if ( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pStaging ) ) )
	{
		WriteToLog( _T("%s in %s at line %d.  FAILED: m_pDevice->CreateBuffer() failed - hr = %s"), __FUNCTION__, __FILE__, __LINE__, D3DHResultToString(hr).c_str() );
		goto Exit;
	}

	switch ( m_DrawIndirectType )
	{
		case UpdateSubresource:
		{
			D3D11_BOX udpateArea = { m_nAlignedByteOffsetForArgs, 0, 0, 
									 m_nAlignedByteOffsetForArgs + nElements * sizeof(UINT), 1, 1};
			m_pDeviceContext->UpdateSubresource(m_pDrawIndirectArgumentBuffer, 0, &udpateArea, pData, nElements * sizeof(UINT), 0);
			break;
		}

		case CopyStructureCount:
		{
			if ( FAILED( hr = SetupUnorderedAccessView( pData, nElements ) ) )
			{
				WriteToLog( _T("%s in %s at line %d.  FAILED: SetupUnorderedAccessView() failed"), __FUNCTION__, __FILE__, __LINE__ );
				goto Exit;
			}
			for ( UINT i = 0; i < nElements; i++ )
			{
				m_pDeviceContext->CopyStructureCount( m_pDrawIndirectArgumentBuffer, 
					i * sizeof(UINT) + m_nAlignedByteOffsetForArgs, m_pUnorderedAccessView[i] ); 
			}

			break;
		}

		case StreamOutputBuffer:
		case RenderTargetBuffer:	
			break;
	}
	if ( FAILED( hr = ValidateArgumentBuffer( pData, nElements ) ) )
	{
		WriteToLog( _T("%s in %s at line %d.  FAILED: ValidateArgumentBuffer() failed"), __FUNCTION__, __FILE__, __LINE__ );
		goto Exit;
	}

  
Exit:
	delete[] pInitData;
	return hr;
}

//
HRESULT  CDrawTest::SetupUnorderedAccessView( UINT* pData, UINT nElements )
{
	HRESULT                             hr = E_FAIL;
	D3D11_UNORDERED_ACCESS_VIEW_DESC    uavDesc;
	D3D11_BUFFER_DESC                   bufDesc = {0};

	// UAV Buffer
	bufDesc.ByteWidth = nElements * sizeof(*pData);
	bufDesc.Usage = D3D11_USAGE_DEFAULT; 
	bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufDesc.StructureByteStride = sizeof(UINT);

	if ( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &bufDesc, NULL, &m_pUAVSrcBuf ) ) )
	{
		WriteToLog( _T("%s in %s at line %d.  FAILED: m_pDevice->CreateBuffer() failed - hr = %s"), __FUNCTION__, __FILE__, __LINE__, D3DHResultToString(hr).c_str() );
		goto Exit;
	}
	
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = (D3D11_BUFFER_UAV_FLAG)m_nBufferUAVFlag;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = 1;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	for ( int i = 0; i < (int)nElements; i++ )
	{
		if ( FAILED( hr = m_pDevice->CreateUnorderedAccessView( m_pUAVSrcBuf, &uavDesc, &m_pUnorderedAccessView[i] ) ) )
		{
			WriteToLog( _T("%s in %s at line %d.  FAILED: CreateUnorderedAccessView() failed - hr = %s"), __FUNCTION__, __FILE__, __LINE__, D3DHResultToString(hr).c_str() );
			goto Exit;
		}

		m_pDeviceContext->CSSetUnorderedAccessViews( 0, 1, &m_pUnorderedAccessView[i], &pData[i] );
	}
  
Exit:
	return hr;
}

//
HRESULT  CDrawTest::ValidateArgumentBuffer( UINT* pData, UINT nElements )
{
	HRESULT                     hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE    OutMapData = {0};

	m_pDeviceContext->CopyResource( m_pStaging, m_pDrawIndirectArgumentBuffer);

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute command list.  hr = %s"), D3DHResultToString(hr).c_str() );
		goto Exit;
	}

	if ( FAILED( hr = GetImmediateContext()->Map( m_pStaging, 0, D3D11_MAP_READ, 0, &OutMapData ) ) )
	{
		WriteToLog( _T("%s in %s at line %d.  FAILED: m_pDeviceContext->Map() failed - hr = %s"), __FUNCTION__, __FILE__, __LINE__, D3DHResultToString(hr).c_str() );
		goto Exit;
	}

	UINT UintOffsetForArgs = m_nAlignedByteOffsetForArgs / sizeof(UINT);
	for ( int i = 0; i < (int)nElements; i++ )
	{
		if ( ((PUINT)(PVOID)OutMapData.pData)[i + UintOffsetForArgs] != pData[i] )
		{
			WriteToLog( _T("%s in %s at line %d.  FAILED: Argument Buffer element is invalid - expecting 0x%08x but found 0x%08x at data element %d"),
						__FUNCTION__, __FILE__, __LINE__, pData[i], ((PUINT)(PVOID)OutMapData.pData)[i + UintOffsetForArgs], i );
			hr = E_FAIL;
			GetImmediateContext()->Unmap( m_pStaging, 0 );
			goto Exit;
		}
	}

	GetImmediateContext()->Unmap( m_pStaging, 0 );

Exit:
	return hr;
}

bool CDrawTest::IsNoVBBound()
{
    // Threshold bug 2237122: when 11on12 is used, large offsets => no VB bound
    if (m_VBOffsetsCase == BigOffsets)
    {
        ID3D11On12Device* pDevice11on12 = nullptr;
        if (SUCCEEDED(GetDevice()->QueryInterface(&pDevice11on12)))
        {
            pDevice11on12->Release();
            return true;
        }
    }
    return false;
}


//
TEST_RESULT  CDrawTest::ExecuteTestCase()
{
	TEST_RESULT     tRes = RESULT_PASS;
	HRESULT         hr = E_FAIL;

	if( GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0 && m_NumElements > 16 )
		return RESULT_SKIP;

	ZeroMemory( m_InputElements, sizeof(m_InputElements) );

	for (UINT i=0; i<m_NumElements; i++)
	{
		m_InputElements[i].InputSlotClass = InputClassLayouts[m_InputClassCase][i];
		m_InputElements[i].InputSlot = i;

		m_InputElements[i].SemanticName = szSemantic;
		m_InputElements[i].SemanticIndex = i;

		m_InputElements[i].Format = DXGI_FORMAT_R32_UINT;
	}

	m_LayoutInfo.InitializeInputElements(m_InputElements, m_NumElements);

	//Figure the offset from the case and format and inputslot
	//UINT currentOffset = 0;
	for (UINT iSlot=0; iSlot<m_LayoutInfo.maxSlot; iSlot++)
	{
		UINT stride = m_LayoutInfo.slots[iSlot].stride;
		UINT numElementsInSlot = 0;

		UINT shuffledElements[MaxInputElements];
		ZeroMemory(shuffledElements, sizeof(shuffledElements));
		ShuffleElements(GetFramework()->GetCurrentTestCase(), m_LayoutInfo.slots[iSlot].numElements, shuffledElements);

		for (UINT j=0; j < m_LayoutInfo.slots[iSlot].numElements; j++)
		{
			UINT iElement = m_LayoutInfo.slots[iSlot].elementsIndex[j];
			UINT iShuffledElement = m_LayoutInfo.slots[iSlot].elementsIndex[shuffledElements[j]];

			assert (m_InputElements[iElement].InputSlot == iSlot);
			assert (m_InputElements[iShuffledElement].InputSlot == iSlot);

			//All elements in same slot must have same InstanceDataStepRate;
			if (m_InputElements[iElement].InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA)
				m_InputElements[iElement].InstanceDataStepRate = 0;
			else if (m_InputElements[iElement].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA)
				m_InputElements[iElement].InstanceDataStepRate = InstanceDataStepRateElements[IDSRC_Random][iSlot];

			//case ABOC_MatchFormat:
			//Need to add format stride for previous element with same input slot
			if ( j > 0 )
			{
				UINT iPrevElement = m_LayoutInfo.slots[iSlot].elementsIndex[j-1];
				assert (m_InputElements[iPrevElement].InputSlot == iSlot);

				m_InputElements[iElement].AlignedByteOffset = m_InputElements[iPrevElement].AlignedByteOffset + GetFormatStrideInBytes(m_InputElements[iPrevElement].Format);
			} 
		}  
	}

	UINT  numDrawElements = 0;

	switch (m_DrawMethod )
	{
		case Draw:
			numDrawElements = 1 * m_VertexCount;
			break;
		case DrawInstanced:
		case DrawInstancedIndirect:
			numDrawElements = m_InstanceCount * m_VertexCount;
			break;
		case DrawIndexed:
			numDrawElements = 1 * m_IndexCount;
			break;
		case DrawIndexedInstanced:
		case DrawIndexedInstancedIndirect:
			numDrawElements = m_InstanceCount * m_IndexCount;
			break;
	}

	// Set the layout data
	m_LayoutInfo.SetLayoutData(m_bVertexID, m_bInstanceID, numDrawElements);

	// Setup the shaders and inputlayout
	if ( RESULT_PASS != SetupPipeline(false) )
		goto TESTFAIL;

	// Create buffer for argument if needed
	if (m_DrawMethod == DrawInstancedIndirect || m_DrawMethod == DrawIndexedInstancedIndirect)
	{
		// Setup argument buffer for Draw****Indirect - function no-ops for other draw method types
		if ( FAILED( hr = SetupArgumentBufferForDrawIndirect() ) )
		{
			WriteToLog( _T("%s in %s at line %d.  FAILED: SetupArgumentBufferForDrawIndirect() failed"), __FUNCTION__, __FILE__, __LINE__ );
			goto TESTFAIL;
		}	
	}

	// Bind the index buffer
	switch (m_DrawMethod )
	{
		case Draw:
		case DrawInstanced:
		case DrawInstancedIndirect:
			m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
			m_LayoutInfo.IBBoundSize = 0;
			m_LayoutInfo.IBOffset = 0;
			m_LayoutInfo.IBStride = 0;
			m_LayoutInfo.pIBData = NULL;
			break;

		case DrawIndexed:
		case DrawIndexedInstanced:
		case DrawIndexedInstancedIndirect:
			m_LayoutInfo.IBStride = sizeof(WORD);
			m_LayoutInfo.pIBData = m_IndexBufferData;

			//select the Index buffer size and offset
			switch (m_IBOffsetCase)
			{
				case ZeroOffsets:
					m_LayoutInfo.IBOffset = 0;
					break;

				case SmallOffsets:
					m_LayoutInfo.IBOffset = 2;
					break;

				case BigOffsets:
					m_LayoutInfo.IBOffset = 40000;
					break;
			}

			ID3D11Buffer  *pIB = NULL;

			switch (m_IBSizeCase)
			{
				case MaxBufferSizeCase:
					m_LayoutInfo.IBBoundSize = MaxBufferSize;
					pIB = m_pIndexBufferBig;
					break;

				case SmallSizeCase:
					m_LayoutInfo.IBBoundSize = 1;
					pIB = m_pIndexBufferSmall;
					break;

				case SizeRequiredByDrawCase:
					//TODO: Need to fill this correctly
					m_pDrawSizeIndexBuffer = NULL;

					D3D11_BUFFER_DESC bufferDesc;
					bufferDesc.ByteWidth = m_IndexCount * sizeof(WORD);
					bufferDesc.Usage = D3D11_USAGE_DEFAULT;
					bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bufferDesc.CPUAccessFlags = 0;
					bufferDesc.MiscFlags = 0;
					D3D11_SUBRESOURCE_DATA subData;
					subData.pSysMem = (void *)m_IndexBufferData;
					subData.SysMemPitch = 0;
					subData.SysMemSlicePitch = 0;

					if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &m_pDrawSizeIndexBuffer ) ) )
					{
						WriteToLog( "CreateBuffer(m_pDrawSizeIndexBuffer) failed" );
						goto TESTFAIL;
					}

					m_LayoutInfo.IBBoundSize = m_IndexCount;
					pIB = m_pDrawSizeIndexBuffer;
					break;
			}
			m_pDeviceContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R16_UINT, m_LayoutInfo.IBOffset);
			break;
	}

	//Set vertex buffers
	const UINT *pOffsets = NULL;
	switch (m_VBOffsetsCase)
	{
		case ZeroOffsets:
			pOffsets = ZeroOffsetsValues;
			break;

		case SmallOffsets:
			pOffsets = SmallOffsetsValues;
			break;

		case BigOffsets:
			pOffsets = BigOffsetsValues;
			break;
	}
	for (UINT i=0; i<MaxInputElements; i++)
	{
		m_LayoutInfo.slots[i].VBOffset = pOffsets[i];
		m_LayoutInfo.slots[i].pVBData = m_BufferData[i];
	}

	ID3D11Buffer **ppBuffers = NULL;

	switch (m_VBSizesCase)
	{
		case MaxBufferSizeCase:
			for (UINT i=0; i<MaxInputElements; i++)
				m_LayoutInfo.slots[i].VBBoundSize = MaxBufferSize;
			ppBuffers = m_pVertexBuffers;
			break;

		case SmallSizeCase:
			for (UINT i=0; i<MaxInputElements; i++)
				m_LayoutInfo.slots[i].VBBoundSize = i + 2;
			ppBuffers = m_pSmallVertexBuffers;
			break;

		case SizeRequiredByDrawCase:
			for (UINT i=0; i<MaxInputElements; i++)
			{
				if (m_LayoutInfo.slots[i].numElements == 0)
					continue;

				//number of vertices or instance * stride
				UINT size = 0;
				if (m_InputElements[i].InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA)
				{
					if (m_DrawMethod == DrawIndexed || m_DrawMethod == DrawIndexedInstanced || m_DrawMethod == DrawIndexedInstancedIndirect)
					{
						//For drawindexed, use Max Index used.
						size = m_LayoutInfo.slots[i].stride * USHRT_MAX;
					}
					else
					{
						size = m_LayoutInfo.slots[i].stride * m_VertexCount;
					}
				}
				else
				{
					size = m_LayoutInfo.slots[i].stride * m_InstanceCount;
				}
				m_LayoutInfo.slots[i].VBBoundSize = size;

				m_pDrawSizeVertexBuffers[i] = NULL;

				D3D11_BUFFER_DESC bufferDesc;
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = 0;
				bufferDesc.MiscFlags = 0;
				bufferDesc.ByteWidth = size;

				D3D11_SUBRESOURCE_DATA subData;
				subData.pSysMem = (void *)m_BufferData[i];
				subData.SysMemPitch = 0;
				subData.SysMemSlicePitch = 0;

				if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &m_pDrawSizeVertexBuffers[i] ) ) )
				{
					WriteToLog( "CreateBuffer() failed" );
					goto TESTFAIL;
				}
			}
			ppBuffers = m_pDrawSizeVertexBuffers;
			break;
	}

	UINT    Strides[MaxInputElements] = {0};
	for ( UINT i = 0 ; i < MaxInputElements; i++ )
	{
		Strides[i] = m_LayoutInfo.slots[i].stride;
	}

	m_pDeviceContext->IASetVertexBuffers( 0, m_NumElements, ppBuffers, Strides, pOffsets );
  
	//Start the query for pipeline stats.  It's ended in VerifyDraw*Results.
	//Win8:225466 - The query end is after the effective context is executed
	GetEffectiveContext()->Begin(m_pQuery);

	//Draw and verify results
	bool bDrawResults = false;

	switch (m_DrawMethod)
	{
		default:
			assert(!"Unknown draw method.");
			break;

		case Draw:
			m_pDeviceContext->Draw(m_VertexCount, m_StartVertexLocation);
			bDrawResults = VerifyDrawResults(m_VertexCount, m_StartVertexLocation);
			break;

		case DrawInstanced:
			m_pDeviceContext->DrawInstanced(m_VertexCount, m_InstanceCount, m_StartVertexLocation, m_StartInstanceLocation);
			bDrawResults = VerifyDrawInstancedResults(m_VertexCount, m_InstanceCount, m_StartVertexLocation, m_StartInstanceLocation);
			break;

		case DrawInstancedIndirect:
			m_pDeviceContext->DrawInstancedIndirect(m_pDrawIndirectArgumentBuffer, m_nAlignedByteOffsetForArgs);
			bDrawResults = VerifyDrawInstancedResults(m_VertexCount, m_InstanceCount, m_StartVertexLocation, m_StartInstanceLocation);
			break;

		case DrawIndexed:
			m_pDeviceContext->DrawIndexed(m_IndexCount,m_StartIndexLocation, m_BaseVertexLocation);
			bDrawResults = VerifyDrawIndexedResults(m_IndexCount,m_StartIndexLocation, m_BaseVertexLocation);
			break;

		case DrawIndexedInstanced:
			m_pDeviceContext->DrawIndexedInstanced(m_IndexCount, m_InstanceCount, m_StartIndexLocation, m_BaseVertexLocation, m_StartInstanceLocation);
			bDrawResults = VerifyDrawIndexedInstancedResults(m_IndexCount, m_InstanceCount, m_StartIndexLocation, m_BaseVertexLocation, m_StartInstanceLocation);
			break;

		case DrawIndexedInstancedIndirect:
			m_pDeviceContext->DrawIndexedInstancedIndirect(m_pDrawIndirectArgumentBuffer, m_nAlignedByteOffsetForArgs);
			bDrawResults = VerifyDrawIndexedInstancedResults(m_IndexCount, m_InstanceCount, m_StartIndexLocation, m_BaseVertexLocation, m_StartInstanceLocation);
			break;
	}

	if (!bDrawResults)
	{
		goto TESTFAIL;
	}

	goto testDone;

TESTFAIL:
	tRes = RESULT_FAIL;

testDone:

	m_pDeviceContext->VSSetShader( NULL, NULL, 0 );
	m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
	m_pDeviceContext->IASetInputLayout( NULL );
	m_pDeviceContext->SOSetTargets(0, NULL, NULL);
	

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	for (UINT i=0; i<MaxInputElements; i++)
	{
		SAFE_RELEASE( m_pDrawSizeVertexBuffers[i] );
	}

	for ( UINT i = 0; i < nMaxUAVs; i++ )
	{
		SAFE_RELEASE( m_pUnorderedAccessView[i] );
	}

	SAFE_RELEASE( m_pDrawIndirectArgumentBuffer );
	SAFE_RELEASE( m_pSOArgBuffer );
	SAFE_RELEASE( m_pUAVSrcBuf );
	SAFE_RELEASE( m_pDrawSizeIndexBuffer );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pStaging );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pVS );

	return tRes;
}


//-------------------------------------------------------------------------------------------------------------
void CDrawIndirectTest::InitTestParameters()
{
	// Initialize root factors
	CDrawTest::InitCommonTestParameters();

	// Create the parameters
	CTestCaseParameter< DrawMethod > *pDrawMethod = AddParameter( _T("DrawMethod"), &m_DrawMethod);

	CTestCaseParameter< UINT > *pVertexCount = AddParameter< UINT >( _T("VertexCount"), &m_VertexCount);
	CTestCaseParameter< UINT > *pStartVertexLocation = AddParameter< UINT >( _T("StartVertexLocation"), &m_StartVertexLocation);
	CTestCaseParameter< UINT > *pIndexCount = AddParameter< UINT >( _T("IndexCount"), &m_IndexCount);
	CTestCaseParameter< UINT > *pStartIndexLocation = AddParameter< UINT >( _T("StartIndexLocation"), &m_StartIndexLocation);
	CTestCaseParameter< INT > *pBaseVertexLocation = AddParameter< INT >( _T("BaseVertexLocation"), &m_BaseVertexLocation);
	CTestCaseParameter< UINT > *pInstanceCount = AddParameter< UINT >( _T("InstanceCount"), &m_InstanceCount);
	CTestCaseParameter< UINT > *pStartInstanceLocation = AddParameter< UINT >( _T("StartInstanceLocation"), &m_StartInstanceLocation);

	CTestCaseParameter< DrawIndirectType > *pDrawIndirectType = AddParameter( _T( "DrawIndirectType" ), &m_DrawIndirectType );
	CTestCaseParameter< UINT > *pBufferUAVFlag = AddParameter( _T( "BufferUAVFlag" ), &m_nBufferUAVFlag );
	CTestCaseParameter< UINT > *pAlignedByteOffsetPara = AddParameter( _T("AlignedByteOffsetForArgs"), &m_nAlignedByteOffsetForArgs );
	
	// Create the factors
	testfactor::RFactor rDrawInstancedIndirectMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawInstancedIndirect );
    testfactor::RFactor rDrawIndexedInstancedIndirectMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawIndexedInstancedIndirect );

	testfactor::RFactor  rDrawIndirectType[2];
	CUserValueSet< DrawIndirectType > *pDrawIndirectTypeValues = new CUserValueSet< DrawIndirectType >();
	pDrawIndirectTypeValues->AddValue( UpdateSubresource );
	pDrawIndirectTypeValues->AddValue( StreamOutputBuffer );
	pDrawIndirectTypeValues->AddValue( RenderTargetBuffer );
	rDrawIndirectType[0] = AddParameterValueSet< DrawIndirectType >( pDrawIndirectType, pDrawIndirectTypeValues );
	rDrawIndirectType[1] = AddParameterValue( pDrawIndirectType, CopyStructureCount );

	CUserValueSet< UINT > *pBufferUAVFlagValues = new CUserValueSet< UINT >();
	pBufferUAVFlagValues->AddValue( (UINT)D3D11_BUFFER_UAV_FLAG_APPEND );
	pBufferUAVFlagValues->AddValue( (UINT)D3D11_BUFFER_UAV_FLAG_COUNTER );
	testfactor::RFactor  rBufferUAVFlag = AddParameterValueSet< UINT >( pBufferUAVFlag, pBufferUAVFlagValues );
	testfactor::RFactor  rBufferUAVFlagZero = AddParameterValue< UINT >( pBufferUAVFlag, 0 );
	testfactor::RFactor  rDrawIndirectTypeAll = rDrawIndirectType[0] * rBufferUAVFlagZero + rDrawIndirectType[1] * rBufferUAVFlag;

	CUserValueSet< UINT > *pByteOffsetForArgs = new CUserValueSet< UINT >;
	pByteOffsetForArgs->AddValue(0);
	pByteOffsetForArgs->AddValue(4);
	pByteOffsetForArgs->AddValue(16000);
	testfactor::RFactor rfByteOffsetArgs = AddParameterValueSet< UINT >( pAlignedByteOffsetPara, pByteOffsetForArgs );

	// only use 1/4  of the test cases in tables g_DrawInstancedParams and g_DrawIndexedInstancedParams, because they 
	// have been thoroughly executed in the direct version of the draw calls. DrawIndirect test cases put emphasis on other aspects.
	/////////////////////////////
	// DrawInstancedIndirect
	testfactor::RFactor rDrawInstancedVertexCount = AddParameterValueSet< UINT >( pVertexCount, 
		new CTableValueSet<UINT>( &g_DrawInstancedParams[0].VertexCountPerInstance, sizeof( DRAWINSTANCED_PARAMS ) * 4, g_DrawInstancedParamsSize / 4 ) );
	testfactor::RFactor rDrawInstancedInstanceCount = AddParameterValueSet< UINT >( pInstanceCount, 
		new CTableValueSet<UINT>( &g_DrawInstancedParams[0].InstanceCount, sizeof( DRAWINSTANCED_PARAMS ) * 4, g_DrawInstancedParamsSize / 4 ) );
	testfactor::RFactor rDrawInstancedStartVertexLocation = AddParameterValueSet< UINT >( pStartVertexLocation, 
		new CTableValueSet<UINT>( &g_DrawInstancedParams[0].StartVertexLocation, sizeof( DRAWINSTANCED_PARAMS ) * 4, g_DrawInstancedParamsSize / 4) );
	testfactor::RFactor rDrawInstancedStartInstanceLocation = AddParameterValueSet< UINT >( pStartInstanceLocation, 
		new CTableValueSet<UINT>( &g_DrawInstancedParams[0].StartInstanceLocation, sizeof( DRAWINSTANCED_PARAMS ) * 4, g_DrawInstancedParamsSize / 4) );
	testfactor::RFactor rDrawInstancedIndirect = rDrawInstancedIndirectMethod * m_rInstancedInputLayout * 
		(rDrawInstancedVertexCount % rDrawInstancedInstanceCount % rDrawInstancedStartVertexLocation % rDrawInstancedStartInstanceLocation);

	/////////////////////////////
	// DrawIndexedInstancedIndirect
	testfactor::RFactor rDrawIndexedInstancedIndexCount = AddParameterValueSet< UINT >( pIndexCount, 
		new CTableValueSet<UINT>( &g_DrawIndexedInstancedParams[0].IndexCountPerInstance, sizeof( DRAWINDEXEDINSTANCED_PARAMS ) * 4, g_DrawIndexedInstancedParamsSize /4 ) );
	testfactor::RFactor rDrawIndexedInstancedInstanceCount = AddParameterValueSet< UINT >( pInstanceCount, 
		new CTableValueSet<UINT>( &g_DrawIndexedInstancedParams[0].InstanceCount, sizeof( DRAWINDEXEDINSTANCED_PARAMS ) * 4, g_DrawIndexedInstancedParamsSize /4 )  );
	testfactor::RFactor rDrawIndexedInstancedStartIndexLocation = AddParameterValueSet< UINT >( pStartIndexLocation, 
		new CTableValueSet<UINT>( &g_DrawIndexedInstancedParams[0].StartIndexLocation, sizeof( DRAWINDEXEDINSTANCED_PARAMS ) * 4, g_DrawIndexedInstancedParamsSize /4 )  );
	testfactor::RFactor rDrawIndexedInstancedBaseVertexLocation = AddParameterValueSet< INT >( pBaseVertexLocation, 
		new CTableValueSet<INT>( &g_DrawIndexedInstancedParams[0].BaseVertexLocation, sizeof( DRAWINDEXEDINSTANCED_PARAMS ) * 4, g_DrawIndexedInstancedParamsSize /4 )  );
	testfactor::RFactor rDrawIndexedInstancedStartInstanceLocation = AddParameterValueSet< UINT >( pStartInstanceLocation, 
		new CTableValueSet<UINT>( &g_DrawIndexedInstancedParams[0].StartInstanceLocation, sizeof( DRAWINDEXEDINSTANCED_PARAMS ) * 4, g_DrawIndexedInstancedParamsSize /4 )  );
	testfactor::RFactor rDrawIndexedInstancedIndirect = rDrawIndexedInstancedIndirectMethod * m_rInstancedInputLayout * ( m_rIBSizeCase % m_rIBOffsetCase )
									* ( rDrawIndexedInstancedIndexCount % rDrawIndexedInstancedInstanceCount
										% rDrawIndexedInstancedStartIndexLocation % rDrawIndexedInstancedBaseVertexLocation
										% rDrawIndexedInstancedStartInstanceLocation);
	        
	SetRootTestFactor( ( rDrawInstancedIndirect + rDrawIndexedInstancedIndirect ) * m_rVBSizesCase * m_rVBOffsetsCase * rDrawIndirectTypeAll * rfByteOffsetArgs ) ;

	// Use priority patterns to "chip away" non priority 1 test cases
	AddPriorityPatternFilter(FilterGreater<UINT>(pVertexCount, 10), 0.5f); // Drawing 1 vertex is not very interesting, but drawing too many vertices is not needed
	AddPriorityPatternFilter(FilterGreater<UINT>(pStartVertexLocation, 1), 0.5f);

	AddPriorityPatternFilter(FilterNotEqual<UINT>(pIndexCount, 10), 0.5f);
	AddPriorityPatternFilter(FilterGreater<UINT>(pStartIndexLocation, 1), 0.5f);

	AddPriorityPatternFilter(FilterEqual<UINT>(pInstanceCount, 1), 0.5f);	// One instance is not very interesting
	AddPriorityPatternFilter(FilterEqual<UINT>(pInstanceCount, 65540), 0.5f);	// Drawing too many instances is not needed, 2 should be sufficent for pri 1
    AddPriorityPatternFilter(FilterGreater<UINT>(pStartInstanceLocation, 1), 0.5f);

	AddPriorityPatternFilter(FilterEqual<BufferSizeCase>(_T("VBSizesCase"), SmallSizeCase), 0.5f);

    AddPriorityPatternFilter(FilterEqual<UINT>(pAlignedByteOffsetPara, 16000), 0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.00f );
}

//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CDrawIndirectTest::Setup()
{
	TEST_RESULT tr = CDrawTest::Setup();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CDrawTest::Setup() failed"));
		return tr;
	}

	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		WriteToLog(_T("Skip this group on feature levels less than 11_0."));
		return RESULT_SKIP;
	}

	return tr;

}

TEST_RESULT CDrawIndirectTest::SetupTestCase()
{
	// Skip test cases with non-zero byte offset for UpdateSubresource on deferred context for now because of 
	// runtime bug 691458, which is a won't fix for windows7
	if ( m_nAlignedByteOffsetForArgs != 0 
		&& m_DrawIndirectType == UpdateSubresource 
		&& m_pDeviceContext->GetType() ==  D3D11_DEVICE_CONTEXT_DEFERRED )
		return RESULT_SKIP;

	return RESULT_PASS;
}
