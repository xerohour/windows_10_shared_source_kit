// Copyright(C) Microsoft. All rights reserved./

//
#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOVolumeCreate();
DWORD Tc_APOVolumeInitialize();
DWORD Tc_APOVolumeSetVolCoeffns();
DWORD Tc_APOVolumeGetVolCoeffns();
DWORD Tc_APOVolumeSetMute();
DWORD Tc_APOVolumeGetMute();
DWORD Tc_APOVolumeNoInitialRamp();
DWORD Tc_APOVolumeProcess();
DWORD Tc_ManualAPOProcess();
DWORD Tc_APOVolumeParamCurveWindowsFade();

/*typedef enum { TYPE_INT16 = 0,           
               TYPE_FLOAT32 } TYPE_SAMPLE;*/

typedef struct _LOCK_PARAMS {
    UINT32 u32NumInConn;
    APO_CONNECTION_DESCRIPTOR** ppInputConnections;
    UINT32 u32NumOutConn;
    APO_CONNECTION_DESCRIPTOR** ppOutputConnections;
} LOCK_PARAMS, *PLOCK_PARAMS;


class CAPOVolumeTest : public CComBase
{
public:
    CAPOVolumeTest()
    {
        HRESULT hr = S_OK;
        m_pVolume = NULL;
        SLOG(eInfo1, "Creating Volume Object");
        hr = m_pVolume.CreateInstance(__uuidof(AudioVolume));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Volume Object, 0x%X", hr);
            return;
        }
        m_pBaseAPO = NULL;
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &m_pBaseAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Query interface on APO Object, 0x%X", hr);            
        }

        m_pAPO = NULL;
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pVolume.QueryInterface(__uuidof(IAudioProcessingObject), &m_pAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Query interface on APO Object, 0x%X", hr);            
        }

        m_pMute = NULL;
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pVolume.QueryInterface(__uuidof(IAudioMuteAPO), &m_pMute);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Query interface on APO Object, 0x%X", hr);            
        }

		// common fields in the apo init
		apoParams.APOInit.cbSize = sizeof( apoParams );
		apoParams.APOInit.clsid  = __uuidof( AudioVolume );

    };
    virtual ~CAPOVolumeTest()
    {
        m_pMute = NULL;
        m_pVolume = NULL;
        m_pBaseAPO = NULL;
    };     
    
    // Basic Object creation
    DWORD APOVolumeCreate();
    // IAudioVolume method tests 
    DWORD APOSetVolCoeffns();
    DWORD APOGetVolCoeffns();
    DWORD APOInitialize(); 
    DWORD APOGetMute();
    DWORD APOSetMute();
    // Functional Test
    DWORD APOTestVolume();
    DWORD APOTestParamCurveWindowsFade();
    DWORD APOTestNoInitialRamp(AUDIO_CURVE_TYPE ParamCurve);
    //Manual Functional Test
    DWORD APOTestManual();
    
    
    
private:    
    IAudioVolumePtr                             m_pVolume;
    IAudioMuteAPOPtr                            m_pMute;
    IAudioProcessingObjectConfigurationPtr      m_pBaseAPO;
    IAudioProcessingObjectPtr					m_pAPO;

	APO_VOLUME_PARAMS							apoParams;

	// Private methods
    HRESULT CreateObjects();
    //HRESULT Initialize();
    HRESULT LockAPO(UINT32 u32SamplesPerFrame = 1, 
                    FLOAT32* pfCoeffns = NULL,
                    MFTIME* mfTime = NULL,
                    AUDIO_CURVE_TYPE = AUDIO_CURVE_TYPE_NONE,
                    MFTIME* pCurveDuration = NULL,
                    PLOCK_PARAMS pLockParams = NULL);

    HRESULT TestFunctional(UINT32 u32Channels, 
                           FLOAT32* pfVolCoeffns,
                           UINT32 u32NumInConn,
                           APO_CONNECTION_PROPERTY* pInputConn,
                           UINT32 u32NumOutConn,
                           APO_CONNECTION_PROPERTY* pOutputConn,
                           AUDIO_CURVE_TYPE ParamCurve = AUDIO_CURVE_TYPE_NONE,
                           MFTIME* pCurveDuration = NULL);
};
