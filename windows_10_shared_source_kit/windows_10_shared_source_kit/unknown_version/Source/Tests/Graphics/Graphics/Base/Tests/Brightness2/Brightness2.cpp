/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/
#include <windows.h>
#include <d3dkmthk.h>
#include <initguid.h>
#include <Ntddvdeo.h>
#include <Newdev.h>
#include <cfgmgr32.h>

#ifndef _DMM_DIAG_H_
// _DMM_DIAG_H_ needs to be defined to allow "d3dkmdt.h" to be included
#define _DMM_DIAG_H_
#define D3DKMDT_SPECIAL_MULTIPLATFORM_TOOL
#include "d3dkmdt.h"
#endif


#include "brightness2.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;

using namespace std;
using namespace DevX::GRFX;

HRESULT IsBrightnessSupported();

bool SetBrightnessRegKey(bool bOn)
{
    const wchar_t* REG_PATH = L"System\\CurrentControlSet\\Control\\GraphicsDrivers";
    const wchar_t* REG_NAME = L"EnableManualBrightnessMode";

    HKEY hkey = NULL;

    LONG ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                              REG_PATH,
                              0,
                              KEY_WRITE,
                              &hkey);
    if(ERROR_SUCCESS != ret)
    {
        Log::Comment(String().Format(L"Failed to open GraphicsDrivers reg key"));
        return false;
    }

    if (bOn)
    {
        DWORD dwValue = 1;
        ret = ::RegSetValueEx(hkey,
                              REG_NAME,
                              0,
                              REG_DWORD,
                              (BYTE*)&dwValue,
                              sizeof(dwValue));
    }
    else
    {
        ret = ::RegDeleteKeyValue(hkey,
                                  0,
                                  REG_NAME);
    }

    ::RegCloseKey(hkey);
    hkey = NULL;

    return (ERROR_SUCCESS == ret);
}




namespace DevX { namespace GRFX { namespace BaseVideo
{

__forceinline HRESULT Brightness2::CallEscape(__inout D3DKMT_ESCAPE* Escape)
{
    LONG ntStatus = (*m_pfnEscape)(Escape);
    return HRESULT_FROM_WIN32(ntStatus);
}

bool Brightness2::SetupBrightness2()
{
    IDXGIFactory1* pdxFactory = NULL;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pdxFactory);

    if (FAILED(hr))
    {
        Log::Comment(String().Format(L"Failed to CreateDXGIFactory1"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return false;
    }

    m_pAdapter = NULL;
    m_pAd = NULL;
    m_hGdi = NULL;
    
    m_LevelsCnt = 0;
    m_BrightnessMin = m_BrightnessMax = 0;
    m_IsBrightness2 = false;

    // Set up escape data structures
    D3DKMT_ESCAPE Escape = {};
    Escape.Type = D3DKMT_ESCAPE_BRIGHTNESS;
    Escape.Flags.HardwareAccess  = TRUE;
    D3DKMT_BRIGHTNESS_INFO EscapeData = {};
    Escape.pPrivateDriverData    = &EscapeData;
    Escape.PrivateDriverDataSize = sizeof(EscapeData);

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

    // Get WDDM driver version 
    {
        PFND3DKMT_QUERYADAPTERINFO pfnQueryAdapterInfo  = (PFND3DKMT_QUERYADAPTERINFO)GetProcAddress(m_hGdi, "D3DKMTQueryAdapterInfo");
        if (!pfnQueryAdapterInfo)
        	goto Cleanup;
    
        D3DKMT_QUERYADAPTERINFO QueryAdapterInfo = {};
        QueryAdapterInfo.hAdapter = m_hAdapter;
        QueryAdapterInfo.Type =  KMTQAITYPE_DRIVERVERSION;
        QueryAdapterInfo.pPrivateDriverData = &m_DriverVersion;
        QueryAdapterInfo.PrivateDriverDataSize = sizeof(m_DriverVersion);
    
        NTSTATUS Status = (*pfnQueryAdapterInfo)(&QueryAdapterInfo);
        
        if (Status < 0)
        {
        	goto Cleanup;
        }
    }


    Escape.hAdapter = m_hAdapter;

    // Enable an escape access to brigntness data in registry
    if (!SetBrightnessRegKey(true))
    {
        Log::Comment(String().Format(L"Can't update registry"));
        goto Cleanup;
    }

    // Set manual mode to ensure OS won't interfere with a test, it will fail on WDDM 1.1 driver. 
    // Return success, no need to continue. The test will skip.
    {
        memset(&EscapeData,0,sizeof(EscapeData));
        EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_BEGIN_MANUAL_MODE;
        HRESULT Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"D3DKMT_BRIGHTNESS_INFO_BEGIN_MANUAL_MODE failed"));
            return true;
        }
    }
    
    // Get Brigtness2 interface. It is optinal and might not be present. Return success, no need to continue.
    // The test will skip.
    {
        EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_CAPS;
        hr = CallEscape(&Escape);
        m_IsBrightness2 = SUCCEEDED(hr);
        if (!m_IsBrightness2)
        {
            Log::Comment(String().Format(L"Brightness2 interface is not present."));
            return true;
        }
    }

    // Get bightness levels and find min and max
    {
        memset(&EscapeData,0,sizeof(EscapeData));
        EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_POSSIBLE_LEVELS;
        HRESULT Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"Failed to get brightness levels information using Brightness2 interface"));
            goto Cleanup;
        }
        m_LevelsCnt = EscapeData.PossibleLevels.LevelCount;
        // find max and min
        m_BrightnessMin = EscapeData.PossibleLevels.BrightnessLevels[0];
        for (UINT i=0; i < m_LevelsCnt; i++)
        {
            if (EscapeData.PossibleLevels.BrightnessLevels[i] > m_BrightnessMax)
            {
                m_BrightnessMax = EscapeData.PossibleLevels.BrightnessLevels[i];
            }
            else if (EscapeData.PossibleLevels.BrightnessLevels[i] < m_BrightnessMin)
            {
                m_BrightnessMin = EscapeData.PossibleLevels.BrightnessLevels[i];
            }
        }
    }

    return true;

Cleanup:

    Cleanup();
    return false;

}


bool Brightness2::CleanupBrightness2()
{
    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);

    // Restore OS settings for brightness
    {
        D3DKMT_ESCAPE Escape = {};
        Escape.Type = D3DKMT_ESCAPE_BRIGHTNESS;
        Escape.Flags.HardwareAccess  = TRUE;
        D3DKMT_BRIGHTNESS_INFO EscapeData = {};
        Escape.pPrivateDriverData    = &EscapeData;
        Escape.PrivateDriverDataSize = sizeof(EscapeData);
        EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_END_MANUAL_MODE;
        Escape.hAdapter = m_hAdapter;
        HRESULT Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"D3DKMT_BRIGHTNESS_INFO_END_MANUAL_MODE failed"));
        }
    }
    return Cleanup();
}

bool Brightness2::Cleanup()
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

    // Disable an escape access to brigntness data in registry
    if (!SetBrightnessRegKey(false))
    {
        Log::Comment(String().Format(L"Can't delete data from registry"));
    }

    if(m_hGdi)
    {
        m_pfnEscape = NULL;
        FreeLibrary(m_hGdi);
    }

    return true;
}

void Brightness2::TestBrightness2Presence()
{
    HRESULT hr = IsBrightnessSupported();

    // If this is WDDM 1.2 driver and brightness is supported
    // Brightness2 must be supported

    if (SUCCEEDED(hr) && hr != S_FALSE)
    {
        if (m_IsBrightness2)
        {
            Log::Comment(String().Format(L"Brightness2 is supported"));
        }
        else
        {
            if(m_DriverVersion >= KMT_DRIVERVERSION_WDDM_1_2)
            {
                Log::Comment(String().Format(L"Brightness2: WDDM 1.2 driver should support brightness only thru Brightness2 interface."));
                Log::Result(WEX::Logging::TestResults::Failed);
                return;
            }
            else
            {
                Log::Comment(String().Format(L"Brightness2 is not required for WDDM 1.x driver."));
            }
        }
    }
    // Pass
}


void Brightness2::TestSmoothBrightnessControl()
{
    if (!m_IsBrightness2)
    {
        Log::Comment(String().Format(L"Brightness 2 is not supported"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return;
    }

    D3DKMT_ESCAPE Escape = {};
    Escape.hAdapter = m_hAdapter;
    Escape.Type = D3DKMT_ESCAPE_BRIGHTNESS;
    Escape.Flags.HardwareAccess  = TRUE;
    D3DKMT_BRIGHTNESS_INFO EscapeData = {};
    Escape.pPrivateDriverData    = &EscapeData;
    Escape.PrivateDriverDataSize = sizeof(EscapeData);

    // Check Smooth brightness cap
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_CAPS;
    HRESULT Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't get smooth brightness cap"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }
    else if (EscapeData.BrightnessCaps.SmoothBrightness == FALSE)
    {
        Log::Comment(String().Format(L"Smooth brightness must be supported"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // Check minimum brightness levels requirments
    const UINT C_BRIGHTNESS_LEVELS_MIN = 101;
    if(m_LevelsCnt < C_BRIGHTNESS_LEVELS_MIN)
    {
        Log::Comment(String().Format(L"Insufficient number of brightness levels reported by the driver = %d, expecting at least 101",m_LevelsCnt));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // Set backlight optimization level to OFF.
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET_OPTIMIZATION;
    EscapeData.OptimizationLevel = DxgkBacklightOptimizationDisable;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Faild to turn off backlight optimization"));
        // continue anyway
    }

    // Check smooth bightness functionality

    //  Make sure that setting smooth to ON doesn't effect GetBrighness
    // - Set Max
    // - Set smooth brighness ON
    // - Set Min
    // - Get brightness, it should be Min
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET;
    EscapeData.Brightness = m_BrightnessMax;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't set current brightness"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET_STATE;
    EscapeData.BrightnessState.SmoothBrightness = TRUE;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't set smooth brightness"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET;
    EscapeData.Brightness = m_BrightnessMin;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't set current brightness"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't get current brightness"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }
    if (EscapeData.Brightness != m_BrightnessMin)
    {
        Log::Comment(String().Format(L"SBC affects GetBrightness functionality. Actual =  %d Expected = %d",EscapeData.Brightness,m_BrightnessMin));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // Current brighness is set to min and smooth is ON
    // Make sure things are settle down
    Sleep(4000);

    LARGE_INTEGER Frequency;
    if(!QueryPerformanceFrequency(&Frequency) || Frequency.QuadPart == 0)
    {
        Log::Comment(String().Format(L"QueryPerformanceFrequency failed"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return;
    }

    LARGE_INTEGER PerformanceCountStart;
    LARGE_INTEGER PerformanceCountEnd;

    // Rise thread priority
    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);

    // Set max brighness and query user/effective brighness in 60 ms intervals. 
    // Count till numbers are equal or for 15 intervals. Find how long did it take for brightness to settle?
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET;
    EscapeData.Brightness = m_BrightnessMax;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't get current brightness"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }
    const UINT C_SamplesToCollect = 15;
    UINT iSettledDown = 0;
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_REDUCTION;
    QueryPerformanceCounter(&PerformanceCountStart);
    for(iSettledDown=0; iSettledDown < C_SamplesToCollect; iSettledDown++)
    {
        // The thread gets 60 ms (on client) to execute its code + 60 ms to wait -> 120 ms + 200 ms for each loop after 1st one ~ 3 sec
        Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"Can't get brightness reduction information"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }
        if (EscapeData.ReductionInfo.BacklightEffective == EscapeData.ReductionInfo.BacklightUsersetting)
        {
            QueryPerformanceCounter(&PerformanceCountEnd);
            break;
        }
        Sleep(200);
    }
    if (iSettledDown == C_SamplesToCollect)
    {
        QueryPerformanceCounter(&PerformanceCountEnd);
    }
    LARGE_INTEGER delta;
    // vsync frequency in milisec
    delta.QuadPart = 1000*(PerformanceCountEnd.QuadPart - PerformanceCountStart.QuadPart)/Frequency.QuadPart;

//    const long C_MinTimeToSettle = 100;
    const long C_MaxTimeToSettle = 2200;

//    if(delta.QuadPart < C_MinTimeToSettle) // it shouldn't happen immediately
    if(iSettledDown == 0) // it shouldn't happen immediately
    {
        Log::Comment(String().Format(L"Brighness (low->high) settled down too fastly. Actual ~ %d ms ",(long)delta.QuadPart));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }
    else if(delta.QuadPart > C_MaxTimeToSettle) // it shouldn't take longe then 2000 msec
    {
        Log::Comment(String().Format(L"Brighness (low->high) settled down too slowly. Actual ~ %d ms, , expecting <= 2000 ms ",(long)delta.QuadPart));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }

    // Set min brighness and query user/effective brighness in 60 ms intervals.
    // Count till numbers are equal or for 15 intervals. Find how long did it take for brightness to settle?
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET;
    EscapeData.Brightness = (UCHAR)(0.25*m_BrightnessMax);
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't get current brightness"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_REDUCTION;
    QueryPerformanceCounter(&PerformanceCountStart);
    for(iSettledDown=0; iSettledDown < C_SamplesToCollect; iSettledDown++)
    {
        // The thread gets 60 ms (on client) to execute its code + 60 ms to wait -> 120 ms + 200 ms for each loop after 1st one ~ 3 sec
        Status = CallEscape(&Escape);
        if (FAILED(Status))
        {
            Log::Comment(String().Format(L"Can't get brightness reduction information"));
            Log::Result(WEX::Logging::TestResults::Failed);
            return;
        }
        if (EscapeData.ReductionInfo.BacklightEffective == EscapeData.ReductionInfo.BacklightUsersetting)
        {
            QueryPerformanceCounter(&PerformanceCountEnd);
            break;
        }
        Sleep(200);
    }
    if (iSettledDown == C_SamplesToCollect)
    {
        QueryPerformanceCounter(&PerformanceCountEnd);
    }
    // vsync frequency in milisec
    delta.QuadPart = 1000*(PerformanceCountEnd.QuadPart - PerformanceCountStart.QuadPart)/Frequency.QuadPart;
//    if(delta.QuadPart < C_MinTimeToSettle)
    if (iSettledDown == 0)
    {
        Log::Comment(String().Format(L"Brighness (high->low) settled down too fastly. Actual ~ %d ms ",(long)delta.QuadPart));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }
    else if(delta.QuadPart > C_MaxTimeToSettle)
    {
        Log::Comment(String().Format(L"Brighness (high->low) settled down too slowly. Actual ~ %d ms, expecting <= 2000 ms ",(long)delta.QuadPart));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }

    // Lower thread priority
    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
}

void Brightness2::TestAdaptiveBrightnessControl()
{
    if (!m_IsBrightness2)
    {
        Log::Comment(String().Format(L"Brightness 2 is not supported"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return;
    }

    D3DKMT_ESCAPE Escape = {};
    Escape.hAdapter = m_hAdapter;
    Escape.Type = D3DKMT_ESCAPE_BRIGHTNESS;
    Escape.Flags.HardwareAccess  = TRUE;
    D3DKMT_BRIGHTNESS_INFO EscapeData = {};
    Escape.pPrivateDriverData    = &EscapeData;
    Escape.PrivateDriverDataSize = sizeof(EscapeData);

    // Check adaptive brightness cap
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_CAPS;
    HRESULT Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't get adaptive brightness cap"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }
    else if (EscapeData.BrightnessCaps.AdaptiveBrightness == FALSE)
    {
        Log::Comment(String().Format(L"Adaptive brightness is not supported"));
        Log::Result(WEX::Logging::TestResults::Skipped);
        return;
    }


    // Turn OFF smooth brightness, Escape will fail if feature is not supported. Just continue.
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET_STATE;
    EscapeData.BrightnessState.SmoothBrightness = FALSE;
    CallEscape(&Escape);

    // Set backlight optimization level to OFF.
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET_OPTIMIZATION;
    EscapeData.OptimizationLevel = DxgkBacklightOptimizationDisable;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Faild to turn off backlight optimization"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // Set brightness to 0.75*MAX
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET;
    EscapeData.Brightness = (UCHAR)(0.75*m_BrightnessMax);
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Can't set current brightness"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // Let the driver a time to settle down
    Sleep(1000);

    // Set different optimization levels and verify that it effects effective brightness, doesn't affects user settings
    // and there is a correlation between effective brightness and gamma ramp

    // Get OFF setting to use it as a foundation for comparison with other levels
    RtlZeroMemory(&EscapeData,sizeof(EscapeData));
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_REDUCTION;
    Status = CallEscape(&Escape);
    D3DKMT_BRIGHTNESS_INFO BaseData;
    memcpy(&BaseData,&EscapeData,sizeof(EscapeData));

    if (EscapeData.ReductionInfo.BacklightUsersetting != EscapeData.ReductionInfo.BacklightEffective)
    {
        Log::Comment(String().Format(L"Backlight optimization OFF failed. Effecivebacklight = %d Usersettings = %d",
            EscapeData.ReductionInfo.BacklightEffective,EscapeData.ReductionInfo.BacklightUsersetting));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // should we ask for default gamma ramp?

//typedef struct _D3DDDI_GAMMA_RAMP_RGB256x3x16
//{
//    USHORT  Red[256];
//    USHORT  Green[256];
//    USHORT  Blue[256];
//} D3DDDI_GAMMA_RAMP_RGB256x3x16;

    // verify that gamma ramp data is present
    bool bGammaRamp = BaseData.ReductionInfo.GammaRamp.Green[128] != 0;

    if (!bGammaRamp)
    {
        Log::Comment(String().Format(L"Warning: GammaRamp is not supported"));
    }

    // Set backlight optimization level to desktop. DC power settings is default here

    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET_OPTIMIZATION;
    EscapeData.OptimizationLevel = DxgkBacklightOptimizationDesktop;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Failed to set backlight optimization - desktop"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // Let the driver a time to settle down
    Sleep(1000);

    RtlZeroMemory(&EscapeData,sizeof(EscapeData));
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_REDUCTION;
    Status = CallEscape(&Escape);
    if (EscapeData.ReductionInfo.BacklightEffective > BaseData.ReductionInfo.BacklightEffective)
    {
        Log::Comment(String().Format(L"Backlight optimization for desktop failed. Effectivebacklight: actual = %d expected = %d",
            EscapeData.ReductionInfo.BacklightEffective,BaseData.ReductionInfo.BacklightEffective));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }
    if (EscapeData.ReductionInfo.BacklightUsersetting != BaseData.ReductionInfo.BacklightUsersetting)
    {
        Log::Comment(String().Format(L"Backlight optimization for desktop changed backlight user setting: actual = %d expected = %d",
            EscapeData.ReductionInfo.BacklightUsersetting,BaseData.ReductionInfo.BacklightUsersetting));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }

    if (bGammaRamp)
    {
        // check some seetings in the middle of the ramp (actaully we might want to calculate sum and verify it against base)
        if (EscapeData.ReductionInfo.GammaRamp.Green[128] < BaseData.ReductionInfo.GammaRamp.Green[128])
        {
            Log::Comment(String().Format(L"Backlight optimization for dynamic failed. Gamma ramp sample actual= %d base %d",
                EscapeData.ReductionInfo.GammaRamp.Green[128],BaseData.ReductionInfo.GammaRamp.Green[128]));
            Log::Result(WEX::Logging::TestResults::Failed);
//            return;
        }
    }

//    if (bGammaRamp)
//    {
//        if( !memcmp(&EscapeData.ReductionInfo.GammaRamp,&BaseData.ReductionInfo.GammaRamp,sizeof(BaseData.ReductionInfo.GammaRamp)) )
//        {
//            Log::Comment(String().Format(L"Backlight optimization for desktop failed. Gamma ramp should be different."));
//            Log::Result(WEX::Logging::TestResults::Failed);
//            return;
//        }
//    }

    // Set backlight optimization level to dynamic. Settings should not go up.

    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET_OPTIMIZATION;
    EscapeData.OptimizationLevel = DxgkBacklightOptimizationDynamic;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Failed to set backlight optimization - dynamic"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // Let the driver a time to settle down
    Sleep(1000);

    RtlZeroMemory(&EscapeData,sizeof(EscapeData));
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_REDUCTION;
    Status = CallEscape(&Escape);
    if (EscapeData.ReductionInfo.BacklightEffective > BaseData.ReductionInfo.BacklightEffective)
    {
        Log::Comment(String().Format(L"Backlight optimization for dynamic failed. Effectivebacklight: actual = %d base = %d",
            EscapeData.ReductionInfo.BacklightEffective,BaseData.ReductionInfo.BacklightEffective));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }
    if (EscapeData.ReductionInfo.BacklightUsersetting != BaseData.ReductionInfo.BacklightUsersetting)
    {
        Log::Comment(String().Format(L"Backlight optimization for dynamic changed backlight user setting: actual = %d expected = %d",
            EscapeData.ReductionInfo.BacklightUsersetting,BaseData.ReductionInfo.BacklightUsersetting));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }
    if (bGammaRamp)
    {
        // check some seetings in the middle of the ramp (actaully we might want to calculate sum and verify it against base)
        if (EscapeData.ReductionInfo.GammaRamp.Green[128] < BaseData.ReductionInfo.GammaRamp.Green[128])
        {
            Log::Comment(String().Format(L"Backlight optimization for dynamic failed. Gamma ramp sample actual= %d base %d",
                EscapeData.ReductionInfo.GammaRamp.Green[128],BaseData.ReductionInfo.GammaRamp.Green[128]));
            Log::Result(WEX::Logging::TestResults::Failed);
//            return;
        }
    }

    // Set backlight optimization level to dimmed. Settings should go down and be below some predetermined value.

//    UINT ScaleDimmedPst = 100; // it is not clear from the spec how low should it go for sure in any ambient conditions and other settings

    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_SET_OPTIMIZATION;
    EscapeData.OptimizationLevel = DxgkBacklightOptimizationDimmed;
    Status = CallEscape(&Escape);
    if (FAILED(Status))
    {
        Log::Comment(String().Format(L"Failed to set backlight optimization - dimmed"));
        Log::Result(WEX::Logging::TestResults::Failed);
        return;
    }

    // Let the driver a time to settle down and verify thst transition is started
    Sleep(1000);

    RtlZeroMemory(&EscapeData,sizeof(EscapeData));
    EscapeData.Type = D3DKMT_BRIGHTNESS_INFO_GET_REDUCTION;
    Status = CallEscape(&Escape);
    if (EscapeData.ReductionInfo.BacklightEffective > BaseData.ReductionInfo.BacklightEffective)
    {
        Log::Comment(String().Format(L"Backlight optimization for dimmed failed. It should start in less than 1 sec. Effectivebacklight: actual = %d base = %d",
            EscapeData.ReductionInfo.BacklightEffective,BaseData.ReductionInfo.BacklightEffective));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }
    if (EscapeData.ReductionInfo.BacklightUsersetting != BaseData.ReductionInfo.BacklightUsersetting)
    {
        Log::Comment(String().Format(L"Backlight optimization for dynamic changed backlight user setting: actual = %d expected = %d",
            EscapeData.ReductionInfo.BacklightUsersetting,BaseData.ReductionInfo.BacklightUsersetting));
        Log::Result(WEX::Logging::TestResults::Failed);
//        return;
    }
    if (bGammaRamp)
    {
        // check some seetings in the middle of the ramp (actaully we might want to calculate sum and verify it against base.
        if (EscapeData.ReductionInfo.GammaRamp.Green[128] < BaseData.ReductionInfo.GammaRamp.Green[128])
        {
            Log::Comment(String().Format(L"Backlight optimization for dynamic failed. It should start in less than 1 sec. Gamma ramp sample actual= %d base %d",
                EscapeData.ReductionInfo.GammaRamp.Green[128],BaseData.ReductionInfo.GammaRamp.Green[128]));
            Log::Result(WEX::Logging::TestResults::Failed);
//            return;
        }
    }
}

} /* namespace DevX */ } /* namespace GRFX */ } /* namespace BaseVideo */ 

#include <wbemidl.h>
#include "WMIHelper.h"

HRESULT IsBrightnessSupported()
{
    HRESULT hr = S_OK;
    IEnumWbemClassObject *pObjs = NULL;
    IWbemServices *pSvc = NULL;
    BSTR Query = NULL;
    BSTR Lang = NULL;

    CWMIHelper wmiHelper;

    hr = wmiHelper.Init(TEXT("root\\WMI"), NULL);
    if(FAILED(hr))
    {
        Log::Comment(String().Format(L"Failed to initialize Wmi"));
        return hr;
    }

    pSvc = wmiHelper.GetWMIServices();
    if(!pSvc)
    {
        return E_FAIL;
    }

    Query = SysAllocString(L"SELECT * from WmiMonitorBrightness");
    if(Query == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto CLEANUP_IWMI;
    }
    
    Lang = SysAllocString(L"WQL");
    if(Lang == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto CLEANUP_IWMI;
    }

    hr = pSvc->ExecQuery(Lang, Query, WBEM_FLAG_FORWARD_ONLY, NULL, &pObjs);
    if(FAILED(hr))
    {
        if(hr == wbemErrInvalidClass)
        {
            Log::Comment(String().Format(L"WmiMonitorBrightness class not registered. HR = %d", hr));
            hr = S_FALSE;
        }
        else if(hr == wbemErrNotSupported)
        {
            //No instances of brightness controls found
            hr = S_FALSE;
        }
        goto CLEANUP_IWMI;
    }

CLEANUP_IWMI:

    if(Query != NULL)
    {
        SysFreeString(Query);
    }
    if(Lang != NULL)
    {
        SysFreeString(Lang);
    }

    return hr;    	
}
