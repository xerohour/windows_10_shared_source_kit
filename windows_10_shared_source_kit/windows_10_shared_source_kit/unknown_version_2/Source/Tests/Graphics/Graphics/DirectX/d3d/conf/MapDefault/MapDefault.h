//////////////////////////////////////////////////////////////////////////
//  MapDefault.h
//  
//////////////////////////////////////////////////////////////////////////

#include <strsafe.h>
#include <time.h>
#include <stdio.h>
#include "D3D11Test.h"

// Namespaces for LogError
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;

// Useful routine
#define QPC_WITH_ERROR_CHECK( perfCount ) \
	if ( !QueryPerformanceCounter( &perfCount ) )\
	{\
		LogError( __FILEW__, __LINE__, L"QueryPeformanceCounter() failed ", GetLastError(), ERRORCODE_TYPE_WIN32, gFCConformanceFailure );\
		tr = RESULT_FAIL;\
		goto Exit;\
	}

#define QTCT_WITH_ERROR_CHECK( perfCount ) \
	if ( !QueryThreadCycleTime(GetCurrentThread(), &perfCount) )\
	{\
		LogError( __FILEW__, __LINE__, L"QueryThreadCycleTime() failed ", GetLastError(), ERRORCODE_TYPE_WIN32, gFCConformanceFailure );\
		tr = RESULT_FAIL;\
		goto Exit;\
	}

static const char szDispatchShader[] =
	"RWByteAddressBuffer bufUAV; \n"
	"[numthreads(%d,%d,%d)] \n" // x,y,z

	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  for (uint n = 0; n < 1 + threadIndex; ++n) \n"
	"    bufUAV.Store(threadIndex*4, n); \n"
	"} \n"
	;

// The below ETW Mark routine is taken from Dev's unit test
typedef struct _CE_SET_MARK_INFORMATION 
{
	ULONG Flag;
	WCHAR Mark[1];
} CE_SET_MARK_INFORMATION, *PCE_SET_MARK_INFORMATION; 

typedef ULONG (__stdcall *_CESetMarkRoutine) (__in_opt ULONGLONG TraceHandle,__in CE_SET_MARK_INFORMATION *MarkInfo,__in ULONG Size); 

inline VOID CEPRINTF(CHAR *Format, ...) 
{ 
	static _CESetMarkRoutine CESetMarkRoutine; 
	va_list Args; 
	ULONG BufferSize; 

	struct 
	{ 
		CE_SET_MARK_INFORMATION Header; 
		CHAR Buffer[1000];
	} Mark; 

	if (!CESetMarkRoutine) 
	{ 
		HMODULE ModuleNtdll = GetModuleHandleW(L"ntdll.dll"); 
		if(ModuleNtdll) 
		{ 
			CESetMarkRoutine = (_CESetMarkRoutine)GetProcAddress(ModuleNtdll, "EtwSetMark"); 
		} 
		if (!CESetMarkRoutine) 
			return; 
	} 

	va_start(Args, Format); 
	StringCbVPrintfExA((CHAR *)Mark.Header.Mark, sizeof(Mark.Buffer), 0, 0, 0, Format, Args); 
	BufferSize = FIELD_OFFSET(CE_SET_MARK_INFORMATION, Mark); 
	BufferSize += strlen((char *)Mark.Header.Mark) + 1; 
	CESetMarkRoutine(0, &Mark.Header, BufferSize);
};


//------------------------------------------------------------------------------------------------
class CMapDefaultZeroCopyTest : public CD3D11Test
{
public:
	// Constructor/Destructor
	CMapDefaultZeroCopyTest(void)
		: m_pDevice(NULL)
		, m_pEffectiveContext(NULL)
		, m_pDXGIDevice2(NULL)
		, m_SizeInBytes(0)
		, m_MapReadWrite(false)
		, m_pStagingBuffer(NULL)
		, m_pDefaultBuffer(NULL)
		, m_pResourceData(NULL)
	{
	}

	~CMapDefaultZeroCopyTest(void)
	{
	}

	// Test functions
    void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT ExecuteTestCase();
	void Cleanup();

protected:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pEffectiveContext;
	IDXGIDevice2 *m_pDXGIDevice2;

	UINT m_SizeInBytes;
	bool m_MapReadWrite;
	ID3D11Buffer *m_pStagingBuffer;
	ID3D11Buffer *m_pDefaultBuffer;
	BYTE *m_pResourceData;

	TEST_RESULT CreateBuffers();
	TEST_RESULT Scenario_Staging( LARGE_INTEGER& Cost ); 
	TEST_RESULT Scenario_Default( LARGE_INTEGER& Cost ); 
	LARGE_INTEGER CostScore( const LARGE_INTEGER * pCostPerTest, UINT numTests );
};


//------------------------------------------------------------------------------------------------
class CMapDefaultPerfTest : public CMapDefaultZeroCopyTest
{
public:
	// Constructor/Destructor
	CMapDefaultPerfTest(void)
		: m_CPUAccessFlags(0)
	{
	}

	~CMapDefaultPerfTest(void)
	{
	}

	// Test functions
    void InitTestParameters();
	TEST_RESULT ExecuteTestCase();

protected:
	UINT m_CPUAccessFlags;

	TEST_RESULT CreateBuffers();

	// Non-inlined *Loop() functions are used by the Scenario_*() functions to ensure identical code execution is measured

	__declspec(noinline) void WriteLoop(_Out_writes_(Size) BYTE* pDstData, _In_reads_(Size) const BYTE* pSrcData, UINT Size);
	TEST_RESULT Scenario_Write( ULONG64 &CostStaging, ULONG64 &CostDefault, UINT retry );

	__declspec(noinline) UINT ReadLoop(_In_reads_(Size) const BYTE* pData, UINT Size, UINT numReps);
	TEST_RESULT Scenario_Read( ULONG64 &CostStaging, ULONG64 &CostDefault, UINT retry );

	__declspec(noinline) void ReadWriteLoop(_Inout_updates_(Size) BYTE* pData, UINT Size, UINT numReps);
	TEST_RESULT Scenario_ReadWrite( ULONG64 &CostStaging, ULONG64 &CostDefault, UINT retry );

	ULONG64 CostScore( const ULONG64 * pCostPerTest, UINT numTests );
};

//------------------------------------------------------------------------------------------------
class CMapDefaultApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	virtual bool InitTestGroups();
	virtual void InitOptionVariables();
};

extern CMapDefaultApp  g_TestApp;