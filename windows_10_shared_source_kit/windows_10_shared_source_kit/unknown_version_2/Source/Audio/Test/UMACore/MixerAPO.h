// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOMixerCreate();
DWORD Tc_APOMixerProcess();
DWORD Tc_APOMixerProcesswProcMix();
DWORD Tc_APOMixerProcesswProcMix_Copy();

class CAPOMixerTest : public CComBase
{
public:
    CAPOMixerTest()
    {        
        HRESULT hr = S_OK;
        SLOG(eInfo1, "Creating Mixer Object");
        hr = m_pMixer.CreateInstance(__uuidof(AudioMixer));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Mixer Object, 0x%X", hr);
            return;
        }
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pMixer.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &m_pBaseAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Object, 0x%X", hr);            
        }

    };
    virtual ~CAPOMixerTest()
    {
        m_pBaseAPO = NULL;
        m_pMixer = NULL;
    };     
    
    // Basic Object creation
    DWORD APOMixerCreate();
    DWORD APOTestMixer();
    DWORD APOTestwProcMix_Copy();
private:    
    IAudioMixerPtr                          m_pMixer;
    IAudioProcessingObjectConfigurationPtr  m_pBaseAPO;
    HRESULT CreateObjects();
    //HRESULT LockAPO(/*AUDIO_MIXER_FLAGS audMixFlags = AUDIO_MIXER_COPY*/);
    HRESULT TestFunctional( UINT32 u32NumInConn
                            ,IAudioMediaType** ppInputAudioType
                            ,APO_CONNECTION_PROPERTY* pInConn
                            ,UINT32 u32NumOutConn
                            ,IAudioMediaType** ppOutputAudioType
                            ,APO_CONNECTION_PROPERTY* pOutConn
                            );
};
