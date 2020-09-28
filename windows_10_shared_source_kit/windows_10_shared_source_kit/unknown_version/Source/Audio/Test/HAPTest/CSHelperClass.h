// Copyright(C) Microsoft.All rights reserved.

// CSHelperClass.h


HRESULT IsEndpointUnderTestDigitalDisplay(IBasicLog* pLogger, IMMDevice* pMMDevice, BOOL& bIsDigital);


class CSHelperClass
{
public:
    CSHelperClass(IBasicLog* pLogger);
    ~CSHelperClass();
    BOOL    EnterConnectedStandby(IBasicLog* pLogger);
    BOOL    ExitConnectedStandby(IBasicLog* pLogger);

protected:
    struct PowerNotifyStruct
    {
        const GUID* pGUID;
        HPOWERNOTIFY hPowerNotify;
    };

    DWORD RegisterForPowerBroadcasts(IBasicLog* pLogger);
    VOID UnregisterForPowerBroadcasts();
    LPCTSTR GetPowerGuidName( GUID powerGuid );
    ULONG DoPowerNotificationCallback( _In_ ULONG Type, _In_ PVOID Setting );
    static ULONG PowerNotificationCallback( _In_opt_ PVOID Context, _In_ ULONG Type, _In_ PVOID Setting );

    PowerNotifyStruct m_PowerNotifyStruct[2];

    HANDLE m_heScreenOn;
    HANDLE m_heScreenOff;
    HANDLE m_heLowPowerEpochEnter;
    HANDLE m_heLowPowerEpochExit;

    CComPtr<IBasicLog> pLogger;
};

