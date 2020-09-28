// FILE:        mipmap.cpp
// DESC:        mipmap and mipmap multi class methods for point sprite conformance test
// AUTHOR:      Anthony Clarke
// COMMENTS:

#include "sprite.h"

// NAME:        CSpriteMipMap()
// DESC:        constructor for mip map class
// INPUT:       none
// OUTPUT:      none

CSpriteMipMap::CSpriteMipMap()
{
    m_szTestName="Mip map";
    m_szCommandKey="MipMap";

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_LATEST;

	m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        ~CSpriteMipMap()
// DESC:        destructor for mip map class
// INPUT:       none
// OUTPUT:      none

CSpriteMipMap::~CSpriteMipMap()
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteMipMap::TestInitialize(VOID)
{
    if (0 == (m_pSrcDevice->GetCaps()->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
    {
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }

    m_Group.Set(CF_MIPMAP);
    return CSpriteAttenuateFVFL::TestInitialize();
}

// NAME:        CSpriteMipMapMulti()
// DESC:        constructor for mip map multi class
// INPUT:       none
// OUTPUT:      none

CSpriteMipMapMulti::CSpriteMipMapMulti()
{
    m_szTestName="Mip map (multi-texture)";
    m_szCommandKey="MipMapMulti";

	// Force test to use relevant DirectX versions for WHQL
	if (KeySet("WHQL"))
		m_Options.D3DOptions.dwIncVersions = DXVERSION_LATEST;

	m_Group.Replace(0);
    m_Group.Save();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteMipMapMulti::TestInitialize(VOID)
{
    UINT uResult;

    // stability test if hardware doesn't support PS 2.0 or not testing Dx9 interfaces

    if ((m_dwVersion > 0x0800) &&
        (m_pSrcDevice->GetCaps()->dwPixelShaderVersion >= D3DPS_VERSION(2, 0)) &&
        (m_pRefDevice->GetCaps()->dwPixelShaderVersion >= D3DPS_VERSION(2, 0)))
        m_Group.Set(CF_SPAWN);
    else
        m_Group.Set(CF_INVALID | CF_SPAWN);

    m_iNumStagesSet = m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures;

	if (m_iNumStagesSet > 8)
	{
        WriteToLog("Device capability inconsistency: MaxSimultaneousTextures (%d) cannot be greater than 8.\n",m_iNumStagesSet);
        return D3DTESTINIT_ABORT; 
	}

    uResult = CSpriteMipMap::TestInitialize();

    if (D3DTESTINIT_RUN != uResult)
        return uResult; 

    return uResult;
}
