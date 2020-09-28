// Copyright(C) Microsoft. All rights reserved.

//------------------------------------------------------------------------------
// File: AudioSessionEventHandler.cpp
//
// Desc: AudioSessionEvent Test Handler Implementation
//------------------------------------------------------------------------------

#pragma once

#include "stdafx.h"

#include <UMATestStreaming.h>

#include "TestUtilities.h"
#include "AudioSessionEventHandler.h"


//-----------------------------------------------------------------------------
// Method: StringFromDisconnectReason
//
// Description:
//     Given a session disconnection reason enumerant, return a static
//     string description of the disconnect reason.
//
//    
// Parameters: Reason [in] - The disconnect reason.
//
// Return values: A static string describing the disconnect reason.  This
//                value will never be NULL.
//
// ----------------------------------------------------------------------------
LPCSTR StringFromDisconnectReason(AudioSessionDisconnectReason Reason)
{
    switch(Reason)
    {
        case DisconnectReasonDeviceRemoval:
            return "DisconnectResonDeviceRemoval";
        case DisconnectReasonServerShutdown:
            return "DisconnectReasonServerShutdown";
        case DisconnectReasonFormatChanged:
            return "DisconnectReasonFormatChanged";
        case DisconnectReasonSessionLogoff:
            return "DisconnectReasonSessionLogoff";
        case DisconnectReasonSessionDisconnected:
            return "DisconnectReasonSessionDisconnected";
        case DisconnectReasonExclusiveModeOverride:
            return "DisconnectReasonExclusiveModeOverride";
        default:
            return "UnknownReason";
    }
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::CAudioSessionEventHandler()
//
// Description:
//     Default Constructor
//
//    
// Parameters: N/A
//
// Return values: N/A
//
// ----------------------------------------------------------------------------
CAudioSessionEventHandler::CAudioSessionEventHandler()
{
    m_cRef = 1;

    m_EventDataLock.Enter();
        m_wszLastReportedDisplayName = NULL;
        m_wszLastReportedIconPath = NULL;
        m_pfLastReportedChannelVolumes = NULL;
        m_pguidEventFilter = NULL;
    m_EventDataLock.Leave();

    ResetHandler();
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::~CAudioSessionEventHandler()
//
// Description:
//     Class Destructor
//
//    
// Parameters: N/A
//
// Return values: N/A
//
// ----------------------------------------------------------------------------
CAudioSessionEventHandler::~CAudioSessionEventHandler()
{
    // grab the lock in case there are any queued events
    m_EventDataLock.Enter();
        SAFE_DELETE_ARRAY(m_wszLastReportedDisplayName);
        SAFE_DELETE_ARRAY(m_wszLastReportedIconPath);
        SAFE_DELETE_ARRAY(m_pfLastReportedChannelVolumes);    
    m_EventDataLock.Leave();
}    

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::AddRef()
//
// Description:
//     Add a reference to this object
//
//    
// Parameters: N/A
//
// Return values (ULONG): The number of references to this object
//
// ----------------------------------------------------------------------------
ULONG CAudioSessionEventHandler::AddRef(void)
{
    return InterlockedIncrement(&m_cRef);
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::QueryInterface()
//
// Description:
//     IUnknown::QueryInterface implementation
//
//    
// Parameters: [in] iid - The ID of the desired interface
//                        Supported Interfaces: IUnknown, IAudioSessionEvents
//
//             [out]ppv - Interface pointer -- set to a new instance of the
//                        desired interface on success
//                 
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::QueryInterface(REFIID iid, void **ppv)
{
    if (IID_IUnknown == iid)
    {
        this->AddRef();
        *ppv = (IUnknown *)(IAudioSessionEvents *)this;
    }
    else if (__uuidof(IAudioSessionEvents) == iid)
    {
        this->AddRef();
        *ppv = (IAudioSessionEvents *)this;
    }
    else
    {
        *ppv = NULL;
        return (E_NOINTERFACE);
    }

    return (S_OK);
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::Release()
//
// Description:
//     Remove a reference to this object
//
// Parameters: N/A
//
// Return values (ULONG): The number of references to this object
//
// ----------------------------------------------------------------------------
ULONG CAudioSessionEventHandler::Release(void)
{
    long RefCnt = InterlockedDecrement((long*)&m_cRef);
    if(RefCnt == 0)
    {
        delete this;
        return (0);
    }
    return (RefCnt);
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::SetEventFilterAndReset(LPCGUID)
//
// Description:
//     Set the event context ID filter for this event handler and reset
//     the event data collectors.
//
// Parameters: N/A
//
// Return values: N/A
//
// ----------------------------------------------------------------------------
VOID CAudioSessionEventHandler::SetEventFilterAndReset(LPCGUID pguidFilter)
{
    m_EventDataLock.Enter();
        m_pguidEventFilter = pguidFilter;

        m_nDisplayNameChangedEvents = 0;
        m_nIconPathChangedEvents = 0;
        m_nSimpleVolumeChangedEvents = 0;
        m_nChannelVolumeChangedEvents = 0;
        m_nStateChangedEvents = 0;
        m_nGroupingParamChangedEvents = 0;
        m_nSessionDisconnectedEvents = 0;

        m_dwReportedChannelVolumes = 0;
        m_dwLastChangedChannel = 0;

        SAFE_DELETE_ARRAY(m_wszLastReportedDisplayName);
        SAFE_DELETE_ARRAY(m_wszLastReportedIconPath);
        SAFE_DELETE_ARRAY(m_pfLastReportedChannelVolumes);

        m_fLastReportedSimpleVolume = 0.0f;
        m_bLastReportedMuteState = FALSE;
        m_objLastReportedSessionState = (AudioSessionState)0;
        m_guidLastReportedGroupingParam = GUID_NULL;
        m_objLastReportedDisconnectReason = (AudioSessionDisconnectReason)0;

        m_pguidLastDisplayNameChangeContext = NULL;
        m_pguidLastGroupParamChangeContext = NULL;
        m_pguidLastIconPathChangeContext = NULL;
        m_pguidLastSimpleVolumeChangeContext = NULL;
        m_pguidLastChannelVolumeChangeContext = NULL;
    m_EventDataLock.Leave();

    return;
}   

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::SetEventFilterNoReset(LPCGUID)
//
// Description:
//     Set the event context ID filter for this event handler; do not
//     modify event data collectors.
//
// Parameters: N/A
//
// Return values: N/A
//
// ----------------------------------------------------------------------------
VOID CAudioSessionEventHandler::SetEventFilterNoReset(LPCGUID pguidFilter)
{
    m_pguidEventFilter = pguidFilter;
    return;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::_EventFilterTest(LPCGUID)
//
// Description:
//     Determine whether an event should be filtered out, based on its context
//     GUID
//
//     * THIS METHOD SHOULD ONLY EVER BE CALLED WHILE HOLDING m_EventDataLock *
//
// Return values: (BOOL) TRUE if the event should be filtered, FALSE otherwise
//
// ----------------------------------------------------------------------------
inline BOOL CAudioSessionEventHandler::_EventFilterTest(LPCGUID pguidTestContext)
{
    if(NULL == m_pguidEventFilter)
    {
        return FALSE;
    }

    // accept only events that match the filter constraint
    if(!IsEqualGUID(*m_pguidEventFilter, *pguidTestContext))
    {
        SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
        SLOG(eInfo1, "<INCOMING EVENT FILTERED>");
        LogGUID(*pguidTestContext);
        SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
        return TRUE;
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::OnDisplayNameChanged
//
// Description:
//     IAudioSessionControl DisplayNameChanged event handler
//
// Parameters: [in] wszNewDisplayName - the new IAudioSessionControl display name
//             [in] llEventContext - the context id of the event (for event tracking)
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::OnDisplayNameChanged(LPCWSTR wszNewDisplayName,
    LPCGUID pguidEventContext)
{
    HRESULT hr = S_OK;
    size_t strlen = 0;
    WCHAR* wszBuf = NULL;
        
    // null param check
    if(!wszNewDisplayName)
    {
        return E_INVALIDARG;
    }
    
    // get the length of the incoming display name
    hr = StringCchLengthW(wszNewDisplayName, 1024, &strlen);    
    if(FAILED(hr)) {goto CLEANUP;}

    // allocate the new string
    wszBuf = new WCHAR[strlen+1];
    if(NULL == wszBuf)
    {
        return E_OUTOFMEMORY;
    }
       
    // copy the incoming string
    hr = StringCchCopyW(wszBuf, strlen + 1, wszNewDisplayName);
    if(FAILED(hr)) { goto CLEANUP; }
    hr = S_OK;

    // update the received event record
    m_EventDataLock.Enter();
        if(_EventFilterTest(pguidEventContext))
        {
            // filter out the event.
            hr = S_FALSE;
        }
        else
        {
            SAFE_DELETE_ARRAY(m_wszLastReportedDisplayName);
            m_wszLastReportedDisplayName = wszBuf;
            m_pguidLastDisplayNameChangeContext = pguidEventContext;
            m_nDisplayNameChangedEvents++;
        }
    m_EventDataLock.Leave();

    if(S_FALSE == hr) 
    { 
        hr = S_OK;
        goto CLEANUP;
    }

    SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
    SLOG(eInfo1,TEXT("EVENT: DisplayNameChanged (Display Name: %ws)"), wszNewDisplayName);
    LogGUID(*pguidEventContext);
    SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
        
    return hr; // no cleanup necessary in this case.

CLEANUP:
    SAFE_DELETE_ARRAY(wszBuf);
    return hr;    
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::OnIconPathChanged
//
// Description:
//     IAudioSessionControl IconPath change event handler
//
// Parameters: [in] wszIconPath - the new icon path string
//             [in] llEventContext - the context id of the event (for event tracking)
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::OnIconPathChanged(LPCWSTR wszIconPath,
    LPCGUID pguidEventContext)
{
    HRESULT hr = S_OK;
    size_t strlen = 0;
    WCHAR* wszBuf = NULL;

    // null param check
    if(!wszIconPath)
    {
        return E_INVALIDARG;
    }

    // get the length of the incoming display name
    hr = StringCchLengthW(wszIconPath, 1024, &strlen);    
    if(FAILED(hr)) {return hr;}

    // allocate the new string
    wszBuf = new WCHAR[strlen+1];
    if(NULL == wszBuf)
    {
        return E_OUTOFMEMORY;
    }
       
    // copy the incoming string
    hr = StringCchCopyW(wszBuf, strlen + 1, wszIconPath);
    if(FAILED(hr)) { goto CLEANUP; }

    hr = S_OK;

    // update the received event record
    m_EventDataLock.Enter();
        if(_EventFilterTest(pguidEventContext))
        {
            hr = S_FALSE; // filter out the event
        }
        else
        {
            SAFE_DELETE_ARRAY(m_wszLastReportedIconPath);
            m_wszLastReportedIconPath = wszBuf;
            m_pguidLastIconPathChangeContext = pguidEventContext;
            m_nIconPathChangedEvents++;
        }
    m_EventDataLock.Leave();

    if(S_FALSE == hr) 
    { 
        hr = S_OK;
        goto CLEANUP; 
    }

    SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
    SLOG(eInfo1,TEXT("EVENT: IconPathChanged (Icon Path: %ws)"), wszIconPath);
    LogGUID(*pguidEventContext);
    SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");

    return hr;  // No cleanup necessary in this case.

CLEANUP:
    SAFE_DELETE_ARRAY(wszBuf);
    return hr;    
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::OnSimpleVolumeChanged(float fVolume, 
//          BOOL bMuteState)
//
// Description:
//     IAudioSessionControl SimpleVolumeChanged event handler
//
// Parameters: [in] fVolume - the new AudioSession master volume attenuation
//             [in] fVolume - the new AudioSession master volume mute state
//             [in] pguidEventContext - the context id of the event (for event tracking)
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::OnSimpleVolumeChanged(float fVolume , 
    BOOL bMuteState,
    LPCGUID pguidEventContext)
{
    HRESULT hr = S_OK;

    SLOG(eBlab1, "<<<SIMPLE VOLUME CHANGE>>>");
    
    m_EventDataLock.Enter();
        if (_EventFilterTest(pguidEventContext))
        {
            hr = S_FALSE;   // filter out the event
        }
        else
        {
            m_fLastReportedSimpleVolume = fVolume;
            m_bLastReportedMuteState = bMuteState;
            m_pguidLastSimpleVolumeChangeContext = pguidEventContext;
            m_nSimpleVolumeChangedEvents++;
        }
    m_EventDataLock.Leave();


    if (S_FALSE != hr)
    {
        SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
        SLOG(eInfo1,
            TEXT("EVENT: SimpleVolumeChanged (New Volume: %f , New Mute State %d)"),
            fVolume,
            bMuteState);
        LogGUID(*pguidEventContext);
            SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
    }
    
    return S_OK;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::OnChannelVolumeChanged(DWORD ChannelCount, 
//          float NewChannelVolumeArray[  ], DWORD ChangedChannel)
//
// Description:
//     IAudioSessionControl ChannelVolumeChanged event handler
//
// Parameters: [in] dwChannelCount - The number of channels for the AudioSession
//             [in] afChannelVolumeArray - array of size dwChannelCount
//                                         containing the volumes for each chan
//             [in] dwChangedChannel - the channel whose volume change was
//                                     associated with this event
//             [in] pguidEventContext - the context id of the event (for event tracking)
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::OnChannelVolumeChanged(DWORD dwChannelCount, 
    float afChannelVolumeArray[  ], 
    DWORD dwChangedChannel,
    LPCGUID pguidEventContext)
{
    HRESULT hr = S_OK;
    FLOAT* pfVolumeBuff = NULL;

    if(0 >= dwChannelCount)
    {
        SLOG(eFatalError, "ERROR: EVENT: OnChannelVolumeChanged failed with invalid channel count");
        return E_INVALIDARG;
    }

    // allocate memory buffer to store channel volumes
    pfVolumeBuff = new float[dwChannelCount];

    if(NULL == pfVolumeBuff)
    {
        SLOG(eFatalError, "ERROR: Out of memory!");
        return E_OUTOFMEMORY;
    }

    // copy the volume data into the buffer
    for(int i=0;i<(int)(dwChannelCount);i++)
    {
        pfVolumeBuff[i] = afChannelVolumeArray[i];
    }

    hr = S_OK;

    // update the event data
    m_EventDataLock.Enter();
        if(_EventFilterTest(pguidEventContext))
        {
            hr = S_FALSE; // filter event
        }
        else
        {
            SAFE_DELETE_ARRAY(m_pfLastReportedChannelVolumes);
            m_pfLastReportedChannelVolumes = pfVolumeBuff;
            m_dwReportedChannelVolumes = dwChannelCount;
            m_dwLastChangedChannel = dwChangedChannel;
            m_pguidLastChannelVolumeChangeContext = pguidEventContext;
            m_nChannelVolumeChangedEvents++;
        }
    m_EventDataLock.Leave();
    if(S_OK != hr) 
    {  
        SAFE_DELETE_ARRAY(pfVolumeBuff);
        hr = S_OK;
    }
    else
    {
        // log the event
        SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
        SLOG(eInfo1, "EVENT: ChannelVolumeChanged");
        SLOG(eInfo1, "       Channel Count: %d", dwChannelCount);
        for(int i=0;i<(int)(dwChannelCount);i++)
        {
            SLOG(eInfo1, "       Chan %d attenuation: %f", i, afChannelVolumeArray[i]);
        }
        SLOG(eInfo1, "       Changed Channel: %d", dwChangedChannel);
        LogGUID(*pguidEventContext);
        SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
    }

    return hr;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::SessionDisconnected(
//  AudioSessionDisconnectReason objDisconnectReason)
//
// Description:
//     IAudioSessionControl SessionDisconnected event handler
//
// Parameters: [in] objDisconnectReason - The reason for the disconnection
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::OnSessionDisconnected(
    AudioSessionDisconnectReason objDisconnectReason)
{
    m_EventDataLock.Enter();
        m_objLastReportedDisconnectReason = objDisconnectReason;      
        m_nSessionDisconnectedEvents++;
    m_EventDataLock.Leave();

    SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
    SLOG(eInfo1, "EVENT: OnSessionDisconnected");   
    SLOG(eInfo1, "Reason: %s", StringFromDisconnectReason(objDisconnectReason));
    SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
    
    return S_OK;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::StateChanged(AudioSessionState objNewState)
//
// Description:
//     IAudioSessionControl StateChanged event handler
//
// Parameters: [in] objNewState - The new audiosession state
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::OnStateChanged(AudioSessionState objNewState)
{
    m_EventDataLock.Enter();
        m_objLastReportedSessionState = objNewState;
        m_nStateChangedEvents++;
    m_EventDataLock.Leave();

    SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
    SLOG(eInfo1,TEXT("EVENT: OnStateChanged (New State: %d)"), objNewState);
    SLOG(eInfo1, "Context Guid <N/A>");
    SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");

    return S_OK;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::OnGroupingParamChanged(
//  GUID guidNewGroupingParam)
//
// Description:
//     IAudioSessionControl grouping parameter change event handler
//
// Parameters: [in] guidNewGroupingParam - The new grouping parameter
//             [in] pguidEventContext - the context id of the event (for event tracking)
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::OnGroupingParamChanged(LPCGUID pguidNewGroupingParam,
    LPCGUID pguidEventContext)
{
    HRESULT hr = S_OK;

    m_EventDataLock.Enter();
        if(_EventFilterTest(pguidEventContext))
        {
            hr = S_FALSE;   // filter the event
        }
        else
        {
            // Copy the the memory referenced by the GUID pointer
            CopyMemory((void*)&m_guidLastReportedGroupingParam,
                (void*)pguidNewGroupingParam,
                sizeof(GUID));        
            m_pguidLastGroupParamChangeContext = pguidEventContext;
            m_nGroupingParamChangedEvents++;
        }
    m_EventDataLock.Leave();

    if (S_OK == hr)
    {
        SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
        SLOG(eInfo1,TEXT("EVENT: CTestAudioSessionEvents::OnGroupingParamChanged()"));
        LogGUID(*pguidEventContext);
        SLOG(eInfo1, "!!!!!!!!!!!!!!!!!!!!!!!!!");
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::ResetHandler()
//
// Description:
//     Reset the state of the handler
//
// Parameters: N/A
//
// Return values: N/A
//
// ----------------------------------------------------------------------------
void CAudioSessionEventHandler::ResetHandler(void)
{
    m_EventDataLock.Enter();
        m_nDisplayNameChangedEvents = 0;
        m_nIconPathChangedEvents = 0;
        m_nSimpleVolumeChangedEvents = 0;
        m_nChannelVolumeChangedEvents = 0;
        m_nStateChangedEvents = 0;
        m_nGroupingParamChangedEvents = 0;
        m_nSessionDisconnectedEvents = 0;

        m_dwReportedChannelVolumes = 0;
        m_dwLastChangedChannel = 0;

        SAFE_DELETE_ARRAY(m_wszLastReportedDisplayName);
        SAFE_DELETE_ARRAY(m_wszLastReportedIconPath);
        SAFE_DELETE_ARRAY(m_pfLastReportedChannelVolumes);

        m_fLastReportedSimpleVolume = 0.0f;
        m_bLastReportedMuteState = FALSE;
        m_objLastReportedSessionState = (AudioSessionState)0;
        m_guidLastReportedGroupingParam = GUID_NULL;
        m_objLastReportedDisconnectReason = (AudioSessionDisconnectReason)0;

        m_pguidEventFilter = NULL;

        m_pguidLastDisplayNameChangeContext = NULL;
        m_pguidLastGroupParamChangeContext = NULL;
        m_pguidLastIconPathChangeContext = NULL;
        m_pguidLastSimpleVolumeChangeContext = NULL;
        m_pguidLastChannelVolumeChangeContext = NULL;
    m_EventDataLock.Leave();

    return;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::ClearContextIdentifiers()
//
// Description:
//     Reset the event context GUIDs only
//
// Parameters: N/A
//
// Return values: N/A
//
// ----------------------------------------------------------------------------
void CAudioSessionEventHandler::ClearContextIdentifiers(void)
{
    m_EventDataLock.Enter();
        m_pguidLastDisplayNameChangeContext = NULL;
        m_pguidLastGroupParamChangeContext = NULL;
        m_pguidLastIconPathChangeContext = NULL;
        m_pguidLastSimpleVolumeChangeContext = NULL;
        m_pguidLastChannelVolumeChangeContext = NULL; 
    m_EventDataLock.Leave();
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastReportedDisplayName(
//  LPCWSTR wszDisplayName)
//
// Description:
//     Retrieve the last reported display name
//
// Parameters: [out] wszDisplayName -- the display name
//
// Return values (HRESULT): S_OK on success, E_FAIL if no event has been received
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::GetLastReportedDisplayName(LPCWSTR* pwszDisplayName)
{
    HRESULT hr        = S_OK;
    WCHAR*    wszBuf    = NULL;
    size_t    strlen        = 0;
    
    m_EventDataLock.Enter();

        // null param check
        if(NULL == pwszDisplayName)
        {
            hr = E_INVALIDARG;
            goto ERR;
        }    

        // event received check
        if((0 == m_nDisplayNameChangedEvents)||(NULL == m_wszLastReportedDisplayName))
        {
            hr = S_FALSE;
            goto ERR;
        }

        hr = StringCchLengthW(m_wszLastReportedDisplayName,
            1024,
            &strlen);
        if(FAILED(hr)) { goto ERR; }

        wszBuf = new WCHAR[strlen+1];
        if(NULL == wszBuf)
        {
            hr = E_OUTOFMEMORY;
            goto ERR;
        }

        hr = StringCchCopyW(wszBuf, strlen+1, m_wszLastReportedDisplayName);
        if(FAILED(hr)) { goto ERR; }

        *pwszDisplayName = wszBuf;
    m_EventDataLock.Leave();
    
    return hr;

ERR:
    m_EventDataLock.Leave();
    SAFE_DELETE_ARRAY(wszBuf);
    return hr;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastReportedIconPath(
//  LPCWSTR wszIconPath)
//
// Description:
//     Retrieve the last reported icon path
//
// Parameters: [out] wszIconPath -- the icon path string
//
// Return values (HRESULT): S_OK on success, E_FAIL if no event has been received
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::GetLastReportedIconPath(LPCWSTR* pwszIconPath)
{
    HRESULT hr        = S_OK;
    WCHAR*    wszBuf    = NULL;
    size_t    strlen        = 0;
    
    m_EventDataLock.Enter();
        // null param check
        if(NULL == pwszIconPath)
        {
            hr = E_INVALIDARG;
            goto ERR;
        }

        // event received check
        if((0 == m_nIconPathChangedEvents)||(NULL == m_wszLastReportedIconPath))
        {
            hr = S_FALSE;
            goto ERR;
        }

        hr = StringCchLengthW(m_wszLastReportedIconPath,
            1024,
            &strlen);
        if(FAILED(hr)) { goto ERR; }

        wszBuf = new WCHAR[strlen+1];
        if(NULL == wszBuf)
        {
            hr = E_OUTOFMEMORY;
            goto ERR;
        }

        hr = StringCchCopyW(wszBuf, strlen+1, m_wszLastReportedIconPath);
        if(FAILED(hr)) { goto ERR; }

        // actual update happens here
        *pwszIconPath = wszBuf;
    m_EventDataLock.Leave();
    
    return hr;

ERR:
    m_EventDataLock.Leave();
    SAFE_DELETE_ARRAY(wszBuf);
    return hr;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastReportedSimpleVolume(
//  float* pfNewVolume)
//
// Description:
//     Retrieve the last reported audiosession master volume
//
// Parameters: [out] pfNewVolume -- the new audiosession master volume atten.
//
// Return values (HRESULT): S_OK on success, E_FAIL if no event has been received
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::GetLastReportedSimpleVolume(float* pfNewVolume)
{
    HRESULT hr = S_OK;

    if(NULL == pfNewVolume)
    {
        hr = E_INVALIDARG;
        goto END;
    }

    m_EventDataLock.Enter();
        if(0 == m_nSimpleVolumeChangedEvents)
        {
            hr = S_FALSE;
            m_EventDataLock.Leave();
            goto END;
        }

        *pfNewVolume = m_fLastReportedSimpleVolume;    
    m_EventDataLock.Leave();    
    
END:
    return hr;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastReportedChannelVolume
//
// Description:
//     Retrieve the last reported audiosession channel volume settings
//
// Parameters: [out] pfNewVolume -- the new audiosession master volume atten.
//
// Return values (HRESULT): S_OK on success, E_FAIL if no event has been received
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::GetLastReportedChannelVolume(DWORD* pdwChannelCount,
    FLOAT** pafChannelVolumes,
    DWORD* pdwChangedChannel)
{
    HRESULT hr = S_OK;
    FLOAT* pfTempVolumeBuffer = NULL;

    if((NULL == pdwChannelCount)||
       (NULL == pafChannelVolumes)||
       (NULL == pdwChangedChannel))
    {
        hr = E_INVALIDARG;
        goto END;
    }

    m_EventDataLock.Enter();    
        // fail if there were no channel volume changed events
        if(0 == m_nChannelVolumeChangedEvents)
        {
            hr = S_FALSE;
            goto END;
        }

        // allocate a buffer to hold channel volume data
        pfTempVolumeBuffer = new FLOAT[m_dwReportedChannelVolumes];
        if(NULL == pfTempVolumeBuffer)
        {
            SLOG(eFatalError, "Out of memory!");
            hr = E_OUTOFMEMORY;
            goto END;
        }

        // fill the buffer
        for(DWORD i=0;i<m_dwReportedChannelVolumes;i++)
        {
            pfTempVolumeBuffer[i] = m_pfLastReportedChannelVolumes[i];
        }


        *pdwChannelCount = m_dwReportedChannelVolumes;
        *pafChannelVolumes = pfTempVolumeBuffer;
        *pdwChangedChannel = m_dwLastChangedChannel;

END:
    m_EventDataLock.Leave();    
    return hr;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastReportedMuteState(
//  BOOL* pbReportedMuteState)
//
// Description:
//     Retrieve the last reported mute state of the audiosession
//
// Parameters: [out] pbReportedMuteState -- the reported mute state
//
// Return values (HRESULT): S_OK on success, E_FAIL if no event has been received
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::GetLastReportedMuteState(BOOL* pbReportedMuteState)
{
    HRESULT hr = S_OK;

    m_EventDataLock.Enter();
        if(NULL == pbReportedMuteState)
        {
            hr = E_INVALIDARG;
            goto END;
        }
        
        if(0 == m_nSimpleVolumeChangedEvents)
        {
            hr = S_FALSE;
            goto END;
        }

        *pbReportedMuteState = m_bLastReportedMuteState;
END:
    m_EventDataLock.Leave();
    return hr;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastReportedSessionState(
//  AudioSessionState* pobjNewState)
//
// Description:
//     Retrieve the last reported state of the audiosession
//
// Parameters: [out] pobjNewState -- the reported session state
//
// Return values (HRESULT): S_OK on success, E_FAIL if no event has been received
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::GetLastReportedSessionState(
    AudioSessionState* pobjNewState)
{
    HRESULT hr = S_OK;
    
    m_EventDataLock.Enter();

        if(NULL == pobjNewState)
        {
            hr = E_INVALIDARG;
            goto END;
        }

        if(0 == m_nStateChangedEvents)
        {
            hr = S_FALSE;
            goto END;
        }

        *pobjNewState = m_objLastReportedSessionState;        
END:
    m_EventDataLock.Leave();
    return hr;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastReportedGroupingParam(GUID* pguidNewGroupingParam)
//
// Description:
//     Retrieve the last reported grouping param for the audiosession
//
// Parameters: [out] pguidNewGroupingParam -- the reported session class
//
// Return values (HRESULT): S_OK on success, E_FAIL if no event has been received
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::GetLastReportedGroupingParam(GUID* pguidNewGroupingParam)
{
    HRESULT hr = S_OK;

    m_EventDataLock.Enter();

        if(NULL == pguidNewGroupingParam)
        {
            hr = E_INVALIDARG;
            goto END;
        }
        
        if(0 == m_nGroupingParamChangedEvents)
        {
            hr = S_FALSE;
            goto END;
        }       

        CopyMemory((void*)pguidNewGroupingParam,
            (void*)&m_guidLastReportedGroupingParam,
            sizeof(GUID));
END:
    m_EventDataLock.Leave();
    return hr;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastReportedDisconnection
//
// Description:
//     Retrieve the last reported disconnection reason
//
// Parameters: [out] pobjReason -- the reported disconnection reason
//
// Return values (HRESULT): S_OK on success, the appropriate HRESULT on failure
//
// ----------------------------------------------------------------------------
HRESULT CAudioSessionEventHandler::GetLastReportedDisconnection(AudioSessionDisconnectReason* pobjReason)
{
    AudioSessionDisconnectReason temp = (AudioSessionDisconnectReason)(-1);

    if(NULL == pobjReason)
    {
        return E_POINTER;
    }

    m_EventDataLock.Enter();
        temp = m_objLastReportedDisconnectReason;
    m_EventDataLock.Leave();

    *pobjReason = temp;
    
    return S_OK;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetDisplayNameChangedEventCount(VOID)
// Parameters: N/A
//
// Return values: The event count since the last handler reset
//
// ----------------------------------------------------------------------------
UINT CAudioSessionEventHandler::GetDisplayNameChangedEventCount(void)       
{    
    UINT temp = 0;
    
    m_EventDataLock.Enter();
        temp = m_nDisplayNameChangedEvents; 
    m_EventDataLock.Leave();
    
    return temp;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetIconPathChangedEventCount(VOID)
// Parameters: N/A
//
// Return values: The event count since the last handler reset
//
// ----------------------------------------------------------------------------
UINT CAudioSessionEventHandler::GetIconPathChangedEventCount(void)          
{    
    UINT temp = 0;
    
    m_EventDataLock.Enter();
        temp = m_nIconPathChangedEvents; 
    m_EventDataLock.Leave();
    
    return temp;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetVolumeChangedEventCount(VOID)

// Parameters: N/A
//
// Return values: The event count since the last handler reset
//
// ----------------------------------------------------------------------------
UINT CAudioSessionEventHandler::GetVolumeChangedEventCount(void)            
{    
    UINT temp = 0;
    
    m_EventDataLock.Enter();
        temp = m_nSimpleVolumeChangedEvents; 
    m_EventDataLock.Leave();

    return temp;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetChannelVolumeChangedEventCount(VOID)
// Parameters: N/A
//
// Return values: The event count since the last handler reset
//
// ----------------------------------------------------------------------------
UINT CAudioSessionEventHandler::GetChannelVolumeChangedEventCount(void)     
{    
    UINT temp = 0;
    
    m_EventDataLock.Enter();
        temp = m_nChannelVolumeChangedEvents; 
    m_EventDataLock.Leave();

    return temp;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetStateChangedEventCount(VOID)
// Parameters: N/A
//
// Return values: The event count since the last handler reset
//
// ----------------------------------------------------------------------------
UINT CAudioSessionEventHandler::GetStateChangedEventCount(void)             
{    
    UINT temp = 0;
    
    m_EventDataLock.Enter();
        temp = m_nStateChangedEvents; 
    m_EventDataLock.Leave();

    return temp;
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetGroupingParamChangedEventCount(VOID)
// Parameters: N/A
//
// Return values: The event count since the last handler reset
//
// ----------------------------------------------------------------------------
UINT CAudioSessionEventHandler::GetGroupingParamChangedEventCount(void)     
{    
    UINT temp = 0;
    
    m_EventDataLock.Enter();
        temp = m_nGroupingParamChangedEvents; 
    m_EventDataLock.Leave();

    return temp;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetSessionDisconnectedEventCount(VOID)
// Parameters: N/A
//
// Return values: The event count since the last handler reset
//
// ----------------------------------------------------------------------------
UINT CAudioSessionEventHandler::GetSessionDisconnectedEventCount(void)
{
    UINT temp = 0;

    m_EventDataLock.Enter();
        temp = m_nSessionDisconnectedEvents; 
    m_EventDataLock.Leave();

    return temp;    
}

//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastDisplayNameChangeContext(VOID)
// Parameters: N/A
//
// Return values: A pointer to the context guid associated with the last
//                session display name change
//
// ----------------------------------------------------------------------------
LPCGUID CAudioSessionEventHandler::GetLastDisplayNameChangeContext(void)     
{    
    LPCGUID temp = NULL;
    
    m_EventDataLock.Enter();
        temp = m_pguidLastDisplayNameChangeContext;
    m_EventDataLock.Leave();

    return temp;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastIconPathChangeContext(VOID)
// Parameters: N/A
//
// Return values: A pointer to the context guid associated with the last
//                session icon path change
//
// ----------------------------------------------------------------------------
LPCGUID CAudioSessionEventHandler::GetLastIconPathChangeContext(void)     
{    
    LPCGUID temp = NULL;
    
    m_EventDataLock.Enter();
        temp = m_pguidLastIconPathChangeContext;
    m_EventDataLock.Leave();

    return temp;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetSimpleVolumeChangeContext(VOID)
// Parameters: N/A
//
// Return values: A pointer to the context guid associated with the last
//                session simple volume change
//
// ----------------------------------------------------------------------------
LPCGUID CAudioSessionEventHandler::GetLastSimpleVolumeChangeContext(void)     
{    
    LPCGUID temp = NULL;
    
    m_EventDataLock.Enter();
        temp = m_pguidLastSimpleVolumeChangeContext;
    m_EventDataLock.Leave();

    return temp;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastChannelVolumeChangeContext(VOID)
// Parameters: N/A
//
// Return values: A pointer to the context guid associated with the last
//                session channel volume change
//
// ----------------------------------------------------------------------------
LPCGUID CAudioSessionEventHandler::GetLastChannelVolumeChangeContext(void)     
{    
    LPCGUID temp = NULL;
    
    m_EventDataLock.Enter();
        temp = m_pguidLastChannelVolumeChangeContext;
    m_EventDataLock.Leave();

    return temp;
}


//-----------------------------------------------------------------------------
// Method: CAudioSessionEventHandler::GetLastGroupingParamChangeContext(VOID)
// Parameters: N/A
//
// Return values: A pointer to the context guid associated with the last
//                session grouping parameter change
//
// ----------------------------------------------------------------------------
LPCGUID CAudioSessionEventHandler::GetLastGroupingParamChangeContext(void)     
{    
    LPCGUID temp = NULL;
    
    m_EventDataLock.Enter();
        temp = m_pguidLastGroupParamChangeContext;
    m_EventDataLock.Leave();

    return temp;
}

