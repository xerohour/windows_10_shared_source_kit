// Copyright(C) Microsoft. All rights reserved.

//
// AudioEndpointVolumeAPITests.cpp
//  
//
// Description:
//
//  WASAPI IAudioEndpointVolume API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>


#include "StreamingControlTestFramework.h"
#include "EndpointVolumeEventHandler.h"

// GUID used for Endpoitn Volume API Test Contexts
// {A0662494-BC26-43e3-A5B4-268B721A79CE}
static const GUID GUID_ENDPOINTVOLUME_TEST = 
{ 0xa0662494, 0xbc26, 0x43e3, { 0xa5, 0xb4, 0x26, 0x8b, 0x72, 0x1a, 0x79, 0xce } };

const static float DB_TOLER = .0001f;
const static float SCALAR_TOLER = .1f;

const static float MIN_SCALAR_ATTEN = 0.0f;
const static float MAX_SCALAR_ATTEN = 1.0f;

DWORD TC_EndpointVolume_SetMasterVol_Valid0()
{    
    RunStreamingControlTest(ENDPOINTVOLUME_SETMASTERVOL_VALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMasterVol_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMASTERVOL_VALID1);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMasterVol_Valid2()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMASTERVOL_VALID2);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMasterVol_Valid3()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMASTERVOL_VALID3);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMasterVol_Scalar_Valid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMasterVol_Scalar_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID1);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMasterVol_Scalar_Valid2()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID2);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMasterVol_Scalar_Valid3()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMASTERVOL_SCALAR_VALID3);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMute_Valid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMUTE_VALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetMute_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETMUTE_VALID1);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_VolumeStepping_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_VOLUMESTEPPING_VALID1);
    return FNS_PASS;
}
    
DWORD TC_EndpointVolume_VolumeStepping_Valid2()
{
    RunStreamingControlTest(ENDPOINTVOLUME_VOLUMESTEPPING_VALID2);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_GetChannelCount_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETCHANNELCOUNT_VALID1);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUME_VALID1);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid2()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUME_VALID2);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid3()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUME_VALID3);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid4()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUME_VALID4);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevel_Valid5()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUME_VALID5);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID1);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid2()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID2);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid3()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID3);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid4()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID4);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Valid5()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUMESCALAR_VALID5);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_QueryHardwareSupport_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_QUERYHWSUPPORT_VALID1);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_GetVolumeRange_Valid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETVOLUMERANGE_VALID1);
    return FNS_PASS;
}

// invalid scenarios
DWORD TC_EndpointVolume_GetMasterVol_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETMASTERVOL_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_GetMasterVol_Scalar_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETMASTERVOL_SCALAR_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_GetMute_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETMUTE_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_VolumeStepping_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_VOLUMESTEPPING_INVALID0);
    return FNS_PASS;
}  

DWORD TC_EndpointVolume_GetChannelCount_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETCHANNELCOUNT_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevel_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUMELEVEL_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_GetChannelVolumeLevel_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETCHANNELVOLUMELEVEL_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_SetChannelVolumeLevelScalar_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_SETCHANNELVOLUMELEVELSCALAR_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_GetChannelVolumeLevelScalar_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETCHANNELVOLUMELEVELSCALAR_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_QueryHardwareSupport_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_QUERYHWSUPPORT_INVALID0);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_GetVolumeRange_Invalid1()
{
    RunStreamingControlTest(ENDPOINTVOLUME_GETVOLUMERANGE_INVALID1);
    return FNS_PASS;
}

DWORD TC_EndpointVolume_DisconnectService_Invalid0()
{
    RunStreamingControlTest(ENDPOINTVOLUME_DISCONNECTSERVICE_INVALID0);
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Stream audio and perform basic tests on the IAudioEndpointVolume::SetMasterVolumeLevel
//  method
//
// Return values (DWORD):
//     FNS_PASS on success, FAILURE is signalled via XLOG
//-------------------------------------------------------------------------
DWORD Test_EndpointVolume_SetMasterVol_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    FLOAT fCurrVol = 0.0f;
    FLOAT fMinDbVol = 0.0f;
    FLOAT fMaxDbVol = 0.0f;
    FLOAT fVolInc = 0.0f;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);
    
    
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevel(fMaxDbVol, &GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eFatalError, 
             "FAIL: IAudioEndpointVolume::SetMasterVolumeLevel failed for atten=%f",
             fMaxDbVol);
    }

    SLOG(eInfo1, "Verifying volume level...");
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevel(&fCurrVol));

    if(abs(fCurrVol-fMaxDbVol) > DB_TOLER)
    {
        XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fMaxDbVol, fCurrVol);
    }
    

    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevel(fMinDbVol, &GUID_ENDPOINTVOLUME_TEST));

    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError, 
             "FAIL: IAudioEndpointVolume::SetMasterVolumeLevel failed for atten=%f",
             fMinDbVol); 
    }

    Sleep(500);

    SLOG(eInfo1, "Verifying volume level...");
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevel(&fCurrVol));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
                
    if(abs(fCurrVol-(fMinDbVol)) > DB_TOLER)
    {
        XLOG(XFAIL, eFatalError, "FAIL: get %f and set %f volumes do not match", fMinDbVol, fCurrVol);
    }
    
    Sleep(500);

    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevel(fMaxDbVol, &GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError, 
             "FAIL: IAudioEndpointVolume::SetMasterVolumeLevel failed for atten=%f",
             fMaxDbVol);
    }

    SLOG(eInfo1, "Verifying volume level...");
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevel(&fCurrVol));

    if(abs(fCurrVol-(fMaxDbVol)) > DB_TOLER)
    {
        XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fMaxDbVol, fCurrVol);
    }    
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Stream audio and perform basic tests on the IAudioEndpointVolume::SetMasterVolumeLevel
//  method
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//-------------------------------------------------------------------------
DWORD Test_EndpointVolume_SetMasterVol_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    FLOAT fCurrVol = 0.0f;
    FLOAT fMinDbVol = 0.0f;
    FLOAT fMaxDbVol = 0.0f;
    FLOAT fVolInc = 0.0f;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);

    // fade volume up
    float volStep = max(1.0f, (fMaxDbVol - fMinDbVol) / 5.0f);
    for(float fVol = fMinDbVol; fVol <= fMaxDbVol; fVol += volStep)
    {
        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                          hr,
                          spEndpointVolume->SetMasterVolumeLevel(fVol, &GUID_ENDPOINTVOLUME_TEST));

        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: IAudioEndpointVolume::SetMasterVolumeLevel failed for atten=%f",
                 fVol);
        }
        else
        {            
            SLOG(eInfo1, "Verifying volume level...");
            fCurrVol = 0.0f;
            Sleep(100);
            
            METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                              hr,
                              spEndpointVolume->GetMasterVolumeLevel(&fCurrVol));

            if(abs(fCurrVol-fVol) > DB_TOLER)
            {
                XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
                goto END;
            }   
        }
    }

    // fade volume down
    for(float fVol = fMaxDbVol; fVol >= fMinDbVol; fVol -= volStep)
    {
        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                          hr,
                          spEndpointVolume->SetMasterVolumeLevel(fVol, &GUID_ENDPOINTVOLUME_TEST));

        if(FAILED(hr))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: IAudioEndpointVolume::SetMasterVolumeLevel failed for atten=%f",
                 fVol);
        }
        else
        {           
            SLOG(eInfo1, "Verifying volume level...");
            Sleep(500);
            fCurrVol = 0.0f;
            METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                              hr,
                              spEndpointVolume->GetMasterVolumeLevel(&fCurrVol));

            if(abs(fCurrVol-fVol) > DB_TOLER)
            {
                XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
            }    
        }
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Stream audio and perform basic tests on the IAudioEndpointVolume::SetMasterVolumeLevel
//  method.
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetMasterVol_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    float fCurrVol = 0.0f;
    FLOAT fMinDbVol = 0.0f;
    FLOAT fMaxDbVol = 0.0f;
    FLOAT fVolInc = 0.0f;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);

    // fade volume up
    for(float fVol = -150.0f; fVol < 150.0f; fVol += 5.0f)
    {
        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                          hr,
                          spEndpointVolume->SetMasterVolumeLevel(fVol, &GUID_ENDPOINTVOLUME_TEST));

        if((fVol>fMaxDbVol)||(fVol<fMinDbVol))
        {
            if(!FAILED(hr))
            {
                XLOG(XFAIL, eFatalError, "FAIL: Failure expected, for vol = %f dB", fVol);
                continue;              
            }

            //no verify
            continue;
        }
        else if(FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Set should succeed for vol = %f", fVol);
            continue;
        }

        SLOG(eInfo1, "Verifying volume level (top)");
        Sleep(5);

        fCurrVol = 0.0f;
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevel(&fCurrVol));
                          
        if(abs(fCurrVol-fVol) > DB_TOLER)
        {
            XLOG(XFAIL, eFatalError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
            continue;
        }    
    }
    
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Stream audio and perform basic tests on the IAudioEndpointVolume::SetMasterVolumeLevel
//  method.
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
#define VOLUME_CHANGE_WAIT_IN_MS 20 //some random number originally in the test
DWORD Test_EndpointVolume_SetMasterVol_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;

    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    float fMinDbVol = 0.0f, fMaxDbVol = 0.0f, fVolInc = 0.0f;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);

    for(int i = 0; i < 10; i++)
    {
        // rapidly fade volume up
        float volStep = max(2.0f, (fMaxDbVol - fMinDbVol) / 5.0f);
        for(float fVol = fMinDbVol; fVol <= fMaxDbVol; fVol += volStep)
        {
            XLOG(XMSG, eInfo1, "Setting volume to %0.2f(dB)", fVol);
            METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                              hr,
                              spEndpointVolume->SetMasterVolumeLevel(fVol, &GUID_ENDPOINTVOLUME_TEST));

            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }                 
            XLOG(XMSG, eInfo1, "Waiting %d(ms)", VOLUME_CHANGE_WAIT_IN_MS);
            Sleep(VOLUME_CHANGE_WAIT_IN_MS);
        }

        // fade volume down
        for(float fVol = fMaxDbVol; fVol >= fMinDbVol; fVol -= volStep)
        {
            XLOG(XMSG, eInfo1, "Setting volume to %0.2f(dB)", fVol);
            METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                              hr,
                              spEndpointVolume->SetMasterVolumeLevel(fVol,&GUID_ENDPOINTVOLUME_TEST));

            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }                           
            XLOG(XMSG, eInfo1, "Waiting %d(ms)", VOLUME_CHANGE_WAIT_IN_MS);
            Sleep(VOLUME_CHANGE_WAIT_IN_MS);
        }
    }

END:    
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic SetMasterVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetMasterVol_Scalar_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    float fCurrVol = 0.0f;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(MIN_SCALAR_ATTEN, &GUID_ENDPOINTVOLUME_TEST));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eInfo1, "FAIL: API under test returned unexpected result");
        goto END;
    }

    SLOG(eInfo1, "Verifying volume level...");
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));

    if(abs(fCurrVol-MIN_SCALAR_ATTEN) > SCALAR_TOLER)
    {
        XLOG(XFAIL, eFatalError, "FAIL: set %f and get %f volumes do not match", MIN_SCALAR_ATTEN, fCurrVol);
        goto END;
    }
    
    Sleep(500);

    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(MAX_SCALAR_ATTEN, &GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, "Verifying volume level...");
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));

    if(abs(fCurrVol-MAX_SCALAR_ATTEN) > SCALAR_TOLER)
    {
        XLOG(XFAIL, eFatalError, "FAIL: set %f and get %f volumes do not match", MAX_SCALAR_ATTEN, fCurrVol);
        goto END;
    }

END:    
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Method: TC_EndpointVolume_SetMasterVolumeLevelScalar_Basic_1()
//
// Description:
//  Basic SetMasterVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, FNS_FAIL on failure
//
DWORD Test_EndpointVolume_SetMasterVol_Scalar_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    float fCurrVol = 0.0f;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // fade volume up
    for(float fVol = MIN_SCALAR_ATTEN; fVol <= MAX_SCALAR_ATTEN; fVol += 0.2f)
    {
        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetMasterVolumeLevelScalar(fVol,&GUID_ENDPOINTVOLUME_TEST));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }

        SLOG(eInfo1, "Verifying volume level...");
        fCurrVol = 0.0f;
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));

        if(abs(fCurrVol-fVol) > SCALAR_TOLER)
        {
            XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
        }
    
        Sleep(2);
    }

    // fade volume down
    for(float fVol = MAX_SCALAR_ATTEN; fVol >= MIN_SCALAR_ATTEN; fVol -= 0.2f)
    {
        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetMasterVolumeLevelScalar(fVol,&GUID_ENDPOINTVOLUME_TEST));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }
        Sleep(100);

        SLOG(eInfo1, "Verifying volume level...");
        fCurrVol = 0.0f;
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));

        if(abs(fCurrVol-fVol) > SCALAR_TOLER)
        {
            XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
        }
    
        Sleep(2);
    }
   
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetMasterVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetMasterVol_Scalar_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    float fCurrVol = 0.0f;
    float fOldVol = 0.0f;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // fade volume down
    for(float fVol = 3.0f; fVol >= -3.0f; fVol -= 0.25f)
    {
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fOldVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "Error: Unable to obtain previous volume");
            continue;
        }

        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetMasterVolumeLevelScalar(fVol, &GUID_ENDPOINTVOLUME_TEST));

        if((fVol<MIN_SCALAR_ATTEN)||(fVol>MAX_SCALAR_ATTEN))
        {
            if(!FAILED(hr))
            {
                XLOG(XFAIL, eFatalError, "FAIL: Error expected, failing.");
                continue;
            }

            // no need to verify results
            continue;
        }
        else if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }

        SLOG(eInfo1, "Verifying volume level...");
        Sleep(50);

        fCurrVol = 0.0f;
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));

        if(abs(fCurrVol-fVol) > SCALAR_TOLER)
        {
            XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
            continue;
        }   
        Sleep(2);
    }



    // fade volume up
    for(float fVol = -3.0f; fVol <= 3.0f; fVol += 0.25f)
    {
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fOldVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            goto END;
        }

        METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetMasterVolumeLevelScalar(fVol, &GUID_ENDPOINTVOLUME_TEST));

        if((fVol<MIN_SCALAR_ATTEN)||(fVol>MAX_SCALAR_ATTEN))
        {
            if(!FAILED(hr))
            {
                XLOG(XFAIL, eFatalError, "FAIL: Error expected, failing.");
                continue;
            }

            // no need to verify results
            continue;
        }
        else if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }
       
        SLOG(eInfo1, "Verifying volume level...");
        Sleep(50);

        fCurrVol = 0.0f;
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));

        if(abs(fCurrVol-fVol) > SCALAR_TOLER)
        {
            XLOG(XFAIL, eFatalError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
            continue;
        }    
        Sleep(2);
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetMasterVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetMasterVol_Scalar_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    float fCurrVol = 0.0f;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    for(int i = 0; i < 2; i++)
    {
        // rapidly fade volume up
        for(float fVol = MIN_SCALAR_ATTEN; fVol <= MAX_SCALAR_ATTEN; fVol += .2f)
        {
            METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                              hr,
                              spEndpointVolume->SetMasterVolumeLevelScalar(fVol, &GUID_ENDPOINTVOLUME_TEST));

            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }

            SLOG(eInfo1, "Verifying volume level...");
            fCurrVol = 0.0f;
            METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }

            if(abs(fCurrVol-fVol) > SCALAR_TOLER)
            {
                XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
                goto END;
            }
        
            Sleep(2);
        }

        // fade volume down
        for(float fVol = MAX_SCALAR_ATTEN; fVol >= MIN_SCALAR_ATTEN; fVol -= .2f)
        {
            METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                              hr,
                              spEndpointVolume->SetMasterVolumeLevelScalar(fVol, &GUID_ENDPOINTVOLUME_TEST));

            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
                goto END;
            }

            SLOG(eInfo1, "Verifying volume level...");
            fCurrVol = 0.0f;
            METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }

            if(abs(fCurrVol-fVol) > SCALAR_TOLER)
            {
                XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);;
                goto END;
            }
        
            Sleep(2);
        }
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetMute tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetMute_Valid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    BOOL bMuteState = FALSE;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Mute the stream
    METHODCHECKRESULT("IAudioEndpointVolume::SetMute",
                      hr,
                      spEndpointVolume->SetMute(TRUE,&GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // get the stream mute state
    METHODCHECKRESULT("IAudioEndpointVolume::GetMute",
                      hr,
                      spEndpointVolume->GetMute(&bMuteState));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // verify the state
    if(TRUE != bMuteState)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Get and set mute values do not match!");
        goto END;
    }

    // Stream for .5 seconds
    Sleep(500);

    // Un-mute the stream
    METHODCHECKRESULT("IAudioEndpointVolume::SetMute",
                      hr,
                      spEndpointVolume->SetMute(FALSE,&GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // get the stream mute state
    METHODCHECKRESULT("IAudioEndpointVolume::GetMute",
                      hr,
                      spEndpointVolume->GetMute(&bMuteState));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // verify the state
    if(FALSE != bMuteState)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Get and set mute values do not match!");
        goto END;
    }

    // Stream for 0.5 seconds
    Sleep(500);
    
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic SetMute tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, FNS_FAIL on failure
//
DWORD Test_EndpointVolume_SetMute_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    BOOL bMuteState = FALSE;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // rapidly mute and un-mute the stream
    for(int i = 0; i < 2; i++)
    {
        // Stream for 0.1 seconds
        Sleep(100);

        // Mute the stream
        METHODCHECKRESULT("IAudioEndpointVolume::SetMute",
                          hr,
                          spEndpointVolume->SetMute(TRUE, &GUID_ENDPOINTVOLUME_TEST));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            goto END;
        }

        // get the stream mute state
        METHODCHECKRESULT("IAudioEndpointVolume::GetMute",
                          hr,
                          spEndpointVolume->GetMute(&bMuteState));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            goto END;
        }

        // verify the state
        if(TRUE != bMuteState)
        {
            XLOG(XFAIL, eError, "FAIL: Get and set mute values do not match!");
            goto END;
        }

        // Stream for .1 seconds
        Sleep(100);

        // Un-mute the stream
        METHODCHECKRESULT("IAudioEndpointVolume::SetMute",
                          hr,
                          spEndpointVolume->SetMute(FALSE, &GUID_ENDPOINTVOLUME_TEST));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            goto END;
        }

        // get the stream mute state
        METHODCHECKRESULT("IAudioEndpointVolume::GetMute",
                          hr,
                          spEndpointVolume->GetMute(&bMuteState));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            goto END;
        }

        // verify the state
        if(FALSE != bMuteState)
        {
            XLOG(XFAIL, eError, "FAIL: Get and set mute values do not match!");            
            goto END;
        }
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------- 
//
// Description:
//  BVT-type volume stepping tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_VolumeStepping_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    UINT nCurrStep = 0;
    UINT nSteps = 0;

    // validate get stepping info
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                      hr,
                      spEndpointVolume->GetVolumeStepInfo(&nCurrStep, &nSteps));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    // validate step up
    METHODCHECKRESULT("IAudioEndpointVolume::StepUp",
                      hr,
                      spEndpointVolume->VolumeStepUp(&GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    // validate step down
    METHODCHECKRESULT("IAudioEndpointVolume::StepDown",
                      hr,
                      spEndpointVolume->VolumeStepDown(&GUID_ENDPOINTVOLUME_TEST));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

END:    
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
// Stepping functional tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_VolumeStepping_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    UINT nSteps = 0;
    UINT nCurrStep = 0;
    UINT32 nObservedStep = 0;
    FLOAT fCurrVol = 0.0f;

    // set the endpoint volume to 0
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(MIN_SCALAR_ATTEN, &GUID_ENDPOINTVOLUME_TEST));                   
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                      hr,
                      spEndpointVolume->GetVolumeStepInfo(&nCurrStep, &nSteps));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    //
    // basic step-up test
    //
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(MIN_SCALAR_ATTEN, &GUID_ENDPOINTVOLUME_TEST));                    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                      hr,
                      spEndpointVolume->GetVolumeStepInfo(&nCurrStep, &nSteps));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    while(nCurrStep<(nSteps-1))
    {    
        UINT nObservedStep = 0;
    
        METHODCHECKRESULT("IAudioEndpointVolume::VolumeStepUp",
                          hr,
                          spEndpointVolume->VolumeStepUp(&GUID_ENDPOINTVOLUME_TEST));
        if(S_OK != hr)
        {
            XLOG(XFAIL, eError, "FAIL: Error: Expected StepUp to succeed");
        }

        METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                          hr,
                          spEndpointVolume->GetVolumeStepInfo(&nObservedStep, &nSteps))                         
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }

        if(nCurrStep+1 != nObservedStep)
        {
            XLOG(XFAIL, eError, "FAIL: Expected step %d, observed %d", nCurrStep+1, nObservedStep);
        }

        SLOG(eInfo1, "observed step: %d", nObservedStep);

       
        // fill up the volume table in the process
        METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));                
        nCurrStep++;   
    }

    // verify that the volume is maxed out
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevelScalar(&fCurrVol));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    if(MAX_SCALAR_ATTEN != fCurrVol)
    {
        XLOG(XFAIL,
             eFatalError, 
             "FAIL: Expected master volume = %f, observed %f",
             MAX_SCALAR_ATTEN,
             fCurrVol);
    }

    //                               
    // basic step-down test
    //
    while(nCurrStep>0)
    {       
        METHODCHECKRESULT("IAudioEndpointVolume::VolumeStepDown",
                          hr,
                          spEndpointVolume->VolumeStepDown(&GUID_ENDPOINTVOLUME_TEST));
        if(S_OK != hr)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Expected StepDown to succeed");
        }

        METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                           hr,
                           spEndpointVolume->GetVolumeStepInfo(&nObservedStep, &nSteps));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }        
        if(nCurrStep-1 != nObservedStep)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Expected step %d, observed %d", nCurrStep-1, nObservedStep);
        }

        SLOG(eInfo1, "observed step: %d", nObservedStep);           
        
        nCurrStep--;   
    }
    
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic GetChannelCount tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_GetChannelCount_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    UINT nChannels = 0;

    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eFatalError, "Current Channel  Count: %d", nChannels);

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetChannelVolumeLevel tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    FLOAT fMinDbVol = 0.0f, fMaxDbVol = 0.0f, fVolInc = 0.0f;
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);


    // Get the number of channels for the audiosession
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    // stream for 0.5 seconds
    Sleep(500);

    // set volume to silence
    for(UINT32 i = 0; i < nChannels; i++)
    {        
        float fCurrVol = 0.0f;

        METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                          hr,
                          spEndpointVolume->SetChannelVolumeLevel(i,fMinDbVol, &GUID_ENDPOINTVOLUME_TEST));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }

        METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                          hr,
                          spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }

        if((fCurrVol-(fMinDbVol))>DB_TOLER)
        {
            XLOG(XFAIL, eError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, -96.0f, i);
        }        
    }


    // set volume to full-on
    for(UINT32 i = 0; i < nChannels; i++)
    {        
        float fCurrVol = 0.0f;

        METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                          hr,
                          spEndpointVolume->SetChannelVolumeLevel(i,fMaxDbVol, &GUID_ENDPOINTVOLUME_TEST));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }

        METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                          hr,
                          spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }

        if((fCurrVol-(fMaxDbVol))>DB_TOLER)
        {
            XLOG(XFAIL, eError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, 0.0f, i);
        }        
    }

END:    
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic SetChannelVolumeLevel tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    FLOAT fMinDbVol = 0.0f, fMaxDbVol = 0.0f, fVolInc = 0.0f;
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);


    // Get the number of channels for the audiosession
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    // fade the volume from sVolInfo.fMaxDbVol -> sVolInfo.fMinDbVol for each channel
     float volStep = max(2.0f, (fMaxDbVol - fMinDbVol) / 5.0f);
    for(float fSetVol = fMaxDbVol; fSetVol >= fMinDbVol; fSetVol -= volStep)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fCurrVol = 0.0f;
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevel(i,fSetVol, &GUID_ENDPOINTVOLUME_TEST));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            if((fCurrVol-(fSetVol))>DB_TOLER)
            {
                XLOG(XFAIL, eError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, fSetVol, i);
            }        

            Sleep(5);
        }     
    }

    // fade the volume from sVolInfo.fMinDbVol -> sVolInfo.fMaxDbVol for each channel
    for(float fSetVol = fMinDbVol; fSetVol < fMaxDbVol; fSetVol += volStep)
    {
        for(UINT32 i=0; i < nChannels; i++)
        {        
            float fCurrVol = 0.0f;
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevel(i,fSetVol, &GUID_ENDPOINTVOLUME_TEST));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            if((fCurrVol-(fSetVol))>DB_TOLER)
            {
                XLOG(XFAIL, eError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, fSetVol, i);
            }        

            Sleep(5);
        }     
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetChannelVolumeLevel tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    FLOAT fMinDbVol = 0.0f, fMaxDbVol = 0.0f, fVolInc = 0.0f;
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);


    // Get the number of channels for the audiosession
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    // fade the volume from MAX -> MIN for even channels
    // fade the volume from MIN -> MAX for odd channels
    float volStep = max(.5f, (fMaxDbVol - fMinDbVol) / 5.0f);
    for(float fSetVol = fMaxDbVol; fSetVol >= fMinDbVol; fSetVol -= volStep)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fChanSetVol = 0.0f;
            float fCurrVol = 0.0f;

            // even channel setting
            if(i%2 == 0)
            {
                fChanSetVol = fSetVol;
            }
            // odd channel setting
            else
            {
                fChanSetVol = fMaxDbVol - abs(fMinDbVol - fSetVol);
            }
            SLOG(eInfo1, "set channel vol: %f", fChanSetVol);
            
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevel(i,fChanSetVol, &GUID_ENDPOINTVOLUME_TEST));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }
            
            if((fCurrVol-(fChanSetVol))>DB_TOLER)
            {      
                XLOG(XFAIL, eError, "Get %f and set %f volumes for channel %d do not match", fCurrVol, fChanSetVol, i);
            }        

            Sleep(10);
        }     
    }

    // fade the volume from MIN -> MAX for even channels
    // fade the volume from MAX -> MIN for even channels
    for(float fSetVol = fMinDbVol; fSetVol <= fMaxDbVol; fSetVol += volStep)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fChanSetVol = 0.0f;
            float fCurrVol = 0.0f;

            // even channel setting
            if(i%2 == 0)
            {
                fChanSetVol = fSetVol;
            }
            // odd channel setting
            else
            {
                fChanSetVol = fMaxDbVol - abs(fMinDbVol - fSetVol);
            }
            SLOG(eInfo1, "set channel vol: %f", fChanSetVol);

            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevel(i,fChanSetVol, &GUID_ENDPOINTVOLUME_TEST));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }

            if((fCurrVol-(fChanSetVol))>DB_TOLER)
            {
                XLOG(XFAIL, eFatalError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, fChanSetVol, i);
            }        

            Sleep(10);
        }     
    }
END:    
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic SetChannelVolumeLevel tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    FLOAT fMinDbVol = 0.0f, fMaxDbVol = 0.0f, fVolInc = 0.0f;
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    // fade the volume from +100db -> -150dB for each channel
    for(float fSetVol = 100.0f; fSetVol > -150.0f; fSetVol -= 25.0f)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fCurrVol = 0.0f;
            
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevel(i,fSetVol, &GUID_ENDPOINTVOLUME_TEST));
            if((fSetVol>fMaxDbVol)||(fSetVol<fMinDbVol))
            {
                if(!FAILED(hr))
                {                   
                    XLOG(XFAIL, eError, "FAIL: Failure expected for invalid attenuation");
                    continue;                 
                }

                // no need to validate results
                continue;
            }
            else if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Method should not fail for valid attenuation");
                continue;
            }

            Sleep(50);

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            if(abs(fCurrVol-(fSetVol))>DB_TOLER)
            {
                XLOG(XFAIL, eFatalError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, fSetVol, i);
            }                    
        }     
    }

    // fade the volume from -150db -> +100dB for each channel
    for(float fSetVol = -150.0f; fSetVol < 100.0f; fSetVol += 25.0f)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fCurrVol = 0.0f;
            
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevel(i,fSetVol, &GUID_ENDPOINTVOLUME_TEST));
            if((fSetVol>fMaxDbVol)||(fSetVol<fMinDbVol))
            {
                if(!FAILED(hr))
                {                   
                    XLOG(XFAIL, eError, "FAIL: Failure expected for invalid attenuation");
                    continue;
                }

                // no need to validate results
                continue;
            }
            else if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: method should not fail for valid attenuation");
                continue;
            }

            Sleep(50);

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            if(abs(fCurrVol-(fSetVol))>DB_TOLER)
            {
                XLOG(XFAIL, eFatalError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, fSetVol, i);
            }                    
        }     
    }


END:
    return FNS_PASS;
}

//------------------------------------------------------------------------- 
//
// Description:
//  Basic SetChannelVolumeLevel tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevel_Valid5(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    FLOAT fMinDbVol = 0.0f, fMaxDbVol = 0.0f, fVolInc = 0.0f;
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fMinDbVol, &fMaxDbVol, &fVolInc));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    SLOG(eInfo1, 
         "Min Endpoint Vol: %f\n Max Endpoint Vol: %f\n",
         fMinDbVol,
         fMaxDbVol);

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    for(int i = 0; i < 2; i++)
    {
        // for each channel
        for(UINT32 i = 0; i < nChannels; i++)
        {
            float fCurrVol = 0.0f;
            
            // rapidly fade volume up
            float volStep = max(2.0f, (fMaxDbVol - fMinDbVol) / 5.0f);
            for(float fVol = fMinDbVol; fVol <= fMaxDbVol; fVol += volStep)
            {
                METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                                  hr,
                                  spEndpointVolume->SetChannelVolumeLevel(i,fVol, &GUID_ENDPOINTVOLUME_TEST));

                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
                    continue;
                }

                SLOG(eInfo1, "Verifying volume level...");
                fCurrVol = 0.0f;
                METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                                  hr,
                                  spEndpointVolume->GetChannelVolumeLevel(i, &fCurrVol));
                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
                    continue;
                }

                if(abs(fCurrVol-fVol) > DB_TOLER)
                {
                    XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
                    continue;
                }
            
                Sleep(2);
            }

            // fade volume down
            for(float fVol = fMaxDbVol; fVol > fMinDbVol; fVol -= volStep)
            {
                METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolume",
                                  hr,
                                  spEndpointVolume->SetChannelVolumeLevel(i, fVol, &GUID_ENDPOINTVOLUME_TEST));

                if(FAILED(hr))
                {
                    continue;
                }

                SLOG(eInfo1, "Verifying volume level...");
                fCurrVol = 0.0f;

                METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolume",
                                  hr,
                                  spEndpointVolume->GetChannelVolumeLevel(i,&fCurrVol));
                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
                    continue;
                }

                if(abs(fCurrVol-fVol) > DB_TOLER)
                {
                    XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
                    continue;
                }
            
                Sleep(2);
            }
        }
    }
    
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic GetChannelVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    // stream for 0.5 seconds
    Sleep(500);

    // set volume to silence
    for(UINT32 i = 0; i < nChannels; i++)
    {        
        float fCurrVol = 0.0f;

        METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetChannelVolumeLevelScalar(i,MIN_SCALAR_ATTEN, &GUID_ENDPOINTVOLUME_TEST));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
            continue;
        }

        METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
            continue;
        }

        if((fCurrVol-(MIN_SCALAR_ATTEN))>SCALAR_TOLER)
        {
            XLOG(XFAIL, eError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, 0.0f, i);
        }        
    }


    // set volume to full-on
    for(UINT32 i = 0; i < nChannels; i++)
    {        
        float fCurrVol = 0.0f;

        METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetChannelVolumeLevelScalar(i,MAX_SCALAR_ATTEN,&GUID_ENDPOINTVOLUME_TEST));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }

        METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }

        if((fCurrVol-(MAX_SCALAR_ATTEN))>SCALAR_TOLER)
        {
            XLOG(XFAIL, eError, "ERROR: Get %f and set %f volumes for channel %d do not match", fCurrVol, 1.0f, i);
        }        
    }


END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic SetChannelVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid2(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    // fade the volume from 1.0 -> 0.0 for each channel
    for(float fSetVol = MAX_SCALAR_ATTEN; fSetVol > MIN_SCALAR_ATTEN; fSetVol -= .2f)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fCurrVol = 0.0f;
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevelScalar(i,fSetVol,&GUID_ENDPOINTVOLUME_TEST));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }

            if((fCurrVol-(fSetVol))>DB_TOLER)
            {
                XLOG(XFAIL, eError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, fSetVol, i);
            }        

            Sleep(10);
        }     
    }

    // fade the volume from 0.0f -> 1.0 for each channel
    for(float fSetVol = MIN_SCALAR_ATTEN; fSetVol < MAX_SCALAR_ATTEN; fSetVol += .2f)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fCurrVol = 0.0f;
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevelScalar(i,fSetVol,&GUID_ENDPOINTVOLUME_TEST));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }

            if((fCurrVol-(fSetVol))>SCALAR_TOLER)
            {
                XLOG(XFAIL, eError, "Get %g and set %g volumes for channel %d do not match", fCurrVol, fSetVol, i);
            }        

            Sleep(10);
        }     
    }
 
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetChannelVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid3(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    // fade the volume from 0 -> 1.0 for even channels
    // fade the volume from 1.0 -> 0.0dB for odd channels
    for(float fSetVol = MIN_SCALAR_ATTEN; fSetVol <= MAX_SCALAR_ATTEN; fSetVol += .2f)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fChanSetVol = 0.0f;
            float fCurrVol = 0.0f;

            // even channel setting
            if(i%2 == 0)
            {
                fChanSetVol = fSetVol;
            }
            // odd channel setting
            else
            {
                fChanSetVol = max(MIN_SCALAR_ATTEN, MAX_SCALAR_ATTEN-fSetVol);
            }

            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevelScalar(i,fChanSetVol, &GUID_ENDPOINTVOLUME_TEST));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }
            
            if((fCurrVol-(fChanSetVol))>SCALAR_TOLER)
            {      
                XLOG(XFAIL, eError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, fChanSetVol, i);
            }        

            Sleep(10);
        }     
    }

    // fade the volume from 1.0 -> 0.0 for even channels
    // fade the volume from 0.0 -> 1.0dB for even channels
    for(float fSetVol = MAX_SCALAR_ATTEN; fSetVol >= MIN_SCALAR_ATTEN; fSetVol -= .2f)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fChanSetVol = 0.0f;
            float fCurrVol = 0.0f;

            // even channel setting
            if(i%2 == 0)
            {
                fChanSetVol = fSetVol;
            }
            // odd channel setting
            else
            {
                fChanSetVol = max(MIN_SCALAR_ATTEN, MAX_SCALAR_ATTEN-fSetVol);
            }
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevelScalar(i,fChanSetVol, &GUID_ENDPOINTVOLUME_TEST));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                goto END;
            }

            if((fCurrVol-(fChanSetVol))>SCALAR_TOLER)
            {
                XLOG(XFAIL, eFatalError, "FAIL: Get %f and set %f volumes for channel %d do not match", fCurrVol, fChanSetVol, i);
            }        

            Sleep(10);
        }     
    }
    
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetChannelVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid4(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    // set all channels to full volume
    for(UINT32 i = 0; i < nChannels; i++)
    {
        METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                          hr,
                          spEndpointVolume->SetChannelVolumeLevelScalar(i,
                                                                        MAX_SCALAR_ATTEN,
                                                                        &GUID_ENDPOINTVOLUME_TEST));
        if(FAILED(hr))
        {            
            XLOG(XFAIL, eError, "FAIL: Unable to set channel volume to %g", MAX_SCALAR_ATTEN);            
        }
    }
    

    // fade the volume from 3 -> -3 for each channel
    for(float fSetVol = 3.0f; fSetVol > -3.0f; fSetVol -= 0.5f)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            FLOAT fCurrVol = 0.0f;
            
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevelScalar(i,fSetVol, &GUID_ENDPOINTVOLUME_TEST));

            if((fSetVol>MAX_SCALAR_ATTEN)||(fSetVol<MIN_SCALAR_ATTEN))
            {
                if(SUCCEEDED(hr))
                {
                    XLOG(XFAIL, eFatalError, "FAIL: Failure expected for volume set with invalid attenuation");
                    continue;
                }

                // no need to verify results
                continue;
            }
            else if(FAILED(hr))
            {
                XLOG(XFAIL, eFatalError, "FAIL: Failed to set volume %f", fSetVol);
                continue;
            }

            Sleep(50);
          
            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }

            if((fCurrVol-(fSetVol))>SCALAR_TOLER)
            {
                XLOG(XFAIL,
                     eFatalError, 
                     "FAIL: Get %f and set %f volumes for channel %d do not match", 
                     fCurrVol, 
                     fSetVol, 
                     i);
            }
            
            Sleep(10);
        }     
    }


    // fade the volume from -3.0 -> 3.0 for each channel
    for(float fSetVol = -3.0f; fSetVol <= 3.0f; fSetVol += 0.5f)
    {
        for(UINT32 i = 0; i < nChannels; i++)
        {        
            float fCurrVol = 0.0f;
                        
            METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->SetChannelVolumeLevelScalar(i,fSetVol, &GUID_ENDPOINTVOLUME_TEST));

            if((fSetVol>MAX_SCALAR_ATTEN)||(fSetVol<MIN_SCALAR_ATTEN))
            {
                if(SUCCEEDED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: Failure expected for volume set with invalid attenuation");

                    // no need to validate results
                    continue;
                }
                else
                {
                    // accept either failure or success in this range
                    continue;
                }
            }
            else if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Failed to set volume %f", fSetVol);
                continue;
            }

            Sleep(50);
          
            METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                              hr,
                              spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }

            if((fCurrVol-(fSetVol))>SCALAR_TOLER)
            {
                XLOG(XFAIL,
                     eError, 
                     "FAIL: Get %f and set %f volumes for channel %d do not match", 
                     fCurrVol, 
                     fSetVol, 
                     i);
            }
            
            Sleep(10);
        }     
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetChannelVolumeLevelScalar tests
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Valid5(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;

    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    if(nChannels == 0)
    {
        XLOG(XFAIL, eError, "FAIL: No channels for which to set volume!");
        goto END;
    }

    for(int i = 0; i < 2; i++)
    {
        // for each channel
        for(UINT32 i = 0; i < nChannels; i++)
        {
            float fCurrVol = 0.0f;
            
            // rapidly fade volume up
            for(float fVol = 0.0f; fVol <= 1.0f; fVol += .2f)
            {
                METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                                  hr,
                                  spEndpointVolume->SetChannelVolumeLevelScalar(i,fVol,&GUID_ENDPOINTVOLUME_TEST));

                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                    goto END;
                }

                SLOG(eInfo1, "Verifying volume level...");
                fCurrVol = 0.0f;
                METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                                  hr,
                                  spEndpointVolume->GetChannelVolumeLevelScalar(i, &fCurrVol));

                if(abs(fCurrVol-fVol) > SCALAR_TOLER)
                {
                    XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
                    continue;
                }
            
                Sleep(2);
            }

            // fade volume down
            for(float fVol = 1.0f; fVol > 0.0f; fVol -= .2f)
            {
                METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                                  hr,
                                  spEndpointVolume->SetChannelVolumeLevelScalar(i, fVol, &GUID_ENDPOINTVOLUME_TEST));

                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                }

                SLOG(eInfo1, "Verifying volume level...");
                fCurrVol = 0.0f;
                METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                                  hr,
                                  spEndpointVolume->GetChannelVolumeLevelScalar(i,&fCurrVol));

                if(abs(fCurrVol-fVol) > SCALAR_TOLER)
                {
                    XLOG(XFAIL, eError, "FAIL: get %f and set %f volumes do not match", fCurrVol, fVol);
                    goto END;
                }
            
                Sleep(2);
            }
        }
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic QueryHardwareSupport Secenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_QueryHardwareSupport_Valid1(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;

    DWORD dwHardwareSupport = 0;
   
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::QueryHardwareSupport",
                      hr,
                      spEndpointVolume->QueryHardwareSupport(&dwHardwareSupport));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    SLOG(eInfo1, "IAudioEndpointVolume::QueryHardwareSupport Reports \"0x%08X\"", dwHardwareSupport);

    // mask off lower bits
    dwHardwareSupport &= 0x0000000F;
    
    // make sure the returned value is reasonable
    if(dwHardwareSupport > (DWORD)(0x07))
    {
        XLOG(XFAIL, eError, "FAIL: Returned value (%08X) is not valid!", dwHardwareSupport);
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Valid Scenarios for IAudioEndpointVolume::GetVolumeRange
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_GetVolumeRange_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    FLOAT fminVolDb = 0.0f;
    FLOAT fmaxVolDb = 0.0f;
    FLOAT fstepVolDb = 0.0f;

    FLOAT fCurrDbVolume = 0.0f;

    //
    // basic get
    //
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fminVolDb, &fmaxVolDb, &fstepVolDb));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: GetVolumeRange Failed!");
        goto END;
    }
        
    // ensure max != min volume
    if(fminVolDb == fmaxVolDb)
    {
        XLOG(XFAIL,
             eError,
             "FAIL: min (%f) and max (%f) volumes can not be equal!",
             fminVolDb,
             fmaxVolDb);
    }

    //
    // Verify that setting scalar volumes to 0.0f results in a dB volume of fminVolDb
    // Verify that setting scalar volumes to 1.0f results in a dB volume of fmaxVolDb
    //

    // master volume min case
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,                           
                      spEndpointVolume->SetMasterVolumeLevelScalar(0.0f, NULL));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
    
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevel(&fCurrDbVolume));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }


    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fminVolDb, &fmaxVolDb, &fstepVolDb));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    
    SLOG(eInfo1, "Scalar Master Volume Level: %f", 0.0f);
    SLOG(eInfo1, "Reported Master Volume Level: %f dB", fCurrDbVolume);
    SLOG(eInfo1, "Reported Min dB Volume: %f dB", fminVolDb);

    if(EPSILON < abs(fminVolDb - fCurrDbVolume))
    {
        XLOG(XFAIL, eError, "FAIL: Reported and actual min master dB volumes do not match!");
    }

    // master volume max case
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,                           
                      spEndpointVolume->SetMasterVolumeLevelScalar(1.0f, NULL));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevel(&fCurrDbVolume));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }


    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fminVolDb, &fmaxVolDb, &fstepVolDb));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
    
    
    SLOG(eInfo1, "Scalar Master Volume Level: %f", 0.0f);
    SLOG(eInfo1, "Reported Master Volume Level: %f dB", fCurrDbVolume);
    SLOG(eInfo1, "Reported Max dB Volume: %f dB", fmaxVolDb);

    if(EPSILON < abs(fmaxVolDb - fCurrDbVolume))
    {
        XLOG(XFAIL, eError, "FAIL: Reported and actual max master dB volumes do not match!");
    }

    // channel volume min case
    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                           hr,                           
                           spEndpointVolume->SetChannelVolumeLevelScalar(0, 0.0f, NULL));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                           hr,
                           spEndpointVolume->GetChannelVolumeLevel(0, &fCurrDbVolume));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                           hr,
                           spEndpointVolume->GetVolumeRange(&fminVolDb, &fmaxVolDb, &fstepVolDb));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
    
    SLOG(eInfo1, "Scalar Channel Volume Level: %f", 0.0f);
    SLOG(eInfo1, "Reported Channel Volume Level: %f dB", fCurrDbVolume);
    SLOG(eInfo1, "Reported Min dB Volume: %f dB", fminVolDb);

    if(EPSILON < abs(fminVolDb - fCurrDbVolume))
    {
        XLOG(XFAIL, eError, "FAIL: Reported and actual min channel dB volumes do not match!");
    }


    // channel volume max case
    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                      hr,                           
                      spEndpointVolume->SetChannelVolumeLevelScalar(0, 1.0f, NULL));                      
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                       hr,
                       spEndpointVolume->GetChannelVolumeLevel(0, &fCurrDbVolume));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
    
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fminVolDb, &fmaxVolDb, &fstepVolDb));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
    SLOG(eInfo1, "Scalar Channel Volume Level: %f", 1.0f);
    SLOG(eInfo1, "Reported Channel Volume Level: %f dB", fCurrDbVolume);
    SLOG(eInfo1, "Reported Max dB Volume: %f dB", fmaxVolDb);

    if(EPSILON < abs(fmaxVolDb - fCurrDbVolume))
    {
        SLOG(eFatalError, "FAIL: Reported and actual max master dB volumes do not match!");
    }    

    // channel volume min case
    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                      hr,                           
                      spEndpointVolume->SetChannelVolumeLevelScalar(0, 0.0f, NULL));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
    
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevel(0, &fCurrDbVolume));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }

    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fminVolDb, &fmaxVolDb, &fstepVolDb));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
    }
    
    SLOG(eInfo1, "Scalar Channel Volume Level: %f", 0.0f);
    SLOG(eInfo1, "Reported Channel Volume Level: %f dB", fCurrDbVolume);
    SLOG(eInfo1, "Reported Min dB Volume: %f dB", fminVolDb);

    if(EPSILON < abs(fminVolDb - fCurrDbVolume))
    {
        XLOG(XFAIL, eError, "FAIL: Reported and actual min master dB volumes do not match!");
    }    
        
END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Stream audio run various invalid scenarios on the 
// IAudioEndpointVolume::SetMasterVolumeLevel method.
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_GetMasterVol_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
   
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevel(NULL));
    if(!(FAILED(hr)))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Invalid call to GetMasterVolumeLevel should have failed!");
        goto END;
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
//
// Description:
//  Basic GetMasterVolumeLevelScalar tests covering invalid scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_GetMasterVol_Scalar_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
   
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevelScalar(NULL));
    if(!(FAILED(hr)))
    {
        XLOG(XFAIL, eError, "FAIL: Invalid call to GetMasterVolumeLevelScalar should have failed!");
        goto END;
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Basic SetMute tests covering invalid scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, FNS_FAIL on failure
//
DWORD Test_EndpointVolume_GetMute_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
   
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // call GetMute with a NULL parameter
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetMute",
                      hr,
                      spEndpointVolume->GetMute(NULL));
    if(!(FAILED(hr)))
    {
        XLOG(XFAIL, eFatalError, "FAIL: GetMute call should not succeed with a NULL parameter!");
        goto END;
    }    
    
END:    
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  Volume stepping invalid scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_VolumeStepping_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
   
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    UINT32 nParam = 0;

    // null step
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                      hr,
                      spEndpointVolume->GetVolumeStepInfo(&nParam, NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test should have failed");
    }

    // null step count
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                      hr,
                      spEndpointVolume->GetVolumeStepInfo(NULL, &nParam));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test should have failed");
    }

    // two bad args
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                      hr,
                      spEndpointVolume->GetVolumeStepInfo(NULL, NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test should have failed");
    }

END:    
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
// Description:
//  Basic GetChannelCount tests covering invalid scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_GetChannelCount_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
   
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(NULL));

    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "GetChannelCount should not succeed with a NULL pointer");
        goto END;
    }

END:
    return FNS_PASS;
}   

DWORD Test_EndpointVolume_SetChannelVolumeLevel_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // call SetChannelVolume level with invalid channel indices
    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->SetChannelVolumeLevel(nChannels, 0.0f, &GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded!");    
        goto END;
    }

    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->SetChannelVolumeLevel(nChannels+1, 0.0f, &GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded!");    
        goto END;
    }

END:
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
// Description:
//  Basic GetChannelVolumeLevel tests covering invalid scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_GetChannelVolumeLevel_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;
    FLOAT fVolLevel = 0.0f;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    for(UINT32 i = 0; i < nChannels; i++)
    {
        // call SetChannelVolumeLevel with a NULL pointer
#pragma prefast(suppress:6309, "Disable parameter checking")
        METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                          hr,
                          spEndpointVolume->GetChannelVolumeLevel(i, NULL));
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Method succeeded with NULL parameter!");
        }
    }    

    // call SetChannelVolumeLevel with an invalid channel index
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevel(nChannels, &fVolLevel));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method succeeded for an invalid channel index");
    }

    // call SetChannelVolumeLevel with an invalid channel index
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevel(nChannels+1, &fVolLevel));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method succeeded for an invalid channel index");
    }

    // call SetChannelVolumeLevel with two invalid params
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevel(nChannels+1, NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "Method succeeded with two bad params");
    }

END:
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
//
// Description:
//  Basic SetChannelVolumeLevelScalar tests covering invalid scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_SetChannelVolumeLevelScalar_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    // call SetChannelVolume level with invalid channel indices
    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetChannelVolumeLevelScalar(nChannels, 0.0f, &GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded!");    
    }

    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->SetChannelVolumeLevelScalar(nChannels+1, 0.0f, &GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded!");    
        goto END;
    }

END:
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
//
// Description:
//  Basic GetChannelVolumeLevelScalar tests covering invalid scenarios
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_GetChannelVolumeLevelScalar_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    UINT32 nChannels = 0;
    FLOAT fVolLevel = 0.0f;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Get the number of channels for the endpoint
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelCount",
                      hr,
                      spEndpointVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        goto END;
    }

    for(UINT32 i = 0; i < nChannels; i++)
    {
        // call SetChannelVolumeLevel with a NULL pointer
#pragma prefast(suppress:6309, "Disable parameter checking")
        METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                          hr,
                          spEndpointVolume->GetChannelVolumeLevelScalar(i, NULL));
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method succeeded with NULL parameter!");
        }
    }    

    // call SetChannelVolumeLevel with an invalid channel index
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevelScalar(nChannels, &fVolLevel));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method succeeded for an invalid channel index");
    }

    // call SetChannelVolumeLevel with an invalid channel index
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevelScalar(nChannels+1, &fVolLevel));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method succeeded for an invalid channel index");
    }

    // call SetChannelVolumeLevel with two invalid params
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevelScalar(nChannels+1, NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method succeeded with two bad params");
    }
    
END:
    return FNS_PASS;
}   

DWORD Test_EndpointVolume_QueryHardwareSupport_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::QueryHardwareSupport",
                      hr,
                      spEndpointVolume->QueryHardwareSupport(NULL));
    if(E_POINTER != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded with NULL param!");
    }

END:
    return FNS_PASS;
}   

//-------------------------------------------------------------------------
//
// Description:
//  Invalid Scenarios for IAudioEndpointVolume::GetVolumeRange
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, FNS_FAIL on failure
//
DWORD Test_EndpointVolume_GetVolumeRange_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL; 

    FLOAT fminVolDb = 0.0f;
    FLOAT fmaxVolDb = 0.0f;
    FLOAT fstepVolDb = 0.0f;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(NULL, &fmaxVolDb, &fstepVolDb));                   

    if(E_POINTER != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed with E_POINTER with 1 or more NULL params!");
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(NULL, &fmaxVolDb, NULL));                   
    if(E_POINTER != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed with E_POINTER with 1 or more NULL params!");
    }
    
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fminVolDb, NULL, NULL));                   
    if(E_POINTER != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed with E_POINTER with 1 or more NULL params!");
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(NULL, NULL, &fstepVolDb));                   
    if(E_POINTER != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed with E_POINTER with 1 or more NULL params!");
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(NULL, NULL, NULL));                   
    if(E_POINTER != hr)
    {
        XLOG(XFAIL, eError, "FAIL: Method should have failed with E_POINTER with 1 or more NULL params!");
    }

END:
    return FNS_PASS;
}

//-------------------------------------------------------------------------
// Description:
//  RPC Failure Tests for IAudioEndpointVolume
//    
// Parameters:
//      N/A
//
// Return values (DWORD):
//     FNS_PASS on success, failure is signalled via XLOG
//
DWORD Test_EndpointVolume_DisconnectService_Invalid0(DWORD dwDeviceType, ITestStreamingEngine * pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioEndpointVolume> spEndpointVolume = NULL;
    CComPtr<IAudioEndpointVolumeCallback> spEndpointEventHandler = NULL;

    UINT nChannelCount = 0;
    FLOAT fVolLevel = 0.0f;
    BOOL bMuteVal = FALSE;
    UINT nStep=0, nStepCount=0;

    CEndpointVolumeEventHandler* pobjEndpointEventHandler = NULL;
    
    hr = pStreamingEngine->GetAudioEndpointVolume(&spEndpointVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioEndpointVolume interface");
        goto END;
    }

    // Create and endpoint volume notification handler
    pobjEndpointEventHandler = new CEndpointVolumeEventHandler();
    if(NULL == pobjEndpointEventHandler)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }        

    METHODCHECKRESULT("IAudioEndpointVolumeHandler::QueryInterface",
                      hr,
                      pobjEndpointEventHandler->QueryInterface(IID_IAudioEndpointVolumeCallback,
                                                              (void**)(&spEndpointEventHandler)));

    if(FAILED(hr))
    {
        goto END;
    }

    // shut down audiosrv
    if (!StopAudioService())
    {
        XLOG(XFAIL, eError, "FAIL: Unable to stop AudioSrv!");
        goto END;
    }

    // BEGIN IAudioEndpointVolume API Calls
    ////

    // GetChannelCount
    METHODCHECKRESULT("IAudioEndpointVolume::GetGetChannelCount",
                                       hr,
                                       spEndpointVolume->GetChannelCount(&nChannelCount));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // SetMasterVolumeLevel
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevel",
                                       hr,
                                       spEndpointVolume->SetMasterVolumeLevel(0.0f,&GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // SetMasterVolumeLevelScalar
    METHODCHECKRESULT("IAudioEndpointVolume::SetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetMasterVolumeLevelScalar(1.0f,&GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetMasterVolumeLevel
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevel",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevel(&fVolLevel));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetMasterVolumeLevelScalar
    METHODCHECKRESULT("IAudioEndpointVolume::GetMasterVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetMasterVolumeLevelScalar(&fVolLevel));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }
    // SetChannelVolumeLevel
    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->SetChannelVolumeLevel(1,0.0f,&GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // SetChannelVolumeLevelScalar
    METHODCHECKRESULT("IAudioEndpointVolume::SetChannelVolumeLevelScalar",
                      hr,
                      spEndpointVolume->SetChannelVolumeLevelScalar(1,1.0f,&GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetChannelVolumeLevel
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevel",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevel(1,&fVolLevel));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetChannelVolumeLevelScalar
    METHODCHECKRESULT("IAudioEndpointVolume::GetChannelVolumeLevelScalar",
                      hr,
                      spEndpointVolume->GetChannelVolumeLevelScalar(1,&fVolLevel));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // SetMute
    METHODCHECKRESULT("IAudioEndpointVolume::SetMute",
                      hr,
                      spEndpointVolume->SetMute(FALSE,&GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetMute
    METHODCHECKRESULT("IAudioEndpointVolume::GetMute",
                      hr,
                      spEndpointVolume->GetMute(&bMuteVal));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    // GetVolumeStepInfo
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeStepInfo",
                      hr,
                      spEndpointVolume->GetVolumeStepInfo(&nStep, &nStepCount));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");    }

    // VolumeStepUp
    METHODCHECKRESULT("IAudioEndpointVolume::VolumeStepUp",
                      hr,
                      spEndpointVolume->VolumeStepUp(&GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");    }

    // VolumeStepDown
    METHODCHECKRESULT("IAudioEndpointVolume::VolumeStepDown",
                      hr,
                      spEndpointVolume->VolumeStepDown(&GUID_ENDPOINTVOLUME_TEST));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");    }

    //RegisterControlChangeNotify
    METHODCHECKRESULT("IAudioEndpointVolume::RegisterControlChangeNotify",
                      hr,
                      spEndpointVolume->RegisterControlChangeNotify(spEndpointEventHandler));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");    
    }

    // GetVolumeRange
    METHODCHECKRESULT("IAudioEndpointVolume::GetVolumeRange",
                      hr,
                      spEndpointVolume->GetVolumeRange(&fVolLevel, &fVolLevel, &fVolLevel));        
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded");
    }

    ////
    // END IAudioEndpointVolume API Calls

END:
    // Start audiosrv back up. Moving this into end section so that even if this function fails, the following test cases do not fail.
    if (!StartAudioService())
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to re-start AudioSrv! System may be in an unusable state!");
    }

    SAFE_RELEASE_POINTER(pobjEndpointEventHandler);
    return FNS_PASS;
}   
