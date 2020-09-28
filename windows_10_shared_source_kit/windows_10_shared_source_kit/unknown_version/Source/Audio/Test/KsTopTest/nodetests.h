// Copyright(C) Microsoft.All rights reserved.

class CNodeReq: private CKSTopoDevLoop
{
public:
    DWORD WINAPI CheckNodesIOCount(CDevice *pDevice)
        {
            return (LoopFilters(pDevice,
                &CNodeReq::CheckNodesIOCountHelper, static_cast<void*>(this)));                 
        }
    DWORD WINAPI CheckNodeProperties(CDevice *pDevice, GUID guidNodeType)
        {
            m_guidNodeType = guidNodeType;
            return (LoopFiltersNodes(pDevice, 
                &CNodeReq::CheckNodePropertiesHelper, static_cast<void*>(this)));                 
        }
    DWORD WINAPI CheckOrphanedNodes(CDevice *pDevice)
        {
            return (LoopFilters(pDevice,
                &CNodeReq::CheckOrphanedNodesHelper, static_cast<void*>(this)));                 
        }

private:
    // Constants and Enums
    enum  NODE_PROP_REQ { ALL, AT_LEAST_ONE, SPECIAL_COMBINATION, OPTIONAL_ALL };

    // When a connection is read, it contains a pair of 
    // ((FromNode, FromPin), (ToNode, ToPin))
    // When the connection starts/ends at a Pin and not a node,
    // the FromNode/ToNode is set to (UINT)(-1)
    static const UINT PIN_CONN = (UINT)(-1);

    // Variables stored to be used between functions
    GUID m_guidNodeType;

    // Table to keep track of Node Requirements
    // Same for all instances
    static const struct _NodeReqList
    {
        GUID                        m_guidNodeType;
        KSPROPERTY_AUDIO            *m_AudioFilterPropList;
        UINT                        m_iAudioFilterPropListSize;
        KSPROPERTY_AUDIO            *m_AudioPinPropList;
        UINT                        m_iAudioPinPropListSize;
        KSPROPERTY_AUDIOENGINE      *m_AudioEngineFilterPropList;
        UINT                        m_iAudioEngineFilterPropListSize;
        KSPROPERTY_AUDIOENGINE      *m_AudioEnginePinPropList;
        UINT                        m_iAudioEnginePinPropListSize;
        NODE_PROP_REQ               m_iRequired;
        bool                        (*m_pfnSpecialCheck)(int,CNode*);
    } m_NodeReqList[]; 

    // Functions
    static bool CheckNodesIOCountHelper(void *);
    static bool CheckNodePropertiesHelper(void *);
    bool DoesNodeSupportAudioFilterProperties(int);
    bool DoesNodeSupportAEFilterProperties(int);
    bool DoesNodeSupportAudioPinProperties(int);
    bool DoesNodeSupportAEPinProperties(int);
    bool DoesNodeSupportProperties( const GUID*, bool, int, UINT*, UINT);
    static bool DoesPeakMeterNodeSupportProperties(int,CNode*);
    static bool CheckOrphanedNodesHelper(void *);
};
