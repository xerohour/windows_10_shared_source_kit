// Copyright(C) Microsoft. All rights reserved.

//
// SessionVolumeStress.h
//
// Description:
//
//  VOLUME Stressor Class
//
//  Change history:
//      15 June 2005 -- Created this file
//      10 August 2007 -- Ported test to new UMATest app

#pragma once

#include <stdafx.h>

//-------------------------------------------------------------------------
// Class: CVolumeStressor
//
// Description:
//  Class encapsulating per-app volume stress testing
//
class CVolumeStressor
{   
public:
    CVolumeStressor();
    ~CVolumeStressor();

    VOID                                        RunStress(UINT nClients);

private:
    UINT32                                      m_nStreamsUnderTest;    
    CComPtr<ITestStreamingEngine>*              m_aspStreamingEngines;
    CComPtr<IRenderTestStreamingEngine>*        m_aspRenderStreamingEngines;
    CComPtr<ICaptureTestStreamingEngine>*       m_aspCaptureStreamingEngines;
    CComPtr<IAudioSessionControl>*              m_aspAudioSessionControls;
    LPWSTR*                                     m_awszSessionIDs;
           
    HRESULT                                     SpawnAudioSessions();
    HRESULT                                     RampSessionVolumes();
	HRESULT                                     RampSingleSession(IAudioSessionControl* pSession);
	VOID                                        Cleanup();        
};

