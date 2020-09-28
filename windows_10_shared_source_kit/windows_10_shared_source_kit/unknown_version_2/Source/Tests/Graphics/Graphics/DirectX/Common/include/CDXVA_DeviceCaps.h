#ifndef __CDXVA_DEVICE_CAPS_H__
#define __CDXVA_DEVICE_CAPS_H__

#ifndef SAFE_DELETE_CVPCAPS_ARRAY
#define SAFE_DELETE_CVPCAPS_ARRAY(ptr, count)\
    if(ptr) \
    { \
        for (UINT uiPtrCountIdx = 0; uiPtrCountIdx != count; uiPtrCountIdx++) \
        { \
            SAFE_DELETE_ARRAY(ptr[uiPtrCountIdx].pCustomRates); \
        } \
        SAFE_DELETE_ARRAY(ptr); \
    }
#endif

typedef enum _CDXVA_FORMAT_TYPE
{
    CDXVA_FORMAT_TYPE_INPUT  = 0,
    CDXVA_FORMAT_TYPE_OUTPUT = 1
} CDXVA_FORMAT_TYPE;

typedef enum _CDXVA_CAPS_TYPE
{
    CDXVA_CAPS_TYPE_DXVA2_VP   = 0,
    CDXVA_CAPS_TYPE_DXVA2_VD   = 1,
    CDXVA_CAPS_TYPE_DXVAHD_VP  = 2,
    CDXVA_CAPS_TYPE_D3D11_VP   = 3
} CDXVA_CAPS_TYPE;

typedef struct _CDXVAHD_VPCAPSEX
{
    DXVAHD_VPCAPS           VPCaps;
    DXVAHD_CUSTOM_RATE_DATA *pCustomRates;   
} CDXVAHD_VPCAPSEX;

typedef struct _CD3D11_VP_CAPS
{
    D3D11_VIDEO_PROCESSOR_CAPS                  VPCaps;
    D3D11_VIDEO_PROCESSOR_RATE_CONVERSION_CAPS  *pCustomRates;
} CD3D11_VP_CAPS;

const DXGI_FORMAT DXGIFormats[] =
{
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,

    DXGI_FORMAT_B8G8R8X8_UNORM,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,

    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,

    DXGI_FORMAT_R16G16B16A16_FLOAT,

    DXGI_FORMAT_R10G10B10A2_UNORM,

    DXGI_FORMAT_YUY2,
    DXGI_FORMAT_NV12,
    DXGI_FORMAT_AYUV,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT
};
const UINT DXGI_FORMAT_COUNT = (sizeof(DXGIFormats) / sizeof(DXGI_FORMAT));


typedef struct _CDXVA2_RTCAPSEX
{
    D3DFORMAT                   Format;
    DXVA2_VideoProcessorCaps    VPCaps;
    DXVA2_ValueRange            BrightnessRange;
    DXVA2_ValueRange            ContrastRange;
    DXVA2_ValueRange            HueRange;
    DXVA2_ValueRange            SaturationRange;
    DXVA2_ValueRange            NoiseFilterLumaLevel;
    DXVA2_ValueRange            NoiseFilterLumaThreshold;
    DXVA2_ValueRange            NoiseFilterLumaRadius;
    DXVA2_ValueRange            NoiseFilterChromaLevel;
    DXVA2_ValueRange            NoiseFilterChromaThreshold;
    DXVA2_ValueRange            NoiseFilterChromaRadius;
    DXVA2_ValueRange            DetailFilterLumaLevel;
    DXVA2_ValueRange            DetailFilterLumaThreshold;
    DXVA2_ValueRange            DetailFilterLumaRadius;
    DXVA2_ValueRange            DetailFilterChromaLevel;
    DXVA2_ValueRange            DetailFilterChromaThreshold;
    DXVA2_ValueRange            DetailFilterChromaRadius;
} CDXVA2_RTCAPSEX;

typedef struct _CDXVA2_VPCAPSEX
{
    GUID            Guid;
    UINT            uiInputFormatCount;
    D3DFORMAT       *pInputFormats;
    CDXVA2_RTCAPSEX *pRTCapsEx;
} CDXVA2_VPCAPSEX;

typedef struct _CDXVA2_VDCAPSEX
{
    GUID                        Guid;
    DXVA2_ConfigPictureDecode   *pConfigs;
    UINT                        uiConfigCount;
    D3DFORMAT                   *pFormats;
    UINT                        uiFormatCount;
} CDXVA2_VDCAPSEX;

//
// Operator implementations.
//
inline BOOL operator == (const CDXVAHD_VPCAPSEX& x, const CDXVAHD_VPCAPSEX& y)
{
    if (x.VPCaps != y.VPCaps) return FALSE;

    for(UINT uiCustomRateIdx = 0; uiCustomRateIdx != x.VPCaps.CustomRateCount; uiCustomRateIdx++){
        if (x.pCustomRates[uiCustomRateIdx] != y.pCustomRates[uiCustomRateIdx] ) return FALSE;
    }

    return TRUE;
}
inline BOOL operator != (const CDXVAHD_VPCAPSEX& x, const CDXVAHD_VPCAPSEX& y)
{
    return !(x == y);
}

//
// NOTE:
// This really needs to be converted to template for the different caps types 
// but since there is only three types right now leaving it broken by case is 
// fine for now. If more types are added to D3D11 this will need to be converted
//
class CDXVA_DeviceCaps
{
    public:
        CDXVA_CAPS_TYPE             m_CapsType;

    private:
        CDXVA_CONTENT_DESC          m_ContentDesc;
        DXVAHD_VPDEVCAPS            m_VPDevCaps;
        CDXVAHD_VPCAPSEX            *m_pVPCaps;
        CDXVA2_VPCAPSEX             *m_pVPCaps2;
        UINT                        m_uiVPCaps2Count;
        GUID                        *m_pVDGuids2;
        CDXVA2_VDCAPSEX             *m_pVDCaps2;
        UINT                        m_uiVDCaps2Count;
        D3DFORMAT                   *m_pInputFormats;
        D3DFORMAT                   *m_pOutputFormats;
        DXVAHD_FILTER_RANGE_DATA    m_BrightnessRange;
        DXVAHD_FILTER_RANGE_DATA    m_ContrastRange;
        DXVAHD_FILTER_RANGE_DATA    m_HueRange;
        DXVAHD_FILTER_RANGE_DATA    m_SaturationRange;
        DXVAHD_FILTER_RANGE_DATA    m_NoiseReductionRange;
        DXVAHD_FILTER_RANGE_DATA    m_EdgeEnhancementRange;
        DXVAHD_FILTER_RANGE_DATA    m_AnamorphicScalingRange;
        CD3D11_VP_CAPS              *m_pD3D11VPCaps;
        DXGI_FORMAT                 *m_pD3D11InputFormats;
        DXGI_FORMAT                 *m_pD3D11OutputFormats;
        UINT                        m_uiD3D11InputFormatCount;
        UINT                        m_uiD3D11OutputFormatCount;


    public:
        CDXVA_DeviceCaps(
            IDXVAHD_Device*         pDevice,
            CDXVA_CONTENT_DESC      ContentDesc,
            HRESULT*                pResult
        ) : m_ContentDesc(ContentDesc),
            m_CapsType(CDXVA_CAPS_TYPE_DXVAHD_VP),
            m_pVPCaps(NULL),
            m_pVPCaps2(NULL),
            m_uiVPCaps2Count(0),
            m_pVDGuids2(NULL),
            m_pVDCaps2(NULL),
            m_uiVDCaps2Count(0),
            m_pInputFormats(NULL),
            m_pOutputFormats(NULL),
            m_pD3D11VPCaps(NULL),
            m_pD3D11InputFormats(NULL),
            m_pD3D11OutputFormats(NULL),
            m_uiD3D11InputFormatCount(0),
            m_uiD3D11OutputFormatCount(0)
        {
            HRESULT hr = S_OK;

            //
            // Clear out the memory.
            //
            memset(&m_VPDevCaps, 0, sizeof(DXVAHD_VPDEVCAPS));
            memset(&m_BrightnessRange, 0, sizeof(DXVAHD_FILTER_RANGE_DATA));
            memset(&m_ContrastRange, 0, sizeof(DXVAHD_FILTER_RANGE_DATA));
            memset(&m_HueRange, 0, sizeof(DXVAHD_FILTER_RANGE_DATA));
            memset(&m_SaturationRange, 0, sizeof(DXVAHD_FILTER_RANGE_DATA));
            memset(&m_NoiseReductionRange, 0, sizeof(DXVAHD_FILTER_RANGE_DATA));
            memset(&m_EdgeEnhancementRange, 0, sizeof(DXVAHD_FILTER_RANGE_DATA));
            memset(&m_AnamorphicScalingRange, 0, sizeof(DXVAHD_FILTER_RANGE_DATA));

            hr = GetDXVAHDDeviceCaps(
                pDevice,
                &m_VPDevCaps,
                &m_pVPCaps,
                &m_pInputFormats,
                &m_pOutputFormats,
                &m_BrightnessRange,
                &m_ContrastRange,
                &m_HueRange,
                &m_SaturationRange,
                &m_NoiseReductionRange,
                &m_EdgeEnhancementRange,
                &m_AnamorphicScalingRange
            );

            //
            // Result back to the allocation call.
            //
            *pResult = hr;
        }
        CDXVA_DeviceCaps(
            IDirectXVideoProcessorService*  pDevice,
            CDXVA_CONTENT_DESC              ContentDesc,
            HRESULT*                        pResult
        ) : m_ContentDesc(ContentDesc),
            m_CapsType(CDXVA_CAPS_TYPE_DXVA2_VP),
            m_pVPCaps(NULL),
            m_pVPCaps2(NULL),
            m_uiVPCaps2Count(0),
            m_pVDGuids2(NULL),
            m_pVDCaps2(NULL),
            m_uiVDCaps2Count(0),
            m_pInputFormats(NULL),
            m_pOutputFormats(NULL),
            m_pD3D11VPCaps(NULL),
            m_pD3D11InputFormats(NULL),
            m_pD3D11OutputFormats(NULL),
            m_uiD3D11InputFormatCount(0),
            m_uiD3D11OutputFormatCount(0)
        {
            HRESULT hr = S_OK;

            hr = GetDXVA2DeviceCaps(
                pDevice,
                &m_uiVPCaps2Count,
                &m_pVPCaps2
            );

            //
            // Result back to the allocation call.
            //
            *pResult = hr;
        };
        CDXVA_DeviceCaps(
            IDirectXVideoDecoderService*    pDevice,
            CDXVA_CONTENT_DESC              ContentDesc,
            HRESULT*                        pResult
        ) : m_ContentDesc(ContentDesc),
            m_CapsType(CDXVA_CAPS_TYPE_DXVA2_VD),
            m_pVPCaps(NULL),
            m_pVPCaps2(NULL),
            m_uiVPCaps2Count(0),
            m_pVDGuids2(NULL),
            m_pVDCaps2(NULL),
            m_uiVDCaps2Count(0),
            m_pInputFormats(NULL),
            m_pOutputFormats(NULL),
            m_pD3D11VPCaps(NULL),
            m_pD3D11InputFormats(NULL),
            m_pD3D11OutputFormats(NULL),
            m_uiD3D11InputFormatCount(0),
            m_uiD3D11OutputFormatCount(0)
        {
            HRESULT hr = S_OK;

            hr = GetDXVA2DeviceCaps(
                pDevice,
                &m_uiVDCaps2Count,
                &m_pVDGuids2,
                &m_pVDCaps2
            );

            //
            // Result back to the allocation call.
            //
            *pResult = hr;
        };
        CDXVA_DeviceCaps(
            ID3D11VideoProcessorEnumerator*   pVPEnum,
            CDXVA_CONTENT_DESC          ContentDesc,
            HRESULT*                    pResult
        ) : m_ContentDesc(ContentDesc),
            m_CapsType(CDXVA_CAPS_TYPE_D3D11_VP),
            m_pVPCaps(NULL),
            m_pVPCaps2(NULL),
            m_uiVPCaps2Count(0),
            m_pVDGuids2(NULL),
            m_pVDCaps2(NULL),
            m_uiVDCaps2Count(0),
            m_pInputFormats(NULL),
            m_pOutputFormats(NULL),
            m_pD3D11VPCaps(NULL),
            m_pD3D11InputFormats(NULL),
            m_pD3D11OutputFormats(NULL),
            m_uiD3D11InputFormatCount(0),
            m_uiD3D11OutputFormatCount(0)
        {
            HRESULT hr = S_OK;

            //
            // Get D3D11 Video Processor Caps
            //
            hr = GetD3D11VideoProcessorCaps(
                pVPEnum,
                &m_pD3D11VPCaps,
                &m_uiD3D11InputFormatCount,
                &m_uiD3D11OutputFormatCount,
                &m_pD3D11InputFormats,
                &m_pD3D11OutputFormats
            );

            //
            // Result back to the allocation call.
            //
            *pResult = hr;
        }
        ~CDXVA_DeviceCaps()
        {            
            SAFE_DELETE_CVPCAPS_ARRAY(m_pVPCaps, m_VPDevCaps.VideoProcessorCount);
            SAFE_DELETE_ARRAY(m_pVPCaps2);
            SAFE_DELETE_ARRAY(m_pVDCaps2);
            SAFE_DELETE_ARRAY(m_pInputFormats);
            SAFE_DELETE_ARRAY(m_pOutputFormats);

            SAFE_DELETE_ARRAY(m_pD3D11InputFormats);
            SAFE_DELETE_ARRAY(m_pD3D11OutputFormats);

            if( NULL != m_pD3D11VPCaps )
            {
                SAFE_DELETE_ARRAY(m_pD3D11VPCaps->pCustomRates);
                SAFE_DELETE(m_pD3D11VPCaps);
            }
        }
        VOID GetContentDesc(__out DXVAHD_CONTENT_DESC *pContentDesc)
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVAHD_VP) return;

            *pContentDesc = ToDXVAHD_CONTENT_DESC(m_ContentDesc);
            return;
        }
        VOID GetContentDesc(__out DXVA2_VideoDesc *pContentDesc)
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VP &&
                m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VD) 
                return;

            *pContentDesc = ToDXVA2_VideoDesc(m_ContentDesc);
            return;
        }
        VOID GetContentDesc(__out D3D11_VIDEO_PROCESSOR_CONTENT_DESC *pContentDesc)
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_D3D11_VP) 
                return;

            *pContentDesc = ToD3D11VP_CONTENT_DESC(m_ContentDesc);
            return;
        }
        VOID GetDeviceCaps(__out DXVAHD_VPDEVCAPS *pDeviceCaps)
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVAHD_VP) return;

            *pDeviceCaps = m_VPDevCaps;
            return;
        }
        VOID GetDeviceCaps(__out D3D11_VIDEO_PROCESSOR_CAPS *pDeviceCaps)
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_D3D11_VP) return;

            *pDeviceCaps = m_pD3D11VPCaps->VPCaps;
            return;
        }
        VOID GetCaps(
            __out_ecount(*pVideoProcessorCount)  CDXVAHD_VPCAPSEX** ppVideoProcessorCaps, 
            __out                                UINT*              pVideoProcessorCount
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVAHD_VP) return;

            *ppVideoProcessorCaps = m_pVPCaps;
            *pVideoProcessorCount = m_VPDevCaps.VideoProcessorCount;
            return;
        }

        VOID GetCaps(
            __in    UINT                uiIndex,
            __out   CDXVAHD_VPCAPSEX*   pVPCaps
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVAHD_VP) return;

            assert(uiIndex < m_VPDevCaps.VideoProcessorCount);
            *pVPCaps = m_pVPCaps[uiIndex];
            return;
        };
        VOID GetCaps(
            __in    UINT                uiIndex,
            __out   CDXVA2_VPCAPSEX*    pVPCaps
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VP) return;

            assert(uiIndex < m_uiVPCaps2Count);
            *pVPCaps = m_pVPCaps2[uiIndex];
            return;
        };
        VOID GetCaps(
            __in    UINT                uiIndex,
            __out   CDXVA2_VDCAPSEX*    pVDCaps
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VD) return;

            assert(uiIndex < m_uiVDCaps2Count);
            *pVDCaps = m_pVDCaps2[uiIndex];
            return;
        };
        VOID GetCaps(
            __in    GUID                VPGuid,
            __out   CDXVAHD_VPCAPSEX*   pVPCaps
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVAHD_VP) return;

            for (
                UINT uiVPIndex = 0; 
                uiVPIndex != m_uiVPCaps2Count; 
                uiVPIndex++
            ) {
                if (m_pVPCaps[uiVPIndex].VPCaps.VPGuid == VPGuid)
                {
                    *pVPCaps = m_pVPCaps[uiVPIndex];
                    return;
                };
            };

            return;
        };
        VOID GetCaps(
            __in    GUID                VPGuid,
            __out   CDXVA2_VPCAPSEX*    pVPCaps
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VP) return;

            for (
                UINT uiVPIndex = 0; 
                uiVPIndex != m_uiVPCaps2Count; 
                uiVPIndex++
            ) {
                if (m_pVPCaps2[uiVPIndex].Guid == VPGuid)
                {
                    *pVPCaps = m_pVPCaps2[uiVPIndex];
                    return;
                };
            };

            return;
        }
        VOID GetCaps(
            __in    GUID                VDGuid,
            __out   CDXVA2_VDCAPSEX*    pVDCaps
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VD) return;

            for (
                UINT uiVDIndex = 0; 
                uiVDIndex != m_uiVDCaps2Count;
                uiVDIndex++
            ) {
                if (m_pVDCaps2[uiVDIndex].Guid == VDGuid)
                {
                    *pVDCaps = m_pVDCaps2[uiVDIndex];
                    return;
                };
            };

            return;
        }
        VOID GetCaps(
            __out   CDXVA2_VPCAPSEX**   ppVPCaps,
            __in    UINT*               pCapsCount
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VP) return;

            *pCapsCount = m_uiVPCaps2Count;
            *ppVPCaps = m_pVPCaps2;

            return;
        };
        VOID GetCaps(
            __out   CDXVA2_VDCAPSEX**   ppVDCaps,
            __in    UINT*               pCapsCount
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VD) return;

            *pCapsCount = m_uiVDCaps2Count;
            *ppVDCaps = m_pVDCaps2;

            return;
        };
        VOID GetCaps(
            __out   CD3D11_VP_CAPS**   ppVDCaps
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_D3D11_VP) return;

            *ppVDCaps = m_pD3D11VPCaps;

            return;
        };
        VOID GetFormats(
            __in                        CDXVA_FORMAT_TYPE   FormatType, 
            __out_ecount(*pFormatCount) D3DFORMAT**         ppFormats, 
            __out                       UINT*               pFormatCount
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVAHD_VP) return;

            switch (FormatType)
            {
                case CDXVA_FORMAT_TYPE_INPUT:
                    *ppFormats = m_pInputFormats;
                    *pFormatCount = m_VPDevCaps.InputFormatCount;
                    return;
                case CDXVA_FORMAT_TYPE_OUTPUT:
                    *ppFormats = m_pOutputFormats;
                    *pFormatCount = m_VPDevCaps.OutputFormatCount;
                    return;
                default:
                    return;
            }
        }
        VOID GetFormats(
            __in                        GUID                Guid,
            __in                        CDXVA_FORMAT_TYPE   FormatType,
            __out_ecount(*pFormatCount) D3DFORMAT**         ppFormats, 
            __out                       UINT*               pFormatCount
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_DXVA2_VP) return;

            switch (FormatType)
            {
                case CDXVA_FORMAT_TYPE_INPUT:
                    for (
                        UINT uiVPCapsIdx = 0; 
                        uiVPCapsIdx != m_uiVPCaps2Count; 
                        uiVPCapsIdx++
                    )
                    {
                        CDXVA2_VPCAPSEX VPCaps2 = m_pVPCaps2[uiVPCapsIdx];

                        *ppFormats = VPCaps2.pInputFormats;
                        *pFormatCount = VPCaps2.uiInputFormatCount;
                    }

                    return;
                case CDXVA_FORMAT_TYPE_OUTPUT:

                    return;
                default:
                    return;
            }
        }
        VOID GetFormats(
            __in                        CDXVA_FORMAT_TYPE   FormatType, 
            __out_ecount(*pFormatCount) DXGI_FORMAT**       ppFormats, 
            __out                       UINT*               pFormatCount
        )
        {
            //
            // protect against invalid caps type queries.
            //
            if (m_CapsType != CDXVA_CAPS_TYPE_D3D11_VP) return;

            switch (FormatType)
            {
                case CDXVA_FORMAT_TYPE_INPUT:
                    *ppFormats = m_pD3D11InputFormats;
                    *pFormatCount = m_uiD3D11InputFormatCount;
                    return;
                case CDXVA_FORMAT_TYPE_OUTPUT:
                    *ppFormats = m_pD3D11OutputFormats;
                    *pFormatCount = m_uiD3D11OutputFormatCount;
                    return;
                default:
                    return;
            }
        }

        VOID GetFilterRangeData(
            __in    DXVAHD_FILTER               Filter, 
            __out   DXVAHD_FILTER_RANGE_DATA*   pFilterRangeData
        )
        {
            switch (Filter)
            {
                case DXVAHD_FILTER_BRIGHTNESS:
                    *pFilterRangeData = m_BrightnessRange;
                    return;
                case DXVAHD_FILTER_CONTRAST:
                    *pFilterRangeData = m_ContrastRange;
                    return;
                case DXVAHD_FILTER_HUE:
                    *pFilterRangeData = m_HueRange;
                    return;
                case DXVAHD_FILTER_SATURATION:
                    *pFilterRangeData = m_SaturationRange;
                    return;
                case DXVAHD_FILTER_NOISE_REDUCTION:
                    *pFilterRangeData = m_NoiseReductionRange;
                    return;
                case DXVAHD_FILTER_EDGE_ENHANCEMENT:
                    *pFilterRangeData = m_EdgeEnhancementRange;
                    return;
                case DXVAHD_FILTER_ANAMORPHIC_SCALING:
                    *pFilterRangeData = m_AnamorphicScalingRange;
                    return;
                default:
                    return;
            }
        }

        BOOL equals(
            __in    CDXVA_DeviceCaps    *pCaps
        )
        {
            DXVAHD_CONTENT_DESC ContentDesc;
            UINT              uiVPCapsCount = 0;
            UINT         uiInputFormatCount = 0;
            UINT        uiOutputFormatCount = 0;
            CDXVAHD_VPCAPSEX       *pVPCaps = NULL;
            D3DFORMAT        *pInputFormats = NULL;
            D3DFORMAT       *pOutputFormats = NULL;
            D3D11_VIDEO_PROCESSOR_CONTENT_DESC D3D11ContentDesc;
            CD3D11_VP_CAPS       *pD3D11VPCaps = NULL;

            //
            // Verify caps types match up.
            //
            if (pCaps->m_CapsType != m_CapsType) {
                return FALSE;
            };

            if (pCaps->m_CapsType == CDXVA_CAPS_TYPE_DXVAHD_VP) {
                //
                // Get the Cap to compare this Cap to
                //
                pCaps->GetContentDesc(&ContentDesc);
                pCaps->GetCaps(&pVPCaps, &uiVPCapsCount);
                pCaps->GetFormats(CDXVA_FORMAT_TYPE_INPUT, &pInputFormats, &uiInputFormatCount);
                pCaps->GetFormats(CDXVA_FORMAT_TYPE_OUTPUT, &pOutputFormats, &uiOutputFormatCount);

                //
                // Compare counts and exit if they don't match.
                //
                memset(&ContentDesc, 0, sizeof(DXVAHD_CONTENT_DESC));
                if (ContentDesc != ToDXVAHD_CONTENT_DESC(m_ContentDesc)) return FALSE;
                if (uiVPCapsCount != m_VPDevCaps.VideoProcessorCount) return FALSE;
                if (uiInputFormatCount != m_VPDevCaps.InputFormatCount) return FALSE;
                if (uiOutputFormatCount != m_VPDevCaps.OutputFormatCount) return FALSE;

                //
                // Iterate over the list of VP Caps comparing each Cap
                //
                for (UINT uiVPCapIdx = 0; uiVPCapIdx != uiVPCapsCount; uiVPCapIdx++) {
                    CDXVAHD_VPCAPSEX thisCap = m_pVPCaps[uiVPCapIdx];
                    CDXVAHD_VPCAPSEX thatCap = pVPCaps[uiVPCapIdx];

                    if (thisCap != thatCap) return FALSE;
                }

                //
                // Iterate over the list of Input formats comparing each format
                //
                for (UINT uiInputFormatIdx = 0; uiInputFormatIdx != uiInputFormatCount; uiInputFormatIdx++) {
                    D3DFORMAT thisFormat = m_pInputFormats[uiInputFormatIdx];
                    D3DFORMAT thatFormat = pInputFormats[uiInputFormatIdx];

                    if (thisFormat != thatFormat) return FALSE;
                }

                //
                // Iterate over the list of Output formats comparing each format
                //
                for (UINT uiOutputFormatIdx = 0; uiOutputFormatIdx != uiOutputFormatCount; uiOutputFormatIdx++) {
                    D3DFORMAT thisFormat = m_pOutputFormats[uiOutputFormatIdx];
                    D3DFORMAT thatFormat = pOutputFormats[uiOutputFormatIdx];

                    if (thisFormat != thatFormat) return FALSE;
                }

                //
                // TODO: Compare Filter range data
                //
            } else if (pCaps->m_CapsType == CDXVA_CAPS_TYPE_D3D11_VP) {
                //
                // Get the Cap to compare this Cap to
                //
                memset(&D3D11ContentDesc, 0, sizeof(D3D11_VIDEO_PROCESSOR_CONTENT_DESC));
                pCaps->GetContentDesc(&D3D11ContentDesc);
                pCaps->GetCaps(&pD3D11VPCaps);

                // TODO: track formats
                //pCaps->GetFormats(CDXVA_FORMAT_TYPE_INPUT, &pInputFormats, &uiInputFormatCount);
                //pCaps->GetFormats(CDXVA_FORMAT_TYPE_OUTPUT, &pOutputFormats, &uiOutputFormatCount);

                //
                // Compare desc
                //
                D3D11_VIDEO_PROCESSOR_CONTENT_DESC OurD3D11ContentDesc;
                OurD3D11ContentDesc = ToD3D11VP_CONTENT_DESC(m_ContentDesc);               
                if( D3D11ContentDesc.InputWidth != OurD3D11ContentDesc.InputWidth ||
                    D3D11ContentDesc.InputHeight != OurD3D11ContentDesc.InputHeight ||
                    D3D11ContentDesc.OutputWidth != OurD3D11ContentDesc.OutputWidth ||
                    D3D11ContentDesc.OutputHeight != OurD3D11ContentDesc.OutputHeight ||
                    D3D11ContentDesc.Usage != OurD3D11ContentDesc.Usage ||
                    D3D11ContentDesc.InputFrameRate.Denominator != OurD3D11ContentDesc.InputFrameRate.Denominator ||
                    D3D11ContentDesc.InputFrameRate.Numerator != OurD3D11ContentDesc.InputFrameRate.Numerator ||
                    D3D11ContentDesc.OutputFrameRate.Denominator != OurD3D11ContentDesc.OutputFrameRate.Denominator ||
                    D3D11ContentDesc.OutputFrameRate.Numerator != OurD3D11ContentDesc.OutputFrameRate.Numerator )
                {
                    return FALSE;
                }

                if( pD3D11VPCaps->VPCaps.DeviceCaps != m_pD3D11VPCaps->VPCaps.DeviceCaps ||
                    pD3D11VPCaps->VPCaps.FeatureCaps != m_pD3D11VPCaps->VPCaps.FeatureCaps ||
                    pD3D11VPCaps->VPCaps.FilterCaps != m_pD3D11VPCaps->VPCaps.FilterCaps ||
                    pD3D11VPCaps->VPCaps.InputFormatCaps != m_pD3D11VPCaps->VPCaps.InputFormatCaps ||
                    pD3D11VPCaps->VPCaps.AutoStreamCaps != m_pD3D11VPCaps->VPCaps.AutoStreamCaps ||
                    pD3D11VPCaps->VPCaps.RateConversionCapsCount != m_pD3D11VPCaps->VPCaps.RateConversionCapsCount ||
                    pD3D11VPCaps->VPCaps.MaxInputStreams != m_pD3D11VPCaps->VPCaps.MaxInputStreams ||
                    pD3D11VPCaps->VPCaps.MaxStreamStates != m_pD3D11VPCaps->VPCaps.MaxStreamStates )
                {
                    return FALSE;
                }

                //
                // Check custom rates caps
                //
                if( NULL != pD3D11VPCaps->pCustomRates && NULL != m_pD3D11VPCaps->pCustomRates )
                {
                    for( UINT uiRateConversionCapsIndex = 0;
                        uiRateConversionCapsIndex < pD3D11VPCaps->VPCaps.RateConversionCapsCount;
                        uiRateConversionCapsIndex++ )
                    {
                        D3D11_VIDEO_PROCESSOR_RATE_CONVERSION_CAPS cap1;
                        cap1 = pD3D11VPCaps->pCustomRates[uiRateConversionCapsIndex];
                        D3D11_VIDEO_PROCESSOR_RATE_CONVERSION_CAPS cap2;
                        cap2 = pD3D11VPCaps->pCustomRates[uiRateConversionCapsIndex];
                        
                        if( cap1.PastFrames != cap2.PastFrames ||
                            cap1.FutureFrames != cap2.FutureFrames ||
                            cap1.ProcessorCaps != cap2.ProcessorCaps ||
                            cap1.ITelecineCaps != cap2.ITelecineCaps ||
                            cap1.CustomRateCount != cap2.CustomRateCount )
                        {
                            return FALSE;
                        }
                    } 
                }               
                
            } else{
                //
                // HD VideoProcessing caps are only supported now.
                // TODO: Add DXVA2_VP and DXVA2_VD support.
                //
                return FALSE;
            };

            return TRUE;
        }
    private:
        HRESULT GetDXVA2DeviceCaps(
            __in                                IDirectXVideoProcessorService   *pDevice,
            __out                               UINT                            *pCVPCapCount,
            __deref_out_ecount(*pCVPCapCount)   CDXVA2_VPCAPSEX                 **ppCVPCapsEx
        )
        {
            HRESULT                 hr = S_OK;
            UINT           uiGuidCount = 0;
            GUID*               pGuids = NULL;
            DXVA2_VideoDesc  VideoDesc = {0};
            CDXVA2_VPCAPSEX *pVPCapsEx = NULL;

            //
            // Get the number of guids and the list of guids.
            //
            hr = pDevice->GetVideoProcessorDeviceGuids(&VideoDesc,
                                                       &uiGuidCount,
                                                       &pGuids);

            if (FAILED(hr))
            {
                goto Cleanup;
            }
            else if (uiGuidCount == 0 || pGuids == NULL)
            {
                hr = E_FAIL;
                goto Cleanup;
            }

            //
            // Allocate the caps structure.
            //
            pVPCapsEx = new CDXVA2_VPCAPSEX[uiGuidCount];
            if (pVPCapsEx == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto Cleanup;
            }
            memset(pVPCapsEx, 0, sizeof(CDXVA2_VPCAPSEX) * uiGuidCount);

            //
            // Get the number of render target formats supported for each
            // video processing guid.
            //
            for (UINT uiGuidIdx = 0; uiGuidIdx != uiGuidCount; uiGuidIdx++)
            {
                REFGUID Guid                = pGuids[uiGuidIdx];
                CDXVA2_RTCAPSEX *pRTCapsEx  = NULL;
                UINT uiFormatCount          = 0;
                D3DFORMAT *pFormats         = NULL;

                //
                // Query the list of render target formats
                //
                hr = pDevice->GetVideoProcessorRenderTargets(Guid,
                                                             &VideoDesc,
                                                             &uiFormatCount,
                                                             &pFormats);

                if (FAILED(hr))
                {
                    goto ProcessorCleanup;
                }
                else if (uiGuidCount == 0 || pGuids == NULL)
                {
                    hr = E_FAIL;
                    goto ProcessorCleanup;
                }

                //
                // Allocate the render target caps extended array.
                //
                pRTCapsEx = new CDXVA2_RTCAPSEX[uiFormatCount];
                if (pRTCapsEx == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto ProcessorCleanup;
                }
                memset(pRTCapsEx, 0, sizeof(CDXVA2_RTCAPSEX) * uiFormatCount);

                #pragma region RenderTarget Caps
                //
                // If not continue gathering the rest of the caps and range 
                // support.
                //
                for (
                    UINT uiFormatIdx = 0; 
                    uiFormatIdx != uiFormatCount; 
                    uiFormatIdx++
                )
                {
                    CDXVA2_RTCAPSEX                        RTCapsEx;
                    D3DFORMAT                                Format = pFormats[uiFormatIdx];
                    DXVA2_VideoProcessorCaps                 VPCaps = {0};
                    DXVA2_ValueRange                BrightnessRange = {0};
                    DXVA2_ValueRange                  ContrastRange = {0};
                    DXVA2_ValueRange                       HueRange = {0};
                    DXVA2_ValueRange                SaturationRange = {0};
                    DXVA2_ValueRange           NoiseFilterLumaLevel = {0};
                    DXVA2_ValueRange       NoiseFilterLumaThreshold = {0};
                    DXVA2_ValueRange          NoiseFilterLumaRadius = {0};
                    DXVA2_ValueRange         NoiseFilterChromaLevel = {0};
                    DXVA2_ValueRange     NoiseFilterChromaThreshold = {0};
                    DXVA2_ValueRange        NoiseFilterChromaRadius = {0};
                    DXVA2_ValueRange          DetailFilterLumaLevel = {0};
                    DXVA2_ValueRange      DetailFilterLumaThreshold = {0};
                    DXVA2_ValueRange         DetailFilterLumaRadius = {0};
                    DXVA2_ValueRange        DetailFilterChromaLevel = {0};
                    DXVA2_ValueRange    DetailFilterChromaThreshold = {0};
                    DXVA2_ValueRange       DetailFilterChromaRadius = {0};
                    
                    //
                    // Get the video processor caps. 
                    //
                    hr = pDevice->GetVideoProcessorCaps(Guid,
                                                        &VideoDesc,
                                                        Format,
                                                        &VPCaps);
                    
                    if (FAILED(hr)) break;

                    //
                    // Get the brightness range.
                    //
                    hr = pDevice->GetProcAmpRange(Guid,
                                                  &VideoDesc,
                                                  Format,
                                                  DXVA2_ProcAmp_Brightness,
                                                  &BrightnessRange);
                    
                    if (FAILED(hr)) break;
                    
                    //
                    // Get the contrast range.
                    //
                    hr = pDevice->GetProcAmpRange(Guid,
                                                  &VideoDesc,
                                                  Format,
                                                  DXVA2_ProcAmp_Contrast,
                                                  &ContrastRange);
                    
                    if (FAILED(hr)) break;

                    //
                    // Get the contrast range.
                    //
                    hr = pDevice->GetProcAmpRange(Guid,
                                                  &VideoDesc,
                                                  Format,
                                                  DXVA2_ProcAmp_Hue,
                                                  &HueRange);
                    
                    if (FAILED(hr)) break;

                    //
                    // Get the contrast range.
                    //
                    hr = pDevice->GetProcAmpRange(Guid,
                                                  &VideoDesc,
                                                  Format,
                                                  DXVA2_ProcAmp_Saturation,
                                                  &SaturationRange);
                    
                    if (FAILED(hr)) break;

                    #pragma region Detail Filter
                    //
                    // If a form of detail filter technology is supported get 
                    // the caps.
                    //
                    if (VPCaps.DetailFilterTechnology != 
                        DXVA2_DetailFilterTech_Unsupported)
                    {
                        //
                        // Get the luma level for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_DetailFilterLumaLevel,
                                                             &DetailFilterLumaLevel);
                        if (FAILED(hr)) break;

                        //
                        // Get the luma threshold for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_DetailFilterLumaThreshold,
                                                             &DetailFilterLumaThreshold);
                        if (FAILED(hr)) break;

                        //
                        // Get the luma radius for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_DetailFilterLumaRadius,
                                                             &DetailFilterLumaRadius);
                        if (FAILED(hr)) break;

                        //
                        // Get the chroma level for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_DetailFilterChromaLevel,
                                                             &DetailFilterChromaLevel);
                        if (FAILED(hr)) break;

                        //
                        // Get the chroma threshold for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_DetailFilterChromaThreshold,
                                                             &DetailFilterChromaThreshold);
                        if (FAILED(hr)) break;

                        //
                        // Get the chroma radius for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_DetailFilterChromaRadius,
                                                             &DetailFilterChromaRadius);
                        if (FAILED(hr)) break;
                    }
                    #pragma endregion
                    #pragma region Noise Filter
                    //
                    // If a form of detail filter technology is supported get 
                    // the caps.
                    //
                    if (VPCaps.NoiseFilterTechnology != 
                        DXVA2_NoiseFilterTech_Unsupported)
                    {
                        //
                        // Get the luma level for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_NoiseFilterLumaLevel,
                                                             &NoiseFilterLumaLevel);
                        if (FAILED(hr)) break;

                        //
                        // Get the luma threshold for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_NoiseFilterLumaThreshold,
                                                             &NoiseFilterLumaThreshold);
                        if (FAILED(hr)) break;

                        //
                        // Get the luma radius for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_NoiseFilterLumaRadius,
                                                             &NoiseFilterLumaRadius);
                        if (FAILED(hr)) break;

                        //
                        // Get the chroma level for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_NoiseFilterChromaLevel,
                                                             &NoiseFilterChromaLevel);
                        if (FAILED(hr)) break;

                        //
                        // Get the chroma threshold for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_NoiseFilterChromaThreshold,
                                                             &NoiseFilterChromaThreshold);
                        if (FAILED(hr)) break;

                        //
                        // Get the chroma radius for the filter.
                        //
                        hr = pDevice->GetFilterPropertyRange(Guid,
                                                             &VideoDesc,
                                                             Format,
                                                             DXVA2_NoiseFilterChromaRadius,
                                                             &NoiseFilterChromaRadius);
                        if (FAILED(hr)) break;
                    }
                    #pragma endregion

                    //
                    // Create the VPCaps extended info.
                    //
                    memset(&RTCapsEx, 0, sizeof(CDXVA2_RTCAPSEX));
                    RTCapsEx.Format = Format;
                    RTCapsEx.VPCaps = VPCaps;
                    RTCapsEx.BrightnessRange = BrightnessRange;
                    RTCapsEx.ContrastRange = ContrastRange;
                    RTCapsEx.HueRange = HueRange;
                    RTCapsEx.SaturationRange = SaturationRange;
                    RTCapsEx.NoiseFilterLumaLevel = NoiseFilterLumaLevel;
                    RTCapsEx.NoiseFilterLumaThreshold = NoiseFilterLumaThreshold;
                    RTCapsEx.NoiseFilterLumaRadius = NoiseFilterLumaRadius;
                    RTCapsEx.NoiseFilterChromaLevel = NoiseFilterChromaLevel;
                    RTCapsEx.NoiseFilterChromaThreshold = NoiseFilterChromaThreshold;
                    RTCapsEx.NoiseFilterChromaRadius = NoiseFilterChromaRadius;
                    RTCapsEx.DetailFilterLumaLevel = DetailFilterLumaLevel;
                    RTCapsEx.DetailFilterLumaThreshold = DetailFilterLumaThreshold;
                    RTCapsEx.DetailFilterLumaRadius = DetailFilterLumaRadius;
                    RTCapsEx.DetailFilterChromaLevel = DetailFilterChromaLevel;
                    RTCapsEx.DetailFilterChromaThreshold = DetailFilterChromaThreshold;
                    RTCapsEx.DetailFilterChromaRadius = DetailFilterChromaRadius;

                    //
                    // Fill in the array.
                    //
                    pRTCapsEx[uiFormatIdx] = RTCapsEx;
                }
                #pragma endregion

            ProcessorCleanup:
                

                if (SUCCEEDED(hr))
                {
                    pVPCapsEx[uiGuidIdx].Guid = Guid;
                    pVPCapsEx[uiGuidIdx].uiInputFormatCount = uiFormatCount;
                    pVPCapsEx[uiGuidIdx].pInputFormats = pFormats;
                    pVPCapsEx[uiGuidIdx].pRTCapsEx = pRTCapsEx;
                }
                else
                {
                    //
                    // Cleanup the RT caps array as there was a failure then
                    // break out of the loop.
                    //
                    SAFE_DELETE_ARRAY(pRTCapsEx);

                    //
                    // Cleanup the formats regardless.
                    //
                    SAFE_COTASK_MEM_FREE(pFormats);

                    break;
                }
            }

        Cleanup:
            SAFE_COTASK_MEM_FREE(pGuids);

            if (SUCCEEDED(hr))
            {
                //
                // Successful gather so fill in the output caps array.
                //
                *ppCVPCapsEx = pVPCapsEx;
                *pCVPCapCount = uiGuidCount;
            }
            else
            {
                //
                // Cleanup the caps array as there was a failure while 
                // gathering the capabilities.
                //
                SAFE_DELETE_ARRAY(pVPCapsEx);
            }

            return hr;
        };
        HRESULT GetDXVA2DeviceCaps(
            __in                            IDirectXVideoDecoderService *pDevice,
            __out                           UINT                        *pGuidCount,
            __deref_out_ecount(*pGuidCount) GUID                        **ppGuids,            
            __deref_out_ecount(*pGuidCount) CDXVA2_VDCAPSEX             **ppCVDCapsEx
        )
        {
            HRESULT                 hr = S_OK;
            UINT           uiGuidCount = 0;
            GUID*               pGuids = NULL;
            DXVA2_VideoDesc  VideoDesc = {0};
            CDXVA2_VDCAPSEX *pVDCapsEx = NULL;

            //
            // Get the number of guids and the list of guids.
            //
            hr = pDevice->GetDecoderDeviceGuids(&uiGuidCount,
                                                ppGuids);

            if (FAILED(hr))
            {
                goto Cleanup;
            }
            else if (uiGuidCount == 0 || *ppGuids == NULL)
            {
                hr = E_FAIL;
                goto Cleanup;
            }

            //
            // Allocate the caps structure.
            //
            pVDCapsEx = new CDXVA2_VDCAPSEX[uiGuidCount];
            if (pVDCapsEx == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto Cleanup;
            }
            memset(pVDCapsEx, 0, sizeof(CDXVA2_VDCAPSEX) * uiGuidCount);

            //
            // To DXVA2 standard video desc.
            //
            VideoDesc = ToDXVA2_VideoDesc(m_ContentDesc);

            //
            // Get the number of render target formats supported for each
            // video decoding guid.
            //
            for (UINT uiGuidIdx = 0; uiGuidIdx != uiGuidCount; uiGuidIdx++)
            {
                REFGUID Guid                        = (*ppGuids)[uiGuidIdx];
                UINT uiFormatCount                  = 0;
                D3DFORMAT *pFormats                 = NULL;
                UINT uiConfigCount                  = 0;
                DXVA2_ConfigPictureDecode *pConfigs = NULL;

                //
                // Query the list of render target formats
                //
                hr = pDevice->GetDecoderRenderTargets(Guid,
                                                      &uiFormatCount,
                                                      &pFormats);

                if (FAILED(hr))
                {
                    goto DecoderCleanup;
                }
                else if (uiFormatCount == 0 || pFormats == NULL)
                {
                    hr = E_FAIL;
                    goto DecoderCleanup;
                }

                //
                // Query the list of decode configurations
                //
                hr = pDevice->GetDecoderConfigurations(Guid,
                                                       &VideoDesc,
                                                       NULL,
                                                       &uiConfigCount,
                                                       &pConfigs);

                if (FAILED(hr))
                {
                    goto DecoderCleanup;
                }
                else if (uiConfigCount == 0 || pConfigs == NULL)
                {
                    hr = E_FAIL;
                    goto DecoderCleanup;
                }                

            DecoderCleanup:
                if (SUCCEEDED(hr))
                {
                    pVDCapsEx[uiGuidIdx].Guid           = Guid;
                    pVDCapsEx[uiGuidIdx].uiFormatCount  = uiFormatCount;
                    pVDCapsEx[uiGuidIdx].pFormats       = pFormats;
                    pVDCapsEx[uiGuidIdx].uiConfigCount  = uiConfigCount;
                    pVDCapsEx[uiGuidIdx].pConfigs       = pConfigs;
                }
                else
                {
                    //
                    // Cleanup the Formats and Configs for this GUID.
                    //
                    SAFE_COTASK_MEM_FREE(pFormats);
                    SAFE_COTASK_MEM_FREE(pConfigs);
                    break;
                }
            }

        Cleanup:
            SAFE_COTASK_MEM_FREE(pGuids);

            if (SUCCEEDED(hr))
            {
                //
                // Successful gather so fill in the output caps array.
                //
                *pGuidCount = uiGuidCount;
                *ppGuids = pGuids;
                *ppCVDCapsEx = pVDCapsEx;
            }
            else
            {
                //
                // Cleanup the caps array as there was a failure while 
                // gathering the capabilities.
                //
                SAFE_COTASK_MEM_FREE(pGuids);
                SAFE_DELETE_ARRAY(pVDCapsEx);
            }

            return hr;
        };
        HRESULT GetDXVAHDDeviceCaps(
            __in        IDXVAHD_Device              *pDevice,
            __out       DXVAHD_VPDEVCAPS            *pVPDeviceCaps,
            __deref_out CDXVAHD_VPCAPSEX            **ppCVPCaps,
            __deref_out D3DFORMAT                   **ppInputFormats,
            __deref_out D3DFORMAT                   **ppOutputFormats,
            __out       DXVAHD_FILTER_RANGE_DATA    *pBrightnessRange,
            __out       DXVAHD_FILTER_RANGE_DATA    *pContrastRange,
            __out       DXVAHD_FILTER_RANGE_DATA    *pHueRange,
            __out       DXVAHD_FILTER_RANGE_DATA    *pSaturationRange,
            __out       DXVAHD_FILTER_RANGE_DATA    *pNoiseReductionRange,
            __out       DXVAHD_FILTER_RANGE_DATA    *pEdgeEnhancementRange,
            __out       DXVAHD_FILTER_RANGE_DATA    *pAnamorphicScalingRange
        )
        {
            HRESULT                                      hr = S_OK;
            DXVAHD_VPDEVCAPS                   VPDeviceCaps;
            DXVAHD_VPCAPS                          *pVPCaps = NULL;
            CDXVAHD_VPCAPSEX                      *pCVPCaps = NULL;
            D3DFORMAT                        *pInputFormats = NULL;
            D3DFORMAT                       *pOutputFormats = NULL;
            DXVAHD_FILTER_RANGE_DATA        BrightnessRange = {0};
            DXVAHD_FILTER_RANGE_DATA          ContrastRange = {0};
            DXVAHD_FILTER_RANGE_DATA               HueRange = {0};
            DXVAHD_FILTER_RANGE_DATA        SaturationRange = {0};
            DXVAHD_FILTER_RANGE_DATA    NoiseReductionRange = {0};
            DXVAHD_FILTER_RANGE_DATA   EdgeEnhancementRange = {0};
            DXVAHD_FILTER_RANGE_DATA AnamorphicScalingRange = {0};
            UINT                      uiVideoProcessorCount = 0;
            UINT                         uiInputFormatCount = 0;
            UINT                        uiOutputFormatCount = 0;

            //
            // Gather the caps for the Device
            //
            memset(&VPDeviceCaps, 0, sizeof(DXVAHD_VPDEVCAPS));
            hr = pDevice->GetVideoProcessorDeviceCaps(&VPDeviceCaps);
            if (FAILED(hr))
            {
                goto CleanUp;
            }

            //
            // For later use
            //
            uiVideoProcessorCount = VPDeviceCaps.VideoProcessorCount;
            uiInputFormatCount    = VPDeviceCaps.InputFormatCount;
            uiOutputFormatCount   = VPDeviceCaps.OutputFormatCount;

            //
            // Allocate the memory for the Caps list.
            //
            pVPCaps = new DXVAHD_VPCAPS[uiVideoProcessorCount];
            if (!pVPCaps)
            {
                goto CleanUp;
            }
            memset(pVPCaps, 0, sizeof(DXVAHD_VPCAPS)*uiVideoProcessorCount);

            //
            // Gather the caps for the Video Processor
            //
            hr = pDevice->GetVideoProcessorCaps(uiVideoProcessorCount,
                                                pVPCaps);
            if (FAILED(hr))
            {
                goto CleanUp;
            }

            #pragma region Formats and Video Processor Allocation
            //
            // Allocate memory for formats and VPs
            //
            pInputFormats = new D3DFORMAT[uiInputFormatCount];
            if (!pInputFormats)
            {
                goto CleanUp;
            }
            memset(pInputFormats, 0, sizeof(D3DFORMAT)*uiInputFormatCount);

            pOutputFormats = new D3DFORMAT[uiOutputFormatCount];
            if (!pOutputFormats)
            {
                goto CleanUp;
            }
            memset(pOutputFormats, 0, sizeof(D3DFORMAT)*uiOutputFormatCount);

            pCVPCaps = new CDXVAHD_VPCAPSEX[uiVideoProcessorCount];
            if (!pCVPCaps)
            {
                goto CleanUp;
            }
            memset(pCVPCaps, 0, sizeof(CDXVAHD_VPCAPSEX)*uiVideoProcessorCount);
            #pragma endregion

            //
            // Get the formats and filter ranges for this device.
            //
            hr = GetDXVAHDVPFormatsAndRanges(pDevice,
                                             &VPDeviceCaps,
                                             pInputFormats,
                                             pOutputFormats,
                                             &BrightnessRange,
                                             &ContrastRange,
                                             &HueRange,
                                             &SaturationRange,
                                             &NoiseReductionRange,
                                             &EdgeEnhancementRange,
                                             &AnamorphicScalingRange);
            if (FAILED(hr))
            {
                goto CleanUp;
            }            

            #pragma region Get VPCaps
            for (UINT uiVPIndex = 0; uiVPIndex != uiVideoProcessorCount; uiVPIndex++)
            {
                UINT                uiCustomRateCount = 0;
                DXVAHD_CUSTOM_RATE_DATA *pCustomRates = NULL;
                CDXVAHD_VPCAPSEX              CVPCaps = {0};
                DXVAHD_VPCAPS                  VPCaps = pVPCaps[uiVPIndex];
                GUID                           VPGuid = VPCaps.VPGuid;

                //
                // Zero the memory for the struct
                //
                memset(&CVPCaps, 0, sizeof(CDXVAHD_VPCAPSEX));
                CVPCaps.VPCaps = VPCaps;
                uiCustomRateCount = VPCaps.CustomRateCount;

                //
                // Only get CustomRates if the count is greater than zero if not skip
                // it.
                //
                if (uiCustomRateCount > 0)
                {                    
                    //
                    // Allocate the memory for the list of custom rates.
                    //
                    pCustomRates = new DXVAHD_CUSTOM_RATE_DATA[uiCustomRateCount];
                    if (!pCustomRates)
                    {
                        goto CleanUp;
                    }
                    memset(pCustomRates, 0, sizeof(DXVAHD_CUSTOM_RATE_DATA) * uiCustomRateCount);

                    //
                    // Get the list of Custom Rates for this VP
                    //
                    hr = pDevice->GetVideoProcessorCustomRates(&VPGuid,
                                                               uiCustomRateCount,
                                                               pCustomRates);
                    if (FAILED(hr))
                    {
                        goto CleanUp;
                    }

                    //
                    // Fill in the custom rates of the CVPCaps for this VP.
                    //
                    CVPCaps.pCustomRates = pCustomRates;
                } // Custom Rate Count

                pCVPCaps[uiVPIndex] = CVPCaps;
            } // VideoProcessors
            #pragma endregion

            //
            // Made it this far then everything was gathered successfully
            //
            (*pVPDeviceCaps) = VPDeviceCaps;
            (*ppCVPCaps) = pCVPCaps;
            (*ppInputFormats) = pInputFormats;
            (*ppOutputFormats) = pOutputFormats;
            (*pBrightnessRange) = BrightnessRange;
            (*pContrastRange) = ContrastRange;
            (*pHueRange) = HueRange;
            (*pSaturationRange) = SaturationRange;
            (*pNoiseReductionRange) = NoiseReductionRange;
            (*pEdgeEnhancementRange) = EdgeEnhancementRange;
            (*pAnamorphicScalingRange) = AnamorphicScalingRange;

        CleanUp:
            //
            // Cleanup the DXVAHD_VP_CAPS as they are soft copied into the 
            // pCVPCaps class of values.
            //
            SAFE_DELETE_ARRAY(pVPCaps);

            if (FAILED(hr))
            {
                //
                // Cleanup the memory here as the routine has failed and this memory
                // will not get cleaned up by the caller.
                //                
                SAFE_DELETE_CVPCAPS_ARRAY(pCVPCaps, VPDeviceCaps.VideoProcessorCount);
                SAFE_DELETE_ARRAY(pInputFormats);
                SAFE_DELETE_ARRAY(pOutputFormats);
            }

            return hr;
        }
        HRESULT GetDXVAHDVPFormatsAndRanges(
            __in                                         IDXVAHD_Device            *pDevice,
            __in                                         const DXVAHD_VPDEVCAPS    *pDeviceCaps,
            __out_ecount(pDeviceCaps->InputFormatCount)  D3DFORMAT                 *pInputFormats,
            __out_ecount(pDeviceCaps->OutputFormatCount) D3DFORMAT                 *pOutputFormats,
            __out                                        DXVAHD_FILTER_RANGE_DATA  *pBrightnessRange,
            __out                                        DXVAHD_FILTER_RANGE_DATA  *pContrastRange,
            __out                                        DXVAHD_FILTER_RANGE_DATA  *pHueRange,
            __out                                        DXVAHD_FILTER_RANGE_DATA  *pSaturationRange,
            __out                                        DXVAHD_FILTER_RANGE_DATA  *pNoiseReductionRange,
            __out                                        DXVAHD_FILTER_RANGE_DATA  *pEdgeEnhancementRange,
            __out                                        DXVAHD_FILTER_RANGE_DATA  *pAnamorphicScalingRange
        )
        {
            HRESULT               hr = E_FAIL;
            UINT  uiInputFormatCount = 0;
            UINT uiOutputFormatCount = 0;
            UINT        uiFilterCaps = 0;


            #pragma region Parameter Validation
            if (!pDevice)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }

            if (!pInputFormats)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }

            if (!pOutputFormats)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }

            if (!pBrightnessRange)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }

            if (!pContrastRange)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }

            if (!pHueRange)
            {
                goto CleanUp;
            }

            if (!pSaturationRange)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }

            if (!pNoiseReductionRange)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }

            if (!pEdgeEnhancementRange)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }

            if (!pAnamorphicScalingRange)
            {
                hr = E_INVALIDARG;
                goto CleanUp;
            }
            #pragma endregion

            uiInputFormatCount  = pDeviceCaps->InputFormatCount;
            uiOutputFormatCount = pDeviceCaps->OutputFormatCount;
            uiFilterCaps        = pDeviceCaps->FilterCaps;

            #pragma region GetVideoProcessorInput/OutputFormats
            //
            // Get the VideoProcessor Input and Output formats.
            //
            hr = pDevice->GetVideoProcessorInputFormats(uiInputFormatCount,
                                                        pInputFormats);
            if (FAILED(hr))
            {
                goto CleanUp;
            }    

            hr = pDevice->GetVideoProcessorOutputFormats(uiOutputFormatCount,
                                                         pOutputFormats);
            if (FAILED(hr))
            {
                goto CleanUp;
            }
            #pragma endregion
            #pragma region GetVideoProcessorFilterRange
            if (uiFilterCaps & DXVAHD_FILTER_CAPS_BRIGHTNESS)
            {
                //
                // Gather the Brightness Range available.
                //
                hr = pDevice->GetVideoProcessorFilterRange(DXVAHD_FILTER_BRIGHTNESS,
                                                           pBrightnessRange);
                if (FAILED(hr))
                {
                    goto CleanUp;
                }
            }

            if (uiFilterCaps & DXVAHD_FILTER_CAPS_CONTRAST)
            {
                //
                // Gather the Contrast Range available.
                //
                hr = pDevice->GetVideoProcessorFilterRange(DXVAHD_FILTER_CONTRAST,
                                                           pContrastRange);
                if (FAILED(hr))
                {
                    goto CleanUp;
                }
            }

            if (uiFilterCaps & DXVAHD_FILTER_CAPS_HUE)
            {
                //
                // Gather the Hue Range available.
                //
                hr = pDevice->GetVideoProcessorFilterRange(DXVAHD_FILTER_HUE,
                                                           pHueRange);
                if (FAILED(hr))
                {
                    goto CleanUp;
                }
            }

            if (uiFilterCaps & DXVAHD_FILTER_CAPS_SATURATION)
            {    
                //
                // Gather the Saturation Range available.
                //
                hr = pDevice->GetVideoProcessorFilterRange(DXVAHD_FILTER_SATURATION,
                                                           pSaturationRange);
                if (FAILED(hr))
                {
                    goto CleanUp;
                }
            }

            if (uiFilterCaps & DXVAHD_FILTER_CAPS_NOISE_REDUCTION)
            {    
                //
                // Gather the Noise Reduction Range available.
                //
                hr = pDevice->GetVideoProcessorFilterRange(DXVAHD_FILTER_NOISE_REDUCTION,
                                                           pNoiseReductionRange);
                if (FAILED(hr))
                {
                    goto CleanUp;
                }
            }

            if (uiFilterCaps & DXVAHD_FILTER_CAPS_EDGE_ENHANCEMENT)
            {    
                //
                // Gather the Edge Enhancement Range available.
                //
                hr = pDevice->GetVideoProcessorFilterRange(DXVAHD_FILTER_EDGE_ENHANCEMENT,
                                                           pEdgeEnhancementRange);
                if (FAILED(hr))
                {
                    goto CleanUp;
                }
            }

            if (uiFilterCaps & DXVAHD_FILTER_CAPS_ANAMORPHIC_SCALING)
            {    
                //
                // Gather the Anamorphic Scaling Range available.
                //
                hr = pDevice->GetVideoProcessorFilterRange(DXVAHD_FILTER_ANAMORPHIC_SCALING,
                                                           pAnamorphicScalingRange);
                if (FAILED(hr))
                {
                    goto CleanUp;
                }
            }
            #pragma endregion

        CleanUp:
            return hr;
        }

        HRESULT GetD3D11VideoProcessorCaps(
            __in            ID3D11VideoProcessorEnumerator  *pVPEnum,
            __deref_out     CD3D11_VP_CAPS                  **ppD3D11VPCaps,
            __out           UINT                            *pInputFormatCount,
            __out           UINT                            *pOutputFormatCount,
            __out_ecount(InputFormatCount)  DXGI_FORMAT    **ppInputFormats,
            __out_ecount(OutputFormatCount) DXGI_FORMAT    **ppOutputFormats
        )
        {

            HRESULT               hr = E_FAIL;
            CD3D11_VP_CAPS        *pD3D11VPCaps = NULL;
            D3D11_VIDEO_PROCESSOR_RATE_CONVERSION_CAPS *pRateConversionCaps = NULL;
            UINT InputFormatCount = 0;
            UINT OutputFormatCount = 0;

            #pragma region Parameter Validation
            if (!pVPEnum)
            {
                return E_INVALIDARG;
            }

            if (!ppD3D11VPCaps)
            {
                return E_INVALIDARG;
            }
            #pragma endregion

            
            pD3D11VPCaps = new CD3D11_VP_CAPS;
            if( NULL == pD3D11VPCaps )
            {
                return E_FAIL;
            }

            //
            // Get the Video Processor caps
            //
            hr = pVPEnum->GetVideoProcessorCaps(&pD3D11VPCaps->VPCaps);
            if (FAILED(hr))
            {
                SAFE_DELETE(pD3D11VPCaps);
                return hr;
            }  

            if( 0 == pD3D11VPCaps->VPCaps.RateConversionCapsCount )
            {
                SAFE_DELETE(pD3D11VPCaps);
                return E_FAIL;
            }
            
            pRateConversionCaps = new D3D11_VIDEO_PROCESSOR_RATE_CONVERSION_CAPS[pD3D11VPCaps->VPCaps.RateConversionCapsCount];
            if( NULL == pRateConversionCaps )
            {
                SAFE_DELETE(pD3D11VPCaps);
                return E_FAIL;
            }

            for( UINT RateConversionCapsIndex = 0;
                 RateConversionCapsIndex < pD3D11VPCaps->VPCaps.RateConversionCapsCount;
                 RateConversionCapsIndex++ )
            {
                hr = pVPEnum->GetVideoProcessorRateConversionCaps(RateConversionCapsIndex,&pRateConversionCaps[RateConversionCapsIndex]);
                if( FAILED(hr) )
                {
                    SAFE_DELETE_ARRAY(pRateConversionCaps);
                    SAFE_DELETE(pD3D11VPCaps);
                    return hr;
                }
            }

            //
            // Get formats
            //
            for( UINT FormatIndex = 0;
                FormatIndex < DXGI_FORMAT_COUNT;
                FormatIndex++ )
            {
                DXGI_FORMAT chkFormat = DXGIFormats[FormatIndex];
                UINT Flags = 0;
                hr = pVPEnum->CheckVideoProcessorFormat(chkFormat, &Flags);
                if( SUCCEEDED(hr) )
                {
                    if( D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_INPUT & Flags )
                    {
                        InputFormatCount++;
                    }
                    if( D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT & Flags )
                    {
                        OutputFormatCount++;
                    }
                }
            }
            
            if( 0 == InputFormatCount || 0 == OutputFormatCount )
            {
                SAFE_DELETE_ARRAY(pRateConversionCaps);
                SAFE_DELETE(pD3D11VPCaps);
                return E_FAIL;
            }
    
            //
            // Add formats to list
            //
            DXGI_FORMAT  *pInputFormats = NULL;
            DXGI_FORMAT  *pOutputFormats = NULL;
            pInputFormats  = new DXGI_FORMAT[InputFormatCount];
            if( NULL == pInputFormats )
            {
                SAFE_DELETE_ARRAY(pRateConversionCaps);
                SAFE_DELETE(pD3D11VPCaps);
                return E_FAIL;
            }
            pOutputFormats = new DXGI_FORMAT[OutputFormatCount];
            if( NULL == pInputFormats )
            {
                SAFE_DELETE_ARRAY(pRateConversionCaps);
                SAFE_DELETE(pD3D11VPCaps);
                SAFE_DELETE_ARRAY(pInputFormats);
                return E_FAIL;
            }

            UINT InputFormatsAdded = 0;
            UINT OutputFormatsAdded = 0;
            for( UINT FormatIndex = 0;
                FormatIndex < DXGI_FORMAT_COUNT;
                FormatIndex++ )
            {
                DXGI_FORMAT chkFormat = DXGIFormats[FormatIndex];
                UINT Flags = 0;
                hr = pVPEnum->CheckVideoProcessorFormat(chkFormat, &Flags);
                if( SUCCEEDED(hr) )
                {
                    if( D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_INPUT & Flags && InputFormatsAdded <= InputFormatCount )
                    {
                        pInputFormats[InputFormatsAdded] = chkFormat;
                        InputFormatsAdded++;
                    }
                    if( D3D11_VIDEO_PROCESSOR_FORMAT_SUPPORT_OUTPUT & Flags && OutputFormatsAdded <= OutputFormatCount)
                    {
                        pOutputFormats[OutputFormatsAdded] = chkFormat;
                        OutputFormatsAdded++;
                    }
                }
            }

            *pInputFormatCount = InputFormatCount;
            *pOutputFormatCount = OutputFormatCount;
            (*ppInputFormats)   = pInputFormats;
            (*ppOutputFormats)  = pOutputFormats;
            (*ppD3D11VPCaps)    = pD3D11VPCaps;
            (*ppD3D11VPCaps)->pCustomRates = pRateConversionCaps;

            return S_OK;
        }
};

#endif // __CDXVA_DEVICE_CAPS_H__