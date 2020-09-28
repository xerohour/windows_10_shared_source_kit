// Copyright (C) Microsoft. All rights reserved.
//  Misc. Stuff
extern const ULONG HNSTIME1MSEC;

typedef struct 
{
    //  RateConvert
    FLOAT32             Rate_fInputFrameRate;
    FLOAT32             Rate_fOutputFrameRate;
    FLOAT32             Rate_fMinInputFrameRate;
    FLOAT32             Rate_fMaxInputFrameRate;
    FLOAT32             Rate_fMinOutputFrameRate;
    FLOAT32             Rate_fMaxOutputFrameRate;
    SRC_ALGORITHM_TYPE  Rate_eAlgorithmType;
    AUDIO_FLOW_TYPE     Rate_eFlowType;
    UINT32              Rate_u32SamplesPerFrame;
    UINT32              Rate_u32HighQualityTaplength;
    
    //  Volume
    UINT32              Volume_u32SamplesPerFrame;
    FLOAT32             Volume_f32FrameRate;
    
    //  Matrix
    UINT32              Matrix_u32InputSamplesPerFrame;
    UINT32              Matrix_u32OutputSamplesPerFrame;
    FLOAT32             Matrix_f32FrameRate;
    
    //  Meter
    UINT32              Meter_u32SamplesPerFrame;
    UINT32              Meter_u32FramesPerAverage;
    UINT32              Meter_u32NumAverages;
    UINT32              Meter_u32FramesPerQuantum;
    
    //  Limiter
    UINT32              Limiter_u32SamplesPerFrame;
    FLOAT32             Limiter_f32FrameRate;
    BOOL                Limiter_bInDevicePipe;
    
//    //  General
//    FLOAT32             f32MinInFramesPerSecond;
//    FLOAT32             f32MaxInFramesPerSecond;
//    FLOAT32             f32MinOutFramesPerSecond;
//    FLOAT32             f32MaxOutFramesPerSecond;
//    
//    UINT32              u32MinInSamplesPerSecond;
//    UINT32              u32MaxInSamplesPerSecond;
//    UINT32              u32MinOutSamplesPerSecond;
//    UINT32              u32MaxOutSamplesPerSecond;
} APOLIMITS;

BOOL InRangeScalar
(
    float       fValue,
    float       fCompare,
    float       fTolerance = 0.001
);

class CCTMString
{
    public:
        CCTMString() { m_pstrString = NULL; }
        CCTMString(__inout_opt LPWSTR pstrString);
        ~CCTMString();
        LPWSTR *operator&() { return (&m_pstrString); }
        LPWSTR  operator=(__inout_opt LPWSTR pstrString);
        LPWSTR  operator=(__inout_opt LPSTR pstrString);
        operator LPWSTR() { return (m_pstrString); }
        void JumbleCase();
        
    private:
        LPWSTR  m_pstrString;
};

class CRegKeyClean
{
    public:
        CRegKeyClean() { m_hKey = NULL; }
        CRegKeyClean(HKEY hKey) { m_hKey = hKey; }
        ~CRegKeyClean() { CloseKey(); }
        void AssignKey(HKEY hKey) { CloseKey(); m_hKey = hKey; }

    private:
        void CloseKey() { if (NULL != m_hKey) { RegCloseKey(m_hKey); m_hKey = NULL; } }
        HKEY    m_hKey;
};

class CTestFormats
{
    public:
        CTestFormats();
        ~CTestFormats() {};
        UINT GetCount() { return (m_cFormats + 1); }
        UNCOMPRESSEDAUDIOFORMAT *GetFormat(UINT iFormat);
        HRESULT GetAMTFormat(UINT iFormat, IAudioMediaType **ppAMTFormat);
        DWORD GetFormatFlags(UINT iFormat);
        void GetFormatString(UINT iFormat, __out_ecount(256) LPSTR pszFormat);
        
    private:
        UINT    m_cFormats;
};

class CAudioBuffer
{
    public:
        CAudioBuffer();
        CAudioBuffer(IAudioMediaType *pFormat);
        ~CAudioBuffer();
        void SetFormat(IAudioMediaType *pFormat);
        BOOL GenerateBuffer(float fSeconds);
        BOOL GenerateOffsetBuffer(float fSeconds);
        BOOL AllocateBuffer(float fSeconds);
        UINT GetBufferSize() { return (m_cbData); }
        LPVOID GetAudioBuffer() { return (m_pData); }
        IAudioMediaType *GetFormat() { return (m_pFormat); }
    
    private:
        LPVOID              m_pData;
        UINT                m_cbData;
        IAudioMediaType    *m_pFormat;
};

class CBufferCollection
{
    public:
        CBufferCollection(UINT cBuffers);
        ~CBufferCollection();
        BOOL IsValid() { return (m_fIsValid); }
        void SetBuffer(UINT iBuffer, CAudioBuffer *pBuffer);
        CAudioBuffer *GetBuffer(UINT iBuffer) { return (m_ppBuffers[iBuffer]); }

    private:
        BOOL                m_fIsValid;
        CAudioBuffer      **m_ppBuffers;
        UINT                m_cBuffers;
};

HRESULT ApplySilenceFlag
(
    APO_CONNECTION_DESCRIPTOR   *pOutputConnection,
    APO_CONNECTION_PROPERTY     *pOutputProperty
);

void Log_ApoRegProperties
(
    PAPO_REG_PROPERTIES     pApoRegProperties
);

void APOTest_Initialize
(
    GUID        guidClassId,
    UINT32      cbAPOInit,
    BYTE       *pAPOInit,
    LPCSTR      pszLabel,
    HRESULT     hrExpected,
    LPCSTR      pszHRLabel
);

UINT GetEndpointFriendlyNameW
(
    IMMDevice                      *pIEndpoint,
    __out_ecount(cchName) LPWSTR    pszName,
    UINT                            cchName
);

UINT GetEndpointFriendlyNameA
(
    IMMDevice                      *pIEndpoint,
    __out_ecount(cchName) LPSTR     pszName,
    UINT                            cchName
);

void GetFormatString
(
    UNCOMPRESSEDAUDIOFORMAT    *pFormat,
    __out_ecount(100) LPSTR     pszFormat
);

LPSTR GetFormatString
(
    IAudioMediaType            *pAMTFormat,
    __out_ecount(256) LPSTR     pszName
);

BOOL IsEqualFormat
(
    IAudioMediaType    *pAMTFormat1,
    IAudioMediaType    *pAMTFormat2
);

BOOL VerifyFormatFill
(
    IAudioMediaType    *pAMTFormat
);

KSDATAFORMAT_WAVEFORMATEX *CreateKSDataFromWFX
(
    WAVEFORMATEX               *pwfx
);

class CFileStream
{
    public:
        CFileStream();
        CFileStream(LPCSTR pszFileName, DWORD dwAccess, DWORD dwShare, DWORD dwDisposition, DWORD dwFlags);
        HANDLE OpenFile(LPCSTR pszFileName, DWORD dwAccess, DWORD dwShare, DWORD dwDisposition, DWORD dwFlags);
        ~CFileStream();
        HANDLE GetFileHandle() { return m_hFile; }

    private:
        HANDLE      m_hFile;
};

class CAutoBuffer
{
    public:
        CAutoBuffer();
        CAutoBuffer(UINT cbAlloc);
        LPVOID Allocate(UINT cbAlloc);
        ~CAutoBuffer();
        UINT GetBufferSize() { return m_cbData; }
        LPVOID GetBuffer() { return (LPVOID)(m_pData); }
    
    private:
        UINT        m_cbData;
        LPBYTE      m_pData;
};

class CCTMAutoFree
{
    public:
        CCTMAutoFree(LPVOID pBuffer);
        ~CCTMAutoFree();
        
    private:
        LPVOID  m_pBuffer;
};

BOOL FileCompare
(
    LPCSTR      pszFileName1,
    LPCSTR      pszFileName2
);

#define TestAPO_Initialize(g, cb, p, pszLabel, hr) APOTest_Initialize(g, cb, p, pszLabel, hr, #hr)