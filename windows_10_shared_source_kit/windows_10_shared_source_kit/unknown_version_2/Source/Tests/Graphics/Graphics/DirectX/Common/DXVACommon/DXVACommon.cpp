#include "DXVACommon.h"
#include <sstream>

/*****************************************************************************
 * Function: D3D9CreateDevice(IDirect3D9*, UINT, HWND, IDirect3DDevice9**)
 *
 * Description: Creates a windowed D3D9 Device with default Present Parameters.
 *
 * Returns: HRESULT : E_INVALIDARG, S_OK
 *****************************************************************************/
HRESULT D3D9CreateDevice
(
    __in        IDirect3D9*         pD3D9,
    __in        UINT                uiAdapterIndex,
    __in        HWND                hWnd,
    __deref_out IDirect3DDevice9**  ppD3DDevice9
)
{
    HRESULT                      hr = S_OK;
    D3DDISPLAYMODE      DisplayMode = {0};
    D3DPRESENT_PARAMETERS   Present = {0};
    IDirect3DDevice9   *pD3DDevice9 = NULL;
    DWORD                dwBehavior = 0;
    RECT                      rcWnd = {0};

    if (!pD3D9)
    {
        hr = E_INVALIDARG;
        goto Cleanup;
    }

    //
    // Get the client rect for the window. This will be used for the initial
    // size of the back buffer.
    //
    if (!GetClientRect(hWnd,
                       &rcWnd))
    {
        //
        // Can't get the client area of the rect. Try device creation with
        // some static window height/width values.
        //
        rcWnd.right = 100;
        rcWnd.left = 0;
        rcWnd.bottom = 100;
        rcWnd.top = 0;
    }

    //
    // Get the current displaymode for the device.
    //
    memset(&DisplayMode, 0, sizeof(D3DDISPLAYMODE));
    hr = pD3D9->GetAdapterDisplayMode(uiAdapterIndex,
                                      &DisplayMode);
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    //
    // Set the Present Parameters for this device. A very basic present using
    // the size of the window and the format of the desktop.
    //
    memset(&Present, 0, sizeof(D3DPRESENT_PARAMETERS));
    Present.BackBufferWidth = rcWnd.right - rcWnd.left;
    Present.BackBufferHeight = rcWnd.bottom - rcWnd.top;
    Present.BackBufferFormat = DisplayMode.Format;
    Present.hDeviceWindow = hWnd;
    Present.SwapEffect = D3DSWAPEFFECT_FLIP;
    Present.Windowed = TRUE;
    Present.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    dwBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING | 
                 D3DCREATE_FPU_PRESERVE | 
                 D3DCREATE_MULTITHREADED;

    //
    // First attempt to create the device.
    //
    hr = pD3D9->CreateDevice(uiAdapterIndex,
                             D3DDEVTYPE_HAL,
                             NULL,
                             dwBehavior,
                             &Present,
                             &pD3DDevice9);

    if (FAILED(hr))
    {
        //
        // Try again as the first call may have corrected some invalid present
        // parameters.
        //
        hr = pD3D9->CreateDevice(uiAdapterIndex,
                                 D3DDEVTYPE_HAL,
                                 NULL,
                                 dwBehavior,
                                 &Present,
                                 &pD3DDevice9);
    }

    if (SUCCEEDED(hr))
    {
        //
        // Clear the back buffer
        //
        hr = pD3DDevice9->Clear(0,
                                NULL,
                                D3DCLEAR_TARGET,
                                0,
                                0.F,
                                0);
    }
    else
    {
        //
        // Something is wrong device creation failed twice cleanup.
        //
        goto Cleanup;
    }

    //
    // Update the ref ptr.
    //
    *ppD3DDevice9 = pD3DDevice9;

Cleanup:
    if (FAILED(hr))
    {
        if (pD3DDevice9)
        {
            pD3DDevice9->Release();
            pD3DDevice9 = NULL;
        }
    }

    return hr;
}
/*****************************************************************************
 * Function: IsCapSupported(const UINT, DWORD)
 *
 * Description: Returns TRUE if the cap is supported false if it is not
 *
 * Returns: BOOL : TRUE, FALSE
 *****************************************************************************/
const BOOL IsCapSupported
(
    __in    const UINT      uiCaps,
    __in    DWORD           Cap
)
{
    if (!(uiCaps & Cap))
    {
        return FALSE;
    }

    return TRUE;
}
/*****************************************************************************
 * Function: GetChannelOffsetAndMask(UINT, UINT*, UINT*)
 *
 * Description: Obtains the Channel's offset and mask to use based on the index.
 *
 * Returns: VOID
 *****************************************************************************/
VOID GetChannelOffsetAndMask
(
    __in    UINT    uiChannelIndex,
    __out   UINT    *puiOffset,
    __out   UINT    *puiMask
)
{    
    //
    // Shift the channel offset left of the 0xFF000000 mask.
    // Set the actual mask for the channel.
    //
    *puiOffset = ((uiChannelIndex + 1) % 4) * 8;
    *puiMask = 0xFF000000 >> *puiOffset;

    return;
}
/*****************************************************************************
 * Function: GetPalettizedColorIndex(D3DFORMAT)
 *
 * Description: Gets the Color Index for given palettized format.
 *
 * Returns: UINT : The Palettized color index.
 *****************************************************************************/
UINT GetPalettizedColorIndex
(
    __in    UINT        uiPaletteEntryCount,
    __in    UINT        uiColorIndex,
    __in    D3DFORMAT   Format
)
{
    UINT uiColor = 0;

    switch(Format)
    {
        case D3DFMT_AI44:
            uiColor = (0xF << 4) | ((BYTE)uiColorIndex % uiPaletteEntryCount);
            break;

        case D3DFMT_IA44:
            uiColor = (((BYTE)uiColorIndex % uiPaletteEntryCount) << 4) | 0xF;
            break;

        case D3DFMT_A8P8:
            uiColor = (0xFF << 8) | ((BYTE)uiColorIndex % uiPaletteEntryCount);
            break;

        case D3DFMT_P8:
            uiColor = ((BYTE)uiColorIndex % uiPaletteEntryCount);
            break;
    }

    return uiColor;
}
/*****************************************************************************
 * Function: GetD3DFormatDescriptor(D3DFORMAT*, D3DFormatDescriptor&)
 *
 * Description: Gets the D3D Format Descriptor for a given D3DFormat.
 *
 * Returns: BOOL : TRUE, FALSE
 *****************************************************************************/
BOOL GetD3DFormatDescriptor
(
    __in    const D3DFORMAT*    pFormat, 
    __out   CD3DFMT_DESCRIPTOR& D3DFormatDescriptor
)
{
    BOOL bResult = FALSE;

    //
    // Iterate over the list of format descriptors determining if this format
    // is in the list
    //
    UINT uiFormatDescriptorIndex = 0;
    for 
    (
        uiFormatDescriptorIndex = 0; 
        uiFormatDescriptorIndex != D3DFORMAT_DESCRIPTORS_COUNT; 
        uiFormatDescriptorIndex++)
    {
        if ((*pFormat) == 
            D3DFormatDescriptors[uiFormatDescriptorIndex].D3DFormat)
        {
            //
            // Format descriptor found assign to output param and flag result
            //
            D3DFormatDescriptor = D3DFormatDescriptors[uiFormatDescriptorIndex];
            bResult = TRUE;
            break;
        }
    }

    return bResult;
}
/*****************************************************************************
 * Function: SetFormatInitialOffset(CD3DFMT_DESCRIPTOR& D3DSURFACE_DESC&, UINT)
 *
 * Description: The initial offset can not accurately be determined for some 
 *              planar YUV formats until the Width and the Height of the surface
 *              are known. Since the height and width are not determined until
 *              runtime this routine handles the adjustments accordingly.
 *
 * Returns: VOID
 *****************************************************************************/
VOID SetFormatInitialOffset
(
    __inout CD3DFMT_DESCRIPTOR&     FormatDesc,
    __inout const D3DSURFACE_DESC&  SurfaceDesc,
    __in    UINT                    uiPitch
)
{
    UINT       uiWidth = SurfaceDesc.Width; 
    UINT      uiHeight = SurfaceDesc.Height;
    UINT  uiYPlaneSize = uiHeight * uiPitch;
    UINT  uiXPlaneSize = (UINT)((uiHeight / FormatDesc.D3DChannelDescriptors[CCHANNEL_U].VSubSampling) *
								(uiPitch * FormatDesc.D3DChannelDescriptors[CCHANNEL_U].PitchFactor));

    //
	// There needs to be some preparation for planar formats, the initial 
    // offset for chroma planes is a product of the dimensions of the surface
	// which were unknown until now.
    //
	switch (SurfaceDesc.Format)
	{
	    case D3DFMT_YV12:
            FormatDesc.D3DChannelDescriptors[CCHANNEL_V].InitialOffset = uiYPlaneSize;
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_U].InitialOffset = uiYPlaneSize + 
                                                                         uiXPlaneSize;
		    break;

	    case D3DFMT_NV12:
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_U].InitialOffset = uiYPlaneSize;
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_V].InitialOffset = uiYPlaneSize + 
                                                                         1;
		    break;

	    case D3DFMT_NV21:
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_V].InitialOffset = uiYPlaneSize;
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_U].InitialOffset = uiYPlaneSize + 
                                                                         1;
		    break;

	    case D3DFMT_IMC1:
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_V].InitialOffset = uiYPlaneSize;
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_U].InitialOffset = uiYPlaneSize + 
                                                                         uiXPlaneSize;
		    break;

	    case D3DFMT_IMC2:
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_V].InitialOffset = uiYPlaneSize;
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_U].InitialOffset = uiYPlaneSize + 
                                                                         uiWidth / 
                                                                         FormatDesc.D3DChannelDescriptors[CCHANNEL_U].HSubSampling;
		    break;

	    case D3DFMT_IMC3:
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_U].InitialOffset = uiYPlaneSize;
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_V].InitialOffset = uiYPlaneSize + 
                                                                         uiXPlaneSize;
		    break;

	    case D3DFMT_IMC4:
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_U].InitialOffset = uiYPlaneSize;
		    FormatDesc.D3DChannelDescriptors[CCHANNEL_V].InitialOffset = uiYPlaneSize + 
                                                                         uiWidth / 
                                                                         FormatDesc.D3DChannelDescriptors[CCHANNEL_U].HSubSampling;
		    break;

	    default:
            break;
	} // SurfaceDesc.Format

	return;
}
//-----------------------------------------------------------------------------
// Name: D3DColorToString
// Desc: Returns the string for the given D3DCOLOR.
//-----------------------------------------------------------------------------
const tstring D3DColorToString
(
    const D3DCOLOR  &D3DColor
)
{    
    ENUM_TO_STRING(D3DColor, RGB_TRUE_WHITE);
    ENUM_TO_STRING(D3DColor, RGB_TRUE_RED);
    ENUM_TO_STRING(D3DColor, RGB_TRUE_YELLOW);
    ENUM_TO_STRING(D3DColor, RGB_TRUE_GREEN);
    ENUM_TO_STRING(D3DColor, RGB_TRUE_BLUE);
    ENUM_TO_STRING(D3DColor, RGB_TRUE_BLACK);
    ENUM_TO_STRING(D3DColor, RGB_WHITE);
    ENUM_TO_STRING(D3DColor, RGB_RED);
    ENUM_TO_STRING(D3DColor, RGB_YELLOW);
    ENUM_TO_STRING(D3DColor, RGB_GREEN);
    ENUM_TO_STRING(D3DColor, RGB_CYAN);
    ENUM_TO_STRING(D3DColor, RGB_BLUE);
    ENUM_TO_STRING(D3DColor, RGB_MAGENTA);
    ENUM_TO_STRING(D3DColor, RGB_BLACK);
    ENUM_TO_STRING(D3DColor, RGB_PINK);
    ENUM_TO_STRING(D3DColor, RGB_WHITE_75pc);
    ENUM_TO_STRING(D3DColor, RGB_YELLOW_75pc);
    ENUM_TO_STRING(D3DColor, RGB_CYAN_75pc);
    ENUM_TO_STRING(D3DColor, RGB_GREEN_75pc);
    ENUM_TO_STRING(D3DColor, RGB_MAGENTA_75pc);
    ENUM_TO_STRING(D3DColor, RGB_RED_75pc);
    ENUM_TO_STRING(D3DColor, RGB_BLUE_75pc);
    ENUM_TO_STRING(D3DColor, RGB_BLACK_n4pc);
    ENUM_TO_STRING(D3DColor, RGB_BLACK_p4pc);
    ENUM_TO_STRING(D3DColor, RGB_I);
    ENUM_TO_STRING(D3DColor, RGB_Q);

    return tstring(_T("D3DCOLOR_UNKNOWN"));
}
//-----------------------------------------------------------------------------
// Name: YUVFormatToString
// Desc: Returns the string for the given YUV D3DFORMAT.
//-----------------------------------------------------------------------------
const tstring YUVFormatToString
(
    const D3DFORMAT &YUVFormat
)
{
    static TCHAR  pstr[12];
    TCHAR ch0 = LOBYTE(LOWORD(YUVFormat));
    TCHAR ch1 = HIBYTE(LOWORD(YUVFormat));
    TCHAR ch2 = LOBYTE(HIWORD(YUVFormat));
    TCHAR ch3 = HIBYTE(HIWORD(YUVFormat));

    if (IsCharAlphaNumeric(ch0) && 
        IsCharAlphaNumeric(ch1) &&
        IsCharAlphaNumeric(ch2) &&
        IsCharAlphaNumeric(ch3))
    {
        _stprintf_s(pstr, 12, _T("D3DFMT_%c%c%c%c"), ch0, ch1, ch2, ch3);
    }
    else
    {
        _stprintf_s(pstr, 12, _T("%u"), YUVFormat);
    }

    return tstring(pstr);
}
//-----------------------------------------------------------------------------
// Name: DXVAHDRationalToUINT
// Desc: Returns the UINT value for the given DXVAHD_RATIONAL.
//-----------------------------------------------------------------------------
const UINT DXVAHDRationalToUINT
(
    DXVAHD_RATIONAL dxvaHDRational
)
{    
	UINT uValue = 0;

    // assert(dxvaHDRational.Numerator < dxvaHDRational.Denominator);
    // assert(dxvaHDRational.Denominator == 0);

    uValue = (UINT)(dxvaHDRational.Numerator / dxvaHDRational.Denominator);

	return uValue;
}
//-----------------------------------------------------------------------------
// Name: D3DColorToDXVAHDColorRGBA
// Desc: Returns the DXVAHD_COLOR_RGBA value for the given D3DCOLOR.
//-----------------------------------------------------------------------------
const DXVAHD_COLOR_RGBA D3DColorToDXVAHDColorRGBA
(
    const D3DCOLOR  Color
)
{
    const FLOAT CH0 = FLOAT(LOBYTE(LOWORD(Color))) / 255;
    const FLOAT CH1 = FLOAT(HIBYTE(LOWORD(Color))) / 255;
    const FLOAT CH2 = FLOAT(LOBYTE(HIWORD(Color))) / 255;
    const FLOAT CH3 = FLOAT(HIBYTE(HIWORD(Color))) / 255;

    DXVAHD_COLOR_RGBA HDColor = {CH1, CH2, CH3, CH0};

    return HDColor;
}
//-----------------------------------------------------------------------------
// Name: D3DColorToDXVAHDColorYCbCrA
// Desc: Returns the DXVAHD_COLOR_YCbCrA value for the given D3DCOLOR.
//-----------------------------------------------------------------------------
const DXVAHD_COLOR_YCbCrA D3DColorToDXVAHDColorYCbCrA
(
    const D3DCOLOR  Color
)
{
    const FLOAT CH0 = FLOAT(LOBYTE(LOWORD(Color))) / 255;
    const FLOAT CH1 = FLOAT(HIBYTE(LOWORD(Color))) / 255;
    const FLOAT CH2 = FLOAT(LOBYTE(HIWORD(Color))) / 255;
    const FLOAT CH3 = 1.0;// XRGB -> 1.0 Opaque value alpha channel.

    DXVAHD_COLOR_YCbCrA HDColor = {CH0, CH1, CH2, CH3};

    return HDColor;
}
//-----------------------------------------------------------------------------
// Name: DXVAHDColorToString_RGBA
// Desc: Returns the string representation of the RGBA color.
//-----------------------------------------------------------------------------
const tstring DXVAHDColorToString_RGBA
(
    const DXVAHD_COLOR  Color
)
{
    DXVAHD_COLOR_RGBA HDColor = Color.RGB;    

    return DXVAHDColorToString(
        LOBYTE(LOWORD(HDColor.R * 255)),
        HIBYTE(LOWORD(HDColor.G * 255)),
        LOBYTE(HIWORD(HDColor.B * 255)),
        HIBYTE(HIWORD(HDColor.A * 255))
    );

};
//-----------------------------------------------------------------------------
// Name: DXVAHDColorToString_YCbCr
// Desc: Returns the string representation of the YCbCr color.
//-----------------------------------------------------------------------------
const tstring DXVAHDColorToString_YCbCr
(
    const DXVAHD_COLOR  Color
)
{
    DXVAHD_COLOR_YCbCrA HDColor = Color.YCbCr;    

    return DXVAHDColorToString(
        LOBYTE(LOWORD(HDColor.Y * 255)),
        HIBYTE(LOWORD(HDColor.Cb * 255)),
        LOBYTE(HIWORD(HDColor.Cr * 255)),
        HIBYTE(HIWORD(HDColor.A * 255))
    );

};
//-----------------------------------------------------------------------------
// Name: DXVAHDColorToString
// Desc: Returns the string representation of the BYTEs for a given color.
//-----------------------------------------------------------------------------
const tstring DXVAHDColorToString
(
    const BYTE CH0,
    const BYTE CH1,
    const BYTE CH2,
    const BYTE CH3
)
{
    std::ostringstream  oss;
    
    oss << CH0;
    std::string pCH0 = oss.str();

    oss << CH1;
    std::string pCH1 = oss.str();

    oss << CH2;
    std::string pCH2 = oss.str();

    oss << CH3;
    std::string pCH3 = oss.str();

    tstring szColor(pCH0);
    szColor.append(pCH1);
    szColor.append(pCH2);
    szColor.append(pCH3);

    return szColor;
}
//-----------------------------------------------------------------------------
// Name: ToDXVA2_VideoDesc
// Desc: Returns the DXVA2_VideoDesc value for the given CDXVA_CONTENT_DESC.
//-----------------------------------------------------------------------------
const DXVA2_VideoDesc ToDXVA2_VideoDesc
(
    const CDXVA_CONTENT_DESC ContentDesc
)
{
    DXVA2_VideoDesc VideoDesc = {0};

    VideoDesc.SampleWidth                   = ContentDesc.InputWidth;
    VideoDesc.SampleHeight                  = ContentDesc.InputHeight;
    VideoDesc.SampleFormat                  = ContentDesc.ExtendedFormat;
    VideoDesc.Format                        = ContentDesc.Format;
    VideoDesc.InputSampleFreq.Numerator     = ContentDesc.InputFrameRate.Numerator;
    VideoDesc.InputSampleFreq.Denominator   = ContentDesc.InputFrameRate.Denominator;
    VideoDesc.OutputFrameFreq.Numerator     = ContentDesc.OutputFrameRate.Numerator;
    VideoDesc.OutputFrameFreq.Denominator   = ContentDesc.OutputFrameRate.Denominator;
    VideoDesc.UABProtectionLevel            = 0;
    VideoDesc.Reserved                      = 0;

    return VideoDesc;
}
//-----------------------------------------------------------------------------
// Name: ToDXVAHD_CONTENT_DESC
// Desc: Returns the DXVAHD_CONTENT_DESC value for the given CDXVA_CONTENT_DESC.
//-----------------------------------------------------------------------------
const DXVAHD_CONTENT_DESC ToDXVAHD_CONTENT_DESC
(
    const CDXVA_CONTENT_DESC    ContentDesc
)
{
    DXVAHD_CONTENT_DESC VideoDesc;

    memset(&VideoDesc, 0, sizeof(DXVAHD_CONTENT_DESC));
    switch (ContentDesc.SampleFormat)
    {
        case DXVA2_SampleProgressiveFrame:
        case DXVA2_SampleSubStream:
            VideoDesc.InputFrameFormat = DXVAHD_FRAME_FORMAT_PROGRESSIVE;
            break;
        case DXVA2_SampleFieldInterleavedEvenFirst:
        case DXVA2_SampleFieldSingleEven:
            VideoDesc.InputFrameFormat = DXVAHD_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST;
            break;
        case DXVA2_SampleFieldInterleavedOddFirst:
        case DXVA2_SampleFieldSingleOdd:
            VideoDesc.InputFrameFormat = DXVAHD_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST;
            break;
        default:
            VideoDesc.InputFrameFormat = DXVAHD_FRAME_FORMAT_PROGRESSIVE;
            break;
    }

    VideoDesc.InputFrameRate    = ContentDesc.InputFrameRate;
    VideoDesc.InputWidth        = ContentDesc.InputWidth;
    VideoDesc.InputHeight       = ContentDesc.InputHeight;
    VideoDesc.OutputFrameRate   = ContentDesc.OutputFrameRate;
    VideoDesc.OutputWidth       = ContentDesc.OutputWidth;
    VideoDesc.OutputHeight      = ContentDesc.OutputHeight;

    return VideoDesc;
}

//-----------------------------------------------------------------------------
// Name: ToD3D11VP_CONTENT_DESC
// Desc: Returns the D3D11_VIDEO_PROCESSOR_CONTENT_DESC value for the 
// given CDXVA_CONTENT_DESC.
//-----------------------------------------------------------------------------
const D3D11_VIDEO_PROCESSOR_CONTENT_DESC ToD3D11VP_CONTENT_DESC
(
    const CDXVA_CONTENT_DESC  ContentDesc
)
{
   D3D11_VIDEO_PROCESSOR_CONTENT_DESC VideoDesc;

    memset(&VideoDesc, 0, sizeof(D3D11_VIDEO_PROCESSOR_CONTENT_DESC));
    switch (ContentDesc.SampleFormat)
    {
        case DXVA2_SampleProgressiveFrame:
        case DXVA2_SampleSubStream:
            VideoDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
            break;
        case DXVA2_SampleFieldInterleavedEvenFirst:
        case DXVA2_SampleFieldSingleEven:
            VideoDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST;
            break;
        case DXVA2_SampleFieldInterleavedOddFirst:
        case DXVA2_SampleFieldSingleOdd:
            VideoDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST;
            break;
        default:
            VideoDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
            break;
    }

    VideoDesc.InputFrameRate.Denominator    = ContentDesc.InputFrameRate.Denominator;
    VideoDesc.InputFrameRate.Numerator      = ContentDesc.InputFrameRate.Numerator;
    VideoDesc.InputWidth                    = ContentDesc.InputWidth;
    VideoDesc.InputHeight                   = ContentDesc.InputHeight;
    VideoDesc.OutputFrameRate.Denominator   = ContentDesc.OutputFrameRate.Denominator;
    VideoDesc.OutputFrameRate.Numerator     = ContentDesc.OutputFrameRate.Numerator;
    VideoDesc.OutputWidth                   = ContentDesc.OutputWidth;
    VideoDesc.OutputHeight                  = ContentDesc.OutputHeight;

    return VideoDesc;
};

//-----------------------------------------------------------------------------
// Name: ToCDXVA_CONTENT_DESC
// Desc: Returns the CDXVA_CONTENT_DESC value for the given DXVA2_VideoDesc.
//-----------------------------------------------------------------------------
const CDXVA_CONTENT_DESC ToCDXVA_CONTENT_DESC
(
    const DXVA2_VideoDesc   VideoDesc
)
{
    CDXVA_CONTENT_DESC ContentDesc = {0};

    ContentDesc.InputWidth                  = VideoDesc.SampleWidth;
    ContentDesc.InputHeight                 = VideoDesc.SampleHeight;   
    ContentDesc.InputFrameRate.Numerator    = VideoDesc.InputSampleFreq.Numerator;
    ContentDesc.InputFrameRate.Denominator  = VideoDesc.InputSampleFreq.Denominator;
    ContentDesc.OutputWidth                 = VideoDesc.SampleWidth;
    ContentDesc.OutputHeight                = VideoDesc.SampleHeight;   
    ContentDesc.OutputFrameRate.Numerator   = VideoDesc.OutputFrameFreq.Numerator ;
    ContentDesc.OutputFrameRate.Denominator = VideoDesc.OutputFrameFreq.Denominator;
    ContentDesc.Format                      = VideoDesc.Format;
    ContentDesc.SampleFormat                = (DXVA2_SampleFormat)VideoDesc.SampleFormat.SampleFormat;
    ContentDesc.ExtendedFormat              = VideoDesc.SampleFormat;

    return ContentDesc;
};
//-----------------------------------------------------------------------------
// Name: ToCDXVA_CONTENT_DESC
// Desc: Returns the CDXVA_CONTENT_DESC value for the given DXVAHD_CONTENT_DESC.
//-----------------------------------------------------------------------------
const CDXVA_CONTENT_DESC ToCDXVA_CONTENT_DESC
(
    const DXVAHD_CONTENT_DESC   VideoDesc
)
{
    CDXVA_CONTENT_DESC ContentDesc = {0};

    memset(&ContentDesc, 0, sizeof(CDXVA_CONTENT_DESC));

    switch (VideoDesc.InputFrameFormat)
    {
        case DXVAHD_FRAME_FORMAT_PROGRESSIVE:
            ContentDesc.SampleFormat = DXVA2_SampleProgressiveFrame;
            break;
        case DXVAHD_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST:
            ContentDesc.SampleFormat = DXVA2_SampleFieldInterleavedEvenFirst;
            break;
        case DXVAHD_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST:
            ContentDesc.SampleFormat = DXVA2_SampleFieldInterleavedOddFirst;
            break;
        default:
            ContentDesc.SampleFormat = DXVA2_SampleProgressiveFrame;
            break;
    }

    ContentDesc.InputWidth      = VideoDesc.InputWidth;
    ContentDesc.InputHeight     = VideoDesc.InputHeight;
    ContentDesc.InputFrameRate  = VideoDesc.InputFrameRate;
    ContentDesc.OutputWidth     = VideoDesc.OutputWidth;
    ContentDesc.OutputHeight    = VideoDesc.OutputHeight;
    ContentDesc.OutputFrameRate = VideoDesc.OutputFrameRate;
    ContentDesc.Format          = D3DFMT_UNKNOWN;

    return ContentDesc;
};
//-----------------------------------------------------------------------------
// Name: ToCDXVA_CONTENT_DESC
// Desc: Returns the CDXVA_CONTENT_DESC value for the given 
// D3D11_VIDEO_PROCESSOR_CONTENT_DESC.
//-----------------------------------------------------------------------------
const CDXVA_CONTENT_DESC ToCDXVA_CONTENT_DESC
(
    const D3D11_VIDEO_PROCESSOR_CONTENT_DESC   VideoDesc
)
{
    CDXVA_CONTENT_DESC ContentDesc = {0};

    memset(&ContentDesc, 0, sizeof(CDXVA_CONTENT_DESC));

    switch (VideoDesc.InputFrameFormat)
    {
        case D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE:
            ContentDesc.SampleFormat = DXVA2_SampleProgressiveFrame;
            break;
        case D3D11_VIDEO_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST:
            ContentDesc.SampleFormat = DXVA2_SampleFieldInterleavedEvenFirst;
            break;
        case D3D11_VIDEO_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST:
            ContentDesc.SampleFormat = DXVA2_SampleFieldInterleavedOddFirst;
            break;
        default:
            ContentDesc.SampleFormat = DXVA2_SampleProgressiveFrame;
            break;
    }

    ContentDesc.InputWidth                  = VideoDesc.InputWidth;
    ContentDesc.InputHeight                 = VideoDesc.InputHeight;
    ContentDesc.InputFrameRate.Denominator  = VideoDesc.InputFrameRate.Denominator;
    ContentDesc.InputFrameRate.Numerator    = VideoDesc.InputFrameRate.Numerator;
    ContentDesc.OutputWidth                 = VideoDesc.OutputWidth;
    ContentDesc.OutputHeight                = VideoDesc.OutputHeight;
    ContentDesc.OutputFrameRate.Denominator = VideoDesc.OutputFrameRate.Denominator;
    ContentDesc.OutputFrameRate.Numerator   = VideoDesc.OutputFrameRate.Numerator;
    ContentDesc.Format                      = D3DFMT_UNKNOWN;

    return ContentDesc;
};

//-----------------------------------------------------------------------------
// Name: DXVA2DecoderGuidToString
// Desc: Returns the string value for the given DXVA2 Video Decoder Guid.
//-----------------------------------------------------------------------------
const tstring DXVA2DecoderGuidToString
(
    GUID dxva2DecoderGuid
)
{
    TCHAR*  pstrMode = NULL;

    if (dxva2DecoderGuid == DXVA2_ModeMPEG2_VLD)
    {
        pstrMode = TEXT("DXVA2_ModeMPEG2_VLD");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeMPEG2_MOCOMP)
    {
        pstrMode = TEXT("DXVA2_ModeMPEG2_MOCOMP");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeMPEG2_IDCT)
    {
        pstrMode = TEXT("DXVA2_ModeMPEG2_IDCT");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeWMV9_B)
    {
        pstrMode = TEXT("DXVA2_ModeWMV9_B");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeWMV9_C)
    {
        pstrMode = TEXT("DXVA2_ModeWMV9_C");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeVC1_B)
    {
        pstrMode = TEXT("DXVA2_ModeVC1_B");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeVC1_C)
    {
        pstrMode = TEXT("DXVA2_ModeVC1_C");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeVC1_D)
    {
        pstrMode = TEXT("DXVA2_ModeWMV9_C");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeH264_MoComp_NoFGT)
    {
        pstrMode = TEXT("DXVA2_ModeWMV9_C");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeH264_MoComp_FGT)
    {
        pstrMode = TEXT("DXVA2_ModeH264_MoComp_FGT");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeH264_IDCT_NoFGT)
    {
        pstrMode = TEXT("DXVA2_ModeH264_IDCT_NoFGT");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeH264_IDCT_FGT)
    {
        pstrMode = TEXT("DXVA2_ModeH264_IDCT_FGT");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeH264_VLD_NoFGT)
    {
        pstrMode = TEXT("DXVA2_ModeH264_VLD_NoFGT");
    }
    else if (dxva2DecoderGuid == DXVA2_ModeH264_VLD_FGT)
    {
        pstrMode = TEXT("DXVA2_ModeH264_VLD_FGT");
    }
    else
    {
        return ToString(dxva2DecoderGuid).c_str();
    }
    
    return tstring(pstrMode);
}
//-----------------------------------------------------------------------------
// Name: DXVA2DecoderBufferTypeToString
// Desc: Returns the string for the given UINT representation of a buffer type.
//-----------------------------------------------------------------------------
const tstring DXVA2DecoderBufferTypeToString
(
    UINT dxva2BufferType
)
{
    TCHAR*  pstrBufferType = NULL;

    if (dxva2BufferType == DXVA2_PictureParametersBufferType)
    {
        pstrBufferType = TEXT("DXVA2_PictureParametersBufferType");
    }
    else if (dxva2BufferType == DXVA2_MacroBlockControlBufferType)
    {
        pstrBufferType = TEXT("DXVA2_MacroBlockControlBufferType");
    }
    else if (dxva2BufferType == DXVA2_ResidualDifferenceBufferType)
    {
        pstrBufferType = TEXT("DXVA2_ResidualDifferenceBufferType");
    }
    else if (dxva2BufferType == DXVA2_DeblockingControlBufferType)
    {
        pstrBufferType = TEXT("DXVA2_DeblockingControlBufferType");
    }
    else if (dxva2BufferType == DXVA2_InverseQuantizationMatrixBufferType)
    {
        pstrBufferType = TEXT("DXVA2_InverseQuantizationMatrixBufferType");
    }
    else if (dxva2BufferType == DXVA2_SliceControlBufferType)
    {
        pstrBufferType = TEXT("DXVA2_SliceControlBufferType");
    }
    else if (dxva2BufferType == DXVA2_BitStreamDateBufferType)
    {
        pstrBufferType = TEXT("DXVA2_BitStreamDateBufferType");
    }
    else if (dxva2BufferType == DXVA2_MotionVectorBuffer)
    {
        pstrBufferType = TEXT("DXVA2_MotionVectorBuffer");
    }
    else if (dxva2BufferType == DXVA2_FilmGrainBuffer)
    {
        pstrBufferType = TEXT("DXVA2_FilmGrainBuffer");
    }
    else
    {
        pstrBufferType = TEXT("DXVA2_UnknownBufferType");
    }
    
    return tstring(pstrBufferType);
}
//-----------------------------------------------------------------------------
// Name: DXVAHDDeviceCapsToString
// Desc: Returns the string for the given UINT representation of capabilities.
//-----------------------------------------------------------------------------
const TCHAR* DXVAHDDeviceCapsToString
(
    UINT dxvaHDDeviceCaps
)
{
    static TCHAR pstr[MAX_STRING] = _T("");
    TCHAR* pstrSrc = NULL;

    _stprintf_s(pstr, MAX_STRING, _T(""));
    if (dxvaHDDeviceCaps & DXVAHD_DEVICE_CAPS_LINEAR_SPACE)
    {
        pstrSrc = TEXT("DXVAHD_DEVICE_CAPS_LINEAR_SPACE | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDDeviceCaps & DXVAHD_DEVICE_CAPS_xvYCC)
    {
        pstrSrc = TEXT("DXVAHD_DEVICE_CAPS_xvYCC | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    //
    // Remove the trailing | if pstr was populated
    //
    if (!pstrSrc)
    {
        _stprintf_s(pstr, MAX_STRING, _T("NONE"));
    }     
    else
    {
        TCHAR* pDest = NULL;
        int ch = '|';
        __int64 index = 0;

        pDest = _tcsrchr(pstr, ch);
        if (pDest)
        {
            index = (pDest - pstr);
            pstr[index] = 0;
        }
    }

    return pstr;
}
//-----------------------------------------------------------------------------
// Name: DXVAHDFeatureCapsToString
// Desc: Returns the string for the given UINT representation of capabilities.
//-----------------------------------------------------------------------------
const TCHAR* DXVAHDFeatureCapsToString
(
    UINT dxvaHDFeatureCaps
)
{
    static TCHAR pstr[MAX_STRING] = _T("");
    TCHAR* pstrSrc = NULL;

    _stprintf_s(pstr, MAX_STRING, _T(""));
    if (dxvaHDFeatureCaps & DXVAHD_FEATURE_CAPS_CONSTRICTION)
    {
        pstrSrc = TEXT("DXVAHD_FEATURE_CAPS_CONSTRICTION | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDFeatureCaps & DXVAHD_FEATURE_CAPS_LUMA_KEY)
    {
        pstrSrc = TEXT("DXVAHD_FEATURE_CAPS_LUMA_KEY | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    //
    // Remove the trailing | if pstr was populated
    //
    if (!pstrSrc)
    {
       _stprintf_s(pstr, MAX_STRING, _T("NONE"));
    }     
    else
    {
        TCHAR* pDest = NULL;
        int ch = '|';
        __int64 index = 0;

        pDest = _tcsrchr(pstr, ch);
        if (pDest)
        {
            index = (pDest - pstr);
            pstr[index] = 0;
        }
    }

    return pstr;
}
//-----------------------------------------------------------------------------
// Name: DXVAHDFilterCapsToString
// Desc: Returns the string for the given UINT representation of capabilities.
//-----------------------------------------------------------------------------
const TCHAR* DXVAHDFilterCapsToString
(
    UINT dxvaHDFilterCaps
)
{
    static TCHAR pstr[MAX_STRING] = _T("");
    TCHAR* pstrSrc = NULL;

    _stprintf_s(pstr, MAX_STRING, _T(""));
    if (dxvaHDFilterCaps & DXVAHD_FILTER_CAPS_BRIGHTNESS)
    {
        pstrSrc = TEXT("DXVAHD_FILTER_CAPS_BRIGHTNESS | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDFilterCaps & DXVAHD_FILTER_CAPS_CONTRAST)
    {
        pstrSrc = TEXT("DXVAHD_FILTER_CAPS_CONTRAST | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDFilterCaps & DXVAHD_FILTER_CAPS_HUE)
    {
        pstrSrc = TEXT("DXVAHD_FILTER_CAPS_HUE | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }
    if (dxvaHDFilterCaps & DXVAHD_FILTER_CAPS_SATURATION)
    {
        pstrSrc = TEXT("DXVAHD_FILTER_CAPS_SATURATION | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDFilterCaps & DXVAHD_FILTER_CAPS_NOISE_REDUCTION)
    {
        pstrSrc = TEXT("DXVAHD_FILTER_CAPS_NOISE_REDUCTION | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDFilterCaps & DXVAHD_FILTER_CAPS_EDGE_ENHANCEMENT)
    {
        pstrSrc = TEXT("DXVAHD_FILTER_CAPS_EDGE_ENHANCEMENT | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }
    if (dxvaHDFilterCaps & DXVAHD_FILTER_CAPS_ANAMORPHIC_SCALING)
    {
        pstrSrc = TEXT("DXVAHD_FILTER_CAPS_ANAMORPHIC_SCALING | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    //
    // Remove the trailing | if pstr was populated
    //
    if (!pstrSrc)
    {
        _stprintf_s(pstr, MAX_STRING, _T("NONE"));
    }     
    else
    {
        TCHAR* pDest = NULL;
        int ch = '|';
        __int64 index = 0;

        pDest = _tcsrchr(pstr, ch);
        if (pDest)
        {
            index = (pDest - pstr);
            pstr[index] = 0;
        }
    }

    return pstr;
}
//-----------------------------------------------------------------------------
// Name: DXVAHDInputFormatCapsToString
// Desc: Returns the string for the given UINT representation of capabilities.
//-----------------------------------------------------------------------------
const TCHAR* DXVAHDInputFormatCapsToString
(
    UINT dxvaHDInputFormatCaps
)
{
    static TCHAR pstr[MAX_STRING] = _T("");
    TCHAR* pstrSrc = NULL;

    _stprintf_s(pstr, MAX_STRING, _T(""));
    if (dxvaHDInputFormatCaps & DXVAHD_INPUT_FORMAT_CAPS_RGB_INTERLACED)
    {
        pstrSrc = TEXT("DXVAHD_INPUT_FORMAT_CAPS_RGB_INTERLACED | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDInputFormatCaps & DXVAHD_INPUT_FORMAT_CAPS_RGB_PROCAMP)
    {
        pstrSrc = TEXT("DXVAHD_INPUT_FORMAT_CAPS_RGB_PROCAMP | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDInputFormatCaps & DXVAHD_INPUT_FORMAT_CAPS_RGB_LUMA_KEY)
    {
        pstrSrc = TEXT("DXVAHD_INPUT_FORMAT_CAPS_RGB_LUMA_KEY | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    //
    // Remove the trailing | if pstr was populated
    //
    if (!pstrSrc)
    {
        _stprintf_s(pstr, MAX_STRING, _T("NONE"));
    }     
    else
    {
        TCHAR* pDest = NULL;
        int ch = '|';
        __int64 index = 0;

        pDest = _tcsrchr(pstr, ch);
        if (pDest)
        {
            index = (pDest - pstr);
            pstr[index] = 0;
        }
    }

    return pstr;
}

//-----------------------------------------------------------------------------
// Name: DXVAHDITelecineCapsToString
// Desc: Returns the string for the given UINT representation of capabilities.
//-----------------------------------------------------------------------------
const TCHAR* DXVAHDITelecineCapsToString
(
    UINT dxvaHDITelecineCaps
)
{
    static TCHAR pstr[MAX_STRING] = _T("");
    TCHAR* pstrSrc = NULL;

    _stprintf_s(pstr, MAX_STRING, _T(""));
    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_32)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_32 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_22)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_22 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_2224)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_2224 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }
    
    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_2332)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_2332 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }
    
    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_32322)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_32322 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_55)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_55 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_64)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_64 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_87)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_87 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }
    
    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_222222222223)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_222222222223 | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }
    
    if (dxvaHDITelecineCaps & DXVAHD_ITELECINE_CAPS_OTHER)
    {
        pstrSrc = TEXT("DXVAHD_ITELECINE_CAPS_OTHER | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    //
    // Remove the trailing | if pstr was populated
    //
    if (!pstrSrc)
    {
        _stprintf_s(pstr, MAX_STRING, _T("NONE"));
    }     
    else
    {
        TCHAR* pDest = NULL;
        int ch = '|';
        __int64 index = 0;

        pDest = _tcsrchr(pstr, ch);
        if (pDest)
        {
            index = (pDest - pstr);
            pstr[index] = 0;
        }
    }

    return pstr;
}
//-----------------------------------------------------------------------------
// Name: DXVAHDProcessorCapsToString
// Desc: Returns the string for the given UINT representation of capabilities.
//-----------------------------------------------------------------------------
const TCHAR* DXVAHDProcessorCapsToString
(
    UINT dxvaHDProcessorCaps
)
{
    static TCHAR pstr[MAX_STRING] = _T("");
    TCHAR* pstrSrc = NULL;

    _stprintf_s(pstr, MAX_STRING, _T(""));
    if (dxvaHDProcessorCaps & DXVAHD_PROCESSOR_CAPS_DEINTERLACE_BLEND)
    {
        pstrSrc = TEXT("DXVAHD_PROCESSOR_CAPS_DEINTERLACE_BLEND | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDProcessorCaps & DXVAHD_PROCESSOR_CAPS_DEINTERLACE_BOB)
    {
        pstrSrc = TEXT("DXVAHD_PROCESSOR_CAPS_DEINTERLACE_BOB | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDProcessorCaps & DXVAHD_PROCESSOR_CAPS_DEINTERLACE_ADAPTIVE)
    {
        pstrSrc = TEXT("DXVAHD_PROCESSOR_CAPS_DEINTERLACE_ADAPTIVE | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDProcessorCaps & DXVAHD_PROCESSOR_CAPS_DEINTERLACE_MOTION_COMPENSATION)
    {
        pstrSrc = TEXT("DXVAHD_PROCESSOR_CAPS_DEINTERLACE_MOTION_COMPENSATION | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDProcessorCaps & DXVAHD_PROCESSOR_CAPS_INVERSE_TELECINE)
    {
        pstrSrc = TEXT("DXVAHD_PROCESSOR_CAPS_INVERSE_TELECINE | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    if (dxvaHDProcessorCaps & DXVAHD_PROCESSOR_CAPS_FRAME_RATE_CONVERSION)
    {
        pstrSrc = TEXT("DXVAHD_PROCESSOR_CAPS_FRAME_RATE_CONVERSION | ");
        _tcscat_s(pstr, MAX_STRING, pstrSrc);
    }

    //
    // Remove the trailing | if pstr was populated
    //
    if (!pstrSrc)
    {
        _stprintf_s(pstr, MAX_STRING, _T("NONE"));
    }     
    else
    {
        TCHAR* pDest = NULL;
        int ch = '|';
        __int64 index = 0;

        pDest = _tcsrchr(pstr, ch);
        if (pDest)
        {
            index = (pDest - pstr);
            pstr[index] = 0;
        }
    }

    return pstr;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given GUID.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const   GUID    &Guid
)
{    
    LPOLESTR  pszGUID = new WCHAR[MAX_GUID];
    TCHAR   *pstrGUID = NULL;
    ULONG      cbAnsi = 0;
    ULONG cCharacters = 0;
    DWORD     dwError = 0;

    StringFromGUID2(Guid, pszGUID, MAX_GUID);   

    cCharacters = wcslen(pszGUID)+1;
    
    //
    // Determine number of bytes to be allocated for ANSI string. An
    // ANSI string can have at most 2 bytes per character (for Double
    // Byte Character Strings.)
    //
    cbAnsi = cCharacters * 2;

    //
    // Use of the OLE allocator is not required because the resultant
    // ANSI  string will never be passed to another COM component. You
    // can use your own allocator.
    //
    pstrGUID = (LPSTR)CoTaskMemAlloc(cbAnsi);

    //
    // Convert to ANSI.
    //
    if (0 == WideCharToMultiByte(CP_ACP, 
                                 WC_NO_BEST_FIT_CHARS, 
                                 pszGUID, 
                                 cCharacters, 
                                 pstrGUID,
                                 cbAnsi, 
                                 NULL, 
                                 NULL))
    {
        CoTaskMemFree(pstrGUID);
        pstrGUID = NULL;
    }

    //
    // Free up the GUID as it has been copied into the multi byte string.
    //
    SAFE_DELETE_ARRAY(pszGUID);

    return tstring(pstrGUID);
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3DMULTISAMPLE_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
      D3DMULTISAMPLE_TYPE     *pD3DMultiSampleType,
      const tstring           &D3DMultiSampleString
)
{
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_NONE, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_NONMASKABLE, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_2_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_3_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_4_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_5_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_6_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_7_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_8_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_9_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_10_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_11_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_12_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_13_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_14_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_15_SAMPLES, pD3DMultiSampleType);
    STRING_TO_ENUM(D3DMultiSampleString, D3DMULTISAMPLE_16_SAMPLES, pD3DMultiSampleType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given D3DMULTISAMPLE_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3DMULTISAMPLE_TYPE &D3DMultiSampleType
)
{
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_NONE);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_NONMASKABLE);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_2_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_3_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_4_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_5_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_6_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_7_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_8_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_9_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_10_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_11_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_12_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_13_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_14_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_15_SAMPLES);
    ENUM_TO_STRING(D3DMultiSampleType, D3DMULTISAMPLE_16_SAMPLES);

    return tstring(_T("D3DMULTISAMPLE_UNKNOWN"));
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3DPOOL.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
      D3DPOOL       *pD3DPoolType,
      const tstring &D3DPoolString
)
{
    STRING_TO_ENUM(D3DPoolString, D3DPOOL_DEFAULT, pD3DPoolType);
    STRING_TO_ENUM(D3DPoolString, D3DPOOL_MANAGED, pD3DPoolType);
    STRING_TO_ENUM(D3DPoolString, D3DPOOL_SYSTEMMEM, pD3DPoolType);
    STRING_TO_ENUM(D3DPoolString, D3DPOOL_SCRATCH, pD3DPoolType);
    STRING_TO_ENUM(D3DPoolString, D3DPOOL_FORCE_DWORD, pD3DPoolType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given D3DPOOL.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const   D3DPOOL &D3DPool
)
{    
    ENUM_TO_STRING(D3DPool, D3DPOOL_DEFAULT);
    ENUM_TO_STRING(D3DPool, D3DPOOL_MANAGED);
    ENUM_TO_STRING(D3DPool, D3DPOOL_SYSTEMMEM);
    ENUM_TO_STRING(D3DPool, D3DPOOL_SCRATCH);
    ENUM_TO_STRING(D3DPool, D3DPOOL_FORCE_DWORD);
    
    return tstring(_T("D3DPOOL_UNKNONW"));
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3DFORMAT.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3DFORMAT     *pD3DFormatType,
    const tstring &D3DFormatTypeString
)
{
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_R8G8B8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A8R8G8B8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_X8R8G8B8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_R5G6B5, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_X1R5G5B5, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A1R5G5B5, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A4R4G4B4, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_R3G3B2, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A8R3G3B2, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_X4R4G4B4, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A2B10G10R10, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A8B8G8R8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_X8B8G8R8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_G16R16, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A2R10G10B10, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A16B16G16R16, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A8P8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_P8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_L8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A8L8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A4L4, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_V8U8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_L6V5U5, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_X8L8V8U8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_Q8W8V8U8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_V16U16, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_A2W10V10U10, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_R8G8_B8G8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_G8R8_G8B8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_DXT1, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_DXT2, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_DXT3, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_DXT4, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_DXT5, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D16_LOCKABLE, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D32, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D15S1, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D24S8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D24X8, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D24X4S4, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D16, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D32F_LOCKABLE, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_D24FS8, pD3DFormatType);
    
    //
    // YUV Formats
    //
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_YUY2, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_YV12, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_NV11, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_NV12, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_NV21, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_AYUV, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_IMC1, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_IMC2, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_IMC3, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_IMC4, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_AI44, pD3DFormatType);
    STRING_TO_ENUM(D3DFormatTypeString, D3DFMT_IA44, pD3DFormatType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3DFORMAT.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3DFORMAT &D3DFormatType
)
{
    if (IsRGBFormat(D3DFormatType))
    {
        ENUM_TO_STRING(D3DFormatType, D3DFMT_R8G8B8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A8R8G8B8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_X8R8G8B8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_R5G6B5);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_X1R5G5B5);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A1R5G5B5);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A4R4G4B4);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_R3G3B2);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A8R3G3B2);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_X4R4G4B4);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A2B10G10R10);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A8B8G8R8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_X8B8G8R8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_G16R16);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A2R10G10B10);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A16B16G16R16);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A8P8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_P8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_L8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A8L8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A4L4);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_V8U8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_L6V5U5);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_X8L8V8U8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_Q8W8V8U8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_V16U16);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_A2W10V10U10);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_R8G8_B8G8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_G8R8_G8B8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_DXT1);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_DXT2);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_DXT3);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_DXT4);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_DXT5);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D16_LOCKABLE);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D32);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D15S1);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D24S8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D24X8);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D24X4S4);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D16);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D32F_LOCKABLE);
        ENUM_TO_STRING(D3DFormatType, D3DFMT_D24FS8);
    }
    else
    {
        return tstring(YUVFormatToString(D3DFormatType));
    }

    return tstring(_T("D3DFMT_NOTSUPPORTED"));
};

//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3DSWAPEFFECT.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3DSWAPEFFECT *pSwapEffectType,
    const tstring &SwapEffectTypeString
)
{
    STRING_TO_ENUM(SwapEffectTypeString, 
                   D3DSWAPEFFECT_DISCARD, 
                   pSwapEffectType);
    STRING_TO_ENUM(SwapEffectTypeString, 
                   D3DSWAPEFFECT_FLIP, 
                   pSwapEffectType);
    STRING_TO_ENUM(SwapEffectTypeString, 
                   D3DSWAPEFFECT_COPY, 
                   pSwapEffectType);
    STRING_TO_ENUM(SwapEffectTypeString, 
                   D3DSWAPEFFECT_OVERLAY, 
                   pSwapEffectType);
    STRING_TO_ENUM(SwapEffectTypeString, 
                   D3DSWAPEFFECT_FORCE_DWORD, 
                   pSwapEffectType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3DSWAPEFFECT.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3DSWAPEFFECT &SwapEffectType
)
{
      ENUM_TO_STRING(SwapEffectType, D3DSWAPEFFECT_DISCARD);
      ENUM_TO_STRING(SwapEffectType, D3DSWAPEFFECT_FLIP);
      ENUM_TO_STRING(SwapEffectType, D3DSWAPEFFECT_COPY);
      ENUM_TO_STRING(SwapEffectType, D3DSWAPEFFECT_OVERLAY);
      ENUM_TO_STRING(SwapEffectType, D3DSWAPEFFECT_FORCE_DWORD);

      return tstring(_T("D3DSWAPEFFECT_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_DEVICE_USAGE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_DEVICE_USAGE   *pDeviceUsage, 
    const tstring         &DeviceUsageString
)
{
    STRING_TO_ENUM(DeviceUsageString, 
                   DXVAHD_DEVICE_USAGE_PLAYBACK_NORMAL, 
                   pDeviceUsage);
    STRING_TO_ENUM(DeviceUsageString, 
                   DXVAHD_DEVICE_USAGE_OPTIMAL_SPEED, 
                   pDeviceUsage);
    STRING_TO_ENUM(DeviceUsageString, 
                   DXVAHD_DEVICE_USAGE_OPTIMAL_QUALITY, 
                   pDeviceUsage);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_DEVICE_USAGE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_DEVICE_USAGE &DeviceUsageType
)
{
      ENUM_TO_STRING(DeviceUsageType, DXVAHD_DEVICE_USAGE_PLAYBACK_NORMAL);
      ENUM_TO_STRING(DeviceUsageType, DXVAHD_DEVICE_USAGE_OPTIMAL_SPEED);
      ENUM_TO_STRING(DeviceUsageType, DXVAHD_DEVICE_USAGE_OPTIMAL_QUALITY);

      return tstring(_T("DXVAHD_DEVICE_USAGE_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3D11_VIDEO_USAGE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3D11_VIDEO_USAGE   *pUsage, 
    const tstring       &UsageString
)
{
    STRING_TO_ENUM(UsageString, 
                   D3D11_VIDEO_USAGE_PLAYBACK_NORMAL, 
                   pUsage);
    STRING_TO_ENUM(UsageString, 
                   D3D11_VIDEO_USAGE_OPTIMAL_SPEED, 
                   pUsage);
    STRING_TO_ENUM(UsageString, 
                   D3D11_VIDEO_USAGE_OPTIMAL_QUALITY, 
                   pUsage);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3D11_VIDEO_USAGE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3D11_VIDEO_USAGE &Usage
)
{
      ENUM_TO_STRING(Usage, D3D11_VIDEO_USAGE_PLAYBACK_NORMAL);
      ENUM_TO_STRING(Usage, D3D11_VIDEO_USAGE_OPTIMAL_SPEED);
      ENUM_TO_STRING(Usage, D3D11_VIDEO_USAGE_OPTIMAL_QUALITY);

      return tstring(_T("D3D11_VIDEO_USAGE_PLAYBACK_NORMAL"));
};
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given DXVA2_Frequency.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const   DXVA2_Frequency &Frequency
)
{    
	DXVAHD_RATIONAL Rational = *(DXVAHD_RATIONAL*)&Frequency;

	return ToString(Rational);
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given DXVAHD_RATIONAL.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const   DXVAHD_RATIONAL &DXVAHDRational
)
{    
	static TCHAR  pstr[MAX_STRING];

	_stprintf_s(pstr, 
                MAX_STRING, 
                TEXT("%d/%d"), 
                DXVAHDRational.Numerator, 
                DXVAHDRational.Denominator);

	return tstring(pstr);
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_FRAME_FORMAT.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_FRAME_FORMAT   *pDXVAHDFrameFormat, 
    const tstring         &DXVAHDFrameFormatString
)
{
    STRING_TO_ENUM(DXVAHDFrameFormatString, 
                   DXVAHD_FRAME_FORMAT_PROGRESSIVE, 
                   pDXVAHDFrameFormat);
    STRING_TO_ENUM(DXVAHDFrameFormatString, 
                   DXVAHD_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST, 
                   pDXVAHDFrameFormat);
    STRING_TO_ENUM(DXVAHDFrameFormatString, 
                   DXVAHD_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST, 
                   pDXVAHDFrameFormat);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_FRAME_FORMAT.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_FRAME_FORMAT &DXVAHDFrameFormatType
)
{
      ENUM_TO_STRING(DXVAHDFrameFormatType, 
                     DXVAHD_FRAME_FORMAT_PROGRESSIVE);
      ENUM_TO_STRING(DXVAHDFrameFormatType, 
                     DXVAHD_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST);
      ENUM_TO_STRING(DXVAHDFrameFormatType, 
                     DXVAHD_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST);

      return tstring(_T("DXVAHD_FRAME_FORMAT_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3D11_VIDEO_FRAME_FORMAT.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3D11_VIDEO_FRAME_FORMAT   *pFrameFormat, 
    const tstring              &FrameFormatString
)
{
    STRING_TO_ENUM(FrameFormatString, 
                   D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE, 
                   pFrameFormat);
    STRING_TO_ENUM(FrameFormatString, 
                   D3D11_VIDEO_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST, 
                   pFrameFormat);
    STRING_TO_ENUM(FrameFormatString, 
                   D3D11_VIDEO_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST, 
                   pFrameFormat);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3D11_VIDEO_FRAME_FORMAT.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3D11_VIDEO_FRAME_FORMAT &FrameFormatType
)
{
      ENUM_TO_STRING(FrameFormatType, 
                     D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE);
      ENUM_TO_STRING(FrameFormatType, 
                     D3D11_VIDEO_FRAME_FORMAT_INTERLACED_TOP_FIELD_FIRST);
      ENUM_TO_STRING(FrameFormatType, 
                     D3D11_VIDEO_FRAME_FORMAT_INTERLACED_BOTTOM_FIELD_FIRST);

      return tstring(_T("DXVAHD_FRAME_FORMAT_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_DEVICE_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_DEVICE_TYPE    *pDXVAHDDeviceType, 
    const tstring         &DXVAHDDeviceTypeString
)
{
    STRING_TO_ENUM(DXVAHDDeviceTypeString, 
                   DXVAHD_DEVICE_TYPE_HARDWARE, 
                   pDXVAHDDeviceType);
    STRING_TO_ENUM(DXVAHDDeviceTypeString, 
                   DXVAHD_DEVICE_TYPE_SOFTWARE, 
                   pDXVAHDDeviceType);
    STRING_TO_ENUM(DXVAHDDeviceTypeString, 
                   DXVAHD_DEVICE_TYPE_REFERENCE, 
                   pDXVAHDDeviceType);
    STRING_TO_ENUM(DXVAHDDeviceTypeString, 
                   DXVAHD_DEVICE_TYPE_OTHER, 
                   pDXVAHDDeviceType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given DXVAHD_DEVICE_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_DEVICE_TYPE &DXVAHDDeviceType
)
{
      ENUM_TO_STRING(DXVAHDDeviceType, 
                     DXVAHD_DEVICE_TYPE_HARDWARE);
      ENUM_TO_STRING(DXVAHDDeviceType, 
                     DXVAHD_DEVICE_TYPE_SOFTWARE);
      ENUM_TO_STRING(DXVAHDDeviceType, 
                     DXVAHD_DEVICE_TYPE_REFERENCE);
      ENUM_TO_STRING(DXVAHDDeviceType, 
                     DXVAHD_DEVICE_TYPE_OTHER);

      return tstring(_T("DXVAHD_DEVICE_TYPE_UNKNOWN"));
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_SURFACE_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_SURFACE_TYPE   *pDXVAHDSurfaceType, 
    const tstring         &DXVAHDSurfaceTypeString
)
{
    STRING_TO_ENUM(DXVAHDSurfaceTypeString, 
                   DXVAHD_SURFACE_TYPE_VIDEO_INPUT, 
                   pDXVAHDSurfaceType);
    STRING_TO_ENUM(DXVAHDSurfaceTypeString, 
                   DXVAHD_SURFACE_TYPE_VIDEO_INPUT_PRIVATE, 
                   pDXVAHDSurfaceType);
    STRING_TO_ENUM(DXVAHDSurfaceTypeString, 
                   DXVAHD_SURFACE_TYPE_VIDEO_OUTPUT, 
                   pDXVAHDSurfaceType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_SURFACE_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_SURFACE_TYPE &DXVAHDSurfaceType
)
{
      ENUM_TO_STRING(DXVAHDSurfaceType, 
                     DXVAHD_SURFACE_TYPE_VIDEO_INPUT);
      ENUM_TO_STRING(DXVAHDSurfaceType, 
                     DXVAHD_SURFACE_TYPE_VIDEO_INPUT_PRIVATE);
      ENUM_TO_STRING(DXVAHDSurfaceType, 
                     DXVAHD_SURFACE_TYPE_VIDEO_OUTPUT);

      return tstring(_T("DXVAHD_SURFACE_TYPE_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_STREAM_STATE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_STREAM_STATE   *pDXVAHDStreamStateType, 
    const tstring         &DXVAHDStreamStateTypeString
)
{
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_D3DFORMAT, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_FRAME_FORMAT, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_OUTPUT_RATE, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_SOURCE_RECT, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_DESTINATION_RECT, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_ALPHA, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_PALETTE, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_LUMA_KEY, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_ASPECT_RATIO, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_FILTER_BRIGHTNESS, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_FILTER_CONTRAST, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_FILTER_HUE, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_FILTER_SATURATION, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_FILTER_NOISE_REDUCTION, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_FILTER_EDGE_ENHANCEMENT, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_FILTER_ANAMORPHIC_SCALING, 
                   pDXVAHDStreamStateType);
    STRING_TO_ENUM(DXVAHDStreamStateTypeString, 
                   DXVAHD_STREAM_STATE_PRIVATE, 
                   pDXVAHDStreamStateType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_STREAM_STATE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_STREAM_STATE &DXVAHDStreamState
)
{
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_D3DFORMAT);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_FRAME_FORMAT);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_OUTPUT_RATE);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_SOURCE_RECT);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_DESTINATION_RECT);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_ALPHA);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_PALETTE);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_LUMA_KEY);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_ASPECT_RATIO);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_FILTER_BRIGHTNESS);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_FILTER_CONTRAST);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_FILTER_HUE);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_FILTER_SATURATION);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_FILTER_NOISE_REDUCTION);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_FILTER_EDGE_ENHANCEMENT);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_FILTER_ANAMORPHIC_SCALING);
      ENUM_TO_STRING(DXVAHDStreamState, 
                     DXVAHD_STREAM_STATE_PRIVATE);

      return tstring(_T("DXVAHD_STREAM_STATE_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_FILTER.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_FILTER *pDXVAHDFilter, 
    const tstring &DXVAHDFilterString
)
{
    STRING_TO_ENUM(DXVAHDFilterString, 
                   DXVAHD_FILTER_BRIGHTNESS, 
                   pDXVAHDFilter);
    STRING_TO_ENUM(DXVAHDFilterString, 
                   DXVAHD_FILTER_CONTRAST, 
                   pDXVAHDFilter);
    STRING_TO_ENUM(DXVAHDFilterString, 
                   DXVAHD_FILTER_HUE, 
                   pDXVAHDFilter);
    STRING_TO_ENUM(DXVAHDFilterString, 
                   DXVAHD_FILTER_SATURATION, 
                   pDXVAHDFilter);
    STRING_TO_ENUM(DXVAHDFilterString, 
                   DXVAHD_FILTER_NOISE_REDUCTION, 
                   pDXVAHDFilter);
    STRING_TO_ENUM(DXVAHDFilterString, 
                   DXVAHD_FILTER_EDGE_ENHANCEMENT, 
                   pDXVAHDFilter);
    STRING_TO_ENUM(DXVAHDFilterString, 
                   DXVAHD_FILTER_ANAMORPHIC_SCALING, 
                   pDXVAHDFilter);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_FILTER.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_FILTER &DXVAHDFilterType
)
{
    ENUM_TO_STRING(DXVAHDFilterType, DXVAHD_FILTER_BRIGHTNESS);
    ENUM_TO_STRING(DXVAHDFilterType, DXVAHD_FILTER_CONTRAST);
    ENUM_TO_STRING(DXVAHDFilterType, DXVAHD_FILTER_HUE);
    ENUM_TO_STRING(DXVAHDFilterType, DXVAHD_FILTER_SATURATION);
    ENUM_TO_STRING(DXVAHDFilterType, DXVAHD_FILTER_NOISE_REDUCTION);
    ENUM_TO_STRING(DXVAHDFilterType, DXVAHD_FILTER_EDGE_ENHANCEMENT);
    ENUM_TO_STRING(DXVAHDFilterType, DXVAHD_FILTER_ANAMORPHIC_SCALING);

    return tstring(_T("DXVAHD_FILTER_UNKNOWN"));
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3D11_VIDEO_PROCESSOR_FILTER.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3D11_VIDEO_PROCESSOR_FILTER    *pFilter, 
    const tstring                   &FilterString
)
{
    STRING_TO_ENUM(FilterString, 
                   D3D11_VIDEO_PROCESSOR_FILTER_BRIGHTNESS, 
                   pFilter);
    STRING_TO_ENUM(FilterString, 
                   D3D11_VIDEO_PROCESSOR_FILTER_CONTRAST, 
                   pFilter);
    STRING_TO_ENUM(FilterString, 
                   D3D11_VIDEO_PROCESSOR_FILTER_HUE, 
                   pFilter);
    STRING_TO_ENUM(FilterString, 
                   D3D11_VIDEO_PROCESSOR_FILTER_SATURATION, 
                   pFilter);
    STRING_TO_ENUM(FilterString, 
                   D3D11_VIDEO_PROCESSOR_FILTER_NOISE_REDUCTION, 
                   pFilter);
    STRING_TO_ENUM(FilterString, 
                   D3D11_VIDEO_PROCESSOR_FILTER_EDGE_ENHANCEMENT, 
                   pFilter);
    STRING_TO_ENUM(FilterString, 
                   D3D11_VIDEO_PROCESSOR_FILTER_ANAMORPHIC_SCALING, 
                   pFilter);
    STRING_TO_ENUM(FilterString, 
                   D3D11_VIDEO_PROCESSOR_FILTER_STEREO_ADJUSTMENT, 
                   pFilter);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3D11_VIDEO_PROCESSOR_FILTER.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3D11_VIDEO_PROCESSOR_FILTER &FilterType
)
{
    ENUM_TO_STRING(FilterType, D3D11_VIDEO_PROCESSOR_FILTER_BRIGHTNESS);
    ENUM_TO_STRING(FilterType, D3D11_VIDEO_PROCESSOR_FILTER_CONTRAST);
    ENUM_TO_STRING(FilterType, D3D11_VIDEO_PROCESSOR_FILTER_HUE);
    ENUM_TO_STRING(FilterType, D3D11_VIDEO_PROCESSOR_FILTER_SATURATION	);
    ENUM_TO_STRING(FilterType, D3D11_VIDEO_PROCESSOR_FILTER_NOISE_REDUCTION);
    ENUM_TO_STRING(FilterType, D3D11_VIDEO_PROCESSOR_FILTER_EDGE_ENHANCEMENT);
    ENUM_TO_STRING(FilterType, D3D11_VIDEO_PROCESSOR_FILTER_ANAMORPHIC_SCALING);
    ENUM_TO_STRING(FilterType, D3D11_VIDEO_PROCESSOR_FILTER_STEREO_ADJUSTMENT);

    return tstring(_T("DXVAHD_FILTER_UNKNOWN"));
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given DXVAHD_FILTER_RANGE_DATA.
//-----------------------------------------------------------------------------
tstring _stdcall ToString
(
    const DXVAHD_FILTER_RANGE_DATA  &DXVAHDFilterRangeData
)
{
    int                                  decimal = 0;
    int                                     sign = 0;
    int                                      err = 0;
	TCHAR                            Minimum[64];
    TCHAR                            Maximum[64];
    TCHAR                            Default[64];
    TCHAR                           *pMultiplier = NULL;
    static TCHAR pstrFilterRangeData[MAX_STRING] = _T("");

    _itoa_s(DXVAHDFilterRangeData.Minimum, Minimum, 64, 0);
    _itoa_s(DXVAHDFilterRangeData.Maximum, Maximum, 64, 0);
    _itoa_s(DXVAHDFilterRangeData.Default, Default, 64, 0);
    
    //
    // Float is not as straight forward first allocate for the
    // CVT buffer then convert the float to string with no decimal
    // shiffting.
    //
    pMultiplier = (TCHAR*) malloc(_CVTBUFSIZE);
    _ecvt_s(pMultiplier, 
            _CVTBUFSIZE, 
            DXVAHDFilterRangeData.Multiplier, 
            0, 
            &decimal, 
            &sign);

    //
    // Write the data into the FilterRangeData buffer.
    //
    _stprintf_s
    (
        pstrFilterRangeData, 
        MAX_STRING, 
        "Minimum: %s, Maximum: %s, Default: %s, Multiplier: %s", 
        Minimum, 
        Maximum, 
        Default, 
        pMultiplier
    );

    return tstring(_T(pstrFilterRangeData));
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_ALPHA_FILL_MODE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_ALPHA_FILL_MODE  *pDXVAHDAlphaFillMode, 
    const tstring           &DXVAHDAlphaFillModeString
)
{
    STRING_TO_ENUM(DXVAHDAlphaFillModeString, 
                   DXVAHD_ALPHA_FILL_MODE_BACKGROUND, 
                   pDXVAHDAlphaFillMode);
    STRING_TO_ENUM(DXVAHDAlphaFillModeString, 
                   DXVAHD_ALPHA_FILL_MODE_DESTINATION, 
                   pDXVAHDAlphaFillMode);
    STRING_TO_ENUM(DXVAHDAlphaFillModeString, 
                   DXVAHD_ALPHA_FILL_MODE_SOURCE_STREAM, 
                   pDXVAHDAlphaFillMode);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_ALPHA_FILL_MODE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_ALPHA_FILL_MODE &DXVAHDAlphaFillModeType
)
{
    ENUM_TO_STRING(DXVAHDAlphaFillModeType, 
                   DXVAHD_ALPHA_FILL_MODE_BACKGROUND);
    ENUM_TO_STRING(DXVAHDAlphaFillModeType, 
                   DXVAHD_ALPHA_FILL_MODE_DESTINATION);
    ENUM_TO_STRING(DXVAHDAlphaFillModeType, 
                   DXVAHD_ALPHA_FILL_MODE_SOURCE_STREAM);

    return tstring(_T("DXVAHD_ALPHA_FILL_MODE_UNKNOWN"));
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_COLOR_RGBA.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_COLOR_RGBA   *pDXVAHDColorRGBA, 
    const tstring       &DXVAHDColorRGBAString
)
{
    if (DXVAHDColorRGBAString.compare(_T("RGB_HD_WHITE")))
    {
        (*pDXVAHDColorRGBA) = D3DCOLOR_TO_DXVAHD_COLOR_RGBA(RGB_TRUE_WHITE);
        return true;
    }
    if (DXVAHDColorRGBAString.compare(_T("RGB_HD_BLACK")))
    {
        (*pDXVAHDColorRGBA) = D3DCOLOR_TO_DXVAHD_COLOR_RGBA(RGB_TRUE_BLACK);
        return true;
    }
    if (DXVAHDColorRGBAString.compare(_T("RGB_HD_YELLOW")))
    {
        (*pDXVAHDColorRGBA) = D3DCOLOR_TO_DXVAHD_COLOR_RGBA(RGB_TRUE_YELLOW);
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_COLOR_RGBA.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_COLOR_RGBA &DXVAHDColorRGBAType
)
{
    if (DXVAHDColorRGBAType == D3DCOLOR_TO_DXVAHD_COLOR_RGBA(RGB_TRUE_WHITE)) 
        return tstring(_T("RGB_HD_WHITE"));
    if (DXVAHDColorRGBAType == D3DCOLOR_TO_DXVAHD_COLOR_RGBA(RGB_TRUE_BLACK)) 
        return tstring(_T("RGB_HD_BLACK"));
    if (DXVAHDColorRGBAType == D3DCOLOR_TO_DXVAHD_COLOR_RGBA(RGB_TRUE_YELLOW)) 
        return tstring(_T("RGB_HD_YELLOW"));
    
    return tstring(_T("RGB_HD_UNKNOWN")); 
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_COLOR_YCbCrA.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_COLOR_YCbCrA *pDXVAHDColorYCbCrA, 
    const tstring       &DXVAHDColorYCbCrAString
)
{
    if (DXVAHDColorYCbCrAString.compare(_T("YCbCr_HD_WHITE")))
    {
        (*pDXVAHDColorYCbCrA) = D3DCOLOR_TO_DXVAHD_COLOR_YCbCrA(YUV_WHITE);
        return true;
    }
    if (DXVAHDColorYCbCrAString.compare(_T("YCbCr_HD_BLACK")))
    {
        (*pDXVAHDColorYCbCrA) = D3DCOLOR_TO_DXVAHD_COLOR_YCbCrA(YUV_BLACK);
        return true;
    }
    if (DXVAHDColorYCbCrAString.compare(_T("YCbCr_HD_YELLOW")))
    {
        (*pDXVAHDColorYCbCrA) = D3DCOLOR_TO_DXVAHD_COLOR_YCbCrA(YUV_YELLOW);
        return true;
    }    

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_COLOR_RGBA.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_COLOR_YCbCrA &DXVAHDColorYCbCrAType
)
{
    if (DXVAHDColorYCbCrAType == D3DCOLOR_TO_DXVAHD_COLOR_YCbCrA(YUV_WHITE)) 
        return tstring(_T("YCbCr_HD_WHITE"));
    if (DXVAHDColorYCbCrAType == D3DCOLOR_TO_DXVAHD_COLOR_YCbCrA(YUV_BLACK)) 
        return tstring(_T("YCbCr_HD_BLACK"));
    if (DXVAHDColorYCbCrAType == D3DCOLOR_TO_DXVAHD_COLOR_YCbCrA(YUV_YELLOW)) 
        return tstring(_T("YCbCr_HD_YELLOW"));    
    
    return tstring(_T("YCbCr_HD_UNKNOWN"));    
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Converts the DXVAHD Color to a byte rep string.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_COLOR  &DXVAHDColor,
    BOOL                bYCbCr
)
{
    if (bYCbCr) {
        return DXVAHDColorToString_YCbCr(DXVAHDColor);
    } else {
        return DXVAHDColorToString_RGBA(DXVAHDColor);
    };
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXVAHD_OUTPUT_RATE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXVAHD_OUTPUT_RATE  *pDXVAHDOutputRate, 
    const tstring       &DXVAHDOutputRateString
)
{
    STRING_TO_ENUM(DXVAHDOutputRateString, 
                   DXVAHD_OUTPUT_RATE_NORMAL, 
                   pDXVAHDOutputRate);
    STRING_TO_ENUM(DXVAHDOutputRateString, 
                   DXVAHD_OUTPUT_RATE_HALF, 
                   pDXVAHDOutputRate);
    STRING_TO_ENUM(DXVAHDOutputRateString, 
                   DXVAHD_OUTPUT_RATE_CUSTOM, 
                   pDXVAHDOutputRate);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXVAHD_OUTPUT_RATE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXVAHD_OUTPUT_RATE &DXVAHDOutputRateType
)
{
    ENUM_TO_STRING(DXVAHDOutputRateType, DXVAHD_OUTPUT_RATE_NORMAL);
    ENUM_TO_STRING(DXVAHDOutputRateType, DXVAHD_OUTPUT_RATE_HALF);
    ENUM_TO_STRING(DXVAHDOutputRateType, DXVAHD_OUTPUT_RATE_CUSTOM);

    return tstring(_T("DXVAHD_OUTPUT_RATE_UNKNOWN"));  
}

//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3D11_VIDEO_PROCESSOR_OUTPUT_RATE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3D11_VIDEO_PROCESSOR_OUTPUT_RATE   *pOutputRate, 
    const tstring                       &OutputRateString
)
{
    STRING_TO_ENUM(OutputRateString, 
                   D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_NORMAL, 
                   pOutputRate);
    STRING_TO_ENUM(OutputRateString, 
                   D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_HALF, 
                   pOutputRate);
    STRING_TO_ENUM(OutputRateString, 
                   D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_CUSTOM, 
                   pOutputRate);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3D11_VIDEO_PROCESSOR_OUTPUT_RATE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3D11_VIDEO_PROCESSOR_OUTPUT_RATE &OutputRate
)
{
    ENUM_TO_STRING(OutputRate, D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_NORMAL);
    ENUM_TO_STRING(OutputRate, D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_HALF);
    ENUM_TO_STRING(OutputRate, D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_CUSTOM);

    return tstring(_T("D3D11_VIDEO_PROCESSOR_OUTPUT_RATE_UNKNOWN"));  
}
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a CDXVAHD_SURFACE_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    CDXVAHD_SURFACE_TYPE *pCDXVAHDSurfaceType, 
    const tstring &CDXVAHDSurfaceTypeString
)
{
    STRING_TO_ENUM(CDXVAHDSurfaceTypeString,
                   CDXVAHD_SURFACE_TYPE_OUTPUT, 
                   pCDXVAHDSurfaceType);
    STRING_TO_ENUM(CDXVAHDSurfaceTypeString, 
                   CDXVAHD_SURFACE_TYPE_INPUT, 
                   pCDXVAHDSurfaceType);
    STRING_TO_ENUM(CDXVAHDSurfaceTypeString, 
                   CDXVAHD_SURFACE_TYPE_VIDEO_OUTPUT, 
                   pCDXVAHDSurfaceType);
    STRING_TO_ENUM(CDXVAHDSurfaceTypeString, 
                   CDXVAHD_SURFACE_TYPE_VIDEO_INPUT, 
                   pCDXVAHDSurfaceType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a CDXVAHD_SURFACE_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const CDXVAHD_SURFACE_TYPE &CDXVAHDSurfaceType
)
{
      ENUM_TO_STRING(CDXVAHDSurfaceType, CDXVAHD_SURFACE_TYPE_OUTPUT);
      ENUM_TO_STRING(CDXVAHDSurfaceType, CDXVAHD_SURFACE_TYPE_INPUT);
      ENUM_TO_STRING(CDXVAHDSurfaceType, CDXVAHD_SURFACE_TYPE_VIDEO_OUTPUT);
      ENUM_TO_STRING(CDXVAHDSurfaceType, CDXVAHD_SURFACE_TYPE_VIDEO_INPUT);

      return tstring(_T("CDXVAHD_SURFACE_TYPE_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a CSCALE_TYPE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    CSCALE_TYPE   *pScaleType, 
    const tstring &ScaleTypeString
)
{
    STRING_TO_ENUM(ScaleTypeString, CSCALE_TYPE_NONE, pScaleType);
    STRING_TO_ENUM(ScaleTypeString, CSCALE_TYPE_STRETCH, pScaleType);
    STRING_TO_ENUM(ScaleTypeString, CSCALE_TYPE_SHRINK, pScaleType);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a CSCALE_TYPE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const CSCALE_TYPE &ScaleType
)
{
      ENUM_TO_STRING(ScaleType, CSCALE_TYPE_NONE);
      ENUM_TO_STRING(ScaleType, CSCALE_TYPE_STRETCH);
      ENUM_TO_STRING(ScaleType, CSCALE_TYPE_SHRINK);

      return tstring(_T("CSCALE_UNKNOWN"));
};
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given CHANNEL.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    UINT uiChannelIndex, 
    BOOL RGBChannelLayout
)
{
    if (RGBChannelLayout)
    {
        switch(uiChannelIndex)
        {
            case CCHANNEL_R:
                return tstring(_T("R"));
            case CCHANNEL_G:
                return tstring(_T("G"));
            case CCHANNEL_B:
                return tstring(_T("B"));
            case CCHANNEL_ALPHA:
                return tstring(_T("A"));
            default:
                return tstring(_T("UNKNOWN"));
        }
    }
    else
    {
        switch(uiChannelIndex)
        {
            case CCHANNEL_Y:
                return tstring(_T("Y"));
            case CCHANNEL_U:
                return tstring(_T("U"));
            case CCHANNEL_V:
                return tstring(_T("V"));
            case CCHANNEL_ALPHA:
                return tstring(_T("A"));
            default:
                return tstring(_T("UNKNOWN"));
        }
    }
}

//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given DXGI_RATIONAL.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const   DXGI_RATIONAL &Rational
)
{    
	static TCHAR  pstr[MAX_STRING];

	_stprintf_s(pstr, 
                MAX_STRING, 
                TEXT("%d/%d"), 
                Rational.Numerator, 
                Rational.Denominator);

	return tstring(pstr);
}

//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given DXGI_MODE_SCANLINE_ORDER.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const   DXGI_MODE_SCANLINE_ORDER &ScanLineOrder
)
{    
    switch(ScanLineOrder)
    {
        case DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED:
            return tstring(_T("DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED"));
        case DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE:
            return tstring(_T("DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE"));
        case DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST:
            return tstring(_T("DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST"));
        case DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST:
            return tstring(_T("DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST"));
        default:
            return tstring(_T("UNKNOWN"));
    }
}

//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given DXGI_MODE_SCALING.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const   DXGI_MODE_SCALING &Scalling
)
{    
    switch(Scalling)
    {
        case DXGI_MODE_SCALING_UNSPECIFIED:
            return tstring(_T("DXGI_MODE_SCALING_UNSPECIFIED"));
        case DXGI_MODE_SCALING_CENTERED:
            return tstring(_T("DXGI_MODE_SCALING_CENTERED"));
        case DXGI_MODE_SCALING_STRETCHED:
            return tstring(_T("DXGI_MODE_SCALING_STRETCHED"));
        default:
            return tstring(_T("UNKNOWN"));
    }
}

//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3D11_VIDEO_PROCESSOR_STEREO_FORMAT.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3D11_VIDEO_PROCESSOR_STEREO_FORMAT   *pStereoFormat, 
    const tstring              &StereoFormatString
)
{
    STRING_TO_ENUM(StereoFormatString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_MONO, 
                   pStereoFormat);
    STRING_TO_ENUM(StereoFormatString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_HORIZONTAL, 
                   pStereoFormat);
    STRING_TO_ENUM(StereoFormatString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_VERTICAL, 
                   pStereoFormat);
    STRING_TO_ENUM(StereoFormatString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_SEPARATE, 
                   pStereoFormat);
    STRING_TO_ENUM(StereoFormatString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_MONO_OFFSET, 
                   pStereoFormat);
    STRING_TO_ENUM(StereoFormatString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_ROW_INTERLEAVED, 
                   pStereoFormat);
    STRING_TO_ENUM(StereoFormatString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_COLUMN_INTERLEAVED, 
                   pStereoFormat);
    STRING_TO_ENUM(StereoFormatString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_CHECKERBOARD, 
                   pStereoFormat);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3D11_VIDEO_PROCESSOR_STEREO_FORMAT.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3D11_VIDEO_PROCESSOR_STEREO_FORMAT &StereoFormatType
)
{
      ENUM_TO_STRING(StereoFormatType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_MONO);
      ENUM_TO_STRING(StereoFormatType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_HORIZONTAL);
      ENUM_TO_STRING(StereoFormatType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_VERTICAL);
      ENUM_TO_STRING(StereoFormatType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_SEPARATE);
      ENUM_TO_STRING(StereoFormatType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_MONO_OFFSET);
      ENUM_TO_STRING(StereoFormatType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_ROW_INTERLEAVED);
      ENUM_TO_STRING(StereoFormatType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_COLUMN_INTERLEAVED);
      ENUM_TO_STRING(StereoFormatType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_CHECKERBOARD);

      return tstring(_T("D3D11_VIDEO_PROCESSOR_STEREO_FORMAT_UNKNOWN"));
};

//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3D11_VIDEO_PROCESSOR_STEREO_FLIP_MODE.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3D11_VIDEO_PROCESSOR_STEREO_FLIP_MODE   *pStereoFlipMode, 
    const tstring              &StereoFlipModeString
)
{
    STRING_TO_ENUM(StereoFlipModeString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FLIP_NONE, 
                   pStereoFlipMode);
    STRING_TO_ENUM(StereoFlipModeString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FLIP_FRAME0, 
                   pStereoFlipMode);
    STRING_TO_ENUM(StereoFlipModeString, 
                   D3D11_VIDEO_PROCESSOR_STEREO_FLIP_FRAME1, 
                   pStereoFlipMode);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3D11_VIDEO_PROCESSOR_STEREO_FLIP_MODE.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3D11_VIDEO_PROCESSOR_STEREO_FLIP_MODE &StereoFlipModeType
)
{
      ENUM_TO_STRING(StereoFlipModeType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FLIP_NONE);
      ENUM_TO_STRING(StereoFlipModeType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FLIP_FRAME0);
      ENUM_TO_STRING(StereoFlipModeType, 
                     D3D11_VIDEO_PROCESSOR_STEREO_FLIP_FRAME1);


      return tstring(_T("D3D11_VIDEO_PROCESSOR_STEREO_FLIP_MODE_UNKNOWN"));
};

//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a D3D11_VIDEO_PROCESSOR_ROTATION.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    D3D11_VIDEO_PROCESSOR_ROTATION   *pRotatationValue, 
    const tstring              &RotateValString
)
{
    STRING_TO_ENUM(RotateValString, 
                   D3D11_VIDEO_PROCESSOR_ROTATION_IDENTITY, 
                   pRotatationValue);
    STRING_TO_ENUM(RotateValString, 
                   D3D11_VIDEO_PROCESSOR_ROTATION_90, 
                   pRotatationValue);
    STRING_TO_ENUM(RotateValString, 
                   D3D11_VIDEO_PROCESSOR_ROTATION_180, 
                   pRotatationValue);
    STRING_TO_ENUM(RotateValString, 
                   D3D11_VIDEO_PROCESSOR_ROTATION_270, 
                   pRotatationValue);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a D3D11_VIDEO_PROCESSOR_ROTATION.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const D3D11_VIDEO_PROCESSOR_ROTATION &RotatationValue
)
{
      ENUM_TO_STRING(RotatationValue, 
                     D3D11_VIDEO_PROCESSOR_ROTATION_IDENTITY);
      ENUM_TO_STRING(RotatationValue, 
                     D3D11_VIDEO_PROCESSOR_ROTATION_90);
      ENUM_TO_STRING(RotatationValue, 
                     D3D11_VIDEO_PROCESSOR_ROTATION_180);
      ENUM_TO_STRING(RotatationValue, 
                     D3D11_VIDEO_PROCESSOR_ROTATION_270);

      return tstring(_T("D3D11_VIDEO_PROCESSOR_ROTATION_UNKNOWN"));
};

//-----------------------------------------------------------------------------
// Name: FromString
// Desc: Converts the string to a DXGI_MODE_ROTATION.
//-----------------------------------------------------------------------------
bool __stdcall FromString
(
    DXGI_MODE_ROTATION   *pRotatationValue, 
    const tstring              &RotateValString
)
{
    STRING_TO_ENUM(RotateValString, 
                   DXGI_MODE_ROTATION_IDENTITY, 
                   pRotatationValue);
    STRING_TO_ENUM(RotateValString, 
                   DXGI_MODE_ROTATION_ROTATE90, 
                   pRotatationValue);
    STRING_TO_ENUM(RotateValString, 
                   DXGI_MODE_ROTATION_ROTATE180, 
                   pRotatationValue);
    STRING_TO_ENUM(RotateValString, 
                   DXGI_MODE_ROTATION_ROTATE270, 
                   pRotatationValue);

    return false;
}
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string of a DXGI_MODE_ROTATION.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const DXGI_MODE_ROTATION &RotatationValue
)
{
      ENUM_TO_STRING(RotatationValue, 
                     DXGI_MODE_ROTATION_IDENTITY);
      ENUM_TO_STRING(RotatationValue, 
                     DXGI_MODE_ROTATION_ROTATE90);
      ENUM_TO_STRING(RotatationValue, 
                     DXGI_MODE_ROTATION_ROTATE180);
      ENUM_TO_STRING(RotatationValue, 
                     DXGI_MODE_ROTATION_ROTATE270);

      return tstring(_T("DXGI_MODE_ROTATION_UNSPECIFIED"));
};

#ifdef WGFTEST
//-----------------------------------------------------------------------------
// Name: ToString
// Desc: Returns the string for the given TEST_RESULT.
//-----------------------------------------------------------------------------
tstring __stdcall ToString
(
    const TEST_RESULT   &TestResult
)
{
    ENUM_TO_STRING(TestResult, RESULT_NONE);
    ENUM_TO_STRING(TestResult, RESULT_PASS);
    ENUM_TO_STRING(TestResult, RESULT_FAIL);
    ENUM_TO_STRING(TestResult, RESULT_SKIP);
    ENUM_TO_STRING(TestResult, RESULT_ABORT);
    ENUM_TO_STRING(TestResult, RESULT_PENDING);
 
    return tstring(_T("RESULT_UNKNOWN"));
}
#endif