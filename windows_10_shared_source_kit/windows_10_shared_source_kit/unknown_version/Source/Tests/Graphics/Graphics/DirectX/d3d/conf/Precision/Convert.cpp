#include "Convert.h"

//---------------------------------------------------------------


BEGIN_NAMED_VALUES( CONV_OP )
	NAMED_VALUE( _T( "f32tof16" ), CO_F32TOF16 )
	NAMED_VALUE( _T( "f16tof32" ), CO_F16TOF32 )
	NAMED_VALUE( _T( "ftoi" ), CO_FTOI )
	NAMED_VALUE( _T( "ftou" ), CO_FTOU )
	NAMED_VALUE( _T( "itof" ), CO_ITOF )
	NAMED_VALUE( _T( "utof" ), CO_UTOF )
	NAMED_VALUE( _T( "dtof" ), CO_DTOF )
	NAMED_VALUE( _T( "ftod" ), CO_FTOD )
END_NAMED_VALUES( CONV_OP )



template< class TDest, class TSrc >
CMinPrecValue<TDest> Convert( const CMinPrecValue<TSrc> & A )
{
	CMinPrecValue< TDest > R;
	for( UINT i=0; i < A.m_Values.size(); i++ )
	{
		TSrc v = A.m_Values[i];
		R.m_Values.push_back( static_cast<TDest>( v ) );
	}
	R.m_Levels = EPL_32;

	R.Uniquify();

	return R;
}
template<>
CMinPrecValue<UINT> Convert( const CMinPrecValue<float> & A )
{
	CMinPrecValue<UINT> R;
	for( UINT i=0; i < A.m_Values.size(); i++ )
	{
		// Deal with float-UINT conversion specially, since negative floats convert to 0, but static cast does not
		// INF should convert to 0xffffffff but static cast converts it to 0
		float v = max( 0, A.m_Values[i] );
		UINT r = static_cast<UINT>( v );
		R.m_Values.push_back( r == 0 && v > 1 ? 0xffffffff : r );
	}
	R.m_Levels = EPL_32;

	R.Uniquify();

	return R;
}
template<>
CMinPrecValue<int> Convert( const CMinPrecValue<float> & A )
{
	CMinPrecValue<int> R;
	for( UINT i=0; i < A.m_Values.size(); i++ )
	{
		// Deal with float-int conversion specially, since +INF converts to 0x7fffffff, but static cast converts to 0x80000000
		float v = A.m_Values[i];
		int r = static_cast<int>( v );
		R.m_Values.push_back( r == 0x80000000 && v > 0 ? 0x7fffffff : r );
	}
	R.m_Levels = EPL_32;

	R.Uniquify();

	return R;
}

CMinPrecValue<UINT> f32tof16( const CMinPrecValue<float> & A )
{
	CMinPrecValue<UINT> R;
	for( UINT i=0; i < A.m_Values.size(); i++ )
	{
		float v = A.m_Values[i];
		Float16Maker f16( v );
		UINT f16asUINT = static_cast<UINT16>( f16 );
		R.m_Values.push_back( f16asUINT );
		if( v == 0 )
		{
			// Add both -0 and 0
			R.m_Values.push_back( static_cast<UINT16>( Float16Maker( -v ) ) );
		}
	}
	R.m_Levels = EPL_32;

	R.Uniquify();

	return R;
}

CMinPrecValue<float> f16tof32( const CMinPrecValue<UINT> & A )
{
	CMinPrecValue<float> R;
	for( UINT i=0; i < A.m_Values.size(); i++ )
	{
		UINT v = A.m_Values[i];
		assert( ( v & 0xffff0000 ) == 0 );
		Float16Maker f16( static_cast<UINT16>( v ) );
		float f16asFloat = static_cast<float>( f16 );
		R.m_Values.push_back( f16asFloat );
	}
	R.m_Levels = EPL_32;

	R.Uniquify();

	return R;
}

/*************************************************************
* CConvertPrecisionTest Setup and Initialization methods     *
**************************************************************/

TEST_RESULT CConvertPrecisionTest::Setup()
{
	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	if( FeatureLevel < D3D_FEATURE_LEVEL_10_0 )
	{
		WriteToLog( "TypeConversion tests require feature level 10.0 support." );
		return RESULT_SKIP;
	}

	return CPrecisionTest::Setup();
}

void CConvertPrecisionTest::InitTestParameters()
{
    // If needed setup the D3D device for caps checking caps
    // BUGBUG: Generally speaking it is unwise to use device caps generate test factors
    // this will cause test cases to vary across different devices potentially causing 
    // confusion.  If possible it is suggested that caps are checked during test case
    // execution and return RESULT_SKIP for unsupported variations with a log entry explaining why
    // the case is skipping.
    // Another problem with the following is it creates a potential failure condition within
    // a void function.  The only way to handle this failure is to throw an exception.
    if (nullptr == GetDevice())
    {
        TEST_RESULT tr = GetFramework()->SetupD3D();
        assert(RESULT_PASS == tr);
        if (RESULT_PASS != tr)
        {
            WriteToLog(_T("CConvertPrecisionTest::InitTestParameters(): SetupD3D() failed"));
        }
    }

    assert(GetDevice());

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();

	// Check that the current adapter supports Doubles
	D3D11_FEATURE_DATA_DOUBLES hwOptions = {0};
	HRESULT hr = S_OK;
	if( FAILED( hr = GetDevice()->CheckFeatureSupport(D3D11_FEATURE_DOUBLES, &hwOptions, sizeof(hwOptions) ) ) )
	{
		LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed for D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCheckFeatureSupport );
	}

	// For each instruction, input is either:
	//   float32 (any MP), float16, int (16+ MP), UINT (16+ MP), double
	// Output is:
	//   float32 (any MP), float16, int (16+ MP), UINT (16+ MP), double
	// Inter-stage type is:
	//   float32,          UINT,    int,          UINT,          UINT
	// Input type is:
	//   float32,          UINT,    int,          UINT,          double
	// Num operands:
	//   if doubles involved: 2, otherwise 4


	// Initialize test factors

	// Operation
	CTestCaseParameter<CONV_OP> *pInstructionParam = AddParameter( _T( "Inst" ), &m_ConvOp );
	testfactor::RFactor rf32tof16;
	testfactor::RFactor rf16tof32;
	testfactor::RFactor rftoi = AddParameterValue( pInstructionParam, CO_FTOI );
	testfactor::RFactor rftou = AddParameterValue( pInstructionParam, CO_FTOU );
	testfactor::RFactor ritof = AddParameterValue( pInstructionParam, CO_ITOF );
	testfactor::RFactor rutof = AddParameterValue( pInstructionParam, CO_UTOF );
	testfactor::RFactor rdtof;
	testfactor::RFactor rftod;
	if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		rf32tof16 = AddParameterValue( pInstructionParam, CO_F32TOF16 );	// SM5+
		rf16tof32 = AddParameterValue( pInstructionParam, CO_F16TOF32 );	// SM5+
	}
	if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 && hwOptions.DoublePrecisionFloatShaderOps )
	{
		rdtof = AddParameterValue( pInstructionParam, CO_DTOF );			// SM5+, Supports doubles
		rftod = AddParameterValue( pInstructionParam, CO_FTOD );			// SM5+, Supports doubles
	}
	SetParameterDefaultValue<CONV_OP>( pInstructionParam, CO_FTOU );

	// Source Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pMinPrecisionParam = AddParameter( _T( "SrcMinPrecision" ), &m_MinPrecision );
	CUserValueSet<D3D_MIN_PRECISION> *pPrecisionValues = new CUserValueSet<D3D_MIN_PRECISION>( );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_DEFAULT );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_16 );
	testfactor::RFactor rMinPrecision16Plus = AddParameterValueSet( pMinPrecisionParam, pPrecisionValues );
	testfactor::RFactor rMinPrecision10 = AddParameterValue( pMinPrecisionParam, D3D_MIN_PRECISION_ANY_10 );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	// Dest Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pDestMinPrecisionParam = AddParameter( _T( "DestMinPrecision" ), &m_DestMinPrecision );
	CUserValueSet<D3D_MIN_PRECISION> *pDestPrecisionValues = new CUserValueSet<D3D_MIN_PRECISION>( );
	pDestPrecisionValues->AddValue( D3D_MIN_PRECISION_DEFAULT );
	testfactor::RFactor rDestMinPrecision32Bit = AddParameterValueSet( pDestMinPrecisionParam, pDestPrecisionValues );
	pDestPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_16 );
	testfactor::RFactor rDestMinPrecision16Plus = AddParameterValueSet( pDestMinPrecisionParam, pDestPrecisionValues );
	pDestPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_10 );
	testfactor::RFactor rDestMinPrecision8Plus = AddParameterValueSet( pDestMinPrecisionParam, pDestPrecisionValues );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pDestMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	testfactor::RFactor rShaderStage = InitializeShaderStageModifierFactor();
	testfactor::RFactor rSrcMod = InitializeSrcModifierFactor();
	testfactor::RFactor rOperandType = InitializeOperandTypeFactor();

	// Values
	CUserValueSet<UINT>* pUintValues = new CUserValueSet<UINT>( );
	pUintValues->AddValue( 0 );
	pUintValues->AddValue( 1 );
	pUintValues->AddValue( 1170 );
	pUintValues->AddValue( USHORT_MAX );
	pUintValues->AddValue( USHORT_MAX - 1 );
	pUintValues->AddValue( USHORT_MAX + 1 );

	CUserValueSet<int>* pIntValues = new CUserValueSet<int>( );
	pIntValues->AddValue( 0 );
	pIntValues->AddValue( 1 );
	pIntValues->AddValue( -1 );
	pIntValues->AddValue( 3445 );
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
	pFloat10Values->AddValue( -1.12312312312f );
	pFloat10Values->AddValue( -0.123e-10f );
	pFloat10Values->AddValue( 0.123e-20f );

	CUserValueSet<float>* pFloatValues = new CUserValueSet<float>( );
	pFloatValues->AddValue( -1.0f );
	pFloatValues->AddValue( 0.0f );
	pFloatValues->AddValue( 45.5236236234f );
	pFloatValues->AddValue( -101.125f );
	pFloatValues->AddValue( -1.125f );
	pFloatValues->AddValue( FLT_MAX ); //3.402823466e+38F
	pFloatValues->AddValue( 1.12523423452342f );
	pFloatValues->AddValue( -1.0f / 255.0f );
	pFloatValues->AddValue( -2.0f );
	pFloatValues->AddValue( -3.88f );
	pFloatValues->AddValue( 1.0f / 255.0f );
	pFloatValues->AddValue( 1.0f );
	pFloatValues->AddValue( 1.0f + 1.0f / 255.0f );
	pFloatValues->AddValue( g_fDenormFloat16 );
	pFloatValues->AddValue( 12345678 );	// out of range of float16 (approx max 2 ^ 16 = 65000)
	pFloatValues->AddValue( 1.0f + 255.0f / 256.0f );
	pFloatValues->AddValue( g_fINFn32 );
	pFloatValues->AddValue( g_fINFp32 );
	pFloatValues->AddValue( FLT_MIN );
	pFloatValues->AddValue( -FLT_MIN );

	CUserValueSet<UINT>* pFloat16Values = new CUserValueSet<UINT>( );
	pFloat16Values->AddValue( 0 );
	pFloat16Values->AddValue( Float16Maker( 1.0f ) );
	pFloat16Values->AddValue( Float16Maker( -1.0f ) );
	pFloat16Values->AddValue( Float16Maker( 75.1252342343f ) );
	pFloat16Values->AddValue( Float16Maker( g_fDenormFloat16 ) );
	pFloat16Values->AddValue( Float16Maker( g_fSmallFloat16 ) );
	pFloat16Values->AddValue( Float16Maker( g_fMaxFloat16 ) );
	pFloat16Values->AddValue( Float16Maker( g_fMinFloat16 ) );
	pFloat16Values->AddValue( Float16Maker( -g_fDenormFloat16 ) );
	pFloat16Values->AddValue( Float16Maker( g_fMaxFloat16 + 1 ) );
	pFloat16Values->AddValue( Float16Maker( FLT_MAX ) );
	pFloat16Values->AddValue( Float16Maker( FLT_MIN) );

	CUserValueSet<double>* pDoubleValues = new CUserValueSet<double>( );
	pDoubleValues->AddValue( -1.0f );
	pDoubleValues->AddValue( 0.0f );
	pDoubleValues->AddValue( 1.0f );
	pDoubleValues->AddValue( 125.25f );
	pDoubleValues->AddValue( 12523987235498739.25 );
	pDoubleValues->AddValue( -FLT_MAX ); // -3.402823466e+38F
	pDoubleValues->AddValue( DBL_MAX  );
	pDoubleValues->AddValue( DBL_MIN  );
	pDoubleValues->AddValue( -g_fDenormFloat16  );
	pDoubleValues->AddValue( g_fSmallFloat16  );
	pDoubleValues->AddValue( (double)FLT_MAX * 4.0 );	
	pDoubleValues->AddValue( g_fMinFloat16  );


	// Create lists of values
	testfactor::RFactor rValues10Float4 =      InitializeValueFactor<float,1>( this, pFloat10Values, m_SrcFloat, NULL, 4, "" );
	testfactor::RFactor rValuesFloat4 =      InitializeValueFactor<float,1>( this, pFloatValues, m_SrcFloat, NULL, 4, "" );
	testfactor::RFactor rValues10Float2 =      InitializeValueFactor<float,1>( this, pFloat10Values, m_SrcFloat, NULL, 2, "" );
	testfactor::RFactor rValuesFloat2 =      InitializeValueFactor<float,1>( this, pFloatValues, m_SrcFloat, NULL, 2, "" );

	testfactor::RFactor rValuesSmallFloat4 = InitializeValueFactor<UINT,1>( this, pFloat16Values, m_SrcUint, NULL, 4, "u" );
	testfactor::RFactor rValuesUint4 =       InitializeValueFactor<UINT,1>( this, pUintValues, m_SrcUint, NULL, 4, "u" );
	testfactor::RFactor rValuesInt4 =        InitializeValueFactor<int,1>( this, pIntValues, m_SrcInt, NULL, 4, "i" );
	testfactor::RFactor rValuesDoubles2 =    InitializeValueFactor<double,1>( this, pDoubleValues, m_SrcDouble, NULL, 2, "d" );


	// Combine values with min precision and source mod
	testfactor::RFactor rMinPrecision10PlusValuesFloat4SrcMod = rMinPrecision16Plus * rValuesFloat4 % rSrcMod  + 
		                                                        rMinPrecision10 * rValues10Float4 % rSrcMod;
	testfactor::RFactor rMinPrecision8PlusValuesFloat4SrcMod = rMinPrecision10PlusValuesFloat4SrcMod;
	testfactor::RFactor rMinPrecision8PlusValuesFloat2SrcMod = rMinPrecision16Plus * rValuesFloat2 % rSrcMod  + 
		                                                       rMinPrecision10 * rValues10Float2 % rSrcMod;


	testfactor::RFactor rInstructionsAndValuesFactorNoSrcMod = 
		rf16tof32 *	rDestMinPrecision8Plus * rValuesSmallFloat4;

	testfactor::RFactor rInstructionsAndValuesAndSrcModFactor = 
		rf32tof16 * rMinPrecision10PlusValuesFloat4SrcMod                              + 
		rftoi     * rMinPrecision8PlusValuesFloat4SrcMod     * rDestMinPrecision16Plus + 
		rftou     * rMinPrecision8PlusValuesFloat4SrcMod     * rDestMinPrecision16Plus + 
		ritof     * rMinPrecision16Plus * rValuesInt4        * rDestMinPrecision8Plus  + 
		rutof     * rMinPrecision16Plus * rValuesUint4       * rDestMinPrecision8Plus  + 
		rdtof                           * rValuesDoubles2    * rDestMinPrecision8Plus  + 
		rftod     * rMinPrecision8PlusValuesFloat2SrcMod                                ;

	SetRootTestFactor( ( rInstructionsAndValuesFactorNoSrcMod + rInstructionsAndValuesAndSrcModFactor ) % rOperandType % rShaderStage );


	// Set priority values
	CTestCaseParameter<OPERAND_TYPE>* pOperandTypeParam = static_cast<CTestCaseParameter<OPERAND_TYPE>*>( GetTestCaseParameter( _T( "OperandType" ) ) );
	CTestCaseParameter<SRC_MOD_TYPE>* pSrcModParam = static_cast<CTestCaseParameter<SRC_MOD_TYPE>*>( GetTestCaseParameter( _T( "SrcMod" ) ) );
	assert( pOperandTypeParam != NULL );
	assert( pSrcModParam != NULL );

	AddPriorityPatternFilter( FilterNotEqual<D3D_MIN_PRECISION>(pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT), WeightFactorVeryLarge );
	AddPriorityPatternFilter( FilterNotEqual<D3D_MIN_PRECISION>(pDestMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT), WeightFactorVeryLarge );
	AddPriorityPatternFilter( FilterEqual<OPERAND_TYPE>(pOperandTypeParam, OT_CONSTANT_BUFFER), WeightFactorLarge );
	AddPriorityPatternFilter( FilterEqual<SRC_MOD_TYPE>(pSrcModParam, SMOD_NONE), WeightFactorLarge );

	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );
}


/*************************************************************
* Shader generation methods                                  *
**************************************************************/

CArithmeticPrecisionTest::EValueType CConvertPrecisionTest::GetInterShaderValueType()
{
	CArithmeticPrecisionTest::EValueType m_DestValueType = GetShaderOperationDestValueType();
	return m_DestValueType == VT_Double ? CArithmeticPrecisionTest::VT_Uint : m_DestValueType;
}

CArithmeticPrecisionTest::EValueType CConvertPrecisionTest::GetShaderOperationDestValueType()
{
	switch( m_ConvOp )
	{
	case CO_F32TOF16: return VT_Uint;
	case CO_F16TOF32: return VT_Float;
	case CO_FTOI: return VT_Int;
	case CO_FTOU: return VT_Uint;
	case CO_ITOF: return VT_Float;
	case CO_UTOF: return VT_Float;
	case CO_DTOF: return VT_Float;
	case CO_FTOD: return VT_Double;
	}
	assert( false );
	return VT_Float;
}

CArithmeticPrecisionTest::EValueType CConvertPrecisionTest::GetShaderOperationSrcValueType()
{
	switch( m_ConvOp )
	{
	case CO_F32TOF16: return VT_Float;
	case CO_F16TOF32: return VT_Uint;
	case CO_FTOI: return VT_Float;
	case CO_FTOU: return VT_Float;
	case CO_ITOF: return VT_Int;
	case CO_UTOF: return VT_Uint;
	case CO_DTOF: return VT_Double;
	case CO_FTOD: return VT_Float;
	}
	assert( false );
	return VT_Float;
}

string CConvertPrecisionTest::OperationToString( string pSrc0 )
{
	// Source modifier
	switch( m_SrcModifier )
	{
	case SMOD_NONE:
		break;
	case SMOD_NEG:
		pSrc0 = "-" + pSrc0;
		break;
	case SMOD_ABS:
		pSrc0 = "abs(" + pSrc0 + ")";
		break;
	case SMOD_ABSNEG:
		pSrc0 = "-abs(" + pSrc0 + ")";
		break;
	}

	string result = "";

	switch( m_ConvOp )
	{
	case CO_F32TOF16: 
		result = "f32tof16(" + pSrc0 + ")"; 
		break;
	case CO_F16TOF32: 
		result = "f16tof32(" + pSrc0 + ")"; 
		break;
	case CO_FTOI: 
	case CO_FTOU: 
	case CO_ITOF: 
	case CO_UTOF: 
		result = pSrc0; 
		break;
	case CO_DTOF: 
		result = "float4(" + pSrc0 + ",0,0)"; 
		break;
	case CO_FTOD:
		result = pSrc0 + ".xy"; 
		break;
	}


	return result;
}

char* CConvertPrecisionTest::GetTypeName( CArithmeticPrecisionTest::EValueType type )
{
	switch( type )
	{
	case VT_Float:
		return "float4";
	case VT_Int:
		return "int4";
	case VT_Uint:
		return "uint4";
	case VT_Double:
		return "double2";
	}
	assert( false );
	return "";
}

bool CConvertPrecisionTest::CreateShaders()
{
	HRESULT hr;

	const D3D_FEATURE_LEVEL FeatureLevel = GetDevice()->GetFeatureLevel();
	const EValueType SrcValueType = GetShaderOperationSrcValueType();
	const EValueType DestValueType = GetShaderOperationDestValueType();
	const EValueType InterShaderValueType = GetInterShaderValueType();
	SPassThroughShaders& PassThroughShaders = m_PassThroughShaders[ static_cast<UINT>(InterShaderValueType) ];

	string pSrcDecl = MinPrecisionPrefix( m_MinPrecision );
	string pDestDecl = MinPrecisionPrefix( m_DestMinPrecision );

	// The compiler flags can conceivably be modified later if there are situations that require SKIP_OPTIMIZATIONS
	UINT Flags = 0;

	string sDestType = pDestDecl + GetTypeName( DestValueType );		// ex. "min8float4"
	string sInterShaderType = GetTypeName( InterShaderValueType );		// ex. "float4"
	string sSrcType = pSrcDecl + GetTypeName( SrcValueType );			// ex. "min8float4"

	// Deal with different data types
	string sSrc0;
	string sArithmetic;
	switch( SrcValueType )
	{
	case VT_Float:
		sSrc0 = sSrcType + "(" + ToStringSafe( m_SrcFloat[0] ) + ")";
		break;
	case VT_Uint:
		sSrc0 = sSrcType + "(" + ToString( m_SrcUint[0] ) + ")";
		break;
	case VT_Int:
		sSrc0 = sSrcType + "(" + ToString( m_SrcInt[0] ) + ")";
		break;
	case VT_Double:
		sSrc0 = sSrcType + "(" + ToString( m_SrcDouble[0] ) + ")";
		break;
	}

	string sCopyToValue = "value = c;\n";
	if( DestValueType == VT_Double )
	{
		assert( InterShaderValueType == VT_Uint );
		sCopyToValue = "asuint( c.x, value.x, value.y ); \n"
					   "asuint( c.y, value.z, value.w ); \n";
	}

	// Deal with different operations
	switch( m_OperandType )
	{
	case OT_TEMP:
		sArithmetic = 
			sSrcType + " a = " + sSrc0 + ";\n" +
			sDestType + " c = " + OperationToString( "a" ) + ";\n" +
			sCopyToValue;
		break;
	case OT_CONSTANT_BUFFER:
		sArithmetic = 
			sDestType + " c = " + OperationToString( "val0" ) + ";\n" +
			sCopyToValue;
		break;
	case OT_CONSTANT_BUFFER_AND_IMMEDIATE:
		sArithmetic = 
			sDestType + " c = " + OperationToString( "val0" ) + ";\n" +
			sCopyToValue;
		break;
	case OT_TEMP_AND_IMMEDIATE:
		sArithmetic = 
			sSrcType + " a = " + sSrc0 + ";\n" +
			sDestType + " c = " + OperationToString( sSrc0 ) + ";\n" +
			sCopyToValue;
		break;
	case OT_TEMP_AND_CONSTANT_BUFFER:
		sArithmetic = 
			sSrcType + " a = " + sSrc0 + ";\n" +
			sDestType + " c = " + OperationToString( "val0" ) +  ";\n" +
			sCopyToValue;
		break;
	}

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

	static const char szShaderTemplate[] = 
		"#define SRCTYPE %s \n"
		"#define INTERSHADERTYPE %s \n"
		"#define ZEROS %s \n"
		"\n"
		"cbuffer cb  : register(b0)\n"
		"{\n"
		"    SRCTYPE val0;\n"
		"    SRCTYPE val1;\n"
		"    SRCTYPE val2;\n"
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
		"    INTERSHADERTYPE value = INTERSHADERTYPE ZEROS;\n"
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
		"INTERSHADERTYPE PSMain( nointerpolation DSOUT psin) : SV_Target\n"
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
		"    INTERSHADERTYPE value = INTERSHADERTYPE ZEROS;\n"
		"    %s \n"
		"    Result[0] = value; \n"
		"}\n";

	// Form the actual shader code (by adding a bit of arithmetic to the current shader stage
	static const UINT ArithmeticBuffer = 300;
	static const UINT shaderBufferSize = sizeof(szShaderTemplate) + ArithmeticBuffer;
	char szCurShader[shaderBufferSize];
	int MadeShader = sprintf( szCurShader, szShaderTemplate, sSrcType.c_str(), sInterShaderType.c_str(), InterShaderValueType == VT_Double ? "(0,0)" : "(0,0,0,0)", pVSCode, pHSCode, pDSCode, pGSCode,pPSCode, pCSCode );
	if( MadeShader < 0 )
	{
		LogError( __FILEW__, __LINE__, L"sprintf failed.", false, ERRORCODE_TYPE_BOOL, gFCLogicError );
		return false;
	}

	UINT shaderSize = strlen(szCurShader);
	D3D_FEATURE_LEVEL CompileFL = FeatureLevel;

	return CreateShadersFromString( szCurShader, shaderSize, CompileFL, CompileFL, Flags, PassThroughShaders );
}

/*************************************************************
* Other test case execution methods                          *
**************************************************************/

DXGI_FORMAT CConvertPrecisionTest::GetRenderTargetFormat()
{
	// Determine RT format
	const EValueType InterShaderValueType = GetInterShaderValueType();
	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	switch( InterShaderValueType )
	{
	case VT_Float: format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	case VT_Int: format = DXGI_FORMAT_R32G32B32A32_SINT; break;
	case VT_Uint: format = DXGI_FORMAT_R32G32B32A32_UINT; break;
	default:
		assert( false );
	}

	return format;
}

// Returns a pointer to the operand data for the current test case
void* CConvertPrecisionTest::GetSrcData()
{
	const EValueType SrcValueType = GetShaderOperationSrcValueType();
	switch( SrcValueType )
	{
	case VT_Float:
		return reinterpret_cast<void*>(&m_SrcFloat[0]);
	case VT_Int:
		return reinterpret_cast<void*>(&m_SrcInt[0]);
	case VT_Uint:
		return reinterpret_cast<void*>(&m_SrcUint[0]);
	case VT_Double:
		return reinterpret_cast<void*>(&m_SrcDouble[0]);
	}
	assert( false );
	return NULL;
}


/*************************************************************
* Test Evaluation methods                                    *
**************************************************************/

template< class TDest, class TSrc >
CMinPrecValue<TDest> CConvertPrecisionTest::PerformOperation( CONV_OP op, CMinPrecValue<TSrc>& a )
{
	switch( op )
	{
	case CO_FTOI: 
	case CO_ITOF: 
	case CO_DTOF: 
	case CO_FTOD: 
		return Convert<TDest,TSrc>( a );
	}

	assert( false );
	CMinPrecValue<TDest> Empty;
	return Empty;
}
template<>
CMinPrecValue<UINT> CConvertPrecisionTest::PerformOperation( CONV_OP op, CMinPrecValue<float>& a )
{
	switch( op )
	{
	case CO_F32TOF16: 
		return f32tof16( a );
	case CO_FTOU: 
		return Convert<UINT,float>( a );
	}

	assert( false );
	CMinPrecValue<UINT> Empty;
	return Empty;
}
template<>
CMinPrecValue<float> CConvertPrecisionTest::PerformOperation( CONV_OP op, CMinPrecValue<UINT>& a )
{
	switch( op )
	{
	case CO_F16TOF32: 
		return f16tof32( a );
	case CO_UTOF: 
		return Convert<float,UINT>( a );
	}

	assert( false );
	CMinPrecValue<float> Empty;
	return Empty;
}


template< class T >
void CConvertPrecisionTest::ApplySourceModifier( SRC_MOD_TYPE ModType, CMinPrecValue<T>& a )
{
	switch( ModType )
	{
	case SMOD_NONE:
		break;
	case SMOD_NEG:
		a._neg();
		break;
	case SMOD_ABS:
		a._abs();
		break;
	case SMOD_ABSNEG:
		a._absneg();
		break;
	}
}

TEST_RESULT CConvertPrecisionTest::ValidateResult()
{
	const EValueType DestValueType = GetShaderOperationDestValueType();
	const EValueType SrcValueType = GetShaderOperationSrcValueType();

	switch( DestValueType )
	{
	case VT_Float:
		switch( SrcValueType )
		{
		case VT_Float: return ValidateResultWithTemplates< float, float >();
		case VT_Int: return ValidateResultWithTemplates< float, int >();
		case VT_Uint: return ValidateResultWithTemplates< float, UINT >();
		case VT_Double: return ValidateResultWithTemplates< float, double >();
		}
	case VT_Int:
		switch( SrcValueType )
		{
		case VT_Float: return ValidateResultWithTemplates< int, float >();
		case VT_Int: return ValidateResultWithTemplates< int, int >();
		case VT_Uint: return ValidateResultWithTemplates< int, UINT >();
		case VT_Double: return ValidateResultWithTemplates< int, double >();
		}
	case VT_Uint:
		switch( SrcValueType )
		{
		case VT_Float: return ValidateResultWithTemplates< UINT, float >();
		case VT_Int: return ValidateResultWithTemplates< UINT, int >();
		case VT_Uint: return ValidateResultWithTemplates< UINT, UINT >();
		case VT_Double: return ValidateResultWithTemplates< UINT, double >();
		}
	case VT_Double:
		switch( SrcValueType )
		{
		case VT_Float: return ValidateResultWithTemplates< double, float >();
		case VT_Int: return ValidateResultWithTemplates< double, int >();
		case VT_Uint: return ValidateResultWithTemplates< double, UINT >();
		case VT_Double: return ValidateResultWithTemplates< double, double >();
		}
	}

	assert( false );
	return RESULT_FAIL;
}

template< class TDest, class TSrc >
TEST_RESULT CConvertPrecisionTest::ValidateResultWithTemplates()
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

	EPrecisionLevel MinPrecisionCheck = CMinPrecValue<float>::ConvertToPrecisionLevel( m_MinPrecision );

	// Get the result
	Vec4<TDest> Result;
	memcpy( Result.values, mapTex2D.pData, sizeof(Vec4<TDest>) );


	// We do not loop through data below.
	assert( m_RTWidth == 1 && m_RTHeight == 1 );

	// There are only 2 doubles per register, while int/UINT/float have 4
	const UINT NumComponents = ( m_ConvOp == CO_DTOF || m_ConvOp == CO_FTOD ) ? 2 : 4;

	for( UINT comp = 0; comp < NumComponents; comp++ )
	{
		// Generate the expected value
		CMinPrecValue<TSrc> a( reinterpret_cast<Vec4<TSrc>*>(GetSrcData())[0][comp] );
		a.EnsureMinPrecisionLevel( CMinPrecValue<float>::ConvertToPrecisionLevel( m_MinPrecision ), FeatureLevel );

		// Source modifiers are applied before type conversion
		ApplySourceModifier( m_SrcModifier, a );

		CMinPrecValue<TDest> expected = PerformOperation<TDest,TSrc>( m_ConvOp, a );
		expected.EnsureMinPrecisionLevel( CMinPrecValue<float>::ConvertToPrecisionLevel( m_DestMinPrecision ), FeatureLevel );

		float ULPMultiplier = 1.0f;

		if( !expected.Matches( Result[comp], MinPrecisionCheck, FeatureLevel, false, ULPMultiplier ) )
		{
			WriteActualResultAndExpectedResultsToTheLog<TDest>( 0, comp, Result[comp], expected );
			LogError( __FILEW__, __LINE__, L"Result not within expected ranges.", false, ERRORCODE_TYPE_BOOL, gFCConformanceFailure );
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}
