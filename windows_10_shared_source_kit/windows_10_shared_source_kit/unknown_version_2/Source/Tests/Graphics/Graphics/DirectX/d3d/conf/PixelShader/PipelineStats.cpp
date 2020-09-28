#include "PixelShader.h"

const char szPS[] = 
	"struct PSIn"	"\n"
	"{"				"\n"
	"	float4 pos : SV_Position;"			"\n"
	"	float4 dummy : DUMMY;"				"\n"
	"};"				"\n"
	"struct PSOut"	"\n"
	"{"				"\n"
	"	uint4 color : SV_Target;"	"\n"
	"};"				"\n"
	"PSOut main( PSIn input )\n"
	"{\n"
	"	PSOut output;\n"
	"	output.color = uint4(input.pos.x, input.pos.y, input.dummy.x, input.dummy.z); \n"
	"	return output;\n"
	"}\n"
	;

const char szPSNoInput[] = 
	"struct PSOut"	"\n"
	"{"				"\n"
	"	uint4 color : SV_Target;"	"\n"
	"};"				"\n"
	"PSOut main( )\n"
	"{\n"
	"	PSOut output;\n"
	"	output.color = uint4(1, 2, 3, 4); \n"
	"	return output;\n"
	"}\n"
	;

void CPipelineStatsTest::InitTestParameters()
{
	//Initialize root factors
	ID3D11PixelShaderTest::InitTestParameters();

	//Create the parameters
	CTestCaseParameter< bool > *pNullPS = AddParameter< bool >( _T("NullPS"), &m_NullPS);
	CTestCaseParameter< bool > *pNoInputPS = AddParameter< bool >( _T("NoInputPS"), &m_NoInputPS);
	CTestCaseParameter< bool > *pDepthEnable = AddParameter< bool >( _T("DepthEnable"), &m_DepthEnable);
	CTestCaseParameter< bool > *pStencilEnable = AddParameter< bool >( _T("StencilEnable"), &m_StencilEnable);

	SetParameterDefaultValue(pNullPS, false);

	//Create the factors
	testfactor::RFactor rNullPS = AddParameterValue< bool >( pNullPS, true );
	testfactor::RFactor rNoInputPS = AddParameterValueSet< bool >( pNoInputPS, BoolValueSet() );
	testfactor::RFactor rDepthEnable = AddParameterValueSet< bool >( pDepthEnable, BoolValueSet() );
	testfactor::RFactor rStencilEnable = AddParameterValueSet< bool >( pStencilEnable, BoolValueSet() );

	SetRootTestFactor( (rNullPS + rNoInputPS) * rDepthEnable * rStencilEnable);
}

TEST_RESULT CPipelineStatsTest::Setup()
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


TEST_RESULT CPipelineStatsTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr;
	ID3D11RasterizerState *pRasterizerState = NULL;
	ID3D11Query *pQuery = NULL;
	ID3D11DepthStencilState *pDSState = NULL;

	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;
	ID3D11PixelShader *pPS = NULL;

	if (m_NullPS)
	{
		m_pDeviceContext->PSSetShader( pPS, NULL, 0 );
	}
	else
	{
		//
		//Create Pixel Shader
		//
		const char* szShader = NULL;

		szShader = m_NoInputPS ? szPSNoInput : szPS;

		hr = D3DX10CompileFromMemory( szShader, strlen( szShader ), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pShader, &pError, NULL );

		if( pError )
		{
			WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
		}

		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CPipelineStatsTest::ExecuteTestCase() CompileShaderFromMemory() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tRes = RESULT_FAIL;
			goto testDone;
		}

		if( FAILED( m_pDevice->CreatePixelShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &pPS ) ) )
		{
			WriteToLog( _T( "CPipelineStatsTest::ExecuteTestCase() failed creating pixelShader" ) );
			tRes = RESULT_FAIL;
			goto testDone;
		}
		m_pDeviceContext->PSSetShader( pPS, NULL, 0 );
	}

	//
	//Set Rasterizer State
	//
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = TRUE;
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
		WriteToLog( _T( "CPipelineStatsTest::ExecuteTestCase: CreateRasterizerState() failed. hr = %s" ), 
			D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto testDone;
	}
	m_pDeviceContext->RSSetState( pRasterizerState );

	//
	//Set DepthStencil State
	//
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = m_DepthEnable ? TRUE : FALSE;
	dsDesc.StencilEnable = m_StencilEnable ? TRUE : FALSE;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;

	if ( FAILED( m_pDevice->CreateDepthStencilState( &dsDesc, &pDSState ) ) )
	{
		WriteToLog( "CreateDepthStencilState() failed" );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	m_pDeviceContext->OMSetDepthStencilState( pDSState, NULL );

	//
	//Set PrimitiveTopology
	//
	m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

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
	numVertices = numPointListData;
	subData.pSysMem = (void *)pointListData;
	bufferDesc.ByteWidth = sizeof(float) * 4 * numVertices;

	if ( FAILED( hr = m_pDevice->CreateBuffer( &bufferDesc, &subData, &m_pVertexBuffer ) ) )
	{
		WriteToLog( "CreateBuffer() failed : %s", D3DHResultToString( hr ).c_str() );
		tRes = RESULT_FAIL;
		goto testDone;
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
	//Create Query
	//
	D3D11_QUERY_DESC queryDesc;
	queryDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryDesc.MiscFlags = 0;
	if ( FAILED(hr = m_pDevice->CreateQuery(&queryDesc, &pQuery)) )
	{
		WriteToLog( _T("Failed to create Query. hr=%s\n"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto testDone;
	}


	//
	//Get initial stats
	//
	//pQuery->Begin();
	//pQuery->End();
	//D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeStatsBegin;
	//while ( S_FALSE == pQuery->GetData( &pipeStatsBegin, sizeof(pipeStatsBegin), 0 ) );


	m_pDeviceContext->Begin(pQuery);

	//
	//Draw
	//
	m_pDeviceContext->Draw(numVertices, 0);

	//
	//Get end stats
	//
	m_pDeviceContext->End(pQuery);

	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeStatsEnd;
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pQuery, &pipeStatsEnd, sizeof(pipeStatsEnd), 0 );
	if (hr != S_OK)
	{
		WriteToLog( _T( "pQuery->GetData failed or timed out with %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	UINT64 expectedValue = m_NullPS && !m_DepthEnable && !m_StencilEnable ? 0 : numVertices;
	if (pipeStatsEnd.CInvocations != expectedValue)
	{
		WriteToLog( _T( "CInvocation (%d) is not the expected value (%d)." ), (UINT)pipeStatsEnd.CInvocations, (UINT)expectedValue );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	if (pipeStatsEnd.CPrimitives != expectedValue)
	{
		WriteToLog( _T( "CPrimitives (%d) is not the expected value (%d)." ), (UINT)pipeStatsEnd.CPrimitives, (UINT)expectedValue );
		tRes = RESULT_FAIL;
		goto testDone;
	}


	if (m_NullPS ? pipeStatsEnd.PSInvocations != 0 : pipeStatsEnd.PSInvocations == 0)
	{
		WriteToLog( _T( "PSInvocations (%d) is not the expected value." ), (UINT)pipeStatsEnd.PSInvocations );
		tRes = RESULT_FAIL;
		goto testDone;
	}



testDone:

	
	//
	//Present
	//
	if (CanPresent())
	{
		m_PresentHelper.PresentTexture2D(m_pRTTexture2D);
	}
	ShowResource(L"Render Target", m_pRTTexture2D);

	m_pDeviceContext->RSSetState( NULL );
	m_pDeviceContext->PSSetShader( NULL, NULL, 0 );

	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );
	SAFE_RELEASE( pQuery );
	SAFE_RELEASE( pRasterizerState );
	SAFE_RELEASE( pDSState );
	SAFE_RELEASE( pPS );

	ID3D11Buffer *pBuffers[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	UINT Strides[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	m_pDeviceContext->IASetVertexBuffers(0, 16, pBuffers, Strides, Offsets);
	SAFE_RELEASE( m_pVertexBuffer );

	return tRes;
}
