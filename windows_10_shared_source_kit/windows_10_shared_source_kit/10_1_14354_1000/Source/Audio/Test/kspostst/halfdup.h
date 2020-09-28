// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// File Name:
//
//  HalfDup.h
//
// Abstract:
//
//  CHalfApp declarations
//



// -------------------------------------------------------------------------------

#ifndef _HALFDUP_H_
#define _HALFDUP_H_

#define DATA_POINTS     100
#define BUFLEN_MS       2000
#define USB_BUFLEN_MS   220     //USB Buffers can not exceed 250ms

#define FILTERTYPE_SYSAUDIO 0xffffffff

typedef double (*TIMEPROC) (void);
double  tpQPC (void);

typedef struct _tag_tm
{
    TIMEPROC    Proc;
    LPCSTR      strRep;
} TIMER_MECHANISM, *PTIMER_MECHANISM;

class CThreadPriorityChange
{
public:
    CThreadPriorityChange(int nNewPriority, BOOL *bSuccess)
    {
        hThread = GetCurrentThread();
        nPreviousPriority = GetThreadPriority(hThread);
        *bSuccess = SetThreadPriority(hThread, nNewPriority);
    }
    ~CThreadPriorityChange()
    {
        SetThreadPriority(hThread, nPreviousPriority);
    }
private:
    HANDLE  hThread;
    int     nPreviousPriority;
};

enum STACKWISE_DATAFLOW {render, capture};

// ------------------------------------------------------------------------------
// structs used to hold results of perf tests
typedef struct _t_statdump
{
    double  b;
    double  s;
    double  t;
} STAT_DUMP, *PSTAT_DUMP;

typedef struct
{
     double dTime;              // ms
     double dPos;           // we will store these as byte positions, NOT samples
} POSITION_SET, *PPOSITION_SET;

typedef struct
{
    PPOSITION_SET   argPosSets;
    ULONG           cPosSets;
    ULONG           ctLatencyMeas;
    ULONG           csBufferMeas;
    double          dSampleRate;    // measured (in bytes/sec, NOT samples/sec)
    double          dOffset;        // measured
    STAT_DUMP       sdMax;
    STAT_DUMP       sdStdDev;
} PERF_RESULTS, *PPERF_RESULTS;

typedef struct
{
    WAVEFORMATEXTENSIBLE    wfxFormat;
    BOOL                    fPerfDataCollected; // set when the entry is populated
    PERF_RESULTS            perfResults;        // 
} FORMAT_ENTRY, *PFORMAT_ENTRY;

class CHalfApp
{
public:
    STACKWISE_DATAFLOW          m_flow;
    CAutoPtr<CKsFilter>         m_pFilter;
    CPCMAudioPin*               m_pPin;
    FORMAT_ENTRY                m_FmtCurrent;
    const PFORMAT_ENTRY         m_pFmtCurrent;

    CHalfApp(void)
    : m_flow(render)
    , m_pPin(NULL)
    , m_pFmtCurrent(&m_FmtCurrent)
    , m_fIsWaveRTPin(false)
    , m_fIsWaveRTPinWithReadPacketSupport(false)
    , m_fIsWaveRTPinWithWritePacketSupport(false)
    , m_fIsStdStrmPin(false)
    , m_fIsStdLoopedStrmPin(false)
    {
        memset(m_pFmtCurrent, 0, sizeof(FORMAT_ENTRY));
    }

    ~CHalfApp(void)
    {}

    BOOL    PreparePin(WAVEFORMATEX *pwfx, BOOL fTestingLoopedDevice, OPTIONAL ULONG ulBufferTime = BUFLEN_MS, OPTIONAL BOOL bRawSupport = FALSE, OPTIONAL ULONG ulNotificationCount = 0);
    BOOL    PreparePinConsiderUSB(WAVEFORMATEX *pwfx, BOOL fTestingLoopedDevice, OPTIONAL BOOL bRawSupport);
    bool    IsUSBPin();
    bool    IsBTHPin();
    bool    IsKWDPin();
    bool    IsWaveRTPacketStreamingSupported();
    HRESULT FindInstanceIDInDeviceSet(ITestDeviceSet *pDeviceSet, LPCWSTR szInstanceID, bool &fFound, bool &isBth);
    HRESULT GetCurrentEndpointsUnderTest(IMMDeviceCollection **ppEndpointCollection);
    bool    IsTestSupportedForDeviceType(BOOL fTestingLoopedDevice);

    bool    IsWaveRTPin() { return m_fIsWaveRTPin; }
    bool    m_fIsWaveRTPin;                         // true if the current pin is WaveRT
    bool    m_fIsWaveRTPinWithReadPacketSupport;    // true if the current pin is WaveRT and support read packets. Valid only after PreparePin.
    bool    m_fIsWaveRTPinWithWritePacketSupport;   // true if the current pin is WaveRT and support write packets. Valid only after PreparePin.
    bool    m_fIsStdStrmPin;                        // true if the current pin supports KSINTERFACE_STANDARD_STREAMING
    bool    m_fIsStdLoopedStrmPin;                  // true if the current pin supports KSINTERFACE_STANDARD_LOOPED_STREAMING
};

// {7C9E248B-0358-4515-A86D-13F27C0CCF2D}
DEFINE_GUID(IID_IHalfAppContainer, 
0x7c9e248b, 0x358, 0x4515, 0xa8, 0x6d, 0x13, 0xf2, 0x7c, 0xc, 0xcf, 0x2d);

DECLARE_INTERFACE_IID_(
    IHalfAppContainer,
    IUnknown,
    "7C9E248B-0358-4515-A86D-13F27C0CCF2D")
{
    STDMETHOD (GetHalfApp)(THIS_ CHalfApp ** ppHalfApp) PURE;
};

BOOL InstantiatePinWfx(CPCMAudioPin* pPin, WAVEFORMATEX * pWfx, BOOL fLooped, OPTIONAL BOOL bRawSupport = 0, OPTIONAL BOOL bDefaultSupport = 0);

#endif
