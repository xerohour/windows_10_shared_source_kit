FrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1); // Ignore any error returns

    WriteToLog(_T("Waiting for GetFrameStatistics to reflect first Present..."));

    //
    // Call GetFrameStatistics until our first present is reflected in the 
    // PresentCount or we time out.
    //
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
    if (FAILED(hr))
    {
        WriteToLog(_T("Unable to get baseline frame statistics"));
        ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
        if( DXGI_STATUS_OCCLUDED == hr )
            return RESULT_SKIP;

        return RESULT_FAIL;
    }

    nTimeout = 0;
    while( 0 == fs1.PresentCount )
    {
        Sleep(1000);
        ZeroMemory( &fs1, sizeof(fs1) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs1);
        if( FAILED(hr) )
        {
            WriteToLog(_T("Unable to get baseline frame statistics"));
            ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
            if( DXGI_STATUS_OCCLUDED == hr )
                return RESULT_SKIP;

            return RESULT_FAIL;
        }
        if(nTimeout++ > 9 )
        {
            ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
            WriteToLog(_T("*** FAILURE: Present count is not incrementing after trying repeatedly for %u seconds ***"), nTimeout);
            return RESULT_FAIL;
        }
    }

    // Log present stats from first frame
    WriteToLog(_T("First Frame Present Stats:"));
    WriteToLog(_T("PresentCount: %u"), fs1.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fs1.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fs1.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fs1.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fs1.SyncGPUTime);


    // Up the present count
    WriteToLog(_T("Presenting second frame..."));
    hr = IncrementPresentCount(pDXGISwapChain);
    if( DXGI_STATUS_OCCLUDED == hr )
    {
        WriteToLog(_T("Test is occluded - Unable to increment the present count"));
        ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
        return RESULT_SKIP;
    }
    else if( FAILED(hr) )
    {
        WriteToLog(_T("Unable to increment the present count"));
        ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
        return RESULT_FAIL;
    }

    // Pause for frame to be output and get test present stats
    WriteToLog(_T("Waiting for GetFrameStatistics to reflect second Present..."));
    Sleep(1000);
    hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs2);
    nTimeout = 0;
    while( fs2.PresentCount <= fs1.PresentCount )
    {
        if( 9 < nTimeout )
        {
            ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
            WriteToLog(_T("Present count is not incrementing"));
            return RESULT_FAIL;
        }
        Sleep(1000);
        ZeroMemory( &fs2, sizeof(fs2) );
        hr = GetFrameStatistics(pDXGISwapChain, pDXGIOutput, &fs2);
        if( FAILED(hr) )
        {
            WriteToLog(_T("Unable to get test frame statistics"));
            ReleaseOwnership(pDXGISwapChain, pDXGIOutput);
            if( DXGI_STATUS_OCCLUDED == hr )
                return RESULT_SKIP;

            return RESULT_FAIL;
        }
        nTimeout++;
    }

    // Release ownership
    ReleaseOwnership(pDXGISwapChain, pDXGIOutput);

    // Log present stats from second frame
    WriteToLog(_T("Second Frame Present Stats:"));
    WriteToLog(_T("Pre