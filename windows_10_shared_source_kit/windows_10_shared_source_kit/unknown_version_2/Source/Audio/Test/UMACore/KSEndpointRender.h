// Copyright(C) Microsoft. All rights reserved.

#pragma once

DWORD Tc_KSRenderEndpointCreate();
DWORD Tc_KSRenderEndpointGetPosition();
DWORD Tc_KSRenderEndpointGetFrequency();
DWORD Tc_KSRenderEndpointCPClientInitialize();
DWORD Tc_KSRenderEndpointTestPositions();
DWORD Tc_KSRenderEndpointSetBuffer();
DWORD Tc_KSRenderEndpointStream();
DWORD Tc_KSRenderEndpointWriteExclusiveModeParametersToSharedMemory();
DWORD Tc_KSRenderEndpointGetRTCaps();
DWORD Tc_KSRenderEndpointGetFrameFormat();
DWORD Tc_KSRenderEndpointGetLatency();
DWORD Tc_KSRenderEndpointGetFramesPerPacket();
DWORD Tc_KSRenderEndpointGetProtectedOutputController();
DWORD Tc_KSRenderEndpointCreateVirtualProtectedOutput();
DWORD Tc_KSRenderEndpointGetDescription();
DWORD Tc_KSRenderEndpointSetPolicySchemas();

class CKSEndpointRenderTest : public CKSEndpointBaseTest
{
public:
    CKSEndpointRenderTest():CKSEndpointBaseTest(true)
    {};
    virtual ~CKSEndpointRenderTest()
    {};
    HRESULT InitializeAndSetBuffer(LPCWSTR szPnpID  = NULL, PROPVARIANT* pCreateParams = NULL, MFTIME mfBufferTime = 10 * MFTIME1MSEC);
    HRESULT CheckExclusiveLoopedBufferSize();
    HRESULT CheckRealtimeExclusiveLoopedBufferSize();
    HRESULT CheckStandardExclusiveLoopedBufferSize();
    HRESULT GetAudioVPO(UINT uiAction, IAudioVirtualProtectedOutput **ppAudioVPO);
    DWORD EndpointGetOutputDataPointer();
    DWORD EndpointReleaseOutputDataPointer();
    DWORD EndpointStream();
    DWORD EndpointGetProtectedOutputController();
    DWORD EndpointCreateVirtualProtectedOutput();
    DWORD EndpointGetDescription();
    DWORD EndpointSetPolicySchemas();
};
