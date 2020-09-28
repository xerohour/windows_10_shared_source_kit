//-----------------------------------------------------------------------------
//
// Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
//
// File:	floattex.cpp
// Desc:	Contains the member functions for floating point texture tests.
// Author:	Matthew E. Haigh - September 2003
//-----------------------------------------------------------------------------

#include <d3dx9.h>
#include <types.h>

#include "TexFormats.h"
#include "CEffect.h"

//-----------------------------------------------------------------------------

CFloatTexTest::CFloatTexTest()
{
    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

	m_Options.ModeOptions.TextureFmt = FMT_ALL;
	m_Options.ModeOptions.dwTexInclude = PF_FLOATTEX;

	//set render size (we set to 100x100 so it runs faster)
	m_Options.D3DOptions.nBackBufferWidth = 100;
	m_Options.D3DOptions.nBackBufferHeight = 100;

	m_pNEffects = NULL;
	m_pTexFormat = NULL;
}

//-----------------------------------------------------------------------------

bool CFloatTexTest::CapsCheck (void)
{
	if (m_pSrcDevice != NULL)
	{
		//check to make sure src device supports pixel shader 2_0
		if (((m_pSrcDevice->GetCaps()->dwPixelShaderVersion & 0x0000FF00) >> 8) < 2)
		{
			WriteToLog ("Device does not support minimum pixel shader version (ps_2_0).\n");
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------

bool CFloatTexTest::Setup (void)
{
    float fWidth = (float) m_pCurrentMode->nBackBufferWidth;
    float fHeight = (float) m_pCurrentMode->nBackBufferHeight;
	
	m_pVertList[0] = TLVERT (0.03f * fWidth, 0.03f * fHeight, 0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 0.0f);
	m_pVertList[1] = TLVERT (0.83f * fWidth, 0.95f * fHeight, 0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 1.0f);
	m_pVertList[2] = TLVERT (0.03f * fWidth, 0.95f * fHeight, 0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 1.0f);
	m_pVertList[3] = TLVERT (0.03f * fWidth, 0.03f * fHeight, 0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 0.0f);
	m_pVertList[4] = TLVERT (0.83f * fWidth, 0.03f * fHeight, 0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 0.0f);
	m_pVertList[5] = TLVERT (0.83f * fWidth, 0.95f * fHeight, 0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 1.0f);

	m_pFullSizeRect[0] = TLVERT (0.0f,		0.0f,		0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 0.0f); //upper left
	m_pFullSizeRect[1] = TLVERT (fWidth,	fHeight,	0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 1.0f); //lower right
	m_pFullSizeRect[2] = TLVERT (0.0f,		fHeight,	0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 1.0f); //lower left
	m_pFullSizeRect[3] = TLVERT (0.0f,		0.0f,		0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 0.0f); //upper left
	m_pFullSizeRect[4] = TLVERT (fWidth,	0.0f,		0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 0.0f); //upper right
	m_pFullSizeRect[5] = TLVERT (fWidth,	fHeight,	0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 1.0f); //lower right

	return true;
}

//-----------------------------------------------------------------------------

UINT CFloatTexTest::TestInitialize (void)
{
	LPD3DXBUFFER lpErrorBuffer = NULL;
	D3DXVECTOR4 channelFilter (1.0f, 1.0f, 1.0f, 1.0f);

	//get the current format
	m_pTexFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	//determine if our current format is 16 bit or 32 bit
	if ((FMT)m_pTexFormat->d3dfFormat == FMT_R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_G16R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_A16B16G16R16F)
	{
		SetTestRange (1, NFLOAT16_TESTS);
		m_b16BitTest = true;
	}
	else
	{
		SetTestRange (1, NFLOAT32_TESTS);
		m_b16BitTest = false;
	}

	//create the effect
	if (!CreateEffectFromResource (NULL, "floattex.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pNEffects, &lpErrorBuffer))
	{
		WriteToLog ("CreateEffectFromFile() failed on %s.\n", "floattex.fx");
		if (NULL != lpErrorBuffer)
		{
			WriteToLog ("CreateEffectFromFile() compile errors: %s.\n", lpErrorBuffer->GetBufferPointer());
		}

		RELEASE (lpErrorBuffer);
		return D3DTESTINIT_ABORT;
	}

	//set the HLSL channel filter so we only display data according to the relevant texture format
	if ((FMT)m_pTexFormat->d3dfFormat == FMT_R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_R32F)
	{
		channelFilter.y = 0.0f;
		channelFilter.z = 0.0f;
	}
	else if ((FMT)m_pTexFormat->d3dfFormat == FMT_G16R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_G32R32F)
		channelFilter.z = 0.0f;

	//set the channel filter into the effect
	if (!m_pNEffects->SetVector ("g_ChannelFilter", &channelFilter))
	{
		WriteToLog  ("CnEffect::SetVector() failed: %s.\n", GLE (GetLastError()));
		RELEASE (lpErrorBuffer);
		return D3DTESTINIT_ABORT;
	}

	//set the texture coordinate offsets (1/2 of a pixel)
	if (!m_pNEffects->SetFloat ("g_TexXCoordOffset", 0.5f * 1.0f / (float) m_pCurrentMode->nBackBufferWidth))
	{
		WriteToLog  ("CnEffect::SetFloat() failed: %s.\n", GLE (GetLastError()));
		RELEASE (lpErrorBuffer);
		return D3DTESTINIT_ABORT;
	}	
	if (!m_pNEffects->SetFloat ("g_TexYCoordOffset", 0.5f * 1.0f / (float) m_pCurrentMode->nBackBufferHeight))
	{
		WriteToLog  ("CnEffect::SetFloat() failed: %s.\n", GLE (GetLastError()));
		RELEASE (lpErrorBuffer);
		return D3DTESTINIT_ABORT;
	}	

	RELEASE (lpErrorBuffer);
	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CFloatTexTest::TestTerminate (void)
{
	RELEASE (m_pNEffects);
	return true;
}

//-----------------------------------------------------------------------------

void CFloatTexTest::SceneRefresh (void)
{
}
//-----------------------------------------------------------------------------
// Name:		ExecuteMultiTextureTest()
// Desc:		This function is used by the multiple texture test group.  This
//				function produces a HDR lighting type effect using a varying 
//				number of textures (currently supports 2, 4 or 8 textures).  It 
//				creates a background texture using the supplied technique, and 
//				then applies lighting to that texture.  It also creates textures
//				containing a glare effect at the light point.  The last step is 
//				the addition of all the lighting and glare textures.
// Arguments:	numTexPairs - the number of texture pairs to use, which 
//					corresponds to the number of light points in the scene.
//					Can only be set to the values 1, 2 or 4.
//				brightness - the brightness of the light points
//				backTexCreateTech - the technique to use to create the 
//					background texture.
//-----------------------------------------------------------------------------

bool CFloatTexTest::ExecuteMultiTextureTest (UINT numTexPairs, float brightness, char* backTexCreateTech)
{
	bool bResult;
	bool bReturn = true;
	HRESULT hrResult = D3D_OK;
	CnTexture* pSourceTex = NULL;
	CnSurface* pSourceTexSurf = NULL;
	CnTexture** ppLitTex = NULL;
	CnSurface** ppLitTexSurf = NULL;
	CnTexture** ppGlareTex = NULL;
	CnSurface** ppGlareTexSurf = NULL;
	CnSurface* pOrigSurface = NULL;
	D3DXVECTOR4 glareVec[7] = { D3DXVECTOR4 (0.5f,  0.5f,  0.0f, 0.01f * brightness),
								D3DXVECTOR4 (0.25f, 0.5f,  0.0f, 0.01f * brightness),
								D3DXVECTOR4 (0.75f, 0.5f,  0.0f, 0.01f * brightness),
								D3DXVECTOR4 (0.25f, 0.25f, 0.0f, 0.01f * brightness),
								D3DXVECTOR4 (0.25f, 0.75f, 0.0f, 0.01f * brightness),
								D3DXVECTOR4 (0.75f, 0.25f, 0.0f, 0.01f * brightness),
								D3DXVECTOR4 (0.75f, 0.75f, 0.0f, 0.01f * brightness)};
	D3DXVECTOR4 lightVec[7] = { D3DXVECTOR4 (0.5f,  0.5f,  0.0f, 1.0f * brightness),
								D3DXVECTOR4 (0.25f, 0.5f,  0.0f, 1.0f * brightness),
								D3DXVECTOR4 (0.75f, 0.5f,  0.0f, 1.0f * brightness),
								D3DXVECTOR4 (0.25f, 0.25f, 0.0f, 1.0f * brightness),
								D3DXVECTOR4 (0.25f, 0.75f, 0.0f, 1.0f * brightness),
								D3DXVECTOR4 (0.75f, 0.25f, 0.0f, 1.0f * brightness),
								D3DXVECTOR4 (0.75f, 0.75f, 0.0f, 1.0f * brightness)};

	//make sure the number of texture pairs is 1, 2 or 4.
	if (numTexPairs != 1 && numTexPairs != 2 && numTexPairs != 4)
	{
		WriteToLog ("ExecuteMultiTextureTest() failed: Invalid number of textures.\n");
		FAIL_AND_EXIT;
	}

	//allocate the texture and surface pointers
	ppLitTex = new CnTexture*[numTexPairs];
	ppLitTexSurf = new CnSurface*[numTexPairs];
	ppGlareTex = new CnTexture*[numTexPairs];
	ppGlareTexSurf = new CnSurface*[numTexPairs];

	if (NULL == ppLitTex || NULL == ppLitTexSurf || NULL == ppGlareTex || NULL == ppGlareTexSurf)
	{
		WriteToLog ("ExecuteMultiTextureTest() failed: Out of memory.\n");
		FAIL_AND_EXIT;
	}
	
	memset( ppLitTex, 0, numTexPairs * sizeof( CnTexture *) );
	memset( ppLitTexSurf, 0, numTexPairs * sizeof( CnSurface *) );
	memset( ppGlareTex, 0, numTexPairs * sizeof( CnTexture *) );
	memset( ppGlareTexSurf, 0, numTexPairs * sizeof( CnSurface *) );

	// Make sure the device can render to the texture format being created
	if( FAILED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
											m_pSrcDevice->GetDevType(), 
											m_pCurrentMode->pDisplay->Format,
											USAGE_RENDERTARGET, 
											RTYPE_TEXTURE, 
											*m_pTexFormat ) ) )
	{
		Skip();
		bReturn = false;
		goto Exit;
	}
	
	
	//the size of the textures must EXACTLY match the size of the back buffer, or else
	//the texture will be filtered which we would like to avoid
	for (UINT i = 0; i < numTexPairs; i++)
	{
		ppLitTex[i] = NULL;
		bResult = CreateTexture (m_pCurrentMode->nBackBufferWidth, m_pCurrentMode->nBackBufferHeight, 1, D3DUSAGE_RENDERTARGET, *m_pTexFormat, FMWK_DEFAULT, &(ppLitTex[i]));
		if (!bResult || (NULL == ppLitTex[i]))
		{
			hrResult = GetLastError();
			WriteToLog ("CreateTexture() failed: %s.\n", GLE (hrResult));

			// No need to fail if we run out of memory...full screen cases require lots.
			if (hrResult == D3DERR_OUTOFVIDEOMEMORY)
			{
				Skip();
				bReturn = false;
				goto Exit;
			}

			FAIL_AND_EXIT;
		}

		ppGlareTex[i] = NULL;
		bResult = CreateTexture (m_pCurrentMode->nBackBufferWidth, m_pCurrentMode->nBackBufferHeight, 1, D3DUSAGE_RENDERTARGET, *m_pTexFormat, FMWK_DEFAULT, &(ppGlareTex[i]));
		if (!bResult || (NULL == ppGlareTex[i]))
		{
			hrResult = GetLastError();
			WriteToLog ("CreateTexture() failed: %s.\n", GLE (hrResult));

			// No need to fail if we run out of memory...full screen cases require lots.
			if (hrResult == D3DERR_OUTOFVIDEOMEMORY)
			{
				Skip();
				bReturn = false;
				goto Exit;
			}

			FAIL_AND_EXIT;
		}
	}		
	
	bResult = CreateTexture (m_pCurrentMode->nBackBufferWidth, m_pCurrentMode->nBackBufferHeight, 1, D3DUSAGE_RENDERTARGET, *m_pTexFormat, FMWK_DEFAULT, &pSourceTex);
	if (!bResult || (NULL == pSourceTex))
	{
		hrResult = GetLastError();
		WriteToLog ("CreateTexture() failed: %s.\n", GLE (hrResult));

		// No need to fail if we run out of memory...full screen cases require lots.
		if (hrResult == D3DERR_OUTOFVIDEOMEMORY)
		{
			Skip();
			bReturn = false;
			goto Exit;
		}

		FAIL_AND_EXIT;
	}

	//get the surface of the textures
	for (UINT i = 0; i < numTexPairs; i++)
	{
		bResult = ppLitTex[i]->GetSurfaceLevel (0, &(ppLitTexSurf[i]));
		if (!bResult || (NULL == ppLitTexSurf[i]))
		{
			WriteToLog ("GetSurfaceLevel() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		bResult = ppGlareTex[i]->GetSurfaceLevel (0, &(ppGlareTexSurf[i]));
		if (!bResult || (NULL == ppGlareTexSurf[i]))
		{
			WriteToLog ("GetSurfaceLevel() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
	}
	bResult = pSourceTex->GetSurfaceLevel (0, &pSourceTexSurf);
	if (!bResult || (NULL == pSourceTexSurf))
	{
		WriteToLog ("GetSurfaceLevel() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//get the original rendering surface
	bResult = GetRenderTarget (&pOrigSurface);
	if (!bResult || (NULL == pOrigSurface))
	{
		WriteToLog ("GetRenderTarget() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//start the scene
	if (!BeginScene())
	{
		WriteToLog  ("BeginScene() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//make the source texture
	if (!m_pNEffects->SetTechniqueByName (backTexCreateTech))
	{
		WriteToLog  ("CnEffect::SetTechnique() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->SetFloat ("g_ScaleFactor", 1.0f))
	{
		WriteToLog  ("CnEffect::SetFloat() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!SetRenderTarget ((DWORD)0, pSourceTexSurf))
	{
		WriteToLog ("SetRenderTarget() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->Begin (NULL, 0))
	{
		WriteToLog  ("CnEffect::Begin() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->BeginPass (0))
	{
		WriteToLog  ("CnEffect::BeginPass(0) failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!RenderPrimitive (D3DPT_TRIANGLELIST, FMT_TLVERT, m_pFullSizeRect, 6, NULL, 0, 0))
	{
		WriteToLog  ("RenderPrimitive() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->EndPass ())
	{
		WriteToLog  ("CnEffect::EndPass() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->End())
	{
		WriteToLog  ("CnEffect::End() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//make the glare textures
	if (!m_pNEffects->SetTechniqueByName ("MakeGlareTextureTech"))
	{
		WriteToLog  ("CnEffect::SetTechnique() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->Begin (NULL, 0))
	{
		WriteToLog  ("CnEffect::Begin() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	for (UINT i = 0; i < numTexPairs; i++)
	{	
		if (!m_pNEffects->BeginPass (0))
		{
			WriteToLog  ("CnEffect::BeginPass(0) failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!m_pNEffects->SetVector ("g_GlarePos", &glareVec[numTexPairs + i - 1]))
		{
			WriteToLog ("SetVector() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!SetRenderTarget ((DWORD)0, ppGlareTexSurf[i]))
		{
			WriteToLog ("SetRenderTarget() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!m_pNEffects->CommitChanges ())
		{
			WriteToLog  ("CnEffect::CommitChanges() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!RenderPrimitive (D3DPT_TRIANGLELIST, FMT_TLVERT, m_pFullSizeRect, 6, NULL, 0, 0))
		{
			WriteToLog  ("RenderPrimitive() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!m_pNEffects->EndPass ())
		{
			WriteToLog  ("CnEffect::EndPass() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
	}
	if (!m_pNEffects->End())
	{
		WriteToLog  ("CnEffect::End() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//apply the light to the source textures
	if (!m_pNEffects->SetTexture ("floatTex0", pSourceTex))
	{
		WriteToLog  ("CnEffect::SetTechnique() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->SetTechniqueByName ("ApplyLightTech"))
	{
		WriteToLog  ("CnEffect::SetTechnique() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->Begin (NULL, 0))
	{
		WriteToLog  ("CnEffect::Begin() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	for (int i = numTexPairs - 1; i >= 0; i--)
	{
		if (!m_pNEffects->BeginPass (0))
		{
			WriteToLog  ("CnEffect::BeginPass(0) failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}	
		if (!m_pNEffects->SetVector ("g_LightPos", &lightVec[numTexPairs + i - 1]))
		{
			WriteToLog ("SetVector() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!SetRenderTarget ((DWORD)0, ppLitTexSurf[i]))
		{
			WriteToLog ("SetRenderTarget() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!m_pNEffects->CommitChanges ())
		{
			WriteToLog  ("CnEffect::CommitChanges() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!RenderPrimitive (D3DPT_TRIANGLELIST, FMT_TLVERT, m_pFullSizeRect, 6, NULL, 0, 0))
		{
			WriteToLog  ("RenderPrimitive() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
		if (!m_pNEffects->EndPass ())
		{
			WriteToLog  ("CnEffect::EndPass() failed: %s.\n", GLE (GetLastError()));
			FAIL_AND_EXIT;
		}
	}
	if (!m_pNEffects->End())
	{
		WriteToLog  ("CnEffect::End() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//display  by switching back to the original rendertarget
	if (!SetRenderTarget ((DWORD)0, pOrigSurface))
	{
		WriteToLog ("SetRenderTarget() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//apply the add texture technique
	//first set the samplers
	bResult = true;
	bResult &= m_pNEffects->SetTexture ("floatTex0", ppLitTex[0]);
	bResult &= m_pNEffects->SetTexture ("glareTex0", ppGlareTex[0]);
	if (numTexPairs > 1)
	{
		bResult &= m_pNEffects->SetTexture ("floatTex1", ppLitTex[1]);
		bResult &= m_pNEffects->SetTexture ("glareTex1", ppGlareTex[1]);
	}
	if (numTexPairs > 2)
	{
		bResult &= m_pNEffects->SetTexture ("floatTex2", ppLitTex[2]);
		bResult &= m_pNEffects->SetTexture ("glareTex2", ppGlareTex[2]);
		bResult &= m_pNEffects->SetTexture ("floatTex3", ppLitTex[3]);
		bResult &= m_pNEffects->SetTexture ("glareTex3", ppGlareTex[3]);
	}
	if (!bResult)
	{
		WriteToLog  ("CnEffect::SetTexture() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}	

	//next set the technique
	if (1 == numTexPairs)
		bResult = m_pNEffects->SetTechniqueByName ("TwoTextureAddTech");
	else if (2 == numTexPairs)
		bResult = m_pNEffects->SetTechniqueByName ("FourTextureAddTech");
	else
		bResult = m_pNEffects->SetTechniqueByName ("EightTextureAddTech");
	if (!bResult)
	{
		WriteToLog  ("CnEffect::SetTechnique() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->Begin (NULL, 0))
	{
		WriteToLog  ("CnEffect::Begin() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->BeginPass (0))
	{
		WriteToLog  ("CnEffect::BeginPass(0) failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!RenderPrimitive (D3DPT_TRIANGLELIST, FMT_TLVERT, m_pFullSizeRect, 6, NULL, 0, 0))
	{
		WriteToLog  ("RenderPrimitive() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->EndPass ())
	{
		WriteToLog  ("CnEffect::EndPass() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!m_pNEffects->End())
	{
		WriteToLog  ("CnEffect::End() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	
	//end the scene
	if (!EndScene())
	{
		WriteToLog  ("EndScene() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//and finish
Exit:
	for (UINT i = 0; i < numTexPairs; i++)
	{
		if (NULL != ppLitTex)
			RELEASE (ppLitTex[i]);
		if (NULL != ppLitTexSurf)
			RELEASE (ppLitTexSurf[i]);
		if (NULL != ppGlareTex)
			RELEASE (ppGlareTex[i]);
		if (NULL != ppGlareTexSurf)
			RELEASE (ppGlareTexSurf[i]);
	}
	SAFEDELETEA (ppLitTex);
	SAFEDELETEA (ppLitTexSurf);
	SAFEDELETEA (ppGlareTex);
	SAFEDELETEA (ppGlareTexSurf);
	RELEASE (pOrigSurface);
	RELEASE (pSourceTex);
	RELEASE (pSourceTexSurf);

	return bReturn;
}

//-----------------------------------------------------------------------------
// CFloatMultipleTexTest class members
//		
// Desc:	This group uses multiple textures to compose a scene which is not
//			entirely between 0 and 1.  This tests purpose is thus to ensure
//			proper behavior when many floating point textures are used at once,
//			and also to ensure that values outside the range of 0 to 1 are
//			properly displayed. 
//
// Cases:	This group has test cases that enumerate between 2, 4 and 8 
//			textures, several brightness values, and two background textures
//			(stripes and gradient).
//-----------------------------------------------------------------------------

CFloatMultipleTexTest::CFloatMultipleTexTest()
{
	m_szTestName = "Float Tex Multiple Textures";
	m_szCommandKey = "FloatTexMultTex";
}

//-----------------------------------------------------------------------------

UINT CFloatMultipleTexTest::TestInitialize (void)
{
	UINT uReturnVal = CFloatTexTest::TestInitialize ();
	
	if (uReturnVal == D3DTESTINIT_RUN)
		SetTestRange (1, NMULTITEX_TESTS);

	return uReturnVal;
}

//-----------------------------------------------------------------------------

bool CFloatMultipleTexTest::ExecuteTest (UINT uTest)
{
	bool bReturn = true;
	CHAR msgBuffer[80];
	UINT factorIndex = (uTest-1) % NMULTITEX_FACTORS;

	sprintf (msgBuffer, "Multiple texture test %d.  Num textures: %d Brightness: %f", uTest, (UINT)MULTITEX_FACTORS[factorIndex*2], MULTITEX_FACTORS[factorIndex*2 + 1]);
	
	BeginTestCase (msgBuffer, uTest);

	if (((uTest-1) / NMULTITEX_FACTORS) == 0)
		bReturn = ExecuteMultiTextureTest ((UINT)MULTITEX_FACTORS[factorIndex*2], MULTITEX_FACTORS[factorIndex*2 + 1], "MakeScaledStripesTech");
	else
		bReturn = ExecuteMultiTextureTest ((UINT)MULTITEX_FACTORS[factorIndex*2], MULTITEX_FACTORS[factorIndex*2 + 1], "MakeScaledGradientTech");

	return bReturn;
}

