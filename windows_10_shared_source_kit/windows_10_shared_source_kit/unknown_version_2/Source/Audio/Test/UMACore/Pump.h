// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)

//Global Test functions
DWORD Tc_AudioPumpCreate();
DWORD Tc_AudioPumpInitialize();
//DWORD Tc_AudioPumpInitializeDependant();
DWORD Tc_AudioPumpStart();
DWORD Tc_AudioPumpStop();
DWORD Tc_AudioPumpGetRegisteredClock();
DWORD Tc_AudioPumpReconfigureTimer();


class CAudioPumpTest : public CComBase
{
public:
    CAudioPumpTest()
    {
        HRESULT hr = S_OK;
        
        SLOG(eInfo1, "Creating Audio Pump");
        hr = m_pAudioPump.CreateInstance(__uuidof(AudioPump));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Audio Pump, 0x%X", hr);
            m_pAudioPump = NULL;
        }
    };
    virtual ~CAudioPumpTest()
    {        
        m_pAudioPump = NULL;
    };        
    //Actual test cases
    DWORD AudioPumpInitialize();
    //DWORD AudioPumpInitializeDependant();
    DWORD AudioPumpCreate();
    DWORD AudioPumpStart();
    DWORD AudioPumpStop();    
    DWORD AudioPumpReconfigureTimer();
    // Internal Helper functions
    HRESULT CreatePump();
    HRESULT InitializePump(LPVOID pBuffer, UINT32 cbBuffer);
    HRESULT CreateRenderEndpoint(IAudioEndpointRTPtr* ppAudioRenderEndpoint);
private:
    	IAudioPumpPtr         m_pAudioPump;
};



//class CBaseTestObject;
//typedef DWORD_PTR* DWPTR;
//
//// Audio Pump Test class.
//class CAudioPumpTest : public CBaseTestObject
//{
//public:
//    CAudioPumpTest();
//    virtual ~CAudioPumpTest()
//    {
//        if(m_pAudioPump)
//            m_pAudioPump->Release();
//        m_pAudioPump = NULL;
//        if(m_pProcessor)
//            m_pProcessor->Release();
//        m_pProcessor = NULL;
//    };    
//    //get
//    //const DWORD_PTR*  operator[](LPWSTR szProperty) const;    
//    //set
//	DWPTR&  operator[](LPWSTR szProperty);
//    // This is the function dispatch
//	DWORD operator()(LPWSTR szMethod);
//private:
//    // All internal variables are declared as Properties
//    DWORD_PTR*          m_pu32PumpFlags;    
//    DWORD_PTR*          m_pMFTime;
//    IAudioPump*         m_pAudioPump;        
//    IAudioProcessRT*    m_pProcessor;    
//    // All methods are called from the dispatch function
//    DWORD AudioPumpInitialize();
//    DWORD AudioPumpCreate();
//    DWORD AudioPumpStart();
//    DWORD AudioPumpStop();
//    HRESULT InitPump();
//};

//class CBaseTestObject
//{
//public:
//	virtual ~CBaseTestObject() {}
//	// get property
//	virtual const DWORD_PTR*  operator[](LPWSTR szProperty) const = 0;
//	// set property
//	virtual DWORD_PTR*  operator[](LPWSTR szProperty) = 0;
//	// method dispatch
//	virtual DWORD operator()(LPWSTR szMethod) = 0;
//};
//class CSimpleTest : public CBaseTestObject
//{
//public:
//	const DWORD_PTR*  operator[](LPWSTR szProperty) const;
//	DWORD_PTR*  operator[](LPWSTR szProperty);
//	DWORD operator()(LPWSTR szMethod);
//  DWORD TestMethod();
//};
