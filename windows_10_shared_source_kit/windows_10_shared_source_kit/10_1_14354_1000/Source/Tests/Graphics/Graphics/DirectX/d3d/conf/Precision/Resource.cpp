#include "Resource.h"
#include "FormatDesc.hpp"

//---------------------------------------------------------------


BEGIN_NAMED_VALUES( RES_OP )
	NAMED_VALUE( _T( "sample" ), RO_SAMPLE )
	NAMED_VALUE( _T( "gather" ), RO_GATHER )
	NAMED_VALUE( _T( "ld" ), RO_LD )
	NAMED_VALUE( _T( "ld2dms" ), RO_LD2DMS )
	NAMED_VALUE( _T( "bufinfo" ), RO_BUFINFO )
	NAMED_VALUE( _T( "resinfo" ), RO_RESINFO )
	NAMED_VALUE( _T( "resinfo_float" ), RO_RESINFO_FLOAT )
	NAMED_VALUE( _T( "ld_uav_typed" ), RO_LDUAVTYPED )
END_NAMED_VALUES( RES_OP )

BEGIN_NAMED_VALUES( RES_SOURCE )
	NAMED_VALUE( _T( "SRV" ), RS_SRV )
	NAMED_VALUE( _T( "UAV" ), RS_UAV )
END_NAMED_VALUES( RES_SOURCE )

float GetBorderColor( D3D_FEATURE_LEVEL fl )
{
	if( fl < D3D_FEATURE_LEVEL_10_0 )
	{
		return 63.0f / 255.0f;
	}
	else
	{
		return 0.25f;
	}
}


/*************************************************************
* CResourcePrecisionTest Setup and Initialization methods     *
**************************************************************/

TEST_RESULT CResourcePrecisionTest::Setup()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	if( FeatureLevel < D3D_FEATURE_LEVEL_9_3 )
	{
		WriteToLog( "This test requires 9.3 or higher feature levels." );
		return RESULT_SKIP;
	}

	return CPrecisionTest::Setup();
}

void CResourcePrecisionTest::InitTestParameters()
{
	// RefreshGraphics() in order to ensure the framework device is available during parameter initialization
	// BUGBUG: WinBlue 551704: This is a hack to enable the scenario of accessing the device in InitTestParameters()
	// This should not be copied into other tests. It may not be supported in the future.
	GetFramework()->RefreshGraphics();

	if( !GetDevice() )
		return;

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwOptions = {0};
	HRESULT hr = S_OK;
	if( FAILED( hr = GetDevice()->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwOptions, sizeof(hwOptions) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed for D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCheckFeatureSupport );
	}
	const BOOL bSupportsCS4 = hwOptions.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x;


	// Initialize test factors

	// Operation
	CTestCaseParameter<RES_OP> *pInstructionParam = AddParameter( _T( "Inst" ), &m_ResOp );
	testfactor::RFactor rsample = AddParameterValue( pInstructionParam, RO_SAMPLE );
	testfactor::RFactor rgather;
	testfactor::RFactor rld;
	testfactor::RFactor rld2dms;
	testfactor::RFactor rbufinfo;
	testfactor::RFactor rresinfo;
	testfactor::RFactor rresinfoF;
	testfactor::RFactor rlduav;
	if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		rgather = AddParameterValue( pInstructionParam, RO_GATHER );
		rlduav = AddParameterValue( pInstructionParam, RO_LDUAVTYPED );	
		rld2dms = AddParameterValue( pInstructionParam, RO_LD2DMS );
		rbufinfo = AddParameterValue( pInstructionParam, RO_BUFINFO );
	}
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		rld = AddParameterValue( pInstructionParam, RO_LD );
		rresinfo = AddParameterValue( pInstructionParam, RO_RESINFO );	
		rresinfoF = AddParameterValue( pInstructionParam, RO_RESINFO_FLOAT );	
	}
	SetParameterDefaultValue<RES_OP>( pInstructionParam, RO_SAMPLE );

	// Resource Dimensions
	CTestCaseParameter<D3D_SRV_DIMENSION> *pDimParam = AddParameter( _T( "Dimension" ), &m_Dimension );
	testfactor::RFactor rTex1D;
	testfactor::RFactor rTex1DArray;
	testfactor::RFactor rTex2D = AddParameterValue( pDimParam, D3D_SRV_DIMENSION_TEXTURE2D );
	testfactor::RFactor rTex2DArray;
	testfactor::RFactor rTex2DMS;
	testfactor::RFactor rTex2DMSArray;
	testfactor::RFactor rDimension3D = AddParameterValue( pDimParam, D3D_SRV_DIMENSION_TEXTURE3D );
	testfactor::RFactor rDimensionBuffer;
	if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		rTex2DMS = AddParameterValue( pDimParam, D3D_SRV_DIMENSION_TEXTURE2DMS );
		rTex2DMSArray = AddParameterValue( pDimParam, D3D_SRV_DIMENSION_TEXTURE2DMSARRAY );
	}
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		rDimensionBuffer = AddParameterValue( pDimParam, D3D_SRV_DIMENSION_BUFFER );
		rTex1D = AddParameterValue( pDimParam, D3D_SRV_DIMENSION_TEXTURE1D );
		rTex1DArray = AddParameterValue( pDimParam, D3D_SRV_DIMENSION_TEXTURE1DARRAY );
		rTex2DArray = AddParameterValue( pDimParam, D3D_SRV_DIMENSION_TEXTURE2DARRAY );
	}

	testfactor::RFactor rDimension1D = rTex1D + rTex1DArray;
	testfactor::RFactor rDimension2DNonMS = rTex2D + rTex2DArray;
	testfactor::RFactor rDimension2DMS = rTex2DMS + rTex2DMSArray;
	testfactor::RFactor rDimension2D = rDimension2DNonMS + rDimension2DMS;
	testfactor::RFactor rDimensionTexNonMS = rDimension1D + rDimension2DNonMS + rDimension3D;
	testfactor::RFactor rDimensionNonMS = rDimensionBuffer + rDimensionTexNonMS;
	testfactor::RFactor rDimension = rDimensionNonMS + rDimension2DMS;
	testfactor::RFactor rDimensionTex = rDimensionTexNonMS + rDimension2DMS;
	SetParameterDefaultValue<D3D_SRV_DIMENSION>( pDimParam, D3D_SRV_DIMENSION_BUFFER );

	// Formats
	CTestCaseParameter<DXGI_FORMAT> *pFormatParam = AddParameter( _T( "Format" ), &m_Format );
	testfactor::RFactor rFloatUAVFormats = AddParameterValue( pFormatParam, DXGI_FORMAT_R32_FLOAT );
	testfactor::RFactor rUintUAVFormats = AddParameterValue( pFormatParam, DXGI_FORMAT_R32_UINT );
	testfactor::RFactor rIntUAVFormats = AddParameterValue( pFormatParam, DXGI_FORMAT_R32_SINT );
	testfactor::RFactor rSRGBFloatFormats = AddParameterValue( pFormatParam, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB );
	CUserValueSet<DXGI_FORMAT> *pGatherFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R32G32B32A32_FLOAT );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R8G8_SNORM );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R16G16B16A16_UNORM );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R16G16B16A16_FLOAT );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R16G16B16A16_SNORM );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R32G32_FLOAT );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_UNORM );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R16G16_FLOAT );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R16G16_SNORM );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_SNORM );
	pGatherFormatValues->AddValue( DXGI_FORMAT_R8G8_UNORM );
	testfactor::RFactor rMostGatherFloatFormats = AddParameterValueSet( pFormatParam, pGatherFormatValues );
	CUserValueSet<DXGI_FORMAT> *pOtherFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pOtherFormatValues->AddValue( DXGI_FORMAT_R32G32B32_FLOAT );
	testfactor::RFactor rOtherFloatFormats = AddParameterValueSet( pFormatParam, pOtherFormatValues );
	CUserValueSet<DXGI_FORMAT> *pOtherUintFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pOtherUintFormatValues->AddValue( DXGI_FORMAT_R10G10B10A2_UINT );
	pOtherUintFormatValues->AddValue( DXGI_FORMAT_R32G32B32A32_UINT );
	pOtherUintFormatValues->AddValue( DXGI_FORMAT_R32G32B32_UINT );
	pOtherUintFormatValues->AddValue( DXGI_FORMAT_R16G16B16A16_UINT );
	pOtherUintFormatValues->AddValue( DXGI_FORMAT_R32G32_UINT );
	pOtherUintFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_UINT );
	pOtherUintFormatValues->AddValue( DXGI_FORMAT_R8G8_UINT );
	pOtherUintFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_UINT );
	testfactor::RFactor rOtherUintFormats = AddParameterValueSet( pFormatParam, pOtherUintFormatValues );
	CUserValueSet<DXGI_FORMAT> *pOtherIntFormatValues = new CUserValueSet<DXGI_FORMAT>( );
	pOtherIntFormatValues->AddValue( DXGI_FORMAT_R32G32B32A32_SINT );
	pOtherIntFormatValues->AddValue( DXGI_FORMAT_R16_SINT );
	pOtherIntFormatValues->AddValue( DXGI_FORMAT_R32G32B32_SINT );
	pOtherIntFormatValues->AddValue( DXGI_FORMAT_R16G16B16A16_SINT );
	pOtherIntFormatValues->AddValue( DXGI_FORMAT_R32G32_SINT );
	pOtherIntFormatValues->AddValue( DXGI_FORMAT_R8G8B8A8_SINT );
	pOtherIntFormatValues->AddValue( DXGI_FORMAT_R8G8_SINT );
	pOtherIntFormatValues->AddValue( DXGI_FORMAT_R16G16_SINT );
	testfactor::RFactor rOtherIntFormats = AddParameterValueSet( pFormatParam, pOtherIntFormatValues );

	testfactor::RFactor rGatherFloatFormats = rFloatUAVFormats + rMostGatherFloatFormats;
	testfactor::RFactor rFloatFormatsNoSRGB = rFloatUAVFormats + rGatherFloatFormats;
	testfactor::RFactor rFloatFormats = rFloatFormatsNoSRGB + rSRGBFloatFormats;
	testfactor::RFactor rFloatFormatsMS = rFloatFormats;
	testfactor::RFactor rUintFormats = rUintUAVFormats + rOtherUintFormats;
	testfactor::RFactor rUintFormatsMS = rUintFormats;
	testfactor::RFactor rIntFormats = rIntUAVFormats + rOtherIntFormats;
	testfactor::RFactor rIntFormatsMS = rIntFormats;

	SetParameterDefaultValue<DXGI_FORMAT>( pFormatParam, DXGI_FORMAT_UNKNOWN );

	// Dest Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pDestMinPrecisionParam = AddParameter( _T( "DestMinPrecision" ), &m_DestMinPrecision );
	CUserValueSet<D3D_MIN_PRECISION> *pDestPrecisionValues = new CUserValueSet<D3D_MIN_PRECISION>( );
	pDestPrecisionValues->AddValue( D3D_MIN_PRECISION_DEFAULT );
	pDestPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_16 );
	testfactor::RFactor rDestMinPrecision16Plus = AddParameterValueSet( pDestMinPrecisionParam, pDestPrecisionValues );
	pDestPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_10 );
	testfactor::RFactor rDestMinPrecision8Plus = AddParameterValueSet( pDestMinPrecisionParam, pDestPrecisionValues );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pDestMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	// Source Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pMinPrecisionParam = AddParameter( _T( "TexcoordMinPrecision" ), &m_MinPrecision );
	CUserValueSet<D3D_MIN_PRECISION> *pPrecisionValues = new CUserValueSet<D3D_MIN_PRECISION>( );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_DEFAULT );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_16 );
	testfactor::RFactor rMinPrecision16Plus = AddParameterValueSet( pMinPrecisionParam, pPrecisionValues );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_10 );
	testfactor::RFactor rMinPrecision8Plus = AddParameterValueSet( pMinPrecisionParam, pPrecisionValues );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	// View Type
	CTestCaseParameter<RES_SOURCE> *pTypeParam = AddParameter( _T( "ViewType" ), &m_ViewType );
	testfactor::RFactor rSRV = AddParameterValue( pTypeParam, RS_SRV );
	testfactor::RFactor rUAV;
	if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		// Not available in CS4 because CS4 allows only one UAV
		rUAV = AddParameterValue( pTypeParam, RS_UAV );
	}
	SetParameterDefaultValue<RES_SOURCE>( pTypeParam, RS_SRV );

	// Texcoord
	CTestCaseParameter<Vec4<float>> *pTexcoordParam = AddParameter( _T( "Texcoord" ), &m_Texcoord );
	CUserValueSet<Vec4<float>> *pTexcoordValues = new CUserValueSet<Vec4<float>>( );
	pTexcoordValues->AddValue( Vec4<float>( 0.123f, 0.88888888f, 0, 0 ) );
	pTexcoordValues->AddValue( Vec4<float>( 0.223f, 0.23243f, 1, 0 ) );
	pTexcoordValues->AddValue( Vec4<float>( 0.11111f, 0.6919222f, 0.75f, 0 ) );
	pTexcoordValues->AddValue( Vec4<float>( 0.223f, 0.9999999f, 0.2222f, 0 ) );
	pTexcoordValues->AddValue( Vec4<float>( 0.55555f, 0.5f, 0.98f, 0 ) );
	pTexcoordValues->AddValue( Vec4<float>( 0.787654f, 0.0001f, 0.02f, 0 ) );
	testfactor::RFactor rTexcoords = AddParameterValueSet( pTexcoordParam, pTexcoordValues );

	CTestCaseParameter<Vec4<UINT>> *pLdcoordParam = AddParameter( _T( "Ldcoord" ), &m_Ldcoord );
	CUserValueSet<Vec4<UINT>> *pLdcoordValues = new CUserValueSet<Vec4<UINT>>( );
	// x: X, y: Y, z: Array index (ignored for non-array) or depth, w: mip
	pLdcoordValues->AddValue( Vec4<UINT>( 4, 7, 0, 0 ) );
	pLdcoordValues->AddValue( Vec4<UINT>( 14, 17, 1, 0 ) );
	pLdcoordValues->AddValue( Vec4<UINT>( 31, 3, 2, 0 ) );
	pLdcoordValues->AddValue( Vec4<UINT>( 20, 21, 1, 0 ) );
	pLdcoordValues->AddValue( Vec4<UINT>( 19, 31, 3, 0 ) );
	pLdcoordValues->AddValue( Vec4<UINT>( 0, 12, 3, 0 ) );
	pLdcoordValues->AddValue( Vec4<UINT>( 1, 18, 0, 0 ) );
	testfactor::RFactor rLdcoords = AddParameterValueSet( pLdcoordParam, pLdcoordValues );


	// Combine factors
	testfactor::RFactor rInstDimFormatCoord = 
		rsample   * rDimensionTexNonMS * rFloatFormats       * rTexcoords * rSRV % rDestMinPrecision8Plus  % rMinPrecision8Plus  +

		rgather   * rDimension2DNonMS  * rGatherFloatFormats * rTexcoords * rSRV % rDestMinPrecision8Plus  % rMinPrecision8Plus  +

		rld       * rDimensionTexNonMS * rFloatFormats       * rLdcoords  * rSRV % rDestMinPrecision8Plus  % rMinPrecision16Plus +
		rld       * rDimensionTexNonMS * rUintFormats        * rLdcoords  * rSRV % rDestMinPrecision16Plus % rMinPrecision16Plus +
		rld       * rDimensionTexNonMS * rIntFormats         * rLdcoords  * rSRV % rDestMinPrecision16Plus % rMinPrecision16Plus +
		rld       * rDimensionBuffer   * rFloatFormatsNoSRGB * rLdcoords  * rSRV % rDestMinPrecision8Plus  % rMinPrecision16Plus +
		rld       * rDimensionBuffer   * rUintFormats        * rLdcoords  * rSRV % rDestMinPrecision16Plus % rMinPrecision16Plus +
		rld       * rDimensionBuffer   * rIntFormats         * rLdcoords  * rSRV % rDestMinPrecision16Plus % rMinPrecision16Plus +

		rld2dms   * rDimension2DMS     * rFloatFormatsMS     * rLdcoords  * rSRV % rDestMinPrecision8Plus  % rMinPrecision16Plus +
		rld2dms   * rDimension2DMS     * rUintFormatsMS      * rLdcoords  * rSRV % rDestMinPrecision16Plus % rMinPrecision16Plus +
		rld2dms   * rDimension2DMS     * rIntFormatsMS       * rLdcoords  * rSRV % rDestMinPrecision16Plus % rMinPrecision16Plus +

		rbufinfo  * rDimensionBuffer   * rFloatFormatsNoSRGB              * rSRV % rDestMinPrecision16Plus                       +
		rbufinfo  * rDimensionBuffer   * rUintFormats                     * rSRV % rDestMinPrecision16Plus                       +
		rbufinfo  * rDimensionBuffer   * rIntFormats                      * rSRV % rDestMinPrecision16Plus                       +

		rresinfo  * rDimensionTex      % rFloatFormatsNoSRGB              % rSRV % rDestMinPrecision16Plus                       +
		rresinfo  * rDimensionTex      % rUintFormats                     % rSRV % rDestMinPrecision16Plus                       +
		rresinfo  * rDimensionTex      % rIntFormats                      % rSRV % rDestMinPrecision16Plus                       +

		rresinfoF * rDimensionTex      % rFloatFormats                    % rSRV % rDestMinPrecision8Plus                        +
		rresinfoF * rDimensionTex      % rUintFormats                     % rSRV % rDestMinPrecision8Plus                        +
		rresinfoF * rDimensionTex      % rIntFormats                      % rSRV % rDestMinPrecision8Plus                        ;

	testfactor::RFactor rInstDimFormatCoordCS = 
		rlduav    * rDimensionNonMS    * rFloatUAVFormats    * rLdcoords  * rUAV % rDestMinPrecision8Plus  % rMinPrecision16Plus +
		rlduav    * rDimensionNonMS    * rUintUAVFormats     * rLdcoords  * rUAV % rDestMinPrecision8Plus  % rMinPrecision16Plus +
		rlduav    * rDimensionNonMS    * rIntUAVFormats      * rLdcoords  * rUAV % rDestMinPrecision8Plus  % rMinPrecision16Plus +

		rbufinfo  * rDimensionBuffer   * rFloatUAVFormats                 * rUAV % rDestMinPrecision16Plus                       +
		rbufinfo  * rDimensionBuffer   * rUintUAVFormats                  * rSRV % rDestMinPrecision16Plus                       +
		rbufinfo  * rDimensionBuffer   * rIntUAVFormats                   * rSRV % rDestMinPrecision16Plus                       +

		rresinfo  * rDimensionTexNonMS % rFloatUAVFormats                 % rSRV % rDestMinPrecision16Plus                       +
		rresinfo  * rDimensionTexNonMS % rUintUAVFormats                  % rUAV % rDestMinPrecision16Plus                       +
		rresinfo  * rDimensionTexNonMS % rIntUAVFormats                   % rSRV % rDestMinPrecision16Plus                       +

		rresinfoF * rDimensionTexNonMS % rFloatUAVFormats                 % rSRV % rDestMinPrecision8Plus                        +
		rresinfoF * rDimensionTexNonMS % rUintUAVFormats                  % rSRV % rDestMinPrecision8Plus                        +
		rresinfoF * rDimensionTexNonMS % rIntUAVFormats                   % rUAV % rDestMinPrecision8Plus                        ;


	// Shader Stage
	testfactor::RFactor rShaderStage = InitializeShaderStageModifierFactor();
	testfactor::RFactor rCS;
	CTestCaseParameter<D3D_SHADER_VARIABLE_TYPE> *pShaderStageParam = static_cast<CTestCaseParameter<D3D_SHADER_VARIABLE_TYPE>*>( GetTestCaseParameter( _T("ShaderStage") ) );
	assert( pShaderStageParam != NULL );
	if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 || bSupportsCS4 )
	{
		rCS = AddParameterValue( pShaderStageParam, D3D_SVT_COMPUTESHADER );
	}
	else if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		// Force PS in 9.x since VS doesn't support SRVs
		rShaderStage = AddParameterValue( pShaderStageParam, D3D_SVT_PIXELSHADER );
	}

	// Set Root Factor
	SetRootTestFactor( rInstDimFormatCoord % rShaderStage + rInstDimFormatCoordCS * rCS );


	// Set priority values
	AddPriorityPatternFilter( FilterNotEqual<D3D_MIN_PRECISION>(pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT), WeightFactorVeryLarge );
	AddPriorityPatternFilter( FilterNotEqual<D3D_MIN_PRECISION>(pDestMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT), WeightFactorVeryLarge );
	AddPriorityPatternFilter( FilterEqual<D3D_SHADER_VARIABLE_TYPE>(pShaderStageParam, D3D_SVT_COMPUTESHADER), WeightFactorLarge );
	AddPriorityPatternFilter( FilterEqual<D3D_SHADER_VARIABLE_TYPE>(pShaderStageParam, D3D_SVT_PIXELSHADER), WeightFactorLarge );
	AddPriorityPatternFilter( FilterEqual<D3D_SRV_DIMENSION>(pDimParam, D3D_SRV_DIMENSION_TEXTURE2D), WeightFactorLarge );
	AddPriorityPatternFilter( FilterEqual<D3D_SRV_DIMENSION>(pDimParam, D3D_SRV_DIMENSION_BUFFER), WeightFactorLarge );
	AddPriorityPatternFilter( FilterEqual<RES_OP>(pInstructionParam, RO_BUFINFO), WeightFactorSmall );
	AddPriorityPatternFilter( FilterEqual<RES_OP>(pInstructionParam, RO_RESINFO), WeightFactorSmall );
	
	DeclarePriorityLevelByPriorityValue( 1, 4.0f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );
}


/*************************************************************
* Shader generation methods                                  *
**************************************************************/

CArithmeticPrecisionTest::EValueType CResourcePrecisionTest::GetInterShaderValueType()
{
	return GetShaderOperationDestValueType();
}

CArithmeticPrecisionTest::EValueType CResourcePrecisionTest::GetShaderOperationDestValueType()
{
	// BufInfo and ResInfo return UINT values
	if( IsResinfoUintOperation() )
	{
		return VT_Uint;
	}
	else if( IsResinfoFloatOperation() )
	{
		return VT_Float;
	}
	else
	{
		return GetTextureType();
	}
}

CArithmeticPrecisionTest::EValueType CResourcePrecisionTest::GetTextureType()
{
	D3D10_FORMAT_COMPONENT_INTERPRETATION detail = CD3D10FormatHelper_Test::GetFormatComponentInterpretation( m_Format, 0 );

	switch( detail )
	{
	case D3D10FCI_SNORM:
	case D3D10FCI_UNORM:
	case D3D10FCI_UNORM_SRGB:
	case D3D10FCI_FLOAT: return VT_Float;
	case D3D10FCI_SINT: return VT_Int;
	case D3D10FCI_UINT: return VT_Uint;
	}

	assert( false );
	return VT_Float;
}

// Returns the coordinate swizzle for Load coordinates.  We do this for Load and not for Sample because for Sample,
// Array indices and mip levels are clamped so we don't have to worry about out-of-bounds there.
string CResourcePrecisionTest::GetSRVLoadCoordinates()
{
	switch( m_Dimension )
	{
	case D3D_SRV_DIMENSION_TEXTURE1D: return "xw";
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY: return "xzw";
	case D3D_SRV_DIMENSION_TEXTURE2D: return "xyw";
	case D3D_SRV_DIMENSION_TEXTURE2DMS: return "xy,0";
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY: return "xyzw";
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY: return "xyz,0";
	case D3D_SRV_DIMENSION_TEXTURE3D: return "xyzw";
	case D3D_SRV_DIMENSION_BUFFER: return "x";
	default:
		assert( false );
	}
	return "";
}

string CResourcePrecisionTest::GetUAVLoadCoordinates()
{
	// Same as SRV except no mip coordinate
	switch( m_Dimension )
	{
	case D3D_SRV_DIMENSION_TEXTURE1D: return "x";
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY: return "xz";
	case D3D_SRV_DIMENSION_TEXTURE2D: return "xy";
	case D3D_SRV_DIMENSION_TEXTURE2DMS: return "xy,0";
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY: return "xyz";
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY: return "xyz,0";
	case D3D_SRV_DIMENSION_TEXTURE3D: return "xyz";
	case D3D_SRV_DIMENSION_BUFFER: return "x";
	default:
		assert( false );
	}
	return "";
}

string CResourcePrecisionTest::GetResinfoCoordinates()
{
	// Same as SRV except no mip coordinate
	switch( m_Dimension )
	{
	case D3D_SRV_DIMENSION_BUFFER:
	case D3D_SRV_DIMENSION_TEXTURE1D: return "value.x";
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
	case D3D_SRV_DIMENSION_TEXTURE2D: return "value.x,value.y";
	case D3D_SRV_DIMENSION_TEXTURE2DMS: 
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
	case D3D_SRV_DIMENSION_TEXTURE3D: return "value.x,value.y,value.z";
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:  return "value.x,value.y,value.z,value.w";
	default:
		assert( false );
	}
	return "";
}

string CResourcePrecisionTest::OperationToString()
{
	string result = "";
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	switch( m_ResOp )
	{
	case RO_SAMPLE: 
		if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
		{
			result = "value = resource.SampleLevel(MySampler,val0,0);"; 
		}
		else
		{
			// SampleLevel not supported on 9.x
			result = "value = resource.Sample(MySampler,val0);"; 
		}
		break;
	case RO_GATHER: 
		result = "value = resource.Gather(MySampler,val0,0);"; 
		break;
	case RO_LD: 
	case RO_LD2DMS: 
		result = "value = resource.Load(val0." + GetSRVLoadCoordinates() + ");"; 
		break;
	case RO_LDUAVTYPED: 
		result = "value = resource[val0." + GetUAVLoadCoordinates() + "];"; 
		break;
	case RO_BUFINFO: 
	case RO_RESINFO: 
	case RO_RESINFO_FLOAT:
		result = "resource.GetDimensions(" + GetResinfoCoordinates() + ");"; 
		break;
	}

	return result;
}

char* CResourcePrecisionTest::GetTypeName( CArithmeticPrecisionTest::EValueType type )
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

char* CResourcePrecisionTest::GetResourceTypeName( CArithmeticPrecisionTest::EValueType type )
{
	if( m_ResOp == RO_LDUAVTYPED )
	{
		switch( type )
		{
		case VT_Float:
			return "float";
		case VT_Int:
			return "int";
		case VT_Uint:
			return "uint";
		}
		assert( false );
		return "";
	}
	else
		return GetTypeName( type );
}

string CResourcePrecisionTest::TextureDeclaration()
{
	string sDecl = "";
	switch( m_Dimension )
	{
	case D3D_SRV_DIMENSION_TEXTURE1D: sDecl = "Texture1D"; break;
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY: sDecl = "Texture1DArray"; break;
	case D3D_SRV_DIMENSION_TEXTURE2D: sDecl = "Texture2D"; break;
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY: sDecl = "Texture2DArray"; break;
	case D3D_SRV_DIMENSION_TEXTURE3D: sDecl = "Texture3D"; break;
	case D3D_SRV_DIMENSION_TEXTURE2DMS: sDecl = "Texture2DMS"; break;
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY: sDecl = "Texture2DMSArray"; break;
	case D3D_SRV_DIMENSION_BUFFER: sDecl = "Buffer"; break;
	default:
		assert( false );
	}

	if( m_ViewType == RS_UAV )
	{
		sDecl = "RW" + sDecl;
	}

	sDecl = sDecl + "<" + GetResourceTypeName( GetTextureType() ) + "> resource; \n";
	return sDecl;
}

CArithmeticPrecisionTest::EValueType CResourcePrecisionTest::TextureCoordType()
{
	CTestCaseParameterBase* pLdcoordParam = GetTestCaseParameter( _T( "Ldcoord" ) );
	if( pLdcoordParam->IsActive() )
		return VT_Uint;
	else
		return VT_Float;
}

bool CResourcePrecisionTest::CreateShaders()
{
	HRESULT hr;

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();
	const EValueType DestValueType = GetShaderOperationDestValueType();
	const EValueType InterShaderValueType = GetInterShaderValueType();
	SPassThroughShaders& PassThroughShaders = m_PassThroughShaders[ static_cast<UINT>(InterShaderValueType) ];

	string pSrcDecl = MinPrecisionPrefix( m_MinPrecision );
	string pDestDecl = MinPrecisionPrefix( m_DestMinPrecision );

	// The compiler flags can conceivably be modified later if there are situations that require SKIP_OPTIMIZATIONS
	UINT Flags = 0;

	string sDestType = pDestDecl + GetTypeName( DestValueType );		// ex. "min8float4"
	string sInterShaderType = GetTypeName( InterShaderValueType );		// ex. "float4"


	string sDecl = TextureDeclaration();
	string sArithmetic = OperationToString();
	string sTexcoordType = GetTypeName( TextureCoordType() );

	// Add arithmetic to one shader
	const char* pVSCode = "";
	const char* pHSCode = "";
	const char* pDSCode = "";
	const char* pGSCode = "";
	const char* pPSCode = "";
	const char* pCSCode = "";
	switch( m_ShaderStage )
	{
	case D3D_SVT_VERTEXSHADER:
		pVSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_HULLSHADER:
		pHSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_DOMAINSHADER:
		pDSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_GEOMETRYSHADER:
		pGSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_PIXELSHADER:
		pPSCode = sArithmetic.c_str();
		break;
	case D3D_SVT_COMPUTESHADER:
		pCSCode = sArithmetic.c_str();
		break;
	}

	char* pInterp = "";
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		// No interpolation in the pixel shader since we're only passing one value down
		pInterp = "nointerpolation";
	}

	static const char szShaderTemplate[] = 
		"#define TEXCOORDTYPE %s \n"
		"#define INTERSHADERTYPE %s \n"
		"\n"
		"%s"
		"\n"
		"SamplerState MySampler \n"
		"{ \n"
		"    Filter = MIN_MAG_MIP_POINT; \n"
		"    AddressU = Border; \n"
		"    AddressV = Border; \n"
		"    AddressW = Border; \n"
		"}; \n"
		"\n"
		"cbuffer cb  : register(b0)\n"
		"{\n"
		"    TEXCOORDTYPE val0;\n"
		"}\n"
		"struct VSIN\n"
		"{\n"
		"    float2 pos : POS;\n"
		"};\n"
		"\n"
		"struct VSOUT\n"
		"{\n"
		"    float4 pos : SV_Position;\n"
		"    INTERSHADERTYPE data : texcoord;\n"
		"};\n"
		"\n"
		"struct DSOUT\n"
		"{\n"
		"    float4 pos : SV_Position;\n"
		"    INTERSHADERTYPE data : texcoord;\n"
		"};\n"
		"\n"
		"VSOUT VSMain(VSIN v)\n"
		"{\n"
		"    VSOUT ret;\n"
		"\n"
		"    ret.pos = float4(v.pos,0,1);\n"
		"    INTERSHADERTYPE value = INTERSHADERTYPE(0,0,0,0);\n"
		"    %s \n"
		"    ret.data = value;\n"
		"    return ret;\n"
		"}\n"
		"\n"
		"struct PCOUT\n"
		"{\n"
		"    float InsideTF[2] : SV_TessFactor;\n"
		"};\n"
		"\n"
		"PCOUT PCMain(InputPatch<VSOUT, 1> ipatch, OutputPatch<VSOUT, 1> opatch, uint p : SV_PrimitiveID)\n"
		"{\n"
		"    PCOUT ret;\n"
		"    ret.InsideTF = (float[2])(float2(1,1));\n"
		"    return ret;\n"
		"}\n"
		"\n"
		"[patchconstantfunc(\"PCMain\")]\n"
		"[domain(\"isoline\")]\n"
		"[partitioning(\"fractional_odd\")]\n"
		"[outputtopology(\"point\")]\n"
		"[maxtessfactor(1.0)]\n"
		"[outputcontrolpoints(1)]\n"
		"VSOUT HSMain(InputPatch<VSOUT, 1> patch, uint i : SV_OutputControlPointID, uint p : SV_PrimitiveID)\n"
		"{\n"
		"    VSOUT ret;\n"
		"    ret.pos = patch[0].pos;\n"
		"    INTERSHADERTYPE value = patch[0].data;\n"
		"    %s \n"
		"    ret.data = value;\n"
		"    return ret;\n"
		"}\n"
		"\n"
		"[domain(\"isoline\")]\n"
		"DSOUT DSMain(OutputPatch<VSOUT, 1> patch, PCOUT pconst )\n"
		"{\n"
		"    VSOUT ret;\n"
		"    ret.pos = patch[0].pos;\n"
		"    INTERSHADERTYPE value = patch[0].data;\n"
		"    %s \n"
		"    ret.data = value;\n"
		"    return ret;\n"
		"}\n"
		"\n"
		"[maxvertexcount(1)]\n"
		"void GSMain(point DSOUT pts[1], inout PointStream<DSOUT> s)\n"
		"{\n"
		"    DSOUT ret;\n"
		"    ret.pos = pts[0].pos;\n"
		"    INTERSHADERTYPE value = pts[0].data;\n"
		"    %s \n"
		"    ret.data = value;\n"
		"    s.Append(ret);\n"
		"}\n"
		"\n"
		"\n"
		"INTERSHADERTYPE PSMain( %s DSOUT psin) : SV_Target\n"
		"{\n"
		"    INTERSHADERTYPE value = psin.data; \n"
		"    %s \n"
		"    return value;\n"
		"}\n"
		"\n"
		"RWStructuredBuffer<INTERSHADERTYPE> Result : register(u0); \n"
		"[numthreads(1, 1, 1)] \n"
		"void CSMain( uint3 threadID: SV_GroupThreadID )  \n"
		"{  \n"
		"    INTERSHADERTYPE value = INTERSHADERTYPE(0,0,0,0);\n"
		"    %s \n"
		"    Result[0] = value; \n"
		"}\n";

	// Form the actual shader code (by adding a bit of arithmetic to the current shader stage
	static const UINT ArithmeticBuffer = 300;
	static const UINT shaderBufferSize = sizeof(szShaderTemplate) + ArithmeticBuffer;
	char szCurShader[shaderBufferSize];
	int MadeShader = sprintf( szCurShader, szShaderTemplate, sTexcoordType.c_str(), sInterShaderType.c_str(), sDecl.c_str(), pVSCode, pHSCode, pDSCode, pGSCode, pInterp, pPSCode, pCSCode );
	if( MadeShader < 0 )
	{
		LogError( __FILEW__, __LINE__, L"sprintf failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		return false;
	}

	UINT shaderSize = strlen(szCurShader);
	D3D_FEATURE_LEVEL CompileFL = FeatureLevel;	// 9.x VS does not support SRVs
	D3D_FEATURE_LEVEL CompilePSFL = FeatureLevel;

	return CreateShadersFromString( szCurShader, shaderSize, CompileFL, CompilePSFL, Flags, PassThroughShaders );
}

/*************************************************************
* Resource creation methods                                  *
**************************************************************/

void CResourcePrecisionTest::CleanupTestCase()
{
	// Objects are automatically released when CComPtr is set to NULL
	m_pBuf = NULL;
	m_pBufStaging = NULL;
	m_pTex1D = NULL;
	m_pTex1DStaging = NULL;
	m_pTex2D = NULL;
	m_pTex2DStaging = NULL;
	m_pTex3D = NULL;
	m_pTex3DStaging = NULL;
	m_pSRV = NULL;
	m_pUAV = NULL;
	delete m_InitData[0].pSysMem;
	m_InitData[0].pSysMem = NULL;
	m_InitData[1].pSysMem = NULL;  // no need to delete since it was alocated with m_InitData[0].pSysMem
}

TEST_RESULT CResourcePrecisionTest::SetupTestCase()
{
	UINT uSupport = 0;
	HRESULT hr = GetDevice()->CheckFormatSupport( m_Format, &uSupport );
	CFilterHandle skipFormat = FilterEqual( _T("Format"), m_Format );
	if( FAILED(hr) )
	{
		WriteToLog( _T("CheckFormatSupport failed on Format (%s). hr=%s\n"), ToString(m_Format).c_str(), D3DHResultToString(hr).c_str() );
		SkipMatching( GetSkipMatchingFilter() || skipFormat );
		return RESULT_SKIP;
	}

	UINT RequiredBits = 0;
	switch( m_Dimension )
	{
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
	case D3D_SRV_DIMENSION_TEXTURE1D: RequiredBits = D3D11_FORMAT_SUPPORT_TEXTURE1D; break;
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
	case D3D_SRV_DIMENSION_TEXTURE2D: RequiredBits = D3D11_FORMAT_SUPPORT_TEXTURE2D; break;
	case D3D_SRV_DIMENSION_TEXTURE3D: RequiredBits = D3D11_FORMAT_SUPPORT_TEXTURE3D; break;
	}
	if( (uSupport & RequiredBits) != RequiredBits )
	{
		WriteToLog( _T("No support for current view type (%s) for format %s."), ToString(m_Dimension).c_str(), ToString(m_Format).c_str() );
		CFilterHandle skipFilter = FilterEqual( _T("Dimension"), m_Dimension );
		SkipMatching( GetSkipMatchingFilter() || ( skipFormat && skipFilter ) );
		return RESULT_SKIP;
	}

	switch( m_ResOp )
	{
	case RO_LD: RequiredBits = D3D11_FORMAT_SUPPORT_SHADER_LOAD; break;
	case RO_GATHER: RequiredBits = D3D11_FORMAT_SUPPORT_SHADER_GATHER; break;
	case RO_SAMPLE: RequiredBits = D3D11_FORMAT_SUPPORT_SHADER_SAMPLE; break;
	}
	if( (uSupport & RequiredBits) != RequiredBits )
	{
		WriteToLog( _T("No support for current instruction (%s) for format %s."), ToString(m_ResOp).c_str(), ToString(m_Format).c_str() );
		CFilterHandle skipFilter = FilterEqual( _T("Inst"), m_ResOp );
		SkipMatching( GetSkipMatchingFilter() || ( skipFormat && skipFilter ) );
		return RESULT_SKIP;
	}

	switch( m_ViewType )
	{
	case RS_UAV: RequiredBits = D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW; break;
	}
	if( (uSupport & RequiredBits) != RequiredBits )
	{
		WriteToLog( _T("No support for current view type (%s) for format %s."), ToString(m_ViewType).c_str(), ToString(m_Format).c_str() );
		CFilterHandle skipFilter = FilterEqual( _T("ViewType"), m_ViewType );
		SkipMatching( GetSkipMatchingFilter() || ( skipFormat && skipFilter ) );
		return RESULT_SKIP;
	}


	m_Width = 32;

	switch( m_Dimension )
	{
	case D3D_SRV_DIMENSION_BUFFER:
	case D3D_SRV_DIMENSION_TEXTURE1D:
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
		m_Height = 1;
		break;
	case D3D_SRV_DIMENSION_TEXTURE2D:
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
	case D3D_SRV_DIMENSION_TEXTURE2DMS:
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
	case D3D_SRV_DIMENSION_TEXTURE3D:
		m_Height = 32;
		break;
	default:
		assert( false );
	}

	switch( m_Dimension )
	{
	case D3D_SRV_DIMENSION_BUFFER:
	case D3D_SRV_DIMENSION_TEXTURE1D:
	case D3D_SRV_DIMENSION_TEXTURE2D:
	case D3D_SRV_DIMENSION_TEXTURE2DMS:
		m_ArraySizeOrDepth = 1;
		break;
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
		m_ArraySizeOrDepth = 2;
		break;
	case D3D_SRV_DIMENSION_TEXTURE3D:
		m_ArraySizeOrDepth = 4;
		break;
	}
	assert( m_ArraySizeOrDepth <= MAX_DEPTH );

	bool bPassed = InitInitialData();
	if( !bPassed )
	{
		// Error logged in InitInitialData
		return RESULT_FAIL;
	}

	switch( m_Dimension )
	{
	case D3D_SRV_DIMENSION_TEXTURE1D:
	case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
		bPassed = InitTexture1D();
		break;
	case D3D_SRV_DIMENSION_TEXTURE2D:
	case D3D_SRV_DIMENSION_TEXTURE2DMS:
	case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
	case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
		bPassed = InitTexture2D();
		break;
	case D3D_SRV_DIMENSION_TEXTURE3D:
		bPassed = InitTexture3D();
		break;
	case D3D_SRV_DIMENSION_BUFFER:
		bPassed = InitBuffer();
		break;
	}

	if( !bPassed )
	{
		// Error logged in resource Init function
		return RESULT_FAIL;
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

bool CResourcePrecisionTest::CreateResources()
{
	if( !CArithmeticPrecisionTest::CreateResources() )
	{
		return false;
	}

	if( m_pSampler == NULL )
	{
		const float fBorderColor = GetBorderColor( GetDevice()->GetFeatureLevel() );

		D3D11_SAMPLER_DESC sampDesc = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_TEXTURE_ADDRESS_BORDER, 
		                                D3D11_TEXTURE_ADDRESS_BORDER, 0, 1, D3D11_COMPARISON_NEVER, {fBorderColor,fBorderColor,fBorderColor,fBorderColor}, 0, 
										D3D11_FLOAT32_MAX };
		HRESULT hr = GetDevice()->CreateSamplerState( &sampDesc, &m_pSampler );
		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"CreateSamplerState() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
			return false;
		}
	}

	return true;
}

// Fill in initial data for the SRV/UAV resource.  The data will also be used to verify the output of the shader.
bool CResourcePrecisionTest::InitInitialData()
{
	const UINT ElementBitSize = CD3D10FormatHelper_Test::GetBitsPerElement( m_Format );
	assert( ElementBitSize % 8 == 0 );
	const UINT ElementByteSize = ElementBitSize/8;
	const UINT RowSize = ElementByteSize * m_Width;
	const UINT SliceSize = RowSize * m_Height;
	const UINT TotalSize = SliceSize * m_ArraySizeOrDepth;

	assert( m_InitData[0].pSysMem == NULL );
	m_InitData[0].SysMemPitch = RowSize;
	m_InitData[0].SysMemSlicePitch = SliceSize;
	m_InitData[0].pSysMem = new BYTE[TotalSize];
	if( !m_InitData[0].pSysMem )
	{
		LogError( __FILEW__, __LINE__, L"InitialData creation failed.", E_OUTOFMEMORY, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}
	for( UINT i=0; i < MAX_DEPTH; i++ )
	{
		m_InitData[i].SysMemPitch = RowSize;
		m_InitData[i].SysMemSlicePitch = SliceSize;
		m_InitData[i].pSysMem = (BYTE*)m_InitData[0].pSysMem + i * SliceSize;
	}

	// Data:
	//  FLOAT:
	//     8-or-less-bit: 0 to 1 (or 0 and 1 for SRGB)
	//     16+bit: 
	//       8 MP: 0-0.999
	//       10 MP: -1.999-0.999
	//       16+ MP: x:close to 65000, y:0-0.999, z:close to -65000
	//  INT:  x: close to 0, y:close to -MAX_SHORT, z:close to MAX_SHORT
	//  UINT: x: close to 0, y:close to MAX_SHORT, z:close to 100000
	//        Note that there is no ConvertPixel routine for int/UINT so these integers are stored as floats
	//        and converted as needed.

	float bounds[3][2];
	UINT NumComponents = CD3D10FormatHelper_Test::GetNumComponentsInFormat( m_Format );

	for( UINT i = 0; i < min( NumComponents, 3 ); i++ )
	{
		const D3D10_FORMAT_COMPONENT_INTERPRETATION CompType = CD3D10FormatHelper_Test::GetFormatComponentInterpretation( m_Format, i );
	    const UINT CompBits = CD3D10FormatHelper_Test::GetBitsPerComponent( m_Format, i );

		bounds[i][0] = 0;
		bounds[i][1] = 1;
		switch( CompType )
		{
		case D3D10FCI_SNORM:
			bounds[i][0] = -1;
			break;
		case D3D10FCI_UNORM_SRGB:
			switch( i )
			{
			case 0:
				bounds[i][0] = 0;
				bounds[i][1] = 0;
				break;
			case 1:
				bounds[i][0] = 1;
				bounds[i][1] = 1;
				break;
			case 2:
				bounds[i][0] = 0.5;
				bounds[i][1] = 0.5;
				break;
			}
			break;
		case D3D10FCI_UNORM:
			break;
		case D3D10FCI_FLOAT: 
			if( CompBits < 16 )
			{
				break;
			}
			switch( m_DestMinPrecision )
			{
			case D3D_MIN_PRECISION_ANY_10:
				bounds[i][0] = -0.997f;
				bounds[i][1] = 0.999f;
				break;
			case D3D_MIN_PRECISION_ANY_16:
			case D3D_MIN_PRECISION_DEFAULT:
				switch( i )
				{
				case 0:
					bounds[i][0] = 65002.12354f;
					bounds[i][1] = 65402.54333f;
					break;
				case 1:
					bounds[i][1] = 0.987f;
					break;
				case 2:
					bounds[i][0] = -65001.82354f;
					bounds[i][1] = -65401.94333f;
					break;
				}
			}
			break;
		case D3D10FCI_SINT:
			switch( i )
			{
			case 0:
				bounds[i][0] = -90;
				bounds[i][1] = 70;
				break;
			case 1:
				bounds[i][0] = -33000;
				bounds[i][1] = -32000;
				break;
			case 2:
				bounds[i][0] = 32500;
				bounds[i][1] = 32900;
				break;
			}
			break;
		case D3D10FCI_UINT:
			switch( i )
			{
			case 0:
				bounds[i][0] = 0;
				bounds[i][1] = 100;
				break;
			case 1:
				bounds[i][0] = 65400;
				bounds[i][1] = 67000;
				break;
			case 2:
				bounds[i][0] = 100000;
				bounds[i][1] = 120000;
				break;
			}
			break;
		default:
			assert( false );
		}
	}

#if 1
	const float left = bounds[0][0];
	const float right = bounds[0][1];
	const float top = bounds[1][0];
	const float bottom = bounds[1][1];
	const float front = bounds[2][0];
	const float back = bounds[2][1];
#else
	// This is much easier to debug - the texture will contain values from 0 to 1 in each coordinate
	const float left = 0;
	const float right = 1;
	const float top = 0;
	const float bottom = 1;
	const float front = 0;
	const float back = 1;
#endif

	// Note that the x, y, and z values are independant of each other.  This simplifies the SampleTexture, GatherTexture, and 
	// ValidateResultWithTemplates routines (in the latter, the comp loop looks at each component independently).
	for( UINT z = 0; z < m_ArraySizeOrDepth; z++ )
	{
		float zVal = m_ArraySizeOrDepth == 1 ? back : z * back / (m_ArraySizeOrDepth-1) + (m_ArraySizeOrDepth - z) * front / (m_ArraySizeOrDepth-1);
		BYTE* pSliceStart = (BYTE*)m_InitData[0].pSysMem + z * SliceSize;
		assert( pSliceStart == (BYTE*)m_InitData[z].pSysMem );
		for( UINT y = 0; y < m_Height; y++ )
		{
			float yVal = m_Height == 1 ? bottom : y * bottom / (m_Height-1) + (m_Height - y) * top / (m_Height-1);
			BYTE* pRowStart = pSliceStart + y * RowSize;
			for( UINT x = 0; x < m_Width; x++ )
			{
				float xVal = m_Width == 1 ? right : x * right / (m_Width-1) + (m_Width - x) * left / (m_Width-1);
				BYTE* pElementData = pRowStart + x * ElementByteSize;

				FLOAT_COLOR fPixel;
				fPixel.comp[0] = xVal;
				fPixel.comp[1] = yVal;
				fPixel.comp[2] = zVal;
				fPixel.comp[3] = 0.5f;
				bool bConverted = CFormatHelper::ConvertPixels( CONV_TO_FORMAT, 
								   m_Format, 
								   &fPixel, 
								   1, 
								   1, 
								   1, 
								   pElementData );
				assert( bConverted );
				if ( !bConverted )
				{
					LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
					return false;
				}
			}
		}
	}

	return true;
}

bool CResourcePrecisionTest::InitBuffer()
{
	assert( m_pBuf == NULL );
	assert( m_pBufStaging == NULL );
	assert( m_pSRV == NULL );

	UINT ElementBitWidth = CD3D10FormatHelper_Test::GetBitsPerElement( m_Format );
	assert( ElementBitWidth % 8 == 0 );

	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth           = m_Width * ElementBitWidth / 8;
	bufDesc.Usage               = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags           = ( m_ViewType == RS_SRV ) ? D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_UNORDERED_ACCESS;
	bufDesc.CPUAccessFlags      = 0;
	bufDesc.MiscFlags           = 0;
	bufDesc.StructureByteStride = 0;

	assert( m_InitData[0].pSysMem != NULL && m_InitData[1].pSysMem != NULL);
	HRESULT hr = GetDevice()->CreateBuffer( &bufDesc, m_InitData, &m_pBuf );
	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"CreateBuffer() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}

	bufDesc.BindFlags = 0;
	bufDesc.Usage = D3D11_USAGE_STAGING;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pBufStaging );
	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"CreateBuffer() failed for staging.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}

	if( m_ViewType == RS_SRV )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format              = m_Format;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER; 
		srvDesc.Buffer.NumElements  = m_Width;

		hr = GetDevice()->CreateShaderResourceView( m_pBuf, &srvDesc, &m_pSRV );
		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"CreateShaderResourceView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}
	else
	{
		CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC( D3D11_UAV_DIMENSION_BUFFER, m_Format, 0, m_Width );

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pBuf, &uavDesc, &m_pUAV ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}

	return true;
}

bool CResourcePrecisionTest::InitTexture1D()
{
	assert( m_pTex1D == NULL );
	assert( m_pTex1DStaging == NULL );
	assert( m_pSRV == NULL );

	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width              = m_Width;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = m_ArraySizeOrDepth;
	texDesc.Format             = m_Format;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = ( m_ViewType == RS_SRV ) ? D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	assert( m_InitData[0].pSysMem != NULL && m_InitData[1].pSysMem != NULL);
	HRESULT hr = GetDevice()->CreateTexture1D( &texDesc, m_InitData, &m_pTex1D );
	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"CreateTexture1D() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}

	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = GetDevice()->CreateTexture1D( &texDesc, NULL, &m_pTex1DStaging );
	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"CreateTexture1D() failed for staging.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format              = m_Format;
	
	if( m_ViewType == RS_SRV )
	{
		if( m_Dimension == D3D_SRV_DIMENSION_TEXTURE1D )
		{
			srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MipLevels       = 1;
			srvDesc.Texture1D.MostDetailedMip = 0;
		}
		else
		{
			srvDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			srvDesc.Texture1DArray.ArraySize       = m_ArraySizeOrDepth;
			srvDesc.Texture1DArray.FirstArraySlice = 0;
			srvDesc.Texture1DArray.MipLevels       = 1;
			srvDesc.Texture1DArray.MostDetailedMip = 0;
		}

		hr = GetDevice()->CreateShaderResourceView( m_pTex1D, &srvDesc, &m_pSRV );
		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"CreateShaderResourceView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}
	else
	{
		D3D11_UAV_DIMENSION dim = m_Dimension == D3D_SRV_DIMENSION_TEXTURE1D ? D3D11_UAV_DIMENSION_TEXTURE1D : D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
		CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC( dim, m_Format );

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pTex1D, &uavDesc, &m_pUAV ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}

	return true;
}

bool CResourcePrecisionTest::InitTexture2D()
{
	assert( m_pTex2D == NULL );
	assert( m_pTex2DStaging == NULL );
	assert( m_pSRV == NULL );

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width              = m_Width;
	texDesc.Height             = m_Height;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = m_ArraySizeOrDepth;
	texDesc.Format             = m_Format;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = ( m_ViewType == RS_SRV ) ? D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	assert( m_InitData[0].pSysMem != NULL && m_InitData[1].pSysMem != NULL);
	HRESULT hr = GetDevice()->CreateTexture2D( &texDesc, m_InitData, &m_pTex2D );
	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}

	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = GetDevice()->CreateTexture2D( &texDesc, NULL, &m_pTex2DStaging );
	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"CreateTexture2D() failed for staging.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}

	if( m_ViewType == RS_SRV )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format              = m_Format;
		
		if( m_Dimension == D3D_SRV_DIMENSION_TEXTURE2D )
		{
			srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels       = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
		}
		else if( m_Dimension == D3D_SRV_DIMENSION_TEXTURE2DARRAY )
		{
			srvDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize       = m_ArraySizeOrDepth;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.MipLevels       = 1;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
		}
		else if( m_Dimension == D3D_SRV_DIMENSION_TEXTURE2DMS )
		{
			srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		else if( m_Dimension == D3D_SRV_DIMENSION_TEXTURE2DMSARRAY )
		{
			srvDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srvDesc.Texture2DMSArray.ArraySize       = m_ArraySizeOrDepth;
			srvDesc.Texture2DMSArray.FirstArraySlice = 0;
		}
		else
		{
			assert( false );
		}

		hr = GetDevice()->CreateShaderResourceView( m_pTex2D, &srvDesc, &m_pSRV );
		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"CreateShaderResourceView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}
	else
	{
		D3D11_UAV_DIMENSION dim = m_Dimension == D3D_SRV_DIMENSION_TEXTURE2D ? D3D11_UAV_DIMENSION_TEXTURE2D : D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC( dim, m_Format );

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pTex2D, &uavDesc, &m_pUAV ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}

	return true;
}

bool CResourcePrecisionTest::InitTexture3D()
{
	assert( m_pTex3D == NULL );
	assert( m_pTex3DStaging == NULL );
	assert( m_pSRV == NULL );

	D3D11_TEXTURE3D_DESC texDesc;
	texDesc.Width              = m_Width;
	texDesc.Height             = m_Height;
	texDesc.Depth			   = m_ArraySizeOrDepth;
	texDesc.MipLevels          = 1;
	texDesc.Format             = m_Format;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = ( m_ViewType == RS_SRV ) ? D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	assert( m_InitData[0].pSysMem != NULL && m_InitData[1].pSysMem != NULL);
	HRESULT hr = GetDevice()->CreateTexture3D( &texDesc, m_InitData, &m_pTex3D );
	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"CreateTexture3D() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}

	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = GetDevice()->CreateTexture3D( &texDesc, NULL, &m_pTex3DStaging );
	if (FAILED(hr))
	{
		LogError( __FILEW__, __LINE__, L"CreateTexture3D() failed for staging.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateResource );
		return false;
	}

	if( m_ViewType == RS_SRV )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format              = m_Format;
		srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels       = 1;
		srvDesc.Texture3D.MostDetailedMip = 0;

		hr = GetDevice()->CreateShaderResourceView( m_pTex3D, &srvDesc, &m_pSRV );
		if (FAILED(hr))
		{
			LogError( __FILEW__, __LINE__, L"CreateShaderResourceView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}
	else
	{
		CD3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC( D3D11_UAV_DIMENSION_TEXTURE3D, m_Format );

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pTex3D, &uavDesc, &m_pUAV ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CreateUnorderedAccessView() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCreateView );
			return false;
		}
	}

	return true;
}

/*************************************************************
* Other test case execution methods                          *
**************************************************************/

DXGI_FORMAT CResourcePrecisionTest::GetRenderTargetFormat()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();
	const EValueType ValueType = GetShaderOperationDestValueType();

	// Determine RT format
	DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		switch( ValueType )
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
		assert( ValueType == VT_Float );
	}

	return format;
}

bool CResourcePrecisionTest::IsLoadOperation()
{
	return m_ResOp == RO_LD || m_ResOp == RO_LD2DMS || m_ResOp == RO_LDUAVTYPED;
}

bool CResourcePrecisionTest::IsResinfoUintOperation()
{
	return m_ResOp == RO_BUFINFO || m_ResOp == RO_RESINFO;
}

bool CResourcePrecisionTest::IsResinfoFloatOperation()
{
	return m_ResOp == RO_RESINFO_FLOAT;
}

// Returns a pointer to the cbuffer data (the texcoord or ldcoord) for the current test case
void* CResourcePrecisionTest::GetSrcData()
{
	if( IsLoadOperation() )
		return reinterpret_cast<void*>(&m_Ldcoord[0]);
	else		
		return reinterpret_cast<void*>(&m_Texcoord[0]);
}

UINT CResourcePrecisionTest::GetSrcDataSize()
{
	if( IsLoadOperation() )
		return sizeof(m_Ldcoord);
	else		
		return sizeof(m_Texcoord);
}

TEST_RESULT CResourcePrecisionTest::ExecuteTestCase()
{
	HRESULT hr;
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	{
		assert( m_pCB != NULL );
		void* pSrc = GetSrcData();
		BYTE temp[m_CBSize];
		memcpy(temp, pSrc, GetSrcDataSize()); // pSrc may not be as big as the CB
		assert(sizeof(temp) >= GetSrcDataSize());
		GetEffectiveContext()->UpdateSubresource( m_pCB, 0, NULL, temp, 0, 0 );

		ID3D11UnorderedAccessView* ppNULLUAV[] = {NULL, NULL};
		ID3D11ShaderResourceView* ppNULLSRV[] = {NULL};
		ID3D11Buffer* ppNULLBuff[] = {NULL};
		ID3D11RenderTargetView* ppNULLRTV[] = {NULL};
		ID3D11SamplerState* ppNULLSamp[] = {NULL};

		assert( m_pSampler != NULL );
		assert( m_pSRV != NULL || m_pUAV != NULL );

		if( m_ShaderStage == D3D_SVT_COMPUTESHADER )
		{
			assert( m_pCS != NULL );
			assert( m_pBufUAV != NULL );

			GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, &m_pBufUAV, NULL );
			if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
			{
				GetEffectiveContext()->CSSetUnorderedAccessViews( 1, 1, &m_pUAV, NULL );
			}
			GetEffectiveContext()->CSSetShaderResources( 0, 1, &m_pSRV );
			GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->CSSetSamplers( 0, 1, &m_pSampler );
			GetEffectiveContext()->CSSetShader( m_pCS, NULL, 0 );

			GetEffectiveContext()->Dispatch( 1, 1, 1 );

			GetEffectiveContext()->CSSetUnorderedAccessViews( 0, 1, ppNULLUAV, NULL );
			if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
			{
				GetEffectiveContext()->CSSetUnorderedAccessViews( 1, 1, ppNULLUAV, NULL );
			}
			GetEffectiveContext()->CSSetShaderResources( 0, 1, ppNULLSRV );
			GetEffectiveContext()->CSSetConstantBuffers( 0, 1, ppNULLBuff );
			GetEffectiveContext()->CSSetSamplers( 0, 1, ppNULLSamp );
			GetEffectiveContext()->CSSetShader( NULL, NULL, 0 );
		}
		else
		{
			UINT Stride = sizeof(Vertex9);
			UINT Offset = 0;

			D3D11_VIEWPORT vp = { 0,0, m_RTWidth,m_RTHeight, 0, 1 };

			assert( m_pIL != NULL );
			assert( m_pVB != NULL );
			assert( m_pVS != NULL );
			if( m_ShaderStage == D3D_SVT_HULLSHADER || m_ShaderStage == D3D_SVT_DOMAINSHADER )
			{
				assert( m_pHS != NULL );
				assert( m_pDS != NULL );
			}
			if( m_ShaderStage == D3D_SVT_GEOMETRYSHADER )
			{
				assert( m_pGS != NULL );
			}
			assert( m_pPS != NULL );
			assert( m_pRTV != NULL );
			assert( m_pRS != NULL );

			FLOAT clearColor[4] = {0};
			GetEffectiveContext()->ClearRenderTargetView( m_pRTV, clearColor );

			GetEffectiveContext()->IASetInputLayout( m_pIL );
			GetEffectiveContext()->IASetPrimitiveTopology( m_pHS == NULL ? D3D11_PRIMITIVE_TOPOLOGY_POINTLIST : D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
			GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVB, &Stride, &Offset );
			GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
			GetEffectiveContext()->PSSetShader( m_pPS, NULL, 0 );
			GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &m_pCB );
			GetEffectiveContext()->PSSetShaderResources( 0, 1, &m_pSRV );
			GetEffectiveContext()->PSSetSamplers( 0, 1, &m_pSampler );
			if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
			{
				GetEffectiveContext()->HSSetShader( m_pHS, NULL, 0 );
				GetEffectiveContext()->DSSetShader( m_pDS, NULL, 0 );
				GetEffectiveContext()->HSSetConstantBuffers( 0, 1, &m_pCB );
				GetEffectiveContext()->DSSetConstantBuffers( 0, 1, &m_pCB );
				GetEffectiveContext()->HSSetShaderResources( 0, 1, &m_pSRV );
				GetEffectiveContext()->DSSetShaderResources( 0, 1, &m_pSRV );
				GetEffectiveContext()->HSSetSamplers( 0, 1, &m_pSampler );
				GetEffectiveContext()->DSSetSamplers( 0, 1, &m_pSampler );
			}
			if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
			{
				// VS 9.x does not support sampling
				GetEffectiveContext()->VSSetShaderResources( 0, 1, &m_pSRV );
				GetEffectiveContext()->VSSetSamplers( 0, 1, &m_pSampler );
				GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
				GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &m_pCB );
				GetEffectiveContext()->GSSetShaderResources( 0, 1, &m_pSRV );
				GetEffectiveContext()->GSSetSamplers( 0, 1, &m_pSampler );
			}
			GetEffectiveContext()->OMSetRenderTargets( 1, &m_pRTV, NULL );
			GetEffectiveContext()->RSSetState( m_pRS );
			GetEffectiveContext()->RSSetViewports( 1, &vp );

			GetEffectiveContext()->Draw( 1, 0 );

			// reset the context

			GetEffectiveContext()->IASetInputLayout( NULL );
			GetEffectiveContext()->IASetVertexBuffers( 0, 1, ppNULLBuff, &Stride, &Offset );
			GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
			GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
			GetEffectiveContext()->VSSetConstantBuffers( 0, 1, ppNULLBuff );
			GetEffectiveContext()->PSSetConstantBuffers( 0, 1, ppNULLBuff );
			GetEffectiveContext()->PSSetShaderResources( 0, 1, ppNULLSRV );
			GetEffectiveContext()->PSSetSamplers( 0, 1, ppNULLSamp );
			if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
			{
				GetEffectiveContext()->HSSetShader( NULL, NULL, 0 );
				GetEffectiveContext()->DSSetShader( NULL, NULL, 0 );
				GetEffectiveContext()->HSSetConstantBuffers( 0, 1, ppNULLBuff );
				GetEffectiveContext()->DSSetConstantBuffers( 0, 1, ppNULLBuff );
				GetEffectiveContext()->HSSetShaderResources( 0, 1, ppNULLSRV );
				GetEffectiveContext()->DSSetShaderResources( 0, 1, ppNULLSRV );
				GetEffectiveContext()->HSSetSamplers( 0, 1, ppNULLSamp );
				GetEffectiveContext()->DSSetSamplers( 0, 1, ppNULLSamp );
			}
			if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
			{
				GetEffectiveContext()->VSSetShaderResources( 0, 1, ppNULLSRV );
				GetEffectiveContext()->VSSetSamplers( 0, 1, ppNULLSamp );
				GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
				GetEffectiveContext()->GSSetConstantBuffers( 0, 1, ppNULLBuff );
				GetEffectiveContext()->GSSetShaderResources( 0, 1, ppNULLSRV );
				GetEffectiveContext()->GSSetSamplers( 0, 1, ppNULLSamp );
			}
			GetEffectiveContext()->RSSetState( NULL );
		}

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

/*************************************************************
* Test Evaluation methods                                    *
**************************************************************/

template< class TCoord >
CMinPrecValue<int> ConvertCoordToInt( CMinPrecValue<TCoord>& x, UINT Range, bool bFL9 );


// Determine if the coordinate lies within the texture
template<>
CMinPrecValue<int> ConvertCoordToInt<UINT>( CMinPrecValue<UINT>& x, UINT Range, bool bFL9 )
{
	CMinPrecValue<int> R;


	for( UINT i = 0; i < x.m_Values.size(); i++ )
	{
		UINT val = x.m_Values[i];
		if( Range == 1 )
		{
			// If this coordinate is not used, force to 0
			R.m_Values.push_back( 0 );
		}
		else if( val >= Range )
		{
			// Border
			R.m_Values.push_back( -1 );
		}
		else
			R.m_Values.push_back( (int)val );
	}
	R.Uniquify();
	return R;
}

// Convert a set of floating point coordinates (between 0 and 1) to integer texture coordinates (between 0 and Range-1)
template<>
CMinPrecValue<int> ConvertCoordToInt<float>( CMinPrecValue<float>& x, UINT Range, bool bFL9 )
{
	CMinPrecValue<int> R;

	// coordinates are accurate to 1/256 of a pixel for 10+ and 1/16 for 9.x
	float fuzz = bFL9 ? 1.0f / 16.0f : 1.0f / 256.0f;

	for( UINT i = 0; i < x.m_Values.size(); i++ )
	{
		float normVal = x.m_Values[i];
		float normFrac = normVal - floor(normVal);
		float val = normVal * Range;
		float xFrac = val - floor(val);
		if( Range < 3 )
		{
			// If this coordinate is not used, force to 0
			// If this coordinate is an array index, round and clamp
			if( normVal <= 0.5f + fuzz )
			{
				// round down
				R.m_Values.push_back( static_cast<int>( max( 0, min( Range-1, floor(normVal) ) ) ) );
			}
			if( normVal > 0.5f - fuzz )
			{
				// round up
				R.m_Values.push_back( static_cast<int>( max( 0, min( Range-1, floor(normVal+0.8f) ) ) ) );
			}
		}
		else 
		{
			if( val <= fuzz || val >= Range - fuzz )
			{
				// border color
				R.m_Values.push_back( -1 );
			}
			if( val >= -fuzz && val < Range + fuzz )
			{
				if( xFrac <= fuzz && val > 0 )
				{
					// If value is on edge of two pixels, add the left neighbour
					R.m_Values.push_back( static_cast<int>(floor(val))-1 );
				}
				if( val < Range && val >= 0 )
				{
					// Add main pixel
					R.m_Values.push_back( static_cast<int>(floor(val)) );
				}
				if( xFrac >= 1.0f - fuzz && val < Range - 1 )
				{
					// If value is on edge of two pixels, add the right neighbour
					R.m_Values.push_back( static_cast<int>(floor(val))+1 );
				}
			}
		}
	}
	R.Uniquify();
	return R;
}

// Perform a point sample operation at each {x,y,z}
template< class TDest, class TCoord >
bool CResourcePrecisionTest::SampleTexture( CMinPrecValue<TCoord>& x, CMinPrecValue<TCoord>& y, CMinPrecValue<TCoord>& z, CMinPrecValue<TDest> ReadBack[4] )
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	const UINT ElementBitSize = CD3D10FormatHelper_Test::GetBitsPerElement( m_Format );
	assert( ElementBitSize % 8 == 0 );
	const UINT ElementByteSize = ElementBitSize/8;
	const UINT RowSize = ElementByteSize * m_Width;
	const UINT SliceSize = RowSize * m_Height;
	const UINT TotalSize = SliceSize * m_ArraySizeOrDepth;

	// Note the special definition of Z below.  For sampling from 1D texture arrays, the y-coord is the array index.  
	// This code forces Z to contain the array index.
	bool bZasY = m_Dimension == D3D_SRV_DIMENSION_TEXTURE1DARRAY && !IsLoadOperation();

	CMinPrecValue<int>& X = ConvertCoordToInt<TCoord>( x, m_Width, FeatureLevel < D3D_FEATURE_LEVEL_10_0 );
	CMinPrecValue<int>& Y = ConvertCoordToInt<TCoord>( y, m_Height, FeatureLevel < D3D_FEATURE_LEVEL_10_0 );
	CMinPrecValue<int>& Z = ConvertCoordToInt<TCoord>( bZasY ? y : z, m_ArraySizeOrDepth, FeatureLevel < D3D_FEATURE_LEVEL_10_0 );

	bool bFoundBorder = false;
	for( UINT i = 0; i < X.m_Values.size(); i++ )
	{
		int xVal = X.m_Values[i];
		if( xVal == -1 )
		{
			bFoundBorder = true;
			continue;
		}
		for( UINT j = 0; j < Y.m_Values.size(); j++ )
		{
			int yVal = Y.m_Values[j];
			if( yVal == -1 )
			{
				bFoundBorder = true;
				continue;
			}
			for( UINT k = 0; k < Z.m_Values.size(); k++ )
			{
				int zVal = Z.m_Values[k];
				if( zVal == -1 )
				{
					bFoundBorder = true;
					continue;
				}

				assert( xVal >= 0 && xVal < (int)m_Width );
				assert( yVal >= 0 && yVal < (int)m_Height );
				assert( zVal >= 0 && zVal < (int)m_ArraySizeOrDepth );

				// Convert coordinate to data pointer
				BYTE* pElementData = (BYTE*)m_InitData[0].pSysMem + zVal * SliceSize + yVal * RowSize + xVal * ElementByteSize;

				// Perform the sample
				FLOAT_COLOR fPixel;
				bool bConverted = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
								   m_Format, 
								   &fPixel, 
								   1, 
								   1, 
								   1, 
								   pElementData );
				assert( bConverted );
				if ( !bConverted )
				{
					LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
					return false;
				}

				for( UINT comp = 0; comp < 4; comp++ )
				{
					ReadBack[comp].m_Values.push_back( static_cast<TDest>( fPixel.comp[comp] ) );
				}
			}
		}
	}

	// Add border color if we might have sampled off the texture
	if( bFoundBorder )
	{
		for( UINT comp = 0; comp < 4; comp++ )
		{
			if( IsLoadOperation() )
			{
				ReadBack[comp].m_Values.push_back( static_cast<TDest>( 0 ) );
			}
			else
			{
				ReadBack[comp].m_Values.push_back( static_cast<TDest>( GetBorderColor( FeatureLevel ) ) );
			}
		}
	}

	return true;
}

// Perform a gather operation of the x-coordinate at each {x,y,z}
template< class TDest, class TCoord >
bool CResourcePrecisionTest::GatherTexture( CMinPrecValue<TCoord>& x, CMinPrecValue<TCoord>& y, CMinPrecValue<TCoord>& z, CMinPrecValue<TDest> ReadBack[4] )
{
	// We are only interested in the X component (because the shader uses component 0 for gather)

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	const UINT ElementBitSize = CD3D10FormatHelper_Test::GetBitsPerElement( m_Format );
	assert( ElementBitSize % 8 == 0 );
	const UINT ElementByteSize = ElementBitSize/8;
	const UINT RowSize = ElementByteSize * m_Width;
	const UINT SliceSize = RowSize * m_Height;
	const UINT TotalSize = SliceSize * m_ArraySizeOrDepth;

	// Note the special definition of Z below.  For sampling from 1D texture arrays, the y-coord is the array index.  
	// This code forces Z to contain the array index.
	bool bZasY = m_Dimension == D3D_SRV_DIMENSION_TEXTURE1DARRAY && !IsLoadOperation();

	CMinPrecValue<int>& Y = ConvertCoordToInt<TCoord>( y, m_Height, FeatureLevel < D3D_FEATURE_LEVEL_10_0 );
	CMinPrecValue<int>& Z = ConvertCoordToInt<TCoord>( bZasY ? y : z, m_ArraySizeOrDepth, FeatureLevel < D3D_FEATURE_LEVEL_10_0 );

	bool bFoundBorder = false;

	// Check for out-of-bounds in Y and Z components
	for( UINT i = 0; i < Y.m_Values.size(); i++ )
	{
		int yVal = Y.m_Values[i];
		if( yVal == -1 )
		{
			bFoundBorder = true;
			break;;
		}
	}
	for( UINT i = 0; i < Z.m_Values.size(); i++ )
	{
		int zVal = Z.m_Values[i];
		if( zVal == -1 )
		{
			bFoundBorder = true;
			break;;
		}
	}

	for( UINT i=0; i < x.m_Values.size(); i++ )
	{
		float xVal = static_cast<float>( x.m_Values[i] * m_Width );
		int xFloor = static_cast<int>( floor(xVal) );
		float xFrac = xVal - floor(xVal);

		std::vector<int> left;
		std::vector<int> right;

		if( xFloor < 0 || xFloor == m_Width && xFrac > 0 )
		{
			// outside of texture
			bFoundBorder = true;
			continue;
		}
		if( xFrac == 0.5 )
		{
			// all in one pixel
			left.push_back( xFloor );
			right.push_back( xFloor );

			// left range
			if( xFloor > 0 )
			{
				left.push_back( xFloor - 1 );
				right.push_back( xFloor );
			}

			// right range
			if( xFloor < (int)m_Width - 1 )
			{
				left.push_back( xFloor );
				right.push_back( xFloor + 1 );
			}
		}
		else if( xFrac < 0.5 )
		{
			if( xFloor == 0 )
			{
				// all in one pixel at left
				left.push_back( xFloor );
				right.push_back( xFloor );
			}
			else if( xFloor == m_Width )
			{
				// all in one pixel at right (touches border)
				assert( xFrac == 0 );
				left.push_back( xFloor - 1 );
				right.push_back( xFloor - 1);
			}
			else
			{
				left.push_back( xFloor - 1);
				right.push_back( xFloor );
			}
		}
		else
		{
			// all in one pixel
			if( xFloor == m_Width-1 )
			{
				left.push_back( xFloor );
				right.push_back( xFloor );
			}
			else
			{
				left.push_back( xFloor );
				right.push_back( xFloor + 1 );
			}
		}

		assert( left.size() == right.size() );
		for( UINT j=0; j < left.size(); j++ )
		{
			// Gether will return the four pixel values, but two are the same since the data in each component is independent

			// Read the left pixel
			BYTE* pElementData = (BYTE*)m_InitData[0].pSysMem + left[0] * ElementByteSize;
			FLOAT_COLOR fPixel;
			bool bConverted = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
							   m_Format, 
							   &fPixel, 
							   1, 
							   1, 
							   1, 
							   pElementData );
			assert( bConverted );
			if ( !bConverted )
			{
				LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
				return false;
			}
			ReadBack[0].m_Values.push_back( static_cast<TDest>( fPixel.comp[0] ) );
			ReadBack[3].m_Values.push_back( static_cast<TDest>( fPixel.comp[0] ) );

			// Read the right pixel
			pElementData = (BYTE*)m_InitData[0].pSysMem + right[0] * ElementByteSize;
			bConverted = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
							   m_Format, 
							   &fPixel, 
							   1, 
							   1, 
							   1, 
							   pElementData );
			assert( bConverted );
			if ( !bConverted )
			{
				LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
				return false;
			}
			ReadBack[1].m_Values.push_back( static_cast<TDest>( fPixel.comp[0] ) );
			ReadBack[2].m_Values.push_back( static_cast<TDest>( fPixel.comp[0] ) );
		}
	}

	if( bFoundBorder )
	{
		for( UINT comp = 0; comp < 4; comp++ )
		{
			ReadBack[comp].m_Values.push_back( static_cast<TDest>( GetBorderColor( FeatureLevel ) ) );
		}
	}

	return true;
}

template< class TDest, class TCoord >
bool CResourcePrecisionTest::PerformOperation( CMinPrecValue<TCoord>& x, CMinPrecValue<TCoord>& y, CMinPrecValue<TCoord>& z, CMinPrecValue<TDest> Result[4] )
{
	if( IsResinfoUintOperation() || IsResinfoFloatOperation() )
	{
		switch( m_Dimension )
		{
		case D3D_SRV_DIMENSION_BUFFER:
		case D3D_SRV_DIMENSION_TEXTURE1D:
			Result[0].m_Values.push_back( static_cast<TDest>( m_Width ) );
			break;
		case D3D_SRV_DIMENSION_TEXTURE1DARRAY:
			Result[0].m_Values.push_back( static_cast<TDest>( m_Width ) );
			Result[1].m_Values.push_back( static_cast<TDest>( m_ArraySizeOrDepth ) );
			break;
		case D3D_SRV_DIMENSION_TEXTURE2D:
			Result[0].m_Values.push_back( static_cast<TDest>( m_Width ) );
			Result[1].m_Values.push_back( static_cast<TDest>( m_Height ) );
			break;
		case D3D_SRV_DIMENSION_TEXTURE2DMS:
			Result[0].m_Values.push_back( static_cast<TDest>( m_Width ) );
			Result[1].m_Values.push_back( static_cast<TDest>( m_Height ) );
			Result[2].m_Values.push_back( 1 ); // sample count
			break;
		case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
		case D3D_SRV_DIMENSION_TEXTURE3D:
			Result[0].m_Values.push_back( static_cast<TDest>( m_Width ) );
			Result[1].m_Values.push_back( static_cast<TDest>( m_Height ) );
			Result[2].m_Values.push_back( static_cast<TDest>( m_ArraySizeOrDepth ) );
			break;
		case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY:
			Result[0].m_Values.push_back( static_cast<TDest>( m_Width ) );
			Result[1].m_Values.push_back( static_cast<TDest>( m_Height ) );
			Result[2].m_Values.push_back( static_cast<TDest>( m_ArraySizeOrDepth ) );
			Result[3].m_Values.push_back( 1 ); // sample count
			break;
		}

		// Fill the rest with 0
		for( UINT i=0; i < 4; i++ )
		{
			if( Result[i].m_Values.size() == 0 )
			{
				Result[i].m_Values.push_back( 0 );
			}
		}
	}
	else if( m_ResOp != RO_GATHER )
	{
		if ( !SampleTexture( x, y, z, Result ) )
		{
			return false;
		}
	}
	else
	{
		if ( !GatherTexture( x, y, z, Result ) )
		{
			return false;
		}
	}

	return true;
}

TEST_RESULT CResourcePrecisionTest::ValidateResult()
{
	const EValueType DestValueType = GetInterShaderValueType();
	const bool bUint = IsLoadOperation() || IsResinfoUintOperation();

	switch( DestValueType )
	{
	case VT_Float:
		if( bUint )
			return ValidateResultWithTemplates< float, UINT >();
		else
			return ValidateResultWithTemplates< float, float >();
	case VT_Int:
		if( bUint )
			return ValidateResultWithTemplates< int, UINT >();
		else
			return ValidateResultWithTemplates< int, float >();
	case VT_Uint:
		if( bUint )
			return ValidateResultWithTemplates< UINT, UINT >();
		else
			return ValidateResultWithTemplates< UINT, float >();
	}

	assert( false );
	return RESULT_FAIL;
}

template< class TDest, class TCoord >
TEST_RESULT CResourcePrecisionTest::ValidateResultWithTemplates()
{
	D3D11_MAPPED_SUBRESOURCE mapTex2D;
	HRESULT hr = S_OK;
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	if( m_ShaderStage != D3D_SVT_COMPUTESHADER )
	{
		GetImmediateContext()->CopyResource( m_pRT2DStaging, m_pRT2D );
	}
	else
	{
		GetImmediateContext()->CopyResource( m_pStructuredStaging, m_pStructuredBuffer );
	}

	// Map the output resource
	CMapHelper mapHelper( GetImmediateContext() );
	if( m_ShaderStage == D3D_SVT_COMPUTESHADER )
	{
		hr = mapHelper.Map( m_pStructuredStaging, 0, D3D11_MAP_READ, 0, &mapTex2D );
	}
	else
	{
		hr = mapHelper.Map( m_pRT2DStaging, 0, D3D11_MAP_READ, 0, &mapTex2D );
	}
	if( FAILED( hr ) )
	{
		LogError( __FILEW__, __LINE__, L"m_pRT2DStaging Map() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceContextMap );
		return RESULT_FAIL;
	}

	D3D11_TEXTURE2D_DESC tex2DDesc;
	m_pRT2DStaging->GetDesc( &tex2DDesc );

	EPrecisionLevel MinPrecisionCheck = CMinPrecValue<float>::ConvertToPrecisionLevel( m_DestMinPrecision );
	const EValueType DestValueType = GetInterShaderValueType();
	// In 9.x, only 16 bit floats are outputted, so make sure we don't expect more precision than 16 bits
	if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 && DestValueType == VT_Float && MinPrecisionCheck == EPL_32 )
	{
		assert( tex2DDesc.Format == DXGI_FORMAT_R16G16B16A16_FLOAT );
		MinPrecisionCheck = EPL_16;
	}

	// Get the result
	Vec4<TDest> Result;
	if( tex2DDesc.Format == DXGI_FORMAT_R16G16B16A16_FLOAT )
	{
		// convert 16-bit to 32-bit answers
		FLOAT_COLOR fPixel;
		bool bConverted = CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, 
						   tex2DDesc.Format, 
						   &fPixel, 
						   1, 
						   1, 
						   1, 
						   mapTex2D.pData );
		if ( !bConverted )
		{
			LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters );
			return RESULT_FAIL;
		}
		memcpy( Result.values, &fPixel, sizeof(Vec4<TDest>) );
	}
	else
	{
		memcpy( Result.values, mapTex2D.pData, sizeof(Vec4<TDest>) );
	}

	// We do not loop through data below.
	assert( m_RTWidth == 1 && m_RTHeight == 1 );

	const UINT NumComponents = CD3D10FormatHelper_Test::GetNumComponentsInFormat( m_Format );

	// Generate the expected value
	CMinPrecValue<TCoord> x( reinterpret_cast<Vec4<TCoord>*>(GetSrcData())[0][0] );
	CMinPrecValue<TCoord> y( reinterpret_cast<Vec4<TCoord>*>(GetSrcData())[0][1] );
	CMinPrecValue<TCoord> z( reinterpret_cast<Vec4<TCoord>*>(GetSrcData())[0][2] );
	x.EnsureMinPrecisionLevel( CMinPrecValue<float>::ConvertToPrecisionLevel( m_MinPrecision ), FeatureLevel );
	y.EnsureMinPrecisionLevel( CMinPrecValue<float>::ConvertToPrecisionLevel( m_MinPrecision ), FeatureLevel );
	z.EnsureMinPrecisionLevel( CMinPrecValue<float>::ConvertToPrecisionLevel( m_MinPrecision ), FeatureLevel );

	assert( NumComponents <= 4 );
	CMinPrecValue<TDest> expected[4];
	if ( !PerformOperation<TDest,TCoord>( x, y, z, expected ) )
	{
		LogError(__FILEW__, __LINE__, L"PerformOperation failed.", false, ERRORCODE_TYPE_BOOL, gFCConflictingParameters);
		return RESULT_FAIL;
	}

	// SRGB precision is very loosely defined.
	float ULPMultiplier = CD3D10FormatHelper_Test::IsSRGBFormat( m_Format ) ? 10.0f : 1.0f;

	for( UINT comp = 0; comp < NumComponents; comp++ )
	{
		expected[comp].EnsureMinPrecisionLevel( MinPrecisionCheck, FeatureLevel );

		const D3D10_FORMAT_COMPONENT_INTERPRETATION CompType = CD3D10FormatHelper_Test::GetFormatComponentInterpretation( m_Format, comp );
	    const UINT CompBits = CD3D10FormatHelper_Test::GetBitsPerComponent( m_Format, comp );
		if( CompBits <= 8 || CompType == D3D10FCI_SNORM || CompType == D3D10FCI_UNORM )
		{
			MinPrecisionCheck = EPL_8;
		}

		if( !expected[comp].Matches( Result[comp], MinPrecisionCheck, FeatureLevel, false, ULPMultiplier ) )
		{
			// For SRGB, since it is so loosely for 9.x, only check 0.0f and 1.0f
			if( CompType != D3D10FCI_UNORM_SRGB || FeatureLevel > D3D_FEATURE_LEVEL_10_0 || ( expected[comp].m_Values.size() == 1 && ( expected[comp].m_Values[0] == 0 || expected[comp].m_Values[0] == 1 ) ) )
			{
				WriteActualResultAndExpectedResultsToTheLog<TDest>( 0, comp, Result[comp], expected[comp] );
				LogError( __FILEW__, __LINE__, L"Result not within expected ranges.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
				return RESULT_FAIL;
			}
		}
	}

	return RESULT_PASS;
}

