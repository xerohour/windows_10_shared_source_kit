#pragma once

typedef enum _WFC_TIMER_REGISTRATION_STATE
{
    TimerRegistrationStateInvalid,
    TimerRegistrationStateInit,
    TimerRegistrationStateTimerSet,
    TimerRegistrationStateDone
} WFC_TIMER_REGISTRATION_STATE;

class EventQueue;

class TimerRegistrationContext : ITimerCallback,
                                 IActivityId
{
public:
    TimerRegistrationContext(
        _In_ WFC_ACTIVITY_ID ActivityId );
    ~TimerRegistrationContext();

    NDIS_STATUS Initialize(
        _In_ EventQueue* pEventQueue,
        _In_ CGlobalCtlDevice* pCtlDevice,
        _In_ ITimerCallback* pTimerCallback,
        _In_ ULONG RelativeTimeoutMs,
        _In_opt_ WFC_TIMER_CONTEXT TimerContext,
        _In_ WDF_EXECUTION_LEVEL level,
        _In_ bool bReuseTimer,
        _Out_ WFC_TIMER_REGISTRATION_TOKEN* pRegistrationToken );

    void StopTimer( bool fromUnregister);
    
    NDIS_STATUS StartTimer( 
        _In_ ULONG RelativeTimeoutMs );

    void OnTimerCallback(
        _In_ WFC_TIMER_CONTEXT TimerRegistrationContext );

    PLIST_ENTRY get_ListEntry();

    static TimerRegistrationContext* get_FromListEntry(
        _In_ PLIST_ENTRY pListEntry );

    void IssueCallback();

    WFC_TIMER_REGISTRATION_TOKEN get_RegistrationToken();
    void set_RegistrationToken(
        _In_ WFC_TIMER_REGISTRATION_TOKEN RegistrationToken );

    bool IsReuseTimer() { return m_bReuseTimer;};
        
protected:
    static WFC_TIMER_REGISTRATION_TOKEN get_NextRegistrationToken();

    ULONG                        m_RelativeTimeoutMs;

    CGlobalCtlDevice*            m_pCtlDevice;
    ITimerCallback*              m_pTimerCallback;
    WFC_TIMER_CONTEXT            m_TimerContext;

    WFC_TIMER_REGISTRATION_TOKEN m_RegistrationToken;
    WFC_TIMER_REGISTRATION_STATE m_State;
    ITimerContext*               m_pTimer;
    CPP_LIST_ENTRY               m_Link;
    bool                         m_bReuseTimer;

    EventQueue*                  m_pEventQueue;
    Event                        m_FailsafeTimerFiredEvent;
};
