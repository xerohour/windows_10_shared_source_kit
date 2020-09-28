/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPresentPnP.cpp
//
//	Abstract:
//	This file contains implementation of DXGIHybridPresentPnP tests
//
//	History:
//	1/14/2013	MarMel	Created.
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "DXGIHybridPresentPnPStop.h"
#include "TestApp.h"

#pragma warning(disable: 4355)


TEST_RESULT CHybridPresentPnPStop::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;
    HANDLE hPnPStopThread = NULL;
    PnPStopThreadParams params;
    
    DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - start");

    // Launch TDR thread
    DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - about to launch PnP thread");
    {
    CSLocker Lock(params.csCritSection);
    params.bIGPU = false;
    params.bDGPU = false;
    params.pbExpectDeviceLost = &m_bExpectDeviceLoss;
    params.pbStopNow = &m_bStopNow;
    params.action = (int) m_HelperThreadScenario;
    params.hr = S_OK;
    params.pETWMaster = GetETWMaster();
    ZeroMemory(params.strError, sizeof(params.strError));
    }
    
    HANDLE hStartThreadEvent = CreateEvent(NULL, FALSE, FALSE, START_THREAD_EVENT);
    HANDLE hStopThreadEvent = CreateEvent(NULL, FALSE, FALSE, STOP_THREAD_EVENT);

    if(!hStartThreadEvent || !hStopThreadEvent)
    {
        tr = RESULT_ABORT;
        WriteToLog(_T("CHybridPresentPnPStop::ExecuteTestCase() - ABORT: Failed to create sync events, error = %d"), GetLastError());
    }

    if(tr == RESULT_PASS)
    {
        hPnPStopThread = CreateThread(0, 0, PnPStopThread, &params, 0, 0);

        if(!hPnPStopThread)
        {
            tr = RESULT_ABORT;
            WriteToLog(_T("CHybridPresentPnPStop::ExecuteTestCase() - ABORT: Failed to create TDR Thread, error = %d"), GetLastError());
        }
    }


    if(tr == RESULT_PASS)
    {
        DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - successfully launched PnPStop thread");
        // Wait for thread to start
        if(WAIT_OBJECT_0 != WaitForSingleObject(hStartThreadEvent, TDR_THREAD_WAIT_TIME*2))
        {
            tr = RESULT_ABORT;
            WriteToLog(_T("CHybridPresentPnPStop::ExecuteTestCase() - ABORT: PnPStop Thread failed to start"));
        }
    }

    if(tr == RESULT_PASS)
    {
        DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - PnPStop thread successfully started");
        try
        {
            m_NumberOfPresents = m_BufferCount;

            if(m_StartFullScreen)
                m_NumberOfPresents *= 2;

            m_TexturesToLoad = m_BufferCount;

            m_dwPresentTime = g_TimeToExecute;

            DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - starting execution of the main test");
            tr = CPresentWinConvertStretch::ExecuteTestCase();
            DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - finished execution of the main test");

        }
        catch(...)
        {
            DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - caught exception while executing main test");
            {
            CSLocker Lock(params.csCritSection);

            WriteToLog(_T("CHybridPresentPnPStop::ExecuteTestCase() - ERROR: Unhandled exception in CHybridPresentRedirection::ExecuteTestCase(), bResettingIGPU = %s, bResettingDGPU = %s, hr = 0x%X, error from PnPStop thread = %s; stopping TDR thread"),
                        (params.bIGPU ? "true" : "false"),
                        (params.bDGPU ? "true" : "false"),
                         params.hr,
                         params.strError);

            }

            SetEvent(hStopThreadEvent);
            CloseHandle(hStopThreadEvent);
            hStopThreadEvent = NULL;

            DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - stopping PnPStop thread");
            WaitForSingleObject(hPnPStopThread, INFINITE);
            DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - stopped PnPStop thread");
            CloseHandle(hPnPStopThread);
            hPnPStopThread = NULL;

            // pass exception further
            throw;
        }
    }

    // Check how's PnPStop thread is doing
    {
    DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - checking PnPStop thread status");
    CSLocker Lock(params.csCritSection);

    if(FAILED(params.hr))
    {
        WriteToLog(_T("CHybridPresentPnPStop::ExecuteTestCase() - ERROR: Failure is detected on PnPStop thread, bResettingIGPU = %s, bResettingDGPU = %s, hr = 0x%X, error from PnPStop thread = %s"),
                    (params.bIGPU ? "true" : "false"),
                    (params.bDGPU ? "true" : "false"),
                    params.hr,
                    params.strError);
        tr = RESULT_FAIL;
    }
    }

    if(hStopThreadEvent)
    {
        DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - signaling PnPStop thread to finish");
        SetEvent(hStopThreadEvent);
        CloseHandle(hStopThreadEvent);
        hStopThreadEvent = NULL;
    }

    if(hStartThreadEvent)
    {
        SetEvent(hStartThreadEvent);
        CloseHandle(hStartThreadEvent);
        hStartThreadEvent = NULL;
    }

    if(hPnPStopThread)		
    {
        DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - waiting for PnPStop thread to finish");
        WaitForSingleObject(hPnPStopThread, INFINITE);
        DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - PnPStop thread exited");
        CloseHandle(hPnPStopThread);
        hPnPStopThread = NULL;
    }

    DBGPrint(L"CHybridPresentPnPStop::ExecuteTestCase - returning");
    return tr;
}

DWORD WINAPI PnPStopThread(LPVOID _PnPStopThreadParams)
{
    DBGPrint(L"PnPStopThread - Enter");

    PnPStopThreadParams* pParams = (PnPStopThreadParams*) _PnPStopThreadParams;
    AdapterUtil* pAdapterUtil = NULL;
    HANDLE hStartThreadEvent = NULL;
    HANDLE hStopThreadEvent = NULL;
    AdapterDescriptor* pIntegrated = NULL;
    AdapterDescriptor* pDiscrete = NULL;
    TCHAR strError[ERROR_SIZE] = _T("");

    try
    {
        // Enumerate all devices
        pAdapterUtil = AdapterUtil::CreateAdapterUtil();
        vector<AdapterDescriptor*> vAdapters = pAdapterUtil->GetAllAdapters();
        UINT uiAdapterCount                  = 0;
    
        for (AdapterDescriptor* pAdapter : vAdapters)
        {
            bool bSoftwareDevice = false;

            // skip BRD
            pAdapter->IsSoftwareDevice(bSoftwareDevice);
            if (bSoftwareDevice)
            {
               continue;
            }

            bool bHybridDiscrete = false;

            if(SUCCEEDED(pAdapter->IsHybridDiscrete(bHybridDiscrete)))
            {
                if(bHybridDiscrete)
                    pDiscrete = pAdapter;
                else
                    pIntegrated = pAdapter;
            }
            else
            {
                CSLocker Lock(pParams->csCritSection);
                pParams->hr = E_INVALIDARG;
                *(pParams->pbStopNow) = true;
                StringCchPrintf(pParams->strError, ERROR_SIZE, "%s", _T("PnPStopThread -Unable to determine adapter type"));
                DBGPrint(L"PnPStopThread -Unable to determine adapter type");
                if(pAdapterUtil)
                    delete pAdapterUtil;

                return -1;
            }

            uiAdapterCount++;
        }

        DBGPrint(L"PnPStopThread - Enumerated %d devices", uiAdapterCount);

        if(uiAdapterCount != 2 || !pDiscrete || !pIntegrated)
        {
            CSLocker Lock(pParams->csCritSection);
            pParams->hr = E_INVALIDARG;
            *(pParams->pbStopNow) = true;

            StringCchPrintf(pParams->strError, ERROR_SIZE, _T("PnPStopThread - Enumerated devices don't meet expected configuration: AdapterCount = %d (expected %d), Discrete device count = %d (expected %d), Integrated device count = %d (expected %d)"),
                    uiAdapterCount,
                    2,
                    (pDiscrete) ? 1 : 0,
                    1,
                    (pIntegrated) ? 1 : 0,
                    1);

            if(pAdapterUtil)
                delete pAdapterUtil;

            return -1;
        }

        // create sync Events
        hStartThreadEvent = CreateEvent(NULL, FALSE, FALSE, START_THREAD_EVENT);
        hStopThreadEvent = CreateEvent(NULL, FALSE, FALSE, STOP_THREAD_EVENT);
        if(!hStartThreadEvent || !hStopThreadEvent)
        {
            CSLocker Lock(pParams->csCritSection);
            StringCchPrintf(pParams->strError, ERROR_SIZE, _T("PnPStopThread - Failed to create synchronization events, error = %d"), GetLastError());
            pParams->hr = E_FAIL;
            *(pParams->pbStopNow) = true;

            if(pAdapterUtil)
                delete pAdapterUtil;

            return -1;
        }

        // Set Event to let main thread know that we are ready
        SetEvent(hStartThreadEvent);
         DBGPrint(L"PnPStopThread - Signaled main thread - Start");

        Sleep(1000); // Give main thread a chance to initialize and start presenting
        //srand(123);
        int action = -1;

        DBGPrint(L"PnPStopThread - Starting PnPStop loop");
        do
        {
            UINT action = 0;

            {
                CSLocker Lock(pParams->csCritSection);
                action = pParams->action;
            }

            switch(action)
            {
            case 0:
                {
                    DBGPrint(L"PnPStopThread - PnPStop/wait/PnPStart iGPU adapter");

                    // When one of GPUs in Hybrid mode stops we expect application to re-create device and be in non-hybrid mode
                    // Reset event counters after PnPStop and validate that we only get regular presents when GPU is Stopped and
                    // CrossAdaper Presents when it is restarted
                    UINT RegularPresentsCount = 0;
                    UINT CrossAdapterPresentsCount = 0;

                    {
                    CSLocker Lock(pParams->csCritSection);
                    pParams->bDGPU = false;
                    pParams->bIGPU = true;

                    *(pParams->pbExpectDeviceLost) = true;
                    }

                    if(pIntegrated->PnPDisable())
                    {
                        DBGPrint(L"PnPStopThread - successfully stopped iGPU");
                        pParams->pETWMaster->WaitForEvents();
                        pParams->pETWMaster->ResetPresentCounters();
                        Sleep(PNPSTOP_WAIT_TIME);

                        pParams->pETWMaster->WaitForEvents();
                        CrossAdapterPresentsCount = pParams->pETWMaster->GetCrossAdapterPresentsCount();
                        RegularPresentsCount = pParams->pETWMaster->GetRegularPresentsCount();

                        DBGPrint(L"PnPStopThread - saw %d Regular presents and %d CrossAdapter presents while iGPU is stopped", RegularPresentsCount, CrossAdapterPresentsCount);

                        if(CrossAdapterPresentsCount != 0)
                        {
                            StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - ERROR: Got %d CrossAdapter presents after stopping iGPU"), CrossAdapterPresentsCount);
                            DBGPrint(L"PnPStopThread - ERROR: Got %d CrossAdapter presents after stopping iGPU", CrossAdapterPresentsCount);
                            goto PnPStopThreadError;
                        }

                        if(!pIntegrated->PnPEnable())
                        {
                            StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - failed to start iGPU, error = %d"), GetLastError());
                            DBGPrint(L"PnPStopThread - failed to start iGPU, error = %d", GetLastError());
                            goto PnPStopThreadError;
                        }
                        else
                        {
                            DBGPrint(L"PnPStopThread - successfully re-started iGPU");
                            pParams->pETWMaster->WaitForEvents();
                            pParams->pETWMaster->ResetPresentCounters();
                            Sleep(PNPSTOP_WAIT_TIME);

                            pParams->pETWMaster->WaitForEvents();
                            CrossAdapterPresentsCount = pParams->pETWMaster->GetCrossAdapterPresentsCount();
                            RegularPresentsCount = pParams->pETWMaster->GetRegularPresentsCount();

                            DBGPrint(L"PnPStopThread - saw %d Regular presents and %d CrossAdapter presents after iGPU had been re-started", RegularPresentsCount, CrossAdapterPresentsCount);

                            if(RegularPresentsCount != 0)
                            {
                                StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - ERROR: Got %d regular presents after re-starting iGPU"), RegularPresentsCount);
                                DBGPrint(L"PnPStopThread - ERROR: Got %d regular presents after re-starting iGPU", RegularPresentsCount);
                                goto PnPStopThreadError;
                            }
                        }
                    }
                    else
                    {
                        StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - failed to stop iGPU, error = %d"), GetLastError());
                        DBGPrint(L"TDRThread - failed to stop iGPU, error = %d", GetLastError());
                        goto PnPStopThreadError;
                    }

                    DBGPrint(L"PnPStopThread - Successfully Stopped and Startede iGPU");
                    break;
                }
            case 1:
                 {
                    DBGPrint(L"PnPStopThread - PnPStop/wait/PnPStart dGPU adapter");

                    // When one of GPUs in Hybrid mode stops we expect application to re-create device and be in non-hybrid mode
                    // Reset event counters after PnPStop and validate that we only get regular presents when GPU is Stopped and
                    // CrossAdaper Presents when it is restarted
                    UINT RegularPresentsCount = 0;
                    UINT CrossAdapterPresentsCount = 0;

                    {
                    CSLocker Lock(pParams->csCritSection);
                    pParams->bDGPU = true;
                    pParams->bIGPU = false;

                    *(pParams->pbExpectDeviceLost) = true;
                    }

                    if(pDiscrete->PnPDisable())
                    {
                        DBGPrint(L"PnPStopThread - successfully stopped dGPU");
                        pParams->pETWMaster->WaitForEvents();
                        pParams->pETWMaster->ResetPresentCounters();
                        Sleep(PNPSTOP_WAIT_TIME);

                        pParams->pETWMaster->WaitForEvents();
                        CrossAdapterPresentsCount = pParams->pETWMaster->GetCrossAdapterPresentsCount();
                        RegularPresentsCount = pParams->pETWMaster->GetRegularPresentsCount();

                        DBGPrint(L"PnPStopThread - saw %d Regular presents and %d CrossAdapter presents while dGPU is stopped", RegularPresentsCount, CrossAdapterPresentsCount);

                        if(CrossAdapterPresentsCount != 0)
                        {
                            StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - ERROR: Got %d CrossAdapter presents after stopping dGPU"), CrossAdapterPresentsCount);
                            DBGPrint(L"PnPStopThread - ERROR: Got %d CrossAdapter presents after stopping dGPU", CrossAdapterPresentsCount);
                            goto PnPStopThreadError;
                        }

                        if(!pDiscrete->PnPEnable())
                        {
                            StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - failed to start dGPU, error = %d"), GetLastError());
                            DBGPrint(L"PnPStopThread - failed to start dGPU, error = %d", GetLastError());
                            goto PnPStopThreadError;
                        }
                        else
                        {
                            DBGPrint(L"PnPStopThread - successfully re-started dGPU");
                            pParams->pETWMaster->WaitForEvents();
                            pParams->pETWMaster->ResetPresentCounters();
                            Sleep(PNPSTOP_WAIT_TIME);

                            pParams->pETWMaster->WaitForEvents();
                            CrossAdapterPresentsCount = pParams->pETWMaster->GetCrossAdapterPresentsCount();
                            RegularPresentsCount = pParams->pETWMaster->GetRegularPresentsCount();

                            DBGPrint(L"PnPStopThread - saw %d Regular presents and %d CrossAdapter presents after dGPU had been re-started", RegularPresentsCount, CrossAdapterPresentsCount);

                            if(CrossAdapterPresentsCount != 0)
                            {
                                StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - ERROR: Got %d cross-adapter presents after re-starting dGPU"), CrossAdapterPresentsCount);
                                DBGPrint(L"PnPStopThread - ERROR: Got %d cross-adapter presents after re-starting dGPU", CrossAdapterPresentsCount);
                                goto PnPStopThreadError;
                            }
                        }
                    }
                    else
                    {
                        StringCchPrintf(strError, ERROR_SIZE, _T("TDRThread - failed to stop dGPU, error = %d"), GetLastError());
                        DBGPrint(L"TDRThread - failed to stop dGPU, error = %d", GetLastError());
                        goto PnPStopThreadError;
                    }

                    DBGPrint(L"PnPStopThread - Successfully Stopped and Started dGPU");
                    break;
                }
            case 2:
                 {
                    DBGPrint(L"PnPStopThread - PnPStop/wait/PnPStart iGPU and dGPU adapters");

                    // When one of GPUs in Hybrid mode stops we expect application to re-create device and be in non-hybrid mode
                    // Reset event counters after PnPStop and validate that we only get regular presents when GPU is Stopped and
                    // CrossAdaper Presents when it is restarted
                    UINT RegularPresentsCount = 0;
                    UINT CrossAdapterPresentsCount = 0;

                    {
                    CSLocker Lock(pParams->csCritSection);
                    pParams->bDGPU = true;
                    pParams->bIGPU = true;

                    *(pParams->pbExpectDeviceLost) = true;
                    }

                    if(pDiscrete->PnPDisable())
                    {
                        DBGPrint(L"PnPStopThread - successfully stopped dGPU");
                        Sleep(5000);

                        if(pIntegrated->PnPDisable())
                        {
                            DBGPrint(L"PnPStopThread - successfully stopped iGPU");
                            pParams->pETWMaster->WaitForEvents();
                            pParams->pETWMaster->ResetPresentCounters();
                            Sleep(PNPSTOP_WAIT_TIME);

                            pParams->pETWMaster->WaitForEvents();
                            CrossAdapterPresentsCount = pParams->pETWMaster->GetCrossAdapterPresentsCount();
                            RegularPresentsCount = pParams->pETWMaster->GetRegularPresentsCount();

                            DBGPrint(L"PnPStopThread - saw %d Regular presents and %d CrossAdapter presents while iGPU is stopped", RegularPresentsCount, CrossAdapterPresentsCount);

                            if(CrossAdapterPresentsCount != 0)
                            {
                                StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - ERROR: Got %d CrossAdapter presents after stopping iGPU and dGPU"), CrossAdapterPresentsCount);
                                DBGPrint(L"PnPStopThread - ERROR: Got %d CrossAdapter presents after stopping iGPU and dGPU", CrossAdapterPresentsCount);
                                goto PnPStopThreadError;
                            }

                            if(!pDiscrete->PnPEnable())
                            {
                                StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - failed to start dGPU, error = %d"), GetLastError());
                                DBGPrint(L"PnPStopThread - failed to start dGPU, error = %d", GetLastError());
                                goto PnPStopThreadError;
                            }
                            else
                            {
                                DBGPrint(L"PnPStopThread - successfully re-started dGPU");

                                Sleep(5000);

                                if(!pIntegrated->PnPEnable())
                                {
                                    StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - failed to start iGPU, error = %d"), GetLastError());
                                    DBGPrint(L"PnPStopThread - failed to start iGPU, error = %d", GetLastError());
                                    goto PnPStopThreadError;
                                }

                                DBGPrint(L"PnPStopThread - successfully re-started iGPU");
                                pParams->pETWMaster->WaitForEvents();
                                pParams->pETWMaster->ResetPresentCounters();
                                Sleep(PNPSTOP_WAIT_TIME);

                                pParams->pETWMaster->WaitForEvents();
                                CrossAdapterPresentsCount = pParams->pETWMaster->GetCrossAdapterPresentsCount();
                                RegularPresentsCount = pParams->pETWMaster->GetRegularPresentsCount();

                                DBGPrint(L"PnPStopThread - saw %d Regular presents and %d CrossAdapter presents after iGPU and dGPU had been re-started", RegularPresentsCount, CrossAdapterPresentsCount);

                                if(RegularPresentsCount != 0)
                                {
                                    StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - ERROR: Got %d regular presents after re-starting iGPU and dGPU"), RegularPresentsCount);
                                    DBGPrint(L"PnPStopThread - ERROR: Got %d regular presents after re-starting iGPU and dGPU", RegularPresentsCount);
                                    goto PnPStopThreadError;
                                }
                            }
                        }
                        else
                        {
                            StringCchPrintf(strError, ERROR_SIZE, _T("TDRThread - failed to stop iGPU, error = %d"), GetLastError());
                            DBGPrint(L"TDRThread - failed to stop iGPU, error = %d", GetLastError());
                            goto PnPStopThreadError;
                        }

                    }
                    else
                    {
                        StringCchPrintf(strError, ERROR_SIZE, _T("TDRThread - failed to stop dGPU, error = %d"), GetLastError());
                        DBGPrint(L"TDRThread - failed to stop dGPU, error = %d", GetLastError());
                        goto PnPStopThreadError;
                    }

                    DBGPrint(L"PnPStopThread - Successfully Stopped and Startede iGPU");
                    break;
                }
            default:
                {
                    StringCchPrintf(strError, ERROR_SIZE, _T("PnPStopThread - invalid action"));
                    DBGPrint(L"PnPStopThread - invalid action");
                    break;
                }
            }
        }
        while(WAIT_TIMEOUT == WaitForSingleObject(hStopThreadEvent, PNPSTOP_WAIT_TIME));

    }
    catch(...)
    {
        DBGPrint(L"PnPStopThread - exception is caught in PnPStop thread");
        CSLocker Lock(pParams->csCritSection);
        if(SUCCEEDED(pParams->hr))
        {
            StringCchPrintf(pParams->strError, ERROR_SIZE, _T("PnPStopThread - exception is caught in PnPStop thread"));
            pParams->hr = E_FAIL;
        }
        *(pParams->pbStopNow) = true;

        if(pAdapterUtil)
            delete pAdapterUtil;

        return -1;
    }

    if(pAdapterUtil)
        delete pAdapterUtil;

    if(hStopThreadEvent)
    {
        CloseHandle(hStopThreadEvent);
        hStopThreadEvent = NULL;
    }

    if(hStartThreadEvent)
    {
        CloseHandle(hStartThreadEvent);
        hStartThreadEvent = NULL;
    }

    DBGPrint(L"PnPStopThread - Exiting");
    return 1;

PnPStopThreadError:
    DBGPrint(L"PnPStopThread - Exiting with error");

    if(pIntegrated)
        pIntegrated->PnPEnable();

    if(pDiscrete)
        pDiscrete->PnPEnable();

    CSLocker Lock(pParams->csCritSection);
    if(SUCCEEDED(pParams->hr))
    {
        StringCchPrintf(pParams->strError, ERROR_SIZE, _T("%s"), strError);
        pParams->hr = E_FAIL;
    }
    *(pParams->pbStopNow) = true;

    if(pAdapterUtil)
        delete pAdapterUtil;

    if(hStopThreadEvent)
    {
        CloseHandle(hStopThreadEvent);
        hStopThreadEvent = NULL;
    }

    if(hStartThreadEvent)
    {
        CloseHandle(hStartThreadEvent);
        hStartThreadEvent = NULL;
    }

    return -1;
}
