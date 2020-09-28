#include "PixelShader.h" 

ID3D11PixelShaderApp  g_App;

const char szInitInputStruct[] = 
	"struct In"	"\n"
	"{"				"\n";

const char szInitOutputStruct[] = 
	"struct Out"	"\n"
	"{"				"\n";

const char szFinishInputOutputStruct[] = 
	"};"			"\n";

const char szInputPrimitiveId[] = 
	"uint primitiveID : SV_PrimitiveID;"		"\n";

const char szInputInstanceId[] = 
	"uint instanceID : SV_InstanceID;"	"\n";

const char szSemanticVertexId[] =
	"vertexID";

const char szSemanticInstanceId[] =
	"instanceID";

const char szInputOutputVSBody[] = 
	"InOut main( InOut input )\n"
	"{\n"
	"	return input; \n"
	"}\n"
	;

const char szVSBody[] = 
	"Out main( In input )\n"
	"{\n"
	"	return input; \n"
	"}\n"
	;

const char szVSPassthrough[] = 
	"struct In\n"
	"{\n"
	"	float4 pos : POS;\n"
	"	float4 dummy : DUMMY;\n"
	"};\n"
	"struct Out\n"
	"{\n"
	"	float4 pos : SV_Position;\n"
	"	float4 dummy : DUMMY;\n"
	"};\n"
	"Out main( In input )\n"
	"{\n"
	"	return input; \n"
	"}\n"
	;



BEGIN_NAMED_VALUES( DrawMethod )
	NAMED_VALUE( _T( "Draw" ), Draw )
	NAMED_VALUE( _T( "DrawInstanced" ), DrawInstanced )
	NAMED_VALUE( _T( "DrawIndexed" ), DrawIndexed )
	NAMED_VALUE( _T( "DrawIndexedInstanced" ), DrawIndexedInstanced )
	NAMED_VALUE( _T( "DrawAuto" ), DrawAuto )
END_NAMED_VALUES( DrawMethod )



ID3D11PixelShaderTest::ID3D11PixelShaderTest() : 
	m_pDevice(NULL), 
	m_pDeviceContext(NULL),
	m_pDebug(NULL), 
	m_pVertexBuffer(NULL), 
	m_pPassThroughVS(NULL),
	m_pDSState(NULL), 
	m_pInputLayout(NULL),
	m_pRTView(NULL),
	m_pRTTexture2D(NULL),
	m_pRTTexture2DCopy(NULL),
	m_pSRV(NULL)
{
}

bool ID3D11PixelShaderTest::CanPresent()
{ 
	return g_App.m_bPresent; 
}

TEST_RESULT ID3D11PixelShaderTest::Setup()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr;
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	m_pDevice = g_App.GetDevice();
	m_pDeviceContext = g_App.GetEffectiveContext();

	if ( g_App.m_D3DOptions.Debug )
	{
		m_pDebug = g_App.GetDebug();
		m_pInfoQueue = g_App.GetInfoQueue();
		//m_InfoQueueHelper.SetInfoQueue( m_pInfoQueue );
	}
	else
	{
		m_pDebug = NULL;
		m_pInfoQueue = NULL;
	}

	//
	// Disable DepthStencil
	//
	m_pDSState = NULL;

	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = FALSE;
	desc.StencilEnable = FALSE;

	if ( FAILED( m_pDevice->CreateDepthStencilState( &desc, &m_pDSState ) ) )
	{
		WriteToLog( "CreateDepthStencilState() failed" );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	m_pDeviceContext->OMSetDepthStencilState( m_pDSState, NULL );

	////
	//// Create input buffers
	////
	//float vertices[6][4];
	////CounterClockwise Triangle
	//vertices[0][0] = 0.0;
	//vertices[0][1] = 0.5;
	//vertices[0][2] = 0;
	//vertices[0][3] = 1;
	//vertices[1][0] = 0.5;
	//vertices[1][1] = 0.5;
	//vertices[1][2] = 0;
	//vertices[1][3] = 1;
	//vertices[2][0] = 0.5;
	//vertices[2][1] = 0;
	//vertices[2][2] = 0;
	//vertices[2][3] = 1;
	////Clockwise Triangle
	//vertices[3][0] = 0.0;
	//vertices[3][1] = 0.5;
	//vertices[3][2] = 0;
	//vertices[3][3] = 1;
	//vertices[4][0] = 0.5;
	//vertices[4][1] = 0;
	//vertices[4][2] = 0;
	//vertices[4][3] = 1;
	//vertices[5][0] = 0.5;
	//vertices[5][1] = 0.5;
	//vertices[5][2] = 0;
	//vertices[5][3] = 1;

	//D3D11_SUBRESOURCE_DATA subData;
	//subData.pSysMem = (void *)vertices;
	//subData.SysMemPitch = 0;
	//subData.SysMemSlicePitch = 0;

	//D3D11_BUFFER_DESC bufferDesc;
	//bufferDesc.ByteWidth = sizeof(float) * 4 * 6;
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bufferDesc.CPUAccessFlags = 0;
	//bufferDesc.MiscFlags = 0;


	//if ( FAILED( hr = m_pDevice->CreateBuffer( &bufferDesc, &subData, &m_pVertexBuffer ) ) )
	//{
	//	WriteToLog( "CreateBuffer() failed : %s", D3DHResultToString( hr ).c_str() );
	//	goto FAIL;
	//}

	//
	// Create passthrough VS
	//
	hr = D3DX10CompileFromMemory( szVSPassthrough, strlen( szVSPassthrough ), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "ID3D11PixelShaderTest::Setup() CompileShaderFromMemory() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	if( FAILED( GetDevice()->CreateVertexShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pPassThroughVS ) ) )
	{
		WriteToLog( _T( "ID3D11PixelShaderTest::Setup() failed creating PassThroughVS" ) );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	//
	//Create input layout
	//
	D3D11_INPUT_ELEMENT_DESC pIEDesc[] =
	{
		{ "POS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DUMMY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	if( FAILED( GetDevice()->CreateInputLayout( pIEDesc, 2, pShader->GetBufferPointer(), pShader->GetBufferSize(), &m_pInputLayout ) ) )
	{
		WriteToLog( _T( "ID3D11PixelShaderTest::Setup() - CreateInputLayout() failed" ) );
		tr = RESULT_FAIL;
		goto setupDone;
	}

	//
	// Setup the default viewport
	//
	D3D11_VIEWPORT vp;
	vp.Height = 64;
	vp.Width = 64;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	GetEffectiveContext()->RSSetViewports( 1, &vp );

	
	// Setup RenderTarget
	//
	D3D11_TEXTURE2D_DESC tex2ddesc;
	tex2ddesc.ArraySize = 1;
	tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex2ddesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	tex2ddesc.Height = 64;
	tex2ddesc.Width = 64;
	tex2ddesc.CPUAccessFlags = 0;
	tex2ddesc.MipLevels = 1;
	tex2ddesc.MiscFlags = 0;
	tex2ddesc.SampleDesc.Count = 1;
	tex2ddesc.SampleDesc.Quality = 0;
	tex2ddesc.Usage = D3D11_USAGE_DEFAULT;
	
	hr = GetFramework()->CreateTexture2DMapDefaultOptional( &tex2ddesc, NULL, &m_pRTTexture2D);
	if (!SUCCEEDED(hr))
	{
		WriteToLog( _T( "CPixelShaderTest::Setup: CreateTexture2D failed. hr = %s" ), D3DHResultToString(hr).c_str() );
		goto setupDone;
	}


	tex2ddesc.BindFlags = 0;
	tex2ddesc.Usage = D3D11_USAGE_STAGING;
	tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	tex2ddesc.MiscFlags = 0;
	
	hr = GetFramework()->CreateTexture2DMapDefaultOptional( &tex2ddesc, NULL, &m_pRTTexture2DCopy );
	if (!SUCCEEDED(hr))
	{
		WriteToLog( _T( "CPixelShaderTest::Setup: CreateTexture2D failed. hr = %s" ), D3DHResultToString(hr).c_str() );
		goto setupDone;
	}

	
	D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
	rtviewdesc.Texture2D.MipSlice = 0;
	rtviewdesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	GetDevice()->CreateRenderTargetView( m_pRTTexture2D, &rtviewdesc, &m_pRTView );
	GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTView, NULL );

	//
	//PresentHelper
	//
	if (CanPresent())
	{
		//
		//Create a Shader Resource View
		//
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		HRESULT hr = m_pDevice->CreateShaderResourceView(m_pRTTexture2D, &srvDesc, &m_pSRV);
		if (FAILED(hr))
		{
			WriteToLog( _T( "ID3D11PixelShaderTest::Setup() - CreateShaderResourceView failed" ) );
			tr = RESULT_FAIL;
			goto setupDone;
		}

        if (!m_PresentHelper.Setup(g_App.GetRTView(), g_App.GetSwapChain(), g_App.GetLogWrapper()))
		{
			WriteToLog( _T( "ID3D11PixelShaderTest::Setup() - m_PresentHelper.Setup() failed" ) );
			tr = RESULT_FAIL;
			goto setupDone;
		}
	}

setupDone:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );

	return tr;
}

void ID3D11PixelShaderTest::Cleanup()
{
	if (m_pDevice)
	{
		ID3D11Buffer *pBuffers[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		UINT Offsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		UINT Strides[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		m_pDeviceContext->OMSetDepthStencilState( NULL, NULL );

		m_pDeviceContext->IASetVertexBuffers(0, 16, pBuffers, Strides, Offsets);
		m_pDeviceContext->IASetInputLayout( NULL );
		m_pDeviceContext->VSSetShader( NULL, NULL, 0 );
	}

	SAFE_RELEASE( m_pDSState );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pPassThroughVS );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pRTTexture2D );
	SAFE_RELEASE( m_pRTTexture2DCopy );
	SAFE_RELEASE( m_pSRV );
}

void ID3D11PixelShaderTest::InitTestParameters()
{
}

void ID3D11PixelShaderTest::ShuffleElements(UINT numElements, UINT *pArray)
{
	//Set the seed to the test case number
	srand(GetFramework()->GetCurrentTestCase());

	for (UINT i = 0; i<numElements; i++)
	{
		pArray[i] = i;
	}

	UINT random;
	UINT temp;
	for (UINT last = numElements; last > 1; last--)
	{
		random = rand() % last;
		temp = pArray[random];
		pArray[random] = pArray[last - 1];
		pArray[last - 1] = temp;
	}
}


std::string ID3D11PixelShaderTest::GetShaderTypeFromFormat(DXGI_FORMAT format)
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(format);

	//Other formats not handled
	assert(pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT | FF_UINT | FF_SINT));

	std::string type;
	if (pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT))
		type = "float";
	else if (pFormatInfo->dwFlags & FF_UINT)
		type = "uint";
	else if (pFormatInfo->dwFlags & FF_SINT)
		type = "int";

	UINT numComponents = 0;
	for (; numComponents<4; numComponents++)
	{
		if (pFormatInfo->pBitsPerComponent[numComponents] == 0)
			break;
	}

	char vectorSize[2];
	vectorSize[0] = '0' + numComponents;
	vectorSize[1] = 0;
	type += vectorSize;

	return type;
}

UINT ID3D11PixelShaderTest::GetNumComponentsForFormat(DXGI_FORMAT format)
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(format);

	//Other formats not handled
	assert(pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT | FF_UINT | FF_SINT));

	UINT numComponents = 0;
	for (; numComponents<4; numComponents++)
	{
		if (pFormatInfo->pBitsPerComponent[numComponents] == 0)
			break;
	}

	return numComponents;
}


UINT ID3D11PixelShaderTest::GetFormatStrideInBytes(DXGI_FORMAT format)
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(format);

	//Other formats not handled
	assert(pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT | FF_UINT | FF_SINT));

	UINT bytes = pFormatInfo->uBitsPerElement / 8;

	//Return 1 byte for formats with less than 8 bit.
	return bytes == 0 ? 1 : bytes;
}

UINT  ID3D11PixelShaderTest::GetVerticesPerPrimitive(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	switch(topology)
	{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
			return 1;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
			return 2;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
			return 4;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			return 3;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			return 6;
			break;

		//2 lines per strip
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
			return 2;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
			return 4;
			break;

		//2 triangles per strip
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			return 3;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			return 6;
			break;
	}
	return 0;
}

UINT  ID3D11PixelShaderTest::GetPrimitiveCount(UINT numVertices, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	switch(topology)
	{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
			return numVertices;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
			return numVertices / 2;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
			return numVertices / 4;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			return numVertices / 3;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			return numVertices / 6;
			break;

		//2 lines per strip
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
			return numVertices / 2;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
			return numVertices / 2;
			break;

		//2 triangles per strip
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			return numVertices / 3;
			break;

		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			return numVertices / 3;
			break;
	}
	return 0;
}

TEST_RESULT ID3D11PixelShaderTest::SetupVSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_INPUT_ELEMENT_DESC *iedesc, UINT numInputElements, ID3D11VertexShader **ppVS, ID3D11InputLayout **ppInputLayout )
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	//
	// Compile shader source
	//
	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, NULL, NULL, entryProc, "vs_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}
	
	//if( pShader )
	//{
	//	ID3D10Blob *pDisasm;
	//	D3D10DisassembleShader( (CONST UINT *) pShader->GetBufferPointer(), false, NULL, &pDisasm );
	//	LPCTSTR szDisasm = (LPCTSTR) pDisasm->GetBufferPointer();
	//}
	
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto CLEANUP;
	}

	//
	// Create vertex shader object
	//
	hr = GetDevice()->CreateVertexShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, ppVS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateVertexShader() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto CLEANUP;
	}
	
	if( ppInputLayout && iedesc )
	{
		//
		// Create an input layout to correspond with this shader
		//
		hr = GetDevice()->CreateInputLayout( iedesc, numInputElements, pShader->GetBufferPointer(), pShader->GetBufferSize(), ppInputLayout );
		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CreateInputLayout() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
			tr = RESULT_FAIL;
			goto CLEANUP;
		}
	}

CLEANUP:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );
	
	return tr;
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT ID3D11PixelShaderTest::SetupGSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11GeometryShader **ppGS )
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	//
	// Compile shader source
	//
	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, NULL, NULL, entryProc, "gs_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto CLEANUP;
	}

	//
	// Create geometry shader object
	//
	hr = GetDevice()->CreateGeometryShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, ppGS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreateGeometryShader() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto CLEANUP;
	}

CLEANUP:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );
	
	return tr;
}

//////////////////////////////////////////////////////////////////////////

TEST_RESULT ID3D11PixelShaderTest::SetupPSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11PixelShader **ppPS )
{
	TEST_RESULT tr = RESULT_PASS;
	ID3D10Blob *pShader = NULL;
	ID3D10Blob *pError = NULL;

	HRESULT hr = D3DX10CompileFromResource( NULL, resourceID, NULL, NULL, NULL, entryProc, "ps_4_0", D3D10_SHADER_DEBUG, 0, NULL, &pShader, &pError, NULL );

	if( pError )
	{
		WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}

	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CompileShaderFromResource() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto CLEANUP;
	}

	hr = GetDevice()->CreatePixelShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, ppPS );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CreatePixelShader() unexpectedly failed - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		tr = RESULT_FAIL;
		goto CLEANUP;
	}

CLEANUP:
	SAFE_RELEASE( pError );
	SAFE_RELEASE( pShader );

	return tr;
}


//Generates x and y coordinates from -1.0 to 1.0.  Z coordinates are from -1.0 to 0.0 linearly increasing for each primitive
void ID3D11PixelShaderTest::GenerateData(UINT numVertices, D3D11_PRIMITIVE_TOPOLOGY topology, VERTEXPOS *pVertex)
{
	UINT numPrimitives = GetPrimitiveCount(numVertices, topology);
	float x = -0.95f;
	float y = -0.95f;
	float z = -1.0f;
	const float w = 1.0f;
	float xInc = 2.0f / 64;
	float yInc = 2.0f / 64;
	float zInc = 1.0f / numPrimitives;

	switch (topology)
	{
		case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			for (UINT i=0; i< numPrimitives; i++)
			{
				switch (topology)
				{
					case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
						pVertex[0].x = x;
						pVertex[0].y = y;
						pVertex[0].z = z;
						pVertex[0].w = w;
						pVertex += 1;
						break;

					case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
						pVertex[0].x = x;
						pVertex[0].y = y;
						pVertex[0].z = z;
						pVertex[0].w = w;
						pVertex[1].x = x + 0.1f;
						pVertex[1].y = y;
						pVertex[1].z = z;
						pVertex[1].w = w;
						pVertex += 2;
						break;

					case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
						pVertex[0].x = x - 0.1f;
						pVertex[0].y = y;
						pVertex[0].z = z;
						pVertex[0].w = w;
						pVertex[1].x = x;
						pVertex[1].y = y;
						pVertex[1].z = z;
						pVertex[1].w = w;
						pVertex[2].x = x + 0.1f;
						pVertex[2].y = y;
						pVertex[2].z = z;
						pVertex[2].w = w;
						pVertex[3].x = x + 0.2f;
						pVertex[3].y = y;
						pVertex[3].z = z;
						pVertex[3].w = w;
						pVertex += 4;
						break;

					case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
						pVertex[0].x = x - 0.1f;
						pVertex[0].y = y - 0.1f;
						pVertex[0].z = z;
						pVertex[0].w = w;
						pVertex[1].x = x;
						pVertex[1].y = y + 0.1f;
						pVertex[1].z = z;
						pVertex[1].w = w;
						pVertex[2].x = x + 0.1f;
						pVertex[2].y = y - 0.1f;
						pVertex[2].z = z;
						pVertex[2].w = w;
						pVertex += 3;
						break;

					case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
						pVertex[0].x = x - 0.1f;
						pVertex[0].y = y - 0.1f;
						pVertex[0].z = z;
						pVertex[0].w = w;
						pVertex[1].x = x - 0.2f;
						pVertex[1].y = y + 0.1f;
						pVertex[1].z = z;
						pVertex[1].w = w;
						pVertex[2].x = x;
						pVertex[2].y = y + 0.1f;
						pVertex[2].z = z;
						pVertex[2].w = w;
						pVertex[3].x = x + 0.2f;
						pVertex[3].y = y + 0.1f;
						pVertex[3].z = z;
						pVertex[3].w = w;
						pVertex[4].x = x + 0.1f;
						pVertex[4].y = y - 0.1f;
						pVertex[4].z = z;
						pVertex[4].w = w;
						pVertex[5].x = x;
						pVertex[5].y = y - 0.1f;
						pVertex[5].z = z;
						pVertex[5].w = w;
						pVertex += 6;
						break;				}
				x += xInc;
				if (x >= 0.95f)
				{
					x = -0.95f;
					y += yInc;
					if (y >= 0.95f)
						y = -0.95f;
				}
				z += zInc;
			}
			break;
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			zInc = 1.0f / numVertices;
			float yOffset = -0.1f;
			float yAdjOffset = 0.1f;
			for (UINT i=0; i< numVertices; i++)
			{
				pVertex[0].x = x;
				pVertex[0].y = y;
				pVertex[0].z = z;
				pVertex[0].w = w;
				pVertex += 1;

				z += zInc;

				switch (topology)
				{
					case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
					case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
						x += xInc;
						if (x >= 0.95f || x <= -0.95f)
						{
							xInc = -xInc;
							y += yInc;
							if (y >= 0.95f || y <= -0.95f)
							{
								yInc = -yInc;
							}
						}
					break;

					case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
						x += xInc;
						yOffset = -yOffset;
						y += yOffset;
						if (x >= 0.95f || x <= -0.95f)
						{
							xInc = -xInc;
							y += yInc;
							if (y >= 0.95f || y <= -0.95f)
							{
								yInc = -yInc;
							}
						}
						break;

					case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
						if ((i % 2) != 0)
						{
							x += xInc;
							yOffset = -yOffset;
							y += yOffset;
						}
						else
						{
							y += 2 * yOffset;
						}
						if (x >= 0.95f || x <= -0.95f)
						{
							xInc = -xInc;
							y += yInc;
							if (y >= 0.95f || y <= -0.95f)
							{
								yInc = -yInc;
							}
						}
						break;
				}
			}
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void ID3D11PixelShaderApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();

    if (!GetShell()->IsConsole())
    {
	    int n = AddOptionCategory( _T( "Debug" ) );
	    RegisterOptionVariable( _T( "Present" ), _T( "Causes the tests to actually Present the render target." ), &m_bPresent, false, 0, n );
    }

	// TODO: REF only is a temporary measure while no HAL capable hardware available
	//UnRegisterOptionVariable( _T( "SrcOnly" ) );
	m_D3DOptions.SrcOnly = true;
}

bool ID3D11PixelShaderApp::InitTestGroups()
{
	// Register test classes with the group manager.
	if( !ADD_TEST( _T( "IsFrontFace" ), CIsFrontFaceTest ) ) return false;
	if( !ADD_TEST( _T( "GeneratedValues" ), CGeneratedValuesTest ) ) return false;

	// This test group should not run as Pri 1 test cases, because of HW contingencies. Also
	// because PipelineStats won't produce any visual artifacts for end user, failures in this
	// test group should not block RI.
	if( GetExecutionPriorityLevel() > 1 || GetExecutionPriorityLevel() == -1)
	{
		if( !ADD_TEST( _T( "PipelineStats" ), CPipelineStatsTest ) ) return false;
	}

	if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		if( !ADD_TEST( _T( "UAVWrite" ), CPixelShaderUAVWrite ) ) return false;
	}

	return true;
}
