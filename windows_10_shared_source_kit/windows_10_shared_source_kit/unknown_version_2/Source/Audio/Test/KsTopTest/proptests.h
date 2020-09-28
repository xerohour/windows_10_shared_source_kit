// Copyright(C) Microsoft.All rights reserved.

class CPropDescReq: private CKSTopoDevLoop
{
public:
    bool m_fIsFilterProperty;

    CPropDescReq();
    DWORD WINAPI CheckPropInValidNodes(CDevice *pDevice, const GUID* pPropertySetId, UINT iPropIDChecked)
        {
            m_pPropertySetId = pPropertySetId;
            m_iPropIDChecked = iPropIDChecked;
            return (LoopFiltersNodesProps(pDevice,
                        &CPropDescReq::CheckPropInValidNodesHelper, static_cast<void*>(this)));
        }
    DWORD WINAPI CheckPropAccessFlags(CDevice *pDevice)
        { 
            return (LoopFiltersNodesProps(pDevice,
                        &CPropDescReq::CheckPropAccessFlagsHelper1, static_cast<void*>(this)));
        }
    DWORD WINAPI CheckPropDescHeaders(CDevice *pDevice)
        {
            return (LoopFiltersNodesProps(pDevice,
                        &CPropDescReq::CheckPropDescHeadersHelper, static_cast<void*>(this)));
        }
    DWORD WINAPI CheckPropGetSetRanges(CDevice *pDevice)
        {
            return (LoopFiltersNodesProps(pDevice,
                        &CPropDescReq::CheckPropGetSetRangesHelper, static_cast<void*>(this)));
        }

private:
    // Constants and Enum
    static const UINT UNKNOWN = UINT_MAX;
    static const UINT INVALID = UINT_MAX - 1;

    static const UINT MAX_NUM_PROPDESC = KSPROPERTY_AUDIO_LAST + 1;
    enum PROP_DESC_FLAG_SETTINGS {FL_YES, FL_NO, FL_UNKNOWN};
    enum PROP_SET_INDEX{AUDIO_SET,AUDIOENGINE_SET,UNKNOWN_SET};

    // Variables stored to be used between functions
    const GUID* m_pPropertySetId;
    UINT m_iPropIDChecked;
    UINT m_iNumChannels;
    bool m_fIsChannelProperty;
    
    KSPROPERTY_DESCRIPTION  *m_pDesc;
    // List of Property Descriptor Requirements
    UINT m_iPropDescReqList[UNKNOWN_SET][MAX_NUM_PROPDESC];

    // Logging Functions
    inline void LogPropErr(LPCSTR , LPCSTR );

    // Checking Functions
    static bool CheckPropInValidNodesHelper(void *);

    static bool CheckPropAccessFlagsHelper1(void *);
    bool CheckPropAccessFlagsHelper2(ULONG, PROP_DESC_FLAG_SETTINGS, PROP_DESC_FLAG_SETTINGS);
    inline bool IsAccessFlagsValid(ULONG);

    static bool CheckPropDescHeadersHelper(void *);
    bool HasValidPropDescStruct(KSPROPERTY_DESCRIPTION *);
    bool HasValidMembersListCount(UINT);
    bool HasValidPropTypeSet(KSIDENTIFIER);
    bool HasValidMemHeaderStruct(KSPROPERTY_DESCRIPTION *);
    inline bool IsMembersHeaderMembersFlagsValid(ULONG);
    bool IsMembersHeaderFlagsValid(ULONG, UINT);

    static bool CheckPropGetSetRangesHelper(void *);
    template <typename T, typename X> bool IsValueInRange(T, T, X, UINT);
} ;

std::string PropTypeToString(UINT PropTypeID);
