#include "CContentGenerator.h"

CContentGenerator::CContentGenerator()
    : m_uiContentCount(0),
      m_pContentDesc(NULL),
      m_pDefaultContentDesc(NULL),
      m_pSurface(NULL),
      m_uiFutureFrameCount(0),
      m_ppFutureSurfaces(NULL),
      m_pFutureSurfaceDescs(NULL),
      m_uiPastFrameCount(0),
      m_ppPastSurfaces(NULL),
      m_pPastSurfaceDescs(NULL)
{
    memset(&m_SurfaceDesc, 0, sizeof(D3DSURFACE_DESC));
    memset(&m_FormatDescriptor, 0, sizeof(CD3DFMT_DESCRIPTOR));
}
CContentGenerator::~CContentGenerator()
{
    SAFE_DELETE_ARRAY(m_pContentDesc);
    SAFE_DELETE_ARRAY(m_pDefaultContentDesc);
    SAFE_DELETE_ARRAY(m_pFutureSurfaceDescs);
    SAFE_DELETE_ARRAY(m_pPastSurfaceDescs);
}
/*****************************************************************************
 * Function: Initialize(UINT, UINT, CCONTENT*, UINT, IDirect3DSurface9*, UINT,
 *                      IDirect3DSurface9*, UINT, IDirect3DSurface9*)
 *
 * Description: 
 *
 * Returns: HRESULT : S_OK, E_INVALIDARG
 *****************************************************************************/
HRESULT CContentGenerator::Initialize
(
    __in                                UINT                uiFrameCount,
    __in                                UINT                uiContentCount,
    __in_ecount(uiContentCount)         CCONTENT_DESC*      pContentDesc,
    __inout_ecount(uiFrameCount)        IDirect3DSurface9*  pSurface,
    __in                                UINT                uiFutureFrameCount,
    __inout_ecount(uiFutureFrameCount)  IDirect3DSurface9** ppFutureSurfaces,
    __in                                UINT                uiPastFrameCount,
    __inout_ecount(uiPastFrameCount)    IDirect3DSurface9** ppPastSurfaces
)
{
    HRESULT                               hr = S_OK;
    D3DFORMAT                  SurfaceFormat = D3DFMT_UNKNOWN;
    D3DSURFACE_DESC              SurfaceDesc;
    D3DSURFACE_DESC*     pFutureSurfaceDescs = NULL;
    D3DSURFACE_DESC*       pPastSurfaceDescs = NULL;
    CCONTENT_DESC*         pLocalContentDesc = NULL;
    CCONTENT_DESC*       pDefaultContentDesc = NULL;
    CD3DFMT_DESCRIPTOR      FormatDescriptor = CD3DFMT_DESCRIPTOR_DEFAULT;

    #pragma region Parameter Validation
    if (!pContentDesc)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::Initialize: Invalid pointer " \
                "pContentTypes was null."
            )
        );
        return E_INVALIDARG;
    }
    if (!pSurface)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::Initialize: Invalid pointer pSurface " \
                "was null."
            )
        );

        return E_INVALIDARG;
    }
    if (uiFutureFrameCount > 0 && !ppFutureSurfaces)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::Initialize: Invalid pointer " \
                "ppFutureSurfaces was null while uiFutureFrameCount " \
                "was greater than 0."
            )
        );

        return E_INVALIDARG;
    }
    if (uiPastFrameCount > 0 && !ppPastSurfaces)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::Initialize: Invalid pointer " \
                "ppPastSurfaces was null while uiPastFrameCount was " \
                "greater than 0."
            )
        );
        return E_INVALIDARG;
    }
    #pragma endregion

    //
    // First obtain the Surface description for the main surface/frame this is 
    // to later determine the surface format for generating proper pixel format.
    //
    memset(&SurfaceDesc, 0, sizeof(D3DSURFACE_DESC));
    hr = pSurface->GetDesc(&SurfaceDesc);
    if (FAILED(hr))
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::Initialize: Failed to obtain a " \
                "description for the main surface with hr: %s."
            ),
            HR_TO_STRING(hr)
        );
        goto Cleanup;
    }
    SurfaceFormat = SurfaceDesc.Format;

    #pragma region Memory Allocation / GetDesc Past/Future
    if (uiFutureFrameCount > 0)
    {
        //
        // Allocate the memory for the future Surface descriptions.
        //
        pFutureSurfaceDescs = new D3DSURFACE_DESC[uiFutureFrameCount];
        if (!pFutureSurfaceDescs)
        {
            __DPF
            (
                1,
                _T
                (
                    "CContentGenerator::Initialize: Failed to allocate " \
                    "enough memory for %d D3DSURFACE_DESCs."
                ),
                uiFutureFrameCount
            );
            goto Cleanup;
        }
        memset(pFutureSurfaceDescs, 
               0, 
               sizeof(D3DSURFACE_DESC) * uiFutureFrameCount);

        //
        // Obtain the array of future surface descs which indexes will match that
        // of the corresponding surface arrangement
        //
        hr = GetSurfaceDescs(SurfaceFormat,
                             uiFutureFrameCount,
                             ppFutureSurfaces,
                             pFutureSurfaceDescs);
        if (FAILED(hr))
        {
            goto Cleanup;
        }
    }

    if (uiPastFrameCount > 0)
    {
        //
        // Allocate the memory for the past Surface descriptions.
        //
        pPastSurfaceDescs = new D3DSURFACE_DESC[uiPastFrameCount];
        if (!pPastSurfaceDescs)
        {
            __DPF
            (
                1,
                _T
                (
                    "CContentGenerator::Initialize: Failed to allocate " \
                    "enough memory for %d D3DSURFACE_DESCs."
                ),
                uiPastFrameCount
            );
            goto Cleanup;
        }
        memset(pPastSurfaceDescs, 
               0, 
               sizeof(D3DSURFACE_DESC) * uiPastFrameCount);

        //
        // Obtain the array of past surface descs which indexes will match that
        // of the corresponding surface arrangement
        //
        hr = GetSurfaceDescs(SurfaceFormat,
                             uiPastFrameCount,
                             ppPastSurfaces,
                             pPastSurfaceDescs);
        if (FAILED(hr))
        {
            goto Cleanup;
        }
    }

    //
    // Allocate the memory for the dynamic content values.
    //
    pLocalContentDesc = new CCONTENT_DESC[uiContentCount];
    if (!pLocalContentDesc)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::Initialize: Failed to allocate enough " \
                "memory for %d CONTENT."
            ),
            uiContentCount
        );
        goto Cleanup;
    }
    memset(pLocalContentDesc, 0, sizeof(CCONTENT_DESC) * uiContentCount);
    memcpy(pLocalContentDesc, 
           pContentDesc, 
           sizeof(CCONTENT_DESC) * uiContentCount);

    //
    // Allocate the memory for the default content values.
    //
    pDefaultContentDesc = new CCONTENT_DESC[uiContentCount];
    if (!pDefaultContentDesc)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::Initialize: Failed to allocate enough " \
                "memory for %d CONTENT."
            ),
            uiContentCount
        );
        goto Cleanup;
    }
    memset(pDefaultContentDesc, 0, sizeof(CCONTENT_DESC) * uiContentCount);
    memcpy(pDefaultContentDesc,
           pContentDesc, 
           sizeof(CCONTENT_DESC) * uiContentCount);
    #pragma endregion

    //
    // This obtains the necessary descriptor information for the given input
    // format. This provides a way to use the static table for adding additional
    // input formats for generating content.
    //
    if (!GetD3DFormatDescriptor(&SurfaceFormat, FormatDescriptor))
    {
        //
        // Logging occurs inside the routine.
        //
        hr = E_FAIL;
        goto Cleanup;
    }

Cleanup:
    if (SUCCEEDED(hr))
    {
        m_uiContentCount      = uiContentCount;
        m_pContentDesc        = pLocalContentDesc;
        m_pDefaultContentDesc = pDefaultContentDesc;
        m_pSurface            = pSurface;
        m_SurfaceDesc         = SurfaceDesc;
        m_uiFutureFrameCount  = uiFutureFrameCount;
        m_ppFutureSurfaces    = ppFutureSurfaces;
        m_pFutureSurfaceDescs = pFutureSurfaceDescs;
        m_uiPastFrameCount    = uiPastFrameCount;
        m_ppPastSurfaces      = ppPastSurfaces;
        m_pPastSurfaceDescs   = pPastSurfaceDescs;
        m_FormatDescriptor    = FormatDescriptor;
    }
    else
    {
        SAFE_DELETE_ARRAY(pFutureSurfaceDescs);
        SAFE_DELETE_ARRAY(pPastSurfaceDescs);
        SAFE_DELETE_ARRAY(pLocalContentDesc);
        SAFE_DELETE_ARRAY(pDefaultContentDesc);
    }

    return hr;   
}
/*****************************************************************************
 * Function: GenerateFrame(UINT, CFRAME_LINES)
 *
 * Description: 
 *
 * Returns: HRESULT : S_OK, E_INVALIDARG
 *****************************************************************************/
HRESULT CContentGenerator::GenerateFrame
(
    __in    UINT            uiFrameIndex,
    __in    CFRAME_LINES    FrameLines
)
{
    HRESULT hr = S_OK;

    #pragma region Pointer Validation
    if (!m_pSurface)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::GenerateFrame: Invalid pointer " \
                "m_pSurface was null. Make sure ContentGenerator::Initialize " \
                "has been called before generating the frames."
            )
        );
        return E_POINTER;
    }
    if (m_uiFutureFrameCount > 0 && !m_ppFutureSurfaces)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::GenerateFrame: Invalid pointer " \
                "m_ppFutureSurfaces was null and the future frame count was " \
                "%d. Make sure ContentGenerator::Initialize has been called " \
                "before generating the frames."
            ),
            m_uiFutureFrameCount
        );
        return E_POINTER;
    }
    if (m_uiPastFrameCount > 0 && !m_ppPastSurfaces)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::GenerateFrame: Invalid pointer " \
                "m_ppPastSurfaces was null and the past frame count was " \
                "%d. Make sure ContentGenerator::Initialize has been called " \
                "before generating the frames."
            ),
            m_uiPastFrameCount
        );
        return E_POINTER;
    }
    #pragma endregion

    //
    // Generate the Main frame content.
    //
    hr = GenerateFrames(uiFrameIndex,
                        FrameLines,
                        CFRAME_TYPE_MAIN,
                        1,
                        &m_pSurface);
    if (FAILED(hr))
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::GenerateFrame: GenerateFrames failed to " \
                "Generate the main frame with hr: %s"
            ),
            HR_TO_STRING(hr)
        );
        goto Cleanup;
    }

    //
    // Only attempt to generate if there are future frames to generate.
    //
    if (m_uiFutureFrameCount > 0)
    {
        //
        // Generate the Future frames content.
        //
        hr = GenerateFrames(uiFrameIndex,
                            FrameLines,
                            CFRAME_TYPE_FUTURE,
                            m_uiFutureFrameCount,
                            m_ppFutureSurfaces);
        if (FAILED(hr))
        {
            __DPF
            (
                1,
                _T
                (
                    "CContentGenerator::GenerateFrame: GenerateFrames " \
                    "failed to Generate the future frames with hr: %s"
                ),
                HR_TO_STRING(hr)
            );
            goto Cleanup;
        }
    }

    //
    // Only attempt to generate if there are past frames to generate.
    //
    if (m_uiPastFrameCount > 0)
    {
        //
        // Generate the Past frames content.
        //
        hr = GenerateFrames(uiFrameIndex,
                            FrameLines,
                            CFRAME_TYPE_PAST,
                            m_uiPastFrameCount,
                            m_ppPastSurfaces);
        if (FAILED(hr))
        {
            __DPF
            (
                1,
                _T
                (
                    "CContentGenerator::GenerateFrame: GenerateFrames failed to " \
                    "Generate the past frames with hr: %s"
                ),
                HR_TO_STRING(hr)
            );
            goto Cleanup;
        }
    }

Cleanup:
    return hr;
}
/*****************************************************************************
 * Function: GetSurfaceDescs(D3DFORMAT, UINT, IDirect3DSurface9*, 
 *                           D3DSURFACE_DESC*)
 *
 * Description: 
 *
 * Returns: HRESULT : S_OK, E_INVALIDARG
 *****************************************************************************/
HRESULT CContentGenerator::GetSurfaceDescs
(
    __in                            D3DFORMAT           SurfaceFormat,
    __in                            UINT                uiSurfaceCount,
    __in_ecount(uiSurfaceCount)     IDirect3DSurface9** ppSurfaces,
    __out_ecount(uiSurfaceCount)    D3DSURFACE_DESC*    pSurfaceDescs
)
{
    HRESULT hr = S_OK;

    //
    // Validate that surface descs have been allocated.
    //
    if (!pSurfaceDescs)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::GetSurfaceDescs: Invalid parameter " \
                "pSurfaceDescs."
            )
        );

        return E_INVALIDARG;
    }

    //
    // Iterate over the surfaces obtaining each surface desc.
    //
    for 
    (
        UINT uiSurfaceIndex = 0; 
        uiSurfaceIndex != uiSurfaceCount; 
        uiSurfaceIndex++
    )
    {
        IDirect3DSurface9* pSurface = NULL;        
        pSurface = ppSurfaces[uiSurfaceIndex];

        //
        // Get the surface desc.
        //
        hr = pSurface->GetDesc(&pSurfaceDescs[uiSurfaceIndex]);
        if (FAILED(hr))
        {
            __DPF
            (
                1,
                _T
                (
                    "CContentGenerator::GetSurfaceDescs: Failed to obtain a " \
                    "description for surface %d with hr: %s."
                ),
                uiSurfaceIndex,
                HR_TO_STRING(hr)
            );

            break;
        }

        //
        // Check to validate the format of the surfaces match that of the input
        // format because if they don't the content generator will fail to 
        // generate correctly.
        //
        if (SurfaceFormat != pSurfaceDescs[uiSurfaceIndex].Format)
        {
            __DPF
            (
                1,
                _T
                (
                    "CContentGenerator::GetSurfaceDescs: The surface formats " \
                    "did not match. ContentGenerator only generates matching " \
                    "surface format content."
                )
            );

            break;
        }
    }

    if (FAILED(hr))
    {
        //
        // clean up the surface descs if there were any failures.
        //
        memset(pSurfaceDescs, 0, sizeof(D3DSURFACE_DESC)*uiSurfaceCount);
    }

    return hr;
}
/*****************************************************************************
 * Function: GenerateFrames(UINT, CFRAME_LINES, CFRAME_TYPE, UINT, 
 *                          IDirect3DSurface9**)
 *
 * Description: 
 *
 * Returns: HRESULT : S_OK, E_INVALIDARG
 *****************************************************************************/
HRESULT CContentGenerator::GenerateFrames
(
    __in                            UINT                uiMainFrameIndex,
    __in                            CFRAME_LINES        FrameLines,
    __in                            CFRAME_TYPE         FrameType,
    __in                            UINT                uiFrameCount,
    __inout_ecount(uiFrameCount)    IDirect3DSurface9** ppSurfaces
)
{
    HRESULT hr = S_OK;

    if (!ppSurfaces)
    {
        return E_INVALIDARG;
    }

    //
    // Generate each frame one by one.
    //
    for (UINT uiFrameIndex = 0; uiFrameIndex != uiFrameCount; uiFrameIndex++)
    {
        IDirect3DSurface9*  pSurface = NULL;
        D3DSURFACE_DESC  SurfaceDesc;
        UINT  uiFrameIndexToGenerate = 0;
        int    nFrameIndexDifference = 0;

        //
        // If the frame count is greater than 1 the pointer is an array of
        // surfaces else it is a handle to a single pointer surface.
        //
        if (uiFrameCount > 1)
        {
            pSurface = ppSurfaces[uiFrameIndex];
        }
        else
        {
            pSurface = *ppSurfaces;
        }

        //
        // Get the correct surface description.
        //
        memset(&SurfaceDesc, 0, sizeof(D3DSURFACE_DESC));
        switch (FrameType)
        {
            case CFRAME_TYPE_MAIN:
                SurfaceDesc = m_SurfaceDesc;
                uiFrameIndexToGenerate = uiMainFrameIndex;
                break;

            case CFRAME_TYPE_FUTURE:
                SurfaceDesc = m_pFutureSurfaceDescs[uiFrameIndex];
                uiFrameIndexToGenerate = uiMainFrameIndex + (uiFrameIndex + 1);
                break;

            case CFRAME_TYPE_PAST:
                SurfaceDesc = m_pPastSurfaceDescs[uiFrameIndex];
                nFrameIndexDifference = uiMainFrameIndex - (uiFrameIndex + 1);
                
                if (nFrameIndexDifference < 0)
                {
                    //
                    // Frame to generate was less than 0 so default to 0.
                    //
                    uiFrameIndexToGenerate = 0;
                }
                else
                {
                    uiFrameIndexToGenerate = uiMainFrameIndex - uiFrameIndex;
                }
                break;

            default:
                
                return E_UNEXPECTED;
        }

        //
        // Generate the frame.
        //
        hr = GenerateFrame(uiFrameIndexToGenerate,
                           FrameLines,
                           FrameType,
                           SurfaceDesc,
                           pSurface);
        if (FAILED(hr))
        {
            return hr;
        }
    }

    return hr;
}
/*****************************************************************************
 * Function: GenerateFrame(UINT, CFRAME_LINES, CFRAME_TYPE, D3DSURFACE_DESC, 
 *                         IDirect3DSurface9**)
 *
 * Description: 
 *
 * Returns: HRESULT : S_OK, E_INVALIDARG
 *****************************************************************************/
HRESULT CContentGenerator::GenerateFrame
(
    __in    UINT                uiFrameIndex,
    __in    CFRAME_LINES        FrameLines,
    __in    CFRAME_TYPE         FrameType,
    __in    D3DSURFACE_DESC     SurfaceDesc,
    __inout IDirect3DSurface9*  pSurface
)
{
    HRESULT                hr = S_OK;
    D3DLOCKED_RECT  lrSurface = {0};

    if (!pSurface)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::GenerateFrame: pSurface was null."
            )
        );
        return E_INVALIDARG;
    }

    //
    // Lock the rect before generating the content.
    //
    memset(&lrSurface, 0, sizeof(D3DLOCKED_RECT));
    hr = pSurface->LockRect(&lrSurface, NULL, D3DLOCK_NOSYSLOCK);
    if (FAILED(hr))
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGenerator::GenerateFrame: Failed to lock the " \
                "surface rect with hr: %s."
            ),
            HR_TO_STRING(hr)
        );
        goto Cleanup;
    }

    //
    // Iterate over all content types generating them all and placing them on
    // the main content.
    //
    for 
    (
        UINT uiContentDescIndex = 0; 
        uiContentDescIndex != m_uiContentCount; 
        uiContentDescIndex++
    )
    {
        CCONTENT_DESC        *pContentDesc = NULL;
        CCONTENT_DESC   DefaultContentDesc;

        pContentDesc = &m_pContentDesc[uiContentDescIndex];
        DefaultContentDesc = m_pDefaultContentDesc[uiContentDescIndex];

        //
        // Generate the content here.
        //
        hr = GenerateContent(uiFrameIndex,
                             pContentDesc,
                             DefaultContentDesc,
                             FrameLines,
                             FrameType,
                             &lrSurface,
                             SurfaceDesc);
        if (FAILED(hr))
        {
            goto Cleanup;
        }
    } // ContentType

Cleanup:
    hr = pSurface->UnlockRect();
    if (FAILED(hr))
    {
        __DPF
        (
            1,
            _T
            (
                "GenerateContent: Failed to unlock surface rect with " \
                "hr: %s."
            ),
            HR_TO_STRING(hr)
        );
    }

    return hr;
}
/*****************************************************************************
 * Function: GenerateContent(UINT, CCONTENT, D3DLOCKED_RECT*, D3DSURFACE_DESC)
 *
 * Description: 
 *
 * Returns: HRESULT : S_OK, E_INVALIDARG
 *****************************************************************************/
HRESULT CContentGenerator::GenerateContent
(
    __in    UINT                uiFrameIndex,
    __inout CCONTENT_DESC*      pContentDesc,
    __in    const CCONTENT_DESC DefaultContentDesc,
    __in    CFRAME_LINES        FrameLines,
    __in    CFRAME_TYPE         FrameType,
    __inout D3DLOCKED_RECT*     plrSurface,
    __in    D3DSURFACE_DESC     SurfaceDesc
)
{
    UNREFERENCED_PARAMETER(FrameLines);

    HRESULT                                 hr = S_OK;
    UINT                        uiSurfaceWidth = SurfaceDesc.Width;
    UINT                       uiSurfaceHeight = SurfaceDesc.Height;
    UINT                               uiWidth = 0;
    UINT                              uiHeight = 0;
    UINT                               uiPitch = (UINT)(*plrSurface).Pitch;
    CCONTENT_TYPE                  ContentType = CCONTENT_TYPE_NONE;
    DXVAHD_FRAME_FORMAT            FrameFormat = DXVAHD_FRAME_FORMAT_PROGRESSIVE;
    float                              flAlpha = 0.0;
    SIZE                         siCoordinates = {0};
    float                     flSizePercentage = 1.0;
    BOOL                               bMoving = FALSE;
    CCONTENT_MOVEMENT_TYPE        MovementType = CCONTENT_MOVEMENT_TYPE_NONE;
    float                       flMovementRate = 0.0;
    BOOL                         bSizeChanging = FALSE;
    CCONTENT_SIZE_CHANGE_TYPE SizeChangingType = CCONTENT_SIZE_CHANGE_TYPE_NONE;
    float                   flSizeChangingRate = 0.0;
    BOOL                        bColorChanging = FALSE;
    UINT                   uiColorChangingRate = 0;
    UINT                 uiSolidColorFillIndex = SOLID_COLOR_FILL_INDEX;
    UINT                   uiPaletteEntryCount = 0;
    D3DCOLOR*                         pPalette = NULL;
    
    #pragma region Argument Validation
    if (!pContentDesc)
    {
        __DPF
        (
            1,
            _T
            (
                "CContentGeneratro::GenerateContent: Invalid argument " \
                "pContentDesc is null."
            )
        );

        hr = E_INVALIDARG;
        goto Cleanup;
    }
    #pragma endregion

    //
    // Initialize the local content variables.
    //
    flAlpha             = pContentDesc->flAlpha;
    uiWidth             = pContentDesc->uiWidth;
    uiHeight            = pContentDesc->uiHeight;
    ContentType         = pContentDesc->Type;
    FrameFormat         = pContentDesc->FrameFormat;
    siCoordinates       = pContentDesc->siCoordinates;
    flSizePercentage    = pContentDesc->flSizePercentage;  
    bMoving             = pContentDesc->bMoving;
    MovementType        = pContentDesc->MovementType;
    flMovementRate      = pContentDesc->flMovementRate;
    bSizeChanging       = pContentDesc->bSizeChanging;
    SizeChangingType    = pContentDesc->SizeChangingType;
    flSizeChangingRate  = pContentDesc->flSizeChangingRate;      
    bColorChanging      = pContentDesc->bColorChanging;
    uiColorChangingRate = pContentDesc->uiColorChangingRate;
    uiPaletteEntryCount = m_FormatDescriptor.uiPaletteCount;
    pPalette            = m_FormatDescriptor.pPalette;

    if (uiFrameIndex == 0)
    {
        //
        // If this is frame 0 and the width and Height are 0 set the content 
        // to the size percentage.
        //
        uiWidth = (UINT)(uiSurfaceWidth * flSizePercentage);
        uiHeight = (UINT)(uiSurfaceHeight * flSizePercentage);
    }

    //
    // Adjust the location of the content accordingly.
    //
    if (bMoving)
    {
        AdjustContentLocation(uiSurfaceWidth,
                              uiSurfaceHeight,
                              siCoordinates,
                              uiFrameIndex,
                              MovementType,
                              flMovementRate);
    }

    //
    // Adjust the size accordingly
    //
    if (bSizeChanging)
    {
        AdjustContentSize(uiWidth,
                          uiHeight,
                          DefaultContentDesc,
                          uiSurfaceWidth,
                          uiSurfaceHeight,
                          flSizePercentage,
                          uiFrameIndex,
                          SizeChangingType,
                          flSizeChangingRate);
    }

    //
    // Adjust the color if this was color changing type content.
    //
    if (bColorChanging && uiColorChangingRate > 0 &&
        (ContentType == CCONTENT_TYPE_SOLID_COLOR ||
         ContentType == CCONTENT_TYPE_BOX))
    {
        AdjustColorIndex(uiFrameIndex,
                         uiColorChangingRate,
                         uiPaletteEntryCount,
                         &uiSolidColorFillIndex);
    }

    //
    // For some planar YUV formats the initial offset can not be determined
    // until after the Width and Height of the surface are set. This routine
    // adjusts the chroma planes.
    //
    SetFormatInitialOffset(m_FormatDescriptor,
                           SurfaceDesc,
                           uiPitch);

    //
    // Iterate over the channels filling them with specified content.
    //
    for 
    (
        UINT uiChannelIndex = 0; 
        uiChannelIndex != MAX_CHANNELS; 
        uiChannelIndex++
    )
    {
        CD3DCHANNEL_DESCRIPTOR   ChannelDescriptor = {0};
        BYTE                            *pbChannel = NULL;
        UINT                    uiDistToNextSample = 0;
        BYTE                            *pRectBits = (BYTE*)(*plrSurface).pBits;
        UINT                        uiChannelWidth = 0;
        UINT                       uiChannelHeight = 0;
        
        if (uiChannelIndex >= MAX_CHANNELS)
        {
            break;
        }

        //
        // Fill in the descriptor variables for generating the content.
        //
        ChannelDescriptor = m_FormatDescriptor.D3DChannelDescriptors[uiChannelIndex];
        pbChannel = (BYTE*)pRectBits + ChannelDescriptor.InitialOffset;
        uiDistToNextSample = ChannelDescriptor.DistToNextSample;
        uiPitch = (UINT)((*plrSurface).Pitch * ChannelDescriptor.PitchFactor);

        //
        // Adjust the sampling for the Horizontal and Vertical planes.
        //
        if (ChannelDescriptor.HSubSampling > 0)
        {
            uiChannelWidth = uiWidth / ChannelDescriptor.HSubSampling;
            //siCoordinates.cy /= ChannelDescriptor.HSubSampling;
        }
        if (ChannelDescriptor.VSubSampling > 0)
        {
            uiChannelHeight = uiHeight / ChannelDescriptor.VSubSampling;
            //siCoordinates.cx /= ChannelDescriptor.VSubSampling;
        }

        if (uiChannelIndex == CCHANNEL_ALPHA && !m_FormatDescriptor.bIsPalettized)
        {
                FillAlphaChannel(pbChannel,
                                 uiChannelIndex,
                                 siCoordinates,
                                 FrameFormat,
                                 FrameLines,
                                 uiChannelWidth,
                                 uiChannelHeight,
                                 uiDistToNextSample,
                                 uiPitch,
                                 1.0f);
                continue;
        }

        switch (ContentType)
        {
            case CCONTENT_TYPE_NONE:
                break;
            case CCONTENT_TYPE_COLOR_BARS:
                GenerateColorBars(pbChannel,
                                  uiChannelIndex,
                                  siCoordinates,
                                  FrameFormat,
                                  FrameLines,
                                  uiChannelWidth,
                                  uiChannelHeight,
                                  uiDistToNextSample,
                                  uiPitch,
                                  uiPaletteEntryCount,
                                  pPalette);
                break;
            case CCONTENT_TYPE_SOLID_COLOR:
                GenerateSolidColor(pbChannel,
                                   uiChannelIndex,
                                   siCoordinates,
                                   FrameFormat,
                                   FrameLines,
                                   uiChannelWidth,
                                   uiChannelHeight,
                                   uiDistToNextSample,
                                   uiPitch,
                                   uiPaletteEntryCount,
                                   pPalette,
                                   uiSolidColorFillIndex);
                    break;
            case CCONTENT_TYPE_LINES_V:
                GenerateLines(pbChannel,
                              uiChannelIndex,
                              siCoordinates,
                              FrameFormat,
                              FrameLines,
                              uiChannelWidth,
                              uiChannelHeight,
                              uiDistToNextSample,
                              uiPitch,
                              uiPaletteEntryCount,
                              pPalette,
                              CORIENTATION_VERTICAL);
                break;
            case CCONTENT_TYPE_LINES_H:
                GenerateLines(pbChannel,
                              uiChannelIndex,
                              siCoordinates,
                              FrameFormat,
                              FrameLines,
                              uiChannelWidth,
                              uiChannelHeight,
                              uiDistToNextSample,
                              uiPitch,
                              uiPaletteEntryCount,
                              pPalette,
                              CORIENTATION_HORIZONTAL);
                break;
            case CCONTENT_TYPE_BOX:
                /*GenerateSolidColor(pbChannel,
                                   uiChannelIndex,
                                   siCoordinates,
                                   FrameFormat,
                                   FrameLines,
                                   uiChannelWidth,
                                   uiChannelHeight,
                                   uiDistToNextSample,
                                   uiPitch,
                                   uiPaletteEntryCount,
                                   pPalette,
                                   uiSolidColorFillIndex);*/
                break;
            default:
                __DPF
                (
                    1,
                    _T
                    (
                        "GenerateContent: Invalid Content Type specified: %u"
                    ),
                    ContentType
                );
                break;
        } // switch (ContentType)
    } // Channel

    //
    // Update the content location and size for the reference parameter.
    // This allows the content to actually update and not reset each exit of
    // the routine. This should only be updated when the main frame content
    // is generated, or there will be multiple updates to the wrong frames.
    //
    if (FrameType == CFRAME_TYPE_MAIN)
    {
        (*pContentDesc).uiWidth       = uiWidth;
        (*pContentDesc).uiHeight      = uiHeight;
        (*pContentDesc).siCoordinates = siCoordinates;
    }

Cleanup:
    return hr;
}
/*****************************************************************************
 * Function: AdjustContentLocation(UINT, UINT, SIZE&, UINT, CCONTENT_MOVEMENT_TYPE, 
 *                                 float)
 *
 * Description: 
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::AdjustContentLocation
(
    __in    UINT                    uiSurfaceWidth,
    __in    UINT                    uiSurfaceHeight,
    __inout SIZE&                   siLocation,
    __in    UINT                    uiFrameIndex,
    __in    CCONTENT_MOVEMENT_TYPE  MovementType,
    __in    float                   flMovementRate
)
{
    UINT    uiPixelsToMoveHorizontally = 0;
    UINT      uiPixelsToMoveVertically = 0;

    //
    // Don't do anything with the content until the second frame.
    //
    if (uiFrameIndex == 0)
    {
        return;
    }

    //
    // Determine the pixels to move which will be the width of the surface or
    // height of the surface multiplied by the percentage of the surface to 
    // move.
    //
    uiPixelsToMoveHorizontally = (UINT)(uiSurfaceWidth * flMovementRate);
    uiPixelsToMoveVertically = (UINT)(uiSurfaceHeight * flMovementRate);

    if ((siLocation.cx + uiPixelsToMoveHorizontally) > uiSurfaceWidth)
    {
        //
        // The content would have moved completely off the surface so lets reset
        // the location.
        //
        siLocation.cx = 0;
    }
    if ((siLocation.cy + uiPixelsToMoveVertically) > uiSurfaceHeight)
    {
        //
        // The content would have moved completely off the surface so lets reset
        // the location.
        //
        siLocation.cy = 0;
    }

    switch (MovementType)
    {
        case CCONTENT_MOVEMENT_TYPE_NONE:
            break;
        case CCONTENT_MOVEMENT_TYPE_HORIZONTAL:
            siLocation.cx += uiPixelsToMoveHorizontally;
            break;
        case CCONTENT_MOVEMENT_TYPE_VERTICAL:
            siLocation.cy += uiPixelsToMoveVertically;
            break;
        case CCONTENT_MOVEMENT_TYPE_CIRCULAR:
            // E_NOTIMPL yet
            break;
        case CCONTENT_MOVEMENT_TYPE_DIAGONALLY:
            siLocation.cx += uiPixelsToMoveHorizontally;
            siLocation.cy += uiPixelsToMoveVertically;
            break;
        default:
            break;
    }

    return;
}
/*****************************************************************************
 * Function: AdjustContentSize(UINT, UINT, UINT, UINT, float, UINT,
 *                             CCONTENT_MOVEMENT_TYPE, float)
 *
 * Description: 
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::AdjustContentSize
(
    __inout UINT                        &uiContentWidth,
    __inout UINT                        &uiContentHeight,
    __in    CCONTENT_DESC               DefaultContentDesc,
    __in    UINT                        uiSurfaceWidth,
    __in    UINT                        uiSurfaceHeight,
    __in    float                       flContentSizePercentage,
    __in    UINT                        uiFrameIndex,
    __in    CCONTENT_SIZE_CHANGE_TYPE   SizeChangingType,
    __in    float                       flSizeChangingRate
)
{
    UINT    uiWidthIncrease = 0;
    UINT   uiHeightIncrease = 0;
    UINT    uiWidthDecrease = 0;
    UINT   uiHeightDecrease = 0;

    //
    // Don't do anything with the content until the second frame.
    //
    if (uiFrameIndex == 0)
    {
        return;
    }

    //
    // Determine the pixels to move which will be the width of the surface or
    // height of the surface multiplied by the percentage of the surface to 
    // move.
    //
    uiWidthIncrease = (UINT)(uiContentWidth * flSizeChangingRate);
    uiHeightIncrease = (UINT)(uiContentHeight * flSizeChangingRate);
    uiWidthDecrease = (UINT)(uiContentWidth * flSizeChangingRate);
    uiHeightDecrease = (UINT)(uiContentHeight * flSizeChangingRate);

    if ((uiContentWidth + uiWidthIncrease) > uiSurfaceWidth ||
        (uiContentWidth - uiWidthDecrease) <= 0)
    {
        //
        // The content would have either been larger than the surface or less
        // than 0 or 1 in size so reset the content to the original size.
        //
        uiContentWidth = DefaultContentDesc.uiWidth;
    }

    if ((uiContentHeight + uiHeightIncrease) > uiSurfaceHeight ||
        (uiContentHeight - uiHeightDecrease) <= 0)
    {
        //
        // The content would have been larger than the surface so lets reset
        // the size of the content.
        //
        uiContentHeight = DefaultContentDesc.uiHeight;
    }

    switch (SizeChangingType)
    {
        case CCONTENT_SIZE_CHANGE_TYPE_NONE:
            break;
        case CCONTENT_SIZE_CHANGE_TYPE_SHRINK:
            uiContentWidth -= uiWidthDecrease;
            uiContentHeight -= uiHeightDecrease;
            break;
        case CCONTENT_SIZE_CHANGE_TYPE_STRETCH:
            uiContentWidth -= uiWidthIncrease;
            uiContentHeight -= uiHeightIncrease;
            break;
        case CCONTENT_SIZE_CHANGE_TYPE_ANGLE:
            // E_NOTIMPL yet
            break;
        default:
            break;
    }

    return;
}
/*****************************************************************************
 * Function: AdjustColorIndex(UINT, UINT, UINT, UINT*)
 *
 * Description: 
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::AdjustColorIndex
(
    __in    UINT    uiFrameIndex,
    __in    UINT    uiColorChangeRate,
    __in    UINT    uiPaletteEntryCount,
    __out   UINT*   pColor
)
{
    //
    // Hack expecting the FrameCount of a scene will be the
    //
    UINT uiColor = uiFrameIndex;

    //
    // Protect against over run of colors available in the palette.
    //
    if (uiColor < uiPaletteEntryCount)
    {
        *pColor = uiColor;
    }
    else
    {
        *pColor = 0;
    }

    return;
}
/*****************************************************************************
 * Function: FillAlphaChannel(BYTE*, UINT, UINT, UINT, UINT, UINT, float)
 *
 * Description: Fills the alpha channel with the alpha value provided.
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::FillAlphaChannel
(
    __inout BYTE*               pbChannel,
    __in    UINT                uiChannelIndex,
    __in    SIZE                siCoordinates,
    __in    DXVAHD_FRAME_FORMAT FrameFormat,
    __in    CFRAME_LINES        FrameLines,
    __in    UINT                uiWidth,
    __in    UINT                uiHeight,
    __in    UINT                uiDistToNextSample,
    __in    UINT                uiPitch,
    __in    float               fAlpha
)
{
    BYTE*           pbLine = pbChannel;
    UINT          uiOffset = 0;
    UINT            uiMask = 0;
    UINT           uiAlpha = 0;
    BYTE        bPelOffset = 0;
    BYTE     bOffsetLength = 0;

    //
    // Get the offset and mask for the channel.
    //
    GetChannelOffsetAndMask(uiChannelIndex, &uiOffset, &uiMask);

    //
    // Get the Pixel Offset and the length
    //
    GetPelOffsetAndLength(uiWidth,
                          siCoordinates,
                          FrameFormat,
                          &bPelOffset,
                          &bOffsetLength);

    //  TODO: generalize this for n-bit formats
    uiAlpha = (UINT)min(255, (abs(fAlpha) * 256));

    //
    // Adjust location of the bits to fill. (y)
    //
    pbLine += siCoordinates.cy * uiPitch;

    for (UINT uiLineIndex = 0; uiLineIndex < uiHeight; uiLineIndex++)
    {
        //
        // Adjust location of the bits to fill. (x)
        //
        pbChannel += siCoordinates.cx * uiDistToNextSample;

        for (UINT uiColumnIndex = 0; uiColumnIndex < uiWidth; uiColumnIndex++)
        {
            *pbChannel = (BYTE)uiAlpha;
            pbChannel += uiDistToNextSample;
        }

        pbLine += uiPitch;
        pbChannel = pbLine;
    }

    return;
}   //  FillAlphaChannel
/*****************************************************************************
 * Function: GenerateColorBars(BYTE*, UINT, UINT[], UINT, UINT, UINT, UINT)
 *
 * Description: generates an array of RECTs matching the TV type color bars 
 *              using extreme colors.
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::GenerateColorBars
(
    __inout BYTE*               pbChannel,
    __in    UINT                uiChannelIndex,
    __in    SIZE                siCoordinates,
    __in    DXVAHD_FRAME_FORMAT FrameFormat,
    __in    CFRAME_LINES        FrameLines,
    __in    UINT                uiWidth,
    __in    UINT                uiHeight,
    __in    UINT                uiDistToNextSample,
    __in    UINT                uiPitch,
    __in    UINT                uiPaletteEntryCount,
    __in    D3DCOLOR*           pPalette
)
{
    UINT    uiEndRowIndex0 = 0;
    UINT    uiEndRowIndex1 = 0;
    UINT    uiEndRowIndex2 = 0;
    UINT uiColorStrideRow0 = 0;
    UINT uiColorStrideRow1 = 0;
    UINT uiColorStrideRow2 = 0;

    // 8: White_75%, 9: Yellow_75%, 10: Cyan_75%, 11: Green_75%,
    // 12: Magenta_75%, 13: Red_75%, 14: Blue_75%
    const UINT ColorIndexesRow0[] = { 0, 1, 2, 3, 4, 5, 6 };
    const UINT uiColorIndexesCountRow0 = (sizeof(ColorIndexesRow0) / 
                                          sizeof(UINT));

    // 14: Blue_75%, 7: Black, 12: Magenta_75%, 7: Black, 10: Cyan_75%,
    // 7: Black, 8: White_75%
    const UINT ColorIndexesRow1[] = { 14, 7, 12, 7, 10, 7, 8 };
    const UINT uiColorIndexesCountRow1 = (sizeof(ColorIndexesRow1) / 
                                          sizeof(UINT));

    // 17: Quadrature, 0: White, 16: Inphase, 7: Black, 14: Black_-4%, 
    // 15: Black_+4%
    const UINT ColorIndexesRow2[] = { 17, 0, 16, 7, 14, 15 };
    const UINT uiColorIndexesCountRow2 = (sizeof(ColorIndexesRow2) / 
                                          sizeof(UINT));

    //
    // This will provide the vertical stride for a given row of bars so the 
    // Channel will use the appropriate color for the right length of bits.
    //
    uiEndRowIndex0 = (uiHeight * 2 / 3);
    uiEndRowIndex1 = (uiHeight * 3 / 4);
    uiEndRowIndex2 = uiHeight;

    //
    // This determines the Color stride. How long a given color fills the 
    // channel. 
    //
    uiColorStrideRow0 = (UINT)ceil((double)(uiWidth / uiColorIndexesCountRow0));
    uiColorStrideRow1 = (UINT)ceil((double)(uiWidth / uiColorIndexesCountRow1));
    uiColorStrideRow2 = (UINT)ceil((double)(uiWidth / uiColorIndexesCountRow2));

    //
    // Fill the top row of color bars.
    //
    FillColorBarRow(pbChannel, 
                    uiChannelIndex,
                    siCoordinates,
                    FrameFormat,
                    FrameLines,
                    0, 
                    uiEndRowIndex0,
                    uiWidth, 
                    uiDistToNextSample, 
                    uiPitch,
                    uiPaletteEntryCount,
                    pPalette,
                    uiColorIndexesCountRow0, 
                    (UINT*)ColorIndexesRow0, 
                    uiColorStrideRow0);

    //
    // Fill the middle row of color bars.
    //
    FillColorBarRow(pbChannel,
                    uiChannelIndex,
                    siCoordinates,
                    FrameFormat,
                    FrameLines,
                    uiEndRowIndex0 - 1, 
                    uiEndRowIndex1,
                    uiWidth, 
                    uiDistToNextSample,
                    uiPitch,
                    uiPaletteEntryCount,
                    pPalette,
                    uiColorIndexesCountRow1, 
                    (UINT*)ColorIndexesRow1, 
                    uiColorStrideRow1);

    //
    // Fill the bottom row of color bars.
    //
    FillColorBarRow(pbChannel, 
                    uiChannelIndex,
                    siCoordinates,
                    FrameFormat,
                    FrameLines,
                    uiEndRowIndex1 - 1, 
                    uiEndRowIndex2,
                    uiWidth, 
                    uiDistToNextSample, 
                    uiPitch,
                    uiPaletteEntryCount,
                    pPalette,
                    uiColorIndexesCountRow2, 
                    (UINT*)ColorIndexesRow2, 
                    uiColorStrideRow2);

    return;
}
/*****************************************************************************
 * Function: FillColorBarRow(BYTE*, UINT, UINT, UINT, UINT, UINT, const UINT*,
 *                           UINT, UINT[])
 *
 * Description: Fills given row height and width with an array of colors based
 *              on the input Palette of colors.
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::FillColorBarRow
(
    __inout                             BYTE*               pbChannel,
    __in                                UINT                uiChannelIndex,
    __in                                SIZE                siCoordinates,
    __in                                DXVAHD_FRAME_FORMAT FrameFormat,
    __in                                CFRAME_LINES        FrameLines,
    __in                                UINT                uiInitialLineIndex,
    __in                                UINT                uiEndLineIndex,
    __in                                UINT                uiRowWidth,
    __in                                UINT                uiDistToNextSample,
    __in                                UINT                uiPitch,
    __in                                UINT                uiPaletteEntryCount,
    __in                                D3DCOLOR*           pPalette,
    __in                                UINT                uiColorIndexesCount,
    __in_ecount(uiColorIndexesCount)    const UINT          *pColorIndexes,
    __in                                UINT                uiColorStride
)
{
    BYTE*                         pbLine = pbChannel;
    UINT                        uiOffset = 0;
    UINT                          uiMask = 0;
    UINT                         uiColor = 0;
    BYTE                   bOffsetLength = FALSE;
    BYTE                      bPelOffset = 0;

    //
    // Get the offset and mask for the channel.
    //
    GetChannelOffsetAndMask(uiChannelIndex, &uiOffset, &uiMask);

    //
    // Get the Pixel Offset and the length
    //
    GetPelOffsetAndLength(uiRowWidth,
                          siCoordinates,
                          FrameFormat,
                          &bPelOffset,
                          &bOffsetLength);

    //
    // Set the bytes to the current line.
    //
    pbLine += uiInitialLineIndex * uiPitch;

    for 
    (
        UINT uiLineIndex = uiInitialLineIndex; 
        uiLineIndex < uiEndLineIndex; 
        uiLineIndex++
    )
    {
        for 
        (
            UINT uiColumnIndex = 0, uiColorIndexIndex = 0, uiStrideIndex = 0; 
            uiColumnIndex < uiRowWidth; 
            uiColumnIndex++, uiStrideIndex++
        )
        {
            if (uiColorIndexIndex >= uiColorIndexesCount)
            {
                //
                // Adjust the color index as it was out of range and we don't
                // want to fail so force it valid.
                //
                uiColorIndexIndex = uiColorIndexesCount - 1;
            }

            //
            // Since the ColorIndex in this context is actually the index of
            // the color palette for this row which is an index in itself
            // the RowColorIndex will be the actual color to use.
            //
            UINT uiColorIndex = pColorIndexes[uiColorIndexIndex];

            //
            // Some calls may contain a color index which is outside that of
            // the number of entries in the palette so truncate it.
            //
            if (uiColorIndex >= uiPaletteEntryCount)
            {
                uiColorIndex = uiPaletteEntryCount - 1;
            }

            //
            // Set the color to the lowest byte
            //
            uiColor = GetColor(pPalette,
                               uiColorIndex,
                               uiMask,
                               uiOffset);

            *pbChannel = (BYTE)uiColor;
            pbChannel += uiDistToNextSample;

            //
            // Change the color only if the stride index has reached the 
            // stride count for this row. 
            //
            if (uiStrideIndex == uiColorStride)
            {
                //
                // reset the stride index and advance the color.
                //
                uiStrideIndex = 0;
                uiColorIndexIndex++;
            }
        }

        pbLine += uiPitch;
        pbChannel = pbLine;
    }

    return;
}
/*****************************************************************************
 * Function: GenerateSolidColor(BYTE*, UINT, UINT, UINT, UINT, UINT, UINT, UINT)
 *
 * Description: Fills given channel with a solid color.
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::GenerateSolidColor
(
    __inout BYTE*               pbChannel,
    __in    UINT                uiChannelIndex,
    __in    SIZE                siCoordinates,
    __in    DXVAHD_FRAME_FORMAT FrameFormat,
    __in    CFRAME_LINES        FrameLines,
    __in    UINT                uiWidth,
    __in    UINT                uiHeight,
    __in    UINT                uiDistToNextSample,
    __in    UINT                uiPitch,
    __in    UINT                uiPaletteEntryCount,
    __in    D3DCOLOR*           pPalette,
    __in    UINT                uiFillIndex
)
{
    BYTE*       pbLine = pbChannel;
    UINT      uiOffset = 0;
    UINT        uiMask = 0;
    UINT       uiColor = 0;
    BYTE bOffsetLength = FALSE;
    BYTE    bPelOffset = 0;
    UINT uiYCoordinate = 0;
    UINT uiXCoordinate = 0;

    //
    // Get the offset and mask for the channel.
    //
    GetChannelOffsetAndMask(uiChannelIndex, &uiOffset, &uiMask);

    //
    // Get the Pixel Offset and the length
    //
    GetPelOffsetAndLength(uiWidth,
                          siCoordinates,
                          FrameFormat,
                          &bPelOffset,
                          &bOffsetLength);

    uiYCoordinate = siCoordinates.cy;// * uiPitch;
    uiXCoordinate = siCoordinates.cx;// * uiDistToNextSample;

    //
    // Some calls may contain a color index which is outside that of
    // the number of entries in the palette so truncate it.
    //
    if (uiFillIndex > uiPaletteEntryCount)
    {
        uiFillIndex = uiPaletteEntryCount - 1;
    }

    //
    // Set the color to the lowest byte
    //
    uiColor = GetColor(pPalette,
                       uiFillIndex,
                       uiMask,
                       uiOffset);

    //
    // Goto (x, y) coordinates for the rect to fill.
    //
    pbLine += uiYCoordinate;

    for (UINT uiLineIndex = 0; uiLineIndex < uiHeight; uiLineIndex++)
    {
        //
        // indent accordingly on the x axis.
        //
        pbChannel = pbLine + uiXCoordinate + bPelOffset * bOffsetLength;

        for (UINT uiColumnIndex = 0; uiColumnIndex < uiWidth; uiColumnIndex++)
        {
            *pbChannel = (BYTE)uiColor;
            pbChannel += uiDistToNextSample;
        }
        
        bOffsetLength = !bOffsetLength;
        pbLine += uiPitch;
    }

    return;
}
/*****************************************************************************
 * Function: GenerateLines(BYTE*, UINT, UINT[], UINT, UINT, UINT, UINT, 
 *                         CORIENTATION)
 *
 * Description: Fills given channel with Vertical or Horizontal bits to generate
 *              lines of multiple colors.
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::GenerateLines
(
    __inout BYTE*               pbChannel,
    __in    UINT                uiChannelIndex,
    __in    SIZE                siCoordinates,
    __in    DXVAHD_FRAME_FORMAT FrameFormat,
    __in    CFRAME_LINES        FrameLines,
    __in    UINT                uiWidth,
    __in    UINT                uiHeight,
    __in    UINT                uiDistToNextSample,
    __in    UINT                uiPitch,
    __in    UINT                uiPaletteEntryCount,
    __in    D3DCOLOR*           pPalette,
    __in    CORIENTATION        Orientation
)
{
    BYTE*       pbLine = pbChannel;
    UINT      uiOffset = 0;
    UINT        uiMask = 0;
    UINT       uiColor = 0;
    UINT uiColorStride = 0;
    BYTE    bPelOffset = 0;
    BYTE bOffsetLength = 0;

    //
    // Get the offset and mask for the channel.
    //
    GetChannelOffsetAndMask(uiChannelIndex, &uiOffset, &uiMask);

    //
    // Get the Pixel Offset and the length
    //
    GetPelOffsetAndLength(uiWidth,
                          siCoordinates,
                          FrameFormat,
                          &bPelOffset,
                          &bOffsetLength);

    if (Orientation == CORIENTATION_HORIZONTAL)
    {
        //
        // This will fill the Channel with all the colors at the correct stride.
        //
        uiColorStride = uiHeight / uiPaletteEntryCount;

        for 
        (
            UINT uiLineIndex = 0, uiColorIndex = 0, uiColorStrideIndex = 0; 
            uiLineIndex < uiHeight; 
            uiLineIndex++, uiColorStrideIndex++
        )
        {
            //
            // Set the color to the lowest byte
            //
            uiColor = GetColor(pPalette,
                               uiColorIndex,
                               uiMask,
                               uiOffset);

            for (UINT uiColumnIndex = 0; uiColumnIndex < uiWidth; uiColumnIndex++)
            {
                *pbChannel = (BYTE)uiColor;
                pbChannel += uiDistToNextSample;
            }

            pbLine += uiPitch;
            pbChannel = pbLine;

            //
            // Change the color only if the stride index has reached the 
            // stride count. 
            //
            if (uiColorStrideIndex == uiColorStride)
            {
                uiColorStrideIndex = 0;
                uiColorIndex++;
            }
        }
    }
    else if (Orientation == CORIENTATION_VERTICAL)
    {
        //
        // This will fill the Channel with all the colors at the correct stride.
        //
        uiColorStride = uiWidth / uiPaletteEntryCount;

        for (UINT uiLineIndex = 0; uiLineIndex < uiHeight; uiLineIndex++)
        {       
            for 
            (
                UINT uiColumnIndex = 0, uiColorIndex = 0, uiColorStrideIndex = 0; 
                uiColumnIndex < uiWidth; 
                uiColumnIndex++, uiColorStrideIndex++
            )
            {
                //
                // Set the color to the lowest byte
                //
                uiColor = GetColor(pPalette,
                                   uiColorIndex,
                                   uiMask,
                                   uiOffset);

                *pbChannel = (BYTE)uiColor;
                pbChannel += uiDistToNextSample;

                //
                // Change the color only if the stride index has reached the 
                // stride count. 
                //
                if (uiColorStrideIndex == uiColorStride)
                {
                    uiColorStrideIndex = 0;
                    uiColorIndex++;
                }
            }
            
            pbLine += uiPitch;
            pbChannel = pbLine;
        }
    }

    return;
} 
/*****************************************************************************
 * Function: GetPelOffsetAndLength(UINT, SIZE, CFRAME_LINES, BYTE*, BYTE*)
 *
 * Description: Obtains the Channel's offset and mask to use based on the index.
 *
 * Returns: VOID
 *****************************************************************************/
VOID CContentGenerator::GetPelOffsetAndLength
(
    __in    UINT                uiChannelWidth,
    __in    SIZE                siCoordinates,
    __in    DXVAHD_FRAME_FORMAT FrameFormat,
    __out   BYTE                *pbPelOffset,
    __out   BYTE                *pbOffsetLength
)
{
    if (FrameFormat != DXVAHD_FRAME_FORMAT_PROGRESSIVE)
    {
        //
        // Set the Offset and stride for it.
        //
        *pbPelOffset = uiChannelWidth >> 1;
        *pbOffsetLength = (((siCoordinates.cx >> 1) && (FrameFormat == DXVAHD_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST)) ||
                          ((siCoordinates.cx & 1) && (FrameFormat == DXVAHD_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST))) & 1;
    }

    return;
}
/*****************************************************************************
 * Function: GetColor(D3DCOLOR*, UINT, UINT, UINT)
 *
 * Description: Grabs the color or color index depending on the format.
 *
 * Returns: UINT : The color or color index
 *****************************************************************************/
UINT CContentGenerator::GetColor
(
    __in_ecount(m_FormatDescriptor.uiPaletteCount)  D3DCOLOR*   pPalette,
    __in                                            UINT        uiColorIndex,
    __in                                            UINT        uiMask,
    __in                                            UINT        uiOffset
)
{
    UINT uiColor = 0;
 
    if (m_FormatDescriptor.bIsPalettized)
    {
        uiColor = GetPalettizedColorIndex(m_FormatDescriptor.uiPaletteCount,
                                          uiColorIndex,
                                          m_FormatDescriptor.D3DFormat);
    }
    else
    {
        uiColor = (pPalette[uiColorIndex] & uiMask) >> (24 - uiOffset);
    }

    return uiColor;
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a CCONTENT_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    CCONTENT_TYPE   *pContentType, 
    const tstring   &ContentTypeString
)
{
    STRING_TO_ENUM(ContentTypeString, CCONTENT_TYPE_NONE, pContentType);
    STRING_TO_ENUM(ContentTypeString, CCONTENT_TYPE_COLOR_BARS, pContentType);
    STRING_TO_ENUM(ContentTypeString, CCONTENT_TYPE_SOLID_COLOR, pContentType);
    STRING_TO_ENUM(ContentTypeString, CCONTENT_TYPE_LINES_V, pContentType);
    STRING_TO_ENUM(ContentTypeString, CCONTENT_TYPE_LINES_H, pContentType);
    STRING_TO_ENUM(ContentTypeString, CCONTENT_TYPE_BOX, pContentType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a CCONTENT_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const CCONTENT_TYPE &ContentType
)
{
      ENUM_TO_STRING(ContentType, CCONTENT_TYPE_NONE);
      ENUM_TO_STRING(ContentType, CCONTENT_TYPE_COLOR_BARS);
      ENUM_TO_STRING(ContentType, CCONTENT_TYPE_SOLID_COLOR);
      ENUM_TO_STRING(ContentType, CCONTENT_TYPE_LINES_V);
      ENUM_TO_STRING(ContentType, CCONTENT_TYPE_LINES_H);
      ENUM_TO_STRING(ContentType, CCONTENT_TYPE_BOX);

      return tstring(_T("CCONTENT_TYPE_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a CFRAME_LINES.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    CFRAME_LINES   *pFrameLines, 
    const tstring   &FrameLinesString
)
{
    STRING_TO_ENUM(FrameLinesString, CFRAME_LINES_ODD, pFrameLines);
    STRING_TO_ENUM(FrameLinesString, CFRAME_LINES_EVEN, pFrameLines);
    STRING_TO_ENUM(FrameLinesString, CFRAME_LINES_ALL, pFrameLines);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a CFRAME_LINES.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const CFRAME_LINES &FrameLines
)
{
      ENUM_TO_STRING(FrameLines, CFRAME_LINES_ODD);
      ENUM_TO_STRING(FrameLines, CFRAME_LINES_EVEN);
      ENUM_TO_STRING(FrameLines, CFRAME_LINES_ALL);

      return tstring(_T("CFRAME_LINES_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a CFRAME_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    CFRAME_TYPE     *pFrameType, 
    const tstring   &FrameTypeString
)
{
    STRING_TO_ENUM(FrameTypeString, CFRAME_TYPE_MAIN, pFrameType);
    STRING_TO_ENUM(FrameTypeString, CFRAME_TYPE_FUTURE, pFrameType);
    STRING_TO_ENUM(FrameTypeString, CFRAME_TYPE_PAST, pFrameType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a CFRAME_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const CFRAME_TYPE &FrameType
)
{
      ENUM_TO_STRING(FrameType, CFRAME_TYPE_MAIN);
      ENUM_TO_STRING(FrameType, CFRAME_TYPE_FUTURE);
      ENUM_TO_STRING(FrameType, CFRAME_TYPE_PAST);

      return tstring(_T("CFRAME_TYPE_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a CCONTENT_MOVEMENT_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    CCONTENT_MOVEMENT_TYPE  *pMovementType, 
    const tstring           &MovementTypeString
)
{
    STRING_TO_ENUM(MovementTypeString, CCONTENT_MOVEMENT_TYPE_NONE, pMovementType);
    STRING_TO_ENUM(MovementTypeString, CCONTENT_MOVEMENT_TYPE_HORIZONTAL, pMovementType);
    STRING_TO_ENUM(MovementTypeString, CCONTENT_MOVEMENT_TYPE_VERTICAL, pMovementType);
    STRING_TO_ENUM(MovementTypeString, CCONTENT_MOVEMENT_TYPE_CIRCULAR, pMovementType);
    STRING_TO_ENUM(MovementTypeString, CCONTENT_MOVEMENT_TYPE_DIAGONALLY, pMovementType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a CCONTENT_MOVEMENT_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const CCONTENT_MOVEMENT_TYPE &MovementType
)
{
      ENUM_TO_STRING(MovementType, CCONTENT_MOVEMENT_TYPE_NONE);
      ENUM_TO_STRING(MovementType, CCONTENT_MOVEMENT_TYPE_HORIZONTAL);
      ENUM_TO_STRING(MovementType, CCONTENT_MOVEMENT_TYPE_VERTICAL);
      ENUM_TO_STRING(MovementType, CCONTENT_MOVEMENT_TYPE_CIRCULAR);
      ENUM_TO_STRING(MovementType, CCONTENT_MOVEMENT_TYPE_DIAGONALLY);

      return tstring(_T("CCONTENT_MOVEMENT_TYPE_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a CCONTENT_SIZE_CHANGE_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    CCONTENT_SIZE_CHANGE_TYPE   *pSizeChangeType, 
    const tstring               &SizeChangeTypeString
)
{
    STRING_TO_ENUM(SizeChangeTypeString, CCONTENT_SIZE_CHANGE_TYPE_NONE, pSizeChangeType);
    STRING_TO_ENUM(SizeChangeTypeString, CCONTENT_SIZE_CHANGE_TYPE_SHRINK, pSizeChangeType);
    STRING_TO_ENUM(SizeChangeTypeString, CCONTENT_SIZE_CHANGE_TYPE_STRETCH, pSizeChangeType);
    STRING_TO_ENUM(SizeChangeTypeString, CCONTENT_SIZE_CHANGE_TYPE_ANGLE, pSizeChangeType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a CCONTENT_SIZE_CHANGE_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const CCONTENT_SIZE_CHANGE_TYPE &SizeChangeType
)
{
      ENUM_TO_STRING(SizeChangeType, CCONTENT_SIZE_CHANGE_TYPE_NONE);
      ENUM_TO_STRING(SizeChangeType, CCONTENT_SIZE_CHANGE_TYPE_SHRINK);
      ENUM_TO_STRING(SizeChangeType, CCONTENT_SIZE_CHANGE_TYPE_STRETCH);
      ENUM_TO_STRING(SizeChangeType, CCONTENT_SIZE_CHANGE_TYPE_ANGLE);

      return tstring(_T("CCONTENT_MOVEMENT_TYPE_UNKNOWN"));
};