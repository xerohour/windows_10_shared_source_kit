Count )
    {
        WriteToLog(_T("*** FAILURE: SyncRefreshCount is not incrementing ***"));
        return RESULT_FAIL;
    }

    // SyncQPCTime should have incremented
    if( fs1.SyncQPCTime.QuadPart == fs2.SyncQPCTime.QuadPart )
    {
        WriteToLog(_T("*** FAILURE: SyncQPCTime.QuadPart is not incrementing ***"));
        return RESULT_FAIL;
    }

    // SyncGPUTime should have incremented.. if not full screen
    /* GPUTime is not yet supported in full-screen or under DWM.
    if( TRUE == GetOwnByFullScreen() )
    {
    if( fs1.SyncGPUTime.QuadPart == fs2.SyncGPUTime.QuadPart )
    {
    WriteToLog(_T("SyncGPUTime.QuadPart is not incrementing"));
    return RESULT_FAIL;
    }  
    }*/

    return tr;
}

//-----------------------------------------------------------------------------
// The CDXGIGetLastPresentCountIncrementTest Test
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: RunCase
// Desc: Verify that present will up the count returned by GetLastPresentCount
//-----------------------------------------------------------------------------
TEST_RESULT CDXGIGetLastPresentCountIncrementTest::RunCase(IDXGISwapChain *pDXGISwapChain, IDXGIOutput *pDXGIOutput)
{
    // Verify input
    LOG_RETURN_ON_NULL(pDXGISwapChain, RESULT_FAIL, _T("RunCase: Bad input (DXGISwapChain = NULL)"));
    if( DXGISWAPCHAIN != GetCurrentResourceType() )
    {
        WriteToLog(_T("Can only run on DXGISWAPCHAIN."));
        return RESULT_SKIP;
    }

    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = E_FAIL;

    // PresentCount before and after present
    UINT nPresentCountBeforePresent = 0;
    UINT nPresentCountAfterPresent = 0;

    // Get frame present count
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, &nPresentCountBeforePresent);
    if( FAILED(hr) )
    {
        WriteToLog(_T("*** FAILURE: GetLastPresentCount() failed before present ***"));
        return RESULT_FAIL;
    }

    // Up the present count
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        WriteToLog(_T("Test is occluded - Unable to increment the present count"));
        return RESULT_SKIP;
    }
    else if( FAILED(hr) )
    {
        WriteToLog(_T("*** FAILURE: Unable to increment the present count ***"));
        return RESULT_FAIL;
    }

    // Get frame statistics after present
    hr = GetLastPresentCount(pDXGISwapChain, pDXGIOutput, &nPresentCountAfterPresent);
    if( FAILED(hr) )
    {
        WriteToLog(_T("*** FAILURE: GetLastPresentCount() failed after present ***"));
        return RESULT_FAIL;
    }

    // PresentCount should have incremented
    if( nPresentCountBeforePresent == nPresentCountAfterPresent )
    {
        WriteTo