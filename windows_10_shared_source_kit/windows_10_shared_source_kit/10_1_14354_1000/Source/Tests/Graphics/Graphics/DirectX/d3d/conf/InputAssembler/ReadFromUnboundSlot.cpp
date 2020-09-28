#include "InputAssembler.h"

//Read from a buffer when not set (VB, IB) should return default value
//Also cover buffers too small. (Views and Constant buffers are covered in another test)

void CUnboundBuffersTest::InitTestParameters()
{
	//Initialize root factors
	CInputAssemblerTest::InitTestParameters();

	//Create the parameters

	CTestCaseParameter< InputClassCase > *pInputClassCaseParam = AddParameter( _T( "InputClassCase" ), &m_InputClassCase );
	CTestCaseParameter< UINT > *pNumElements = AddParameter< UINT >( _T("NumElements"), &m_NumElements);

	CTestCaseParameter< DrawMethod > *pDrawMethod = AddParameter( _T("DrawMethod"), &m_DrawMethod);

	CTestCaseParameter< bool > *pUseOffsetInDraw = AddParameter< bool >( _T("UseOffsetInDraw"), &m_bUseOffsetInDraw);

	CTestCaseParameter< bool > *pIBBound = AddParameter< bool >( _T("IBBound"), &m_bIBBound);

	CTestCaseParameter< bool > *pNoVBBound = AddParameter< bool >( _T("NoVBBound"), &m_bNoVBBound);

	//Create the factors

	//Only 3 different input layouts : All vertex, All instance, mixed
	//Only try a random instance data step rate to make sure it doesn't mess things.

	CUserValueSet< InputClassCase > *pInputClassCaseValues = new CUserValueSet< InputClassCase >();
	pInputClassCaseValues->AddValue( ICC_AlternateByVertex );
	pInputClassCaseValues->AddValue( ICC_AllInstance );
	pInputClassCaseValues->AddValue( ICC_AllVertex );
	testfactor::RFactor rInputClassCase = AddParameterValueSet< InputClassCase >( pInputClassCaseParam, pInputClassCaseValues );

	CUserValueSet< InputClassCase > *pInputClassCaseNoAlternateValues = new CUserValueSet< InputClassCase >();
	pInputClassCaseNoAlternateValues->AddValue( ICC_AllInstance );
	pInputClassCaseNoAlternateValues->AddValue( ICC_AllVertex );
	testfactor::RFactor rInputClassCaseNoAlternate = AddParameterValueSet< InputClassCase >( pInputClassCaseParam, pInputClassCaseNoAlternateValues );

	testfactor::RFactor rNumElements1 = AddParameterValue< UINT >( pNumElements, 1 );

	CUserValueSet< UINT > *pNumElementsValues = new CUserValueSet< UINT >();
	if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		pNumElementsValues->AddValue( 32 );
	else
		pNumElementsValues->AddValue( 16 );
	testfactor::RFactor rNumElementsMulti = AddParameterValueSet< UINT >( pNumElements, pNumElementsValues );

	//should not cycle any of the inputclasscase for 0 elements, and should not cycle alternate for 1 element
	testfactor::RFactor rInputLayout = (rNumElements1 * rInputClassCaseNoAlternate) + (rNumElementsMulti * rInputClassCase);

	//With and without IB
	//With and without Vertex VB
	//With and without Instance VB
	//With and without Input layout

	//Index Buffer cases: No IB, IB smaller than VertexCount
	testfactor::RFactor rIBBound = AddParameterValue< bool >( pIBBound, true );
	testfactor::RFactor rIBUnbound = AddParameterValue< bool >( pIBBound, false );

	testfactor::RFactor rIBCases = rIBUnbound + rIBBound;

	//Vertex Buffers cases: No VB, some VB bounds and smaller than VertexCount
	testfactor::RFactor rNoVBBound = AddParameterValue< bool >( pNoVBBound, true );
	testfactor::RFactor rSomeVBBound = AddParameterValue< bool >( pNoVBBound, false );

	testfactor::RFactor rVBCases = rNoVBBound + rSomeVBBound;

	testfactor::RFactor rUseOffsetInDraw = AddParameterValue< bool >( pUseOffsetInDraw, true );
	testfactor::RFactor rDontUseOffsetInDraw = AddParameterValue< bool >( pUseOffsetInDraw, false );

	testfactor::RFactor rDrawMethod = AddParameterValue< DrawMethod >( pDrawMethod, Draw );
	testfactor::RFactor rDrawInstancedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawInstanced );
	testfactor::RFactor rDrawIndexedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawIndexed );
	testfactor::RFactor rDrawIndexedInstancedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawIndexedInstanced );
	testfactor::RFactor rDrawAutoMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawAuto );

	testfactor::RFactor rDraw = rDrawMethod * rUseOffsetInDraw;
	testfactor::RFactor rDrawInstanced = rDrawInstancedMethod * rUseOffsetInDraw;
	testfactor::RFactor rDrawIndexed = rDrawIndexedMethod * rUseOffsetInDraw;
	testfactor::RFactor rDrawIndexedInstanced = rDrawIndexedInstancedMethod * rUseOffsetInDraw;

	testfactor::RFactor rIndexed = rIBCases * rVBCases * (rDrawIndexed + rDrawIndexedInstanced);
	testfactor::RFactor rNonIndexed = rVBCases * (rDraw + rDrawInstanced);

	SetRootTestFactor( ( (rIndexed + rNonIndexed) * rInputLayout ) );

	// Use priority patterns to "chip away" non priority 1 test cases
	AddPriorityPatternFilter(FilterNotEqual<InputClassCase>(_T("InputClassCase"), ICC_AllVertex), 0.5f);
	AddPriorityPatternFilter(FilterNotEqual<bool>(_T("NoVBBound"), true), 0.5f);
	AddPriorityPatternFilter(FilterNotEqual<bool>(_T("IBBound"), false), 0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

TEST_RESULT CUnboundBuffersTest::Setup()
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

void CUnboundBuffersTest::Cleanup()
{
	CInputAssemblerTest::Cleanup();	

	for (UINT i=0; i<MaxInputElements; i++)
	{
		SAFE_RELEASE( m_pSmallVertexBuffers[i] );
	}
}

TEST_RESULT CUnboundBuffersTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	if( GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0 && m_NumElements > 16 )
		return RESULT_SKIP;

	ID3D11Buffer *pNullBuffer[] = { NULL };
	UINT ZeroOffsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

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

	UINT VertexCount = 10;
	UINT InstanceCount = 10;
	UINT IndexCount = 10;
	UINT StartVertexLocation = m_bUseOffsetInDraw ? 27 : 0;
	UINT StartInstanceLocation = m_bUseOffsetInDraw ? 27 : 0;
	UINT StartIndexLocation = m_bUseOffsetInDraw ? 27 : 0;
	UINT BaseVertexLocation = m_bUseOffsetInDraw ? 27 : 0;

	UINT numDrawElements = 0;
	switch (m_DrawMethod )
	{
		case Draw:
			numDrawElements = 1 * VertexCount;
			break;
		case DrawInstanced:
			numDrawElements = InstanceCount * VertexCount;
			break;
		case DrawIndexed:
			numDrawElements = 1 * IndexCount;
			break;
		case DrawIndexedInstanced:
			numDrawElements = InstanceCount * IndexCount;
			break;
	}

	//Set the stride
	m_LayoutInfo.SetLayoutData(m_bVertexID, m_bInstanceID, numDrawElements);

	//Bind the index buffer
	m_LayoutInfo.IBStride = sizeof(WORD);
	m_LayoutInfo.pIBData = m_IndexBufferData;
	if (m_bIBBound)
	{
		m_LayoutInfo.IBBoundSize = 1;
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBufferSmall, DXGI_FORMAT_R16_UINT, 0);
	}
	else
	{
		m_LayoutInfo.IBBoundSize = 0;
		m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
	}
	m_LayoutInfo.IBOffset = 0;

	//Set vertex buffers
	if (m_bNoVBBound)
	{
		for (UINT i=0; i<MaxInputElements; i++)
		{
			m_LayoutInfo.slots[i].VBBoundSize = 0;
			m_LayoutInfo.slots[i].pVBData = m_BufferData[i];
		}

		ID3D11Buffer *pBuffers[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		UINT Strides[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		m_pDeviceContext->IASetVertexBuffers(0, m_MaxNumElements, pBuffers, Strides, ZeroOffsets);
	}
	else
	{
		for (UINT i=0; i<MaxInputElements; i++)
		{
			m_LayoutInfo.slots[i].VBBoundSize = i + 2;
			m_LayoutInfo.slots[i].pVBData = m_BufferData[i];
		}

		UINT Strides[MaxInputElements];
		UINT FormatStride = GetFormatStrideInBytes(DXGI_FORMAT_R32_UINT);
		for (UINT i=0; i<MaxInputElements; i++)
		{
			Strides[i] = m_LayoutInfo.slots[i].stride;
		}
		m_pDeviceContext->IASetVertexBuffers(0, m_MaxNumElements, m_pSmallVertexBuffers, Strides, ZeroOffsets);
	}

	//Setup the shaders and inputlayout
	if ( RESULT_PASS != SetupPipeline(false) )
		goto TESTFAIL;

	//Start the query for pipeline stats.  It's ended in VerifyDraw*Results.
	GetEffectiveContext()->Begin(m_pQuery);

	//Draw and verify results

	bool bDrawResults = true;
	switch (m_DrawMethod )
	{
		case Draw:
			m_pDeviceContext->Draw(VertexCount, StartVertexLocation);
			bDrawResults = VerifyDrawResults(VertexCount, StartVertexLocation);
			break;
		case DrawInstanced:
			m_pDeviceContext->DrawInstanced(VertexCount, InstanceCount, StartVertexLocation, StartInstanceLocation);
			bDrawResults = VerifyDrawInstancedResults(VertexCount, InstanceCount, StartVertexLocation, StartInstanceLocation);
			break;
		case DrawIndexed:
			m_pDeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
			bDrawResults = VerifyDrawIndexedResults(IndexCount, StartIndexLocation, BaseVertexLocation);
			break;
		case DrawIndexedInstanced:
			m_pDeviceContext->DrawIndexedInstanced(IndexCount, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
			bDrawResults = VerifyDrawIndexedInstancedResults(IndexCount, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
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
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pVS );

	return tRes;
}

