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
#pragma once

typedef struct _tag_tm
{
    TIMEPROC    Proc;
    LPCSTR      strRep;
} TIMER_MECHANISM, *PTIMER_MECHANISM;

class CHalfApp
{
public:
    STACKWISE_DATAFLOW          m_flow;
    CAutoPtr<CKsFilter>         m_pFilter;
    CPCMAudioPin*               m_pPin;
    FORMAT_ENTRY                m_FmtCurrent;
    const PFORMAT_ENTRY         m_pFmtCurrent;
    GUID                        m_Mode;

    PFORMAT_ENTRY       m_rgFmts;
    UINT_PTR            m_cFmts;

    CHalfApp(void)
    : m_flow(render)
    , m_pPin(NULL)
    , m_pFmtCurrent(&m_FmtCurrent)
    , m_fIsWaveRTPin(false)
    , m_fIsWaveRTPinPacketSupport(false)
    , m_fIsStdStrmPin(false)
    , m_fIsStdLoopedStrmPin(false)
    , m_fIsKnownNonPortclsDriver(false)
    {
        memset(m_pFmtCurrent, 0, sizeof(FORMAT_ENTRY));
        m_Mode = GUID_NULL;
    }

    ~CHalfApp(void)
    {}

    BOOL    Initialize(TTList<CKsFilter>* plistFilters);
    BOOL    GenerateFormatEntries();
    BOOL    PreparePin(BOOL fLooped, BOOL RTPort = FALSE, OPTIONAL ULONG ulBufferTime = BUFLEN_MS);
    BOOL    PreparePin(WAVEFORMATEX *pWfxIn = NULL, BOOL fLooped = 0, BOOL RTPort = FALSE, OPTIONAL ULONG ulBufferTime = BUFLEN_MS, OPTIONAL BOOL bRawSupport  = false);
    BOOL    PrepareBuffer(BOOL RTPort = FALSE, OPTIONAL ULONG ulBufferTime = BUFLEN_MS);
    bool    IsPropertySupported(IN ULONG nProperty, IN DWORD dwSupport);
    bool    IsBTHPin();
    bool    IsWaveRTPacketStreamingSupported();
    bool    IsTestSupportedForDeviceType(BOOL fTestingLoopedDevice);

    bool    IsWaveRTPin() { return m_fIsWaveRTPin; }
    bool    m_fIsWaveRTPin;             // true if the current pin is WaveRT
    bool    m_fIsWaveRTPinPacketSupport;    // true if the current pin is packet based WaveRT support.
    bool    m_fIsStdStrmPin;            // true if the current pin supports KSINTERFACE_STANDARD_STREAMING
    bool    m_fIsStdLoopedStrmPin;      // true if the current pin supports KSINTERFACE_STANDARD_LOOPED_STREAMING
    bool    m_fIsKnownNonPortclsDriver; // true if the current pin is installed from a known portcls driver
};

// {571BF784-A9D6-420A-BCA0-C579A8710236}
DEFINE_GUID(IID_IHalfAppContainer, 
0x571bf784, 0xa9d6, 0x420a, 0xbc, 0xa0, 0xc5, 0x79, 0xa8, 071, 0x02, 0x36);

DECLARE_INTERFACE_IID_(
    IHalfAppContainer,
    IUnknown,
    "571BF784-A9D6-420A-BCA0-C579A8710236")
{
    STDMETHOD (GetHalfApp)(THIS_ CHalfApp ** ppHalfApp) PURE;
};
