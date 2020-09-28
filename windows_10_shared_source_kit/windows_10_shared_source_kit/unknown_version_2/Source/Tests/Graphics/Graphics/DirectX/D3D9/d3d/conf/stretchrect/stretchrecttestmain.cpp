// SDReflect!

#include "stretchrecttest.h"
#include "StretchRectTestMain.h"

#include <stdio.h>
#include <d3dx9.h>

#include "resource.h"

#include "dxfmt.h"
#include "dxpool.h"
#include "dxusage.h"
#include "dxmultisample.h"
#include "dxfilter.h"
#include "dims.h"
#include "rects.h"

#include "auxiliary.h"

const float CStretchRectTestMain::fMinimumConformanceFraction = 0.50f;

CStretchRectTestMain::BASICTEXTUREVERTEX CStretchRectTestMain::QuadVertices[] =
{
	{-1.0f, -1.0f, 0.1f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f},
	{-1.0f, 1.0f, 0.1f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
	{1.0f, -1.0f, 0.1f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f},
	{-1.0f, -1.0f, 0.1f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f}
};
CONST DWORD CStretchRectTestMain::dwD3DFVF_VERTEX = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);

LPVOID CStretchRectTestMain::BitmapDataForeground = NULL;
LPVOID CStretchRectTestMain::BitmapDataBackground = NULL;

////////////////////////////////////////////////////////////////////////////////
// clsSurfaceSpecList methods

clsSurfaceSpecList::~clsSurfaceSpecList()
{
	SurfaceSpecListElement *pCurrentElement, *pNextElement;

	if (pCurrentElement = pFirst)
		do
		{
			pNextElement = pCurrentElement -> pNext;
			delete pCurrentElement;
		}
		while (pCurrentElement = pNextElement);
}

void clsSurfaceSpecList::AddSurfaceSpec(DWORD dwSurfaceSpec)
{
	SurfaceSpecListElement *pNewElement = new SurfaceSpecListElement;

	pNewElement -> dwSurfaceSpec = dwSurfaceSpec;
	pNewElement -> pNext = NULL;

	if (pFirst)
		pLast = (pLast -> pNext = pNewElement);
	else
		pLast = (pFirst = pNewElement);

	++uiNoOfElements;
}

void clsSurfaceSpecList::InitializeSurfaceSpecArray(clsSurfaceSpecArray *pTargetArray)
{
	if (pTargetArray -> pdwSurfaceSpecs)
	{
		delete [] (pTargetArray -> pdwSurfaceSpecs);
		pTargetArray -> pdwSurfaceSpecs = NULL;
	}
	pTargetArray -> uiNoOfElements = 0;

	if (uiNoOfElements)
	{
		DWORD *dwAllocatedMemory = NULL;
		UINT uiIndex = 0;
		SurfaceSpecListElement *pCurrentElement = pFirst;

		dwAllocatedMemory = new DWORD[uiNoOfElements];

		do
		{
			dwAllocatedMemory[uiIndex++] = pCurrentElement -> dwSurfaceSpec;
		}
		while (pCurrentElement = pCurrentElement -> pNext);

		pTargetArray -> pdwSurfaceSpecs = dwAllocatedMemory;
		pTargetArray -> uiNoOfElements = uiNoOfElements;
	}
}

////////////////////////////////////////////////////////////////////////////////
// clsSurfaceSpecArray methods

clsSurfaceSpecArray::~clsSurfaceSpecArray()
{
	Clear();
}

DWORD clsSurfaceSpecArray::GetElementValue(unsigned int uiIndex)
{
	if (uiIndex < uiNoOfElements)
		return pdwSurfaceSpecs[uiIndex];
	else
		return 0;
}

void clsSurfaceSpecArray::Clear()
{
	if (pdwSurfaceSpecs)
	{
		delete [] pdwSurfaceSpecs;
		pdwSurfaceSpecs = NULL;
	}
	uiNoOfElements = 0;
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
		pLast = (pLast -> pNext = pNewElement);
	else
		pLast = (pFirst = pNewElement);

	return &(pNewElement -> FormatPairData);
}

void clsFormatPairList::DeleteAll()
{
	FormatPairListElement *pCurrentElement, *pNextElement;

	if (pCurrentElement = pFirst)
	{
		do
		{
			pNextElement = pCurrentElement -> pNext;
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
	texsurfSourceLoader.ReleaseDXObjects();

	if (pSourceTestSurface)
	{
		delete pSourceTestSurface;
		pSourceTestSurface = NULL;
	}

	bInitialized = false;
	dwMainSurfaceSpec = 0;
}

bool clsSourceSurfaceSet::PrepareFromDWORD(CStretchRectTestMain *pTestEnv, DWORD dwSurfaceSpec)
{
	UINT uiSurfaceTypeId = (UINT)ExtractBitsRange(dwSurfaceSpec, 28, 31);

	if (bInitialized)
		if ((dwMainSurfaceSpec == dwSurfaceSpec) && (uiSurfaceTypeId != 5) && (uiSurfaceTypeId != 6))
			return true;
		// Special treatment of types 5 and 6:
		// Whenever a test surface belongs to the device swap chain, it must be initialized "from
		// scratch".  This is because the Present operation in DisplayFrame invalidates any such
		// surfaces that may have been created by a prior test case.
		else
			ReleaseAllSurfaces();

    if (!(pSourceTestSurface = pTestEnv -> CreateSurfaceFromDWORD(dwSurfaceSpec)))
	{
		pTestEnv -> WriteToLog("Unable to create source surface.");
	}
	else
	{
		CSurface *lpd3dsurfSource = NULL;

		pSourceTestSurface -> Describe(szDescription);

		// Retrieve a pointer to the DX surface interface
		// for the source surface.
		lpd3dsurfSource = pSourceTestSurface -> GetSurfacePointer();

		// Initialize a SURFACEDESC structure, based on lpd3dsurfSource, to allow convenient
		// programmatic access to the surface properties.
		lpd3dsurfSource -> GetDesc(&sdSourceSurfaceDesc);

		// The following steps initialize the source surface, using an UpdateSurface operation
		// for non-multisampled surfaces and a rendering operation for multisampled surfaces.
		if ((sdSourceSurfaceDesc.MultiSampleType) == MULTISAMPLE_NONE)
		{
			if (!pTestEnv -> InitializeCompatibleSMSurface(sdSourceSurfaceDesc, &texsurfSourceLoader))
				pTestEnv -> WriteToLog("Unable to create a system memory surface compatible with the source surface.");
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
							(pTestEnv -> m_pSrcDevice) ->
							D3DXLoadSurfaceFromFileInMemory
							(
								lpd3dsurfSourceLoader,
								NULL,
								NULL,
								pTestEnv -> BitmapDataForeground,
								((PBITMAPFILEHEADER)(pTestEnv -> BitmapDataForeground)) -> bfSize,
								NULL,
								D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
								0,
								&d3dxii
							)
						)
					)
				)
					pTestEnv -> WriteToLog("D3DXLoadSurfaceFromFileInMemory hresult=0x%x. Unable to load the source-compatible system memory surface with the 'foreground image'.",hresult);
				else
				{
					// Copy the contents of the loader surface onto
					// the source surface.

					/*
					if
					(
						FAILED
						(
							(
								hresult =
								(pTestEnv -> m_pSrcDevice) -> CopyRects
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
					*/

					// If the source surface is a "back buffer N" surface, it will have been
					// initialized already so the UpdateSurface operation should be skipped.
					if (uiSurfaceTypeId == 6)
					{
                        bInitialized = true;
						dwMainSurfaceSpec = dwSurfaceSpec;
						return true;
					}

					if
					(
						FAILED
						(
							(
								hresult =
                                ::D3DXLoadSurfaceFromSurface
                                (
                                    lpd3dsurfSource-> GetD3DSurface9(),
                                    NULL,
                                    NULL,
                                    lpd3dsurfSourceLoader-> GetD3DSurface9(),
                                    NULL,
                                    NULL,
                                    FILTER_NONE,
                                    0
                                )
								/*(pTestEnv -> m_pSrcDevice) -> UpdateSurface
								(
									lpd3dsurfSourceLoader,
									NULL,
									lpd3dsurfSource,
									NULL
								)*/
							)
						)
					)
                    {
						pTestEnv -> WriteToLog("Unable to copy the 'foreground image' onto the source surface.");
                    }
					else
					{
                        bInitialized = true;
						dwMainSurfaceSpec = dwSurfaceSpec;
						return true;
					}
				}

				texsurfSourceLoader.ReleaseDXObjects();
			}
		}
		else
		{
			if (!pTestEnv -> InitializeCompatibleMMSurface(sdSourceSurfaceDesc, &texsurfSourceLoader))
				pTestEnv -> WriteToLog("zUnable to create a system memory surface compatible with the source surface.");
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
							(pTestEnv -> m_pSrcDevice) ->
							D3DXLoadSurfaceFromFileInMemory
							(
								lpd3dsurfSourceLoader,
								NULL,
								NULL,
								pTestEnv -> BitmapDataForeground,
								((PBITMAPFILEHEADER)(pTestEnv -> BitmapDataForeground)) -> bfSize,
								NULL,
								D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
								0,
								&d3dxii
							)
						)
					)
				)
                pTestEnv -> WriteToLog("D3DXLoadSurfaceFromFileInMemory hresult=0x%x. Unable to load the source-compatible system memory surface with the 'foreground image'.",hresult);
				else
				{
                    // If the source surface is a "back buffer N" surface, it will have been
					// initialized already so the render operation should be skipped.

					if (uiSurfaceTypeId == 6)
					{
						bInitialized = true;
						dwMainSurfaceSpec = dwSurfaceSpec;
						return true;
					}

					// Render the "foreground image" from the managed texture onto the source
					// surface.

					// From
					// texsurfSourceLoader.GetTexturePointer()
					// to
					// lpd3dsurfSource

					// Obtain the dimensions of the texture surface and use them to set up the viewport
					// for the texture-rendering step.  Since the dimensions are derived from one of the
					// texture's mip levels, the matching mip level should be selected for rendering.

					VIEWPORT vpRenderTexture;

					vpRenderTexture.X = (vpRenderTexture.Y = 0);
					vpRenderTexture.Width = sdSourceSurfaceDesc.Width;
					vpRenderTexture.Height = sdSourceSurfaceDesc.Height;
					vpRenderTexture.MinZ = 0.0f;
					vpRenderTexture.MaxZ = 1.0f;

					
                    //
                    //  Need a texture to pass to the renderer. All temp surfaces are offscreenplain
                    //
                    //
                    //  clsTestSurface* pRenderTexture = pTestEnv -> CreateSurfaceFromDWORD(dwSurfaceSpec);
                    //
                    //
                    
                    

                    SURFACEDESC desc;
                    hresult = texsurfSourceLoader.GetSurfacePointer()->GetDesc( &desc );
                    FMT format = (FMT)((desc.Format).d3dfFormat);
                    
                    hresult = pTestEnv -> m_pD3D -> CheckDeviceFormat(
                        pTestEnv -> uiSrcDeviceAdapterOrdinal,
		                pTestEnv -> dwSrcDeviceType,
		                pTestEnv -> displaymodeAdapter.Format,
		                desc.Usage,
		                RTYPE_TEXTURE,
		                format);

                    //
                    // Try to find a valid texture format
                    //
                     if(FAILED(hresult))
                     {
                        for ( int i = 0; i < cuiNoOfTextureFormats; i++)
                        {
                            hresult =pTestEnv -> m_pD3D -> CheckDeviceFormat(
                                pTestEnv -> uiSrcDeviceAdapterOrdinal,
		                        pTestEnv -> dwSrcDeviceType,
		                        pTestEnv -> displaymodeAdapter.Format,
		                        0,
		                        RTYPE_TEXTURE,
		                        FormatSubsetTexture[i]);
                            
                           pTestEnv ->  WriteToLog("CheckDeviceFormat0x%x, %d", hresult, FormatSubsetTexture[i]);
                            if(SUCCEEDED(hresult))
                            {
                                format = FormatSubsetTexture[i];
                                break;
                            }         
                        }
                     }
                    
                    clsTextureSurface tempTexture;
                    if(tempTexture.Initialize(pTestEnv, desc.Width, desc.Height, format, desc.Usage, desc.Pool))
                    {
                        hresult = ::D3DXLoadSurfaceFromSurface
                        (
                            tempTexture.GetSurfacePointer()->GetD3DSurface9(),
                            NULL,
                            NULL,
                            texsurfSourceLoader.GetSurfacePointer()->GetD3DSurface9(),
                            NULL,
                            NULL,
                            FILTER_NONE,
                            0
                        );
                    }
                    
                    if(SUCCEEDED(hresult))
                    {
                        tempTexture.GetTexturePointer() -> AddDirtyRect(NULL);
                        hresult = pTestEnv -> MyRenderTexture(tempTexture.GetTexturePointer(), lpd3dsurfSource, &vpRenderTexture);
                    }

                    tempTexture.ReleaseDXObjects();


					if (FAILED(hresult))
                    {
						pTestEnv -> WriteToLog("Unable to copy the 'foreground image' onto the source surface. hr = 0x%x", hresult);
                    }
					else
					{
						bInitialized = true;
						dwMainSurfaceSpec = dwSurfaceSpec;
						return true;
					}
				}

				texsurfSourceLoader.ReleaseDXObjects();
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

bool clsDestSurfaceSet::PrepareFromDWORD(CStretchRectTestMain *pTestEnv, DWORD dwSurfaceSpec)
{
	UINT uiSurfaceTypeId = (UINT)ExtractBitsRange(dwSurfaceSpec, 28, 31);

	if (bInitialized)
		if ((dwMainSurfaceSpec == dwSurfaceSpec) && (uiSurfaceTypeId != 5) && (uiSurfaceTypeId != 6))
		// Special treatment of types 5 and 6:
		// Whenever a test surface belongs to the device swap chain, it must be initialized "from
		// scratch".  This is because the Present operation in DisplayFrame invalidates any such
		// surfaces that may have been created by a prior test case.
			return true;
		else
			ReleaseAllSurfaces();

    if (!(pDestTestSurface = pTestEnv -> CreateSurfaceFromDWORD(dwSurfaceSpec)))
	{
		//pTestEnv -> WriteToLog("Unable to create destination surface.");
	}
	else
	{
		CSurface *lpd3dsurfDest;

		pDestTestSurface -> Describe(szDescription);

		// Retrieve a pointer to the DX surface interface
		// for the destination surface.
		lpd3dsurfDest = pDestTestSurface -> GetSurfacePointer();

		// Initialize a SURFACEDESC structure, based on lpd3dsurfDest, to allow convenient
		// programmatic access to the surface properties.
		lpd3dsurfDest -> GetDesc(&sdDestSurfaceDesc);

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
			!pTestEnv -> InitializeCompatibleSMSurface(sdDestSurfaceDesc, &texsurfDestLoader) ||
			!pTestEnv -> InitializeCompatibleMMSurface(sdDestSurfaceDesc, &texsurfReference) ||
			!pTestEnv -> InitializeCompatibleSMSurface(sdDestSurfaceDesc, &texsurfDestCopy)
		)
			pTestEnv -> WriteToLog("Unable to create all of the destination-compatible auxiliary surfaces.");
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
						(pTestEnv -> m_pSrcDevice) ->
						D3DXLoadSurfaceFromFileInMemory
						(
							lpd3dsurfDestLoader,
							NULL,
							NULL,
							pTestEnv -> BitmapDataBackground,
							((PBITMAPFILEHEADER)(pTestEnv -> BitmapDataBackground)) -> bfSize,
							NULL,
							D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
							0,
							&d3dxii
						)
					)
				)
			)
				pTestEnv -> WriteToLog("Unable to load the destination-compatible 'loader' surface with the 'background image'.");
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
// CStretchRectTestMain methods

CStretchRectTestMain::CStretchRectTestMain()
{
	SetTestType(TESTTYPE_CONF);

	m_szCommandKey= _T("StretchRectTest");
	m_szLogName = _T("StretchRect");
	m_szTestName= _T("StretchRectTest");

	m_Options.D3DOptions.fMinDXVersion = 9.f;
	//m_Options.D3DOptions.fMaxDXVersion = 9.f;
	m_Options.TestOptions.bCompareSrcRef = false;
	m_Options.D3DOptions.bZBuffer		 = false;

	m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
#if FAST_TEST
    m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
#else
	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;
#endif
	m_Options.D3DOptions.dwSwapEffect = D3DSWAPEFFECT_FLIP;

	/*
	{
		FORMAT fmtFSFormat(FMT_X8R8G8B8);

		m_Options.D3DOptions.DisplayMode.dwWidth = 1024;
		m_Options.D3DOptions.DisplayMode.dwHeight = 768;
		m_Options.D3DOptions.DisplayMode.dwBPP = 32;
		m_Options.D3DOptions.DisplayMode.dwRefreshRate = 60;
		m_Options.D3DOptions.DisplayMode.Format = fmtFSFormat;
	}
	*/

	//m_bUseOptimizedCompare = false;

	//m_Options.TestOptions.fPassRequired = 0.97f;

	//BitmapDataForeground = (BitmapDataBackground = NULL);

	bDeviceInfoGathered = false;
	bSurfaceSpecsEnumerated = false;

	ValidSurfacesByFormat = NULL;
	ValidFilterRecordIndices = NULL;

	pvbAuxiliaryVertexBuffer = NULL;
	//psurfAuxiliaryRenderTarget = NULL;
	psurfAuxiliaryRenderTargetTest = NULL;
	psurfAuxiliaryRenderTargetRef = NULL;
}

CStretchRectTestMain::~CStretchRectTestMain()
{
	if (ValidSurfacesByFormat)
		delete [] ValidSurfacesByFormat;
	if (ValidFilterRecordIndices)
		delete [] ValidFilterRecordIndices;
}

/*
bool CStretchRectTestMain::InitializeAuxiliaryResources()
{
	HRESULT hresult;

	FORMAT fmtRT((FMT)D3DFMT_A8R8G8B8);

	// AndrewLu: Create a "global", lockable render target for MyLoadSurfaceFromTexture.
	if (FAILED((hresult = m_pSrcDevice -> CreateRenderTarget(cuiGlobalRTWidth, cuiGlobalRTHeight, fmtRT, MULTISAMPLE_NONE, TRUE, &psurfAuxiliaryRenderTarget))))
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
					m_pSrcDevice -> CreateVertexBuffer
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
						pvbAuxiliaryVertexBuffer -> Lock
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
				pvbAuxiliaryVertexBuffer -> Unlock();

				return true;
			}

			pvbAuxiliaryVertexBuffer -> Release();
			pvbAuxiliaryVertexBuffer = NULL;
		}

		psurfAuxiliaryRenderTarget -> Release();
		psurfAuxiliaryRenderTarget = NULL;
	}

	return false;
}
*/

bool CStretchRectTestMain::InitializeAuxiliaryResources()
{
	HRESULT hresult;

	FORMAT fmtRT(FMT_X8R8G8B8);

	// AndrewLu: Create a "global", lockable render target for rendering test surfaces.
	if (FAILED((hresult = m_pSrcDevice -> CreateRenderTarget(cuiGlobalRTWidth, cuiGlobalRTHeight, /*sdSrcBackBuffer.Format*/fmtRT, MULTISAMPLE_NONE, TRUE, &psurfAuxiliaryRenderTargetTest))))
		WriteToLog(_T("CreateRenderTarget failed in CUpdateSurfaceTestMain::InitializeAuxiliaryResources.\n"));
	else
	{
		// AndrewLu: Create a "global", lockable render target for rendering reference surfaces.
		if (FAILED((hresult = m_pSrcDevice -> CreateRenderTarget(cuiGlobalRTWidth, cuiGlobalRTHeight, /*sdSrcBackBuffer.Format*/fmtRT, MULTISAMPLE_NONE, TRUE, &psurfAuxiliaryRenderTargetRef))))
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
						m_pSrcDevice -> CreateVertexBuffer
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
							pvbAuxiliaryVertexBuffer -> Lock
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
					pvbAuxiliaryVertexBuffer -> Unlock();

					return true;
				}

				pvbAuxiliaryVertexBuffer -> Release();
				pvbAuxiliaryVertexBuffer = NULL;
			}

			psurfAuxiliaryRenderTargetRef -> Release();
			psurfAuxiliaryRenderTargetRef = NULL;
		}

		psurfAuxiliaryRenderTargetTest -> Release();
		psurfAuxiliaryRenderTargetTest = NULL;
	}

	return false;
}

/*
void CStretchRectTestMain::ClearAuxiliaryResources()
{
	if (pvbAuxiliaryVertexBuffer)
	{
		pvbAuxiliaryVertexBuffer -> Release();
		pvbAuxiliaryVertexBuffer = NULL;
	}

	if (psurfAuxiliaryRenderTarget)
	{
		psurfAuxiliaryRenderTarget -> Release();
		psurfAuxiliaryRenderTarget = NULL;
	}
}
*/

void CStretchRectTestMain::ClearAuxiliaryResources()
{
	if (pvbAuxiliaryVertexBuffer)
	{
		pvbAuxiliaryVertexBuffer -> Release();
		pvbAuxiliaryVertexBuffer = NULL;
	}

	if (psurfAuxiliaryRenderTargetRef)
	{
		psurfAuxiliaryRenderTargetRef -> Release();
		psurfAuxiliaryRenderTargetRef = NULL;
	}

	if (psurfAuxiliaryRenderTargetTest)
	{
		psurfAuxiliaryRenderTargetTest -> Release();
		psurfAuxiliaryRenderTargetTest = NULL;
	}
}

bool CStretchRectTestMain::InitializeBBCompatibleOSP()
{
	// Create an offscreen plain surface that matches the display mode.

	opsBBZeroInitializer.ReleaseDXObjects();
	opsBBZeroInitializer.Initialize((clsWrappedSurfaceClient*)this, displaymodeAdapter.dwWidth, displaymodeAdapter.dwHeight, (FMT)((displaymodeAdapter.Format).d3dfFormat), POOL_SYSTEMMEM);

	{
		CSurface *lpd3dsurfSourceLoader;
		HRESULT hresult;
		D3DXIMAGE_INFO d3dxii;

		// Retrieve a pointer to the "loader surface".
		lpd3dsurfSourceLoader = opsBBZeroInitializer.GetSurfacePointer();

		// Load the "foreground" image onto the surface.

		hresult =
		m_pSrcDevice -> D3DXLoadSurfaceFromFileInMemory
		(
			lpd3dsurfSourceLoader,
			NULL,
			NULL,
			BitmapDataForeground,
			((PBITMAPFILEHEADER)BitmapDataForeground) -> bfSize,
			NULL,
			D3DX_FILTER_LINEAR | D3DX_FILTER_MIRROR,
			0,
			&d3dxii
		);
	}

	return true;
}

/*
void CStretchRectTestMain::FindValidTextureSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
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
					m_pD3D -> CheckDeviceFormat
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

			pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
			//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
			//pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
		}
	}
}
*/

void CStretchRectTestMain::ClearFormatSurfaceSpecs()
// Delete the sets of source- and destination "surface specs" associated with each format.
{
	if (ValidSurfacesByFormat)
	{
		UINT uiFormatIndex;

		for (uiFormatIndex = 0; uiFormatIndex < cuiNoOfFormats; ++uiFormatIndex)
		{
			((ValidSurfacesByFormat[uiFormatIndex]).ValidSourceSpecs).Clear();
			((ValidSurfacesByFormat[uiFormatIndex]).ValidDestSpecs).Clear();
		}
	}
}

void CStretchRectTestMain::FindValidSurfaceSpecsPerFormat()
{
	if (!bSurfaceSpecsEnumerated)
	// For each format, determine the set of valid source surface specs and the set of valid
	// destination surface specs.
	{
		UINT uiFormatIndex;

		GatherDeviceInfo();
		ClearFormatSurfaceSpecs();
        HRESULT hresult;
        
	    bool printOnce = true;
        for (uiFormatIndex = 0; uiFormatIndex < cuiNoOfFormats; ++uiFormatIndex)
		{
            FORMAT FrameworkFormat(DXGraphicsFormats[uiFormatIndex].fmtFW);

            //
            // Source surfaces require an offscreenplainsurface in addition to the test surface,
            // to load the source texture into. It is converted to a texture only when necessary to 
            // render for test source vaildation.
            //
            if
			(
				SUCCEEDED
				(
					(
						hresult =
						m_pD3D -> CheckDeviceFormat
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
                if(printOnce)
                {
                   WriteToLog(_T("Adding Source surface Specs...")); 
                   printOnce = false;
                }

                FindValidSourceSurfaceSpecs(uiFormatIndex, &((ValidSurfacesByFormat[uiFormatIndex]).ValidSourceSpecs));
			}
        }  

        printOnce = true;
        for (uiFormatIndex = 0; uiFormatIndex < cuiNoOfFormats; ++uiFormatIndex)
		{
            FORMAT FrameworkFormat(DXGraphicsFormats[uiFormatIndex].fmtFW);

            // AndrewLu; 05/05/2004; Consider only the formats that can serve as texture formats.
			// For now, the test is limited to using UpdateSurface from a system memory texture
			// surface to initialize the video memory test surfaces.  Greater flexibility could
			// be achieved by enabling the use offscreen plain surfaces in the initialization
			// step if the format were not supported as a texture format.
			if
			(
				SUCCEEDED
				(
					(
						hresult =
						m_pD3D -> CheckDeviceFormat
						(
							uiSrcDeviceAdapterOrdinal,
							dwSrcDeviceType,
							displaymodeAdapter.Format,
							0,
							RTYPE_TEXTURE,
							FrameworkFormat
						)
					)
				)
			)
			{				
		        if(printOnce)
                {
                    WriteToLog(_T("Adding Dest surface Specs...")); 
                    printOnce = false;
                }

                FindValidDestSurfaceSpecs(uiFormatIndex, &((ValidSurfacesByFormat[uiFormatIndex]).ValidDestSpecs));
			}
		}

		bSurfaceSpecsEnumerated = true;
	}
}


void CStretchRectTestMain::FindValidTextureSpecs(UINT uiFormatIndex, UINT *CandidateUsages, UINT uiNoOfCandidateUsages, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList)
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
					m_pD3D -> CheckDeviceFormat
					(
						uiSrcDeviceAdapterOrdinal,
						dwSrcDeviceType,
						displaymodeAdapter.Format,
						dwUsageSets[CandidateUsages[uiUsageSetIndex]],
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
			(DWORD)(CandidateUsages[uiUsageSetIndex]) |
			((DWORD)uiPoolIndex << 8);	// Bits 8 to 15 of the surface spec contain the pool type index.

			pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
            WriteToLog("Add Fromat: %s. CheckDeviceFormat for RTYPE_TEXTURE, usage=%d" , DXGraphicsFormats[uiFormatIndex].szName, dwUsageSets[CandidateUsages[uiUsageSetIndex]]);

			//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
			//pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
		}
	}
}

void CStretchRectTestMain::FindValidCubeTextureSpecs(UINT uiFormatIndex, UINT *CandidateUsages, UINT uiNoOfCandidateUsages, UINT uiPoolIndex, clsSurfaceSpecList *pSpecList)
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
					m_pD3D -> CheckDeviceFormat
					(
						uiSrcDeviceAdapterOrdinal,
						dwSrcDeviceType,
						displaymodeAdapter.Format,
						dwUsageSets[CandidateUsages[uiUsageSetIndex]],
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
			(DWORD)(CandidateUsages[uiUsageSetIndex]) |
			((DWORD)uiPoolIndex << 8);	// Bits 8 to 15 of the surface spec contain the pool type index.

			pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
            WriteToLog("Add Fromat: %s. CheckDeviceFormat for RTYPE_CUBETEXTURE usage=%d" , DXGraphicsFormats[uiFormatIndex].szName, dwUsageSets[CandidateUsages[uiUsageSetIndex]]);

			//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
			//pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
		}
	}
}

void CStretchRectTestMain::FindValidOffscreenPlainSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(DXGraphicsFormats[uiFormatIndex].fmtFW);

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				m_pD3D -> CheckDeviceFormat
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

		pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
		//dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
		//pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
        WriteToLog("Add Fromat: %s. CheckDeviceFormat for RTYPE_SURFACE" , DXGraphicsFormats[uiFormatIndex].szName);
	}
}

void CStretchRectTestMain::FindValidBBZeroSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
{
	if ((DXGraphicsFormats[uiFormatIndex]).fmtFW == (FMT)((displaymodeAdapter.Format).d3dfFormat))
	{
		pSpecList -> AddSurfaceSpec(((DWORD)5 << 28));
	}
}

void CStretchRectTestMain::FindValidBBNSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
{
	if ((DXGraphicsFormats[uiFormatIndex]).fmtFW == (FMT)((displaymodeAdapter.Format).d3dfFormat))
	{
		pSpecList -> AddSurfaceSpec(((DWORD)6 << 28));
	}
}

/*
void CStretchRectTestMain::FindValidTextureSpecs(clsSurfaceSpecList *pSpecList)
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
						m_pD3D -> CheckDeviceFormat
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

						pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
						dwSurfaceSpec |= (DWORD)1 << 24; // Set the dimension index (bits 24--27) to 1.
						pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
					}
				}
			}
		}
	}
}
*/

/*
void CStretchRectTestMain::FindValidSourceSurfaceSpecs() {}
void CStretchRectTestMain::FindValidDestSurfaceSpecs() {}
*/

/*
void CStretchRectTestMain::FindValidSurfaceSpecs()
{
	clsSurfaceSpecList SurfaceSpecList;

	// Based on the format of the source device adapter's current mode and
	// the capabilities of the device, populate the surface specification list.
	FindValidTextureSpecs(&SurfaceSpecList);

	// Use the list to initialize the surface specification array.
	// SurfaceSpecList.InitializeSurfaceSpecArray(&SurfaceSpecArray);
}
*/

void CStretchRectTestMain::InitializeTestGroup()
{
	// Initialization steps that are performed only once per test execution:

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

	// Ensure availability of an array whose elements will (in conjunction with
	// uiNoOfValidFilterRecordIndices) specify texture filter types supported by the device.
	if (!ValidFilterRecordIndices)
		ValidFilterRecordIndices = new UINT[cuiNoOfFilterTypes];
}

void CStretchRectTestMain::SetScreenDisplayRects()
{
	HRESULT hresult;
	CSurface *psurfSrcBackBuffer = NULL;

	// Obtain a pointer to a surface representing a back buffer.
	if (SUCCEEDED((hresult = m_pSrcDevice -> /*GetBackBuffer8*/GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &psurfSrcBackBuffer))))
	{
		SURFACEDESC sdSrcBackBuffer;

		// Get details of the back buffer surface.
		hresult = psurfSrcBackBuffer -> /*GetDesc8*/GetDesc(&sdSrcBackBuffer);
		psurfSrcBackBuffer -> Release();
		
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
			WriteToLog(_T("del: %d\n"), uiDisplayEdgeLength);

			WriteToLog(_T("w: %d; h: %d; l: %d; t: %d; r: %d; b: %d\n"),
			sdSrcBackBuffer.Width, sdSrcBackBuffer.Height, rectReferenceDisplay.left,
			rectReferenceDisplay.top, rectReferenceDisplay.right, rectReferenceDisplay.bottom);
			*/


		}
	}
}

bool CStretchRectTestMain::GatherDeviceInfo()
{
	if (!bDeviceInfoGathered)
	{
		HRESULT hresult;

		// Determine the adapter ordinal and the type of the Direct3D device, i.e. HAL, REF, etc.
		{
			DEVICE_CREATION_PARAMETERS dcpSrcDevice;

			hresult = m_pSrcDevice -> GetCreationParameters(&dcpSrcDevice);
			uiSrcDeviceAdapterOrdinal = dcpSrcDevice.AdapterOrdinal;
			dwSrcDeviceType = (DWORD)(dcpSrcDevice.DeviceType);
		}

		// Retrieve the caps for the tested device.
		hresult = m_pD3D -> GetDeviceCaps(uiSrcDeviceAdapterOrdinal, dwSrcDeviceType, &SrcDeviceCaps);

		// Get the adapter's current display mode.
		hresult = m_pD3D -> GetAdapterDisplayMode(uiSrcDeviceAdapterOrdinal, &displaymodeAdapter);

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

		bDeviceInfoGathered = true;
	}

	return bDeviceInfoGathered;
}

bool CStretchRectTestMain::VerifyDeviceCompatibility()
{
	GatherDeviceInfo();
	return true;
}

UINT CStretchRectTestMain::TestInitialize(void)
{
	HRESULT hresult;
	UINT uiTotalTestCases = 0;

	// Initialization steps that are performed only once per test execution:

	InitializeTestGroup();

	// Initialization steps that are performed once per TestInitialize / TestTerminate pair:

	bDeviceInfoGathered = false;
	bSurfaceSpecsEnumerated = false;

	SetScreenDisplayRects();

	GatherDeviceInfo();

	if (!VerifyDeviceCompatibility())
	{
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

	ClearAuxiliaryResources();
	if (!InitializeAuxiliaryResources())
	{
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

	if (!InitializeBBCompatibleOSP())
	{
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

	// Enable multisampling:
	m_pSrcDevice -> SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	// Avoid generating debug output when GetDepthStencilSurface returns "not found".
	m_pSrcDevice -> SetSuppressGetDepthStencilDBGOutput(true);

    FindValidSurfaceSpecsPerFormat();

	uiTotalTestCases = EnumerateTestCases();

	if (uiTotalTestCases)
	{
		SetTestRange(1, uiTotalTestCases);
		return D3DTESTINIT_RUN;
	}
	else
	{
		WriteToLog("No test cases found.\n");
		SetTestRange(1, 1);
		return D3DTESTINIT_SKIPALL;
	}

	return D3DTESTINIT_ABORT;
}

bool CStretchRectTestMain::TestTerminate(void)
{
	SourceSurfaceSet.ReleaseAllSurfaces();
	DestSurfaceSet.ReleaseAllSurfaces();
	TestCasesByFormatPair.DeleteAll();
	ClearFormatSurfaceSpecs();
	ClearAuxiliaryResources();
	opsBBZeroInitializer.ReleaseDXObjects();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// "Wrapped Surface Client" methods

HRESULT CStretchRectTestMain::WSCCreateTexture(UINT uiWidth, UINT uiHeight, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CTexture **ppTexture)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(fmt);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice -> CreateTexture
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
		uiWidth, uiHeight, uiLevels, szUsageString, (FindFormatRecord(fmt)) -> szName, (FindPoolRecord(dwPool)) -> szName);
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

HRESULT CStretchRectTestMain::WSCCreateCubeTexture(UINT uiEdgeLength, UINT uiLevels, DWORD dwUsage, FMT fmt, DWORD dwPool, CCubeTexture **ppCubeTexture)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(fmt);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice -> CreateCubeTexture
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
		uiEdgeLength, uiLevels, szUsageString, (FindFormatRecord(fmt)) -> szName, (FindPoolRecord(dwPool)) -> szName);
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

HRESULT CStretchRectTestMain::WSCCreateOffscreenPlainSurface(UINT uiWidth, UINT uiHeight, FMT fmt, DWORD dwPool, CSurface **ppSurface)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(fmt);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice -> CreateOffscreenPlainSurface
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
		uiWidth, uiHeight, (FindFormatRecord(fmt)) -> szName, (FindPoolRecord(dwPool)) -> szName);
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

HRESULT CStretchRectTestMain::WSCCreateRenderTarget(UINT uiWidth, UINT uiHeight, FMT fmt, MULTISAMPLE_TYPE MultiSample, DWORD dwMultisampleQuality, BOOL bLockable, CSurface **ppSurface)
{
	HRESULT hresult;
	FORMAT FrameworkFormat(fmt);

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice -> CreateRenderTarget
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
		uiWidth, uiHeight, (FindFormatRecord(fmt)) -> szName, (FindMultiSampleRecord(MultiSample)) -> szName, dwMultisampleQuality, bLockable ? "TRUE" : "FALSE");
		WriteToLog(szErrorMessage);
	}

	return hresult;
}

bool CStretchRectTestMain::WSCRetrieveBackBufferZero(CSurface **ppBackBuffer)
{
	HRESULT hresult;

	if (!m_pSrcDevice)
		WriteToLog("RetrieveBackBufferZero failed.  Src device unavailable.\n");
	else
		if (FAILED((hresult = m_pSrcDevice -> GetBackBuffer(0, 0, BACKBUFFER_TYPE_MONO, ppBackBuffer))))
			WriteToLog("RetrieveBackBufferZero failed.  GetBackBuffer failed.\n");
		else
			return true;

	return false;
}

bool CStretchRectTestMain::WSCRetrieveBackBufferN(CSurface **ppBackBuffer)
{
	HRESULT hresult;

	if (!m_pSrcDevice)
		WriteToLog("RetrieveBackBufferN failed.  Src device unavailable.\n");
	else
		if (FAILED((hresult = m_pSrcDevice -> GetBackBuffer(0, 1, BACKBUFFER_TYPE_MONO, ppBackBuffer))))
			WriteToLog("RetrieveBackBufferN failed.  GetBackBuffer failed.\n");
		else
			return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Surface Specification Interpretation

clsTestSurface *CStretchRectTestMain::CreateSurfaceFromDWORD(DWORD dwSpec)
{
	clsTestSurface *ptestsurfNew = NULL;
	UINT uiTypeIndex;

	//WriteToLog("\nCreateSurfaceFromDWORD called with dwSpec=%d index=%d\n", dwSpec, (UINT)ExtractBitsRange(dwSpec, 28, 31));

	switch ((uiTypeIndex = (UINT)ExtractBitsRange(dwSpec, 28, 31)))
	{
	case 5:
		// Back Buffer 0 surface (device swap chain)
		{
			clsBBZeroSurface *pbbzerosurfNew = NULL;

			if (pbbzerosurfNew = new clsBBZeroSurface)
				if (pbbzerosurfNew -> Initialize((clsWrappedSurfaceClient *)this))
					ptestsurfNew = pbbzerosurfNew;
				else
					delete pbbzerosurfNew;
		}
		break;
	case 6:
		// Back Buffer N surface (device swap chain)
		{
			clsBBNSurface *pbbnsurfNew = NULL;

			if (pbbnsurfNew = new clsBBNSurface)
				if (pbbnsurfNew -> Initialize((clsWrappedSurfaceClient *)this))
					ptestsurfNew = pbbnsurfNew;
				else
					delete pbbnsurfNew;
		}
		break;
	default:
		{
			// Surface types whose specifications include format and dimension data.
			UINT uiFormatIndex, uiDimensionIndex;

			if ((uiFormatIndex = (UINT)ExtractBitsRange(dwSpec, 16, 23)) < cuiNoOfFormats)
				if ((uiDimensionIndex = (UINT)ExtractBitsRange(dwSpec, 24, 27)) < cuiNoOfDimensions)
                {
                    //WriteToLog("CreateSurfaceFromDword Surface types %d format %s",uiTypeIndex,DXGraphicsFormats[uiFormatIndex].szName);
				
                    switch (uiTypeIndex)
					{
					case 0:
					// Back buffer surface (additional swap chain)
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
											prtsurfNew -> Initialize
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
											ptexsurfNew -> Initialize
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
						{
							clsCubeMapSurface *ptexsurfNew = NULL;
							UINT uiUsageIndex, uiPoolTypeIndex;

							if ((uiUsageIndex = (UINT)ExtractBitsRange(dwSpec, 0, 7)) < cuiNoOfUsageSets)
								if ((uiPoolTypeIndex = (UINT)ExtractBitsRange(dwSpec, 8, 15)) < cuiNoOfPoolTypes)
									if (ptexsurfNew = new clsCubeMapSurface)
										if
										(
											ptexsurfNew -> Initialize
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
										pospsurfNew -> Initialize
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
                    }
		}

		break;
	}

	return ptestsurfNew;
}


bool CStretchRectTestMain::InitializeCompatibleSMSurface(SURFACEDESC sdDescription, clsOffscreenPlainSurface *pSystemMemSurface)
{
	    if
	    (
		    pSystemMemSurface -> Initialize
		    (
			    (clsWrappedSurfaceClient *)this,
			    sdDescription.dwWidth,
			    sdDescription.dwHeight,
			    (FMT)((sdDescription.Format).d3dfFormat),
			    POOL_SYSTEMMEM
		    )
	    )
        return true;
    
	return false;
}

bool CStretchRectTestMain::InitializeCompatibleSMSurface(SURFACEDESC sdDescription, clsTextureSurface *pSystemMemSurface)
{
	
    // Try to create a (lockable) system memory Texture compatible with the
	// surface description and format.  Other wise find a suitable Texture format
    FMT format = (FMT)((sdDescription.Format).d3dfFormat);

     HRESULT hresult = m_pD3D -> CheckDeviceFormat(
            uiSrcDeviceAdapterOrdinal,
		    dwSrcDeviceType,
		    displaymodeAdapter.Format,
		    0,
		    RTYPE_TEXTURE,
		    (FMT)((sdDescription.Format).d3dfFormat));

     //
     // Try to find a valid texture format
     //
     if(FAILED(hresult))
     {
        for ( int i = 0; i < cuiNoOfTextureFormats; i++)
        {
            hresult = m_pD3D -> CheckDeviceFormat(
                uiSrcDeviceAdapterOrdinal,
		        dwSrcDeviceType,
		        displaymodeAdapter.Format,
		        0,
		        RTYPE_TEXTURE,
		        FormatSubsetTexture[i]);
            
            WriteToLog("InitializeCompatibleSMSurface CheckDeviceFormat0x%x, %d", hresult, FormatSubsetTexture[i]);
            if(SUCCEEDED(hresult))
            {
                format = FormatSubsetTexture[i];
                break;
            }         
        }
     }
     
     if(SUCCEEDED(hresult))
     {
	    if
	    (
		    pSystemMemSurface -> Initialize
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
     }
    
	return false;
}

bool CStretchRectTestMain::InitializeCompatibleMMSurface(SURFACEDESC sdDescription, clsTextureSurface *pManagedMemSurface)
{
	// Create a managed memory surface compatible with the
	// surface description.
	if
	(
		pManagedMemSurface -> Initialize
		(
			(clsWrappedSurfaceClient *)this,
			sdDescription.dwWidth,
			sdDescription.dwHeight,
            (FMT)((sdDescription.Format).d3dfFormat),
			(DWORD)0,
			POOL_DEFAULT
		)
	)
    {
        return true;
    }
    
    return false;
}

bool CStretchRectTestMain::InitializeCompatibleMMSurface(SURFACEDESC sdDescription, clsOffscreenPlainSurface *pManagedMemSurface)
{
	// Create a managed memory surface compatible with the
	// surface description.
	if
	(
		pManagedMemSurface -> Initialize
		(
			(clsWrappedSurfaceClient *)this,
			sdDescription.dwWidth,
			sdDescription.dwHeight,
            (FMT)((sdDescription.Format).d3dfFormat),
			POOL_DEFAULT
		)
	)
    {
        return true;
    }
    
    return false;
}

bool CStretchRectTestMain::TestStretchRectOperation(RECT *prectSource, RECT *prectDest, UINT uiFilterRecordIndex, float fSurfaceMinimumConformanceFraction)
{
	HRESULT hresult;

	CSurface *lpd3dsurfSource, *lpd3dsurfDest, *lpd3dsurfDestLoader,
	*lpd3dsurfSourceRef, *lpd3dsurfDestRef;
    
    //CSurface *lpd3dsurfDestCopy;

	// Obtain pointers to the surface interfaces.
	lpd3dsurfSource = (SourceSurfaceSet.pSourceTestSurface) -> GetSurfacePointer();
	lpd3dsurfDest = (DestSurfaceSet.pDestTestSurface) -> GetSurfacePointer();
	lpd3dsurfDestLoader = (DestSurfaceSet.texsurfDestLoader).GetSurfacePointer();
	lpd3dsurfSourceRef = (SourceSurfaceSet.texsurfSourceLoader).GetSurfacePointer();
	lpd3dsurfDestRef = (DestSurfaceSet.texsurfReference).GetSurfacePointer();
	//lpd3dsurfDestCopy = (DestSurfaceSet.texsurfDestCopy).GetSurfacePointer();

	// Load the background image onto the destination surface.
	/*
	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice -> CopyRects
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
				m_pSrcDevice -> UpdateSurface
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

	// Perform the tested StretchRect operation.

	if
	(
		FAILED
		(
			(
				hresult =
				m_pSrcDevice -> StretchRect
				(
					lpd3dsurfSource,
					prectSource,
					lpd3dsurfDest,
					prectDest,
					(DXFilterTypes[uiFilterRecordIndex]).dwFWFilter
				)
			)
		)
	)
	{
		WriteToLog("The tested StretchRect operation failed.\n");
		Fail();
	}
	else
	{
		UINT uiReferenceFilterIndex;

		// Copy the result of the tested StretchRect operation onto a lockable surface
		// so that it can be compared with the result of the reference copy/copies.
		/*
		if
		(
			FAILED
			(
				(
					hresult =
					m_pSrcDevice -> CopyRects
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
		if (!((DestSurfaceSet.pDestTestSurface) -> DumpContentToSysMem(this, lpd3dsurfDestCopy)))
		{
			WriteToLog("Unable to retrieve the result of the tested copy for comparison with the reference copy.\n");
			return false;
		}
		*/

		// For each of the D3DX Filter types that D3DXLoadSurfaceFromSurface could use to
		// generate a comparable result, generate a reference image and compare it with
		// the result of the tested StretchRect operation.

		for (uiReferenceFilterIndex = 0; uiReferenceFilterIndex < (DXFilterTypes[uiFilterRecordIndex]).uiNoOfD3DXFilterTypes; ++uiReferenceFilterIndex)
		{
			float fCompareResult;

			// Load the background image onto the destination surface for the reference copy.
			/*
			if
			(
				FAILED
				(
					(
						hresult =
						m_pSrcDevice -> CopyRects
						(
							lpd3dsurfDestLoader,
							NULL,
							0,
							lpd3dsurfDestRef,
							NULL
						)
					)
				)
			)
			*/
			if (FAILED((hresult = /*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(lpd3dsurfDestRef -> GetD3DSurface9(), NULL, NULL, lpd3dsurfDestLoader -> GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
			{
				WriteToLog("D3DXLoadSurfaceFromSurface failed hresult=0x%x. Unable to load the destination surface for the reference copy.\n",hresult);
				return false;
			}

			// Perform the reference copy.
			if
			(
				!ReferenceStretchRect(lpd3dsurfSourceRef, prectSource, lpd3dsurfDestRef, prectDest, (DXFilterTypes[uiFilterRecordIndex]).D3DXFilterTypesArray[uiReferenceFilterIndex])
			)
			{
				WriteToLog("Unable to perform the reference StretchRect operation.\n");
				return false;
			}

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

			// Check the results
			/*
			if ((TestApp.m_ImageCmp).GetCompareResult(1.f - m_Options.TestOptions.fPassRequired, m_Options.TestOptions.fColorRange, 
											m_Options.TestOptions.uMinPixels, m_Options.TestOptions.uMaxPixels))
			*/
			//fCompareResult = (TestApp.m_ImageCmp).GetCompareResult();

			if (!((DestSurfaceSet.pDestTestSurface) -> VerifyAgainstSysMemTexture(this, (DestSurfaceSet.texsurfReference).GetTexturePointer(), &fCompareResult)))
			{
				WriteToLog("Unable to retrieve the result of the tested copy for comparison with the reference copy.\n");
				return false;
			}

			if (fCompareResult >= fSurfaceMinimumConformanceFraction)
			{
				Pass();
				return true;
			}
			else WriteToLog("Expected Conformance: %f; Actual Conformance: %f\n", fSurfaceMinimumConformanceFraction, fCompareResult);
		}

		WriteToLog("The reference and test images differ significantly.\n");
		Fail();
	}

	// The tested StretchRect operation failed or generated an incorrect result.  Report the
	// details of the surfaces, rectangles, and filter type involved.
	//WriteToLog("\n");
	WriteToLog("Source: %s\n", SourceSurfaceSet.szDescription);
	WriteToLog("Destination: %s\n", DestSurfaceSet.szDescription);
	//WriteToLog("Source Rectangle: {%d, %d, %d, %d}\n", rectSource.left, rectSource.top, rectSource.right, rectSource.bottom);
	//WriteToLog("Destination Rectangle: {%d, %d, %d, %d}\n", rectDest.left, rectDest.top, rectDest.right, rectDest.bottom);
	WriteToLog("Filter: %s\n", (DXFilterTypes[uiFilterRecordIndex]).szName);
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
				sprintf(szLogLine, "Source: %s", pSourceSurfaceSet -> szDescription);
				WriteToLog(szLogLine);
				sprintf(szLogLine, "Destination: %s", pDestSurfaceSet -> szDescription);
				WriteToLog(szLogLine);
				sprintf(szLogLine, "Rectangle set: %d (%s)", uiRectSetIndex, RectangleSets[uiRectSetIndex].szDescription);
				WriteToLog(szLogLine);

				sprintf(szLogLine, "Repro: Execute 'copyrects -M%d -T%d' in a %dbpp desktop mode.",
				uiTestedAdapterOrdinal + 1, (UINT)dwTestCaseID, (pd3dfmtrecDesktop -> uiBytesPerPixel) * 8);
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

bool CStretchRectTestMain::ReferenceStretchRect(CSurface *pSourceSurface, CONST RECT *pSourceRect, CSurface *pDestSurface, CONST RECT *pDestRect, DWORD dwD3DXFilter)
{
	HRESULT hresult;

	if
	(
		SUCCEEDED
		(
			(
				hresult =
				m_pSrcDevice -> D3DXLoadSurfaceFromSurface
				(
					pDestSurface,
					NULL,
					pDestRect,
					pSourceSurface,
					NULL,
					pSourceRect,
					dwD3DXFilter,
					0
				)
			)
		)
	)
		return true;
	else
		return false;
}

//virtual HRESULT     D3DXLoadSurfaceFromSurface9(CSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey);

bool CStretchRectTestMain::DisplayFrame(void)
{
	HRESULT hresult;
	CSurface *psurfSrcBackBuffer = NULL;

	if (SUCCEEDED((hresult = m_pSrcDevice -> /*GetBackBuffer8*/GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &psurfSrcBackBuffer))))
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
				psurfSrcBackBuffer -> /*GetD3DSurface8*/GetD3DSurface9(),
				NULL,
				&rectReferenceDisplay,
				pReferenceSurface -> /*GetD3DSurface8*/GetD3DSurface9(),
				NULL,
				NULL,
				D3DX_FILTER_LINEAR,
				0
			);

			// Copy the appropriate reference image(s) to the back buffer for display.

			hresult =
			::D3DXLoadSurfaceFromSurface
			(
				psurfSrcBackBuffer -> /*GetD3DSurface8*/GetD3DSurface9(),
				NULL,
				&rectTestDisplay,
				pTestSurface -> /*GetD3DSurface8*/GetD3DSurface9(),
				NULL,
				NULL,
				D3DX_FILTER_LINEAR,
				0
			);
		}

		psurfSrcBackBuffer -> Release();
	}

	m_pSrcDevice -> Present(NULL, NULL, NULL, NULL);

	return true;
}

/*
HRESULT CStretchRectTestMain::MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture)
{
	HRESULT hresult;
	SURFACEDESC sdSysmemSurface;

	// Obtain the dimensions of the system memory surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pSysmemSurface -> GetDesc(&sdSysmemSurface))))
		WriteToLog(_T("GetDesc failed in UpdateTex::MyLoadSurfaceFromTexture.\n"));
	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;

		// Set the stream source and vertex shader.
		m_pSrcDevice -> SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		//m_pSrcDevice -> SetVertexShader(D3DFVF_VERTEX);
		m_pSrcDevice -> SetFVF(dwD3DFVF_VERTEX);

		// Set the render target and viewport.
		m_pSrcDevice -> SetRenderTarget(psurfAuxiliaryRenderTarget, NULL);
		vpRenderTexture.X = (vpRenderTexture.Y = 0);
		vpRenderTexture.Width = sdSysmemSurface.Width;
		vpRenderTexture.Height = sdSysmemSurface.Height;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;
		m_pSrcDevice -> SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice -> SetTexture(0, pVidmemTexture);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice -> BeginScene();
		m_pSrcDevice -> Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice -> DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice -> EndScene();

		// Copy the contents of the viewport to the system memory texture.
		rectViewport.left = vpRenderTexture.X;
		rectViewport.top = vpRenderTexture.Y;
		rectViewport.right = rectViewport.left + vpRenderTexture.Width;
		rectViewport.bottom = rectViewport.top + vpRenderTexture.Height;

		if (!pSysmemSurface)
			DebugOut.Write("pSysmemSurface is NULL!");
		if (!psurfAuxiliaryRenderTarget)
			DebugOut.Write("psurfAuxiliaryRenderTarget is NULL!");

		::D3DXLoadSurfaceFromSurface(pSysmemSurface -> GetD3DSurface9(), NULL, NULL, psurfAuxiliaryRenderTarget -> GetD3DSurface9(), NULL, &rectViewport, FILTER_NONE, 0);
	}

	return hresult;
}
*/

HRESULT CStretchRectTestMain::MyLoadSurfaceFromTexture(CSurface *pSysmemSurface, CTexture *pVidmemTexture)
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
	if (FAILED((hresult = pSysmemSurface -> GetDesc(&sdSysmemSurface))))
		WriteToLog(_T("GetDesc failed in UpdateTex::MyLoadSurfaceFromTexture.\n"));
	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;
		CSurface *pCurrentRenderTarget = NULL, *pCurrentDepthStencil = NULL;

		// Set the stream source and vertex shader.
		m_pSrcDevice -> SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		m_pSrcDevice -> SetFVF(dwD3DFVF_VERTEX);

		/*
		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		m_pSrcDevice -> GetRenderTarget(&pCurrentRenderTarget);
		m_pSrcDevice -> GetDepthStencilSurface(&pCurrentDepthStencil);
		*/

		// Set the render target and viewport.
		m_pSrcDevice -> SetRenderTarget(psurfAuxiliaryRenderTargetTest, NULL);
		vpRenderTexture.X = (vpRenderTexture.Y = 0);
		vpRenderTexture.Width = sdSysmemSurface.Width;
		vpRenderTexture.Height = sdSysmemSurface.Height;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;
		m_pSrcDevice -> SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice -> SetTexture(0, pVidmemTexture);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice -> Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice -> BeginScene();
		m_pSrcDevice -> DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice -> EndScene();

		/*
		// Restore the original render target and depth stencil.
		m_pSrcDevice -> SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget -> Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil -> Release();
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

		/*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(pSysmemSurface -> GetD3DSurface9(), NULL, NULL, psurfAuxiliaryRenderTargetTest -> GetD3DSurface9(), NULL, &rectViewport, FILTER_NONE, 0);
	}

	return hresult;
}

HRESULT CStretchRectTestMain::MyLoadSurfaceFromCubeTexture(CSurface *pSysmemSurface, CCubeTexture *pVidmemTexture)
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
	if (FAILED((hresult = pSysmemSurface -> GetDesc(&sdSysmemSurface))))
		WriteToLog(_T("GetDesc failed in UpdateTex::MyLoadSurfaceFromTexture.\n"));
	else
	{
		VIEWPORT vpRenderTexture;
		RECT rectViewport;
		CSurface *pCurrentRenderTarget = NULL, *pCurrentDepthStencil = NULL;

		// Set the stream source and vertex shader.
		m_pSrcDevice -> SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		m_pSrcDevice -> SetFVF(dwD3DFVF_VERTEX);

		/*
		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		m_pSrcDevice -> GetRenderTarget(&pCurrentRenderTarget);
		m_pSrcDevice -> GetDepthStencilSurface(&pCurrentDepthStencil);
		*/

		// Set the render target and viewport.
		m_pSrcDevice -> SetRenderTarget(psurfAuxiliaryRenderTargetTest, NULL);
		vpRenderTexture.X = (vpRenderTexture.Y = 0);
		vpRenderTexture.Width = sdSysmemSurface.Width;
		vpRenderTexture.Height = sdSysmemSurface.Height;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;
		m_pSrcDevice -> SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice -> SetTexture(0, pVidmemTexture);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice -> Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice -> BeginScene();
		m_pSrcDevice -> DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice -> EndScene();

		/*
		// Restore the original render target and depth stencil.
		m_pSrcDevice -> SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget -> Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil -> Release();
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

		/*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(pSysmemSurface -> GetD3DSurface9(), NULL, NULL, psurfAuxiliaryRenderTargetTest -> GetD3DSurface9(), NULL, &rectViewport, FILTER_NONE, 0);
	}

	return hresult;
}

/*
HRESULT CStretchRectTestMain::MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface)
{
	HRESULT hresult;
	SURFACEDESC sdRT;

	if (FAILED((hresult = pRTSurface -> GetDesc(&sdRT))))
		WriteToLog(_T("GetDesc failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
	else
	{
		CSurface *pTemporaryOSP = NULL;

		if (FAILED((hresult = m_pSrcDevice -> CreateOffscreenPlainSurface(sdRT.Width, sdRT.Height, sdRT.Format, POOL_SYSTEMMEM, &pTemporaryOSP))))
			WriteToLog(_T("CreateOffScreenPlainSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
		else
		{
			WriteToLog("About to call GetRenderTargetData ...\n");

			if (FAILED((hresult = m_pSrcDevice -> GetRenderTargetData(pRTSurface, pTemporaryOSP))))
				WriteToLog(_T("GetRenderTargetData failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
			else
			{
				WriteToLog(_T("GetRenderTargetData succeeded in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
				if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(pSysmemSurface -> GetD3DSurface9(), NULL, NULL, pTemporaryOSP -> GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
					WriteToLog(_T("D3DXLoadSurfaceFromSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
			}

			pTemporaryOSP -> Release();
		}
	}

	return hresult;
}
*/

HRESULT CStretchRectTestMain::MyLoadSurfaceFromRT(CSurface *pSysmemSurface, CSurface *pRTSurface)
{
	HRESULT hresult;

	// Now that GetRenderTarget can accept a system memory texture surface as a destination,
	// it should be unnecessary to create a temporary offscreen plain surface.  Since
	// pSysmemSurface should always point to a system memory texture surface, simply call
	// GetRenderTargetData.

	/*
	SURFACEDESC sdRT;

	if (FAILED((hresult = pRTSurface -> GetDesc(&sdRT))))
		WriteToLog(_T("GetDesc failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
	else
	{
		CSurface *pTemporaryOSP = NULL;

		if (FAILED((hresult = m_pSrcDevice -> CreateOffscreenPlainSurface(sdRT.Width, sdRT.Height, sdRT.Format, POOL_SYSTEMMEM, &pTemporaryOSP))))
			WriteToLog(_T("CreateOffScreenPlainSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
		else
		{
			WriteToLog("About to call GetRenderTargetData ...\n");

			if (FAILED((hresult = m_pSrcDevice -> GetRenderTargetData(pRTSurface, pTemporaryOSP))))
				WriteToLog(_T("GetRenderTargetData failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
			else
			{
				WriteToLog(_T("GetRenderTargetData succeeded in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
				if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(pSysmemSurface -> GetD3DSurface9(), NULL, NULL, pTemporaryOSP -> GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
					WriteToLog(_T("D3DXLoadSurfaceFromSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromRT.\n"));
			}

			pTemporaryOSP -> Release();
		}
	}
	*/

	return (hresult = m_pSrcDevice -> GetRenderTargetData(pRTSurface, pSysmemSurface));
}

HRESULT CStretchRectTestMain::MyLoadSurfaceFromOSP(CSurface *pSysmemSurface, CSurface *pOSPSurface)
{
	HRESULT hresult;

	if (FAILED((hresult = /*m_pSrcDevice ->*/ ::D3DXLoadSurfaceFromSurface(pSysmemSurface -> GetD3DSurface9(), NULL, NULL, pOSPSurface -> GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
		WriteToLog(_T("D3DXLoadSurfaceFromSurface failed in CUpdateSurfaceTestMain::MyLoadSurfaceFromOSP.\n"));

	return hresult;
}

bool CStretchRectTestMain::CompareRenderedTextures(CTexture *pReferenceTexture, CTexture *pTestTexture, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	SURFACEDESC sdManMemSurface;

	// Obtain the dimensions of the texture surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pReferenceTexture -> GetLevelDesc(0, &sdManMemSurface))))
		WriteToLog(_T("GetLevelDesc failed in CompareRenderedTextures.\n"));
	else
	{
		SURFACEDESC renderTargetDesc;
        if (FAILED((hresult = psurfAuxiliaryRenderTargetTest -> GetDesc(&renderTargetDesc))))
        {
            WriteToLog(_T("GetLevelDesc failed in CompareRenderedTextures.\n"));
            return false;
        }


        
        VIEWPORT vpRenderTexture;
		RECT rectViewport;
		CSurface *pCurrentRenderTarget = NULL, *pCurrentDepthStencil = NULL;

		//
        // Clamp Viewport to rendertarget size. If it is larger  was causing AV on ref.
        //
        vpRenderTexture.X = (vpRenderTexture.Y = 0);
        vpRenderTexture.Width  = sdManMemSurface.Width  > renderTargetDesc.Width ? renderTargetDesc.Width : sdManMemSurface.Width ;
		vpRenderTexture.Height = sdManMemSurface.Height > renderTargetDesc.Height ? renderTargetDesc.Height : sdManMemSurface.Height ;
		vpRenderTexture.MinZ = 0.0f;
		vpRenderTexture.MaxZ = 1.0f;

		// Set the stream source and vertex shader.
		m_pSrcDevice -> SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		m_pSrcDevice -> SetFVF(dwD3DFVF_VERTEX);

		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		m_pSrcDevice -> GetRenderTarget(&pCurrentRenderTarget);
		m_pSrcDevice -> GetDepthStencilSurface(&pCurrentDepthStencil);

		//////////////////////////////////////////////////////////////
		// Render the test image:

		// Set the render target and viewport for rendering the test image.
		m_pSrcDevice -> SetRenderTarget(psurfAuxiliaryRenderTargetTest, NULL);
		m_pSrcDevice -> SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice -> SetTexture(0, pTestTexture);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice -> Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			0xffffffff,
			//RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice -> BeginScene();
		m_pSrcDevice -> DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice -> EndScene();

		//////////////////////////////////////////////////////////////
		// Render the reference image:

		
        SURFACEDESC refRenderTargetDesc;
        if (FAILED((hresult = psurfAuxiliaryRenderTargetRef -> GetDesc(&refRenderTargetDesc))))
        {
            WriteToLog(_T("GetLevelDesc failed in CompareRenderedTextures.\n"));
            return false;
        }

        //
        // Clamp Viewport to rendertarget size. If it is larger was causing AV on ref.
        //
        vpRenderTexture.Width  = sdManMemSurface.Width  > refRenderTargetDesc.Width ? refRenderTargetDesc.Width : sdManMemSurface.Width ;
		vpRenderTexture.Height = sdManMemSurface.Height > refRenderTargetDesc.Height ? refRenderTargetDesc.Height : sdManMemSurface.Height ;
	
        
        pReferenceTexture -> AddDirtyRect(NULL);

		// Set the render target and viewport for rendering the reference image.
		m_pSrcDevice -> SetRenderTarget(psurfAuxiliaryRenderTargetRef, NULL);
		m_pSrcDevice -> SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice -> SetTexture(0, pReferenceTexture);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice -> Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			0xffffffff,
			//RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice -> BeginScene();
		m_pSrcDevice -> DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice -> EndScene();

		// Restore the original render target and depth stencil.
		m_pSrcDevice -> SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget -> Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil -> Release();

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

bool CStretchRectTestMain::CompareRenderedCubeMaps(CCubeTexture *pReferenceTexture, CCubeTexture *pTestTexture, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	SURFACEDESC sdManMemSurface;

	// Obtain the dimensions of a cubemap surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pReferenceTexture -> GetLevelDesc(0, &sdManMemSurface))))
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
		m_pSrcDevice -> SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
		m_pSrcDevice -> SetFVF(dwD3DFVF_VERTEX);

		// Record the current render target and depth stencil.
		//GetRenderTarget(CSurface **ppRenderTarget)
		m_pSrcDevice -> GetRenderTarget(&pCurrentRenderTarget);
		m_pSrcDevice -> GetDepthStencilSurface(&pCurrentDepthStencil);

		//////////////////////////////////////////////////////////////
		// Render the test image:

		// Set the render target and viewport for rendering the test image.
		m_pSrcDevice -> SetRenderTarget(psurfAuxiliaryRenderTargetTest, NULL);
		m_pSrcDevice -> SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice -> SetTexture(0, pTestTexture);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice -> Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			0xffffffff,
			//RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice -> BeginScene();
		m_pSrcDevice -> DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice -> EndScene();

		//////////////////////////////////////////////////////////////
		// Render the reference image:

		//pReferenceTexture -> AddDirtyRect(NULL);

		// Set the render target and viewport for rendering the reference image.
		m_pSrcDevice -> SetRenderTarget(psurfAuxiliaryRenderTargetRef, NULL);
		m_pSrcDevice -> SetViewport(&vpRenderTexture);

		// Define the role of the texture in the rendering operation.
		m_pSrcDevice -> SetTexture(0, pReferenceTexture);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
		m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

		// Clear the viewport and render the texture.
		m_pSrcDevice -> Clear
		(
			0,
			NULL,
			D3DCLEAR_TARGET,
			0xffffffff,
			//RGB_MAKE(0x00, 0x00, 0x00),
			0.0f,
			0
		);
		m_pSrcDevice -> BeginScene();
		m_pSrcDevice -> DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		m_pSrcDevice -> EndScene();

		// Restore the original render target and depth stencil.
		m_pSrcDevice -> SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
		pCurrentRenderTarget -> Release();
		if (pCurrentDepthStencil) pCurrentDepthStencil -> Release();

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

bool CStretchRectTestMain::CompareCubeTextureToTexture(CTexture *pManagedTexture, CCubeTexture *pCubeTexture, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	SURFACEDESC sdManMemSurface;
	bool bReturnVal = false;

	// Obtain the dimensions of the texture surface and use them to set up the viewport
	// for the texture-rendering step.  Since the dimensions are derived from one of the
	// texture's mip levels, the matching mip level should be selected for rendering.
	if (FAILED((hresult = pManagedTexture -> GetLevelDesc(0, &sdManMemSurface))))
		WriteToLog(_T("GetLevelDesc failed in CompareCubeTextureToTexture.\n"));
	else
	{
		CSurface *pManTexSurface = NULL;

		if (FAILED((hresult = pManagedTexture -> GetSurfaceLevel(0, &pManTexSurface))))
			WriteToLog("GetSurfaceLevel failed in CompareCubeTextureToTexture.\n");
		else
		{
			CCubeTexture *pTemporarySysMemTexture = NULL;

			if (FAILED((hresult = m_pSrcDevice -> CreateCubeTexture(sdManMemSurface.Width, 1, 0, sdManMemSurface.Format, POOL_SYSTEMMEM, &pTemporarySysMemTexture))))
				WriteToLog("Failed to create the temporary system memory texture in CompareCubeTextureToTexture.\n");
			else
			{
				CCubeTexture *pTemporaryVidMemTexture = NULL;

				if (FAILED((hresult = m_pSrcDevice -> CreateCubeTexture(sdManMemSurface.Width, 1, 0, sdManMemSurface.Format, POOL_DEFAULT, &pTemporaryVidMemTexture))))
					WriteToLog("Failed to create the temporary video memory texture in CompareCubeTextureToTexture.\n");
				else
				{
					CSurface *pSurfaceCopy = NULL;

					if (FAILED((hresult = pTemporarySysMemTexture -> GetCubeMapSurface(CUBEMAP_FACE_POSITIVE_Z, 0, &pSurfaceCopy))))
						WriteToLog("GetSurfaceLevel (temporary texture) failed in CompareCubeTextureToTexture.\n");
					else
					{
						if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(pSurfaceCopy -> GetD3DSurface9(), NULL, NULL, pManTexSurface -> GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
							WriteToLog("D3DXLoadSurfaceFromSurface failed in CompareCubeTextureToTexture.\n");
						else
						{
							if (FAILED((hresult = m_pSrcDevice -> UpdateTexture(pTemporarySysMemTexture, pTemporaryVidMemTexture))))
								WriteToLog("UpdateTexture failed in CompareCubeTextureToTexture.\n");
							else
							{
								bReturnVal = CompareRenderedCubeMaps(pTemporaryVidMemTexture, pCubeTexture, pfSurfaceConformanceFraction);
							}
						}

						pSurfaceCopy -> Release();
					}

					pTemporaryVidMemTexture -> Release();
				}

				pTemporarySysMemTexture -> Release();
			}

			pManTexSurface -> Release();
		}
	}

	return bReturnVal;
}

bool CStretchRectTestMain::CompareOffscreenPlainToTexture(CTexture *pManagedTexture, CSurface *pOffscreenPlainSurface, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	bool bReturnVal = false;
	SURFACEDESC sdOSP;

	if (FAILED((hresult = pOffscreenPlainSurface -> GetDesc(&sdOSP))))
		WriteToLog("CSurface::GetDesc failed in CompareOffscreenPlainToTexture.\n");
	else
	{
		CTexture *pTemporarySysMemTexture = NULL;

		if (FAILED((hresult = m_pSrcDevice -> CreateTexture(sdOSP.Width, sdOSP.Height, 1, 0, sdOSP.Format, POOL_SYSTEMMEM, &pTemporarySysMemTexture))))
			WriteToLog("Failed to create the temporary system memory texture in CompareOffscreenPlainToTexture.\n");
		else
		{
			CTexture *pTemporaryVidMemTexture = NULL;

			if (FAILED((hresult = m_pSrcDevice -> CreateTexture(sdOSP.Width, sdOSP.Height, 1, 0, sdOSP.Format, POOL_DEFAULT, &pTemporaryVidMemTexture))))
				WriteToLog("Failed to create the temporary video memory texture in CompareOffscreenPlainToTexture.\n");
			else
			{
				CSurface *pSurfaceCopy = NULL;

				if (FAILED((hresult = pTemporarySysMemTexture -> GetSurfaceLevel(0, &pSurfaceCopy))))
					WriteToLog("GetSurfaceLevel (temporary texture) failed in CompareOffscreenPlainToTexture.\n");
				else
				{
					if (FAILED((hresult = ::D3DXLoadSurfaceFromSurface(pSurfaceCopy -> GetD3DSurface9(), NULL, NULL, pOffscreenPlainSurface -> GetD3DSurface9(), NULL, NULL, FILTER_NONE, 0))))
						WriteToLog("D3DXLoadSurfaceFromSurface failed in CompareOffscreenPlainToTexture.\n");
					else
					{
						if (FAILED((hresult = m_pSrcDevice -> UpdateTexture(pTemporarySysMemTexture, pTemporaryVidMemTexture))))
							WriteToLog("UpdateTexture failed in CompareOffscreenPlainToTexture.\n");
						else
							bReturnVal = CompareRenderedTextures(pManagedTexture, pTemporaryVidMemTexture, pfSurfaceConformanceFraction);
					}

					pSurfaceCopy -> Release();
				}

				pTemporaryVidMemTexture -> Release();
			}

			pTemporarySysMemTexture -> Release();
		}
	}

	return bReturnVal;

	/*
	HRESULT hresult;
	bool bReturnVal = false;
	CSurface *pTextureSurface = NULL;

	if (FAILED((hresult = pManagedTexture -> GetSurfaceLevel(0, &pTextureSurface))))
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

		pTextureSurface -> Release();
	}

	return bReturnVal;
	*/
}

bool CStretchRectTestMain::CompareRTToTexture(CTexture *pManagedTexture, CSurface *pRTSurface, float *pfSurfaceConformanceFraction)
{
	HRESULT hresult;
	bool bReturnVal = false;
	SURFACEDESC sdRT;

	if (FAILED((hresult = pRTSurface -> GetDesc(&sdRT))))
		WriteToLog("CSurface::GetDesc failed in CompareRTToTexture.\n");
	else
	{
		CTexture *pTemporarySysMemTexture = NULL;

		if (FAILED((hresult = m_pSrcDevice -> CreateTexture(sdRT.Width, sdRT.Height, 1, 0, sdRT.Format, POOL_SYSTEMMEM, &pTemporarySysMemTexture))))
			WriteToLog("Failed to create the temporary system memory texture in CompareRTToTexture.\n");
		else
		{
			CTexture *pTemporaryVidMemTexture = NULL;

			if (FAILED((hresult = m_pSrcDevice -> CreateTexture(sdRT.Width, sdRT.Height, 1, 0, sdRT.Format, POOL_DEFAULT, &pTemporaryVidMemTexture))))
				WriteToLog("Failed to create the temporary video memory texture in CompareRTToTexture.\n");
			else
			{
				CSurface *pSurfaceCopy = NULL;

				if (FAILED((hresult = pTemporarySysMemTexture -> GetSurfaceLevel(0, &pSurfaceCopy))))
					WriteToLog("GetSurfaceLevel (temporary texture) failed in CompareRTToTexture.\n");
				else
				{
					if (FAILED((hresult = m_pSrcDevice -> GetRenderTargetData(pRTSurface, pSurfaceCopy))))
						WriteToLog("GetRenderTargetData failed in CompareRTToTexture.\n");
					else
					{
						if (FAILED((hresult = m_pSrcDevice -> UpdateTexture(pTemporarySysMemTexture, pTemporaryVidMemTexture))))
							WriteToLog("UpdateTexture failed in CompareRTToTexture.\n");
						else
							bReturnVal = CompareRenderedTextures(pManagedTexture, pTemporaryVidMemTexture, pfSurfaceConformanceFraction);
					}

					pSurfaceCopy -> Release();
				}

				pTemporaryVidMemTexture -> Release();
			}

			pTemporarySysMemTexture -> Release();
		}
	}

	return bReturnVal;
}

HRESULT CStretchRectTestMain::MyRenderTexture(CTexture *ptex, CSurface *psurfRT, VIEWPORT *pVP)
{
	HRESULT hresult;

	CSurface *pCurrentRenderTarget = NULL, *pCurrentDepthStencil = NULL;

	// Set the stream source and vertex shader.
	m_pSrcDevice -> SetStreamSource(0, pvbAuxiliaryVertexBuffer, sizeof(BASICTEXTUREVERTEX), 0);
	m_pSrcDevice -> SetFVF(dwD3DFVF_VERTEX);

	// Record the current render target and depth stencil.
	//GetRenderTarget(CSurface **ppRenderTarget)
	m_pSrcDevice -> GetRenderTarget(&pCurrentRenderTarget);
	m_pSrcDevice -> GetDepthStencilSurface(&pCurrentDepthStencil);

	/////////////////////////////////////////////////////
	// Render the contents of the texture to the surface.

	// Set the render target and viewport for rendering the test image.
	m_pSrcDevice -> SetRenderTarget(psurfRT, NULL);
	m_pSrcDevice -> SetViewport(pVP);

	// Define the role of the texture in the rendering operation.
	m_pSrcDevice -> SetTexture(0, ptex);
	m_pSrcDevice -> SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, (DWORD)0);
	m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
	//m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
	m_pSrcDevice -> SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);

	// Clear the viewport and render the texture.
	m_pSrcDevice -> Clear
	(
		0,
		NULL,
		D3DCLEAR_TARGET,
		0xffffffff,
		//RGB_MAKE(0x00, 0x00, 0x00),
		0.0f,
		0
	);
	m_pSrcDevice -> BeginScene();
	hresult = m_pSrcDevice -> DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	m_pSrcDevice -> EndScene();

	// Restore the original render target and depth stencil.
	m_pSrcDevice -> SetRenderTarget(pCurrentRenderTarget, pCurrentDepthStencil);
	pCurrentRenderTarget -> Release();
	if (pCurrentDepthStencil) pCurrentDepthStencil -> Release();

	return hresult;
}

// Override this to avoid incurring failures when a failing return code is actually expected.
void CStretchRectTestMain::ProcessError(HRESULT hr)
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

void CStretchRectTestMain::AdjustRectForDXTn(RECT *pRect, UINT uiSurfaceWidth, UINT uiSurfaceHeight)
{
	pRect -> left = (UINT)((DWORD)(pRect -> left) & 0xfffffffc);
	pRect -> top = (UINT)((DWORD)(pRect -> top) & 0xfffffffc);
	pRect -> right = (UINT)((DWORD)(pRect -> right + 3) & 0xfffffffc);
	if (pRect -> right >= uiSurfaceWidth)
		pRect -> right = uiSurfaceWidth;
	pRect -> bottom = (UINT)((DWORD)(pRect -> bottom + 3) & 0xfffffffc);
	if (pRect -> bottom >= uiSurfaceHeight)
		pRect -> bottom = uiSurfaceHeight;
}

void CStretchRectTestMain::AdjustRectForUYVY(RECT *pRect, UINT uiSurfaceWidth, UINT uiSurfaceHeight)
{
	pRect -> left = (UINT)((DWORD)(pRect -> left) & 0xfffffffe);
	pRect -> top = (UINT)((DWORD)(pRect -> top) & 0xfffffffe);
	pRect -> right = (UINT)((DWORD)(pRect -> right + 1) & 0xfffffffe);
	if (pRect -> right >= uiSurfaceWidth)
		pRect -> right = uiSurfaceWidth;
	pRect -> bottom = (UINT)((DWORD)(pRect -> bottom + 1) & 0xfffffffe);
	if (pRect -> bottom >= uiSurfaceHeight)
		pRect -> bottom = uiSurfaceHeight;
}

bool CStretchRectTestMain::TestDXTn(FORMAT fmtTest)
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

bool CStretchRectTestMain::TestUYVY(FORMAT fmtTest)
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


////////////////////////////////////////////////////////////////////////////////
// CStretchRectTestNoMSRTs

CStretchRectTestNoMSRTs::CStretchRectTestNoMSRTs()
{
}

CStretchRectTestNoMSRTs::~CStretchRectTestNoMSRTs()
{
}

void CStretchRectTestNoMSRTs::FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
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
				m_pD3D -> CheckDeviceFormat
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
		DWORD dwSurfaceSpec;
		
		dwSurfaceSpec =
		((DWORD)1 << 28) | // Surface type == 1; denotes render target.
		((DWORD)uiFormatIndex << 16);
		// Note: Bits 8--15 specify the multisample type in terms of an array index.
		// Zero indexes an array entry denoting D3DMULTISAMPLE_NONE.

		pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
		pSpecList -> AddSurfaceSpec(dwSurfaceSpec | 1);
		// Note: Bit 0 denotes "lockability".  Since this method is limited to finding
		// specifications of non-multisampled surfaces, it can safely add specifications
		// for both the lockable and non-lockable variants.
	}
}


////////////////////////////////////////////////////////////////////////////////
// CStretchRectTestMSRTs

CStretchRectTestMSRTs::CStretchRectTestMSRTs()
{
}

CStretchRectTestMSRTs::~CStretchRectTestMSRTs()
{
}

void CStretchRectTestMSRTs::FindValidRTSpecs(UINT uiFormatIndex, clsSurfaceSpecList *pSpecList)
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
				m_pD3D -> CheckDeviceFormat
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
		for (uiMultiSampleTypeIndex = 1; uiMultiSampleTypeIndex < cuiNoOfMultiSampleTypes; ++uiMultiSampleTypeIndex)
		{
			if
			(
				SUCCEEDED
				(
					(
						hresult =
						m_pD3D -> CheckDeviceMultiSampleType
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

				pSpecList -> AddSurfaceSpec(dwSurfaceSpec);
			}
		}

	}
}


////////////////////////////////////////////////////////////////////////////////
// CStretchRectTestGeneral

CStretchRectTestGeneral::CStretchRectTestGeneral()
{
}

CStretchRectTestGeneral::~CStretchRectTestGeneral()
{
}

UINT CStretchRectTestGeneral::RegisterTestableFormatPair(UINT uiSourceFormatIndex, UINT uiDestFormatIndex)
{
	UINT uiNoOfSourceSpecs, uiNoOfDestSpecs, uiNoOfTestCases, uiNoOfRectPairs;
	FormatPair *pNewFormatPairElement;

	uiNoOfSourceSpecs = ((ValidSurfacesByFormat[uiSourceFormatIndex]).ValidSourceSpecs).GetNumberOfElements();
	
    uiNoOfDestSpecs = ((ValidSurfacesByFormat[uiDestFormatIndex]).ValidDestSpecs).GetNumberOfElements();

#if FAST_TEST
    uiNoOfSourceSpecs = min(uiNoOfSourceSpecs, 1);
    uiNoOfDestSpecs = min(uiNoOfDestSpecs, 1);
#endif

	uiNoOfRectPairs = (IsWHQL())? cuiNoOfWHQLRectPairs: (cuiNoOfSourceRects * cuiNoOfDestRects);

	uiNoOfTestCases =
	uiNoOfSourceSpecs *
	uiNoOfDestSpecs *
	uiNoOfRectPairs *
	uiNoOfValidFilterRecordIndices;

	//WriteToLog("In RegisterTestableFormatPair: uiNoOfTestCases == %d\n", uiNoOfTestCases);

	// Record the source/destination format pair and the associated number of test cases.
	pNewFormatPairElement = TestCasesByFormatPair.AddNewElement();
	pNewFormatPairElement -> uiSourceFormatIndex = uiSourceFormatIndex;
	pNewFormatPairElement -> uiDestFormatIndex = uiDestFormatIndex;
	pNewFormatPairElement -> uiNoOfTestCases = uiNoOfTestCases;

	return uiNoOfTestCases;
}

UINT CStretchRectTestGeneral::EnumerateTestCases()
{
	HRESULT hresult;
	UINT uiTotalTestCases = 0;

    FindValidSurfaceSpecsPerFormat();

	//WriteToLog("In EnumerateTestCases\n");

	// For each valid source/destination format pair, compute and record the number of test cases.
	// Pair a small set of potential source formats with all potential destination formats.
	{
		UINT uiSourceFormatSubsetIndex;

		for (uiSourceFormatSubsetIndex = 0; uiSourceFormatSubsetIndex < cuiNoOfSourceFormats; ++uiSourceFormatSubsetIndex)
		{
			UINT uiSourceFormatIndex;

			if (FindFormatRecordIndex(FormatSubsetSource[uiSourceFormatSubsetIndex], &uiSourceFormatIndex))
			{
				UINT uiNoOfSourceSpecs;

				if (uiNoOfSourceSpecs = ((ValidSurfacesByFormat[uiSourceFormatIndex]).ValidSourceSpecs).GetNumberOfElements())
				{
					FORMAT fmtSource((DXGraphicsFormats[uiSourceFormatIndex]).fmtFW);
					UINT uiDestFormatIndex;

					for (uiDestFormatIndex = 0; uiDestFormatIndex < cuiNoOfFormats; ++uiDestFormatIndex)
					{
						UINT uiNoOfDestSpecs;

						//WriteToLog("uiDestFormatIndex = %d\n", uiDestFormatIndex);

						if (uiNoOfDestSpecs = ((ValidSurfacesByFormat[uiDestFormatIndex]).ValidDestSpecs).GetNumberOfElements())
						{
							//WriteToLog("Found %d specs for dest index %d\n", uiNoOfDestSpecs, uiDestFormatIndex);

							FORMAT fmtDest((DXGraphicsFormats[uiDestFormatIndex]).fmtFW);

							if ((uiSourceFormatIndex == uiDestFormatIndex) || (SUCCEEDED((hresult = m_pD3D -> CheckDeviceFormatConversion(uiSrcDeviceAdapterOrdinal, dwSrcDeviceType, fmtSource, fmtDest)))))
								uiTotalTestCases += RegisterTestableFormatPair(uiSourceFormatIndex, uiDestFormatIndex);
						}
					}
				}
			}
		}
	}

	// For each valid source/destination format pair, compute and record the number of test cases.
	// Pair a small set of potential destination formats with all potential source formats.
	{
		UINT uiSourceFormatIndex;

		for (uiSourceFormatIndex = 0; uiSourceFormatIndex < cuiNoOfFormats; ++uiSourceFormatIndex)
		{
			UINT uiNoOfSourceSpecs;

			//WriteToLog("uiSourceFormatIndex = %d\n", uiSourceFormatIndex);

			if (uiNoOfSourceSpecs = ((ValidSurfacesByFormat[uiSourceFormatIndex]).ValidSourceSpecs).GetNumberOfElements())
			{
				//WriteToLog("Found %d specs for source index %d\n", uiNoOfSourceSpecs, uiSourceFormatIndex);

				FORMAT fmtSource((DXGraphicsFormats[uiSourceFormatIndex]).fmtFW);
				UINT uiDestFormatSubsetIndex;

				for (uiDestFormatSubsetIndex = 0; uiDestFormatSubsetIndex < cuiNoOfDestFormats; ++uiDestFormatSubsetIndex)
				{
					UINT uiDestFormatIndex;

					if (FindFormatRecordIndex(FormatSubsetDest[uiDestFormatSubsetIndex], &uiDestFormatIndex))
					{
						UINT uiNoOfDestSpecs;

						//WriteToLog("uiDestFormatIndex = %d\n", uiDestFormatIndex);

						if (uiNoOfDestSpecs = ((ValidSurfacesByFormat[uiDestFormatIndex]).ValidDestSpecs).GetNumberOfElements())
						{
							//WriteToLog("Found %d specs for dest index %d\n", uiNoOfDestSpecs, uiDestFormatIndex);

							FORMAT fmtDest((DXGraphicsFormats[uiDestFormatIndex]).fmtFW);

							if ((uiSourceFormatIndex == uiDestFormatIndex) || (SUCCEEDED((hresult = m_pD3D -> CheckDeviceFormatConversion(uiSrcDeviceAdapterOrdinal, dwSrcDeviceType, fmtSource, fmtDest)))))
								uiTotalTestCases += RegisterTestableFormatPair(uiSourceFormatIndex, uiDestFormatIndex);
						}
					}
				}
			}
		}
	}

	return uiTotalTestCases;
}

bool CStretchRectTestGeneral::ExecuteTest(UINT uiTestCaseId)
{
	UINT uiFormatPairSpecificId = uiTestCaseId - 1;
	clsFormatPairListScanner FormatPairListScanner;
	FormatPair *pCurrentPair;
	UINT
	uiNoOfTestCases, uiNoOfSourceSurfaceSpecs, uiNoOfDestSurfaceSpecs,
	uiSourceSpecIndex, uiDestSpecIndex,

	uiSourceRectIndex, uiDestRectIndex,
	uiWHQLRectPairIndex,
	/*
	uiSourceLeftIndex, uiSourceTopIndex, uiSourceWidthIndex, uiSourceHeightIndex,
	uiDestLeftIndex, uiDestTopIndex, uiDestWidthIndex, uiDestHeightIndex,
	uiSourceRectWidth, uiSourceRectHeight,
	*/

	uiFilterIndex;
	RECT rectSource, rectDest;
	clsSurfaceSpecArray *pSourceSurfaceSet, *pDestSurfaceSet;
	DWORD dwSourceSurfSpec, dwDestSurfSpec;
	float fSurfaceMinimumConformanceFraction;

	// Determine the source-format/destination-format pair and the offset of the test case
	// within the set of test cases associated with the pair.
	FormatPairListScanner.Reset(&TestCasesByFormatPair);
	if (!(pCurrentPair = FormatPairListScanner.GetCurrent()))
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}
	uiNoOfTestCases = pCurrentPair -> uiNoOfTestCases;
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

			uiNoOfTestCases = pCurrentPair -> uiNoOfTestCases;
		}
		while (uiFormatPairSpecificId >= uiNoOfTestCases);

	// Based on the source and destination formats identified by *pCurrentPair, determine the
	// sets of source and destination specifications.
	pSourceSurfaceSet = &((ValidSurfacesByFormat[pCurrentPair -> uiSourceFormatIndex]).ValidSourceSpecs);
	uiNoOfSourceSurfaceSpecs = pSourceSurfaceSet -> GetNumberOfElements();
	pDestSurfaceSet = &((ValidSurfacesByFormat[pCurrentPair -> uiDestFormatIndex]).ValidDestSpecs);
	uiNoOfDestSurfaceSpecs = pDestSurfaceSet -> GetNumberOfElements();


	{
		UINT uiNoOfRectPairs = (IsWHQL())? cuiNoOfWHQLRectPairs: (cuiNoOfSourceRects * cuiNoOfDestRects);

		UINT uiTestCasesPerSourceDest = uiNoOfValidFilterRecordIndices * uiNoOfRectPairs;
		UINT uiTestCasesPerSource = uiTestCasesPerSourceDest * uiNoOfDestSurfaceSpecs;

		// Compute the upper bound of the range of test cases to skip in the event that it is
		// necessary to skip to the next destination surface type.

		uiSkipDestinationTarget = uiTestCaseId + uiTestCasesPerSourceDest -
		(uiFormatPairSpecificId % uiTestCasesPerSourceDest) - 1;

		// Compute the upper bound of the range of test cases to skip in the event that it is
		// necessary to skip to the next source surface type.

		uiSkipSourceTarget = uiTestCaseId + uiTestCasesPerSource -
		(uiFormatPairSpecificId % uiTestCasesPerSource) - 1;
	}

	// Derive the test case attributes---each an array index---from the test case descriptor.

	uiFilterIndex = uiFormatPairSpecificId % uiNoOfValidFilterRecordIndices;
	uiFormatPairSpecificId /= uiNoOfValidFilterRecordIndices;

	if (IsWHQL())
	{
		uiWHQLRectPairIndex = uiFormatPairSpecificId % cuiNoOfWHQLRectPairs;
		uiFormatPairSpecificId /= cuiNoOfWHQLRectPairs;
	}
	else
	{
		uiDestRectIndex = uiFormatPairSpecificId % cuiNoOfDestRects;
		uiFormatPairSpecificId /= cuiNoOfDestRects;
		uiSourceRectIndex = uiFormatPairSpecificId % cuiNoOfSourceRects;
		uiFormatPairSpecificId /= cuiNoOfSourceRects;
	}

	uiDestSpecIndex = uiFormatPairSpecificId % uiNoOfDestSurfaceSpecs;

	if ((uiSourceSpecIndex = uiFormatPairSpecificId / uiNoOfDestSurfaceSpecs) >= uiNoOfSourceSurfaceSpecs)
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}

	// Record commencement of a test case.
	BeginTestCase("Test Case", uiTestCaseId);

	// Prepare the source and destination surface sets.

	dwSourceSurfSpec = pSourceSurfaceSet -> GetElementValue(uiSourceSpecIndex);
	dwDestSurfSpec = pDestSurfaceSet -> GetElementValue(uiDestSpecIndex);
  
    WriteToLog("Surface Type %d (%dX%d) Format %s Pool %d Usage %d MS %d ",ExtractBitsRange(dwSourceSurfSpec, 28, 31), SurfaceDimensions[(UINT)ExtractBitsRange(dwSourceSurfSpec, 24, 27)].uiWidth, SurfaceDimensions[(UINT)ExtractBitsRange(dwSourceSurfSpec, 24, 27)].uiHeight, DXGraphicsFormats[(UINT)ExtractBitsRange(dwSourceSurfSpec, 16, 23)].szName, DXPoolTypes[(UINT)ExtractBitsRange(dwSourceSurfSpec, 8, 15)].dwFWPool, dwUsageSets[(UINT)ExtractBitsRange(dwSourceSurfSpec, 0, 7)], DXMultiSampleTypes[(UINT)ExtractBitsRange(dwSourceSurfSpec, 8, 15)].mstFW);
    WriteToLog("Dest    Type %d (%dX%d) Format %s Pool %d Usage %d MS %d ",ExtractBitsRange(dwDestSurfSpec, 28, 31), SurfaceDimensions[(UINT)ExtractBitsRange(dwDestSurfSpec, 24, 27)].uiWidth, SurfaceDimensions[(UINT)ExtractBitsRange(dwDestSurfSpec, 24, 27)].uiHeight,       DXGraphicsFormats[(UINT)ExtractBitsRange(dwDestSurfSpec, 16, 23)].szName,   DXPoolTypes[(UINT)ExtractBitsRange(dwDestSurfSpec, 8, 15)].dwFWPool,   dwUsageSets[(UINT)ExtractBitsRange(dwDestSurfSpec, 0, 7)], DXMultiSampleTypes[(UINT)ExtractBitsRange(dwDestSurfSpec, 8, 15)].mstFW);



	// If the source surface is a "back buffer N surface", commence test case execution
	// by initializing back buffer 0 of the device swap chain and then calling Present.
	// These steps effectively initialize the back buffer N surface.
	if ((UINT)ExtractBitsRange(dwSourceSurfSpec, 28, 31) == 6)
	{
		CSurface *pOSPSurface = NULL, *pBBZero = NULL;

		pOSPSurface = opsBBZeroInitializer.GetSurfacePointer();
		m_pSrcDevice -> GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &pBBZero);
		m_pSrcDevice -> UpdateSurface(pOSPSurface, NULL, pBBZero, NULL);
		pBBZero -> Release();
		m_pSrcDevice -> Present(NULL, NULL, NULL, NULL);
	}
	
    if (!SourceSurfaceSet.PrepareFromDWORD(this, dwSourceSurfSpec))
	{
		//WriteToLog(_T("The test was unable to prepare the set of source surfaces.\n"));
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiSkipSourceTarget);
		return false;
	}

    if (!DestSurfaceSet.PrepareFromDWORD(this, dwDestSurfSpec))
	{
		//WriteToLog(_T("The test was unable to prepare the set of destination surfaces.\n"));
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiSkipDestinationTarget);
		return false;
	}

	// Compute the source and destination rectangles for the StretchRect operation.

	if (IsWHQL())
	{
		// Compute the source rectangle.

		rectSource.left = (SourceSurfaceSet.sdSourceSurfaceDesc).dwWidth *
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iLeftNumerator /
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iLeftDenominator +
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iLeftConstant;

		rectSource.top = (SourceSurfaceSet.sdSourceSurfaceDesc).dwHeight *
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iTopNumerator /
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iTopDenominator +
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iTopConstant;

		rectSource.right = (SourceSurfaceSet.sdSourceSurfaceDesc).dwWidth *
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iRightNumerator /
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iRightDenominator +
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iRightConstant;

		rectSource.bottom = (SourceSurfaceSet.sdSourceSurfaceDesc).dwHeight *
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iBottomNumerator /
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iBottomDenominator +
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).SourceRect).iBottomConstant;

		// Compute the destination rectangle.

		rectDest.left = (DestSurfaceSet.sdDestSurfaceDesc).dwWidth *
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iLeftNumerator /
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iLeftDenominator +
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iLeftConstant;

		rectDest.top = (DestSurfaceSet.sdDestSurfaceDesc).dwHeight *
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iTopNumerator /
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iTopDenominator +
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iTopConstant;

		rectDest.right = (DestSurfaceSet.sdDestSurfaceDesc).dwWidth *
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iRightNumerator /
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iRightDenominator +
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iRightConstant;

		rectDest.bottom = (DestSurfaceSet.sdDestSurfaceDesc).dwHeight *
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iBottomNumerator /
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iBottomDenominator +
		((WHQL_Rect_Pairs[uiWHQLRectPairIndex]).DestRect).iBottomConstant;
	}
	else
	{
		// Compute the source rectangle.

		rectSource.left = (SourceSurfaceSet.sdSourceSurfaceDesc).dwWidth *
		(SourceRects[uiSourceRectIndex]).iLeftNumerator /
		(SourceRects[uiSourceRectIndex]).iLeftDenominator +
		(SourceRects[uiSourceRectIndex]).iLeftConstant;

		rectSource.top = (SourceSurfaceSet.sdSourceSurfaceDesc).dwHeight *
		(SourceRects[uiSourceRectIndex]).iTopNumerator /
		(SourceRects[uiSourceRectIndex]).iTopDenominator +
		(SourceRects[uiSourceRectIndex]).iTopConstant;

		rectSource.right = (SourceSurfaceSet.sdSourceSurfaceDesc).dwWidth *
		(SourceRects[uiSourceRectIndex]).iRightNumerator /
		(SourceRects[uiSourceRectIndex]).iRightDenominator +
		(SourceRects[uiSourceRectIndex]).iRightConstant;

		rectSource.bottom = (SourceSurfaceSet.sdSourceSurfaceDesc).dwHeight *
		(SourceRects[uiSourceRectIndex]).iBottomNumerator /
		(SourceRects[uiSourceRectIndex]).iBottomDenominator +
		(SourceRects[uiSourceRectIndex]).iBottomConstant;

		// Compute the destination rectangle.

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
	}

	// If necessary, adjust the source rectangle in accordance with DXTn/UYVY constraints.
	if (TestDXTn((SourceSurfaceSet.sdSourceSurfaceDesc).Format))
		AdjustRectForDXTn(&rectSource, (SourceSurfaceSet.sdSourceSurfaceDesc).dwWidth,
		(SourceSurfaceSet.sdSourceSurfaceDesc).dwHeight);
	else
		if (TestUYVY((SourceSurfaceSet.sdSourceSurfaceDesc).Format))
			AdjustRectForUYVY(&rectSource, (SourceSurfaceSet.sdSourceSurfaceDesc).dwWidth,
			(SourceSurfaceSet.sdSourceSurfaceDesc).dwHeight);

	// If necessary, adjust the destination rectangle in accordance with DXTn/UYVY constraints.
	if (TestDXTn((DestSurfaceSet.sdDestSurfaceDesc).Format))
		AdjustRectForDXTn(&rectDest, (DestSurfaceSet.sdDestSurfaceDesc).dwWidth,
		(DestSurfaceSet.sdDestSurfaceDesc).dwHeight);
	else
		if (TestUYVY((DestSurfaceSet.sdDestSurfaceDesc).Format))
			AdjustRectForUYVY(&rectDest, (DestSurfaceSet.sdDestSurfaceDesc).dwWidth,
			(DestSurfaceSet.sdDestSurfaceDesc).dwHeight);

	// Compute the fractional conformance expected of the entire destination surface.
	fSurfaceMinimumConformanceFraction =
	1.0f -
	(
		(1.0f - fMinimumConformanceFraction) *
		(float)((rectDest.right - rectDest.left) * (rectDest.bottom - rectDest.top)) /
		(float)(DestSurfaceSet.GetNoOfSurfacePixels())
	);

	if (!TestStretchRectOperation(&rectSource, &rectDest, ValidFilterRecordIndices[uiFilterIndex], fSurfaceMinimumConformanceFraction))
	{
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiSkipDestinationTarget);
		return false;
	}

	//Fail(); // Test maximum error logging

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// CStretchRectTestMainNonMS

CStretchRectTestMainNonMS::CStretchRectTestMainNonMS()
{
}

CStretchRectTestMainNonMS::~CStretchRectTestMainNonMS()
{
}

void CStretchRectTestMainNonMS::FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray)
{
	clsSurfaceSpecList SourceSpecList;

	// Build the list.

	if ((SrcDeviceCaps.DevCaps2) & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES)
	// For the purposes of StretchRect, the underlying driver is "DX9 or later".
	{
		FindValidOffscreenPlainSpecs(uiFormatIndex, &SourceSpecList);
		FindValidRTSpecs(uiFormatIndex, &SourceSpecList);
		FindValidTextureSpecs(uiFormatIndex, CandidateUsagesDX9Source, cuiNoOfCandidateUsagesDX9Source, 0, &SourceSpecList);
		FindValidCubeTextureSpecs(uiFormatIndex, CandidateUsagesDX9Source, cuiNoOfCandidateUsagesDX9Source, 0, &SourceSpecList);
		// '0' indexes 'D3DPOOL_DEFAULT' in the array of pool types.

		FindValidBBNSpecs(uiFormatIndex, &SourceSpecList);
	}
	else
	// For the purposes of StretchRect, the underlying driver is "DX8 or earlier".
	{
		FindValidOffscreenPlainSpecs(uiFormatIndex, &SourceSpecList);
		FindValidRTSpecs(uiFormatIndex, &SourceSpecList);

		//FindValidBBNSpecs(uiFormatIndex, &SourceSpecList);
	}

	// Initialize the array.
	SourceSpecList.InitializeSurfaceSpecArray(pSourceSpecArray);
}

void CStretchRectTestMainNonMS::FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray)
{
	clsSurfaceSpecList DestSpecList;

	// Build the list.

	if ((SrcDeviceCaps.DevCaps2) & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES)
	// For the purposes of StretchRect, the underlying driver is "DX9 or later".
	{
        FindValidRTSpecs(uiFormatIndex, &DestSpecList);
		FindValidTextureSpecs(uiFormatIndex, CandidateUsagesDX9Dest, cuiNoOfCandidateUsagesDX9Dest, 0, &DestSpecList);
		FindValidCubeTextureSpecs(uiFormatIndex, CandidateUsagesDX9Dest, cuiNoOfCandidateUsagesDX9Dest, 0, &DestSpecList);
		FindValidBBZeroSpecs(uiFormatIndex, &DestSpecList);
	}
	else
	// For the purposes of StretchRect, the underlying driver is "DX8 or earlier".
	{
		FindValidRTSpecs(uiFormatIndex, &DestSpecList);
		FindValidTextureSpecs(uiFormatIndex, CandidateUsagesDX8Dest, cuiNoOfCandidateUsagesDX8Dest, 0, &DestSpecList);
		FindValidCubeTextureSpecs(uiFormatIndex, CandidateUsagesDX8Dest, cuiNoOfCandidateUsagesDX8Dest, 0, &DestSpecList);
		FindValidBBZeroSpecs(uiFormatIndex, &DestSpecList);
	}

	// Initialize the array.
	DestSpecList.InitializeSurfaceSpecArray(pDestSpecArray);
}

////////////////////////////////////////////////////////////////////////////////
// CStretchRectTestMainMS

CStretchRectTestMainMS::CStretchRectTestMainMS()
{
	m_szCommandKey= _T("StretchRectTestMS");
	m_szTestName= _T("StretchRectTestMS");
}

CStretchRectTestMainMS::~CStretchRectTestMainMS()
{
}

void CStretchRectTestMainMS::FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray)
{
	clsSurfaceSpecList SourceSpecList;

	// Build the list.

	if ((SrcDeviceCaps.DevCaps2) & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES)
	// For the purposes of StretchRect, the underlying driver is "DX9 or later".
	{
		FindValidOffscreenPlainSpecs(uiFormatIndex, &SourceSpecList);
		FindValidRTSpecs(uiFormatIndex, &SourceSpecList);
		FindValidTextureSpecs(uiFormatIndex, CandidateUsagesDX9Source, cuiNoOfCandidateUsagesDX9Source, 0, &SourceSpecList);
		FindValidCubeTextureSpecs(uiFormatIndex, CandidateUsagesDX9Source, cuiNoOfCandidateUsagesDX9Source, 0, &SourceSpecList);
		// '0' indexes 'D3DPOOL_DEFAULT' in the array of pool types.
	}
	else
	// For the purposes of StretchRect, the underlying driver is "DX8 or earlier".
	{
		// Uncomment the following statement for DX9.1!
		// A DX9.0a runtime bug prevents multisampled StretchRect operations on pre-DDI9 drivers.
		// Ensuring that no source surface specifications are entered into the array when running
		// against a pre-DDI9 driver forces the test group to be skipped.

		FindValidRTSpecs(uiFormatIndex, &SourceSpecList);
	}

	// Initialize the array.
	SourceSpecList.InitializeSurfaceSpecArray(pSourceSpecArray);
}

void CStretchRectTestMainMS::FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray)
{
	clsSurfaceSpecList DestSpecList;

	// Build the list.

	if ((SrcDeviceCaps.DevCaps2) & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES)
	// For the purposes of StretchRect, the underlying driver is "DX9 or later".
	{
		//FindValidRTSpecs(uiFormatIndex, &DestSpecList);
		FindValidTextureSpecs(uiFormatIndex, MSUsagesDest, cuiNoOfMSUsagesDest, 0, &DestSpecList);
		//FindValidCubeTextureSpecs(uiFormatIndex, CandidateUsagesDX9Dest, cuiNoOfCandidateUsagesDX9Dest, 0, &DestSpecList);
	}
	else
	// For the purposes of StretchRect, the underlying driver is "DX8 or earlier".
	{
		//FindValidRTSpecs(uiFormatIndex, &DestSpecList);
		FindValidTextureSpecs(uiFormatIndex, MSUsagesDest, cuiNoOfMSUsagesDest, 0, &DestSpecList);
		//FindValidCubeTextureSpecs(uiFormatIndex, CandidateUsagesDX8Dest, cuiNoOfCandidateUsagesDX8Dest, 0, &DestSpecList);
	}

	// Initialize the array.
	DestSpecList.InitializeSurfaceSpecArray(pDestSpecArray);
}




////////////////////////////////////////////////////////////////////////////////
// CStretchRectTestMainLDDM

CStretchRectTestMainLDDM::CStretchRectTestMainLDDM()
{
	m_szCommandKey= _T("StretchRectTestLDDM");
	m_szTestName= _T("StretchRectTestLDDM");
    m_Options.D3DOptions.fMinDXVersion = 9.0999f;
    m_Options.D3DOptions.fMaxDXVersion = 9.1f;  // Only run on DX9L
}


CStretchRectTestMainLDDM::~CStretchRectTestMainLDDM()
{
}

bool CStretchRectTestMainLDDM::VerifyDeviceCompatibility()
{
	GatherDeviceInfo();

	switch (dwSrcDeviceType)
	{
	case DEVICETYPE_HAL:
		if ((SrcDeviceCaps.Caps2) & D3DCAPS2_CANSHARERESOURCE)
			return true;
		break;
	case DEVICETYPE_REF:
		if ((SrcDeviceCaps.TextureFilterCaps) & D3DPTFILTERCAPS_CONVOLUTIONMONO)
			return true;
		break;
	default:
		break;
	}

	return false;
}

void CStretchRectTestMainLDDM::FindValidSourceSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pSourceSpecArray)
{
	clsSurfaceSpecList SourceSpecList;

	// Build the list.
	{
		FindValidOffscreenPlainSpecs(uiFormatIndex, &SourceSpecList);
		FindValidRTSpecs(uiFormatIndex, &SourceSpecList);
		FindValidTextureSpecs(uiFormatIndex, CandidateUsagesLDDMSource, cuiNoOfCandidateUsagesLDDMSource, 0, &SourceSpecList);
		FindValidCubeTextureSpecs(uiFormatIndex, CandidateUsagesLDDMSource, cuiNoOfCandidateUsagesLDDMSource, 0, &SourceSpecList);
		// '0' indexes 'D3DPOOL_DEFAULT' in the array of pool types.
	}

	// Initialize the array.
	SourceSpecList.InitializeSurfaceSpecArray(pSourceSpecArray);
}

void CStretchRectTestMainLDDM::FindValidDestSurfaceSpecs(UINT uiFormatIndex, clsSurfaceSpecArray *pDestSpecArray)
{
	clsSurfaceSpecList DestSpecList;

	// Build the list.
	{
		FindValidOffscreenPlainSpecs(uiFormatIndex, &DestSpecList);
		FindValidRTSpecs(uiFormatIndex, &DestSpecList);
		FindValidTextureSpecs(uiFormatIndex, CandidateUsagesLDDMDest, cuiNoOfCandidateUsagesLDDMDest, 0, &DestSpecList);
		FindValidCubeTextureSpecs(uiFormatIndex, CandidateUsagesLDDMDest, cuiNoOfCandidateUsagesLDDMDest, 0, &DestSpecList);
		// '0' indexes 'D3DPOOL_DEFAULT' in the array of pool types.
	}

	// Initialize the array.
	DestSpecList.InitializeSurfaceSpecArray(pDestSpecArray);
}

UINT CStretchRectTestMainLDDM::RegisterTestableFormatPair(UINT uiSourceFormatIndex, UINT uiDestFormatIndex)
{
	UINT uiNoOfSourceSpecs, uiNoOfDestSpecs, uiNoOfTestCases;
	FormatPair *pNewFormatPairElement;

	uiNoOfSourceSpecs = ((ValidSurfacesByFormat[uiSourceFormatIndex]).ValidSourceSpecs).GetNumberOfElements();
	uiNoOfDestSpecs = ((ValidSurfacesByFormat[uiDestFormatIndex]).ValidDestSpecs).GetNumberOfElements();

	uiNoOfTestCases =
	uiNoOfSourceSpecs *
	uiNoOfDestSpecs;

	// Record the source/destination format pair and the associated number of test cases.
	pNewFormatPairElement = TestCasesByFormatPair.AddNewElement();
	pNewFormatPairElement -> uiSourceFormatIndex = uiSourceFormatIndex;
	pNewFormatPairElement -> uiDestFormatIndex = uiDestFormatIndex;
	pNewFormatPairElement -> uiNoOfTestCases = uiNoOfTestCases;

	return uiNoOfTestCases;
}

UINT CStretchRectTestMainLDDM::EnumerateTestCases()
{
	HRESULT hresult;
	UINT uiTotalTestCases = 0;

    FindValidSurfaceSpecsPerFormat();

	// For each valid source/destination format pair, compute and record the number of test cases.
	{
		UINT uiFormatIndex;

		for (uiFormatIndex = 0; uiFormatIndex < cuiNoOfFormats; ++uiFormatIndex)
		{
			UINT uiNoOfSourceSpecs, uiNoOfDestSpecs;

			if
			(
				(uiNoOfSourceSpecs = ((ValidSurfacesByFormat[uiFormatIndex]).ValidSourceSpecs).GetNumberOfElements())
				&&
				(uiNoOfDestSpecs = ((ValidSurfacesByFormat[uiFormatIndex]).ValidDestSpecs).GetNumberOfElements())
			)
			{
				//if (SUCCEEDED((hresult = m_pD3D -> CheckDeviceFormatConversion(uiSrcDeviceAdapterOrdinal, dwSrcDeviceType, fmtSource, fmtDest))))
				uiTotalTestCases += RegisterTestableFormatPair(uiFormatIndex, uiFormatIndex);
			}
		}
	}

	return uiTotalTestCases;
}

bool CStretchRectTestMainLDDM::ExecuteTest(UINT uiTestCaseId)
{
	UINT uiFormatPairSpecificId = uiTestCaseId - 1;
	clsFormatPairListScanner FormatPairListScanner;
	FormatPair *pCurrentPair;
	UINT
	uiNoOfTestCases, uiNoOfSourceSurfaceSpecs, uiNoOfDestSurfaceSpecs,
	uiSourceSpecIndex, uiDestSpecIndex;

	clsSurfaceSpecArray *pSourceSurfaceSet, *pDestSurfaceSet;
	float fSurfaceMinimumConformanceFraction;

	// Determine the source-format/destination-format pair and the offset of the test case
	// within the set of test cases associated with the pair.
	FormatPairListScanner.Reset(&TestCasesByFormatPair);
	if (!(pCurrentPair = FormatPairListScanner.GetCurrent()))
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}
	uiNoOfTestCases = pCurrentPair -> uiNoOfTestCases;
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

			uiNoOfTestCases = pCurrentPair -> uiNoOfTestCases;
		}
		while (uiFormatPairSpecificId >= uiNoOfTestCases);

	// Based on the source and destination formats identified by *pCurrentPair, determine the
	// sets of source and destination specifications.
	pSourceSurfaceSet = &((ValidSurfacesByFormat[pCurrentPair -> uiSourceFormatIndex]).ValidSourceSpecs);
	uiNoOfSourceSurfaceSpecs = pSourceSurfaceSet -> GetNumberOfElements();
	pDestSurfaceSet = &((ValidSurfacesByFormat[pCurrentPair -> uiDestFormatIndex]).ValidDestSpecs);
	uiNoOfDestSurfaceSpecs = pDestSurfaceSet -> GetNumberOfElements();


	{
		UINT uiTestCasesPerSourceDest = 1;
		UINT uiTestCasesPerSource = uiTestCasesPerSourceDest * uiNoOfDestSurfaceSpecs;

		// Compute the upper bound of the range of test cases to skip in the event that it is
		// necessary to skip to the next destination surface type.

		uiSkipDestinationTarget = uiTestCaseId + uiTestCasesPerSourceDest -
		(uiFormatPairSpecificId % uiTestCasesPerSourceDest) - 1;

		// Compute the upper bound of the range of test cases to skip in the event that it is
		// necessary to skip to the next source surface type.

		uiSkipSourceTarget = uiTestCaseId + uiTestCasesPerSource -
		(uiFormatPairSpecificId % uiTestCasesPerSource) - 1;
	}

	// Derive the test case attributes---each an array index---from the test case descriptor.

	uiDestSpecIndex = uiFormatPairSpecificId % uiNoOfDestSurfaceSpecs;

	if ((uiSourceSpecIndex = uiFormatPairSpecificId / uiNoOfDestSurfaceSpecs) >= uiNoOfSourceSurfaceSpecs)
	{
		ReportInvalidTCID(uiTestCaseId);
		return false;
	}

	// Record commencement of a test case.
	BeginTestCase("Test Case", uiTestCaseId);

	// Prepare the source and destination surface sets.
	
    if (!SourceSurfaceSet.PrepareFromDWORD(this, pSourceSurfaceSet -> GetElementValue(uiSourceSpecIndex)))
	{
		//WriteToLog(_T("The test was unable to prepare the set of source surfaces.\n"));
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiSkipSourceTarget);
		return false;
	}

    if (!DestSurfaceSet.PrepareFromDWORD(this, pDestSurfaceSet -> GetElementValue(uiDestSpecIndex)))
	{
		//WriteToLog(_T("The test was unable to prepare the set of destination surfaces.\n"));
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiSkipDestinationTarget);
		return false;
	}

	// Compute the fractional conformance expected of the entire destination surface.
	fSurfaceMinimumConformanceFraction = fMinimumConformanceFraction;

	if (!TestStretchRectOperation(NULL, NULL, 0 /*ValidFilterRecordIndices[uiFilterIndex]*/, fSurfaceMinimumConformanceFraction))
	{
		// Skip the test case.
		SkipTestRange(uiTestCaseId, uiSkipDestinationTarget);
		return false;
	}

	//Fail();

	return true;
}
