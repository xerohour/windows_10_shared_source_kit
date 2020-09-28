#ifndef _CCONTENTGENERATOR_H_
#define _CCONTENTGENERATOR_H_

#include "d3d9.h"
#include "dxerr9.h"

#include <strsafe.h>
#include <math.h>

#include "cdebugout.h"
#include "DXVACommon.h"
#include "WGFHelpers.h"
#include "CContentGeneratorTypes.h"

//
// Used for logging.
//
extern CDebugOut DebugOut;

class CContentGenerator
{
    private:
        UINT                m_uiContentCount;
        CCONTENT_DESC*      m_pContentDesc;
        CCONTENT_DESC*      m_pDefaultContentDesc;
        IDirect3DSurface9*  m_pSurface;
        D3DSURFACE_DESC     m_SurfaceDesc;
        UINT                m_uiFutureFrameCount;
        IDirect3DSurface9** m_ppFutureSurfaces;
        D3DSURFACE_DESC*    m_pFutureSurfaceDescs;
        UINT                m_uiPastFrameCount;
        IDirect3DSurface9** m_ppPastSurfaces;
        D3DSURFACE_DESC*    m_pPastSurfaceDescs;
        CD3DFMT_DESCRIPTOR  m_FormatDescriptor;

    public:
        CContentGenerator();
        ~CContentGenerator();
        HRESULT Initialize
        (
            __in                                UINT                uiFrameCount,
            __in                                UINT                uiContentCount,
            __in_ecount(uiContentCount)         CCONTENT_DESC*      pContentDesc,
            __inout_ecount(uiFrameCount)        IDirect3DSurface9*  pSurface,
            __in                                UINT                uiFutureFrameCount,
            __inout_ecount(uiFutureFrameCount)  IDirect3DSurface9** ppFutureSurfaces,
            __in                                UINT                uiPastFrameCount,
            __inout_ecount(uiPastFrameCount)    IDirect3DSurface9** ppPastSurfaces
        );
        VOID AddContent
        (
            __in                        UINT            uiContentCount,
            __in_ecount(uiContentCount) CCONTENT_TYPE*  pContentDesc
        );
        HRESULT GenerateFrame
        (
            __in    UINT            uiFrameIndex,
            __in    CFRAME_LINES    FrameLines
        );

    private:
        HRESULT GetSurfaceDescs
        (
            __in                            D3DFORMAT           SurfaceFormat,
            __in                            UINT                uiSurfaceCount,
            __in_ecount(uiSurfaceCount)     IDirect3DSurface9** ppSurfaces,
            __out_ecount(uiSurfaceCount)    D3DSURFACE_DESC*    pSurfaceDescs
        );
        HRESULT GenerateFrames
        (
            __in                            UINT                uiMainFrameIndex,
            __in                            CFRAME_LINES        FrameLines,
            __in                            CFRAME_TYPE         FrameType,
            __in                            UINT                uiFrameCount,
            __inout_ecount(uiFrameCount)    IDirect3DSurface9** ppSurfaces
        );
        HRESULT GenerateFrame
        (
            __in    UINT                uiFrameIndex,
            __in    CFRAME_LINES        FrameLines,
            __in    CFRAME_TYPE         FrameType,
            __in    D3DSURFACE_DESC     SurfaceDesc,
            __inout IDirect3DSurface9*  pSurface
        );
        HRESULT GenerateContent
        (
            __in    UINT                uiFrameIndex,
            __inout CCONTENT_DESC*      pContentDesc,
            __in    const CCONTENT_DESC DefaultContentDesc,
            __in    CFRAME_LINES        FrameLines,
            __in    CFRAME_TYPE         FrameType,
            __inout D3DLOCKED_RECT*     plrSurface,
            __in    D3DSURFACE_DESC     SurfaceDesc
        );
        VOID AdjustContentLocation
        (
            __in    UINT                    uiSurfaceWidth,
            __in    UINT                    uiSurfaceHeight,
            __inout SIZE&                   siLocation,
            __in    UINT                    uiFrameIndex,
            __in    CCONTENT_MOVEMENT_TYPE  MovementType,
            __in    float                   flMovementRate
        );
        VOID AdjustContentSize
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
        );
        VOID AdjustColorIndex
        (
            __in    UINT    uiFrameIndex,
            __in    UINT    uiColorChangeRate,
            __in    UINT    uiPaletteEntryCount,
            __out   UINT*   pColor
        );
        VOID FillAlphaChannel
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
        );
        VOID GenerateColorBars
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
        );
        VOID FillColorBarRow
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
        );
        VOID GenerateSolidColor
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
        );
        VOID GenerateLines
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
        );
        VOID GetPelOffsetAndLength
        (
            __in    UINT                uiChannelWidth,
            __in    SIZE                siCoordinates,
            __in    DXVAHD_FRAME_FORMAT FrameFormat,
            __out   BYTE                *pbPelOffset,
            __out   BYTE                *pbOffsetLength
        );
        UINT GetColor
        (
            __in    D3DCOLOR*   pPalette,
            __in    UINT        uiColorIndex,
            __in    UINT        uiMask,
            __in    UINT        uiOffset
        );
};

//
// String conversion routines
//
extern bool __stdcall FromString(CCONTENT_TYPE *pContentType, const tstring &ContentTypeString);
extern bool __stdcall FromString(CFRAME_LINES *pFrameLines, const tstring &FrameLinesString);
extern bool __stdcall FromString(CFRAME_TYPE *pFrameType, const tstring &FrameTypeString);
extern bool __stdcall FromString(CCONTENT_MOVEMENT_TYPE *pMovementType, const tstring &MovementTypeString);
extern bool __stdcall FromString(CCONTENT_SIZE_CHANGE_TYPE *pSizeChangeType, const tstring &SizeChangeTypeString);

extern tstring __stdcall ToString(const CCONTENT_TYPE &ContentType);
extern tstring __stdcall ToString(const CFRAME_LINES &FrameLines);
extern tstring __stdcall ToString(const CFRAME_TYPE &FrameType);
extern tstring __stdcall ToString(const CCONTENT_MOVEMENT_TYPE &MovementType);
extern tstring __stdcall ToString(const CCONTENT_SIZE_CHANGE_TYPE &SizeChangeType);

//
// Inline helper functions.
//
inline VOID  __DPF(int nLevel, const CHAR * szFormat, ...)
{
    CHAR szBuffer[4096] = "";

    va_list arglist;
    va_start(arglist, szFormat);
    StringCchVPrintfA
    (
        szBuffer,
        ARRAYSIZE(szBuffer),
        szFormat,
        arglist
    );
    va_end(arglist);

    DebugOut.WriteWhole(nLevel, szBuffer);
}
#endif // _CCONTENTGENERATOR_H_