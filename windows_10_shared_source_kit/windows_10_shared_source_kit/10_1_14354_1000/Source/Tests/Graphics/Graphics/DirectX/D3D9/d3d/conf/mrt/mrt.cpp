//
// MRT.CPP - Implementation of the MultipleRenderTarget test
//

#include <d3d9.h>
#include <d3dx9.h>
#include "cd3dx.h"
#include "MRT.h"

// Global Test and App instance
CD3DWindowFramework	App;
#ifndef FAST_TEST
CMRTOneTexture		OneTexture;
CMRTTwoTexture		TwoTexture;
CMRTThreeTexture	ThreeTexture;
#endif
CMRTFourTexture		FourTexture;
#ifndef FAST_TEST
CMRTOneBlend		OneBlend;
CMRTTwoBlend		TwoBlend;
CMRTThreeBlend  	ThreeBlend;
#endif
CMRTFourBlend		FourBlend;
CMRTFog				Fog;
CMRTColorWrite 	    ColorWrite;
CMRTInvalid			Invalid;
CMETexture			METextue;
CMETBlend			METBlend;
CMETFog				METFog;
CMETColorWrite 	    METColorWrite;

CMRT::CMRT()
{
    // Init Framework options
    SetTestType(TESTTYPE_CONF);
	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	m_Options.ModeOptions.dwTexInclude = PF_RGB;
	m_Options.ModeOptions.dwTexExclude = PF_PALETTEINDEXED4 | PF_PALETTEINDEXED8; 

    // Init variables
	nNumRTs = 1;
	nNumElements = 0;
    nWidthRT = 256;
    nHeightRT = 256;
	bFog = false;
	bMETex = false;
	bBlend = false;
	bInvalid = false;
	bColorWrite = false;
	pPixelShader = NULL;

    // Init pointer variables
	for (int i=0; i<4; i++)
	{
	    pTempRT[i] = NULL;
		pTextureRT[i] = NULL;
	}

    pTempZ = NULL;
    pOriginalRT = NULL;
    pOriginalZ = NULL;
}

CMRT::~CMRT()
{
}

void CMRT::CommandLineHelp(void)
{
    WriteCommandLineHelp("$y-Width:$wn\t\t$wSets the texture width to n");
    WriteCommandLineHelp("$y-Height:$wn\t\t$wSets the texture height to n");
}

UINT CMRT::TestInitialize(void)
{
	// Get the device caps
	CAPS DevCaps;
	m_pSrcDevice->GetDeviceCaps(&DevCaps);

	// Set up a default test range until we calculate it
	SetTestRange(1, 1);

	// Make sure that we have at least one render target
	if (DevCaps.NumSimultaneousRTs < 1)
	{
		WriteToLog("Device capability not supported: NumSimultaneousRTs is %d, needs to be at least 1.\n",DevCaps.NumSimultaneousRTs);
    	return D3DTESTINIT_ABORT;
	}

	// Check the number of requested render targets with whats supported
	if (!bInvalid && (nNumRTs > DevCaps.NumSimultaneousRTs))
	{
		WriteToLog("Device capability not supported: NumSimultaneousRTs is %d, needs to be at least %d.\n",DevCaps.NumSimultaneousRTs,nNumRTs);
    	return D3DTESTINIT_SKIPALL;
	}

	// Make sure we have PS >= 2.0 for multiple render targets
	if (DevCaps.PixelShaderVersion < D3DPS_VERSION(2,0))
	{
		if (bMETex)
		{
			WriteToLog("Device capability not supported: Multiple Element Textures require PixelShaderVersion >= 2.0.\n");
    		return D3DTESTINIT_ABORT;
		}

		// Special case one render target, we don't need a pixel shader
		if (bInvalid || (DevCaps.NumSimultaneousRTs > 1))
		{
			WriteToLog("Device capability not supported: PixelShaderVersion must be >= 2.0.\n");
    		return D3DTESTINIT_SKIPALL;
		}
	}

	// For the MRT fog case, use the max number of supported render targets
	if (!bInvalid && !bMETex && bFog)
		nNumRTs = DevCaps.NumSimultaneousRTs;

	// Check color write caps
	if (bColorWrite)
	{
		// Color write with METs require the IndependentWriteMask
		if (bMETex)
		{
			if (!(DevCaps.PrimitiveMiscCaps & D3DPMISCCAPS_INDEPENDENTWRITEMASKS))
			{
				WriteToLog("Device capability not supported: PrimitiveMiscCaps IndependentWriteMasks.\n");
    			return D3DTESTINIT_SKIPALL;
			}
		}
		// Color write with MRTs depends on NumSimultaneousRTs
		else 
		{
			// Set the number of render targets to be tested to be the max supported
			if (!bInvalid)
				nNumRTs = DevCaps.NumSimultaneousRTs;

			if (nNumRTs == 1)
			{
				// Check the single ColorWriteEnable cap
				if (!(DevCaps.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE))
				{
					WriteToLog("Device capability not supported: PrimitiveMiscCaps ColorWriteEnable.\n");
    				return D3DTESTINIT_SKIPALL;
				}
			}
			else
			{
				// Check the multiple IndependentWriteMask cap
				if (!bInvalid && !(DevCaps.PrimitiveMiscCaps & D3DPMISCCAPS_INDEPENDENTWRITEMASKS))
				{
					// If IndendentWriteMasks not supported but ColorWriteEnable is, just test 1
					if (DevCaps.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE)
					{
						WriteToLog("IndependentWriteMasks not supported for %d render targets, trying single render target.\n", nNumRTs);
						nNumRTs = 1;
					}
					else
					{
						WriteToLog("Device capability not supported: PrimitiveMiscCaps IndependentWriteMasks.\n");
    					return D3DTESTINIT_SKIPALL;
					}
				}
			}
		}
	}

	// Initialize the number of render target texture formats
	nRTFormats = 0;

    // Pair down to just render target textures 
	for (int i=0; i < m_uCommonTextureFormats; i++)
	{
		if (SUCCEEDED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
												USAGE_RENDERTARGET, RTYPE_TEXTURE, m_pCommonTextureFormats[i])))
		{
#ifdef FAST_TEST
			if (m_pCommonTextureFormats[i].d3dfFormat != D3DFMT_A8R8G8B8)
			{
				continue;
			}
#endif

			// Use all render target format for the invalid case
			if (bInvalid)
			{
				// Copy the format
				memset(&(RTFormats[nRTFormats]), 0, sizeof(FORMAT));
				memcpy(&(RTFormats[nRTFormats]), &(m_pCommonTextureFormats[i]), sizeof(FORMAT) - sizeof(FORMAT *));

				// Increment the count
				nRTFormats++;
			}
			else
			{
				// Can't use 16bpp and lower if MRTIndepententBitDepths not set for multiple render targets
				if ((nNumRTs == 1) ||
					(m_pCommonTextureFormats[i].ddpfPixelFormat.dwRGBBitCount > 16) || 
					(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS))
				{
					// Pair down to just post pixel shader blendable textures 
					if (bColorWrite || bBlend || bFog)
					{
						if (SUCCEEDED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format,
																USAGE_QUERY_POSTPIXELSHADER_BLENDING, RTYPE_TEXTURE, m_pCommonTextureFormats[i])))
						{
							// Copy the format
							memset(&(RTFormats[nRTFormats]), 0, sizeof(FORMAT));
							memcpy(&(RTFormats[nRTFormats]), &(m_pCommonTextureFormats[i]), sizeof(FORMAT) - sizeof(FORMAT *));

							// Increment the count
							nRTFormats++;
						}
					}
					else
					{
						// Copy the format
						memset(&(RTFormats[nRTFormats]), 0, sizeof(FORMAT));
						memcpy(&(RTFormats[nRTFormats]), &(m_pCommonTextureFormats[i]), sizeof(FORMAT) - sizeof(FORMAT *));

						// Increment the count
						nRTFormats++;
					}
				}
			}
		}
	}

	// Make sure we get at least one render target texture format
	if (nRTFormats == 0)
	{
		if (bColorWrite || bBlend || bFog)
		{
			WriteToLog("No post pixel shader blendable texture formats found.\n");
			return D3DTESTINIT_SKIPALL;
		}

		WriteToLog("No render target texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

	// Check pixel shader cap and setup our shader
    if (DevCaps.PixelShaderVersion >= D3DPS_VERSION(2,0))
    {
	    LPD3DXBUFFER pCode;
		LPCSTR szPS1[1] = 
		{   "ps_2_0 "
			"dcl v0 "
			"mov oC0, v0 ",
		};
		LPCSTR szPS2[1] = 
		{   "ps_2_0 "
			"dcl v0 "
			"mov oC0, v0 "
			"mov oC1, v0 ",
		};
		LPCSTR szPS3[1] = 
		{   "ps_2_0 "
			"dcl v0 "
			"mov oC0, v0 "
			"mov oC1, v0 "
			"mov oC2, v0 ",
		};
		LPCSTR szPS4[1] = 
		{   "ps_2_0 "
			"dcl v0 "
			"mov oC0, v0 "
			"mov oC1, v0 "
			"mov oC2, v0 "
			"mov oC3, v0 ",
		};
		LPCSTR szPS2_Blend[1] = 
		{   "ps_2_0 "
            "def c0, 1.0, 1.0, 1.0, 1.75 "
			"dcl v0 "
			"mul r0, c0, v0 "
			"mov oC0, r0 "
			"mov oC1, v0 ",
		};
		LPCSTR szPS3_Blend[1] = 
		{   "ps_2_0 "
            "def c0, 1.0, 1.0, 1.0, 1.5 "
			"dcl v0 "
			"mul r0, c0, v0 "
			"mov oC0, r0 "
			"mov oC1, v0 "
			"mov oC2, r0 ",
		};
		LPCSTR szPS4_Blend[1] = 
		{   "ps_2_0 "
            "def c0, 1.0, 1.0, 1.0, 1.75 "
			"dcl v0 "
			"mul r0, c0, v0 "
			"mov oC0, v0 "
			"mov oC1, r0 "
			"mov oC2, v0 "
			"mov oC3, r0 ",
		};

		if (bMETex)
		{
			TCHAR szBuffer[80];

			// Calculate number of elements in the multiple element texture
			switch (RTFormats[m_pCurrentMode->nTextureFormat].d3dfFormat)
			{
				case FMT_MULTI2_ARGB8:  nNumElements = 2;
										break;
// Removed from Dx9
//
//				case FMT_MULTI4_ARGB8:  nNumElements = 4;
//										break;
				default:	FmtToString(FORMAT_TEXTURE,&RTFormats[m_pCurrentMode->nTextureFormat],szBuffer);
							WriteToLog("Unknown Multiple Element Texture format %s.\n", szBuffer);
    						return D3DTESTINIT_ABORT;
			}
		}

		// Chose the appropriate shader based on number of elements / render targets
		switch ((bMETex) ? nNumElements : nNumRTs)
		{
			case 1: // Only use oC0
				D3DXAssembleShader9(szPS1[0], strlen(szPS1[0]), NULL, NULL, 0, &pCode, NULL);

				// Use default texture 256x256
				break;
			case 2: // Use oC0 and oC1
				if (bBlend)
					D3DXAssembleShader9(szPS2_Blend[0], strlen(szPS2_Blend[0]), NULL, NULL, 0, &pCode, NULL);
				else
					D3DXAssembleShader9(szPS2[0], strlen(szPS2[0]), NULL, NULL, 0, &pCode, NULL);

				// Make texture be 128x256
				nWidthRT = 128;
				nHeightRT = 256;
				break;
			case 3: // Use oC0, oC1, and oC2
				if (bBlend)
					D3DXAssembleShader9(szPS3_Blend[0], strlen(szPS3_Blend[0]), NULL, NULL, 0, &pCode, NULL);
				else
					D3DXAssembleShader9(szPS3[0], strlen(szPS3[0]), NULL, NULL, 0, &pCode, NULL);

				// Make texture be 128x128
				nWidthRT = 128;
				nHeightRT = 128;
				break;
			case 4: // Use oC0, oC1, oC2, and oC3
				if (bBlend)
					D3DXAssembleShader9(szPS4_Blend[0], strlen(szPS4_Blend[0]), NULL, NULL, 0, &pCode, NULL);
				else
					D3DXAssembleShader9(szPS4[0], strlen(szPS4[0]), NULL, NULL, 0, &pCode, NULL);

				// Make texture be 128x128
				nWidthRT = 128;
				nHeightRT = 128;
				break;
			default:	
				if (bMETex) WriteToLog("Unknown number of elements %d.\n", nNumElements);
				else WriteToLog("Unknown number of render targets %d.\n", nNumRTs);
    			return D3DTESTINIT_ABORT;
		}

        CreatePixelShader((DWORD *) pCode->GetBufferPointer(), &pPixelShader);

        pCode->Release();
    }

#ifdef FAST_TEST
	m_uCommonZBufferFormats = 1;
#endif

	// Setup the Test range
	if (bMETex || (nNumRTs == 1))
		SetTestRange(1, nRTFormats * m_uCommonZBufferFormats);
	else
		SetTestRange(1, nRTFormats * m_uCommonZBufferFormats * 2);

    // Get the current render target
    if (!GetRenderTarget(&pOriginalRT))
    {
		WriteToLog("GetRenderTarget() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pOriginalRT);
    	return D3DTESTINIT_ABORT;
    }

    // Get the current z buffer
    if (!GetDepthStencilSurface(&pOriginalZ))
    {
		WriteToLog("GetDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pOriginalZ);
		RELEASE(pOriginalRT);
    	return D3DTESTINIT_ABORT;
    }

    // Remember the original format used
    nOriginalZFormat = m_pCurrentMode->nZBufferFormat;

	ReadInteger("Width", nWidthRT, &nWidthRT);
	ReadInteger("Height", nHeightRT, &nHeightRT);

    // Initialize the render target viewport
	ViewportRT.X = 0;    
	ViewportRT.Y = 0;    
	ViewportRT.Width = nWidthRT;
	ViewportRT.Height = nHeightRT;
	ViewportRT.MinZ = 0.f; 
	ViewportRT.MaxZ = 1.f; 

	// Initialize the triangles and grids
	DrawGrid((bMETex) ? nNumElements : nNumRTs);

    // Initialize data
    Tri[0].y = nHeightRT - 1.0f;
    Tri[2].y = nHeightRT - 1.0f;

    Tri[1].x = nWidthRT / 2.0f;
    Tri[2].x = nWidthRT - 1.0f;

    Tri[3].y = nHeightRT - 2.0f;
    Tri[5].y = nHeightRT - 2.0f;

    Tri[4].x = nWidthRT / 2.0f;
    Tri[5].x = nWidthRT - 2.0f;

    return D3DTESTINIT_RUN;
}

bool CMRT::ClearFrame(void)
{
	// Clear to dark blue for the non-invalid fog cases
	if (!bInvalid && bFog)
		return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);

	// Clear to white
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(255,255,255,255), 1.0f, 0);
}

bool CMRT::ExecuteTest(UINT uTestNum)
{
    FORMAT *pFormatRT[4], *pFormatZ;
	bool bSameFormat = true;
 	DWORD dwPasses;
    TCHAR buf[256];
    bool bResult;
	int i, j, k;
	buf[0] = '\0';

    // Cleanup previous texture and zbuffers
    SetTexture(0, NULL);
	for (i=0; i < nNumRTs; i++)
	{
	    RELEASE(pTempRT[i]);
	    RELEASE(pTextureRT[i]);
	}
    RELEASE(pTempZ);

	// Setup our format indexes 
	nTexFormat = ((uTestNum-1) / m_uCommonZBufferFormats) % nRTFormats;
    nZFormat = (uTestNum-1) % m_uCommonZBufferFormats;

	if ((uTestNum > nRTFormats * m_uCommonZBufferFormats) && !bMETex)
	{
		if (bSameFormat) 
			bSameFormat = false;
	}

    // Tell framework what texture formats to use
	for (i=k=0; i < nNumRTs; i++)
	{
		if (bSameFormat)
			pFormatRT[i] = &RTFormats[nTexFormat];
		else
		{
			// Need to special case the format if the device can't handle independant bit depths
			if (!bInvalid && !(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS))
			{
				// Ensure matching bit depths
				for (j=k; j < nRTFormats; j++)
				{
					if (RTFormats[nTexFormat].ddpfPixelFormat.dwRGBBitCount == RTFormats[(nTexFormat+j) % nRTFormats].ddpfPixelFormat.dwRGBBitCount)
					{
						pFormatRT[i] = &RTFormats[(nTexFormat+j) % nRTFormats];
						break;
					}
				}

				if (j == nRTFormats)
					pFormatRT[i] = &RTFormats[nTexFormat];

				k = (j+1) % nRTFormats;
			}
			// If device supports indepentant bit depths, just grab the next format in the list
			else
			{
				pFormatRT[i] = &RTFormats[(nTexFormat+i) % nRTFormats];
			}
		}
	}

    // Tell framework what z format to use
    pFormatZ = &m_pCommonZBufferFormats[nZFormat];

    // Start the log
    DescribeTest(buf, pFormatRT, pFormatZ);
    BeginTestCase(buf,uTestNum);

    // Don't create a new ZBuffer for the original
    if (nZFormat == nOriginalZFormat)
    {
        pTempZ = pOriginalZ;
		pTempZ->AddRef();
    }
	// Create the ZBuffer
	else if (!CreateDepthStencilSurface(nWidthRT, nHeightRT, *pFormatZ, MULTISAMPLE_NONE, &pTempZ))
    {
	    WriteToLog("CreateDepthStencilSurface() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		RELEASE(pTempZ);
    	SkipTests(1);
		return false;
	}

	for (i=0; i < nNumRTs; i++)
	{
	    // Create the texture
	    bResult = CreateTexture(nWidthRT,nHeightRT,1,USAGE_RENDERTARGET,*pFormatRT[i],FMWK_DEFAULT,&pTextureRT[i]);
 
		if (!bResult || (NULL == pTextureRT[i]))
		{
			WriteToLog("CreateTexture(%d) failed with HResult = %s.\n",i,m_pD3D->HResultToString(GetLastError()));

			// Need to fail this case as the CheckDeviceFormat said it was a render target
			Fail();
			return false;
		}
	}

	for (i=0; i < nNumRTs; i++)
	{
		// Get our surface pointers
		pTextureRT[i]->GetSurfaceLevel(0,&pTempRT[i]);

		// Set the texture as the render target
		bResult = SetRenderTarget((DWORD)i, pTempRT[i]);
		SetDepthStencilSurface(pTempZ);	

		// Ignore any errors we get for the Invalid case
		if (bInvalid) 
		{
			bResult = true;
			GetLastError();
		}

		if (!bResult)
		{
			// Reset device to original targets
			SetRenderTarget(pOriginalRT, pOriginalZ);

			WriteToLog("SetRenderTarget(%d) failed with HResult = %s.\n",i,m_pD3D->HResultToString(GetLastError()));
			SkipTests(1);
			return false;
		}
	}

    // Setup the viewport for our new render target
    SetViewport(&ViewportRT);

	// Turn on the pixel shader and set some states
	if (pPixelShader != NULL)
		SetPixelShader(pPixelShader);

	if (bColorWrite)
	{
		// Set the color write render states based on number of elements / render targets
		switch ((bMETex) ? nNumElements : nNumRTs)
		{
			case 1:	SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_ALPHA);
					break;
			case 2:	SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE1, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);
					break;
			case 3:	SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE1, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE2, (DWORD)D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					break;
			case 4:	SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE1, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE2, (DWORD)D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE3, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);
					break;
		}

		// Ignore any errors we get for the Invalid case
		if (bInvalid)
		{
			GetLastError();
		}
	}

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_DISABLE);

	SetRenderState(D3DRS_ZFUNC, (DWORD)D3DCMP_LESS);

	if (bBlend || bFog)
	{
		if (bBlend)
		{
			// Turn on Alpha blending with the most common alpha blend modes
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Turn on Alpha testing, set compare to greater, and give a reference value
			SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)TRUE);
			SetRenderState(D3DRS_ALPHAFUNC, (DWORD)D3DCMP_GREATER);
			SetRenderState(D3DRS_ALPHAREF, (DWORD)116);
		}

		if (bFog)
		{
			// Turn on Fog, use vertex fog, and make yellow our fog color
			SetRenderState(D3DRS_FOGENABLE, (DWORD)TRUE);
			SetRenderState(D3DRS_FOGTABLEMODE, (DWORD)D3DFOG_NONE);
			SetRenderState(D3DRS_FOGCOLOR, (DWORD)RGB_MAKE(255,255,0));
		}

		// Don't check cap for single render target
		if (bMETex || (nNumRTs > 1))
		{
			// If device doesn't support MRTPostPixelShaderBlending, force post PS blending operations off on ref
			if (!(m_pSrcDevice->GetCaps()->dwPrimitiveMiscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING))
			{
				if (bBlend)
				{
					SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE, CD3D_REF);
					SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)FALSE, CD3D_REF);
				}

				if (bFog)
					SetRenderState(D3DRS_FOGENABLE, (DWORD)FALSE, CD3D_REF);
			}
		}
	}
	else
	{
	    SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
	    SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_ONE);
	    SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);
	}


	if (!SetTexture(0,NULL))
	{
		WriteToLog("SetTexture(0,NULL) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		Fail();
		return false;
	}

	if (!bInvalid && !ValidateDevice(&dwPasses))
    {
        // Validate failed, reset device and skip
        SetRenderTarget(pOriginalRT, pOriginalZ);

        // Reset to the original viewport
        SetDefaultViewport();

		// Disable the extra render targets
		for (i=1; i < nNumRTs; i++)
		{
			SetRenderTarget((DWORD)i, NULL);
		}

        // Print the error code and skip
  		WriteToLog("ValidateDevice() failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
		Skip();
        return false;
    }

	// Clear to dark blue
	Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,100,255), 1.0f, 0);

    // Draw to the new render target
	if (BeginScene())
    {
	    RenderPrimitive(D3DPT_TRIANGLELIST,MRT_FVFTLVERTEX,Tri,6,NULL,0,NULL);

        EndScene();
	}

    // Reset to the original render target
	bResult = SetRenderTarget((DWORD)0, pOriginalRT);

    if (!bResult)
    {
		WriteToLog("SetRenderTarget(0,pOriginalRT) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
        return false;
	}

	// Disable the extra render targets
	for (i=1; i < nNumRTs; i++)
	{
		bResult = SetRenderTarget((DWORD)i, NULL);

		// Ignore any errors we get for the Invalid case
		if (bInvalid) 
		{
			bResult = true;
			GetLastError();
		}

		if (!bResult)
		{
			WriteToLog("SetRenderTarget(%d,NULL) failed with HResult = %s.\n",i,m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}

    // Reset to the original z buffer 
	bResult = SetDepthStencilSurface(pOriginalZ);	

    if (!bResult)
    {
		WriteToLog("SetDepthStencilSurface(pOriginalZ) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
        return false;
	}

    // Reset to the original viewport
    SetDefaultViewport();

	// Turn off the pixel shader and reset some states
	if (pPixelShader != NULL)
	    SetPixelShader((CnPixelShader*)NULL);

	if (bColorWrite)
	{
		// Reset the color write render states based on number of elements / render targets
		switch ((bMETex) ? nNumElements : nNumRTs)
		{
			case 1:	SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					break;
			case 2:	SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE1, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					break;
			case 3:	SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE1, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE2, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					break;
			case 4:	SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE1, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE2, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					SetRenderState(D3DRS_COLORWRITEENABLE3, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
					break;
		}

		// Ignore any errors we get for the Invalid case
		if (bInvalid)
		{
			GetLastError();
		}
	}

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);

	SetRenderState(D3DRS_ZFUNC, (DWORD)D3DCMP_LESSEQUAL);

	if (bBlend || bFog)
	{
		if (bBlend)
		{
			// Reset to Alpha Blend defaults
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
			SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO);

			// Reset to Alpha Test defaults
			SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)FALSE);
			SetRenderState(D3DRS_ALPHAFUNC, (DWORD)D3DCMP_ALWAYS);
			SetRenderState(D3DRS_ALPHAREF, (DWORD)0);
		}

		if (bFog)
		{
			// Reset to Fog defaults
			SetRenderState(D3DRS_FOGENABLE, (DWORD)FALSE);
			SetRenderState(D3DRS_FOGTABLEMODE, (DWORD)D3DFOG_NONE);
			SetRenderState(D3DRS_FOGCOLOR, (DWORD)0);
		}
	}
	else
	{
	    SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
	    SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	    SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);
	}

    // Draw to the real target
    if (BeginScene())
    {
		if (bMETex)
		{
			if (!SetTexture(0,pTextureRT[0]))
			{
				WriteToLog("METex SetTexture(0,pTextureRT) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
				Fail();
				return false;
			}

			// Fog cases should only render the first element
			int nIndex = (bFog) ? 1 : nNumElements;

			// Draw based on number of elements
			for (i=0; i < nIndex; i++)
			{
				SetSamplerState(0,SAMP_ELEMENTINDEX,i);
				RenderPrimitive(D3DPT_TRIANGLESTRIP,MRT_FVFTLVERTEX,Quad[i],4,NULL,0,NULL);
			}
		}
		else
		{
			// Fog cases should only render the first render target
			int nIndex = (!bInvalid && bFog) ? 1 : nNumRTs;

			// Draw based on number of render targets
			for (i=0; i < nIndex; i++)
			{
				if (!SetTexture(0,pTextureRT[i]))
				{
					WriteToLog("SetTexture(0,pTextureRT[%d]) failed with HResult = %s.\n",i,m_pD3D->HResultToString(GetLastError()));
					Fail();
					return false;
				}

				RenderPrimitive(D3DPT_TRIANGLESTRIP,MRT_FVFTLVERTEX,Quad[i],4,NULL,0,NULL);
			}
		}

        EndScene();
    }

    return true;
}

bool CMRT::TestTerminate()
{
    // Clean up memory
    SetTexture(0, NULL);

	RELEASE(pPixelShader);

    RELEASE(pTempZ);
    RELEASE(pOriginalZ);
    RELEASE(pOriginalRT);

	for (int i=0; i < nNumRTs; i++)
	{
	    RELEASE(pTempRT[i]);
	    RELEASE(pTextureRT[i]);
	}

    return true;
}

