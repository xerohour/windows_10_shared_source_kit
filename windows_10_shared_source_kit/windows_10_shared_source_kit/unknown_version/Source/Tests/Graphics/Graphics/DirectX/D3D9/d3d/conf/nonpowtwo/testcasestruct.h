#ifndef __TESTCASESTRUCT_H
#define __TESTCASESTRUCT_H

#include "d3dx9.h"
#include "frameworktypes.h"

//#define FILTER_TEXTURE_COUNT 3
//#define MIPBIASSTAGES 1
//#define TEXTURE_ADDRESS_MODE_COUNT 2
#define MAX_TEXTURE_COUNT 3
#define TEXTURE_COUNT 6
#define NAME_SIZE 150

enum TEXTURE_TYPE
{
	TEXTURE_2D   = 0,
	TEXTURE_CUBE = 1,
	TEXTURE_3D   = 2,
};

struct STextureInfo
{
	TEXTURE_TYPE	TexType;
	FORMAT          Format;
	DWORD           Pool;
	UINT			uWidth;
	UINT			uHeight;
	UINT			uDepth;
	UINT			uMipCount;
};

struct STextureCase
{
	int		TexIndices  [MAX_TEXTURE_COUNT]; //indices into an array of STextureInfo.  An index of <0 means set no texture
	int     StageIndices[MAX_TEXTURE_COUNT]; //indices indicating on which stage o set up texture    
};

struct SAlphaBlendCase
{
	D3DBLEND	SrcBlend;
	D3DBLEND	DestBlend;
	DWORD		dwSrcRequiredCaps;
	DWORD		dwDestRequiredCaps;
};

struct SFilterCase
{
	 D3DTEXTUREFILTERTYPE	MagFilter;//[MAX_TEXTURE_COUNT];
	 D3DTEXTUREFILTERTYPE	MinFilter;//[MAX_TEXTURE_COUNT];
	 D3DTEXTUREFILTERTYPE	MipFilter;//[MAX_TEXTURE_COUNT];
	 DWORD					dwRequiredCaps;//[MAX_TEXTURE_COUNT];
};

struct STextureOpCase
{
	 D3DTEXTUREOP	TextureOp;
	 DWORD		    dwRequiredCaps;
};

struct SLODBiasCase
{
	float LODBias[MAX_TEXTURE_COUNT];
};

struct SAddressModeCase
{
	 D3DTEXTUREADDRESS	AddressModeU[MAX_TEXTURE_COUNT];
	 D3DTEXTUREADDRESS	AddressModeV[MAX_TEXTURE_COUNT];
	 D3DTEXTUREADDRESS	AddressModeW[MAX_TEXTURE_COUNT];
	 DWORD				dwRequiredCapsUV[MAX_TEXTURE_COUNT];
	 DWORD				dwRequiredCapsW [MAX_TEXTURE_COUNT]; //need to keep it separatly in case of 2d textures
};

struct SVShaderCase
{
	bool	      bDecl;           //if true then use declorations
	bool		  bVShader;        //if true then use vertex shader
	bool          bPShader;        //if true then use pixel shader
	DWORD	      dwTexCoordIndex; //index of the texture coordinates | texgen
	const TCHAR*  szTexCoordIndex; //string representation of dwTexCoordIndex
	DWORD	      dwVPCaps;
};

struct SWrapCase
{
	DWORD dwWrapParams[MAX_TEXTURE_COUNT];
};

extern STextureInfo   g_TexInfo  [TEXTURE_COUNT];

extern CnBaseTexture *g_pTextures[TEXTURE_COUNT];

extern UINT g_uTexCaseIndex;

extern STextureCase g_TextureCases[];

extern UINT g_MaxStageCount;
extern bool g_bUseTL;
extern bool g_isVShaderCase;
extern bool g_isWrapCase;
extern bool g_isZeroDiffuse;
extern bool g_isHalfDiffuse;

#endif