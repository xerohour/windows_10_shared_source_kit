// ***************************************************************
//  GSPrimID   version:  1.0   ·  date: 02/23/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "GSPrimID.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_NAMED_VALUES( GS_INPUT_PRIM_TYPE )
	NAMED_VALUE( "POINT", PRIM_TYPE_POINT )
	NAMED_VALUE( "LINE", PRIM_TYPE_LINE )
	NAMED_VALUE( "LINE_ADJ", PRIM_TYPE_LINE_ADJ )
	NAMED_VALUE( "TRIANGLE", PRIM_TYPE_TRIANGLE )
	NAMED_VALUE( "TRIANGLE_ADJ", PRIM_TYPE_TRIANGLE_ADJ )
END_NAMED_VALUES( GS_INPUT_PRIM_TYPE )


//////////////////////////////////////////////////////////////////////////

struct VS_IN
{
	UINT data[4];
};


//////////////////////////////////////////////////////////////////////////

static const VS_IN inputDataTriList[] = 
{
	{ 0x1000, 0x1001, 0x1002, 0x1003 },
	{ 0x2000, 0x2001, 0x2002, 0x2003 },
	{ 0x3000, 0x3001, 0x3002, 0x3003 },
									 
	{ 0x1100, 0x1101, 0x1102, 0x1103 },
	{ 0x2100, 0x2101, 0x2102, 0x2103 },
	{ 0x3100, 0x3101, 0x3102, 0x3103 },
									 
	{ 0x1200, 0x1201, 0x1202, 0x1203 },
	{ 0x2200, 0x2201, 0x2202, 0x2203 },
	{ 0x3200, 0x3201, 0x3202, 0x3203 },
									 
	{ 0x1300, 0x1301, 0x1302, 0x1303 },
	{ 0x2300, 0x2301, 0x2302, 0x2303 },
	{ 0x3300, 0x3301, 0x3302, 0x3303 },
									 
	{ 0x1400, 0x1401, 0x1402, 0x1403 },
	{ 0x2400, 0x2401, 0x2402, 0x2403 },
	{ 0x3400, 0x3401, 0x3402, 0x3403 },
									 
	{ 0x1500, 0x1501, 0x1502, 0x1503 },
	{ 0x2500, 0x2501, 0x2502, 0x2503 },
	{ 0x3500, 0x3501, 0x3502, 0x3503 },
									 
	{ 0x1600, 0x1601, 0x1602, 0x1603 },
	{ 0x2600, 0x2601, 0x2602, 0x2603 },
	{ 0x3600, 0x3601, 0x3602, 0x3603 },
									 
	{ 0x1700, 0x1701, 0x1702, 0x1703 },
	{ 0x2700, 0x2701, 0x2702, 0x2703 },
	{ 0x3700, 0x3701, 0x3702, 0x3703 },
									 
	{ 0x1800, 0x1801, 0x1802, 0x1803 },
	{ 0x2800, 0x2801, 0x2802, 0x2803 },
	{ 0x3800, 0x3801, 0x3802, 0x3803 },
									 
	{ 0x1900, 0x1901, 0x1902, 0x1903 },
	{ 0x2900, 0x2901, 0x2902, 0x2903 },
	{ 0x3900, 0x3901, 0x3902, 0x3903 },
								 	
	{ 0x1a00, 0x1a01, 0x1a02, 0x1a03 },
	{ 0x2a00, 0x2a01, 0x2a02, 0x2a03 },
	{ 0x3a00, 0x3a01, 0x3a02, 0x3a03 },
								 	
	{ 0x1b00, 0x1b01, 0x1b02, 0x1b03 },
	{ 0x2b00, 0x2b01, 0x2b02, 0x2b03 },
	{ 0x3b00, 0x3b01, 0x3b02, 0x3b03 },
								 	
	{ 0x1c00, 0x1c01, 0x1c02, 0x1c03 },
	{ 0x2c00, 0x2c01, 0x2c02, 0x2c03 },
	{ 0x3c00, 0x3c01, 0x3c02, 0x3c03 },
								 	
	{ 0x1d00, 0x1d01, 0x1d02, 0x1d03 },
	{ 0x2d00, 0x2d01, 0x2d02, 0x2d03 },
	{ 0x3d00, 0x3d01, 0x3d02, 0x3d03 },
								 	
	{ 0x1e00, 0x1e01, 0x1e02, 0x1e03 },
	{ 0x2e00, 0x2e01, 0x2e02, 0x2e03 },
	{ 0x3e00, 0x3e01, 0x3e02, 0x3e03 },
								 	
	{ 0x1f00, 0x1f01, 0x1f02, 0x1f03 },
	{ 0x2f00, 0x2f01, 0x2f02, 0x2f03 },
	{ 0x3f00, 0x3f01, 0x3f02, 0x3f03 },
};

static const VS_IN inputDataLineList[] = 
{
	{ 0x1000, 0x1001, 0x1002, 0x1003 },
	{ 0x2000, 0x2001, 0x2002, 0x2003 },
									 
	{ 0x1100, 0x1101, 0x1102, 0x1103 },
	{ 0x2100, 0x2101, 0x2102, 0x2103 },
									 
	{ 0x1200, 0x1201, 0x1202, 0x1203 },
	{ 0x2200, 0x2201, 0x2202, 0x2203 },
									 
	{ 0x1300, 0x1301, 0x1302, 0x1303 },
	{ 0x2300, 0x2301, 0x2302, 0x2303 },
									 
	{ 0x1400, 0x1401, 0x1402, 0x1403 },
	{ 0x2400, 0x2401, 0x2402, 0x2403 },
									 
	{ 0x1500, 0x1501, 0x1502, 0x1503 },
	{ 0x2500, 0x2501, 0x2502, 0x2503 },
									 
	{ 0x1600, 0x1601, 0x1602, 0x1603 },
	{ 0x2600, 0x2601, 0x2602, 0x2603 },
									 
	{ 0x1700, 0x1701, 0x1702, 0x1703 },
	{ 0x2700, 0x2701, 0x2702, 0x2703 },
									 
	{ 0x1800, 0x1801, 0x1802, 0x1803 },
	{ 0x2800, 0x2801, 0x2802, 0x2803 },
									 
	{ 0x1900, 0x1901, 0x1902, 0x1903 },
	{ 0x2900, 0x2901, 0x2902, 0x2903 },
								 	
	{ 0x1a00, 0x1a01, 0x1a02, 0x1a03 },
	{ 0x2a00, 0x2a01, 0x2a02, 0x2a03 },
								 	
	{ 0x1b00, 0x1b01, 0x1b02, 0x1b03 },
	{ 0x2b00, 0x2b01, 0x2b02, 0x2b03 },
								 	
	{ 0x1c00, 0x1c01, 0x1c02, 0x1c03 },
	{ 0x2c00, 0x2c01, 0x2c02, 0x2c03 },
								 	
	{ 0x1d00, 0x1d01, 0x1d02, 0x1d03 },
	{ 0x2d00, 0x2d01, 0x2d02, 0x2d03 },
								 	
	{ 0x1e00, 0x1e01, 0x1e02, 0x1e03 },
	{ 0x2e00, 0x2e01, 0x2e02, 0x2e03 },
								 	
	{ 0x1f00, 0x1f01, 0x1f02, 0x1f03 },
	{ 0x2f00, 0x2f01, 0x2f02, 0x2f03 },
};

static const VS_IN inputDataPointList[] = 
{
	{ 0x1000, 0x1001, 0x1002, 0x1003 },
	{ 0x1100, 0x1101, 0x1102, 0x1103 },
	{ 0x1200, 0x1201, 0x1202, 0x1203 },
	{ 0x1300, 0x1301, 0x1302, 0x1303 },
	{ 0x1400, 0x1401, 0x1402, 0x1403 },
	{ 0x1500, 0x1501, 0x1502, 0x1503 },
	{ 0x1600, 0x1601, 0x1602, 0x1603 },
	{ 0x1700, 0x1701, 0x1702, 0x1703 },
	{ 0x1800, 0x1801, 0x1802, 0x1803 },
	{ 0x1900, 0x1901, 0x1902, 0x1903 },
	{ 0x1a00, 0x1a01, 0x1a02, 0x1a03 },
	{ 0x1b00, 0x1b01, 0x1b02, 0x1b03 },
	{ 0x1c00, 0x1c01, 0x1c02, 0x1c03 },
	{ 0x1d00, 0x1d01, 0x1d02, 0x1d03 },
	{ 0x1e00, 0x1e01, 0x1e02, 0x1e03 },
	{ 0x1f00, 0x1f01, 0x1f02, 0x1f03 },
};



static const UINT numInputTriangles = sizeof( inputDataTriList ) / ( 3 * sizeof( inputDataTriList[0] ) );
static const UINT numInputLines = sizeof( inputDataLineList ) / ( 3 * sizeof( inputDataLineList[0] ) );
static const UINT numInputPoints = sizeof( inputDataPointList ) / ( 3 * sizeof( inputDataPointList[0] ) );


//////////////////////////////////////////////////////////////////////////

CGSPrimIDTest::CGSPrimIDTest() :
	m_numInputPrimitives( 0 ),
	m_numInstances( 1 )
{
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CGSPrimIDTest::Setup()
{
	TEST_RESULT tr = RESULT_PASS;
	
	//
	// Resize the output window
	//
	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Height = 16;
	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Width = 16;
	GetFramework()->m_RenderTargetOptions.FullscreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	GetFramework()->CleanupRenderTarget();
	GetFramework()->SetupRenderTarget();
	
	//
	// Setup pass through vertex shader
	//
	D3D11_INPUT_ELEMENT_DESC iedesc[] = 
	{
		{ "data", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	tr = SetupVSFromResource( "PrimID.sh", "VSPassThrough", iedesc, 1, &m_pVS, &m_pInputLayout );
	if( tr != RESULT_PASS )
		return tr;

	//
	// Setup pixel shader
	//		
	tr = SetupPSFromResource( "PrimID.sh", "PSMain", &m_pPS );
	if( tr != RESULT_PASS )
		return tr;
	
	tr = CommonSetup();
	
	return tr;
}


//////////////////////////////////////////////////////////////////////////

void CGSPrimIDTest::Cleanup()
{
	ID3D11DeviceContext *pDeviceContext = GetEffectiveContext();
	
	if ( pDeviceContext )
	{
		pDeviceContext->IASetInputLayout(NULL);
		pDeviceContext->VSSetShader(NULL, NULL, 0);
	}

	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVS );
	
	CommonCleanup();
}


//////////////////////////////////////////////////////////////////////////

void CGSPrimIDTest::InitTestParameters()
{
	CUserValueSet<UINT> *pNumInputPrimitivesValueSet = new CUserValueSet<UINT>();
	pNumInputPrimitivesValueSet->AddValue( 1 );
	pNumInputPrimitivesValueSet->AddValue( 9 );
	pNumInputPrimitivesValueSet->AddValue( 16 );
	
	CUserValueSet<UINT> *pNumInstancesValueSet = new CUserValueSet<UINT>();
	pNumInstancesValueSet->AddValue( 1 );
	pNumInstancesValueSet->AddValue( 3 );
	pNumInstancesValueSet->AddValue( 16 );
	
	CUserValueSet<GS_INPUT_PRIM_TYPE> *pInputPrimTypeValueSet = new CUserValueSet<GS_INPUT_PRIM_TYPE>();
	pInputPrimTypeValueSet->AddValue( PRIM_TYPE_POINT );
	pInputPrimTypeValueSet->AddValue( PRIM_TYPE_LINE );
	pInputPrimTypeValueSet->AddValue( PRIM_TYPE_TRIANGLE );
	
	CTestCaseParameter<UINT> *pNumInputPrimitivesParam = AddParameter( "NumInputPrimitives", &m_numInputPrimitives );
	CTestCaseParameter<UINT> *pNumInstancesParam = AddParameter( "NumInstances", &m_numInstances );
	CTestCaseParameter<GS_INPUT_PRIM_TYPE> *pInputPrimTypeParam = AddParameter( "InputPrimType", &m_inputPrimType );
	
	testfactor::RFactor numInputPrimitives = AddParameterValueSet( pNumInputPrimitivesParam, pNumInputPrimitivesValueSet );
	testfactor::RFactor numInstances = AddParameterValueSet( pNumInstancesParam, pNumInstancesValueSet );
	testfactor::RFactor inputPrimType = AddParameterValueSet( pInputPrimTypeParam, pInputPrimTypeValueSet );
	
	SetRootTestFactor( numInputPrimitives * numInstances * inputPrimType );

	AddPriorityPatternFilter(FilterNotEqual<UINT>(pNumInputPrimitivesParam, 16), 0.5f);
	AddPriorityPatternFilter(FilterNotEqual<UINT>(pNumInstancesParam, 16 ), 0.5f);
	AddPriorityPatternFilter(FilterEqual<UINT>(pNumInstancesParam, 1 ), 0.5f);
	AddPriorityPatternFilter(
		FilterNotEqual<GS_INPUT_PRIM_TYPE>(pInputPrimTypeParam, PRIM_TYPE_TRIANGLE) &&
		FilterNotEqual<GS_INPUT_PRIM_TYPE>(pInputPrimTypeParam, PRIM_TYPE_TRIANGLE_ADJ) &&
		FilterNotEqual<GS_INPUT_PRIM_TYPE>(pInputPrimTypeParam, PRIM_TYPE_POINT), 
		0.5f);

	DeclarePriorityLevelByPriorityValue( 1, 1.0f );
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CGSPrimIDTest::SetupTestCase()
{
	TEST_RESULT tr;

	//
	// Setup vertex input
	//
	D3D11_BUFFER_DESC bufDesc;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA srData;
	srData.pSysMem = NULL;
	srData.SysMemPitch = 0;
	srData.SysMemSlicePitch = 0;
	switch( m_inputPrimType )
	{
		case PRIM_TYPE_TRIANGLE:
			bufDesc.ByteWidth = m_numInputPrimitives * 3 * sizeof( VS_IN );
			srData.pSysMem = inputDataTriList;
			break;
			
		case PRIM_TYPE_LINE:
			bufDesc.ByteWidth = m_numInputPrimitives * 2 * sizeof( VS_IN );
			srData.pSysMem = inputDataLineList;
			break;
		
		case PRIM_TYPE_POINT:
			bufDesc.ByteWidth = m_numInputPrimitives * sizeof( VS_IN );
			srData.pSysMem = inputDataPointList;
			break;
	}
	
	GetDevice()->CreateBuffer( &bufDesc, &srData, &m_pVB );
	
	//
	// Setup geometry shader and input layout
	//
	
	LPCSTR gsEntry = NULL;
	
	switch( m_inputPrimType )
	{
		case PRIM_TYPE_POINT:
			gsEntry = "GSPrimIDPointIn";
			break;
			
		case PRIM_TYPE_LINE:
			gsEntry = "GSPrimIDLineIn";
			break;
			
		case PRIM_TYPE_TRIANGLE:
			gsEntry = "GSPrimIDTriangleIn";
			break;
			
		case PRIM_TYPE_LINE_ADJ:
			gsEntry = "GSPrimIDLineAdjIn";
			break;
			
		case PRIM_TYPE_TRIANGLE_ADJ:
			gsEntry = "GSPrimIDTriangleAdjIn";
			break;
	};
	
	if( gsEntry == NULL )
		return RESULT_SKIP;
	
	tr = SetupGSFromResource( "PrimID.sh", gsEntry, &m_pGS );
	if( tr != RESULT_PASS )
		return tr;

	
	return tr;
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT CGSPrimIDTest::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	
	//
	// Fetch the viewport dimensions
	//
	CONST_DATA *constData;
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	GetEffectiveContext()->Map( m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes );
	constData = (CONST_DATA*) mappedRes.pData;
	constData->vpSize[0] = 16;
	constData->vpSize[1] = 16;
	GetEffectiveContext()->Unmap(m_pCB,0);
	GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCB );
	
	//
	// Setup the rendering pipeline
	//
	GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
	GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
	GetEffectiveContext()->RSSetState( m_pRS );
	GetEffectiveContext()->OMSetDepthStencilState( m_pDSS, 0xffffffff );
	
	
	ID3D11Buffer *buffers[] = { m_pVB };
	UINT strides[] = { sizeof( VS_IN ) };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, buffers, strides, offsets );
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );

	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	UINT vertexCount = 0;
	
	switch( m_inputPrimType )
	{
		case PRIM_TYPE_POINT:
			topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			vertexCount = m_numInputPrimitives;
			break;
			
		case PRIM_TYPE_LINE:
			topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			vertexCount = 2 * m_numInputPrimitives;
			break;
			
		case PRIM_TYPE_TRIANGLE:
			topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			vertexCount = 3 * m_numInputPrimitives;
			break;
			
		case PRIM_TYPE_LINE_ADJ:
			break;
			
		case PRIM_TYPE_TRIANGLE_ADJ:
			break;
	};
	
	GetEffectiveContext()->IASetPrimitiveTopology( topology );
	
	float clearColor[4] = { 0, 0, 0, 1 };
	GetEffectiveContext()->ClearRenderTargetView( GetFramework()->GetRTView(), clearColor );
	
	switch( m_numInstances )
	{
		case 1:
			GetEffectiveContext()->Draw( vertexCount, 0 );
			break;
		default:
			GetEffectiveContext()->DrawInstanced( vertexCount, m_numInstances, 0, 0 );
			break;
	}
	
	// Copy the back buffer to the memory surface
	UINT failCount = 0;
	GetEffectiveContext()->CopyResource( m_pMemTexture, g_App.GetRTTexture() );

	// execute any deferred operations
	if ( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		tr = RESULT_FAIL;
		WriteToLog("ExecuteEffectiveContext failed. hr = %s", D3DHResultToString(hr).c_str() );
		goto testDone;
	}

	typedef unsigned char PIXEL[4];
	D3D11_MAPPED_SUBRESOURCE map;
	GetImmediateContext()->Map( m_pMemTexture, 0, D3D11_MAP_READ, 0, &map );
	for( UINT j = 0; j < m_numInstances; ++j )
	{
		PIXEL *pixels = (PIXEL *) ( (CHAR *) map.pData + map.RowPitch * j );
		
		for( UINT i = 0; i < m_numInputPrimitives; ++i )
		{
			if( pixels[i][1] < 127 )
			{
				WriteToLog( _T( "Primitive ID incorrect in GS for input primitive %i, instance %i" ), i, j );
				tr = RESULT_FAIL;
				failCount++;
			}
			
			if( pixels[i][2] < 127 )
			{
				WriteToLog( _T( "Primitive ID passed into PS by GS incorrect for primitive %i, instance %i.  Expected 7 + actual primitive id" ), i, j );
				tr = RESULT_FAIL;
				failCount++;
			}
		}
	}
	GetImmediateContext()->Unmap( m_pMemTexture, 0 );
	
	GetFramework()->Present( NULL, NULL, NULL, NULL, 0, 0 );

testDone:
	return tr;
}

//////////////////////////////////////////////////////////////////////////

void CGSPrimIDTest::CleanupTestCase()
{
	static ID3D11Buffer *vbclear[] = { 0 };
	static UINT vbstridesclear[] = { 0 };
	static UINT vboffsetsclear[] = { 0 };
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, vbclear, vbstridesclear, vboffsetsclear );
	SAFE_RELEASE( m_pVB );
	
	GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
	SAFE_RELEASE( m_pGS );
}


