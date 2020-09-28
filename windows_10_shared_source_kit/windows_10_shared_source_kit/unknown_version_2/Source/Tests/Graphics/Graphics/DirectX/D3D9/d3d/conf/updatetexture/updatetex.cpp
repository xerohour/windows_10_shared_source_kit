//
//  UpdateTex.cpp
//
//  IDirect3DDevice8::UpdateTExture Tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 1998 Microsoft Corporation.
//
//  Original Author :  xfu
//

#include "CUpdateTex.h"
#include "TestCases.h"

#include "dxusage.h"

#include "CBuffers.h"

#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

typedef
struct _BASICTEXTUREVERTEX
{
	FLOAT x, y, z;
	FLOAT nx, ny, nz;
	FLOAT u, v;
}
BASICTEXTUREVERTEX;

BASICTEXTUREVERTEX QuadVertices[] =
{
	{-1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f}
};

BASICTEXTUREVERTEX QuadVertices_POSITIVE_Z[] =
{
	{-1.0f, -1.0f, 0.1f, -31.5f, -31.5f, 32.0f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, -31.5f, 31.5f, 32.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 31.5f, 31.5f, 32.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 31.5f, 31.5f, 32.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 31.5f, -31.5f, 32.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, -31.5f, -31.5f, 32.0f, 0.0f, 1.0f}
};

BASICTEXTUREVERTEX QuadVertices_NEGATIVE_Z[] =
{
	{-1.0f, -1.0f, 0.1f, 31.5f, -31.5f, -32.0f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, 31.5f, 31.5f, -32.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, -31.5f, 31.5f, -32.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, -31.5f, 31.5f, -32.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, -31.5f, -31.5f, -32.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, 31.5f, -31.5f, -32.0f, 0.0f, 1.0f}
};

BASICTEXTUREVERTEX QuadVertices_POSITIVE_Y[] =
{
	{-1.0f, -1.0f, 0.1f, -31.5f, 32.0f, 31.5f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, -31.5f, 32.0f, -31.5f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 31.5f, 32.0f, -31.5f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 31.5f, 32.0f, -31.5f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 31.5f, 32.0f, 31.5f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, -31.5f, 32.0f, 31.5f, 0.0f, 1.0f}
};

BASICTEXTUREVERTEX QuadVertices_NEGATIVE_Y[] =
{
	{-1.0f, -1.0f, 0.1f, -31.5f, -32.0f, -31.5f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, -31.5f, -32.0f, 31.5f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 31.5f, -32.0f, 31.5f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 31.5f, -32.0f, 31.5f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 31.5f, -32.0f, -31.5f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, -31.5f, -32.0f, -31.5f, 0.0f, 1.0f}
};

BASICTEXTUREVERTEX QuadVertices_POSITIVE_X[] =
{
	{-1.0f, -1.0f, 0.1f, 32.0f, -31.5f, 31.5f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, 32.0f, 31.5f, 31.5f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 32.0f, 31.5f, -31.5f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 32.0f, 31.5f, -31.5f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 32.0f, -31.5f, -31.5f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, 32.0f, -31.5f, 31.5f, 0.0f, 1.0f}
};

BASICTEXTUREVERTEX QuadVertices_NEGATIVE_X[] =
{
	{-1.0f, -1.0f, 0.1f, -32.0f, -31.5f, -31.5f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, -32.0f, 31.5f, -31.5f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, -32.0f, 31.5f, 31.5f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, -32.0f, 31.5f, 31.5f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, -32.0f, -31.5f, 31.5f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, -32.0f, -31.5f, -31.5f, 0.0f, 1.0f}
};

static CONST UINT cuiFaceVBSize = sizeof(QuadVertices_POSITIVE_Z);

typedef
struct _CUBE_TEXTURE_FACE
{
	const char *szFaceName;
	const BASICTEXTUREVERTEX *VertexBufferData;
	LPVERTEXBUFFERS RenderCubeVertexBuffer;
}
CUBE_TEXTURE_FACE;

CUBE_TEXTURE_FACE FaceVBs[] =
{
	{"POSITIVE_Z", QuadVertices_POSITIVE_Z, NULL},
	{"NEGATIVE_Z", QuadVertices_NEGATIVE_Z, NULL},
	{"POSITIVE_Y", QuadVertices_POSITIVE_Y, NULL},
	{"NEGATIVE_Y", QuadVertices_NEGATIVE_Y, NULL},
	{"POSITIVE_X", QuadVertices_POSITIVE_X, NULL},
	{"NEGATIVE_X", QuadVertices_NEGATIVE_X, NULL}
};

static CONST UINT cuiGlobalRTWidth = 128;
static CONST UINT cuiGlobalRTHeight = 128;

CONST UINT UpdateTex::cuiLevelDisplaySeparation = 2;
CONST UINT UpdateTex::cuiFaceTopLeftDisplacement = 70;

bool UpdateTex::InitializeAuxiliaryResources()
{
	HRESULT hresult;

	ClearAuxiliaryResources();

	// AndrewLu: Create and initialize a vertex buffer for the texture rendering operation.
	if
	(
		CreateVertexBuffer
		(
			sizeof(QuadVertices),
			0,
			D3DFVF_VERTEX,
			POOL_DEFAULT,
			&pvbAuxiliaryVertexBuffer
		) != true
	)
		WriteToLog(_T("CreateVertexBuffer failed in UpdateTex::InitializeAuxiliaryResources.\n"));
	else
	{
		void *pVertexBufferData = NULL;

		if
		(
			pvbAuxiliaryVertexBuffer->Lock
			(
				0,
				sizeof(QuadVertices),
				&pVertexBufferData,
				0
			) != true
		)
			WriteToLog(_T("Unable to lock vertex buffer in UpdateTex::InitializeAuxiliaryResources.\n"));
		else
		{
			memcpy(
				(void *)pVertexBufferData,
				(void *)QuadVertices,
				sizeof(QuadVertices));

			pvbAuxiliaryVertexBuffer->Unlock();

			// Set a palette to accommodate D3DFMT_P8
			{
				PALETTEENTRY Palette[256];
				UINT uiIndex;

				for (uiIndex = 0; uiIndex < 256; ++uiIndex)
				{
					Palette[uiIndex].peRed = (BYTE)uiIndex;
					Palette[uiIndex].peGreen = (BYTE)uiIndex;
					Palette[uiIndex].peBlue = (BYTE)uiIndex;
					Palette[uiIndex].peFlags = 255;
				}

				if (SetPaletteEntries(0, Palette) != true)
					WriteToLog(_T("SetPaletteEntries failed.\n"));
				else
				{
					if (SetCurrentTexturePalette(0) != true)
						WriteToLog(_T("SetCurrentTexturePalette failed.\n"));
					else
					{
						if (InitializeCubeMapRenderVBs())
						{
							if (InitializeBumpLuminanceSupport())
								return true;

							ClearBumpLuminanceSupport();
							ClearCubeMapVBs();
						}
					}
				}
			}
		}

		pvbAuxiliaryVertexBuffer->Release();
		pvbAuxiliaryVertexBuffer = NULL;
	}

	return false;
}


bool UpdateTex::InitializeCubeMapRenderVBs()
{
	HRESULT hresult;
	UINT uiFaceIndex = 0;

	ClearCubeMapVBs();

	for (;;)
	{
		void *pVertexBufferData;

		// AndrewLu: Create and initialize a vertex buffer for the face rendering operation.
		if
		(
			CreateVertexBuffer
			(
				cuiFaceVBSize,
				0,
				D3DFVF_VERTEX,
				POOL_DEFAULT,
				&((FaceVBs[uiFaceIndex]).RenderCubeVertexBuffer)
			) != true
		)
		{
			WriteToLog(_T("CreateVertexBuffer failed in UpdateTex::InitializeCubeMapRenderVBs.\n"));
			break;
		}

		if
		(
			((FaceVBs[uiFaceIndex]).RenderCubeVertexBuffer)->Lock
			(
				0,
				cuiFaceVBSize,
				&pVertexBufferData,
				0
			) != true
		)
		{
			WriteToLog(_T("Unable to lock vertex buffer in UpdateTex::InitializeCubeMapRenderVBs.\n"));
			break;
		}


		// 
		// VertexBufferData
		memcpy((void *)pVertexBufferData, (void *)((FaceVBs[uiFaceIndex]).VertexBufferData), cuiFaceVBSize);
		((FaceVBs[uiFaceIndex]).RenderCubeVertexBuffer)->Unlock();

		if ((++uiFaceIndex) == 6)
			return true;
	}

	ClearCubeMapVBs();

	return false;
}


bool UpdateTex::InitializeBumpLuminanceSupport()
{
	ClearBumpLuminanceSupport();

	if
	(
		CreateTex
		(
			D3DRTYPE_TEXTURE,
			D3DFMT_A8R8G8B8, 
			D3DPOOL_DEFAULT,
			GENERICTEXSIZE,
			GENERICTEXLEVEL,
			0,
			SEQ,
			(CBaseTex**)(&pBumpMapAuxBase),
			TRUE
		)
	)
		if
		(
			CreateTex
			(
				D3DRTYPE_TEXTURE,
				D3DFMT_A8R8G8B8, 
				D3DPOOL_DEFAULT,
				GENERICTEXSIZE,
				GENERICTEXLEVEL,
				0,
				SEQ,
				(CBaseTex**)(&pBumpMapAuxEnv),
				TRUE
			)
		)
			return true;

	return false;
}

void UpdateTex::ClearAuxiliaryResources()
{
	if (pvbAuxiliaryVertexBuffer)
	{
		pvbAuxiliaryVertexBuffer->Release();
		pvbAuxiliaryVertexBuffer = NULL;
	}

	ClearCubeMapVBs();
	ClearBumpLuminanceSupport();
}

void UpdateTex::ClearCubeMapVBs()
{
	UINT uiFaceIndex;

	for (uiFaceIndex = 0; uiFaceIndex < 6; ++uiFaceIndex)
	{
		if ((FaceVBs[uiFaceIndex]).RenderCubeVertexBuffer)
		{
			(FaceVBs[uiFaceIndex]).RenderCubeVertexBuffer->Release();
			(FaceVBs[uiFaceIndex]).RenderCubeVertexBuffer = NULL;
		}
	}
}

void UpdateTex::ClearBumpLuminanceSupport()
{
	if (pBumpMapAuxBase)
	{
		delete pBumpMapAuxBase;
		pBumpMapAuxBase = NULL;
	}

	if (pBumpMapAuxEnv)
	{
		delete pBumpMapAuxEnv;
		pBumpMapAuxEnv = NULL;
	}
}

bool UpdateTex::Setup()
{
	bool bReturn = false;

	if (InitializeAuxiliaryResources())
	{
		CSurface *psurfBackBuffer = NULL;
		// Obtain back buffer dimensions.  This is to set a viewport that can be
		// used to clear the back buffer in preparation for each test case.

		if (SUCCEEDED(m_pSrcDevice->GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &psurfBackBuffer)))
		{
			SURFACEDESC sdBBSurfDesc;

			if (SUCCEEDED(psurfBackBuffer->GetDesc(&sdBBSurfDesc)))
			{
				vpEntireBackBuffer.X = 0;
				vpEntireBackBuffer.Y = 0;
				vpEntireBackBuffer.Width = sdBBSurfDesc.Width;
				vpEntireBackBuffer.Height = sdBBSurfDesc.Height;
				vpEntireBackBuffer.MinZ = 0.0;
				vpEntireBackBuffer.MaxZ = 1.0;

				bReturn = true;
			}

			psurfBackBuffer->Release();
		}
	}

	m_pFramework->GetImageCmp()->SetMinPixels(0);

	return bReturn;
}

void UpdateTex::Cleanup()
{
	if (m_pSrcTex) {delete m_pSrcTex; m_pSrcTex = NULL;}
	if (m_pDestTex) {delete m_pDestTex; m_pDestTex = NULL;}
	if (m_pBogusTex) {delete m_pBogusTex; m_pBogusTex = NULL;}

	ClearAuxiliaryResources();

	return CD3DTest::Cleanup();
}

UINT UpdateTex_MipTextures::TestInitialize()
{
	HRESULT			hr;
	UINT			uResult;

	// Determine the format for this set of test cases, as determined by -tex or
	// the framework's format cycling.
	formatCurrent = *(GetCurrentTextureFormat());

    if ((uResult = CUpdateTexBase::TestInitialize()) == D3DTESTINIT_RUN)
	{
		hr = m_pSrcDevice->GetDeviceCaps(&capsSrcDevice);

		uiManagedTextureTestCases = TOTALCASES * cuiNoOfDimensionAdjustments * GENERICTEXLEVEL;

		uiUpdateTextureTestCases = uiManagedTextureTestCases * cuiNoOfSourceUsages * cuiNoOfDestUsages;

		uiManagedTextureTestCases = 0;

		uiTotalTestCases = uiManagedTextureTestCases + uiUpdateTextureTestCases;
		ClearRangeList();
		SetTestRange(1,uiTotalTestCases);

		//m_pFramework->GetImageCmp()->SetPassRequired(0.98f);
	}
	else
	{
		SetTestRange(1, 1);
		uResult = D3DTESTINIT_SKIPALL;
	}

	return uResult;    
}

UINT UpdateTex_CubeTextures::TestInitialize()
{
	HRESULT			hr;
	UINT			uResult;

	// Determine the format for this set of test cases, as determined by -tex or
	// the framework's format cycling.
	formatCurrent = *(GetCurrentTextureFormat());

    if ((uResult = CUpdateTexBase::TestInitialize()) == D3DTESTINIT_RUN)
	{
		hr = m_pSrcDevice->GetDeviceCaps(&capsSrcDevice);

#if FAST_TEST
        uiManagedTextureTestCases = TOTALCASES * cuiNoOfDimensionAdjustments * GENERICTEXLEVEL * 2;
#else
		uiManagedTextureTestCases = TOTALCASES * cuiNoOfDimensionAdjustments * GENERICTEXLEVEL * 6;
#endif

		uiUpdateTextureTestCases = uiManagedTextureTestCases * cuiNoOfSourceUsages * cuiNoOfDestUsages;

		uiManagedTextureTestCases = 0;

		uiTotalTestCases = uiManagedTextureTestCases + uiUpdateTextureTestCases;
		ClearRangeList();
		SetTestRange(1,uiTotalTestCases);

		//m_pFramework->GetImageCmp()->SetPassRequired(0.98f);
	}
	else
	{
		SetTestRange(1, 1);
		uResult = D3DTESTINIT_SKIPALL;
	}

	return uResult;    
}


bool UpdateTex::TestTerminate(void)
{
	ClearAuxiliaryResources();

	return true;
}


HRESULT UpdateTex::RenderTextureSurface(LPSURFACES pDestinationRT, LPTEXTURES pRenderTexture, UINT uiTextureLevel)
{
	SURFACEDESC sdMipLevel;

	// Obtain the dimensions of the mip level surface and use them to set up the viewport
	// for the texture-rendering step.
	if (pRenderTexture->GetLevelDesc(uiTextureLevel, &sdMipLevel) != true)
	{
		WriteToLog(_T("GetDesc failed in UpdateTex::RenderTextureSurface.\n"));
		return E_FAIL;
	}

	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;
		LPSURFACES pCurrentRenderTarget = NULL;
		LPSURFACES pCurrentDepthStencil = NULL;

		// Set the stream source and vertex shader.
		SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		//m_pDevice->SetVertexShader(D3DFVF_VERTEX);
		SetFVF(D3DFVF_VERTEX);

		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		GetRenderTarget(&pCurrentRenderTarget);
		GetDepthStencilSurface(&pCurrentDepthStencil);

		// Set the render target and viewport.
		SetRenderTarget(pDestinationRT, NULL);
		vpRenderTexture.X = (vpRenderTexture.Y = 0);
		vpRenderTexture.Width = sdMipLevel.Width;
		vpRenderTexture.Height = sdMipLevel.Height;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;
		SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		SetTexture(0, pRenderTexture);
		SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Set the sampler state to favour the desired miplevel.
		SetSamplerState(0, SAMP_MAXMIPLEVEL, uiTextureLevel);

		// Enable mipmapping ...
		SetSamplerState(0, SAMP_MIPFILTER, TEXF_LINEAR /* 2004/07/15 */);

		// Clear the viewport and render the texture.
		BeginScene();
		Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x01, 0x01, 0x01),
			0.5f,
			0
		);
		DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		EndScene();

		// Restore the original render target and depth stencil.
		SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget->Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil->Release();
	}

	return S_OK;
}


HRESULT UpdateTex::RenderEntireTexture(LPTEXTURES pRenderTexture)
{
	HRESULT hresult;
	UINT uiTotalLevels, uiLevelIndex;
	RECT rectLevel;

	rectLevel.left = (rectLevel.top = 0);

	uiTotalLevels = (UINT)(pRenderTexture->GetLevelCount());

	for (uiLevelIndex = 0; uiLevelIndex < uiTotalLevels; ++uiLevelIndex)
	{
		SURFACEDESC sd;

		pRenderTexture->GetLevelDesc(uiLevelIndex, &sd);
		rectLevel.right = rectLevel.left + sd.Width;
		rectLevel.bottom = rectLevel.top + sd.Height;

		if (FAILED((hresult = RenderTextureLevelToRect(pRenderTexture, uiLevelIndex, rectLevel))))
			break;

		rectLevel.left = rectLevel.right + cuiLevelDisplaySeparation;
	}

	return hresult;
}


void UpdateTex::SetTextureStageStates(LPTEXTURES pTestTexture, UINT uiTextureLevel)
{
	// Set the texture stage states, based on the "render category" of the test texture.

	switch(TestTextureRenderProcedure)
	{
		case rpMipMap:
			// The test texture is a "standard" texture.

			SetTexture(0, pTestTexture);
			SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(0, SAMP_MAXMIPLEVEL, uiTextureLevel);
			// Enable mipmapping ...
			SetSamplerState(0, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(0, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(0, SAMP_MAGFILTER, MipFilteringType);


			SetTextureStageState(1, D3DTSS_COLOROP, (DWORD)D3DTOP_DISABLE);
			SetTexture(1, NULL);
		break;

		case rpLuminance:
			// Determine whether the texture can be used as a luminance map.  If not,
			// try using it as a conventional texture.

			SetTexture(0, pBumpMapAuxBase->GetLPTEXTURES());
			SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(0, SAMP_MAXMIPLEVEL, uiTextureLevel);
			// Enable mipmapping ...
			SetSamplerState(0, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(0, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(0, SAMP_MAGFILTER, MipFilteringType);

			SetTexture(1, pTestTexture);
			SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(1, D3DTSS_COLOROP, (DWORD)D3DTOP_BUMPENVMAPLUMINANCE);
			SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);

			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(1, SAMP_MAXMIPLEVEL, uiTextureLevel);
			// Enable mipmapping ...
			SetSamplerState(1, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(1, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(1, SAMP_MAGFILTER, MipFilteringType);


			SetTexture(2, pBumpMapAuxEnv->GetLPTEXTURES());
			SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(2, D3DTSS_COLOROP, (DWORD)D3DTOP_ADD);
			SetTextureStageState(2, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(2, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);

			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(2, SAMP_MAXMIPLEVEL, uiTextureLevel);
			// Enable mipmapping ...
			SetSamplerState(2, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(2, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(2, SAMP_MAGFILTER, MipFilteringType);
		break;


		case rpBumpMap:
			SetTexture(0, pBumpMapAuxBase->GetLPTEXTURES());
			SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(0, SAMP_MAXMIPLEVEL, uiTextureLevel);
			// Enable mipmapping ...
			SetSamplerState(0, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(0, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(0, SAMP_MAGFILTER, MipFilteringType);

			SetTexture(1, pTestTexture);
			SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(1, D3DTSS_COLOROP, (DWORD)D3DTOP_BUMPENVMAP);
			SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);

			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(1, SAMP_MAXMIPLEVEL, uiTextureLevel);
			// Enable mipmapping ...
			SetSamplerState(1, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(1, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(1, SAMP_MAGFILTER, MipFilteringType);


			SetTexture(2, pBumpMapAuxEnv->GetLPTEXTURES());
			SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(2, D3DTSS_COLOROP, (DWORD)D3DTOP_ADD);
			SetTextureStageState(2, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(2, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);

			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(2, SAMP_MAXMIPLEVEL, uiTextureLevel);
			// Enable mipmapping ...
			SetSamplerState(2, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(2, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(2, SAMP_MAGFILTER, MipFilteringType);
		break;


		case rpBumpLuminance:
			SetTexture(0, pBumpMapAuxBase->GetLPTEXTURES());
			SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(0, SAMP_MAXMIPLEVEL, uiTextureLevel);

			// Enable mipmapping ...
			SetSamplerState(0, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(0, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(0, SAMP_MAGFILTER, MipFilteringType);

			SetTexture(1, pTestTexture);
			SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(1, D3DTSS_COLOROP, (DWORD)D3DTOP_BUMPENVMAPLUMINANCE);
			SetTextureStageState(1, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(1, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);

			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(1, SAMP_MAXMIPLEVEL, uiTextureLevel);

			// Enable mipmapping ...
			SetSamplerState(1, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(1, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(1, SAMP_MAGFILTER, MipFilteringType);

			SetTexture(2, pBumpMapAuxEnv->GetLPTEXTURES());
			SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(2, D3DTSS_COLOROP, (DWORD)D3DTOP_ADD);
			SetTextureStageState(2, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			SetTextureStageState(2, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);

			// Set the sampler state to favour the desired miplevel.
			SetSamplerState(2, SAMP_MAXMIPLEVEL, uiTextureLevel);
			// Enable mipmapping ...
			SetSamplerState(2, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);
			SetSamplerState(2, SAMP_MINFILTER, MipFilteringType);
			SetSamplerState(2, SAMP_MAGFILTER, MipFilteringType);
		break;
	}
}


HRESULT UpdateTex::RenderTextureLevelToRect(LPTEXTURES pRenderTexture, UINT uiTextureLevel, RECT rectDest)
{
	HRESULT hresult;
	SURFACEDESC sdMipLevel;

	// Obtain the dimensions of the mip level surface and use them to set up the viewport
	// for the texture-rendering step.
	if (pRenderTexture->GetLevelDesc(uiTextureLevel, &sdMipLevel) != true)
		WriteToLog(_T("GetDesc failed in UpdateTex::RenderTextureSurface.\n"));
	else
	{
		DWORD dwNoOfPasses = 0;
		VIEWPORT vpRenderTexture;

		RENDERCATEGORY rcTestTextureCategory;

		rcTestTextureCategory = GetFormatRenderCategory((sdMipLevel.Format).d3dfFormat);

		// Set the stream source and vertex shader.
		SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		SetFVF(D3DFVF_VERTEX);

		// Set the viewport.
		vpRenderTexture.X = rectDest.left;
		vpRenderTexture.Y = rectDest.top;
		vpRenderTexture.Width = rectDest.right - rectDest.left;
		vpRenderTexture.Height = rectDest.bottom - rectDest.top;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;
		SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.

		SetTextureStageStates(/*rcTestTextureCategory,*/ pRenderTexture, uiTextureLevel);

		// Validate the ability of the "source" device to render the primitive with the
		// current texture stage settings.
		if (FAILED((hresult = m_pSrcDevice->ValidateDevice(&dwNoOfPasses))))
			return hresult;

		// Clear the viewport and render the texture.
		BeginScene();
		Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x01, 0x01, 0x01),
			0.5f,
			0
		);
		DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		EndScene();
	}

	return hresult;
}

HRESULT UpdateTex::RenderEntireFace(LPCUBETEXTURES pRenderTexture, CUBEMAP_FACES cmFace)
{
	HRESULT hresult;
	UINT uiTotalLevels, uiLevelIndex;
	RECT rectLevel;

	rectLevel.left = 0;
	rectLevel.top = ((UINT)cmFace) * cuiFaceTopLeftDisplacement;

	uiTotalLevels = (UINT)(pRenderTexture->GetLevelCount());

	for (uiLevelIndex = 0; uiLevelIndex < uiTotalLevels; ++uiLevelIndex)
	{
		SURFACEDESC sd;

		pRenderTexture->GetLevelDesc(uiLevelIndex, &sd);
		rectLevel.right = rectLevel.left + sd.Width;
		rectLevel.bottom = rectLevel.top + sd.Height;

		if (FAILED((hresult = RenderFaceLevelToRect(pRenderTexture, uiLevelIndex, rectLevel, cmFace))))
			break;

		rectLevel.left = rectLevel.right + cuiLevelDisplaySeparation;
	}

	return hresult;
}

HRESULT UpdateTex::RenderFaceLevelToRect(LPCUBETEXTURES pRenderTexture, UINT uiTextureLevel, RECT rectDest, CUBEMAP_FACES cmFace)
{
	HRESULT hresult = D3D_OK;
	VIEWPORT vpRenderTexture;

	vpRenderTexture.X = rectDest.left;
	vpRenderTexture.Y = rectDest.top;
	vpRenderTexture.Width = rectDest.right - rectDest.left;
	vpRenderTexture.Height = rectDest.bottom - rectDest.top;
	vpRenderTexture.MinZ = 0.0f;
	vpRenderTexture.MaxZ = 1.0f;
	SetViewport(&vpRenderTexture);

	// Set the stream source and vertex shader.
	SetStreamSource(0, (FaceVBs[((UINT)cmFace)]).RenderCubeVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
	SetFVF(D3DFVF_VERTEX);

	// Define the role of the texture in the rendering operation.
	SetTexture(0, pRenderTexture);
	SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
	SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
	SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

	// Set the sampler state to favour the desired miplevel.
	SetSamplerState(0, SAMP_MAXMIPLEVEL, uiTextureLevel);

	// Enable mipmapping ...
	SetSamplerState(0, SAMP_MIPFILTER, MipFilteringType /* 2004/07/15 */);

	//SetSamplerState(0, SAMP_MINFILTER, TEXF_LINEAR);
	//SetSamplerState(0, SAMP_MAGFILTER, TEXF_LINEAR);


	// Clear the viewport and render the texture.
	Clear
	(
		0,
		NULL,
		D3DCLEAR_TARGET,
		0xffffffff,
		//RGB_MAKE(0x00, 0x00, 0x00),
		0.0f,
		0
	);
	BeginScene();
	DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	EndScene();

	return hresult;
}


UpdateTex::UpdateTex()
{
	UINT uiFaceIndex;

	//m_szTestName = _T("UpdateTexture Tests");

	pvbAuxiliaryVertexBuffer = NULL;

	pBumpMapAuxBase = NULL;
	pBumpMapAuxEnv = NULL;

	//m_Options.D3DOptions.nBackBufferHeight  = 450;

	(m_Options.ModeOptions).dwTexExclude = PF_YUV;
	(m_Options.ModeOptions).TextureFmt = FMT_ALL;

	// Re-enable this statement to force WHQL exeuction against D3D8.
	//m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91 | DXVERSION_8_OR_81;
}

UpdateTex_MipTextures::UpdateTex_MipTextures()
{
	m_szTestName = _T("UpdateTexture (MipMaps)");
	m_szCommandKey = _T("Update");
	iResType = 0;
}

UpdateTex_MipTextures::~UpdateTex_MipTextures()
{
}

bool UpdateTex_MipTextures::CapsCheck()
{
	RENDERCATEGORY rcCurrentTexture;
	FORMAT TestedFormat = *(GetCurrentTextureFormat());

	if ((rcCurrentTexture = GetFormatRenderCategory(TestedFormat.d3dfFormat)) != rcMultiElement)
	{
		DEVICE_CREATION_PARAMETERS dcp;
		DISPLAYMODE Mode;

		m_pSrcDevice->GetCreationParameters(&dcp);
		m_pD3D->GetAdapterDisplayMode(dcp.AdapterOrdinal, &Mode);

		if (SUCCEEDED((m_pD3D->CheckDeviceFormat(dcp.AdapterOrdinal, dcp.DeviceType, Mode.Format, 0, RTYPE_TEXTURE, TestedFormat))))
		if (SUCCEEDED((m_pD3D->CheckDeviceFormat(dcp.AdapterOrdinal, dcp.DeviceType, Mode.Format, USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, TestedFormat))))
		{
			TestTextureRenderProcedure = rpMipMap;

			MipFilteringType = TEXF_POINT;

			if (SUCCEEDED((m_pD3D->CheckDeviceFormat(dcp.AdapterOrdinal, dcp.DeviceType, Mode.Format, USAGE_QUERY_FILTER, RTYPE_TEXTURE, TestedFormat))))
				MipFilteringType = TEXF_LINEAR;


			//TEXF_LINEAR

			/*
			// If the device has the appropriate capabilities, select a more appropriate
			// texture-rendering procedure.
			if (SUCCEEDED((m_pD3D->CheckDeviceFormat(dcp.AdapterOrdinal, dcp.DeviceType, Mode.Format, USAGE_QUERY_LEGACYBUMPMAP, RTYPE_TEXTURE, TestedFormat))))
			{
				m_pSrcDevice->GetDeviceCaps(&capsSrcDevice);

				if (capsSrcDevice.MaxTextureBlendStages >= 3)
				{
					switch(rcCurrentTexture)
					{
					case rcLuminance:
						if
						(
							((capsSrcDevice.TextureOpCaps) & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE) &&
							((capsSrcDevice.TextureOpCaps) & D3DTEXOPCAPS_ADD)
						)
							TestTextureRenderProcedure = rpLuminance;
						break;
					case rcBumpMap:
						if
						(
							((capsSrcDevice.TextureOpCaps) & D3DTEXOPCAPS_BUMPENVMAP) &&
							((capsSrcDevice.TextureOpCaps) & D3DTEXOPCAPS_ADD)
						)
							TestTextureRenderProcedure = rpBumpMap;
						break;
					case rcBumpLuminance:
						if
						(
							((capsSrcDevice.TextureOpCaps) & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE) &&
							((capsSrcDevice.TextureOpCaps) & D3DTEXOPCAPS_ADD)
						)
							TestTextureRenderProcedure = rpBumpLuminance;
						break;
					default:
						break;
					}
				}
			}
			*/

			return true;
		}
	}

	return false;
}

UpdateTex_CubeTextures::UpdateTex_CubeTextures()
{
	m_szTestName = _T("UpdateTexture (Cubemaps)");
	m_szCommandKey = _T("Cube");
	iResType = 1;
}

UpdateTex_CubeTextures::~UpdateTex_CubeTextures()
{
}

bool UpdateTex_CubeTextures::CapsCheck()
{
	DEVICE_CREATION_PARAMETERS dcp;
	DISPLAYMODE Mode;

	TestTextureRenderProcedure = rpMipMap;

	m_pSrcDevice->GetCreationParameters(&dcp);
	m_pD3D->GetAdapterDisplayMode(dcp.AdapterOrdinal, &Mode);

	if (GetFormatRenderCategory(GetCurrentTextureFormat()->d3dfFormat) != rcMipMap)
		return false;

	if (SUCCEEDED(m_pD3D->CheckDeviceFormat(dcp.AdapterOrdinal, dcp.DeviceType, Mode.Format, 0, RTYPE_CUBETEXTURE, *(GetCurrentTextureFormat()))))
	if (SUCCEEDED(m_pD3D->CheckDeviceFormat(dcp.AdapterOrdinal, dcp.DeviceType, Mode.Format, USAGE_QUERY_WRAPANDMIP, RTYPE_CUBETEXTURE, *(GetCurrentTextureFormat()))))
		return true;
	return false;
}

bool UpdateTex::ExecuteTest
(
	UINT nTest,
	UINT uiMipLevelIndex,
	UINT uiDimensionAdjustmentIndex,
	UINT uiDestUsageIndex,
	UINT uiSourceUsageIndex,
	UINT iTestCase
)
{
	UINT iFormat;
	BOOL bRet = TRUE;
	BOOL bRet1 = TRUE;
	BOOL bRet2 = TRUE;
	BOOL bRet3 = TRUE;
	BOOL bRun = FALSE;
	HRESULT hr;

	static TCHAR szTitle[512];
	static char szFormatString[256];

	PTESTCASE pTC;

	SetViewport(&vpEntireBackBuffer);
	ClearFrame();

	// The test originally has three loops as above. It has been modified upon WHQL user's request to seperate
	// test cases. Now, this function will be called TOTALCASES*TOTALTYPES*TOTALFORMATS times, each time with a unique
	// nTest #. To make minimal changes upon previous code, iTestCase, iResType, iFormat are still used now.
	// They are extracted from nTest, instead of being the loop counter now, though. The do... while (0)  thing is 
	// also to make the change minimal.

	// The test does not currently address cube textures whose formats are bump / luminance formats.
	if
	(
		((CRESOURCETYPE[iResType]).Type == D3DRTYPE_CUBETEXTURE)
		&&
		(GetFormatRenderCategory(formatCurrent.d3dfFormat) != rcMipMap)
	)
	{
		WriteToLog("Skipping bump / luminance formats for cube texture.\n");
		SkipTestRange(nTest, nTest);
		return false;
	}

	pTC=(PTESTCASE)&(tc[iTestCase]);
	uiWidthAdjustment = (DimensionAdjustments[uiDimensionAdjustmentIndex]).uiWidthAdjustment;
	uiHeightAdjustment = (DimensionAdjustments[uiDimensionAdjustmentIndex]).uiHeightAdjustment;

	//_tcscpy (szTitle, bTestUpdateTexture ? TEXT("UPDATETEXTURE") : TEXT("MANAGED TEXTURES"));
	//_tcscat (szTitle, TEXT(","));
	_tcscpy (szTitle, CRESOURCETYPE[iResType].Name);
	_tcscat (szTitle, TEXT(","));
	if ((CRESOURCETYPE[iResType]).Type == D3DRTYPE_CUBETEXTURE)
	{
		_tcscat(szTitle, (FaceVBs[uiCubeFaceIndex]).szFaceName);
		_tcscat (szTitle, TEXT(","));
	}

	{
		char szMipLevelString[3];
		sprintf(szMipLevelString, "%d", uiMipLevelIndex);
		_tcscat(szTitle, szMipLevelString);
		_tcscat (szTitle, TEXT(","));
	}

	FmtToString(FORMAT_TEXTURE, &formatCurrent, szFormatString);
	_tcscat (szTitle, szFormatString);

	_tcscat (szTitle, TEXT(","));
	_tcscat (szTitle, pTC->szTitle);

	BeginTestCase(szTitle, nTest);

	do
	{
		{
			{
				UINT uiFormatAsInt, uiBitsPerHorizontalCoordinate;

				m_pSrcTex=NULL;
				m_pDestTex=NULL;
				m_pBogusTex=NULL;

				uiFormatAsInt = (UINT)(formatCurrent.d3dfFormat);

				uiBitsPerHorizontalCoordinate = formatCurrent.ddpfPixelFormat.dwRGBBitCount;

				if (!uiBitsPerHorizontalCoordinate)
				{
					WriteToLog("Obtained value of zero for 'bits per horizontal coordinate'.  Skipping.\n");
					SkipTestRange(nTest, nTest);
					break;
				}

				// Skip the test case if m_bCanDoNonSquare is FALSE and either "dimension
				// adjustment" is non-zero.
				if ((!m_bCanDoNonSquare) && ((uiWidthAdjustment != 0) || (uiHeightAdjustment != 0)))
				{
					WriteToLog("m_bCanDoNonSquare is FALSE, non-zero dimension adjustment.  Skipping.\n");
					SkipTestRange(nTest, nTest);
					break;
				}

				// Skip the test case if bTestUpdateTexture is 'false', but the 'tc' array
				// entry indicates that the test case isn't applicable to managed textures.
				if ((!bTestUpdateTexture) && (pTC->bSkipForManagedTextures))
				{
					SkipTestRange(nTest, nTest);
					break;
				}

				// Skip the test case if the test case record specifies a number of destination
				// levels greater than 1 and the usage flags for the destination texture include
				// USAGE_AUTOGENMIPMAP.
				if ((dwDestUsages[uiDestUsageIndex] & USAGE_AUTOGENMIPMAP) && (pTC->uDestLevels >= 2))
				{
					SkipTestRange(nTest, nTest);
					break;
				}

				// skip the invalid parameter test if with retail build.
				if (!m_bDebugMode && !pTC->bExpectSucceed)
				{
					WriteToLog("Invalid parameter test case run against retail runtime.  Skipping.\n");
					SkipTestRange(nTest, nTest);
					continue;
				}

				// determine levels depending on device cap
				if (CRESOURCETYPE[iResType].Type==D3DRTYPE_TEXTURE)
				{
					if (!m_bCanDoMipmap)
					{
						if (pTC->uSrcLevels==0)
							pTC->uSrcLevels=1;
						else if (pTC->uSrcLevels !=1)
						{
							WriteToLog (TEXT("Device doesn't support mipmap, skip test\n"));
							SkipTestRange(nTest, nTest);
							break;
						}

						if (pTC->uDestLevels==0)
							pTC->uDestLevels=1;
						else if (pTC->uDestLevels !=1)
						{
							WriteToLog (TEXT("Device doesn't support mipmap, skip test\n"));
							SkipTestRange(nTest, nTest);
							break;
						}
					}
				}

				else if (CRESOURCETYPE[iResType].Type==D3DRTYPE_CUBETEXTURE)
				{
					if (!m_bCanDoCubeMap)
					{
						WriteToLog (TEXT("Device doesn't support cubemap, skip test\n"));
						SkipTestRange(nTest, nTest);
						break;
					}

					if (!m_bCanDoCubeMipmap)
					{
						if (pTC->uSrcLevels==0)
							pTC->uSrcLevels=1;
						else if (pTC->uSrcLevels !=1)
						{
							WriteToLog (TEXT("Device doesn't support mip cube map, skip test\n"));
							SkipTestRange(nTest, nTest);
							break;
						}

						if (pTC->uDestLevels==0)
							pTC->uDestLevels=1;
						else if (pTC->uDestLevels !=1)
						{
							WriteToLog (TEXT("Device doesn't support cube mipmap, skip test\n"));
							SkipTestRange(nTest, nTest);
							break;
						}
					}
				}

				else if (CRESOURCETYPE[iResType].Type==D3DRTYPE_VOLUMETEXTURE)
				{
					if (!m_bCanDoVolMap)
					{
						WriteToLog (TEXT("Device doesn't support volume map, skip test\n"));
						SkipTestRange(nTest, nTest);
						break;
					}

					if (!m_bCanDoVolMipmap)
					{
						if (pTC->uSrcLevels==0)
							pTC->uSrcLevels=1;
						else if (pTC->uSrcLevels !=1)
						{
							WriteToLog (TEXT("Device doesn't support mip volume map, skip test\n"));
							SkipTestRange(nTest, nTest);
							break;
						}

						if (pTC->uDestLevels==0)
							pTC->uDestLevels=1;
						else if (pTC->uDestLevels !=1)
						{
							WriteToLog (TEXT("Device doesn't support volume mipmap, skip test\n"));
							SkipTestRange(nTest, nTest);
							break;
						}
					}
				}

				bRun=TRUE;

				//
				//	Create texture
				//

				if (bTestUpdateTexture)
				{
					// Create the three textures used to test UpdateTexture:
					// - A system memory source texture;
					// - A default memory destination texture;
					// - A default memory 'bogus' texture.  This is just the target texture for
					//   a preliminary call to UpdateTexture, effected for the sole purpose of
					//   ensuring that the source texture has no outstanding dirty regions.

					bRet1= CreateTex (CRESOURCETYPE[iResType].Type, formatCurrent.d3dfFormat, D3DPOOL_SYSTEMMEM,pTC->uSrcSize,pTC->uSrcLevels, dwSourceUsages[uiSourceUsageIndex], SEQ,&m_pSrcTex);
					bRet2= CreateTex (CRESOURCETYPE[iResType].Type, formatCurrent.d3dfFormat, D3DPOOL_DEFAULT ,pTC->uDestSize,pTC->uDestLevels, dwDestUsages[uiDestUsageIndex],SEQ,&m_pDestTex);
					bRet3= CreateTex (CRESOURCETYPE[iResType].Type, formatCurrent.d3dfFormat, D3DPOOL_DEFAULT ,pTC->uSrcSize,pTC->uSrcLevels, /*USAGE_HEAVYWEIGHT*/0,SEQ,&m_pBogusTex);

					if (!bRet1 || !bRet2 || !bRet3)
					{
						WriteToLog (TEXT("Cannot create texture, skip test \n"));
						SkipTestRange(nTest, nTest);
						bRun=FALSE;
					}
				}
				else
				{
					// Create a single, managed texture.
					bRet1= CreateTex (CRESOURCETYPE[iResType].Type, formatCurrent.d3dfFormat, D3DPOOL_MANAGED,pTC->uSrcSize,pTC->uSrcLevels, 0, SEQ,&m_pSrcTex);

					if (!bRet1)
					{
						WriteToLog (TEXT("Cannot create texture, skip test \n"));
						SkipTestRange(nTest, nTest);
						bRun=FALSE;
					}

					m_pDestTex = m_pSrcTex;
				}

				if (bRun) m_pDestTex->SetSrcTex(m_pSrcTex);

				if (bRun)
				{
					if (bTestUpdateTexture)
					// The test case applies to a pair of textures.  Ensure that the source
					// texture has no associated dirty rectangles.
					{
						//
						// Here, the UpdateTexture is called, to make the m_pSrcTex clean. 
						//
						try
						{
							bool bUpdateTextureResult = UpdateTexture(m_pSrcTex->GetLPTEXTURES(), m_pBogusTex->GetLPTEXTURES());
							if (bUpdateTextureResult != true)
							{
								WriteToLog (TEXT("UpdateTexture failed with m_pBogusTex\n"));
								SkipTestRange(nTest, nTest);
								bRun = FALSE;
							}
						}
						catch (...)
						{
							WriteToLog (TEXT("Unhandled exception when calling UpdateTexture m_pBogusTex\n"));
							SkipTestRange(nTest, nTest);
							bRun=FALSE;
						}
					}
					else
					// This is a managed texture test case.
					// Call AddDirtyRect for the entire texture and perform an
					// initial rendering of the managed texture to ensure that its video memory
					// portion is properly initialized.  This also removes any dirty rectangles
					// created during the texture initialization (SetTexValue in CreateTex).
					{
						// For managed texture test cases, m_pSrcTex and m_pDestTex reference
						// the same texture.
						m_pDestTex->DirtyEntireTexture();
						m_pDestTex->Render(uiMipLevelIndex);
					}
				}

				if (bRun)
				{
					// Apply an operation to "dirty" the source texture (or the managed
					// texture, if the test case applies to a single managed texture).
					//
					// The chosen operation (AddDirtyRect, LockRect, etc.) is a property of
					// the test case.

					m_SkipInTheMiddle = FALSE;
					bRet = (this->*pTC->PreUpdateFunc)();


					if (!m_SkipInTheMiddle)
					{
						if (!bRet)
						{
							WriteToLog (TEXT("PreUpdateFunc() failed\n"));
							Fail();
						}
						else
						{
							try
							{
								//
								//	Update the texture
								//

								bool bUpdateTextureResult = false;
								if (bTestUpdateTexture)
								// If the test case involves a managed texture (as opposed to
								// system memory and video memory textures), skip the call to
								// UpdateTexture.
								{
									bUpdateTextureResult = UpdateTexture(m_pSrcTex->GetLPTEXTURES(), m_pDestTex->GetLPTEXTURES());

									if (bUpdateTextureResult == true && pTC->bExpectSucceed != TRUE)
									{
										WriteToLog(TEXT("UpdateTexture succeeded when expected to fail\n"));
										Fail();
									}

									if (bUpdateTextureResult == false && pTC->bExpectSucceed == TRUE)
									{
										WriteToLog(TEXT("UpdateTexture failed when expected to succeed\n"));
										Fail();
									}
								}

								if (bTestUpdateTexture != true)
								{
									if (FAILED((m_pSrcTex->Render(uiMipLevelIndex))))
									{
										SkipTestRange(nTest, nTest);
										bRun = false;
									}
								}
								else
								{
									if (bUpdateTextureResult == true && pTC->bExpectSucceed == TRUE)
									{
										// Render the default memory textures.  Let the framework
										// take care of presenting and comparing the back buffers.

										if (FAILED((m_pDestTex->Render(uiMipLevelIndex))))
										{
											SkipTestRange(nTest, nTest);
											bRun = false;
										}
									}
								}
							}
							catch (...)
							{
								WriteToLog (TEXT("Unhandled exception when calling UpdateTexture\n"));
								Fail();
							}
						}
					}
				}

				if (m_pSrcTex) {delete m_pSrcTex; m_pSrcTex = NULL;}
				if (bTestUpdateTexture && m_pDestTex) {delete m_pDestTex; m_pDestTex = NULL;}
				// If bTestUpdateTexture is 'false', m_pSrcTex and m_pDestTex point to the same texture.
				if (m_pBogusTex) {delete m_pBogusTex; m_pBogusTex = NULL;}
			}
		}
	} while (0);

	if (bRun)
		return true;

	return false;
}

bool UpdateTex_MipTextures::ExecuteTest(UINT nTest)
{
	UINT uiZeroBasedTCId;
	UINT uiMipLevelIndex;
	UINT uiDimensionAdjustmentIndex;
	UINT uiDestUsageIndex;
	UINT uiSourceUsageIndex;
	UINT iTestCase;

	uiZeroBasedTCId = nTest - 1;

	// Determine whether the test case must be applied to a pair of textures (sysmem and vidmem)
	// or to a single managed texture.
	if (uiZeroBasedTCId >= uiManagedTextureTestCases)
	{
		// Test UpdateTexture.
		bTestUpdateTexture = true;
		uiZeroBasedTCId -= uiManagedTextureTestCases;

		uiMipLevelIndex = uiZeroBasedTCId % GENERICTEXLEVEL;
		uiZeroBasedTCId /= GENERICTEXLEVEL;
		uiDimensionAdjustmentIndex = uiZeroBasedTCId % cuiNoOfDimensionAdjustments;
		uiZeroBasedTCId /= cuiNoOfDimensionAdjustments;
		uiDestUsageIndex = uiZeroBasedTCId % cuiNoOfDestUsages;
		uiZeroBasedTCId /= cuiNoOfDestUsages;
		uiSourceUsageIndex = uiZeroBasedTCId % cuiNoOfSourceUsages;
		uiZeroBasedTCId /= cuiNoOfSourceUsages;
		iTestCase = uiZeroBasedTCId;
	}
	else
	{
		// Test a managed texture.
		bTestUpdateTexture = false;

		uiMipLevelIndex = uiZeroBasedTCId % GENERICTEXLEVEL;
		uiZeroBasedTCId /= GENERICTEXLEVEL;
		uiDimensionAdjustmentIndex = uiZeroBasedTCId % cuiNoOfDimensionAdjustments;
		uiZeroBasedTCId /= cuiNoOfDimensionAdjustments;
		uiDestUsageIndex = 0;
		uiSourceUsageIndex = 0;

		iTestCase = uiZeroBasedTCId;
	}

	return UpdateTex::ExecuteTest
	(
		nTest,
		uiMipLevelIndex,
		uiDimensionAdjustmentIndex,
		uiDestUsageIndex,
		uiSourceUsageIndex,
		iTestCase
	);
}

bool UpdateTex_CubeTextures::ExecuteTest(UINT nTest)
{
	UINT uiZeroBasedTCId;
	UINT uiMipLevelIndex;
	UINT uiDimensionAdjustmentIndex;
	UINT uiDestUsageIndex;
	UINT uiSourceUsageIndex;
	UINT iTestCase;

	uiZeroBasedTCId = nTest - 1;

	// Determine whether the test case must be applied to a pair of textures (sysmem and vidmem)
	// or to a single managed texture.
	if (uiZeroBasedTCId >= uiManagedTextureTestCases)
	{
		// Test UpdateTexture.
		bTestUpdateTexture = true;
		uiZeroBasedTCId -= uiManagedTextureTestCases;

		uiMipLevelIndex = uiZeroBasedTCId % GENERICTEXLEVEL;
		uiZeroBasedTCId /= GENERICTEXLEVEL;
#if FAST_TEST
        uiCubeFaceIndex = (uiZeroBasedTCId % 2) * 3;// this will sample the 1st and 6th face
        uiZeroBasedTCId /= 2;
#else
        uiCubeFaceIndex = uiZeroBasedTCId % 6;
        uiZeroBasedTCId /= 6;
#endif
		uiDimensionAdjustmentIndex = uiZeroBasedTCId % cuiNoOfDimensionAdjustments;
		uiZeroBasedTCId /= cuiNoOfDimensionAdjustments;
		uiDestUsageIndex = uiZeroBasedTCId % cuiNoOfDestUsages;
		uiZeroBasedTCId /= cuiNoOfDestUsages;
		uiSourceUsageIndex = uiZeroBasedTCId % cuiNoOfSourceUsages;
		uiZeroBasedTCId /= cuiNoOfSourceUsages;
		iTestCase = uiZeroBasedTCId;
	}
	else
	{
		// Test a managed texture.
		bTestUpdateTexture = false;

		uiMipLevelIndex = uiZeroBasedTCId % GENERICTEXLEVEL;
		uiZeroBasedTCId /= GENERICTEXLEVEL;
		uiDimensionAdjustmentIndex = uiZeroBasedTCId % cuiNoOfDimensionAdjustments;
		uiZeroBasedTCId /= cuiNoOfDimensionAdjustments;
		uiDestUsageIndex = 0;
		uiSourceUsageIndex = 0;

		iTestCase = uiZeroBasedTCId;
	}

	return UpdateTex::ExecuteTest
	(
		nTest,
		uiMipLevelIndex,
		uiDimensionAdjustmentIndex,
		uiDestUsageIndex,
		uiSourceUsageIndex,
		iTestCase
	);
}

BOOL UpdateTex::CheckEntireTexOK()
{
	return TRUE;
}

BOOL UpdateTex::CheckEntireTexFail()
{
	return TRUE;
}

//
// This is used to check after less or equal to6 rectangles has been locked/AddDirtyXXX
//

BOOL UpdateTex::CheckSubTexOK()
{
	return TRUE;
}

//
// This is used to check after more than 6 rectangles has been locked/AddDirtyXXX
//
BOOL UpdateTex::CheckManySubTexOK()
{
	return TRUE;
}



BOOL UpdateTex::DoNothing()
{
	return TRUE;
}



//
// Case1 is to Lock with NULL, verify the entire surface has been updated
//
BOOL UpdateTex::Case1()
{
	BOOL				bRet;

	bRet=m_pSrcTex->LockWithNULL();

	if (!bRet)
	{
		WriteToLog (TEXT("Lock failed in Case1\n"));
	}
	return bRet;
}




//
// Case2 is to Lock with differnt sub rect, totally less than 6
//
BOOL UpdateTex::Case2()
{
	BOOL					bRet;

	bRet=m_pSrcTex->LockManyAreas(5);

	if (!bRet)
	{
		WriteToLog (TEXT("LockManyArea failed in Case2\n"));
	}
	return bRet;
}

//
// Case3 is to Lock with differnt sub rect, totally more than 6
//
BOOL UpdateTex::Case3()
{
	BOOL					bRet;

	bRet=m_pSrcTex->LockManyAreas(7);

	if (!bRet)
	{
		WriteToLog (TEXT("LockManyArea failed in Case3\n"));
	}
	return bRet;
}

//
// Case 4 is to Call AddDirtyXXX (NULL)
//

BOOL UpdateTex::Case4()
{
	BOOL					bRet;

	bRet=m_pSrcTex->AddDirtyWithNULL();

	if (!bRet)
	{
		WriteToLog (TEXT("AddDirtyXXX failed in Case4\n"));
	}
	return bRet;
}



//
// Case5 is to call AddDirtyXXX with differnt sub rect, totally <=6
//
BOOL UpdateTex::Case5()
{
	BOOL					bRet;

	bRet=m_pSrcTex->AddDirtyManyAreas(5);

	if (!bRet)
	{
		WriteToLog (TEXT("LockManyArea failed in Case5\n"));
	}
	return bRet;
}

//
// Case6 is to call AddDirtyXXX with differnt sub rect, totally more than 6
//
BOOL UpdateTex::Case6()
{
	BOOL					bRet;

	bRet=m_pSrcTex->AddDirtyManyAreas(7);

	if (!bRet)
	{
		WriteToLog (TEXT("AddDirtyManyArea failed in Case6\n"));
	}
	return bRet;
}


//
// Case7 is to Lock with D3DLOCK_NO_DIRTY_UPDATE, verify the entire surface has NOT been updated
//
BOOL UpdateTex::Case7()
{
	BOOL					bRet;

	bRet=m_pSrcTex->LockWithNULL(D3DLOCK_NO_DIRTY_UPDATE );

	if (!bRet)
	{
		WriteToLog (TEXT("Lock failed in Case7\n"));
	}
	return bRet;
}


//
// Case7_2 is to Lock with D3DLOCK_READONLY, verify the entire surface has NOT been updated
//
BOOL UpdateTex::Case7_2()
{
	BOOL					bRet;

	bRet=m_pSrcTex->LockWithNULL(D3DLOCK_READONLY );

	if (!bRet)
	{
		WriteToLog (TEXT("Lock failed in Case7\n"));
	}
	return bRet;
}


//
// Case8 is to call UpdateTexture immediately after another UpdateTexture
//
BOOL UpdateTex::Case8()
{
	BOOL bRet = TRUE;
	try
	{
		bool bUpdateTextureResult = UpdateTexture(m_pSrcTex->GetLPTEXTURES(), m_pBogusTex->GetLPTEXTURES());
		if (bUpdateTextureResult != true)
		{
			WriteToLog (TEXT("UpdateTexture failed with m_pBogusTex in Case8\n"));
			bRet = FALSE;
		}
	}
	catch (...)
	{
		WriteToLog(TEXT("Unhandled exception when calling UpdateTexture m_pBogusTex\n"));
		bRet = FALSE;
	}

	return bRet;
}


//
// Case9 is to change the sublevels without modifying the top level one
//
BOOL UpdateTex::Case9()
{
	return m_pSrcTex->LockSubLevel();
}


//
// Case10 is to Update from square to nonsquare case Texture
//
BOOL UpdateTex::Case10()
{
	if (m_pDestTex->GetResType()==D3DRTYPE_CUBETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}

	if (!m_bCanDoNonSquare)
	{
		WriteToLog (TEXT("device doesn't support non-square texture, skip test\n"));
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}
	if (!m_pDestTex->MakeNonSquare())
	{
		WriteToLog (TEXT("\nMakeNonSquare() failed in Case10()"));
	}
	//call Case4() to make sure the src tex is all dirty
	return Case4();
}


//
// Case11 is to Update from nonsquare to square case Texture
//
BOOL UpdateTex::Case11()
{
	if (m_pSrcTex->GetResType()==D3DRTYPE_CUBETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}

	if (!m_bCanDoNonSquare)
	{
		WriteToLog (TEXT("device doesn't support non-square texture, skip test\n"));
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}
	if (!m_pSrcTex->MakeNonSquare())
	{
		WriteToLog (TEXT("\nMakeNonSquare() failed in Case11()"));
	}
	//call Case4() to make sure the src tex is all dirty
	return Case4();
}


//
// Case12 is to Update from nonsquare to nonsquare case Texture
//
BOOL UpdateTex::Case12()
{
	BOOL bRet;
	if (m_pSrcTex->GetResType()==D3DRTYPE_CUBETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}

	if (!m_bCanDoNonSquare)
	{
		WriteToLog (TEXT("device doesn't support non-square texture, skip test\n"));
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}
	bRet= m_pSrcTex->MakeNonSquare();
	if (bRet) 
	{
		bRet=m_pDestTex->MakeNonSquare();
		m_pDestTex->SetSrcTex (m_pSrcTex);

		//call Case4() to make sure the src tex is all dirty
		bRet=Case4();
	}
	return bRet;
}

	
//
// Case13 is to call CopyRects (<=6),system mem
//
BOOL UpdateTex::Case13()
{
	BOOL					bRet;

	if (m_pDestTex->GetResType()==D3DRTYPE_VOLUMETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}

	bRet=m_pSrcTex->CopyRects(5,D3DPOOL_SYSTEMMEM);

	if (!bRet)
	{
		WriteToLog (TEXT("CopyRects failed in Case13\n"));
	}
	return bRet;
}

//
// Case13_2 is to call CopyRects (<=6),managed mem
//
BOOL UpdateTex::Case13_2()
{
	BOOL					bRet;

	if (m_pDestTex->GetResType()==D3DRTYPE_VOLUMETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}

	bRet=m_pSrcTex->CopyRects(5,D3DPOOL_MANAGED);

	if (!bRet)
	{
		WriteToLog (TEXT("CopyRects failed in Case13_2\n"));
	}
	return bRet;
}

//
// Case13_3 is to call CopyRects (<=6),video mem
//
BOOL UpdateTex::Case13_3()
{
	BOOL					bRet;

	if (m_pDestTex->GetResType()==D3DRTYPE_VOLUMETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}

	bRet=m_pSrcTex->CopyRects(5,D3DPOOL_DEFAULT);

	if (!bRet)
	{
		WriteToLog (TEXT("CopyRects failed in Case13\n"));
	}
	return bRet;
}




//
// Case14 is to call CopyRects (>6), system mem
//
BOOL UpdateTex::Case14()
{
	BOOL					bRet;

	if (m_pDestTex->GetResType()==D3DRTYPE_VOLUMETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}


	bRet=m_pSrcTex->CopyRects(7,D3DPOOL_SYSTEMMEM);

	if (!bRet)
	{
		WriteToLog (TEXT("CopyRects failed in Case14\n"));
	}
	return bRet;
}



//
// Case14_2 is to call CopyRects (>6), managed mem
//
BOOL UpdateTex::Case14_2()
{
	BOOL					bRet;

	if (m_pDestTex->GetResType()==D3DRTYPE_VOLUMETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}


	bRet=m_pSrcTex->CopyRects(7,D3DPOOL_MANAGED);

	if (!bRet)
	{
		WriteToLog (TEXT("CopyRects failed in Case14_2\n"));
	}
	return bRet;
}


//
// Case14_3 is to call CopyRects (>6), system mem
//
BOOL UpdateTex::Case14_3()
{
	BOOL					bRet;

	if (m_pDestTex->GetResType()==D3DRTYPE_VOLUMETEXTURE)
	{
		m_SkipInTheMiddle=TRUE;
		return TRUE;
	}


	bRet=m_pSrcTex->CopyRects(7,D3DPOOL_DEFAULT);

	if (!bRet)
	{
		WriteToLog (TEXT("CopyRects failed in Case14_3\n"));
	}
	return bRet;
}

// Override this to avoid incurring failures when a failing return code is actually expected.
void UpdateTex::ProcessError(HRESULT hr)
{
	if(FAILED(hr))
	{
		WriteStatus(_T("$wError"), _T("$r%s"), m_pD3D->HResultToString(hr));

		if (hr == 0x8876086e)
		{
			WriteToLog(_T("\nCommand batch failed, skipping remaining test cases.\n"));
			Abort();
			TestFinish(D3DTESTFINISH_SKIPREMAINDER);
		}
		else
		{
			WriteToLog(_T("GetLastError() returned %s(%x).\n"), m_pD3D->HResultToString(hr), hr);
			//Fail();
		}
	}
}

#if FAST_TEST
bool UpdateTex::AddFormatModes(DISPLAYMODE *pMode, bool bWindowed)
{
    UINT uTexture = 0;
    UINT uZBuffer = 0;
    UINT uCube = 0;
    UINT uVolume = 0;
    UINT uMS = 0;
    UINT uBB = 0;

    // Convert the formats into indexes into the common lists
    if (!GetFormatIndex(m_Options.ModeOptions.TextureFmt, m_pCommonTextureFormats, &uTexture) ||
        !GetZBufferFormatIndex(m_Options.ModeOptions.ZBufferFmt, m_pCommonZBufferFormats, &(pMode->Format), &uZBuffer) ||
        !GetFormatIndex(m_Options.ModeOptions.CubeFmt, m_pCommonCubeFormats, &uCube) ||
        !GetFormatIndex(m_Options.ModeOptions.VolumeFmt, m_pCommonVolumeFormats, &uVolume) ||
        !GetFormatIndex(m_Options.ModeOptions.MultisampType, m_pCommonMultiSampleFormats, m_uCommonMultiSampleFormats, &uMS) ||
        !GetBackBufferFormatIndex(m_Options.ModeOptions.BackBufferFmt, m_pCommonBackBufferFormats, &(pMode->Format), &uBB) ||
        // We need to validate MS here.  If the device doens't support this MS type, we don't want to add it as a mode
        (m_pCommonMultiSampleFormats && m_pCommonZBufferFormats &&
        (FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)) ||
        FAILED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[uZBuffer], bWindowed, m_pCommonMultiSampleFormats[uMS], NULL)))))
    {
        DPF(2, _T("Requested fmt not supported by device.\n"));
        return false;
    }

    // If only one mode was requested, add it and return
    if (m_Options.ModeOptions.TextureFmt != FMT_ALL &&
        m_Options.ModeOptions.ZBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.CubeFmt != FMT_ALL &&
        m_Options.ModeOptions.VolumeFmt != FMT_ALL &&
        m_Options.ModeOptions.BackBufferFmt != FMT_ALL &&
        m_Options.ModeOptions.MultisampType != MULTISAMPLE_ALL)
    {
        return AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB);
    }

    if (m_Options.ModeOptions.BackBufferFmt == FMT_ALL)
    {
        for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
        {
            // Filter out unsupported BB modes based on ModeOptions
            if (m_pD3D->GetVersion() < 8.f)
            {
                if (pMode->Format.d3dfFormat != m_pCommonBackBufferFormats[nBB].d3dfFormat)
                    continue;
            }
            else if (FAILED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format,
                m_pCommonBackBufferFormats[nBB], bWindowed)))
                continue;

            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, uVolume, uMS, 0, nBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.TextureFmt == FMT_ALL)
    {
        for (UINT nTexture = 0; nTexture < m_uCommonTextureFormats; nTexture++)
        {
            if (m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_A8R8G8B8 ||
                m_pCommonTextureFormats[nTexture].d3dfFormat == FMT_DXT1)
            {
                if (!AddMode(pMode, bWindowed, nTexture, uZBuffer, uCube, uVolume, uMS, 0, uBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    //static const FMT formats[] = { FMT_A8R8G8B8, FMT_DXT1 };
    //
    //for (FMT fmt : formats)
    //{
    //    if (!AddMode(pMode, bWindowed, fmt, uZBuffer, uCube, uVolume, uMS, 0, uBB))
    //    {
    //        DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
    //        return false;
    //    }
    //}

    if (m_Options.ModeOptions.ZBufferFmt == FMT_ALL)
    {
        for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, nZBuffer, uCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.CubeFmt == FMT_ALL)
    {
        for (UINT nCube = 0; nCube < m_uCommonCubeFormats; nCube++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, nCube, uVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
    {
        for (UINT nVolume = 0; nVolume < m_uCommonVolumeFormats; nVolume++)
        {
            if (!AddMode(pMode, bWindowed, uTexture, uZBuffer, uCube, nVolume, uMS, 0, uBB))
            {
                DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                return false;
            }
        }
    }

    if (m_Options.ModeOptions.MultisampType == FMT_ALL)
    {
        DWORD dwMSQuality = 0;

        for (UINT nMS = 0; nMS < m_uCommonMultiSampleFormats; nMS++)
        {
            int nValidBB = -1, nValidZBuffer = -1;

            // For each MS type, find a BB and ZBuffer that support that type
            for (UINT nBB = 0; nBB < m_uCommonBackBufferFormats; nBB++)
            for (UINT nZBuffer = 0; nZBuffer < m_uCommonZBufferFormats; nZBuffer++)
            {
                // If specific modes are requested, only allow those modes to be matched.
                if (m_Options.ModeOptions.BackBufferFmt != FMT_ALL && m_Options.ModeOptions.BackBufferFmt != FMT_ANY &&
                    nBB != uBB)
                    continue;
                if (m_Options.ModeOptions.ZBufferFmt != FMT_ALL && m_Options.ModeOptions.ZBufferFmt != FMT_ANY &&
                    nZBuffer != uZBuffer)
                    continue;

                // Filter out unsupported BB modes based on ModeOptions
                if (SUCCEEDED(m_pD3D->CheckDeviceType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), pMode->Format, m_pCommonBackBufferFormats[nBB], bWindowed)) &&
                    SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonBackBufferFormats[nBB], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)) &&
                    SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(m_DeviceList[0]->GetAdapterID(), m_DeviceList[0]->GetDevType(), m_pCommonZBufferFormats[nZBuffer], bWindowed, m_pCommonMultiSampleFormats[nMS], &dwMSQuality)))
                {
                    nValidBB = nBB;
                    nValidZBuffer = nZBuffer;
                    break;
                }
            }

            if (nValidBB == -1 || nValidZBuffer == -1)
                continue;

            for (UINT nMSQ = 0; nMSQ < dwMSQuality; nMSQ++)
            {
                if (!AddMode(pMode, bWindowed, uTexture, nValidZBuffer, uCube, uVolume, nMS, nMSQ, nValidBB))
                {
                    DPF(1, _T("CD3DTest::AddModes:  AddTestMode failed.\n"));
                    return false;
                }
            }
        }
    }

    return true;
}
#endif //FAST_TEST