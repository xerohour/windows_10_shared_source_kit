#ifndef D3DLIGHTCAPS_PARALLELPOINT
	#define D3DLIGHTCAPS_PARALLELPOINT      0x00000008L /* Parallel point lights supported */
#endif

#ifndef D3DDEVCAPS_HWVERTEXBUFFER
	#define D3DDEVCAPS_HWVERTEXBUFFER       0x02000000L
#endif

#ifndef D3DDEVCAPS_HWINDEXBUFFER
	#define D3DDEVCAPS_HWINDEXBUFFER        0x04000000L
#endif

#ifndef D3DDEVCAPS_SUBVOLUMELOCK
	#define D3DDEVCAPS_SUBVOLUMELOCK           0x08000000L
#endif

#ifndef D3DPMISCCAPS_FOGINFVF
	#define D3DPMISCCAPS_FOGINFVF           0x00002000L
#endif
	
typedef struct _CapStruct
{
	LPCSTR szName;
	DWORD dwValue;
} CapStruct, * pCapStruct;

CapStruct VertexShaderCap[] =
{
	{ "D3DVS20CAPS_PREDICATION", D3DVS20CAPS_PREDICATION }
};

CapStruct PixelShaderCap[] =
{
	{ "D3DPS20CAPS_ARBITRARYSWIZZLE", D3DPS20CAPS_ARBITRARYSWIZZLE },
	{ "D3DPS20CAPS_GRADIENTINSTRUCTIONS", D3DPS20CAPS_GRADIENTINSTRUCTIONS },
	{ "D3DPS20CAPS_PREDICATION", D3DPS20CAPS_PREDICATION },
	{ "D3DPS20CAPS_NODEPENDENTREADLIMIT", D3DPS20CAPS_NODEPENDENTREADLIMIT },
	{ "D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT", D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT }
};

CapStruct CapsCap[] =
{
	{ "D3DCAPS_OVERLAY", D3DCAPS_OVERLAY }, // BUG: 252006 CapsCheck.exe is failing due to new D3DCAPS in D3D9 API
	{ "D3DCAPS_READ_SCANLINE", D3DCAPS_READ_SCANLINE }
};

CapStruct Caps2Cap[] =
{
	{ "D3DCAPS2_NO2DDURING3DSCENE", D3DCAPS2_NO2DDURING3DSCENE },
	{ "D3DCAPS2_FULLSCREENGAMMA", D3DCAPS2_FULLSCREENGAMMA },
	{ "D3DCAPS2_CANRENDERWINDOWED", D3DCAPS2_CANRENDERWINDOWED },
	{ "D3DCAPS2_CANCALIBRATEGAMMA", D3DCAPS2_CANCALIBRATEGAMMA },
	{ "D3DCAPS2_CANMANAGERESOURCE", D3DCAPS2_CANMANAGERESOURCE },
	{ "D3DCAPS2_DYNAMICTEXTURES", D3DCAPS2_DYNAMICTEXTURES },
	{ "D3DCAPS2_CANAUTOGENMIPMAP", D3DCAPS2_CANAUTOGENMIPMAP },
	{ "D3DCAPS2_CANSHARERESOURCE", D3DCAPS2_CANSHARERESOURCE },
	{ "D3DCAPS2_RESERVED", D3DCAPS2_RESERVED }
};

CapStruct Caps3Cap[] =
{
	{ "D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD", D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD },
	{ "D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION", D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION },
	{ "D3DCAPS3_COPY_TO_VIDMEM", D3DCAPS3_COPY_TO_VIDMEM },
	{ "D3DCAPS3_COPY_TO_SYSTEMMEM", D3DCAPS3_COPY_TO_SYSTEMMEM },
	//{ "D3DCAPS3_MANAGEDDEVICE", D3DCAPS3_MANAGEDDEVICE },
	{ "D3DCAPS3_RESERVED", D3DCAPS3_RESERVED },
	{ "D3DCAPS3_DXVAHD", D3DCAPS3_DXVAHD } // BUG: 252006 CapsCheck.exe is failing due to new D3DCAPS in D3D9 API
};

CapStruct PresentCap[] =
{
	{ "D3DPRESENT_INTERVAL_DEFAULT", D3DPRESENT_INTERVAL_DEFAULT },
	{ "D3DPRESENT_INTERVAL_ONE", D3DPRESENT_INTERVAL_ONE },
	{ "D3DPRESENT_INTERVAL_TWO", D3DPRESENT_INTERVAL_TWO },
	{ "D3DPRESENT_INTERVAL_THREE", D3DPRESENT_INTERVAL_THREE },
	{ "D3DPRESENT_INTERVAL_FOUR", D3DPRESENT_INTERVAL_FOUR },
	{ "D3DPRESENT_INTERVAL_IMMEDIATE", D3DPRESENT_INTERVAL_IMMEDIATE}
};

CapStruct CursorCap[] =
{
	{ "D3DCURSORCAPS_COLOR", D3DCURSORCAPS_COLOR },
	{ "D3DCURSORCAPS_LOWRES", D3DCURSORCAPS_LOWRES }
};

CapStruct DevCap[] =
{
	{ "D3DDEVCAPS_FLOATTLVERTEX", D3DDEVCAPS_FLOATTLVERTEX },
	{ "D3DDEVCAPS_SORTINCREASINGZ", D3DDEVCAPS_SORTINCREASINGZ },
	{ "D3DDEVCAPS_SORTDECREASINGZ", D3DDEVCAPS_SORTDECREASINGZ },
	{ "D3DDEVCAPS_SORTEXACT", D3DDEVCAPS_SORTEXACT },
	{ "D3DDEVCAPS_EXECUTESYSTEMMEMORY", D3DDEVCAPS_EXECUTESYSTEMMEMORY },
	{ "D3DDEVCAPS_EXECUTEVIDEOMEMORY", D3DDEVCAPS_EXECUTEVIDEOMEMORY },
	{ "D3DDEVCAPS_TLVERTEXSYSTEMMEMORY", D3DDEVCAPS_TLVERTEXSYSTEMMEMORY },
	{ "D3DDEVCAPS_TLVERTEXVIDEOMEMORY", D3DDEVCAPS_TLVERTEXVIDEOMEMORY },
	{ "D3DDEVCAPS_TEXTURESYSTEMMEMORY", D3DDEVCAPS_TEXTURESYSTEMMEMORY },
	{ "D3DDEVCAPS_TEXTUREVIDEOMEMORY", D3DDEVCAPS_TEXTUREVIDEOMEMORY },
	{ "D3DDEVCAPS_DRAWPRIMTLVERTEX", D3DDEVCAPS_DRAWPRIMTLVERTEX },
	{ "D3DDEVCAPS_CANRENDERAFTERFLIP", D3DDEVCAPS_CANRENDERAFTERFLIP },
	{ "D3DDEVCAPS_TEXTURENONLOCALVIDMEM", D3DDEVCAPS_TEXTURENONLOCALVIDMEM },
	{ "D3DDEVCAPS_DRAWPRIMITIVES2", D3DDEVCAPS_DRAWPRIMITIVES2 },
	{ "D3DDEVCAPS_SEPARATETEXTUREMEMORIES", D3DDEVCAPS_SEPARATETEXTUREMEMORIES },
	{ "D3DDEVCAPS_DRAWPRIMITIVES2EX", D3DDEVCAPS_DRAWPRIMITIVES2EX },
	{ "D3DDEVCAPS_HWTRANSFORMANDLIGHT", D3DDEVCAPS_HWTRANSFORMANDLIGHT },
	{ "D3DDEVCAPS_CANBLTSYSTONONLOCAL", D3DDEVCAPS_CANBLTSYSTONONLOCAL },
	{ "D3DDEVCAPS_HWRASTERIZATION", D3DDEVCAPS_HWRASTERIZATION },
	{ "D3DDEVCAPS_PUREDEVICE", D3DDEVCAPS_PUREDEVICE },
	{ "D3DDEVCAPS_QUINTICRTPATCHES", D3DDEVCAPS_QUINTICRTPATCHES },
	{ "D3DDEVCAPS_RTPATCHES", D3DDEVCAPS_RTPATCHES },
	{ "D3DDEVCAPS_RTPATCHHANDLEZERO", D3DDEVCAPS_RTPATCHHANDLEZERO },
	{ "D3DDEVCAPS_NPATCHES", D3DDEVCAPS_NPATCHES },
	{ "D3DDEVCAPS_HWVERTEXBUFFER", D3DDEVCAPS_HWVERTEXBUFFER },
	{ "D3DDEVCAPS_HWINDEXBUFFER", D3DDEVCAPS_HWINDEXBUFFER },
	{ "D3DDEVCAPS_SUBVOLUMELOCK", D3DDEVCAPS_SUBVOLUMELOCK },
	//{ "D3DDEVCAPS_USERMODEDRIVER", D3DDEVCAPS_USERMODEDRIVER }
};

CapStruct MiscPrimitiveCap[] =
{
	{ "D3DPMISCCAPS_MASKPLANES", D3DPMISCCAPS_MASKPLANES },
	{ "D3DPMISCCAPS_MASKZ", D3DPMISCCAPS_MASKZ },
	{ "D3DPMISCCAPS_LINEPATTERNREP", D3DPMISCCAPS_LINEPATTERNREP },
	{ "D3DPMISCCAPS_CONFORMANT", D3DPMISCCAPS_CONFORMANT },
	{ "D3DPMISCCAPS_CULLNONE", D3DPMISCCAPS_CULLNONE },
	{ "D3DPMISCCAPS_CULLCW", D3DPMISCCAPS_CULLCW },
	{ "D3DPMISCCAPS_CULLCCW", D3DPMISCCAPS_CULLCCW },
	{ "D3DPMISCCAPS_COLORWRITEENABLE", D3DPMISCCAPS_COLORWRITEENABLE },
	{ "D3DPMISCCAPS_CLIPPLANESCALEDPOINTS", D3DPMISCCAPS_CLIPPLANESCALEDPOINTS },
	{ "D3DPMISCCAPS_CLIPTLVERTS", D3DPMISCCAPS_CLIPTLVERTS },
	{ "D3DPMISCCAPS_TSSARGTEMP", D3DPMISCCAPS_TSSARGTEMP },
	{ "D3DPMISCCAPS_BLENDOP", D3DPMISCCAPS_BLENDOP },
	{ "D3DPMISCCAPS_NULLREFERENCE", D3DPMISCCAPS_NULLREFERENCE },
	{ "D3DPMISCCAPS_FOGINFVF", D3DPMISCCAPS_FOGINFVF },
	{ "D3DPMISCCAPS_INDEPENDENTWRITEMASKS", D3DPMISCCAPS_INDEPENDENTWRITEMASKS },
	{ "D3DPMISCCAPS_PERSTAGECONSTANT", D3DPMISCCAPS_PERSTAGECONSTANT },
	{ "D3DPMISCCAPS_FOGANDSPECULARALPHA", D3DPMISCCAPS_FOGANDSPECULARALPHA },
	{ "D3DPMISCCAPS_SEPARATEALPHABLEND", D3DPMISCCAPS_SEPARATEALPHABLEND },
	{ "D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS", D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS },
	{ "D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING", D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING },
	{ "D3DPMISCCAPS_FOGVERTEXCLAMPED", D3DPMISCCAPS_FOGVERTEXCLAMPED },
	{ "D3DPMISCCAPS_POSTBLENDSRGBCONVERT", D3DPMISCCAPS_POSTBLENDSRGBCONVERT }
};

CapStruct LineCap[] =
{
	{ "D3DLINECAPS_TEXTURE", D3DLINECAPS_TEXTURE },
	{ "D3DLINECAPS_ZTEST", D3DLINECAPS_ZTEST },
	{ "D3DLINECAPS_BLEND", D3DLINECAPS_BLEND },
	{ "D3DLINECAPS_ALPHACMP", D3DLINECAPS_ALPHACMP },
	{ "D3DLINECAPS_FOG", D3DLINECAPS_FOG },
	{ "D3DLINECAPS_ANTIALIAS", D3DLINECAPS_ANTIALIAS }
};

CapStruct RasterCap[] =
{
	{ "D3DPRASTERCAPS_DITHER", D3DPRASTERCAPS_DITHER },
	{ "D3DPRASTERCAPS_ROP2", D3DPRASTERCAPS_ROP2 },
	{ "D3DPRASTERCAPS_XOR", D3DPRASTERCAPS_XOR },
	{ "D3DPRASTERCAPS_PAT", D3DPRASTERCAPS_PAT },
	{ "D3DPRASTERCAPS_ZTEST", D3DPRASTERCAPS_ZTEST },
	{ "D3DPRASTERCAPS_SUBPIXEL", D3DPRASTERCAPS_SUBPIXEL },
	{ "D3DPRASTERCAPS_SUBPIXELX", D3DPRASTERCAPS_SUBPIXELX },
	{ "D3DPRASTERCAPS_FOGVERTEX", D3DPRASTERCAPS_FOGVERTEX },
	{ "D3DPRASTERCAPS_FOGTABLE", D3DPRASTERCAPS_FOGTABLE },
	{ "D3DPRASTERCAPS_STIPPLE", D3DPRASTERCAPS_STIPPLE },
	{ "D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT", D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT },
	{ "D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT", D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT },
	{ "D3DPRASTERCAPS_ANTIALIASEDGES", D3DPRASTERCAPS_ANTIALIASEDGES },
	{ "D3DPRASTERCAPS_MIPMAPLODBIAS", D3DPRASTERCAPS_MIPMAPLODBIAS },
	{ "D3DPRASTERCAPS_ZBIAS", D3DPRASTERCAPS_ZBIAS },
	{ "D3DPRASTERCAPS_ZBUFFERLESSHSR", D3DPRASTERCAPS_ZBUFFERLESSHSR },
	{ "D3DPRASTERCAPS_FOGRANGE", D3DPRASTERCAPS_FOGRANGE },
	{ "D3DPRASTERCAPS_ANISOTROPY", D3DPRASTERCAPS_ANISOTROPY },
	{ "D3DPRASTERCAPS_WBUFFER", D3DPRASTERCAPS_WBUFFER },
	{ "D3DPRASTERCAPS_TRANSLUCENTSORTINDEPENDENT", D3DPRASTERCAPS_TRANSLUCENTSORTINDEPENDENT },
	{ "D3DPRASTERCAPS_WFOG", D3DPRASTERCAPS_WFOG },
	{ "D3DPRASTERCAPS_ZFOG", D3DPRASTERCAPS_ZFOG },
	{ "D3DPRASTERCAPS_COLORPERSPECTIVE", D3DPRASTERCAPS_COLORPERSPECTIVE },
	{ "D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE", D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE },
	{ "D3DPRASTERCAPS_SCISSORTEST", D3DPRASTERCAPS_SCISSORTEST },
	{ "D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS", D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS },
	{ "D3DPRASTERCAPS_DEPTHBIAS", D3DPRASTERCAPS_DEPTHBIAS },
	{ "D3DPRASTERCAPS_MULTISAMPLE_TOGGLE", D3DPRASTERCAPS_MULTISAMPLE_TOGGLE }
};

CapStruct AlphaZCmpCap[] =
{
	{ "D3DPCMPCAPS_NEVER", D3DPCMPCAPS_NEVER },
	{ "D3DPCMPCAPS_LESS", D3DPCMPCAPS_LESS },
	{ "D3DPCMPCAPS_EQUAL", D3DPCMPCAPS_EQUAL },
	{ "D3DPCMPCAPS_LESSEQUAL", D3DPCMPCAPS_LESSEQUAL },
	{ "D3DPCMPCAPS_GREATER", D3DPCMPCAPS_GREATER },
	{ "D3DPCMPCAPS_NOTEQUAL", D3DPCMPCAPS_NOTEQUAL },
	{ "D3DPCMPCAPS_GREATEREQUAL", D3DPCMPCAPS_GREATEREQUAL },
	{ "D3DPCMPCAPS_ALWAYS", D3DPCMPCAPS_ALWAYS }
};

CapStruct DestSrcBlendCap[] =
{
	{ "D3DPBLENDCAPS_ZERO", D3DPBLENDCAPS_ZERO },
	{ "D3DPBLENDCAPS_ONE", D3DPBLENDCAPS_ONE },
	{ "D3DPBLENDCAPS_SRCCOLOR", D3DPBLENDCAPS_SRCCOLOR },
	{ "D3DPBLENDCAPS_INVSRCCOLOR", D3DPBLENDCAPS_INVSRCCOLOR },
	{ "D3DPBLENDCAPS_SRCALPHA", D3DPBLENDCAPS_SRCALPHA },
	{ "D3DPBLENDCAPS_INVSRCALPHA", D3DPBLENDCAPS_INVSRCALPHA },
	{ "D3DPBLENDCAPS_DESTALPHA", D3DPBLENDCAPS_DESTALPHA },
	{ "D3DPBLENDCAPS_INVDESTALPHA", D3DPBLENDCAPS_INVDESTALPHA },
	{ "D3DPBLENDCAPS_DESTCOLOR", D3DPBLENDCAPS_DESTCOLOR },
	{ "D3DPBLENDCAPS_INVDESTCOLOR", D3DPBLENDCAPS_INVDESTCOLOR },
	{ "D3DPBLENDCAPS_SRCALPHASAT", D3DPBLENDCAPS_SRCALPHASAT },
	{ "D3DPBLENDCAPS_BOTHSRCALPHA", D3DPBLENDCAPS_BOTHSRCALPHA },
	{ "D3DPBLENDCAPS_BOTHINVSRCALPHA", D3DPBLENDCAPS_BOTHINVSRCALPHA },
	{ "D3DPBLENDCAPS_BLENDFACTOR", D3DPBLENDCAPS_BLENDFACTOR },
	{ "D3DPBLENDCAPS_SRCCOLOR2", D3DPBLENDCAPS_SRCCOLOR2 },
	{ "D3DPBLENDCAPS_INVSRCCOLOR2", D3DPBLENDCAPS_INVSRCCOLOR2 }
};

CapStruct ShadeCap[] =
{
	{ "D3DPSHADECAPS_COLORFLATMONO", D3DPSHADECAPS_COLORFLATMONO },
	{ "D3DPSHADECAPS_COLORFLATRGB", D3DPSHADECAPS_COLORFLATRGB },
	{ "D3DPSHADECAPS_COLORGOURAUDMONO", D3DPSHADECAPS_COLORGOURAUDMONO },
	{ "D3DPSHADECAPS_COLORGOURAUDRGB", D3DPSHADECAPS_COLORGOURAUDRGB },
	{ "D3DPSHADECAPS_COLORPHONGMONO", D3DPSHADECAPS_COLORPHONGMONO },
	{ "D3DPSHADECAPS_COLORPHONGRGB", D3DPSHADECAPS_COLORPHONGRGB },
	{ "D3DPSHADECAPS_SPECULARFLATMONO", D3DPSHADECAPS_SPECULARFLATMONO },
	{ "D3DPSHADECAPS_SPECULARFLATRGB", D3DPSHADECAPS_SPECULARFLATRGB },
	{ "D3DPSHADECAPS_SPECULARGOURAUDMONO", D3DPSHADECAPS_SPECULARGOURAUDMONO },
	{ "D3DPSHADECAPS_SPECULARGOURAUDRGB", D3DPSHADECAPS_SPECULARGOURAUDRGB },
	{ "D3DPSHADECAPS_SPECULARPHONGMONO", D3DPSHADECAPS_SPECULARPHONGMONO },
	{ "D3DPSHADECAPS_SPECULARPHONGRGB", D3DPSHADECAPS_SPECULARPHONGRGB },
	{ "D3DPSHADECAPS_ALPHAFLATBLEND", D3DPSHADECAPS_ALPHAFLATBLEND },
	{ "D3DPSHADECAPS_ALPHAFLATSTIPPLED", D3DPSHADECAPS_ALPHAFLATSTIPPLED },
	{ "D3DPSHADECAPS_ALPHAGOURAUDBLEND", D3DPSHADECAPS_ALPHAGOURAUDBLEND },
	{ "D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED", D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED },
	{ "D3DPSHADECAPS_ALPHAPHONGBLEND", D3DPSHADECAPS_ALPHAPHONGBLEND },
	{ "D3DPSHADECAPS_ALPHAPHONGSTIPPLED", D3DPSHADECAPS_ALPHAPHONGSTIPPLED },
	{ "D3DPSHADECAPS_FOGFLAT", D3DPSHADECAPS_FOGFLAT },
	{ "D3DPSHADECAPS_FOGGOURAUD", D3DPSHADECAPS_FOGGOURAUD },
	{ "D3DPSHADECAPS_FOGPHONG", D3DPSHADECAPS_FOGPHONG},
};

CapStruct TextureCap[] =
{
	{ "D3DPTEXTURECAPS_PERSPECTIVE", D3DPTEXTURECAPS_PERSPECTIVE },
	{ "D3DPTEXTURECAPS_POW2", D3DPTEXTURECAPS_POW2 },
	{ "D3DPTEXTURECAPS_ALPHA", D3DPTEXTURECAPS_ALPHA },
	{ "D3DPTEXTURECAPS_TRANSPARENCY", D3DPTEXTURECAPS_TRANSPARENCY },
	{ "D3DPTEXTURECAPS_BORDER", D3DPTEXTURECAPS_BORDER },
	{ "D3DPTEXTURECAPS_SQUAREONLY", D3DPTEXTURECAPS_SQUAREONLY },
	{ "D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE", D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE },
	{ "D3DPTEXTURECAPS_ALPHAPALETTE", D3DPTEXTURECAPS_ALPHAPALETTE },
	{ "D3DPTEXTURECAPS_NONPOW2CONDITIONAL", D3DPTEXTURECAPS_NONPOW2CONDITIONAL },
	{ "D3DPTEXTURECAPS_PROJECTED", D3DPTEXTURECAPS_PROJECTED },
	{ "D3DPTEXTURECAPS_CUBEMAP", D3DPTEXTURECAPS_CUBEMAP },
	{ "D3DPTEXTURECAPS_COLORKEYBLEND", D3DPTEXTURECAPS_COLORKEYBLEND},
	{ "D3DPTEXTURECAPS_VOLUMEMAP", D3DPTEXTURECAPS_VOLUMEMAP },
	{ "D3DPTEXTURECAPS_MIPMAP", D3DPTEXTURECAPS_MIPMAP },
	{ "D3DPTEXTURECAPS_MIPVOLUMEMAP", D3DPTEXTURECAPS_MIPVOLUMEMAP },
	{ "D3DPTEXTURECAPS_MIPCUBEMAP", D3DPTEXTURECAPS_MIPCUBEMAP },
	{ "D3DPTEXTURECAPS_CUBEMAP_POW2", D3DPTEXTURECAPS_CUBEMAP_POW2 },
	{ "D3DPTEXTURECAPS_VOLUMEMAP_POW2", D3DPTEXTURECAPS_VOLUMEMAP_POW2 },
	{ "D3DPTEXTURECAPS_NOPROJECTEDBUMPENV", D3DPTEXTURECAPS_NOPROJECTEDBUMPENV },
};

CapStruct TextureFilterCap[] =
{
	{ "D3DPTFILTERCAPS_NEAREST", D3DPTFILTERCAPS_NEAREST },
	{ "D3DPTFILTERCAPS_LINEAR", D3DPTFILTERCAPS_LINEAR },
	{ "D3DPTFILTERCAPS_MIPNEAREST", D3DPTFILTERCAPS_MIPNEAREST },
	{ "D3DPTFILTERCAPS_MIPLINEAR", D3DPTFILTERCAPS_MIPLINEAR },
	{ "D3DPTFILTERCAPS_LINEARMIPNEAREST", D3DPTFILTERCAPS_LINEARMIPNEAREST },
	{ "D3DPTFILTERCAPS_LINEARMIPLINEAR", D3DPTFILTERCAPS_LINEARMIPLINEAR },
	{ "D3DPTFILTERCAPS_MINFPOINT", D3DPTFILTERCAPS_MINFPOINT },
	{ "D3DPTFILTERCAPS_MINFLINEAR", D3DPTFILTERCAPS_MINFLINEAR },
	{ "D3DPTFILTERCAPS_MINFANISOTROPIC", D3DPTFILTERCAPS_MINFANISOTROPIC },
	{ "D3DPTFILTERCAPS_MINFPYRAMIDALQUAD", D3DPTFILTERCAPS_MINFPYRAMIDALQUAD },
	{ "D3DPTFILTERCAPS_MINFGAUSSIANQUAD", D3DPTFILTERCAPS_MINFGAUSSIANQUAD }, 
	{ "D3DPTFILTERCAPS_MIPFPOINT", D3DPTFILTERCAPS_MIPFPOINT },
	{ "D3DPTFILTERCAPS_MIPFLINEAR", D3DPTFILTERCAPS_MIPFLINEAR },
	{ "D3DPTFILTERCAPS_CONVOLUTIONMONO", D3DPTFILTERCAPS_CONVOLUTIONMONO },
	{ "D3DPTFILTERCAPS_MAGFPOINT", D3DPTFILTERCAPS_MAGFPOINT },
	{ "D3DPTFILTERCAPS_MAGFLINEAR", D3DPTFILTERCAPS_MAGFLINEAR },
	{ "D3DPTFILTERCAPS_MAGFANISOTROPIC", D3DPTFILTERCAPS_MAGFANISOTROPIC },
	{ "D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD", D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD },
	//{ "D3DPTFILTERCAPS_MAGFAFLATCUBIC", D3DPTFILTERCAPS_MAGFAFLATCUBIC },
	{ "D3DPTFILTERCAPS_MAGFGAUSSIANQUAD", D3DPTFILTERCAPS_MAGFGAUSSIANQUAD }
	//{ "D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC", D3DPTFILTERCAPS_MAGFGAUSSIANCUBIC }
};

CapStruct StretchFilterCap[] =
{
	{ "D3DPTFILTERCAPS_MINFPOINT", D3DPTFILTERCAPS_MINFPOINT },
	{ "D3DPTFILTERCAPS_MINFLINEAR", D3DPTFILTERCAPS_MINFLINEAR },
	{ "D3DPTFILTERCAPS_MAGFPOINT", D3DPTFILTERCAPS_MAGFPOINT },
	{ "D3DPTFILTERCAPS_MAGFLINEAR", D3DPTFILTERCAPS_MAGFLINEAR }
};

CapStruct TextureBlendCap[] =
{
	{ "D3DPTBLENDCAPS_DECAL", D3DPTBLENDCAPS_DECAL },
	{ "D3DPTBLENDCAPS_MODULATE", D3DPTBLENDCAPS_MODULATE },
	{ "D3DPTBLENDCAPS_DECALALPHA", D3DPTBLENDCAPS_DECALALPHA },
	{ "D3DPTBLENDCAPS_MODULATEALPHA", D3DPTBLENDCAPS_MODULATEALPHA },
	{ "D3DPTBLENDCAPS_DECALMASK", D3DPTBLENDCAPS_DECALMASK },
	{ "D3DPTBLENDCAPS_MODULATEMASK", D3DPTBLENDCAPS_MODULATEMASK },
	{ "D3DPTBLENDCAPS_COPY", D3DPTBLENDCAPS_COPY },
	{ "D3DPTBLENDCAPS_ADD", D3DPTBLENDCAPS_ADD }
};

CapStruct TextureAddressCap[]  =
{
	{ "D3DPTADDRESSCAPS_WRAP", D3DPTADDRESSCAPS_WRAP },
	{ "D3DPTADDRESSCAPS_MIRROR", D3DPTADDRESSCAPS_MIRROR },
	{ "D3DPTADDRESSCAPS_CLAMP", D3DPTADDRESSCAPS_CLAMP },
	{ "D3DPTADDRESSCAPS_BORDER", D3DPTADDRESSCAPS_BORDER },
	{ "D3DPTADDRESSCAPS_INDEPENDENTUV", D3DPTADDRESSCAPS_INDEPENDENTUV },
	{ "D3DPTADDRESSCAPS_MIRRORONCE", D3DPTADDRESSCAPS_MIRRORONCE }
};

CapStruct StencilCap[] =
{
	{ "D3DSTENCILCAPS_KEEP", D3DSTENCILCAPS_KEEP },
	{ "D3DSTENCILCAPS_ZERO", D3DSTENCILCAPS_ZERO },
	{ "D3DSTENCILCAPS_REPLACE", D3DSTENCILCAPS_REPLACE }, 
	{ "D3DSTENCILCAPS_INCRSAT", D3DSTENCILCAPS_INCRSAT },
	{ "D3DSTENCILCAPS_DECRSAT", D3DSTENCILCAPS_DECRSAT },
	{ "D3DSTENCILCAPS_INVERT", D3DSTENCILCAPS_INVERT },
	{ "D3DSTENCILCAPS_INCR", D3DSTENCILCAPS_INCR },
	{ "D3DSTENCILCAPS_DECR", D3DSTENCILCAPS_DECR },
	{ "D3DSTENCILCAPS_TWOSIDED", D3DSTENCILCAPS_TWOSIDED  }
};

CapStruct TextureOpCap[] =
{
	{ "D3DTEXOPCAPS_DISABLE", D3DTEXOPCAPS_DISABLE },
	{ "D3DTEXOPCAPS_SELECTARG1", D3DTEXOPCAPS_SELECTARG1 },
	{ "D3DTEXOPCAPS_SELECTARG2", D3DTEXOPCAPS_SELECTARG2 },
	{ "D3DTEXOPCAPS_MODULATE", D3DTEXOPCAPS_MODULATE },
	{ "D3DTEXOPCAPS_MODULATE2X", D3DTEXOPCAPS_MODULATE2X },
	{ "D3DTEXOPCAPS_MODULATE4X", D3DTEXOPCAPS_MODULATE4X },
	{ "D3DTEXOPCAPS_ADD", D3DTEXOPCAPS_ADD },
	{ "D3DTEXOPCAPS_ADDSIGNED", D3DTEXOPCAPS_ADDSIGNED },
	{ "D3DTEXOPCAPS_ADDSIGNED2X", D3DTEXOPCAPS_ADDSIGNED2X },
	{ "D3DTEXOPCAPS_SUBTRACT", D3DTEXOPCAPS_SUBTRACT },
	{ "D3DTEXOPCAPS_ADDSMOOTH", D3DTEXOPCAPS_ADDSMOOTH },
	{ "D3DTEXOPCAPS_BLENDDIFFUSEALPHA", D3DTEXOPCAPS_BLENDDIFFUSEALPHA },
	{ "D3DTEXOPCAPS_BLENDTEXTUREALPHA", D3DTEXOPCAPS_BLENDTEXTUREALPHA },
	{ "D3DTEXOPCAPS_BLENDFACTORALPHA", D3DTEXOPCAPS_BLENDFACTORALPHA },
	{ "D3DTEXOPCAPS_BLENDTEXTUREALPHAPM", D3DTEXOPCAPS_BLENDTEXTUREALPHAPM },
	{ "D3DTEXOPCAPS_BLENDCURRENTALPHA", D3DTEXOPCAPS_BLENDCURRENTALPHA },
	{ "D3DTEXOPCAPS_PREMODULATE", D3DTEXOPCAPS_PREMODULATE },
	{ "D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR", D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR },
	{ "D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA", D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA },
	{ "D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR", D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR },
	{ "D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA", D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA },
	{ "D3DTEXOPCAPS_BUMPENVMAP", D3DTEXOPCAPS_BUMPENVMAP },
	{ "D3DTEXOPCAPS_BUMPENVMAPLUMINANCE", D3DTEXOPCAPS_BUMPENVMAPLUMINANCE },
	{ "D3DTEXOPCAPS_DOTPRODUCT3", D3DTEXOPCAPS_DOTPRODUCT3 },
	{ "D3DTEXOPCAPS_MULTIPLYADD", D3DTEXOPCAPS_MULTIPLYADD },
	{ "D3DTEXOPCAPS_LERP", D3DTEXOPCAPS_LERP }
};

CapStruct FVFCap[] =
{
	{ "D3DFVFCAPS_TEXCOORDCOUNTMASK", D3DFVFCAPS_TEXCOORDCOUNTMASK },
	{ "D3DFVFCAPS_DONOTSTRIPELEMENTS", D3DFVFCAPS_DONOTSTRIPELEMENTS },
	{ "D3DFVFCAPS_PSIZE", D3DFVFCAPS_PSIZE } 
};

CapStruct VertexProcessCap[] =
{
	{ "D3DVTXPCAPS_TEXGEN", D3DVTXPCAPS_TEXGEN },
	{ "D3DVTXPCAPS_MATERIALSOURCE7", D3DVTXPCAPS_MATERIALSOURCE7 },
	{ "D3DVTXPCAPS_VERTEXFOG", D3DVTXPCAPS_VERTEXFOG },
	{ "D3DVTXPCAPS_DIRECTIONALLIGHTS", D3DVTXPCAPS_DIRECTIONALLIGHTS },
	{ "D3DVTXPCAPS_POSITIONALLIGHTS", D3DVTXPCAPS_POSITIONALLIGHTS },
	{ "D3DVTXPCAPS_LOCALVIEWER", D3DVTXPCAPS_LOCALVIEWER },
	{ "D3DVTXPCAPS_TWEENING", D3DVTXPCAPS_TWEENING },
	{ "D3DVTXPCAPS_NO_VSDT_UBYTE4", D3DVTXPCAPS_NO_VSDT_UBYTE4 },
	{ "D3DVTXPCAPS_TEXGEN_SPHEREMAP", D3DVTXPCAPS_TEXGEN_SPHEREMAP },
	{ "D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER", D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER }
};

CapStruct Dev2Cap[] =
{
	{ "D3DDEVCAPS2_STREAMOFFSET", D3DDEVCAPS2_STREAMOFFSET },
	{ "D3DDEVCAPS2_DMAPNPATCH", D3DDEVCAPS2_DMAPNPATCH },
	{ "D3DDEVCAPS2_ADAPTIVETESSRTPATCH", D3DDEVCAPS2_ADAPTIVETESSRTPATCH },
	{ "D3DDEVCAPS2_ADAPTIVETESSNPATCH", D3DDEVCAPS2_ADAPTIVETESSNPATCH },
	{ "D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES", D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES },
	{ "D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH", D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH },
	{ "D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET", D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET }
};

CapStruct DeclTypeCap[] =
{
	{ "D3DDTCAPS_UBYTE4", D3DDTCAPS_UBYTE4 },
	{ "D3DDTCAPS_UBYTE4N", D3DDTCAPS_UBYTE4N },
	{ "D3DDTCAPS_SHORT2N", D3DDTCAPS_SHORT2N },
	{ "D3DDTCAPS_SHORT4N", D3DDTCAPS_SHORT4N },
	{ "D3DDTCAPS_USHORT2N", D3DDTCAPS_USHORT2N },
	{ "D3DDTCAPS_USHORT4N", D3DDTCAPS_USHORT4N },
	{ "D3DDTCAPS_UDEC3", D3DDTCAPS_UDEC3 },
	{ "D3DDTCAPS_DEC3N", D3DDTCAPS_DEC3N },
	{ "D3DDTCAPS_FLOAT16_2", D3DDTCAPS_FLOAT16_2 },
	{ "D3DDTCAPS_FLOAT16_4", D3DDTCAPS_FLOAT16_4  }
};

CapStruct LightModelCap[] =
{
	{ "D3DLIGHTINGMODEL_RGB", D3DLIGHTINGMODEL_RGB },
	{ "D3DLIGHTINGMODEL_MONO", D3DLIGHTINGMODEL_MONO },
};

CapStruct TransformCap[] = 
{
	{ "D3DTRANSFORMCAPS_CLIP", D3DTRANSFORMCAPS_CLIP }
};

CapStruct ColorModelCap[] =
{
	{ "D3DCOLOR_MONO", D3DCOLOR_MONO },
	{ "D3DCOLOR_RGB", D3DCOLOR_RGB }
};

CapStruct FlagCap[] =
{
	{ "D3DDD_COLORMODEL", D3DDD_COLORMODEL },
	{ "D3DDD_DEVCAPS", D3DDD_DEVCAPS },
	{ "D3DDD_TRANSFORMCAPS", D3DDD_TRANSFORMCAPS },
	{ "D3DDD_LIGHTINGCAPS", D3DDD_LIGHTINGCAPS },
	{ "D3DDD_BCLIPPING", D3DDD_LIGHTINGCAPS },
	{ "D3DDD_LINECAPS", D3DDD_LINECAPS },
	{ "D3DDD_TRICAPS", D3DDD_TRICAPS },
	{ "D3DDD_DEVICERENDERBITDEPTH", D3DDD_DEVICERENDERBITDEPTH },
	{ "D3DDD_DEVICEZBUFFERBITDEPTH", D3DDD_DEVICEZBUFFERBITDEPTH },
	{ "D3DDD_MAXBUFFERSIZE", D3DDD_MAXBUFFERSIZE },
	{ "D3DDD_MAXVERTEXCOUNT", D3DDD_MAXBUFFERSIZE }
};

CapStruct LightCap[] = 
{
	{ "D3DLIGHTCAPS_POINT", D3DLIGHTCAPS_POINT },
	{ "D3DLIGHTCAPS_SPOT", D3DLIGHTCAPS_SPOT },
	{ "D3DLIGHTCAPS_DIRECTIONAL", D3DLIGHTCAPS_DIRECTIONAL },
	{ "D3DLIGHTCAPS_PARALLELPOINT", D3DLIGHTCAPS_PARALLELPOINT }
};

CapStruct BitDepthCap[] =
{
	{ "DDBD_8", DDBD_8 },
	{ "DDBD_16", DDBD_16 },
	{ "DDBD_24", DDBD_24 },
	{ "DDBD_32", DDBD_32 }
};

UINT VertexShaderCapSize =		sizeof( VertexShaderCap ) / sizeof( CapStruct );
UINT PixelShaderCapSize	=		sizeof( PixelShaderCap ) / sizeof( CapStruct );
UINT CapsCapSize =				sizeof( CapsCap ) / sizeof( CapStruct );
UINT Caps2CapSize =				sizeof( Caps2Cap ) / sizeof( CapStruct );
UINT Caps3CapSize =				sizeof( Caps3Cap ) / sizeof( CapStruct );
UINT PresentCapSize	=			sizeof( PresentCap ) / sizeof( CapStruct );
UINT CursorCapSize =			sizeof( CursorCap ) / sizeof( CapStruct );
UINT DevCapSize =				sizeof( DevCap ) / sizeof( CapStruct );
UINT MiscPrimitiveCapSize =		sizeof( MiscPrimitiveCap ) / sizeof( CapStruct );
UINT LineCapSize =				sizeof( LineCap ) / sizeof( CapStruct );
UINT RasterCapSize =			sizeof( RasterCap ) / sizeof( CapStruct );
UINT AlphaZCmpCapSize =			sizeof( AlphaZCmpCap ) / sizeof( CapStruct );
UINT DestSrcBlendCapSize =		sizeof( DestSrcBlendCap ) / sizeof( CapStruct );
UINT ShadeCapSize =				sizeof( ShadeCap ) / sizeof( CapStruct );
UINT TextureCapSize =			sizeof( TextureCap ) / sizeof( CapStruct );
UINT TextureFilterCapSize =		sizeof( TextureFilterCap ) / sizeof( CapStruct );
UINT StretchFilterCapSize =		sizeof( StretchFilterCap ) / sizeof( CapStruct );
UINT TextureBlendCapSize =		sizeof( TextureBlendCap ) / sizeof( CapStruct );
UINT TextureAddressCapSize =	sizeof( TextureAddressCap ) / sizeof( CapStruct );
UINT StencilCapSize =			sizeof( StencilCap ) / sizeof( CapStruct );
UINT TextureOpCapSize =			sizeof( TextureOpCap ) / sizeof( CapStruct );
UINT FVFCapSize =				sizeof( FVFCap ) / sizeof( CapStruct );
UINT VertexProcessCapSize =		sizeof( VertexProcessCap ) / sizeof( CapStruct );
UINT Dev2CapSize =				sizeof( Dev2Cap ) / sizeof( CapStruct );
UINT DeclTypeCapSize =			sizeof( DeclTypeCap ) / sizeof( CapStruct );
UINT LightModelCapSize =		sizeof( LightModelCap ) / sizeof( CapStruct );
UINT TransformCapSize =			sizeof( TransformCap ) / sizeof( CapStruct );
UINT ColorModelCapSize =		sizeof( ColorModelCap ) / sizeof( CapStruct );
UINT FlagCapSize =				sizeof( FlagCap ) / sizeof( CapStruct );
UINT LightCapSize =				sizeof( LightCap ) / sizeof( CapStruct );
UINT BitDepthCapSize =			sizeof( BitDepthCap ) / sizeof( CapStruct );