#include "InputAssembler.h"

//Generated values should reset for each draw
//Make sure that all VS Generated values are generated correctly
//	-When IB is used, VertexID should be the IB Values
//		-Also try IB smaller than number of vertices to draw
//	-No Input Layouts (only Generated IDs)

const char szVSPassthroughForDrawAuto[] = 
	"struct InOut"	"\n"
	"{"				"\n"
	"	uint data : data;\n"
	"};"				"\n"
	"InOut main( InOut input )\n"
	"{\n"
	"	return input; \n"
	"}\n"
	;

void CGeneratedValuesTest::InitTestParameters()
{
	//Initialize root factors
	CInputAssemblerTest::InitTestParameters();

	//Create the parameters

	CTestCaseParameter< InputClassCase > *pInputClassCaseParam = AddParameter( _T( "InputClassCase" ), &m_InputClassCase );
	CTestCaseParameter< UINT > *pNumElements = AddParameter< UINT >( _T("NumElements"), &m_NumElements);

	CTestCaseParameter< DrawMethod > *pDrawMethod = AddParameter( _T("DrawMethod"), &m_DrawMethod);

	CTestCaseParameter< UINT > *pVertexCount = AddParameter< UINT >( _T("VertexCount"), &m_VertexCount);
	CTestCaseParameter< UINT > *pStartVertexLocation = AddParameter< UINT >( _T("StartVertexLocation"), &m_StartVertexLocation);
	CTestCaseParameter< UINT > *pIndexCount = AddParameter< UINT >( _T("IndexCount"), &m_IndexCount);
	CTestCaseParameter< UINT > *pStartIndexLocation = AddParameter< UINT >( _T("StartIndexLocation"), &m_StartIndexLocation);
	CTestCaseParameter< INT > *pBaseVertexLocation = AddParameter< INT >( _T("BaseVertexLocation"), &m_BaseVertexLocation);
	CTestCaseParameter< UINT > *pInstanceCount = AddParameter< UINT >( _T("InstanceCount"), &m_InstanceCount);
	CTestCaseParameter< UINT > *pStartInstanceLocation = AddParameter< UINT >( _T("StartInstanceLocation"), &m_StartInstanceLocation);

	CTestCaseParameter< bool > *pVertexID = AddParameter< bool >( _T("GenerateVertexID"), &m_bVertexID);
	CTestCaseParameter< bool > *pInstanceID = AddParameter< bool >( _T("GenerateInstanceID"), &m_bInstanceID);
	CTestCaseParameter< bool > *pIBBound = AddParameter< bool >( _T("IBBound"), &m_bIBBound);
	CTestCaseParameter< bool > *pIBBigEnough = AddParameter< bool >( _T("IBBigEnough"), &m_bIBBigEnough);
	CTestCaseParameter< bool > *pNullInputLayout = AddParameter< bool >( _T("NullInputLayout"), &m_NullInputLayout);

	SetParameterDefaultValue< UINT >( pStartVertexLocation, 0 );
	SetParameterDefaultValue< UINT >( pStartIndexLocation, 0 );
	SetParameterDefaultValue< UINT >( pStartInstanceLocation, 0 );
	SetParameterDefaultValue< bool >( pNullInputLayout, false );

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

	testfactor::RFactor rNumElements0 = AddParameterValue< UINT >( pNumElements, 0 );
	testfactor::RFactor rNumElements1 = AddParameterValue< UINT >( pNumElements, 1 );
	testfactor::RFactor rNullInputLayout = AddParameterValueSet< bool >( pNullInputLayout, BoolValueSet() );

	CUserValueSet< UINT > *pNumElementsValues = new CUserValueSet< UINT >();
	if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		pNumElementsValues->AddValue( 28 );
	else
		pNumElementsValues->AddValue( 14 );
	testfactor::RFactor rNumElementsMulti = AddParameterValueSet< UINT >( pNumElements, pNumElementsValues );

	//should not cycle any of the inputclasscase for 0 elements, and should not cycle alternate for 1 element
	testfactor::RFactor rInputLayout = (rNumElements0 * rNullInputLayout) + (rNumElements1 * rInputClassCaseNoAlternate) + (rNumElementsMulti * rInputClassCase);


	CUserValueSet< UINT > *pVertexCountValues = new CUserValueSet< UINT >();
	pVertexCountValues->AddValue( 1 );
	pVertexCountValues->AddValue( 10 );
	//pVertexCountValues->AddValue( 65450 ); //To exceed 16 bit
	pVertexCountValues->AddValue( 65540 ); //To exceed 16 bit
	testfactor::RFactor rVertexCount = AddParameterValueSet< UINT >( pVertexCount, pVertexCountValues );

	CUserValueSet< UINT > *pStartVertexLocationValues = new CUserValueSet< UINT >();
	pStartVertexLocationValues->AddValue( 0 );
	pStartVertexLocationValues->AddValue( 31 );
	testfactor::RFactor rStartVertexLocation = AddParameterValueSet< UINT >( pStartVertexLocation, pStartVertexLocationValues );

	CUserValueSet< UINT > *pIndexCountValues = new CUserValueSet< UINT >();
	pIndexCountValues->AddValue( 1 );
	pIndexCountValues->AddValue( 10 );
	pIndexCountValues->AddValue( 65540 ); //To exceed 16 bit
	testfactor::RFactor rIndexCount = AddParameterValueSet< UINT >( pIndexCount, pIndexCountValues );

	CUserValueSet< UINT > *pStartIndexLocationValues = new CUserValueSet< UINT >();
	pStartIndexLocationValues->AddValue( 0 );
	pStartIndexLocationValues->AddValue( 27 );
	testfactor::RFactor rStartIndexLocation = AddParameterValueSet< UINT >( pStartIndexLocation, pStartIndexLocationValues );

	CUserValueSet< INT > *pBaseVertexLocationValues = new CUserValueSet< INT >();
	pBaseVertexLocationValues->AddValue( 0 );
	pBaseVertexLocationValues->AddValue( 1 );
	pBaseVertexLocationValues->AddValue( 16 );
	testfactor::RFactor rBaseVertexLocation = AddParameterValueSet< INT >( pBaseVertexLocation, pBaseVertexLocationValues );

	CUserValueSet< UINT > *pInstanceCountValues = new CUserValueSet< UINT >();
	pInstanceCountValues->AddValue( 1 );
	pInstanceCountValues->AddValue( 10 );
	//pInstanceCountValues->AddValue( 65540 ); //To exceed 16 bit
	testfactor::RFactor rInstanceCount = AddParameterValueSet< UINT >( pInstanceCount, pInstanceCountValues );

	CUserValueSet< UINT > *pStartInstanceLocationValues = new CUserValueSet< UINT >();
	pStartInstanceLocationValues->AddValue( 0 );
	pStartInstanceLocationValues->AddValue( 65 );
	testfactor::RFactor rStartInstanceLocation = AddParameterValueSet< UINT >( pStartInstanceLocation, pStartInstanceLocationValues );

	//With and without IB
	//With and without Vertex VB
	//With and without Instance VB
	//With and without Input layout

	//Index Buffer cases: No IB, IB smaller than VertexCount, IB bigger than VertexCount
	testfactor::RFactor rIBBound = AddParameterValue< bool >( pIBBound, true );
	testfactor::RFactor rIBUnbound = AddParameterValue< bool >( pIBBound, false );
	testfactor::RFactor rIBBigEnough = AddParameterValue< bool >( pIBBigEnough, true );
	testfactor::RFactor rIBTooSmall = AddParameterValue< bool >( pIBBigEnough, false );

	testfactor::RFactor rIBCases = rIBUnbound + rIBBound * (rIBBigEnough + rIBTooSmall);
	//Bind an Index Buffer for non indexed draw, to make sure it doesn't mess the ids.
	testfactor::RFactor rIBCasesForNonIndexed = rIBBound * rIBBigEnough;


	//VertexID, InstanceID or Both
	testfactor::RFactor rVertexIDOn = AddParameterValue< bool >( pVertexID, true );
	testfactor::RFactor rVertexIDOff = AddParameterValue< bool >( pVertexID, false );
	testfactor::RFactor rInstanceIDOn = AddParameterValue< bool >( pInstanceID, true );
	testfactor::RFactor rInstanceIDOff = AddParameterValue< bool >( pInstanceID, false );

	testfactor::RFactor rGeneratedIDs = rVertexIDOn * rInstanceIDOff + rVertexIDOff * rInstanceIDOn + rVertexIDOn * rInstanceIDOn;


	testfactor::RFactor rDrawMethod = AddParameterValue< DrawMethod >( pDrawMethod, Draw );
	testfactor::RFactor rDrawInstancedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawInstanced );
	testfactor::RFactor rDrawIndexedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawIndexed );
	testfactor::RFactor rDrawIndexedInstancedMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawIndexedInstanced );
	testfactor::RFactor rDrawAutoMethod = AddParameterValue< DrawMethod >( pDrawMethod, DrawAuto );

	testfactor::RFactor rDraw = rDrawMethod * rVertexCount * rStartVertexLocation;
	testfactor::RFactor rDrawInstanced = rDrawInstancedMethod * rVertexCount * rInstanceCount * rStartVertexLocation * rStartInstanceLocation;
	testfactor::RFactor rDrawIndexed = rDrawIndexedMethod * rIndexCount * rStartIndexLocation * rBaseVertexLocation;
	testfactor::RFactor rDrawIndexedInstanced = rDrawIndexedInstancedMethod * rIndexCount * rInstanceCount * rStartIndexLocation * rBaseVertexLocation * rStartInstanceLocation;

	testfactor::RFactor rDrawAuto = rDrawAutoMethod * rNumElements1 * rVertexCount;
	//TODO: Need to try instance data and more elements with DrawAuto

	testfactor::RFactor rIndexed = rIBCases * (rDrawIndexed + rDrawIndexedInstanced);
	testfactor::RFactor rNonIndexed = rIBCasesForNonIndexed * (rDraw + rDrawInstanced);

	SetRootTestFactor( ( ( (rIndexed + rNonIndexed) * rInputLayout ) + rDrawAuto ) * rGeneratedIDs );

	// Use priority patterns to "chip away" non priority 1 test cases
	AddPriorityPatternFilter(FilterNotEqual<UINT>(pVertexCount, 10), 0.5f); // Drawing 1 vertex is not very interesting, but drawing too many vertices is not needed
	AddPriorityPatternFilter(FilterGreater<UINT>(pStartVertexLocation, 1), 0.5f);

	AddPriorityPatternFilter(FilterNotEqual<UINT>(pIndexCount, 10), 0.5f);
	AddPriorityPatternFilter(FilterGreater<UINT>(pStartIndexLocation, 1), 0.5f);

    AddPriorityPatternFilter(FilterNotEqual<UINT>(pInstanceCount, 10), 0.5f);	// One instance is not very interesting
	AddPriorityPatternFilter(FilterGreater<UINT>(pStartInstanceLocation, 1), 0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

TEST_RESULT CGeneratedValuesTest::Setup()
{
	return CInputAssemblerTest::Setup();	
}


TEST_RESULT CGeneratedValuesTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;

	if( GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0 && m_NumElements > 16 )
		return RESULT_SKIP;

	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;
	ID3D11Buffer *pNullBuffer[] = { NULL };
	UINT ZeroOffsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	ZeroMemory( m_InputElements, sizeof(m_InputElements) );

	if (m_DrawMethod == DrawAuto)
	{
		m_InputElements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_InputElements[0].InputSlot = 0;
		m_InputElements[0].Format = DXGI_FORMAT_R32_UINT;
		m_InputElements[0].AlignedByteOffset = 0;
		m_InputElements[0].InstanceDataStepRate = 0;
		m_InputElements[0].SemanticName = szSemantic;
		m_InputElements[0].SemanticIndex = 0;

		m_LayoutInfo.InitializeInputElements(m_InputElements, m_NumElements);

		for (UINT i=0; i<MaxInputElements; i++)
		{
			m_LayoutInfo.slots[i].VBBoundSize = MaxBufferSize;
			m_LayoutInfo.slots[i].pVBData = m_BufferData[i];
		}

		//Set the stride
		m_LayoutInfo.SetLayoutData(false, false, m_VertexCount);

		hr = g_App.CompileShaderFromMemory( szVSPassthroughForDrawAuto, strlen( szVSPassthroughForDrawAuto ), "main", D3D_SHADER_STAGE_VERTEX, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0, 0, &pShaderBuf, &pErrorBuf );
		if( FAILED( hr ) )
		{
			WriteToLog( "CompileShaderFromMemory() failed %s", pErrorBuf->GetBufferPointer() );
			goto TESTFAIL;
		}

		hr = m_pDevice->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS );
		if ( FAILED(hr) )
		{
			WriteToLog( "CreateVertexShader() failed" );
			goto TESTFAIL;
		}

		//Create a SO decl that matches
		D3D11_SO_DECLARATION_ENTRY soLayout[MaxInputElements];
		ZeroMemory(soLayout, sizeof(soLayout));
		//Preallocate memory for semantic names
		char szSOSemantic[MaxInputElements][32];
		ZeroMemory( szSOSemantic, sizeof(szSOSemantic) );
		for (int i=0; i<MaxInputElements; i++)
			soLayout[i].SemanticName = szSOSemantic[i];

		m_LayoutInfo.CreateStreamOutputDecl( soLayout );

		hr = m_pDevice->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), soLayout, m_LayoutInfo.numSOElements, &m_LayoutInfo.SOStride, 1, 0, NULL, &m_pGS );		if ( FAILED(hr) )
		{
			WriteToLog( "CreateGeometryShaderWithStreamOutput() failed" );
			goto TESTFAIL;
		}

		if (m_NullInputLayout)
		{
			m_pDeviceContext->IASetInputLayout( NULL );
		}
		else
		{
			//Create the input layout
			hr = m_pDevice->CreateInputLayout(m_InputElements, m_NumElements, pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pInputLayout);
			if ( FAILED(hr) )
			{
				WriteToLog( "CreateInputLayout() failed" );
				goto TESTFAIL;
			}

			m_pDeviceContext->IASetInputLayout( m_pInputLayout );
		}

		m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
		m_pDeviceContext->GSSetShader( m_pGS, NULL, 0 );
		m_pDeviceContext->PSSetShader( NULL, NULL, 0 );
		

		//Set vertex buffers
		UINT Strides[MaxInputElements];
		UINT FormatStride = GetFormatStrideInBytes(DXGI_FORMAT_R32_UINT);
		for (UINT i=0; i<MaxInputElements; i++)
		{
			Strides[i] = FormatStride;
		}
		m_pDeviceContext->IASetVertexBuffers(0, m_MaxNumElements, m_pVertexBuffers, Strides, ZeroOffsets);

		//Set the SOVB buffer that will be the input to DrawAuto
		m_pDeviceContext->SOSetTargets(1, pNullBuffer, ZeroOffsets);
		m_pDeviceContext->SOSetTargets(1, &m_pSOVBBuffer, ZeroOffsets);

		//Draw to fill the buffer
		m_pDeviceContext->Draw(m_VertexCount, 0);
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
			goto TESTFAIL;
		}

		
		SAFE_RELEASE( pShaderBuf );
		SAFE_RELEASE( pErrorBuf );
		m_pDeviceContext->VSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		SAFE_RELEASE( m_pGS );
		SAFE_RELEASE( m_pVS );
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
			goto TESTFAIL;
		}

		//Need to reinitialize to reset the strides to 0
		m_LayoutInfo.InitializeInputElements(m_InputElements, m_NumElements);

		for (UINT i=0; i<MaxInputElements; i++)
		{
			m_LayoutInfo.slots[i].VBBoundSize = MaxBufferSize;
			m_LayoutInfo.slots[i].pVBData = m_BufferData[i];
		}

		//Set layout data
		m_LayoutInfo.SetLayoutData(m_bVertexID, m_bInstanceID, m_VertexCount);

		//Now get a VS with generated IDs
		tstring szVS = m_LayoutInfo.CreateVSCode();

		hr = g_App.CompileShaderFromMemory( szVS.c_str(), strlen( szVS.c_str() ), "main", D3D_SHADER_STAGE_VERTEX, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0, 0, &pShaderBuf, &pErrorBuf );
		if( FAILED( hr ) )
		{
			WriteToLog( "CompileShaderFromMemory() failed %s", pErrorBuf->GetBufferPointer() );
			goto TESTFAIL;
		}

		hr = m_pDevice->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS );
		if ( FAILED(hr) )
		{
			WriteToLog( "CreateVertexShader() failed" );
			goto TESTFAIL;
		}

		//Create a SO decl that matches
		m_LayoutInfo.CreateStreamOutputDecl( soLayout );

		hr = m_pDevice->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), soLayout, m_LayoutInfo.numSOElements, &m_LayoutInfo.SOStride, 1, 0, NULL, &m_pGS );
		if ( FAILED(hr) )
		{
			WriteToLog( "CreateGeometryShaderWithStreamOutput() failed" );
			goto TESTFAIL;
		}

		m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
		m_pDeviceContext->GSSetShader( m_pGS, NULL, 0 );
		m_pDeviceContext->PSSetShader( NULL, NULL, 0 );

		m_pDeviceContext->SOSetTargets(1, pNullBuffer, ZeroOffsets);
		m_pDeviceContext->SOSetTargets(1, &m_pSOBuffer, ZeroOffsets);

		m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pSOVBBuffer, Strides, ZeroOffsets);

	}
	else
	{
		for (UINT i=0; i<m_NumElements; i++)
		{
			m_InputElements[i].InputSlotClass = InputClassLayouts[m_InputClassCase][i];
			m_InputElements[i].InputSlot = i;

			m_InputElements[i].SemanticName = szSemantic;
			m_InputElements[i].SemanticIndex = i;

			m_InputElements[i].Format = DXGI_FORMAT_R32_UINT;
		}

		m_LayoutInfo.InitializeInputElements(m_InputElements, m_NumElements);

		for (UINT i=0; i<MaxInputElements; i++)
		{
			m_LayoutInfo.slots[i].VBBoundSize = MaxBufferSize;
			m_LayoutInfo.slots[i].pVBData = m_BufferData[i];
		}

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

		UINT numDrawElements = 0;
		switch (m_DrawMethod )
		{
			case Draw:
				numDrawElements = 1 * m_VertexCount;
				break;
			case DrawInstanced:
				numDrawElements = m_InstanceCount * m_VertexCount;
				break;
			case DrawIndexed:
				numDrawElements = 1 * m_IndexCount;
				break;
			case DrawIndexedInstanced:
				numDrawElements = m_InstanceCount * m_IndexCount;
				break;
		}

		//Set the stride
		m_LayoutInfo.SetLayoutData(m_bVertexID, m_bInstanceID, numDrawElements);

		//Bind the index buffer
		m_LayoutInfo.IBStride = sizeof(WORD);
		m_LayoutInfo.pIBData = m_IndexBufferData;
		if (m_bIBBound)
		{
			if (m_bIBBigEnough)
			{
				m_LayoutInfo.IBBoundSize = MaxBufferSize;
				m_pDeviceContext->IASetIndexBuffer(m_pIndexBufferBig, DXGI_FORMAT_R16_UINT, 0);
			}
			else
			{
				m_LayoutInfo.IBBoundSize = 1;
				m_pDeviceContext->IASetIndexBuffer(m_pIndexBufferSmall, DXGI_FORMAT_R16_UINT, 0);
			}
		}
		else
		{
			m_LayoutInfo.IBBoundSize = 0;
			m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
		}
		m_LayoutInfo.IBOffset = 0;

		//Set vertex buffers
		UINT Strides[MaxInputElements];
		UINT FormatStride = GetFormatStrideInBytes(DXGI_FORMAT_R32_UINT);
		for (UINT i=0; i<MaxInputElements; i++)
		{
			Strides[i] = m_LayoutInfo.slots[i].stride;
		}
		m_pDeviceContext->IASetVertexBuffers(0, m_MaxNumElements, m_pVertexBuffers, Strides, ZeroOffsets);

		//Setup the shaders and inputlayout
		if ( RESULT_PASS != SetupPipeline(m_NullInputLayout) )
			goto TESTFAIL;

	}//End non DrawAuto


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

		case DrawIndexed:
			m_pDeviceContext->DrawIndexed(m_IndexCount,m_StartIndexLocation, m_BaseVertexLocation);
			bDrawResults = VerifyDrawIndexedResults(m_IndexCount,m_StartIndexLocation, m_BaseVertexLocation);
			break;

		case DrawIndexedInstanced:
			m_pDeviceContext->DrawIndexedInstanced(m_IndexCount, m_InstanceCount, m_StartIndexLocation, m_BaseVertexLocation, m_StartInstanceLocation);
			bDrawResults = VerifyDrawIndexedInstancedResults(m_IndexCount, m_InstanceCount, m_StartIndexLocation, m_BaseVertexLocation, m_StartInstanceLocation);
			break;

		case DrawAuto:
			m_pDeviceContext->DrawAuto();
			bDrawResults = VerifyDrawAutoResults(m_VertexCount);
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

	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pVS );

	return tRes;
}
