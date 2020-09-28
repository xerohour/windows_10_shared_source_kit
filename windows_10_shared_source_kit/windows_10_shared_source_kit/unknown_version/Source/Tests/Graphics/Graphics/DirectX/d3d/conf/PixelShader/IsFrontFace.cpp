#include "PixelShader.h"

const char szPSInputStartStruct[] = 
	"struct PSIn"	"\n"
	"{"				"\n"
	"	uint isFrontFace : SV_IsFrontFace;"	"\n"
	"	float4 pos : SV_Position;"			"\n"
	"	float4 dummy : DUMMY;"				"\n"
	"};"				"\n"
	;

const char szPSInputMiddleStruct[] = 
	"struct PSIn"	"\n"
	"{"				"\n"
	"	float4 pos : SV_Position;"			"\n"
	"	uint isFrontFace : SV_IsFrontFace;"	"\n"
	"	float4 dummy : DUMMY;"				"\n"
	"};"				"\n"
	;

const char szPSInputEndStruct[] = 
	"struct PSIn"	"\n"
	"{"				"\n"
	"	float4 pos : SV_Position;"			"\n"
	"	float4 dummy : DUMMY;"				"\n"
	"	uint isFrontFace : SV_IsFrontFace;"	"\n"
	"};"				"\n"
	;

const char szPSInputOnlyStruct[] = 
	"struct PSIn"	"\n"
	"{"				"\n"
	"	uint isFrontFace : SV_IsFrontFace;"	"\n"
	"};"				"\n"
	;

const char szPSOutputStruct[] = 
	"struct PSOut"	"\n"
	"{"				"\n"
	"	uint4 color : SV_Target;"	"\n"
	"};"				"\n"
	;

const char szPSOnlyIsFrontFace[] = 
	"PSOut main( PSIn input )\n"
	"{\n"
	"	PSOut output;\n"
	"	uint frontface = input.isFrontFace == 0xffffffff ? 255 : 128; \n"
	"	output.color = uint4(1, 1, frontface, 1); \n"
	"	return output;\n"
	"}\n"
	;

const char szPS[] = 
	"PSOut main( PSIn input )\n"
	"{\n"
	"	PSOut output;\n"
	"	uint frontface = input.isFrontFace == 0xffffffff ? 255 : 128; \n"
	"	output.color = uint4(input.pos.x, input.pos.y, frontface, input.dummy.z); \n"
	"	return output;\n"
	"}\n"
	;

const float triangleStripDataZeroArea[numTriangleStripData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.3f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.3f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f}
};

const float triangleStripAdjDataZeroArea[numTriangleStripAdjData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.5f, 0.0f, 1.0f},
	{0.1f, 0.5f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.5f, 0.0f, 1.0f},
	{0.1f, 0.5f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},
};

const float triangleListDataZeroArea[numTriangleListData][4] =
{
	{0.1f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.1f, 0.0f, 1.0f},

	{0.35f, 0.55f, 0.0f, 1.0f},
	{0.7f, 0.9f, 0.0f, 1.0f},
	{0.35f, 0.55f, 0.0f, 1.0f},

	{0.55f, 0.1f, 0.0f, 1.0f},
	{0.7f, 0.5f, 0.0f, 1.0f},
	{0.55f, 0.1f, 0.0f, 1.0f}
};

const float triangleListAdjDataZeroArea[numTriangleListAdjData][4] =
{
	{0.3f, 0.1f, 0.0f, 1.0f},
	{0.1f, 0.3f, 0.0f, 1.0f},
	{0.1f, 0.3f, 0.0f, 1.0f},
	{0.1f, 0.3f, 0.0f, 1.0f},
	{0.3f, 0.1f, 0.0f, 1.0f},
	{0.3f, 0.1f, 0.0f, 1.0f},

	{0.5f, 0.6f, 0.0f, 1.0f},
	{0.3f, 0.9f, 0.0f, 1.0f},
	{0.3f, 0.9f, 0.0f, 1.0f},
	{0.3f, 0.9f, 0.0f, 1.0f},
	{0.5f, 0.6f, 0.0f, 1.0f},
	{0.5f, 0.6f, 0.0f, 1.0f},

	{0.7f, 0.1f, 0.0f, 1.0f},
	{0.5f, 0.3f, 0.0f, 1.0f},
	{0.5f, 0.3f, 0.0f, 1.0f},
	{0.5f, 0.3f, 0.0f, 1.0f},
	{0.7f, 0.1f, 0.0f, 1.0f},
	{0.7f, 0.1f, 0.0f, 1.0f},
};

BEGIN_NAMED_VALUES( IsFrontFaceInStruct )
	NAMED_VALUE( _T( "Start" ), Start )
	NAMED_VALUE( _T( "Middle" ), Middle )
	NAMED_VALUE( _T( "End" ), End )
	NAMED_VALUE( _T( "Only" ), Only )
END_NAMED_VALUES( IsFrontFaceInStruct )

void CIsFrontFaceTest::InitTestParameters()
{
	//Initialize root factors
	ID3D11PixelShaderTest::InitTestParameters();

	//Create the parameters
	CTestCaseParameter< BOOL > *pFrontCounterClockwise = AddParameter< BOOL >( _T("FrontCounterClockwise"), &m_FrontCounterClockwise);
	CTestCaseParameter< D3D11_CULL_MODE > *pCullMode = AddParameter< D3D11_CULL_MODE >( _T("CullMode"), &m_CullMode);
	CTestCaseParameter< D3D11_FILL_MODE > *pFillMode = AddParameter< D3D11_FILL_MODE >( _T("FillMode"), &m_FillMode);
	CTestCaseParameter< D3D11_PRIMITIVE_TOPOLOGY > *pPrimitiveTopology = AddParameter< D3D11_PRIMITIVE_TOPOLOGY >( _T("PrimitiveTopology"), &m_PrimitiveTopology);
	CTestCaseParameter< IsFrontFaceInStruct > *pIsFrontFaceInStruct = AddParameter< IsFrontFaceInStruct >( _T("IsFrontFaceInStruct"), &m_IsFrontFaceInStruct);
	CTestCaseParameter< bool > *pClockwiseData = AddParameter< bool >( _T("ClockwiseData"), &m_ClockwiseData);

	//CTestCaseParameter< bool > *pVertexID = AddParameter< bool >( _T("GenerateVertexID"), &m_bVertexID);
	//CTestCaseParameter< DrawMethod > *pDrawMethod = AddParameter( _T("DrawMethod"), &m_DrawMethod);


	//Create the factors

	CUserValueSet< BOOL > *pFrontCounterClockwiseValues = new CUserValueSet< BOOL >();
	pFrontCounterClockwiseValues->AddValue( FALSE );
	pFrontCounterClockwiseValues->AddValue( TRUE );
	testfactor::RFactor rFrontCounterClockwise = AddParameterValueSet< BOOL >( pFrontCounterClockwise, pFrontCounterClockwiseValues );

	CUserValueSet< D3D11_CULL_MODE > *pCullModeValues = new CUserValueSet< D3D11_CULL_MODE >();
	pCullModeValues->AddValue( D3D11_CULL_NONE );
	pCullModeValues->AddValue( D3D11_CULL_FRONT );
	pCullModeValues->AddValue( D3D11_CULL_BACK );
	testfactor::RFactor rCullMode = AddParameterValueSet< D3D11_CULL_MODE >( pCullMode, pCullModeValues );

	CUserValueSet< D3D11_FILL_MODE > *pFillModeValues = new CUserValueSet< D3D11_FILL_MODE >();
	pFillModeValues->AddValue( D3D11_FILL_WIREFRAME );
	pFillModeValues->AddValue( D3D11_FILL_SOLID );
	testfactor::RFactor rFillMode = AddParameterValueSet< D3D11_FILL_MODE >( pFillMode, pFillModeValues );

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

	testfactor::RFactor rClockwiseData = AddParameterValueSet< bool >( pClockwiseData, BoolValueSet() );

	SetParameterDefaultValue(pClockwiseData, true);

	testfactor::RFactor rPrimitiveTopology;
	if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
	{
		CTestCaseParameter< bool > *pZeroArea = AddParameter< bool >( _T("ZeroArea"), &m_ZeroArea);
		SetParameterDefaultValue(pZeroArea, false);
		testfactor::RFactor rZeroArea = AddParameterValue< bool >( pZeroArea, true );
		rPrimitiveTopology = rPointLinePrimitiveTopology + (rTrianglePrimitiveTopology * ( rClockwiseData + rZeroArea ) );
	}
	else
	{
		m_ZeroArea = false;
		rPrimitiveTopology = rPointLinePrimitiveTopology + (rTrianglePrimitiveTopology * rClockwiseData);
	}

	CUserValueSet< IsFrontFaceInStruct > *pIsFrontFaceInStructValues = new CUserValueSet< IsFrontFaceInStruct >();
	//BUGBUG: Disabled until 1613565 is resolved
	//pIsFrontFaceInStructValues->AddValue( Start );
	//pIsFrontFaceInStructValues->AddValue( Middle );
	pIsFrontFaceInStructValues->AddValue( End );
	pIsFrontFaceInStructValues->AddValue( Only );
	testfactor::RFactor rIsFrontFaceInStruct = AddParameterValueSet< IsFrontFaceInStruct >( pIsFrontFaceInStruct, pIsFrontFaceInStructValues );

	SetRootTestFactor( rFrontCounterClockwise * rCullMode * rFillMode * rPrimitiveTopology * rIsFrontFaceInStruct );
}

TEST_RESULT CIsFrontFaceTest::Setup()
{
	TEST_RESULT tr = ID3D11PixelShaderTest::Setup();	
	if (tr != RESULT_PASS)
		return tr;

	//
	//Set states
	//
	m_pDeviceContext->VSSetShader( m_pPassThroughVS, NULL, 0 );
	m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
	m_pDeviceContext->IASetInputLayout( m_pInputLayout );

	tr = RESULT_PASS;

//FAIL:
	return tr;
}


TEST_RESULT CIsFrontFaceTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;
	ID3D11RasterizerState *pRasterizerState = NULL;

	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;
	ID3D11PixelShader *pPS = NULL;

	//
	//Create Pixel Shader
	//

	tstring shader;
	switch (m_IsFrontFaceInStruct)
	{
		case Start:
			shader = tstring(szPSInputStartStruct) + tstring(szPSOutputStruct) + tstring(szPS);
			break;

		case Middle:
			shader = tstring(szPSInputMiddleStruct) + tstring(szPSOutputStruct) + tstring(szPS);
			break;

		case End:
			shader = tstring(szPSInputEndStruct) + tstring(szPSOutputStruct) + tstring(szPS);
			break;

		case Only:
			shader = tstring(szPSInputOnlyStruct) + tstring(szPSOutputStruct) + tstring(szPSOnlyIsFrontFace);
			break;
	}

	hr = D3DX10CompileFromMemory( shader.c_str(), strlen( shader.c_str() ), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CIsFrontFaceTest::ExecuteTestCase() CompileShaderFromMemory() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;

	}

	if( FAILED( m_pDevice->CreatePixelShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &pPS ) ) )
	{
		WriteToLog( _T( "CIsFrontFaceTest::ExecuteTestCase() failed creating pixelShader" ) );
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;

	}
	m_pDeviceContext->PSSetShader( pPS, NULL, 0 );

	//
	//Set Rasterizer State
	//
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.FillMode = m_FillMode;
	rastDesc.CullMode = m_CullMode;
	rastDesc.FrontCounterClockwise = m_FrontCounterClockwise;
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
		WriteToLog( _T( "CIsFrontFaceTest::ExecuteTestCase: CreateRasterizerState() failed. hr = %s" ), 
			D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;

	}
	m_pDeviceContext->RSSetState( pRasterizerState );

	//
	//Set PrimitiveTopology
	//
	m_pDeviceContext->IASetPrimitiveTopology( m_PrimitiveTopology );

	//
	//Set vertex buffer
	//
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA subData;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	UINT numVertices = 0;
	switch (m_PrimitiveTopology)
	{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST :
			numVertices = numPointListData;
			subData.pSysMem = (void *)pointListData;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST :
			numVertices = numLineListData;
			subData.pSysMem = (void *)lineListData;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ :
			numVertices = numLineListAdjData;
			subData.pSysMem = (void *)lineListAdjData;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP :
			numVertices = numLineStripData;
			subData.pSysMem = (void *)lineStripData;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ :
			numVertices = numLineStripAdjData;
			subData.pSysMem = (void *)lineStripAdjData;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST :
			numVertices = numTriangleListData;
			subData.pSysMem = m_ZeroArea ? (void*)triangleListDataZeroArea : m_ClockwiseData ? (void *)triangleListDataCW : (void *)triangleListDataCCW;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ :
			numVertices = numTriangleListAdjData;
			subData.pSysMem = m_ZeroArea ? (void*)triangleListAdjDataZeroArea : m_ClockwiseData ? (void *)triangleListAdjDataCW : (void *)triangleListAdjDataCCW;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP :
			numVertices = numTriangleStripData;
			subData.pSysMem = m_ZeroArea ? (void*)triangleStripDataZeroArea : m_ClockwiseData ? (void *)triangleStripDataCW : (void *)triangleStripDataCCW;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ :
			numVertices = numTriangleStripAdjData;
			subData.pSysMem = m_ZeroArea ? (void*)triangleStripAdjDataZeroArea : m_ClockwiseData ? (void *)triangleStripAdjDataCW : (void *)triangleStripAdjDataCCW;
			break;

	}
	bufferDesc.ByteWidth = sizeof(float) * 4 * numVertices;

	if ( FAILED( hr = m_pDevice->CreateBuffer( &bufferDesc, &subData, &m_pVertexBuffer ) ) )
	{
		WriteToLog( "CreateBuffer() failed : %s", D3DHResultToString( hr ).c_str() );
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;

	}
	UINT Offsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	UINT FormatStride = GetFormatStrideInBytes(DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &FormatStride, Offsets);

	//
	//Clear RenderTarget
	//
	FLOAT colors[4] = { 42.0f, 163.0f, 12.0f, 211.0f };
	UINT clearValue = 0xd30ca32a; //42, 163, 12, 211 = 0x2a, 0xa3, 0x0c, 0xd3
	m_pDeviceContext->ClearRenderTargetView( m_pRTView, colors );

	//
	//Draw
	//
	m_pDeviceContext->Draw(numVertices, 0);

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
		WriteToLog( _T( "CIsFrontFaceTest::ExecuteTestCase: m_pRTTexture2DCopy->Map failed.hr = %s" ), 
			D3DHResultToString(hr).c_str() );	
		tRes = RESULT_FAIL;
		goto TESTFAIL_1;
	}


	pResult = (UINT *) texmap.pData;

	UINT expectedIsFrontValue = GetExpectedFrontFace();

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

			int xValue = pByteResult[0];
			int yValue = pByteResult[1];
			int isFrontValue = pByteResult[2];

			int dummy = pByteResult[3];

			if (isFrontValue != expectedIsFrontValue)
			{
				WriteToLog( _T( "IsFrontFace returned %u when expecting %u (255 means TRUE, 128 means FALSE) for pixel %u,%u" ), isFrontValue, expectedIsFrontValue, x, y );
				tRes = RESULT_FAIL;
				goto TESTFAIL_2;
			}
		}
	}

	if (bRendered && expectedIsFrontValue == 0)
	{
		WriteToLog( _T( "Something rendered when none expected" ) );
		tRes = RESULT_FAIL;
		goto TESTFAIL_2;
	}
	else if (!bRendered && expectedIsFrontValue != 0)
	{
		WriteToLog( _T( "Nothing rendered when expected something" ) );
		tRes = RESULT_FAIL;
		goto TESTFAIL_2;
	}

	//
	//Present
	//
	if (CanPresent())
	{
		m_PresentHelper.PresentTexture2D(m_pRTTexture2D, m_pSRV);
	}
	ShowResource(L"Render Target", m_pRTTexture2D);
	
TESTFAIL_2:
	GetImmediateContext()->Unmap( m_pRTTexture2DCopy, 0 );

TESTFAIL_1:	

	m_pDeviceContext->RSSetState( NULL );
	m_pDeviceContext->PSSetShader( NULL, NULL, 0 );

	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );
	SAFE_RELEASE( pRasterizerState );
	SAFE_RELEASE( pPS );

	ID3D11Buffer *pBuffers[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	UINT Strides[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	m_pDeviceContext->IASetVertexBuffers(0, 16, pBuffers, Strides, Offsets);
	SAFE_RELEASE( m_pVertexBuffer );

	return tRes;
}

UINT CIsFrontFaceTest::GetExpectedFrontFace()
{
	switch (m_PrimitiveTopology)
	{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST :
    	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST :
    	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP :
    	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ :
    	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ :
			return 0xff;
	}

	bool isFront =	m_ZeroArea ? false :	(m_FrontCounterClockwise == FALSE && m_ClockwiseData) ||
											(m_FrontCounterClockwise != FALSE && !m_ClockwiseData);

	if (m_ZeroArea && m_FillMode == D3D11_FILL_SOLID)
	{
		//Degenerate triangles are not rendered, but the wireframes are.
		return 0;
	}
	else if (m_CullMode == D3D11_CULL_FRONT && isFront)
	{
		return 0;
	}
	else if (m_CullMode == D3D11_CULL_BACK && !isFront)
	{
		return 0;
	}

	return isFront ? 0xff : 0x80;
}