//////////////////////////////////////////////////////////////////////////
//  ResourceAccess.cpp
//  created:	2005/03/23
//
//  purpose: Tests Resource related shader instructions such as resinfo, ld, sample_*
//////////////////////////////////////////////////////////////////////////

#include "ResourceAccess.h"
#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"

#include <list>
#include <vector>
using namespace std;

#define MAX_STR 4000
#define MAX_TEXSTAGE 128
const size_t EXPECTED_RESULT_SIZE = 32 * 32 * 4 * sizeof(float);

#define D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES	( 128 )

#define MAX_BUFFER_BYTEWIDTH (D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_B_TERM * 1024 * 1024)

CHAR* szResInfoSuffix[] = {
	"",
	"_uint",
	"_rcpFloat"
};
#define SUFFIX_COUNT ( sizeof(szResInfoSuffix)/sizeof(szResInfoSuffix[ 0 ]) )

const int3 g_pOffsets [ ] =
{
	{	0,	0,	0	},
	{	1,	1,	1	},
	{	-1,	-1,	-1	},
	{	-2,	1,	3	},
	{	7,	7,	7	},
	{	-8,	-8,	-8	},
};

const int3 g_pOffsetsProgrammable [ ] =
{
	{	0,	0,	0	},
	{	1,	1,	1	},
	{	-1,	-1,	-1	},
	{	16,	13,	10	},
	{	-16,-13,-10	},
	{	31,	31,	31	},
	{	-32,-32,-32	},
	{	63,	99,	75	}, // values outside valid range, only 6 LSB should be used
	{	-64,-99,-76	}, // values outside valid range, only 6 LSB should be used
};

CHAR* g_szSwizzle [ ] =
{
	"rgba",
	"gbar",
	"bagr",
	"argb"
};


char szVSCodeBegin[] =
	"vs_4_0"							"\n"
	"dcl_constantBuffer c0[20], dynamicIndexed"			"\n"
	"dcl_temps 4"						"\n"
	"dcl_input v[0].xyzw"				"\n"	// in Position
	"dcl_input v[1].xyzw"				"\n"	// in Texture coordinates
	"dcl_input v[2].xyzw"				"\n"	// in pre-scaled position
	"dcl_output_sv o[0].xyzw, position"	"\n"	// out Position
	"dcl_output_typed o[1].xyzw, float"				"\n"	// out Texture coordinates or Result
	"dcl_output o[2].xyzw"				"\n"	// out TL Position (used for ld instruction in GS)
	"dcl_sampler s0, mode_default "		"\n"
	;

char szVSCodeEnd[] =
	"mad r2.xy, v[0].xy, c0[0].zwww, c0[0].xyyy""\n"
	"mov r2.zw, vec4( 0.0, 0.0, 0.0, 1.0 )"		"\n"
	"mov o[ 0 ], r2"							"\n"
	"mov o[ 1 ], r1"							"\n"
	"mov o[ 2 ], v[ 0 ].xy"					"\n"
	;

// Do not put anything >= 0.5 in this table
// as it may cause the hardware to choose a different LOD than the test expects
// it is important that 0.0 stays in this table
// as this triggers the default test cases that do not excersize this code path
// finally, these values must all be positive to ensure that mip levels below m_MipLevel are not allowed to be resident
static const float g_fLODOffsetTable[] =
{
	 0.0f,
	 0.1f,
	 0.2f,
};

// This table is used for the LD instruction
// Here it is important to test the case where the mip-map LOD requested by the instruction (last operand)
// is lower than the MinLOD of the resource
static const float g_fLODOffsetTableLD[] =
{
	 0.0f,
	 0.1f,
	 0.2f,
	 1.0f,
	 1.1f,
	 2.0f,
};

//-----------------------------------------------------------------------------

void __stdcall Tokenize( const tstring &str, vector<tstring> *pTokens )
{
	list<UINT> vIdx;
	for( UINT i = 0; i < str.length( ); ++i )
		if( str[ i ] == ',' )
			vIdx.push_back( i );

	list<UINT>::const_iterator itr = vIdx.begin( );
	int last = 0;
	while( itr != vIdx.end( ) )
	{
		pTokens->push_back( str.substr( last, *itr - last ) );
		last = *itr++ + 2;
	}
	pTokens->push_back( str.substr( last ) );
}

//-----------------------------------------------------------------------------

tstring __stdcall ToString( int3 val )
{
	return ToString( val.x ) + _T( ", " ) + ToString( val.y ) + _T( ", " ) + ToString( val.z );
}

//-----------------------------------------------------------------------------

bool __stdcall FromString( int3 *pVal, const tstring &str )
{
	vector<tstring> vStr;
	Tokenize( str, &vStr );

	FromString( &pVal->x, vStr[ 0 ] );
	FromString( &pVal->y, vStr[ 1 ] );
	FromString( &pVal->z, vStr[ 2 ] );

	return true;
}

//-----------------------------------------------------------------------------

tstring __stdcall ToString( RESOURCE_TYPE type )
{
	switch( type )
	{
	case RESOURCE_BUFFER:
//		return tstring( _T( "Buffer" ) );// "B"uffer causes error X2000: syntax error : unexpected token 'Buffer'
		return tstring( _T( "buffer" ) );
		break;
	case RESOURCE_TEXTURE1D:
		return tstring( _T( "Texture1D" ) );
		break;
	case RESOURCE_TEXTURE1D_ARRAY:
		return tstring( _T( "Texture1DArray" ) );
		break;
	case RESOURCE_TEXTURE2D:
		return tstring( _T( "Texture2D" ) );
		break;
	case RESOURCE_TEXTURE2D_ARRAY:
		return tstring( _T( "Texture2DArray" ) );
		break;
	case RESOURCE_TEXTURE3D:
		return tstring( _T( "Texture3D" ) );
		break;
	case RESOURCE_TEXTURECUBE:
		return tstring( _T( "TextureCube" ) );
		break;
	case RESOURCE_TEXTURECUBE_ARRAY:
		return tstring( _T( "TextureCubeArray" ) );
		break;
	default:
		return tstring( _T( "" ) );
		break;
	}
};

//-----------------------------------------------------------------------------

bool __stdcall FromString( RESOURCE_TYPE *pType, const tstring &str )
{
	if( tcistring( str ) == _T( "Buffer" ) )
	{
		*pType = RESOURCE_BUFFER;
		return true;
	}

	if( tcistring( str ) == _T( "Texture1D" ) )
	{
		*pType = RESOURCE_TEXTURE1D;
		return true;
	}

	if( tcistring( str ) == _T( "Texture1D Array" ) )
	{
		*pType = RESOURCE_TEXTURE1D_ARRAY;
		return true;
	}

	if( tcistring( str ) == _T( "Texture2D" ) )
	{
		*pType = RESOURCE_TEXTURE2D;
		return true;
	}

	if( tcistring( str ) == _T( "Texture2D Array" ) )
	{
		*pType = RESOURCE_TEXTURE2D_ARRAY;
		return true;
	}

	if( tcistring( str ) == _T( "Texture3D" ) )
	{
		*pType = RESOURCE_TEXTURE3D;
		return true;
	}

	if( tcistring( str ) == _T( "TextureCube" ) )
	{
		*pType = RESOURCE_TEXTURECUBE;
		return true;
	}

	if( tcistring( str ) == _T( "TextureCube Array" ) )
	{
		*pType = RESOURCE_TEXTURECUBE_ARRAY;
		return true;
	}

	pType = NULL;
	return false;
}

static bool streq(LPTSTR a, LPTSTR b)
{
	return !strcmp(a, b);
}

static bool streq(tstring a, tstring b)
{
	return a == b;
}

void CShader5xTest_ResourceAccess::TransformVertices(TLVERTEX* vTL, UINT numVerts)
{
	for (UINT idx = 0; idx < numVerts; ++idx)
	{
		// duplicate pre-scaled position
		 vTL[idx].positionTL[0] = vTL[idx].position[0];
		 vTL[idx].positionTL[1] = vTL[idx].position[1];
		// transform position
		// mad r2.xy, v[0].xy, c0[0].zwww, c0[0].xyyy
		// mov r2.zw, vec4( 0.0, 0.0, 0.0, 1.0 )
		vTL[idx].position[0] = vTL[idx].position[0] * m_matWorld[0][2] + m_matWorld[0][0];
		vTL[idx].position[1] = vTL[idx].position[1] * m_matWorld[0][3] + m_matWorld[0][1];
		vTL[idx].position[2] = 0;
		vTL[idx].position[3] = 1;
	}
}

//-----------------------------------------------------------------------------
// CShader5xTest_ResourceAccess
//-----------------------------------------------------------------------------

CShader5xTest_ResourceAccess::CShader5xTest_ResourceAccess(const TCHAR* Instruction, bool bTestResourceMinLOD) : CShader5xTest( DXGI_FORMAT_R32G32B32A32_FLOAT ),
                                                                                                                 InstAttributes( Instruction )
{
	m_pBufferRes        = NULL;
	m_pTexture1D		= NULL;
	m_pTexture2D		= NULL;
	m_pTexture3D		= NULL;
	//m_pTextureCube		= NULL;
	m_pSRV				= NULL;
	m_pUAV				= NULL;
	m_pBufferTL			= NULL;
	m_pLayoutTL			= NULL;
	m_pSampler			= NULL;

	m_pExpectedResult	= NULL;

	//m_Resource           = D3D11_RESOURCE_ERROR;
	m_DestMask			 = AssemblyHelper::xyzw;
	m_ResourceSwizzle	 = AssemblyHelper::swizXYZW;
	m_AddressSwizzle	 = AssemblyHelper::swizXYZW;
	m_OffsetSwizzle		 = AssemblyHelper::swizXYZW;
	m_nSamplerComponent  = 0;
	m_fReferenceValue	 = 0.f;
	m_ComparisonFunc	 = D3D11_COMPARISON_GREATER;
	m_bIsBound			 = false;
	m_bIsUAV			 = false;
	m_Width              = 0;
	m_Height             = 0;
	m_Depth              = 0;
	m_BufferStructStride = 0;
	m_MipCount			 = 1;
	m_ArraySize          = 1;
	m_SampleDesc.Count   = 1;
	m_SampleDesc.Quality = 0;
	m_Usage              = D3D11_USAGE_DEFAULT;
	m_BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	m_CPUAccessFlags     = (D3D11_CPU_ACCESS_FLAG) 0;
	m_MiscFlags          = (D3D11_RESOURCE_MISC_FLAG) 0;

	m_MipLevel			 = 0;
	m_ArraySlice		 = 0;
	m_fMipLODBias		 = 0.f;
	// We need to use a 16-bit component format if FL10 because 32-bit is optional
	m_Format			 = ( g_TestApp.GetFeatureLevel() >= D3D_FEATURE_LEVEL_10_1 ) ? RES_ACCESS_FORMAT_FL10_1_AND_GREATER : RES_ACCESS_FORMAT_FL10;
	m_bUsingFL10Format	 = ( m_Format == RES_ACCESS_FORMAT_FL10 );
	m_RTFormat			 = RES_RTFORMAT;
	m_bufType            = BUFFER_TYPED;
	m_nOffsets			 = g_pOffsets[ 0 ];
	m_fBorderColor[ 0 ]  = 0.25f;
	m_fBorderColor[ 1 ]  = 0.5f;
	m_fBorderColor[ 2 ]  = 0.75f;
	m_fBorderColor[ 3 ]  = 1.f;

	m_fResourceMinLODOffset = 0.0f;

	m_bTestResourceMinLOD   = bTestResourceMinLOD && (GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0);

	m_numComponentsRawOutput = 1;
}

//-----------------------------------------------------------------------------

void CShader5xTest_ResourceAccess::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor rfPSStage = AddParameterValue< D3D_SHADER_STAGE >( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );

	CTestCaseParameter< RESOURCE_TYPE > *pResTypeParam = AddParameter( _T( "Resource" ), &m_Resource );
	testfactor::RFactor rfResBuffer     = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_BUFFER		  );
	testfactor::RFactor rfResTex1D      = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE1D		  );
	testfactor::RFactor rfResTex1DArray = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE1D_ARRAY );
	testfactor::RFactor rfResTex2D      = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE2D		  );
	testfactor::RFactor rfResTex2DArray = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE2D_ARRAY );
	testfactor::RFactor rfResTexCube      = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURECUBE		  );
	testfactor::RFactor rfResTexCubeArray = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURECUBE_ARRAY );
	testfactor::RFactor rfResTex3D      = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE3D		  );

	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );


	//we need to check values when resource is bound or not to the stage
	//for non-bound resource we'll only need to run one test case for each resource type
	CTestCaseParameter<bool> *pBoundParam = AddParameter( _T("IsBound"), &m_bIsBound );
	AddParameterValueSet( pBoundParam, BoolValueSet() );
	testfactor::RFactor rfIsNotBound = AddParameterValue( pBoundParam, false );
	testfactor::RFactor rfIsBound	 = AddParameterValue( pBoundParam, true );

	CTestCaseParameter< int > *pMipLevelParam = AddParameter( _T( "MipLevel" ), &m_MipLevel);
	testfactor::RFactor rfMipLevel    = AddParameterValueSet( pMipLevelParam, GradientValueSet< int >( 0, 5, 6 ));
	SetParameterDefaultValue( pMipLevelParam, 0 );

	testfactor::RFactor rfMipCount = AddParameter( _T( "MipCount" ), &m_MipCount, GradientValueSet<UINT32>( 0, 0, 1 ));

	CTestCaseParameter<UINT32> *pArraySizeParam = AddParameter( _T( "ArraySize" ), &m_ArraySize);
	testfactor::RFactor rfArraySize = AddParameterValueSet<UINT32>( pArraySizeParam, GradientValueSet<UINT32>( 0, 0, 1 ));
	SetParameterDefaultValue<UINT32>( pArraySizeParam, 1 );

	CTestCaseParameter<int3> *pOffsetParam = AddParameter( _T( "AddrOffset" ), &m_nOffsets );
	CTableValueSet<int3> *pOffsetValues = new CTableValueSet<int3>( g_pOffsets, sizeof( g_pOffsets[ 0 ] ), ARRAYSIZE( g_pOffsets ) );
	testfactor::RFactor rfOffsets = AddParameterValueSet<int3>( pOffsetParam, pOffsetValues );

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pDestMaskParam = AddParameter( _T( "DestMask" ), &m_DestMask );
	SetParameterDefaultValue( pDestMaskParam, AssemblyHelper::xyzw );
	CTableValueSet<AssemblyHelper::MaskFlag> *pMaskValues = new CTableValueSet<AssemblyHelper::MaskFlag>( g_pMasks, sizeof( g_pMasks[ 0 ] ), ARRAYSIZE( g_pMasks ) );
	testfactor::RFactor rfDestMasks = AddParameterValueSet<AssemblyHelper::MaskFlag>( pDestMaskParam, pMaskValues );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pRscSwizzleParam = AddParameter( _T( "ResourceSwizzle" ), &m_ResourceSwizzle );
	SetParameterDefaultValue( pRscSwizzleParam, AssemblyHelper::swizXYZW );
	CTableValueSet<AssemblyHelper::Swizzle> *pSwizzleValues = new CTableValueSet<AssemblyHelper::Swizzle>( g_pSwizzles, sizeof( g_pSwizzles[ 0 ] ), ARRAYSIZE( g_pSwizzles ) );
	testfactor::RFactor rfRscSwizzles = AddParameterValueSet<AssemblyHelper::Swizzle>( pRscSwizzleParam, pSwizzleValues );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pAddrSwizzleParam = AddParameter( _T( "AddressSwizzle" ), &m_AddressSwizzle );
	SetParameterDefaultValue( pAddrSwizzleParam, AssemblyHelper::swizXYZW );
	CTableValueSet<AssemblyHelper::Swizzle> *pAddrSwizzleValues
		= new CTableValueSet<AssemblyHelper::Swizzle>( g_pAddressXYSwizzles, sizeof( g_pAddressXYSwizzles[ 0 ] ), ARRAYSIZE( g_pAddressXYSwizzles ) );
	testfactor::RFactor rfAddrSwizzles = NewReverseFactor( AddParameterValueSet<AssemblyHelper::Swizzle>( pAddrSwizzleParam, pAddrSwizzleValues ) );

	CTestCaseParameter<float> *pResourceMinLODParam = AddParameter( _T( "ResourceMinLODOffset" ), &m_fResourceMinLODOffset );
	SetParameterDefaultValue( pResourceMinLODParam, 0.0f );

	testfactor::RFactor rfResourceMinLOD;

	// Only test resource min lod for the test cases that explictly request it
	if ( m_bTestResourceMinLOD )
	{
		if( InstAttributes.FullMatch(Opcode::Load, Modifier::None, Modifier::ResidencyFeedback) )
		{
			rfResourceMinLOD = AddParameterValueSet<float>( pResourceMinLODParam, new CTableValueSet<float>( g_fLODOffsetTableLD, sizeof( g_fLODOffsetTableLD[0] ), ARRAYSIZE( g_fLODOffsetTableLD ) ) );
		}
		else
		{
			rfResourceMinLOD = AddParameterValueSet<float>( pResourceMinLODParam, new CTableValueSet<float>( g_fLODOffsetTable, sizeof( g_fLODOffsetTable[0] ), ARRAYSIZE( g_fLODOffsetTable ) ) );
		}
	}
	else
	{
		rfResourceMinLOD = AddParameterValueSet<float>( pResourceMinLODParam, new CUserValueSet<float>( 0.0f ) );
	}

	//figure out which formats are used with each instruction
	const bool bPSOnly = InstAttributes.FullMatch(Opcode::Sample, Modifier::None, Modifier::ResidencyFeedback | Modifier::Clamp | Modifier::Comparison | Modifier::LODBias);

	// Utilize a base class function to add the shader stages to the "important" testcases
	rfResBuffer = ( rfResBuffer ) * ( rfIsNotBound + rfIsBound );

	// Utilize pairwise to reduce the number of testcases
	testfactor::RFactor rfResTexNonArray = rfResTex1D + rfResTex2D + rfResTexCube + rfResTex3D;
	testfactor::RFactor rfResTexArray = rfResTex1DArray + rfResTex2DArray + rfResTexCubeArray;
	testfactor::RFactor rfMipLevelAndMinLOD = rfMipLevel * rfResourceMinLOD;
	testfactor::RFactor rfMipCountAndOffsets = rfMipCount * rfOffsets;

	testfactor::XFACTOR resNonArrayTexPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{rfResTexNonArray, 1, NULL, 0},
		{rfMipLevelAndMinLOD, 1, NULL, 0},
		{rfMipCountAndOffsets, 1, NULL, 0},
	};

	testfactor::XFACTOR resArrayTexPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{rfResTexArray, 1, NULL, 0},
		{rfMipLevelAndMinLOD, 1, NULL, 0},
		{rfMipCountAndOffsets, 1, NULL, 0},
		{rfArraySize, 1, NULL, 0},
	};

	size_t groupOrders[] =
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	testfactor::RFactor rfResNonArrayIsBound = rfIsBound * NewCrossFactor(resNonArrayTexPairwiseTable, _countof(resNonArrayTexPairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfResNonArrayIsNotBound = rfIsNotBound * rfResTexNonArray;
	testfactor::RFactor rfResNonArray = rfResNonArrayIsBound + rfResNonArrayIsNotBound;

	testfactor::RFactor rfResArrayIsBound = rfIsBound * NewCrossFactor(resArrayTexPairwiseTable, _countof(resArrayTexPairwiseTable), "Pairwise", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfResArrayIsNotBound = rfIsNotBound * rfResTexArray;
	testfactor::RFactor rfResArray = rfResArrayIsBound + rfResArrayIsNotBound;

	// Add buffer resource to "ld" instruction only, for now
	if ( InstAttributes.FullMatch(Opcode::Load, Modifier::None, Modifier::ResidencyFeedback) )
	{
		testfactor::RFactor rfLDTest = rfResBuffer + rfResNonArray + rfResArray;
		testfactor::RFactor rfLDTestWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfLDTest );
		SetRootTestFactor( rfLDTestWithShaderStages % (rfDestMasks + rfRscSwizzles + rfAddrSwizzles + (rfRscSwizzles % rfDestMasks % rfAddrSwizzles) ));
	}
	else
	{
		testfactor::RFactor rfTest = rfResNonArray + rfResArray;

		if( bPSOnly )
			rfTest = rfPSStage * rfTest;
		else
			rfTest = GetTestFactorWithDefaultStageCoverage( rfTest );

		SetRootTestFactor( rfTest % (rfDestMasks + rfRscSwizzles + rfAddrSwizzles + (rfRscSwizzles % rfDestMasks % rfAddrSwizzles) ) );
	}

	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("Resource"), RESOURCE_BUFFER), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("Resource"), RESOURCE_TEXTURE2D), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("IsBound"), true), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::MaskFlag>(_T("DestMask"), g_pMasks[0]), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("ResourceSwizzle"), AssemblyHelper::swizXYZW), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::Setup()
{
	HRESULT hr;
	TEST_RESULT tr = CShader5xTest::Setup();
	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
		return tr;

	if (SkipTiled())
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::ExecuteTestCase - Tiled resource ops not supported."));
		return RESULT_SKIP;
	}

	// Set additional bind flags (HW was checked in CShader5xTestApp::InitTestGroups())
	m_BindFlags	|=  ( g_TestApp.IsFL11OrGreater()  ? D3D11_BIND_UNORDERED_ACCESS : 0 );

	GetEffectiveContext()->RSSetState( m_pRSEnabled );

	//create default vertex shader
	ID3D10Blob *pShaderBuf = NULL;
	ID3D10Blob *pErrorBuf = NULL;

	char szCode [ MAX_STR ];
	_snprintf( szCode, MAX_STR  - 1, "%smov r1, v[ 1 ]\n%s", szVSCodeBegin, szVSCodeEnd);
	szCode[MAX_STR - 1] = 0; // Make Prefast Happy

	if( !AssembleShader( szCode, &pShaderBuf, &pErrorBuf ) )
	{
		if (pErrorBuf != NULL)
		{
			WriteToLog( _T( "CShader5xTest_ResourceAccess::Setup - AssembleShader() failed assembling default VS: %s" ), pErrorBuf->GetBufferPointer() );
		}
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		return RESULT_FAIL;
	}

	SAFE_RELEASE( m_pPassThroughVS );
	if( FAILED( hr = GetDevice()->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pPassThroughVS ) ) )
	{
		WriteToLog( _T( "CShader5xTest_ResourceAccess::Setup - CreateVertexShader() failed creating m_pVS. hr = %s"), D3DHResultToString(hr).c_str() );
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		return RESULT_FAIL;
	}

	if( g_bDumpDissasm )
		MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	//declare buffer used for input data and setup input assembler
	D3D11_INPUT_ELEMENT_DESC DeclTL[] =
	{
		// these are all really float inputs, but we'll call them uint to avoid debug spew
		{"0_", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},  //position
		{"1_", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},  //texture coordinates
		{"2_", 0, DXGI_FORMAT_R32G32_UINT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},  //pre-scaled position
	};
	if(	FAILED(	hr = GetDevice()->CreateInputLayout( DeclTL, ARRAYSIZE(DeclTL), pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), &m_pLayoutTL ) ) )
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::Setup - CreateInputLayout() failed. hr=%s"), D3DHResultToString(hr).c_str() );
		SAFE_RELEASE( pErrorBuf );
		SAFE_RELEASE( pShaderBuf );
		return RESULT_FAIL;
	}
	GetEffectiveContext()->IASetInputLayout( m_pLayoutTL );


	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	m_pExpectedResult = malloc( EXPECTED_RESULT_SIZE );
	FillMemory( (BYTE*)m_pExpectedResult, EXPECTED_RESULT_SIZE, 0);//0x81);

	//setup view port
	m_viewPort = CD3D11_VIEWPORT( 0.f, 0.f, DEFAULT_WIDTH, DEFAULT_HEIGHT );
	GetEffectiveContext()->RSSetViewports(1, &m_viewPort);

	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.ByteWidth      = 32 * 32 * sizeof( TLVERTEX );
	buffer_desc.Usage          = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags      = 0;

	if( FAILED( hr = GetDevice()->CreateBuffer( &buffer_desc, NULL, &m_pBufferTL) ) )
	{
		WriteToLog( _T("CShader5xTest_ResourceAccess::Setup - CreateBuffer() for Stream Input Buffer failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	UINT StridesTL = sizeof( TLVERTEX );
	UINT Offset = 0;
	GetEffectiveContext()->IASetVertexBuffers(0, 1, &m_pBufferTL, &StridesTL, &Offset);

	// The coordinates are given in screen-space.
	// We must compensate for the viewport transform in the shader.
	// This step can be reduced to a single shader mad instruction
	// by prescaling the inverse viewport translation by the
	// inverse viewport scale.

	memset( m_matWorld, 0, sizeof( m_matWorld ) );
	m_matWorld[0][0] = 1;
	m_matWorld[1][1] = 1;
	m_matWorld[2][2] = 1;
	m_matWorld[3][3] = 1;

	float transx = m_viewPort.Width / 2.f;
	float transy = m_viewPort.Height / 2.f;

	// Inverse viewport scale.
	m_matWorld[0][2] = 1 / transx;
	m_matWorld[0][3] = -1 / transy;

	// Prescaled inverse viewport translation.
	m_matWorld[0][0] = -transx * m_matWorld[0][2];
	m_matWorld[0][1] = -transy * m_matWorld[0][3];

	//setup sampler
	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.BorderColor[0] = 0.0f;
	SamplerDesc.BorderColor[1] = 0.0f;
	SamplerDesc.BorderColor[2] = 0.0f;
	SamplerDesc.BorderColor[3] = 0.0f;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = 5;

	if ( InstAttributes.ModifiersMatch(Modifier::Comparison) )
	{
		SamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		SamplerDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;
	}

	if( FAILED( hr = GetDevice()->CreateSamplerState(&SamplerDesc, &m_pSampler ) ))
	{
		WriteToLog( _T("CShader5xTest_ResourceAccess::Setup - CreateSamplerState() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	memset( m_pGradient, 0, sizeof( m_pGradient ) );

	for (UINT i = 0; i < MAX_MIP_COUNT; i++)
	{
		// Ensure that each mip level gets different colors
		float GradValue = (i + 1) * 32.f;
		const float NZeroVal = 1.0f;

		FLOAT_COLOR pGradColors [ ] =
		{
			{GradValue,	NZeroVal,	NZeroVal,	GradValue },
			{NZeroVal,	GradValue,	NZeroVal,	GradValue },
			{NZeroVal,	NZeroVal,	GradValue,	GradValue },
			{GradValue,	NZeroVal,	GradValue,	GradValue }
		};

		CreateGradient( m_Format, DEFAULT_HEIGHT, DEFAULT_WIDTH, pGradColors, m_pGradient[i] );
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_ResourceAccess::Setup - ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

void CShader5xTest_ResourceAccess::Cleanup()
{
	if( GetEffectiveContext() )
	{
		ID3D11SamplerState *pSamplers[] = { NULL };
		GetEffectiveContext()->VSSetSamplers(0, 1, pSamplers);
		GetEffectiveContext()->GSSetSamplers(0, 1, pSamplers);
		GetEffectiveContext()->PSSetSamplers(0, 1, pSamplers);
		GetEffectiveContext()->HSSetSamplers(0, 1, pSamplers);
		GetEffectiveContext()->DSSetSamplers(0, 1, pSamplers);
		GetEffectiveContext()->CSSetSamplers(0, 1, pSamplers);

		ID3D11InputLayout *pLayout = NULL;
		GetEffectiveContext()->IASetInputLayout( pLayout );

		ID3D11Buffer *pNullBuffer = NULL;
		UINT uInt = 0;
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &pNullBuffer, &uInt, &uInt );
		GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &pNullBuffer );

		ID3D11RenderTargetView *pRTV = NULL;
		GetEffectiveContext()->OMSetRenderTargets( 1, &pRTV, NULL );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
		}

		SAFE_RELEASE( m_pBufferRes );
		SAFE_RELEASE( m_pLayoutTL );
		SAFE_RELEASE( m_pBufferTL );
		SAFE_RELEASE( m_pSampler );
		SAFE_RELEASE( m_pRTView );

		free( m_pExpectedResult );
	}

	CShader5xTest::Cleanup();
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::SetupTestCase()
{

	TEST_RESULT tr = CShader5xTest::SetupTestCase();
	if( RESULT_PASS != tr )
		return tr;

	// texture cube arrays only supported on 10.1 and above
	if( g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_1 && m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
	{
		return RESULT_SKIP;
	}

	if (!g_TestApp.IsFL11OrGreater() &&
		m_bIsUAV &&
		!(m_Resource == RESOURCE_BUFFER &&
		(m_bufType == BUFFER_RAW || m_bufType == BUFFER_STRUCTURED)))
	{
		return RESULT_SKIP;
	}

	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE && !g_TestApp.UAVIsAllowed() )
	{
		return RESULT_SKIP;
	}

	// Compute shader stage uses raw buffer as output UAV
	if ( m_Format == COMP_RTFORMAT ) // for comparison groups, use 1-component result
		m_RTFormat = COMP_RTFORMAT;
	else
		m_RTFormat = RES_RTFORMAT;

	m_numComponentsRawOutput = 1;
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		m_RTFormat = DXGI_FORMAT_R32_TYPELESS;
		m_numComponentsRawOutput = 4; // ResourceAccess groups need 4-component result for each pixel/thread
		if ( m_Format == COMP_RTFORMAT ) // for comparison groups, use 1-component result
			m_numComponentsRawOutput = 1;
	}

	// version check, skip texcube arrays for 10.0
	if( GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 && m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
		return RESULT_SKIP;

    const bool bNeedsPixelDerivatives = InstAttributes.FullMatch(Opcode::Sample, Modifier::None, Modifier::Comparison | Modifier::LODBias | Modifier::ResidencyFeedback | Modifier::Clamp) ||
                                        InstAttributes.FullMatch(Opcode::LOD, Modifier::None, Modifier::None);

	if (m_ShaderStage == D3D_SHADER_STAGE_PIXEL)
	{
		// certain instructions cannot have .xx or .yy address swizzle due to degenerate derivatives
		if ( bNeedsPixelDerivatives && m_AddressSwizzle.r == m_AddressSwizzle.g )
		{
			m_AddressSwizzle = AssemblyHelper::swizXYZW;
		}
	}
	else
	{
		// certain instructions only work in pixel shader
		if ( bNeedsPixelDerivatives )
		{
			return RESULT_SKIP;
		}
	}

	//BUGBUG change m_ArraySize
	//BUGBUG change m_Width/m_Height/m_Depth
	//finalize resource dimensions
	m_Width = 32;
	m_Height = 32;
	m_Depth = 16;
	m_MipCount = 0;

	m_bTestOffset = GetTestCaseParameter( _T( "AddrOffset" ) ) && GetTestCaseParameter( _T( "AddrOffset" ) )->IsActive() && !(m_nOffsets == g_pOffsets[ 0 ]);
	if( (m_Resource == RESOURCE_TEXTURECUBE || m_Resource == RESOURCE_TEXTURECUBE_ARRAY) )
	{
		// ld is undefined on texcube, as is aoffimmi on all sample instructions
		// sample_d needs derivative transformation for texcube, which is too complex for this test
		if (m_bTestOffset || InstAttributes.FullMatch(Opcode::Load, Modifier::None, Modifier::ResidencyFeedback) ||
			InstAttributes.FullMatch(Opcode::Sample, Modifier::Derivative, Modifier::ResidencyFeedback | Modifier::Clamp))
			return RESULT_SKIP;
		// don't mess with address swizzle for texcube, because we always face +X
		m_AddressSwizzle = AssemblyHelper::swizXYZW;
	}
	if ( m_Resource == RESOURCE_TEXTURE1D_ARRAY )
	{
		// don't mess with address swizzle for tex1d array, because x is float and y is int
		m_AddressSwizzle = AssemblyHelper::swizXYZW;
	}

	if (!InstAttributes.FullMatch(Opcode::Sample, Modifier::LODBias, Modifier::ResidencyFeedback | Modifier::Clamp) &&
        !InstAttributes.FullMatch(Opcode::Load2dMultiSamp, Modifier::None, Modifier::ResidencyFeedback))
	{
		if ( m_Resource == RESOURCE_TEXTURE3D )
		{
			m_ArraySize = 16;
			m_ArraySlice = (int)(rand() % ( max( 1, m_Depth >> m_MipLevel) ));
		}
		else if ( m_Resource == RESOURCE_TEXTURE1D_ARRAY ||
			 m_Resource == RESOURCE_TEXTURE2D_ARRAY )
		{
			m_ArraySize = 16;
		}
		else if ( m_Resource == RESOURCE_TEXTURECUBE )
		{
			m_ArraySize = 6;
		}
		else if ( m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
		{
			m_ArraySize = 18;
		}
		else
		{
			m_ArraySize = 1;
			m_ArraySlice = 0;
		}
	}

	const bool bSampleCond = InstAttributes.ModifiersMatch(Modifier::Comparison);
	if( bSampleCond
		&& ( m_Resource == RESOURCE_TEXTURE2D_ARRAY || m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
		&& GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 )
	{
		return RESULT_SKIP;
	}

	const bool bGather = InstAttributes.OpcodeMatch(Opcode::Gather4);
	const bool bGather_po = bGather && InstAttributes.ModifiersMatch(Modifier::ProgrammableOffset);
	if (bGather_po && !(m_Resource == RESOURCE_TEXTURE2D || m_Resource == RESOURCE_TEXTURE2D_ARRAY) )
	{
		return RESULT_SKIP;
	}

	const bool bLOD = InstAttributes.FullMatch(Opcode::LOD, Modifier::None, Modifier::None);

	// recreate sampler with comparison functions for sample_c/sample_c_lz & gather
	if ( bSampleCond || bGather ||
         InstAttributes.FullMatch(Opcode::Sample, Modifier::LODBias, Modifier::ResidencyFeedback | Modifier::Clamp) ||
         bLOD )
	{
		SAFE_RELEASE( m_pSampler );

		D3D11_SAMPLER_DESC SamplerDesc;
		SamplerDesc.Filter = bSampleCond ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamplerDesc.AddressU = bGather ? m_D3DSamplerDesc.AddressU : D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressV = bGather ? m_D3DSamplerDesc.AddressV : D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.MipLODBias = bSampleCond || bGather ? 0 : m_fMipLODBias;
		SamplerDesc.MaxAnisotropy = 1;
		SamplerDesc.ComparisonFunc = m_ComparisonFunc;
		SamplerDesc.BorderColor[0] = m_fBorderColor[ 0 ];
		SamplerDesc.BorderColor[1] = m_fBorderColor[ 1 ];
		SamplerDesc.BorderColor[2] = m_fBorderColor[ 2 ];
		SamplerDesc.BorderColor[3] = m_fBorderColor[ 3 ];
		SamplerDesc.MinLOD = 0;
		SamplerDesc.MaxLOD = MAX_LOD;

		// Win7:719684
		// The lod instruction is not defined when used with a sampler that specifies point mip filtering

		if( bLOD )
		{
			SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}

		HRESULT hr;
		if( FAILED( hr = GetDevice()->CreateSamplerState(&SamplerDesc, &m_pSampler ) ))
		{
			WriteToLog( _T("CShader5xTest_ResourceAccess::SetupTestCase - CreateSamplerState() failed. hr = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	GetEffectiveContext()->VSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->GSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->PSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->HSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->DSSetSamplers(0, 1, &m_pSampler);
	GetEffectiveContext()->CSSetSamplers(0, 1, &m_pSampler);

	HRESULT hr;

	// create result render target or UAV
	tr = CreateResultRTVorUAV();
	if ( tr != RESULT_PASS )
	{
			WriteToLog( _T("CShader5xTest_ResourceAccess::SetupTestCase - CreateResultRTVorUAV failed. ") );
			return RESULT_FAIL;
	}


	if ( m_bIsBound )
	{
		tr = InitResource();
		if( tr == RESULT_SKIP || tr == RESULT_FAIL )
			return tr;
	}

	tr= SetupInputBuffer();
	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
		return tr;

	//set up resource to a shader
	UINT szTexStage;
	//if( streq( _T( "sample_c" ), szInstName ) || streq( _T( "sample_c_lz" ), szInstName ) )
		szTexStage = 0;
	//else
	//	szTexStage = ( GetFramework()->GetCurrentTestCase() - 1 )% MAX_TEXSTAGE;

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
		GetEffectiveContext()->VSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		GetEffectiveContext()->GSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_PIXEL:
		GetEffectiveContext()->PSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_HULL:
		GetEffectiveContext()->HSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		GetEffectiveContext()->DSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		GetEffectiveContext()->CSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_ResourceAccess::SetupTestCase - ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}
	return RESULT_PASS;
}


TEST_RESULT	CShader5xTest_ResourceAccess::CreateResultRTVorUAV()
{
	// create result RenderTarget or result UAV
	HRESULT hr;

	SAFE_RELEASE( m_pRTView );
	SAFE_RELEASE( m_pRTUAView );
	if ( m_ShaderStage != D3D_SHADER_STAGE_COMPUTE )
	{
		// 2D render target
		ID3D11RenderTargetView *pNullRTV = NULL;
		GetEffectiveContext()->OMSetRenderTargets( 1, &pNullRTV, NULL );

		SAFE_RELEASE( m_pRTTexture2D );
		D3D11_TEXTURE2D_DESC tex2ddesc;
		tex2ddesc.ArraySize = 1;
		tex2ddesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		tex2ddesc.Format = m_RTFormat;
		tex2ddesc.Height = 32;
		tex2ddesc.Width = 32;
		tex2ddesc.CPUAccessFlags = 0;//D3D11_CPU_ACCESS_READ;
		tex2ddesc.MipLevels = 1;
		tex2ddesc.MiscFlags = 0;
		tex2ddesc.SampleDesc.Count = 1;
		tex2ddesc.SampleDesc.Quality = 0;
		tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

		hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pRTTexture2D);
		if ( FAILED (hr) )
		{
			WriteToLog( _T( "CShader5xTest_ResourceAccess::SetupTestCase() - CreateTexture2D failed for m_pRTTexture2D, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		//need to keep copy to read data from
		SAFE_RELEASE( m_pRTTexture2DCopy );
		tex2ddesc.BindFlags = 0;
		tex2ddesc.Usage = D3D11_USAGE_STAGING;
		tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tex2ddesc.MiscFlags = 0;

		hr = GetDevice()->CreateTexture2D( &tex2ddesc, NULL, &m_pRTTexture2DCopy);
		if ( FAILED (hr) )
		{
			WriteToLog( _T( "CShader5xTest_ResourceAccess::SetupTestCase() - CreateTexture2D failed for m_pRTTexture2DCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtviewdesc;
		rtviewdesc.Texture2D.MipSlice = 0;
		rtviewdesc.Format = tex2ddesc.Format;
		rtviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = GetDevice()->CreateRenderTargetView( m_pRTTexture2D, &rtviewdesc, &m_pRTView );

		if( FAILED( hr ) )
		{
			WriteToLog( _T( "CShader5xTest_ResourceAccess::SetupTestCase() - failed to create render target view, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

	}
	else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE && g_TestApp.UAVIsAllowed() )
	{
		// Raw buffer UAV for compute shader stage
		SAFE_RELEASE( m_pRTBuffer );
		const DXGI_FORMAT uavFormat = DXGI_FORMAT_R32_TYPELESS;
		const UINT uavFormatElementSizeInBytes = GetBitsPerElement( uavFormat ) / 8;

		const UINT bufferSize = uavFormatElementSizeInBytes * 32 * 32 * m_numComponentsRawOutput;
		CD3D11_BUFFER_DESC bufferDesc = CD3D11_BUFFER_DESC(
			bufferSize,
			D3D11_BIND_UNORDERED_ACCESS );
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTBuffer ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ResourceAccess::SetupTestCase() - CreateBuffer failed for m_pRTBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		SAFE_RELEASE( m_pRTBufferCopy );
		bufferDesc = CD3D11_BUFFER_DESC( bufferSize, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ );
		if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTBufferCopy ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ResourceAccess::SetupTestCase() - CreateBuffer failed for m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		CD3D11_UNORDERED_ACCESS_VIEW_DESC rtuaviewdesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC(
			m_pRTBuffer,
			uavFormat,
			0,
			bufferSize / uavFormatElementSizeInBytes, D3D11_BUFFER_UAV_FLAG_RAW );

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pRTBuffer, &rtuaviewdesc, &m_pRTUAView ) ) )
		{
			WriteToLog( _T( "CShader5xTest_ResourceAccess::SetupTestCase() - failed to create unordered access view, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

	}

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::CreateResource()
{
	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::InitResource()
{
	//create resource
	switch ( m_Resource )
	{
	//we use this case to set NULL resource view
	//BUGBUG add case for Buffer
	case RESOURCE_BUFFER:
		return RESULT_PASS;
		break;
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		return InitTexture1D( true, TestingPerResourceMinLOD() );
		break;
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
	case RESOURCE_TEXTURECUBE:
	case RESOURCE_TEXTURECUBE_ARRAY:
		return InitTexture2D( true, TestingPerResourceMinLOD() );
		break;
	case RESOURCE_TEXTURE3D:
		return InitTexture3D( true, TestingPerResourceMinLOD() );
		break;
	default:
		return RESULT_FAIL;
		break;
	}
}

//-----------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ResourceAccess::InitBufferRes( bool bSetData )
{
	D3D11_BUFFER_DESC bufDesc;
	bufDesc.ByteWidth           = m_Width;
	bufDesc.Usage               = m_Usage;
	bufDesc.BindFlags           = m_BindFlags | (g_TestApp.UAVIsAllowed() ? D3D11_BIND_UNORDERED_ACCESS : 0);
	bufDesc.CPUAccessFlags      = m_CPUAccessFlags;
	bufDesc.MiscFlags           = 0;
	bufDesc.StructureByteStride = 0;

	// for FL10.x, when the buffer bindFlags has UNORDERED_ACCESS, the MiscFlags has to set either Raw or Structured
	// set as Raw by default
	if ( m_bufType == BUFFER_RAW || g_TestApp.IsFL10AndHWSupportsComputeOn10() )
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	if ( m_bufType == BUFFER_STRUCTURED )
	{
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufDesc.StructureByteStride = m_BufferStructStride;
	}

	SAFE_RELEASE( m_pBufferRes );
	HRESULT hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &m_pBufferRes );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitBufferRes - CreateBuffer() failed for m_pBufferRes. hr = %s"),
			D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	ID3D11Buffer *pBufferResCopy = NULL;
	bufDesc.BindFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.Usage = D3D11_USAGE_STAGING;
	bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = GetDevice()->CreateBuffer( &bufDesc, NULL, &pBufferResCopy );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitBufferRes - CreateBuffer() failed for pBufferResCopy. hr = %s"),
			D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	// Fill in buffer data
	if ( bSetData )
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		hr = GetImmediateContext()->Map( pBufferResCopy, 0, D3D11_MAP_WRITE, 0, &mappedBuffer );
		if( FAILED( hr ) )
		{
			WriteToLog(_T("CShader5xTest_ResourceAccess::InitBufferRes - Map() failed. hr = %s"), D3DHResultToString(hr).c_str() );
			SAFE_RELEASE( pBufferResCopy );
			return RESULT_FAIL;
		}
		void* pData = mappedBuffer.pData;
		void* pTempData = NULL;

		pTempData = pData;
		BYTE *pGradient = m_pGradient[0];
		BYTE uNextByteValue;

		for (UINT iX = 0; iX < m_Width; iX++)
		{

			BYTE uByteValue;
			uByteValue = pGradient[ iX ];
			((BYTE *)pTempData)[ iX ] = uByteValue;
			((BYTE *)m_pExpectedResult)[ iX ] = uByteValue;
		}

		GetImmediateContext()->Unmap( pBufferResCopy, 0 );
		GetEffectiveContext()->CopyResource( m_pBufferRes, pBufferResCopy );
	}

	SAFE_RELEASE( pBufferResCopy );

	// create SRView
	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Buffer.FirstElement = 0;
	if ( m_bufType == BUFFER_TYPED )
	{
		srv.Format              = m_Format;
		srv.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.NumElements  = ( m_Width * 8 ) / GetBitsPerElement(m_Format);
	}
	else if( m_bufType == BUFFER_RAW )
	{
		srv.ViewDimension         = D3D11_SRV_DIMENSION_BUFFEREX;
		srv.Format                = DXGI_FORMAT_R32_TYPELESS;
		srv.BufferEx.Flags        = D3D11_BUFFEREX_SRV_FLAG_RAW;
		srv.BufferEx.NumElements = ( m_Width * 8 ) / GetBitsPerElement(DXGI_FORMAT_R32_TYPELESS);

	}
	else if (m_bufType == BUFFER_STRUCTURED)
	{
		srv.ViewDimension         = D3D11_SRV_DIMENSION_BUFFEREX;
		srv.Format                = DXGI_FORMAT_UNKNOWN;
		srv.BufferEx.Flags        = 0;
		srv.BufferEx.NumElements = m_Width / m_BufferStructStride;
	}

	hr = GetDevice()->CreateShaderResourceView(m_pBufferRes, &srv, &m_pSRV );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitBufferRes - CreateShaderResourceView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//create UAViews
	if (m_bIsUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
		uav.ViewDimension         = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.FirstElement  = 0;
		uav.Buffer.Flags                 = 0;

		if ( m_bufType == BUFFER_TYPED )
		{
			uav.Format                = m_Format;
			uav.Buffer.NumElements   = ( m_Width * 8 ) / GetBitsPerElement(m_Format);
		}
		else if( m_bufType == BUFFER_RAW )
		{
			uav.Format                = DXGI_FORMAT_R32_TYPELESS;
			uav.Buffer.Flags                 = D3D11_BUFFEREX_SRV_FLAG_RAW;
			uav.Buffer.NumElements = ( m_Width * 8 ) / GetBitsPerElement(DXGI_FORMAT_R32_TYPELESS);
		}
		else if (m_bufType == BUFFER_STRUCTURED)
		{
			uav.Format                = DXGI_FORMAT_UNKNOWN;
			uav.Buffer.NumElements = m_Width / m_BufferStructStride;
		}

		hr = GetDevice()->CreateUnorderedAccessView(m_pBufferRes, &uav, &m_pUAV );
		if (FAILED(hr))
		{
			WriteToLog(_T("CShader5xTest_ResourceAccess::InitBufferRes - CreateUnorderedAccessView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;

}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::InitTexture1D( bool bSetData, bool bRepeatValues )
{
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width              = m_Width;
	texDesc.MipLevels          = m_MipCount;
	texDesc.ArraySize          = m_ArraySize;
	texDesc.Format             = m_Format;
	texDesc.Usage              = m_Usage;
	texDesc.BindFlags          = m_BindFlags;
	texDesc.CPUAccessFlags     = m_CPUAccessFlags;
	texDesc.MiscFlags          = 0;//m_MiscFlags;

	if (m_bTestResourceMinLOD)
	{
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_RESOURCE_CLAMP;
	}

	HRESULT hr = GetDevice()->CreateTexture1D( &texDesc, NULL, &m_pTexture1D );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture1D - CreateTexture1D() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	if (m_bTestResourceMinLOD)
	{
		SetResourceMinLOD( m_pTexture1D );
	}

	ID3D11Texture1D *pTexture1DCopy = NULL;
	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	hr = GetDevice()->CreateTexture1D( &texDesc, NULL, &pTexture1DCopy );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture1D - CreateTexture1D() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//if m_MipCount == 0 then need to find real value for MipLevels
	if ( m_MipCount == 0 )
		m_MipCount = (UINT32) (log((float)m_Width)/log(2.f) + 1);

	//set data
	if ( bSetData )
	{
		UINT uBitsPerElement = GetBitsPerElement( m_Format );
		UINT uBits[4];
		for(UINT index = 0; index < 4; index++)
		{
			uBits[index] = GetBitsPerComponent( index, m_Format );
		}

		for ( UINT32 iA = 0; iA < m_ArraySize; iA++ )
		{
			UINT iWidth  = m_Width;

			for( UINT32 iM = 0; iM < m_MipCount; iM++ )
			{
				UINT uRepeat = bRepeatValues ? (1 << m_MipLevel) : 1;

				D3D11_MAPPED_SUBRESOURCE mappedTexture;
				hr = GetImmediateContext()->Map( pTexture1DCopy, iA*m_MipCount + iM, D3D11_MAP_WRITE, 0, &mappedTexture );
				if( FAILED( hr ) )
				{
					WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture1D - Map() failed on level %d. hr = %s"), iM, D3DHResultToString(hr).c_str() );
					return RESULT_FAIL;
				}
				void* pData = mappedTexture.pData;
				void* pTempData = NULL;

				pTempData = pData;
				BYTE *pGradient = m_pGradient[iM % MAX_MIP_COUNT];
				BYTE uNextByteValue;

				for (UINT iX = 0; iX < iWidth; iX++)
				{
					for(UINT k = 0; k < uBitsPerElement / 8; k++)
					{
						BYTE uByteValue;

						uByteValue = pGradient[ k ];

						((BYTE *)pTempData)[ 0 ] = uByteValue;

						if ( iA == m_ArraySlice && iM == m_MipLevel )
						{
							UINT offset = (iX * uRepeat) * (uBitsPerElement/8) + k;
							for (UINT iiX = 0; iiX < uRepeat; iiX++)
							{
								size_t idx = offset + iiX * (uBitsPerElement/8);
								if (idx >= EXPECTED_RESULT_SIZE)
									break;
								((BYTE *)m_pExpectedResult)[ idx ] = uByteValue;
							}
						}

						pTempData = (BYTE*)pTempData + 1;
					}

					pGradient += (uBitsPerElement / 8);
				}

				//need to copy result for all rows
				if ( iA == m_ArraySlice && iM == m_MipLevel )
				{
					void *pTempResult = (BYTE*)m_pExpectedResult + m_Width * (uBitsPerElement/8);
					for (UINT i = 1; i < iWidth * uRepeat; i++)
					{
						memcpy(pTempResult, m_pExpectedResult, m_Width * (uBitsPerElement/8));
						pTempResult = (BYTE*)pTempResult + m_Width * (uBitsPerElement/8);
					}
				}

				GetImmediateContext()->Unmap( pTexture1DCopy, iA*m_MipCount + iM );
				iWidth = max( 1, iWidth >> 1 );
			}
		}

		GetEffectiveContext()->CopyResource( m_pTexture1D, pTexture1DCopy );
	}

	SAFE_RELEASE( pTexture1DCopy );

	//BUGBUG for arrays change FirstArraySlice to diff. values
	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format              = m_Format;

	if( m_Resource == RESOURCE_TEXTURE1D )
	{
		srv.ViewDimension         = D3D11_SRV_DIMENSION_TEXTURE1D;
		srv.Texture1D.MipLevels       = m_MipCount;
		srv.Texture1D.MostDetailedMip = 0;
	}
	else
	{
		srv.ViewDimension         = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		srv.Texture1DArray.ArraySize		  = m_ArraySize;
		srv.Texture1DArray.FirstArraySlice = 0;
		srv.Texture1DArray.MipLevels       = m_MipCount;
		srv.Texture1DArray.MostDetailedMip = 0;

	}

	hr = GetDevice()->CreateShaderResourceView(m_pTexture1D, &srv, &m_pSRV );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture1D - CreateShaderResourceView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//create UAViews
	if (m_bIsUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
		uav.Format = m_Format;

		if( m_Resource == RESOURCE_TEXTURE1D )
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
			uav.Texture1D.MipSlice = 0;
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
			uav.Texture1DArray.ArraySize = m_ArraySize;
			uav.Texture1DArray.FirstArraySlice = 0;
			uav.Texture1DArray.MipSlice = m_MipCount - 1;
		}

		SAFE_RELEASE( m_pUAV );
		hr = GetDevice()->CreateUnorderedAccessView(m_pTexture1D, &uav, &m_pUAV );
		if (FAILED(hr))
		{
			WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture1D - CreateUnorderedAccessView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::InitTexture2D( bool bSetData, bool bRepeatValues )
{
	const bool bSampleCond = InstAttributes.ModifiersMatch(Modifier::Comparison);
	const bool bGather = InstAttributes.OpcodeMatch(Opcode::Gather4);
	bool bGather40 = bGather && (g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0);
	bool bCube = m_Resource == RESOURCE_TEXTURECUBE_ARRAY || m_Resource == RESOURCE_TEXTURECUBE;
	UINT arraySize = bCube ? 6 : m_ArraySize;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width              = m_Width;
	texDesc.Height             = bCube ? m_Width : m_Height;
	texDesc.MipLevels          = m_MipCount;
	texDesc.ArraySize          = m_Resource == RESOURCE_TEXTURECUBE_ARRAY ? arraySize * 6 : arraySize;
	texDesc.Format             = (bSampleCond || bGather40) ? COMP_RTFORMAT : m_Format;
	texDesc.SampleDesc.Count   = m_SampleDesc.Count;
	texDesc.SampleDesc.Quality = m_SampleDesc.Quality;
	texDesc.Usage              = m_Usage;
	texDesc.BindFlags          = m_BindFlags;
	texDesc.CPUAccessFlags     = m_CPUAccessFlags;
	texDesc.MiscFlags          = bCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

	if (m_bTestResourceMinLOD)
	{
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_RESOURCE_CLAMP;
	}

	HRESULT hr = GetDevice()->CreateTexture2D( &texDesc, NULL, &m_pTexture2D );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture2D - CreateTexture2D() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	if (m_bTestResourceMinLOD)
	{
		SetResourceMinLOD( m_pTexture2D );
	}

	ID3D11Texture2D *pTexture2DCopy = NULL;
	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = GetDevice()->CreateTexture2D( &texDesc, NULL, &pTexture2DCopy );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture2D - CreateTexture2D() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//if m_MipCount == 0 then need to find real value for MipLevels
	if ( m_MipCount == 0 )
		m_MipCount = (UINT32) max( log((float)m_Width)/log(2.f) + 1, log((float)m_Height)/log(2.f) + 1);
	// if texcube then we're always facing +X (array slice 0)
	if( bCube )
	{
		m_ArraySlice = 0;
	}

	if ( bSetData )
	{
		for ( UINT32 iA = 0; iA < arraySize; iA++ )
		{
			UINT uBitsPerElementSRV = (bSampleCond || bGather40) ? 32 : GetBitsPerElement( m_Format );
			UINT uBits[4];
			for(UINT index = 0; index < 4; index++)
			{
				uBits[index] = GetBitsPerComponent( index, m_Format );
			}

			for( UINT32 iM = 0; iM < m_MipCount; iM++ )
			{
				void *tempData;
				UINT uRepeat = bRepeatValues ? (1 << m_MipLevel) : 1;
				D3D11_MAPPED_SUBRESOURCE mappedTex;
				mappedTex.pData    = NULL;
				mappedTex.RowPitch = 0;
				hr = GetImmediateContext()->Map( pTexture2DCopy, iA*m_MipCount + iM, D3D11_MAP_WRITE, 0, &mappedTex );
				if( FAILED( hr ) )
				{
					WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture2D - Map() failed on level %d. hr = %s"), iM, D3DHResultToString(hr).c_str() );
					return RESULT_FAIL;
				}

				tempData = mappedTex.pData;
				BYTE uNextByteValue = 0;
				BYTE *pGradient = m_pGradient[iM % MAX_MIP_COUNT];

				UINT iWidth = max( 1, m_Width >> iM );
				UINT iHeight = max( 1, m_Height >> iM );

				for(UINT iY = 0; iY < iHeight; iY++)
				{
					for (UINT iX = 0; iX < iWidth; iX++)
					{
						for(UINT k = 0; k < uBitsPerElementSRV / 8; k++)
						{
							BYTE uByteValue;

							uByteValue = pGradient[ k ];

							((BYTE *)tempData)[ 0 ] = uByteValue;

							if ( iA == m_ArraySlice && iM == m_MipLevel )
							{
								UINT offset = (UINT)((iY * uRepeat * m_Width + iX * uRepeat) * (uBitsPerElementSRV / 8) + k);

								for (UINT iiY = 0; iiY < uRepeat; iiY++)
								{
									for (UINT iiX = 0; iiX < uRepeat; iiX++)
									{
										size_t idx = offset + (iiY * 32 + iiX) * (uBitsPerElementSRV / 8);
										if (idx >= EXPECTED_RESULT_SIZE)
											break;
										((BYTE *)m_pExpectedResult)[ idx ] = uByteValue;
									}
								}
							}
							else if( bGather && bCube ) // faces not being tested directly
							{
								((BYTE *)tempData)[ 0 ] = 0; // unique face values?
							}

							tempData = ((BYTE*)tempData) + 1;
						}
						pGradient += (uBitsPerElementSRV / 8);
					}
					tempData = (BYTE*) mappedTex.pData + (iY + 1) * mappedTex.RowPitch;
					pGradient = m_pGradient[iM % MAX_MIP_COUNT] + (iY + 1) * (uBitsPerElementSRV / 8) * DEFAULT_WIDTH;
				}

				GetImmediateContext()->Unmap( pTexture2DCopy, iA*m_MipCount + iM );
			}
		}
	}

	GetEffectiveContext()->CopyResource( m_pTexture2D, pTexture2DCopy );

	SAFE_RELEASE( pTexture2DCopy );

	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format              = (bSampleCond || bGather40) ? COMP_RTFORMAT : m_Format;
	if( m_Resource == RESOURCE_TEXTURE2D )
	{
		srv.ViewDimension         = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv.Texture2D.MipLevels       = m_MipCount;
		srv.Texture2D.MostDetailedMip = 0;
	}
	else if( m_Resource == RESOURCE_TEXTURE2D_ARRAY )
	{
		srv.ViewDimension			   = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srv.Texture2DArray.ArraySize	   = arraySize;
		srv.Texture2DArray.FirstArraySlice = 0;
		srv.Texture2DArray.MipLevels       = m_MipCount;
		srv.Texture2DArray.MostDetailedMip = 0;
	}
	else if( m_Resource == RESOURCE_TEXTURECUBE )
	{
		srv.ViewDimension         = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srv.TextureCube.MipLevels       = m_MipCount;
		srv.TextureCube.MostDetailedMip = 0;
	}
	else if( m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
	{
		srv.ViewDimension			   = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		srv.TextureCubeArray.NumCubes	   = arraySize;
		srv.TextureCubeArray.First2DArrayFace = 0;
		srv.TextureCubeArray.MipLevels       = m_MipCount;
		srv.TextureCubeArray.MostDetailedMip = 0;
	}

	hr = GetDevice()->CreateShaderResourceView(m_pTexture2D, &srv, &m_pSRV );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture2D - CreateShaderResourceView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	//create UAViews
	if (m_bIsUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
		uav.Format	= m_Format;

		if( m_Resource == RESOURCE_TEXTURE2D || m_Resource == RESOURCE_TEXTURECUBE)
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uav.Texture2D.MipSlice = 0;
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uav.Texture2DArray.ArraySize = arraySize;
			uav.Texture2DArray.FirstArraySlice = 0;
			uav.Texture2DArray.MipSlice = m_MipCount - 1;
		}

		SAFE_RELEASE( m_pUAV );
		hr = GetDevice()->CreateUnorderedAccessView(m_pTexture2D, &uav, &m_pUAV );
		if (FAILED(hr))
		{
			WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture2D - CreateUnorderedAccessView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::InitTexture3D( bool bSetData, bool bRepeatValues )
{
	D3D11_TEXTURE3D_DESC texDesc;
	texDesc.Width              = m_Width;
	texDesc.Height             = m_Height;
	texDesc.Depth              = m_Depth;
	texDesc.MipLevels          = m_MipCount;
	texDesc.Format             = m_Format;
	texDesc.Usage              = m_Usage;
	texDesc.BindFlags          = m_BindFlags;
	texDesc.CPUAccessFlags     = m_CPUAccessFlags;
	texDesc.MiscFlags          = 0;

	if (m_bTestResourceMinLOD)
	{
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_RESOURCE_CLAMP;
	}

	HRESULT hr = GetDevice()->CreateTexture3D( &texDesc, NULL, &m_pTexture3D );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture3D - CreateTexture3D() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	if (m_bTestResourceMinLOD)
	{
		SetResourceMinLOD( m_pTexture3D );
	}

	ID3D11Texture3D *pTexture3DCopy = NULL;
	texDesc.BindFlags = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	hr = GetDevice()->CreateTexture3D( &texDesc, NULL, &pTexture3DCopy );
	if (FAILED(hr))
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture3D - CreateTexture3D() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//if m_MipCount == 0 then need to find real value for MipLevels
	if ( m_MipCount == 0 )
		m_MipCount = (UINT32) max( log((float)m_Width)/log(2.f) + 1, log((float)m_Height)/log(2.f) + 1);

	//set data
	if ( bSetData )
	{
		UINT iWidth  = m_Width;
		UINT iHeight = m_Height;
		UINT iDepth  = m_Depth;
		UINT uBitsPerElement = GetBitsPerElement( m_Format );
		UINT uBits[4];
		for(UINT index = 0; index < 4; index++)
		{
			uBits[index] = GetBitsPerComponent( index, m_Format );
		}

		for( UINT32 iM = 0; iM < m_MipCount; iM++ )
		{
			void *tempData;
			UINT uRepeat = bRepeatValues ? (1 << m_MipLevel) : 1;
			D3D11_MAPPED_SUBRESOURCE mappedTex;
			hr = GetImmediateContext()->Map( pTexture3DCopy, iM, D3D11_MAP_WRITE, 0, &mappedTex );
			if( FAILED( hr ) )
			{
				WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture3D - Map() failed on level %d. hr = %s"), iM, D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}

			tempData = mappedTex.pData;
			BYTE uNextByteValue = 0;

			for(UINT iD = 0; iD < iDepth; iD++)
			{
				BYTE *pGradient = m_pGradient[iM % MAX_MIP_COUNT];

				for(UINT iY = 0; iY < iHeight; iY++)
				{
					for (UINT iX = 0; iX < iWidth; iX++)
					{
						for(UINT k = 0; k < uBitsPerElement / 8; k++)
						{
							BYTE uByteValue;

							uByteValue = pGradient[ k ];

							((BYTE *)tempData)[ 0 ] = uByteValue;

							if ( iM == m_MipLevel && iD == m_ArraySlice )
							{
								UINT offset = (UINT)((iY * uRepeat * 32 + iX * uRepeat) * (uBitsPerElement/8) + k);
								for (UINT iiY = 0; iiY < uRepeat; iiY++)
									for (UINT iiX = 0; iiX < uRepeat; iiX++)
									{
										size_t idx = offset + (iiY * 32 + iiX) * (uBitsPerElement/8);
										if (idx >= EXPECTED_RESULT_SIZE)
											break;
										((BYTE *)m_pExpectedResult)[ idx ] = uByteValue;
									}
							}

							tempData = ((BYTE*)tempData) + 1;
						}
						pGradient += (uBitsPerElement/8);
					}
					tempData = ((BYTE*)mappedTex.pData) + iD * mappedTex.DepthPitch + (iY + 1) * mappedTex.RowPitch;
					pGradient = m_pGradient[iM % MAX_MIP_COUNT] + (iY + 1) * (uBitsPerElement/8) * DEFAULT_WIDTH;
				}
				tempData = ((BYTE*)mappedTex.pData) + (iD + 1) * mappedTex.DepthPitch;
			}

			GetImmediateContext()->Unmap( pTexture3DCopy, iM );

			iWidth  = max( 1, iWidth  >> 1 );
			iHeight = max( 1, iHeight >> 1 );
			iDepth  = max( 1, iDepth  >> 1 );
		}

		GetEffectiveContext()->CopyResource( m_pTexture3D, pTexture3DCopy );
	}

	SAFE_RELEASE( pTexture3DCopy );

	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format              = m_Format;
	srv.ViewDimension        = D3D11_SRV_DIMENSION_TEXTURE3D;
	srv.Texture3D.MipLevels       = m_MipCount;
	srv.Texture3D.MostDetailedMip = 0;
	hr = GetDevice()->CreateShaderResourceView(m_pTexture3D, &srv, &m_pSRV );
	if( FAILED( hr ) )
	{
		WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture3D - CreateShaderResourceView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//create UAViews
	if (m_bIsUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav;
		uav.Format	= m_Format;

		uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uav.Texture3D.MipSlice = m_MipCount - 1;
		uav.Texture3D.WSize = -1;
		uav.Texture3D.FirstWSlice = 0;

		SAFE_RELEASE( m_pUAV );
		hr = GetDevice()->CreateUnorderedAccessView(m_pTexture3D, &uav, &m_pUAV );
		if (FAILED(hr))
		{
			WriteToLog(_T("CShader5xTest_ResourceAccess::InitTexture3D - CreateUnorderedAccessView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}


//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::GenerateCurrentWidth()
{
	//define max valid value for m_Width
	UINT maxWidth = 0;
	switch ( m_Resource )
	{
	//case D3D11_RESOURCE_ERROR:
	case RESOURCE_BUFFER:
		//maxWidth = 0;
		m_Width = 0;
		return RESULT_PASS;
		break;
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		maxWidth = 8192; //D3D11_REQ_TEXTURE1D_U_DIMENSION;
		break;
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
		maxWidth = 1024;//D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		break;
	case RESOURCE_TEXTURE3D:
		maxWidth = 512;//D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
		break;
	case RESOURCE_TEXTURECUBE:
	case RESOURCE_TEXTURECUBE_ARRAY:
		maxWidth = 1024;//D3D11_REQ_TEXTURECUBE_DIMENSION;
		break;
	default:
		return RESULT_FAIL;
	}

	//define current m_Width
	switch ( m_Width )
	{
	case 0:
		m_Width = 1;
		break;
	case 1:
	//case 2:
	//case 3:
		m_Width = rand() % (maxWidth - 1) + 2;
		break;
	case 2:
		m_Width = maxWidth;
		break;
	default:
		m_Width = 0;
		break;
	}

	if ( FF_BC & GetFlags( m_Format ))
		m_Width =  max( (m_Width /4)*4, 4 );

	if ( FF_STRIDED & GetFlags( m_Format ))
		m_Width =  max( (m_Width /2)*2, 2 );

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::GenerateCurrentHeight()
{
	//define max valid value for m_Height
	UINT maxHeight = 0;
	switch ( m_Resource )
	{
	//case D3D11_RESOURCE_ERROR:
	case RESOURCE_BUFFER:
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
	case RESOURCE_TEXTURECUBE:
	case RESOURCE_TEXTURECUBE_ARRAY:
		//maxHeight = 0;
		m_Height = 0;
		return RESULT_PASS;
		break;
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
		maxHeight = 1024;//D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		break;
	case RESOURCE_TEXTURE3D:
		maxHeight = 512;//D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
		break;
	default:
		return RESULT_FAIL;
	}

	//define current m_Height
	switch ( m_Height )
	{
	case 0:
		m_Height = 1;
		break;
	case 1:
	//case 2:
	//case 3:
		m_Height = rand() % (maxHeight - 1) + 2;
		break;
	case 2:
		m_Height = maxHeight;
		break;
	default:
		m_Height = 0;
		break;
	}

	if ( FF_BC & GetFlags( m_Format ))
		m_Height =  max( (m_Height /4)*4, 4 );

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::GenerateCurrentDepth()
{
	//define max valid value for m_Depth
	UINT maxDepth = 0;
	switch ( m_Resource )
	{
	//case D3D11_RESOURCE_ERROR:
	case RESOURCE_BUFFER:
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
	case RESOURCE_TEXTURECUBE:
	case RESOURCE_TEXTURECUBE_ARRAY:
		//maxDepth = 0;
		m_Depth = 0;
		return RESULT_PASS;
		break;
	case RESOURCE_TEXTURE3D:
		maxDepth = 64;//D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
		break;
	default:
		return RESULT_FAIL;
	}

	//define current m_Depth
	switch ( m_Depth )
	{
	case 0:
		m_Depth = 1;
		break;
	case 1:
		m_Depth = rand() % (maxDepth - 1) + 2;
		break;
	case 2:
		m_Depth = maxDepth;
		break;
	default:
		m_Depth = 0;
		break;
	}

	m_ArraySlice = rand() % (int)m_Depth;

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::GenerateCurrentMipCount()
{
	//calculate number of Mip Levels given current width and height
	float MipCount = max( log((float)m_Width)/log(2.f) + 1, log((float)m_Height)/log(2.f) + 1 );

	//define current m_MipCount
	/*switch ( m_MipCount )
	{
	case 0:
	case 1:
		break;
	case 2:
		if (MipCount > 2)
			m_MipCount = rand() % (MipCount - 2) + 2;
		else
			return RESULT_SKIP;
		break;
	case 3:
		if (MipCount >= 2)
			m_MipCount = MipCount;
		else
			return RESULT_SKIP;
		break;
	/*case 4:
		m_MipCount = rand() % 10 + MipCount + 1;
		break;*
	default:
		return RESULT_FAIL;
		break;
	}*/

	//randomly choose m_MipCount from [0, MipCount]
	m_MipCount = (UINT32) fmod( (float) rand(), (MipCount + 1) );

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::GenerateCurrentMipLevel()
{
	//calculate number of Mip Levels given current width and height
	float TempMipCount = log((float)m_Width)/log(2.f) + 1;
	float MipCount = max( log((float)m_Height)/log(2.f) + 1, TempMipCount);

	//define current m_MipCount
	switch ( m_MipLevel )
	{
	//choose level < 0
	/*case 0:
		m_MipLevel = -1;
		return RESULT_SKIP;
		break;*/
	//choose level [ 0, MipCount-1 ]
	case 0:
		m_MipLevel = (int) fmod( (float) rand(), MipCount );
		break;
	//choose level 0
	/*case 1:
		m_MipLevel = 0;
		break;
	//choose level ( 0, MipCount-1 )
	case 2:
		if (MipCount > 2)
			m_MipLevel = rand() % (MipCount - 2) + 1;
		else
			return RESULT_SKIP;
		break;
	//choose level MipCount-1
	case 3:
		if ( MipCount > 1 )
			m_MipLevel = MipCount - 1;
		else
			return RESULT_SKIP;
		break;
	//choose level MipCount
	case 4:
		m_MipLevel = MipCount;
		break;
	//choose level > MipCount
	case 5:
		m_MipLevel = rand() % 10 + MipCount + 1;
		break;*/
	//choose level >= MipCount
	case 1:
		m_MipLevel = (int)(rand() % 8 + MipCount);
		break;
	default:
		return RESULT_FAIL;
		break;
	}

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::GenerateArraySize()
{
	//define max valid value for m_ArraySize
	UINT maxArraySize = 0;
	switch ( m_Resource )
	{
	//case D3D11_RESOURCE_ERROR:
	case RESOURCE_BUFFER:
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE3D:
	case RESOURCE_TEXTURECUBE:
		m_ArraySize = 1;
		return RESULT_PASS;
		break;
	case RESOURCE_TEXTURE1D_ARRAY:
		maxArraySize = 64;//D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION;
		break;
	case RESOURCE_TEXTURE2D_ARRAY:
	case RESOURCE_TEXTURECUBE_ARRAY:
		maxArraySize = 16;//D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
		break;
	default:
		return RESULT_FAIL;
	}

	//define current m_ArraySize
	m_ArraySize = rand() % ((UINT32)maxArraySize - 1) + 1;
	m_ArraySlice = rand() % m_ArraySize;

	return RESULT_PASS;
}

//
// Some sample instruction determine LOD based on the screen-space gradients of the input texture coordinates
// for these instructions, the primitive size is adjusted to ensure that the LOD coressponding to m_MipLevel is returned
// this function returns true if the current test case needs to decrease the primitive size
//
bool CShader5xTest_ResourceAccess::DecreasePrimitiveSize()
{
	bool bResult = false;

	if ( InstAttributes.FullMatch(Opcode::Sample, Modifier::None, Modifier::ResidencyFeedback | Modifier::Clamp | Modifier::LODBias | Modifier::Comparison) )
	{
		//when testing with per-resource MinLOD
		//the call to SetResourceMinLOD() will cause m_MipLevel to be chosen
		//so there is no need to decrease the size (decreasing the size would make the test case useless)
		if ( !TestingPerResourceMinLOD() )
		{
			bResult = true;
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------
TEST_RESULT CShader5xTest_ResourceAccess::SetupInputBuffer()
{
	float v0 = 0.f, v1 = 1.f;
	float tz = 0.f, tw = 0.f;
	//for sampling instructions need adjust 3rd(Z) texture coordinate to pick up correct volume slice

	//set up input buffer
	UINT iWidth = (UINT)m_Width;
	UINT iHeight = (UINT)m_Height;
	UINT iDepth = (UINT)m_Depth;

	if ( m_Resource == RESOURCE_TEXTURE3D )
		tz = (float) m_ArraySlice / (max(1, m_Depth >> m_MipLevel)) + 1.f / ( 2.f * (max(1, m_Depth >> m_MipLevel)) );
		//z = (float) 0.f;
	else if ( m_Resource == RESOURCE_TEXTURE1D_ARRAY )
		v0 = v1 = (float) m_ArraySlice;
	else if ( m_Resource == RESOURCE_TEXTURE2D_ARRAY )
		tz = (float) m_ArraySlice;
	else if ( m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
		tw = (float) m_ArraySlice;

	//for resinfo instruction we use 1x1 primitive
	if ( InstAttributes.OpcodeMatch(Opcode::ResourceInfo | Opcode::BufferInfo) )
	{
		iWidth = 1;
		iHeight = 1;
	}
	//for sample instruction we decrease primitive sizes
	else if ( DecreasePrimitiveSize() )
	{
		iWidth = max( 1, (UINT)m_Width >> m_MipLevel );
		iHeight = max( 1, (UINT)m_Height >> m_MipLevel );
	}
	//if ld is used then need to add LOD as 4th(W) texture coordinate
	else if ( InstAttributes.FullMatch(Opcode::Load, Modifier::None, Modifier::ResidencyFeedback) )
	{
		tz = (float) m_ArraySlice;
		tw = (float) m_MipLevel;
	}
	else if ( InstAttributes.FullMatch(Opcode::Load, Modifier::Raw, Modifier::ResidencyFeedback) ||
              InstAttributes.FullMatch(Opcode::Load, Modifier::Structured, Modifier::ResidencyFeedback) )
	{
		iHeight = iWidth = DEFAULT_WIDTH;
	}
	//if sample_l is used then need to add LOD as 4th(W) texture coordinate
	else if ( InstAttributes.FullMatch(Opcode::Sample, Modifier::ScalarLOD, Modifier::ResidencyFeedback) )
	{
		tw = (float) m_MipLevel;

		// When testing SetResourceMinLOD
		// the 4th texture coordinate will be set to m_MipLevel - 2.2f
		// but the hardware must still choose m_MipLevel because SetResourceMinLOD() was called
		if( TestingPerResourceMinLOD() )
		{
			tw -= 2.2f;
		}
	}

	// Copy values of texcoords to const buffer for Compute Shader to use
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedConstBuffer;
	if (FAILED( hr = GetEffectiveContext()->Map( m_pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedConstBuffer ) ) )
	{
		WriteToLog( _T("CShader5xTest_ResourceAccess::SetupInputBuffer - Map() for m_pConstBuffer failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}
	float* pConstData = (float*)mappedConstBuffer.pData;

	UINT uXCount, uYCount;
	TLVERTEX *vTL = NULL;
	//for pixel shaders we use triangle strip of 2 triangles
	if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL )
	{
		uXCount = uYCount = 2;
		vTL = new TLVERTEX[ uXCount * uYCount ];

		float x0 = 0.f;
		float x1 = (float) iWidth;
		float y0 = (float) iHeight;
		float y1 = 0.f;
		float tx0 = 0.f;
		float tx1 = 1.f;
		float ty0 = 0.f;
		float ty1 = 1.f;
		float tz1 = tz;
		float tz0 = tz;
		if( m_Resource == RESOURCE_TEXTURECUBE || m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
		{
			tx0 = 1.f;
			tx1 = 1.f;
			ty0 = 1.f;
			ty1 = -1.f;
			tz0 = 1.f;
			tz1 = -1.f;
		}

		TLVERTEX vTL_Temp[] = {
			{{x0, y0}, {tx0, ty1, tz0, tw} },
			{{x0, y1}, {tx0, ty0, tz0, tw} },
			{{x1, y0}, {tx1, ty1, tz1, tw} },
			{{x1, y1}, {tx1, ty0, tz1, tw} }
		};
		memcpy( vTL, vTL_Temp, sizeof(vTL_Temp) );

	}
	//for vertex and geometry shaders we use point lists 32x32
	else
	{
		uXCount = uYCount = 32;
		vTL = new TLVERTEX[ uXCount * uYCount ];

		for (UINT y = 0; y < uYCount; y++)
		{
			for(UINT x = 0; x < uXCount; x++)
			{
				const int idx = y * uXCount + x;
				vTL[ idx ].position[ 0 ] = (float) x + 0.5f;
				vTL[ idx ].position[ 1 ] = (float) y + 0.5f;

				vTL[ idx ].texcoord[ 0 ] = (float)x / uXCount + 1.f / (uXCount << 1);
				if ( m_Resource == RESOURCE_TEXTURE1D_ARRAY )
					vTL[ idx ].texcoord[ 1 ] = v0;
				else
					vTL[ idx ].texcoord[ 1 ] = (float)y / uYCount + 1.f / (uYCount << 1);
				vTL[ idx ].texcoord[ 2 ] = tz;
				vTL[ idx ].texcoord[ 3 ] = tw;
				if( m_Resource == RESOURCE_TEXTURECUBE || m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
				{
					float stepX = 2.f / (float)(uXCount<<1);
					float stepY = 2.f / (float)(uYCount<<1);
					vTL[ idx ].texcoord[ 0 ] = 1.f;
					vTL[ idx ].texcoord[ 1 ] = 1.f - (float)y * (2.f / (float)uYCount) - stepY;
					vTL[ idx ].texcoord[ 2 ] = 1.f - (float)x * (2.f / (float)uXCount) - stepX;
					vTL[ idx ].texcoord[ 3 ] = tw;
				}
				memcpy( pConstData + 4 * idx, vTL[ idx ].texcoord, sizeof(float) * 4 );
			}
		}

		GetEffectiveContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	}

	// copy the reference data after the texcoord values for Compute Stage
	float pRefData [ ] = { m_fReferenceValue, m_fReferenceValue, m_fReferenceValue, m_fReferenceValue };
	memcpy( ((BYTE*) pConstData) + 32 * 32 * 4 * sizeof(float), pRefData, sizeof(pRefData) );

	// copy the programmable offsets after thre reference data
	int pOffsetData [ ] = { m_nOffsets.x, m_nOffsets.y, m_nOffsets.z, 0 };
	memcpy( ((BYTE*) pConstData) + 32 * 32 * 4 * sizeof(float) + sizeof(pRefData), pOffsetData, sizeof(pOffsetData) );

	GetEffectiveContext()->Unmap(m_pConstBuffer, 0);
	TransformVertices(vTL, uXCount * uYCount);

	D3D11_MAPPED_SUBRESOURCE mappedDataTL;
	if( FAILED(hr = GetEffectiveContext()->Map( m_pBufferTL, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataTL)) )
	{
		WriteToLog( _T("CShader5xTest_ResourceAccess::SetupInputBuffer - Map() for Stream Input Buffer failed. hr = %s"), D3DHResultToString(hr).c_str() );
		delete [] vTL;
		return RESULT_FAIL;
	}
	memcpy(mappedDataTL.pData, vTL, uXCount * uYCount * sizeof(TLVERTEX));
	GetEffectiveContext()->Unmap(m_pBufferTL, 0);

	delete [] vTL;

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ResourceAccess::ExecuteTestCase()
{
	HRESULT hr;
	//create shaders
	if ( !CreateShaders() )
		return RESULT_FAIL;

	bool bResult;

	UINT ClearColor[4] = {0, 0, 0, 0};
	if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL )
	{
		GetEffectiveContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		GetEffectiveContext()->Draw( 4, 0 );
	}
	else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pRTUAView, ClearColor);
		GetEffectiveContext()->Dispatch( 32, 32, 1 );
	}
	else if ( InstAttributes.OpcodeMatch(Opcode::ResourceInfo | Opcode::BufferInfo) )
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}
	else
	{
		// primitive topology taken care of by AsmHelper
		GetEffectiveContext()->Draw( 32*32, 0 );
	}

	// Lock the render target and examine the results
	D3D11_MAPPED_SUBRESOURCE texmap;

	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );
	else
		GetEffectiveContext()->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2D, 0, NULL );


	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_ResourceAccess::ExecuteTestCase - ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		hr = GetImmediateContext()->Map( m_pRTBufferCopy, 0, D3D11_MAP_READ, 0, &texmap );
	else
		hr = GetImmediateContext()->Map( m_pRTTexture2DCopy, 0, D3D11_MAP_READ, 0, &texmap );
	if( FAILED(hr) )
	{
		WriteToLog( _T("CShader5xTest_ResourceAccess::ExecuteTestCase - Map() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		const UINT rtFormatElementSizeInBytes = GetBitsPerElement( m_RTFormat ) / 8;
		bResult = CheckResult( texmap.pData, rtFormatElementSizeInBytes * m_numComponentsRawOutput * DEFAULT_WIDTH );
		GetImmediateContext()->Unmap( m_pRTBufferCopy, 0 );
	}
	else
	{
		bResult = CheckResult( texmap.pData, texmap.RowPitch );
		GetImmediateContext()->Unmap( m_pRTTexture2DCopy, 0 );
	}

	if ( !bResult )
		return RESULT_FAIL;
	else
		return RESULT_PASS;
}

//-----------------------------------------------------------------------------

bool CShader5xTest_ResourceAccess::CreateShaders()
{
	HRESULT hr;
	ID3D10Blob *pShaderBuf = NULL;

	//First Clear RT
	DWORD dwColor = 0;//CLEAR_VALUE;
	float color = *( (float*) &dwColor );
	FLOAT colors[4] = { color, color, color, color };
	//GetEffectiveContext()->ClearRenderTargetView( m_pRTView, colors );

	ID3D11Buffer *pNullBuffers[] = { NULL };
	UINT offsets[] = { 0 };
	GetEffectiveContext()->SOSetTargets( 1, pNullBuffers, offsets );

	// register allocations: (note input registers must be abstracted away to make this work on compute)
	//*v[0] : position
	//*v[1] : texcoord
	//*v[2] : pre-scaled position
	//*r4   : result value
	// o[0] : result value
	// o[1] : position to drive PS

	// using 32*32 elements in the constant buffer for Compute Shader stage, so that
	// each thread group can have a 4-component value for texcoord
	// the other 2 elements in the constant buffer are for reference data and programmable offsets
	AssemblyHelper asmHelper( 32*32 + 2, 200, m_RTFormat, m_viewPort, m_numComponentsRawOutput);
	if ( !g_TestApp.IsFL11OrGreater()
		&& g_TestApp.IsFL10AndHWSupportsComputeOn10()
		&& m_ShaderStage != D3D_SHADER_STAGE_COMPUTE )
		asmHelper.EnableRawAndStructuredSupport();

	// input position in v[0] from input layout, output in o[1] to feed PS
	asmHelper.SetPositionInputRegIdx(0);
	asmHelper.SetPositionOutputRegIdx(1);
	// input texcoord in v[1]
	asmHelper.SetUserInputRegMaskTyped(1, AssemblyHelper::xyzw, "linear", "float");
	// input pre-scaled position in v[2]
	asmHelper.SetUserInputRegMaskTyped(2, AssemblyHelper::xy, "linear", "float");

	// output result in o[0]
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		if ( m_Format == COMP_RTFORMAT )
		{
			asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::x);
			asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::x, 4, AssemblyHelper::swizX);
		}
		else
		{
			asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
			asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::xyzw, 4, AssemblyHelper::swizXYZW);
		}
	}
	else
	{
		asmHelper.SetOutputRegMaskTyped(0, AssemblyHelper::xyzw, "float");
		asmHelper.SetupMovToOutputFromTemp(0, AssemblyHelper::xyzw, 4, AssemblyHelper::swizXYZW);
	}

	// write result from r4 to o[0]
	asmHelper.SetupMovToTempFromLiteral(4, AssemblyHelper::xyzw, "vec4(0,0,0,0)" );
	// write position from v[0] to o[1]
	asmHelper.SetupMovToOutputFromInput(1, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);

	// get the specific instruction code
	tstring instructionCode = GetShaderCode(&asmHelper);

	asmHelper.EnableTiledResources(InstAttributes.ModifiersMatch(Modifier::Clamp) || InstAttributes.ModifiersMatch(Modifier::ResidencyFeedback));

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
		m_ShaderStage, g_TestApp.m_D3DOptions,
		instructionCode, &m_pRTView, &m_pRTUAView, 1, &pShaderBuf,
		&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

	asmHelper.EnableTiledResources(false);

	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		TEST_RESULT tRes = RESULT_FAIL;
		goto createFail;
	}

	if( g_bDumpDissasm && pShaderBuf != NULL )
		MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );

	// set UAVs when it's necessary. This can only be done after asmHelper.SetupPipelineWithShaderCode
	// because it would change the effect of OMSetRenderTargetsAndUnorderedAccessViews
	if ( m_bIsUAV )
	{
		if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
			GetEffectiveContext()->CSSetUnorderedAccessViews( 7, 1, (ID3D11UnorderedAccessView**)&m_pUAV, NULL );
		else if ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL )
			GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( -1, NULL, NULL, 7, 1,
			(ID3D11UnorderedAccessView**)&m_pUAV, NULL );
	}

	SAFE_RELEASE(pShaderBuf);
	return true;

createFail:
	SAFE_RELEASE(pShaderBuf);
	return false;
}

//-----------------------------------------------------------------------------

tstring CShader5xTest_ResourceAccess::GetShaderCode(AssemblyHelper* pAsmHelper)
{
	UINT width = 0, height = 0, depth = 0;

	tstring retStr = "";

	TCHAR szDerivatives[ 30 ]; //used with sample_d
	TCHAR szResSwizzle[ 30 ];  //used with sample_c and sample_c_lz
	TCHAR szOffsetReg[ 30 ] = { 0 }; // used with gather4_po
	TCHAR szTexType[ 30 ];
	TCHAR szSampler[ 30 ];
	TCHAR szInstMod[ 30 ] = { 0 };

	_snprintf( szResSwizzle, extent<decltype(szResSwizzle)>::value - 1, ".%s", ToString(m_ResourceSwizzle).c_str() );
	szDerivatives[ 0 ] = '\0';
	const bool bGather = InstAttributes.OpcodeMatch(Opcode::Gather4);
	if( bGather )
	{
		const TCHAR* componentStr[4] = {"r","g","b","a"};
		_snprintf( szSampler, extent<decltype(szSampler)>::value - 1, ", s0.%s", componentStr[m_nSamplerComponent] );
	}
	else
		_snprintf( szSampler, extent<decltype(szSampler)>::value - 1 , ", s0");
	szSampler[sizeof( szSampler ) / sizeof( szSampler[0] ) - 1] = 0; // Make Prefast Happy

	//if ( FF_FLOAT & GetFlags( m_Format ))
		_snprintf( szTexType, extent<decltype(szTexType)>::value - 1, "float");
	//else if ( FF_UINT & GetFlags( m_Format ))
	//	_snprintf( szTexType, 20, "uint");
	//else if ( FF_SINT & GetFlags( m_Format ))
	//	_snprintf( szTexType, 20, "sint");
	//else if ( FF_UNORM & GetFlags( m_Format ))
	//	_snprintf( szTexType, 20, "unorm");
	//else if ( FF_SNORM & GetFlags( m_Format ))
	//	_snprintf( szTexType, 20, "snorm");

	szTexType[sizeof( szTexType ) / sizeof( szTexType[0] ) - 1] = 0; // Make prefast happy

	UINT32 uCaseNumber = GetFramework()->GetCurrentTestCase() - 1;
	UINT32 uTexStage;
	//if( streq( _T( "sample_c" ), szInstName ) || streq( _T( "sample_c_lz" ), szInstName ) )
		uTexStage = 0;
	//else
	//	uTexStage = uCaseNumber % MAX_TEXSTAGE;

	pAsmHelper->SetResourceDecl(uTexStage, FormatString( _T("%s, %s"), ToString( m_Resource ).c_str(), szTexType ) );

	if( InstAttributes.ModifiersMatch(Modifier::Comparison) )
		pAsmHelper->SetSamplerDecl(0, "mode_comparison");
	else
		pAsmHelper->SetSamplerDecl(0, "mode_default");

	//BUGBUG need to use input register as srcAddress sometimes
	//if ld is used then need to add LOD as r0.w
	if ( InstAttributes.FullMatch(Opcode::Load, Modifier::None, Modifier::ResidencyFeedback) )
	{
		// ld params: .a = mip level, .g & .b = array index on arrays
		if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY || m_ShaderStage == D3D_SHADER_STAGE_HULL )
			retStr += "ftoi r0, v[ 0 ][ 1 ]\n";
		else if ( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
			retStr += "ftoi r0, vcp[ 0 ][ 1 ]\n";
		else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		{
			// fetch the thread group's texcoord in the constant buffer
			retStr += "imad r1.x, vThreadGroupID.y, 32, vThreadGroupID.x \n";
			retStr += "ftoi r0.xyzw, c0[r1.x].xyzw\n";
		}
		else
			retStr += "ftoi r0, v[ 1 ]\n";

		// use pre-scaled position as ld coordinate
		if ( m_Resource ==RESOURCE_TEXTURE1D_ARRAY )
		{
			if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY || m_ShaderStage == D3D_SHADER_STAGE_HULL )
				retStr += "ftoi r0.x, v[ 0 ][ 2 ].xy\n";
			else if ( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
				retStr += "ftoi r0.x, vcp[ 0 ][ 2 ].xy\n";
			else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
				retStr += "mov r0.x, vThreadGroupID.x \n";
			else
				retStr += "ftoi r0.x, v[ 2 ].xy\n";
			retStr += "itof r1, r0.x\n";
			retStr += FormatString( "div r0.x, r1, %d.f\n", 1 << m_MipLevel );
			retStr += "ftoi r0.x, r0 \n";
		}
		else
		{
			if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY || m_ShaderStage == D3D_SHADER_STAGE_HULL )
				retStr += "ftoi r0.xy, v[ 0 ][ 2 ].xy\n";
			else if ( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
				retStr += "ftoi r0.xy, vcp[ 0 ][ 2 ].xy\n";
			else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
				retStr += "mov r0.xy, vThreadGroupID.xy \n";
			else
				retStr += "ftoi r0.xy, v[ 2 ].xy\n";
			retStr += "itof r1, r0.xy\n";
			retStr += FormatString( "div r0.xy, r1, %d.f\n", 1 << m_MipLevel );
			retStr += "ftoi r0.xy, r0 \n";
		}

		szSampler[ 0 ] = '\0';

		if( m_bTestOffset )
		{
			_snprintf( szInstMod, extent<decltype(szInstMod)>::value - 1, _T( "_aoffimmi(%d,%d,%d)" ), m_nOffsets.x, m_nOffsets.y, m_nOffsets.z );
		}

	}
	else
	{
		// load texture coordinates
		if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY || m_ShaderStage == D3D_SHADER_STAGE_HULL )
			retStr += "mov r0, v[ 0 ][ 1 ]\n";
		else if ( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
			retStr += "mov r0, vcp[ 0 ][ 1 ]\n";
		else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		{
			// fetch the thread group's texcoord in the constant buffer
			retStr += "imad r1.x, vThreadGroupID.y, 32, vThreadGroupID.x \n";
			retStr += "mov r0.xyzw, c0[r1.x].xyzw\n";
		}
		else
			retStr += "mov r0, v[ 1 ]\n";

		//if sample_d is used then need to add derivatives
		if ( InstAttributes.FullMatch(Opcode::Sample, Modifier::Derivative, Modifier::ResidencyFeedback | Modifier::Clamp) )
		{
			float fGradient = 1.f / (32 >> m_MipLevel);

			// when testing per-resource MinLOD
			// make the gradient smaller, which would cause the hardware
			// to try to sample from a lower mip level than m_MipLevel
			// but the per-resource MinLOD will ensure that m_MipLevel is used anyways
			if ( TestingPerResourceMinLOD() )
			{
				fGradient /= 4.0f;
			}

			retStr += FormatString( "mov r2, vec4(%6f, 0.f, 0.f, 0.f)\n", fGradient);
			retStr += FormatString( "mov r3, vec4(0.f, %6f, 0.f, 0.f)\n", fGradient );
			_snprintf( szDerivatives, extent<decltype(szDerivatives)>::value - 1, ", r2, r3");
		}

		if( InstAttributes.FullMatch(Opcode::Sample, Modifier::ScalarLOD, Modifier::ResidencyFeedback) )
		{
			retStr += "mov r3, r0\n";

			// For sample_l, the w component is being used to pass through the mip-level.
			// In the case of a 3D texture cube array, that coordinate is also needed to select which cube to sample.
			// So, we need to make sure and zero out the w compenent after this is done. Note: if this test were
			// ever to cycle through the cubes, this would need to be handled a different way.
			if( m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
			{
				retStr += "mov r0.w, l(0,0,0,0)\n";
			}
			_snprintf( szDerivatives, extent<decltype(szDerivatives)>::value - 1, ", r3.a");
		}

		if ( InstAttributes.ModifiersMatch(Modifier::Comparison) )
		{
			if ( InstAttributes.OpcodeMatch(Opcode::Sample) )
			{
				// srID3D11Resource.r must be .r swizzle
				_snprintf( szResSwizzle, extent<decltype(szResSwizzle)>::value - 1, ".rrrr");
			}
			//use srcReferenceValue = 0
			retStr += "mov r3, c0[ 1024 ]\n";
			_snprintf( szDerivatives, extent<decltype(szDerivatives)>::value - 1, ", r3.r");
		}

		// need to add LODBias for sample_b
		if ( InstAttributes.FullMatch(Opcode::Sample, Modifier::LODBias, Modifier::ResidencyFeedback | Modifier::Clamp) )
		{
			retStr += FormatString( "mov r3, vec4(%6f, 0.f, 0.f, 0.f)\n", m_fInstBias );
			_snprintf( szDerivatives, extent<decltype(szDerivatives)>::value - 1, ", r3.r" );
		}

		if ( InstAttributes.ModifiersMatch(Modifier::ProgrammableOffset) )
		{
			// programmable offsets are in cb[5]
			retStr += "mov r2, c0[ 1025 ]\n";
			_snprintf( szOffsetReg, extent<decltype(szOffsetReg)>::value - 1, _T( " r2.%s," ), ToString(m_OffsetSwizzle).c_str() );
		}
		else if( m_bTestOffset )
		{
			_snprintf( szInstMod, extent<decltype(szInstMod)>::value - 1, _T( "_aoffimmi(%d,%d,%d)" ), m_nOffsets.x, m_nOffsets.y, m_nOffsets.z );
		}
	}

	szSampler[sizeof( szSampler ) / sizeof( szSampler[0] ) - 1] = 0; // Make prefast happy.
	szDerivatives[ sizeof( szDerivatives ) / sizeof( szDerivatives[0] ) - 1] = 0; // Make prefast happy.
	szResSwizzle[ sizeof( szResSwizzle ) / sizeof( szResSwizzle[0] ) - 1] = 0; // Make prefast happy.

	// Does instruction require a residency status output register?
	const tstring FeedbackRegisterToUse = UseResidencyFeedback() ? _T("r5.x") : _T("null");
	const tstring ResidencyFeedbackRegWithComma = InstAttributes.ModifiersMatch(Modifier::ResidencyFeedback) ? tstring(_T(", ")) + FeedbackRegisterToUse : _T("");

	// Clamp
	const bool bClamp = InstAttributes.ModifiersMatch(Modifier::Clamp);
	const tstring ClampRegisterToUse = UseClamp() ? _T(", r6.x") : _T(", l(0.0)");
	const tstring ClampReg = bClamp ? ClampRegisterToUse : _T("");
	if (bClamp && UseClamp())
	{
		retStr += "mov r6.x, l(0.0)\n";
	}

	tstring instructionStr
		= InstAttributes.GetInstruction() + szInstMod + " r4." + ToString(m_DestMask) + ResidencyFeedbackRegWithComma + ", r0." + ToString(m_AddressSwizzle)
		+ "," + szOffsetReg + " t" + ToString(uTexStage) + szResSwizzle
		+ szSampler + szDerivatives + ClampReg;
	retStr += instructionStr;

	if(UseResidencyFeedback() && InstAttributes.ModifiersMatch(Modifier::ResidencyFeedback))
	{
		// force the driver to use feedback result to avoid UMD optimizing out the modifier
		tstring forceFeedback =
			tstring(_T("\ncheck_access_fully_mapped ")) + FeedbackRegisterToUse + _T(", ") + FeedbackRegisterToUse + _T("\n") +
			_T("xor ") + FeedbackRegisterToUse + _T(", ") + FeedbackRegisterToUse + (", 0xffffffff\n") +
			_T("xor r4.") + ToString(m_DestMask) + _T(", r4, ") + FeedbackRegisterToUse;
		retStr += forceFeedback;
	}

	return retStr;
}

//-----------------------------------------------------------------------------

void CShader5xTest_ResourceAccess::CleanupTestCase()
{
	if( GetEffectiveContext() )
	{
		ID3D11ShaderResourceView *pSRVNull[] = { NULL, NULL, NULL, NULL };
		ID3D11RenderTargetView *pRTVNull[] = { NULL, NULL, NULL, NULL };

		GetEffectiveContext()->VSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->GSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->PSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->HSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->DSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->CSSetShader( NULL, NULL, 0 );
		GetEffectiveContext()->VSSetShaderResources( 0, 4, pSRVNull );
		GetEffectiveContext()->GSSetShaderResources( 0, 4, pSRVNull );
		GetEffectiveContext()->PSSetShaderResources( 0, 4, pSRVNull );
		GetEffectiveContext()->HSSetShaderResources( 0, 4, pSRVNull );
		GetEffectiveContext()->DSSetShaderResources( 0, 4, pSRVNull );
		GetEffectiveContext()->CSSetShaderResources( 0, 4, pSRVNull );

		ID3D11SamplerState *pNullSamp = NULL;
		GetEffectiveContext()->VSSetSamplers( 0, 1, &pNullSamp );
		GetEffectiveContext()->GSSetSamplers( 0, 1, &pNullSamp );
		GetEffectiveContext()->PSSetSamplers( 0, 1, &pNullSamp );
		GetEffectiveContext()->HSSetSamplers( 0, 1, &pNullSamp );
		GetEffectiveContext()->DSSetSamplers( 0, 1, &pNullSamp );
		GetEffectiveContext()->CSSetSamplers( 0, 1, &pNullSamp );

		if( FAILED( ExecuteEffectiveContext() ) )
		{
			WriteToLog("ExecuteEffectiveContext() failed");
		}
	}

	SAFE_RELEASE( m_pTexture1D );
	SAFE_RELEASE( m_pTexture2D );
	SAFE_RELEASE( m_pTexture3D );
//	SAFE_RELEASE( m_pTextureCube );
	SAFE_RELEASE( m_pVS );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pPS );
	SAFE_RELEASE( m_pHS );
	SAFE_RELEASE( m_pDS );
	SAFE_RELEASE( m_pCS );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pUAV );


	FillMemory( (BYTE*)m_pExpectedResult, EXPECTED_RESULT_SIZE, 0);//0x81);
}

//-----------------------------------------------------------------------------

bool CShader5xTest_ResourceAccess::ValidateParameters( )
{
	UINT uBitsPerElement = GetBitsPerElement( m_Format );
	float MipCount = (float) m_MipCount;
	if ( MipCount == 0 )
	{
		UINT32 TempMipCount = (UINT32) (log((float)m_Width)/log(2.f) + 1);
		MipCount = max( log((float)m_Height)/log(2.f) + 1, TempMipCount);
	}

	//D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES
	switch ( m_Resource )
	{
	/*case D3D11_RESOURCE_ERROR:
		return false;
		break;*/
	case RESOURCE_BUFFER:
		return true;
		break;
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		while ( true )
		{
			//calculate size of resource
			float fSize = 0;
			UINT uSize = 0;
			UINT uWidth = m_Width;
			for (UINT32 i = 0; i < MipCount; i++)
			{
				uSize = (uWidth * m_ArraySize * uBitsPerElement + 7) / 8;
				fSize += uSize / 1024000.f;
				if ( uWidth > 1 )
					uWidth = uWidth >> 1;
			}

			if ( fSize <= D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES )
				break;

			//if overexceeded max size then reduce dimensions
			if ( m_ArraySize > 1)
				m_ArraySize--;
			else if ( m_Width > 1 )
			{
				m_Width = (UINT)(m_Width - m_Width*0.1f);

				if ( FF_STRIDED & GetFlags( m_Format ))
					m_Width =  max( (m_Width /2)*2, 2 );
			}
			else
				return false;

			//recalculate MipCount
			UINT32 MipCount = (UINT32) (log((float)m_Width)/log(2.f) + 1);
			if ( MipCount < m_MipCount )
				m_MipCount = MipCount;
		}
		break;
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
		while ( true )
		{
			//calculate size of resource
			float fSize = 0.f;
			UINT uSize = 0;
			UINT uWidth = m_Width;
			UINT uHeight = m_Height;
			for (UINT32 i = 0; i < MipCount; i++)
			{
				uSize = (uWidth * uHeight * m_ArraySize * uBitsPerElement + 7) / 8;
				fSize += uSize / 1024000.f;
				if ( uWidth > 1 )
					uWidth = uWidth >> 1;
				if ( uHeight > 1 )
					uHeight = uHeight >> 1;
			}

			if ( fSize <= D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES )
				break;

			//if overexceeded max size then reduce dimensions
			if ( m_ArraySize > 1)
				m_ArraySize--;
			else if ( m_Width > m_Height )
			{
				m_Width = (UINT)(m_Width - m_Width*0.1f);

				if ( FF_BC & GetFlags( m_Format ))
					m_Width =  max( (m_Width /4)*4, 4 );

				if ( FF_STRIDED & GetFlags( m_Format ))
					m_Width =  max( (m_Width /2)*2, 2 );
			}
			else if ( m_Height >= m_Width && m_Height > 1 )
			{
				m_Height = (UINT)(m_Height - m_Height*0.1f);

				if ( FF_BC & GetFlags( m_Format ))
					m_Height =  max( (m_Height /4)*4, 4 );
			}
			else
				return false;

			//recalculate MipCount
			float MipCount = max( log((float)m_Width)/log(2.f) + 1, log((float)m_Height)/log(2.f) + 1);
			if ( MipCount < (float) m_MipCount )
				m_MipCount = (UINT32) MipCount;
		}
		break;
	case RESOURCE_TEXTURE3D:
		while ( true )
		{
			//calculate size of resource
			float fSize = 0;
			UINT uSize = 0;
			UINT uWidth = m_Width;
			UINT uHeight = m_Height;
			UINT uDepth = m_Depth;
			for (UINT32 i = 0; i < MipCount; i++)
			{
				uSize = (uWidth * uHeight * uDepth * uBitsPerElement + 7) / 8;
				fSize += uSize / 1024000.f;
				if ( uWidth > 1 )
					uWidth = uWidth >> 1;
				if ( uHeight > 1 )
					uHeight = uHeight >> 1;
				if ( uDepth > 1 )
					uDepth = uDepth >> 1;
			}

			if ( fSize <= D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES )
				break;

			//if overexceeded max size then reduce dimensions
			if ( m_Depth >= m_Width && m_Depth >= m_Height )
				m_Depth = (UINT)(m_Depth - m_Depth*0.1f);
			else if ( m_Width >= m_Height && m_Width >= m_Depth )
			{
				m_Width = (UINT)(m_Width - m_Width*0.1f);

				if ( FF_BC & GetFlags( m_Format ))
					m_Width =  max( (m_Width /4)*4, 4 );

				if ( FF_STRIDED & GetFlags( m_Format ))
					m_Width =  max( (m_Width /2)*2, 2 );
			}
			else if ( m_Height >= m_Width && m_Height >= m_Depth && m_Height > 1 )
			{
				m_Height = (UINT)(m_Height - m_Height*0.1f);

				if ( FF_BC & GetFlags( m_Format ))
					m_Height =  max( (m_Height /4)*4, 4 );
			}
			else
				return false;

			//recalculate MipCount
			UINT32 MipCount = (UINT32) max( log((float)m_Width)/log(2.f) + 1, log((float)m_Height)/log(2.f) + 1);
			if ( MipCount < m_MipCount )
				m_MipCount = MipCount;
		}
		break;
	case RESOURCE_TEXTURECUBE:
	case RESOURCE_TEXTURECUBE_ARRAY:
		while ( true )
		{
			//calculate size of resource
			float fSize = 0;
			UINT uSize = 0;
			UINT uWidth = m_Width;
			for (UINT32 i = 0; i < MipCount; i++)
			{
				uSize = (uWidth * uWidth * 6 * uBitsPerElement + 7) / 8;
				fSize += uSize / 1024000.f;
				if ( uWidth > 1 )
					uWidth = uWidth >> 1;
			}

			if ( fSize <= D3D11_REQ_RESOURCE_SIZE_IN_MEGABYTES )
				break;

			//if overexceeded max size then reduce dimensions
			if ( m_Width > 1 )
			{
				m_Width = (UINT)(m_Width - m_Width*0.1f);

				if ( FF_BC & GetFlags( m_Format ))
					m_Width =  max( (m_Width /4)*4, 4 );

				if ( FF_STRIDED & GetFlags( m_Format ))
				{
					WriteToLog( _T("change width to %d"), m_Width);
					m_Width =  max( (m_Width /2)*2, 2 );
				}
			}
			else
				return false;

			//recalculate MipCount
			float MipCount = log((float)m_Width)/log(2.f) + 1;
			if ( MipCount < (float) m_MipCount )
				m_MipCount = (UINT32) MipCount;
		}
		break;
	default:
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------

bool CShader5xTest_ResourceAccess::CheckResult( void * pResult, UINT uRowPitch )
{

	BYTE * pOutputResult  = (BYTE *) pResult;
	bool bReturn = true;
	const bool bLogVerbose = ((CShader5xTestApp*)GetFramework())->LogVerbose();
	const bool bCube = m_Resource == RESOURCE_TEXTURECUBE_ARRAY || m_Resource == RESOURCE_TEXTURECUBE;
	const bool bPixelSample = ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL && InstAttributes.FullMatch(Opcode::Sample, Modifier::None, Modifier::ResidencyFeedback | Modifier::Clamp) && !TestingPerResourceMinLOD() );
	const bool bOutOfBoundsLD = InstAttributes.FullMatch(Opcode::Load, Modifier::None, Modifier::ResidencyFeedback) && (m_fResourceMinLODOffset >= 1.0f);

	UINT uBitsPerElementSRV = GetBitsPerElement( m_Format );
	UINT uBitsPerElementRTV = GetBitsPerElement( RES_RTFORMAT );

	int rscSwizzleIndices[4];
	m_ResourceSwizzle.ToIndices(rscSwizzleIndices);
	int addrSwizzleIndices[4];
	m_AddressSwizzle.ToIndices(addrSwizzleIndices);
	int offsetSwizzleIndices[4];
	m_OffsetSwizzle.ToIndices(offsetSwizzleIndices);

	UINT uWidth = DEFAULT_WIDTH;
	UINT uHeight = DEFAULT_HEIGHT;

	// If the primtive size was reduced, then reduce uWidth and uHeight so that only lit pixels are checked
	if( DecreasePrimitiveSize() )
	{
		uWidth >>= m_MipLevel;
		uHeight >>= m_MipLevel;
	}

	for (UINT j = 0; j < uHeight; j++)
	{
		for(UINT i = 0; i < uWidth; i++)
		{
			int texCoords[2] = {i,j};
			int x = texCoords[ addrSwizzleIndices[0] ];
			int y = texCoords[ addrSwizzleIndices[1] ];
			int z = m_ArraySlice; // arraySlice used for tex3d Z coordinate
			GetOffsetCoords( &x, &y, &z, (bPixelSample || bCube) ? 0 : m_MipLevel, 4, offsetSwizzleIndices );
			if( m_Resource == RESOURCE_TEXTURE1D || m_Resource == RESOURCE_TEXTURE1D_ARRAY )
				y = j;
			if ( m_Resource == RESOURCE_BUFFER)
				y = 0;
			if( m_Resource != RESOURCE_TEXTURE3D )
				z = 0;

			for(UINT k = 0; k < 4; k++) // four channels
			{
				float fOutput = ((float*)pOutputResult)[ k ];
				float fExpected;

				if ( m_Resource == RESOURCE_BUFFER)
				{
					if ( x < 0 || x >= (int)( m_Width / (uBitsPerElementSRV / 8) ) )
						fExpected = 0.f;
					else
					{
						BYTE* expectedResult = (BYTE*)m_pExpectedResult + x * (uBitsPerElementSRV / 8);

						if( m_bUsingFL10Format )
						{
							// 16-bit channels, need conversion to 32 bit
							fExpected = Float16Maker(((UINT16*)(expectedResult))[ rscSwizzleIndices[k] ]);
						}
						else
						{
							// 32-bit channels
							fExpected = ((float*)(expectedResult))[ rscSwizzleIndices[k] ];
						}
					}
				}
				else
				{
					if( x < 0 || x >= (int)m_Width || y < 0 || y >= (int)m_Height || z < 0 || z >= (int)max(1, m_Depth >> m_MipLevel) )
						fExpected = 0.f;
					else
					{
						BYTE* expectedResult = (BYTE*)m_pExpectedResult + y * (uBitsPerElementSRV / 8) * DEFAULT_WIDTH + x * (uBitsPerElementSRV / 8);

						if( m_bUsingFL10Format )
						{
							// 16-bit channels, need conversion to 32 bit
							fExpected = Float16Maker(((UINT16*)(expectedResult))[ rscSwizzleIndices[k] ]);
						}
						else
						{
							// 32-bit channels
							fExpected = ((float*)(expectedResult))[ rscSwizzleIndices[k] ];
						}
					}
				}

				//when m_MipLevel == (m_MipCount - 1)
				//and per-resource MinLOD is being tested, then
				//the MinLOD will be > (m_MipCount - 1), and hence the sampler should produce out-of-bounds behavior
				if ((m_fResourceMinLODOffset > 0.0f) && (m_MipLevel == (m_MipCount - 1)))
				{
					fExpected = 0.0f;
				}

				//The LD instruction can set the per-resource MinLOD to a value >= (m_MipLevel + 1.0f)
				//which will cause the LD instruction to return an out-of-bounds result
				if (bOutOfBoundsLD)
				{
					fExpected = 0.0f;
				}

				if ( m_DestMask != AssemblyHelper::noMask )
				{
					switch(k)
					{
					case 0: if (!(m_DestMask & AssemblyHelper::x)) fExpected = 0; break;
					case 1: if (!(m_DestMask & AssemblyHelper::y)) fExpected = 0; break;
					case 2: if (!(m_DestMask & AssemblyHelper::z)) fExpected = 0; break;
					case 3: if (!(m_DestMask & AssemblyHelper::w)) fExpected = 0; break;
					}
				}

				if( IsDenorm( fExpected ) )
					fExpected = 0.f;
				if( IsDenorm( fOutput ) )
					fOutput = 0.f;
				if ( !WithinOneULP( fExpected, fOutput ) && !( IsNaN( fOutput ) && IsNaN( fExpected ) ) )
				{
					if ( bLogVerbose || bReturn )
					{
						bReturn = false;
						WriteToLog( _T( "CShader5xTest_ResourceAccess::CheckResult - pixel at (%d, %d), component %d is different. \tExpected value is %f \tOutput value is %f" ), i, j, k, fExpected, fOutput );

					}
				}
			}
			pOutputResult += uBitsPerElementRTV / 8;
		}
		pOutputResult = ((BYTE*) pResult) + ( (j + 1) * uRowPitch);
	}

	return bReturn;
}

//-------------------------------------------------------------------------------

bool CShader5xTest_ResourceAccess::IsDenorm( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

//-------------------------------------------------------------------------------

bool CShader5xTest_ResourceAccess::IsNaN( float fValue )
{
	UINT32 uVal = *( (UINT32 *) &fValue ) & 0x7fffffff;
	return uVal >= 0x7f800001 && uVal <= 0x7fffffff;
}

//-------------------------------------------------------------------------------

void CShader5xTest_ResourceAccess::GetOffsetCoords( int *pX, int *pY, int *pZ, int mipLevel, UINT numBits, int offsetSwizzleIndices[4] )
{
	if( m_bTestOffset )
	{
		int offsetVals[4] = {m_nOffsets.x, m_nOffsets.y, m_nOffsets.z, 0};
		int offsetValsSwizzled[4];
		for (int n = 0; n < 4; ++n)
			offsetValsSwizzled[n] = offsetVals[ offsetSwizzleIndices[n] ];

		int * coords[ 3 ] = { pX, pY, pZ };
		int dimensions[ 3 ] = { DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_DEPTH };
		for( UINT i = 0; i < 3 && coords[ i ] != NULL; ++i )
		{
			// assuming border addressing mode
			/*int n = *(coords[ i ]) + m_nOffsets.i[ i ];
			if( n < 0 )
				*(coords[ i ]) = dimensions[ i ] + n;
			else if( n >= dimensions[ i ] )
				*(coords[ i ]) = n - dimensions[ i ];*/

			const int mipMultiplier = (i < 2) ? (1 << mipLevel) : 1;

			// only use the bottom "numBits" bits
			// sign extended
			offsetValsSwizzled[ i ] =  (offsetValsSwizzled[ i ] << (32 - numBits)) >> (32 - numBits);

			*(coords[ i ]) += mipMultiplier * offsetValsSwizzled[ i ];
		}
	}
}

//-------------------------------------------------------------------------------

bool CShader5xTest_ResourceAccess::WithinOneULP( float fReference, float fOutput )
{
	// WARNING:
	// This function does not correctly handle edge cases involving negative floats, zeros
	// and NaNs. If future test result discrepancies are discovered to have been masked by this function,
	// please start using CompareFloatULP() function in Framework\Core\FloatUtils.cpp which
	// does not suffer from these problems.

	if( fReference == fOutput )
		return true;

	UINT32 uRef = *( (UINT32*) &fReference );
	UINT32 uOut = *( (UINT32*) &fOutput );
	if( abs( INT32( uRef - uOut ) ) <= 1 )
		return true;

	return false;
}

bool CShader5xTest_ResourceAccess::GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight )
{
	if( uWidth == 0 || uHeight == 0 )
		return false;

	UINT uCurrMip = 0;
	while( uCurrMip < uMipSlice )
	{
		if( uWidth > 1 ) uWidth /= 2;
		if( uHeight > 1 ) uHeight /= 2;
		uCurrMip++;
	}

	return true;
}

//---------------------------------------------------------------

bool CShader5xTest_ResourceAccess::CreateStripes(	DXGI_FORMAT resFormat,
												 const UINT &uHeight,
												 const UINT &uWidth,
												 const UINT &uNumStripes,
												 FLOAT_COLOR *pColors,
												 const UINT &uNumColors,
												 void *pStripes,
												 bool bVertical )
{
	if( ( bVertical && uNumStripes > uWidth ) ||
		( !bVertical && uNumStripes > uHeight ) ||
		uNumStripes == 0 ||
		pStripes == NULL || pColors == NULL )
	{
		WriteToLog( _T( "CreateStripes() failed, invalid or null parameters passed" ) );
		return false;
	}

	UINT uStripeSize;
	if( bVertical )
		uStripeSize = uWidth / uNumStripes;
	else
		uStripeSize = uHeight / uNumStripes;

	FLOAT_COLOR *pStripesFloat = new FLOAT_COLOR[ uWidth * uHeight ];

	for( UINT uY = 0; uY < uHeight; ++uY )
	{
		for( UINT uX = 0; uX < uWidth; ++uX )
		{
			UINT uStep = bVertical ? uX : uY;
			pStripesFloat[ uY * uWidth + uX ] = pColors[ (uStep / uStripeSize) % uNumColors ];
		}
	}

	if( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, resFormat, pStripesFloat, uHeight, uWidth, pStripes ) )
	{
		WriteToLog( _T("CreateStripes() failed, call to CFormatHelper::ConvertPixels() failed") );
		delete [ ] pStripesFloat;
		return false;
	}

	delete [ ] pStripesFloat;

	return true;
}

//---------------------------------------------------------------

bool CShader5xTest_ResourceAccess::CreateGradient(	DXGI_FORMAT resFormat,
												  const UINT &uHeight,
												  const UINT &uWidth,
												  FLOAT_COLOR pColors[ 4 ],
												  void *pGradient )
{
	if( pColors == NULL || pGradient == NULL )
	{
		WriteToLog( _T( "CreateGradient() failed, null parameters passed" ) );
		return false;
	}

	FLOAT_COLOR *pGradientFloat = new FLOAT_COLOR[ uWidth * uHeight ];

	float dist[ 4 ];
	for( UINT uY = 0; uY < uHeight; ++uY )
	{
		for( UINT uX = 0; uX < uWidth; ++uX )
		{
			float uNormX = (float) uX / (float) uWidth;
			float uNormY = (float) uY / (float) uHeight;
			float uInvX = 1.f - uX;
			float uInvY = 1.f - uY;

			dist[ 0 ] = sqrt( (uNormX * uNormX) + (uNormY * uNormY) );
			dist[ 1 ] = sqrt( (uInvX * uInvX) + (uNormY * uNormY) );
			dist[ 2 ] = sqrt( (uNormX * uNormX) + (uInvY * uInvY) );
			dist[ 3 ] = sqrt( (uInvX * uInvX) + (uInvY * uInvY) );

			pGradientFloat[ uY * uWidth + uX ] = ( pColors[ 0 ] * dist[ 0 ] ) +
				( pColors[ 1 ] * dist[ 1 ] ) +
				( pColors[ 2 ] * dist[ 2 ] ) +
				( pColors[ 3 ] * dist[ 3 ] );
		}
	}

	if( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, resFormat, pGradientFloat, uHeight, uWidth, pGradient ) )
	{
		WriteToLog( _T("CreateGradient() failed, call to CFormatHelper::ConvertPixels() failed") );
		delete [ ] pGradientFloat;
		return false;
	}

	delete [ ] pGradientFloat;

	return true;
}

//---------------------------------------------------------------

bool CShader5xTest_ResourceAccess::CreateTiles(	DXGI_FORMAT resFormat,
											   const UINT &uHeight, const UINT &uWidth,
											   const UINT &uEdgeWidth,
											   FLOAT_COLOR *pColors, const UINT &uNumColors,
											   void *pTiles )
{
	if( uEdgeWidth > uHeight || uEdgeWidth > uWidth ||
		pColors == NULL || pTiles == NULL )
	{
		WriteToLog( _T( "CreateTiles() failed, invalid or null parameters passed" ) );
		return false;
	}

	FLOAT_COLOR *pTilesFloat = new FLOAT_COLOR[ uWidth * uHeight ];

	UINT uNumTilesX = uWidth / uEdgeWidth;
	UINT uNumTilesY = uHeight / uEdgeWidth;
	if( uWidth % uEdgeWidth )
		uNumTilesX++;
	if( uHeight % uEdgeWidth )
		uNumTilesY++;

	for( UINT uY = 0; uY < uHeight; ++uY )
	{
		for( UINT uX = 0; uX < uWidth; ++uX )
		{
			UINT uTileX = uX / uEdgeWidth;
			UINT uTileY = uY / uEdgeWidth;
			UINT uTileNum = uTileY * uNumTilesX + uTileX;
			pTilesFloat[ uY * uWidth + uX ] = pColors[ uTileNum % uNumColors ];
		}
	}

	if( !CFormatHelper::ConvertPixels( CONV_TO_FORMAT, resFormat, pTilesFloat, uHeight, uWidth, pTiles ) )
	{
		WriteToLog( _T("CreateTiles() failed, call to CFormatHelper::ConvertPixels() failed") );
		delete [ ] pTilesFloat;
		return false;
	}

	delete [ ] pTilesFloat;

	return true;
}

bool CShader5xTest_ResourceAccess::TestingPerResourceMinLOD()
{
	return m_fResourceMinLODOffset != 0.0f;
}




//-----------------------------------------------------------------------------
// CShader5xTest_resinfo
//-----------------------------------------------------------------------------

void CShader5xTest_resinfo::InitTestParameters()
{
	// UAV resources supported by pixel shader and compute shader
	CUserValueSet< D3D_SHADER_STAGE > *pShaderStagesUAV = new CUserValueSet< D3D_SHADER_STAGE >();
	pShaderStagesUAV->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStagesUAV->AddValue( D3D_SHADER_STAGE_COMPUTE );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor rfShaderStageUAV = AddParameterValueSet( _T(m_shaderStageParameterStr), pShaderStagesUAV );

	CTestCaseParameter< RESOURCE_TYPE > *pResTypeParam = AddParameter( "Resource", &m_Resource );
	testfactor::RFactor rfResBuffer     	= AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_BUFFER );
	testfactor::RFactor rfResTex1D      	= AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE1D );
	testfactor::RFactor rfResTex1DArray 	= AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE1D_ARRAY );
	testfactor::RFactor rfResTex2D      	= AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE2D );
	testfactor::RFactor rfResTex2DArray 	= AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE2D_ARRAY );
	testfactor::RFactor rfResTex3D      	= AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE3D );
	testfactor::RFactor rfResTexCube      	= AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURECUBE );
	testfactor::RFactor rfResTexCubeArray	= AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURECUBE_ARRAY );

	//we need to check resinfo values when resource is bound or not to the stage
	//for non-bound resource we'll only need to run one test case for each resource type and each of 3 resinfo instructions
	CTestCaseParameter<bool> *pBoundParam = AddParameter( _T("IsBound"), &m_bIsBound );
	testfactor::RFactor rfIsNotBound = AddParameterValue( pBoundParam, false );
	testfactor::RFactor rfIsBound	 = AddParameterValue( pBoundParam, true );

	// this parameter only applies to pixel shader and compute shader
	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );
	testfactor::RFactor rfIsUAV = AddParameterValue( pIsUAVResourceParam, true );
	testfactor::RFactor rfIsNotUAV = AddParameterValue( pIsUAVResourceParam, false );

	CTestCaseParameter<UINT> *pWidthParam = AddParameter<UINT>( _T( "Width" ), &m_Width );
	CTestCaseParameter<UINT> *pHeightParam = AddParameter<UINT>( _T( "Height" ), &m_Height );
	CTestCaseParameter<UINT> *pDepthParam = AddParameter<UINT>( _T( "Depth" ), &m_Depth );
	testfactor::RFactor rfWidth     = AddParameterValueSet<UINT>( pWidthParam,     GradientValueSet<UINT>( 0, 2, 3 ));
	testfactor::RFactor rfHeight    = AddParameterValueSet<UINT>( pHeightParam,    GradientValueSet<UINT>( 0, 2, 3 ));
	testfactor::RFactor rfDepth     = AddParameterValueSet<UINT>( pDepthParam,     GradientValueSet<UINT>( 0, 2, 3 ));
	SetParameterDefaultValue<UINT>( pWidthParam, 0 );
	SetParameterDefaultValue<UINT>( pHeightParam, 0 );
	SetParameterDefaultValue<UINT>( pDepthParam, 0 );

	CTestCaseParameter<UINT32> *pMipCountParam  = AddParameter<UINT32>( _T( "MipCount"  ), &m_MipCount );//,  GradientValueSet<UINT32>( 0, 0, 1 ));
	testfactor::RFactor rfMipCount  = AddParameterValueSet<UINT32>( pMipCountParam,  GradientValueSet<UINT32>( 0, 0, 1 ));
	SetParameterDefaultValue<UINT32>( pMipCountParam, 0 );

	CTestCaseParameter<int> *pMipLevelParam  = AddParameter<int>( _T( "MipLevel"  ), &m_MipLevel );//,  GradientValueSet( 0, 1, 2 ));
	testfactor::RFactor rfMipLevel  = AddParameterValueSet<int>( pMipLevelParam,  GradientValueSet( 0, 1, 2 ));
	SetParameterDefaultValue<int>( pMipLevelParam, 0 );

	CTestCaseParameter<UINT> *pArraySizeParam = AddParameter<UINT>( _T( "ArraySize" ), &m_ArraySize);
	testfactor::RFactor rfArraySize = AddParameterValueSet<UINT>( pArraySizeParam, GradientValueSet<UINT32>( 0, 0, 1 ));
	SetParameterDefaultValue<UINT>( pArraySizeParam, 0 );

	CTestCaseParameter<UINT> *pSwizzleParam = AddParameter<UINT>( _T( "Swizzle" ), &m_uSwizzle );
	CUserValueSet<UINT> *pSwizzleValues = new CUserValueSet<UINT>( );
	pSwizzleValues->AddValue( 0 );
	pSwizzleValues->AddValue( 1 );
	pSwizzleValues->AddValue( 2 );
	pSwizzleValues->AddValue( 3 );
	testfactor::RFactor rfSwizzle = AddParameterValueSet<UINT>( pSwizzleParam, pSwizzleValues );
	SetParameterDefaultValue<UINT>( pSwizzleParam, (UINT)0 );

	testfactor::RFactor rfTestCases = (
		( rfResTex1D	  	* (rfIsNotBound + rfIsBound * rfWidth * rfMipCount * rfMipLevel * rfSwizzle )) +
		( rfResTex1DArray 	* (rfIsNotBound + rfIsBound * rfArraySize * rfWidth * rfMipCount * rfMipLevel )) +
		( rfResTex2D	  	* (rfIsNotBound + rfIsBound * rfWidth * rfHeight * rfMipCount * rfMipLevel )) +
		( rfResTex2DArray 	* (rfIsNotBound + rfIsBound * rfArraySize * rfWidth * rfHeight * rfMipCount * rfMipLevel )) +
		( rfResTex3D	  	* (rfIsNotBound + rfIsBound * rfWidth * rfHeight * rfDepth * rfMipCount * rfMipLevel )) +
		( rfResTexCube	  	* (rfIsNotBound + rfIsBound * rfWidth * rfMipCount * rfMipLevel )) +
		( rfResTexCubeArray	* (rfIsNotBound + rfIsBound * rfWidth * rfArraySize * rfMipCount * rfMipLevel )));

	testfactor::RFactor rfTestCasesWithShaderStageNotUAV = rfIsNotUAV * GetTestFactorWithDefaultStageCoverage( rfTestCases );
	testfactor::RFactor rfTestCasesWithShaderStageUAV = rfIsUAV * rfShaderStageUAV * rfTestCases;

	SetRootTestFactor( rfTestCasesWithShaderStageNotUAV + rfTestCasesWithShaderStageUAV );

	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("Resource"), RESOURCE_BUFFER), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("Resource"), RESOURCE_TEXTURE2D), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("IsBound"), true), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_resinfo::Setup()
{
	TEST_RESULT tr = CShader5xTest_ResourceAccess::Setup();
	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
		return tr;

	return RESULT_PASS;

}


//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_resinfo::SetupTestCase()
{
	TEST_RESULT tr = CShader5xTest::SetupTestCase();
	if( RESULT_PASS != tr )
		return tr;

	// texture cube arrays only supported on 10.1 and above
	if( g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_1 && m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
	{
		return RESULT_SKIP;
	}

	if (!g_TestApp.IsFL11OrGreater() &&
		m_bIsUAV &&
		!(m_Resource == RESOURCE_BUFFER &&
		(m_bufType == BUFFER_RAW || m_bufType == BUFFER_STRUCTURED)))
	{
		return RESULT_SKIP;
	}

	// Compute shader stage uses raw buffer as output UAV
	m_RTFormat = RES_RTFORMAT;
	m_numComponentsRawOutput = 1;
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		m_RTFormat = DXGI_FORMAT_R32_TYPELESS;
		m_numComponentsRawOutput = 4; // ResourceAccess groups need 4-component result for each pixel/thread
	}

	if ( m_bIsBound )
	{
		m_MipCount = 1;
		m_MipLevel = 0;
		m_Width = DEFAULT_WIDTH;
		m_Height = DEFAULT_HEIGHT;
		m_Depth = DEFAULT_DEPTH;
		m_ArraySize = RES_ACCESS_ARRAY_SIZE;

		//create resource
		switch ( m_Resource )
		{
		/*case D3D11_RESOURCE_ERROR:
			return RESULT_FAIL;
			break;*/
		//we use this case to set NULL resource view
		case RESOURCE_BUFFER:
			break;
		case RESOURCE_TEXTURE1D:
		case RESOURCE_TEXTURE1D_ARRAY:
			tr = InitTexture1D( false, false );
			break;
		case RESOURCE_TEXTURE2D:
		case RESOURCE_TEXTURE2D_ARRAY:
		case RESOURCE_TEXTURECUBE:
		case RESOURCE_TEXTURECUBE_ARRAY:
			tr = InitTexture2D( false, false );
			break;
		case RESOURCE_TEXTURE3D:
			tr = InitTexture3D( false, false );
			break;
		default:
			tr = RESULT_FAIL;
			break;
		}
		if( tr == RESULT_SKIP || tr == RESULT_FAIL )
			return tr;
	}

	// create result render target or UAV
	tr = CreateResultRTVorUAV();
	if ( tr != RESULT_PASS )
	{
			WriteToLog( _T("CShader5xTest_resinfo::SetupTestCase - CreateResultRTVorUAV failed. ") );
			return RESULT_FAIL;
	}

	tr = SetupInputBuffer();
	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
		return tr;

	//set up resource to a shader
	UINT szTexStage = 0;//( GetFramework()->GetCurrentTestCase() - 1 )% MAX_TEXSTAGE;

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
		GetEffectiveContext()->VSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		GetEffectiveContext()->GSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_PIXEL:
		GetEffectiveContext()->PSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_HULL:
		GetEffectiveContext()->HSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		GetEffectiveContext()->DSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		GetEffectiveContext()->CSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_resinfo::SetupTestCase - ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	return RESULT_PASS;

}

//-----------------------------------------------------------------------------

bool CShader5xTest_resinfo::CheckResult( void * pResult, UINT uRowPitch )
{
	UINT nCaseNumber = GetFramework()->GetCurrentTestCase() - 1;

	if ( !streq( "_uint", szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ] ))
		return CheckFloatResult( pResult, uRowPitch );
	else
		return CheckUintResult( pResult, uRowPitch );
}

//-----------------------------------------------------------------------------

bool CShader5xTest_resinfo::CheckUintResult( void * pResult, UINT uRowPitch )
{
	UINT * pExpectedResult = (UINT *) m_pExpectedResult;
	UINT * pOutputResult  = (UINT *) pResult;
	UINT temp = pExpectedResult[ 0 ];
	UINT temp2 = pExpectedResult[ 3 ];
	switch( m_uSwizzle )
	{
	case 1: // gbar
		pExpectedResult[ 0 ] = pExpectedResult[ 1 ];
		pExpectedResult[ 1 ] = pExpectedResult[ 2 ];
		pExpectedResult[ 2 ] = pExpectedResult[ 3 ];
		pExpectedResult[ 3 ] = temp;
		break;
	case 2: // bagr
		pExpectedResult[ 0 ] = pExpectedResult[ 2 ];
		pExpectedResult[ 2 ] = pExpectedResult[ 1 ];
		pExpectedResult[ 1 ] = temp2;
		pExpectedResult[ 3 ] = temp;
		break;
	case 3: // argb
		pExpectedResult[ 3 ] = pExpectedResult[ 2 ];
		pExpectedResult[ 2 ] = pExpectedResult[ 1 ];
		pExpectedResult[ 1 ] = pExpectedResult[ 0 ];
		pExpectedResult[ 0 ] = temp2;
		break;
	default:
		break;
	}

	bool bReturn = true;

	if ( pOutputResult[ 0 ] != pExpectedResult[ 0 ] )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_resinfo::CheckUintResult - X/R channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %d\n" ), pExpectedResult[ 0 ] );
		WriteToLog( _T( "Output value is %d\n" ), pOutputResult[ 0 ] );
	}

	if ( pOutputResult[ 1 ] != pExpectedResult[ 1 ] )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_resinfo::CheckUintResult - Y/G channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %d\n" ), pExpectedResult[ 1 ] );
		WriteToLog( _T( "Output value is %d\n" ), pOutputResult[ 1 ] );
	}

	if ( pOutputResult[ 2 ] != pExpectedResult[ 2 ] && m_Resource != RESOURCE_TEXTURECUBE_ARRAY ) // texcubearray size undefined for D3D11/earlier resinfo
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_resinfo::CheckUintResult - Z/B channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %d\n" ), pExpectedResult[ 2 ] );
		WriteToLog( _T( "Output value is %d\n" ), pOutputResult[ 2 ] );
	}

	if ( pOutputResult[ 3 ] != pExpectedResult[ 3 ] )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_resinfo::CheckUintResult - W/A channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %d\n" ), pExpectedResult[ 3 ] );
		WriteToLog( _T( "Output value is %d\n" ), pOutputResult[ 3 ] );
	}

	return bReturn;
}

//-----------------------------------------------------------------------------

bool CShader5xTest_resinfo::CheckFloatResult( void * pResult, UINT uRowPitch )
{
	float * pExpectedResult = (float *) m_pExpectedResult;
	float * pOutputResult  = (float *) pResult;
	float temp = pExpectedResult[ 0 ];
	float temp2 = pExpectedResult[ 3 ];
	switch( m_uSwizzle )
	{
	case 1: // gbar
		pExpectedResult[ 0 ] = pExpectedResult[ 1 ];
		pExpectedResult[ 1 ] = pExpectedResult[ 2 ];
		pExpectedResult[ 2 ] = pExpectedResult[ 3 ];
		pExpectedResult[ 3 ] = temp;
		break;
	case 2: // bagr
		pExpectedResult[ 0 ] = pExpectedResult[ 2 ];
		pExpectedResult[ 2 ] = pExpectedResult[ 1 ];
		pExpectedResult[ 1 ] = temp2;
		pExpectedResult[ 3 ] = temp;
		break;
	case 3: // argb
		pExpectedResult[ 3 ] = pExpectedResult[ 2 ];
		pExpectedResult[ 2 ] = pExpectedResult[ 1 ];
		pExpectedResult[ 1 ] = pExpectedResult[ 0 ];
		pExpectedResult[ 0 ] = temp2;
		break;
	default:
		break;
	}

	bool bReturn = true;

	if ( !WithinOneULP( pExpectedResult[ 0 ],  pOutputResult[ 0 ] ) )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_resinfo::CheckFloatResult - X/R channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %.3f\n" ), pExpectedResult[ 0 ] );
		WriteToLog( _T( "Output value is %.3f\n" ), pOutputResult[ 0 ] );
	}

	if ( !WithinOneULP( pExpectedResult[ 1 ],  pOutputResult[ 1 ] ) )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_resinfo::CheckFloatResult - Y/G channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %.3f\n" ), pExpectedResult[ 1 ] );
		WriteToLog( _T( "Output value is %.3f\n" ), pOutputResult[ 1 ] );
	}

	if ( !WithinOneULP( pExpectedResult[ 2 ],  pOutputResult[ 2 ] ) && m_Resource != RESOURCE_TEXTURECUBE_ARRAY ) // texcubearray size undefined for D3D11/earlier resinfo
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_resinfo::CheckFloatResult - Z/B channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %.3f\n" ), pExpectedResult[ 2 ] );
		WriteToLog( _T( "Output value is %.3f\n" ), pOutputResult[ 2 ] );
	}

	if ( !WithinOneULP( pExpectedResult[ 3 ],  pOutputResult[ 3 ] ) )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_resinfo::CheckFloatResult - W/A channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %.3f\n" ), pExpectedResult[ 3 ] );
		WriteToLog( _T( "Output value is %.3f\n" ), pOutputResult[ 3 ] );
	}

	return bReturn;
}

//-----------------------------------------------------------------------------

tstring CShader5xTest_resinfo::GetShaderCode(AssemblyHelper* pAsmHelper)
{
	bool bRIsUsed = true;
	bool bGIsUsed = true;
	bool bBIsUsed = true;
	bool bAIsUsed = true;
	UINT width = 0, height = 0, depth = 0;

	UINT nCaseNumber = GetFramework()->GetCurrentTestCase() - 1;
	//UINT szTexStage  = nCaseNumber % MAX_TEXSTAGE;
	UINT uTexStage = 0;

	// UAV resource should not conflict with the UAV output slots for Compute in asmHelper.
	// D3D11 Assembler Error: Invalid Bytecode: PS ouputs must be declared in slots less than UAVs
	// UINT uUAVSlot = pAsmHelper->GetNumOutputReg();
	UINT uUAVSlot = 7;

	//BUGBUG - should use returnType(s) based on resource format
	switch ( m_Resource )
	{
	/*case D3D11_RESOURCE_ERROR:
		return false;
		break;*/
	case RESOURCE_BUFFER:
		//we use this case to set NULL resource view
		pAsmHelper->SetResourceDecl(uTexStage, "buffer, float");
		break;
	case RESOURCE_TEXTURE1D:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_typed u%d, Texture1D, float", uUAVSlot ) );
		else
			pAsmHelper->SetResourceDecl(uTexStage, "Texture1D, float");
		width = max( 1, m_Width >> m_MipLevel);
		bGIsUsed = false;
		bBIsUsed = false;
		break;
	case RESOURCE_TEXTURE1D_ARRAY:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_typed u%d, Texture1DArray, float", uUAVSlot ) );
		else
			pAsmHelper->SetResourceDecl(uTexStage, "Texture1DArray, float");
		width = max( 1, m_Width >> m_MipLevel);
		height = m_ArraySize;
		bBIsUsed = false;
		break;
	case RESOURCE_TEXTURE2D:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_typed u%d, Texture2D, float", uUAVSlot ) );
		else
			pAsmHelper->SetResourceDecl(uTexStage, "Texture2D, float");
		width  = max( 1, m_Width >> m_MipLevel);
		height = max( 1, m_Height >> m_MipLevel);
		bBIsUsed = false;
		break;
	case RESOURCE_TEXTURE2D_ARRAY:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_typed u%d, Texture2DArray, float", uUAVSlot ) );
		else
			pAsmHelper->SetResourceDecl(uTexStage, "Texture2DArray, float");
		width  = max( 1, m_Width >> m_MipLevel);
		height = max( 1, m_Height >> m_MipLevel);
		depth  = m_ArraySize;
		break;
	case RESOURCE_TEXTURECUBE_ARRAY:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_typed u%d, Texture2DArray, float", uUAVSlot ) );
		else
			pAsmHelper->SetResourceDecl(uTexStage, "TextureCubeArray, float");
		width  = max( 1, m_Width >> m_MipLevel);
		height = width;
		depth  = m_ArraySize;
		break;
	case RESOURCE_TEXTURE3D:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_typed u%d, Texture3D, float", uUAVSlot ) );
		else
			pAsmHelper->SetResourceDecl(uTexStage, "Texture3D, float");
		width  = max( 1, m_Width >> m_MipLevel);
		height = max( 1, m_Height >> m_MipLevel);
		depth  = max( 1, m_Depth >> m_MipLevel);
		break;
	case RESOURCE_TEXTURECUBE:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_typed u%d, Texture2DArray, float", uUAVSlot ) );
		else
			pAsmHelper->SetResourceDecl(uTexStage, "TextureCube, float");
		width = max( 1, m_Width >> m_MipLevel);
		height = width;
		depth = 0;
		bBIsUsed = false;
		break;
	default:
		return false;
	}

	//calculate expected result
	//if MipLevel is outside [0, m_MipCount)
	//when the resouce is not bound, the result should be the same as out-of-bound miplevel
	if ( m_MipLevel >= (int) m_MipCount || m_MipLevel < 0 )
	{
		//_snprintf( szShaderCode, MAX_STR  - 1, "%smov r1, vec4(0.f, 0.f, 0.f, %d.f )\n", m_MipCount );
		// resinfo_rcpFloat returns (INF, INF, INF, m_MipCount)
		if ( strstr( szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], "_rcpFloat" ))
		{
			((UINT *)m_pExpectedResult)[ 0 ] = 0x7F800000;
			((UINT *)m_pExpectedResult)[ 1 ] = bGIsUsed ? (m_Resource != RESOURCE_TEXTURE1D_ARRAY ? 0x7F800000 : m_ArraySize ) : 0;
			((UINT *)m_pExpectedResult)[ 2 ] = bBIsUsed ? (m_Resource != RESOURCE_TEXTURE2D_ARRAY && m_Resource != RESOURCE_TEXTURECUBE_ARRAY ? 0x7F800000 : m_ArraySize ) : 0;
			((float *) m_pExpectedResult)[ 3 ] = (float) m_MipCount;
		}
		// resinfo_uint returns (0, 0, 0, m_MipCount)
		else if ( strstr( szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], "_uint" ))
		{
			((UINT *)m_pExpectedResult)[ 0 ] = 0;
			((UINT *)m_pExpectedResult)[ 1 ] = 0;
			((UINT *)m_pExpectedResult)[ 2 ] = 0;
			((UINT *)m_pExpectedResult)[ 3 ] = m_MipCount;
		}
		// resinfo returns (0.f, 0.f, 0.f, m_MipCount)
		else
		{
			((float *)m_pExpectedResult)[ 0 ] = 0;
			((float *)m_pExpectedResult)[ 1 ] = 0;
			((float *)m_pExpectedResult)[ 2 ] = 0;
			((float *)m_pExpectedResult)[ 3 ] = (float) m_MipCount;
		}
	}
	//return values for resinfo_rcpFloat if MipLevel is inside [0, m_MipCount)
	else if ( strstr( szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], "_rcpFloat" ))
	{
		((float *)m_pExpectedResult)[ 0 ] = 1.f / width;
		((float *)m_pExpectedResult)[ 1 ] = bGIsUsed ? (m_Resource != RESOURCE_TEXTURE1D_ARRAY ? 1.f / height : m_ArraySize ) : 0;
		((float *)m_pExpectedResult)[ 2 ] = bBIsUsed ? (m_Resource != RESOURCE_TEXTURE2D_ARRAY && m_Resource != RESOURCE_TEXTURECUBE_ARRAY ? 1.f / depth : m_ArraySize ) : 0;
		((float *)m_pExpectedResult)[ 3 ] = (float) m_MipCount;
	}
	//return values for resinfo_uint if MipLevel is inside [0, m_MipCount)
	else if ( strstr( szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], "_uint" ))
	{
		((UINT *)m_pExpectedResult)[ 0 ] = (UINT)width;
		((UINT *)m_pExpectedResult)[ 1 ] = (UINT)height;
		((UINT *)m_pExpectedResult)[ 2 ] = (UINT)depth;
		((UINT *)m_pExpectedResult)[ 3 ] = m_MipCount;
	}
	//return values for resinfo if MipLevel is inside [0, m_MipCount)
	else
	{
		((float *)m_pExpectedResult)[ 0 ] = (float) width;
		((float *)m_pExpectedResult)[ 1 ] = (float) height;
		((float *)m_pExpectedResult)[ 2 ] = (float) depth;
		((float *)m_pExpectedResult)[ 3 ] = (float) m_MipCount;
	}

	// UAV can only have a tex2DArray view of a texCube, Z returns 1
	if( m_bIsUAV && m_bIsBound && m_Resource == RESOURCE_TEXTURECUBE )
	{
		if ( strstr( szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], "_uint" ))
		{
			((UINT *)m_pExpectedResult)[ 2 ] = 1;
		}
		else
		{
			((float *)m_pExpectedResult)[ 2 ] = 1.0f;
		}
	}

	tstring retStr;

	//set up mip level
	char szMipLevel[ 50 ];
	if (nCaseNumber % 5 == 0)
	{
		//use immediate value
		_snprintf( szMipLevel, 50 - 1, " %d", m_MipLevel );
	}
	else
	{
		retStr += FormatString( "mov r0, vec4(%d, %d, %d, %d )\n", m_MipLevel, m_MipLevel, m_MipLevel, m_MipLevel );
		switch( nCaseNumber % 5 )
		{
		case 1:
			_snprintf( szMipLevel, 49, "r0.x" );
			break;
		case 2:
			_snprintf( szMipLevel, 49, "r0.y" );
			break;
		case 3:
			_snprintf( szMipLevel, 49, "r0.z" );
			break;
		case 4:
			_snprintf( szMipLevel, 49, "r0.w" );
			break;
		}
	}

	retStr += "mov r4, vec4(-1.0, -1.0, -1.0, -1.0)\n";
	if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
	{
		if( m_Resource == RESOURCE_TEXTURE1D )
			retStr += FormatString( "resinfo%s r4, %s, u%d.%s\n", szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], szMipLevel, uUAVSlot, g_szSwizzle[ m_uSwizzle ] );
		else
			retStr += FormatString( "resinfo%s r4, %s, u%d\n", szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], szMipLevel, uUAVSlot );
	}
	else
	{
		if( m_Resource == RESOURCE_TEXTURE1D )
			retStr += FormatString( "resinfo%s r4, %s, t%d.%s\n", szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], szMipLevel, uTexStage, g_szSwizzle[ m_uSwizzle ] );
		else
			retStr += FormatString( "resinfo%s r4, %s, t%d\n", szResInfoSuffix[ nCaseNumber%SUFFIX_COUNT ], szMipLevel, uTexStage );
	}

	return retStr;
}



//-----------------------------------------------------------------------------
//	CShader5xTest_ld class functions
//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_ld_base::InitResource()
{
	//create resource
	switch ( m_Resource )
	{
	case RESOURCE_BUFFER:
		return InitBufferRes( true );
		break;
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		return InitTexture1D( true, true );
		break;
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
		return InitTexture2D( true, true );
		break;
	case RESOURCE_TEXTURE3D:
		return InitTexture3D( true, true );
		break;
	default:
		return RESULT_FAIL;
		break;
	}
}


//-----------------------------------------------------------------------------
//	CShader5xTest_sample_d class functions
//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_sample_d_base::InitResource()
{
	//create resource
	switch ( m_Resource )
	{
	//we use this case to set NULL resource view
	//BUGBUG add case for Buffer
	case RESOURCE_BUFFER:
		return RESULT_PASS;
		break;
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		return InitTexture1D( true, true );
		break;
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
	case RESOURCE_TEXTURECUBE:
	case RESOURCE_TEXTURECUBE_ARRAY:
		return InitTexture2D( true, true );
		break;
	case RESOURCE_TEXTURE3D:
		return InitTexture3D( true, true );
		break;
	default:
		return RESULT_FAIL;
		break;
	}
}

//-----------------------------------------------------------------------------
//	CShader5xTest_sample_l class functions
//-----------------------------------------------------------------------------

TEST_RESULT CShader5xTest_sample_l_base::InitResource()
{
	//create resource
	switch ( m_Resource )
	{
	//we use this case to set NULL resource view
	//BUGBUG add case for Buffer
	case RESOURCE_BUFFER:
		return RESULT_PASS;
		break;
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		return InitTexture1D( true, true );
		break;
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
	case RESOURCE_TEXTURECUBE:
	case RESOURCE_TEXTURECUBE_ARRAY:
		return InitTexture2D( true, true );
		break;
	case RESOURCE_TEXTURE3D:
		return InitTexture3D( true, true );
		break;
	default:
		return RESULT_FAIL;
		break;
	}
}


//-----------------------------------------------------------------------------
//	CShader5xTest_Comparison class functions
//-----------------------------------------------------------------------------

CShader5xTest_Comparison::CShader5xTest_Comparison( const TCHAR* Instruction, bool bTestPerResourceMinLOD ) : CShader5xTest_ResourceAccess( Instruction, bTestPerResourceMinLOD )
{
	m_Resource = RESOURCE_TEXTURE2D;
	m_bIsBound = true;
	m_RTFormat = COMP_RTFORMAT;
	m_Format = COMP_RTFORMAT;
	m_nSamplerComponent = 0;
}

//-----------------------------------------------------------------------------

void CShader5xTest_Comparison::InitTestParameters()
{
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor rfPSStage = AddParameterValue< D3D_SHADER_STAGE >( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );

	CTestCaseParameter< RESOURCE_TYPE > *pResTypeParam = AddParameter( _T( "Resource" ), &m_Resource );
	testfactor::RFactor rfResTex2D      = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE2D		  );
	testfactor::RFactor rfResTex2DArray = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE2D_ARRAY );
	testfactor::RFactor rfResTexCube    = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURECUBE	  );
	testfactor::RFactor rfResTexCubeArray    = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURECUBE_ARRAY	  );

	// test parameter for comparison functions
	CUserValueSet< D3D11_COMPARISON_FUNC > *pCompFuncValues = new CUserValueSet< D3D11_COMPARISON_FUNC >();
	pCompFuncValues->AddValue( D3D11_COMPARISON_NEVER );
	pCompFuncValues->AddValue( D3D11_COMPARISON_LESS );
	pCompFuncValues->AddValue( D3D11_COMPARISON_EQUAL );
	pCompFuncValues->AddValue( D3D11_COMPARISON_LESS_EQUAL );
	pCompFuncValues->AddValue( D3D11_COMPARISON_GREATER );
	pCompFuncValues->AddValue( D3D11_COMPARISON_NOT_EQUAL );
	pCompFuncValues->AddValue( D3D11_COMPARISON_GREATER_EQUAL );
	pCompFuncValues->AddValue( D3D11_COMPARISON_ALWAYS );
	CTestCaseParameter< D3D11_COMPARISON_FUNC > *pCompFuncParam = AddParameter( _T( "ComparisonFunc" ), &m_ComparisonFunc );
	SetParameterDefaultValue( pCompFuncParam, D3D11_COMPARISON_NEVER );
	testfactor::RFactor rfCompFunc = AddParameterValueSet( pCompFuncParam, pCompFuncValues );

	DWORD dwInfP = 0x7f800000;	// +INF
	DWORD dwInfN = 0xff800000;  // -INF
	DWORD dwNaN = 0x7f800001;	// NaN

	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );

	CUserValueSet< float > *pReferenceValues = new CUserValueSet< float >( );
	pReferenceValues->AddValue( 0.f );
	pReferenceValues->AddValue( 1.f );
	pReferenceValues->AddValue( -1.f );
	pReferenceValues->AddValue( (float)(rand() % 1000) );
	pReferenceValues->AddValue( -(float)(rand() % 1000) );
	pReferenceValues->AddValue( *( (float*) &dwInfP ));
	pReferenceValues->AddValue( *( (float*) &dwInfN ));
	pReferenceValues->AddValue( *( (float*) &dwNaN ));
	CTestCaseParameter< float > *pReference = AddParameter( _T( "ReferenceValue" ), &m_fReferenceValue );
	testfactor::RFactor rfReference = AddParameterValueSet< float >( pReference, pReferenceValues );

	SetParameterDefaultValue( _T(m_shaderStageParameterStr), D3D_SHADER_STAGE_PIXEL );
	SetParameterDefaultValue( pResTypeParam, RESOURCE_TEXTURE2D );

	CTestCaseParameter< int > *pMipLevelParam = AddParameter( _T( "MipLevel" ), &m_MipLevel);
	SetParameterDefaultValue( pMipLevelParam, 0 );

	testfactor::RFactor rfMipLevel;

	if( m_bTestResourceMinLOD )
	{
		rfMipLevel = AddParameterValueSet< int >( pMipLevelParam, GradientValueSet< int >( 0, 5, 6 ));
	}
	else
	{
		rfMipLevel = AddParameterValueSet< int >( pMipLevelParam, new CUserValueSet< int >( 0 ));
	}


	CTestCaseParameter<float> *pResourceMinLODParam = AddParameter( _T( "ResourceMinLODOffset" ), &m_fResourceMinLODOffset );
	SetParameterDefaultValue( pResourceMinLODParam, 0.0f );
	testfactor::RFactor rfResourceMinLOD;

	// Only test resource min lod for the test cases that explictly request it
	if ( m_bTestResourceMinLOD )
	{
		rfResourceMinLOD = AddParameterValueSet<float>( pResourceMinLODParam, new CTableValueSet<float>( g_fLODOffsetTable, sizeof( g_fLODOffsetTable[0] ), ARRAYSIZE( g_fLODOffsetTable ) ) );
	}
	else
	{
		rfResourceMinLOD = AddParameterValueSet<float>( pResourceMinLODParam, new CUserValueSet<float>( 0.0f ) );
	}

	const bool bPSOnly = InstAttributes.FullMatch(Opcode::Sample, Modifier::Comparison, Modifier::ResidencyFeedback | Modifier::Clamp);

	// Define a cross factor between the existing test factors and the shader stage
	testfactor::RFactor resourceType = rfResTex2D + rfResTex2DArray + rfResTexCube + rfResTexCubeArray;
	testfactor::RFactor compFuncAndReferenceValue = rfCompFunc * rfReference;
	testfactor::RFactor mipLevelsAndMinLOD = rfMipLevel * rfResourceMinLOD;

	testfactor::XFACTOR resourcePairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{resourceType, 1, NULL, 0},
		{compFuncAndReferenceValue, 1, NULL, 0},
		{mipLevelsAndMinLOD, 1, NULL, 0},
	};

	size_t groupOrders[] =
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Create the pairwise factor
	testfactor::RFactor rfTestCases = NewCrossFactor(resourcePairwiseTable, _countof(resourcePairwiseTable), "PairwiseRoot", groupOrders, _countof(groupOrders));

	if( bPSOnly )
	{
		SetRootTestFactor( rfPSStage * rfTestCases );
	}
	else
	{
		testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );
		SetRootTestFactor( rfTestCasesWithShaderStages );
	}

	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("Resource"), RESOURCE_TEXTURE2D), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<int>(_T("MipLevel"), 0), WeightFactorLarge);

	// Out-of-bounds are not priority 1
	// This is out-of-bounds because 5 is the max mip level defined in the test
	AddPriorityPatternFilter(	FilterEqual<int>(_T("MipLevel"), 5) &&
								FilterGreater<float>(_T("ResourceMinLODOffset"), 0.0f), WeightFactorVerySmall);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//----------------------------------------------------------------------------

bool CShader5xTest_Comparison::CheckResult( void * pResult, UINT uRowPitch )
{
	bool bSuccess = true;
	float *pOutputResult = (float*) pResult;
	float *pTexture = (float*) m_pExpectedResult;
	const bool bLogVerbose = ((CShader5xTestApp*)GetFramework())->LogVerbose();
	const bool bCube = m_Resource == RESOURCE_TEXTURECUBE_ARRAY || m_Resource == RESOURCE_TEXTURECUBE;

	int rscSwizzleIndices[4];
	m_ResourceSwizzle.ToIndices(rscSwizzleIndices);
	int addrSwizzleIndices[4];
	m_AddressSwizzle.ToIndices(addrSwizzleIndices);
	int offsetSwizzleIndices[4];
	m_OffsetSwizzle.ToIndices(offsetSwizzleIndices);

	UINT uWidth = DEFAULT_WIDTH;
	UINT uHeight = DEFAULT_HEIGHT;

	// If the primtive size was reduced, then reduce uWidth and uHeight so that only lit pixels are checked
	if( DecreasePrimitiveSize() )
	{
		uWidth >>= m_MipLevel;
		uHeight >>= m_MipLevel;
	}

	for (UINT j = 0; j < uHeight; j++)
	{
		for(UINT i = 0; i < uWidth; i++)
		{
			int texCoords[2] = {i,j};
			int xTex = texCoords[ addrSwizzleIndices[0] ];
			int yTex = texCoords[ addrSwizzleIndices[1] ];
			GetOffsetCoords( &xTex, &yTex, NULL, bCube ? 0 : m_MipLevel, 4, offsetSwizzleIndices );
			int texIdx = yTex * DEFAULT_WIDTH + xTex;
			float shaderResult = pOutputResult[ j * ( uRowPitch / sizeof(float) ) + i ];
			float expectedResult = pTexture[ texIdx ];

			if (!(m_DestMask & AssemblyHelper::x))
				expectedResult = 0;

			//when m_MipLevel == (m_MipCount - 1)
			//and per-resource MinLOD is being tested, then
			//the MinLOD will be > (m_MipCount - 1), and hence the sampler should produce out-of-bounds behavior
			if ((m_fResourceMinLODOffset > 0.0f) && (m_MipLevel == (m_MipCount - 1)))
			{
				expectedResult = 0.0f;
			}

			if( !CompareValues( m_ComparisonFunc, m_fReferenceValue, expectedResult, shaderResult, bSuccess || bLogVerbose ) )
			{
				bSuccess = false;
			}
		}

	}

	return bSuccess;
}

TEST_RESULT CShader5xTest_sample_c_lz_min_lod::InitResource()
{
	//create resource
	switch ( m_Resource )
	{
	//we use this case to set NULL resource view
	//BUGBUG add case for Buffer
	case RESOURCE_BUFFER:
		return RESULT_PASS;
		break;
	case RESOURCE_TEXTURE1D:
	case RESOURCE_TEXTURE1D_ARRAY:
		return InitTexture1D( true, true );
		break;
	case RESOURCE_TEXTURE2D:
	case RESOURCE_TEXTURE2D_ARRAY:
	case RESOURCE_TEXTURECUBE:
	case RESOURCE_TEXTURECUBE_ARRAY:
		return InitTexture2D( true, true );
		break;
	case RESOURCE_TEXTURE3D:
		return InitTexture3D( true, true );
		break;
	default:
		return RESULT_FAIL;
		break;
	}
}

// CShader5xTest_sample_c_lz_min_lod always uses SetResourceMinLOD
// to force the hardware to sample from m_MipLevel
bool CShader5xTest_sample_c_lz_min_lod::TestingPerResourceMinLOD()
{
	return true;
}

//-----------------------------------------------------------------------------
//
// All test cases (except LD) are configured so that m_MipLevel is the mip level that is finally chosen by the hardware.
// therefore the value returned here will always be < (m_MipLevel + 0.5)
// to ensure that the the mip level specified by m_MipLevel is always resident
// And finally, the value passed to SetResourceMinLOD is always >= (float)m_MipLevel to ensure that (m_MipLevel-1) is not allowed to be used
//
void CShader5xTest_ResourceAccess::SetResourceMinLOD(ID3D11Resource* pResource)
{
	assert(m_fResourceMinLODOffset >= 0.0f);

	// SetResourceMinLOD is only called for some test cases.  This allows these test cases to work on feature level < 11.0
	if( TestingPerResourceMinLOD() )
	{
		assert( m_bTestResourceMinLOD );

		const float MinLOD = m_MipLevel + m_fResourceMinLODOffset;

		GetImmediateContext()->SetResourceMinLOD(pResource, MinLOD);
	}
}

//-------------------------------------------------------------------------------

bool CShader5xTest_ResourceAccess::CompareValues(D3D11_COMPARISON_FUNC func,
												 float refVal,
												 float fValue,
												 float fOutput,
												 bool bLog )
{
	float fResult = 0.f;
	bool bResult = true;

	switch( func )
	{
	case D3D11_COMPARISON_NEVER:
		break;
	case D3D11_COMPARISON_LESS:
		fResult = ( (refVal < fValue) ? 1.f : 0.f );
		break;
	case D3D11_COMPARISON_EQUAL:
		fResult = ( (refVal == fValue ) ? 1.f : 0.f );
		break;
	case D3D11_COMPARISON_LESS_EQUAL:
		fResult = ( (refVal <= fValue) ? 1.f : 0.f );
		break;
	case D3D11_COMPARISON_GREATER:
		fResult = ( (refVal > fValue) ? 1.f : 0.f );
		break;
	case D3D11_COMPARISON_NOT_EQUAL:
		fResult = ( ( refVal != fValue ) ? 1.f : 0.f );
		break;
	case D3D11_COMPARISON_GREATER_EQUAL:
		fResult = ( (refVal >= fValue) ? 1.f : 0.f );
		break;
	case D3D11_COMPARISON_ALWAYS:
		fResult = 1.0f;
		break;
	}

	if( !WithinOneULP( fResult, fOutput ) && bLog )
	{
		bResult = false;
		WriteToLog( _T( "CShader5xTest_Comparison::CompareValues - Red component does not match" ) );
		WriteToLog( _T( "\tExpected value is %f" ), fResult );
		WriteToLog( _T( "\tOutput value is   %f" ), fOutput );
		WriteToLog( _T( "\tTexture value is  %f" ), fValue );
	}

	return bResult;
}

//-------------------------------------------------------------------------------

void CShader5xTest_sample_b_base::InitTestParameters()
{
	CTestCaseParameter<int> *pArraySliceParam = AddParameter<int>( _T( "ArraySlice" ), &m_ArraySlice );
	CUserValueSet<int> *pArrayValues = new CUserValueSet<int>( );
	pArrayValues->AddValue( 0 );
	pArrayValues->AddValue( RES_ACCESS_ARRAY_SIZE - 2 );
	pArrayValues->AddValue( RES_ACCESS_ARRAY_SIZE - 1 );
	testfactor::RFactor rfArray = AddParameterValueSet( pArraySliceParam, pArrayValues );

	CTestCaseParameter<int> *pMipSliceParam = AddParameter<int>( _T( "MipSlice" ), &m_MipLevel );
	CUserValueSet<int> *pMipValues = new CUserValueSet<int>( );
	pMipValues->AddValue( 0 );
	pMipValues->AddValue( MAX_LOD - 2 );
	pMipValues->AddValue( MAX_LOD - 1 );
	testfactor::RFactor rfMip = AddParameterValueSet( pMipSliceParam, pMipValues );

	CTestCaseParameter<float> *pLODBiasParam = AddParameter<float>( _T( "MipLODBias" ), &m_fMipLODBias );
	CUserValueSet<float> *pLODBiasValues = new CUserValueSet<float>( );
	pLODBiasValues->AddValue( 0.f );
	pLODBiasValues->AddValue( 1.f );
	pLODBiasValues->AddValue( (float) MAX_LOD );
	testfactor::RFactor rfLODBias = AddParameterValueSet( pLODBiasParam, pLODBiasValues );

	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );

	CTestCaseParameter<float> *pInstBiasParam = AddParameter<float>( _T( "InstructionBias" ), &m_fInstBias );
	CUserValueSet<float> *pInstBiasValues = new CUserValueSet<float>( );
	pInstBiasValues->AddValue( 0.f );
	pInstBiasValues->AddValue( 1.f );
	pInstBiasValues->AddValue( (float)MAX_LOD );
	pInstBiasValues->AddValue( 15.99f );
	pInstBiasValues->AddValue( -1.f );
	pInstBiasValues->AddValue( (float)-MAX_LOD );
	pInstBiasValues->AddValue( -16.f );
	testfactor::RFactor rfInstBias = AddParameterValueSet( pInstBiasParam, pInstBiasValues );

	CTestCaseParameter<float> *pResourceMinLODParam = AddParameter<float>( _T( "ResourceMinLOD" ), &m_fMinLOD );
	CUserValueSet<float> *pResourceMinLODValues = new CUserValueSet<float>( );
	pResourceMinLODValues->AddValue( 0.f );

	if (m_bTestResourceMinLOD )
	{
		pResourceMinLODValues->AddValue( 1.f );
		pResourceMinLODValues->AddValue( 2.0f );
		pResourceMinLODValues->AddValue( 5.0f );
		pResourceMinLODValues->AddValue( 6.0f );
		pResourceMinLODValues->AddValue( 7.0f );
	}

	testfactor::RFactor rfResourceMinLOD = AddParameterValueSet( pResourceMinLODParam, pResourceMinLODValues );

	SetRootTestFactor( rfArray * rfMip * rfLODBias * rfInstBias * rfResourceMinLOD );
}

TEST_RESULT CShader5xTest_sample_b_base::InitResource()
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize			= RES_ACCESS_ARRAY_SIZE;
	texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags		= 0;
	texDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Height				= DEFAULT_HEIGHT;
	texDesc.Width				= DEFAULT_WIDTH;
	texDesc.MipLevels			= 0;
	texDesc.MiscFlags			= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Usage				= D3D11_USAGE_IMMUTABLE;

	if ( m_fMinLOD != 0.0f )
	{
		texDesc.MiscFlags |= D3D11_RESOURCE_MISC_RESOURCE_CLAMP;
	}

	// size not exact but always more than enough, assuming DXGI_FORMAT_R32G32B32A32_FLOAT
	m_pResourceData = new float[ RES_ACCESS_ARRAY_SIZE * RES_ACCESS_MIP_LEVELS * DEFAULT_HEIGHT * DEFAULT_WIDTH * 4 ];
	D3D11_SUBRESOURCE_DATA pSubResData [ RES_ACCESS_ARRAY_SIZE * RES_ACCESS_MIP_LEVELS ];

	UINT iComp = 0;
	for( UINT iA = 0; iA < RES_ACCESS_ARRAY_SIZE; ++iA )
	{
		for( UINT iM = 0; iM < RES_ACCESS_MIP_LEVELS; ++iM )
		{
			UINT uWidth = DEFAULT_WIDTH;
			UINT uHeight = DEFAULT_HEIGHT;
			GetMipDimensions( iM, uWidth, uHeight );

			// assuming DXGI_FORMAT_R32G32B32A32_FLOAT
			UINT uSize = uWidth * uHeight * 4;
			UINT uSub = ( iA * RES_ACCESS_MIP_LEVELS ) + iM;
			pSubResData[ uSub ].pSysMem = &(m_pResourceData[ iComp ]);
			pSubResData[ uSub ].SysMemPitch = uWidth * 4 * sizeof( float );
			pSubResData[ uSub ].SysMemSlicePitch = 0;

			// values in color components set to current subresource number
			for( UINT i = 0; i < uSize; ++i, ++iComp )
				m_pResourceData[ iComp ] = (float)uSub;
		}
	}

	SAFE_RELEASE( m_pTexture2D );
	HRESULT hr;
	if( FAILED( hr = GetDevice()->CreateTexture2D( &texDesc, pSubResData, &m_pTexture2D ) ) )
	{
		WriteToLog( _T( "CShader5xTest_sample_b::InitResource - CreateTexture2D() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	delete [ ] m_pResourceData;

	if ( m_fMinLOD != 0.0f )
	{
		GetImmediateContext()->SetResourceMinLOD( m_pTexture2D, m_fMinLOD );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.ArraySize = 1;
	viewDesc.Texture2DArray.FirstArraySlice = m_ArraySlice;
	viewDesc.Texture2DArray.MipLevels = RES_ACCESS_MIP_LEVELS;
	viewDesc.Texture2DArray.MostDetailedMip = 0;

	if( FAILED( hr = GetDevice()->CreateShaderResourceView( m_pTexture2D, &viewDesc, &m_pSRV ) ) )
	{
		WriteToLog( _T( "CShader5xTest_sample_b::InitResource - CreateShaderResourceView() failed. hr = %s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	GetEffectiveContext()->PSSetShaderResources( 0, 1, (ID3D11ShaderResourceView**)&m_pSRV );

	return RESULT_PASS;
}

bool CShader5xTest_sample_b_base::CheckResult( void *pResult, UINT uRowPitch )
{
	float fLOD = m_MipLevel + (m_fMipLODBias + m_fInstBias);
	fLOD = max(fLOD, m_fMinLOD);

	int nBiasedMip = (int)fLOD;
	UINT uExpectedMip;
	if( nBiasedMip <= 0 )
		uExpectedMip = 0;
	else if( nBiasedMip > MAX_LOD )
		uExpectedMip = MAX_LOD;
	else
		uExpectedMip = (UINT) nBiasedMip;

	UINT uWidth = DEFAULT_WIDTH;
	UINT uHeight = DEFAULT_HEIGHT;
	GetMipDimensions( uExpectedMip, uWidth, uHeight );
	UINT uSub = ( m_ArraySlice * RES_ACCESS_MIP_LEVELS ) + uExpectedMip;

	// Out-of bounds behavior
	if( m_fMinLOD > (float)(RES_ACCESS_MIP_LEVELS - 1) )
	{
		uSub = 0;
	}

	FLOAT fOutput = *( (float*) pResult );
	if( !WithinOneULP( (float)uSub, fOutput ) )
	{
		WriteToLog( _T( "CShader5xTest_sample_b::CheckResult() - Test case failed: expected subresource %f, sampled subresource %f" ), (float)uSub, fOutput );
		return false;
	}

	return true;
}

static float g_GatherMinLODTable[] =
{
	0.0f,
	0.1f,
	1.0f,
	2.0f,
	10.0f
};

CShader5xTest_gather4_base::CShader5xTest_gather4_base(const TCHAR* Instruction) : CShader5xTest_ResourceAccess( Instruction, true )
{
	m_bIsBound = true;
}

void CShader5xTest_gather4_base::InitTestParameters()
{
	const bool bGatherPO = InstAttributes.FullMatch(Opcode::Gather4, Modifier::ProgrammableOffset, Modifier::Comparison | Modifier::ResidencyFeedback);
	const bool bGatherComp = InstAttributes.FullMatch(Opcode::Gather4, Modifier::Comparison, Modifier::ProgrammableOffset | Modifier::ResidencyFeedback);

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	CTestCaseParameter<RESOURCE_TYPE> *pResTypeParam = AddParameter( _T( "Resource" ), &m_Resource );
	testfactor::RFactor rfResTex2D      = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE2D		  );
	testfactor::RFactor rfResTex2DArray = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURE2D_ARRAY );
	testfactor::RFactor rfResTexCube    = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURECUBE	  );
	testfactor::RFactor rfResTexCubeArray    = AddParameterValue< RESOURCE_TYPE >( pResTypeParam, RESOURCE_TEXTURECUBE_ARRAY	  );

	testfactor::RFactor rfRes2D = rfResTex2D + rfResTex2DArray;
	testfactor::RFactor rfResCube = rfResTexCube + rfResTexCubeArray;

	CTestCaseParameter<D3D11_TEXTURE_ADDRESS_MODE> *pAddressU = AddParameter( _T( "AddressU" ), &m_D3DSamplerDesc.AddressU );
	CTestCaseParameter<D3D11_TEXTURE_ADDRESS_MODE> *pAddressV = AddParameter( _T( "AddressV" ), &m_D3DSamplerDesc.AddressV );
	CUserValueSet<D3D11_TEXTURE_ADDRESS_MODE> *pAddrValues = new CUserValueSet<D3D11_TEXTURE_ADDRESS_MODE>( );
	pAddrValues->AddValue( D3D11_TEXTURE_ADDRESS_BORDER );
	pAddrValues->AddValue( D3D11_TEXTURE_ADDRESS_CLAMP );
	pAddrValues->AddValue( D3D11_TEXTURE_ADDRESS_WRAP );
	pAddrValues->AddValue( D3D11_TEXTURE_ADDRESS_MIRROR );
	pAddrValues->AddValue( D3D11_TEXTURE_ADDRESS_MIRROR_ONCE );
	testfactor::RFactor rfAddrU = AddParameterValueSet<D3D11_TEXTURE_ADDRESS_MODE>( pAddressU, pAddrValues );
	testfactor::RFactor rfAddrV = AddParameterValueSet<D3D11_TEXTURE_ADDRESS_MODE>( pAddressV, pAddrValues );
	testfactor::RFactor rfAddrU_Border = AddParameterValue<D3D11_TEXTURE_ADDRESS_MODE>( pAddressU, D3D11_TEXTURE_ADDRESS_BORDER );
	testfactor::RFactor rfAddrV_Border = AddParameterValue<D3D11_TEXTURE_ADDRESS_MODE>( pAddressV, D3D11_TEXTURE_ADDRESS_BORDER );

	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );

	CTestCaseParameter<int3> *pOffsetParam = AddParameter( _T( "AddrOffset" ), &m_nOffsets );
	SetParameterDefaultValue( pOffsetParam, g_pOffsets[ 0 ] );
	testfactor::RFactor rfOffsets;
	if (bGatherPO)
	{
		CTableValueSet<int3> *pOffsetValues = new CTableValueSet<int3>( g_pOffsetsProgrammable, sizeof( g_pOffsetsProgrammable[ 0 ] ), ARRAYSIZE( g_pOffsetsProgrammable ) );
		rfOffsets = AddParameterValueSet<int3>( pOffsetParam, pOffsetValues );
	}
	else
	{
		CTableValueSet<int3> *pOffsetValues = new CTableValueSet<int3>( g_pOffsets, sizeof( g_pOffsets[ 0 ] ), ARRAYSIZE( g_pOffsets ) );
		rfOffsets = AddParameterValueSet<int3>( pOffsetParam, pOffsetValues );
	}

	testfactor::RFactor rfTestSpecific;
	if ( bGatherComp )
	{
		// test parameter for comparison functions
		CUserValueSet< D3D11_COMPARISON_FUNC > *pCompFuncValues = new CUserValueSet< D3D11_COMPARISON_FUNC >();
		pCompFuncValues->AddValue( D3D11_COMPARISON_NEVER );
		pCompFuncValues->AddValue( D3D11_COMPARISON_LESS );
		pCompFuncValues->AddValue( D3D11_COMPARISON_EQUAL );
		pCompFuncValues->AddValue( D3D11_COMPARISON_LESS_EQUAL );
		pCompFuncValues->AddValue( D3D11_COMPARISON_GREATER );
		pCompFuncValues->AddValue( D3D11_COMPARISON_NOT_EQUAL );
		pCompFuncValues->AddValue( D3D11_COMPARISON_GREATER_EQUAL );
		pCompFuncValues->AddValue( D3D11_COMPARISON_ALWAYS );
		CTestCaseParameter< D3D11_COMPARISON_FUNC > *pCompFuncParam = AddParameter( _T( "ComparisonFunc" ), &m_ComparisonFunc );
		SetParameterDefaultValue( pCompFuncParam, D3D11_COMPARISON_NEVER );
		testfactor::RFactor rfCompFunc = AddParameterValueSet( pCompFuncParam, pCompFuncValues );

		DWORD dwInfP = 0x7f800000;	// +INF
		DWORD dwInfN = 0xff800000;  // -INF
		DWORD dwNaN = 0x7f800001;	// NaN

		CUserValueSet< float > *pReferenceValues = new CUserValueSet< float >( );
		pReferenceValues->AddValue( 0.f );
		pReferenceValues->AddValue( 1.f );
		pReferenceValues->AddValue( *( (float*) &dwInfP ));
		pReferenceValues->AddValue( *( (float*) &dwInfN ));
		pReferenceValues->AddValue( *( (float*) &dwNaN ));
		CTestCaseParameter< float > *pReference = AddParameter( _T( "ReferenceValue" ), &m_fReferenceValue );
		testfactor::RFactor rfReference = AddParameterValueSet< float >( pReference, pReferenceValues );

		rfTestSpecific = (rfCompFunc * rfReference) % (rfAddrU * rfAddrV );
	}
	else
	{
		CTestCaseParameter<int>* pComponentParam = AddParameter<int>( _T( "Component" ), &m_nSamplerComponent );
		testfactor::RFactor rfComponent_All = AddParameterValueSet<int>( pComponentParam, RangeValueSet<int>(0, 3, 1) );

		rfTestSpecific = (rfAddrU * rfAddrV) % rfComponent_All;
	}

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pDestMaskParam = AddParameter( _T( "DestMask" ), &m_DestMask );
	SetParameterDefaultValue( pDestMaskParam, AssemblyHelper::xyzw );
	CTableValueSet<AssemblyHelper::MaskFlag> *pMaskValues = new CTableValueSet<AssemblyHelper::MaskFlag>( g_pMasks, sizeof( g_pMasks[ 0 ] ), ARRAYSIZE( g_pMasks ) );
	testfactor::RFactor rfDestMasks = AddParameterValueSet<AssemblyHelper::MaskFlag>( pDestMaskParam, pMaskValues );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pRscSwizzleParam = AddParameter( _T( "ResourceSwizzle" ), &m_ResourceSwizzle );
	SetParameterDefaultValue( pRscSwizzleParam, AssemblyHelper::swizXYZW );
	CTableValueSet<AssemblyHelper::Swizzle> *pRscSwizzleValues = new CTableValueSet<AssemblyHelper::Swizzle>( g_pSwizzles, sizeof( g_pSwizzles[ 0 ] ), ARRAYSIZE( g_pSwizzles ) );
	testfactor::RFactor rfRscSwizzles = AddParameterValueSet<AssemblyHelper::Swizzle>( pRscSwizzleParam, pRscSwizzleValues );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pOffsetSwizzleParam = AddParameter( _T( "OffsetSwizzle" ), &m_OffsetSwizzle );
	SetParameterDefaultValue( pOffsetSwizzleParam, AssemblyHelper::swizXYZW );
	CTableValueSet<AssemblyHelper::Swizzle> *pOffSwizzleValues = new CTableValueSet<AssemblyHelper::Swizzle>( g_pSwizzles, sizeof( g_pSwizzles[ 0 ] ), ARRAYSIZE( g_pSwizzles ) );
	testfactor::RFactor rfOffsetSwizzles = NewReverseFactor( AddParameterValueSet<AssemblyHelper::Swizzle>( pOffsetSwizzleParam, pOffSwizzleValues ) );
	if (!bGatherPO)
		rfOffsetSwizzles = AddParameterValue<AssemblyHelper::Swizzle>( pOffsetSwizzleParam, AssemblyHelper::swizXYZW );

	CTestCaseParameter<AssemblyHelper::Swizzle> *pAddrSwizzleParam = AddParameter( _T( "AddressSwizzle" ), &m_AddressSwizzle );
	SetParameterDefaultValue( pAddrSwizzleParam, AssemblyHelper::swizXYZW );
	CTableValueSet<AssemblyHelper::Swizzle> *pAddrSwizzleValues
		= new CTableValueSet<AssemblyHelper::Swizzle>( g_pAddressXYSwizzles, sizeof( g_pAddressXYSwizzles[ 0 ] ), ARRAYSIZE( g_pAddressXYSwizzles ) );
	testfactor::RFactor rfAddrSwizzles = NewReverseFactor( AddParameterValueSet<AssemblyHelper::Swizzle>( pAddrSwizzleParam, pAddrSwizzleValues ) );

	CTestCaseParameter<float> *pResourceMinLODParam = AddParameter( _T( "ResourceMinLOD" ), &m_fResourceMinLOD );
	SetParameterDefaultValue( pResourceMinLODParam, 1.0f );
	testfactor::RFactor rfResourceMinLOD;

	if ( m_bTestResourceMinLOD )
	{
		rfResourceMinLOD = AddParameterValueSet< float > ( pResourceMinLODParam, new CTableValueSet<float>( g_GatherMinLODTable, sizeof( g_GatherMinLODTable[0] ), ARRAYSIZE( g_GatherMinLODTable ) ) );
	}
	else
	{
		rfResourceMinLOD = AddParameterValueSet< float > ( pResourceMinLODParam, new CUserValueSet<float>( 0.0f ) );
	}

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = ( rfRes2D * (rfTestSpecific % rfOffsets) + rfResCube * rfTestSpecific ) * rfResourceMinLOD;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages % (rfDestMasks + rfOffsetSwizzles + rfRscSwizzles + rfAddrSwizzles + (rfRscSwizzles % rfOffsetSwizzles % rfDestMasks % rfAddrSwizzles) ) );

	AddPriorityPatternFilter(FilterEqual<RESOURCE_TYPE>(_T("Resource"), RESOURCE_TEXTURE2D), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::MaskFlag>(_T("DestMask"), AssemblyHelper::xyzw), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("ResourceSwizzle"), AssemblyHelper::swizXYZW), WeightFactorLarge);

	// Out-of-bounds are not priority 1
	// This is out-of-bounds because 10 is the max mip level defined in the test
	AddPriorityPatternFilter( FilterEqual<float>(_T("ResourceMinLOD"), 10.0f), WeightFactorVerySmall);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.0f );
}

TEST_RESULT CShader5xTest_gather4_base::Setup( )
{
	if( GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 )
		return RESULT_SKIP;
	//
	// only gather4 supported on 10.1
	//
	if( GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1 && !InstAttributes.FullMatch(Opcode::Gather4, Modifier::None, Modifier::ResidencyFeedback) )
		return RESULT_SKIP;

	return CShader5xTest_ResourceAccess::Setup( );
}

TEST_RESULT CShader5xTest_gather4_base::SetupTestCase()
{
	TEST_RESULT tr = CShader5xTest_ResourceAccess::SetupTestCase();
	if( RESULT_PASS != tr )
	{
		return tr;
	}

	//
	// Only the red component is supportted in gather4 for 10.1
	//
	if(m_nSamplerComponent != 0 && GetFramework()->m_D3DOptions.FeatureLevel == D3D_FEATURE_LEVEL_10_1)
	{
		return RESULT_SKIP;
	}

	return RESULT_PASS;
}


TEST_RESULT CShader5xTest_gather4_base::InitResource()
{
	ID3D11Resource* pResource = NULL;

	TEST_RESULT Result = CShader5xTest_ResourceAccess::InitResource();

	if( (m_fResourceMinLOD != 0.0f) && (Result == RESULT_PASS) )
	{
		assert( m_pSRV );

		if( m_pSRV )
		{
			m_pSRV->GetResource( &pResource );

			GetImmediateContext()->SetResourceMinLOD( pResource, m_fResourceMinLOD );
		}
	}

	SAFE_RELEASE( pResource );

	return Result;
}

bool CShader5xTest_gather4_base::CheckResult( void *pResult, UINT uRowPitch )
{
	const bool bSampleCond = InstAttributes.FullMatch(Opcode::Gather4, Modifier::Comparison, Modifier::ProgrammableOffset | Modifier::ResidencyFeedback);
	const bool bGather40 = (g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0);
	const bool bGather_po = InstAttributes.FullMatch(Opcode::Gather4, Modifier::ProgrammableOffset, Modifier::Comparison | Modifier::ResidencyFeedback);
	const bool bGather_compare = bSampleCond;
	const bool bCube = m_Resource == RESOURCE_TEXTURECUBE || m_Resource == RESOURCE_TEXTURECUBE_ARRAY;

	const size_t elementStride = (bSampleCond || bGather40) ? sizeof(float) : 4 * sizeof(float);

	int rscSwizzleIndices[4];
	m_ResourceSwizzle.ToIndices(rscSwizzleIndices);
	int addrSwizzleIndices[4];
	m_AddressSwizzle.ToIndices(addrSwizzleIndices);
	int offsetSwizzleIndices[4];
	m_OffsetSwizzle.ToIndices(offsetSwizzleIndices);

	float *pOutput = (float*) pResult;
	for( UINT y = 0; y < DEFAULT_HEIGHT; ++y )
	{
		for( UINT x = 0; x < DEFAULT_WIDTH; ++x )
		{
			float quad[ 4 ];
			int texCoords[2] = {x,y};
			int xTex = texCoords[ addrSwizzleIndices[0] ];
			int yTex = texCoords[ addrSwizzleIndices[1] ];
			GetOffsetCoords( &xTex, &yTex, NULL, 0, bGather_po ? 6 : 4, offsetSwizzleIndices );
			GetQuad( xTex, yTex, elementStride, elementStride * DEFAULT_WIDTH, quad );
			bool componentPassed = true;

			for( UINT i = 0; i < 4; ++i )
			{

				float shaderResult = *(pOutput + 4 * x + i);
				float quadVal = quad[ rscSwizzleIndices[i] ];
				bool bInMask = true;

				switch(i)
				{
				case 0: if (!(m_DestMask & AssemblyHelper::x)) bInMask = false; break;
				case 1: if (!(m_DestMask & AssemblyHelper::y)) bInMask = false; break;
				case 2: if (!(m_DestMask & AssemblyHelper::z)) bInMask = false; break;
				case 3: if (!(m_DestMask & AssemblyHelper::w)) bInMask = false; break;
				}
				if (!bInMask)
					quadVal = 0;

				// Win7:719272
				// According to the spec, a fractional per-resource MinLOD clamp basically requires the floor of the MinLOD
				// miplevel and the less detailed miplevels to be resident. So MinLOD of 0.1 still allows mip 0 to be resident.
				// However, gather4 instrutions can only operate on view mip 0. Note: This is VIEW mip 0, however since the sampler state
				// has MinLOD = 0, we only need to see if the per resource min lod is >= 1.

				// Make sure our assumption for the per resource minlod is correct
				D3D11_SAMPLER_DESC samplerDesc;
				m_pSampler->GetDesc( &samplerDesc );
				assert( samplerDesc.MinLOD == 0 );

				if (m_fResourceMinLOD >= 1.0f)
					quadVal = 0;

				if (bSampleCond && bInMask)
				{

					//
					//  Synthesized texel in cube corner for Gather4_c*.
					//  Index == 1 is the sythesized texel for corner of faces 0, 3, and 5
					//
					if( bGather_compare &&
						bCube &&
						(x == DEFAULT_WIDTH - 1 && y == DEFAULT_HEIGHT - 1) &&
						rscSwizzleIndices[i] == 1 )
					{
						const float result = (floor(shaderResult * 1000.f)) / 1000.f;

						// The spec says the "returned comparison result for the syntesized texel can be 0, 0.33…, 0.66…, or 1"
						// "Some implementations may only return either 0 or 1 for the synthesized texel"
						if( !WithinOneULP(0.0f, result) &&
							!WithinOneULP(1.0f, result) &&
							!(result >= 0.33f && result < 0.34f) &&
							!(result >= 0.66f && result < 0.67f) )
						{
							WriteToLog( _T("Unexpected Synthesized pixel(%d,%d resourceswizzle=%d ). Value %f not in list of acceptable values for comparison filter {0.0, 0.33, 0.66, or 1.0}."), x, y, rscSwizzleIndices[i], shaderResult);
							componentPassed = false;
						}
					}
					else
					{

						if( !CompareValues( m_ComparisonFunc, m_fReferenceValue, quadVal, shaderResult, true ) )
						{
							WriteToLog( _T("Unexpected quad for %d, %d, resourceswizzle=%d does not match."), x, y, rscSwizzleIndices[i] );
							componentPassed = false;
						}
					}
				}
				else
				{
					//
					// Synthesized texel in cube corner.  Should be an Average of the 3 available texels.
					// Index == 1 is the sythesized texel for corner of faces 0, 3, and 5
					//
					if( quadVal != 0.0f && (bCube && (x == DEFAULT_WIDTH - 1 && y == DEFAULT_HEIGHT - 1) && rscSwizzleIndices[i] == 1) )
					{
						//
						// Calculated this based on actual AMD hardware failures off by 0.00003.
						// Win7:688254 - NVidia's approximation is within .007813. Increased to .00782 to be safe.
						// Win8:261761 - Intel's sandybridge lacks 24-bit precision and is within 0.015625. Increased to 0.01563 to be safe.
						// Win8:399679 - AMD's 3850 is within 0.031250. Increased to 0.0313 to be safe.
						//

						float maxPercentDifference = 0.03130f;
						float percentDifference = fabs(quadVal - shaderResult) / quadVal;

						if(percentDifference > maxPercentDifference)
						{
							WriteToLog( _T("Unexpected quad %d, %d, component(ResourceSwizzle)=%d. The synthesized value=%f ((%f+%f+%f)/3). This is within %f of output %f; maxTolerence is (%f). One of the other components used to synthesize may be incorrect, but may not show up in log because of the swizzle"),
								x,
								y,
								rscSwizzleIndices[i],
								quadVal,
								quad[0], quad[2], quad[3],
								percentDifference,
								shaderResult,
								maxPercentDifference);

							componentPassed = false;
						}
					}
					else
					{

						if( !WithinOneULP( quadVal, shaderResult ) )
						{
							WriteToLog( _T("Unexpected quad for %d, %d, ResourceSwizzle component %d does not match.\n\t\tExpected: %f\n\t\tOutput: %f"), x, y, rscSwizzleIndices[i], quadVal, shaderResult );
							componentPassed = false;
						}
					}

				}
			}

			if(!componentPassed)
			{
				return false;
			}
		}
		pOutput = (float*)((BYTE*)pResult + ( y + 1 ) * uRowPitch);
	}
	return true;
}

void CShader5xTest_gather4_base::GetQuad( UINT x, UINT y, UINT elementStride, UINT uRowPitch, float *pQuad )
{
	bool bCube = m_Resource == RESOURCE_TEXTURECUBE || m_Resource == RESOURCE_TEXTURECUBE_ARRAY;

	BYTE *pExpected = (BYTE*) m_pExpectedResult;

	int x0 = x;
	int y0 = y + 1;
	int x1 = x + 1;
	int y1 = y + 1;
	int x2 = x + 1;
	int y2 = y;
	int x3 = x;
	int y3 = y;

	if( bCube ) // correct as long as other faces are black
	{
		pQuad[ 3 ] = *((float*)( pExpected + y * uRowPitch + x * elementStride + m_nSamplerComponent * sizeof(float) ));

		// corner. Index == 1 is the sythesized texel for corner of faces 0, 3, and 5
		if( x == DEFAULT_WIDTH - 1 && y == DEFAULT_HEIGHT - 1 )
			pQuad[ 1 ] = pQuad[ 3 ] / 3.f;
		else if ( x1 >= DEFAULT_WIDTH || y1 >= DEFAULT_HEIGHT )
			pQuad[ 1 ] = 0.f; // might be better to have unique values for other faces
		else
			pQuad[ 1 ] = *((float*)( pExpected + y1 * uRowPitch + x1 * elementStride + m_nSamplerComponent * sizeof(float) ));

		if( y0 >= DEFAULT_HEIGHT )
			pQuad[ 0 ] = 0.f;
		else
			pQuad[ 0 ] = *((float*)( pExpected + y0 * uRowPitch + x0 * elementStride + m_nSamplerComponent * sizeof(float) ));

		if( x2 >= DEFAULT_WIDTH )
			pQuad[ 2 ] = 0.f;
		else
			pQuad[ 2 ] = *((float*)( pExpected + y2 * uRowPitch + x2 * elementStride + m_nSamplerComponent * sizeof(float) ));
	}
	else
	{
		bool bBorder0 = ( m_D3DSamplerDesc.AddressU == D3D11_TEXTURE_ADDRESS_BORDER && (x0 < 0 || x0 >= DEFAULT_WIDTH) ) ||
			( m_D3DSamplerDesc.AddressV == D3D11_TEXTURE_ADDRESS_BORDER && (y0 < 0 || y0 >= DEFAULT_HEIGHT) );
		bool bBorder1 = ( m_D3DSamplerDesc.AddressU == D3D11_TEXTURE_ADDRESS_BORDER && (x1 < 0 || x1 >= DEFAULT_WIDTH) ) ||
			( m_D3DSamplerDesc.AddressV == D3D11_TEXTURE_ADDRESS_BORDER && (y1 < 0 || y1 >= DEFAULT_HEIGHT) );
		bool bBorder2 = ( m_D3DSamplerDesc.AddressU == D3D11_TEXTURE_ADDRESS_BORDER && (x2 < 0 || x2 >= DEFAULT_WIDTH) ) ||
			( m_D3DSamplerDesc.AddressV == D3D11_TEXTURE_ADDRESS_BORDER && (y2 < 0 || y2 >= DEFAULT_HEIGHT) );
		bool bBorder3 = ( m_D3DSamplerDesc.AddressU == D3D11_TEXTURE_ADDRESS_BORDER && (x3 < 0 || x3 >= DEFAULT_WIDTH) ) ||
			( m_D3DSamplerDesc.AddressV == D3D11_TEXTURE_ADDRESS_BORDER && (y3 < 0 || y3 >= DEFAULT_HEIGHT) );

		switch( m_D3DSamplerDesc.AddressU )
		{
		case D3D11_TEXTURE_ADDRESS_WRAP:
			if( x0 < 0 ) x0 += DEFAULT_WIDTH;
			if( x1 < 0 ) x1 += DEFAULT_WIDTH;
			if( x2 < 0 ) x2 += DEFAULT_WIDTH;
			if( x3 < 0 ) x3 += DEFAULT_WIDTH;
			if( x0 >= DEFAULT_WIDTH ) x0 -= DEFAULT_WIDTH;
			if( x1 >= DEFAULT_WIDTH ) x1 -= DEFAULT_WIDTH;
			if( x2 >= DEFAULT_WIDTH ) x2 -= DEFAULT_WIDTH;
			if( x3 >= DEFAULT_WIDTH ) x3 -= DEFAULT_WIDTH;
			break;
		case D3D11_TEXTURE_ADDRESS_CLAMP:
			if( x0 < 0 ) x0 = 0;
			if( x1 < 0 ) x1 = 0;
			if( x2 < 0 ) x2 = 0;
			if( x3 < 0 ) x3 = 0;
			if( x0 >= DEFAULT_WIDTH ) x0 = DEFAULT_WIDTH - 1;
			if( x1 >= DEFAULT_WIDTH ) x1 = DEFAULT_WIDTH - 1;
			if( x2 >= DEFAULT_WIDTH ) x2 = DEFAULT_WIDTH - 1;
			if( x3 >= DEFAULT_WIDTH ) x3 = DEFAULT_WIDTH - 1;
			break;
		case D3D11_TEXTURE_ADDRESS_MIRROR:
			if( x0 >= DEFAULT_WIDTH ) x0 = DEFAULT_WIDTH - (1 + x0 - DEFAULT_WIDTH);
			if( x1 >= DEFAULT_WIDTH ) x1 = DEFAULT_WIDTH - (1 + x1 - DEFAULT_WIDTH);
			if( x2 >= DEFAULT_WIDTH ) x2 = DEFAULT_WIDTH - (1 + x2 - DEFAULT_WIDTH);
			if( x3 >= DEFAULT_WIDTH ) x3 = DEFAULT_WIDTH - (1 + x3 - DEFAULT_WIDTH);
			// intentional fall-through
		case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
			if( x0 < 0 ) x0 = -x0 - 1;
			if( x1 < 0 ) x1 = -x1 - 1;
			if( x2 < 0 ) x2 = -x2 - 1;
			if( x3 < 0 ) x3 = -x3 - 1;
			if( x0 >= DEFAULT_WIDTH ) x0 = DEFAULT_WIDTH - 1;
			if( x1 >= DEFAULT_WIDTH ) x1 = DEFAULT_WIDTH - 1;
			if( x2 >= DEFAULT_WIDTH ) x2 = DEFAULT_WIDTH - 1;
			if( x3 >= DEFAULT_WIDTH ) x3 = DEFAULT_WIDTH - 1;
			break;
		default:
			break;
		}

		switch( m_D3DSamplerDesc.AddressV )
		{
		case D3D11_TEXTURE_ADDRESS_WRAP:
			if( y0 < 0 ) y0 += DEFAULT_HEIGHT;
			if( y1 < 0 ) y1 += DEFAULT_HEIGHT;
			if( y2 < 0 ) y2 += DEFAULT_HEIGHT;
			if( y3 < 0 ) y3 += DEFAULT_HEIGHT;
			if( y0 >= DEFAULT_HEIGHT ) y0 -= DEFAULT_HEIGHT;
			if( y1 >= DEFAULT_HEIGHT ) y1 -= DEFAULT_HEIGHT;
			if( y2 >= DEFAULT_HEIGHT ) y2 -= DEFAULT_HEIGHT;
			if( y3 >= DEFAULT_HEIGHT ) y3 -= DEFAULT_HEIGHT;
			break;
		case D3D11_TEXTURE_ADDRESS_CLAMP:
			if( y0 < 0 ) y0 = 0;
			if( y1 < 0 ) y1 = 0;
			if( y2 < 0 ) y2 = 0;
			if( y3 < 0 ) y3 = 0;
			if( y0 >= DEFAULT_HEIGHT ) y0 = DEFAULT_HEIGHT - 1;
			if( y1 >= DEFAULT_HEIGHT ) y1 = DEFAULT_HEIGHT - 1;
			if( y2 >= DEFAULT_HEIGHT ) y2 = DEFAULT_HEIGHT - 1;
			if( y3 >= DEFAULT_HEIGHT ) y3 = DEFAULT_HEIGHT - 1;
			break;
		case D3D11_TEXTURE_ADDRESS_MIRROR:
			if( y0 >= DEFAULT_HEIGHT ) y0 = DEFAULT_HEIGHT - (1 + y0 - DEFAULT_HEIGHT);
			if( y1 >= DEFAULT_HEIGHT ) y1 = DEFAULT_HEIGHT - (1 + y1 - DEFAULT_HEIGHT);
			if( y2 >= DEFAULT_HEIGHT ) y2 = DEFAULT_HEIGHT - (1 + y2 - DEFAULT_HEIGHT);
			if( y3 >= DEFAULT_HEIGHT ) y3 = DEFAULT_HEIGHT - (1 + y3 - DEFAULT_HEIGHT);
			// intentional fall-through
		case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
			if( y0 < 0 ) y0 = -y0 - 1;
			if( y1 < 0 ) y1 = -y1 - 1;
			if( y2 < 0 ) y2 = -y2 - 1;
			if( y3 < 0 ) y3 = -y3 - 1;
			if( y0 >= DEFAULT_HEIGHT ) y0 = DEFAULT_HEIGHT - 1;
			if( y1 >= DEFAULT_HEIGHT ) y1 = DEFAULT_HEIGHT - 1;
			if( y2 >= DEFAULT_HEIGHT ) y2 = DEFAULT_HEIGHT - 1;
			if( y3 >= DEFAULT_HEIGHT ) y3 = DEFAULT_HEIGHT - 1;
			break;
		default:
			break;
		}

		assert( bBorder0 || 0 <= x0 && x0 < DEFAULT_WIDTH );
		assert( bBorder1 || 0 <= x1 && x1 < DEFAULT_WIDTH );
		assert( bBorder2 || 0 <= x2 && x2 < DEFAULT_WIDTH );
		assert( bBorder3 || 0 <= x3 && x3 < DEFAULT_WIDTH );
		assert( bBorder0 || 0 <= y0 && y0 < DEFAULT_WIDTH );
		assert( bBorder1 || 0 <= y1 && y1 < DEFAULT_WIDTH );
		assert( bBorder2 || 0 <= y2 && y2 < DEFAULT_WIDTH );
		assert( bBorder3 || 0 <= y3 && y3 < DEFAULT_WIDTH );

		pQuad[ 0 ] = bBorder0 ? m_fBorderColor[ m_nSamplerComponent ] : *((float*)( pExpected + y0 * uRowPitch + x0 * elementStride + m_nSamplerComponent * sizeof(float) ));
		pQuad[ 1 ] = bBorder1 ? m_fBorderColor[ m_nSamplerComponent ] : *((float*)( pExpected + y1 * uRowPitch + x1 * elementStride + m_nSamplerComponent * sizeof(float) ));
		pQuad[ 2 ] = bBorder2 ? m_fBorderColor[ m_nSamplerComponent ] : *((float*)( pExpected + y2 * uRowPitch + x2 * elementStride + m_nSamplerComponent * sizeof(float) ));
		pQuad[ 3 ] = bBorder3 ? m_fBorderColor[ m_nSamplerComponent ] : *((float*)( pExpected + y3 * uRowPitch + x3 * elementStride + m_nSamplerComponent * sizeof(float) ));
	}
}

CShader5xTest_lod::CShader5xTest_lod() : CShader5xTest_ResourceAccess(_T("lod"))
{
	m_bIsBound = true;
	m_MipLevel = 0;
	m_ArraySlice = 0;
	m_RTFormat = RES_RTFORMAT;
	m_ShaderStage = D3D_SHADER_STAGE_PIXEL;
}

void CShader5xTest_lod::InitTestParameters()
{
	CTestCaseParameter<float> *pLODBiasParam = AddParameter<float>( _T( "MipLODBias" ), &m_fMipLODBias );
	CUserValueSet<float> *pLODBiasValues = new CUserValueSet<float>( );
	pLODBiasValues->AddValue( 0.f );
	pLODBiasValues->AddValue( 1.f );
	pLODBiasValues->AddValue( (float) MAX_LOD );
	testfactor::RFactor rfLODBias = AddParameterValueSet( pLODBiasParam, pLODBiasValues );

	CTestCaseParameter< RESOURCE_TYPE > *pResTypeParam = AddParameter( _T( "Resource" ), &m_Resource );
	CUserValueSet<RESOURCE_TYPE> *pResValues = new CUserValueSet<RESOURCE_TYPE>( );
	pResValues->AddValue( RESOURCE_TEXTURE1D );
	pResValues->AddValue( RESOURCE_TEXTURE1D_ARRAY );
	pResValues->AddValue( RESOURCE_TEXTURE2D );
	pResValues->AddValue( RESOURCE_TEXTURE2D_ARRAY );
	pResValues->AddValue( RESOURCE_TEXTURECUBE );
	pResValues->AddValue( RESOURCE_TEXTURECUBE_ARRAY );
	pResValues->AddValue( RESOURCE_TEXTURE3D );
	testfactor::RFactor rfResType = AddParameterValueSet( pResTypeParam, pResValues );

	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );

	CTestCaseParameter<float> *pScaleParam = AddParameter( _T( "TexCoordScale" ), &m_fScale );
	CUserValueSet<float> *pScaleValues = new CUserValueSet<float>( );
	pScaleValues->AddValue( 8.f );
	pScaleValues->AddValue( 4.f );
	pScaleValues->AddValue( 2.f );
	pScaleValues->AddValue( 1.f );
	testfactor::RFactor rfScale = AddParameterValueSet( pScaleParam, pScaleValues );

	SetRootTestFactor(
		( rfResType * rfScale * rfLODBias )
	);
}

TEST_RESULT CShader5xTest_lod::Setup( )
{
	if( GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_0 )
		return RESULT_SKIP;

	return CShader5xTest_ResourceAccess::Setup( );
}

TEST_RESULT CShader5xTest_lod::SetupInputBuffer()
{
	TEST_RESULT tRes = RESULT_PASS;
	float v0 = 0.f, v1 = 1.f;
	float z = 0.f, tw = 0.f;
	//for sampling instructions need adjust 3rd(Z) texture coordinate to pick up correct volume slice

	//set up input buffer
	UINT iWidth = (UINT)m_Width;
	UINT iHeight = (UINT)m_Height;
	UINT iDepth = (UINT)m_Depth;

	if ( m_Resource == RESOURCE_TEXTURE3D )
		z = (float) m_ArraySlice / (max(1, m_Depth >> m_MipLevel)) + 1.f / ( 2.f * (max(1, m_Depth >> m_MipLevel)) );
	//z = (float) 0.f;
	else if ( m_Resource == RESOURCE_TEXTURE1D_ARRAY )
		v0 = v1 = (float) m_ArraySlice;
	else if ( m_Resource == RESOURCE_TEXTURE2D_ARRAY )
		z = (float) m_ArraySlice;
	else if ( m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
		tw = (float) m_ArraySlice;

	UINT uXCount, uYCount;

	uXCount = uYCount = 2;
	TLVERTEX *vTL = new TLVERTEX[ uXCount * uYCount ];

	float x0 = 0.f;
	float x1 = (float) iWidth;
	float y0 = (float) iHeight;
	float y1 = 0.f;
	float tx0 = 0.f;
	float tx1 = 1.f * m_fScale;
	float ty0 = 0.f;
	float ty1 = 1.f * m_fScale;
	float tz1 = 0.f;
	float tz0 = 0.f;
	if( m_Resource == RESOURCE_TEXTURECUBE || m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
	{
		// fix cube texcoord scaling
		if( m_fScale != 1.f )
		{
			tRes = RESULT_SKIP;
			goto setupDone;
		}

		tx0 = 1.f * m_fScale;
		tx1 = 1.f * m_fScale;
		ty0 = 1.f * m_fScale;
		ty1 = -1.f * m_fScale;
		tz0 = 1.f * m_fScale;
		tz1 = -1.f * m_fScale;
	}

	TLVERTEX vTL_Temp[] = {
		{{x0, y0}, {tx0, ty1, tz0, tw} },
		{{x0, y1}, {tx0, ty0, tz0, tw} },
		{{x1, y0}, {tx1, ty1, tz1, tw} },
		{{x1, y1}, {tx1, ty0, tz1, tw} }
	};
	memcpy( vTL, vTL_Temp, sizeof(vTL_Temp) );
	TransformVertices(vTL, uXCount * uYCount);

	GetEffectiveContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedDataTL;
	if( FAILED(hr = GetEffectiveContext()->Map( m_pBufferTL, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDataTL)) )
	{
		WriteToLog( _T("CShader5xTest_lod::SetupInputBuffer - Map() for Stream Input Buffer failed. hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		goto setupDone;
	}
	memcpy(mappedDataTL.pData, vTL, uXCount * uYCount * sizeof(TLVERTEX));
	GetEffectiveContext()->Unmap(m_pBufferTL, 0);

setupDone:
	delete [] vTL;

	return tRes;
}


bool CShader5xTest_lod::CheckResult( void *pResult, UINT uRowPitch )
{
	float *pOutput = (float*) pResult;

	float fLOD;

	if( m_Resource == RESOURCE_TEXTURECUBE || m_Resource == RESOURCE_TEXTURECUBE_ARRAY )
	{
		// For texture cubes, the unclamped LOD is always 0 + the mip bias
		// This is ensured by CShader5xTest_lod::SetupInputBuffer which skips all tests cases
		// for cubes where m_fScale != 1.f
		assert( m_fScale == 1.f );
		fLOD = m_fMipLODBias;
	}
	else
	{
		fLOD = floor( (log( m_fScale )/log(2.f)) + 0.5f + m_fMipLODBias );
	}

	float fClampedLOD = max( 0.f, min( fLOD, (float)MAX_LOD ) );

	for( UINT y = 0; y < DEFAULT_HEIGHT; ++y )
	{
		for( UINT x = 0; x < DEFAULT_WIDTH; ++x )
		{
			bool bPass = true;

			// Bug Win7:182495
			// Compare the expected value with the output value using epsilon instead of ULPs.
			// This is done because interpolated PS attributes are used as inputs into the LOD
			// instruction, and those attributes are inherently imprecise (the spec does not specify
			// how interpolation of attributes should be done)

			if( !CompareFloatEpsilon( fClampedLOD, pOutput[ 0 ], LOD_PRECISION_EPSILON ) )
			{
				WriteToLog( _T("Clamped LOD does not match:\n\t\tExpected: %f\n\t\tOutput: %f"), fClampedLOD, pOutput[ 0 ] );
				bPass = false;
			}
			if( !CompareFloatEpsilon( fLOD, pOutput[ 1 ], LOD_PRECISION_EPSILON ) )
			{
				WriteToLog( _T("Non-clamped LOD does not match:\n\t\tExpected: %f\n\t\tOutput: %f"), fLOD, pOutput[ 1 ] );
				bPass = false;
			}

			if( !WithinOneULP( 0.f, pOutput[ 2 ] ) )
			{
				WriteToLog( _T("B component should be zero, output: %f"), pOutput[ 2 ] );
				bPass = false;
			}
			if( !WithinOneULP( 0.f, pOutput[ 3 ] ) )
			{
				WriteToLog( _T("A component should be zero, output: %f"), pOutput[ 3 ] );
				bPass = false;
			}

			if( !bPass )
				return false;

			pOutput += 4;
		}
		pOutput = (float*)((BYTE*)pResult + ( y + 1 ) * uRowPitch);
	}
	return true;
}


//-----------------------------------------------------------------------------
void CShader5xTest_bufinfo::InitTestParameters()
{
	// UAV resources supported by pixel shader and compute shader
	CUserValueSet< D3D_SHADER_STAGE > *pShaderStagesUAV = new CUserValueSet< D3D_SHADER_STAGE >();
	pShaderStagesUAV->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStagesUAV->AddValue( D3D_SHADER_STAGE_COMPUTE );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor rfShaderStageUAV = AddParameterValueSet( _T(m_shaderStageParameterStr), pShaderStagesUAV );

	CTestCaseParameter< BUFFER_TYPE > *pBufTypeParam = AddParameter( "Resource", &m_bufType );
	testfactor::RFactor rfBufTyped     = AddParameterValue< BUFFER_TYPE >( pBufTypeParam, BUFFER_TYPED );
	testfactor::RFactor rfBufRaw      = AddParameterValue< BUFFER_TYPE >( pBufTypeParam, BUFFER_RAW );
	testfactor::RFactor rfBufStructured = AddParameterValue< BUFFER_TYPE >( pBufTypeParam, BUFFER_STRUCTURED );

	//for non-bound resource we'll only need to run one test case for each resource type
	CTestCaseParameter<bool> *pBoundParam = AddParameter( _T("IsBound"), &m_bIsBound );
	testfactor::RFactor rfIsNotBound = AddParameterValue( pBoundParam, false );
	testfactor::RFactor rfIsBound	 = AddParameterValue( pBoundParam, true );

	// this parameter only applies to pixel shader and compute shader
	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );
	testfactor::RFactor rfIsUAV = AddParameterValue( pIsUAVResourceParam, true );
	testfactor::RFactor rfIsNotUAV = AddParameterValue( pIsUAVResourceParam, false );

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pMaskParam = AddParameter( _T( "Mask" ), &m_uMask );
	CUserValueSet<AssemblyHelper::MaskFlag> *pMaskValues = new CUserValueSet<AssemblyHelper::MaskFlag>( );
	pMaskValues->AddValue(g_pMasks[7]);
	pMaskValues->AddValue(g_pMasks[8]);
	pMaskValues->AddValue(g_pMasks[9]);
	pMaskValues->AddValue(g_pMasks[10]);
	pMaskValues->AddValue(g_pMasks[11]);
	pMaskValues->AddValue(AssemblyHelper::noMask);
	testfactor::RFactor rfMask = AddParameterValueSet<AssemblyHelper::MaskFlag>( pMaskParam, pMaskValues );


	CUserValueSet< UINT > *pBufStructStrideValues = new CUserValueSet< UINT >;
	pBufStructStrideValues->AddValue( 4 );
	pBufStructStrideValues->AddValue( 32 );
	pBufStructStrideValues->AddValue( 16 );
	testfactor::RFactor rfBufStructStride = AddParameter( "BufferStructureStride", &m_BufferStructStride,
		pBufStructStrideValues );

	CUserValueSet< UINT > *pWidthValues = new CUserValueSet< UINT >;
	pWidthValues->AddValue( 16 );
	pWidthValues->AddValue( 32 );
	pWidthValues->AddValue( (UINT)MAX_BUFFER_BYTEWIDTH );
	testfactor::RFactor rfWidth = AddParameter( _T( "ByteWidth"), &m_Width, pWidthValues );

	CUserValueSet< DXGI_FORMAT > *pFormatValues = new CUserValueSet< DXGI_FORMAT >;
	pFormatValues->AddValue(  DXGI_FORMAT_R32G32B32A32_FLOAT );
	pFormatValues->AddValue(  DXGI_FORMAT_R8G8_SNORM  );
	pFormatValues->AddValue(  DXGI_FORMAT_R16G16B16A16_UNORM );
	pFormatValues->AddValue(  DXGI_FORMAT_R16_SINT  );
	pFormatValues->AddValue(  DXGI_FORMAT_R10G10B10A2_UINT  );
	testfactor::RFactor rfFormat = AddParameter( _T( "Format" ), &m_Format, pFormatValues);

	testfactor::RFactor rfTestCases =
		(( rfBufTyped * (rfIsNotBound + rfIsBound * rfWidth * rfMask * rfFormat)) +
		( rfBufRaw * (rfIsNotBound + rfIsBound * rfWidth * rfMask )) +
		( rfBufStructured * (rfIsNotBound + rfIsBound * ( rfWidth % rfBufStructStride ) * rfMask )) );

	testfactor::RFactor rfTestCasesWithShaderStageNotUAV = rfIsNotUAV * GetTestFactorWithDefaultStageCoverage( rfTestCases );
	testfactor::RFactor rfTestCasesWithShaderStageUAV = rfIsUAV * rfShaderStageUAV * rfTestCases;

	SetRootTestFactor( rfTestCasesWithShaderStageNotUAV + rfTestCasesWithShaderStageUAV );

	AddPriorityPatternFilter(FilterEqual<BUFFER_TYPE>(_T("Resource"), BUFFER_STRUCTURED), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<BUFFER_TYPE>(_T("Resource"), BUFFER_RAW), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("IsBound"), true), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//-----------------------------------------------------------------------------
TEST_RESULT CShader5xTest_bufinfo::Setup( )
{
	TEST_RESULT tr = CShader5xTest_ResourceAccess::Setup( );
	if ( tr != RESULT_PASS )
		return tr;
	if( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
	{
		WriteToLog( _T("Skip test group: bufinfo is not supported on feature levels less than 11.0") );
		return RESULT_SKIP;
	}

	return RESULT_PASS;
}

//-----------------------------------------------------------------------------
TEST_RESULT	CShader5xTest_bufinfo::SetupTestCase()
{
	TEST_RESULT tr = CShader5xTest::SetupTestCase();
	if( RESULT_PASS != tr )
		return tr;

	if (!g_TestApp.IsFL11OrGreater() &&
		m_bIsUAV &&
		!(m_Resource == RESOURCE_BUFFER &&
		(m_bufType == BUFFER_RAW || m_bufType == BUFFER_STRUCTURED)))
	{
		return RESULT_SKIP;
	}

	// Compute shader stage uses raw buffer as output UAV
	m_RTFormat = RES_RTFORMAT;
	m_numComponentsRawOutput = 1;
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		m_RTFormat = DXGI_FORMAT_R32_TYPELESS;
		m_numComponentsRawOutput = 4; // ResourceAccess groups need 4-component result for each pixel/thread
	}

	if ( m_bIsBound )
	{
		tr = InitBufferRes( false );
	}

	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
			return tr;

	tr = SetupInputBuffer();
	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
		return tr;

	//set up resource to a shader
	UINT szTexStage = 0;//( GetFramework()->GetCurrentTestCase() - 1 )% MAX_TEXSTAGE;

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
		GetEffectiveContext()->VSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		GetEffectiveContext()->GSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_PIXEL:
		GetEffectiveContext()->PSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_HULL:
		GetEffectiveContext()->HSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		GetEffectiveContext()->DSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		GetEffectiveContext()->CSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	}

	// create result render target or UAV
	tr = CreateResultRTVorUAV();
	if ( tr != RESULT_PASS )
	{
			WriteToLog( _T("CShader5xTest_bufinfo::SetupTestCase - CreateResultRTVorUAV failed. ") );
			return RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_bufinfo::SetupTestCase - ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	return RESULT_PASS;

}

//-----------------------------------------------------------------------------
tstring CShader5xTest_bufinfo::GetShaderCode(AssemblyHelper* pAsmHelper)
{
	UINT uTexStage = 0;

	// UAV resource should not conflict with the UAV output slots for Compute in asmHelper.
	// D3D11 Assembler Error: Invalid Bytecode: PS ouputs must be declared in slots less than UAVs
	// UINT uUAVSlot = pAsmHelper->GetNumOutputReg();
	UINT uUAVSlot = 7;

	tstring resType;
	if (m_bIsBound)
	{
		switch(m_Format)
		{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			resType = "float";
			break;
		case DXGI_FORMAT_R8G8_SNORM:
			resType = "snorm";
			break;
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			resType = "unorm";
			break;
		case DXGI_FORMAT_R16_SINT:
			resType = "sint";
			break;
		case DXGI_FORMAT_R10G10B10A2_UINT:
			resType = "uint";
			break;
		default:
			resType = "float";
			break;
		}
	}
	else
		resType = "float";

	if (!m_bIsBound)
		m_BufferStructStride = 4;


	switch ( m_bufType )
	{
	case BUFFER_TYPED:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_typed u%d, buffer, %s",
													uUAVSlot, resType.c_str() ) );
		else
			pAsmHelper->SetCustomUAVDecl( uTexStage, FormatString("dcl_resource t%d, buffer, %s",
													 uTexStage, resType.c_str() ) );
		break;
	case BUFFER_RAW:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_raw u%d ", uUAVSlot ) );
		else
			pAsmHelper->SetCustomUAVDecl( uTexStage, FormatString( "dcl_resource_raw t%d", uTexStage ) );
		break;
	case BUFFER_STRUCTURED:
		if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
			pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_structured u%d, %d",
													uUAVSlot, m_BufferStructStride ) );
		else
			pAsmHelper->SetCustomUAVDecl( uTexStage, FormatString( "dcl_resource_structured t%d, %d",
													 uTexStage, m_BufferStructStride) );
		break;
	default:
		return "";

	}

	// calculate expected results
	UINT numElements = 0;
	if (m_bIsBound)
	{
		switch ( m_bufType )
		{
		case BUFFER_TYPED:
			numElements = ( m_Width * 8 ) / GetBitsPerElement(m_Format);
			break;
		case BUFFER_RAW:
			numElements = m_Width;
			break;
		case BUFFER_STRUCTURED:
			numElements = m_Width / m_BufferStructStride;
			break;
		default:
			;
		}
	}
	for ( int i=0; i < 4; i++)
		((UINT *)m_pExpectedResult)[ i ] = numElements;
	// apply masks
	tstring maskStr = "";
	if (m_bIsBound)
	{
		if ( m_uMask > AssemblyHelper::noMask)
		{
			if ( !(m_uMask & AssemblyHelper::x) )
				((UINT *)m_pExpectedResult)[ 0 ] = (UINT)(-1);
			if ( !(m_uMask & AssemblyHelper::y) )
				((UINT *)m_pExpectedResult)[ 1 ] = (UINT)(-1);
			if ( !(m_uMask & AssemblyHelper::z) )
				((UINT *)m_pExpectedResult)[ 2 ] = (UINT)(-1);
			if ( !(m_uMask & AssemblyHelper::w) )
				((UINT *)m_pExpectedResult)[ 3 ] = (UINT)(-1);

			maskStr += ".";
			maskStr += ToString(m_uMask);
		}
	}

	tstring retStr;
	retStr += "mov r4, vec4(-1, -1, -1, -1)\n";
	if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
	{
		retStr += FormatString( "bufinfo r4%s, u%d\n", maskStr.c_str(), uUAVSlot );
	}
	else
	{
		retStr += FormatString( "bufinfo r4%s, t%d\n", maskStr.c_str(), uTexStage );
	}

	return retStr;

}

//-----------------------------------------------------------------------------
bool CShader5xTest_bufinfo::CheckResult( void *pResult, UINT uRowPitch )
{
	UINT * pExpectedResult = (UINT *) m_pExpectedResult;
	UINT * pOutputResult  = (UINT *) pResult;

	bool bReturn = true;

	if ( pOutputResult[ 0 ] != pExpectedResult[ 0 ] )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_bufinfo::CheckResult - X/R channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %d\n" ), pExpectedResult[ 0 ] );
		WriteToLog( _T( "Output value is %d\n" ), pOutputResult[ 0 ] );
	}

	if ( pOutputResult[ 1 ] != pExpectedResult[ 1 ] )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_bufinfo::CheckResult - Y/G channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %d\n" ), pExpectedResult[ 1 ] );
		WriteToLog( _T( "Output value is %d\n" ), pOutputResult[ 1 ] );
	}

	if ( pOutputResult[ 2 ] != pExpectedResult[ 2 ] )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_bufinfo::CheckResult - Z/B channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %d\n" ), pExpectedResult[ 2 ] );
		WriteToLog( _T( "Output value is %d\n" ), pOutputResult[ 2 ] );
	}

	if ( pOutputResult[ 3 ] != pExpectedResult[ 3 ] )
	{
		bReturn = false;
		WriteToLog( _T( "CShader5xTest_bufinfo::CheckResult - W/A channel is different.\n" ) );
		WriteToLog( _T( "Expected value is %d\n" ), pExpectedResult[ 3 ] );
		WriteToLog( _T( "Output value is %d\n" ), pOutputResult[ 3 ] );
	}

	return bReturn;
}

//-----------------------------------------------------------------------------
tstring ToString(BUFFER_TYPE bufType)
{
	tstring retVal;
	if (bufType == BUFFER_TYPED)
		retVal = "BUFFER_TYPED";
	else if (bufType == BUFFER_RAW)
		retVal = "BUFFER_RAW";
	else if (bufType == BUFFER_STRUCTURED)
		retVal = "BUFFER_STRUCTURED";
	return retVal;
}

//-----------------------------------------------------------------------------
bool FromString(BUFFER_TYPE* pBufType, tstring str)
{
	if( tcistring( str ) == _T( "BUFFER_TYPED" ) )
	{
		*pBufType = BUFFER_TYPED;
		return true;
	}

	if( tcistring( str ) == _T( "BUFFER_RAW" ) )
	{
		*pBufType = BUFFER_RAW;
		return true;
	}

	if( tcistring( str ) == _T( "BUFFER_STRUCTURED" ) )
	{
		*pBufType = BUFFER_STRUCTURED;
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------
TEST_RESULT	CShader5xTest_ldRawOrStruct::SetupTestCase()
{
	TEST_RESULT tr = CShader5xTest::SetupTestCase();
	if( RESULT_PASS != tr )
		return tr;

	if ( !(g_TestApp.IsFL11OrGreater() || g_TestApp.IsFL10AndHWSupportsComputeOn10() ) )
	{
		return RESULT_SKIP;
	}
	if ( !g_TestApp.IsFL11OrGreater() && g_TestApp.IsFL10AndHWSupportsComputeOn10() &&
		m_bIsUAV )
	{
		// Skip UAV test cases on FL10.x, since only 1 UAV is allowed for Compute on 10.x and
		// UAV is only supported by Compute stage on FL10.x
		return RESULT_SKIP;
	}


	// Compute shader stage uses raw buffer as output UAV
	m_RTFormat = RES_RTFORMAT;
	m_numComponentsRawOutput = 1;
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		m_RTFormat = DXGI_FORMAT_R32_TYPELESS;
		m_numComponentsRawOutput = 4; // ResourceAccess groups need 4-component result for each pixel/thread
	}

	if ( m_bIsBound )
	{
		tr = InitBufferRes( true );
	}

	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
			return tr;

	tr = SetupInputBuffer();
	if( tr == RESULT_SKIP || tr == RESULT_FAIL )
		return tr;

	//set up resource to a shader
	UINT szTexStage = 0;//( GetFramework()->GetCurrentTestCase() - 1 )% MAX_TEXSTAGE;

	switch ( m_ShaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
		GetEffectiveContext()->VSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		GetEffectiveContext()->GSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_PIXEL:
		GetEffectiveContext()->PSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_HULL:
		GetEffectiveContext()->HSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		GetEffectiveContext()->DSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		GetEffectiveContext()->CSSetShaderResources( szTexStage, 1, (ID3D11ShaderResourceView**)&m_pSRV );
		break;
	}

	// create result render target or UAV
	tr = CreateResultRTVorUAV();
	if ( tr != RESULT_PASS )
	{
			WriteToLog( _T("CShader5xTest_ldRawOrStruct::SetupTestCase - CreateResultRTVorUAV failed. ") );
			return RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("CShader5xTest_ldRawOrStruct::SetupTestCase - ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	return RESULT_PASS;

}


//-----------------------------------------------------------------------------
enum EVerification
{
	e_Defined,
	e_Undefined, // Skip completely
	e_Unsupported, // Undefined results that cannot return what would be expected
};

bool CShader5xTest_ldRawOrStruct::CheckResult( void * pResult, UINT uRowPitch )
{
	BYTE * pOutputResult  = (BYTE *) pResult;
	bool bReturn = true;
	const bool bLogVerbose = ((CShader5xTestApp*)GetFramework())->LogVerbose();

	UINT uBitsPerElement = GetBitsPerElement( RES_RTFORMAT );
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		uBitsPerElement = GetBitsPerElement( DXGI_FORMAT_R32_TYPELESS );

	int rscSwizzleIndices[4];
	m_ResourceSwizzle.ToIndices(rscSwizzleIndices);
	int addrSwizzleIndices[4];
	m_AddressSwizzle.ToIndices(addrSwizzleIndices);
	int offsetSwizzleIndices[4];
	m_OffsetSwizzle.ToIndices(offsetSwizzleIndices);

	for( UINT j = 0; j < DEFAULT_HEIGHT && bReturn; j++ )
	{
		for(UINT i = 0; i < DEFAULT_WIDTH && bReturn; i++)
		{
			int texCoords[2] = {i,j};
			int x = texCoords[ addrSwizzleIndices[0] ];
			int y = texCoords[ addrSwizzleIndices[1] ];

			int byteStart = x;
			if ( m_uOffsetSelectComp == AssemblyHelper::y)
				byteStart = y;

			int structStart = y;
			if ( m_uAddressSelectComp == AssemblyHelper::x)
				structStart = x;

			for(UINT k = 0; k < 4; k++) // four 32-bit channels
			{
				float fOutput = ((float*)pOutputResult)[ k ];
				float fExpected;
				EVerification eVerify = e_Defined;

				if (!m_bIsBound)
				{
					fExpected = 0.0;
				}
				else
				{
                    if ( InstAttributes.FullMatch(Opcode::Load, Modifier::Structured, Modifier::ResidencyFeedback) )
					{
						// when the structure does not provide enough bytes for swizzling, the entire element is undefined.
						if ( structStart >= (int)( m_Width / m_BufferStructStride ) ||
							byteStart < 0 || byteStart >= (int)m_BufferStructStride ||
							rscSwizzleIndices[k] * 4 >= (int)(m_BufferStructStride - byteStart) )
						{
							eVerify = e_Undefined;
						}
						// buffer byteoffset must be 4-byte aligned
						else if ( (byteStart % 4) )
						{
							eVerify = e_Unsupported;
							fExpected = ((float*)((BYTE*)m_pExpectedResult + structStart * m_BufferStructStride + byteStart ))[ rscSwizzleIndices[k] ];
						}
						else
							fExpected = ((float*)((BYTE*)m_pExpectedResult + structStart * m_BufferStructStride + byteStart ))[ rscSwizzleIndices[k] ];
					}
					else if ( InstAttributes.FullMatch(Opcode::Load, Modifier::Raw, Modifier::ResidencyFeedback) )
					{
						if ( byteStart < 0 || byteStart >= (int)( m_Width ) )
								fExpected = 0.f;
						else
						{
							// buffer byteoffset must be 4-byte aligned
							eVerify = ((byteStart % 4) == 0 ? e_Defined : e_Unsupported);
							fExpected = ((float*)((BYTE*)m_pExpectedResult + byteStart ))[ rscSwizzleIndices[k] ];
						}
					}

					if ( m_DestMask != AssemblyHelper::noMask )
					{
						switch(k)
						{
						case 0: if (!(m_DestMask & AssemblyHelper::x)) { eVerify = e_Defined; fExpected = 0; } break;
						case 1: if (!(m_DestMask & AssemblyHelper::y)) { eVerify = e_Defined; fExpected = 0; } break;
						case 2: if (!(m_DestMask & AssemblyHelper::z)) { eVerify = e_Defined; fExpected = 0; } break;
						case 3: if (!(m_DestMask & AssemblyHelper::w)) { eVerify = e_Defined; fExpected = 0; } break;
						}
					}
				}

				switch ( eVerify )
				{
				case ( e_Defined ):
					if( IsDenorm( fExpected ) )
						fExpected = 0.f;
					if( IsDenorm( fOutput ) )
						fOutput = 0.f;
					if ( !WithinOneULP( fExpected, fOutput ) && !( IsNaN( fOutput ) && IsNaN( fExpected ) ) )
					{
						if ( bLogVerbose || bReturn )
						{
							bReturn = false;
							WriteToLog( _T( "CShader5xTest_ldRawOrStruct::CheckResult - pixel at (%d, %d), component %d is different. \tExpected value is %f \tOutput value is %f" ), i, j, k, fExpected, fOutput );
						}
					}
					break;

				case ( e_Unsupported ):
					if ( !WithinOneULP( fExpected, 0.0f ) &&
						WithinOneULP( fExpected, fOutput ) && !( IsNaN( fOutput ) && IsNaN( fExpected ) ) )
					{
						if ( bLogVerbose || bReturn )
						{
							bReturn = false;
							WriteToLog( _T( "CShader5xTest_ldRawOrStruct::CheckResult - pixel at (%d, %d), component %d is same. \tExpected value is %f \tOutput value is %f" ), i, j, k, fExpected, fOutput );
						}
					}
					break;

				case ( e_Undefined ):
					bReturn = true;
					break; // Avoid testing value at all.

				default: break;
				}
			}

			pOutputResult += uBitsPerElement / 8 * m_numComponentsRawOutput;
		}
		pOutputResult = ((BYTE*) pResult) + ( (j + 1) * uRowPitch);
	}

	return bReturn;
}

//-----------------------------------------------------------------------------
void CShader5xTest_ld_raw_base::InitTestParameters()
{
	m_Resource = RESOURCE_BUFFER;
	m_bufType = BUFFER_RAW;
	m_BufferStructStride = 0;
	m_Format = DXGI_FORMAT_R32_TYPELESS;

	// UAV resources supported by pixel shader and compute shader
	CUserValueSet< D3D_SHADER_STAGE > *pShaderStagesUAV = new CUserValueSet< D3D_SHADER_STAGE >();
	pShaderStagesUAV->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStagesUAV->AddValue( D3D_SHADER_STAGE_COMPUTE );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor rfShaderStageUAV = AddParameterValueSet( _T(m_shaderStageParameterStr), pShaderStagesUAV );

	//for non-bound resource we'll only need to run one test case for each resource type
	CTestCaseParameter<bool> *pBoundParam = AddParameter( _T("IsBound"), &m_bIsBound );
	testfactor::RFactor rfIsNotBound = AddParameterValue( pBoundParam, false );
	testfactor::RFactor rfIsBound	 = AddParameterValue( pBoundParam, true );

	// this parameter only applies to pixel shader and compute shader
	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );
	testfactor::RFactor rfIsUAV = AddParameterValue( pIsUAVResourceParam, true );
	testfactor::RFactor rfIsNotUAV = AddParameterValue( pIsUAVResourceParam, false );

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pMaskParam = AddParameter( _T( "Mask" ), &m_DestMask );
	CUserValueSet<AssemblyHelper::MaskFlag> *pMaskValues = new CUserValueSet<AssemblyHelper::MaskFlag>( );
	pMaskValues->AddValue(g_pMasks[7]);
	pMaskValues->AddValue(g_pMasks[8]);
	pMaskValues->AddValue(g_pMasks[9]);
	pMaskValues->AddValue(g_pMasks[10]);
	pMaskValues->AddValue(g_pMasks[11]);
	pMaskValues->AddValue(AssemblyHelper::noMask);
	testfactor::RFactor rfMask = AddParameterValueSet<AssemblyHelper::MaskFlag>( pMaskParam, pMaskValues );
	SetParameterDefaultValue(pMaskParam, AssemblyHelper::noMask);

	CTestCaseParameter<AssemblyHelper::Swizzle> *pSwizzleParam = AddParameter( _T( "Swizzle" ), &m_ResourceSwizzle );
	CUserValueSet<AssemblyHelper::Swizzle> *pSwizzleValues = new CUserValueSet<AssemblyHelper::Swizzle>( );
	pSwizzleValues->AddValue( g_pSwizzles[0] );
	pSwizzleValues->AddValue( g_pSwizzles[2] );
	pSwizzleValues->AddValue( g_pSwizzles[4] );
	pSwizzleValues->AddValue( g_pSwizzles[9] );
	pSwizzleValues->AddValue( g_pSwizzles[14] );
	pSwizzleValues->AddValue( g_pSwizzles[16] );
	testfactor::RFactor rfSwizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSwizzleParam, pSwizzleValues );
	SetParameterDefaultValue(pSwizzleParam, AssemblyHelper::swizXYZW);

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pOffsetParam = AddParameter( _T( "OffsetSelectComponent" ),
		&m_uOffsetSelectComp );
	CUserValueSet<AssemblyHelper::MaskFlag> *pOffsetValues = new CUserValueSet<AssemblyHelper::MaskFlag>( );
	pOffsetValues->AddValue(AssemblyHelper::x);
	pOffsetValues->AddValue(AssemblyHelper::y);
//	pOffsetValues->AddValue(AssemblyHelper::z);
//	pOffsetValues->AddValue(AssemblyHelper::w);
	testfactor::RFactor rfOffset = AddParameterValueSet<AssemblyHelper::MaskFlag>( pOffsetParam, pOffsetValues );
	SetParameterDefaultValue(pOffsetParam, AssemblyHelper::x);

	CUserValueSet< UINT > *pWidthValues = new CUserValueSet< UINT >;
	pWidthValues->AddValue( 16 );
	pWidthValues->AddValue( 32 );
	pWidthValues->AddValue( (UINT)MAX_RESOURCE_SIZE );
	testfactor::RFactor rfWidth = AddParameter( _T( "ByteWidth"), &m_Width, pWidthValues );

	testfactor::RFactor rfTestCases = ( rfIsNotBound + rfIsBound * rfWidth * ( rfMask % rfSwizzle ) * rfOffset ) ;

	testfactor::RFactor rfTestCasesWithShaderStageNotUAV = rfIsNotUAV * GetTestFactorWithDefaultStageCoverage( rfTestCases );
	testfactor::RFactor rfTestCasesWithShaderStageUAV = rfIsUAV * rfShaderStageUAV * rfTestCases;

	SetRootTestFactor( rfTestCasesWithShaderStageNotUAV + rfTestCasesWithShaderStageUAV );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ByteWidth"), (UINT)MAX_RESOURCE_SIZE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Swizzle"), AssemblyHelper::swizXYZW), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("IsBound"), true), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//-----------------------------------------------------------------------------
tstring CShader5xTest_ld_raw_base::GetShaderCode(AssemblyHelper* pAsmHelper)
{
	UINT uTexStage = 0;

	// UAV resource should not conflict with the UAV output slots for Compute in asmHelper.
	// D3D11 Assembler Error: Invalid Bytecode: PS ouputs must be declared in slots less than UAVs
	// UINT uUAVSlot = pAsmHelper->GetNumOutputReg();
	UINT uUAVSlot = 7;

	tstring maskStr = "";
	tstring swizzleStr = "";
	tstring offsetStr = "";
	if (m_bIsBound)
	{
		if ( m_DestMask > AssemblyHelper::noMask)
		{
			maskStr += ".";
			maskStr += ToString(m_DestMask);
		}

		swizzleStr += ".";
		swizzleStr += ToString(m_ResourceSwizzle);

		offsetStr += ".";
		offsetStr += ToString(m_uOffsetSelectComp);
	}
	else
	{
		offsetStr += ".x";
	}

	tstring retStr;
	if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY || m_ShaderStage == D3D_SHADER_STAGE_HULL )
		retStr += "ftoi r0, v[ 0 ][ 1 ]\n";
	else if ( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
		retStr += "ftoi r0, vcp[ 0 ][ 1 ]\n";
	else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		retStr += "mov r0.xyz, vThreadGroupID.xyz\n";
	else
		retStr += "ftoi r0, v[ 1 ]\n";

	if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY || m_ShaderStage == D3D_SHADER_STAGE_HULL )
		retStr += "ftoi r0.xy, v[ 0 ][ 2 ].xy\n";
	else if ( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
		retStr += "ftoi r0.xy, vcp[ 0 ][ 2 ].xy\n";
	else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		retStr += "mov r0.xy, vThreadGroupID.xy\n";
	else
		retStr += "ftoi r0.xy, v[ 2 ].xy\n";

	if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
		pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_raw u%d ", uUAVSlot ) );
	else
		pAsmHelper->SetCustomUAVDecl( uTexStage, FormatString( "dcl_resource_raw t%d", uTexStage ) );


	if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
	{
		retStr += FormatString( "ld_raw r4%s, r0%s, u%d%s\n", maskStr.c_str(),
			offsetStr.c_str(), uUAVSlot, swizzleStr.c_str() );
	}
	else
	{
		retStr += FormatString( "ld_raw r4%s, r0%s, t%d%s\n", maskStr.c_str(),
			offsetStr.c_str(), uTexStage, swizzleStr.c_str() );
	}

	return retStr;

}

//-----------------------------------------------------------------------------
void CShader5xTest_ld_structured_base::InitTestParameters()
{
	m_Resource = RESOURCE_BUFFER;
	m_bufType = BUFFER_STRUCTURED;
	m_Format = DXGI_FORMAT_UNKNOWN;
	m_BufferStructStride = 4;

	// UAV resources supported by pixel shader and compute shader
	CUserValueSet< D3D_SHADER_STAGE > *pShaderStagesUAV = new CUserValueSet< D3D_SHADER_STAGE >();
	pShaderStagesUAV->AddValue( D3D_SHADER_STAGE_PIXEL );
	pShaderStagesUAV->AddValue( D3D_SHADER_STAGE_COMPUTE );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	testfactor::RFactor rfShaderStageUAV = AddParameterValueSet( _T(m_shaderStageParameterStr), pShaderStagesUAV );

	//for non-bound resource we'll only need to run one test case for each resource type
	CTestCaseParameter<bool> *pBoundParam = AddParameter( _T("IsBound"), &m_bIsBound );
	testfactor::RFactor rfIsNotBound = AddParameterValue( pBoundParam, false );
	testfactor::RFactor rfIsBound	 = AddParameterValue( pBoundParam, true );

	// this parameter only applies to pixel shader and compute shader
	CTestCaseParameter<bool> *pIsUAVResourceParam = AddParameter( _T("IsUAVResource"), &m_bIsUAV );
	pIsUAVResourceParam->SetDefaultValue( false );
	testfactor::RFactor rfIsUAV = AddParameterValue( pIsUAVResourceParam, true );
	testfactor::RFactor rfIsNotUAV = AddParameterValue( pIsUAVResourceParam, false );

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pMaskParam = AddParameter( _T( "Mask" ), &m_DestMask );
	CUserValueSet<AssemblyHelper::MaskFlag> *pMaskValues = new CUserValueSet<AssemblyHelper::MaskFlag>( );
	pMaskValues->AddValue(g_pMasks[7]);
	pMaskValues->AddValue(g_pMasks[8]);
	pMaskValues->AddValue(g_pMasks[9]);
	pMaskValues->AddValue(g_pMasks[10]);
	pMaskValues->AddValue(g_pMasks[11]);
	pMaskValues->AddValue(AssemblyHelper::noMask);
	testfactor::RFactor rfMask = AddParameterValueSet<AssemblyHelper::MaskFlag>( pMaskParam, pMaskValues );
	SetParameterDefaultValue(pMaskParam, AssemblyHelper::noMask);

	CTestCaseParameter<AssemblyHelper::Swizzle> *pSwizzleParam = AddParameter( _T( "Swizzle" ), &m_ResourceSwizzle );
	CUserValueSet<AssemblyHelper::Swizzle> *pSwizzleValues = new CUserValueSet<AssemblyHelper::Swizzle>( );
	pSwizzleValues->AddValue( g_pSwizzles[0] );
	pSwizzleValues->AddValue( g_pSwizzles[2] );
	pSwizzleValues->AddValue( g_pSwizzles[4] );
	pSwizzleValues->AddValue( g_pSwizzles[9] );
	pSwizzleValues->AddValue( g_pSwizzles[14] );
	pSwizzleValues->AddValue( g_pSwizzles[16] );
	testfactor::RFactor rfSwizzle = AddParameterValueSet<AssemblyHelper::Swizzle>( pSwizzleParam, pSwizzleValues );
	SetParameterDefaultValue(pSwizzleParam, AssemblyHelper::swizXYZW);

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pOffsetParam = AddParameter( _T( "OffsetSelectComponent" ),
		&m_uOffsetSelectComp );
	CUserValueSet<AssemblyHelper::MaskFlag> *pOffsetValues = new CUserValueSet<AssemblyHelper::MaskFlag>( );
	pOffsetValues->AddValue(AssemblyHelper::x);
	pOffsetValues->AddValue(AssemblyHelper::y);
//	pOffsetValues->AddValue(AssemblyHelper::z);
//	pOffsetValues->AddValue(AssemblyHelper::w);
	testfactor::RFactor rfOffset = AddParameterValueSet<AssemblyHelper::MaskFlag>( pOffsetParam, pOffsetValues );

	CTestCaseParameter<AssemblyHelper::MaskFlag> *pAddressParam = AddParameter( _T( "AddressSelectComponent" ),
		&m_uAddressSelectComp );
	CUserValueSet<AssemblyHelper::MaskFlag> *pAddressValues = new CUserValueSet<AssemblyHelper::MaskFlag>( );
	pAddressValues->AddValue(AssemblyHelper::y);
//	pAddressValues->AddValue(AssemblyHelper::w);
	pAddressValues->AddValue(AssemblyHelper::x);
//	pAddressValues->AddValue(AssemblyHelper::z);
	testfactor::RFactor rfAddress = AddParameterValueSet<AssemblyHelper::MaskFlag>( pAddressParam, pAddressValues );

	CUserValueSet< UINT > *pBufStructStrideValues = new CUserValueSet< UINT >;
	pBufStructStrideValues->AddValue( 4 );
	pBufStructStrideValues->AddValue( 32 );
	pBufStructStrideValues->AddValue( 16 );
	CTestCaseParameter<UINT> *pBufStructStrideParam = AddParameter( _T( "BufferStructureStride" ), &m_BufferStructStride );
	testfactor::RFactor rfBufStructStride = AddParameterValueSet<UINT>( pBufStructStrideParam,
		pBufStructStrideValues );
	pBufStructStrideParam->SetDefaultValue( 4 );

	CUserValueSet< UINT > *pWidthValues = new CUserValueSet< UINT >;
	pWidthValues->AddValue( 16 );
	pWidthValues->AddValue( 32 );
	pWidthValues->AddValue( (UINT)MAX_RESOURCE_SIZE );
	CTestCaseParameter<UINT> *pByteWidthParam = AddParameter( _T( "ByteWidth"), &m_Width );
	testfactor::RFactor rfWidth = AddParameterValueSet<UINT>( pByteWidthParam, pWidthValues );
	pByteWidthParam->SetDefaultValue( 16 );

	testfactor::RFactor rfTestCases = ( rfIsNotBound + rfIsBound * ( rfWidth % rfBufStructStride ) * ( rfMask % rfSwizzle ) * ( rfOffset % rfAddress ) );

	testfactor::RFactor rfTestCasesWithShaderStageNotUAV = rfIsNotUAV * GetTestFactorWithDefaultStageCoverage( rfTestCases );
	testfactor::RFactor rfTestCasesWithShaderStageUAV = rfIsUAV * rfShaderStageUAV * rfTestCases;

	SetRootTestFactor( rfTestCasesWithShaderStageNotUAV + rfTestCasesWithShaderStageUAV );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ByteWidth"), (UINT)MAX_RESOURCE_SIZE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<AssemblyHelper::Swizzle>(_T("Swizzle"), AssemblyHelper::swizXYZW), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<bool>(_T("IsBound"), true), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//-----------------------------------------------------------------------------
tstring CShader5xTest_ld_structured_base::GetShaderCode(AssemblyHelper* pAsmHelper)
{
	UINT uTexStage = 0;

	// UAV resource should not conflict with the UAV output slots for Compute in asmHelper.
	// D3D11 Assembler Error: Invalid Bytecode: PS ouputs must be declared in slots less than UAVs
	// UINT uUAVSlot = pAsmHelper->GetNumOutputReg();
	UINT uUAVSlot = 7;

	tstring maskStr = "";
	tstring swizzleStr = "";
	tstring offsetStr = "";
	tstring addressStr = "";
	if (m_bIsBound)
	{
		if ( m_DestMask > AssemblyHelper::noMask)
		{
			maskStr += ".";
			maskStr += ToString(m_DestMask);
		}

		swizzleStr += ".";
		swizzleStr += ToString(m_ResourceSwizzle);

		offsetStr += ".";
		offsetStr += ToString(m_uOffsetSelectComp);

		addressStr += ".";
		addressStr += ToString(m_uAddressSelectComp);
	}
	else
	{
		offsetStr += ".x";
		addressStr += ".x";
	}

	tstring retStr;
	if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY || m_ShaderStage == D3D_SHADER_STAGE_HULL )
		retStr += "ftoi r0, v[ 0 ][ 1 ]\n";
	else if ( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
		retStr += "ftoi r0, vcp[ 0 ][ 1 ]\n";
	else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		retStr += "mov r0.xyz, vThreadGroupID.xyz\n";
	else
		retStr += "ftoi r0, v[ 1 ]\n";

	if ( m_ShaderStage == D3D_SHADER_STAGE_GEOMETRY || m_ShaderStage == D3D_SHADER_STAGE_HULL )
		retStr += "ftoi r0.xy, v[ 0 ][ 2 ].xy\n";
	else if ( m_ShaderStage == D3D_SHADER_STAGE_DOMAIN )
		retStr += "ftoi r0.xy, vcp[ 0 ][ 2 ].xy\n";
	else if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
		retStr += "mov r0.xy, vThreadGroupID.xy\n";
	else
		retStr += "ftoi r0.xy, v[ 2 ].xy\n";


	if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
		pAsmHelper->SetCustomUAVDecl( uUAVSlot, FormatString( "dcl_uav_structured u%d, %d",
													uUAVSlot, m_BufferStructStride ) );
	else
		pAsmHelper->SetCustomUAVDecl( uTexStage, FormatString( "dcl_resource_structured t%d, %d",
													uTexStage, m_BufferStructStride ) );


	if ( m_bIsUAV && ( m_ShaderStage == D3D_SHADER_STAGE_PIXEL || m_ShaderStage == D3D_SHADER_STAGE_COMPUTE ) )
	{
		retStr += FormatString( "ld_structured r4%s, r0%s, r0%s, u%d%s\n", maskStr.c_str(),
			addressStr.c_str(), offsetStr.c_str(), uUAVSlot, swizzleStr.c_str() );
	}
	else
	{
		retStr += FormatString( "ld_structured r4%s, r0%s, r0%s, t%d%s\n", maskStr.c_str(),
			addressStr.c_str(), offsetStr.c_str(), uTexStage, swizzleStr.c_str() );
	}

	return retStr;

}

