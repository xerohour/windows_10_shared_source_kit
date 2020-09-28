//-----------------------------------------------------------------------------
// File: AGP.cpp
//
// Desc: AGP Test - Base Class and Texture size test groups.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "BasicTex.h"

//---CAGP Test Functions------------------------------------------------------
//----------------------------------------------------------------------------
// Name: CAGP()
// Desc: Construct
//----------------------------------------------------------------------------
CAGP::CAGP(void)
{
    // Init Framework options
     m_Options.D3DOptions.fMaxDXVersion = 7.0f;

     // Use a strip image
     m_bUseStripeImage = true;

     // Only need to run one test per tex format
     m_uNumTests = 1;
}

//---CAGP Texture Size tests--------------------------------------------------
//----------------------------------------------------------------------------
// Name: CAGPnxn()
// Desc: Setup Texture size, test name, and test key for AGP Texture size test
//----------------------------------------------------------------------------
CAGP8x8::CAGP8x8(void)
{
    // Init Framework options
    m_szTestName = _T("AGP - TexSize: 8x8");
	m_szCommandKey = _T("AGP8x8");

    // Set texture size
    m_nTextureSize = 8;
}
CAGP16x16::CAGP16x16(void)
{
    // Init Framework options
    m_szTestName = _T("AGP - TexSize: 16x16");
	m_szCommandKey = _T("AGP16x16");

    // Set texture size
    m_nTextureSize = 16;
}
CAGP64x64::CAGP64x64(void)
{
    // Init Framework options
    m_szTestName = _T("AGP - TexSize: 64x64");
	m_szCommandKey = _T("AGP64x64");

    // Set texture size
    m_nTextureSize = 64;
}
CAGP256x256::CAGP256x256(void)
{
    // Init Framework options
    m_szTestName = _T("AGP - TexSize: 256x256");
	m_szCommandKey = _T("AGP256x256");

    // Set texture size
    m_nTextureSize = 256;
}
CAGP1024x1024::CAGP1024x1024(void)
{
    // Init Framework options
    m_szTestName = _T("AGP - TexSize: 1024x1024");
	m_szCommandKey = _T("AGP1024x1024");

    // Set texture size
    m_nTextureSize = 1024;
}

//---D3D Framework Derived Functions------------------------------------------

//----------------------------------------------------------------------------
// Name: CommandLineHelp()
// Desc: Command line help for the test case. 
//----------------------------------------------------------------------------
void CAGP::CommandLineHelp(void) 
{
    WriteCommandLineHelp(_T("This test group creates a texture of size %ix%i using the AGP pool."), m_nTextureSize,m_nTextureSize);
    WriteCommandLineHelp(_T("After render, the texture is verified to be in AGP memory."));
    WriteCommandLineHelp(_T("If the texture is not in AGP memory the test fails."));
    WriteCommandLineHelp(_T("This test group cycles each common texture format supported by the hardware."));
}

//----------------------------------------------------------------------------
// Name: CapsCheck()
// Desc: Check caps requred by test. 
//----------------------------------------------------------------------------
bool CAGP::CapsCheck(void)
{
    // Make sure we are not on Dx8 or better
	if (m_dwVersion >= 0x0800)
	{
		WriteToLog("This test is not valid on Dx8 or greater.\n");
		return false;
	}

    // Check the caps we are interested in
	DWORD dwDevCaps = m_pSrcDevice->GetCaps()->dwDevCaps;

	// Check the NonLocalVidMem cap for Hardware devices
	if (m_pSrcDevice->GetDevType() == DEVICETYPE_HAL)
	{
		if (!(dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM))
		{
			WriteToLog(_T("Device capability not supported: TextureNonLocalVidMem.\n"));
			return false;
		}
        m_dwTexturePool = POOL_AGP;
	}
    else
        m_dwTexturePool = POOL_SYSTEMMEM;

    return  CBasicTex::CapsCheck();
}

//----------------------------------------------------------------------------
// Name: ProcessFrame
// Desc: Make sure texture is in AGP
//----------------------------------------------------------------------------
bool CAGP::ProcessFrame(void)
{
    // If using AGP memory, make sure our hardware texture is in AGP.
    SURFACEDESC Desc;
	if(POOL_AGP == m_dwTexturePool)
	{
        m_pTexture->GetLevelDesc(0,&Desc);
		if (Desc.Pool != POOL_AGP)
		{
			WriteToLog(_T("Texture not in AGP memory, dwCaps are %x.\n"),Desc.ddsCaps.dwCaps);
		    Fail();
            return false;
		}
	}

    return CBasicTex::ProcessFrame();
}