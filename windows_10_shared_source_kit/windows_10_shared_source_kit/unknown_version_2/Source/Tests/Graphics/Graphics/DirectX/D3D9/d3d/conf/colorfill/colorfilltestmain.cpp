#include "CD3DWindowFramework.h"
#include "ColorFillTestMain.h"

#include <stdio.h>
#include <d3dx9.h>

#include "resource.h"

#include "dxfmt.h"
#include "dxpool.h"
#include "dxusage.h"
#include "dxmultisample.h"
#include "dims.h"
#include "rects.h"
#include "colors.h"

#include "auxiliary.h"

extern CD3DWindowFramework TestApp;

const float CColorFillTestMain::fMinimumConformanceFraction = 1.00f;

CColorFillTestMain::BASICTEXTUREVERTEX CColorFillTestMain::QuadVertices[] =
{
	{-1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f}
};
CONST DWORD CColorFillTestMain::dwD3DFVF_VERTEX = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

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
// clsFormatCasesList methods

FormatCases* clsFormatCasesList::AddNewElement()
{
	FormatCasesListElement *pNewElement;

	if (!(pNewElement = new FormatCasesListElement))
		return NULL;

	ZeroMemory((PVOID)pNewElement, (SIZE_T)(sizeof(FormatCasesListElement)));

	if (pFirst)
		pLast = (pLast->pNext = pNewElement);
	else
		pLast = (pFirst = pNewElement);

	return &(pNewElement->FormatCasesData);
}

void clsFormatCasesList::DeleteAll()
{
	FormatCasesListElement *pCurrentElement, *pNextElement;

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

/*
////////////////////////////////////////////////////////////////////////////////
// clsSourceSurfaceSet methods

void clsSourceSurfaceSet::ReleaseAllSurfaces()
{
	texsurfSourceLoader.ReleaseDXObjects();

	if (pSourceTestSurface)
	{
		delete pSourceTestSurface;
		pSourceTestSurface = NULL;
	}

	bInitialized = false;
	dwMainSurfaceSpec = 0;
}

bool clsSourceSurfaceSet::PrepareFromDWORD(CColorFillTestMain *pTestEnv, DWORD dwSurfaceSpec)
{
	if (bInitialized)
		if (dwMainSurfaceSpec == dwSurfaceSpec)
			return true;
		else
			ReleaseAllSurfaces();

	if (!(pSourceTestSurface = pTestEnv->CreateSurfaceFromDWORD(dwSurfaceSpec)))
		pTestEnv->WriteToLog("Unable to create source surface.");
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

		if (!pTestEnv->InitializeCompatibleSMSurface(sdSourceSurfaceDesc, &texsurfSourceLoader))
			pTestEnv->WriteToLog("Unable to create a system memory surface compatible with the source surface.");
		else
		{
			CSurface *lpd3dsurfSourceLoader;
			HRESULT hresult;
			D3DXIMAGE_INFO d3dxii;

			// Retrieve a pointer to the "loader surface".
			lpd3dsurfSourceLoader = texsurfSourceLoader.GetSurfacePointer();

			// Load the "foreground" image onto the surface.
			if
			(
				FAILED
				(
					(
						hresult =
						(pTestEnv->m_pSrcDevice) ->
						D3DXLoadSurfaceFromFileInMemory
						(
							lpd3dsurfSourceLoader,
							NULL,
							NULL,
							pTestEnv->BitmapDataForeground,
							((PBITMAPFILEHEADER)(pTestEnv->BitmapDataForeground))->bfSize,
							NULL,
							D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
							0,
							&d3dxii
						)
					)
				)
			)
				pTestEnv->WriteToLog("Unable to load the source-compatible system memory surface with the 'foreground image'.");
			else
			{
				// Copy the contents of the loader surface onto
				// the source surface.
				if
				(
					FAILED
					(
						(
							hresult =
							(pTestEnv->m_pSrcDevice)->CopyRects
							(
								lpd3dsurfSourceLoader,
								NULL,
								0,
								lpd3dsurfSource,
								NULL
							)
						)
					)
				)
					pTestEnv->WriteToLog("Unable to copy the 'foreground image' onto the source surface.");
				else
				{
					bInitialized = true;
					dwMainSurfaceSpec = dwSurfaceSpec;
					return true;
				}
			}

			texsurfSourceLoader.ReleaseDXObjects();
		}

		delete pSourceTestSurface;
		pSourceTestSurface = NULL;
	}

	return false;
}
*/

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

bool clsDestSurfaceSet::PrepareFromDWORD(CColorFillTestMain *pTestEnv, DWORD dwSurfaceSpec)
{
	if (bInitialized)
		if (dwMainSurfaceSpec == dwSurfaceSpec)
			return true;
		else
			ReleaseAllSurfaces();

	if (!(pDestTestSurface = pTestEnv->CreateSurfaceFromDWORD(dwSurfaceSpec)))
		pTestEnv->WriteToLog("Unable to create destination surface.");
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

		// Create three system memory surfaces compatible with the
		// destination surface:
		//
		// texsurfDestLoader -	used to load the destination surface
		//						with a "background image";
		// texsurfReference -	the destination surface for the
		//						reference copy;
		// texsurfDestCopy -	used to obtain a lockable copy of
		//						the destination which can be compared
		//						with the reference copy.

		if
		(
			!pTestEnv->InitializeCompatibleSMSurface(sdDestSurfaceDesc, &texsurfDestLoader) ||
			!pTestEnv->InitializeCompatibleMMSurface(sdDestSurfaceDesc, &texsurfReference) ||
			!pTestEnv->InitializeCompatibleSMSurface(sdDestSurfaceDesc, &texsurfDestCopy)
		)
			pTestEnv->WriteToLog("Unable to create all of the destination-compatible auxiliary surfaces.");
		else
		{
			CSurface *lpd3dsurfDestLoader;
			HRESULT hresult;
			D3DXIMAGE_INFO d3dxii;

			// Retrieve a pointer to the "loader surface".
			lpd3dsurfDestLoader = texsurfDestLoader.GetSurfacePointer();

			// Load the "background" image onto the loader surface.
			if
			(
				FAILED
				(
					(
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
						)
					)
				)
			)
				pTestEnv->WriteToLog("Unable to load the destination-compatible 'loader' surface with the 'background image'.");
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
// CColorFillTestMain methods

CColorFillTestMain::CColorFillTestMain()
{
	SetTestType(TESTTYPE_CONF);

	m_szCommandKey= _T("ColorFillTest");
	m_szLogName = _T("ColorFillTest");
	m_szTestName= _T("ColorFillTest");

	m_Options.D3DOptions.fMinDXVersion = 9.f;
	//m_Options.D3DOptions.fMaxDXVersion = 9.f;
	m_Options.TestOptions.bCompareSrcRef = false;
	m_Options.D3DOptions.bZBuffer		 = false;

	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;

	//m_Options.TestOptions.fPassRequired = 0.97f;

	BitmapDataBackground = NULL;
	ValidSurfacesByFormat = NULL;

	pvbAuxiliaryVertexBuffer = NULL;
	//psurfAuxiliaryRenderTarget = NULL;
	psurfAuxiliaryRenderTargetTest = NULL;
	psurfAuxiliaryRenderTargetRef = NULL;
}

CColorFillTestMain::~CColorFillTestMain()
{
	if (ValidSurfacesByFormat)
		delete [] ValidSurfacesByFormat;
}

/*
bool CColorFillTestMain::InitializeAuxiliaryResources()
{
	HRESULT hresult;

	// AndrewLu: Create a "global", lockable render target for MyLoadSurfaceFromTexture.
	if (FAILED((hresult = m_pSrcDevice->CreateRenderTarget(cuiGlobalRTWidth, cuiGlobalRTHeight, sdSrcBackBuffer.Format, MULTISAMPLE_NONE, TRUE, &psurfAuxiliaryRenderTarget))))
		WriteToLog(_T("CreateRenderTarget failed in CStretchRectTestMain::InitializeAuxiliaryResources.\n"));
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

		psurfAuxiliaryRenderTarget->Release();
		psurfAuxiliaryRenderTarget = NULL;
	}

	return false;
}
*/

bool CColorFillTestMain::InitializeAuxiliaryResources()
{
	HRESULT hresult;

	FORMAT fmtRT(FMT_X8R8G8B8);

	// AndrewLu: Create a "global", lockable render target for rendering test surfaces.
	if (FAILED((hresult = m_pSrcDevice->CreateRenderTarget(cuiGlobalRTWidth, cuiGlobalRTHeight, /*sdSrcBackBuffer.Format*/fmtRT, MULTISAMPLE_NONE, TRUE, &psurfAuxiliaryRenderTargetTest))))
		WriteToLog(_T("CreateRenderTarget failed in InitializeAuxiliaryResources.\n"));
	else
	{
		// AndrewLu: Create a "global", lockable render target for rendering reference surfaces.
		if (FAILED((hresult = m_pSrcDevice->CreateRenderTarget(cuiGlobalRTWidth, cuiGlobalRTHeight, /*sdSrcBackBuffer.Format*/fmtRT, MULTISAMPLE_NONE, TRUE, &psurfAuxiliaryRenderTargetRef))))
			WriteToLog(_T("CreateRenderTarget failed in InitializeAuxiliaryResources.\n"));
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
				WriteToLog(_T("CreateVertexBuffer failed in InitializeAuxiliaryResources.\n"));
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
					WriteToLog(_T("Unable to lock vertex buffer in InitializeAuxiliaryResources.\n"));
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

/*
void CColorFillTestMain::ClearAuxiliaryResources()
{
	if (pvbAuxiliaryVertexBuffer)
	{
		pvbAuxiliaryVertexBuffer->Release();
		pvbAuxiliaryVertexBuffer = NULL;
	}

	if (psurfAuxiliaryRenderTarget)
	{
		psurfAuxiliaryRenderTarget->Release();
		psurfAuxiliaryRenderTarget = NULL;
	}
}
*/

void CColorFillTestMain::ClearAuxiliaryResources()
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
}

void CColorFillTestMain::FindValidTextureSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
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
			DWORD dwSurfaceSpec;

			dwSurfaceSpec =
			((DWORD)2 << 28) | // Surface type == 2; denotes texture surface.
			((DWORD)uiFormatIndex << 16) |
			(DWORD)uiUsageSetIndex;
			// Note: Bits 8 to 15 of the surface spec contain a pool type index of zero,
			// denoting D3DPOOL_DEFAULT

			// DanielRo: we have to reject the textures without the RENDERTARGET usage (ColorFill doesn't support them)
			if(D3DUSAGE_RENDERTARGET & dwUsageSets[uiUsageSetIndex])
				pSpecList->AddSurfaceSpec(dwSurfaceSpec);
			//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
			//pSpecList->AddSurfaceSpec(dwSurfaceSpec);
		}
	}
}

void CColorFillTestMain::FindValidOffscreenPlainSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
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
		((DWORD)4 << 28) | // Surface type == 4; denotes offscreen plain surface.
		((DWORD)uiFormatIndex << 16);
		// Note: Bits 8 to 15 of the surface spec contain a pool type index of zero,
		// denoting D3DPOOL_DEFAULT

		pSpecList->AddSurfaceSpec(dwSurfaceSpec);
		//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
		//pSpecList->AddSurfaceSpec(dwSurfaceSpec);
	}
}

void CColorFillTestMain::FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
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
			}
		}

	}
}

/*
void CColorFillTestMain::FindValidTextureSpecs(clsSurfaceSpecList *pSpecList)
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
void CColorFillTestMain::FindValidSourceSurfaceSpecs() {}
void CColorFillTestMain::FindValidDestSurfaceSpecs() {}
*/

/*
void CColorFillTestMain::FindValidSurfaceSpecs()
{
	clsSurfaceSpecList SurfaceSpecList;

	// Based on the format of the source device adapter's current mode and
	// the capabilities of the device, populate the surface specification list.
	FindValidTextureSpecs(&SurfaceSpecList);

	// Use the list to initialize the surface specification array.
	// SurfaceSpecList.InitializeSurfaceSpecArray(&SurfaceSpecArray);
}
*/

/*
void CColorFillTestMain::FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray)
{
	clsSurfaceSpecList SourceSpecList;

	// Build the list.
	FindValidOffscreenPlainSpecs(uiFormatIndex, &SourceSpecList);
	FindValidRTSpecs(uiFormatIndex, &SourceSpecList);

	// Initialize the array.
	SourceSpecList.InitializeSurfaceSpecArray(pSourceSpecArray);
}
*/

void CColorFillTestMain::FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray)
{
	clsSurfaceSpecList DestSpecList;

	// Build the list.
	FindValidOffscreenPlainSpecs(uiFormatIndex, &DestSpecList);
	FindValidRTSpecs(uiFormatIndex, &DestSpecList);
	FindValidTextureSpecs(uiFormatIndex, &DestSpecList);

	// Initialize the array.
	DestSpecList.InitializeSurfaceSpecArray(pDestSpecArray);
}

UINT CColorFillTestMain::TestInitialize(void)
{
	HRESULT hresult;
	UINT uiTotalTestCases = 0;
	CSurface *psurfSrcBackBuffer;

	// Initialization steps that are performed only once per test execution:

	InitializeSurfaceTypeClasses();

	// Ensure that the bitmap resources are available.
	if (!BitmapDataBackground)
	{
		HRSRC hrsrcFindResourceResult;
		HGLOBAL hgBitmapData;

		hrsrcFindResourceResult = FindResource(TestApp.m_hInstance, MAKEINTRESOURCE(IDR_DIBFILETYPE2),
		"DIBFILETYPE");
		hgBitmapData = LoadResource(TestApp.m_hInstance, hrsrcFindResourceResult);
		BitmapDataBackground = LockResource(hgBitmapData);
	}

	// Ensure availability of an array whose elements will specify valid destination
	// surface specifications on a "per format" basis.
	if (!ValidSurfacesByFormat)
		ValidSurfacesByFormat = new clsSurfaceSpecArray[cuiNoOfFormats];

	// Initialization steps that are performed once per TestInitialize / TestTerminate pair:

	// Obtain a pointer to a surface representing a back buffer.
	if (SUCCEEDED((hresult = m_pSrcDevice->/*GetBackBuffer8*/GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &psurfSrcBackBuffer))))
	{
		//SURFACEDESC sdSrcBackBuffer;

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

			/*
			rectReferenceDisplay.left = (uiPanelWidth >> 1) - (uiDisplayEdgeLength >> 1);
			rectReferenceDisplay.top = (uiPanelHeight >> 1) - (uiDisplayEdgeLength >> 1);
			rectReferenceDisplay.right = rectReferenceDisplay.left + uiDisplayEdgeLength;
			rectReferenceDisplay.bottom = rectReferenceDisplay.top + uiDisplayEdgeLength;
			*/

			/*
			WriteToLog(_T("del: %d\n"), uiDisplayEdgeLength);

			WriteToLog(_T("w: %d; h: %d; l: %d; t: %d; r: %d; b: %d\n"),
			sdSrcBackBuffer.Width, sdSrcBackBuffer.Height, rectReferenceDisplay.left,
			rectReferenceDisplay.top, rectReferenceDisplay.right, rectReferenceDisplay.bottom);
			*/

			/*
			rectTestDisplay.left = ((sdSrcBackBuffer.Width + 1) >> 1) + (uiPanelWidth >> 1) - (uiDisplayEdgeLength >> 1);
			rectTestDisplay.top = rectReferenceDisplay.top;
			rectTestDisplay.right = rectTestDisplay.left + uiDisplayEdgeLength;
			rectTestDisplay.bottom = rectReferenceDisplay.bottom;
			*/
		}
	}

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

	// For each format, determine the set of valid destination surface specs.
	{
		UINT uiFormatIndex;

		for (uiFormatIndex = 0; uiFormatIndex < cuiNoOfFormats; ++uiFormatIndex)
		{
			//FindValidSourceSurfaceSpecs(uiFormatIndex, &((ValidSurfacesByFormat[uiFormatIndex]).ValidSourceSpecs));
			FindValidDestSurfaceSpecs(uiFormatIndex, &(ValidSurfacesByFormat[uiFormatIndex]));
		}
	}

	// Compute and record the number of test cases per format.
	{
		UINT uiDestFormatIndex;

		for (uiDestFormatIndex = 0; uiDestFormatIndex < cuiNoOfFormats; ++uiDestFormatIndex)
		{
			UINT uiNoOfDestSpecs;

			if (uiNoOfDestSpecs = (ValidSurfacesByFormat[uiDestFormatIndex]).GetNumberOfElements())
			{
				UINT uiNoOfTestCases;
				FormatCases *pNewFormatElement;

				uiNoOfTestCases =
				uiNoOfDestSpecs *
				cuiNoOfDestRects *
				cuiNoOfColors;

				// Record the destination format and the associated number of test cases.
				pNewFormatElement = TestCasesByFormat.AddNewElement();
				pNewFormatElement->uiFormatIndex = uiDestFormatIndex;
				pNewFormatElement->uiNoOfTestCases = uiNoOfTestCases;

				uiTotalTestCases += uiNoOfTestCases;
			}
		}
	}

	/*
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

						if (SUCCEEDED((hresult = m_pD3D->CheckDeviceFormatConversion(uiSrcDeviceAdapterOrdinal, dwSrcDeviceType, fmtSource, fmtDest))))
						{
							UINT uiNoOfTestCases;
							FormatPair *pNewFormatPairElement;

							uiNoOfTestCases =
							uiNoOfSourceSpecs *
							uiNoOfDestSpecs *
							cuiSourceLeftVals *
							cuiSourceTopVals *
							cuiSourceWidthVals *
							cuiSourceHeightVals *
							cuiDestLeftVals *
							cuiDestTopVals *
							cuiDestWidthVals *
							cuiDestHeightVals *
							uiNoOfValidFilterRecordIndices;

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
	*/

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

bool CColorFillTestMain::ExecuteTest(UINT uiTestCaseId)
{
	UINT uiFormatSpecificId = uiTestCaseId - 1;
	clsFormatCasesListScanner FormatListScanner;
	FormatCases *pCurrentFormat;
	UINT
	uiNoOfTestCases, uiNoOfDestSurfaceSpecs,
	uiColorIndex,
	uiDestRectIndex,
	uiDestSpecIndex;
	RECT rectDest;
	clsSurfaceSpecArray *pDestSurfaceSet;
	//float fSurfaceMinimumConformanceFraction;

	// Determine the format and the offset of the test case within the set of test cases
	// associated with the format.
	FormatListScanner.Reset(&TestCasesByFormat);
	if (!(pCurrentFormat = FormatListScanner.GetCurrent()))
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}
	uiNoOfTestCases = pCurrentFormat->uiNoOfTestCases;
	if (uiFormatSpecificId >= uiNoOfTestCases)
		do
		{
			FormatListScanner.Advance();
			if (!(pCurrentFormat = FormatListScanner.GetCurrent()))
			{
				ReportInvalidTCID(uiTestCaseId);
				return false;
			}
			uiFormatSpecificId -= uiNoOfTestCases;

			uiNoOfTestCases = pCurrentFormat->uiNoOfTestCases;
		}
		while (uiFormatSpecificId >= uiNoOfTestCases);

	// Based on the destination format identified by *pCurrentFormat, determine the
	// set of destination specifications.
	//pSourceSurfaceSet = &((ValidSurfacesByFormat[pCurrentFormat->uiSourceFormatIndex]).ValidSourceSpecs);
	//uiNoOfSourceSurfaceSpecs = pSourceSurfaceSet->GetNumberOfElements();
	pDestSurfaceSet = &(ValidSurfacesByFormat[pCurrentFormat->uiFormatIndex]);
	uiNoOfDestSurfaceSpecs = pDestSurfaceSet->GetNumberOfElements();

	// Derive the test case attributes---each an array index---from the test case descriptor.
	uiColorIndex = uiFormatSpecificId % cuiNoOfColors;
	uiFormatSpecificId /= cuiNoOfColors;

	uiDestRectIndex = uiFormatSpecificId % cuiNoOfDestRects;

	if ((uiDestSpecIndex = uiFormatSpecificId / cuiNoOfDestRects) >= uiNoOfDestSurfaceSpecs)
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}

	// Record commencement of a test case.
	BeginTestCase("Test Case", uiTestCaseId);

	// Prepare the destination surface set.
	
	if (!DestSurfaceSet.PrepareFromDWORD(this, pDestSurfaceSet->GetElementValue(uiDestSpecIndex)))
	{
		WriteToLog(_T("The test was unable to prepare the set of destination surfaces.\n"));
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiTestCaseId);
		return false;
	}

	// Compute the destination rectangle for the ColorFill operation.

	rectDest.left = (DestSurfaceSet.sdDestSurfaceDesc).dwWidth *
	(DestRects[uiDestRectIndex]).iLeftNumerator /
	(DestRects[uiDestRectIndex]).iLeftDenominator +
	(DestRects[uiDestRectIndex]).iLeftConstant;

	rectDest.top = (DestSurfaceSet.sdDestSurfaceDesc).dwHeight *
	(DestRects[uiDestRectIndex]).iTopNumerator /
	(DestRects[uiDestRectIndex]).iTopDenominator +
	(DestRects[uiDestRectIndex]).iTopConstant;

	rectDest.right = (DestSurfaceSet.sdDestSurfaceDesc).dwWidth *
	(DestRects[uiDestRectIndex]).iRightNumerator /
	(DestRects[uiDestRectIndex]).iRightDenominator +
	(DestRects[uiDestRectIndex]).iRightConstant;

	rectDest.bottom = (DestSurfaceSet.sdDestSurfaceDesc).dwHeight *
	(DestRects[uiDestRectIndex]).iBottomNumerator /
	(DestRects[uiDestRectIndex]).iBottomDenominator +
	(DestRects[uiDestRectIndex]).iBottomConstant;

	// If necessary, adjust the destination rectangle in accordance with DXTn/UYVY constraints.
	if (TestDXTn((DestSurfaceSet.sdDestSurfaceDesc).Format))
		AdjustRectForDXTn(&rectDest, (DestSurfaceSet.sdDestSurfaceDesc).dwWidth,
		(DestSurfaceSet.sdDestSurfaceDesc).dwHeight);
	else
		if (TestUYVY((DestSurfaceSet.sdDestSurfaceDesc).Format))
			AdjustRectForUYVY(&rectDest, (DestSurfaceSet.sdDestSurfaceDesc).dwWidth,
			(DestSurfaceSet.sdDestSurfaceDesc).dwHeight);

	/*
	// Compute the fractional conformance expected of the entire destination surface.
	fSurfaceMinimumConformanceFraction =
	1.0f -
	(
		(1.0f - fMinimumConformanceFraction) *
		(float)((rectDest.right - rectDest.left) * (rectDest.bottom - rectDest.top)) /
		(float)(DestSurfaceSet.GetNoOfSurfacePixels())
	);
	*/

	if (!TestColorFillOperation(rectDest, uiColorIndex, fMinimumConformanceFraction))
	{
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiTestCaseId);
		return false;
	}

	return true;
}

bool CColorFillTestMain::TestTerminate(void)
{
	//SourceSurfaceSet.ReleaseAllSurfaces();
	DestSurfaceSet.ReleaseAllSurfaces();
	TestCasesByFormat.DeleteAll();
	ClearAuxiliaryResources();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// "Wrapped Surface Client" methods

HRESULT CColorFillTestMain::WSCCreateTexture(UINT uiWidth, UINT uiHeight, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CTexture **ppTexture)
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

HRESULT CColorFillTestMain::WSCCreateOffscreenPlainSurface(UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool, CSurface **ppSurface)
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

HRESULT CColorFillTestMain::WSCCreateRenderTarget(UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE MultiSample, DWORD dwMultisampleQuality, BOOL bLockable, CSurface **ppSurface)
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

HRESULT CColorFillTestMain::WSCCreateAdditionalSwapChain(PRESENT_PARAMETERS* pPresentationParameters, CSwapChain** ppSwapChain)
{
	HRESULT hresult;

	if (FAILED((hresult = m_pSrcDevice->CreateAdditionalSwapChain(pPresentationParameters, ppSwapChain))))
		WriteToLog("CreateAdditionalSwapChain failed.\n");

	return hresult;
}

////////////////////////////////////////////////////////////////////////////////
// Surface Specification Interpretation

clsTestSurface *CColorFillTestMain::CreateSurfaceFromDWORD(DWORD dwSpec)
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
					clsTextureSurface *ptexsurfNew = NULL;
					UINT uiUsageIndex, uiPoolTypeIndex;

					if ((uiUsageIndex = (UINT)ExtractBitsRange(dwSpec, 0, 7)) < cuiNoOfUsageSets)
						if ((uiPoolTypeIndex = (UINT)ExtractBitsRange(dwSpec, 8, 15)) < cuiNoOfPoolTypes)
							if (ptexsurfNew = new clsTextureSurface)
								if
								(
									ptexsurfNew->Initialize
									(
										(clsWrappedSurfaceClient *)this,
										SurfaceDimensions[uiDimensionIndex].uiWidth,
										SurfaceDimensions[uiDimensionIndex].uiHeight,
										DXGraphicsFormats[uiFormatIndex].fmtFW,
										dwUsageSets[uiUsageIndex],
										DXPoolTypes[uiPoolTypeIndex].dwFWPool
									)
								)
									ptestsurfNew = ptexsurfNew;
								else
									delete ptexsurfNew;
				}
				break;
			case 3:
			// Cubemap surface
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

	return ptestsurfNew;
}

bool CColorFillTestMain::InitializeCompatibleSMSurface(SURFACEDESC sdDescription, clsTextureSurface *pSystemMemSurface)
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
			POOL_SYSTEMMEM
		)
	)
		return true;

	return false;
}

bool CColorFillTestMain::InitializeCompatibleMMSurface(SURFACEDESC sdDescription, clsTextureSurface *pManagedMemSurface)
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
			POOL_MANAGED
		)
	)
		return true;

	return false;
}

bool CColorFillTestMain::TestColorFillOperation(RECT rectDest, UINT uiColorIndex, float fSurfaceMinimumConformanceFraction)
{
	HRESULT hresult;

	CSurface *lpd3dsurfDest, *lpd3dsurfDestLoader, *lpd3dsurfDestRef, *lpd3dsurfDestCopy;

	// Obtain pointers to the surface interfaces.
	lpd3dsurfDest = (DestSurfaceSet.pDestTestSurface)->GetSurfacePointer();
	lpd3dsurfDestLoader = (DestSurfaceSet.texsurfDestLoader).GetSurfacePointer();
	lpd3dsurfDestRef = (DestSurfaceSet.texsurfReference).GetSurfacePointer();
	lpd3dsurfDestCopy = (DestSurfaceSet.texsurfDestCopy).GetSurfacePointer();

	// Load the background image onto the destination surface.
	/*
	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice->CopyRects
				(
					lpd3dsurfDestLoader,
					NULL,
					0,
					lpd3dsurfDest,
					NULL
				)
			)
		)
	)
	*/
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
		WriteToLog("Unable to copy the 'background image' onto the destination surface.");
		return false;
	}

	// Clear the surface to some random color
	if( FAILED( hresult = m_pSrcDevice->ColorFill( lpd3dsurfDest, &rectDest, 0xdeadbeef ) ) )
	{
		WriteToLog("ColorFill (clear surface) operation failed.");
		return false;
	}
	//m_pSrcDevice->D3DXSaveSurfaceToFile( "lpd3dsurfDest.dds", D3DXIFF_DDS, lpd3dsurfDest, NULL, NULL );

	// Perform the tested ColorFill operation.
	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice->ColorFill
				(
					lpd3dsurfDest,
					&rectDest,
					dwColors[uiColorIndex]
				)
			)
		)
	)
	{
		WriteToLog("The tested ColorFill operation failed.");
		Fail();
	}
	else
	{
		if (FAILED((hresult = /*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(lpd3dsurfDestCopy->GetD3DSurface9(), NULL, NULL, lpd3dsurfDest->GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
		{
			WriteToLog("Unable to retrieve the result of the tested copy for comparison with the reference copy.\n");
			return false;
		}

		/*if (!((DestSurfaceSet.pDestTestSurface)->DumpContentToSysMem(this, lpd3dsurfDestCopy)))
		{
			WriteToLog("Unable to retrieve the result of the tested copy for comparison with the reference copy.");
			return false;
		}*/

		{
			float fCompareResult;

			if (FAILED((hresult = /*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(lpd3dsurfDestRef->GetD3DSurface9(), NULL, NULL, lpd3dsurfDestLoader->GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
			{
				WriteToLog("Unable to load the destination surface for the reference copy.\n");
				return false;
			}

			// Perform the reference copy.
			if
			(
				!ReferenceColorFill(lpd3dsurfDestRef, &rectDest, dwColors[uiColorIndex])
			)
			{
				WriteToLog("Unable to perform the reference ColorFill operation.\n");
				return false;
			}

			if (!((DestSurfaceSet.pDestTestSurface)->VerifyAgainstSysMemTexture(this, (DestSurfaceSet.texsurfReference).GetTexturePointer(), &fCompareResult)))
			{
				WriteToLog("Unable to verify the result of the tested copy.\n");
				return false;
			}

			//::D3DXSaveSurfaceToFile( "lpd3dsurfDestCopy.dds", D3DXIFF_DDS, lpd3dsurfDestCopy->GetD3DSurface9(), NULL, NULL );
			//::D3DXSaveSurfaceToFile( "lpd3dsurfDestRef.dds", D3DXIFF_DDS, lpd3dsurfDestRef->GetD3DSurface9(), NULL, NULL );
			// Compare the buffers
			/*
			if (FAILED((TestApp.m_ImageCmp).CompareImages(lpd3dsurfDestCopy, lpd3dsurfDestRef, NULL)))
			{
				WriteToLog("Unable to compare the test and reference images.\n");
					return false;
			}
			*/

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
			else WriteToLog("Minimum Conformance Expected: %f; Actual Conformance: %f\n", fSurfaceMinimumConformanceFraction, fCompareResult);
		}

		WriteToLog("The reference and test images differ significantly.");
		Fail();
	}

	// The tested ColorFill operation failed or generated an incorrect result.  Report the
	// details of the surface, rectangle, and color involved.
	WriteToLog("\n");
	WriteToLog("Destination: %s\n", DestSurfaceSet.szDescription);
	WriteToLog("Destination Rectangle: {%d, %d, %d, %d}\n", rectDest.left, rectDest.top, rectDest.right, rectDest.bottom);
	WriteToLog("Color Value: %d\n", dwColors[uiColorIndex]);
	/*
	sprintf(szLogLine, "Rectangle set: %d (%s)", uiRectSetIndex, RectangleSets[uiRectSetIndex].szDescription);
	WriteToLog(szLogLine);
	*/

	return true;

	/*
	else
		else
		{


			// Compare the result of the tested CopyRects operation with that of the reference
			// copy operation.
			if (!CompareSurfaces(lpd3dsurf8DestCopy, lpd3dsurf8DestRef))
			{
				WriteToLog("");
				WriteToLog("Discrepancy between CopyRects result and reference copy result.");
				sprintf(szLogLine, "Source: %s", pSourceSurfaceSet->szDescription);
				WriteToLog(szLogLine);
				sprintf(szLogLine, "Destination: %s", pDestSurfaceSet->szDescription);
				WriteToLog(szLogLine);
				sprintf(szLogLine, "Rectangle set: %d (%s)", uiRectSetIndex, RectangleSets[uiRectSetIndex].szDescription);
				WriteToLog(szLogLine);

				sprintf(szLogLine, "Repro: Execute 'copyrects -M%d -T%d' in a %dbpp desktop mode.",
				uiTestedAdapterOrdinal + 1, (UINT)dwTestCaseID, (pd3dfmtrecDesktop->uiBytesPerPixel) * 8);
				WriteToLog(szLogLine);
				strcpy(szTestRunSummary, "FAIL: At least one test image differed from its corresponding reference image.");
				++uiTestCasesFailed;

				TestCaseOutcome = TCFailed;
			}
			else
			{
				//++uiTestCasesPassed;
				//WriteToLog("It really passed!\n");
				TestCaseOutcome = TCPassed;
			}

			//ShowSurfaces(lpd3dsurf8DestRef, lpd3dsurf8DestCopy);
		}
	*/
}

bool CColorFillTestMain::ReferenceColorFill(CSurface *pSurface, CONST RECT *pRect, DWORD color)
{
	HRESULT hresult;
	bool bResult = false;
	CSurface *p32bppColorSurface = NULL;

	if (SUCCEEDED((hresult = WSCCreateOffscreenPlainSurface(32, 32, (FMT)D3DFMT_A8R8G8B8, POOL_SYSTEMMEM, &p32bppColorSurface))))
	{
		LOCKED_RECT lrColorSurface;

		if (SUCCEEDED((hresult = p32bppColorSurface->LockRect(&lrColorSurface, NULL, 0))))
		{
			UINT uiRowsRemaining;
			BYTE *pRowLeft = (BYTE *)(lrColorSurface.pBits);

			for (uiRowsRemaining = 32; uiRowsRemaining; --uiRowsRemaining)
			{
				UINT uiRowIndex;

				for (uiRowIndex = 0; uiRowIndex < 32; ++uiRowIndex)
				{
					((DWORD*)pRowLeft)[uiRowIndex] = color;
				}

				pRowLeft += lrColorSurface.iPitch;
			}

			p32bppColorSurface->UnlockRect();

			if
			(
				SUCCEEDED
				(
					(
						hresult =
						m_pSrcDevice->D3DXLoadSurfaceFromSurface
						(
							pSurface,
							NULL,
							pRect,
							p32bppColorSurface,
							NULL,
							NULL,
							D3DX_FILTER_LINEAR,
							0
						)
					)
				)
			)
				bResult = true;
		}

		p32bppColorSurface->Release();
	}

	return bResult;
}

//virtual HRESULT     D3DXLoadSurfaceFromSurface9(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);

bool CColorFillTestMain::DisplayFrame(void)
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

/*
HRESULT CColorFillTestMain::MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture)
{
	HRESULT hresult;
	SURFACEDESC sdSysmemSurface;

	// Obtain the dimensions of the system memory surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pSysmemSurface->GetDesc(&sdSysmemSurface))))
		WriteToLog(_T("GetDesc failed in UpdateTex::MyLoadSurfaceFromTexture.\n"));
	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;

		// Set the stream source and vertex shader.
		m_pSrcDevice->SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		//m_pSrcDevice->SetVertexShader(D3DFVF_VERTEX);
		m_pSrcDevice->SetFVF(dwD3DFVF_VERTEX);

		// Set the render target and viewport.
		m_pSrcDevice->SetRenderTarget(psurfAuxiliaryRenderTarget, NULL);
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

		// Copy the contents of the viewport to the system memory texture.
		rectViewport.left = vpRenderTexture.X;
		rectViewport.top = vpRenderTexture.Y;
		rectViewport.right = rectViewport.left + vpRenderTexture.Width;
		rectViewport.bottom = rectViewport.top + vpRenderTexture.Height;

		if (!pSysmemSurface)
			DebugOut.Write("pSysmemSurface is NULL!");
		if (!psurfAuxiliaryRenderTarget)
			DebugOut.Write("psurfAuxiliaryRenderTarget is NULL!");

		::D3DXLoadSurfaceFromSurface(pSysmemSurface->GetD3DSurface9(), NULL, NULL, psurfAuxiliaryRenderTarget->GetD3DSurface9(), NULL, &rectViewport, FILTER_NONE, 0);
	}

	return hresult;
}
*/

HRESULT CColorFillTestMain::MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture)
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

/*
HRESULT CColorFillTestMain::MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface)
{
	HRESULT hresult;
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

	return hresult;
}
*/

HRESULT CColorFillTestMain::MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface)
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

HRESULT CColorFillTestMain::MyLoadSurfaceFromOSP(CSurface *pSysmemSurface, CSurface *pOSPSurface)
{
	HRESULT hresult;

	if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(pSysmemSurface->GetD3DSurface9(), NULL, NULL, pOSPSurface->GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
		WriteToLog(_T("D3DXLoadSurfaceFromSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromOSP.\n"));

	return hresult;
}

bool CColorFillTestMain::CompareRenderedTextures(CTexture *pReferenceTexture, CTexture *pTestTexture, float *pfSurfaceConformanceFraction)
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

bool CColorFillTestMain::CompareOffscreenPlainToTexture(CTexture *pManagedTexture, CSurface *pOffscreenPlainSurface, float *pfSurfaceConformanceFraction)
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

bool CColorFillTestMain::CompareRTToTexture(CTexture *pManagedTexture, CSurface *pRTSurface, float *pfSurfaceConformanceFraction)
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
void CColorFillTestMain::ProcessError(HRESULT hr)
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

void CColorFillTestMain::AdjustRectForDXTn(RECT *pRect, UINT uiSurfaceWidth, UINT uiSurfaceHeight)
{
	pRect->left = (UINT)((DWORD)(pRect->left) & 0xfffffffc);
	pRect->top = (UINT)((DWORD)(pRect->top) & 0xfffffffc);
	pRect->right = (UINT)((DWORD)(pRect->right + 3) & 0xfffffffc);
	if (pRect->right >= uiSurfaceWidth)
		pRect->right = uiSurfaceWidth;
	pRect->bottom = (UINT)((DWORD)(pRect->bottom + 3) & 0xfffffffc);
	if (pRect->bottom >= uiSurfaceHeight)
		pRect->bottom = uiSurfaceHeight;
}

void CColorFillTestMain::AdjustRectForUYVY(RECT *pRect, UINT uiSurfaceWidth, UINT uiSurfaceHeight)
{
	pRect->left = (UINT)((DWORD)(pRect->left) & 0xfffffffe);
	pRect->top = (UINT)((DWORD)(pRect->top) & 0xfffffffe);
	pRect->right = (UINT)((DWORD)(pRect->right + 1) & 0xfffffffe);
	if (pRect->right >= uiSurfaceWidth)
		pRect->right = uiSurfaceWidth;
	pRect->bottom = (UINT)((DWORD)(pRect->bottom + 1) & 0xfffffffe);
	if (pRect->bottom >= uiSurfaceHeight)
		pRect->bottom = uiSurfaceHeight;
}

bool CColorFillTestMain::TestDXTn(FORMAT fmtTest)
{
	D3DFORMAT d3dfmtTest = (fmtTest.d3dfFormat);

	if
	(
		(d3dfmtTest == D3DFMT_DXT1) ||
		(d3dfmtTest == D3DFMT_DXT2) ||
		(d3dfmtTest == D3DFMT_DXT3) ||
		(d3dfmtTest == D3DFMT_DXT4) ||
		(d3dfmtTest == D3DFMT_DXT5)
	)
		return true;
	else
		return false;
}

bool CColorFillTestMain::TestUYVY(FORMAT fmtTest)
{
	D3DFORMAT d3dfmtTest = (fmtTest.d3dfFormat);

	if
	(
		(d3dfmtTest == D3DFMT_UYVY) ||
		(d3dfmtTest == D3DFMT_YUY2)
	)
		return true;
	else
		return false;
}
