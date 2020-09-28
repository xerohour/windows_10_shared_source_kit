// Copyright(C) Microsoft. All rights reserved.

//------------------------------------------------------------------------------
// File: SessionManagerEventHandler.h
//
// Desc: Inteface definition for Audio Session arrival event handler
//------------------------------------------------------------------------------

#pragma once

class CSessionManagerEventHandler : public IAudioSessionNotification
{
public:
    CSessionManagerEventHandler();
    ~CSessionManagerEventHandler();

    //  IUnknown
    STDMETHODIMP                            QueryInterface(REFIID  iid, void **ppv);
    STDMETHODIMP_(ULONG)                    AddRef(void);
    STDMETHODIMP_(ULONG)                    Release(void);

    // IAudioSessionNotification interface
    STDMETHODIMP                            OnSessionCreated(IAudioSessionControl *SessionControl);

    // Event data accessor methods
    BOOL                                    ErrorReported();
    UINT32                                  GetSessionCount();
    
    IAudioSessionControl*                   GetAudioSession(DWORD SessionIndex);
    IAudioSessionControl*                   GetAudioSession(LPCWSTR SessionId);
    IAudioSessionControl*                   GetAudioSessionByPID(DWORD dwPID);    

private:
    IAudioSessionManager2*                  m_AudioSessionManager2;
    IAudioSessionManager*                   m_AudioSessionManagerPublic;

    CInterfaceArray<IAudioSessionControl>   m_SessionList;

    CCriticalSection                        m_SessionListLock;
    CCriticalSection                        m_ErrorLock;

    LONG                                    m_cRef;
    BOOL                                    m_bErrorReported;

    VOID                                    _FlagError();  
};
