// Copyright(C) Microsoft. All rights reserved.

//
// AudioStreamVolumeAPITests.cpp
//  
//
// Description:
//
//  WASAPI IAudioStreamVolume API Tests

#include <stdafx.h>
#include <UMATestStreaming.h>


#include "StreamingControlTestFramework.h"

// GUID used for AudioStreamVolume API Test Contexts
// {72083CF3-1184-4e2b-8E5F-1036C7AAD57C}
static const GUID AUDIOSTREAMVOLUME_TEST_CONTEXT = 
{ 0x72083cf3, 0x1184, 0x4e2b, { 0x8e, 0x5f, 0x10, 0x36, 0xc7, 0xaa, 0xd5, 0x7c } };

DWORD TC_StreamAudioVolume_GetChannelCount_Valid0()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_GETCHANNELCOUNT_VALID0);
    return FNS_PASS;
}
DWORD TC_StreamAudioVolume_GetSetChannelVolume_Valid0()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_GETSETCHANNELVOLUME_VALID0);
    return FNS_PASS;
}

DWORD TC_StreamAudioVolume_GetSetAllVolumes_Valid0()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_GETSETALLVOLUMES_VALID0);
    return FNS_PASS;
}

DWORD TC_StreamAudioVolume_GetSetAllVolumes_Valid1()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_GETSETALLVOLUMES_VALID1);
    return FNS_PASS;
}

DWORD TC_StreamAudioVolume_GetChannelCount_Invalid0()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_GETCHANNELCOUNT_INVALID0);
    return FNS_PASS;
}

DWORD TC_StreamAudioVolume_SetChannelVolume_Invalid0()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_SETCHANNELVOLUME_INVALID0);
    return FNS_PASS;
}

DWORD TC_StreamAudioVolume_GetChannelVolume_Invalid0()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_GETCHANNELVOLUME_INVALID0);
    return FNS_PASS;
}

DWORD TC_StreamAudioVolume_GetAllVolumes_Invalid0()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_GETALLVOLUMES_INVALID0);
    return FNS_PASS;
}

DWORD TC_StreamAudioVolume_SetAllVolumes_Invalid0()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_SETALLVOLUMES_INVALID0);
    return FNS_PASS;
}

DWORD TC_StreamAudioVolume_DisconnectService()
{
    RunStreamingControlTest(STREAMAUDIOVOLUME_DISCONNECTSERVICE);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Basic GetChannelVolume/SetChannelVolume verification tests
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_GetChannelCount_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;
    
    FLOAT fTestValues[] = {0.0, 0.5, 1.0};
    UINT32 nTestValues = sizeof(fTestValues)/sizeof(FLOAT);
    
    UINT32 nChannelCount = 0;
    FLOAT fChanVolume = 0.0f;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count for later use
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&nChannelCount));
    if(FAILED(hr)||(0 == nChannelCount))
    {
        XLOG(XFAIL, eFatalError, "FAIL: GetChannelCount failed!");
        goto END;
    }

    for(UINT32 i=0;i<nTestValues;i++)
    {
        for(UINT32 j=0;j<nChannelCount;j++)
        {
            // set the volume
            METHODCHECKRESULT("IAudioStreamVolume::SetChannelVolume",
                              hr,
                              spStreamVolume->SetChannelVolume(j, fTestValues[i]));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }

            // verify the set took effect
            fChanVolume = 0.0f;
            METHODCHECKRESULT("IAudioStreamVolume::GetChannelVolume",
                              hr,
                              spStreamVolume->GetChannelVolume(j,&fChanVolume));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }
            else if(fChanVolume != fTestValues[i])
            {
                XLOG(XFAIL,
                     eFatalError, 
                     "FAIL: Set (%f) and retrieved (%f) volumes for channel %d do not match!",
                     fTestValues[i],
                     fChanVolume,
                     j);
            }
        }   
    }
   
END:        
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: GetChannelVolume/SetChannelVolume functional tests
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_GetSetChannelVolume_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    UINT32 nChannelCount = 0;
    FLOAT fSetVolume = 0.0f;
    FLOAT fRetrievedVolume = 0.0f;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count for later use
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&nChannelCount));
    if(FAILED(hr)||(0 == nChannelCount))
    {
        XLOG(XFAIL, eFatalError, "FAIL: GetChannelCount failed!");
        goto END;
    }

    // fade up
    for(FLOAT fSetVolIndex=0.0f;fSetVolIndex<=1.0f;fSetVolIndex+=0.2f)
    {
        for(UINT32 j=0;j<nChannelCount;j++)
        {
            // calculate the volume to set
            if((j%2)==1)
            {
                fSetVolume = 1.0f-fSetVolIndex;
            }
            else
            {
                fSetVolume = fSetVolIndex;
            }


            // set the volume
            METHODCHECKRESULT("IAudioStreamVolume::SetChannelVolume",
                              hr,
                              spStreamVolume->SetChannelVolume(j, fSetVolume));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
                continue;
            }

            // verify the set took effect
            fRetrievedVolume = 0.0f;
            METHODCHECKRESULT("IAudioStreamVolume::GetChannelVolume",
                              hr,
                              spStreamVolume->GetChannelVolume(j,&fRetrievedVolume));
            if(FAILED(hr))
            {
                XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            }
            else if(abs(fRetrievedVolume-fSetVolume)>EPSILON)
            {
                XLOG(XFAIL,
                     eError,
                     "Error: Set (%f) and retrieved (%f) volumes for channel %d do not match!",
                     fSetVolume,
                     fRetrievedVolume,
                     j);
            }
        }   
    }
    
END:    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: GetAllVolumes/SetAllVolumes basic tests
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_GetSetAllVolumes_Valid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    FLOAT fTestValues[] = {0.0, 0.5, 1.0};
    INT nTestValues = sizeof(fTestValues)/sizeof(FLOAT);

    UINT32 nChannelCount = 0;
    FLOAT* pfSetBuffer = NULL;
    FLOAT* pfGetBuffer = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count for later use
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&nChannelCount));
    if(FAILED(hr)||(0 == nChannelCount))
    {
        XLOG(XFAIL, eFatalError, "FAIL: GetChannelCount failed!");
        goto END;
    }

    // allocate memory buffers
    pfSetBuffer = new FLOAT[nChannelCount];
    pfGetBuffer = new FLOAT[nChannelCount];

    if((NULL==pfSetBuffer)||(NULL==pfGetBuffer))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    // zero buffers
    for(UINT32 i=0;i<nChannelCount;i++)
    {
        pfSetBuffer[i] = 0.0f;
        pfGetBuffer[i] = 0.0f;
    }

    for(int i=0;i<nTestValues;i++)
    {
        // populate volume buffer
        for(UINT32 nCurChan=0;nCurChan<nChannelCount;nCurChan++)
        {
            pfSetBuffer[nCurChan] = fTestValues[i];    
        }

        // set the volumes             
        METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                          hr,
                          spStreamVolume->SetAllVolumes(nChannelCount, pfSetBuffer));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }      

        // get the volumes             
        METHODCHECKRESULT("IAudioStreamVolume::GetAllVolumes",
                          hr,
                          spStreamVolume->GetAllVolumes(nChannelCount, pfGetBuffer));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }      
        
        
        // verify the results
        for(UINT32 nCurChan=0;nCurChan<nChannelCount;nCurChan++)
        {
            if(EPSILON<(pfSetBuffer[nCurChan]-pfGetBuffer[nCurChan]))
            {
                XLOG(XFAIL,
                     eFatalError,
                     "FAIL: Set (%f) and reported (%f) volumes for channel %d do not match",
                     pfSetBuffer[nCurChan],
                     pfGetBuffer[nCurChan],
                     nCurChan);                     
            }
        }     
    }
   
END:        
    SAFE_DELETE_ARRAY(pfGetBuffer);
    SAFE_DELETE_ARRAY(pfSetBuffer);
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: GetAllVolumes/SetAllVolumes functional tests
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_GetSetAllVolumes_Valid1(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    UINT32 nChannelCount = 0;
    FLOAT* pfSetBuffer = NULL;
    FLOAT* pfGetBuffer = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count for later use
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&nChannelCount));
    if(FAILED(hr)||(0 == nChannelCount))
    {
        XLOG(XFAIL, eFatalError, "FAIL: GetChannelCount failed!");
        goto END;
    }

    // allocate memory buffers
    pfSetBuffer = new FLOAT[nChannelCount];
    pfGetBuffer = new FLOAT[nChannelCount];

    if((NULL==pfSetBuffer)||(NULL==pfGetBuffer))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    // zero buffers
    for(UINT32 i=0;i<nChannelCount;i++)
    {
        pfSetBuffer[i] = 0.0f;
        pfGetBuffer[i] = 0.0f;
    }

    for(FLOAT fSetIndex=0.0f;fSetIndex<=1.0f;fSetIndex+=0.1f)
    {
        // populate volume set buffer
        for(UINT32 nCurChan=0;nCurChan<nChannelCount;nCurChan++)
        {
            if((nCurChan%2)==1)
            {
                pfSetBuffer[nCurChan] = 1.0f-fSetIndex;    
            }
            else
            {
                pfSetBuffer[nCurChan] = fSetIndex;
            }
        }

        // set the volumes             
        METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                          hr,
                          spStreamVolume->SetAllVolumes(nChannelCount, pfSetBuffer));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }      

        // get the volumes             
        METHODCHECKRESULT("IAudioStreamVolume::GetAllVolumes",
                          hr,
                          spStreamVolume->GetAllVolumes(nChannelCount, pfGetBuffer));
        if(FAILED(hr))
        {
            XLOG(XFAIL, eError, "FAIL: API under test returned unexpected HRESULT");
            continue;
        }      
        
        
        // verify the results
        for(UINT32 nCurChan=0;nCurChan<nChannelCount;nCurChan++)
        {
            if(EPSILON<(pfSetBuffer[nCurChan]-pfGetBuffer[nCurChan]))
            {
                XLOG(XFAIL,
                     eFatalError,
                     "FAIL: Set (%f) and reported (%f) volumes for channel %d do not match",
                     pfSetBuffer[nCurChan],
                     pfGetBuffer[nCurChan],
                     nCurChan);                     
            }
        }     
    }
   
END:        
    SAFE_DELETE_ARRAY(pfGetBuffer);
    SAFE_DELETE_ARRAY(pfSetBuffer);
    return FNS_PASS;

}

//------------------------------------------------------------------------------
// Desc: Basic GetChannelCount functionality
// Returns:
//              FNS_PASS if the test passes (failures are signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_GetChannelCount_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: GetChannelCount succeeded with NULL parameter");
    }
    
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Various invalid SetChannelVolume scenarios
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_SetChannelVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    UINT32 nChannelCount = 0;

    FLOAT afVolumes[] = {-10.0f, -5.0f, 5.0f, 10.0f};
    int nInvalidVolumes = sizeof(afVolumes)/sizeof(float);

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&nChannelCount));
    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to obtain channel count, aborting!");
        goto END;
    }

    // invalid channel index tests
    METHODCHECKRESULT("IAudioStreamVolume::SetChannelVolume",
                      hr,
                      spStreamVolume->SetChannelVolume(nChannelCount, 1.0f));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Set for channel index = %d should not have succeeded!", nChannelCount);
    }
    
    METHODCHECKRESULT("IAudioStreamVolume::SetChannelVolume",
                      hr,
                      spStreamVolume->SetChannelVolume(999, 1.0f));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Set for channel index = %d should not have succeeded!", 999);
    }


    // invalid level tests
    for(int i=0;i<nInvalidVolumes;i++)
    {
        METHODCHECKRESULT("IAudioStreamVolume::SetChannelVolume",
                          hr,
                          spStreamVolume->SetChannelVolume(0, afVolumes[i]));
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Set with level = %g should not have succeeded!", afVolumes[i]);
        }
    }


    // mixed invalid tests
    for(int i=0;i<nInvalidVolumes;i++)
    {
        METHODCHECKRESULT("IAudioStreamVolume::SetChannelVolume",
                          hr,
                          spStreamVolume->SetChannelVolume(nChannelCount, afVolumes[i]));
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: Set with level = %g should not have succeeded!", afVolumes[i]);
        }
    }
    
END:    
    return FNS_PASS;

}

//------------------------------------------------------------------------------
// Desc: Various invalid GetChannelVolume scenarios
// Returns:
//              FNS_PASS (failures are signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_GetChannelVolume_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    UINT32 nChannelCount = 0;
    FLOAT fChanVolBuff = 0.0f;

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&nChannelCount));
    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to obtain channel count, aborting!");
        goto END;
    }

    //
    // invalid channels
    //
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelVolume",
                      hr,
                      spStreamVolume->GetChannelVolume(nChannelCount, &fChanVolBuff));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should have failed for invalid channel index");
    }
    else if (0.0f != fChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Output parameter was changed on an invalid call!");
    }

    METHODCHECKRESULT("IAudioStreamVolume::GetChannelVolume",
                      hr,
                      spStreamVolume->GetChannelVolume(999, &fChanVolBuff));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should have failed for invalid channel index");
    }
    else if (0.0f != fChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Output parameter was changed on an invalid call!");
    }
    
    //
    // NULL-pointer
    //
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelVolume",
                      hr,
                      spStreamVolume->GetChannelVolume(0, NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should have failed for NULL param");
    }
    else if (0.0f != fChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Output parameter was changed on an invalid call!");
    }

    //
    // two invalid arguments
    //
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelVolume",
                      hr,
                      spStreamVolume->GetChannelVolume(nChannelCount, NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should have failed for multiple bad params");
    }
    else if (0.0f != fChanVolBuff)
    {
        XLOG(XFAIL, eError, "FAIL: Output parameter was changed on an invalid call!");     
    }

END:
    return FNS_PASS;
}
    
//------------------------------------------------------------------------------
// Desc: Various invalid SetAllVolumes scenarios
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_SetAllVolumes_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    UINT32 nChannelCount = 0;
    FLOAT *pfChanVolBuff = NULL; 

    UINT32 nChannelToSet = 0;

    FLOAT afBadVolumes[4] = {-99.0f, -2.0f, 2.0f, 99.0f};
    INT nBadVolumes = sizeof(afBadVolumes)/sizeof(FLOAT);

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&nChannelCount));
    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to obtain channel count, aborting!");
        goto END;
    }

    //
    // invalid channel counts
    //

    // channel count = nChannelCount+1

    pfChanVolBuff = new float[nChannelCount+1];
    if(NULL == pfChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    
    METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                      hr,
                      spStreamVolume->SetAllVolumes(nChannelCount+1, pfChanVolBuff));    
    SAFE_DELETE_ARRAY(pfChanVolBuff);
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: SetAllVolumes should fail for invalid channel count!");
    }

    // channel count = 999

    pfChanVolBuff = new float[999];
    if(NULL == pfChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    
    METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                      hr,
                      spStreamVolume->SetAllVolumes(999, pfChanVolBuff));    
    SAFE_DELETE_ARRAY(pfChanVolBuff);
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: SetAllVolumes should fail for invalid channel count!");
    }

    //
    // NULL volume pointer
    //
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                      hr,
                      spStreamVolume->SetAllVolumes(nChannelCount, NULL));    
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: SetAllVolumes should fail for NULL param!");
    }

    //
    // single bad channel volume level
    //

    pfChanVolBuff = new float[nChannelCount];
    if(NULL == pfChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    // avoid setting the first channel if possible
    nChannelToSet = (UINT32)(max(0, (INT)(nChannelCount-1)));

    for(int i=0;i<nBadVolumes;i++)
    {
        for(UINT32 j=0;j<nChannelCount;j++)
        {
            if(j == nChannelToSet)
            {
                pfChanVolBuff[j] = afBadVolumes[i];
            }
        }

        METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                          hr,
                          spStreamVolume->SetAllVolumes(nChannelCount, pfChanVolBuff));    
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: SetAllVolumes should fail for invalid volume level %f!", afBadVolumes[i]);
        }
    }

    //
    // multiple bad channel volume levels
    //

    for(int i=0;i<nBadVolumes;i++)
    {
        for(UINT32 j=0;j<nChannelCount;j++)
        {           
                pfChanVolBuff[j] = afBadVolumes[i];         
        }

        METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                          hr,
                          spStreamVolume->SetAllVolumes(nChannelCount, pfChanVolBuff));    
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: SetAllVolumes should fail for invalid volume level %f!", afBadVolumes[i]);
        }
    }


    SAFE_DELETE_ARRAY(pfChanVolBuff);

    //
    // invalid-count, invalid level
    //
    pfChanVolBuff = new float[nChannelCount+1];
    if(NULL == pfChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    for(int i=0;i<nBadVolumes;i++)
    {
        for(UINT32 j=0;j<nChannelCount+1;j++)
        {           
                pfChanVolBuff[j] = afBadVolumes[i];         
        }

        METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                          hr,
                          spStreamVolume->SetAllVolumes(nChannelCount, pfChanVolBuff));    
        if(!FAILED(hr))
        {
            XLOG(XFAIL, eFatalError, "FAIL: SetAllVolumes should fail for invalid volume level %f!", afBadVolumes[i]);
        }
    }

    SAFE_DELETE_ARRAY(pfChanVolBuff);

    //
    // invalid-count, NULL pointer
    //
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                      hr,
                      spStreamVolume->SetAllVolumes(nChannelCount+1, NULL));    
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "Error: SetAllVolumes should fail for invalid channel count and NULL param!");
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Various invalid GetAllVolumes scenarios
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_GetAllVolumes_Invalid0(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    UINT32 nChannelCount = 0;
    FLOAT *pfChanVolBuff = NULL; 

    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&nChannelCount));
    
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to obtain channel count, aborting!");
        goto END;
    }

    //
    // Invalid channel counts
    //

    // nChannelCount+1 channels case
    pfChanVolBuff = new FLOAT[nChannelCount+1];
    if(NULL == pfChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    METHODCHECKRESULT("IAudioStreamVolume::GetAllVolumes",
                      hr,
                      spStreamVolume->GetAllVolumes(nChannelCount+1, pfChanVolBuff));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded with invalid channel count!");
    }

    SAFE_DELETE_ARRAY(pfChanVolBuff);
    

    // 999 channels case
    pfChanVolBuff = new FLOAT[999];
    if(NULL == pfChanVolBuff)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }
    
    METHODCHECKRESULT("IAudioStreamVolume::GetAllVolumes",
                      hr,
                      spStreamVolume->GetAllVolumes(999, pfChanVolBuff));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded with invalid channel count!");
    }

    SAFE_DELETE_ARRAY(pfChanVolBuff);

    //
    // NULL volume pointer
    //
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioStreamVolume::GetAllVolumes",
                      hr,
                      spStreamVolume->GetAllVolumes(nChannelCount, NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded with NULL param!");
    }
    

    //
    // Invalid channel count, null VOLUME pointer mix
    //
#pragma prefast(suppress:6309, "Disable parameter checking")
    METHODCHECKRESULT("IAudioStreamVolume::GetAllVolumes",
                      hr,
                      spStreamVolume->GetAllVolumes(nChannelCount+1, NULL));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded with multiple bad params!");
    }

END:
    return FNS_PASS;
}

//------------------------------------------------------------------------------
// Desc: Test IAudioStreamVolume methods under RPC failure conditions
//
// Returns:
//              FNS_PASS (failure is signalled via XLOG)
//------------------------------------------------------------------------------
DWORD Test_StreamAudioVolume_DisconnectService(DWORD dwDeviceType, ITestStreamingEngine* pStreamingEngine)
{
    HRESULT hr = S_OK;
    CComPtr<IAudioStreamVolume> spStreamVolume = NULL;

    UINT32 dwCount = 0;
    UINT32 dwDummyCount = 0;
    FLOAT fChanVol = 0.0f;
    FLOAT* pfSetChanVolArray = NULL;
    FLOAT* pfGetChanVolArray = NULL;
    
    assert(NULL != pStreamingEngine);

    hr = pStreamingEngine->GetStreamAudioVolume(&spStreamVolume);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain IAudioStreamVolume for test!");
    }

    // cache the channel count before attempting to shut down the WAS
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&dwCount));
    if(FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Unable to obtain stream channel count, aborting.");
        goto END;
    }

    if(dwCount == 0)
    {
        XLOG(XFAIL, eFatalError, "FAIL: Stream reports 0 channels, aborting.");
        goto END;
    }

    // allocate and initialize buffers for setting/retrieving channel volumes
    pfSetChanVolArray = new FLOAT[dwCount];
    pfGetChanVolArray = new FLOAT[dwCount];

    if((NULL == pfSetChanVolArray)||(NULL == pfGetChanVolArray))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Out of memory!");
        goto END;
    }

    for(UINT32 i=0;i<dwCount;i++)
    {
        pfSetChanVolArray[i] = 1.0f;
        pfGetChanVolArray[i] = 0.0f;
    }
    
    // shut down audiosrv
    if (!StopAudioService())
    {
        XLOG(XFAIL, eError, "FAIL: Unable to shut down audiosrv!");
        goto END;
    }  
   
    // BEGIN IAudioStreamVolume API Calls
    ////
    
    // GetChannelCount
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelCount",
                      hr,
                      spStreamVolume->GetChannelCount(&dwDummyCount));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }

    // SetChannelVolume
    METHODCHECKRESULT("IAudioStreamVolume::SetChannelVolume",
                      hr,
                      spStreamVolume->SetChannelVolume(0, 1.0f));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }


    // GetChannelVolume
    METHODCHECKRESULT("IAudioStreamVolume::GetChannelVolume",
                      hr,
                      spStreamVolume->GetChannelVolume(0, &fChanVol));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }

    // SetAllVolumes
    METHODCHECKRESULT("IAudioStreamVolume::SetAllVolumes",
                      hr,
                      spStreamVolume->SetAllVolumes(dwCount, pfSetChanVolArray));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }

    // SetAllVolumes
    METHODCHECKRESULT("IAudioStreamVolume::GetAllVolumes",
                      hr,
                      spStreamVolume->GetAllVolumes(dwCount, pfGetChanVolArray));
    if(!FAILED(hr))
    {
        XLOG(XFAIL, eFatalError, "FAIL: Method should not have succeeded");
    }
   
    ////
    // END IStreamAudioVolume API Calls
    
END:
   // Start audiosrv back up. Moving this into end section so that even if this function fails, the following test cases do not fail.
    if (!StartAudioService())
    {
        XLOG(XFAIL, eFatalError, "ERROR: Unable to re-start AudioSrv! System may be in an unusable state!");
    }

    SAFE_DELETE_ARRAY(pfSetChanVolArray);
    SAFE_DELETE_ARRAY(pfGetChanVolArray);
    
    return FNS_PASS;
}

