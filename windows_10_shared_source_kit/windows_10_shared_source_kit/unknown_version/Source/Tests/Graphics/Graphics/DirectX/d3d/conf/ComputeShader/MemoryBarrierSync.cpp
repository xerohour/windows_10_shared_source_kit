#include "MemoryBarrierSync.h"
//#include "D3DCompiler.h"
#include "D3DX11async.h"

BEGIN_NAMED_VALUES(MemoryBarrierType)
	NAMED_VALUE("NoBarrier", MemoryBarrier_NoBarrier)
	NAMED_VALUE("GroupMem", MemoryBarrier_GroupMem)
	NAMED_VALUE("DeviceMem", MemoryBarrier_DeviceMem)
	NAMED_VALUE("AllMem", MemoryBarrier_AllMem)
	NAMED_VALUE("AllMemGroup", MemoryBarrier_AllMemGroup)
	NAMED_VALUE("AllMemGroupSync", MemoryBarrier_AllMemGroupSync)
	NAMED_VALUE("DeviceMemGroup", MemoryBarrier_DeviceMemGroup)
	NAMED_VALUE("DeviceMemGroupSync", MemoryBarrier_DeviceMemGroupSync)
END_NAMED_VALUES(MemoryBarrierType)


BEGIN_NAMED_VALUES(UAV_RESOURCE_TYPE)
	NAMED_VALUE("RawBuffer", RAW_BUFFER)
	NAMED_VALUE("TypedBuffer", TYPED_BUFFER)
	NAMED_VALUE("StructuredBuffer", STRUCTURED_BUFFER)
	NAMED_VALUE("Texture1D", TEXTURE1D)
	NAMED_VALUE("Texture1DArray", TEXTURE1DARRAY)
	NAMED_VALUE("Texture2D", TEXTURE2D)
	NAMED_VALUE("Texture2DArray", TEXTURE2DARRAY)
	NAMED_VALUE("Texture3D", TEXTURE3D)
END_NAMED_VALUES(UAV_RESOURCE_TYPE)

//------------------------------------------------------------------------------------------------
// CMemoryTest
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
TEST_RESULT CComputeMemoryTest::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = CComputeTest::SetupTestCase();
	if (tRes != RESULT_PASS)
	{
		WriteToLog( "CComputeTest::SetupTestCase did not succeed" );
		goto setupDone;
	}

	m_NumMemorySourceResources = MAX_MEMORY_SOURCE_RESOURCES;
	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
	{
		m_NumMemorySourceResources = 0;
	}

	// clear target UAV
	UINT clearVals[4] = { 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD };
	m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pTargetUAV, clearVals );
	
	// bind target UAV to pipeline
	UINT initialCounts = 0;
	m_pEffectiveContext->CSSetUnorderedAccessViews(0, 1, &m_pTargetUAV, &initialCounts);


setupDone:
	return tRes;
}

void CComputeMemoryTest::CleanupTestCase()
{
	for (UINT n = 0; n < 1 + m_NumMemorySourceResources; ++n)
	{
		UINT initialCounts = 0;
		ID3D11UnorderedAccessView *pNullView = NULL;
		m_pEffectiveContext->CSSetUnorderedAccessViews(n, 1, &pNullView, &initialCounts);
	}

	for (UINT i = 0; i < MAX_MEMORY_SOURCE_RESOURCES; ++i)
	{
		SAFE_RELEASE(m_pSourceUAVs[i]);
		SAFE_RELEASE(m_pSourceResources[i]);
	}

	SAFE_RELEASE(m_pCS);

	CComputeTest::CleanupTestCase();
}


//------------------------------------------------------------------------------------------------
// CMemorySyncTest
//------------------------------------------------------------------------------------------------
UINT g_NumThreads[] =
{
//  x, y, z
	1, 1, 1,		// 1
	2, 2, 2,		// 8
	3, 7, 11,		// 231
	12, 8, 8,		// 768 : the max limit for compute 10x
	16, 4, 16,		// 1024: the max limit for compute 11
};

void CMemorySyncTest::InitTestParameters()
{
	// we make all these, but only use buffer because others are currently uninteresting
	CTestCaseParameter<UAV_RESOURCE_TYPE>* pDimensionParam = AddParameter<UAV_RESOURCE_TYPE>( _T("Dimension"), &m_UAVSourceType );
	CUserValueSet<UAV_RESOURCE_TYPE> *pUAVTypeValues = new CUserValueSet<UAV_RESOURCE_TYPE>;
	pUAVTypeValues->AddValue(TEXTURE1D);
	pUAVTypeValues->AddValue(TEXTURE1DARRAY);
	pUAVTypeValues->AddValue(TEXTURE2D);
	pUAVTypeValues->AddValue(TEXTURE2DARRAY);
	pUAVTypeValues->AddValue(TEXTURE3D);
	pUAVTypeValues->AddValue(RAW_BUFFER);
	pUAVTypeValues->AddValue(TYPED_BUFFER);
	pUAVTypeValues->AddValue(STRUCTURED_BUFFER);
	
	testfactor::RFactor rDimension_RawBuffer = AddParameterValue<UAV_RESOURCE_TYPE>( pDimensionParam, RAW_BUFFER );
	testfactor::RFactor rDimension_StructBuffer = AddParameterValue<UAV_RESOURCE_TYPE>( pDimensionParam, STRUCTURED_BUFFER );
	testfactor::RFactor rDimension_TypedBuffer = AddParameterValue<UAV_RESOURCE_TYPE>( pDimensionParam, TYPED_BUFFER );
	testfactor::RFactor rDimension_Tex1D = AddParameterValue<UAV_RESOURCE_TYPE>( pDimensionParam, TEXTURE1D );
	testfactor::RFactor rDimension_Tex1DArray = AddParameterValue<UAV_RESOURCE_TYPE>( pDimensionParam, TEXTURE1DARRAY );
	testfactor::RFactor rDimension_Tex2D = AddParameterValue<UAV_RESOURCE_TYPE>( pDimensionParam, TEXTURE2D );
	testfactor::RFactor rDimension_Tex2DArray = AddParameterValue<UAV_RESOURCE_TYPE>( pDimensionParam, TEXTURE2DARRAY );
	testfactor::RFactor rDimension_Tex3D = AddParameterValue<UAV_RESOURCE_TYPE>( pDimensionParam, TEXTURE3D ); 

	testfactor::RFactor rDimension_All = AddParameterValueSet<UAV_RESOURCE_TYPE>( pDimensionParam, pUAVTypeValues );

	CTestCaseParameter<MemoryBarrierType>* pBarrierTypeParam = AddParameter<MemoryBarrierType>( _T("Barrier"), &m_BarrierType );
	testfactor::RFactor rBarrier_GroupMem = AddParameterValue<MemoryBarrierType>( pBarrierTypeParam, MemoryBarrier_GroupMem );
	testfactor::RFactor rBarrier_DeviceMem = AddParameterValue<MemoryBarrierType>( pBarrierTypeParam, MemoryBarrier_DeviceMem );
	testfactor::RFactor rBarrier_AllMem = AddParameterValue<MemoryBarrierType>( pBarrierTypeParam, MemoryBarrier_AllMem );
	testfactor::RFactor rBarrier_AllMemGroupSync = AddParameterValue<MemoryBarrierType>( pBarrierTypeParam, MemoryBarrier_AllMemGroupSync );
	testfactor::RFactor rBarrier_DeviceMemGroupSync = AddParameterValue<MemoryBarrierType>( pBarrierTypeParam, MemoryBarrier_DeviceMemGroupSync );

	CTestCaseParameter<UINT>* pThreadsXParam = AddParameter<UINT>( _T("ThreadsX"), &m_NumThreads[0] );
	CTestCaseParameter<UINT>* pThreadsYParam = AddParameter<UINT>( _T("ThreadsY"), &m_NumThreads[1] );
	CTestCaseParameter<UINT>* pThreadsZParam = AddParameter<UINT>( _T("ThreadsZ"), &m_NumThreads[2] );

	testfactor::RFactor rNumThreadsX_All = AddParameterValueSet<UINT>( pThreadsXParam, 
		new CTableValueSet<UINT>(&(g_NumThreads[0]), sizeof(g_NumThreads[0]) * 3, ARRAY_SIZE(g_NumThreads)/3 ) );
	testfactor::RFactor rNumThreadsY_All = AddParameterValueSet<UINT>( pThreadsYParam,
		new CTableValueSet<UINT>(&(g_NumThreads[1]), sizeof(g_NumThreads[0]) * 3, ARRAY_SIZE(g_NumThreads)/3 ));
	testfactor::RFactor rNumThreadsZ_All = AddParameterValueSet<UINT>( pThreadsZParam, 
		new CTableValueSet<UINT>(&(g_NumThreads[2]), sizeof(g_NumThreads[0]) * 3, ARRAY_SIZE(g_NumThreads)/3 ));

	testfactor::RFactor rNumThreads_All = rNumThreadsX_All % rNumThreadsY_All % rNumThreadsZ_All;

	SetRootTestFactor
		( 
		rDimension_All * (rBarrier_DeviceMem + rBarrier_AllMem + rBarrier_AllMemGroupSync + rBarrier_DeviceMemGroupSync ) * rNumThreads_All + 
		rDimension_RawBuffer * rBarrier_GroupMem * rNumThreads_All
		);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 2), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 3), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 1), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 12), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 16), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

static const char szSyncShader_GroupSharedMem[] =
	"RWByteAddressBuffer targetUAV; \n"
	"groupshared static uint sharedMemory[%d]; \n" // arraysize = x*y*z
	"#define numThreadX %d \n"
	"#define numThreadY %d \n"
	"#define numThreadZ %d \n"
	"[numthreads(numThreadX,numThreadY,numThreadZ)] \n" // x,y,z
	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  for (uint n = 0; n < 1 + threadIndex; ++n) \n"
	"    sharedMemory[threadIndex] = n; \n"
	"  %s(); \n" // sync
	"  uint threadsTotal = numThreadX * numThreadY * numThreadZ; \n"
	"  uint index = (threadIndex + threadsTotal / 2) %% threadsTotal; \n"  // out[n] = shared[n + numThreads/2]
	"  targetUAV.Store(threadIndex*4, sharedMemory[index]); \n"
	"} \n"
	;

static const char szSyncShader_UAVMem[] =
	"struct Record \n"
	"{ \n"
	"   uint Data; \n"
	"}; \n"
	"RWByteAddressBuffer targetUAV; \n"
	"%s sourceUAV; \n" // RWByteAddressBuffer, RWBuffer, RWTexture2D, etc
	"#define numThreadX %d \n"
	"#define numThreadY %d \n"
	"#define numThreadZ %d \n"
	"[numthreads(numThreadX,numThreadY,numThreadZ)] \n" // x,y,z
	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  for (uint n = 0; n < 1 + threadIndex; ++n) \n"
	"     %s \n"//"    sourceUAV[threadIndex] = n; \n"
	"  %s(); \n" // sync
	"  uint threadsTotal = numThreadX * numThreadY * numThreadZ; \n"
	"  uint index = (threadIndex + threadsTotal / 2) %% threadsTotal; \n"  // out[n] = shared[n + numThreads/2]
	"  uint UAVValue = %s; \n"
	"  targetUAV.Store(threadIndex*4, UAVValue); \n"
	"} \n"
	;

static const char szSyncShader_AllMem[] =
	"struct Record \n"
	"{ \n"
	"   uint Data; \n"
	"}; \n"
	"RWByteAddressBuffer targetUAV; \n"
	"%s sourceUAV; \n" // RWByteAddressBuffer, RWBuffer, RWTexture2D, etc
	"groupshared static uint sharedMemory[%d]; \n" // arraysize = x*y*z
	"#define numThreadX %d \n"
	"#define numThreadY %d \n"
	"#define numThreadZ %d \n"
	"[numthreads(numThreadX,numThreadY,numThreadZ)] \n" // x,y,z
	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  for (uint n = 0; n < 1 + threadIndex; ++n) \n"
	"  { \n"
	"     %s \n"//"    sourceUAV[threadIndex] = n; \n"
	"    sharedMemory[threadIndex] = n; \n"
	"  } \n"
	"  %s(); \n" // sync
	"  uint threadsTotal = numThreadX * numThreadY * numThreadZ; \n"
	"  uint index = (threadIndex + threadsTotal / 2) %% threadsTotal; \n"  // out[n] = shared[n + numThreads/2]
	"  uint UAVValue = %s; \n"
	"  targetUAV.Store(threadIndex*4,(UAVValue + sharedMemory[index]) / 2); \n"
	"} \n"
	;

// shader4x only allows 1 UAV bound, so shaders need to be adjusted to that.
static const char szSyncShader_GroupSharedMem4x[] =
	"RWByteAddressBuffer targetUAV; \n"
	"groupshared static uint sharedMemory[%d]; \n" // arraysize = x*y*z
	"#define numThreadX %d \n"
	"#define numThreadY %d \n"
	"#define numThreadZ %d \n"
	"[numthreads(numThreadX,numThreadY,numThreadZ)] \n" // x,y,z
	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  for (uint n = 0; n < 1 + threadIndex; ++n) \n"
	"    sharedMemory[threadIndex] = n; \n"
	"  %s(); \n" // sync
	"  uint threadsTotal = numThreadX * numThreadY * numThreadZ; \n"
	"  uint index = (threadIndex + threadsTotal / 2) %% threadsTotal; \n"  // out[n] = shared[n + numThreads/2]
	"  targetUAV.Store(threadsTotal*4 + threadIndex*4, sharedMemory[index]); \n"
	"} \n"
	;

// shader4x only allows 1 UAV bound, so shaders need to be adjusted to that.
static const char szSyncShader_UAVMem4x[] =
	"struct Record \n"
	"{ \n"
	"   uint Data; \n"
	"}; \n"
	"%s sourceUAV; \n" // RWByteAddressBuffer, RWBuffer
	"#define numThreadX %d \n"
	"#define numThreadY %d \n"
	"#define numThreadZ %d \n"
	"[numthreads(numThreadX,numThreadY,numThreadZ)] \n" // x,y,z
	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  for (uint n = 0; n < 1 + threadIndex; ++n) \n"
	"     %s \n"//"    sourceUAV[threadIndex] = n; \n"
	"  %s(); \n" // sync
	"  uint threadsTotal = numThreadX * numThreadY * numThreadZ; \n"
	"  uint index = (threadIndex + threadsTotal / 2) %% threadsTotal; \n"  // out[n] = shared[n + numThreads/2]
	"  uint UAVValue = %s; \n"
	"  %s \n"  //sourceUAV.Store(threadsTotal*4 + threadIndex*4, UAVValue);
	"} \n"
	;

// shader4x only allows 1 UAV bound, so shaders need to be adjusted to that.
static const char szSyncShader_AllMem4x[] =
	"struct Record \n"
	"{ \n"
	"   uint Data; \n"
	"}; \n"
	"%s sourceUAV; \n" // RWByteAddressBuffer
	"groupshared static uint sharedMemory[%d]; \n" // arraysize = x*y*z
	"#define numThreadX %d \n"
	"#define numThreadY %d \n"
	"#define numThreadZ %d \n"
	"[numthreads(numThreadX,numThreadY,numThreadZ)] \n" // x,y,z
	"void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
	"{ \n"
	"  for (uint n = 0; n < 1 + threadIndex; ++n) \n"
	"  { \n"
	"     %s \n"//"    sourceUAV[threadIndex] = n; \n"
	"    sharedMemory[threadIndex] = n; \n"
	"  } \n"
	"  %s(); \n" // sync
	"  uint threadsTotal = numThreadX * numThreadY * numThreadZ; \n"
	"  uint index = (threadIndex + threadsTotal / 2) %% threadsTotal; \n"  // out[n] = shared[n + numThreads/2]
	"  uint UAVValue = ( %s + sharedMemory[index] ) / 2; \n"
	"  %s \n" // sourceUAV.Store(threadsTotal*4 + threadIndex*4, UAVValue);
	"} \n"
	;


tstring CMemorySyncTest::GetShaderCode()
{
	TEST_RESULT tRes;
	const char* sourceUAVType = NULL;
	const char* accessUAV = NULL;
	const char* UAVValue = NULL;
	const char* sourceStore = NULL;

	if (m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
	{
		switch (m_UAVSourceType)
		{
		default: 
			tRes = RESULT_FAIL;
			WriteToLog( "Unexpected UAV dimension %s", ToString(m_UAVSourceType).c_str() );
			break;

		case RAW_BUFFER: 
			sourceUAVType = "RWByteAddressBuffer"; 
			accessUAV = "sourceUAV.Store(threadIndex*4, n);";
			UAVValue = "sourceUAV.Load(index*4)";
			break;
		case TYPED_BUFFER: 
			sourceUAVType = "RWBuffer<uint>"; 
			accessUAV = "sourceUAV[threadIndex] = n;";
			UAVValue = "sourceUAV[index]";
			break;
		case STRUCTURED_BUFFER: 
			sourceUAVType = "RWStructuredBuffer<Record>"; 
			accessUAV = "sourceUAV[threadIndex].Data = n;";
			UAVValue = "sourceUAV[index].Data";
			break;
		case TEXTURE1D: 
			sourceUAVType = "RWTexture1D<uint>"; 
			accessUAV = "sourceUAV[threadIndex] = n;";
			UAVValue = "sourceUAV[index]";
			break;
		case TEXTURE1DARRAY: 
			sourceUAVType = "RWTexture1DArray<uint>"; 
			accessUAV = "{ uint2 index2 = {threadIndex, 1}; \n sourceUAV[index2] = n; }";
			UAVValue = "sourceUAV[uint2(index, 1)]";
			break;
		case TEXTURE2D: 
			sourceUAVType = "RWTexture2D<uint>";
			accessUAV = "{ uint2 index2 = {(threadID.y * numThreadX + threadID.x), threadID.z}; \n sourceUAV[index2] = n; }";
			UAVValue = "sourceUAV[uint2(index%(numThreadX * numThreadY), index/(numThreadX * numThreadY))]";
			break;
		case TEXTURE2DARRAY: 
			sourceUAVType = "RWTexture2DArray<uint>"; 
			accessUAV = "{ uint3 index3 = {(threadID.y * numThreadX + threadID.x), threadID.z, 1}; \n sourceUAV[index3] = n; }";
			UAVValue = "sourceUAV[uint3(index%(numThreadX * numThreadY), index/(numThreadX * numThreadY), 1)]";
			break;
		case TEXTURE3D: 
			sourceUAVType = "RWTexture3D<uint>"; 
			accessUAV = "{ uint3 index3 = {threadID.x, threadID.y, threadID.z}; \n sourceUAV[index3] = n; }";
			// get the x, y, z for the thread with index "index"
			UAVValue = "sourceUAV[uint3( (index%(numThreadX * numThreadY))% numThreadX, \
					   (index%(numThreadX * numThreadY))/ numThreadX, index/(numThreadX * numThreadY))]";
			break;
		}
	}
	else
	{
		switch (m_UAVSourceType)
		{
		default: 
			tRes = RESULT_FAIL;
			WriteToLog( "Unexpected UAV dimension %s", ToString(m_UAVSourceType).c_str() );
			break;

		case RAW_BUFFER: 
			sourceUAVType = "RWByteAddressBuffer"; 
			accessUAV = "sourceUAV.Store(threadIndex*4, n);";
			UAVValue = "sourceUAV.Load(index*4)";
			sourceStore = "sourceUAV.Store(threadsTotal*4 + threadIndex*4, UAVValue);";
			break;
		case STRUCTURED_BUFFER: 
			sourceUAVType = "RWStructuredBuffer<Record>"; 
			accessUAV = "sourceUAV[threadIndex].Data = n;";
			UAVValue = "sourceUAV[index].Data";
			sourceStore = "sourceUAV[threadsTotal + threadIndex].Data = UAVValue;";
			break;
		}
	}

	const char* syncStr = NULL;
	switch (m_BarrierType)
	{
	default:
		tRes = RESULT_FAIL;
		WriteToLog( "Unexpected barrier type %s", ToString(m_BarrierType).c_str() );
		break;
	case MemoryBarrier_GroupMem: syncStr = "GroupMemoryBarrierWithGroupSync"; break;
	case MemoryBarrier_DeviceMem: syncStr = "DeviceMemoryBarrierWithGroupSync"; break;
	case MemoryBarrier_AllMem: syncStr = "AllMemoryBarrierWithGroupSync"; break;
	case MemoryBarrier_AllMemGroupSync: syncStr = "__test_intrin2"; break;
	case MemoryBarrier_DeviceMemGroupSync: syncStr = "__test_intrin4"; break;
	}

	const char* shaderSyncAll = szSyncShader_AllMem;
	const char* shaderSyncDevice = szSyncShader_UAVMem;
	const char* shaderSyncGroupShared = szSyncShader_GroupSharedMem;
	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		shaderSyncAll = szSyncShader_AllMem4x;
		shaderSyncDevice = szSyncShader_UAVMem4x;
		shaderSyncGroupShared = szSyncShader_GroupSharedMem4x;
	}
	const UINT numThreadsInGroup = m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];

	tstring formattedShaderCode;
	switch (m_BarrierType)
	{
	default:
		tRes = RESULT_FAIL;
		WriteToLog( "Unexpected barrier type %s", ToString(m_BarrierType).c_str() );
		break;
	case MemoryBarrier_GroupMem:
		formattedShaderCode = FormatString( shaderSyncGroupShared, numThreadsInGroup, 
			m_NumThreads[0],m_NumThreads[1],m_NumThreads[2], syncStr );
		break;
	case MemoryBarrier_DeviceMem:
		formattedShaderCode = FormatString( shaderSyncDevice, sourceUAVType, 
			m_NumThreads[0],m_NumThreads[1],m_NumThreads[2], accessUAV, syncStr, UAVValue, sourceStore );
		break;
	case MemoryBarrier_AllMem:
		formattedShaderCode = FormatString( shaderSyncAll, sourceUAVType, numThreadsInGroup, 
			m_NumThreads[0],m_NumThreads[1],m_NumThreads[2], accessUAV, syncStr, UAVValue, sourceStore );
		break;
	case MemoryBarrier_AllMemGroupSync:
		formattedShaderCode = FormatString( shaderSyncAll, sourceUAVType, numThreadsInGroup, 
			m_NumThreads[0],m_NumThreads[1],m_NumThreads[2], accessUAV, syncStr, UAVValue, sourceStore );
		break;
	case MemoryBarrier_DeviceMemGroupSync:
		formattedShaderCode = FormatString( shaderSyncDevice, sourceUAVType, 
			m_NumThreads[0],m_NumThreads[1],m_NumThreads[2], accessUAV, syncStr, UAVValue, sourceStore );
		break;
	}

	return formattedShaderCode;
}


//-------------------------------------------------------------------------------------------------------------
#define CREATERESOURCE(descStruct, pResource, funcName) \
	hr = m_pDevice->funcName(&descStruct, NULL, &(pResource)); \
	if (FAILED( hr )) \
	{ \
	WriteToLog(_T("CMemorySyncTest::CreateSourceUAVs(): "#funcName" failed for "#pResource". hr = %s"), \
				D3DHResultToString(hr).c_str() ); \
		return RESULT_FAIL; \
	}

#define CREATERESOURCETILEDOPT(descStruct, pResource, funcName) \
	hr = GetFramework()->##funcName##TiledOptional(&descStruct, NULL, &(pResource)); \
	if (FAILED( hr )) \
	{ \
	WriteToLog(_T("CMemorySyncTest::CreateSourceUAVs(): "#funcName" failed for "#pResource". hr = %s"), \
				D3DHResultToString(hr).c_str() ); \
		return RESULT_FAIL; \
	}

#define CREATEUAV(pResource, descStruct, pUAView) \
	hr = m_pDevice->CreateUnorderedAccessView(pResource, &descStruct, &(pUAView) ); \
	if (FAILED( hr )) \
	{ \
	WriteToLog(_T("CMemorySyncTest::CreateSourceUAVs(): CreateUnorderedAccessView failed for "#pUAView". hr = %s"), \
				D3DHResultToString(hr).c_str() ); \
		return RESULT_FAIL; \
	}
TEST_RESULT CMemorySyncTest::CreateSourceUAVs()
{
	D3D11_TEXTURE1D_DESC  tex1DDesc;
	tex1DDesc.Width = m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];
    tex1DDesc.MipLevels = 1;
    tex1DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex1DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex1DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex1DDesc.CPUAccessFlags = 0;
    tex1DDesc.MiscFlags = 0;

	D3D11_TEXTURE2D_DESC  tex2DDesc;
	tex2DDesc.Width = m_NumThreads[0] * m_NumThreads[1];
	tex2DDesc.Height = m_NumThreads[2];
    tex2DDesc.MipLevels = 1;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
    tex2DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex2DDesc.CPUAccessFlags = 0;
    tex2DDesc.MiscFlags = 0;

	D3D11_TEXTURE3D_DESC  tex3DDesc;
	tex3DDesc.Width = m_NumThreads[0];
	tex3DDesc.Height = m_NumThreads[1];
	tex3DDesc.Depth = m_NumThreads[2];
    tex3DDesc.MipLevels = 1;
    tex3DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex3DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex3DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex3DDesc.CPUAccessFlags = 0;
    tex3DDesc.MiscFlags = 0;

	HRESULT hr;
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.ByteWidth = sizeof(UINT) * m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2]; 
	
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.MiscFlags = 0; 
	buffDesc.StructureByteStride = 0;
	
	// Create UAV resources for the two read/write UAVs
	for (unsigned int n = 0; n < m_NumMemorySourceResources; ++n)
	{
		if ( m_UAVSourceType == RAW_BUFFER
			|| m_UAVSourceType == TYPED_BUFFER 
			|| m_UAVSourceType == STRUCTURED_BUFFER )
		{
			if ( m_UAVSourceType == RAW_BUFFER )
			{
				buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
				buffDesc.StructureByteStride = 0;
			}
			else if ( m_UAVSourceType == TYPED_BUFFER )
			{
				buffDesc.MiscFlags = 0;
				buffDesc.StructureByteStride = 0;
			}
			else if ( m_UAVSourceType == STRUCTURED_BUFFER )
			{
				buffDesc.MiscFlags = 0;
				buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				buffDesc.StructureByteStride = 4;
			}
			ID3D11Buffer *pBuffer = NULL;
			CREATERESOURCETILEDOPT(buffDesc, pBuffer, CreateBuffer);
			m_pSourceResources[n]= pBuffer;
		}
		else if ( m_UAVSourceType == TEXTURE1D || m_UAVSourceType == TEXTURE1DARRAY)
		{
			if ( m_UAVSourceType == TEXTURE1D )
				tex1DDesc.ArraySize = 1;
			else if ( m_UAVSourceType == TEXTURE1DARRAY)
				tex1DDesc.ArraySize = 2;
			ID3D11Texture1D *pTex1D = NULL;
			CREATERESOURCE(tex1DDesc, pTex1D, CreateTexture1D);
			m_pSourceResources[n]= pTex1D;
		}
		else if ( m_UAVSourceType == TEXTURE2D || m_UAVSourceType == TEXTURE2DARRAY)
		{
			if ( m_UAVSourceType == TEXTURE2D )
				tex2DDesc.ArraySize = 1;
			else if (m_UAVSourceType == TEXTURE2DARRAY)
				tex2DDesc.ArraySize = 2;
			ID3D11Texture2D *pTex2D = NULL;
			CREATERESOURCETILEDOPT(tex2DDesc, pTex2D, CreateTexture2D);
			m_pSourceResources[n]= pTex2D;
		}
		else if (m_UAVSourceType == TEXTURE3D)
		{
			ID3D11Texture3D *pTex3D = NULL;
			CREATERESOURCE(tex3DDesc, pTex3D, CreateTexture3D);
			m_pSourceResources[n]= pTex3D;
		}
	}

	// Create UAVs on the above created resources
	D3D11_BUFFER_UAV BufUAV = {0, m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2], 0};
	D3D11_TEX1D_UAV Tex1DUAV = {0};
	D3D11_TEX1D_ARRAY_UAV Tex1DArrayUAV = {0, 0, 2};
	D3D11_TEX2D_UAV Tex2DUAV = {0};
    D3D11_TEX2D_ARRAY_UAV Tex2DArrayUAV = {0, 0, 2};
    D3D11_TEX3D_UAV Tex3DUAV = {0, 0, m_NumThreads[2]};

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = DXGI_FORMAT_R32_UINT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER ;
	UAVDesc.Buffer = BufUAV;

	// Create UAViews for the two read/write UAVs: m_ppUAVs[0] and m_ppUAVs[1]
	for ( unsigned int n = 0; n < m_NumMemorySourceResources; ++n)
	{
		UAVDesc.Format = DXGI_FORMAT_R32_UINT;
		if ( m_UAVSourceType == RAW_BUFFER
			|| m_UAVSourceType == TYPED_BUFFER 
			|| m_UAVSourceType == STRUCTURED_BUFFER )
		{
			UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			UAVDesc.Buffer = BufUAV;
			if ( m_UAVSourceType == RAW_BUFFER )
			{
				UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			}
			else if (m_UAVSourceType == STRUCTURED_BUFFER)
			{
				UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
			}
		}
		else if ( m_UAVSourceType == TEXTURE1D || m_UAVSourceType == TEXTURE1DARRAY)
		{
			if ( m_UAVSourceType == TEXTURE1D )
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
				UAVDesc.Texture1D = Tex1DUAV;
			}
			else if (m_UAVSourceType == TEXTURE1DARRAY)
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
				UAVDesc.Texture1DArray = Tex1DArrayUAV;
			}
		}
		else if ( m_UAVSourceType == TEXTURE2D || m_UAVSourceType == TEXTURE2DARRAY)
		{
			if ( m_UAVSourceType == TEXTURE2D )
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
				UAVDesc.Texture2D = Tex2DUAV;
			}
			else if (m_UAVSourceType == TEXTURE2DARRAY)
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
				UAVDesc.Texture2DArray = Tex2DArrayUAV;
			}
		}
		else if (m_UAVSourceType == TEXTURE3D)
		{
			UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
			UAVDesc.Texture3D = Tex3DUAV;
		}
		CREATEUAV(m_pSourceResources[n], UAVDesc, m_pSourceUAVs[n]);
	}

	return RESULT_PASS;
	
}


TEST_RESULT CMemorySyncTest::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	// set up some member variables before calling into the base-class SetupTestCase
	// setup target UAV as raw buffer
	m_TargetUAVDimension = D3D11_UAV_DIMENSION_BUFFER;
	m_TargetUAVMiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	m_TargetUAVBufferFlag = D3D11_BUFFER_UAV_FLAG_RAW;
	m_TargetUAVFormat = DXGI_FORMAT_R32_TYPELESS;
	m_TargetUAVSize[0] = m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];

	const UINT numThreadsInGroup = m_TargetUAVSize[0];

	// private intrinsics for _ugroup sync instructions are only valid on debug compiler
	// see bug 669359, 692145
	if( !( m_pDevice->GetCreationFlags() & D3D11_CREATE_DEVICE_DEBUG ) && 
		( m_BarrierType == MemoryBarrier_AllMemGroupSync || 
		m_BarrierType == MemoryBarrier_DeviceMemGroupSync) 
		)
	{
		return RESULT_SKIP;
	}

	if (numThreadsInGroup > 1024)
	{
		tRes = RESULT_SKIP;
		WriteToLog( "Cannot use more than 1024 threads in a group, current number is %d", numThreadsInGroup );
		goto setupDone;
	}

	if ( m_UAVSourceType != STRUCTURED_BUFFER && m_UAVSourceType != RAW_BUFFER 
		&& m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
	{
		WriteToLog(_T("Only structured buffer and raw buffer are supported on Compute 10x."));
		return RESULT_SKIP;
	} 

	if ( numThreadsInGroup > 768  
		&& m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
	{
		WriteToLog(_T("Maximum 768 threads are allowed in a group on Compute 10x."));
		return RESULT_SKIP;
	} 

	
	if ( m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
	{
		// The same buffer used for both source and target UAV, since only 1 UAV is allowed on Compute 10x. 
		// So double the size, the first half of the buffer is used as source and the second half as result
		m_TargetUAVSize[0] = 2 * m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];
		if (m_UAVSourceType == STRUCTURED_BUFFER)
		{
			m_TargetUAVMiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			m_TargetUAVBufferFlag = (D3D11_BUFFER_UAV_FLAG)0;
			m_TargetUAVFormat = DXGI_FORMAT_UNKNOWN;
			m_TargetUAVElementByteStride = sizeof(UINT);
		}
	}
	// call base class SetupTestCase
	tRes = CComputeMemoryTest::SetupTestCase();
	if (tRes != RESULT_PASS)
	{
		WriteToLog( "CComputeMemoryTest::SetupTestCase did not succeed" );
		goto setupDone;
	}

	// only FL 11 and up can have more than 1 UAV
	if ( m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 )
	{
		tRes = CreateSourceUAVs();
		if (tRes != RESULT_PASS)
		{
			WriteToLog( "CMemorySyncTest::SetupTestCase():  CreateSourceUAVs() did not succeed" );
			goto setupDone;
		}

		// clear and bind source UAVs
		UINT initialCounts = 0;
		UINT clearVals[4] = { 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD };
		for (unsigned int n = 0; n < m_NumMemorySourceResources; ++n)
		{
			m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pSourceUAVs[n], clearVals );
			m_pEffectiveContext->CSSetUnorderedAccessViews(n + 1, 1, &m_pSourceUAVs[n], &initialCounts);
		}
	}
	

	// compile our shaders
	{
		tstring shaderCode = GetShaderCode();

		hr = CompileAndCreateComputeShaderFromMemory(shaderCode.c_str(), shaderCode.size(), "cs_main", 
			D3D_FEATURE_LEVEL_10_0, &m_pCS);
		if (FAILED(hr) || NULL == m_pCS)
		{
			tRes = RESULT_FAIL;
			WriteToLog( "CompileAndCreateComputeShaderFromMemory failed, hr=%s", D3DHResultToString(hr).c_str() );
			goto setupDone;
		}
	}

	m_pEffectiveContext->CSSetShader(m_pCS, NULL, 0);

setupDone:
	return tRes;
}

void CMemorySyncTest::CleanupTestCase()
{
	CComputeMemoryTest::CleanupTestCase();
}

TEST_RESULT CMemorySyncTest::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	// execute the compute shader
	m_pEffectiveContext->Dispatch( 1, 1, 1 );

	// execute deferred commands
	g_TestApp.ExecuteEffectiveContext();

	// copy the result to staging
	GetImmediateContext()->CopyResource( m_pTargetResourceCopy, m_pTargetResource );

	const UINT numThreadsInGroup = m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];

	// map to verify results

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = GetImmediateContext()->Map( m_pTargetResourceCopy, 0, D3D11_MAP_READ, 0, &mappedRes );
	if (FAILED(hr))
	{
		tRes = RESULT_FAIL;
		WriteToLog( "Map(target copy) failed, hr=%s", D3DHResultToString(hr).c_str() );
		goto testDone;
	}
	UINT * pResData = (UINT*)mappedRes.pData;
	// for FL10 and FL10.1, only 1 UAV is allowed, the second half of the UAV contains the results
	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0 )
		pResData += m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];

	for (UINT i = 0; i < m_NumThreads[2] && tRes != RESULT_FAIL; i++)
		for (UINT j = 0; j < m_NumThreads[1] && tRes != RESULT_FAIL; j++)
			for (UINT k = 0; k < m_NumThreads[0] && tRes != RESULT_FAIL; k++)
			{
				UINT x = i * m_NumThreads[1] * m_NumThreads[0] + j * m_NumThreads[0] + k;
				const UINT resultValue = pResData[x];
				UINT index = x;
				const UINT expectedValue = (index + numThreadsInGroup / 2) % numThreadsInGroup;
				if (resultValue != expectedValue)
				{
					tRes = RESULT_FAIL;
					WriteToLog( "Result doesn't match expectation at (x,y,z)=(%d,%d,%d). Expected: %#x, Result: %#x ", 
						k, j, i, expectedValue, resultValue );
				}
			}
	
	GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
	
testDone:
	return tRes;
}
//------------------------------------------------------------------------------------------------
// Loading stuff from a resource file.
//------------------------------------------------------------------------------------------------
//bool CMemoryBarrierTest::LoadStringFileFromResource(const TCHAR* pFileName, tstring& s )
//{
//	// Locate the resource
//	const HRSRC ribHandle = FindResource( NULL, pFileName, RT_RCDATA );
//	if( ribHandle == NULL ){
//		WriteToLog( _T("Failed to locate resrouce %s"), pFileName );
//		return false;
//	}
//	
//	// Load the resource
//	const HGLOBAL dataHandle = LoadResource( NULL, ribHandle );
//	if( dataHandle == NULL ){
//		WriteToLog( _T("Failed to load resource: %s"), pFileName );
//		return false;
//	}
//
//	// Lock the resource
//	LPVOID data = LockResource( dataHandle );
//	if( data == NULL ){
//		WriteToLog( _T("Failed to lock resrouce: %s"), pFileName );
//		return false;
//	}
//
//	// Determine the size of the resource
//	const DWORD size = SizeofResource( NULL, ribHandle );
//	if( size == 0 ){
//		WriteToLog( _T("Failed to determine the size of the resource: %s"), pFileName );
//		return false;
//	}
//
//	// Create a temp variable
//	char* buffer = new char[size+1]();
//	memcpy_s( buffer, size, data, size );
//	buffer[size] = '\0';
//
//	// Convert the char* to a string
//	std::string bufferStr(buffer);
//	s = str2tstr(bufferStr);
//
//	// Free the resource
//	if( FreeResource( dataHandle ) != 0 )
//	{
//		WriteToLog( _T("Failed to free resource: %s"), pFileName );
//		delete [] buffer;
//		return false;
//	}
//
//	delete [] buffer;
//	return true;
//}
//------------------------------------------------------------------------------------------------
// CMemoryBarrierTest
//------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierTest::Setup()
{
	TEST_RESULT tr = CComputeMemoryTest::Setup();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CComputeMemoryTest::Setup() failed"));
		return tr;
	}

	//if( LoadStringFileFromResource( _T("MemoryBarrier.fx"), m_ShaderCode ) == false )
	//	return RESULT_FAIL;

	HRESULT hr(S_OK);
	return RESULT_PASS;
}

//static const struct CS_TGSM_TABLE
//{
//	UINT const m_MaxThreads;
//	UINT const m_MaxBytesPerThread;
//}  CS_4_X_TGSM_Table[] = 
//{
//	{ D3D11_CS_4_X_BUCKET00_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET00_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET01_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET01_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET02_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET02_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET03_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET03_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET04_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET04_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET05_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET05_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET06_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET06_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET07_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET07_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET08_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET08_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET09_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET09_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET10_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET10_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET11_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET11_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET12_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET12_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET13_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET13_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET14_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET14_MAX_BYTES_TGSM_WRITABLE_PER_THREAD },
//	{ D3D11_CS_4_X_BUCKET15_MAX_NUM_THREADS_PER_GROUP, D3D11_CS_4_X_BUCKET15_MAX_BYTES_TGSM_WRITABLE_PER_THREAD }
//};

void CMemoryBarrierTest::InitTestParameters()
{
	// Parameters (API)
	//CTestCaseParameter<UINT>* pTargetUAVSizeXParam = AddParameter<UINT>( _T("TargetUAVSizeX"), m_TargetUAVSize );
	//CTestCaseParameter<UINT>* pTargetUAVSizeYParam = AddParameter<UINT>( _T("TargetUAVSizeY"), m_TargetUAVSize + 1 );
	//CTestCaseParameter<UINT>* pTargetUAVSizeZParam = AddParameter<UINT>( _T("TargetUAVSizeZ"), m_TargetUAVSize + 2 );
	CTestCaseParameter<D3D11_UAV_DIMENSION>* pTargetUAVDimensionParam = AddParameter<D3D11_UAV_DIMENSION>( _T("TargetUAVDimension"), &m_TargetUAVDimension );
	//CTestCaseParameter<UAV_BUFFER_TYPE>* pTargetUAVBufferTypeParam = AddParameter<UAV_BUFFER_TYPE>( _T("TargetUAVBufferType"), &m_TargetUAVBufferType );
	CTestCaseParameter<D3D11_RESOURCE_MISC_FLAG>* pTargetUAVBufferTypeParam = AddParameter<D3D11_RESOURCE_MISC_FLAG>( _T("TargetUAVMsicFlags"), &m_TargetUAVMiscFlags );
	pTargetUAVBufferTypeParam->SetDefaultValue((D3D11_RESOURCE_MISC_FLAG)0);
	//CTestCaseParameter<UINT>* pTargetUAVArraySizeParam = AddParameter<UINT>( _T("TargetUAVArraySize"), &m_TargetUAVArraySize );
	CTestCaseParameter<UINT>* pTargetUAVSructStrideParam = AddParameter<UINT>( _T("TargetUAVStructStride"), &m_TargetUAVStructureStrideMin );
	CTestCaseParameter<UINT>* pNumGroupsXParam = AddParameter<UINT>( _T("NumGroupsX"), m_NumGroups );
	CTestCaseParameter<UINT>* pNumGroupsYParam = AddParameter<UINT>( _T("NumGroupsY"), m_NumGroups + 1 );
	CTestCaseParameter<UINT>* pNumGroupsZParam = AddParameter<UINT>( _T("NumGroupsZ"), m_NumGroups + 2 );
	CTestCaseParameter<DXGI_FORMAT>* pTargetUAVFormat = AddParameter<DXGI_FORMAT>( _T("TargetUAVFormat"), &m_TargetUAVFormat );

	CTestCaseParameter<MemoryBarrierType>* pMemoryBarrierType = AddParameter<MemoryBarrierType>( _T("MemoryBarrierType"), &m_MemoryBarrierType );
	CTestCaseParameter<bool>* pDebugParam = AddParameter<bool>( _T("DebugDevice"), &(g_TestApp.m_D3DOptions.Debug) );
	
	// Parameters (Shader)
	CTestCaseParameter<UINT>* pNumThreadsXParam = AddParameter<UINT>( _T("NumThreadsX"), m_NumGroupThreads );
	CTestCaseParameter<UINT>* pNumThreadsYParam = AddParameter<UINT>( _T("NumThreadsY"), m_NumGroupThreads + 1 );
	CTestCaseParameter<UINT>* pNumThreadsZParam = AddParameter<UINT>( _T("NumThreadsZ"), m_NumGroupThreads + 2 );
	CTestCaseParameter<UINT>* pCheckWindowSizeParam = AddParameter<UINT>( _T("CheckWindowSize"), &m_CheckWindowSize );
	CTestCaseParameter<UINT>* pNumSharedMemUpdatesParam = AddParameter<UINT>( _T("NumberSharedMemoryUpdates"), &m_NumSharedMemUpdates );
	CTestCaseParameter<UINT>* pNumSharedMemChecksParam = AddParameter<UINT>( _T("NumberSharedMemoryChecks"), &m_NumSharedMemChecks );
	CTestCaseParameter<bool>* pUseGroupSharedMemoryParam = AddParameter<bool>( _T("UseGroupSharedMemory"), &m_UseGroupSharedMemoryFlag );
	CTestCaseParameter<bool>* pUseCrossGroupReads = AddParameter<bool>( _T("UseCrossGroupReads"), &m_UseCrossGroupReads );

	// Default Parameters
	//SetParameterDefaultValue<UINT>( pTargetUAVSizeYParam, 1u );
	//SetParameterDefaultValue<UINT>( pTargetUAVSizeZParam, 1u );
	//SetParameterDefaultValue<UINT>( pNumGroupsYParam, 1u );
	//SetParameterDefaultValue<UINT>( pNumGroupsZParam, 1u );
	//SetParameterDefaultValue<UINT>( pNumThreadsYParam, 1u );
	//SetParameterDefaultValue<UINT>( pNumThreadsZParam, 1u );
	//SetParameterDefaultValue<D3D11_UAV_DIMENSION>( pTargetUAVDimensionParam, D3D11_UAV_DIMENSION_BUFFER );
	//SetParameterDefaultValue<UAV_BUFFER_TYPE>( pTargetUAVBufferTypeParam, UAV_BUFFER_TYPE_RAW );
	//SetParameterDefaultValue<UINT>( pTargetUAVArraySizeParam, 1u );
	SetParameterDefaultValue<UINT>( pTargetUAVSructStrideParam, 16u ); // bytes
	SetParameterDefaultValue<DXGI_FORMAT>( pTargetUAVFormat, DXGI_FORMAT_R32_UINT );
	SetParameterDefaultValue<bool>( pUseCrossGroupReads, false );
	SetParameterDefaultValue<bool>( pDebugParam, g_TestApp.m_D3DOptions.Debug );
	SetParameterDefaultValue<bool>( pUseGroupSharedMemoryParam, false );

	// Common value sets
	//testfactor::RFactor defaultTargetUAVSizeY = AddParameterValue<UINT>( pTargetUAVSizeYParam, 1u );
	//testfactor::RFactor defaultTargetUAVSizeZ = AddParameterValue<UINT>( pTargetUAVSizeZParam, 1u );
	testfactor::RFactor defaultNumGroupsY = AddParameterValue<UINT>( pNumGroupsYParam, 1u );
	testfactor::RFactor defaultNumGroupsZ = AddParameterValue<UINT>( pNumGroupsZParam, 1u );
	testfactor::RFactor defaultNumThreadsY = AddParameterValue<UINT>( pNumThreadsYParam, 1u );
	testfactor::RFactor defaultNumThreadsZ = AddParameterValue<UINT>( pNumThreadsZParam, 1u );
	//testfactor::RFactor defaultCheckWindowSize = AddParameterValue( pCheckWindowSizeParam, 8u );
	testfactor::RFactor defaultSharedMemUpdates = AddParameterValue<UINT>( pNumSharedMemUpdatesParam, 65u );
	//testfactor::RFactor defaultSharedMemChecks = AddParameterValue( pNumSharedMemChecksParam, 16u );

	testfactor::RFactor checkWindowSize = AddParameterValueSet<UINT>( pCheckWindowSizeParam, new CUserValueSet<UINT>( 8u, 13u, 19u, 32u ) );
	testfactor::RFactor checkTime = AddParameterValueSet<UINT>( pNumSharedMemChecksParam, new CUserValueSet<UINT>( 16u, 33u ) );

	testfactor::RFactor useGroupSharedMemTrue = AddParameterValue<bool>( pUseGroupSharedMemoryParam, true );
	testfactor::RFactor useGroupSharedMemFalse = AddParameterValue<bool>( pUseGroupSharedMemoryParam, false );

	// the base equation for the shader
	testfactor::RFactor defaultBase = /*defaultTargetUAVSizeY * defaultTargetUAVSizeZ */ 
		defaultNumGroupsY * defaultNumGroupsZ * defaultNumThreadsY * defaultNumThreadsZ *
		/*defaultCheckWindowSize */ defaultSharedMemUpdates /* defaultSharedMemChecks*/ *
		( checkWindowSize % checkTime );

	//testfactor::RFactor useGroupSharedMemTrue = AddParameterValue( pUseGroupSharedMemoryParam, true );
	//testfactor::RFactor useGroupSharedMemfalse = AddParameterValue( pUseGroupSharedMemoryParam, false );
	
	testfactor::RFactor targetUAVFormatTYPELESS32 = AddParameterValue( pTargetUAVFormat, DXGI_FORMAT_R32_TYPELESS );
	testfactor::RFactor targetUAVFormatUnknown32 = AddParameterValue( pTargetUAVFormat, DXGI_FORMAT_UNKNOWN );

	testfactor::RFactor targetUAVBuffer = AddParameterValue( pTargetUAVBufferTypeParam, static_cast<D3D11_RESOURCE_MISC_FLAG>(0) );
	testfactor::RFactor targetUAVRaw = AddParameterValue( pTargetUAVBufferTypeParam, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS );
	testfactor::RFactor targetUAVStructured = AddParameterValue( pTargetUAVBufferTypeParam, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED );

	testfactor::RFactor allMemoryBarrierSet = AddParameterValueSet( pMemoryBarrierType, new CUserValueSet<MemoryBarrierType>( MemoryBarrier_AllMem, MemoryBarrier_AllMemGroup ) );
	testfactor::RFactor grpMemoryBarrierSet = AddParameterValueSet( pMemoryBarrierType, new CUserValueSet<MemoryBarrierType>( MemoryBarrier_GroupMem ) );
	testfactor::RFactor UAVMemoryBarrierSet = AddParameterValueSet( pMemoryBarrierType, new CUserValueSet<MemoryBarrierType>( MemoryBarrier_DeviceMem, MemoryBarrier_DeviceMemGroup ) );

	testfactor::RFactor targetUAV_Buffer = AddParameterValue( pTargetUAVDimensionParam, D3D11_UAV_DIMENSION_BUFFER );

	testfactor::RFactor crossGroupReadsTrue = AddParameterValue( pUseCrossGroupReads, true );
	//testfactor::RFactor crossGroupReadsFalse = AddParameterValue( pUseCrossGroupReads, false );

	//testfactor::RFactor defaultStructureStride = AddParameterValue( pTargetUAVSructStrideParam, 16u ); // in bytes

	//testfactor::RFactor targetUAVArraySize = AddParameterValueSet<UINT>( pTargetUAVArraySizeParam, new CUserValueSet<UINT>( /*1u,*/ 3u/*, 5u, 12u*/ ) );
	
	testfactor::RFactor useD3DDebugDevice = AddParameterValue( pDebugParam, true );

	testfactor::RFactor rfNonCrossGroup = grpMemoryBarrierSet * useGroupSharedMemTrue 
					+ UAVMemoryBarrierSet * useGroupSharedMemFalse 
					+ allMemoryBarrierSet * ( useGroupSharedMemTrue + useGroupSharedMemFalse);

	testfactor::RFactor rfCrossGroup = crossGroupReadsTrue * UAVMemoryBarrierSet;

	testfactor::RFactor root;

	if( g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		// ByteAddressedBuffer values
		root = targetUAVRaw * targetUAV_Buffer * targetUAVFormatTYPELESS32;

		// StructuredBuffer values
		{
			// unsigned int max_structure_elements = D3D11_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES / 4;
			testfactor::RFactor structStride = AddParameterValueSet<UINT>( pTargetUAVSructStrideParam, new CUserValueSet<UINT>( 8u, 16u, 64u, D3D11_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES / 8 ) ); // in bytes
			root = root + targetUAVStructured * targetUAV_Buffer * structStride * targetUAVFormatUnknown32;
		}
	
		// Buffer values
		root = root + targetUAVBuffer * targetUAV_Buffer;

		// Texture1DArray values
		{
			testfactor::RFactor targetUAVTexture1DArray = AddParameterValue( pTargetUAVDimensionParam, D3D11_UAV_DIMENSION_TEXTURE1DARRAY );
			root = root + targetUAVTexture1DArray;
		}

		// Texture2D values
		{
			testfactor::RFactor targetUAVTexture2D = AddParameterValue( pTargetUAVDimensionParam, D3D11_UAV_DIMENSION_TEXTURE2D );
			root = root + targetUAVTexture2D;
		}

		// Texture2DArray values
		{
			testfactor::RFactor targetUAVTexture2DArray = AddParameterValue( pTargetUAVDimensionParam, D3D11_UAV_DIMENSION_TEXTURE2DARRAY );
			root = root + targetUAVTexture2DArray;
		}

		// Texture3D values
		{
			testfactor::RFactor targetUAVTexture3D = AddParameterValue( pTargetUAVDimensionParam, D3D11_UAV_DIMENSION_TEXTURE3D );
			root = root + targetUAVTexture3D;
		}

		// Common
		{
			// cross group reads
			testfactor::RFactor numThreadsX_cgr = AddParameterValueSet<UINT>( pNumThreadsXParam, new CUserValueSet<UINT>( 1u, 16u, 23u, 54u ) );
			testfactor::RFactor numGroupsX_cgr = AddParameterValueSet<UINT>( pNumGroupsXParam, new CUserValueSet<UINT>( 64u, 32u, 71u, 212u ) );

			// within group reads
			testfactor::RFactor numThreadsX = AddParameterValueSet<UINT>( pNumThreadsXParam, new CUserValueSet<UINT>( 32u, 129u, 319u, 1024u ) );
			testfactor::RFactor numGroupsX = AddParameterValueSet<UINT>( pNumGroupsXParam, new CUserValueSet<UINT>( 2u, 3u, 5u, 1u ) );

			root = root * 
				( ( numGroupsX % numThreadsX ) * rfNonCrossGroup
				+ ( numThreadsX_cgr % numGroupsX_cgr ) * rfCrossGroup 
				);
		}

		// Texture1D value
		{
			// maximum size of computation must fit in memory.  To simplify this, the UAV could need up to index locations per
			// thread, so we'll just assume a uav with half of the real space available.
			//const unsigned int MaxThreadCount = D3D11_REQ_TEXTURE1D_U_DIMENSION / 2;
			testfactor::RFactor targetUAVTexture1D = AddParameterValue( pTargetUAVDimensionParam, D3D11_UAV_DIMENSION_TEXTURE1D );
			// Texture1d is pretty small memory wise.... limit the number of groups * number of threads

			testfactor::RFactor numThreadsX = AddParameterValueSet( pNumThreadsXParam, new CUserValueSet<UINT>( 319u, 755u ) );
			testfactor::RFactor numThreadsXLarge = AddParameterValueSet( pNumThreadsXParam, new CUserValueSet<UINT>( 1024u ) );
			testfactor::RFactor numGroupsX = AddParameterValueSet( pNumGroupsXParam, new CUserValueSet<UINT>( 2u, 1u ) );
			testfactor::RFactor numGroupsXLarge = AddParameterValueSet( pNumGroupsXParam, new CUserValueSet<UINT>( 7u ) );
			testfactor::RFactor totalThreads = 
				numThreadsX * ( numGroupsX + numGroupsXLarge) + ( numThreadsX + numThreadsXLarge) * numGroupsX;

			// When doing cross group reads
			testfactor::RFactor numThreadsX_cgr = AddParameterValueSet( pNumThreadsXParam, new CUserValueSet<UINT>( 23u, 16u )  );
			testfactor::RFactor numThreadsXLarge_cgr = AddParameterValueSet( pNumThreadsXParam, new CUserValueSet<UINT>( 45u )  );
			testfactor::RFactor numGroupsX_cgr = AddParameterValueSet( pNumGroupsXParam, new CUserValueSet<UINT>( 16u, 1u ) );
			testfactor::RFactor numGroupsXLarge_cgr = AddParameterValueSet( pNumGroupsXParam, new CUserValueSet<UINT>( 170u ) );
			testfactor::RFactor totalThreads_cgr = 
				numThreadsX_cgr * ( numGroupsX_cgr + numGroupsXLarge_cgr) 
				+ ( numThreadsX_cgr + numThreadsXLarge_cgr) * numGroupsX_cgr;

			root = root + targetUAVTexture1D * ( totalThreads * rfNonCrossGroup + totalThreads_cgr * rfCrossGroup );
		}

	}
	else // compute on feature level 10
	{
		// ByteAddressedBuffer values
		root = targetUAVRaw * targetUAVFormatTYPELESS32;
		
		// StructuredBuffer values
		{
			// Sturct size can be very big
			testfactor::RFactor structStride = AddParameterValueSet<UINT>( pTargetUAVSructStrideParam, new CUserValueSet<UINT>( 8u, 16u, 64u, D3D11_REQ_MULTI_ELEMENT_STRUCTURE_SIZE_IN_BYTES / 8 ) ); // in bytes
			root = root + structStride * targetUAVStructured * targetUAVFormatUnknown32;
		}

		// Common
		{
			// cross group reads
			testfactor::RFactor numThreadsX_cgr = AddParameterValueSet<UINT>( pNumThreadsXParam, new CUserValueSet<UINT>( 1u, 16u, 23u, 54u ) );
			testfactor::RFactor numGroupsX_cgr = AddParameterValueSet<UINT>( pNumGroupsXParam, new CUserValueSet<UINT>( 64u, 32u, 71u, 212u ) );

			// within group reads
			testfactor::RFactor numThreadsX = AddParameterValueSet<UINT>( pNumThreadsXParam, new CUserValueSet<UINT>( 32u, 129u, 319u, D3D11_CS_4_X_THREAD_GROUP_MAX_THREADS_PER_GROUP) );
			testfactor::RFactor numGroupsX = AddParameterValueSet<UINT>( pNumGroupsXParam, new CUserValueSet<UINT>( 2u, 3u, 5u, 1u ) );

			root = root * targetUAV_Buffer * 
				( ( numGroupsX % numThreadsX ) * rfNonCrossGroup + ( numThreadsX_cgr % numGroupsX_cgr ) * rfCrossGroup );
		}
	}

	root = root * defaultBase;

	SetRootTestFactor( root );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumThreadsX"), 16), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("NumThreadsX"), 129), WeightFactorVeryLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("CheckWindowSize"), 8), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("CheckWindowSize"), 13), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );
}


TEST_RESULT CMemoryBarrierTest::SetupTestCase()
{
	HRESULT hr(S_OK);
	//const tstring max_thread_group_shared = g_TestApp.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ? _T("1024000") : _T("512000");
	const unsigned int numGroups = m_NumGroups[0] * m_NumGroups[1] * m_NumGroups[2];
	const unsigned int numThreads = m_NumGroupThreads[0] * m_NumGroupThreads[1] * m_NumGroupThreads[2];

	if( SkipSlowOnRefWithMessage( numGroups * numThreads > 512 ) )
		return RESULT_SKIP;


	// private intrinsics for _ugroup sync instructions are only valid on debug compiler
	// see bug 669359, 692145
	if( !( m_pDevice->GetCreationFlags() & D3D11_CREATE_DEVICE_DEBUG ) && 
		( m_MemoryBarrierType == MemoryBarrier_AllMemGroup || 
		m_MemoryBarrierType == MemoryBarrier_DeviceMemGroup ) 
		)
	{
		return RESULT_SKIP;
	}

	{
		TCOMPtr<ID3D10Blob*> pShader;
		{
			m_TargetUAVSize[0] = m_TargetUAVSize[1] = m_TargetUAVSize[2] = 1u;
			m_TargetUAVArraySize = 1u;
			m_TargetUAVElementByteStride = sizeof(UINT);
			m_TargetUAVBufferFlag = static_cast<D3D11_BUFFER_UAV_FLAG>(0);

			unsigned int bytesNeeded = 0;
			unsigned int max_elements_x = 1;
			unsigned int max_elements_y = 1;
			unsigned int max_elements_z = 1;
			unsigned int max_array_size = 1;

			// Each thread should have ( a value, a result ) or ( a value, a result, a flag)
			const unsigned int memory_factor = m_UseGroupSharedMemoryFlag ? 2 : 3;
			// the part at the end of the UAV stores the fault results, except for Structured buffers
			m_ResultOffset = ( memory_factor - 1 ) * numGroups * numThreads;

			//tstring dimensions;
			tstring resource_type;
			tstring resource;
			switch( m_TargetUAVDimension )
			{
				case D3D11_UAV_DIMENSION_BUFFER:
					if( m_TargetUAVMiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
					{
						resource = _T("RWByteAddressBuffer");
						resource_type = _T("2");
						bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
						m_TargetUAVBufferFlag = D3D11_BUFFER_UAV_FLAG_RAW;
					}
					else if( m_TargetUAVMiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
					{
						resource = _T("RWStructuredBuffer<SpecialType>");
						resource_type = _T("3");
						m_TargetUAVElementByteStride = max( memory_factor * 4u, m_TargetUAVStructureStrideMin ); // bytes
						bytesNeeded = numGroups * numThreads * m_TargetUAVElementByteStride; // convert to elements
						// m_ResultOffset only has meanings for other resource types than Structured Buffer
						m_ResultOffset = 0;
					}
					else
					{
						resource = _T("RWBuffer<uint>");
						resource_type = _T("1");
						bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					}
					//dimensions = _T("1");
					max_elements_x = (0x1 << D3D10_REQ_BUFFER_RESOURCE_TEXEL_COUNT_2_TO_EXP) / m_TargetUAVElementByteStride;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE1D:
					resource = _T("RWTexture1D<uint>");
					resource_type = _T("4");
					//dimensions = _T("1");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x =  D3D11_REQ_TEXTURE1D_U_DIMENSION;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
					resource = _T("RWTexture1DArray<uint>");
					resource_type = _T("5");
					//dimensions = _T("2");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x = D3D11_REQ_TEXTURE1D_U_DIMENSION;
					max_array_size = D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE2D:
					resource = _T("RWTexture2D<uint>");
					resource_type = _T("6");
					//dimensions = _T("2");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
					max_elements_y = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
					resource = _T("RWTexture2DArray<uint>");
					resource_type = _T("7");
					//dimensions = _T("3");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
					max_elements_y = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
					max_array_size = D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
					break;
				case D3D11_UAV_DIMENSION_TEXTURE3D:
					resource = _T("RWTexture3D<uint>");
					resource_type = _T("8");
					//dimensions = _T("3");
					bytesNeeded = memory_factor * numGroups * numThreads * m_TargetUAVElementByteStride;
					max_elements_x = D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
					max_elements_y = D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
					max_elements_z = D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
					break;
				default:
					WriteToLog(_T("Unknown D3D11_UAV_DIMENSION: %s"), ToString( m_TargetUAVDimension ) ); 
					return RESULT_FAIL;
			}

			const unsigned int max_texture_bytes = max_elements_x * max_elements_y * max_elements_z * m_TargetUAVElementByteStride;

			switch( m_TargetUAVDimension )
			{
				case D3D11_UAV_DIMENSION_TEXTURE1DARRAY:
				case D3D11_UAV_DIMENSION_TEXTURE2DARRAY:
					unsigned int needed_array_count = bytesNeeded / max_texture_bytes;
					if( bytesNeeded % max_texture_bytes )
					{
						needed_array_count += 1;
					}
					if( needed_array_count > 1 )
					{
						bytesNeeded = max_texture_bytes;
					}
					if( needed_array_count > max_array_size )
					{
						WriteToLog( _T("Texture requirements are too large: (array)") );
						return RESULT_FAIL;
					}
					m_TargetUAVArraySize = max( needed_array_count, m_TargetUAVArraySize );
					break;
			}

			// determine how big it really has to be
			if( bytesNeeded > (max_elements_x * max_elements_y * m_TargetUAVElementByteStride) )
			{
				m_TargetUAVSize[2] = bytesNeeded / ( max_elements_x * max_elements_y * m_TargetUAVElementByteStride);
				if( bytesNeeded % ( max_elements_x * max_elements_y * m_TargetUAVElementByteStride) )
				{
					m_TargetUAVSize[2] += 1;
				}
				m_TargetUAVSize[1] = max_elements_y;
				m_TargetUAVSize[0] = max_elements_x;
				if( m_TargetUAVSize[2] > max_elements_z )
				{
					WriteToLog( _T("Texture requirements are too large:(3D)") );
					return RESULT_FAIL;
				}
			}
			else if( bytesNeeded > (max_elements_x * m_TargetUAVElementByteStride))
			{
				m_TargetUAVSize[1] = bytesNeeded / (max_elements_x * m_TargetUAVElementByteStride);
				if( bytesNeeded % (max_elements_x * m_TargetUAVElementByteStride) )
				{
					m_TargetUAVSize[1] += 1;
				}
				m_TargetUAVSize[0] = max_elements_x;
				if( m_TargetUAVSize[1] > max_elements_y )
				{
					WriteToLog( _T("Texture requirements are too large:(2D)") );
					return RESULT_FAIL;
				}
			}
			else
			{
				m_TargetUAVSize[0] = bytesNeeded / m_TargetUAVElementByteStride;
			}

			// Set the macros
			const tstring group_thread_count_X( FormatString( _T("%d"), m_NumGroupThreads[0] ) );
			const tstring group_thread_count_Y( FormatString( _T("%d"), m_NumGroupThreads[1] ) );
			const tstring group_thread_count_Z( FormatString( _T("%d"), m_NumGroupThreads[2] ) );
			const tstring group_count_X( FormatString( _T("%d"), m_NumGroups[0] ) );
			const tstring group_count_Y( FormatString( _T("%d"), m_NumGroups[1] ) );
			const tstring group_count_Z( FormatString( _T("%d"), m_NumGroups[2] ) );
			//const tstring group_count( FormatString( _T("%d"), numGroups ) );
			const tstring window_size( FormatString( _T("%d"), m_CheckWindowSize ) );
			//const tstring update_count( FormatString( _T("%d"), m_NumSharedMemUpdates ) );
			//const tstring check_time( FormatString( _T("%d"), m_NumSharedMemChecks ) );
			const tstring struct_size( FormatString( _T("%d"), m_TargetUAVElementByteStride / sizeof(UINT) ) );
			const tstring memory_barrier_type( FormatString( _T("%d"), m_MemoryBarrierType ) );
			const tstring resultOffsetStr( FormatString(_T("%d"), m_ResultOffset ));

			D3D10_SHADER_MACRO const macros[] = 
			{
				{ _T("DEFINES_SET"), _T("1") },
				{ _T("GROUP_THREAD_COUNT_X"), group_thread_count_X.c_str() },
				{ _T("GROUP_THREAD_COUNT_Y"), group_thread_count_Y.c_str() },
				{ _T("GROUP_THREAD_COUNT_Z"), group_thread_count_Z.c_str() },
				{ _T("GROUP_COUNT_X"), group_count_X.c_str() },
				{ _T("GROUP_COUNT_Y"), group_count_Y.c_str() },
				{ _T("GROUP_COUNT_Z"), group_count_Z.c_str() },
				//{ _T("GROUP_COUNT"), group_count.c_str() },
				{ _T("WINDOW_SIZE"), window_size.c_str() },
				//{ _T("UPDATE_COUNT"), update_count.c_str() },
				//{ _T("CHECK_TIME"), check_time.c_str() },
				{ _T("RESOURCE"), resource.c_str() },
				{ _T("RESOURCE_TYPE"), resource_type.c_str() },
				//{ _T("DIMENSIONS"), dimensions.c_str() },
				//{ _T("MAX_THREAD_GROUP_SHARED"), max_thread_group_shared.c_str() },
				{ _T("IS_STRUCTURED"), m_TargetUAVMiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED ? _T("1") : _T("0") },
				{ _T("IS_BYTE_ADDRESSED"), m_TargetUAVMiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS ? _T("1") : _T("0") },
				{ _T("USE_GROUP_SHARE_FLAG"), m_UseGroupSharedMemoryFlag ? _T("1") : _T("0") },
				{ _T("STRUCT_SIZE"), struct_size.c_str() },
				{ _T("MEMORY_BARRIER_TYPE"), memory_barrier_type.c_str() },
				{ _T("USE_CROSS_GROUP_READS"), m_UseCrossGroupReads ? _T("1") : _T("0") },
				{ _T("RESULT_OFFSET"), resultOffsetStr.c_str() },
				NULL
			};

			{
				TCOMPtr<ID3D10Blob*> pError;
				char const* fLevel = g_TestApp.GetShaderProfile( D3D_SHADER_STAGE_COMPUTE, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0 );
				
				{
					TCOMPtr<ID3D10Blob*> pText;
					hr = D3DX11PreprocessShaderFromResource(	NULL,
																_T("MemoryBarrier.fx"),
																_T("MemoryBarrier.fx"),
																macros,
																NULL,
																NULL,
																pText,
																pError,
																NULL );
					if( SUCCEEDED(hr ) )
					{
						if( GetFramework()->LogVerbose() )
							WriteMessage( _T("Shader:\n%s"), pText->GetBufferPointer() );
					}
				}

				hr = D3DX11CompileFromResource(	NULL, 
												_T("MemoryBarrier.fx"),
												_T("MemoryBarrier.fx"),
												macros,
												NULL,
												_T("main"),
												fLevel,
												D3D10_SHADER_IEEE_STRICTNESS | D3D10_SHADER_OPTIMIZATION_LEVEL0,
												0,
												NULL,
												pShader,
												pError,
												NULL );
												
				if( FAILED(hr) )
				{
					if( *pError )
					{
						WriteToLog( _T("Compile failed %s\n%s"), D3DHResultToString(hr).c_str(), pError->GetBufferPointer() );
					}
					else
					{
						WriteToLog( _T("Compile failed: %s"), D3DHResultToString( hr ).c_str() );
					}
					
					tstring tmacros;
					for( unsigned int i(0); macros[i].Name != NULL; ++i )
					{
						tmacros = tmacros + FormatString( _T(" /D%s=%s"), macros[i].Name, macros[i].Definition );
					}
					WriteToLog( _T("Macros: %s"), tmacros.c_str() );
					return RESULT_FAIL;
				}
			}

			// create shader
			SAFE_RELEASE(m_pCS);
			hr = GetDevice()->CreateComputeShader(	pShader->GetBufferPointer(),
					pShader->GetBufferSize(),
					NULL,
					&m_pCS );
			if( FAILED(hr) || m_pCS == NULL )
			{
				WriteToLog( _T("CreateComputeShader failed, hr=%s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		}
	}


	// set the shader
	GetEffectiveContext()->CSSetShader( m_pCS, NULL, 0 );

	// Constant buffer setup
	{
		D3D11_BUFFER_DESC const desc = 
		{
			16,
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_CONSTANT_BUFFER,
			D3D11_CPU_ACCESS_WRITE,
			0,
			0
		};

		if( FAILED(hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, &m_pConstBuffer )) )
		{
			WriteToLog( _T("Failed to create constant buffer, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	// map and bind constant buffer
	{
		struct ConstantBufferMapping
		{
			unsigned int m_UpdateCount;
			unsigned int m_CheckTime;
		} globalBuffer = 
		{
			m_NumSharedMemUpdates,
			m_NumSharedMemChecks
		};

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, globalBuffer ) ) )
		{
			WriteToLog( _T("Failed to write data to constant buffer, hr=%s"), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		GetEffectiveContext()->CSSetConstantBuffers( 0, 1, &m_pConstBuffer );
	}

	// must go last since the m_TargetUAVSize is set above
	return CComputeMemoryTest::SetupTestCase();
	// validate
	//
}

TEST_RESULT CMemoryBarrierTest::ExecuteTestCase()
{
	// fix the uav shader indexing to allow for multiple dimensions? -NO
	// execute
	GetEffectiveContext()->Dispatch( m_NumGroups[0], m_NumGroups[1], m_NumGroups[2] );

	ExecuteEffectiveContext();
	// copy uav
	GetImmediateContext()->CopyResource( m_pTargetResourceCopy, m_pTargetResource );
	
	// verify results
	// All fault values in the uav should be zero
	{
		const unsigned int numGroups = m_NumGroups[0] * m_NumGroups[1] * m_NumGroups[2];
		const unsigned int numThreads = m_NumGroupThreads[0] * m_NumGroupThreads[1] * m_NumGroupThreads[2];
		unsigned int elementsToCheck = numGroups * numThreads;

		for( unsigned int array_index(0); array_index < m_TargetUAVArraySize; ++array_index )
		{
			D3D11_MAPPED_SUBRESOURCE mappedRes;
			HRESULT hr = GetImmediateContext()->Map( m_pTargetResourceCopy, array_index, D3D11_MAP_READ, 0, &mappedRes );
			if( FAILED(hr) )
			{
				WriteToLog( _T("Map(target copy) failed, hr=%s"), D3DHResultToString(hr).c_str() );
				return RESULT_FAIL;
			}
		
			UINT rowPitch = mappedRes.RowPitch;
			UINT depthPitch = mappedRes.DepthPitch;
			const unsigned int totalElements = m_TargetUAVSize[0] * m_TargetUAVSize[1] * m_TargetUAVSize[2];
			for( unsigned int uIndex(0); uIndex < totalElements; ++uIndex )
			{
				UINT numFaults = 0;
				if ( m_TargetUAVMiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
					// for structured buffer, the result is stored in the last UINT field of the structure
					numFaults = static_cast<UINT const*>(mappedRes.pData)[ (uIndex + 1) * m_TargetUAVElementByteStride/sizeof(UINT) - 1];
				else
				{
					UINT rowIndex = 0;
					UINT depthIndex = 0;
					UINT dataIndex = m_ResultOffset + uIndex;
					if ( m_TargetUAVDimension == D3D11_UAV_DIMENSION_TEXTURE2D )
					{
						rowIndex = (m_ResultOffset + uIndex) / rowPitch;
						dataIndex = (m_ResultOffset + uIndex) % rowPitch;
					}
					else if ( m_TargetUAVDimension == D3D11_UAV_DIMENSION_TEXTURE3D )
					{
						depthIndex = (m_ResultOffset + uIndex) / depthPitch;
						rowIndex = (m_ResultOffset + uIndex - depthIndex * depthPitch ) / rowPitch;
						dataIndex = (m_ResultOffset + uIndex) % rowPitch;
					}
					numFaults = static_cast<UINT const*>(mappedRes.pData)[depthIndex * depthPitch + rowIndex * rowPitch + dataIndex];
				}
				if( numFaults != 0u )
				{
					WriteToLog( _T("Error: Encountered a fault: %u for thread id: %u"), numFaults, uIndex );
					GetImmediateContext()->Unmap( m_pTargetResourceCopy, array_index );
					return RESULT_FAIL;
				}

				elementsToCheck--;
				if( elementsToCheck == 0 )
				{
					break;
				}
			}

			GetImmediateContext()->Unmap( m_pTargetResourceCopy, array_index );
			if( elementsToCheck == 0 )
			{
				break;
			}
		}
	}
	
	return RESULT_PASS;

}

void CMemoryBarrierTest::CleanupTestCase()
{
	SAFE_RELEASE( m_pConstBuffer );
	CComputeMemoryTest::CleanupTestCase();
}




//////////////////////////////////////////////////////////////////
// class CMemoryBarrierPSTest : public CMemoryBarrierTest
//////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------------------
void CMemoryBarrierPSTest::InitTestParameters()
{
	// viewport width and height, working as the X and Y dimensions of a thead group in Compute
	CUserValueSet<UINT> *pWidthHeight = new CUserValueSet<UINT>;
	pWidthHeight->AddValue(16);
	pWidthHeight->AddValue(31);
	testfactor::RFactor rfViewWidth = AddParameter<UINT>(_T("ViewWidth"), &m_ViewWidth, pWidthHeight);
	testfactor::RFactor rfViewHeight = AddParameter<UINT>(_T("ViewHeight"), &m_ViewHeight, pWidthHeight);

	CUserValueSet<UINT> *pWidthHeightLarge = new CUserValueSet<UINT>;
	pWidthHeightLarge->AddValue(67);
	pWidthHeightLarge->AddValue(128);
	testfactor::RFactor rfViewWidthLarge = AddParameterValueSet<UINT>(_T("ViewWidth"), pWidthHeightLarge);
	testfactor::RFactor rfViewHeightLarge = AddParameterValueSet<UINT>(_T("ViewHeight"), pWidthHeightLarge);

	// resource types
	CUserValueSet<UAV_RESOURCE_TYPE> *pUAVTypeGroup1 = new CUserValueSet<UAV_RESOURCE_TYPE>;
	pUAVTypeGroup1->AddValue(TEXTURE1D);
	pUAVTypeGroup1->AddValue(TEXTURE1DARRAY);
	pUAVTypeGroup1->AddValue(TEXTURE2D);
	pUAVTypeGroup1->AddValue(TEXTURE2DARRAY);
	CUserValueSet<UAV_RESOURCE_TYPE> *pUAVTypeGroup2 = new CUserValueSet<UAV_RESOURCE_TYPE>;
	pUAVTypeGroup2->AddValue(TEXTURE3D);
	pUAVTypeGroup2->AddValue(RAW_BUFFER);
	pUAVTypeGroup2->AddValue(TYPED_BUFFER);
	pUAVTypeGroup2->AddValue(STRUCTURED_BUFFER);
	testfactor::RFactor rfTypeUAV1Group1 = AddParameter<UAV_RESOURCE_TYPE>(_T("FirstUAV"), &(m_TypeUAV[0]), pUAVTypeGroup1);
	testfactor::RFactor rfTypeUAV1Group2 = AddParameterValueSet<UAV_RESOURCE_TYPE>(_T("FirstUAV"), pUAVTypeGroup2);
	testfactor::RFactor rfTypeUAV2Group1 = AddParameter<UAV_RESOURCE_TYPE>(_T("SecondUAV"), &(m_TypeUAV[1]), pUAVTypeGroup1);
	testfactor::RFactor rfTypeUAV2Group2 = AddParameterValueSet<UAV_RESOURCE_TYPE>(_T("SecondUAV"), pUAVTypeGroup2);

	CUserValueSet<UAV_RESOURCE_TYPE> *pUAVTypeLarge = new CUserValueSet<UAV_RESOURCE_TYPE>;
	pUAVTypeLarge->AddValue(TEXTURE1D);
	pUAVTypeLarge->AddValue(TEXTURE1DARRAY);
	pUAVTypeLarge->AddValue(TEXTURE2D);
	pUAVTypeLarge->AddValue(TEXTURE2DARRAY);
	pUAVTypeLarge->AddValue(RAW_BUFFER);
	pUAVTypeLarge->AddValue(TYPED_BUFFER);
	pUAVTypeLarge->AddValue(STRUCTURED_BUFFER);
	testfactor::RFactor rfTypeUAV1Large = AddParameterValueSet<UAV_RESOURCE_TYPE>(_T("FirstUAV"), pUAVTypeLarge);
	testfactor::RFactor rfTypeUAV2Large = AddParameterValueSet<UAV_RESOURCE_TYPE>(_T("SecondUAV"),pUAVTypeLarge);

	// data type: only need to test the difference between scalar type and vector type; 
	// there's no difference between uint and float in terms of store operation and memory flush
	// BUGBUG: Compiler error X3676: typed UAV loads are only allowed for single-component element types; disable uint2 for now
	CUserValueSet<tstring> *pDataType = new CUserValueSet<tstring>;
	pDataType->AddValue(_T("uint"));
	//pDataType->AddValue(_T("uint2"));
	testfactor::RFactor rfDataType1 = AddParameter<tstring>(_T("DataTypeFirstUAV"), &(m_DataTypeUAV[0]), pDataType);
	testfactor::RFactor rfDataType2 = AddParameter<tstring>(_T("DataTypeSecondUAV"), &(m_DataTypeUAV[1]), pDataType);


	testfactor::RFactor root = (rfViewWidth * rfViewHeight) * rfDataType1 * rfDataType2 
		* ( ( (rfTypeUAV1Group1 + rfTypeUAV1Group2) % (rfTypeUAV2Group1 + rfTypeUAV2Group2) )
		    + ( rfTypeUAV1Group1 % rfTypeUAV2Group2)
		  ) 
		  + (rfViewWidthLarge * rfViewHeightLarge) * rfDataType1 * rfDataType2 * (rfTypeUAV1Large % rfTypeUAV2Large) ;

	SetRootTestFactor( root );
}

//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierPSTest::Setup()
{
	TEST_RESULT tr = CComputeMemoryTest::Setup();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CComputeMemoryTest::Setup() failed"));
		return tr;
	}

	if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		WriteToLog(_T("Skip this group on feature levels less than 11_0."));
		return RESULT_SKIP;
	}
	
	tr = SetupVSStage();
	if ( tr != RESULT_PASS)
	{
		WriteToLog(_T("CMemoryBarrierPSTest::SetupVSStage() failed"));
		return tr;
	}

	return RESULT_PASS;
}


//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierPSTest::SetupTestCase()
{
	if ( SkipSlowOnRefWithMessage( m_ViewWidth * m_ViewHeight  > 64 * 64  ) )
		return RESULT_SKIP;

	if ( CreateUAVs() != RESULT_PASS )
	{
		WriteToLog(_T("CMemoryBarrierPSTest::SetupTestCase: SetupUAVs failed "));
		return RESULT_FAIL;
	}

	if ( SetupPixelShader() != RESULT_PASS )
	{
		WriteToLog(_T("CMemoryBarrierPSTest::SetupTestCase: SetupPixelShader failed "));
		return RESULT_FAIL;
	}

	// setup viewport
	D3D11_VIEWPORT vp;
	vp.Height = (float)m_ViewHeight;
	vp.Width = (float)m_ViewWidth;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	GetEffectiveContext()->RSSetViewports( 1, &vp );
	// create a dummy rendertarget
	if ( CComputeTest::SetupRTV(m_ViewWidth, m_ViewHeight) != RESULT_PASS )
	{
		WriteToLog(_T("CMemoryBarrierPSTest::SetupTestCase: CComputeTest::SetupRTV() failed "));
		return RESULT_FAIL;
	}


	// Set vertex buffer
	UINT stride = sizeof( float ) * 3;
	UINT offset = 0;
	GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	GetEffectiveContext()->IASetInputLayout( m_pInputLayout );

	// Set shaders
	GetEffectiveContext()->VSSetShader( m_pVertexShader, NULL, 0 );
	GetEffectiveContext()->PSSetShader( m_pPixelShader, NULL, 0 );

	// Set UAVs: 3 UAVs; the first 2 are read/write UAVs; the last one is the result UAV
	GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 
		1, 
		&m_pRTView, 
		NULL,
		1, 
		3,
		m_ppUAVs, 
		NULL);

	return RESULT_PASS;
}

//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierPSTest::CreateUAVs()
{
	D3D11_TEXTURE1D_DESC  tex1DDesc;
	tex1DDesc.Width = m_ViewWidth * m_ViewHeight;
    tex1DDesc.MipLevels = 1;
    tex1DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex1DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex1DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex1DDesc.CPUAccessFlags = 0;
    tex1DDesc.MiscFlags = 0;

	D3D11_TEXTURE2D_DESC  tex2DDesc;
	tex2DDesc.Width = m_ViewWidth * m_ViewHeight;
	tex2DDesc.Height = 2;
    tex2DDesc.MipLevels = 1;
	tex2DDesc.SampleDesc.Count = 1;
	tex2DDesc.SampleDesc.Quality = 0;
    tex2DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex2DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex2DDesc.CPUAccessFlags = 0;
    tex2DDesc.MiscFlags = 0;

	D3D11_TEXTURE3D_DESC  tex3DDesc;
	tex3DDesc.Width = m_ViewWidth * m_ViewHeight;
	tex3DDesc.Height = 2;
	tex3DDesc.Depth = 2;
    tex3DDesc.MipLevels = 1;
    tex3DDesc.Format = DXGI_FORMAT_R32_UINT;
    tex3DDesc.Usage = D3D11_USAGE_DEFAULT;
    tex3DDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    tex3DDesc.CPUAccessFlags = 0;
    tex3DDesc.MiscFlags = 0;

	HRESULT hr;
	D3D11_BUFFER_DESC buffDesc;
	// create staging output UAV resource
	buffDesc.ByteWidth = sizeof(UINT) * m_ViewWidth * m_ViewHeight; 
	buffDesc.Usage = D3D11_USAGE_STAGING;
	buffDesc.BindFlags = 0;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	buffDesc.MiscFlags = 0; 
	buffDesc.StructureByteStride = 0;
	CREATERESOURCETILEDOPT(buffDesc, m_pResultCopyBuffer, CreateBuffer);
	// create output UAV resource
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	buffDesc.CPUAccessFlags = 0;
	CREATERESOURCETILEDOPT(buffDesc, m_pResultUAVBuffer, CreateBuffer);

	// Create UAV resources for the two read/write UAVs
	for ( UINT i = 0; i < 2; i++ )
	{
		if ( m_TypeUAV[i] == RAW_BUFFER
			|| m_TypeUAV[i] == TYPED_BUFFER 
			|| m_TypeUAV[i] == STRUCTURED_BUFFER )
		{
			if ( m_TypeUAV[i] == RAW_BUFFER )
			{
				buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
				buffDesc.StructureByteStride = 0;
			}
			else if ( m_TypeUAV[i] == TYPED_BUFFER )
			{
				buffDesc.MiscFlags = 0;
				buffDesc.StructureByteStride = 0;
			}
			else if ( m_TypeUAV[i] == STRUCTURED_BUFFER )
			{
				buffDesc.MiscFlags = 0;
				buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				if ( m_DataTypeUAV[i] == _T("uint"))
					buffDesc.StructureByteStride = 4;
				else if ( m_DataTypeUAV[i] == _T("uint2"))
					buffDesc.StructureByteStride = 8;
			}
			CREATERESOURCETILEDOPT(buffDesc, m_ppUAVBuf[i], CreateBuffer);
		}
		else if ( m_TypeUAV[i] == TEXTURE1D || m_TypeUAV[i] == TEXTURE1DARRAY)
		{
			if ( m_TypeUAV[i] == TEXTURE1D )
				tex1DDesc.ArraySize = 1;
			else if (m_TypeUAV[i] == TEXTURE1DARRAY)
				tex1DDesc.ArraySize = 2;
			CREATERESOURCE(tex1DDesc, m_ppUAVTex1D[i], CreateTexture1D);
		}
		else if ( m_TypeUAV[i] == TEXTURE2D || m_TypeUAV[i] == TEXTURE2DARRAY)
		{
			if ( m_TypeUAV[i] == TEXTURE2D )
				tex2DDesc.ArraySize = 1;
			else if (m_TypeUAV[i] == TEXTURE2DARRAY)
				tex2DDesc.ArraySize = 2;
			CREATERESOURCETILEDOPT(tex2DDesc, m_ppUAVTex2D[i], CreateTexture2D);
		}
		else if (m_TypeUAV[i] == TEXTURE3D)
		{
			CREATERESOURCE(tex3DDesc, m_ppUAVTex3D[i], CreateTexture3D);
		}
	}

	// Create UAVs on the above created resources
	D3D11_BUFFER_UAV BufUAV = {0, m_ViewWidth * m_ViewHeight, 0};
	D3D11_TEX1D_UAV Tex1DUAV = {0};
	D3D11_TEX1D_ARRAY_UAV Tex1DArrayUAV = {0, 0, 2};
	D3D11_TEX2D_UAV Tex2DUAV = {0};
    D3D11_TEX2D_ARRAY_UAV Tex2DArrayUAV = {0, 0, 2};
    D3D11_TEX3D_UAV Tex3DUAV = {0, 0, 2};

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = DXGI_FORMAT_R32_UINT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER ;
	UAVDesc.Buffer = BufUAV;

	// Create result UAV
	CREATEUAV(m_pResultUAVBuffer, UAVDesc, m_ppUAVs[2]);

	// Create UAViews for the two read/write UAVs: m_ppUAVs[0] and m_ppUAVs[1]
	for ( UINT i = 0; i < 2; i++ )
	{
		UAVDesc.Format = DXGI_FORMAT_R32_UINT;
		if ( m_TypeUAV[i] == RAW_BUFFER
			|| m_TypeUAV[i] == TYPED_BUFFER 
			|| m_TypeUAV[i] == STRUCTURED_BUFFER )
		{
			UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			UAVDesc.Buffer = BufUAV;
			if ( m_TypeUAV[i] == RAW_BUFFER )
			{
				UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			}
			else if (m_TypeUAV[i] == STRUCTURED_BUFFER)
			{
				UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
			}
			
			CREATEUAV(m_ppUAVBuf[i], UAVDesc, m_ppUAVs[i]);
		}
		else if ( m_TypeUAV[i] == TEXTURE1D || m_TypeUAV[i] == TEXTURE1DARRAY)
		{
			if ( m_TypeUAV[i] == TEXTURE1D )
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
				UAVDesc.Texture1D = Tex1DUAV;
			}
			else if (m_TypeUAV[i] == TEXTURE1DARRAY)
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
				UAVDesc.Texture1DArray = Tex1DArrayUAV;
			}
			CREATEUAV(m_ppUAVTex1D[i], UAVDesc, m_ppUAVs[i]);
		}
		else if ( m_TypeUAV[i] == TEXTURE2D || m_TypeUAV[i] == TEXTURE2DARRAY)
		{
			if ( m_TypeUAV[i] == TEXTURE2D )
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
				UAVDesc.Texture2D = Tex2DUAV;
			}
			else if (m_TypeUAV[i] == TEXTURE2DARRAY)
			{
				UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
				UAVDesc.Texture2DArray = Tex2DArrayUAV;
			}
			CREATEUAV(m_ppUAVTex2D[i], UAVDesc, m_ppUAVs[i]);
		}
		else if (m_TypeUAV[i] == TEXTURE3D)
		{
			UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
			UAVDesc.Texture3D = Tex3DUAV;
			CREATEUAV(m_ppUAVTex3D[i], UAVDesc, m_ppUAVs[i]);
		}
	}

	return RESULT_PASS;
	
}


//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CMemoryBarrierPSTest::SetupPixelShader()
{
	TEST_RESULT tr= RESULT_PASS;
	HRESULT hr;
	
	tstring ViewWidthStr = FormatString( _T("%i"), m_ViewWidth );
	tstring ViewHeightStr = FormatString( _T("%i"), m_ViewHeight );

	tstring UAVDclsStr;
	tstring UAVWriteStr;
	tstring UAVReadStr;

	// Generate UAV Dcls
	// UAV must be declared as "globallycoherent" to do cross-PS-invocation communication  
	for (UINT i = 0; i < 2; i++)
	{
		if ( m_TypeUAV[i] == RAW_BUFFER)
			UAVDclsStr += FormatString(_T("globallycoherent RWByteAddressBuffer u%d; "), i+1);
		else if ( m_TypeUAV[i] == STRUCTURED_BUFFER)
			UAVDclsStr += FormatString(_T("globallycoherent RWStructuredBuffer<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TYPED_BUFFER)
			UAVDclsStr += FormatString(_T("globallycoherent RWBuffer<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE1D)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture1D<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE1DARRAY)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture1DArray<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE2D)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture2D<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE2DARRAY)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture2DArray<%s> u%d; "),(m_DataTypeUAV[i]).c_str(), i+1);
		else if ( m_TypeUAV[i] == TEXTURE3D)
			UAVDclsStr += FormatString(_T("globallycoherent RWTexture3D<%s> u%d; "), (m_DataTypeUAV[i]).c_str(), i+1);
		else
		{
			WriteToLog(_T("CMemoryBarrierPSTest::SetupPixelShader(): Wrong UAV resource type."));
			return RESULT_FAIL;
		}
	}

	// Generate UAV write code
	for (UINT i = 0; i < 2; i++)
	{
		if ( m_TypeUAV[i] == RAW_BUFFER)
			UAVWriteStr += FormatString(_T("u%d.Store(pid*4, pid); "), i+1);
		else if ( m_TypeUAV[i] == TEXTURE1D || m_TypeUAV[i] == STRUCTURED_BUFFER 
			     || m_TypeUAV[i] == TYPED_BUFFER )
		{
			UAVWriteStr += FormatString(_T("uint index%d = {pid}; "), i+1);	
			UAVWriteStr += FormatString(_T("u%d[index%d] = pid; "), i+1, i+1);	
		}
		else if ( m_TypeUAV[i] == TEXTURE1DARRAY || m_TypeUAV[i] == TEXTURE2D)
		{
			UAVWriteStr += FormatString(_T("uint2 index%d = {pid, 1}; "), i+1);	
			UAVWriteStr += FormatString(_T("u%d[index%d] = pid;  "), i+1, i+1);	
		}
		else if ( m_TypeUAV[i] == TEXTURE2DARRAY || m_TypeUAV[i] == TEXTURE3D)
		{
			UAVWriteStr += FormatString(_T("uint3 index%d = {pid, 1, 1}; "), i+1);	
			UAVWriteStr += FormatString(_T("u%d[index%d] = pid; "), i+1, i+1);	
		}
		if ( i == 0 )
			UAVWriteStr += _T("DeviceMemoryBarrier(); ");
	}

	// Generate UAV read code
	for (int i = 1; i >= 0; i--)
	{
		if ( m_TypeUAV[i] == RAW_BUFFER)
			UAVReadStr += FormatString(_T("uint sig_%d = u%d.Load(otherid*4);  "), i+1, i+1);
		else if ( m_TypeUAV[i] == TEXTURE1D || m_TypeUAV[i] == STRUCTURED_BUFFER 
			     || m_TypeUAV[i] == TYPED_BUFFER )
		{ 
			UAVReadStr += FormatString(_T("uint indexOther%d = {otherid};  "), i+1);	
			UAVReadStr += FormatString(_T("uint sig_%d = u%d[indexOther%d];  "), i+1, i+1, i+1);	
		}
		else if ( m_TypeUAV[i] == TEXTURE1DARRAY || m_TypeUAV[i] == TEXTURE2D)
		{
			UAVReadStr += FormatString(_T("uint2 indexOther%d = {otherid, 1};  "), i+1);	
			UAVReadStr += FormatString(_T("uint sig_%d = u%d[indexOther%d];  "), i+1, i+1, i+1);
		}
		else if ( m_TypeUAV[i] == TEXTURE2DARRAY || m_TypeUAV[i] == TEXTURE3D)
		{
			UAVReadStr += FormatString(_T("uint3 indexOther%d = {otherid, 1, 1};  "), i+1);	
			UAVReadStr += FormatString(_T("uint sig_%d = u%d[indexOther%d];  "), i+1, i+1, i+1);
		}
		if ( i == 1 )
			UAVReadStr += _T("DeviceMemoryBarrier(); ");
	}

	// to finish the Macro define
	UAVDclsStr += _T("\n");
	UAVWriteStr += _T("\n");
	UAVReadStr += _T("\n");

	D3D10_SHADER_MACRO macroSet[] =
	{
		{ "MACRO_SET", "true" },
		{ "VIEWPORT_WIDTH", ViewWidthStr.c_str() },
		{ "VIEWPORT_HEIGHT", ViewHeightStr.c_str() },
		{ "UAV_DCLS", UAVDclsStr.c_str() },
		{ "UAV_WRITE", UAVWriteStr.c_str() },
		{ "UAV_READ", UAVReadStr.c_str() },
		NULL
	};

	hr = CompileAndCreatePixelShaderFromResource (
		_T("RC_MEMORY_BARRIER_PS_FX"), 
		_T("PSMain"), 
		D3D_FEATURE_LEVEL_11_0, 
		&m_pPixelShader,
		macroSet
		);

	if (FAILED(hr))
	{
		WriteToLog( _T( "CMemoryBarrierPSTest::SetupPixelShader: CompileAndCreatePixelShaderFromResource() failed."));
		tr = RESULT_FAIL;
		goto shader_done;
			
	}

shader_done:
	return tr;
}

//-------------------------------------------------------------------------------------------------------------
#define NUM_FAILED_PIXELS_IN_LOG 4  // Only log first 4 error pixels
TEST_RESULT CMemoryBarrierPSTest::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;

	UINT clearZero[4] = {0, 0, 0, 0};
	for ( UINT i = 0; i < 2; ++i )
	{
		if ( m_ppUAVs[i] )
			GetEffectiveContext()->ClearUnorderedAccessViewUint(m_ppUAVs[i], clearZero);
	}
	UINT clearOne[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
	// Clear the result UAV to all 1s
	if ( m_ppUAVs[2] )
			GetEffectiveContext()->ClearUnorderedAccessViewUint(m_ppUAVs[2], clearOne);

	GetEffectiveContext()->Draw( 6, 0 );
	ExecuteEffectiveContext();

	GetImmediateContext()->CopyResource( m_pResultCopyBuffer, m_pResultUAVBuffer );
	
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	hr = GetImmediateContext()->Map( m_pResultCopyBuffer, 0, D3D11_MAP_READ, 0, &mappedRes );
	if( FAILED( hr ) )
	{
		WriteToLog( _T( "CMemoryBarrierPSTest::ExecuteTestCase() - Map failed, hr=%s" ), D3DHResultToString( hr ).c_str() );
		return RESULT_FAIL;
	}

	UINT* pUintData = (UINT*)mappedRes.pData;
	UINT numErrMsg = NUM_FAILED_PIXELS_IN_LOG; // Only log first 4 error pixels
	UINT numFailPixels = 0;
	for( UINT i = 0; i < m_ViewWidth * m_ViewHeight; i++)
	{
		if ( pUintData[i] != 0 )
		{
			numFailPixels++;
			if ( numErrMsg > 0 )
			{
				numErrMsg--;
				WriteToLog( _T("CMemoryBarrierPSTest::ExecuteTestCase(): %d faults found in pixel %d."), 
						   pUintData[i], i );
			}
			tr = RESULT_FAIL;
		}
	}
	if ( ( tr == RESULT_FAIL ) && ( numFailPixels > NUM_FAILED_PIXELS_IN_LOG ) )
	{
		WriteToLog( _T("CMemoryBarrierPSTest::ExecuteTestCase(): More failed pixels following.\
					   %d failed pixels in total."), numFailPixels );
	}

		
	GetImmediateContext()->Unmap( m_pResultCopyBuffer, 0 );

	return tr;
}



//-------------------------------------------------------------------------------------------------------------
void CMemoryBarrierPSTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pResultUAVBuffer);
	SAFE_RELEASE(m_pResultCopyBuffer);
	SAFE_RELEASE(m_pRTTex2D);
	SAFE_RELEASE(m_pRTView);

	for(UINT i = 0; i < 3; i++)
		SAFE_RELEASE(m_ppUAVs[i]);

	for(UINT i = 0; i < 2; i++)
	{
		SAFE_RELEASE(m_ppUAVBuf[i]);
		SAFE_RELEASE(m_ppUAVTex1D[i]);
		SAFE_RELEASE(m_ppUAVTex2D[i]);
		SAFE_RELEASE(m_ppUAVTex3D[i]);
	}

}

//-------------------------------------------------------------------------------------------------------------
void CMemoryBarrierPSTest::Cleanup()
{
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pVertexBuffer);
}
