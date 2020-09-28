#include "WaitVBl.h"

#include "qpc.h"

#include <mmsystem.h>

clsWaitVBl::clsWaitVBl()
{
    // Set the test properties.
    SetTestType(TESTTYPE_CONF);

    m_szCommandKey= _T("WaitVBl");
    m_szLogName =   _T("WaitVBl");
    m_szTestName=   _T("WaitVBl");

    (m_Options.D3DOptions).dwDevTypes = DEVICETYPE_HAL;
    (m_Options.D3DOptions).fMinDXVersion = 9.1f;
    (m_Options.D3DOptions).fMaxDXVersion = 9.1f;
    (m_Options.TestOptions).bCompareSrcRef = false;
     m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
    (m_Options.ModeOptions).uMaxDisplayModes = MODEOPT_MAXMODES;
    //(m_Options.D3DOptions).dwIncVersions = DXVERSION_91;

    ZeroMemory( m_fMeasuredIntervals, NO_OF_TESTED_INTERVALS * sizeof( float ) );
}

clsWaitVBl::~clsWaitVBl()
{
}

UINT clsWaitVBl::TestInitialize(void)
{
    D3DDEVICE_CREATION_PARAMETERS d3ddcpSrcDevice8;

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    m_pSrcDevice-> GetCreationParameters(&d3ddcpSrcDevice8);
    uiTestedAdapterOrdinal = d3ddcpSrcDevice8.AdapterOrdinal;
    d3ddevtypeTestedType = d3ddcpSrcDevice8.DeviceType;
    m_pD3D -> GetAdapterDisplayMode(uiTestedAdapterOrdinal, &d3ddisplaymodeAdapter);

    sprintf
    (
        szModeDesc,
        "Width: %d, Height: %d, BPP: %d, RR: %d\n",
        d3ddisplaymodeAdapter.dwWidth,
        d3ddisplaymodeAdapter.dwHeight,
        d3ddisplaymodeAdapter.dwBPP,
        d3ddisplaymodeAdapter.dwRefreshRate
    );

    SetTestRange(1, 1);

    if (d3ddcpSrcDevice8.DeviceType != D3DDEVTYPE_HAL)
        WriteToLog("Test should be executed against HAL only");
    else

    if (clsQPC::Initialize() != QPC_OK)
        WriteToLog("No suitable performance counter is available on this system.\n");
    else
    {
        liFrequency = clsQPC::GetFrequency();

        /*
        WriteToLog("Frequency high: %d\n", liFrequency.HighPart);
        WriteToLog("Frequency low: %d\n", liFrequency.LowPart);
        */

        if (liFrequency.QuadPart <= 0)
            WriteToLog("Performance counter reported an invalid frequency.\n");
        else
            if (liFrequency.QuadPart < 2 * d3ddisplaymodeAdapter.dwRefreshRate)
                WriteToLog("The resolution of the performance counter is too low for reliable testing of this mode.\n");
            else
                return D3DTESTINIT_RUN;
    }

    return D3DTESTINIT_SKIPALL;
}

bool clsWaitVBl::ExecuteTest(UINT uiTestCaseId)
{
    float fExpectedInterval, fMinInterval, fMaxInterval, fTotalIntervals;
    float fMinMeasuredInterval= 10000.f, fMaxMeasuredInterval= 0.f;
    //DWORD dwPreviousReturnTime;
    LARGE_INTEGER liPreviousReturnTime;
    UINT uiCount, uiFailedIntervalCount = 0;
    UINT uNumberOfSwapChains;
    HRESULT hr;
    DISPLAYMODE modeMonitor;

    BeginTestCase(szModeDesc, uiTestCaseId);
    
    Sleep(2000);

    // Check to see if we should test ADAPTERGROUPS.
    // When testing adaptergroups there will be multiple swapchains, one for each monitor in the group.
    //
    // We'll assume the framework always creates swapchains for all monitors when testing ADAPTERGROUPS.
    // if this changes, hopefully we'll be given a helper function that tells us how many swapchains were
    // created.
    //
    if (KeySet(_T("multihead")))
    {
        WriteToLog(_T("--------------------------------------------------------------------"));
        WriteToLog(_T("Testing AdapterGroups.   We'll test WaitForVBlank on all swapchains."));
        WriteToLog(_T("--------------------------------------------------------------------"));
        uNumberOfSwapChains = m_pSrcDevice -> GetCaps() -> NumberOfAdaptersInGroup;
    }
    else 
        uNumberOfSwapChains = 1;  // Only one monitor, not testing Adaptergroups.

    //
    // Cycle on all adapters in the group.
    //
    for (UINT uSwapchain=0; uSwapchain < uNumberOfSwapChains; uSwapchain++)
    {
        WriteToLog(_T("Testing WaitForVBlank on swapchain: %u"), uSwapchain);

        m_pSrcDevice -> GetDisplayMode(uSwapchain, &modeMonitor);
        WriteToLog(_T("Width: %d, Height: %d, BPP: %d, RR: %d\n"),
            modeMonitor.dwWidth,
            modeMonitor.dwHeight,
            modeMonitor.dwBPP,
            modeMonitor.dwRefreshRate
        );

    
        fTotalIntervals = 0; 
        fExpectedInterval = 1000.0f / (float)(modeMonitor.dwRefreshRate);
        bool bInterlaced = false;
        hr = InterlacedMode(uSwapchain, bInterlaced);
        if (FAILED(hr))
        {
            WriteToLog(_T("FAILURE *** InterlacedMode() failed"));
            Fail();
            goto Exit;
        }
        else
        {
            if (bInterlaced == true)
            {
                fExpectedInterval /= 2;
            }
        }

        fMinInterval = (fExpectedInterval * (1.0f - MAX_PERCENT_DIFFERENCE)) - 2.0f;
        fMaxInterval = (fExpectedInterval * (1.0f + MAX_PERCENT_DIFFERENCE)) + 2.0f; 

        hr = m_pSrcDevice -> WaitForVBlank(uSwapchain);
        if (FAILED(hr))
        {
            WriteToLog(_T("FAILURE *** Initial WaitForVBlank failed with %s(0x%x)***"), m_pD3D->HResultToString(hr), hr);
            WriteToLog(_T("Aborting Test!"));
            Fail();
            goto Exit;
        }

        //dwPreviousReturnTime = timeGetTime();
        QueryPerformanceCounter(&liPreviousReturnTime);

        for (uiCount = 0; uiCount < NO_OF_TESTED_INTERVALS; ++uiCount)
        {
            //DWORD dwCurrentReturnTime;
            LARGE_INTEGER liCurrentReturnTime;
            float fMeasuredInterval;

            hr = m_pSrcDevice -> WaitForVBlank(uSwapchain);
            if (FAILED(hr))
            {
                WriteToLog(_T("FAILURE ***WaitForVBlank %d/%d failed with %s(0x%x)***"), uiCount, NO_OF_TESTED_INTERVALS, m_pD3D->HResultToString(hr), hr);
                 WriteToLog(_T("Aborting Test!"));
                Fail();
                goto Exit;
            }

            //dwCurrentReturnTime = timeGetTime();
            QueryPerformanceCounter(&liCurrentReturnTime);

            //fMeasuredInterval = (float)(dwCurrentReturnTime - dwPreviousReturnTime);

            fMeasuredInterval =
            ((float)(liCurrentReturnTime.QuadPart - liPreviousReturnTime.QuadPart)) * 1000.0f /
            (float)(liFrequency.QuadPart);

            if (uiCount)
            {
                if (fMeasuredInterval < fMinMeasuredInterval)
                    fMinMeasuredInterval = fMeasuredInterval;
                if (fMeasuredInterval > fMaxMeasuredInterval)
                    fMaxMeasuredInterval = fMeasuredInterval;
            }
            else
                fMaxMeasuredInterval = (fMinMeasuredInterval = fMeasuredInterval);
            fTotalIntervals += fMeasuredInterval;
            
            if ((fMeasuredInterval < fMinInterval) || (fMeasuredInterval > fMaxInterval))
            {
                WriteToLog( "Failure[%u times]: the measured interval[%f] vs expected interval[Min, Standard, Max][%f, %f, %f]\n", 
                            uiFailedIntervalCount+1, fMeasuredInterval, fMinInterval, fExpectedInterval, fMaxInterval);
                ++uiFailedIntervalCount;
            }

            //dwPreviousReturnTime = dwCurrentReturnTime;
            liPreviousReturnTime = liCurrentReturnTime;
        }

        if (uiFailedIntervalCount > MAX_FAILING_INTERVALS)
        {
            WriteToLog("[Warning]: %d out of %d measured intervals deviated significantly from the expected value.***\n"
                       "If the test case passes the IHV should check if a wrong refresh rate was reported.\n", 
                       uiFailedIntervalCount, NO_OF_TESTED_INTERVALS);
            float fAverageInterval = fTotalIntervals / NO_OF_TESTED_INTERVALS;
            if ( fAverageInterval < fMinInterval || fAverageInterval > fMaxInterval)
            {
                float fMinAverageInterval = fAverageInterval * 0.67f;
                float fMaxAverageInterval = fAverageInterval * 1.33f;

                uiFailedIntervalCount = 0;
                for ( uiCount = 0; uiCount < NO_OF_TESTED_INTERVALS; ++uiCount)
                {
                    if ( m_fMeasuredIntervals[uiCount] < fMinAverageInterval || m_fMeasuredIntervals[uiCount] > fMaxAverageInterval )
                    {
                        ++uiFailedIntervalCount;
                    }
                }

                if ( uiFailedIntervalCount > MAX_FAILING_INTERVALS )
                {
                    WriteToLog("Expected interval: %.4f; Average measured interval: %.4f\n", fExpectedInterval, (fTotalIntervals / NO_OF_TESTED_INTERVALS));
                    WriteToLog("Minimum Measured Interval: %.4f; Maximum Measured Interval: %.4f\n",
                    fMinMeasuredInterval, fMaxMeasuredInterval);
                    Fail();
                }
            }
        }
    }

Exit:

    return true;
}

bool clsWaitVBl::TestTerminate(void)
{
    return true;
}

bool clsWaitVBl::DisplayFrame()
{
    return true;
}

//--------------------------------------------------------------------------------------------
// Function: InterlacedMode()
// Purpose:  Check if the display mode is interlaced. If the function fails, it returns false
// return code: S_OK - TRUE, S_FALSE - FALSE, others - failed.
//---------------------------------------------------------------------------------------------
// #define REFERENCE_SCANLINE	100
HRESULT clsWaitVBl::InterlacedMode(__in DWORD dwAdapter, __in bool &bInterlaced)
{
    BOOL            bResult;
    HRESULT         hrResult = S_OK;
    DISPLAY_DEVICE  displayDevice = {0};
    DEVMODE         devMode = {0};
    DWORD           dwLastError;
    HMONITOR        hMonitor = NULL;  
    MONITORINFOEX   MonitorInfoEx;

    bInterlaced = false;
	displayDevice.cb = sizeof(displayDevice);
    hMonitor = m_pD3D->GetAdapterMonitor(dwAdapter);
    if (hMonitor == NULL)
    {
        dwLastError = GetLastError();
        WriteToLog("GetAdapterMonitor() failed with the last error %u.\n", dwLastError);
        hrResult = E_FAIL;
        goto Exit;
    }

    ZeroMemory((void *)&MonitorInfoEx, sizeof(MonitorInfoEx));
    MonitorInfoEx.cbSize = sizeof(MonitorInfoEx);
    bResult = GetMonitorInfo(hMonitor, &MonitorInfoEx);
    if (!bResult)
    {
        dwLastError = GetLastError();
        WriteToLog("GetMonitorInfo() failed with the last error %u.\n", dwLastError);
        hrResult = E_FAIL;
        goto Exit;
    }

    devMode.dmSize = sizeof(devMode);
 	devMode.dmSpecVersion = DM_SPECVERSION;

    bResult = EnumDisplaySettings(MonitorInfoEx.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
    if (!bResult)
    {
        dwLastError = GetLastError();
        WriteToLog("EnumDisplaySettings(), dwAdapter[%u], DeviceName[%s] failed with the last error %u.\n", 
                    dwAdapter, displayDevice.DeviceName, dwLastError);
        hrResult = E_FAIL;
        goto Exit;
    }

    if ((devMode.dmDisplayFlags & DM_INTERLACED) != 0)
    {
        WriteToLog("Interlaced mode detected: devMode.dmDisplayFlags[0x%08x].\n", devMode.dmDisplayFlags);
        bInterlaced = true;
    }
    else
    {
        WriteToLog("Non-interlaced mode detected: devMode.dmDisplayFlags[0x%08x].\n", devMode.dmDisplayFlags);
    }

Exit:
    return hrResult;
}