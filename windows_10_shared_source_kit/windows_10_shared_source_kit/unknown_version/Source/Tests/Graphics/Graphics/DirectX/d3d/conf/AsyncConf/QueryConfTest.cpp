#include "AsyncConf.h" 

// TODO: test more than just Draw for asynchronous behavior. see DX bugs 97047-97049

#include <mmsystem.h>
#pragma comment(lib, "winmm")

static HRESULT hrLatch(HRESULT a, HRESULT b)
{
    if (FAILED(a))
        return a;
    return b;
}

struct TEST_D3D11_QUERY_DATA_SO_STATISTICS : public D3D11_QUERY_DATA_SO_STATISTICS
{
public:
    bool operator==(const D3D11_QUERY_DATA_SO_STATISTICS& rhs) const
    {
        if (rhs.NumPrimitivesWritten != this->NumPrimitivesWritten)
            return false;
        if (rhs.PrimitivesStorageNeeded != this->PrimitivesStorageNeeded)
            return false;
        return true;
    }

    bool IsZero() const
    {
        if (this->NumPrimitivesWritten != 0)
            return false;

        if (this->PrimitivesStorageNeeded !=0)
            return false;

        return true;
    }
};

struct TEST_D3D11_QUERY_DATA_PIPELINE_STATISTICS : public D3D11_QUERY_DATA_PIPELINE_STATISTICS
{
public:
    bool operator==(const D3D11_QUERY_DATA_PIPELINE_STATISTICS& rhs) const
    {
        if (rhs.CInvocations != this->CInvocations)
            return false;
        if (rhs.CPrimitives != this->CPrimitives)
            return false;
        if (rhs.GSInvocations != this->GSInvocations)
            return false;
        if (rhs.GSPrimitives != this->GSPrimitives)
            return false;
        if (rhs.IAPrimitives != this->IAPrimitives)
            return false;
        if (rhs.IAVertices != this->IAVertices)
            return false;
        if (rhs.PSInvocations != this->PSInvocations)
            return false;
        if (rhs.VSInvocations != this->VSInvocations)
            return false;
        return true;
    }

    bool IsZero() const
    {
        if (this->CInvocations != 0)
            return false;

        if (this->CPrimitives != 0)
            return false;

        if (this->GSInvocations != 0)
            return false;

        if (this->GSPrimitives != 0)
            return false;

        if (this->IAPrimitives != 0)
            return false;

        if (this->IAVertices != 0)
            return false;

        if (this->PSInvocations != 0)
            return false;

        if (this->VSInvocations != 0)
            return false;

        return true;
    }
};

void CQueryConfTest::InitTestParameters()
{
    m_QueryDesc.MiscFlags = 0;

    testfactor::RFactor rF1, rF2, rF3, rF4, rF5;
    testfactor::RFactor rI1a, rI1b, rI2;
    testfactor::RFactor rOverAllocPart;
    testfactor::RFactor rPrefixWork, rSuffixWork;

    CUserValueSet<D3D11_QUERY> *pQueryValues = new CUserValueSet< D3D11_QUERY >();
    pQueryValues->AddValue( D3D11_QUERY_EVENT );
    pQueryValues->AddValue( D3D11_QUERY_TIMESTAMP );
    pQueryValues->AddValue( D3D11_QUERY_OCCLUSION );
    pQueryValues->AddValue( D3D11_QUERY_TIMESTAMP_DISJOINT );
    pQueryValues->AddValue( D3D11_QUERY_PIPELINE_STATISTICS );
    pQueryValues->AddValue( D3D11_QUERY_SO_STATISTICS );
    pQueryValues->AddValue( D3D11_QUERY_SO_OVERFLOW_PREDICATE );
    pQueryValues->AddValue( D3D11_QUERY_OCCLUSION_PREDICATE );

    CUserValueSet<bool> *pBeginValues = new CUserValueSet< bool >();
    pBeginValues->AddValue( false );
    pBeginValues->AddValue( false );
    pBeginValues->AddValue( true );
    pBeginValues->AddValue( true );
    pBeginValues->AddValue( true );
    pBeginValues->AddValue( true );
    pBeginValues->AddValue( true );
    pBeginValues->AddValue( true );

    CUserValueSet<int> *pGpuWorkValues = new CUserValueSet< int >();
    for (int k = 0; k < 5; ++k)
    {
        pGpuWorkValues->AddValue(k);
    }

    rF1 = AddParameter<D3D11_QUERY>( _T( "Query_Name" ), &m_QueryDesc.Query, pQueryValues );
    rF2 = AddParameter<bool>( _T( "Begin" ), &m_bCanBegin, pBeginValues );
    rF3 = AddParameter<bool>( _T( "Nest_Queries" ), &m_bNestQueries, BoolValueSet() );
    rF4 = AddParameter<bool>( _T( "Overlap_Queries" ), &m_bOverlapQueries, BoolValueSet() );
    rF5 = AddParameter<bool>( _T( "Do_GpuJob" ), &m_bDoGpuJob, BoolValueSet() );
    rPrefixWork = AddParameter<int>( _T( "Prefix_Gpu_Work" ), &m_nPrefixGpuIterations, pGpuWorkValues );
    rSuffixWork = AddParameter<int>( _T( "Suffix_Gpu_Work" ), &m_nSuffixGpuIterations, pGpuWorkValues );

    SetRootTestFactor( (rF1 % rF2) * rF3 * rF4 * rF5 * rPrefixWork * rSuffixWork );
}

bool CQueryConfTest::CallProcess(LPSTR pszwCommand)
{
    HRESULT hr = S_OK;
    
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if (FALSE == CreateProcess(0, 
                                pszwCommand,
                                0,
                                0,
                                FALSE,
                                0,
                                0,
                                NULL,
                                &si,
                                &pi))
    {
        WriteToLog( _T("CreateProcess for xperf.exe failed. This test requires xperf.exe to be installed. hr = %s\n"),  D3DHResultToString(HRESULT_FROM_WIN32(GetLastError())).c_str());
        return false;
    }

    // Wait for xperf to exit so we know for sure that trace enabling is a success
    DWORD dwRet = WaitForSingleObject(pi.hProcess, 5000);
    if (WAIT_TIMEOUT == dwRet || WAIT_ABANDONED == dwRet)
    {
        WriteToLog( _T("xperf.exe call timed out.\n") );
        return false;
    }

    //
    // Verify the exit code of xperf
    //
    dwRet=0;
    if(GetExitCodeProcess(pi.hProcess, &dwRet))
    {
        hr = HRESULT_FROM_WIN32(dwRet);
        if (FAILED(hr))
        {
            WriteToLog( _T("xperf.exe failed, hr = %s.\n"), D3DHResultToString(hr).c_str());
            return false;
        }
    }
    else
    {
        WriteToLog( _T("GetExitCodeProcess failed, hr = %s.\n"), D3DHResultToString(HRESULT_FROM_WIN32(GetLastError())).c_str());
        return false;
    }
    
    if (FALSE == CloseHandle(pi.hProcess))
    {
        WriteToLog( _T("xperf.exe failed to close handle for process, hr = %s.\n"), D3DHResultToString(HRESULT_FROM_WIN32(GetLastError())).c_str());
        return false;
    }
    
    if (FALSE == CloseHandle(pi.hThread))
    {
        WriteToLog( _T("xperf.exe failed to close handle for thread, hr = %s.\n"), D3DHResultToString(HRESULT_FROM_WIN32(GetLastError())).c_str());
        return false;
    }
    
    return true;
}

TEST_RESULT CQueryConfTest::ExecuteTestCase()
{
    TEST_RESULT tRes = RESULT_PASS;

    if (g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_10_0 && m_QueryDesc.Query != D3D11_QUERY_TIMESTAMP && m_QueryDesc.Query != D3D11_QUERY_TIMESTAMP_DISJOINT)
    {
        // only targeting timestamps on FL9, anything else should be a SKIP
        return RESULT_SKIP;
    }
    
    switch (m_QueryDesc.Query)
    {
    case D3D11_QUERY_EVENT:
        tRes = TimingTestCase();
        break;
    case D3D11_QUERY_TIMESTAMP:
    case D3D11_QUERY_TIMESTAMP_DISJOINT:

        tRes = TimingTestCase();
        
#if defined(_ARM_) || defined(_ARM64_) // temp. check for arm64
    if (tRes == RESULT_FAIL)
    {
        bool bCallProcessSucceeded = false;

        bCallProcessSucceeded = CallProcess("xperf.exe -start dxgklogger -on 802ec45a-1e99-4b83-9920-87c98277ba9d:0x1400:5 -BufferSize 1024 -MinBuffers 20 -MaxBuffers 160 -f dxgk.etl");
        if (!bCallProcessSucceeded)
        {
            // error will be logged in CallProcess
            tRes = RESULT_FAIL;
            goto EndCase;
        }

        WriteToLog( _T("Re-running while holding p-state constant.") );
        tRes = TimingTestCase();

        bCallProcessSucceeded = CallProcess("xperf.exe -stop dxgklogger");
        if (!bCallProcessSucceeded)
        {
            // error will be logged in CallProcess
            tRes = RESULT_FAIL;
            goto EndCase;
        }
    }

EndCase:
#endif
        break;

    case D3D11_QUERY_PIPELINE_STATISTICS:
    case D3D11_QUERY_SO_STATISTICS:
    case D3D11_QUERY_SO_OVERFLOW_PREDICATE:
        tRes = StatsTestCase();
        break;
    case D3D11_QUERY_OCCLUSION:
    case D3D11_QUERY_OCCLUSION_PREDICATE:
        tRes = OcclusionTestCase();
        break;
    default:
        assert(!"Invalid Test Case");
        tRes = RESULT_FAIL;
        break;
    }

    return tRes;
}

TEST_RESULT CQueryConfTest::TimingTestCase()
{
    HRESULT hr = S_OK, hr2 = S_OK;
    TEST_RESULT tRes = RESULT_PASS;
    ID3D11Query *pQuery0 = NULL;
    ID3D11Query *pQuery1 = NULL;
    ID3D11Query *pQuery2 = NULL;

    D3D_DRIVER_TYPE DriverType = g_App.m_D3DOptions.DriverType;

    if ( FAILED(hr = GetDevice()->CreateQuery(&m_QueryDesc, &pQuery1)) )
    {
        WriteToLog( _T("Failed to create Query1. hr=S_FALSE\n") );
        tRes = RESULT_FAIL;
        goto testDone;
    }
    if ( FAILED(hr = GetDevice()->CreateQuery(&m_QueryDesc, &pQuery2)) )
    {
        WriteToLog( _T("Failed to create Query2. hr=S_FALSE\n") );
        tRes = RESULT_FAIL;
        goto testDone;
    }

    SetupGpuJob();// Makes context calls :)

    if (m_bCanBegin)
    {
        if (m_bOverlapQueries)
        {
            if (m_bNestQueries)
            {
                GetEffectiveContext()->Begin( pQuery2 );
                GetEffectiveContext()->Begin( pQuery1 );
            }
            else
            {
                GetEffectiveContext()->Begin( pQuery1 );
                GetEffectiveContext()->Begin( pQuery2 );
            }
        }
        else
        {
            GetEffectiveContext()->Begin( pQuery1 );
        }
    }
    else
    {
        if ( FAILED(hr = GetDevice()->CreateQuery(&m_QueryDesc, &pQuery0)) )
        {
            WriteToLog( _T("Failed to create Query0. hr=S_FALSE\n") );
            tRes = RESULT_FAIL;
            goto testDone;
        }
        GetEffectiveContext()->End(pQuery0);
    }

    // issue a couple of GPU jobs to put stuff into the command buffer
    if (m_bDoGpuJob)
        hr = GpuBoundJob(false); // makes context calls :)
    GetEffectiveContext()->End( pQuery1 );

    if (m_bCanBegin && !m_bOverlapQueries)
    {
        GetEffectiveContext()->Begin( pQuery2 );
    }

    if (m_bDoGpuJob)
        hr = hrLatch(hr, GpuBoundJob(true)); // makes context calls
    GetEffectiveContext()->End( pQuery2 );
    if (FAILED(hr))
    {
        WriteToLog( _T("Failed to execute GPU job. hr=S_FALSE\n") );
        tRes = RESULT_FAIL;
        goto testDone;
    }

    //if (m_pDebug)
    //	hr = m_pDebug->GetLastError();
    // TODO: use ID3D11InfoQueue. 

    //GetImmediateContext()->Flush();
    
    if( FAILED( hr = ExecuteEffectiveContext() ) )
    {
        WriteToLog( _T("Failed to execute deferred context.") );
        tRes = RESULT_FAIL;
        goto testDone;
    }

    // TODO: known issue with REF:
    // GetData(noflush) will spin forever
    // GetData(flush) will block the calling thread
    // neither behavior is asynchronous

    // get 2 first, then make sure 1 was also finished
    {
        if (m_bDoGpuJob)
        {
            // Make sure it can't immediately succeed if no flush was ever inserted
            hr = GetImmediateContext()->GetData(pQuery2, nullptr, 0, D3D11_ASYNC_GETDATA_DONOTFLUSH);

            // D3D11_QUERY_TIMESTAMP_DISJOINT can be synchronous if the driver knows that the frequency will never change
            // WARP can fail this because EndData caused render tasks to begin execution
            // Ref is syncronous when running on a deferred context
            if ((hr == S_OK) && (D3D11_QUERY_TIMESTAMP_DISJOINT != m_QueryDesc.Query) && (DriverType != D3D_DRIVER_TYPE_WARP) && !m_bDeferredREF)
            {
                WriteToLog( _T("Query2.GetData(NoFlush) returned data immediately, query likely synchronous.\n") );
                tRes = RESULT_FAIL;
            }
        }
        else
        {
            hr = S_FALSE;
        }

        if (hr == S_FALSE)
        {
            hr = GetImmediateContext()->GetData(pQuery2, nullptr, 0, 0);
            // Make sure it can't immediately succeed *when* the flush is inserted
            // D3D11_QUERY_TIMESTAMP_DISJOINT can be synchronous if the driver knows that the frequency will never change
            // WARP can fail this because EndData caused render tasks to begin execution
            // Ref is syncronous when running on a deferred context
            if (hr == S_OK && m_bDoGpuJob && (D3D11_QUERY_TIMESTAMP_DISJOINT != m_QueryDesc.Query) && (DriverType != D3D_DRIVER_TYPE_WARP) && !m_bDeferredREF)
            {
                WriteToLog( _T("Query2.GetData returned S_OK on first call with Flush, query likely synchronous.\n") );
                tRes = RESULT_FAIL;
            }
        }

        // Run until timeout
        if (hr == S_FALSE)
        {
            hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pQuery2, nullptr, 0, 0 );
        }

        if (hr == S_FALSE)
        {
            WriteToLog( _T("Query2.GetData(Flush) appears to be non-terminating (%d seconds elapsed).\n"), GetFramework()->GetQueryTimeout() / 1000 );
            tRes = RESULT_FAIL;
            goto testDone;
        }

        if (hr != S_OK)
        {
            WriteToLog( _T("Failed on call to Query2.GetData. hr=%s\n"), D3DHResultToString(hr).c_str() );
            tRes = RESULT_FAIL;
            goto testDone;
        }
    }

    // got 2, ensure 1 is also finished (proper ordering)
    {
        hr = GetImmediateContext()->GetData(pQuery1, nullptr, 0, D3D11_ASYNC_GETDATA_DONOTFLUSH);
        if (hr == S_FALSE)
        {
            WriteToLog( _T("Query1 should have finished before Query2, but GetData returned hr=S_FALSE\n") );
            tRes = RESULT_FAIL;
            hr = GetImmediateContext()->GetData(pQuery1, nullptr, 0, 0);

        }

        // Check for asynchronous behavior
        if (hr == S_FALSE)
        {
            WriteToLog( _T("Query1.GetData still not done after flush, ordering problem between Queries 1 & 2.\n") );
            tRes = RESULT_FAIL;
            hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pQuery1, nullptr, 0, 0 );
        }

        if (hr != S_OK)
        {
            WriteToLog( _T("Failed on call to Query1.GetData. hr=%s\n"), D3DHResultToString(hr).c_str() );
            tRes = RESULT_FAIL;
            goto testDone;
        }
    }

    // Now retrieve the specific data.
    // Use DONOTFLUSH flag, since the data should be available by now. If not, it's an error.
    switch (m_QueryDesc.Query)
    {
    case D3D11_QUERY_EVENT:
        {
            // query 0 is actually the first-issued
            BOOL value0, value1, value2;
            hr = GetImmediateContext()->GetData( pQuery0, &value0, sizeof(value0), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            hr = (S_OK != hr)? hr : GetImmediateContext()->GetData( pQuery1, &value1, sizeof(value1), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            hr = (S_OK != hr)? hr : GetImmediateContext()->GetData( pQuery2, &value2, sizeof(value2), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if (hr != S_OK)
            {
                WriteToLog( _T("EVENT GetData did not return S_OK. hr=%s\n"), D3DHResultToString(hr).c_str() );
                tRes = RESULT_FAIL;
                goto testDone;
            }
            if (value0 != TRUE || value1 != TRUE || value2 != TRUE)
            {
                WriteToLog( _T("EVENT values are not TRUE.\n") );
                tRes = RESULT_FAIL;
                goto testDone;
            }
        }
        break;
    case D3D11_QUERY_TIMESTAMP:
        {
            // query 0 is actually the first-issued
            UINT64 value0, value1, value2;
            hr = GetImmediateContext()->GetData( pQuery0, &value0, sizeof(value0), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            hr = (S_OK != hr)? hr : GetImmediateContext()->GetData( pQuery1, &value1, sizeof(value1), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            hr = (S_OK != hr)? hr : GetImmediateContext()->GetData( pQuery2, &value2, sizeof(value2), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if (hr != S_OK)
            {
                WriteToLog( _T("TIMESTAMP GetData did not return S_OK. hr=%s\n"), D3DHResultToString(hr).c_str() );
                tRes = RESULT_FAIL;
                goto testDone;
            }
            if (m_bDoGpuJob && (value0 == value1 || value0 == value2 || value1 == value2))
            {
                tRes = RESULT_FAIL;
                WriteToLog( _T("TIMESTAMP values are identical.\n"));
            }
            if (value0 > value1 || value1 > value2)
            {
                tRes = RESULT_FAIL;
                WriteToLog( _T("TIMESTAMP values are out of order.\n"));
            }
            //if (tRes != RESULT_PASS)
            {
                WriteToLog( _T("v0=%#I64x, v1=%#I64x, v2=%#I64x\n"), value0, value1, value2 );
            }
        }
        break;
        // TODO: more test case evaluation
    case D3D11_QUERY_TIMESTAMP_DISJOINT:
        {
            // query 0 is actually the first-issued
            D3D11_QUERY_DATA_TIMESTAMP_DISJOINT value1, value2;
            hr = GetImmediateContext()->GetData( pQuery1, &value1, sizeof(value1), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            hr = (S_OK != hr)? hr : GetImmediateContext()->GetData( pQuery2, &value2, sizeof(value2), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if (hr != S_OK)
            {
                WriteToLog( _T("TIMESTAMP_DISJOINT GetData did not return S_OK. hr=%s\n"), D3DHResultToString(hr).c_str() );
                tRes = RESULT_FAIL;
                goto testDone;
            }
            if (   value1.Frequency != value2.Frequency
                || value1.Frequency < 1)
            {
                WriteToLog( _T("TIMESTAMP_DISJOINT frequencies are different. v1=%#I64x, v2=%#I64x\n"), value1.Frequency, value2.Frequency );
                tRes = RESULT_FAIL;
            }
        }
        break;
    default:
        assert(!"Invalid Test Case");
        tRes = RESULT_FAIL;
        break;
    }

testDone:
    // cleanup
    SAFE_RELEASE( pQuery0 );
    SAFE_RELEASE( pQuery1 );
    SAFE_RELEASE( pQuery2 );

    return tRes;
}

TEST_RESULT CQueryConfTest::OcclusionTestCase()
{
    HRESULT hr = E_FAIL;
    TEST_RESULT tRes = RESULT_PASS;

    ID3D11Query *pQuery[3] = {NULL,NULL,NULL};
    ID3D11RasterizerState* pRSState = NULL;
    ID3D11DepthStencilState* pDSState = NULL;

    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(rastDesc));
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));

    // initialize pipeline
    {
        for (int k = 0; k < 3; ++k)
        {
            if ( FAILED(hr = GetDevice()->CreateQuery(&m_QueryDesc, &pQuery[k])) )
            {
                WriteToLog( _T("Failed to create Query%d. hr=%s\n"), k, D3DHResultToString(hr).c_str() );
                tRes = RESULT_FAIL;
                goto testDone;
            }
        }

        rastDesc.CullMode = D3D11_CULL_NONE;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        rastDesc.MultisampleEnable = TRUE;
        hr = GetDevice()->CreateRasterizerState(&rastDesc, &pRSState);
        if (FAILED(hr))
        {
            WriteToLog(_T("CreateRasterizerState failed"), D3DHResultToString(hr).c_str() );
            tRes = RESULT_FAIL;
            goto testDone;
        }
        GetEffectiveContext()->RSSetState(pRSState);

        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        hr = GetDevice()->CreateDepthStencilState(&dsDesc, &pDSState);
        if (FAILED(hr))
        {
            WriteToLog(_T("CreateDepthStencilState failed"), D3DHResultToString(hr).c_str() );
            tRes = RESULT_FAIL;
            goto testDone;
        }
        GetEffectiveContext()->OMSetDepthStencilState(pDSState, 0);

        FLOAT clearColor[4] = {0,0,0,0};
        GetEffectiveContext()->ClearRenderTargetView(m_pRTView, clearColor);
        GetEffectiveContext()->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    SetupGpuJob();

    GetEffectiveContext()->Begin(pQuery[2]);
    GetEffectiveContext()->Begin(pQuery[0]);
    GpuBoundJob(false);
    GetEffectiveContext()->End(pQuery[0]);
    GetEffectiveContext()->Begin(pQuery[1]);
    GpuBoundJob(false);
    GetEffectiveContext()->End(pQuery[1]);
    GetEffectiveContext()->End(pQuery[2]);

    if( FAILED(hr = ExecuteEffectiveContext() ) )
    {
        WriteToLog( _T("Failed to execute deferred context.") );
        tRes = RESULT_FAIL;
        goto testDone;
    }

    if (m_QueryDesc.Query == D3D11_QUERY_OCCLUSION)
    {
        UINT64 queryData[3];
        for (int k = 0; k < 3; ++k)
        {
            hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pQuery[k], &queryData[k], sizeof(queryData[k]), 0 );
            if (hr != S_OK)
            {
                WriteToLog(_T("Query%d.GetDataWithTimeout() did not finish in time. hr=%s"), k, D3DHResultToString(hr).c_str() );
            }
            // TODO: test asynchronous execution
            if (hr != S_OK)
            {
                if (FAILED(hr))
                {
                    WriteToLog(_T("Query%d.GetData() failed. hr=%s"), k, D3DHResultToString(hr).c_str() );
                    tRes = RESULT_FAIL;
                    goto testDone;
                }
                WriteToLog(_T("Query%d.GetData() did not return S_OK. hr=%s"), k, D3DHResultToString(hr).c_str() );
            }
        }

        if (queryData[0] == 0)
        {
            WriteToLog(_T("First render was occluded. queryData[0]=%I64d"), 
                queryData[0] );
            tRes = RESULT_FAIL;
        }

        if (queryData[1] != 0)
        {
            WriteToLog(_T("Second render was not fully occluded. queryData[1]=%I64d"), 
                queryData[1] );
            tRes = RESULT_FAIL;
        }

        if (queryData[0] != queryData[2])
        {
            WriteToLog(_T("Occlusion values do not correspond. queryData[0]=%I64d, queryData[2]=%I64d"), 
                queryData[0], queryData[2] );
            tRes = RESULT_FAIL;
        }
    }
    else if (m_QueryDesc.Query == D3D11_QUERY_OCCLUSION_PREDICATE)
    {
        BOOL queryData[3];
        for (int k = 0; k < 3; ++k)
        {
            hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pQuery[k], &queryData[k], sizeof(queryData[k]), 0 );
            if (hr != S_OK)
            {
                WriteToLog(_T("Query%d.GetDataWithTimeout() did not finish in time. hr=%s"), k, D3DHResultToString(hr).c_str() );
            }
            // TODO: test asynchronous execution
            if (hr != S_OK)
            {
                if (FAILED(hr))
                {
                    WriteToLog(_T("Query%d.GetData() failed. hr=%s"), k, D3DHResultToString(hr).c_str() );
                    tRes = RESULT_FAIL;
                    goto testDone;
                }
                WriteToLog(_T("Query%d.GetData() did not return S_OK. hr=%s"), k, D3DHResultToString(hr).c_str() );
            }
        }

        if (queryData[0] == 0)
        {
            WriteToLog(_T("First render was occluded. queryData[0]=%d"), 
                queryData[0] );
            tRes = RESULT_FAIL;
        }

        if (queryData[1] != 0)
        {
            WriteToLog(_T("Second render was not fully occluded. queryData[1]=%d"), 
                queryData[1] );
            tRes = RESULT_FAIL;
        }

        if (queryData[0] != queryData[2])
        {
            WriteToLog(_T("Occlusion values do not correspond. queryData[0]=%I64d, queryData[2]=%d"), 
                queryData[0], queryData[2] );
            tRes = RESULT_FAIL;
        }
    }
    else
    {
        assert(!"Unexpected Query type");
    }


testDone:
    GetEffectiveContext()->RSSetState(NULL);
    GetEffectiveContext()->OMSetDepthStencilState(NULL, 0);

    if( FAILED(hr = ExecuteEffectiveContext() ) )
    {
        WriteToLog( _T("Failed to execute deferred context.") );
        tRes = RESULT_FAIL;
    }

    SAFE_RELEASE(pRSState);
    SAFE_RELEASE(pDSState);
    for (int k = 0; k < 3; ++k)
    {
        SAFE_RELEASE(pQuery[k]);
    }

    return tRes;
}

TEST_RESULT CQueryConfTest::StatsTestCase()
{
    HRESULT hr = S_OK, hr2 = S_OK;
    TEST_RESULT tRes = RESULT_PASS;
    ID3D11Query *pQuery1 = NULL;
    ID3D11Query *pQuery2 = NULL;

    assert(m_bCanBegin);

    D3D_DRIVER_TYPE DriverType = g_App.m_D3DOptions.DriverType;

    if ( FAILED(hr = GetDevice()->CreateQuery(&m_QueryDesc, &pQuery1)) )
    {
        WriteToLog( _T("Failed to create Query1. hr=S_FALSE\n") );
        tRes = RESULT_FAIL;
        goto testDone;
    }
    if ( FAILED(hr = GetDevice()->CreateQuery(&m_QueryDesc, &pQuery2)) )
    {
        WriteToLog( _T("Failed to create Query2. hr=S_FALSE\n") );
        tRes = RESULT_FAIL;
        goto testDone;
    }

    SetupGpuJob();

    // execute GPU jobs before issuing the queries
    // these jobs should not affect the query results
    for (int k = 0; k < m_nPrefixGpuIterations; ++k)
    {
        hr = FAILED(hr) ? hr : GpuBoundJob(false);
    }

    if (m_bOverlapQueries)
    {
        if (m_bNestQueries)
        {
            GetEffectiveContext()->Begin( pQuery2 );
            GetEffectiveContext()->Begin( pQuery1 );
        }
        else
        {
            GetEffectiveContext()->Begin( pQuery1 );
            GetEffectiveContext()->Begin( pQuery2 );
        }
    }
    else
    {
        GetEffectiveContext()->Begin( pQuery1 );
    }

    // issue a couple of GPU jobs to put stuff into the command buffer
    if (m_bDoGpuJob)
        hr = FAILED(hr) ? hr : GpuBoundJob(false);

    GetEffectiveContext()->End( pQuery1 );

    if (!m_bOverlapQueries)
    {
        GetEffectiveContext()->Begin( pQuery2 );
        
        if (!m_bDoGpuJob) // query2 does opposite of query1 here
            hr = FAILED(hr)?hr : GpuBoundJob(false);
    }

    GetEffectiveContext()->End( pQuery2 );

    if (FAILED(hr))
    {
        WriteToLog( _T("Failed to execute GPU job. hr=S_FALSE\n") );
        tRes = RESULT_FAIL;
        goto testDone;
    }

    // execute GPU jobs after issuing the queries
    // these jobs should not affect the query results
    for (int k = 0; k < m_nSuffixGpuIterations; ++k)
    {
        hr = FAILED(hr) ? hr : GpuBoundJob(false);
    }

    //if (m_pDebug)
    //	hr = m_pDebug->GetLastError();
    // TODO: use ID3D11InfoQueue. 

    //GetImmediateContext()->Flush();
    
    if( FAILED( hr = ExecuteEffectiveContext() ) )
    {
        WriteToLog( _T("Failed to execute deferred context.") );
        tRes = RESULT_FAIL;
        goto testDone;
    }

    // TODO: known issue with REF:
    // GetData(noflush) will spin forever
    // GetData(flush) will block the calling thread
    // neither behavior is asynchronous

    // get 2 first, then make sure 1 was also finished
    {
        // Make sure it can't immediately succeed if no flush was ever inserted
        hr = GetImmediateContext()->GetData(pQuery2, nullptr, 0, D3D11_ASYNC_GETDATA_DONOTFLUSH);

        // 
        // Allow GetData(NoFlush) to be synchronous on software rasterizers
        // because they may not be batching commands like a hardware driver
        //
        if ((hr == S_OK) && (DriverType == D3D_DRIVER_TYPE_HARDWARE))
        {
            WriteToLog( _T("Query2.GetData(NoFlush) returned data immediately, query likely synchronous.\n") );
            tRes = RESULT_FAIL;
        }

        // Run until timeout
        if (hr == S_FALSE)
        {
            hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pQuery2, nullptr, 0, 0 );
        }

        if (hr == S_FALSE)
        {
            WriteToLog( _T("Query2.GetData(Flush) appears to be non-terminating (%d seconds elapsed).\n"), GetFramework()->GetQueryTimeout() / 1000 );
            tRes = RESULT_FAIL;
            goto testDone;
        }

        if (hr != S_OK)
        {
            WriteToLog( _T("Failed on call to Query2.GetData. hr=%s\n"), D3DHResultToString(hr).c_str() );
            tRes = RESULT_FAIL;
            goto testDone;
        }
    }

    // got 2, ensure 1 is also finished (proper ordering)
    {
        hr = GetImmediateContext()->GetData(pQuery1, nullptr, 0, D3D11_ASYNC_GETDATA_DONOTFLUSH);
        if (hr == S_FALSE)
        {
            WriteToLog( _T("Query1 should have finished before Query2, but GetData returned hr=S_FALSE\n") );
            tRes = RESULT_FAIL;
            hr = GetImmediateContext()->GetData(pQuery1, nullptr, 0, 0);
        }

        // Check for asynchronous behavior
        if (hr == S_FALSE)
        {
            WriteToLog( _T("Query1.GetData still not done after flush, ordering problem between Queries 1 & 2.\n") );
            tRes = RESULT_FAIL;
            hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), pQuery1, nullptr, 0, 0 );
        }

        if (hr != S_OK)
        {
            WriteToLog( _T("Failed on call to Query1.GetData. hr=%s\n"), D3DHResultToString(hr).c_str() );
            tRes = RESULT_FAIL;
            goto testDone;
        }
    }

    // Now retrieve the specific data.
    // Use DONOTFLUSH flag, since the data should be available by now. If not, it's an error.
    switch (m_QueryDesc.Query)
    {
    case D3D11_QUERY_PIPELINE_STATISTICS:
        {
            // query 0 is actually the first-issued
            TEST_D3D11_QUERY_DATA_PIPELINE_STATISTICS value1, value2;
            hr = GetImmediateContext()->GetData( pQuery1, &value1, sizeof(value1), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            hr = (S_OK != hr)? hr : GetImmediateContext()->GetData( pQuery2, &value2, sizeof(value2), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if (hr != S_OK)
            {
                WriteToLog( _T("EVENT GetData did not return S_OK. hr=%s\n"), D3DHResultToString(hr).c_str() );
                tRes = RESULT_FAIL;
                goto testDone;
            }

            tRes = VerifyStats<TEST_D3D11_QUERY_DATA_PIPELINE_STATISTICS>(value1, value2);
            if(RESULT_PASS != tRes)
            {
                goto testDone;
            }
            // TODO: test the pipeline statistics more thoroughly
        }
        break;
    case D3D11_QUERY_SO_STATISTICS:
        {
            // query 0 is actually the first-issued
            TEST_D3D11_QUERY_DATA_SO_STATISTICS value1, value2;
            hr = GetImmediateContext()->GetData( pQuery1, &value1, sizeof(value1), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            hr = (S_OK != hr)? hr : GetImmediateContext()->GetData( pQuery2, &value2, sizeof(value2), D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if (hr != S_OK)
            {
                WriteToLog( _T("TIMESTAMP GetData did not return S_OK. hr=%s\n"), D3DHResultToString(hr).c_str() );
                tRes = RESULT_FAIL;
                goto testDone;
            }
            
            tRes = VerifyStats<TEST_D3D11_QUERY_DATA_SO_STATISTICS>(value1, value2);
            if(RESULT_PASS != tRes)
            {
                goto testDone;
            }
            // TODO: test the stream-out statistics more thoroughly
        }
        break;
        // TODO: more test case evaluation
    case D3D11_QUERY_SO_OVERFLOW_PREDICATE:
        break;
    default:
        assert(!"Invalid Test Case");
        tRes = RESULT_FAIL;
        break;
    }

testDone:
    // cleanup
    SAFE_RELEASE( pQuery1 );
    SAFE_RELEASE( pQuery2 );

    return tRes;
}
