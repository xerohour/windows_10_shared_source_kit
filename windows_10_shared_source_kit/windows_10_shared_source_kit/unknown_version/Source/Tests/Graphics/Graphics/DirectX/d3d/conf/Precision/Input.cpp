#include "Input.h"
#include "strsafe.h"


//---------------------------------------------------------------

BEGIN_NAMED_VALUES( CPSInputPrecisionTest::ERegisterType )
	NAMED_VALUE( _T( "POSITION" ), CPSInputPrecisionTest::ERT_POSITION )
	NAMED_VALUE( _T( "TEXCOORD" ), CPSInputPrecisionTest::ERT_TEXCOORD )
	NAMED_VALUE( _T( "INT" ), CPSInputPrecisionTest::ERT_INT )
END_NAMED_VALUES( CPSInputPrecisionTest::ERegisterType )

const DXGI_FORMAT g_rtFormatFloatAllFLs = DXGI_FORMAT_R16G16B16A16_FLOAT;
const DXGI_FORMAT g_rtFormatInt = DXGI_FORMAT_R32_SINT;

// interesting integer values
const int c_ipos = 2 << 18;
const int c_ineg = -c_ipos;
const int c_inegClamped = -(int)(2 < 15);

// Each texture component has a different range that affects each precision level differently:
//  x: -1 to 1 (representable in 10,16,32 bits)
//  y: -2.5 to 2.5 (with -2 to 2 representable in 10 bit, the rest clamping)
//  z:  -0.5 to 1.5 (with 0 to 1 representable in 8 bit and the rest clamping)
//  w:  0 to 1 (all representable)
CPSInputPrecisionTest::Vertex10 VBData[4] = { {-1, 1, -1,-2.5,-0.5,0, c_ineg}, 
                                              { 1, 1,  1, 2.5, 1.5,1, c_ineg}, 
                                              {-1,-1, -1,-2.5,-0.5,0, c_ipos}, 
                                              { 1,-1,  1, 2.5, 1.5,1, c_ipos} };

//---------------------------------------------------------------

/******************************
* CPSInputPrecisionTest methods
* Tests the min-precision flags on pixel shader inputs (and vertex shader outputs)
******************************/

bool CPSInputPrecisionTest::CreateResources()
{
	HRESULT hr;

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	// Create RTV
	{
		D3D11_TEXTURE2D_DESC tex2DDesc;
		tex2DDesc.ArraySize			= 1;
		tex2DDesc.BindFlags			= D3D11_BIND_RENDER_TARGET;
		tex2DDesc.CPUAccessFlags	= 0;
		tex2DDesc.Format			= (m_RegisterType == ERT_INT) ? g_rtFormatInt : g_rtFormatFloatAllFLs;
		tex2DDesc.Width				= m_RTWidth;
		tex2DDesc.Height			= m_RTHeight;
		tex2DDesc.MipLevels			= 1;
		tex2DDesc.MiscFlags			= 0;
		tex2DDesc.SampleDesc.Count	= 1;
		tex2DDesc.SampleDesc.Quality= 0;
		tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;

		assert( m_pRT2D == NULL );
		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRT2D ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed for render target.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}

		tex2DDesc.BindFlags = 0;
		tex2DDesc.Usage = D3D11_USAGE_STAGING;
		tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		assert( m_pRT2DStaging == NULL );
		if( FAILED( hr = GetDevice()->CreateTexture2D( &tex2DDesc, NULL, &m_pRT2DStaging ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed for render target staging copy.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = tex2DDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		assert( m_pRTV == NULL );
		if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRT2D, &rtvDesc, &m_pRTV ) ) )
		{
			// some feature levels or cards may not support RTVs in this format
			LogError( __FILEW__, __LINE__, L"CreateRenderTargetView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}

	}

	// Pass-through VS. Pass through PS (passes through one regieter, depending on m_RegisterType)
	static const char szShaderTemplate[] = 
		"struct VSIn\n"
		"{\n"
		"	%sfloat2 pos : pos;\n"
		"	%sfloat4 tex : texcoord0;\n"
		"	%s%s data : texcoord1;\n"
		"}; \n"
		"struct VSOut\n"
		"{\n"
		"	%sfloat4 pos : SV_Position;\n"
		"	%sfloat4 tex : texcoord0;\n"
		"	%s%s data : texcoord1;\n"
		"};\n"
		"VSOut VSMain(VSIn input)\n"
		"{\n"
		"	VSOut output;\n"
		"	output.pos.xy = input.pos;\n"
		"	output.pos.z = 0;\n"
		"	output.pos.w = 1;\n"
		"	output.tex = input.tex;\n"
		"	output.data = input.data;\n"
		"	return output;\n"
		"}\n"
		"%s4 PSMain (VSOut input) : SV_Target\n"
		"{\n"
		"	%s\n"
		"}\n";

	char* pRegisterCopy = NULL;
	char* pOutputType = NULL;
	switch( m_RegisterType )
	{
	case ERT_POSITION:
		// 9.x pixel shaders cannot read from POSITION
		assert( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 );
		pRegisterCopy = "return input.pos;";
		pOutputType = "float";
		break;
	case ERT_TEXCOORD:
		pRegisterCopy = "return input.tex;";
		pOutputType = "float";
		break;
	case ERT_INT:
		// 9.x shaders do not support INT
		assert( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 );
		pRegisterCopy = "return int4(input.data,0,0,0);";
		pOutputType = "int";
		break;
	}

	char* pMinPrecDecl = MinPrecisionPrefix( m_MinPrecision );
	char* pMinPrecDeclPos = m_RegisterType == ERT_POSITION ? pMinPrecDecl : "";

	// Form the actual shader code.
	static const UINT shaderBufferSize = sizeof(szShaderTemplate) + 100;
	char szCurShader[shaderBufferSize];
	int MadeShader = sprintf( szCurShader, szShaderTemplate, pMinPrecDeclPos, pMinPrecDecl, pMinPrecDecl, pOutputType, pMinPrecDeclPos, 
		pMinPrecDecl, pMinPrecDecl, pOutputType, pOutputType, pRegisterCopy );
	if( MadeShader < 0 )
	{
		LogError( __FILEW__, __LINE__, L"sprintf failed.  Perhaps the buffer size is too small.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		return false;
	}

	UINT shaderSize = strlen(szCurShader);

	// Outputing position is not available in 9.x pixel shader.  INTs are also not available in 9.x
	D3D_FEATURE_LEVEL CompileFL = (m_RegisterType == ERT_TEXCOORD) ? D3D_FEATURE_LEVEL_9_1 : D3D_FEATURE_LEVEL_10_0;

	// The D3D9 validation has not been updated to handle min precision
	UINT Flags = (m_RegisterType == ERT_INT) ? 0 : D3D10_SHADER_SKIP_VALIDATION;

	// Create VS, IL
	{
		assert( m_pIL == NULL && m_pVS == NULL );
		ATL::CComPtr<ID3D10Blob> pShaderBlob;
		ATL::CComPtr<ID3D10Blob> pErrorBuf;
		hr = g_TestApp.CompileShaderFromMemory( szCurShader, shaderSize, "VSMain", D3D_SHADER_STAGE_VERTEX, CompileFL, CompileFL, Flags, &pShaderBlob, &pErrorBuf );
		if( FAILED( hr ) )
		{
			if( pErrorBuf ) WriteToLog( "CompileShaderFromMemory() failed %s", pErrorBuf->GetBufferPointer() );
			WriteToLog( "Entrypoint: %s    FeatureLevel: %s", "VSMain", ToString( CompileFL ).c_str() );
			WriteToLog( szCurShader );
			LogError( __FILEW__, __LINE__, L"CompileShaderFromMemory() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceCreateShader );
			return false;
		}

		if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}

		const UINT NumVertexElements = 3;
		D3D11_INPUT_ELEMENT_DESC IADesc[NumVertexElements];
		IADesc[0].InputSlot = 0;
		IADesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		IADesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
		IADesc[0].SemanticName = "pos";
		IADesc[0].SemanticIndex = 0;
		IADesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		IADesc[0].InstanceDataStepRate = 0;

		IADesc[1].InputSlot = 0;
		IADesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		IADesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		IADesc[1].SemanticName = "texcoord";
		IADesc[1].SemanticIndex = 0;
		IADesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		IADesc[1].InstanceDataStepRate = 0;

		IADesc[2].InputSlot = 0;
		IADesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		IADesc[2].Format = (m_RegisterType == ERT_INT) ? DXGI_FORMAT_R32_SINT : DXGI_FORMAT_R32_FLOAT;
		IADesc[2].SemanticName = "texcoord";
		IADesc[2].SemanticIndex = 1;
		IADesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		IADesc[2].InstanceDataStepRate = 0;

		if( FAILED( hr = GetDevice()->CreateInputLayout( IADesc, NumVertexElements, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pIL ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateInputLayout() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateInputLayout );
			return false;
		}
	}

	// Create PS
	{
		assert( m_pPS == NULL );

		ATL::CComPtr<ID3D10Blob> pShaderBlob;
		ATL::CComPtr<ID3D10Blob> pErrorBuf;
		hr = g_TestApp.CompileShaderFromMemory( szCurShader, shaderSize, "PSMain", D3D_SHADER_STAGE_PIXEL, CompileFL, CompileFL, Flags, &pShaderBlob, &pErrorBuf );
		if( FAILED( hr ) )
		{
			if( pErrorBuf ) WriteToLog( "CompileShaderFromMemory() failed %s", pErrorBuf->GetBufferPointer() );
			WriteToLog( "Entrypoint: %s    FeatureLevel: %s", "PSMain", ToString( CompileFL ).c_str() );
			WriteToLog( szCurShader );
			LogError( __FILEW__, __LINE__, L"CompileShaderFromMemory() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceCreateShader );
			return false;
		}

		if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreatePixelShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}
	}

	// Create VB
	{
		float dleft = 0;
		float dright = 1;
		float dtop = 0;
		float dbottom = 1;
		int ipos = 2 << 11;
		int ineg = -ipos;

		// We're filling the VB with data used for both the 9 and 10 case, so make sure the vertex sizes are the same.
		C_ASSERT( sizeof(Vertex10) == sizeof(Vertex9) );

		D3D11_SUBRESOURCE_DATA bufInit = { VBData, sizeof(VBData), 0 };
		D3D11_BUFFER_DESC bufDesc = { sizeof(VBData), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
		assert( m_pVB == NULL );
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufDesc, &bufInit, &m_pVB ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateBuffer() failed for VB.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}
	}

	// Create RS
	if( m_pRS == NULL )
	{
		D3D11_RASTERIZER_DESC rastDesc = { D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE, 0, 0, 0, TRUE, FALSE, FALSE, FALSE };
		if( FAILED( hr = GetDevice()->CreateRasterizerState( &rastDesc, &m_pRS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateRasterizerState() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateRasterizerState );
			return false;
		}
	}

	return true;
}

TEST_RESULT CPSInputPrecisionTest::SetupTestCase()
{
	HRESULT hr;

	// The runtime will strip min precision from the shader if the driver doesn't support it.

	if( GetDevice()->GetFeatureLevel() <= D3D_FEATURE_LEVEL_9_1 )
	{
		// This test assumes feature level 9.2 because it uses DXGI_FORMAT_R16G16B16A16_FLOAT, which is required for 9.2+
		// Otherwise, the test would have to use DXGI_FORMAT_R8G8B8A8_UNORM and encode all up-to-16-bit results into this.
		CFilterHandle skipAll = FilterActive<D3D_MIN_PRECISION>( _T("MinPrecision")  );
		WriteToLog(_T("Min precision is not supported on feature level 9.1.") );
		SkipMatching( GetSkipMatchingFilter() || skipAll );
	}
	else if( GetDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_10_0  )
	{
		if( m_RegisterType == ERT_INT )
		{
			CFilterHandle skipRegister = FilterEqual( _T("Register"), ERT_INT );
			WriteToLog(_T("INT registers are not supported at 9.x feature levels.") );
			SkipMatching( GetSkipMatchingFilter() || skipRegister );
			return RESULT_SKIP;
		}
		else if( m_RegisterType == ERT_POSITION )
		{
			CFilterHandle skipRegister = FilterEqual( _T("Register"), ERT_POSITION );
			WriteToLog(_T("Reading from POSITION is not supported at 9.x feature levels.") );
			SkipMatching( GetSkipMatchingFilter() || skipRegister );
			return RESULT_SKIP;
		}
	}

	if( !CreateResources() )
	{
		// Error logged in CreateResources
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void CPSInputPrecisionTest::CleanupTestCase()
{
	// Objects are automatically released when CComPtr is set to NULL
	m_pVB = NULL;
	m_pIL = NULL;
	m_pVS = NULL;
	m_pPS = NULL;
	m_pRTV = NULL;
	m_pRT2D = NULL;
	m_pRT2DStaging = NULL;
}

void CPSInputPrecisionTest::InitTestParameters()
{
	D3D_FEATURE_LEVEL FeatureLevel = GetFramework()->m_D3DOptions.FeatureLevel;

	// Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pMinPrecisionParam = AddParameter( _T( "MinPrecision" ), &m_MinPrecision );
	CUserValueSet<D3D_MIN_PRECISION> *pPrecisionValues = new CUserValueSet<D3D_MIN_PRECISION>( );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_DEFAULT );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_16 );
	testfactor::RFactor rMinPrecisionInt = AddParameterValueSet( pMinPrecisionParam, pPrecisionValues );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_10 );
	testfactor::RFactor rMinPrecisionFloat = AddParameterValueSet( pMinPrecisionParam, pPrecisionValues );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	// Float/Int/Position register
	CTestCaseParameter<ERegisterType> *pRegisterParam = AddParameter( _T( "Register" ), &m_RegisterType );
	CUserValueSet<ERegisterType> *pRegisterValues = new CUserValueSet<ERegisterType>( );
	pRegisterValues->AddValue( ERT_TEXCOORD );
	testfactor::RFactor rRegisterTexCoord = AddParameterValueSet( pRegisterParam, pRegisterValues );
	pRegisterValues->AddValue( ERT_POSITION );
	testfactor::RFactor rRegisterFloat = AddParameterValueSet( pRegisterParam, pRegisterValues );
	testfactor::RFactor rRegisterInt = AddParameterValue( pRegisterParam, ERT_INT );
	SetParameterDefaultValue<ERegisterType>( pRegisterParam, ERT_POSITION );

	if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		SetRootTestFactor( rMinPrecisionFloat * rRegisterTexCoord );
	}
	else
	{
		SetRootTestFactor( rMinPrecisionFloat * rRegisterFloat + rMinPrecisionInt * rRegisterInt );
	}
	
	// Set priority values
	AddPriorityPatternFilter( FilterNotEqual<D3D_MIN_PRECISION>(pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT), WeightFactorVeryLarge );

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

TEST_RESULT CPSInputPrecisionTest::ExecuteTestCase()
{
	HRESULT hr;

	{
		UINT Stride = sizeof(Vertex10);
		UINT Offset = 0;

		// Note that the position has min precision, which means it can be clamped.  In the case of 8-bit precision,
		// it can be clamped to [0,1] which means that only the top-right quadrant of the render target will be covered
		D3D11_VIEWPORT vp = { 0,0, m_RTWidth,m_RTHeight, 0, 1 };

		assert( m_pIL != NULL );
		assert( m_pVB != NULL );
		assert( m_pVS != NULL );
		assert( m_pPS != NULL );
		assert( m_pRTV != NULL );
		assert( m_pRS != NULL );

		FLOAT clearColor[4] = {0};
		GetEffectiveContext()->ClearRenderTargetView( m_pRTV, clearColor );

		GetEffectiveContext()->IASetInputLayout( m_pIL );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVB, &Stride, &Offset );
		GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
		GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, NULL );
		GetEffectiveContext()->RSSetState( m_pRS );
		GetEffectiveContext()->RSSetViewports( 1, &vp );

		GetEffectiveContext()->Draw( 4, 0 );

		// reset the context
		ID3D11RenderTargetView* ppNULLRTV[] = {NULL};
		ID3D11Buffer* ppNULLBuff[] = {NULL};

		GetEffectiveContext()->IASetInputLayout( NULL );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, ppNULLBuff, &Stride, &Offset );
		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->RSSetState( NULL );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceContextExecute );
			return RESULT_FAIL;
		}
	}

	if( RESULT_PASS != ValidateResult( ) )
	{
		// Error logged in ValidateResult
		return RESULT_FAIL;
	}


	return RESULT_PASS;
}

TEST_RESULT CPSInputPrecisionTest::ValidateResult()
{
	D3D11_MAPPED_SUBRESOURCE mapTex2D;
	HRESULT hr = S_OK;

	GetImmediateContext()->CopyResource( m_pRT2DStaging, m_pRT2D );

	CMapHelper mapHelper( GetImmediateContext() );
	hr = mapHelper.Map( m_pRT2DStaging, 0, D3D11_MAP_READ, 0, &mapTex2D );
	if( FAILED( hr ) )
	{
		LogError( __FILEW__, __LINE__, L"m_pRT2DStaging Map() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
		return RESULT_FAIL;
	}

	if( m_RegisterType == ERT_INT )
	{
		// The integer expected for each pixel is equal to the integer value associated with the first vertex of the triangle
		// (there is no integer interpolation).  Since there is no actual integer math, the value should not have changed, so 
		// we'll make the epsilon very small.
		const UINT IntEpsilon = 1;

		for( UINT y = 0; y < m_RTHeight; y++ )
		{
			for( UINT x = 0; x < m_RTWidth; x++ )
			{
				// integer values are not interpalated
				int pixel = ((int*)((BYTE*)mapTex2D.pData + mapTex2D.RowPitch * y))[x];
				int expected = c_ineg;
				int expectedClamped = c_inegClamped;

				if( abs( pixel - expected ) > IntEpsilon && abs( pixel - expectedClamped ) > IntEpsilon )
				{
					WriteToLog( _T( "CPrecisionTest::ValidateResult() - INT comparison failed at position (%d,%d).  Expected %d or %d, found %d." ), x, y, expected, expectedClamped, pixel );
					LogError( __FILEW__, __LINE__, L"INT comparison failed.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
			}
		}
	}
	else
	{
		// Only the x-coordinate of position is interesting (because the RT height is only 2).  Texcoord has 4
		UINT NumComponents = ( m_RegisterType == ERT_POSITION ) ? 1 : 4;
		for( UINT y = 0; y < m_RTHeight; y++ )
		{
			for( UINT c = 0; c < NumComponents; c++ )
			{
				UINT numValues = 0;
				float lastValue = -10;
				for( UINT x = 0; x < m_RTWidth; x++ )
				{
					FLOAT_COLOR fPixel;
					bool bConverted = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
									   DXGI_FORMAT_R16G16B16A16_FLOAT, 
									   &fPixel, 
									   1, 
									   1, 
									   1, 
									   ((BYTE*)mapTex2D.pData + mapTex2D.RowPitch * y) + 8 * x );

					if ( !bConverted )
					{
						LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
						return RESULT_FAIL;
					}

					// Select the component to read
					float pixel = fPixel.comp[c];


					// The expected value is not used to determine pass/fail, but it can be looked at manually to see if the HW is 
					// doing something strange when the test fails.
					// Note that the position has min precision, which means it can be clamped.  In the case of 8-bit precision,
					// it can be clamped to [0,1] which means that only the top-right quadrant of the render target will be covered.
					// The expected value calculated below does not take this (or any other clamping) into account because we don't 
					// know if the HW will clamp.
					float rRight = (x + 0.5f) / m_RTWidth;
					float rLeft = 1 - rRight;
					float expected = 0;
					if( m_RegisterType == ERT_POSITION )
					{
						expected = rLeft * 0 + rRight * m_RTWidth;
					}
					else if( m_RegisterType == ERT_TEXCOORD )
					{
						expected = rLeft * VBData[0].texcoord[c] + rRight * VBData[1].texcoord[c];
					}


					// Values must be in increasing order
					if( pixel < lastValue )
					{
						WriteToLog( _T( "CPrecisionTest::ValidateResult() - monotonic increase failed at position (%d,%d).  Previous %f, current %f." ), x, y, lastValue, pixel );
						LogError( __FILEW__, __LINE__, L"monotonic increase failed.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
						return RESULT_FAIL;
					}

					if( lastValue != pixel )
					{
						lastValue = pixel;
						numValues++;
					}
				}

				C_ASSERT( m_RTWidth >= 2048 ); // The values below assume there are at least 2048 pixels rendered
				C_ASSERT( m_RTHeight == 2 );   // "y == 1" is used to determine the bottom half of the render target

				// POSITION is handled differently because the hardware converts from normalized to render target coordinates
				float left = m_RegisterType == ERT_POSITION ? 0 : VBData[0].texcoord[c];
				float right = m_RegisterType == ERT_POSITION ? m_RTWidth :VBData[1].texcoord[c];
				float clampedLeft = left;
				float clampedRight = right;
				UINT MaxExpected = m_RTWidth;

				// We can't rely on exact float32 interporlation since the center of the pixel is only known to 4 bits of precision.
				// etc.  So we allow a little room (5%) of error.  
				float WiggleRoom = 0.05f; 

				switch( m_MinPrecision )
				{
				case D3D_MIN_PRECISION_ANY_10:
					// (-2,2) represntable
					clampedLeft = max( -1.99f, clampedLeft );  // 10-bit precision doesn't necessarily include -2 or +2, just very close.
					clampedRight = min( 1.99f, clampedRight );
					MaxExpected = (UINT)( ( clampedRight - clampedLeft ) / 3.98f * 1024 );  // 1024 = 2 ^ 10, 3.98 is the range (-1.99,1.99)
					break;
				case D3D_MIN_PRECISION_ANY_16:
					MaxExpected = 2048;  // 16-bit float has 10 bits of precision, however each of the ranges
					                     // include at least two exponent ranges
					break;
				case D3D_MIN_PRECISION_DEFAULT:
					WiggleRoom = 0.0f;
					break;
				}

				UINT NumPixelsInRange = min( m_RTWidth, (UINT)( m_RTWidth * ( clampedRight - clampedLeft ) / ( right - left ) ) + 1 );
				UINT MaxExpectedWithWiggle = (UINT)( MaxExpected * ( 1.0f - WiggleRoom ) );
				UINT MinNumValues = min( MaxExpectedWithWiggle, NumPixelsInRange );

				if( numValues < MinNumValues )
				{
					WriteToLog( _T( "CPrecisionTest::ValidateResult() - Error: For regiter %s[%d] row %d, expected at least %d unique values, found %d." ), ToString(m_RegisterType).c_str(), c, y, MinNumValues, numValues );
					LogError( __FILEW__, __LINE__, L"Number of unique values failed.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
			}
		}
	}

	// m_pRT2DStaging unmapped automatically.

	return RESULT_PASS;
}



/*************************************************************
* CVSInputPrecisionTest methods                              *
**************************************************************/

void CVSInputPrecisionTest::InitTestParameters()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetFramework()->m_D3DOptions.FeatureLevel;

	// Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pMinPrecisionParam = AddParameter( _T( "MinPrecision" ), &m_MinPrecision );
	CUserValueSet<D3D_MIN_PRECISION> *pPrecisionValues = new CUserValueSet<D3D_MIN_PRECISION>( );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_DEFAULT );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_16 );
	testfactor::RFactor rMinPrecision16Plus = AddParameterValueSet( pMinPrecisionParam, pPrecisionValues );
	testfactor::RFactor rMinPrecision10 = AddParameterValue( pMinPrecisionParam, D3D_MIN_PRECISION_ANY_10 );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	// Value type
	CTestCaseParameter<EValueType> *pValueTypeParam = AddParameter( _T( "ValueType" ), &m_ValueType );
	testfactor::RFactor rIntType = AddParameterValue( pValueTypeParam, VT_Int );
	testfactor::RFactor rUintType = AddParameterValue( pValueTypeParam, VT_Uint );
	testfactor::RFactor rFloatType = AddParameterValue( pValueTypeParam, VT_Float );
	SetParameterDefaultValue<EValueType>( pValueTypeParam, VT_Float );

	// Values
	CUserValueSet<UINT>* pUintValues = new CUserValueSet<UINT>( );
	pUintValues->AddValue( 0 );
	pUintValues->AddValue( 1 );
	pUintValues->AddValue( 2051 );
	pUintValues->AddValue( USHORT_MAX );
	pUintValues->AddValue( USHORT_MAX - 1 );
	pUintValues->AddValue( USHORT_MAX + 1 );

	CUserValueSet<int>* pIntValues = new CUserValueSet<int>( );
	pIntValues->AddValue( 0 );
	pIntValues->AddValue( 1 );
	pIntValues->AddValue( -2 );
	pIntValues->AddValue( 12004 );
	pIntValues->AddValue( SHORT_MAX );
	pIntValues->AddValue( SHORT_MIN );
	pIntValues->AddValue( SHORT_MIN - 1 );
	pIntValues->AddValue( SHORT_MAX + 1 );

	CUserValueSet<float>* pFloat10Values = new CUserValueSet<float>( );
	pFloat10Values->AddValue( 0.0f );
	pFloat10Values->AddValue( 1.0f / 255.0f );
	pFloat10Values->AddValue( 1.0f );
	pFloat10Values->AddValue( 0.11111111111f );
	pFloat10Values->AddValue( -1.125f );
	pFloat10Values->AddValue( 1.12512341235f );
	pFloat10Values->AddValue( -1.0f / 255.0f );
	pFloat10Values->AddValue( -2.0f );
	pFloat10Values->AddValue( 1.0f + 255.0f / 256.0f );
	pFloat10Values->AddValue( -1.45612312312f );
	pFloat10Values->AddValue( -0.143e-10f );
	pFloat10Values->AddValue( 0.128e-20f );

	CUserValueSet<float>* pFloatValues = new CUserValueSet<float>( );
	pFloatValues->AddValue( -1.0f );
	pFloatValues->AddValue( 0.0f );
	pFloatValues->AddValue( 45.5236236234f );
	pFloatValues->AddValue( -101.125f );
	pFloatValues->AddValue( -1.625f );
	pFloatValues->AddValue( FLT_MAX ); //3.402823466e+38F
	pFloatValues->AddValue( 1.12523423452342f );
	pFloatValues->AddValue( -1.0f / 255.0f );
	pFloatValues->AddValue( -2.0f );
	pFloatValues->AddValue( -3.78f );
	pFloatValues->AddValue( 1.0f / 255.0f );
	pFloatValues->AddValue( 1.0f );
	pFloatValues->AddValue( 1.0f + 1.0f / 255.0f );
	pFloatValues->AddValue( g_fDenormFloat16 );
	pFloatValues->AddValue( 16435278 );	// out of range of float16 (approx max 2 ^ 16 = 65000)
	pFloatValues->AddValue( 1.0f + 255.0f / 256.0f );
	pFloatValues->AddValue( g_fINFn32 );
	pFloatValues->AddValue( g_fINFp32 );
	pFloatValues->AddValue( FLT_MIN );
	pFloatValues->AddValue( -FLT_MIN );

	// Create lists of values
	testfactor::RFactor rValues10Float4 =    InitializeValueFactor<float,1>( this, pFloat10Values, m_SrcFloat, NULL, 4, "" );
	testfactor::RFactor rValuesFloat4 =      InitializeValueFactor<float,1>( this, pFloatValues, m_SrcFloat, NULL, 4, "" );
	testfactor::RFactor rValuesUint4 =       InitializeValueFactor<UINT,1>( this, pUintValues, m_SrcUint, NULL, 4, "u" );
	testfactor::RFactor rValuesInt4 =        InitializeValueFactor<int,1>( this, pIntValues, m_SrcInt, NULL, 4, "i" );

	testfactor::RFactor rIntFactors = 
		rIntType   * rMinPrecision16Plus * rValuesInt4     + 
		rUintType  * rMinPrecision16Plus * rValuesUint4    ; 
	testfactor::RFactor rFloatFactors = 
		rFloatType * rMinPrecision16Plus * rValuesFloat4   + 
		rFloatType * rMinPrecision10     * rValues10Float4 ;

	if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		SetRootTestFactor( rFloatFactors );
	}
	else
	{
		SetRootTestFactor( rFloatFactors + rIntFactors );
	}
	
	// Set priority values
	AddPriorityPatternFilter( FilterNotEqual<D3D_MIN_PRECISION>(pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT), WeightFactorVeryLarge );

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

TEST_RESULT CVSInputPrecisionTest::SetupTestCase()
{
	HRESULT hr;

	// Recreate resources if different than previous test case
	if( m_CreatedValueType != m_ValueType )
	{
		m_pVS = NULL;
		m_pIL = NULL;
		m_pPS = NULL;
		m_pRTV = NULL;
		m_CreatedValueType = m_ValueType;
	}

	if( !CreateShaders() )
	{
		// Error logged in CreateShaders
		return RESULT_FAIL;
	}

	if( !CreateResources() )
	{
		// Error logged in CreateResources
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

DXGI_FORMAT CVSInputPrecisionTest::GetRenderTargetFormat()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	// Determine RT format
	DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		switch( m_ValueType )
		{
		case VT_Float:
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case VT_Uint:
			format = DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		case VT_Int:
			format = DXGI_FORMAT_R32G32B32A32_SINT;
			break;
		}
	}
	else
	{
		assert( m_ValueType == VT_Float );
	}

	return format;
}

DXGI_FORMAT CVSInputPrecisionTest::GetVertexBufferFormat()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();
	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	switch( m_ValueType )
	{
	case VT_Float:
		// Required for VB contents on all feature levels
		format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case VT_Uint:
		// Required for feature level 10+ (and UINTs not tested on 9.x)
		assert( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 );
		format = DXGI_FORMAT_R32G32B32A32_UINT;
		break;
	case VT_Int:
		// Required for feature level 10+ (and UINTs not tested on 9.x)
		assert( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 );
		format = DXGI_FORMAT_R32G32B32A32_SINT;
		break;
	default:
		assert( false );
	}

	return format;
}

char* CVSInputPrecisionTest::GetTypeName( CArithmeticPrecisionTest::EValueType type )
{
	switch( type )
	{
	case VT_Float:
		return "float4";
	case VT_Int:
		return "int4";
	case VT_Uint:
		return "uint4";
	}
	assert( false );
	return "";
}

bool CVSInputPrecisionTest::CreateShaders()
{
	HRESULT hr = S_OK;

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	// The compiler flags can conceivably be modified later if there are situations that require SKIP_OPTIMIZATIONS
	UINT Flags = 0;

	string pMinPrecDecl = MinPrecisionPrefix( m_MinPrecision );
	string sMinPrecType = pMinPrecDecl + GetTypeName( m_ValueType );	// ex. "min8float4"
	string sInterShaderType = GetTypeName( m_ValueType );				// ex. "float4"

	char* pInterp = "";
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		// No interpolation in the pixel shader since we're only passing one value down
		pInterp = "nointerpolation";
	}

	static const char szShaderTemplate[] = 
		"#define TYPE %s \n"
		"#define MINPRECTYPE %s \n"
		"\n"
		"struct VSIn\n"
		"{\n"
		"	float2 pos : pos;\n"
		"	MINPRECTYPE tex : texcoord0;\n"
		"}; \n"
		"struct VSOut\n"
		"{\n"
		"	float4 pos : SV_Position;\n"
		"	TYPE tex : texcoord0;\n"
		"};\n"
		"VSOut VSMain(VSIn input)\n"
		"{\n"
		"	VSOut output;\n"
		"	output.pos.xy = input.pos;\n"
		"	output.pos.z = 0;\n"
		"	output.pos.w = 1;\n"
		"	output.tex = input.tex;\n"
		"	return output;\n"
		"}\n"
		"TYPE PSMain ( %s VSOut input) : SV_Target\n"
		"{\n"
		"	return input.tex; \n"
		"}\n";

	// Form the actual shader code.
	static const UINT shaderBufferSize = sizeof(szShaderTemplate) + 80;
	char szCurShader[shaderBufferSize];
	hr = StringCchPrintf( szCurShader, shaderBufferSize, szShaderTemplate, sInterShaderType.c_str(), sMinPrecType.c_str(), pInterp );
	if( FAILED( hr ) )
	{
		LogError( __FILEW__, __LINE__, L"StringCchPrintf failed.  Perhaps the buffer size is too small.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		return false;
	}

	UINT shaderSize = strlen(szCurShader);
	D3D_FEATURE_LEVEL CompileFL = FeatureLevel < D3D_FEATURE_LEVEL_10_0 ? D3D_FEATURE_LEVEL_9_2 : D3D_FEATURE_LEVEL_10_0;

	// Create VS, IL 
	if( m_pVS == NULL || m_pIL == NULL )
	{
		m_pVS = NULL;
		m_pIL = NULL;

		ATL::CComPtr<ID3D10Blob> pShaderBlob;
		ATL::CComPtr<ID3D10Blob> pErrorBuf;
		hr = g_TestApp.CompileShaderFromMemory( szCurShader, shaderSize, "VSMain", D3D_SHADER_STAGE_VERTEX, CompileFL, CompileFL, Flags, &pShaderBlob, &pErrorBuf );
		if( FAILED( hr ) )
		{
			if( pErrorBuf ) WriteToLog( "CompileShaderFromMemory() failed %s", pErrorBuf->GetBufferPointer() );
			WriteToLog( "Entrypoint: %s    FeatureLevel: %s", "VSMain", ToString( CompileFL ).c_str() );
			WriteToLog( szCurShader );
			LogError( __FILEW__, __LINE__, L"CompileShaderFromMemory() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceCreateShader );
			return false;
		}

		if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pVS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}

		{
			const UINT NumVertexElements = 2;
			D3D11_INPUT_ELEMENT_DESC IADesc[NumVertexElements];
			IADesc[0].InputSlot = 0;
			IADesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			IADesc[0].Format = DXGI_FORMAT_R32G32_FLOAT;
			IADesc[0].SemanticName = "pos";
			IADesc[0].SemanticIndex = 0;
			IADesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			IADesc[0].InstanceDataStepRate = 0;
			IADesc[1].InputSlot = 0;
			IADesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			IADesc[1].Format = GetVertexBufferFormat();
			IADesc[1].SemanticName = "texcoord";
			IADesc[1].SemanticIndex = 0;
			IADesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			IADesc[1].InstanceDataStepRate = 0;

			if( FAILED( hr = GetDevice()->CreateInputLayout( IADesc, NumVertexElements, pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), &m_pIL ) ) )
			{
				LogError( __FILEW__, __LINE__, L"CreateInputLayout() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateInputLayout );
				return false;
			}
		}
	}


	// Create PS
	if( m_pPS == NULL )
	{
		ATL::CComPtr<ID3D10Blob> pShaderBlob;
		ATL::CComPtr<ID3D10Blob> pErrorBuf;
		hr = g_TestApp.CompileShaderFromMemory( szCurShader, shaderSize, "PSMain", D3D_SHADER_STAGE_PIXEL, CompileFL, CompileFL, Flags, &pShaderBlob, &pErrorBuf );
		if( FAILED( hr ) )
		{
			if( pErrorBuf ) WriteToLog( "CompileShaderFromMemory() failed %s", pErrorBuf->GetBufferPointer() );
			WriteToLog( "Entrypoint: %s    FeatureLevel: %s", "PSMain", ToString( CompileFL ).c_str() );
			WriteToLog( szCurShader );
			LogError( __FILEW__, __LINE__, L"CompileShaderFromMemory() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceCreateShader );
			return false;
		}

		if( FAILED( hr = GetDevice()->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pPS ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateVertexShader() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateShader );
			return false;
		}
	}

	return true;
}


bool CVSInputPrecisionTest::CreateResources()
{
	if( !CArithmeticPrecisionTest::CreateResources() )
	{
		return false;
	}

	HRESULT hr = S_OK;

	// Create VB
	m_pVB = NULL;
	{
		Vertex VBData;

		// Drawing a single point at (0,0)
		VBData.pos[0] = VBData.pos[1] = 0;

		switch( m_ValueType )
		{
		case VT_Float:
			memcpy( VBData.fTexcoord, &m_SrcFloat[0].values, sizeof(float) * 4 );
			break;
		case VT_Uint:
			memcpy( VBData.uTexcoord, &m_SrcUint[0].values, sizeof(UINT) * 4 );
			break;
		case VT_Int:
			memcpy( VBData.iTexcoord, &m_SrcInt[0].values, sizeof(int) * 4 );
			break;
		}
		
		D3D11_SUBRESOURCE_DATA bufInit = { &VBData, sizeof(VBData), 0 };
		D3D11_BUFFER_DESC bufDesc = { sizeof(VBData), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufDesc, &bufInit, &m_pVB ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateBuffer() failed for VB.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}
	}
	return true;
}


TEST_RESULT CVSInputPrecisionTest::ExecuteTestCase()
{
	HRESULT hr;

	{
		UINT Stride = sizeof(Vertex);
		UINT Offset = 0;

		// Note that the position has min precision, which means it can be clamped.  In the case of 8-bit precision,
		// it can be clamped to [0,1] which means that only the top-right quadrant of the render target will be covered
		D3D11_VIEWPORT vp = { 0,0, m_RTWidth,m_RTHeight, 0, 1 };

		assert( m_pIL != NULL );
		assert( m_pVB != NULL );
		assert( m_pVS != NULL );
		assert( m_pPS != NULL );
		assert( m_pRTV != NULL );
		assert( m_pRS != NULL );

		FLOAT clearColor[4] = {0};
		GetEffectiveContext()->ClearRenderTargetView( m_pRTV, clearColor );

		GetEffectiveContext()->IASetInputLayout( m_pIL );
		GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVB, &Stride, &Offset );
		GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
		GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
		GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, NULL );
		GetEffectiveContext()->RSSetState( m_pRS );
		GetEffectiveContext()->RSSetViewports( 1, &vp );

		GetEffectiveContext()->Draw( 1, 0 );

		// reset the context
		ID3D11RenderTargetView* ppNULLRTV[] = {NULL};
		ID3D11Buffer* ppNULLBuff[] = {NULL};

		GetEffectiveContext()->IASetInputLayout( NULL );
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, ppNULLBuff, &Stride, &Offset );
		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->RSSetState( NULL );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			LogError( __FILEW__, __LINE__, L"ExecuteEffectiveContext() failed.", false, ERRORCODE_TYPE_BOOL, gFCDeviceContextExecute );
			return RESULT_FAIL;
		}
	}

	if( RESULT_PASS != ValidateResult( ) )
	{
		// Error logged in ValidateResult
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

TEST_RESULT CVSInputPrecisionTest::ValidateResult()
{
	D3D11_MAPPED_SUBRESOURCE mapTex2D;
	HRESULT hr = S_OK;

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	GetImmediateContext()->CopyResource( m_pRT2DStaging, m_pRT2D );

	CMapHelper mapHelper( GetImmediateContext() );
	hr = mapHelper.Map( m_pRT2DStaging, 0, D3D11_MAP_READ, 0, &mapTex2D );
	if( FAILED( hr ) )
	{
		LogError( __FILEW__, __LINE__, L"m_pRT2DStaging Map() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
		return RESULT_FAIL;
	}

	// We do not loop through data below.
	assert( m_RTWidth == 1 && m_RTHeight == 1 );

	const UINT NumComponents = 4;

	D3D11_TEXTURE2D_DESC tex2DDesc;
	m_pRT2DStaging->GetDesc( &tex2DDesc );

	EPrecisionLevel MinPrecisionCheck = CMinPrecValue<float>::ConvertToPrecisionLevel( m_MinPrecision );
	// In 9.x, only 16 bit floats are outputted, so make sure we don't expect more precision than 16 bits
	if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 && m_ValueType == CArithmeticPrecisionTest::VT_Float && MinPrecisionCheck == EPL_32 )
	{
		assert( tex2DDesc.Format == DXGI_FORMAT_R16G16B16A16_FLOAT );
		MinPrecisionCheck = EPL_16;
	}


	switch( m_ValueType )
	{
	case CArithmeticPrecisionTest::VT_Float:
		{
			FLOAT_COLOR fPixel;
			bool bConverted = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
							   tex2DDesc.Format, 
							   &fPixel, 
							   1, 
							   1, 
							   1, 
							   (BYTE*)mapTex2D.pData );

			if (!bConverted)
			{
				LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
				return RESULT_FAIL;
			}

			for( UINT comp = 0; comp < NumComponents; comp++ )
			{
				// Generate the expected value
				CMinPrecValue<float> expected( m_SrcFloat[0][comp] );
				expected.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );

				// Select the component to read
				float pixel = fPixel.comp[comp];

				// Check for exact precision
				float ULPMultiplier = 1.0f;
				if( !expected.Matches( pixel, MinPrecisionCheck, FeatureLevel, false, ULPMultiplier ) )
				{
					WriteActualResultAndExpectedResultsToTheLog<float>( 0, comp, pixel, expected );
					LogError( __FILEW__, __LINE__, L"Result not within expected ranges.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
			}
		}
		break;
	case CArithmeticPrecisionTest::VT_Int:
		{
			int* pPixel = reinterpret_cast<int*>( mapTex2D.pData );
			for( UINT comp = 0; comp < NumComponents; comp++ )
			{
				// Generate the expected value
				CMinPrecValue<int> expected( m_SrcInt[0][comp] );
				expected.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );

				// Select the component to read
				int pixel = pPixel[comp];

				// Check for exact precision
				if( !expected.Matches( pixel, MinPrecisionCheck, FeatureLevel, true ) )
				{
					WriteActualResultAndExpectedResultsToTheLog<int>( 0, comp, pixel, expected );
					LogError( __FILEW__, __LINE__, L"Result not within expected ranges.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
			}
		}
		break;
	case CArithmeticPrecisionTest::VT_Uint:
		{
			UINT* pPixel = reinterpret_cast<UINT*>( mapTex2D.pData );
			for( UINT comp = 0; comp < NumComponents; comp++ )
			{
				// Generate the expected value
				CMinPrecValue<UINT> expected( m_SrcUint[0][comp] );
				expected.EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );

				// Select the component to read
				UINT pixel = pPixel[comp];

				// Check for exact precision
				if( !expected.Matches( pixel, MinPrecisionCheck, FeatureLevel, true ) )
				{
					WriteActualResultAndExpectedResultsToTheLog<UINT>( 0, comp, pixel, expected );
					LogError( __FILEW__, __LINE__, L"Result not within expected ranges.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
					return RESULT_FAIL;
				}
			}
		}
		break;
	}

	// m_pRT2DStaging unmapped automatically.

	return RESULT_PASS;
}


