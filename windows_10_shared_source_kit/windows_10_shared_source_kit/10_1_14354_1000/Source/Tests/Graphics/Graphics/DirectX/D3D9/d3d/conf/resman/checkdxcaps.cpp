#define D3D_OVERLOADS
#include "d3dx9.h"
#include "Types.h"
#include "ResMan.h"

void ResMan::CheckDX9Caps()
{
	DEVICEDESC *Caps = m_pSrcDevice->GetCaps();
    
	if ( KeySet("maxmiplevel") || KeySet("default_pool") )
		m_UseLODBias = false;
	
	if ( Caps->dwMaxVertexIndex <= 0x0000ffff )
		m_Use32BitIB = false;

	if ( Caps->dwMaxTextureWidth )
		m_MaxTextureWidth = (WORD)min(Caps->dwMaxTextureWidth, 1024);
	else
		m_MaxTextureWidth = 1024;
	
	if ( Caps->dwMaxTextureHeight )
		m_MaxTextureHeight = (WORD)min(Caps->dwMaxTextureHeight, 1024);
	else
		m_MaxTextureHeight = 1024;
	
	m_MaxCubeWidth = m_MaxTextureWidth;

	if ( Caps->dwMaxVolumeExtent )
		m_MaxVolumeExtent = (WORD)min(Caps->dwMaxVolumeExtent, 1024);
	else
		m_MaxVolumeExtent = 1024;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_POW2)
		m_TextureIsPow2 = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
		m_NonPow2Conditional = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_CUBEMAP_POW2)
		m_CubeMapIsPow2 = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_VOLUMEMAP_POW2)
		m_VolumeMapIsPow2 = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_SQUAREONLY)
		m_SquareOnly = true;

	if (KeySet(_T("!tex")))
		m_UseTextures = false;

	if (!(Caps->dwTextureCaps  & D3DPTEXTURECAPS_MIPMAP))
		m_UseMipMaps = false;
    else if (Caps->dwTextureFilterCaps  & D3DPTFILTERCAPS_MIPFLINEAR)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
	else if (Caps->dwTextureFilterCaps  & D3DPTFILTERCAPS_MIPFPOINT)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
	else
	{
		m_UseMipMaps = false;
		WriteToLog("Error: Mipmaping is set without corresponding filter caps\n");
	}
	
	if ( !(Caps->dwTextureCaps  & D3DPTEXTURECAPS_CUBEMAP) || KeySet(_T("!cube")) ) 
	{
		m_UseCubeMaps = false;
    }

	if (!(Caps->dwTextureCaps  & D3DPTEXTURECAPS_MIPCUBEMAP))
		m_UseMipCubeMaps = false;
	else if (Caps->dwCubeTextureFilterCaps  & D3DPTFILTERCAPS_MIPFLINEAR)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
	else if (Caps->dwCubeTextureFilterCaps  & D3DPTFILTERCAPS_MIPFPOINT)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
	else
	{
		m_UseMipCubeMaps = false;
		WriteToLog("Error: Cube mipmaping is set without corresponding filter caps\n");
	}
	
	if ( !(Caps->dwTextureCaps  & D3DPTEXTURECAPS_VOLUMEMAP) || KeySet(_T("!vol"))) 
		m_UseVolMaps = false;

	if (!(Caps->dwTextureCaps  & D3DPTEXTURECAPS_MIPVOLUMEMAP)) 
		m_UseMipVolMaps = false;
	else if (Caps->dwVolumeTextureFilterCaps  & D3DPTFILTERCAPS_MIPFLINEAR)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
	else if (Caps->dwVolumeTextureFilterCaps  & D3DPTFILTERCAPS_MIPFPOINT)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
	else
	{
		m_UseMipVolMaps = false;
		WriteToLog("Error: Volume mipmaping is set without corresponding filter caps\n");
	}
}

void ResMan::CheckDX8Caps()
{
	DEVICEDESC *Caps = m_pSrcDevice->GetCaps();

	if ( KeySet("maxmiplevel") || KeySet("default_pool") )
		m_UseLODBias = false;

	if ( Caps->dwMaxVertexIndex <= 0x0000ffff )
		m_Use32BitIB = false;

	if ( Caps->dwMaxTextureWidth )
		m_MaxTextureWidth = (WORD)min(Caps->dwMaxTextureWidth, 1024);
	else
		m_MaxTextureWidth = 1024;
	
	if ( Caps->dwMaxTextureHeight )
		m_MaxTextureHeight = (WORD)min(Caps->dwMaxTextureHeight, 1024);
	else
		m_MaxTextureHeight = 1024;
	
	m_MaxCubeWidth = m_MaxTextureWidth;

	if ( Caps->dwMaxVolumeExtent )
		m_MaxVolumeExtent = (WORD)min(Caps->dwMaxVolumeExtent, 1024);
	else
		m_MaxVolumeExtent = 1024;
	
	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_POW2)
		m_TextureIsPow2 = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
		m_NonPow2Conditional = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_CUBEMAP_POW2)
		m_CubeMapIsPow2 = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_VOLUMEMAP_POW2)
		m_VolumeMapIsPow2 = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_SQUAREONLY)
		m_SquareOnly = true;

	if (KeySet(_T("!tex")))
		m_UseTextures = false;

	if (!(Caps->dwTextureCaps  & D3DPTEXTURECAPS_MIPMAP))
		m_UseMipMaps = false;
	else if (Caps->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
	else if (Caps->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
	else
	{
		m_UseMipMaps = false;
		WriteToLog("Error: Mipmaping is set without corresponding filter caps\n");
	}
	
	if ( !(Caps->dwTextureCaps  & D3DPTEXTURECAPS_CUBEMAP) || KeySet(_T("!cube")) ) 
	{
		m_UseCubeMaps = false;
    }

	if (!(Caps->dwTextureCaps  & D3DPTEXTURECAPS_MIPCUBEMAP))
		m_UseMipCubeMaps = false;
	else if (Caps->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
	else if (Caps->dwCubeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
	else
	{
		m_UseMipCubeMaps = false;
		WriteToLog("Error: Cube mipmaping is set without corresponding filter caps\n");
	}
	
	if ( !(Caps->dwTextureCaps  & D3DPTEXTURECAPS_VOLUMEMAP) || KeySet(_T("!vol")) ) 
		m_UseVolMaps = false;

	if (!(Caps->dwTextureCaps  & D3DPTEXTURECAPS_MIPVOLUMEMAP)) 
		m_UseMipVolMaps = false;
	else if (Caps->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
	else if (Caps->dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
	else
	{
		m_UseMipVolMaps = false;
		WriteToLog("Error: Volume mipmaping is set without corresponding filter caps\n");
	}
}

void ResMan::CheckDX7Caps()
{
	DEVICEDESC *Caps = m_pSrcDevice->GetCaps();

    if ( KeySet("maxmiplevel") || KeySet("default_pool") )
		m_UseLODBias = false;
	
	
	if ( Caps->dwMaxTextureWidth )
		m_MaxTextureWidth = (WORD)min(Caps->dwMaxTextureWidth, 1024);
	else
		m_MaxTextureWidth = 1024;
	
	if ( Caps->dwMaxTextureHeight )
		m_MaxTextureHeight = (WORD)min(Caps->dwMaxTextureHeight, 1024);
	else
		m_MaxTextureHeight = 1024;

	m_MaxCubeWidth = m_MaxTextureWidth;

	if (Caps->dwTextureCaps & D3DPTEXTURECAPS_POW2)
	{
		m_TextureIsPow2 = true;
		m_CubeMapIsPow2 = true;
	}

	if (Caps->dwTextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
		m_NonPow2Conditional = true;
	if (Caps->dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
		m_SquareOnly = true;

	if (KeySet(_T("!tex")))
		m_UseTextures = false;

	if (Caps->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
	else if (Caps->dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
	else
	{
		m_UseMipMaps = false;
		m_UseMipCubeMaps = false;

	}

	if ( !(Caps->dwTextureCaps  & D3DPTEXTURECAPS_CUBEMAP) || KeySet(_T("!cube")) ) 
	{
		m_UseCubeMaps = false;
		m_UseMipCubeMaps = false;
	}

	//volume textures don't exist in DX7
	m_UseVolMaps    = false;
	m_UseMipVolMaps = false;
}
/*
void ResMan::CheckDX6Caps()
{
	DEVICEDESC *Caps = m_pSrcDevice->GetCaps();

	m_UseLODBias = false;

	if ( Caps->dwMaxTextureWidth )
		m_MaxTextureWidth = (WORD)min(Caps->dwMaxTextureWidth, 1024);
	else
		m_MaxTextureWidth = 1024;
	
	if ( Caps->dwMaxTextureHeight )
		m_MaxTextureHeight = (WORD)min(Caps->dwMaxTextureHeight, 1024);
	else
		m_MaxTextureHeight = 1024;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_POW2)
		m_TextureIsPow2 = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
		m_NonPow2Conditional = true;

	if (Caps->dwTextureCaps  & D3DPTEXTURECAPS_SQUAREONLY)
		m_SquareOnly = true;

	if (KeySet(_T("!tex")))
		m_UseTextures = false;

	if (Caps->dwTextureFilterCaps  & D3DPTFILTERCAPS_MIPFLINEAR)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_LINEAR);		
	else if (Caps->dwTextureFilterCaps  & D3DPTFILTERCAPS_MIPFPOINT)
		SetSamplerState(0, SAMP_MIPFILTER, (DWORD)TEXF_POINT);
	else
		m_UseMipMaps = false;

	//cube maps and volumes don't exist in DX6
	m_UseCubeMaps    = false;
	m_UseMipCubeMaps = false;
	m_UseVolMaps     = false;
	m_UseMipVolMaps  = false;

}
*/

