#include "InputAssembler.h" 

//Need to test 128MB Index and Vertex buffers
//Try to create as many 128Mb buffers as possible
//Try to access SRV in shader with Load
//Access VB data in shader in 5 positions (begin, middle, end and between)

const UINT SOBufferSize = 100;
const UINT Size128MB = 128 * 1024 * 1024;
const UINT Size32MB = 32 * 1024 * 1024;

const char szInitInputStruct[] = 
	"struct In"	"\n"
	"{"				"\n"
	"    uint index : SV_VertexID;\n";

const char szFinishInputStruct[] = 
	"};"			"\n";

const char szOutputStruct[] = 
	"struct Out"	"\n"
	"{"				"\n"
	"    uint4 Result : Result;"			"\n"
	"};"				"\n";

const char szInitVSBody[] = 
	"Out main( In input )\n"
	"{\n"
	"	Out output; \n"
	"	output.Result = 0; \n"
	;

const char szFinishVSBody[] = 
	"	return output; \n"
	"}\n"
	;

const char szGS[] = 
	"struct Out"	"\n"
	"{"				"\n"
	"    uint4 Result : Result;"			"\n"
	"};"				"\n"
	"[maxvertexcount (1)]\n"
	"void main(inout PointStream<Out> Stream, point Out input[1] )\n"
	"{\n"
	"	if (input[0].Result.x == 0xffffffff) \n"
	"	    Stream.Append(input[0]); \n"
	"}\n"
	;

TEST_RESULT CLimitsTest::Setup()
{
	//Do NOT create the buffers in Setup
	m_bSetupBuffers = false;

	TEST_RESULT tr = CInputAssemblerTest::Setup();
	if (tr != RESULT_PASS)
		return tr;

	HRESULT hr;

	//
	//Create SO Buffer
	//
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = SOBufferSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pSOBuffer ) ) )
	{
		WriteToLog( "CreateBuffer(SO) failed" );
		return RESULT_FAIL;
	}

	// create an all-zero buffer, used to clear the SO target
	UINT SOInitZero[SOBufferSize];
	ZeroMemory(SOInitZero, sizeof(SOInitZero));
	D3D11_SUBRESOURCE_DATA SOInitDataZero;
	SOInitDataZero.SysMemPitch = 0;
	SOInitDataZero.SysMemSlicePitch = 0;
	SOInitDataZero.pSysMem = (void *)SOInitZero;
	if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &SOInitDataZero, &m_pSOBufferZero ) ) )
	{
		WriteToLog( "CreateBuffer(SOZero) failed" );
		return RESULT_FAIL;
	}

	bufferDesc.ByteWidth = SOBufferSize;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.BindFlags = 0;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pSOBufferCopy ) ) )
	{
		WriteToLog( "CreateBuffer(SOCopy) failed" );
		return RESULT_FAIL;
	}

	ID3D11Buffer *pBuffers[] = { m_pSOBuffer };
	UINT Offsets[] = { 0 };
	m_pDeviceContext->SOSetTargets( 1, pBuffers, Offsets );
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//
	//Default VB
	//
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = 10 * 4 * 2;

	D3D11_SUBRESOURCE_DATA subData;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	subData.pSysMem = (void *)m_pDefaultVBData;

	WORD *pData = (WORD*)m_pDefaultVBData;
	for (UINT i=0; i< 10 * 4; i++)
	{
		pData[i] = i + 11 + (i * 5);
	}

	if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &m_pDefaultVB ) ) )
	{
		WriteToLog( "CreateBuffer() Default VB failed" );
		return RESULT_FAIL;
	}

	//
	//Create a Buffer to hold all the data
	//
	m_pInitialData = new BYTE[Size128MB];

	return RESULT_PASS;
}

const UINT MaxBuffers = 0xffffffff;
const UINT BuffersToUse[9][3] =
{
//		IB		VB			SRV
	{	0,		1,			0	},
	{	1,		0,			0	},
	{	0,		0,			1	},
	{	1,		1,			0	},
	{	1,		0,			1	},
	{	0,		1,			1	},
	{	1,		1,			1	},
	//Next 2 cases uses 32mb buffers until we can figure how we can make it work on HW with 128mb
	//{	0,		MaxBuffers,	0	},
	//{	0,		0,			MaxBuffers	}
	//temporarily reduce the max memory cost for VB to 32mb * 8 = 256M
	{	0,		8,	0	},
	{	0,		0,			8	}
};
const UINT SizeBuffersToUse = ( sizeof(BuffersToUse) / (sizeof(UINT) * 3) );

void CLimitsTest::InitTestParameters()
{
	//Initialize root factors
	CInputAssemblerTest::InitTestParameters();

	//Create the parameters
	//CTestCaseParameter< D3D11_BIND_FLAGS > *pBindFlags = AddParameter( _T("BindFlags"), &m_BindFlags);
	//CTestCaseParameter< DXGI_FORMAT > *pFormat = AddParameter( _T("Format"), &m_Format);
	//CTestCaseParameter< UINT > *pNumBuffers = AddParameter( _T("NumBuffers"), &m_NumBuffers);
	CTestCaseParameter< UINT > *pNumIB = AddParameter( _T("NumIB"), &m_NumIB);
	CTestCaseParameter< UINT > *pNumVBs = AddParameter( _T("NumVBs"), &m_NumVBs);
	CTestCaseParameter< UINT > *pNumSRVBuffers = AddParameter( _T("NumSRVBuffers"), &m_NumSRVBuffers);


	//Create the value sets
	CTableValueSet< UINT > *pNumIBValues = new CTableValueSet< UINT >( &BuffersToUse[0][0], sizeof( BuffersToUse[0] ), sizeof( BuffersToUse ) / sizeof( BuffersToUse[0] ) );
	CTableValueSet< UINT > *pNumVBsValues = new CTableValueSet< UINT >( &BuffersToUse[0][1], sizeof( BuffersToUse[0] ), sizeof( BuffersToUse ) / sizeof( BuffersToUse[0] ) );
	CTableValueSet< UINT > *pNumSRVBuffersValues = new CTableValueSet< UINT >( &BuffersToUse[0][2], sizeof( BuffersToUse[0] ), sizeof( BuffersToUse ) / sizeof( BuffersToUse[0] ) );


	//Create the factors
	testfactor::RFactor rNumIB = AddParameterValueSet< UINT >( pNumIB, pNumIBValues );
	testfactor::RFactor rNumVBs = AddParameterValueSet< UINT >( pNumVBs, pNumVBsValues );
	testfactor::RFactor rNumSRVBuffers = AddParameterValueSet< UINT >( pNumSRVBuffers, pNumSRVBuffersValues );
	testfactor::RFactor rNumBuffers = rNumIB % rNumVBs % rNumSRVBuffers;

	SetRootTestFactor( rNumBuffers );

	// No need to do priority filtering here because this group is only run for priority level > 1
}

void CLimitsTest::Cleanup()
{
	CInputAssemblerTest::Cleanup();	

	SAFE_RELEASE( m_pDefaultVB );
	SAFE_DELETE_ARRAY( m_pInitialData );
}

tstring CLimitsTest::GetSRVCheckCode(UINT buffer, UINT index)
{
	tstring szVS;
	UINT data = (buffer + index + 17 + (index * 3)) % USHRT_MAX;
	szVS  = m_bFirstIf ? "    " : "    else ";
	szVS += "if (srv" + ToString(buffer) + ".Load(" + ToString(index) + ") != " + ToString(data) + ")\n";
	szVS += "    {\n";
	szVS += "        output.Result.x = 0xffffffff;\n";
	szVS += "        output.Result.y = " + ToString(buffer) + " + 0x8000;\n";
	szVS += "        output.Result.z = " + ToString(index) + ";\n";
	szVS += "        output.Result.w = srv" + ToString(buffer) + ".Load(" + ToString(index) + ");\n";
	//szVS += "        return output;\n";
	szVS += "    }\n";
	m_bFirstIf = false;

	return szVS;
}

TEST_RESULT CLimitsTest::ExecuteTestCase()
{

	TEST_RESULT tRes = RESULT_FAIL;
	HRESULT hr;
	UINT ZeroOffsets[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	ID3D11Buffer *pNullBuffers[32] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	ID3D11ShaderResourceView *pNullSRVs[32] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	const UINT strides[32] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
	UINT NumElements = 0;
	UINT Indices[5];
	tstring szVS;
	UINT NumCreatedIB = 0;
	UINT NumCreatedVBs = 0;
	UINT NumCreatedSRVs = 0;
	m_bFirstIf = true;

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA subData;

	ID3D11Buffer *pIB = NULL;
	ID3D11Buffer	*pVBs[MaxInputElements];
	ID3D11Buffer	*pSRVBuffers[MaxInputElements];
	ID3D11ShaderResourceView *pSRVs[MaxInputElements];

	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	for (UINT i=0; i< MaxInputElements; i++)
	{
		pVBs[i] = NULL;
		pSRVBuffers[i] = NULL;
		pSRVs[i] = NULL;
	}
	
	m_NumBuffers = 0;

	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;
	subData.pSysMem = (void *)m_pInitialData;

	//HW has problems binding big resources.  Until 1969663 is resolved, not sure what to do.
	UINT bufferSize = Size128MB / (m_NumIB + m_NumVBs + m_NumSRVBuffers);
	if (m_NumIB + m_NumVBs + m_NumSRVBuffers > 3)
		bufferSize = Size32MB;

	/////////////////
	//IB
	if (m_NumIB > 0)
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.ByteWidth = bufferSize;

		//Fill with data
		NumElements = bufferSize / /*Sizeof(Format)*/ GetFormatStrideInBytes( DXGI_FORMAT_R16_UINT );
		WORD *pInitialData = (WORD*)m_pInitialData;
		for (UINT iElement=0; iElement<NumElements; iElement++)
		{
			UINT value = m_NumVBs == 0 ? 10 : NumElements; //NumElements should be changed to number of elements in VBs if format can change
			pInitialData[iElement] = (iElement + 3) % value;
		}

		//We will check 5 values in the shader, store the indices
		Indices[0] = pInitialData[5];
		Indices[1] = pInitialData[NumElements / 4];
		Indices[2] = pInitialData[NumElements / 2];
		Indices[3] = pInitialData[NumElements - (NumElements / 4)];
		Indices[4] = pInitialData[NumElements - 5];

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &pIB ) ) )
		{
			WriteToLog( "CreateBuffer() src failed" );
			goto TESTFAIL;
		}

		m_NumBuffers++;
		NumCreatedIB++;

		m_pDeviceContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R16_UINT, 0);

		//Use default VB is no Huge VB
		if (m_NumVBs == 0)
		{
			m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pDefaultVB, strides, ZeroOffsets);
		}
	}
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}

	/////////////////
	//VBs

	UINT MaxNumVBs = 16;
	if (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		MaxNumVBs = 32;
	//The max number of inputs needs to account for the indexID.
	MaxNumVBs--;

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = bufferSize;

	NumElements = bufferSize / /*Sizeof(Format)*/ GetFormatStrideInBytes( DXGI_FORMAT_R16_UINT );
	WORD *pInitialData = (WORD*)m_pInitialData;
	for (UINT i=0; i<m_NumVBs && i<MaxNumVBs; i++)
	{
		//Fill with data
		for (UINT iElement=0; iElement<NumElements; iElement++)
		{
			pInitialData[iElement] = (i + iElement + 30 + (iElement * 2)) % USHRT_MAX;
		}

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &pVBs[i] ) ) )
		{
			break;
			//WriteToLog( "CreateBuffer() src failed" );
			//goto TESTFAIL;
		}

		m_NumBuffers++;
		NumCreatedVBs++;

		// When using feature_level_10_0 the maximum number of inputs is 16
		if( GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_0 && NumCreatedVBs == 15 )
			break;
	}
	if (NumCreatedVBs > 0)
	{
		//This is to make sure we don't unset the default vb if no huge vbs
		m_pDeviceContext->IASetVertexBuffers(0, m_MaxNumElements, pVBs, strides, ZeroOffsets);
	}
	
	/////////////////
	//SRV Buffers
	NumElements = bufferSize / /*Sizeof(Format)*/ GetFormatStrideInBytes( DXGI_FORMAT_R16_UINT );

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = bufferSize;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R16_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = NumElements; //sizeof(WORD);

	//When running all test cases, the compiler gets in a stack overflow on compilation with 20 srv buffers.
	//Until DX#107778 is fixed, limit it to 17
	for (UINT i=0; i<m_NumSRVBuffers && i<17; i++)
	{
		//Fill with data
		for (UINT iElement=0; iElement<NumElements; iElement++)
		{
			pInitialData[iElement] = (i + iElement + 17 + (iElement * 3)) % USHRT_MAX;
		}

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &pSRVBuffers[i] ) ) )
		{
			break;
			//WriteToLog( "CreateBuffer() src failed" );
			//goto TESTFAIL;
		}

		if ( FAILED( m_pDevice->CreateShaderResourceView( pSRVBuffers[i], &srvDesc, &pSRVs[i] ) ) )
		{
			break;
		}

		m_NumBuffers++;
		NumCreatedSRVs++;
	}
	m_pDeviceContext->VSSetShaderResources(0, 32, pSRVs);


	if (m_NumBuffers == 0)
	{
		tRes = RESULT_SKIP;
		goto TESTFAIL;
	}

	//////////////////
	//Vertex shader

	szVS = szInitInputStruct;
	if (m_NumIB > 0  && m_NumVBs == 0)
	{
		szVS += "    uint data0 : data0;\n";
	}
	else
	{
		for (UINT i=0; i<NumCreatedVBs; i++)
		{
			szVS += "    uint data" + ToString(i) + " : data" + ToString(i) + ";\n";
		}
	}
	szVS += szFinishInputStruct;
	szVS += szOutputStruct;
	for (UINT i=0; i<NumCreatedSRVs; i++)
	{
		szVS += "    Buffer<uint> srv" + ToString(i) + ";\n";
	}
	szVS += szInitVSBody;
	bool bFirstIf = true;
	if (m_NumIB > 0  && m_NumVBs == 0)
	{
			szVS += bFirstIf ? "    " : "    else ";
			szVS += "if ( input.data0 != (input.index + 11 + (input.index * 5)) )\n";
			szVS += "    {\n";
			szVS += "        output.Result.x = 0xffffffff;\n";
			szVS += "        output.Result.y = 0;\n";
			szVS += "        output.Result.z = input.index;\n";
			szVS += "        output.Result.w = input.data0;\n";
			//szVS += "        return output;\n";
			szVS += "    }\n";
			bFirstIf = false;
	}
	else
	{
		for (UINT i=0; i<NumCreatedVBs; i++)
		{
			szVS += bFirstIf ? "    " : "    else ";
			szVS += "if ( input.data" + ToString(i) + " != ((" + ToString(i) + " + (input.index) + 30 + ((input.index) * 2)) % 0xffff) )\n";
			szVS += "    {\n";
			szVS += "        output.Result.x = 0xffffffff;\n";
			szVS += "        output.Result.y = " + ToString(i) + ";\n";
			szVS += "        output.Result.z = input.index;\n";
			szVS += "        output.Result.w = input.data" + ToString(i) + ";\n";
			//szVS += "        return output;\n";
			szVS += "    }\n";
			bFirstIf = false;
		}
	}
	for (UINT i=0; i<NumCreatedSRVs; i++)
	{
		szVS += GetSRVCheckCode(i, 5);
		// szVS += GetSRVCheckCode(i, NumElements / 4);
		szVS += GetSRVCheckCode(i, NumElements / 2);
		// szVS += GetSRVCheckCode(i, NumElements - (NumElements / 4) );
		// szVS += GetSRVCheckCode(i, NumElements - 5);
	}
	szVS += szFinishVSBody;

	hr = g_App.CompileShaderFromMemory( szVS.c_str(), strlen( szVS.c_str() ), "main", D3D_SHADER_STAGE_VERTEX, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0, 0, &pShaderBuf, &pErrorBuf );
	if( FAILED( hr ) )
	{
		if ( pErrorBuf )
			WriteToLog( _T("CompileShaderFromMemory() failed. Error: %s"), pErrorBuf->GetBufferPointer() );
		else 
			WriteToLog( _T("CompileShaderFromMemory() failed. NULL error blob. "));
		goto TESTFAIL;
	}

	hr = m_pDevice->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pVS );
	if ( FAILED(hr) )
	{
		WriteToLog( "CreateVertexShader() failed" );
		goto TESTFAIL;
	}

	//////////////////
	// Input Layout

	ZeroMemory( m_InputElements, sizeof(m_InputElements) );

	UINT NumInputElements = 0;
	if (m_NumIB > 0  && m_NumVBs == 0)
	{
		NumInputElements = 1;

		m_InputElements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_InputElements[0].InputSlot = 0;
		m_InputElements[0].SemanticName = szSemantic;
		m_InputElements[0].SemanticIndex = 0;
		m_InputElements[0].Format = DXGI_FORMAT_R16_UINT;
	}
	else
	{
		NumInputElements = NumCreatedVBs;
		for (UINT i=0; i<NumCreatedVBs; i++)
		{
			m_InputElements[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			m_InputElements[i].InputSlot = i;
			m_InputElements[i].SemanticName = szSemantic;
			m_InputElements[i].SemanticIndex = i;
			m_InputElements[i].Format = DXGI_FORMAT_R16_UINT;
		}
	}
	hr = m_pDevice->CreateInputLayout(m_InputElements, NumInputElements, pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pInputLayout);
	if ( FAILED(hr) )
	{
		WriteToLog( "CreateInputLayout() failed" );
		goto TESTFAIL;
	}

	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( pErrorBuf );

	//////////////////
	// GS with SO

	// use a GS and do not emit anything if result passed.
	hr = g_App.CompileShaderFromMemory( szGS, strlen( szGS ), "main", D3D_SHADER_STAGE_GEOMETRY, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0, 0, &pShaderBuf, &pErrorBuf );
	if( FAILED( hr ) )
	{
		WriteToLog( "CompileShaderFromMemory() GS failed %s", pErrorBuf->GetBufferPointer() );
		goto TESTFAIL;
	}

	//Create a SO decl that matches
	D3D11_SO_DECLARATION_ENTRY soLayout[1];
	ZeroMemory(soLayout, sizeof(soLayout));
	soLayout[0].SemanticName = "Result";
	soLayout[0].ComponentCount = 4;

	UINT soStride = sizeof(UINT) * 4;

	hr = m_pDevice->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), soLayout, 1, &soStride, 1, 0, NULL, &m_pGS );
	if ( FAILED(hr) )
	{
		WriteToLog( "CreateGeometryShaderWithStreamOutput() failed" );
		goto TESTFAIL;
	}

	//////////////////
	// Set Pipeline
	m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
	m_pDeviceContext->GSSetShader( m_pGS, NULL, 0 );
	m_pDeviceContext->PSSetShader( NULL, NULL, 0 );

	m_pDeviceContext->IASetInputLayout( m_pInputLayout );

	// clear the SOtarget
	m_pDeviceContext->CopyResource(m_pSOBuffer, m_pSOBufferZero);
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}
	m_pDeviceContext->SOSetTargets(1, &m_pSOBuffer, ZeroOffsets);

	UINT DrawCount = m_NumIB + m_NumVBs == 0 ? 1 : NumElements;
	//////////////////
	// Draw
	if (m_NumIB > 0)
		m_pDeviceContext->DrawIndexed(DrawCount, 0, 0);
	else
		m_pDeviceContext->Draw(DrawCount, 0);

	/////////////////
	// Verify Results
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}

	UINT *pResult = NULL;

	//Clear buffer
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if (FAILED (hr = GetImmediateContext()->Map( m_pSOBufferCopy, 0, D3D11_MAP_WRITE, 0, &mappedRes )))
	{
		WriteToLog( "Map (WRITE) failed on SO Buffer copy with hr=%s", D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}
	ZeroMemory(mappedRes.pData, SOBufferSize);
	GetImmediateContext()->Unmap(m_pSOBufferCopy,0);

	m_pDeviceContext->CopyResource( m_pSOBufferCopy, m_pSOBuffer );
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}
	
	if (FAILED (hr = GetImmediateContext()->Map( m_pSOBufferCopy, 0, D3D11_MAP_READ, 0, &mappedRes )))
	{
		WriteToLog( "Map (READ) failed on SO Buffer copy with hr=%s", D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}

	pResult = (UINT*) mappedRes.pData;

	if (pResult[0] == 0xffffffff)
	{
		UINT buffer;
		if (pResult[1] & 0x8000)
		{
			buffer = pResult[1] - 0x8000;
			WriteToLog( "Failure on loading from ShaderResourceView." );
		}
		else
		{
			buffer = pResult[1];
		}
		WriteToLog( "Value is not what's expected (%d) on buffer %d at SV_VertexId= %d", pResult[3], buffer, pResult[2] );
		goto TESTFAIL;
	}

	tRes = RESULT_PASS;
TESTFAIL:

	GetImmediateContext()->Unmap(m_pSOBufferCopy,0);

	m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);
	m_pDeviceContext->IASetVertexBuffers(0, m_MaxNumElements, pNullBuffers, strides, ZeroOffsets);
	m_pDeviceContext->VSSetShaderResources(0, m_MaxNumElements, pNullSRVs);
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
	}

	SAFE_RELEASE( pIB );
	for (UINT i=0; i< MaxInputElements; i++)
	{
		SAFE_RELEASE( pVBs[i] );
		SAFE_RELEASE( pSRVs[i] );
		SAFE_RELEASE( pSRVBuffers[i] );
	}

	SAFE_RELEASE( pShaderBuf );
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pInputLayout );


	return tRes;
}
