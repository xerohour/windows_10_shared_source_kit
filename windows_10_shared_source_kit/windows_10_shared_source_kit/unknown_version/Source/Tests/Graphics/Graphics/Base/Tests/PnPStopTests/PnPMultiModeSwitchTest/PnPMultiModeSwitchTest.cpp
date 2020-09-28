#include "PnPMultiModeSwitchTest.h"

CModeSwitch::CModeSwitch()
{
    SetDescription(L"Transitions between windowed and full screen mode.");

    AddParam<UINT>(L"Repetitions", L"The number of times the mode will be switched.");
    SetParamTestDomainSingleValue<UINT>(L"Repetitions", 10);
}


void CModeSwitch::PreWalk(CLogWriter* pLog)
{
    m_nThreadsActive = 0;
    ThreadData threadData;

    threadData.numRepetitions = GetParam<UINT>(L"Repetitions");
    threadData.pThreadsActive = &m_nThreadsActive;

    LaunchThread<ThreadData, CModeSwitchThread>(threadData, CModeSwitchThread());
    InterlockedIncrement(&m_nThreadsActive);

    LaunchThread<ThreadData, CPnPStopThread>(threadData, CPnPStopThread());
    InterlockedIncrement(&m_nThreadsActive);

}

void CModeSwitchThread::operator()(ThreadData threadData)
{
    CI3DPtr pI3D;
    CI3DDevicePtr pDevice;
    bool bWindowed = true;
    DeviceCreateOptions Options;
    
    for(UINT i=0; i < threadData.numRepetitions ; i++)
    {
        try
        {

            H_CHECK(::CreateI3D(&pI3D.p, NULL));

            H_CHECK(pI3D->GetValidDeviceCreateOptions(bWindowed,
                                                NULL,
                                                640,
                                                480,
                                                1,
                                                true,
                                                false,
                                                NULL,
                                                &Options));
            
            H_CHECK(pI3D->CreateDevice(&Options,
                                &pDevice.p));

            pDevice.Release();
            pI3D.Release();
            bWindowed = !bWindowed;
        }
        catch(...)
        {
            OutputDebugString(L"Thread threw an exception\n");
        }
    }  

    //this must happen on all paths after the test has completed (even if an exception is thrown)
    InterlockedDecrement(threadData.pThreadsActive);

}
void CPnPStopThread::operator()(ThreadData threadData)
{
    CPnPControlDevice controlAdapter;

    for(UINT i=0; i < threadData.numRepetitions ; i++)
    {
        try
        {
            //Stop the display device.
            controlAdapter.PnPStop();

            //Sleep for a minute.
            Sleep(1000);

            //Start the device.
            controlAdapter.PnPStart();
        }
        catch(...)
        {
            OutputDebugString(L"Thread threw an exception\n");
        }
    }

    // Workaround for Win8 # 954862
    controlAdapter.PnPStop();
    Sleep(6000);
    controlAdapter.PnPStart();

    //this must happen on all paths after the test has completed (even if an exception is thrown)
    InterlockedDecrement(threadData.pThreadsActive);
}

void CModeSwitch::PostWalk(CLogWriter * pLog)
{  
    //wait for all threads to complete
    while(0 != InterlockedCompareExchange(&m_nThreadsActive, 0, 0))
    {
        Sleep(100);
    }
}