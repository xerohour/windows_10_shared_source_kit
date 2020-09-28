#include "CD3DWindowFramework.h"
#include "UpdateSurfaceTestMain.h"

#include <stdio.h>
#include <d3dx9.h>

#include "resource.h"

#include "dxfmt.h"
#include "dxpool.h"
#include "dxusage.h"
#include "dxmultisample.h"
//#include "dxfilter.h"
#include "dims.h"
#include "rects.h"

#include "auxiliary.h"

extern CD3DWindowFramework TestApp;

const float CUpdateSurfaceTestMain::fMinimumConformanceFraction = 0.50f;

CUpdateSurfaceTestMain::BASICTEXTUREVERTEX CUpdateSurfaceTestMain::QuadVertices[] =
{
	{-1.0f, -1.0f, 0.1f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f}
};
/*
{
	{-1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f}
};
*/
CONST DWORD CUpdateSurfaceTestMain::dwD3DFVF_VERTEX = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);


//****************************************************************************
//* IsDXTn
//****************************************************************************
bool IsDXTn(D3DFORMAT d3dfmt)
{
    if ((d3dfmt == D3DFMT_DXT1) ||
        (d3dfmt == D3DFMT_DXT2) ||
        (d3dfmt == D3DFMT_DXT3) ||
        (d3dfmt == D3DFMT_DXT4) ||
        (d3dfmt == D3DFMT_DXT5))
        return true;
    else
        return false;
}

////////////////////////////////////////////////////////////////////////////////
// clsSurfaceSpecList methods

clsSurfaceSpecList::~clsSurfaceSpecList()
{
	SurfaceSpecListElement *pCurrentElement, *pNextElement;

	if (pCurrentElement = pFirst)
		do
		{
			pNextElement = pCurrentElement->pNext;
			delete pCurrentElement;
		}
		while (pCurrentElement = pNextElement);
}

void clsSurfaceSpecList::AddSurfaceSpec(DWORD dwSurfaceSpec)
{
	SurfaceSpecListElement *pNewElement = new SurfaceSpecListElement;

	pNewElement->dwSurfaceSpec = dwSurfaceSpec;
	pNewElement->pNext = NULL;

	if (pFirst)
		pLast = (pLast->pNext = pNewElement);
	else
		pLast = (pFirst = pNewElement);

	++uiNoOfElements;
}

void clsSurfaceSpecList::InitializeSurfaceSpecArray(clsSurfaceSpecArray *pTargetArray)
{
	if (pTargetArray->pdwSurfaceSpecs)
	{
		delete [] (pTargetArray->pdwSurfaceSpecs);
		pTargetArray->pdwSurfaceSpecs = NULL;
	}
	pTargetArray->uiNoOfElements = 0;

	if (uiNoOfElements)
	{
		DWORD *dwAllocatedMemory = NULL;
		UINT uiIndex = 0;
		SurfaceSpecListElement *pCurrentElement = pFirst;

		dwAllocatedMemory = new DWORD[uiNoOfElements];

		do
		{
			dwAllocatedMemory[uiIndex++] = pCurrentElement->dwSurfaceSpec;
		}
		while (pCurrentElement = pCurrentElement->pNext);

		pTargetArray->pdwSurfaceSpecs = dwAllocatedMemory;
		pTargetArray->uiNoOfElements = uiNoOfElements;
	}
}

////////////////////////////////////////////////////////////////////////////////
// clsSurfaceSpecArray methods

clsSurfaceSpecArray::~clsSurfaceSpecArray()
{
	if (pdwSurfaceSpecs)
		delete [] pdwSurfaceSpecs;
}

DWORD clsSurfaceSpecArray::GetElementValue(unsigned int uiIndex)
{
	if (uiIndex < uiNoOfElements)
		return pdwSurfaceSpecs[uiIndex];
	else
		return 0;
}

////////////////////////////////////////////////////////////////////////////////
// clsFormatPairList methods

FormatPair* clsFormatPairList::AddNewElement()
{
	FormatPairListElement *pNewElement;

	if (!(pNewElement = new FormatPairListElement))
		return NULL;

	ZeroMemory((PVOID)pNewElement, (SIZE_T)(sizeof(FormatPairListElement)));

	if (pFirst)
		pLast = (pLast->pNext = pNewElement);
	else
		pLast = (pFirst = pNewElement);

	return &(pNewElement->FormatPairData);
}

void clsFormatPairList::DeleteAll()
{
	FormatPairListElement *pCurrentElement, *pNextElement;

	if (pCurrentElement = pFirst)
	{
		do
		{
			pNextElement = pCurrentElement->pNext;
			delete pCurrentElement;
		}
		while (pCurrentElement = pNextElement);
		pFirst = (pLast = NULL);
	}
}

////////////////////////////////////////////////////////////////////////////////
// clsSourceSurfaceSet methods

void clsSourceSurfaceSet::ReleaseAllSurfaces()
{
	//texsurfSourceLoader.ReleaseDXObjects();

	if (pSourceTestSurface)
	{
		delete pSourceTestSurface;
		pSourceTestSurface = NULL;
	}

	bInitialized = false;
	dwMainSurfaceSpec = 0;
}

bool clsSourceSurfaceSet::PrepareFromDWORD(CUpdateSurfaceTestMain *pTestEnv, DWORD dwSurfaceSpec)
{
	if (bInitialized)
		if (dwMainSurfaceSpec == dwSurfaceSpec)
			return true;
		else
			ReleaseAllSurfaces();

	if (!(pSourceTestSurface = pTestEnv->CreateSurfaceFromDWORD(dwSurfaceSpec)))
		pTestEnv->WriteToLog("Unable to create source surface.\n");
	else
	{
		CSurface *lpd3dsurfSource = NULL;

		pSourceTestSurface->Describe(szDescription);

		// Retrieve a pointer to the DX surface interface
		// for the source surface.
		lpd3dsurfSource = pSourceTestSurface->GetSurfacePointer();

		// Initialize a SURFACEDESC structure, based on lpd3dsurfSource, to allow convenient
		// programmatic access to the surface properties.
		lpd3dsurfSource->GetDesc(&sdSourceSurfaceDesc);

		// Initialize the surface.

		/*
		if (!pTestEnv->InitializeCompatibleSMSurface(sdSourceSurfaceDesc, &texsurfSourceLoader))
			pTestEnv->WriteToLog("Unable to create a system memory surface compatible with the source surface.");
		else
		*/
		{
			//CSurface *lpd3dsurfSourceLoader;

			HRESULT hresult;

			// Determine whether the surface format is a bump/luminance map format.

			if
			(
				SUCCEEDED
				(
					(
						hresult =
						(pTestEnv->m_pD3D)->CheckDeviceFormat
						(
							pTestEnv->uiSrcDeviceAdapterOrdinal,
							pTestEnv->dwSrcDeviceType,
							(pTestEnv->displaymodeAdapter).Format,
							USAGE_QUERY_LEGACYBUMPMAP,
							RTYPE_TEXTURE,
							sdSourceSurfaceDesc.Format
						)
					)
				)
			)
			{
				LOCKED_RECT lr;

				if (SUCCEEDED((hresult = lpd3dsurfSource->LockRect(&lr, NULL, 0))))
				{
					UINT uiY;
					BYTE *pRowStart = (BYTE *)(lr.pBits);
					UINT uiBytesPerRow = (UINT)(((DWORD)(sdSourceSurfaceDesc.Width * ((sdSourceSurfaceDesc.Format).ddpfPixelFormat).dwBumpBitCount)) >> 3);
					BYTE byteSetVal = 0;

                    UINT Height = IsDXTn(sdSourceSurfaceDesc.Format.d3dfFormat)?sdSourceSurfaceDesc.Height/4:sdSourceSurfaceDesc.Height;

					for (uiY = 0; uiY < Height; ++uiY)
					{
						UINT uiOffsetWithinRow;

						for (uiOffsetWithinRow = 0; uiOffsetWithinRow < uiBytesPerRow; ++uiOffsetWithinRow)
						{
							pRowStart[uiOffsetWithinRow] = byteSetVal++;
						}

						pRowStart += lr.iPitch;
					}

					lpd3dsurfSource->UnlockRect();
				}
			}
			else
			{
				D3DXIMAGE_INFO d3dxii;

				// Retrieve a pointer to the "loader surface".
				// lpd3dsurfSourceLoader = texsurfSourceLoader.GetSurfacePointer();

				// Load the "foreground" image onto the surface.

				hresult =
				(pTestEnv->m_pSrcDevice) ->
				D3DXLoadSurfaceFromFileInMemory
				(
					lpd3dsurfSource,
					NULL,
					NULL,
					pTestEnv->BitmapDataForeground,
					((PBITMAPFILEHEADER)(pTestEnv->BitmapDataForeground))->bfSize,
					NULL,
					D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
					0,
					&d3dxii
				);
			}

			if (FAILED(hresult))
				pTestEnv->WriteToLog("Unable to load the source surface with the 'foreground image'.\n");
			else
			{
				bInitialized = true;
				dwMainSurfaceSpec = dwSurfaceSpec;
				return true;
			}
		}

		delete pSourceTestSurface;
		pSourceTestSurface = NULL;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// clsDestSurfaceSet methods

void clsDestSurfaceSet::ReleaseAllSurfaces()
{
	texsurfDestLoader.ReleaseDXObjects();
	texsurfReference.ReleaseDXObjects();
	texsurfDestCopy.ReleaseDXObjects();

	if (pDestTestSurface)
	{
		delete pDestTestSurface;
		pDestTestSurface = NULL;
	}

	bInitialized = false;
	dwMainSurfaceSpec = 0;
}

bool clsDestSurfaceSet::PrepareFromDWORD(CUpdateSurfaceTestMain *pTestEnv, DWORD dwSurfaceSpec)
{
	if (bInitialized)
		if (dwMainSurfaceSpec == dwSurfaceSpec)
			return true;
		else
			ReleaseAllSurfaces();

	if (!(pDestTestSurface = pTestEnv->CreateSurfaceFromDWORD(dwSurfaceSpec)))
		pTestEnv->WriteToLog("Unable to create destination surface.\n");
	else
	{
		CSurface *lpd3dsurfDest;

		pDestTestSurface->Describe(szDescription);

		// Retrieve a pointer to the DX surface interface
		// for the destination surface.
		lpd3dsurfDest = pDestTestSurface->GetSurfacePointer();

		// Initialize a SURFACEDESC structure, based on lpd3dsurfDest, to allow convenient
		// programmatic access to the surface properties.
		lpd3dsurfDest->GetDesc(&sdDestSurfaceDesc);

		// Compute the number of pixels on the destination surface.  The value is used,
		// along with the number of pixels in the destination rectangle, to determine
		// expected conformance for the entire destination surface.
		uiNoOfSurfacePixels = sdDestSurfaceDesc.Width * sdDestSurfaceDesc.Height;

		// Create three surfaces compatible with the destination surface:
		//
		// texsurfDestLoader -	used to load the destination surface
		//						with a "background image" (system memory);
		// texsurfReference -	the destination surface for the
		//						reference copy (managed memory, to facilitate rendering for
		//						comparison purposes);
		// texsurfDestCopy -	used to obtain a lockable copy of
		//						the destination which can be compared
		//						with the reference copy (system memory).
		//						NOTE: Copying the test image back into system memory is
		//						no longer the preferred verification technique. (11/12/2002)

		if
		(
			!pTestEnv->InitializeCompatibleSMSurface(sdDestSurfaceDesc, &texsurfDestLoader) ||
			!pTestEnv->InitializeCompatibleMMSurface(sdDestSurfaceDesc, &texsurfReference) ||
			!pTestEnv->InitializeCompatibleSMSurface(sdDestSurfaceDesc, &texsurfDestCopy)
		)
			pTestEnv->WriteToLog("Unable to create all of the destination-compatible auxiliary surfaces.\n");
		else
		{
			CSurface *lpd3dsurfDestLoader;
			HRESULT hresult;

			// Retrieve a pointer to the "loader surface".
			lpd3dsurfDestLoader = texsurfDestLoader.GetSurfacePointer();

			// Load the "background" image onto the loader surface.

			if
			(
				SUCCEEDED
				(
					(
						hresult =
						(pTestEnv->m_pD3D)->CheckDeviceFormat
						(
							pTestEnv->uiSrcDeviceAdapterOrdinal,
							pTestEnv->dwSrcDeviceType,
							(pTestEnv->displaymodeAdapter).Format,
							USAGE_QUERY_LEGACYBUMPMAP,
							RTYPE_TEXTURE,
							sdDestSurfaceDesc.Format
						)
					)
				)
			)
			{
				LOCKED_RECT lr;

				if (SUCCEEDED((hresult = lpd3dsurfDestLoader->LockRect(&lr, NULL, 0))))
				{
					UINT uiY;
					BYTE *pRowStart = (BYTE *)(lr.pBits);
					UINT uiBytesPerRow = (UINT)(((DWORD)(sdDestSurfaceDesc.Width * ((sdDestSurfaceDesc.Format).ddpfPixelFormat).dwBumpBitCount)) >> 3);

					BYTE byteSetVal = 0;
                    UINT Height = IsDXTn(sdDestSurfaceDesc.Format.d3dfFormat)?sdDestSurfaceDesc.Height/4:sdDestSurfaceDesc.Height;

					for (uiY = 0; uiY < Height; ++uiY)
					{
						UINT uiOffsetWithinRow;

						for (uiOffsetWithinRow = 0; uiOffsetWithinRow < uiBytesPerRow; ++uiOffsetWithinRow)
						{
							pRowStart[uiOffsetWithinRow] = 0;
						}

						pRowStart += lr.iPitch;
					}

					lpd3dsurfDestLoader->UnlockRect();
				}
			}
			else
			{
				D3DXIMAGE_INFO d3dxii;

				hresult =
				(pTestEnv->m_pSrcDevice) ->
				D3DXLoadSurfaceFromFileInMemory
				(
					lpd3dsurfDestLoader,
					NULL,
					NULL,
					pTestEnv->BitmapDataBackground,
					((PBITMAPFILEHEADER)(pTestEnv->BitmapDataBackground))->bfSize,
					NULL,
					D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
					0,
					&d3dxii
				);
			}

			if (FAILED(hresult))
				pTestEnv->WriteToLog("Unable to load the destination-compatible 'loader' surface with the 'background image'.\n");
			else
			{
				bInitialized = true;
				dwMainSurfaceSpec = dwSurfaceSpec;
				return true;
			}
		}

		texsurfDestLoader.ReleaseDXObjects();
		texsurfReference.ReleaseDXObjects();
		texsurfDestCopy.ReleaseDXObjects();

		delete pDestTestSurface;
		pDestTestSurface = NULL;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// CUpdateSurfaceTestMain methods

CUpdateSurfaceTestMain::CUpdateSurfaceTestMain()
{
	SetTestType(TESTTYPE_CONF);

	m_szCommandKey= _T("UpdateSurfaceTest");
	m_szLogName = _T("UpdateSurfaceTest");
	m_szTestName= _T("UpdateSurfaceTest");

	m_Options.D3DOptions.fMinDXVersion = 9.f;
	//m_Options.D3DOptions.fMaxDXVersion = 9.f;
	m_Options.TestOptions.bCompareSrcRef = false;
	m_Options.D3DOptions.bZBuffer		 = false;

	//m_Options.TestOptions.fPassRequired = 0.97f;

	BitmapDataForeground = (BitmapDataBackground = NULL);
	ValidSurfacesByFormat = NULL;
	//ValidFilterRecordIndices = NULL;

	pvbAuxiliaryVertexBuffer = NULL;
	psurfAuxiliaryRenderTargetTest = NULL;
	psurfAuxiliaryRenderTargetRef = NULL;
	ptexAuxiliaryTexture = NULL;
}

CUpdateSurfaceTestMain::~CUpdateSurfaceTestMain()
{
	if (ValidSurfacesByFormat)
		delete [] ValidSurfacesByFormat;
	/*
	if (ValidFilterRecordIndices)
		delete [] ValidFilterRecordIndices;
	*/
}

bool CUpdateSurfaceTestMain::InitializeAuxiliaryResources()
{
	HRESULT hresult;

	FORMAT fmtRT(FMT_X8R8G8B8);

	// Create and initialize the "auxiliary texture".  This is assigned to the second
	// texture stage when rendering a primitive in order to verify a bump/luminance map.

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				m_pSrcDevice->CreateTexture
				(
					32,
					32,
					1,
					0,
					fmtRT,
					POOL_MANAGED,
					&ptexAuxiliaryTexture,
					NULL
				)
			)
		)
	)
	{
		CSurface *psurfTex = NULL;

		if (SUCCEEDED((hresult = ptexAuxiliaryTexture->GetSurfaceLevel(0, &psurfTex))))
		{
			D3DXIMAGE_INFO d3dxii;

			hresult =
			m_pSrcDevice->D3DXLoadSurfaceFromFileInMemory
			(
				psurfTex,
				NULL,
				NULL,
				BitmapDataForeground,
				((PBITMAPFILEHEADER)BitmapDataForeground)->bfSize,
				NULL,
				D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
				0,
				&d3dxii
			);

			psurfTex->Release();
		}
	}

	// AndrewLu: Create a "global", lockable render target for rendering test surfaces.
	if (FAILED((hresult = m_pSrcDevice->CreateRenderTarget(cuiGlobalRTWidth, cuiGlobalRTHeight, /*sdSrcBackBuffer.Format*/fmtRT, MULTISAMPLE_NONE, TRUE, &psurfAuxiliaryRenderTargetTest))))
		WriteToLog(_T("CreateRenderTarget failed in CUpdateSurfaceTestMain::InitializeAuxiliaryResources.\n"));
	else
	{
		// AndrewLu: Create a "global", lockable render target for rendering reference surfaces.
		if (FAILED((hresult = m_pSrcDevice->CreateRenderTarget(cuiGlobalRTWidth, cuiGlobalRTHeight, /*sdSrcBackBuffer.Format*/fmtRT, MULTISAMPLE_NONE, TRUE, &psurfAuxiliaryRenderTargetRef))))
			WriteToLog(_T("CreateRenderTarget failed in CUpdateSurfaceTestMain::InitializeAuxiliaryResources.\n"));
		else
		{
			// AndrewLu: Create and initialize a vertex buffer for the texture rendering operation.
			if
			(
				FAILED
				(
					(
						hresult =
						m_pSrcDevice->CreateVertexBuffer
						(
							sizeof(QuadVertices),
							0,
							dwD3DFVF_VERTEX,
							POOL_SYSTEMMEM,
							&pvbAuxiliaryVertexBuffer
						)
					)
				)
			)
				WriteToLog(_T("CreateVertexBuffer failed in UpdateTex::InitializeAuxiliaryResources.\n"));
			else
			{
				void *pVertexBufferData;

				if
				(
					FAILED
					(
						(
							hresult =
							pvbAuxiliaryVertexBuffer->Lock
							(
								0,
								sizeof(QuadVertices),
								&pVertexBufferData,
								0
							)
						)
					)
				)
					WriteToLog(_T("Unable to lock vertex buffer in UpdateTex::InitializeAuxiliaryResources.\n"));
				else
				{
					memcpy((void *)pVertexBufferData, (void *)QuadVertices,
					sizeof(QuadVertices));
					pvbAuxiliaryVertexBuffer->Unlock();

					return true;
				}

				pvbAuxiliaryVertexBuffer->Release();
				pvbAuxiliaryVertexBuffer = NULL;
			}

			psurfAuxiliaryRenderTargetRef->Release();
			psurfAuxiliaryRenderTargetRef = NULL;
		}

		psurfAuxiliaryRenderTargetTest->Release();
		psurfAuxiliaryRenderTargetTest = NULL;
	}

	return false;
}

void CUpdateSurfaceTestMain::ClearAuxiliaryResources()
{
	if (pvbAuxiliaryVertexBuffer)
	{
		pvbAuxiliaryVertexBuffer->Release();
		pvbAuxiliaryVertexBuffer = NULL;
	}

	if (psurfAuxiliaryRenderTargetRef)
	{
		psurfAuxiliaryRenderTargetRef->Release();
		psurfAuxiliaryRenderTargetRef = NULL;
	}

	if (psurfAuxiliaryRenderTargetTest)
	{
		psurfAuxiliaryRenderTargetTest->Release();
		psurfAuxiliaryRenderTargetTest = NULL;
	}

	if (ptexAuxiliaryTexture)
	{
		ptexAuxiliaryTexture->Release();
		ptexAuxiliaryTexture = NULL;
	}
}

void CUpdateSurfaceTestMain::FindValidTextureSpecs(UINT uiFormatIndex, DWORD *dwCandidateUsages, UINT uiNoOfCandidateUsages, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList)
{
	FORMAT FrameworkFormat(DXGraphicsFormats[uiFormatIndex].fmtFW);

	UINT uiUsageSetIndex;

	for (uiUsageSetIndex = 0; uiUsageSetIndex < uiNoOfCandidateUsages; ++uiUsageSetIndex)
	{
		HRESULT hresult;

		if
		(
			SUCCEEDED
			(
				(
					hresult =
					m_pD3D->CheckDeviceFormat
					(
						uiSrcDeviceAdapterOrdinal,
						dwSrcDeviceType,
						displaymodeAdapter.Format,
						dwCandidateUsages[uiUsageSetIndex],
						RTYPE_TEXTURE,
						FrameworkFormat
					)
				)
			)
		)
		{
			DWORD dwSurfaceSpec;

			dwSurfaceSpec =
			((DWORD)2 << 28) |	// Surface type == 2; denotes texture surface.
			((DWORD)uiFormatIndex << 16) |
			(DWORD)uiUsageSetIndex |
			((DWORD)uiPoolIndex << 8);	// Bits 8 to 15 of the surface spec contain the pool type index.

			pSpecList->AddSurfaceSpec(dwSurfaceSpec);
			//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
			//pSpecList->AddSurfaceSpec(dwSurfaceSpec);
		}
	}
}

void CUpdateSurfaceTestMain::FindValidCubeTextureSpecs(UINT uiFormatIndex, DWORD *dwCandidateUsages, UINT uiNoOfCandidateUsages, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList)
{
	FORMAT FrameworkFormat(DXGraphicsFormats[uiFormatIndex].fmtFW);

	UINT uiUsageSetIndex;

	for (uiUsageSetIndex = 0; uiUsageSetIndex < uiNoOfCandidateUsages; ++uiUsageSetIndex)
	{
		HRESULT hresult;

		if
		(
			SUCCEEDED
			(
				(
					hresult =
					m_pD3D->CheckDeviceFormat
					(
						uiSrcDeviceAdapterOrdinal,
						dwSrcDeviceType,
						displaymodeAdapter.Format,
						dwCandidateUsages[uiUsageSetIndex],
						RTYPE_CUBETEXTURE,
						FrameworkFormat
					)
				)
			)
		)
		{
			DWORD dwSurfaceSpec;

			dwSurfaceSpec =
			((DWORD)3 << 28) |	// Surface type == 3; denotes cube texture surface.
			((DWORD)uiFormatIndex << 16) |
			(DWORD)uiUsageSetIndex |
			((DWORD)uiPoolIndex << 8);	// Bits 8 to 15 of the surface spec contain the pool type index.

			pSpecList->AddSurfaceSpec(dwSurfaceSpec);
			//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
			//pSpecList->AddSurfaceSpec(dwSurfaceSpec);
		}
	}
}

void CUpdateSurfaceTestMain::FindValidOffscreenPlainSpecs(UINT uiFormatIndex, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(DXGraphicsFormats[uiFormatIndex].fmtFW);

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				m_pD3D->CheckDeviceFormat
				(
					uiSrcDeviceAdapterOrdinal,
					dwSrcDeviceType,
					displaymodeAdapter.Format,
					0,
					RTYPE_SURFACE,
					FrameworkFormat
				)
			)
		)
	)
	{
		DWORD dwSurfaceSpec;

		dwSurfaceSpec =
		((DWORD)4 << 28) |	// Surface type == 4; denotes offscreen plain surface.
							// Bits 28--31 of the surface spec contain the surface type.
		((DWORD)uiFormatIndex << 16) |	// Bits 16--23 of the surface spec contain the format index.
		(DWORD)uiPoolIndex << 8;	// Bits 8 to 15 of the surface spec contain a pool type index.

		pSpecList->AddSurfaceSpec(dwSurfaceSpec);
		//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
		//pSpecList->AddSurfaceSpec(dwSurfaceSpec);
	}
}

void CUpdateSurfaceTestMain::FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(DXGraphicsFormats[uiFormatIndex].fmtFW);

	// Determine whether the format is a valid render target format.
	if
	(
		SUCCEEDED
		(
			(
				hresult =
				m_pD3D->CheckDeviceFormat
				(
					uiSrcDeviceAdapterOrdinal,
					dwSrcDeviceType,
					displaymodeAdapter.Format,
					USAGE_RENDERTARGET,
					RTYPE_SURFACE,
					FrameworkFormat
				)
			)
		)
	)
	{
		UINT uiMultiSampleTypeIndex;

		// For each testable multisampling type, add four surface types to the list.
		for (uiMultiSampleTypeIndex = 0; uiMultiSampleTypeIndex < cuiNoOfMultiSampleTypes; ++uiMultiSampleTypeIndex)
		{
			if
			(
				SUCCEEDED
				(
					(
						hresult =
						m_pD3D->CheckDeviceMultiSampleType
						(
							uiSrcDeviceAdapterOrdinal,
							dwSrcDeviceType,
							FrameworkFormat,
							TRUE,
							(MULTISAMPLE_TYPE)((DXMultiSampleTypes[uiMultiSampleTypeIndex]).mstFW)
						)
					)
				)
			)
			{
				DWORD dwSurfaceSpec;
				
				dwSurfaceSpec =
				((DWORD)1 << 28) | // Surface type == 1; denotes render target.
				((DWORD)uiFormatIndex << 16) |
				(((DWORD)uiMultiSampleTypeIndex) << 8);

				pSpecList->AddSurfaceSpec(dwSurfaceSpec);
				if ((MULTISAMPLE_TYPE)((DXMultiSampleTypes[uiMultiSampleTypeIndex]).mstFW) == MULTISAMPLE_NONE)
					pSpecList->AddSurfaceSpec(dwSurfaceSpec | 1);
				//pSpecList->AddSurfaceSpec(dwSurfaceSpec | 4);
				//pSpecList->AddSurfaceSpec(dwSurfaceSpec | 5);
				// The four surface type variants account for two different surface types
				// and lockable vs non-lockable render targets.
				// Bit index 2?  "Legacy code"? ???
				// See CopyRects test!
			}
		}
	}
}

/*
void CUpdateSurfaceTestMain::FindValidTextureSpecs(clsSurfaceSpecList *pSpecList)
{
	UINT uiFormatIndex;

	for (uiFormatIndex = 0; uiFormatIndex < cuiNoOfFormats; ++uiFormatIndex)
	{
		FORMAT FrameworkFormat(DXGraphicsFormats[uiFormatIndex].fmtFW);

		UINT uiUsageSetIndex;

		for (uiUsageSetIndex = 0; uiUsageSetIndex < cuiNoOfUsageSets; ++uiUsageSetIndex)
		{
			HRESULT hresult;

			if
			(
				SUCCEEDED
				(
					(
						hresult =
						m_pD3D->CheckDeviceFormat
						(
							uiSrcDeviceAdapterOrdinal,
							dwSrcDeviceType,
							displaymodeAdapter.Format,
							dwUsageSets[uiUsageSetIndex],
							RTYPE_TEXTURE,
							FrameworkFormat
						)
					)
				)
			)
			{
				UINT uiPoolTypeIndex;

				for (uiPoolTypeIndex = 0; uiPoolTypeIndex < cuiNoOfPoolTypes; ++uiPoolTypeIndex)
				{
					if ((dwUsageSets[uiUsageSetIndex] != USAGE_RENDERTARGET) ||
						(DXPoolTypes[uiPoolTypeIndex].dwFWPool == POOL_DEFAULT))
					{
						DWORD dwSurfaceSpec;

						dwSurfaceSpec =
						((DWORD)2 << 28) | // Surface type == 2; denotes texture surface.
						((DWORD)uiFormatIndex << 16) |
						((DWORD)uiPoolTypeIndex << 8) |
						(DWORD)uiUsageSetIndex;

						pSpecList->AddSurfaceSpec(dwSurfaceSpec);
						dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
						pSpecList->AddSurfaceSpec(dwSurfaceSpec);
					}
				}
			}
		}
	}
}
*/

/*
void CUpdateSurfaceTestMain::FindValidSourceSurfaceSpecs() {}
void CUpdateSurfaceTestMain::FindValidDestSurfaceSpecs() {}
*/

/*
void CUpdateSurfaceTestMain::FindValidSurfaceSpecs()
{
	clsSurfaceSpecList SurfaceSpecList;

	// Based on the format of the source device adapter's current mode and
	// the capabilities of the device, populate the surface specification list.
	FindValidTextureSpecs(&SurfaceSpecList);

	// Use the list to initialize the surface specification array.
	// SurfaceSpecList.InitializeSurfaceSpecArray(&SurfaceSpecArray);
}
*/

void CUpdateSurfaceTestMain::FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray)
{
	clsSurfaceSpecList SourceSpecList;

	// Build the list.

	FindValidOffscreenPlainSpecs(uiFormatIndex, 1, &SourceSpecList);
	// The second argument, temporarily "hard-coded" as 1, is the pool index for D3DPOOL_SYSTEMMEM.

	FindValidTextureSpecs(uiFormatIndex, dwSourceUsageSets, cuiNoOfSourceUsageSets, 1, &SourceSpecList);
	FindValidCubeTextureSpecs(uiFormatIndex, dwSourceUsageSets, cuiNoOfSourceUsageSets, 1, &SourceSpecList);
	// The fourth argument, temporarily "hard-coded" as 1, is the pool index for D3DPOOL_SYSTEMMEM.

	//FindValidRTSpecs(uiFormatIndex, &SourceSpecList);

	// Initialize the array.
	SourceSpecList.InitializeSurfaceSpecArray(pSourceSpecArray);
}

void CUpdateSurfaceTestMain::FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray)
{
	clsSurfaceSpecList DestSpecList;

	// Build the list.

	FindValidOffscreenPlainSpecs(uiFormatIndex, 0, &DestSpecList);
	// The second argument, temporarily "hard-coded" as 0, is the pool index for D3DPOOL_DEFAULT.

	FindValidRTSpecs(uiFormatIndex, &DestSpecList);

	FindValidTextureSpecs(uiFormatIndex, dwDestUsageSets, cuiNoOfDestUsageSets, 0, &DestSpecList);
	FindValidCubeTextureSpecs(uiFormatIndex, dwDestUsageSets, cuiNoOfDestUsageSets, 0, &DestSpecList);
	// The fourth argument, temporarily "hard-coded" as 0, is the pool index for D3DPOOL_DEFAULT.

	// Initialize the array.
	DestSpecList.InitializeSurfaceSpecArray(pDestSpecArray);
}

UINT CUpdateSurfaceTestMain::TestInitialize(void)
{
	HRESULT hresult;
	UINT uiTotalTestCases = 0;
	CSurface *psurfSrcBackBuffer;

	// Ensure that the bitmap resources are available.
	if (!BitmapDataForeground)
	{
		HRSRC hrsrcFindResourceResult;
		HGLOBAL hgBitmapData;

		hrsrcFindResourceResult = FindResource(TestApp.m_hInstance, MAKEINTRESOURCE(IDR_DIBFILETYPE1),
		"DIBFILETYPE");
		hgBitmapData = LoadResource(TestApp.m_hInstance, hrsrcFindResourceResult);
		BitmapDataForeground = LockResource(hgBitmapData);

		hrsrcFindResourceResult = FindResource(TestApp.m_hInstance, MAKEINTRESOURCE(IDR_DIBFILETYPE2),
		"DIBFILETYPE");
		hgBitmapData = LoadResource(TestApp.m_hInstance, hrsrcFindResourceResult);
		BitmapDataBackground = LockResource(hgBitmapData);
	}

	// Ensure availability of an array whose elements will specify valid source and destination
	// surface specifications on a "per format" basis.
	if (!ValidSurfacesByFormat)
		ValidSurfacesByFormat = new ValidSurfaces[cuiNoOfFormats];

	/*
	// Ensure availability of an array whose elements will (in conjunction with
	// uiNoOfValidFilterRecordIndices) specify texture filter types supported by the device.
	if (!ValidFilterRecordIndices)
		ValidFilterRecordIndices = new UINT[cuiNoOfFilterTypes];
	*/

//////////////////////////////////////////////
//******************************************//

	// Obtain a pointer to a surface representing a back buffer.
	if (SUCCEEDED((hresult = m_pSrcDevice->/*GetBackBuffer8*/GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &psurfSrcBackBuffer))))
	{
		// Get details of the back buffer surface.
		hresult = psurfSrcBackBuffer->/*GetDesc8*/GetDesc(&sdSrcBackBuffer);
		psurfSrcBackBuffer->Release();
		
		if (SUCCEEDED(hresult))
		{
			UINT uiPanelWidth, uiPanelHeight, uiDisplayEdgeLength;
			CONST UINT cuiMarginAdjustment = 4;

			// Define the "destination" rectangles for copying reference and test maps
			// to the back buffer for display.

			uiPanelWidth = sdSrcBackBuffer.Width >> 1;

			uiPanelHeight = sdSrcBackBuffer.Height;
			if (uiPanelWidth < uiPanelHeight)
				uiDisplayEdgeLength = uiPanelWidth - (uiPanelWidth >> cuiMarginAdjustment);
			else
				uiDisplayEdgeLength = uiPanelHeight - (uiPanelHeight >> cuiMarginAdjustment);

			// Define rectangles for displaying the test and reference  map images.

			rectReferenceDisplay.left = ((sdSrcBackBuffer.Width + 1) >> 1) + (uiPanelWidth >> 1) - (uiDisplayEdgeLength >> 1);
			rectReferenceDisplay.top = (uiPanelHeight >> 1) - (uiDisplayEdgeLength >> 1);
			rectReferenceDisplay.right = rectReferenceDisplay.left + uiDisplayEdgeLength;
			rectReferenceDisplay.bottom = rectReferenceDisplay.top + uiDisplayEdgeLength;

			rectTestDisplay.left = (uiPanelWidth >> 1) - (uiDisplayEdgeLength >> 1);
			rectTestDisplay.top = rectReferenceDisplay.top;
			rectTestDisplay.right = rectTestDisplay.left + uiDisplayEdgeLength;
			rectTestDisplay.bottom = rectReferenceDisplay.bottom;
		}
	}

//******************************************//
//////////////////////////////////////////////

	// Determine the adapter ordinal and the type of the Direct3D device, i.e. HAL, REF, etc.
	{
		DEVICE_CREATION_PARAMETERS dcpSrcDevice;

		hresult = m_pSrcDevice->GetCreationParameters(&dcpSrcDevice);
		uiSrcDeviceAdapterOrdinal = dcpSrcDevice.AdapterOrdinal;
		dwSrcDeviceType = (DWORD)(dcpSrcDevice.DeviceType);
	}

	// Retrieve the caps for the tested device.
	hresult = m_pD3D->GetDeviceCaps(uiSrcDeviceAdapterOrdinal, dwSrcDeviceType, &SrcDeviceCaps);

	// Get the adapter's current display mode.
	hresult = m_pD3D->GetAdapterDisplayMode(uiSrcDeviceAdapterOrdinal, &displaymodeAdapter);

	ClearAuxiliaryResources();
	if (!InitializeAuxiliaryResources())
	{
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

	/*
	// Determine the set of texture filter types supported by the tested device.
	{
		UINT uiFilterIndex;
		DWORD dwNotStretchRectFilterCaps = ~(SrcDeviceCaps.StretchRectFilterCaps);

		uiNoOfValidFilterRecordIndices = 0;
		for (uiFilterIndex = 0; uiFilterIndex < cuiNoOfFilterTypes; ++uiFilterIndex)
		{
			if (!(((DXFilterTypes[uiFilterIndex]).StretchRectCapsBits) & dwNotStretchRectFilterCaps))
				ValidFilterRecordIndices[uiNoOfValidFilterRecordIndices++] = uiFilterIndex;
		}
	}
	*/

	// For each format, determine the set of valid source surface specs and the set of valid
	// destination surface specs.
	{
		UINT uiFormatIndex;

		for (uiFormatIndex = 0; uiFormatIndex < cuiNoOfFormats; ++uiFormatIndex)
		{
			FindValidSourceSurfaceSpecs(uiFormatIndex, &((ValidSurfacesByFormat[uiFormatIndex]).ValidSourceSpecs));
			FindValidDestSurfaceSpecs(uiFormatIndex, &((ValidSurfacesByFormat[uiFormatIndex]).ValidDestSpecs));
		}
	}

	// For each valid source/destination format pair, compute and record the number of test cases.
	{
		UINT uiSourceFormatIndex;

		for (uiSourceFormatIndex = 0; uiSourceFormatIndex < cuiNoOfFormats; ++uiSourceFormatIndex)
		{
			UINT uiNoOfSourceSpecs;

			if (uiNoOfSourceSpecs = ((ValidSurfacesByFormat[uiSourceFormatIndex]).ValidSourceSpecs).GetNumberOfElements())
			{
				FORMAT fmtSource((DXGraphicsFormats[uiSourceFormatIndex]).fmtFW);
				UINT uiDestFormatIndex;

				for (uiDestFormatIndex = 0; uiDestFormatIndex < cuiNoOfFormats; ++uiDestFormatIndex)
				{
					UINT uiNoOfDestSpecs;

					if (uiNoOfDestSpecs = ((ValidSurfacesByFormat[uiDestFormatIndex]).ValidDestSpecs).GetNumberOfElements())
					{
						FORMAT fmtDest((DXGraphicsFormats[uiDestFormatIndex]).fmtFW);

						//if (SUCCEEDED((hresult = m_pD3D->CheckDeviceFormatConversion(uiSrcDeviceAdapterOrdinal, dwSrcDeviceType, fmtSource, fmtDest))))
						if (uiSourceFormatIndex == uiDestFormatIndex)
						{
							UINT uiNoOfTestCases;
							FormatPair *pNewFormatPairElement;

							uiNoOfTestCases =
							uiNoOfSourceSpecs *
							uiNoOfDestSpecs *
							cuiNoOfSourceDestPairs;
							/*
							cuiSourceLeftVals *
							cuiSourceTopVals *
							cuiSourceWidthVals *
							cuiSourceHeightVals *
							cuiDestLeftVals *
							cuiDestTopVals *
							cuiDestWidthVals *
							cuiDestHeightVals;
							uiNoOfValidFilterRecordIndices
							*/

							// Record the source/destination format pair and the associated number of test cases.
							pNewFormatPairElement = TestCasesByFormatPair.AddNewElement();
							pNewFormatPairElement->uiSourceFormatIndex = uiSourceFormatIndex;
							pNewFormatPairElement->uiDestFormatIndex = uiDestFormatIndex;
							pNewFormatPairElement->uiNoOfTestCases = uiNoOfTestCases;

							uiTotalTestCases += uiNoOfTestCases;
						}
					}
				}
			}
		}
	}

	/*
	// Initialize the array of surface specifications, based on adapter ordinal, device type,
	// current display mode, etc.
	FindValidSurfaceSpecs();
	*/

	// Compute the number of test cases and set the test range accordingly.
	/*
	if (!(uiNoOfSurfaceTypes = SurfaceSpecArray.GetNumberOfElements()))
		return D3DTESTINIT_ABORT;
	*/

	/*
	SetTestRange
	(
		1,
		uiNoOfSurfaceTypes *
		uiNoOfSurfaceTypes *
		cuiSourceLeftVals *
		cuiSourceTopVals *
		cuiSourceWidthVals *
		cuiSourceHeightVals *
		cuiDestLeftVals *
		cuiDestTopVals *
		cuiDestWidthVals *
		cuiDestHeightVals
	);
	*/

	if (uiTotalTestCases)
	{
		SetTestRange(1, uiTotalTestCases);
		return D3DTESTINIT_RUN;
	}

	return D3DTESTINIT_ABORT;
}

bool CUpdateSurfaceTestMain::ExecuteTest(UINT uiTestCaseId)
{
	UINT uiFormatPairSpecificId = uiTestCaseId - 1;
	clsFormatPairListScanner FormatPairListScanner;
	FormatPair *pCurrentPair;
	UINT
	uiNoOfTestCases, uiNoOfSourceSurfaceSpecs, uiNoOfDestSurfaceSpecs,
	uiSourceSpecIndex, uiDestSpecIndex,
	uiSourceDestPairIndex;
	clsSurfaceSpecArray *pSourceSurfaceSet, *pDestSurfaceSet;
	const float fSurfaceMinimumConformanceFraction = 0.90f;

	// Determine the source-format/destination-format pair and the offset of the test case
	// within the set of test cases associated with the pair.
	FormatPairListScanner.Reset(&TestCasesByFormatPair);
	if (!(pCurrentPair = FormatPairListScanner.GetCurrent()))
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}
	uiNoOfTestCases = pCurrentPair->uiNoOfTestCases;
	if (uiFormatPairSpecificId >= uiNoOfTestCases)
		do
		{
			FormatPairListScanner.Advance();
			if (!(pCurrentPair = FormatPairListScanner.GetCurrent()))
			{
				ReportInvalidTCID(uiTestCaseId);
				return false;
			}
			uiFormatPairSpecificId -= uiNoOfTestCases;

			uiNoOfTestCases = pCurrentPair->uiNoOfTestCases;
		}
		while (uiFormatPairSpecificId >= uiNoOfTestCases);

	// Based on the source and destination formats identified by *pCurrentPair, determine the
	// sets of source and destination specifications.
	pSourceSurfaceSet = &((ValidSurfacesByFormat[pCurrentPair->uiSourceFormatIndex]).ValidSourceSpecs);
	uiNoOfSourceSurfaceSpecs = pSourceSurfaceSet->GetNumberOfElements();
	pDestSurfaceSet = &((ValidSurfacesByFormat[pCurrentPair->uiDestFormatIndex]).ValidDestSpecs);
	uiNoOfDestSurfaceSpecs = pDestSurfaceSet->GetNumberOfElements();

	// Derive the test case attributes---each an array index---from the test case descriptor.

	/*
	uiFilterIndex = uiFormatPairSpecificId % uiNoOfValidFilterRecordIndices;
	uiFormatPairSpecificId /= uiNoOfValidFilterRecordIndices;
	*/

	uiSourceDestPairIndex = uiFormatPairSpecificId % cuiNoOfSourceDestPairs;
	uiFormatPairSpecificId /= cuiNoOfSourceDestPairs;
	uiDestSpecIndex = uiFormatPairSpecificId % uiNoOfDestSurfaceSpecs;
	if ((uiSourceSpecIndex = uiFormatPairSpecificId / uiNoOfDestSurfaceSpecs) >= uiNoOfSourceSurfaceSpecs)
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}

	// Record commencement of a test case.
	BeginTestCase("Test Case", uiTestCaseId);

	// Prepare the source and destination surface sets.
	
	if (!SourceSurfaceSet.PrepareFromDWORD(this, pSourceSurfaceSet->GetElementValue(uiSourceSpecIndex)))
	{
		WriteToLog(_T("The test was unable to prepare the set of source surfaces.\n"));
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiTestCaseId);
		return false;
	}

	if (!DestSurfaceSet.PrepareFromDWORD(this, pDestSurfaceSet->GetElementValue(uiDestSpecIndex)))
	{
		WriteToLog(_T("The test was unable to prepare the set of destination surfaces.\n"));
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiTestCaseId);
		return false;
	}

	if (!TestUpdateSurfaceOperation((SourceDestPairs[uiSourceDestPairIndex]).rectSource, (SourceDestPairs[uiSourceDestPairIndex]).pointDest, fSurfaceMinimumConformanceFraction))
	{
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiTestCaseId);
		return false;
	}

	return true;
}

bool CUpdateSurfaceTestMain::TestTerminate(void)
{
	SourceSurfaceSet.ReleaseAllSurfaces();
	DestSurfaceSet.ReleaseAllSurfaces();
	TestCasesByFormatPair.DeleteAll();
	ClearAuxiliaryResources();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// "Wrapped Surface Client" methods

HRESULT CUpdateSurfaceTestMain::WSCCreateTexture(UINT uiWidth, UINT uiHeight, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CTexture **ppTexture)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(fmt);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice->CreateTexture
				(
					uiWidth,
					uiHeight,
					uiLevels,
					dwUsage,
					FrameworkFormat,
					dwPool,
					ppTexture
				)
			)
		)
	)
	{
		char szErrorMessage[cuiMaxErrorMessageLength];
		char szUsageString[cuiMaxErrorMessageLength];

		ListUsages(dwUsage, szUsageString);
		sprintf(szErrorMessage,
		"Texture Creation Failed.  Width: %d; Height: %d; Levels: %d; Usage: %s; Format: %s; Pool: %s\n",
		uiWidth, uiHeight, uiLevels, szUsageString, (FindFormatRecord(fmt))->szName, (FindPoolRecord(dwPool))->szName);
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

HRESULT CUpdateSurfaceTestMain::WSCCreateCubeTexture(UINT uiEdgeLength, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CCubeTexture **ppCubeTexture)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(fmt);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice->CreateCubeTexture
				(
					uiEdgeLength,
					uiLevels,
					dwUsage,
					FrameworkFormat,
					dwPool,
					ppCubeTexture
				)
			)
		)
	)
	{
		char szErrorMessage[cuiMaxErrorMessageLength];
		char szUsageString[cuiMaxErrorMessageLength];

		ListUsages(dwUsage, szUsageString);
		sprintf(szErrorMessage,
		"CubeTexture Creation Failed.  Edge Length: %d; Levels: %d; Usage: %s; Format: %s; Pool: %s\n",
		uiEdgeLength, uiLevels, szUsageString, (FindFormatRecord(fmt))->szName, (FindPoolRecord(dwPool))->szName);
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

HRESULT CUpdateSurfaceTestMain::WSCCreateOffscreenPlainSurface(UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool, CSurface **ppSurface)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(fmt);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice->CreateOffscreenPlainSurface
				(
					uiWidth,
					uiHeight,
					FrameworkFormat,
					dwPool,
					ppSurface
				)
			)
		)
	)
	{
		char szErrorMessage[cuiMaxErrorMessageLength];

		sprintf(szErrorMessage, "CreateOffscreenPlainSurface failed.  Width: %d; Height: %d; Format: %s; Pool: %s\n",
		uiWidth, uiHeight, (FindFormatRecord(fmt))->szName, (FindPoolRecord(dwPool))->szName);
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

HRESULT CUpdateSurfaceTestMain::WSCCreateRenderTarget(UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE MultiSample, DWORD dwMultisampleQuality, BOOL bLockable, CSurface **ppSurface)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(fmt);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice->CreateRenderTarget
				(
					uiWidth,
					uiHeight,
					FrameworkFormat,
					MultiSample,
					dwMultisampleQuality,
					bLockable,
					ppSurface
				)
			)
		)
	)
	{
		char szErrorMessage[cuiMaxErrorMessageLength];

		sprintf(szErrorMessage, "CreateRenderTarget failed.  Width: %d; Height: %d; Format: %s; Multisample: %s; Multisample Quality: %d; Lockable: %s\n",
		uiWidth, uiHeight, (FindFormatRecord(fmt))->szName, (FindMultiSampleRecord(MultiSample))->szName, dwMultisampleQuality, bLockable ? "TRUE" : "FALSE");
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

////////////////////////////////////////////////////////////////////////////////
// Surface Specification Interpretation

clsTestSurface *CUpdateSurfaceTestMain::CreateSurfaceFromDWORD(DWORD dwSpec)
{
	clsTestSurface *ptestsurfNew = NULL;
	UINT uiTypeIndex, uiFormatIndex, uiDimensionIndex;

	WriteToLog("\nCreateSurfaceFromDWORD called with dwSpec==%d\n", dwSpec);

	if ((uiFormatIndex = (UINT)ExtractBitsRange(dwSpec, 16, 23)) < cuiNoOfFormats)
		if ((uiDimensionIndex = (UINT)ExtractBitsRange(dwSpec, 24, 27)) < cuiNoOfDimensions)
			switch (uiTypeIndex = (UINT)ExtractBitsRange(dwSpec, 28, 31))
			{
			case 0:
			// Back buffer surface
				break;
			case 1:
			// Render target surface
				{
					clsRenderTargetSurface *prtsurfNew = NULL;
					UINT uiMultiSampleIndex;

					if ((uiDimensionIndex = (UINT)ExtractBitsRange(dwSpec, 24, 27)) < cuiNoOfDimensions)
						if ((uiMultiSampleIndex = (UINT)ExtractBitsRange(dwSpec, 8, 15)) < cuiNoOfMultiSampleTypes)
							if (prtsurfNew = new clsRenderTargetSurface)
								if
								(
									prtsurfNew->Initialize
									(
										(clsWrappedSurfaceClient *)this,
										SurfaceDimensions[uiDimensionIndex].uiWidth,
										SurfaceDimensions[uiDimensionIndex].uiHeight,
										DXGraphicsFormats[uiFormatIndex].fmtFW,
										DXMultiSampleTypes[uiMultiSampleIndex].mstFW,
										(dwSpec & 1) ? TRUE : FALSE
									)
								)
									ptestsurfNew = prtsurfNew;
								else
									delete prtsurfNew;
				}
				break;
			case 2:
			// Texture surface
				{
					UINT uiPoolTypeIndex;

					if ((uiPoolTypeIndex = (UINT)ExtractBitsRange(dwSpec, 8, 15)) < cuiNoOfPoolTypes)
					{
						DWORD *dwPoolBasedUsageSets = NULL;
						UINT uiPoolBasedNoOfUsageSets;
						UINT uiUsageIndex;

						// For now, reference the pool types (assumed to be D3DPOOL_DEFAULT or
						// D3DPOOL_SYSTEMMEM) in terms of the "hard-coded" indices of their
						// array entries.
						if (uiPoolTypeIndex == 0)	// D3DPOOL_DEFAULT
						{
							dwPoolBasedUsageSets = dwDestUsageSets;
							uiPoolBasedNoOfUsageSets = cuiNoOfDestUsageSets;
						}
						else	// D3DPOOL_SYSTEMMEM
						{
							dwPoolBasedUsageSets = dwSourceUsageSets;
							uiPoolBasedNoOfUsageSets = cuiNoOfSourceUsageSets;
						}

						if ((uiUsageIndex = (UINT)ExtractBitsRange(dwSpec, 0, 7)) < uiPoolBasedNoOfUsageSets)
						{
							clsTextureSurface *ptexsurfNew = NULL;

							if (ptexsurfNew = new clsTextureSurface)
								if
								(
									ptexsurfNew->Initialize
									(
										(clsWrappedSurfaceClient *)this,
										SurfaceDimensions[uiDimensionIndex].uiWidth,
										SurfaceDimensions[uiDimensionIndex].uiHeight,
										DXGraphicsFormats[uiFormatIndex].fmtFW,
										dwPoolBasedUsageSets[uiUsageIndex],
										DXPoolTypes[uiPoolTypeIndex].dwFWPool,
										1,
										0
									)
								)
									ptestsurfNew = ptexsurfNew;
								else
									delete ptexsurfNew;
						}
					}
				}
				break;
			case 3:
			// Cubemap surface
				{
					UINT uiPoolTypeIndex;

					if ((uiPoolTypeIndex = (UINT)ExtractBitsRange(dwSpec, 8, 15)) < cuiNoOfPoolTypes)
					{
						DWORD *dwPoolBasedUsageSets = NULL;
						UINT uiPoolBasedNoOfUsageSets;
						UINT uiUsageIndex;

						// For now, reference the pool types (assumed to be D3DPOOL_DEFAULT or
						// D3DPOOL_SYSTEMMEM) in terms of the "hard-coded" indices of their
						// array entries.
						if (uiPoolTypeIndex == 0)	// D3DPOOL_DEFAULT
						{
							dwPoolBasedUsageSets = dwDestUsageSets;
							uiPoolBasedNoOfUsageSets = cuiNoOfDestUsageSets;
						}
						else	// D3DPOOL_SYSTEMMEM
						{
							dwPoolBasedUsageSets = dwSourceUsageSets;
							uiPoolBasedNoOfUsageSets = cuiNoOfSourceUsageSets;
						}

						if ((uiUsageIndex = (UINT)ExtractBitsRange(dwSpec, 0, 7)) < uiPoolBasedNoOfUsageSets)
						{
							clsCubeMapSurface *ptexsurfNew = NULL;

							if (ptexsurfNew = new clsCubeMapSurface)
								if
								(
									ptexsurfNew->Initialize
									(
											(clsWrappedSurfaceClient *)this,
											SurfaceDimensions[uiDimensionIndex].uiWidth,
											DXGraphicsFormats[uiFormatIndex].fmtFW,
											dwPoolBasedUsageSets[uiUsageIndex],
											DXPoolTypes[uiPoolTypeIndex].dwFWPool
									)
								)
									ptestsurfNew = ptexsurfNew;
								else
									delete ptexsurfNew;
						}
						/*
						clsCubeMapSurface *ptexsurfNew = NULL;
						UINT uiUsageIndex, uiPoolTypeIndex;

						if ((uiUsageIndex = (UINT)ExtractBitsRange(dwSpec, 0, 7)) < cuiNoOfUsageSets)
							if ((uiPoolTypeIndex = (UINT)ExtractBitsRange(dwSpec, 8, 15)) < cuiNoOfPoolTypes)
								if (ptexsurfNew = new clsCubeMapSurface)
									if
									(
										ptexsurfNew->Initialize
										(
											(clsWrappedSurfaceClient *)this,
											SurfaceDimensions[uiDimensionIndex].uiWidth,
											DXGraphicsFormats[uiFormatIndex].fmtFW,
											dwUsageSets[uiUsageIndex],
											DXPoolTypes[uiPoolTypeIndex].dwFWPool
										)
									)
										ptestsurfNew = ptexsurfNew;
									else
										delete ptexsurfNew;
						*/
					}
				}
				break;
			case 4:
			// Offscreen Plain Surface
				{
					clsOffscreenPlainSurface *pospsurfNew = NULL;
					UINT uiPoolTypeIndex;

					if ((uiPoolTypeIndex = (UINT)ExtractBitsRange(dwSpec, 8, 15)) < cuiNoOfPoolTypes)
						if (pospsurfNew = new clsOffscreenPlainSurface)
							if
							(
								pospsurfNew->Initialize
								(
									(clsWrappedSurfaceClient *)this,
									SurfaceDimensions[uiDimensionIndex].uiWidth,
									SurfaceDimensions[uiDimensionIndex].uiHeight,
									DXGraphicsFormats[uiFormatIndex].fmtFW,
									DXPoolTypes[uiPoolTypeIndex].dwFWPool
								)
							)
								ptestsurfNew = pospsurfNew;
							else
								delete pospsurfNew;
				}
				break;
			default:
				break;
			}

	if (!ptestsurfNew)
	{
		WriteToLog("Unexpected resource creation failure.\n");
		Fail();
	}

	return ptestsurfNew;
}

bool CUpdateSurfaceTestMain::InitializeCompatibleSMSurface(SURFACEDESC sdDescription, clsTextureSurface *pSystemMemSurface)
{
	// Create a (lockable) system memory surface compatible with the
	// surface description.
	if
	(
		pSystemMemSurface->Initialize
		(
			(clsWrappedSurfaceClient *)this,
			sdDescription.dwWidth,
			sdDescription.dwHeight,
			(FMT)((sdDescription.Format).d3dfFormat),
			(DWORD)0,
			POOL_SYSTEMMEM,
			1,
			0
		)
	)
		return true;

	return false;
}

bool CUpdateSurfaceTestMain::InitializeCompatibleMMSurface(SURFACEDESC sdDescription, clsTextureSurface *pManagedMemSurface)
{
	// Create a managed memory surface compatible with the
	// surface description.
	if
	(
		pManagedMemSurface->Initialize
		(
			(clsWrappedSurfaceClient *)this,
			sdDescription.dwWidth,
			sdDescription.dwHeight,
			(FMT)((sdDescription.Format).d3dfFormat),
			(DWORD)0,
			POOL_MANAGED,
			1,
			0
		)
	)
		return true;

	return false;
}

bool CUpdateSurfaceTestMain::TestUpdateSurfaceOperation(RECT rectSource, POINT pointDest, float fSurfaceMinimumConformanceFraction)
{
	HRESULT hresult;

	CSurface *lpd3dsurfSource, *lpd3dsurfDest, *lpd3dsurfDestLoader,
	*lpd3dsurfSourceRef, *lpd3dsurfDestRef, *lpd3dsurfDestCopy;

	// Obtain pointers to the surface interfaces.
	lpd3dsurfSource = (SourceSurfaceSet.pSourceTestSurface)->GetSurfacePointer();
	lpd3dsurfDest = (DestSurfaceSet.pDestTestSurface)->GetSurfacePointer();
	lpd3dsurfDestLoader = (DestSurfaceSet.texsurfDestLoader).GetSurfacePointer();
	//lpd3dsurfSourceRef = (SourceSurfaceSet.texsurfSourceLoader).GetSurfacePointer();
	lpd3dsurfDestRef = (DestSurfaceSet.texsurfReference).GetSurfacePointer();
	lpd3dsurfDestCopy = (DestSurfaceSet.texsurfDestCopy).GetSurfacePointer();

	// First prepare the reference image.

	// Load the background image onto the destination surface for the reference copy.
	if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(lpd3dsurfDestRef->GetD3DSurface9(), NULL, NULL, lpd3dsurfDestLoader->GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
	{
		WriteToLog("Unable to load the destination surface for the reference copy.\n");
		return false;
	}

	// Perform the reference copy.
	if
	(
		!ReferenceUpdateSurface(lpd3dsurfSource, &rectSource, lpd3dsurfDestRef, &pointDest)
	)
	{
		WriteToLog("Unable to perform the reference UpdateSurface operation.\n");
		return false;
	}

	// Prepare the destination surface for the tested UpdateSurface operation.

	// Load the background image onto the destination surface.
	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice->UpdateSurface
				(
					lpd3dsurfDestLoader,
					NULL,
					lpd3dsurfDest,
					NULL
				)
			)
		)
	)
	{
		WriteToLog("Unable to copy the 'background image' onto the destination surface.\n");
		return false;
	}

	WriteToLog("About to call UpdateSurface with the following arguments:\n");
	WriteToLog("Source: %s\n", SourceSurfaceSet.szDescription);
	WriteToLog("Destination: %s\n", DestSurfaceSet.szDescription);
	WriteToLog("Source Rectangle: {%d, %d, %d, %d}\n", rectSource.left, rectSource.top, rectSource.right, rectSource.bottom);
	WriteToLog("Destination Point: {%d, %d}\n", pointDest.x, pointDest.y);
	
	// Perform the tested UpdateSurface operation.
	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice->UpdateSurface
				(
					lpd3dsurfSource,
					&rectSource,
					lpd3dsurfDest,
					&pointDest
				)
			)
		)
	)
	{
		WriteToLog("The tested UpdateSurface operation failed.\n");
		Fail();
	}
	else
	{
		// AndrewLu: Test modified on 04/17/2006.  Motivation: Windows OS Bug #1605689.
		//
		// Test the possibility that a LockRect operation against the source surface can
		// corrupt the source surface data before the UpdateSurface operation has been
		// "committed".  In this case, the destination surface will also be corrupted, at least
		// in part.

		/*
		{
			LOCKED_RECT lr;

			if (SUCCEEDED((hresult = lpd3dsurfSource->LockRect(&lr, NULL, 0))))
			{
				UINT uiY;
				BYTE *pRowStart = (BYTE *)(lr.pBits);
				UINT uiBytesPerRow = (UINT)(((DWORD)((SourceSurfaceSet.sdSourceSurfaceDesc).Width *
				(((SourceSurfaceSet.sdSourceSurfaceDesc).Format).ddpfPixelFormat).dwRGBBitCount)) >> 3);
				UINT uiRowsPerByteSequence = (((SourceSurfaceSet.sdSourceSurfaceDesc).Format).ddpfPixelFormat).dwRowsPerByteSequence;
				BYTE byteSetVal = 0;

				UINT uiNoOfByteSequences = (SourceSurfaceSet.sdSourceSurfaceDesc).Height / uiRowsPerByteSequence;

				for (uiY = 0; uiY < uiNoOfByteSequences; ++uiY)
				{
					UINT uiOffsetWithinRow;

					for (uiOffsetWithinRow = 0; uiOffsetWithinRow < uiBytesPerRow; ++uiOffsetWithinRow)
					{
						pRowStart[uiOffsetWithinRow] = 0;// byteSetVal++;
					}

					pRowStart += lr.iPitch;
				}

				lpd3dsurfSource->UnlockRect();
			}
		}
		*/
		// This effectively invalidates the source surface, forcing it to be recreated and
		// initialized when subsequent test cases are executed.
		//SourceSurfaceSet.dwMainSurfaceSpec -= 1;

		float fCompareResult;

		WriteToLog("The tested UpdateSurface operation succeeded.\n");

		// Copy the result of the tested UpdateSurface operation onto a lockable surface
		// so that it can be compared with the result of the reference copy.
		/*
		if
		(
			FAILED
			(
				(
					hresult =
					m_pSrcDevice->CopyRects
					(
						lpd3dsurfDest,
						NULL,
						0,
						lpd3dsurfDestCopy,
						NULL
					)
				)
			)
		)
		*/

		/*
		if (!((DestSurfaceSet.pDestTestSurface)->DumpContentToSysMem(this, lpd3dsurfDestCopy)))
		{
			WriteToLog("Unable to retrieve the result of the tested copy for comparison with the reference copy.\n");
			return false;
		}
		*/


		//::D3DXSaveSurfaceToFile( "lpd3dsurfDestCopy.dds", D3DXIFF_DDS, lpd3dsurfDestCopy->GetD3DSurface9(), NULL, NULL );
		//::D3DXSaveSurfaceToFile( "lpd3dsurfDestRef.dds", D3DXIFF_DDS, lpd3dsurfDestRef->GetD3DSurface9(), NULL, NULL );
		// Compare the buffers
		// TestApp
		/*
		if (FAILED((TestApp.m_ImageCmp).CompareImages(lpd3dsurfDestCopy, lpd3dsurfDestRef, NULL)))
		{
			WriteToLog("Unable to compare the test and reference images.\n");
				return false;
		}
		*/
		if (!((DestSurfaceSet.pDestTestSurface)->VerifyAgainstSysMemTexture(this, (DestSurfaceSet.texsurfReference).GetTexturePointer(), &fCompareResult)))
		{
			WriteToLog("Unable to retrieve the result of the tested copy for comparison with the reference copy.\n");
			return false;
		}


		// Check the results
		/*
		if ((TestApp.m_ImageCmp).GetCompareResult(1.f - m_Options.TestOptions.fPassRequired, m_Options.TestOptions.fColorRange, 
										m_Options.TestOptions.uMinPixels, m_Options.TestOptions.uMaxPixels))
		*/
		//fCompareResult = (TestApp.m_ImageCmp).GetCompareResult();

		if (fCompareResult >= fSurfaceMinimumConformanceFraction)
		{
			Pass();
			return true;
		}
		else WriteToLog("Expected conformance: %f; Observed conformance: %f\n", fSurfaceMinimumConformanceFraction, fCompareResult);

		WriteToLog("The reference and test images differ significantly.\n");
		Fail();
	}

	// The tested UpdateSurface operation failed or generated an incorrect result.  Report the
	// details of the surfaces, rectangles, and filter type involved.
	WriteToLog("Source: %s\n", SourceSurfaceSet.szDescription);
	WriteToLog("Destination: %s\n", DestSurfaceSet.szDescription);
	WriteToLog("Source Rectangle: {%d, %d, %d, %d}\n", rectSource.left, rectSource.top, rectSource.right, rectSource.bottom);
	WriteToLog("Destination Point: {%d, %d}\n", pointDest.x, pointDest.y);

	return true;
}

bool CUpdateSurfaceTestMain::ReferenceUpdateSurface(CSurface *pSourceSurface, RECT *pSourceRects, CSurface *pDestinationSurface, POINT *pDestPoint)
{
	HRESULT hresult;
	RECT rectDest;

	if (pDestPoint)
	{
		rectDest.left = pDestPoint->x;
		rectDest.top = pDestPoint->y;
	}
	else
	{
		rectDest.left = pSourceRects->left;
		rectDest.top = pSourceRects->top;
	}

	rectDest.right = rectDest.left + pSourceRects->right - pSourceRects->left;
	rectDest.bottom = rectDest.top + pSourceRects->bottom - pSourceRects->top;

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				/*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface
				(
					pDestinationSurface->GetD3DSurface9(),
					NULL,
					&rectDest,
					pSourceSurface->GetD3DSurface9(),
					NULL,
					pSourceRects,
					FILTER_NONE,
					0
				)
			)
		)
	)
		return true;
	else
		return false;

	return true;
}

//virtual HRESULT     D3DXLoadSurfaceFromSurface9(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);

bool CUpdateSurfaceTestMain::DisplayFrame(void)
{
	HRESULT hresult;
	CSurface *psurfSrcBackBuffer = NULL;

	if (SUCCEEDED((hresult = m_pSrcDevice->/*GetBackBuffer8*/GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &psurfSrcBackBuffer))))
	{
		// Copy the appropriate reference image(s) to the back buffer for display.
		CSurface *pReferenceSurface = NULL;
		CSurface *pTestSurface = NULL;

		pReferenceSurface = psurfAuxiliaryRenderTargetRef; //(DestSurfaceSet.texsurfReference).GetSurfacePointer();
		pTestSurface = psurfAuxiliaryRenderTargetTest; //(DestSurfaceSet.texsurfDestCopy).GetSurfacePointer();

		if (pReferenceSurface && pTestSurface)
		{
			hresult =
			::D3DXLoadSurfaceFromSurface
			(
				psurfSrcBackBuffer->/*GetD3DSurface8*/GetD3DSurface9(),
				NULL,
				&rectReferenceDisplay,
				pReferenceSurface->/*GetD3DSurface8*/GetD3DSurface9(),
				NULL,
				NULL,
				D3DX_FILTER_LINEAR,
				0
			);

			// Copy the appropriate reference image(s) to the back buffer for display.

			hresult =
			::D3DXLoadSurfaceFromSurface
			(
				psurfSrcBackBuffer->/*GetD3DSurface8*/GetD3DSurface9(),
				NULL,
				&rectTestDisplay,
				pTestSurface->/*GetD3DSurface8*/GetD3DSurface9(),
				NULL,
				NULL,
				D3DX_FILTER_LINEAR,
				0
			);
		}

		psurfSrcBackBuffer->Release();
	}

	m_pSrcDevice->Present(NULL, NULL, NULL, NULL);

	return true;
}

HRESULT CUpdateSurfaceTestMain::MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture)
{
	HRESULT hresult = E_FAIL;
	SURFACEDESC sdSysmemSurface;

	if (pSysmemSurface == NULL)
	{
		return E_INVALIDARG;
	}

	if (pVidmemTexture == NULL)
	{
		return E_INVALIDARG;
	}


	// Obtain the dimensions of the system memory surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pSysmemSurface->GetDesc(&sdSysmemSurface))))
		WriteToLog(_T("GetDesc failed in UpdateTex::MyLoadSurfaceFromTexture.\n"));
	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;
		CSurface *pCurrentRenderTarget = NULL, *pCurrentDepthStencil = NULL;

		// Set the stream source and vertex shader.
		m_pSrcDevice->SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		m_pSrcDevice->SetFVF(dwD3DFVF_VERTEX);

		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		m_pSrcDevice->GetRenderTarget(&pCurrentRenderTarget);
		m_pSrcDevice->GetDepthStencilSurface(&pCurrentDepthStencil);

		// Set the render target and viewport.
		m_pSrcDevice->SetRenderTarget(psurfAuxiliaryRenderTargetTest, NULL);
		vpRenderTexture.X = (vpRenderTexture.Y = 0);
		vpRenderTexture.Width = sdSysmemSurface.Width;
		vpRenderTexture.Height = sdSysmemSurface.Height;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;
		m_pSrcDevice->SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice->SetTexture(0, pVidmemTexture);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice->BeginScene();
		m_pSrcDevice->Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x00, 0x00, 0x00),
			0.5f,
			0
		);
		m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice->EndScene();

		// Restore the original render target and depth stencil.
		m_pSrcDevice->SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget->Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil->Release();

		// Copy the contents of the viewport to the system memory texture.
		rectViewport.left = vpRenderTexture.X;
		rectViewport.top = vpRenderTexture.Y;
		rectViewport.right = rectViewport.left + vpRenderTexture.Width;
		rectViewport.bottom = rectViewport.top + vpRenderTexture.Height;

		if (!pSysmemSurface)
			DebugOut.Write("pSysmemSurface is NULL!");
		if (!psurfAuxiliaryRenderTargetTest)
			DebugOut.Write("psurfAuxiliaryRenderTargetTest is NULL!");

		/*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(pSysmemSurface->GetD3DSurface9(), NULL, NULL, psurfAuxiliaryRenderTargetTest->GetD3DSurface9(), NULL, &rectViewport, FILTER_NONE, 0);
	}

	return hresult;
}

HRESULT CUpdateSurfaceTestMain::MyLoadSurfaceFromCubeTexture(CSurface *pSysmemSurface, CCubeTexture *pVidmemTexture)
{
	HRESULT hresult = E_FAIL;
	SURFACEDESC sdSysmemSurface;

	if (pSysmemSurface == NULL)
	{
		return E_INVALIDARG;
	}

	if (pVidmemTexture == NULL)
	{
		return E_INVALIDARG;
	}


	// Obtain the dimensions of the system memory surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pSysmemSurface->GetDesc(&sdSysmemSurface))))
		WriteToLog(_T("GetDesc failed in UpdateTex::MyLoadSurfaceFromTexture.\n"));
	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;
		CSurface *pCurrentRenderTarget = NULL, *pCurrentDepthStencil = NULL;

		// Set the stream source and vertex shader.
		m_pSrcDevice->SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		m_pSrcDevice->SetFVF(dwD3DFVF_VERTEX);

		/*
		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		m_pSrcDevice->GetRenderTarget(&pCurrentRenderTarget);
		m_pSrcDevice->GetDepthStencilSurface(&pCurrentDepthStencil);
		*/

		// Set the render target and viewport.
		m_pSrcDevice->SetRenderTarget(psurfAuxiliaryRenderTargetTest, NULL);
		vpRenderTexture.X = (vpRenderTexture.Y = 0);
		vpRenderTexture.Width = sdSysmemSurface.Width;
		vpRenderTexture.Height = sdSysmemSurface.Height;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;
		m_pSrcDevice->SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice->SetTexture(0, pVidmemTexture);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice->Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice->BeginScene();
		m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice->EndScene();

		/*
		// Restore the original render target and depth stencil.
		m_pSrcDevice->SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget->Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil->Release();
		*/

		// Copy the contents of the viewport to the system memory texture.
		rectViewport.left = vpRenderTexture.X;
		rectViewport.top = vpRenderTexture.Y;
		rectViewport.right = rectViewport.left + vpRenderTexture.Width;
		rectViewport.bottom = rectViewport.top + vpRenderTexture.Height;

		if (!pSysmemSurface)
			DebugOut.Write("pSysmemSurface is NULL!");
		if (!psurfAuxiliaryRenderTargetTest)
			DebugOut.Write("psurfAuxiliaryRenderTargetTest is NULL!");

		/*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(pSysmemSurface->GetD3DSurface9(), NULL, NULL, psurfAuxiliaryRenderTargetTest->GetD3DSurface9(), NULL, &rectViewport, FILTER_NONE, 0);
	}

	return hresult;
}

HRESULT CUpdateSurfaceTestMain::MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface)
{
	HRESULT hresult;

	// Now that GetRenderTarget can accept a system memory texture surface as a destination,
	// it should be unnecessary to create a temporary offscreen plain surface.  Since
	// pSysmemSurface should always point to a system memory texture surface, simply call
	// GetRenderTargetData.

	/*
	SURFACEDESC sdRT;

	if (FAILED((hresult = pRTSurface->GetDesc(&sdRT))))
		WriteToLog(_T("GetDesc failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
	else
	{
		CSurface *pTemporaryOSP = NULL;

		if (FAILED((hresult = m_pSrcDevice->CreateOffscreenPlainSurface(sdRT.Width, sdRT.Height, sdRT.Format, POOL_SYSTEMMEM, &pTemporaryOSP))))
			WriteToLog(_T("CreateOffScreenPlainSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
		else
		{
			WriteToLog("About to call GetRenderTargetData ...\n");

			if (FAILED((hresult = m_pSrcDevice->GetRenderTargetData(pRTSurface, pTemporaryOSP))))
				WriteToLog(_T("GetRenderTargetData failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
			else
			{
				WriteToLog(_T("GetRenderTargetData succeeded in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
				if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(pSysmemSurface->GetD3DSurface9(), NULL, NULL, pTemporaryOSP->GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
					WriteToLog(_T("D3DXLoadSurfaceFromSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
			}

			pTemporaryOSP->Release();
		}
	}
	*/

	return (hresult = m_pSrcDevice->GetRenderTargetData(pRTSurface, pSysmemSurface));
}

HRESULT CUpdateSurfaceTestMain::MyLoadSurfaceFromOSP(CSurface *pSysmemSurface, CSurface *pOSPSurface)
{
	HRESULT hresult;

	if (FAILED((hresult = /*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(pSysmemSurface->GetD3DSurface9(), NULL, NULL, pOSPSurface->GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
		WriteToLog(_T("D3DXLoadSurfaceFromSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromOSP.\n"));

	return hresult;
}

bool CUpdateSurfaceTestMain::CompareRenderedTextures(CTexture *pReferenceTexture, CTexture *pTestTexture, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	SURFACEDESC sdManMemSurface;

	// Obtain the dimensions of the texture surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pReferenceTexture->GetLevelDesc(0, &sdManMemSurface))))
		WriteToLog(_T("GetLevelDesc failed in CompareRenderedTextures.\n"));
	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;
		CSurface *pCurrentRenderTarget = NULL, *pCurrentDepthStencil = NULL;

		vpRenderTexture.X = (vpRenderTexture.Y = 0);
		vpRenderTexture.Width = sdManMemSurface.Width;
		vpRenderTexture.Height = sdManMemSurface.Height;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;

		// Set the stream source and vertex shader.
		m_pSrcDevice->SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		m_pSrcDevice->SetFVF(dwD3DFVF_VERTEX);

		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		m_pSrcDevice->GetRenderTarget(&pCurrentRenderTarget);
		m_pSrcDevice->GetDepthStencilSurface(&pCurrentDepthStencil);

		//////////////////////////////////////////////////////////////
		// Render the test image:

		// Set the render target and viewport for rendering the test image.
		m_pSrcDevice->SetRenderTarget(psurfAuxiliaryRenderTargetTest, NULL);
		m_pSrcDevice->SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice->SetTexture(0, pTestTexture);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice->BeginScene();
		m_pSrcDevice->Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x00, 0x00, 0x00),
			0.5f,
			0
		);
		m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice->EndScene();

		//////////////////////////////////////////////////////////////
		// Render the reference image:

		// Set the render target and viewport for rendering the reference image.
		m_pSrcDevice->SetRenderTarget(psurfAuxiliaryRenderTargetRef, NULL);
		m_pSrcDevice->SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice->SetTexture(0, pReferenceTexture);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice->BeginScene();
		m_pSrcDevice->Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x00, 0x00, 0x00),
			0.5f,
			0
		);
		m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice->EndScene();

		// Restore the original render target and depth stencil.
		m_pSrcDevice->SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget->Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil->Release();

		// Compare the test and reference images.
		if (FAILED((TestApp.m_ImageCmp).CompareImages(psurfAuxiliaryRenderTargetTest, psurfAuxiliaryRenderTargetRef, NULL)))
			WriteToLog("Unable to compare the test and reference images (CompareRenderedTextures).\n");
		else
		{
			*pfSurfaceConformanceFraction = (TestApp.m_ImageCmp).GetCompareResult();
			return true;
		}
	}

	return false;
}

bool CUpdateSurfaceTestMain::CompareRenderedCubeMaps(CCubeTexture *pReferenceTexture, CCubeTexture *pTestTexture, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	SURFACEDESC sdManMemSurface;

	// Obtain the dimensions of a cubemap surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pReferenceTexture->GetLevelDesc(0, &sdManMemSurface))))
		WriteToLog(_T("GetLevelDesc failed in CompareRenderedCubeMaps.\n"));
	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;
		CSurface *pCurrentRenderTarget = NULL, *pCurrentDepthStencil = NULL;

		vpRenderTexture.X = (vpRenderTexture.Y = 0);
		vpRenderTexture.Width = sdManMemSurface.Width;
		vpRenderTexture.Height = sdManMemSurface.Height;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;

		// Set the stream source and vertex shader.
		m_pSrcDevice->SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		m_pSrcDevice->SetFVF(dwD3DFVF_VERTEX);

		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		m_pSrcDevice->GetRenderTarget(&pCurrentRenderTarget);
		m_pSrcDevice->GetDepthStencilSurface(&pCurrentDepthStencil);

		//////////////////////////////////////////////////////////////
		// Render the test image:

		// Set the render target and viewport for rendering the test image.
		m_pSrcDevice->SetRenderTarget(psurfAuxiliaryRenderTargetTest, NULL);
		m_pSrcDevice->SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice->SetTexture(0, pTestTexture);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		//m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice->Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			0xffffffff,
			//RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice->BeginScene();
		m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice->EndScene();

		//////////////////////////////////////////////////////////////
		// Render the reference image:

		//pReferenceTexture->AddDirtyRect(NULL);

		// Set the render target and viewport for rendering the reference image.
		m_pSrcDevice->SetRenderTarget(psurfAuxiliaryRenderTargetRef, NULL);
		m_pSrcDevice->SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice->SetTexture(0, pReferenceTexture);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		//m_pSrcDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice->SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice->Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			0xffffffff,
			//RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice->BeginScene();
		m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice->EndScene();

		// Restore the original render target and depth stencil.
		m_pSrcDevice->SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget->Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil->Release();

		// Compare the test and reference images.
		if (FAILED((TestApp.m_ImageCmp).CompareImages(psurfAuxiliaryRenderTargetTest, psurfAuxiliaryRenderTargetRef, NULL)))
			WriteToLog("Unable to compare the test and reference images (CompareRenderedTextures).\n");
		else
		{
			*pfSurfaceConformanceFraction = (TestApp.m_ImageCmp).GetCompareResult();
			return true;
		}
	}

	return false;
}

bool CUpdateSurfaceTestMain::CompareCubeTextureToTexture(CTexture *pManagedTexture, CCubeTexture *pCubeTexture, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	SURFACEDESC sdManMemSurface;
	bool bReturnVal = false;

	// Obtain the dimensions of the texture surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pManagedTexture->GetLevelDesc(0, &sdManMemSurface))))
		WriteToLog(_T("GetLevelDesc failed in CompareCubeTextureToTexture.\n"));
	else
	{
		CSurface *pManTexSurface = NULL;

		if (FAILED((hresult = pManagedTexture->GetSurfaceLevel(0, &pManTexSurface))))
			WriteToLog("GetSurfaceLevel failed in CompareCubeTextureToTexture.\n");
		else
		{
			CCubeTexture *pTemporarySysMemTexture = NULL;

			if (FAILED((hresult = m_pSrcDevice->CreateCubeTexture(sdManMemSurface.Width, 1, 0, sdManMemSurface.Format, POOL_SYSTEMMEM, &pTemporarySysMemTexture))))
				WriteToLog("Failed to create the temporary system memory texture in CompareCubeTextureToTexture.\n");
			else
			{
				CCubeTexture *pTemporaryVidMemTexture = NULL;

				if (FAILED((hresult = m_pSrcDevice->CreateCubeTexture(sdManMemSurface.Width, 1, 0, sdManMemSurface.Format, POOL_DEFAULT, &pTemporaryVidMemTexture))))
					WriteToLog("Failed to create the temporary video memory texture in CompareCubeTextureToTexture.\n");
				else
				{
					CSurface *pSurfaceCopy = NULL;

					if (FAILED((hresult = pTemporarySysMemTexture->GetCubeMapSurface(CUBEMAP_FACE_POSITIVE_Z, 0, &pSurfaceCopy))))
						WriteToLog("GetSurfaceLevel (temporary texture) failed in CompareCubeTextureToTexture.\n");
					else
					{
						if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(pSurfaceCopy->GetD3DSurface9(), NULL, NULL, pManTexSurface->GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
							WriteToLog("D3DXLoadSurfaceFromSurface failed in CompareOffscreenPlainToTexture.\n");
						else
						{
							if (FAILED((hresult = m_pSrcDevice->UpdateTexture(pTemporarySysMemTexture, pTemporaryVidMemTexture))))
								WriteToLog("UpdateTexture failed in CompareOffscreenPlainToTexture.\n");
							else
							{
								bReturnVal = CompareRenderedCubeMaps(pTemporaryVidMemTexture, pCubeTexture, pfSurfaceConformanceFraction);
							}
						}

						pSurfaceCopy->Release();
					}

					pTemporaryVidMemTexture->Release();
				}

				pTemporarySysMemTexture->Release();
			}

			pManTexSurface->Release();
		}
	}

	return bReturnVal;
}

bool CUpdateSurfaceTestMain::CompareOffscreenPlainToTexture(CTexture *pManagedTexture, CSurface *pOffscreenPlainSurface, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	bool bReturnVal = false;
	SURFACEDESC sdOSP;

	if (FAILED((hresult = pOffscreenPlainSurface->GetDesc(&sdOSP))))
		WriteToLog("CSurface::GetDesc failed in CompareOffscreenPlainToTexture.\n");
	else
	{
		CTexture *pTemporarySysMemTexture = NULL;

		if (FAILED((hresult = m_pSrcDevice->CreateTexture(sdOSP.Width, sdOSP.Height, 1, 0, sdOSP.Format, POOL_SYSTEMMEM, &pTemporarySysMemTexture))))
			WriteToLog("Failed to create the temporary system memory texture in CompareOffscreenPlainToTexture.\n");
		else
		{
			CTexture *pTemporaryVidMemTexture = NULL;

			if (FAILED((hresult = m_pSrcDevice->CreateTexture(sdOSP.Width, sdOSP.Height, 1, 0, sdOSP.Format, POOL_DEFAULT, &pTemporaryVidMemTexture))))
				WriteToLog("Failed to create the temporary video memory texture in CompareOffscreenPlainToTexture.\n");
			else
			{
				CSurface *pSurfaceCopy = NULL;

				if (FAILED((hresult = pTemporarySysMemTexture->GetSurfaceLevel(0, &pSurfaceCopy))))
					WriteToLog("GetSurfaceLevel (temporary texture) failed in CompareOffscreenPlainToTexture.\n");
				else
				{
					if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(pSurfaceCopy->GetD3DSurface9(), NULL, NULL, pOffscreenPlainSurface->GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
						WriteToLog("D3DXLoadSurfaceFromSurface failed in CompareOffscreenPlainToTexture.\n");
					else
					{
						if (FAILED((hresult = m_pSrcDevice->UpdateTexture(pTemporarySysMemTexture, pTemporaryVidMemTexture))))
							WriteToLog("UpdateTexture failed in CompareOffscreenPlainToTexture.\n");
						else
							bReturnVal = CompareRenderedTextures(pManagedTexture, pTemporaryVidMemTexture, pfSurfaceConformanceFraction);
					}

					pSurfaceCopy->Release();
				}

				pTemporaryVidMemTexture->Release();
			}

			pTemporarySysMemTexture->Release();
		}
	}

	return bReturnVal;

	/*
	HRESULT hresult;
	bool bReturnVal = false;
	CSurface *pTextureSurface = NULL;

	if (FAILED((hresult = pManagedTexture->GetSurfaceLevel(0, &pTextureSurface))))
		WriteToLog("GetSurfaceLevel failed in CompareOffscreenPlainToTexture.\n");
	else
	{
		if (FAILED((TestApp.m_ImageCmp).CompareImages(pOffscreenPlainSurface, pTextureSurface, NULL)))
			WriteToLog("Unable to compare the test and reference images (CompareOffscreenPlainToTexture).\n");
		else
		{
			*pfSurfaceConformanceFraction = (TestApp.m_ImageCmp).GetCompareResult();
			bReturnVal = true;
		}

		pTextureSurface->Release();
	}

	return bReturnVal;
	*/
}

bool CUpdateSurfaceTestMain::CompareRTToTexture(CTexture *pManagedTexture, CSurface *pRTSurface, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	bool bReturnVal = false;
	SURFACEDESC sdRT;

	if (FAILED((hresult = pRTSurface->GetDesc(&sdRT))))
		WriteToLog("CSurface::GetDesc failed in CompareRTToTexture.\n");
	else
	{
		CTexture *pTemporarySysMemTexture = NULL;

		if (FAILED((hresult = m_pSrcDevice->CreateTexture(sdRT.Width, sdRT.Height, 1, 0, sdRT.Format, POOL_SYSTEMMEM, &pTemporarySysMemTexture))))
			WriteToLog("Failed to create the temporary system memory texture in CompareRTToTexture.\n");
		else
		{
			CTexture *pTemporaryVidMemTexture = NULL;

			if (FAILED((hresult = m_pSrcDevice->CreateTexture(sdRT.Width, sdRT.Height, 1, 0, sdRT.Format, POOL_DEFAULT, &pTemporaryVidMemTexture))))
				WriteToLog("Failed to create the temporary video memory texture in CompareRTToTexture.\n");
			else
			{
				CSurface *pSurfaceCopy = NULL;

				if (FAILED((hresult = pTemporarySysMemTexture->GetSurfaceLevel(0, &pSurfaceCopy))))
					WriteToLog("GetSurfaceLevel (temporary texture) failed in CompareRTToTexture.\n");
				else
				{
					if (FAILED((hresult = m_pSrcDevice->GetRenderTargetData(pRTSurface, pSurfaceCopy))))
						WriteToLog("GetRenderTargetData failed in CompareRTToTexture.\n");
					else
					{
						if (FAILED((hresult = m_pSrcDevice->UpdateTexture(pTemporarySysMemTexture, pTemporaryVidMemTexture))))
							WriteToLog("UpdateTexture failed in CompareRTToTexture.\n");
						else
							bReturnVal = CompareRenderedTextures(pManagedTexture, pTemporaryVidMemTexture, pfSurfaceConformanceFraction);
					}

					pSurfaceCopy->Release();
				}

				pTemporaryVidMemTexture->Release();
			}

			pTemporarySysMemTexture->Release();
		}
	}

	return bReturnVal;
}

// Override this to avoid incurring failures when a failing return code is actually expected.
void CUpdateSurfaceTestMain::ProcessError(HRESULT hr)
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
