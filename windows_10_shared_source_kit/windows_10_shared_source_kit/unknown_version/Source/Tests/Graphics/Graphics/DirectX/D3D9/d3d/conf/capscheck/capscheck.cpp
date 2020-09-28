// FILE:        CapsCheck.cpp
// DESC:        CapsCheck class methods for D3D caps conformance test
// AUTHOR:      Robert Rodrigues
// COMMENTS:    Determines the existence of required caps, that all caps are consistent,
//				and that they match the correct DX version.

#include "CapsCheck.h"
#include "CapStructs.h"
#include "strsafe.h"

#define SZ_BUF_SIZE 512

CD3DWindowFramework App;		// global instance of D3D window framework class

ConsistencyTestGroup	ConsistencyTest;
RequiredTestGroup		RequiredTest;
VersionMatchTestGroup	VersionTest;

CCaps::CCaps()
{
	m_bShowCompareWindow = false;
	m_bGenGolden = false;

	SetTestType( TESTTYPE_CONF );
	SetTestRange( 1, 1 );

	m_Options.D3DOptions.bReference = false;
	m_Options.D3DOptions.dwIncVersions = DXVERSION_91 | DXVERSION_9 | DXVERSION_8_OR_81 | DXVERSION_7;
}

CCaps::~CCaps()
{
}

bool CCaps::ExecuteTest( UINT uTest )
{
	if ( m_bFail )
		Fail();
	else if ( m_bWarn )
	{
		if ( KeySet( "WHQL" ) )
		{
			//For WHQL skip instead of warn.  This prevents weird results from showing up in their shell
			Skip();
		}
		else
            Warn();
	}

	return !m_bFail;
}
bool CCaps::CapsCheck()
{
	bool bReturn = false;
	m_pDeviceDesc = NULL;
	
	m_pDeviceDesc = m_pSrcDevice->GetCaps();
    
	if ( m_pDeviceDesc ) 
	{
		//We have received caps, continue
		bReturn = true;
	}

	return bReturn;
}

void CCaps::LogInvalidCapValue( DWORD dwValue, eCapType CapName, char * szValueName, bool bFailure )
{
	char szValues[SZ_BUF_SIZE] = "\0";

	if ( ParseCaps( dwValue, CapName, szValues ) )
		bFailure = true;

	if ( bFailure )
		WriteToLog( "DX version: %#0x\nCap failure: %s%s\n\n", m_dwVersion, szValueName, szValues );
	else
		WriteToLog( "DX version: %#0x\nCap warning: %s%s\n\n", m_dwVersion, szValueName, szValues );
}

bool CCaps::ParseCaps( DWORD dwValue, eCapType CapName, char * szCaps )
{
	bool bForceFailure = false;
	bool bFound = false;
	HRESULT hr = S_OK;

	pCapStruct pCaps = NULL;
	UINT uMaxValue = 0;

	switch ( CapName )
	{
		case CT_VertexShader:
			uMaxValue = VertexShaderCapSize;
			pCaps = VertexShaderCap;
			break;
		case CT_PixelShader:
			uMaxValue = PixelShaderCapSize;
			pCaps = PixelShaderCap;
			break;
		case CT_Caps:
			uMaxValue = CapsCapSize;
			pCaps = CapsCap;
			break;
		case CT_Caps2:
			uMaxValue = Caps2CapSize;
			pCaps = Caps2Cap;
			break;
		case CT_Caps3:
			uMaxValue = Caps3CapSize;
			pCaps = Caps3Cap;
			break;
		case CT_Present:
			uMaxValue = PresentCapSize;
			pCaps = PresentCap;
			break;
		case CT_Cursor:
			uMaxValue = CursorCapSize;
			pCaps = CursorCap;
			break;
		case CT_Dev:
			uMaxValue = DevCapSize;
			pCaps = DevCap;
			break;
		case CT_MiscPrimitive:
			uMaxValue = MiscPrimitiveCapSize;
			pCaps = MiscPrimitiveCap;
			break;
		case CT_Line:
			uMaxValue = LineCapSize;
			pCaps = LineCap;
			break;
		case CT_Raster:
			uMaxValue = RasterCapSize;
			pCaps = RasterCap;
			break;
		case CT_AlphaZCmp:
			uMaxValue = AlphaZCmpCapSize;
			pCaps = AlphaZCmpCap;
			break;
		case CT_DestSrcBlend:
			uMaxValue = DestSrcBlendCapSize;
			pCaps = DestSrcBlendCap;
			break;
		case CT_Shade:
			uMaxValue = ShadeCapSize;
			pCaps = ShadeCap;
			break;
		case CT_Texture:
			uMaxValue = TextureCapSize;
			pCaps = TextureCap;
			break;
		case CT_TextureFilter:
			uMaxValue = TextureFilterCapSize;
			pCaps = TextureFilterCap;
			break;
		case CT_StretchFilter:
			uMaxValue = StretchFilterCapSize;
			pCaps = StretchFilterCap;
			break;
		case CT_TextureBlend:
			uMaxValue = TextureBlendCapSize;
			pCaps = TextureBlendCap;
			break;
		case CT_TextureAddress:
			uMaxValue = TextureAddressCapSize;
			pCaps = TextureAddressCap;
			break;
		case CT_Stencil:
			uMaxValue = StencilCapSize;
			pCaps = StencilCap;
			break;
		case CT_TextureOp:
			uMaxValue = TextureOpCapSize;
			pCaps = TextureOpCap;
			break;
		case CT_FVF:
			uMaxValue = FVFCapSize;
			pCaps = FVFCap;
			break;
		case CT_VertexProcess:
			uMaxValue = VertexProcessCapSize;
			pCaps = VertexProcessCap;
			break;
		case CT_Dev2:
			uMaxValue = Dev2CapSize;
			pCaps = Dev2Cap;
			break;
		case CT_DeclType:
			uMaxValue = DeclTypeCapSize;
			pCaps = DeclTypeCap;
			break;
		case CT_LightModel:
			uMaxValue = LightModelCapSize;
			pCaps = LightModelCap;
			break;
		case CT_Transform:
			uMaxValue = TransformCapSize;
			pCaps = TransformCap;
			break;
		case CT_ColorModel:
			uMaxValue = ColorModelCapSize;
			pCaps = ColorModelCap;
			break;
		case CT_Flag:
			uMaxValue = FlagCapSize;
			pCaps = FlagCap;
			break;
		case CT_Light:
			uMaxValue = LightCapSize;
			pCaps = LightCap;
			break;
		case CT_BitDepth:
			uMaxValue = BitDepthCapSize;
			pCaps = BitDepthCap;
			break;
		default:
			WriteToLog( "TEST FAILURE: unidentified cap section." ) ;
			return true;
	}

	for ( UINT uCounter = 0; uCounter < uMaxValue; uCounter++ )
	{
		//If we match a cap value, concatenate it onto our return string.
		if( dwValue & pCaps[uCounter].dwValue )
		{
			if ( !bFound )
			{
				hr = StringCchCat( szCaps, SZ_BUF_SIZE, "\nInvalid value:" );
				bFound = true;
			}

			//Take the value out so we can catch unidentified values
			dwValue ^= pCaps[uCounter].dwValue;
			hr = StringCchCat( szCaps, SZ_BUF_SIZE, " " );
			hr = StringCchCat( szCaps, SZ_BUF_SIZE, pCaps[uCounter].szName );
		}
	}

	//If we still have bits in here
	if ( dwValue )
	{
		char szTemp[SZ_BUF_SIZE];

		hr = StringCchPrintf( szTemp, SZ_BUF_SIZE, "\nUnidentified Values: %#010x", dwValue );
		hr = StringCchCat ( szCaps, SZ_BUF_SIZE, szTemp );

		m_bUnidentified = true;
		bForceFailure = true;
	}

	return bForceFailure;
}