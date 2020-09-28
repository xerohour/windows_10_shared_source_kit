//
// Copyright(C) Microsoft.All rights reserved.
//
// LullabyTests.cpp
//
// Description:
//
//  Lullaby Test Implementation
//

#include <stdafx.h>
#include "powrprof.h"
#include "powrprofp.h"
#include "ntpoapi.h"

#include "initguid.h"
#include "lpe.h"
#include "QueryMachineInfo.h"
#include "AudioOffload.h"
#include "DevTrans.h"
#include "DetectBluetooth.h"

//******************************************************************************
//  CTestCase class implementation methods
//******************************************************************************
BOOL CTestCase::Initialize( void )
{
   return TRUE;
}

// ----------------------------------------------------------
void CTestCase::CleanUp ( void )
{

};

// ----------------------------------------------------------
DWORD CTestCase::RunPreTest( void )
{
    return FNS_NOTIMPLEMENTED;
};

// ----------------------------------------------------------
DWORD CTestCase::RunPostTest( void )
{
    return FNS_NOTIMPLEMENTED;
};

// ----------------------------------------------------------
DWORD CTestCase::RunDuringTest( INT /*nSyncronous*/ )
{
    return FNS_NOTIMPLEMENTED;
};


HRESULT GetSystemPowerCapabilities(SYSTEM_POWER_CAPABILITIES* SysPowerCaps)
{
    HRESULT hr = S_OK;
    NTSTATUS status;
    status = NtPowerInformation(SystemPowerCapabilities, NULL, 0, SysPowerCaps, sizeof(SYSTEM_POWER_CAPABILITIES));
    if (!NT_SUCCESS(status))
    {
        hr = HRESULT_FROM_NT(status);
        XLOG(XFAIL, eError, L"FAIL: Failed Querying for system power capabilites with (hr = %s)", GetHRString(hr));
        return hr;
    }

    return hr;
}

HRESULT IsEndpointUnderTestDigitalDisplay(BOOL& bIsDigital)
{
    HRESULT hr = S_OK;
    CLullabyDevice* pCurrentDevice = NULL;
    CComPtr<IMMDevice> spDeviceUnderTest = NULL;
    
    if (NULL == (pCurrentDevice = g_pLullabyDeviceTestModule->GetDevice()))
    {
        XLOG(XFAIL, eError, L"FAIL: IMMDevice under test was null!");
        return FALSE;
    }

    hr = pCurrentDevice->GetEndpoint(&spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: GetEndpoint encountered error (hr = %s).", GetHRString(hr));
        return hr;
    }

    EndpointFormFactor eFormFactor = EndpointFormFactor::UnknownFormFactor;
    hr = DevTrans::GetEndpointFormFactor(&eFormFactor, spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"Failed getting endpoint form factor with (hr = %s)", GetHRString(hr));
        return hr;
    }

    bIsDigital = (EndpointFormFactor::DigitalAudioDisplayDevice == eFormFactor);
    return hr;
}

// ----------------------------------------------------------
BOOL CheckForDeviceAvailable()
{
    CLullabyDevice* pCurrentDevice = NULL;
    CComPtr<IMMDevice> spDeviceUnderTest = NULL;
    CComPtr<IAudioClient> spAudioClient = NULL;
    //  Get the endpoint under test
    if (NULL == (pCurrentDevice = g_pLullabyDeviceTestModule->GetDevice()))
    {
        XLOG(XFAIL, eError, L"FAIL: IMMDevice under test was null!");
        return FALSE;
    }

    HRESULT hr = pCurrentDevice->GetEndpoint(&spDeviceUnderTest);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: GetEndpoint encountered error (hr = %s).", GetHRString(hr));
        return FALSE;
    }

    hr = spDeviceUnderTest->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&spAudioClient);
    if (AUDCLNT_E_DEVICE_INVALIDATED == hr)
    {
        //try again for 30s
        UINT i = 0;
        for (i = 0; i < 30 && AUDCLNT_E_DEVICE_INVALIDATED == hr; i++)
        {
            Sleep(1000);
            hr = spDeviceUnderTest->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&spAudioClient);
        }

        if (FAILED(hr))
        {
            XLOG(XFAIL, eError, L"Failed Activating MMDevice(hr = %s) after %d seconds", GetHRString(hr), i);
            return FALSE;
        }
        else if (30 == i && hr != S_OK)
        {
            //we got through the loop but the device isnt ready.
            XLOG(XFAIL, eError, L"Device was not ready within 30 seconds of the machine returning from sleep");
            return FALSE;
        }
    }
    else if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"Failed Activating MMDevice(hr = %s)", GetHRString(hr));
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------
DWORD Tc_Common(CTestCase* pTestCase)
{
    CComPtr<IBasicLog> pLogger;
    HRESULT hr = g_IShell->GetBasicLog(&pLogger);
    if(FAILED(hr))
    {
        XLOG(XMSG, eError, L"FAIL: Unable to create IBasicLog!");
        return FALSE;
    }

    //check if machine supports power state we are testing, skip if it does not
    SYSTEM_POWER_CAPABILITIES Capabilites = {0};
    
    hr = GetSystemPowerCapabilities(&Capabilites);
    if (FAILED(hr))
    {
        XLOG(XMSG, eError, L"FAIL: Unable query system power capabilities.");
        return FALSE;
    }

    if (PowerSystemSleeping3 == pTestCase->m_spsState)
    {
        if (!Capabilites.SystemS3)
        {
            XLOG( XMSG, eInfo2, L"Machine does not support S3. Skipping S3 testing...");
            return FNS_SKIPPED;
        }
    }
    else if (PowerSystemHibernate == pTestCase->m_spsState)
    {
        if (!Capabilites.SystemS4 || !Capabilites.HiberFilePresent)
        {
            XLOG( XMSG, eInfo2, L"Machine does not support S4. Skipping S4 testing...");
            return FNS_SKIPPED;
        }
    }
    else if (TRUE == pTestCase->m_bIsCSTest)
    {
        //skip if we are not AOAC Capable
        bool bIsAOAC = false;

        HRESULT hr = QueryMachineInfo::IsAOACCapable(pLogger, bIsAOAC);
        if (FAILED(hr))
        {
            XLOG( XMSG, eInfo2, L"Failed to determine AOAC Support(%s), skipping tests.", GetHRString(hr));
            return FNS_SKIPPED;
        }

        if (false == bIsAOAC)
        {
            XLOG( XMSG, eInfo2, L"Machine does not support AOAC, skipping tests...");
            return FNS_SKIPPED;
        }

        // we are a CS test and we support AOAC, but we need to skip if the endpoint is a digital display since they 
        // can't stream audio without a video signal being present too
        BOOL bIsEndpointDigitalDisplay = FALSE;
        hr = IsEndpointUnderTestDigitalDisplay(bIsEndpointDigitalDisplay);
        if (FAILED(hr))
        {
            XLOG( XMSG, eInfo2, L"Failed to determine query if device is a digital output with (hr = %s)", GetHRString(hr));
            return FALSE;
        }

        if (TRUE == bIsEndpointDigitalDisplay)
        {
            XLOG( XMSG, eInfo2, L"Endpoint under test is a digital output. Skipping...");
            return FNS_SKIPPED;
        }
    }

    DWORD dwRes = FNS_FAIL;
    XLOG( XMSG, eInfo1, L"%s", pTestCase->m_szTestName );
    if( FALSE == pTestCase->Initialize() )
    {
        //special case for offload return
        if (FNS_SKIPPED == pTestCase->m_dwResPre)
        {
            dwRes = FNS_SKIPPED;
        }
        XLOG( XMSG, eError, L"Failed Testcase Initialization");
        goto Exit;
    }

    dwRes = pTestCase->RunPreTest();
    if( PASSED(dwRes) == FALSE )
    {
        XLOG( XMSG, eError, L"Failed PreTest");
        goto Exit;
    }

    if (TRUE == pTestCase->m_bIsCSTest)
    {
        //run during CS to test state entry and power settings
        // pretest of the CS Classes put the machine to CS
        dwRes = pTestCase->m_dwResPost = pTestCase->RunDuringTest(pTestCase->m_nSynchronous);
        if( PASSED(dwRes) == FALSE )
        {
            goto Exit;
        }
    }
    else
    {
        //not cs, put the machine to sleep
        hr = (SuspendAndResume(pLogger, XFAIL, pTestCase->m_spsState, pTestCase->m_nSleep));
        //special case ERROR_NOT_SUPPORTED for resume timers
        if (HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) == hr)
        {
           //already logged not supporting in the SleepResume lib
            dwRes = FNS_SKIPPED;
            goto Exit;
        }
        else if (FAILED(hr))
        {
            dwRes = FNS_FAIL;
            goto Exit;
        }
    }
    
    dwRes = pTestCase->RunPostTest();
    if( PASSED(dwRes) == FALSE )
    {
        XLOG( XMSG, eError, L"Failed Post Test");
        goto Exit;
    }

Exit:
    pTestCase->CleanUp();
    return dwRes;
}

DWORD TC_Render_Sync(SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api, BOOL bOffload)
{
    CRenderTest TestCase(SYNC, spsState, nSleep, nSnooze, api, bOffload);
    return Tc_Common(&TestCase);
}
DWORD TC_Render_Async(SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api, BOOL bOffload)
{
    CRenderTest TestCase(ASYNC, spsState, nSleep, nSnooze, api, bOffload);
    return Tc_Common(&TestCase);
}
DWORD TC_Capture_Sync(SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api)
{
    CCaptureTest TestCase(SYNC, spsState, nSleep, nSnooze, api);
    return Tc_Common(&TestCase);
}
DWORD TC_Capture_Async(SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api)
{
    CCaptureTest TestCase(ASYNC, spsState, nSleep, nSnooze, api);
    return Tc_Common(&TestCase);
}

DWORD TC_WASAPI_Render_Sync_S3()
{
    return TC_Render_Sync(PowerSystemSleeping3, 35000, 10000, api_WASAPI, FALSE);
}
DWORD TC_WASAPI_Render_Sync_SH()
{
    return TC_Render_Sync(PowerSystemHibernate, 40000, 15000, api_WASAPI, FALSE);
}
DWORD TC_WASAPI_Render_Offload_Sync_S3()
{
    return TC_Render_Sync(PowerSystemSleeping3, 35000, 10000, api_WASAPI, TRUE);
}
DWORD TC_WASAPI_Render_Offload_Sync_SH()
{
    return TC_Render_Sync(PowerSystemHibernate, 40000, 15000, api_WASAPI, TRUE);
}

DWORD TC_WASAPI_Render_Async_S3()
{
    return TC_Render_Async(PowerSystemSleeping3, 35000, 10000, api_WASAPI, FALSE);
}
DWORD TC_WASAPI_Render_Async_SH()
{
    return TC_Render_Async(PowerSystemHibernate, 40000, 15000, api_WASAPI, FALSE);
}
DWORD TC_WASAPI_Render_Offload_Async_S3()
{
    return TC_Render_Async(PowerSystemSleeping3, 35000, 10000, api_WASAPI, TRUE);
}
DWORD TC_WASAPI_Render_Offload_Async_SH()
{
    return TC_Render_Async(PowerSystemHibernate, 40000, 15000, api_WASAPI, TRUE);
}

DWORD TC_WASAPI_Capture_Sync_S3()
{
    return TC_Capture_Sync(PowerSystemSleeping3, 35000, 10000, api_WASAPI);
}
DWORD TC_WASAPI_Capture_Sync_SH()
{
    return TC_Capture_Sync(PowerSystemHibernate, 40000, 15000, api_WASAPI);
}

DWORD TC_WASAPI_Capture_Async_S3()
{
    return TC_Capture_Async(PowerSystemSleeping3, 35000, 10000, api_WASAPI);
}
DWORD TC_WASAPI_Capture_Async_SH()
{
    return TC_Capture_Async(PowerSystemHibernate, 40000, 15000, api_WASAPI);
}

DWORD TC_WASAPI_Render_Sync_CS()
{
    CCSRenderTest TestCase(SYNC, api_WASAPI, FALSE, FALSE);
    return Tc_Common(&TestCase);
}
DWORD TC_WASAPI_Render_Async_CS()
{
    CCSRenderTest TestCase(ASYNC, api_WASAPI, FALSE, FALSE);
    return Tc_Common(&TestCase);
}
DWORD TC_WASAPI_Render_Offload_Sync_CS()
{
    CCSRenderTest TestCase(SYNC, api_WASAPI, TRUE, FALSE);
    return Tc_Common(&TestCase);
}
DWORD TC_WASAPI_Render_Offload_Async_CS()
{
    CCSRenderTest TestCase(ASYNC, api_WASAPI, TRUE, FALSE);
    return Tc_Common(&TestCase);
}
/*
DWORD TC_WASAPI_Render_Async_CS_Glitch()
{
    CCSRenderTest TestCase(ASYNC, api_WASAPI, FALSE, TRUE);
    return Tc_Common(&TestCase);
}
*/
DWORD TC_WASAPI_Render_Offload_Async_CS_Glitch()
{
    CCSRenderTest TestCase(ASYNC, api_WASAPI, TRUE, TRUE);
    return Tc_Common(&TestCase);
}
DWORD TC_WASAPI_Capture_Async_CS()
{
    CCSCaptureTest TestCase(ASYNC, api_WASAPI, FALSE);
    return Tc_Common(&TestCase);
}
DWORD TC_WASAPI_Capture_Sync_CS()
{
    CCSCaptureTest TestCase(SYNC, api_WASAPI, FALSE);
    return Tc_Common(&TestCase);
}
DWORD TC_WASAPI_Capture_Async_CS_Glitch()
{
    CCSCaptureTest TestCase(ASYNC, api_WASAPI, TRUE);
    return Tc_Common(&TestCase);
}

DWORD TC_Verify_BTH()
{
    //this should only run in the system tests
#ifndef BUILD_TAEF
    if(NULL != g_pLullabyDeviceTestModule->m_pShell->m_pszHwIdTestDeviceSelect &&
        0 != lstrcmp(L"", g_pLullabyDeviceTestModule->m_pShell->m_pszHwIdTestDeviceSelect))
    {
        XLOG( XSKIP, eInfo1, L"Bluetooth presence test is run only in the system kit. Skipping...");
        return FNS_SKIPPED;
    }
#else
    using namespace WEX::Common;
    using namespace WEX::TestExecution;
    String targetDevice;
    if (SUCCEEDED(RuntimeParameters::TryGetValue(L"InstanceId", targetDevice)))
    {
        XLOG( XSKIP, eInfo1, L"Bluetooth presence test is run only in the system kit. Skipping...");
        return FNS_SKIPPED;
    }
#endif

    //
    // This test only applies to Windows 6.3 and above
    //
    OSVERSIONINFO ver = {0};
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (!GetVersionEx(&ver))
    {
        XLOG(XFAIL, eFatalError, L"FAIL: GetVersionEx() failed: GetLastError() = %u", GetLastError());
        return FNS_FAIL;
    }

    XLOG(XMSG, eInfo1, L"INFO: OS version is %u.%u", ver.dwMajorVersion, ver.dwMinorVersion);

    if (ver.dwMajorVersion < 6 || (ver.dwMajorVersion == 6 && ver.dwMinorVersion < 3))
    {
        XLOG(XSKIP, eInfo1, L"INFO: This test case is only applicable to Windows versions 6.3 and above.");
        return FNS_SKIPPED;
    }

    BOOL bBthRunnging = FALSE;
    UINT cDevices = 0;

    CComPtr<IBasicLog> pLogger;
    HRESULT hr = g_IShell->GetBasicLog(&pLogger);
    if(FAILED(hr))
    {
        XLOG(XMSG, eError, L"FAIL: Unable to create IBasicLog!");
        return FNS_FAIL;
    }

    //see if there is a bth service running
    hr = DetectBluetooth::IsBthServiceRunning(pLogger, bBthRunnging);
    if(FAILED(hr))
    {
        XLOG(XMSG, eError, L"FAIL: Unable to Query Bth Services (%s)", GetHRString(hr));
        return FNS_FAIL;
    }

    if (FALSE == bBthRunnging)
    {
        XLOG(XMSG, eInfo1, L"Bluetooth is not running on the system");
        return FNS_SKIPPED;
    }

    //see how many devices are present
    hr = DetectBluetooth::BluetoothAudioDeviceCount(pLogger, &cDevices);
    if(FAILED(hr))
    {
        XLOG(XMSG, eError, L"FAIL: Unable to Query Bth device count (%s)", GetHRString(hr));
        return FNS_FAIL;
    }

    if (0 == cDevices)
    {
        XLOG(XMSG, eError, L"FAIL: BLuetooth is present on the system but there are no Bluetooth Audio Devices attached."
            L"\nPlease attach one and rerun these tests.");
        return FNS_FAIL;
    }
    else
    {
        XLOG(XMSG, eError, L"PASS: BLuetooth is present on the system and there are %d Bluetooth Audio Devices", cDevices);
    }

    return FNS_PASS;
}

DWORD TC_Verify_SleepResume()
{
    DWORD dwRes = FNS_PASS;
    CComPtr<IBasicLog> pLogger;
    HRESULT hr = g_IShell->GetBasicLog(&pLogger);
    if(FAILED(hr))
    {
        XLOG(XMSG, eError, L"FAIL: Unable to create IBasicLog!");
        return FNS_FAIL;
    }
    hr = (SuspendAndResume(pLogger, XFAIL, PowerSystemSleeping3, 35000));
    //special case ERROR_NOT_SUPPORTED for resume timers
    if (HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) == hr)
    {
       //already logged not supporting in the SleepResume lib
        dwRes = FNS_SKIPPED;
        goto Exit;
    }
    else if (FAILED(hr))
    {
        dwRes = FNS_FAIL;
        goto Exit;
    }
    hr = (SuspendAndResume(pLogger, XFAIL, PowerSystemHibernate, 40000));
    //special case ERROR_NOT_SUPPORTED for resume timers
    if (HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED) == hr)
    {
       //already logged not supporting in the SleepResume lib
        dwRes = FNS_SKIPPED;
        goto Exit;
    }
    else if (FAILED(hr))
    {
        dwRes = FNS_FAIL;
        goto Exit;
    }

Exit:
    return FNS_PASS;
}
