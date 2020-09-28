// Copyright(C) Microsoft. All rights reserved.

#pragma once

//VOID CALLBACK TimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);

class CKSEndpointBaseTest : public CComBase
{
public:

    IAudioDeviceEndpointPtr m_pDevEndpoint; //Device Endpoint

    CKSEndpointBaseTest(bool bRender);
//    {
//        HRESULT hr = S_OK;
//
//        m_bRender = bRender;       
//        SLOG(eInfo1, "Creating Device Endpoint");
//        hr =m_pDevEndpoint.CreateInstance( m_bRender ? __uuidof(AudioKSRenderEndpoint) 
//                                                     : __uuidof(AudioKSCaptureEndpoint)
//                                         );
//        if(FAILED(hr))
//        {
//            SLOG(eError, "ERROR: Device Endpoint Creation FAILED, 0x%X", hr);
//        }        
//    };

    virtual ~CKSEndpointBaseTest()
    {
        m_pDevEndpoint = NULL;
    };

    HRESULT CreateEndpoint();
//    {
//        SLOG(eInfo1, "Creating Device Endpoint");
//        return m_pDevEndpoint.CreateInstance( m_bRender ? __uuidof(AudioKSRenderEndpoint) 
//                                                     : __uuidof(AudioKSCaptureEndpoint)
//                                            );
//    };
    DWORD EndpointCreate();
    DWORD EndpointGetFrequency();
    DWORD EndpointGetPosition();
    DWORD EndpointGetGlitchCount();
    DWORD EndpointCPClientInitialize();
    DWORD EndpointSetBuffer();
    DWORD EndpointWriteExclusiveModeParametersToSharedMemory();
    DWORD EndpointGetRTCaps();
    DWORD EndpointGetFrameFormat();
    DWORD EndpointGetLatency();
    DWORD EndpointGetFramesPerPacket();
    DWORD EndpointStart();
    DWORD EndpointStop();
    DWORD EndpointReset();
    DWORD EndpointGetCurrentPadding();
    DWORD EndpointGetTimeStamp();
    DWORD EndpointTestPositions();
    HRESULT GetIMMDevice(IMMDevice **ppIMMDevice);
    static void TimerCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);
    virtual HRESULT InitializeAndSetBuffer(LPCWSTR szPnpID = NULL, PROPVARIANT* pCreateParams = NULL, MFTIME mfBufferTime = 10 * MFTIME1MSEC) = 0;
    virtual HRESULT CheckExclusiveLoopedBufferSize() = 0;
    bool                    m_bRender;    

private:    
    HRESULT CreateRenderGraph(CGraphRepresentation** ppGraph, IAudioEndpointRT* InputEndpoint, IAudioEndpointRT* OutputEndpoint);
    HRESULT CreateCaptureGraph(CGraphRepresentation** ppGraph, IAudioEndpointRT* InputEndpoint, IAudioEndpointRT* OutputEndpoint);
    DWORD TestBaseEndPointInitialize
    (
        UINT_PTR    hSharedMemHandle, 
        UINT32      u32MaxFramesPerCall,
        UINT32*     pu32SharedMemorySize,
        UINT_PTR*   phSharedMemory,
        BOOL        bWriteExclModeParams,
        BOOL        bValidTestCase
    );
    CComPtr<IMMDevice>  m_pIMMDevice;
    
protected:
    HRESULT SetDeviceBuffer(MFTIME mfRequestedBufferTime, WAVEFORMATEX * lpWfx);

};
