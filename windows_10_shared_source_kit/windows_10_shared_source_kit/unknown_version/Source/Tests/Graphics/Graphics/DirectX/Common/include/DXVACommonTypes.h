#define  BFT_BITMAP         0x4d42

const size_t    MAX_CHANNELS            = 4;
const size_t    MAX_GUID                = 39;
const size_t    MAX_STRING              = 512;
const D3DFORMAT D3DFMT_YUVCOMMON        = D3DFMT_YUY2;
const float     THRESHOLD_PSNR          = 20.0f;
const float     THRESHOLD_LOOSE_PSNR_R  = 6.4f;
const float     THRESHOLD_LOOSE_PSNR_G  = 6.4f;
const float     THRESHOLD_LOOSE_PSNR_B  = 6.4f;
const UINT      DXVA2_UAB_INVALID       = 16;
const UINT      DXVA2_RESERVED_INVALID  = 16;
const RECT      EMPTY_RECT              = {0, 0, 0, 0};

//
// Recognized Pixel formats by dxva tests.
//
const D3DFORMAT D3DFMT_YV12 = (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2');
const D3DFORMAT D3DFMT_NV11 = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '1');
const D3DFORMAT D3DFMT_NV12 = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
const D3DFORMAT D3DFMT_NV21 = (D3DFORMAT)MAKEFOURCC('N', 'V', '2', '1');
const D3DFORMAT D3DFMT_AYUV = (D3DFORMAT)MAKEFOURCC('A', 'Y', 'U', 'V');
const D3DFORMAT D3DFMT_IMC1 = (D3DFORMAT)MAKEFOURCC('I', 'M', 'C', '1');
const D3DFORMAT D3DFMT_IMC2 = (D3DFORMAT)MAKEFOURCC('I', 'M', 'C', '2');
const D3DFORMAT D3DFMT_IMC3 = (D3DFORMAT)MAKEFOURCC('I', 'M', 'C', '3');
const D3DFORMAT D3DFMT_IMC4 = (D3DFORMAT)MAKEFOURCC('I', 'M', 'C', '4');
const D3DFORMAT D3DFMT_AI44 = (D3DFORMAT)MAKEFOURCC('A', 'I', '4', '4');
const D3DFORMAT D3DFMT_IA44 = (D3DFORMAT)MAKEFOURCC('I', 'A', '4', '4');

#define MAKEYUVCOLOR(Color) \
                    (D3DCOLOR_XYUV((DWORD)((0.257 * LOBYTE(LOWORD(Color))) + (0.504 * HIBYTE(LOWORD(Color))) + (0.098 * LOBYTE(HIWORD(Color))) + 16), \
                                   (DWORD)((0.439 * LOBYTE(LOWORD(Color))) - (0.368 * HIBYTE(LOWORD(Color))) - (0.071 * LOBYTE(HIWORD(Color))) + 128), \
                                   (DWORD)(-(0.148 * LOBYTE(LOWORD(Color))) - (0.291 * HIBYTE(LOWORD(Color))) + (0.439 * LOBYTE(HIWORD(Color))) + 128)))

//
// Logging definitions and constants.
//
#define  LOG_NOCOMMANDLINE      0x00000001
#define  LOG_BEGIN_TESTCASE     0x00000002
#define  LOG_END_TESTCASE       0x00000004
#define  LOG_STREAM_STATE_DATA  0x00000100
#define  LOG_BLT_STATE_DATA     0x00000200

const size_t    LOG_FLAG_MIN_STRING     = 512;
const size_t    LOG_FLAG_MID_STRING     = 1024;
const size_t    LOG_FLAG_MAX_STRING     = 4096;
const LPCSTR    LOG_BEGIN_FLAG          = "==========================================================================================";
const LPCSTR    LOG_END_FLAG            = "==========================================================================================";
const LPCSTR    LOG_BEGIN_TESTCASE_STR  = "BEGIN TESTCASE:";
const LPCSTR    LOG_END_TESTCASE_STR    = "END TESTCASE:";


//
// Common DXVA2 frame rate and output freq.
//
const DXVA2_Frequency DXVA2_FREQ_24      = { 1, 24 };
const DXVA2_Frequency DXVA2_FREQ_30      = { 3003, 90000 };
const DXVA2_Frequency DXVA2_FREQ_50      = { 1, 50 };
const DXVA2_Frequency DXVA2_FREQ_60      = { 3003, 180000 };
const DXVA2_Frequency DXVA2_FREQ_75      = { 1, 75 };
const DXVA2_Frequency DXVA2_FREQ_INVALID = { 60, 0 };

//
// DXVA2 Commonly used SampleFormat configurations.
//
const DXVA2_ExtendedFormat DXVA2_SAMPLE_FORMAT_PROGRESSIVE = { DXVA2_SampleProgressiveFrame,
                                                               DXVA2_VideoChromaSubsampling_Unknown,
                                                               DXVA2_NominalRange_Normal,
                                                               DXVA2_VideoTransferMatrix_Unknown,
                                                               DXVA2_VideoLighting_Unknown,
                                                               DXVA2_VideoPrimaries_Unknown,
                                                               DXVA2_VideoTransFunc_Unknown };
const DXVA2_ExtendedFormat DXVA2_SAMPLE_FORMAT_INTERLACED  = { DXVA2_SampleFieldInterleavedEvenFirst,
                                                               DXVA2_VideoChromaSubsampling_Unknown,
                                                               DXVA2_NominalRange_Normal,
                                                               DXVA2_VideoTransferMatrix_Unknown,
                                                               DXVA2_VideoLighting_Unknown,
                                                               DXVA2_VideoPrimaries_Unknown,
                                                               DXVA2_VideoTransFunc_Unknown };
const DXVA2_ExtendedFormat DXVA2_SAMPLE_FORMAT_ADVANCED    = { DXVA2_SampleFieldInterleavedOddFirst,
                                                               DXVA2_VideoChromaSubsampling_MPEG2,
                                                               DXVA2_NominalRange_Wide,
                                                               DXVA2_VideoTransferMatrix_BT709,
                                                               DXVA2_VideoLighting_bright,
                                                               DXVA2_VideoPrimaries_BT709,
                                                               DXVA2_VideoTransFunc_Unknown };

//
// List of DXVAHD blt states.
//
const DXVAHD_BLT_STATE BltStates[] =
{
    DXVAHD_BLT_STATE_TARGET_RECT,
    DXVAHD_BLT_STATE_BACKGROUND_COLOR,
    DXVAHD_BLT_STATE_OUTPUT_COLOR_SPACE,
    DXVAHD_BLT_STATE_ALPHA_FILL,
    DXVAHD_BLT_STATE_CONSTRICTION,
    DXVAHD_BLT_STATE_PRIVATE
};
const UINT BLT_STATE_COUNT = (sizeof(BltStates) / sizeof(DXVAHD_BLT_STATE));

//
// DXVAHD Default Blt state data.
//
const DXVAHD_COLOR_RGBA                              DefaultBackgroundColor = {0, 0, 0, 1};
const DXVAHD_BLT_STATE_TARGET_RECT_DATA               DefaultTargetRectData = {FALSE, EMPTY_RECT};
const DXVAHD_BLT_STATE_BACKGROUND_COLOR_DATA     DefaultBackgroundColorData = {FALSE, DefaultBackgroundColor};
const DXVAHD_BLT_STATE_OUTPUT_COLOR_SPACE_DATA  DefaultOutputColorSpaceData = {0, 0, 0, 0, 0};
const DXVAHD_BLT_STATE_ALPHA_FILL_DATA                 DefaultAlphaFillData = {DXVAHD_ALPHA_FILL_MODE_OPAQUE, 0};
const DXVAHD_BLT_STATE_CONSTRICTION_DATA            DefaultConstrictionData = {FALSE, {1, 1}};

//
// List of DXVAHD stream states.
//
const DXVAHD_STREAM_STATE StreamStates[] =
{
    DXVAHD_STREAM_STATE_D3DFORMAT,
    DXVAHD_STREAM_STATE_FRAME_FORMAT,
    DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE,
    DXVAHD_STREAM_STATE_OUTPUT_RATE,
    DXVAHD_STREAM_STATE_SOURCE_RECT,
    DXVAHD_STREAM_STATE_DESTINATION_RECT,
    DXVAHD_STREAM_STATE_ALPHA,
    DXVAHD_STREAM_STATE_PALETTE,
    DXVAHD_STREAM_STATE_LUMA_KEY,
    DXVAHD_STREAM_STATE_ASPECT_RATIO,
    DXVAHD_STREAM_STATE_FILTER_BRIGHTNESS,
    DXVAHD_STREAM_STATE_FILTER_CONTRAST,
    DXVAHD_STREAM_STATE_FILTER_HUE,
    DXVAHD_STREAM_STATE_FILTER_SATURATION,
    DXVAHD_STREAM_STATE_FILTER_NOISE_REDUCTION,
    DXVAHD_STREAM_STATE_FILTER_EDGE_ENHANCEMENT,
    DXVAHD_STREAM_STATE_FILTER_ANAMORPHIC_SCALING,
    DXVAHD_STREAM_STATE_PRIVATE
};
const UINT STREAM_STATE_COUNT = (sizeof(StreamStates) / sizeof(DXVAHD_STREAM_STATE));

//
// DXVAHD Default Stream state data.
//
const DXVAHD_STREAM_STATE_D3DFORMAT_DATA               DefaultD3DFormatData = {D3DFMT_UNKNOWN};
const DXVAHD_STREAM_STATE_FRAME_FORMAT_DATA          DefaultFrameFormatData = {DXVAHD_FRAME_FORMAT_PROGRESSIVE};
const DXVAHD_STREAM_STATE_INPUT_COLOR_SPACE_DATA DefaultInputColorSpaceData = {0, 0, 0, 0, 0};
const DXVAHD_STREAM_STATE_OUTPUT_RATE_DATA            DefaultOutputRateData = {FALSE, DXVAHD_OUTPUT_RATE_NORMAL, {1, 1}};
const DXVAHD_STREAM_STATE_SOURCE_RECT_DATA            DefaultSourceRectData = {FALSE, EMPTY_RECT};
const DXVAHD_STREAM_STATE_DESTINATION_RECT_DATA  DefaultDestinationRectData = {FALSE, EMPTY_RECT};
const DXVAHD_STREAM_STATE_ALPHA_DATA                       DefaultAlphaData = {FALSE, 1.0};
const DXVAHD_STREAM_STATE_PALETTE_DATA                   DefaultPaletteData = {0, NULL};
const DXVAHD_STREAM_STATE_LUMA_KEY_DATA                  DefaultLumaKeyData = {FALSE, 0.0, 0.0};
const DXVAHD_STREAM_STATE_ASPECT_RATIO_DATA          DefaultAspectRatioData = {FALSE, {1, 1}, {1, 1}};
const DXVAHD_STREAM_STATE_FILTER_DATA                     DefaultFilterData = {FALSE, 0};

//
// Mapping directly to DXVAHD_SURFACE_TYPE with an additional surface type
// of non-video input.
//
typedef enum _CDXVAHD_SURFACE_TYPE
{
    CDXVAHD_SURFACE_TYPE_VIDEO_OUTPUT = 0,
    CDXVAHD_SURFACE_TYPE_VIDEO_INPUT = 1,
    CDXVAHD_SURFACE_TYPE_OUTPUT = 2,
    CDXVAHD_SURFACE_TYPE_INPUT = 3
} CDXVAHD_SURFACE_TYPE;

//
// The way to scale content or a rect.
//
typedef enum _CSCALE_TYPE
{
	CSCALE_TYPE_NONE	= 0,
	CSCALE_TYPE_STRETCH = 1,
	CSCALE_TYPE_SHRINK	= 2
} CSCALE_TYPE;

//
// The orientation of a movement or a scale.
//
typedef enum _CORIENTATION
{
    CORIENTATION_VERTICAL   = 0,
    CORIENTATION_HORIZONTAL = 1
} CORIENTATION;

//
// Channel table used for filling A/XRGB and YUV surfaces with content 
// as well as used for evaluation of content to software and static.
//
typedef enum _CCHANNEL
{   
    CCHANNEL_R = 0,
    CCHANNEL_Y = 0,
    CCHANNEL_G = 1,
    CCHANNEL_U = 1,
    CCHANNEL_B = 2,
    CCHANNEL_V = 2,
    CCHANNEL_ALPHA = 3
}   CCHANNEL;

typedef enum _CDXVA_CONTENT_COMPRESSION
{
    CDXVA_CONTENT_COMPRESSION_MPEG2 = 0,
    CDXVA_CONTENT_COMPRESSION_H264  = 1,
    CDXVA_CONTENT_COMPRESSION_VC1   = 2,
    CDXVA_CONTENT_COMPRESSION_WMV8  = 3,
    CDXVA_CONTENT_COMPRESSION_WMV9  = 4,
    CDXVA_CONTENT_COMPRESSION_NONE  = 5
} CDXVA_CONTENT_COMPRESSION;

//
// Global DXVA1/2 HD content Desc can be converted for all interfaces.
//
typedef struct _CDXVA_CONTENT_DESC
{
    UINT                        InputWidth;
    UINT                        InputHeight;
    DXVAHD_RATIONAL             InputFrameRate;
    UINT                        OutputWidth;
    UINT                        OutputHeight;
    DXVAHD_RATIONAL             OutputFrameRate;
    D3DFORMAT                   Format;
    CDXVA_CONTENT_COMPRESSION   ContentCompression;
    UINT                        StreamCount;
    DXVA2_SampleFormat          SampleFormat;
    DXVA2_ExtendedFormat        ExtendedFormat;
} CDXVA_CONTENT_DESC;
const CDXVA_CONTENT_DESC CDXVA_CONTENT_DESC_DEFAULT = {640, 
                                                       480, 
                                                       {24, 1}, 
                                                       640, 
                                                       480, 
                                                       {60, 1}, 
                                                       D3DFMT_YUVCOMMON, 
                                                       CDXVA_CONTENT_COMPRESSION_NONE,
                                                       1,
                                                       DXVA2_SampleUnknown,
                                                       {DXVA2_SampleProgressiveFrame,
                                                        DXVA2_VideoChromaSubsampling_MPEG2,
                                                        DXVA2_NominalRange_Normal,
                                                        DXVA2_VideoTransferMatrix_BT601,
                                                        DXVA2_VideoLighting_Unknown,
                                                        DXVA2_VideoPrimaries_Unknown,
                                                        DXVA2_VideoTransFunc_Unknown}};

//
// Used for adjusting surface validation
//
typedef struct _CDXVA_SURFACE_VALIDATION_ARGS
{
    float LOWERTHAN_CHANNEL0; // PSNR should be lower than this value for this validation
    float THRESHOLD_CHANNEL0; // PSNR should be higher than this value for this validation
    float LOWERTHAN_CHANNEL1;
    float THRESHOLD_CHANNEL1;
    float LOWERTHAN_CHANNEL2;
    float THRESHOLD_CHANNEL2;
    float LOWERTHAN_CHANNEL3;
    float THRESHOLD_CHANNEL3;
} CDXVA_SURFACE_VALIDATION_ARGS;
const CDXVA_SURFACE_VALIDATION_ARGS CDXVAHD_SURFACE_VALIDATION_ARGS_DEFAULT = {THRESHOLD_PSNR,
                                                                               THRESHOLD_PSNR, 
                                                                               THRESHOLD_PSNR, 
                                                                               THRESHOLD_PSNR};

//
// D3D pixel format and channel description structs.
//
typedef	struct	_CD3DCHANNEL_DESCRIPTOR
{
	UINT		InitialOffset;          //  distance (in bytes) to the first sample of this channel
	UINT		DistToNextSample;       //  distance to next sample on the same channel
	UINT		HSubSampling;           //  horizontal sub-sampling factor
	UINT		VSubSampling;           //  vertical sub-sampling factor
	float		PitchFactor;            //  pitch multiplication factor
}	CD3DCHANNEL_DESCRIPTOR;

typedef	struct	_CD3DFMT_DESCRIPTOR
{
	D3DFORMAT				D3DFormat;
    bool                    bIsPalettized;
    UINT                    uiPaletteCount;
    D3DCOLOR*               pPalette;
	UINT					uiChannelCount;
	CD3DCHANNEL_DESCRIPTOR	D3DChannelDescriptors[MAX_CHANNELS];
}	CD3DFMT_DESCRIPTOR;
const CD3DFMT_DESCRIPTOR CD3DFMT_DESCRIPTOR_DEFAULT = {D3DFMT_UNKNOWN, false, 0, NULL, {0}};

//
// Video Description struct with a boolean value representing a valid or invalid
// video description.
//
typedef struct _CDXVA2_VIDEO_DESC
{
    BOOL            bInvalid;
    DXVA2_VideoDesc VideoDesc;
} CDXVA2_VIDEO_DESC;

////////////////////////////// RGB ////////////////////////////////
// True RGB [0...255] colors.
// 100%
const D3DCOLOR RGB_TRUE_WHITE   = D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF);
const D3DCOLOR RGB_TRUE_RED     = D3DCOLOR_XRGB(0xFF, 0x00, 0x00);
const D3DCOLOR RGB_TRUE_YELLOW  = D3DCOLOR_XRGB(0xFF, 0xFF, 0x00);
const D3DCOLOR RGB_TRUE_GREEN   = D3DCOLOR_XRGB(0x00, 0xFF, 0x00);
const D3DCOLOR RGB_TRUE_BLUE    = D3DCOLOR_XRGB(0x00, 0x00, 0xFF);
const D3DCOLOR RGB_TRUE_BLACK   = D3DCOLOR_XRGB(0x00, 0x00, 0x00);

// Studio RGB [16...235] colors.
// 100%
const D3DCOLOR RGB_WHITE        = D3DCOLOR_XRGB(0xEB, 0xEB, 0xEB);
const D3DCOLOR RGB_RED          = D3DCOLOR_XRGB(0xEB, 0x10, 0x10);
const D3DCOLOR RGB_YELLOW       = D3DCOLOR_XRGB(0xEB, 0xEB, 0x10);
const D3DCOLOR RGB_GREEN        = D3DCOLOR_XRGB(0x10, 0xEB, 0x10);
const D3DCOLOR RGB_CYAN         = D3DCOLOR_XRGB(0x10, 0xEB, 0xEB);
const D3DCOLOR RGB_BLUE         = D3DCOLOR_XRGB(0x10, 0x10, 0xEB);
const D3DCOLOR RGB_MAGENTA      = D3DCOLOR_XRGB(0xEB, 0x10, 0xEB);
const D3DCOLOR RGB_BLACK        = D3DCOLOR_XRGB(0x10, 0x10, 0x10);
const D3DCOLOR RGB_PINK         = D3DCOLOR_XRGB(0xCD, 0x10, 0x76);

// 75%
const D3DCOLOR RGB_WHITE_75pc   = D3DCOLOR_XRGB(0xB4, 0xB4, 0xB4);
const D3DCOLOR RGB_YELLOW_75pc  = D3DCOLOR_XRGB(0xB4, 0xB4, 0x10);
const D3DCOLOR RGB_CYAN_75pc    = D3DCOLOR_XRGB(0x10, 0xB4, 0xB4);
const D3DCOLOR RGB_GREEN_75pc   = D3DCOLOR_XRGB(0x10, 0xB4, 0x10);
const D3DCOLOR RGB_MAGENTA_75pc = D3DCOLOR_XRGB(0xB4, 0x10, 0xB4);
const D3DCOLOR RGB_RED_75pc     = D3DCOLOR_XRGB(0xB4, 0x10, 0x10);
const D3DCOLOR RGB_BLUE_75pc    = D3DCOLOR_XRGB(0x10, 0x10, 0xB4);

// -4% / +4%
const D3DCOLOR RGB_BLACK_n4pc   = D3DCOLOR_XRGB(0x07, 0x07, 0x07);
const D3DCOLOR RGB_BLACK_p4pc   = D3DCOLOR_XRGB(0x18, 0x18, 0x18);

// -Inphase / +Quadrature
const D3DCOLOR RGB_I            = D3DCOLOR_XRGB(0x00, 0x1D, 0x42);
const D3DCOLOR RGB_Q            = D3DCOLOR_XRGB(0x2C, 0x00, 0x5C);
//////////////////////////// END RGB //////////////////////////////

////////////////////////////// YUV ////////////////////////////////
// YUV BT.709 colors
// 100%
const D3DCOLOR YUV_WHITE        = MAKEYUVCOLOR(RGB_WHITE);
const D3DCOLOR YUV_RED          = MAKEYUVCOLOR(RGB_RED);
const D3DCOLOR YUV_YELLOW       = MAKEYUVCOLOR(RGB_YELLOW);
const D3DCOLOR YUV_GREEN        = MAKEYUVCOLOR(RGB_GREEN);
const D3DCOLOR YUV_CYAN         = MAKEYUVCOLOR(RGB_CYAN);
const D3DCOLOR YUV_BLUE         = MAKEYUVCOLOR(RGB_BLUE);
const D3DCOLOR YUV_MAGENTA      = MAKEYUVCOLOR(RGB_MAGENTA);
const D3DCOLOR YUV_BLACK        = MAKEYUVCOLOR(RGB_BLACK);
const D3DCOLOR YUV_PINK         = MAKEYUVCOLOR(RGB_PINK);

// 75%
const D3DCOLOR YUV_WHITE_75pc   = MAKEYUVCOLOR(RGB_WHITE_75pc);
const D3DCOLOR YUV_YELLOW_75pc  = MAKEYUVCOLOR(RGB_YELLOW_75pc);
const D3DCOLOR YUV_CYAN_75pc    = MAKEYUVCOLOR(RGB_CYAN_75pc);
const D3DCOLOR YUV_GREEN_75pc   = MAKEYUVCOLOR(RGB_GREEN_75pc);
const D3DCOLOR YUV_MAGENTA_75pc = MAKEYUVCOLOR(RGB_MAGENTA_75pc);
const D3DCOLOR YUV_RED_75pc     = MAKEYUVCOLOR(RGB_RED_75pc);
const D3DCOLOR YUV_BLUE_75pc    = MAKEYUVCOLOR(RGB_BLUE_75pc);

// -4% / +4%
const D3DCOLOR YUV_BLACK_n4pc   = MAKEYUVCOLOR(RGB_BLACK_n4pc);
const D3DCOLOR YUV_BLACK_p4pc   = MAKEYUVCOLOR(RGB_BLACK_p4pc);

// -Inphase / +Quadrature
const D3DCOLOR YUV_I            = MAKEYUVCOLOR(RGB_I);
const D3DCOLOR YUV_Q            = MAKEYUVCOLOR(RGB_Q);
//////////////////////////// END YUV //////////////////////////////

const UINT SOLID_COLOR_FILL_INDEX = 12; // MAGENTA_75pc

const D3DCOLOR  RGBPalette[] = 
{    
    RGB_TRUE_WHITE,
    RGB_TRUE_YELLOW,
    RGB_CYAN,
    RGB_TRUE_GREEN,
    RGB_MAGENTA,
    RGB_TRUE_RED,
    RGB_TRUE_BLUE,
    RGB_TRUE_BLACK,

    RGB_WHITE_75pc,
    RGB_YELLOW_75pc,
    RGB_CYAN_75pc,
    RGB_GREEN_75pc,
    RGB_MAGENTA_75pc,
    RGB_RED_75pc,
    RGB_BLUE_75pc,
    RGB_BLACK_n4pc,
    RGB_BLACK_p4pc,

    RGB_I,
    RGB_Q
};
const UINT RGB_PALETTE_COUNT = (sizeof(RGBPalette) / sizeof(D3DCOLOR));

const D3DCOLOR YUVPalette_BT709[] = 
{
    YUV_WHITE,
    YUV_YELLOW,
    YUV_CYAN,
    YUV_GREEN,
    YUV_MAGENTA,
    YUV_RED,
    YUV_BLUE,
    YUV_BLACK,

    YUV_WHITE_75pc,
    YUV_YELLOW_75pc,
    YUV_CYAN_75pc,
    YUV_GREEN_75pc,
    YUV_MAGENTA_75pc,
    YUV_RED_75pc,
    YUV_BLUE_75pc,
    YUV_BLACK_n4pc,
    YUV_BLACK_p4pc,

    YUV_I,
    YUV_Q
};
const UINT YUV_PALETTE_BT709_COUNT = (sizeof(YUVPalette_BT709) / sizeof(D3DCOLOR));

const D3DCOLOR YUVPalette_16Color[] =
{
    YUV_WHITE,
    YUV_YELLOW,
    YUV_CYAN,
    YUV_GREEN,
    YUV_MAGENTA,
    YUV_RED,
    YUV_BLUE,
    YUV_BLACK,

    YUV_WHITE_75pc,
    YUV_YELLOW_75pc,
    YUV_CYAN_75pc,
    YUV_GREEN_75pc,
    YUV_MAGENTA_75pc,
    YUV_RED_75pc,
    YUV_BLUE_75pc,
    YUV_BLACK_n4pc
};
const UINT YUV_PALETTE_16COLOR_COUNT = (sizeof(YUVPalette_16Color) / sizeof(D3DCOLOR));

//
// YUV and RGB pixel format description table.
//
const CD3DFMT_DESCRIPTOR	D3DFormatDescriptors[] =
{
    // Struct representation definition.
    // D3DFORMAAT, IsPalettized, PALETTE_COUNT, Palette, CHANNEL_COUNT, Initial Offset, Dist to next sample, HSubSmpl, VSubSmpl, Pitch Factor
    //                                                                                     Y                   U                   V                  A
    //	YUV, 4:2:2
    {D3DFMT_YUY2, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{0, 2, 1, 1, 1.0}, {1, 4, 2, 1, 1.0}, {3, 4, 2, 1, 1.0}, {0, 0, 0, 0, .0}}},
    {D3DFMT_UYVY, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{1, 2, 1, 1, 1.0}, {0, 4, 2, 1, 1.0}, {2, 4, 2, 1, 1.0}, {0, 0, 0, 0, .0}}},

    //	YUV, 4:2:0
    // NOTE: If you add planar formats to the list of supported formats update the common routine 
    //       SetInitialOffset to include the initial offset value based on the height and width of 
    //       the surface.
    {D3DFMT_YV12, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{0, 1, 1, 1, 1.0}, {0, 1, 2, 2, 0.5}, {0, 1, 2, 2, 0.5}, {0, 0, 0, 0, .0}}},
    {D3DFMT_NV12, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{0, 1, 1, 1, 1.0}, {0, 2, 2, 2, 1.0}, {1, 2, 2, 2, 1.0}, {0, 0, 0, 0, .0}}},
    {D3DFMT_NV21, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{0, 1, 1, 1, 1.0}, {0, 2, 2, 2, 1.0}, {0, 2, 2, 2, 1.0}, {0, 0, 0, 0, .0}}},
    {D3DFMT_IMC1, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{0, 1, 1, 1, 1.0}, {0, 1, 2, 2, 1.0}, {0, 1, 2, 2, 1.0}, {0, 0, 0, 0, .0}}},
    {D3DFMT_IMC2, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{0, 1, 1, 1, 1.0}, {0, 1, 2, 2, 1.0}, {0, 1, 2, 2, 1.0}, {0, 0, 0, 0, .0}}},
    {D3DFMT_IMC3, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{0, 1, 1, 1, 1.0}, {0, 1, 2, 2, 1.0}, {0, 1, 2, 2, 1.0}, {0, 0, 0, 0, .0}}},
    {D3DFMT_IMC4, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 3, {{0, 1, 1, 1, 1.0}, {0, 1, 2, 2, 1.0}, {0, 1, 2, 2, 1.0}, {0, 0, 0, 0, .0}}},

    //  YUV, 4:1:1

    //	YUV, 4:4:4 (Packed)
    {D3DFMT_AYUV, FALSE, YUV_PALETTE_BT709_COUNT, (D3DCOLOR*)&YUVPalette_BT709, 4, {{2, 4, 1, 1, 1.0}, {1, 4, 1, 1, 1.0}, {0, 4, 1, 1, 1.0}, {3, 4, 1, 1, 1.0}}},

    //  YUV, 4:4:4 (Palettized)
    //  NOTE: If you add a palettized format be sure to update the DXVACommon.h IsPalettized format
    //        inline function so that any tests which require knowledge of palettized formats are 
    //        accurate. As well as updating the GetPalettizedColorIndex routine in DXVACommon.cpp so
    //        that the color index is at the correct offset in the word.
    {D3DFMT_AI44, TRUE, YUV_PALETTE_16COLOR_COUNT, (D3DCOLOR*)&YUVPalette_16Color, 4, {{0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}}},
    {D3DFMT_IA44, TRUE, YUV_PALETTE_16COLOR_COUNT, (D3DCOLOR*)&YUVPalette_16Color, 4, {{0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}}},

    //	RGB, 32 bits
    {D3DFMT_A8R8G8B8, FALSE, RGB_PALETTE_COUNT, (D3DCOLOR*)&RGBPalette, 4, {{2, 4, 1, 1, 1.0}, {1, 4, 1, 1, 1.0}, {0, 4, 1, 1, 1.0}, {3, 4, 1, 1, 1.0}}}, 
    {D3DFMT_X8R8G8B8, FALSE, RGB_PALETTE_COUNT, (D3DCOLOR*)&RGBPalette, 4, {{2, 4, 1, 1, 1.0}, {1, 4, 1, 1, 1.0}, {0, 4, 1, 1, 1.0}, {3, 4, 1, 1, 1.0}}},

    //  RGB, 16 bits (SEE 4:4:4 YUV Palettized above for palette entries)
    {D3DFMT_A8P8, TRUE, RGB_PALETTE_COUNT, (D3DCOLOR*)&RGBPalette, 4, {{2, 2, 2, 2, 1.0}, {1, 2, 2, 2, 1.0}, {0, 2, 2, 2, 1.0}, {3, 2, 2, 2, 1.0}}},

    //  RGB, 8 bits (SEE 4:4:4 YUV Palettized above for palette entries)
    {D3DFMT_P8,   TRUE, RGB_PALETTE_COUNT, (D3DCOLOR*)&RGBPalette, 4, {{0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}, {0, 1, 1, 1, 1.0}}}
};
const UINT D3DFORMAT_DESCRIPTORS_COUNT = (sizeof(D3DFormatDescriptors) / sizeof(CD3DFMT_DESCRIPTOR));

const D3DSCANLINEORDERING ScanLineOrderings[] =
{
    D3DSCANLINEORDERING_UNKNOWN,
    D3DSCANLINEORDERING_PROGRESSIVE,
    D3DSCANLINEORDERING_INTERLACED
};
const UINT SCANLINEORDERINGS_COUNT = (sizeof(ScanLineOrderings) / sizeof(D3DSCANLINEORDERING));

const CDXVA2_VIDEO_DESC VideoDescriptions[] =
{
    //
    // 480p Descriptions
    //
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YUY2, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YUY2, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YUY2, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV11, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV11, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV11, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV11, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV11, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV11, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV12, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV12, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV12, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV12, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV12, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_NV12, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YV12, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YV12, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YV12, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YV12, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YV12, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_PROGRESSIVE, D3DFMT_YV12, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},

    //
    // 480i Descriptions
    //
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YUY2, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YUY2, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YUY2, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV11, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV11, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV11, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV11, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV11, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV11, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV12, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV12, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV12, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV12, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV12, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_NV12, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YV12, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YV12, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YV12, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YV12, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YV12, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_INTERLACED, D3DFMT_YV12, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},

    //
    // 480i Advanced Descriptions
    //
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YUY2, DXVA2_FREQ_24, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_50, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_NV11, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_NV11, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_NV12, DXVA2_FREQ_24, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_NV12, DXVA2_FREQ_60, DXVA2_FREQ_75, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YV12, DXVA2_FREQ_24, DXVA2_FREQ_60, 0, 0 }},
    {FALSE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YV12, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},

    //
    // Invalid Descriptions
    //
    /*{TRUE, { 0, 0, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {TRUE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_UNKNOWN, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, 0 }},
    {TRUE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YUY2, DXVA2_FREQ_INVALID, DXVA2_FREQ_60, 0, 0 }},
    {TRUE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_INVALID, 0, 0 }},
    {TRUE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_60, DXVA2_UAB_INVALID, 0 }},
    {TRUE, { 640, 480, DXVA2_SAMPLE_FORMAT_ADVANCED, D3DFMT_YUY2, DXVA2_FREQ_60, DXVA2_FREQ_60, 0, DXVA2_RESERVED_INVALID }},*/

};
const UINT DXVA2_VIDEO_DESC_COUNT = (sizeof(VideoDescriptions) / sizeof(CDXVA2_VIDEO_DESC));