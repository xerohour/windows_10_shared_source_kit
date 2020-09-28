//
// Copyright(C) Microsoft.All rights reserved.
//
// TestFunctions.h
//
// Description:
//
//  Test function prototypes for Lullaby Test
//

#pragma once

const DWORD    WAIT_MULTIPLIER          = 2;

class CTestCase
{
public:
    WCHAR                    m_szTestName[128];
    int                     m_nSynchronous;
    int                     m_nTestType;
    DWORD                   m_dwResPre;
    DWORD                   m_dwResPost;
    UINT                    m_nSleep;
    UINT                    m_nSnooze;
    SYSTEM_POWER_STATE      m_spsState;
    ApiEnum                 m_api;
    BOOL m_bIsCSTest;

public:
    CTestCase( int nSynchronous, int nTestType, SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api)
      : m_nSynchronous(nSynchronous), m_dwResPre(FNS_NOTIMPLEMENTED), m_dwResPost(FNS_NOTIMPLEMENTED), m_nTestType(nTestType),
      m_nSleep(nSleep), m_nSnooze(nSnooze), m_spsState(spsState), m_api(api), m_bIsCSTest(FALSE)
    {
        wcscpy_s(m_szTestName, sizeof(CHAR) * 10, L"No Test");
    }

    CTestCase(int nSynchronous, int nTestType, ApiEnum api)
        : m_nSynchronous(nSynchronous), m_dwResPre(FNS_NOTIMPLEMENTED), m_dwResPost(FNS_NOTIMPLEMENTED), m_nTestType(nTestType),
        m_nSleep(0), m_nSnooze(0), m_spsState(PowerSystemUnspecified), m_api(api), m_bIsCSTest(TRUE)
    {
        wcscpy_s(m_szTestName, sizeof(CHAR) * 10, L"No Test");
    }

    ~CTestCase( void )
    {
        return;
    }

    virtual BOOL    Initialize( void );
    virtual void    CleanUp ( void );
    virtual DWORD   RunPreTest( void );
    virtual DWORD   RunPostTest( void );
    virtual DWORD   RunDuringTest( INT nSyncronous );
};

class CRenderTest : public CTestCase
{
public:
   IAudioStreamingRender*  m_pStreaming;
   BOOL m_bOffload;

public:
    CRenderTest( int nSynchronous, SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api, BOOL bOffload);
    CRenderTest( int nSynchronous, ApiEnum api, BOOL bOffload);

    ~CRenderTest( void );

    BOOL    Initialize( void );
    void    CleanUp ( void );
    DWORD   RunPreTest( void );
    DWORD   RunPostTest( void );

private: 
    HRESULT CreateRendererForDUT();
};

class CCaptureTest : public CTestCase
{
public:
    IAudioStreamingCapture*         m_pCapture;
    IAudioStreamingControl*         m_pAudioStreamingControl;
    IAudioStreamingAsyncCallback*   m_pCallback;
    HANDLE                          m_heStreamingCompletionEvent;
    BYTE*                           m_pbBuffer;

public:
    CCaptureTest( int nSynchronous, SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api);
    CCaptureTest (int nSynchronous, ApiEnum api);
    ~CCaptureTest( void );

    BOOL    Initialize( void );
    void    CleanUp ( void );
    DWORD   RunPreTest( void );
    DWORD   RunPostTest( void );

private:
    HRESULT CreateCapturerForDUT();
};

class CCSBaseTest
{
public:
    CCSBaseTest();
    CCSBaseTest(BOOL bGLitchTest);
    ~CCSBaseTest();
    BOOL    Initialize();
    void    CleanUp ();
    BOOL    EnterConnectedStandby();
    BOOL    ExitConnectedStandby();
    DWORD   RunDuringTest(INT nSyncronous);
    HRESULT SetupAndStartTracing(_Outptr_result_nullonfailure_ LPWSTR* ppwszTraceName);
    HRESULT StopProcessAndCleanupTracing(LPCWSTR pwszTraceName);

protected:
    struct PowerNotifyStruct
    {
        const GUID* pGUID;
        HPOWERNOTIFY hPowerNotify;
    };

    DWORD RegisterForPowerBroadcasts();
    DWORD UnregisterForPowerBroadcasts();
    LPCTSTR GetPowerGuidName( GUID powerGuid );
    ULONG DoPowerNotificationCallback( _In_ ULONG Type, _In_ PVOID Setting );
    static ULONG PowerNotificationCallback( _In_opt_ PVOID Context, _In_ ULONG Type, _In_ PVOID Setting );

    BOOL SendKeyInput();

    PowerNotifyStruct m_PowerNotifyStruct[2];

    HANDLE m_heScreenOn;
    HANDLE m_heScreenOff;
    HANDLE m_heLowPowerEpochEnter;
    HANDLE m_heLowPowerEpochExit;
    BOOL   m_bGlitchTest;
    CComHeapPtr<WCHAR> m_pwszGlitchFileName;
};

class CCSRenderTest : public CRenderTest, public CCSBaseTest
{ 
public:
    CCSRenderTest(int nSynchronous, ApiEnum api, BOOL bOffload, BOOL bGlitchTest);
    ~CCSRenderTest( void );

    BOOL    Initialize();
    void    CleanUp ();
    DWORD   RunPreTest();
    DWORD   RunPostTest();
};

class CCSCaptureTest : public CCaptureTest, public CCSBaseTest
{
public:
    CCSCaptureTest(int nSynchronous, ApiEnum api, BOOL bGlitchTest);
    ~CCSCaptureTest( void );

    BOOL    Initialize();
    void    CleanUp ();
    DWORD   RunPreTest();
    DWORD   RunPostTest();
};

//helper functions
HRESULT IsEndpointUnderTestDigitalDisplay(BOOL& bIsDigital);
BOOL CheckForDeviceAvailable();

//test cases
DWORD TC_WASAPI_Render_Sync_S3();
DWORD TC_WASAPI_Render_Sync_SH();
DWORD TC_WASAPI_Render_Offload_Sync_S3();
DWORD TC_WASAPI_Render_Offload_Sync_SH();
DWORD TC_Render_Sync(SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api, BOOL bOffload);

DWORD TC_WASAPI_Render_Async_S3();
DWORD TC_WASAPI_Render_Async_SH();
DWORD TC_WASAPI_Render_Offload_Async_S3();
DWORD TC_WASAPI_Render_Offload_Async_SH();
DWORD TC_Render_Async(SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api, BOOL bOffload);

DWORD TC_WASAPI_Capture_Sync_S3();
DWORD TC_WASAPI_Capture_Sync_SH();
DWORD TC_Capture_Sync(SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api);

DWORD TC_WASAPI_Capture_Async_S3();
DWORD TC_WASAPI_Capture_Async_SH();
DWORD TC_Capture_Async(SYSTEM_POWER_STATE spsState, UINT nSleep, UINT nSnooze, ApiEnum api);

DWORD TC_WASAPI_Render_Sync_CS();
DWORD TC_WASAPI_Render_Offload_Sync_CS();

DWORD TC_WASAPI_Render_Async_CS();
DWORD TC_WASAPI_Render_Offload_Async_CS();
DWORD TC_WASAPI_Render_Async_CS_Glitch();
DWORD TC_WASAPI_Render_Offload_Async_CS_Glitch();

DWORD TC_WASAPI_Capture_Sync_CS();
DWORD TC_WASAPI_Capture_Async_CS();
DWORD TC_WASAPI_Capture_Async_CS_Glitch();

DWORD TC_Verify_BTH();
DWORD TC_Verify_SleepResume();
