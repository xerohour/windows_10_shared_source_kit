// FILE:        CapsCheck.h
// DESC:        CapsCheck class header for D3D caps conformance test
// AUTHOR:      Robert Rodrigues

#define DIRECT3D_VERSION         0x0900

#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"
#include "d3dtypes.h"
#include "d3d9types.h"
#include "d3d9caps.h"
#include "d3d8caps.h"
#include "d3dcaps.h"

#pragma once

const UINT MAXBUFFER = 512;

enum eCapType
{
	CT_VertexShader,
	CT_PixelShader,
	CT_Caps,
	CT_Caps2,
	CT_Caps3,
	CT_Present,
	CT_Cursor,
	CT_Dev,
	CT_MiscPrimitive,
	CT_Line,
	CT_Raster,
	CT_AlphaZCmp,
	CT_DestSrcBlend,
	CT_Shade,
	CT_Texture,
	CT_TextureFilter,
	CT_StretchFilter,
	CT_TextureBlend,
	CT_TextureAddress,
	CT_Stencil,
	CT_TextureOp,
	CT_FVF,
	CT_VertexProcess,
	CT_Dev2,
	CT_DeclType,
	CT_LightModel,
	CT_Transform,
	CT_ColorModel,
	CT_Flag,
	CT_Light,
	CT_BitDepth,
};

class CCaps: public CD3DTest
{
private:
	bool ParseCaps( DWORD dwValue, eCapType CapName, char * szCaps );
protected:
	DEVICEDESC * m_pDeviceDesc;
	bool m_bFail;
	bool m_bWarn;
	bool m_bUnidentified;
public:
	bool ExecuteTest( UINT );
	void LogInvalidCapValue( DWORD dwValue, eCapType CapName, char * szValueName, bool bFailure );
	UINT TestInitialize(void) { return D3DTESTINIT_RUN; }
    CCaps();
    ~CCaps();

	bool CapsCheck( void );
};

//X = CAP being tested, Y=Bitmask to test it with, Z=Name of the cap, W=Cap enum
#define MASK_TEST( x, y, z, w ) \
	if(dwCurrentCompare=( ( x ) & ( y ) ) )\
	{\
		bRetVal = false;\
		LogInvalidCapValue( dwCurrentCompare, w, z, true );\
	}\

//X = CAP being tested, Y=Bitmask to test it with, Z=Name of the cap, W=Cap enum
#define MASK_WARN( x, y, z, w ) \
	if(dwCurrentCompare=( ( x ) & ( y ) ) )\
	{\
		m_bWarn = true;\
		LogInvalidCapValue( dwCurrentCompare, w, z, false );\
	}\

class ConsistencyTestGroup: public CCaps 	//Test group to check Caps consitency
{
public:
	ConsistencyTestGroup();	
	bool ExecuteTest( UINT );
    
private:
	bool AnisotropyCheck();
	bool CubeCheck();
	bool FilterCheck();
	bool FogCheck();
	bool MipCubeCheck();
	bool MipVolumeCheck();
	bool VolumeCheck();
	bool TextureCheck();
	bool LineCheck();
	bool MultisampleStencilCheck();
	bool StencilCheck();
	bool Pow2Check();
	bool VertexShaderCheck();
	bool PixelShaderCheck();
	
	bool DX6Consistency();
	bool DX7Consistency();
	bool DX8Consistency();
	bool DX9Consistency();
};

class RequiredTestGroup: public CCaps 		//Test group to check required caps
{
public:
	VOID CommandLineHelp(VOID);
	RequiredTestGroup();
	bool ExecuteTest( UINT );
};

class VersionMatchTestGroup: public CCaps 	//Test group to ensure the caps match the DX version
{
public:
	VersionMatchTestGroup();
	bool ExecuteTest( UINT );

private:
	bool DX6Match();
	bool DX7Match();
	bool DX8Match();
	bool DX9Match();
};