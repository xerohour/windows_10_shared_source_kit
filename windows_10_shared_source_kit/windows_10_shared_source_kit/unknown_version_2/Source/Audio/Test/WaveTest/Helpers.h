// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// Module Name:
//
//  Helpers.h
//
// Abstract:
//
//  Header for common helpers/defines
//



// -------------------------------------------------------------------------------
#pragma once

#define S_A static_cast<LPCSTR>
#define S_W static_cast<LPCWSTR>

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

#define DATA_POINTS     100
#define BUFLEN_MS       2000
#define USB_BUFLEN_MS   200

// ------------------------------------------------------------------------------
typedef double (*TIMEPROC) (void);

typedef struct _tag_ksi
{
    LPGUID      arpguidCategories[2];
    ULONG       cpguidCategories;
    LPCSTR      strRep;
} FILTER_TYPE, *PFILTER_TYPE;

typedef enum { eNotSupported, eNoIntersection, eIntersection } EInterResult;

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

// ----------------------------------------------------------
// ----------------------------------------------------------
BOOL
CreatePin_WAVEFORMATEXTENSIBLE
(
    CKsPin                 *pKsPin,
    CKsPin                 *pKsPinConnectTo,    // Sink pin to connect.  Non-NULL implies that we are creating a source pin.
    PWAVEFORMATEXTENSIBLE   pWFX,
    BOOL                    fLooped,
    REFGUID                 guidMode
);

BOOL 
InstantiatePinWfx
(
    CPCMAudioPin       *pPin,
    WAVEFORMATEX       *pWfx,
    BOOL                fLooped,
    OPTIONAL BOOL       bRawSupport = 0,
    OPTIONAL BOOL       bDefaultSupport = 0
);

double  tpTGT (void);
double  tpQPC (void);
#ifndef _WIN64
double  tpCCC (void);
INT     GetCPUSpeed(void);
#endif

USHORT  BitsSetInDWORD(DWORD dw);
WORD    BitDepthFromMask(DWORD dw);
ULONG   SampleRateFromMask(DWORD dw);
ULONG   SpeakerMaskFromMask(DWORD dw);

BOOL    IsPinRTPort(CPCMAudioPin*  m_pPin);

// ----------------------------------------------------------
typedef struct _kspin_connect_waveformat_extensible
{
    KSPIN_CONNECT                           Connect;
    KSDATAFORMAT                            DataFormat;
    WAVEFORMATEXTENSIBLE                    WaveFormat;
    // The below are valid only if DataFormat has the KSDATAFORMAT_ATTRIBUTES flag set
    KSMULTIPLE_ITEM                         MultipleItem;
    KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE  ModeAttribute;
} KSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE, *PKSPIN_CONNECT_WAVEFORMAT_EXTENSIBLE;

typedef struct {
    WORD   Channels;
    WORD   BitsPerSample;
    DWORD   SampleFrequency;
} DataRangeInfo;

#define DisplayGUID(g)	g.Data1, g.Data2,g.Data3,g.Data4[0],g.Data4[1],g.Data4[2],g.Data4[3],g.Data4[4],g.Data4[5],g.Data4[6],g.Data4[7]
#define MSB_FORMAT          WAVE_FORMAT_96S16
inline double dabs(double a)
{
	if (a < 0.0)
		return -a;
	else
		return a;
}

inline int IsEqualGUIDAligned(REFGUID guid1, REFGUID guid2)
{
    return ((*(PLONGLONG)(&guid1) == *(PLONGLONG)(&guid2)) && (*((PLONGLONG)(&guid1) + 1) == *((PLONGLONG)(&guid2) + 1)));
}

#define FILTERTYPE_SYSAUDIO 0xffffffff

#define STREAMING_EVENT_TOLERANCE 1.1
