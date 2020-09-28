/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/
#include <windows.h>
#include <d3dkmthk.h>
#include <initguid.h>
#include <Ntddvdeo.h>
#include <Newdev.h>
#include <cfgmgr32.h>

#include "dmmdiag.h"
#include "PowrProf.h"

#include "brightness2OS.h"

// Video settings (from ntpoapi.h)
// --------------
//
// Specifies the subgroup which will contain all of the video
// settings for a single policy.
//
DEFINE_GUID( GUID_VIDEO_SUBGROUP, 0x7516B95F, 0xF776, 0x4464, 0x8C, 0x53, 0x06, 0x16, 0x7F, 0x40, 0xCC, 0x99 );
//
// Specifies (in seconds) how long we wait after the last user input has been
// recieved before we dim the video.
//
DEFINE_GUID( GUID_VIDEO_DIM_TIMEOUT, 0x17aaa29b, 0x8b43, 0x4b94, 0xaa, 0xfe, 0x35, 0xf6, 0x4d, 0xaa, 0xf1, 0xee);


using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;

using namespace std;
using namespace DevX::GRFX;


namespace DevX { namespace GRFX { namespace BaseVideo
{

HANDLE StartProcess(__in __nullterminated const char* szCmdLine)
{
    STARTUPINFOA StartInfo;
    ZeroMemory( &StartInfo, sizeof(StartInfo) );
    StartInfo.cb = sizeof( StartInfo );

    PROCESS_INFORMATION ProcessInfo;
    ZeroMemory( &ProcessInfo, sizeof(ProcessInfo) );

    if ( CreateProcessA(NULL,
                        (LPSTR)szCmdLine,
                        NULL,
                        NULL,
                        FALSE,
                        0,
                        NULL,
                        NULL,
                        &StartInfo,
                        &ProcessInfo))
    {
        CloseHandle(ProcessInfo.hThread);
        return ProcessInfo.hProcess;
    }
    else
    {
        return NULL;
    }
}

bool SetDimTimeoutDC(UINT dwDimmTimeSec)
{
	bool bRet = true;

#ifdef USE_POWERCFG
    // Set dimming time out on DC to Never
    HANDLE hProcess = StartProcess("powercfg.exe /setdcvalueindex scheme_current SUB_VIDEO VIDEODIM 0");
    DWORD exitReason = WaitForSingleObject(hProcess,5000);
    CloseHandle(hProcess);
    if (exitReason == WAIT_TIMEOUT)
    {
        Log::Comment(String().Format(L"simbattctl.exe didn't exit in 30 sec"));
		bRet = false;
    }
#else
    LPGUID pguidActiveScheme = nullptr;
    DWORD dwResult = PowerGetActiveScheme(NULL, &pguidActiveScheme);
    if ((dwResult == ERROR_SUCCESS) && (pguidActiveScheme != nullptr))
    {
        dwResult = PowerWriteDCValueIndex(NULL,
                                            pguidActiveScheme,
                                            &GUID_VIDEO_SUBGROUP,
                                            &GUID_VIDEO_DIM_TIMEOUT,
                                            dwDimmTimeSec);
        // set changed policy
        DWORD dwResult = PowerSetActiveScheme(NULL,pguidActiveScheme);
        if (dwResult != ERROR_SUCCESS)
        {
            Log::Comment(String().Format(L"Can't set active power scheme. Dim timeout won't be changed"));
            bRet = false;
        }

        LocalFree(pguidActiveScheme);
        bRet = (dwResult == ERROR_SUCCESS);
    }
	else
	{
		bRet = false;
	}
#endif

	Sleep(3000);
	return bRet;
}

__forceinline HRESULT Brightness2OS::CallEscape(__inout D3DKMT_ESCAPE* Escape)
{
    LONG ntStatus = (*m_pfnEscape)(Escape);
    return HRESULT_FROM_WIN32(ntStatus);
}

bool Brightness2OS::SetupBrightness2OS()
{

    IDXGIFactory1* pdxFactory = NULL;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pdxFactory);

    if (FAILED(hr))
    {
        Log::Comment(String().Format(L"Failed to CreateDXGIFactory1"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return false;
    }

    DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
    D3DKMT_ESCAPE Escape = {0};

    m_pAdapter = NULL;
    m_pAd = NULL;
    m_hGdi = NULL;
    
    m_IsAdaptiveBrightness = false;

    pdxFactory->EnumAdapters1(0, &m_pAdapter);

    pdxFactory->Release();

    if (m_pAdapter)
    {
        m_pAd = AdapterDescriptor::GetAdapterDescriptor(m_pAdapter);
        if (!m_pAd)
        {
            Log::Comment(String().Format(L"Failed to get AdapterDescriptor"));
            goto Cleanup;
        }
    }
    else
    {
        Log::Comment(String().Format(L"Failed to Create IDXGIAdapter1"));
        return false;
    }

    m_hGdi = LoadLibrary( TEXT("gdi32.dll") );

    if (!m_hGdi)
    {
        Log::Comment(String().Format(L"Can't load gdi32.dll"));
        goto Cleanup;
    }

    m_pfnEscape = reinterpret_cast<PFND3DKMT_ESCAPE>( GetProcAddress(m_hGdi, "D3DKMTEscape") );
    if (!m_pfnEscape)
        goto Cleanup;

    // Get kernel mode handle for the adapter and save if to use in tests later on
    hr = m_pAd->OpenKernelHandle(m_hAdapter);
    if (FAILED(hr))
    {
        Log::Comment(String().Format(L"Can't open kernel handle"));
        goto Cleanup;
    }

    Escape.hAdapter = m_hAdapter;
    Escape.hDevice = NULL;
    Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
    Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
    Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

    HRESULT Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
        goto Cleanup;
    }

    m_IsAdaptiveBrightness = GenericAdapterDiagInfo.BrightnessCaps.AdaptiveBrightness == TRUE;

    return true;

Cleanup:

    Cleanup();
    return false;

}


bool Brightness2OS::CleanupBrightness2OS()
{
    // Set dimming time out on DC to Never
    SetDimTimeoutDC(0);

    return Cleanup();
}

bool Brightness2OS::Cleanup()
{
    if (m_pAd)
    {
        if (m_hAdapter)
        {
            m_pAd->CloseKernelHandle(m_hAdapter);
        }
        delete m_pAd;
        m_pAd = NULL;
    }

    if (m_pAdapter)
    {
        m_pAdapter->Release();
        m_pAdapter = NULL;
    }

    if(m_hGdi)
    {
        m_pfnEscape = NULL;
        FreeLibrary(m_hGdi);
    }

    return true;
}

void Brightness2OS::TestAdaptiveBrightnessPresence()
{
    if(m_IsAdaptiveBrightness)
    {
        Log::Comment(String().Format(L"Adaptive Brightness interfaces is supported by the driver"));
    }
    else
    {
        Log::Comment(String().Format(L"Adaptive Brightness interfaces is not supported by the driver"));
    }
    // Pass
}


void Brightness2OS::TestAC2DCTransition()
{
    if (!m_IsAdaptiveBrightness)
    {
        Log::Comment(String().Format(L"Adaptive Brightness 2 is not supported"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return;
    }

    // Set AC power
    {
        HANDLE hProcess = StartProcess("simbattctl.exe -a");

        if (!hProcess)
        {
            Log::Comment(String().Format(L"Can't start simbattctl.exe"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        DWORD exitReason = WaitForSingleObject(hProcess,30000);
        CloseHandle(hProcess);
        if (exitReason == WAIT_TIMEOUT)
        {
            Log::Comment(String().Format(L"simbattctl.exe didn't exit in 30 sec"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        Sleep(1000);

        // Verify that On AC power

        DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
        D3DKMT_ESCAPE Escape = {0};
        Escape.hAdapter = m_hAdapter;
        Escape.hDevice = NULL;
        Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
        Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
        Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

        HRESULT Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }
        if (GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags.BrightnessStateOnACPower == FALSE)
        {
            Log::Comment(String().Format(L"The adapter should be on AC Power. Actual:%d",
                GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags));
            Log::Result(WEX::Logging::TestResults::Failed);
        }
        if (GenericAdapterDiagInfo.HwBacklightOptLevel != DxgkBacklightOptimizationDisable)
        {
            Log::Comment(String().Format(L"The backlight optimization state should be Disable. Actual:%d",
                (UINT)GenericAdapterDiagInfo.HwBacklightOptLevel));
            Log::Result(WEX::Logging::TestResults::Failed);
        }
    }


    // Set DC power
    {
        HANDLE hProcess = StartProcess("simbattctl.exe -d");

        if (!hProcess)
        {
            Log::Comment(String().Format(L"Can't start simbattctl.exe"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        DWORD exitReason = WaitForSingleObject(hProcess,30000);
        CloseHandle(hProcess);
        if (exitReason == WAIT_TIMEOUT)
        {
            Log::Comment(String().Format(L"simbattctl.exe didn't exit in 30 sec"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        Sleep(2000);

        // Verify that On DC power

        DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
        D3DKMT_ESCAPE Escape = {0};

        Escape.hAdapter = m_hAdapter;
        Escape.hDevice = NULL;
        Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
        Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
        Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

        HRESULT Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }
        if (GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags.BrightnessStateOnACPower == TRUE)
        {
            Log::Comment(String().Format(L"The adapter should be on DC Power. Actual:%d",
                GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags));
            Log::Result(WEX::Logging::TestResults::Failed);
        }
        if (GenericAdapterDiagInfo.HwBacklightOptLevel != DxgkBacklightOptimizationDesktop)
        {
            Log::Comment(String().Format(L"The backlight optimization state should be Desktop. Actual:%d",
                (UINT)GenericAdapterDiagInfo.HwBacklightOptLevel));
            Log::Result(WEX::Logging::TestResults::Failed);
        }
    }
}


void Brightness2OS::TestDC2ACTransition()
{
    if (!m_IsAdaptiveBrightness)
    {
        Log::Comment(String().Format(L"Adaptive Brightness 2 is not supported"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return;
    }


    // Set DC power
    {
        HANDLE hProcess = StartProcess("simbattctl.exe -d");

        if (!hProcess)
        {
            Log::Comment(String().Format(L"Can't start simbattctl.exe"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        DWORD exitReason = WaitForSingleObject(hProcess,30000);
        CloseHandle(hProcess);
        if (exitReason == WAIT_TIMEOUT)
        {
            Log::Comment(String().Format(L"simbattctl.exe didn't exit in 30 sec"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        Sleep(2000);

        // Verify that On DC power

        DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
        D3DKMT_ESCAPE Escape = {0};

        Escape.hAdapter = m_hAdapter;
        Escape.hDevice = NULL;
        Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
        Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
        Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

        HRESULT Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }
        if (GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags.BrightnessStateOnACPower == TRUE)
        {
            Log::Comment(String().Format(L"The adapter should be on DC Power. Actual:%d",
                GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags));
            Log::Result(WEX::Logging::TestResults::Failed);
        }
        if (GenericAdapterDiagInfo.HwBacklightOptLevel != DxgkBacklightOptimizationDesktop)
        {
            Log::Comment(String().Format(L"The backlight optimization state should be Desktop. Actual:%d",
                (UINT)GenericAdapterDiagInfo.HwBacklightOptLevel));
            Log::Result(WEX::Logging::TestResults::Failed);
        }
    }


    // Set AC power
    {
        HANDLE hProcess = StartProcess("simbattctl.exe -a");

        if (!hProcess)
        {
            Log::Comment(String().Format(L"Can't start simbattctl.exe"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        DWORD exitReason = WaitForSingleObject(hProcess,30000);
        CloseHandle(hProcess);
        if (exitReason == WAIT_TIMEOUT)
        {
            Log::Comment(String().Format(L"simbattctl.exe didn't exit in 30 sec"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        Sleep(2000);

        // Verify that On AC power

        DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
        D3DKMT_ESCAPE Escape = {0};
        Escape.hAdapter = m_hAdapter;
        Escape.hDevice = NULL;
        Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
        Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
        Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

        HRESULT Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }
        if (GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags.BrightnessStateOnACPower == FALSE)
        {
            Log::Comment(String().Format(L"The adapter should be on AC Power. Actual:%d",
                GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags));
            Log::Result(WEX::Logging::TestResults::Failed);
        }
        if (GenericAdapterDiagInfo.HwBacklightOptLevel != DxgkBacklightOptimizationDisable)
        {
            Log::Comment(String().Format(L"The backlight optimization state should be Disable. Actual:%d",
                (UINT)GenericAdapterDiagInfo.HwBacklightOptLevel));
            Log::Result(WEX::Logging::TestResults::Failed);
        }
    }
}


void Brightness2OS::TestFullsreenVideoSenario()
{
    if (!m_IsAdaptiveBrightness)
    {
        Log::Comment(String().Format(L"Adaptive Brightness 2 is not supported"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return;
    }

    // Set DC power

    {
        HANDLE hProcess = StartProcess("simbattctl.exe -d");

        if (!hProcess)
        {
            Log::Comment(String().Format(L"Can't start simbattctl.exe"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        DWORD exitReason = WaitForSingleObject(hProcess,30000);
        CloseHandle(hProcess);
        if (exitReason == WAIT_TIMEOUT)
        {
            Log::Comment(String().Format(L"simbattctl.exe didn't exit in 30 sec"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        Sleep(1000);
    }

    // Start fullscreen video using direct flip swap-chain

    {
        HANDLE hProcess = StartProcess("MediaEngineTest.exe -DCompTrident -fullscreen -autoplay -autoexit Coral_Reef.mp4");
        BOOL bShouldReturn = FALSE;

        if (!hProcess)
        {
            Log::Comment(String().Format(L"Can't start MediaEngineTest.exe"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        // Get some time to settle
        Sleep (10000);

        // Verify that the level is dynamic and fullscreen video flag is set in dsgkrnl
        {
            DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
            D3DKMT_ESCAPE Escape = {0};
            Escape.hAdapter = m_hAdapter;
            Escape.hDevice = NULL;
            Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
            Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
            Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

            HRESULT Status = CallEscape(&Escape);
            if (FAILED(Status))
            {
                Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
                Log::Result(WEX::Logging::TestResults::Failed);
				return;
            }
            if (GenericAdapterDiagInfo.HwBacklightOptLevel != DxgkBacklightOptimizationDynamic)
            {
                Log::Comment(String().Format(L"The backlight optimization state should be Dynamic. Actual:%d",
                    (UINT)GenericAdapterDiagInfo.HwBacklightOptLevel));
                Log::Result(WEX::Logging::TestResults::Failed);
            }
            if (GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags.BrightnessStateFullscreenVideo == FALSE)
            {
                Log::Comment(String().Format(L"The adapter brightness state should be FullscreenVideo. Actual:%d",
                    GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags));
                Log::Result(WEX::Logging::TestResults::Failed);
            }
        }

        // The process, a video clip, should exit in less than 2 mins (as we already waited for 1 min)
        DWORD exitReason = WaitForSingleObject(hProcess,2*60000);
        CloseHandle(hProcess);
        if (exitReason == WAIT_TIMEOUT)
        {
            Log::Comment(String().Format(L"MediaEngineTest.exe didn't exit in 3 min"));
//            Log::Result(WEX::Logging::TestResults::Failed);
//            return;
        }

        // Get some time to settle
        Sleep(8000);

        // Verify that the level is desktop and fullscreen video flag is reset in dsgkrnl
        {
            DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
            D3DKMT_ESCAPE Escape = {0};
            Escape.hAdapter = m_hAdapter;
            Escape.hDevice = NULL;
            Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
            Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
            Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

            HRESULT Status = CallEscape(&Escape);
            if (FAILED(Status))
            {
                Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
                Log::Result(WEX::Logging::TestResults::Failed);
                return;
            }
            if (GenericAdapterDiagInfo.HwBacklightOptLevel != DxgkBacklightOptimizationDesktop)
            {
                Log::Comment(String().Format(L"The backlight optimization state should be Desktop. Actual:%d",
                    (UINT)GenericAdapterDiagInfo.HwBacklightOptLevel));
                Log::Result(WEX::Logging::TestResults::Failed);
            }
            if (GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags.BrightnessStateFullscreenVideo == TRUE)
            {
                Log::Comment(String().Format(L"The adapter brightness state shouldn't be FullscreenVideo."));
                Log::Result(WEX::Logging::TestResults::Failed);
            }
        }
    }
}


// The test is not working. It doesn't go to dim state for some reason.
// Disable it, as it should verfy that after the dim was set, dxgkrnl set the correct Optimization Level.
// We need to find a way to reliably plase induce dim state.

void Brightness2OS::TestDynamicDimmedTrasition()
{
    if (!m_IsAdaptiveBrightness)
    {
        Log::Comment(String().Format(L"Adaptive Brightness 2 is not supported"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return;
    }

    // Set DC power
    {
        HANDLE hProcess = StartProcess("simbattctl.exe -d");

        if (!hProcess)
        {
            Log::Comment(String().Format(L"Can't start simbattctl.exe"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        DWORD exitReason = WaitForSingleObject(hProcess,30000);
        CloseHandle(hProcess);
        if (exitReason == WAIT_TIMEOUT)
        {
            Log::Comment(String().Format(L"simbattctl.exe didn't exit in 30 sec"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        Sleep(1000);
    }

    // Set dimming time out on DC to 60 sec
    {
        if (SetDimTimeoutDC(60) == false)
        {
            Log::Comment(String().Format(L"Failed to set dim timeout"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

        Sleep(1000);
    }

    // Start fullscreen video using direct flip swap-chain
    {
        HANDLE hProcess = StartProcess("MediaEngineTest.exe -DCompTrident -fullscreen -autoplay -autoexit Coral_Reef.mp4");
        BOOL bShouldReturn = FALSE;

        if (!hProcess)
        {
            Log::Comment(String().Format(L"Can't start MediaEngineTest.exe"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }

		// Wait for for 60+ sec and check state, it should become dimmed
        Sleep (70000);

        // Verify that the level is dimmed and monitor is in dimmed state
        {
            DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
            D3DKMT_ESCAPE Escape = {0};
            Escape.hAdapter = m_hAdapter;
            Escape.hDevice = NULL;
            Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
            Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
            Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

            HRESULT Status = CallEscape(&Escape);
            if (FAILED(Status))
            {
                Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
                Log::Result(WEX::Logging::TestResults::Failed);
                bShouldReturn = TRUE;
            }
            if (GenericAdapterDiagInfo.HwBacklightOptLevel != DxgkBacklightOptimizationDimmed)
            {
                Log::Comment(String().Format(L"The backlight optimization state should be Dimmed. Actual:%d",
                    (UINT)GenericAdapterDiagInfo.HwBacklightOptLevel));
                Log::Result(WEX::Logging::TestResults::Failed);
                bShouldReturn = TRUE;
            }
            if (GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags.BrightnessStateMonitorDimmed == FALSE)
            {
                Log::Comment(String().Format(L"The adapter brightness state should be Dimmed. Actual:%d",
                    GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags));
                Log::Result(WEX::Logging::TestResults::Failed);
                bShouldReturn = TRUE;
            }
        }

        // The process, a video clip, should exit in less than 2 mins now
        DWORD exitReason = WaitForSingleObject(hProcess,2*60000);
        CloseHandle(hProcess);
        if (exitReason == WAIT_TIMEOUT)
        {
            Log::Comment(String().Format(L"MediaEngineTest.exe didn't exit in 3 min"));
//            Log::Result(WEX::Logging::TestResults::Failed);
//            return;
        }


        // Get some time to settle down
        Sleep(8000);

        // Verify that the level is dimmed and monitor is in dimmed state
        {
            DXGK_GENERIC_ADAPTER_DIAG_INFO GenericAdapterDiagInfo = {0};
            D3DKMT_ESCAPE Escape = {0};
            Escape.hAdapter = m_hAdapter;
            Escape.hDevice = NULL;
            Escape.Type = D3DKMT_ESCAPE_GENERIC_ADAPTER_DIAG_INFO;
            Escape.pPrivateDriverData = &GenericAdapterDiagInfo;
            Escape.PrivateDriverDataSize = sizeof(GenericAdapterDiagInfo);

            HRESULT Status = CallEscape(&Escape);
            if (FAILED(Status))
            {
                Log::Comment(String().Format(L"Can't get AdapterDiagInfo"));
                Log::Result(WEX::Logging::TestResults::Failed);
            }
            if (GenericAdapterDiagInfo.HwBacklightOptLevel != DxgkBacklightOptimizationDimmed)
            {
                Log::Comment(String().Format(L"The backlight optimization state should be Dimmed. Actual:%d",
                    (UINT)GenericAdapterDiagInfo.HwBacklightOptLevel));
                Log::Result(WEX::Logging::TestResults::Failed);
            }
            if (GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags.BrightnessStateMonitorDimmed == FALSE)
            {
                Log::Comment(String().Format(L"The adapter brightness state should be Dimmed. Actual:%d",
                    GenericAdapterDiagInfo.DxgkGenericAdapterDiagInfoFlags));
                Log::Result(WEX::Logging::TestResults::Failed);
            }
        }
    }
}

} /* namespace DevX */ } /* namespace GRFX */ } /* namespace BaseVideo */ 
