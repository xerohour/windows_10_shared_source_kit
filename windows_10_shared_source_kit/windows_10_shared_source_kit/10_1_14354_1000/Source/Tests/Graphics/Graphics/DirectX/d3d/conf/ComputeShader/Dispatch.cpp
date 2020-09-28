
// Includes
#include "Dispatch.h"

//------------------------------------------------------------------------------------------------
// Shaders

static const UINT g_clearVal = 0xbaadf00d;
static const char szDispatchShader_NoOutput[] =
	"groupshared static uint sharedMemory[%d]; \n" // arraysize = x*y*z
	"[numthreads(%d,%d,%d)] \n" // x,y,z

	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  sharedMemory[threadIndex] = threadIndex; \n"
	"} \n"
	;

static const char szDispatchShader_WithOutput[] =
	"struct output \n"
	"{ \n"
	"  uint sum; \n"
	"}; \n"

	"RWStructuredBuffer<output> result[%d]; \n"	// The number of output resources
	"groupshared static uint sharedMemory[%d]; \n" // arraysize = x*y*z
	"[numthreads(%d,%d,%d)] \n" // x,y,z

	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  sharedMemory[threadIndex] = uint(1); \n"
	"  GroupMemoryBarrierWithGroupSync(); \n"
	"  if( 0 == threadIndex ) \n"
	"  { \n"
	"    uint flatGroupID = ( groupID.z * %d * %d ) + ( groupID.y * %d ) + groupID.x; \n"	// x, y, x (Same as how SV_GroupIndex is computed)
	"    uint outputResourceIndex = flatGroupID %% %d; \n"	// The number of output resources
	"    uint sum = 0; \n"
	"    for( uint i = 0; i < %d; ++i ) \n"	// Add all the elements in the shared memory
	"    { \n"
	"      sum += sharedMemory[i]; \n"
	"    } \n"

	"    switch( outputResourceIndex ) \n"
	"    { \n"
	"      case 0: \n"   
	"        result[0][flatGroupID].sum = sum; \n"	// Output the sum from all threads in the current group to the structured buffer
	"        break; \n"
	"      case 1: \n"   
	"        result[1][flatGroupID].sum = sum; \n"	// Output the sum from all threads in the current group to the structured buffer
	"        break; \n"
	"      case 2: \n"   
	"        result[2][flatGroupID].sum = sum; \n"	// Output the sum from all threads in the current group to the structured buffer
	"        break; \n"
	"      case 3: \n"   
	"        result[3][flatGroupID].sum = sum; \n"	// Output the sum from all threads in the current group to the structured buffer
	"        break; \n"
	"      case 4: \n"   
	"        result[4][flatGroupID].sum = sum; \n"	// Output the sum from all threads in the current group to the structured buffer
	"        break; \n"
	"      case 5: \n"   
	"        result[5][flatGroupID].sum = sum; \n"	// Output the sum from all threads in the current group to the structured buffer
	"        break; \n"
	"      case 6: \n"   
	"        result[6][flatGroupID].sum = sum; \n"	// Output the sum from all threads in the current group to the structured buffer
	"        break;\n"
	"      case 7: \n"   
	"        result[7][flatGroupID].sum = sum; \n"	// Output the sum from all threads in the current group to the structured buffer
	"        break; \n"
	"    } \n"
	"  } \n"
	"} \n"
	;

static const char szSVShader[] =
	"struct output \n"
	"{ \n"
	"  uint3 dispatchThreadID; \n"
	"  uint3 groupID; \n"
	"  uint3 groupThreadID; \n"
	"  uint groupIndex; \n"
	"}; \n"

	"RWStructuredBuffer<output> result; \n"
	"[numthreads(%d,%d,%d)] \n" // x,y,z

	"#define MIN_GROUP_INDEX %d \n"
	"#define MAX_GROUP_INDEX %d \n"

	"void cs_main(uint3 dispatchID : SV_DispatchThreadID, uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  uint flatGroupThreadID = ( threadID.z * %d * %d ) + ( threadID.y * %d ) + threadID.x; \n"	// x, y, x (Thread dimensions)
	"  uint flatGroupID = ( groupID.z * %d * %d ) + ( groupID.y * %d ) + groupID.x; \n"	// x, y, x (Same as how SV_GroupIndex is computed) (Group Dimensions)
	"  if ( flatGroupID >= MIN_GROUP_INDEX && flatGroupID <= MAX_GROUP_INDEX ) \n"
	"  { \n "
	"      uint resultIndex = ( ( flatGroupID - MIN_GROUP_INDEX ) * %d ) + flatGroupThreadID; \n"	// Number of threads per group dispatched
	"      result[resultIndex].dispatchThreadID		= dispatchID; \n"
	"      result[resultIndex].groupID				= groupID; \n"
	"      result[resultIndex].groupThreadID		= threadID; \n"
	"      result[resultIndex].groupIndex			= threadIndex; \n"
	"  } \n "
	"} \n"
	;



//------------------------------------------------------------------------------------------------
// Test case data

#define MAX_X_DISPATCH_DIMENSION_FL10 D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION 
#define MAX_Y_DISPATCH_DIMENSION_FL10 D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION
#define MAX_Z_DISPATCH_DIMENSION_FL10 1

#define MIN_X_DISPATCH_DIMENSION_FL10 1
#define MIN_Y_DISPATCH_DIMENSION_FL10 1
#define MIN_Z_DISPATCH_DIMENSION_FL10 1

#define MAX_X_DISPATCH_DIMENSION_FL11 D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION 
#define MAX_Y_DISPATCH_DIMENSION_FL11 D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION
#define MAX_Z_DISPATCH_DIMENSION_FL11 D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION

#define MIN_X_DISPATCH_DIMENSION_FL11 1
#define MIN_Y_DISPATCH_DIMENSION_FL11 1
#define MIN_Z_DISPATCH_DIMENSION_FL11 1

#define MAX_X_THREADS_PER_GROUP_FL10 768
#define MAX_Y_THREADS_PER_GROUP_FL10 768
#define MAX_Z_THREADS_PER_GROUP_FL10 64

#define MIN_X_THREADS_PER_GROUP_FL10 1
#define MIN_Y_THREADS_PER_GROUP_FL10 1
#define MIN_Z_THREADS_PER_GROUP_FL10 1

#define MAX_X_THREADS_PER_GROUP_FL11 1024
#define MAX_Y_THREADS_PER_GROUP_FL11 1024
#define MAX_Z_THREADS_PER_GROUP_FL11 64

#define MIN_X_THREADS_PER_GROUP_FL11 1
#define MIN_Y_THREADS_PER_GROUP_FL11 1
#define MIN_Z_THREADS_PER_GROUP_FL11 1

// IMPORTANT NOTE!!!! Combinations of large x's, y's, and z's will result in extremly slow behavior. Testing this is unrealistic. (Talking about Trillions of threads)
COMPUTE_DISPATCH_DIMENSIONS g_dispatchDimensionValuesFL10[] =
{
	// Changes on X
	{ MIN_X_DISPATCH_DIMENSION_FL10 + 1,	MIN_Y_DISPATCH_DIMENSION_FL10, MIN_Z_DISPATCH_DIMENSION_FL10 },
	{ 101,									MIN_Y_DISPATCH_DIMENSION_FL10, MIN_Z_DISPATCH_DIMENSION_FL10 },
	{ MAX_X_DISPATCH_DIMENSION_FL10,		MIN_Y_DISPATCH_DIMENSION_FL10, MIN_Z_DISPATCH_DIMENSION_FL10 },

	// Changes on Y
	{ MIN_X_DISPATCH_DIMENSION_FL10, MIN_Y_DISPATCH_DIMENSION_FL10 + 1, MIN_Z_DISPATCH_DIMENSION_FL10 },
	{ MIN_X_DISPATCH_DIMENSION_FL10, MAX_Y_DISPATCH_DIMENSION_FL10 - 1, MIN_Z_DISPATCH_DIMENSION_FL10 },
	{ MIN_X_DISPATCH_DIMENSION_FL10, MAX_Y_DISPATCH_DIMENSION_FL10,		MIN_Z_DISPATCH_DIMENSION_FL10 },

	// No changes on Z for FL10

	// Combinations (NOTE, combinations of large x's, y's, and z's will result in extremly slow behavior. Testing this is unrealistic.
	{ 23, 17, MIN_Z_DISPATCH_DIMENSION_FL10 },
	{ MIN_X_DISPATCH_DIMENSION_FL10, MIN_Y_DISPATCH_DIMENSION_FL10,		MIN_Z_DISPATCH_DIMENSION_FL10 },
	{ 500, 501, MIN_Z_DISPATCH_DIMENSION_FL10 },
	//{ MAX_X_DISPATCH_DIMENSION_FL10, MAX_Y_DISPATCH_DIMENSION_FL10, MIN_Z_DISPATCH_DIMENSION_FL10 }, // Unrealistic, this will take ages
};

COMPUTE_DISPATCH_DIMENSIONS g_dispatchDimensionValuesFL11[] =
{
	// Changes on X
	{ MIN_X_DISPATCH_DIMENSION_FL11 + 1,	MIN_Y_DISPATCH_DIMENSION_FL11, MIN_Z_DISPATCH_DIMENSION_FL11 },
	{ MAX_X_DISPATCH_DIMENSION_FL11,		MIN_Y_DISPATCH_DIMENSION_FL11, MIN_Z_DISPATCH_DIMENSION_FL11 },

	// Changes on Y
	{ MIN_X_DISPATCH_DIMENSION_FL11, 7,									MIN_Z_DISPATCH_DIMENSION_FL11 },
	{ MIN_X_DISPATCH_DIMENSION_FL11, MAX_Y_DISPATCH_DIMENSION_FL11,		MIN_Z_DISPATCH_DIMENSION_FL11 },

	// Changes on Z
	{ MIN_X_DISPATCH_DIMENSION_FL11, MIN_Y_DISPATCH_DIMENSION_FL11, MAX_Z_DISPATCH_DIMENSION_FL11 - 1 },
	{ MIN_X_DISPATCH_DIMENSION_FL11, MIN_Y_DISPATCH_DIMENSION_FL11, MAX_Z_DISPATCH_DIMENSION_FL11 },

	// Combinations (NOTE, combinations of large x's, y's, and z's will result in extremly slow behavior. Testing this is unrealistic.
	{ 23, 17, 111 },
	{ MIN_X_DISPATCH_DIMENSION_FL11, MIN_Y_DISPATCH_DIMENSION_FL11, MIN_Z_DISPATCH_DIMENSION_FL11 },
	{ 80, 79, 81 },
	//{ MAX_X_DISPATCH_DIMENSION_FL11, MAX_Y_DISPATCH_DIMENSION_FL11, MAX_Z_DISPATCH_DIMENSION_FL11 }, // Unrealistic, this will take ages
};

COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP g_dispatchNumThreadsInGroupValuesFL10[] =
{
	// Changes on X
	{ MIN_X_THREADS_PER_GROUP_FL10 + 1,	MIN_Y_THREADS_PER_GROUP_FL10, MIN_Z_THREADS_PER_GROUP_FL10 },
	{ MAX_X_THREADS_PER_GROUP_FL10,		MIN_Y_THREADS_PER_GROUP_FL10, MIN_Z_THREADS_PER_GROUP_FL10 },

	// Changes on Y
	{ MIN_X_THREADS_PER_GROUP_FL10, 700,								MIN_Z_THREADS_PER_GROUP_FL10 },
	{ MIN_X_THREADS_PER_GROUP_FL10, MAX_Y_THREADS_PER_GROUP_FL10,		MIN_Z_THREADS_PER_GROUP_FL10 },

	// Changes on Z
	{ MIN_X_THREADS_PER_GROUP_FL10, MIN_Y_THREADS_PER_GROUP_FL10, MAX_Z_THREADS_PER_GROUP_FL10 - 1 },
	{ MIN_X_THREADS_PER_GROUP_FL10, MIN_Y_THREADS_PER_GROUP_FL10, MAX_Z_THREADS_PER_GROUP_FL10 },

	// Combinations
	{ 2, 96, 4},	// 768
	{ 3, 256, 1},	// 768
	{ MIN_X_THREADS_PER_GROUP_FL10, MIN_Y_THREADS_PER_GROUP_FL10, MIN_Z_THREADS_PER_GROUP_FL10 },
};

COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP g_dispatchNumThreadsInGroupValuesFL11[] =
{
	// Changes on X
	{ 769,								MIN_Y_THREADS_PER_GROUP_FL11, MIN_Z_THREADS_PER_GROUP_FL11 },
	{ MAX_X_THREADS_PER_GROUP_FL11,		MIN_Y_THREADS_PER_GROUP_FL11, MIN_Z_THREADS_PER_GROUP_FL11 },

	// Changes on Y
	{ MIN_X_THREADS_PER_GROUP_FL11, MIN_Y_THREADS_PER_GROUP_FL11 + 1,	MIN_Z_THREADS_PER_GROUP_FL11 },
	{ MIN_X_THREADS_PER_GROUP_FL11, MAX_Y_THREADS_PER_GROUP_FL11,		MIN_Z_THREADS_PER_GROUP_FL11 },

	// Changes on Z
	{ MIN_X_THREADS_PER_GROUP_FL11, MIN_Y_THREADS_PER_GROUP_FL11, MAX_Z_THREADS_PER_GROUP_FL11 - 1 },
	{ MIN_X_THREADS_PER_GROUP_FL11, MIN_Y_THREADS_PER_GROUP_FL11, MAX_Z_THREADS_PER_GROUP_FL11 },

	// Combinations
	{ 341, 3, 1},	// 1023
	{ 2, 8, 64},	// 1024
	{ 512, 1, 2},	// 1024
	{ MIN_X_THREADS_PER_GROUP_FL11, MIN_Y_THREADS_PER_GROUP_FL11, MIN_Z_THREADS_PER_GROUP_FL11 },
};

//------------------------------------------------------------------------------------------------

BEGIN_NAMED_VALUES( COMPUTE_DISPATCH_METHOD )
	NAMED_VALUE( _T( "Dispatch" ), COMPUTE_DISPATCH_METHOD_DISPATCH )
	NAMED_VALUE( _T( "DispatchIndirect" ), COMPUTE_DISPATCH_METHOD_INDIRECT )
END_NAMED_VALUES( COMPUTE_DISPATCH_METHOD )

BEGIN_NAMED_VALUES( CS_STATE )
	NAMED_VALUE( _T( "NULL" ), CS_STATE_NULL )
	NAMED_VALUE( _T( "NoOutput" ), CS_STATE_NO_OUTPUT )
	NAMED_VALUE( _T( "Output" ), CS_STATE_OUTPUT )
END_NAMED_VALUES( CS_STATE )

tstring __stdcall ToString( const COMPUTE_DISPATCH_DIMENSIONS& dimensions )
{
	return ToString( dimensions.x ) + tstring( ", " ) + ToString( dimensions.y ) + tstring( ", " ) + ToString( dimensions.z );
}

bool __stdcall FromString( COMPUTE_DISPATCH_DIMENSIONS *pDimensions, const tstring &str )
{
	// Local variables
   	tstring::size_type lastPos = 0;
	UINT *pCurrDimension = &(pDimensions->x);

	// Find the components in the string
	for (UINT i = 0; i < 3; ++i)
    {
		// Find the current item in pDimensions
		if( 1 == i ) 
			pCurrDimension = &(pDimensions->y);
		else if( 2 == i ) 
			pCurrDimension = &(pDimensions->z);

	   	tstring::size_type pos = str.find_first_of(",", lastPos);
		if (pos == tstring::npos)
			return false;
        
		// Found a token, add it to the vector
        if (! FromString( pCurrDimension, str.substr(lastPos, pos - lastPos) ) )
			return false;

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(",", pos);
    }

	// If we got here, it worked
	return true;
}

tstring __stdcall ToString( const COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP& threadGroups )
{
	return ToString( threadGroups.x ) + tstring( ", " ) + ToString( threadGroups.y ) + tstring( ", " ) + ToString( threadGroups.z );
}

bool __stdcall FromString( COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP *pThreadGroups, const tstring &str )
{
	// Local variables
   	tstring::size_type lastPos = 0;
	UINT *pCurrThreadGroup = &(pThreadGroups->x);

	// Find the components in the string
	for (UINT i = 0; i < 3; ++i)
    {
		// Find the current item in pDimensions
		if( 1 == i ) 
			pCurrThreadGroup = &(pThreadGroups->y);
		else if( 2 == i ) 
			pCurrThreadGroup = &(pThreadGroups->z);

	   	tstring::size_type pos = str.find_first_of(",", lastPos);
		if (pos == tstring::npos)
			return false;
        
		// Found a token, add it to the vector
        if (! FromString( pCurrThreadGroup, str.substr(lastPos, pos - lastPos) ) )
			return false;

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(",", pos);
    }

	// If we got here, it worked
	return true;
}

//------------------------------------------------------------------------------------------------

CDispatchTest::~CDispatchTest(void)
{
}

/////

TEST_RESULT CDispatchTest::Setup()
{
	return CComputeTest::Setup(); 
}

/////

TEST_RESULT CDispatchTest::SetupTestCase()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;
	const UINT uavInitialCounts = 0;
	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;
	const UINT numThreadsPerGroup = m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y * m_dispatchThreadsPerGroup.z;

	// Determine if this testcase needs to be skipped if it will run to slow (arbitrary)
	if ( SkipSlowOnRefWithMessage( (numGroupsDispatched * numThreadsPerGroup) > 500000 ) )
                  return RESULT_SKIP;

	if( COMPUTE_DISPATCH_METHOD_INDIRECT == m_dispatchMethod )
	{
		D3D11_BUFFER_DESC buffDesc;
		D3D11_SUBRESOURCE_DATA dispatchIndirectData;
		UINT pDispatchIndirectArgs[] = { m_dispatchDimensions.x, m_dispatchDimensions.y, m_dispatchDimensions.z };

		// should have been cleaned up
		if( m_pDispatchIndirectArgs )
		{
			tRes = RESULT_FAIL;
			WriteToLog("CDispatchTest::SetupTestCase() - m_pDispatchIndirectArgs should have been released, but it wasn't.\n");
			goto setupDone;
		}

		// Setup the buffer for the dispatch indirect args
		buffDesc.ByteWidth = sizeof( pDispatchIndirectArgs );
		buffDesc.Usage = D3D11_USAGE_DEFAULT;	// Don't change once created
		buffDesc.BindFlags = 0;
		buffDesc.CPUAccessFlags = 0;
		buffDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		buffDesc.StructureByteStride = 0;	// Not used here

		dispatchIndirectData.pSysMem = pDispatchIndirectArgs;
		dispatchIndirectData.SysMemPitch = 0;
		dispatchIndirectData.SysMemSlicePitch = 0;

		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &buffDesc, &dispatchIndirectData, &m_pDispatchIndirectArgs ) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog("CDispatchTest::SetupTestCase() - Failed to CreateBuffer (m_pDispatchIndirectArgs). HR = %s\n", D3DHResultToString(hr));
			goto setupDone;
		}
	}

	// Create/Set the Target Buffers and UAVs (Note, the descriptions must be setup before calling SetupTestCase
	for( UINT i = 0; i < m_numBoundOutputResources; ++i )
	{
		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &m_destBuffDesc, NULL, &(m_pTargetUABuffers[i]) ) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( "CDispatchTest::SetupTestCase() - Failed to CreateBuffer (m_pTargetUABuffers[%d]). HR = %s", i, D3DHResultToString(hr).c_str() );
			goto setupDone;
		}

		// Create the staging buffer to copy data to and map
		if( FAILED( hr = GetFramework()->CreateBufferTiledOptional( &m_destStageBuffDesc, NULL, &(m_pTargetStagingBuffers[i]) ) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( "CDispatchTest::SetupTestCase() - Failed to CreateBuffer (m_pTargetStagingBuffers[%d]). HR = %s", i, D3DHResultToString(hr).c_str() );
			goto setupDone;
		}

		// Create the Target Structured Buffer UAV
		if( FAILED( hr = m_pDevice->CreateUnorderedAccessView( m_pTargetUABuffers[i], &m_destUAVDesc, &(m_pTargetUAVs[i]) ) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( "CDispatchTest::SetupTestCase() - Failed to CreateUnorderedAccessView (m_pTargetUAVs[%d]). HR = %s", i, D3DHResultToString(hr).c_str() );
			goto setupDone;
		}

		// Clear Target UAV
		m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pTargetUAVs[i], &g_clearVal );

		// Bind Target UAV to pipeline
		m_pEffectiveContext->CSSetUnorderedAccessViews(i, 1, &(m_pTargetUAVs[i]), &uavInitialCounts);
	}

setupDone:
	return tRes;
}

/////

TEST_RESULT CDispatchTest::ExecuteTestCase()
{
	// Local variables
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	D3D11_QUERY_DESC queryPipelineDesc;
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipelineStats;
	ID3D11Query	*pCSInvocationsQuery = NULL;
	D3D11_MAPPED_SUBRESOURCE mappedResources[MAX_NUM_CS_OUTPUT_RESOURCES];
	
	UINT countMS = 0;
	const UINT sleepMS = 10;	// 10 ms

	// Setup the query
	queryPipelineDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryPipelineDesc.MiscFlags = 0;

	if( FAILED( hr = m_pDevice->CreateQuery(&queryPipelineDesc, &pCSInvocationsQuery) ) )
	{
		WriteToLog( _T( "CDispatchTest::ExecuteTestCase() - CreateQuery() for Pipeline unexpectedly failed. HR = %s" ), D3DHResultToString( hr ) );
		tr = RESULT_FAIL;
		goto exit;
	}

	// test cases with large number of threads will be verified through multiple passes to reduce the 
	// memory requirement for the output resource
	UINT numPasses = 1;
	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;
	if ( m_bMultiPasses )
		numPasses = ( numGroupsDispatched % m_groupStep ) ? ( numGroupsDispatched / m_groupStep + 1) 
													: ( numGroupsDispatched / m_groupStep );

	for (UINT i = 0; i < numPasses; i++)
	{
		// Set the Compute Shader
		// for cases that need multiple passes, shader changes between passes
		if ( m_bMultiPasses )
			m_currentPass = i;
		tr = SetupCS();

		if( RESULT_PASS != tr )
		{
			tr = RESULT_FAIL;
			WriteToLog( "CDispatchTest::ExecuteTestCase() - SetupCS() failed." );
			goto exit;
		}

		for( UINT j = 0; j < m_numBoundOutputResources; ++j )
		{
			m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pTargetUAVs[j], &g_clearVal );
		}

		// Begin query
		m_pEffectiveContext->Begin(pCSInvocationsQuery);

		// Dispatch
		if( COMPUTE_DISPATCH_METHOD_DISPATCH == m_dispatchMethod )
			m_pEffectiveContext->Dispatch( m_dispatchDimensions.x, m_dispatchDimensions.y, m_dispatchDimensions.z );
		else if( COMPUTE_DISPATCH_METHOD_INDIRECT == m_dispatchMethod )
			m_pEffectiveContext->DispatchIndirect( m_pDispatchIndirectArgs, 0 );

		// End query
		m_pEffectiveContext->End(pCSInvocationsQuery);

		// Copy the buffer to a staging resource buffer
		for( UINT i = 0; i < m_numBoundOutputResources; ++i )
			m_pEffectiveContext->CopyResource( m_pTargetStagingBuffers[i], m_pTargetUABuffers[i] );

		//
		// Deferred context checking/execute
		//

		// Execute the command list
		if ( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			tstring errStr = "CDispatchTest::ExecuteTestCase() - ExecuteEffectiveContext() failed: " + D3DHResultToString(hr);
			WriteToLog( errStr.c_str() );
			tr = RESULT_FAIL;
			goto exit;
		}

		// Map the staging buffer
		for( UINT i = 0; i < m_numBoundOutputResources; ++i )
		{
			if( FAILED( hr = GetImmediateContext()->Map( m_pTargetStagingBuffers[i], 0, D3D11_MAP_READ, 0, &(mappedResources[i]) ) ) )
			{
				WriteToLog( _T( "CDispatchTest::ExecuteTestCase() - Map() for Staging Target UA Buffer (m_pTargetStagingBuffers[%d]) unexpectedly failed. HR = %s" ),
					i, D3DHResultToString( hr ).c_str() );
				tr = RESULT_FAIL;
				goto exit;
			}
		}

		// Verify the CS outputs
		tr = VerifyCSResult( mappedResources );
		
		// Unmap
		for( UINT i = 0; i < m_numBoundOutputResources; ++i )
			GetImmediateContext()->Unmap( m_pTargetStagingBuffers[i], 0 );

		if( RESULT_PASS != tr )
		{
			WriteToLog( _T( "CDispatchTest::ExecuteTestCase() - VerifyCSResult() failed." ) );
			goto exit;
		}

		//
		// Verify the CS invocations was correct
		//

		// Check pipeline statistics
		
		hr = GetFramework()->GetDataWithTimeout(GetImmediateContext(), pCSInvocationsQuery, ( void **) &pipelineStats, sizeof( pipelineStats ), 0 );

		if ( hr != S_OK )
		{
			WriteToLog( "CDispatchTest::ExecuteTestCase() - GetData() failed or timed out for pCSInvocationsQuery. HR = %s", D3DHResultToString(hr) );
			tr = RESULT_FAIL;
			goto exit;
		}

		tr = VerifyCSInvocations( pipelineStats );

		if( RESULT_PASS != tr )
		{
			WriteToLog( _T( "CDispatchTest::ExecuteTestCase() - VerifyCSInvocations() failed." ) );
			goto exit;
		}
	}

exit:
	// Cleanup
	SAFE_RELEASE( pCSInvocationsQuery );

	// Return the test result
	return tr;
}

/////

void CDispatchTest::CleanupTestCase()
{
	// Local variables
	const UINT uavInitialCounts = 0;
	ID3D11UnorderedAccessView *pNullView = NULL;

	SAFE_RELEASE( m_pDispatchIndirectArgs );
	
	for( UINT i = 0; i < MAX_NUM_CS_OUTPUT_RESOURCES; ++i )
	{
		// Clear the View
		m_pEffectiveContext->CSSetUnorderedAccessViews(i, 1, &pNullView, &uavInitialCounts);

		SAFE_RELEASE( m_pTargetUABuffers[i] );
		SAFE_RELEASE( m_pTargetStagingBuffers[i] );
		SAFE_RELEASE( m_pTargetUAVs[i] );
	}
}

/////

void CDispatchTest::Cleanup()
{
	CComputeTest::Cleanup();
}

//------------------------------------------------------------------------------------------------

void CCallTest::InitTestParameters()
{
	//
	// Dispatch Method (Indirect, etc...)
	//
	CTestCaseParameter<COMPUTE_DISPATCH_METHOD> *pDispatchMethodParam = AddParameter( _T("DispatchMethod"), &m_dispatchMethod);

	CUserValueSet<COMPUTE_DISPATCH_METHOD> *pDispatchMethodValues = new CUserValueSet<COMPUTE_DISPATCH_METHOD>();
	pDispatchMethodValues->AddValue(COMPUTE_DISPATCH_METHOD_DISPATCH);

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )	// Indirect is only valid on FL11
		pDispatchMethodValues->AddValue(COMPUTE_DISPATCH_METHOD_INDIRECT);

	testfactor::RFactor rfDispatchMethod = AddParameterValueSet( pDispatchMethodParam, pDispatchMethodValues );

	//
	// Dispatch Dimensions (x, y, z)
	//
	CTableValueSet<COMPUTE_DISPATCH_DIMENSIONS> *dispatchDimensionsValues = NULL;

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
		dispatchDimensionsValues = new CTableValueSet< COMPUTE_DISPATCH_DIMENSIONS >( &g_dispatchDimensionValuesFL11[0], sizeof( g_dispatchDimensionValuesFL11[0] ), sizeof( g_dispatchDimensionValuesFL11 ) / sizeof( g_dispatchDimensionValuesFL11[0] ) );
	else
		dispatchDimensionsValues = new CTableValueSet< COMPUTE_DISPATCH_DIMENSIONS >( &g_dispatchDimensionValuesFL10[0], sizeof( g_dispatchDimensionValuesFL10[0] ), sizeof( g_dispatchDimensionValuesFL10 ) / sizeof( g_dispatchDimensionValuesFL10[0] ) );

	testfactor::RFactor rfDispatchDimensions = AddParameter( _T("DispatchDimensions"), &m_dispatchDimensions, dispatchDimensionsValues );

	//
	// Threads per Group (x, y, z)
	//
	CTableValueSet<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP> *dispatchThreadsInGroupValues = NULL;

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
		dispatchThreadsInGroupValues = new CTableValueSet< COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP >( &g_dispatchNumThreadsInGroupValuesFL11[0], sizeof( g_dispatchNumThreadsInGroupValuesFL11[0] ), sizeof( g_dispatchNumThreadsInGroupValuesFL11 ) / sizeof( g_dispatchNumThreadsInGroupValuesFL11[0] ) );
	else
		dispatchThreadsInGroupValues = new CTableValueSet< COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP >( &g_dispatchNumThreadsInGroupValuesFL10[0], sizeof( g_dispatchNumThreadsInGroupValuesFL10[0] ), sizeof( g_dispatchNumThreadsInGroupValuesFL10 ) / sizeof( g_dispatchNumThreadsInGroupValuesFL10[0] ) );

	testfactor::RFactor rfDispatchThreadsInGroup = AddParameter( _T("DispatchThreadsInGroups"), &m_dispatchThreadsPerGroup, dispatchThreadsInGroupValues );

	//
	// Number of output resources bound
	//
	CTestCaseParameter<UINT> *pNumBoundOutputResourcesParam = AddParameter( _T("NumBoundOutputResources"), &m_numBoundOutputResources);

	CUserValueSet<UINT> *pNumBoundOutputResourcesValues = new CUserValueSet<UINT>();
	pNumBoundOutputResourcesValues->AddValue(1);

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )	// More than one resource is only valid on FL11
	{
		pNumBoundOutputResourcesValues->AddValue(2);
		pNumBoundOutputResourcesValues->AddValue(MAX_NUM_CS_OUTPUT_RESOURCES);
	}

	testfactor::RFactor rfNumBoundOutputResources = AddParameterValueSet( pNumBoundOutputResourcesParam, pNumBoundOutputResourcesValues );
	testfactor::RFactor rfOneBoundOutputResource = AddParameterValue<UINT>( pNumBoundOutputResourcesParam, 1 );

	//
	// Compute Shader state (NULL, has no output, has output, etc...)
	//
	CTestCaseParameter<CS_STATE> *pCSStateParam = AddParameter( _T("ComputeShader"), &m_CSState);

	CUserValueSet<CS_STATE> *pCSStateValues = new CUserValueSet<CS_STATE>();
	pCSStateValues->AddValue(CS_STATE_NULL);
	pCSStateValues->AddValue(CS_STATE_NO_OUTPUT);
	//pCSStateValues->AddValue(CS_STATE_OUTPUT);	// No output in this value set (to help lower test case numbers)

	testfactor::RFactor rfCSStateNoOutput = AddParameterValueSet( pCSStateParam, pCSStateValues );
	testfactor::RFactor rfCSStateOutput = AddParameterValue( pCSStateParam, CS_STATE_OUTPUT );

	testfactor::RFactor rfTestWithCSOutputs		= rfDispatchMethod * (( rfDispatchDimensions * rfDispatchThreadsInGroup ) % ( rfNumBoundOutputResources * rfCSStateOutput ));
	testfactor::RFactor rfTestWithoutCSOutputs	= rfDispatchMethod * (( rfDispatchDimensions * rfDispatchThreadsInGroup ) % ( rfOneBoundOutputResource * rfCSStateNoOutput ));

	// Set the root factor for the call test
	SetRootTestFactor( rfTestWithCSOutputs + rfTestWithoutCSOutputs );
	
	// adding priority filters
	CFilterHandle filterDimensions;
	CFilterHandle filterThreadsInGroup;

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		filterDimensions = FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL11[0] ) ||
							FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL11[2] ) ||
							FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL11[6] );

		filterThreadsInGroup = FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL11[0] ) ||											
							FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL11[2] ) ||										
							FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL11[6] );											
	}
	else
	{

		filterDimensions = FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL10[1] ) ||
							FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL10[3] ) ||
							FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL10[6] );

		filterThreadsInGroup = FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL10[0] ) ||											
							FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL10[2] ) ||										
							FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL10[6] );	
	}

	AddPriorityPatternFilter( filterDimensions, WeightFactorLarge );
	AddPriorityPatternFilter( filterThreadsInGroup, WeightFactorLarge );
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
	
}

/////

TEST_RESULT CCallTest::SetupCS()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;
	tstring formattedShaderCode;
	const UINT numThreadsDispatched = m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y * m_dispatchThreadsPerGroup.z;

	// Determine which shader to use before formatting it
	switch( m_CSState )
	{
	default:
		tRes = RESULT_FAIL;
		WriteToLog( "CCallTest::SetupCS() - CompileAndCreateComputeShaderFromMemory failed, hr=%s", D3DHResultToString(hr).c_str() );
		goto setupCSDone;
		break;
	case CS_STATE_NULL:
		SAFE_RELEASE( m_pCS );
		m_pCS = NULL;
		break;
	case CS_STATE_OUTPUT:
		formattedShaderCode = FormatString( szDispatchShader_WithOutput,
			m_numBoundOutputResources,
			numThreadsDispatched,
			m_dispatchThreadsPerGroup.x, m_dispatchThreadsPerGroup.y, m_dispatchThreadsPerGroup.z,	// Number of threads used per group
			m_dispatchDimensions.x, m_dispatchDimensions.y, m_dispatchDimensions.x,					// Used to calculate flattened groupID
			m_numBoundOutputResources,
			numThreadsDispatched																	// Cycle through all threads in shared memory (after sync) to add them up
			);
		break;
	case CS_STATE_NO_OUTPUT:
		formattedShaderCode = FormatString( szDispatchShader_NoOutput, 
			numThreadsDispatched,
			m_dispatchThreadsPerGroup.x, m_dispatchThreadsPerGroup.y, m_dispatchThreadsPerGroup.z	// Number of threads used per group
			);
		break;
	};

	// Compile the shader
	if( CS_STATE_NULL != m_CSState )
		hr = CompileAndCreateComputeShaderFromMemory( formattedShaderCode.c_str(), formattedShaderCode.size(), "cs_main", D3D_FEATURE_LEVEL_10_0, &m_pCS );

	if ( FAILED(hr) || ( (NULL == m_pCS) && (CS_STATE_NULL != m_CSState) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( "CCallTest::SetupCS() - CompileAndCreateComputeShaderFromMemory failed, hr=%s", D3DHResultToString(hr).c_str() );
		goto setupCSDone;
	}

	// Set the shader
	m_pEffectiveContext->CSSetShader( m_pCS, NULL, 0);

setupCSDone:
	return tRes;
}

/////

TEST_RESULT CCallTest::SetupTestCase()
{
	// Local variables
	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;

	//
	// Create the Target Buffer and UAV descriptions
	//

	m_destBuffDesc.ByteWidth = sizeof( UINT ) * numGroupsDispatched;
	m_destBuffDesc.Usage = D3D11_USAGE_DEFAULT;	// Don't change once created
	m_destBuffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	m_destBuffDesc.CPUAccessFlags = 0;
	m_destBuffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_destBuffDesc.StructureByteStride = sizeof( UINT );	// Only one UINT per group

	m_destStageBuffDesc.ByteWidth = m_destBuffDesc.ByteWidth;
	m_destStageBuffDesc.Usage = D3D11_USAGE_STAGING;
	m_destStageBuffDesc.BindFlags = 0;
	m_destStageBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	m_destStageBuffDesc.MiscFlags = 0;
	m_destStageBuffDesc.StructureByteStride = m_destBuffDesc.StructureByteStride;

	m_destUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	m_destUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	m_destUAVDesc.Buffer.FirstElement = 0;
	m_destUAVDesc.Buffer.NumElements = numGroupsDispatched;	// One UINT per group
	m_destUAVDesc.Buffer.Flags = 0;

	// Call the common SetupTestCase to set the UAV's and CS
	return CDispatchTest::SetupTestCase();
}

/////

TEST_RESULT CCallTest::VerifyCSResult(const D3D11_MAPPED_SUBRESOURCE mappedResources[MAX_NUM_CS_OUTPUT_RESOURCES])
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	UINT *pResult[MAX_NUM_CS_OUTPUT_RESOURCES] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	const UINT numThreadsDispatched = m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y * m_dispatchThreadsPerGroup.z;
	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;
	const UINT expectedResult = (CS_STATE_OUTPUT == m_CSState) ? numThreadsDispatched : g_clearVal;

	// Map the staging buffer
	for( UINT i = 0; i < m_numBoundOutputResources; ++i )
		pResult[i] = (UINT*)(mappedResources[i].pData);

	// Go through the results (One UINT is ONE group) to make sure the correct number of threads were executed (in that group)
	for(UINT i = 0; i < numGroupsDispatched; ++i)
	{
		const UINT outputResourceIndex = i % m_numBoundOutputResources;
		const UINT numThreadsExecutedInGroup = pResult[outputResourceIndex][i];
		if( numThreadsExecutedInGroup != expectedResult )
		{
			WriteToLog( "CCallTest::VerifyResult() - The number of threads executed for the group does not match the expected value.\n");
			WriteToLog( " Result expected in resource #%d \n", outputResourceIndex);
			WriteToLog( " GroupID = %d \n", i);
			WriteToLog( " Expected number of threads = %d (%#x)\n", expectedResult, expectedResult );
			WriteToLog( " Actual number of threads = %d (%#x)\n", numThreadsExecutedInGroup, numThreadsExecutedInGroup );
			tr = RESULT_FAIL;
			goto exit;
		}
	}

exit:
	return tr;
}

/////

TEST_RESULT CCallTest::VerifyCSInvocations(const D3D11_QUERY_DATA_PIPELINE_STATISTICS &pipelineStats)
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	// Determine the number of expected CS invocations
	const UINT numThreadsDispatched = m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y * m_dispatchThreadsPerGroup.z;
	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;
	const UINT64 numExpectedCSInvocations = (CS_STATE_NULL == m_CSState) ? 0 : numGroupsDispatched * numThreadsDispatched;

	// Check the CS invocations
	if( pipelineStats.CSInvocations != numExpectedCSInvocations )
	{
		WriteToLog( "CCallTest::ExecuteTestCase() - Number of CSInvocations: %llu, expected %llu", pipelineStats.CSInvocations, numExpectedCSInvocations );
		tr = RESULT_FAIL;
		goto exit;
	}

exit:
	return tr;
}

/////

void CCallTest::CleanupTestCase()
{
	// Nothing extra to clean up
	CDispatchTest::CleanupTestCase();
}

//------------------------------------------------------------------------------------------------

void CSVTest::InitTestParameters()
{
	//
	// Dispatch Method (Indirect, etc...)
	//
	CTestCaseParameter<COMPUTE_DISPATCH_METHOD> *pDispatchMethodParam = AddParameter( _T("DispatchMethod"), &m_dispatchMethod);

	CUserValueSet<COMPUTE_DISPATCH_METHOD> *pDispatchMethodValues = new CUserValueSet<COMPUTE_DISPATCH_METHOD>();
	pDispatchMethodValues->AddValue(COMPUTE_DISPATCH_METHOD_DISPATCH);

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )	// Indirect is only valid on FL11
		pDispatchMethodValues->AddValue(COMPUTE_DISPATCH_METHOD_INDIRECT);

	testfactor::RFactor rfDispatchMethod = AddParameterValueSet( pDispatchMethodParam, pDispatchMethodValues );

	//
	// Dispatch Dimensions (x, y, z)
	//
	CTableValueSet<COMPUTE_DISPATCH_DIMENSIONS> *dispatchDimensionsValues = NULL;

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
		dispatchDimensionsValues = new CTableValueSet< COMPUTE_DISPATCH_DIMENSIONS >( &g_dispatchDimensionValuesFL11[0], sizeof( g_dispatchDimensionValuesFL11[0] ), sizeof( g_dispatchDimensionValuesFL11 ) / sizeof( g_dispatchDimensionValuesFL11[0] ) );
	else
		dispatchDimensionsValues = new CTableValueSet< COMPUTE_DISPATCH_DIMENSIONS >( &g_dispatchDimensionValuesFL10[0], sizeof( g_dispatchDimensionValuesFL10[0] ), sizeof( g_dispatchDimensionValuesFL10 ) / sizeof( g_dispatchDimensionValuesFL10[0] ) );

	testfactor::RFactor rfDispatchDimensions = AddParameter( "DispatchDimensions", &m_dispatchDimensions, dispatchDimensionsValues );

	//
	// Dispatch Thread Groups (x, y, z)
	//
	CTableValueSet<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP> *dispatchThreadsInGroupValues = NULL;

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
		dispatchThreadsInGroupValues = new CTableValueSet< COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP >( &g_dispatchNumThreadsInGroupValuesFL11[0], sizeof( g_dispatchNumThreadsInGroupValuesFL11[0] ), sizeof( g_dispatchNumThreadsInGroupValuesFL11 ) / sizeof( g_dispatchNumThreadsInGroupValuesFL11[0] ) );
	else
		dispatchThreadsInGroupValues = new CTableValueSet< COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP >( &g_dispatchNumThreadsInGroupValuesFL10[0], sizeof( g_dispatchNumThreadsInGroupValuesFL10[0] ), sizeof( g_dispatchNumThreadsInGroupValuesFL10 ) / sizeof( g_dispatchNumThreadsInGroupValuesFL10[0] ) );

	testfactor::RFactor rfDispatchThreadsInGroup = AddParameter( "DispatchThreadsInGroups", &m_dispatchThreadsPerGroup, dispatchThreadsInGroupValues );

	//
	// Compute Shader state (NULL, has no output, has output, etc...)
	//
	CTestCaseParameter<CS_STATE> *pCSStateParam = AddParameter( _T("ComputeShader"), &m_CSState);
	testfactor::RFactor rfCSStateOutput = AddParameterValue( pCSStateParam, CS_STATE_OUTPUT );

	// Set the root factor for the call test
	SetRootTestFactor( rfDispatchMethod * rfDispatchDimensions * rfDispatchThreadsInGroup * rfCSStateOutput );

	// adding priority filters
	CFilterHandle filterDimensions;
	CFilterHandle filterThreadsInGroup;

	if( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		filterDimensions = FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL11[0] ) ||
							FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL11[2] ) ||
							FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL11[6] );

		filterThreadsInGroup = FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL11[0] ) ||											
							FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL11[2] ) ||										
							FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL11[6] );											
	}
	else
	{

		filterDimensions = FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL10[1] ) ||
							FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL10[3] ) ||
							FilterEqual<COMPUTE_DISPATCH_DIMENSIONS>( _T("DispatchDimensions"), g_dispatchDimensionValuesFL10[6] );

		filterThreadsInGroup = FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL10[0] ) ||											
							FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL10[2] ) ||										
							FilterEqual<COMPUTE_DISPATCH_NUM_THREADS_IN_GROUP>( _T("DispatchThreadsInGroups"), g_dispatchNumThreadsInGroupValuesFL10[6] );	
	}

	AddPriorityPatternFilter( filterDimensions, WeightFactorLarge );
	AddPriorityPatternFilter( filterThreadsInGroup, WeightFactorLarge );
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

/////

TEST_RESULT CSVTest::SetupTestCase()
{
	// Local variables
	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;
	const UINT numThreadsPerGroup = m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y * m_dispatchThreadsPerGroup.z;

	// test cases with large number of threads will be verified through multiple passes to reduce the 
	// memory requirement for the output resource. Each pass only verifies 1024 groups
	if ( numGroupsDispatched >= 1024 && numThreadsPerGroup >= 32)
	{
		m_bMultiPasses = true;
		m_currentPass = 0;
		m_groupStep = 1024;
	}
	else
	{
		m_bMultiPasses = false;
		m_currentPass = 0;
		m_groupStep = numGroupsDispatched;
	}

	//
	// Create the Target Buffer and UAV descriptions
	//

	m_destBuffDesc.ByteWidth = sizeof( SV_TEST_RESULT ) * m_groupStep * numThreadsPerGroup;
	m_destBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	m_destBuffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	m_destBuffDesc.CPUAccessFlags = 0;
	m_destBuffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	m_destBuffDesc.StructureByteStride = sizeof( SV_TEST_RESULT );

	m_destStageBuffDesc.ByteWidth = m_destBuffDesc.ByteWidth;
	m_destStageBuffDesc.Usage = D3D11_USAGE_STAGING;
	m_destStageBuffDesc.BindFlags = 0;
	m_destStageBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	m_destStageBuffDesc.MiscFlags = 0;
	m_destStageBuffDesc.StructureByteStride = m_destBuffDesc.StructureByteStride;

	m_destUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	m_destUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	m_destUAVDesc.Buffer.FirstElement = 0;
	m_destUAVDesc.Buffer.NumElements = m_groupStep * numThreadsPerGroup;	// One UINT per group
	m_destUAVDesc.Buffer.Flags = 0;

	// Call the common SetupTestCase to set the UAV's and CS
	return CDispatchTest::SetupTestCase();
}

/////

TEST_RESULT CSVTest::SetupCS()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = S_OK;
	tstring formattedShaderCode;

	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;
	UINT minGroupIndex = m_groupStep * m_currentPass;
	UINT maxGroupIndex = min( numGroupsDispatched - 1, minGroupIndex + m_groupStep - 1);

	// Format the shader code
	formattedShaderCode = FormatString( szSVShader,
			m_dispatchThreadsPerGroup.x, m_dispatchThreadsPerGroup.y, m_dispatchThreadsPerGroup.z,	// Number of threads used per group
			minGroupIndex, maxGroupIndex,		// min and max groups that get verified in the current pass
			m_dispatchThreadsPerGroup.x, m_dispatchThreadsPerGroup.y, m_dispatchThreadsPerGroup.x,	// Number of threads used per group, used to calculate flattened groupThreadID
			m_dispatchDimensions.x, m_dispatchDimensions.y, m_dispatchDimensions.x,					// Used to calculate flattened groupID
			m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y * m_dispatchThreadsPerGroup.z	// Used to calculate the unique resultIndex
			);

	// Compile the shader
	if( CS_STATE_NULL != m_CSState )
		hr = CompileAndCreateComputeShaderFromMemory( formattedShaderCode.c_str(), formattedShaderCode.size(), "cs_main", D3D_FEATURE_LEVEL_10_0, &m_pCS );

	if ( FAILED(hr) || ( (NULL == m_pCS) && (CS_STATE_NULL != m_CSState) ) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( "CSVTest::SetupCS() - CompileAndCreateComputeShaderFromMemory failed. HR = %s", D3DHResultToString(hr).c_str() );
		goto setupCSDone;
	}

	// Set the shader
	m_pEffectiveContext->CSSetShader( m_pCS, NULL, 0);

setupCSDone:
	return tRes;
}

/////

TEST_RESULT CSVTest::VerifyCSResult(const D3D11_MAPPED_SUBRESOURCE mappedResources[MAX_NUM_CS_OUTPUT_RESOURCES])
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;
	SV_TEST_RESULT *pResult = NULL;	// Array of SV_TEST_RESULT's
	const UINT numThreadsDispatched = m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y * m_dispatchThreadsPerGroup.z;

	// Map the staging buffer
	pResult = (SV_TEST_RESULT*)(mappedResources[0].pData);

	//
	// Go through the results
	//
	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;
	UINT minGroupIndex = m_groupStep * m_currentPass;
	UINT maxGroupIndex = min( numGroupsDispatched - 1, minGroupIndex + m_groupStep - 1);


	for(UINT flatGroupID = minGroupIndex; flatGroupID <= maxGroupIndex; ++flatGroupID)
	{
		UINT gZ = flatGroupID / ( m_dispatchDimensions.x * m_dispatchDimensions.y );
		UINT gY = ( flatGroupID % ( m_dispatchDimensions.x * m_dispatchDimensions.y ) ) / m_dispatchDimensions.x;
		UINT gX = ( flatGroupID % ( m_dispatchDimensions.x * m_dispatchDimensions.y ) ) % m_dispatchDimensions.x;
	
		// Now start to cycle on threads in the group
		for(UINT tZ = 0; tZ < m_dispatchThreadsPerGroup.z; ++tZ)
		{
			for(UINT tY = 0; tY < m_dispatchThreadsPerGroup.y; ++tY)
			{
				for(UINT tX = 0; tX < m_dispatchThreadsPerGroup.x; ++tX)
				{
					// Get the current result
					const UINT flatGroupThreadID	= ( tZ * m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y ) + ( tY * m_dispatchThreadsPerGroup.x ) + tX;	// x, y, x (Thread dimensions)
					const resultIndex = ( ( flatGroupID - minGroupIndex ) * numThreadsDispatched ) + flatGroupThreadID;
					const SV_TEST_RESULT svResults = pResult[resultIndex];

					// Calculate expected results
					const UINT expectedGroupIndex = flatGroupThreadID;
					const UINT expectedGroupID[3] = { gX, gY, gZ };
					const UINT expectedGroupThreadID[3] = { tX, tY, tZ };
					const UINT expectedDispatchThreadID[3] = 
						{((gX * m_dispatchThreadsPerGroup.x) + tX),
						((gY * m_dispatchThreadsPerGroup.y) + tY),
						((gZ * m_dispatchThreadsPerGroup.z) + tZ)};

					// Check the group index
					// Provides a flattened index for a given thread within a given group
					if( svResults.groupIndex != expectedGroupIndex )
					{
						WriteToLog( "CSVTest::VerifyResult() - The system value, SV_GroupIndex, didn't match the expected result.");
						WriteToLog( " Expected SV_GroupIndex = %d", expectedGroupIndex );
						WriteToLog( " Actual SV_GroupIndex = %d", svResults.groupIndex );
						tr = RESULT_FAIL;
					}

					// Check the group ID
					// Defines the group offset within a ID3D11DeviceContext::Dispatch call, per dimension of the dispatch call.
					if( svResults.groupID[0] != expectedGroupID[0] &&
						svResults.groupID[1] != expectedGroupID[1] &&
						svResults.groupID[2] != expectedGroupID[2] )
					{
						WriteToLog( "CSVTest::VerifyResult() - The system value, SV_GroupID, didn't match the expected result.");
						WriteToLog( " Expected SV_GroupID = (%d, %d, %d)", expectedGroupID[0], expectedGroupID[1], expectedGroupID[2]);
						WriteToLog( " Actual SV_GroupID = (%d, %d, %d)", svResults.groupID[0], svResults.groupID[1], svResults.groupID[2]);
						tr = RESULT_FAIL;
					}

					// Check the group thread ID
					// Defines the global thread offset within the ID3D11DeviceContext::Dispatch call, per dimension of the group
					if( svResults.groupThreadID[0] != expectedGroupThreadID[0] &&
						svResults.groupThreadID[1] != expectedGroupThreadID[1] &&
						svResults.groupThreadID[2] != expectedGroupThreadID[2] )
					{
						WriteToLog( "CSVTest::VerifyResult() - The system value, SV_GroupThreadID, didn't match the expected result.");
						WriteToLog( " Expected SV_GroupThreadID = (%d, %d, %d)", expectedGroupThreadID[0], expectedGroupThreadID[1], expectedGroupThreadID[2]);
						WriteToLog( " Actual SV_GroupThreadID = (%d, %d, %d)", svResults.groupThreadID[0], svResults.groupThreadID[1], svResults.groupThreadID[2]);
						tr = RESULT_FAIL;
					}

					// Check the dispatch thread ID
					// Defines the global thread offset within the ID3D11DeviceContext::Dispatch call, per dimension of the group
					if( svResults.dispatchThreadID[0] != expectedDispatchThreadID[0] &&
						svResults.dispatchThreadID[1] != expectedDispatchThreadID[1] &&
						svResults.dispatchThreadID[2] != expectedDispatchThreadID[2] )
					{
						WriteToLog( "CSVTest::VerifyResult() - The system value, SV_DispatchThreadID, didn't match the expected result.");
						WriteToLog( " Expected SV_DispatchThreadID = (%d, %d, %d)", expectedDispatchThreadID[0], expectedDispatchThreadID[1], expectedDispatchThreadID[2]);
						WriteToLog( " Actual SV_DispatchThreadID = (%d, %d, %d)", svResults.dispatchThreadID[0], svResults.dispatchThreadID[1], svResults.dispatchThreadID[2]);
						tr = RESULT_FAIL;
					}

					// Check to see if we had a failure
					if( RESULT_PASS != tr )
					{
						WriteToLog( " General Info:", gX, gY, gZ);
						WriteToLog( " Group = (%d, %d, %d)", gX, gY, gZ);
						WriteToLog( " Thread = (%d, %d, %d)", tX, tY, tZ);
						goto exit;
					}
				}
			}
		}
	}

exit:
	return tr;
}

/////

TEST_RESULT CSVTest::VerifyCSInvocations(const D3D11_QUERY_DATA_PIPELINE_STATISTICS &pipelineStats)
{
	// Local variables
	TEST_RESULT tr = RESULT_PASS;

	// Determine the number of expected CS invocations
	const UINT numThreadsDispatched = m_dispatchThreadsPerGroup.x * m_dispatchThreadsPerGroup.y * m_dispatchThreadsPerGroup.z;
	const UINT numGroupsDispatched = m_dispatchDimensions.x * m_dispatchDimensions.y * m_dispatchDimensions.z;
	const UINT64 numExpectedCSInvocations = (CS_STATE_NULL == m_CSState) ? 0 : numGroupsDispatched * numThreadsDispatched;

	// Check the CS invocations
	if( pipelineStats.CSInvocations != numExpectedCSInvocations )
	{
		WriteToLog( "CSVTest::ExecuteTestCase() - Number of CSInvocations: %llu, expected %llu", pipelineStats.CSInvocations, numExpectedCSInvocations );
		tr = RESULT_FAIL;
		goto exit;
	}

exit:
	return tr;
}

/////

void CSVTest::CleanupTestCase()
{
	// Nothing extra to clean up
	CDispatchTest::CleanupTestCase();
}