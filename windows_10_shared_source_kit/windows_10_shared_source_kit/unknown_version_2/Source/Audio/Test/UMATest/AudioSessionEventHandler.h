// Copyright(C) Microsoft. All rights reserved.

//------------------------------------------------------------------------------
// File: AudioSessionEventHandler.h
//
// Desc: Inteface definition for AudioSessionEvent Test Handler
//------------------------------------------------------------------------------

#pragma once

/*
 * Audio Event-Type Codes
 * (these codes are arbitrary; they have no bearing on anything
 */
static const UINT AUDIO_EVENT_SIMPLEVOLUME_CHANGED = 0;
static const UINT AUDIO_EVENT_CHANNELVOLUME_CHANGED = 1;
static const UINT AUDIO_EVENT_STATE_CHANGED = 2;
static const UINT AUDIO_EVENT_GROUPINGPARAM_CHANGED = 3;
static const UINT AUDIO_EVENT_ICONPATH_CHANGED = 4;
static const UINT AUDIO_EVENT_DISPLAYNAME_CHANGED = 5;
static const UINT AUDIO_EVENT_SESSION_DISCONNECTED = 6;

LPCSTR StringFromDisconnectReason(AudioSessionDisconnectReason Reason);

//
//  Class for handling audiosession event callbacks
//
class CAudioSessionEventHandler: public IAudioSessionEvents
{
public:
    CAudioSessionEventHandler();
    ~CAudioSessionEventHandler();

    //  IUnknown
    STDMETHODIMP            QueryInterface(REFIID  iid, void **ppv);
    STDMETHODIMP_(ULONG)    AddRef(void);
    STDMETHODIMP_(ULONG)    Release(void);
    
    //  IAudioSessionEvents
    STDMETHODIMP    OnDisplayNameChanged(LPCWSTR newDisplayName, LPCGUID pguidEventContext);
    STDMETHODIMP    OnIconPathChanged(LPCWSTR newIconPath, LPCGUID pguidEventContext);
    STDMETHODIMP    OnSimpleVolumeChanged(float newVolume,BOOL newMute, LPCGUID pguidEventContext);
    STDMETHODIMP    OnChannelVolumeChanged(DWORD dwChannelCount, float afChannelVolumeArray[  ], DWORD dwChangedChannel, LPCGUID pguidEventContext);
    STDMETHODIMP    OnGroupingParamChanged(LPCGUID newGroupingParam, LPCGUID pguidEventContext);
    STDMETHODIMP    OnStateChanged(AudioSessionState newState);
    STDMETHODIMP    OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason);
   
    VOID        ResetHandler(void);
    VOID        ClearContextIdentifiers(void);
    VOID        SetEventFilterAndReset(LPCGUID pguidFilter);
    VOID        SetEventFilterNoReset(LPCGUID pguidFilter);

    // Event data accessor methods
   
    UINT        GetDisplayNameChangedEventCount(void);
    UINT        GetIconPathChangedEventCount(void);
    UINT        GetVolumeChangedEventCount(void);
    UINT        GetChannelVolumeChangedEventCount(void);
    UINT        GetStateChangedEventCount(void);
    UINT        GetGroupingParamChangedEventCount(void);
    UINT        GetSessionDisconnectedEventCount(void);

    HRESULT     GetLastReportedDisplayName(LPCWSTR*);
    HRESULT     GetLastReportedIconPath(LPCWSTR*);
    HRESULT     GetLastReportedSimpleVolume(FLOAT*);
    HRESULT     GetLastReportedChannelVolume(DWORD*, FLOAT**, DWORD*);
    HRESULT     GetLastReportedMuteState(BOOL*);
    HRESULT     GetLastReportedSessionState(AudioSessionState*);
    HRESULT     GetLastReportedGroupingParam(GUID*);
    HRESULT     GetLastReportedDisconnection(AudioSessionDisconnectReason*);

    LPCGUID     GetLastDisplayNameChangeContext(void);
    LPCGUID     GetLastIconPathChangeContext(void);
    LPCGUID     GetLastSimpleVolumeChangeContext(void);
    LPCGUID     GetLastChannelVolumeChangeContext(void);
    LPCGUID     GetLastGroupingParamChangeContext(void);

private:
    LONG                            m_cRef;

    CCriticalSection                m_EventDataLock;

    // guid pointer for filtering incoming event data
    // Note: Setting this value to NULL means no filter is applied
    LPCGUID                         m_pguidEventFilter;

    // private method used for event filtering
    BOOL                            _EventFilterTest(LPCGUID pguidTestContext);
    
    // received event counts
    UINT                            m_nDisplayNameChangedEvents;
    UINT                            m_nIconPathChangedEvents;
    UINT                            m_nSimpleVolumeChangedEvents;
    UINT                            m_nChannelVolumeChangedEvents;
    UINT                            m_nStateChangedEvents;
    UINT                            m_nGroupingParamChangedEvents;
    UINT                            m_nSessionDisconnectedEvents;

    // last-received event data
    LPCWSTR                         m_wszLastReportedDisplayName;
    LPCGUID                         m_pguidLastDisplayNameChangeContext;
    
    LPCWSTR                         m_wszLastReportedIconPath;
    LPCGUID                         m_pguidLastIconPathChangeContext;
    
    FLOAT*                          m_pfLastReportedChannelVolumes;        
    DWORD                           m_dwReportedChannelVolumes;    
    DWORD                           m_dwLastChangedChannel;
    LPCGUID                         m_pguidLastChannelVolumeChangeContext;

    FLOAT                           m_fLastReportedSimpleVolume;       
    BOOL                            m_bLastReportedMuteState;
    LPCGUID                         m_pguidLastSimpleVolumeChangeContext;

    AudioSessionState               m_objLastReportedSessionState;

    GUID                            m_guidLastReportedGroupingParam;
    LPCGUID                         m_pguidLastGroupParamChangeContext;

    AudioSessionDisconnectReason    m_objLastReportedDisconnectReason;

    // Event Context ID hash
};
