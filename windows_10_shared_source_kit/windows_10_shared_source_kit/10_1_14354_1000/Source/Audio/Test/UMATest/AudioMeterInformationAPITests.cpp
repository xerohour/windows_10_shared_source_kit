// Copyright(C) Microsoft. All rights reserved.

//
// AudioMeterInformationAPITests.cpp
//  
//
// Description:
//
//  WASAPI IAudioEndpointVolume API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>


#include "StreamingControlTestFramework.h"

DWORD TC_AudioMeterInf_Valid0()
{
    RunStreamingControlTest(AUDMETERINF_VALID0);
    return FNS_PASS;
}

DWORD TC_AudioMeterInf_Valid1()
{
    RunStreamingControlTest(AUDMETERINF_VALID1);
    return FNS_PASS;
}

DWORD TC_AudioMeterInf_Invalid0()
{
    RunStreamingControlTest(AUDMETERINF_INVALID0);
    return FNS_PASS;
}

DWORD TC_AudioMeterInf_DisconnectService()
{
    RunStreamingControlTest(AUDMETERINF_DISCONNECTSERVICE);
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic Metering APO tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_AudioMeterInf_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioMeterInformation> spMeter = NULL;

    FLOAT fPeakVal;
    FLOAT* pfChannelsPeaks = NULL;
    UINT nChannels = 0;

    hr = pStreamingEngine->GetAudioMeterInformation(&spMeter);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioMeterInformation interface (0x%x)", hr);
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetPeakValue",
                      hr,
                      spMeter->GetPeakValue(&fPeakVal));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // get the current number of channels  
    METHODCHECKRESULT("IAudioEndpointVolume::GetMeteringChannelCount",
                      hr,                      
                      spMeter->GetMeteringChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(NULL == (pfChannelsPeaks = new float[nChannels]))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelsPeakValue",
                      hr,                      
                      spMeter->GetChannelsPeakValues(nChannels, pfChannelsPeaks));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

END:
    SAFE_DELETE_ARRAY(pfChannelsPeaks);
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
// Description:
//     Extended metering tests.
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_AudioMeterInf_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioMeterInformation> spMeter = NULL;

    FLOAT fPeakValue = 0.0f;
    FLOAT* pfChannelsPeaks = NULL;
    UINT nChannels = 0;    

    hr = pStreamingEngine->GetAudioMeterInformation(&spMeter);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioMeterInformation interface (0x%x)", hr);
        goto END;
    }
   
    SLOG(eInfo1, "Testing GetPeakValue");
    for(int count=0;count<10;count++)
    {        
        METHODCHECKRESULT("IAudioMeterInformation::GetPeakValue",
                          hr,
                          spMeter->GetPeakValue(&fPeakValue));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }

        Sleep(1);
    }

    // get the current number of channels  
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,                      
                      spMeter->GetMeteringChannelCount(&nChannels));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            goto END;
        }

    if(NULL == (pfChannelsPeaks = new float[nChannels]))
    {
        SLOG(eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    for(int count=0;count<10;count++)
    {    
        for(UINT32 i=0;i<nChannels;i++)
        {          
            METHODCHECKRESULT("IAudioMeterInformation::GetChannelsPeakValue",
                              hr,                      
                              spMeter->GetChannelsPeakValues(nChannels,pfChannelsPeaks));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }
        }
        Sleep(1);
    }        

END:
    SAFE_DELETE_ARRAY(pfChannelsPeaks);
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
// Description:
//  Basic Metering API invalid scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_AudioMeterInf_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioMeterInformation> spMeter = NULL;

    FLOAT* pfChannelsPeaks = NULL;
    UINT nChannels = 0;
  
    hr = pStreamingEngine->GetAudioMeterInformation(&spMeter);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioMeterInformation interface (0x%x)", hr);
        goto END;
    }

    // Call GetPeakValue with a NULL pointer
    METHODCHECKRESULT("IAudioMeterInformation::GetPeakValue",
                      hr,
                      spMeter->GetPeakValue(NULL));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "Method should have failed with a NULL param!");
    }

    // Call GetChannelCount with a NULL pointer
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioMeterInformation::GetMeteringChannelCount",
                      hr,
                      spMeter->GetMeteringChannelCount(NULL));
    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "Method should have failed with a NULL param!");
    }    

    // get the current number of channels  
    METHODCHECKRESULT("IAudioEndpointVolume::GetMeteringChannelCount",
                      hr,                      
                      spMeter->GetMeteringChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(NULL == (pfChannelsPeaks = new FLOAT[nChannels+1]))
    {
        XLOG(XFAIL, eError, "FAIL: Out of memory!");
        goto END;
    }


    // Call GetChannelsPeakValues with a NULL param
    METHODCHECKRESULT("IAudioMeterInformation::GetChannelsPeakValues",
                      hr,                      
                      spMeter->GetChannelsPeakValues(nChannels,NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed with a NULL param!");
    }

    // Call GetChannelsPeakValues with an invalid channel count
    METHODCHECKRESULT("IAudioMeterInformation::GetChannelsPeakValues",
                      hr,                      
                      spMeter->GetChannelsPeakValues(0,pfChannelsPeaks));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed with invalid channel count!");
    }

    // Call GetAverageRMSMetering with an invalid channel count and an NULL pointer
    METHODCHECKRESULT("IAudioMeterInformation::GetChannelsPeakValues",
                      hr,                      
                      spMeter->GetChannelsPeakValues(0,NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have failed multiple invalid params!");
    }
    
END:
    SAFE_DELETE_ARRAY(pfChannelsPeaks);
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
// Description:
//  RPC Failure Tests for IAudioMeterInformation
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_AudioMeterInf_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioMeterInformation> spMeter = NULL;

    FLOAT fMeterVal = 0;
    const UINT METER_CHANNELS = 2;
    FLOAT fMeterVals[METER_CHANNELS] = {0.0f, 0.0f};


    hr = pStreamingEngine->GetAudioMeterInformation(&spMeter);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioMeterInformation interface (0x%x)", hr);
        goto END;
    }

    // shut down audiosrv
    if (!StopAudioService())
    {
        XLOG(XFAIL, eError, "FAIL: Unable to stop AudioSrv!");
        goto END;
    }

    // Begin Scenario
    ////

    // GetPeakValue
    METHODCHECKRESULT("IAudioMeterInformation::GetPeakValue",
                      hr,
                      spMeter->GetPeakValue(&fMeterVal));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetChannelsRMS
    METHODCHECKRESULT("IAudioMeterInformation::GetChannelsPeakValues",
                      hr,
                      spMeter->GetChannelsPeakValues(METER_CHANNELS, fMeterVals));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    ////
    // End Scenario

END:
    // Start audiosrv back up. Moving this into end section so that even if this function fails, the following test cases do not fail.
    if (!StartAudioService())
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to re-start AudioSrv! System may be in an unusable state!");
    }
    return FNS_PASS;
}   
