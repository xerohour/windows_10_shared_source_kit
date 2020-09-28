// Copyright(C) Microsoft.All rights reserved.
class CPinReq: private CKSTopoDevLoop
{
public:
    DWORD WINAPI CheckPinKsDataRange(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckPinKsDataRangeHelper, static_cast<void*>(this)));                 
        }
    DWORD WINAPI CheckPinKsDataFormat(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckPinKsDataFormatHelper, static_cast<void*>(this)));                 
        }
    DWORD WINAPI CheckPinDataFlow(CDevice *pDevice)
        {
            return (LoopFilters(pDevice, 
                &CPinReq::CheckPinDataFlowHelper, static_cast<void*>(this)));                 
        }
    DWORD WINAPI CheckOrphanedPins(CDevice *pDevice)
        {
            return (LoopFilters(pDevice, 
                &CPinReq::CheckOrphanedPinsHelper, static_cast<void*>(this)));                 
        }
    DWORD WINAPI CheckJackDesc(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckJackDescHelper, static_cast<void*>(this)));                 
        }
    DWORD WINAPI CheckJackDesc2(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckJackDesc2Helper, static_cast<void*>(this)));                 
        }

    DWORD WINAPI CheckJackSinkInfo(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckJackSinkInfoHelper, static_cast<void*>(this)));                 
        }

    DWORD WINAPI CheckMicArrayGeometry(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckMicArrayGeometryHelper, static_cast<void*>(this)));                 
        }

    DWORD WINAPI CheckProcessingModes(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckProcessingModesHelper, static_cast<void*>(this)));                 
        }

    DWORD WINAPI CheckProposeDataformat2(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckProposeDataformat2Helper, static_cast<void*>(this)));                 
        }

    DWORD WINAPI CheckPinInstanceCount(CDevice *pDevice)
        {
            return (LoopFiltersPins(pDevice, 
                &CPinReq::CheckPinInstanceCountHelper, static_cast<void*>(this)));                 
        }

    DWORD WINAPI CheckMicrophoneSensitivity(CDevice *pDevice)
    {
        return (LoopFiltersPins(pDevice,
            &CPinReq::CheckMicrophoneSensitivityHelper, static_cast<void*>(this)));
    }

    DWORD WINAPI CheckMicrophoneSNR(CDevice *pDevice)
    {
        return (LoopFiltersPins(pDevice,
            &CPinReq::CheckMicrophoneSNRHelper, static_cast<void*>(this)));
    }

private:
    enum
    {   
        DATA_NONE = 0,
        DATA_INTERNAL_OUT = 1, 
        DATA_INTERNAL_IN = 2
    };

    enum
    {   
        JACKTEST_JACKDESC = 0,
        JACKTEST_JACKDESC2, 
        JACKTEST_SINKINFO,
        JACKTEST_NUMTESTS
    };

    static bool CheckPinKsDataRangeHelper(void *);
    static bool CheckPinKsDataFormatHelper(void *);
    static bool IsPinKsDataFormat(void *, GUID);
    static bool CheckPinDataFlowHelper(void *);
    static bool CheckOrphanedPinsHelper(void *);    
    static bool CheckJackDescHelper(void *);
    static bool CheckJackDesc2Helper(void *);
    static bool CheckJackSinkInfoHelper(void *);
    static bool CheckMicArrayGeometryHelper(void *);
    static bool CheckProcessingModesHelper(void *);
    static bool CheckProposeDataformat2Helper(void *);
    static bool CheckPinInstanceCountHelper(void *);
    static bool CheckMicrophoneSensitivityHelper(void *);
    static bool CheckMicrophoneSNRHelper(void *);

    // audio follows a path through the Kernel Streaming graph
    // it can hop from filter to filter along connections
    // the path has two endpoints:
    // the "streaming pin" which supports the various formats
    // and the "hardware bridge pin" which represents the interface to hardware
    bool IsHardwareBridgePin();
    bool IsSoftwareStreamingPin();
        
    bool CheckJackPropHelper(UINT);
    bool IsJackDescSupported(EndpointFormFactor, EDataFlow);
    bool IsJackDesc2Supported();
    bool IsJackSinkInfoSupported();
    bool CreatePin(GUID);
    bool CheckMicArrayGeometryOnPin();
    bool CheckProcessingModes();
    bool CheckProposeDataformat2();
    bool CheckMicrophoneSensitivity();
    bool CheckMicrophoneSNR();
    bool QueryForProcessingModes(PKSMULTIPLE_ITEM  *pKsMultiple);
    bool EvaluateProposeDataformat2ForMode(const GUID& gProcessingMode);
    bool EvaluateProposeDataformat2ForRender(const GUID* garrayProcessingMode, UINT cModes);


    static bool GetPinInternalConnections(CFilter *, UINT[], UINT);
    bool PinIoCtl_Get(GUID, ULONG, void*, UINT, void **, UINT &);
    bool PinIoCtl_Support(GUID, ULONG, DWORD &);
    HRESULT GetEndpointFormFactorAndFlow(
        _In_ IMMDevice *,
        _Out_ EndpointFormFactor *,
        _Out_ EDataFlow *
    );
    // Logging Functions
    inline void LogPinMsg(UINT, LPCSTR);
    inline void LogPinErr(LPCSTR , LPCSTR );
    inline void LogPinErr(LPCSTR , LPCSTR , UINT);
    inline void LogPinSkip(LPCSTR);
};

// Indexed by SourcePinId
typedef struct _PHYS_CONN
{
    UINT            iDestFilterId;
    UINT            iDestPinId;
} PHYS_CONN;
// Indexed by SourceFilterId
typedef struct _FILTER_CONN
{
    PHYS_CONN      *pPhysicalConn;
    UINT            iPhysicalConnSize;
} FILTER_CONN;

typedef struct _MODE_INFO
{
    GUID        guidType;
    WCHAR        *wszFriendlyName;
    UINT        iCount;
} MODE_INFO;