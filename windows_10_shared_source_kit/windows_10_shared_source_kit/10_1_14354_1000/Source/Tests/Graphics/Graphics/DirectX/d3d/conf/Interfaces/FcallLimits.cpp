#include "FcallLimits.h"
#include <sstream>

#include "testasm_api.h"

typedef std::basic_stringstream<TCHAR> tstringstream;

#define MAX_HLSL_DEPTH 4                // The HLSL compiler takes geological time to compile shaders with deeply nested interface methods.
										// So if it is nested deeper then this we move to an assembler version of the shader.

#define IFR(x) if( FAILED(x) ) { WriteToLog( _T("%s:%d , %s"), __FILE__, __LINE__, D3DHResultToString(x).c_str() ); return RESULT_FAIL; }
#define IFRM(x,m) if( FAILED(x) ) { WriteToLog( _T("%s:%d , %s, %s"), __FILE__, __LINE__, m, D3DHResultToString(x).c_str() ); return RESULT_FAIL; }

tstring ToString( StructCallTree const& CallTree )
{
	tstringstream ss;
	tstring comma("");
	unsigned int i = 0;
	while ( i < CallTreeMaxDepth  &&  CallTree.CallTreeList[i] != 0 )
	{
		unsigned int temp = CallTree.CallTreeList[i];
		ss << comma << temp;
		comma = ",";	
		i++;
	}	
	return ss.str();
}

bool FromString( StructCallTree * CallTree , tstring const& str )
{
	tstring comma(",");
	unsigned int i = 0;
	unsigned int val = 0;
	unsigned int ptr = 0;
	while ( ptr < str.length()  &&  i < CallTreeMaxDepth )
	{	
		unsigned int start = ptr;
		while ( ptr < str.length()  &&  str.substr(ptr,1) != comma )
			ptr++;
		tstringstream ss(str.substr(start, ptr-start));
		ss >> val;
		CallTree->CallTreeList[i] = val;
		i++;
	}
	while (i < CallTreeMaxDepth )
	{
		CallTree->CallTreeList[i] = 0;
		i++;
	}
	return true;
}

bool StructCallTree::operator==(const StructCallTree &other) const 
{
	return memcmp( this->CallTreeList, other.CallTreeList, sizeof(CallTreeList) ) == 0 ? true : false;
}

const CFcallLimits::Vertex CFcallLimits::s_pVertexData[] = 
{
	{ -1.0f, -1.0f, 0.5f },
	{ -1.0f, 1.0f, 0.5f },
	{ 1.0f, -1.0f, 0.5f },
	{ 1.0f, 1.0f, 0.5f }
};

static const D3D11_SO_DECLARATION_ENTRY SO_DECL[] = 
{
	0, "RESOURCE_DATA", 0, 0, 4, 0,
	0, "RESOURCE_DATA", 1, 0, 4, 0,
	0, "RESOURCE_DATA", 2, 0, 4, 0,
	0, "RESOURCE_DATA", 3, 0, 4, 0,
};

static UINT SO_BUFFER_STRIDE = sizeof(UINT) * 4 * 4;


void CFcallLimits::InitTestParameters()
{
	//
	// Call tree
	//
	CTestCaseParameter<StructCallTree>* pCallTree = AddParameter<StructCallTree>( _T("CallTree"), &m_CallTree ); 
	// Tree fan out parameters must end in zero
	static const unsigned int pCallTree0[] = {3,4,3,4,0} ;          // HLSL - Easy test, 4 deep, not very wide.
	static const unsigned int pCallTree1[] = {3,6,5,7,2,0} ;        // ASM - Easy test (Just past max for HLSL)  
	static const unsigned int pCallTree2[] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0} ;	    // ASM - 32 deep (max)
	static const unsigned int pCallTree3[] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0} ;    // ASM - 33 deep (hardware should ignore last)
	static const unsigned int pCallTree4[] = {64,63,0} ;            // HLSL - Wide test, 2 deep, 4096 bodies (max)     
	static const StructCallTree sCallTree0(pCallTree0);
	static const StructCallTree sCallTree1(pCallTree1);
	static const StructCallTree sCallTree2(pCallTree2);
	static const StructCallTree sCallTree3(pCallTree3);
	static const StructCallTree sCallTree4(pCallTree4);
	testfactor::RFactor  factorCallTree0 = AddParameterValue<StructCallTree>( pCallTree, sCallTree0 );
	testfactor::RFactor  factorCallTree1 = AddParameterValue<StructCallTree>( pCallTree, sCallTree1 );
	testfactor::RFactor  factorCallTree2 = AddParameterValue<StructCallTree>( pCallTree, sCallTree2 );
	testfactor::RFactor  factorCallTree3 = AddParameterValue<StructCallTree>( pCallTree, sCallTree3 );
	testfactor::RFactor  factorCallTree4 = AddParameterValue<StructCallTree>( pCallTree, sCallTree4 );

	CTestCaseParameter<unsigned int>* pPermutations = AddParameter<unsigned int>( _T("Permutations"), &m_Permutations );
	testfactor::RFactor  factorPermutations1 = AddParameterValueSet<unsigned int>( pPermutations , new CRangeValueSet<unsigned int>(1u, 1u, 1u) );
	testfactor::RFactor  factorPermutations2 = AddParameterValueSet<unsigned int>( pPermutations , new CRangeValueSet<unsigned int>(1u, 2u, 1u) );
	testfactor::RFactor  factorPermutations3 = AddParameterValueSet<unsigned int>( pPermutations , new CRangeValueSet<unsigned int>(1u, 3u, 1u) );
	testfactor::RFactor  factorPermutations8 = AddParameterValueSet<unsigned int>( pPermutations , new CRangeValueSet<unsigned int>(1u, 8u, 1u) );

	testfactor::RFactor callTree = factorCallTree0 * factorPermutations8
								 + factorCallTree1 * factorPermutations8
								 + factorCallTree2 * factorPermutations3
								 + factorCallTree3 * factorPermutations3;
								 // This last set of cases causes HW to hang/take very long and the test to be unqualified
								 //+ factorCallTree4 * factorPermutations2;

	//
	// Shader stage
	//
	CTestCaseParameter<D3D_SHADER_STAGE> *pShaderStageParam = AddParameter( _T("ShaderStage"), &m_shaderStage);

	CUserValueSet<D3D_SHADER_STAGE> *pShaderStageValues = new CUserValueSet<D3D_SHADER_STAGE>();
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_VERTEX);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_HULL);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_DOMAIN);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_GEOMETRY);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_PIXEL);
	pShaderStageValues->AddValue(D3D_SHADER_STAGE_COMPUTE);

	testfactor::RFactor factorShaderStage = AddParameterValueSet( pShaderStageParam, pShaderStageValues );


	SetRootTestFactor( callTree * factorShaderStage );
}

TEST_RESULT CFcallLimits::Setup()
{
	{
		TEST_RESULT tr = CInterfacesTest::Setup();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}
	{
		TEST_RESULT tr = CompilePassthroughShaders();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}

	return RESULT_PASS;
}

// Here are the various parts that will be using to generate our HLSL program
// These must match up with the passthrough shaders from the FCVertexShaderPassThrough.fx
const char gStructDefinitions[] =
"%s"
"struct passthrough_vertex { float4 pos : POSITION; };\n"
"struct output_vertex { row_major uint4x4 data : RESOURCE_DATA; };\n"
"struct patch_constant_out {float OutsideTessFactor[3] : SV_TessFactor; float InsideTessFactor     : SV_InsideTessFactor; };\n"
"patch_constant_out hs_patch_constant(InputPatch<passthrough_vertex,3> input_patch, OutputPatch<output_vertex,3> output_patch)\n"
"{\n"
"	patch_constant_out ret;\n"
"	ret.OutsideTessFactor[0] = 1;\n"
"	ret.OutsideTessFactor[1] = 1;\n"
"	ret.OutsideTessFactor[2] = 1;\n"
"	ret.InsideTessFactor = 1;\n"
"   return ret;\n"
"}\n";

// Parms: Type Number
const char gHlslInterface[] =
"interface Type%d\n"
"{\n"
"	uint4x4 func( uint4x4 x );\n"
"};\n"
;

// Parms: Type Number
//        Type Number
const char gHlslInterfacePtr[] =
"Type%d gType%d;\n"
;

// Parms: Type Number
//        Class within type Number
//        Type Number
//        uint4x4 index (0, 1, 2 or 3)
//        uint4x4 componant selector (x, y, z or w)
//        Prime Number multiplyer
//        Line to call lower level <blank with lowest level classes>
const char gHlslClass[] =
"class Class%d_%d : Type%d\n"
"{\n"
"	uint m;\n"
"	uint4x4 func( uint4x4 x )\n"
"	{\n"
"		x[%i].%s *= ( m * %d);\n"
"%s"
"		return x;\n"
"	}\n"
"};\n"
;

// Parms: Lower level Type Number
const char gHlslCall[] =
"		x = gType%d.func(x);\n"
;

// Parms: None
const char gHlslMain1[] =
"{\n"
"	    uint4x4 x = 1;\n"
;
// Parms: None
const char gHlslMain2[] =
"       output_vertex output_vert;\n"
"       output_vert.data = x;\n"
"	    return output_vert;\n"
"}\n"
;

const char gHlslMain2_gs[] =
"{ output_vertex v0; v0.data = x; output.Append(v0); }\n"
"{ output_vertex v0; v0.data = x; output.Append(v0); }\n"
"{ output_vertex v0; v0.data = x; output.Append(v0); }\n"
"}\n"
;

const char gHlslMain2_uav[] =
"RBuff.Store4(0*4,  x[0].xyzw);\n"
"RBuff.Store4(4*4,  x[1].xyzw);\n"
"RBuff.Store4(8*4,  x[2].xyzw);\n"
"RBuff.Store4(12*4, x[3].xyzw);\n"
"}\n"
;


// *** Here are the various parts that will be using to generate our Assembler (IL) program *** //

// Parms: None
const char gAsmHead[] = 
"dcl_globalFlags refactoringAllowed\n"
;
const char gAsmDclBody[] = "dcl_function_body fb%d\n";              // Parms: BodyNumber
const char gAsmDclTable[] = "dcl_function_table ft%d = {fb%d}\n";   // Parms: TableNumber, BodyNumber
const char gAmsDclTnterface1[] = "dcl_interface fp%d[1][1] = {";      // Parms: InterfaceNumber       // One of these per interfaces
const char gAmsDclTnterface2[] = "ft%d";                              // Parms: TableNumber           // As many of these as classes for this interface (comma delimited)
const char gAmsDclTnterface3[] = "}\n";                             // Parms: None

const char gAsmMain[] = 
"%s"
"%s"
"dcl_temps 5\n"
"mov r0.xyzw, l(1,1,1,1)\n"
"mov r1.xyzw, l(1,1,1,1)\n"
"mov r2.xyzw, l(1,1,1,1)\n"
"mov r3.xyzw, l(1,1,1,1)\n"
"fcall fp0[0][0]\n"
"%s"
"ret\n"
"%s"
;
const char gAsmFcall[] = "fcall fp%d[0][0]\n";          // Parm: Interface Number 
// Parms: Body Number
//        Interface Number
//        Register, Componant[Selector]
//        Register, Componant[Selector]
//        Register, Componant[Selector]
//        PrimeNumber
//        Register, Componant[Selector]      
//        fcall string
const char gAsmBody[] = 
"label fb%d\n"
"mov r4.xy, this[%d].xy\n"
"imul null, r%d.%s, cb[r4.x + 0][r4.y + 0].x, r%d.%s\n"
"imul null, r%d.%s, l(%d), r%d.%s\n"
"%s"
"ret\n"
;

void SimulateHlslMultiply( unsigned int i , unsigned int u , unsigned int x[16] )
{
	unsigned int idx = i / 2 ;
	if ( idx < 16 )
		x[idx] *= u;
}

TEST_RESULT CFcallLimits::SetupTestCase()
{
	TEST_RESULT tr = RESULT_PASS;

	// Reset device state, this helps prevent interference between test cases
	GetEffectiveContext()->ClearState();

	{
		tr = CInterfacesTest::SetupTestCase();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}

	HRESULT hr;

//  Generate the HLSL for this test

	std::stringstream ss;
	char buffer[2048];

	// Generate HLSL global functions

	// Prime number assignment:
	// - First "depth" prime numbers assigned to the API created class objects
	//   (top level gets smallest prime)
	// - From bottom level to top level, Class0_0, Class0_1, ... ClassN_N
	//   (bottom level gets smaller primes)

	// Loop thru the call tree to:
	// - Find the end of the tree (depth) (we generate the program from the lowest depth upward)
	// - Count the number of bodies
	m_bodies = 0;
	unsigned int bodiesAtThisDepth = 1;
	for ( m_depth = 0 ; m_depth < CallTreeMaxDepth && m_CallTree.CallTreeList[m_depth] != 0 ; m_depth++ )
	{
		bodiesAtThisDepth *= m_CallTree.CallTreeList[m_depth];
		m_bodies += bodiesAtThisDepth;
	}

	// Loop thru the call tree (from bottom to top):
	// - Generate HLSL Interface section
	for ( int i = m_depth-1 ; i >= 0 ; i-- )
	{
		sprintf(buffer, gHlslInterface, i);
		ss << buffer;
	}

	// Loop thru the call tree (from bottom to top):
	// - Generate HLSL Interface Pointer section
	for ( int i = m_depth-1 ; i >= 0 ; i-- )
	{
		sprintf(buffer, gHlslInterfacePtr, i, i);
		ss << buffer;
	}

	// Loop thru the call tree (from bottom to top):
	// - Assign which class at each depth we are going to use at set shader time.
	// - Remember the prime assigned to that class
	// - Generate HLSL Class section
	unsigned int primePtr = m_depth;
	unsigned int whichPrime[CallTreeMaxDepth];
	char CallStr[64];
	char const * const components[4] = {"x", "y", "z", "w"};
	CallStr[0] = '\0';  // Lowest level doesn't call lower
	srand(m_Permutations);
	for ( int i = m_depth-1 ; i >= 0 ; i-- )
	{
		// i = 0,1  ->  x[0].x 
		// i = 2,3  ->  x[0].y
		// i = 8,9  ->  x[1].x
		unsigned int indexer = min( i / 8 , 3 );
		unsigned int selector = (i / 2) % 4;
		m_whichClass[i] = rand() % m_CallTree.CallTreeList[i];
		for ( unsigned int j = 0 ; j < m_CallTree.CallTreeList[i] ; j++ )
		{
			if (j == m_whichClass[i])
				whichPrime[i] = PrimeNumbers[primePtr];
			sprintf(buffer, gHlslClass, i, j, i, indexer, components[selector], PrimeNumbers[primePtr], CallStr);
			primePtr++;
			ss << buffer;
		}
		if (m_depth <= MAX_HLSL_DEPTH)
			sprintf(CallStr, gHlslCall, i);
	}

	{
		const char* uavDecl = "";

		switch( m_shaderStage )
		{
			// CS & PS stagse declares a structured buffer at slot 1
		case D3D_SHADER_STAGE_PIXEL:
		case D3D_SHADER_STAGE_COMPUTE:
			uavDecl = "RWByteAddressBuffer RBuff : register(u1);\n";
			break;
		}

		sprintf(buffer, gStructDefinitions, uavDecl);

		ss << buffer;
	}

	// Generate HLSL main functions
	ss << GetEntryPoint() << gHlslMain1;
	int iCount = 1;
	if (m_depth > MAX_HLSL_DEPTH)
		iCount = m_depth ;
	for ( int i = 0 ; i < iCount ; i++ )
	{
		sprintf(CallStr, gHlslCall, i);
		ss << CallStr;
	}

	switch( m_shaderStage )
	{
	case D3D_SHADER_STAGE_GEOMETRY:
		ss << gHlslMain2_gs; // gs has no return value, it uses Append instead
		break;

	case D3D_SHADER_STAGE_COMPUTE:
	case D3D_SHADER_STAGE_PIXEL:
		ss << gHlslMain2_uav; // no return value, write to a UAV instead
		break;

	default:
		ss << gHlslMain2;       // all other stages output output_vertex
	}

//  Calculate the expected result
	
	for ( int i = 0 ; i < 16 ; i++ )
		m_ExpectedResults[i] = 1;
	// Loop thru the call tree:
	for ( unsigned int i = 0 ; i < m_depth ; i++ )
	{
		SimulateHlslMultiply( i, whichPrime[i] * PrimeNumbers[i] , m_ExpectedResults ) ;
	}


//  Create a Constant Buffer
	{
		const D3D11_BUFFER_DESC desc =
		{
			m_depth * 16,		// depth*16 assumes we don't have any HLSL globals
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_CONSTANT_BUFFER,
			0,
			0,
			0
		};
		unsigned int cb[CallTreeMaxDepth * 4]; 
		for ( unsigned int i = 0 ; i < m_depth ; i++ )
		{
			cb[i * 4 + 0] = PrimeNumbers[i];
			cb[i * 4 + 1] = 6;                  // Just nonsense non-prime values that should never be used by shader
			cb[i * 4 + 2] = 10;                 // Just nonsense non-prime values that should never be used by shader
			cb[i * 4 + 3] = 14;                 // Just nonsense non-prime values that should never be used by shader
		}

		const D3D11_SUBRESOURCE_DATA data = 
		{
			cb,
			0,
			0
		};

		HRESULT hr = GetDevice()->CreateBuffer( &desc, &data, m_rConstBuffer );
		IFR(hr);
	}

// compile shaders
	tr = CompileTestShader( ss.str() );
	if( RESULT_PASS != tr )
		return tr;   

//  Handle Input Stream
	{ // input layout
		const D3D11_INPUT_ELEMENT_DESC desc = 
		{
			"Position",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		};

		HRESULT hr = GetDevice()->CreateInputLayout( 
			&desc,
			1,
			m_rPassthroughVSByteCode->GetBufferPointer(),
			m_rPassthroughVSByteCode->GetBufferSize(),
			m_rInputLayout );
		IFR(hr);
	}
	{ // vertex buffer
		const D3D11_BUFFER_DESC desc = 
		{
			4 * sizeof(Vertex),
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_VERTEX_BUFFER,
			0,
			0,
			0
		};

		const D3D11_SUBRESOURCE_DATA data = 
		{
			s_pVertexData,
			0,
			0
		};

		HRESULT hr = GetDevice()->CreateBuffer(
			&desc,
			&data,
			m_rVertexBuffer );
		IFR(hr);
	}

	{ // create the rasterizer state
		const D3D11_RASTERIZER_DESC desc =
		{
			D3D11_FILL_SOLID,
			D3D11_CULL_NONE,
			TRUE,
			0,
			0.0f,
			0.0f,
			FALSE,
			FALSE,
			FALSE,
			FALSE
		};
		HRESULT hr = GetDevice()->CreateRasterizerState( &desc, m_rRastState );
		IFR(hr);
	}
	{ // create the depth stencil state
		const D3D11_DEPTH_STENCIL_DESC desc = 
		{
			FALSE,
			D3D11_DEPTH_WRITE_MASK_ZERO,
			D3D11_COMPARISON_LESS,
			FALSE,
			D3D11_DEFAULT_STENCIL_READ_MASK,
			D3D11_DEFAULT_STENCIL_WRITE_MASK,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_STENCIL_OP_KEEP,
			D3D11_COMPARISON_ALWAYS
		};
		HRESULT hr = GetDevice()->CreateDepthStencilState( &desc, m_rDepthStencilState );
		IFR(hr);
	}


	tr = CreateOutputBuffers();
	if( RESULT_PASS != tr )
		return tr;

	return RESULT_PASS;
}

TEST_RESULT CFcallLimits::ExecuteTestCase()
{
	{ // set the viewport
		const D3D11_VIEWPORT vp = 
		{
			0.0f, 
			0.0f,
			1.0f,		// ViewportWidth
			1.0f,		// ViewportHeight
			0.0f,
			1.0f
		};

		GetEffectiveContext()->RSSetViewports( 1, &vp );
	}

	GetEffectiveContext()->IASetInputLayout( m_rInputLayout );
	{
		const unsigned int stride = sizeof( Vertex );
		const unsigned int offset = 0u;
		GetEffectiveContext()->IASetVertexBuffers( 0, 1, &*m_rVertexBuffer, &stride, &offset );
	}

	TCOMPtr<ID3D11ClassInstance*> rInstances[CallTreeMaxDepth];
	ID3D11ClassInstance* pInstances[CallTreeMaxDepth];
	for ( unsigned int i = 0 ; i < m_depth ; i++ )
	{
		char ClassName[32];
		sprintf(ClassName, "Class%d_%d", i, m_whichClass[i]);

		HRESULT hr = m_pClassLinkage->CreateClassInstance(
				ClassName,                  // LPCSTR pszClassTypeName,
				0,                          // UINT ConstantBufferOffset,
				i,                          // UINT ConstantVectorOffset,
				0,                          // UINT TextureOffset
				0,                          // UINT SamplerOffset
				rInstances[i]               // ID3D11ClassInstance **ppInstance
				);

		pInstances[i] = rInstances[i];

		IFR(hr);
	}
	for ( unsigned int i = 0 ; i < m_depth ; i++ )
	{
			D3D11_CLASS_INSTANCE_DESC desc;
			pInstances[i]->GetDesc( &desc );
			char name[64];
			SIZE_T length = ARRAYSIZE(name);
			SIZE_T *pBufferLength;
			pInstances[i]->GetTypeName( name, &length );
	}

	BindResources( pInstances );

	GetEffectiveContext()->OMSetDepthStencilState( m_rDepthStencilState, 1u );	
	GetEffectiveContext()->RSSetState( m_rRastState );

	// Clear the UAV before it is used
	ID3D11UnorderedAccessView* pUAV = m_rUAV;

	if( pUAV )
	{
		const UINT clearValue[4] = { 0,0,0,0 };
		GetEffectiveContext()->ClearUnorderedAccessViewUint( pUAV, clearValue );
	}

	if( D3D_SHADER_STAGE_COMPUTE == m_shaderStage )
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	else
		GetEffectiveContext()->Draw( 4, 0 );

	{
		HRESULT hr = ExecuteEffectiveContext();
		IFR(hr);
	}

//  Copy the results from the draw
	unsigned int ActualResults[16];

	{
		TEST_RESULT tr = ReadOutputBuffers( ActualResults );
		if( RESULT_PASS != tr )
			return tr;
	}

// Compare actual results vs expected results
	
	bool bFailedCompare = false;
	for (unsigned int i = 0 ; i < 16 ; i++)
		if (ActualResults[i] != m_ExpectedResults[i])
			bFailedCompare = true;

	if ( bFailedCompare )
	{
		WriteToLog( _T("Actual Results are different then Expected Results:") );
		WriteToLog( _T("          Act X      Act Y      Act Z      Act W              Exp X      Exp Y      Exp Z      Exp W") );
		for (unsigned int i = 0 ; i < 16 ; i+=4)
		{
			WriteToLog( _T(" [%.1u] %.10u %.10u %.10u %.10u     [%.1u] %.10u %.10u %.10u %.10u"),
				i / 4,     ActualResults[i+0],     ActualResults[i+1],     ActualResults[i+2],     ActualResults[i+3], 
				i / 4, m_ExpectedResults[i+0], m_ExpectedResults[i+1], m_ExpectedResults[i+2], m_ExpectedResults[i+3] );
		}
		return RESULT_FAIL;
	}    

	return RESULT_PASS;
}

void CFcallLimits::CleanupTestCase()
{
	m_rConstBuffer.Release();

	m_rVertexShader.Release();
	m_rHullShader.Release();
	m_rDomainShader.Release();
	m_rGeometryShader.Release();
	m_rPixelShader.Release();
	m_rComputeShader.Release();

	m_rOutputBuffer.Release();
	m_rOutputBufferCopy.Release();

	m_rUAV.Release();

	m_rRastState.Release();
	m_rDepthStencilState.Release();
	m_rInputLayout.Release();
	m_rVertexBuffer.Release();
	CInterfacesTest::CleanupTestCase();
}

void CFcallLimits::Cleanup()
{
	m_rPassthroughVS1.Release();
	m_rPassthroughVS2.Release();
	m_rPassthroughHS.Release();
	m_rPassthroughDS.Release();
	m_rPassthroughGS.Release();
	m_rPassthroughVSByteCode.Release();

	CInterfacesTest::Cleanup();
}

const char* CFcallLimits::GetEntryPoint()
{
	switch( m_shaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
		return "output_vertex main( float3 pos : POSITION ) : RESOURCE_DATA\n";

	case D3D_SHADER_STAGE_HULL:
		return  "[patchconstantfunc(\"hs_patch_constant\")]\n"
				"[domain(\"tri\")]\n"
				"[partitioning(\"integer\")]\n"
				"[outputtopology(\"triangle_cw\")]\n"
				"[maxtessfactor(1.0)]\n"
				"[outputcontrolpoints(3)]\n"
				"output_vertex main(InputPatch<passthrough_vertex, 3> input_patch, uint id : SV_OutputControlPointID)\n";

	case D3D_SHADER_STAGE_DOMAIN:
		return  "[domain(\"tri\")]\n"
				"output_vertex main(OutputPatch<passthrough_vertex, 3> patch, patch_constant_out constant, float2 uv : SV_DomainLocation)\n";

	case D3D_SHADER_STAGE_GEOMETRY:
		return "[maxvertexcount(3)]\n"
			   "void main(triangle passthrough_vertex input[3], inout TriangleStream<output_vertex> output)\n";

	case D3D_SHADER_STAGE_PIXEL:
		return "void main( float4 pos : SV_POSITION )\n";

	case D3D_SHADER_STAGE_COMPUTE:
		return "[numthreads(1,1,1)]\n"
			   "void main( uint pid : SV_GroupIndex, uint3 gid : SV_GroupID )\n";
		
	default:
		assert( false );
		return "";
	}
}

TEST_RESULT CFcallLimits::CompilePassthroughShaders()
{
	struct
	{
		const TCHAR* entryPoint;
		D3D_SHADER_STAGE stage;
		TCOMPtr<ID3D10Blob*> byteCode;
	} passthroughShaders[] = 
	{
		{ _T("vs_main1"), D3D_SHADER_STAGE_VERTEX,   TCOMPtr<ID3D10Blob*>() },
		{ _T("vs_main2"), D3D_SHADER_STAGE_VERTEX,   TCOMPtr<ID3D10Blob*>() },
		{ _T("hs_main"),  D3D_SHADER_STAGE_HULL,     TCOMPtr<ID3D10Blob*>() },
		{ _T("ds_main"),  D3D_SHADER_STAGE_DOMAIN,   TCOMPtr<ID3D10Blob*>() },
		{ _T("gs_main"),  D3D_SHADER_STAGE_GEOMETRY, TCOMPtr<ID3D10Blob*>() },
	};

	for( UINT i = 0; i < ARRAY_SIZE(passthroughShaders); i++ )
	{
		TCOMPtr<ID3D10Blob*> rError;
		HRESULT hr = GetFramework()->CompileShaderFromResource(
			NULL,
			_T("FCVertexShaderPassThrough.fx"),
			passthroughShaders[i].entryPoint,
			passthroughShaders[i].stage,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_0,
			0,
			passthroughShaders[i].byteCode,
			rError,
			NULL,
			NULL );
		if( FAILED(hr) )
		{
			if( *rError )
			{
				WriteToLog( _T("%s, hr=%s"), rError->GetBufferPointer(), D3DHResultToString(hr).c_str() );
			}
			else
			{
				WriteToLog( _T("hr=%s"), D3DHResultToString(hr).c_str() );
			}
			return RESULT_FAIL;
		}
	}

	// Create shaders
	{
		HRESULT hr = S_OK;

		hr = GetDevice()->CreateVertexShader(
			passthroughShaders[0].byteCode->GetBufferPointer(),
			passthroughShaders[0].byteCode->GetBufferSize(),
			NULL,
			m_rPassthroughVS1 );
		IFR(hr);

		hr = GetDevice()->CreateVertexShader(
			passthroughShaders[1].byteCode->GetBufferPointer(),
			passthroughShaders[1].byteCode->GetBufferSize(),
			NULL,
			m_rPassthroughVS2 );
		IFR(hr);

		hr = GetDevice()->CreateHullShader(
			passthroughShaders[2].byteCode->GetBufferPointer(),
			passthroughShaders[2].byteCode->GetBufferSize(),
			NULL,
			m_rPassthroughHS );
		IFR(hr);

		hr = GetDevice()->CreateDomainShader(
			passthroughShaders[3].byteCode->GetBufferPointer(),
			passthroughShaders[3].byteCode->GetBufferSize(),
			NULL,
			m_rPassthroughDS );
		IFR(hr);

		hr = GetDevice()->CreateGeometryShaderWithStreamOutput(
			passthroughShaders[4].byteCode->GetBufferPointer(),
			passthroughShaders[4].byteCode->GetBufferSize(),
			SO_DECL,
			ARRAY_SIZE(SO_DECL),
			&SO_BUFFER_STRIDE,
			1,
			D3D11_SO_NO_RASTERIZED_STREAM,
			NULL,
			m_rPassthroughGS );
		IFR(hr);

		m_rPassthroughVSByteCode = passthroughShaders[0].byteCode;
	}

	return RESULT_PASS;
}

TEST_RESULT CFcallLimits::CompileTestShader( const std::string& shaderSource )
{
	const char* profile = "";

	switch( m_shaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:   profile = "vs_5_0"; break;
	case D3D_SHADER_STAGE_HULL:     profile = "hs_5_0"; break;
	case D3D_SHADER_STAGE_DOMAIN:   profile = "ds_5_0"; break;
	case D3D_SHADER_STAGE_GEOMETRY: profile = "gs_5_0"; break;
	case D3D_SHADER_STAGE_PIXEL:    profile = "ps_5_0"; break;
	case D3D_SHADER_STAGE_COMPUTE:  profile = "cs_5_0"; break;
	default: assert( false ); break;
	}

	// Compile the test Shader
	{ 
		TCOMPtr<ID3D10Blob*> rByteCode;
		TCOMPtr<ID3D10Blob*> rErrorBuff;
		{ // compile generated shader
			HRESULT hr = D3DX10CompileFromMemory( 
					shaderSource.c_str(),
					shaderSource.length(),
					"FCShaderGenerated.fx", 
					NULL, 			// No Defines
					NULL, 			// No Includes
					"main", 
					profile, 
					0, 0, NULL,     // Flags1, Flags2, ThreadPump
					rByteCode, 
					rErrorBuff, 
					NULL );			// result code for asynchronous compiling in a separate thread

			if( FAILED(hr) )
			{
				WriteToLog( _T("FCShaderGenerated.fx") );
				if( *rErrorBuff )
				{
					WriteToLog( _T("Compile Error: %s"), rErrorBuff->GetBufferPointer() );
				}
				else
				{
					WriteToLog( _T("Failed to compile shader: hr=%s"), D3DHResultToString(hr).c_str() );
				}
				return RESULT_FAIL;
			}

			// Generate the replacement asm for our deep tests
			if (m_depth > MAX_HLSL_DEPTH)
			{
				char buffer[2048];
				std::stringstream ss;
				ss << GetAssemblyHeader() << "\n";
				ss << gAsmHead;

				int iBody;

				iBody = 0;
				for (unsigned int i = 0 ; i < m_depth ; i++)
				{
					for (unsigned int j = 0 ; j < m_CallTree.CallTreeList[i] ; j++)
					{
						sprintf(buffer, gAsmDclBody, iBody);
						iBody++;
						ss << buffer;
					}
				}

				iBody = 0;
				for (unsigned int i = 0 ; i < m_depth ; i++)
				{
					for (unsigned int j = 0 ; j < m_CallTree.CallTreeList[i] ; j++)
					{
						sprintf(buffer, gAsmDclTable, iBody, iBody);
						iBody++;
						ss << buffer;
					}
				}

				char * comma;
				iBody = 0;
				for (unsigned int i = 0 ; i < m_depth ; i++)
				{
					sprintf(buffer, gAmsDclTnterface1, i);
					ss << buffer;
					comma = "";
					for (unsigned int j = 0 ; j < m_CallTree.CallTreeList[i] ; j++)
					{
						sprintf(buffer, gAmsDclTnterface2, iBody);
						iBody++;
						ss << comma << buffer;
						comma = ", ";
					}
					ss << gAmsDclTnterface3;
				}

				
				{
					const char* str1 = "";
					const char* str2 = "dcl_output o0.xyzw\n"
									   "dcl_output o1.xyzw\n"
									   "dcl_output o2.xyzw\n"
									   "dcl_output o3.xyzw\n";

					const char* str3 = "mov o0.xyzw, r0.xyzw\n"
									   "mov o1.xyzw, r1.xyzw\n"
									   "mov o2.xyzw, r2.xyzw\n"
									   "mov o3.xyzw, r3.xyzw\n";
					const char* str4 = "";

					switch( m_shaderStage )
					{
					case D3D_SHADER_STAGE_HULL:
						{
							// hull shader requires additional work around the main entrypoint
							str1 = "hs_control_point_phase\n";
							str4 = "hs_fork_phase\n"
								   "dcl_output_siv o0.x, finalTriUeq0EdgeTessFactor\n"
								   "mov o0.x, l(1.000000)\n"
								   "ret\n"
								   "hs_fork_phase\n"
								   "dcl_output_siv o1.x, finalTriVeq0EdgeTessFactor\n"
								   "mov o1.x, l(1.000000)\n"
								   "ret\n"
								   "hs_fork_phase\n"
								   "dcl_output_siv o2.x, finalTriWeq0EdgeTessFactor\n"
								   "mov o2.x, l(1.000000)\n"
								   "ret\n"
								   "hs_fork_phase\n"
								   "dcl_output_siv o3.x, finalTriInsideTessFactor\n"
								   "mov o3.x, l(1.000000)\n"
								   "ret\n";
						}
						break;

					case D3D_SHADER_STAGE_GEOMETRY:
						{
							// geometry shader requires data to be emiited
							str3 = "mov o0.xyzw, r0.xyzw\n"
								   "mov o1.xyzw, r1.xyzw\n"
								   "mov o2.xyzw, r2.xyzw\n"
								   "mov o3.xyzw, r3.xyzw\n"
								   "emit_stream m0\n"
								   "mov o0.xyzw, r0.xyzw\n"
								   "mov o1.xyzw, r1.xyzw\n"
								   "mov o2.xyzw, r2.xyzw\n"
								   "mov o3.xyzw, r3.xyzw\n"
								   "emit_stream m0\n"
								   "mov o0.xyzw, r0.xyzw\n"
								   "mov o1.xyzw, r1.xyzw\n"
								   "mov o2.xyzw, r2.xyzw\n"
								   "mov o3.xyzw, r3.xyzw\n"
								   "emit_stream m0\n";
						}
						break;

					case D3D_SHADER_STAGE_COMPUTE:
					case D3D_SHADER_STAGE_PIXEL:
						{
							// compute shader & pixel shader declare no outputs, and store results to UAV
							str2 = "";
							str3 =  "store_raw u1.xyzw, l(0),  r0\n"
									"store_raw u1.xyzw, l(16), r1\n"
									"store_raw u1.xyzw, l(32), r2\n"
									"store_raw u1.xyzw, l(48), r3\n";
						}
						break;
					}

					sprintf(buffer, gAsmMain, str1, str2, str3, str4);
					ss << buffer;
				}
				
				// Prime allocation is backwards compared to the HLSL version.  
				unsigned int primePtr = m_depth;
				for ( int i = m_depth-1 ; i >= 0 ; i-- )
					for ( unsigned int j = 0 ; j < m_CallTree.CallTreeList[i] ; j++ )
						primePtr++;

				iBody = 0;
				for (unsigned int i = 0 ; i < m_depth ; i++)
				{
					char CallStr[64];
					char const * const components[4] = {"x", "y", "z", "w"};
					CallStr[0] = '\0';  // Lowest level doesn't call lower

					if (i == m_depth - 1) 
					{
						CallStr[0] = '\0';
					}
					else
					{
						sprintf(CallStr, gAsmFcall, i+1);
					}
					unsigned int indexer =  min( i / 8 , 3 );
					unsigned int selector = (i / 2) % 4;
					for (unsigned int j = 0 ; j < m_CallTree.CallTreeList[i] ; j++)
					{
						primePtr--;
						sprintf(buffer, gAsmBody, iBody, i, indexer, components[selector], indexer, components[selector], indexer, components[selector]
								  , PrimeNumbers[primePtr], indexer, components[selector], CallStr);
						iBody++;
						ss << buffer;
					}
				}

				std::string strAsm = ss.str();

				D3DTEST_ASSEMBLER_ARGUMENTS args;

				ZeroMemory(&args, sizeof(args));
				args.pSrcString = strAsm.c_str();
				args.uSrcStringSize = strAsm.size();
				args.uAssemblerFlags = D3DX11_ASSEMBLER_ADD_FEATURE_INFO_BLOB;    
				args.pProtoShader = rByteCode->GetBufferPointer();
				args.uProtoShaderSize = static_cast<UINT>(rByteCode->GetBufferSize());
				args.qwFeatureInfoBlobContents = 
					g_TestApp.m_D3DOptions.Debug ? 0 : D3DX11_ASSEMBLER_RETAIL_SIGNED_ASSEMBLED_SHADER;

				hr = D3DTestAssembleShader( &args );

				if( FAILED(hr) )
				{
					WriteToLog( _T("FCShaderGenerated.fx") );
					if( *rErrorBuff )
					{
						WriteToLog( _T("Assemble Error: %s"), args.pErrorBlob->GetBufferPointer() );
					}
					else
					{
						WriteToLog( _T("Failed to assemble shader: hr=%s"), D3DHResultToString(hr).c_str() );
					}
					SAFE_RELEASE(args.pErrorBlob);
					return RESULT_FAIL;
				}
				SAFE_RELEASE(args.pErrorBlob);
				rByteCode = args.pShaderBlob;
				SAFE_RELEASE(args.pShaderBlob);

			}  // End special assemble code

			switch( m_shaderStage )
			{
			case D3D_SHADER_STAGE_VERTEX:
				hr = GetDevice()->CreateVertexShader( rByteCode->GetBufferPointer(), rByteCode->GetBufferSize(), m_pClassLinkage, m_rVertexShader );
				break;

			case D3D_SHADER_STAGE_HULL:  
				hr = GetDevice()->CreateHullShader( rByteCode->GetBufferPointer(), rByteCode->GetBufferSize(), m_pClassLinkage, m_rHullShader );
				break;

			case D3D_SHADER_STAGE_DOMAIN:   
				hr = GetDevice()->CreateDomainShader( rByteCode->GetBufferPointer(), rByteCode->GetBufferSize(), m_pClassLinkage, m_rDomainShader );
				break;

			case D3D_SHADER_STAGE_GEOMETRY: 
				hr = GetDevice()->CreateGeometryShaderWithStreamOutput( rByteCode->GetBufferPointer(), 
																		rByteCode->GetBufferSize(), 
																		SO_DECL, 
																		ARRAY_SIZE(SO_DECL), 
																		&SO_BUFFER_STRIDE, 
																		1, 
																		D3D11_SO_NO_RASTERIZED_STREAM, 
																		m_pClassLinkage, 
																		m_rGeometryShader );
				break;

			case D3D_SHADER_STAGE_PIXEL:    
				hr = GetDevice()->CreatePixelShader( rByteCode->GetBufferPointer(), rByteCode->GetBufferSize(), m_pClassLinkage, m_rPixelShader );
				break;

			case D3D_SHADER_STAGE_COMPUTE:  
				hr = GetDevice()->CreateComputeShader( rByteCode->GetBufferPointer(), rByteCode->GetBufferSize(), m_pClassLinkage, m_rComputeShader );
				break;

			default: assert( false ); break;
			}

			//
			// From the test spec:
			// "If the hardware returns out-of-memory during shader creation time, the test will return RESULT_SKIP so long as it is reasonable to do so."
			//
			if(E_OUTOFMEMORY == hr)
			{
				WriteToLog(_T("Create*Shader failed with E_OUTOFMEMORY, skipping test case"));
				return RESULT_SKIP;
			}

			IFRM(hr,_T("FCShaderGenerated.fx"));
		}
	}

	return RESULT_PASS;
}

TEST_RESULT CFcallLimits::CreateOutputBuffers()
{
	HRESULT hr = S_OK;

	// Output buffer
	UINT maxPrimitiveCount = 3;
	UINT bytesPerVertex = sizeof(UINT) * 4 * 4;
	UINT byteWidth = bytesPerVertex * maxPrimitiveCount;

	UINT bindFlags = 0;
	UINT miscFlags = 0;
	
	switch( m_shaderStage )
	{
	// VS, HS, DS, and GS, write with stream out
	// others write with UAV
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
	case D3D_SHADER_STAGE_GEOMETRY:
		bindFlags = D3D11_BIND_STREAM_OUTPUT;
		miscFlags = 0;
		break;

	default:
		bindFlags = D3D11_BIND_UNORDERED_ACCESS;
		miscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = bindFlags;
	bufferDesc.MiscFlags = miscFlags;

	hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, m_rOutputBuffer );
	IFR(hr);

	// Staging buffer for CPU access to output buffer
	bufferDesc.Usage          = D3D11_USAGE_STAGING;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.BindFlags      = 0;
	bufferDesc.MiscFlags      = 0;

	hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, m_rOutputBufferCopy );
	IFR(hr);

	// create unordered access view
	if( bindFlags & D3D11_BIND_UNORDERED_ACCESS )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory( &uavDesc, sizeof(uavDesc) );
		uavDesc.Format              = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements  = byteWidth / sizeof(UINT);
		uavDesc.Buffer.Flags        = D3D11_BUFFER_UAV_FLAG_RAW;

		hr = GetDevice()->CreateUnorderedAccessView( m_rOutputBuffer, &uavDesc, m_rUAV );
		IFR(hr);
	}

	return RESULT_PASS;
}

TEST_RESULT CFcallLimits::ReadOutputBuffers(unsigned int Results[16])
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr;

	GetImmediateContext()->CopyResource( m_rOutputBufferCopy, m_rOutputBuffer );

	hr = GetImmediateContext()->Map( m_rOutputBufferCopy, 0, D3D11_MAP_READ, 0, &mapped );
	IFR(hr);

	memcpy( Results, mapped.pData, sizeof(UINT) * 16 );

	GetImmediateContext()->Unmap( m_rOutputBufferCopy, 0 );

	return RESULT_PASS;
}

void CFcallLimits::BindResources( ID3D11ClassInstance** pInstances )
{
	ID3D11Buffer* pSOTargets[1] = { m_rOutputBuffer };
	UINT aSOOffset[1] = {0};

	ID3D11UnorderedAccessView* pUAVArray[1] = { m_rUAV };
	UINT aUAVInitialCount[1] = { 0 };
	
	switch( m_shaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
		{
			GetEffectiveContext()->VSSetConstantBuffers( 0, 1, &*m_rConstBuffer );

			GetEffectiveContext()->VSSetShader( m_rVertexShader, pInstances, m_depth );
			GetEffectiveContext()->GSSetShader( m_rPassthroughGS, NULL, 0 );

			GetEffectiveContext()->SOSetTargets( 1, pSOTargets, aSOOffset );

			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		}
		break;

	case D3D_SHADER_STAGE_HULL:
		{
			GetEffectiveContext()->HSSetConstantBuffers( 0, 1, &*m_rConstBuffer );

			GetEffectiveContext()->VSSetShader( m_rPassthroughVS1, NULL, NULL );
			GetEffectiveContext()->HSSetShader( m_rHullShader, pInstances, m_depth );
			GetEffectiveContext()->DSSetShader( m_rPassthroughDS, NULL, NULL );
			GetEffectiveContext()->GSSetShader( m_rPassthroughGS, NULL, 0 );

			GetEffectiveContext()->SOSetTargets( 1, pSOTargets, aSOOffset );

			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
		}
		break;

	case D3D_SHADER_STAGE_DOMAIN:
		{
			GetEffectiveContext()->DSSetConstantBuffers( 0, 1, &*m_rConstBuffer );

			GetEffectiveContext()->VSSetShader( m_rPassthroughVS1, NULL, NULL );
			GetEffectiveContext()->HSSetShader( m_rPassthroughHS, NULL, NULL );
			GetEffectiveContext()->DSSetShader( m_rDomainShader, pInstances, m_depth );
			GetEffectiveContext()->GSSetShader( m_rPassthroughGS, NULL, 0 );

			GetEffectiveContext()->SOSetTargets( 1, pSOTargets, aSOOffset );

			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
		}
		break;

	case D3D_SHADER_STAGE_GEOMETRY:
		{
			GetEffectiveContext()->GSSetConstantBuffers( 0, 1, &*m_rConstBuffer );

			GetEffectiveContext()->VSSetShader( m_rPassthroughVS1, NULL, NULL );
			GetEffectiveContext()->GSSetShader( m_rGeometryShader, pInstances, m_depth );

			GetEffectiveContext()->SOSetTargets( 1, pSOTargets, aSOOffset );

			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		}
		break;

	case D3D_SHADER_STAGE_PIXEL:
		{
			GetEffectiveContext()->PSSetConstantBuffers( 0, 1, &*m_rConstBuffer );

			GetEffectiveContext()->VSSetShader( m_rPassthroughVS2, NULL, NULL );
			GetEffectiveContext()->PSSetShader( m_rPixelShader, pInstances, m_depth );

			GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 0, NULL, NULL, 1, 1, pUAVArray, aUAVInitialCount );

			GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		}
		break;   

	case D3D_SHADER_STAGE_COMPUTE:
		{
			GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &*m_rConstBuffer );

			GetEffectiveContext()->CSSetShader( m_rComputeShader, pInstances, m_depth );

			GetEffectiveContext()->CSSetUnorderedAccessViews( 1, 1, pUAVArray, aUAVInitialCount );
		}
		break;

	default:
		assert( false );
	}
}

const char* CFcallLimits::GetAssemblyHeader()
{
	switch( m_shaderStage )
	{
	case D3D_SHADER_STAGE_VERTEX:
		return "vs_5_0";

	case D3D_SHADER_STAGE_HULL:
		return  "hs_5_0\n"
				"hs_decls\n"
				"dcl_input_control_point_count 3\n"
				"dcl_output_control_point_count 3\n"
				"dcl_tessellator_domain domain_tri\n"
				"dcl_tessellator_partitioning partitioning_integer\n"
				"dcl_tessellator_output_primitive output_triangle_cw\n"
				"dcl_hs_max_tessfactor 1.f\n";

	case D3D_SHADER_STAGE_DOMAIN:
		return "ds_5_0\n"
			   "dcl_input_control_point_count 3\n"
			   "dcl_tessellator_domain domain_tri\n";

	case D3D_SHADER_STAGE_GEOMETRY:
		return "gs_5_0\n"
			   "dcl_inputprimitive triangle\n"
			   "dcl_stream m0\n"
			   "dcl_outputtopology trianglestrip\n"
			   "dcl_maxout 3\n";
				
	case D3D_SHADER_STAGE_PIXEL:
		return "ps_5_0\n"
			   "dcl_uav_raw u1\n";

	case D3D_SHADER_STAGE_COMPUTE:
		return "cs_5_0\n"
			   "dcl_uav_raw u1\n"
			   "dcl_thread_group 1, 1, 1\n";

	default:
		assert( false );
		return "";
	}
}
