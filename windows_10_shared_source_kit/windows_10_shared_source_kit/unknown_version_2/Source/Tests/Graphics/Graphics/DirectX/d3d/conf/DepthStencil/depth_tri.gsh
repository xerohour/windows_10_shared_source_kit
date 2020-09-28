sentCount: %u"), fs2.PresentCount);
    WriteToLog(_T("PresentRefreshCount: %u"), fs2.PresentRefreshCount);
    WriteToLog(_T("SyncRefreshCount: %u"), fs2.SyncRefreshCount);
    WriteToLog(_T("SyncQPCTime: %lu"), fs2.SyncQPCTime);
    WriteToLog(_T("SyncGPUTime: %lu"), fs2.SyncGPUTime);


    // PresentCount should have incremented
    if( (fs1.PresentCount + 1) != fs2.PresentCount )
    {
        WriteToLog(_T("*** FAILURE: Present count incrementing by more then one ***"));
        return RESULT_FAIL;
    }

    // PresentRefreshCount should have incremented
    if( fs1.PresentRefreshCount >= fs2.PresentRefreshCount )
    {
        WriteToLog(_T("*** FAILURE: PresentRefreshCount is not incrementing ***"));
        return RESULT_FAIL;
    }

    // SyncRefreshCount should have incremented
    if( fs1.SyncRefreshCount >= fs2.SyncRefresh