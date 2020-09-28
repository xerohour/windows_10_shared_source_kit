Log(_T("*** FAILURE: Present count is not incrementing ***"));
        return RESULT_FAIL;
    }

    return tr;
}
//-----------------------------------------------------------------------------
// The CDXGIWithoutOwnershipTest Test
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Verify calling GetFrameStatistics() without ownerhsip will fail
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIWithoutOwnershipTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    LOG_RETURN_ON_NULL(pDXGIOutput, RESULT_FAIL, _T("RunCase: Bad input (DXGIOutput = NULL)"));

    TEST_RESULT tr = RESULT_FAIL;
    HRESULT hr = E_FAIL;
    HRESULT expectedhr = DXGI_ERROR_UNSUPPORTED;

    // Frame statistices
    DXGI_FRAME_STATISTICS fs1;
    ZeroMemory( &fs1, sizeof(fs1) );


    // Get frame statistics
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
    if( expectedhr != hr )
    {
        WriteToLog(_T("GetFrameStatistics() returned an unexpected HRESULT()."));
        goto cleanup;
    }

    tr = RESULT_PASS;

cleanup:

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGINULLStatsTest Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: GetFrameStatistics(NULL)
//-----------------------------------------------------------------------------
TEST_RESULT CDXGINULLStatsTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    TEST_RESULT tr = RESULT_FAIL;
    HRESULT hr = E_FAIL;

    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    LOG_RETURN_ON_NULL(pDXGIOutput, RESULT_FAIL, _T("RunCase: Bad input (DXGIOutput = NULL)"));

    // Take ownership
    hr = TakeOwnership(pDXGISwapChain, pDXGIOutput);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("Unable to take ownership"));
        return RESULT_SKIP;
    }

    // Get frame statistics before present
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, NULL);
    if( SUCCEEDED(hr) )
    {
        WriteToLog(_T("Error GetFrameStatistics(NULL) Succeeded"));
        goto cleanup;
    }

    tr = RESULT_PASS;

cleanup:
    // Release ownership
    ReleaseOwnership(pDXGISwapChain, pDXGIOutput);

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIGetLastPresentCountNULL Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: GetLastPresentCount(NULL)
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIGetLastPresentCountNULLTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = E_FAIL;

    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    if( DXGISWAPCHAIN != GetCurrentResourceType() )
    {
        WriteToLog(_T("Can only run on DXGISWAPCHAIN."));
        return RESULT_SKIP;
    }

    // Get frame statistics before present
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, NULL);
    if( SUCCEEDED(hr) )
    {
        WriteToLog(_T("*** FAILURE: Error GetFrameStatistics(NULL) Succeeded ***"));
        return RESULT_FAIL;
    }

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIGlitchTest Test
//-----------------------------------------------------------------------------

// In windowed mode, Present Stats can be quite laggy.
#define DELAY_BETWEEN_GFS_CALLS 1000

//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Verify that that a glitch can be detected.
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIGlitchTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    if( DXGISWAPCHAIN != GetCurrentResourceType() )
    {
        WriteToLog(_T("RunCase: Can only run on DXGISWAPCHAIN."));
        return RESULT_SKIP;
    }

    if( GetFramework()->m_D3DOptions.SrcDriverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        WriteToLog(_T("RunCase: Can't run case against reference rasterizer."));
        return RESULT_SKIP;
    }
    else if( GetFramework()->m_D3DOptions.SrcDriverType == D3D_DRIVER_TYPE_SOFTWARE )
    {
        WriteToLog(_T("RunCase: Can't run case against software rasterizer."));
        return RESULT_SKIP;
    }
    else if( GetFramework()->m_D3DOptions.SrcDriverType == D3D_DRIVER_TYPE_WARP )
    {
        WriteToLog(_T("RunCase: Can't run case against Warp rasterizer."));
        return RESULT_SKIP;
    };


    TEST_RESULT tr = RESULT_FAIL;   // case result
    HRESULT hr = E_FAIL;

    // Frame statistices
    DXGI_FRAME_STATISTICS fs1, fs2, fsInitial;
    ZeroMemory( &fs1, sizeof(fs1) );// 1st frame stats
    ZeroMemory( &fs2, sizeof(fs2) );// 2nd frame stats
    ZeroMemory( &fsInitial, sizeof(fsInitial) );// Initial frame stats

    // Timeout for waiting for fame stats to update
    UINT nTimeout = 0;

    // Take ownership
    if( DXGISWAPCHAIN == GetCurrentResourceType() && FALSE == GetOwnByFullScreen() )
    {
        // We want the swapchain to GetFrameStats without having ownership
        WriteToLog(_T("RunCase: Not taking ownership for this case."));
    }
    else
    {
        hr = TakeOwnership(pDXGISwapChain, pDXGIOutput);
        if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
        {
            WriteToLog(_T("RunCase: Unable to take ownership"));
            tr = RESULT_SKIP;
            goto cleanup;
        }

    }

    // Reset FrameStats (Clear out initial DISJOINT error and get initial stats)
    WriteToLog(_T("Calling GetFrameStatistics to clear out initial DISJOINT return code..."));
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fsInitial);
    if( FAILED(hr) && hr != DXGI_ERROR_FRAME_STATISTICS_DISJOINT)
    {
        WriteToLog(_T("RunCase: GetFrameStatistics is failing with %s(0x%x)."), HResultToString(hr), hr);
        tr = RESULT_SKIP;
        goto cleanup;
    }

    WriteToLog(_T("Calling GetFrameStatistics to get initial stats..."));
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fsInitial);
    if( FAILED(hr))
    {
        WriteToLog(_T("RunCase: GetFrameStatistics is failing with %s(0x%x)."), HResultToString(hr), hr);
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Log present stats from initial state
    WriteToLog(_T("******************************"));
    WriteToLog(_T("Initial Present Stats (prior to presenting any frames):"));
    WriteToLog(_T("PresentCount: %u"), fsInitial.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fsInitial.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fsInitial.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fsInitial.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fsInitial.SyncGPUTime);
    WriteToLog(_T("******************************"));


    // Render our baseline
    WriteToLog(_T("Making first Present call..."));
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present #1: Failed"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Reset FrameStats (Clear out initial DISJOINT error and get initial stats)
    WriteToLog(_T("Calling GetFrameStatistics to clear out any second DISJOINT return code when DWM is off..."));
    ZeroMemory( &fs1, sizeof(fs1) );
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
    if( FAILED(hr) && hr != DXGI_ERROR_FRAME_STATISTICS_DISJOINT)
    {
        WriteToLog(_T("RunCase: GetFrameStatistics is failing with %s(0x%x)."), HResultToString(hr), hr);
        tr = RESULT_SKIP;
        goto cleanup;
    }

    WriteToLog(_T("Calling GetFrameStatistics repeatedly until our first present is reflected in the results..."));
    //
    // Wait for GetFrameStats to register our Present call
    //
    for(nTimeout = 0; nTimeout < 10 && fs1.PresentCount<=fsInitial.PresentCount  ; nTimeout++ )
    {
        Sleep(DELAY_BETWEEN_GFS_CALLS);

        ZeroMemory( &fs1, sizeof(fs1) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
        if( FAILED(hr) )
        {
            if (DXGI_ERROR_FRAME_STATISTICS_DISJOINT == hr)
            {
                WriteToLog(_T("RunCase: Present #1: Warning! GetFrameStatistics returned unexpected DXGI_ERROR_FRAME_STATISTICS_DISJOINT on call %d."), nTimeout);
            }
            else
            {
                WriteToLog(_T("RunCase: Present #1: GetFrameStatistics is failing."));
                tr = RESULT_SKIP;
                goto cleanup;
            }
        }

    }

    // Log present stats from first frame
    WriteToLog(_T("******************************"));
    WriteToLog(_T("First Frame Present Stats:"));
    WriteToLog(_T("PresentCount: %u"), fs1.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fs1.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fs1.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fs1.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fs1.SyncGPUTime);
    WriteToLog(_T("******************************"));

    //
    //  If we timed out waiting, log a failure.
    //
    if( 10 == nTimeout )
    {
        WriteToLog(_T("*** FAILURE: RunCase: Present #1: PresentCount is not updating after %u ms. ***"), (nTimeout)*DELAY_BETWEEN_GFS_CALLS);
        tr = RESULT_FAIL;
        goto cleanup;
    }

    //
    // If GetFrameStats didn't reflect our present call after a couple iterations, let's log a warning.  It's clearly taking too long.
    //
    if (nTimeout > 1)
        WriteToLog(_T("RunCase: Present #1: Warning!  It took over %ums for GetFrameStatistics to reflect our present call."), nTimeout*DELAY_BETWEEN_GFS_CALLS );

    // Glitch - Introduce simulated delay between presented frames.
    Sleep(1000);

    // Render our 2nd frame
    WriteToLog(_T("Making second Present call..."));
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present #2: Unable to increment the present count"));
        tr = RESULT_SKIP;
        goto cleanup;
    }


    //
    // Wait for GetFrameStats to register our Present call
    //
    WriteToLog(_T("Calling GetFrameStatistics repeatedly until our second present is reflected in the results..."));
    for( nTimeout = 0;  nTimeout < 10 && fs2.PresentCount <= fs1.PresentCount; nTimeout++ )
    {
        Sleep(DELAY_BETWEEN_GFS_CALLS);

        ZeroMemory( &fs2, sizeof(fs2) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs2);
        if( FAILED(hr) )
        {
            if (DXGI_ERROR_FRAME_STATISTICS_DISJOINT == hr)
            {
                WriteToLog(_T("RunCase: Present #2: Warning! GetFrameStatistics returned unexpected DXGI_ERROR_FRAME_STATISTICS_DISJOINT on call %d."), nTimeout);
            }
            else
            {
                WriteToLog(_T("RunCase: Present #2: GetFrameStatistics is failing."));
                tr = RESULT_SKIP;
                goto cleanup;
            }
        }

    }

    // Log present stats from first frame
    WriteToLog(_T("******************************"));
    WriteToLog(_T("Second Frame Present Stats:"));
    WriteToLog(_T("PresentCount: %u"), fs2.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fs2.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fs2.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fs2.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fs2.SyncGPUTime);
    WriteToLog(_T("******************************"));

    //
    //  If we timed out waiting, log a failure.
    //
    if( 10 == nTimeout )
    {
        WriteToLog(_T("*** FAILURE: RunCase: Present #2: PresentCount is not updating after %u ms. ***"), nTimeout*DELAY_BETWEEN_GFS_CALLS);
        tr = RESULT_FAIL;
        goto cleanup;
    }

    //
    // If GetFrameStats didn't reflect our present call after a couple iterations, let's log a warning.  It's clearly taking too long.
    //
    if (nTimeout > 1)
        WriteToLog(_T("RunCase: Present #2: Warning!  It took over %ums for GetFrameStatistics to reflect our present call."), nTimeout*DELAY_BETWEEN_GFS_CALLS );

    // Find the delta of the 1st and 2nd frame... should only be one.
    UINT FramesCompleteDelta = (UINT)fs2.PresentCount - (UINT)fs1.PresentCount;
    if( 1 != FramesCompleteDelta )
    {
        // Fail. Present count did not update.
        WriteToLog(_T("RunCase: Present count did not update by 1, from 1st frame to 2nd frame. Present #1 ID: %u, Present #2 ID: %u,"), fs1.PresentCount, fs2.PresentCount);
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Find the delta of the refresh count for each presented frame.
    UINT PresentRefreshCountDelta = (UINT)fs2.PresentRefreshCount - (UINT)fs1.PresentRefreshCount;

    // Verify that a glitch did happen.
    if( FramesCompleteDelta < PresentRefreshCountDelta )
    {
        // PASS! frame missed
        WriteToLog(_T("RunCase: GetFrameStatistics properly detected simulated glitch."));
    }
    else
    {
        // FAIL! no frames missed
        WriteToLog(_T("*** FAILURE: RunCase: GetFrameStatistics failed to detect simulated glitch!  Frame Delta: %d, PresentRefrestCount Delta: %d ***"), FramesCompleteDelta, PresentRefreshCountDelta);
        goto cleanup;
    }

    // if we made it here the test passed
    tr = RESULT_PASS;

cleanup:

    ReleaseOwnership(pDXGISwapChain, pDXGIOutput);

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIQueuedFrameTest Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Try to detect a queued frame.
// TODO: Detect a queued frame.
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIQueuedFrameTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    if( DXGISWAPCHAIN != GetCurrentResourceType() )
    {
        WriteToLog(_T("RunCase: Can only run on DXGISWAPCHAIN."));
        return RESULT_SKIP;
    }

    TEST_RESULT tr = RESULT_FAIL;   // case result to return
    HRESULT hr = E_FAIL;            

    // Frame statistices
    DXGI_FRAME_STATISTICS fs1, fs2;              
    ZeroMemory( &fs1, sizeof(fs1) );    // baseline stats        
    ZeroMemory( &fs2, sizeof(fs2) );    // test stats

    // Last present count GetLastPresentCount
    UINT nLPC1 = 0;     // baseline count
    UINT nLPC2 = 0;     // test count

    // Timeout for waiting for fame stats to update
    UINT nTimeout = 0;

    // Take ownership
    if( DXGISWAPCHAIN == GetCurrentResourceType() && FALSE == GetOwnByFullScreen() )
    {
        // We want the swapchain to GetFrameStats without having ownership
        WriteToLog(_T("RunCase: Not taking ownership for this case."));
    }
    else
    {
        hr = TakeOwnership(pDXGISwapChain, pDXGIOutput);
        if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
        {
            WriteToLog(_T("RunCase: Unable to take ownership"));
            tr = RESULT_SKIP;
            goto cleanup;
        }

        // Reset after fullscreen change
        GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
        ZeroMemory( &fs1, sizeof(fs1) );
    }

    // Render a baseline frame to get everything going.
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present1: Unable to increment the present count"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Pause for any outstanding frames
    Sleep(1000);

    // Get baseline frame stats
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1); // This should clear out any initial DISJOINT error
    nTimeout = 0;
    for(; nTimeout < 10 && 0 == fs1.PresentCount; nTimeout++ )
    {
        Sleep(1000);
        ZeroMemory( &fs1, sizeof(fs1) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
        if( FAILED(hr) )
        {
            WriteToLog(_T("RunCase: Present1: GetFrameStatistics is failing."));
            tr = RESULT_SKIP;
            goto cleanup;
        }
    }
    if( 10 == nTimeout )
    {

        WriteToLog(_T("RunCase: Present1: PresentCount is not updating."));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Get baseline LastPresentCount
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, &nLPC1);
    if( FAILED(hr) || 0 == nLPC1)
    {
        WriteToLog(_T("RunCase: Present1: GetLastPresentCount is failing"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Render the 2nd frame
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr || FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present2: Unable to increment the present count"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Get test frame stats
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs2);
    if( FAILED(hr) )
    {
        WriteToLog(_T("RunCase: Present2: GetFrameStatistics is failing."));
        tr = RESULT_SKIP;
        goto cleanup;
    }
    // are we able to call GetFrameStatistics before the present got displayed?
    if( fs1.PresentCount != fs2.PresentCount )
    {
        // nope
        WriteToLog(_T("RunCase: Unable to queue a frame."));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // Get test LastPresentCount
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, &nLPC2);
    if( FAILED(hr) || 0 == nLPC1)
    {
        WriteToLog(_T("RunCase: Present2: GetLastPresentCount is failing"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    if( nLPC1 == nLPC2 || 1 != (nLPC2-nLPC1) )
    {
        // There are other test case that will fail for this.
        WriteToLog(_T("RunCase: GetLastPresentCount is not updating correctly after a call to present"));
        tr = RESULT_SKIP;
        goto cleanup;
    }

    // If we get here we know thee number of queued frams is 1. 
    // as in (nLPC2 - nLPC1) - (fs2.PresentCount - fs1.PresentCount).
    // TODO: Detect more then 1 queued frame.
    tr = RESULT_PASS;

cleanup:

    ReleaseOwnership(pDXGISwapChain, pDXGIOutput);

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIResource_D3D10VB
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: CreateTestResource
// Desc: Creates the resource to test
//-----------------------------------------------------------------------------
HRESULT CDXGIResource_D3D10VB::CreateResource(ID3D10Device* pDevice, CD3D10TestFramework* pTestFramework)
{
    // Verify input
    if( NULL == pDevice || NULL == pTestFramework )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource(): Bad input"));
        return E_POINTER;
    }

    if( NULL != m_pD3D10VB || NULL != m_pDXGIResource || NULL != m_pVS || NULL != m_pPS || NULL != m_pInputLayout)
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource(): Resources not released before call"));
        return E_FAIL;
    }

    HRESULT hr = E_FAIL;

    // Create vertex buffer
    XYZW_VERTEX v[3] = { 0.0f,  0.5f, 0.5f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1.0f };

    D3D10_SUBRESOURCE_DATA sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.pSysMem = v;
    sd.SysMemPitch = 0;
    sd.SysMemSlicePitch = 0;

    D3D10_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(XYZW_VERTEX) * 3;
    bd.Usage = GetUsageFlags();
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.MiscFlags = 0;
    bd.CPUAccessFlags = GetUsageFlags() == D3D10_USAGE_DYNAMIC? D3D10_CPU_ACCESS_WRITE: 0;

    // Create buffer
    hr = pDevice->CreateBuffer( &bd, &sd, &m_pD3D10VB);
    if( FAILED(hr) )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource(): ID3D10Device->CreateBuffer() Failed"));
        return hr;
    }
    if( NULL == m_pD3D10VB )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource(): ID3D10Device->CreateBuffer() Succeed but pVB is NULL!"));
        return E_FAIL;
    }    

    // Get DXGIResource
    hr = SetResourceFromInterface((IUnknown**)&m_pD3D10VB);
    if( FAILED(hr) || NULL == m_pDXGIResource )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: ISetResourceFromInterface failed"));
        return E_FAIL;
    }

    // Setup shaders
    // Compile vertex shader
    CBlob *pShaderBuff = NULL;
    CBlob *pErrorBuff = NULL;
    hr = pTestFramework->CompileShaderFromResource(NULL, _T("vs.vsh"), "VS", "vs_4_0", D3D_SHADER_DEBUG, &pShaderBuff, &pErrorBuff);
    if( FAILED(hr) || NULL == pShaderBuff)
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: CompileShaderFromResource(vs.vsh) failed"));
        ReleaseResource();
        return hr;
    }

    // Create vertex shader
    hr = pDevice->CreateVertexShader(pShaderBuff->GetBufferPointer(), pShaderBuff->GetBufferSize(), &m_pVS);
    if( FAILED(hr) || NULL == m_pVS )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: ID3D10Device->CreateVertexShader() failed"));
        ReleaseResource();
        return hr;
    } 

    // Create the input for the vertex shader
    D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { _T("POSITION"), 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },  
    };
    UINT numElements = sizeof(layout)/sizeof(layout[0]);

    hr = pDevice->CreateInputLayout(layout, numElements, pShaderBuff->GetBufferPointer(), pShaderBuff->GetBufferSize(), &m_pInputLayout);
    if( FAILED(hr) || NULL == m_pInputLayout )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: ID3D10Device->CreateVertexShader() failed"));
        ReleaseResource();
        return hr;
    }

    SAFE_RELEASE(pShaderBuff);
    SAFE_RELEASE(pErrorBuff);

    // Compile pixel shader
    hr = pTestFramework->CompileShaderFromResource(NULL, _T("ps.psh"), "PS", "ps_4_0", D3D_SHADER_DEBUG, &pShaderBuff, &pErrorBuff);
    if( FAILED(hr) || NULL == pShaderBuff )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: CompileShaderFromResource(ps.psh) failed"));
        ReleaseResource();
        return hr;
    }

    // Create pixel shader
    hr = pDevice->CreatePixelShader(pShaderBuff->GetBufferPointer(), pShaderBuff->GetBufferSize(), &m_pPS);
    if( FAILED(hr) || NULL == m_pPS )
    {
        pTestFramework->WriteToLog(_T("CResChk_D3D10VB::CreateResource: ID3D10Device->CreatePixelShader() failed"));
        ReleaseResource();
        return hr;
    }

    SAFE_RELEASE(pShaderBuff);
    SAFE_RELEASE(pErrorBuff);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Function: UseResource
// Desc: Uses the resource. To get the resource into memory.
//-----------------------------------------------------------------------------
BOOL CDXGIResource_D3D10VB::UseResource(ID3D10Device* pDevice)
{    
    // render resource
    UINT stride = sizeof(XYZW_VERTEX);
    UINT offset = 0;
    pDevice->IASetVertexBuffers(0, 1, &m_pD3D10VB, &stride, &offset);
    pDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    pDevice->IASetInputLayout(m_pInputLayout);
    pDevice->VSSetShader(m_pVS);
    pDevice->PSSetShader(m_pPS);
    pDevice->GSSetShader(NULL);
    pDevice->Draw(3,0);

    return TRUE;
}// FILE:        multisample.h
// DESC:        multisample antialiasing/masking conformance test header

#include <d3d9.h>
#include <d3dx9.h>
#include "CD3DTest.h"

#include "CFlags.h"

#define GLE(x) (m_pD3D->HResultToString(x))

#ifndef D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE
#define D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE 0x00800000L
#endif

const D3DCOLOR RED		= 0x00FF0000;
const D3DCOLOR GREEN	= 0x0000FF00;
const D3DCOLOR BLUE		= 0x000000FF;
const D3DCOLOR YELLOW	= 0x00FFFF00;
const D3DCOLOR CYAN		= 0x0000FFFF;
const D3DCOLOR MAGENTA	= 0x00FF00FF;

const D3DCOLOR BLACK = 0x00000000;
const D3DCOLOR GRAY	 = 0x00A0A0A0;
const D3DCOLOR WHITE = 0x00FFFFFF;

const UINT MAXBUFFER = 512; // max generic buffer length

const float SUBPIX_FOURBIT =		0.0625f;
const float SUBPIX_EIGHTBIT =		0.00390625f;
/*
These levels of accuracy would make the test take a rediculous amount of time
In fact, even eightbit is just too long
const float SUBPIX_SIXTEENBIT =		0.0000152587890625f;
const float SUBPIX_TWENTYFOURBIT =	0.000000059604644775390625f;
const float SUBPIX_THIRTYTWOBIT =	0.00000000023283064365386962890625f;
*/

const DWORD FLEXFMT = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ); // FVF format

const UINT TEST_POINT_COUNT = 13;

typedef struct _FlexVert
{
    float fPx;
    float fPy;
    float fPz;
    float fRHW;
    D3DCOLOR Diffuse;
} FLEXVERTEX, *PFLEXVERTEX;

class CMultisample: public CD3DTest
{
private:
	RECT MouseTrap;

protected:
	bool m_bGetSamplePoints;
	bool m_bVerbose;
	void HideMouse();
	void ShowMouse();
	HRESULT GetScene( LOCKED_RECT ** pRect, CSurface ** fbSurf );

public:
    CMultisample();
    ~CMultisample();
	virtual bool DisplayFrame();
    virtual UINT TestInitialize();
	bool ParseCmdLine();
};

class CSamples: public CMultisample
{
private:
	void Render( PFLEXVERTEX pfvSubPix );
	bool QuickTest();
	bool ExhaustiveTest();
	void WasSampled( BOOL * rgbResults );
	UINT SampleCount();
	
	//Diagnostic test
	void GetPointsTest();
	void FindPoints( CSurface *pSurface, UINT X, UINT Y );
	void SaveSurface( CSurface *pSurface );
	void DrawDividers( CSurface *pSurface );

	POINT TestPoint[TEST_POINT_COUNT];

	bool m_bForceSlow;
	float m_fSampleSize;

public:
	CSamples();
	~CSamples();
	void CommandLineHelp();

	UINT TestInitialize();
	bool ExecuteTest(UINT nTest);
	bool ProcessFrame() { return true; };
	bool ParseCmdLine();
};

class CMask: public CMultisample
{
private:
	D3DCOLOR GetColor( D3DCOLOR OldColor );
	bool VerifyDiff( D3DCOLOR * ColorDiff, UINT uiSamples, UINT uiMax );

public:
	CMask();
	~CMask();
	UINT TestInitialize();
	void CommandLineHelp();
	bool ExecuteTest( UINT nTest );
	bool ProcessFrame( void ) { return true; };
};

class CToggle: public CMultisample
{
private:
	void QuickSampleTest( UINT iYOffset );
	void MaskTest();
	void VerifyResults();

	bool m_bMask;

public:
	CToggle();
	~CToggle();
	UINT TestInitialize();
	bool ExecuteTest( UINT nTest );
	bool ProcessFrame( void ) { return true; };
};

class CCentroid: public CMultisample
{
private:
	bool SetupPixelShader( UINT nTest );
	bool SetupRender( UINT nTest );
	bool Render( UINT nTest );
	bool VerifyResults( UINT nTest );
	bool m_bWarning;

	CPShader		*pShader;
	CnTexture		*ppTexture;

public:
	CCentroid();
	~CCentroid();
	UINT TestInitialize();
	bool ExecuteTest( UINT nTest );
	bool ProcessFrame( void ) { return true; };
	bool Setup( void );
	void Cleanup();
};
// geometry shader to select render target array index
struct GSIn
{
	float4 pos		: SV_Position;
	float  odepth   : odepth;
};

struct GSOut
{
	float4 pos		 : SV_Position;
	float  odepth    : odepth;
	uint   rt_index  : SV_RenderTargetArrayIndex;
};

cbuffer cb0 : register( b0 )
{	
	float4		inv_vp;		    // inverse viewport
	float		default_depth;  // used when odepth is enabled
	uint		rt_index;		// render target array index
};

[maxvertexcount(3)]
void main(inout TriangleStream<GSOut> stream, triangle GSIn input[3])
{
	GSOut output;
	output.rt_index = rt_index;
	
	output.pos = input[0].pos;
	output.odepth = input[0].odepth;
	stream.Append(output);

	output.pos = input[1].pos;
	output.odepth = input[1].odepth;
	stream.Append(output);
	
	output.pos = input[2].pos;
	output.odepth = input[2].odepth;
	stream.Append(output);	
}#include "precomp.hpp"
#include "wdiapi.tmh"

extern ULONG       g_DebugDllRefCount;

NDIS_STATUS
Wdi_NdisMRegisterWdiMiniportDriver(
	_In_     PDRIVER_OBJECT                              DriverObject,
	_In_     PUNICODE_STRING                             RegistryPath,
	_In_opt_ NDIS_HANDLE                                 MiniportDriverContext,
	_In_     PNDIS_MINIPORT_DRIVER_CHARACTERISTICS       MiniportDriverCharacteristics,
	_In_     PNDIS_MINIPORT_DRIVER_WDI_CHARACTERISTICS   MiniportWdiCharacteristics,
	_Out_    PNDIS_HANDLE                                NdisMiniportDriverHandle
	)
{
    UINT ndisVersion = NdisGetVersion();

    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    *NdisMiniportDriverHandle = NULL;
    
    if (ndisVersion >= NDIS_RUNTIME_VERSION_650)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WDIWiFi: Wdi_NdisMRegisterWdiMiniportDriver should NOT be called on this OS\n");
        return NDIS_STATUS_NOT_ACCEPTED;
    }
    
    if (g_pWdiDriver == NULL)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "WDIWiFi: DriverEntry NOT called with NdisMRegisterWdiMiniportDriver\n");    
        return NDIS_STATUS_NOT_ACCEPTED;
    }
    g_DebugDllRefCount++;

    ndisStatus = g_pWdiDriver->RegisterWdiMiniportDriver(
            DriverObject,
            RegistryPath,
            MiniportDriverContext,
            MiniportDriverCharacteristics,
            MiniportWdiCharacteristics,
            NdisMiniportDriverHandle
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to Register IHV's Miniport Driver. Error = %08x", ndisStatus);
        g_DebugDllRefCount--;
        goto exit;
    }

exit:
    
    return ndisStatus;
}

VOID
Wdi_NdisMDeregisterWdiMiniportDriver(
	_In_ NDIS_HANDLE              NdisMiniportDriverHandle
	)
{
    g_pWdiDriver->DeregisterWdiMiniportDriver(NdisMiniportDriverHandle);

    g_DebugDllRefCount--;    
}


VOID
Wdi_NdisMOidRequestComplete(
    _In_  NDIS_HANDLE             MiniportAdapterHandle,
    _In_  PNDIS_OID_REQUEST       Request,
    _In_  NDIS_STATUS             Status
    )
{
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT(pAdapter);
    
    pAdapter->OnOidRequestCompleteFromMiniport(Request, Status);
    
    TraceExit(Status);
}

EXPORT
VOID
Wdi_NdisMIndicateStatusEx(
    _In_ NDIS_HANDLE              MiniportAdapterHandle,
    _In_ PNDIS_STATUS_INDICATION  StatusIndication
    )
{
    CAdapter *pAdapter = NULL;
    
    TraceEntry();

    pAdapter = g_pWdiDriver->GetAdapterFromAdapterHandle(MiniportAdapterHandle);
    NT_ASSERT(pAdapter);
    
    pAdapter->OnStatusIndicationFromMiniport(StatusIndication);
    
    TraceExit(NDIS_STATUS_SUCCESS);
}
#include "include.fx"

Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1.xyzw, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1.500000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 1.500000, 1.500000, 1.500000, 1.500000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 3.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 3.000000, 3.000000, 3.000000, 3.000000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = -2.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -2.000000, -2.000000, -2.000000, -2.000000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = -2.500000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -2.500000, -2.500000, -2.500000, -2.500000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/2.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/-3.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -0.333333, -0.333333, -0.333333, -0.333333
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/8";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/16";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/64";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 1;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 3;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 5;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MaxMipLevel[0] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 6;
        MipMapLODBias[0] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 1;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 2;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 3;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 4;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 5;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1, MipMapLODBias[0] = 6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 6;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -1";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -2";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -3";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -4";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -5";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex0, w = 1/32, MipMapLODBias[0] = -6";
	String	Shape = "TexTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
 		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.03125, 0.03125, 0.03125, 0.03125
            dcl_2d s0
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s0
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, writemask = xyzw";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1.xyzw, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 3.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 3.000000, 3.000000, 3.000000, 3.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 6.000000, 6.000000, 6.000000, 6.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = -4.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -4.000000, -4.000000, -4.000000, -4.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = -5.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -5.000000, -5.000000, -5.000000, -5.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/4.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/-6.000000";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = None;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
			def  c0, -0.166667, -0.166667, -0.166667, -0.166667
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.000000, 1.000000, 1.000000, 1.000000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.500000, 0.500000, 0.500000, 0.500000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.250000, 0.250000, 0.250000, 0.250000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/8";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.125000, 0.125000, 0.125000, 0.125000
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/16";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.062500, 0.062500, 0.062500, 0.062500
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/32";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.031250, 0.031250, 0.031250, 0.031250
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1/64";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 0.015625, 0.015625, 0.015625, 0.015625
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 1;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 2;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 3;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 4;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 5;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def