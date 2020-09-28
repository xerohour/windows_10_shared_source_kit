// Copyright(C) Microsoft.All rights reserved.

#include "common.h"

KSPROPERTY_AUDIO g_Audio_OneOff[KSPROPERTY_AUDIO_LAST + 1] = {};

#define EMPTY_LIST() nullptr, 0
#define THREE_EMPTY_LISTS() EMPTY_LIST(), EMPTY_LIST(), EMPTY_LIST()
#define FOUR_EMPTY_LISTS() EMPTY_LIST(), EMPTY_LIST(), EMPTY_LIST(), EMPTY_LIST()

// using the comma operator to lazily initialize g_Audio_OneOff[x] to x
// and then return the address to the location which has value x
#define ONE_OFF(x) (g_Audio_OneOff[x] = x, &g_Audio_OneOff[x]), 1

// multiple properties
#define LIST(x) x, ARRAYSIZE(x)

KSPROPERTY_AUDIO g_Audio_AEC_Filter_Properties[] =
{
    KSPROPERTY_AUDIO_CPU_RESOURCES,
    KSPROPERTY_AUDIO_ALGORITHM_INSTANCE,
};

KSPROPERTY_AUDIO g_Audio_Equalizer_Filter_Properties[] =
{
    KSPROPERTY_AUDIO_EQ_LEVEL,
    KSPROPERTY_AUDIO_NUM_EQ_BANDS,
    KSPROPERTY_AUDIO_EQ_BANDS,
};

KSPROPERTY_AUDIO g_Audio_Loudness_Filter_Properties[] =
{
    KSPROPERTY_AUDIO_LOUDNESS,
    KSPROPERTY_AUDIO_DYNAMIC_RANGE,
};

KSPROPERTY_AUDIO g_Audio_NoiseSuppress_Filter_Properties[] =
{
    KSPROPERTY_AUDIO_CPU_RESOURCES,
    KSPROPERTY_AUDIO_ALGORITHM_INSTANCE,
};

KSPROPERTY_AUDIO g_Audio_PeakMeter_Filter_Properties[] =
{
    KSPROPERTY_AUDIO_CPU_RESOURCES,
    KSPROPERTY_AUDIO_PEAKMETER,
#if NTDDI_VERSION >= NTDDI_WIN8
    // KSPROPERTY_AUDIO_PEAKMETER is required to NOT be supported
    KSPROPERTY_AUDIO_PEAKMETER2,
#endif
};

KSPROPERTY_AUDIO g_Audio_Tone_Filter_Properties[] =
{
    KSPROPERTY_AUDIO_BASS,
    KSPROPERTY_AUDIO_MID,
    KSPROPERTY_AUDIO_TREBLE,
    KSPROPERTY_AUDIO_BASS_BOOST,
};

KSPROPERTY_AUDIO g_Audio_SuperMix_Filter_Properties[] =
{
    KSPROPERTY_AUDIO_MIX_LEVEL_TABLE,
    KSPROPERTY_AUDIO_MIX_LEVEL_CAPS,
};

#if NTDDI_VERSION >= NTDDI_WIN8
KSPROPERTY_AUDIO g_Audio_AudioEngine_Filter_Properties[] =
{
    KSPROPERTY_AUDIO_VOLUMELEVEL,
    KSPROPERTY_AUDIO_MUTE,
    KSPROPERTY_AUDIO_PEAKMETER2,
};

KSPROPERTY_AUDIO g_Audio_AudioEngine_Pin_Properties[] =
{
    KSPROPERTY_AUDIO_VOLUMELEVEL,
    KSPROPERTY_AUDIO_MUTE,
    KSPROPERTY_AUDIO_PEAKMETER2,
    KSPROPERTY_AUDIO_PRESENTATION_POSITION,
    KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_POSITION,
    KSPROPERTY_AUDIO_LINEAR_BUFFER_POSITION,
#if NTDDI_VERSION >= NTDDI_WINBLUE
    KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_LASTBUFFER_POSITION,
#endif // NTDDI_VERSION >= NTDDI_WINBLUE
};

KSPROPERTY_AUDIOENGINE g_AudioEngine_AudioEngine_Filter_Properties[] =
{
    KSPROPERTY_AUDIOENGINE_GFXENABLE,
    KSPROPERTY_AUDIOENGINE_MIXFORMAT,
    KSPROPERTY_AUDIOENGINE_DEVICEFORMAT,
    KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS,
    KSPROPERTY_AUDIOENGINE_DESCRIPTOR,
    KSPROPERTY_AUDIOENGINE_BUFFER_SIZE_RANGE,
};

KSPROPERTY_AUDIOENGINE g_AudioEngine_AudioEngine_Pin_Properties[] =
{
    KSPROPERTY_AUDIOENGINE_LFXENABLE,
    KSPROPERTY_AUDIOENGINE_BUFFER_SIZE_RANGE,
    KSPROPERTY_AUDIOENGINE_LOOPBACK_PROTECTION,
    KSPROPERTY_AUDIOENGINE_VOLUMELEVEL 
};
#endif // NTDDI_VERSION >= NTDDI_WIN8

const CNodeReq::_NodeReqList CNodeReq::m_NodeReqList[] = 
{
    {   KSNODETYPE_ACOUSTIC_ECHO_CANCEL,
        LIST(g_Audio_AEC_Filter_Properties),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_AGC,
        ONE_OFF(KSPROPERTY_AUDIO_AGC),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_CHORUS,
        ONE_OFF(KSPROPERTY_AUDIO_CHORUS_LEVEL),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_DELAY,
        ONE_OFF(KSPROPERTY_AUDIO_DELAY),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_DEMUX,
        ONE_OFF(KSPROPERTY_AUDIO_DEMUX_DEST),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_DEV_SPECIFIC,
        ONE_OFF(KSPROPERTY_AUDIO_DEV_SPECIFIC),
        THREE_EMPTY_LISTS(),
        OPTIONAL_ALL, NULL
    },
    {   KSNODETYPE_EQUALIZER,
        LIST(g_Audio_Equalizer_Filter_Properties),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_LOUDNESS,
        LIST(g_Audio_Loudness_Filter_Properties),
        THREE_EMPTY_LISTS(),
        AT_LEAST_ONE, NULL
    },
    {   KSNODETYPE_MUTE,
        ONE_OFF(KSPROPERTY_AUDIO_MUTE),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_MUX,
        ONE_OFF(KSPROPERTY_AUDIO_MUX_SOURCE),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_NOISE_SUPPRESS,
        LIST(g_Audio_NoiseSuppress_Filter_Properties),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_PEAKMETER,
        LIST(g_Audio_PeakMeter_Filter_Properties),
        THREE_EMPTY_LISTS(),
        SPECIAL_COMBINATION, DoesPeakMeterNodeSupportProperties
    },
    {   KSNODETYPE_PROLOGIC_DECODER,
        ONE_OFF(KSPROPERTY_AUDIO_CHANNEL_CONFIG),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_REVERB,
        ONE_OFF(KSPROPERTY_AUDIO_REVERB_LEVEL),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_STEREO_WIDE,
        ONE_OFF(KSPROPERTY_AUDIO_WIDENESS),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_SUPERMIX,
        LIST(g_Audio_SuperMix_Filter_Properties),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_SYNTHESIZER,
        FOUR_EMPTY_LISTS(),
        ALL, NULL
    },
    {   KSNODETYPE_TONE,
        LIST(g_Audio_Tone_Filter_Properties),
        THREE_EMPTY_LISTS(),
        AT_LEAST_ONE, NULL
    },
    {   KSNODETYPE_VOLUME,
        ONE_OFF(KSPROPERTY_AUDIO_VOLUMELEVEL),
        THREE_EMPTY_LISTS(),
        ALL, NULL
    },
#if NTDDI_VERSION >= NTDDI_WIN8
    {   KSNODETYPE_AUDIO_ENGINE,
        LIST(g_Audio_AudioEngine_Filter_Properties),
        LIST(g_Audio_AudioEngine_Pin_Properties),
        LIST(g_AudioEngine_AudioEngine_Filter_Properties),
        LIST(g_AudioEngine_AudioEngine_Pin_Properties),
        ALL, 
        NULL
    },
#endif
};

bool CNodeReq::CheckNodesIOCountHelper(void *pArg)
{
    CNodeReq       *pThis = static_cast<CNodeReq*>(pArg);
    bool            fRet = true;
    CConnection    *pConnection = NULL;
    UINT            iNodeCount=0;
    UINT            iConnection = 0, cConnection = 0;
    UINT           *pInputConn = NULL, *pOutputConn = NULL;
    GUID            nodeType;

    // Create input/output counters for the nodes in each filter
     iNodeCount = pThis->m_pFilter->GetCountNodes();
     if( 0 == iNodeCount )
     {
         SLOG(eBlab2, "   Filter %s contains no nodes.", pThis->m_pFilter->GetFriendlyName());
         goto Exit;
     }

    pInputConn = new UINT[iNodeCount];
    if(NULL == pInputConn)
    {
        XLOG(XFAIL, eError, " ERROR: Unable to allocate memory.");
        fRet = false;
        goto Exit;
    }
    SecureZeroMemory(pInputConn, iNodeCount*sizeof(UINT));
    pOutputConn = new UINT[iNodeCount];
    if(NULL == pOutputConn)
    {
        XLOG(XFAIL, eError, " ERROR: Unable to allocate memory.");
        fRet = false;
        goto Exit;
    }
    SecureZeroMemory(pOutputConn, iNodeCount*sizeof(UINT));

    // Loop through the connections and count the number of inputs/outputs for every node
    cConnection = pThis->m_pFilter->GetCountConnections();

    for(iConnection = 0; iConnection < cConnection; ++iConnection)
    {
        pConnection = pThis->m_pFilter->GetConnection(iConnection);
        if (NULL == pConnection)
        {
            XLOG(XFAIL, eError, "   ERROR: Unable to obtain Connection[%02d]", iConnection);
            fRet = false;
            goto Exit;
        }
        SLOG(eBlab2, "   Connection: Getting Node From/To information from Connection[%d]", iConnection);

        if((pConnection->FromNode() != PIN_CONN) && (pConnection->FromNode() < (INT)iNodeCount))
        {
            ++pOutputConn[pConnection->FromNode()];
        }
        if((pConnection->ToNode() != PIN_CONN) && (pConnection->ToNode() < (INT)iNodeCount))
        {
            ++pInputConn[pConnection->ToNode()];
        }
    } // For Loop iConnection

    // Check that each node has at least one input and one output except for:
    // KSNODETYPE_MUX:          Multiple Input / 1 Output
    // KSNODETYPE_SUM:          Multiple Input / 1 Output
    // KSNODETYPE_DEMUX:        1 Input / Multiple Outputs
    // KSNODETYPE_ACOUSTIC_ECHO_CANCEL: 2 Inputs / 2 Outputs
    // KSNODETYPE_AUDIO_ENGINE: 2 Inputs / 2 Outputs
    for(pThis->m_iNode = 0; pThis->m_iNode < iNodeCount; ++pThis->m_iNode)
    {
        pThis->m_pNode = pThis->m_pFilter->GetNode(pThis->m_iNode);
        if(NULL == pThis->m_pNode)
        {
            XLOG(XFAIL, eError, "  ERROR: Unable to obtain reference to Node[%02d]", pThis->m_iNode);
            fRet = false;
            break;
        }

        nodeType = *(pThis->m_pNode->GetType());
        SLOG(eBlab2, "  Node: Node[%d] (%s) has %d inputs and %d outputs", pThis->m_iNode, 
            GetKSGuidName(nodeType), pInputConn[pThis->m_iNode], pOutputConn[pThis->m_iNode]);

        if( IsEqualGUID(nodeType, KSNODETYPE_SUM) )
        {
            // A SUM node must have at least one output.
            // If a SUM node has more than one output, that's fine:
            // there's no KSNODETYPE_TEE, so the way you tee a signal
            // is to add multiple output connections on any node.
            //
            // A SUM node must have at least one input.
            // There are two reasons a SUM node could have only one input:
            // 1) XP's parser relies on a SUM or MUX node to parse the graph.
            //    So USB Audio 1.0 devices that want backcompat to XP will put a SUM node
            //    which does nothing.
            //    Since XP is no longer supported, this reason no longer applies.
            //
            // 2) Windows Blue drivers that support multiple modes on a given pin factory
            //    but which do not support audio offload
            //    will have a SUM node with one input, from the host pin factory
            //        this is instantiated twice, so the SUM is actually doing some work
            //    and with two outputs:
            //        one to the loopback pin factory,
            //        and the other to the bridge pin that eventually hits the hardware
            if((pInputConn[pThis->m_iNode] < 1) || (pOutputConn[pThis->m_iNode] < 1))
            {
                XLOG(XFAIL, eError, "  FAIL: Node[%02d] (%s) has %d inputs and %d outputs", pThis->m_iNode, 
                    GetKSGuidName(nodeType), pInputConn[pThis->m_iNode], pOutputConn[pThis->m_iNode]);
                fRet = false;
            }
        }
        else if( IsEqualGUID(nodeType, KSNODETYPE_MUX) )
        {
            // A MUX node must have at least one output.
            // If a MUX node has more than one output, that's fine:
            // there's no KSNODETYPE_TEE, so the way you tee a signal
            // is to add multiple output connections on any node.
            //
            // A MUX node must have at least two inputs.
            //
            // Previous versions of this test allowed MUXes to have only a single input,
            // for USB Audio 1.0 devices; this was necessary for backcompat to XP.
            //
            // XP is no longer supported, so this test should now fail such muxes.
            if((pInputConn[pThis->m_iNode] < 2) || (pOutputConn[pThis->m_iNode] < 1))
            {
                XLOG(XFAIL, eError, "  FAIL: Node[%02d] (%s) has %d inputs and %d outputs", pThis->m_iNode, 
                    GetKSGuidName(nodeType), pInputConn[pThis->m_iNode], pOutputConn[pThis->m_iNode]);
                fRet = false;
            }
        }
        else if( IsEqualGUID(nodeType, KSNODETYPE_DEMUX) )
        {   
            if((pInputConn[pThis->m_iNode] != 1) || (pOutputConn[pThis->m_iNode] <= 1))
            {
                XLOG(XFAIL, eError, "  FAIL: Node[%02d] (KSNODETYPE_DEMUX) has %d inputs and %d outputs", 
                    pThis->m_iNode, pInputConn[pThis->m_iNode], pOutputConn[pThis->m_iNode]);
                fRet = false;
            }
        }
        else if( IsEqualGUID(nodeType, KSNODETYPE_ACOUSTIC_ECHO_CANCEL))
        {   
            if((pInputConn[pThis->m_iNode] != 2) || (pOutputConn[pThis->m_iNode] != 2))
            {
                XLOG(XFAIL, eError, "  FAIL: Node[%02d] (KSNODETYPE_ACOUSTIC_ECHO_CANCEL) has %d inputs and %d outputs",
                    pThis->m_iNode, pInputConn[pThis->m_iNode], pOutputConn[pThis->m_iNode]);
                fRet = false;
            }
        }
        else if( IsEqualGUID(nodeType, KSNODETYPE_AUDIO_ENGINE) )
        {
            // On Render:
            // An audio engine node is required to have the following connections:
            //      - two ks sink pin factories (pInputConn == 2)
            //      - a single ks source pin factory
            //      - a single ks none pin factory -or- a DAC (pOutputConn == 2)
            if((pInputConn[pThis->m_iNode] != 2) || (pOutputConn[pThis->m_iNode] != 2))
            {
                XLOG(XFAIL, eError,
                    "  FAIL: Node[%02d] (KSNODETYPE_AUDIO_ENGINE) has %d inputs and %d outputs.  Expecting 2 inputs and 2 outputs.",
                    pThis->m_iNode, pInputConn[pThis->m_iNode], pOutputConn[pThis->m_iNode]);
                fRet = false;
            }
        }
        else // All other node types
        {
            if((0 == pInputConn[pThis->m_iNode]) || (0 == pOutputConn[pThis->m_iNode]))
            {
                XLOG(XFAIL, eError, "  FAIL: Node[%02d] (%s) has %d inputs and %d outputs", pThis->m_iNode, GetKSGuidName(nodeType),
                    pInputConn[pThis->m_iNode], pOutputConn[pThis->m_iNode]);
                fRet = false;
            }
            // Extra fuzzy check Audio Gain Control
            // Check if the words 
            //      automatic AND gain are missing
            //  OR  gain AND control are missing
            //  OR  microphone OR boost are present
            if( IsEqualGUID(nodeType, KSNODETYPE_AGC) )
            {
                LPSTR szName = pThis->m_pNode->GetNodeName();
                _strlwr(szName);
                if ( (NULL == (strstr(szName, "automatic")) && (NULL == strstr(szName, "gain")))
                    ||((NULL == strstr(szName, "gain")) && (NULL == strstr(szName, "control")))
                    ||(NULL != strstr(szName, "microphone")) 
                    ||(NULL != strstr(szName, "boost")) )
                {
                    SLOG(eWarn1, "  WARNING: Node[%02d] (KSNODETYPE_AGC) possibly used incorrectly (not as an Automatic Gain Control)", pThis->m_iNode);
                }
            }
        }
    }  // For Loop pThis->m_iNode

Exit:
    if(pInputConn != NULL)  delete[] pInputConn;
    if(pOutputConn != NULL) delete[] pOutputConn;
    return (fRet);
}

bool CNodeReq::DoesNodeSupportAudioFilterProperties(int iIndex)
{
    return DoesNodeSupportProperties( &KSPROPSETID_Audio, true, iIndex,
        (UINT *)(m_NodeReqList[iIndex].m_AudioFilterPropList), m_NodeReqList[iIndex].m_iAudioFilterPropListSize );
}

bool CNodeReq::DoesNodeSupportAEFilterProperties(int iIndex)
{
    return DoesNodeSupportProperties( &KSPROPSETID_AudioEngine, true, iIndex,
        (UINT *)(m_NodeReqList[iIndex].m_AudioEngineFilterPropList), m_NodeReqList[iIndex].m_iAudioEngineFilterPropListSize );
}

bool CNodeReq::DoesNodeSupportAudioPinProperties(int iIndex)
{
    return DoesNodeSupportProperties( &KSPROPSETID_Audio, false, iIndex,
        (UINT *)(m_NodeReqList[iIndex].m_AudioPinPropList), m_NodeReqList[iIndex].m_iAudioPinPropListSize );
}

bool CNodeReq::DoesNodeSupportAEPinProperties(int iIndex)
{
    return DoesNodeSupportProperties( &KSPROPSETID_AudioEngine, false, iIndex,
        (UINT *)(m_NodeReqList[iIndex].m_AudioEnginePinPropList), m_NodeReqList[iIndex].m_iAudioEnginePinPropListSize );
}

bool CNodeReq::DoesNodeSupportProperties( const GUID* pPropertySet, bool checkFilter, int iIndex, UINT* pRequiredProperties, UINT cRequiredProperties )
{
    bool                        fRet = true;
    const struct _NodeReqList  *pNodeReqEnt = &m_NodeReqList[iIndex];
    UINT                        iReqList = 0, cNodes=0, cReqPropListSize=0;
    ULONG                       ulAEPropListCheck = 0;

    if( checkFilter )
    {
        cNodes = m_pNode->GetCountFilterProperties();

        if( IsEqualGUID( *pPropertySet, KSPROPSETID_Audio ) )
        {
            cReqPropListSize = pNodeReqEnt->m_iAudioFilterPropListSize;
        }
        else if( IsEqualGUID( *pPropertySet, KSPROPSETID_AudioEngine ) )
        {
            cReqPropListSize = pNodeReqEnt->m_iAudioEngineFilterPropListSize;
        }
    }
    else
    {
        cNodes = m_pNode->GetCountPinProperties();

        if( IsEqualGUID( *pPropertySet, KSPROPSETID_Audio ) )
        {
            cReqPropListSize = pNodeReqEnt->m_iAudioPinPropListSize;
        }
        else if( IsEqualGUID( *pPropertySet, KSPROPSETID_AudioEngine ) )
        {
            cReqPropListSize = pNodeReqEnt->m_iAudioEnginePinPropListSize;
        }
    }

    for(m_iPropDesc = 0; m_iPropDesc < cNodes; ++m_iPropDesc)
    {
        if( checkFilter )
        {
            m_pPropDesc = m_pNode->GetFilterProperty(m_iPropDesc);
        }
        else
        {
            m_pPropDesc = m_pNode->GetPinProperty(m_iPropDesc);
        }

        if( (NULL == m_pPropDesc) || !m_pPropDesc->IsValid())
        {
            XLOG(XFAIL, eError, "   ERROR: Unable to obtain reference to Property Descriptor[%d]", m_iPropDesc);
            return false;
        }
        else if( !IsEqualGUID( *(m_pPropDesc->GetPropertySet()), *pPropertySet ) )
        {
            // m_pPropDesc is not the right property set -- continue
            continue;
        }

        // Check if the property in this loop is one that is being 
        // checked for in the KSPROPERTY_AUDIOENGINE List
        for(iReqList = 0; iReqList < cRequiredProperties; ++iReqList)
        {
            if( pRequiredProperties[iReqList] == m_pPropDesc->GetPropertyId() )
            {
                ulAEPropListCheck |= 1<<iReqList;
                break;
            }
        } // For Loop iReqList
    } // For Loop m_iPropDesc

    // Loop and display which properties are not available ONLY when one of the following is true:
    // 1. AT_LEAST_ONE property is required and all of the required property ulFlags are 0
    // 2. ALL properties are required and not all of the required property ulFlags are 1
    if(    ((pNodeReqEnt->m_iRequired == AT_LEAST_ONE) && (ulAEPropListCheck == 0))
        || ((pNodeReqEnt->m_iRequired == ALL) 
                    && ((ulAEPropListCheck+1) != (ULONG)(1<<cReqPropListSize)))
      )
    {
        for(iReqList = 0; iReqList < cRequiredProperties; ++iReqList)
        {
            if( (ulAEPropListCheck & (1<<iReqList)) == 0)
            {
                XLOG(XFAIL, eError, "   ERROR: Node[%d] (%s) does not support %s through the %s", m_pNode->GetId(),
                             GetKSGuidName(*m_pNode->GetType()),
                             IsEqualGUID( *pPropertySet, KSPROPSETID_Audio ) ?
                               GetKSAudioPropertyName( static_cast<KSPROPERTY_AUDIO>(pRequiredProperties[iReqList]) ) :
                               GetKSAudioEnginePropertyName( static_cast<KSPROPERTY_AUDIOENGINE>(pRequiredProperties[iReqList]) ),
                             ( checkFilter ? "filter" : "pin" ));
                fRet = false;
            }
        }
    }

    return fRet;
}

bool CNodeReq::CheckNodePropertiesHelper(void *pArg)
{
    CNodeReq   *pThis = static_cast<CNodeReq*>(pArg);
    bool        fRet = true;
    UINT        iNodeList=0;

    if( !IsEqualGUID(*pThis->m_pNode->GetType(), pThis->m_guidNodeType) )
    {
        //This is NOT the node to be checked
        return true; 
    }
    SLOG(eBlab2, "  Node[%d] is the one to be checked", pThis->m_iNode);

    // Find index of required m_NodeReqList and send it to functions called
    for(iNodeList = 0; iNodeList < ARRAYSIZE(pThis->m_NodeReqList) ; ++iNodeList)
    {
        if(IsEqualGUID(pThis->m_guidNodeType, pThis->m_NodeReqList[iNodeList].m_guidNodeType))
        {
            if(SPECIAL_COMBINATION == pThis->m_NodeReqList[iNodeList].m_iRequired)
            {
                fRet = ((pThis->m_NodeReqList[iNodeList].m_pfnSpecialCheck(iNodeList, pThis->m_pNode)) ? fRet : false);
            }
            else
            {
                fRet = ((pThis->DoesNodeSupportAudioFilterProperties(iNodeList)) ? fRet : false);
                fRet = ((pThis->DoesNodeSupportAudioPinProperties(iNodeList)) ? fRet : false);
                fRet = ((pThis->DoesNodeSupportAEFilterProperties(iNodeList)) ? fRet : false);
                fRet = ((pThis->DoesNodeSupportAEPinProperties(iNodeList)) ? fRet : false);
            }
            break;
        }
    } // For Loop iPropList

    return (fRet);
}

bool CNodeReq::CheckOrphanedNodesHelper(void *pArg)
{
    bool            fRet = true;
    CNodeReq       *pThis = static_cast<CNodeReq*>(pArg);
    CConnection    *pConnection = NULL;
    UINT           *piIsConnected = NULL;
    UINT            iConnection=0, cConnection=0,
                    iNodeCount = pThis->m_pFilter->GetCountNodes();

    // Get the number of nodes
    piIsConnected = new UINT[iNodeCount];
    SecureZeroMemory(piIsConnected, sizeof(UINT)*iNodeCount);

    // Check which nodes have input and output connections
    cConnection = pThis->m_pFilter->GetCountConnections();
    for(iConnection = 0; iConnection < cConnection; ++iConnection)
    {
        pConnection = pThis->m_pFilter->GetConnection(iConnection);
        if(NULL == pConnection)
        {
            XLOG(XFAIL, eError, "  ERROR: Unable to obtain reference to Connection[%d]", iConnection);
            fRet = false;
            goto Exit;
        }

        if(pConnection->FromNode() != -1)
        {
            piIsConnected[pConnection->FromNode()] |= 1;
        }
        if(pConnection->ToNode() != -1)
        {
            piIsConnected[pConnection->ToNode()] |= 2;
        }
    }

    // Make sure no node has no input and output connections
    for(UINT i=0; i<iNodeCount; ++i)
    {
        if(piIsConnected[i] < 3)
        {
            pThis->m_pNode = pThis->m_pFilter->GetNode(i);
            if(pThis->m_pNode == NULL)
            {
                XLOG(XFAIL, eError, "  ERROR: Node orphaned, but unable "
                                "to obtain reference to it");
            }
            else
            {
                XLOG(XFAIL, eError, "  FAIL: Node[%d] is orphaned", pThis->m_pNode->GetId());
            }
            fRet = false;
        }
    }
    
Exit:
    if(piIsConnected != NULL)
    {
        delete[] piIsConnected;
    }
    return fRet;
}

bool CNodeReq::DoesPeakMeterNodeSupportProperties(int iIndex, CNode* pCNode)
{
    bool fRet = true;

    const KSPROPERTY_AUDIO* pRequiredProperties = m_NodeReqList[iIndex].m_AudioFilterPropList;
    UINT cRequiredProperties = m_NodeReqList[iIndex].m_iAudioFilterPropListSize;
    UINT cNodeProperties = pCNode->GetCountFilterProperties();

    // The Peak Meter Node is special in that the node must support
    //  KSPROPERTY_AUDIO_PEAKMETER -or- KSPROPERTY_AUDIO_PEAKMETER2, but NOT both
    for( UINT iRequiredProperty = 0; iRequiredProperty < cRequiredProperties; ++iRequiredProperty )
    {
        KSPROPERTY_AUDIO reqProp = pRequiredProperties[iRequiredProperty];
        bool foundProperty = false;

        for( UINT iPropDesc = 0; iPropDesc < cNodeProperties; ++iPropDesc )
        {
            CPropDesc* pPropDesc = pCNode->GetFilterProperty(iPropDesc);

            if( static_cast<KSPROPERTY_AUDIO>(pPropDesc->GetPropertyId()) == reqProp )
            {
                foundProperty = true;

                break;
            }
        }

#if NTDDI_VERSION >= NTDDI_WIN8
        // special handling for deprecated PEAKMETER property for Windows 8
        if( reqProp == KSPROPERTY_AUDIO_PEAKMETER)
        {
            if( foundProperty )
            {
                fRet = false;
                XLOG( XFAIL, eError, "    ERROR: Node[%d] (%s) incorrectly supports deprecated KSPROPERTY_AUDIO_PEAKMETER",
                        pCNode->GetId(),
                        GetKSGuidName(*pCNode->GetType()) );
            }
        }
        else
#endif
        if(!foundProperty)
        {
            fRet = false;
            XLOG( XFAIL, eError, "   ERROR: Node[%d] (%s) does not support %s through the filter",
                    pCNode->GetId(),
                    GetKSGuidName(*pCNode->GetType()),
                    GetKSAudioPropertyName( pRequiredProperties[iIndex] ) );
        }
    }

    return fRet;
}
