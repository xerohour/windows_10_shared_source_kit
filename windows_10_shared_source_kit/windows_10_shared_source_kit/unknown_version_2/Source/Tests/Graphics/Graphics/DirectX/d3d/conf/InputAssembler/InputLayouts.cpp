#include "InputAssembler.h"

/*
	InputSlotClass
		- Layouts with varying slot class, like
			- vertex, instance, vertex, vertex
			- instance, vertex, vertex, instance
			- max number of elements varying
			- etc.

	InputSlot
		- Layouts with :
			- Out of order slots
			- Skipped slots
			- Not using slot 0

	InstanceDataStepRate 
		- Make sure they are respected

	Alignments
		- Make sure they are respected

	Number of elements in input layout :
		- smaller than number of elements in shader
			- HW needs to set default value
		- bigger than number of elements in shader
			- expect same behavior as equal numbers
		- Maximum number of elements
*/



const UINT PseudoRandomElements[MaxInputElements] =
{
	9, 14, 3, 7, 11, 12, 6, 1, 15, 0, 13, 2, 4, 10, 5, 8
};

BEGIN_NAMED_VALUES( InputSlotCase )
	NAMED_VALUE( _T( "Sequential" ), ISC_Sequential )
	NAMED_VALUE( _T( "Random3" ), ISC_Random3 )
END_NAMED_VALUES( InputSlotCase )

BEGIN_NAMED_VALUES( AlignedByteOffsetCase )
	NAMED_VALUE( _T( "MatchFormat" ), ABOC_MatchFormat )
	NAMED_VALUE( _T( "MatchFormatOutOfOrder" ), ABOC_MatchFormatOutOfOrder )
	NAMED_VALUE( _T( "BiggerThanFormat" ), ABOC_BiggerThanFormat )
	NAMED_VALUE( _T( "BiggerThanFormatOutOfOrder" ), ABOC_BiggerThanFormatOutOfOrder )
	NAMED_VALUE( _T( "SameOffsets" ), ABOC_SameOffsets )
END_NAMED_VALUES( AlignedByteOffsetCase )

BEGIN_NAMED_VALUES( InstanceDataStepRateCase )
	NAMED_VALUE( _T( "Zero" ), IDSRC_Zero )
	NAMED_VALUE( _T( "One" ), IDSRC_One )
	NAMED_VALUE( _T( "Random" ), IDSRC_Random )
END_NAMED_VALUES( InstanceDataStepRateCase )

void ID3D11InputLayoutsTest::InitTestParameters()
{
	//Initialize root factors
	CInputAssemblerTest::InitTestParameters();

	//Create the parameters

	CTestCaseParameter< InputClassCase > *pInputClassCaseParam = AddParameter( _T( "InputClassCase" ), &m_InputClassCase );
	CTestCaseParameter< UINT > *pNumElements = AddParameter< UINT >( _T("NumElements"), &m_NumElements);
	CTestCaseParameter< InputSlotCase > *pInputSlotCase = AddParameter( _T("InputSlotCase"), &m_InputSlotCase);
	CTestCaseParameter< DXGI_FORMAT > *pFormatParam = AddParameter( _T( "Format" ), &m_Format );
	CTestCaseParameter< AlignedByteOffsetCase > *pAlignedByteOffsetCase = AddParameter( _T("AlignedByteOffsetCase"), &m_AlignedByteOffsetCase);
	CTestCaseParameter< InstanceDataStepRateCase > *pInstanceDataStepRateCase = AddParameter( _T("InstanceDataStepRateCase"), &m_InstanceDataStepRateCase);
	CTestCaseParameter< DrawMethod > *pDrawMethod = AddParameter( _T("DrawMethod"), &m_DrawMethod);


	//Create the factors

	CUserValueSet< InputClassCase > *pInputClassCaseValues = new CUserValueSet< InputClassCase >();
	pInputClassCaseValues->AddValue( ICC_AlternateByVertex );
	pInputClassCaseValues->AddValue( ICC_AlternateByInstance );
	pInputClassCaseValues->AddValue( ICC_AlternateBy2Vertex );
	pInputClassCaseValues->AddValue( ICC_AlternateBy2Instance );
	pInputClassCaseValues->AddValue( ICC_AlternateByRandomVertex );
	pInputClassCaseValues->AddValue( ICC_AlternateByRandomInstance );
	testfactor::RFactor rInputClassCase = AddParameterValueSet<InputClassCase>( pInputClassCaseParam, pInputClassCaseValues );

	testfactor::RFactor rVertexOnlyInputClassCase = AddParameterValue<InputClassCase>( pInputClassCaseParam, ICC_AllVertex );
	testfactor::RFactor rInstanceOnlyInputClassCase = AddParameterValue<InputClassCase>( pInputClassCaseParam, ICC_AllInstance );

	CUserValueSet< UINT > *pNumElementsValues = new CUserValueSet< UINT >();
	pNumElementsValues->AddValue( 1 );
	pNumElementsValues->AddValue( 2 );
	pNumElementsValues->AddValue( 8 );
	if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		pNumElementsValues->AddValue( 32 );
	else
		pNumElementsValues->AddValue( 16 );
	testfactor::RFactor rNumElements = AddParameterValueSet< UINT >( pNumElements, pNumElementsValues );

	CUserValueSet< UINT > *pSameInputClassNumElementsValues = new CUserValueSet< UINT >();
	pSameInputClassNumElementsValues->AddValue( 3 );
	if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		pSameInputClassNumElementsValues->AddValue( 32 );
	else
		pSameInputClassNumElementsValues->AddValue( 16 );
	testfactor::RFactor rSameInputClassNumElements = AddParameterValueSet< UINT >( pNumElements, pSameInputClassNumElementsValues );

	CUserValueSet< InputSlotCase > *pInputSlotCaseValues = new CUserValueSet< InputSlotCase >();
	pInputSlotCaseValues->AddValue( ISC_Sequential );
	pInputSlotCaseValues->AddValue( ISC_Random3 );
	testfactor::RFactor rInputSlotCase = AddParameterValueSet< InputSlotCase >( pInputSlotCase, pInputSlotCaseValues );

	CUserValueSet< AlignedByteOffsetCase > *pAlignedByteOffsetCaseValues = new CUserValueSet< AlignedByteOffsetCase >();
	pAlignedByteOffsetCaseValues->AddValue( ABOC_MatchFormat );
	pAlignedByteOffsetCaseValues->AddValue( ABOC_MatchFormatOutOfOrder );
	pAlignedByteOffsetCaseValues->AddValue( ABOC_BiggerThanFormat );
	pAlignedByteOffsetCaseValues->AddValue( ABOC_BiggerThanFormatOutOfOrder );
	pAlignedByteOffsetCaseValues->AddValue( ABOC_SameOffsets );
	testfactor::RFactor rAlignedByteOffsetCase = AddParameterValueSet< AlignedByteOffsetCase >( pAlignedByteOffsetCase, pAlignedByteOffsetCaseValues );

	CUserValueSet< InstanceDataStepRateCase > *pInstanceDataStepRateCaseValues = new CUserValueSet< InstanceDataStepRateCase >();
	pInstanceDataStepRateCaseValues->AddValue( IDSRC_Zero );
	pInstanceDataStepRateCaseValues->AddValue( IDSRC_One );
	pInstanceDataStepRateCaseValues->AddValue( IDSRC_Random );
	testfactor::RFactor rInstanceDataStepRateCase = AddParameterValueSet< InstanceDataStepRateCase >( pInstanceDataStepRateCase, pInstanceDataStepRateCaseValues );

	CUserValueSet< InstanceDataStepRateCase > *pVertexOnlyInstanceDataStepRateCaseValues = new CUserValueSet< InstanceDataStepRateCase >();
	pVertexOnlyInstanceDataStepRateCaseValues->AddValue( IDSRC_Zero );
	testfactor::RFactor rVertexOnlyInstanceDataStepRateCase = AddParameterValueSet< InstanceDataStepRateCase >( pInstanceDataStepRateCase, pVertexOnlyInstanceDataStepRateCaseValues );

	CUserValueSet< InstanceDataStepRateCase > *pInstanceOnlyInstanceDataStepRateCaseValues = new CUserValueSet< InstanceDataStepRateCase >();
	pInstanceOnlyInstanceDataStepRateCaseValues->AddValue( IDSRC_Zero ); //TODO: can probably remove that one since random covers 0s
	pInstanceOnlyInstanceDataStepRateCaseValues->AddValue( IDSRC_One );
	pInstanceOnlyInstanceDataStepRateCaseValues->AddValue( IDSRC_Random );
	testfactor::RFactor rInstanceOnlyInstanceDataStepRateCase = AddParameterValueSet< InstanceDataStepRateCase >( pInstanceDataStepRateCase, pInstanceOnlyInstanceDataStepRateCaseValues );

	CUserValueSet< DrawMethod > *pDrawMethodValues = new CUserValueSet< DrawMethod >();
	pDrawMethodValues->AddValue( Draw );
	pDrawMethodValues->AddValue( DrawInstanced );
	testfactor::RFactor rDrawMethod = AddParameterValueSet< DrawMethod >( pDrawMethod, pDrawMethodValues );

	CFilterHandleCommonSource<DXGI_FORMAT> formatFilter = 
		FilterEqual<DXGI_FORMAT>( DXGI_FORMAT_R32_UINT ) ||
		FilterEqual<DXGI_FORMAT>( DXGI_FORMAT_R32G32_UINT ) ||
		FilterEqual<DXGI_FORMAT>( DXGI_FORMAT_R32G32B32_UINT ) ||
		FilterEqual<DXGI_FORMAT>( DXGI_FORMAT_R32G32B32A32_UINT );
	testfactor::RFactor rFormat = AddParameterValueSet( pFormatParam, FormatValueSet( formatFilter ) );

	testfactor::RFactor rAnyInputClass = rInputSlotCase * rFormat * rAlignedByteOffsetCase;
	testfactor::RFactor rSameInputClass = rSameInputClassNumElements * rInputSlotCase * rFormat * rAlignedByteOffsetCase;

	testfactor::RFactor rVertexOnlyInputClass = rVertexOnlyInputClassCase * rSameInputClassNumElements * rVertexOnlyInstanceDataStepRateCase;
	testfactor::RFactor rInstanceOnlyInputClass = rInstanceOnlyInputClassCase * rSameInputClassNumElements * rInstanceOnlyInstanceDataStepRateCase;
	testfactor::RFactor rMixedInputClass = rInputClassCase * rNumElements * rInstanceDataStepRateCase;

	SetRootTestFactor( rDrawMethod * rAnyInputClass * (rVertexOnlyInputClass + rInstanceOnlyInputClass + rMixedInputClass) );

	// Use priority patterns to "chip away" non priority 1 test cases
	AddPriorityPatternFilter(FilterEqual<InputClassCase>(pInputClassCaseParam, ICC_AlternateBy2Vertex), 0.5f);
    AddPriorityPatternFilter(FilterEqual<InputClassCase>(pInputClassCaseParam, ICC_AlternateBy2Instance), 0.5f);
    AddPriorityPatternFilter(FilterEqual<InputClassCase>(pInputClassCaseParam, ICC_AlternateByRandomVertex), 0.5f);
    AddPriorityPatternFilter(FilterEqual<InputClassCase>(pInputClassCaseParam, ICC_AlternateByRandomInstance), 0.5f);

	AddPriorityPatternFilter(	FilterNotEqual<UINT>(pNumElements, 1) && 
								FilterNotEqual<UINT>(pNumElements, 2), 0.5f);

	AddPriorityPatternFilter(	FilterNotEqual<AlignedByteOffsetCase>(pAlignedByteOffsetCase, ABOC_MatchFormat) && 
								FilterNotEqual<AlignedByteOffsetCase>(pAlignedByteOffsetCase, ABOC_MatchFormatOutOfOrder), 0.5f);

    AddPriorityPatternFilter(FilterEqual<InstanceDataStepRateCase>(pInstanceDataStepRateCase, IDSRC_Random), 0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

TEST_RESULT ID3D11InputLayoutsTest::Setup()
{
	m_bVertexID = false;
	m_bInstanceID = false;

	return CInputAssemblerTest::Setup();	
}


TEST_RESULT ID3D11InputLayoutsTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	if( GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0 && m_NumElements > 16 )
		return RESULT_SKIP;

	ZeroMemory( m_InputElements, sizeof(m_InputElements) );

	for (UINT i=0; i<m_NumElements; i++)
	{
		m_InputElements[i].InputSlotClass = InputClassLayouts[m_InputClassCase][i];
		m_InputElements[i].InputSlot = InputSlotElements[m_InputSlotCase][i];

		//For cases that conflicts, use the slot of the previous element with same slot class
		//if can't resolve conflict, use slot of the element #
		bool bNeedNewSlot = false;
		for (int iPrev = i-1; iPrev >= 0; iPrev--)
		{
			if ( bNeedNewSlot && m_InputElements[iPrev].InputSlotClass == m_InputElements[i].InputSlotClass )
			{
				bNeedNewSlot = false;
				m_InputElements[i].InputSlot = m_InputElements[iPrev].InputSlot;
				break;
			}

			if ( !bNeedNewSlot &&
				 (m_InputElements[iPrev].InputSlot == m_InputElements[i].InputSlot) &&
				 (m_InputElements[iPrev].InputSlotClass != m_InputElements[i].InputSlotClass) )
			{
				//restart loop
				iPrev = i-1;
				bNeedNewSlot = true;
			}
		}
		if (bNeedNewSlot)
		{
			//TODO:  This might not work, might need to get the first unused slot
			m_InputElements[i].InputSlot = i;
		}

		
		m_InputElements[i].SemanticName = szSemantic;
		m_InputElements[i].SemanticIndex = i;

		m_InputElements[i].Format = m_Format;
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
				m_InputElements[iElement].InstanceDataStepRate = InstanceDataStepRateElements[m_InstanceDataStepRateCase][iSlot];

			switch(m_AlignedByteOffsetCase)
			{
				case ABOC_MatchFormat:
					//Need to add format stride for previous element with same input slot
					if ( j > 0 )
					{
						UINT iPrevElement = m_LayoutInfo.slots[iSlot].elementsIndex[j-1];
						assert (m_InputElements[iPrevElement].InputSlot == iSlot);

						m_InputElements[iElement].AlignedByteOffset = m_InputElements[iPrevElement].AlignedByteOffset + GetFormatStrideInBytes(m_InputElements[iPrevElement].Format);
					}
					break;
				case ABOC_MatchFormatOutOfOrder:
					for (UINT k=0; k < m_LayoutInfo.slots[iSlot].numElements; k++)
					{
						UINT iTempElement = m_LayoutInfo.slots[iSlot].elementsIndex[k];
						UINT iPrevShuffledElement = m_LayoutInfo.slots[iSlot].elementsIndex[shuffledElements[k]];

						assert (m_InputElements[iPrevShuffledElement].InputSlot == iSlot);

						if (iPrevShuffledElement < iShuffledElement)
						{
							m_InputElements[iElement].AlignedByteOffset += GetFormatStrideInBytes(m_InputElements[iTempElement].Format);
						}
					}
					break;
				case ABOC_BiggerThanFormat:
					//Find Previous slot offset and add format stride + value
					if ( j > 0 )
					{
						UINT iPrevElement = m_LayoutInfo.slots[iSlot].elementsIndex[j-1];
						assert (m_InputElements[iPrevElement].InputSlot == iSlot);

						m_InputElements[iElement].AlignedByteOffset = m_InputElements[iPrevElement].AlignedByteOffset + GetFormatStrideInBytes(m_InputElements[iPrevElement].Format) * ((j % 3) + 1);
					}
					break;
				case ABOC_BiggerThanFormatOutOfOrder:
					for (UINT k=0; k < m_LayoutInfo.slots[iSlot].numElements; k++)
					{
						UINT iTempElement = m_LayoutInfo.slots[iSlot].elementsIndex[k];
						UINT iPrevShuffledElement = m_LayoutInfo.slots[iSlot].elementsIndex[shuffledElements[k]];

						assert (m_InputElements[iPrevShuffledElement].InputSlot == iSlot);

						if (iPrevShuffledElement < iShuffledElement)
						{
							m_InputElements[iElement].AlignedByteOffset += GetFormatStrideInBytes(m_InputElements[iTempElement].Format) * ((j % 3) + 1);
						}
					}
					break;
				case ABOC_SameOffsets:
					//increment offset by biggest format stride of the slot every 3 elements in the slot
					m_InputElements[iElement].AlignedByteOffset = numElementsInSlot / 3 * m_LayoutInfo.slots[iSlot].maxFormatStride;
					break;
			}

			numElementsInSlot++;
		}
	}


	UINT numDrawElements = 0;
	switch (m_DrawMethod )
	{
		case Draw:
			numDrawElements = 1 * 10;
			break;
		case DrawInstanced:
			numDrawElements = 10 * 1;
			break;
	}

	//Set the stride
	m_LayoutInfo.SetLayoutData(false, false, numDrawElements);

	//Setup the shaders and inputlayout
	if ( RESULT_PASS != SetupPipeline(false) )
		goto TESTFAIL;

	//Set vertex buffers
	UINT Strides[MaxInputElements];
	UINT Offsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	UINT FormatStride = GetFormatStrideInBytes(m_Format);
	for (UINT i=0; i<MaxInputElements; i++)
	{
		m_LayoutInfo.slots[i].VBBoundSize = MaxBufferSize;
		m_LayoutInfo.slots[i].VBOffset = 0;
		m_LayoutInfo.slots[i].pVBData = m_BufferData[i];
		Strides[i] = m_LayoutInfo.slots[i].stride;
	}
	m_pDeviceContext->IASetVertexBuffers(0, m_MaxNumElements, m_pVertexBuffers, Strides, Offsets);


	UINT startVertexLocation = 0;
	UINT startInstanceLocation = 0;
	

	//Start the query for pipeline stats.  It's ended in VerifyDraw*Results.
	GetEffectiveContext()->Begin(m_pQuery);

	//Draw and verify
	bool bDrawResults = true;
	if (m_DrawMethod == Draw)
	{
		m_pDeviceContext->Draw(10, startVertexLocation);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
			goto TESTFAIL;
		}
		bDrawResults = VerifyDrawResults(10, startVertexLocation);
	}
	else
	{
		m_pDeviceContext->DrawInstanced(10, 1, startVertexLocation, startInstanceLocation);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
			goto TESTFAIL;
		}
		bDrawResults = VerifyDrawInstancedResults(10, 1, startVertexLocation, startInstanceLocation);
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
