// Copyright(C) Microsoft.All rights reserved.

#include "common.h"
#include <strsafe.h>


CPropDescReq::CPropDescReq()
{
    m_fIsFilterProperty  = false;
    m_iNumChannels       = 0;
    m_fIsChannelProperty = false;
    m_pDesc              = NULL;

    for(int i=0; i < MAX_NUM_PROPDESC; i++)
    {
        m_iPropDescReqList[AUDIO_SET][i] = INVALID;
    }
    for(int i=0; i < MAX_NUM_PROPDESC; i++)
    {
        m_iPropDescReqList[AUDIOENGINE_SET][i] = INVALID;
    }

    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_DEV_SPECIFIC] = UNKNOWN;

    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_AGC] = VT_BOOL;                      // BOOL
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_BASS_BOOST] = VT_BOOL;               // BOOL
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE] = VT_BOOL;    // BOOL
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_LOUDNESS] = VT_BOOL;                 // BOOL
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_MUTE] = VT_BOOL;                     // BOOL
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_SURROUND_ENCODE] = VT_BOOL;          // BOOL

    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_VOLUMELEVEL] = VT_I4;                // LONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_QUALITY] = VT_UI4;                   // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_SAMPLING_RATE] = VT_UI4;             // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_MUX_SOURCE] = VT_UI4;                // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_BASS] = VT_I4;                       // LONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_MID] = VT_I4;                        // LONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_TREBLE] = VT_I4;                     // LONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_NUM_EQ_BANDS] = VT_UI4;              // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_WIDENESS] = VT_UI4;                  // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_REVERB_LEVEL] = VT_UI4;              // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_CHORUS_LEVEL] = VT_UI4;              // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_DEMUX_DEST] = VT_UI4;                // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_CPU_RESOURCES] = VT_UI4;             // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY] = VT_I4;    // LONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_SURROUND_ENCODE] = VT_BOOL;          // BOOL
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_3D_INTERFACE] = VT_CLSID;            // GUID
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_PEAKMETER] = VT_I4;                  // LONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_ALGORITHM_INSTANCE] = VT_CLSID;      // GUID
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_FILTER_STATE] = VT_CLSID | VT_VECTOR; // GUID[]
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_PREFERRED_STATUS] = 0;               // KSAUDIO_PREFERRED_STATUS
#if (NTDDI_VERSION >= NTDDI_WIN8)
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_PEAKMETER2] = VT_I4;                 // LONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_POSITION] = VT_UI4; // ULONG
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_PRESENTATION_POSITION] = 0;          // KSAUDIO_PRESENTATION_POSITION
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_LINEAR_BUFFER_POSITION] = VT_UI8;    // ULONGLONG

    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_LFXENABLE] = VT_BOOL;          // BOOL
    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_GFXENABLE] = VT_BOOL;          // BOOL
    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_MIXFORMAT] = 0;                // KSDATAFORMAT
    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_DEVICEFORMAT] = 0;             // KSDATAFORMAT
    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS] = 0;   // KSDATAFORMAT
    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_DESCRIPTOR] =  0;              // KSAUDIOENGINE_DESCRIPTOR
    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_BUFFER_SIZE_RANGE] =  0;       // KSAUDIOENGINE_BUFFER_SIZE_RANGE
    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_LOOPBACK_PROTECTION] =  VT_UI4; // ULONG
    m_iPropDescReqList[AUDIOENGINE_SET][KSPROPERTY_AUDIOENGINE_VOLUMELEVEL] =  VT_I4;         // KSAUDIOENGINE_VOLUMELEVEL
#endif // #if (NTDDI_VERSION >= NTDDI_WIN8)

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
    m_iPropDescReqList[AUDIO_SET][KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_LASTBUFFER_POSITION] = VT_UI4; //ULONG
#endif //#if (NTDDI_VERSION >= NTDDI_WINBLUE)

}

inline void CPropDescReq::LogPropErr(LPCSTR type, LPCSTR message)
{
    XLOG(XFAIL, eError, "    %s:  Node[%02d] %s - Property[%02d] %s: %s", 
        type, m_iNode, GetKSGuidName(*(m_pNode->GetType())), m_iPropDesc,
        m_pPropDesc->GetPropertyName(),
        message);
}

bool CPropDescReq::CheckPropInValidNodesHelper(void *pArg)
{
    bool fResideInValidNode = false;
    GUID *nodeType = NULL;
    CPropDescReq *pThis = static_cast<CPropDescReq*>(pArg);

    // Return true since if it is not a property that is being checked for it should
    // not affect the result in any of the looping functions
    if( !IsEqualGUID(*(pThis->m_pPropDesc->GetPropertySet()),*(pThis->m_pPropertySetId)) ||
        pThis->m_pPropDesc->GetPropertyId() != pThis->m_iPropIDChecked)
    {
        return true;
    }

    nodeType = pThis->m_pNode->GetType();
    if( IsEqualGUID(*(pThis->m_pPropertySetId),KSPROPSETID_Audio) )
    {
        switch(pThis->m_iPropIDChecked)
        {
            case KSPROPERTY_AUDIO_AGC:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AGC, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_BASS:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_TONE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_BASS_BOOST:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_TONE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_CHORUS_LEVEL:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_CHORUS, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_CPU_RESOURCES:
                if ( pThis->m_fIsFilterProperty &&
                        (  IsEqualGUID(KSNODETYPE_ACOUSTIC_ECHO_CANCEL, *nodeType) 
                        || IsEqualGUID(KSNODETYPE_NOISE_SUPPRESS, *nodeType)
                        || IsEqualGUID(KSNODETYPE_PEAKMETER, *nodeType) ) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_DELAY:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_DELAY, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_DEMUX_DEST:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_DEMUX, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_DEV_SPECIFIC:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_DEV_SPECIFIC, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_DYNAMIC_RANGE:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_LOUDNESS, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE:
                if ( pThis->m_fIsFilterProperty && 
                        (  IsEqualGUID(KSNODETYPE_ADC, *nodeType) 
                        || IsEqualGUID(KSNODETYPE_DAC, *nodeType)
                        || IsEqualGUID(KSNODETYPE_SRC, *nodeType) ) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_EQ_BANDS:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_EQUALIZER, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_EQ_LEVEL:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_EQUALIZER, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_LOUDNESS:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_LOUDNESS, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_MID:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_TONE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_MIX_LEVEL_CAPS:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_SUPERMIX, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_MIX_LEVEL_TABLE:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_SUPERMIX, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_MUTE:
                if ( pThis->m_fIsFilterProperty &&
                        (  IsEqualGUID(KSNODETYPE_MUTE, *nodeType)
                        || IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) ) )
                {
                    fResideInValidNode = true;
                }
                else if ( !pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_MUX_SOURCE:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_MUX, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_NUM_EQ_BANDS:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_EQUALIZER, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_PEAKMETER:
#if (NTDDI_VERSION >= NTDDI_WIN8)
                pThis->LogPropErr("ERROR", "KSPROPERTY_AUDIO_PEAKMETER is deprecated as of Windows 8");
                fResideInValidNode = false;
#else //#if (NTDDI_VERSION >= NTDDI_WIN8)
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_PEAKMETER, *nodeType) )
                {
                    fResideInValidNode = true;
                }
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)
                break;
#if (NTDDI_VERSION >= NTDDI_WIN8)
            case KSPROPERTY_AUDIO_PEAKMETER2:
                if ( pThis->m_fIsFilterProperty &&
                        (  IsEqualGUID(KSNODETYPE_PEAKMETER, *nodeType)
                        || IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) ) )
                {
                    fResideInValidNode = true;
                }
                else if ( !pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)
            case KSPROPERTY_AUDIO_QUALITY:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_SRC, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_REVERB_LEVEL:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_REVERB, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_SAMPLING_RATE:
                if ( pThis->m_fIsFilterProperty &&
                        (  IsEqualGUID(KSNODETYPE_ADC, *nodeType)
                        || IsEqualGUID(KSNODETYPE_DAC, *nodeType)
                        || IsEqualGUID(KSNODETYPE_SRC, *nodeType) ) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY:
                if ( pThis->m_fIsFilterProperty &&
                        (  IsEqualGUID(KSNODETYPE_3D_EFFECTS, *nodeType) 
                        || IsEqualGUID(KSNODETYPE_DAC, *nodeType) ) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_TREBLE:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_TONE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_VOLUMELEVEL:
                if ( pThis->m_fIsFilterProperty &&
                        (  IsEqualGUID(KSNODETYPE_VOLUME, *nodeType)
                        || IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) ) )
                {
                    fResideInValidNode = true;
                }
                else if ( !pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_WIDENESS:
                if ( pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_STEREO_WIDE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
#if (NTDDI_VERSION >= NTDDI_WIN8)
            case KSPROPERTY_AUDIO_PRESENTATION_POSITION:
                if ( !pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_POSITION:
                if ( !pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
            case KSPROPERTY_AUDIO_LINEAR_BUFFER_POSITION:
                if ( !pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) )
                {
                    fResideInValidNode = true;
                }
                break;
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)
#if (NTDDI_VERSION >= NTDDI_WINBLUE)
            case KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_LASTBUFFER_POSITION:
                if ( !pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) )
                {
                    fResideInValidNode = TRUE;
                }
                break;
#endif //#if (NTDDI_VERSION >= NTDDI_WINBLUE)
            default:
                fResideInValidNode = false;
                pThis->LogPropErr("ERROR", "Invalid KSPROPERTY_Audio ID being checked for");
                break;

        }
    }
    else if( IsEqualGUID(*(pThis->m_pPropertySetId),KSPROPSETID_AudioEngine) )
    {
        switch(static_cast<UINT>(pThis->m_iPropIDChecked))
        {
        case KSPROPERTY_AUDIOENGINE_LFXENABLE:
        case KSPROPERTY_AUDIOENGINE_LOOPBACK_PROTECTION:
        case KSPROPERTY_AUDIOENGINE_VOLUMELEVEL:
            if ( !pThis->m_fIsFilterProperty && IsEqualGUID(KSNODETYPE_AUDIO_ENGINE, *nodeType) )
            {
                fResideInValidNode = true;
            }
            break;
        case KSPROPERTY_AUDIOENGINE_GFXENABLE:
        case KSPROPERTY_AUDIOENGINE_MIXFORMAT:
        case KSPROPERTY_AUDIOENGINE_DEVICEFORMAT:
        case KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS:
        case KSPROPERTY_AUDIOENGINE_DESCRIPTOR:
            if( pThis->m_fIsFilterProperty && IsEqualGUID( KSNODETYPE_AUDIO_ENGINE, *nodeType ) )
            {
                fResideInValidNode = true;
            }
            break;
        case KSPROPERTY_AUDIOENGINE_BUFFER_SIZE_RANGE:
            if( IsEqualGUID( KSNODETYPE_AUDIO_ENGINE, *nodeType ) )
            {
                fResideInValidNode = true;
            }
            break;
        default:
            fResideInValidNode = false;
            pThis->LogPropErr("ERROR", "Invalid KSPROPERTY_AudioEngine ID being checked for");
            break;
        }
    }

    // Make sure that the property is in a valid node type
    if(false == fResideInValidNode)
    {
        pThis->LogPropErr("FAIL", "Node must not support this property descriptor");
    }

    return (fResideInValidNode);
}

bool CPropDescReq::CheckPropAccessFlagsHelper1(void *pArg)
{
    bool                fRet = true;
    CPropDescReq        *pThis = static_cast<CPropDescReq*>(pArg);
    UINT                iPropID = pThis->m_pPropDesc->GetPropertyId();
    ULONG               ulFlags = pThis->m_pPropDesc->GetAccessFlags();


    // Check that the access ulFlags have valid values
    if( false == pThis->IsAccessFlagsValid(ulFlags))
    {
        pThis->LogPropErr("FAIL", "Invalid KSPROPERTY_DESCRIPTION.AccessFlags");
        return (false);
    }

    //Check that the specific property has correct flag settings
    if( IsEqualGUID(*(pThis->m_pPropDesc->GetPropertySet()),KSPROPSETID_Audio) )
    {
        switch(iPropID)
        {
        case KSPROPERTY_AUDIO_3D_INTERFACE:
        case KSPROPERTY_AUDIO_AGC:
        case KSPROPERTY_AUDIO_ALGORITHM_INSTANCE:
        case KSPROPERTY_AUDIO_BASS:
        case KSPROPERTY_AUDIO_BASS_BOOST:
        case KSPROPERTY_AUDIO_CHANNEL_CONFIG:
        case KSPROPERTY_AUDIO_CHORUS_LEVEL:
        case KSPROPERTY_AUDIO_COPY_PROTECTION:
        case KSPROPERTY_AUDIO_DELAY:
        case KSPROPERTY_AUDIO_DEMUX_DEST:
        case KSPROPERTY_AUDIO_DYNAMIC_RANGE:
        case KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE:
        case KSPROPERTY_AUDIO_EQ_LEVEL:
        case KSPROPERTY_AUDIO_LOUDNESS:
            //case KSPROPERTY_AUDIO_MANUFACTURE_GUID:
        case KSPROPERTY_AUDIO_MID:
        case KSPROPERTY_AUDIO_MIX_LEVEL_TABLE:
        case KSPROPERTY_AUDIO_MUTE:
        case KSPROPERTY_AUDIO_MUX_SOURCE:
        case KSPROPERTY_AUDIO_POSITION:
            //case KSPROPERTY_AUDIO_PRODUCT_GUID:
        case KSPROPERTY_AUDIO_QUALITY:
        case KSPROPERTY_AUDIO_REVERB_LEVEL:
        case KSPROPERTY_AUDIO_SAMPLING_RATE:
        case KSPROPERTY_AUDIO_STEREO_ENHANCE:
        case KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY:
        case KSPROPERTY_AUDIO_SURROUND_ENCODE:
        case KSPROPERTY_AUDIO_TREBLE:
        case KSPROPERTY_AUDIO_VOLUMELEVEL:
        case KSPROPERTY_AUDIO_WIDE_MODE:
        case KSPROPERTY_AUDIO_WIDENESS:
            return pThis->CheckPropAccessFlagsHelper2(ulFlags, FL_YES, FL_YES);

        case KSPROPERTY_AUDIO_DEV_SPECIFIC:
            return pThis->CheckPropAccessFlagsHelper2(ulFlags, FL_UNKNOWN, FL_UNKNOWN);


        case KSPROPERTY_AUDIO_CPU_RESOURCES:
        case KSPROPERTY_AUDIO_EQ_BANDS:
        case KSPROPERTY_AUDIO_FILTER_STATE:
        case KSPROPERTY_AUDIO_LATENCY:
        case KSPROPERTY_AUDIO_MIX_LEVEL_CAPS:
        case KSPROPERTY_AUDIO_NUM_EQ_BANDS:
        case KSPROPERTY_AUDIO_PEAKMETER:
#if (NTDDI_VERSION >= NTDDI_WIN8)
        case KSPROPERTY_AUDIO_PEAKMETER2:
        case KSPROPERTY_AUDIO_PRESENTATION_POSITION:
        case KSPROPERTY_AUDIO_LINEAR_BUFFER_POSITION:
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)
            return pThis->CheckPropAccessFlagsHelper2(ulFlags, FL_YES, FL_NO);
        case KSPROPERTY_AUDIO_PREFERRED_STATUS:
#if (NTDDI_VERSION >= NTDDI_WIN8)
        case KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_POSITION:
#endif //#if (NTDDI_VERSION >= NTDDI_WIN8)
#if (NTDDI_VERSION >= NTDDI_WINBLUE)
        case KSPROPERTY_AUDIO_WAVERT_CURRENT_WRITE_LASTBUFFER_POSITION:
#endif //#if (NTDDI_VERSION >= NTDDI_WINBLUE)
            return pThis->CheckPropAccessFlagsHelper2(ulFlags, FL_NO, FL_YES);
        default:
            pThis->LogPropErr("ERROR", "Invalid KSPROPERTY_Audio ID being checked for");
            fRet = false;
            break;
        }
    }
    else if( IsEqualGUID(*(pThis->m_pPropDesc->GetPropertySet()),KSPROPSETID_AudioEngine) )
    {
        switch(iPropID)
        {
        case KSPROPERTY_AUDIOENGINE_LFXENABLE:
        case KSPROPERTY_AUDIOENGINE_GFXENABLE:
        case KSPROPERTY_AUDIOENGINE_DEVICEFORMAT:
        case KSPROPERTY_AUDIOENGINE_VOLUMELEVEL:
            return pThis->CheckPropAccessFlagsHelper2(ulFlags, FL_YES, FL_YES);

        case KSPROPERTY_AUDIOENGINE_MIXFORMAT:
        case KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS:
        case KSPROPERTY_AUDIOENGINE_DESCRIPTOR:
        case KSPROPERTY_AUDIOENGINE_BUFFER_SIZE_RANGE:
            return pThis->CheckPropAccessFlagsHelper2(ulFlags, FL_YES, FL_NO);

        case KSPROPERTY_AUDIOENGINE_LOOPBACK_PROTECTION:
            return pThis->CheckPropAccessFlagsHelper2(ulFlags, FL_NO, FL_YES);

        default:
            pThis->LogPropErr("ERROR", "Invalid KSPROPERTY_AudioEngine ID being checked for");
            fRet = false;
            break;
        }
    }

    return (fRet);
}


bool CPropDescReq::CheckPropAccessFlagsHelper2(
    ULONG                   ulFlags,
    PROP_DESC_FLAG_SETTINGS ulGetFlag,
    PROP_DESC_FLAG_SETTINGS ulSetFlag)
{
    bool fRet = true;

    if(   (ulGetFlag == FL_YES)
        && ((ulFlags & KSMETHOD_TYPE_READ) != KSMETHOD_TYPE_READ ))
    {
        LogPropErr("FAIL", "Should have the KSMETHOD_TYPE_READ set for "
            "KSPROPERTY_DESCRIPTION.AccessFlags");
        fRet = false;
    }
    else if(   (ulGetFlag == FL_NO)
        && ((ulFlags & KSMETHOD_TYPE_READ) == KSMETHOD_TYPE_READ ))
    {
        LogPropErr("FAIL", "Should NOT have the KSMETHOD_TYPE_READ set for "
            "KSPROPERTY_DESCRIPTION.AccessFlags");
        fRet = false;
    }
    if(   (ulSetFlag == FL_YES)
        && ((ulFlags & KSMETHOD_TYPE_WRITE) != KSMETHOD_TYPE_WRITE ))
    {
        LogPropErr("FAIL", "Should have the KSMETHOD_TYPE_WRITE set for "
            "KSPROPERTY_DESCRIPTION.AccessFlags");
        fRet = false;
    }
    else if(   (ulSetFlag == FL_NO)
        && ((ulFlags & KSMETHOD_TYPE_WRITE) == KSMETHOD_TYPE_WRITE ))
    {
        LogPropErr("FAIL", "Should NOT have the KSMETHOD_TYPE_WRITE set for "
            "KSPROPERTY_DESCRIPTION.AccessFlags");
        fRet = false;
    }

    // Check that GET and SET work
    if(false == fRet)
    {
        if(   ((ulFlags & KSMETHOD_TYPE_READ) == KSMETHOD_TYPE_READ)
            && (m_pPropDesc->GetPropertyGetActual()==FALSE) )
        {
            LogPropErr("FAIL", "READ can not be performed");
            fRet = false;
        }
        if(   ((ulFlags & KSMETHOD_TYPE_WRITE) == KSMETHOD_TYPE_WRITE)
            && (m_pPropDesc->GetPropertySetActual()==FALSE) )
        {
            LogPropErr("FAIL", "WRITE can not be performed");
            fRet = false;
        }    
    }
    return fRet;
}

inline bool CPropDescReq::IsAccessFlagsValid(ULONG ulFlags)
{
    const ULONG  VALID_FLAGS_MASK  = 
        KSMETHOD_TYPE_NONE | KSMETHOD_TYPE_READ | KSMETHOD_TYPE_WRITE 
        | KSMETHOD_TYPE_MODIFY | KSMETHOD_TYPE_SOURCE | KSMETHOD_TYPE_SEND 
        | KSMETHOD_TYPE_SETSUPPORT | KSMETHOD_TYPE_BASICSUPPORT 
        | KSMETHOD_TYPE_TOPOLOGY;
    return ( (ulFlags & (~VALID_FLAGS_MASK)) == 0 );
}

bool CPropDescReq::CheckPropDescHeadersHelper(void *pArg)
{
    CPropDescReq               *pThis = static_cast<CPropDescReq *>(pArg);
    bool                        fRet = true;    
    HANDLE                      hTargetHandle = INVALID_HANDLE_VALUE;
    ULONG                       cbReturned = 0;
    KSP_NODE                    ksp = {0};
    KSPROPERTY_DESCRIPTION      desc = {0};
    UINT                        ii = 0, OutputBuffSize[] = {0, 1, 2, 6, 8};
    CPinInstance                *pPinInstance = NULL;



    if( pThis->m_fIsFilterProperty )
    {
        hTargetHandle = pThis->m_pFilter->GetHandle();        
    }
    else
    {
        pPinInstance = pThis->m_pNode->GetPinInstance();
        hTargetHandle = pPinInstance->GetPinHandle();
    }

    if( NULL == hTargetHandle || INVALID_HANDLE_VALUE == hTargetHandle )
    {
        pThis->LogPropErr("ERROR", "Invalid handle");
        fRet = false;
        goto Exit;		
    }


    desc.AccessFlags = KSMETHOD_TYPE_NONE;
    desc.DescriptionSize = 0xFA1DCE;       // A random test value
    desc.MembersListCount = 205;            // A random test value

    // Initialize KSP_NODE for support query
    ksp.NodeId = pThis->m_iNode;
    ksp.Property.Set = *(pThis->m_pPropDesc->GetPropertySet());
    ksp.Property.Id = pThis->m_pPropDesc->GetPropertyId();
    ksp.Property.Flags = KSPROPERTY_TYPE_BASICSUPPORT | KSPROPERTY_TYPE_TOPOLOGY;

    // Make sure if the size specified is not one of the following standard values,
    // ERROR_INSUFFICIENT_BUFFER is returned. Possible values are:
    //  1. sizeof(ULONG)
    //  2. sizeof(KSPROPERTY_DESCRIPTION)
    //  3. sizeof(KSPROPERTY_DESCRIPTION) + SUM(0: MembersListCount-1)
    //     (sizeof(KSPROPERTY_MEMBERSHEADER) + (pMemHeader->MembersCount * pMemHeader->MembersSize))
    for(ii = 0; ii < ARRAYSIZE(OutputBuffSize); ++ii)
    {
        SLOG(eBlab2, "    IOCTL called with an output buffer size "
            "of %d.", OutputBuffSize[ii]);
#pragma prefast(suppress:__WARNING_POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY, "Intentionally sending an invalid buffer size for property handling validation.")
        if ( DeviceIoControl(hTargetHandle, IOCTL_KS_PROPERTY, &ksp, sizeof(ksp), 
            &desc, OutputBuffSize[ii], &cbReturned, NULL)
            || (GetLastError() != ERROR_INSUFFICIENT_BUFFER) )
        {
            char buffer[1000] = {0};
            _snprintf_s(buffer, sizeof(buffer)/sizeof(buffer[0]), 
                sizeof(buffer)/sizeof(buffer[0])-1, 
                "DeviceIoControl must return ERROR_INSUFFICIENT_BUFFER.\n"
                "            Error returned 0x%X.", GetLastError());
            buffer[sizeof(buffer)/sizeof(buffer[0])-1] = 0;
            pThis->LogPropErr("FAIL", buffer);
            fRet = false;
        }
    }

    // Perform a get with the output buffer the size of ULONG 
    // and see it does not write on more memory than the size specified
    if (!DeviceIoControl(hTargetHandle, IOCTL_KS_PROPERTY, &ksp, sizeof(ksp), &desc, sizeof(ULONG), &cbReturned, NULL))
    {
        pThis->LogPropErr("FAIL", "IOCTL failed for this property with "
            "output size of sizeof(ULONG)");
        SLOG(eBlab2, "          %s Handle: 0x%p - Error 0x%X from DeviceIoControl",
            (pThis->m_fIsFilterProperty ? "Filter" : "Pin"), hTargetHandle, GetLastError());
        fRet = false;
    }
    if( (desc.DescriptionSize != 0xFA1DCE) || (desc.MembersListCount != 205) )
    {
        pThis->LogPropErr("FAIL", "IOCTL performed accesses memory larger than "
            "the size of the specified output buffer "
            "sizeof(ULONG)");
        fRet = false;        
    }

    // Perform a Get with the output buffer the size of KSPROPERTY_DESCRIPTION 
    // Get the size and basic support info and make sure they are valid
    if (!DeviceIoControl(hTargetHandle, IOCTL_KS_PROPERTY, &ksp, sizeof(ksp), &desc, sizeof(desc), &cbReturned, NULL))
    {
        pThis->LogPropErr("FAIL", "Unable to to perform IOCTL for this property");
        SLOG(eBlab2, "          %s Handle: 0x%p - Result of HrIoctl: 0x%X",
            (pThis->m_fIsFilterProperty ? "Filter" : "Pin"), hTargetHandle, GetLastError());

        fRet = false;
        goto Exit;

    }
    if(!pThis->HasValidPropDescStruct(&desc))
    {
        fRet = false;
        goto Exit;

    }

    // Allocate memory for full information
    if(NULL != pThis->m_pDesc)
    {
        delete[] pThis->m_pDesc;
        pThis->m_pDesc = NULL;
    }
    pThis->m_pDesc = (PKSPROPERTY_DESCRIPTION)(new unsigned char[desc.DescriptionSize]);
    if(NULL == pThis->m_pDesc)
    {
        XLOG(XFAIL, eError, "   ERROR: Unable to allocate memory for full property descriptor with the required size %d bytes", 
            desc.DescriptionSize);
        fRet = false;
        goto Exit;

    }
    // Get the full information
    if (!DeviceIoControl(hTargetHandle, IOCTL_KS_PROPERTY, &ksp, sizeof(ksp), pThis->m_pDesc, desc.DescriptionSize, &cbReturned, NULL))
    {
        pThis->LogPropErr("FAIL", "Unable to to perform IOCTL for this property");
        SLOG(eBlab2, "          %s Handle: 0x%p - Result of HrIoctl: 0x%X",
            (pThis->m_fIsFilterProperty ? "Filter" : "Pin"), hTargetHandle, GetLastError());
        fRet = false;
    }
    else if(!pThis->HasValidPropDescStruct(pThis->m_pDesc))
    {
        fRet = false;
    }
    else if(!pThis->HasValidMemHeaderStruct(pThis->m_pDesc))
    {
        fRet = false;
        goto Exit;		
    }
Exit:
    return fRet;
}


// -----------------------------------------------------------------------------
// Name:    CPropDescReq::HasValidPropDescStruct
// Desc:    This function checks the validity of the KSPROPERTY_DESCRIPTION
//          structure, making sure that each member has a valid value or range.
//
//          KSPROPERTY_DESCRIPTION.AccessFlags is NOT checked here because
//          it has its own test case
// -----------------------------------------------------------------------------
bool CPropDescReq::HasValidPropDescStruct(KSPROPERTY_DESCRIPTION *pDesc)
{
    bool    fRet = true;

    // Check that KSPROPERTY_DESCRIPTION.DescriptionSize is valid
    if(    (pDesc->DescriptionSize < sizeof(KSPROPERTY_DESCRIPTION)) 
        || (pDesc->DescriptionSize > MAX_PROPERTY_DESCRIPTION_SIZE) )
    {
        LogPropErr("FAIL", "Size of KSPROPERTY_DESCRIPTION.DescriptionSize "
            "is invalid");
        fRet = false;
    }

    // Check property has a valid KSPROPERTY_DESCRIPTION.PropTypeSet
    if(!HasValidPropTypeSet(pDesc->PropTypeSet))
    {
        fRet = false;
    }

    // Check that KSPROPERTY_DESCRIPTION.MembersListCount is valid
    if(!HasValidMembersListCount(pDesc->MembersListCount)) 
    {
        fRet =  false;
    }

    // Check that KSPROPERTY_DESCRIPTION.Reserved is actually zero
    if(pDesc->Reserved != 0)
    {
        LogPropErr("FAIL", "KSPROPERTY_DESCRIPTION.Reserved is not Zero");
        fRet = false;
    }

    return fRet;
}

bool CPropDescReq::HasValidMembersListCount(UINT iMemListCount)
{
    bool                fRet = true;
    char                buffer[16] = {0};
    KSPROPERTY_AUDIO    nProperty 
        = static_cast<KSPROPERTY_AUDIO>(m_pPropDesc->GetPropertyId());

    // Check if the property is a channel property
    if( IsEqualGUID(*(m_pPropDesc->GetPropertySet()), KSPROPSETID_Audio) )
    {
        switch (nProperty)
        {
        case KSPROPERTY_AUDIO_AGC:
        case KSPROPERTY_AUDIO_BASS:
        case KSPROPERTY_AUDIO_BASS_BOOST:
        case KSPROPERTY_AUDIO_DELAY:
        case KSPROPERTY_AUDIO_EQ_BANDS:
        case KSPROPERTY_AUDIO_EQ_LEVEL:
        case KSPROPERTY_AUDIO_LOUDNESS:
        case KSPROPERTY_AUDIO_MID:
        case KSPROPERTY_AUDIO_MUTE:
        case KSPROPERTY_AUDIO_NUM_EQ_BANDS:
        case KSPROPERTY_AUDIO_PEAKMETER:
#if NTDDI_VERSION >= NTDDI_WIN8
        case KSPROPERTY_AUDIO_PEAKMETER2:
#endif
        case KSPROPERTY_AUDIO_TREBLE:
        case KSPROPERTY_AUDIO_VOLUMELEVEL:
            m_fIsChannelProperty = true;
            m_iNumChannels = 0;
            // Count number of channels
            while(m_pPropDesc->GetValue(static_cast<void*>(buffer), m_iNumChannels))
            {
                ++m_iNumChannels;
                if(m_iNumChannels > MAXIMUM_NUMBER_OF_CHANNELS)
                {
                    m_iNumChannels = 0;
                    break;
                }
            }

            break;
        default:
            m_fIsChannelProperty = false;
            m_iNumChannels = 0;
            break;
        }
    }
    else if( IsEqualGUID(*(m_pPropDesc->GetPropertySet()), KSPROPSETID_AudioEngine ) )
    {
        m_fIsChannelProperty = false;
        m_iNumChannels = 0;
    }

    // If a property is a channel property or it has more than 0 channels
    // then KSPROPERTY_DESCRIPTION.MembersListCount must be 1.
    if( (m_fIsChannelProperty || (m_iNumChannels > 0)) && (iMemListCount != 1))
    {
        char message[200];
        _snprintf_s(message, sizeof(message)/sizeof(message[0]), 
            sizeof(message)/sizeof(message[0]) - 1,
            "Invalid value of KSPROPERTY_DESCRIPTION.Members"
            "ListCount: %d; must be 1", iMemListCount);
        message[sizeof(message)/sizeof(message[0])-1] = 0;
        LogPropErr("FAIL", message);
        fRet = false;
    }

    return fRet;
}

bool CPropDescReq::HasValidPropTypeSet(KSIDENTIFIER KSIDPropTypeSet)
{
    bool    fRet = true;
    UINT    index = ( IsEqualGUID(*(m_pPropDesc->GetPropertySet()), KSPROPSETID_Audio) ?
AUDIO_SET : AUDIOENGINE_SET );

    // PropTypeSet.Set = GUID_NULL
    if(IsEqualGUID(KSIDPropTypeSet.Set, GUID_NULL))
    {
        if(KSIDPropTypeSet.Id != 0)
        {
            LogPropErr("FAIL", "KSPROPERTY_DESCRIPTION.PropTypeSet.Set is GUID_NULL; "
                "Id should be 0");
            fRet = false;
        }
    }
    // PropTypeSet.Set = GUID_GENERAL
    else if(IsEqualGUID(KSIDPropTypeSet.Set, KSPROPTYPESETID_General))
    {
        // Check if this Property Description is implemented
        // If so make sure that the required type set in MSDN matches the type
        //  defined in KSPROPERTY_DESCRIPTION.PropTypeSet.Id
        switch(m_iPropDescReqList[index][m_pPropDesc->GetPropertyId()])
        {
        case INVALID:
            LogPropErr("FAIL", "KSPROPERTY_DESCRIPTION.PropTypeSet.Set "
                "should be GUID_NULL OR Invalid Audio"
                " Property Description ID");
            fRet = false;
            break;
        case UNKNOWN:
            switch (KSIDPropTypeSet.Id)
            {
            case VT_BOOL:
            case VT_I4:
            case VT_UI4:
            case VT_I8:
            case VT_UI8:
                break;
            default:
                LogPropErr("FAIL", "KSPROPERTY_DESCRIPTION.PropTypeSet"
                    ".Id has an invalid type set. The "
                    "possible values are  VT_BOOL, "
                    "VT_I4, VT_UI4");
                fRet = false;
                break;
            }
            break;
        default:
            if(m_iPropDescReqList[index][m_pPropDesc->GetPropertyId()] != KSIDPropTypeSet.Id)
            {
                std::string ss("");

                ss += "There is a mismatch between the ";
                ss += "property set in KSPROPERTY_DESCRIPTION.";
                ss += "PropTypeSet.Id(";
                ss += PropTypeToString(KSIDPropTypeSet.Id);
                ss += ") and what the type should be set to (";
                ss += PropTypeToString(m_iPropDescReqList[index][m_pPropDesc->GetPropertyId()]);
                ss += ")";

                LogPropErr("FAIL", ss.c_str());
                fRet = false;                    
            }
            break;
        }
    }
    else
    {
        LogPropErr("FAIL", "KSPROPERTY_DESCRIPTION.PropTypeSet.Set "
            "should be GUID_NULL or KSPROPTYPESETID_General");
        fRet = false;
    }

    return fRet;
}

bool CPropDescReq::HasValidMemHeaderStruct(KSPROPERTY_DESCRIPTION *pDesc)
{
    bool                        fRet = true;
    UINT                        iExpDescSize = sizeof(KSPROPERTY_DESCRIPTION);
    UINT                        iTempCount = 0, i=0;
    KSPROPERTY_MEMBERSHEADER   *pMemHeader = NULL; 

    pMemHeader = (KSPROPERTY_MEMBERSHEADER *)
        ((char *)pDesc + sizeof(KSPROPERTY_DESCRIPTION));

    for(i=0; i < pDesc->MembersListCount; i++)
    {
        // Track KSPROPERTY_MEMBERSHEADER.MembersSize and KSPROPERTY_MEMBERSHEADER.MembersCount
        iTempCount = sizeof(KSPROPERTY_MEMBERSHEADER) + 
            (pMemHeader->MembersCount * pMemHeader->MembersSize);
        iExpDescSize += iTempCount;

        //Check KSPROPERTY_MEMBERSHEADER.MembersFlags values
        if(!IsMembersHeaderMembersFlagsValid(pMemHeader->MembersFlags))
        {
            LogPropErr("FAIL", "Value of KSPROPERTY_MEMBERSHEADER.MembersFlags is invalid");
            fRet = false;
        }

        //Check KSPROPERTY_MEMBERSHEADER.Flags values
        if(!IsMembersHeaderFlagsValid(pMemHeader->Flags, pMemHeader->MembersCount))
        {
            fRet = false;
        }

        // Get pointer to next KSPROPERTY_MEMBERSHEADER
        pMemHeader = (KSPROPERTY_MEMBERSHEADER *) ((char *)pMemHeader + iTempCount);
    }

    // Check KSPROPERTY_MEMBERSHEADER.MembersSize and KSPROPERTY_MEMBERSHEADER.MembersCount
    if(iExpDescSize != pDesc->DescriptionSize)
    {
        LogPropErr("FAIL", "Value of KSPROPERTY_DESCRIPTION.MembersListCount and"
            " KSPROPERTY_DESCRIPTION.DescriptionSize do not correlate");
        XLOG(XFAIL, eError, "         MemberListCount = %d - Expected Size = %d - DescSize: %d", 
            pDesc->MembersListCount, iExpDescSize, pDesc->DescriptionSize);
        fRet = false;
    }

    return fRet;
}

inline bool CPropDescReq::IsMembersHeaderMembersFlagsValid(ULONG ulFlags)
{
    // KSPROPERTY_MEMBER_VALUES is not valid with current model of
    // representing channels
    if(   ((ulFlags & KSPROPERTY_MEMBER_RANGES) == KSPROPERTY_MEMBER_RANGES)
        || ((ulFlags & KSPROPERTY_MEMBER_STEPPEDRANGES) == KSPROPERTY_MEMBER_STEPPEDRANGES))
    {
        return true;    
    }
    return false;
}

bool CPropDescReq::IsMembersHeaderFlagsValid(ULONG ulFlags, UINT iMembersCount)
{
    bool fRet = true;
    const ULONG VALID_FLAGS_MASK = 
        KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL
        | KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM
        | KSPROPERTY_MEMBER_FLAG_DEFAULT;
    UINT i=0;
    char *buffer[16];

    // Check that only valid flags are set
    if((ulFlags & (~VALID_FLAGS_MASK)) != 0)
    {
        LogPropErr("FAIL", "Invalid values of KSPROPERTY_MEMBERSHEADER.Flags");
        return false;
    }

    // Check that if there is more than one channel, or if the property is a
    // channel property, the the multichannel flag must be set
    if(m_fIsChannelProperty || m_iNumChannels > 0)
    {
        if((ulFlags & KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL) 
            != KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL)
        {
            LogPropErr("FAIL", "KSPROPERTY_MEMBERSHEADER.Flags is a channel "
                "property or has more than one channel and therefore must have"
                "KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL set");
            return false;
        }

        if((m_iNumChannels != iMembersCount) || (0==iMembersCount))
        {
            LogPropErr("FAIL", "KSPROPERTY_MEMBERSHEADER.Flags has "
                "KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL "
                "set and must return the number of channels in "
                "KSPROPERTY_MEMBERSHEADER.MembersCount");
            return false;
        }
    }

    // Check that if UNIFORM flag is set that only the zero channel can be set
    // and that multichannel is set also
    if((ulFlags & KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM) 
        == KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM)
    {
        if((ulFlags & KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL) 
            != KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL)
        {
            LogPropErr("FAIL", "KSPROPERTY_MEMBERSHEADER.Flags has the "
                "KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM flag set and must "
                "have KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL flag "
                "also set. Uniform flag applies only to channel properties.");
            fRet = false;
        }

        for(i=0; i < m_iNumChannels; ++i)
        {
            if( (i == 0) && !m_pPropDesc->GetValue(static_cast<void*>(buffer),i))
            {
                LogPropErr("FAIL", "KSPROPERTY_MEMBERSHEADER.Flags has the "
                    "KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM flag set"
                    "and can not GET the value of channel 0.");
                fRet = false;
                break;
            }
            else if(m_pPropDesc->GetValue(static_cast<void*>(buffer),i))
            {
                LogPropErr("FAIL", "KSPROPERTY_MEMBERSHEADER.Flags has the "
                    "KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_UNIFORM flag set"
                    "and allows GET for channels greater than zero.");
                fRet = false;
                break;
            }
        }
    }

    return fRet;
}



bool CPropDescReq::CheckPropGetSetRangesHelper(void *pArg)
{
    bool                          fRet    = true;
    CPropDescReq                 *pThis   = static_cast<CPropDescReq *>(pArg);
    ULONG                         ulFlags = pThis->m_pPropDesc->GetAccessFlags();
    KSPROPERTY_MEMBERSHEADER     *pKsMemHead;
    KSPROPERTY_BOUNDS_LONG       *bounds1;
    KSPROPERTY_BOUNDS_LONGLONG   *bounds2;
    KSPROPERTY_STEPPING_LONG     *stepping1;
    KSPROPERTY_STEPPING_LONGLONG *stepping2;

    // Make sure that the access flags and KSPROPERTY_DESCRIPTION are 
    // valid before checking for valid set/get values
    if(!CheckPropAccessFlagsHelper1(pArg) || 
       !CheckPropDescHeadersHelper(pArg))
    {
        fRet = false;
        goto Exit;
    }

    // Check to see that it is a channel property
    if(!pThis->m_fIsChannelProperty)
    {
        goto Exit;
    }
    // Check to see if there is read and write access for
    // this property
    if(((ulFlags & KSMETHOD_TYPE_READ)  != KSMETHOD_TYPE_READ)
       ||((ulFlags & KSMETHOD_TYPE_WRITE) != KSMETHOD_TYPE_WRITE))

    {
        goto Exit;
    }

    // m_pDesc is populated from the previous call to CheckPropDescHeadersHelper
    // and will be freed either when CheckPropDescHeadersHelper is called again
    // or when the class destructor is called
    pKsMemHead = (KSPROPERTY_MEMBERSHEADER *)
        ((char *)(pThis->m_pDesc) + sizeof(KSPROPERTY_DESCRIPTION));

    for(UINT ii = 0; ii < pKsMemHead->MembersCount; ++ii)
    {
        if((pKsMemHead->MembersFlags & KSPROPERTY_MEMBER_RANGES)
            == KSPROPERTY_MEMBER_RANGES)
        {
            if(pKsMemHead->MembersSize == sizeof(KSPROPERTY_BOUNDS_LONG))
            {
                bounds1 = (KSPROPERTY_BOUNDS_LONG*)
                    ((char*)(pKsMemHead) + sizeof(KSPROPERTY_MEMBERSHEADER) 
                    + ii*pKsMemHead->MembersSize);
                if(!pThis->IsValueInRange<LONG, ULONG>(
                    bounds1->SignedMinimum, 
                    bounds1->SignedMaximum, 1, ii))
                {
                    fRet = false;
                    break;
                }
            }
            else if(pKsMemHead->MembersSize == sizeof(KSPROPERTY_BOUNDS_LONGLONG))
            {
                bounds2 = (KSPROPERTY_BOUNDS_LONGLONG*)
                    ((char*)(pKsMemHead) + sizeof(KSPROPERTY_MEMBERSHEADER) 
                    + ii*pKsMemHead->MembersSize);
                if(!pThis->IsValueInRange<LONGLONG, ULONGLONG>(
                    bounds2->SignedMinimum, 
                    bounds2->SignedMaximum, 1, ii))
                {
                    fRet = false;
                    break;
                }
            }
            else
            {
                pThis->LogPropErr("FAIL", "Size of KSPROPERTY_MEMBERSHEADER."
                    "MembersSize does not match the expected "
                    "sizes");
                fRet = false;
            }
        }
        else if((pKsMemHead->MembersFlags & KSPROPERTY_MEMBER_STEPPEDRANGES)
            == KSPROPERTY_MEMBER_STEPPEDRANGES)
        {
            if(pKsMemHead->MembersSize == sizeof(KSPROPERTY_STEPPING_LONG))
            {
                stepping1 = (KSPROPERTY_STEPPING_LONG*)
                    ((char*)(pKsMemHead) + sizeof(KSPROPERTY_MEMBERSHEADER) 
                    + ii*pKsMemHead->MembersSize);
                if(!pThis->IsValueInRange<LONG, ULONG>(
                    stepping1->Bounds.SignedMinimum,
                    stepping1->Bounds.SignedMaximum,
                    stepping1->SteppingDelta, ii))
                {
                    fRet = false;
                }
            }
            else if(pKsMemHead->MembersSize == sizeof(KSPROPERTY_STEPPING_LONGLONG))
            {
                stepping2 = (KSPROPERTY_STEPPING_LONGLONG*)
                    ((char*)(pKsMemHead) + sizeof(KSPROPERTY_MEMBERSHEADER) 
                    + ii*pKsMemHead->MembersSize);            
                if(!pThis->IsValueInRange<LONGLONG, ULONGLONG>(
                    stepping2->Bounds.SignedMinimum,
                    stepping2->Bounds.SignedMaximum,
                    stepping2->SteppingDelta, ii))
                {
                    fRet = false;
                    break;
                }

            }
            else
            {
                pThis->LogPropErr("FAIL", "Size of KSPROPERTY_MEMBERSHEADER."
                    "MembersSize does not match the expected "
                    "sizes");
                fRet = false;
                break;
            }
        }
        else
        {
            pThis->LogPropErr("FAIL", "KSPROPERTY_MEMBERSHEADER.Members flag set to"
                " an invalid value.");
            fRet = false;
            break;
        }
    }

Exit:
    return fRet;
}

template <typename T, typename X>
bool CPropDescReq::IsValueInRange(T Min, T Max, X Step, UINT Channel)
{
    bool fRet = true;
    T valueS=0, valueR=0;
    std::ostrstream ss;
    ss.sync_with_stdio();

    if(Min >= Max)

    {
        ss<<"Channel[ "<<Channel<<"]: Min: "<<Min<< " ,Max: "<<Max;
        ss<<", Step: "<<Step<<". Min must be less than Max to allow for different value settings."<<std::ends;
        LogPropErr("FAIL", ss.str());
        fRet = false;
    }
    else if(0 == Step)
    {
        ss<<"Channel[ "<<Channel<<"]: Min: "<<Min<< " ,Max: "<<Max;
        ss<<", Step: "<<Step<<". Step must be greater than zero to allow";
        ss<<" for different value settings."<<std::ends;
        LogPropErr("FAIL", ss.str());
        fRet = false;
    }
    else
    {
        // Set to random invalid value outside of range
        for(int i=0; i<10; ++i)
        {
            valueS = ((i%2==0) ? (Max + rand()%1000) : (Min - rand()%1000));
            valueR = 0;
            if (!m_pPropDesc->SetValue(&valueS, Channel))
            {
                // it's perfectly OK to reject an invalid value
                continue;
            }
            else if (!m_pPropDesc->GetValue(&valueR, Channel))
            {
                // reading should always succeed, though
                ss<<"Channel[ "<<Channel<<"]: Min="<<Min<< ", Max="<<Max<<", ";
                ss<<"Step="<<Step<<". ValueSet="<<valueS<<". ";
                ss<<"Reading value failed. GetLastError() = "<<GetLastError()<<std::ends; 
                LogPropErr("FAIL", ss.str());
                fRet = false;
            }
            else if((valueR < Min) || (valueR > Max))
            {
                // and the value should be clipped
                ss<<"Channel[ "<<Channel<<"]: Min="<<Min<< ", Max="<<Max<<", ";
                ss<<"Step="<<Step<<". ValueSet="<<valueS<<", ValueRead="<<valueR<<". ";
                ss<<"Value Read is outside the valid range of values."<<std::ends; 
                LogPropErr("FAIL", ss.str());
                fRet = false;
            }
        }

        if(Step > 1)
        {
            for(int i=0; i<5; ++i)
            {
                // Any invalid value
                do
                {
                    valueS = (((Min + Max)/2) + (rand()%Step));
                }while(0 == ((valueS-Min)%Step));
                valueR = 0;
                if (!m_pPropDesc->SetValue(&valueS, Channel))
                {
                    // setting an invalid value can be rejected
                    continue;
                }
                else if (!m_pPropDesc->GetValue(&valueR, Channel))
                {
                    // reading should always work
                    ss<<"Channel[ "<<Channel<<"]: Min="<<Min<< ", Max="<<Max<<", ";
                    ss<<"Step="<<Step<<". ValueSet="<<valueS<<". ";
                    ss<<"Reading value failed. GetLastError() = "<<GetLastError()<<std::ends; 
                    LogPropErr("FAIL", ss.str());
                    fRet = false;
                }
                else if((valueR < Min) || (valueR > Max))
                {
                    // and the value read should be valid
                    ss<<"Channel[ "<<Channel<<"]: Min="<<Min<< ", Max="<<Max<<", ";
                    ss<<"Step="<<Step<<". ValueSet="<<valueS<<", ValueRead="<<valueR<<". ";
                    ss<<"Value Read is outside the valid range of values."<<std::ends; 
                    LogPropErr("FAIL", ss.str());
                    fRet = false;
                }  
                else 
                {
                    // and reasonably close to what we set
                    X ReadStep = (valueS - Min) / Step;
                    T LowerBound = Min + ReadStep * Step;
                    T UpperBound = Min + (ReadStep + 1) * Step;
                    UpperBound = (Max < UpperBound) ? Max : UpperBound;

                    // Allowed return values:
                    //  1. Exact cached value
                    //  2. If it was set to a step, then return same step
                    //  3. If it was set between steps, return either Upper or Lower bound
                    bool fSuccess = 
                        ((valueR == valueS) ||
                        (valueR == LowerBound) ||
                        ((valueS != LowerBound) && (valueR == UpperBound)));
                    if(!fSuccess)
                    {
                        ss<<"Channel[ "<<Channel<<"]: Min="<<Min<< ", Max="<<Max<<", ";
                        ss<<"Step="<<Step<<". ValueSet="<<valueS<<", ValueRead="<<valueR<<". ";
                        ss<<"Value Read is not a valid stepping value."<<std::ends; 
                        LogPropErr("FAIL", ss.str());
                        fRet = false;
                    }
                }
            }
        }
    }    
    return fRet;
}

std::string PropTypeToString(UINT PropTypeID)
{
    switch(PropTypeID)
    {
    case VT_BOOL:
        return "VT_BOOL";
        break;
    case VT_I4:
        return "VT_I4";
    case VT_UI4:
        return "VT_UI4";
    case VT_I8:
        return "VT_I8";
    case VT_UI8:
        return "VT_UI8";
    default:
        char ss[10];
        StringCbPrintf( ss, sizeof(ss), "%d", PropTypeID);
        return ss;
    }

}
