// Copyright(C) Microsoft. All rights reserved.

#pragma once

DWORD Tc_KSCaptureEndpointCreate();
DWORD Tc_KSCaptureEndpointGetFrequency();
DWORD Tc_KSCaptureEndpointGetPosition();
DWORD Tc_KSCaptureEndpointCPClientInitialize();
DWORD Tc_KSCaptureEndpointSetBuffer();
DWORD Tc_KSCaptureEndpointTestPositions();
DWORD Tc_KSCaptureEndpointWriteExclusiveModeParametersToSharedMemory();
DWORD Tc_KSCaptureEndpointGetRTCaps();
DWORD Tc_KSCaptureEndpointGetFrameFormat();
DWORD Tc_KSCaptureEndpointGetLatency();
DWORD Tc_KSCaptureEndpointGetFramesPerPacket();

class CKSEndpointCaptureTest : public CKSEndpointBaseTest
{
public:
    CKSEndpointCaptureTest():CKSEndpointBaseTest(false)
    {};
    virtual ~CKSEndpointCaptureTest()
    {};
    
    HRESULT InitializeAndSetBuffer(LPCWSTR szPnpID  = NULL, PROPVARIANT* pCreateParams = NULL, MFTIME mfBufferTime = 10 * MFTIME1MSEC);
    HRESULT CheckExclusiveLoopedBufferSize();
    HRESULT CheckRealtimeExclusiveLoopedBufferSize();
    HRESULT CheckStandardExclusiveLoopedBufferSize();
    DWORD   EndpointGetInputDataPointer();
    DWORD   EndpointReleaseInputDataPointer();
};
