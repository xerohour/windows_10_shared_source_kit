#include "PixelShader.h"

struct ExpectedResults
{
	UINT ExpectedId;
	UINT ExpectedIsFront;
};

const UINT MaxVertices = 0x80000;
const UINT MaxVBPosSize = sizeof(float) * 4 * MaxVertices;
const UINT MaxVBExpectedSize = sizeof(ExpectedResults) * MaxVertices;

CGeneratedValuesTest::CGeneratedValuesTest() :
	m_pInputLayout( NULL ),
	m_pVS( NULL ),
	m_pVSNoExpected( NULL ),
	m_pPS( NULL ),
	m_pPSWithFrontFace( NULL ),
	m_pPSNoExpected( NULL ),
	m_pVBPos( NULL ),
	m_pVBExpected( NULL )
{
}

void CGeneratedValuesTest::InitTestParameters()
{
	//Initialize root factors
	ID3D11PixelShaderTest::InitTestParameters();

	//Create the parameters

	CTestCaseParameter< D3D11_PRIMITIVE_TOPOLOGY > *pPrimitiveTopology = AddParameter< D3D11_PRIMITIVE_TOPOLOGY >( _T("PrimitiveTopology"), &m_PrimitiveTopology);
	CTestCaseParameter< bool > *pIsFrontFace = AddParameter< bool >( _T("GenerateIsFrontFace"), &m_bIsFrontFace);
	CTestCaseParameter< bool > *pNoExpected = AddParameter< bool >( _T("NoExpected"), &m_bNoExpected);
	CTestCaseParameter< UINT > *pVerticesCount = AddParameter< UINT >( _T("VerticesCount"), &m_VerticesCount);
	CTestCaseParameter< D3D11_FILL_MODE > *pFillMode = AddParameter< D3D11_FILL_MODE >( _T("FillMode"), &m_FillMode);

	//Create the factors

	CUserValueSet< D3D11_PRIMITIVE_TOPOLOGY > *pPrimitiveTopologyValues = new CUserValueSet< D3D11_PRIMITIVE_TOPOLOGY >();
	pPrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	pPrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	pPrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
	pPrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ );
	pPrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ );
	testfactor::RFactor rPointLinePrimitiveTopology = AddParameterValueSet< D3D11_PRIMITIVE_TOPOLOGY >( pPrimitiveTopology, pPrimitiveTopologyValues );

	CUserValueSet< D3D11_PRIMITIVE_TOPOLOGY > *pTrianglePrimitiveTopologyValues = new CUserValueSet< D3D11_PRIMITIVE_TOPOLOGY >();
	pTrianglePrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pTrianglePrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	pTrianglePrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ );
	pTrianglePrimitiveTopologyValues->AddValue( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ );
	testfactor::RFactor rTrianglePrimitiveTopology = AddParameterValueSet< D3D11_PRIMITIVE_TOPOLOGY >( pPrimitiveTopology, pTrianglePrimitiveTopologyValues );

	CUserValueSet< D3D11_FILL_MODE > *pFillModeValues = new CUserValueSet< D3D11_FILL_MODE >();
	pFillModeValues->AddValue( D3D11_FILL_WIREFRAME );
	pFillModeValues->AddValue( D3D11_FILL_SOLID );
	testfactor::RFactor rFillMode = AddParameterValueSet< D3D11_FILL_MODE >( pFillMode, pFillModeValues );
	SetParameterDefaultValue(pFillMode, D3D11_FILL_SOLID);

	CUserValueSet< UINT > *pVertexCountValues = new CUserValueSet< UINT >();
	pVertexCountValues->AddValue( 1 );
	pVertexCountValues->AddValue( 2 );
	pVertexCountValues->AddValue( 3 );
	pVertexCountValues->AddValue( 4 );
	pVertexCountValues->AddValue( 5 );
	pVertexCountValues->AddValue( 6 );
	pVertexCountValues->AddValue( 7 );
	pVertexCountValues->AddValue( 15 );
	pVertexCountValues->AddValue( 50 );
	testfactor::RFactor rVertexCount = AddParameterValueSet< UINT >( pVerticesCount, pVertexCountValues );

	//TODO: should have different value sets for different topology so that we don't try to render primitives that doesn't have enough vertices
	//		or have a way to skip these cases without returning RESULT_SKIP;

	testfactor::RFactor rHugeVertexCount = AddParameterValue< UINT >( pVerticesCount, 0x80000 );//16bit limit for all primitive types

	testfactor::RFactor rNoExpectedTrue = AddParameterValue< bool >( pNoExpected, true );
	testfactor::RFactor rNoExpectedFalse = AddParameterValue< bool >( pNoExpected, false );
	testfactor::RFactor rIsFrontFaceTrue = AddParameterValue< bool >( pIsFrontFace, true );
	testfactor::RFactor rIsFrontFaceFalse = AddParameterValue< bool >( pIsFrontFace, false );
	SetParameterDefaultValue(pNoExpected, false);
	SetParameterDefaultValue(pIsFrontFace, false);

	testfactor::RFactor rPixelShader = (rNoExpectedTrue * rIsFrontFaceFalse) + (rNoExpectedFalse * rIsFrontFaceTrue) + (rNoExpectedFalse * rIsFrontFaceFalse);
	testfactor::RFactor rPrimitives = rPointLinePrimitiveTopology + ( rTrianglePrimitiveTopology * rFillMode );

	testfactor::RFactor rHugeDraw = rHugeVertexCount * rPrimitives * rNoExpectedTrue;
	testfactor::RFactor rSmallDraw = rVertexCount * rPrimitives * rPixelShader;

	SetRootTestFactor(  rSmallDraw + rHugeDraw );

	// make cases with vertice count larger than 4K lower priority
	AddPriorityPatternFilter( FilterGreater<UINT>(_T("VerticesCount"), 0x1000), WeightFactorSmall);
	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

TEST_RESULT CGeneratedValuesTest::Setup()
{
	TEST_RESULT tr = ID3D11PixelShaderTest::Setup();
	if( tr != RESULT_PASS )
		return tr;

	HRESULT hr;

	//
	// Create Input elements
	//
	D3D11_INPUT_ELEMENT_DESC iedesc[] = 
	{
		{ "POS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ExpectedId", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ExpectedIsFront", 0, DXGI_FORMAT_R32_UINT, 1, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//
	// Create Vertex Shaders and Input Layout
	//
	tr = SetupVSFromResource( "GeneratedValues.hlsl", "VS", iedesc, 3, &m_pVS, &m_pInputLayout );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupVSFromResource( "GeneratedValues.hlsl", "VSNoExpected", NULL, 0, &m_pVSNoExpected, NULL );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Create Pixel Shaders
	//
	tr = SetupPSFromResource( "GeneratedValues.hlsl", "PS", &m_pPS );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupPSFromResource( "GeneratedValues.hlsl", "PSWithFrontFace", &m_pPSWithFrontFace );
	if( tr != RESULT_PASS )
		return tr;

	tr = SetupPSFromResource( "GeneratedValues.hlsl", "PSNoExpected", &m_pPSNoExpected );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Create Vertex Buffers
	//
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = MaxVBPosSize;

	if ( FAILED( hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pVBPos ) ) )
	{
		WriteToLog( "CreateBuffer() failed : %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	bufferDesc.ByteWidth = MaxVBExpectedSize;

	if ( FAILED( hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pVBExpected ) ) )
	{
		WriteToLog( "CreateBuffer() failed : %s", D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	//
	// Set Input Layout
	//
	m_pDeviceContext->IASetInputLayout( m_pInputLayout );

	//
	// Set Vertex Buffers
	//
	UINT Offsets[2] = { 0, 0 };
	UINT Strides[2] = { sizeof(float) * 4, sizeof(ExpectedResults) };
	ID3D11Buffer *pBuffers[2] = { m_pVBPos, m_pVBExpected };

	m_pDeviceContext->IASetVertexBuffers( 0, 2, pBuffers, Strides, Offsets );

	return RESULT_PASS;	
}

void CGeneratedValuesTest::Cleanup()
{
	ID3D11PixelShaderTest::Cleanup();

	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pVSNoExpected );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pPSWithFrontFace );
	SAFE_RELEASE( m_pPSNoExpected );
	SAFE_RELEASE( m_pVBPos );
	SAFE_RELEASE( m_pVBExpected );
}


TEST_RESULT CGeneratedValuesTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;
	ID3D11RasterizerState *pRasterizerState = NULL;

	UINT vertPerPrimitive = GetVerticesPerPrimitive(m_PrimitiveTopology);
	if (m_VerticesCount < vertPerPrimitive )
		return RESULT_SKIP;

	//
	//Set Rasterizer State
	//
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.FillMode = m_FillMode;
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FrontCounterClockwise = FALSE;
	rastDesc.DepthBias = 0;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = FALSE;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.MultisampleEnable = FALSE;
	rastDesc.AntialiasedLineEnable = FALSE;

	hr = m_pDevice->CreateRasterizerState(&rastDesc, &pRasterizerState);
	if (FAILED(hr))
	{
		WriteToLog( _T( "CGeneratedValuesTest::ExecuteTestCase: CreateRasterizerState() failed. hr = %s" ), 
			D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;

	}
	m_pDeviceContext->RSSetState( pRasterizerState );

	//
	// Set Vertex Shader
	//
	if (m_bNoExpected)
	{
		m_pDeviceContext->VSSetShader( m_pVSNoExpected, NULL, 0 );
	}
	else
	{
		m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
	}

	//
	// Set Geometry Shader
	//
	m_pDeviceContext->GSSetShader( NULL, NULL, 0 );

	//
	// Set Pixel Shader
	//
	if (m_bIsFrontFace)
	{
		m_pDeviceContext->PSSetShader( m_pPSWithFrontFace, NULL, 0 );
	}
	else if (m_bNoExpected)
	{
		m_pDeviceContext->PSSetShader( m_pPSNoExpected, NULL, 0 );
	}
	else
	{
		m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );
	}

	//
	// Set Primitive Topology
	//
	m_pDeviceContext->IASetPrimitiveTopology( m_PrimitiveTopology );

	//
	// Calculate expected values
	//
	ExpectedResults *pExpected = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = m_pDeviceContext->Map( m_pVBExpected, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	if (!SUCCEEDED(hr))
	{
		WriteToLog( _T( "CGeneratedValuesTest::ExecuteTestCase: Map on m_pVBExpected failed.hr = %s" ), 
			D3DHResultToString(hr).c_str() );	
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;
	}

	pExpected = (ExpectedResults*) mappedRes.pData;


	//TODO: for DrawIndexed with cut values, this will need to be fixed
	for (UINT i=0; i<m_VerticesCount; i++)
	{
		switch (m_PrimitiveTopology)
		{
			case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
				pExpected[i].ExpectedId = i;
				break;

			case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
				pExpected[i].ExpectedId = i / 2;
				break;

			case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
				pExpected[i].ExpectedId = i / 4;
				break;

			case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
				pExpected[i].ExpectedId = i / 3;
				break;

			case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
				pExpected[i].ExpectedId = i / 6;
				break;

			//2 lines per strip
			case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
				pExpected[i].ExpectedId = i;
				break;

			case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
				pExpected[i].ExpectedId = i == 0 ? 0 : i - 1;
				break;

			//2 triangles per strip
			case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
				pExpected[i].ExpectedId = i;
				break;

			case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
				pExpected[i].ExpectedId = i / 2;
				break;
		}
		pExpected[i].ExpectedIsFront = 0xffffffff;
	}

	m_pDeviceContext->Unmap(m_pVBExpected,0);


	//
	// Fill Positions Vertex Buffer
	//
	VERTEXPOS *pVertexPos = NULL;
	hr = m_pDeviceContext->Map( m_pVBPos, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	if (!SUCCEEDED(hr))
	{
		WriteToLog( _T( "CGeneratedValuesTest::ExecuteTestCase: Map on m_pVBPos failed.hr = %s" ), 
			D3DHResultToString(hr).c_str() );	
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;
	}

	pVertexPos = (VERTEXPOS*) mappedRes.pData;
	GenerateData( m_VerticesCount, m_PrimitiveTopology, pVertexPos );

	m_pDeviceContext->Unmap(m_pVBPos,0);

	//
	//Clear RenderTarget
	//
	FLOAT colors[4] = { 42.0f, 163.0f, 12.0f, 211.0f };
	UINT clearValue = 0xd30ca32a; //42, 163, 12, 211 = 0x2a, 0xa3, 0x0c, 0xd3
	m_pDeviceContext->ClearRenderTargetView( m_pRTView, colors );

	//
	// Draw
	//
	m_pDeviceContext->Draw( m_VerticesCount, 0 );

	//
	//Copy rendertarget and check results
	//
	UINT *pResult;
	m_pDeviceContext->CopyResource( m_pRTTexture2DCopy, m_pRTTexture2D );

	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;
	}

	D3D11_MAPPED_SUBRESOURCE texmap;
	hr = GetImmediateContext()->Map( m_pRTTexture2DCopy, 0, D3D11_MAP_READ, 0, &texmap );
	if (!SUCCEEDED(hr))
	{
		WriteToLog( _T( "CGeneratedValuesTest::ExecuteTestCase: Map on m_pRTTexture2DCopy failed.hr = %s" ), 
			D3DHResultToString(hr).c_str() );	
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;
	}

	pResult = (UINT *) texmap.pData;

	bool bRendered = false;
	//need to look at every pixels!
	//TODO: This is not optimal and could miss some rendered primitives.  Should make sure all expected lit pixels have correct result.
	for (int y=0; y<64; y++)
	{
		pResult = (UINT*)((BYTE*)texmap.pData + y * texmap.RowPitch);
		for (int x=0; x<64; x++)
		{
			if (pResult[x] == clearValue)
				continue;

			bRendered = true;

			BYTE* pByteResult = (BYTE*)(pResult + x);

			int expectedIdResult = pByteResult[0];
			int expectedIsFrontResult = pByteResult[1];

			int dummy1 = pByteResult[2];
			int dummy2 = pByteResult[3];

			if (!m_bNoExpected)
			{
				if (expectedIdResult != 255)
				{
					WriteToLog( _T( "PrimitiveId (%u) didn't return expected value : %u" ), dummy1, dummy2 );
					tRes = RESULT_FAIL;
					goto TESTFAIL_2;
				}

				if (m_bIsFrontFace && expectedIsFrontResult == 0)
				{
					WriteToLog( _T( "IsFrontFace didn't return expected value" ) );
					tRes = RESULT_FAIL;
					goto TESTFAIL_2;
				}
			}
		}
	}

	if (!m_bNoExpected)
	{
		if (!bRendered && vertPerPrimitive <= m_VerticesCount)
		{
			WriteToLog( _T( "Nothing rendered when expected something" ) );
			tRes = RESULT_FAIL;
			goto TESTFAIL_2;
		}
	}
	
	//
	//Present
	//
	if (CanPresent())
	{
		m_PresentHelper.PresentTexture2D(m_pRTTexture2D);
	}
	ShowResource(L"Render Target", m_pRTTexture2D);

TESTFAIL_2:
	GetImmediateContext()->Unmap( m_pRTTexture2DCopy, 0 );
TESTFAIL_1:
	m_pDeviceContext->RSSetState( NULL );
	SAFE_RELEASE( pRasterizerState );
	
	return tRes;

}
