#include "Atomicity.h"
#include "D3DX11async.h"


//------------------------------------------------------------------------------------------------
// CComputeAtomicityTest
//------------------------------------------------------------------------------------------------
TEST_RESULT CComputeAtomicityTest::Setup()
{
    if (g_TestApp.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0)
    {
        return RESULT_SKIP;
    }
    return CComputeTest::Setup();
}

TEST_RESULT CComputeAtomicityTest::SetupTestCase()
{
    {
        UINT gx, gy, gz;
        GetThreadGroupCount( &gx, &gy, &gz );
        const UINT totalThreadCount = m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2] * gx * gy * gz;
        if (SkipSlowOnRefWithMessage( totalThreadCount >= m_skipTotalThreadCount ))
            return RESULT_SKIP;
    }
    
    HRESULT hr;
    TEST_RESULT tRes = CComputeTest::SetupTestCase();
    if (tRes != RESULT_PASS)
    {
        WriteToLog( "CComputeTest::SetupTestCase did not succeed" );
        goto setupDone;
    }

    if (SHARED_MEMORY_UAV == m_SharedMemType)
    {
        D3D11_BUFFER_DESC desc;
        ZeroStructMem(desc);

        desc.ByteWidth = sizeof(UINT) * 2048;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        SAFE_RELEASE(m_pSharedUAVBuffer);
        hr = GetFramework()->CreateBufferTiledOptionalMapDefaultOptional( &desc, NULL, &m_pSharedUAVBuffer );
        if( FAILED(hr) || m_pSharedUAVBuffer == NULL )
        {
            tRes = RESULT_FAIL;
            WriteToLog( _T("CreateBuffer(shared buffer) failed, hr=%s"), D3DHResultToString(hr).c_str() );
            goto setupDone;
        }

        {
            CD3D11_UNORDERED_ACCESS_VIEW_DESC const uavDesc(
                m_pSharedUAVBuffer,
                m_TargetUAVFormat,
                0,
                2048,
                0
            );

            SAFE_RELEASE(m_pSharedUAV);
            hr = m_pDevice->CreateUnorderedAccessView( m_pSharedUAVBuffer, &uavDesc, &m_pSharedUAV );
            if (FAILED(hr) || NULL == m_pSharedUAV)
            {
                tRes = RESULT_FAIL;
                WriteToLog( "CreateUnorderedAccessView(shared buffer) failed. hr=%s", D3DHResultToString(hr).c_str() );
                goto setupDone;
            }
        }

        SAFE_RELEASE(m_pSharedUAVBufferCopy);
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;
        hr = GetFramework()->CreateBufferTiledOptional( &desc, NULL, &m_pSharedUAVBufferCopy );
        if( FAILED(hr) || m_pSharedUAVBufferCopy == NULL )
        {
            WriteToLog( _T("CreateBuffer(shared buffer copy) failed, hr=%s"), D3DHResultToString(hr).c_str() );
            return RESULT_FAIL;
        }
    }

    // clear & bind target UAV
    if (NULL == m_pTargetUAV)
    {
        tRes = RESULT_FAIL;
    }
    else
    {
        UINT targetClearVal = 0;
        UINT targetClearVals[4] = { targetClearVal, targetClearVal, targetClearVal, targetClearVal };
        m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pTargetUAV, targetClearVals );

        UINT initialCounts = 0;
        m_pEffectiveContext->CSSetUnorderedAccessViews(0, 1, &m_pTargetUAV, &initialCounts);
    }
    // clear & bind shared UAV
    if (NULL != m_pSharedUAV)
    {
        UINT m_sharedUAVClearVals[4] = { m_sharedUAVClearVal, m_sharedUAVClearVal, m_sharedUAVClearVal, m_sharedUAVClearVal };
        m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pSharedUAV, m_sharedUAVClearVals );
        
        UINT initialCounts = 0;
        m_pEffectiveContext->CSSetUnorderedAccessViews(1, 1, &m_pSharedUAV, &initialCounts);
    }

    // compile our shaders
    {
        D3D10_SHADER_MACRO defines[MAX_DEFINES];
        ZeroStructMem(defines);
        tstring shaderCode = GetShaderCode(defines);

        hr = CompileAndCreateComputeShaderFromMemory(shaderCode.c_str(), shaderCode.size(), "cs_main", 
            D3D_FEATURE_LEVEL_11_0, &m_pCS, defines);
        if (FAILED(hr) || NULL == m_pCS)
        {
            tRes = RESULT_FAIL;
            WriteToLog( "CompileAndCreateComputeShaderFromMemory failed, hr=%s", D3DHResultToString(hr).c_str() );
            goto setupDone;
        }

        m_pEffectiveContext->CSSetShader(m_pCS, NULL, 0);
    }

setupDone:
    return tRes;
}

void CComputeAtomicityTest::CleanupTestCase()
{
    for (int n = 0; n < 2; ++n)
    {
        UINT initialCounts = 0;
        ID3D11UnorderedAccessView *pNullView = NULL;
        m_pEffectiveContext->CSSetUnorderedAccessViews(n, 1, &pNullView, &initialCounts);
    }

    SAFE_RELEASE(m_pSharedUAV);
    SAFE_RELEASE(m_pSharedUAVBuffer);
    SAFE_RELEASE(m_pSharedUAVBufferCopy);
    SAFE_RELEASE(m_pCS);

    CComputeTest::CleanupTestCase();
}

TEST_RESULT CComputeAtomicityTest::ExecuteTestCase()
{
    HRESULT hr;
    TEST_RESULT tRes = RESULT_PASS;

    // execute the compute shader
    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );
    m_pEffectiveContext->Dispatch( gx, gy, gz );

    // execute deferred commands
    g_TestApp.ExecuteEffectiveContext();

    // copy the result to staging
    GetImmediateContext()->CopyResource( m_pTargetResourceCopy, m_pTargetResource );

    D3D11_MAPPED_SUBRESOURCE mappedBuffer;
    hr = GetImmediateContext()->Map( m_pTargetResourceCopy, 0, D3D11_MAP_READ, 0, &mappedBuffer );
    if (FAILED(hr))
    {
        tRes = RESULT_FAIL;
        WriteToLog( "Map(m_pTargetResourceCopy) failed, hr=%s", D3DHResultToString(hr).c_str() );
    }
    else
    {
        tRes = VerifyResult( &mappedBuffer );
    
        GetImmediateContext()->Unmap( m_pTargetResourceCopy, 0 );
    }

    return tRes;
}


//------------------------------------------------------------------------------------------------
// CAtomicSingleIntrinsicTest
//------------------------------------------------------------------------------------------------
void CAtomicSingleIntrinsicTest::InitTestParameters()
{
    const tstring intrinsicName = GetIntrinsicName();

    m_TargetUAVSize[0] = 2048;
    m_TargetUAVSize[1] = 1;
    m_TargetUAVSize[2] = 1;

    CTestCaseParameter<bool>* pSubsetOpsParam = AddParameter<bool>( _T("SubsetOperations"), &m_bSubsetOperations );
    testfactor::RFactor rSubsetOps_F = AddParameterValue<bool>( pSubsetOpsParam, false );
    testfactor::RFactor rSubsetOps_T = AddParameterValue<bool>( pSubsetOpsParam, true );
    testfactor::RFactor rSubsetOps_FT = rSubsetOps_F + rSubsetOps_T;

    CTestCaseParameter<bool>* pOriginalValParam = AddParameter<bool>( _T("OriginalValueVariant"), &m_bOriginalValueVariant );
    testfactor::RFactor rOriginalVal_F = AddParameterValue<bool>( pOriginalValParam, false );
    testfactor::RFactor rOriginalVal_T = AddParameterValue<bool>( pOriginalValParam, true );
    testfactor::RFactor rOriginalVal_FT = rOriginalVal_F + rOriginalVal_T;

    CTestCaseParameter<bool>* pSignedIntParam = AddParameter<bool>( _T("SignedInt"), &m_bSignedInt );
    testfactor::RFactor rSignedInt_F = AddParameterValue<bool>( pSignedIntParam, false );
    testfactor::RFactor rSignedInt_T = AddParameterValue<bool>( pSignedIntParam, true );
    testfactor::RFactor rSignedInt = rSignedInt_F;
    if (intrinsicName == "Add" || intrinsicName == "Min" || intrinsicName == "Max")
        rSignedInt = rSignedInt + rSignedInt_T;

/*	CTestCaseParameter<MemoryBarrierType>* pBarrierTypeParam = AddParameter<MemoryBarrierType>( _T("Barrier"), &m_BarrierType );
    testfactor::RFactor rBarrier_GroupMem = AddParameterValue<MemoryBarrierType>( pBarrierTypeParam, MemoryBarrier_GroupMem );
    testfactor::RFactor rBarrier_DeviceMem = AddParameterValue<MemoryBarrierType>( pBarrierTypeParam, MemoryBarrier_DeviceMem );
    testfactor::RFactor rBarrier_AllMem = AddParameterValue<MemoryBarrierType>( pBarrierTypeParam, MemoryBarrier_AllMem );
*/
    CTestCaseParameter<UINT>* pThreadsXParam = AddParameter<UINT>( _T("ThreadsX"), &m_NumThreads[0] );
    CTestCaseParameter<UINT>* pThreadsYParam = AddParameter<UINT>( _T("ThreadsY"), &m_NumThreads[1] );
    CTestCaseParameter<UINT>* pThreadsZParam = AddParameter<UINT>( _T("ThreadsZ"), &m_NumThreads[2] );


    testfactor::RFactor rNumThreadsX_All = AddParameterValueSet<UINT>( pThreadsXParam, new CUserValueSet<UINT>(1, 2, 3, 10) );
    testfactor::RFactor rNumThreadsY_All = AddParameterValueSet<UINT>( pThreadsYParam, new CUserValueSet<UINT>(1, 3, 7, 10) );
    testfactor::RFactor rNumThreadsZ_All = AddParameterValueSet<UINT>( pThreadsZParam, new CUserValueSet<UINT>(1, 6, 11, 10) );

    testfactor::RFactor rNumThreads_All = rNumThreadsX_All % rNumThreadsY_All % rNumThreadsZ_All;

    SetRootTestFactor
        ( rNumThreads_All * rSubsetOps_FT * rOriginalVal_FT * rSignedInt
        );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 2), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 10), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );

}

tstring CAtomicSingleIntrinsicTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    defines[0].Name = "INTorUINT";
    defines[0].Definition = m_bSignedInt ? "int" : "uint";

    static const char szShader_GroupSharedMem[] =
        "RWBuffer<uint> targetUAV; \n"
        "%s; \n" // insert shared memory decl here
        "[numthreads(%d,%d,%d)] \n" // insert x,y,z ; 1000 threads = 10 * 10 * 10
        "void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
        "{ \n"
        "  uint addressMask = 0xffffffff; \n"
        "  INTorUINT valToUse = %s; \n" // insert value string here
        "  uint clearVal = %#x; \n" // insert clearVal here
        // initialize all shared memory to clearVal
        "  if (threadIndex == 0) \n"
        "  { \n"
		"    [fastopt]"
        "    for (uint total = 0; total < 2048; ++total) \n"
        "      sharedMemory[total] = (INTorUINT)clearVal; \n"
        "  } \n"
        // sync for initialization
        "  GroupMemoryBarrierWithGroupSync(); \n" 
        "  uint index = 0; \n"
		"  [fastopt]"
        "  for (uint outer = 0; outer < 128; ++outer) \n"
        "  { \n"
        "    INTorUINT originalVal = 0; \n"
        "    %s \n" // insert condition for doing atomic operation
        "    { \n"
        "      for (uint inner = 0; inner < 16; ++inner) \n"
        "      { \n"
        "        Interlocked%s(sharedMemory[index & addressMask], \n" // insert intrinsic name here
        "            valToUse %s ); \n" // insert original value optional argument here
        "        ++index; \n"
        "      } \n"
        "    } \n"
        // sync every 16 iterations
        "    GroupMemoryBarrierWithGroupSync(); \n" 
        "  } \n"
        ;
    static const char szShader_FinalWrite[] =
        "  if (threadIndex == 0) \n"
        "  { \n"
		"    [fastopt]"
        "    for (index = 0; index < 2048; ++index) \n"
        "    { \n"
        "      InterlockedAdd(targetUAV[index], (uint)sharedMemory[index] ); \n"
        "    } \n"
        "  } \n"
        "} \n"
        ;

    const UINT tx = m_NumThreads[0];
    const UINT ty = m_NumThreads[1];
    const UINT tz = m_NumThreads[2];
    const UINT numThreadsInGroup = tx*ty*tz;

    tstring sharedMemDecl = "groupshared static INTorUINT sharedMemory[2048]";
    if (SHARED_MEMORY_UAV == m_SharedMemType)
        sharedMemDecl = "RWBuffer<INTorUINT> sharedMemory";

    tstring subsetOpsString = "";
    if (m_bSubsetOperations)
    {
        subsetOpsString += "if ((threadIndex % 4) == 2) addressMask = 0xfffffffc; \n    "; // non-multiple-of-4 even threads write to 4-element aligned locations
        subsetOpsString += "if ((threadIndex % 2) == 0)"; // skip all odd thread index values
    }
    tstring intrinsicName = GetIntrinsicName();
    tstring optionalOriginalValArg = "";
    if (m_bOriginalValueVariant)
        optionalOriginalValArg = ", originalVal";
    tstring shaderValueString = GetShaderValueString();
    UINT clearVal = GetTargetStartVal();

    tstring shaderCode = FormatString( szShader_GroupSharedMem, 
        sharedMemDecl.c_str(), tx,ty,tz, shaderValueString.c_str(), clearVal, 
        subsetOpsString.c_str(), intrinsicName.c_str(), optionalOriginalValArg.c_str() );

    shaderCode += szShader_FinalWrite;
    return shaderCode;
}


//------------------------------------------------------------------------------------------------
// CAtomicAddTest
//------------------------------------------------------------------------------------------------
UINT CAtomicAddTest::GetTargetStartVal()
{
    return 0;
}

tstring CAtomicAddTest::GetShaderValueString()
{
    tstring retVal = "(1 + (threadIndex % 4))";
    if (m_bSignedInt)	
        retVal = "-(int)" + retVal;
    return retVal;
}

TEST_RESULT CAtomicAddTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;

    const UINT tx = m_NumThreads[0];
    const UINT ty = m_NumThreads[1];
    const UINT tz = m_NumThreads[2];
    const UINT numThreadsInGroup = tx*ty*tz;

    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );
    const UINT numGroups = gx*gy*gz;

    // under normal operation, all threads add values to same location
    // location += (threadindex % 4) + 1;
    // every 4 threads add 10, additional past multiple of 4 are +1, +2, +3
    UINT expectedVal = (numThreadsInGroup / 4) * 10;
    for (UINT n = 0; n < numThreadsInGroup % 4; ++n)
        expectedVal += 1 + (n % 4);

    // with subset operations, things are more complicated
    // odd threads write nothing (only values 1 and 3 are written per 4 threads)
    // multiples-of-4 write as normal
    // non-multiples-of-4 write only to 4-element-aligned locations
    // this means you'll see this pattern in the output buffer:
    //   1, 1, 1, 1,  1, ... (1 group, 1-2 threads each)
    //   13,1, 1, 1, 13, ... (1 group, 3-4 threads each)
    //   14,2, 2, 2, 14, ... (1 group, 5-6 threads each)
    //   26,2, 2, 2, 26, ... (1 group, 7-8 threads each)
    //   27,3, 3, 3, 27, ... (1 group, 9-10 threads each)
    UINT expectedValEvery4 = expectedVal;
    if (m_bSubsetOperations)
    {
        expectedVal = (numThreadsInGroup / 4) * 1;
        expectedValEvery4 = expectedVal * 13;
        if ((numThreadsInGroup % 4) > 0)
        {
            expectedVal += 1;
            expectedValEvery4 += 1;
        }
        if ((numThreadsInGroup % 4) > 2)
            expectedValEvery4 += 3 * 4;
    }

    // potential overflow, but it should match GPU
    expectedVal *= numGroups;
    expectedValEvery4 *= numGroups;

    if (m_bSignedInt)
    {
        expectedVal = (UINT)(-(INT)expectedVal);
        expectedValEvery4 = (UINT)(-(INT)expectedValEvery4);
    }

    UINT32* pData = (UINT32*)pBufferData->pData;
    for (UINT n = 0; n < m_TargetUAVSize[0]; ++n)
    {
        UINT specificExpected = (n % 4 == 0) ? expectedValEvery4 : expectedVal;
        if (pData[n] != specificExpected)
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Unexpected result at buffer[%d], Expected %d, Found %d", n, specificExpected, pData[n] );
            break;
        }
    }

    return tRes;
}


//------------------------------------------------------------------------------------------------
// CAtomicMinMaxTest
//------------------------------------------------------------------------------------------------
UINT CAtomicMinMaxTest::GetTargetStartVal()
{
    if (GetIntrinsicName() == "Max")
    {
        if (m_bSignedInt)
            return -1000;
        else
            return 0;
    }
    else
    {
        if (m_bSignedInt)
            return 0;
        else
            return 1000;
    }
}

tstring CAtomicMinMaxTest::GetShaderValueString()
{
    tstring retVal = "((1 + threadIndex) * 509) % 139";
    if (m_bSignedInt)
        retVal = "-(int)" + retVal;
    return retVal;
}

UINT CAtomicMinMaxTest::GetThreadIterationValue(UINT threadIndex)
{
    UINT retVal = ((1 + threadIndex) * 509) % 139;
    if (m_bSignedInt)
        retVal = (UINT)(-(INT)retVal);
    return retVal;
}

UINT CAtomicMinMaxTest::ModifyExpectedValue(UINT currVal, UINT threadVal, const tstring& intrinsicName)
{
    if (m_bSignedInt)
    {
        if (intrinsicName == "Max")
            currVal = (UINT)max((INT)currVal, (INT)threadVal);
        else 
            currVal = (UINT)min((INT)currVal, (INT)threadVal);
    }
    else
    {
        if (intrinsicName == "Max")
            currVal = max(currVal, threadVal);
        else 
            currVal = min(currVal, threadVal);
    }
    return currVal;
}

TEST_RESULT CAtomicMinMaxTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;

    const UINT tx = m_NumThreads[0];
    const UINT ty = m_NumThreads[1];
    const UINT tz = m_NumThreads[2];
    const UINT numThreadsInGroup = tx*ty*tz;

    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );
    const UINT numGroups = gx*gy*gz;

    const tstring intrinsicName = GetIntrinsicName();

    UINT expectedVal = GetTargetStartVal();
    UINT expectedValEvery4 = expectedVal;

    if (!m_bSubsetOperations)
    {
        // under normal operation, all threads add values to same location
        for (UINT n = 0; n < numThreadsInGroup; ++n)
        {
            UINT threadVal = GetThreadIterationValue(n);
            expectedVal = ModifyExpectedValue(expectedVal, threadVal, intrinsicName);
        }
        expectedValEvery4 = expectedVal;
    }
    else
    {
        // with subset operations, things are more complicated
        // odd threads write nothing (only values [0] and [2] are written per 4 threads)
        // multiples-of-4 write as normal
        // non-multiples-of-4 write only to 4-element-aligned locations
        // so all even threads write to the 4-element-aligned locations, 
        // but only multiples of 4 write to all memory locations
        for (UINT n = 0; n < numThreadsInGroup; ++n)
        {
            if (n % 2 == 1)
                continue;
            UINT threadVal = GetThreadIterationValue(n);
            if (n % 4 == 0)
            {
                expectedVal = ModifyExpectedValue(expectedVal, threadVal, intrinsicName);
            }
            expectedValEvery4 = ModifyExpectedValue(expectedValEvery4, threadVal, intrinsicName);
        }
    }

    // potential overflow, but it should match GPU
    expectedVal *= numGroups;
    expectedValEvery4 *= numGroups;

    UINT32* pData = (UINT32*)pBufferData->pData;
    for (UINT n = 0; n < m_TargetUAVSize[0]; ++n)
    {
        UINT specificExpected = (n % 4 == 0) ? expectedValEvery4 : expectedVal;
        if (pData[n] != specificExpected)
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Unexpected result at buffer[%d], Expected %d, Found %d", n, specificExpected, pData[n] );
            break;
        }
    }

    return tRes;
}


//------------------------------------------------------------------------------------------------
// CAtomicBitwiseTest
//------------------------------------------------------------------------------------------------
UINT CAtomicBitwiseTest::GetTargetStartVal()
{
    if (GetIntrinsicName() == "And")
        return 0xffffffff;
    else
        return 0x10000008;
}

tstring CAtomicBitwiseTest::GetShaderValueString()
{
    if (GetIntrinsicName() == "And")
        return "~((uint)1 << (threadIndex % 32))";
    else
        return "((uint)1 << (threadIndex % 32))";
}

UINT CAtomicBitwiseTest::GetThreadIterationValue(UINT threadIndex, const tstring& intrinsicName)
{
    if (intrinsicName == "And")
        return ~((UINT)1 << (threadIndex % 32));
    else
        return ((UINT)1 << (threadIndex % 32));
}

UINT CAtomicBitwiseTest::ModifyExpectedValue(UINT currVal, UINT threadVal, const tstring& intrinsicName)
{
    if (intrinsicName == "And")
        currVal &= threadVal;
    else if (intrinsicName == "Or")
        currVal |= threadVal;
    else if (intrinsicName == "Xor")
        currVal ^= threadVal;
    return currVal;
}

TEST_RESULT CAtomicBitwiseTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;

    const UINT tx = m_NumThreads[0];
    const UINT ty = m_NumThreads[1];
    const UINT tz = m_NumThreads[2];
    const UINT numThreadsInGroup = tx*ty*tz;

    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );
    const UINT numGroups = gx*gy*gz;

    const tstring intrinsicName = GetIntrinsicName();

    UINT expectedVal = GetTargetStartVal();
    UINT expectedValEvery4 = expectedVal;

    if (!m_bSubsetOperations)
    {
        // under normal operation, all threads add values to same location
        for (UINT n = 0; n < numThreadsInGroup; ++n)
        {
            UINT threadVal = GetThreadIterationValue(n, intrinsicName);
            expectedVal = ModifyExpectedValue(expectedVal, threadVal, intrinsicName);
        }
        expectedValEvery4 = expectedVal;
    }
    else
    {
        // with subset operations, things are more complicated
        // odd threads write nothing (only values 1 and 3 are written per 4 threads)
        // multiples-of-4 write as normal
        // non-multiples-of-4 write only to 4-element-aligned locations
        // so all even threads write to the 4-element-aligned locations, 
        // but only multiples of 4 write to all memory locations
        for (UINT n = 0; n < numThreadsInGroup; ++n)
        {
            if (n % 2 == 1)
                continue;
            UINT threadVal = GetThreadIterationValue(n, intrinsicName);
            if (n % 4 == 0)
            {
                expectedVal = ModifyExpectedValue(expectedVal, threadVal, intrinsicName);
            }
            expectedValEvery4 = ModifyExpectedValue(expectedValEvery4, threadVal, intrinsicName);
            if (n % 4 == 2 && intrinsicName == "Xor")
            {
                for (int k = 0; k < 3; ++k)
                {
                    expectedValEvery4 = ModifyExpectedValue(expectedValEvery4, threadVal, intrinsicName);
                }
            }
        }
    }

    // potential overflow, but it should match GPU
    expectedVal *= numGroups;
    expectedValEvery4 *= numGroups;

    UINT32* pData = (UINT32*)pBufferData->pData;
    for (UINT n = 0; n < m_TargetUAVSize[0]; ++n)
    {
        UINT specificExpected = (n % 4 == 0) ? expectedValEvery4 : expectedVal;
        if (pData[n] != specificExpected)
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Unexpected result at buffer[%d], Expected %#x, Found %#x", n, specificExpected, pData[n] );
            break;
        }
    }

    return tRes;
}


//------------------------------------------------------------------------------------------------
// CAtomicStoreTest
//------------------------------------------------------------------------------------------------
void CAtomicStoreTest::InitTestParameters()
{
    const tstring intrinsicName = GetIntrinsicName();
    m_sharedUAVClearVal = 0xffffffff;


    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );
    const UINT numThreadGroups = gx * gy * gz;

    // enough memory for all threads, plus an extra slot per group to record shared mem value
    
    // width or x data
    m_TargetUAVSize[0] = 2048;
    
    // hieght or y data
    m_TargetUAVSize[1] = (1000 * numThreadGroups) + numThreadGroups;
    
    m_TargetUAVSize[2] = 1;

    m_TargetUAVDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

    CTestCaseParameter<SHARED_MEMORY_TYPE>* pSharedMemTypeParam = AddParameter<SHARED_MEMORY_TYPE>( _T("SharedMemType"), &m_SharedMemType );
    testfactor::RFactor rSharedMemType_Group = AddParameterValue<SHARED_MEMORY_TYPE>( pSharedMemTypeParam, SHARED_MEMORY_TGSM );
    testfactor::RFactor rSharedMemType_UAV = AddParameterValue<SHARED_MEMORY_TYPE>( pSharedMemTypeParam, SHARED_MEMORY_UAV );
    testfactor::RFactor rSharedMemType_GroupAndUAV = rSharedMemType_Group + rSharedMemType_UAV;

    CTestCaseParameter<UINT>* pThreadsXParam = AddParameter<UINT>( _T("ThreadsX"), &m_NumThreads[0] );
    CTestCaseParameter<UINT>* pThreadsYParam = AddParameter<UINT>( _T("ThreadsY"), &m_NumThreads[1] );
    CTestCaseParameter<UINT>* pThreadsZParam = AddParameter<UINT>( _T("ThreadsZ"), &m_NumThreads[2] );

    testfactor::RFactor rNumThreadsX_All = AddParameterValueSet<UINT>( pThreadsXParam, new CUserValueSet<UINT>(1, 2, 3, 10) );
    testfactor::RFactor rNumThreadsY_All = AddParameterValueSet<UINT>( pThreadsYParam, new CUserValueSet<UINT>(1, 3, 7, 10) );
    testfactor::RFactor rNumThreadsZ_All = AddParameterValueSet<UINT>( pThreadsZParam, new CUserValueSet<UINT>(1, 6, 11, 10) );

    testfactor::RFactor rNumThreads_All = rNumThreadsX_All % rNumThreadsY_All % rNumThreadsZ_All;


    SetRootTestFactor
        ( rNumThreads_All * rSharedMemType_GroupAndUAV
        );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 2), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 10), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );

}

tstring CAtomicStoreTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    defines[0].Name = "INNER_SIZE";
    defines[0].Definition = "2";

    static const char szShader_GroupSharedMem[] =
        "RWTexture2D<uint> targetUAV; \n"
        "%s; \n" // insert shared memory decl here
        "[numthreads(%d,%d,%d)] \n" // insert x,y,z ; 1000 threads = 10 * 10 * 10
        "void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
        "{ \n"
        "  uint3 dispatchSize = {%#x,%#x,%#x}; \n" // insert (x,y,z) dispatch dimensions here
        "  uint numThreadsInGroup = %#x; \n" // insert num threads in group here
        "  uint numTotalThreads = numThreadsInGroup * dispatchSize.x * dispatchSize.y * dispatchSize.z; \n"
        "  uint groupIndex = groupID.z*dispatchSize.x*dispatchSize.y + groupID.y*dispatchSize.x + groupID.x; \n"
        "  uint globalThreadIndex = groupIndex * numThreadsInGroup + threadIndex; \n"
        "  uint addressMask = 0xffffffff; \n"
        "  uint clearVal = %#x; \n" // insert clearVal here
        // initialize all shared memory to clearVal
        "  if (%s) \n" // insert clear condition here (ie thread index == 0)
        "  { \n"
		"    [fastopt]"
        "    for (uint total = 0; total < 2048; ++total) \n"
        "      sharedMemory[total] = clearVal; \n"
        "  } \n"
        "  uint index = 0; \n"
        "  bool doOperation = true; \n"
		"  [fastopt]"
        "  for (uint outer = 0; outer < (2048/INNER_SIZE); ++outer) \n"
        "  { \n"
        // sync before atomic operation
        "    uint originalVals[INNER_SIZE]; \n"
        "    uint inner; \n"
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      originalVals[inner] = 0xbaadf00d; \n"
        "    } \n"
        "    AllMemoryBarrierWithGroupSync(); \n" 
        "    [unroll] \n"
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      if (doOperation) \n"
        "      { \n"
        "        Interlocked%s(sharedMemory[index], \n" // insert intrinsic name here
        "            %s  \n" // insert compare value optional argument here
        "            globalThreadIndex %s ); \n" // insert original value optional argument here
        "      } \n"
        "      %s \n" // insert optional modification of 'doOperation' here, for Compare*
        "      ++index; \n"
        "    } \n"
        // sync & record every outer iteration
        "    AllMemoryBarrierWithGroupSync(); \n" 
        ;

    static const char szShader_WriteOriginalValue[] =
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      targetUAV[uint2( outer*INNER_SIZE + inner, globalThreadIndex)] = originalVals[inner]; \n"
        "    } \n"
        "  } \n"
        ;

    static const char szShader_WriteGroupSharedMem[] =
        "  AllMemoryBarrierWithGroupSync(); \n" 
        "  if (threadIndex == 0) \n"
        "  { \n"
		"    [fastopt]"
        "    for (index = 0; index < 2048; ++index) \n"
        "    { \n"
        "      targetUAV[uint2( index, numTotalThreads + groupIndex)] = sharedMemory[index]; \n"
        "    } \n"
        "  } \n"
        ;

    static const char szShader_WriteGlobalSharedMem[] =
        "  AllMemoryBarrierWithGroupSync(); \n" 
        "  if (threadIndex == 0) \n"
        "  { \n"
		"    [fastopt]"
        "    for (index = 0; index < 2048; ++index) \n"
        "    { \n"
        "      targetUAV[uint2(index, numTotalThreads)] = sharedMemory[index]; \n"
        "    } \n"
        "  } \n"
        ;

    static const char szShader_FinishFunc[] =
        "} \n"
        ;

    const UINT tx = m_NumThreads[0];
    const UINT ty = m_NumThreads[1];
    const UINT tz = m_NumThreads[2];
    const UINT numThreadsInGroup = tx*ty*tz;

    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );

    // pick either thread-group shared memory, or a global UAV
    tstring sharedMemDecl = "groupshared static uint sharedMemory[2048]";
    tstring clearCondition = "true";
    if (SHARED_MEMORY_UAV == m_SharedMemType)
    {
        sharedMemDecl = "RWBuffer<uint> sharedMemory";
        clearCondition = "false";
    }

    const tstring intrinsicName = GetIntrinsicName();

    // if we do an exchange, we need to record the exchanged value
    tstring optionalOriginalValArg = "";
    if (tstring::npos != intrinsicName.find("Exchange"))
        optionalOriginalValArg = ", originalVals[inner]";

    // if we're doing a compare, we need to write the compare value (clearVal)
    tstring optionalCompareValArg = "";
    tstring optionalDoOperation = "";
    if (tstring::npos != intrinsicName.find("Compare"))
    {
        // with a compare, only one thread can win, so we disqualify this thread from winning in subsequent iterations
        optionalCompareValArg = "clearVal, ";
        optionalDoOperation = "if (globalThreadIndex == sharedMemory[index]) { doOperation = false; ";
        if (tstring::npos == intrinsicName.find("Exchange"))
        {
            // if we don't have an exchange, we still need to modify the "originalval" that gets written to target
            optionalDoOperation += "originalVals[inner] = clearVal; ";
        }
        optionalDoOperation += "}";
        // reset the 'doOperation' value every N iterations, where N = num threads that can see the shared memory
        const char* szThreadCount = (SHARED_MEMORY_UAV == m_SharedMemType) ? "numTotalThreads" : "numThreadsInGroup";
        optionalDoOperation += FormatString("if ((index %% %s) == (%s - 1)) doOperation = true;", szThreadCount, szThreadCount);
    }

    tstring shaderCode = FormatString( szShader_GroupSharedMem, 
        sharedMemDecl.c_str(), tx,ty,tz, gx,gy,gz, numThreadsInGroup, m_sharedUAVClearVal, clearCondition.c_str(),
        intrinsicName.c_str(), optionalCompareValArg.c_str(), optionalOriginalValArg.c_str(),
        optionalDoOperation.c_str() );

    shaderCode += szShader_WriteOriginalValue;
    if (SHARED_MEMORY_TGSM == m_SharedMemType)
        shaderCode += szShader_WriteGroupSharedMem;
    else
        shaderCode += szShader_WriteGlobalSharedMem;
    shaderCode += szShader_FinishFunc;
    
	if( g_TestApp.m_bLogShaderText )
		WriteToLog( "Shader Code =%s", shaderCode.c_str() );

    return shaderCode;
}

TEST_RESULT CAtomicStoreTest::VerifyWinner(WinnerInfo winner)
{
    TEST_RESULT tRes = RESULT_FAIL;

    if(winner.Count == 1 && winner.SharedUAVEqualsTargetUAV == true)
    {
        tRes = RESULT_PASS;
    }
    else
    {
        if(winner.Count == 0)
        {
            WriteToLog("FAIL: No winning thread.");
        }

        if(winner.Count > 1)
        {
            WriteToLog("FAIL: More than one winning thread.");
        }

        if(winner.SharedUAVEqualsTargetUAV == false)
        {
            WriteToLog("FAIL: sharedUAV array != targetUAV arrray");
        }


        tRes = RESULT_FAIL;
    }

    return tRes;
}

TEST_RESULT CAtomicStoreTest::VerifyCompareExchangeReturnValues(
    UINT iteration,
    WinnerInfo winner,
    UINT startGroup,
    UINT numberOfGroups,
    UINT numThreadsPerGroup,
    const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;
    UINT unchangedReturnCount = 0;

    for (UINT group = startGroup; group < startGroup + numberOfGroups ; ++group)
    {
        for (UINT thread = 0; thread < numThreadsPerGroup; ++thread)
        {
            UINT threadId = (group * numThreadsPerGroup) + thread;

            const UINT* pReturnData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * threadId   );				
            const UINT returnData = pReturnData[iteration];

            if(thread == winner.Thread && group == winner.Group)
            {
                if(returnData != m_sharedUAVClearVal)
                {
                    tRes = RESULT_FAIL;
                    WriteToLog("FAIL: VerifyCompareExchangeReturnValues. returnData(%u) != clearValue(0x%x) group=%u thread=%u x=%u",returnData, m_sharedUAVClearVal, group, thread, iteration);
                    goto CleanUp;
                }
            }
            else
            {
                //
                // If the operation passes the compare but doesn't gain the lock, it doesn't
                // update the shared location and the return value is unchanged
                //
                if(returnData != winner.Value && returnData != 0xbaadf00d)
                {
                    tRes = RESULT_FAIL;
                    goto CleanUp;
                }

                if(returnData == 0xbaadf00d)
                {
                    unchangedReturnCount++;	
                }
            }			
        }//thread
    }

    //
    // Shader disables one winning thread. Then re-enables them when they are all disbaled either per group or per iteration.
    //
    if(unchangedReturnCount != (iteration % ( numberOfGroups * numThreadsPerGroup)) )
    {
        WriteToLog("FAIL: VerifyCompareExchangeReturnValues. x=%u. %u threads had unchanged return values. Expected(%u).  iteration(%u)  mod  numThreads(%u)",  iteration, unchangedReturnCount, (iteration % ( numberOfGroups * numThreadsPerGroup)), iteration ,( numberOfGroups * numThreadsPerGroup) );	
        tRes = RESULT_FAIL;
    }

CleanUp:
    return tRes;

}

TEST_RESULT CAtomicStoreTest::VerifyExchangeReturnValues(
    UINT iteration,
    UINT startGroup,
    UINT numberOfGroups,
    UINT numThreadsPerGroup,
    const D3D11_MAPPED_SUBRESOURCE* pBufferData,
    __out UINT* pLastUpdate)
{
    TEST_RESULT tRes = RESULT_PASS;
    bool foundClear = false;

    
    bool* pFound = new bool[numberOfGroups * numThreadsPerGroup];
    ZeroMemory( (void*)pFound, sizeof(bool) * (numberOfGroups * numThreadsPerGroup) );

    UINT lowerBound = (startGroup * numThreadsPerGroup);	
    UINT upperBound = (startGroup + numberOfGroups) * numThreadsPerGroup;
            

    for (UINT group = startGroup; group < startGroup + numberOfGroups ; ++group)
    {
        for (UINT thread = 0; thread < numThreadsPerGroup; ++thread)
        {
            UINT threadId = (group * numThreadsPerGroup) + thread;

            const UINT* pReturnData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * threadId );				
            const UINT returnData = pReturnData[iteration];
            
            if(returnData >= lowerBound &&  returnData < upperBound )
            {
                if(pFound[returnData % (numberOfGroups * numThreadsPerGroup)] == false)
                {
                    pFound[returnData % (numberOfGroups * numThreadsPerGroup)] = true;
                }
                else
                {
                    WriteToLog("FAIL:VerifyExchangeReturnValues. Found duplicate entry %u ", returnData);
                    tRes = RESULT_FAIL;	
                    goto CleanUp;
                }
            }
            else if(returnData == m_sharedUAVClearVal)
            {
                foundClear = true;	
            }
            else
            {
                WriteToLog("FAIL:VerifyExchangeReturnValues. Found value outside expected values %u ", returnData);
                tRes = RESULT_FAIL;	
                goto CleanUp;
            }

        }//thread

    }//group

    if(!foundClear)
    {
        WriteToLog("FAIL:VerifyExchangeReturnValues. Clear value not found.  Exhange did not occur");
        tRes = RESULT_FAIL;	
        goto CleanUp;
    }

    //
    // Find the last one to update. This will be the value of shared mem
    //
    bool foundOne = false;
    for (UINT i = 0; i < numberOfGroups * numThreadsPerGroup ; ++i)
    {
        if( !pFound[i])
        {
            if(foundOne == false)
            {
                *pLastUpdate = i + lowerBound;
                foundOne = true;
            }
            else
            {
                WriteToLog("FAIL:VerifyExchangeReturnValues. Found multiple unchanged entries. %u ", i);
                tRes = RESULT_FAIL;	
                break;
            }
        }
    }

CleanUp:
    delete[] pFound;
    return tRes;
}

TEST_RESULT CAtomicStoreTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    HRESULT hr;
    TEST_RESULT tRes = RESULT_PASS;
    bool bMappedSharedBuffer = false;
    const tstring intrinsicName = GetIntrinsicName();


    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );
    const UINT numThreadGroups = gx * gy * gz;
    const UINT numThreadsPerGroup = m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];
    const UINT totalThreadCount = numThreadsPerGroup * numThreadGroups;

    assert(totalThreadCount <= 10000);

    D3D11_MAPPED_SUBRESOURCE mappedSharedBuffer;
    ZeroStructMem(mappedSharedBuffer);
    if (m_pSharedUAVBufferCopy != NULL)
    {
        GetImmediateContext()->CopyResource( m_pSharedUAVBufferCopy, m_pSharedUAVBuffer );
        hr = GetImmediateContext()->Map( m_pSharedUAVBufferCopy, 0, D3D11_MAP_READ, 0, &mappedSharedBuffer );
        if (FAILED(hr))
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Map(m_pSharedUAVBufferCopy) failed, hr=%s", D3DHResultToString(hr).c_str() );
            goto CleanUp;
        }
        else
        {
            bMappedSharedBuffer = true;
        }
    }

    
    ////
    //// For debugging. Print out all the data from the test
    ////
    //for (UINT x = 0; x < 2048 ; ++x)
    //{
    //		UINT sharedMemData = 0xffffffff;

    //		if(SHARED_MEMORY_UAV == m_SharedMemType)
    //		{
    //			sharedMemData = *((UINT*)mappedSharedBuffer.pData + x);
    //		}
    //		
    //		
    //		const UINT* pTargetUAV = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * ((numThreadsPerGroup * numThreadGroups)) );
    //		const UINT returnData = pTargetUAV[x];
    //		

    //		for(UINT i = 0; i < numThreadGroups ; ++i)
    //		{
    //			if(SHARED_MEMORY_TGSM == m_SharedMemType)
    //			{
    //				const UINT* pSharedMemData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * ((numThreadsPerGroup * numThreadGroups) + i) );
    //				sharedMemData = pSharedMemData[x];
    //			}

    //			for(UINT j = 0; j < numThreadsPerGroup ; ++j)
    //			{

    //				UINT y = (i * numThreadsPerGroup) + j;
    //				const UINT* pdata = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * (y) );

    //				WriteToLog("RAW DATA: sharedmem[0][%u]=0x%x, copied sharedmem[0][%u]=0x%x data[%u][%u]=0x%x",x, sharedMemData, x, returnData,y,x,pdata[x]);
    //			}
    //		}
    //}

    for (UINT x = 0; x < 2048 ; ++x)
    {
        WinnerInfo iterationWinner = {0xffffffff, 0xffffffff,0xbaadf00d,0,true };
        for (UINT group = 0; group < numThreadGroups ; ++group)
        {
            WinnerInfo groupWinner = {0xffffffff, 0xffffffff,0xbaadf00d,0,true };

            UINT sharedMemData = 0xffffffff;
            if(SHARED_MEMORY_TGSM == m_SharedMemType)
            {
                const UINT* pSharedMemData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * ((numThreadsPerGroup * numThreadGroups) + group) );
                sharedMemData = pSharedMemData[x];
            }
            else if (SHARED_MEMORY_UAV == m_SharedMemType)
            {
                const UINT* pTargetUAV = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * ((numThreadsPerGroup * numThreadGroups)) );	
                sharedMemData = *((UINT*)mappedSharedBuffer.pData + x);

                //
                // Verify targetUAV copied in from shader equals sharedUAV
                //
                if(sharedMemData != pTargetUAV[x])
                {
                    WriteMessage("targetUAV(%u) != sharedUAV(%u) x=%u",pTargetUAV[x], sharedMemData, x);
                    groupWinner.SharedUAVEqualsTargetUAV = false;
                }
            }
            
            for (UINT thread = 0; thread < numThreadsPerGroup; ++thread)
            {
                if( sharedMemData == ( group * numThreadsPerGroup ) + thread)
                {
                    if(groupWinner.Count == 0)
                    {
                        groupWinner.Thread = thread;
                        groupWinner.Group = group;
                        groupWinner.Value = sharedMemData;
                    }

                    groupWinner.Count = groupWinner.Count + 1;

                    if(iterationWinner.Count == 0)
                    {
                        iterationWinner.Thread = thread;
                        iterationWinner.Group = group;
                        iterationWinner.Value = sharedMemData;
                    }

                    iterationWinner.Count = iterationWinner.Count + 1;
                }

            }//thread

            if(SHARED_MEMORY_TGSM == m_SharedMemType)
            {
                tRes = VerifyWinner(groupWinner);
                if (tRes != RESULT_PASS)
                {
                    goto CleanUp;
                }

                if ( (intrinsicName == "CompareExchange") )
                {
                    tRes = VerifyCompareExchangeReturnValues(
                        x,
                        groupWinner,
                        group,
                        1,
                        numThreadsPerGroup,
                        pBufferData);

                    if (tRes != RESULT_PASS)
                    {
                        goto CleanUp;
                    }

                }
                else if (intrinsicName == "Exchange")
                {
                    UINT lastUpdate = 0;
                    tRes = VerifyExchangeReturnValues(
                        x,
                        group,
                        1,
                        numThreadsPerGroup,
                        pBufferData,
                        &lastUpdate);

                    if (tRes != RESULT_PASS)
                    {
                        goto CleanUp;
                    }

                    if(groupWinner.Value != lastUpdate)
                    {
                        WriteToLog("groupWinner.value(%u) != lastUpdate(%u)", groupWinner.Value, lastUpdate);
                        tRes = RESULT_FAIL;
                        goto CleanUp;
                    }
                }
            }//group validation
        }//group

        if(SHARED_MEMORY_UAV == m_SharedMemType)
        {
            tRes = VerifyWinner(iterationWinner);
            if (tRes != RESULT_PASS) 
            {
                goto CleanUp;
            }

            if ( (intrinsicName == "CompareExchange") )
            {
                tRes = VerifyCompareExchangeReturnValues(
                    x,
                    iterationWinner,
                    0,
                    numThreadGroups,
                    numThreadsPerGroup,
                    pBufferData);

                if (tRes != RESULT_PASS)
                {
                    goto CleanUp;
                }
            }
            else if ( (intrinsicName == "Exchange") )
            {
                UINT lastUpdate = 0;
                tRes = VerifyExchangeReturnValues(
                    x,
                    0,
                    numThreadGroups,
                    numThreadsPerGroup,
                    pBufferData,
                    &lastUpdate);

                if (tRes != RESULT_PASS)
                {
                    goto CleanUp;
                }

                if(iterationWinner.Value != lastUpdate)
                {
                    WriteToLog("iterationWinner.value(%u) != lastUpdate(%u)", iterationWinner.Value, lastUpdate);
                    tRes = RESULT_FAIL;
                    goto CleanUp;
                }
            }

        }//iteration validation

    } //iteration

CleanUp:

    if (bMappedSharedBuffer)
        GetImmediateContext()->Unmap( m_pSharedUAVBufferCopy, 0 );

    return tRes;
}

//------------------------------------------------------------------------------------------------
//
// CMultiAtomicTest
// Base class for tests the call InterlockedAdd with InterlockedStore 
// and InterlockedExchange methods
//
//------------------------------------------------------------------------------------------------
CMultiAtomicTest::CMultiAtomicTest()
{
    m_Shader_GroupSharedMem =
    "RWTexture2D<uint> targetUAV; \n"
    "%s; \n" // insert shared memory decl here
    "[numthreads(%d,%d,%d)] \n" // insert x,y,z ; 1000 threads = 10 * 10 * 10
    "void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
    "{ \n"
    "  uint3 dispatchSize = {%#x,%#x,%#x}; \n" // insert (x,y,z) dispatch dimensions here
    "  uint numThreadsInGroup = %#x; \n" // insert num threads in group here
    "  uint numTotalThreads = numThreadsInGroup * dispatchSize.x * dispatchSize.y * dispatchSize.z; \n"
    "  uint groupIndex = groupID.z*dispatchSize.x*dispatchSize.y + groupID.y*dispatchSize.x + groupID.x; \n"
    "  uint globalThreadIndex = groupIndex * numThreadsInGroup + threadIndex; \n"
    "  uint clearVal = %#x; \n"
    "  uint originalValsClearVal = %#x; \n"
    // initialize all shared memory to clearVal
    "  if (%s) \n" // insert clear condition here (ie thread index == 0)
    "  { \n"
	"    [fastopt]"
    "    for (uint total = 0; total < 2048; ++total) \n"
    "      sharedMemory[total] = clearVal; \n"
    "  } \n"
    "  uint index = 0; \n"
    "  bool doOperation = true; \n"
	"  [fastopt]"
    "  for (uint outer = 0; outer < (2048/INNER_SIZE); ++outer) \n"
    "  { \n"
    // sync before atomic operation
    "    uint originalVals[INNER_SIZE]; \n"
    "    uint inner; \n"
    "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
    "    { \n"
    "      originalVals[inner] = originalValsClearVal; \n"
    "    } \n"
    "    AllMemoryBarrierWithGroupSync(); \n" 
    "    [unroll] \n"
    "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
    "    { \n"
    "      %s \n" // insert optional modification of 'doOperation' here, for Compare*
    "      ++index; \n"
    "    } \n"
    // sync & record every outer iteration
    "    AllMemoryBarrierWithGroupSync(); \n" 
    "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
    "    { \n"
    "      targetUAV[uint2( outer*INNER_SIZE + inner, globalThreadIndex)] = originalVals[inner]; \n"
    "    } \n"
    "  } \n"
    "	AllMemoryBarrierWithGroupSync(); \n" 
    "	if (threadIndex == 0) \n"
    "	{ \n"
	"       [fastopt]"
    "		for (index = 0; index < 2048; ++index) \n"
    "		{ \n"
    "			targetUAV[uint2(index, numTotalThreads %s)] = sharedMemory[index]; \n"
    "		} \n"
    "  } \n"
    "} \n";
}
    
void CMultiAtomicTest::InitTestParameters()
{
    const tstring intrinsicName = GetIntrinsicName();
    m_sharedUAVClearVal = 0;


    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );
    const UINT numThreadGroups = gx * gy * gz;

    // enough memory for all threads, plus an extra slot per group to record shared mem value
    
    // width or x data
    m_TargetUAVSize[0] = 2048;
    
    // hieght or y data
    m_TargetUAVSize[1] = (1000 * numThreadGroups) + numThreadGroups;
    
    m_TargetUAVSize[2] = 1;

    m_TargetUAVDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

    CTestCaseParameter<SHARED_MEMORY_TYPE>* pSharedMemTypeParam = AddParameter<SHARED_MEMORY_TYPE>( _T("SharedMemType"), &m_SharedMemType );
    testfactor::RFactor rSharedMemType_Group = AddParameterValue<SHARED_MEMORY_TYPE>( pSharedMemTypeParam, SHARED_MEMORY_TGSM );
    testfactor::RFactor rSharedMemType_UAV = AddParameterValue<SHARED_MEMORY_TYPE>( pSharedMemTypeParam, SHARED_MEMORY_UAV );
    testfactor::RFactor rSharedMemType_GroupAndUAV = rSharedMemType_Group + rSharedMemType_UAV;

    CTestCaseParameter<UINT>* pThreadsXParam = AddParameter<UINT>( _T("ThreadsX"), &m_NumThreads[0] );
    CTestCaseParameter<UINT>* pThreadsYParam = AddParameter<UINT>( _T("ThreadsY"), &m_NumThreads[1] );
    CTestCaseParameter<UINT>* pThreadsZParam = AddParameter<UINT>( _T("ThreadsZ"), &m_NumThreads[2] );

    //
    // Always need an even number of threads for this test.  
    //
    testfactor::RFactor rNumThreadsX_All = AddParameterValueSet<UINT>( pThreadsXParam, new CUserValueSet<UINT>(1, 2, 4, 10) );
    testfactor::RFactor rNumThreadsY_All = AddParameterValueSet<UINT>( pThreadsYParam, new CUserValueSet<UINT>(2, 3, 6, 10) );
    testfactor::RFactor rNumThreadsZ_All = AddParameterValueSet<UINT>( pThreadsZParam, new CUserValueSet<UINT>(1, 6, 8, 10) );

    testfactor::RFactor rNumThreads_All = rNumThreadsX_All % rNumThreadsY_All % rNumThreadsZ_All;


    SetRootTestFactor
        ( rNumThreads_All * rSharedMemType_GroupAndUAV
        );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 2), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 10), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

TEST_RESULT CMultiAtomicTest::SetupTestCase()
{
    const UINT threadCount = m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];
    if (threadCount % 2 != 0)
    {
        WriteToLog("Test requires an even number of threads");
        return RESULT_SKIP;
    }

    return CComputeAtomicityTest::SetupTestCase();
}

TEST_RESULT CMultiAtomicTest::VerifyReturnValues(
    UINT iteration,
    UINT startGroup,
    UINT numberOfGroups,
    UINT numThreadsPerGroup,
    bool replaceOccured,
    const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;

	//
	// number of adds + clear
	//
	UINT numberOfExpectValues = 1+ ((numberOfGroups * numThreadsPerGroup) / 2);
    bool* pFound = new bool[numberOfExpectValues];
    ZeroMemory( (void*)pFound, sizeof(bool) * (numberOfExpectValues) );
        
    for (UINT group = startGroup; group < startGroup + numberOfGroups ; ++group)
    {       
        for (UINT thread = 0; thread < numThreadsPerGroup; ++thread)
        {
            UINT threadId = (group * numThreadsPerGroup) + thread;

            const UINT* pReturnData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * threadId );				
            const UINT returnData = pReturnData[iteration];

            UINT expectedValue = returnData;
			if(returnData == (m_ClearVal + 1) && (GetIntrinsicName() == "CompareStore"))
            {
                //
                // CompareStore does not have a return value. But the code block sets m_ClearVal + 1
                //
                continue;
            }
            else if( (returnData & m_ReplaceValue)  && replaceOccured)
            {
                expectedValue &=  ~m_ReplaceValue;	
            }
            else if (replaceOccured && returnData != 0)
            {
                WriteToLog("Expected return data to contain m_ReplaceValue(0x%x)", m_ReplaceValue);
                tRes = RESULT_FAIL;	
                goto CleanUp;
            }
            else if (returnData & m_ReplaceValue)
            {
                WriteToLog("Return data contains m_ReplaceValue(0x%x) Not expected. replaceOccured %u", m_ReplaceValue, replaceOccured);
                tRes = RESULT_FAIL;
                goto CleanUp;
            }

            if(expectedValue == m_sharedUAVClearVal)
            {
                //
                // Use the 0 location for the clear color
                //
                pFound[0] = true;
            }
            else if( expectedValue != 0  && expectedValue < numberOfExpectValues)
            {
                pFound[expectedValue] = true;
            }
            else
            {
                WriteToLog("FAIL:VerifyReturnValues. Value(%u) outside of range(%u)", expectedValue, numberOfExpectValues);
                tRes = RESULT_FAIL;
                goto CleanUp;
            }
        }
    }

	UINT foundCount = 0;
    for(UINT i = 0; i < numberOfExpectValues; i++)
    {
        //WriteToLog("pFound[%u]=%u", i, pFound[i], numberOfExpectValues);
        if(pFound[i] == true)
        {
            foundCount++;
        }
    }

	if( foundCount != numberOfExpectValues && foundCount != (numberOfExpectValues - 1))
	{
		if(replaceOccured)
		{
			WriteToLog("Replace Occured: Expected return values (#additions + clear) = %u. Actual = %u.", numberOfExpectValues, foundCount);
		}
		else
		{
			WriteToLog("Expected return values (#additions + clear) = %u. Actual = %u.", numberOfExpectValues, foundCount);
		}
		
		tRes = RESULT_FAIL;	
	}

CleanUp:

    delete [] pFound;

    return tRes;

}

TEST_RESULT CMultiAtomicTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    HRESULT hr;
    TEST_RESULT tRes = RESULT_PASS;
    bool bMappedSharedBuffer = false;
    const tstring intrinsicName = GetIntrinsicName();

    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );
    const UINT numThreadGroups = gx * gy * gz;
    const UINT numThreadsPerGroup = m_NumThreads[0] * m_NumThreads[1] * m_NumThreads[2];
    const UINT totalThreadCount = numThreadsPerGroup * numThreadGroups;

    assert(totalThreadCount <= 10000);

    D3D11_MAPPED_SUBRESOURCE mappedSharedBuffer;
    ZeroStructMem(mappedSharedBuffer);
    if (m_pSharedUAVBufferCopy != NULL)
    {
        GetImmediateContext()->CopyResource( m_pSharedUAVBufferCopy, m_pSharedUAVBuffer );
        hr = GetImmediateContext()->Map( m_pSharedUAVBufferCopy, 0, D3D11_MAP_READ, 0, &mappedSharedBuffer );
        if (FAILED(hr))
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Map(m_pSharedUAVBufferCopy) failed, hr=%s", D3DHResultToString(hr).c_str() );
            goto CleanUp;
        }
        else
        {
            bMappedSharedBuffer = true;
        }
    }

    /****************************For debugging. Print out all the data from the test******************/
    bool debugPrint = false;
    if(debugPrint)
    {
        for (UINT x = 0; x < 2048 ; ++x)
        {
            UINT sharedMemData = 0xffffffff;

            if(SHARED_MEMORY_UAV == m_SharedMemType)
            {
                sharedMemData = *((UINT*)mappedSharedBuffer.pData + x);
            }


            const UINT* pTargetUAV = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * ((numThreadsPerGroup * numThreadGroups)) );
            const UINT returnData = pTargetUAV[x];


            for(UINT i = 0; i < numThreadGroups ; ++i)
            {
                if(SHARED_MEMORY_TGSM == m_SharedMemType)
                {
                    const UINT* pSharedMemData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * ((numThreadsPerGroup * numThreadGroups) + i) );
                    sharedMemData = pSharedMemData[x];
                }

                for(UINT j = 0; j < numThreadsPerGroup ; ++j)
                {

                    UINT y = (i * numThreadsPerGroup) + j;
                    const UINT* pdata = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * (y) );

                    WriteToLog("RAW DATA: sharedmem[0][%u]=0x%x, copied sharedmem[0][%u]=0x%x data[%u][%u]=0x%x",x, sharedMemData, x, returnData,y,x,pdata[x]);
                }
            }
        }
    }
    /*************************************************************************************************************/

    for (UINT x = 0; x < 2048 ; ++x)
    {
        UINT sharedMemData = 0xffffffff;
        bool replaceOccured = false;
        for (UINT group = 0; group < numThreadGroups ; ++group)
        {
            sharedMemData = 0xffffffff;
            if(SHARED_MEMORY_TGSM == m_SharedMemType)
            {
                const UINT* pSharedMemData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * ((numThreadsPerGroup * numThreadGroups) + group) );
                sharedMemData = pSharedMemData[x];
            }
            else if (SHARED_MEMORY_UAV == m_SharedMemType)
            {
                const UINT* pTargetUAV = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * ((numThreadsPerGroup * numThreadGroups)) );	
                sharedMemData = *((UINT*)mappedSharedBuffer.pData + x);

                //
                // Verify targetUAV copied in from shader equals sharedUAV
                //
                if(sharedMemData != pTargetUAV[x])
                {
                    WriteToLog("targetUAV(0x%x) != sharedUAV(0x%x) x=%u",pTargetUAV[x], sharedMemData, x);
                    tRes == RESULT_FAIL;
                    goto CleanUp;
                }
            }

            if(sharedMemData & m_ReplaceValue)
            {
                replaceOccured = true;
            }			

            if(SHARED_MEMORY_TGSM == m_SharedMemType)
            {
                UINT numberOfAdds = (numThreadsPerGroup / 2);

                if( (sharedMemData ==  (m_ReplaceValue | (m_AddValue *numberOfAdds))) 
                    || (sharedMemData == (m_AddValue * numberOfAdds)) )
                {
                    assert((tRes = RESULT_FAIL) || (tRes == RESULT_PASS));
                }
                else
                {
                    WriteToLog("FAIL: group= %u, iteration= %u, sharedMemData = 0x%x. Should be 0x%x or 0x%x",  
                        group,
                        x,
                        sharedMemData,
                        (m_ReplaceValue | m_AddValue) * numberOfAdds,
                        m_AddValue * numberOfAdds );

                    tRes = RESULT_FAIL;
                    goto CleanUp;
                }

                tRes = VerifyReturnValues(
                    x,
                    group,
                    1,
                    numThreadsPerGroup,
                    replaceOccured,
                    pBufferData);

                if (tRes != RESULT_PASS)
                {
                    goto CleanUp;
                }
            }
        }// group

        if(SHARED_MEMORY_UAV == m_SharedMemType)
        {
            UINT numberOfAdds = (numThreadGroups * numThreadsPerGroup / 2);

            if( (sharedMemData ==  (m_ReplaceValue | (m_AddValue * numberOfAdds))) 
                || (sharedMemData == (m_AddValue * numberOfAdds)) )
            {
                assert((tRes = RESULT_FAIL) || (tRes == RESULT_PASS));
            }
            else
            {
                WriteToLog("iteration= %u, Final sharedMemData = 0x%x. Should be 0x%x or 0x%x",  x, sharedMemData, m_ReplaceValue | m_AddValue, m_AddValue );
                tRes = RESULT_FAIL;
                goto CleanUp;
            }

            tRes = VerifyReturnValues(
                x,
                0,
                numThreadGroups,
                numThreadsPerGroup,
                replaceOccured,
                pBufferData);

            if (tRes != RESULT_PASS)
            {
                goto CleanUp;
            }
        }
    }

CleanUp:

    if (bMappedSharedBuffer)
        GetImmediateContext()->Unmap( m_pSharedUAVBufferCopy, 0 );

    return tRes;
}

tstring CMultiAtomicCompareExchangeTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    defines[0].Name = "INNER_SIZE";
    defines[0].Definition = "4";

    m_ReplaceValue = (1 << 30);
    m_AddValue = 1;
    m_ClearVal = 0xbaadf00d;
    

    static const char testedOperation[] = 
        "		if (doOperation) \n"
        "		{ \n"
        "			if ( ((globalThreadIndex ) %% 2) == 1) \n"
        "			{ \n"
        "				InterlockedCompareExchange(sharedMemory[index], clearVal, 0x%x , originalVals[inner] ); \n"
        "			} \n" 
        "			else \n"
        "			{ \n"
        "				InterlockedAdd( sharedMemory[index], 0x%x,  originalVals[inner] ); \n" 
        "			} \n"
        "			AllMemoryBarrierWithGroupSync(); \n" 
        "		} \n";

    tstring test = FormatString(testedOperation, m_ReplaceValue, m_AddValue);

    const UINT tx = m_NumThreads[0];
    const UINT ty = m_NumThreads[1];
    const UINT tz = m_NumThreads[2];
    const UINT numThreadsInGroup = tx*ty*tz;

    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );

    tstring shaderCode = FormatString( m_Shader_GroupSharedMem.c_str(), 
        SHARED_MEMORY_UAV == m_SharedMemType ? "RWBuffer<uint> sharedMemory" : "groupshared static uint sharedMemory[2048]",
        tx,ty,tz,
        gx,gy,gz,
        numThreadsInGroup,
        m_sharedUAVClearVal,
        m_ClearVal,
        SHARED_MEMORY_UAV == m_SharedMemType ? "false" : "true", // Clear condition
        test.c_str(),
        SHARED_MEMORY_UAV == m_SharedMemType ? "" : " + groupIndex");

	if( g_TestApp.m_bLogShaderText )
		WriteToLog( "Shader Code =%s", shaderCode.c_str() );
    
    return shaderCode;
}

tstring CMultiAtomicCompareStoreTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    defines[0].Name = "INNER_SIZE";
    defines[0].Definition = "4";

    m_ReplaceValue = (1 << 30);
    m_AddValue = 1;
    m_ClearVal = 0xbaadf00d;

    static const char testedOperation[] = 
        "		if (doOperation) \n"
        "		{ \n"
        "			if ( ((globalThreadIndex ) %% 2) == 1) \n"
        "			{ \n"
        "				InterlockedAdd( sharedMemory[index], 0x%x,  originalVals[inner] ); \n" 
        "			} \n" 
        "			else \n"
        "			{ \n"
        "				InterlockedCompareStore(sharedMemory[index], clearVal, 0x%x ); \n"
        "				originalVals[inner] = originalVals[inner] + 1;\n" //Fill with something so we can skip in validation
        "			} \n"
        "			AllMemoryBarrierWithGroupSync(); \n" 
        "			} \n";

    tstring test = FormatString(testedOperation, m_AddValue, m_ReplaceValue);

    const UINT tx = m_NumThreads[0];
    const UINT ty = m_NumThreads[1];
    const UINT tz = m_NumThreads[2];
    const UINT numThreadsInGroup = tx*ty*tz;

    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );

    tstring shaderCode = FormatString( m_Shader_GroupSharedMem.c_str(), 
        SHARED_MEMORY_UAV == m_SharedMemType ? "RWBuffer<uint> sharedMemory" : "groupshared static uint sharedMemory[2048]",
        tx,ty,tz,
        gx,gy,gz,
        numThreadsInGroup,
        m_sharedUAVClearVal,
        m_ClearVal,
        SHARED_MEMORY_UAV == m_SharedMemType ? "false" : "true",// Clear condition
        test.c_str(),
        SHARED_MEMORY_UAV == m_SharedMemType ? "" : " + groupIndex");

	if( g_TestApp.m_bLogShaderText )
		WriteToLog( "Shader Code =%s", shaderCode.c_str() );
    
    return shaderCode;
}

//------------------------------------------------------------------------------------------------
// CAtomicMultipleIntrinsicTest
//------------------------------------------------------------------------------------------------
UINT g_numThreads[]= 
{
	// 2048 groups of 32 threads
	32, 1, 1, 
	1, 32, 1, 
	1, 1, 32,
	// 32 groups of 1024 threads
	1024, 1, 1, 
	1, 1024, 1,
	//1, 1, 1024 // the Max value for Z-dimention is 64
};

void CAtomicMultipleIntrinsicTest::InitTestParameters()
{
 
    m_TargetUAVDimension = D3D11_UAV_DIMENSION_BUFFER;

    CTestCaseParameter<SHARED_MEMORY_TYPE>* pSharedMemTypeParam = AddParameter<SHARED_MEMORY_TYPE>( _T("SharedMemType"), &m_SharedMemType );
    testfactor::RFactor rSharedMemType_Group = AddParameterValue<SHARED_MEMORY_TYPE>( pSharedMemTypeParam, SHARED_MEMORY_TGSM );
    testfactor::RFactor rSharedMemType_UAV = AddParameterValue<SHARED_MEMORY_TYPE>( pSharedMemTypeParam, SHARED_MEMORY_UAV );
    testfactor::RFactor rSharedMemType_GroupAndUAV = rSharedMemType_Group + rSharedMemType_UAV;

    CTestCaseParameter<UINT>* pThreadsXParam = AddParameter<UINT>( _T("ThreadsX"), &m_NumThreads[0] );
    CTestCaseParameter<UINT>* pThreadsYParam = AddParameter<UINT>( _T("ThreadsY"), &m_NumThreads[1] );
    CTestCaseParameter<UINT>* pThreadsZParam = AddParameter<UINT>( _T("ThreadsZ"), &m_NumThreads[2] );

	CTestCaseParameter<bool>* pIsInterGroupParam = AddParameter<bool>( _T("InterGroupAccess"), &m_bInterGroup );
	testfactor::RFactor rfInterGroupTrue = AddParameterValue( pIsInterGroupParam, true );
	testfactor::RFactor rfInterGroupFalse = AddParameterValue( pIsInterGroupParam, false );

    testfactor::RFactor rfSmallNumX = AddParameterValueSet<UINT>( pThreadsXParam, 
		new CTableValueSet<UINT>( &(g_numThreads[0]), sizeof(UINT) * 3, 3) );
    testfactor::RFactor rfSmallNumY = AddParameterValueSet<UINT>( pThreadsYParam, 
		new CTableValueSet<UINT>( &(g_numThreads[1]), sizeof(UINT) * 3, 3));
    testfactor::RFactor rfSmallNumZ = AddParameterValueSet<UINT>( pThreadsZParam, 
		new CTableValueSet<UINT>( &(g_numThreads[2]), sizeof(UINT) * 3, 3));

	testfactor::RFactor rfBigNumX = AddParameterValueSet<UINT>( pThreadsXParam, 
		new CTableValueSet<UINT>( &(g_numThreads[9 + 0]), sizeof(UINT) * 3, 2) );
    testfactor::RFactor rfBigNumY = AddParameterValueSet<UINT>( pThreadsYParam, 
		new CTableValueSet<UINT>( &(g_numThreads[9 + 1]), sizeof(UINT) * 3, 2));
    testfactor::RFactor rfBigNumZ = AddParameterValueSet<UINT>( pThreadsZParam, 
		new CTableValueSet<UINT>( &(g_numThreads[9 + 2]), sizeof(UINT) * 3, 2));


    testfactor::RFactor rfRoot = 
		// Simultaneous access of the threads within the same group
		( rfSmallNumX % rfSmallNumY % rfSmallNumZ ) * rfInterGroupFalse * rSharedMemType_GroupAndUAV + 
		// Simultaneous access of intergroup threads
		( rfBigNumX % rfBigNumY % rfBigNumZ ) * rfInterGroupTrue * rSharedMemType_UAV;

    SetRootTestFactor( rfRoot );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 32), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ThreadsX"), 1024), WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );

}

TEST_RESULT CAtomicMultipleIntrinsicTest::SetupTestCase()
{
	 // Enough memory for all thread groups, with 32 output values for each group
    m_TargetUAVSize[0] = GetNumberOfGroupsDispatched() * 32;
    m_TargetUAVSize[1] = 1;
    m_TargetUAVSize[2] = 1;
	if ( m_bInterGroup )
	{
		// 32 groups of 1024 threads
		 m_TargetUAVSize[0] = GetNumberOfGroupsDispatched() * 1024;
	}

	return CComputeAtomicityTest::SetupTestCase();
}

tstring CAtomicMultipleIntrinsicTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    static const char szShader_GroupSharedMem[] =
        "RWBuffer<uint> targetUAV; \n"
        "%s; \n" // insert shared memory decl here
        "[numthreads(%d,%d,%d)] \n" // insert x,y,z ; 1000 threads = 10 * 10 * 10
        "void cs_main(uint3 threadID : SV_GroupThreadID, uint3 groupID : SV_GroupID, uint threadIndex : SV_GroupIndex) \n"
        "{ \n"
        "  uint3 dispatchSize = {%#x,%#x,%#x}; \n" // insert (x,y,z) dispatch dimensions here
        "  uint numThreadsInGroup = %#x; \n" // insert num threads in group here
        "  uint numTotalThreads = numThreadsInGroup * dispatchSize.x * dispatchSize.y * dispatchSize.z; \n"
        "  uint groupIndex = groupID.z*dispatchSize.x*dispatchSize.y + groupID.y*dispatchSize.x + groupID.x; \n"
        "  uint globalThreadIndex = %s;  \n"
        "  uint tgsmClearVal = %#x; \n"
        "  uint origValue = 0xdeadbeef; \n"
		"  uint OpSwitchID = %s ; \n"
		"  uint MemSwitchID = %s ; \n"
        // initialize all shared memory to clearVal
        "  if (%s) \n" // insert clear condition here (ie thread index == 0)
        "  { \n"
        "    sharedMemory[groupIndex] = tgsmClearVal; \n"
        "  } \n"
        "  GroupMemoryBarrierWithGroupSync(); \n"
        "  if( OpSwitchID <= 3 ) \n"
        "  { \n"
        // Unused bits
        "  } \n"
        "  else if( OpSwitchID <= 7 ) \n"
        "  { \n"
        "  	InterlockedAdd( sharedMemory[MemSwitchID], int(-16), origValue ); \n" // insert destination
        "  } \n"
        "  else if( OpSwitchID <= 11 ) \n"
        "  { \n"
        "  	InterlockedExchange( sharedMemory[MemSwitchID], ( (1 << OpSwitchID) + tgsmClearVal ), origValue ); \n" // insert destination
        "  } \n"
        "  else if( OpSwitchID <= 14 ) \n"
        "  { \n"
        // Unused bits
        "  } \n"
        "  else if( OpSwitchID <= 17 ) \n"
        "  { \n"
        "  	InterlockedAnd( sharedMemory[MemSwitchID], ( ~(1 << OpSwitchID)), origValue ); \n" // insert destination
        "  } \n"
        "  else if( OpSwitchID <= 20 ) \n"
        "  { \n"
        "  	InterlockedOr( sharedMemory[MemSwitchID], (1 << OpSwitchID), origValue ); \n" // insert destination
        "  } \n"
        "  else if( OpSwitchID <= 23 ) \n"
        "  { \n"
        "  	InterlockedXor( sharedMemory[MemSwitchID], (1 << OpSwitchID), origValue ); \n" // insert destination
        "  } \n"
        "  else if( OpSwitchID <= 27 ) \n"
        "  { \n"
        "  	InterlockedAdd( sharedMemory[MemSwitchID], +(1 << 24), origValue ); \n" // insert destination
        "  } \n"
        "  else if( OpSwitchID <= 29 ) \n"
        "  { \n"
        // Unused bits
        "  } \n"
        "  else if( OpSwitchID == 30 ) \n"
        "  { \n"
        "    InterlockedMax( sharedMemory[MemSwitchID], ( (3 << 30) | tgsmClearVal ), origValue ); \n" // insert destination
        "  } \n"
        "  else if( OpSwitchID == 31 ) \n"
        "  { \n"
        "    InterlockedMin( sharedMemory[MemSwitchID], ( (~(3 << 30)) & tgsmClearVal ), origValue ); \n" // insert destination
        "  } \n"
        "  else \n"
        "  { \n"
        // Unused bits
        "  } \n"
        "  if( (OpSwitchID > 3 && OpSwitchID <= 11)  || \n"
        "      (OpSwitchID > 14 && OpSwitchID <= 27) || \n"
        "      (OpSwitchID > 29 && OpSwitchID <= 31) ) \n"
        "  { \n"
        "    targetUAV[globalThreadIndex] = origValue; \n"
        "  } \n"
        "} \n"
        ;

    const UINT tx = m_NumThreads[0];
    const UINT ty = m_NumThreads[1];
    const UINT tz = m_NumThreads[2];
    const UINT numThreadsInGroup = GetNumberOfThreadsPerGroup();

    UINT gx, gy, gz;
    GetThreadGroupCount( &gx, &gy, &gz );

    //
    // Pick either thread-group shared memory, or a global UAV
    //

    tstring sharedMemDecl = "groupshared static uint sharedMemory[2048]";
    tstring clearCondition = "true";
    // If we are using a UAV for our shared memory, then we don't need to clear it in the HLSL because we clear it using the device context at bind time.
    if (SHARED_MEMORY_UAV == m_SharedMemType)
    {
        sharedMemDecl = "RWBuffer<uint> sharedMemory";
        clearCondition = "false";
    }

	tstring opSwitchIDStr = _T("threadIndex");
	tstring memSwitchIDStr = _T("groupIndex");
	tstring globalThreadIDStr = _T("groupIndex * numThreadsInGroup + threadIndex");
	if ( m_bInterGroup )
	{
		opSwitchIDStr = _T("groupIndex");
		memSwitchIDStr = _T("threadIndex");
		globalThreadIDStr = _T("threadIndex * 32 + groupIndex");
	}

    //
    // Format the shader code
    //

    const tstring shaderCode = FormatString( szShader_GroupSharedMem, 
        sharedMemDecl.c_str(), tx,ty,tz, gx,gy,gz, numThreadsInGroup, globalThreadIDStr.c_str(), m_sharedUAVClearVal, 
		opSwitchIDStr.c_str(), memSwitchIDStr.c_str(), clearCondition.c_str() );

    return shaderCode;
}

TEST_RESULT CAtomicMultipleIntrinsicTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    // Local varaibles
    TEST_RESULT tRes = RESULT_PASS;
    UINT *pResults = (UINT*)pBufferData->pData;

    std::vector<UINT> threadResultsToIgnore;
    bool foundOriginalClearValue = false;
    bool foundMatchingValues = false;
    UINT numOriginalClearValues = 0;
    UINT numMatchingValues = 0;

    // Setup which thread's results need to be ignored
    threadResultsToIgnore.push_back(0);
    threadResultsToIgnore.push_back(1);
    threadResultsToIgnore.push_back(2);
    threadResultsToIgnore.push_back(3);
    threadResultsToIgnore.push_back(12);
    threadResultsToIgnore.push_back(13);
    threadResultsToIgnore.push_back(14);
    threadResultsToIgnore.push_back(28);
    threadResultsToIgnore.push_back(29);

    // Go through each group
    UINT numValueGroups = GetNumberOfGroupsDispatched();
	UINT numValuesInGroup = 32;
	if ( m_bInterGroup )
	{
		numValueGroups = GetNumberOfThreadsPerGroup();
	}
    
    for( UINT groupIndex = 0; groupIndex < numValueGroups; ++groupIndex )
    {
        // Reset some flags per group
        foundOriginalClearValue = false;
        foundMatchingValues = false;
        numOriginalClearValues = 0;
        numMatchingValues = 0;

        // Each group should have 32 values associated with it
        // Each representing the "original" values before a atomic operation was executed
        for( UINT threadIndex = 0; threadIndex < numValuesInGroup; ++threadIndex )
        {
            bool ignoreCurrentResult = false;

            // Check to see if we need to ignore the current thread's result
            for(UINT i = 0; i < threadResultsToIgnore.size(); ++i)
                if( threadResultsToIgnore[i] == threadIndex )
                    ignoreCurrentResult = true;

            if( ignoreCurrentResult )
                continue;

            if( pResults[threadIndex] == m_sharedUAVClearVal )
            {
                foundOriginalClearValue = true;
                numOriginalClearValues++;
            }

            // Check the current value against all other values in the array to verify there are no duplicates
            for( UINT index = threadIndex + 1; index < numValuesInGroup; ++index )
            {
                ignoreCurrentResult = false;

                // Check to see if we need to ignore the current thread's result
                for(UINT i = 0; i < threadResultsToIgnore.size(); ++i)
                    if( threadResultsToIgnore[i] == index )
                        ignoreCurrentResult = true;

                if( ignoreCurrentResult )
                    continue;

                if( pResults[threadIndex] == pResults[index] )
                {
                    foundMatchingValues = true;
                    numMatchingValues++;
                    WriteToLog("Found two values in the result UAV that match. UAV[%d][%d] = %#x, UAV[%d][%d] = %#x.", groupIndex, threadIndex, pResults[threadIndex], groupIndex, index, pResults[index] );
                    break;
                }
            }
        }

        //
        // Check the results from the current group
        //

        // Check to make sure we didnt find any duplicate values
        if( foundMatchingValues )
        {
            tRes = RESULT_FAIL;
            WriteToLog("GroupID = %d. Found values in the Target UAV which matched. This means at least two instuctions didn't operate atomically because they each returned the same original value. Found %d instances.", groupIndex, numMatchingValues);
        }

        // Check to make sure we found one, and only one, instance of the initial cleared value
        if( foundOriginalClearValue )
        {
            if( numOriginalClearValues > 1 )
            {
                tRes = RESULT_FAIL;
                WriteToLog("GroupID = %d. Found more than 1 instance of the original clear value (for shared memory). Original clear value = %#x. Found %d instances.", groupIndex, m_sharedUAVClearVal, numOriginalClearValues);
            }
        }
        else
        {
            tRes = RESULT_FAIL;
            WriteToLog("GroupID = %d. Did NOT find any instances of the original clear value (for shared memory). Original clear value = %#x.", groupIndex, m_sharedUAVClearVal);
        }
        
        // Increment the pointer to the next set of group data
        pResults += numValuesInGroup;

        // If there were errors in this group, exit to avoid creating a massive log
        if( RESULT_FAIL == tRes )
            break;
    }

    return tRes;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//	Below tests are Atomic operations tests for Pixel Shader. They were copied from the 
//	versions of CS tests and modified for PS. Similarities between two are mostly contained to
//	GetShaderCode and VerifyResults functions, but there are some differences in those:
//	1. There is no group shared memory in PS
//	2. There is no syncing in PS 
//

const char g_VSPassthrough[] = 
"float4 main( float4 input : POSITION ): SV_POSITION \n"
"{\n"
"	return input; \n"
"}\n"
;



TEST_RESULT CAtomicityPSTest::Setup()
{
    TEST_RESULT tr = CComputeTest::Setup();
    if ( tr != RESULT_PASS)
    {
        WriteToLog(_T("CComputeTest::Setup() failed"));
        return tr;
    }

    if (m_pDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
    {
        WriteToLog(_T("Skip this group on feature levels less than 11_0."));
        return RESULT_SKIP;
    }

    tr = CComputeTest::SetupVSStage();
    if ( tr != RESULT_PASS)
    {
        WriteToLog(_T("CAtomicityTest::SetupVSStage() failed"));
        return tr;
    }

    return RESULT_PASS;
}


//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CAtomicityPSTest::SetupTestCase()
{
    HRESULT hr;
    TEST_RESULT tRes = CComputeTest::SetupTestCase();
    if (tRes != RESULT_PASS)
    {
        WriteToLog( "CAtomicityPSTest::CComputeTest::SetupTestCase did not succeed" );
        return tRes;
    }

    if ( SkipSlowOnRefWithMessage( m_ViewWidth * m_ViewHeight  > 64 * 64  ) )
        return RESULT_SKIP;

    if ( CreateUAVs() != RESULT_PASS )
    {
        WriteToLog(_T("CAtomicityPSTest::SetupTestCase: SetupUAVs failed "));
        return RESULT_FAIL;
    }

    if ( SetupPixelShader() != RESULT_PASS )
    {
        WriteToLog(_T("CAtomicityPSTest::SetupTestCase: SetupPixelShader failed "));
        return RESULT_FAIL;
    }

	// create a dummy rendertarget
	if ( CComputeTest::SetupRTV(m_ViewWidth, m_ViewHeight) != RESULT_PASS )
	{
		WriteToLog(_T("CAtomicityPSTest::SetupTestCase: CComputeTest::SetupRTV() failed "));
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


    // Set vertex buffer
    UINT stride = sizeof( float ) * 3;
    UINT offset = 0;
    GetEffectiveContext()->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
    GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    GetEffectiveContext()->IASetInputLayout( m_pInputLayout );

    // Set shaders
    GetEffectiveContext()->VSSetShader( m_pVertexShader, NULL, 0 );
    GetEffectiveContext()->PSSetShader( m_pPixelShader, NULL, 0 );

    // Set UAVs: 2 UAVs; the first one is the result array the last one is the final UAV
    GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 
		1, &m_pRTView, 
        NULL,
        1, 
        2,
        m_ppUAVs, 
        NULL);

    return RESULT_PASS;
}


//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CAtomicityPSTest::CreateUAVs()
{
#define CREATERESOURCE(descStruct, pResource, funcName) \
    hr = m_pDevice->funcName(&descStruct, NULL, &(pResource)); \
    if (FAILED( hr )) \
    { \
    WriteToLog(_T("CAtomicityPSTest::CreateUAVs(): "#funcName" failed for "#pResource". hr = %s"), \
    D3DHResultToString(hr).c_str() ); \
    return RESULT_FAIL; \
}

#define CREATERESOURCETILEDMAPOPT(descStruct, pResource, funcName) \
    hr = GetFramework()->##funcName##TiledOptionalMapDefaultOptional(&descStruct, NULL, &(pResource)); \
    if (FAILED( hr )) \
    { \
    WriteToLog(_T("CAtomicityPSTest::CreateUAVs(): "#funcName" failed for "#pResource". hr = %s"), \
                D3DHResultToString(hr).c_str() ); \
        return RESULT_FAIL; \
    }

#define CREATEUAV(pResource, descStruct, pUAView) \
    hr = m_pDevice->CreateUnorderedAccessView(pResource, &descStruct, &(pUAView) ); \
    if (FAILED( hr )) \
    { \
    WriteToLog(_T("CAtomicityPSTest::CreateUAVs(): CreateUnorderedAccessView failed for "#pUAView". hr = %s"), \
    D3DHResultToString(hr).c_str() ); \
    return RESULT_FAIL; \
}

    HRESULT hr;
    D3D11_BUFFER_DESC buffDesc;
    // create output UAV resource
    buffDesc.ByteWidth = sizeof(UINT) * m_BufferSize; 
    buffDesc.Usage = D3D11_USAGE_STAGING;
    buffDesc.BindFlags = 0;
    buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    buffDesc.MiscFlags = 0; 
    buffDesc.StructureByteStride = 0;
    CREATERESOURCETILEDMAPOPT(buffDesc, m_pUAVBufCopy, CreateBuffer);

    buffDesc.Usage = D3D11_USAGE_DEFAULT;
    buffDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    buffDesc.CPUAccessFlags = 0;

    if ( m_TypeUAV == _T("RawBuffer")
        || m_TypeUAV == _T("TypedBuffer") 
        || m_TypeUAV == _T("StructuredBuffer") )
    {
        if ( m_TypeUAV == _T("RawBuffer") )
        {
            buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
            buffDesc.StructureByteStride = 0;
        }
        else if ( m_TypeUAV == _T("TypedBuffer") )
        {
            buffDesc.MiscFlags = 0;
            buffDesc.StructureByteStride = 0;
        }
        else if ( m_TypeUAV == _T("StructuredBuffer") )
        {
            buffDesc.MiscFlags = 0;
            buffDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            buffDesc.StructureByteStride = 4;
        }
        CREATERESOURCETILEDMAPOPT(buffDesc, m_pUAVBuf, CreateBuffer);
    }

    // Create UAVs on the above created resources
    D3D11_BUFFER_UAV BufUAV = {0, m_BufferSize, 0};
    D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
    UAVDesc.Format = DXGI_FORMAT_R32_UINT;
    UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER ;
    UAVDesc.Buffer = BufUAV;
    UAVDesc.Format = DXGI_FORMAT_R32_UINT;

    if ( m_TypeUAV == _T("RawBuffer")
        || m_TypeUAV == _T("TypedBuffer") 
        || m_TypeUAV == _T("StructuredBuffer") )
    {
        UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        UAVDesc.Buffer = BufUAV;
        if ( m_TypeUAV == _T("RawBuffer") )
        {
            UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
            UAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        }
        else if (m_TypeUAV == _T("StructuredBuffer"))
        {
            UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        }

        CREATEUAV(m_pUAVBuf, UAVDesc, m_ppUAVs[0]);
    }

    if (NULL == m_pTargetUAV)
    {
        return RESULT_FAIL;
    }
    else
    {
        // Result UAV
        m_ppUAVs[1] = m_pTargetUAV;

        // Clear UAVs
        UINT targetClearVal = 0;
        UINT targetClearVals[4] = { targetClearVal, targetClearVal, targetClearVal, targetClearVal };
        m_pEffectiveContext->ClearUnorderedAccessViewUint( m_pTargetUAV, targetClearVals );

        UINT bufferClearVal = GetTargetStartVal();
        UINT bufferClearVals[4] = {bufferClearVal, bufferClearVal, bufferClearVal, bufferClearVal};
        m_pEffectiveContext->ClearUnorderedAccessViewUint( m_ppUAVs[0], bufferClearVals );
    }

    return RESULT_PASS;

}

TEST_RESULT CAtomicityPSTest::SetupPixelShader()
{
    TEST_RESULT tr= RESULT_PASS;
    ID3D10Blob *pShaderBuf = NULL;
    ID3D10Blob *pErrorBuf = NULL;
    ID3D10Blob* pShaderText = NULL;
    HRESULT hr;


    // compile our shader
    D3D10_SHADER_MACRO defines[MAX_DEFINES];
    ZeroStructMem(defines);
    tstring shaderCode = GetShaderCode(defines);

    hr = g_TestApp.CompileShaderFromMemory(shaderCode.c_str(), 
                                           shaderCode.size(), 
                                           "ps_main", 
                                           D3D_SHADER_STAGE_PIXEL, 
                                           D3D_FEATURE_LEVEL_11_0,
                                           D3D_FEATURE_LEVEL_11_0,
                                           0,
                                           &pShaderBuf,
                                           &pErrorBuf,
                                           defines,
                                           &pShaderText);

    if ( FAILED(hr) || g_TestApp.m_bLogShaderText)
    {
        tr = RESULT_FAIL;
        WriteToLog( _T( "Compiler errors:"));
        if (pErrorBuf)
        {
            LPVOID pBuf = pErrorBuf->GetBufferPointer();
            size_t bufSize = pErrorBuf->GetBufferSize();
            char* szError = new char[bufSize+1];
            memcpy(szError, pBuf, bufSize);
            szError[bufSize] = 0;
            WriteToLog( _T( "%s\n" ), szError );
            delete[] szError;
        }
        goto shader_done;
    }

    hr = m_pDevice->CreatePixelShader( pShaderBuf->GetBufferPointer( ), pShaderBuf->GetBufferSize(), NULL, &m_pPixelShader ); 
    if( FAILED( hr ) )
    {
        WriteToLog( _T( "CAtomicityPSTest::SetupPixelShader(): CreatePixelShader() failed. hr=%s\n" ), D3DHResultToString(hr).c_str() );
        tr = RESULT_FAIL;
        goto shader_done;
    }

shader_done:
    SAFE_RELEASE(pShaderBuf);
    SAFE_RELEASE(pErrorBuf);
    return tr;
}


//-------------------------------------------------------------------------------------------------------------
TEST_RESULT CAtomicityPSTest::ExecuteTestCase()
{
    HRESULT hr;
    TEST_RESULT tr = RESULT_PASS;

    m_pEffectiveContext->Draw( 6, 0 );
    g_TestApp.ExecuteEffectiveContext();

    D3D11_MAPPED_SUBRESOURCE mappedRes;
    ID3D11Resource* pResource = NULL, * pResourceCopy = NULL;

    if(m_bUseTargetUAV)
    {
        pResource = m_pTargetResource;
        pResourceCopy = m_pTargetResourceCopy;
    }
    else
    {
        pResource = m_pUAVBuf;
        pResourceCopy = m_pUAVBufCopy;
    }

    GetImmediateContext()->CopyResource( pResourceCopy, pResource );
    hr = GetImmediateContext()->Map( pResourceCopy, 0, D3D11_MAP_READ, 0, &mappedRes );
    if( FAILED( hr ) )
    {
        WriteToLog( _T( "CAtomicityPSTest::ExecuteTestCase() - Failed to map resource, hr=%s" ), D3DHResultToString( hr ).c_str() );
        return RESULT_FAIL;
    }
    else
    {
        tr = VerifyResult(&mappedRes);
        GetImmediateContext()->Unmap( pResourceCopy, 0 );
    }

    return tr;
}

UINT CAtomicityPSTest::GetPixelCount()
{
    return m_ViewWidth * m_ViewHeight;
}

//-------------------------------------------------------------------------------------------------------------
void CAtomicityPSTest::CleanupTestCase()
{
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_ppUAVs[0]);
    SAFE_RELEASE(m_pUAVBuf);
    SAFE_RELEASE(m_pUAVBufCopy);
	SAFE_RELEASE(m_pRTTex2D);
	SAFE_RELEASE(m_pRTView);
    CComputeTest::CleanupTestCase();
}

//-------------------------------------------------------------------------------------------------------------
void CAtomicityPSTest::Cleanup()
{
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pVertexBuffer);

}

//------------------------------------------------------------------------------------------------
// CAtomicSingleIntrinsicPSTest
//------------------------------------------------------------------------------------------------
void CAtomicSingleIntrinsicPSTest::InitTestParameters()
{
    const tstring intrinsicName = GetIntrinsicName();

    // Won't be using TargetUAV for this group of tests, will be mapping test buffer directly
    m_bUseTargetUAV = false;
    m_TargetUAVSize[0] = 1;
    m_TargetUAVSize[1] = 1;
    m_TargetUAVSize[2] = 1;

    m_TargetUAVDimension = D3D11_UAV_DIMENSION_BUFFER;

    m_BufferSize = 2048;

    // viewport width and height, working as the X and Y dimensions of a thread group in Compute
    CUserValueSet<UINT> *pWidth = new CUserValueSet<UINT>;
    pWidth->AddValue(31);
    pWidth->AddValue(16);
    testfactor::RFactor rfViewWidth = AddParameter<UINT>(_T("ViewWidth"), &m_ViewWidth, pWidth);

    CUserValueSet<UINT> *pHeight = new CUserValueSet<UINT>;
    pHeight->AddValue(31);
    pHeight->AddValue(16);
    testfactor::RFactor rfViewHeight = AddParameter<UINT>(_T("ViewHeight"), &m_ViewHeight, pHeight);

    CUserValueSet<tstring> *pUAVType = new CUserValueSet<tstring>;
    pUAVType->AddValue(_T("RawBuffer"));
    testfactor::RFactor rfTypeUAV = AddParameter<tstring>(_T("UAV"), &m_TypeUAV, pUAVType);

    CTestCaseParameter<bool>* pSubsetOpsParam = AddParameter<bool>( _T("SubsetOperations"), &m_bSubsetOperations );
    testfactor::RFactor rSubsetOps_F = AddParameterValue<bool>( pSubsetOpsParam, false );
    testfactor::RFactor rSubsetOps_T = AddParameterValue<bool>( pSubsetOpsParam, true );
    testfactor::RFactor rSubsetOps_FT = rSubsetOps_F + rSubsetOps_T;

    CTestCaseParameter<bool>* pOriginalValParam = AddParameter<bool>( _T("OriginalValueVariant"), &m_bOriginalValueVariant );
    testfactor::RFactor rOriginalVal_F = AddParameterValue<bool>( pOriginalValParam, false );
    testfactor::RFactor rOriginalVal_T = AddParameterValue<bool>( pOriginalValParam, true );
    testfactor::RFactor rOriginalVal_FT = rOriginalVal_F + rOriginalVal_T;

    testfactor::RFactor root = (rfViewWidth * rfViewHeight) * rfTypeUAV * rSubsetOps_FT * rOriginalVal_FT;
    SetRootTestFactor( root );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ViewWidth"), 31), WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );

}

tstring CAtomicSingleIntrinsicPSTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    static const char szShader_PSSingleIntrinsic[] =
        "RWByteAddressBuffer buffer; \n"
        "RWBuffer<uint> targetUAV; \n"
		"float ps_main(float4 Pos : SV_POSITION) : SV_TARGET \n"
        "{ \n"
        "  uint addressMask = 0xffffffff; \n"
        "  uint PID = (Pos.y - 0.5) * %d + (Pos.x - 0.5); \n" // insert viewport width
        "  uint valToUse = %s; \n" // insert value string here
		"  [fastopt]"
        "  for (uint index = 0; index < 2048; ++index) \n"
        "  { \n"
        "    uint originalVal = 0; \n"
        "    %s \n" // insert condition for doing atomic operation
        "    { \n"
        "		  buffer.Interlocked%s((index & addressMask)*4, \n" // insert intrinsic name here
        "								valToUse %s ); \n" // insert original value optional argument here
        "    } \n"
        "  } \n"
		"  return 1.0; \n"  //dummy output to the dummy rendertarget
        "} \n"
        ;

    tstring subsetOpsString = "";
    if (m_bSubsetOperations)
    {
        subsetOpsString += "if ((PID % 4) == 2) addressMask = 0xfffffffc; \n    "; // non-multiple-of-4 even threads write to 4-element aligned locations
        subsetOpsString += "if ((PID % 2) == 0)"; // skip all odd thread index values
    }
    tstring intrinsicName = GetIntrinsicName();
    tstring optionalOriginalValArg = "";
    if (m_bOriginalValueVariant)
        optionalOriginalValArg = ", originalVal";
    tstring shaderValueString = GetShaderValueString();
    UINT clearVal = GetTargetStartVal();

    tstring shaderCode = FormatString( szShader_PSSingleIntrinsic, m_ViewWidth, shaderValueString.c_str(),
        subsetOpsString.c_str(), intrinsicName.c_str(), optionalOriginalValArg.c_str() );

    return shaderCode;
}

//------------------------------------------------------------------------------------------------
// CAtomicAddPSTest
//------------------------------------------------------------------------------------------------
UINT CAtomicAddPSTest::GetTargetStartVal()
{
    return 0;
}

tstring CAtomicAddPSTest::GetShaderValueString()
{
    tstring retVal = "(1 + (PID % 4))";
    return retVal;
}

TEST_RESULT CAtomicAddPSTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;

    UINT numPixels = GetPixelCount();

    // under normal operation, all pixels add values to same location
    // location += (PID % 4) + 1;
    // every 4 pixels add 10, additional past multiple of 4 are +1, +2, +3
    UINT expectedVal = (numPixels / 4) * 10;
    for (UINT n = 0; n < numPixels % 4; ++n)
        expectedVal += 1 + (n % 4);

    // with subset operations, things are more complicated
    // odd threads write nothing (only values 1 and 3 are written per 4 threads)
    // multiples-of-4 write as normal
    // non-multiples-of-4 write only to 4-element-aligned locations
    // this means you'll see this pattern in the output buffer:
    //   1, 1, 1, 1,  1, ... (1 group, 1-2 threads each)
    //   13,1, 1, 1, 13, ... (1 group, 3-4 threads each)
    //   14,2, 2, 2, 14, ... (1 group, 5-6 threads each)
    //   26,2, 2, 2, 26, ... (1 group, 7-8 threads each)
    //   27,3, 3, 3, 27, ... (1 group, 9-10 threads each)
    UINT expectedValEvery4 = expectedVal;
    if (m_bSubsetOperations)
    {
        expectedVal = (numPixels / 4) * 1;
        expectedValEvery4 = expectedVal * 13;
        if ((numPixels % 4) > 0)
        {
            expectedVal += 1;
            expectedValEvery4 += 1;
        }
        if ((numPixels % 4) > 2)
            expectedValEvery4 += 3 * 4;
    }

    UINT32* pData = (UINT32*)pBufferData->pData;
    for (UINT n = 0; n < m_BufferSize; ++n)
    {
        UINT specificExpected = (n % 4 == 0) ? expectedValEvery4 : expectedVal;
        if (pData[n] != specificExpected)
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Unexpected result at buffer[%d], Expected %d, Found %d", n, specificExpected, pData[n] );
            break;
        }
    }

    return tRes;
}


//------------------------------------------------------------------------------------------------
// CAtomicMinMaxTest
//------------------------------------------------------------------------------------------------
UINT CAtomicMinMaxPSTest::GetTargetStartVal()
{
    if (GetIntrinsicName() == "Max")
    {
        return 0;
    }
    else
    {
        return 1000;
    }
}

tstring CAtomicMinMaxPSTest::GetShaderValueString()
{
    tstring retVal = "((1 + PID) * 509) % 139";
    return retVal;
}

UINT CAtomicMinMaxPSTest::GetPixelIterationValue(UINT PID)
{
    UINT retVal = ((1 + PID) * 509) % 139;
    return retVal;
}

UINT CAtomicMinMaxPSTest::ModifyExpectedValue(UINT currVal, UINT pixelVal, const tstring& intrinsicName)
{
    if (intrinsicName == "Max")
        currVal = max(currVal, pixelVal);
    else 
        currVal = min(currVal, pixelVal);

    return currVal;
}

TEST_RESULT CAtomicMinMaxPSTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;

    UINT numPixels = GetPixelCount();

    const tstring intrinsicName = GetIntrinsicName();

    UINT expectedVal = GetTargetStartVal();
    UINT expectedValEvery4 = expectedVal;

    if (!m_bSubsetOperations)
    {
        // under normal operation, all threads add values to same location
        for (UINT n = 0; n < numPixels; ++n)
        {
            UINT pixelVal = GetPixelIterationValue(n);
            expectedVal = ModifyExpectedValue(expectedVal, pixelVal, intrinsicName);
        }
        expectedValEvery4 = expectedVal;
    }
    else
    {
        // with subset operations, things are more complicated
        // odd threads write nothing (only values [0] and [2] are written per 4 threads)
        // multiples-of-4 write as normal
        // non-multiples-of-4 write only to 4-element-aligned locations
        // so all even threads write to the 4-element-aligned locations, 
        // but only multiples of 4 write to all memory locations
        for (UINT n = 0; n < numPixels; ++n)
        {
            if (n % 2 == 1)
                continue;
            UINT pixelVal = GetPixelIterationValue(n);
            if (n % 4 == 0)
            {
                expectedVal = ModifyExpectedValue(expectedVal, pixelVal, intrinsicName);
            }
            expectedValEvery4 = ModifyExpectedValue(expectedValEvery4, pixelVal, intrinsicName);
        }
    }


    UINT32* pData = (UINT32*)pBufferData->pData;
    for (UINT n = 0; n < m_BufferSize; ++n)
    {
        UINT specificExpected = (n % 4 == 0) ? expectedValEvery4 : expectedVal;
        if (pData[n] != specificExpected)
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Unexpected result at buffer[%d], Expected %d, Found %d", n, specificExpected, pData[n] );
            break;
        }
    }

    return tRes;
}

//------------------------------------------------------------------------------------------------
// CAtomicBitwisePSTest
//------------------------------------------------------------------------------------------------
UINT CAtomicBitwisePSTest::GetTargetStartVal()
{
    if (GetIntrinsicName() == "And")
        return 0xffffffff;
    else
        return 0x10000008;
}

tstring CAtomicBitwisePSTest::GetShaderValueString()
{
    if (GetIntrinsicName() == "And")
        return "~((uint)1 << (PID % 32))";
    else
        return "((uint)1 << (PID % 32))";
}

UINT CAtomicBitwisePSTest::GetPixelIterationValue(UINT PID, const tstring& intrinsicName)
{
    if (intrinsicName == "And")
        return ~((UINT)1 << (PID % 32));
    else
        return ((UINT)1 << (PID % 32));
}

UINT CAtomicBitwisePSTest::ModifyExpectedValue(UINT currVal, UINT pixelVal, const tstring& intrinsicName)
{
    if (intrinsicName == "And")
        currVal &= pixelVal;
    else if (intrinsicName == "Or")
        currVal |= pixelVal;
    else if (intrinsicName == "Xor")
        currVal ^= pixelVal;
    return currVal;
}

TEST_RESULT CAtomicBitwisePSTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;

    UINT numPixels = GetPixelCount();

    const tstring intrinsicName = GetIntrinsicName();

    UINT expectedVal = GetTargetStartVal();
    UINT expectedValEvery4 = expectedVal;

    if (!m_bSubsetOperations)
    {
        // under normal operation, all threads add values to same location
        for (UINT n = 0; n < numPixels; ++n)
        {
            UINT pixelVal = GetPixelIterationValue(n, intrinsicName);
            expectedVal = ModifyExpectedValue(expectedVal, pixelVal, intrinsicName);
        }
        expectedValEvery4 = expectedVal;
    }
    else
    {
        // with subset operations, things are more complicated
        // odd threads write nothing (only values 1 and 3 are written per 4 threads)
        // multiples-of-4 write as normal
        // non-multiples-of-4 write only to 4-element-aligned locations
        // so all even threads write to the 4-element-aligned locations, 
        // but only multiples of 4 write to all memory locations
        for (UINT n = 0; n < numPixels; ++n)
        {
            if (n % 2 == 1)
                continue;
            UINT pixelVal = GetPixelIterationValue(n, intrinsicName);
            if (n % 4 == 0)
            {
                expectedVal = ModifyExpectedValue(expectedVal, pixelVal, intrinsicName);
            }
            expectedValEvery4 = ModifyExpectedValue(expectedValEvery4, pixelVal, intrinsicName);
            if (n % 4 == 2 && intrinsicName == "Xor")
            {
                for (int k = 0; k < 3; ++k)
                {
                    expectedValEvery4 = ModifyExpectedValue(expectedValEvery4, pixelVal, intrinsicName);
                }
            }
        }
    }


    UINT32* pData = (UINT32*)pBufferData->pData;
    for (UINT n = 0; n < m_BufferSize; ++n)
    {
        UINT specificExpected = (n % 4 == 0) ? expectedValEvery4 : expectedVal;
        if (pData[n] != specificExpected)
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Unexpected result at buffer[%d], Expected %#x, Found %#x", n, specificExpected, pData[n] );
            break;
        }
    }

    return tRes;
}


//------------------------------------------------------------------------------------------------
// CAtomicStorePSTest
//------------------------------------------------------------------------------------------------
void CAtomicStorePSTest::InitTestParameters()
{
    const tstring intrinsicName = GetIntrinsicName();
    m_UAVClearVal = 0xffffffff;

    
    // Target UAV will store original values
    m_TargetUAVSize[0] = 2048; 
    m_TargetUAVSize[1] = 100;	// width * height
    m_TargetUAVSize[2] = 1;

    m_TargetUAVDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

    m_BufferSize = 2048;

    // viewport width and height, working as the X and Y dimensions of a thread group in Compute
    CUserValueSet<UINT> *pWidth = new CUserValueSet<UINT>;
    pWidth->AddValue(10);
    pWidth->AddValue(100);
    pWidth->AddValue(5);
    testfactor::RFactor rfViewWidth = AddParameter<UINT>(_T("ViewWidth"), &m_ViewWidth, pWidth);

    CUserValueSet<UINT> *pHeight = new CUserValueSet<UINT>;
    pHeight->AddValue(10);
    pHeight->AddValue(1);
    pHeight->AddValue(20);
    testfactor::RFactor rfViewHeight = AddParameter<UINT>(_T("ViewHeight"), &m_ViewHeight, pHeight);

    CUserValueSet<tstring> *pUAVType = new CUserValueSet<tstring>;
    pUAVType->AddValue(_T("RawBuffer"));
    testfactor::RFactor rfTypeUAV = AddParameter<tstring>(_T("UAV"), &m_TypeUAV, pUAVType);


    testfactor::RFactor root = (rfViewWidth % rfViewHeight) * rfTypeUAV;
    SetRootTestFactor(root);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ViewWidth"), 5), WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );

}

tstring CAtomicStorePSTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    defines[0].Name = "INNER_SIZE";
    defines[0].Definition = "2";

    static const char szShader_PSAtomicStore[] =
        "RWByteAddressBuffer buffer; \n"
        "RWTexture2D<uint> targetUAV; \n"
		"float ps_main(float4 Pos : SV_POSITION): SV_TARGET \n"
        "{ \n"
        "  uint numPixels = %#x; \n" // insert num of pixels here
        "  uint PID = (Pos.y - 0.5) * %d + (Pos.x - 0.5); \n" // insert viewport width
        "  uint clearVal = %#x; \n" // insert clearVal here
        "  uint index = 0; \n"
        "  bool doOperation = true; \n"
		"  [fastopt]"
        "  for (uint outer = 0; outer < (2048/INNER_SIZE); ++outer) \n"
        "  { \n"
        "    uint originalVals[INNER_SIZE]; \n"
        "    uint inner; \n"
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      originalVals[inner] = 0xbaadf00d; \n"
        "    } \n"
        "    [unroll] \n"
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      if (doOperation) \n"
        "      { \n"
        "        buffer.Interlocked%s(index*4, \n" // insert intrinsic name here
        "            %s  \n" // insert compare value optional argument here
        "            PID %s ); \n" // insert original value optional argument here
        "      } \n"
        "      %s \n" // insert optional modification of 'doOperation' here, for Compare*
        "      ++index; \n"
        "    } \n"
        ;

    static const char szShader_WriteOriginalValue[] =
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      targetUAV[uint2(outer*INNER_SIZE + inner, PID)] = originalVals[inner]; \n"
        "    } \n"
        "  } \n"
        ;

    static const char szShader_FinishFunc[] =
		"    return 1.0; \n"  //dummy output to the dummy rendertarget
        "} \n"
        ;

    const tstring intrinsicName = GetIntrinsicName();

    // if we do an exchange, we need to record the exchanged value
    tstring optionalOriginalValArg = "";
    if (tstring::npos != intrinsicName.find("Exchange"))
        optionalOriginalValArg = ", originalVals[inner]";

    // if we're doing a compare, we need to write the compare value (clearVal)
    tstring optionalCompareValArg = "";
    tstring optionalDoOperation = "";
    if (tstring::npos != intrinsicName.find("Compare"))
    {
        // with a compare, only one thread can win, so we disqualify this thread from winning in subsequent iterations
        optionalCompareValArg = "clearVal, ";
        optionalDoOperation = "if (PID == buffer.Load(index*4)) { doOperation = false; ";
        if (tstring::npos == intrinsicName.find("Exchange"))
        {
            // if we don't have an exchange, we still need to modify the "originalval" that gets written to target
            optionalDoOperation += "originalVals[inner] = clearVal; ";
        }
        optionalDoOperation += "}\n";
        // reset the 'doOperation' value every N iterations, where N = num threads that can see the shared memory
        const char* szPixelCount = "numPixels";
        optionalDoOperation += FormatString("if ((index %% %s) == (%s - 1)) {doOperation = true;}", szPixelCount, szPixelCount);
    }

    tstring shaderCode = FormatString( szShader_PSAtomicStore, 
        GetPixelCount(), m_ViewWidth, m_UAVClearVal, intrinsicName.c_str(), optionalCompareValArg.c_str(), optionalOriginalValArg.c_str(),
        optionalDoOperation.c_str() );

    shaderCode += szShader_WriteOriginalValue;
    shaderCode += szShader_FinishFunc;

	if( g_TestApp.m_bLogShaderText )
		WriteToLog( "Shader Code =%s", shaderCode.c_str() );

    return shaderCode;
}

TEST_RESULT CAtomicStorePSTest::VerifyWinner(WinnerInfo winner)
{
    TEST_RESULT tRes = RESULT_FAIL;

    if(winner.Count == 1)
    {
        tRes = RESULT_PASS;
    }
    else
    {
        if(winner.Count == 0)
        {
            WriteToLog("FAIL: No winning thread.");
        }

        if(winner.Count > 1)
        {
            WriteToLog("FAIL: More than one winning thread.");
        }

        tRes = RESULT_FAIL;
    }

    return tRes;
}

TEST_RESULT CAtomicStorePSTest::VerifyCompareExchangeReturnValues(
    UINT iteration,
    WinnerInfo winner,
    UINT numPixels,
    const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;
    UINT unchangedReturnCount = 0;

    for (UINT PID = 0; PID < numPixels; ++PID)
    {
        const UINT* pReturnData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * PID   );				
        const UINT returnData = pReturnData[iteration];

        if(PID == winner.PID)
        {
            if(returnData != m_UAVClearVal)
            {
                tRes = RESULT_FAIL;
                WriteToLog("FAIL: VerifyCompareExchangeReturnValues. returnData(%u) != clearValue(0x%x) PID=%u x=%u",returnData, 0xffffffff, PID, iteration);
                break;
            }
        }
        else
        {
            //
            // If the operation passes the compare but doesn't gain the lock, it doesn't
            // update the shared location and the return value is unchanged
            //
            if(returnData != winner.Value && returnData != 0xbaadf00d)
            {
                tRes = RESULT_FAIL;
                WriteToLog("FAIL: VerifyCompareExchangeReturnValues. Shared location was updated when it shoudn't have been: returnData(%u), PID=%u x=%u",returnData, PID, iteration);
                break;
            }

            if(returnData == 0xbaadf00d)
            {
                unchangedReturnCount++;	
            }
        }			
    }

    //
    // Shader disables one winning thread. Then re-enables them when they are all disabled either per group or per iteration.
    //
    if(tRes != RESULT_FAIL)
    {
        if(unchangedReturnCount != (iteration % numPixels) )
        {
            WriteToLog("FAIL: VerifyCompareExchangeReturnValues. x=%u. %u pixels had unchanged return values. Expected(%u).  iteration(%u)  mod  numPixels(%u)",  iteration, unchangedReturnCount, (iteration % numPixels), iteration , numPixels );	
            tRes = RESULT_FAIL;
        }
    }

    return tRes;

}

TEST_RESULT CAtomicStorePSTest::VerifyExchangeReturnValues(
    UINT iteration,
    UINT numPixels,
    const D3D11_MAPPED_SUBRESOURCE* pBufferData,
    __out UINT* pLastUpdate)
{
    TEST_RESULT tRes = RESULT_PASS;
    bool foundClear = false;

    bool* pFound = new bool[numPixels];
    ZeroMemory( (void*)pFound, sizeof(bool) * (numPixels) );


    for (UINT PID = 0; PID < numPixels; ++PID)
    {
        const UINT* pReturnData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * PID);				
        const UINT returnData = pReturnData[iteration];;

        if(returnData < numPixels )
        {
            if(pFound[returnData % numPixels] == false)
            {
                pFound[returnData % numPixels] = true;
            }
            else
            {
                WriteToLog("FAIL:VerifyExchangeReturnValues. Found duplicate entry %u ", returnData);
                tRes = RESULT_FAIL;	
                break;
            }
        }
        else if(returnData == m_UAVClearVal)
        {
            foundClear = true;	
        }
        else
        {
            WriteToLog("FAIL:VerifyExchangeReturnValues. Found value outside expected values %u ", returnData);
            tRes = RESULT_FAIL;	
            break;
        }
    }


    if(tRes	!= RESULT_FAIL)
    {
        if(!foundClear)
        {
            WriteToLog("FAIL:VerifyExchangeReturnValues. Clear value not found.  Exhange did not occur");
            tRes = RESULT_FAIL;	
        }

        //
        // Find the last one to update. This will be the buffer value
        //
        bool foundOne = false;
        for (UINT i = 0; i < numPixels ; ++i)
        {
            if( !pFound[i])
            {
                if(foundOne == false)
                {
                    *pLastUpdate = i;
                    foundOne = true;
                }
                else
                {
                    WriteToLog("FAIL:VerifyExchangeReturnValues. Found multiple unchanged entries. %u ", i);
                    tRes = RESULT_FAIL;	
                    break;
                }
            }
        }
    }

    delete[] pFound;
    return tRes;
}

TEST_RESULT CAtomicStorePSTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    HRESULT hr;
    TEST_RESULT tRes = RESULT_PASS;
    bool bMappedBuffer = false;
    const tstring intrinsicName = GetIntrinsicName();

    const UINT numPixels = GetPixelCount();

    assert(numPixels <= 10000);

    // Map result buffer directly
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;
    ZeroStructMem(mappedBuffer);
    if (m_pUAVBufCopy != NULL)
    {
        GetImmediateContext()->CopyResource( m_pUAVBufCopy, m_pUAVBuf );
        hr = GetImmediateContext()->Map( m_pUAVBufCopy, 0, D3D11_MAP_READ, 0, &mappedBuffer );
        if (FAILED(hr))
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Map(m_pUAVBufCopy) failed, hr=%s", D3DHResultToString(hr).c_str() );
            goto CleanUp;
        }
        else
        {
            bMappedBuffer = true;
        }
    }


    for (UINT x = 0; x < 2048 ; ++x)
    {
        WinnerInfo Winner = {0,0xbaadf00d,0};

        UINT Value = 0xffffffff;
        Value = *((UINT*)mappedBuffer.pData + x);


        for (UINT pid = 0; pid < numPixels; ++pid)
        {
            if( Value == pid)
            {
                if(Winner.Count == 0)
                {
                    Winner.PID = pid;
                    Winner.Value = Value;
                }

                Winner.Count = Winner.Count + 1;
            }

        }

        tRes = VerifyWinner(Winner);
        if(tRes != RESULT_PASS) break;

        if ( (intrinsicName == "CompareExchange") )
        {
            tRes = VerifyCompareExchangeReturnValues(
                    x,
                    Winner,
                    numPixels,
                    pBufferData);

            if(tRes != RESULT_PASS) break;

        }
        else if ( (intrinsicName == "Exchange") )
        {
            UINT lastUpdate = 0;
            tRes = VerifyExchangeReturnValues(
                    x,
                    numPixels,
                    pBufferData,
                    &lastUpdate);

            if(tRes != RESULT_PASS) break;

            if(Winner.Value != lastUpdate)
            {
                WriteToLog("Winner.value(%u) != lastUpdate(%u)", Winner.Value, lastUpdate);
                tRes = RESULT_FAIL;	
                break;
            }
        }

    } //iteration

CleanUp:

    if (bMappedBuffer)
        GetImmediateContext()->Unmap( m_pUAVBufCopy, 0 );

    return tRes;
}



//------------------------------------------------------------------------------------------------
//
// CMultiAtomicPSTest
// Base class for tests the call InterlockedAdd with InterlockedStore 
// and InterlockedExchange methods
//
//------------------------------------------------------------------------------------------------
CMultiAtomicPSTest::CMultiAtomicPSTest()
{
    m_Shader =
        "RWByteAddressBuffer buffer; \n"
        "RWTexture2D<uint> targetUAV; \n"
		"float ps_main(float4 Pos : SV_POSITION) : SV_TARGET \n"
        "{ \n"
        "  uint numPixels = %#x; \n" // insert num of pixels here
        "  uint PID = (Pos.y - 0.5) * %d + (Pos.x - 0.5); \n" // insert viewport width
        "  uint clearVal = %#x; \n" // insert clearVal here
        "  uint originalValsClearVal = %#x; \n"
        "  uint index = 0; \n"
        "  bool doOperation = true; \n"
		"  [fastopt]"
        "  for (uint outer = 0; outer < (2048/INNER_SIZE); ++outer) \n"
        "  { \n"
        "    uint originalVals[INNER_SIZE]; \n"
        "    uint inner; \n"
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      originalVals[inner] = originalValsClearVal; \n"
        "    } \n"
        "    [unroll] \n"
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      %s \n" // insert operation here (implemented differently for different derived classes)
        "      ++index; \n"
        "    } \n"
        // record every outer iteration
        "    for (inner = 0; inner < INNER_SIZE; ++inner) \n"
        "    { \n"
        "      targetUAV[uint2( outer*INNER_SIZE + inner, PID)] = originalVals[inner]; \n"
        "    } \n"
        "  } \n"
		"  return 1.0; \n"  //dummy output to the dummy rendertarget
        "} \n";
}

void CMultiAtomicPSTest::InitTestParameters()
{
    const tstring intrinsicName = GetIntrinsicName();
    m_UAVClearVal = 0;

    // Target UAV will store original values
    m_TargetUAVSize[0] = 2048; 
    m_TargetUAVSize[1] = 100;	// width * height (needs to be even for this test)
    m_TargetUAVSize[2] = 1;

    m_TargetUAVDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

    m_BufferSize = 2048;

    // viewport width and height, working as the X and Y dimensions of a thread group in Compute
    CUserValueSet<UINT> *pWidth = new CUserValueSet<UINT>;
    pWidth->AddValue(10);
    pWidth->AddValue(100);
	pWidth->AddValue(5);

    testfactor::RFactor rfViewWidth = AddParameter<UINT>(_T("ViewWidth"), &m_ViewWidth, pWidth);

    CUserValueSet<UINT> *pHeight = new CUserValueSet<UINT>;
    pHeight->AddValue(10);
    pHeight->AddValue(1);
    pHeight->AddValue(20);
    testfactor::RFactor rfViewHeight = AddParameter<UINT>(_T("ViewHeight"), &m_ViewHeight, pHeight);

    CUserValueSet<tstring> *pUAVType = new CUserValueSet<tstring>;
    pUAVType->AddValue(_T("RawBuffer"));
    testfactor::RFactor rfTypeUAV = AddParameter<tstring>(_T("UAV"), &m_TypeUAV, pUAVType);

    testfactor::RFactor root = (rfViewWidth % rfViewHeight) * rfTypeUAV;
    SetRootTestFactor(root);

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ViewWidth"), 5), WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );
}

TEST_RESULT CMultiAtomicPSTest::SetupTestCase()
{
    if (GetPixelCount() % 2 != 0)
    {
        WriteToLog("Test requires an even number of pixels");
        return RESULT_SKIP;
    }

    return CAtomicityPSTest::SetupTestCase();
}

TEST_RESULT CMultiAtomicPSTest::VerifyReturnValues(
    UINT iteration,
    UINT numPixels,
    bool replaceOccured,
    const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    TEST_RESULT tRes = RESULT_PASS;

	//
	// number of adds + clear
	//
	UINT numberOfExpectValues = 1 + (numPixels / 2);
    
	bool* pFound = new bool[numberOfExpectValues];
    ZeroMemory( (void*)pFound, sizeof(bool) * (numberOfExpectValues) );

    for (UINT PID = 0; PID < numPixels; ++PID)
    {
        const UINT* pReturnData = (UINT*)((BYTE*)pBufferData->pData + pBufferData->RowPitch * PID );				
        const UINT returnData = pReturnData[iteration];

        UINT expectedValue = returnData;		
		if(returnData == (m_ClearVal + 1) && (GetIntrinsicName() == "CompareStore"))
        {
            //
            // CompareStore does not have a return value. But the code block sets m_ClearVal + 1
            //
            continue;
        }
        else if( (returnData & m_ReplaceValue)  && replaceOccured)
        {
            expectedValue &=  ~m_ReplaceValue;
        }
        else if (replaceOccured && returnData != 0)
        {
            WriteToLog("Expected return data to contain m_ReplaceValue(0x%x)", m_ReplaceValue);
            tRes = RESULT_FAIL;	
            goto CleanUp;
        }
        else if (returnData & m_ReplaceValue)
        {
            WriteToLog("Return data contains m_ReplaceValue(0x%x) Not expected. replaceOccured %u", m_ReplaceValue, replaceOccured);
            tRes = RESULT_FAIL;
            goto CleanUp;
        }

        if(expectedValue == m_UAVClearVal)
        {
            //
            // Use the 0 location for the clear color
            //
            pFound[0] = true;
        }
        else if( expectedValue != 0  && expectedValue < numberOfExpectValues)
        {
            pFound[expectedValue] = true;
        }
        else
        {
            WriteToLog("FAIL:VerifyReturnValues. Value(%u) outside of range(%u)", expectedValue, numberOfExpectValues);
            tRes = RESULT_FAIL;	
            goto CleanUp;
        }
    }

	
	UINT foundCount = 0;
	for(UINT i = 0; i < numberOfExpectValues; i++)
	{
		//WriteToLog("pFound[%u]=%u %u %u", i, pFound[i], numberOfExpectValues, numPixels);
		if(pFound[i] == true)
		{
            foundCount++;
        }
	}

	if( foundCount != numberOfExpectValues && foundCount != (numberOfExpectValues - 1))
	{
		if(replaceOccured)
		{
			WriteToLog("Replace Occured: Expected return values (#additions + clear) = %u. Actual = %u.", numberOfExpectValues, foundCount);
		}
		else
		{
			WriteToLog("Expected return values (#additions + clear) = %u. Actual = %u.", numberOfExpectValues, foundCount);
		}
		
		tRes = RESULT_FAIL;	
	}

CleanUp:

    delete [] pFound;

    return tRes;

}

TEST_RESULT CMultiAtomicPSTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    HRESULT hr;
    TEST_RESULT tRes = RESULT_PASS;
    bool bMappedBuffer = false;
    const tstring intrinsicName = GetIntrinsicName();

    const UINT numPixels = GetPixelCount();

    assert(numPixels <= 100);

    D3D11_MAPPED_SUBRESOURCE mappedBuffer;
    ZeroStructMem(mappedBuffer);
    if (m_pUAVBufCopy != NULL)
    {
        GetImmediateContext()->CopyResource( m_pUAVBufCopy, m_pUAVBuf );
        hr = GetImmediateContext()->Map( m_pUAVBufCopy, 0, D3D11_MAP_READ, 0, &mappedBuffer );
        if (FAILED(hr))
        {
            tRes = RESULT_FAIL;
            WriteToLog( "Map(m_pUAVBufCopy) failed, hr=%s", D3DHResultToString(hr).c_str() );
            goto CleanUp;
        }
        else
        {
            bMappedBuffer = true;
        }
    }

    for (UINT x = 0; x < 2048 ; ++x)
    {
        UINT Value = 0xffffffff;
        bool replaceOccured = false;

        Value = *((UINT*)mappedBuffer.pData + x);

        if(Value & m_ReplaceValue)
        {
            replaceOccured = true;
        }			

        UINT numberOfAdds = numPixels / 2;

        if( (Value ==  (m_ReplaceValue | (m_AddValue * numberOfAdds))) 
            || (Value == (m_AddValue * numberOfAdds)) )
        {
            assert((tRes = RESULT_FAIL) || (tRes == RESULT_PASS));
        }
        else
        {
            WriteToLog("iteration= %u, Final Value = 0x%x. Should be 0x%x or 0x%x",  x, Value, m_ReplaceValue | (m_AddValue * numberOfAdds), m_AddValue * numberOfAdds );
            tRes = RESULT_FAIL;
            goto CleanUp;
        }

        tRes = VerifyReturnValues(
            x,
            numPixels,
            replaceOccured,
            pBufferData);

        if (tRes != RESULT_PASS)
        {
            goto CleanUp;
        }
    }

CleanUp:

    if (bMappedBuffer)
        GetImmediateContext()->Unmap( m_pUAVBufCopy, 0 );

    return tRes;
}

tstring CMultiAtomicCompareExchangePSTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    defines[0].Name = "INNER_SIZE";
    defines[0].Definition = "4";

    m_ReplaceValue = (1 << 30);
    m_AddValue = 1;
    m_ClearVal = 0xbaadf00d;


    static const char testedOperation[] = 
        "		if (doOperation) \n"
        "		{ \n"
        "			if ( ((PID ) %% 2) == 1) \n"
        "			{ \n"
        "				buffer.InterlockedCompareExchange(index*4, clearVal, 0x%x , originalVals[inner] ); \n"
        "			} \n" 
        "			else \n"
        "			{ \n"
        "				buffer.InterlockedAdd( index*4, 0x%x,  originalVals[inner] ); \n" 
        "			} \n"
        "		} \n";

    tstring test = FormatString(testedOperation, m_ReplaceValue, m_AddValue);

    tstring shaderCode = FormatString( m_Shader.c_str(), 
        GetPixelCount(),
        m_ViewWidth,
        m_UAVClearVal,
        m_ClearVal,
        test.c_str());

	if( g_TestApp.m_bLogShaderText )
		WriteToLog( "Shader Code =%s", shaderCode.c_str() );

    return shaderCode;
}

tstring CMultiAtomicCompareStorePSTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    defines[0].Name = "INNER_SIZE";
    defines[0].Definition = "4";

    m_ReplaceValue = (1 << 30);
    m_AddValue = 1;
    m_ClearVal = 0xbaadf00d;

    static const char testedOperation[] = 
        "		if (doOperation) \n"
        "		{ \n"
        "			if ( ((PID ) %% 2) == 1) \n"
        "			{ \n"
        "				buffer.InterlockedAdd( index*4, 0x%x,  originalVals[inner] ); \n" 
        "			} \n" 
        "			else \n"
        "			{ \n"
        "				buffer.InterlockedCompareStore(index*4, clearVal, 0x%x ); \n"
        "				originalVals[inner] = originalVals[inner] + 1;\n" //Fill with something so we can skip in validation
        "			} \n"
        "		} \n";

    tstring test = FormatString(testedOperation, m_AddValue, m_ReplaceValue);

    tstring shaderCode = FormatString( m_Shader.c_str(), 
        GetPixelCount(),
        m_ViewWidth,
        m_UAVClearVal,
        m_ClearVal,
        test.c_str());

	if( g_TestApp.m_bLogShaderText )
		WriteToLog( "Shader Code =%s", shaderCode.c_str() );

    return shaderCode;
}





//------------------------------------------------------------------------------------------------
// CAtomicMultipleIntrinsicPSTest
//------------------------------------------------------------------------------------------------

void CAtomicMultipleIntrinsicPSTest::InitTestParameters()
{
    m_TargetUAVSize[0] = 32;
    m_TargetUAVSize[1] = 1;
    m_TargetUAVSize[2] = 1;

    m_TargetUAVDimension = D3D11_UAV_DIMENSION_BUFFER;

    m_BufferSize = 1;

    // viewport width and height, working as the X and Y dimensions of a thread group in Compute
    CUserValueSet<UINT> *pWidth = new CUserValueSet<UINT>;
    pWidth->AddValue(32);
    pWidth->AddValue(4);
    pWidth->AddValue(1);
    testfactor::RFactor rfViewWidth = AddParameter<UINT>(_T("ViewWidth"), &m_ViewWidth, pWidth);

    CUserValueSet<UINT> *pHeight = new CUserValueSet<UINT>;
    pHeight->AddValue(1);
    pHeight->AddValue(8);
    pHeight->AddValue(32);
    testfactor::RFactor rfViewHeight = AddParameter<UINT>(_T("ViewHeight"), &m_ViewHeight, pHeight);

    CUserValueSet<tstring> *pUAVType = new CUserValueSet<tstring>;
    pUAVType->AddValue(_T("RawBuffer"));
    testfactor::RFactor rfTypeUAV = AddParameter<tstring>(_T("UAV"), &m_TypeUAV, pUAVType);


    testfactor::RFactor root = (rfViewWidth % rfViewHeight) * rfTypeUAV;

    SetRootTestFactor( root );

	AddPriorityPatternFilter(FilterEqual<UINT>(_T("ViewWidth"), 4), WeightFactorLarge);
	DeclarePriorityLevelByPriorityValue( 1, 1.5f );

}

tstring CAtomicMultipleIntrinsicPSTest::GetShaderCode(D3D10_SHADER_MACRO defines[MAX_DEFINES])
{
    static const char szShader_PSMultipleIntrinsic[] =
        "RWByteAddressBuffer buffer; \n"
        "RWBuffer<uint> targetUAV; \n"
		"float ps_main(float4 Pos : SV_POSITION) : SV_TARGET \n"
        "{ \n"
        "  uint PID = (Pos.y - 0.5) * %d + (Pos.x - 0.5); \n" // insert viewport width
        "  uint tgsmClearVal = %#x; \n"
        "  uint origValue = 0xdeadbeef; \n"
        "  if( PID <= 3 ) \n"
        "  { \n"
        // Unused bits
        "  } \n"
        "  else if( PID <= 7 ) \n"
        "  { \n"
        "  	buffer.InterlockedAdd( 0, int(-16), origValue ); \n" 
        "  } \n"
        "  else if( PID <= 11 ) \n"
        "  { \n"
        "  	buffer.InterlockedExchange( 0, ( (1 << PID) + tgsmClearVal ), origValue ); \n" 
        "  } \n"
        "  else if( PID <= 14 ) \n"
        "  { \n"
        // Unused bits
        "  } \n"
        "  else if( PID <= 17 ) \n"
        "  { \n"
        "  	buffer.InterlockedAnd( 0, ( ~(1 << PID)), origValue ); \n" 
        "  } \n"
        "  else if( PID <= 20 ) \n"
        "  { \n"
        "  	buffer.InterlockedOr( 0, (1 << PID), origValue ); \n" 
        "  } \n"
        "  else if( PID <= 23 ) \n"
        "  { \n"
        "  	buffer.InterlockedXor( 0, (1 << PID), origValue ); \n" 
        "  } \n"
        "  else if( PID <= 27 ) \n"
        "  { \n"
        "  	buffer.InterlockedAdd( 0, +(1 << 24), origValue ); \n" 
        "  } \n"
        "  else if( PID <= 29 ) \n"
        "  { \n"
        // Unused bits
        "  } \n"
        "  else if( PID == 30 ) \n"
        "  { \n"
        "    buffer.InterlockedMax( 0, ( (3 << 30) | tgsmClearVal ), origValue ); \n" // insert destination
        "  } \n"
        "  else if( PID == 31 ) \n"
        "  { \n"
        "    buffer.InterlockedMin( 0, ( (~(3 << 30)) & tgsmClearVal ), origValue ); \n" // insert destination
        "  } \n"
        "  else \n"
        "  { \n"
        // Unused bits
        "  } \n"
        "  if( (PID > 3 && PID <= 11)  || \n"
        "      (PID > 14 && PID <= 27) || \n"
        "      (PID > 29 && PID <= 31) ) \n"
        "  { \n"
        "    targetUAV[PID] = origValue; \n"
        "  } \n"
		"  return 1.0; \n"  //dummy output to the dummy rendertarget
        "} \n"

        ;

    //
    // Format the shader code
    //

    const tstring shaderCode = FormatString( szShader_PSMultipleIntrinsic, m_ViewWidth, m_UAVClearVal);

    WriteToLog(shaderCode.c_str());

    return shaderCode;
}

TEST_RESULT CAtomicMultipleIntrinsicPSTest::VerifyResult(const D3D11_MAPPED_SUBRESOURCE* pBufferData)
{
    // Local varaibles
    TEST_RESULT tRes = RESULT_PASS;
    UINT *pResults = (UINT*)pBufferData->pData;
    const UINT numPixels = GetPixelCount();
    std::vector<UINT> ResultsToIgnore;
    bool foundOriginalClearValue = false;
    bool foundMatchingValues = false;
    UINT numOriginalClearValues = 0;
    UINT numMatchingValues = 0;

    // Check the number of pixels (should be 32)
    if( 32 != numPixels )
    {
        tRes = RESULT_FAIL;
        WriteToLog("Unexpected number of pixels. There should only be 32 pixels. Found %d", numPixels);
    }

    // Setup which PID's results need to be ignored
    ResultsToIgnore.push_back(0);
    ResultsToIgnore.push_back(1);
    ResultsToIgnore.push_back(2);
    ResultsToIgnore.push_back(3);
    ResultsToIgnore.push_back(12);
    ResultsToIgnore.push_back(13);
    ResultsToIgnore.push_back(14);
    ResultsToIgnore.push_back(28);
    ResultsToIgnore.push_back(29);


    // Resulting buffer should have 32 values
    // Each representing the "original" values before a atomic operation was executed
    for( UINT PID = 0; PID < numPixels; ++PID )
    {
        bool ignoreCurrentResult = false;

        // Check to see if we need to ignore the current pixel's result
        for(UINT i = 0; i < ResultsToIgnore.size(); ++i)
            if( ResultsToIgnore[i] == PID )
                ignoreCurrentResult = true;

        if( ignoreCurrentResult )
            continue;

        if( pResults[PID] == m_UAVClearVal )
        {
            foundOriginalClearValue = true;
            numOriginalClearValues++;
        }

        // Check the current value against all other values in the array to verify there are no duplicates
        for( UINT index = PID + 1; index < numPixels; ++index )
        {
            ignoreCurrentResult = false;

            // Check to see if we need to ignore the current pixel's result
            for(UINT i = 0; i < ResultsToIgnore.size(); ++i)
                if( ResultsToIgnore[i] == index )
                    ignoreCurrentResult = true;

            if( ignoreCurrentResult )
                continue;

            if( pResults[PID] == pResults[index] )
            {
                foundMatchingValues = true;
                numMatchingValues++;
                WriteToLog("Found two values in the result UAV that match. UAV[%d] = %#x, UAV[%d] = %#x.", PID, pResults[PID], PID, pResults[PID] );
                break;
            }
        }
    }

    //
    // Check the results from the current group
    //

    // Check to make sure we didnt find any duplicate values
    if( foundMatchingValues )
    {
        tRes = RESULT_FAIL;
        WriteToLog("Found values in the Target UAV which matched. This means at least two instructions didn't operate atomically because they each returned the same original value. Found %d instances.", numMatchingValues);
    }

    // Check to make sure we found one, and only one, instance of the initial cleared value
    if( foundOriginalClearValue )
    {
        if( numOriginalClearValues > 1 )
        {
            tRes = RESULT_FAIL;
            WriteToLog("Found more than 1 instance of the original clear value. Original clear value = %#x. Found %d instances.", m_UAVClearVal, numOriginalClearValues);
        }
    }
    else
    {
        tRes = RESULT_FAIL;
        WriteToLog("Did NOT find any instances of the original clear value (for shared memory). Original clear value = %#x.", m_UAVClearVal);
    }

    return tRes;
}
