// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)



DWORD Tc_APOMeterCreate();
DWORD Tc_APOMeterInitialize();
DWORD Tc_APOMeterGetMeteringData();
DWORD Tc_APOMeterProcess();
DWORD Tc_APOMeterCheckAllocation();

class CAPOMeterTest : public CComBase
{
public:
    CAPOMeterTest()
    {
        HRESULT hr = S_OK;
        m_pMeter = NULL;
        SLOG(eInfo1, "Creating Meter Object");
        hr = m_pMeter.CreateInstance(__uuidof(AudioMeter));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Meter Object, 0x%X", hr);
            return;
        }
        m_pBaseAPO = NULL;
        SLOG(eInfo1, "Creating APO Config Object");
        hr = m_pMeter.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &m_pBaseAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Config Object, 0x%X", hr);            
        }

        m_pAPO = NULL;
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pMeter.QueryInterface(__uuidof(IAudioProcessingObject), &m_pAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Object, 0x%X", hr);            
        }

		// common fields in the apo init
		apoParams.APOInit.cbSize = sizeof( apoParams );
		apoParams.APOInit.clsid  = __uuidof( AudioMeter );
    };

	virtual ~CAPOMeterTest()
    {
        m_pMeter = NULL;
        m_pBaseAPO = NULL;
		m_pAPO = NULL;
    };     
    
    // Basic Object creation
    DWORD APOMeterCreate();
    //API tests
    DWORD APOMeterGetMeteringData();
    DWORD APOMeterInitialize();
    // Functional Test
    DWORD APOTestMeter();
    DWORD APOCheckAllocation();
    
    //Manual Functional Test
    
    
    
    
private:    
    IAudioMeterPtr                              m_pMeter;
    IAudioProcessingObjectConfigurationPtr      m_pBaseAPO;
    IAudioProcessingObjectPtr					m_pAPO;

	APO_METER_PARAMS							apoParams;

	// Private methods
    HRESULT CreateObjects();
    //HRESULT Initialize();
    HRESULT LockAPO(IAudioMediaType** ppInputAudioMediaType);

    HRESULT TestFunctional(UINT32 u32Channels, 
                           FLOAT32* pfVolCoeffns,
                           UINT32 u32NumInConn,
                           APO_CONNECTION_PROPERTY* pInputConn,
                           UINT32 u32NumOutConn,
                           APO_CONNECTION_PROPERTY* pOutputConn);
};
