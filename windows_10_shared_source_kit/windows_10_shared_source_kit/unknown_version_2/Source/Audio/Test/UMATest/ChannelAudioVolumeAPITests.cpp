// Copyright(C) Microsoft. All rights reserved.

//
// ChannelAudioVolumeAPITests.cpp
//  
//
// Description:
//
//  WASAPI IChannelAudioVolume API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>


#include "StreamingControlTestFramework.h"

// GUID used for ChannelVolume API Test Contexts
// {275F8087-750A-48c1-A9D9-35F6775CB626}
static const GUID CHANNELVOLUME_TEST_CONTEXT = 
{ 0x275f8087, 0x750a, 0x48c1, { 0xa9, 0xd9, 0x35, 0xf6, 0x77, 0x5c, 0xb6, 0x26 } };

DWORD TC_ChannelAudioVolume_GetChannelCount_Valid0()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETCHANNELCOUNT_VALID0);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetChannelVolume_Valid0()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETCHANNELVOLUME_VALID0);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetChannelVolume_Valid1()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETCHANNELVOLUME_VALID1);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetSetAllVolumes_Valid0()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETSETALLVOLUMES_VALID0);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetSetAllVolumes_Valid1()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETSETALLVOLUMES_VALID1);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetChannelCount_Invalid0()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETCHANNELCOUNT_INVALID0);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetChannelVolume_Invalid0()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID0);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetChannelVolume_Invalid1()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID1);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetChannelVolume_Invalid2()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETCHANNELVOLUME_INVALID2);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetChannelVolume_Invalid0()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID0);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetChannelVolume_Invalid1()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID1);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetChannelVolume_Invalid2()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETCHANNELVOLUME_INVALID2);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetAllVolumes_Invalid0()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID0);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetAllVolumes_Invalid1()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID1);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetAllVolumes_Invalid2()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID2);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_SetAllVolumes_Invalid3()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_SETALLVOLUMES_INVALID3);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetAllVolumes_Invalid0()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID0);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetAllVolumes_Invalid1()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID1);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_GetAllVolumes_Invalid2()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_GETALLVOLUMES_INVALID2);
    return FNS_PASS;   
}

DWORD TC_ChannelAudioVolume_DisconnectService()
{
    RunStreamingControlTest(CHANNELAUDIOVOLUME_DISCONNECTSERVICE);
    return FNS_PASS;   
}

//------------------------------------------------------------------------------
// Desc: Test the basic functionality of GetChannelCount using 1-10 channels
//
// Returns:
//              FNS_PASS (Failure signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetChannelCount_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    UINT32 nChannelCount = 0;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannelCount));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Call to GetChannelCount failed!");
        goto END;
    }
    else if(nChannelCount <= 0)
    {
        XLOG(XFAIL, eError, "FAIL: Invalid channel count: %d", nChannelCount);
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify SetChannelVolume behaves appropriately in basic situations
//
// Returns:
//              FNS_PASS (Failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_SetChannelVolume_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

                  
    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                  hr,
                  spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }
                              
    // mono-channel tests
    if(1 == nChannels)
    {
        float fGetVol = 0.0;
        float fSetVol = 0.5;
        
        SLOG(eInfo1, "Running mono-channel SetChannelVolume tests...");

        // set the per-channel volume to a known value
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(0,
                                                            fSetVol,
                                                            &(CHANNELVOLUME_TEST_CONTEXT)));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test failed for channel 0, volume = %f", fSetVol);
        }

        Sleep(500);

        // verify that the set was effective
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(0,&fGetVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
        }

        if(EPSILON < abs(fSetVol-fGetVol))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: Set (%f) and obtained (%f) volumes do not match!",
                 fSetVol,
                 fGetVol);
        }
    }

    // stereo-channel tests
    else if(2 == nChannels)
    {
        float fGetVol = 0.0f;
        float fSetVols[] = {0.3f,0.6f};

        SLOG(eInfo1, "Running stereo-channel SetChannelVolume tests...");

        // set the per-channel volumes to known values
        for(int nSetChannel=0;nSetChannel<2;nSetChannel++)
        {
            METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                              hr,
                              spChannelVolume->SetChannelVolume(nSetChannel,fSetVols[nSetChannel],&(CHANNELVOLUME_TEST_CONTEXT)));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
            }
        }     

        Sleep(500);
                                                         
        // verify that the sets were effective
        for(int nGetChannel=0;nGetChannel<2;nGetChannel++)
        {
            METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                              hr,
                              spChannelVolume->GetChannelVolume(nGetChannel,&fGetVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
            }

            if(EPSILON < abs(fGetVol-fSetVols[nGetChannel]))
            {
                XLOG(XFAIL,
                     eError, 
                     "FAIL: Set (%f) and obtained (%f) volumes do not match!",
                     fSetVols[nGetChannel],
                     fGetVol);
            }
        }
    
    }
    else
    {
        SLOG(eError, "This test does not apply to multichannel (>2) endpoints.");
    }

END:   
    return FNS_PASS;   
}


//------------------------------------------------------------------------------
// Desc: More rigorous testing of IAudioChannelVolume::SetChannelVolume in valid
//       scenarios
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_SetChannelVolume_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                  hr,
                  spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }

    if(1 == nChannels)
    {
        SLOG(eInfo1,"\n\nTesting SetChannelVolume for monoaural audio",nChannels);
    }
    else if(2 == nChannels)
    {
        SLOG(eInfo1,"\n\nTesting SetChannelVolume for stereo audio",nChannels);
    }
    else
    {
        SLOG(eInfo1,"\n\nTesting SetChannelVolume for %d-channel audio",nChannels);
    }
              
    // give all channels unique volumes
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        float fSetVol = ((float)nCurChannel)/(2.0f*((float)nChannels));

        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(nCurChannel,fSetVol,&(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
        }
    }
    // verify channel volumes
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        float fExpectedVol = ((float)nCurChannel)/(2.0f*((float)nChannels));
        float fActualVol = -1.0f;
        
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel,&fActualVol));               

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
        }

        if(EPSILON < abs(fExpectedVol-fActualVol))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: set (%f) and retrieved (%f) volumes do not match!",
                 fExpectedVol,
                 fActualVol);
        }

    }

    // give channels new unique volumes
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        float fSetVol = 1.0f-((float)nCurChannel)/((float)nChannels);

        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(nCurChannel,fSetVol,&(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
        }
    }
    // verify new unique volumes
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        float fExpectedVol = 1.0f-((float)nCurChannel)/((float)nChannels);
        float fActualVol = -1.0f;
        
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel,&fActualVol));               

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");           
        }

        if(EPSILON < abs(fExpectedVol-fActualVol))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: set (%f) and retrieved (%f) volumes do not match!",
                 fExpectedVol,
                 fActualVol);
        }
        
    }

    // return  all channels to 1.0 volume
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {              
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(nCurChannel,1.0f,&(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
        }
    }
    // verify all volumes are 1.0
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        float fCurVol = 1.1f;
       
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel,&fCurVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");           
        }

        if(EPSILON < abs(1.0f-fCurVol))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Set (%f) and retrieved (%f) channel volumes do not match!",
                 0.0f,
                 fCurVol);
        }                    
    }

END:      
    return FNS_PASS;   
}
    
//------------------------------------------------------------------------------
// Desc: Verify SetAllVolumes behaves appropriately in basic situations
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetSetAllVolumes_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                  hr,
                  spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }

    SLOG(eInfo1, "Testing SetAllVolumes for a %d-channel stream",nChannels);
          
    if(1 == nChannels)
    {
        const float fSetVols[] = {1.0f, 0.0f, 1.0f};
        float fGetVol = 0.0f;
        
        // 1-channel basic tests
        for(int i=0;i<sizeof(fSetVols)/sizeof(float);i++)
        {
            SLOG(eInfo1, "Setting volume to %f", fSetVols[i]);
            METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                               hr,
                               spChannelVolume->SetAllVolumes(1,fSetVols+i, &(CHANNELVOLUME_TEST_CONTEXT)));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
            }

            // verify that the set call had the appropriate effect
            METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                               hr,
                               spChannelVolume->GetAllVolumes(1,&fGetVol));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
            }
            else
            {
                if(EPSILON < abs(fGetVol-fSetVols[i]))
                {
                    XLOG(XFAIL,
                         eError, 
                         "FAIL: Set (%f) and retrieved (%f) volumes do not match!",
                         fSetVols[i],
                         fGetVol);
                }
            }            
        }
    }
    else if(2 == nChannels)
    {
        // volumes to use for testing --
        const INT nVols = 3;
        float ppfVols[nVols][2];
        float pfGetVols[2];

        // case 0
        ppfVols[0][0] = 0.5f;           // left vol
        ppfVols[0][1] = 0.5f;           // right vol

        // case 1
        ppfVols[1][0] = 1.0f;           // left vol
        ppfVols[1][1] = 0.0f;           // right vol

        // case 2
        ppfVols[2][0] = 0.0f;           // left vol
        ppfVols[2][1] = 1.0f;           // right vol
        
        // 2-channel basic tests
        for(int nCase=0;nCase<nVols;nCase++)
        {
            SLOG(eInfo1, "Setting volume to [%f,%f]", ppfVols[nCase][0],ppfVols[nCase][1]);
            METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                               hr,
                               spChannelVolume->SetAllVolumes(2,(const float*)(ppfVols[nCase]), &(CHANNELVOLUME_TEST_CONTEXT)));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
            }

            // verify that the set call had the appropriate effect
            METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                               hr,
                               spChannelVolume->GetAllVolumes(2,pfGetVols));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
            }
            else
            {
                if(!( (EPSILON > abs(pfGetVols[0]-ppfVols[nCase][0])) && (EPSILON > abs(pfGetVols[1]-ppfVols[nCase][1]))))
                {
                    XLOG(XFAIL,
                         eError,
                         "FAIL: Set [%f,%f] and retrieved [%f,%f] values do not match!",
                         ppfVols[nCase][0],
                         ppfVols[nCase][1],
                         pfGetVols[0],
                         pfGetVols[1]);
                }
            }
        }
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify SetAllVolumes behaves appropriately
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetSetAllVolumes_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }
    
    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "ERROR: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }


    SLOG(eBlab1, "Running test for %d channels", nChannels);

    // the number of volume settings to test
    int nSettings = 10;
    float* pfSetVols = NULL;
    float* pfGetVols = NULL;
    
    // allocate space for the volumes
    pfSetVols = new float[nChannels];
    if(NULL == pfSetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    // allocate a buffer to store the GetVolume results
    pfGetVols = new float[nChannels];
    if(NULL == pfGetVols)
    {
        XLOG(XFAIL, eFatalError,"FAIL: Out of memory!");
        goto END;
    }

    // iterate through each setting, call SetAllVolumes and check the result
    for(int nCurSetting=0;nCurSetting<nSettings;nCurSetting++)
    {
        // fill the volume settings array with valid random attenuations
        for(UINT32 i=0;i<nChannels;i++)
        {
            pfSetVols[i] = (float)((g_IShell->Random())%100)/100.0f;
        }

        // set
        SLOG(eBlab1, "Calling SetAllVolumes with atten. values: ");
        for(UINT32 i=0;i<nChannels;i++)
        {
            SLOG(eBlab1, "  Channel %d: %f",i,pfSetVols[i]);
        }
        
        METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                          hr,
                          spChannelVolume->SetAllVolumes(nChannels,pfSetVols,&(CHANNELVOLUME_TEST_CONTEXT)));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
        }

        // check
        SLOG(eBlab1, "Calling GetAllVolumes...");

        METHODCHECKRESULT("IChannelAudioVolume::GetAllVolume",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Method under test returned unexpected HRESULT");
        }

        for(UINT32 i=0;i<nChannels;i++)
        {
            SLOG(eInfo1,"Channel %d / %d",i,nChannels);
        
            if((pfGetVols[i] - pfSetVols[i]) < EPSILON)
            {
                SLOG(eInfo1, "Volumes for channel %d match.", i);                
            }
            else
            {
                XLOG(XFAIL,
                     eError, 
                     "Set (%f) and retrieved (%f) volumes for channel %d do not match! (delta=%f)",
                     pfSetVols[i],
                     pfGetVols[i],
                     i,
                     pfGetVols[i]-pfSetVols[i]
                     );
            }
        }                        
    }

    // cleanup
END:
    SAFE_DELETE_ARRAY(pfSetVols);
    SAFE_DELETE_ARRAY(pfGetVols);
    return FNS_PASS;
}
    
//------------------------------------------------------------------------------
// Desc: Verify GetChannelCount behaves appropriately when passed a NULL pointer
//
// Returns:
//              FNS_PASS (Failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetChannelCount_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }
    
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(NULL));

    if(SUCCEEDED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount succeeded with NULL parameter!");
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify SetChannelVolume works correctly for attenuations < 0.0
//
// Returns:
//              FNS_PASS (Failure is signalled by XLOG)
//------------------------------------------------------------------------------    
DWORD Test_ChannelAudioVolume_SetChannelVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    const float fDefaultAtten = 0.5f;
    float fCurVolume = 0.0f;

    // get the number of channels for the current audiosession
    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
              hr,
              spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }

    SLOG(eInfo1, "---%d Channel Case---", nChannels);
    
    // set the volume(s) to a baseline value
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(nCurChannel, fDefaultAtten, &(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Interface under test returned unexpected HRESULT");
        }

        // verify the set was effective
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel, &fCurVolume));


        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Interface under test returned unexpected HRESULT");
        }
        else if(EPSILON < abs(fDefaultAtten-fCurVolume))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                 fDefaultAtten,
                 fCurVolume,
                 nCurChannel);
        }
    }

    // attempt to set each channel to an invalid volume, and ensure
    // all channels are unaffected by the invalid call
    // Repeat this for various negative values
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        const float pfInvalidValues[] = {-.0000000000001f, -.01f, -1.0f, -999.0f};

        for(int i=0;i<(sizeof(pfInvalidValues)/sizeof(float));i++)
        {
            //make the invalid attempt
            METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                              hr,
                              spChannelVolume->SetChannelVolume(nCurChannel,pfInvalidValues[i],
                                                                &(CHANNELVOLUME_TEST_CONTEXT)));
            if(!FAILED(hr))
            {
               XLOG(XFAIL,
                    eError, 
                    "FAIL: Call IChannelAudioVolume::SetChannelVolume(%f) should not have succeeded!",
                    pfInvalidValues[i]);
            }
       
            for(UINT32 nVerChannel=0;nVerChannel<nChannels;nVerChannel++)
            {
                // make sure that the attenuation was not actually changed for any channel
                METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                                  hr,
                                  spChannelVolume->GetChannelVolume(nVerChannel,&fCurVolume));


                if(EPSILON < abs(fDefaultAtten-fCurVolume))
                {
                    XLOG(XFAIL,
                         eError,
                         "FAIL: Volume for channel %d was %f, expected: 0.5",
                         nVerChannel,
                         fCurVolume);
                }
            }
        }
    }
        
END:
    return FNS_PASS;
    
}

//------------------------------------------------------------------------------
// Name: TC_AudioClient_AudioChannelVolume_SetChannelVolume_Invalid1
// Desc: Verify SetChannelVolume works correctly for attenuations > 1.0
//
// Returns:
//              FNS_PASS if the test passes
//              FNS_FAIL otherwise
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_SetChannelVolume_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                  hr,
                  spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }
      
    // set all channel volumes to 0.5 for baseline purposes
    for(UINT32 i=0;i<nChannels;i++)
    {            
        const FLOAT fSetVol = 0.5f;
        FLOAT fReportedVolume = 0.0f;

        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                         hr,
                         spChannelVolume->SetChannelVolume(i, fSetVol, &(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unexpected HRESULT returned by API under test");
        }

        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(i,&fReportedVolume));

        if(EPSILON < abs(fReportedVolume-fSetVol))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Set (%f) and retrieved (%f) values do not match for channel vol initializtion!",
                 fSetVol,
                 fReportedVolume);
        }
        
    }

    // set the channel volume slightly above one for each channel
    for(UINT32 i=0;i<nChannels;i++)
    {      
        float fSetVol = 1.0f+((float)(i+1))/10.0f;
         
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(i,fSetVol,&(CHANNELVOLUME_TEST_CONTEXT)));
        if(SUCCEEDED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: IChannelAudioVolume::SetChannelVolume should not have succeeded for volume = %f",
                 fSetVol);
        }
    }

    // make sure the volumes remain unchanged
    for(UINT32 i=0;i<nChannels;i++)
    {      
        float fSetVol = 0.5f;
        float fGetVol = 0.0f;
         
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(i,&fGetVol));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Unexpected HRESULT returned by API under test");    
        }
        else if (EPSILON < abs(fSetVol-fGetVol))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: Set (%f) and retrieved (%f) channel volumes for channel %d do not match",
                 fSetVol,
                 fGetVol,
                 i);
        }
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Name: TC_AudioClient_AudioChannelVolume_SetChannelVolume_Invalid2
// Desc: Verify SetChannelVolume responds correctly when invalid channels are
//       specified
//
// Returns:
//              FNS_PASS (Failures are signalled through XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_SetChannelVolume_Invalid2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    const float fDefaultAtten = 0.5f;
    const float fAltAtten = 1.0f;
    float fCurVolume = 0.0f;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }


    // set all channel volumes to a known baseline
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(nCurChannel, fDefaultAtten, &(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }

        // verify the set was effective
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel, &fCurVolume));


        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }
        else if(EPSILON < abs(fDefaultAtten-fCurVolume))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: Set (%f) and retrieved (%f) values for channel % do not match!",
                 fDefaultAtten,
                 fCurVolume,
                 nCurChannel);
        }
    }


    // Call set volume with the following channel indexes:
    // -> lower edge: channel = nChannels
    // -> arbitrarily large channel number
    // -> (UINT32)(-1)
    UINT32 pcChannelValues[] = {nChannels, 999, (UINT32)(-1)};

    for(UINT32 i=0;i<sizeof(pcChannelValues)/sizeof(UINT32);i++)
    {
        SLOG(eInfo1, "Attempting SetChannelVolume call for channel = %u", pcChannelValues[i]);

        //make the invalid attempt
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(pcChannelValues[i], fAltAtten, &(CHANNELVOLUME_TEST_CONTEXT)));

        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError,
                 "IChannelAudioVolume::SetChannelVolume(%u,%u) should not have succeeded!",
                 pcChannelValues[i],
                 fAltAtten);
        }
        else
        {
            SLOG(eInfo1, "Verifying that channel volumes have not changed...");

            for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
            {
                METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                                  hr,
                                  spChannelVolume->GetChannelVolume(nCurChannel, &fCurVolume));

                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: Unable to obtain volume for channel %u", nCurChannel);                    
                }
                else if(EPSILON < abs(fCurVolume-fDefaultAtten))
                {
                    XLOG(XFAIL,
                         eError, 
                         "ERROR: Volume for channel %u has been changed to %f (expected: %f)",
                         nCurChannel,
                         fDefaultAtten);
                }
            }            
        }   
        
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify GetChannelVolume responds correctly with a null output param
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetChannelVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    const float fDefAtten = 0.5f;
    float fCurVolume = 0.0f;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        goto END;
    }

    // set all channel volumes to a known baseline
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(nCurChannel, fDefAtten, &(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }

        // verify the set was effective
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel, &fCurVolume));


        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }
        else if(EPSILON < abs(fDefAtten-fCurVolume))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: Set (%f) and retrieved (%f) values for channel %u do not match!",
                 fDefAtten,
                 fCurVolume,
                 nCurChannel);
        }
    }

    // call GetVolume with an NULL param
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
#pragma prefast(suppress:6309, "Disable parameter checking")
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel,NULL));
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelVolume succeeded with NULL parameter!");
        }
        else
        {
            // just to be safe, verify that all channel volumes are unaffected by the call
            for(UINT32 nCurCheckChannel=0;nCurCheckChannel<nChannels;nCurCheckChannel++)
            {
                METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                                  hr,
                                  spChannelVolume->GetChannelVolume(nCurCheckChannel,&fCurVolume));

                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: GetChannelVolume for channel %d failed!", nCurCheckChannel);                 
                }
                else if(EPSILON < abs(fCurVolume-fDefAtten))
                {
                    XLOG(XFAIL,
                         eError, 
                         "FAIL: Set (%f) and retrieved (%f) values for channel (%u) no longer match",
                         fDefAtten,
                         fCurVolume,
                         nCurCheckChannel);
                }
            }           
        }     
    }

END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify GetChannelVolume responds correctly with an invalid channel param
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetChannelVolume_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    const float fDefaultAtten = 0.5f;
    float fCurVolume = 0.0f;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                  hr,
                  spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }
      
    // set all channel volumes to a known baseline
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(nCurChannel, fDefaultAtten, &(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }

        // verify the set was effective
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel, &fCurVolume));


        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
        }
        else if(EPSILON < abs(fDefaultAtten-fCurVolume))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAILSet (%f) and retrieved (%f) values for channel %d do not match!",
                 fDefaultAtten,
                 fCurVolume,
                 nCurChannel);
        }
    }

    // call GetChannelVolume with various bad channel indexes
    UINT32 pcChannelValues[] = {nChannels, 999, (UINT32)(-1)};

    for(int i=0;i<sizeof(pcChannelValues)/sizeof(UINT32);i++)
    {
        // make the get call
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(pcChannelValues[i],&fCurVolume));

        if(SUCCEEDED(hr))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: IChannelAudioVolume::GetChannelVolume succeeded with invalid channel param (%u)",
                 pcChannelValues[i]);
        }
        else
        {
            // double-check to make sure that none of the channel volumes have been altered
            for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
            {
                METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                                  hr,
                                  spChannelVolume->GetChannelVolume(nCurChannel,&fCurVolume));

                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "ERROR: API under test returned unexpected HRESULT");;
                }
                else if(EPSILON < abs(fCurVolume-fDefaultAtten))
                {
                    XLOG(XFAIL,
                         eError, 
                         "FAIL: Set (%f) and retrieved (%f) volumes for channel %u do not match!",
                         fDefaultAtten,
                         fCurVolume,
                         nCurChannel);
                }               
            }           
        }
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify GetChannelVolume responds correctly with a null output param
//       and bad channel count
//
// Returns:
//              FNS_PASS (Failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetChannelVolume_Invalid2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    const float fDefaultAtten = 0.5f;
    float fCurVolume = 0.0f;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }
    
    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));

    if(FAILED(hr)||(0==nChannels))
    {
        XLOG(XFAIL, eError, "FAIL: IChannelAudioVolume::GetChannelCount failed");
        goto END;
    }

    // set all channel volumes to a known baseline
    for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
    {
        METHODCHECKRESULT("IChannelAudioVolume::SetChannelVolume",
                          hr,
                          spChannelVolume->SetChannelVolume(nCurChannel, fDefaultAtten, &(CHANNELVOLUME_TEST_CONTEXT)));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }

        // verify the set was effective
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(nCurChannel, &fCurVolume));


        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }
        else if(EPSILON < abs(fDefaultAtten-fCurVolume))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: Set (%f) and retrieved (%f) values for channel % do not match!",
                 fDefaultAtten,
                 fCurVolume,
                 nCurChannel);
        }
    }

    // try a series of invalid channel IDs
    UINT32 pcChannelValues[] = {nChannels, 999, (UINT32)(-1)};

    for(int i=0;i<sizeof(pcChannelValues)/sizeof(UINT32);i++)
    {
        // make the get call
        // use a NULL param as well, for good measure
#pragma prefast(suppress:6309, "Disable parameter checking")
        METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                          hr,
                          spChannelVolume->GetChannelVolume(pcChannelValues[i],NULL));

        if(!FAILED(hr))
        {
            XLOG(XFAIL,
                 eError, 
                 "FAIL: IChannelAudioVolume::GetChannelVolume succeeded with invalid channel params (%u) and NULL",
                 pcChannelValues[i]);
        }
        else
        {
            // double-check to make sure that none of the channel volumes have been altered
            for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
            {
                METHODCHECKRESULT("IChannelAudioVolume::GetChannelVolume",
                                  hr,
                                  spChannelVolume->GetChannelVolume(nCurChannel,&fCurVolume));

                if(FAILED(hr))
                {
                    XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
                }
                else if(EPSILON < abs(fCurVolume-fDefaultAtten))
                {
                    XLOG(XFAIL,
                         eError, 
                         "FAIL: Set (%f) and retrieved (%f) volumes for channel %u do not match!",
                         fDefaultAtten,
                         fCurVolume,
                         nCurChannel);
                }               
            }           
        }
    }

END:
    return FNS_PASS;
}


//------------------------------------------------------------------------------
// Desc: Verify SetAllVolumes behaves appropriately when passed a NULL pointer
//
// Returns:
//              FNS_PASS if the test passes
//              FNS_FAIL otherwise
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_SetAllVolumes_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    float* pfSetVols = NULL;
    float* pfGetVols = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");        
        goto END;
    }

    // set the volumes for each channel to a known baseline
    pfSetVols = new float[nChannels];
    if(NULL == pfSetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    pfGetVols = new float[nChannels];
    if(NULL == pfGetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    
    for(UINT32 i=0;i<nChannels;i++)
    {
        pfSetVols[i] = 1.0f;
    }

    METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                      hr,
                      spChannelVolume->SetAllVolumes(nChannels,pfSetVols,&(CHANNELVOLUME_TEST_CONTEXT)));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");   
    }

    // verify the set was successful
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
    }

    for(UINT32 i=0;i<nChannels;i++)
    {
        if(EPSILON < abs(pfSetVols[i]-pfGetVols[i]))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                 pfSetVols[i],
                 pfGetVols[i],
                 i);
        }
    }

    // Call SetAllVolumes with a null pointer
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                      hr,
                      spChannelVolume->SetAllVolumes(nChannels,NULL,&(CHANNELVOLUME_TEST_CONTEXT)));

    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
    }
    
    // Verify the volumes remained unchanged

    SLOG(eInfo1, "Verifying that all channel volumes remained unchanged");
    
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");    
    }

    for(UINT32 i=0;i<nChannels;i++)
    {
        if(EPSILON < abs(pfSetVols[i]-pfGetVols[i]))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                 pfSetVols[i],
                 pfGetVols[i],
                 i);
        }
    }

END:
    SAFE_DELETE_ARRAY(pfGetVols);
    SAFE_DELETE_ARRAY(pfSetVols);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify SetAllVolumes behaves appropriately when passed an invalid
//       number of channels, and a buffer that corresponds to that number
//       of channels
//
// Returns:
//              FNS_PASS if the test passes
//              FNS_FAIL otherwise
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_SetAllVolumes_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    float* pfSetVols = NULL;
    float* pfGetVols = NULL;
    float* pfInvalidBuffer = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");        
        goto END;
    }

    // number of channels to use in call to SetAllVolumes
    const INT nChannelCounts = 5;
    INT pcChannelCounts[nChannelCounts];
    pcChannelCounts[0] = 0;
    pcChannelCounts[1] = nChannels+1;
    pcChannelCounts[2] = nChannels+5;
    pcChannelCounts[3] = 100;
    pcChannelCounts[4] = 99999;

    // allocate buffers for setting/retrieving volumes
    pfSetVols = new float[nChannels];
    if(NULL == pfSetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    pfGetVols = new float[nChannels];
    if(NULL == pfGetVols)        
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    

    for(int nSetIndex=0;nSetIndex<nChannelCounts;nSetIndex++)
    {
        // set the volumes for each channel to a known baseline
        for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
        {
            pfSetVols[nCurChannel] = 1.0f;
        }

        METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                          hr,
                          spChannelVolume->SetAllVolumes(nChannels,pfSetVols,&(CHANNELVOLUME_TEST_CONTEXT)));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }

        // verify the set was successful
        METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
            goto END;
        }

        for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
        {
            if(EPSILON < abs(pfSetVols[nCurChannel]-pfGetVols[nCurChannel]))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                     pfSetVols[nCurChannel],
                     pfGetVols[nCurChannel],
                     nCurChannel);
            }
        } 

        // now that we've established a baseline, attempt the invalid set
        // allocate the buffer
        SAFE_DELETE_ARRAY(pfInvalidBuffer);
        pfInvalidBuffer = new float[pcChannelCounts[nSetIndex]];
        if(NULL == pfInvalidBuffer)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
            goto END;
        }

        // set all entries in the buffer to a value different than any current
        // channel volume, so we can tell if it has any effect
        for(int nCurChannel=0;nCurChannel<pcChannelCounts[nSetIndex];nCurChannel++)
        {
            pfInvalidBuffer[nCurChannel] = 0.0f;
        }
        
        METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                          hr,
                          spChannelVolume->SetAllVolumes(pcChannelCounts[nSetIndex],pfInvalidBuffer,&(CHANNELVOLUME_TEST_CONTEXT)));
        
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eError,"FAIL: Expected SetAllVolumes to fail for num channels = %d", pcChannelCounts[nSetIndex]);
        }
        else
        {
            SLOG(eBlab1,"SetAllVolumes failed for num channels = %d", pcChannelCounts[nSetIndex]);
        }

        // verify that the invalid call has no effect on the volumes
        METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
            goto END;
        }

        for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
        {
            if(EPSILON < abs(pfSetVols[nCurChannel]-pfGetVols[nCurChannel]))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                     pfSetVols[nCurChannel],
                     pfGetVols[nCurChannel],
                     nCurChannel);
            }
        } 
    
    }

END:
    SAFE_DELETE_ARRAY(pfSetVols);
    SAFE_DELETE_ARRAY(pfGetVols);
    return FNS_PASS;
}

DWORD Test_ChannelAudioVolume_SetAllVolumes_Invalid2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    float* pfSetVols = NULL;
    float* pfGetVols = NULL;
    float* pfInvalidVols = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");        
        goto END;
    }

    pfSetVols = new float[nChannels];
    if(NULL == pfSetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    pfGetVols = new float[nChannels];
    if(NULL == pfGetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    pfInvalidVols = new float[nChannels];
    if(NULL == pfInvalidVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    // values to use as the default volume
    for(UINT32 i=0;i<nChannels;i++)
    {
        pfSetVols[i] = 1.0f;
    }


    // perform three variations of the test --
    // 0) Set the value of the first channel to be invalid
    // 1) Set the value of the last channel to be invalid
    // 2) Set the value of all channels to be invalid
    for(int nCase=0;nCase<3;nCase++)
    {

        // set the volumes to a baseline so we can detect changes
        METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                          hr,
                          spChannelVolume->SetAllVolumes(nChannels,pfSetVols,&(CHANNELVOLUME_TEST_CONTEXT)));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }

        // verify the set was successful
        METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }

        for(UINT32 i=0;i<nChannels;i++)
        {
            if(EPSILON < abs(pfSetVols[i]-pfGetVols[i]))
            {
                XLOG(XFAIL,
                     eError,
                     "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                     pfSetVols[i],
                     pfGetVols[i],
                     i);
            }
        }

        // now that we've established a baseline, attempt the invalid calls
        for(UINT32 i=0;i<nChannels;i++)
        {
            if(0 == nCase)
            {            
                // set the first channel to an invalid value only
                if(0 == i)
                {
                    pfInvalidVols[i] = -1.0f;
                }
                else
                {
                    pfInvalidVols[i] = 0.5f;
                }
            }
            else if(1 == nCase)
            {
                // set the last channel to an invalid value only
                if(i==(nChannels-1))
                {
                    pfInvalidVols[i] = -1.0f;                    
                }
                else
                {
                    pfInvalidVols[i] = 0.5f;
                }
            }
            else if(2 == nCase)
            {
                // set all channels to an invalid value
                pfInvalidVols[i] = -1.0f;
            }
            else
            {
                XLOG(XFAIL, eFatalError, "FAIL: Invalid case encountered!");
                goto END;
            }
        }

        SLOG(eInfo1, "Setting channel volumes to: ");
        for(UINT32 i=0;i<nChannels;i++)
        {
            SLOG(eInfo1,"  %f (Channel %d)",pfInvalidVols[i],i);
        }

        METHODCHECKRESULT("IChannelVolume::SetAllVolumes",
                          hr,
                          spChannelVolume->SetAllVolumes(nChannels,pfInvalidVols,&(CHANNELVOLUME_TEST_CONTEXT)));

        if(!FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: SetAllVolumes should have failed for values: ");
            for(UINT32 i=0;i<nChannels;i++)
            {
                SLOG(eError, "%f (Channel %d)",pfInvalidVols,i);
            }
        }

        // verify that nothing changed as a result of the failure
        METHODCHECKRESULT("IChannelVolume::GetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL Unable to retrieve volumes!");
        }
        else
        {
            for(UINT32 i=0;i<nChannels;i++)
            {
                if(! (pfSetVols[i] == pfGetVols[i]))
                {
                    XLOG(XFAIL,
                         eError, 
                         "FAIL: Volume for channel %d has changed from %f to %f",
                         i,
                         pfSetVols[i],
                         pfGetVols[i]);
                }           
            }
        }
    } // end for(int nCase...

END:
    SAFE_DELETE_ARRAY(pfSetVols);
    SAFE_DELETE_ARRAY(pfGetVols);
    return FNS_PASS;
    
}

//------------------------------------------------------------------------------
// Desc: Verify SetAllVolumes behaves appropriately when passed a multiple
//       invalid parameters
//
// Returns:
//              FNS_PASS (Failure signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_SetAllVolumes_Invalid3(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    float* pfSetVols = NULL;
    float* pfGetVols = NULL;
    float* pfBadVols = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        goto END;
    }

    if (0 == nChannels)
    {
        XLOG(XFAIL, 
             eError, 
             "FAIL: IChannelAudioVolume::GetChannelCount returned a channel count of 0!");
        goto END;
    }

    INT nBadVols = nChannels-1;

    pfSetVols = new float[nChannels];
    if(NULL == pfSetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    
    pfGetVols = new float[nChannels];
    if(NULL == pfGetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    
    pfBadVols = new float[nBadVols];
    if(NULL == pfBadVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    // set the volumes to a baseline so we can detect changes
    for(UINT32 i=0;i<nChannels;i++)
    {
        pfSetVols[i] = 0.5f;
        pfGetVols[i] = 0.0f;       
    }
    

    METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                      hr,
                      spChannelVolume->SetAllVolumes(nChannels,pfSetVols,&(CHANNELVOLUME_TEST_CONTEXT)));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
    }

    // verify the set was successful
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        goto END;
    }

    for(UINT32 i=0;i<nChannels;i++)
    {
        if(EPSILON < abs(pfSetVols[i]-pfGetVols[i]))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                 pfSetVols[i],
                 pfGetVols[i],
                 i);
        }
    }

    // initialize pfBadVols to contain bad attenuation values
    for(int i=0;i<nBadVols;i++)
    {
        pfBadVols[i] = -1.0f;
    }

    // make the invalid call
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(nBadVols,pfBadVols));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetAllVolumes succeeded with nChannels = %d and channel volumes: ",nBadVols);
        for(int i=0;i<nBadVols;i++)
        {
            XLOG(XFAIL, eError,"  %f",pfBadVols[i]);
        }       
    }
    else
    {
        // verify none of the volumes changed
        METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
        }
        else
        {
            for(UINT32 i=0;i<nChannels;i++)
            {
                if(abs(pfSetVols[i]-pfGetVols[i]))
                {
                    XLOG(XFAIL,
                         eError,
                         "FAIL: Volume for channel %d changed from %f to %f",
                         i,
                         pfSetVols[i],
                         pfGetVols[i]);                    
                }
            }
        }
    }
      
END:
    SAFE_DELETE_ARRAY(pfSetVols);
    SAFE_DELETE_ARRAY(pfGetVols);
    SAFE_DELETE_ARRAY(pfBadVols);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify GetAllVolumes behaves appropriately when passed a NULL buffer
//       pointer.
//
// Returns:
//              FNS_PASS (Failure is signalled via XLOG
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetAllVolumes_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    UINT32 nChannels = 0;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    float* pfSetVols = NULL;
    float* pfGetVols = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
        goto END;
    }
        
    pfSetVols = new float[nChannels];
    if(NULL == pfSetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    pfGetVols = new float[nChannels];
    if(NULL == pfGetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    // set the volumes for all channels to a known baseline
    for(UINT32 i=0;i<nChannels;i++)
    {
        pfSetVols[i] = 1.0f;
    }

    METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                      hr,
                      spChannelVolume->SetAllVolumes(nChannels,pfSetVols,&(CHANNELVOLUME_TEST_CONTEXT)));

    if(FAILED(hr))
    {
         XLOG(XFAIL, eError, "FAIL: API Under test returned unexpected HRESULT");
    }

    // verify the set was successful
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
        goto END;
    }

    for(UINT32 i=0;i<nChannels;i++)
    {
        if(EPSILON < abs(pfSetVols[i]-pfGetVols[i]))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                 pfSetVols[i],
                 pfGetVols[i],
                 i);
        }
    }

    // Call GetAllVolumes with a null pointer
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->SetAllVolumes(nChannels,NULL,&(CHANNELVOLUME_TEST_CONTEXT)));

    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Method should not have succeeded!");
    }
    
    // Verify the volumes remained unchanged

    SLOG(eInfo1, "Verifying that all channel volumes remained unchanged");
    
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
        goto END;
    }

    for(UINT32 i=0;i<nChannels;i++)
    {
        if(EPSILON < abs(pfSetVols[i]-pfGetVols[i]))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                 pfSetVols[i],
                 pfGetVols[i],
                 i);
        }
    }

     
END:
    SAFE_DELETE_ARRAY(pfSetVols);
    SAFE_DELETE_ARRAY(pfGetVols);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify GetAllVolumes behaves correctly when passed an invalid channel
//       count and corresponding buffer.
//
// Returns:
//              FNS_PASS (Failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetAllVolumes_Invalid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    float* pfGetVols = NULL;
    float* pfSetVols = NULL;
    float* pfInvalidBuffer = NULL;
    UINT32 nChannels = 0;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
    }
    

    // number of channels to use in call to GetAllVolumes
    const INT nChannelCounts = 5;
    INT pcChannelCounts[nChannelCounts];
    pcChannelCounts[0] = 0;
    pcChannelCounts[1] = nChannels+1;
    pcChannelCounts[2] = nChannels+5;
    pcChannelCounts[3] = 100;
    pcChannelCounts[4] = 99999;

    // allocate buffers for setting/retrieving volumes
    pfSetVols = new float[nChannels];
    if(NULL == pfSetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    pfGetVols = new float[nChannels];
    if(NULL == pfGetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    
    // perform the test for each number of invalid channels
    for(int nSetIndex=0;nSetIndex<nChannelCounts;nSetIndex++)
    {
        // set the volumes for each channel to a known baseline
        for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
        {
            pfSetVols[nCurChannel] = 1.0f;
        }

        METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                          hr,
                          spChannelVolume->SetAllVolumes(nChannels,pfSetVols,&(CHANNELVOLUME_TEST_CONTEXT)));

        if(FAILED(hr))
        {
             XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
        }

        // verify the set was successful
        METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
            goto END;
        }

        for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
        {
            if(EPSILON < abs(pfSetVols[nCurChannel]-pfGetVols[nCurChannel]))
            {
                XLOG(XFAIL,
                     eError,
                     "Set (%f) and retrieved (%f) values for channel %d do not match!",
                     pfSetVols[nCurChannel],
                     pfGetVols[nCurChannel],
                     nCurChannel);
            }
        } 

        // now that we've established a baseline, attempt the invalid get
        // allocate the buffer
        SAFE_DELETE_ARRAY(pfInvalidBuffer);
        pfInvalidBuffer = new float[pcChannelCounts[nSetIndex]];
        if(NULL == pfInvalidBuffer)
        {
            XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
            goto END;
        }

        // set all entries in the buffer to a value different than any current
        // channel volume, so we can tell if it has any effect
        for(int nCurChannel=0;nCurChannel<pcChannelCounts[nSetIndex];nCurChannel++)
        {
            pfInvalidBuffer[nCurChannel] = 0.0f;
        }
        
        METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(pcChannelCounts[nSetIndex],pfInvalidBuffer));
        
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: Expected SetAllVolumes to fail for num channels = %d", pcChannelCounts[nSetIndex]);
        }
        else
        {
            SLOG(eBlab1,"SetAllVolumes failed for num channels = %d", pcChannelCounts[nSetIndex]);
        }

        // verify that the invalid call has no effect on the volumes
        METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
            goto END;
        }

        for(UINT32 nCurChannel=0;nCurChannel<nChannels;nCurChannel++)
        {
            if(EPSILON < abs(pfSetVols[nCurChannel]-pfGetVols[nCurChannel]))
            {
                XLOG(XFAIL,
                     eError,
                     "Set (%f) and retrieved (%f) values for channel %d do not match!",
                     pfSetVols[nCurChannel],
                     pfGetVols[nCurChannel],
                     nCurChannel);
            }
        }        
    }

END:
    SAFE_DELETE_ARRAY(pfInvalidBuffer);
    SAFE_DELETE_ARRAY(pfSetVols);
    SAFE_DELETE_ARRAY(pfGetVols);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify that GetAllVolumes behaves appropriately when passed multiple
//       invalid parameters.
//
// Returns:
//              FNS_PASS if the test passes
//              FNS_FAIL otherwise
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_GetAllVolumes_Invalid2(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;
    float* pfSetVols = NULL;
    float* pfGetVols = NULL;    
    INT nBadVols = 0;    
    UINT32 nChannels = 0;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }

    METHODCHECKRESULT("IChannelAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&nChannels));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
    }

    nBadVols = nChannels-1;

    pfSetVols = new float[nChannels];
    if(NULL == pfSetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    pfGetVols = new float[nChannels];
    if(NULL == pfGetVols)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    // set the volumes to a baseline so we can detect changes
    for(UINT32 i=0;i<nChannels;i++)
    {
        pfSetVols[i] = 0.5f;
        pfGetVols[i] = 0.0f;       
    }
    

    METHODCHECKRESULT("IChannelAudioVolume::SetAllVolumes",
                      hr,
                      spChannelVolume->SetAllVolumes(nChannels,pfSetVols,&(CHANNELVOLUME_TEST_CONTEXT)));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
    }

    // verify the set was successful
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(nChannels,pfGetVols));

    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
        goto END;
    }

    for(UINT32 i=0;i<nChannels;i++)
    {
        if(EPSILON < abs(pfSetVols[i]-pfGetVols[i]))
        {
            XLOG(XFAIL,
                 eError,
                 "FAIL: Set (%f) and retrieved (%f) values for channel %d do not match!",
                 pfSetVols[i],
                 pfGetVols[i],
                 i);
        }
    }
   
    // make the invalid call
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(nBadVols,NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: GetAllVolumes succeeded with nChannels = %d and a NULL buffer!",nBadVols);
    }
    else
    {
        // verify none of the volumes changed
        METHODCHECKRESULT("IChannelAudioVolume::GetAllVolumes",
                          hr,
                          spChannelVolume->GetAllVolumes(nChannels,pfGetVols));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT!");
        }
        else
        {
            for(UINT32 i=0;i<nChannels;i++)
            {
                if(EPSILON < abs(pfSetVols[i]-pfGetVols[i]))
                {
                    XLOG(XFAIL,
                         eError,
                         "FAIL: Volume for channel %d changed from %f to %f",
                         i,
                         pfSetVols[i],
                         pfGetVols[i]);
                }
            }
        }
    }
    
    
END:
    SAFE_DELETE_ARRAY(pfSetVols);
    SAFE_DELETE_ARRAY(pfGetVols);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Verify that IChannelAudioVolume methods behave correctly under RPC failure conditions
//
// Returns:
//              FNS_PASS if the test passes
//              FNS_FAIL otherwise
//------------------------------------------------------------------------------
DWORD Test_ChannelAudioVolume_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IChannelAudioVolume> spChannelVolume = NULL;

    UINT32 dwChannelCount = 0;
    FLOAT fVol1;
    FLOAT pfVolArray[2] = {0.0f, 0.0f};

    
    assert(NULL != pStreamingEngine);
    hr = pStreamingEngine->GetChannelAudioVolume(&spChannelVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain ISimpleAudioVolume for test!");
        goto END;
    }
    
    // get a handle to audiosrv
    // shut down audiosrv
    if (!StopAudioService())
    {
        XLOG(XFAIL, eError, "FAIL: Unable to stop AudioSrv!");
        goto END;
    }

    // BEGIN IChannelAudioVolume API Calls
    ////

    // GetChannelCount
    METHODCHECKRESULT("ISimpleAudioVolume::GetChannelCount",
                      hr,
                      spChannelVolume->GetChannelCount(&dwChannelCount));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }

    // SetChannelVolume
    METHODCHECKRESULT("ISimpleAudioVolume::SetChannelVolume",
                      hr,
                      spChannelVolume->SetChannelVolume(1,1.0f,&(CHANNELVOLUME_TEST_CONTEXT)));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }

    // GetChannelVolume
    METHODCHECKRESULT("ISimpleAudioVolume:GetChannelVolume",
                      hr,
                      spChannelVolume->GetChannelVolume(1,&fVol1));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }

    // SetAllVolumes
    METHODCHECKRESULT("ISimpleAudioVolume:SetAllVolumes",
                      hr,
                      spChannelVolume->SetAllVolumes(2,pfVolArray,&(CHANNELVOLUME_TEST_CONTEXT)));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }

    // GetAllVolumes
    METHODCHECKRESULT("ISimpleAudioVolume:GetAllVolumes",
                      hr,
                      spChannelVolume->GetAllVolumes(2,pfVolArray));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }

    ////
    // END IChannelAudioVolume API Calls


END:
   // Start audiosrv back up. Moving this into end section so that even if this function fails, the following test cases do not fail.
    if (!StartAudioService())
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to re-start AudioSrv! System may be in an unusable state!");
    }

    return FNS_PASS;
}

