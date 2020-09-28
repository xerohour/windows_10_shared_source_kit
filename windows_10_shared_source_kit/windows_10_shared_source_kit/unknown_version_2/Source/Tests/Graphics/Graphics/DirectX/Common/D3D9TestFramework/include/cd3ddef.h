/******************************************************************************
 *                                                                            *
 *  File:      cd3ddef.h                                                         *
 *  Content:   Forward declarations for DirectDraw and Direct3D               *
 *                                                                            *
 ******************************************************************************/
#ifndef __CD3DDEF_h__
#define __CD3DDEF_h__

#ifdef _IA64_   // Removes IA64 compiler alignment warnings
  #pragma pack(16)
#endif

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */


typedef struct IDirectDraw              *LPDIRECTDRAW;
typedef struct IDirectDraw2             *LPDIRECTDRAW2;
typedef struct IDirectDraw4             *LPDIRECTDRAW4;
typedef struct IDirectDraw7             *LPDIRECTDRAW7;
typedef struct IDirectDrawSurface       *LPDIRECTDRAWSURFACE;
typedef struct IDirectDrawSurface2      *LPDIRECTDRAWSURFACE2;
typedef struct IDirectDrawSurface3      *LPDIRECTDRAWSURFACE3;
typedef struct IDirectDrawSurface4      *LPDIRECTDRAWSURFACE4;
typedef struct IDirectDrawSurface7      *LPDIRECTDRAWSURFACE7;
typedef struct IDirectDrawPalette       *LPDIRECTDRAWPALETTE;
typedef struct IDirectDrawClipper       *LPDIRECTDRAWCLIPPER;

typedef struct _DDSURFACEDESC            *LPDDSURFACEDESC;
typedef struct _DDSURFACEDESC2           *LPDDSURFACEDESC2;
typedef struct _DDPIXELFORMAT            *LPDDPIXELFORMAT;
typedef struct _DDCOLORKEY               *LPDDCOLORKEY;

typedef struct _D3DVERTEXBUFFER_DESC   *LPD3DVERTEXBUFFER_DESC;
typedef struct _D3DINDEXBUFFER_DESC    *LPD3DINDEXBUFFER_DESC;
typedef struct _D3DSURFACE_DESC        *LPD3DSURFACE_DESC;
typedef struct _D3DVOLUME_DESC         *LPD3DVOLUME_DESC;
typedef struct _D3DRECT				   *LPD3DRECT;

#if !defined(HMONITOR_DECLARED) && (WINVER < 0x0500)
     #define HMONITOR_DECLARED
     DECLARE_HANDLE(HMONITOR);
#endif

// Stolen defs from Dx9.0c header to avoid dependancy
#ifndef D3DCREATE_DISABLE_DRIVER_MANAGEMENT_EX
#define D3DCREATE_DISABLE_DRIVER_MANAGEMENT_EX  0x00000400L
#endif
#ifndef D3DCREATE_DISABLE_PSGP_THREADING
#define D3DCREATE_DISABLE_PSGP_THREADING        0x00002000L
#endif

// Framework constants for device flags
const DWORD	DEVICECREATE_SWVP			=	0x1;
const DWORD	DEVICECREATE_HWVP			=	0x2;
const DWORD	DEVICECREATE_SW_OR_HW_VP	=	0x4;
const DWORD	DEVICECREATE_HWVP_OR_PURE	=	0x8;
const DWORD	DEVICECREATE_BESTAVAILABLE	=	0x10;
const DWORD	DEVICECREATE_STANDARD		=	0x100;
const DWORD	DEVICECREATE_MIXED			=	0x200;
const DWORD	DEVICECREATE_PURE			=	0x400;
const DWORD	DEVICECREATE_MANAGED		=	0x800;
const DWORD	DEVICECREATE_ALL=	DEVICECREATE_SWVP | DEVICECREATE_HWVP | DEVICECREATE_STANDARD | DEVICECREATE_MIXED | DEVICECREATE_PURE | DEVICECREATE_MANAGED;

const DWORD	DEVICECREATE_MULTITHREADED	=	0x10000;
const DWORD	DEVICECREATE_FPU_PRESERVE	=	0x20000;
const DWORD	DEVICECREATE_DISABLE_DRIVER_MANAGEMENT	=	0x40000;
const DWORD	DEVICECREATE_ADAPTERGROUP_DEVICE	    =	0x80000;
const DWORD DEVICECREATE_DISABLE_DRIVER_MANAGEMENT_EX = 0x100000;
const DWORD DEVICECREATE_DISABLE_PSGP_THREADING = 0x200000;

const DWORD	DEVICETYPE_HAL		=	0x1;
const DWORD	DEVICETYPE_REF		=	0x2;
const DWORD	DEVICETYPE_SW		=	0x4;
const DWORD DEVICETYPE_NULLREF =	0x8;
const DWORD	DEVICETYPE_ALL		=	DEVICETYPE_HAL | DEVICETYPE_REF | DEVICETYPE_SW;

const DWORD CD3D_DEV0			=   0x1;
const DWORD CD3D_DEV1			=   0x2;
const DWORD CD3D_DEV2			=   0x4;
const DWORD CD3D_DEV3			=   0x8;
const DWORD CD3D_DEV4			=   0x10;
const DWORD CD3D_DEV5			=   0x20;
const DWORD CD3D_DEV6			=   0x40;
const DWORD CD3D_DEV7			=   0x80;
const DWORD CD3D_DEV8			=   0x100;
const DWORD CD3D_DEV9			=   0x200;
const DWORD CD3D_DEV10			=   0x400;
const DWORD CD3D_DEV11			=   0x800;
const DWORD CD3D_DEV12			=   0x1000;
const DWORD CD3D_DEV13			=   0x2000;
const DWORD CD3D_DEV14			=   0x4000;
const DWORD CD3D_DEV15			=   0x8000;
const DWORD CD3D_ALL			=	0xFFFF;
const DWORD CD3D_IMAGECMP		=	0x00010000;

#ifndef CD3D_SRC
const DWORD CD3D_SRC			= CD3D_DEV0;
#endif

#ifndef CD3D_REF
const DWORD CD3D_REF			= CD3D_DEV1;
#endif

#ifndef CD3D_BOTH
const DWORD CD3D_BOTH			= CD3D_DEV0 | CD3D_DEV1;
#endif

// Framework defines for Pixel Format flags
#define PF_ALPHAPIXELS                        0x00000001l
#define PF_ALPHA                              0x00000002l
#define PF_FOURCC                             0x00000004l
#define PF_PALETTEINDEXED4                    0x00000008l
#define PF_PALETTEINDEXEDTO8                  0x00000010l
#define PF_PALETTEINDEXED8                    0x00000020l
#define PF_RGB                                0x00000040l
#define PF_COMPRESSED                         0x00000080l
#define PF_RGBTOYUV                           0x00000100l
#define PF_YUV                                0x00000200l
#define PF_ZBUFFER                            0x00000400l
#define PF_PALETTEINDEXED1                    0x00000800l
#define PF_PALETTEINDEXED2                    0x00001000l
#define PF_STENCILBUFFER                      0x00004000l
#define PF_ALPHAPREMULT                       0x00008000l
#define PF_LUMINANCE                          0x00020000l
#define PF_BUMPLUMINANCE                      0x00040000l
#define PF_BUMPDUDV                           0x00080000l
#define PF_LOCKABLE							  0x00100000l		// DX8 lockable
#define PF_UNKNOWNFOURCC					  0x00200000l
#define PF_DXTN								  0x00400000l
#define PF_METN								  0x00800000l
#define PF_DMAP								  0x01000000l
#define PF_FLOATTEX							  0x02000000l

// Group of private pixel format flags
#define PF_PRIVATE							  PF_LOCKABLE | PF_UNKNOWNFOURCC | PF_DXTN | PF_METN | PF_DMAP | PF_FLOATTEX

/*
 * FOURCC codes for DX compressed-texture pixel formats
 */
#define FOURCC_DXT1  (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2  (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3  (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4  (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5  (MAKEFOURCC('D','X','T','5'))

// Framework defines for Texture Filter type
#define TEXF_NONE                           0
#define TEXF_POINT                          1
#define TEXF_LINEAR                         2
#define TEXF_ANISOTROPIC                    3
#define TEXF_FLATCUBIC                      4
#define TEXF_GAUSSIANCUBIC                  5
#define TEXF_PYRAMIDALQUAD					6    // 4-sample tent
#define TEXF_GAUSSIANQUAD					7    // 4-sample gaussian
#define TEXF_CONVOLUTIONMONO				8	 // Convolution filter for monochrome textures

//----------------------------------------------------------------------------
// Framework defines for D3DX_FILTER flags
// ------------------
//
// A valid filter must contain one of these values:
//
//  FILTER_NONE
//      No scaling or filtering will take place.  Pixels outside the bounds
//      of the source image are assumed to be transparent black.
//  FILTER_POINT
//      Each destination pixel is computed by sampling the nearest pixel
//      from the source image.
//  FILTER_LINEAR
//      Each destination pixel is computed by linearly interpolating between
//      the nearest pixels in the source image.  This filter works best 
//      when the scale on each axis is less than 2.
//  FILTER_TRIANGLE
//      Every pixel in the source image contributes equally to the
//      destination image.  This is the slowest of all the filters.
//  FILTER_BOX
//      Each pixel is computed by averaging a 2x2(x2) box pixels from 
//      the source image. Only works when the dimensions of the 
//      destination are half those of the source. (as with mip maps)
//
// And can be OR'd with any of these optional flags:
//
//  FILTER_MIRROR_U
//      Indicates that pixels off the edge of the texture on the U-axis
//      should be mirrored, not wraped.
//  FILTER_MIRROR_V
//      Indicates that pixels off the edge of the texture on the V-axis
//      should be mirrored, not wraped.
//  FILTER_MIRROR_W
//      Indicates that pixels off the edge of the texture on the W-axis
//      should be mirrored, not wraped.
//  FILTER_MIRROR
//      Same as specifying FILTER_MIRROR_U | FILTER_MIRROR_V |
//      FILTER_MIRROR_V
//  FILTER_DITHER
//      Dithers the resulting image using a 4x4 order dither pattern.
//  FILTER_SRGB_IN
//      Denotes that the input data is in sRGB (gamma 2.2) colorspace.
//  FILTER_SRGB_OUT
//      Denotes that the output data is in sRGB (gamma 2.2) colorspace.
//  FILTER_SRGB
//      Same as specifying FILTER_SRGB_IN | FILTER_SRGB_OUT
//
//----------------------------------------------------------------------------
#define FILTER_NONE            (1 << 0)
#define FILTER_POINT           (2 << 0)
#define FILTER_LINEAR          (3 << 0)
#define FILTER_TRIANGLE        (4 << 0)
#define FILTER_BOX             (5 << 0)

#define FILTER_MIRROR_U        (1 << 16)
#define FILTER_MIRROR_V        (2 << 16)
#define FILTER_MIRROR_W        (4 << 16)
#define FILTER_MIRROR          (7 << 16)
#define FILTER_DITHER          (1 << 19)
#define FILTER_SRGB_IN         (1 << 20)
#define FILTER_SRGB_OUT        (2 << 20)
#define FILTER_SRGB            (3 << 20)


// FVF defines
#define TEXTUREFORMATS_SHIFT  16      // Number of bits to shift to get texture format descriptions
#define TEXTUREFORMAT_MASK    0x03    // Mask to extract texture format description
#define TEXTUREFORMAT_SIZE    2       // Number of bits for one texture format description

// Generic Framework flags
const DWORD FMWK_DEFAULT = 0x80000000;
const DWORD FMWK_VSTEXTURE = 0x40000000;	// Use to automatically select correct POOL for vertex shader textures 

// Framework defines for Clear flags
const DWORD CLEARF_TARGET =		1;
const DWORD CLEARF_ZBUFFER = 	2;
const DWORD CLEARF_STENCIL = 	4;
// Framework defines for BackBufferTypes
const DWORD BACKBUFFER_TYPE_MONO   = 1;
const DWORD BACKBUFFER_TYPE_LEFT   = 2;
const DWORD BACKBUFFER_TYPE_RIGHT  = 3;

// Framework defines for memory pool
const DWORD POOL_DEFAULT	= 0x00000000;
const DWORD POOL_MANAGED	= 0x00000001;
const DWORD POOL_SYSTEMMEM 	= 0x00000002;
const DWORD POOL_SCRATCH	= 0x00000003;
const DWORD POOL_VIDEO 		= 0x00000004;
const DWORD POOL_AGP 		= 0x00000008;
const DWORD POOL_OFFSCREEN 	= 0x00000010;
const DWORD POOL_MANAGED_INTERNAL = 0x00000006;
const DWORD POOL_DEFAULT_NOBACKINGSTORE = 0x00000011;

const DWORD SWAPEFFECT_DISCARD		= 0x00000001;
const DWORD SWAPEFFECT_FLIP     	= 0x00000002;
const DWORD SWAPEFFECT_COPY     	= 0x00000003;
const DWORD SWAPEFFECT_COPY_VSYNC   = 0x00000004;
const DWORD SWAPEFFECT_FLIPEX       = 0x00000005;


// Vertex element semantics
typedef enum _DECLUSAGE
{
    DECLUSAGE_POSITION = 0,
    DECLUSAGE_BLENDWEIGHT,   // 1
    DECLUSAGE_BLENDINDICES,  // 2
    DECLUSAGE_NORMAL,        // 3
    DECLUSAGE_PSIZE,         // 4
    DECLUSAGE_TEXCOORD,      // 5
    DECLUSAGE_TANGENT,       // 6
    DECLUSAGE_BINORMAL,      // 7
	DECLUSAGE_TESSFACTOR,    // 8
	DECLUSAGE_POSITIONT,     // 9
	DECLUSAGE_COLOR,         // 10
    DECLUSAGE_FOG,           // 11
    DECLUSAGE_DEPTH,         // 12
	DECLUSAGE_SAMPLE,        // 13
} DECLUSAGE;

#define MAXDECLUSAGE DECLUSAGE_TEXCOORD

typedef enum _DECLMETHOD
{
    DECLMETHOD_DEFAULT = 0,
    DECLMETHOD_PARTIALU,
    DECLMETHOD_PARTIALV,
    DECLMETHOD_CROSSUV,    // Normal
    DECLMETHOD_UV,
    DECLMETHOD_LOOKUP,         		// Lookup a displacement map
    DECLMETHOD_LOOKUPPRESAMPLED    	// Lookup a pre-sampled displacement map
} DECLMETHOD;

#define MAXDECLMETHOD DECLMETHOD_LOOKUP

// Declarations for _Type fields
typedef enum _DECLTYPE
{
    DECLTYPE_FLOAT1    =  0,  // 1D float expanded to (value, 0., 0., 1.)
    DECLTYPE_FLOAT2    =  1,  // 2D float expanded to (value, value, 0., 1.)
    DECLTYPE_FLOAT3    =  2,  // 3D float expanded to (value, value, value, 1.)
    DECLTYPE_FLOAT4    =  3,  // 4D float
    DECLTYPE_D3DCOLOR  =  4,  // 4D packed unsigned bytes mapped to 0. to 1. range
                              // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
    DECLTYPE_UBYTE4    =  5,  // 4D unsigned byte
    DECLTYPE_SHORT2    =  6,  // 2D signed short expanded to (value, value, 0., 1.)
    DECLTYPE_SHORT4    =  7,  // 4D signed short

	// The following types are valid only with vertex shaders >= 2.0

    DECLTYPE_UBYTE4N   =  8,  // Each of 4 bytes is normalized by dividing to 255.0
    DECLTYPE_SHORT2N   =  9,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
    DECLTYPE_SHORT4N   = 10,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
    DECLTYPE_USHORT2N  = 11,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
    DECLTYPE_USHORT4N  = 12,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
    DECLTYPE_UDEC3     = 13,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
    DECLTYPE_DEC3N     = 14,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
    DECLTYPE_FLOAT16_2 = 15,  // Two 16-bit floating point values, expanded to (value, value, 0, 1) 
    DECLTYPE_FLOAT16_4 = 16,  // Four 16-bit floating point values
    DECLTYPE_UNUSED    = 17,  // When the type field in a decl is unused.
} DECLTYPE;

#define MAXDECLTYPE D3DDECLTYPE_UNUSED

// Values for IDirect3DDevice9::SetStreamSourceFreq's Setting parameter
#define STREAMSOURCE_INDEXEDDATA  (1<<30)
#define STREAMSOURCE_INSTANCEDATA (2<<30)

/* Special sampler which is used in the tesselator */
#define DMAPSAMPLER 256

// Samplers used in vertex shaders
#define VERTEXTEXTURESAMPLER0 (D3DDMAPSAMPLER+1) 
#define VERTEXTEXTURESAMPLER1 (D3DDMAPSAMPLER+2) 
#define VERTEXTEXTURESAMPLER2 (D3DDMAPSAMPLER+3) 
#define VERTEXTEXTURESAMPLER3 (D3DDMAPSAMPLER+4) 

/* Usages */
#define USAGE_RENDERTARGET       				(0x00000001L)
#define USAGE_DEPTHSTENCIL       				(0x00000002L)
#define USAGE_AUTOGENMIPMAP      				(0x00000400L)
#define USAGE_DMAP         				        (0x00004000L)


// The following usages are valid only for querying CheckDeviceFormat
#define USAGE_QUERY_LEGACYBUMPMAP				(0x00008000L)
#define USAGE_QUERY_SRGBREAD     				(0x00010000L)
#define USAGE_QUERY_FILTER       				(0x00020000L)
#define USAGE_QUERY_SRGBWRITE    				(0x00040000L)
#define USAGE_QUERY_POSTPIXELSHADER_BLENDING	(0x00080000L)
#define USAGE_QUERY_VERTEXTEXTURE				(0x00100000L) 
#define USAGE_QUERY_WRAPANDMIP		            (0x00200000L)

/* Usages for Vertex/Index buffers */
#define USAGE_WRITEONLY          (0x00000008L)
#define USAGE_SOFTWAREPROCESSING (0x00000010L)
#define USAGE_DONOTCLIP          (0x00000020L)
#define USAGE_POINTS             (0x00000040L)
#define USAGE_RTPATCHES          (0x00000080L)
#define USAGE_NPATCHES           (0x00000100L)
#define USAGE_DYNAMIC            (0x00000200L)

// For convenience this can be used to remove query flags from calls.
// E.g. CreateTexture(..., usage & ~USAGE_QUERY, ...)
#define USAGE_QUERY   (USAGE_QUERY_LEGACYBUMPMAP | USAGE_QUERY_SRGBREAD | USAGE_QUERY_FILTER | USAGE_QUERY_SRGBWRITE | USAGE_QUERY_POSTPIXELSHADER_BLENDING | USAGE_QUERY_VERTEXTEXTURE | USAGE_QUERY_WRAPANDMIP)

// Framework defines for VertexOp for DX7 ProcessVertices
#define VOP_LIGHT       (1 << 10)
#define VOP_TRANSFORM   (1 << 0)
#define VOP_CLIP        (1 << 2)
#define VOP_EXTENTS     (1 << 3)

#define PRESENTFLAG_LOCKABLE_BACKBUFFER  (0x00000001L)
#define PRESENTFLAG_DISCARD_DEPTHSTENCIL (0x00000002L)

#define PRESENT_INTERVAL_DEFAULT     (0x00000000L)
#define PRESENT_INTERVAL_ONE         (0x00000001L)
#define PRESENT_INTERVAL_TWO         (0x00000002L)
#define PRESENT_INTERVAL_THREE       (0x00000004L)
#define PRESENT_INTERVAL_FOUR        (0x00000008L)
#define PRESENT_INTERVAL_IMMEDIATE   (0x80000000L)

#define PPINTERNAL_CACHEBACKBUFFER  (0x00000001L)
#define PPINTERNAL_HINTANTIALIAS    (0x00000002L)
#define PPINTERNAL_DEVICE0		    (0x00000004L)		// Indicated that this is Device0(src)

// Flags for toggling texture loading path
const DWORD TEXLOAD_DEFAULT			= 0x00000000;
const DWORD TEXLOAD_UPDATETEXTURE   = 0x00000001;
const DWORD TEXLOAD_UPDATESURFACE	= 0x00000002;
const DWORD TEXLOAD_COPYRECTS		= 0x00000003;
const DWORD TEXLOAD_LOCKCOPY		= 0x00000004;

const DWORD RESTYPE_SURFACE			=  1;
const DWORD RESTYPE_VOLUME          =  2;
const DWORD RESTYPE_TEXTURE         =  3;
const DWORD RESTYPE_VOLUMETEXTURE   =  4;
const DWORD RESTYPE_CUBETEXTURE     =  5;
const DWORD RESTYPE_VERTEXBUFFER    =  6;
const DWORD RESTYPE_INDEXBUFFER     =  7;

// Lock flags
#define LOCK_READONLY           0x00000010L
#define LOCK_DISCARD            0x00002000L
#define LOCK_NOOVERWRITE        0x00001000L
#define LOCK_NOSYSLOCK          0x00000800L
#define LOCK_NO_DIRTY_UPDATE    0x00008000L

// friendly names for surface, texture, etc management classes
typedef class CnSurface		*LPSURFACES;
typedef class CSurface		*LPSURFACE;
typedef class CnBaseTexture	*LPBASETEXTURES;
typedef class CnTexture		*LPTEXTURES;
typedef class CnCubeTexture	*LPCUBETEXTURES;
typedef class CnVolTexture	*LPVOLUMETEXTURES;
typedef class CnQuery   	*LPQUERYS;
typedef class CBaseTexture  *LPBASETEXTURE;
typedef class CTexture		*LPTEXTURE;
typedef class CVolTexture	*LPVOLUMETEXTURE;
typedef class CCubeTexture  *LPCUBETEXTURE;
typedef class CSwapChain	*LPSWAPCHAIN;
typedef class CnSwapChain	*LPSWAPCHAINS;
typedef class CnVertexBuffer *LPVERTEXBUFFERS;
typedef class CVertexBuffer	*LPVERTEXBUFFER;
typedef class CnIndexBuffer *LPINDEXBUFFERS;
typedef class CIndexBuffer	*LPINDEXBUFFER;
typedef class CDevice		*LPDEVICE;
typedef class CD3D			*LPD3D;
typedef class CVolume       *LPVOLUME;
typedef class CQuery        *LPQUERY;
typedef class CEffect		*LPEFFECT;
typedef class CnEffect		*LPEFFECTS;
typedef class CResource		*LPRESOURCE;

// friendly non-D3D names for DX8+ structs
// for now, we're using the DX8 structs for these types
typedef struct _D3DRASTER_STATUS	RASTER_STATUS;
typedef struct _D3DGAMMARAMP		GAMMARAMP;
typedef struct _D3DRECTPATCH_INFO	RECTPATCH_INFO;
typedef struct _D3DTRIPATCH_INFO	TRIPATCH_INFO;
typedef struct _D3DDEVICE_CREATION_PARAMETERS	DEVICE_CREATION_PARAMETERS;
typedef struct _D3DADAPTER_IDENTIFIER8			ADAPTER_IDENTIFIER;
typedef struct _D3DBOX				D3DBOX;

/*
 * DDSCAPS
 */
typedef struct _DSCAPS
{
    DWORD       dwCaps;         // capabilities of surface wanted
} DSCAPS;

/*
 * DDSCAPS2
 */
typedef struct _DSCAPS2
{
    DWORD       dwCaps;         // capabilities of surface wanted
    DWORD       dwCaps2;
    DWORD       dwCaps3;
    union
    {
        DWORD       dwCaps4;
        DWORD       dwVolumeDepth;
    };
} DSCAPS2;

#define DD_ROP_SPACE            (256/32)        // space required to store ROP array

typedef struct _DCAPS
{
/*  0*/ DWORD   dwSize;                 // size of the DDDRIVERCAPS structure
/*  4*/ DWORD   dwCaps;                 // driver specific capabilities
/*  8*/ DWORD   dwCaps2;                // more driver specific capabilites
/*  c*/ DWORD   dwCKeyCaps;             // color key capabilities of the surface
/* 10*/ DWORD   dwFXCaps;               // driver specific stretching and effects capabilites
/* 14*/ DWORD   dwFXAlphaCaps;          // alpha driver specific capabilities
/* 18*/ DWORD   dwPalCaps;              // palette capabilities
/* 1c*/ DWORD   dwSVCaps;               // stereo vision capabilities
/* 20*/ DWORD   dwAlphaBltConstBitDepths;       // DDBD_2,4,8
/* 24*/ DWORD   dwAlphaBltPixelBitDepths;       // DDBD_1,2,4,8
/* 28*/ DWORD   dwAlphaBltSurfaceBitDepths;     // DDBD_1,2,4,8
/* 2c*/ DWORD   dwAlphaOverlayConstBitDepths;   // DDBD_2,4,8
/* 30*/ DWORD   dwAlphaOverlayPixelBitDepths;   // DDBD_1,2,4,8
/* 34*/ DWORD   dwAlphaOverlaySurfaceBitDepths; // DDBD_1,2,4,8
/* 38*/ DWORD   dwZBufferBitDepths;             // DDBD_8,16,24,32
/* 3c*/ DWORD   dwVidMemTotal;          // total amount of video memory
/* 40*/ DWORD   dwVidMemFree;           // amount of free video memory
/* 44*/ DWORD   dwMaxVisibleOverlays;   // maximum number of visible overlays
/* 48*/ DWORD   dwCurrVisibleOverlays;  // current number of visible overlays
/* 4c*/ DWORD   dwNumFourCCCodes;       // number of four cc codes
/* 50*/ DWORD   dwAlignBoundarySrc;     // source rectangle alignment
/* 54*/ DWORD   dwAlignSizeSrc;         // source rectangle byte size
/* 58*/ DWORD   dwAlignBoundaryDest;    // dest rectangle alignment
/* 5c*/ DWORD   dwAlignSizeDest;        // dest rectangle byte size
/* 60*/ DWORD   dwAlignStrideAlign;     // stride alignment
/* 64*/ DWORD   dwRops[DD_ROP_SPACE];   // ROPS supported
/* 84*/ DSCAPS  ddsOldCaps;             // Was DDSCAPS  ddsCaps. ddsCaps is of type DDSCAPS2 for DX6
/* 88*/ DWORD   dwMinOverlayStretch;    // minimum overlay stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
/* 8c*/ DWORD   dwMaxOverlayStretch;    // maximum overlay stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
/* 90*/ DWORD   dwMinLiveVideoStretch;  // minimum live video stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
/* 94*/ DWORD   dwMaxLiveVideoStretch;  // maximum live video stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
/* 98*/ DWORD   dwMinHwCodecStretch;    // minimum hardware codec stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
/* 9c*/ DWORD   dwMaxHwCodecStretch;    // maximum hardware codec stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
/* a0*/ DWORD   dwReserved1;            // reserved
/* a4*/ DWORD   dwReserved2;            // reserved
/* a8*/ DWORD   dwReserved3;            // reserved
/* ac*/ DWORD   dwSVBCaps;              // driver specific capabilities for System->Vmem blts
/* b0*/ DWORD   dwSVBCKeyCaps;          // driver color key capabilities for System->Vmem blts
/* b4*/ DWORD   dwSVBFXCaps;            // driver FX capabilities for System->Vmem blts
/* b8*/ DWORD   dwSVBRops[DD_ROP_SPACE];// ROPS supported for System->Vmem blts
/* d8*/ DWORD   dwVSBCaps;              // driver specific capabilities for Vmem->System blts
/* dc*/ DWORD   dwVSBCKeyCaps;          // driver color key capabilities for Vmem->System blts
/* e0*/ DWORD   dwVSBFXCaps;            // driver FX capabilities for Vmem->System blts
/* e4*/ DWORD   dwVSBRops[DD_ROP_SPACE];// ROPS supported for Vmem->System blts
/*104*/ DWORD   dwSSBCaps;              // driver specific capabilities for System->System blts
/*108*/ DWORD   dwSSBCKeyCaps;          // driver color key capabilities for System->System blts
/*10c*/ DWORD   dwSSBFXCaps;            // driver FX capabilities for System->System blts
/*110*/ DWORD   dwSSBRops[DD_ROP_SPACE];// ROPS supported for System->System blts
/*130*/ DWORD   dwMaxVideoPorts;        // maximum number of usable video ports
/*134*/ DWORD   dwCurrVideoPorts;       // current number of video ports used
/*138*/ DWORD   dwSVBCaps2;             // more driver specific capabilities for System->Vmem blts
/*13c*/ DWORD   dwNLVBCaps;               // driver specific capabilities for non-local->local vidmem blts
/*140*/ DWORD   dwNLVBCaps2;              // more driver specific capabilities non-local->local vidmem blts
/*144*/ DWORD   dwNLVBCKeyCaps;           // driver color key capabilities for non-local->local vidmem blts
/*148*/ DWORD   dwNLVBFXCaps;             // driver FX capabilities for non-local->local blts
/*14c*/ DWORD   dwNLVBRops[DD_ROP_SPACE]; // ROPS supported for non-local->local blts
// Members added for DX6 release
/*16c*/ DSCAPS2  ddsCaps;               // Surface Caps
} DCAPS;



typedef float D3DVALUE, *LPD3DVALUE;


typedef struct IDirect3D            *LPDIRECT3D;
typedef struct IDirect3DDevice      *LPDIRECT3DDEVICE;
typedef struct IDirect3DExecuteBuffer   *LPDIRECT3DEXECUTEBUFFER;
typedef struct IDirect3DLight       *LPDIRECT3DLIGHT;
typedef struct IDirect3DMaterial    *LPDIRECT3DMATERIAL;
typedef struct IDirect3DTexture     *LPDIRECT3DTEXTURE;
typedef struct IDirect3DViewport    *LPDIRECT3DVIEWPORT;

typedef struct IDirect3D2           *LPDIRECT3D2;
typedef struct IDirect3DDevice2     *LPDIRECT3DDEVICE2;
typedef struct IDirect3DMaterial2   *LPDIRECT3DMATERIAL2;
typedef struct IDirect3DTexture2    *LPDIRECT3DTEXTURE2;
typedef struct IDirect3DViewport2   *LPDIRECT3DVIEWPORT2;

typedef struct IDirect3D3            *LPDIRECT3D3;
typedef struct IDirect3DDevice3      *LPDIRECT3DDEVICE3;
typedef struct IDirect3DMaterial3    *LPDIRECT3DMATERIAL3;
typedef struct IDirect3DViewport3    *LPDIRECT3DVIEWPORT3;
typedef struct IDirect3DVertexBuffer *LPDIRECT3DVERTEXBUFFER;

typedef struct IDirect3D7               *LPDIRECT3D7;
typedef struct IDirect3DDevice7         *LPDIRECT3DDEVICE7;
typedef struct IDirect3DVertexBuffer7   *LPDIRECT3DVERTEXBUFFER7;

typedef struct IDirect3D8                *LPDIRECT3D8;
typedef struct IDirect3DDevice8          *LPDIRECT3DDEVICE8;
typedef struct IDirect3DSurface8         *LPDIRECT3DSURFACE8;
typedef struct IDirect3DVolume8          *LPDIRECT3DVOLUME8;
typedef struct IDirect3DSwapChain8       *LPDIRECT3DSWAPCHAIN8;
typedef struct IDirect3DVertexBuffer8    *LPDIRECT3DVERTEXBUFFER8;
typedef struct IDirect3DIndexBuffer8     *LPDIRECT3DINDEXBUFFER8;
typedef struct IDirect3DResource8        *LPDIRECT3DRESOURCE8;
typedef struct IDirect3DBaseTexture8     *LPDIRECT3DBASETEXTURE8;
typedef struct IDirect3DTexture8         *LPDIRECT3DTEXTURE8;
typedef struct IDirect3DVolumeTexture8   *LPDIRECT3DVOLUMETEXTURE8;
typedef struct IDirect3DCubeTexture8     *LPDIRECT3DCUBETEXTURE8;

typedef struct IDirect3D9					*LPDIRECT3D9;
typedef struct IDirect3D9Ex					*LPDIRECT3D9EX;
typedef struct IDirect3DDevice9				*LPDIRECT3DDEVICE9;
typedef struct IDirect3DDevice9Ex			*LPDIRECT3DDEVICE9EX;
typedef struct IDirect3DSurface9			*LPDIRECT3DSURFACE9;
typedef struct IDirect3DVolume9				*LPDIRECT3DVOLUME9;
typedef struct IDirect3DSwapChain9       	*LPDIRECT3DSWAPCHAIN9;
typedef struct IDirect3DVertexBuffer9    	*LPDIRECT3DVERTEXBUFFER9;
typedef struct IDirect3DIndexBuffer9     	*LPDIRECT3DINDEXBUFFER9;
typedef struct IDirect3DResource9        	*LPDIRECT3DRESOURCE9;
typedef struct IDirect3DBaseTexture9     	*LPDIRECT3DBASETEXTURE9;
typedef struct IDirect3DTexture9         	*LPDIRECT3DTEXTURE9;
typedef struct IDirect3DVolumeTexture9   	*LPDIRECT3DVOLUMETEXTURE9;
typedef struct IDirect3DCubeTexture9     	*LPDIRECT3DCUBETEXTURE9;
typedef struct IDirect3DStateBlock9      	*LPDIRECT3DSTATEBLOCK9;
typedef struct IDirect3DVertexDeclaration9  *LPDIRECT3DVERTEXDECLARATION9;
typedef struct IDirect3DVertexShader9       *LPDIRECT3DVERTEXSHADER9;
typedef struct IDirect3DPixelShader9        *LPDIRECT3DPIXELSHADER9;
typedef struct IDirect3DQuery9				*LPDIRECT3DQUERY9;

typedef LPCSTR D3DXHANDLE;
typedef struct _D3DXCONSTANTTABLE_DESC D3DXCONSTANTTABLE_DESC;
typedef struct _D3DXCONSTANT_DESC      D3DXCONSTANT_DESC;
typedef struct _D3DXPARAMETER_DESC     D3DXPARAMETER_DESC;
typedef struct _D3DXFONT_DESCA		   D3DXFONT_DESCA, *LPD3DXFONT_DESCA;
typedef struct _D3DXFONT_DESCW		   D3DXFONT_DESCW, *LPD3DXFONT_DESCW;
//typedef struct _D3DXPLAYBACK_TYPE		D3DXPLAYBACK_TYPE, *LPD3DXPLAYBACK_TYPE;
typedef struct _D3DXKEY_CALLBACK		D3DXKEY_CALLBACK, *LPD3DXKEY_CALLBACK;


typedef struct ID3DXConstantTable       *LPD3DXCONSTANTTABLE;
typedef struct ID3DXFragmentLinker     *LPD3DXFRAGMENTLINKER;
typedef struct ID3DXFont				*LPD3DXFONT;
typedef struct ID3DXBaseMesh			*LPD3DXBASEMESH;
typedef struct ID3DXMesh				*LPD3DXMESH;
typedef struct ID3DXPatchMesh				*LPD3DXPATCHMESH;
typedef struct ID3DXSkinMesh			*LPD3DXSKINMESH;
typedef struct ID3DXSkinInfo			*LPD3DXSKININFO;
typedef struct ID3DXPMesh				*LPD3DXPMESH;
typedef struct ID3DXSPMesh				*LPD3DXSPMESH;
typedef struct ID3DXBuffer				*LPD3DXBUFFER;
typedef struct ID3DXEffect				*LPD3DXEFFECT;
typedef struct ID3DXEffectCompiler		*LPD3DXEFFECTCOMPILER;
typedef struct ID3DXRenderToSurface		*LPD3DXRENDERTOSURFACE;
typedef struct ID3DXRenderToEnvMap		*LPD3DXRenderToEnvMap;
typedef struct ID3DXSprite				*LPD3DXSPRITE;
typedef struct ID3DXLine				*LPD3DXLINE;
typedef struct _D3DXIMAGE_INFO			D3DXIMAGE_INFO, *LPD3DXIMAGE_INFO;
typedef struct IDirectXFileData			*LPDIRECTXFILEDATA;
typedef struct _D3DXATTRIBUTEWEIGHTS	*LPD3DXATTRIBUTEWEIGHTS;
typedef struct _D3DXATTRIBUTERANGE		*LPD3DXATTRIBUTERANGE;
typedef struct _D3DXEFFECTINSTANCE		D3DXEFFECTINSTANCE, *LPD3DXEFFECTINSTANCE;
//typedef struct _D3DXMATERIAL			D3DXMATERIAL;
//typedef struct D3DXMATERIAL				*LPD3DXMATERIAL;
typedef struct _D3DXKEY_VECTOR3			D3DXKEY_VECTOR3, *LPD3DXKEY_VECTOR3;
typedef struct _D3DXKEY_QUATERNION		D3DXKEY_QUATERNION, *LPD3DXKEY_QUATERNION;
typedef struct _D3DXPATCHINFO                                   D3DXPATCHINFO, *LPD3DXPATCHINFO;
typedef struct ID3DXInterpolator		*LPD3DXINTERPOLATOR;
//typedef struct ID3DXKeyframedAnimationSet *LPD3DXKEYFRAMEDANIMATIONSET;
typedef struct ID3DXKeyframedAnimationSet *LPD3DXKEYFRAMEDANIMATIONSET;
typedef struct ID3DXAnimationSet		*LPD3DXANIMATIONSET;
typedef struct ID3DXAnimationMixer		*LPD3DXANIMATIONMIXER;
typedef struct ID3DXAnimationController *LPD3DXANIMATIONCONTROLLER;
typedef struct ID3DXCompressedAnimationSet *LPD3DXCOMPRESSEDANIMATIONSET;
typedef struct ID3DXAllocateHierarchy	*LPD3DXALLOCATEHIERARCHY;
typedef struct ID3DXLoadUserData		*LPD3DXLOADUSERDATA;
typedef struct ID3DXSaveUserData		*LPD3DXSAVEUSERDATA;
typedef struct _D3DXMESHCONTAINER		D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;
typedef struct _D3DXFRAME				D3DXFRAME, *LPD3DXFRAME;
typedef struct _D3DXSHADER_DEBUGINFO       D3DXSHADER_DEBUGINFO, *LPD3DXSHADER_DEBUGINFO;
typedef struct _D3DXSHADER_INSTRUCTIONINFO D3DXSHADER_INSTRUCTIONINFO, *LPD3DXSHADER_INSTRUCTIONINFO;
typedef struct _D3DXSHADER_CONSTANTTABLE   D3DXSHADER_CONSTANTTABLE, *LPD3DXSHADER_CONSTANTTABLE;
typedef struct _D3DXSHADER_CONSTANTINFO    D3DXSHADER_CONSTANTINFO, *LPD3DXSHADER_CONSTANTINFO;
typedef struct _D3DXRTS_DESC			D3DXRTS_DESC, *LPD3DXRTS_DESC;
typedef struct _D3DXRTE_DESC			D3DXRTE_DESC, *LPD3DXRTE_DESC;
typedef struct _D3DXMACRO				D3DXMACRO, *LPD3DXMACRO;


typedef struct _D3DDeviceDesc           *LPD3DDEVICEDESC;
typedef struct _D3DDeviceDesc7          *LPD3DDEVICEDESC7;
typedef struct _D3DLIGHT2               *LPD3DLIGHT2;
typedef struct _D3DLIGHT7               *LPD3DLIGHT7;
typedef struct _D3DLIGHT8               D3DLIGHT8, *LPD3DLIGHT8;
typedef struct _D3DLIGHT9               D3DLIGHT9, *LPD3DLIGHT9;
typedef struct _D3DMATERIAL             *LPD3DMATERIAL;
typedef struct _D3DMATERIAL7            D3DMATERIAL7,*LPD3DMATERIAL7;
typedef struct _D3DMATERIAL8            D3DMATERIAL8,*LPD3DMATERIAL8;
typedef struct _D3DMATERIAL9            D3DMATERIAL9,*LPD3DMATERIAL9;
typedef struct _D3DDRAWPRIMITIVESTRIDEDDATA *LPD3DDRAWPRIMITIVESTRIDEDDATA;
typedef struct _D3DMATRIX               D3DMATRIX, *LPD3DMATRIX;
typedef struct _D3DVECTOR               D3DVECTOR, *LPD3DVECTOR;
typedef struct _D3DVERTEX               D3DVERTEX, *LPD3DVERTEX; 
typedef struct _D3DLVERTEX              D3DLVERTEX, *LPD3DLVERTEX; 
typedef struct _D3DHVERTEX              D3DHVERTEX, *LPD3DHVERTEX; 
typedef struct _D3DTLVERTEX             D3DTLVERTEX, *LPD3DTLVERTEX; 
typedef struct _D3DVERTEXELEMENT9      D3DVERTEXELEMENT9, *LPD3DVERTEXELEMENT9;
typedef struct _D3DCLIPSTATUS           D3DCLIPSTATUS, *LPD3DCLIPSTATUS;
typedef struct _D3DRECTPATCH_INFO		D3DRECTPATCH_INFO, *LPD3DRECTPATCH_INFO;
typedef struct _D3DTRIPATCH_INFO		D3DTRIPATCH_INFO, *LPD3DTRIPATCH_INFO;
typedef struct _D3DPRESENT_PARAMETERS_	D3DPRESENT_PARAMETERS;
struct D3DDISPLAYMODEEX;
struct D3DDISPLAYMODEFILTER;
typedef struct _D3DCLIPSTATUS8			D3DCLIPSTATUS8;
typedef struct _D3DCLIPSTATUS9			D3DCLIPSTATUS9;
typedef struct _D3DVIEWPORT7			D3DVIEWPORT7, *LPD3DVIEWPORT7;
typedef struct _D3DVIEWPORT8			D3DVIEWPORT8;
typedef struct _D3DVIEWPORT9			D3DVIEWPORT9;
typedef struct _D3DCAPS8				D3DCAPS8;
typedef struct _D3DCAPS9				D3DCAPS9;
typedef struct _D3DRECT					D3DRECT;
typedef struct _D3DDISPLAYMODE			D3DDISPLAYMODE;
typedef struct _D3DGAMMARAMP			D3DGAMMARAMP;
typedef struct _D3DDEVICE_CREATION_PARAMETERS D3DDEVICE_CREATION_PARAMETERS;
typedef struct _D3DRASTER_STATUS		D3DRASTER_STATUS;
typedef struct ID3DXMatrixStack *LPD3DXMATRIXSTACK;
typedef struct _D3DXWELDEPSILONS		*LPD3DXWELDEPSILONS;
typedef DWORD D3DCOLOR, *LPD3DCOLOR;
struct D3DXFLOAT16;

struct D3DXVECTOR2;
struct D3DXVECTOR3;
struct D3DXVECTOR4;
struct D3DXMATRIX;
struct D3DXPLANE;
struct D3DXCOLOR;
struct D3DXQUATERNION;
typedef struct _D3DXMACRO				D3DXMACRO;
typedef struct ID3DXInclude				*LPD3DXINCLUDE;
typedef struct ID3DXEffectPool			*LPD3DXEFFECTPOOL;
struct IStream;

typedef enum _D3DXWELDEPSILONSFLAGS		D3DXWELDEPSILONSFLAGS;
typedef enum _D3DFOGMODE                D3DFOGMODE;
typedef enum _D3DLIGHTSTATETYPE         D3DLIGHTSTATETYPE;
typedef enum _D3DRENDERSTATETYPE        D3DRENDERSTATETYPE;
typedef enum _D3DTEXTURESTAGESTATETYPE  D3DTEXTURESTAGESTATETYPE;
typedef enum _D3DSAMPLERSTATETYPE  		D3DSAMPLERSTATETYPE;
typedef enum _D3DTRANSFORMSTATETYPE     D3DTRANSFORMSTATETYPE;
typedef enum _D3DSTATEBLOCKTYPE         D3DSTATEBLOCKTYPE;
typedef enum _D3DPRIMITIVETYPE          D3DPRIMITIVETYPE;
typedef enum _D3DLIGHTTYPE              D3DLIGHTTYPE;
typedef enum _D3DDEVTYPE                D3DDEVTYPE;
typedef enum _D3DFORMAT                 D3DFORMAT;
typedef enum _D3DMULTISAMPLE_TYPE		D3DMULTISAMPLE_TYPE;
typedef enum _D3DSWAPEFFECT				D3DSWAPEFFECT;
typedef enum _D3DPOOL					D3DPOOL;
typedef enum _D3DUSAGE					D3DUSAGE;
typedef enum _D3DBACKBUFFER_TYPE		D3DBACKBUFFER_TYPE;
typedef enum _D3DXIMAGE_FILEFORMAT		D3DXIMAGE_FILEFORMAT;
typedef enum _D3DXPLAYBACK_TYPE			D3DXPLAYBACK_TYPE;

typedef enum _D3DRESOURCETYPE			D3DRESOURCETYPE;
typedef enum _D3DCUBEMAP_FACES			D3DCUBEMAP_FACES;


// --------------------------------------------------------------------------------------
// Asynchronous Notifications - tied directly to Dx9 versions

typedef DWORD QUERYID;
typedef enum _QUERYTYPE
{
    QUERYTYPE_VCACHE                 = 4,
	QUERYTYPE_RESOURCEMANAGER        = 5,
	QUERYTYPE_VERTEXSTATS            = 6,
	QUERYTYPE_EVENT                  = 8,
    QUERYTYPE_OCCLUSION              = 9,
    QUERYTYPE_TIMESTAMP              = 10, /* D3DISSUE_END */
    QUERYTYPE_TIMESTAMPDISJOINT      = 11, /* D3DISSUE_BEGIN, D3DISSUE_END */
    QUERYTYPE_TIMESTAMPFREQ          = 12, /* D3DISSUE_END */
    QUERYTYPE_PIPELINETIMINGS        = 13, /* D3DISSUE_BEGIN, D3DISSUE_END */
    QUERYTYPE_INTERFACETIMINGS       = 14, /* D3DISSUE_BEGIN, D3DISSUE_END */
    QUERYTYPE_VERTEXTIMINGS          = 15, /* D3DISSUE_BEGIN, D3DISSUE_END */
    QUERYTYPE_PIXELTIMINGS           = 16, /* D3DISSUE_BEGIN, D3DISSUE_END */
    QUERYTYPE_BANDWIDTHTIMINGS       = 17, /* D3DISSUE_BEGIN, D3DISSUE_END */
    QUERYTYPE_CACHEUTILIZATION       = 18, /* D3DISSUE_BEGIN, D3DISSUE_END */
} QUERYTYPE; 

typedef struct _D3DDEVINFO_VCACHE DEVINFO_VCACHE, *LPDEVINFO_VCACHE;
typedef struct _D3DDEVINFO_RESOURCEMANAGER DEVINFO_RESOURCEMANAGER, *LPDEVINFO_RESOURCEMANAGER;
typedef struct _D3DDEVINFO_D3DVERTEXSTATS  DEVINFO_D3DVERTEXSTATS,  *LPDEVINFO_D3DVERTEXSTATS;
typedef struct _D3DDEVINFO_D3D9PIPELINETIMINGS DEVINFO_D3D9PIPELINETIMINGS;
typedef struct _D3DDEVINFO_D3D9INTERFACETIMINGS DEVINFO_D3D9INTERFACETIMINGS;
typedef struct _D3DDEVINFO_D3D9STAGETIMINGS DEVINFO_D3D9STAGETIMINGS;
typedef struct _D3DDEVINFO_D3D9BANDWIDTHTIMINGS DEVINFO_D3D9BANDWIDTHTIMINGS;
typedef struct _D3DDEVINFO_D3D9CACHEUTILIZATION DEVINFO_D3D9CACHEUTILIZATION;

// Flags field for Issue
#define ISSUE_END       (1 << 0) // Tells the runtime to issue the end of a query, changing it's state to "non-signaled". 
#define ISSUE_BEGIN     (1 << 1) // Tells the runtime to issue the beginng of a query.

// Flags field for GetData
#define GETDATA_FLUSH   (1 << 0) // Tells the runtime to flush if the query is outstanding. 


// --------------------------------------------------------------------------------------
// PIXELFORMAT - direct copy of the Dx7 version

typedef struct _PIXELFORMAT
{
    DWORD       dwSize;                 // size of structure
    DWORD       dwFlags;                // pixel format flags
    DWORD       dwFourCC;               // (FOURCC code)
    union
    {
        DWORD   dwRGBBitCount;          // how many bits per pixel
        DWORD   dwYUVBitCount;          // how many bits per pixel
        DWORD   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
        DWORD   dwAlphaBitDepth;        // how many bits for alpha channels
        DWORD   dwLuminanceBitCount;    // how many bits per pixel
        DWORD   dwBumpBitCount;         // how many bits per "buxel", total
    };
    union
    {
        DWORD   dwRBitMask;             // mask for red bit
        DWORD   dwYBitMask;             // mask for Y bits
        DWORD   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
        DWORD   dwLuminanceBitMask;     // mask for luminance bits
        DWORD   dwBumpDuBitMask;        // mask for bump map U delta bits
    };
    union
    {
        DWORD   dwGBitMask;             // mask for green bits
        DWORD   dwUBitMask;             // mask for U bits
        DWORD   dwZBitMask;             // mask for Z bits
        DWORD   dwBumpDvBitMask;        // mask for bump map V delta bits
    };
    union
    {
        DWORD   dwBBitMask;             // mask for blue bits
        DWORD   dwVBitMask;             // mask for V bits
        DWORD   dwStencilBitMask;       // mask for stencil bits
        DWORD   dwBumpLuminanceBitMask; // mask for luminance in bump map
    };
    union
    {
        DWORD   dwRGBAlphaBitMask;      // mask for alpha channel
        DWORD   dwYUVAlphaBitMask;      // mask for alpha channel
        DWORD   dwLuminanceAlphaBitMask;// mask for alpha channel
        DWORD   dwRGBZBitMask;          // mask for Z channel
        DWORD   dwYUVZBitMask;          // mask for Z channel
    };
	DWORD   dwRowsPerByteSequence;		// The surface memory comprises sequences of consecutive
										// bytes, with the beginning of each sequence separated
										// from the next by a number of bytes equal to the
										// surface pitch.  This member denotes the number of
										// rows spanned by the texels corresponding to one
										// byte sequence.  It's value is 4 for DXTn formats, 1
										// for all other formats.
} PIXELFORMAT, *LPPIXELFORMAT;

// --------------------------------------------------------------------------------------
// DDSCAPS2 - direct copy of the Dx7 version

typedef struct _SCAPS2
{
    DWORD       dwCaps;         // capabilities of surface wanted
    DWORD       dwCaps2;
    DWORD       dwCaps3;
    DWORD       dwCaps4;
} SCAPS2;

// --------------------------------------------------------------------------------------
// DDCOLORKEY - direct copy of the Dx7 version

typedef struct _COLORKEY
{
    DWORD       dwColorSpaceLowValue;   // low boundary of color space that is to
                                        // be treated as Color Key, inclusive
    DWORD       dwColorSpaceHighValue;  // high boundary of color space that is
                                        // to be treated as Color Key, inclusive
} COLORKEY;

// --------------------------------------------------------------------------------------
// RGBSIZE - defines a 24 bit RGB value... 3 bytes.

typedef struct
{
    BYTE    Red;
    BYTE    Green;
    BYTE    Blue;
} RGBSIZE, *PRGBSIZE;

// --------------------------------------------------------------------------------------
// FORMATTYPE - define the different types of formats

enum FORMATTYPE
{
	FORMAT_TEXTURE,
	FORMAT_ZBUFFER,
	FORMAT_MULTISAMPLE
};

// --------------------------------------------------------------------------------------
// FMT - version independant initializer that can be passed into a FORMAT constructor
enum FMT
{
    FMT_UNKNOWN              =  0,
    
    FMT_R8G8B8               = 20,
    FMT_A8R8G8B8             = 21,
    FMT_X8R8G8B8             = 22,
    FMT_R5G6B5               = 23,
    FMT_X1R5G5B5             = 24,
    FMT_A1R5G5B5             = 25,
    FMT_A4R4G4B4             = 26,
    FMT_R3G3B2               = 27,
    FMT_A8                   = 28,
    FMT_A8R3G3B2             = 29,
    FMT_X4R4G4B4             = 30,
    FMT_A2B10G10R10          = 31,
    FMT_A8B8G8R8             = 32,
    FMT_X8B8G8R8             = 33,
    FMT_G16R16               = 34,
    FMT_A2R10G10B10          = 35,
	FMT_A16B16G16R16		 = 36,

	FMT_A8P8                 = 40,
    FMT_P8                   = 41,

	FMT_P4					 = 49,		// 4 bit palettized format is added for DX6/DX7 compatibility
    
	FMT_L8                   = 50,
    FMT_A8L8                 = 51,
    FMT_A4L4                 = 52,
    FMT_L16                  = 81,
    
	FMT_V8U8                 = 60,
    FMT_L6V5U5               = 61,
    FMT_X8L8V8U8             = 62,
    FMT_Q8W8V8U8             = 63,
    FMT_V16U16               = 64,
    FMT_W11V11U10            = 65,
    FMT_W10V11U11            = 66,
    FMT_A2W10V10U10          = 67,
	
    FMT_Q16W16V16U16         =110,
    
    FMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    FMT_R8G8_B8G8            = MAKEFOURCC('R', 'G', 'B', 'G'),
    FMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
    FMT_G8R8_G8B8            = MAKEFOURCC('G', 'R', 'G', 'B'),
    FMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
    FMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
    FMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
    FMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
    FMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

    FMT_S1D15                = 72,		// These are for downlevel DDI formats
    FMT_S8D24                = 74,
    FMT_X8D24                = 76,
    FMT_X4S4D24              = 78,

	FMT_D16_LOCKABLE         = 70,
    FMT_D32                  = 71,
    FMT_D15S1                = 73,
    FMT_D24S8                = 75,
    FMT_D24X8                = 77,
    FMT_D24X4S4              = 79,
    FMT_D16                  = 80,
    FMT_D32F_LOCKABLE        = 82,
    FMT_D24FS8               = 83,
    FMT_D32_LOCKABLE         = 84,
    FMT_S8_LOCKABLE          = 85,

    FMT_VERTEXDATA           =100,
    FMT_INDEX16              =101,
    FMT_INDEX32              =102,

    FMT_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),
	
	FMT_R16F                 = 111,
    FMT_G16R16F              = 112,
    FMT_A16B16G16R16F        = 113,
    
    // IEEE s23e8 formats (32-bits per channel) 
    FMT_R32F                 = 114,
    FMT_G32R32F              = 115,
    FMT_A32B32G32R32F        = 116,

	FMT_CxV8U8               = 117,

	// Monochrome 1 bit per pixel format
	FMT_A1					 = 118,

	FMT_A2B10G10R10_XR_BIAS	 = 119,

	FMT_FORCE_DWORD			 =0x7FFFFFFF,

    FMT_ANY					 =0xFFFFFFFE,
	FMT_ALL					 =0xFFFFFFFF
};

typedef enum _MULTISAMPLE_TYPE
{
    MULTISAMPLE_NONE            =  0,
    MULTISAMPLE_NONMASKABLE     =  1,
    MULTISAMPLE_2_SAMPLES       =  2,
    MULTISAMPLE_3_SAMPLES       =  3,
    MULTISAMPLE_4_SAMPLES       =  4,
    MULTISAMPLE_5_SAMPLES       =  5,
    MULTISAMPLE_6_SAMPLES       =  6,
    MULTISAMPLE_7_SAMPLES       =  7,
    MULTISAMPLE_8_SAMPLES       =  8,
    MULTISAMPLE_9_SAMPLES       =  9,
    MULTISAMPLE_10_SAMPLES      = 10,
    MULTISAMPLE_11_SAMPLES      = 11,
    MULTISAMPLE_12_SAMPLES      = 12,
    MULTISAMPLE_13_SAMPLES      = 13,
    MULTISAMPLE_14_SAMPLES      = 14,
    MULTISAMPLE_15_SAMPLES      = 15,
    MULTISAMPLE_16_SAMPLES      = 16,
    MULTISAMPLE_ANY      		= 0xFFFFFFFE,
    MULTISAMPLE_ALL      		= 0xFFFFFFFF
} MULTISAMPLE_TYPE;

typedef enum _SAMPLERSTATETYPE
{
    SAMP_ADDRESSU               = 1,         // D3DTEXTUREADDRESS for U coordinate
    SAMP_ADDRESSV               = 2,         // D3DTEXTUREADDRESS for V coordinate
    SAMP_ADDRESSW               = 3,         // D3DTEXTUREADDRESS for W coordinate
    SAMP_BORDERCOLOR            = 4,         // D3DCOLOR
    SAMP_MAGFILTER              = 5,         // D3DTEXTUREFILTER filter to use for magnification
    SAMP_MINFILTER              = 6,         // D3DTEXTUREFILTER filter to use for minification
    SAMP_MIPFILTER              = 7,         // D3DTEXTUREFILTER filter to use between mipmaps during minification
    SAMP_MIPMAPLODBIAS          = 8,         // float Mipmap LOD bias
    SAMP_MAXMIPLEVEL            = 9,         // DWORD 0..(n-1) LOD index of largest map to use (0 == largest)
    SAMP_MAXANISOTROPY          = 10,        // DWORD maximum anisotropy
    SAMP_SRGBTEXTURE            = 11,        // default = 0 (Gamma 1.0, no correction required) else correct for Gamma = 2.2
    SAMP_ELEMENTINDEX			= 12,		 // Default = 0. When multi-element texture is assigned to sampler, this indicates which element index to use. 
    SAMP_DMAPOFFSET				= 13,		 // Offset in vertices in the pre-sampled displacement map. Only valid for D3DDMAPSAMPLER sampler.
    SAMP_FORCE_DWORD            = 0x7fffffff // force 32-bit size enum
} SAMPLERSTATETYPE; // state enumerants for per-sampler texture processing

typedef enum _RESOURCETYPE
{
    RTYPE_SURFACE                =  1,
    RTYPE_VOLUME                 =  2,
    RTYPE_TEXTURE                =  3,
    RTYPE_VOLUMETEXTURE          =  4,
    RTYPE_CUBETEXTURE            =  5,
    RTYPE_VERTEXBUFFER           =  6,
    RTYPE_INDEXBUFFER            =  7,
    RTYPE_FORCE_DWORD            = 0x7fffffff
} RESOURCETYPE;

typedef enum _CUBEMAP_FACES
{
    CUBEMAP_FACE_POSITIVE_X     = 0,
    CUBEMAP_FACE_NEGATIVE_X     = 1,
    CUBEMAP_FACE_POSITIVE_Y     = 2,
    CUBEMAP_FACE_NEGATIVE_Y     = 3,
    CUBEMAP_FACE_POSITIVE_Z     = 4,
    CUBEMAP_FACE_NEGATIVE_Z     = 5,

    CUBEMAP_FACE_ALL			= 0x80000000,
    CUBEMAP_FACE_FORCE_DWORD    = 0x7fffffff
} CUBEMAP_FACES;

// --------------------------------------------------------------------------------------
// d3df2ddpf declaration, used in the constructor of FORMAT and elsewhere
bool d3df2ddpf(D3DFORMAT d3dfFormat, PIXELFORMAT *pddpfFormat);		// definition in testutils8.cpp

// --------------------------------------------------------------------------------------
// FORMAT struct - version independent format
typedef struct _FORMAT
{
	inline _FORMAT() {} 
	inline _FORMAT(D3DFORMAT fmt)
			{d3dfFormat=fmt; d3df2ddpf(d3dfFormat,&ddpfPixelFormat);} 
	inline _FORMAT(FMT fmt)
			{d3dfFormat=(D3DFORMAT)fmt; d3df2ddpf(d3dfFormat,&ddpfPixelFormat);} 
    PIXELFORMAT		ddpfPixelFormat;
    D3DFORMAT       d3dfFormat;

	_FORMAT			*pNext;
}FORMAT, *PFORMAT;

// --------------------------------------------------------------------------------------
// MASK struct - version independent MASK information for a FORMAT
typedef struct _MASK
{
	DWORD                   dwRedMask;
    DWORD                   dwBlueMask;
    DWORD                   dwGreenMask;
    DWORD                   dwAlphaMask;

    int                     nRedShift,   nRedBits;
    int                     nBlueShift,  nBlueBits;
    int                     nGreenShift, nGreenBits;
    int                     nAlphaShift, nAlphaBits;	
}MASK, *LPMASK;

// --------------------------------------------------------------------------------------
// Vertex Buffer Description struct - version independent format

typedef struct _VERTEXBUFFERDESC
{
	union{
    DWORD       dwFVF;          //describes the vertex format of the vertices in this buffer
    DWORD       FVF;               
	};

	//DX 6 and 7
	DWORD       dwSize;         //Size of this structure, in bytes
    DWORD       dwCaps;         //Capability flags that describe the vertex buffer 
    DWORD       dwNumVertices;  //The maximum number of vertices that this vertex buffer can contain

	//dx 8
	FORMAT      Format;         //describes the surface format of the vertex buffer data
    DWORD       Type;           //identifies this resource as a vertex buffer
    DWORD       Usage;          //specifies the usage for this resource
    DWORD       Pool;           //specifies the class of memory allocated for this vertex buffer
    UINT        Size;           //Size of the vertex buffer, in bytes 

} VERTEXBUFFERDESC, *PVERTEXBUFFERDESC;

// --------------------------------------------------------------------------------------
// Index Buffer Description struct - version independent format

typedef struct _INDEXBUFFERDESC
{
	//dx 8
	FORMAT            Format;       //describes the surface format of the vertex buffer data
    DWORD             Type;         //identifies this resource as a vertex buffer
    DWORD             Usage;        //specifies the usage for this resource
    DWORD             Pool;         //specifies the class of memory allocated for this vertex buffer
    UINT              Size;         //Size of the vertex buffer, in bytes 
} INDEXBUFFERDESC, *PINDEXBUFFERDESC;


// --------------------------------------------------------------------------------------
// Surface Description struct - version independent format

typedef struct _SURFACEDESC
{
	//Dx 6 and 7
    DWORD               dwSize;                 // size of the DDSURFACEDESC structure
    DWORD               dwFlags;                // determines what fields are valid
	union{
    DWORD               dwHeight;               // height of surface to be created
    DWORD               Height;               
	};
	union{
    DWORD               dwWidth;                // width of input surface
    DWORD               Width;
	};
    LONG				lPitch;                 // distance to start of next line (return value only)
    DWORD               dwBackBufferCount;      // number of back buffers requested
    DWORD				dwMipMapCount;          // number of mip-map levels requestde
    DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
    DWORD               dwReserved;             // reserved
    LPVOID              lpSurface;              // pointer to the associated surface memory
    COLORKEY			ddckCKDestOverlay;      // color key for destination overlay use
    COLORKEY			ddckCKDestBlt;          // color key for destination blt use
    COLORKEY			ddckCKSrcOverlay;       // color key for source overlay use
    COLORKEY			ddckCKSrcBlt;           // color key for source blt use
    SCAPS2				ddsCaps;                // direct draw surface capabilities
    DWORD               dwTextureStage;         // stage in multitexture cascade
	FORMAT           	Format;					// Surface format
    DWORD     			Type;
    DWORD               Usage;
    DWORD				Pool;
    UINT                Size;
    MULTISAMPLE_TYPE 	MultiSampleType;
    DWORD               MultiSampleQuality;
} SURFACEDESC, *PSURFACEDESC;

// --------------------------------------------------------------------------------------
// Volume Description struct - version independent format
typedef struct _VOLUME_DESC {
    FORMAT              Format;
    RESOURCETYPE        Type;
    DWORD               Usage;
    DWORD               Pool;
    UINT                Size;
    UINT                Width;
    UINT                Height;
    UINT                Depth;
}VOLUME_DESC, *LPVOLUME_DESC;

typedef HRESULT (CALLBACK* LPD3DENUMPIXELFORMATSCALLBACK)(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);

typedef struct _PrimCaps {
    DWORD dwMiscCaps;                 /* Capability flags */
    DWORD dwRasterCaps;
    DWORD dwZCmpCaps;
    DWORD dwSrcBlendCaps;
    DWORD dwDestBlendCaps;
    DWORD dwAlphaCmpCaps;
    DWORD dwShadeCaps;
    DWORD dwTextureCaps;
    DWORD dwTextureFilterCaps;
    DWORD dwTextureBlendCaps;
    DWORD dwTextureAddressCaps;
    DWORD dwStippleWidth;             /* maximum width and height of */
    DWORD dwStippleHeight;            /* of supported stipple (up to 32x32) */
} PRIMCAPS, *LPPRIMCAPS;


typedef struct _VSHADERCAPS2_0
{
        DWORD Caps;
        INT DynamicFlowControlDepth;
        INT NumTemps;
        INT StaticFlowControlDepth;
} VSHADERCAPS2_0;

typedef struct _PSHADERCAPS2_0
{
    DWORD Caps;
    INT DynamicFlowControlDepth;
    INT NumTemps;
    INT StaticFlowControlDepth;
    INT NumInstructionSlots;
} PSHADERCAPS2_0;

typedef struct _DeviceDesc {
	// Direct3D caps
    DWORD       dwCaps;
    DWORD       dwCaps2;
    DWORD       dwCaps3;
    DWORD       dwPresentationIntervals;
    DWORD       dwCursorCaps;
	DWORD       dwDevCaps; 

	// Here are the DX9 specific ones
    DWORD       dwDevCaps2;
    UINT    	MasterAdapterOrdinal;
    UINT    	AdapterOrdinalInGroup;
    UINT    	NumberOfAdaptersInGroup;
	DWORD		NumSimultaneousRTs;
	DWORD		dwReserved5;							// DX9

	// Pre-DX8 style caps
	DWORD		dwFlags;								// DX6
	DWORD		dwColorModel;							// DX6
	DWORD		dwTransformCaps;						// DX6
	DWORD		dwLightingCaps;							// DX6
	DWORD		dwLightingModel;						// DX6
	DWORD       dwReserved1;							// DX7
    DWORD       dwReserved2;							// DX7
    DWORD       dwReserved3;							// DX7
    DWORD       dwReserved4;							// DX7
	PRIMCAPS    dpcLineCaps;
    PRIMCAPS    dpcTriCaps;
    DWORD       dwDeviceRenderBitDepth;
    DWORD       dwDeviceZBufferBitDepth;

    // DX8 style caps
	DWORD		dwPrimitiveMiscCaps;
	DWORD		dwRasterCaps;
	DWORD		dwZCmpCaps;
	DWORD		dwSrcBlendCaps;
	DWORD		dwDestBlendCaps;
	DWORD		dwAlphaCmpCaps;
	DWORD		dwShadeCaps;
	DWORD		dwTextureCaps;
	DWORD		dwTextureFilterCaps;
	DWORD		dwCubeTextureFilterCaps;
	DWORD		dwVolumeTextureFilterCaps;
	DWORD		dwTextureAddressCaps;
	DWORD		dwVolumeTextureAddressCaps;
	DWORD		dwLineCaps;
	
	// Common
	UINT		AdapterOrdinal;							// DX8 - 9
	DWORD       dwMinTextureWidth, dwMinTextureHeight;
    DWORD       dwMaxTextureWidth, dwMaxTextureHeight;
	DWORD		dwMinVolumeExtent, dwMaxVolumeExtent;

    DWORD       dwMaxTextureRepeat;
    DWORD       dwMaxTextureAspectRatio;
    DWORD       dwMaxAnisotropy;
    D3DVALUE    dvMaxVertexW;

    D3DVALUE    dvGuardBandLeft;
    D3DVALUE    dvGuardBandTop;
    D3DVALUE    dvGuardBandRight;
    D3DVALUE    dvGuardBandBottom;

    D3DVALUE    dvExtentsAdjust;
    DWORD       dwStencilCaps;

    DWORD       dwFVFCaps;
    DWORD       dwTextureOpCaps;
    WORD        wMaxTextureBlendStages;
    WORD        wMaxSimultaneousTextures;

    DWORD       dwVertexProcessingCaps;
    DWORD       dwMaxActiveLights;
    WORD        wMaxUserClipPlanes;
    WORD        wMaxVertexBlendMatrices;
	DWORD		dwMaxVertexBlendMatrixIndex;			// DX8

    D3DVALUE    dvMaxPointSize;							// DX8
	DWORD		dwMaxPrimitiveCount;					// DX8
    DWORD       dwMaxVertexIndex;						// DX8
    DWORD       dwMaxStreams;							// DX8
    DWORD       dwMaxStreamStride;						// DX8

    DWORD       dwVertexShaderVersion;					// DX8
    DWORD       dwMaxVertexShaderConst;					// DX8
    DWORD       dwPixelShaderVersion;					// DX8
    D3DVALUE    dvMaxPixelShaderValue;					// DX8

    DWORD		dwDeclTypes;							// DX9
	
	VSHADERCAPS2_0	VS20Caps;							// DX9
	PSHADERCAPS2_0	PS20Caps;							// DX9

    DWORD		dwVertexTextureFilterCaps;				// DX9
	DWORD		dwStretchRectFilterCaps;				// DX9
    DWORD		dwMaxVShaderInstructionsExecuted;		// DX9
    DWORD		dwMaxPShaderInstructionsExecuted;		// DX9
	DWORD		MaxVertexShader30InstructionSlots;		// DX9
    DWORD		MaxPixelShader30InstructionSlots;		// DX9
	float		MaxNpatchTessellationLevel;				// DX9
} DEVICEDESC, *LPDEVICEDESC;

typedef struct _CAPS
{
    /* Device Info */
    D3DDEVTYPE  DeviceType;
    UINT    AdapterOrdinal;

    /* Caps from DX7 Draw */
    DWORD   Caps;
    DWORD   Caps2;
    DWORD   Caps3;
    DWORD   PresentationIntervals;

    /* Cursor Caps */
    DWORD   CursorCaps;

    /* 3D Device Caps */
    DWORD   DevCaps;

    DWORD   PrimitiveMiscCaps;
    DWORD   RasterCaps;
    DWORD   ZCmpCaps;
    DWORD   SrcBlendCaps;
    DWORD   DestBlendCaps;
    DWORD   AlphaCmpCaps;
    DWORD   ShadeCaps;
    DWORD   TextureCaps;
    DWORD   TextureFilterCaps;          // D3DPTFILTERCAPS for IDirect3DTexture8's
    DWORD   CubeTextureFilterCaps;      // D3DPTFILTERCAPS for IDirect3DCubeTexture8's
    DWORD   VolumeTextureFilterCaps;    // D3DPTFILTERCAPS for IDirect3DVolumeTexture8's
    DWORD   TextureAddressCaps;         // D3DPTADDRESSCAPS for IDirect3DTexture8's
    DWORD   VolumeTextureAddressCaps;   // D3DPTADDRESSCAPS for IDirect3DVolumeTexture8's

    DWORD   LineCaps;                   // D3DLINECAPS

    DWORD   MaxTextureWidth, MaxTextureHeight;
    DWORD   MaxVolumeExtent;

    DWORD   MaxTextureRepeat;
    DWORD   MaxTextureAspectRatio;
    DWORD   MaxAnisotropy;
    float   MaxVertexW;

    float   GuardBandLeft;
    float   GuardBandTop;
    float   GuardBandRight;
    float   GuardBandBottom;

    float   ExtentsAdjust;
    DWORD   StencilCaps;

    DWORD   FVFCaps;
    DWORD   TextureOpCaps;
    DWORD   MaxTextureBlendStages;
    DWORD   MaxSimultaneousTextures;

    DWORD   VertexProcessingCaps;
    DWORD   MaxActiveLights;
    DWORD   MaxUserClipPlanes;
    DWORD   MaxVertexBlendMatrices;
    DWORD   MaxVertexBlendMatrixIndex;

    float   MaxPointSize;

    DWORD   MaxPrimitiveCount;          // max number of primitives per DrawPrimitive call
    DWORD   MaxVertexIndex;
    DWORD   MaxStreams;
    DWORD   MaxStreamStride;            // max stride for SetStreamSource

    DWORD   VertexShaderVersion;
    DWORD   MaxVertexShaderConst;       // number of vertex shader constant registers

    DWORD   PixelShaderVersion;
    float   MaxPixelShaderValue;        // max value of pixel shader arithmetic component

    // Here are the DX9 specific ones
    DWORD   DevCaps2;

	float   MaxNpatchTessellationLevel;
    DWORD   Reserved5;

    UINT    MasterAdapterOrdinal;       // ordinal of master adaptor for adapter group
    UINT    AdapterOrdinalInGroup;      // ordinal inside the adapter group
    UINT    NumberOfAdaptersInGroup;    // number of adapters in this adapter group (only if master)
    DWORD   DeclTypes;                  // Data types, supported in vertex declarations
    DWORD   NumSimultaneousRTs;         // Will be at least 1
    DWORD   StretchRectFilterCaps;      // Filter caps supported by StretchRect
	VSHADERCAPS2_0 VS20Caps;
    PSHADERCAPS2_0 PS20Caps;
    DWORD   VertexTextureFilterCaps;    // D3DPTFILTERCAPS for IDirect3DTexture9's for texture, used in vertex shaders
    DWORD   MaxVShaderInstructionsExecuted; // maximum number of vertex shader instructions that can be executed
    DWORD   MaxPShaderInstructionsExecuted; // maximum number of pixel shader instructions that can be executed
    DWORD   MaxVertexShader30InstructionSlots; 
    DWORD   MaxPixelShader30InstructionSlots;
} CAPS;

//
// LineCaps
//
#define LINECAPS_TEXTURE             0x00000001L
#define LINECAPS_ZTEST               0x00000002L
#define LINECAPS_BLEND               0x00000004L
#define LINECAPS_ALPHACMP            0x00000008L
#define LINECAPS_FOG                 0x00000010L

typedef struct _COLORVALUE {
    union {
    D3DVALUE r;
    D3DVALUE dvR;
    };
    union {
    D3DVALUE g;
    D3DVALUE dvG;
    };
    union {
    D3DVALUE b;
    D3DVALUE dvB;
    };
    union {
    D3DVALUE a;
    D3DVALUE dvA;
    };
} COLORVALUE, *LPCOLORVALUE;


typedef struct _VECTOR {
    union {
    D3DVALUE x;
    D3DVALUE dvX;
    };
    union {
    D3DVALUE y;
    D3DVALUE dvY;
    };
    union {
    D3DVALUE z;
    D3DVALUE dvZ;
    };
    public:

    _VECTOR& operator = (const _D3DVECTOR& v);
	
} VECTOR, *LPVECTOR;

typedef struct _LIGHT {
	union{
    D3DLIGHTTYPE    Type;            /* Type of light source */
    D3DLIGHTTYPE    dltType;            /* Type of light source */
	};
    union{
    COLORVALUE      dcvDiffuse;         /* Diffuse color of light */
    COLORVALUE      Diffuse;            /* Diffuse color of light */
	};
    union{
    COLORVALUE      dcvSpecular;        /* Specular color of light */
    COLORVALUE      Specular;           /* Specular color of light */
    };
	union{
    COLORVALUE      dcvAmbient;         /* Ambient color of light */
    COLORVALUE      Ambient;            /* Ambient color of light */
    };
	union{
    VECTOR      	dvPosition;         /* Position in world space */
    VECTOR      	Position;           /* Position in world space */
    };
	union{
    VECTOR  	    dvDirection;        /* Direction in world space */
    VECTOR	    Direction;          /* Direction in world space */
    };
	union{
    D3DVALUE        dvRange;            /* Cutoff range */
    D3DVALUE        Range;              /* Cutoff range */
    };
	union{
    D3DVALUE        dvFalloff;          /* Falloff */
    D3DVALUE        Falloff;            /* Falloff */
    };
	union{
    D3DVALUE        dvAttenuation0;     /* Constant attenuation */
    D3DVALUE        Attenuation0;       /* Constant attenuation */
    };
	union{
    D3DVALUE        dvAttenuation1;     /* Linear attenuation */
    D3DVALUE        Attenuation1;       /* Linear attenuation */
    };
	union{
    D3DVALUE        dvAttenuation2;     /* Quadratic attenuation */
    D3DVALUE        Attenuation2;       /* Quadratic attenuation */
    };
	union{
    D3DVALUE        dvTheta;            /* Inner angle of spotlight cone */
    D3DVALUE        Theta;              /* Inner angle of spotlight cone */
    };
    union{
    D3DVALUE        dvPhi;              /* Outer angle of spotlight cone */
    D3DVALUE        Phi;                /* Outer angle of spotlight cone */
    };
} LIGHT, *LPLIGHT;

typedef struct _MATERIAL {
    union {
    COLORVALUE      Diffuse;
    COLORVALUE      diffuse;        
    COLORVALUE      dcvDiffuse;
    };
    union {
    COLORVALUE      Ambient;
    COLORVALUE      ambient;
    COLORVALUE      dcvAmbient;
    };
    union {
    COLORVALUE      Specular;
    COLORVALUE      specular;
    COLORVALUE      dcvSpecular;
    };
    union {
    COLORVALUE      Emissive;
    COLORVALUE      emissive;
    COLORVALUE      dcvEmissive;
    };
    union {
    D3DVALUE        Power;
    D3DVALUE        power;
    D3DVALUE        dvPower;
    };
} MATERIAL, *LPMATERIAL;

typedef struct _DP_PTRSTRIDE
{
    LPVOID lpvData;
    DWORD  dwStride;
} DP_PTRSTRIDE;

typedef struct _DRAWPRIMITIVESTRIDEDDATA
{
    DP_PTRSTRIDE position;
    DP_PTRSTRIDE normal;
    DP_PTRSTRIDE diffuse;
    DP_PTRSTRIDE specular;
    DP_PTRSTRIDE textureCoords[8];
} DRAWPRIMITIVESTRIDEDDATA, *LPDRAWPRIMITIVESTRIDEDDATA;



#define CLIPSTATUS_LEFT        0x00000001L
#define CLIPSTATUS_RIGHT       0x00000002L
#define CLIPSTATUS_TOP         0x00000004L
#define CLIPSTATUS_BOTTOM      0x00000008L
#define CLIPSTATUS_FRONT       0x00000010L
#define CLIPSTATUS_BACK        0x00000020L
#define CLIPSTATUS_PLANE0      0x00000040L
#define CLIPSTATUS_PLANE1      0x00000080L
#define CLIPSTATUS_PLANE2      0x00000100L
#define CLIPSTATUS_PLANE3      0x00000200L
#define CLIPSTATUS_PLANE4      0x00000400L
#define CLIPSTATUS_PLANE5      0x00000800L

#define CLIPSTATUS_ALL CLIPSTATUS_LEFT   | \
                       CLIPSTATUS_RIGHT  | \
                       CLIPSTATUS_TOP    | \
                       CLIPSTATUS_BOTTOM | \
                       CLIPSTATUS_FRONT  | \
                       CLIPSTATUS_BACK   | \
                       CLIPSTATUS_PLANE0 | \
                       CLIPSTATUS_PLANE1 | \
                       CLIPSTATUS_PLANE2 | \
                       CLIPSTATUS_PLANE3 | \
                       CLIPSTATUS_PLANE4 | \
                       CLIPSTATUS_PLANE5;

typedef struct _CLIPSTATUS
{
    DWORD dwClipUnion;
    DWORD dwClipIntersection;
	float minx, maxx; 
	float miny, maxy; 
	float minz, maxz; 
} CLIPSTATUS, *LPCLIPSTATUS;

typedef struct _VIEWPORT
{
    DWORD       X;
    DWORD       Y;
    DWORD       Width;
    DWORD       Height;
    float       MinZ;
    float       MaxZ;
} VIEWPORT, *LPVIEWPORT;

typedef struct _LOCKED_RECT {
    INT                 iPitch;
    void*               pBits;
} LOCKED_RECT;

/* Structures for LockBox */
typedef struct _BOX
{
    UINT                Left;
    UINT                Top;
    UINT                Right;
    UINT                Bottom;
    UINT                Front;
    UINT                Back;
} BOX;

typedef struct _LOCKED_BOX
{
    INT                 RowPitch;
    INT                 SlicePitch;
    void*               pBits;
} LOCKED_BOX;

// --------------------------------------------------------------------------------------
// PRESENT_PARAMETERS - Present Params for D3D Devices

typedef struct _PRESENT_PARAMETERS
{
	UINT					uWidth;
	UINT					uHeight;
	FORMAT					BackBufferFormat;
	UINT					uBackBufferCount;
	MULTISAMPLE_TYPE		MultiSample;
    DWORD               	MultiSampleQuality;
	DWORD					SwapEffect;
	HWND					hDeviceWindow;
	bool					bWindowed;
	bool					bAutoDepthStencil;
	bool					bCachedBackBuffers;	// Do the BackBuffers get cached
	FORMAT					AutoDepthStencilFormat;
	DWORD					dwFlags;
	UINT					uFullscreenRefresh;
	union
	{
		UINT					uFullscreenPresentInterval;
        UINT					uPresentInterval;
	};
	DWORD					dwInternalFlags;
	FORMAT					DesktopFormat;		// Format of the mode the device will be created in
}PRESENT_PARAMETERS, *LPPRESENT_PARAMETERS;

// --------------------------------------------------------------------------------------
// VERTEXELEMENT - Used for defining Declarator

typedef struct _VERTEXELEMENT
{
    WORD            Stream;     // Stream index
    WORD            Offset;     // Offset in the stream in bytes
    BYTE     		Type;       // Data type
    BYTE   			Method;     // Processing method
    BYTE    		Usage;      // Semantics
    BYTE            UsageIndex; // Semantic index
} VERTEXELEMENT, *LPVERTEXELEMENT;
// Vertex element terminator:
#define DECL_END() {0xFF,0,DECLTYPE_UNUSED,0,0,0}

/*
 * Format of RGBA colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    alpha      |      red      |     green     |     blue      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
#define RGBA_GETRED(rgb)        (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       ((rgb) & 0xff)
#define RGBA_MAKE(r, g, b, a)   ((D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

/*
 * Format of RGB colors is
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    ignored    |      red      |     green     |     blue      |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
#define RGB_GETRED(rgb)         (((rgb) >> 16) & 0xff)
#define RGB_GETGREEN(rgb)       (((rgb) >> 8) & 0xff)
#define RGB_GETBLUE(rgb)        ((rgb) & 0xff)
#define RGBA_SETALPHA(rgba, x) (((x) << 24) | ((rgba) & 0x00ffffff))
#define RGB_MAKE(r, g, b)       ((D3DCOLOR) (((r) << 16) | ((g) << 8) | (b)))
#define RGBA_TORGB(rgba)       ((D3DCOLOR) ((rgba) & 0xffffff))
#define RGB_TORGBA(rgb)        ((D3DCOLOR) ((rgb) | 0xff000000))

// Constants for texture coordinates size calculations
#define CDIRECT3D_TEXTUREFORMATS_SHIFT  16      // Number of bits to shift to get texture format descriptions
#define CDIRECT3D_TEXTUREFORMAT_MASK    0x03    // Mask to extract texture format description
#define CDIRECT3D_TEXTUREFORMAT_SIZE    2       // Number of bits for one texture format description

// --------------------------------------------------------------------------------------

bool FormatToMask(FORMAT *pFormat, MASK *pMask);		// definition in testutils8.cpp
DWORD ColorConvert(DWORD dwSource, LPMASK pMask);
DWORD ReadColor(PVOID * ppSurfacePtr, FORMAT* pFmt, MASK *pMask, bool bInc);

#endif // __CD3DDEF_H__
