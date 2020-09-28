// Copyright(C) Microsoft. All rights reserved.

//
// TestUtilities.h
//
// Description:
//
//  Misc Utility Function to support UMATest
//

#pragma once

#include <DeviceAccessHelper.h>
#include <EndpointModule.h>

class CEndpointVolumeEventHandler;
class CAudioSessionEventHandler;

// Logging utilities
void    LogGUID(REFGUID Guid);
void    LogDate(DATE date);

// Verify the validity of a date structure
HRESULT    VerifyDate(DATE date);

// Wait for an endpoint volume event
HRESULT WaitForEndpointEvent(CEndpointVolumeEventHandler* pEventHandler, ULONG nEventTimeoutMillis);

// Wait for an audiosession volume/control event
HRESULT WaitForSessionEvent(CAudioSessionEventHandler* pEventHandler, ULONG nEventType, ULONG nEventTimeoutMillis);

// Convert a DB volume value to a unit-scalar (wiper) value
FLOAT ConvertDbToWiper(FLOAT fDbVol, FLOAT fMinDbVolume, FLOAT fMaxDbVolume, DWORD dwDeviceType);

// Create an empty AudioSession in the current process
HRESULT CreateEmptySessionInProcess(IAudioSessionManager* pPubAudSessMgr, __out LPWSTR& wszNewSessionID);

struct ITestStreamingEngine;
struct IRenderTestStreamingEngine;
struct ICaptureTestStreamingEngine;
// Create a new audiosession with a single stream in the current process
// Note: Stream flow direction is determined by the device type under test
HRESULT CreateStreamSessionInProcess(ITestStreamingEngine** ppStreamingEngine, 
                                     IRenderTestStreamingEngine** ppRenderStreamingEngine,
                                     ICaptureTestStreamingEngine** ppCaptureStreamingEngine,
                                     __out LPWSTR& wszNewSessionID);

// forward declaration
namespace AudioOffload
{
enum EApiLevel;
}
// check if the current device supports offload. If the test is run within app container, then 
// the check is done through device access helper.
HRESULT CheckDeviceSupportsOffload(IMMDevice *pDevice, AudioOffload::EApiLevel api, bool& fSupported);

// check instance information for device. If the test is run within app container, then 
// the check is done through device access helper.
HRESULT GetMMDeviceKSInstanceInformation(IMMDevice *pMMDevice, ULONG &cPossibleInstance, ULONG &cCurrentInstances);

// check if the current device supports raw mode.
HRESULT CheckDeviceSupportsRawMode(IMMDevice* pMMDevice, BOOL * pbRawModeSupported);

// returns a randomly selected float
// in the range [0, 1] inclusive
FLOAT rand_float();

