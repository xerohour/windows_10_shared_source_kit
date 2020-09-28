//////////////////////////////////////////////////////////////////////////
//  DXVACommon.h
//  created:	2007/11/11
//
//  purpose: DXVA Common macros and static extern routines with defs.
//
//////////////////////////////////////////////////////////////////////////
#ifndef _DXVACOMMON_H_
#define _DXVACOMMON_H_

#include "d3d11.h"
#include "ddraw.h"
#include "d3d9.h"
#include "dxva2api.h"
#include "dxvahd.h"

#ifdef WGFTEST
    #include "WGFHelpers.h"
#else
    #include "stringutils.h"
    #include "assert.h"
#endif

#include "DXVACommonTypes.h"

DEFINE_GUID(IID_Zeroed, 0x00000000,0x0000,0x0000,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);

#ifndef STRING_TO_ENUM
#define STRING_TO_ENUM(x, y, z) \
    if ((x) == _T(#y)) \
    { \
        *z = y; \
        return true; \
    }
#endif
#ifndef ENUM_TO_STRING
#define ENUM_TO_STRING(x, y) \
    if (x == y) \
    { \
        return tstring(_T(#y)); \
    }
#endif
#ifndef SAFE_CLOSE
#define SAFE_CLOSE(handle) \
    if(handle != 0) \
    { \
        CloseHandle(handle); \
        handle = 0; \
    }
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(handle) \
    if(handle != 0) \
    { \
        free(handle); \
        handle = NULL; \
    }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(ptr) \
	if(ptr) { ptr->Release(); ptr = NULL; }
#endif
#ifndef SAFE_RELEASE_LONG
#define SAFE_RELEASE_LONG(ptr, count) \
    if(ptr) \
    { \
        for (UINT uiPtrCountIdx = 0; uiPtrCountIdx != count; uiPtrCountIdx++) \
        { \
            SAFE_RELEASE(ptr[uiPtrCountIdx]); \
        } \
        SAFE_DELETE_ARRAY(ptr); \
    }
#endif
#ifndef SAFE_RELEASE_ARRAY
#define SAFE_RELEASE_ARRAY(ptr, count) \
    if(ptr) \
    { \
        for (UINT uiPtrCountIdx = 0; uiPtrCountIdx != count; uiPtrCountIdx++) \
        { \
            SAFE_RELEASE(ptr[uiPtrCountIdx]); \
        } \
    }
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) \
	if(ptr) { delete ptr; ptr = NULL; }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(ptr) \
	if(ptr) { delete[] ptr; ptr = NULL; }
#endif
#ifndef SAFE_DELETE_LONG
#define SAFE_DELETE_LONG(ptr, count) \
    if(ptr) \
    { \
        for (UINT uiPtrCountIdx = 0; uiPtrCountIdx != count; uiPtrCountIdx++) \
        { \
            SAFE_DELETE(ptr[uiPtrCountIdx]); \
        } \
        SAFE_DELETE_ARRAY(ptr); \
    }
#endif
#ifndef SAFE_FREE_LIBRARY
#define SAFE_FREE_LIBRARY(lib) \
    if(lib) \
    { \
        FreeLibrary(lib); \
        lib = NULL; \
    }
#endif
#ifndef SAFE_COTASK_MEM_FREE
#define SAFE_COTASK_MEM_FREE(ptr) \
    if(ptr) \
    { \
        CoTaskMemFree(ptr); \
        ptr = NULL; \
    }
#endif
#ifndef SAFE_DELETE_CDXVA2_DECODER_DEVICE_ARRAY
#define SAFE_DELETE_CDXVA2_DECODER_DEVICE_ARRAY(ptr, count)\
    if(ptr) \
    { \
        for (UINT uiPtrCountIdx = 0; uiPtrCountIdx != count; uiPtrCountIdx++) \
        { \
            SAFE_COTASK_MEM_FREE((ptr[uiPtrCountIdx].pRenderTargetFormats)); \
        } \
        SAFE_DELETE_ARRAY(ptr); \
    }
#endif
#ifndef LOCK
#define LOCK(mutex) \
    if (mutex && MULTITHREAD_SURFACE_COMPARE) \
    { \
        WaitForSingleObject(mutex, INFINITE); \
    } 
#endif
#ifndef UNLOCK
#define UNLOCK(mutex) \
    if (mutex && MULTITHREAD_SURFACE_COMPARE) \
    { \
        ReleaseMutex(mutex); \
    } 
#endif
#ifndef DXVAHD_RATIONAL_TO_UINT
#define DXVAHD_RATIONAL_TO_UINT(dxvaHDRational) \
    DXVAHDRationalToUINT(dxvaHDRational)
#endif
#ifndef D3DCOLOR_TO_DXVAHD_COLOR_RGBA
#define D3DCOLOR_TO_DXVAHD_COLOR_RGBA(d3dColor) \
    D3DColorToDXVAHDColorRGBA(d3dColor)
#endif
#ifndef D3DCOLOR_TO_DXVAHD_COLOR_YCbCrA
#define D3DCOLOR_TO_DXVAHD_COLOR_YCbCrA(d3dColor) \
    D3DColorToDXVAHDColorYCbCrA(d3dColor)
#endif
#ifndef D3DCOLOR_TO_STRING
#define D3DCOLOR_TO_STRING(d3dColor) \
    D3DColorToString(d3dColor)
#endif
#ifndef DXVA2_DECODER_GUID_TO_STRING
#define DXVA2_DECODER_GUID_TO_STRING(dxva2DecoderGuid) \
    DXVA2DecoderGuidToString(dxva2DecoderGuid)
#endif
#ifndef DXVA2_DECODER_BUFFER_TYPE_TO_STRING
#define DXVA2_DECODER_BUFFER_TYPE_TO_STRING(dxva2BufferType) \
    DXVA2DecoderBufferTypeToString(dxva2BufferType)
#endif
#ifndef DXVAHD_DEVICE_CAPS_TO_STRING 
#define DXVAHD_DEVICE_CAPS_TO_STRING(dxvaHDDeviceCaps) \
	DXVAHDDeviceCapsToString(dxvaHDDeviceCaps)
#endif
#ifndef DDXVAHD_FEATURE_CAPS_TO_STRING 
#define DDXVAHD_FEATURE_CAPS_TO_STRING(dxvaHDFeatureCaps) \
	DXVAHDFeatureCapsToString(dxvaHDFeatureCaps)
#endif
#ifndef DXVAHD_FILTER_CAPS_TO_STRING 
#define DXVAHD_FILTER_CAPS_TO_STRING(dxvaHDFilterCaps) \
	DXVAHDFilterCapsToString(dxvaHDFilterCaps)
#endif
#ifndef DXVAHD_INPUT_FORMAT_CAPS_TO_STRING 
#define DXVAHD_INPUT_FORMAT_CAPS_TO_STRING(dxvaHDInputFormatCaps) \
	DXVAHDInputFormatCapsToString(dxvaHDInputFormatCaps)
#endif
#ifndef DXVAHD_ITELECINE_CAPS_TO_STRING 
#define DXVAHD_ITELECINE_CAPS_TO_STRING(dxvaHDITelecineCaps) \
	DXVAHDITelecineCapsToString(dxvaHDITelecineCaps)
#endif
#ifndef DXVAHD_PROCESSOR_CAPS_TO_STRING 
#define DXVAHD_PROCESSOR_CAPS_TO_STRING(dxvaHDProcessorCaps) \
	DXVAHDProcessorCapsToString(dxvaHDProcessorCaps)
#endif

//
// D3D Support functions.
//
extern HRESULT D3D9CreateDevice(__in        IDirect3D9*         pD3D9,
                                __in        UINT                uiAdapterIndex,
                                __in        HWND                hWnd,
                                __deref_out IDirect3DDevice9**  ppD3DDevice9);

//
// Support query routines
//
extern const BOOL IsCapSupported(
    __in    const UINT  Caps, 
    __in    DWORD       Cap
);
extern VOID GetChannelOffsetAndMask(
    __in    UINT    uiChannelIndex, 
    __out   UINT*   puiOffset, 
    __out   UINT*   puiMask
);
extern UINT GetPalettizedColorIndex(
    __in    UINT        uiPaletteEntryCount, 
    __in    UINT        uiColorIndex, 
    __in    D3DFORMAT   Format
);
extern BOOL GetD3DFormatDescriptor(
    __in    const D3DFORMAT*    pFormat, 
    __out   CD3DFMT_DESCRIPTOR& D3DFormatDescriptor
);
extern VOID SetFormatInitialOffset(
    __inout CD3DFMT_DESCRIPTOR&     FormatDesc, 
    __inout const D3DSURFACE_DESC&  SurfaceDesc, 
    __in    UINT                    uiPitch
);

//
// Misc other conversion routins.
//
extern const UINT DXVAHDRationalToUINT(const DXVAHD_RATIONAL dxvaHDRational);
extern const DXVAHD_COLOR_RGBA D3DColorToDXVAHDColorRGBA(const D3DCOLOR Color);
extern const DXVAHD_COLOR_YCbCrA D3DColorToDXVAHDColorYCbCrA(const D3DCOLOR Color);
extern const tstring DXVAHDColorToString_RGBA(const DXVAHD_COLOR  Color);
extern const tstring DXVAHDColorToString_YCbCr(const DXVAHD_COLOR  Color);
extern const tstring DXVAHDColorToString(const BYTE CH0, const BYTE CH1, const BYTE CH2, const BYTE CH3);
extern const DXVA2_VideoDesc ToDXVA2_VideoDesc(const CDXVA_CONTENT_DESC ContentDesc);
extern const DXVAHD_CONTENT_DESC ToDXVAHD_CONTENT_DESC(const CDXVA_CONTENT_DESC ContentDesc);
extern const D3D11_VIDEO_PROCESSOR_CONTENT_DESC ToD3D11VP_CONTENT_DESC(const CDXVA_CONTENT_DESC  ContentDesc);
extern const CDXVA_CONTENT_DESC ToCDXVA_CONTENT_DESC(const DXVA2_VideoDesc VideoDesc);
extern const CDXVA_CONTENT_DESC ToCDXVA_CONTENT_DESC(const DXVAHD_CONTENT_DESC VideoDesc);
extern const CDXVA_CONTENT_DESC ToCDXVA_CONTENT_DESC(const D3D11_VIDEO_PROCESSOR_CONTENT_DESC VideoDesc);

//
// String conversion routines
//
extern const tstring D3DColorToString(const D3DCOLOR& D3DColor);
extern const tstring YUVFormatToString(const D3DFORMAT& YUVFormat);
extern const tstring DXVA2DecoderGuidToString(GUID dxva2DecoderGuid);
extern const tstring DXVA2DecoderBufferTypeToString(UINT dxva2BufferType);
extern const TCHAR*  DXVAHDDeviceCapsToString(UINT dxvaHDDeviceCaps);
extern const TCHAR*  DXVAHDFeatureCapsToString(UINT dxvaHDFeatureCaps);
extern const TCHAR*  DXVAHDFilterCapsToString(UINT dxvaHDFilterCaps);
extern const TCHAR*  DXVAHDInputFormatCapsToString(UINT dxvaHDInputFormatCaps);
extern const TCHAR*  DXVAHDITelecineCapsToString(UINT dxvaHDITelecineCaps);
extern const TCHAR*  DXVAHDProcessorCapsToString(UINT dxvaHDProcessorCaps);
extern const TCHAR*  CChannelIndexToString(UINT ChannelIndex, BOOL RGBChannelLayout);

//
// String converstion routines which can be used for the framework as well.
//
extern bool __stdcall FromString(D3DMULTISAMPLE_TYPE* pD3DMultiSampleType, const tstring& D3DMultiSampleTypeString);
extern bool __stdcall FromString(D3DPOOL* pD3DPool, const tstring& D3DPoolString);
extern bool __stdcall FromString(D3DFORMAT* pD3DFormatType, const tstring& D3DFormatTypeString);
extern bool __stdcall FromString(D3DSWAPEFFECT* pSwapEffectType, const tstring& SwapEffectTypeString);
extern bool __stdcall FromString(DXVAHD_DEVICE_USAGE* pDeviceUsage, const tstring& DeviceUsageString);
extern bool __stdcall FromString(DXVAHD_FRAME_FORMAT* pDXVAHDFrameFormat, const tstring& DXVAHDFrameFormatString);
extern bool __stdcall FromString(DXVAHD_DEVICE_TYPE* pDXVAHDDeviceType, const tstring& DXVAHDDeviceTypeString);
extern bool __stdcall FromString(DXVAHD_SURFACE_TYPE* pDXVAHDSurfaceType, const tstring& DXVAHDSurfaceTypeString);
extern bool __stdcall FromString(DXVAHD_FILTER* pDXVAHDFilter, const tstring& DXVAHDFilterString);
extern bool __stdcall FromString(DXVAHD_FILTER_RANGE_DATA* pDXVAHDFilterRangeData, const tstring& DXVAHDFilterRangeDataString);
extern bool __stdcall FromString(DXVAHD_ALPHA_FILL_MODE* pDXVAHDAlphaFillMode, const tstring& DXVAHDAlphaFillModeString);
extern bool __stdcall FromString(DXVAHD_COLOR_RGBA* pDXVAHDColorRGBA, const tstring& DXVAHDColorRGBAString);
extern bool __stdcall FromString(DXVAHD_COLOR_YCbCrA* pDXVAHDColorYCbCrA, const tstring& DXVAHDColorYCbCrAString);
extern bool __stdcall FromString(DXVAHD_OUTPUT_RATE* pDXVAHDOutputRate, const tstring& DXVAHDOutputRateString);
extern bool __stdcall FromString(CDXVAHD_SURFACE_TYPE* pCDXVAHDSurfaceType, const tstring& CDXVAHDSurfaceTypeString);
extern bool __stdcall FromString(CSCALE_TYPE* pScaleType, const tstring& ScaleTypeString);
extern bool __stdcall FromString(D3D11_VIDEO_USAGE* pUsage, const tstring& UsageString);
extern bool __stdcall FromString(D3D11_VIDEO_FRAME_FORMAT* pFrameFormat, const tstring& FrameFormatString);
extern bool __stdcall FromString(D3D11_VIDEO_PROCESSOR_FILTER* pFilter, const tstring& FilterString);
extern bool __stdcall FromString(D3D11_VIDEO_PROCESSOR_OUTPUT_RATE* pOutputRate, const tstring& OutputRateString);
extern bool __stdcall FromString(D3D11_VIDEO_PROCESSOR_STEREO_FORMAT* pStereoFormat, const tstring& StereoFormatString);
extern bool __stdcall FromString(D3D11_VIDEO_PROCESSOR_STEREO_FLIP_MODE* pStereoFlipMode, const tstring& StereoFlipModeString);
extern bool __stdcall FromString(D3D11_VIDEO_PROCESSOR_ROTATION* pRotatationValue, const tstring& RotateValString);
extern bool __stdcall FromString(DXGI_MODE_ROTATION* pRotatationValue, const tstring& RotateValString);


extern tstring __stdcall ToString(const GUID& Guid);
extern tstring __stdcall ToString(const D3DMULTISAMPLE_TYPE& D3DMultiSampleType);
extern tstring __stdcall ToString(const D3DPOOL& D3DPool);
extern tstring __stdcall ToString(const D3DFORMAT& D3DFormatType);
extern tstring __stdcall ToString(const D3DSWAPEFFECT& SwapEffectType);
extern tstring __stdcall ToString(const DXVAHD_DEVICE_USAGE& DeviceUsageType);
extern tstring __stdcall ToString(const DXVA2_Frequency &Frequency);
extern tstring __stdcall ToString(const DXVAHD_RATIONAL& DXVAHDRational);
extern tstring __stdcall ToString(const DXVAHD_FRAME_FORMAT& DXVAHDFrameFormatType);
extern tstring __stdcall ToString(const DXVAHD_DEVICE_TYPE& DXVAHDDeviceType);
extern tstring __stdcall ToString(const DXVAHD_SURFACE_TYPE& DXVAHDSurfaceType);
extern tstring __stdcall ToString(const DXVAHD_FILTER& DXVAHDFilterType);
extern tstring __stdcall ToString(const DXVAHD_FILTER_RANGE_DATA& DXVAHDFilterRangeData);
extern tstring __stdcall ToString(const DXVAHD_ALPHA_FILL_MODE& DXVAHDAlphaFillModeType);
extern tstring __stdcall ToString(const DXVAHD_COLOR_RGBA& DXVAHDColorRGBAType);
extern tstring __stdcall ToString(const DXVAHD_COLOR_YCbCrA& DXVAHDColorYCbCrAType);
extern tstring __stdcall ToString(const DXVAHD_COLOR  &DXVAHDColor, BOOL bYCbCr);
extern tstring __stdcall ToString(const DXVAHD_OUTPUT_RATE& DXVAHDOutputRateType);
extern tstring __stdcall ToString(const CDXVAHD_SURFACE_TYPE& CDXVAHDSurfaceType);
extern tstring __stdcall ToString(const CSCALE_TYPE& ScaleType);
extern tstring __stdcall ToString(const UINT uiChannelIndex, BOOL RGBChannelLayout);
extern tstring __stdcall ToString(const DXGI_RATIONAL& Rational);
extern tstring __stdcall ToString(const DXGI_MODE_SCALING &Scalling);
extern tstring __stdcall ToString(const DXGI_MODE_SCANLINE_ORDER &ScanLineOrder);
extern tstring __stdcall ToString(const D3D11_VIDEO_FRAME_FORMAT &FrameFormat);
extern tstring __stdcall ToString(const D3D11_VIDEO_USAGE& Usage);
extern tstring __stdcall ToString(const D3D11_VIDEO_FRAME_FORMAT& FrameFormatType);
extern tstring __stdcall ToString(const D3D11_VIDEO_PROCESSOR_FILTER& Filter);
extern tstring __stdcall ToString(const D3D11_VIDEO_PROCESSOR_OUTPUT_RATE& OutputRate);
extern tstring __stdcall ToString(const D3D11_VIDEO_PROCESSOR_STEREO_FORMAT& StereoFormatType);
extern tstring __stdcall ToString(const D3D11_VIDEO_PROCESSOR_STEREO_FLIP_MODE& StereoFlipModeType);
extern tstring __stdcall ToString(const D3D11_VIDEO_PROCESSOR_ROTATION& RotatationValue);
extern tstring __stdcall ToString(const DXGI_MODE_ROTATION& RotatationValue);


//
// Only support test result to string if this is a test.
//
#ifdef WGFTEST
    extern tstring __stdcall ToString(const TEST_RESULT &TestResult);
#endif

//
// Inline routines (operator overrides)
//
inline BOOL operator == (const DXVA2_Frequency& x, const DXVA2_Frequency& y)
{
    if (x.Denominator != y.Denominator) return FALSE;
    if (x.Numerator != y.Numerator) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVA2_Frequency& x, const DXVA2_Frequency& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVA2_ExtendedFormat& x, const DXVA2_ExtendedFormat& y)
{
    if (x.NominalRange != y.NominalRange) return FALSE;
    if (x.SampleFormat != y.SampleFormat) return FALSE;
    if (x.value != y.value) return FALSE;
    if (x.VideoChromaSubsampling != y.VideoChromaSubsampling) return FALSE;
    if (x.VideoLighting != y.VideoLighting) return FALSE;
    if (x.VideoPrimaries != y.VideoPrimaries) return FALSE;
    if (x.VideoTransferFunction != y.VideoTransferFunction) return FALSE;
    if (x.VideoTransferMatrix != y.VideoTransferMatrix) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVA2_ExtendedFormat& x, const DXVA2_ExtendedFormat& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVA2_VideoDesc& x, const DXVA2_VideoDesc& y)
{
    if (x.Format != y.Format) return FALSE;
    if (x.InputSampleFreq != y.InputSampleFreq) return FALSE;
    if (x.OutputFrameFreq != y.OutputFrameFreq) return FALSE;
    if (x.Reserved != y.Reserved) return FALSE;
    if (x.SampleFormat != y.SampleFormat) return FALSE;
    if (x.SampleHeight != y.SampleHeight) return FALSE;
    if (x.SampleWidth != y.SampleWidth) return FALSE;
    if (x.UABProtectionLevel != y.UABProtectionLevel) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVA2_VideoDesc& x, const DXVA2_VideoDesc& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVA2_ConfigPictureDecode& x, const DXVA2_ConfigPictureDecode& y)
{
    if (x.Config4GroupedCoefs != y.Config4GroupedCoefs) return FALSE;
    if (x.ConfigBitstreamRaw != y.ConfigBitstreamRaw) return FALSE;
    if (x.ConfigDecoderSpecific != y.ConfigDecoderSpecific) return FALSE;
    if (x.ConfigHostInverseScan != y.ConfigHostInverseScan) return FALSE;
    if (x.ConfigIntraResidUnsigned != y.ConfigIntraResidUnsigned) return FALSE;
    if (x.ConfigMBcontrolRasterOrder != y.ConfigMBcontrolRasterOrder) return FALSE;
    if (x.ConfigMinRenderTargetBuffCount != y.ConfigMinRenderTargetBuffCount) return FALSE;
    if (x.ConfigResid8Subtraction != y.ConfigResid8Subtraction) return FALSE;
    if (x.ConfigResidDiffAccelerator != y.ConfigResidDiffAccelerator) return FALSE;
    if (x.ConfigResidDiffHost != y.ConfigResidDiffHost) return FALSE;
    if (x.ConfigSpatialHost8or9Clipping != y.ConfigSpatialHost8or9Clipping) return FALSE;
    if (x.ConfigSpatialResid8 != y.ConfigSpatialResid8) return FALSE;
    if (x.ConfigSpatialResidInterleaved != y.ConfigSpatialResidInterleaved) return FALSE;
    if (x.ConfigSpecificIDCT != y.ConfigSpecificIDCT) return FALSE;
    if (x.guidConfigBitstreamEncryption != y.guidConfigBitstreamEncryption) return FALSE;
    if (x.guidConfigMBcontrolEncryption != y.guidConfigMBcontrolEncryption) return FALSE;
    if (x.guidConfigResidDiffEncryption != y.guidConfigResidDiffEncryption) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVA2_ConfigPictureDecode& x, const DXVA2_ConfigPictureDecode& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_BLT_STATE_TARGET_RECT_DATA& x, const DXVAHD_BLT_STATE_TARGET_RECT_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_BLT_STATE_TARGET_RECT_DATA& x, const DXVAHD_BLT_STATE_TARGET_RECT_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_BLT_STATE_BACKGROUND_COLOR_DATA& x, const DXVAHD_BLT_STATE_BACKGROUND_COLOR_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_BLT_STATE_BACKGROUND_COLOR_DATA& x, const DXVAHD_BLT_STATE_BACKGROUND_COLOR_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_BLT_STATE_OUTPUT_COLOR_SPACE_DATA& x, const DXVAHD_BLT_STATE_OUTPUT_COLOR_SPACE_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_BLT_STATE_OUTPUT_COLOR_SPACE_DATA& x, const DXVAHD_BLT_STATE_OUTPUT_COLOR_SPACE_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_BLT_STATE_ALPHA_FILL_DATA& x, const DXVAHD_BLT_STATE_ALPHA_FILL_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_BLT_STATE_ALPHA_FILL_DATA& x, const DXVAHD_BLT_STATE_ALPHA_FILL_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_BLT_STATE_CONSTRICTION_DATA& x, const DXVAHD_BLT_STATE_CONSTRICTION_DATA& y)
{
    //if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    if (x.Enable != y.Enable) return FALSE;
    if (x.Size.cx != y.Size.cx) return FALSE;
    if (x.Size.cy != y.Size.cy) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVAHD_BLT_STATE_CONSTRICTION_DATA& x, const DXVAHD_BLT_STATE_CONSTRICTION_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE_DATA& x, const DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE_DATA x, const DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE_DATA y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_OUTPUT_RATE_DATA& x, const DXVAHD_STREAM_STATE_OUTPUT_RATE_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_OUTPUT_RATE_DATA& x, const DXVAHD_STREAM_STATE_OUTPUT_RATE_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_SOURCE_RECT_DATA& x, const DXVAHD_STREAM_STATE_SOURCE_RECT_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_SOURCE_RECT_DATA& x, const DXVAHD_STREAM_STATE_SOURCE_RECT_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_DESTINATION_RECT_DATA& x, const DXVAHD_STREAM_STATE_DESTINATION_RECT_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_DESTINATION_RECT_DATA& x, const DXVAHD_STREAM_STATE_DESTINATION_RECT_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_ALPHA_DATA& x, const DXVAHD_STREAM_STATE_ALPHA_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_ALPHA_DATA& x, const DXVAHD_STREAM_STATE_ALPHA_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_PALETTE_DATA& x, const DXVAHD_STREAM_STATE_PALETTE_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_PALETTE_DATA& x, const DXVAHD_STREAM_STATE_PALETTE_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_LUMA_KEY_DATA& x, const DXVAHD_STREAM_STATE_LUMA_KEY_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_LUMA_KEY_DATA& x, const DXVAHD_STREAM_STATE_LUMA_KEY_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_ASPECT_RATIO_DATA& x, const DXVAHD_STREAM_STATE_ASPECT_RATIO_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_ASPECT_RATIO_DATA& x, const DXVAHD_STREAM_STATE_ASPECT_RATIO_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_STREAM_STATE_FILTER_DATA& x, const DXVAHD_STREAM_STATE_FILTER_DATA& y)
{
    if (memcmp(&x, &y, sizeof(x)) == 0) return TRUE;

    return FALSE;
}
inline BOOL operator != (const DXVAHD_STREAM_STATE_FILTER_DATA& x, const DXVAHD_STREAM_STATE_FILTER_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_FILTER_RANGE_DATA& x, const DXVAHD_FILTER_RANGE_DATA& y)
{
    if (x.Default != y.Default) return FALSE;
    if (x.Maximum != y.Maximum) return FALSE;
    if (x.Minimum != y.Minimum) return FALSE;
    if (x.Multiplier != y.Multiplier) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVAHD_FILTER_RANGE_DATA& x, const DXVAHD_FILTER_RANGE_DATA& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_COLOR_RGBA& x, const DXVAHD_COLOR_RGBA& y)
{
    return memcmp(&x, &y, sizeof(x)) ? TRUE : FALSE;
}
inline BOOL operator == (const DXVAHD_COLOR_YCbCrA& x, const DXVAHD_COLOR_YCbCrA& y)
{
    return memcmp(&x, &y, sizeof(x)) ? TRUE : FALSE;
}
inline BOOL operator == (const DXVAHD_CONTENT_DESC& x, const DXVAHD_CONTENT_DESC& y)
{
    if (!memcmp(&x, &y, sizeof(DXVAHD_CONTENT_DESC))) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVAHD_CONTENT_DESC& x, const DXVAHD_CONTENT_DESC& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_VPDEVCAPS& x, const DXVAHD_VPDEVCAPS& y)
{
    if (x.DeviceCaps != y.DeviceCaps) return FALSE;
    if (x.DeviceType != y.DeviceType) return FALSE;
    if (x.FeatureCaps != y.FeatureCaps) return FALSE;
    if (x.FilterCaps != y.FilterCaps) return FALSE;
    if (x.InputFormatCaps != y.InputFormatCaps) return FALSE;
    if (x.InputFormatCount != y.InputFormatCount) return FALSE;
    if (x.InputPool != y.InputPool) return FALSE;
    if (x.MaxInputStreams != y.MaxInputStreams) return FALSE;
    if (x.MaxStreamStates != y.MaxStreamStates) return FALSE;
    if (x.OutputFormatCount != y.OutputFormatCount) return FALSE;
    if (x.VideoProcessorCount != y.VideoProcessorCount) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVAHD_VPDEVCAPS& x, const DXVAHD_VPDEVCAPS& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_RATIONAL& x, const DXVAHD_RATIONAL& y)
{
    if (x.Denominator != y.Denominator) return FALSE;
    if (x.Numerator != y.Numerator) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVAHD_RATIONAL& x, const DXVAHD_RATIONAL& y)
{
    return !(x == y);
}
inline BOOL operator == (const DXVAHD_VPCAPS& x, const DXVAHD_VPCAPS& y)
{
    if (!memcmp(&x, &y, sizeof(DXVAHD_VPCAPS))) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVAHD_VPCAPS& x, const DXVAHD_VPCAPS& y)
{
    return !(x == y);
}

inline BOOL operator == (const DXVAHD_CUSTOM_RATE_DATA& x, const DXVAHD_CUSTOM_RATE_DATA& y)
{
    if (x.CustomRate != y.CustomRate) return FALSE;
    if (x.InputFramesOrFields != y.InputFramesOrFields) return FALSE;
    if (x.InputInterlaced != y.InputInterlaced) return FALSE;
    if (x.OutputFrames != y.OutputFrames) return FALSE;

    return TRUE;
}
inline BOOL operator != (const DXVAHD_CUSTOM_RATE_DATA& x, const DXVAHD_CUSTOM_RATE_DATA& y)
{
    return !(x == y);
}


//
// Inline Routines (Non operator overrides)
//
inline BOOL IsYUVFormat(D3DFORMAT Format)
{
    return Format >= D3DFORMAT('0000');
}
inline BOOL IsRGBFormat(D3DFORMAT Format)
{
    return Format < D3DFORMAT('0000');
}
inline BOOL IsVLDMode(GUID gMode)
{
    if (gMode == DXVA2_ModeMPEG2_VLD ||
        gMode == DXVA2_ModeVC1_VLD ||
        gMode == DXVA2_ModeH264_VLD_NoFGT ||
        gMode == DXVA2_ModeH264_VLD_FGT)
    {
        return TRUE;
    }

    return FALSE;
}
inline BOOL IsH264Mode(GUID gMode)
{
    if (gMode == DXVA2_ModeH264_MoComp_NoFGT ||
        gMode == DXVA2_ModeH264_MoComp_FGT ||
        gMode == DXVA2_ModeH264_IDCT_NoFGT ||
        gMode == DXVA2_ModeH264_IDCT_FGT ||
        gMode == DXVA2_ModeH264_VLD_NoFGT ||
        gMode == DXVA2_ModeH264_VLD_FGT)
    {
        return TRUE;
    }

    return FALSE;
}
inline BOOL IsVC1Mode(GUID gMode)
{
    if (gMode == DXVA2_ModeVC1_PostProc ||
        gMode == DXVA2_ModeVC1_MoComp ||
        gMode == DXVA2_ModeVC1_IDCT ||
        gMode == DXVA2_ModeVC1_VLD)
    {
        return TRUE;
    }

    return FALSE;
}
inline BOOL IsWMV9Mode(GUID gMode)
{
    if (gMode == DXVA2_ModeWMV9_PostProc ||
        gMode == DXVA2_ModeWMV9_MoComp ||
        gMode == DXVA2_ModeWMV9_IDCT)
    {
        return TRUE;
    }

    return FALSE;
}
inline BOOL IsFGTMode(GUID gMode)
{
    if (gMode == DXVA2_ModeH264_MoComp_FGT ||
        gMode == DXVA2_ModeH264_IDCT_FGT ||
        gMode == DXVA2_ModeH264_VLD_FGT)
    {
        return TRUE;
    }

    return FALSE;
}
inline BOOL IsKnownDecodeGuid(GUID gMode)
{
    if (gMode == DXVA2_ModeMPEG2_MOCOMP ||
        gMode == DXVA2_ModeWMV8_PostProc ||
        gMode == DXVA2_ModeWMV8_MoComp ||
        gMode == DXVA2_ModeWMV9_PostProc ||
        gMode == DXVA2_ModeWMV9_MoComp ||
        gMode == DXVA2_ModeWMV9_IDCT ||
        gMode == DXVA2_ModeVC1_PostProc ||
        gMode == DXVA2_ModeVC1_MoComp ||
        gMode == DXVA2_ModeVC1_IDCT ||
        gMode == DXVA2_ModeVC1_VLD ||
        gMode == DXVA2_ModeH264_MoComp_NoFGT ||
        gMode == DXVA2_ModeH264_MoComp_FGT ||
        gMode == DXVA2_ModeH264_IDCT_NoFGT ||
        gMode == DXVA2_ModeH264_IDCT_FGT ||
        gMode == DXVA2_ModeH264_VLD_NoFGT ||
        gMode == DXVA2_ModeH264_VLD_FGT)
    {
        return TRUE;
    }

    return FALSE;
}
inline BOOL IsValidSizeDenominators(UINT uiWidthDenominator, UINT uiHeightDenominator)
{
    //
    // Update surface size for any other stream
    //
    if (uiWidthDenominator == 0 || uiHeightDenominator == 0)
    {
        return FALSE;
    }

    return TRUE;
}
inline BOOL IsFilterState(DXVAHD_STREAM_STATE State)
{
    switch (State)
    {
        case DXVAHD_STREAM_STATE_FILTER_BRIGHTNESS:
        case DXVAHD_STREAM_STATE_FILTER_CONTRAST:
        case DXVAHD_STREAM_STATE_FILTER_HUE:
        case DXVAHD_STREAM_STATE_FILTER_SATURATION:
        case DXVAHD_STREAM_STATE_FILTER_NOISE_REDUCTION:
        case DXVAHD_STREAM_STATE_FILTER_EDGE_ENHANCEMENT:
        case DXVAHD_STREAM_STATE_FILTER_ANAMORPHIC_SCALING:
            return true;
        default:
            return false;
    }
}
inline BOOL IsRectPixelsInsideSurface(RECT rcToValidate, D3DSURFACE_DESC SurfaceDesc)
{
    UINT uiSurfaceHeight = 0;
    UINT  uiSurfaceWidth = 0;
    UINT    uiRectHeight = 0;
    UINT     uiRectWidth = 0;

    uiSurfaceHeight = SurfaceDesc.Height;
    uiSurfaceWidth  = SurfaceDesc.Width;
    uiRectHeight    = rcToValidate.bottom - rcToValidate.top;
    uiRectWidth     = rcToValidate.right - rcToValidate.left;

    //
    // Pixels would be off surface.
    //
    if (rcToValidate.top + uiRectHeight > uiSurfaceHeight)
    {
        return false;
    }

    //
    // Pixels would be off surface.
    //
    if (rcToValidate.left + uiRectWidth > uiSurfaceWidth)
    {
        return false;
    }


    return true;    
}
inline BOOL IsPalettizedFormat(D3DFORMAT Format)
{
    switch (Format)
    {
        case D3DFMT_AI44:
        case D3DFMT_IA44:
        case D3DFMT_A8P8:
        case D3DFMT_P8:
            return TRUE;
    }

    return FALSE;
}
inline BOOL IsCommonYUVFormat(D3DFORMAT Format)
{
    return (Format == D3DFMT_YUVCOMMON) ? TRUE : FALSE;
}
inline BOOL IsDefaultStateData(DXVAHD_BLT_STATE State, const VOID* pData)
{
    switch (State)
    {
        
        case DXVAHD_BLT_STATE_TARGET_RECT:
            if ((*(DXVAHD_BLT_STATE_TARGET_RECT_DATA*)pData) ==
                DefaultTargetRectData)
            {
                return true;
            }
            break;
        case DXVAHD_BLT_STATE_BACKGROUND_COLOR:
            if ((*(DXVAHD_BLT_STATE_BACKGROUND_COLOR_DATA*)pData) ==
                DefaultBackgroundColorData)
            {
                return true;
            }
            break;
        case DXVAHD_BLT_STATE_OUTPUT_COLOR_SPACE:
            if ((*(DXVAHD_BLT_STATE_OUTPUT_COLOR_SPACE_DATA*)pData) ==
                DefaultOutputColorSpaceData)
            {
                return true;
            }
            break;
        case DXVAHD_BLT_STATE_ALPHA_FILL:
            if ((*(DXVAHD_BLT_STATE_ALPHA_FILL_DATA*)pData) ==
                DefaultAlphaFillData)
            {
                return true;
            }
            break;
        case DXVAHD_BLT_STATE_CONSTRICTION:
            if ((*(DXVAHD_BLT_STATE_CONSTRICTION_DATA*)pData) ==
                DefaultConstrictionData)
            {
                return true;
            }
            break;
    default:
            break;
    }

    return false;
}
inline BOOL IsDefaultStateData(DXVAHD_STREAM_STATE State, const VOID* pData)
{
    switch (State)
    {
        case DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE:
            if ((*(DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE_DATA*)pData) ==
                DefaultInputColorSpaceData)
            {
                return true;
            }
            break;
        case DXVAHD_STREAM_STATE_OUTPUT_RATE:
            if ((*(DXVAHD_STREAM_STATE_OUTPUT_RATE_DATA*)pData) ==
                DefaultOutputRateData)
            {
                return true;
            }
            break;
        case DXVAHD_STREAM_STATE_SOURCE_RECT:
            if ((*(DXVAHD_STREAM_STATE_SOURCE_RECT_DATA*)pData) ==
                DefaultSourceRectData)
            {
                return true;
            }
            break;
        case DXVAHD_STREAM_STATE_DESTINATION_RECT:
            if ((*(DXVAHD_STREAM_STATE_DESTINATION_RECT_DATA*)pData) ==
                DefaultDestinationRectData)
            {
                return true;
            }
            break;
        case DXVAHD_STREAM_STATE_ALPHA:
            if ((*(DXVAHD_STREAM_STATE_ALPHA_DATA*)pData) ==
                DefaultAlphaData)
            {
                return true;
            }
            break;
        case DXVAHD_STREAM_STATE_PALETTE:
            if ((*(DXVAHD_STREAM_STATE_PALETTE_DATA*)pData) ==
                DefaultPaletteData)
            {
                return true;
            }
            break;
        case DXVAHD_STREAM_STATE_LUMA_KEY:
            if ((*(DXVAHD_STREAM_STATE_LUMA_KEY_DATA*)pData) ==
                DefaultLumaKeyData)
            {
                return true;
            }
            break;
        case DXVAHD_STREAM_STATE_ASPECT_RATIO:
            if ((*(DXVAHD_STREAM_STATE_ASPECT_RATIO_DATA*)pData) ==
                DefaultAspectRatioData)
            {
                return true;
            }
            break;
        case DXVAHD_STREAM_STATE_FILTER_BRIGHTNESS:
        case DXVAHD_STREAM_STATE_FILTER_CONTRAST:
        case DXVAHD_STREAM_STATE_FILTER_HUE:
        case DXVAHD_STREAM_STATE_FILTER_SATURATION:
        case DXVAHD_STREAM_STATE_FILTER_NOISE_REDUCTION:
        case DXVAHD_STREAM_STATE_FILTER_EDGE_ENHANCEMENT:
        case DXVAHD_STREAM_STATE_FILTER_ANAMORPHIC_SCALING:
            if ((*(DXVAHD_STREAM_STATE_FILTER_DATA*)pData) ==
                DefaultFilterData)
            {
                return true;
            }
            break;
        default:
            break;
    }

    return false;
}
inline BOOL CoTaskMemCpy(VOID* pDestination, VOID* pSource, ULONG ulAllocSize)
{
    pDestination = CoTaskMemAlloc(ulAllocSize);
    if (!pDestination)
    {
        return FALSE;
    }
    memset(pDestination, 0, ulAllocSize);
    memcpy(pDestination, pSource, ulAllocSize);

    return TRUE;
}
#endif // _DXVACOMMON_H_

#include "CDXVA_DeviceCaps.h"