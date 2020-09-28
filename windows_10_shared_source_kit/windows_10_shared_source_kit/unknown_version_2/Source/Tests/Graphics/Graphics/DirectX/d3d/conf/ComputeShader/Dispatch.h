#pragma once

// Includes
#include "WGFCompute.h"

// Defines
#define MAX_NUM_CS_OUTPUT_RESOURCES 8

// Enums and structs
enum CS_STATE
{
	CS_STATE_NULL,
	CS_STATE_NO_OUTPUT,
	CS_STATE_OUTPUT
};

enum COMPUTE_DISPATCH_METHOD
{
	COMPUTE_DISPATCH_METHOD_DISPATCH,
	COMPUTE_DISPATCH_METHOD_INDIRECT
};

struct COMPUTE_DISPATCH_DIMENSIONS
{
	UINT x;
	UINT y;
	UINT z;

	bool operator== (const COMPUTE_DISPATCH_DIMENSIONS &rhs) const
	{
		if( x == rhs.x && y == rhs.y && z == rhs.z )
			return true;

		return false;
	}
};

struct COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP
{
	UINT x;
	UINT y;
	UINT z;

	bool operator== (const COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP &rhs) const
	{
		if( x == rhs.x && y == rhs.y && z == rhs.z )
			return true;

		return false;
	}
};

struct SV_TEST_RESULT
{
	UINT dispatchThreadID[3];
	UINT groupID[3];
	UINT groupThreadID[3];
	UINT groupIndex;
};

DECLARE_NAMED_VALUES( COMPUTE_DISPATCH_METHOD );
DECLARE_NAMED_VALUES( CS_STATE );

//------------------------------------------------------------------------------------------------

class CDispatchTest :
	public CComputeTest
{
protected:
	CS_STATE								m_CSState;
	COMPUTE_DISPATCH_METHOD					m_dispatchMethod;
	COMPUTE_DISPATCH_DIMENSIONS				m_dispatchDimensions;
	COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP	m_dispatchThreadsPerGroup;
	UINT									m_numBoundOutputResources;
	ID3D11Buffer							*m_pDispatchIndirectArgs;
	ID3D11Buffer							*m_pTargetUABuffers[MAX_NUM_CS_OUTPUT_RESOURCES];
	ID3D11Buffer							*m_pTargetStagingBuffers[MAX_NUM_CS_OUTPUT_RESOURCES];
	ID3D11UnorderedAccessView				*m_pTargetUAVs[MAX_NUM_CS_OUTPUT_RESOURCES];

	D3D11_BUFFER_DESC					m_destBuffDesc;
	D3D11_BUFFER_DESC					m_destStageBuffDesc;
	D3D11_UNORDERED_ACCESS_VIEW_DESC	m_destUAVDesc;

	// test cases with large number of threads will be verified through multiple passes to reduce the 
	// memory requirement for the output resource
	bool	m_bMultiPasses;
	UINT	m_currentPass;
	UINT	m_groupStep;


public:
	CDispatchTest(void)
		: m_dispatchMethod(COMPUTE_DISPATCH_METHOD_DISPATCH)
		, m_pDispatchIndirectArgs(NULL)
		, m_bMultiPasses(false)
		, m_currentPass(0)
		, m_groupStep(0)
	{
		for(UINT i = 0; i < MAX_NUM_CS_OUTPUT_RESOURCES; ++i)
		{
			m_pTargetUABuffers[i] = NULL;
			m_pTargetStagingBuffers[i] = NULL;
			m_pTargetUAVs[i] = NULL;
		}
	}

	virtual ~CDispatchTest(void);

	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
	virtual void Cleanup();

	// Helper functions (pure virtual)
	virtual TEST_RESULT SetupCS() = 0;
	virtual TEST_RESULT VerifyCSResult(const D3D11_MAPPED_SUBRESOURCE mappedResources[MAX_NUM_CS_OUTPUT_RESOURCES]) = 0;
	virtual TEST_RESULT VerifyCSInvocations(const D3D11_QUERY_DATA_PIPELINE_STATISTICS &pipelineStats) = 0;
};

//------------------------------------------------------------------------------------------------

class CCallTest :
	public CDispatchTest
{
public:
	CCallTest(void)
	{
	}

	~CCallTest(void)
	{
	}

	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();

	// Helper functions
	TEST_RESULT SetupCS();
	TEST_RESULT VerifyCSResult(const D3D11_MAPPED_SUBRESOURCE mappedResources[MAX_NUM_CS_OUTPUT_RESOURCES]);
	TEST_RESULT VerifyCSInvocations(const D3D11_QUERY_DATA_PIPELINE_STATISTICS &pipelineStats);
};

//------------------------------------------------------------------------------------------------

class CSVTest :
	public CDispatchTest
{
public:
	CSVTest(void)
	{
		// Only need 1 output resource for this test
		m_numBoundOutputResources = 1;
	}

	~CSVTest(void)
	{
	}

	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual void CleanupTestCase();

	// Helper functions
	TEST_RESULT SetupCS();
	TEST_RESULT VerifyCSResult(const D3D11_MAPPED_SUBRESOURCE mappedResources[MAX_NUM_CS_OUTPUT_RESOURCES]);
	TEST_RESULT VerifyCSInvocations(const D3D11_QUERY_DATA_PIPELINE_STATISTICS &pipelineStats);
};

