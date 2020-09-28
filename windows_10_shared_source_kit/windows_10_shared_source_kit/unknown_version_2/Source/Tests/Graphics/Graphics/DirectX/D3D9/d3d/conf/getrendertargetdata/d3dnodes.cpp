#include "D3DNodes.h"
#include <CSysInfo.h>

CFrameworkTypeMap::CFrameworkTypeMap()
{
    addType( ENUMARG( MULTISAMPLE_NONE ) );
    addType( ENUMARG( MULTISAMPLE_NONMASKABLE ) );
    addType( ENUMARG( MULTISAMPLE_2_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_3_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_4_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_5_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_6_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_7_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_8_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_9_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_10_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_11_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_12_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_13_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_14_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_15_SAMPLES ) );
    addType( ENUMARG( MULTISAMPLE_16_SAMPLES ) );

    addType( ENUMARG( FMT_UNKNOWN ) );
    addType( ENUMARG( FMT_R8G8B8 ) );
    addType( ENUMARG( FMT_A8R8G8B8 ) );
    addType( ENUMARG( FMT_X8R8G8B8 ) );
    addType( ENUMARG( FMT_R5G6B5 ) );
    addType( ENUMARG( FMT_X1R5G5B5 ) );
    addType( ENUMARG( FMT_A1R5G5B5 ) );
    addType( ENUMARG( FMT_A4R4G4B4 ) );
    addType( ENUMARG( FMT_R3G3B2 ) );
    addType( ENUMARG( FMT_A8 ) );
    addType( ENUMARG( FMT_A8R3G3B2 ) );
    addType( ENUMARG( FMT_X4R4G4B4 ) );
    addType( ENUMARG( FMT_A2B10G10R10 ) );
    addType( ENUMARG( FMT_A8B8G8R8 ) );
    addType( ENUMARG( FMT_X8B8G8R8 ) );
    addType( ENUMARG( FMT_G16R16 ) );
    addType( ENUMARG( FMT_A2R10G10B10 ) );
    addType( ENUMARG( FMT_A16B16G16R16 ) );
    addType( ENUMARG( FMT_A8P8 ) );
    addType( ENUMARG( FMT_P8 ) );
    addType( ENUMARG( FMT_P4 ) );
    addType( ENUMARG( FMT_L8 ) );
    addType( ENUMARG( FMT_A8L8 ) );
    addType( ENUMARG( FMT_A4L4 ) );
    addType( ENUMARG( FMT_L16 ) );
    addType( ENUMARG( FMT_V8U8 ) );
    addType( ENUMARG( FMT_L6V5U5 ) );
    addType( ENUMARG( FMT_X8L8V8U8 ) );
    addType( ENUMARG( FMT_Q8W8V8U8 ) );
    addType( ENUMARG( FMT_V16U16 ) );
    addType( ENUMARG( FMT_W11V11U10 ) );
    addType( ENUMARG( FMT_W10V11U11 ) );
    addType( ENUMARG( FMT_A2W10V10U10 ) );
    addType( ENUMARG( FMT_Q16W16V16U16 ) );
    addType( ENUMARG( FMT_UYVY ) );
    addType( ENUMARG( FMT_YUY2 ) );
    addType( ENUMARG( FMT_DXT1 ) );
    addType( ENUMARG( FMT_DXT2 ) );
    addType( ENUMARG( FMT_DXT3 ) );
    addType( ENUMARG( FMT_DXT4 ) );
    addType( ENUMARG( FMT_DXT5 ) );
    addType( ENUMARG( FMT_S1D15 ) );
    addType( ENUMARG( FMT_S8D24 ) );
    addType( ENUMARG( FMT_X8D24 ) );
    addType( ENUMARG( FMT_X4S4D24 ) );
    addType( ENUMARG( FMT_D16_LOCKABLE ) );
    addType( ENUMARG( FMT_D32 ) );
    addType( ENUMARG( FMT_D15S1 ) );
    addType( ENUMARG( FMT_D24S8 ) );
    addType( ENUMARG( FMT_D24X8 ) );
    addType( ENUMARG( FMT_D24X4S4 ) );
    addType( ENUMARG( FMT_D16 ) );
    addType( ENUMARG( FMT_D32F_LOCKABLE ) );
    addType( ENUMARG( FMT_D24FS8 ) );
    addType( ENUMARG( FMT_VERTEXDATA ) );
    addType( ENUMARG( FMT_INDEX16 ) );
    addType( ENUMARG( FMT_INDEX32 ) );
    addType( ENUMARG( FMT_MULTI2_ARGB8 ) );
    addType( ENUMARG( FMT_R16F ) );
    addType( ENUMARG( FMT_G16R16F ) );
    addType( ENUMARG( FMT_A16B16G16R16F ) );
    addType( ENUMARG( FMT_R32F ) );
    addType( ENUMARG( FMT_G32R32F ) );
    addType( ENUMARG( FMT_A32B32G32R32F ) );

    addType( ENUMARG( CUBEMAP_FACE_POSITIVE_X ) );
    addType( ENUMARG( CUBEMAP_FACE_NEGATIVE_X ) );
    addType( ENUMARG( CUBEMAP_FACE_POSITIVE_Y ) );
    addType( ENUMARG( CUBEMAP_FACE_NEGATIVE_Y ) );
    addType( ENUMARG( CUBEMAP_FACE_POSITIVE_Z ) );
    addType( ENUMARG( CUBEMAP_FACE_NEGATIVE_Z ) );

    addType( "POOL", DWORDARG( POOL_DEFAULT ) );
    addType( "POOL", DWORDARG( POOL_MANAGED ) );
    addType( "POOL", DWORDARG( POOL_SYSTEMMEM ) );
    addType( "POOL", DWORDARG( POOL_SCRATCH ) );
    addType( "POOL", DWORDARG( POOL_VIDEO ) );
    addType( "POOL", DWORDARG( POOL_AGP ) );
    addType( "POOL", DWORDARG( POOL_OFFSCREEN ) );

    addBitType( "USAGE", DWORDARG( USAGE_RENDERTARGET ) );
    addBitType( "USAGE", DWORDARG( USAGE_DEPTHSTENCIL ) );
    addBitType( "USAGE", DWORDARG( USAGE_DYNAMIC ) );
    addBitType( "USAGE", DWORDARG( USAGE_AUTOGENMIPMAP ) );
    addBitType( "USAGE", DWORDARG( USAGE_QUERY_LEGACYBUMPMAP ) );
    addBitType( "USAGE", DWORDARG( USAGE_QUERY_SRGBREAD ) );
    addBitType( "USAGE", DWORDARG( USAGE_QUERY_FILTER ) );
    addBitType( "USAGE", DWORDARG( USAGE_QUERY_SRGBWRITE ) );
    addBitType( "USAGE", DWORDARG( USAGE_QUERY_POSTPIXELSHADER_BLENDING ) );
    addBitType( "USAGE", DWORDARG( USAGE_QUERY_VERTEXTEXTURE ) );
    addBitType( "USAGE", DWORDARG( USAGE_WRITEONLY ) );
    addBitType( "USAGE", DWORDARG( USAGE_SOFTWAREPROCESSING ) );
    addBitType( "USAGE", DWORDARG( USAGE_DONOTCLIP ) );
    addBitType( "USAGE", DWORDARG( USAGE_POINTS ) );
    addBitType( "USAGE", DWORDARG( USAGE_RTPATCHES ) );
    addBitType( "USAGE", DWORDARG( USAGE_NPATCHES ) );
}

/*
 *  These (non-public) lists came directly from the framework and used for filtering formats
 */

bool TextureFmts::operator()( CValue *pValue ) const
{
    static FMT g_TextureFormatList9[] = 
    {
        FMT_A8R8G8B8,
        FMT_X8R8G8B8,
        FMT_R5G6B5,
        FMT_X1R5G5B5,
        FMT_A1R5G5B5,
        FMT_A4R4G4B4,
        FMT_R3G3B2,
        FMT_A8,
        FMT_A8R3G3B2,
        FMT_X4R4G4B4,
        FMT_R8G8B8,
        FMT_A2B10G10R10,
        FMT_A8B8G8R8,
        FMT_X8B8G8R8,
        FMT_G16R16,
        FMT_A2R10G10B10,
	    FMT_A16B16G16R16,
	    FMT_R16F,
        FMT_G16R16F,
        FMT_A16B16G16R16F,
        FMT_R32F,
        FMT_G32R32F,
        FMT_A32B32G32R32F,
        FMT_A8P8,
        FMT_P8,
        FMT_L8,
        FMT_A8L8,
        FMT_A4L4,
	    FMT_L16,
        FMT_V8U8,
        FMT_L6V5U5,
        FMT_X8L8V8U8,
        FMT_Q8W8V8U8,
        FMT_V16U16,
        FMT_A2W10V10U10,
	    FMT_Q16W16V16U16,
	    FMT_CxV8U8,
        FMT_UYVY,
	    FMT_R8G8_B8G8,
        FMT_YUY2,
	    FMT_G8R8_G8B8,
        FMT_DXT1,
        FMT_DXT2,
        FMT_DXT3,
        FMT_DXT4,
        FMT_DXT5,
	    FMT_MULTI2_ARGB8
    };
    static DWORD g_dwTextureFormatListSize9 = sizeof(g_TextureFormatList9) / sizeof(FMT);

    FMT fmt;
    return pValue && pValue->get( fmt ) && std::count( g_TextureFormatList9, 
                                                       g_TextureFormatList9 + g_dwTextureFormatListSize9, 
                                                       fmt );
}

bool ZBufferFmts::operator()( CValue *pValue ) const
{
    static FMT g_ZBufferFormatList9[] = 
    {
        FMT_D16,
        FMT_D32,
        FMT_D15S1,
        FMT_D24S8,
        FMT_D24X8,
	    FMT_D24FS8,
	    FMT_D24X4S4,
        FMT_D16_LOCKABLE,
	    FMT_D32F_LOCKABLE
    };
    static DWORD g_dwZBufferFormatListSize9 = sizeof(g_ZBufferFormatList9) / sizeof(FMT);

    FMT fmt;
    return pValue && pValue->get( fmt ) && std::count( g_ZBufferFormatList9,
                                                       g_ZBufferFormatList9 + g_dwZBufferFormatListSize9,
                                                       fmt );
}

bool DisplayModeFmts::operator()( CValue *pValue ) const
{
    static FMT g_DisplayModeFormats9[] =
    {
        FMT_X1R5G5B5,
        FMT_R5G6B5,
        FMT_R8G8B8,
        FMT_X8R8G8B8,
	    FMT_A2R10G10B10
    };
    static DWORD g_dwDisplayModeFormats9 = sizeof(g_DisplayModeFormats9) / sizeof(FMT);

    FMT fmt;
    return pValue && pValue->get( fmt ) && std::count( g_DisplayModeFormats9,
                                                       g_DisplayModeFormats9 + g_dwDisplayModeFormats9,
                                                       fmt );
}

bool BackBufferFmts::operator()( CValue *pValue ) const
{
    static FMT g_BackBufferFormats9[] = {
	    FMT_A2R10G10B10,
	    FMT_A8R8G8B8,
	    FMT_X8R8G8B8,
	    FMT_A1R5G5B5,
	    FMT_X1R5G5B5,
	    FMT_R5G6B5
    };
    static DWORD g_dwBackBufferFormats9 = sizeof(g_BackBufferFormats9) / sizeof(FMT);

    unsigned offset = 0;

#ifdef FAST_TEST
    CSysInfo sysInfo;
    int nOS = sysInfo.GetOSID();
    offset += nOS == 2 || nOS == 5 || nOS == 6 || nOS == 7; // win9x OS => skip 2:10:10:10
#endif

    FMT fmt;
    return pValue && pValue->get( fmt ) && std::count( g_BackBufferFormats9 + offset,
                                                       g_BackBufferFormats9 + g_dwBackBufferFormats9,
                                                       fmt );
}
