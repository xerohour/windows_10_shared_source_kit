// FILE:        Required.cpp
// DESC:        Required test group class methods for the required D3D caps tests
// AUTHOR:      Robert Rodrigues

#include "CapsCheck.h"

//W = CAP being tested, X=Bitmask to test it with, Y=Name of the bitmask, Z=Name of the cap
#define REQ_TEST( w, x, y, z ) \
	if ( !( ( w ) & ( x ) ) )\
	{\
		m_bFail = true;\
		WriteToLog( "Required Cap Failure: %s not set for %s\n\n", y, z );\
	}

//X = Pass condition, Y = Description of test failure
#define REQ_VALUE_TEST( x, y ) \
	if ( ! ( x ) ) \
	{\
		m_bFail = true;\
		WriteToLog( "Required Cap Value Failure: %s\n\n", y );\
	}

RequiredTestGroup::RequiredTestGroup()
{
	m_szTestName = "Required Caps Test";
	m_szCommandKey = "Required";
}

// NAME:        CommandLineHelp()
// DESC:        Note the command line options for the test
// INPUT:       none
// OUTPUT:      none

VOID RequiredTestGroup::CommandLineHelp(VOID)
{
	WriteCommandLineHelp("$yMobile: $wSwitch test to mobile WLP requirements.");
}

bool RequiredTestGroup::ExecuteTest( UINT uTest )
{
	m_bFail = false;
	m_bWarn = false;
	bool bFoundA1 = false, bFoundA4 = false, bFoundA8 = false;
	bool bFoundZ1 = false, bFoundZ2 = false;

	BeginTestCase(m_szTestName,uTest);

	//Stencil test
	if ( !KeySet( "mobile" ) || m_pD3D->GetVersion() >= 9.1f )
	{
		for ( UINT i = 0; i< m_uCommonZBufferFormats; i++ )
		{
			if ( m_pD3D->GetVersion() >= 9.1f )
			{
				if ( (UINT)FMT_D24S8 == (UINT)m_pCommonZBufferFormats[i].d3dfFormat )
				{
					bFoundZ1 = true;
				}
				if ( (UINT)FMT_D16 == (UINT)m_pCommonZBufferFormats[i].d3dfFormat )
				{
					bFoundZ2 = true;
				}
			}
			else
			{
				if ( (UINT)FMT_D24S8 == (UINT)m_pCommonZBufferFormats[i].d3dfFormat )
				{
					bFoundZ1 = true;
					break;
				}
				if ( (UINT)FMT_S8D24 == (UINT)m_pCommonZBufferFormats[i].d3dfFormat )
				{
					bFoundZ1 = true;
					break;
				}
			}
		}

		if ( m_pD3D->GetVersion() >= 9.1f )
		{
			if ( !bFoundZ1 )
			{
				m_bFail = true;
				WriteToLog( "Required z-buffer support failure: D3DFMT_D24S8 required.\n\n" );
			}
			if ( !bFoundZ2 )
			{
				m_bFail = true;
				WriteToLog( "Required z-buffer support failure: D3DFMT_D16 required.\n\n" );
			}
		}
		else if ( !bFoundZ1 )
		{
			m_bFail = true;
			WriteToLog( "Required z-buffer support failure: 24 bits for depth and 8 bits for stencil are required.\n\n" );
		}
	}
	else
	{
		for ( UINT i = 0; i< m_uCommonZBufferFormats; i++ )
		{
			if ( (UINT)FMT_D16 == (UINT)m_pCommonZBufferFormats[i].d3dfFormat || 
				 (UINT)FMT_D15S1 == (UINT)m_pCommonZBufferFormats[i].d3dfFormat || 
				 (UINT)FMT_D16_LOCKABLE == (UINT)m_pCommonZBufferFormats[i].d3dfFormat )
			{
				bFoundZ1 = true;
				break;
			}
		}

		if ( !bFoundZ1 )
		{
			m_bFail = true;
			WriteToLog( "Required texture support failure: D3DFMT_D16, D3DFMT_D15S1, or D3DFMT_D16_LOCKABLE is required.\n\n" );
		}
	}
	
	//Required texture support test
	switch( m_dwVersion )
	{
	case 0x600:
	case 0x700:
		for ( UINT i = 0; i< m_uCommonTextureFormats; i++ )
		{
			if ( (UINT)FMT_A1R5G5B5 == (UINT)m_pCommonTextureFormats[i].d3dfFormat )
			{
				bFoundA1 = true;
			}
			else if ( (UINT)FMT_A4R4G4B4 == (UINT)m_pCommonTextureFormats[i].d3dfFormat )
			{
				bFoundA4 = true;
			}
			else if ( (UINT)FMT_A8R8G8B8 == (UINT)m_pCommonTextureFormats[i].d3dfFormat )
			{
				bFoundA8 = true;
			}
		}
		if ( !bFoundA1 )
		{
			m_bFail = true;
			WriteToLog( "Required texture support failure: D3DFMT_A1R5G5B5 is required unless integrated part.\n\n" );
		}
		if ( !bFoundA4 )
		{
			m_bFail = true;
			WriteToLog( "Required texture support failure: D3DFMT_A4R4G4B4 is required unless integrated part.\n\n" );
		}
		if ( !KeySet( "mobile" ) )
		{
			if ( !bFoundA8 )
			{
				m_bFail = true;
				WriteToLog( "Required texture support failure: D3DFMT_A8R8G8B8 is required.\n\n" );
			}
		}
		break;
	case 0x800:
	case 0x900:
		FORMAT *pCheckedFmts = NULL;
		FORMAT *pTempFmt = NULL;
		FORMAT *CheckFmt = NULL;
		LPTSTR szFmt[80];

		for ( UINT i = 0; i < m_uCommonDisplayModes; i++ )
		{
			if ( NULL == pCheckedFmts )
			{
				pCheckedFmts = new FORMAT(m_pCommonDisplayModes[i].Format);
				pCheckedFmts->pNext = NULL;
			}
			else
			{
				pTempFmt = pCheckedFmts;
				
				while ( NULL != pTempFmt->pNext )
				{
					if( pTempFmt->d3dfFormat == m_pCommonDisplayModes[i].Format.d3dfFormat )
					{
						break;
					}
					pTempFmt = pTempFmt->pNext;
				}
			

				if( pTempFmt->d3dfFormat == m_pCommonDisplayModes[i].Format.d3dfFormat )
				{
					continue;
				}
				else
				{
					pTempFmt->pNext = new FORMAT(m_pCommonDisplayModes[i].Format);
					pTempFmt->pNext->pNext = NULL;
				}
			}

			FmtToString( FORMAT_TEXTURE, &m_pCommonDisplayModes[i].Format, (LPTSTR)szFmt );
			
			//Don't check for these formats on an integrated part.
			CheckFmt = new FORMAT(FMT_A1R5G5B5);
			if ( FAILED( m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCommonDisplayModes[i].Format,
				0, RTYPE_TEXTURE, *CheckFmt ) ) )
			{
				m_bFail = true;
				WriteToLog( "Required texture support failure: D3DFMT_A1R5G5B5 is required unless integrated part.\n" );
				WriteToLog( "D3DFMT_A1R5G5B5 is unsupported with a display format of: %s .\n\n", szFmt );
			}

			SAFEDELETE( CheckFmt );
			CheckFmt = new FORMAT(FMT_A4R4G4B4);
			if ( FAILED( m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCommonDisplayModes[i].Format,
				0, RTYPE_TEXTURE, *CheckFmt ) ) )
			{
				m_bFail = true;
				WriteToLog( "Required texture support failure: D3DFMT_A4R4G4B4 is required unless integrated part.\n" );
				WriteToLog( "D3DFMT_A4R4G4B4 is unsupported with a display format of: %s .\n\n", szFmt );
			}

			SAFEDELETE( CheckFmt );

			if ( !KeySet( "mobile" ) || m_pD3D->GetVersion() >= 9.1f )
			{
				CheckFmt = new FORMAT(FMT_A8R8G8B8);
				if ( FAILED( m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCommonDisplayModes[i].Format,
					0, RTYPE_TEXTURE, *CheckFmt ) ) )
				{
					m_bFail = true;
					WriteToLog( "Required texture support failure: D3DFMT_A8R8G8B8 is required.\n" );
					WriteToLog( "FMT_A8R8G8B8 is unsupported with a display format of: %s .\n\n", szFmt );
				}
				SAFEDELETE( CheckFmt );
			}

			// 8-bit texture support for Vista
			if ( m_pD3D->GetVersion() >= 9.1f )
			{
				CheckFmt = new FORMAT(FMT_P8);
				if ( FAILED( m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
					m_pCommonDisplayModes[i].Format, 0, RTYPE_TEXTURE, *CheckFmt ) ) )
				{
					SAFEDELETE( CheckFmt );
					CheckFmt = new FORMAT(FMT_A8);
					if ( FAILED( m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
						m_pCommonDisplayModes[i].Format, 0, RTYPE_TEXTURE, *CheckFmt ) ) )
					{
						SAFEDELETE( CheckFmt );
						CheckFmt = new FORMAT(FMT_L8);
						if ( FAILED( m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), 
							m_pCommonDisplayModes[i].Format, 0, RTYPE_TEXTURE, *CheckFmt ) ) )
						{
							m_bFail = true;
							WriteToLog( "Required texture support failure: at least one 8-bit format is required.\n" );
							WriteToLog( "8-bit formats unsupported with a display format of: %s .\n\n", szFmt );
						}
					}
				}
				SAFEDELETE( CheckFmt );
			}
		}
		while ( pCheckedFmts )
		{
			pTempFmt = pCheckedFmts->pNext;
			SAFEDELETE( pCheckedFmts );
			pCheckedFmts = pTempFmt;
		}
	}
	
	//Display mode test
	if ( m_pD3D->GetVersion() >= 9.1f )
	{
		if ( FAILED( m_pD3D->CheckDeviceType( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), FMT_X8R8G8B8, FMT_X8R8G8B8, FALSE ) ) )
		{
			m_bFail = true;
			WriteToLog( "Required fullscreen display mode support failure: D3DFMT_X8R8G8B8.\n\n" );
		}
	}
	
	//Common tests
	REQ_TEST( m_pDeviceDesc->dwTextureOpCaps, D3DTEXOPCAPS_SELECTARG1, "D3DTEXOPCAPS_SELECTARG1", "dwTextureOpCaps" )
	REQ_TEST( m_pDeviceDesc->dwTextureOpCaps, D3DTEXOPCAPS_SELECTARG2, "D3DTEXOPCAPS_SELECTARG2", "dwTextureOpCaps" )

	REQ_TEST( m_pDeviceDesc->dwStencilCaps, D3DSTENCILCAPS_KEEP, "D3DSTENCILCAPS_KEEP", "dwStencilCaps" )
	REQ_TEST( m_pDeviceDesc->dwStencilCaps, D3DSTENCILCAPS_ZERO, "D3DSTENCILCAPS_ZERO", "dwStencilCaps" )
	REQ_TEST( m_pDeviceDesc->dwStencilCaps, D3DSTENCILCAPS_REPLACE, "D3DSTENCILCAPS_REPLACE", "dwStencilCaps" )
	REQ_TEST( m_pDeviceDesc->dwStencilCaps, D3DSTENCILCAPS_INCRSAT, "D3DSTENCILCAPS_INCRSAT", "dwStencilCaps" )
	REQ_TEST( m_pDeviceDesc->dwStencilCaps, D3DSTENCILCAPS_DECRSAT, "D3DSTENCILCAPS_DECRSAT", "dwStencilCaps" )
	REQ_TEST( m_pDeviceDesc->dwStencilCaps, D3DSTENCILCAPS_INVERT, "D3DSTENCILCAPS_INVERT", "dwStencilCaps" )

	REQ_VALUE_TEST( m_pDeviceDesc->dwMaxAnisotropy > 0, "MaxAnisotropy less than 1." )
	REQ_VALUE_TEST( ( m_pDeviceDesc->dwFVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK ) > 1, "Must support more than 1 texture coordinate." )

	//*************************************************************************************************************************************************
	//			From WHQL_WLP 
	REQ_TEST( m_pDeviceDesc->dwTextureOpCaps, D3DTEXOPCAPS_ADD, "D3DTEXOPCAPS_ADD", "dwTextureOpCaps" )
	REQ_TEST( m_pDeviceDesc->dwTextureOpCaps, D3DTEXOPCAPS_MODULATE, "D3DTEXOPCAPS_MODULATE", "dwTextureOpCaps" )
	REQ_TEST( m_pDeviceDesc->dwTextureOpCaps, D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR, "D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR", "dwTextureOpCaps" )
	REQ_TEST( m_pDeviceDesc->dwTextureOpCaps, D3DTEXOPCAPS_BLENDFACTORALPHA, "D3DTEXOPCAPS_BLENDFACTORALPHA", "dwTextureOpCaps" )
	REQ_TEST( m_pDeviceDesc->dwTextureOpCaps, D3DTEXOPCAPS_BLENDDIFFUSEALPHA, "D3DTEXOPCAPS_BLENDDIFFUSEALPHA", "dwTextureOpCaps" )
		
	REQ_VALUE_TEST( m_pDeviceDesc->dwMinTextureWidth < 2, "MinTextureWidth greater than 1." )
	REQ_VALUE_TEST( m_pDeviceDesc->dwMinTextureHeight < 2, "MinTextureHeight greater than 1." )

	if ( KeySet( "mobile") )
	{
		REQ_VALUE_TEST( m_pDeviceDesc->dwMaxTextureWidth > 255, "MaxTextureWidth less than 256." )
		REQ_VALUE_TEST( m_pDeviceDesc->dwMaxTextureHeight > 255, "MaxTextureHeight less than 256." )
	}
	else
	{
        REQ_VALUE_TEST( m_pDeviceDesc->dwMaxTextureWidth > 1023, "MaxTextureWidth less than 1024." )
		REQ_VALUE_TEST( m_pDeviceDesc->dwMaxTextureHeight > 1023, "MaxTextureHeight less than 1024." )
	}

	REQ_VALUE_TEST( m_pDeviceDesc->wMaxSimultaneousTextures > 1, "MaxSimultaneousTextures less than 2." )
	//*************************************************************************************************************************************************

	//Version specific tests
	switch ( m_dwVersion )
	{
	case 0x600:
	case 0x700:
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "D3DPTFILTERCAPS_MINFPOINT", "dpcLineCaps.dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "D3DPTFILTERCAPS_MINFPOINT", "dpcTriCaps.dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "D3DPTFILTERCAPS_MAGFPOINT", "dpcLineCaps.dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "D3DPTFILTERCAPS_MAGFPOINT", "dpcTriCaps.dwTextureFilterCaps" )
		//*************************************************************************************************************************************************
		//			From WHQL_WLP 
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT, "D3DPTFILTERCAPS_MIPFPOINT", "dpcLineCaps.dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT, "D3DPTFILTERCAPS_MIPFPOINT", "dpcTriCaps.dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "D3DPTFILTERCAPS_MINFLINEAR", "dpcLineCaps.dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "D3DPTFILTERCAPS_MINFLINEAR", "dpcTriCaps.dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "D3DPTFILTERCAPS_MAGFLINEAR", "dpcLineCaps.dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "D3DPTFILTERCAPS_MAGFLINEAR", "dpcTriCaps.dwTextureFilterCaps" )
		//*************************************************************************************************************************************************

		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureBlendCaps, D3DPTBLENDCAPS_DECAL, "D3DPTBLENDCAPS_DECAL", "dpcLineCaps.dwTextureBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureBlendCaps, D3DPTBLENDCAPS_DECAL, "D3DPTBLENDCAPS_DECAL", "dpcTriCaps.dwTextureBlendCaps" )
		//*************************************************************************************************************************************************
		//			From WHQL_WLP 
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureBlendCaps, D3DPTBLENDCAPS_MODULATE, "D3DPTBLENDCAPS_MODULATE", "dpcLineCaps.dwTextureBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureBlendCaps, D3DPTBLENDCAPS_MODULATE, "D3DPTBLENDCAPS_MODULATE", "dpcTriCaps.dwTextureBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureBlendCaps, D3DPTBLENDCAPS_MODULATEALPHA, "D3DPTBLENDCAPS_MODULATEALPHA", "dpcLineCaps.dwTextureBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureBlendCaps, D3DPTBLENDCAPS_MODULATEALPHA, "D3DPTBLENDCAPS_MODULATEALPHA", "dpcTriCaps.dwTextureBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureBlendCaps, D3DPTBLENDCAPS_ADD, "D3DPTBLENDCAPS_ADD", "dpcLineCaps.dwTextureBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureBlendCaps, D3DPTBLENDCAPS_ADD, "D3DPTBLENDCAPS_ADD", "dpcTriCaps.dwTextureBlendCaps" )
		//*************************************************************************************************************************************************
		
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwRasterCaps, D3DPRASTERCAPS_SUBPIXEL | D3DPRASTERCAPS_SUBPIXELX, "D3DPRASTERCAPS_SUBPIXEL or D3DPRASTERCAPS_SUBPIXELX", "dpcLineCaps.dwRasterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwRasterCaps, D3DPRASTERCAPS_SUBPIXEL | D3DPRASTERCAPS_SUBPIXELX, "D3DPRASTERCAPS_SUBPIXEL or D3DPRASTERCAPS_SUBPIXELX", "dpcTriCaps.dwRasterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwRasterCaps, D3DPRASTERCAPS_FOGVERTEX, "D3DPRASTERCAPS_FOGVERTEX", "dpcLineCaps.dwRasterCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwRasterCaps, D3DPRASTERCAPS_FOGVERTEX, "D3DPRASTERCAPS_FOGVERTEX", "dpcTriCaps.dwRasterCaps" )

		//*************************************************************************************************************************************************
		//			From WHQL_WLP 
		if ( !( m_pDeviceDesc->dpcLineCaps.dwRasterCaps & D3DPRASTERCAPS_FOGRANGE ) && !KeySet( "mobile" ) )
		{//If we don't have range based fog
			if ( ( m_pDeviceDesc->dpcLineCaps.dwRasterCaps & D3DPRASTERCAPS_FOGTABLE ) )
			{//Check to see that our table has z or w fog	
				REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwRasterCaps, D3DPRASTERCAPS_WFOG | D3DPRASTERCAPS_ZFOG, "D3DPRASTERCAPS_WFOG or D3DPRASTERCAPS_ZFOG", "dpcLineCaps.dwRasterCaps" )
			}
			else
			{//And we don't have depth based fog
				m_bFail = true;
				WriteToLog( "Required Cap Failure: Per-pixel fog is not supported in linecaps.\n\n" );
			}
		}

		if ( !( m_pDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGRANGE ) && !KeySet( "mobile" ) )
		{//If we don't have range based fog
			if ( ( m_pDeviceDesc->dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGTABLE ) )
			{//Check to see that our table has z or w fog				
				REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwRasterCaps, D3DPRASTERCAPS_WFOG | D3DPRASTERCAPS_ZFOG, "D3DPRASTERCAPS_WFOG or D3DPRASTERCAPS_ZFOG", "dpcTriCaps.dwRasterCaps" )
			}
			else
			{//And we don't have depth based fog
				m_bFail = true;
				WriteToLog( "Required Cap Failure: Per-pixel fog is not supported in tricaps.\n\n" );
			}
		}

		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureCaps, D3DPTEXTURECAPS_PERSPECTIVE, "D3DPTEXTURECAPS_PERSPECTIVE", "dpcLineCaps.dwTextureCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureCaps, D3DPTEXTURECAPS_PERSPECTIVE, "D3DPTEXTURECAPS_PERSPECTIVE", "dpcTriCaps.dwTextureCaps" )
		//*************************************************************************************************************************************************
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureCaps, D3DPTEXTURECAPS_TRANSPARENCY, "D3DPTEXTURECAPS_TRANSPARENCY", "dpcLineCaps.dwTextureCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureCaps, D3DPTEXTURECAPS_TRANSPARENCY, "D3DPTEXTURECAPS_TRANSPARENCY", "dpcTriCaps.dwTextureCaps" )

		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwTextureAddressCaps, D3DPTADDRESSCAPS_WRAP, "D3DPTADDRESSCAPS_WRAP", "dpcLineCaps.dwTextureAddressCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwTextureAddressCaps, D3DPTADDRESSCAPS_WRAP, "D3DPTADDRESSCAPS_WRAP", "dpcTriCaps.dwTextureAddressCaps" )

		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwMiscCaps, D3DPMISCCAPS_MASKZ, "D3DPMISCCAPS_MASKZ", "dpcLineCaps.dwMiscCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwMiscCaps, D3DPMISCCAPS_MASKZ, "D3DPMISCCAPS_MASKZ", "dpcTriCaps.dwMiscCaps" )
		
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwMiscCaps, D3DPMISCCAPS_CULLNONE, "D3DPMISCCAPS_CULLNONE", "dpcTriCaps.dwMiscCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwMiscCaps, D3DPMISCCAPS_CULLCW, "D3DPMISCCAPS_CULLCW", "dpcTriCaps.dwMiscCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwMiscCaps, D3DPMISCCAPS_CULLCCW, "D3DPMISCCAPS_CULLCCW", "dpcTriCaps.dwMiscCaps" )

		//*************************************************************************************************************************************************
		//			From WHQL_WLP 
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwShadeCaps, D3DPSHADECAPS_COLORFLATRGB, "D3DPSHADECAPS_COLORFLATRGB", "dpcLineCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwShadeCaps, D3DPSHADECAPS_COLORFLATRGB, "D3DPSHADECAPS_COLORFLATRGB", "dpcTriCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwShadeCaps, D3DPSHADECAPS_COLORGOURAUDRGB, "D3DPSHADECAPS_COLORGOURAUDRGB", "dpcLineCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwShadeCaps, D3DPSHADECAPS_COLORGOURAUDRGB, "D3DPSHADECAPS_COLORGOURAUDRGB", "dpcTriCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwShadeCaps, D3DPSHADECAPS_FOGFLAT, "D3DPSHADECAPS_FOGFLAT", "dpcLineCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwShadeCaps, D3DPSHADECAPS_FOGFLAT, "D3DPSHADECAPS_FOGFLAT", "dpcTriCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwShadeCaps, D3DPSHADECAPS_FOGGOURAUD, "D3DPSHADECAPS_FOGGOURAUD", "dpcLineCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwShadeCaps, D3DPSHADECAPS_FOGGOURAUD, "D3DPSHADECAPS_FOGGOURAUD", "dpcTriCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwShadeCaps, D3DPSHADECAPS_SPECULARFLATRGB, "D3DPSHADECAPS_SPECULARFLATRGB", "dpcLineCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwShadeCaps, D3DPSHADECAPS_SPECULARFLATRGB, "D3DPSHADECAPS_SPECULARFLATRGB", "dpcTriCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwShadeCaps, D3DPSHADECAPS_SPECULARGOURAUDRGB, "D3DPSHADECAPS_SPECULARGOURAUDRGB", "dpcLineCaps.dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwShadeCaps,  D3DPSHADECAPS_SPECULARGOURAUDRGB, "D3DPSHADECAPS_SPECULARGOURAUDRGB", "dpcTriCaps.dwShadeCaps" )

		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, D3DPBLENDCAPS_ZERO, "D3DPBLENDCAPS_ZERO", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_ZERO, "D3DPBLENDCAPS_ZERO", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, D3DPBLENDCAPS_ONE, "D3DPBLENDCAPS_ONE", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_ONE, "D3DPBLENDCAPS_ONE", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, D3DPBLENDCAPS_SRCALPHA, "D3DPBLENDCAPS_SRCALPHA", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_SRCALPHA, "D3DPBLENDCAPS_SRCALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, D3DPBLENDCAPS_INVSRCALPHA, "D3DPBLENDCAPS_INVSRCALPHA", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_INVSRCALPHA, "D3DPBLENDCAPS_INVSRCALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, D3DPBLENDCAPS_DESTCOLOR, "D3DPBLENDCAPS_DESTCOLOR", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_DESTCOLOR, "D3DPBLENDCAPS_DESTCOLOR", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwSrcBlendCaps, D3DPBLENDCAPS_INVDESTCOLOR, "D3DPBLENDCAPS_INVDESTCOLOR", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_INVDESTCOLOR, "D3DPBLENDCAPS_INVDESTCOLOR", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_DESTALPHA, "D3DPBLENDCAPS_DESTALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_DESTALPHA, "D3DPBLENDCAPS_DESTALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_INVDESTALPHA, "D3DPBLENDCAPS_INVDESTALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_INVDESTALPHA, "D3DPBLENDCAPS_INVDESTALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_BOTHSRCALPHA, "D3DPBLENDCAPS_BOTHSRCALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_BOTHSRCALPHA, "D3DPBLENDCAPS_BOTHSRCALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_BOTHINVSRCALPHA, "D3DPBLENDCAPS_BOTHINVSRCALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwSrcBlendCaps, D3DPBLENDCAPS_BOTHINVSRCALPHA, "D3DPBLENDCAPS_BOTHINVSRCALPHA", "dpcTriCaps.dwSrcBlendCaps" )

		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, D3DPBLENDCAPS_ZERO, "D3DPBLENDCAPS_ZERO", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, D3DPBLENDCAPS_ZERO, "D3DPBLENDCAPS_ZERO", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, D3DPBLENDCAPS_ONE, "D3DPBLENDCAPS_ONE", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, D3DPBLENDCAPS_ONE, "D3DPBLENDCAPS_ONE", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, D3DPBLENDCAPS_SRCALPHA, "D3DPBLENDCAPS_SRCALPHA", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, D3DPBLENDCAPS_SRCALPHA, "D3DPBLENDCAPS_SRCALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, D3DPBLENDCAPS_INVSRCALPHA, "D3DPBLENDCAPS_INVSRCALPHA", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, D3DPBLENDCAPS_INVSRCALPHA, "D3DPBLENDCAPS_INVSRCALPHA", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, D3DPBLENDCAPS_SRCCOLOR, "D3DPBLENDCAPS_SRCCOLOR", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, D3DPBLENDCAPS_SRCCOLOR, "D3DPBLENDCAPS_SRCCOLOR", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, D3DPBLENDCAPS_INVSRCCOLOR, "D3DPBLENDCAPS_INVSRCCOLOR", "dpcLineCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, D3DPBLENDCAPS_INVSRCCOLOR, "D3DPBLENDCAPS_INVSRCCOLOR", "dpcTriCaps.dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, D3DPBLENDCAPS_DESTALPHA, "D3DPBLENDCAPS_DESTALPHA", "dpcLineCaps.dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, D3DPBLENDCAPS_DESTALPHA, "D3DPBLENDCAPS_DESTALPHA", "dpcTriCaps.dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwDestBlendCaps, D3DPBLENDCAPS_INVDESTALPHA, "D3DPBLENDCAPS_INVDESTALPHA", "dpcLineCaps.dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwDestBlendCaps, D3DPBLENDCAPS_INVDESTALPHA, "D3DPBLENDCAPS_INVDESTALPHA", "dpcTriCaps.dwDestBlendCaps" )

		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, D3DPCMPCAPS_NEVER, "D3DPCMPCAPS_NEVER", "dpcLineCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, D3DPCMPCAPS_NEVER, "D3DPCMPCAPS_NEVER", "dpcTriCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, D3DPCMPCAPS_LESS, "D3DPCMPCAPS_LESS", "dpcLineCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, D3DPCMPCAPS_LESS, "D3DPCMPCAPS_LESS", "dpcTriCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, D3DPCMPCAPS_EQUAL, "D3DPCMPCAPS_EQUAL", "dpcLineCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, D3DPCMPCAPS_EQUAL, "D3DPCMPCAPS_EQUAL", "dpcTriCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, D3DPCMPCAPS_LESSEQUAL, "D3DPCMPCAPS_LESSEQUAL", "dpcLineCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, D3DPCMPCAPS_LESSEQUAL, "D3DPCMPCAPS_LESSEQUAL", "dpcTriCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, D3DPCMPCAPS_GREATER, "D3DPCMPCAPS_GREATER", "dpcLineCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, D3DPCMPCAPS_GREATER, "D3DPCMPCAPS_GREATER", "dpcTriCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, D3DPCMPCAPS_NOTEQUAL, "D3DPCMPCAPS_NOTEQUAL", "dpcLineCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, D3DPCMPCAPS_NOTEQUAL, "D3DPCMPCAPS_NOTEQUAL", "dpcTriCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, D3DPCMPCAPS_GREATEREQUAL, "D3DPCMPCAPS_GREATEREQUAL", "dpcLineCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, D3DPCMPCAPS_GREATEREQUAL, "D3DPCMPCAPS_GREATEREQUAL", "dpcTriCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcLineCaps.dwZCmpCaps, D3DPCMPCAPS_ALWAYS, "D3DPCMPCAPS_ALWAYS", "dpcLineCaps.dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dpcTriCaps.dwZCmpCaps, D3DPCMPCAPS_ALWAYS, "D3DPCMPCAPS_ALWAYS", "dpcTriCaps.dwZCmpCaps" )
		//*************************************************************************************************************************************************

		break;
	case 0x800:
	case 0x900:

		//Do LDDM checks  
		if ( m_pD3D->GetVersion() >= 9.1f )
		{
			/* TODO: What about these formats?
			D3DFMT_A2R10G10B10
			D3DFMT_X1R5G5B5
			D3DFMT_R5G6B5*/

			/* Apparently this is not actually a requirement currently.
			if ( FAILED(m_pD3D->CheckDeviceFormat( m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), D3DFMT_X8R8G8B8, 
				D3DUSAGE_QUERY_WRAPANDMIP | D3DUSAGE_RENDERTARGET, RTYPE_TEXTURE, D3DFMT_A16B16G16R16F )) )
			{
				m_bFail = true;
				WriteToLog( "Required texture/RT failure: D3DFMT_A16B16G16R16F is required for LDDM.\n\n" );
			}*/

			REQ_TEST( m_pDeviceDesc->dwPrimitiveMiscCaps, D3DPMISCCAPS_COLORWRITEENABLE, "D3DPMISCCAPS_COLORWRITEENABLE", "dwPrimitiveMiscCaps" )

			REQ_TEST( m_pDeviceDesc->dwCaps, D3DCAPS_READ_SCANLINE, "D3DCAPS_READ_SCANLINE", "dwCaps" )
			REQ_TEST( m_pDeviceDesc->dwCaps2, D3DCAPS2_DYNAMICTEXTURES, "D3DCAPS2_DYNAMICTEXTURES", "dwCaps2" )
			REQ_TEST( m_pDeviceDesc->dwCaps3, D3DCAPS3_COPY_TO_VIDMEM, "D3DCAPS3_COPY_TO_VIDMEM", "D3DCAPS3_COPY_TO_VIDMEM" )

			REQ_TEST( m_pDeviceDesc->dwPresentationIntervals, D3DPRESENT_INTERVAL_ONE, "D3DPRESENT_INTERVAL_ONE", "dwPresentationIntervals" )
			REQ_TEST( m_pDeviceDesc->dwPresentationIntervals, D3DPRESENT_INTERVAL_IMMEDIATE, "D3DPRESENT_INTERVAL_IMMEDIATE", "dwPresentationIntervals" )
			
			REQ_TEST( m_pDeviceDesc->dwShadeCaps, D3DPSHADECAPS_ALPHAGOURAUDBLEND, "D3DPSHADECAPS_ALPHAGOURAUDBLEND", "dwShadeCaps" )

			REQ_TEST( m_pDeviceDesc->dwStretchRectFilterCaps, D3DPTFILTERCAPS_MINFPOINT , "D3DPTFILTERCAPS_MINFPOINT ", "dwStretchRectFilterCaps" )
			REQ_TEST( m_pDeviceDesc->dwStretchRectFilterCaps, D3DPTFILTERCAPS_MAGFPOINT , "D3DPTFILTERCAPS_MAGFPOINT ", "dwStretchRectFilterCaps" )
			REQ_TEST( m_pDeviceDesc->dwStretchRectFilterCaps, D3DPTFILTERCAPS_MINFLINEAR , "D3DPTFILTERCAPS_MINFLINEAR ", "dwStretchRectFilterCaps" )
			REQ_TEST( m_pDeviceDesc->dwStretchRectFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR , "D3DPTFILTERCAPS_MAGFLINEAR ", "dwStretchRectFilterCaps" )

			REQ_TEST( m_pDeviceDesc->dwTextureCaps, D3DPTEXTURECAPS_ALPHA, "D3DPTEXTURECAPS_ALPHA", "dwTextureCaps" )
			REQ_TEST( m_pDeviceDesc->dwTextureCaps, D3DPTEXTURECAPS_CUBEMAP, "D3DPTEXTURECAPS_CUBEMAP", "dwTextureCaps" )
			if ( (0 < (m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)) != (0 < (m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_POW2)) )
			{
				m_bFail = true;
				WriteToLog( "Required Cap Failure: Non power of 2 textures must be at least conditionally supported.\n\n" );
			}
			REQ_TEST( m_pDeviceDesc->dwTextureCaps, D3DPTEXTURECAPS_PERSPECTIVE, "D3DPTEXTURECAPS_PERSPECTIVE", "dwTextureCaps" )

			REQ_TEST( m_pDeviceDesc->dwTextureAddressCaps, D3DPTADDRESSCAPS_MIRROR, "D3DPTADDRESSCAPS_MIRROR", "dwTextureAddressCaps" )
			REQ_TEST( m_pDeviceDesc->dwTextureAddressCaps, D3DPTADDRESSCAPS_CLAMP, "D3DPTADDRESSCAPS_CLAMP", "dwTextureAddressCaps" )
			REQ_TEST( m_pDeviceDesc->dwTextureAddressCaps, D3DPTADDRESSCAPS_INDEPENDENTUV, "D3DPTADDRESSCAPS_INDEPENDENTUV", "dwTextureAddressCaps" )

			REQ_TEST( m_pDeviceDesc->dwTextureFilterCaps, D3DPTFILTERCAPS_MIPFLINEAR, "D3DPTFILTERCAPS_MIPFLINEAR", "dwTextureFilterCaps" )

			REQ_TEST( m_pDeviceDesc->dwCubeTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "D3DPTFILTERCAPS_MINFLINEAR", "dwCubeTextureFilterCaps" )
			REQ_TEST( m_pDeviceDesc->dwCubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "D3DPTFILTERCAPS_MAGFLINEAR", "dwCubeTextureFilterCaps" )

			REQ_TEST( m_pDeviceDesc->dwTextureOpCaps, D3DTEXOPCAPS_DISABLE, "D3DTEXOPCAPS_DISABLE", "dwTextureOpCaps" )

			REQ_VALUE_TEST( m_pDeviceDesc->wMaxSimultaneousTextures == 8, "MaxSimultaneousTextures less than 8." )
			REQ_VALUE_TEST( m_pDeviceDesc->wMaxTextureBlendStages == 8, "MaxTextureBlendStages less than 8." )

			if ( (DWORD)D3DPS_VERSION(2, 0) > m_pDeviceDesc->dwPixelShaderVersion )
			{
				WriteToLog( "Pixel shader version 2.0 is required.\n\n" );
				m_bFail = true;
			}
		}

		REQ_TEST( m_pDeviceDesc->dwTextureAddressCaps, D3DPTADDRESSCAPS_WRAP, "D3DPTADDRESSCAPS_WRAP", "dwTextureAddressCaps" )

		REQ_TEST( m_pDeviceDesc->dwTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "D3DPTFILTERCAPS_MINFPOINT", "dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dwTextureFilterCaps, D3DPTFILTERCAPS_MIPFPOINT, "D3DPTFILTERCAPS_MIPFPOINT", "dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dwTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "D3DPTFILTERCAPS_MAGFPOINT", "dwTextureFilterCaps" )

		//*************************************************************************************************************************************************
		//			From WHQL_WLP 
		REQ_TEST( m_pDeviceDesc->dwTextureFilterCaps, D3DPTFILTERCAPS_MINFLINEAR, "D3DPTFILTERCAPS_MINFLINEAR", "dwTextureFilterCaps" )
		REQ_TEST( m_pDeviceDesc->dwTextureFilterCaps, D3DPTFILTERCAPS_MAGFLINEAR, "D3DPTFILTERCAPS_MAGFLINEAR", "dwTextureFilterCaps" )


		REQ_TEST( m_pDeviceDesc->dwTextureCaps, D3DPTEXTURECAPS_MIPMAP, "D3DPTEXTURECAPS_MIPMAP", "dwTextureCaps" )
		//*************************************************************************************************************************************************

		if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_CUBEMAP )
		{
			REQ_TEST( m_pDeviceDesc->dwCubeTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "D3DPTFILTERCAPS_MINFPOINT", "dwCubeTextureFilterCaps" )
			REQ_TEST( m_pDeviceDesc->dwCubeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "D3DPTFILTERCAPS_MAGFPOINT", "dwCubeTextureFilterCaps" )
		}

		if ( m_pDeviceDesc->dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP )
		{
			REQ_TEST( m_pDeviceDesc->dwVolumeTextureFilterCaps, D3DPTFILTERCAPS_MINFPOINT, "D3DPTFILTERCAPS_MINFPOINT", "dwVolumeTextureFilterCaps" )
			REQ_TEST( m_pDeviceDesc->dwVolumeTextureFilterCaps, D3DPTFILTERCAPS_MAGFPOINT, "D3DPTFILTERCAPS_MAGFPOINT", "dwVolumeTextureFilterCaps" )
		
			REQ_TEST( m_pDeviceDesc->dwVolumeTextureAddressCaps, D3DPTADDRESSCAPS_WRAP, "D3DPTADDRESSCAPS_WRAP", "dwVolumeTextureAddressCaps" )
		}

		REQ_TEST( m_pDeviceDesc->dwPrimitiveMiscCaps, D3DPMISCCAPS_MASKZ, "D3DPMISCCAPS_MASKZ", "dwPrimitiveMiscCaps" )
		REQ_TEST( m_pDeviceDesc->dwPrimitiveMiscCaps, D3DPMISCCAPS_CULLNONE, "D3DPMISCCAPS_CULLNONE", "dwPrimitiveMiscCaps" )
		REQ_TEST( m_pDeviceDesc->dwPrimitiveMiscCaps, D3DPMISCCAPS_CULLCW, "D3DPMISCCAPS_CULLCW", "dwPrimitiveMiscCaps" )
		REQ_TEST( m_pDeviceDesc->dwPrimitiveMiscCaps, D3DPMISCCAPS_CULLCCW, "D3DPMISCCAPS_CULLCCW", "dwPrimitiveMiscCaps" )
		
		REQ_TEST( m_pDeviceDesc->dwRasterCaps, D3DPRASTERCAPS_FOGVERTEX, "D3DPRASTERCAPS_FOGVERTEX", "dwRasterCaps" )

		//*************************************************************************************************************************************************
		//			From WHQL_WLP 
		if ( !( m_pDeviceDesc->dwRasterCaps & D3DPRASTERCAPS_FOGRANGE ) && !KeySet( "mobile" ) )
		{//If we don't have range based fog
			if ( ( m_pDeviceDesc->dwRasterCaps & D3DPRASTERCAPS_FOGTABLE ) )
			{//Check to see that our table has z or w fog	
				REQ_TEST( m_pDeviceDesc->dwRasterCaps, D3DPRASTERCAPS_WFOG | D3DPRASTERCAPS_ZFOG, "D3DPRASTERCAPS_WFOG or D3DPRASTERCAPS_ZFOG", "dwRasterCaps" )
			}
			else
			{//And we don't have depth based fog
				m_bFail = true;
				WriteToLog( "Required Cap Failure: per-pixel fog is not supported.\n\n" );
			}
		}
		//*************************************************************************************************************************************************

		REQ_TEST( m_pDeviceDesc->dwShadeCaps, D3DPSHADECAPS_COLORGOURAUDRGB, "D3DPSHADECAPS_COLORGOURAUDRGB", "dwShadeCaps" )
		
		//*************************************************************************************************************************************************
		//			From WHQL_WLP 
		REQ_TEST( m_pDeviceDesc->dwShadeCaps, D3DPSHADECAPS_SPECULARGOURAUDRGB, "D3DPSHADECAPS_SPECULARGOURAUDRGB", "dwShadeCaps" )
		REQ_TEST( m_pDeviceDesc->dwShadeCaps, D3DPSHADECAPS_FOGGOURAUD, "D3DPSHADECAPS_FOGGOURAUD", "dwShadeCaps" )

		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_ZERO, "D3DPBLENDCAPS_ZERO", "dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_ONE, "D3DPBLENDCAPS_ONE", "dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_SRCALPHA, "D3DPBLENDCAPS_SRCALPHA", "dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_INVSRCALPHA, "D3DPBLENDCAPS_INVSRCALPHA", "dwSrcBlendCaps" )
        REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_DESTALPHA, "D3DPBLENDCAPS_DESTALPHA", "dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_INVDESTALPHA, "D3DPBLENDCAPS_INVDESTALPHA", "dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_DESTCOLOR, "D3DPBLENDCAPS_DESTCOLOR", "dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_INVDESTCOLOR, "D3DPBLENDCAPS_INVDESTCOLOR", "dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_BOTHSRCALPHA, "D3DPBLENDCAPS_BOTHSRCALPHA", "dwSrcBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwSrcBlendCaps, D3DPBLENDCAPS_BOTHINVSRCALPHA, "D3DPBLENDCAPS_BOTHINVSRCALPHA", "dwSrcBlendCaps" )
		
		REQ_TEST( m_pDeviceDesc->dwDestBlendCaps, D3DPBLENDCAPS_ZERO, "D3DPBLENDCAPS_ZERO", "dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwDestBlendCaps, D3DPBLENDCAPS_ONE, "D3DPBLENDCAPS_ONE", "dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwDestBlendCaps, D3DPBLENDCAPS_SRCCOLOR, "D3DPBLENDCAPS_SRCCOLOR", "dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwDestBlendCaps, D3DPBLENDCAPS_INVSRCCOLOR, "D3DPBLENDCAPS_INVSRCCOLOR", "dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwDestBlendCaps, D3DPBLENDCAPS_SRCALPHA, "D3DPBLENDCAPS_SRCALPHA", "dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwDestBlendCaps, D3DPBLENDCAPS_INVSRCALPHA, "D3DPBLENDCAPS_INVSRCALPHA", "dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwDestBlendCaps, D3DPBLENDCAPS_DESTALPHA, "D3DPBLENDCAPS_DESTALPHA", "dwDestBlendCaps" )
		REQ_TEST( m_pDeviceDesc->dwDestBlendCaps, D3DPBLENDCAPS_INVDESTALPHA, "D3DPBLENDCAPS_INVDESTALPHA", "dwDestBlendCaps" )

		REQ_TEST( m_pDeviceDesc->dwZCmpCaps, D3DPCMPCAPS_NEVER, "D3DPCMPCAPS_NEVER", "dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dwZCmpCaps, D3DPCMPCAPS_LESS, "D3DPCMPCAPS_LESS", "dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dwZCmpCaps, D3DPCMPCAPS_EQUAL, "D3DPCMPCAPS_EQUAL", "dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dwZCmpCaps, D3DPCMPCAPS_LESSEQUAL, "D3DPCMPCAPS_LESSEQUAL", "dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dwZCmpCaps, D3DPCMPCAPS_GREATER, "D3DPCMPCAPS_GREATER", "dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dwZCmpCaps, D3DPCMPCAPS_NOTEQUAL, "D3DPCMPCAPS_NOTEQUAL", "dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dwZCmpCaps, D3DPCMPCAPS_GREATEREQUAL, "D3DPCMPCAPS_GREATEREQUAL", "dwZCmpCaps" )
		REQ_TEST( m_pDeviceDesc->dwZCmpCaps, D3DPCMPCAPS_ALWAYS, "D3DPCMPCAPS_ALWAYS", "dwZCmpCaps" )
		//*************************************************************************************************************************************************
	}

	return CCaps::ExecuteTest( uTest );
}