#pragma once
#include "D3D11Test.h"
#include "WGFTestCore.h"
#include <vector>
#include <sstream>

#include "ShaderCode.h"


#define LOG_HEADER ("[" __FUNCTION__ "()]")
#define ZeroStructMem(x) ZeroMemory(&x, sizeof(x))
#define SAFE_RELEASE_ARRAY(x) for (int n = 0; n < ARRAYSIZE(x); ++n) { SAFE_RELEASE((x)[n]); }
#define ON_EACH_DEVICE for(int DEVICE_ID = 0; DEVICE_ID < ARRAYSIZE(m_pDeviceArray); ++DEVICE_ID) m_pDeviceArray[DEVICE_ID]
#define ON_EACH_CONTEXT for(int CONTEXT_ID = 0; CONTEXT_ID < ARRAYSIZE(m_pContextArray); ++CONTEXT_ID) m_pContextArray[CONTEXT_ID]


enum HullOutputType
{
	HullOutput_ControlPoints,
	HullOutput_PatchConstants,
};
DECLARE_NAMED_VALUES( HullOutputType );

typedef struct _VERT {
	float   x;      // aka U
	float   y;      // aka V
	float   z;      // aka W
	float   w;
} VERT, *PVERT;

typedef struct _TRI {
	VERT    vert[3];
} TRI, *PTRI;

typedef struct _LINE {
	VERT    vert[2];
} LINE, *PLINE;

#include "TestBuffer.h"

enum SHADER_SOURCE_LOCATION
{
	SHADER_SOURCE_RESOURCE_HLSL,
	SHADER_SOURCE_MEMORY_HLSL,
	SHADER_SOURCE_MEMORY_ASM,
};

struct SHADER_RECORD
{
	tstring                                        shaderCode;
	SHADER_SOURCE_LOCATION                         location;
};

//    
class CTessellationConfTest : public CD3D11Test
{
public:
	CTessellationConfTest()
		: m_pDevice(NULL)
		, m_pDeviceContext(NULL)
		, m_pRefDevice(NULL)
		, m_pRefDeviceContext(NULL)
		, m_nNumTrisInput(0)
		, m_hcompiler(NULL)
	{
		ZeroStructMem(m_pConstBuffers);
		ZeroStructMem(m_TessVals);
		ZeroStructMem(m_uintTessFactors);
		ZeroStructMem(m_pVS);
		ZeroStructMem(m_pHS);
		ZeroStructMem(m_pDS);
		ZeroStructMem(m_pGS);
		ZeroStructMem(m_pSOBuffer);
		ZeroStructMem(m_pSOQuery);
		ZeroStructMem(m_pPipeStatsQuery);

		m_VertexShader.location = SHADER_SOURCE_RESOURCE_HLSL;
		m_HullShader.location   = SHADER_SOURCE_MEMORY_HLSL;
		m_DomainShader.location = SHADER_SOURCE_MEMORY_HLSL;
	}
	virtual TEST_RESULT Setup();
	virtual void Cleanup( );

	TEST_RESULT SetupResources(D3D11_SO_DECLARATION_ENTRY* soDecls, UINT numSoDecls);
	void CleanupResources();

protected:
	union
	{
		struct
		{
			ID3D11Device			*m_pDevice;
			ID3D11Device			*m_pRefDevice;
		};
		ID3D11Device* m_pDeviceArray[2];
	};
	union
	{
		struct
		{
			ID3D11DeviceContext		*m_pDeviceContext;
			ID3D11DeviceContext		*m_pRefDeviceContext;
		};
		ID3D11DeviceContext* m_pContextArray[2];
	};

	ID3D11VertexShader				*m_pVS[2];
	ID3D11HullShader				*m_pHS[2];
	ID3D11DomainShader				*m_pDS[2];
	ID3D11GeometryShader			*m_pGS[2];
	ID3D11Buffer					*m_pConstBuffers[2][2];
	ID3D11Buffer					*m_pSOBuffer[2];
	ID3D11Query						*m_pSOQuery[2];
	ID3D11Query						*m_pPipeStatsQuery[2];

	tstring			        m_DomainName;
	tstring                 m_PartitionName;
	tstring                 m_TopologyName;
	float					m_TessVals[4];
	UINT					m_uintTessFactors[4];
	float					m_TessFactors[4];
	UINT                    m_nNumTrisInput;

	SHADER_RECORD           m_VertexShader;
	SHADER_RECORD           m_HullShader;
	SHADER_RECORD           m_DomainShader;

	TEST_RESULT  CheckPipeStats( D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeStats[2], int size = 2 );
	TEST_RESULT  CheckSOQueries( D3D11_QUERY_DATA_SO_STATISTICS soStats[2], int size = 2, bool validateResults = true );
	
	
	template<typename Ty> TEST_RESULT  MatchTessellationBuffers( UINT64 bufferSizes[2] );

private:
	TEST_RESULT  SetupShaders( D3D11_SO_DECLARATION_ENTRY* soDecls, UINT numSoDecls );
	TEST_RESULT  SetupBuffers( );
	HMODULE m_hcompiler;
};

//
class CTessellatorTest : public CTessellationConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
private:
	TEST_RESULT VersusRefTest();
	template<typename Ty> TEST_RESULT VersusSelfTest();

	tstring                 m_Versus;
};

//
class CHullShaderTest : public CTessellationConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

protected:

	UINT			m_NumCPsIn;
	UINT            m_NumInCPElements;
	UINT            m_NumOutCPElements;
	HullOutputType	m_OutputType;
	tstring			m_ConstDataType;
	tstring         m_CPElementsDataType;
	bool            m_HasCPPhase;
	TEST_RESULT (CHullShaderTest::* m_pfnMatchTessellationBuffers)( UINT64 bufferSizes[2] );
};

//
class CDomainShaderTest : public CTessellationConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
};


//
class CHullShaderPhasesTest : public CTessellationConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

	enum TESS_FACTOR_ASSIGNMENT
	{
		TESS_FACTOR_IN_ONE_JOIN_INSTANCED,        // All tesselation factors are output by 1 join phase (which is instanced)
		TESS_FACTOR_IN_SEPARATE_JOIN,             // There are N join phases, each outputs 1 tesselation factor
		TESS_FACTOR_IN_ONE_FORK_INSTANCED,        // All tesselation factors are output by 1 fork phase (which is instanced)
		TESS_FACTOR_IN_SEPARATE_FORK,             // There are N fork phases, each outputs 1 tesselation factor
		TESS_FACTOR_EDGE_IN_FORK,                 // There are N fork phases, each outputs 1 edge tesseleation factor.  There are 2 join phases that each output 1 inside tesselation factor
		TESS_FACTOR_EDGE_IN_JOIN,                 // There are N joinphases, each outputs 1 edge tesseleation factor.  There are 2 fork phases that each output 1 inside tesselation factor
		TESS_FACTOR_SPECIAL,                      // NAN or INF tesselation factor
	};

private:
	TESS_FACTOR_ASSIGNMENT m_TessFactorPhase;
	bool                   m_ExtraPatchConstantData; // true if the patch constant data contains more than just tesselation factors
	UINT                   m_NumInputControlPoints;
	UINT                   m_NumOutputControlPoints;
};

//
class CTessellationConfApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	bool InitTestGroups();
	void InitOptionVariables();
};

// The Test App
extern CTessellationConfApp  g_TestApp;


float  f_getNaN();
bool  isNaN(float in);
bool  isINF(float in);

//
struct float4_sortable
{
	struct {
		float x, y, z, w;
	} fl4;

	// HW and REF results should be bit-for-bit similar and able to be compared
	// as RAW bits.
	bool operator < ( const float4_sortable& rhs ) 
	{
		if ( ( memcmp( &fl4, &rhs.fl4, sizeof(fl4) ) >= 0 ) )
		{
			return true;
		}

		return false;
	}

	bool operator == ( const float4_sortable& rhs ) 
	{
		if ( ( memcmp( &fl4, &rhs.fl4, sizeof(fl4) ) == 0 ) )
		{
			return true;
		}

		return false;
	}

	tstring  ToString()
	{
		return FormatString( _T("{%g, %g, %g, %g}"), fl4.x, fl4.y, fl4.z, fl4.w );
	}
};

//
struct tri_sortable
{
	TRI     tri;

	// HW and REF results should be bit-for-bit similar and able to be compared
	// as RAW bits.
	bool operator < ( const tri_sortable& rhs ) 
	{
		if ( ( memcmp( &tri, &rhs.tri, sizeof(tri) ) >= 0 ) )
		{
			return true;
		}

		return false;
	}

	tstring  ToString()
	{
		return FormatString( _T("v0{%g, %g, %g, %g}, v1{%g, %g, %g, %g}, v2{%g, %g, %g, %g}"),
							 tri.vert[0].x, tri.vert[0].y, tri.vert[0].z, tri.vert[0].w,
							 tri.vert[1].x, tri.vert[1].y, tri.vert[1].z, tri.vert[1].w,
							 tri.vert[2].x, tri.vert[2].y, tri.vert[2].z, tri.vert[2].w );
	}
};

//
struct line_sortable
{
	LINE     line;

	// HW and REF results should be bit-for-bit similar and able to be compared
	// as RAW bits.
	bool operator < ( const line_sortable& rhs ) 
	{
		if ( ( memcmp( &line, &rhs.line, sizeof(line) ) >= 0 ) )
		{
			return true;
		}

		return false;
	}

	tstring  ToString()
	{
		return FormatString( _T("v0{%g, %g, %g, %g}, v1{%g, %g, %g, %g}, v2{%g, %g, %g, %g}"),
							 line.vert[0].x, line.vert[0].y, line.vert[0].z, line.vert[0].w,
							 line.vert[1].x, line.vert[1].y, line.vert[1].z, line.vert[1].w );
	}
};

//
template<typename Ty> 
TEST_RESULT  CTessellationConfTest::MatchTessellationBuffers( UINT64 bufferSizes[2] )
{                                                            
	tstring topologyName = GetTestCaseParameter( _T("Topology") )->GetValueAsString();

	// create first buffer
	std::auto_ptr< TestBuffer<Ty> > buffer0( new TestBuffer<Ty>( 
		m_pDeviceArray[0], m_pContextArray[0], m_pSOBuffer[0], 
		(size_t)bufferSizes[0], topologyName ) );
		
	if (buffer0->HasError())
	{
		WriteToLog( _T( "%s %s" ), LOG_HEADER, buffer0->ErrorString().c_str() );
		return RESULT_FAIL;
	}
		
	// create second buffer
	std::auto_ptr< TestBuffer<Ty> > buffer1( new TestBuffer<Ty>( 
		m_pDeviceArray[1], m_pContextArray[1], m_pSOBuffer[1], 
		(size_t)bufferSizes[1], topologyName ) );

	if (buffer1->HasError())
	{
		WriteToLog( _T( "%s %s" ), LOG_HEADER, buffer1->ErrorString().c_str() );
		return RESULT_FAIL;
	}
		
	// sort data in both buffers
	buffer0->StandardizeVertexSequence();	    
	buffer0->SortVertices();	    

	buffer1->StandardizeVertexSequence();	    
	buffer1->SortVertices();	    
	
	// and compare
	TEST_RESULT result = buffer0->Compare( buffer1.get() );
	if (buffer0->HasError())
	{
		WriteToLog( _T( "%s - Domain: %s   Partitioning: %s   Topology: %s\n" ), LOG_HEADER, 
			GetTestCaseParameter( _T("Domain") )->GetValueAsString().c_str(),
			GetTestCaseParameter( _T("Partition") )->GetValueAsString().c_str(),
			GetTestCaseParameter( _T("Topology") )->GetValueAsString().c_str() );
		WriteToLog( _T(" %s "), buffer0->ErrorString().c_str() ); 
	}

	return result;    
}
